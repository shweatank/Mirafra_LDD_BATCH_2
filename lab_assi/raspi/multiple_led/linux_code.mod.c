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
	{ 0xc3c21c78, "__platform_driver_register" },
	{ 0x6703e37e, "platform_driver_unregister" },
	{ 0x5454f7e8, "of_count_phandle_with_args" },
	{ 0xf99d1f32, "of_get_named_gpio_flags" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x882ce6fa, "gpio_to_desc" },
	{ 0x183a612f, "gpiod_direction_output_raw" },
	{ 0xedb6f3de, "kthread_create_on_node" },
	{ 0x56f38bce, "wake_up_process" },
	{ 0x32290c51, "_dev_info" },
	{ 0x52b3a0c2, "_dev_err" },
	{ 0xfe990052, "gpio_free" },
	{ 0x983e2d76, "kthread_stop" },
	{ 0xd1cb405a, "gpiod_set_raw_value" },
	{ 0x92997ed8, "_printk" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xf9a482f9, "msleep" },
	{ 0x8f80e6e5, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Ctechdhaba,multi-led-blink");
MODULE_ALIAS("of:N*T*Ctechdhaba,multi-led-blinkC*");

MODULE_INFO(srcversion, "4C7CC8080B5310951A571F7");
