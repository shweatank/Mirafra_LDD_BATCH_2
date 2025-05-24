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
	{ 0xb18b6df6, "i2c_get_adapter" },
	{ 0x15c21d2a, "i2c_new_client_device" },
	{ 0x60b678d1, "i2c_register_driver" },
	{ 0xd39fd4a8, "i2c_put_adapter" },
	{ 0x122c3a7e, "_printk" },
	{ 0x14d1e1b0, "i2c_unregister_device" },
	{ 0x464601cd, "i2c_del_driver" },
	{ 0xbc9ac584, "i2c_transfer_buffer_flags" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xf9a482f9, "msleep" },
	{ 0x39ff040a, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("i2c:ETX_OLED");

MODULE_INFO(srcversion, "B4D1C9D191A0460A84D01B2");
