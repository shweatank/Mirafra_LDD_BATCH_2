// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init oops_demo_init(void)
{
	pr_info("oops_demo: loading; going to crash now …\n");

	/* Classic null‑pointer write → “Unable to handle kernel NULL pointer dereference” */
	*(int *)0 = 42;

	return 0;           /* never reached */
}

static void __exit oops_demo_exit(void)
{
	pr_info("oops_demo: unloaded (this message is never printed)\n");
}

module_init(oops_demo_init);
module_exit(oops_demo_exit);

MODULE_AUTHOR("demo");
MODULE_DESCRIPTION("Intentional Oops generator");
MODULE_LICENSE("GPL");

