/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

/* Version definition for DiagSentinelErrorsGrabber */

#ifndef _DiagSentinelErrorsGrabberV_h_
#define _DiagSentinelErrorsGrabberV_h_

#include "PackageInfo.h"

#define DIAGSENTINELERRORSGRABBER_PACKAGE_VERSION "4.0"

namespace DiagSentinelErrorsGrabber 
{
    const string package  =  "DiagSentinelErrorsGrabber";
    const string versions =  DIAGSENTINELERRORSGRABBER_PACKAGE_VERSION;
    const string description = "DiagSentinelErrorsGrabber: Listen and Subscribe to Sentinel Contexts";
    const string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    set<string, less<string> > getPackageDependencies();
}

#endif
