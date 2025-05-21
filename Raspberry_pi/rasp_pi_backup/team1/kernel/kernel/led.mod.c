#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
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

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x983e2d76, "kthread_stop" },
	{ 0x882ce6fa, "gpio_to_desc" },
	{ 0xd1cb405a, "gpiod_set_raw_value" },
	{ 0xfe990052, "gpio_free" },
	{ 0x92997ed8, "_printk" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x183a612f, "gpiod_direction_output_raw" },
	{ 0xedb6f3de, "kthread_create_on_node" },
	{ 0x56f38bce, "wake_up_process" },
	{ 0x1b024b6c, "filp_open" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0x989e9207, "kernel_read" },
	{ 0xf9a482f9, "msleep" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0xd0575c87, "filp_close" },
	{ 0x8f80e6e5, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "BAE3DC59D42416A8D9F048D");
