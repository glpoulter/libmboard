# $Id: Makefile.am 1871 2009-06-12 13:58:18Z lsc $
# 
# Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : May 2009

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
noinst_LTLIBRARIES  = lib_t_avltree.la

lib_t_avltree_la_SOURCES = setup_suite_avltree.c \
                           testsuite_avl_utils.c \
						   test_avl_create_delete.c \
						   test_avl_insert.c \
						   test_avl_getmin_getmax.c \
						   test_avl_dump.c \
						   test_avl_walk.c \
						   test_avl_getdata.c \
						   test_avl_getnode.c \
						   test_avl_memberof.c \
						   header_avltree.h
							 
lib_t_avltree_la_LIBS    = @CUNITLIBS@ @MB_PARALLEL_LIBSS@ 
lib_t_avltree_la_LDFLAGS = @CUNITLDFLAGS@ @MB_PARALLEL_LDFLAGS@ 
lib_t_avltree_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif