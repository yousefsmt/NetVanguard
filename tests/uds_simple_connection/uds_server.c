#include "uds_handler.h"
#include "parser.h"

#define SERVER_ADDR "/tmp/server"
#define CLIENT_ADDR "/tmp/client"

#define ADDR_LEN (sizeof(SERVER_ADDR))

#define BUFFER_SIZE (1024)

int main() {

    struct uds_config_t uds_config;
    char recv_msg[BUFFER_SIZE];
    int err;

    err = uds_socket_init(&uds_config, SERVER_ADDR, ADDR_LEN);
    if(err < 0) {
        ERROR("init uds failed!");
        return -1;
    }
    SUCCESS("uds server socket create with %s address", SERVER_ADDR);

    ssize_t recv_bytes = uds_socket_recv(&uds_config, recv_msg, BUFFER_SIZE);
    if(recv_bytes < 0)
    {
        ERROR("uds recv failed!");
        return -1;
    }
    SUCCESS("uds server recv %ld bytes succesfull [%s] message", recv_bytes, recv_msg);

    strncpy(recv_msg, "hello client", 13);
    ssize_t send_bytes = uds_socket_send(&uds_config, recv_msg, 13, CLIENT_ADDR, ADDR_LEN);
    if(send_bytes < 0) {
        ERROR("uds send error!");
        return -1;
    }
    SUCCESS("uds server send %ld bytes successfull [%s] message", send_bytes, recv_msg);

    err = uds_socket_close(&uds_config);
    if(err < 0) {
        ERROR("uds close error!");
        return -1;
    }
    SUCCESS("uds server close successfull");

    return 0;
}