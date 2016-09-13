##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##

##################################################################
include ../FecHeader.linux
include ../config/FecRules.linux
##################################################################

ConfigDir:=$(XDAQ_ROOT)/config
include $(ConfigDir)/mfAutoconf.rules
include $(ConfigDir)/mfDefs.linux

#
# Packages to be built
#
BUILD_HOME=$(FECSOFTWARE_ROOT)
Package=FecUsbDeviceDriver

Sources=\
	fec_usb_drv.c ftdipio_func.c  

Executables=

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(FECSOFTWARE_ROOT)/${Package}/include \
	$(FECSOFTWARE_ROOT)/$(Project)/$(Package)/libftdi-0.7/include \
	$(FECSOFTWARE_ROOT)/$(Project)/$(Package)/ftdi245dd/include \
        $(FECSOFTWARE_ROOT)/$(Project)/$(Package)/ftdi245lib/include \


LibraryDirs = 
#	$(XDAQ_ROOT)/lib 
#        $(ORACLE_LDDFLAGS) \
#	$(FECSOFTWARE_ROOT)/generic/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 

else
IncludeDirs = \
	$(XDAQ_ROOT)/$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/$(XDAQ_PLATFORM)/include/$(XDAQ_OS) \
	$(FECSOFTWARE_ROOT)/${Package}/include \
	$(FECSOFTWARE_ROOT)/$(Project)/$(Package)/libftdi-0.7/include \
	$(FECSOFTWARE_ROOT)/$(Project)/$(Package)/ftdi245dd/include \
        $(FECSOFTWARE_ROOT)/$(Project)/$(Package)/ftdi245lib/include \

LibraryDirs = 
#	$(XDAQ_ROOT)/$(XDAQ_PLATFORM)/lib 
#        $(ORACLE_LDDFLAGS) \
#	$(FECSOFTWARE_ROOT)/generic/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 

endif

UserSourcePath = \
	$(FECSOFTWARE_ROOT)/$(Project)/$(Package)/src/common \
	$(FECSOFTWARE_ROOT)/$(Project)/$(Package)/ftdi245dd/src\
	$(FECSOFTWARE_ROOT)/$(Project)/$(Package)/ftdi245lib/src

UserCFlags =  -DWB_FEC_UNIQ -DFECUSELIBUSB
UserCCFlags = ${FECSOFT_C++FLAGS}  \
	        -DLINUX -DBT1003  -DWB_FEC_UNIQ

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

ExternalObjects = $(FECSOFTWARE_ROOT)/$(Project)/$(Package)/libftdi-0.7/src/ftdi.o 

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = pthread_nonshared

#
# Compile the source files and create a shared library
#
DynamicLibrary=  
StaticLibrary= fec_usb_glue

TestExecutables= test.c test2.c test_perf.c test_block_access.c 
#	test_block_access_fifos.c 
#TestExecutables= test.c test1.c test2.c test3.c test_2crepe.c
TestLibraries = fec_usb_glue \
	usb \
	pthread_nonshared


include $(ConfigDir)/Makefile.rules

