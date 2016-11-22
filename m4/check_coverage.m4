# $Id: check_coverage.m4 2925 2012-07-20 14:12:17Z lsc $
# 
# Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Feb 2009

AC_DEFUN([MBOARD_CHECK_COVERAGE], [

GCOV_REQUIRED_FLAGS="-fprofile-arcs -ftest-coverage"
GCOV_EXTRA_CFLAGS="-D_COVERAGE_ANALYSIS"

# C compiler is required
AC_REQUIRE([AC_PROG_CC])

# coverage analysis disabled by default
want_coverage=no

# Allow users to enable coverage analysis using gcov
AC_ARG_ENABLE([coverage],
    [AC_HELP_STRING([--enable-coverage],
                    [Instrument binaries to enable coverage analysis])],
    [want_coverage=yes],
    [want_coverage=no])

# if coverage analysis requested
if test ! "x${want_coverage}" = xno;
then

	AC_MSG_CHECKING([requirements for performing coverage analysis])	
	
	#### compilation of test code must be enabled
	
	if test ! x"${WANT_TESTS}" = "x1"
	then
		AC_MSG_RESULT(no)
		AC_MSG_ERROR([
 *** Unit tests must be compiler in order for coverage analysis to work.
 *** Remove the --disable-tests option in your configuration and try again.
 ])
	fi
	
	### gcc must be used
	# for simplicity, limit --enable-coverage to only GCC 
	${CC} --version 2> /dev/null | head -n 1 | grep "GCC" -q -i
	if test ! $? = 0
	then
		AC_MSG_RESULT(no)
		AC_MSG_ERROR([
*** Coverage analysis only available for GNU C Compilers (gcc, g++, ...)
*** Select a suitable compiler and try again. Example:
     $ ./configure CC=gcc --enable-coverage
])
	fi
	AC_MSG_RESULT(yes)
	
	# Append flags
	CFLAGS="${CFLAGS} ${GCOV_REQUIRED_FLAGS} ${GCOV_EXTRA_CFLAGS}"
	LDFLAGS="${LDFLAGS} ${GCOV_REQUIRED_FLAGS}"

    AM_CONDITIONAL([COVERAGE_CONFIGURED], [true])
else
	AM_CONDITIONAL([COVERAGE_CONFIGURED], [false])
fi

])dnl MBOARD_CHECK_COVERAGE

