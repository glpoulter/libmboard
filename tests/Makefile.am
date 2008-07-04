# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

testcode_dir = $(top_srcdir)/tests
INCLUDEDIR = $(top_srcdir)/include


if TESTS_CONFIGURED

SUBDIRS = testsuite_commqueue \
          testsuite_commroutines \
          testsuite_mb_parallel \
          testsuite_mb_serial \
          testsuite_objmap \
          testsuite_pooled_list \
          testsuite_syncqueue
          
DIST_SUBDIRS = $(SUBDIRS)

noinst_PROGRAMS  = run_test_utils \
				   run_test_serial

if COMPILE_PARALLEL
noinst_PROGRAMS += run_test_parallel_utils \
				   run_test_parallel

run_test_parallel_utils_SOURCES = run_test_parallel_utils.c testing.h
run_test_parallel_utils_LDFLAGS = -static @CUNITLDFLAGS@ @THREADS_LDFLAGS@ @MPILDFLAGS@
run_test_parallel_utils_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@ @THREADS_CFLAGS@ @MPICFLAGS@
run_test_parallel_utils_LDADD   =  \
								  $(testcode_dir)/testsuite_commqueue/lib_t_commqueue.la \
                                  $(testcode_dir)/testsuite_syncqueue/lib_t_syncqueue.la \
                                  $(testcode_dir)/testsuite_commroutines/lib_t_commroutines.la \
                                  $(top_srcdir)/src/parallel/lib_mb_pd.la \
                                  $(top_srcdir)/src/utils/lib_utils.la \
						  		  @CUNITLIBS@ @THREADS_LIBS@ @MPILIBS@
						    	  
						    	  
               
run_test_parallel_SOURCES = run_test_parallel.c testing.h
run_test_parallel_LDFLAGS = -static @CUNITLDFLAGS@ @THREADS_LDFLAGS@ @MPILDFLAGS@
run_test_parallel_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@ @THREADS_CFLAGS@ @MPICFLAGS@
run_test_parallel_LDADD   = \
							$(testcode_dir)/testsuite_mb_parallel/lib_t_mb_parallel.la \
						    $(top_srcdir)/src/parallel/lib_mb_pd.la \
						    $(top_srcdir)/src/utils/lib_utils.la \
						    @CUNITLIBS@ @THREADS_LIBS@ @MPILIBS@ 
						    

endif

run_test_utils_SOURCES  = run_test_utils.c testing.h
run_test_utils_LDFLAGS  = -static @CUNITLDFLAGS@
run_test_utils_CFLAGS   = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@
run_test_utils_LDADD    = $(testcode_dir)/testsuite_objmap/lib_t_objmap.la \
                          $(testcode_dir)/testsuite_pooled_list/lib_t_pooled_list.la \
						  $(top_srcdir)/src/utils/lib_utils.la \
						  @CUNITLIBS@

               
run_test_serial_SOURCES = run_test_serial.c testing.h
run_test_serial_LDFLAGS = -static @CUNITLDFLAGS@
run_test_serial_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@
run_test_serial_LDADD   = $(testcode_dir)/testsuite_mb_serial/lib_t_mb_serial.la \
						  $(top_srcdir)/src/serial/lib_mb_sd.la \
						  $(top_srcdir)/src/utils/lib_utils.la \
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

else

all: run

run:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif