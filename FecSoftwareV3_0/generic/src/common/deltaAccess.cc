/*
This file is part of CMS Preshower Control Software project.

Author: Wojciech BIALAS , CERN, Geneva, Switzerland

based on apvAccess class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include <iostream>

#include "tscTypes.h"

#include "deltaDefinition.h"
#include "deltaDescription.h"
#include "deltaAccess.h"

/** Constructor to store the access to the DELTA3B (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
deltaAccess::deltaAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress )
  : deviceAccess (DELTA,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress )
  
{

}

/** Constructor to store the access to the DELTA3B (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
deltaAccess::deltaAccess (FecAccess *fec, 
                      keyType key) 
  : deviceAccess (DELTA,
                fec,
                key) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
deltaAccess::~deltaAccess ( ) {

  // ~deviceAccess ( ) ;
}

/**
 * deltaValues - DELTA description
 */
void deltaAccess::setDescription ( class deltaDescription& deltaValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = deltaValues.clone() ;
}

/** Take a description value of an DELTA3B and download the values in a specific DELTA3B
 * \param deltaValues - all the values for an DELTA3B
 * \exception FecExceptionHandler
 */
void deltaAccess::setValues ( class deltaDescription& deltaValues ) {
  
  int i ; 
  for(i=0;i<DELTA_REG_NUM;i++) { 
    if (!ro_[i]) setValue(i,deltaValues.getValue(i)) ; 
  }   

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = deltaValues.clone() ;
}

/** Build a description of an DELTA3B and return it
 * \return the DELTA3B description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
deltaDescription *deltaAccess::getValues ( ) {

   deltaDescription *deltaValues = new deltaDescription(getKey());

   if (deltaValues) { 
      int i; 
      for(i=0;i<DELTA_DESC_NUM;i++) { 
	 try { 
	 deltaValues->setValue(i,getValue(i));
	 } catch (FecExceptionHandler& e) { 
	    delete deltaValues; 
	    throw; // i.e. re-throw
	 } 
      }
      deltaValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;
   }

  return (deltaValues) ;
}

/** Set the DELTA3B CR0 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setCR0( tscType8 cr0 ) {

  accessToFec_->write (accessKey_, DELTA_CR0_REG, cr0) ;
}

/** Set the DELTA3B CR1 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setCR1 ( tscType8 cr1 ) {

  accessToFec_->write (accessKey_, DELTA_CR1_REG, cr1) ;
}

/** Set the DELTA3B CalChan0 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setCalChan0 ( tscType8 cal0 ) {

  accessToFec_->write (accessKey_, DELTA_CALCHAN0_REG, cal0) ;
}


/** Set the DELTA3B CalChan1 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setCalChan1 ( tscType8 cal1 ) {

  accessToFec_->write (accessKey_, DELTA_CALCHAN1_REG, cal1) ;
}

/** Set the DELTA3B CalChan2 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setCalChan2 ( tscType8 cal2 ) {

  accessToFec_->write (accessKey_, DELTA_CALCHAN2_REG, cal2) ;
}

/** Set the DELTA3B CalChan0 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setCalChan3 ( tscType8 cal3 ) {

  accessToFec_->write (accessKey_, DELTA_CALCHAN3_REG, cal3) ;
}


/** Set the DELTA3B VCal register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setVcal ( tscType8 vcal  ) {

  accessToFec_->write (accessKey_, DELTA_VCAL_REG, vcal) ;
}

/** Set the APV Ipre
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setVoPreamp ( tscType8 vopre ) {

  accessToFec_->write (accessKey_, DELTA_VOPREAMP_REG, vopre) ;
}

/** Set the APV Ipcasc
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setVoShaper ( tscType8 voshaper ) {

  accessToFec_->write (accessKey_, DELTA_VOSHAPER_REG, voshaper) ;
}

/** Set the APV Ipsf
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setVspare ( tscType8 vspare ) {

  accessToFec_->write (accessKey_, DELTA_VSPARE_REG, vspare) ;
}

/** Set the APV Isha
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setIpreamp ( tscType8 ipreamp ) {

  accessToFec_->write (accessKey_, DELTA_IPREAMP_REG, ipreamp) ;
}

/** Set the APV Issf
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setIshaper ( tscType8 ishaper ) {

  accessToFec_->write (accessKey_, DELTA_ISHAPER_REG, ishaper) ;
}

/** Set the APV Ipsp
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setISF ( tscType8 isf  ) {

  accessToFec_->write (accessKey_, DELTA_ISF_REG, isf) ;
}

/** Set the APV Imuxin
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void deltaAccess::setIspare ( tscType8 ispare ) {

  accessToFec_->write (accessKey_, DELTA_ISPARE_REG, ispare) ;
}


void deltaAccess::setValue(int reg, tscType8 val) { 
  if (!ro_[reg]) { // if not read-only
    accessToFec_->write (accessKey_, (reg << 1), val) ;
  } 
} 

/** Get APV Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getValue (int reg )  {

  return (accessToFec_->read (accessKey_, (reg<<1) | DELTA_READ)) ;
}
/** Get APV Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getMaskedValue (int reg )  {

  return (rmask_[reg] & (accessToFec_->read (accessKey_, (reg<<1) | DELTA_READ))) ;
}





/** Get APV Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getCR0 ( )  {

  return (accessToFec_->read (accessKey_, DELTA_CR0_REG + DELTA_READ)) ;
}

/** Get APV Mode
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getCR1 ( ) {

  return (accessToFec_->read (accessKey_, DELTA_CR1_REG + DELTA_READ)) ;
}

/** Get APV Latency
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getChipID0 ( )  {

  return (accessToFec_->read (accessKey_, DELTA_CHIPID0_REG + DELTA_READ)) ;
}

/** Get APV Mux Gain
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getChipID1 ( )  {

  return (accessToFec_->read (accessKey_, DELTA_CHIPID1_REG + DELTA_READ)) ;
}

/** Get DELTA3B calchan0
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getCalChan0 ( ) {

  return (accessToFec_->read (accessKey_, DELTA_CALCHAN0_REG + DELTA_READ)) ;
}
/** Get DELTA3B calchan1
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getCalChan1 ( ) {

  return (accessToFec_->read (accessKey_, DELTA_CALCHAN1_REG + DELTA_READ)) ;
}
/** Get DELTA3B calchan2
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getCalChan2 ( ) {

  return (accessToFec_->read (accessKey_, DELTA_CALCHAN2_REG + DELTA_READ)) ;
}
/** Get DELTA3B calchan3
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getCalChan3 ( ) {

  return (accessToFec_->read (accessKey_, DELTA_CALCHAN3_REG + DELTA_READ)) ;
}

/** Get APV Ipcasc
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getVcal ( )  {

  return (accessToFec_->read (accessKey_, DELTA_VCAL_REG + DELTA_READ)) ;
}

/** Get APV Ipsf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getVoPreamp ( ) {

  return (accessToFec_->read (accessKey_, DELTA_VOPREAMP_REG + DELTA_READ)) ;
}

/** Get APV Isha
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getVoShaper ( )  {

  return (accessToFec_->read (accessKey_, DELTA_VOSHAPER_REG + DELTA_READ)) ;
}

/** Get APV Issf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getVspare ( ) {

  return (accessToFec_->read (accessKey_, DELTA_VSPARE_REG + DELTA_READ)) ;
}

/** Get APV Ipsp
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getIpreamp ( ) {

  return (accessToFec_->read (accessKey_, DELTA_IPREAMP_REG + DELTA_READ)) ;
}

/** Get APV Imuxin
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getIshaper ( ) {

  return (accessToFec_->read (accessKey_, DELTA_ISHAPER_REG + DELTA_READ)) ;
}

/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 deltaAccess::getISF ( ) {

  return (accessToFec_->read (accessKey_, DELTA_ISF_REG + DELTA_READ)) ;
}

/** Get APV Ispare
 * \return 0
 * \warning not used, this register is not used in the APV
 */
tscType8 deltaAccess::getIspare ( ) {
  return (accessToFec_->read (accessKey_, DELTA_ISF_REG + DELTA_READ)) ;
}

/** Take a description value of a delta a build a block of frames to be set in the hardware
 * \param deltaValues - all the values for a detla chip
 * \param vAccess - block of frames
 */
void deltaAccess::getBlockWriteValues ( class deltaDescription& deltaValues, accessDeviceTypeList &vAccess ) {

  for(int i = 0 ; i < DELTA_REG_NUM ; i++) { 
    if (!ro_[i]) { // if not read-only
      // accessToFec_->write (accessKey_, (reg << 1), val) ;
      accessDeviceType setIt = { getKey(), RALMODE, MODE_WRITE, (tscType16)(i << 1), deltaValues.getValue(i), false, 0, 0, 0, NULL} ;
      vAccess.push_back (setIt) ;
    } 
  }
}

/** This static method read out several delta at the same time
 * \param fecAccess - hardware access
 * \param deltaSet - all the APV to be readout
 * \param deltaVector - list of the readout DELTA (suppose to be empty at the beginning). The deltaDescription created must be delete by the remote method.
 * \param errorList - list of errors, should be deleted by the remote method
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int deltaAccess::getDeltaValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, deltaAccess *> &deltaSet, deviceVector &deltaVector,
							 std::list<FecExceptionHandler *> &errorList ) 
  throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, deltaDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesApv ;
  for ( Sgi::hash_map<keyType, deltaAccess *>::iterator itDelta = deltaSet.begin() ; itDelta != deltaSet.end() ; itDelta ++ ) {

    for(int i = 0 ; i < DELTA_REG_NUM ; i++) { 
      // accessToFec_->read (accessKey_, (reg<<1) | DELTA_READ)
      accessDeviceType readIt = { itDelta->second->getKey(), RALMODE, MODE_READ, (tscType16)((i<<1) + DELTA_READ), 0, false, 0, 0, 0, NULL} ;
      vAccessesApv[getFecRingKey(itDelta->second->getKey())].push_back(readIt) ;
    }

    // Put it in a map and in the vector
    deviceDescriptionsMap[itDelta->second->getKey()] = new deltaDescription ( itDelta->second->getKey() ) ;
    deltaVector.push_back(deviceDescriptionsMap[itDelta->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itDelta->second->getKey()]->setFecHardwareId ( itDelta->second->getFecHardwareId(),itDelta->second->getCrateId()  ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesApv, errorList ) ;

  // Collect the answer and fill the corresponding apvDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesApv.begin() ; itList != vAccessesApv.end() ; itList ++) {
    
    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // Offset
      tscType8 offset = itDevice->offset-DELTA_READ ;
      offset = offset >> 1 ;
      deviceDescriptionsMap[itDevice->index]->setValue (offset,data) ;
    }
  }

  return (error) ;
}


const char* deltaAccess::names[] = { "CR0" , "CR1" , "ChipID0","ChipID1","CalChan0",
			    "CalChan1","CalChan2","CalChan3","Vcal","VoPreamp",
			    "VoShaper","Vspare","Ipreamp","Ishaper","ISF",
			    "Ispare" } ; 
const tscType8 deltaAccess::rmask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff } ; // i.e. all bits are checked 

const bool deltaAccess::ro_[] = { 
  false,false, true, true,false,false,false,false,
  false,false,false,false,false,false,false,false };

 

