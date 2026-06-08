#include "netlink_handler.h"

int netlink_socket_init( struct socket_config_t* src_socket_config, struct socket_config_t* dest_socket_config )
{
    int ret;

    src_socket_config->socket_fd = socket( PF_NETLINK, SOCK_RAW, NETLINK_USER );

    if( src_socket_config->socket_fd < 0 )
    {
        return -1;
    }

    memset( &src_socket_config->addr , 0, sizeof( src_socket_config->addr ) );
    src_socket_config->addr.nl_family = AF_NETLINK;
    src_socket_config->addr.nl_pid    = getpid();

    ret = bind( src_socket_config->socket_fd, ( struct sockaddr* )&src_socket_config->addr, sizeof( src_socket_config->addr ) );

    if( ret < 0 )
    {
        perror("bind()");
        ( void )close( src_socket_config->socket_fd );
        return -1;
    }

    memset(&dest_socket_config->addr, 0, sizeof(dest_socket_config->addr));
    dest_socket_config->addr.nl_family = AF_NETLINK;
    dest_socket_config->addr.nl_pid    = 0;
    dest_socket_config->addr.nl_groups = 0;

    return 0;
}