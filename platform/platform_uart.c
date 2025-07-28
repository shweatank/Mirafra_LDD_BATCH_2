#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>//copy to user
#include <linux/io.h>//file operations 
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define UART_BASE 0xFE201000 //uart memory mapped reg 
#define UART_DR   0x00
#define UART_FR   0x18
#define UART_IBRD 0x24
#define UART_FBRD 0x28
#define UART_LCRH 0x2C
#define UART_CR   0x30
#define UART_IMSC 0x38
#define UART_ICR  0x44

#define UART_FR_TXFF (1 << 5) //uart flag reg defines status of uart whether uart is ready,bc
#define UART_FR_RXFE (1 << 4) //bit pos of status flag in the UART flag reg
#define UART_FR_BUSY (1 << 3)

#define UART_LCRH_WLEN_8BIT (0x3 << 5) // to configure line ctrl reg
#define UART_LCRH_FEN       (1 << 4)

#define UART_CR_UARTEN (1 << 0)
#define UART_CR_TXE    (1 << 8)
#define UART_CR_RXE    (1 << 9)

#define DRIVER_NAME "rpi_uart"//registering the driver with kernel(register chrdev)
#define DEVICE_NAME "rpi_uart"//device file name /dev/rpi_uart which allows the user progrm to interact with device
#define BUFFER_SIZE 1024 //size of internal buffer

//#define LED_GPIO 18//gipochip in linux kernel that handles all 32 gpio on rasperrypi & gpio no starts from (base) no is 512
#define LED_GPIO 530 //physical pin 12 i.8 gpio is 18

struct rpi_uart_dev {
    struct cdev cdev; //kernel struct
    void __iomem *regs;// 	A pointer to mem mapped i/o region   ioremap(phys_addr, size)
    dev_t dev_no;
    struct class *class;// class_create which will create sysfs class helps user space tools like udev to create device file
    struct device *device; //dv create will create a dev file /dev/rpi_uart by intergrating with udev 
};

static struct rpi_uart_dev *uart_dev;

#define uart_readl(offset)      readl(uart_dev->regs + offset) //ioremap return virtual add of uart base addrs that is storef in uart_dev->regs + offset  will be the actual virtual address,
#define uart_writel(val, offset) writel(val, uart_dev->regs + offset) i.e acessing phsyical mem via virtual address 

static void rpi_uart_hw_init(void) //initialising or setting uart hardware
{
    uart_writel(0, UART_CR);
    uart_writel(0x7FF, UART_ICR); //clearing all the interupt flag ensuring a clean start
                                      // Baud Rate Divisor = UART_CLK / (16 × BaudRate)
                                    // = 48,000,000 / (16 × 115200) ≈ 26.04
    uart_writel(26, UART_IBRD); //integer baud rate divisor
    uart_writel(3, UART_FBRD);//fractional baud rate divisor(round(64×0.041666)=round(2.6666)=3)
    uart_writel(UART_LCRH_WLEN_8BIT | UART_LCRH_FEN, UART_LCRH);
    uart_writel(UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE, UART_CR);
    uart_writel(0, UART_IMSC); //Interrupt Mask Set/Clear Register //maskig all the uart interupts disabling all uart interupts
}

static int rpi_uart_open(struct inode *inode, struct file *filp)// Called when the device file is opened (e.g. /dev/rpi_uart)
{
    filp->private_data = uart_dev;// Store pointer to our device data in file's private_data
    return 0;
}

static int rpi_uart_release(struct inode *inode, struct file *filp)//Called when the device file is closed
/
{
    return 0;
}

static void blink_led(void)
{
    gpio_set_value(LED_GPIO, 1);
    mdelay(50)// Wait 50 milliseconds
    gpio_set_value(LED_GPIO, 0);
}

static ssize_t rpi_uart_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)// Called when user reads from the device file
{
    size_t i = 0;
    char tmp;
// Check if Receive FIFO is empty
    while (i < count) {
        if (uart_readl(UART_FR) & UART_FR_RXFE) {
            if (i == 0)
                break ;//exit // No data available, return 0
            else
                return i;// Return number of bytes read so far
        }

        tmp = (char)(uart_readl(UART_DR) & 0xFF);// Read a 1byte from UART data register
        blink_led(); // Blink on each received byte
        if (copy_to_user(buf + i, &tmp, 1))  // Copy the byte to user buffer
       
            return -EFAULT;
        i++; // Move to next byte
    }

    return i;
}

static ssize_t rpi_uart_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)// Called when user writes to the device file
{
    size_t i = 0;
    char tmp;

    while (i < count) {
        if (uart_readl(UART_FR) & UART_FR_TXFF) // check If Transmit FIFO is full, stop or wait
        {
            if (i == 0) // FIFO is full, no data written
                break;
            else
                return i;// Return number of bytes written managed to write b4 tx got full
        }

        if (copy_from_user(&tmp, buf + i, 1)) // Copy one byte from user-space to tmp
        
            return -EFAULT;

        uart_writel(tmp, UART_DR); // Write 1byte to UART data register
       
        while (uart_readl(UART_FR) & UART_FR_BUSY) // Wait until UART finishes sending the byte
        
            cpu_relax();// Avoid CPU spinning too hard waiting b4 sedning any data
        i++; //move to nxt byte
    }

    return i;
}

static struct file_operations rpi_uart_fops = //1st
{
    .owner = THIS_MODULE,
    .open = rpi_uart_open,
    .release = rpi_uart_release,
    .read = rpi_uart_read,
    .write = rpi_uart_write,
};

static int __init rpi_uart_module_init(void) //This function runs when the kernel module is loaded (insmod)
{
    int ret;

    uart_dev = kzalloc(sizeof(struct rpi_uart_dev), GFP_KERNEL); //kzalloc allocates memory and sets it to zero.
    if (!uart_dev)
        return -ENOMEM;

    uart_dev->regs = ioremap(UART_BASE, 0x100); //    // Map UART hardware registers to virtual memory (MMIO) //size of mem region we need to map
    if (!uart_dev->regs) {
        pr_err("Failed to ioremap UART registers\n");
        kfree(uart_dev); //If it fails, we log and clean up memory.
        return -ENOMEM;
    }

    ret = alloc_chrdev_region(&uart_dev->dev_no, 0, 1, DEVICE_NAME); // // Dynamically allocate a character device region (major and minor number)
    if (ret)
        return ret;

    cdev_init(&uart_dev->cdev, &rpi_uart_fops);// Initialize and register cdev (character device object)
    uart_dev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&uart_dev->cdev, uart_dev->dev_no, 1); //registers a vhar dev
    if (ret)
        goto err_cdev;

    uart_dev->class = class_create(DEVICE_NAME); //sysfs/class/rpi_uart
    if (IS_ERR(uart_dev->class)) {
        ret = PTR_ERR(uart_dev->class);
        goto err_class;
    }

    uart_dev->device = device_create(uart_dev->class, NULL, uart_dev->dev_no, NULL, DEVICE_NAME);
    if (IS_ERR(uart_dev->device)) {
        ret = PTR_ERR(uart_dev->device);
        goto err_dev;
    }

    rpi_uart_hw_init();

    ret = gpio_request(LED_GPIO, "LED18");
    if (ret) {
        pr_err("LED GPIO %d request failed\n", LED_GPIO);
        goto err_gpio;
    }

    ret = gpio_direction_output(LED_GPIO, 0);
    if (ret) {
        pr_err("LED GPIO direction set failed\n");
        gpio_free(LED_GPIO);
        goto err_gpio;
    }

    pr_info("RPi UART driver from ARYA loaded successfully\n");
    return 0;
//If something fails mid-way, these labels ensure partial work is cleaned up properly.
err_gpio:
    device_destroy(uart_dev->class, uart_dev->dev_no);
err_dev:
    class_destroy(uart_dev->class);
err_class:
    cdev_del(&uart_dev->cdev);
err_cdev:
    unregister_chrdev_region(uart_dev->dev_no, 1);//freeing the allocated range of char device numbers(major minor)
    iounmap(uart_dev->regs);
    kfree(uart_dev);   // Free device struct memory
    return ret;
}

static void __exit rpi_uart_module_exit(void)
{
    gpio_set_value(LED_GPIO, 0);
    gpio_free(LED_GPIO);
    device_destroy(uart_dev->class, uart_dev->dev_no);
    class_destroy(uart_dev->class);
    cdev_del(&uart_dev->cdev);
    unregister_chrdev_region(uart_dev->dev_no, 1);
    iounmap(uart_dev->regs);
    kfree(uart_dev);
    pr_info("RPi UART driver from ARYA unloaded\n");
}

module_init(rpi_uart_module_init); //2nd //called when the module is inserted
module_exit(rpi_uart_module_exit); //called when module is unloaded 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ARYA");
MODULE_DESCRIPTION("Custom UART driver for Raspberry Pi 4B with GPIO18 LED blink");









