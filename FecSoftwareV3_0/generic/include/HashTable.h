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
#ifndef FECHASHTABLE_H
#define FECHASHTABLE_H

#include <vector>

#include "hashMapDefinition.h"

#include "tscTypes.h"
#include "keyType.h"

#include "apvAccess.h"         //class apvAccess ;
#include "pllAccess.h"         //class pllAccess ;
#include "laserdriverAccess.h" //class laserdriverAccess ;
#include "DohAccess.h"         //class DohAccess ;
#include "muxAccess.h"         //class muxAccess ;
#include "philipsAccess.h"     //class philipsAccess ;
#include "dcuAccess.h"         //class dcuAccess ;
#include "PiaResetAccess.h"    //class PiaResetAccess ;
#include "delay25Access.h"     //class delay25Access ;

//#include "deviceDescription.h"
//#include "piaResetDescription.h"

#ifdef PRESHOWER
// For CMS Preshower:
#include "deltaAccess.h" //class deltaAccess ;
#include "paceAccess.h"  //class paceAccess  ;
#include "kchipAccess.h" //class kchipAccess ;
#include "gohAccess.h"   //class gohAccess ;
#endif // PRESHOWER

#ifdef TOTEM
#include "vfatAccess.h" //class vfatAccess ;
#include "totemCChipAccess.h" //class totemCChipAccess ;
#include "totemBBAccess.h" //class totemBBAccess ;
#endif // TOTEM

// All the map needed for the FecSupervisor program and threads program
// Create new type in order to store the access of each device type

// Hash table for the Delay25
typedef Sgi::hash_map<keyType, delay25Access *> delay25AccessedType ;

// Hash table for the APV
typedef Sgi::hash_map<keyType, apvAccess *> apvAccessedType ;

// Hash table for the PLL
typedef Sgi::hash_map<keyType, pllAccess *> pllAccessedType ;

// Hash table for the Laserdriver
typedef Sgi::hash_map<keyType, laserdriverAccess *> laserdriverAccessedType ;

// Hash table for the Doh
typedef Sgi::hash_map<keyType, DohAccess *> dohAccessedType ;

// Hash table for the MUX
typedef Sgi::hash_map<keyType, muxAccess *> muxAccessedType ;

// Hash table for the Philips (not used in normal case)
typedef Sgi::hash_map<keyType, philipsAccess *> philipsAccessedType ;

// Hash table for the DCU
typedef Sgi::hash_map<keyType, dcuAccess *> dcuAccessedType ;

// Hash table for PIA reset - 8 bits for one channel
typedef Sgi::hash_map<keyType, PiaResetAccess *> piaAccessedType ;

#ifdef PRESHOWER
// Hash table for Delta
typedef Sgi::hash_map<keyType, deltaAccess *> deltaAccessedType ;

// Hash table for PaceAM
typedef Sgi::hash_map<keyType, paceAccess *> paceAccessedType ;

// Hash table for Kchip
typedef Sgi::hash_map<keyType, kchipAccess *> kchipAccessedType ;

// Hash table for Kchip
typedef Sgi::hash_map<keyType, gohAccess *> gohAccessedType ;
#endif // PRESHOWER

#ifdef TOTEM
// Hash table for Vfat
typedef Sgi::hash_map<keyType, vfatAccess *> vfatAccessedType ;

// Hash table for CChip
typedef Sgi::hash_map<keyType, totemCChipAccess *> cchipAccessedType ;

// Hash table for TTB
typedef Sgi::hash_map<keyType, totemBBAccess *> tbbAccessedType ;
#endif // TOTEM

// Hash table for the DCU values
typedef Sgi::hash_map<keyType, tscType16[8]> dcuValuesType ;

#endif
