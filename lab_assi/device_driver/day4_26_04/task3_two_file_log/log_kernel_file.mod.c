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
	{ 0x754d539c, "strlen" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0xfff5afc, "time64_to_tm" },
	{ 0x96848186, "scnprintf" },
	{ 0x148653, "vsnprintf" },
	{ 0xbb244ef5, "kernel_write" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x760e5b9b, "__register_chrdev" },
	{ 0x417d8d98, "filp_open" },
	{ 0x56939b2c, "filp_close" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x2273f01b, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "A4DD28BB18A165C423CF3F3");
