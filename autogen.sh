#!/bin/bash

LOGFILE="autogen.log"
echo -e "$0 begins " `date` "\n" > $LOGFILE

function complainAndQuit() {
	echo "FAILED"
	echo "+ see $LOGFILE for details"
	exit 1
}

echo -n "Running libtoolize ... "
echo "(libtoolize)" >> $LOGFILE
libtoolize --copy --force >> $LOGFILE 2>> $LOGFILE
if test ! $? = 0; then complainAndQuit; fi
echo "DONE"

echo -n "Running aclocal    ... "
echo "(aclocal)" >> $LOGFILE
(aclocal -I m4) >> $LOGFILE 2>> $LOGFILE
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
