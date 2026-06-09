#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/cli/utils.h>
#include "netlink_handler.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    struct nl_sock *socket = NULL;
    struct nl_msg *msg     = NULL;
    int family_id = 0;
	int err = 0;
	// void *hdr = NULL;
    uint32_t ip_to_block;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS>\n", argv[0]);
        return -1;
    }

    // Convert string IP to 32-bit integer (Network Byte Order)
    ip_to_block = inet_addr(argv[1]);
    if (ip_to_block == INADDR_NONE) {
        fprintf(stderr, "Invalid IP address format.\n");
        return -1;
    }

    // 1. Allocate and connect the Netlink socket
    socket = nl_socket_alloc();
    genl_connect(socket);

    // 2. Resolve the family ID using the name we defined in the kernel
    family_id = genl_ctrl_resolve(socket, FW_NETLINK_NAME);
    if (family_id < 0) {
        fprintf(stderr, "Failed to resolve Netlink family! Is the kernel module loaded?\n");
        nl_socket_free(socket);
        return -1;
    }
	/*-------------------------------------------------------------*/

	// err = netlink_socket_pack_msg( msg, hdr, family_id, FW_ATTR_SRC_IP, ip_to_block );
	// if (err < 0)
	// {
    //     fprintf(stderr, "Failed to send message: %d\n", err);
	// 	return -1;
    // }
    // 3. Allocate a new Netlink message
    msg = nlmsg_alloc();

    // 4. Setup the Generic Netlink Header (Command: BLOCK_IP)
    genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0, FW_CMD_BLOCK_IP, FW_NETLINK_VERSION);

    // 5. PACK THE TLV: Type = FW_ATTR_SRC_IP, Length/Value = ip_to_block
    nla_put_u32(msg, FW_ATTR_SRC_IP, ip_to_block);

    // 6. Send the message to the kernel
    err = nl_send_auto(socket, msg);
    if (err < 0) {
        fprintf(stderr, "Failed to send message: %d\n", err);
    } else {
        printf("Successfully sent rule to block %s to the kernel.\n", argv[1]);
    }

    // 7. Cleanup
    nlmsg_free(msg);
    nl_socket_free(socket);

    return 0;
}