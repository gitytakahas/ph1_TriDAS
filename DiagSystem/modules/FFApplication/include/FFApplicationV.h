// $Id: FFApplicationV.h,v 1.1 2006/06/07 16:56:31 lgross Exp $

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
#ifndef _FFApplicationV_h_
#define _FFApplicationV_h_

#include "PackageInfo.h"

#define FFAPPLICATION_PACKAGE_VERSION "2.0.0"

namespace DummySupervisorDiagLvlOne 
{
    const string package  =  "FFApplication";
    const string versions =  FFAPPLICATION_PACKAGE_VERSION;
    const string description = "FFApplication: Process used to emulate error messages sent by a filter farm";
    const string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    toolbox::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
    set<string, less<string> > getPackageDependencies();
}

#endif
