#include <arpa/inet.h>
#include <unistd.h>
#include "uds_handler.h"
#include "netlink_handler.h"
#include "parser.h"
#include "db.h"

#define DAEMON_ADDR "/tmp/netvanguardd"
#define DAEMON_ADDR_LEN ( 18 )

static sqlite3 *db;
static struct uds_config_t uds_config;
static struct nl_sock *nl_socket;
static struct nl_msg *msg;

static void freeup_resource(sqlite3 **db,
	                        struct uds_config_t *config,
							struct nl_sock **socket,
							struct nl_msg ** msg);

static void handler_signal(int sig);

int main() {
	struct van_str_rule_t rules;
	void *hdr;
	size_t str_rule_len = sizeof(struct van_str_rule_t);
	int err;
	int family_id;

	signal(SIGINT, handler_signal);
	signal(SIGKILL, handler_signal);
	signal(SIGTERM, handler_signal);
	signal(SIGSEGV, handler_signal);

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

	family_id = netlink_socket_init(&nl_socket);
	SUCCESS("netlink initilaize and resolve complete with family ID: %d", family_id);

	err = netlink_socket_init_cb(&nl_socket);
	if (err < 0) {
		ERROR("Adding callback error occur!!\n");
		return -1;
	}
	SUCCESS("netlink add callback for reply message");

	while (1) {
		err = uds_socket_recv(&uds_config, &rules, str_rule_len);
		if(err < 0) {
			ERROR("uds recv failed!");
			return -1;
		}
		SUCCESS("UDS socket receive new rule");

		if ((rules.flags & REMOVE_BYTE) == REMOVE_BYTE) {
			err = netlink_socket_pack_msg(&nl_socket, &msg,
							&hdr, &rules,
							family_id, FW_CMD_REMOVE);
			if (err < 0) {
				ERROR("During packing message error occur!!\n");
				return -1;
			}
			SUCCESS("netlink packed remover message");

		} else {
			err = netlink_socket_pack_msg(&nl_socket, &msg,
							&hdr, &rules,
							family_id, FW_CMD_REQUEST);
			if (err < 0) {
				ERROR("During packing message error occur!!\n");
				return -1;
			}
			SUCCESS("netlink packed add rule to kernel module");
		}
		err = netlink_socket_send_msg(&nl_socket, &msg);
		if (err < 0) {
			ERROR("During sending message error occur!!\n");
			return -1;
		}
		SUCCESS("netlink sent new rule to kernel");

		err = netlink_socket_recv_msg(&nl_socket);
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

static void freeup_resource(sqlite3 **db,
	                        struct uds_config_t *config,
							struct nl_sock **socket,
							struct nl_msg ** msg) {
	int err;

	err = sql_close(db);
	if(err < 0) {
		ERROR("SQL database closing failed!!");
	}
	SUCCESS("SQL database successfull closed");

	err = uds_socket_close(config);
	if(err < 0) {
		ERROR("uds socket closing failed!!");
	}
	SUCCESS("UDS socket successfull closed");

	err = netlink_socket_free(socket, msg);
	if(err < 0) {
		ERROR("netlink closing failed!!");
	}
	SUCCESS("netlink socket successfull closed");
	SUCCESS("***** Close Successfull *****");
}

static void handler_signal(int sig) {
	switch (sig)
	{
	case SIGINT: {
		const char msg_str[] = "Receiving SIGINT signal close daemon ...\n";
		write(STDOUT_FILENO, msg_str, sizeof(msg_str) - 1);
		break;
	}
	case SIGKILL: {
		const char msg_str[] = "Receiving SIGKILL signal close daemon ...\n";
		write(STDOUT_FILENO, msg_str, sizeof(msg_str) - 1);
		break;
	}
	case SIGTERM: {
		const char msg_str[] = "Receiving SIGTERM signal close daemon ...\n";
		write(STDOUT_FILENO, msg_str, sizeof(msg_str) - 1);
		break;
	}
	case SIGSEGV: {
		const char msg_str[] = "Receiving SIGSEGV signal close daemon ...\n";
		write(STDOUT_FILENO, msg_str, sizeof(msg_str) - 1);
		break;
	}
	default:
		ERROR("Receiving signal invalid!!!!");
		break;
	}
	freeup_resource(&db, &uds_config, &nl_socket, &msg);
	_exit(sig);
}