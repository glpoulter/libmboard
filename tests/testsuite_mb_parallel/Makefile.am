INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED
if COMPILE_PARALLEL
noinst_LTLIBRARIES  = lib_t_mb_parallel.la

lib_t_mb_parallel_la_SOURCES = setup_suite_mb_parallel.c \
							   test_mb_p1.c \
							   test_mb_p2.c \
							   test_mb_p3.c \
							   test_mb_p4.c \
							   test_mb_p5.c \
							   test_mb_p6.c \
							   test_mb_p7.c \
							   test_mb_p8.c \
							   test_mb_p9.c \
							   test_mb_p10.c \
							   test_mb_p11.c \
							   testsuite_mb_parallel_utils.c \
							   header_mb_parallel.h
							 
lib_t_mb_parallel_la_LDFLAGS = @COMMON_LDFLAGS@ @MPILIBS@ @CUNITLIBS@
lib_t_mb_parallel_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @MPICFLAGS@ @PARALLEL_EXTRA_CFLAGS@ @NORMAL_CFLAGS@
endif

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif