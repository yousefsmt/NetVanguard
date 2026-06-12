#ifndef NETVANGUARD_CONFIG_TYPES_H_
#define NETVANGUARD_CONFIG_TYPES_H_

#include <stdint.h>
#include <netlink/netlink.h>
#include "kernel.h"

enum van_log_t
{
    DEBUG,
    SUCCESS,
    WARNING,
    ERROR,
    __LOG_MAX
};

#define LOG_MAX ( __LOG_MAX - 1 )

enum van_prog_t
{
    VANGUARD_TEST_TCP,
    VANGUARD_TEST_NL,
    VANGUARD_CLI,
    VANGUARD_DAEMON,
    __VANGUARD_MAX
};

#define VANGUARD_MAX ( __VANGUARD_MAX - 1 )

struct van_cli_t
{
    struct van_str_rule_t rules;
    struct nl_sock       *socket;
    struct nl_msg        *msg;
    void                 *hdr;
};

typedef void (*help_t)( const char* );
typedef void (*parser_t)( void*, int, char* );

#ifdef __EMSCRIPTEN
    #define NO_ANSI
#endif

#endif /* NETVANGUARD_CONFIG_TYPES_H_ */