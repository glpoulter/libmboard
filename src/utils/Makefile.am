# $Id: Makefile.am 2925 2012-07-20 14:12:17Z lsc $
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

noinst_LTLIBRARIES  = lib_utils.la lib_utils_d.la
if COMPILE_PARALLEL
noinst_LTLIBRARIES += lib_utils_p.la lib_utils_pd.la
endif

common_sources   = objmap.c pooled_list.c unmap.c tag_table.c \
                    kdtree.c avltree.c string_map.c settings.c \
                    banner.c \
                    $(INCLUDEDIR)/khash.h \
                    $(INCLUDEDIR)/mb_config.h \
                    $(INCLUDEDIR)/mb_objmap.h \
                    $(INCLUDEDIR)/mb_tag_table.h \
                    $(INCLUDEDIR)/mb_string_map.h \
                    $(INCLUDEDIR)/mb_pooled_list.h \
                    $(INCLUDEDIR)/mb_avltree.h \
                    $(INCLUDEDIR)/mb_kdtree.h \
                    $(INCLUDEDIR)/mboard.h \
                    $(INCLUDEDIR)/mb_utils.h \
                    $(INCLUDEDIR)/mb_banner.h \
                    $(INCLUDEDIR)/mb_settings.h \
                    $(INCLUDEDIR)/mb_common.h
common_cflags    = -I$(INCLUDEDIR)
common_libadd    = 

if COMPILE_MEMLOG
common_sources += memlog.c $(INCLUDEDIR)/mb_memlog.h 
endif

if COMPILE_SQLITE
noinst_LTLIBRARIES += lib_utils_sqlite3.la
common_libadd      += lib_utils_sqlite3.la

lib_utils_sqlite3_la_SOURCES = sqlite3.c $(INCLUDEDIR)/sqlite3.h
# manually handle thread safety so serial lib would not need pthread    
lib_utils_sqlite3_la_CFLAGS  = -DSQLITE_THREADSAFE=0 \
                               -DSQLITE_OMIT_LOAD_EXTENSION
endif

lib_utils_la_SOURCES   = $(common_sources)     
lib_utils_la_CFLAGS    = $(common_cflags) @PRODUCTION_CFLAGS@
lib_utils_la_LIBADD    = $(common_libadd)

lib_utils_d_la_SOURCES  = $(common_sources)     
lib_utils_d_la_CFLAGS   = $(common_cflags) @DEBUG_CFLAGS@
lib_utils_d_la_LIBADD   = $(common_libadd)

if COMPILE_PARALLEL
lib_utils_p_la_SOURCES  = $(common_sources)
lib_utils_p_la_LDFLAGS  = @MB_PARALLEL_LDFLAGS@
lib_utils_p_la_LIBS     = @MB_PARALLEL_LIBS@
lib_utils_p_la_CFLAGS   = $(common_cflags) @MB_PARALLEL_CFLAGS@ @PRODUCTION_CFLAGS@
lib_utils_p_la_LIBADD   = $(common_libadd)

lib_utils_pd_la_SOURCES = $(common_sources)  
lib_utils_pd_la_LDFLAGS = @MB_PARALLEL_LDFLAGS@
lib_utils_pd_la_LIBS    = @MB_PARALLEL_LIBS@
lib_utils_pd_la_CFLAGS  = $(common_cflags) @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@
lib_utils_pd_la_LIBADD  = $(common_libadd)
endif
