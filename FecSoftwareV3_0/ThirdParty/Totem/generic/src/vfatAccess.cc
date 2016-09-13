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
  based on philipsAccess.cc made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include <iostream>

#include "tscTypes.h"
#include "vfatDefinition.h"
#include "vfatAccess.h"

/** Constructor to store the access to the VFAT (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */

vfatAccess::vfatAccess ( FecAccess *fec, 
                         tscType16 fecSlot,
                         tscType16 ringSlot,
                         tscType16 ccuAddress,
                         tscType16 i2cChannel,
                         tscType16 i2cAddress ) : 

  deviceAccess ( VFAT,
                 fec, 
                 fecSlot, 
                 ringSlot, 
                 ccuAddress,
                 i2cChannel, 
                 i2cAddress ) {

}

/** Constructor to store the access to the VFAT (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
vfatAccess::vfatAccess ( FecAccess *fec, 
                         keyType key ) :

  deviceAccess ( VFAT,
                 fec,
                 key ) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
vfatAccess::~vfatAccess ( ) {

  //~deviceAccess ( ) ;
}

/**
 * \param vfatValues - VFAT description
 */
void vfatAccess::setVfatDescription ( vfatDescription vfatValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = vfatValues.clone() ;
}

/** Take a description value of a vfat and download the values in a specific vfat
 * \param vfatValues - all the vfat values
 * \exception FecExceptionHandler
 */
void vfatAccess::setVfatValues ( vfatDescription vfatValues ) throw (FecExceptionHandler) {

  setVfatControl1 ( vfatValues.getControl1 ( ) ) ;
  setVfatIPreampIn ( vfatValues.getIPreampIn ( ) ) ;
  setVfatIPreampFeed ( vfatValues.getIPreampFeed ( ) ) ;
  setVfatIPreampOut ( vfatValues.getIPreampOut ( ) ) ;
  setVfatIShaper ( vfatValues.getIShaper ( ) ) ;
  setVfatIShaperFeed ( vfatValues.getIShaperFeed ( ) ) ;
  setVfatIComp ( vfatValues.getIComp ( ) ) ;

  setVfatLat ( vfatValues.getLat ( ) ) ;
  setVfatAllChanReg ( vfatValues.getAllChanReg ( ) ) ;
  setVfatVCal ( vfatValues.getVCal ( ) ) ;
  setVfatVThreshold1 ( vfatValues.getVThreshold1 ( ) ) ;
  setVfatVThreshold2 ( vfatValues.getVThreshold2 ( ) ) ;
  setVfatCalPhase ( vfatValues.getCalPhase ( ) ) ;
  setVfatControl2 ( vfatValues.getControl2 ( ) ) ;
  setVfatControl3 ( vfatValues.getControl3 ( ) ) ;
  setVfatControl0 ( vfatValues.getControl0 ( ) ) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = vfatValues.clone() ;
}

/** Build a description of a vfat and return it
 * \return the vfat description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
vfatDescription *vfatAccess::getVfatValues ( ) throw (FecExceptionHandler) {

  vfatDescription *vfatValues = 
    new vfatDescription ( accessKey_,
			  getVfatControl0 (),
			  getVfatControl1 (),
                          getVfatIPreampIn (),
                          getVfatIPreampFeed (),
                          getVfatIPreampOut (),
                          getVfatIShaper (),
                          getVfatIShaperFeed (),
                          getVfatIComp (),

                          getVfatChipid0 (),
                          getVfatChipid1 (),
                          getVfatUpset (),
                          getVfatHitCount0 (),
                          getVfatHitCount1 (),
                          getVfatHitCount2 (),

                          getVfatLat (),
                          getVfatAllChanReg (),
                          getVfatVCal (),
                          getVfatVThreshold1 (),
                          getVfatVThreshold2 (),
                          getVfatCalPhase (),
		  	  getVfatControl2 (),
			  getVfatControl3 () ) ;

  vfatValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;
  
  return (vfatValues) ;
}

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatControl0 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_CONTROL0)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
   void vfatAccess::setVfatControl0   ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_CONTROL0, value ) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatControl1 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_CONTROL1)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatControl1   ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_CONTROL1, value ) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatIPreampIn ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_IPREAMPIN)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatIPreampIn  ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_IPREAMPIN, value ) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatIPreampFeed ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_IPREAMPFEED)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatIPreampFeed  ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_IPREAMPFEED, value ) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatIPreampOut ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_IPREAMPOUT)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatIPreampOut  ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_IPREAMPOUT, value ) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatIShaper ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_ISHAPER)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatIShaper  ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_ISHAPER, value ) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatIShaperFeed ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_ISHAPERFEED)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatIShaperFeed  ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_ISHAPERFEED, value ) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatIComp ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_ICOMP)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatIComp  ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_ICOMP, value ) ; }

  // chipid0, chipid1, upset, hitcount0, hitcount1, hitcount2 are READ ONLY

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatChipid0 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_CHIPID0)) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatChipid1 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_CHIPID1)) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatUpset ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_UPSET)) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatHitCount0 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_HITCOUNT0)) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatHitCount1 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_HITCOUNT1)) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatHitCount2 ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_HITCOUNT2)) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatExtRegPointer ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_EXTREGPOINTER)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatExtRegPointer  ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, value ) ; }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatExtRegData ( ) throw (FecExceptionHandler) { return (accessToFec_->readOffset (accessKey_, VFAT_EXTREGDATA)) ; }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatExtRegData  ( tscType8 value ) throw (FecExceptionHandler) { accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ; }

  //Set and Get functions - EXTENDED

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatLat ( ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_LATENCY ) ; 
    return ( accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA ) );
  }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */

  void vfatAccess::setVfatLat  ( tscType8 value ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_LATENCY ) ; 
    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ;
  }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatChanReg ( int index )  throw (FecExceptionHandler){

    if(index>0 && index<129){
      accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CHANREG+index ) ; 
      return (accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA ) );
    }
    else { std::cout << "Index given was out of array boundaries." << std::endl ; return((tscType8)0x00) ; }
  }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatChanReg ( int index, tscType8 value ) throw (FecExceptionHandler){

    if(index>0 && index<129){

      accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CHANREG+index ) ; 
      accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ;
    }
    else { std::cout << "Index given was out of array boundaries." << std::endl ; }
  }

  /** Get the all VFAT channel registers' values
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 *vfatAccess::getVfatAllChanReg ( ) throw (FecExceptionHandler){

    for (int i=1;i<129;i++)
    {
      accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CHANREG+i ) ; 
      setVfatArray( i, accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA )) ;
    }

    return (tmpArray) ; 
  }

  /** Set the all VFAT channel register' values
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatAllChanReg ( tscType8 *value ) throw (FecExceptionHandler){

    for (int i=1;i<129;i++)
    {
      accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CHANREG+i ) ; 
      accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value[i] ) ;
    }
  }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatVCal ( ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_VCAL ) ; 
    return (accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA) );
  }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatVCal  ( tscType8 value ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_VCAL ) ; 
    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ;
  }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatVThreshold1 ( ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_VTHRESHOLD1 ) ; 
    return (accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA ) );
  }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatVThreshold1  ( tscType8 value ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_VTHRESHOLD1 ) ; 
    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ;
  }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatVThreshold2 ( ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_VTHRESHOLD2 ) ; 
    return (accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA ) );
  }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatVThreshold2  ( tscType8 value ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_VTHRESHOLD2 ) ; 
    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ;
  }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatCalPhase ( ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CALPHASE ) ; 
    return (accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA ) );
  }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatCalPhase ( tscType8 value ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CALPHASE ) ; 
    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ;
  }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatControl2 ( ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CONTROL2 ) ; 
    return (accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA ) );
  }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatControl2 ( tscType8 value ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CONTROL2 ) ; 
    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ;
  }

  /** Get VFAT register value
   * \return return the value read
   * \exception FecExceptionHandler
   */
  tscType8 vfatAccess::getVfatControl3 ( ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CONTROL3 ) ; 
    return (accessToFec_->readOffset ( accessKey_, VFAT_EXTREGDATA ) );
  }

  /** Set the VFAT register value
   * \param Value to be set
   * \exception FecExceptionHandler
   */
  void vfatAccess::setVfatControl3 ( tscType8 value ) throw (FecExceptionHandler){

    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGPOINTER, VFAT_CONTROL3) ; 
    accessToFec_->writeOffset ( accessKey_, VFAT_EXTREGDATA, value ) ;
  }

  /** Set the tmpArray value
   * \param Index - index of array
   * \param Value to be set
   */
  void vfatAccess::setVfatArray ( int index, tscType8 value ){ tmpArray[index] = value; }

/** Take a description of a vfat and build all the frames (through a dedicated structure) to be sent
 * \param vfatValues - all the values for a vfat
 * \param vAccess - block of frames
 */
void vfatAccess::getVfatBlockWriteValues ( vfatDescription vfatValues, std::list<accessDeviceType> &vAccess ) {

  // Buffer of multiple frame block transfer

  accessDeviceType control1 = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getControl1(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control1) ;

  accessDeviceType ipreampin = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getIPreampIn(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ipreampin) ;

  accessDeviceType ipreampfeed = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getIPreampFeed(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ipreampfeed) ;

  accessDeviceType ipreampout = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getIPreampOut(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ipreampout) ;

  accessDeviceType ishaper = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getIShaper(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ishaper) ;

  accessDeviceType ishaperfeed = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getIShaperFeed(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ishaperfeed) ;

  accessDeviceType icomp = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getIComp(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (icomp) ;

  accessDeviceType latpointer = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (latpointer) ;
  accessDeviceType lat = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getLat(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (lat) ;

  for(int i=1; i<129; i++)
  {
    accessDeviceType chanregpointer = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
    vAccess.push_back (chanregpointer) ;
    accessDeviceType chanreg = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getChanReg(i), false, 0, 0, 0, NULL} ;
    vAccess.push_back (chanreg) ;
  }

  accessDeviceType vcalpointer = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vcalpointer) ;
  accessDeviceType vcal = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getVCal(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vcal) ;

  accessDeviceType vthreshold1pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vthreshold1pointer) ;
  accessDeviceType vthreshold1 = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getVThreshold1(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vthreshold1) ;

  accessDeviceType vthreshold2pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vthreshold2pointer) ;
  accessDeviceType vthreshold2 = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getVThreshold2(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vthreshold2) ;

  accessDeviceType calphasepointer = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (calphasepointer) ;
  accessDeviceType calphase = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getCalPhase(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (calphase) ;

  accessDeviceType control2pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control2pointer) ;
  accessDeviceType control2 = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getControl2(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control2) ;

  accessDeviceType control3pointer = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getExtRegPointer(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control3pointer) ;
  accessDeviceType control3 = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getControl3(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control3) ;

  accessDeviceType control0 = { getKey(), NORMALMODE, MODE_WRITE, 0, vfatValues.getControl0(), false, 0, 0, 0, NULL} ;
  vAccess.push_back (control0) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = vfatValues.clone() ;
}

/** This static method read out several VFAT at the same time
 * \param fecAccess - hardware access
 * \param vfatSet - all the VFAT to be readout
 * \param vfatVector - list of the readout VFAT (suppose to be empty at the beginning). The vfatDescription created must be delete by the remote method.
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int vfatAccess::getVfatValuesMultipleFrames ( FecAccess &fecAccess, 
                                                       Sgi::hash_map<keyType, vfatAccess *> &vfatSet,
                                                       deviceVector &vfatVector,
						       std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, vfatDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // Data of Extended register?
  bool edata = false ;
  tscType8 channel = 0x00 ;

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesVfat ;
  for ( Sgi::hash_map<keyType, vfatAccess *>::iterator itVfat = vfatSet.begin() ; itVfat != vfatSet.end() ; itVfat ++ ) {

    accessDeviceType control0 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_CONTROL0, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(control0) ;

    accessDeviceType control1 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_CONTROL1, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(control1) ;

    accessDeviceType ipreampin = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_IPREAMPIN, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(ipreampin) ;

    accessDeviceType ipreampfeed = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_IPREAMPFEED, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(ipreampfeed) ;

    accessDeviceType ipreampout = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_IPREAMPOUT, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(ipreampout) ;

    accessDeviceType ishaper = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_ISHAPER, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(ishaper) ;

    accessDeviceType ishaperfeed = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_ISHAPERFEED, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(ishaperfeed) ;

    accessDeviceType icomp = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_ICOMP, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(icomp) ;

    accessDeviceType chipid0 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_CHIPID0, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(chipid0) ;

    accessDeviceType chipid1 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_CHIPID1, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(chipid1) ;

    accessDeviceType upset = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_UPSET, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(upset) ;

    accessDeviceType hitcount0 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_HITCOUNT0, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(hitcount0) ;

    accessDeviceType hitcount1 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_HITCOUNT1, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(hitcount1) ;

    accessDeviceType hitcount2 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_HITCOUNT2, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(hitcount2) ;

    accessDeviceType latpointer = { itVfat->second->getKey(), NORMALMODE, MODE_WRITE, 0, VFAT_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back (latpointer) ;
    accessDeviceType lat = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_LATENCY, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(lat) ;

    for(int i=1; i<129; i++)
    {
      accessDeviceType chanregpointer = { itVfat->second->getKey(), NORMALMODE, MODE_WRITE, 0, VFAT_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
      vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back (chanregpointer) ;
      accessDeviceType chanreg = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_CHANREG+i, 0, false, 0, 0, 0, NULL} ;
      vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(chanreg) ;
    }

    accessDeviceType vcalpointer = { itVfat->second->getKey(), NORMALMODE, MODE_WRITE, 0, VFAT_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back (vcalpointer) ;
    accessDeviceType vcal = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_VCAL, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(vcal) ;

    accessDeviceType vthreshold1pointer = { itVfat->second->getKey(), NORMALMODE, MODE_WRITE, 0, VFAT_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back (vthreshold1pointer) ;
    accessDeviceType vthreshold1 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_VTHRESHOLD1, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(vthreshold1) ;

    accessDeviceType vthreshold2pointer = { itVfat->second->getKey(), NORMALMODE, MODE_WRITE, 0, VFAT_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back (vthreshold2pointer) ;
    accessDeviceType vthreshold2 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_VTHRESHOLD2, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(vthreshold2) ;

    accessDeviceType calphasepointer = { itVfat->second->getKey(), NORMALMODE, MODE_WRITE, 0, VFAT_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back (calphasepointer) ;
    accessDeviceType calphase = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_CALPHASE, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(calphase) ;

    accessDeviceType control2pointer = { itVfat->second->getKey(), NORMALMODE, MODE_WRITE, 0, VFAT_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back (control2pointer) ;
    accessDeviceType control2 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_CONTROL2, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(control2) ;

    accessDeviceType control3pointer = { itVfat->second->getKey(), NORMALMODE, MODE_WRITE, 0, VFAT_EXTREGPOINTER, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back (control3pointer) ;
    accessDeviceType control3 = { itVfat->second->getKey(), NORMALMODE, MODE_READ, VFAT_CONTROL3, 0, false, 0, 0, 0, NULL} ;
    vAccessesVfat[getFecRingKey(itVfat->second->getKey())].push_back(control3) ;

    // Put it in a map and in the vector
    deviceDescriptionsMap[itVfat->second->getKey()] = new vfatDescription ( itVfat->second->getKey() ) ;
    vfatVector.push_back(deviceDescriptionsMap[itVfat->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itVfat->second->getKey()]->setFecHardwareId ( itVfat->second->getFecHardwareId(), itVfat->second->getCrateId() ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesVfat, errorList ) ;

  // Collect the answer and fill the corresponding vfatDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesVfat.begin() ; itList != vAccessesVfat.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFFFF ;
      if (itDevice->e != NULL) data = 0xFFFF ;
      else data = itDevice->data ;

      // If Just the Pointer
      if (itDevice->offset == VFAT_EXTREGPOINTER) 
      {
        channel = data ; // Data Is in this Case the Channel's Number
        edata = true ;
      }

      // If Data Set the Register Variable
      else
      {

        // If Principal Register
        if (!edata)
        {     
          switch ((itDevice->offset)) 
          {

          case VFAT_CONTROL0:
	    deviceDescriptionsMap[itDevice->index]->setControl0 (data) ;
	    break ;

          case VFAT_CONTROL1:
	    deviceDescriptionsMap[itDevice->index]->setControl1 (data) ;
	    break ;

          case VFAT_IPREAMPIN:
	    deviceDescriptionsMap[itDevice->index]->setIPreampIn (data) ;
	    break ;

          case VFAT_IPREAMPFEED:
	    deviceDescriptionsMap[itDevice->index]->setIPreampFeed (data) ;
	    break ;

          case VFAT_IPREAMPOUT:
  	    deviceDescriptionsMap[itDevice->index]->setIPreampOut (data) ;
	    break ;

          case VFAT_ISHAPER:
	    deviceDescriptionsMap[itDevice->index]->setIShaper (data) ;
	    break ;

          case VFAT_ISHAPERFEED:
	    deviceDescriptionsMap[itDevice->index]->setIShaperFeed (data) ;
	    break ;

          case VFAT_ICOMP:
	    deviceDescriptionsMap[itDevice->index]->setIComp (data) ;
	    break ;

          case VFAT_CHIPID0:
	    deviceDescriptionsMap[itDevice->index]->setChipid0 (data) ;
	    break ;

          case VFAT_CHIPID1:
	    deviceDescriptionsMap[itDevice->index]->setChipid1 (data) ;
	    break ;

          case VFAT_UPSET:
	    deviceDescriptionsMap[itDevice->index]->setUpset (data) ;
	    break ;

          case VFAT_HITCOUNT0:
	    deviceDescriptionsMap[itDevice->index]->setHitCount0 (data) ;
	    break ;

          case VFAT_HITCOUNT1:
  	    deviceDescriptionsMap[itDevice->index]->setHitCount1 (data) ;
	    break ;

          case VFAT_HITCOUNT2:
	    deviceDescriptionsMap[itDevice->index]->setHitCount2 (data) ;
  	    break ;

          }
        }
        // If Extended Register
        else
        {
          switch ((itDevice->offset)) 
          {

          case VFAT_LATENCY:
	    deviceDescriptionsMap[itDevice->index]->setLat (data) ;
  	    break ;

          case VFAT_VCAL:
	    deviceDescriptionsMap[itDevice->index]->setVCal (data) ;
  	    break ;

          case VFAT_VTHRESHOLD1:
	    deviceDescriptionsMap[itDevice->index]->setVThreshold1 (data) ;
  	    break ;

          case VFAT_VTHRESHOLD2:
	    deviceDescriptionsMap[itDevice->index]->setVThreshold2 (data) ;
  	    break ;

          case VFAT_CALPHASE:
	    deviceDescriptionsMap[itDevice->index]->setCalPhase (data) ;
  	    break ;

          case VFAT_CONTROL2:
	    deviceDescriptionsMap[itDevice->index]->setControl2 (data) ;
	    break ;

          case VFAT_CONTROL3:
	    deviceDescriptionsMap[itDevice->index]->setControl3 (data) ;
	    break ;

	  default: // Channel Register
            deviceDescriptionsMap[itDevice->index]->setChanReg ((int)channel, data) ;
	    break ;

    	  }
          edata = false ;

        }
      }
    }
  }

  return (error) ;
}
