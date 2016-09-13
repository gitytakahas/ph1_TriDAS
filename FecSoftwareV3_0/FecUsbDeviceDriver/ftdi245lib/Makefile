#FileName : 		Makefile
#Content :		Makefile for FTDI PIO 245BM access library
#Programmer : 		BIALAS Wojciech
#Version : 		0.2.1
#Support : 		mail to : Wojciech.Bialas@cern.ch


FECUSBDRIVER_DIR=..

#path to local dir ; gcc output will issue there
FTDILIB_DIR=.

#pathes to source and include files of the driver
SRC=$(FTDILIB_DIR)/src
INC=$(FTDILIB_DIR)/include
TEST=$(FTDILIB_DIR)/test

#Gives the current version tag for the device driver
#include $(INC)/make_versionning.h

#compiler & flags
# KH commented for distcc
#CC=gcc
CFLAGS := -Wall  -I$(INC) 
#Linker & flags
LD=ld


#ar & flags
AR=ar
ARFLAGS := -rv

#name of module once compiled, and subsequently
#name of the module once loaded in memory
#if modified here, must be modified in
# - the file ./include/defines.h (search for DD_MODULE)
# - the ./install script (search for MODULE)

all: libftdipio_access

tests: test2select

libftdipio_access: 	$(FTDILIB_DIR)/libftdipio_access.a

$(FTDILIB_DIR)/libftdipio_access.a: ftdipio_access.o 
	ar -rv  $(FTDILIB_DIR)/libftdipio_access.a ftdipio_access.o
	ranlib libftdipio_access.a

ftdipio_access.o: $(SRC)/ftdipio_access.c $(INC)/ftdipio_access.h
	$(CC) -g -o ftdipio_access.o -c $(CFLAGS) $(SRC)/ftdipio_access.c
# KH commented for distcc
#	gcc -g -o ftdipio_access.o -c $(CFLAGS) $(SRC)/ftdipio_access.c

clean: 
	rm -f *.a *.o core.* test2select

install:
	( if  -d $(FECUSBDRIVER_DIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) ; \
	cp -p libftdipio_access.a $(FECUSBDRIVER_DIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) ; \
	fi ; )


