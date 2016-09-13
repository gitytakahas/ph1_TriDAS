/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include <iostream>
#include <unistd.h>

#include "tscTypes.h"

#include "laserdriverDefinition.h"
#include "DohDefinition.h"
#include "DohAccess.h"

/** Constructor to store the access to the APV (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
DohAccess::DohAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress ) : 

  laserdriverAccess ( fec, 
                      fecSlot, 
                      ringSlot, 
                      ccuAddress,
                      i2cChannel, 
                      i2cAddress ) {

  deviceType_ = DOH ;
}

/** Constructor to store the access to the DOH (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
DohAccess::DohAccess (FecAccess *fec, 
                      keyType key) :

  laserdriverAccess (fec,
                     key) {

  deviceType_ = DOH ;

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
DohAccess::~DohAccess ( ) {

  // ~deviceAccess ( ) ;
}

/** Build a description of a laserdriver and return it
 * \return the laserdriver DOH description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
laserdriverDescription *DohAccess::getValues ( ) {

  laserdriverDescription *dohValues = laserdriverAccess::getValues() ;
  dohValues->setDeviceType(DOH) ;
  dohValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

  return (dohValues) ;
}

/** Take a description value of a doh and download the values in a specific doh
 * \param dohValues - all the values for a doh
 * \exception FecExceptionHandler
 * \warning the differences between the two values must be set and step by step
 * setting are observed for the gain (48, 40, 32, 24). 48 is the initial value
 * after setting up the hybrid
 * \warning the first step is to read gain value set in the DOH and go the 
 * optimal setting step by step. The step observed is defined in 
 * DohAccess.h: DOHSTEP.
 * \warning Whatever the bias values are, the value set will 24, 24, 0
 */
void DohAccess::setValues ( laserdriverDescription dohValues, bool realValues ) {

  // to set the real values given by the methods
  //realValues = true ;
  if (realValues) {
    // set the Gain
    setGain ( dohValues.getGain ( ) ) ;

    // Set the bias
    tscType8 bias[MAXLASERDRIVERCHANNELS] ;
    dohValues.getBias ( bias ) ;
    setBias ( bias ) ;
  }

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = dohValues.clone() ;

  if (!realValues) {
    tscType8 bias[MAXLASERDRIVERCHANNELS] ;
    getBias(bias) ;
    ((laserdriverDescription *)deviceValues_)->setBias(bias) ;
  }
}

/** Set the DOH bias
 * \param bias - Value to be set
 * \exception FecExceptionHandler
 */
void DohAccess::setBias ( tscType8 *bias ) {

  // Read the bias in the DOH
  tscType8 oldBias[MAXLASERDRIVERCHANNELS] ;
  getBias( oldBias ) ;

  // End
  bool termine = false ;

  while (! termine) {

    // End ?
    termine = true ;

    // For each channel
    for (int i = 0 ; i < MAXDOHCHANNELS ; i ++) {

      if (oldBias[i] != bias[i]) {

	// Not the end
	termine = false ;

	int val, puis ;
	if (bias[i] > oldBias[i]) {
	  val = (int)(bias[i]-oldBias[i]) ;
	  puis = 1 ;
	}
	else {
	  val = (int)(oldBias[i]-bias[i]) ;
	  puis = -1 ;
	}

	if (val <= DOHSTEP)
	  oldBias[i] = bias[i] ;
	else
	  oldBias[i] += (DOHSTEP*puis) ;
      }
    }

    if (!termine) {
#ifdef DEBUGMSGERROR
      std::cout << "---------------------*********************** hhhhhhhhhhhhhhhhhhhhhhh" << std::endl ;
      for (int i = 0 ; i < MAXDOHCHANNELS ; i ++) {
	std::cout << "DohAccess::setBias: " << std::dec << i << ": " << (int)oldBias[i] << std::hex << " (0x" << (int)oldBias[i] << ")" << std::endl ;
      }
#endif

      laserdriverAccess::setBias (oldBias) ;
    }
  }
}

/** Calibration for the DOH invidually
 * \param displayDebugMessage - display debug messages
 * \param gain - optional parameter (set by default to 1)
 * \param minBiasValue - minimum value for the bias (by default 9+1)
 * \return the description optimized
 * \exception FecExceptionHandler
 * \warning this method reset the ring several times so do not forget to apply the reset on FecAccessManager object if you use it
 */
laserdriverDescription *DohAccess::calibrateDOH ( bool displayDebugMessage, unsigned char gain, unsigned char minBiasValue ) throw (FecExceptionHandler) {

  displayDebugMessage = true ;

  accessToFec_->fecRingReset(accessKey_) ;
  accessToFec_->fecRingResetB(accessKey_) ;
  unsigned int timeout = 0 ;
  do {
    if (!isFecSR0Correct(accessToFec_->getFecRingSR0(accessKey_))) {
      if (displayDebugMessage) std::cerr << "Bad SR0 = 0x" << std::hex << accessToFec_->getFecRingSR0(accessKey_) << std::endl ;
      //accessToFec_->fecRingReset(accessKey_) ;
      //accessToFec_->fecRingResetB(accessKey_) ;
    }
    sleep((unsigned int)1) ;
  }
  while (!isFecSR0Correct(accessToFec_->getFecRingSR0(accessKey_)) && timeout++ < 3) ;

  if (displayDebugMessage) std::cout << "After the reset, SR0 = 0x" << std::hex << accessToFec_->getFecRingSR0(accessKey_) << std::endl ;

  // Re-enable channels and the DOH access
  accessToFec_->addi2cAccess (accessKey_, deviceType_, MODE_SHARE ) ;

  if (displayDebugMessage) {
    laserdriverDescription *mald = getValues( ) ;
    std::cout << "Value read back from the DOH" << std::endl ;
    std::cout << "Channel\t1\t2\t3" << std::endl ;
    std::cout << "Bias   \t" << std::dec
	      << (int)mald->getBias0() << "\t" 
	      << (int)mald->getBias1() << "\t" 
	      << (int)mald->getBias2() << std::endl ;
    std::cout << "Gain   \t" << std::dec
	      << (int)mald->getGain0() << "\t" 
	      << (int)mald->getGain1() << "\t" 
	      << (int)mald->getGain2() << std::endl ;
    delete mald ;
  }

  // Gain settings
  unsigned char value = 0 ;
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) value |= ((gain & 0x3) << (i*2)) ;
  if (displayDebugMessage) std::cout << "Set the gain for each laser to 0x" << std::hex << (int)gain << ", (value = 0x" << (int)value << ")" << std::endl ;
  setGain(value) ;

  // Read CR0 and check
  tscType16 fecSR0 = accessToFec_->getFecRingSR0(accessKey_) ;    
  if (displayDebugMessage) std::cout << "Status register 0 of the FEC " << getFecKey(accessKey_) << " ring " << getRingKey(accessKey_) << ": " << std::hex << fecSR0 << std::endl ;

  if (! isFecSR0Correct(fecSR0)) {

    accessToFec_->fecRingReset(accessKey_) ;
    accessToFec_->fecRingResetB(accessKey_) ;

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_SR0NOTNOMINAL,
				      TSCFEC_SR0NOTNOMINAL_MSG,
				      ERRORCODE,
				      accessKey_,
				      "FEC status register 0", fecSR0) ;
  }

  // Read the values from the DOH
  tscType8 biasToBeSet[MAXLASERDRIVERCHANNELS] ;
  // Get the original values and display it
  laserdriverDescription *mald = getValues() ;
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    biasToBeSet[i] = mald->getBias(i) ;
  }
    
  if (displayDebugMessage) {
    std::cout << "Channel\t1\t2\t3" << std::endl ;
    std::cout << "Bias   \t" << std::dec
	      << (int)biasToBeSet[0] << "\t"
	      << (int)biasToBeSet[1] << "\t" 
	      << (int)biasToBeSet[2] << std::endl ;
    std::cout << "Gain   \t" << std::dec 
	      << (int)mald->getGain(0) << "\t" 
	      << (int)mald->getGain(1) << "\t" 
	      << (int)mald->getGain(2) << std::endl ;
  }
  delete mald ;
  
  while (isFecSR0Correct(fecSR0) && 
	 (biasToBeSet[0] >= minBiasValue) &&
	 (biasToBeSet[1] >= minBiasValue) &&
	 (biasToBeSet[2] >= minBiasValue) ) {

    try {
      // Save the bias
      for (uint i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
	biasToBeSet[i] -- ;
      }

      // Set the bias
      setBias (biasToBeSet) ;

      fecSR0 = accessToFec_->getFecRingSR0(accessKey_) ;    
      if (displayDebugMessage) std::cout << "Status register 0 of the FEC " << getFecKey(accessKey_) << " ring " << getRingKey(accessKey_) << ": " << std::hex << fecSR0 << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      
      fecSR0 = accessToFec_->getFecRingSR0(accessKey_) ;    
      if (displayDebugMessage) std::cout << "End calibration: the ring is lost (SR0 = 0x" << std::hex << fecSR0 << ")" << std::endl ;
    }
  }

  accessToFec_->fecRingReset(accessKey_) ;
  accessToFec_->fecRingResetB(accessKey_) ;
  timeout = 0 ;
  do {
    if (!isFecSR0Correct(accessToFec_->getFecRingSR0(accessKey_))) {
      if (displayDebugMessage) std::cerr << "Bad SR0 = 0x" << std::hex << accessToFec_->getFecRingSR0(accessKey_) << std::endl ;
      //accessToFec_->fecRingReset(accessKey_) ;
      //accessToFec_->fecRingResetB(accessKey_) ;
    }
    sleep((unsigned int)1) ;
  }
  while (!isFecSR0Correct(accessToFec_->getFecRingSR0(accessKey_)) && timeout++ < 3) ;

  if (displayDebugMessage) std::cout << "After the reset, SR0 = 0x" << std::hex << accessToFec_->getFecRingSR0(accessKey_) << std::endl ;

  if (! isFecSR0Correct(accessToFec_->getFecRingSR0(accessKey_))) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_SR0NOTNOMINAL,
				      TSCFEC_SR0NOTNOMINAL_MSG,
				      ERRORCODE,
				      accessKey_,
				      "FEC status register 0", fecSR0) ;
  }

  // Re-enable channels and the DOH access
  accessToFec_->addi2cAccess (accessKey_, deviceType_, MODE_SHARE ) ;

  // New Settings
  for (uint i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    if (biasToBeSet[i] == 0) biasToBeSet[i] = MINBIASVALUE ;
    else biasToBeSet[i] = biasToBeSet[i] + DOHSTEP ;
  }
  laserdriverDescription ldd(value, biasToBeSet) ;

  if (displayDebugMessage) {
    std::cout << "Bias to be set" << std::endl ;
    std::cout << "Channel\t1\t2\t3" << std::endl ;
    std::cout << "Bias   \t" << std::dec
	      << (int)ldd.getBias0() << "\t"
	      << (int)ldd.getBias1() << "\t" 
	      << (int)ldd.getBias2() << std::endl ;
    std::cout << "Gain   \t" << std::dec 
	      << (int)ldd.getGain0() << "\t" 
	      << (int)ldd.getGain1() << "\t" 
	      << (int)ldd.getGain2() << std::endl ;
  }

  // Set the values
  setValues(ldd, true) ;

  // Get the original values and display it
  mald = getValues() ;

  if (displayDebugMessage) {
    std::cout << "Channel\t1\t2\t3" << std::endl ;
    std::cout << "Bias   \t" << std::dec
	      << (int)mald->getBias0() << "\t"
	      << (int)mald->getBias1() << "\t" 
	      << (int)mald->getBias2() << std::endl ;
    std::cout << "Gain   \t" << std::dec 
	      << (int)mald->getGain(0) << "\t" 
	      << (int)mald->getGain(1) << "\t" 
	      << (int)mald->getGain(2) << std::endl ;
  }

  if (displayDebugMessage) std::cout << "DohAccess::calibrateDOH: end of the procedure: SR0 = 0x" << std::hex << accessToFec_->getFecRingSR0(accessKey_) << std::endl ;

  return (mald) ;
}



