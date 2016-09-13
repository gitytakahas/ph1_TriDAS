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
#ifndef TOTEMCCHIPACCESS_H
#define TOTEMCCHIPACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "FecAccess.h"
#include "deviceAccess.h"
#include "totemCChipDescription.h"

/**
 * \class TotemCChipAccess
 * This class defined a cc and make possible the downloading or
 * uploading of the values related to a specific registers.
 * \author Juha Petajajarvi
 * \date 2006
 * \warning all the I2C accesses are done in normal mode
 * \brief This class define all the hardware access for the cc througth the FecAccess class
 */
class totemCChipAccess: public deviceAccess {

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
  totemCChipAccess ( FecAccess *fec, 
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
  totemCChipAccess ( FecAccess *fec, 
		     keyType key ) ;

  /** \brief Remove the access
   */
  ~totemCChipAccess ( ) ;

  /** \brief set a description for future comparison
   * \param ccValues - description values to be set
   */
  void setDescription ( totemCChipDescription ccValues ) ;

  /** \brief Set all values from an CC
   * \param ccValues - values to be set
   */
  void setValues ( totemCChipDescription ccValues ) throw (FecExceptionHandler) ;

  /** \brief Set all values for CC in multiple frames
   * \param ccValues - all the values for a CC
   * \param vAccess - block of frames
   */
   void getBlockWriteValues ( totemCChipDescription ccValues, std::list<accessDeviceType> &vAccess ) ;  

  /** \brief Get all values from an Cc
   */
  totemCChipDescription *getValues ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl0 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setControl0 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl1 ( ) throw (FecExceptionHandler);

  /** \brief Set the value in the specified register
   */
  void setControl1 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl2 ( ) throw (FecExceptionHandler);

  /** \brief Set the value in the specified register
   */
  void setControl2 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl3 ( ) throw (FecExceptionHandler);

  /** \brief Set the value in the specified register
   */
  void setControl3 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl4 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setControl4 ( tscType8 value ) throw (FecExceptionHandler) ;

  // Chip id and Counters 0-2 are READ ONLY

  /** \brief Get the value in the specified register
   */
  tscType8 getChipid0 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getChipid1 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getCounter0 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getCounter1 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getCounter2 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask0 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask0 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask1 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask1 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask2 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask2 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask3 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask3 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getExtRegPointer ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setExtRegPointer ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getExtRegData ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setExtRegData ( tscType8 value ) throw (FecExceptionHandler) ;

  // Set and Get functions - EXTENDED

  /** \brief Get the value in the specified register
   */
  tscType8 getMask4 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask4 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask5 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask5 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask6 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask6 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask7 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask7 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask8 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask8 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMask9 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setMask9 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getRes ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setRes ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief static method to upload from the hardware the devices
   * \param fecAccess - hardware access
   * \param ccSet - all the CHIPs to be readout
   * \param ccVector - list of the readout CHIPs (suppose to be empty at the beginning). The totemCChipDescription created must be delete by the remote method.
   * \param errorList - error list
   * \return number of errors
   * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
   */
  static unsigned int getValuesMultipleFrames ( FecAccess &fecAccess,
						Sgi::hash_map<keyType, totemCChipAccess *> &ccSet,
						deviceVector &ccVector,
						std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) ;

};

#endif

