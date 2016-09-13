KERTYPE := $(shell uname -r |grep 2.6)
ifeq ($(KERTYPE),)

#FileName : 		Makefile
#Content :		Makefile for FEC device driver
#Programmer : 		Laurent GROSS
#Version : 		Unified-6.0
#Last modification date	03/05/2005
#Support : 		mail to : fec-support@ires.in2p3.fr

KERNVER=2.4.X

#path to local dir ; gcc output will issue there
LCL=./

#pathes to source and include files of the driver
SRC=$(LCL)src/
INC=$(LCL)include/

#Gives the current version tag for the device driver
#include $(INC)make_versionning.h

#compiler & flags
# KH commented for distcc
#CC=gcc
#compiler flags for optical fecs
#CCFLAGS := -Wall -DMODULE -D__KERNEL__  -I /lib/modules/`uname -r`/build/include $(FEC_DRIVER_FLAGS)
CCFLAGS := -Wall -DMODULE -D__KERNEL__  -I /lib/modules/`uname -r`/build/include

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
#MODULE=fecpmc00

#name of the glue library
GLUE_LIB=libfec_glue

#Configuration file, for driver properties description
include include/driver.conf

default:	driver

#---------------------
#Default make command.
#---------------------
# KH replace
driver:		compile_libs 



#------------------------------------
#similar to make clean && make driver
#------------------------------------
all:			clean \
			driver


#------------------
#Prints help screen
#------------------
help:
	@echo "*********************************************"
	@echo "Options available are :"
	@echo ""
	@echo "     make help"
	@echo "     ---------"
	@echo "         - Produces current output"
	@echo ""
	@echo "    make driver"
	@echo "    ------------"
	@echo "         - Default make command."
	@echo "         - Builds a device driver compatible with both optical and electrical FECs"
	@echo "              . driver name : fecpmc00"
	@echo "              . major used  : 247"
	@echo "              . minor used  : 0"
	@echo ""
	@echo "         - Builds the glue layer libraries"
	@echo "              . raw library     : libfec_glue.o"
	@echo "              . static library  : libfec_glue.a"
	@echo ""
	@echo "    make load -- USE ONLY WITH KERNEL 2.0.X, 2.2.X or 2.4.X"
	@echo "    -------------------------------------------------------"
	@echo "         Rem : you MUST BE ROOT on the computer to use this option"
	@echo "         -----------------------------------------------------------"
	@echo "         - copy the file fecpmc00.o to /lib/modules/'uname -r'/misc/fecpmc00.o"
	@echo "         - deletes existing /dev/fecpmc00 file, if any"
	@echo "         - creates a new /dev/fecpmc00 device
	@echo "         - load, if possible, the new fec driver"
	@echo ""
	@echo "    make all"
	@echo "    ---------"
	@echo "         - Similar to make clean && make driver"
	@echo ""	
	@echo "    make clean"
	@echo "    ---------"
	@echo "         - Cleans up binaries"
	@echo ""
	@echo ""

#-----------------------
#LOAD DRIVER IN MEMORY
#-----------------------
#external script install is called with right parameters for this job
load:
	bash $(LCL)install $(MINOR) $(DEVICE) $(MODULE) $(MAJOR) $(KERNVER)



#-----------------------
#MAKE FEC DRIVER PACKAGE
#-----------------------
#next line is used for backcompatibility with FecSoftware only
electrical:		driver
#next line is used for backcompatibility with FecSoftware only
optical:		driver



#---------------
#COMPILE DRIVER
#---------------
compile_driver:		$(LCL)$(MODULE).o

$(LCL)$(MODULE).o:	$(SRC)fastfec.o
			$(LD) $(LDFLAGS) -r -o $(LCL)$(MODULE).o $(SRC)fastfec.o


$(SRC)fastfec.o:	$(SRC)fastfec.c \
			$(SRC)dd_perror.c \
			$(SRC)glue.c \
			$(INC)datatypes.h \
			$(INC)defines.h \
			$(INC)fec_ioctl.h \
			$(INC)pci_ioctl.h \
			$(INC)plx_ioctl.h \
			$(INC)dderrors.h \
			$(INC)dd_perror.h \
			$(INC)glue.h 
			$(CC) -I$(INC) $(CCFLAGS) -c -o $(SRC)fastfec.o $(SRC)fastfec.c



#-------------
#COMPILE LIBS
#-------------

compile_libs:		compile_driver \
			$(LCL)$(GLUE_LIB).o \
			$(LCL)$(GLUE_LIB).a
			$(LCL)$(GLUE_LIB).so

$(LCL)$(GLUE_LIB).o:	$(SRC)glue.o \
			$(SRC)dd_perror.o
		$(LD) $(LDFLAGS) -r -o $(LCL)$(GLUE_LIB).o \
			$(SRC)glue.o \
			$(SRC)dd_perror.o

$(LCL)$(GLUE_LIB).a:	$(SRC)glue.o \
			$(SRC)dd_perror.o
		$(AR) $(ARFLAGS) $(LCL)$(GLUE_LIB).a \
				$(SRC)glue.o \
				$(SRC)dd_perror.o

$(LCL)$(GLUE_LIB).so:	$(SRC)glue.o \
			$(SRC)dd_perror.o
		$(LD) $(LDFLAGS) -shared -o $(LCL)$(GLUE_LIB).so \
				$(SRC)glue.o \
				$(SRC)dd_perror.o

$(SRC)glue.o:		$(SRC)glue.c \
			$(INC)datatypes.h \
			$(INC)defines.h \
			$(INC)fec_ioctl.h \
			$(INC)pci_ioctl.h \
			$(INC)plx_ioctl.h \
			$(INC)glue.h \
			$(INC)dderrors.h \
			$(INC)dd_perror.h 
	$(CC) -I$(INC) $(CCFLAGS) -c -o $(SRC)glue.o $(SRC)glue.c



$(SRC)dd_perror.o:	$(SRC)dd_perror.c \
			$(INC)datatypes.h \
			$(INC)defines.h \
			$(INC)fec_ioctl.h \
			$(INC)pci_ioctl.h \
			$(INC)plx_ioctl.h \
			$(INC)glue.h \
			$(INC)dderrors.h \
			$(INC)dd_perror.h 
		$(CC) -I$(INC) $(CCFLAGS) -c -o $(SRC)dd_perror.o $(SRC)dd_perror.c



#-----------------------------------
#GENERATE HEADER FILE FOR THE DRIVER
#-----------------------------------
generate_header:
			rm -f $(INC)driver_conf.h
			@echo "/* File auto-generated - do not modify */" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "/*" >> $(INC)driver_conf.h
			@echo "	FileName :		DRIVER_CONF.H" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "	Content :		settings of the driver properties" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "	Used in :		Driver" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "	Programmer :		Laurent GROSS" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "	Version :		Unified-6.0" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "	Support :		mail to : fec-support@ires.in2p3.fr" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "	Last modification :	03/05/2005" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "	Remarks :		This file should not be modified by users ;" >> $(INC)driver_conf.h
			@echo "				File auto-generated by Makefile from driver.conf descriptions." >> $(INC)driver_conf.h
			@echo "*/" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "/*" >> $(INC)driver_conf.h
			@echo "This file is part of Fec Software project." >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "Fec Software is free software; you can redistribute it and/or modify" >> $(INC)driver_conf.h
			@echo "it under the terms of the GNU General Public License as published by" >> $(INC)driver_conf.h
			@echo "the Free Software Foundation; either version 2 of the License, or" >> $(INC)driver_conf.h
			@echo "(at your option) any later version." >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "Fec Software is distributed in the hope that it will be useful," >> $(INC)driver_conf.h
			@echo "but WITHOUT ANY WARRANTY; without even the implied warranty of" >> $(INC)driver_conf.h
			@echo "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" >> $(INC)driver_conf.h
			@echo "GNU General Public License for more details." >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "You should have received a copy of the GNU General Public License" >> $(INC)driver_conf.h
			@echo "along with Fec Software; if not, write to the Free Software" >> $(INC)driver_conf.h
			@echo "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "Copyright 2002 - 2005, Laurent GROSS - IReS/IN2P3" >> $(INC)driver_conf.h
			@echo "*/" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "/* Begin Block - avoid multiple inclusions of this file */" >> $(INC)driver_conf.h
			@echo "#ifndef DD_DRIVERCONF_H" >> $(INC)driver_conf.h
			@echo "#define DD_DRIVERCONF_H" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "#ifdef __cplusplus" >> $(INC)driver_conf.h
			@echo "extern \"C\" {" >> $(INC)driver_conf.h
			@echo "#endif" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "#define DD_MINOR	"$(MINOR) >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "#define DD_DEVICE	\"/dev/"$(DEVICE)"\"" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "#define DD_MODULE	\""$(MODULE)"\"" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "#define DD_MAJOR	"$(MAJOR) >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "#define DD_FEC_NUMBER	"$(FECNUMBER) >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "#ifdef __cplusplus" >> $(INC)driver_conf.h
			@echo "}" >> $(INC)driver_conf.h
			@echo "#endif" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h
			@echo "/* End Block - avoid multiple inclusions of this file */" >> $(INC)driver_conf.h
			@echo "#endif" >> $(INC)driver_conf.h
			@echo "" >> $(INC)driver_conf.h


#-----------
#MAKE clean
#-----------
clean:
	rm -f $(LCL)*.o
	rm -f $(LCL)*.out
	rm -f $(LCL)*.bin
	rm -f $(LCL)*.a
	rm -f $(LCL)*.so
	rm -f $(SRC)*.o
	rm -f $(LCL)*.linux



##########################################################################################
################  KERNEL 2.6 #############################################################
##########################################################################################

else

KERNVER=2.6.X
#main working directory used as base compilation path
#PWD=/root/drv3

#PWD := $(shell pwd)

#LCL = ./

#list of user defined include files
INC=$(PWD)/include


#include driver specs description file
include $(INC)/driver.conf

#path to sources, relative to PWD
#Must try to find how to provide absolute pathes,
#something like $(PWD)/src
SRC=src

#ar & flags
AR=ar
ARFLAGS := -rv

#name of the glue library
GLUE_LIB=libfec_glue




ifneq ($(KERNELRELEASE),)
	obj-m := $(MODULE).o
	EXTRA_CFLAGS += -I$(INC)
	$(MODULE)-objs := $(SRC)/fastfec.o
else
#	KERNELDIR ?= /usr/src/kernels/2.6.9-42.0.2.EL.cern-i686
# KH, SLC4 
#	KERNELDIR ?= /usr/src/kernels/`uname -r`-`uname -m`
	RELEASE = $(shell uname -r | grep -c smp)
	ifeq ($(RELEASE),1)
	 KERNELDIR ?= /usr/src/kernels/`uname -r | sed -e 's/smp/\-smp/'`-`uname -m` 
	else 
	 KERNELDIR = /usr/src/kernels/`uname -r`-`uname -m` 
	endif
	PWD := $(shell pwd)
endif

#--------------------------
#trap default make command
#--------------------------
default:	driver


#---------------------
#Default make command.
#---------------------
driver:			compile_driver \
			compile_libs

#------------------
#Prints help screen
#------------------
help:
	@echo "*********************************************"
	@echo "Options available are :"
	@echo ""
	@echo "     make help"
	@echo "     ---------"
	@echo "         - Produces current output"
	@echo ""
	@echo "    make driver"
	@echo "    ------------"
	@echo "         - Default make command."
	@echo "         - Builds a device driver compatible with both optical and electrical FECs"
	@echo "              . driver name : fecpmc00"
	@echo "              . major used  : 247"
	@echo "              . minor used  : 0"
	@echo ""
	@echo "         - Builds the glue layer libraries"
	@echo "              . raw library     : libfec_glue.o"
	@echo "              . static library  : libfec_glue.a"
	@echo ""
	@echo "    make load -- USE ONLY WITH KERNEL 2.6.X"
	@echo "    ---------------------------------------"
	@echo "         Rem : you MUST BE ROOT on the computer to use this option"
	@echo "         -----------------------------------------------------------"
	@echo "         - copy the file fecpmc00.ko to /lib/modules/'uname -r'/misc/fecpmc00.ko"
	@echo "         - deletes existing /dev/fecpmc00 file, if any"
	@echo "         - creates a new /dev/fecpmc00 device
	@echo "         - load, if possible, the new fec driver"
	@echo ""
	@echo "    make all"
	@echo "    ---------"
	@echo "         - Similar to make clean && make driver"
	@echo ""	
	@echo "    make clean"
	@echo "    ---------"
	@echo "         - Cleans up binaries"
	@echo ""
	@echo ""


#------------------------------------
#similar to make clean && make driver
#------------------------------------
all:			clean \
			driver



#------------------------------------
#compiles the memory moduel on disk
#------------------------------------
compile_driver: 
	echo "RELEASE: $(RELEASE), $(KERNELDIR)"
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules


#-----------
#MAKE clean
#-----------
clean: generate_header
	rm -f *.o
	rm -f $(PWD)/$(SRC)/*.o
	rm -f *.ko
	rm -f $(PWD)/$(SRC)/*.ko
	rm -f *.mod*
	rm -f $(PWD)/$(SRC)/*.mod*
	rm -f *.a
	rm -f *.so
	rm -f *.bin
	rm -f *.linux




#-----------------------
#LOAD DRIVER IN MEMORY
#-----------------------
#external script install is called with right parameters for this job
load:
	bash $(PWD)/install $(MINOR) $(DEVICE) $(MODULE) $(MAJOR) $(KERNVER)

#-----------------------
#MAKE FEC DRIVER PACKAGE
#-----------------------
#next line is used for backcompatibility with FecSoftware only
electrical:		driver
#next line is used for backcompatibility with FecSoftware only
optical:		driver



#-------------
#COMPILE LIBS
#-------------

compile_libs:		$(PWD)/$(GLUE_LIB).o \
			$(PWD)/$(GLUE_LIB).a \
			$(PWD)/$(GLUE_LIB).so

$(PWD)/$(GLUE_LIB).o:	$(PWD)/$(SRC)/glue.o \
			$(PWD)/$(SRC)/dd_perror.o
		$(LD) $(LDFLAGS) -r -o $(PWD)/$(GLUE_LIB).o \
			$(PWD)/$(SRC)/glue.o \
			$(PWD)/$(SRC)/dd_perror.o

$(PWD)/$(GLUE_LIB).a:	$(PWD)/$(SRC)/glue.o \
			$(PWD)/$(SRC)/dd_perror.o
		$(AR) $(ARFLAGS) $(PWD)/$(GLUE_LIB).a \
				$(PWD)/$(SRC)/glue.o \
				$(PWD)/$(SRC)/dd_perror.o

$(PWD)/$(GLUE_LIB).so:	$(PWD)/$(SRC)/glue.o \
			$(PWD)/$(SRC)/dd_perror.o
		$(LD) $(LDFLAGS) -shared -o $(PWD)/$(GLUE_LIB).so \
				$(PWD)/$(SRC)/glue.o \
				$(PWD)/$(SRC)/dd_perror.o

$(PWD)/$(SRC)/glue.o:		$(PWD)/$(SRC)/glue.c \
			$(INC)/datatypes.h \
			$(INC)/defines.h \
			$(INC)/fec_ioctl.h \
			$(INC)/pci_ioctl.h \
			$(INC)/plx_ioctl.h \
			$(INC)/glue.h \
			$(INC)/dderrors.h \
			$(INC)/dd_perror.h 
		$(CC) -I$(INC) $(CCFLAGS) -c -o $(PWD)/$(SRC)/glue.o $(PWD)/$(SRC)/glue.c


$(PWD)/$(SRC)/dd_perror.o:	$(PWD)/$(SRC)/dd_perror.c \
			$(INC)/datatypes.h \
			$(INC)/defines.h \
			$(INC)/fec_ioctl.h \
			$(INC)/pci_ioctl.h \
			$(INC)/plx_ioctl.h \
			$(INC)/glue.h \
			$(INC)/dderrors.h \
			$(INC)/dd_perror.h \
			$(INC)/driver_conf.h
		$(CC) -I$(INC) $(CCFLAGS) -c -o $(PWD)/$(SRC)/dd_perror.o $(PWD)/$(SRC)/dd_perror.c




#-----------------------------------
#GENERATE HEADER FILE FOR THE DRIVER
#-----------------------------------
generate_header:
			rm -f $(INC)/driver_conf.h
			@echo "/* File auto-generated - do not modify */" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "/*" >> $(INC)/driver_conf.h
			@echo "	FileName :		DRIVER_CONF.H" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "	Content :		settings of the driver properties" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "	Used in :		Driver" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "	Programmer :		Laurent GROSS" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "	Version :		Unified-6.0" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "	Support :		mail to : fec-support@ires.in2p3.fr" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "	Last modification :	03/05/2005" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "	Remarks :		This file should not be modified by users ;" >> $(INC)/driver_conf.h
			@echo "				File auto-generated by Makefile from driver.conf descriptions." >> $(INC)/driver_conf.h
			@echo "*/" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "/*" >> $(INC)/driver_conf.h
			@echo "This file is part of Fec Software project." >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "Fec Software is free software; you can redistribute it and/or modify" >> $(INC)/driver_conf.h
			@echo "it under the terms of the GNU General Public License as published by" >> $(INC)/driver_conf.h
			@echo "the Free Software Foundation; either version 2 of the License, or" >> $(INC)/driver_conf.h
			@echo "(at your option) any later version." >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "Fec Software is distributed in the hope that it will be useful," >> $(INC)/driver_conf.h
			@echo "but WITHOUT ANY WARRANTY; without even the implied warranty of" >> $(INC)/driver_conf.h
			@echo "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" >> $(INC)/driver_conf.h
			@echo "GNU General Public License for more details." >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "You should have received a copy of the GNU General Public License" >> $(INC)/driver_conf.h
			@echo "along with Fec Software; if not, write to the Free Software" >> $(INC)/driver_conf.h
			@echo "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "Copyright 2002 - 2005, Laurent GROSS - IReS/IN2P3" >> $(INC)/driver_conf.h
			@echo "*/" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "/* Begin Block - avoid multiple inclusions of this file */" >> $(INC)/driver_conf.h
			@echo "#ifndef DD_DRIVERCONF_H" >> $(INC)/driver_conf.h
			@echo "#define DD_DRIVERCONF_H" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "#ifdef __cplusplus" >> $(INC)/driver_conf.h
			@echo "extern \"C\" {" >> $(INC)/driver_conf.h
			@echo "#endif" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "#define DD_MINOR	"$(MINOR) >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "#define DD_DEVICE	\"/dev/"$(DEVICE)"\"" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "#define DD_MODULE	\""$(MODULE)"\"" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "#define DD_MAJOR	"$(MAJOR) >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "#define DD_FEC_NUMBER	"$(FECNUMBER) >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "#ifdef __cplusplus" >> $(INC)/driver_conf.h
			@echo "}" >> $(INC)/driver_conf.h
			@echo "#endif" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h
			@echo "/* End Block - avoid multiple inclusions of this file */" >> $(INC)/driver_conf.h
			@echo "#endif" >> $(INC)/driver_conf.h
			@echo "" >> $(INC)/driver_conf.h



endif

