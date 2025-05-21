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
	{ 0x801168f2, "misc_deregister" },
	{ 0x882ce6fa, "gpio_to_desc" },
	{ 0xd1cb405a, "gpiod_set_raw_value" },
	{ 0xfe990052, "gpio_free" },
	{ 0x92997ed8, "_printk" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x183a612f, "gpiod_direction_output_raw" },
	{ 0x9d2a154d, "misc_register" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x8f80e6e5, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F7C2D41E856660522847F44");
