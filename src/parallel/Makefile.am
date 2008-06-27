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
				    synccomplete.c \
				    syncstart.c \
				    synctest.c \
				    util_commqueue.c \
				    util_syncqueue.c \
                    $(INCLUDEDIR)/objmap.h \
                    $(INCLUDEDIR)/commqueue.h \
                    $(INCLUDEDIR)/syncqueue.h \
                    $(INCLUDEDIR)/pooled_list.h \
                    $(INCLUDEDIR)/mboard.h \
                    $(INCLUDEDIR)/mb_utils.h \
                    $(INCLUDEDIR)/mb_common.h \
                    $(INCLUDEDIR)/mb_parallel.h 
                    
common_libadd    = @COMMON_LDFLAGS@
common_cflags    = -I$(INCLUDEDIR)

if COMPILE_PARALLEL
lib_mb_p_la_SOURCES   = $(common_sources)   
lib_mb_p_la_LDFLAGS   = @THREADS_LDFLAGS@ @MPILDFLAGS@
lib_mb_p_la_LIBADD    = $(common_libadd) @THREADS_LIBS@ @MPILIBS@
lib_mb_p_la_CFLAGS    = $(common_cflags) @THREADS_CFLAGS@ @MPICFLAGS@ @PARALLEL_EXTRA_CFLAGS@ @PRODUCTION_CFLAGS@

lib_mb_pd_la_SOURCES  = $(common_sources)     
lib_mb_pd_la_LDFLAGS   = @THREADS_LDFLAGS@ @MPILDFLAGS@
lib_mb_pd_la_LIBADD   = $(common_libadd) @THREADS_LIBS@ @MPILIBS@
lib_mb_pd_la_CFLAGS   = $(common_cflags) @THREADS_CFLAGS@ @MPICFLAGS@ @PARALLEL_EXTRA_CFLAGS@ @DEBUG_CFLAGS@
endif