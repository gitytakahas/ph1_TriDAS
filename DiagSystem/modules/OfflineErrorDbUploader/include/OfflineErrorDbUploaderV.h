/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _OfflineErrorDbUploaderV_h_
#define _OfflineErrorDbUploaderV_h_

#include "config/PackageInfo.h"

/* Version definition for OfflineErrorDbUploader */
#define OFFLINEERRORDBUPLOADER_PACKAGE_VERSION "1.0"

namespace OfflineErrorDbUploader 
{
    const std::string package  =  "OfflineErrorDbUploader";
    const std::string versions =  OFFLINEERRORDBUPLOADER_PACKAGE_VERSION;
    const std::string description = "OfflineErrorDbUploader: Process used to read and display logs";
    const std::string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    const std::string authors  =  "Laurent GROSS - laurent.gross@cern.ch";
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
