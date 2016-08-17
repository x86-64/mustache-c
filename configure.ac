AC_PREREQ(2.61)
AC_INIT([mustache_c], [1.0.0], [x86mail@gmail.com])

AC_CONFIG_FILES([mustache_c-1.0.pc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build])

AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_LIBTOOL
AC_HEADER_STDC
AC_FUNC_MALLOC

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

AC_PATH_PROGS(BISON, [bison], no)
AC_PATH_PROGS(FLEX, [flex], no)

AC_CHECK_HEADER([stdint.h], [],
[AC_MSG_ERROR([Couldn't find or include stdint.h])],
[])

AC_ARG_ENABLE(debug,  AS_HELP_STRING([--enable-debug],  [compile with debug functions]))
if test "x$enable_debug" == "xyes"; then
        AC_DEFINE([DEBUG], [1], [Defined if debug functions requested])
fi

AC_ARG_ENABLE(helpers, AS_HELP_STRING([--disable-helpers],  [do not include helper functions ]))
if test "x$enable_helpers" != "xyes"; then
        AC_DEFINE([HELPERS], [1], [Define if helpers routines enabled])
fi

AC_CONFIG_FILES([Makefile
        src/Makefile
        test/Makefile
        examples/Makefile
])
AC_OUTPUT
