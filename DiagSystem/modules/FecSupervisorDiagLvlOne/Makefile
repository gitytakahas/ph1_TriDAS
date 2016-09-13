#FileName : 		Makefile
#Content :		Makefile for FecSupervisorDiagLvlOne module
#Version : 		TDS 1.3
#Last modification date	03/03/2006
#Support : 		mail to : laurent.gross@ires.in2p3.fr
#online help : https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem

#Programmer : 		Laurent GROSS, work based on :
# $Id: Makefile,v 1.13 2011/12/07 17:43:12 agiassi Exp $
#########################################################################
# XDAQ Components for Distributed Data Acquisition                      #
# Copyright (C) 2000-2004, CERN.			                #
# All rights reserved.                                                  #
# Authors: J. Gutleber and L. Orsini					#
#                                                                       #
# For the licensing terms see LICENSE.		                        #
# For the list of contributors see CREDITS.   			        #
#########################################################################
##
#
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


Package=modules/FecSupervisorDiagLvlOne

Sources= FecSupervisorDiagLvlOne.cc FecSupervisorDiagLvlOneV.cc

#ifeq ($(XDAQ_RPMBUILD),yes)
IncludeDirs = \
	$(XDAQ_ROOT_DIR)/daq/extern/xerces/include \
	$(XDAQ_ROOT_DIR)/daq/extern/log4cplus/include \
	$(XDAQ_ROOT_DIR)/daq/extern/cgicc/include \
	$(XDAQ_ROOT_DIR)/daq/toolbox/include \
	$(XDAQ_ROOT_DIR)/daq/toolbox/include/$(XDAQ_OS) \
	$(XDAQ_ROOT_DIR)/daq/toolbox/include/solaris \
	$(XDAQ_ROOT_DIR)/daq/extern/mimetic/include \
	$(XDAQ_ROOT_DIR)/daq/xoap/include \
	$(XDAQ_ROOT_DIR)/daq/xdata/include \
	$(XDAQ_ROOT_DIR)/daq/pt/include \
	$(XDAQ_ROOT_DIR)/daq/xcept/include \
	$(XDAQ_ROOT_DIR)/daq/xgi/include \
	$(XDAQ_ROOT_DIR)/daq/ \
	$(XDAQ_ROOT_DIR)/daq/extern/i2o/include/ \
	$(XDAQ_ROOT_DIR)/daq/extern/i2o/include/i2o \
	$(XDAQ_ROOT_DIR)/daq/extern/i2o/include/i2o/shared \
	$(XDAQ_ROOT_DIR)/daq/xdaq/include \
	$(XDAQ_ROOT_DIR)/daq/xdaq/include/solaris \
	$(XDAQ_ROOT_DIR)/daq/xdaq/include/linux \
	$(XDAQ_ROOT_DIR)/daq/xdaq/executive/include \
	$(XDAQ_ROOT_DIR)/daq/xplore/include \
	$(XDAQ_ROOT_DIR)/daq/xplore/xslp/include \
	$(XDAQ_ROOT_DIR)/daq/extern/slp/include/ \
	$(XDAQ_ROOT_DIR)/daq/sentinel/include \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/include \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT) \
	$(XDAQ_ROOT_DIR)/config \
	$(ENV_CMS_TK_XDAQ)/include \
        $(XDAQ_ROOT_DIR)/daq/xrelay/include
	
LibraryDirs = $(XDAQ_ROOT_DIR)/daq/xoap/lib/ \
	$(XDAQ_ROOT_DIR)/daq/extern/xerces/lib \
	$(XDAQ_ROOT_DIR)/daq/extern/log4cplus/lib \
	$(XDAQ_ROOT_DIR)/daq/toolbox/lib/ \
	$(XDAQ_ROOT_DIR)/daq/xdata/lib/ \
	$(XDAQ_ROOT_DIR)/daq/xcept/lib/ \
	$(XDAQ_ROOT_DIR)/daq/pt/lib/ \
	$(XDAQ_ROOT_DIR)/daq/extern/mimetic/lib \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/lib \
	$(XDAQ_ROOT_DIR)/daq/xrelay/lib\
        $(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/lib/ \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/lib/ \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/lib/ \
#else
IncludeDirs = \
	$(XDAQ_ROOT_DIR)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT_DIR)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT_DIR)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT_DIR)/daq/toolbox/include \
	$(XDAQ_ROOT_DIR)/daq/toolbox/include/$(XDAQ_OS) \
	$(XDAQ_ROOT_DIR)/daq/toolbox/include/solaris \
	$(XDAQ_ROOT_DIR)/daq/extern/mimetic/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT_DIR)/daq/xoap/include \
	$(XDAQ_ROOT_DIR)/daq/xdata/include \
	$(XDAQ_ROOT_DIR)/daq/pt/include \
	$(XDAQ_ROOT_DIR)/daq/xcept/include \
	$(XDAQ_ROOT_DIR)/daq/xgi/include \
	$(XDAQ_ROOT_DIR)/daq/ \
	$(XDAQ_ROOT_DIR)/daq/extern/i2o/include/ \
	$(XDAQ_ROOT_DIR)/daq/extern/i2o/include/i2o \
	$(XDAQ_ROOT_DIR)/daq/extern/i2o/include/i2o/shared \
	$(XDAQ_ROOT_DIR)/daq/xdaq/include \
	$(XDAQ_ROOT_DIR)/daq/xdaq/include/solaris \
	$(XDAQ_ROOT_DIR)/daq/xdaq/include/linux \
	$(XDAQ_ROOT_DIR)/daq/xdaq/executive/include \
	$(XDAQ_ROOT_DIR)/daq/xplore/include \
	$(XDAQ_ROOT_DIR)/daq/xplore/xslp/include \
	$(XDAQ_ROOT_DIR)/daq/extern/slp/linuxx86/include/ \
	$(XDAQ_ROOT_DIR)/daq/sentinel/include \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/include \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/include \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/include \
	$(ENV_CMS_TK_DIAG_ROOT) \
	$(XDAQ_ROOT_DIR)/config \
	$(ENV_CMS_TK_XDAQ)/include \
        $(XDAQ_ROOT_DIR)/daq/xrelay/include
	
LibraryDirs = $(XDAQ_ROOT_DIR)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT_DIR)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT_DIR)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT_DIR)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT_DIR)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT_DIR)/daq/xcept/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT_DIR)/daq/pt/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT_DIR)/daq/extern/mimetic/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/lib \
	$(XDAQ_ROOT_DIR)/daq/xrelay/lib/linux/x86 \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/soapdiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(ENV_CMS_TK_DIAG_ROOT)/tools/xmldiagappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(ENV_CMS_TK_DIAG_ROOT)/tools/diagbag/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#endif


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
DynamicLibrary=FecSupervisorDiagLvlOne
StaticLibrary=
Executables=

include $(XDAQ_ROOT_DIR)/config/Makefile.rules
