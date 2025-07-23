#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>

#define MY_VENDOR_ID  0x8086  // Example: Intel
#define MY_DEVICE_ID  0x15d7  // Example: Intel PRO/1001 NIC

static struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(MY_VENDOR_ID, MY_DEVICE_ID), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    printk(KERN_INFO "PCI device detected\n");
    return 0;
}

static void my_pci_remove(struct pci_dev *pdev)
{
    printk(KERN_INFO "PCI device removed\n");
}

static struct pci_driver my_driver = {
    .name = "my_pci_driver",
    .id_table = pci_ids,
    .probe = my_pci_probe,
    .remove = my_pci_remove,
};

module_pci_driver(my_driver);

MODULE_LICENSE("GPL");

