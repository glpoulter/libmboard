# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : April 2008
#
#
# ----------- Module configuration ------------
#
# included by ../../Makefile

# Name of this module
MODULE_NAME = mb_parallel

# Modules this module depend on
MODULE_DEPENDS = objmap pooled_list

# Additional header files used, apart from mb_$(MODULE_NAME).h
EXTRA_INC = objmap.h pooled_list.h syncqueue.h commqueue.h mb_common.h mboard.h

# Load common include.mk to do the rest
include include.mk