#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

static struct hrtimer hr_timer;
static ktime_t kt_periode;

enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer)
{
    pr_info("High-res Timer fired at: %llu ns\n", ktime_to_ns(ktime_get()));

    // Restart the timer
    hrtimer_forward_now(timer, kt_periode);
    return HRTIMER_RESTART;
}

static int __init hrtimer_example_init(void)
{
    pr_info("Starting hrtimer example\n");

    kt_periode = ktime_set(0, 500 * 1000000); // 500 milliseconds

    hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    hr_timer.function = &my_hrtimer_callback;

    hrtimer_start(&hr_timer, kt_periode, HRTIMER_MODE_REL);

    return 0;
}

static void __exit hrtimer_example_exit(void)
{
    int ret = hrtimer_cancel(&hr_timer);
    if (ret)
        pr_info("Timer was still active\n");

    pr_info("Hrtimer example exit\n");
}

module_init(hrtimer_example_init);
module_exit(hrtimer_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("High-Resolution Timer Example");
