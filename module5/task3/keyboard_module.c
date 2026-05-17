#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>

MODULE_AUTHOR("Alexander Kuleshin");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Timer module");

#define MODULE_NAME "keyboard_module"
#define BLINK_DELAY (HZ/5)
#define MAX_MASK 7
#define RESTORE_LED 0xFF

static struct timer_list my_timer;

static int led_mask = MAX_MASK;
static struct kobject* led_kobject;

static struct tty_driver* my_driver;
static int led_status = 0;

static ssize_t mask_read(struct kobject* kobj, struct kobj_attribute* attr, char* buf)
{
    return sprintf(buf, "%d\n", led_mask);
}

static ssize_t mask_write(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count) 
{
    int new_mask;
    sscanf(buf, "%d", &new_mask);

    if (new_mask < 0 || new_mask > MAX_MASK) {
        return -EINVAL;
    }

    led_mask = new_mask;
    printk(KERN_INFO "LED mask changed to: %d\n", led_mask);
    return count;
}

static struct kobj_attribute mask_attribute = __ATTR(led_mask, 0660, mask_read, mask_write);

static void my_timer_func(struct timer_list* ptr)
{
    if (led_status == led_mask) {
        led_status = RESTORE_LED;
    } else {
        led_status = led_mask;
    }
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, led_status);
    mod_timer(&my_timer, jiffies + BLINK_DELAY);
}

static int __init my_init(void)
{
    int error;

    my_driver = vc_cons[fg_console].d->port.tty->driver;

    led_kobject = kobject_create_and_add("keyboard_leds", kernel_kobj);
    if (!led_kobject) {
        return -ENOMEM;
    }

    error = sysfs_create_file(led_kobject, &mask_attribute.attr);
    if (error) {
        kobject_put(led_kobject);
        return error;
    }

    timer_setup(&my_timer, my_timer_func, 0);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    printk(KERN_INFO "LED module loaded successfully, /proc/%s created\n", MODULE_NAME);
    return 0;
}

static void __exit my_exit(void)
{
    del_timer(&my_timer);
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LED);
    kobject_put(led_kobject);

    printk(KERN_INFO "Module unloaded, /proc/%s removed\n", MODULE_NAME);
}

module_init(my_init);
module_exit(my_exit);