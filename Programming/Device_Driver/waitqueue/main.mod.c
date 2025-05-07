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
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x21be7f7b, "cdev_init" },
	{ 0x54c057fa, "cdev_add" },
	{ 0x932aac8c, "class_create" },
	{ 0x1421f594, "device_create" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0x78445a7c, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xa9edd0d8, "class_destroy" },
	{ 0xada84b28, "device_destroy" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0xe2964344, "__wake_up" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "E0401BBE04FD3D9A545A536");
