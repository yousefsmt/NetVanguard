#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "tcp_handler.h"

ssize_t tcp_socket_read( struct socket_config_t* socket_config,
                     char*                       buffer,
                     const size_t                buffer_size )
{
    ssize_t read_byte;

    read_byte = recv( socket_config->client.client_fd, buffer, buffer_size, 0 );
    if ( ( size_t )read_byte < buffer_size )
    {
        perror("read()");
        close( socket_config->socket_fd );
        close( socket_config->client.client_fd );
        return -1;
    }

    return read_byte;
}

ssize_t tcp_socket_send( struct socket_config_t* socket_config,
                     const char*             message,
                     size_t                  message_len)
{

    ssize_t send_byte = send( socket_config->socket_fd, message, message_len, 0 );
    if ( send_byte < 15 )
    {
        perror("send()");
        close( socket_config->socket_fd );
        return -1;
    }

    return send_byte;
}

static int tcp_socket_listening( struct socket_config_t* socket_config, int max_connections )
{
    int ret;

    ret = listen( socket_config->socket_fd, max_connections );
    if ( ret < 0 )
    {
        perror("listen()");
        close( socket_config->socket_fd );
        return -1;
    }

    socket_config->client.client_fd = accept( socket_config->socket_fd,
                                              ( struct sockaddr* )&socket_config->client.addr,
                                              &socket_config->client.length );
    if ( socket_config->client.client_fd < 0 )
    {
        perror("accept()");
        close( socket_config->socket_fd );
        close( socket_config->client.client_fd );
        return -1;
    }

    return 0;
}


static int tcp_socket_connecting( struct socket_config_t* socket_config,
                                  const char*             ip_addr,
                                  const uint16_t          port_addr )
{
    int ret;

    socket_config->server.addr.sin_family = AF_INET;
    ret = inet_pton( AF_INET, ip_addr, &socket_config->server.addr.sin_addr.s_addr );
    if ( ret < 0 )
    {
        perror("inet_pton()");
        return -1;
    }
    socket_config->server.addr.sin_port = htons( port_addr );
    socket_config->server.length = sizeof( struct sockaddr_in );


    ret = connect( socket_config->socket_fd,
                   (const struct sockaddr*)&socket_config->server.addr,
                   socket_config->server.length );
    if ( ret < 0 )
    {
        perror("connect()");
        close( socket_config->socket_fd );
        return -1;
    }

    return 0;
}


int tcp_socket_init( struct socket_config_t* socket_config,
                     enum   socket_type_t    socket_type,
                     const char*             ip_addr,
                     const uint16_t          port_addr)
{
    int    ret;
    size_t ip_addr_len;

    ip_addr_len = strlen( ip_addr );
    if ( ip_addr_len > 15 ){return -1;}

    socket_config->addr.sin_family = AF_INET;
    ret = inet_pton( AF_INET, ip_addr, &socket_config->addr.sin_addr.s_addr );
    if ( ret < 0 )
    {
        perror("inet_pton()");
        return -1;
    }
    socket_config->addr.sin_port = htons( port_addr );
    socket_config->length        = sizeof( struct sockaddr_in );

    socket_config->socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( socket_config->socket_fd < 0 )
    {
        perror("socket()");
        close( socket_config->socket_fd );
        return -1;
    }

    ret = bind( socket_config->socket_fd,
               (const struct sockaddr*)&socket_config->addr,
                socket_config->length );
    if ( ret < 0 )
    {
        perror("bind()");
        close( socket_config->socket_fd );
        return -1;
    }

    switch ( socket_type )
    {
    case SERVER_SIDE:
    {
        ret = tcp_socket_listening( socket_config, 10 );
        break;
    }
    case CLIENT_SIDE:
    {
        ret = tcp_socket_connecting( socket_config, "127.0.1.10", 3635 );
        break;
    }
    default:
        printf("server_type invalid\n");
        return -1;
    }

    return 0;
}

int tcp_socket_close( struct socket_config_t* socket_config,
                      enum   socket_type_t    socket_type )
{
    switch ( socket_type )
    {
    case SERVER_SIDE:
    {
        close( socket_config->socket_fd );
        close( socket_config->client.client_fd );
        break;
    }
    case CLIENT_SIDE:
    {
        close( socket_config->socket_fd );
        break;
    }
    default:
        printf("server_type invalid\n");
        return -1;
    }

    return 0;
}