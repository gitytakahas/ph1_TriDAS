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
#include <string>

#include "deviceDefinition.h"
#include "deviceDescription.h"

#include "ParameterDescription.h" 

/** \brief Build a device without any access to the hardware
 */
deviceDescription::deviceDescription ( enumDeviceType type, keyType accessKey, std::string fecHardwareId, tscType16 crateId ): vmeControllerDaisyChainId_(0) {

  accessKey_ = accessKey ;
  
  if ( (type == LASERDRIVER) && (getAddressKey(accessKey_) == 0x70) )
    deviceType_ = DOH ;
  else
    deviceType_ = type ;
  
  // hardware id of the FEC, not assigned
  setFecHardwareId (fecHardwareId, crateId) ;

  // device enabled by default
  enabled_ = true ;
}

/** \brief Build a device without any access to the hardware
 */
deviceDescription::deviceDescription ( enumDeviceType type,
				       tscType16 fecSlot,
				       tscType16 ringSlot,
				       tscType16 ccuAddress,
				       tscType16 channel,
				       tscType16 address,
				       std::string fecHardwareId,
				       tscType16 crateId): vmeControllerDaisyChainId_(0) {

  accessKey_ = buildCompleteKey(fecSlot,ringSlot,ccuAddress,channel,address) ;
  if ( (type == LASERDRIVER) && (getAddressKey(accessKey_) == 0x70) )
    deviceType_ = DOH ;
  else
    deviceType_ = type ;

  // hardware id of the FEC, not assigned
  setFecHardwareId (fecHardwareId, crateId) ;

  // device enabled by default
  enabled_ = true ;
}

/** \brief Build a device without any access to the hardware
 * \param type - device type
 * \param parameterNames - description of the parameters (fec, ring, ccu, channel, address)
 */
deviceDescription::deviceDescription ( enumDeviceType type,
				       parameterDescriptionNameType parameterNames ): crateId_(0), vmeControllerDaisyChainId_(0) {

  // device type
  deviceType_ = type ;

  //setCrateId ( *((tscType16 *) parameterNames[FECPARAMETERNAMES[CRATEID]]->getValueConverted()) ) ;
  setFecHardwareId ((std::string)parameterNames[FECPARAMETERNAMES[FECHARDWAREID]]->getValue(),
		    *((tscType16 *) parameterNames[FECPARAMETERNAMES[CRATEID]]->getValueConverted())) ;

  setVMEControllerDaisyChainId ( *((tscType16 *) parameterNames[FECPARAMETERNAMES[VMECONTROLLERDAISYCHAINID]]->getValueConverted()) ) ;

  accessKey_ = buildCompleteKey(*((tscType16 *) parameterNames[FECPARAMETERNAMES[FECSLOT]]->getValueConverted()),
				*((tscType16 *) parameterNames[FECPARAMETERNAMES[RINGSLOT]]->getValueConverted()),
				*((tscType16 *) parameterNames[FECPARAMETERNAMES[CCUADDRESS]]->getValueConverted()),
				*((tscType16 *) parameterNames[FECPARAMETERNAMES[I2CCHANNEL]]->getValueConverted()),
				*((tscType16 *) parameterNames[FECPARAMETERNAMES[I2CADDRESS]]->getValueConverted()));


  if ((std::string)parameterNames[FECPARAMETERNAMES[ENABLED]]->getValue() == STRFALSE) setEnabled(false) ;
  else setEnabled(true) ;

  // Tracker DOH address
  if ( (type == LASERDRIVER) && (getAddressKey(accessKey_) == 0x70) ) deviceType_ = DOH ;
}

/** Destructor
 */
deviceDescription::~deviceDescription ( ) { } ;

/** \brief Set the FEC hardware identification number
 */
void deviceDescription::setFecHardwareId ( std::string fecHardwareId, tscType16 crateId ) {

  if (fecHardwareId == "") fecHardwareId = toString((crateId_ << 16) | getFecSlot()) ;

  strncpy (fecHardwareId_, fecHardwareId.c_str(), 100) ;
  crateId_ = crateId ;
  //fecHardwareId_ = fecHardwareId ;
}

/** \brief return the FEC hardware identification number
 */
std::string deviceDescription::getFecHardwareId ( ) {

  return (fecHardwareId_) ;
}

/** \brief return the status of the device (enable, disable)
 */
bool deviceDescription::getEnabled( ) {

  return enabled_ ;
}

/** \brief return the status of the device (enable, disable)
 */
bool deviceDescription::isEnabled( ) {

  return enabled_ ;
}

/** \brief set the status of the device (enable, disable)
 */
void deviceDescription::setEnabled( bool enabled ) {

  enabled_ = enabled ;
}

/** \brief retreive the type of the current device
 * \return the device type 
 */
enumDeviceType deviceDescription::getDeviceType ( ) {

  return (deviceType_) ;
}

/** \brief set a key for this device
 * \param index - set the access key to a device
 */
void deviceDescription::setAccessKey ( keyType index ) {

  accessKey_ = index ;
}

/** \brief set the crate slot
 * \param crateId - slot of the crate to be set
 */
void deviceDescription::setCrateId ( tscType16 crateId ) {

  crateId_ = crateId ;
}

/** \brief set the VME controller daisy chain id
 * \param vmeControllerDaisyChainId - VME controller daisy chain id
 */
void deviceDescription::setVMEControllerDaisyChainId ( tscType16 vmeControllerDaisyChainId ) {

  vmeControllerDaisyChainId_ = vmeControllerDaisyChainId ;
}


/** \brief set the FEC slot
 * \param fecSlot - slot of the FEC to be set
 * \warning rebuild the key access
 */
void deviceDescription::setFecSlot ( tscType16 fecSlot ) {

  accessKey_ = ( accessKey_ & ~(MASKFECKEY << OFFFECKEY) ) | setFecSlotKey (fecSlot) ;
}

/** \brief set the ring slot
 * \param ringSlot - slot of the ring to be set
 * \warning rebuild the key access
 */
void deviceDescription::setRingSlot ( tscType16 ringSlot ) {

  accessKey_ = ( accessKey_ & ~(MASKRINGKEY << OFFRINGKEY) ) | setRingKey (ringSlot) ;
}

/** \brief set the CCU address
 * \param ccuAddress - CCU address to be set
 * \warning rebuild the key access
 */
void deviceDescription::setCcuAddress ( tscType16 ccuAddress ) {

  accessKey_ = ( accessKey_ & ~(MASKCCUKEY << OFFCCUKEY) ) | setCcuKey (ccuAddress) ;
}

/** \brief set the channel
 * \param channel - channel to be set
 * \warning rebuild the key access
 */
void deviceDescription::setChannel ( tscType16 channel ) {

  accessKey_ = ( accessKey_ & ~(MASKCHANNELKEY << OFFCHANNELKEY) ) | setChannelKey (channel) ;
}

/** \brief set the I2C address
 * \param address - address of the device to be set
 * \warning rebuild the key access
 */
void deviceDescription::setAddress ( tscType16 address ) {

  accessKey_ = ( accessKey_ & ~(MASKADDRESSKEY << OFFADDRESSKEY) ) | setAddressKey (address) ;
}

/** \brief set the device type
 * \param type - type to be set (APV25, PLL, MUX ...)
 */
void deviceDescription::setDeviceType ( enumDeviceType type ) {

  deviceType_ = type ;
}

/** \brief return the FEC slot
 * \return the FEC slot
 */
tscType16 deviceDescription::getFecSlot ( ) {

  return (getFecKey(accessKey_)) ;
}

/** \brief return the crate ID
 * \return the crate ID
 */
tscType16 deviceDescription::getCrateId ( ) {

  return (crateId_) ;
}


/** \brief get the VME controller daisy chain id
 * \return VME controller daisy chain id
 */
tscType16 deviceDescription::getVMEControllerDaisyChainId ( ) {

  return (vmeControllerDaisyChainId_) ;
}


/** \brief return the ring slot
 * \return the ring slot
 */
tscType16 deviceDescription::getRingSlot ( ) {

  return (getRingKey(accessKey_)) ;
}

/** \brief return the CCU address
 * \return the CCU address
 */
tscType16 deviceDescription::getCcuAddress ( ) {

  return (getCcuKey(accessKey_)) ;
}

/** \brief the channel on the CCU
 * \return the channel on the CCU
 */
tscType16 deviceDescription::getChannel ( ) {

  return (getChannelKey(accessKey_)) ;
}

/** \brief return the address on the channel
 * \return the address on the channel
 */
tscType16 deviceDescription::getAddress ( ) {

  return (getAddressKey(accessKey_)) ;
}

/** \brief return the key value
 * \return the device key
 */
keyType deviceDescription::getKey ( ) {

  return (accessKey_) ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *deviceDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;

  // For the rest of the parameters
  for (unsigned int i = 0 ; i < (sizeof(FECPARAMETERNAMES)/sizeof(const char *)) ; i ++) {
    (*parameterNames)[FECPARAMETERNAMES[i]] = new ParameterDescription(FECPARAMETERNAMES[i], ParameterDescription::INTEGER16) ;
  }

  (*parameterNames)[FECPARAMETERNAMES[FECHARDWAREID]]->setType(ParameterDescription::STRING) ;
  (*parameterNames)[FECPARAMETERNAMES[ENABLED]]->setType(ParameterDescription::STRING) ;

  return parameterNames ;
}

/** \brief Delete a list of parameter name but only its content
 * this method is available for any delete in any description class
 */
void deviceDescription::deleteParameterNames(parameterDescriptionNameType *parameterNames){

  for (parameterDescriptionNameType::iterator p = parameterNames->begin() ; p != parameterNames->end() ; p ++) {

    ParameterDescription *val = p->second ;
    delete val ;
  }
  parameterNames->clear() ;
}

/** Use to sort a vector of devices
 * \param r1 - device description
 * \param r2 - device description
 */
bool deviceDescription::sortByKey ( deviceDescription *r1, deviceDescription *r2 ) {

  if (r1->getKey() < r2->getKey()) return true ;
  else return false ;
}

/** Parameter's names
 */
const char *deviceDescription::FECPARAMETERNAMES[] = {"fecHardwareId","vmeControllerDaisyChainId","crateSlot","fecSlot","ringSlot","ccuAddress","i2cChannel","i2cAddress","enabled"} ;
