/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "ErrorDbUploaderV.h"


#ifdef WILL_COMPILE_DIAG_WITH_PACKAGE_INFOS

GETPACKAGEINFO(ErrorDbUploader);

void ErrorDbUploader::checkPackageDependencies() throw (config::PackageInfo::VersionException)
{
/*
    CHECKDEPENDENCY(toolbox)
    CHECKDEPENDENCY(xoap)
    CHECKDEPENDENCY(xdaq)
*/
}

std::set<std::string, std::less<std::string> > ErrorDbUploader::getPackageDependencies()
{
    std::set<std::string, std::less<std::string> > dependencies;
/*
    ADDDEPENDENCY(dependencies,toolbox);
    ADDDEPENDENCY(dependencies,xoap);
    ADDDEPENDENCY(dependencies,xdaq);
*/
    return dependencies;
}	

#endif
