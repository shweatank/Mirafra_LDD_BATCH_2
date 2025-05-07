#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
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
	{ 0x2c635209, "module_layout" },
	{ 0x701e897, "simple_statfs" },
	{ 0xedce1ebd, "generic_delete_inode" },
	{ 0xf96a27e7, "unregister_filesystem" },
	{ 0x90726a81, "register_filesystem" },
	{ 0xe1dcd6f2, "d_make_root" },
	{ 0xa41baaf9, "current_time" },
	{ 0x9a69d79b, "simple_dir_operations" },
	{ 0x2d3a1f5c, "simple_dir_inode_operations" },
	{ 0xe953b21f, "get_next_ino" },
	{ 0x2a3a0fa3, "inode_init_owner" },
	{ 0x6a368b20, "new_inode" },
	{ 0x8f3d62e4, "mount_nodev" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xf43590bf, "kill_litter_super" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "5033F2E9D37373178B6D0E8");
