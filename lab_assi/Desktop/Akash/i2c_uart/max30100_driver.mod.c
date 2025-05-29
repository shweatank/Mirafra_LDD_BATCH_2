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
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xd3cfb107, "i2c_transfer" },
	{ 0x92997ed8, "_printk" },
	{ 0xf9a482f9, "msleep" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x424d82fe, "i2c_del_driver" },
	{ 0x983e2d76, "kthread_stop" },
	{ 0x5a9c7cd0, "i2c_smbus_read_byte_data" },
	{ 0xedb6f3de, "kthread_create_on_node" },
	{ 0x56f38bce, "wake_up_process" },
	{ 0x8f80e6e5, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("i2c:max30102");

MODULE_INFO(srcversion, "FF9D184D936666DF5E3AC35");
