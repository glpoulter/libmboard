# Process this file with autoconf to produce a configure script.
# $Id: configure.ac 989 2008-06-27 10:33:36Z lsc $
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

AC_PREREQ(2.59)
AC_REVISION($Revision$)

AC_INIT(circles_example, 0.1.2, L.S.Chin@rl.ac.uk)
AM_INIT_AUTOMAKE

# default CFLAGS
NORMAL_CFLAGS="-Wall -pedantic -O3 -g"

PARALLEL_EXTRA_CFLAGS=" -std=c9x -D_PARALLEL"

AC_SUBST(PARALLEL_EXTRA_CFLAGS)

AC_CONFIG_SRCDIR([debug.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# Allow users to manually specify path to MPI installation
AC_ARG_WITH([expat],
            [AC_HELP_STRING([--with-expat=EXPATDIR],
                            [Specify path to libexpat installation])]
           )
if test ! x${with_expat} = x; then
    CFLAGS="${CFLAGS} -I${with_expat}/include"
    LDFLAGS="${LDFLAGS} -L${with_expat}/lib"
fi

### Checks for libraries.

# these should automatically append to LDFLAGS
AC_SEARCH_LIBS([sqrt], [m], [], [AC_MSG_ERROR([sqrt() from libm is required])])
AC_SEARCH_LIBS([XML_ParserCreate], [expat], [], [AC_MSG_ERROR([
** libexpat is required. Please install that library and try again.
** If you do have libexpat installed, try specifying the installation path 
   using --with-expat=EXPAT_DIR.])])

# Check requirements for parallel build
MBOARD_CHECK_PARALLEL

# Check for libmboard
MBOARD_CHECK_LIBAVAILABLE

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADER([expat.h],[],[AC_MSG_ERROR([expat.h is required and cannot be found])])
AC_CHECK_HEADERS([float.h stddef.h stdlib.h string.h],[],
                 [AC_MSG_ERROR([This header file is required and cannot be found])])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC


#AC_CONFIG_FILES([Makefile])
AC_OUTPUT([Makefile])
