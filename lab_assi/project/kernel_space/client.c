#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/string.h>
#include <net/sock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Kernel TCP Client");

#define PORT 12345
#define BUF_LEN 1024

static struct socket *client_sock = NULL;

static int __init client_init(void) {
    struct sockaddr_in s_addr;
    struct kvec vec;
    struct msghdr msg;
    char buf[BUF_LEN] = "2";  // change to "1", "2", etc.
    char recv_buf[BUF_LEN] = {0};
    int len, ret;

    pr_info("Client: Starting...\n");

    ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &client_sock);
    if (ret < 0) {
        pr_err("Client: Socket create failed\n");
        return ret;
    }

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    s_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // 127.0.0.1

    ret = client_sock->ops->connect(client_sock, (struct sockaddr *)&s_addr, sizeof(s_addr), 0);
    if (ret < 0) {
        pr_err("Client: Connect failed\n");
        goto out;
    }

    memset(&msg, 0, sizeof(msg));
    vec.iov_base = buf;
    vec.iov_len = strlen(buf);
    kernel_sendmsg(client_sock, &msg, &vec, 1, strlen(buf));

    memset(&msg, 0, sizeof(msg));
    vec.iov_base = recv_buf;
    vec.iov_len = BUF_LEN;

    len = kernel_recvmsg(client_sock, &msg, &vec, 1, BUF_LEN, 0);
    if (len > 0) {
        recv_buf[len] = '\0';
        pr_info("Client: received -> %s\n", recv_buf);
    }

out:
    if (client_sock)
        sock_release(client_sock);
    return 0;
}

static void __exit client_exit(void) {
    pr_info("Client module unloaded\n");
}

module_init(client_init);
module_exit(client_exit);

