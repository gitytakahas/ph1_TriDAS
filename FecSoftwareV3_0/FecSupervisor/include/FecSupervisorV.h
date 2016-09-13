/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
//
// Version definition for FecSupervisor
//
#ifndef _FecSupervisorV_h_
#define _FecSupervisorV_h_

#include <string>

#include "config/PackageInfo.h"

namespace FecSupervisor 
{
    const std::string package  =  "FecSupervisor";
    const std::string versions =  "4.0";
    const std::string description = "FecSupervisor: download and upload the Tracker Front-End devices, readout of the DCU thanks a work loop (DCU thread still available), Database caching system integrated with asynchronous finite state machine";
    const std::string link = "http://cern.ch/xdaq/doc/3/FecSupervisor/index.html";
    const std::string authors  =  "Frederic Drouhin (Universite de Haute-Alsace - Frederic/Drouhin@cern.ch" ;
    const std::string summary  =  "";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
