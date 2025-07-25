#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>

#define MY_VENDOR_ID 0x1234  // vendor ID
#define MY_DEVICE_ID 0x11e8  // device ID

// Probe function: Called when a matching PCI device is found
static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    pr_info("my_pci_driver: Device found! Vendor: 0x%x, Device: 0x%x\n", 
            pdev->vendor, pdev->device);

    if (pci_enable_device(pdev)) {
        pr_err("my_pci_driver: Failed to enable PCI device\n");
        return -ENODEV;
    }

    pr_info("my_pci_driver: PCI device enabled successfully\n");

    return 0;
}

// Remove function: Called when device is removed/unregistered
static void my_pci_remove(struct pci_dev *pdev)
{
    pr_info("my_pci_driver: Device removed\n");
    pci_disable_device(pdev);
}

// Device ID table
static const struct pci_device_id my_pci_table[] = {
    { PCI_DEVICE(MY_VENDOR_ID, MY_DEVICE_ID) },
    { 0, } // Terminating entry
};
MODULE_DEVICE_TABLE(pci, my_pci_table);

// PCI driver structure
static struct pci_driver my_pci_driver = {
    .name = "my_pci_driver",
    .id_table = my_pci_table,
    .probe = my_pci_probe,
    .remove = my_pci_remove,
};

// Init and Exit
static int __init my_pci_init(void)
{
    pr_info("my_pci_driver: Module loaded\n");
    return pci_register_driver(&my_pci_driver);
}

static void __exit my_pci_exit(void)
{
    pr_info("my_pci_driver: Module unloaded\n");
    pci_unregister_driver(&my_pci_driver);
}

module_init(my_pci_init);
module_exit(my_pci_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ganga");
MODULE_DESCRIPTION("Simple PCI driver with probe and remove");

