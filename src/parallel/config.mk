# Auth: L.S.Chin@rl.ac.uk (STFC Rutherford Appleton laboratory)
# Date: Nov 2007
#
# ----------- Module configuration ------------
#
# included by ../../Makefile

# Name of this module
MODULE_NAME = parallel

# Modules this module depend on
MODULE_DEPENDS = utils 

# Additional header files used, apart from mb_$(MODULE_NAME).h
EXTRA_INC = mb_utils.h mboard.h mb_common.h commqueue.h syncqueue.h

# Load common include.mk to do the rest
include include.mk