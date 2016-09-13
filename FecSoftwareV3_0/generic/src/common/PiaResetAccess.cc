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

#include <unistd.h>  // usleep

#include "tscTypes.h"
#include "deviceType.h"
#include "deviceFrame.h"

#include "PiaResetAccess.h"

/** Constructor to store the access to the PIA (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param piaChannel - PIA channel
 * \param initialValue - initialise the PIA channel to this value (!)
 * \exception FecExceptionHandler
 */
PiaResetAccess::PiaResetAccess (FecAccess *fec, 
                                tscType16 fecSlot,
                                tscType16 ringSlot,
                                tscType16 ccuAddress,
                                tscType16 piaChannel,
                                tscType8 initialValue) : 

  PiaChannelAccess ( fec, 
                     fecSlot, 
                     ringSlot, 
                     ccuAddress,
                     piaChannel ) {

  piaResetValues_ = NULL ;

  accessToFec_->setPiaChannelDDR (accessKey_, INITCCU25PIADDR) ;
  accessToFec_->write (accessKey_, initialValue) ;

  initialValue_ = initialValue ;
}

/** Constructor to store the access to the PIA reset (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \param initialValue - initialise the PIA channel to this value (!)
 * \exception FecExceptionHandler
 */
PiaResetAccess::PiaResetAccess (FecAccess *fec, 
                                keyType key,
                                tscType8 initialValue ) :

  PiaChannelAccess ( fec,
		     key) {

  piaResetValues_ = NULL ;

  accessToFec_->setPiaChannelDDR (accessKey_, INITCCU25PIADDR) ;
  accessToFec_->write (accessKey_, initialValue) ;

  initialValue_ = initialValue ;
}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
PiaResetAccess::~PiaResetAccess ( ) {

  // ~PiaChannelAccess ( ) ;

  if (piaResetValues_ != NULL) delete piaResetValues_ ;
}

/** This method takes a PIA description and clone it in its own attribut for future comparison
 * It is mainly used for multiple frames methods (see PiaResetAccess::resetPiaModulesMultipleFrame)
 * \param piaValues - PIA description
 */
void PiaResetAccess::setDescription ( piaResetDescription piaValues ) {

 // Create a copy of the data set
  if (piaResetValues_ != NULL) delete piaResetValues_ ;
  piaResetValues_ = piaValues.clone() ;
}

/** Take a description value of a PIA reset and download it to the data register
 * \param piaValues - all the values for the PIA reset
 * \exception FecExceptionHandler
 */
void PiaResetAccess::setValues ( piaResetDescription piaValues ) {

  setPiaReset ( piaValues.getMask(), 
                piaValues.getDelayActiveReset(),
                piaValues.getIntervalDelayReset()) ;

  // Create a copy of the data set
  if (piaResetValues_ != NULL) delete piaResetValues_ ;
  piaResetValues_ = piaValues.clone() ;
}

/** Build a description of a PIA and return it
 * \return the PIA reset description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
piaResetDescription *PiaResetAccess::getValues ( ) {

  piaResetDescription *piaValues = 
    new piaResetDescription (accessKey_,
                             piaResetValues_->getDelayActiveReset(),
                             piaResetValues_->getIntervalDelayReset(),
                             getMask( ) ) ;
  piaValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

  return (piaValues) ;
}

/** Get the value of the mux resistor
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType8 PiaResetAccess::getMask ( ) {

  return (accessToFec_->read ( accessKey_ )) ;
}

/** Set the value for the PIA reset
 * \param mask - Value to be set
 * \param delayActiveReset - delay active reset
 * \param intervalDelayReset - interval delay between two reset
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning this value is written per bit and bit and maintain during delayActiveReset. 
 * Each write is separated by intervalDelayReset
 */
void PiaResetAccess::setPiaReset ( tscType8 mask,
                                   unsigned long delayActiveReset,
                                   unsigned long intervalDelayReset) 
  throw (FecExceptionHandler) {

  // ?????????????????????????????????????????????
  // Semaphore ici

  // For each bit set
  for ( unsigned int i = 0 ; i < 8 ; i ++) {
    
    tscType8 word  = mask & (1 << i) ;
    if (word) {

      tscType8 uval = initialValue_ & ~word ;

      accessToFec_->write ( accessKey_, uval ) ;
#ifdef DEBUGMSGERROR
      printf ("Write 0x%x (0x%x) ", uval, accessToFec_->read ( accessKey_ ) ) ;
      printf ("for %lu\n", delayActiveReset) ;
#endif
      usleep (delayActiveReset) ;
      accessToFec_->write ( accessKey_, initialValue_ ) ;
#ifdef DEBUGMSGERROR
      printf ("Write 0x%x (0x%x) ", initialValue_, accessToFec_->read ( accessKey_ ) ) ;
      printf ("for %lu\n", intervalDelayReset) ;
#endif
      usleep (intervalDelayReset) ;
    }
  }

  // Wait delay
  usleep (intervalDelayReset) ;

  // ?????????????????????????????????
  // Fin de la semaphore
}

/** Take a vector of PIA reset and reset all modules given by this vector
 * This method use a multiple frames algorithm to send in parallel on each CCU/PIA channel the reset. 
 * \param fecAccess - hardware access
 * \param vPiaReset - vector of PIA reset descriptions
 * \param delayActiveReset - max values of delayActiveReset. Delay to wait between the set of the value and the set of the initial value ie. the time which maintain the reset.
 * \param intervalDelayReset - max values of intervalDelayReset. Delay to wait between two resets
 * \warning The mask must be the same
 * \warning The delayActiveReset and intervalDelayReset that will be used will be the maximum time given by this two arguments
 */
unsigned int PiaResetAccess::resetPiaModulesMultipleFrame ( FecAccess &fecAccess, piaResetVector vPiaReset, 
							    unsigned int delayActiveReset, unsigned int intervalDelayReset,
							    tscType8 initialValue,
							    std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  typedef Sgi::hash_map< keyType, std::list<accessDeviceType> > accessPiaTypeListMap ;

  // Number of errors 
  unsigned int error = 0 ;

  // Map for each word
  accessPiaTypeListMap vAccessReset ; // reset to be set
  accessPiaTypeListMap vAccessInialValue ; // after the reset

  // Fill the previous map
  for (piaResetVector::iterator itPia = vPiaReset.begin() ; itPia != vPiaReset.end() ; itPia ++) {

    tscType8 word  = (*itPia)->getMask() & (1 << 0) ;
    if (word) {
      tscType8 uval = initialValue & ~word ;
      // the offset is used to store the mask in order to simplify the change
      accessDeviceType piaReset = { (*itPia)->getKey(), 0, MODE_WRITE, (*itPia)->getMask(), uval, false, 0, 0, 0, NULL } ;
      vAccessReset[getFecRingKey((*itPia)->getKey())].push_back(piaReset) ;

      accessDeviceType piaInitialValue = { (*itPia)->getKey(), 0, MODE_WRITE, 0, PIAINITIALVALUE, false, 0, 0, 0, NULL } ;
      vAccessInialValue[getFecRingKey((*itPia)->getKey())].push_back(piaInitialValue) ;
    }
  }

  // -----------------------------------------------------------------------------------------
  // Send the frames over the ring for each FEC ring
  // For each bit set
  for ( unsigned int i = 0 ; i < 8 ; i ++) {

    // modify the value to be set in the hardware, reset bit
    if (i > 0) {
      // For each ring, take the list
      for (Sgi::hash_map< keyType, std::list<accessDeviceType> >::iterator vAccesses = vAccessReset.begin() ; vAccesses != vAccessReset.end() ; vAccesses ++) {
	// for each element in the list, modify the word to be set
	for (std::list<accessDeviceType>::iterator itAccessDevice = vAccesses->second.begin() ; itAccessDevice != vAccesses->second.end() ; itAccessDevice ++) {
	  tscType8 word  = itAccessDevice->offset & (1 << i) ;
	  if (word) {
	    tscType8 uval = initialValue & ~word ;
	    // accessDeviceType piaReset = { (*itPia)->getKey(), 0, MODE_WRITE, (*itPia)->getMask(), uval, false, 0, 0, 0, NULL } ;
	    itAccessDevice->data = uval ;
	    itAccessDevice->sent = false ;
	    itAccessDevice->dAck = 0 ;
	    itAccessDevice->fAck = 0 ;
	    itAccessDevice->tnum = 0 ;
	    itAccessDevice->e = 0 ;
	  }
	}
      }
    }

    // send the frames over the ring
    fecAccess.setBlockDevices( vAccessReset, errorList, true ) ;

    // Wait the time for the reset
    usleep (delayActiveReset) ;

    // Now send the intial value to the PIA channels
    // send the frames over the ring
    // For each ring, take the list and send it
    for (Sgi::hash_map< keyType, std::list<accessDeviceType> >::iterator vAccesses = vAccessInialValue.begin() ; vAccesses != vAccessInialValue.end() ; vAccesses ++) {

      // Download the block of frames into the hardware
#ifdef DEBUGMSGERROR
      std::cout << "Download " << vAccesses->second.size() << " PIA reset for the initial valueframes for the ring " << getFecKey(vAccesses->first) << "." << getRingKey(vAccesses->first) << std::endl ;
#endif
      
      if (vAccesses->second.size() > 0 ) {
	
	// reset all the values set by the previous frames sent
	for (std::list<accessDeviceType>::iterator itAccessDevice = vAccesses->second.begin() ; itAccessDevice != vAccesses->second.end() ; itAccessDevice++) {

	  // accessDeviceType piaInitialValue = { (*itPia)->getKey(), 0, MODE_WRITE, 0, PIAINITIALVALUE, false, 0, 0, 0, NULL } ;
	  itAccessDevice->sent = false ;
	  itAccessDevice->dAck = 0 ;
	  itAccessDevice->fAck = 0 ;
	  itAccessDevice->tnum = 0 ;
	  itAccessDevice->e = NULL ;
	}
      }
    }

    // send the frames over the ring
    fecAccess.setBlockDevices( vAccessInialValue, errorList, true ) ;

    // Wait the time between two resets
    usleep (intervalDelayReset) ;
  }
  
  // Wait again the interval delay reset
  usleep (intervalDelayReset) ;

  // return the number of errors
  return (error) ;
}
