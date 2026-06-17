#include "tcp_handler.h"
#include "parser.h"

int main()
{
	struct socket_config_t config;
	const char ip_addr[] = "127.0.1.10";
	const uint16_t port = 2025;
	char msg[25];
	int err;

	err = tcp_socket_init(&config, CLIENT_SIDE, ip_addr, port);
	if (err < 0) {
		ERROR("tcp client socket init failed!");
		return -1;
	}
	SUCCESS("tcp client scoket init successfull with ip: %s and port: %s",
		ip_addr, port);

	strncpy(msg, "Hello Server!!", 15);
	ssize_t send_bytes = tcp_socket_send(&config, msg, 15, CLIENT_SIDE);
	if (send_bytes < 0) {
		ERROR("tcp client socket send failed!");
		return -1;
	}
	SUCCESS("tcp client socket send %ld bytes succesfull [%s] message",
		send_bytes, msg);

	ssize_t recv_bytes = tcp_socket_read(&config, msg, 25, CLIENT_SIDE);
	if (recv_bytes < 0) {
		ERROR("tcp client recv failed!");
		return -1;
	}
	SUCCESS("tcp client recv %ld bytes successfull [%s] message",
		recv_bytes, msg);

	err = tcp_socket_close(&config, CLIENT_SIDE);
	if (err < 0) {
		ERROR("tcp client close failed!");
		return -1;
	}
	SUCCESS("tcp client socket close successfull");

	return 0;
}