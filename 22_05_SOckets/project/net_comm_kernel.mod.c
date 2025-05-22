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
	{ 0x92997ed8, "_printk" },
	{ 0x801168f2, "misc_deregister" },
	{ 0x983e2d76, "kthread_stop" },
	{ 0xc08ed1f6, "sock_release" },
	{ 0x882ce6fa, "gpio_to_desc" },
	{ 0xd1cb405a, "gpiod_set_raw_value" },
	{ 0xfe990052, "gpio_free" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x183a612f, "gpiod_direction_output_raw" },
	{ 0x66bb453b, "init_net" },
	{ 0x2b8084ef, "sock_create_kern" },
	{ 0x89bbd504, "kernel_bind" },
	{ 0xedb6f3de, "kthread_create_on_node" },
	{ 0x56f38bce, "wake_up_process" },
	{ 0x9d2a154d, "misc_register" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0x1b6314fd, "in_aton" },
	{ 0xc1af339b, "kernel_sendmsg" },
	{ 0x670ae423, "kmalloc_caches" },
	{ 0x26d9d21b, "kmalloc_trace" },
	{ 0xd55d04c9, "kernel_recvmsg" },
	{ 0x1000e51, "schedule" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0x37a0cba, "kfree" },
	{ 0x4829a47e, "memcpy" },
	{ 0xf9a482f9, "msleep" },
	{ 0xa7eedcc4, "call_usermodehelper" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x8f80e6e5, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "180E875986D75B4BFDB95FC");
