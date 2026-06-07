#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual MIT/GPL");

static int __init testmod_init(void)
{
    printk(KERN_INFO "Hi there!\n");
    return 0;
}

static void __exit testmod_exit(void)
{
    printk(KERN_INFO "Exit!\n");
}

module_init(testmod_init);
module_exit(testmod_exit);