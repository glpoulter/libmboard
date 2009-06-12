# $Id$
# 
# Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2009

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED

noinst_LTLIBRARIES  = lib_t_string_map.la

lib_t_string_map_la_SOURCES = setup_suite_sm.c \
						  test_sm_contains.c \
						  test_sm_create.c \
						  test_sm_delete.c \
						  test_sm_addstring.c \
						  test_sm_removestring.c \
					      header_string_map.h
						
lib_t_string_map_la_LIBS    = @CUNITLIBS@	 
lib_t_string_map_la_LDFLAGS = @CUNITLDFLAGS@
lib_t_string_map_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif