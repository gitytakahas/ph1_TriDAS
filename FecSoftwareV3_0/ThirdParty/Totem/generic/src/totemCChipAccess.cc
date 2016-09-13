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
#include "totemCChipDefinition.h"
#include "totemCChipAccess.h"

/** Constructor to store the access to the CHIP (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
totemCChipAccess::totemCChipAccess ( FecAccess *fec, 
				     tscType16 fecSlot,
				     tscType16 ringSlot,
				     tscType16 ccuAddress,
				     tscType16 i2cChannel,
				     tscType16 i2cAddress ) : 

  deviceAccess ( CCHIP,
                 fec, 
                 fecSlot, 
                 ringSlot, 
                 ccuAddress,
                 i2cChannel, 
                 i2cAddress ) {

}

/** Constructor to store the access to the CCHIP (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
totemCChipAccess::totemCChipAccess ( FecAccess *fec, 
                     keyType key ) :

  deviceAccess ( CCHIP,
                 fec,
                 key ) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
totemCChipAccess::~totemCChipAccess ( ) {

  //~deviceAccess ( ) ;
}

/**
 * \param ccValues - CHIP description
 */
void totemCChipAccess::setDescription ( totemCChipDescription ccValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) { delete deviceValues_ ; }
  deviceValues_ = ccValues.clone() ;
}

/** Take a description value of a cchip and download the values in a specific cchip
 * \param ccValues - all the cc values
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setValues ( totemCChipDescription ccValues ) throw (FecExceptionHandler) {

  setControl1 ( ccValues.getControl1 ( ) ) ;
  setControl2 ( ccValues.getControl2 ( ) ) ;
  setControl3 ( ccValues.getControl3 ( ) ) ;
  setControl4 ( ccValues.getControl4 ( ) ) ;

  setMask0 ( ccValues.getMask0 ( ) ) ;
  setMask1 ( ccValues.getMask1 ( ) ) ;
  setMask2 ( ccValues.getMask2 ( ) ) ;
  setMask3 ( ccValues.getMask3 ( ) ) ;
  setMask4 ( ccValues.getMask4 ( ) ) ;
  setMask5 ( ccValues.getMask5 ( ) ) ;
  setMask6 ( ccValues.getMask6 ( ) ) ;
  setMask7 ( ccValues.getMask7 ( ) ) ;
  setMask8 ( ccValues.getMask8 ( ) ) ;
  setMask9 ( ccValues.getMask9 ( ) ) ;
  setRes   ( ccValues.getRes ( ) ) ;
  setControl0 ( ccValues.getControl0 ( ) ) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) { delete deviceValues_ ; }
  deviceValues_ = ccValues.clone() ;
}

/** Build a description of a cchip and return it
 * \return the cc description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not forget to delete it.
 */
totemCChipDescription *totemCChipAccess::getValues ( ) throw (FecExceptionHandler) {

  totemCChipDescription *ccValues = 
    new totemCChipDescription ( accessKey_,
				getControl0 ( ),
				getControl1 ( ),
				getControl2 ( ),
				getControl3 ( ),
				getControl4 ( ),
				getChipid0 ( ),
				getChipid1 ( ),
				getCounter0 ( ),
				getCounter1 ( ),
				getCounter2 ( ),

				getMask0 ( ),
				getMask1 ( ),
				getMask2 ( ),
				getMask3 ( ),
				getMask4 ( ),
				getMask5 ( ),
				getMask6 ( ),
				getMask7 ( ),
				getMask8 ( ),
				getMask9 ( ),
				getRes ( ) ) ;

  ccValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;
  
  return (ccValues) ;
}

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getControl0 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_CONTROL0)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setControl0 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_CONTROL0, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getControl1 ( )  throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_CONTROL1)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setControl1 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_CONTROL1, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getControl2 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_CONTROL2)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setControl2 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_CONTROL2, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getControl3 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_CONTROL3)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setControl3 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_CONTROL3, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getControl4 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_CONTROL4)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setControl4 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_CONTROL4, value ) ; }

// chipid0, chipid1, upset, hitcount0, hitcount1, hitcount2 are READ ONLY

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getChipid0 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_CHIPID0)) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getChipid1 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_CHIPID1)) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getCounter0 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_COUNTER0)) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getCounter1 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_COUNTER1)) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getCounter2 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_COUNTER2)) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask0 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_MASK0)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask0 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_MASK0, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask1 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_MASK1)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask1 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_MASK1, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask2 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_MASK2)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask2 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_MASK2, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask3 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_MASK3)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask3 ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_MASK3, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getExtRegPointer ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_EXTREGPOINTER)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setExtRegPointer( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, value ) ; }

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getExtRegData ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, CCHIP_EXTREGDATA)) ; }

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setExtRegData( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGDATA, value ) ; }

// Set and Get functions - EXTENDED

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask4 ( ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK4 ) ; 
  return ( accessToFec_->readOffset ( accessKey_, CCHIP_EXTREGDATA ) );
}

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask4( tscType8 value ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK4 ) ; 
  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGDATA, value ) ;
}

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask5 ( ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK5 ) ; 
  return ( accessToFec_->readOffset ( accessKey_, CCHIP_EXTREGDATA ) );
}

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask5  ( tscType8 value ) throw (FecExceptionHandler){
  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK5 ) ; 
  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGDATA, value ) ;
}

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask6 ( ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK6 ) ; 
  return ( accessToFec_->readOffset ( accessKey_, CCHIP_EXTREGDATA ) );
}

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask6( tscType8 value ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK6 ) ; 
  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGDATA, value ) ;
}

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask7 ( ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK7 ) ; 
  return ( accessToFec_->readOffset ( accessKey_, CCHIP_EXTREGDATA ) );
}

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask7( tscType8 value ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK7 ) ; 
  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGDATA, value ) ;
}

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask8 ( ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK8 ) ; 
  return ( accessToFec_->readOffset ( accessKey_, CCHIP_EXTREGDATA ) );
}

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask8( tscType8 value ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK8 ) ; 
  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGDATA, value ) ;
}

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getMask9 ( ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK9 ) ; 
  return ( accessToFec_->readOffset ( accessKey_, CCHIP_EXTREGDATA ) );
}

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setMask9( tscType8 value ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_MASK9 ) ; 
  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGDATA, value ) ;
}

/** Get CHIP register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 totemCChipAccess::getRes ( ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_RES ) ; 
  return ( accessToFec_->readOffset ( accessKey_, CCHIP_EXTREGDATA ) );
}

/** Set the CHIP register value
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void totemCChipAccess::setRes( tscType8 value ) throw (FecExceptionHandler){

  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGPOINTER, CCHIP_RES ) ; 
  accessToFec_->writeOffset ( accessKey_, CCHIP_EXTREGDATA, value ) ;
}

/** Take a description of a CHIP and build all the frames (through a dedicated structure) to be sent
 * \param ccValues - all the values for a CHIP
 * \param vAccess - block of frames
 */
void totemCChipAccess::getBlockWriteValues ( totemCChipDescription ccValues, std::list<accessDeviceType> &vAccess ) {

  // Buffer of multiple frame block transfer

  accessDeviceType control1 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getControl1(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control1) ;

  accessDeviceType control2 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getControl2(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control2) ;

  accessDeviceType control3 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getControl3(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control3) ;

  accessDeviceType control4 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getControl4(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control4) ;

  accessDeviceType mask0 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask0(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask0) ;

  accessDeviceType mask1 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask1(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask1) ;

  accessDeviceType mask2 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask2(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask2) ;

  accessDeviceType mask3 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask3(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask3) ;

  accessDeviceType mask4pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask4pointer) ;
  accessDeviceType mask4 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask4(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask4) ;

  accessDeviceType mask5pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask5pointer) ;
  accessDeviceType mask5 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask5(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask5) ;

  accessDeviceType mask6pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask6pointer) ;
  accessDeviceType mask6 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask6(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask6) ;

  accessDeviceType mask7pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask7pointer) ;
  accessDeviceType mask7 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask7(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask7) ;

  accessDeviceType mask8pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask8pointer) ;
  accessDeviceType mask8 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask8(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask8) ;

  accessDeviceType mask9pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask9pointer) ;
  accessDeviceType mask9 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getMask9(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (mask9) ;

  accessDeviceType respointer = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (respointer) ;
  accessDeviceType res = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getRes(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (res) ;

  accessDeviceType control0 = { getKey(), NORMALMODE, MODE_WRITE, 0, ccValues.getControl0(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control0) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL){ delete deviceValues_ ; }
  deviceValues_ = ccValues.clone() ;
}

/** This static method read out several CHIPs at the same time
 * \param fecAccess - hardware access
 * \param ccSet - all the CHIP to be readout
 * \param ccVector - list of the readout CHIPs (suppose to be empty at the beginning). The totemCChipDescription created must be delete by the remote method.
 * \param errorList - error list
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int totemCChipAccess::getValuesMultipleFrames ( FecAccess &fecAccess,
							 Sgi::hash_map<keyType, totemCChipAccess *> &ccSet,
							 deviceVector &ccVector,
							 std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, totemCChipDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // Data of Extended register?
  bool edata = false ;
  tscType8 channel = 0x00 ;

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesCc ;
  for ( Sgi::hash_map<keyType, totemCChipAccess *>::iterator itCc = ccSet.begin() ; itCc != ccSet.end() ; itCc ++ ) {

    accessDeviceType control0 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_CONTROL0, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(control0) ;

    accessDeviceType control1 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_CONTROL1, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(control1) ;

    accessDeviceType control2 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_CONTROL2, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(control2) ;

    accessDeviceType control3 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_CONTROL3, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(control3) ;

    accessDeviceType control4 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_CONTROL4, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(control4) ;

    accessDeviceType chipid0 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_CHIPID0, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(chipid0) ;

    accessDeviceType chipid1 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_CHIPID1, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(chipid1) ;

    accessDeviceType counter0 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_COUNTER0, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(counter0) ;

    accessDeviceType counter1 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_COUNTER1, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(counter1) ;

    accessDeviceType counter2 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_COUNTER2, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(counter2) ;

    accessDeviceType mask0 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK0, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask0) ;

    accessDeviceType mask1 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK0, 1, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask1) ;

    accessDeviceType mask2 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK0, 2, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask2) ;

    accessDeviceType mask3 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK0, 3, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask3) ;

    accessDeviceType mask4pointer = { itCc->second->getKey(), NORMALMODE, MODE_WRITE, 0, CCHIP_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back (mask4pointer) ;
    accessDeviceType mask4 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK4, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask4) ;

    accessDeviceType mask5pointer = { itCc->second->getKey(), NORMALMODE, MODE_WRITE, 0, CCHIP_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back (mask5pointer) ;
    accessDeviceType mask5 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK5, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask5) ;

    accessDeviceType mask6pointer = { itCc->second->getKey(), NORMALMODE, MODE_WRITE, 0, CCHIP_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back (mask6pointer) ;
    accessDeviceType mask6 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK6, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask6) ;

    accessDeviceType mask7pointer = { itCc->second->getKey(), NORMALMODE, MODE_WRITE, 0, CCHIP_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back (mask7pointer) ;
    accessDeviceType mask7 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK7, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask7) ;

    accessDeviceType mask8pointer = { itCc->second->getKey(), NORMALMODE, MODE_WRITE, 0, CCHIP_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back (mask8pointer) ;
    accessDeviceType mask8 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK8, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask8) ;

    accessDeviceType mask9pointer = { itCc->second->getKey(), NORMALMODE, MODE_WRITE, 0, CCHIP_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back (mask9pointer) ;
    accessDeviceType mask9 = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_MASK9, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(mask9) ;

    accessDeviceType respointer = { itCc->second->getKey(), NORMALMODE, MODE_WRITE, 0, CCHIP_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back (respointer) ;
    accessDeviceType res = { itCc->second->getKey(), NORMALMODE, MODE_READ, CCHIP_RES, 0, false, 0, 0, 0, NULL} ;
    vAccessesCc[getFecRingKey(itCc->second->getKey())].push_back(res) ;

    // Put it in a map and in the vector
    deviceDescriptionsMap[itCc->second->getKey()] = new totemCChipDescription ( itCc->second->getKey() ) ;
    ccVector.push_back(deviceDescriptionsMap[itCc->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itCc->second->getKey()]->setFecHardwareId ( itCc->second->getFecHardwareId(), itCc->second->getCrateId() ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesCc, errorList ) ;

  // Collect the answer and fill the corresponding totemCChipDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesCc.begin() ; itList != vAccessesCc.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // If Just the Pointer
      if (itDevice->offset == CCHIP_EXTREGPOINTER) {

        channel = data ; // Data Is in this Case the Channel's Number
        edata = true ;
      }

      // If Data Set the Register Variable
      else {

        // If Principal Register
        if (!edata) {
  
          switch (itDevice->offset) {

          case CCHIP_CONTROL0:
	    deviceDescriptionsMap[itDevice->index]->setControl0 (data) ;
	    break ;

          case CCHIP_CONTROL1:
	    deviceDescriptionsMap[itDevice->index]->setControl1 (data) ;
	    break ;

          case CCHIP_CONTROL2:
	    deviceDescriptionsMap[itDevice->index]->setControl2 (data) ;
	    break ;

          case CCHIP_CONTROL3:
	    deviceDescriptionsMap[itDevice->index]->setControl3 (data) ;
	    break ;

          case CCHIP_CONTROL4:
	    deviceDescriptionsMap[itDevice->index]->setControl4 (data) ;
	    break ;

          case CCHIP_CHIPID0:
	    deviceDescriptionsMap[itDevice->index]->setChipid0 (data) ;
	    break ;

          case CCHIP_CHIPID1:
	    deviceDescriptionsMap[itDevice->index]->setChipid1 (data) ;
	    break ;

          case CCHIP_COUNTER0:
	    deviceDescriptionsMap[itDevice->index]->setCounter0 (data) ;
	    break ;

          case CCHIP_COUNTER1:
  	    deviceDescriptionsMap[itDevice->index]->setCounter1 (data) ;
	    break ;

          case CCHIP_COUNTER2:
	    deviceDescriptionsMap[itDevice->index]->setCounter2 (data) ;
  	    break ;

          case CCHIP_MASK0:
	    deviceDescriptionsMap[itDevice->index]->setMask0 (data) ;
  	    break ;

          case CCHIP_MASK1:
	    deviceDescriptionsMap[itDevice->index]->setMask1 (data) ;
  	    break ;

          case CCHIP_MASK2:
	    deviceDescriptionsMap[itDevice->index]->setMask2 (data) ;
  	    break ;

          case CCHIP_MASK3:
	    deviceDescriptionsMap[itDevice->index]->setMask3 (data) ;
  	    break ;

          }
        }
        // If Extended Register
        else {

          switch ((itDevice->offset)){

          case CCHIP_MASK4:
	    deviceDescriptionsMap[itDevice->index]->setMask4 (data) ;
  	    break ;

          case CCHIP_MASK5:
	    deviceDescriptionsMap[itDevice->index]->setMask5 (data) ;
  	    break ;

          case CCHIP_MASK6:
	    deviceDescriptionsMap[itDevice->index]->setMask6 (data) ;
  	    break ;

          case CCHIP_MASK7:
	    deviceDescriptionsMap[itDevice->index]->setMask7 (data) ;
  	    break ;

          case CCHIP_MASK8:
	    deviceDescriptionsMap[itDevice->index]->setMask8 (data) ;
  	    break ;

          case CCHIP_MASK9:
	    deviceDescriptionsMap[itDevice->index]->setMask9 (data) ;
  	    break ;

          case CCHIP_RES:
	    deviceDescriptionsMap[itDevice->index]->setRes (data) ;
  	    break ;

    	  }
          edata = false ;

        }
      }
    }
  }
  return (error) ;

}
