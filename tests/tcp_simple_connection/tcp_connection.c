#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <getopt.h>
#include <time.h>

#include "tcp_handler.h"
#include "parser.h"

static int tcp_socket_transfer( struct socket_config_t* socket_config, enum socket_type_t socket_type, char* buffer, const size_t buffer_size )
{

    switch ( socket_type )
    {
    case SERVER_SIDE:
    {
        ssize_t read_byte;

        read_byte = tcp_socket_read( socket_config, buffer, buffer_size );
        if ( read_byte < 0 )
        {
            debug_msg("read error\n");
            return -1;
        }
        debug_msg( "Read %ld bytes message[%s]\n", read_byte, buffer );
        break;
    }
    case CLIENT_SIDE:
    {
        ssize_t send_byte;

        send_byte = tcp_socket_send( socket_config, buffer, buffer_size );
        if ( send_byte < 0 )
        {
            debug_msg("send error\n");
            return -1;
        }
        debug_msg( "Send %ld bytes message[%s]\n", send_byte, buffer );
        break;
    }
    default:
        debug_msg("socket_type invalid\n");
        return -1;
    }
    
    return 0;
}

int main( int argc, char* argv[] )
{
    struct socket_address_t socket_address;
    struct socket_config_t  socket_config;
    char                    buffer[1024];
    enum   socket_type_t    socket_type;
    int                     ret;

    socket_type = parse_args( &socket_config, &socket_address, argc, argv, buffer, 1024 );
    debug_msg("Role side: %s", ( socket_type == SERVER_SIDE ) ? "SERVER_SIDE" : "CLIENT_SIDE" );
    debug_msg("IP Address: %s", socket_address.ip_addr );
    debug_msg("Port Address: %d", socket_address.port );
    debug_msg("Message: %s", buffer );


    ret = tcp_socket_init( &socket_config, socket_type, socket_address.ip_addr, socket_address.port );
    if ( ret < 0 )
    {
        printf("init error\n");
        return -1;
    }

    ret = tcp_socket_transfer( &socket_config, socket_type, buffer, 1024 );
    if ( ret < 0 )
    {
        printf("transfer error\n");
        return -1;
    }

    ret = tcp_socket_close( &socket_config, socket_type );
    if ( ret < 0 )
    {
        printf("close error\n");
        return -1;
    }

    return 0;
}