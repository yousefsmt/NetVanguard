#ifndef NETVANGUARD_LIB_INCLUDE_NETLINK_HANDLER_H_
#define NETVANGUARD_LIB_INCLUDE_NETLINK_HANDLER_H_

#include <netlink/cli/utils.h>
#include "include/kernel_module/kernel.h"

int netlink_socket_init( struct nl_sock *sock );
int netlink_socket_pack_msg( struct nl_msg *msg, void *hdr, int family_id, int attrtype, uint32_t ip_address );
int netlink_socket_send_msg( struct nl_sock *sock, struct nl_msg *msg );
int netlink_socket_free( struct nl_sock *sock, struct nl_msg *msg );

#endif /* NETVANGUARD_LIB_INCLUDE_NETLINK_HANDLER_H_ */