# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : July 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED

noinst_LTLIBRARIES  = lib_t_tag_table.la

lib_t_tag_table_la_SOURCES = setup_suite_tt.c \
						     test_tt_new.c \
						     test_tt_getrow.c \
						     test_tt_setbits.c \
						     test_tt_setbyte.c \
						     test_tt_getcount_row.c \
						     test_tt_getcount_col.c \
					         header_tag_table.h
						
lib_t_tag_table_la_LIBS    = @CUNITLIBS@	 
lib_t_tag_table_la_LDFLAGS = @CUNITLDFLAGS@
lib_t_tag_table_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif