#include "uds_handler.h"
#include "parser.h"

#define SERVER_ADDR "/tmp/server"
#define CLIENT_ADDR "/tmp/client"

#define ADDR_LEN (sizeof(SERVER_ADDR))

#define BUFFER_SIZE (24)

int main()
{
	struct uds_config_t uds_config;
	char send_msg[BUFFER_SIZE];
	int err;

	err = uds_socket_init(&uds_config, CLIENT_ADDR, ADDR_LEN);
	if (err < 0) {
		ERROR("init uds failed!");
		return -1;
	}
	SUCCESS("uds client socket create with %s address", CLIENT_ADDR);

	strncpy(send_msg, "Hello Server!!", 15);
	ssize_t send_bytes = uds_socket_send(&uds_config, send_msg, 15,
					     SERVER_ADDR, ADDR_LEN);
	if (send_bytes < 0) {
		ERROR("uds send error!");
		return -1;
	}
	SUCCESS("uds client send %ld bytes successfull [%s] message",
		send_bytes, send_msg);

	ssize_t recv_bytes =
		uds_socket_recv(&uds_config, send_msg, BUFFER_SIZE);
	if (recv_bytes < 0) {
		ERROR("uds recv failed!");
		return -1;
	}
	SUCCESS("uds client recv %ld bytes succesfull [%s] message", recv_bytes,
		send_msg);

	err = uds_socket_close(&uds_config);
	if (err < 0) {
		ERROR("uds close error!");
		return -1;
	}
	SUCCESS("uds client close successfull");

	return 0;
}