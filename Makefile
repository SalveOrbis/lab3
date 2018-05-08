
# We could simply compile with "gcc *.c -lgmp", but this is an
# instructional Makefile that executes each step separately when
# building an executable file to allow inspection of each step.
#
# In a real project there are shortcuts and more powerful rules that
# can be used. For large projects Autotools can be used to allow
# proper installation as well. In particular, it is rare to use the
# preprocessor cpp or the linker ld explicitly.
#
# The test suite assumes that GMP <https://www.gmplib.org> has been
# installed, but this is mostly the case on Linux distributions and it
# is not needed or allowed to be used to solve the problems.
#
# Author: Douglas Wikstrom <dog@kth.se>

# ###################################################################
# At the top of a Makefile various variables are defined that are
# later used when compiling the software. Preferably this should be
# the only place where one needs to modify the Makefile when smaller
# changes in the software takes place.
# ###################################################################

# These are all the C header files that declares types and functions
# exposed outside modules.
HEADERS = bigintmacros.h biginthelp.h bigint.h bigintimplement.h

# These are the C source files. They contain the implementations of
# the declared functions.
FILES = bi.c biginthelp.c bigintimplement.c

# These are the corresponding object files that we need to build.
O_FILES = $(shell echo $(FILES) | sed "s/\.c/\.pre\.o/g")

# Here sed is a useful utility that tranforms text using rules. Here
# we have piped the contents of the variable FILES into sed which is
# instructed to substitute each occurrence of a string ending with
# ".c" by a string ending with ".pre.o" instead. This is executed by
# the shell and the result stored in O_FILES.

# ###################################################################
# A target is something that must be built by processing other
# files. There are also phony targets and a few built-in targets which
# simply triggers a command to be executed without building anything.
#
# Each target may have dependencies that must be built before the
# target is built. A dependency is simply something we need as a
# building block to put together a program. This gives a directed
# acyclical graph of targets that can be built from the bottom up.
#
# Whenever a dependency changes the target that uses it must be built
# again. This means that we do not have to worry that targets are not
# up to date.
#
# There are many automatic variables that are defined in a neat
# way. Below we use: the name of the target $@, the root of a rule $<,
# and the list of all dependencies $^.
# ###################################################################

# The built-in "all" target is special in that it is built if one
# simply executes "make". Here we state that it depends on bi, so bi
# must be built first. Thus, we allow building bi by simply executing
# "make".
all: bi

# Rule for compiling a pre-processed file from the corresponding
# source file in C using the C Pre-processor (cpp). (Not to be
# confused with cpp as a postfix used for C++ files.)
#
# Note that % is a regexp that is replaced on both sides of ":", e.g.,
# it is as if we write down a rule for any file <filename>.c.
#
# This replaces all #define, #ifdef, #include, etc directives by C
# code. It can be inspected, but is not human friendly, since it is
# generated, e.g., it may be formatted in a weird way and contain many
# blank lines.
#
# To keep things simple below we include all header files as
# dependencies.
%.pre.c : %.c $(HEADERS)
	cpp -o $@ $<

# Rule for compiling an object file (with a .o postfix) from a
# corresponding pre-processed source file in C. We use a different
# suffix to emphasize that the results have been pre-processed, but
# pre-processed files are perfectly valid C files.
#
# The -c flag means that we compile the the file separately without
# linking. Thus, it only needs the declarations of the functions from
# other files it uses and not their definitions.
%.pre.o : %.pre.c
	cc -c -o $@ $<

# Rule for linking object files into an executable file
# bi. Conceptually, this merely concatenates the object files and
# replaces symbolic addresses by explicit values.
#
# We could use ld directly here instead of indirectly through gcc, but
# gcc figures out where the standard libraries we need are located, so
# this is easier.
bi : $(O_FILES)
	gcc -o $@ $^ -lgmp -lgcc

# Rule for running only the self-tests stated in the file
# testcases.txt. This pipes the contents of the file to stdin of the
# program bi.
out: bi
	./bi < testcases.txt

# Standard target for running the complete self-test suite of the
# software.
check: bi
	./bi -v -a

# Use this when testing your software. It only executes the test cases
# in the testcases.txt file. Note that it rebuilds your executable bi
# if needed, so during development you only need to use either "make"
# (only building), or "make chf" (build and run the tests you want).
chf: bi
	./bi -v < testcases.txt

# Standard target for deleting all files produced during compilation.
clean:
	rm -rf student* *.o *.cp bi *~

# ###################################################################
# These rules are used to provide a student package with stripped down
# biginthelp.c and bigintimplement.c files. It cannot be used by
# students, since files are missing in the package you have.
# ###################################################################

student : student.tar.gz
student.tar.gz : bi
	mkdir -p student
	cp *.c *.h Makefile testcases.txt student/
	rm -f student/*.pre.c student/biginthelp.c student/bigintimplement.c
	./strip biginthelp
	./strip bigintimplement
	tar cvfz student.tar.gz student
