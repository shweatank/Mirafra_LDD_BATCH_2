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
	{ 0x122c3a7e, "_printk" },
	{ 0xedc03953, "iounmap" },
	{ 0xf92d6d0f, "pci_release_region" },
	{ 0xdac1fff4, "pci_disable_device" },
	{ 0x3d53ea68, "pci_enable_device" },
	{ 0x18039951, "pci_request_region" },
	{ 0xde80cd09, "ioremap" },
	{ 0xa78af5f3, "ioread32" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xfd3de8a2, "pci_unregister_driver" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x8b091dd, "__pci_register_driver" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc6227e48, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("pci:v000010ECd00008168sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "C69A1352F69D3C855295B4A");
