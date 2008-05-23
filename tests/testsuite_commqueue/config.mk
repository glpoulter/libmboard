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
MODULE_NAME = commqueue

# Modules this module depend on
MODULE_DEPENDS =  

# Additional header files used, apart from mb_$(MODULE_NAME).h
EXTRA_INC = commqueue.h mboard.h

# Load common include.mk to do the rest
include include.mk