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
	{ 0x4c9d28b0, "phys_base" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x21be7f7b, "cdev_init" },
	{ 0x54c057fa, "cdev_add" },
	{ 0x932aac8c, "class_create" },
	{ 0x1421f594, "device_create" },
	{ 0xf09b5d9a, "get_zeroed_page" },
	{ 0xada84b28, "device_destroy" },
	{ 0xa9edd0d8, "class_destroy" },
	{ 0x78445a7c, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x4302d0eb, "free_pages" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x920ce4b8, "remap_pfn_range" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "7ACA7BC79DD3F6D89F64EDF");
