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
#ifndef PHILIPSACCESS_H
#define PHILIPSACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "FecAccess.h"
#include "deviceAccess.h"
#include "philipsDescription.h"

/**
 * \class philipsAccess
 * This class defined a philips and make possible the downloading or
 * uploading of the values related to a specific register.
 * It also implements all the method specific to the laserdevice device.
 * \author Frederic Drouhin
 * \date April 2002
 * \warning all the I2C accesses are done in normal mode
 * \warning This class is done for an example of i2c access
 * \brief This class define all the hardware access for the philips througth the FecAccess class
 */
class philipsAccess: public deviceAccess {

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  philipsAccess (FecAccess *fec, 
                 tscType16 Fec_slot,
                 tscType16 Ring_slot,
                 tscType16 Ccu_slot,
                 tscType16 channelId,
                 tscType16 I2C_address ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  philipsAccess (FecAccess *fec, 
                 keyType key ) ;

  /** \brief Remove the access
   */
  ~philipsAccess ( ) ;

  /** \brief set a description for future comparison
   */
  void setDescription ( philipsDescription philipsValues ) ;

  /** \brief Set all values from an Philips
   */
  void setValues ( philipsDescription philipsValues ) ;

  /** \brief Set all values for a laserdriver in multiple frames
   */
  void getBlockWriteValues ( philipsDescription philipsValues, std::list<accessDeviceType> &vAccess ) ;  

  /** \brief Get all values from an Philips
   */
  philipsDescription *getValues ( ) ;

  /** \brief Set the value in the specified register
   */
  void setValue ( tscType8 Value ) ;

  /** \brief Get the value from the specified register
   */
  tscType8 getValue ( ) ;

  /** \brief static method to upload from the hardware the devices
   */
  static unsigned int getPhilipsValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, philipsAccess *> &philipsSet, deviceVector &philipsVector,
						       std::list<FecExceptionHandler *> &errorList ) 
    throw (FecExceptionHandler) ;

};

#endif

