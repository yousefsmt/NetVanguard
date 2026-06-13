#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <arpa/inet.h>

#include "types.h"
#include "parser.h"
#include "netlink_handler.h"

static void     pr_help( const char* str );
static int      cli_parser( struct van_cli_t* van_cli, int argc, char* argv[] );
static uint8_t  get_mode( const char* argv );

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

static void pr_help( const char* str )
{
    printf(
    "NetVanguard-CLI: Lightweight FireWall ( simple iptable ) v0.1.0\n"
    "Developed by: Yousef.smt\n\n"
    "Usage: %s command [OPTION]\n\n"
    "Commands:\n"
    "  add-rule                    Add new rule.\n"
    "  remove-rule                 Remove specific rule ID.\n"
    "  show                        Show database rules.\n\n"
    "  help                        Display this help message and exit.\n\n"
    "Options:\n"
    "                              Which side must be block?\n"
    "  -I, --input                       - Input before pre-routing hook\n"
    "  -O, --output                      - Output post-routing hook\n"
    "  -s, --source                Source mode\n"
    "  -d, --dest                  Destination mode\n"
    "  -t, --type <type>           Set IP address (ACCEPT, BLOCK, REJECT).\n"
    "  -i, --ip <address>          Set IP address (e.g., 192.168.1.1).\n"
    "  -p, --port <port>           Set port number (0 - 65535).\n\n"
    "Example:\n"
    "./netvanguard-cli add-rule -I -t ACCEPT -i 10.24.36.22 -p 80  # Set accept rule set in input routing\n"
    "./netvanguard-cli add-rule -I -t BLOCK  -i 10.24.10.10        # Set block rule in input routing block all IP source from any port\n"
    "./netvanguard-cli add-rule -O -t BLOCK  -p 2020               # Set block rule in output host block all output packet from 2020 port\n\n"
    "NOTE: \n"
    "1-You must set at least one command.\n"
    "2-After the add-rule argument, it is mandatory for the blocking side and any one of the args ( ip or port )\n",
    str );
    exit(0x00);
}

static uint8_t get_mode( const char* argv )
{
    const char *_type[] = { "", "ACCEPT", "BLOCK", "REJECT" };
    uint8_t type;
    for (type = 1; type < 4; type++)
    {
        if ( strncmp( argv, _type[type], strlen( _type[type] ) ) == 0 )
        {   
            break;
        }
    }

    return type;
}

static int cli_parser( struct van_cli_t* van_cli, int argc, char* argv[] )
{
    if ( van_cli )
    {
        int    option_index;
        int    c;
        size_t i = 0;
        char *msg[] = { "add-rule", "remove-rule", "show", "help", NULL };

        memset( van_cli, 0, sizeof( struct van_cli_t ) );

        if ( argc < 2 ){ pr_help("no args"); }

        while ( msg[i] )
        {
            if ( strncmp( argv[1], msg[i], strlen( argv[1] ) ) == 0 )
            {
                break;
            }
            i++;
        }

        if ( i > 3 ){return -1;}

        switch ( i )
        {
        case 0:
            break;
        case 1:
            uint8_t id = strtol( argv[2], NULL, 10 );
            van_cli->rules.flags = ( REMOVE_BYTE | id );
            return 0;
        case 2:
            /* TODO: must show database*/
             pr_help("!!!! Not Supported !!!!");
            break;
        case 3:
            pr_help("help");
            break;
        
        default:
            break;
        }
        
        while ( msg[i] )
        {
            option_index = 0x00;

            static struct option long_options[] =
            {
                {"input",  no_argument,       0,  'I'},
                {"output", no_argument,       0,  'O'},
                {"source", no_argument,       0,  's'},
                {"dest",   no_argument,       0,  'd'},
                {"type",   required_argument, 0,  't'},
                {"ip",     required_argument, 0,  'i'},
                {"port",   required_argument, 0,  'p'},
                {0,        0,                 0,   0 }
            };

            c = getopt_long(argc, argv, "IOsdtip", long_options, &option_index);
            if (c == -1)
                break;

            switch (c)
            {
            case 'I':
            {
                van_cli->rules.flags |= SET_SIDE( INPUT );
                break;
            }
            case 'O':
            {
                van_cli->rules.flags |= SET_SIDE( OUTPUT );
                break;
            }
            case 's':
            {
                van_cli->rules.flags |= SET_HOOK_TYPE( SOURCE );
                break;
            }
            case 'd':
            {
                van_cli->rules.flags |= SET_HOOK_TYPE( DESTINATION );
                break;
            }
            case 't':
            {
                size_t idx = (strncmp(argv[optind-0x01], "-t", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
                van_cli->rules.flags |= SET_RULE_TYPE( get_mode( argv[idx] ) );
                break;
            }
            case 'i':
            {
                size_t idx = (strncmp(argv[optind-0x01], "-i", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
                van_cli->rules.ip = inet_addr( argv[idx] );
                break;
            }
            case 'p':
            {
                size_t idx = (strncmp(argv[optind-0x01], "-p", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
                van_cli->rules.port = strtol( argv[idx], NULL, 10 ); 
                break;
            }
            case '?':
                pr_help(argv[optind-0x01]);
                break;
            default:
                ERROR( "argument parser returned character code 0%o is NOT valid!!!", c);
            }
        }

        if ( argc == 1 ){pr_help("no args");}

        return 0;
    }
    else
    {
        ERROR( "config structure invalid" );
        return -1;
    }

    return 0;

}