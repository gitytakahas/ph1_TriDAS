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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace,
  Mulhouse-France
*/
#ifndef DCUDEFINITION_H
#define DCUDEFINITION_H

// Number of channels
#define MAXDCUCHANNELS 8
#define DCUSEUDETECTED 0x20
#define DIGITISATIONOK 0x80
#define WAITTIME4DIGITOK 250    // in micro seconds

#define DCUTIMEOUT 5

// Macro
// result[11:0] = SHREG[3:0],LREG[7:0]
#define isDigitisationOk(x) ((x & DIGITISATIONOK) == DIGITISATIONOK)
#define getValueShreg(x) ((x & 0xF) << 8)
#define getValueLreg(x) (x)

// This file defines all the offsets for DCU registers
#define CREG  0x00
#define SHREG 0x01
#define AREG  0x02
#define LREG  0x03
#define TREG  0x04

// This registers are defined for all DCU but used
// mainly for DCU X
#define CHIPADDL 0x05
#define CHIPADDM 0x06
#define CHIPADDH 0x07

#endif
