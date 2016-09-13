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
#ifndef TOTEMBBACCESS_H
#define TOTEMBBACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "FecAccess.h"
#include "deviceAccess.h"
#include "totemBBDescription.h"

/**
 * \class TotemBBAccess
 * This class defined a TBB and make possible the downloading or
 * uploading of the values related to a specific registers.
 * \author Juha Petajajarvi
 * \date 2006
 * \warning all the I2C accesses are done in normal mode
 * \brief This class define all the hardware access for the TBB througth the FecAccess class
 */
class totemBBAccess: public deviceAccess {

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   * \param *fec - FEC Access object
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   */
  totemBBAccess ( FecAccess *fec, 
		  tscType16 fecSlot,
		  tscType16 ringSlot,
		  tscType16 ccuAddress,
		  tscType16 i2cChannel,
		  tscType16 i2cAddress ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   * \param *fec - FEC Access object
   * \param accessKey - key access of a device
   */
  totemBBAccess ( FecAccess *fec, 
		     keyType key ) ;

  /** \brief Remove the access
   */
  ~totemBBAccess ( ) ;

  /** \brief set a description for future comparison
   * \param tbbValues - description values to be set
   */
  void setDescription ( totemBBDescription tbbValues ) ;

  /** \brief Set all values from an TBB
   * \param tbbValues - values to be set
   */
  void setValues ( totemBBDescription tbbValues ) throw (FecExceptionHandler) ;

  /** \brief Set all values for TBB in multiple frames
   * \param tbbValues - all the values for a TBB
   * \param vAccess - block of frames
   */
   void getBlockWriteValues ( totemBBDescription tbbValues, std::list<accessDeviceType> &vAccess ) ;  

  /** \brief Get all values from an TBB
   */
  totemBBDescription *getValues ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setControl ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getLatency1 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setLatency1 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getLatency2 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setLatency2 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getRepeats1 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setRepeats1 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getRepeats2 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setRepeats2 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief static method to upload from the hardware the devices
   * \param fecAccess - hardware access
   * \param tbbSet - all the TBB to be readout
   * \param tbbVector - list of the readout TBB (suppose to be empty at the beginning). The totemBBDescription created must be delete by the remote method.
   * \param errorList - error list
   * \return number of errors
   * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
   */
  static unsigned int getValuesMultipleFrames ( FecAccess &fecAccess, 
						Sgi::hash_map<keyType, totemBBAccess *> &tbbSet,
						deviceVector &tbbVector,
						std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) ;
};

#endif

