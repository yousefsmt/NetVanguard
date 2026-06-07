AC_PREREQ([2.69])
AC_INIT([NetVanguard], [1.0], [bugs@example.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Basic checks
AC_PROG_CC
AC_PROG_MAKE_SET

# Add a --with-kernel option for cross-compiling or custom kernels
AC_ARG_WITH([kernel],
  [AS_HELP_STRING([--with-kernel=DIR], [Path to the kernel build directory])],
  [KDIR="$withval"],
  [KDIR="/lib/modules/$(uname -r)/build"]
)

# Verify the kernel directory actually exists
AC_CHECK_FILE([$KDIR/Makefile],
  [AC_MSG_NOTICE([Using kernel build directory: $KDIR])],
  [AC_MSG_ERROR([Kernel build directory $KDIR not found. Install linux-headers.])]
)

# Export variables to Makefile.am
AC_SUBST([KDIR])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT