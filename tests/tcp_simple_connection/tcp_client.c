#include "tcp_handler.h"
#include "parser.h"

int main(int argc, char *argv[])
{
	struct socket_config_t config;
	char msg[25];
	char ip_addr[17];
	uint16_t port;
	int err;

	if (argc < 5) {
		ERROR("you must set --ip and --port argument");
		return -1;
	}

	if (strncmp(argv[1], "--ip", 5) != 0) {
		ERROR("first argument must be --ip <ip address>");
		return -1;
	}

	if (strncmp(argv[3], "--port", 7) != 0) {
		ERROR("third argument must be --port <port address>");
		return -1;
	}

	strncpy(ip_addr, argv[2], 16);
	port = strtol(argv[4], NULL, 10);

	err = tcp_socket_init(&config, CLIENT_SIDE, ip_addr, port);
	if (err < 0) {
		ERROR("tcp client socket init failed!");
		return -1;
	}
	SUCCESS("tcp client scoket init successfull with ip: %s and port: %d",
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