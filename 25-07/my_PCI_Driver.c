#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/io.h>

#define MY_VENDOR_ID  0x8086  // Replace with your device's vendor ID
#define MY_DEVICE_ID  0x156f  // Replace with your device's device ID

static void __iomem *bar0_virt;  // virtual address after ioremap
static struct pci_dev *pdev_global;

static struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(MY_VENDOR_ID, MY_DEVICE_ID), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int my_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    resource_size_t bar_start, bar_len;

    printk(KERN_INFO "PCIe device detected!\n");

    // Enable the PCI device
    if (pci_enable_device(pdev)) {
        printk(KERN_ERR "Cannot enable PCI device\n");
        return -ENODEV;
    }

    pdev_global = pdev;

    // Get BAR0 info
    bar_start = pci_resource_start(pdev, 0);
    bar_len = pci_resource_len(pdev, 0);

    printk(KERN_INFO "BAR0 start: %pa, length: %pa\n", &bar_start, &bar_len);

    // Request region to prevent conflicts
    if (!request_mem_region(bar_start, bar_len, "my_pci_driver")) {
        printk(KERN_ERR "Cannot request BAR0 memory region\n");
        return -EBUSY;
    }

    // Map BAR0 to kernel virtual address space
    bar0_virt = ioremap(bar_start, bar_len);
    if (!bar0_virt) {
        printk(KERN_ERR "ioremap failed\n");
        release_mem_region(bar_start, bar_len);
        return -ENOMEM;
    }

    // Example: Read 32-bit value from offset 0
    u32 val = ioread32(bar0_virt);
    printk(KERN_INFO "Read value at offset 0: 0x%x\n", val);

    return 0;
}

static void my_remove(struct pci_dev *pdev)
{
    resource_size_t bar_start = pci_resource_start(pdev, 0);
    resource_size_t bar_len   = pci_resource_len(pdev, 0);

    iounmap(bar0_virt);
    release_mem_region(bar_start, bar_len);
    pci_disable_device(pdev);
    printk(KERN_INFO "PCIe device removed.\n");
}

static struct pci_driver my_pci_driver = {
    .name     = "my_pci_driver",
    .id_table = pci_ids,
    .probe    = my_probe,
    .remove   = my_remove,
};

module_pci_driver(my_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Keerthi");
MODULE_DESCRIPTION("Minimal PCIe BAR0 MMIO Mapping Driver");

