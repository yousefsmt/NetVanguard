#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <getopt.h>
#include <time.h>

#include "types.h"
#include "tcp_handler.h"
#include "parser.h"

struct socket_address_t {
	char ip_addr[15];
	uint16_t port;
};

static void help(void *pname)
{
	printf("NetVanguard: TCP Simple Connection v0.1.0\n"
	       "Developed by: Yousef.smt\n\n"
	       "Usage: %s [OPTION]\n\n"
	       "Options:\n"
	       "  -r, --role <role>        Set socket role: 'server' or 'client'.\n"
	       "  -t, --type <type>        Set socket type: 'server' or 'client'.\n"
	       "  -i, --ip <address>       Set IP address (e.g., 192.168.1.1).\n"
	       "  -p, --port <port>        Set port number (0 - 65535).\n"
	       "  -m, --message <string>   Set the payload message.\n"
	       "                             - Client: Message sent to the server.\n"
	       "                             - Server: Custom echo response to the client.\n"
	       "  -h, --help               Display this help message and exit.\n",
	       (const char *)pname);
	exit(0x00);
}

static enum socket_type_t parse_args(struct socket_config_t *socket_config,
				     struct socket_address_t *socket_address,
				     int argc, char *argv[], char *buffer,
				     size_t buffer_size)
{
	enum socket_type_t socket_type = SERVER_SIDE;
	enum socket_type_t socket_type_temp = SERVER_SIDE;

	int option_index = 0x00;
	int c = 0x00;

	while (true) {
		option_index = 0x00;

		static struct option long_options[] = {
			{ "role", required_argument, 0, 'r' },
			{ "type", required_argument, 0, 't' },
			{ "ip", required_argument, 0, 'i' },
			{ "port", required_argument, 0, 'p' },
			{ "message", required_argument, 0, 'm' },
			{ 0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "rtipm", long_options,
				&option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'r': {
			size_t idx = (strncmp(argv[optind - 0x01], "-r",
					      0x02UL) == 0x00) ?
					     (size_t)(optind) :
					     (size_t)(optind - 0x01);
			socket_type = (strncmp("server", argv[idx], 7) == 0) ?
					      SERVER_SIDE :
					      CLIENT_SIDE;
			break;
		}
		case 't': {
			size_t idx = (strncmp(argv[optind - 0x01], "-t",
					      0x02UL) == 0x00) ?
					     (size_t)(optind) :
					     (size_t)(optind - 0x01);
			socket_type_temp =
				(strncmp("server", argv[idx], 7) == 0) ?
					SERVER_SIDE :
					CLIENT_SIDE;
			break;
		}
		case 'i': {
			size_t idx = (strncmp(argv[optind - 0x01], "-i",
					      0x02UL) == 0x00) ?
					     (size_t)(optind) :
					     (size_t)(optind - 0x01);
			if (socket_type == socket_type_temp) {
				strncpy(socket_address->ip_addr, argv[idx], 14);
				break;
			} else if (socket_type == CLIENT_SIDE &&
				   socket_type_temp == SERVER_SIDE) {
				strncpy(socket_config->server.ip_addr,
					argv[idx], 14);
				break;
			} else if (socket_type == SERVER_SIDE &&
				   socket_type_temp == CLIENT_SIDE) {
				// strncpy( socket_config->server.ip_addr, argv[idx], 14 );
				break;
			}
			break;
		}
		case 'p': {
			size_t idx = (strncmp(argv[optind - 0x01], "-p",
					      0x02UL) == 0x00) ?
					     (size_t)(optind) :
					     (size_t)(optind - 0x01);
			if (socket_type == socket_type_temp) {
				socket_address->port =
					strtol(argv[idx], NULL, 10);
				break;
			} else if (socket_type == CLIENT_SIDE &&
				   socket_type_temp == SERVER_SIDE) {
				socket_config->server.port_addr =
					strtol(argv[idx], NULL, 10);
				break;
			} else if (socket_type == SERVER_SIDE &&
				   socket_type_temp == CLIENT_SIDE) {
				// strncpy( socket_config->server.ip_addr, argv[idx], 15 );
				break;
			}
			break;
		}
		case 'm': {
			size_t idx = (strncmp(argv[optind - 0x01], "-m",
					      0x02UL) == 0x00) ?
					     (size_t)(optind) :
					     (size_t)(optind - 0x01);
			if (socket_type == CLIENT_SIDE) {
				strncpy(buffer, argv[idx], buffer_size - 1);
				break;
			}
			memset(buffer, 0, buffer_size);
			break;
		}
		case '?':
			help(argv[optind - 0x01]);
			break;
		default:
			ERROR("argument parser returned character code 0%o is NOT valid!!!",
			      c);
		}
	}

	if (argc == 1) {
		help("no args");
	}

	return socket_type;
}

static int tcp_socket_transfer(struct socket_config_t *socket_config,
			       enum socket_type_t socket_type, char *buffer,
			       const size_t buffer_size)
{
	switch (socket_type) {
	case SERVER_SIDE: {
		ssize_t read_byte;

		read_byte = tcp_socket_read(socket_config, buffer, buffer_size);
		if (read_byte < 0) {
			ERROR("read error\n");
			return -1;
		}
		SUCCESS("Read %ld bytes message[%s]\n", read_byte, buffer);
		break;
	}
	case CLIENT_SIDE: {
		ssize_t send_byte;

		send_byte = tcp_socket_send(socket_config, buffer, buffer_size);
		if (send_byte < 0) {
			SUCCESS("send error\n");
			return -1;
		}
		SUCCESS("Send %ld bytes message[%s]\n", send_byte, buffer);
		break;
	}
	default:
		ERROR("socket_type invalid\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct socket_address_t socket_address;
	struct socket_config_t socket_config;
	char buffer[1024];
	enum socket_type_t socket_type;
	int ret;

	socket_type = parse_args(&socket_config, &socket_address, argc, argv,
				 buffer, 1024);
	SUCCESS("Role side: %s",
		(socket_type == SERVER_SIDE) ? "SERVER_SIDE" : "CLIENT_SIDE");
	SUCCESS("IP Address: %s", socket_address.ip_addr);
	SUCCESS("Port Address: %d", socket_address.port);
	SUCCESS("Message: %s", buffer);

	ret = tcp_socket_init(&socket_config, socket_type,
			      socket_address.ip_addr, socket_address.port);
	if (ret < 0) {
		printf("init error\n");
		return -1;
	}

	ret = tcp_socket_transfer(&socket_config, socket_type, buffer, 1024);
	if (ret < 0) {
		printf("transfer error\n");
		return -1;
	}

	ret = tcp_socket_close(&socket_config, socket_type);
	if (ret < 0) {
		printf("close error\n");
		return -1;
	}

	return 0;
}