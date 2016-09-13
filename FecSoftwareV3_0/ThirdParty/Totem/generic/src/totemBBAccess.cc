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

#include <iostream>

#include "tscTypes.h"
#include "totemBBDefinition.h"
#include "totemBBAccess.h"

/** Constructor to store the access to the TBB (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
totemBBAccess::totemBBAccess ( FecAccess *fec, 
			       tscType16 fecSlot,
			       tscType16 ringSlot,
			       tscType16 ccuAddress,
			       tscType16 i2cChannel,
			       tscType16 i2cAddress ) :

  deviceAccess ( TBB,
                 fec, 
                 fecSlot, 
                 ringSlot, 
                 ccuAddress,
                 i2cChannel, 
                 i2cAddress ) {

}

/** Constructor to store the access to the TBB (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
totemBBAccess::totemBBAccess ( FecAccess *fec, 
			       keyType key ) :

  deviceAccess ( TBB,
                 fec,
                 key ) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
totemBBAccess::~totemBBAccess ( ) {

  //~deviceAccess ( ) ;
}

/**
 * \param tbbValues - TBB description
 */
void totemBBAccess::setDescription ( totemBBDescription tbbValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) { delete deviceValues_ ; }
  deviceValues_ = tbbValues.clone() ;
}

/** Take a description value of a TBB and download the values in a specific TBB
 * \param tbbValues - all the TBB values
 * \exception FecExceptionHandler
 */
void totemBBAccess::setValues ( totemBBDescription tbbValues ) throw (FecExceptionHandler) {

  setControl  (tbbValues.getControl ( )) ;
  setLatency1 (tbbValues.getLatency1 ( )) ;
  setLatency2 (tbbValues.getLatency2 ( )) ;
  setRepeats1 (tbbValues.getRepeats1 ( )) ;
  setRepeats2 (tbbValues.getRepeats2 ( )) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) { delete deviceValues_ ; }
  deviceValues_ = tbbValues.clone() ;
}

/** Build a description of a TBB and return it
 * \return the TBB description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not forget to delete it.
 */
totemBBDescription *totemBBAccess::getValues ( ) throw (FecExceptionHandler) {

  totemBBDescription *tbbValues = 
    new totemBBDescription ( accessKey_,
			     getControl (),
			     getLatency1 (),
			     getLatency2 (),
			     getRepeats1 (),
			     getRepeats2 () ) ;

  tbbValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;
  
  return (tbbValues) ;
}

/** Get TBB register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemBBAccess::getControl ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, TBB_CONTROL)) ; }

/** Set the TBB register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemBBAccess::setControl ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, TBB_CONTROL, value ) ; }

/** Get TBB register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemBBAccess::getLatency1 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, TBB_LATENCY1)) ; }

/** Set the TBB register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemBBAccess::setLatency1 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, TBB_LATENCY1, value ) ; }

/** Get TBB register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemBBAccess::getLatency2 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, TBB_LATENCY2)) ; }

/** Set the TBB register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemBBAccess::setLatency2 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, TBB_LATENCY2, value ) ; }

/** Get TBB register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemBBAccess::getRepeats1 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, TBB_REPEATS1)) ; }

/** Set the TBB register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemBBAccess::setRepeats1 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, TBB_REPEATS1, value ) ; }

/** Get TBB register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemBBAccess::getRepeats2 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, TBB_REPEATS2)) ; }

/** Set the TBB register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemBBAccess::setRepeats2 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, TBB_REPEATS2, value ) ; }

/** Take a description of a TBB and build all the frames (through a dedicated structure) to be sent
 * \param tbbValues - all the values for a TBB
 * \param vAccess - block of frames
 */
void totemBBAccess::getBlockWriteValues ( totemBBDescription tbbValues, std::list<accessDeviceType> &vAccess ) {

  // Buffer of multiple frame block transfer

  accessDeviceType control = { getKey(), NORMALMODE, MODE_WRITE, 0, tbbValues.getControl(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control) ;

  accessDeviceType lat1 = { getKey(), NORMALMODE, MODE_WRITE, 0, tbbValues.getLatency1(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (lat1) ;

  accessDeviceType lat2 = { getKey(), NORMALMODE, MODE_WRITE, 0, tbbValues.getLatency2(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (lat2) ;

  accessDeviceType rep1 = { getKey(), NORMALMODE, MODE_WRITE, 0, tbbValues.getRepeats1(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (rep1) ;

  accessDeviceType rep2 = { getKey(), NORMALMODE, MODE_WRITE, 0, tbbValues.getRepeats2(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (rep2) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL){ delete deviceValues_ ; }
  deviceValues_ = tbbValues.clone() ;
}

/** This static method read out several TBBs at the same time
 * \param fecAccess - hardware access
 * \param tbbSet - all the TBBs to be readout
 * \param tbbVector - list of the readout TBBs (suppose to be empty at the beginning). The totemBBDescription created must be delete by the remote method.
 * \param errorList - error list
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int totemBBAccess::getValuesMultipleFrames ( FecAccess &fecAccess, 
						      Sgi::hash_map<keyType, totemBBAccess *> &tbbSet,
						      deviceVector &tbbVector,
						      std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, totemBBDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesTbb ;
  for ( Sgi::hash_map<keyType, totemBBAccess *>::iterator itTbb = tbbSet.begin() ; itTbb != tbbSet.end() ; itTbb ++ ) {

    accessDeviceType control = { itTbb->second->getKey(), NORMALMODE, MODE_READ, TBB_CONTROL, 0, false, 0, 0, 0, NULL} ;
    vAccessesTbb[getFecRingKey(itTbb->second->getKey())].push_back(control) ;

    accessDeviceType lat1 = { itTbb->second->getKey(), NORMALMODE, MODE_READ, TBB_LATENCY1, 0, false, 0, 0, 0, NULL} ;
    vAccessesTbb[getFecRingKey(itTbb->second->getKey())].push_back(lat1) ;

    accessDeviceType lat2 = { itTbb->second->getKey(), NORMALMODE, MODE_READ, TBB_LATENCY2, 0, false, 0, 0, 0, NULL} ;
    vAccessesTbb[getFecRingKey(itTbb->second->getKey())].push_back(lat2) ;

    accessDeviceType rep1 = { itTbb->second->getKey(), NORMALMODE, MODE_READ, TBB_REPEATS1, 0, false, 0, 0, 0, NULL} ;
    vAccessesTbb[getFecRingKey(itTbb->second->getKey())].push_back(rep1) ;

    accessDeviceType rep2 = { itTbb->second->getKey(), NORMALMODE, MODE_READ, TBB_REPEATS2, 0, false, 0, 0, 0, NULL} ;
    vAccessesTbb[getFecRingKey(itTbb->second->getKey())].push_back(rep2) ;

    // Put it in a map and in the vector
    deviceDescriptionsMap[itTbb->second->getKey()] = new totemBBDescription ( itTbb->second->getKey() ) ;
    tbbVector.push_back(deviceDescriptionsMap[itTbb->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itTbb->second->getKey()]->setFecHardwareId ( itTbb->second->getFecHardwareId(), itTbb->second->getCrateId() ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesTbb, errorList ) ;

  // Collect the answer and fill the corresponding totemBBDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesTbb.begin() ; itList != vAccessesTbb.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      switch (itDevice->offset) {
      case TBB_CONTROL:
	deviceDescriptionsMap[itDevice->index]->setControl(data) ;
	break ;
      case TBB_LATENCY1:
	deviceDescriptionsMap[itDevice->index]->setLatency1(data) ;
	break ;
      case TBB_LATENCY2:
	deviceDescriptionsMap[itDevice->index]->setLatency2(data) ;
	break ;
      case TBB_REPEATS1:
	deviceDescriptionsMap[itDevice->index]->setRepeats1(data) ;
	break ;
      case TBB_REPEATS2:
	deviceDescriptionsMap[itDevice->index]->setRepeats2(data) ;
	break ;      
      }
    }
  }
  return (error) ;
}
