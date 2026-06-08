#ifndef NETVANGUARD_LIB_INCLUDE_NETLINK_HANDLER_H_
#define NETVANGUARD_LIB_INCLUDE_NETLINK_HANDLER_H_

#include <netlink/netlink.h>


#define NETLINK_USER 30 // same customized protocol as in my kernel module
#define MAX_PAYLOAD 1024 // maximum payload size

struct socket_config_t
{
    struct sockaddr_nl addr;
    int                socket_fd;
};

int netlink_socket_init( struct socket_config_t* socket_config, struct socket_config_t* dest_socket_config );

ssize_t netlink_socket_send( struct socket_config_t* socket_config,
                         const char*             message,
                         const size_t            message_len);

ssize_t netlink_socket_read( struct socket_config_t* socket_config,
                         char*                   buffer,
                         const size_t            buffer_size );

// int netlink_socket_close( struct socket_config_t* socket_config,
//                       enum   socket_type_t    socket_type );

#endif /* NETVANGUARD_LIB_INCLUDE_NETLINK_HANDLER_H_ */