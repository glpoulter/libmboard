INCLUDEDIR = $(top_srcdir)/include

if TESTS_CONFIGURED

noinst_LTLIBRARIES  = lib_t_objmap.la

lib_t_objmap_la_SOURCES = setup_suite_om.c \
						  test_om1.c \
						  testsuite_om_utils.c \
					      header_objmap.h
							 
lib_t_objmap_la_LDFLAGS = @COMMON_LDFLAGS@ @CUNITLIBS@
lib_t_objmap_la_CFLAGS  = -I$(INCLUDEDIR) @CUNITCFLAGS@ @NORMAL_CFLAGS@

else

all:
	@echo -e "\n\n** Building of tests was disabled during ./configure"
	
endif