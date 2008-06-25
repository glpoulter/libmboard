INCLUDEDIR = $(top_srcdir)/include

noinst_LTLIBRARIES  = lib_mb_s.la lib_mb_sd.la

common_sources   = 	addmessage.c \
				    clear.c \
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
				    serial_utils.c \
				    synccomplete.c \
				    syncstart.c \
				    synctest.c \
                    $(INCLUDEDIR)/objmap.h \
                    $(INCLUDEDIR)/pooled_list.h \
                    $(INCLUDEDIR)/mboard.h \
                    $(INCLUDEDIR)/mb_utils.h \
                    $(INCLUDEDIR)/mb_common.h \
                    $(INCLUDEDIR)/mb_serial.h
                    
common_libadd    = @COMMON_LDFLAGS@
common_cflags    = -I$(INCLUDEDIR)

lib_mb_s_la_SOURCES   = $(common_sources)     
lib_mb_s_la_LIBADD    = $(common_libadd)
lib_mb_s_la_CFLAGS    = $(common_cflags) @PRODUCTION_CFLAGS@

lib_mb_sd_la_SOURCES  = $(common_sources)     
lib_mb_sd_la_LIBADD   = $(common_libadd)
lib_mb_sd_la_CFLAGS   = $(common_cflags) @DEBUG_CFLAGS@
