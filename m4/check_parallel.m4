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

if test "x${want_parallel}" != xno;
then
	
	### Check for MPI compiler
	
	# if MPI path specified, use that. Else, use env PATH
	if test "x${MPIDIR}" = x;
	then
		MPIPATH="${PATH}"
	else
		MPIPATH="${MPIDIR}/bin"
	fi
	
	# Save env values
	original_CC="${CC}"
	original_LIBS="${LIBS}"
	original_CFLAGS="${CFLAGS}"
	original_LDFLAGS="${CFLAGS}"
	
	# Look for MPI C compiler wrappers
	MPICC=
	MPILIBS=
	MPICFLAGS=

	AC_PATH_PROGS(MPICC, [mpicc mpcc_r mpcc mpxlc_rmpxlc hcc cmpicc], none, ${MPIPATH})
	AC_MSG_CHECKING([for MPI C Compiler wrapper])
	if test ! "x${MPICC}"  = "xnone"
	then
		AC_MSG_RESULT([found ${mpicc}])
		
		AC_MSG_CHECKING([for MPI Compiler style])
		if test ! "`${MPICC} -showme 2>&1 | grep lmpi`" = ""
		then
			AC_MSG_RESULT([LAM])
			
			mpi_compile_test="`${MPICC} -showme -c DUMMY.c`"
			mpi_compile_args="`echo ${mpi_compile_test} | cut -d' ' -f2-`"
			mpi_link_test="`${MPICC} -showme DUMMY.o -o DUMMY`"
			mpi_link_args="`echo ${mpi_link_test} | cut -d' ' -f2-`"
			
			# remove DUMMY filenames
			mpi_compile_args="`echo ${mpi_compile_args} | sed -e \"s/-c DUMMY.c//g\"`"
			mpi_link_args="`echo ${mpi_link_args} | sed -e \"s/DUMMY.o -o DUMMY//g\"`"
			
			# remove -I from link - the @<:@ and @:>@ become [ and ] when m4sh is done
          	mpi_link_args="`echo ${mpi_link_args} | sed -e \"s/\-I@<:@^ @:>@*//g\"`"
          	
          	AC_MSG_CHECKING([compiler flags for MPI support])
          	MPICFLAGS="${mpi_compile_args}"
          	AC_MSG_RESULT([${MPICFLAGS}])
          	
          	AC_MSG_CHECKING([libraries for MPI support])
          	MPILIBS="${mpi_link_args}"
          	AC_MSG_RESULT([${MPILIBS}])
          	
		elif test ! "`${MPICC} -show 2>&1 | grep lmpi`" = ""
		then
			AC_MSG_RESULT([MPICH])
			
			mpi_compile_test="`${MPICC} -show -c DUMMY.c`"
			mpi_compile_args="`echo ${mpi_compile_test} | cut -d' ' -f2-`"
			mpi_link_test="`${MPICC} -show DUMMY.o -o DUMMY`"
			mpi_link_args="`echo ${mpi_link_test} | cut -d' ' -f2-`"
			
			# remove DUMMY filenames
			mpi_compile_args="`echo ${mpi_compile_args} | sed -e \"s/-c DUMMY.c//g\"`"
			mpi_link_args="`echo ${mpi_link_args} | sed -e \"s/DUMMY.o -o DUMMY//g\"`"
			
			# remove -I from link - the @<:@ and @:>@ become [ and ] when m4sh is done
          	mpi_link_args="`echo ${mpi_link_args} | sed -e \"s/\-I@<:@^ @:>@*//g\"`"
          	
          	AC_MSG_CHECKING([compiler flags used by ${MPICC}])
          	MPICFLAGS="${mpi_compile_args}"
          	AC_MSG_RESULT([${MPICFLAGS}])
          	
          	AC_MSG_CHECKING([linker flags used by ${MPICC}])
          	MPILIBS="${mpi_link_args}"
          	AC_MSG_RESULT([${MPILIBS}])
		else
			AC_MSG_RESULT([unknown])
			AC_MSG_WARN([Unknown MPI compiler found. Will try default compiler.])
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
	# Use AC_TRY_COMPILE because AC_CHECK_HEADERS uses $CPP
	AC_MSG_CHECKING([for mpi.h])

	# CC="${MPICC}"
	LIBS="${MPILIBS}"
	CFLAGS="${MPICFLAGS}"
	AC_TRY_COMPILE([#include <mpi.h>],[],
                   [mpi_header="yes"; AC_MSG_RESULT(yes)],
                   [mpi_header="no"; AC_MSG_RESULT(no) ])
    if test x"${mpi_header}" = "xno"
    then
    	AC_MSG_ERROR([Could not locate MPI header file (mpi.h)])
    fi
    
    # restore env values
	CC="${original_CC}"
	LIBS="${original_LIBS}"
	CFLAGS="${original_CFLAGS}"
	
	# export MPI vars
	AC_SUBST(MPICC)
	AC_SUBST(MPILIBS)
	AC_SUBST(MPICFLAGS)
	
	
	### Check for pthread
	THREADS_LIBS=
	pthreads_found="no"
	
	# Check for libpthread
	AC_CHECK_LIB([pthread], [pthread_create], [THREADS_LIBS=-lpthread; pthreads_found="yes"])
	
	# Else, check for libpthreads
	if test x"$pthreads_found" = xno
	then
		AC_CHECK_LIB([pthreads], [pthread_create], [THREADS_LIBS=-lpthreads; pthreads_found="yes"])
	fi
	
	# Else, check for pthread support within standard C library
	if test x"$pthreads_found" = xno
	then
		AC_CHECK_LIB([c], [pthread_create], [THREADS_LIBS=""; pthreads_found="yes"],
		             [AC_MSG_ERROR([
 ** pthread support is required for building parallel library.
 ** Please install the pthread library and try again. 
 ** Or, if you do not need the parallel libraries, reconfigure with the
    --disable-parallel option to build only the serial libraries and tests.	])
                     ])
   
	fi

	AC_SUBST(THREADS_LIBS)
	AC_SUBST(THREADS_CFLAGS)
	
	AM_CONDITIONAL([COMPILE_PARALLEL], [true])
else
	AM_CONDITIONAL([COMPILE_PARALLEL], [false])
fi

])dnl
