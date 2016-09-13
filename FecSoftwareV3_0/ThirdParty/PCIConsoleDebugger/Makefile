#FileName : 		Makefile
#Content : 		Makefile for PCI FEC console debugger
#Programmer : 		Laurent GROSS
#Version : 		Unified-6.0
#Last modification date	17/05/2005
#Support : 		mail to : fec-support@ires.in2p3.fr
#Remarks : 		You MUST have the driver installed and compiled, and the
#			correct pathes setted in this makefile for compiling this
#			program


#path to the FEC device driver root directory
DDPATH=../../FecPciDeviceDriver/

#path to the FEC device driver include files
DDINC=${DDPATH}include/

#path to local dir ; gcc output will issue there
LCL=./

#pathes to source and include files of the debugger
SRC=${LCL}src/
INC=${LCL}include/

#Tells for which kind of fec driver, electrical or optical, the fec device driver
#we are linking to has been compiled. Gives the fec driver version too.
#include ${DDPATH}driver.info

#compil. flags
# KH commented for distcc
#CC=gcc
#O_CCFLAGS := -O2 -Wall -DMY_FEC_IS_OPTICAL_AND_MY_FIFO_WORDS_ARE_32_BITS_LENGTH
#E_CCFLAGS := -O2 -Wall -DMY_FEC_IS_ELECTRICAL_AND_MY_FIFO_WORDS_ARE_16_BITS_LENGTH
CCFLAGS := -O2 -Wall


#--------------------------------
#MAKE frontend debugger binaries
#Default make command.
#--------------------------------
debugger:	${SRC}debugger.c \
		${INC}debugger.h \
		${DDPATH}libfec_glue.o
		${CC} -I${INC} ${CCFLAGS} -I${DDINC} -I${INC} -o ${LCL}PciDebugger.exe ${SRC}debugger.c ${DDPATH}libfec_glue.o

#next line is used for backcompatibility with FecSoftware only
optical:	debugger
#next line is used for backcompatibility with FecSoftware only
electrical:	debugger
#-------------------------
#MAKE all
#-------------------------
all:		clean \
		debugger

#-------------------------------
#Make help : Prints help screen
#-------------------------------
help:
	@echo "*********************************************"
	@echo "Options available are :"
	@echo ""
	@echo "     make help"
	@echo "     ---------"
	@echo "         - Produces current output"
	@echo ""
	@echo "    make debugger"
	@echo "    ------------"
	@echo "         - Default make command. Builds debugger.bin for optical FEC drivers"
	@echo ""
	@echo "    make clean"
	@echo "    ------------"
	@echo "         - Clean up binaries files"
	@echo ""
	@echo "    make all"
	@echo "    ------------"
	@echo "         - Calls make clean && make debugger"
	@echo ""



#-----------
#MAKE clean
#-----------
clean:
	rm -f ${LCL}*.o
	rm -f ${LCL}*.out
	rm -f ${LCL}*.bin
	rm -f ${LCL}*.a
	rm -f ${SRC}*.o
