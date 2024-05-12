#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define ARDUINO_VENDOR_ID 0x2341
#define ARDUINO_PRODUCT_ID 0x0043
#define ARDUINO_EP_IN 0x81 // Endpoint de entrada (lectura)
#define ARDUINO_EP_OUT 0x02 // Endpoint de salida (escritura)
#define MAX_BUF_SIZE 64 // Tamaño máximo del buffer de lectura/escritura

static struct usb_device_id arduino_table[] = {
    { USB_DEVICE(ARDUINO_VENDOR_ID, ARDUINO_PRODUCT_ID) },
    {},
};

MODULE_DEVICE_TABLE(usb, arduino_table);

static int arduino_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Arduino Uno device opened\n");
    return 0;
}

static int arduino_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Arduino Uno device closed\n");
    return 0;
}

static ssize_t arduino_read(struct file *file, char *buffer, size_t count, loff_t *offset)
{
    struct usb_interface *interface;
    struct usb_device *dev;
    int retval;
    char *data;
    size_t read_count;

    // Obtener la interfaz y el dispositivo USB
    interface = file->private_data;
    dev = interface_to_usbdev(interface);

    // Reservar memoria para el buffer de lectura
    data = kmalloc(MAX_BUF_SIZE, GFP_KERNEL);
    if (!data) {
        return -ENOMEM;
    }

    // Leer desde el dispositivo USB
    retval = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0), ARDUINO_EP_IN,
                             USB_TYPE_VENDOR | USB_DIR_IN,
                             0, 0, data, MAX_BUF_SIZE, 1000);
    if (retval < 0) {
        printk(KERN_ERR "Error reading from Arduino Uno (%d)\n", retval);
        kfree(data);
        return retval;
    }

    // Copiar los datos al buffer del usuario
    read_count = min_t(size_t, count, retval);
    if (copy_to_user(buffer, data, read_count)) {
        kfree(data);
        return -EFAULT;
    }

    kfree(data);
    return read_count;
}

static ssize_t arduino_write(struct file *file, const char *buffer, size_t count, loff_t *offset)
{
    struct usb_interface *interface;
    struct usb_device *dev;
    int retval;
    char *data;

    // Obtener la interfaz y el dispositivo USB
    interface = file->private_data;
    dev = interface_to_usbdev(interface);

    // Reservar memoria para el buffer de escritura
    data = kmalloc(count, GFP_KERNEL);
    if (!data) {
        return -ENOMEM;
    }

    // Copiar los datos desde el buffer del usuario
    if (copy_from_user(data, buffer, count)) {
        kfree(data);
        return -EFAULT;
    }

    // Escribir en el dispositivo USB
    retval = usb_control_msg(dev, usb_sndctrlpipe(dev, 0), ARDUINO_EP_OUT,
                             USB_TYPE_VENDOR | USB_DIR_OUT,
                             0, 0, data, count, 1000);
    if (retval < 0) {
        printk(KERN_ERR "Error writing to Arduino Uno (%d)\n", retval);
        kfree(data);
        return retval;
    }

    kfree(data);
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = arduino_open,
    .release = arduino_release,
    .read = arduino_read,
    .write = arduino_write,
};

static struct usb_class_driver arduino_class = {
    .name = "arduino%d",
    .fops = &fops,
};

static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;

    printk(KERN_INFO "Arduino Uno device (%04X:%04X) plugged\n", id->idVendor, id->idProduct);

    // Registrar el dispositivo como un dispositivo de clase USB
    retval = usb_register_dev(interface, &arduino_class);
    if (retval) {
        printk(KERN_ERR "Error registering Arduino Uno device (%d)\n", retval);
        return retval;
    }

    return 0;
}

static void arduino_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Arduino Uno device unplugged\n");

    // Eliminar el dispositivo de clase USB
    usb_deregister_dev(interface, &arduino_class);
}

static struct usb_driver arduino_driver = {
    .name = "arduino_driver",
    .id_table = arduino_table,
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
};

static int __init arduino_init(void)
{
    int retval;

    // Inicializar el driver USB
    retval = usb_register(&arduino_driver);
    if (retval) {
        printk(KERN_ERR "Error registering Arduino Uno USB driver (%d)\n", retval);
        return retval;
    }

    printk(KERN_INFO "Arduino Uno USB driver loaded\n");
    return 0;
}

static void __exit arduino_exit(void)
{
    // Deregistrar el driver USB
    usb_deregister(&arduino_driver);
    printk(KERN_INFO "Arduino Uno USB driver unloaded\n");
}

module_init(arduino_init);
module_exit(arduino_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Arduino Uno USB Driver");
