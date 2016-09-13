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
#ifndef LASERDRIVERDEFINITION_H
#define LASERDRIVERDEFINITION_H

// This file gives the offset of the registers and number of the channels
// Take care about the version of the laserdriver. The version can be set
// int the Makefile
// Number of registers (gain, bias) for the laserdriver version 4.2
#define MAXLASERDRIVERCHANNELS 0x3
#define GAINSELECTION          0x3
#define LASERDRIVERSEUDETECTED 0x8

#endif
