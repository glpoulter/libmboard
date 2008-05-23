# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : May 2008
#
#
# ----------- Module configuration ------------
#
# included by ../../Makefile

# Name of this module
MODULE_NAME = commroutines

# Modules this module depend on
MODULE_DEPENDS =  

# Additional header files used, apart from mb_$(MODULE_NAME).h
EXTRA_INC = commqueue.h syncqueue.h mb_parallel.h pooled_list.h objmap.h

# Load common include.mk to do the rest
include include.mk