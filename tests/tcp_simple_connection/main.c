#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tcp_handler.h"

int main( int argc, char* argv[] )
{
    enum socket_type_t socket_type;
    int                ret;

    if ( argc != 7 )
    {
        return -1;
    }

    switch ( socket_type )
    {
    case SERVER_SIDE:
    {
        struct socket_config_t socket_config;
        const size_t           BUFFER_SIZE = 15;
        char                   buffer[BUFFER_SIZE];
        ssize_t                read_byte;
        uint16_t               port_addr = strtol( argv[6], NULL, 10 );
        ret = tcp_socket_init( &socket_config, SERVER_SIDE, argv[5], port_addr );
        if ( ret < 0 )
        {
            printf("close error\n");
            return -1;
        }

        read_byte = tcp_socket_read( &socket_config, buffer, BUFFER_SIZE );
        if ( read_byte < 0 )
        {
            printf("read error\n");
            return -1;
        }
        printf( "Read %ld bytes message[%s]\n", read_byte, buffer );

        ret = tcp_socket_close( &socket_config, SERVER_SIDE );
        if ( ret < 0 )
        {
            printf("init error\n");
            return -1;
        }
        break;
    }
    case CLIENT_SIDE:
    {
        struct socket_config_t socket_config;
        const size_t           BUFFER_SIZE = 13;
        const char             buffer[] = "Hello Server";
        ssize_t                send_byte;
        uint16_t               port_addr = strtol( argv[3], NULL, 10 );
        ret = tcp_socket_init( &socket_config, CLIENT_SIDE, argv[2], port_addr );
        if ( ret < 0 )
        {
            printf("init error\n");
            return -1;
        }

        send_byte = tcp_socket_send( &socket_config, buffer, BUFFER_SIZE );
        if ( send_byte < 0 )
        {
            printf("send error\n");
            return -1;
        }
        printf( "Send %ld bytes message[%s]\n", send_byte, buffer );

        ret = tcp_socket_close( &socket_config, CLIENT_SIDE );
        if ( ret < 0 )
        {
            printf("close error\n");
            return -1;
        }
        break;
    }
    default:
        printf("server_type invalid\n");
        return -1;
    }

    return 0;
}