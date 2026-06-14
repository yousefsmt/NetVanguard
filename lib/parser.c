#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <stdarg.h>
#include <linux/version.h>

#include "types.h"
#include "parser.h"

static const char name[] = "./netvanguard-cli";

static char *log_str[LOG_MAX + 1] =
{
    "DEBUG",
    "SUCCESS",
    "WARNING",
    "ERROR",
};

static char * colors[LOG_MAX + 1] = {
    "\x1b[0m",
    "\x1b[32m",
    "\x1b[1;33m",
    "\x1b[31m"
};

static struct tm *m_time; 
static time_t    current_time;

void handler_signal(int sig)
{
    const char msg[] = "Caught SIGINT\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    _exit(sig);
}

static void pr_sh_help(const char *prog_name)
{
    int maj = LINUX_VERSION_CODE >> 16;
    int min = ( LINUX_VERSION_CODE - ( maj << 16 ) ) >> 8;
    int pat = LINUX_VERSION_CODE - ( maj << 16 ) - ( min << 8 );

    printf(
        "NetVanguard-CLI v0.1.0 - Lightweight iptables Firewall\n"
        "Kernel Version: %d.%d.%d\n"
        "Copyright (C) 2026 Yoosef Samet\n"
        "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
        "This is free software: you are free to change and redistribute it.\n\n"
        "Usage: %s show [options]\n\n"
        "Options:\n"
        "  -v, --verbose               Show more detail about each rule\n",
        maj, min, pat, prog_name
    );
    
    exit(EXIT_SUCCESS);
}

static void pr_rm_help(const char *prog_name)
{
    int maj = LINUX_VERSION_CODE >> 16;
    int min = ( LINUX_VERSION_CODE - ( maj << 16 ) ) >> 8;
    int pat = LINUX_VERSION_CODE - ( maj << 16 ) - ( min << 8 );

    printf(
        "NetVanguard-CLI v0.1.0 - Lightweight iptables Firewall\n"
        "Kernel Version: %d.%d.%d\n"
        "Copyright (C) 2026 Yoosef Samet\n"
        "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
        "This is free software: you are free to change and redistribute it.\n\n"
        "Usage: %s remove-rule [options]\n\n"
        "Options:\n"
        "  -i, --index                 Apply rule to INPUT hook (pre-routing)\n"
        "Examples:\n"
        "  %s remove-rule -i 5\n"
        "      Remove rule stored on index of 5\n\n"
        "  %s remove-rule --index 3\n"
        "      Remove rule stored on index of 5 (same as above)\n\n",
        maj, min, pat, prog_name, prog_name, prog_name
    );
    
    exit(EXIT_SUCCESS);
}

static void pr_add_help(const char *prog_name)
{
    int maj = LINUX_VERSION_CODE >> 16;
    int min = ( LINUX_VERSION_CODE - ( maj << 16 ) ) >> 8;
    int pat = LINUX_VERSION_CODE - ( maj << 16 ) - ( min << 8 );

    printf(
        "NetVanguard-CLI v0.1.0 - Lightweight iptables Firewall\n"
        "Kernel Version: %d.%d.%d\n"
        "Copyright (C) 2026 Yoosef Samet\n"
        "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
        "This is free software: you are free to change and redistribute it.\n\n"
        "Usage: %s add-rule [options]\n\n"
        "Options:\n"
        "  -I, --input                 Apply rule to INPUT hook (pre-routing)\n"
        "  -O, --output                Apply rule to OUTPUT hook (post-routing)\n"
        "  -s, --source                Match traffic by source\n"
        "  -d, --dest                  Match traffic by destination\n"
        "  -t, --type <action>         Target action: ACCEPT, BLOCK, REJECT\n"
        "  -i, --ip <address>          Target IP address (e.g., 192.168.1.1)\n"
        "  -p, --port <port>           Target port number (0 - 65535)\n\n"
        "Examples:\n"
        "  %s add-rule -I -t ACCEPT -i 10.24.36.22 -p 80\n"
        "      Accept incoming traffic targeting 10.24.36.22 on port 80\n\n"
        "  %s add-rule -I -t BLOCK -i 10.24.10.10\n"
        "      Block all incoming traffic from IP 10.24.10.10\n\n"
        "  %s add-rule -O -t BLOCK -p 2020\n"
        "      Block all outgoing traffic on port 2020\n\n"
        "Requirements:\n"
        "  * Direction is mandatory: You must specify either -I (--input) or -O (--output).\n"
        "  * Criteria is mandatory: You must provide at least an IP address (-i) or a port (-p).\n",
        maj, min, pat, prog_name, prog_name, prog_name, prog_name
    );
    
    exit(EXIT_SUCCESS);
}

static void pr_main_help(const char *prog_name)
{
    int maj = LINUX_VERSION_CODE >> 16;
    int min = ( LINUX_VERSION_CODE - ( maj << 16 ) ) >> 8;
    int pat = LINUX_VERSION_CODE - ( maj << 16 ) - ( min << 8 );

    printf(
        "NetVanguard-CLI v0.1.0 - Lightweight iptables Firewall\n"
        "Kernel Version: %d.%d.%d\n"
        "Copyright (C) 2026 Yoosef Samet\n"
        "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
        "This is free software: you are free to change and redistribute it.\n\n"
        "Usage: %s <command> [options]\n\n"
        "Commands:\n"
        "  add-rule          Add a new firewall rule\n"
        "  remove-rule       Remove a specific rule by ID\n"
        "  show              Display active database rules\n"
        "  help              Display this help message and exit\n\n"
        "Report bugs to: <yoosefsamet@gmail.com>\n",
        maj, min, pat, prog_name
    );
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

static int cli_add_parser(struct van_cli_t* van_cli, int argc, char* argv[])
{
    int option_index;
    int c;

    if(argc==2)pr_add_help(name);

    memset(van_cli, 0, sizeof(struct van_cli_t));

    while (1)
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
            pr_add_help(name);
            break;
        default:
            ERROR( "argument parser returned character code 0%o is NOT valid!!!", c);
        }
    }

    return 0;
}

static int cli_rm_parser(struct van_cli_t* van_cli, int argc, char* argv[])
{
    int option_index;
    int c;

    if(argc==2)pr_rm_help(name);

    memset( van_cli, 0, sizeof( struct van_cli_t ) );

    while (1)
    {
        option_index = 0x00;

        static struct option long_options[] =
        {
            {"index",  required_argument, 0,  'i'},
            {0,        0,                 0,   0 }
        };

        c = getopt_long(argc, argv, "i", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 'i':
        {
            size_t idx = (strncmp(argv[optind-0x01], "-i", 0x02UL) == 0x00) ? (size_t)(optind) : (size_t)(optind-0x01);
            uint8_t id = strtol( argv[idx], NULL, 10 );
            van_cli->rules.flags = ( REMOVE_BYTE | id );
            break;
        }
        case '?':
            pr_rm_help(name);
            break;
        default:
            ERROR( "argument parser returned character code 0%o is NOT valid!!!", c);
        }
    }

    return 0;
}

static int cli_sh_parser(struct van_cli_t* van_cli, int argc, char* argv[])
{
    int option_index;
    int c;

    if(argc == 2)pr_sh_help(name);

    memset(van_cli, 0, sizeof(struct van_cli_t));

    while (1)
    {
        option_index = 0x00;

        static struct option long_options[] =
        {
            {"verbose",  required_argument, 0, 'v'},
            {0,          0,                 0,   0 }
        };

        c = getopt_long(argc, argv, "v", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 'v':
        {
            /* TODO: verbose handler*/
            break;
        }
        case '?':
            pr_sh_help(name);
            break;
        default:
            ERROR( "argument parser returned character code 0%o is NOT valid!!!", c);
        }
    }

    return 0;
}

void get_time() {
    time(&current_time);
    m_time = localtime(&current_time);
}

void pr_log( enum van_log_t level, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
        get_time();
        #ifndef NO_ANSI 
            printf("%s", colors[level]);
        #endif
        
        printf("[%d/%d/%d %d:%d:%d][%s] ", m_time -> tm_mday,
                                            m_time -> tm_mon,
                                            m_time -> tm_year + 1900,
                                            m_time -> tm_hour, 
                                            m_time -> tm_min, 
                                            m_time -> tm_sec,
                                            log_str[level]); 
        vfprintf(stdout, fmt, args);
        printf("\n%s", colors[DEBUG]);
    va_end(args);
}

int cli_parser(struct van_cli_t* van_cli, int argc, char* argv[])
{
    if (van_cli)
    {
        if (argc < 2)pr_main_help(name);
        size_t i = 0;
        const char *msg[] = { "add-rule", "remove-rule", "show", "help" };
        const size_t it[] = {9, 12, 5, 5};

        for (i = 0; i < 4; i++)
        {
            if(strncmp(argv[1], msg[i], it[i]) == 0)
                break;
        }

        switch (i)
        {
        case 0:
            return cli_add_parser(van_cli, argc, argv);
        case 1:
            return cli_rm_parser(van_cli, argc, argv);
        case 2:
            return cli_sh_parser(van_cli, argc, argv);
        case 3:
            pr_main_help(name);
            break;
        default:
            pr_main_help(name);
            break;
        }
        return 0;
    }
    else
    {
        ERROR( "config structure invalid" );
        return -1;
    }

    return 0;
}