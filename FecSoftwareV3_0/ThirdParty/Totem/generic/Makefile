##################################################################
include ../../../FecHeader.linux
include ../../../config/FecRules.linux
##################################################################

ConfigDir:=${XDAQ_ROOT}/config
include ${ConfigDir}/mfAutoconf.rules
include ${ConfigDir}/mfDefs.linux

#
# Packages to be built
#
BUILD_HOME=${FECSOFTWARE_ROOT}/ThirdParty
Project=Totem
Package=generic

ifeq (${Library},TotemDeviceDescriptions)
  Sources=\
	vfatDescription.cc totemCChipDescription.cc totemBBDescription.cc \
	TotemMemBufOutputSource.cc TotemXMLFecDevice.cc TotemFecFactory.cc \
	${ORACLEC++SOURCES}

	ExternalObjects = ${BUSADAPTERLIBDIRL} ${FECSOFT_LIBL} ${FECUSBSOFT_LIBL}
else
  Library=TotemDeviceAccess
  Sources=\
	vfatDescription.cc totemCChipDescription.cc totemBBDescription.cc \
	vfatAccess.cc totemCChipAccess.cc totemBBAccess.cc \
	TotemFecAccessManager.cc \
	${ORACLEC++SOURCES} \
	${SOURCESFED9U}

endif

Executables=

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
	${XDAQ_ROOT}/include/hal \
	${XDAQ_ROOT}/include/hal/linux \
	${FECSOFTWARE_ROOT}/ThirdParty/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${BUSADAPTERINCLUDEDIR} \
	${ORACLE_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE}

LibraryDirs = \
	${XDAQ_ROOT}/lib \
	${BUSADAPTERLIBDIR} \
	${ORACLE_LDDFLAGS} 

else
IncludeDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/hal \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/hal/linux \
	${FECSOFTWARE_ROOT}/ThirdParty/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${BUSADAPTERINCLUDEDIR} \
	${ORACLE_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE}

LibraryDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
	${BUSADAPTERLIBDIR} \
	${ORACLE_LDDFLAGS} 

endif


UserSourcePath = \
	${FECSOFTWARE_ROOT}/ThirdParty/${Project}/${Package}/src/ \
	${FECSOFTWARE_ROOT}/generic/src/common 

UserCFlags =
UserCCFlags = -g ${FECSOFT_C++FLAGS} \
	${BUSADAPTERC++FLAG} \
	${XMLPARSER_C++FLAGS} \
	${ORACLE_C++FLAGS} \
	-DBT1003 \
	-DLINUX 

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

Libraries = 

DynamicLibrary= ${Library}
StaticLibrary=

include ${ConfigDir}/Makefile.rules

