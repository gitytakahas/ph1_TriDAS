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
#ifndef MUXDEFINITION_H
#define MUXDEFINITION_H

// This file defined all the offset for MUX register

// Note that the APVs are accessible in extended (RAL) mode (I2C)
// Offset of 1 for reading value
#define MUX_READ 0x01
// Offset of 0 for writing value
#define MUX_WRITE 0x00

// Offset of the MUX register (resistor)
#define MUX_RES_REG 0x06

#endif
