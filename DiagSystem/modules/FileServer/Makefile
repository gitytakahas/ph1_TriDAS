#FileName : 		Makefile
#Content :		Makefile for ErrorDispatcher module
#Version : 		DiagSystem 6.0
#Last modification date	19/12/2006
#Support : 		mail to : laurent.gross@ires.in2p3.fr
#online help : https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem

include ../../DiagHeader.linux

#BUILD_HOME environement variable must point to /...../DiagSystem
BUILD_HOME:=$(ENV_CMS_TK_DIAG_ROOT)/

#include DIAGSYSTEM configuration file
include $(BUILD_HOME)/DiagHeader.linux


#if necessary, include FEC & DB files
ifeq ($(DIAG_LOGREADER_USES_DB),yes)
FECSOFTWARE_ROOT:=${ENV_CMS_TK_FEC_ROOT}
include ${FECSOFTWARE_ROOT}/FecHeader.linux
include ${FECSOFTWARE_ROOT}/config/FecRules.linux
include ../../DiagHeader.linux
endif


#include xdaq configuration files
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.linux

#define project path - relative to DiagSystem root
Project = modules

#define package path - relative to DiagSystem/project root
Package = FileServer

#Define package source files
Sources= FileServer.cc FileServerV.cc

#If we are building from RPM source distribution
ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/linux \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT) \
	${FECSOFTWARE_ROOT}/FecPciDeviceDriver/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${ORACLE_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE}

	
LibraryDirs = \
	$(XDAQ_ROOT)/x86/lib \
	$(XDAQ_ROOT)/lib \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${ORACLE_LDDFLAGS} \
	${FED_LIBDIR}

else
IncludeDirs = \
	$(XDAQ_ROOT)/x86/include \
	$(XDAQ_ROOT)/x86/include/linux \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT) \
	${FECSOFTWARE_ROOT}/FecPciDeviceDriver/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${ORACLE_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE}

LibraryDirs = \
	$(XDAQ_ROOT)/x86/lib \
	$(XDAQ_ROOT)/lib \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	${ORACLE_LDDFLAGS} \
	${FED_LIBDIR}

endif


UserCFlags =
#UserCCFlags = -g

#DB USAGE RELATED
EnableUseOfTrackerOnlineDbCCFlags =
ifeq ($(DIAG_LOGREADER_USES_DB),yes)
EnableUseOfTrackerOnlineDbCCFlags = -DDATABASE
endif

#TRACKER SPECIFIC COMPILATION OPTIONS RELATED
CompileForTrackerCCFlags =
ifeq ($(DIAG_WILL_COMPILE_FOR_TRACKER),yes)
CompileForTrackerCCFlags = -DWILL_COMPILE_FOR_TRACKER
endif


#PIXELS SPECIFIC COMPILATION OPTIONS RELATED
CompileForPixelsCCFlags =
ifeq ($(DIAG_WILL_COMPILE_FOR_PIXELS),yes)
CompileForPixelsCCFlags = -DWILL_COMPILE_FOR_PIXELS
endif


#TRACKER EXTRA BUFFER PARSING SPECIFIC COMPILATION OPTIONS RELATED
EnableParsingOfTrackerXtraBufferCCFlags =
ifeq ($(DIAG_PARSE_XTRA_BUFFER),yes)
EnableParsingOfTrackerXtraBufferCCFlags = -DPARSE_XTRA_BUFFER
endif

#GWT FIREITEMS CENTRALIZED VISUALISATION TOOL RELATED
UseGwtAppletForFireitemsVisualisationCCFlags =
ifeq ($(DIAG_WILL_USE_GWT_FIREITEMS_VIEWER),yes)
UseGwtAppletForFireitemsVisualisationCCFlags = -DWILL_USE_GWT_FIREITEMS_VIEWER
endif


#PREVENT TO EMBED PACKAGEINFOS IN CASE OF FC12//GCC 4.3 USED
DiagWillCompileWithPackageInfos =
ifeq ($(DIAG_WILL_COMPILE_WITH_PACKAGE_INFOS),yes)
DiagWillCompileWithPackageInfos = -DWILL_COMPILE_DIAG_WITH_PACKAGE_INFOS
endif


UserCCFlags = -g $(EnableUseOfTrackerOnlineDbCCFlags) $(CompileForTrackerCCFlags) $(CompileForPixelsCCFlags) $(EnableParsingOfTrackerXtraBufferCCFlags) $(UseGwtAppletForFireitemsVisualisationCCFlags) $(DiagWillCompileWithPackageInfos)

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#

#Libraries = libwseventing.so
#ExternalObjects = $(LibraryPaths) -lxmldiagappender -lTkDiagBagWizard -lsoapdiagappender -lsentineldashboard
#ExternalObjects = $(LibraryPaths) -lxmldiagappender -lTkDiagBagWizard -lsoapdiagappender
#DB USAGE RELATED
ifeq ($(DIAG_LOGREADER_USES_DB),yes)
ExternalObjects = $(LibraryPaths) -lxmldiagappender -lTkDiagBagWizard -lsoapdiagappender ${ORACLE_LIBL} -lDeviceDescriptions ${FED_LIBL} 
else
ExternalObjects = $(LibraryPaths) -lxmldiagappender -lTkDiagBagWizard -lsoapdiagappender
endif
#
#
# Compile the source files and create a shared library
#
DynamicLibrary=FileServer
StaticLibrary=
Executables=

include $(XDAQ_ROOT)/config/Makefile.rules
