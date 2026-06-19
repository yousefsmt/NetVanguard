AC_PREREQ([2.69])
AC_INIT([NetVanguard], [1.0], [yoosefsamet@gmail.com])

AC_CONFIG_SRCDIR([lib/tcp_handler.c])
AC_CONFIG_SRCDIR([lib/netlink_handler.c])
AC_CONFIG_SRCDIR([lib/parser.c])
AC_CONFIG_SRCDIR([lib/uds_handler.c])
AC_CONFIG_SRCDIR([lib/db.c])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
PKG_CHECK_MODULES(LIBNL3, [libnl-3.0 >= 3.2.0 libnl-route-3.0 >= 3.2.0 libnl-genl-3.0 >= 3.2.0 libnl-nf-3.0 >= 3.2.0 libnl-cli-3.0 >= 3.2.0])
PKG_CHECK_MODULES(SQLITE3, [sqlite3 >= 3.5.0 ])

AC_PROG_CC
AM_PROG_AR

LT_INIT
AC_PROG_MAKE_SET

AC_ARG_WITH([kernel_module],
  [AS_HELP_STRING([--with-kernel=DIR], [Path to the kernel build directory])],
  [KDIR="$withval"],
  [KDIR="/lib/modules/$(uname -r)/build"]
)

AC_CHECK_FILE([$KDIR/Makefile],
  [AC_MSG_NOTICE([Using kernel build directory: $KDIR])],
  [AC_MSG_ERROR([Kernel build directory $KDIR not found. Install linux-headers.])]
)
AC_SUBST([KDIR])
AC_SUBST([LIBNL_CFLAGS])
AC_SUBST([LIBNL_LIBS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT