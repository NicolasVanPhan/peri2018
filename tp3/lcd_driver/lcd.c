#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <mach/platform.h>

#define RS 7
#define E  27
#define D4 22
#define D5 23
#define D6 24
#define D7 25

#define GPIO_INPUT	0
#define GPIO_OUTPUT	1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bryan and Nicolas, 2018");
MODULE_DESCRIPTION("A little driver that controls the LCD display");

/*******************************************************************************
 * LCD's Instructions ( source = doc )
 ******************************************************************************/

#define B_0000  0
#define B_0001  1
#define B_0010  2
#define B_0011  3
#define B_0100  4
#define B_0101  5
#define B_0110  6
#define B_0111  7
#define B_1000  8
#define B_1001  9
#define B_1010  a
#define B_1011  b
#define B_1100  c
#define B_1101  d
#define B_1110  e
#define B_1111  f

/* On veut definir une macro HEX()
 * qui prend en argument deux nombres de 4 bits exprimes en binaire 
 * representant resp. les 4 bits de poids fort et les 4 bits de poids faible d'un entier sur 8bits  
 * qui rend l'entier.
 * p. ex: HEX(0000,0010) rend 2
 * p. ex: HEX(0001,0000) rend 16
 *
 * QUESTION: expliquez comment ca marche ....
 */
#define __HEX(h,l) 0x##h##l
#define _HEX(h,l)  __HEX(h,l)
#define HEX(h,l)   _HEX(B_##h,B_##l)

/* QUESTION: que fait cette macro ?
 */
#define BIT(b,n)   (((n)>>(b))&1)

/* commands */
#define LCD_CLEARDISPLAY        HEX(0000,0001)
#define LCD_RETURNHOME          HEX(0000,0010)
#define LCD_ENTRYMODESET        HEX(0000,0100)
#define LCD_DISPLAYCONTROL      HEX(0000,1000)
#define LCD_CURSORSHIFT         HEX(0001,0000)
#define LCD_FUNCTIONSET         HEX(0010,0000)
#define LCD_SETCGRAMADDR        HEX(0100,0000)
#define LCD_SETDDRAMADDR        HEX(1000,0000)

/* flags for display entry mode : combine with LCD_ENTRYMODESET */
#define LCD_EM_RIGHT            HEX(0000,0000)
#define LCD_EM_LEFT             HEX(0000,0010)
#define LCD_EM_DISPLAYSHIFT     HEX(0000,0001)
#define LCD_EM_DISPLAYNOSHIFT   HEX(0000,0000)

/* flags for display on/off control : combine with LCD_DISPLAYCONTROL */
#define LCD_DC_DISPLAYON        HEX(0000,0100)
#define LCD_DC_DISPLAYOFF       HEX(0000,0000)
#define LCD_DC_CURSORON         HEX(0000,0010)
#define LCD_DC_CURSOROFF        HEX(0000,0000)
#define LCD_DC_BLINKON          HEX(0000,0001)
#define LCD_DC_BLINKOFF         HEX(0000,0000)

/* flags for display/cursor shift : combine with LCD_CURSORSHIFT */
#define LCD_CS_DISPLAYMOVE      HEX(0000,1000)
#define LCD_CS_CURSORMOVE       HEX(0000,0000)
#define LCD_CS_MOVERIGHT        HEX(0000,0100)
#define LCD_CS_MOVELEFT         HEX(0000,0000)

/* flags for function set : combine with LCD_FUNCTIONSET */
#define LCD_FS_8BITMODE         HEX(0001,0000)
#define LCD_FS_4BITMODE         HEX(0000,0000)
#define LCD_FS_2LINE            HEX(0000,1000)
#define LCD_FS_1LINE            HEX(0000,0000)
#define LCD_FS_5x10DOTS         HEX(0000,0100)
#define LCD_FS_5x8DOTS          HEX(0000,0000)


/* --------- Defining module parameters  ----------------------------------- */

static int major;

/* --------- Defining module globals  -------------------------------------- */
static char lcd_cur_message[20];

/* --------- Defining function prototypes ---------------------------------- */

static int	open_lcd(struct inode *inode, struct file *file);
static ssize_t 	read_lcd(struct file *file, char *buf, size_t count,
			   loff_t *ppos);
static ssize_t 	write_lcd(struct file *file, const char *buf, size_t count,
			    loff_t *ppos);
static int 	release_lcd(struct inode *inode, struct file *file);

static void	gpio_fsel(int pin, int fun);
static void	gpio_write(int pin, bool val);

void lcd_strobe(void);
void lcd_write4bits(int data);
void lcd_command(int cmd);
void lcd_data(int character);
void lcd_init(void);
void lcd_clear(void);
void lcd_message(char* txt);
void lcd_set_cursor(int x, int y);



/* ----------------------------------------------------------------------------
 * -------- Hardware interface configuration  ---------------------------------
 * ------------------------------------------------------------------------- */

/* Defining a structure for easily accessing GPIO ports */
struct gpio_s
{
	uint32_t gpfsel[7];
	uint32_t gpset[3];
	uint32_t gpclr[3];
	uint32_t gplev[3];
	uint32_t gpeds[3];
	uint32_t gpren[3];
	uint32_t gpfen[3];
	uint32_t gphen[3];
	uint32_t gplen[3];
	uint32_t gparen[3];
	uint32_t gpafen[3];
	uint32_t gppud[1];
	uint32_t gppudclk[3];
	uint32_t test[1];
}
*gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

/* Defining ports associated to the LEDs and switch */
static const int gpio_led[2] = {4, 17};
static const int gpio_btn[1] = {18};

/* ----------------------------------------------------------------------------
 * -------- User interface configuration  -------------------------------------
 * ------------------------------------------------------------------------- */

/* Init structure for saving driver in module */
struct file_operations fops_lcd =
{
	.open       = open_lcd,
	.read       = read_lcd,
	.write      = write_lcd,
	.release    = release_lcd 
};

/* ----------------------------------------------------------------------------
 * -------- Functions  --------------------------------------------------------
 * ------------------------------------------------------------------------- */

/* ---- Defining module necessary functions -------------------------------- */
static int
__init mon_module_init(void)
{
	int	i;

	/* Setting up LCD'S GPIO ports */
	gpio_fsel(RS, GPIO_OUTPUT);
	gpio_fsel(E , GPIO_OUTPUT);
	gpio_fsel(D4, GPIO_OUTPUT);
	gpio_fsel(D5, GPIO_OUTPUT);
	gpio_fsel(D6, GPIO_OUTPUT);
	gpio_fsel(D7, GPIO_OUTPUT);

	/* Initialize LCD */
	lcd_init();
	lcd_clear();
	for(i = 0; i < 20; i++)
		lcd_cur_message[i] = 0;

	/* Register the driver */
	major = register_chrdev(0, "lcd", &fops_lcd);
	printk(KERN_DEBUG "Load LCD driver\n");
	return 0;
}

static void
__exit mon_module_cleanup(void)
{
	unregister_chrdev(major, "lcd");
	printk(KERN_DEBUG "Unload LCD driver\n");
}

/* ---- Defining driver functions ------------------------------------------ */
static int 
open_lcd(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "open()\n");
	return 0;
}

static ssize_t 
read_lcd(struct file *file, char *buf, size_t count, loff_t *ppos) {
	int	nb_char_read;
	int	i;

	nb_char_read = count < 20 ? count : 20;
	for(i = 0; i < nb_char_read  && lcd_cur_message[i] != '\0'; i++)
		buf[i] = lcd_cur_message[i];
	return nb_char_read;
}

static ssize_t 
write_lcd(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	int	i;

	lcd_clear();
	lcd_set_cursor(1, 1);
	if (count > 0) {
		for(i = 0; i < 19 && buf[i] != '\0' && buf[i] != '\n'; i++)
			lcd_cur_message[i] = buf[i];
		for(; i < 19; i++)
			lcd_cur_message[i] = ' ';
		lcd_cur_message[i] = '\0';
		lcd_message(lcd_cur_message);
	}
	return count;
}

static int 
release_lcd(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "close()\n");
	return 0;
}


/* ---- Defining internal functions  --------------------------------------- */

/* -------------------- GPIO Operations -------------------- */
static void gpio_fsel(int pin, int fun)
{
	uint32_t reg = pin / 10;
	uint32_t bit = (pin % 10) * 3;
	uint32_t mask = 0b111 << bit;
	gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask)
		| ((fun << bit) & mask);
}

static void gpio_write(int pin, bool val)
{
	if (val)
		gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
	else
		gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}

/* -------------------- LCD Operations -------------------- */

/* generate E signal */
void lcd_strobe(void)
{
  gpio_write(E, 1);
  udelay(1);
  gpio_write(E, 0);
  udelay(1);
}

/* send 4bits to LCD */
void lcd_write4bits(int data)
{
  /* first 4 bits */
  gpio_write(D7, BIT(7,data));
  gpio_write(D6, BIT(6,data));
  gpio_write(D5, BIT(5,data));
  gpio_write(D4, BIT(4,data));
  lcd_strobe();

  /* second 4 bits */
  gpio_write(D7, BIT(3,data));
  gpio_write(D6, BIT(2,data));
  gpio_write(D5, BIT(1,data));
  gpio_write(D4, BIT(0,data));
  lcd_strobe();
  udelay(50); /* le délai minimum est de 37us */
}

void lcd_command(int cmd)
{
  gpio_write(RS, 0);
  lcd_write4bits(cmd);
  udelay(2000); /* delai nécessaire pour certaines commandes */
}

void lcd_data(int character)
{
  gpio_write(RS, 1);
  lcd_write4bits(character);
}

/* initialization 
 * Question : commenter ces initialisations:
 *
 * LCD_FUNCTIONSET    | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS 
 * LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSOROFF 
 * LCD_ENTRYMODESET   | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT 
 */
void lcd_init(void)
{
  gpio_write(E, 0);
  lcd_command(HEX(0011,0011)); /* initialization */
  lcd_command(HEX(0011,0010)); /* initialization */
  lcd_command(LCD_FUNCTIONSET    | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS);
  lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSORON );
  lcd_command(LCD_ENTRYMODESET   | LCD_EM_DISPLAYNOSHIFT | LCD_EM_LEFT );
}

void lcd_clear(void)
{
  lcd_command(LCD_CLEARDISPLAY);
  lcd_command(LCD_RETURNHOME);
}

void lcd_message(char* txt)
{
	int i;

	for (i = 0; i < 20 && i < strlen(txt); i++) {
		lcd_data(txt[i]);
	}
}

void lcd_set_cursor(int x, int y)
{
	int	i;
	int	line_len;
	int	real_y[4] = {1, 3, 2, 4};

	lcd_command(LCD_RETURNHOME);
	line_len = 20;
	for (i = 0; i < line_len * (real_y[y - 1] - 1) + (x - 1); i++)
		lcd_command(LCD_CURSORSHIFT | LCD_CS_CURSORMOVE | LCD_CS_MOVERIGHT);
}


module_init(mon_module_init);
module_exit(mon_module_cleanup);
