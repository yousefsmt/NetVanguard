#ifndef NETVANGUARD_CONFIG_TYPES_H_
#define NETVANGUARD_CONFIG_TYPES_H_

#include <stdint.h>
#include <netlink/netlink.h>
#include "kernel.h"

#define SET_SIDE( x )       ( ( x << 4 ) & 0x30 )
#define SET_HOOK_TYPE( x )  ( ( x << 2 ) & 0x0c )
#define SET_RULE_TYPE( x )  ( x  & 0x03 )

#define GET_SIDE( x )      ( ( x & 0x30 ) >> 4 )  
#define GET_HOOK_TYPE( x ) ( ( x & 0x0c ) >> 2 )
#define GET_RULE_TYPE( x ) ( x & 0x03 )

enum van_side_t
{
    __UNSPEC_SIDE,
    INPUT,
    OUTPUT,
    __SIDE_MAX
};

#define SIDE_MAX ( __SIDE_MAX - 1 )

enum van_hook_t
{
    __UNSPEC_HOOK,
    SOURCE,
    DESTINATION,
    __HOOK_MAX
};

#define HOOK_MAX ( __HOOK_MAX - 1 )

enum van_rule_t
{
    __UNSPEC_RULE,
    ACCEPT,
    BLOCK,
    REJECT,
    __RULE_MAX
};

#define RULE_MAX ( __RULE_MAX - 1 )

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