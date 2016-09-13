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
Project=Preshower
Package=generic

ifeq (${Library},EsDeviceAccess)
  Sources=\
	ESDbAccess.cc ESDbFecAccess.cc ESDbMbResetAccess.cc  \
	XMLESFec.cc XMLESFecDcu.cc XMLESFecDevice.cc XMLESFecDmDcu.cc \
	XMLESFecMbDcu.cc XMLESFecMbReset.cc esMemBufOutputSource.cc\
	FecAccess.cc FecRingDevice.cc ${BUSADAPTERSOURCES} \
	deviceDescription.cc philipsDescription.cc piaResetDescription.cc apvDescription.cc dcuDescription.cc pllDescription.cc laserdriverDescription.cc muxDescription.cc \
	TkRingDescription.cc TkDcuConversionFactors.cc TkDcuInfo.cc CCUDescription.cc TkDcuPsuMap.cc TkIdVsHostnameDescription.cc \
	dcuAccess.cc apvAccess.cc laserdriverAccess.cc DohAccess.cc muxAccess.cc philipsAccess.cc pllAccess.cc \
	PiaResetAccess.cc \
	i2cAccess.cc piaAccess.cc memoryAccess.cc ccuChannelAccess.cc \
	FecAccessManager.cc \
	XMLCommonFec.cc XMLFec.cc XMLFecDcu.cc XMLFecDevice.cc XMLFecPiaReset.cc XMLFecCcu.cc XMLTkDcuPsuMap.cc XMLTkDcuConversion.cc XMLTkIdVsHostname.cc \
	XMLTkDcuInfo.cc MemBufOutputSource.cc \
	PiaResetFactory.cc FecDeviceFactory.cc FecFactory.cc TkDcuConversionFactory.cc TkDcuInfoFactory.cc TkDcuPsuMapFactory.cc TkIdVsHostnameFactory.cc \
	${SOURCESDETECTOR} ${SOURCESDESCRIPTIONDETECTOR} \
	${ORACLEC++SOURCES}


	ExternalObjects = ${BUSADAPTERLIBDIRL} ${FECSOFT_LIBL} ${FECUSBSOFT_LIBL} -lpthread
else
  Library=EsDeviceDescriptions
  Sources=\
	ESDbAccess.cc ESDbFecAccess.cc ESDbMbResetAccess.cc  \
	XMLESFec.cc XMLESFecDcu.cc XMLESFecDevice.cc XMLESFecDmDcu.cc \
	XMLESFecMbDcu.cc XMLESFecMbReset.cc esMemBufOutputSource.cc\
	XMLCommonFec.cc XMLFec.cc XMLFecDcu.cc XMLFecDevice.cc XMLFecPiaReset.cc XMLFecCcu.cc XMLConnection.cc \
	XMLTkDcuPsuMap.cc XMLTkDcuConversion.cc XMLTkDcuInfo.cc XMLTkIdVsHostname.cc \
	MemBufOutputSource.cc ConnectionDescription.cc \
	PiaResetFactory.cc FecDeviceFactory.cc FecFactory.cc TkDcuConversionFactory.cc TkDcuInfoFactory.cc  TkDcuPsuMapFactory.cc TkIdVsHostnameFactory.cc \
	deviceDescription.cc philipsDescription.cc piaResetDescription.cc apvDescription.cc dcuDescription.cc pllDescription.cc laserdriverDescription.cc muxDescription.cc \
	TkRingDescription.cc TkDcuConversionFactors.cc TkDcuInfo.cc CCUDescription.cc TkDcuPsuMap.cc TkIdVsHostnameDescription.cc \
	CommissioningAnalysisDescription.cc \
	ApvLatencyAnalysisDescription.cc \
	CalibrationAnalysisDescription.cc \
	FastFedCablingAnalysisDescription.cc \
	FineDelayAnalysisDescription.cc \
	OptoScanAnalysisDescription.cc \
	PedestalsAnalysisDescription.cc \
	TimingAnalysisDescription.cc \
	VpspScanAnalysisDescription.cc \
	CommissioningAnalysisFactory.cc \
	${SOURCESDESCRIPTIONDETECTOR} \
	${ORACLEC++SOURCES} \
	${SOURCESFED9U}

endif

Executables=

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
	${XDAQ_ROOT}/include/hal \
	${XDAQ_ROOT}/include/hal/linux \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
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
	${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
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
	${FECSOFTWARE_ROOT}/${Project}/${Package}/src/ \
	${FECSOFTWARE_ROOT}/generic/src/common 

UserCFlags =
UserCCFlags = ${FECSOFT_C++FLAGS} \
	${BUSADAPTERC++FLAG} \
	${XMLPARSER_C++FLAGS} \
	${ORACLE_C++FLAGS} \
	-DBT1003 \
	-DLINUX 
#	-DDATABASESTAT
#	-DDATABASEDEBUG
#	-DDISPLAYERRORCOUNTER 
#	-Wall -Werror

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = 
#
# Compile the source files and create a shared library
#
#DynamicLibrary= DeviceAccessV2
DynamicLibrary= ${Library}
StaticLibrary=

include ${ConfigDir}/Makefile.rules
#if [[ -e 'sed.mak' ]] ; then 
#include seb.mak
#fi 
