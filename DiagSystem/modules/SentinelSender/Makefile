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

Package=modules/SentinelSender

Sources= SentinelSender.cc SentinelSenderV.cc

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
UserCCFlags = -g -fPIC

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
DynamicLibrary=SentinelSender
StaticLibrary=
Executables=

include $(XDAQ_ROOT_DIR)/config/Makefile.rules
