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
Package=APIXMLDebugger

Sources= 

Executables=FecProfiler.cc 

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
	${XDAQ_ROOT}/include/hal \
	${XDAQ_ROOT}/include/hal/linux \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${BUSADAPTERINCLUDEDIR} \
	${ORACLE_INCLUDE} 

LibraryDirs = \
	${XDAQ_ROOT}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${BUSADAPTERLIBDIR} \
	${ORACLE_LDDFLAGS} \
	${FED_LIBDIR}

else
IncludeDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${BUSADAPTERINCLUDEDIR} \
	${ORACLE_INCLUDE} 

LibraryDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${BUSADAPTERLIBDIR} \
	${ORACLE_LDDFLAGS} \
	${FED_LIBDIR}

endif

UserSourcePath = \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/src/common

UserCFlags =
UserCCFlags = ${FECSOFT_C++FLAGS} \
	${BUSADAPTERC++FLAG} \
	${XMLPARSER_C++FLAGS} \
	${ORACLE_C++FLAGS} \
	-DBT1003 \
	-DLINUX -std=c++0x
#	-Wall -Werror

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = ${XERCESLIB} ${HALLIB} ${ORACLE_LIB} ${BUSADAPTERLIB} DeviceAccess DeviceDescriptions ${FED_LIB} 

#
# Compile the source files and create a shared library
#
DynamicLibrary= 
StaticLibrary= 

include ${ConfigDir}/Makefile.rules

