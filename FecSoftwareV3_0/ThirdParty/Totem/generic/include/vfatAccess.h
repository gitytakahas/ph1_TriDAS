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
based on philipsAccess made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef VFATACCESS_H
#define VFATACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "FecAccess.h"
#include "deviceAccess.h"
#include "vfatDescription.h"

/**
 * \class vfatAccess
 * This class defined a vfat and make possible the downloading or
 * uploading of the values related to a specific registers.
 * \author Juha Petajajarvi
 * \date June 2006
 * \warning all the I2C accesses are done in normal mode
 * \brief This class define all the hardware access for the vfat througth the FecAccess class
 */
class vfatAccess: public deviceAccess {

 private:

   tscType8 tmpArray[129] ;

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
  vfatAccess (FecAccess *fec, 
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
  vfatAccess (FecAccess *fec, 
                 keyType key ) ;

  /** \brief Remove the access
   */
  ~vfatAccess ( ) ;

  /** \brief set a description for future comparison
   * \param vfatValues - description values to be set
   */
  void setVfatDescription ( vfatDescription vfatValues ) ;

  /** \brief Set all values from an Vfat
   * \param vfatValues - values to be set
   */
  void setVfatValues ( vfatDescription vfatValues ) throw (FecExceptionHandler) ;

  /** \brief Set all values for a vfat in multiple frames
   * \param vfatValues - all the values for a vfat
   * \param vAccess - block of frames
   */
   void getVfatBlockWriteValues ( vfatDescription vfatValues, std::list<accessDeviceType> &vAccess ) ;  

  /** \brief Get all values from an Vfat
   */
   vfatDescription *getVfatValues ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatControl0 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatControl0 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatControl1 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatControl1 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatIPreampIn ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatIPreampIn ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatIPreampFeed ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatIPreampFeed ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatIPreampOut ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatIPreampOut ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatIShaper ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatIShaper ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatIShaperFeed ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatIShaperFeed ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatIComp ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatIComp ( tscType8 value ) throw (FecExceptionHandler) ;

  // chipid0, chipid1, upset, hitcount0, hitcount1, hitcount2 are READ ONLY

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatChipid0 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatChipid1 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatUpset ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatHitCount0 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatHitCount1 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatHitCount2 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatExtRegPointer ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatExtRegPointer ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatExtRegData ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatExtRegData ( tscType8 value ) throw (FecExceptionHandler) ;

  //Set and Get functions - EXTENDED

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatLat ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatLat ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatChanReg ( int index ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatChanReg ( int index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get all the channel registers' values
   */
  tscType8 *getVfatAllChanReg ( ) throw (FecExceptionHandler) ;

  /** \brief Set the all channel registers' values
   */
  void setVfatAllChanReg ( tscType8 *value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatVCal ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatVCal ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatVThreshold1 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatVThreshold1 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatVThreshold2 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatVThreshold2 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatCalPhase ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatCalPhase ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatControl2 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatControl2 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfatControl3 ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setVfatControl3 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Set the tmpArray value
   * \param Index - index of array
   * \param Value to be set
   */
  void setVfatArray ( int index, tscType8 value ) ;

  /** \brief static method to upload from the hardware the devices
   */
  static unsigned int getVfatValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, vfatAccess *> &vfatSet, deviceVector &vfatVector, std::list<FecExceptionHandler *> &errorList ) 
   throw (FecExceptionHandler) ;
};

#endif

