#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include "keyboard.h"  // You must define the key macros here

#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA_PORT 0x60
#define DEVICE_NAME "keyboard_counter"
#define MAJOR_NUM 101

static int key_counts[256] = {0};
static char buffer[1024];
static int dev_opens = 0;

// Key structure
struct key_info {
    unsigned char scancode;
    const char *name;
};

#define ARRAY_SIZES(arr) (sizeof(arr)/ sizeof(arr[0]))
//Array of keyboard keys
static struct key_info keys[] = {
    {A, "A"}, {B, "B"}, {C, "C"}, {D, "D"}, {E, "E"}, {F, "F"},
    {G, "G"}, {H, "H"}, {I, "I"}, {J, "J"}, {K, "K"}, {L, "L"},
    {M, "M"}, {N, "N"}, {O, "O"}, {P, "P"}, {Q, "Q"}, {R, "R"},
    {S, "S"}, {T, "T"}, {U, "U"}, {V, "V"}, {W, "W"}, {X, "X"},
    {Y, "Y"}, {Z, "Z"},

    {one_1, "1"}, {two_2, "2"}, {three_3, "3"}, {four_4, "4"}, {five_5, "5"},
    {six_6, "6"}, {seven_7, "7"}, {eight_8, "8"}, {nine_9, "9"}, {zero_0, "0"},

    {ENTER, "ENTER"}, {ESCAPE, "ESCAPE"}, {BACKSPACE, "BACKSPACE"},
    {SHIFT_LEFT, "SHIFT_LEFT"}, {SHIFT_RIGHT, "SHIFT_RIGHT"},
    {CTRL_LEFT, "CTRL_LEFT"}, {ALT_LEFT, "ALT_LEFT"}, {CAPS_LOCK, "CAPS_LOCK"},

    {F1, "F1"}, {F2, "F2"}, {F3, "F3"}, {F4, "F4"}, {F5, "F5"},
    {F6, "F6"}, {F7, "F7"}, {F8, "F8"}, {F9, "F9"}, {F10, "F10"},
    {F11, "F11"}, {F12, "F12"},

    {SPACEBAR, "SPACEBAR"}, {DELETE, "DELETE"}, {END, "END"},
    {PAGE_DOWN, "PAGE_DOWN"}, {PAGE_UP, "PAGE_UP"}, {HOME, "HOME"},
    {ARROW_LEFT, "ARROW_LEFT"}, {ARROW_RIGHT, "ARROW_RIGHT"},
    {ARROW_UP, "ARROW_UP"}, {ARROW_DOWN, "ARROW_DOWN"}, {NUM_LOCK, "NUM_LOCK"}
};


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Interrupt handler
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    key_counts[scancode]++;
    return IRQ_HANDLED;
}

// Device read
static ssize_t dev_read(struct file *file, char __user *user_buff, size_t len, loff_t *offset)
{
    int i;
    size_t n = 0;

    n += snprintf(buffer + n, sizeof(buffer) - n, "Key press Summary:\n");
    for (i = 0; i < ARRAY_SIZES(keys); i++) {
        unsigned char scancode = keys[i].scancode;
        int count = key_counts[scancode];
        if (count > 0) {
            n += snprintf(buffer + n, sizeof(buffer) - n, "%s is pressed %d times\n", keys[i].name, count);
        }
    }

    if (*offset >= n)
        return 0;

    if (len > n - *offset)
        len = n - *offset;

    if (copy_to_user(user_buff, buffer + *offset, len))
        return -EFAULT;

    *offset += len;
    return len;
}

// Device open
static int dev_open(struct inode *inode, struct file *file)
{
    if (dev_opens)
        return -EBUSY;
    dev_opens++;
    return 0;
}

// Device release
static int dev_release(struct inode *inode, struct file *file)
{
    dev_opens = 0;
    return 0;
}

// File operations
static struct file_operations fops = {
    .read = dev_read,
    .open = dev_open,
    .release = dev_release,
};

// Module initialization
static int __init keyboard_counter_init(void)
{
    int ret;

    ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (ret < 0) {
        printk(KERN_ERR "Failed to register the char device.\n");
        return ret;
    }

    ret = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, IRQF_SHARED, DEVICE_NAME, (void *)keyboard_interrupt_handler);
    if (ret != 0) {
        unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
        printk(KERN_ERR "Failed to request IRQ.\n");
        return ret;
    }

    printk(KERN_INFO "Keyboard counter module loaded. Major number: %d\n", MAJOR_NUM);
    return 0;
}

// Module cleanup
static void __exit keyboard_counter_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)keyboard_interrupt_handler);
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "Keyboard counter module unloaded.\n");
}

module_init(keyboard_counter_init);
module_exit(keyboard_counter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishnuvardhan.k");
MODULE_DESCRIPTION("Keyboard press counter with readable key names");
