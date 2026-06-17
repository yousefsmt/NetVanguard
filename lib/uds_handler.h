#ifndef NETVANGUARD_LIB_UDS_HANDLER_H_
#define NETVANGUARD_LIB_UDS_HANDLER_H_

#include <sys/socket.h>
#include <sys/un.h>

struct uds_config_t {
	char addr_str[20];
	struct sockaddr_un addr;
	socklen_t len;
	int sock;
};

int uds_socket_init(struct uds_config_t *config, const char *addr,
		    size_t addr_len);
ssize_t uds_socket_send(struct uds_config_t *config, const char *msg,
			size_t msg_len, const char *addr, size_t addr_len);
ssize_t uds_socket_recv(struct uds_config_t *config, char *msg, size_t msg_len);
int uds_socket_close(struct uds_config_t *config);

#endif /* NETVANGUARD_LIB_UDS_HANDLER_H_ */