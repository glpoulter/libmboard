
# Define output programs
bin_PROGRAMS  = circles_serial
if COMPILE_PARALLEL
bin_PROGRAMS += circles_parallel
endif

common_sources = agent_functions.c \
				 agent_mgmt.c \
				 circles.h \
				 debug.c \
				 filters.c \
				 io.c \
				 low_primes.h \
				 main.c \
				 messages.c \
				 partition.c \
				 stats.c \
				 utils.c \
				 xmlparse.h


circles_serial_SOURCES = ${common_sources}
circles_serial_CFLAGS  = @NORMAL_CFLAGS@ @MBOARD_CFLAGS@
circles_serial_LDADD   = @MBOARD_S_LDFLAGS@ @MBOARD_S_LIBS@
# circles_serial_LDADD = @MBOARD_SD_LDFLAGS@ @MBOARD_SD_LIBS@

if COMPILE_PARALLEL
circles_parallel_SOURCES = ${common_sources}
circles_parallel_CFLAGS  = @MPICFLAGS@ @PARALLEL_EXTRA_CFLAGS@ @NORMAL_CFLAGS@ @MBOARD_CFLAGS@
circles_parallel_LDADD   = @MBOARD_P_LDFLAGS@ @MBOARD_P_LIBS@ @MPILIBS@
# circles_parallel_LDADD = @MBOARD_PD_LDFLAGS@ @MBOARD_PD_LIBS@ @MPILIBS@
endif