# $Id$

AC_DEFUN([LSC_NOREPEAT_SPACES], [
dnl Remove repreated spaces in var
	$1=`echo [$]$1 | tr -s ' '`
])

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
  AC_DEFINE_UNQUOTED($1, "$ac_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])
