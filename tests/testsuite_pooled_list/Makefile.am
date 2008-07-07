# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED

noinst_LTLIBRARIES  = lib_t_pooled_list.la

lib_t_pooled_list_la_SOURCES = setup_suite_pl.c \
						 	   test_pl1.c \
						 	   test_pl2.c \
						 	   test_pl3.c \
						 	   test_pl4.c \
						 	   testsuite_pl_utils.c \
					           header_pooled_list.h

lib_t_pooled_list_la_LIBS    = @CUNITLIBS@							 
lib_t_pooled_list_la_LDFLAGS = @CUNITLDFLAGS@
lib_t_pooled_list_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif