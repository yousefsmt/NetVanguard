ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/config
AM_CFLAGS = -Wall -Werror -Wextra

lib_LTLIBRARIES = libClp.la
libClp_la_SOURCES = \
					$(top_srcdir)/lib/tcp_handler.c \
					$(top_srcdir)/lib/parser.c \
					$(top_srcdir)/lib/netlink_handler.c \
					$(top_srcdir)/lib/uds_handler.c \
					$(top_srcdir)/lib/db.c
libClp_la_CFLAGS = $(AM_CFLAGS) $(LIBNL3_CFLAGS) $(SQLITE3_CFLAGS)
libClp_la_LIBADD = $(LIBNL3_LIBS) $(SQLITE3_LIBS)

pkginclude_HEADERS = \
					$(top_srcdir)/lib/tcp_handler.h \
					$(top_srcdir)/lib/parser.h \
					$(top_srcdir)/lib/netlink_handler.h \
					$(top_srcdir)/lib/uds_handler.h \
					$(top_srcdir)/lib/db.h

sbin_PROGRAMS = netvanguardd
bin_PROGRAMS = netvanguard-cli

netvanguardd_SOURCES = src/user_daemon/user.c
netvanguardd_LDADD = libClp.la $(LIBNL3_LIBS) $(SQLITE3_LIBS)
netvanguardd_CFLAGS = $(AM_CFLAGS) $(LIBNL3_CFLAGS)

netvanguard_cli_SOURCES = src/user_cli/cli.c
netvanguard_cli_LDADD = libClp.la $(LIBNL3_LIBS)
netvanguard_cli_CFLAGS = $(AM_CFLAGS) $(LIBNL3_CFLAGS) $(SQLITE3_CFLAGS)

check_PROGRAMS = \
				tests/netlink_simple_connection/netlink_test \
				tests/tcp_simple_connection/tcp_server \
				tests/tcp_simple_connection/tcp_client \
				tests/uds_simple_connection/uds_server \
				tests/uds_simple_connection/uds_client

tests_tcp_simple_connection_tcp_server_SOURCES = tests/tcp_simple_connection/tcp_server.c
tests_tcp_simple_connection_tcp_server_LDADD = libClp.la $(LIBNL3_LIBS)
tests_tcp_simple_connection_tcp_server_CFLAGS = $(AM_CFLAGS) $(LIBNL3_CFLAGS)

tests_tcp_simple_connection_tcp_client_SOURCES = tests/tcp_simple_connection/tcp_client.c
tests_tcp_simple_connection_tcp_client_LDADD = libClp.la $(LIBNL3_LIBS)
tests_tcp_simple_connection_tcp_client_CFLAGS = $(AM_CFLAGS) $(LIBNL3_CFLAGS)

tests_netlink_simple_connection_netlink_test_SOURCES = tests/netlink_simple_connection/netlink_connection.c
tests_netlink_simple_connection_netlink_test_LDADD = libClp.la $(LIBNL3_LIBS)
tests_netlink_simple_connection_netlink_test_CFLAGS = $(AM_CFLAGS) $(LIBNL3_CFLAGS)

tests_uds_simple_connection_uds_server_SOURCES = tests/uds_simple_connection/uds_server.c
tests_uds_simple_connection_uds_server_LDADD = libClp.la $(LIBNL3_LIBS)
tests_uds_simple_connection_uds_server_CFLAGS = $(AM_CFLAGS) $(LIBNL3_CFLAGS)

tests_uds_simple_connection_uds_client_SOURCES = tests/uds_simple_connection/uds_client.c
tests_uds_simple_connection_uds_client_LDADD = libClp.la $(LIBNL3_LIBS)
tests_uds_simple_connection_uds_client_CFLAGS = $(AM_CFLAGS) $(LIBNL3_CFLAGS)

EXTRA_DIST = \
	VERSION \
	README.md \
	debian \
	git-version-gen \
	Kbuild \
	src/kernel_module/kernel.c


all-local:
	@mkdir -p $(abs_builddir)/src/kernel_module
	@cp -p $(abs_srcdir)/Kbuild $(abs_builddir)/Kbuild
	@cp -p $(abs_srcdir)/src/kernel_module/kernel.c $(abs_builddir)/src/kernel_module/kernel.c
	$(MAKE) -C $(KDIR) M=$(abs_builddir) CFLAGS_MODULE="-I$(abs_srcdir)/config" modules

install-exec-local:
	$(MAKE) -C $(KDIR) M=$(abs_builddir) modules_install

clean-local:
	$(MAKE) -C $(KDIR) M=$(abs_builddir) clean