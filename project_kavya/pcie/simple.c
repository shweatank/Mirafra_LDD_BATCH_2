https://github.com/Johannes4Linux/gpio_card_pcittl32io/blob/main/pcittl32io.c
// File: simple_pcie_driver.c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/io.h>

#define DRIVER_NAME "simple_pcie"

// Use a known, widely available PCIe device
// Example: Intel Ethernet Controller I225-V (change as needed)
#define PCI_VENDOR_ID_EXAMPLE 0x8086
#define PCI_DEVICE_ID_EXAMPLE 0x2526

static struct pci_device_id simple_pci_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_EXAMPLE, PCI_DEVICE_ID_EXAMPLE), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, simple_pci_ids);

static int simple_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
		printk(KERN_INFO "hi hello\n");
    int err;
    void __iomem *hw_addr;

    pr_info(DRIVER_NAME ": Found PCI device %04x:%04x\n", pdev->vendor, pdev->device);

    err = pci_enable_device(pdev);
    if (err) {
        pr_err(DRIVER_NAME ": Cannot enable PCI device\n");
        return err;
    }

    err = pci_request_regions(pdev, DRIVER_NAME);
    if (err) {
        pr_err(DRIVER_NAME ": Cannot request PCI regions\n");
        pci_disable_device(pdev);
        return err;
    }

    hw_addr = pci_iomap(pdev, 0, pci_resource_len(pdev, 0));
    if (!hw_addr) {
        pr_err(DRIVER_NAME ": pci_iomap failed\n");
        pci_release_regions(pdev);
        pci_disable_device(pdev);
        return -ENOMEM;
    }

    // Read from offset 0x00 (often a status/control register)
    u32 reg_val = ioread32(hw_addr);
    pr_info(DRIVER_NAME ": BAR0[0x00] = 0x%08x\n", reg_val);

    pr_info(DRIVER_NAME ": PCIe communication successful!\n");

    pci_iounmap(pdev, hw_addr);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

    return 0;
}

static void simple_remove(struct pci_dev *pdev)
{
    pr_info(DRIVER_NAME ": Device removed\n");
}

static struct pci_driver simple_pci_driver = {
    .name = DRIVER_NAME,
    .id_table = simple_pci_ids,
    .probe = simple_probe,
    .remove = simple_remove,
};

module_pci_driver(simple_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kavya");
MODULE_DESCRIPTION("Clean PCIe communication demo");

