#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <net/sock.h>

// External functions from LED and ALSA drivers
extern void trigger_led_blink(int duration_ms);
extern void play_sound(int type); // 0 for "Data received", 1 for "LED blinking"

static struct socket *server_sock = NULL;
static struct socket *client_sock = NULL;
static struct task_struct *server_thread = NULL;

#define PORT 12345
#define MAX_MSG_LEN 32

static int server_thread_fn(void *data) {
    struct sockaddr_in addr;
    struct msghdr msg;
    struct iovec iov;
    char buffer[MAX_MSG_LEN];
    int ret;

    // Create socket
    ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &server_sock);
    if (ret < 0) {
        printk(KERN_ERR "Socket creation failed: %d\n", ret);
        return ret;
    }

    // Bind socket
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = server_sock->ops->bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        printk(KERN_ERR "Bind failed: %d\n", ret);
        goto cleanup;
    }

    // Listen
    ret = server_sock->ops->listen(server_sock, 5);
    if (ret < 0) {
        printk(KERN_ERR "Listen failed: %d\n", ret);
        goto cleanup;
    }

    while (!kthread_should_stop()) {
        // Accept connection
        ret = server_sock->ops->accept(server_sock, &client_sock, 0);
        if (ret < 0) {
            printk(KERN_ERR "Accept failed: %d\n", ret);
            msleep(1000);
            continue;
        }

        // Receive data
        memset(&msg, 0, sizeof(msg));
        iov.iov_base = buffer;
        iov.iov_len = MAX_MSG_LEN - 1;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        ret = sock_recvmsg(client_sock, &msg, MAX_MSG_LEN - 1, 0);
        if (ret > 0) {
            buffer[ret] = '\0';
            printk(KERN_INFO "Received: %s\n", buffer);

            // Process command
            if (strcmp(buffer, "blink") == 0) {
                trigger_led_blink(5000); // Blink for 5 seconds
                play_sound(1); // Play "LED blinking" sound
            } else {
                play_sound(0); // Play "Data received" sound
            }
        }

        // Close client socket
        sock_release(client_sock);
        client_sock = NULL;
    }

cleanup:
    if (server_sock)
        sock_release(server_sock);
    return 0;
}

static int __init socket_server_init(void) {
    printk(KERN_INFO "Initializing socket server\n");
    server_thread = kthread_run(server_thread_fn, NULL, "socket_server");
    if (IS_ERR(server_thread)) {
        printk(KERN_ERR "Failed to start server thread\n");
        return PTR_ERR(server_thread);
    }
    return 0;
}

static void __exit socket_server_exit(void) {
    printk(KERN_INFO "Exiting socket server\n");
    if (server_thread)
        kthread_stop(server_thread);
    if (server_sock)
        sock_release(server_sock);
}

module_init(socket_server_init);
module_exit(socket_server_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel TCP server for receiving commands");