#ifndef NETVANGUARD_INCLUDE_KERNEL_H_
#define NETVANGUARD_INCLUDE_KERNEL_H_

#define FW_NETLINK_NAME    "NETVANGUARD"
#define FW_NETLINK_VERSION ( 1 )

enum fw_commands
{
    FW_CMD_UNSPEC,
    FW_CMD_BLOCK_IP,
    __FW_CMD_MAX,
};

#define FW_CMD_MAX (__FW_CMD_MAX - 1)


enum fw_attributes
{
    FW_ATTR_UNSPEC,
    FW_ATTR_SRC_IP,
    __FW_ATTR_MAX,
};

#define FW_ATTR_MAX (__FW_ATTR_MAX - 1)

#endif /* NETVANGUARD_INCLUDE_KERNEL_H_ */