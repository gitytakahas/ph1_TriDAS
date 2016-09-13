/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _AjaxLogReaderWithFileServerV_h_
#define _AjaxLogReaderWithFileServerV_h_


/* Version definition for AjaxLogReaderWithFileServer */
#define AJAXLOGREADERWITHFILESERVER_PACKAGE_VERSION "1.0"

#ifdef WILL_COMPILE_DIAG_WITH_PACKAGE_INFOS

#include "config/PackageInfo.h"

namespace AjaxLogReaderWithFileServer 
{
    const std::string package  =  "AjaxLogReaderWithFileServer";
    const std::string versions =  AJAXLOGREADERWITHFILESERVER_PACKAGE_VERSION;
    const std::string description = "AjaxLogReaderWithFileServer: Process used to read and display logs";
    const std::string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    const std::string authors  =  "Laurent GROSS - laurent.gross@cern.ch";
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
#endif
