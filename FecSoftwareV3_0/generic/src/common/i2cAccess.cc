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
#include "i2cAccess.h"

/**
 * Call the constructor of ccuChannelAccess in order to set
 * a key for this device and the hardware access
 * \param index - index that define the path for a device
 * \param deviceType - device type (pll, apv, ...)
 * \param accessMask - access mode (read, write, read-write)
 * \exception FecExceptionHandler
 */
i2cAccess::i2cAccess ( keyType index,
                       enumDeviceType deviceType,
                       enumAccessModeType accessMask,
                       FecRingDevice *fec ): 

  ccuChannelAccess ( index, deviceType, accessMask, fec ) {

}


/**
 * disconnect the chip via the destructor of ccuChannelAccess, just
 * decrement a counter of the number of access. See the destructor of
 * ccuChannelAccess class
 */
i2cAccess::~i2cAccess ( ) { }

/**
 * Read a value in the corresponding device in philips mode
 * \return the value read in the device register
 * ex
 */
tscType8 i2cAccess::read ( ) {

  int value = fecRingDevice_->readi2cDevice (accessKey_) ;

  return (value) ;
}

/**
 * Read a value in the corresponding device in philips mode
 * \param offset - give an offset to the address set in the constructor
 * \param decal - shift the value. Use only for the laserdriver
 * \return the value read in the device
 * \exception FecExceptionHandler
 */
tscType8 i2cAccess::readOffset ( tscType8 offset, tscType8 decal ) {

  int value = fecRingDevice_->readi2cDeviceOffset (accessKey_, offset, decal) ;

  return (value) ;
}

/**
 * Read a value in the corresponding device in RAL/extended mode.
 * \param offset - offset for the given register
 * \return the value read in the device
 * \exception FecExceptionHandler
 */
tscType8 i2cAccess::read (tscType8 offset) {

  tscType8 value ;

  if (deviceI2cMode[deviceType_] == RALMODE)   // Ral mode
    value = fecRingDevice_->readi2cRalDevice ( accessKey_, offset ) ;
  else                                         // Extended mode
    value = fecRingDevice_->readi2cDevice ( accessKey_, offset ) ;

  return (value) ;
}

/** 
 * Write a value to the corresponding device in normal mode
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void i2cAccess::write (tscType8 value) {

  fecRingDevice_->writei2cDevice (accessKey_, value) ;

}

/** 
 * write a value to the corresponding device in normal mode
 * \param offset - give an offset to the address set in the constructor
 * \param decal - shift the value. Use only for the laserdriver
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void i2cAccess::writeOffset (tscType8 offset, tscType8 decal, tscType8 value) {

  fecRingDevice_->writei2cDeviceOffset (accessKey_, offset, decal, value) ;

}

/** 
 * write a value to the corresponding device in RAL mode.
 * \param offset - offset of the register
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void i2cAccess::write (tscType8 offset, tscType8 value) {

  if (deviceI2cMode[deviceType_] == RALMODE)   // Ral mode
    fecRingDevice_->writei2cRalDevice (accessKey_, offset, value) ;
  else                                         // Extended mode
    fecRingDevice_->writei2cDevice (accessKey_, offset, value) ;
}

/**
 * Enable or disable the i2c channel corresponding corresponding to the argument
 * \param enable - boolean for enable (true) or disable (false)
 * \exception FecExceptionHandler
 */
void i2cAccess::setChannelEnable (bool enable) {

  if (!enable) setChannelForceAck (false) ;

  fecRingDevice_->setChannelEnable (accessKey_, enable) ;
}

/**
 * Enable or disable the i2c channel corresponding to the argument
 * \param force - boolean for enable (true) or disable (false)
 * \exception FecExceptionHandler
 */
void i2cAccess::setChannelForceAck (bool force) {
  fecRingDevice_->setChannelForceAck (accessKey_, force) ;
}


/**
 * In order to initialise the channel control register A: 
 * \exception FecExceptionHandler
 */
void i2cAccess::setChannelInit ( ) {

  fecRingDevice_->setInitI2cChannelCRA (accessKey_) ;
}


/**
 * In order to initialise the channel control register A: 
 * \param forceAck - force acknowledge
 * \exception FecExceptionHandler
 */
void i2cAccess::setChannelInit ( bool forceAck, tscType16 i2cSpeed ) {

  fecRingDevice_->setInitI2cChannelCRA (accessKey_, forceAck, i2cSpeed ) ;
}
