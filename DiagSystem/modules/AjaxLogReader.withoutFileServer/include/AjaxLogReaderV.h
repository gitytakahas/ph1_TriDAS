/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _AjaxLogReaderV_h_
#define _AjaxLogReaderV_h_


/* Version definition for AjaxLogReader */
#define AJAXLOGREADER_PACKAGE_VERSION "1.0"

#ifdef WILL_COMPILE_DIAG_WITH_PACKAGE_INFOS

#include "config/PackageInfo.h"

namespace AjaxLogReader 
{
    const std::string package  =  "AjaxLogReader";
    const std::string versions =  AJAXLOGREADER_PACKAGE_VERSION;
    const std::string description = "AjaxLogReader: Process used to read and display logs";
    const std::string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
    const std::string authors  =  "Laurent GROSS - laurent.gross@cern.ch";
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
#endif
