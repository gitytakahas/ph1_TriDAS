##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##

##################################################################
include ../../FecHeader.linux
include ../../config/FecRules.linux
##################################################################

ConfigDir:=${XDAQ_ROOT}/config
include ${ConfigDir}/mfAutoconf.rules
include ${ConfigDir}/mfDefs.linux

#
# Packages to be built
#
XERCESROOT:=${XERCESROOT}
BUILD_HOME=${FECSOFTWARE_ROOT}
Project=ThirdParty
Package=VMEConsoleDebugger

Sources= FecVmeRingDevice.cc \
	FecFunctions.cc \
	FecRingDevice.cc

Executables= VmeDebugger.cc

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${BUSADAPTERINCLUDEDIR} 

LibraryDirs = \
	${XDAQ_ROOT}/lib \
	${BUSADAPTERLIBDIR} \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} 

else
IncludeDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include 
	${BUSADAPTERINCLUDEDIR}


LibraryDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
	${BUSADAPTERLIBDIR} \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM}

endif

UserSourcePath = \
	${FECSOFTWARE_ROOT}/generic/src/common/ \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/src/common

UserCFlags =
UserCCFlags = ${FECSOFT_C++FLAGS} \
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
Libraries = ${XERCESLIB} ${HALLIB} ${BUSADAPTERLIB}
#
# Compile the source files and create a shared library
#
DynamicLibrary= 
StaticLibrary= FecVMEAccess

include ${ConfigDir}/Makefile.rules

