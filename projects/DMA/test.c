#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>

#define BCM2711_GPIO_BASE     0xFE200000
#define BCM2711_DMA_BASE      0xFE007000
#define GPIO_SET_OFFSET       0x1C
#define GPIO_CLR_OFFSET       0x28
#define DMA_CHANNEL_OFFSET    0x100  // e.g., DMA channel 5 = BASE + 0x500
#define DMA_CHANNEL_NUM       5
#define DMA_CS                0x00
#define DMA_CONBLK_AD         0x04

#define DMA_BASE_PHYS(channel) (BCM2711_DMA_BASE + (channel * DMA_CHANNEL_OFFSET))
#define GPIO_PIN              530

struct dma_cb {
    uint32_t ti;
    uint32_t source_ad;
    uint32_t dest_ad;
    uint32_t tx_len;
    uint32_t stride;
    uint32_t nextconbk;
    uint32_t reserved[2];
};

static void __iomem *gpio_base;
static void __iomem *dma_base;
static struct dma_cb *cb;
static dma_addr_t cb_handle;
static uint32_t *gpio_data;
static dma_addr_t gpio_data_handle;

static int __init dma_gpio_init(void)
{
    pr_info("DMA GPIO LED init\n");

    // ioremap GPIO and DMA regions
    gpio_base = ioremap(BCM2711_GPIO_BASE, 0x100);
    dma_base = ioremap(DMA_BASE_PHYS(DMA_CHANNEL_NUM), 0x100);

    if (!gpio_base || !dma_base) {
        pr_err("ioremap failed\n");
        return -ENOMEM;
    }

    // Allocate DMA-coherent memory for CB and data
    cb = dma_alloc_coherent(NULL, sizeof(struct dma_cb), &cb_handle, GFP_KERNEL);
    gpio_data = dma_alloc_coherent(NULL, sizeof(uint32_t), &gpio_data_handle, GFP_KERNEL);

    if (!cb || !gpio_data) {
        pr_err("DMA memory allocation failed\n");
        return -ENOMEM;
    }

    // Prepare data to write: set bit 18
    *gpio_data = (1 << GPIO_PIN);

    // Prepare Control Block
    cb->ti         = (1 << 26) |  // No wide bursts
                     (1 << 0);    // Enable
    cb->source_ad  = gpio_data_handle;
    cb->dest_ad    = BCM2711_GPIO_BASE + GPIO_SET_OFFSET;
    cb->tx_len     = sizeof(uint32_t);
    cb->stride     = 0;
    cb->nextconbk  = 0;

    // Clear DMA channel (reset)
    writel(1 << 31, dma_base + DMA_CS);
    udelay(10);

    // Write CB address
    writel(cb_handle, dma_base + DMA_CONBLK_AD);

    // Enable DMA
    writel(0x10880001, dma_base + DMA_CS); // Active + end + clear

    msleep(500);

    // Turn off LED via direct write
    writel(1 << GPIO_PIN, gpio_base + GPIO_CLR_OFFSET);

    return 0;
}

static void __exit dma_gpio_exit(void)
{
    writel(1 << GPIO_PIN, gpio_base + GPIO_CLR_OFFSET);

    // Free DMA memory
    if (cb)
        dma_free_coherent(NULL, sizeof(struct dma_cb), cb, cb_handle);

    if (gpio_data)
        dma_free_coherent(NULL, sizeof(uint32_t), gpio_data, gpio_data_handle);

    if (gpio_base)
        iounmap(gpio_base);

    if (dma_base)
        iounmap(dma_base);

    pr_info("DMA GPIO LED exit\n");
}

module_init(dma_gpio_init);
module_exit(dma_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant @ TechDhaba");
MODULE_DESCRIPTION("DMA-driven GPIO LED Blink for BCM2711");
