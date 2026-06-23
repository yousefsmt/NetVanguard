#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "types.h"
#include "tcp_handler.h"
#include "parser.h"

ssize_t tcp_socket_read(struct socket_config_t *socket_config, char *buffer,
			const size_t buffer_size,
			enum socket_type_t socket_type)
{
	ssize_t read_byte = 0;
	memset(buffer, 0, buffer_size);
	if (socket_config) {
		switch (socket_type) {
		case SERVER_SIDE: {
			read_byte = recv(socket_config->_socket_fd, buffer,
					 buffer_size, 0);
			if (read_byte < 0) {
				perror("read()");
				close(socket_config->socket_fd);
				close(socket_config->_socket_fd);
				return -1;
			}
			SUCCESS("socket with %d id read %ld bytes",
				socket_config->_socket_fd, read_byte);
			break;
		}
		case CLIENT_SIDE: {
			read_byte = recv(socket_config->socket_fd, buffer,
					 buffer_size, 0);
			if (read_byte < 0) {
				perror("read()");
				close(socket_config->socket_fd);
				close(socket_config->_socket_fd);
				return -1;
			}
			SUCCESS("socket with %d id read %ld bytes",
				socket_config->socket_fd, read_byte);
			break;
		}
		default:
			return -1;
		}
	} else {
		ERROR("config invalid!!");
	}
	SUCCESS("\tMessage: %s, Length: %ld", buffer, buffer_size);

	return read_byte;
}

ssize_t tcp_socket_send(struct socket_config_t *socket_config,
			const char *message, size_t message_len,
			enum socket_type_t socket_type)
{
	ssize_t send_byte = 0;
	if (socket_config) {
		switch (socket_type) {
		case SERVER_SIDE: {
			send_byte = send(socket_config->_socket_fd, message,
					 message_len, 0);
			if (send_byte < 0) {
				perror("send()");
				close(socket_config->socket_fd);
				return -1;
			}
			SUCCESS("socket with %d id send %ld bytes",
				socket_config->_socket_fd, send_byte);
			break;
		}
		case CLIENT_SIDE: {
			send_byte = send(socket_config->socket_fd, message,
					 message_len, 0);
			if (send_byte < 0) {
				perror("send()");
				close(socket_config->socket_fd);
				return -1;
			}
			SUCCESS("socket with %d id send %ld bytes",
				socket_config->socket_fd, send_byte);
			break;
		}
		default:
			return -1;
		}

	} else {
		ERROR("config invalid!!");
	}
	SUCCESS("\tMessage: %s, Length: %ld", message, message_len);

	return send_byte;
}

static int tcp_socket_listening(struct socket_config_t *socket_config,
				int max_connections)
{
	int ret;

	ret = listen(socket_config->socket_fd, max_connections);
	if (ret < 0) {
		perror("listen()");
		close(socket_config->socket_fd);
		return -1;
	}
	SUCCESS("scoket[%d] on listen mode with %d connections",
		socket_config->socket_fd, max_connections);

	socket_config->_socket_fd =
		accept(socket_config->socket_fd,
		       (struct sockaddr *)&socket_config->addr,
		       &socket_config->length);
	if (socket_config->_socket_fd < 0) {
		perror("accept()");
		close(socket_config->socket_fd);
		close(socket_config->_socket_fd);
		return -1;
	}
	SUCCESS("scoket[%d] accept connection and create new socket with %d id",
		socket_config->socket_fd, socket_config->_socket_fd);

	return 0;
}

static int tcp_socket_connecting(struct socket_config_t *socket_config)
{
	struct sockaddr_in cli_addr;
	socklen_t cli_len;
	int ret;

	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons(2030);
	ret = inet_pton(AF_INET, "127.0.10.20", &cli_addr.sin_addr.s_addr);
	if (ret < 0) {
		perror("inet_pton()");
		return -1;
	}
	cli_len = sizeof(struct sockaddr_in);

	ret = connect(socket_config->socket_fd,
		      (const struct sockaddr *)&cli_addr, cli_len);
	if (ret < 0) {
		perror("connect()");
		close(socket_config->socket_fd);
		return -1;
	}
	SUCCESS("socket with %d id connect to server with:",
		socket_config->socket_fd);
	SUCCESS("\tIP Address: 127.0.10.20");
	SUCCESS("\tPort Address: 2030");

	return 0;
}

int tcp_socket_init(struct socket_config_t *socket_config,
		    enum socket_type_t socket_type, const char *ip_addr,
		    const uint16_t port_addr)
{
	int ret;
	size_t ip_addr_len;

	ip_addr_len = strlen(ip_addr);
	if (ip_addr_len > 15) {
		ERROR("bind ip address exceeded from max len[15] ( you set: %ld)",
		      ip_addr_len);
		return -1;
	}

	socket_config->addr.sin_family = AF_INET;
	ret = inet_pton(AF_INET, ip_addr, &socket_config->addr.sin_addr.s_addr);
	if (ret < 0) {
		perror("inet_pton()");
		ERROR("ip address not valid [%s]", ip_addr);
		return -1;
	}
	socket_config->addr.sin_port = htons(port_addr);
	socket_config->length = sizeof(struct sockaddr_in);

	socket_config->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_config->socket_fd < 0) {
		perror("socket()");
		ERROR("socket doesn't create");
		close(socket_config->socket_fd);
		return -1;
	}
	SUCCESS("create socket with [ID: %d], [IP: %s], [Port: %d]",
		socket_config->socket_fd, ip_addr, port_addr);

	ret = bind(socket_config->socket_fd,
		   (const struct sockaddr *)&socket_config->addr,
		   socket_config->length);
	if (ret < 0) {
		perror("bind()");
		close(socket_config->socket_fd);
		return -1;
	}

	SUCCESS("Successfully bind socket!!!");
	SUCCESS("\tFile Descriptor ID: %d", socket_config->socket_fd);
	SUCCESS("\tIP Address: %s", ip_addr);
	SUCCESS("\tPort Address: %d", port_addr);

	switch (socket_type) {
	case SERVER_SIDE: {
		ret = tcp_socket_listening(socket_config, 10);
		if (ret < 0) {
			return -1;
		}
		break;
	}
	case CLIENT_SIDE: {
		ret = tcp_socket_connecting(socket_config);
		if (ret < 0) {
			return -1;
		}
		break;
	}
	default:
		printf("socket_type invalid\n");
		return -1;
	}

	return 0;
}

int tcp_socket_close(struct socket_config_t *socket_config,
		     enum socket_type_t socket_type)
{
	switch (socket_type) {
	case SERVER_SIDE: {
		close(socket_config->socket_fd);
		close(socket_config->_socket_fd);
		SUCCESS("socket with SERVER_SIDE role closed!!!!!");
		break;
	}
	case CLIENT_SIDE: {
		close(socket_config->socket_fd);
		SUCCESS("socket with CLIENT_SIDE role closed!!!!!");
		break;
	}
	default:
		printf("socket_type invalid\n");
		return -1;
	}

	return 0;
}