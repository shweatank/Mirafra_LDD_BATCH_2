#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/timekeeping.h>

static struct rtc_device *rtc_dummy_dev;

/* Read system time and convert to RTC time */
static int dummy_read_time(struct device *dev, struct rtc_time *tm)
{
    struct timespec64 ts;
    ktime_get_real_ts64(&ts); // Get current system time
    rtc_time64_to_tm(ts.tv_sec, tm);

    dev_info(dev, "Dummy RTC READ: %ptR\n", tm);
    return 0;
}

/* Set system time from RTC */
static int dummy_set_time(struct device *dev, struct rtc_time *tm)
{
    struct timespec64 ts = {
        .tv_sec = rtc_tm_to_time64(tm),
        .tv_nsec = 0,
    };

    dev_info(dev, "Dummy RTC SET: %ptR\n", tm);
    do_settimeofday64(&ts); // WARNING: This changes system time!
    return 0;
}

/* Define RTC class operations */
static const struct rtc_class_ops dummy_rtc_ops = {
    .read_time = dummy_read_time,
    .set_time = dummy_set_time,
};

static int dummy_rtc_probe(struct platform_device *pdev)
{
    rtc_dummy_dev = devm_rtc_device_register(&pdev->dev,
                                             "rtc-dummy-x86",
                                             &dummy_rtc_ops,
                                             THIS_MODULE);
    if (IS_ERR(rtc_dummy_dev)) {
        dev_err(&pdev->dev, "Failed to register dummy RTC\n");
        return PTR_ERR(rtc_dummy_dev);
    }

    dev_info(&pdev->dev, "Dummy RTC driver registered successfully\n");
    return 0;
}

static int dummy_rtc_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Dummy RTC driver removed\n");
    return 0;
}

static struct platform_driver dummy_rtc_driver = {
    .driver = {
        .name = "rtc-dummy-x86",
        .owner = THIS_MODULE,
    },
    .probe = dummy_rtc_probe,
    .remove = dummy_rtc_remove,
};

static struct platform_device *rtc_dummy_pdev;

static int __init dummy_rtc_init(void)
{
    int ret;

    rtc_dummy_pdev = platform_device_register_simple("rtc-dummy-x86", -1, NULL, 0);
    if (IS_ERR(rtc_dummy_pdev)) {
        pr_err("Failed to register dummy RTC platform device\n");
        return PTR_ERR(rtc_dummy_pdev);
    }

    ret = platform_driver_register(&dummy_rtc_driver);
    if (ret) {
        platform_device_unregister(rtc_dummy_pdev);
        return ret;
    }

    pr_info("Dummy RTC driver (x86) loaded\n");
    return 0;
}

static void __exit dummy_rtc_exit(void)
{
    platform_driver_unregister(&dummy_rtc_driver);
    platform_device_unregister(rtc_dummy_pdev);
    pr_info("Dummy RTC driver (x86) unloaded\n");
}

module_init(dummy_rtc_init);
module_exit(dummy_rtc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kavya");
MODULE_DESCRIPTION("Dummy RTC platform driver for x86");
