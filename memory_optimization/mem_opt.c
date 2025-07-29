#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>          // for si_meminfo
#include <linux/slab.h>        // for kmalloc and kfree

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Keerthi");
MODULE_DESCRIPTION("Memory optimization and info kernel module");

static char *leaked_mem = NULL;
static char *normal_mem = NULL;

static int __init mem_opt_init(void)
{
    struct sysinfo i;

    printk(KERN_INFO "Memory Optimization Module Loaded\n");

    // Allocate memory
    leaked_mem = kmalloc(128, GFP_KERNEL);  // Leaked intentionally (not freed)
    if (!leaked_mem)
        printk(KERN_ERR "Failed to allocate leaked_mem\n");

    normal_mem = kmalloc(128, GFP_KERNEL);  // Will be freed
    if (!normal_mem)
        printk(KERN_ERR "Failed to allocate normal_mem\n");

    // Use memory
    snprintf(normal_mem, 128, "Hello from kernel space!\n");

    // Print memory info
    si_meminfo(&i);
    printk(KERN_INFO "Total RAM: %lu MB", (i.totalram * i.mem_unit) >> 20);
    printk(KERN_INFO "Free RAM:  %lu MB", (i.freeram * i.mem_unit) >> 20);
    printk(KERN_INFO "Buffered RAM: %lu MB", (i.bufferram * i.mem_unit) >> 20);

    return 0;
}

static void __exit mem_opt_exit(void)
{
    printk(KERN_INFO "Exiting Memory Optimization Module\n");

    // Free normal allocation
    if (normal_mem)
        kfree(normal_mem);

    // leaked_mem is NOT freed on purpose to simulate a leak
}

module_init(mem_opt_init);
module_exit(mem_opt_exit);

