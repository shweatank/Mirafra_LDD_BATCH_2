#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0x92997ed8, "_printk" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xab6a9a6c, "gpio_to_desc" },
	{ 0x1987d9a3, "gpiod_direction_output_raw" },
	{ 0xe6fa52e0, "kthread_create_on_node" },
	{ 0xfe990052, "gpio_free" },
	{ 0x7294b250, "wake_up_process" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x74f180ad, "gpiod_set_raw_value" },
	{ 0x5f754e5a, "memset" },
	{ 0xe5aa10e2, "filp_open" },
	{ 0x824fa3b6, "kernel_write" },
	{ 0xf9a482f9, "msleep" },
	{ 0x7e70e82f, "kernel_read" },
	{ 0x5f9e4317, "filp_close" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x8a3dbe73, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2902E3847F956A42D055538");
