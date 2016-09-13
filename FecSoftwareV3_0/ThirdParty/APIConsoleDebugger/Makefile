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
BUILD_HOME=${FECSOFTWARE_ROOT}
Project=ThirdParty
Package=APIConsoleDebugger

Sources=APIAccess.cc 
Executables= ProgramTest.cc

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
        ${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${BUSADAPTERINCLUDEDIR} 

LibraryDirs = \
	${XDAQ_ROOT}/lib \
	${ORACLE_LDDFLAGS} \
	${BUSADAPTERLIBDIR} \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} 

else
IncludeDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
        ${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${BUSADAPTERINCLUDEDIR} 

LibraryDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
	${ORACLE_LDDFLAGS} \
	${BUSADAPTERLIBDIR} \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} 

endif

UserSourcePath = \
	${FECSOFTWARE_ROOT}/generic/src/common/ \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/src/common

UserCFlags =
UserCCFlags = ${FECSOFT_C++FLAGS} \
	${BUSADAPTERC++FLAG} \
	${XMLPARSER_C++FLAGS} \
	-DBT1003 -DLINUX \
	-std=c++0x -Wall -Wno-deprecated -Werror

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = ${XERCESLIB} ${HALLIB} ${ORACLE_LIB} ${BUSADAPTERLIB} DeviceAccess pthread

#
# Compile the source files and create a shared library
#
#DynamicLibrary= 
StaticLibrary= APIFecVme

include ${ConfigDir}/Makefile.rules

