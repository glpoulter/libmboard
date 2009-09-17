# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_mb_parallel.la

lib_t_mb_parallel_la_SOURCES = setup_suite_mb_parallel.c \
							   test_mb_p_init.c \
							   test_mb_p_finalise.c \
							   test_mb_p_create.c \
							   test_mb_p_clear.c \
							   test_mb_p_sync.c \
							   test_mb_p_addmessage.c \
							   test_mb_p_setaccessmode.c \
							   test_mb_p_setsyncpattern.c \
							   test_mb_p_iterator_create.c \
							   test_mb_p_iterator_createsorted.c \
							   test_mb_p_iterator_createfiltered.c \
							   test_mb_p_iterator_rewind.c \
							   test_mb_p_iterator_randomise.c \
							   test_mb_p_iterator_createfilteredsorted.c \
							   test_mb_p_indexmap_create.c \
							   test_mb_p_indexmap_delete.c \
							   test_mb_p_indexmap_sync.c \
							   test_mb_p_indexmap_addentry.c \
							   test_mb_p_indexmap_memberof.c \
							   test_mb_p_filter_create.c \
							   test_mb_p_filter_assign.c \
							   test_mb_p_filter_delete.c \
							   testsuite_mb_parallel_utils.c \
							   header_mb_parallel.h
							 
lib_t_mb_parallel_la_LIBS    = @CUNITLIBS@ @MB_PARALLEL_LIBS@ 
lib_t_mb_parallel_la_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@ 
lib_t_mb_parallel_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif