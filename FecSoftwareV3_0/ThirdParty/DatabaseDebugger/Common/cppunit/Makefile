##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##

##################################################################
include ../../../../FecHeader.linux
include ../../../../config/FecRules.linux
##################################################################

XDAQ_ROOT_DIR:=$(XDAQ_ROOT)
ConfigDir:=$(XDAQ_ROOT_DIR)/config
include $(ConfigDir)/mfAutoconf.rules
include $(ConfigDir)/mfDefs.linux

#
# Packages to be built
#
XERCESROOT:=${XERCESROOT}
BUILD_HOME=$(FECSOFTWARE_ROOT)
Project=ThirdParty
Package=DatabaseDebugger/Common/cppunit/


Sources=\
	DbCommonAccess.cc DbCommonAccessTest.cc

Executables= Main.cc

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(FECSOFTWARE_ROOT)/generic/include \
	${ORACLE_INCLUDE} \
	${FECSOFT_INCLUDE} \
	../../cppunit/include

LibraryDirs = \
	$(XDAQ_ROOT)/lib \
	${FECSOFTWARE_ROOT}/generic/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(ORACLE_LDDFLAGS) \
	../../cppunit/lib

else
IncludeDirs = \
	$(XDAQ_ROOT)/$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/$(XDAQ_PLATFORM)/include/$(XDAQ_OS) \
	$(FECSOFTWARE_ROOT)/generic/include \
	${ORACLE_INCLUDE} \
	${FECSOFT_INCLUDE} \
	../../cppunit/include

LibraryDirs = \
	$(XERCESROOT)/lib \
	${FECSOFTWARE_ROOT}/generic/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(ORACLE_LDDFLAGS) \
	../../cppunit/lib

endif

UserSourcePath = \
	$(FECSOFTWARE_ROOT)/generic/src/common/  \
	$(FECSOFTWARE_ROOT)/FecSupervisor/src/common/  \
	$(FECSOFTWARE_ROOT)/$(Project)/$(Package)/src/common/ \
	$(FECSOFTWARE_ROOT)/$(Project)/DatabaseDebugger/COMMON/cppunit/src/common/

UserCFlags =
UserCCFlags = -g ${FECSOFT_C++FLAGS} \
	${XMLPARSER_C++FLAGS} \
	${ORACLE_C++FLAGS} \
	-DBT1003 -DLINUX \
	-Wall -Werror

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XERCESLIB) $(ORACLE_LIB) cppunit

#
# Compile the source files and create a shared library
#
DynamicLibrary= COMMONDebugger
StaticLibrary=

include $(ConfigDir)/Makefile.rules

