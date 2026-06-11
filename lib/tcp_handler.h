#ifndef NETVANGUARD_LIB_TCP_HANDLER_H_
#define NETVANGUARD_LIB_TCP_HANDLER_H_

#include <stdint.h>
#include <netinet/in.h>

enum socket_type_t
{
    SERVER_SIDE,
    CLIENT_SIDE
};

struct server_side
{
    struct sockaddr_in addr;
    char               ip_addr[15];
    socklen_t          length;
    uint16_t           port_addr;
};

struct client_side
{
    struct sockaddr_in addr;
    socklen_t          length;
    int                client_fd;
};

struct socket_config_t
{
    union
    {
        struct server_side server;
        struct client_side client;
    };
    
    struct sockaddr_in addr;
    socklen_t          length;
    int                socket_fd;
};

int tcp_socket_init( struct socket_config_t* socket_config,
                     enum   socket_type_t    socket_type,
                     const char*             ip_addr,
                     const uint16_t          port_addr);

ssize_t tcp_socket_send( struct socket_config_t* socket_config,
                         const char*             message,
                         const size_t            message_len);

ssize_t tcp_socket_read( struct socket_config_t* socket_config,
                         char*                   buffer,
                         const size_t            buffer_size );

int tcp_socket_close( struct socket_config_t* socket_config,
                      enum   socket_type_t    socket_type );

#endif /* NETVANGUARD_LIB_TCP_HANDLER_H_ */