# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

AC_DEFUN([MBOARD_CHECK_PARALLEL], [

# C compiler is required
AC_REQUIRE([AC_PROG_CC])

# Parallel build enabled by default
want_parallel=yes

# If users want to skip parallel build
AC_ARG_ENABLE([parallel],
    [AC_HELP_STRING([--disable-parallel],
                    [Disable building of parallel limboard])],
    [want_parallel=no],
    [want_parallel=yes])

# Allow users to manually specify path to MPI installation
AC_ARG_WITH([mpi],
            [AC_HELP_STRING([--with-mpi=MPIDIR],
                            [Specify path to MPI installation])]
           )
MPIDIR=${with_mpi}

if test "x${want_parallel}" != xno
then
	
	### Check for MPI compiler
	
	# if MPI path specified, use that. Else, use env PATH
	if test "x${MPIDIR}" = x
	then
		MPIPATH="${PATH}"
	else
		MPIPATH="${MPIDIR}/bin"
	fi
	
	# Save env values
	original_CC="${CC}"
	original_LIBS="${LIBS}"
	original_CFLAGS="${CFLAGS}"
	original_LDFLAGS="${LDFLAGS}"
	
	
	MPICC=""
	MPILIBS=""
	MPICFLAGS=""

	# Look for MPI C compiler wrappers
	AC_PATH_PROGS(MPICC, [mpicc mpcc_r mpcc mpxlc_r mpxlc hcc cmpicc], none, ${MPIPATH})
	AC_MSG_CHECKING([for MPI C Compiler wrapper])
	if test ! "x${MPICC}"  = "xnone"
	then
		AC_MSG_RESULT([found ${MPICC}])
		
		AC_MSG_CHECKING([checking ${MPICC} for -show option])
		mpi_compile_test="`${MPICC} -show`"
		
		if test ! x"$?" = x0
		then
			AC_MSG_RESULT([none])
			AC_MSG_CHECKING([checking ${MPICC} for -showme option])
			
			mpi_compile_test="`${MPICC} -showme`"
			if test ! x"$?" = x0
			then
				AC_MSG_RESULT([none])
				mpi_compile_test=""
			else
				AC_MSG_RESULT([found])
				mpi_link_test="`${MPICC} -showme DUMMY.o -o DUMMY`"
			fi
		else
			AC_MSG_RESULT([found])
			mpi_link_test="`${MPICC} -show DUMMY.o -o DUMMY`"
		fi
		
		if test ! "x${mpi_compile_test}" = "x"
		then
			
			mpi_cc="`echo ${mpi_compile_test} | cut -d' ' -f1`"
			mpi_compile_args="`echo ${mpi_compile_test} | cut -d' ' -f2-`"
			mpi_link_args="`echo ${mpi_link_test} | cut -d' ' -f2-`"
			
			# remove -l* entries in CFLAGS
			mpi_compile_args="`echo ${mpi_compile_args} | sed -e \"s/\-l@<:@^ @:>@*//g\" | tr -s ' '`"
			
			# remove DUMMY filenames
			mpi_link_args="`echo ${mpi_link_args} | sed -e \"s/DUMMY.o//g\"`"
			mpi_link_args="`echo ${mpi_link_args} | sed -e \"s/[']*-o[']* DUMMY//g\"`"
			mpi_link_args="`echo ${mpi_link_args} | sed -e \"s/DUMMY.o -o DUMMY//g\"`"
			
			# remove -I from link - the @<:@ and @:>@ become [ and ] when m4sh is done
          	mpi_link_args="`echo ${mpi_link_args} | sed -e \"s/\-I@<:@^ @:>@*//g\"`"
          	mpi_ldflags="`echo ${mpi_link_args} | sed -e \"s/\-l@<:@^ @:>@*//g\"`"
          	mpi_libs="`(for i in ${mpi_link_args}; do echo $i | awk '/^-l/'; done) | tr '\n' ' '`"
          	
          	AC_MSG_CHECKING([compiler flags for MPI support])
          	MPICFLAGS="${mpi_compile_args}"
          	AC_MSG_RESULT([${MPICFLAGS}])
          	
          	AC_MSG_CHECKING([linker flags used by ${MPICC}])
          	MPILDFLAGS="${mpi_ldflags}"
          	AC_MSG_RESULT([${MPILDFLAGS}])
          	
          	AC_MSG_CHECKING([additional libs added by ${MPICC}])
          	MPILIBS="${mpi_libs}"
          	AC_MSG_RESULT([${MPILIBS}])
          	
		else
			AC_MSG_WARN([Unknown MPI compiler (${MPICC}). Will try default compiler.])
			MPICC="none"
		fi
	else
		AC_MSG_RESULT([none found])
	fi
	
	# if mpicc not found or unusable, check for libraries manually
	if test x"${MPICC}" = xnone
	then
	
		mpi_init_found="no"
		
		if test "${mpi_init_found}" = no
		then
			AC_CHECK_FUNC(MPI_Init, [mpi_init_found="yes"])
			if test x"${mpi_init_found}" = xyes
			then
				AC_MSG_RESULT([found])
				MPICC="${CC}"
			fi
		fi
		
		if test "${mpi_init_found}" = no
		then
			AC_CHECK_LIB(mpi, MPI_Init, [mpi_init_found="yes"])
			if test x"${mpi_init_found}" = xyes
			then
				AC_MSG_RESULT([found])
				MPICC="${CC}"
				MPILIBS="-lmpi"
			fi
		fi
		
		if test "${mpi_init_found}" = no
		then
			AC_CHECK_LIB(mpich, MPI_Init, [mpi_init_found="yes"])
			if test x"${mpi_init_found}" = xyes
			then
				AC_MSG_RESULT([found])
				MPICC="${CC}"
				MPILIBS="-lmpich"
			fi
		fi
		
	fi
	
	if test x"${MPICC}" = xnone
	then
	
		AC_MSG_ERROR([
 ** MPI Library not found. If you do have MPI installed, try specifying the 
    installation path using --with-mpi=MPI_DIR.
 ** Or, if you do not need the parallel libraries, reconfigure with the
    --disable-parallel option to build only the serial libraries and tests.	])
    
	fi
	
	# check for mpi headers
	# Use AC TRY COMPILE because AC CHECK HEADERS uses $CPP
	AC_MSG_CHECKING([for mpi.h])

	LIBS="${MPILIBS}"
	LDFLAGS="${MPILDFLAGS}"
	CFLAGS="${MPICFLAGS}"
	
	AC_TRY_COMPILE([#include <mpi.h>],[],
                   [mpi_header="yes"; AC_MSG_RESULT(yes)],
                   [mpi_header="no"; AC_MSG_RESULT(no) ])
                   
    if test x"${mpi_header}" = "xno"
    then
    	AC_MSG_ERROR([Could not locate MPI header file (mpi.h)])
    fi
	
	# Store CC that mpicc wrapper uses
	mpi_cc_wrapper="${MPICC}"
	MPICC="${mpi_cc}"
	
	# check for pthreads.
	ACX_PTHREAD([],[AC_MSG_ERROR([
 ** pthread support is required for building parallel library.
 ** Please install the pthread library and try again. If you do have PTHREADS 
    installed,specify the installation path using --with-pthreads=PTHREADS_DIR.
 ** Or, if you do not need the parallel libraries, reconfigure with the
    --disable-parallel option to build only the serial libraries and tests.	])
    ])

	 # restore env values
	CC="${original_CC}"
	LIBS="${original_LIBS}"
	CFLAGS="${original_CFLAGS}"
	LDFLAGS="${original_LDFLAGS}"
	
	if test ! ${CC} = ${PTHREAD_CC}
	then
		AC_MSG_WARN([Default compiler changed from $CC to $PTHREAD_CC to enable pthread support])
		CC="${PTHREAD_CC}"
	fi
	THREADS_CFLAGS="${PTHREAD_CFLAGS}"
	THREADS_LIBS="${PTHREAD_LIBS}"
	# yes, use of CFLAGS as LDFLAGS is intentional. Not a typo.
	THREADS_LDFLAGS="${THREADS_CFLAGS}" 
	
	
	# export MPI vars
	AC_SUBST(MPICC)
	AC_SUBST(MPILIBS)
	AC_SUBST(MPILDFLAGS)
	AC_SUBST(MPICFLAGS)
	
	# export THREADS vars
	AC_SUBST(THREADS_LIBS)
	AC_SUBST(THREADS_CFLAGS)
	AC_SUBST(THREADS_LDFLAGS)
	
	AM_CONDITIONAL([COMPILE_PARALLEL], [true])
else
	AM_CONDITIONAL([COMPILE_PARALLEL], [false])
fi

])dnl MBOARD_CHECK_PARALLEL
