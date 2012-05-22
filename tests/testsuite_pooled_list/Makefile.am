# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED

noinst_LTLIBRARIES  = lib_t_pooled_list.la

lib_t_pooled_list_la_SOURCES = setup_suite_pl.c \
						 	   test_pl_create.c \
						 	   test_pl_addnode.c \
						 	   test_pl_traversal.c \
						 	   test_pl_reset_recycle.c \
						 	   test_pl_randomise.c \
						 	   testsuite_pl_utils.c \
					       header_pooled_list.h

lib_t_pooled_list_la_LIBS    = @CUNITLIBS@							 
lib_t_pooled_list_la_LDFLAGS = @CUNITLDFLAGS@
lib_t_pooled_list_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif
