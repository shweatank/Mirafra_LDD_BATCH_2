#include <linux/module.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/pm.h>

#define DRIVER_NAME     "e1000_pci_demo"
#define DRIVER_VERSION  "1.1"
#define MY_VENDOR_ID    0x8086  // Intel
#define MY_DEVICE_ID    0x100e  // e1000

struct e1000_dev {
    void __iomem *hw_addr;
    struct pci_dev *pdev;
    int irq;
};

static struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(MY_VENDOR_ID, MY_DEVICE_ID), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

/* Simple IRQ handler */
static irqreturn_t e1000_irq_handler(int irq, void *dev_id)
{
    struct e1000_dev *dev = dev_id;
    pr_info("%s: IRQ %d received\n", DRIVER_NAME, irq);
    pr_info("\nInterrupt received for device with BAR0 at %p\n", dev->hw_addr);

    // Acknowledge IRQ if needed
    return IRQ_HANDLED;
}

static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err;
    u8 rev;
    u16 vendor, device;
    struct e1000_dev *edev;
    resource_size_t mmio_start, mmio_len;

    pr_info("%s: Probe called for %04x:%04x\n", DRIVER_NAME,
            ent->vendor, ent->device);

    err = pci_enable_device(pdev);
    if (err)
        return err;

    pci_set_master(pdev);

    pci_read_config_byte(pdev, PCI_REVISION_ID, &rev);
    pci_read_config_word(pdev, PCI_VENDOR_ID, &vendor);
    pci_read_config_word(pdev, PCI_DEVICE_ID, &device);

    pr_info("%s: Vendor: 0x%04x Device: 0x%04x Rev: 0x%02x\n",
            DRIVER_NAME, vendor, device, rev);

    // Allocate driver data
    edev = devm_kzalloc(&pdev->dev, sizeof(*edev), GFP_KERNEL);
    if (!edev)
        return -ENOMEM;

    pci_set_drvdata(pdev, edev);
    edev->pdev = pdev;

    mmio_start = pci_resource_start(pdev, 0);
    mmio_len = pci_resource_len(pdev, 0);
    if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
        pr_err("%s: BAR0 is not MMIO\n", DRIVER_NAME);
        return -ENODEV;
    }

    err = pci_request_region(pdev, 0, DRIVER_NAME);
    if (err) {
        pr_err("%s: BAR0 region busy\n", DRIVER_NAME);
        return err;
    }

    edev->hw_addr = devm_ioremap(&pdev->dev, mmio_start, mmio_len);
    if (!edev->hw_addr) {
        pci_release_region(pdev, 0);
        return -ENOMEM;
    }

    pr_info("%s: BAR0 mapped at %p (len = 0x%lx)\n", DRIVER_NAME,
            edev->hw_addr, (unsigned long)mmio_len);

    // Test read/write
    u32 val = readl(edev->hw_addr);
    pr_info("%s: BAR0[0x0] = 0x%08x\n", DRIVER_NAME, val);
    writel(val, edev->hw_addr); // write back

    // Request IRQ
    edev->irq = pdev->irq;
    err = request_irq(edev->irq, e1000_irq_handler, IRQF_SHARED,
                      DRIVER_NAME, edev);
    if (err) {
        pr_err("%s: Failed to request IRQ %d\n", DRIVER_NAME, edev->irq);
        pci_release_region(pdev, 0);
        return err;
    }
    pr_info("%s: IRQ %d registered\n", DRIVER_NAME, edev->irq);

    return 0;
}

static void my_pci_remove(struct pci_dev *pdev)
{
    struct e1000_dev *edev = pci_get_drvdata(pdev);

    pr_info("%s: Removing device\n", DRIVER_NAME);

    if (edev->irq)
        free_irq(edev->irq, edev);

    pci_release_region(pdev, 0);
    pci_disable_device(pdev);
}

static struct pci_driver my_pci_driver = {
    .name = DRIVER_NAME,
    .id_table = pci_ids,
    .probe = my_pci_probe,
    .remove = my_pci_remove,
};

module_pci_driver(my_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ganga");
MODULE_DESCRIPTION("Intel e1000 PCI Demo Driver");
MODULE_VERSION(DRIVER_VERSION);
