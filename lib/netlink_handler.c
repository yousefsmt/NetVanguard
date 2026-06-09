#include <arpa/inet.h>

#include "netlink_handler.h"
#include "parser.h"

int netlink_socket_init( struct nl_sock *sock )
{
    int family_id;
    sock = nl_socket_alloc();
    genl_connect(sock);

    // 2. Resolve the family ID using the name we defined in the kernel
    family_id = genl_ctrl_resolve(sock, FW_NETLINK_NAME);
    if (family_id < 0) {
        fprintf(stderr, "Failed to resolve Netlink family! Is the kernel module loaded?\n");
        nl_socket_free(sock);
        return -1;
    }
    return family_id;
}

int netlink_socket_pack_msg( struct nl_msg *msg, void *hdr, int family_id, int attrtype, uint32_t ip_address )
{
    ( void )hdr;
    ( void )attrtype;

    // 3. Allocate a new Netlink message
    msg = nlmsg_alloc();

    // 4. Setup the Generic Netlink Header (Command: BLOCK_IP)
    genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0, FW_CMD_BLOCK_IP, FW_NETLINK_VERSION);

    // 5. PACK THE TLV: Type = FW_ATTR_SRC_IP, Length/Value = ip_to_block
    nla_put_u32(msg, FW_ATTR_SRC_IP, ip_address);

    return 0;
}

int netlink_socket_send_msg( struct nl_sock *sock, struct nl_msg *msg )
{
    if ( sock && msg )
    {
        int err;
        err = nl_send_auto(sock, msg);
        if ( err < 0)
        {
            nl_cli_fatal( err, "Unable to send message: %s", nl_geterror(err) );
        }
    }
    else
    {
        debug_msg("sock and msg doesn't exist");
        return -1;
    }
    return 0;
}

int netlink_socket_free( struct nl_sock *sock, struct nl_msg *msg )
{
    nlmsg_free( msg );
	nl_close( sock );
	nl_socket_free( sock );
    return 0;
}