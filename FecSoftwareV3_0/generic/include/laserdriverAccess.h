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
#ifndef LASERDRIVERACCESS_H
#define LASERDRIVERACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "FecAccess.h"

#include "deviceAccess.h"
#include "laserdriverDescription.h"

/**
 * \class laserdriverAccess
 * This class defined a laserdriver and make possible the downloading or
 * uploading of the values related to a specific register.
 * It also implements all the method specific to the laserdevice device.
 * \author Frederic Drouhin
 * \date April 2002
 * \warning all the I2C accesses are done in normal mode but the address is used
 * with an offset and a shift on it for the old laserdriver. The new one does
 * not use anymore this specification.
 * \warning This implementation of the laserdriver is done for the old laserdriver
 * version and the new version of the laserdriver. 
 * \brief This class define all the hardware access for the laserdriver througth the FecAccess class
 */
class laserdriverAccess: public deviceAccess {

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  laserdriverAccess ( FecAccess *fec, 
                      int Partition_id, int Module_id,
                      int Version_major_id,
                      int Version_minor_id,
                      int LaserdriverId,
                      tscType16 Fec_slot,
                      tscType16 Ring_slot,
                      tscType16 Ccu_slot,
                      tscType16 channelId,
                      tscType16 I2C_address ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  laserdriverAccess (FecAccess *fec, 
                     keyType key ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  laserdriverAccess (FecAccess *fec, 
                     tscType16 Fec_slot,
                     tscType16 Ring_slot,
                     tscType16 Ccu_slot,
                     tscType16 channelId,
                     tscType16 I2C_address ) ;

  /** \brief Remove the access
   */
  ~laserdriverAccess () ;

  /** \brief set a description for future comparison
   */
  void setDescription ( laserdriverDescription laserdriverValues ) ;

  /** \brief Set all values for a laserdriver
   */
  void setValues ( laserdriverDescription laserdriverValues ) throw (FecExceptionHandler) ;

  /** \brief Set all values for a laserdriver in multiple frames
   */
  void getBlockWriteValues ( laserdriverDescription laserdriverValues, std::list<accessDeviceType> &vAccess ) ;

  /** \brief Get all values from a laserdriver
   */
  laserdriverDescription *getValues ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setGain ( tscType8 gain ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setBias ( tscType8 *bias ) throw (FecExceptionHandler) ;

  /** \brief Get the value from the specified register
   */
  void getBias ( tscType8 *bias ) throw (FecExceptionHandler) ;

  /** \brief Get the value from the specified register
   */
  tscType8 getGain ( ) throw (FecExceptionHandler) ;

  /** \brief returns true if as SEU is detected ( bit 8 of gainReg = 1 )
   */
  bool getSeuStatus() throw (FecExceptionHandler) ;

  /** \brief static method to upload from the hardware the devices
   */
  static unsigned int getLaserdriverValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, laserdriverAccess *> &laserdriverSet, deviceVector &laserdriverVector,
							   std::list<FecExceptionHandler *> &errorList ) 
    throw (FecExceptionHandler) ;
};

#endif
