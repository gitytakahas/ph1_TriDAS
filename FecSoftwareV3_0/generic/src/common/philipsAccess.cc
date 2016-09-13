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
#include <iostream>

#include "tscTypes.h"

#include "philipsDefinition.h"
#include "philipsAccess.h"

/** Constructor to store the access to the APV (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
philipsAccess::philipsAccess (FecAccess *fec, 
                              tscType16 fecSlot,
                              tscType16 ringSlot,
                              tscType16 ccuAddress,
                              tscType16 i2cChannel,
                              tscType16 i2cAddress ) : 

  deviceAccess (PHILIPS,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress ) {

}

/** Constructor to store the access to the APV (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
philipsAccess::philipsAccess (FecAccess *fec, 
                              keyType key) :

  deviceAccess (PHILIPS,
                fec,
                key) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
philipsAccess::~philipsAccess ( ) {

  //~deviceAccess ( ) ;
}

/**
 * \param philipsValues - PHILIPS description
 */
void philipsAccess::setDescription ( philipsDescription philipsValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = philipsValues.clone() ;
}

/** Take a description value of a philips and download the values in a specific philips
 * \param philipsValues - all the values for an laserdriver
 * \exception FecExceptionHandler
 */
void philipsAccess::setValues ( philipsDescription philipsValues ) {

  setValue ( philipsValues.getRegister ( ) ) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = philipsValues.clone() ;
}

/** Build a description of a philips and return it
 * \return the philips description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
philipsDescription *philipsAccess::getValues ( ) {

  philipsDescription *philipsValues = 
    new philipsDescription (accessKey_,
                            getValue( ) ) ;
  philipsValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;
  
  return (philipsValues) ;
}

/** Set the philips value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void philipsAccess::setValue ( tscType8 value ) {

  // ..and write it
  accessToFec_->write ( accessKey_, value ) ;
}

/** Get the value of the philips value
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType8 philipsAccess::getValue ( ) {

  return (accessToFec_->read ( accessKey_ )) ;
}

/** Take a description of a philips and build all the frames (through a dedicated structure) to be sent
 * \param philipsValues - all the values for a philips
 * \param vAccess - block of frames
 */
void philipsAccess::getBlockWriteValues ( philipsDescription philipsValues, std::list<accessDeviceType> &vAccess ) {

  // Buffer of multiple frame block transfer
  accessDeviceType registered = { getKey(), NORMALMODE, MODE_WRITE, 0, philipsValues.getRegister (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (registered) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = philipsValues.clone() ;
}

/** This static method read out several PHILIPS at the same time
 * \param fecAccess - hardware access
 * \param philipsSet - all the PHILIPS to be readout
 * \param philipsVector - list of the readout PHILIPS (suppose to be empty at the beginning). The philipsDescription created must be delete by the remote method.
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int philipsAccess::getPhilipsValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, philipsAccess *> &philipsSet, deviceVector &philipsVector,
							     std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, philipsDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesPhilips ;
  for ( Sgi::hash_map<keyType, philipsAccess *>::iterator itPhilips = philipsSet.begin() ; itPhilips != philipsSet.end() ; itPhilips ++ ) {

    accessDeviceType philipsResistor = { itPhilips->second->getKey(), RALMODE, MODE_READ, 0, 0, false, 0, 0, 0, NULL} ;
    vAccessesPhilips[getFecRingKey(itPhilips->second->getKey())].push_back(philipsResistor) ;

    // Put it in a map and in the vector
    deviceDescriptionsMap[itPhilips->second->getKey()] = new philipsDescription ( itPhilips->second->getKey() ) ;
    philipsVector.push_back(deviceDescriptionsMap[itPhilips->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itPhilips->second->getKey()]->setFecHardwareId ( itPhilips->second->getFecHardwareId(), itPhilips->second->getCrateId() ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesPhilips, errorList ) ;

  // Collect the answer and fill the corresponding philipsDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesPhilips.begin() ; itList != vAccessesPhilips.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // set the resistor of the philips
      deviceDescriptionsMap[itDevice->index]->setRegister ( data ) ;
    }
  }

  return (error) ;
}
