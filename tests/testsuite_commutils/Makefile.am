# $Id: Makefile.am 2108 2009-09-14 14:14:31Z lsc $
# 
# Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Aug 2009

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_commutils.la

lib_t_commutils_la_SOURCES = setup_suite_commutils.c \
                             test_cu_utils.c \
                             test_cu_tagmessages.c \
                             test_cu_updatecursor.c \
                             test_cu_buildbuffer_all.c \
                             test_cu_buildbuffers_tagged.c \
                             test_cu_loadbuffer.c \
							 header_commutils.h
							 
lib_t_commutils_la_LIBS    = @CUNITLIBS@ @MB_PARALLEL_LIBS@ 
lib_t_commutils_la_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@ 
lib_t_commutils_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif