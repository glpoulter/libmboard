# $Id$

AC_DEFUN([LSC_NOREPEAT_SPACES], [
dnl Remove repreated spaces in var
	$1=`echo [$]$1 | tr -s ' '`
])

dnl
dnl Check compiler support for C99 variadic macros
dnl sets HAVE_C99_VARIADIC_MACROS as appropriate
dnl
AC_DEFUN([LSC_C_C99_VARIADIC_MACROS],
	[AC_CACHE_CHECK([for Variadic Macros (C99)], [lsc_cv_c99_vamacros],
		[AC_TRY_COMPILE(
		[
			#include <stdio.h>
			#define monkeysays(...) fprintf(stderr, __VA_ARGS__)
		],
		[
			monkeysays("Give me a banana"); 
			monkeysays("Give me %d bananas", 3); 
			monkeysays("Give me %d bananas and %d Redbull", 3, 1); 
			return 0;
		],
		[lsc_cv_c99_vamacros=yes], [lsc_cv_c99_vamacros=no])
	])
	if test $lsc_cv_c99_vamacros = yes ; then
    	AC_DEFINE([HAVE_C99_VARIADIC_MACROS], 1,
            [Define if the compiler supports C99 Variadic Macros])
	fi])

dnl	
dnl Check compiler support for GNU-style variadic macros
dnl sets HAVE_GNU_VARIADIC_MACROS as appropriate
dnl
AC_DEFUN([LSC_C_GNU_VARIADIC_MACROS],
	[AC_CACHE_CHECK([for Variadic Macros (GNU-style)], [lsc_cv_gnu_vamacros],
		[AC_TRY_COMPILE(
		[
			#include <stdio.h>
			#define monkeysays(fmt, args...) fprintf(stderr, fmt , ## args)
		],
		[
			monkeysays("Give me a banana"); 
			monkeysays("Give me %d bananas", 3);
			monkeysays("Give me %d bananas and %d Redbull", 3, 1); 
			return 0;
		],
		[lsc_cv_gnu_vamacros=yes], [lsc_cv_gnu_vamacros=no])
	])
	if test $lsc_cv_gnu_vamacros = yes ; then
    	AC_DEFINE([HAVE_GNU_VARIADIC_MACROS], 1,
            [Define if the compiler supports GNU-style Variadic Macros])
	fi])

AC_DEFUN([AC_C_VAR_FUNC],
[AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether $CC recognizes __func__, ac_cv_c_var_func,
AC_TRY_COMPILE(,
[int main() {
char *s = __func__;
}],
AC_DEFINE(HAVE_FUNC,,
[Define if the C complier supports __func__]) ac_cv_c_var_func=yes,
ac_cv_c_var_func=no) )
])dnl




AC_DEFUN([AC_DEFINE_DIR], [
dnl This macro was copied from http://autoconf-archive.cryp.to/ac_define_dir.html
dnl Copyrights belong to the original authors:
dnl Copyright 2008 Stepan Kasal <kasal@ucw.cz>
dnl Copyright 2008 Andreas Schwab <schwab@suse.de>
dnl Copyright 2008 Guido U. Draheim <guidod@gmx.de>
dnl Copyright 2008 Alexandre Oliva

  prefix_NONE=
  exec_prefix_NONE=
  test "x$prefix" = xNONE && prefix_NONE=yes && prefix=$ac_default_prefix
  test "x$exec_prefix" = xNONE && exec_prefix_NONE=yes && exec_prefix=$prefix

dnl In Autoconf 2.60, ${datadir} refers to ${datarootdir}, which in turn
dnl refers to ${prefix}.  Thus we have to use `eval' twice.

  eval ac_define_dir="\"[$]$2\""
  eval ac_define_dir="\"$ac_define_dir\""
  AC_SUBST($1, "$ac_define_dir")
  #AC_DEFINE_UNQUOTED($1, "$ac_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])

