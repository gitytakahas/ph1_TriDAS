// $Id: ReconfigurationModuleV.h,v 1.2 2007/08/24 14:09:28 lgross Exp $

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
// Version definition for ReconfigurationModule
//
#ifndef _ReconfigurationModuleV_h_
#define _ReconfigurationModuleV_h_

#include "config/PackageInfo.h"

#define RECONFMODULE_PACKAGE_VERSION "5.0"

namespace ReconfigurationModule 
{
    const std::string package  =  "ReconfigurationModule";
    const std::string versions =  RECONFMODULE_PACKAGE_VERSION;
    const std::string description = "ReconfigurationModule";
    const std::string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    const std::string authors  =  "Laurent GROSS - laurent.gross@cern.ch";
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
