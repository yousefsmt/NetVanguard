#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "tcp_handler.h"
#include "parser.h"

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
    debug_msg("socket with %d id read %ld bytes", socket_config->client.client_fd, read_byte );
    debug_msg("\tMessage: %s, Length: %ld", buffer, buffer_size );

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
    debug_msg("socket with %d id send %ld bytes", socket_config->socket_fd, send_byte );
    debug_msg("\tMessage: %s, Length: %ld", message, message_len );

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
    debug_msg("scoket[%d] on listen mode with %d connections", socket_config->socket_fd, max_connections );

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
    debug_msg("scoket[%d] accept connection and create new socket with %d id", socket_config->socket_fd, socket_config->client.client_fd );

    return 0;
}


static int tcp_socket_connecting( struct socket_config_t* socket_config)
{
    int ret;
    socket_config->server.addr.sin_family = AF_INET;
    ret = inet_pton( AF_INET, socket_config->server.ip_addr, &socket_config->server.addr.sin_addr.s_addr );
    if ( ret < 0 )
    {
        perror("inet_pton()");
        return -1;
    }
    socket_config->server.addr.sin_port = htons( socket_config->server.port_addr );
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
    debug_msg("socket with %d id connect to server with:", socket_config->socket_fd);
    debug_msg("\tIP Address: %s", socket_config->server.ip_addr);
    debug_msg("\tPort Address: %d", socket_config->server.port_addr);

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
    if ( ip_addr_len > 15 )
    {
        debug_msg("bind ip address exceeded from max len[15] ( you set: %ld)", ip_addr_len);
        return -1;
    }

    socket_config->addr.sin_family = AF_INET;
    ret = inet_pton( AF_INET, ip_addr, &socket_config->addr.sin_addr.s_addr );
    if ( ret < 0 )
    {
        perror("inet_pton()");
        debug_msg("ip address not valid [%s]", ip_addr );
        return -1;
    }
    socket_config->addr.sin_port = htons( port_addr );
    socket_config->length        = sizeof( struct sockaddr_in );

    socket_config->socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( socket_config->socket_fd < 0 )
    {
        perror("socket()");
        debug_msg("socket doesn't create");
        close( socket_config->socket_fd );
        return -1;
    }
    debug_msg("create socket with [ID: %d], [IP: %s], [Port: %d]", socket_config->socket_fd, ip_addr, port_addr);

    ret = bind( socket_config->socket_fd,
               (const struct sockaddr*)&socket_config->addr,
                socket_config->length );
    if ( ret < 0 )
    {
        perror("bind()");
        close( socket_config->socket_fd );
        return -1;
    }

    debug_msg("Successfully bind socket!!!");
    debug_msg("\tFile Descriptor ID: %d", socket_config->socket_fd);
    debug_msg("\tIP Address: %s", ip_addr);
    debug_msg("\tPort Address: %d", port_addr);

    switch ( socket_type )
    {
    case SERVER_SIDE:
    {
        ret = tcp_socket_listening( socket_config, 10 );
        if ( ret < 0 ){return -1;}
        break;
    }
    case CLIENT_SIDE:
    {
        ret = tcp_socket_connecting( socket_config );
        if ( ret < 0 ){return -1;}
        break;
    }
    default:
        printf("socket_type invalid\n");
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
        debug_msg("socket with SERVER_SIDE role closed!!!!!");
        break;
    }
    case CLIENT_SIDE:
    {
        close( socket_config->socket_fd );
        debug_msg("socket with CLIENT_SIDE role closed!!!!!");
        break;
    }
    default:
        printf("socket_type invalid\n");
        return -1;
    }

    return 0;
}