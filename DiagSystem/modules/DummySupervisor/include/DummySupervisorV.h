// $Id: DummySupervisorV.h,v 1.5 2010/09/26 11:25:58 lgross Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

//
// Version definition for DummySupervisor
//
#ifndef _DummySupervisorV_h_
#define _DummySupervisorV_h_


#define DUMMYSUPERVISOR_PACKAGE_VERSION "5.0"

#ifdef WILL_COMPILE_DIAG_WITH_PACKAGE_INFOS

#include "config/PackageInfo.h"

namespace DummySupervisor 
{
    const std::string package  =  "DummySupervisor";
    const std::string versions =  DUMMYSUPERVISOR_PACKAGE_VERSION;
    const std::string description = "DummySupervisor: Fake supervisor process, used to test TkDiagSystem";
    const std::string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    const std::string authors  =  "Laurent GROSS - laurent.gross@cern.ch";
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
#endif
