//usb_driver
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define USB_VENDOR_ID    0x10c4  // Example: SanDisk
#define USB_PRODUCT_ID   0xea60  // Example: Cruzer Blade

static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "USB Device Plugged In: VID=0x%04x PID=0x%04x\n",
           id->idVendor, id->idProduct);
    return 0;
}

static void usb_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "USB Device Removed\n");
}

static struct usb_device_id usb_table[] = {
    { USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID) },
    {} // Terminating entry
};
MODULE_DEVICE_TABLE(usb, usb_table);

static struct usb_driver usb_example_driver = {
    .name = "usb_simple_driver",
    .id_table = usb_table,
    .probe = usb_probe,
    .disconnect = usb_disconnect,
};

static int __init usb_driver_init(void)
{
    return usb_register(&usb_example_driver);
}

static void __exit usb_driver_exit(void)
{
    usb_deregister(&usb_example_driver);
}

module_init(usb_driver_init);
module_exit(usb_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("Simple USB Driver for Raspberry Pi 4B");
