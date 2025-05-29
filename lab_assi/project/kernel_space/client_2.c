#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/string.h>
#include <linux/delay.h>    // for msleep
#include <net/sock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Kernel TCP Client with multiple commands");

#define PORT 12345
#define BUF_LEN 1024

static struct socket *client_sock = NULL;

// Sends one command string and prints server response
static int send_command(const char *cmd) {
    struct kvec vec;
    struct msghdr msg;
    char recv_buf[BUF_LEN] = {0};
    int len, ret;

    pr_info("Client: Sending command '%s'\n", cmd);

    memset(&msg, 0, sizeof(msg));
    vec.iov_base = (char *)cmd;
    vec.iov_len = strlen(cmd);
    ret = kernel_sendmsg(client_sock, &msg, &vec, 1, strlen(cmd));
    if (ret < 0) {
        pr_err("Client: sendmsg failed\n");
        return ret;
    }

    memset(&msg, 0, sizeof(msg));
    vec.iov_base = recv_buf;
    vec.iov_len = BUF_LEN;

    len = kernel_recvmsg(client_sock, &msg, &vec, 1, BUF_LEN, 0);
    if (len > 0) {
        recv_buf[len] = '\0';
        pr_info("Client: received -> %s\n", recv_buf);
    } else {
        pr_err("Client: recvmsg failed or no data\n");
    }

    return 0;
}

static int __init client_init(void) {
    struct sockaddr_in s_addr;
    int ret;

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
    pr_info("Client: Connected to server\n");

    // Send commands 1, 3, 4 one by one with 2 seconds delay
    send_command("1");
    msleep(2000);

    send_command("2");
    msleep(2000);

    send_command("4");
    msleep(2000);
      
    send_command("3");
    msleep(2000);

out:
    if (client_sock)
        sock_release(client_sock);

    pr_info("Client: Finished sending commands\n");
    return 0;
}

static void __exit client_exit(void) {
    pr_info("Client module unloaded\n");
}

module_init(client_init);
module_exit(client_exit);

