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
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x54f83ed0, "kernel_kobj" },
	{ 0xcbfb903f, "kobject_create_and_add" },
	{ 0x9ac01899, "sysfs_create_file_ns" },
	{ 0x1bed7c85, "input_register_handler" },
	{ 0x154c3e09, "misc_register" },
	{ 0x5aa6db0b, "sysfs_remove_file_ns" },
	{ 0xf3da4071, "kobject_put" },
	{ 0x4d321e7e, "input_unregister_handler" },
	{ 0x2468fabd, "misc_deregister" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0xcb742157, "kmalloc_caches" },
	{ 0xfe1d3f1a, "kmalloc_trace" },
	{ 0x14108376, "input_register_handle" },
	{ 0x54876c5c, "input_open_device" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xa916b694, "strnlen" },
	{ 0xd957e1ac, "kernel_write" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0x8820ff80, "kobject_uevent" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xebd4503, "input_close_device" },
	{ 0xd0c484b1, "input_unregister_handle" },
	{ 0x37a0cba, "kfree" },
	{ 0x122c3a7e, "_printk" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x9166fada, "strncpy" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x4d07136f, "filp_open" },
	{ 0x38373c5f, "kernel_read" },
	{ 0x792d7d44, "filp_close" },
	{ 0x8c8569cb, "kstrtoint" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "DDA49E3683573B2452163B9");
