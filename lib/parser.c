#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

#include "parser.h"

void debug_msg(const char* fmt, ...)
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
 
    fprintf(stderr, "%s.%09ld DEBUG: %s\n", buff, ts.tv_nsec, buf);

}

void help(void *pname)
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

enum socket_type_t parse_args(struct socket_config_t* socket_config, struct socket_address_t* socket_address,
                              int argc, char* argv[], char* buffer, size_t buffer_size)
{
    enum socket_type_t socket_type      = SERVER_SIDE;
    enum socket_type_t socket_type_temp = SERVER_SIDE;

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

        switch (c)
        {
        case 'r':
        {
            size_t idx  = (strncmp(argv[optind-0x01], "-r", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            socket_type = ( strncmp("server", argv[idx], 7 ) == 0 ) ? SERVER_SIDE : CLIENT_SIDE;
            break;
        }
        case 't':
        {
            size_t idx       = (strncmp(argv[optind-0x01], "-t", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            socket_type_temp = ( strncmp("server", argv[idx], 7 ) == 0 ) ? SERVER_SIDE : CLIENT_SIDE;
            break;
        }
        case 'i':
        {
            size_t idx  = (strncmp(argv[optind-0x01], "-i", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            if ( socket_type == socket_type_temp )
            {
                strncpy( socket_address->ip_addr, argv[idx], 14 );
                break;
            }
            else if ( socket_type == CLIENT_SIDE && socket_type_temp == SERVER_SIDE )
            {
                strncpy( socket_config->server.ip_addr, argv[idx], 14 );
                break;
            }
            else if ( socket_type == SERVER_SIDE && socket_type_temp == CLIENT_SIDE )
            {
                // strncpy( socket_config->server.ip_addr, argv[idx], 14 );
                break;
            }
            break;
        }
        case 'p':
        {
            size_t idx  = (strncmp(argv[optind-0x01], "-p", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            if ( socket_type == socket_type_temp )
            {
                socket_address->port = strtol( argv[idx], NULL, 10 );
                break;
            }
            else if ( socket_type == CLIENT_SIDE && socket_type_temp == SERVER_SIDE )
            {
                socket_config->server.port_addr = strtol( argv[idx], NULL, 10 );
                break;
            }
            else if ( socket_type == SERVER_SIDE && socket_type_temp == CLIENT_SIDE )
            {
                // strncpy( socket_config->server.ip_addr, argv[idx], 15 );
                break;
            }
            break;
        }
        case 'm':
        {
            size_t idx  = (strncmp(argv[optind-0x01], "-m", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            if ( socket_type == CLIENT_SIDE )
            {
                strncpy( buffer, argv[idx], buffer_size );
                break;
            }
            memset( buffer, 0, buffer_size );
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

    return socket_type;
}