# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Feb 2008
#
#
# Note:
#  - This is an attempt at using non-recursive make
#  - Requires GNU Make
#  - Ought to accomodate a better use of -j and -k options , although this project
#    may not be large enough to warrant them

# ========= User Config ====================

SERIAL_CC    = gcc
PARALLEL_CC  = mpicc
BASE_CFLAGS  = -Wall -pedantic -O3 -DNDEBUG -fno-common
DEBUG_CFLAGS = -g -Wall -pedantic -fno-common -D_EXTRA_CHECKS
# MPICH uses 'long long' which ISO C90 does not support
PARALLEL_EXTRA_CFLAGS = -std=c9x

# ========= Project Config =================

# Directory to write output libraries
LIB_OUTPUT_DIR = ./lib

# Directory where all header files are stored
INCLUDE_DIR = ./include

# list modules in this project
MODULES = serial parallel utils

OUTPUTS = libmboard_s.a libmboard_p.a libmboard_sd.a libmboard_pd.a

# Options for splint
# -D__sigset_t=int hides problems with splint trying to parse headers
# used by pthread.h
SPLINT_OPTIONS = -weak -D__sigset_t=int

# MPI Include dir (used to provide splint with include dir)
MPI_DIR = /usr/local/mpich2
MPI_INCLUDE_DIR = $(MPI_DIR)/include

# --------------------------------------------

# Allow users to append params during make
APPEND = -DNONE

# For calculating dependencies on common makefiles
MAKEFILES = Makefile include.mk

# Default target should build both serial and parallel libraries
all: serial parallel serial-debug parallel-debug

# target names not associated to actual files
.PHONY: all clean serial parallel serial-debug parallel-debug test doc

# prepend paths to $(OUTPUTS)
OUTPUTS_FULL = $(patsubst %,$(LIB_OUTPUT_DIR)/%,$(OUTPUTS))

# config.mk from each module will append to these vars
SRC := 

# Include config from each module
include $(patsubst %,src/%/config.mk,$(MODULES))

# To allow for a different DEBUG obj file for the same source,
# we define a new suffix .do
.SUFFIXES: .do
.c.do:
	${CC} ${CFLAGS} -c $< -o $@
	
# All object files
OBJ := $(SRC:.c=.o) $(SRC:.c=.do)

# ----------- Custom Targets ----------

# serial library
serial: CC = $(SERIAL_CC)
serial: CFLAGS = $(APPEND) $(BASE_CFLAGS) -I$(INCLUDE_DIR)
serial: $(LIB_OUTPUT_DIR)/libmboard_s.a

# serial-debug library
serial-debug: CC = $(SERIAL_CC)
serial-debug: CFLAGS = $(APPEND) $(DEBUG_CFLAGS) -I$(INCLUDE_DIR)
serial-debug: $(LIB_OUTPUT_DIR)/libmboard_sd.a

# parallel library
parallel: CC = $(PARALLEL_CC)
parallel: CFLAGS = $(APPEND) $(BASE_CFLAGS) $(PARALLEL_EXTRA_CFLAGS) -I$(INCLUDE_DIR)
parallel: $(LIB_OUTPUT_DIR)/libmboard_p.a

# parallel library
parallel-debug: CC = $(PARALLEL_CC)
parallel-debug: CFLAGS = $(APPEND) $(DEBUG_CFLAGS) $(PARALLEL_EXTRA_CFLAGS) -I$(INCLUDE_DIR)
parallel-debug: $(LIB_OUTPUT_DIR)/libmboard_pd.a

clean:
	rm -f $(OUTPUTS_FULL) $(OBJ)
	
test:
	(cd tests && make vclean all)
	
splint:
	@echo "Performing static analysis of serial implementation code ... "
	@echo "-------------------------------------------------------------"
	splint $(SPLINT_OPTIONS) -I$(INCLUDE_DIR) src/utils/*.c src/serial/*.c
	@echo ""
	@echo "Performing static analysis of parallel implementation code ... "
	@echo "---------------------------------------------------------------"
	splint $(SPLINT_OPTIONS) -I$(INCLUDE_DIR) -I$(MPI_INCLUDE_DIR) src/utils/*.c src/parallel/*.c
	
doc: Doxyfile_user Doxyfile_developer_serial Doxyfile_developer_parallel
	@mkdir -p doc/developer &> /dev/null
	@echo "Generating User documentation in ./doc/user"
	@doxygen Doxyfile_user
	@echo "Generating Developer documentation (serial code) in ./doc/developer/serial"
	@doxygen Doxyfile_developer_serial
	@echo "Generating Developer documentation (parallel code) in ./doc/developer/parallel"
	@doxygen Doxyfile_developer_parallel
	
$(LIB_OUTPUT_DIR)/libmboard_s.a: $(serial_OBJ) $(utils_OBJ)
	ar rcs $@ $(serial_OBJ) $(utils_OBJ)
	@echo -e "\n++ Serial MBoard Library written to: $@\n"
	
$(LIB_OUTPUT_DIR)/libmboard_sd.a: $(serial_DEBUG_OBJ) $(utils_DEBUG_OBJ)
	ar rcs $@ $(serial_OBJ) $(utils_OBJ)
	@echo -e "\n++ Serial Debug MBoard Library written to: $@\n"
	
$(LIB_OUTPUT_DIR)/libmboard_p.a: $(parallel_OBJ) $(utils_OBJ)
	ar rcs $@ $(parallel_OBJ) $(utils_OBJ)
	@echo -e "\n++ Parallel MBoard Library written to: $@\n"

$(LIB_OUTPUT_DIR)/libmboard_pd.a: $(parallel_DEBUG_OBJ) $(utils_DEBUG_OBJ)
	ar rcs $@ $(parallel_OBJ) $(utils_OBJ)
	@echo -e "\n++ Parallel Debug MBoard Library written to: $@\n"