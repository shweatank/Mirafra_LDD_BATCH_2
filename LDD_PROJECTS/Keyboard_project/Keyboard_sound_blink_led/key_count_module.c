#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/wait.h>


#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA_PORT 0x60
#define DEVICE_NAME "key_count"


static DECLARE_WAIT_QUEUE_HEAD(wq);

static int major;
static int key_counts[256];
static char key_buffer = 0;
static int data_ready = 0;

static const char* scancode_names[256] = {
    [0x01] = "Esc",
    [0x02] = "1", [0x03] = "2", [0x04] = "3", [0x05] = "4", [0x06] = "5",
    [0x07] = "6", [0x08] = "7", [0x09] = "8", [0x0A] = "9", [0x0B] = "0",
    [0x0C] = "-", [0x0D] = "=", [0x0E] = "Backspace",
    [0x0F] = "Tab",
    [0x10] = "Q", [0x11] = "W", [0x12] = "E", [0x13] = "R", [0x14] = "T",
    [0x15] = "Y", [0x16] = "U", [0x17] = "I", [0x18] = "O", [0x19] = "P",
    [0x1A] = "[", [0x1B] = "]", [0x1C] = "Enter",
    [0x1D] = "Left Ctrl",
    [0x1E] = "A", [0x1F] = "S", [0x20] = "D", [0x21] = "F", [0x22] = "G",
    [0x23] = "H", [0x24] = "J", [0x25] = "K", [0x26] = "L",
    [0x27] = ";", [0x28] = "'", [0x29] = "`",
    [0x2A] = "Left Shift",
    [0x2B] = "\\",
    [0x2C] = "Z", [0x2D] = "X", [0x2E] = "C", [0x2F] = "V", [0x30] = "B",
    [0x31] = "N", [0x32] = "M", [0x33] = ",", [0x34] = ".", [0x35] = "/",
    [0x36] = "Right Shift",
    [0x37] = "Keypad *",
    [0x38] = "Left Alt",
    [0x39] = "Space",
    [0x3A] = "Caps Lock",
    [0x3B] = "F1", [0x3C] = "F2", [0x3D] = "F3", [0x3E] = "F4",
    [0x3F] = "F5", [0x40] = "F6", [0x41] = "F7", [0x42] = "F8",
    [0x43] = "F9", [0x44] = "F10", [0x57] = "F11", [0x58] = "F12",
    [0x48] = "Up Arrow", [0x50] = "Down Arrow"
};
// IRQ handler
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id) {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    // if key pressed, it counts its frequency 
    if (scancode < 256 && (scancode & 0x80) == 0)
	key_counts[scancode]++;
    // Map scancode to lowercase a-z (0x1E to 0x32)
    char key_char = 0;
    switch (scancode) {
	case 0x1E: key_char = 'a'; break;
	case 0x30: key_char = 'b'; break;
	case 0x2E: key_char = 'c'; break;
	case 0x20: key_char = 'd'; break;
	case 0x12: key_char = 'e'; break;
	case 0x21: key_char = 'f'; break;
	case 0x22: key_char = 'g'; break;
	case 0x23: key_char = 'h'; break;
	case 0x17: key_char = 'i'; break;
	case 0x24: key_char = 'j'; break;
	case 0x25: key_char = 'k'; break;
	case 0x26: key_char = 'l'; break;
	case 0x32: key_char = 'm'; break;
	case 0x31: key_char = 'n'; break;
	case 0x18: key_char = 'o'; break;
	case 0x19: key_char = 'p'; break;
	case 0x10: key_char = 'q'; break;
	case 0x13: key_char = 'r'; break;
	case 0x1F: key_char = 's'; break;
	case 0x14: key_char = 't'; break;
	case 0x16: key_char = 'u'; break;
	case 0x2F: key_char = 'v'; break;
	case 0x11: key_char = 'w'; break;
	case 0x2D: key_char = 'x'; break;
	case 0x15: key_char = 'y'; break;
	case 0x2C: key_char = 'z'; break;
	default: break;
    }
    if (key_char != 0) {
        key_buffer = key_char;
        data_ready = 1;
        wake_up_interruptible(&wq);
    }

   /* // If valid key, write to /tmp/key_audio_trigger
    if (key_char != 0) {
	struct file *f;
	short pos = 0;

	f = filp_open("/tmp/key_audio_trigger", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (!IS_ERR(f)) {
	    kernel_write(f, &key_char, 1, &pos);
	    filp_close(f, NULL);
	}
    }*/
    return IRQ_HANDLED;
}

// Read implementation
static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char *kbuf;
    int i, pos = 0;

    if (*off > 0)
	return 0;

    kbuf = kmalloc(4096, GFP_KERNEL);
    if (!kbuf)
	return -ENOMEM;

    pos += snprintf(kbuf + pos, 4096 - pos, "Key Press Count:\n");
    for (i = 0; i < 256; i++) {
	if (key_counts[i] > 0) {
	    const char *name = scancode_names[i];
	    if (name)
		pos += snprintf(kbuf + pos, 4096 - pos, "%s pressed %d times\n", name, key_counts[i]);
	    else
		pos += snprintf(kbuf + pos, 4096 - pos, "Scancode 0x%02x pressed %d times\n", i, key_counts[i]);
	}
    }

    if (copy_to_user(buf, kbuf, pos)) {
	kfree(kbuf);
	return -EFAULT;
    }

    *off += pos;
    kfree(kbuf);

    // For audio function in user space to avoid kernel freeze, using waitqueue
    int ret;

    ret = wait_event_interruptible(wq, data_ready != 0);
    if (ret)
        return -ERESTARTSYS;

    // Now do file write here if needed (safe context)
    {
        struct file *f;
        loff_t pos = 0;

        f = filp_open("/tmp/key_audio_trigger", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (!IS_ERR(f)) {
            kernel_write(f, &key_buffer, 1, &pos);
            filp_close(f, NULL);
        }
    }

    if (len < 1)
        return 0;

    if (copy_to_user(buf, &key_buffer, 1))
        return -EFAULT;

    data_ready = 0;
    return pos;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
};

static int __init keycount_init(void) {
    int result;

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
	pr_err("Failed to register character device\n");
	return major;
    }

    pr_info("key_count device registered with major %d\n", major);

    result = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, IRQF_SHARED, DEVICE_NAME, (void *)(keyboard_interrupt_handler));
    if (result) {
	unregister_chrdev(major, DEVICE_NAME);
	pr_err("Failed to register IRQ\n");
	return result;
    }

    pr_info("Keyboard interrupt handler registered\n");
    return 0;
}

static void __exit keycount_exit(void) {
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("key_count module unloaded\n");
}

module_init(keycount_init);
module_exit(keycount_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kuruba Mohan");
MODULE_DESCRIPTION("Counts keyboard key presses using IRQ with full key mapping");
