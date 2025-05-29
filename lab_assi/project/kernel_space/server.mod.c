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
	{ 0xa916b694, "strnlen" },
	{ 0x8aaddd33, "kernel_sendmsg" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0x26be07bc, "kernel_recvmsg" },
	{ 0x4b1e061b, "sock_release" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xfba81ab0, "kthread_create_on_node" },
	{ 0x97f2f0d2, "wake_up_process" },
	{ 0xd852b3ed, "kthread_stop" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x7acb87aa, "sock_create" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "AD88A8BCDD38E3243DE4B96");
