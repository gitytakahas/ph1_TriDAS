#FileName : 		Makefile
#Content :		Makefile for ErrorDispatcher module
#Version : 		DiagSystem 4.0
#Last modification date	19/12/2006
#Support : 		mail to : laurent.gross@ires.in2p3.fr
#online help : https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem


#Useless for now
include ../../DiagHeader.linux


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

Package=modules/LocalErrorDispatcher

Sources= LocalErrorDispatcher.cc LocalErrorDispatcherV.cc

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	$(XDAQ_ROOT_DIR)/include \
	$(XDAQ_ROOT_DIR)/include/linux \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT) \
	$(XDAQ_ROOT_DIR)/config
LibraryDirs = \
	$(XDAQ_ROOT_DIR)/x86/lib \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 
else
IncludeDirs = \
	$(XDAQ_ROOT_DIR)/x86/include \
	$(XDAQ_ROOT_DIR)/x86/include/linux \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT) \
	$(XDAQ_ROOT_DIR)/config
LibraryDirs = \
	$(XDAQ_ROOT_DIR)/x86/lib \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 
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

#GWT FIREITEMS CENTRALIZED VISUALISATION TOOL RELATED
UseGwtAppletForFireitemsVisualisationCCFlags =
ifeq ($(DIAG_WILL_USE_GWT_FIREITEMS_VIEWER),yes)
UseGwtAppletForFireitemsVisualisationCCFlags = -DWILL_USE_GWT_FIREITEMS_VIEWER
endif


UserCCFlags = -g -fPIC $(EnableUseOfTrackerOnlineDbCCFlags) $(CompileForTrackerCCFlags) $(CompileForPixelsCCFlags) $(EnableParsingOfTrackerXtraBufferCCFlags) $(UseGwtAppletForFireitemsVisualisationCCFlags)

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#

#Libraries = 
ExternalObjects = $(LibraryPaths) -lxmldiagappender -lTkDiagBagWizard -lsoapdiagappender

#
# Compile the source files and create a shared library
#
DynamicLibrary=LocalErrorDispatcher
StaticLibrary=
Executables=

include $(XDAQ_ROOT_DIR)/config/Makefile.rules
