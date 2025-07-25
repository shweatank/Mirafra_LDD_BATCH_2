#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/io.h>

#define MY_VENDOR_ID  0x8086   // Intel
#define MY_DEVICE_ID   0x10d3  // e1000

static struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(MY_VENDOR_ID, MY_DEVICE_ID), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err;
    u8 rev;
    void __iomem *hw_addr;
    resource_size_t mmio_start, mmio_len;

    pr_info(">> My PCI driver: probe called for device %04x:%04x\n",
            ent->vendor, ent->device);

    err = pci_enable_device(pdev);
    if (err) {
        pr_err("Failed to enable PCI device\n");
        return err;
    }

    pci_read_config_byte(pdev, PCI_REVISION_ID, &rev);
    pr_info("PCI device revision: 0x%x\n", rev);

    mmio_start = pci_resource_start(pdev, 0); // BAR0
    mmio_len = pci_resource_len(pdev, 0);
    if (!mmio_start || !mmio_len) {
        pr_err("Invalid BAR0\n");
        return -ENODEV;
    }

    if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
        pr_err("BAR0 is not memory-mapped I/O\n");
        return -ENODEV;
    }

    if (pci_request_region(pdev, 0, "my_pci_region")) {
        pr_err("Failed to request BAR0 region\n");
        return -EBUSY;
    }

    hw_addr = ioremap(mmio_start, mmio_len);
    if (!hw_addr) {
        pr_err("ioremap failed\n");
        pci_release_region(pdev, 0);
        return -ENOMEM;
    }

    pr_info("BAR0 mapped at %p, length = %pa\n", hw_addr, &mmio_len);

    // Example: Read a 32-bit register from offset 0
    u32 val = readl(hw_addr);
    pr_info("Read from BAR0 offset 0x0 = 0x%08x\n", val);

    // Example: Write back (no-op)
    writel(val, hw_addr);
    pr_info("Wrote back same value to BAR0 offset 0x0\n");

    iounmap(hw_addr);
    pci_release_region(pdev, 0);
    pci_disable_device(pdev);
    return 0;
}

static void my_pci_remove(struct pci_dev *pdev)
{
    pr_info("<< My PCI driver: remove called\n");
}

static struct pci_driver my_pci_driver = {
    .name = "write_read_pci",
    .id_table = pci_ids,
    .probe = my_pci_probe,
    .remove = my_pci_remove,
};

module_pci_driver(my_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ganga");
MODULE_DESCRIPTION("Simple PCI Read/Write Example Driver");

