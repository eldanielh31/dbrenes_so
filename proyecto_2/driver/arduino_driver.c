#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#define USB_ARDUINO_MINOR_BASE 192

static struct usb_class_driver arduino_class;
static struct usb_device *device;
static struct mutex io_mutex;
static unsigned char bulk_out_endpoint;
static unsigned char bulk_in_endpoint;

static int arduino_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int arduino_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t arduino_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos)
{
    int retval;
    int actual_length;
    char *buf;

    buf = kmalloc(count, GFP_KERNEL);
    if (!buf) {
        return -ENOMEM;
    }

    if (copy_from_user(buf, user_buffer, count)) {
        kfree(buf);
        return -EFAULT;
    }

    mutex_lock(&io_mutex);
    retval = usb_bulk_msg(device, usb_sndbulkpipe(device, bulk_out_endpoint), buf, count, &actual_length, 5000);
    mutex_unlock(&io_mutex);

    kfree(buf);

    if (retval) {
        printk(KERN_ERR "Failed to write to Arduino: %d\n", retval);
        return retval;
    }

    return actual_length;
}

static ssize_t arduino_read(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos)
{
    int retval;
    int actual_length;
    char *buf;

    buf = kmalloc(count, GFP_KERNEL);
    if (!buf) {
        return -ENOMEM;
    }

    mutex_lock(&io_mutex);
    retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, bulk_in_endpoint), buf, count, &actual_length, 5000);
    mutex_unlock(&io_mutex);

    if (retval) {
        kfree(buf);
        printk(KERN_ERR "Failed to read from Arduino: %d\n", retval);
        return retval;
    }

    if (copy_to_user(user_buffer, buf, actual_length)) {
        kfree(buf);
        return -EFAULT;
    }

    kfree(buf);

    return actual_length;
}

static const struct file_operations arduino_fops = {
    .owner = THIS_MODULE,
    .open = arduino_open,
    .release = arduino_release,
    .write = arduino_write,
    .read = arduino_read,
};

static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    device = interface_to_usbdev(interface);
    iface_desc = interface->cur_altsetting;

    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;

        if (!bulk_out_endpoint && usb_endpoint_is_bulk_out(endpoint)) {
            bulk_out_endpoint = endpoint->bEndpointAddress;
            printk(KERN_INFO "Found bulk out endpoint: 0x%02x\n", bulk_out_endpoint);
        }

        if (!bulk_in_endpoint && usb_endpoint_is_bulk_in(endpoint)) {
            bulk_in_endpoint = endpoint->bEndpointAddress;
            printk(KERN_INFO "Found bulk in endpoint: 0x%02x\n", bulk_in_endpoint);
        }
    }

    arduino_class.name = "usb/arduino%d";
    arduino_class.fops = &arduino_fops;
    retval = usb_register_dev(interface, &arduino_class);
    if (retval) {
        printk(KERN_ERR "Not able to get a minor for this device.");
    } else {
        printk(KERN_INFO "Arduino Uno connected.");
    }

    return retval;
}

static void arduino_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface, &arduino_class);
    printk(KERN_INFO "Arduino Uno disconnected.");
}

static struct usb_device_id arduino_table[] = {
    { USB_DEVICE(0x2341, 0x0043) },
    {}
};
MODULE_DEVICE_TABLE(usb, arduino_table);

static struct usb_driver arduino_driver = {
    .name = "arduino_driver",
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
    .id_table = arduino_table,
};

static int __init arduino_init(void)
{
    int result;

    mutex_init(&io_mutex);
    result = usb_register(&arduino_driver);
    if (result) {
        printk(KERN_ERR "usb_register failed. Error number %d", result);
    }

    return result;
}

static void __exit arduino_exit(void)
{
    usb_deregister(&arduino_driver);
}

module_init(arduino_init);
module_exit(arduino_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Brenes");
MODULE_DESCRIPTION("Driver para Arduino Uno");
