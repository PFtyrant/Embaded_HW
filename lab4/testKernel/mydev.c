#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
MODULE_LICENSE("GPL");

char asdf[8];

static ssize_t my_read(struct file * fp, char *buf, size_t count, loff_t *fpos) {
	printk("call read%d\n", count);
	copy_to_user(buf,asdf, 7);
	return count;
}

static ssize_t my_write(struct file * fp, const char *buf, size_t count, loff_t *fpos){
	printk("call write\n");
	copy_from_user(asdf, buf, 7);
	return count;
}

static int my_open(struct inode *inode, struct file *fp) {
	printk("call open\n");
	return 0;
}

struct file_operations my_fpos = {
	read: my_read,
	write: my_write,
	open: my_open
};

#define MAJOR_NUM 244
#define DEVICE_NAME "my_dev"


static int my_init(void) {
	printk("call init\n");
	if(register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fpos) < 0) {
		printk("Can not get major %d\n",MAJOR_NUM);
		return (-EBUSY);
	}
	printk("My device is started and the major is %d\n",MAJOR_NUM);
	return 0;
}

static void my_exit(void) {
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
	printk("call exit\n");
}

module_init(my_init);
module_exit(my_exit);
