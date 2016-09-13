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

#include "muxDefinition.h"
#include "muxAccess.h"

/** Constructor to store the access to the MUX (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
muxAccess::muxAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress ) : 

  deviceAccess (APVMUX,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress ) {

}

/** Constructor to store the access to the MUX (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
muxAccess::muxAccess (FecAccess *fec, 
                      keyType key) :

  deviceAccess (APVMUX,
                fec,
                key) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
muxAccess::~muxAccess ( ) {

  // ~deviceAccess ( ) ;
}

/**
 * \param muxValues - MUX description
 */
void muxAccess::setDescription ( muxDescription muxValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = muxValues.clone() ;
}

/** Take a description value of a mux and download the values in a specific mux
 * \param muxValues - all the values for a MUX
 * \exception FecExceptionHandler
 */
void muxAccess::setValues ( muxDescription muxValues ) {

  setResistor ( muxValues.getResistor ( ) ) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = muxValues.clone() ;
}

/** Build a description of a mux and return it
 * \return the mux description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
muxDescription *muxAccess::getValues ( ) {

  muxDescription *muxValues = 
    new muxDescription (accessKey_,
                        getResistor( ) ) ;
  muxValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

  return (muxValues) ;
}

/** Set the mux resistor
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void muxAccess::setResistor ( tscType16 resistor ) {

  // ..and write it
  accessToFec_->write ( accessKey_, MUX_RES_REG, resistor ) ;
}

/** Get the value of the mux resistor
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType16 muxAccess::getResistor ( ) {

  return (accessToFec_->read ( accessKey_, MUX_RES_REG + MUX_READ)) ;
}


/** Take a description of a MUX and build all the frames (through a dedicated structure) to be sent
 * \param muxValues - all the values for an MUX
 * \param vAccess - block of frames
 */
void muxAccess::getBlockWriteValues ( muxDescription muxValues, std::list<accessDeviceType> &vAccess ) {

  // Buffer of multiple frame block transfer
  accessDeviceType resistor = { getKey(), RALMODE, MODE_WRITE, MUX_RES_REG, muxValues.getResistor(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (resistor) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = muxValues.clone() ;
}

/** This static method read out several MUX at the same time
 * \param fecAccess - hardware access
 * \param muxSet - all the MUX to be readout
 * \param muxVector - list of the readout MUX (suppose to be empty at the beginning). The muxDescription created must be delete by the remote method.
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int muxAccess::getMuxValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, muxAccess *> &muxSet, deviceVector &muxVector,
						     std::list<FecExceptionHandler *> &errorList ) 
  throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, muxDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesMux ;
  for ( Sgi::hash_map<keyType, muxAccess *>::iterator itMux = muxSet.begin() ; itMux != muxSet.end() ; itMux ++ ) {

    accessDeviceType muxResistor = { itMux->second->getKey(), RALMODE, MODE_READ, MUX_RES_REG + MUX_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesMux[getFecRingKey(itMux->second->getKey())].push_back(muxResistor) ;

    // Put it in a map and in the vector
    deviceDescriptionsMap[itMux->second->getKey()] = new muxDescription ( itMux->second->getKey() ) ;
    muxVector.push_back(deviceDescriptionsMap[itMux->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itMux->second->getKey()]->setFecHardwareId ( itMux->second->getFecHardwareId(), itMux->second->getCrateId() ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesMux, errorList ) ;

  // Collect the answer and fill the corresponding muxDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesMux.begin() ; itList != vAccessesMux.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // set the resistor of the mux
      deviceDescriptionsMap[itDevice->index]->setResistor ( data ) ;
    }
  }

  return (error) ;
}
