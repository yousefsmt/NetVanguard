#include <arpa/inet.h>
#include <netlink/msg.h>
#include <netlink/cli/utils.h>

#include "netlink_handler.h"
#include "parser.h"

static int reply_to_str(uint32_t id)
{
	if ((id & REMOVE_BYTE) == REMOVE_BYTE) {
		uint8_t _id = GET_ID(id);
		SUCCESS("Kernel response to user : Hey user, I removed rule with id: %d\n",
			_id);
		return 0;
	}
	SUCCESS("Kernel response to user : Hey user, I added below rule:\nSIDE: %s\nHOOK: %s\nRULE: %s\n",
		(GET_SIDE(id) == INPUT) ? "INPUT" : "OUTPUT",
		(GET_HOOK_TYPE(id) == SOURCE) ? "SOURCE" : "DESTINATION",
		(GET_RULE_TYPE(id) == ACCEPT) ? "ACCEPT" :
		(GET_RULE_TYPE(id) == BLOCK)  ? "BLOCK" :
						"REJECT");
	return 0;
}

static int netlink_socket_reply_procces(struct nl_msg *msg, void *arg)
{
	(void)arg;

	struct genlmsghdr *genlhdr = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[FW_ATTR_MAX + 1];
	uint32_t id;
	int err;

	err = nla_parse(tb, FW_ATTR_MAX, genlmsg_attrdata(genlhdr, 0),
			genlmsg_attrlen(genlhdr, 0), NULL);

	if (err) {
		ERROR("unable to parse message: %s\n", nl_geterror(err));
		return NL_SKIP;
	}

	if (!tb[FW_ATTR_ACK]) {
		ERROR("msg attribute missing from message\n");
		return NL_SKIP;
	}

	id = nla_get_u32(tb[FW_ATTR_ACK]);
	err = reply_to_str(id);
	if (err < 0) {
		ERROR("failed to to map id to string\n");
		return NL_SKIP;
	}

	return NL_OK;
}

int netlink_socket_init(struct nl_sock **socket)
{
	int family_id;

	*socket = nl_cli_alloc_socket();

	(void)nl_cli_connect(*socket, NETLINK_GENERIC);

	family_id = genl_ctrl_resolve(*socket, FW_NETLINK_NAME);
	if (family_id < 0) {
		nl_close(*socket);
		nl_socket_free(*socket);
		ERROR("Socket cannot resolve with netvanguard kernel module, please check with \"lsmod | grep netvanguard\" ");
		nl_cli_fatal(family_id,
			     "Failed to resolve generic netlink family! [%s]",
			     nl_geterror(family_id));
	}

	return family_id;
}

int netlink_socket_init_cb(struct nl_sock **socket)
{
	int err;

	err = nl_socket_modify_cb(*socket, NL_CB_VALID, NL_CB_CUSTOM,
				  netlink_socket_reply_procces, NULL);
	if (err < 0) {
		nl_cli_fatal(err, "Unable to modify valid message callback");
	}

	return 0;
}

int netlink_socket_pack_msg(struct nl_sock **socket, struct nl_msg **msg,
			    void **hdr, struct van_str_rule_t *rules,
			    int family_id, int cmd)
{
	int err;

	if (*socket == NULL) {
		nl_cli_fatal(
			EINVAL,
			"You must call this message after socket_init! [%s]",
			nl_geterror(EINVAL));
	}

	*msg = nlmsg_alloc();
	if (*msg == NULL) {
		nl_close(*socket);
		nl_socket_free(*socket);
		nl_cli_fatal(ENOMEM, "Failed to allocate netlink message! [%s]",
			     nl_geterror(ENOMEM));
	}

	*hdr = genlmsg_put(*msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0,
			   cmd, FW_NETLINK_VERSION);
	if (*hdr == NULL) {
		nl_close(*socket);
		nl_socket_free(*socket);
		nl_cli_fatal(ENOMEM, "Failed to add header to message [%s]",
			     nl_geterror(ENOMEM));
	}

	err = nla_put_u32(*msg, FW_ATTR_IP, rules->ip);
	if (err < 0) {
		nl_close(*socket);
		nl_socket_free(*socket);
		nl_cli_fatal(err, "Failed to add ip to message [%s]",
			     nl_geterror(err));
	}

	err = nla_put_u16(*msg, FW_ATTR_PORT, rules->port);
	if (err < 0) {
		nl_close(*socket);
		nl_socket_free(*socket);
		nl_cli_fatal(err, "Failed to add port to message [%s]",
			     nl_geterror(err));
	}

	err = nla_put_u8(*msg, FW_ATTR_FLAG, rules->flags);
	if (err < 0) {
		nl_close(*socket);
		nl_socket_free(*socket);
		nl_cli_fatal(err, "Failed to add length to message [%s]",
			     nl_geterror(err));
	}

	return 0;
}

int netlink_socket_send_msg(struct nl_sock **socket, struct nl_msg **msg)
{
	if (*socket && *msg) {
		int err;
		err = nl_send_auto(*socket, *msg);
		if (err < 0) {
			nl_cli_fatal(err, "Failed to send message [%s]",
				     nl_geterror(err));
		}
	} else {
		nl_cli_fatal(ENOMEM,
			     "Message or socket structure is empty [%s]",
			     nl_geterror(ENOMEM));
	}

	return 0;
}

int netlink_socket_recv_msg(struct nl_sock **socket)
{
	int err;

	err = nl_recvmsgs_default(*socket);
	if (err < 0) {
		nl_cli_fatal(err, "%s", nl_geterror(err));
	}

	return 0;
}

int netlink_socket_free(struct nl_sock **socket, struct nl_msg **msg)
{
	if (*socket && *msg) {
		nlmsg_free(*msg);
		nl_close(*socket);
		nl_socket_free(*socket);
	} else {
		nl_cli_fatal(ENOMEM,
			     "Message or socket structure is empty [%s]",
			     nl_geterror(ENOMEM));
	}
	return 0;
}