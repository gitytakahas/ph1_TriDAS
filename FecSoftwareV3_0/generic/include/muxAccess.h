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
#ifndef MUXACCESS_H
#define MUXACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "FecAccess.h"
#include "muxDescription.h"
#include "deviceAccess.h"

/**
 * \class muxAccess
 * This class defined a mux and make possible the downloading or
 * uploading of the values related to a specific register.
 * It also implements all the method specific to the laserdevice device.
 * \author Frederic Drouhin
 * \date April 2002
 * \warning all the I2C accesses are done in normal mode
 * \brief This class define all the hardware access for the mux througth the FecAccess class
 */
class muxAccess: public deviceAccess {

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  muxAccess (FecAccess *fec, 
             tscType16 Fec_slot,
             tscType16 Ring_slot,
             tscType16 Ccu_slot,
             tscType16 channelId,
             tscType16 I2C_address ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  muxAccess (FecAccess *fec, 
             keyType key ) ;

  /** \brief Remove the access
   */
  ~muxAccess () ;

  /** \brief set a description for future comparison
   */
  void setDescription ( muxDescription muxValues ) ;

  /** \brief Set all values for a Mux
   */
  void setValues ( muxDescription muxValues ) ;

  /** \brief Set all values for a mux in multiple frames
   */
  void getBlockWriteValues ( muxDescription muxValues, std::list<accessDeviceType> &vAccess ) ;

  /** \brief Set the value in the specified register
   */
  muxDescription *getValues ( ) ;

  /** \brief Set the value in the resistor register
   */
  void setResistor ( tscType16 Resistor ) ;

  /** \brief get the value from the resistor register
   */
  tscType16 getResistor ( ) ;

  /** \brief static method to upload from the hardware the devices
   */
  static unsigned int getMuxValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, muxAccess *> &muxSet, deviceVector &muxVector, 
							      std::list<FecExceptionHandler *> &errorList ) 
    throw (FecExceptionHandler) ;

};

#endif
