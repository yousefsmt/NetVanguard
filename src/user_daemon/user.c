#include <arpa/inet.h>
#include "uds_handler.h"
#include "netlink_handler.h"
#include "parser.h"
#include "db.h"

#define DAEMON_ADDR "/tmp/netvanguardd"
#define DAEMON_ADDR_LEN ( 18 )

int main() {
	struct van_str_rule_t rules;
	struct uds_config_t uds_config;
	struct nl_sock *socket;
	struct nl_msg *msg;
	void *hdr;
	size_t str_rule_len = sizeof(struct van_str_rule_t);
	sqlite3 *db;
	int err;
	int family_id;

	signal(SIGINT, handler_signal);
	signal(SIGKILL, handler_signal);
	signal(SIGTERM, handler_signal);
	signal(SIGSEGV, handler_signal);

	err = param_init(&db, &uds_config, &socket, &msg);
	if(err < 0) {
		ERROR("init database failed!");
		return -1;
	}

	err = sql_init(&db);
	if(err < 0) {
		ERROR("init database failed!");
		return -1;
	}

	err = uds_socket_init(&uds_config, DAEMON_ADDR, DAEMON_ADDR_LEN);
	if(err < 0) {
		ERROR("init uds failed!");
		return -1;
	}
	SUCCESS("UDS socket initialize successfull with address [%s]", DAEMON_ADDR);

	family_id = netlink_socket_init(&socket);
	SUCCESS("netlink initilaize and resolve complete with family ID: %d", family_id);

	err = netlink_socket_init_cb(&socket);
	if (err < 0) {
		ERROR("Adding callback error occur!!\n");
		return -1;
	}
	SUCCESS("netlink add callback for reply message");

	while (1)
	{
		err = uds_socket_recv(&uds_config, &rules, str_rule_len);
		if(err < 0) {
			ERROR("uds recv failed!");
			return -1;
		}
		SUCCESS("UDS socket receive new rule");

		if ((rules.flags & REMOVE_BYTE) == REMOVE_BYTE) {
		err = netlink_socket_pack_msg(&socket, &msg,
					      &hdr, &rules,
					      family_id, FW_CMD_REMOVE);
		if (err < 0) {
			ERROR("During packing message error occur!!\n");
			return -1;
		}
		SUCCESS("netlink packed remover message");

		} else {
			err = netlink_socket_pack_msg(&socket, &msg,
							&hdr, &rules,
							family_id, FW_CMD_REQUEST);
			if (err < 0) {
				ERROR("During packing message error occur!!\n");
				return -1;
			}
			SUCCESS("netlink packed add rule to kernel module");
		}
		err = netlink_socket_send_msg(&socket, &msg);
		if (err < 0) {
			ERROR("During sending message error occur!!\n");
			return -1;
		}
		SUCCESS("netlink sent new rule to kernel");

		err = netlink_socket_recv_msg(&socket);
		if (err < 0) {
			ERROR("During receiving message error occur!!\n");
			return -1;
		}
		SUCCESS("user receive response from kernel");

		err = sql_add_rule(&db, &rules);
		if(err < 0) {
			ERROR("add rule database failed!");
			return -1;
		}
		SUCCESS("add rule to SQL database successfull");
	}

	return 0;
}