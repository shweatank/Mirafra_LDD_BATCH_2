#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>

#define GPIO_SET_OFFSET       0x1C
#define GPIO_CLR_OFFSET       0x28
#define DMA_CHANNEL_OFFSET    0x100  // e.g., DMA channel 5 = BASE + 0x500
#define DMA_CHANNEL_NUM       5
#define DMA_CS                0x00
#define DMA_CONBLK_AD         0x04
#define GPIO_PIN              18

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

static int dma_gpio_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct resource *res;

    pr_info("DMA GPIO LED probe\n");

    // Map GPIO
    res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "gpio");
    gpio_base = devm_ioremap_resource(dev, res);
    if (IS_ERR(gpio_base))
        return PTR_ERR(gpio_base);

    // Map DMA
    res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dma");
    dma_base = devm_ioremap_resource(dev, res);
    if (IS_ERR(dma_base))
        return PTR_ERR(dma_base);

    // Allocate DMA-coherent memory
    cb = dma_alloc_coherent(dev, sizeof(struct dma_cb), &cb_handle, GFP_KERNEL);
    gpio_data = dma_alloc_coherent(dev, sizeof(uint32_t), &gpio_data_handle, GFP_KERNEL);

    if (!cb || !gpio_data) {
        dev_err(dev, "DMA memory allocation failed\n");
        return -ENOMEM;
    }

    *gpio_data = (1 << GPIO_PIN);

    cb->ti         = (1 << 26) | (1 << 0);
    cb->source_ad  = gpio_data_handle;
    cb->dest_ad    = res->start + GPIO_SET_OFFSET;
    cb->tx_len     = sizeof(uint32_t);
    cb->stride     = 0;
    cb->nextconbk  = 0;

    writel(1 << 31, dma_base + DMA_CS);
    udelay(10);
    writel(cb_handle, dma_base + DMA_CONBLK_AD);
    writel(0x10880001, dma_base + DMA_CS);

    msleep(500);
    writel(1 << GPIO_PIN, gpio_base + GPIO_CLR_OFFSET);

    return 0;
}

static int dma_gpio_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;

    writel(1 << GPIO_PIN, gpio_base + GPIO_CLR_OFFSET);

    if (cb)
        dma_free_coherent(dev, sizeof(struct dma_cb), cb, cb_handle);

    if (gpio_data)
        dma_free_coherent(dev, sizeof(uint32_t), gpio_data, gpio_data_handle);

    pr_info("DMA GPIO LED removed\n");
    return 0;
}

static const struct of_device_id dma_gpio_of_match[] = {
    { .compatible = "rpi,dma-gpio-blink" },
    { }
};
MODULE_DEVICE_TABLE(of, dma_gpio_of_match);

static struct platform_driver dma_gpio_driver = {
    .probe  = dma_gpio_probe,
    .remove = dma_gpio_remove,
    .driver = {
        .name           = "dma-gpio-blink",
        .of_match_table = dma_gpio_of_match,
    },
};

module_platform_driver(dma_gpio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant @ TechDhaba + modified by ChatGPT");
MODULE_DESCRIPTION("DMA-driven GPIO LED Blink using Platform Driver");

