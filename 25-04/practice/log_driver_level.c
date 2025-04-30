#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/ktime.h>
#include<linux/timekeeping.h>
#include<linux/time.h>
static void log_message(const char *level,const char *message)
{
	struct tm tm;
	time64_t now=ktime_get_real_seconds();
	time64_to_tm(now,NSEC_PER_SEC, 0,&tm);
	pr_info("[%04ld-%02d-%02d %02d:%02d:%02d] %s %s\n",tm.tm_year+1900,
			tm.tm_mon +1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec,
			level,
			message);
}

static int __init hello_init(void)
{
	log_message("info","device is opened..\n");
	return 0;
}
static void __exit hello_exit(void)
{
	log_message("info","device is close..\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
