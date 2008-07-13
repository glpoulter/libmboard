#!/bin/bash
# $Id$
# 
# Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
# Author: Lee-Shawn Chin 
# Date  : June 2008

LOGFILE="autogen.log"
AUTOMAKE_MINVER="1.8"
AUTOCONF_MINVER="2.59"

echo -e "$0 begins " `date` "\n" > $LOGFILE

function complainAndQuit() {
	echo "FAILED"
	echo "+ see $LOGFILE for details"
	exit 1
}

# check automake version
ver=`automake --version 2>/dev/null | sed -e '1s/[^0-9]*//' -e q`
if test x$ver = x; then ver="0.0"; fi
if test `expr "${ver} < ${AUTOMAKE_MINVER}"`; then
    echo "ERROR: automake (version >= $AUTOMAKE_MINVER) is required" >&2
    exit 1
fi 

# check autoconf version
ver=`autoconf --version 2>/dev/null | sed -e '1s/[^0-9]*//' -e q`
if test x$ver = x; then ver="0.0"; fi
if test `expr "${ver} < ${AUTOCONF_MINVER}"`; then
    echo "ERROR: autoconf (version >= $AUTOCONF_MINVER) is required" >&2
    exit 1
fi 

# check for autoheader
ver=`autoheader --version 2>/dev/null | sed -e '1s/[^0-9]*//' -e q`
if test x$ver = x; then 
    echo "ERROR: autoheader is required." >&2
    echo "       Plese install autoheader and try again." >&2
    exit 1
fi

# check for aclocal
ver=`aclocal --version 2>/dev/null | sed -e '1s/[^0-9]*//' -e q`
if test x$ver = x; then 
    echo "ERROR: aclocal is required." >&2
    echo "       Plese install aclocal and try again." >&2
    exit 1
fi

# check for libtoolize
ver=`libtoolize --version 2>/dev/null | sed -e '1s/[^0-9]*//' -e q`
if test x$ver = x; then 
    echo "ERROR: libtoolize is required." >&2
    echo "       Plese install libtool and try again." >&2
    exit 1
fi

echo -n "Running aclocal    ... "
echo "(aclocal)" >> $LOGFILE
(aclocal -I ../../m4) >> $LOGFILE 2>> $LOGFILE
if test ! $? = 0; then complainAndQuit; fi
echo "DONE"

echo -n "Running autoheader ... "
echo "(autoheader)" >> $LOGFILE
autoheader >> $LOGFILE 2>> $LOGFILE
if test ! $? = 0; then complainAndQuit; fi
echo "DONE"

echo -n "Running automake   ... "
echo "(automake)" >> $LOGFILE
automake --add-missing >> $LOGFILE 2>> $LOGFILE
if test ! $? = 0; then complainAndQuit; fi
echo "DONE"

echo -n "Running autoconf   ... "
echo "(autoconf)" >> $LOGFILE
autoconf >> $LOGFILE 2>> $LOGFILE
if test ! $? = 0; then complainAndQuit; fi
echo "DONE"

echo -n "Overwriting 'COPYING' file ... "
echo "(overwritng COPYING)" >> $LOGFILE
(rm COPYING; touch COPYING) >> $LOGFILE 2>> $LOGFILE
if test ! $? = 0; then complainAndQuit; fi
echo "DONE"

echo -e "\n\n$0 ends " `date` "\n" >> $LOGFILE
