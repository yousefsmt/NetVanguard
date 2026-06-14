#include "types.h"
#include "parser.h"
#include "netlink_handler.h"

int main( int argc, char *argv[] )
{
    int family_id;
    int err;

    struct van_cli_t van_cli;

    err = cli_parser( &van_cli, argc, argv );
    if ( err < 0 )
    {
        ERROR( "Something is wrong, cannot parse the arguments. Please check all the values of arguments with examples very carefully!!" );
        return -1;
    }

    signal( SIGINT,  handler_signal );
    signal( SIGKILL, handler_signal );
    signal( SIGTERM, handler_signal );
    signal( SIGSEGV, handler_signal );

    family_id = netlink_socket_init( &van_cli.socket );

    err = netlink_socket_init_cb( &van_cli.socket );
    if ( err < 0 )
    {
        ERROR( "Adding callback error occur!!\n");
        return -1;
    }

    if ( ( van_cli.rules.flags & REMOVE_BYTE ) == REMOVE_BYTE )
    {
        err = netlink_socket_pack_msg( &van_cli.socket, &van_cli.msg, &van_cli.hdr, &van_cli.rules, family_id, FW_CMD_REMOVE );
        if ( err < 0 )
        {
            ERROR( "During packing message error occur!!\n");
            return -1;
        }

    }
    else
    {
        err = netlink_socket_pack_msg( &van_cli.socket, &van_cli.msg, &van_cli.hdr, &van_cli.rules, family_id, FW_CMD_REQUEST );
        if ( err < 0 )
        {
            ERROR( "During packing message error occur!!\n");
            return -1;
        }
    }

    err = netlink_socket_send_msg( &van_cli.socket, &van_cli.msg );
    if ( err < 0 )
    {
        ERROR( "During sending message error occur!!\n");
        return -1;
    }

    err = netlink_socket_recv_msg( &van_cli.socket );
    if ( err < 0 )
    {
        ERROR( "During receiving message error occur!!\n");
        return -1;
    }

    err = netlink_socket_free( &van_cli.socket, &van_cli.msg );
    if ( err < 0 )
    {
        ERROR( "During deallocation resource error occur!!\n");
        return -1;
    }

    return 0;
}