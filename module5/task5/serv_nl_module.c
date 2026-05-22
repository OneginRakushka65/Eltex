#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>

MODULE_AUTHOR("Kuleshin Alexander");
MODULE_DESCRIPTION("Netlink module");
MODULE_LICENSE("GPL");

#define NETLINK_PROTO 2

static struct sock* nl_sk = NULL;

static void nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    struct sk_buff  *skb_out;
    char *payload;
    char  response[64];
    int   pid;
    int   msg_size;

    nlh = (struct nlmsghdr *)skb->data;
    payload = (char *)NLMSG_DATA(nlh);
    pid = nlh->nlmsg_pid;

    printk(KERN_INFO "netlink: from pid %d: \"%s\"\n", pid, payload);

    snprintf(response, sizeof(response), "Hello from kernel! Got: %s", payload);
    msg_size = strlen(response) + 1;

    skb_out = nlmsg_new(msg_size, GFP_KERNEL);
    if (!skb_out) {
        printk(KERN_ERR "netlink: failed to allocate skb\n");
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    strncpy(NLMSG_DATA(nlh), response, msg_size);
    nlmsg_unicast(nl_sk, skb_out, pid);

    printk(KERN_INFO "netlink: sent response to pid %d\n", pid);
}

static int __init netlink_init(void) {
    struct netlink_kernel_cfg cfg = {
        .input = nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_PROTO, &cfg);
    if (!nl_sk) {
        printk(KERN_ERR "netlink: failed to create socket\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "netlink: module loaded\n");
    return 0;
}

static void __exit netlink_exit(void) {
    netlink_kernel_release(nl_sk);
    printk(KERN_INFO "netlink: module unloaded\n");
}

module_init(netlink_init);
module_exit(netlink_exit);