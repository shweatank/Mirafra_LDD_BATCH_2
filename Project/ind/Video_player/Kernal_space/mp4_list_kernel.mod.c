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
	{ 0x82f5eb5f, "path_put" },
	{ 0x4d07136f, "filp_open" },
	{ 0xc063935c, "iterate_dir" },
	{ 0x792d7d44, "filp_close" },
	{ 0x122c3a7e, "_printk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x83557adc, "proc_create" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0xcb742157, "kmalloc_caches" },
	{ 0xfe1d3f1a, "kmalloc_trace" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x37a0cba, "kfree" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xc41355a8, "proc_remove" },
	{ 0x52821b6d, "seq_read" },
	{ 0xa797d86c, "seq_lseek" },
	{ 0xcb1b2a3c, "single_release" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xdfcac0d2, "single_open" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xb4b312e4, "seq_printf" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xc1b8b7d, "kern_path" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2213B2AC16357E74036E9C4");
