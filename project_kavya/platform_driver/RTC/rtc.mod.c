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
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0x356461c8, "rtc_time64_to_tm" },
	{ 0x84bea2dc, "devm_rtc_device_register" },
	{ 0x3a1b27aa, "_dev_err" },
	{ 0x81ef79db, "platform_driver_unregister" },
	{ 0x161bea05, "platform_device_unregister" },
	{ 0x122c3a7e, "_printk" },
	{ 0x59c3df8, "platform_device_register_full" },
	{ 0xfc802b99, "__platform_driver_register" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x3b4cf56c, "_dev_info" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x4f2250ba, "rtc_tm_to_time64" },
	{ 0x7a95e5ae, "do_settimeofday64" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xc6227e48, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "7305F7AE716AD267D32FDEF");
