# $Id: Makefile.am 991 2008-06-27 10:41:47Z lsc $
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

# Define output programs
bin_PROGRAMS  = circles_serial circles_serial_debug
if COMPILE_PARALLEL
bin_PROGRAMS += circles_parallel circles_parallel_debug
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
circles_serial_CFLAGS  = @MBOARD_S_CFLAGS@
circles_serial_LDFLAGS = @MBOARD_S_LDFLAGS@
circles_serial_LDADD   = @MBOARD_S_LIBS@

circles_serial_debug_SOURCES = ${common_sources}
circles_serial_debug_CFLAGS  = @MBOARD_SD_CFLAGS@
circles_serial_debug_LDFLAGS = @MBOARD_SD_LDFLAGS@
circles_serial_debug_LDADD   = @MBOARD_SD_LIBS@

if COMPILE_PARALLEL
circles_parallel_SOURCES = ${common_sources}
circles_parallel_CFLAGS  = @MBOARD_P_CFLAGS@ @PARALLEL_EXTRA_CFLAGS@
circles_parallel_LDFLAGS = @MBOARD_P_LDFLAGS@
circles_parallel_LDADD   = @MBOARD_P_LIBS@

circles_parallel_debug_SOURCES = ${common_sources}
circles_parallel_debug_CFLAGS  = @MBOARD_PD_CFLAGS@ @PARALLEL_EXTRA_CFLAGS@
circles_parallel_debug_LDFLAGS = @MBOARD_PD_LDFLAGS@
circles_parallel_debug_LDADD   = @MBOARD_PD_LIBS@
endif