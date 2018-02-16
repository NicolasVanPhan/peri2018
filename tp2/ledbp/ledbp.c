#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bryan and Nicolas, 2018");
MODULE_DESCRIPTION("A little driver that blinks LEDS till a button is pressed");

/* Defining module parameters */
static int nb_leds;
module_param(nb_leds, int, 0);
MODULE_PARM_DESC(nb_leds, "Number of leds");

static int btn_port;
module_param(btn_port, int, 0);
MODULE_PARM_DESC(led1_port, "Number of leds");

static int major;

static int 
open_ledbp(struct inode *inode, struct file *file);
static ssize_t 
read_ledbp(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t 
write_ledbp(struct file *file, const char *buf, size_t count, loff_t *ppos);
static int 
release_ledbp(struct inode *inode, struct file *file);

/* Init structure for saving driver in module */
struct file_operations fops_ledbp =
{
	.open       = open_ledbp,
	.read       = read_ledbp,
	.write      = write_ledbp,
	.release    = release_ledbp 
};


static int
__init mon_module_init(void)
{
	major = register_chrdev(0, "ledbp", &fops_ledbp);
	printk(KERN_DEBUG "Hello World <votre nom> !\n");
	printk(KERN_DEBUG "nb leds=%d !!\n", nb_leds);
	return 0;
}

/* Defining module necessary functions */
static void
__exit mon_module_cleanup(void)
{
	unregister_chrdev(major, "ledbp");
	printk(KERN_DEBUG "Goodbye World!\n");
}

static int 
open_ledbp(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "open()\n");
	return 0;
}

/* Defining driver functions */
static ssize_t 
read_ledbp(struct file *file, char *buf, size_t count, loff_t *ppos) {
	printk(KERN_DEBUG "read()\n");
	return count;
}

static ssize_t 
write_ledbp(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	printk(KERN_DEBUG "write()\n");
	return count;
}

static int 
release_ledbp(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "close()\n");
	return 0;
}


module_init(mon_module_init);
module_exit(mon_module_cleanup);
