#include <unistd.h>
#include <stdio.h>

#include "uds_handler.h"

int uds_socket_init(struct uds_config_t *config, const char *addr,
		    size_t addr_len)
{
	int err;

	config->sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (config->sock < 0) {
		perror("socket()");
		return -1;
	}

	config->addr.sun_family = AF_UNIX;
	strncpy(config->addr.sun_path, addr, addr_len);
	strncpy(config->addr_str, addr, addr_len);
	config->len = sizeof(struct sockaddr_un);
	unlink(addr);

	err = bind(config->sock, (const struct sockaddr *)&config->addr,
		   config->len);
	if (err < 0) {
		perror("bind()");
		close(config->sock);
		unlink(addr);
		return -1;
	}

	return 0;
}

int uds_socket_recv(struct uds_config_t *config, char *msg, size_t msg_len)
{
	struct sockaddr_un cli_addr;
	socklen_t cli_len = 0;
	ssize_t recv_bytes;

	if (config) {
		memset(&cli_addr, 0, sizeof(struct sockaddr_un));
		memset(msg, 0, msg_len);

		recv_bytes = recvfrom(config->sock, msg, msg_len, 0,
				      (struct sockaddr *)&cli_addr, &cli_len);
		if (recv_bytes < 0) {
			perror("recvfrom()");
			close(config->sock);
			unlink(config->addr_str);
			return -1;
		}

	} else {
		return -1;
	}

	return 0;
}

int uds_socket_send(struct uds_config_t *config, const char *msg,
		    size_t msg_len, const char *addr, size_t addr_len)
{
	struct sockaddr_un cli_addr;
	socklen_t cli_len = 0;
	ssize_t send_bytes;

	cli_addr.sun_family = AF_UNIX;
	strncpy(cli_addr.sun_path, addr, addr_len);
	cli_len = sizeof(struct sockaddr_un);

	send_bytes = sendto(config->sock, msg, msg_len, 0,
			    (const struct sockaddr *)&cli_addr, cli_len);
	if (send_bytes < 0) {
		perror("sendto()");
		close(config->sock);
		unlink(config->addr_str);
		return -1;
	}
	return 0;
}

int uds_socket_close(struct uds_config_t *config)
{
	if (config) {
		close(config->sock);
		unlink(config->addr_str);
	} else {
		return -1;
	}

	return 0;
}
