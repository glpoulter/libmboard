# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_commroutines.la

lib_t_commroutines_la_SOURCES = setup_suite_commroutines.c \
							    test_cr1.c \
							    header_commroutines.h
							 
lib_t_commroutines_la_LIBS    = @CUNITLIBS@ @MB_PARALLEL_LIBS@ 
lib_t_commroutines_la_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@ 
lib_t_commroutines_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif