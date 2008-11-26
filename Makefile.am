# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

# Let make know where m4 scripts are if it needs to run aclocal
ACLOCAL_AMFLAGS = "-I m4"

SUBDIRS = src/utils src/parallel src/serial src/progs
DIST_SUBDIRS = $(SUBDIRS) tests

INCLUDEDIR = $(top_srcdir)/include

include_HEADERS = $(INCLUDEDIR)/mboard.h \
			      $(INCLUDEDIR)/mb_memlog.h \
                  $(INCLUDEDIR)/mb_config.h \
			      $(INCLUDEDIR)/mb_objmap.h
noinst_HEADERS  = $(INCLUDEDIR)/mb_common.h \
			      $(INCLUDEDIR)/mb_serial.h \
			      $(INCLUDEDIR)/mb_parallel.h \
			      $(INCLUDEDIR)/mb_utils.h \
			      $(INCLUDEDIR)/mb_syncqueue.h \
			      $(INCLUDEDIR)/mb_commqueue.h \
			      $(INCLUDEDIR)/mb_pooled_list.h \
			      $(INCLUDEDIR)/uthash.h

if COMPILE_SQLITE
noinst_HEADERS += $(INCLUDEDIR)/sqlite3.h
endif

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
libmboard_p_la_LIBADD   = src/utils/lib_utils_p.la src/parallel/lib_mb_p.la @MB_PARALLEL_LIBS@

libmboard_pd_la_SOURCES =
libmboard_pd_la_LDFLAGS = @MB_PARALLEL_LDFLAGS@
libmboard_pd_la_LIBADD  = src/utils/lib_utils_pd.la src/parallel/lib_mb_pd.la @MB_PARALLEL_LIBS@
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
	@echo "** To enable tests, make sure you have CUnit installed, then rerun ./configure"
	
endif

check:
	@echo -e "\n\nPlease use "make test" to compile and run unit tests"
	
doc: Doxyfile_user Doxyfile_developer_serial Doxyfile_developer_parallel
	@( \
	DOXYVER=`doxygen --version 2>/dev/null`; \
	if test ! $$? = 0; then \
		echo "** Doxygen is required to build the documentation" >&2; \
		echo "** It is available from http://www.doxygen.org" >&2; \
		echo -e "\n"; \
		exit 1; \
	fi; \
	echo -e "Found doxygen version $$DOXYVER\n"; \
	)
	@mkdir -p doc/developer &> /dev/null
	@echo "Generating User documentation in ./doc/user"
	@doxygen Doxyfile_user
	@echo "Generating Developer documentation (serial code) in ./doc/developer/serial"
	@doxygen Doxyfile_developer_serial
	@echo "Generating Developer documentation (parallel code) in ./doc/developer/parallel"
	@doxygen Doxyfile_developer_parallel

	@touch Doxyfile_user
	@touch Doxyfile_developer_serial
	@touch Doxyfile_developer_parallel
	
vclean:
	(cd tests; make clean)
	make clean

# Options for splint
# -D__sigset_t=int hides problems with splint trying to parse headers
# used by pthread.h
SPLINT_OPTIONS = -weak -D__sigset_t=int

splint:
	@( \
	SPLINT=`splint --help 2>/dev/null`; \
	if test ! $$? = 0; then \
		echo "** 'splint' command not found" >&2; \
		echo "** It is available from http://www.splint.org/" >&2; \
		echo -e "\n"; \
		exit 1; \
	fi; \
	)
	@echo -e "\n----- (Checking src/utils/*.c)"
	@splint $(SPLINT_OPTIONS) -I$(INCLUDEDIR) `ls src/utils/*.c | grep -v sqlite3.c`
	@echo -e "\n----- (Checking src/serial/*.c)"
	@splint $(SPLINT_OPTIONS) -I$(INCLUDEDIR) src/serial/*.c 
	@echo -e "\n----- (Checking src/progs/*.c)"
	@for i in src/progs/*.c; do splint $(SPLINT_OPTIONS) -I$(INCLUDEDIR) $$i; done
if COMPILE_PARALLEL
	@( \
	iflags=""; \
	for i in dummy @MB_PARALLEL_CFLAGS@; do f=`echo $${i} | grep '^-I'`; iflags="$${iflags} $${f}"; done; \
	echo -e "\n----- (Checking src/parallel/*.c)"; \
	splint $(SPLINT_OPTIONS) $${iflags} -I$(INCLUDEDIR) src/parallel/*.c; \
	) 
endif
if TESTS_CONFIGURED
	@echo -e "\n ----- (Entering test directory: ./tests) "
	(cd tests && make splint)
endif

