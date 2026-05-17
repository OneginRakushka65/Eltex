#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEVICE_NAME "chardev"
#define BUF_LEN 80

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander Kuleshin");
MODULE_DESCRIPTION("Character device module");

static int major;
static struct class* cls;
static char msg[BUF_LEN];
static int msg_len = 0;

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static int device_open(struct inode *inode, struct file *file)
{
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    try_module_get(THIS_MODULE);
    printk(KERN_INFO "chardev: device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    atomic_set(&already_open, CDEV_NOT_USED);
    module_put(THIS_MODULE);
    printk(KERN_INFO "chardev: device closed\n");
    return 0;
}

static ssize_t device_read(struct file *filp, char __user *buffer,
                           size_t length, loff_t *offset)
{
    int bytes_read = 0;

    if (*offset >= msg_len)
        return 0;

    while (length && (*offset < msg_len)) {
        put_user(msg[*offset], buffer++);
        length--;
        bytes_read++;
        (*offset)++;
    }

    printk(KERN_INFO "chardev: sent %d bytes\n", bytes_read);
    return bytes_read;
}

static ssize_t device_write(struct file *filp, const char __user *buffer, size_t length, loff_t *offset)
{
    int i;
    memset(msg, 0, BUF_LEN);

    msg_len = length < BUF_LEN ? length : BUF_LEN - 1;
    for (i = 0; i < msg_len; i++)
        get_user(msg[i], buffer + i);

    printk(KERN_INFO "chardev: received %d bytes: %s\n", msg_len, msg);
    return msg_len;
}

static struct file_operations chardev_fops = {
    .owner   = THIS_MODULE,
    .open    = device_open,
    .release = device_release,
    .read    = device_read,
    .write   = device_write,
};

static int __init chardev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register device: %d\n", major);
        return major;
    }

    cls = class_create(DEVICE_NAME);

    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    printk(KERN_INFO "Device created: /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit chardev_exit(void) {
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Device removed\n");
}

module_init(chardev_init);
module_exit(chardev_exit);