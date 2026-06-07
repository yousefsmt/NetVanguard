KDIR = @KDIR@
EXTRA_DIST = src/kernel/kernel.c Kbuild
TEST_DIR = tests

check_PROGRAMS = $(TEST_DIR)/tcp_simple_connection/tcp_test

__TEST_DIR__tcp_simple_connection_tcp_test_SOURCES = \
	$(TEST_DIR)/tcp_simple_connection/main.c \
	$(TEST_DIR)/tcp_simple_connection/src/tcp_handler.c \
	$(TEST_DIR)/tcp_simple_connection/include/tcp_handler.h

__TEST_DIR__tcp_simple_connection_tcp_test_CFLAGS = -I$(srcdir)/$(TEST_DIR)/tcp_simple_connection/include -Wall -Werror -Wextra

all-local:
	@mkdir -p $(abs_builddir)/src/kernel
	@cp -p $(abs_srcdir)/Kbuild $(abs_builddir)/Kbuild
	@cp -p $(abs_srcdir)/src/kernel/kernel.c $(abs_builddir)/src/kernel/kernel.c
	$(MAKE) -C $(KDIR) M=$(abs_builddir) modules

install-exec-local:
	$(MAKE) -C $(KDIR) M=$(abs_builddir) modules_install

clean-local:
	$(MAKE) -C $(KDIR) M=$(abs_builddir) clean