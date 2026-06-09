#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <net/genetlink.h>
#include "kernel.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yousef.smt");
MODULE_DESCRIPTION("NetVanguard: lightweight firewall");


static __be32 blocked_ip = 0;


static unsigned int fw_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *iph;

    if (!skb) return NF_ACCEPT;

    iph = ip_hdr(skb);
    if (!iph) return NF_ACCEPT;

    // If the packet's source IP matches our blocked IP, drop it!
    if (blocked_ip != 0 && iph->saddr == blocked_ip) {
        printk_ratelimited(KERN_INFO "FW_BLOCKER: Dropping packet from %pI4\n", &iph->saddr);
        return NF_DROP;
    }

    return NF_ACCEPT;
}

static struct nf_hook_ops fw_nf_ops = {
    .hook     = fw_hook_func,
    .pf       = NFPROTO_IPV4,
    .hooknum  = NF_INET_PRE_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};



static const struct nla_policy fw_genl_policy[FW_ATTR_MAX + 1] = {
    [FW_ATTR_SRC_IP] = { .type = NLA_U32 },
};


static int fw_nl_block_ip_cb(struct sk_buff *skb, struct genl_info *info)
{
    __be32 new_ip;

    if (!info->attrs[FW_ATTR_SRC_IP]) {
        printk(KERN_ERR "FW_BLOCKER: Missing IP attribute\n");
        return -EINVAL;
    }

    // Extract the Value from the TLV
    new_ip = nla_get_u32(info->attrs[FW_ATTR_SRC_IP]);
    
    // Update our global firewall rule safely
    WRITE_ONCE(blocked_ip, new_ip);
    
    printk(KERN_INFO "FW_BLOCKER: Rule updated. Now blocking: %pI4\n", &blocked_ip);
    return 0;
}

// Map commands to callbacks
static const struct genl_ops fw_genl_ops[] = {
    {
        .cmd    = FW_CMD_BLOCK_IP,
        .flags  = 0,
        .policy = fw_genl_policy,
        .doit   = fw_nl_block_ip_cb,
    },
};

// Define the Netlink family structure
static struct genl_family fw_genl_family = {
    .name     = FW_NETLINK_NAME,
    .version  = FW_NETLINK_VERSION,
    .maxattr  = FW_ATTR_MAX,
    .ops      = fw_genl_ops,
    .n_ops    = ARRAY_SIZE(fw_genl_ops),
};

/* ------------------------------------------------------------------
 * 3. MODULE INIT / EXIT
 * ------------------------------------------------------------------ */
static int __init fw_mod_init(void)
{
    int ret;

    ret = genl_register_family(&fw_genl_family);
    if ( ret < 0 )
    {
        printk( KERN_ERR "FW_BLOCKER: Failed to register Netlink family error id[%d]\n", ret );
        return ret;
    }

    ret = nf_register_net_hook(&init_net, &fw_nf_ops);
    if ( ret < 0 ) {
        genl_unregister_family(&fw_genl_family);
        printk(KERN_ERR "FW_BLOCKER: Failed to register Netfilter hook\n");
        return ret;
    }

    printk(KERN_INFO "FW_BLOCKER: Module loaded successfully.\n");
    return 0;
}

static void __exit fw_mod_exit(void)
{
    nf_unregister_net_hook(&init_net, &fw_nf_ops);
    genl_unregister_family(&fw_genl_family);
    printk(KERN_INFO "FW_BLOCKER: Module unloaded.\n");
}

module_init(fw_mod_init);
module_exit(fw_mod_exit);