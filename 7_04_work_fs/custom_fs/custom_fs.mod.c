#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

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
	{ 0x19fdd0c3, "module_layout" },
	{ 0x1b453766, "simple_statfs" },
	{ 0x9d540ce7, "generic_delete_inode" },
	{ 0x27196f51, "unregister_filesystem" },
	{ 0x15056f6a, "register_filesystem" },
	{ 0xac802549, "d_make_root" },
	{ 0xb9725d3, "current_time" },
	{ 0x1a9926e5, "simple_dir_operations" },
	{ 0x2d214cf, "simple_dir_inode_operations" },
	{ 0xe953b21f, "get_next_ino" },
	{ 0xd1a2416b, "inode_init_owner" },
	{ 0xccb8255e, "new_inode" },
	{ 0xace5c86d, "mount_nodev" },
	{ 0x1520f059, "kill_litter_super" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "AFF089C3CE1222E384A1DF4");
