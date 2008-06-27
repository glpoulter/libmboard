# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_syncqueue.la

lib_t_syncqueue_la_SOURCES = setup_suite_syncqueue.c \
					   	     test_sq1.c \
							 header_syncqueue.h
							 
lib_t_syncqueue_la_LDFLAGS = @COMMON_LDFLAGS@ @CUNITLIBS@
lib_t_syncqueue_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MPICFLAGS@ @PARALLEL_EXTRA_CFLAGS@ @DEBUG_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif