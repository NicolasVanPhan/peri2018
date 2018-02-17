#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

/* Adding some parameter */
static int nb_leds;
module_param(nb_leds, int, 0);
MODULE_PARM_DESC(nb_leds, "The number of leds used");

static int __init mon_module_init(void)
{
	printk(KERN_DEBUG "Hello World!\n");
	printk(KERN_DEBUG "nb_leds=%d\n", nb_leds);
	return 0;
}

static void __exit mon_module_cleanup(void)
{
	printk(KERN_DEBUG "Goodbye World!\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
