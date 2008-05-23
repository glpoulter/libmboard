# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Feb 2008
#

CC_SERIAL   = gcc
CC_PARALLEL = mpicc
CFLAGS      = -Wall -g -pedantic
LDFLAGS     = -lm

# MPICH path passed on to splint
MPI_INCLUDE_PATH = /usr/local/mpich2/include

# ========= Project Config =================

# Directory where libmboard library is written
MB_LIB_DIR = ../lib

# Directory where libmboard header files are stored
MB_INCLUDE_DIR = ../include

# libmboard source directory
MB_SRC_DIR     = ../

# path to generated libmboard static library
ifeq ($(findstring withdebug, $(MAKECMDGOALS)), withdebug)
MB_LIB_SERIAL    = $(MB_LIB_DIR)/libmboard_sd.a
MB_LIB_PARALLEL  = $(MB_LIB_DIR)/libmboard_pd.a
else
MB_LIB_SERIAL    = $(MB_LIB_DIR)/libmboard_s.a
MB_LIB_PARALLEL  = $(MB_LIB_DIR)/libmboard_p.a
endif
MB_LIBS          = $(MB_LIB_SERIAL) $(MB_LIB_PARALLEL)

# Cunit installation data
CUNIT_SRC_DIR     = ./CUnit-2.1-0
CUNIT_DIR         = ./cunit
CUNIT_INCLUDE_DIR = $(CUNIT_DIR)/include
CUNIT_LIB_DIR     = $(CUNIT_DIR)/lib
CUNIT_LIB_TARGET  = $(CUNIT_LIB_DIR)/libcunit.a

## list of testsuites used in different tests
# testing utilities common to both serial and parallel lib
MODULES_UTIL          = pooled_list objmap 
# testing serial lib
MODULES_SERIAL        = mb_serial
# testing parallel utils
MODULES_PARALLEL_UTIL = commqueue syncqueue commroutines
# testing parallel lib
MODULES_PARALLEL      = mb_parallel

MODULES = $(MODULES_UTIL) $(MODULES_SERIAL) $(MODULES_PARALLEL_UTIL) $(MODULES_PARALLEL)

# test executables
EXE_UTIL          = run_test_utils
EXE_SERIAL        = run_test_serial
EXE_PARALLEL_UTIL = run_test_parallel_util
EXE_PARALLEL      = run_test_parallel

# provide option for users who do not have MPI installed
ifeq ($(findstring serial, $(MAKECMDGOALS)), serial)
CC   = $(CC_SERIAL)
EXES = $(EXE_UTIL) $(EXE_SERIAL) 
else
CC = $(CC_PARALLEL)
# MPICH uses 'long long' which ISO C90 does not support
CFLAGS += -std=c9x  
EXES   = $(EXE_UTIL) $(EXE_SERIAL) $(EXE_PARALLEL_UTIL) $(EXE_PARALLEL)
endif

# Options for splint
# -D__sigset_t=int hides problems with splint trying to parse headers
# used by pthread.h
SPLINT_OPTIONS = -weak -I$(MPI_INCLUDE_PATH) -D__sigset_t=int -warnposix

# --------------------------------------------



# For calculating dependencies on common makefiles
MAKEFILES = Makefile include.mk

# target names not associated to actual files
.PHONY: all clean vclean build run serial

# Default target should build and run tests
all: build run
withdebug: all

# all source refer to header files in ./include
CFLAGS += -I$(MB_INCLUDE_DIR) -I$(CUNIT_INCLUDE_DIR) 

# config.mk from each module will append to this var
SRC := $(EXES:=.c)

# Include config from each module
include $(patsubst %,testsuite_%/config.mk,$(MODULES))

# All object files
OBJ := $(SRC:.c=.o)

# object files we need to build each exe
OBJ_UTIL   := $(EXE_UTIL).o $(foreach module,$(MODULES_UTIL),$($(module)_OBJ)) $(CUNIT_LIB_TARGET) $(MB_LIB_SERIAL) 
OBJ_SERIAL := $(EXE_SERIAL).o $(foreach module,$(MODULES_SERIAL),$($(module)_OBJ)) $(CUNIT_LIB_TARGET) $(MB_LIB_SERIAL) 
OBJ_PARALLEL_UTIL := $(EXE_PARALLEL_UTIL).o $(foreach module,$(MODULES_PARALLEL_UTIL),$($(module)_OBJ)) $(CUNIT_LIB_TARGET) $(MB_LIB_PARALLEL) 
OBJ_PARALLEL := $(EXE_PARALLEL).o $(foreach module,$(MODULES_PARALLEL),$($(module)_OBJ)) $(CUNIT_LIB_TARGET) $(MB_LIB_PARALLEL) 

# ----------- Custom Targets ----------
	
clean:
	rm -f $(EXES) $(OBJ)
	
vclean:
	(cd $(MB_SRC_DIR) && $(MAKE) clean)
	rm -f $(EXES) $(OBJ)
	
splint:
	@echo -e "\n----- ( Checking *.c ) -----\n"
	@splint $(SPLINT_OPTIONS) -I$(MB_INCLUDE_DIR) -I$(CUNIT_INCLUDE_DIR) *.c
	@for TS in $(MODULES); do \
	  echo -e "\n----- ( Checking testsuite_$${TS}/*.c ) -----\n"; \
	  splint $(SPLINT_OPTIONS) -I$(MB_INCLUDE_DIR) -I$(CUNIT_INCLUDE_DIR) \
	       testsuite_$${TS}/*.c; \
	 done

build: $(EXES)

$(EXE_UTIL): $(OBJ_UTIL)
	@echo -e "\n----- Linking Test Executable: $@ -----\n"
	$(CC) -o $@ $(OBJ_UTIL) $(LDFLAGS)

$(EXE_SERIAL): $(OBJ_SERIAL)
	@echo -e "\n----- Linking Test Executable: $@ -----\n"
	$(CC) -o $@ $(OBJ_SERIAL) $(LDFLAGS)
	
$(EXE_PARALLEL_UTIL): $(OBJ_PARALLEL_UTIL)
	@echo -e "\n----- Linking Test Executable: $@ -----\n"
	$(CC) -o $@ $(OBJ_PARALLEL_UTIL) $(LDFLAGS)
	
$(EXE_PARALLEL): $(OBJ_PARALLEL)
	@echo -e "\n----- Linking Test Executable: $@ -----\n"
	$(CC) -o $@ $(OBJ_PARALLEL) $(LDFLAGS)
	
$(CUNIT_LIB_TARGET):
	@echo -e "\n----- Building CUnit library -----"
	(cd $(CUNIT_SRC_DIR) && ./configure --prefix=`pwd`/../$(CUNIT_DIR) && \
	$(MAKE) all install)

$(MB_LIBS):
	@echo -e "\n----- Building MessageBoard library -----"
	(cd $(MB_SRC_DIR) && $(MAKE) clean all)

serial: all

run: $(EXES)
	@ # run all EXEs
	@ $(patsubst %, A=./%; if [ -x $${A} ]; then $${A}; fi &&, $(EXES)) true 
	@echo -e "\n\n===== ALL TESTS COMPLETED SUCCESSFULLY =====\n"
