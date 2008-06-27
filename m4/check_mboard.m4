# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

AC_DEFUN([MBOARD_CHECK_LIBAVAILABLE], [

# C compiler is required
AC_REQUIRE([AC_PROG_CC])

# parallel support checks is required
AC_REQUIRE([MBOARD_CHECK_PARALLEL])

# vars to be exported at end of macro
MBOARD_S_CFLAGS=""
MBOARD_P_CFLAGS="${MPICFLAGS} ${THREADS_CFLAGS}"
MBOARD_SD_CFLAGS=""
MBOARD_PD_CFLAGS="${MPICFLAGS} ${THREADS_CFLAGS}"

MBOARD_S_LIBS="-lmboard_s"
MBOARD_P_LIBS="-lmboard_p ${MPILIBS} ${THREADS_LIBS}"
MBOARD_SD_LIBS="-lmboard_sd"
MBOARD_PD_LIBS="-lmboard_pd ${MPILIBS} ${THREADS_LIBS}"

MBOARD_S_LDFLAGS=""
MBOARD_P_LDFLAGS="${MPILDFLAGS} ${THREADS_LDFLAGS}"
MBOARD_SD_LDFLAGS=""
MBOARD_PD_LDFLAGS="${MPILDFLAGS} ${THREADS_LDFLAGS}"


# Allow users to manually specify path to mboard installation
AC_ARG_WITH([mboard],
            [AC_HELP_STRING([--with-mboard=MBOARDDIR],
                            [Specify path to Libmboard installation])]
           )
MBOARDDIR=${with_mboard}


## Save env vars
original_LIBS="${LIBS}"
original_CFLAGS="${CFLAGS}"
original_LDFLAGS="${LDFLAGS}"

# if MBOARD path specified, redefine cflags
if test ! "x${MBOARDDIR}" = x;
then
	CFLAGS="-I${MBOARDDIR}/include"
	LDFLAGS="-L${MBOARDDIR}/lib"
	
	MBOARD_S_CFLAGS="${CFLAGS} ${MBOARD_S_CFLAGS}"
	MBOARD_P_CFLAGS="${CFLAGS} ${MBOARD_P_CFLAGS}"
	MBOARD_SD_CFLAGS="${CFLAGS} ${MBOARD_SD_CFLAGS}"
	MBOARD_PD_CFLAGS="${CFLAGS} ${MBOARD_PD_CFLAGS}"
	
	MBOARD_S_LDFLAGS="${LDFLAGS} ${MBOARD_S_LDFLAGS}"
	MBOARD_P_LDFLAGS="${LDFLAGS} ${MBOARD_P_LDFLAGS}"
	MBOARD_SD_LDFLAGS="${LDFLAGS} ${MBOARD_SD_LDFLAGS}"
	MBOARD_PD_LDFLAGS="${LDFLAGS} ${MBOARD_PD_LDFLAGS}"
fi
	
# Check that libmboard is installed
save_LIBS="${LIBS}"
AC_CHECK_LIB([mboard_s], [MB_Env_Init], [], [AC_MSG_ERROR([
** libmboard_s could not be found. If you have libmboard installed, try 
   specifying the installation path using --with-mboard=MBOARDDIR.])])
LIBS="${save_LIBS}"

# check for presence of mboard.h
AC_TRY_COMPILE([#include <mboard.h>],[],[],
               [AC_MSG_ERROR([Could not locate MBoard header file (mboard.h)])])
               
               
# check for parallel libmboard
if test ! "x${want_parallel}" = xno;
then

CFLAGS="${MBOARD_P_CFLAGS}"
LDFLAGS="${MBOARD_P_LDFLAGS}"
LIBS="${MBOARD_P_LIBS}"

AC_CHECK_LIB([mboard_p], [MB_SyncTest], [], [AC_MSG_ERROR([
** Parallel libmboard (libmboard_p) could not be found. That is required
   to build the parallel version of our binaries.
** If you do not need the parallel version, reconfigure with the 
   --disable-parallel option to build only the serial version.])])

   
fi


# restore env vars
LDFLAGS="${original_LDFLAGS}"
CFLAGS="${original_CFLAGS}"
LIBS="${original_LIBS}"

AC_SUBST(MBOARD_S_LIBS)
AC_SUBST(MBOARD_P_LIBS)
AC_SUBST(MBOARD_SD_LIBS)
AC_SUBST(MBOARD_PD_LIBS)

AC_SUBST(MBOARD_S_LDFLAGS)
AC_SUBST(MBOARD_P_LDFLAGS)
AC_SUBST(MBOARD_SD_LDFLAGS)
AC_SUBST(MBOARD_PD_LDFLAGS)

AC_SUBST(MBOARD_S_CFLAGS)
AC_SUBST(MBOARD_P_CFLAGS)
AC_SUBST(MBOARD_SD_CFLAGS)
AC_SUBST(MBOARD_PD_CFLAGS)

])dnl
