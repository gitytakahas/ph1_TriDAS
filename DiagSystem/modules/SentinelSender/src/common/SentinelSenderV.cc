/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "SentinelSenderV.h"

GETPACKAGEINFO(SentinelSender)

void SentinelSender::checkPackageDependencies() throw (config::PackageInfo::VersionException)
{
/*
    CHECKDEPENDENCY(toolbox)
    CHECKDEPENDENCY(xoap)
    CHECKDEPENDENCY(xdaq)
*/
}

std::set<std::string, std::less<std::string> > SentinelSender::getPackageDependencies()
{
    std::set<std::string, std::less<std::string> > dependencies;
/*
    ADDDEPENDENCY(dependencies,toolbox);
    ADDDEPENDENCY(dependencies,xoap);
    ADDDEPENDENCY(dependencies,xdaq);
*/
    return dependencies;
}	
