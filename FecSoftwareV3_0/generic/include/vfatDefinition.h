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

Copyright 2006 Juha Petajajarvi -  Rovaniemi University of Applied Sciences, Rovaniemi-Finland
based on philipsDefinition made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef VFATDEFINITION_H
#define VFATDEFINITION_H

// DEFAULT values:
#define VFAT_CONTROL0_DEFAULT      0x00
#define VFAT_CONTROL1_DEFAULT      0x00
#define VFAT_IPREAMPIN_DEFAULT     0x00
#define VFAT_IPREAMPFEED_DEFAULT   0x00
#define VFAT_IPREAMPOUT_DEFAULT    0x00
#define VFAT_ISHAPER_DEFAULT       0x00
#define VFAT_ISHAPERFEED_DEFAULT   0x00
#define VFAT_ICOMP_DEFAULT         0x00
#define VFAT_CHIPID0_DEFAULT       0x00
#define VFAT_CHIPID1_DEFAULT       0x00
#define VFAT_UPSET_DEFAULT         0x00
#define VFAT_HITCOUNT0_DEFAULT     0x00
#define VFAT_HITCOUNT1_DEFAULT     0x00
#define VFAT_HITCOUNT2_DEFAULT     0x00
#define VFAT_LATENCY_DEFAULT       0x00
#define VFAT_CHANREG_DEFAULT       0x00
#define VFAT_VCAL_DEFAULT          0x00
#define VFAT_THRESHOLD1_DEFAULT    0x00
#define VFAT_THRESHOLD2_DEFAULT    0x00
#define VFAT_CALPHASE_DEFAULT      0x00
#define VFAT_CONTROL2_DEFAULT      0x00
#define VFAT_CONTROL3_DEFAULT      0x00

//Principal registers:
#define VFAT_CONTROL0         0x00  
#define VFAT_CONTROL1         0x01
#define VFAT_IPREAMPIN        0x02
#define VFAT_IPREAMPFEED      0x03
#define VFAT_IPREAMPOUT       0x04
#define VFAT_ISHAPER          0x05
#define VFAT_ISHAPERFEED      0x06
#define VFAT_ICOMP            0x07

#define VFAT_CHIPID0          0x08
#define VFAT_CHIPID1          0x09
#define VFAT_UPSET            0x0A
#define VFAT_HITCOUNT0        0x0B
#define VFAT_HITCOUNT1        0x0C
#define VFAT_HITCOUNT2        0x0D

#define VFAT_EXTREGPOINTER    0x0E
#define VFAT_EXTREGDATA       0x0F

//Extended registers (addressed via EXTREGPOINTER and EXTREGDATA) are:
#define VFAT_LATENCY          0x00
#define VFAT_CHANREG          0x00 // Base address, Real ChanRegs are in 0x01 - 0x80
#define VFAT_VCAL             0x81
#define VFAT_VTHRESHOLD1      0x82
#define VFAT_VTHRESHOLD2      0x83
#define VFAT_CALPHASE         0x84
#define VFAT_CONTROL2         0x85
#define VFAT_CONTROL3         0x86
  //#define VFAT_SPARE            0x87

#endif
