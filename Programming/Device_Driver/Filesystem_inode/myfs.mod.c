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
	{ 0x76bbc651, "unregister_filesystem" },
	{ 0x19375793, "new_inode" },
	{ 0xe953b21f, "get_next_ino" },
	{ 0xcb044650, "simple_dir_inode_operations" },
	{ 0xe2491b52, "pcpu_hot" },
	{ 0xbaa5aa1b, "current_time" },
	{ 0xa75f983, "make_kuid" },
	{ 0x4ced70fa, "make_kgid" },
	{ 0x1afd414b, "d_make_root" },
	{ 0x31587213, "init_user_ns" },
	{ 0x6fd61710, "from_kgid" },
	{ 0xf1d9689d, "from_kuid" },
	{ 0xed2b7edb, "kill_litter_super" },
	{ 0x28bd33ef, "generic_delete_inode" },
	{ 0x462a5774, "simple_statfs" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x4845f8de, "register_filesystem" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x6409e3de, "mount_nodev" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F9B762507270AB58534A2E9");
