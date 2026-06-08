#include <arpa/inet.h>

#include "netlink_handler.h"

static int family_id;

int netlink_socket_init( struct nl_sock *sock )
{
    sock = nl_cli_alloc_socket();
    if ( sock == NULL )
    {
		nl_cli_fatal( ENOBUFS, "Unable to allocate netlink socket" );
	}

    ( void )nl_cli_connect( sock, NETLINK_GENERIC );

    family_id = genl_ctrl_resolve( sock, FW_NETLINK_NAME );
    if ( family_id != GENL_ID_CTRL )
    {
        nl_cli_fatal( NLE_INVAL, "Resolving of " FW_NETLINK_NAME " failed" );
    }

    return 0;
}

int netlink_socket_init_msg( struct nl_msg *msg )
{
    msg = nlmsg_alloc();
    if ( msg == NULL )
    {
        nl_cli_fatal( NLE_NOMEM, "Unable to allocate netlink message" );
    }
    return 0;
}

int netlink_socket_add_header( struct nl_msg *msg, void *hdr )
{
    hdr = genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0, FW_CMD_BLOCK_IP, FW_NETLINK_VERSION);
    if ( hdr == NULL )
    {
        nl_cli_fatal(ENOMEM, "Unable to write genl header");
    }
    return 0;
}

int netlink_socket_add_data( struct nl_msg *msg, int attrtype, const char *ip_address )
{
    in_addr_t ip;
    int err;

    ip = inet_addr( ip_address );
    if ( ip == (uint32_t)-1 )
    {
        nl_cli_fatal( 0, "ip address invalid" );
    }
    err = nla_put_u32( msg, attrtype, ip );
    if ( err < 0 )
    {
        nl_cli_fatal(err, "Unable to add attribute: %s", nl_geterror(err));
    }
    return 0;
}

int netlink_socket_send_msg( struct nl_sock *sock, struct nl_msg *msg )
{
    int err;
    err = nl_send_auto(sock, msg);
    if ( err < 0)
    {
        nl_cli_fatal( err, "Unable to send message: %s", nl_geterror(err) );
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