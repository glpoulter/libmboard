# $Id$
# 
# Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Sept 2009

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_commroutines_handshake.la

lib_t_commroutines_handshake_la_SOURCES = setup_suite_cr_handshake.c \
                                          test_cr_handshake_utils.c \
                                          test_cr_handshake_setup.c \
                                          test_cr_handshake_bufsize.c \
                                          test_cr_handshake_prop.c \
                                          test_cr_handshake_loadfree.c \
                                          header_commroutines_handshake.h

lib_t_commroutines_handshake_la_LIBS    = @CUNITLIBS@ @MB_PARALLEL_LIBS@ 
lib_t_commroutines_handshake_la_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@ 
lib_t_commroutines_handshake_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif
