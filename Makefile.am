ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/include

lib_LTLIBRARIES = libClp.la
libClp_la_SOURCES = \
					$(top_srcdir)/lib/tcp_handler.c \
					$(top_srcdir)/lib/parser.c

pkginclude_HEADERS = \
					$(top_srcdir)/lib/tcp_handler.h \
					$(top_srcdir)/lib/parser.h

sbin_PROGRAMS = netvanguardd
bin_PROGRAMS = netvanguard-cli

netvanguardd_SOURCES = src/user/user.c
netvanguardd_LDADD = libClp.la

netvanguard_cli_SOURCES = src/cli/cli.c
netvanguard_cli_LDADD = libClp.la

check_PROGRAMS = tests/tcp_simple_connection/tcp_test

tests_tcp_simple_connection_tcp_test_SOURCES = tests/tcp_simple_connection/tcp_connection.c
tests_tcp_simple_connection_tcp_test_LDADD = libClp.la
tests_tcp_simple_connection_tcp_test_CFLAGS = $(AM_CFLAGS) -Wall -Werror -Wextra

EXTRA_DIST = \
	VERSION \
	README.md \
	debian \
	git-version-gen \
	Kbuild \
	src/kernel/kernel.c


all-local:
	@mkdir -p $(abs_builddir)/src/kernel
	@cp -p $(abs_srcdir)/Kbuild $(abs_builddir)/Kbuild
	@cp -p $(abs_srcdir)/src/kernel/kernel.c $(abs_builddir)/src/kernel/kernel.c
	$(MAKE) -C $(KDIR) M=$(abs_builddir) modules

install-exec-local:
	$(MAKE) -C $(KDIR) M=$(abs_builddir) modules_install

clean-local:
	$(MAKE) -C $(KDIR) M=$(abs_builddir) clean