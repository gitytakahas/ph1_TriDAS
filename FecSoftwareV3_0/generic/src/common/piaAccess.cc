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
#include "cmdDescription.h" 

#include "FecRingDevice.h"
#include "piaAccess.h"

/**
 * Call the constructor of ccuChannelAccess in order to set
 * a key for this device and the hardware access
 * \param index - key for the pia channel access
 * \param accessMask - access mode (read, write, read-write)
 * \exception FecExceptionHandler
 */
piaAccess::piaAccess ( keyType index, 
                       enumAccessModeType accessMask,
                       FecRingDevice *fec ):

  ccuChannelAccess ( index, PHILIPS, accessMask, fec ) {
}

/**
 * disconnect the chip via the destructor of ccuChannelAccess, just
 * decrement a counter of the number of access. See the destructor of
 * ccuChannelAccess class
 */
piaAccess::~piaAccess ( ) { }

/**
 * Read a value in the data register
 * \return the value read in data register
 * \exception FecExceptionHandler
 */
tscType8 piaAccess::read ( ) {

  int value = 0 ; 
  
  value = fecRingDevice_->getPiaChannelDataReg (accessKey_) ;

  return (value) ;
}

/** 
 * Write a value to the data register
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void piaAccess::write (tscType8 value) {
 
  fecRingDevice_->setPiaChannelDataReg (accessKey_, value) ;

}

/**
 * Enable or disable the PIA channel corresponding to the argument
 * \param enable - boolean for enable (true) or disable (false)
 * \exception FecExceptionHandler
 */
void piaAccess::setChannelEnable (bool enable) {

  fecRingDevice_->setChannelEnable (accessKey_, enable) ;
}

/**
 * In order to initialise the channel control register A: 
 * Value is 0x00
 * \exception FecExceptionHandler
 * Only initialise the general control register to 0x0 + strobe signal if it is set
 */
void piaAccess::setChannelInit ( ) {

  fecRingDevice_->setInitPiaChannel (accessKey_) ;
}
