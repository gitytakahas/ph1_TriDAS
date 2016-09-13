/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _SentinelSenderV_h_
#define _SentinelSenderV_h_

#include "config/PackageInfo.h"

/* Version definition for DiagSentinelErrorsGrabber */
#define SENTINELSENDER_PACKAGE_VERSION "5.0"

namespace SentinelSender 
{
    const std::string package  =  "SentinelSender";
    const std::string versions =  SENTINELSENDER_PACKAGE_VERSION;
    const std::string description = "SentinelSender: Process used to catch, manage and route logs";
    const std::string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    const std::string authors  =  "Laurent GROSS - laurent.gross@cern.ch";
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
