#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>

#define DRV_NAME "intel_i219lm_mac_reader"
#define VENDOR_ID 0x10ec  // Realtek
#define DEVICE_ID 0x8168  // RTL8111/8168/8411


static void __iomem *hw_addr;

static const struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int intel_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    int err;
    resource_size_t bar_start, bar_len;
    u32 ral, rah;
    u8 mac[6];

    printk(KERN_INFO DRV_NAME ": Probing Intel I219-LM...\n");

    err = pci_enable_device(pdev); //enable the pci device
    if (err) {
        printk(KERN_ERR DRV_NAME ": Failed to enable PCI device\n");
        return err;
    }

    err = pci_request_region(pdev, 0, DRV_NAME);//Reserves and maps the BAR0 (Base Address Register 0) memory-mapped region of the device.
    if (err) {
        printk(KERN_ERR DRV_NAME ": Failed to request BAR0\n");
        goto disable_device;
    }

    bar_start = pci_resource_start(pdev, 0);
    bar_len   = pci_resource_len(pdev, 0);

    printk(KERN_INFO DRV_NAME ": BAR0 at 0x%llx, size %llu\n",
           (unsigned long long)bar_start, (unsigned long long)bar_len);

    hw_addr = ioremap(bar_start, bar_len);//ioremap() gives the CPU a virtual address to access device registers.
    if (!hw_addr) {
        printk(KERN_ERR DRV_NAME ": Failed to ioremap BAR0\n");
        err = -ENOMEM;
        goto release_region;
    }

    // Read MAC address from 0x5400 and 0x5404 (RAL0 and RAH0)
    ral = ioread32(hw_addr + 0x5400);//reading mac address from the registers
    rah = ioread32(hw_addr + 0x5404);

    mac[0] = ral & 0xFF;
    mac[1] = (ral >> 8) & 0xFF;
    mac[2] = (ral >> 16) & 0xFF;
    mac[3] = (ral >> 24) & 0xFF;
    mac[4] = rah & 0xFF;
    mac[5] = (rah >> 8) & 0xFF;

    printk(KERN_INFO DRV_NAME ": MAC Address: %pM\n", mac); //printing the mac address
    return 0;

release_region:
    pci_release_region(pdev, 0);
disable_device:
    pci_disable_device(pdev);
    return err;
}

static void intel_remove(struct pci_dev *pdev)
{
    printk(KERN_INFO DRV_NAME ": Removing driver\n");
    if (hw_addr)
        iounmap(hw_addr);
    pci_release_region(pdev, 0);
    pci_disable_device(pdev);
}

static struct pci_driver intel_pci_driver = {
    .name = DRV_NAME,
    .id_table = pci_ids,
    .probe = intel_probe,
    .remove = intel_remove,
};

module_pci_driver(intel_pci_driver);//inserting the driver

MODULE_LICENSE("GPL");
MODULE_AUTHOR("arya");
MODULE_DESCRIPTION("Custom PCI Driver to Read MAC from Intel I219-LM NIC");
