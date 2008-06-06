AC_DEFUN([MBOARD_CHECK_TESTING], [
	
# C compiler is required
AC_REQUIRE([AC_PROG_CC])

# Test code build enabled by default
want_tests=yes

# If users want to skip building test code
AC_ARG_ENABLE([tests],
    [AC_HELP_STRING([--disable-tests],
                    [Disable building of unit tests])],
    [want_tests=no],
    [want_tests=yes])
    
# Allow users to manually specify path to CUnit installation
AC_ARG_WITH([cunit],
            [AC_HELP_STRING([--with-cunit=CUNITDIR],
                            [Specify path to CUnit installation])]
           )

CUNITDIR=${with_cunit}


if test ! "x${want_tests}" = xno;
then
   
	CUNITLIBS="-lcunit"
	
	# backup env vars
	original_CFLAGS="${CFLAGS}"
	original_LDFLAGS="${LDFLAGS}"
	original_LIBS="${LIBS}"
	
	# if CUnit path specified, use that.
	if test ! "x${CUNITDIR}" = x;
	then
		CUNITCFLAGS="-I${CUNITDIR}/include -L${CUNITDIR}/lib"	
	fi
	
	LIBS="${CUNITLIBS}"
	CFLAGS="${CUNITCFLAGS}"
	have_cunit=""
	
	AC_CHECK_LIB([cunit], [CU_get_error], [], [AC_MSG_ERROR([
** libcunit could not be found. If you have CUnit installed, try specifying 
   the installation path using --with-cunit=CUNITDIR.
** Or, if you do not need to compile the unit tests, reconfigure with the
   --disable-tests option.])])
   
	AC_MSG_CHECKING([for CUnit/Basic.h])
	AC_TRY_COMPILE([#include <CUnit/Basic.h>],[],
                   [have_cunit="yes"; AC_MSG_RESULT(yes)],
                   [have_cunit="no"; AC_MSG_RESULT(no) ])
    if test ! x"${have_cunit}" = "xyes"
    then
    	AC_MSG_ERROR([Could not locate CUnit header files])
    fi
    
	# restore env vars
	CFLAGS="${original_CFLAGS}"
	LDFLAGS="${original_LDFLAGS}"
	LIBS="${original_LIBS}"
	
	AC_SUBST(CUNITLIBS)
	AC_SUBST(CUNITCFLAGS)
	
	AM_CONDITIONAL([TESTS_CONFIGURED], [true])
else
	AM_CONDITIONAL([TESTS_CONFIGURED], [false])
fi
])dnl
