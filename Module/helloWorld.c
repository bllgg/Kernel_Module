#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

int var=0;

module_param(var,int,S_IRUSR|S_IWUSR);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HELLO WORLD MODULE");
MODULE_VERSION("1.0");


void display(void){
	printk(KERN_INFO "Input : %d",var); 
}
static int __init hello_init(void){
	printk(KERN_INFO "Hello World\n");
	display();
	return 0;
}

static void __exit hello_exit(void){
	printk(KERN_INFO "Goodbye World\n");
}

module_init(hello_init);
module_exit(hello_exit);