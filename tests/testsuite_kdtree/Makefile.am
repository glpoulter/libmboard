# $Id$
# 
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory 
# Author: Shawn Chin 
# Date  : July 2012

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
noinst_LTLIBRARIES  = lib_t_kdtree.la

lib_t_kdtree_la_SOURCES = setup_suite_kdtree.c \
                          testsuite_kd_utils.c \
						  test_kd_create_delete.c \
						  test_kd_search_1d.c \
						  test_kd_search_2d.c \
						  test_kd_search_3d.c \
						  header_kdtree.h
							 
lib_t_kdtree_la_LIBS    = @CUNITLIBS@ @MB_PARALLEL_LIBSS@ 
lib_t_kdtree_la_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@ 
lib_t_kdtree_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif