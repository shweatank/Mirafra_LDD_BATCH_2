#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/inet.h>
#include <linux/net.h>
#include <linux/socket.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/in.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kmod.h>

#define LED_GPIO 17        // GPIO17 (Pin 11 on Pi header)
#define UDP_PORT 9000
#define BUF_SIZE 256

static struct socket *udp_socket = NULL;
static struct task_struct *recv_thread = NULL;
static DECLARE_WAIT_QUEUE_HEAD(wait_queue);
static bool data_ready = false;
static char recv_buffer[BUF_SIZE];
static int recv_len = 0;

static DEFINE_MUTEX(send_mutex);

static void blink_led(int times)
{
    int i;
    for (i = 0; i < times; i++) {
        gpio_set_value(LED_GPIO, 1);
        msleep(200);
        gpio_set_value(LED_GPIO, 0);
        msleep(200);
    }
}

static int play_audio(const char *wavfile)
{
    char *argv[] = {"/usr/local/bin/audio_daemon", (char *)wavfile, NULL};
    static char *envp[] = {
        "HOME=/",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
        NULL
    };

    // Use call_usermodehelper to invoke audio player user-space program
    return call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
}

static int udp_send(const char *data, size_t len)
{
    struct msghdr msg = {0};
    struct kvec iov;
    struct sockaddr_in addr;
    int ret;

    if (!udp_socket)
        return -ENOTCONN;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // send to localhost, change if needed
    addr.sin_port = htons(UDP_PORT);

    iov.iov_base = (char *)data;
    iov.iov_len = len;

    ret = kernel_sendmsg(udp_socket, &msg, &iov, 1, len);
    if (ret < 0)
        pr_err("UDP send failed: %d\n", ret);

    return ret;
}

static int udp_receive_thread(void *data)
{
    struct msghdr msg;
    struct kvec iov;
    char *buf;
    int ret;

    buf = kmalloc(BUF_SIZE, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;

    while (!kthread_should_stop()) {
        memset(&msg, 0, sizeof(msg));
        iov.iov_base = buf;
        iov.iov_len = BUF_SIZE;

        ret = kernel_recvmsg(udp_socket, &msg, &iov, 1, BUF_SIZE, 0);
        if (ret > 0) {
            mutex_lock(&send_mutex);
            memcpy(recv_buffer, buf, ret);
            recv_len = ret;
            data_ready = true;
            mutex_unlock(&send_mutex);

            pr_info("Received UDP data: %.*s\n", ret, buf);

            // Blink LED twice for receive
            blink_led(2);

            // Play received sound
            play_audio("/home/pi/recv.wav");
        } else {
            if (ret == -EAGAIN || ret == -ERESTARTSYS)
                schedule();
            else
                pr_err("UDP receive error: %d\n", ret);
        }
    }
    kfree(buf);
    return 0;
}

// Character device to send data from user space to kernel UDP send
#define DEVICE_NAME "udp_net"

static ssize_t udp_net_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char kbuf[BUF_SIZE];

    if (count > BUF_SIZE)
        return -EINVAL;

    if (copy_from_user(kbuf, buf, count))
        return -EFAULT;

    mutex_lock(&send_mutex);

    // Send UDP packet
    udp_send(kbuf, count);

    // Blink LED once for send
    blink_led(1);

    // Play sent sound
    play_audio("/home/pi/sent.wav");

    mutex_unlock(&send_mutex);

    return count;
}

static ssize_t udp_net_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    int ret = 0;

    if (!data_ready)
        return 0; // No data available

    if (count > recv_len)
        count = recv_len;

    if (copy_to_user(buf, recv_buffer, count))
        return -EFAULT;

    mutex_lock(&send_mutex);
    data_ready = false;
    recv_len = 0;
    mutex_unlock(&send_mutex);

    ret = count;
    return ret;
}

static const struct file_operations udp_net_fops = {
    .owner = THIS_MODULE,
    .write = udp_net_write,
    .read = udp_net_read,
};

static struct miscdevice udp_net_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &udp_net_fops,
};

static int __init net_comm_init(void)
{
    int ret;

    pr_info("UDP Network kernel module init\n");

    // Request LED GPIO
    ret = gpio_request(LED_GPIO, "LED_GPIO");
    if (ret) {
        pr_err("Failed to request GPIO %d\n", LED_GPIO);
        return ret;
    }

    ret = gpio_direction_output(LED_GPIO, 0);
    if (ret) {
        pr_err("Failed to set GPIO %d as output\n", LED_GPIO);
        gpio_free(LED_GPIO);
        return ret;
    }

    // Create UDP socket
    ret = sock_create_kern(&init_net, AF_INET, SOCK_DGRAM, IPPROTO_UDP, &udp_socket);
    if (ret < 0) {
        pr_err("Failed to create UDP socket\n");
        gpio_free(LED_GPIO);
        return ret;
    }

    // Bind socket to port
    {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Bind to localhost
        addr.sin_port = htons(UDP_PORT);

        ret = kernel_bind(udp_socket, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            pr_err("Failed to bind UDP socket\n");
            sock_release(udp_socket);
            gpio_free(LED_GPIO);
            return ret;
        }
    }

    // Start receive thread
    recv_thread = kthread_run(udp_receive_thread, NULL, "udp_recv_thread");
    if (IS_ERR(recv_thread)) {
        pr_err("Failed to create receive thread\n");
        sock_release(udp_socket);
        gpio_free(LED_GPIO);
        return PTR_ERR(recv_thread);
    }

    // Register misc device
    ret = misc_register(&udp_net_device);
    if (ret) {
        pr_err("Failed to register misc device\n");
        kthread_stop(recv_thread);
        sock_release(udp_socket);
        gpio_free(LED_GPIO);
        return ret;
    }

    pr_info("UDP Network kernel module loaded\n");
    return 0;
}

static void __exit net_comm_exit(void)
{
    pr_info("UDP Network kernel module exit\n");

    misc_deregister(&udp_net_device);

    if (recv_thread)
        kthread_stop(recv_thread);

    if (udp_socket)
        sock_release(udp_socket);

    gpio_set_value(LED_GPIO, 0);
    gpio_free(LED_GPIO);
}

module_init(net_comm_init);
module_exit(net_comm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel UDP communication with LED and ALSA sound via user helper");

