#FileName : 		Makefile
#Content :		Makefile for FTDI 245BM device driver
#Programmer : 		BIALAS Wojciech
#Version : 		0.2.1
#Support : 		mail to : Wojciech.Bialas@cern.ch

FECUSBDRIVER_DIR=..

#path to local dir ; gcc output will issue there
FTDI_DIR=.

#pathes to source and include files of the driver
SRC=$(FTDI_DIR)/src
INC=$(FTDI_DIR)/include
TEST=$(FTDI_DIR)/test

#Gives the current version tag for the device driver
#include $(INC)/make_versionning.h

#compiler & flags
# KH commented for distcc
#CC=gcc
#compiler flags for module
DD_CFLAGS := -Wall -DCONFIG_USB_DEBUG -DMODULE -D__KERNEL__  -I$(INC) -I/usr/src/linux-`uname -r`/include -I/lib/modules/`uname -r`/build/include 
CFLAGS := -Wall  -I$(INC) 
#Linker & flags
LD=ld
LDFLAGS := -m elf_i386

#ar & flags
AR=ar
ARFLAGS := -rv

#name of module once compiled, and subsequently
#name of the module once loaded in memory
#if modified here, must be modified in
# - the file ./include/defines.h (search for DD_MODULE)
# - the ./install script (search for MODULE)
MODULE=ftdipio

all: ftdipio_driver ftdipio_lib

tests: test2select

ftdipio_driver:         $(FTDI_DIR)/$(MODULE).o

ftdipio_lib: 	$(FTDI_DIR)/libftdipio.a

$(FTDI_DIR)/libftdipio.a: ftdipio_func.o 
	ar -rv  libftdipio.a ftdipio_func.o
	ranlib libftdipio.a

ftdipio_func.o: $(SRC)/ftdipio_func.c $(INC)/ftdipio_func.h
	$(CC) -g -o ftdipio_func.o -c $(CFLAGS) $(SRC)/ftdipio_func.c
# KH commented for distcc
#	gcc -g -o ftdipio_func.o -c $(CFLAGS) $(SRC)/ftdipio_func.c

$(FTDI_DIR)/$(MODULE).o:       $(SRC)/ftdipio.c \
                $(INC)/version.h
	$(CC) $(DD_CFLAGS) -c -o $(FTDI_DIR)/$(MODULE).o $(SRC)/ftdipio.c

clean: 
	rm -f *.a *.o core.* test2select

install:
	( if  -d $(FECUSBDRIVER_DIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) ; \
	cp -p libftdipio.a $(FECUSBDRIVER_DIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) ; \
	fi ; )

test2select: $(TEST)/test2select.c
	$(CC) $(CFLAGS) -o test2select $(TEST)/test2select.c -L$(FTDI_DIR) -lftdipio
