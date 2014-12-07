AC_PREREQ(2.63)
AC_INIT([ircd], [0.2.0], [bugs@niftylogic.com])

AC_CONFIG_SRCDIR([src/irc.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_HEADERS(config.h)

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AX_TEST_COVERAGE

AC_PROG_CC
AC_PROG_INSTALL
AC_C_INLINE

AC_CHECK_LIB(ctap, main,,
	AC_MSG_ERROR(Missing ctap testing library - see http://github.com/filefrog/ctap))
AC_CHECK_LIB(vigor, string,,
	AC_MSG_ERROR(Missing Vigor - see http://github.com/filefrog/libvigor))
AC_CHECK_LIB(pthread, pthread_create,,
	AC_MSG_ERROR(Missing POSIX threads library))
AC_CHECK_LIB(zmq, zmq_ctx_new,,
	AC_MSG_ERROR(Missing 0MQ library))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
