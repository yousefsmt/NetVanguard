#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <net/genetlink.h>
#include "kernel.h"


static __be32 accept_ip;
static __be32 blocked_ip;
static __be32 reject_ip;

static int van_accept_hook( struct sk_buff *skb, struct genl_info *info )
{
    __be32 new_ip;

    if ( !info->attrs[FW_ATTR_SRC_IP] )
    {
        printk(KERN_ERR "NetVanguard: Missing IP attribute\n");
        return -EINVAL;
    }
    
    new_ip = nla_get_u32( info->attrs[FW_ATTR_SRC_IP] );
    WRITE_ONCE( accept_ip, new_ip );

    printk( KERN_INFO "NetVanguard: Rule updated. Now acceptable: %pI4\n", &accept_ip );

    return 0;
}

static int van_block_hook( struct sk_buff *skb, struct genl_info *info )
{
    __be32 new_ip;

    if ( !info->attrs[FW_ATTR_SRC_IP] )
    {
        printk(KERN_ERR "NetVanguard: Missing IP attribute\n");
        return -EINVAL;
    }
    
    new_ip = nla_get_u32( info->attrs[FW_ATTR_SRC_IP] );
    WRITE_ONCE( blocked_ip, new_ip );

    printk( KERN_INFO "NetVanguard: Rule updated. Now blocking: %pI4\n", &blocked_ip );

    return 0;
}

static int van_reject_hook( struct sk_buff *skb, struct genl_info *info )
{
    __be32 new_ip;

    if ( !info->attrs[FW_ATTR_SRC_IP] )
    {
        printk(KERN_ERR "NetVanguard: Missing IP attribute\n");
        return -EINVAL;
    }
    
    new_ip = nla_get_u32( info->attrs[FW_ATTR_SRC_IP] );
    WRITE_ONCE( reject_ip, new_ip );

    printk( KERN_INFO "NetVanguard: Rule updated. Now rejection: %pI4\n", &reject_ip );

    return 0;
}

static const struct nla_policy van_attr_policy[FW_ATTR_MAX + 1] =
{
    [FW_ATTR_SRC_IP]  = { .type = NLA_U32 },
    [FW_ATTR_PORT]    = { .type = NLA_U16 },
    [FW_ATTR_DEST_IP] = { .type = NLA_U32 }
};

static const struct genl_ops van_ops[] = 
{
    {
        .doit   = van_accept_hook,
        .policy = van_attr_policy,
        .cmd = FW_CMD_ACCEPT_IP,
        .flags  = 0
    },
    {
        .doit   = van_block_hook,
        .policy = van_attr_policy,
        .cmd = FW_CMD_BLOCK_IP,
        .flags  = 0
    },
    {
        .doit   = van_reject_hook,
        .policy = van_attr_policy,
        .cmd = FW_CMD_REJECT_IP,
        .flags  = 0
    }
};

static struct genl_family van_family = 
{
    .name = FW_NETLINK_NAME,
    .version = FW_NETLINK_VERSION,
    .maxattr = FW_ATTR_MAX,
    .policy = van_attr_policy,
    .ops = van_ops,
    .n_ops = ARRAY_SIZE( van_ops )
};

// static int pack_reply_message( struct genl_info **info )
// {
//     struct sk_buff *skb;
//     void           *hdr;

//     skb = genlmsg_new( GENLMSG_DEFAULT_SIZE, GFP_KERNEL );
//     if ( skb == NULL )
//     {
//         printk( KERN_ERR "NetVanguard: Failed to allocate message for reply\n" );
//         return -ENOMEM;
//     }

//     hdr = genlmsg_put_reply( skb, *info, &van_family, 0, FW_CMD_REPLY );
//     if ( hdr == NULL )
//     {
//         printk( KERN_ERR "NetVanguard: Failed to allocate header\n" );
//         return -ENOMEM;
//     }

//     nla_put_u32( skb, )
// }

// static unsigned int fw_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
// {
//     struct iphdr *iph;

//     if (!skb) return NF_ACCEPT;

//     iph = ip_hdr(skb);
//     if (!iph) return NF_ACCEPT;

//     // If the packet's source IP matches our blocked IP, drop it!
//     if (blocked_ip != 0 && iph->saddr == blocked_ip) {
//         printk_ratelimited(KERN_INFO "FW_BLOCKER: Dropping packet from %pI4\n", &iph->saddr);
//         return NF_DROP;
//     }

//     return NF_ACCEPT;
// }

// static struct nf_hook_ops fw_nf_ops = {
//     .hook     = fw_hook_func,
//     .pf       = NFPROTO_IPV4,
//     .hooknum  = NF_INET_PRE_ROUTING,
//     .priority = NF_IP_PRI_FIRST,
// };

// static int fw_nl_block_ip_cb(struct sk_buff *skb, struct genl_info *info)
// {
//     __be32 new_ip;

//     if (!info->attrs[FW_ATTR_SRC_IP]) {
//         printk(KERN_ERR "FW_BLOCKER: Missing IP attribute\n");
//         return -EINVAL;
//     }

//     // Extract the Value from the TLV
//     new_ip = nla_get_u32(info->attrs[FW_ATTR_SRC_IP]);
    
//     // Update our global firewall rule safely
//     WRITE_ONCE(blocked_ip, new_ip);
    
//     printk(KERN_INFO "FW_BLOCKER: Rule updated. Now blocking: %pI4\n", &blocked_ip);
//     return 0;
// }

static int __init netvanguard_init( void )
{
    int ret;

    ret = genl_register_family( &van_family );
    if ( ret < 0 )
    {
        printk( KERN_ERR "NetVanguard: Failed to register Netlink family error id[%d]\n", ret );
        return ret;
    }

    // ret = nf_register_net_hook( &init_net, &fw_nf_ops );
    // if ( ret < 0 )
    // {
    //     genl_unregister_family( &van_family );
    //     printk( KERN_ERR "NetVanguard: Failed to register Netfilter hook\n" );
    //     return ret;
    // }

    printk(KERN_INFO "NetVanguard: Module loaded successfully.\n");
    return 0;
}

static void __exit netvanguard_exit( void )
{
    int ret;

    // nf_unregister_net_hook(&init_net, &fw_nf_ops);

    ret = genl_unregister_family(&van_family);
    if ( ret < 0 )
    {
        printk( KERN_ERR "NetVanguard: Failed to register Netlink family error id[%d]\n", ret );
    }
    printk(KERN_INFO "FW_BLOCKER: Module unloaded.\n");
}

module_init(netvanguard_init);
module_exit(netvanguard_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yousef.smt");
MODULE_SOFTDEP("pre: nfnetlink");
MODULE_DESCRIPTION("NetVanguard: lightweight firewall");
MODULE_VERSION("v0.1.0");