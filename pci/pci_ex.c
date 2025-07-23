#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include<linux/io.h>
#define DRIVER_NAME "pci_ex"

static int pci_ex_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    printk(KERN_INFO "%s: Matched device %04x:%04x\n", DRIVER_NAME, pdev->vendor, pdev->device);
    return 0;
}

static void pci_ex_remove(struct pci_dev *pdev)
{
    printk(KERN_INFO "%s: Removed device\n", DRIVER_NAME);
}

static const struct pci_device_id pci_ex_table[] = {
    { PCI_DEVICE(0x8086, 0x15be) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ex_table);

static struct pci_driver pci_ex_driver = {
    .name = DRIVER_NAME,
    .id_table = pci_ex_table,
    .probe = pci_ex_probe,
    .remove = pci_ex_remove,
};

static int __init pci_ex_init(void)
{
    printk(KERN_INFO "%s: Initializing PCI driver\n", DRIVER_NAME);
    return pci_register_driver(&pci_ex_driver);
}

static void __exit pci_ex_exit(void)
{
    printk(KERN_INFO "%s: Exiting PCI driver\n", DRIVER_NAME);
    pci_unregister_driver(&pci_ex_driver);
}

module_init(pci_ex_init);
module_exit(pci_ex_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("PCI Example Driver");

