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
// Version definition for CrateController
//
#ifndef _CrateControllerV_h_
#define _CrateControllerV_h_

#include <string>

#include "config/PackageInfo.h"

namespace CrateController 
{
    const std::string package  =  "CrateController";
    const std::string versions =  "3.37.4";
    const std::string description = "CrateController: upload file or database with is detected in one crate" ;
    const std::string link = "http://cern.ch/xdaq/doc/3/CrateController/index.html";
    const std::string authors  =  "Frederic Drouhin (Universite de Haute-Alsace - Frederic/Drouhin@cern.ch" ;
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
