# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED

noinst_LTLIBRARIES  = lib_t_mb_serial.la

lib_t_mb_serial_la_SOURCES = setup_suite_mb_serial.c \
							   test_mb_s_init.c \
							   test_mb_s_create.c \
							   test_mb_s_addmessage.c \
							   test_mb_s_setaccessmode.c \
							   test_mb_s_clear.c \
							   test_mb_s_sync.c \
							   test_mb_s_iterator.c \
							   test_mb_s_iterator_sorted.c \
							   test_mb_s_iterator_filtered.c \
							   test_mb_s_iterator_rewind.c \
							   test_mb_s_iterator_randomise.c \
							   test_mb_s_iterator_filteredsorted.c \
							   test_mb_s_filter.c \
							   test_mb_s_indexmap_create.c \
							   test_mb_s_indexmap_delete.c \
							   test_mb_s_indexmap_sync.c \
							   test_mb_s_indexmap_addentry.c \
							   test_mb_s_indexmap_memberof.c \
							   testsuite_mb_serial_utils.c \
							   header_mb_serial.h
							 
lib_t_mb_serial_la_LIBS    = @CUNITLIBS@
lib_t_mb_serial_la_LDFLAGS = @CUNITLDFLAGS@
lib_t_mb_serial_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif