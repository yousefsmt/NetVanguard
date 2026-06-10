#ifndef NETVANGUARD_LIB_PARSER_H_
#define NETVANGUARD_LIB_PARSER_H_

#include "tcp_handler.h"

struct socket_address_t
{
    char     ip_addr[15];
    uint16_t port;
};

enum socket_type_t parse_args(struct socket_config_t* socket_config, struct socket_address_t* socket_address, int argc, char* argv[], char* buffer, size_t buffer_size );
void               debug_msg(const char* fmt, ...);
void               help(void *pname);


#endif /* NETVANGUARD_LIB_PARSER_H_ */