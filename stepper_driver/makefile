#This is a minimal make file.
#anything that starts with a # is a comment
#
#it should be easy to maintain.
#to generate the dependancies automatically
#run "make depend"
#
#
#To clean up the directory
#run "make clean"
#your responsibilities as a programmer:
#
# Run make depend whenever:
#	You add files to the project
#	You change what files are included in a source file
#
# make clean whenever you change this makefile.
#

#Setup the porject root name
#This will built NAME.x and save it as $(NBROOT)/bin/NAME.x
NAME	= stepper_driver
PLATFORM= MOD5441X

CXXSRCS := \
	main.cpp \
	stepper.cpp

#Uncomment and modify these lines if you have C or S files.
#CSRCS := foo.c
#ASRCS := foo.s

#CXXSRCS += htmldata.cpp
#CREATEDTARGS := htmldata.cpp

#include the file that does all of the automagic work!
include $(NBROOT)/make/main.mak


#htmldata.cpp : $(wildcard html/*.*)
#    comphtml html -ohtmldata.cpp

