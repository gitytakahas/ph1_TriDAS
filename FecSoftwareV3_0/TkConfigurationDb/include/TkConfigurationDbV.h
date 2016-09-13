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
// Version definition for TkConfigurationDb
//
#ifndef _TkConfigurationDbV_h_
#define _TkConfigurationDbV_h_

#include "config/PackageInfo.h"

namespace TkConfigurationDb 
{
  const std::string package  =  "TkConfigurationDb";
  const std::string versions =  "1.0";
  const std::string description = "TkConfigurationDb: graphic user interface for the Strip Tracker Configuration database" ;
  const std::string link = "http://cern.ch/xdaq/doc/3/TkConfigurationDb/index.html";
  const std::string authors  =  "Frederic Drouhin (Universite de Haute-Alsace - Frederic/Drouhin@cern.ch" ;
  const std::string summary  =  "";
  config::PackageInfo getPackageInfo();
  void checkPackageDependencies() throw (config::PackageInfo::VersionException);
  std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
