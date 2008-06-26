INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED

noinst_LTLIBRARIES  = lib_t_mb_serial.la

lib_t_mb_serial_la_SOURCES = setup_suite_mb_serial.c \
							   test_mb_s1.c \
							   test_mb_s2.c \
							   test_mb_s3.c \
							   test_mb_s4.c \
							   test_mb_s5.c \
							   test_mb_s6.c \
							   test_mb_s7.c \
							   test_mb_s8.c \
							   test_mb_s9.c \
							   test_mb_s10.c \
							   test_mb_s12.c \
							   testsuite_mb_serial_utils.c \
							   header_mb_serial.h
							 
lib_t_mb_serial_la_LDFLAGS = @COMMON_LDFLAGS@ @CUNITLDFLAGS@
lib_t_mb_serial_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @PRODUCTION_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif