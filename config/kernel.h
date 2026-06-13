#ifndef NETVANGUARD_CONFIG_KERNEL_H_
#define NETVANGUARD_CONFIG_KERNEL_H_

#define FW_NETLINK_NAME    "NETVANGUARD"
#define FW_NETLINK_VERSION ( 1 )
#define FW_GEN_PRE_ALLOC ( ( size_t )10 )

#define SET_SIDE( x )       ( ( x << 4 ) & 0x30 )
#define SET_HOOK_TYPE( x )  ( ( x << 2 ) & 0x0c )
#define SET_RULE_TYPE( x )  ( x  & 0x03 )

#define GET_SIDE( x )      ( ( x & 0x30 ) >> 4 )  
#define GET_HOOK_TYPE( x ) ( ( x & 0x0c ) >> 2 )
#define GET_RULE_TYPE( x ) ( x & 0x03 )

#define REMOVE_BYTE ( 0xC0 )
#define GET_ID( x ) ( x & 0x3f )

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

struct __attribute__((__packed__)) van_str_rule_t
{
    uint32_t ip;
    uint16_t port;
    uint8_t  flags;
};

#define NL_PACKET_SIZE ( sizeof( struct van_str_rule_t ) )

enum van_cmd_t
{
    FW_CMD_UNSPEC,
    FW_CMD_REQUEST,
    FW_CMD_RESPONSE,
    FW_CMD_REMOVE,
    __FW_CMD_MAX
};

#define FW_CMD_MAX (__FW_CMD_MAX - 1)

enum van_attr_t
{
    FW_ATTR_UNSPEC,
    FW_ATTR_IP,
    FW_ATTR_PORT,
    FW_ATTR_FLAG,
    FW_ATTR_ACK,
    __FW_ATTR_MAX
};

#define FW_ATTR_MAX (__FW_ATTR_MAX - 1)

#endif /* NETVANGUARD_CONFIG_KERNEL_H_ */