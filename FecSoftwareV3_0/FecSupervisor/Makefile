#
#
# This is the FecSupervisor Package Makefile
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
Package=FecSupervisor

ifeq (${DynamicLibrary},FecSupervisor)

DynamicLibrary=FecSupervisor
Sources=FecSupervisorV.cc FecSupervisor.cc StateMachine.cc
#ExternalObjects = ${LibraryPaths} ${XERCESLIBL} ${ORACLE_LIBL} ${HALLIBL} ${BUSADAPTERLIBL} ${DIAG_XLIBL} -ltoolbox -lxoap ${FED_LIBDIRL} -lXDaqFec
ExternalObjects = ${LibraryPaths} ${XERCESLIBL} ${ORACLE_LIBL} ${HALLIBL} ${BUSADAPTERLIBL} ${DIAG_XLIBL} -ltoolbox -lxoap ${FED_LIBL} ${FED_LIBDIRL} ${FED_LIBCRATEL} -lXDaqFec -lxdaq2rc -lpthread

else

DynamicLibrary=XDaqFec
Sources=\
	FecAccess.cc FecRingDevice.cc ${BUSADAPTERSOURCES} \
	deviceDescription.cc philipsDescription.cc piaResetDescription.cc apvDescription.cc dcuDescription.cc pllDescription.cc laserdriverDescription.cc muxDescription.cc \
	TkRingDescription.cc TkDcuConversionFactors.cc TkDcuInfo.cc CCUDescription.cc TkDcuPsuMap.cc TkIdVsHostnameDescription.cc \
	dcuAccess.cc apvAccess.cc laserdriverAccess.cc DohAccess.cc muxAccess.cc philipsAccess.cc pllAccess.cc \
	PiaResetAccess.cc \
	i2cAccess.cc piaAccess.cc memoryAccess.cc ccuChannelAccess.cc \
	FecAccessManager.cc \
	XMLCommonFec.cc XMLFec.cc XMLFecDcu.cc XMLFecDevice.cc XMLFecPiaReset.cc XMLFecCcu.cc XMLTkDcuPsuMap.cc XMLTkDcuConversion.cc XMLTkIdVsHostname.cc \
	XMLTkDcuInfo.cc MemBufOutputSource.cc \
	DeviceFactory.cc PiaResetFactory.cc FecDeviceFactory.cc FecFactory.cc TkDcuConversionFactory.cc TkDcuInfoFactory.cc TkDcuPsuMapFactory.cc TkIdVsHostnameFactory.cc \
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
	XMLCommissioningAnalysis.cc \
	DbInterface.cc TShare.cc DbClient.cc DBCacheHandler.cc \
	${SOURCESDETECTOR} ${SOURCESDESCRIPTIONDETECTOR} \
	${ORACLEC++SOURCES} \
	${TRACKERDAQ_C++SOURCE}

# 	Fecaccess.cc FecRingDevice.cc ${BUSADAPTERSOURCES} \
# 	deviceDescription.cc philipsDescription.cc piaResetDescription.cc apvDescription.cc dcuDescription.cc pllDescription.cc laserdriverDescription.cc muxDescription.cc CCUDescription.cc TkRingDescription.cc TkDcuConversionFactors.cc \
# 	dcuAccess.cc apvAccess.cc laserdriverAccess.cc DohAccess.cc muxAccess.cc philipsAccess.cc pllAccess.cc \
# 	PiaResetAccess.cc \
# 	i2cAccess.cc piaAccess.cc memoryAccess.cc ccuChannelAccess.cc \
# 	FecAccessManager.cc \
# 	XMLCommonFec.cc XMLFec.cc XMLFecDevice.cc XMLFecPiaReset.cc XMLFecDcu.cc XMLFecCcu.cc MemBufOutputSource.cc  \
# 	PiaResetFactory.cc FecDeviceFactory.cc FecFactory.cc \
# 	${SOURCESDETECTOR} ${SOURCESDESCRIPTIONDETECTOR} \
# 	${ORACLEC++FECSOURCES} \
#Externalobjects = ${LibraryPaths} ${XERCESLIBL} ${ORACLE_LIBL} ${HALLIBL} ${BUSADAPTERLIBL} ${DIAG_XLIBL} -ltoolbox -lxoap ${FED_LIBDIRL}
ExternalObjects = ${LibraryPaths} ${XERCESLIBL} ${ORACLE_LIBL} ${HALLIBL} ${BUSADAPTERLIBL} ${DIAG_XLIBL} -ltoolbox -lxoap ${FED_LIBL} ${FED_LIBDIRL} ${FED_LIBCRATEL}

endif

Executables= 

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
	${XDAQ_ROOT}/include/hal \
	${XDAQ_ROOT}/include/hal/linux \
	${FECSOFTWARE_ROOT}/generic/include \
	${FECSOFTWARE_ROOT}/${Package}/include \
	${FECSOFT_INCLUDE} \
	${ORACLE_INCLUDE} \
	${BUSADAPTERINCLUDEDIR} \
	${TRACKERDAQ_INCLUDE} \
	${DIAG_INCLUDE} \
	${FED_INCLUDE} 

LibraryDirs = \
	${XDAQ_ROOT}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${BUSADAPTERLIBDIR} \
	${DIAG_LIBDIR} \
	${ORACLE_LDDFLAGS} 

else
IncludeDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
	${FECSOFTWARE_ROOT}/generic/include \
	${FECSOFTWARE_ROOT}/${Package}/include \
	${FECSOFT_INCLUDE} \
	${ORACLE_INCLUDE} \
	${BUSADAPTERINCLUDEDIR} \
	${TRACKERDAQ_INCLUDE} \
	${DIAG_INCLUDE} \
	${FED_INCLUDE} 

LibraryDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${BUSADAPTERLIBDIR} \
	${DIAG_LIBDIR} \
	${ORACLE_LDDFLAGS} 


endif

UserSourcePath = \
		${FECSOFTWARE_ROOT}/FecSupervisor/src/common \
		${FECSOFTWARE_ROOT}/generic/src/common \
		${TRACKERDAQ_SOURCEDIR}/src/common

UserCFlags =
UserCCFlags = -DXDAQAPPLICATION \
	${FECSOFT_C++FLAGS} \
	${BUSADAPTERC++FLAG} \
	${XMLPARSER_C++FLAGS} \
	${ORACLE_C++FLAGS} \
	${DIAG_C++FLAGS} \
	${TRACKERDAQ_C++FLAGS} \
	-DBT1003 \
	-DLINUX -DCMS_TK_64BITS \
        -std=c++0x  
#	-Wall -Werror

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
# Libraries = 
#
# Compile the source files and create a shared library
#
# DynamicLibrary=FecSupervisor
# StaticLibrary=

include ${ConfigDir}/Makefile.rules
