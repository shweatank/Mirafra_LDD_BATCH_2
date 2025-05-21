#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

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
	{ 0xada84b28, "device_destroy" },
	{ 0xa9edd0d8, "class_destroy" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xc330ba9e, "gpio_to_desc" },
	{ 0x414dd1a, "gpiod_set_raw_value" },
	{ 0xd39e86b, "__register_chrdev" },
	{ 0x932aac8c, "class_create" },
	{ 0x1421f594, "device_create" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x9cab73ae, "gpiod_direction_output_raw" },
	{ 0x2aaa3156, "gpiod_direction_input" },
	{ 0x3e503b05, "gpiod_to_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xfe990052, "gpio_free" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2B69E69AB1DC9F497B9B147");
