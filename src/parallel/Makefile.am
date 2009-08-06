# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

INCLUDEDIR = $(top_srcdir)/include

if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_mb_p.la lib_mb_pd.la
endif

common_sources   = 	addmessage.c \
				    clear.c \
				    comm.c \
				    commthread.c \
				    create.c \
				    delete.c \
				    env_finalise.c \
				    env_finalised.c \
				    env_init.c \
				    env_initialised.c \
				    filter_create.c \
				    filter_delete.c \
				    filter_assign.c \
				    function_assign.c \
				    function_free.c \
				    function_register.c \
				    iterator_create.c \
				    iterator_createfiltered.c \
				    iterator_createsorted.c \
				    iterator_createfilteredsorted.c \
				    iterator_delete.c \
				    iterator_getmessage.c \
				    iterator_randomise.c \
				    iterator_rewind.c \
				    indexmap_create.c \
				    indexmap_delete.c \
				    indexmap_sync.c \
				    indexmap_memberof.c \
				    indexmap_addentry.c \
				    synccomplete.c \
				    syncstart.c \
				    synctest.c \
				    util_commqueue.c \
				    util_syncqueue.c \
                    $(INCLUDEDIR)/mb_objmap.h \
                    $(INCLUDEDIR)/mb_commqueue.h \
                    $(INCLUDEDIR)/mb_syncqueue.h \
                    $(INCLUDEDIR)/mb_tag_table.h \
                    $(INCLUDEDIR)/mb_pooled_list.h \
                    $(INCLUDEDIR)/mb_avltree.h \
                    $(INCLUDEDIR)/mboard.h \
                    $(INCLUDEDIR)/mb_utils.h \
                    $(INCLUDEDIR)/mb_common.h \
                    $(INCLUDEDIR)/mb_settings.h \
                    $(INCLUDEDIR)/mb_parallel.h 
                    
common_cflags    = -I$(INCLUDEDIR)

if COMPILE_PARALLEL
lib_mb_p_la_SOURCES   = $(common_sources)   
lib_mb_p_la_LDFLAGS   = @MB_PARALLEL_LDFLAGS@
lib_mb_p_la_LIBADD    = @MB_PARALLEL_LIBS@
lib_mb_p_la_CFLAGS    = $(common_cflags) @MB_PARALLEL_CFLAGS@ @PRODUCTION_CFLAGS@

lib_mb_pd_la_SOURCES  = $(common_sources)     
lib_mb_pd_la_LDFLAGS  = @MB_PARALLEL_LDFLAGS@
lib_mb_pd_la_LIBADD   = @MB_PARALLEL_LIBS@
lib_mb_pd_la_CFLAGS   = $(common_cflags) @MB_PARALLEL_CFLAGS@ @DEBUG_CFLAGS@
endif