#include "netlink_handler.h"

int main()
{
	struct nl_sock *sock = NULL;
	struct nl_msg  *msg  = NULL;
	void           *hdr  = NULL;
	int             err  = 0;

	err = netlink_socket_init( sock );
	if( err < 0 )
	{
		nl_cli_fatal( err, "netlink init" );
	}

	err = netlink_socket_init_msg( msg );
	if( err < 0 )
	{
		nl_cli_fatal( err, "netlink init msg" );
	}

	err = netlink_socket_add_header( msg, hdr );
	if( err < 0 )
	{
		nl_cli_fatal( err, "netlink add header" );
	}

	err =  netlink_socket_add_data( msg, FW_ATTR_SRC_IP, "8.8.8.8" );
	if( err < 0 )
	{
		nl_cli_fatal( err, "netlink add data" );
	}

	err = netlink_socket_send_msg( sock, msg );
	if( err < 0 )
	{
		nl_cli_fatal( err, "netlink send msg" );
	}

	err = netlink_socket_free( sock, msg );
	if( err < 0 )
	{
		nl_cli_fatal( err, "netlink free" );
	}

	return 0;
}