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
	{ 0x440cbd26, "i2c_register_driver" },
	{ 0x2cbf9d63, "i2c_smbus_write_i2c_block_data" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x52b3a0c2, "_dev_err" },
	{ 0xabb51d4f, "i2c_smbus_read_i2c_block_data" },
	{ 0x5f0bbf65, "devm_rtc_device_register" },
	{ 0x32290c51, "_dev_info" },
	{ 0x424d82fe, "i2c_del_driver" },
	{ 0x8f80e6e5, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cmaxim,ds3231");
MODULE_ALIAS("of:N*T*Cmaxim,ds3231C*");
MODULE_ALIAS("i2c:ds3231");

MODULE_INFO(srcversion, "2748E0453BD59BEA44A45A5");
