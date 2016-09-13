// $Id: DummySupervisorDiagLvlOneV.h,v 1.2 2006/06/20 12:23:20 lgross Exp $

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
// Version definition for DiagSentinelErrorsGrabber
//
#ifndef _DummySupervisorDiagLvlOneV_h_
#define _DummySupervisorDiagLvlOneV_h_

#include "PackageInfo.h"

#define DUMMYSUPERVISORDIAGLVLONE_PACKAGE_VERSION "2.1"

namespace DummySupervisorDiagLvlOne 
{
    const string package  =  "DummySupervisorDiagLvlOne";
    const string versions =  DUMMYSUPERVISORDIAGLVLONE_PACKAGE_VERSION;
    const string description = "DummySupervisorDiagLvlOne: Process used to close the system recovery loop";
    const string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    set<string, less<string> > getPackageDependencies();
}

#endif
