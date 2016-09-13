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
Package=DeviceFactoryTemplate

Library=AnalysisCommandLineParameterSet
Sources=CommandLineParameterSet.cc

Executables=deviceFactoryExample.cc DeviceFactoryValidation.cc FecXMLFileMerge.cc \
	testDcuInfo.cc testDcuPsuMap.cc PopulateDBExample.cc TkDcuFileDb.cc CCUTemplateTest.cc \
	DeviceDbInterface.cc UploadInputOutput.cc PerfDB.cc DOMParse.cc SAXCount.cpp \
	testSetRun.cc testVersionStateRun.cc XMLFecParse.cc \
	FedPllDelayAdjustement.cc \
	FecDownloadUploadPerf.cc \
	testAnalysis.cc \
	TestTkDiagErrorAnalyser.cc \
	testOCCI.cc \
	TkRingTemplate.cc \
	TestDiagUploadData.cc

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	${XDAQ_ROOT}/include \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${FECSOFT_INCLUDE} \
	${ORACLE_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE}

LibraryDirs = \
	${XDAQ_ROOT}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${ORACLE_LDDFLAGS} \
	${FED_LIBDIR}

else
IncludeDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${FECSOFT_INCLUDE} \
	${ORACLE_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE}

LibraryDirs = \
	${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${ORACLE_LDDFLAGS} \
	${FED_LIBDIR}

endif

UserSourcePath = \
	${FECSOFTWARE_ROOT}/${Project}/${Package}/src/common

UserCFlags =
UserCCFlags = ${FECSOFT_C++FLAGS} \
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
Libraries = ${XERCESLIB} ${ORACLE_LIB} DeviceDescriptions ${FED_LIB} 

#
# Compile the source files and create a shared library
#
DynamicLibrary= ${Library}
StaticLibrary=

include ${ConfigDir}/Makefile.rules


clean:
	rm -f ./src/$(XDAQ_OS)/$(XDAQ_PLATFORM)/*.o
