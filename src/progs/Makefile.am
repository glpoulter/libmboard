# $Id$
# 
# Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : May 2009

INCLUDEDIR = $(top_srcdir)/include

dist_bin_SCRIPTS = mboard-config \
                   mboard-check-minversion

if COMPILE_MEMLOG
bin_PROGRAMS = mboard-parse-memlogdb
mboard_parse_memlogdb_SOURCES = mboard-parse-memlogdb.c $(INCLUDEDIR)/sqlite3.h
mboard_parse_memlogdb_LDADD  = $(top_srcdir)/src/utils/lib_utils_sqlite3.la
endif