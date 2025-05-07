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
	{ 0x67ac943c, "mount_nodev" },
	{ 0x94b6cced, "unregister_filesystem" },
	{ 0x58f62f44, "new_inode" },
	{ 0xe953b21f, "get_next_ino" },
	{ 0x99cfc7e6, "simple_dir_inode_operations" },
	{ 0x5e5df7b9, "pcpu_hot" },
	{ 0xc058a89d, "current_time" },
	{ 0x154a9f69, "make_kuid" },
	{ 0xa53a18f3, "make_kgid" },
	{ 0x876e2fda, "d_make_root" },
	{ 0x5a036dd9, "init_user_ns" },
	{ 0x796223a0, "from_kgid" },
	{ 0x2bc7f2b1, "from_kuid" },
	{ 0x6a8778ac, "kill_litter_super" },
	{ 0xff2b2f11, "generic_delete_inode" },
	{ 0x81fa8ed3, "simple_statfs" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xeb63624d, "register_filesystem" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xb3d1bec3, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F925D48B6BC88FD8C94C194");
