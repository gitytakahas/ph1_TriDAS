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
*/
#ifndef TOTEMCCHIPDEFINITION_H
#define TOTEMCCHIPDEFINITION_H

//Principal registers:
#define CCHIP_CONTROL0         0x00  
#define CCHIP_CONTROL1         0x01
#define CCHIP_CONTROL2         0x02  
#define CCHIP_CONTROL3         0x03
#define CCHIP_CONTROL4         0x04
#define CCHIP_CHIPID0          0x05
#define CCHIP_CHIPID1          0x06
#define CCHIP_COUNTER0         0x07
#define CCHIP_COUNTER1         0x08
#define CCHIP_COUNTER2         0x09

#define CCHIP_MASK0            0x0A
#define CCHIP_MASK1            0x0B
#define CCHIP_MASK2            0x0C
#define CCHIP_MASK3            0x0D

#define CCHIP_EXTREGPOINTER    0x0E
#define CCHIP_EXTREGDATA       0x0F

//Extended registers:
//Addressed via EXTREGPOINTER and EXTREGDATA
#define CCHIP_MASK4            0x00
#define CCHIP_MASK5            0x01
#define CCHIP_MASK6            0x02
#define CCHIP_MASK7            0x03
#define CCHIP_MASK8            0x04
#define CCHIP_MASK9            0x05
#define CCHIP_RES              0x06

#endif
