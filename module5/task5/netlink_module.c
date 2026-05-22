#include <stdio.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <string.h>
#include <stdlib.h>

#define NETLINK_PROTO  2
#define MAX_PAYLOAD    256

int main(void) {
    struct sockaddr_nl src_addr, dst_addr; //1 - адрес сокета
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_PROTO);

    struct nlmsghdr *nlh;
    struct iovec iov;
    struct msghdr msg;
    const char *text = "Hello, kernel!";

    if (fd < 0) {
        perror("socket failed");
        return 1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    if (bind(fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("bind failed");
        close(fd);
        return 1;
    }

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

    nlh->nlmsg_len = NLMSG_SPACE(strlen(text) + 1);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = NLM_F_REQUEST;
    nlh->nlmsg_seq = 1;

    strcpy(NLMSG_DATA(nlh), text);

    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.nl_family = AF_NETLINK;
    dst_addr.nl_pid = 0;
    dst_addr.nl_groups = 0;

    iov.iov_base = (void*)nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &dst_addr;
    msg.msg_namelen = sizeof(dst_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (sendmsg(fd, &msg, 0) < 0) {
        perror("sendmsg");
    } else {
        printf("Sent: \"%s\"\n", text);
    }

    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

    iov.iov_base = (void*)nlh;
    iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

    if (recvmsg(fd, &msg, 0) < 0) {
        perror("recvmsg");
    } else {
        printf("Received: \"%s\"\n", (char *)NLMSG_DATA(nlh));
    }

    free(nlh);
    close(fd);
    return 0;
}