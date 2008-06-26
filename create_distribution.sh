#!/bin/bash

# Check that ./autogen.sh is executable
if test ! -x ./autogen.sh;
then
	echo "ERROR: ./autogen.sh is either unavailable or not executable" >&2
	exit 1
fi


function clean_dir {

	DIR_TO_CLEAN="$1"
	echo -e "\n\nCleaning up $DIR_TO_CLEAN directory before packaging: "
	echo "---------------------------------------------------------------- "
	
	(
		cd $DIR_TO_CLEAN
		if test -f Makefile;
		then
		
			make clean || (
				echo "ERROR: Automatic cleaning failed. " >&2
				echo "Please clean up $DIR_TO_CLEAN manually and try again" >&2
				exit 1
				)
			
			make distclean || (
				echo "ERROR: Automatic cleaning failed. " >&2
				echo "Please clean up $DIR_TO_CLEAN manually and try again" >&2
				exit 1
				)
		fi	
			
		rm INSTALL 2> /dev/null
		rm aclocal.m4 2> /dev/null
		rm acinclude.m4 2> /dev/null
		rm configure 2> /dev/null
		rm configure.scan 2> /dev/null
		rm config.* 2> /dev/null
		rm *.log 2> /dev/null
		rm *.xml 2> /dev/null
		rm -rf autom4te.cache 2> /dev/null
	)
	
	echo -e "-OK-\n"
}

# clean up ./example/circles_mb directory
clean_dir ./example/circles_mb

# Run autotools
echo "Running ./autogen.sh: "
echo "--------------------- "
./autogen.sh
if test $? != 0; then exit 1; fi

# Run ./configure
echo -e "\n\nRunning ./configure: "
./configure
if test $? != 0; then exit 1; fi

# make dist
echo -e "\n\nCreating distribution files: "
echo "---------------------------- "
make dist
if test $? != 0; then exit 1; fi

# make distcheck
echo -e "\n\nChecking distribution files: "
echo "---------------------------- "
make distcheck
if test $? != 0; then exit 1; fi

# make distclean
echo -e "\n\nDeleting artifacts: "
echo "------------------- "
make distclean
if test $? != 0; then exit 1; fi

echo " ----------------------------------------------------- "
echo ""
echo "Your distribution file is now ready."
