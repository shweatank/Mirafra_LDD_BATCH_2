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
	{ 0x54c057fa, "cdev_add" },
	{ 0x932aac8c, "class_create" },
	{ 0x1421f594, "device_create" },
	{ 0x1bed7c85, "input_register_handler" },
	{ 0xada84b28, "device_destroy" },
	{ 0xa9edd0d8, "class_destroy" },
	{ 0x78445a7c, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x4d321e7e, "input_unregister_handler" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0xcb742157, "kmalloc_caches" },
	{ 0xfe1d3f1a, "kmalloc_trace" },
	{ 0x14108376, "input_register_handle" },
	{ 0x54876c5c, "input_open_device" },
	{ 0xa916b694, "strnlen" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xebd4503, "input_close_device" },
	{ 0xd0c484b1, "input_unregister_handle" },
	{ 0x37a0cba, "kfree" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x21be7f7b, "cdev_init" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "FB09AD99DEF0BF21616C9BD");
