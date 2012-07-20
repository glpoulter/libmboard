# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

testcode_dir = $(top_srcdir)/tests
INCLUDEDIR = $(top_srcdir)/include


if TESTS_CONFIGURED

SUBDIRS = testsuite_commqueue \
          testsuite_commutils \
          testsuite_commroutines_old \
          testsuite_commroutines_handshake \
          testsuite_mb_parallel \
          testsuite_mb_serial \
          testsuite_objmap \
          testsuite_tag_table \
          testsuite_pooled_list \
          testsuite_string_map \
          testsuite_avltree \
          testsuite_kdtree \
          testsuite_syncqueue
          
DIST_SUBDIRS = $(SUBDIRS)

noinst_PROGRAMS  = run_test_utils \
                   run_test_serial

if COMPILE_PARALLEL
noinst_PROGRAMS += run_test_parallel_utils \
                   run_test_parallel

run_test_parallel_utils_SOURCES = run_test_parallel_utils.c testing.h
run_test_parallel_utils_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@
run_test_parallel_utils_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@ @MB_PARALLEL_CFLAGS@
run_test_parallel_utils_LDADD   =  \
                                  $(testcode_dir)/testsuite_commqueue/lib_t_commqueue.la \
                                  $(testcode_dir)/testsuite_syncqueue/lib_t_syncqueue.la \
                                  $(testcode_dir)/testsuite_commroutines_old/lib_t_commroutines_old.la \
                                  $(testcode_dir)/testsuite_commroutines_handshake/lib_t_commroutines_handshake.la \
                                  $(testcode_dir)/testsuite_commutils/lib_t_commutils.la \
                                  $(top_srcdir)/src/parallel/lib_mb_pd.la \
                                  $(top_srcdir)/src/utils/lib_utils_pd.la \
                                  @CUNITLIBS@ @MB_PARALLEL_LIBS@

               
run_test_parallel_SOURCES = run_test_parallel.c testing.h
run_test_parallel_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@
run_test_parallel_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@ @MB_PARALLEL_CFLAGS@
run_test_parallel_LDADD   = \
                             $(testcode_dir)/testsuite_mb_parallel/lib_t_mb_parallel.la \
                             $(top_srcdir)/src/parallel/lib_mb_pd.la \
                             $(top_srcdir)/src/utils/lib_utils_pd.la \
                             @CUNITLIBS@ @MB_PARALLEL_LIBS@ 

endif

run_test_utils_SOURCES  = run_test_utils.c testing.h
run_test_utils_LDFLAGS  = @CUNITLDFLAGS@
run_test_utils_CFLAGS   = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@
run_test_utils_LDADD    = $(testcode_dir)/testsuite_objmap/lib_t_objmap.la \
                          $(testcode_dir)/testsuite_pooled_list/lib_t_pooled_list.la \
                          $(testcode_dir)/testsuite_tag_table/lib_t_tag_table.la \
                          $(testcode_dir)/testsuite_avltree/lib_t_avltree.la \
                          $(testcode_dir)/testsuite_kdtree/lib_t_kdtree.la \
                          $(testcode_dir)/testsuite_string_map/lib_t_string_map.la \
                          $(top_srcdir)/src/utils/lib_utils_d.la \
                          @CUNITLIBS@

               
run_test_serial_SOURCES = run_test_serial.c testing.h
run_test_serial_LDFLAGS = @CUNITLDFLAGS@
run_test_serial_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@
run_test_serial_LDADD   = $(testcode_dir)/testsuite_mb_serial/lib_t_mb_serial.la \
                          $(top_srcdir)/src/serial/lib_mb_sd.la \
                          $(top_srcdir)/src/utils/lib_utils_d.la \
                          @CUNITLIBS@



run: 
	@(for i in $(noinst_PROGRAMS); do ./$$i ; done)
	@echo -e "\n\n===== ALL TESTS COMPLETED SUCCESSFULLY =====\n"
if COMPILE_PARALLEL
	@echo "+ To be sure everything is working as expected, you might want to try running "
	@echo "  the following tests using different processor counts:"
	@echo "   - run_test_parallel_utils"
	@echo "   - run_test_parallel"
	@echo -e "\n\n"
endif

# Options for splint
# -D__sigset_t=int hides problems with splint trying to parse headers
# used by pthread.h
SPLINT_OPTIONS = -weak -D__sigset_t=int -warnposix
SERIAL_TESTSUITES = objmap tag_table pooled_list mb_serial avltree kdtree string_map
PARALLEL_TESTSUITES = syncqueue commqueue commutils commroutines_old commroutines_handshake mb_parallel 

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

	@( \
	iflags=""; \
	for i in dummy @MB_PARALLEL_CFLAGS@ @CUNITCFLAGS@ ; do \
		f=`echo $${i} | grep '^-I'`; \
		iflags="$${iflags} $${f}"; \
	done; \
	echo -e "\n----- (in ./tests: Checking ./*.c)"; \
	splint $(SPLINT_OPTIONS) $${iflags} -I$(INCLUDEDIR) -I/usr/local/include *.c; \
	for ts in $(SERIAL_TESTSUITES); do \
		echo -e "\n----- (in ./tests: Checking ./testsuite_$${ts}/*.c)"; \
		splint $(SPLINT_OPTIONS) $${iflags} -I$(INCLUDEDIR) -I/usr/local/include testsuite_$${ts}/*.c; \
	done; \
	) 
if COMPILE_PARALLEL
	@( \
	iflags=""; \
	for i in dummy @MB_PARALLEL_CFLAGS@ @CUNITCFLAGS@ ; do \
	f=`echo $${i} | grep '^-I'`; \
	iflags="$${iflags} $${f}"; \
	done; \
	for ts in $(PARALLEL_TESTSUITES); do \
		echo -e "\n----- (in ./tests: Checking ./testsuite_$${ts}/*.c)"; \
		splint $(SPLINT_OPTIONS) $${iflags} -I$(INCLUDEDIR) -I/usr/local/include testsuite_$${ts}/*.c; \
	done; \
	)  
endif

else

all: run
splint: run

run:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif


