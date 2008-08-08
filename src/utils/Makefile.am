# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

noinst_LTLIBRARIES  = lib_utils.la lib_utils_d.la
if COMPILE_PARALLEL
noinst_LTLIBRARIES += lib_utils_p.la lib_utils_pd.la
endif

common_sources   = objmap.c pooled_list.c unmap.c memlog.c tag_table.c \
                    $(INCLUDEDIR)/uthash.h \
                    $(INCLUDEDIR)/mb_config.h \
                    $(INCLUDEDIR)/mb_objmap.h \
                    $(INCLUDEDIR)/mb_tag_table.h \
                    $(INCLUDEDIR)/mb_pooled_list.h \
                    $(INCLUDEDIR)/mboard.h \
                    $(INCLUDEDIR)/mb_utils.h \
                    $(INCLUDEDIR)/mb_memlog.h \
                    $(INCLUDEDIR)/mb_common.h
common_cflags    = -I$(INCLUDEDIR)

lib_utils_la_SOURCES   = $(common_sources)     
lib_utils_la_CFLAGS    = $(common_cflags) @PRODUCTION_CFLAGS@

lib_utils_d_la_SOURCES  = $(common_sources)     
lib_utils_d_la_CFLAGS   = $(common_cflags) @DEBUG_CFLAGS@

if COMPILE_PARALLEL
lib_utils_p_la_SOURCES  = $(common_sources)
lib_utils_p_la_LDFLAGS  = @MB_PARALLEL_LDFLAGS@
lib_utils_p_la_LIBS     = @MB_PARALLEL_LIBS@
lib_utils_p_la_CFLAGS   = $(common_cflags) @MB_PARALLEL_CFLAGS@ @PRODUCTION_CFLAGS@

lib_utils_pd_la_SOURCES = $(common_sources)  
lib_utils_pd_la_LDFLAGS = @MB_PARALLEL_LDFLAGS@
lib_utils_pd_la_LIBS    = @MB_PARALLEL_LIBS@
lib_utils_pd_la_CFLAGS  = $(common_cflags) @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@
endif