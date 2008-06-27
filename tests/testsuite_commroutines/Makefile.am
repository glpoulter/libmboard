INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_commroutines.la

lib_t_commroutines_la_SOURCES = setup_suite_commroutines.c \
							    test_cr1.c \
							    header_commroutines.h
							 
lib_t_commroutines_la_LDFLAGS = @COMMON_LDFLAGS@ @CUNITLDFLAGS@
lib_t_commroutines_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MPICFLAGS@ @PARALLEL_EXTRA_CFLAGS@ @DEBUG_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif