#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander Kuleshin");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Proc module for Linux Kernel course");

#define MAX_BUFFER_SIZE 256
#define PROC_FILE_NAME "my_module"

static struct proc_dir_entry* proc_entry;
static char message[MAX_BUFFER_SIZE];

static ssize_t my_read(struct file* file, char __user* buf, size_t count, loff_t* pos)
{
	size_t len = strlen(message);

    	if (len == 0) {
        	len = snprintf(message, MAX_BUFFER_SIZE, "Hello from kernel\n");
    	}

    	if (*pos >= len) return 0;

    	if (copy_to_user(buf, message, len)) return -EFAULT;

    	*pos += len;
    	return len;
}

static ssize_t my_write(struct file* file, const char __user* buf, size_t count, loff_t* pos)
{
	size_t len = min(count, (size_t)(MAX_BUFFER_SIZE - 1));
	memset(message, 0, MAX_BUFFER_SIZE);

	if (copy_from_user(message, buf, len)) return -EFAULT;
	

	message[len] = '\0';
	
	printk(KERN_INFO "Received from user: %s\n", message);
	return len;
}

static const struct proc_ops fops = {
	.proc_read = my_read,
	.proc_write = my_write,
};

static int __init my_init(void)
{
	proc_entry = proc_create(PROC_FILE_NAME, 0666, NULL, &fops);
	if (!proc_entry) return -ENOMEM;

	printk(KERN_INFO "Module loaded succesfully, /proc/%s created\n", PROC_FILE_NAME);
	return 0;
}

static void __exit my_exit(void)
{
	proc_remove(proc_entry);
	printk(KERN_INFO "Module unloaded, /proc/%s removed\n", PROC_FILE_NAME);
}

module_init(my_init);
module_exit(my_exit);
