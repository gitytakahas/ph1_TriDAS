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
#ifndef I2CACCESS_H
#define I2CACCESS_H

#include "tscTypes.h"

#include "keyType.h"

#include "ccuChannelAccess.h"

/** Ral read
 */
#define RAL_READ 0x1

/**
 * \class i2cAccess
 * This class enable i2c access to a device (read and write). It can
 * also enable/disable a i2c channel and initialise a channel to a specified value.
 * All the i2c accesses are implemented:
 * <ul>
 * <li> read/write normal mode
 * <li> read/write extended mode
 * <li> read/write RAL mode
 * </ul>
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class enable i2c access to a device
 */
class i2cAccess: public ccuChannelAccess {

 public:

  /** \brief Call the constructor of ccuChannelAccess 
   */
  i2cAccess ( keyType index,
              enumDeviceType deviceType,
              enumAccessModeType accessMask,
              FecRingDevice *fec ) ;

  /** \brief Call the destructor of ccuChannelAccess
   */
  virtual ~i2cAccess ( ) ;

  /** \brief Read a value from the register specified
   */
  tscType8 read ( ) ;

  /** \brief Read a value from the register specified
   */
  tscType8 readOffset (tscType8 offset) {
    return (readOffset (offset, 0)) ;
  }

  /** \brief Read a value from the register specified
   */
  tscType8 readOffset (tscType8 offset, tscType8 decal) ;

  /** \brief Read a value from the register specified
   */
  tscType8 read (tscType8 offset) ;

  /** \brief Write a value to the register specified
   */
  void write (tscType8 value) ;

  /** \brief Write a value to the register specified
   */
  void writeOffset (tscType8 offset, tscType8 value) {
    writeOffset (offset, 0, value) ;
  }

  /** \brief Write a value to the register specified
   */
  void writeOffset (tscType8 offset, tscType8 decal, tscType8 value) ;

  /** \brief Write a value to the register specified
   */
  void write (tscType8 offset, tscType8 value) ;

  /** \brief Enable or disable the i2c channel
   */
  void setChannelEnable (bool enable) ;

  /** \brief Enable or disable the force acknowledge bit
   */
  void setChannelForceAck (bool enable) ;

  /** \brief Initialise the channel
   */
  void setChannelInit ( ) ;

  /** \brief Initialise the channel and the force ack bit
   */
  void setChannelInit ( bool forceAck, tscType16 i2cSpeed = 100 ) ;
} ;

#endif

