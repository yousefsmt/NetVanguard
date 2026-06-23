#include "parser.h"
#include "uds_handler.h"

#define DAEMON_ADDR "/tmp/netvanguardd"
#define DAEMON_ADDR_LEN ( 18 )

#define USER_ADDR "/tmp/user"
#define USER_ADDR_LEN ( 10 )

int main(int argc, char *argv[])
{
	struct van_str_rule_t van_cli;
	struct uds_config_t uds_config;
	size_t len = sizeof(struct van_str_rule_t);
	int err;

	err = cli_parser(&van_cli, argc, argv);
	if (err < 0) {
		ERROR("Something is wrong, cannot parse the arguments. Please check all the values of arguments with examples very carefully!!");
		return -1;
	}

	err = uds_socket_init(&uds_config, USER_ADDR, USER_ADDR_LEN);
	if (err < 0) {
		ERROR("uds socket failed");
		return -1;
	}

	err = uds_socket_send(&uds_config, &van_cli, len, DAEMON_ADDR, DAEMON_ADDR_LEN);
	if (err < 0) {
		ERROR("uds socket send failed");
		return -1;
	}

	err = uds_socket_close(&uds_config);
	if (err < 0) {
		ERROR("uds socket close failed");
		return -1;
	}


	return 0;
}