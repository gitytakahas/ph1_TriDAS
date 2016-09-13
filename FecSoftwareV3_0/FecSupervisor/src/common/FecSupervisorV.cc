// $Id: FecSupervisorV.cc,v 1.3 2007/08/22 08:35:10 fdrouhin Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include <string>

#include "FecSupervisorV.h"

GETPACKAGEINFO(FecSupervisor)

void FecSupervisor::checkPackageDependencies() throw (config::PackageInfo::VersionException) {

  //CHECKDEPENDENCY(toolbox)
  //CHECKDEPENDENCY(xoap)
  //CHECKDEPENDENCY(xdaq)
}

std::set<std::string, std::less<std::string> > FecSupervisor::getPackageDependencies() {
  std::set<std::string, std::less<std::string> > dependencies;
  //ADDDEPENDENCY(dependencies,toolbox);
  //ADDDEPENDENCY(dependencies,xoap);
  //ADDDEPENDENCY(dependencies,xdaq);
  return dependencies;
}	
