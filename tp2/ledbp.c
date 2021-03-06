#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/platform.h>

#define GPIO_INPUT	0
#define GPIO_OUTPUT	1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bryan and Nicolas, 2018");
MODULE_DESCRIPTION("A little driver that blinks LEDS till a button is pressed");

/* --------- Defining module parameters  ----------------------------------- */
static int nb_leds;
module_param(nb_leds, int, 0);
MODULE_PARM_DESC(nb_leds, "Number of leds");

static int btn_port;
module_param(btn_port, int, 0);
MODULE_PARM_DESC(led1_port, "Number of leds");

static int major;

/* --------- Defining function prototypes ---------------------------------- */

static int	open_ledbp(struct inode *inode, struct file *file);
static ssize_t 	read_ledbp(struct file *file, char *buf, size_t count,
			   loff_t *ppos);
static ssize_t 	write_ledbp(struct file *file, const char *buf, size_t count,
			    loff_t *ppos);
static int 	release_ledbp(struct inode *inode, struct file *file);

static void	gpio_fsel(int pin, int fun);
static void	gpio_write(int pin, bool val);
static int	gpio_read(int pin);


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
struct file_operations fops_ledbp =
{
	.open       = open_ledbp,
	.read       = read_ledbp,
	.write      = write_ledbp,
	.release    = release_ledbp 
};

/* ----------------------------------------------------------------------------
 * -------- Functions  --------------------------------------------------------
 * ------------------------------------------------------------------------- */

/* ---- Defining module necessary functions -------------------------------- */
static int
__init mon_module_init(void)
{
	major = register_chrdev(0, "ledbp", &fops_ledbp);
	printk(KERN_DEBUG "Hello World <Phan Aggoun> !\n");
	printk(KERN_DEBUG "nb leds=%d !!\n", nb_leds);
	return 0;
}
static void

__exit mon_module_cleanup(void)
{
	unregister_chrdev(major, "ledbp");
	printk(KERN_DEBUG "Goodbye World!\n");
}

/* ---- Defining driver functions ------------------------------------------ */
static int 
open_ledbp(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "open()\n");

	gpio_fsel(gpio_led[0], GPIO_OUTPUT);
	gpio_fsel(gpio_led[1], GPIO_OUTPUT);
	gpio_fsel(gpio_btn[0], GPIO_INPUT);
	return 0;
}

static ssize_t 
read_ledbp(struct file *file, char *buf, size_t count, loff_t *ppos) {
	int read_value;
	printk(KERN_DEBUG "read()\n");

	if (count > 0) {
		read_value = gpio_read(gpio_btn[0]);
		if (read_value == 1) {
			buf[0] = '1';
		}
		else if (read_value == 0) {
			buf[0] = '0';
		}
		else {
			buf[0] = '0';
			printk(KERN_DEBUG "GPIO read %d\n", read_value);
		}
	}
	return count;
}

static ssize_t 
write_ledbp(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	int i;

	printk(KERN_DEBUG "write()\n");
	if (count == 0)
		return count;
	for (i = 0; i < 2; i++)
	{
		printk(KERN_DEBUG "ledbp char%d: %c\n", i, buf[i]);
		if (buf[0] & (1 << i))	/* if bit n is 1, turn on LED n*/
			gpio_write(gpio_led[i], 1);
		else		  	/* Otherwise turn off LED n */
			gpio_write(gpio_led[i], 0);
	}
	return count;
}

static int 
release_ledbp(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "close()\n");
	return 0;
}


/* ---- Defining internal functions  --------------------------------------- */

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

static int gpio_read(int pin)
{
	return ((gpio_regs->gplev[pin / 32]) >> (pin % 32)) & 0x1;
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
