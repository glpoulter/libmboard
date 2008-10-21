# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Sept 2008

INCLUDEDIR = $(top_srcdir)/include

bin_PROGRAMS = mboard-config
mboard_config_SOURCES = mboard-config.c

if COMPILE_SQLITE
bin_PROGRAMS = mboard-parse-memlogdb
mboard_parse_memlogdb_SOURCES = mboard-parse-memlogdb.c $(INCLUDEDIR)/sqlite3.h
mboard_parse_memlogdb_LDADD  = $(top_srcdir)/src/utils/lib_utils_sqlite3.la
endif