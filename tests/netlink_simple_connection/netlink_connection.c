#include <stdio.h>
#include <arpa/inet.h>
#include "netlink_handler.h"

int main(int argc, char *argv[])
{
    struct nl_sock *socket = NULL;
    struct nl_msg  *msg    = NULL;

    int family_id = 0;
	int err       = 0;

	void *hdr = NULL;

    uint32_t ip_to_block = 0 ;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS>\n", argv[0]);
        return -1;
    }

    ip_to_block = inet_addr(argv[1]);
    if (ip_to_block == INADDR_NONE) {
        fprintf(stderr, "Invalid IP address format.\n");
        return -1;
    }

    family_id = netlink_socket_init( &socket );

    err = netlink_socket_pack_msg( &socket, &msg, &hdr, /* file descriptor, message, headder*/
                                   family_id, FW_ATTR_SRC_IP, /* family id, attribute send message */
                                   FW_CMD_REJECT_IP, ip_to_block ); /* which command must be send, ip for this rule */
    if ( err < 0 )
    {
        fprintf(stderr, "During packing message error occur!!\n");
        return -1;
    }

    err = netlink_socket_send_msg( &socket, &msg );
    if ( err < 0 )
    {
        fprintf(stderr, "During sending message error occur!!\n");
        return -1;
    }

    err = netlink_socket_free( &socket, &msg );
    if ( err < 0 )
    {
        fprintf(stderr, "During deallocation resource error occur!!\n");
        return -1;
    }

    return 0;
}