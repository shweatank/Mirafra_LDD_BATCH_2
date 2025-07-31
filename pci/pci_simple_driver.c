#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/io.h>

#define DRIVER_NAME "pci_simple"

static const struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(0x10ec, 0x8168) },  // Realtek Ethernet (replace if needed)
    { 0, }  // Terminating entry
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int pci_simple_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err;
    u16 vendor, device;

    printk(KERN_INFO "[pci_simple] Probing device\n");

    err = pci_enable_device(pdev);
    if (err) {
        printk(KERN_ERR "[pci_simple] Failed to enable device\n");
        return err;
    }

    vendor = pdev->vendor;
    device = pdev->device;

    printk(KERN_INFO "[pci_simple] Found device: Vendor ID = 0x%x, Device ID = 0x%x\n", vendor, device);

    // Normally you'd map BAR regions, request regions, etc.

    return 0;
}

static void pci_simple_remove(struct pci_dev *pdev)
{
    printk(KERN_INFO "[pci_simple] Device removed\n");

    // Normally you'd unmap regions, free memory, etc.
}

static struct pci_driver pci_simple_driver = {
    .name = DRIVER_NAME,
    .id_table = pci_ids,
    .probe = pci_simple_probe,
    .remove = pci_simple_remove,
};

static int __init pci_simple_init(void)
{
    printk(KERN_INFO "[pci_simple] Initializing driver\n");
    return pci_register_driver(&pci_simple_driver);
}

static void __exit pci_simple_exit(void)
{
    printk(KERN_INFO "[pci_simple] Exiting driver\n");
    pci_unregister_driver(&pci_simple_driver);
}

module_init(pci_simple_init);
module_exit(pci_simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arya");
MODULE_DESCRIPTION("Simple PCI driver for Realtek Ethernet");

