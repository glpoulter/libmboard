# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

SUBDIRS = src/utils src/parallel src/serial 
DIST_SUBDIRS = $(SUBDIRS) tests

INCLUDEDIR = $(top_srcdir)/include

include_HEADERS = ${INCLUDEDIR}/mboard.h \
			      ${INCLUDEDIR}/objmap.h \
			      ${INCLUDEDIR}/mb_common.h \
			      ${INCLUDEDIR}/mb_serial.h \
			      ${INCLUDEDIR}/mb_parallel.h \
			      ${INCLUDEDIR}/mb_utils.h \
			      ${INCLUDEDIR}/syncqueue.h \
			      ${INCLUDEDIR}/commqueue.h \
			      ${INCLUDEDIR}/pooled_list.h \
			      ${INCLUDEDIR}/uthash.h

lib_LTLIBRARIES  = libmboard_s.la libmboard_sd.la 
if COMPILE_PARALLEL
lib_LTLIBRARIES += libmboard_p.la libmboard_pd.la
endif

libmboard_s_la_SOURCES  =
libmboard_s_la_LDFLAGS  = 
libmboard_s_la_LIBADD   = src/utils/lib_utils.la src/serial/lib_mb_s.la

libmboard_sd_la_SOURCES =
libmboard_sd_la_LDFLAGS = 
libmboard_sd_la_LIBADD  = src/utils/lib_utils_d.la src/serial/lib_mb_sd.la

if COMPILE_PARALLEL
libmboard_p_la_SOURCES  =
libmboard_p_la_LDFLAGS  = @MB_PARALLEL_LDFLAGS@
libmboard_p_la_LIBADD   = src/utils/lib_utils.la src/parallel/lib_mb_p.la @MB_PARALLEL_LIBS@

libmboard_pd_la_SOURCES =
libmboard_pd_la_LDFLAGS = @MB_PARALLEL_LDFLAGS@
libmboard_pd_la_LIBADD  = src/utils/lib_utils_d.la src/parallel/lib_mb_pd.la @MB_PARALLEL_LIBS@
endif

EXTRA_DIST = \
    AUTHORS NEWS INSTALL README  ChangeLog \
    Doxyfile_user \
    Doxyfile_developer_serial \
    Doxyfile_developer_parallel \
    m4/check_mboard.m4 \
    m4/check_parallel.m4 \
    example

dist-hook:
	rm -rf `find $(distdir) -name .svn`
	 
if TESTS_CONFIGURED

test: 
	@(make && cd tests && make && make run)

else

test:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif