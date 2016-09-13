// $Id: DcuFilterV.h,v 1.5 2008/03/13 13:17:50 fdrouhin Exp $

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
// Version definition for DcuFilter
//
#ifndef _DcuFilterV_h_
#define _DcuFilterV_h_

#include <string>

#include "config/PackageInfo.h"

namespace DcuFilter 
{
    const std::string package  =  "DcuFilter";
    const std::string versions =  "3.37.4";
    const std::string description = "DcuFilter: receive DCU information from FecSupervisor, convert the ADC count to real values and send it to PVSS";
    const std::string link = "http://cern.ch/xdaq/doc/3/DcuFilter/index.html";
    const std::string authors  =  "Frederic Drouhin (Universite de Haute-Alsace - Frederic/Drouhin@cern.ch" ;
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
