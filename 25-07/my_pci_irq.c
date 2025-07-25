#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#define DRIVER_NAME "pci_bar_irq_example"

// Replace with actual Vendor and Device ID
#define MY_VENDOR_ID 0x15ef   // Example: Realtek
#define MY_DEVICE_ID 0x8086   // Example: RTL8111/8168/8411 Ethernet Controller

static void __iomem *bar0_addr = NULL;
static resource_size_t bar0_len;
static int irq_line;

static const struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(MY_VENDOR_ID, MY_DEVICE_ID) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

// Interrupt handler
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "[%s] Interrupt handled: IRQ %d\n", DRIVER_NAME, irq);
    // Usually, clear device interrupt status here
    return IRQ_HANDLED;
}

static int pci_bar_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err;

    printk(KERN_INFO "[%s] PCIe device detected!\n", DRIVER_NAME);

    err = pci_enable_device(pdev);
    if (err) {
        printk(KERN_ERR "[%s] Failed to enable PCI device\n", DRIVER_NAME);
        return err;
    }

    // Enable bus-mastering
    pci_set_master(pdev);

    // Request BAR0 region
    err = pci_request_region(pdev, 0, DRIVER_NAME);
    if (err) {
        printk(KERN_ERR "[%s] Failed to request BAR0\n", DRIVER_NAME);
        goto disable_device;
    }

    // Map BAR0
    bar0_len = pci_resource_len(pdev, 0);
    bar0_addr = pci_iomap(pdev, 0, bar0_len);
    if (!bar0_addr) {
        printk(KERN_ERR "[%s] Failed to map BAR0\n", DRIVER_NAME);
        goto release_region;
    }

    printk(KERN_INFO "[%s] BAR0 start: 0x%lx, length: 0x%lx\n",
           DRIVER_NAME,
           (unsigned long)pci_resource_start(pdev, 0),
           (unsigned long)bar0_len);

    // Read value at offset 0 from BAR0
    u32 value = ioread32(bar0_addr);
    printk(KERN_INFO "[%s] Read value at offset 0: 0x%x\n", DRIVER_NAME, value);

    // Get IRQ line and register interrupt handler
    irq_line = pdev->irq;
    err = request_irq(irq_line, my_irq_handler, IRQF_SHARED, DRIVER_NAME, pdev);
    if (err) {
        printk(KERN_ERR "[%s] Failed to request IRQ %d\n", DRIVER_NAME, irq_line);
        goto unmap_ioregion;
    }

    printk(KERN_INFO "[%s] IRQ %d registered successfully\n", DRIVER_NAME, irq_line);
    return 0;

unmap_ioregion:
    pci_iounmap(pdev, bar0_addr);
release_region:
    pci_release_region(pdev, 0);
disable_device:
    pci_disable_device(pdev);
    return err;
}

static void pci_bar_remove(struct pci_dev *pdev)
{
    printk(KERN_INFO "[%s] Device removed\n", DRIVER_NAME);

    free_irq(irq_line, pdev);
    pci_iounmap(pdev, bar0_addr);
    pci_release_region(pdev, 0);
    pci_disable_device(pdev);
}

static struct pci_driver pci_bar_driver = {
    .name = DRIVER_NAME,
    .id_table = pci_ids,
    .probe = pci_bar_probe,
    .remove = pci_bar_remove,
};

module_pci_driver(pci_bar_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Keerthi");
MODULE_DESCRIPTION("PCIe BAR0 with IRQ example driver");

