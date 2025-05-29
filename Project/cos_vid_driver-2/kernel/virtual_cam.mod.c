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
	{ 0xf9a482f9, "msleep" },
	{ 0xb43f9365, "ktime_get" },
	{ 0x5635a60a, "vmalloc_user" },
	{ 0xf9c6fca3, "usb_control_msg" },
	{ 0x86b5950e, "usb_alloc_urb" },
	{ 0x7f993d76, "usb_alloc_coherent" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0xcb742157, "kmalloc_caches" },
	{ 0xfe1d3f1a, "kmalloc_trace" },
	{ 0x84df77fa, "usb_get_dev" },
	{ 0xa1c3117d, "v4l2_device_register" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0x2b703898, "video_device_alloc" },
	{ 0x44658cd3, "video_device_release" },
	{ 0xbe0d758d, "__video_register_device" },
	{ 0xe23cdf5f, "video_ioctl2" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x3d4ed8ba, "usb_register_driver" },
	{ 0x122c3a7e, "_printk" },
	{ 0xbd514f86, "usb_kill_urb" },
	{ 0xf159e76d, "usb_free_coherent" },
	{ 0x9ad1f3c7, "usb_free_urb" },
	{ 0x179ae9fc, "usb_set_interface" },
	{ 0x6b53cb8b, "video_unregister_device" },
	{ 0x999e8297, "vfree" },
	{ 0x94ee6f0c, "v4l2_device_unregister" },
	{ 0x14607ce2, "usb_put_dev" },
	{ 0x37a0cba, "kfree" },
	{ 0xe9835e87, "remap_vmalloc_range" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x904f8725, "usb_deregister" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x69acdf38, "memcpy" },
	{ 0xbc1070b0, "usb_submit_urb" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "videodev");

MODULE_ALIAS("usb:v04CAp7054d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "34BAEF9C99AC9F76D8C758C");
