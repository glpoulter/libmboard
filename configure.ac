# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_REVISION($Revision$)

AC_INIT(libmboard, pre-0.1.5, L.S.Chin@_NOSPAM_rl.ac.uk)
AM_INIT_AUTOMAKE
#AM_MAINTAINER_MODE

AC_CONFIG_SRCDIR([src/serial/env_init.c])
AC_CONFIG_HEADER([include/mb_config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

AC_C_INLINE

# Default CFLAGS
# Need extra checks to set this depending on compiler
case "$CC" in
	gcc*)
		CFLAGS="${CFLAGS} -std=c99 -Wall -pedantic -fno-common"
		;;
	pgcc*)
		CFLAGS="${CFLAGS} -c9x -Minform=warn"
		;;
	icc*)
		CFLAGS="${CFLAGS} -std=c99 -Wall"
		;;
	xlc*)
		CFLAGS="${CFLAGS} -qlanglvl=stdc99 -qinfo=all -qflag=w:u -qcheck=all"
		;;
	*)
		CFLAGS="${CFLAGS}"
		;;
esac

PRODUCTION_CFLAGS="-DNDEBUG -O3"
DEBUG_CFLAGS="-g -D_EXTRA_CHECKS"

AC_SUBST(PRODUCTION_CFLAGS)
AC_SUBST(DEBUG_CFLAGS)

# Checks for libraries.
AC_CHECK_LIB([m], [sqrt])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stddef.h unistd.h],[],
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
#AC_FUNC_MALLOC
#AC_CHECK_FUNCS([memset],[],[AC_MSG_ERROR(memset() is required)])
#AC_CHECK_FUNCS([memcpy],[],[AC_MSG_ERROR(memcpy() is required)])
#AC_CHECK_FUNCS([sqrt],[],[AC_MSG_ERROR(sqrt() is required)])
#AC_CHECK_FUNCS([floor],[],[AC_MSG_ERROR(floor() is required)])


# Allow users to enable instrumentation of memory usage
AC_ARG_ENABLE([memlog], 
              [AC_HELP_STRING([--enable-memlog],
                              [Enable instrumentation of memory usage])],
              [want_memlog=yes],
              [want_memlog=no]
             )
if test "x${want_memlog}" = xyes;
then
	PRODUCTION_CFLAGS="-D_LOG_MEMORY_USAGE ${PRODUCTION_CFLAGS}"
	DEBUG_CFLAGS="-D_LOG_MEMORY_USAGE ${DEBUG_CFLAGS}"
fi

# files to create

AC_CONFIG_FILES([Makefile])

AC_OUTPUT([
README
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
tests/testsuite_tag_table/Makefile
tests/testsuite_pooled_list/Makefile
tests/testsuite_syncqueue/Makefile
])dnl

echo ""
echo "------------------- Configuration successful --------------------------"
echo ""
echo "Compile parallel libs : $want_parallel"
if test x${want_parallel} = xyes; then
echo " - CC used by MPI Compiler Wrapper ($mpi_cc_wrapper) = $mpi_cc"
echo " - MPI CFLAGS       = $MPICFLAGS"
echo " - MPI LDFLAGS      = $MPILDFLAGS"
echo " - MPI LIBS         = $MPILIBS"
echo ""
echo " - PTHREADS CFLAGS  = $THREADS_CFLAGS"
echo " - PTHREADS LDFLAGS = $THREADS_LDFLAGS"
echo " - PTHREADS LIBS    = $THREADS_LIBS"
fi
echo ""
echo "Prepare unit tests    : $want_tests"
if test x${want_tests} = xyes; then
echo " - CUNIT CFLAGS     = $CUNITCFLAGS"
echo " - CUNIT LDFLAGS    = $CUNITLDFLAGS"
echo " - CUNIT LIBS       = $CUNITLIBS"
fi
echo ""
echo "Default compiler : $CC"
echo "Default CFLAGS   : $CFLAGS"
echo "Default LDFLAGS  : $LDFLAGS"
echo "Default LIBS     : $LIBS"
echo ""
echo "Additional CFLAGS (production) : $PRODUCTION_CFLAGS"
echo "Additional CFLAGS (debug)      : $DEBUG_CFLAGS"
echo ""
echo "-----------------------------------------------------------------------"
echo ""
echo "If you wish to use a different compiler and/or CFLAGS, you can "
echo "override CC and CFLAGS during the invocation of 'make'. For example:"
echo '# make CC="xlc_r" CFLAGS="-qlanglvl=stdc99 -qinfo=all -qflag=w:u -qcheck=all "'
echo ""

