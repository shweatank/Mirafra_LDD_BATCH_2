wq#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <linux/slab.h>
#include <net/sock.h>

#define SERVER_PORT 12345
#define SERVER_IP   0x7F000001  // 127.0.0.1 in hex
#define BUF_LEN     128

static struct socket *client_sock = NULL;

static int __init socket_client_init(void)
{
    struct sockaddr_in saddr;
    int ret;
    char *send_buf = "Hello from kernel!";
    struct kvec vec;
    struct msghdr msg = {0};

    printk(KERN_INFO "Kernel module socket client loaded\n");

    // Create socket
    ret = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &client_sock);
    if (ret < 0) {
        printk(KERN_ERR "Failed to create socket: %d\n", ret);
        return ret;
    }

    // Set up address
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SERVER_PORT);
    saddr.sin_addr.s_addr = htonl(SERVER_IP);

    // Connect to server
    ret = client_sock->ops->connect(client_sock, (struct sockaddr *)&saddr,
                                    sizeof(saddr), 0);
    if (ret < 0) {
        printk(KERN_ERR "Failed to connect: %d\n", ret);
        sock_release(client_sock);
        return ret;
    }

    printk(KERN_INFO "Connected to server.\n");

    // Send message
    vec.iov_base = send_buf;
    vec.iov_len = strlen(send_buf);
    ret = kernel_sendmsg(client_sock, &msg, &vec, 1, strlen(send_buf));
    if (ret < 0)
        printk(KERN_ERR "Send failed: %d\n", ret);
    else
        printk(KERN_INFO "Message sent from kernel.\n");

    return 0;
}

static void __exit socket_client_exit(void)
{
    if (client_sock) {
        sock_release(client_sock);
    }
    printk(KERN_INFO "Kernel socket client unloaded\n");
}

module_init(socket_client_init);
module_exit(socket_client_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("A simple kernel socket client example");

