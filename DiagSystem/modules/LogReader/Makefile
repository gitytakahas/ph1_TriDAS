#FileName : 		Makefile
#Content :		Makefile for LogReader module
#Version : 		DiagSystem 4.0
#Last modification date	19/12/2006
#Support : 		mail to : laurent.gross@ires.in2p3.fr
#online help : https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem


include ../../DiagHeader.linux

ifeq ($(DIAG_LOGREADER_USES_DB),yes)
#Next line useless for now ??
#FECSOFTWARE_ROOT:=/exports/slc4/development/FecSoftwareV3_0
FECSOFTWARE_ROOT:=${ENV_CMS_TK_FEC_ROOT}
include ${FECSOFTWARE_ROOT}/FecHeader.linux
include ${FECSOFTWARE_ROOT}/config/FecRules.linux
include ../../DiagHeader.linux
endif


#define real Tridas directory given by $(XDAQ_BASE)
XDAQ_ROOT=$(XDAQ_BASE)
#Store in $(XDAQ_ROOT_DIR)
XDAQ_ROOT_DIR:=$(XDAQ_ROOT)

#include xdaq configuration files
ConfigDir:=$(XDAQ_ROOT_DIR)/config
include $(ConfigDir)/mfAutoconf.rules
include $(ConfigDir)/mfDefs.linux

#include new xdaq root in order to be able to install
#and compile the project anywhere
XDAQ_ROOT:=$(ENV_CMS_TK_DIAG_ROOT)

Package=modules/LogReader

ifeq ($(DIAG_LOGREADER_USES_DB),yes)
Sources= ErrorsTreeBuilder.cc ErrorsChecker.cc LogReader.cc LogReaderV.cc 
else
Sources= LogReader.cc LogReaderV.cc 
endif


ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	$(XDAQ_ROOT_DIR)/include \
	$(XDAQ_ROOT_DIR)/include/linux \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/include \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT) \
	$(XDAQ_ROOT_DIR)/config \
	${FECSOFTWARE_ROOT}/FecPciDeviceDriver/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${ORACLE_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE}


LibraryDirs = \
	$(XDAQ_ROOT_DIR)/lib \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
	${ORACLE_LDDFLAGS} \
	${FED_LIBDIR}
else
IncludeDirs = \
	$(XDAQ_ROOT_DIR)/x86/include \
	$(XDAQ_ROOT_DIR)/x86/include/linux \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/include \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT) \
	$(XDAQ_ROOT_DIR)/config \
	${FECSOFTWARE_ROOT}/FecPciDeviceDriver/include \
	${FECSOFTWARE_ROOT}/generic/include \
	${ORACLE_INCLUDE} \
	${FED_INCLUDE} \
	${TRACKERDAQ_INCLUDE}

LibraryDirs = \
	$(XDAQ_ROOT_DIR)/x86/lib \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	${ORACLE_LDDFLAGS} \
	${FED_LIBDIR}

endif


UserCFlags =

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


UserCCFlags = -g -fPIC $(EnableUseOfTrackerOnlineDbCCFlags) $(CompileForTrackerCCFlags) $(CompileForPixelsCCFlags) $(EnableParsingOfTrackerXtraBufferCCFlags)

UserDynamicLinkFlags = 
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
ifeq ($(DIAG_LOGREADER_USES_DB),yes)
ExternalObjects = $(LibraryPaths) -lxmldiagappender -lTkDiagBagWizard -lsoapdiagappender ${ORACLE_LIBL} -lDeviceDescriptions ${FED_LIBL} 
else
ExternalObjects = $(LibraryPaths) -lxmldiagappender -lTkDiagBagWizard -lsoapdiagappender
endif
#
# Compile the source files and create a shared library
#
DynamicLibrary=LogReader
StaticLibrary=
Executables=

include $(XDAQ_ROOT_DIR)/config/Makefile.rules
