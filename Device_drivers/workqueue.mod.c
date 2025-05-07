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
	{ 0xf9a482f9, "msleep" },
	{ 0x37a0cba, "kfree" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x2cfdf4d6, "cdev_init" },
	{ 0x93a12ffd, "cdev_add" },
	{ 0xbe3c12ff, "__class_create" },
	{ 0xa791088b, "device_create" },
	{ 0x49cd25ed, "alloc_workqueue" },
	{ 0x2c3241c5, "device_destroy" },
	{ 0x1c43c64d, "class_destroy" },
	{ 0xff64f19a, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xbdd7482e, "kmalloc_caches" },
	{ 0xa6302497, "kmalloc_trace" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0xa19b956, "__stack_chk_fail" },
	{ 0x9166fc03, "__flush_workqueue" },
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0xb3d1bec3, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "33DFD4BC32AA2B81A95A3AB");
