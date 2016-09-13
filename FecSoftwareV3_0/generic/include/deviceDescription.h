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
#ifndef DEVICEDESCRIPTION_H
#define DEVICEDESCRIPTION_H

#include <string>

#include "deviceDefinition.h"
#include "keyType.h"

#include "hashMapDefinition.h" // For hash_map
#include "ParameterDescription.h" 

/** Generic device description
 */
class deviceDescription {

 protected:

  /** Device type
   */
  enumDeviceType deviceType_ ;

  /** Crate slot
   */
  tscType16 crateId_ ;

  /** key access of the device
   */
  keyType accessKey_ ;

  /** VME FEC has identification number inside the hardware.
   */
  char fecHardwareId_[100] ;

  /** Enable: the modules/devices can be disabled in the database. The
   * database will return all the devices including the disable one, then
   * it is up to the C++ accesses (FecAccessManager) in the case of Tracker
   * to not used in case of disabling).
   */
  bool enabled_ ;

  /** To have daisy chain in the crate
   */
  tscType16 vmeControllerDaisyChainId_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum DeviceEnumType {FECHARDWAREID,VMECONTROLLERDAISYCHAINID,CRATEID,FECSLOT,RINGSLOT,CCUADDRESS,I2CCHANNEL,I2CADDRESS,ENABLED} ;

  /** Parameter's names
   */
  static const char *FECPARAMETERNAMES[ENABLED+1] ; // = {"fecHardwareId", "vmeControllerDaisyChainId", "crateId", "fecSlot", "ringSlot", "ccuAddress", "i2cChannel", "i2cAddress","enabled"} ;

  /** \brief Build a device without any access to the hardware
   */
  deviceDescription ( enumDeviceType type, keyType accessKey = 0, std::string fecHardwareId = "0", tscType16 crateId = 0 ) ;

  /** \brief Build a device without any access to the hardware
   */
  deviceDescription ( enumDeviceType type,
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 channel,
                      tscType16 address,
		      std::string fecHardwareId = "0",
		      tscType16 crateId = 0 ) ;

  /** \brief Build a device without any access to the hardware
   * \param type - device type
   * \param parameterNames - description of the parameters (fec, ring, ccu, channel, address)
   */
  deviceDescription ( enumDeviceType type,
		      parameterDescriptionNameType parameterNames ) ;

  /** Destructor
   */
  virtual ~deviceDescription ( ) ;

  /** \brief Set the FEC hardware identification number
   */
  void setFecHardwareId ( std::string fecHardwareId, tscType16 crateId ) ;

  /** \brief return the FEC hardware identification number
   */
  std::string getFecHardwareId ( ) ;

  /** \brief return the status of the device (enable, disable)
   */
  bool getEnabled( ) ;

  /** \brief return the status of the device (enable, disable)
   */
  bool isEnabled( ) ;

  /** \brief set the status of the device (enable, disable)
   */
  void setEnabled( bool enabled ) ;

  /** \brief retreive the type of the current device
   * \return the device type 
   */
  enumDeviceType getDeviceType ( ) ;

  /** \brief set a key for this device
   * \param index - set the access key to a device
   */
  void setAccessKey ( keyType index ) ;

  /** \brief set the crate ID
   * \param fecSlot - ID of the FEC to be set
   */
  void setCrateId ( tscType16 crateId ) ;

  /** \brief set the crate ID
   * \param fecSlot - ID of the FEC to be set
   */
  inline void setCrateSlot ( tscType16 crateId ) {

    //#warning "setCrateSlot DEPRECATED> use getCrateId instead"
    std::cerr << "--------------------> " << __PRETTY_FUNCTION__ << " DEPRECATED> use getCrateId" << std::endl ;
    crateId_ = crateId ;
  }

  /** \brief set the VME controller daisy chain id
   */
  void setVMEControllerDaisyChainId ( tscType16 vmeControllerDaisyChainId ) ;

  /** \brief set the FEC slot
   * \param fecSlot - slot of the FEC to be set
   * \warning rebuild the key access
   */
  void setFecSlot ( tscType16 fecSlot ) ;

  /** \brief set the ring slot
   * \param ringSlot - slot of the ring to be set
   * \warning rebuild the key access
   */
  void setRingSlot ( tscType16 ringSlot ) ;

  /** \brief set the CCU address
   * \param ccuAddress - CCU address to be set
   * \warning rebuild the key access
   */
  void setCcuAddress ( tscType16 ccuAddress ) ;

  /** \brief set the channel
   * \param channel - channel to be set
   * \warning rebuild the key access
   */
  void setChannel ( tscType16 channel ) ;

  /** \brief set the I2C address
   * \param address - address of the device to be set
   * \warning rebuild the key access
   */
  void setAddress ( tscType16 address ) ;

  /** \brief set the device type
   * \param type - type to be set (APV25, PLL, MUX ...)
   */
  void setDeviceType ( enumDeviceType type ) ;

  /** \brief return the crate ID
   * \return the crate ID
   */
  tscType16 getCrateId ( ) ;

  /** \brief return the crate ID
   * \return the FEC slot
   */
  inline tscType16 getCrateSlot ( ) {

    //#warning "setCrateSlot DEPRECATED> use getCrateId instead"
    std::cerr << "--------------------> " << __PRETTY_FUNCTION__ << " DEPRECATED> use getCrateId" << std::endl ;
    return getCrateId() ;
  }

  /** \brief return the VME controller daisy chain id
   */
  tscType16 getVMEControllerDaisyChainId ( ) ;

  /** \brief return the FEC slot
   * \return the FEC slot
   */
  tscType16 getFecSlot ( ) ;

  /** \brief return the ring slot
   * \return the ring slot
   */
  tscType16 getRingSlot ( ) ;

  /** \brief return the CCU address
   * \return the CCU address
   */
  tscType16 getCcuAddress ( ) ;

  /** \brief the channel on the CCU
   * \return the channel on the CCU
   */
  tscType16 getChannel ( ) ;

  /** \brief return the address on the channel
   * \return the address on the channel
   */
  tscType16 getAddress ( ) ;

  /** \brief return the key value
   * \return the device key
   */
  keyType getKey ( ) ;

  /** \brief virtual method to clone a device
   */
  virtual deviceDescription *clone ( ) = 0 ;

  /** \brief virtual method for display device values
   */
  virtual void display ( ) = 0 ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

  /** \brief Delete a list of parameter name but only its content
   * this method is available for any delete in any description class
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) ;

  /** \brief compare two descriptions and sort it
   */
  static bool sortByKey ( deviceDescription *r1, deviceDescription *r2 ) ;
};

#endif
