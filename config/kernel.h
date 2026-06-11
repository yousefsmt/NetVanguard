#ifndef NETVANGUARD_CONFIG_KERNEL_H_
#define NETVANGUARD_CONFIG_KERNEL_H_

#define FW_NETLINK_NAME    "NETVANGUARD"
#define FW_NETLINK_VERSION ( 1 )
#define FW_GEN_PRE_ALLOC ( ( size_t )10 )

struct __attribute__((__packed__)) van_str_rule_t
{
    uint32_t ip;
    uint16_t port;
    uint8_t  flags;
};

#define NL_PACKET_SIZE ( sizeof( struct van_str_rule_t ) )

enum van_reply_ip_t
{
    FW_REP_SRC_BLOCK_IP,
    FW_REP_DEST_BLOCK_IP,
    FW_REP_SRC_ACCEPT_IP,
    FW_REP_DEST_ACCEPT_IP,
    FW_REP_SRC_BLOCK_PORT,
    FW_REP_DEST_BLOCK_PORT,
    FW_REP_SRC_ACCEPT_PORT,
    FW_REP_DEST_ACCEPT_PORT,
    FW_REP_ICMP,
    __FW_REP_MAX
};

#define FW_REP_MAX (__FW_REP_MAX - 1)

enum van_cmd_t
{
    FW_CMD_UNSPEC,
    FW_CMD_REQUEST,
    FW_CMD_RESPONSE,
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