#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "types.h"
#include "parser.h"

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

int handler_options( void *config, int argc, char *argv[],
                     help_t cb, parser_t parser, int type )
{
    ( void )config;
    ( void )argc;
    ( void )argv;
    ( void )parser;

    cb( "Hello" );

    switch ( type )
    {
    case VANGUARD_TEST_TCP:
    {

    }
    case VANGUARD_TEST_NL:
    {

    }
    case VANGUARD_CLI:
    {

    }
    case VANGUARD_DAEMON:
    {

    }
    default:
        ERROR( "type argumnet in handle options invalid" );
        return -1;
    }

    return 0;
}

void get_time() {
    time(&current_time);
    m_time = localtime(&current_time);
}

void debug_msg( enum van_log_t level, const char * fmt, ...)
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