AC_PREREQ(2.61)
AC_INIT([mustashe_c], [1.0.0], [x86mail@gmail.com])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build])

AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_LIBTOOL
AC_HEADER_STDC
AC_FUNC_MALLOC

AC_CHECK_LIB(frozen, frozen_init, [FROZEN=1], [FROZEN=0])
AM_CONDITIONAL([HAVE_FROZEN], [test "$FROZEN" = 1])
modulesdir=${libexecdir}/frozen/
AC_SUBST(modulesdir)

AC_PATH_PROGS(BISON, [bison], no)
AC_PATH_PROGS(FLEX, [flex], no)

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
		src/frozen/Makefile
		test/Makefile
])
AC_OUTPUT