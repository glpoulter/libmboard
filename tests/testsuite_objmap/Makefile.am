# $Id: Makefile.am 1871 2009-06-12 13:58:18Z lsc $
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED

noinst_LTLIBRARIES  = lib_t_objmap.la

lib_t_objmap_la_SOURCES = setup_suite_om.c \
						  test_om_new.c \
						  test_om_push.c \
						  test_om_pop.c \
						  test_om_getobj.c \
						  testsuite_om_utils.c \
					      header_objmap.h
						
lib_t_objmap_la_LIBS    = @CUNITLIBS@	 
lib_t_objmap_la_LDFLAGS = @CUNITLDFLAGS@
lib_t_objmap_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @DEBUG_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif