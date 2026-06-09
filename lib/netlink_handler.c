#include <arpa/inet.h>
#include <netlink/msg.h>
#include <netlink/cli/utils.h>

#include "netlink_handler.h"

int netlink_socket_init( struct nl_sock **socket )
{
    int family_id;

    *socket = nl_cli_alloc_socket();

    ( void )nl_cli_connect( *socket, NETLINK_GENERIC );

    family_id = genl_ctrl_resolve( *socket, FW_NETLINK_NAME );
    if ( family_id < 0 )
    {
        nl_close( *socket );
        nl_socket_free( *socket );
        nl_cli_fatal( family_id, "Failed to resolve generic netlink family! [%s]", nl_geterror( family_id ) );
    }

    return family_id;
}

int netlink_socket_pack_msg( struct nl_sock **socket, struct nl_msg **msg, void **hdr,
                               int family_id, int attrtype, int cmd, uint32_t ip_address )
{
    int err;

    if ( *socket == NULL )
    {
        nl_cli_fatal( EINVAL, "You must call this message after socket_init! [%s]", nl_geterror( EINVAL ) );
    }

    *msg = nlmsg_alloc();
    if ( *msg == NULL )
    {
        nl_close( *socket );
        nl_socket_free( *socket );
        nl_cli_fatal( ENOMEM, "Failed to allocate netlink message! [%s]", nl_geterror( ENOMEM ) );
    }

    *hdr = genlmsg_put( *msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0, cmd, FW_NETLINK_VERSION );
    if ( *hdr == NULL )
    {
        nl_close( *socket );
        nl_socket_free( *socket );
        nl_cli_fatal( ENOMEM, "Failed to add header to message [%s]", nl_geterror( ENOMEM ) );
    }

    err = nla_put_u32( *msg, attrtype, ip_address );
    if ( err < 0 )
    {
        nl_close( *socket );
        nl_socket_free( *socket );
        nl_cli_fatal( err, "Failed to add data to message [%s]", nl_geterror( err ) );
    }

    return 0;
}

int netlink_socket_send_msg( struct nl_sock **sock, struct nl_msg **msg )
{
    if ( *sock && *msg )
    {
        int err;
        err = nl_send_auto( *sock, *msg );
        if ( err < 0)
        {
            nl_cli_fatal( err, "Failed to send message [%s]", nl_geterror( err ) );
        }
    }
    else
    {
        nl_cli_fatal( ENOMEM, "Message or socket structure is empty [%s]", nl_geterror( ENOMEM ) );
    }

    return 0;
}

int netlink_socket_free( struct nl_sock **sock, struct nl_msg **msg )
{
    if ( *sock && *msg )
    {
        nlmsg_free( *msg );
        nl_close( *sock );
        nl_socket_free( *sock );
    }
    else
    {
        nl_cli_fatal( ENOMEM, "Message or socket structure is empty [%s]", nl_geterror( ENOMEM ) );
    }
    return 0;
}