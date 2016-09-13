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
#ifndef DEVICEACCESS_H
#define DEVICEACCESS_H

#include "tscTypes.h"
#include "deviceFrame.h"

#include "keyType.h"

#include "deviceDescription.h"
#include "FecAccess.h"

/**
 * \class deviceAccess
 * This class manage the key for a given device on the I2C channel. Is is also an interface
 * for the i2c devices (APV, PLL, PHILIPS, LASERDRIVER, DCU, DOH)
 * \author Frederic Drouhin
 * \date April 2002
 * \brief Devices connected to an I2C channel
 */
class deviceAccess {

 protected:

  /** Access to the FEC hardware, the hardware access is done throught
   * the FecAccess class in order to manage the concurrent access to the devices
   * channels, rings, and FECs.
   */
  FecAccess *accessToFec_ ;

  /** I2C device key
   */
  keyType accessKey_ ;

  /** Type of the device
   */
  enumDeviceType deviceType_ ;

  /** Description of the values set
   */
  deviceDescription *deviceValues_ ;

  /** FEC hardware ID
   */
  std::string fecHardwareId_ ;

  /** Crate ID
   */
  tscType16 crateId_ ;

  /** Private method to be called by the constructor
   * \param deviceType - type of the device (pll, apv, ...)
   * \param fec - hardware access to the fec
   * \param key - key that contains the path to the current device
   */
  void initializeDeviceAccess ( enumDeviceType deviceType,
                                FecAccess *fec, 
                                keyType key ) {

    accessToFec_ = fec ;

    accessKey_ = accessToFec_->addi2cAccess ( key, deviceType, MODE_SHARE ) ;

    deviceType_ = deviceType ;

    deviceValues_ = NULL ;
  }

 public:

  /** \brief Constructor to store the access for this device (use when the database is not accessible)
   * \param deviceType - type of the device (pll, ...)
   * \param fec - fec hardware access
   * \param fecSlot - FEC slot
   * \param ringSlot - Ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - Device address 
   */
  deviceAccess (enumDeviceType deviceType,
                FecAccess *fec, 
                tscType16 fecSlot,
                tscType16 ringSlot,
                tscType16 ccuAddress,
                tscType16 i2cChannel,
                tscType16 i2cAddress ) {

    initializeDeviceAccess (deviceType, fec, buildCompleteKey (fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress)) ;
          
  }

  /** \brief Constructor to store the access for this device (use when the database is not accessible)
   * \param deviceType - type of the device (pll, ...)
   * \param fec - fec hardware access
   * \param key - device key
   */
  deviceAccess (enumDeviceType deviceType,
                FecAccess *fec, 
                keyType key ) {

    initializeDeviceAccess (deviceType, fec, key) ;
  }


  /** \brief Remove the access
   */
  ~deviceAccess ( ) {

    accessToFec_->removei2cAccess ( accessKey_ ) ;

    // Delete the values downloaded
    if (deviceValues_ != NULL) delete deviceValues_ ;
  }

  /** \brief set the FEC hardware id
   */
  void setFecHardwareId ( std::string fecHardwareId, tscType16 crateId ) {

    fecHardwareId_ = fecHardwareId ;
    crateId_ = crateId ;
  }

  /** \brief set the FEC hardware id
   */
  std::string getFecHardwareId ( ) {
    
    return (fecHardwareId_) ;
  }

  /** \brief set the crate ID
   */
  void setCrateId ( tscType16 crateId ) {

    crateId_ = crateId ;
  }

  /** \brief get the crate ID
   */
  tscType16 getCrateId ( ) {
    
    return crateId_ ;
  }


  /** \brief Get a key in order to retreive this device
   * \return Unique key for this device
   */
  keyType getKey ( ) {

    return (accessKey_) ;
  }

  /** \brief return the values downloaded
   * \return a device description
   */
  deviceDescription *getDownloadedValues ( ) {

    return (deviceValues_) ;
  }

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getFecSlot ( ) {return (getFecKey(accessKey_)) ;}

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getRingSlot ( ) {return (getRingKey(accessKey_)) ;}

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getCcuAddress ( ) {return (getCcuKey(accessKey_)) ;}

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getChannelId ( ) {return (getChannelKey(accessKey_)) ;}

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getI2CAddress ( ) {return (getAddressKey(accessKey_)) ;}

  /** \brief return the device type of this access
   * \return device type (APV25, PLL, ...)
   */
  enumDeviceType getDeviceType ( ) { return (deviceType_) ; }
};

#endif
