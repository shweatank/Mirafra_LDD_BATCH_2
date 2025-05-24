// spi_driver_ioremap.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define SPI0_BASE_ADDR 0xFE204000
#define SPI_REG_SIZE 32

#define BCM2835_SPI_CS 0x00
#define BCM2835_SPI_FIFO 0x04
#define BCM2835_SPI_CLK 0x08
#define BCM2835_SPI_DLEN 0x0C
#define BCM2835_SPI_LTOH 0x10
#define BCM2835_SPI_DC 0x14

static void __iomem *ctrl_status, *fifo, *clk, *dlen, *lossi_toh, *dma_ctrl;

static int __init spi_driver_init(void)
{
    printk(KERN_INFO "Initializing SPI Driver with ioremap\n");

    // Map physical memory into virtual address space
    ctrl_status = ioremap(SPI0_BASE_ADDR + BCM2835_SPI_CS, SPI_REG_SIZE);
    if (!ctrl_status) {
        printk(KERN_ERR "Failed to remap CS register\n");
        return -ENOMEM;
    }

    fifo = ioremap(SPI0_BASE_ADDR + BCM2835_SPI_FIFO, SPI_REG_SIZE);
    if (!fifo) {
        printk(KERN_ERR "Failed to remap FIFO register\n");
        iounmap(ctrl_status);
        return -ENOMEM;
    }

    clk = ioremap(SPI0_BASE_ADDR + BCM2835_SPI_CLK, SPI_REG_SIZE);
    if (!clk) {
        printk(KERN_ERR "Failed to remap CLK register\n");
        iounmap(fifo);
        iounmap(ctrl_status);
        return -ENOMEM;
    }

    dlen = ioremap(SPI0_BASE_ADDR + BCM2835_SPI_DLEN, SPI_REG_SIZE);
    if (!dlen) {
        printk(KERN_ERR "Failed to remap DLEN register\n");
        iounmap(clk);
        iounmap(fifo);
        iounmap(ctrl_status);
        return -ENOMEM;
    }

    lossi_toh = ioremap(SPI0_BASE_ADDR + BCM2835_SPI_LTOH, SPI_REG_SIZE);
    if (!lossi_toh) {
        printk(KERN_ERR "Failed to remap LTOH register\n");
        iounmap(dlen);
        iounmap(clk);
        iounmap(fifo);
        iounmap(ctrl_status);
        return -ENOMEM;
    }

    dma_ctrl = ioremap(SPI0_BASE_ADDR + BCM2835_SPI_DC, SPI_REG_SIZE);
    if (!dma_ctrl) {
        printk(KERN_ERR "Failed to remap DC register\n");
        iounmap(lossi_toh);
        iounmap(dlen);
        iounmap(clk);
        iounmap(fifo);
        iounmap(ctrl_status);
        return -ENOMEM;
    }

    printk(KERN_INFO "SPI Driver initialized with ioremap\n");
    return 0;
}

static void __exit spi_driver_exit(void)
{
    printk(KERN_INFO "Exiting SPI Driver\n");

    iounmap(dma_ctrl);
    iounmap(lossi_toh);
    iounmap(dlen);
    iounmap(clk);
    iounmap(fifo);
    iounmap(ctrl_status);

    printk(KERN_INFO "SPI Driver exited\n");
}

module_init(spi_driver_init);
module_exit(spi_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("SPI Driver using ioremap for Raspberry Pi 4B");

