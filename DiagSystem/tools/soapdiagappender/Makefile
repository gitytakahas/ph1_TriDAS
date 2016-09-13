#FileName : 		Makefile
#Content :		Makefile for SOAPDiagAppender module
#Version : 		DiagSystem 4.0
#Last modification date	19/12/2006
#Support : 		mail to : laurent.gross@ires.in2p3.fr
#online help : https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem


#Useless for now
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
Project = tools

#define package path - relative to DiagSystem/project root
Package=soapdiagappender

Sources= SOAPDiagAppender.cc

ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/linux \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT)

LibraryDirs = \
	$(XDAQ_ROOT)/lib
else
IncludeDirs = \
	$(XDAQ_ROOT)/x86/include \
	$(XDAQ_ROOT)/x86/include/linux \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT)

LibraryDirs = \
	$(XDAQ_ROOT)/x86/lib
endif	

UserCFlags=
UserCCFlags= -fPIC
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries=

#
# Compile the source files and create a shared library
#
DynamicLibrary=soapdiagappender
StaticLibrary=
Executables=

#
# For test programs
#
TestLibraries =
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
