INCLUDEDIR = $(top_srcdir)/include

noinst_LTLIBRARIES  = lib_utils.la lib_utils_d.la
#if COMPILE_PARALLEL
#noinst_LTLIBRARIES += lib_utils_p.la lib_utils_pd.la
#endif

common_sources   = objmap.c pooled_list.c unmap.c \
                    $(INCLUDEDIR)/uthash.h \
                    $(INCLUDEDIR)/objmap.h \
                    $(INCLUDEDIR)/pooled_list.h \
                    $(INCLUDEDIR)/mboard.h \
                    $(INCLUDEDIR)/mb_utils.h \
                    $(INCLUDEDIR)/mb_common.h
common_libadd    = -lm
common_cflags    = -I$(INCLUDEDIR)

lib_utils_la_SOURCES   = $(common_sources)     
lib_utils_la_LIBADD    = $(common_libadd)
lib_utils_la_CFLAGS    = $(common_cflags) @PRODUCTION_CFLAGS@

lib_utils_d_la_SOURCES  = $(common_sources)     
lib_utils_d_la_LIBADD   = $(common_libadd)
lib_utils_d_la_CFLAGS   = $(common_cflags) @DEBUG_CFLAGS@
