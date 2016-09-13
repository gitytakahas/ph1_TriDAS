##
#
# This is the TkConfigurationDb Package Makefile
#
##

##################################################################
include ../FecHeader.linux
include ../config/FecRules.linux
##################################################################

ConfigDir:=${XDAQ_ROOT}/config
include ${ConfigDir}/mfAutoconf.rules
include ${ConfigDir}/mfDefs.linux

#
# Packages to be built
#
BUILD_HOME:=${FECSOFTWARE_ROOT}
Package=TkConfigurationDb

Sources=\
	TkConfigurationDbV.cc TkConfigurationDb.cc

Executables= 

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${FECSOFTWARE_ROOT}/TkConfigurationDb/include \
	${ORACLE_INCLUDE} \
	${FECSOFT_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE} \
	${DIAG_INCLUDE} 

LibraryDirs = \
	${XDAQ_ROOT}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${ORACLE_LDDFLAGS} \
	${DIAG_LIBDIR}

else
IncludeDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
	${FECSOFTWARE_ROOT}/generic/include \
	${FECSOFTWARE_ROOT}/TkConfigurationDb/include \
	${ORACLE_INCLUDE} \
	${FECSOFT_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE} \
	${DIAG_INCLUDE} 


LibraryDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM}
	${ORACLE_LDDFLAGS} \
	${DIAG_LIBDIR}

endif

UserSourcePath = \
		${FECSOFTWARE_ROOT}/TkConfigurationDb/src/common \
		${FECSOFTWARE_ROOT}/generic/src/common 

UserCFlags =
UserCCFlags = -DXDAQAPPLICATION \
	${FECSOFT_C++FLAGS} \
	${XMLPARSER_C++FLAGS} \
	${ORACLE_C++FLAGS} \
	${DIAG_C++FLAGS} -std=c++0x

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
#Libraries = toolbox xoap ${XERCESLIB} ${ORACLE_LIB}
ExternalObjects = ${LibraryPaths} ${XERCESLIBL} ${ORACLE_LIBL} ${DIAG_XLIBL} -ltoolbox -lxoap ${FED_LIBL} ${FED_LIBDIRL} -lDeviceDescriptions

#
# Compile the source files and create a shared library
#
DynamicLibrary= TkConfigurationDb
StaticLibrary=

include ${ConfigDir}/Makefile.rules
