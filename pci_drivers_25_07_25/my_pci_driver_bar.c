#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/io.h>

#define DRIVER_NAME "my_pci_driver"

static const struct pci_device_id my_pci_ids[] = {
    { PCI_DEVICE(0x8086, 0x100e) },  // e1000 NIC
    { 0, }
};
MODULE_DEVICE_TABLE(pci, my_pci_ids);

static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err;
    int i;
    u32 val;
    resource_size_t bar_start, bar_len;
    void __iomem *mmio;

    printk(KERN_INFO DRIVER_NAME ": Probing device (vendor=0x%x, device=0x%x)\n",
           pdev->vendor, pdev->device);

    err = pci_enable_device(pdev);
    if (err)
        return err;

    // Loop through all 6 possible BARs (0-5)
    for (i = 0; i < 6; i++) {
        bar_start = pci_resource_start(pdev, i);
        bar_len   = pci_resource_len(pdev, i);

        if (!bar_start || !bar_len)
            continue; // This BAR not implemented

        printk(KERN_INFO DRIVER_NAME ": BAR%d Start: 0x%llx, Length: %llu\n",
               i, (unsigned long long)bar_start, (unsigned long long)bar_len);

        // Request the region
        err = pci_request_region(pdev, i, DRIVER_NAME);
        if (err) {
            printk(KERN_ERR DRIVER_NAME ": Failed to request BAR%d region\n", i);
            continue;
        }

        // Map only BAR0 for MMIO testing
        if (i == 0) {
            mmio = ioremap(bar_start, bar_len);
            if (!mmio) {
                printk(KERN_ERR DRIVER_NAME ": Failed to ioremap BAR0\n");
                pci_release_region(pdev, i);
                continue;
            }

            val = ioread32(mmio + 0x00);  // Read 0x00 offset register
            printk(KERN_INFO DRIVER_NAME ": MMIO[0x00] = 0x%08x\n", val);

            iounmap(mmio);
        }

        pci_release_region(pdev, i);  // Clean up
    }

    return 0;
}

static void my_pci_remove(struct pci_dev *pdev)
{
    printk(KERN_INFO DRIVER_NAME ": Device removed\n");
}

static struct pci_driver my_pci_driver = {
    .name     = DRIVER_NAME,
    .id_table = my_pci_ids,
    .probe    = my_pci_probe,
    .remove   = my_pci_remove,
};

module_pci_driver(my_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ganga");
MODULE_DESCRIPTION("Basic PCI Driver with BAR access");

