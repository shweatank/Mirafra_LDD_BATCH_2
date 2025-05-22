#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>

#define I2C1_BASE_PHYS 0xFE804000
#define I2C_REG_SIZE   0x20  // Enough for basic I2C registers

// I2C register offsets
#define BSC_C   0x00  // Control
#define BSC_S   0x04  // Status
#define BSC_DLEN 0x08 // Data length
#define BSC_A   0x0C  // Slave address
#define BSC_FIFO 0x10 // FIFO data
#define BSC_DIV 0x14  // Clock divider
#define BSC_DEL 0x18  // Data delay
#define BSC_CLKT 0x1C // Clock stretch timeout

// Bit flags
#define BSC_C_I2CEN   (1 << 15)
#define BSC_C_ST      (1 << 7)
#define BSC_S_DONE    (1 << 1)

static void __iomem *i2c_base = NULL;

static int __init i2c_driver_init(void)
{
    pr_info("Initializing I2C driver with ioremap\n");

    i2c_base = ioremap(I2C1_BASE_PHYS, I2C_REG_SIZE);
    if (!i2c_base) {
        pr_err("Failed to remap I2C registers\n");
        return -ENOMEM;
    }

    // Example: Send a single byte to slave 0x50
    iowrite32(0x50, i2c_base + BSC_A);       // Set slave address
    iowrite32(1, i2c_base + BSC_DLEN);       // Set data length
    iowrite32(0x42, i2c_base + BSC_FIFO);    // Write 1 byte into FIFO
    iowrite32(BSC_C_I2CEN | BSC_C_ST, i2c_base + BSC_C);  // Enable + start

    // Wait for completion (polling for example purposes)
    while (!(ioread32(i2c_base + BSC_S) & BSC_S_DONE)) {
        cpu_relax();  // Wait
    }

    pr_info("I2C write done to slave 0x50\n");
    return 0;
}

static void __exit i2c_driver_exit(void)
{
    if (i2c_base) {
        iounmap(i2c_base);
        pr_info("I2C driver unmapped and unloaded\n");
    }
}

module_init(i2c_driver_init);
module_exit(i2c_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishnu.k");
MODULE_DESCRIPTION("Minimal I2C driver using ioremap on Raspberry Pi 4");
