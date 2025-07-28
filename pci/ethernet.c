/* SPDX-License-Identifier: GPL-2.0 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/if_ether.h>
#include <linux/ip.h>

#define DRV_NAME "intel_i219lm_txrx"
#define VENDOR_ID 0x10ec
#define DEVICE_ID 0x8168

#define TX_RING_SIZE  8
#define RX_RING_SIZE  8
#define TX_DESC_SIZE  16
#define RX_DESC_SIZE  16

#define ETH_FRAME_LEN 1518

// Register offsets (Intel e1000e family)
#define REG_CTRL     0x0000
#define REG_STATUS   0x0008
#define REG_TDBAL    0x3800
#define REG_TDBAH    0x3804
#define REG_TDLEN    0x3808
#define REG_TDH      0x3810
#define REG_TDT      0x3818
#define REG_TCTL     0x0400
#define REG_TIPG     0x0410
#define REG_RDBAL    0x2800
#define REG_RDBAH    0x2804
#define REG_RDLEN    0x2808
#define REG_RDH      0x2810
#define REG_RDT      0x2818
#define REG_RCTL     0x0100

struct pci_dev *pdev_g;
void __iomem *hw_addr;

struct tx_desc {
    u64 buffer_addr;
    u16 length;
    u8 cso;
    u8 cmd;
    u8 status;
    u8 css;
    u16 special;
} __attribute__((packed));

struct rx_desc {
    u64 buffer_addr;
    u16 length;
    u16 checksum;
    u8 status;
    u8 errors;
    u16 special;
} __attribute__((packed));

struct tx_desc *tx_ring;
dma_addr_t tx_ring_dma;
char *tx_buffer;
dma_addr_t tx_buffer_dma;

struct rx_desc *rx_ring;
dma_addr_t rx_ring_dma;
char *rx_buffer[RX_RING_SIZE];
dma_addr_t rx_buffer_dma[RX_RING_SIZE];

static const struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID), },
    { 0 }
};//This tells the Linux kernel: “When you detect a PCI device with VENDOR_ID = 0x10ec and DEVICE_ID = 0x8168, use this driver.”
MODULE_DEVICE_TABLE(pci, pci_ids);

static void write_reg(u32 reg, u32 val) {
    iowrite32(val, hw_addr + reg);
}

static u32 read_reg(u32 reg) {
    return ioread32(hw_addr + reg);
}

static void setup_tx_ring(void) {
    tx_ring = dma_alloc_coherent(&pdev_g->dev, TX_RING_SIZE * sizeof(struct tx_desc), &tx_ring_dma, GFP_KERNEL);
    tx_buffer = dma_alloc_coherent(&pdev_g->dev, ETH_FRAME_LEN, &tx_buffer_dma, GFP_KERNEL);

    write_reg(REG_TDBAL, tx_ring_dma & 0xFFFFFFFF);
    write_reg(REG_TDBAH, (tx_ring_dma >> 32) & 0xFFFFFFFF);
    write_reg(REG_TDLEN, TX_RING_SIZE * sizeof(struct tx_desc));
    write_reg(REG_TDH, 0);
    write_reg(REG_TDT, 0);

    write_reg(REG_TCTL, (1 << 1) | (1 << 3) | (0x40 << 12));
    write_reg(REG_TIPG, 10 | (8 << 10) | (12 << 20));
}

static void setup_rx_ring(void) {
    rx_ring = dma_alloc_coherent(&pdev_g->dev, RX_RING_SIZE * sizeof(struct rx_desc), &rx_ring_dma, GFP_KERNEL);

    for (int i = 0; i < RX_RING_SIZE; i++) {
        rx_buffer[i] = dma_alloc_coherent(&pdev_g->dev, ETH_FRAME_LEN, &rx_buffer_dma[i], GFP_KERNEL);
        rx_ring[i].buffer_addr = rx_buffer_dma[i];
        rx_ring[i].status = 0;
    }

    write_reg(REG_RDBAL, rx_ring_dma & 0xFFFFFFFF);
    write_reg(REG_RDBAH, (rx_ring_dma >> 32) & 0xFFFFFFFF);
    write_reg(REG_RDLEN, RX_RING_SIZE * sizeof(struct rx_desc));
    write_reg(REG_RDH, 0);
    write_reg(REG_RDT, RX_RING_SIZE - 1);

    write_reg(REG_RCTL, (1 << 1) | (1 << 15) | (1 << 26)); // RXEN | BAM | SECRC
}

static int pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent) {
    int err;
    resource_size_t bar_start;

    pdev_g = pdev;

    err = pci_enable_device(pdev);
    if (err)
        return err;

    err = pci_request_region(pdev, 0, DRV_NAME);//Reserves and maps the BAR0 (Base Address Register 0) memory-mapped region of the device.
    if (err)
        goto disable;

    bar_start = pci_resource_start(pdev, 0);
    hw_addr = ioremap(bar_start, 128 * 1024);//ioremap() gives the CPU a virtual address to access device registers.

    u32 ral = read_reg(0x5400);//Reads the MAC address directly from hardware registers and prints it.
    u32 rah = read_reg(0x5404);
    u8 mac[6] = {
        ral & 0xFF, (ral >> 8) & 0xFF, (ral >> 16) & 0xFF, (ral >> 24) & 0xFF,
        rah & 0xFF, (rah >> 8) & 0xFF
    };
    printk(KERN_INFO DRV_NAME ": MAC Address: %pM\n", mac);

    write_reg(REG_CTRL, read_reg(REG_CTRL) | (1 << 26));
    udelay(10);
    while (read_reg(REG_CTRL) & (1 << 26));

    setup_tx_ring();//Allocates a TX descriptor ring using dma_alloc_coherent.

Allocates a TX buffer and sets up hardware registers for TX.
    setup_rx_ring();//Allocates RX descriptors and buffers for packet reception.

Initializes the hardware RX registers.

    // Build valid Ethernet frame//destination mac address hardcoded
    u8 dest_mac[6] = {0xf8, 0x75, 0xa4, 0x1a, 0x1c, 0x4a}; // Set to other machine's MAC
    struct ethhdr *eth = (struct ethhdr *)tx_buffer;
    memcpy(eth->h_dest, dest_mac, ETH_ALEN);
    memcpy(eth->h_source, mac, ETH_ALEN);
    eth->h_proto = htons(ETH_P_IP);

    const char *payload = "HelloFromPCI";
    memcpy(tx_buffer + sizeof(struct ethhdr), payload, strlen(payload));
    u16 total_len = sizeof(struct ethhdr) + strlen(payload);

    tx_ring[0].buffer_addr = tx_buffer_dma;
    tx_ring[0].length = total_len;
    tx_ring[0].cmd = (1 << 0) | (1 << 3); // EOP | RS
    tx_ring[0].status = 0;

    write_reg(REG_TDT, 1);
    printk(KERN_INFO DRV_NAME ": Ethernet frame sent to %pM\n", dest_mac);

    return 0;

disable:
    pci_disable_device(pdev);
    return err;
}

static void pci_remove(struct pci_dev *pdev) {
    printk(KERN_INFO DRV_NAME ": driver removed\n");
    if (hw_addr)
        iounmap(hw_addr);
    pci_release_region(pdev, 0);
    pci_disable_device(pdev);
}

static struct pci_driver pci_drv = {
    .name = DRV_NAME,
    .id_table = pci_ids,
    .probe = pci_probe,
    .remove = pci_remove,
};


    /*Registers the driver with the PCI subsystem.

    When a matching PCI device is found, pci_probe() is called.*/
module_pci_driver(pci_drv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arya");
MODULE_DESCRIPTION("Custom PCI Driver with Valid Ethernet Tx Frame");

