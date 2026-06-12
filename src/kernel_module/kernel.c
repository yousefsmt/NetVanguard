#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <net/tcp.h>
#include <net/genetlink.h>
#include <linux/generic-radix-tree.h>

#include "kernel.h"

static unsigned long idx;
static GENRADIX(struct van_str_rule_t) rule_genradix;

static int pack_reply_message( struct genl_info **info, u32 status );

static int van_data_hook( struct sk_buff *skb, struct genl_info *info )
{
    if ( !info->attrs[FW_ATTR_IP] )
    {
        printk(KERN_ERR "NetVanguard: Missing IP attribute\n");
        return -EINVAL;
    }

    if ( !info->attrs[FW_ATTR_PORT] )
    {
        printk(KERN_ERR "NetVanguard: Missing port attribute\n");
        return -EINVAL;
    }

    if ( !info->attrs[FW_ATTR_FLAG] )
    {
        printk(KERN_ERR "NetVanguard: Missing port attribute\n");
        return -EINVAL;
    }

    if ( idx >= FW_GEN_PRE_ALLOC )
    {
        printk(KERN_INFO "NetVanguard: The radix tree database is full; this rule overwrites at zero index\n");
        WRITE_ONCE( idx, 0 );
    }

    struct van_str_rule_t *db = genradix_ptr( &rule_genradix, idx );
    if ( db )
    {
        db->ip    = nla_get_u32( info->attrs[FW_ATTR_IP] );
        db->port  = nla_get_u16( info->attrs[FW_ATTR_PORT] );
        db->flags = nla_get_u8( info->attrs[FW_ATTR_FLAG] );

        printk(KERN_INFO "NetVanguard: receive rule packet\n" );
        printk(KERN_INFO "NetVanguard: # %ld IP: %pI4 | PORT: %d\n", idx, &db->ip, db->port );
        printk(KERN_INFO "NetVanguard: # %ld SIDE: %s\n", idx, (GET_SIDE(db->flags) == INPUT) ? "INPUT" : "OUTPUT");
        printk(KERN_INFO "NetVanguard: # %ld HOOK: %s\n", idx, (GET_HOOK_TYPE(db->flags) == SOURCE) ? "SOURCE" : "DESTINATION");
        printk(KERN_INFO "NetVanguard: # %ld RULE: %s\n", idx, (GET_RULE_TYPE(db->flags) == ACCEPT) ? "ACCEPT" : (GET_RULE_TYPE(db->flags) == BLOCK) ? "BLOCK" : "REJECT");

        if ( pack_reply_message( &info, FW_REP_ICMP ) < 0 )
        {
            printk(KERN_ERR "NetVanguard: Reply failed\n");
            return -EINVAL;
        }

        WRITE_ONCE( idx, idx+1 );
    }
    else
    {
        printk(KERN_ERR "NetVanguard: The radix tree database idx[%ld] does not exist\n", idx );
        return -ENOMEM;

    }

    return 0;
}

static const struct nla_policy van_attr_policy[FW_ATTR_MAX + 1] =
{
    [FW_ATTR_IP]   = { .type = NLA_U32 },
    [FW_ATTR_PORT] = { .type = NLA_U16 },
    [FW_ATTR_FLAG] = { .type = NLA_U8 },
    [FW_ATTR_ACK]  = { .type = NLA_U32 }
};

static const struct genl_ops van_ops[] = 
{
    {
        .doit   = van_data_hook,
        .policy = van_attr_policy,
        .cmd    = FW_CMD_REQUEST,
        .flags  = 0
    }
};

static struct genl_family van_family = 
{
    .name    = FW_NETLINK_NAME,
    .version = FW_NETLINK_VERSION,
    .maxattr = FW_ATTR_MAX,
    .policy  = van_attr_policy,
    .ops     = van_ops,
    .n_ops   = ARRAY_SIZE( van_ops )
};

static int pack_reply_message( struct genl_info **info, u32 status )
{
    struct sk_buff *skb;
    void           *hdr;
    int             ret;

    skb = genlmsg_new( GENLMSG_DEFAULT_SIZE, GFP_KERNEL );
    if ( skb == NULL )
    {
        printk( KERN_ERR "NetVanguard: Failed to allocate message for reply\n" );
        return -ENOMEM;
    }

    hdr = genlmsg_put_reply( skb, *info, &van_family, 0, FW_CMD_RESPONSE );
    if ( hdr == NULL )
    {
        nlmsg_free( skb );
        printk( KERN_ERR "NetVanguard: Failed to allocate header\n" );
        return -ENOMEM;
    }

    if ( nla_put_u32( skb, FW_ATTR_ACK, status ) )
    {
        genlmsg_cancel( skb, hdr );
        nlmsg_free( skb );
        printk( KERN_ERR "NetVanguard: Failed to send message\n" );
        return -EINVAL;
    }

	genlmsg_end( skb, hdr );

	ret = genlmsg_reply( skb, *info );

    printk( KERN_INFO "NetVanguard: Send message for reply\n" );

    return ret;
}

// static unsigned int ntf_post_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
// {
//     struct iphdr *iph;

//     if (!skb) return NF_ACCEPT;

//     iph = ip_hdr(skb);
//     if (!iph) return NF_ACCEPT;

//     if ( iph->protocol == IPPROTO_TCP )
//     {
//         /* port extraction */
//         struct tcphdr *tcph = tcp_hdr(skb);
//         if ( !tcph ) return NF_ACCEPT;
//         if ( tcph->source == port )
//         {
//             /* TODO: must block port*/
//         }
//     }

//     // If the packet's source IP matches our blocked IP, drop it!
//     if (blocked_ip != 0 && iph->saddr == blocked_ip) {
//         printk_ratelimited(KERN_INFO "FW_BLOCKER: Dropping packet from %pI4\n", &iph->saddr);
//         return NF_DROP;
//     }

//     return NF_ACCEPT;
// }

// static struct nf_hook_ops ntf_post_ops =
// {
//     .hook     = ntf_post_hook,
//     .pf       = NFPROTO_IPV4,
//     .hooknum  = NF_INET_POST_ROUTING,
//     .priority = NF_IP_PRI_FIRST,
// };
static unsigned int block_both(const struct sk_buff *skb, const struct van_str_rule_t *rules)
{
    if ( skb && rules )
    {
        u16 port = htons( rules->port );

        struct iphdr *iph = ip_hdr(skb);
        struct tcphdr *tcph = tcp_hdr(skb);

        if (!iph || !tcph ) return NF_ACCEPT;
        switch (GET_HOOK_TYPE(rules->flags))
        {
        case SOURCE:
            if ( iph->saddr == rules->ip && tcph->source == port ) return NF_DROP;
            break;
        case DESTINATION:
            if ( iph->daddr == rules->ip && tcph->dest == port ) return NF_DROP;
            break;
        default:
            return NF_ACCEPT;
        }

    }

    return NF_ACCEPT;
}

static unsigned int block_ip(const struct sk_buff *skb, const struct van_str_rule_t *rules)
{
    if ( skb && rules )
    {
        struct iphdr *iph = ip_hdr(skb);
        if (!iph) return NF_ACCEPT;

        switch (GET_HOOK_TYPE(rules->flags))
        {
        case SOURCE:
            if ( iph->saddr == rules->ip ) return NF_DROP;
            break;
        case DESTINATION:
            if ( iph->daddr == rules->ip ) return NF_DROP;
            break;
        default:
            return NF_ACCEPT;
        }

    }

    return NF_ACCEPT;
}

static unsigned int block_port(const struct sk_buff *skb, const struct van_str_rule_t *rules)
{
    if ( skb && rules )
    {
        u16 port = htons( rules->port );

        struct tcphdr *tcph = tcp_hdr(skb);
        if ( !tcph ) return NF_ACCEPT;

        switch (GET_HOOK_TYPE(rules->flags))
        {
        case SOURCE:
            if ( tcph->source == port ) return NF_DROP;
            break;
        case DESTINATION:
            if ( tcph->dest == port ) return NF_DROP;
            break;
        default:
            return NF_ACCEPT;
        }
    }

    return NF_ACCEPT;
}

static unsigned int ntf_pre_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct van_str_rule_t *rules;
    struct genradix_iter iter;

    if (!skb) return NF_ACCEPT;

    struct iphdr *iph = ip_hdr(skb);
    if (!iph) return NF_ACCEPT;

    if ( iph->protocol != IPPROTO_TCP ) return NF_ACCEPT;

    genradix_for_each(&rule_genradix, iter, rules)
    {
        if (iter.pos >= idx)
            break;

        if (!rules) 
            return NF_ACCEPT;

        if (GET_SIDE(rules->flags) == OUTPUT || 
            GET_RULE_TYPE(rules->flags) == ACCEPT || 
            GET_RULE_TYPE(rules->flags) == REJECT) {
            continue;
        }

        if (rules->ip != 0 && rules->port != 0) {
            if (!block_both(skb, rules)) {
                pr_info_ratelimited("NetVanguard: DROP BOTH\n");
                return NF_DROP;
            }
        } 
        else if (rules->ip != 0) {
            if (!block_ip(skb, rules)) {
                pr_info_ratelimited("NetVanguard: DROP IP\n");
                return NF_DROP;
            }
        } 
        else if (rules->port != 0) {
            if (!block_port(skb, rules)) {
                pr_info_ratelimited("NetVanguard: DROP PORT\n");
                return NF_DROP;
            }
        }
    }

    return NF_ACCEPT;

    // for (i = 0; i < idx; i++)
    // {
    //     rules = genradix_ptr( &rule_genradix, i );
    //     if(!rules) return NF_ACCEPT;

    //     if( GET_SIDE( rules->flags ) == OUTPUT || GET_RULE_TYPE( rules->flags ) == ACCEPT || GET_RULE_TYPE( rules->flags ) == REJECT ) continue;

    //     if ( rules->ip != 0 && rules->port != 0 )
    //     {
    //         // return block_both( skb, rules );
    //         if(!block_both( skb, rules ))
    //         {
    //             printk( KERN_INFO "NetVanguard: DROP BOTH\n" );
    //             return NF_DROP;
    //         }
    //     }
    //     else if ( rules->ip   != 0 )
    //     {
    //         // return block_ip( skb, rules );
    //         if(!block_ip( skb, rules ))
    //         {
    //             printk( KERN_INFO "NetVanguard: DROP IP\n" );
    //             return NF_DROP;
    //         }
    //     }
    //     else if ( rules->port != 0 )
    //     {
    //         // return block_port( skb, rules );
    //         if(!block_port( skb, rules ))
    //         {
    //             printk( KERN_INFO "NetVanguard: DROP PORT\n" );
    //             return NF_DROP;
    //         }
    //     }
    // }

    // return NF_ACCEPT;
}

static struct nf_hook_ops ntf_pre_ops =
{
    .hook     = ntf_pre_hook,
    .pf       = NFPROTO_IPV4,
    .hooknum  = NF_INET_PRE_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};

static int __init netvanguard_init( void )
{
    int ret;
    
    genradix_init( &rule_genradix );

    genradix_prealloc( &rule_genradix, FW_GEN_PRE_ALLOC, GFP_KERNEL );

    WRITE_ONCE( idx, 0 );

    ret = genl_register_family( &van_family );
    if ( ret < 0 )
    {
        printk( KERN_ERR "NetVanguard: Failed to register Netlink family error id[%d]\n", ret );
        return ret;
    }

    
    ret = nf_register_net_hook( &init_net, &ntf_pre_ops );
    if ( ret < 0 )
    {
        genl_unregister_family( &van_family );
        printk( KERN_ERR "NetVanguard: Failed to register Netfilter pre-routing hook\n" );
        return ret;
    }

    /*
    ret = nf_register_net_hook( &init_net, &ntf_post_ops );
    if ( ret < 0 )
    {
        genl_unregister_family( &van_family );
        printk( KERN_ERR "NetVanguard: Failed to register Netfilter post-routing hook\n" );
        return ret;
    }
    */

    printk(KERN_INFO "NetVanguard: Module loaded successfully.\n");
    return 0;
}

static void __exit netvanguard_exit( void )
{
    int ret;

    genradix_free( &rule_genradix );

    
    nf_unregister_net_hook(&init_net, &ntf_pre_ops);
    /*
    nf_unregister_net_hook(&init_net, &ntf_post_ops);
    */

    ret = genl_unregister_family(&van_family);
    if ( ret < 0 )
    {
        printk( KERN_ERR "NetVanguard: Failed to register Netlink family error id[%d]\n", ret );
    }

    printk(KERN_INFO "NetVanguard: Module unloaded.\n");
}

module_init(netvanguard_init);
module_exit(netvanguard_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yousef.smt");
MODULE_SOFTDEP("pre: nfnetlink");
MODULE_DESCRIPTION("NetVanguard: lightweight firewall");
MODULE_VERSION("v0.1.0");