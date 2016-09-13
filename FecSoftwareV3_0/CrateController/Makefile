##
#
# This is the CrateController Package Makefile
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
BUILD_HOME=${FECSOFTWARE_ROOT}
Project=
Package=CrateController

Sources=\
	CrateControllerV.cc CrateController.cc 

Executables= 

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
	${XDAQ_ROOT}/include/hal \
	${XDAQ_ROOT}/include/hal/linux \
	${FECSOFTWARE_ROOT}/generic/include \
	${FECSOFTWARE_ROOT}/${Package}/include \
	${FECSOFTWARE_ROOT}/FecSupervisor/include \
	${FECSOFT_INCLUDE} \
	${ORACLE_INCLUDE} \
	${BUSADAPTERINCLUDEDIR} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE} \
	${DIAG_INCLUDE}

LibraryDirs = \
	${XDAQ_ROOT}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${FECSOFTWARE_ROOT}/FecSupervisor/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${BUSADAPTERLIBDIR} \
	${ORACLE_LDDFLAGS} \
	${DIAG_LIBDIR}

else
IncludeDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/hal \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/hal/linux \
	${FECSOFTWARE_ROOT}/generic/include \
	${FECSOFTWARE_ROOT}/${Package}/include \
	${FECSOFTWARE_ROOT}/FecSupervisor/include \
	${FECSOFT_INCLUDE} \
	${ORACLE_INCLUDE} \
	${BUSADAPTERINCLUDEDIR} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE} \
	${DIAG_INCLUDE}

LibraryDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${FECSOFTWARE_ROOT}/FecSupervisor/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${BUSADAPTERLIBDIR} \
	${DIAG_LIBDIR} \
	${ORACLE_LDDFLAGS} 

endif


UserSourcePath = \
		${FECSOFTWARE_ROOT}/CrateController/src/common \
		${FECSOFTWARE_ROOT}/generic/src/common

UserCFlags =
UserCCFlags = -DXDAQAPPLICATION -DCMS_TK_64BITS \
	${BUSADAPTERC++FLAG} \
	${FECSOFT_C++FLAGS} \
	${XMLPARSER_C++FLAGS} \
	${ORACLE_C++FLAGS} \
	${DIAG_C++FLAGS} \
	-DBT1003 \
	-DLINUX -std=c++0x
#	-Wall -Werror

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
ExternalObjects = ${LibraryPaths} ${XERCESLIBL} ${ORACLE_LIBL} ${HALLIBL} ${BUSADAPTERLIBL} ${DIAG_XLIBL} -ltoolbox -lxoap ${FED_LIBL} ${FED_LIBDIRL} ${FED_LIBCRATEL} -lXDaqFec -lpthread

#
# Compile the source files and create a shared library
#
DynamicLibrary= CrateController
StaticLibrary=

include ${ConfigDir}/Makefile.rules
