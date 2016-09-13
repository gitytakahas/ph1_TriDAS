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

#include "laserdriverDefinition.h"
#include "laserdriverAccess.h"

/** This function make a bitswap and return it
 * \param value - ...
 * \return the bitswaped value
 */
tscType8 bitswap(tscType8 value) { 

  // bitswap for data is only necessary for this version of dcu
  int i = sizeof( value );

  unsigned char resultout = 0, valuein = value; 

  for ( int lc=0 ; lc < 8 * i ; lc ++) {  

    resultout <<=1;
    if ( (0x1 & valuein)) {
      resultout+=1 ;
    }
    valuein >>=1; 
  }

  return (resultout) ;
}

/** Constructor to store the access to the APV (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
laserdriverAccess::laserdriverAccess (FecAccess *fec, 
                                      tscType16 fecSlot,
                                      tscType16 ringSlot,
                                      tscType16 ccuAddress,
                                      tscType16 i2cChannel,
                                      tscType16 i2cAddress ) : 

  deviceAccess (LASERDRIVER,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress ) {

}

/** Constructor to store the access to the laserdriver (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
laserdriverAccess::laserdriverAccess (FecAccess *fec, 
                                      keyType key) :

  deviceAccess (LASERDRIVER,
                fec,
                key) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
laserdriverAccess::~laserdriverAccess ( ) {

  // ~deviceAccess ( ) ;
}

/** 
 * \param laserdriverValues - description of the laserdriver 
 */
void laserdriverAccess::setDescription ( laserdriverDescription laserdriverValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = laserdriverValues.clone() ;
}


/** Take a description value of a laserdriver and download the values in a specific laserdriver
 * \param laserdriverValues - all the values for a laserdriver
 * \exception FecExceptionHandler
 */
void laserdriverAccess::setValues ( laserdriverDescription laserdriverValues ) throw (FecExceptionHandler) {

  tscType8 bias[MAXLASERDRIVERCHANNELS] ;

  laserdriverValues.getBias ( bias ) ;
  setBias ( bias ) ;

  setGain ( laserdriverValues.getGain ( ) ) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = laserdriverValues.clone() ;
}

/** Build a description of a laserdriver and return it
 * \return the laserdriver description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
laserdriverDescription *laserdriverAccess::getValues ( ) throw (FecExceptionHandler) {

  // Retreive the bias value for all the channels
  tscType8 bias[MAXLASERDRIVERCHANNELS] ;
  getBias (bias) ;

  laserdriverDescription *laserdriverValues = 
    new laserdriverDescription (accessKey_,
                                getGain( ), 
                                bias) ;
  laserdriverValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

  return (laserdriverValues) ;
}

/** Set the laserdriver gain
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void laserdriverAccess::setGain ( tscType8 gain ) throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR

  char pos[100] ;
  decodeKey (pos, getKey()) ;

  std::cout << pos << " Gain 0x" << std::hex << gain << std::dec << std::endl ;

#endif

  // Write it
  accessToFec_->writeOffset ( accessKey_, GAINSELECTION, gain ) ;
}

/** Get laserdriver gain
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 laserdriverAccess::getGain ( ) throw (FecExceptionHandler) {

  return (accessToFec_->readOffset ( accessKey_, GAINSELECTION ) ) ;
}

/** Set the laserdriver bias for all the laserdriver channels
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void laserdriverAccess::setBias ( tscType8 *bias ) throw (FecExceptionHandler) {

  // Write it
  for (tscType8 i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {

#ifdef DEBUGMSGERROR
    char pos[100] ;
    decodeKey (pos, getKey()) ;
    std::cout << pos << " Bias chan " << std::dec << i << ": " << bias[i] << std::hex << " (0x" << bias[i] << ")" << std::endl ;
#endif

    accessToFec_->writeOffset ( accessKey_, i, bias[i] ) ;
  }
}

/** Get all the laserdriver bias channels
 * \param bias - pointer to an array of values
 * \exception FecExceptionHandler
 */
void laserdriverAccess::getBias ( tscType8 *bias ) throw (FecExceptionHandler) {

  for (tscType8 i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {

    bias[i] = accessToFec_->readOffset ( accessKey_, i )  ;
  }
}

/** \return true if as SEU is detected ( bit 8 of gainReg = 1 )
 */
bool laserdriverAccess::getSeuStatus() throw (FecExceptionHandler) { 

  return ((bool) ( getGain() & LASERDRIVERSEUDETECTED )) ;
}

/** Take a description of a laserdriver and build all the frames (through a dedicated structure) to be sent
 * \param laserdriverValues - all the values for a laserdriver
 * \param vAccess - block of frames
 */
void laserdriverAccess::getBlockWriteValues ( laserdriverDescription laserdriverValues, std::list<accessDeviceType> &vAccess ) {

  // Buffer of multiple frame block transfer
  accessDeviceType gainR = { getKey(), NORMALMODE, MODE_WRITE, GAINSELECTION, laserdriverValues.getGain (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (gainR) ;

  // Write it
  tscType8 bias[MAXLASERDRIVERCHANNELS] ;
  laserdriverValues.getBias ( bias ) ;
  for (tscType8 i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    accessDeviceType biasR = { getKey(), NORMALMODE, MODE_WRITE, i, bias[i], false, 0, 0, 0, NULL} ;
    vAccess.push_back (biasR) ;
  }

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = laserdriverValues.clone() ;
}

/** This static method read out several LASERDRIVER at the same time
 * \param fecAccess - hardware access
 * \param laserdriverSet - all the LASERDRIVER to be readout
 * \param laserdriverVector - list of the readout LASERDRIVER (suppose to be empty at the beginning). The laserdriverDescription created must be delete by the remote method.
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int laserdriverAccess::getLaserdriverValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, laserdriverAccess *> &laserdriverSet, deviceVector &laserdriverVector,
								      std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, laserdriverDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesLaserdriver ;
  for ( Sgi::hash_map<keyType, laserdriverAccess *>::iterator itLaserdriver = laserdriverSet.begin() ; itLaserdriver != laserdriverSet.end() ; itLaserdriver ++ ) {

    accessDeviceType gainR = { itLaserdriver->second->getKey(), NORMALMODE, MODE_READ, GAINSELECTION, 0, false, 0, 0, 0, NULL} ;
    vAccessesLaserdriver[getFecRingKey(itLaserdriver->second->getKey())].push_back(gainR) ;

    for (tscType8 i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
      accessDeviceType biasR = { itLaserdriver->second->getKey(), NORMALMODE, MODE_READ, i, 0, false, 0, 0, 0, NULL} ;
      vAccessesLaserdriver[getFecRingKey(itLaserdriver->second->getKey())].push_back(biasR) ;
    }

    // Put it in a map and in the vector
    deviceDescriptionsMap[itLaserdriver->second->getKey()] = new laserdriverDescription ( itLaserdriver->second->getKey() ) ;
    laserdriverVector.push_back(deviceDescriptionsMap[itLaserdriver->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itLaserdriver->second->getKey()]->setFecHardwareId ( itLaserdriver->second->getFecHardwareId(), itLaserdriver->second->getCrateId() ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesLaserdriver, errorList ) ;

  // Collect the answer and fill the corresponding laserdriverDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesLaserdriver.begin() ; itList != vAccessesLaserdriver.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF;
      if (itDevice->e != NULL) data = 0xFF;
      else data = itDevice->data ;

      if (itDevice->offset == GAINSELECTION) 
	deviceDescriptionsMap[itDevice->index]->setGain ( data ) ;
      else 
	deviceDescriptionsMap[itDevice->index]->setBias (itDevice->offset, data) ;
    }
  }

  return (error) ;
}
