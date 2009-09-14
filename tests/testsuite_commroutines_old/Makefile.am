# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_commroutines_old.la

lib_t_commroutines_old_la_SOURCES = setup_suite_commroutines_old.c \
							        test_cr_old_setup_teardown.c \
							        test_cr_old_utils.c \
							        test_cr_old_completepropagation.c \
							        test_cr_old_initpropagation.c \
							        test_cr_old_sendbufinfo.c \
							        test_cr_old_tagmessages.c \
							        test_cr_old_waitbufinfo.c \
							        header_commroutines_old.h
							 
lib_t_commroutines_old_la_LIBS    = @CUNITLIBS@ @MB_PARALLEL_LIBS@ 
lib_t_commroutines_old_la_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@ 
lib_t_commroutines_old_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif