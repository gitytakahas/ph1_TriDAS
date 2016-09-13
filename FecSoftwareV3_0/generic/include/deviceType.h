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

#ifndef DEVICETYPE_H
#define DEVICETYPE_H

// std::vector
#include <vector>

#include "deviceDescription.h"
#include "piaResetDescription.h"
#include "CCUDescription.h"
#include "TkDcuConversionFactors.h"
#include "TkDcuInfo.h"
#include "TkState.h"
#include "TkVersion.h"
#include "TkRun.h"
#include "TkRingDescription.h"
#include "ConnectionDescription.h"
#include "TkDcuPsuMap.h"
#include "TkIdVsHostnameDescription.h"

// Definition for the FecSupervisor, XMLDevice class
// Vector of devices for the downloading
typedef std::vector<deviceDescription *> deviceVector ;
// Vector of PIA reset
typedef std::vector<piaResetDescription *> piaResetVector ;
// Vector of DCU conversion factors
typedef std::vector<TkDcuConversionFactors *> dcuConversionVector ;
// Vector of DCU Info
typedef std::vector<TkDcuInfo *> tkDcuInfoVector ;
// Vector of States
typedef std::vector<TkState *> tkStateVector ;
// Vector of Versions
typedef std::vector<TkVersion *> tkVersionVector ;
// Vector of Runs
typedef std::vector<TkRun *> tkRunVector ;
// Vector of CCUDescriptions
typedef std::vector<CCUDescription *> ccuVector ;
// Vector of connections
typedef std::vector<ConnectionDescription *> ConnectionVector ;
// Vector of TkRingDescriptions
typedef std::vector<TkRingDescription *> tkringVector ;
// Vector of DCU-PSU
typedef std::vector<TkDcuPsuMap *> tkDcuPsuMapVector ;
// Vector of TkIdVsHostnameDescription
typedef std::vector<TkIdVsHostnameDescription *> TkIdVsHostnameVector ;

#endif
