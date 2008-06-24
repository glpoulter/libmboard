# Process this file with autoconf to produce a configure script.


AC_PREREQ(2.59)
AC_REVISION($Revision$)

AC_INIT(libmboard, 0.1.1-alpha, L.S.Chin@rl.ac.uk)
AM_INIT_AUTOMAKE
#AM_MAINTAINER_MODE

NORMAL_CFLAGS=" -Wall -pedantic -O3 -DNDEBUG -fno-common"
DEBUG_CFLAGS=" -g -Wall -pedantic -fno-common -D_EXTRA_CHECKS"
PARALLEL_EXTRA_CFLAGS=" -std=c9x"
COMMON_LDFLAGS="-lm"

AC_SUBST(PARALLEL_EXTRA_CFLAGS)
AC_SUBST(NORMAL_CFLAGS)
AC_SUBST(DEBUG_CFLAGS)
AC_SUBST(COMMON_LDFLAGS)

AC_CONFIG_SRCDIR([src/serial/env_init.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([m], [sqrt], [],[AC_MSG_ERROR(sqrt() from libm is required)])
AC_CHECK_LIB([m], [floor],[],[AC_MSG_ERROR(floor() from libm is required)])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h stddef.h stdlib.h string.h unistd.h],[],
				 [AC_MSG_ERROR(That header file is required)])

# Check requirements for parallel implementation
MBOARD_CHECK_PARALLEL

# Check requirements for testing
MBOARD_CHECK_TESTING

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset],[],[AC_MSG_ERROR(memset() is required)])
AC_CHECK_FUNCS([memcpy],[],[AC_MSG_ERROR(memcpy() is required)])

# files to create

AC_CONFIG_FILES([Makefile])

AC_OUTPUT([
Doxyfile_user
Doxyfile_developer_serial
Doxyfile_developer_parallel
src/utils/Makefile
src/serial/Makefile
src/parallel/Makefile
tests/Makefile
tests/testsuite_commqueue/Makefile
tests/testsuite_commroutines/Makefile
tests/testsuite_mb_parallel/Makefile
tests/testsuite_mb_serial/Makefile
tests/testsuite_objmap/Makefile
tests/testsuite_pooled_list/Makefile
tests/testsuite_syncqueue/Makefile
])dnl
