# -*- makefile -*-

#############################################################################
# Main Makefile for the project. It is the driver that sets up the full     #
# compilation process.                                                      #
#############################################################################

# In a real project, there would be some sort of licensing statement
# here.

all:

# Include the variables file that defines the source code structure.
include Make.src
include tests/Make.args

# This line prepends "build/" to all of the source and test
# subdirectory names.
DIRS = $(sort $(addprefix build/,$(SRC_SUBDIRS) tests $(TEST_SRCDIR) src/objects))

# Main entry point to the Makefile here. When you just type "make"
# on the command line, this is where you arrive. This will create
# the "build" directory, cd into it, then run "make" from there.
all check results: $(DIRS) build/Makefile
	cd build && $(MAKE) $@

%.result: $(DIRS) build/Makefile
	cd build && $(MAKE) $@

testsuite: $(DIRS) build/Makefile $(TEST_SUITES)
	cd build && $(MAKE) $@

# Called before cd'ing into the build directory. This will create
# all the subdirectories needed.
$(DIRS):
	mkdir -p $@

# Used to copy the Makefile.build into the build directory as the
# Makefile there. SUBTLE POINT: When we cd into the build directory
# and re-run "make" there, that copy is the Makefile used to
# compile everything.
build/Makefile: Makefile.build
	cp $< $@

clean:
	rm -rf build
