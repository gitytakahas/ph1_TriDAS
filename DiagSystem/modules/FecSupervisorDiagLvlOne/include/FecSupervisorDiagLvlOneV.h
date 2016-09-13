// $Id: FecSupervisorDiagLvlOneV.h,v 1.1 2006/06/07 16:56:32 lgross Exp $

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
#ifndef _FecSupervisorDiagLvlOneV_h_
#define _FecSupervisorDiagLvlOneV_h_

#include "PackageInfo.h"

#define FECSUPERVISORDIAGLVLONE_PACKAGE_VERSION "2.0.0"

namespace FecSupervisorDiagLvlOne 
{
    const string package  =  "FecSupervisorDiagLvlOne";
    const string versions =  FECSUPERVISORDIAGLVLONE_PACKAGE_VERSION;
    const string description = "FecSupervisorDiagLvlOne: Process used to close the system recovery loop for FEC sub-system";
    const string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    set<string, less<string> > getPackageDependencies();
}

#endif
