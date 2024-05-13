#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/cdev.h>

#define ARDUINO_VENDOR_ID   0x2341 // Cambiar si es necesario
#define ARDUINO_PRODUCT_ID  0x0043 // Cambiar si es necesario

#define ARDUINO_MINOR_BASE 192
#define BUFFER_SIZE 256

static int arduino_open(struct inode *inode, struct file *file);
static int arduino_release(struct inode *inode, struct file *file);
static ssize_t arduino_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t arduino_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);


static struct usb_device_id arduino_table[] = {
    { USB_DEVICE(ARDUINO_VENDOR_ID, ARDUINO_PRODUCT_ID) },
    {}
};
MODULE_DEVICE_TABLE(usb, arduino_table);

struct arduino_usb {
    struct usb_device *udev;
    struct usb_interface *interface;
    unsigned char bulk_in_endpointAddr;
    unsigned char bulk_out_endpointAddr;
    struct mutex io_mutex;
};

static struct file_operations arduino_fops = {
    .owner = THIS_MODULE,
    .open = arduino_open,
    .release = arduino_release,
    .read = arduino_read,
    .write = arduino_write,
};

static struct usb_class_driver arduino_class = {
    .name = "arduino_device",
    .fops = &arduino_fops,
    .minor_base = ARDUINO_MINOR_BASE,
};

static int arduino_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int arduino_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t arduino_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct arduino_usb *dev;
    int retval;
    unsigned long missing;
    unsigned int read_cnt;
    char *kernel_buf;

    dev = usb_get_intfdata(file->private_data);

    if (!mutex_trylock(&dev->io_mutex))
        return -EBUSY;

    kernel_buf = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!kernel_buf) {
        mutex_unlock(&dev->io_mutex);
        return -ENOMEM;
    }

    retval = usb_bulk_msg(dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
                          kernel_buf, min_t(size_t, count, BUFFER_SIZE), &read_cnt, 5000);

    if (retval) {
        kfree(kernel_buf);
        mutex_unlock(&dev->io_mutex);
        return retval;
    }

    missing = copy_to_user(buf, kernel_buf, read_cnt);
    kfree(kernel_buf);
    mutex_unlock(&dev->io_mutex);

    return read_cnt - missing;
}

static ssize_t arduino_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    struct arduino_usb *dev;
    int retval;
    unsigned long missing;
    char *kernel_buf;

    dev = usb_get_intfdata(file->private_data);

    if (!mutex_trylock(&dev->io_mutex))
        return -EBUSY;

    kernel_buf = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!kernel_buf) {
        mutex_unlock(&dev->io_mutex);
        return -ENOMEM;
    }

    missing = copy_from_user(kernel_buf, buf, count);
    if (missing) {
        kfree(kernel_buf);
        mutex_unlock(&dev->io_mutex);
        return -EFAULT;
    }

    retval = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                          kernel_buf, min_t(size_t, count, BUFFER_SIZE), NULL, 5000);

    kfree(kernel_buf);
    mutex_unlock(&dev->io_mutex);

    return (retval < 0) ? retval : count;
}

static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(interface);
    struct arduino_usb *dev;
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int retval = -ENOMEM;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev) {
        dev_err(&interface->dev, "Memoria insuficiente\n");
        return retval;
    }

    mutex_init(&dev->io_mutex);
    dev->udev = usb_get_dev(udev);
    dev->interface = interface;
    iface_desc = interface->cur_altsetting;

    for (int i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;
        if (!dev->bulk_in_endpointAddr && usb_endpoint_is_bulk_in(endpoint))
            dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
        if (!dev->bulk_out_endpointAddr && usb_endpoint_is_bulk_out(endpoint))
            dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
    }

    if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr)) {
        dev_err(&interface->dev, "Endpoints no encontrados\n");
        kfree(dev);
        return -ENODEV;
    }

    usb_set_intfdata(interface, dev);

    retval = usb_register_dev(interface, &arduino_class);
    if (retval) {
        dev_err(&interface->dev, "No se pudo registrar el dispositivo\n");
        usb_set_intfdata(interface, NULL);
        kfree(dev);
    } else {
        dev_info(&interface->dev, "Dispositivo registrado\n");
    }

    return retval;
}

static void arduino_disconnect(struct usb_interface *interface)
{
    struct arduino_usb *dev;

    dev = usb_get_intfdata(interface);
    usb_deregister_dev(interface, &arduino_class);

    usb_put_dev(dev->udev);
    kfree(dev);

    dev_info(&interface->dev, "Dispositivo desconectado\n");
}

static struct usb_driver arduino_driver = {
    .name = "arduino_driver",
    .id_table = arduino_table,
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
};

static int __init arduino_init(void)
{
    return usb_register(&arduino_driver);
}

static void __exit arduino_exit(void)
{
    usb_deregister(&arduino_driver);
}

module_init(arduino_init);
module_exit(arduino_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tu nombre");
MODULE_DESCRIPTION("Driver para Arduino Uno");
