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
#ifndef PLLDEFINITION_H
#define PLLDEFINITION_H

// This file defined all the offset for PLL registers

// Internal control register offsets
#define CNTRL_1 0x00
#define CNTRL_2 0x01
#define CNTRL_3 0x02
#define CNTRL_4 0x03

  // Status register bits 
#define GOING_BIT 0x1  // bit 0 
#define SEU_BIT   0x8  // bit 3 read

// Value in order to reset a PLL
#define RST_ON  0x08
#define RST_OFF 0x00
#define MAN_GO  0x80

// PLL Fine Delay
#define CLOCK_DELAY 0x80
    
// PLL Coarse Delay
#define L1_DELAY_ON  0x20
#define L1_DELAY_OFF 0xdf

// Value that must be read
#define FINE_DELAY_READ          0x1f
#define COARSE_DELAY_READ        0x0f
#define COARSE_DELAY_UNUSED_BITS 0xf0

// The next definitions correspond to the new PLL accesses
#define PLL_CTRL1_GOING       0x01
#define PLL_CTRL1_HIGHGAIN    0x02
#define PLL_CTRL1_FORCEPFD    0x04
#define PLL_CTRL1_RESTARTSEU  0x08 // read is SEU, write is restart
#define PLL_CTRL1_TESTSELECT  0x30 // 2 bits
#define PLL_CTRL1_DISABLET1   0x40
#define PLL_CTRL1_MODE        0x80

#define PLL_CTRL2_PHASE       0x0F
#define PLL_CTRL2_PHASEINVERT 0x10
#define PLL_CTRL2_RSEL        0x20
#define PLL_CTRL2_I2CHIGHGAIN 0x40
#define PLL_CTRL2_I2CGOING    0x80

#define PLL_RESET_ON          0x08
#define PLL_RESET_OFF         0x00

#define PLL_CLOCKPHASE_READ   0x1F
#define PLL_TRIGGERDELAY_READ 0x0f

#define PLLTIMEOUT   10      // 10 reads to wait the GOING in control register 1
#define PLLDELAY   5000      // time to wait after the PLL init (or cold PLL reset)

#endif
