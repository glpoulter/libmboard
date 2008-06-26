INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_commqueue.la

lib_t_commqueue_la_SOURCES = setup_suite_commqueue.c \
							 test_cq1.c \
							 header_commqueue.h
							 
lib_t_commqueue_la_LDFLAGS = @COMMON_LDFLAGS@ @CUNITLDFLAGS@
lib_t_commqueue_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MPICFLAGS@ @PARALLEL_EXTRA_CFLAGS@ @PRODUCTION_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif