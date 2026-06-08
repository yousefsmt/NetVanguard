AC_PREREQ([2.69])
AC_INIT([NetVanguard], [1.0], [yoosefsamet@gmail.com])
AC_CONFIG_SRCDIR([lib/tcp_handler.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_PROG_CC
AM_PROG_AR

LT_INIT
AC_PROG_MAKE_SET

AC_ARG_WITH([kernel],
  [AS_HELP_STRING([--with-kernel=DIR], [Path to the kernel build directory])],
  [KDIR="$withval"],
  [KDIR="/lib/modules/$(uname -r)/build"]
)

AC_CHECK_FILE([$KDIR/Makefile],
  [AC_MSG_NOTICE([Using kernel build directory: $KDIR])],
  [AC_MSG_ERROR([Kernel build directory $KDIR not found. Install linux-headers.])]
)
AC_SUBST([KDIR])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT