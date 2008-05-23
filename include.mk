# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Feb 2008
#
#
# This file is to be included by $(MODULE)/config.mk
#
# We expect the following vars to be defined by the calling config.mk
#  MODULE_NAME     : Name of module
#  MODULE_DEPENDS  : Modules that this module depend on
#  EXTRA_INC       : Required header files, apart from mb_$(MODULE_NAME).h

# expand config values to actual vars
MY_PATH := src/$(MODULE_NAME)
MY_SRC  := $(wildcard $(MY_PATH)/*.c)
MY_INC  := mb_$(MODULE_NAME).h $(EXTRA_INC)

# Update main Makefile with module requirements
SRC += $(MY_SRC)
INC += $(patsubst %,$(INCLUDE_DIR)/%,$(MY_INC))

# Define module-based object files
$(MODULE_NAME)_OBJ := $(MY_SRC:.c=.o)
$(MODULE_NAME)_DEBUG_OBJ := $(MY_SRC:.c=.do)

# Define dependencies (header and makefiles) for this module. This includes:
# * Line 1: requred header files
# * Line 2: all objects from modules we depend on
# * Line 3: main Makefiles and those included by the build
$($(MODULE_NAME)_OBJ): $(patsubst %,$(INCLUDE_DIR)/%,$(MY_INC))  \
					   	$(patsubst %,$($(%)_OBJ),$(MODULE_DEPENDS)) \
					   	$(MAKEFILES) src/$(MODULE_NAME)/config.mk