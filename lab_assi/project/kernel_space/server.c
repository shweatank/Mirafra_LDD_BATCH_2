#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <net/sock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Kernel TCP Server with char device interface");

#define PORT 12345
#define BUF_LEN 1024
#define DEVICE_NAME "tcp_server_dev"

static struct socket *listen_sock = NULL, *client_sock = NULL;
static struct task_struct *server_thread = NULL;

static int major;
static char kernel_buffer[BUF_LEN] = {0};
static DEFINE_MUTEX(dev_lock);

// Forward declaration
static void write_to_user_dev(const char *cmd);

// Character device read
static ssize_t dev_read(struct file *filep, char __user *buf, size_t len, loff_t *offset) {
    ssize_t ret;

    mutex_lock(&dev_lock);
    ret = simple_read_from_buffer(buf, len, offset, kernel_buffer, strlen(kernel_buffer));
    kernel_buffer[0] = '\0'; // Clear buffer after read
    mutex_unlock(&dev_lock);

    return ret;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
};

// Write message to user-space daemon via char device
static void write_to_user_dev(const char *cmd) {
    mutex_lock(&dev_lock);
    strncpy(kernel_buffer, cmd, BUF_LEN - 1);
    kernel_buffer[BUF_LEN - 1] = '\0';
    mutex_unlock(&dev_lock);
}

// TCP server kernel thread
static int server_fn(void *data) {
    struct sockaddr_in server_addr;
    struct kvec vec;
    struct msghdr msg;
    char buf[BUF_LEN];
    int len, ret;

    // Create socket
    ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &listen_sock);
    if (ret < 0) {
        pr_err("Server: socket create failed\n");
        return ret;
    }

    // Bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = listen_sock->ops->bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        pr_err("Server: bind failed\n");
        goto out_release;
    }

    // Listen
    ret = listen_sock->ops->listen(listen_sock, 1);
    if (ret < 0) {
        pr_err("Server: listen failed\n");
        goto out_release;
    }

    pr_info("Server: Listening on port %d...\n", PORT);

    // Accept
    ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &client_sock);
    if (ret < 0) {
        pr_err("Server: client socket create failed\n");
        goto out_release;
    }

    ret = listen_sock->ops->accept(listen_sock, client_sock, 0, true);
    if (ret < 0) {
        pr_err("Server: accept failed\n");
        goto out_release;
    }

    pr_info("Server: Client connected\n");

    while (!kthread_should_stop()) {
        memset(&msg, 0, sizeof(msg));
        memset(buf, 0, BUF_LEN);
        vec.iov_base = buf;
        vec.iov_len = BUF_LEN;

        len = kernel_recvmsg(client_sock, &msg, &vec, 1, BUF_LEN, 0);
        if (len <= 0) {
            pr_info("Server: client disconnected\n");
            break;
        }

        buf[len] = '\0';
        pr_info("Server: received -> %s\n", buf);

        if (strcmp(buf, "1") == 0) {
            strcpy(buf, "Camera opened.");
            write_to_user_dev("1");
        } else if (strcmp(buf, "2") == 0) {
            strcpy(buf, "Camera closed.");
            write_to_user_dev("2");
        } else if (strcmp(buf, "3") == 0) {
            strcpy(buf, "Audio playing.");
            write_to_user_dev("3");
        } else if (strcmp(buf, "4") == 0) {
            strcpy(buf, "Audio stopped.");
            write_to_user_dev("4");
        } else {
            strcpy(buf, "Invalid command.");
        }

        memset(&msg, 0, sizeof(msg));
        vec.iov_base = buf;
        vec.iov_len = strlen(buf);
        kernel_sendmsg(client_sock, &msg, &vec, 1, strlen(buf));
    }

out_release:
    if (client_sock) sock_release(client_sock);
    if (listen_sock) sock_release(listen_sock);
    return 0;
}

// Module init
static int __init server_init(void) {
    pr_info("Server module loading...\n");

    // Register char device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register character device\n");
        return major;
    }

    pr_info("Char device /dev/%s registered with major %d\n", DEVICE_NAME, major);

    // Start TCP server thread
    server_thread = kthread_run(server_fn, NULL, "tcp_server_kthread");
    return 0;
}

// Module exit
static void __exit server_exit(void) {
    if (server_thread) {
        kthread_stop(server_thread);
    }
    if (client_sock)
        sock_release(client_sock);
    if (listen_sock)
        sock_release(listen_sock);

    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Server module unloaded\n");
}

module_init(server_init);
module_exit(server_exit);

