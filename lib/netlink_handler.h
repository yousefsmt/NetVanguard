#ifndef NETVANGUARD_LIB_NETLINK_HANDLER_H_
#define NETVANGUARD_LIB_NETLINK_HANDLER_H_

#include <netlink/netlink.h>
#include "kernel.h"

#define BUFFER_MAPPER_SIZE ((size_t)25)

int netlink_socket_init(struct nl_sock **socket);

int netlink_socket_init_cb(struct nl_sock **socket);

int netlink_socket_pack_msg(struct nl_sock **socket, struct nl_msg **msg,
			    void **hdr, struct van_str_rule_t *rules,
			    int family_id, int cmd);

int netlink_socket_send_msg(struct nl_sock **socket, struct nl_msg **msg);

int netlink_socket_recv_msg(struct nl_sock **socket);

int netlink_socket_free(struct nl_sock **socket, struct nl_msg **msg);

#endif /* NETVANGUARD_LIB_NETLINK_HANDLER_H_ */