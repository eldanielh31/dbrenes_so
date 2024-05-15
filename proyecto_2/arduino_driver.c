#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Arduino Uno connected!\n");
    // Aquí puedes agregar cualquier acción adicional que desees realizar cuando se conecte el Arduino Uno
    return 0; // Devuelve 0 para indicar que se ha manejado la conexión correctamente
}

static void arduino_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Arduino Uno disconnected!\n");
    // Aquí puedes agregar cualquier acción adicional que desees realizar cuando se desconecte el Arduino Uno
}

static struct usb_device_id arduino_table[] =
{
    { USB_DEVICE(0x2341, 0x0043) }, // Identificador USB del Arduino Uno
    {} // Terminador de la lista
};

MODULE_DEVICE_TABLE(usb, arduino_table);

static struct usb_driver arduino_driver =
{
    .name = "arduino_driver",
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
    .id_table = arduino_table,
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
MODULE_AUTHOR("Daniel Brenes");
MODULE_DESCRIPTION("Driver para Arduino Uno");
