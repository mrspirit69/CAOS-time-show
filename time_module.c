#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timekeeping.h>

#define DEVICE_NAME "time_dev"
#define CLASS_NAME  "time_class"

MODULE_LICENSE("GPL");

static int major;
static struct class*  time_class  = NULL;
static struct device* time_device = NULL;

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    struct timespec64 ts;
    ktime_get_real_ts64(&ts);  // Получаем текущее время

    char time_str[128];
    int n = snprintf(time_str, sizeof(time_str), "Time: %lld.%.9ld\n",
                     (long long)ts.tv_sec, ts.tv_nsec);

    if (copy_to_user(buffer, time_str, n)) {
        return -EFAULT;
    }

    return n;
}

static struct file_operations fops = {
    .read = dev_read,
};

static int __init time_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "TimeModule: failed to register device\n");
        return major;
    }

    time_class = class_create(THIS_MODULE, CLASS_NAME);
    time_device = device_create(time_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    printk(KERN_INFO "TimeModule: loaded with device /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit time_exit(void) {
    device_destroy(time_class, MKDEV(major, 0));
    class_unregister(time_class);
    class_destroy(time_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "TimeModule: unloaded\n");
}

module_init(time_init);
module_exit(time_exit);
