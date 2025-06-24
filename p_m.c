#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#define UART_BASE 0xFE201000
#define UART_DR   0x00
#define UART_FR   0x18
#define UART_IBRD 0x24
#define UART_FBRD 0x28
#define UART_LCRH 0x2C
#define UART_CR   0x30
#define UART_IMSC 0x38
#define UART_ICR  0x44

#define UART_FR_TXFF (1 << 5)
#define UART_FR_RXFE (1 << 4)
#define UART_FR_BUSY (1 << 3)

#define UART_LCRH_WLEN_8BIT (0x3 << 5)
#define UART_LCRH_FEN       (1 << 4)
#define UART_CR_UARTEN (1 << 0)
#define UART_CR_TXE    (1 << 8)
#define UART_CR_RXE    (1 << 9)

#define DRIVER_NAME "merged_uart_dma"
#define DEVICE_NAME "merged_uart_dma"
#define LED_GPIO 530
#define DMA_SIZE 1

struct rpi_uart_dev {
    struct cdev cdev;
    void __iomem *regs;
    dev_t dev_no;
    struct class *class;
    struct device *device;
};

static struct rpi_uart_dev *uart_dev;
static struct dma_chan *dma_chan;
static u8 *virt_buf;
static dma_addr_t dma_buf;

#define uart_readl(offset)      readl(uart_dev->regs + offset)
#define uart_writel(val, offset) writel(val, uart_dev->regs + offset)

static void blink_led(void)
{
    gpio_set_value(LED_GPIO, 1);
    msleep(100);
    gpio_set_value(LED_GPIO, 0);

    // Restart DMA for next byte
    start_dma_rx_transfer();
}

static void dma_callback(void *param)
{
   /* pr_info("DMA RX complete. Byte received: 0x%02x\n", virt_buf[0]);
    if (virt_buf[0] == '1')
        gpio_set_value(LED_GPIO, 1);
    else if (virt_buf[0] == '0')
        gpio_set_value(LED_GPIO, 0);*/
		blink_led();
}

static void rpi_uart_hw_init(void)
{
    uart_writel(0, UART_CR);
    uart_writel(0x7FF, UART_ICR);
    uart_writel(26, UART_IBRD);
    uart_writel(3, UART_FBRD);
    uart_writel(UART_LCRH_WLEN_8BIT | UART_LCRH_FEN, UART_LCRH);
    uart_writel(UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE, UART_CR);
    uart_writel(0, UART_IMSC);
}

static int rpi_uart_open(struct inode *inode, struct file *filp)
{
    filp->private_data = uart_dev;
    return 0;
}

static int rpi_uart_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t rpi_uart_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    // Not used in DMA mode.
    return 0;
}

static ssize_t rpi_uart_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    size_t i = 0;
    char tmp;

    while (i < count) {
        if (uart_readl(UART_FR) & UART_FR_TXFF)
            break;

        if (copy_from_user(&tmp, buf + i, 1))
            return -EFAULT;

        uart_writel(tmp, UART_DR);
        while (uart_readl(UART_FR) & UART_FR_BUSY)
            cpu_relax();
        i++;
    }

    return i;
}

static struct file_operations rpi_uart_fops = {
    .owner = THIS_MODULE,
    .open = rpi_uart_open,
    .release = rpi_uart_release,
    .read = rpi_uart_read,
    .write = rpi_uart_write,
};

/*static int start_dma_rx_transfer(void)
{
    struct dma_async_tx_descriptor *desc; //pointer to a dma descriptor structure that describes the transfer
    dma_cookie_t cookie; //used track its status

    desc = dmaengine_prep_slave_single(dma_chan, dma_buf, DMA_SIZE,
                                       DMA_DEV_TO_MEM, DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	//dma_buf - dma address of the destination buf
    if (!desc) {
        pr_err("DMA slave prep failed\n");
        return -EIO;
    }

    desc->callback = dma_callback;
    desc->callback_param = NULL;
    cookie = dmaengine_submit(desc);
    dma_async_issue_pending(dma_chan);

    return 0;
}*/
static int start_dma_rx_transfer(void)
{
    struct dma_slave_config config = {0};
    struct dma_async_tx_descriptor *desc;
    dma_cookie_t cookie;
    int ret;

    // Configure DMA slave (RX direction)
    config.direction = DMA_DEV_TO_MEM;
    config.src_addr = UART_BASE + UART_DR;
    config.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
    config.src_maxburst = 1;

    // Apply slave config to the DMA channel
    ret = dmaengine_slave_config(dma_chan, &config);
    if (ret) {
        pr_err("Failed to configure DMA slave: %d\n", ret);
        return ret;
    }

    // Prepare the DMA descriptor
    desc = dmaengine_prep_slave_single(dma_chan, dma_buf, DMA_SIZE,
                                       DMA_DEV_TO_MEM,
                                       DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
    if (!desc) {
        pr_err("Failed to prepare DMA slave transfer\n");
        return -EINVAL;
    }

    desc->callback = dma_callback;
    desc->callback_param = NULL;

    // Submit the DMA transfer
    cookie = dmaengine_submit(desc);
    if (dma_submit_error(cookie)) {
        pr_err("Failed to submit DMA transfer\n");
        return -EINVAL;
    }

    // Start the DMA engine
    dma_async_issue_pending(dma_chan);
    return 0;
}


static int __init rpi_uart_module_init(void)
{
    int ret;

    pr_info("Merged UART-DMA Module Init\n");

    uart_dev = kzalloc(sizeof(struct rpi_uart_dev), GFP_KERNEL);
    if (!uart_dev)
        return -ENOMEM;

    uart_dev->regs = ioremap(UART_BASE, 0x100);
	//maps physical i/o memory region(like uart registers) into a virtual address space that the kernel can access
    if (!uart_dev->regs) {
        pr_err("Failed to map UART registers\n");
        kfree(uart_dev);
        return -ENOMEM;
    }

    ret = alloc_chrdev_region(&uart_dev->dev_no, 0, 1, DEVICE_NAME);
	//dynamically allocate major number and register character device region in linux kernel
    if (ret)
        goto err_unmap;

    cdev_init(&uart_dev->cdev, &rpi_uart_fops);
    uart_dev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&uart_dev->cdev, uart_dev->dev_no, 1);
    if (ret)
        goto err_chrdev;

    uart_dev->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(uart_dev->class)) {
        ret = PTR_ERR(uart_dev->class);
        goto err_class;
    }

    uart_dev->device = device_create(uart_dev->class, NULL, uart_dev->dev_no, NULL, DEVICE_NAME);
	//null- no parent device, null- optional private driver data usually NULL
    if (IS_ERR(uart_dev->device)) {
        ret = PTR_ERR(uart_dev->device);
        goto err_device;
    }

    // Init UART HW
    rpi_uart_hw_init();

    // LED GPIO setup
    ret = gpio_request_one(LED_GPIO, GPIOF_OUT_INIT_LOW, "LED_GPIO");
    if (ret)
        goto err_gpio;

    // DMA buffer alloc
    virt_buf = dma_alloc_coherent(uart_dev->device, DMA_SIZE, &dma_buf, GFP_KERNEL); //NULL : parameter - dev
    if (!virt_buf) {
        pr_err("Failed to allocate DMA buffer\n");
        goto err_dma;
    }

    // Request DMA channel
    dma_chan = dma_request_chan(uart_dev->device, "rx"); // "rx" or platform-specific name
    if (IS_ERR(dma_chan)) {
        pr_err("Failed to request DMA channel\n");
        ret = PTR_ERR(dma_chan);
        goto err_dma_alloc;
    }

    // Start RX DMA
    ret = start_dma_rx_transfer();
    if (ret)
        goto err_dma_chan;

    pr_info("Merged UART-DMA Module loaded\n");
    return 0;

err_dma_chan:
    dma_release_channel(dma_chan);
err_dma_alloc:
    dma_free_coherent(NULL, DMA_SIZE, virt_buf, dma_buf);
err_dma:
    gpio_free(LED_GPIO);
err_gpio:
    device_destroy(uart_dev->class, uart_dev->dev_no);
err_device:
    class_destroy(uart_dev->class);
err_class:
    cdev_del(&uart_dev->cdev);
err_chrdev:
    unregister_chrdev_region(uart_dev->dev_no, 1);
err_unmap:
    iounmap(uart_dev->regs);
    kfree(uart_dev);
    return ret;
}

static void __exit rpi_uart_module_exit(void)
{
    gpio_set_value(LED_GPIO, 0);
    gpio_free(LED_GPIO);

    if (dma_chan)
        dma_release_channel(dma_chan);
    if (virt_buf)
        dma_free_coherent(NULL, DMA_SIZE, virt_buf, dma_buf);

    device_destroy(uart_dev->class, uart_dev->dev_no);
    class_destroy(uart_dev->class);
    cdev_del(&uart_dev->cdev);
    unregister_chrdev_region(uart_dev->dev_no, 1);
    iounmap(uart_dev->regs);
    kfree(uart_dev);

    pr_info("Merged UART-DMA Module unloaded\n");
}

module_init(rpi_uart_module_init);
module_exit(rpi_uart_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kavya KL");
MODULE_DESCRIPTION("Merged Custom UART and DMA-based LED control on Raspberry Pi 4B");

