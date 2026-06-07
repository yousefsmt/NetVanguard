#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

MODULE_LICENSE("GPL");

static struct nf_hook_ops *nfho = NULL;

static unsigned int hfunc( void *priv, struct sk_buff *skb, const struct nf_hook_state *state )
{
	( void )priv;
	struct iphdr  *iph;
	struct udphdr *udph;

	if ( skb && state )
	{
		iph = ip_hdr( skb );

		if ( iph->protocol == IPPROTO_UDP )
		{
			udph = udp_hdr( skb );
			if ( ntohs(udph->dest) == 53 )
			{
				return NF_ACCEPT;
			}
		}
		else if ( iph->protocol == IPPROTO_TCP )
		{
			return NF_ACCEPT;
		}
	}
	else
	{
		return NF_ACCEPT;
	}
	
	return NF_DROP;
}

static int __init LKM_init(void)
{
    int err;
	nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
	
	nfho->hook 	= (nf_hookfn*)hfunc;		/* hook function */
	nfho->hooknum 	= NF_INET_PRE_ROUTING;		/* received packets */
	nfho->pf 	= PF_INET;			/* IPv4 */
	nfho->priority 	= NF_IP_PRI_FIRST;		/* max hook priority */
	
	err = nf_register_net_hook(&init_net, nfho);
	if ( err < 0 )
		return err;

    return 0;
}

static void __exit LKM_exit(void)
{
	nf_unregister_net_hook(&init_net, nfho);
	kfree(nfho);
}

module_init(LKM_init);
module_exit(LKM_exit);