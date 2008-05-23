# $Id:$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : Feb 2008
#
# This file is to be included by $(MODULE)/config.mk
#
# We expect the following vars to be defined by the calling config.mk
#  MODULE_NAME     : Name of module
#  MODULE_DEPENDS  : Modules that this module depend on
#  EXTRA_INC       : Required header files, apart from mb_$(MODULE_NAME).h

# expand config values to actual vars
MY_PATH := testsuite_$(MODULE_NAME)
MY_SRC  := $(wildcard $(MY_PATH)/*.c)
MY_INC  := $(EXTRA_INC)

# Update main Makefile with module requirements
SRC += $(MY_SRC)
#INC += $(patsubst %,$(MB_INCLUDE_DIR)/%,$(MY_INC))

# Define module-based object files
$(MODULE_NAME)_OBJ := $(MY_SRC:.c=.o)

# Define dependencies (header and makefiles) for this module. This includes:
# * Line 1: required external header files 
# * Line 2: local header file
# * Line 3: all objects from modules we depend on
# * Line 4: main Makefiles and those included by the build
$($(MODULE_NAME)_OBJ): $(patsubst %,$(MB_INCLUDE_DIR)/%,$(MY_INC)) \
						$(MY_PATH)/header_$(MODULE_NAME).h \
					   	$(patsubst %,$($(%)_OBJ),$(MODULE_DEPENDS)) \
					   	$(MAKEFILES) $(MY_PATH)/config.mk