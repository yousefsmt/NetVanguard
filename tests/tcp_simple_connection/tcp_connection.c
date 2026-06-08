#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <getopt.h>
#include <time.h>

#include "tcp_handler.h"


void
debug_msg(const char* fmt, ...)
{
    int      is_correct   = 0x00;
    char     buff[0x64UL] = {'0'};
    struct timespec ts           = {.tv_nsec = 0x00L, .tv_sec = 0x00L};

    is_correct = timespec_get(&ts, TIME_UTC);
    if (is_correct == 0x00)
    {
        perror("timespec_get failed");
        return;
    }

    
    is_correct = (int)strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    if (is_correct == 0x00)
    {
        perror("strftime failed");
        return;
    }

    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    char buf[0x01+vsnprintf(NULL, 0x00UL, fmt, args1)];
    va_end(args1);
    vsnprintf(buf, sizeof buf, fmt, args2);
    va_end(args2);
 
    fprintf(stderr, "%s.%09ld ERROR: %s\n", buff, ts.tv_nsec, buf);

}

static void help(void *pname)
{
    printf(
    "NetVanguard: TCP Simple Connection v0.1.0\n"
    "Developed by: Yousef.smt\n\n"
    "Usage: %s [OPTION]\n\n"
    "Options:\n"
    "  -r, --role <role>        Set socket role: 'server' or 'client'.\n"
    "  -t, --type <type>        Set socket type: 'server' or 'client'.\n"
    "  -i, --ip <address>       Set IP address (e.g., 192.168.1.1).\n"
    "  -p, --port <port>        Set port number (0 - 65535).\n"
    "  -m, --message <string>   Set the payload message.\n"
    "                             - Client: Message sent to the server.\n"
    "                             - Server: Custom echo response to the client.\n"
    "  -h, --help               Display this help message and exit.\n", 
    (const char*)pname);
    exit(0x00);
}

int parse_args(struct socket_config_t* socket_config,
               int argc, char* argv[])
{
    int option_index = 0x00;
    int c            = 0x00;

    while (true)
    {
        option_index = 0x00;

        static struct option long_options[] =
        {
            {"role",    required_argument, 0,  'r'},
            {"type",    required_argument, 0,  't'},
            {"ip",      required_argument, 0,  'i'},
            {"port",    required_argument, 0,  'p'},
            {"message", required_argument, 0,  'm'},
            {0,         0,                 0,   0 }
        };

        c = getopt_long(argc, argv, "rtipm", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'r':
        {
            printf("[r] argc: %d\n", argc );
            size_t idx  = (strncmp(argv[optind-0x01], "-r", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            printf("[r] argv[%ld]: %s\n", idx, argv[idx] );
            break;
        }
        case 't':
        {
            printf("[t] argc: %d\n", argc );
            size_t idx  = (strncmp(argv[optind-0x01], "-t", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            printf("[t] argv[%ld]: %s\n", idx, argv[idx] );
            break;
        }
        case 'i':
        {
            printf("[i] argc: %d\n", argc );
            size_t idx  = (strncmp(argv[optind-0x01], "-i", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            printf("[i] argv[%ld]: %s\n", idx, argv[idx] );
            break;
        }
        case 'p':
        {
            printf("[p] argc: %d\n", argc );
            size_t idx  = (strncmp(argv[optind-0x01], "-p", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            printf("[p] argv[%ld]: %s\n", idx, argv[idx] );
            break;
        }
        case 'm':
        {
            printf("[m] argc: %d\n", argc );
            size_t idx  = (strncmp(argv[optind-0x01], "-m", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            printf("[m] argv[%ld]: %s\n", idx, argv[idx] );
            break;
        }
        case '?':
            help(argv[optind-0x01]);
            break;
        default:
            debug_msg("argument parser returned character code 0%o is NOT valid!!!", c);
        }
    }

    if (argc == 1){help("no args");}

    return 0;
}
struct socket_address_t
{
    char     ip_addr[15];
    uint16_t port;
};

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
            printf("read error\n");
            return -1;
        }
        printf( "Read %ld bytes message[%s]\n", read_byte, buffer );

        ret = tcp_socket_close( socket_config, SERVER_SIDE );
        if ( ret < 0 )
        {
            printf("init error\n");
            return -1;
        }
        break;
    }
    case CLIENT_SIDE:
    {
        ssize_t send_byte;

        send_byte = tcp_socket_send( socket_config, buffer, buffer_size );
        if ( send_byte < 0 )
        {
            printf("send error\n");
            return -1;
        }
        printf( "Send %ld bytes message[%s]\n", send_byte, buffer );
        break;
    }
    default:
        printf("server_type invalid\n");
        return -1;
    }
}

int main( int argc, char* argv[] )
{
    struct socket_address_t socket_address;
    struct socket_config_t  socket_config;
    char                    buffer[1024];
    enum   socket_type_t    socket_type;
    int                     ret;

    if ( argc != 7 )
    {
        return -1;
    }

    socket_type = parse_args( &socket_config, argc, argv );

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