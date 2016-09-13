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


#include <time.h> // for nanosleep

#include <iostream>

#include "tscTypes.h"
#include "deviceType.h"
#include "deviceFrame.h"

#include "delay25Definition.h"
#include "delay25Access.h"

// for nanosleep between two loop on the going bit in delay25Reset and delay25Init methods
//int nanosleep(const struct timespec *req, struct timespec *rem);
//static struct timespec DELAY25GOINGBITDELAY = { 0, 5000000 } ; // seconds, nanoseconds (5 ms)

/** Constructor to store the access to the DELAY25
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
delay25Access::delay25Access (FecAccess *fec, 
                              tscType16 fecSlot,
                              tscType16 ringSlot,
                              tscType16 ccuAddress,
                              tscType16 i2cChannel,
                              tscType16 i2cAddress ) : 

  deviceAccess (DELAY25,
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
delay25Access::delay25Access (FecAccess *fec, 
                              keyType key) :

  deviceAccess (DELAY25,
                fec,
                key) {

}


/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
delay25Access::~delay25Access ( ) {

  // ~deviceAccess ( ) ;
}

/** 
 * \param delay25Values - description of the DELAY25
 */
void delay25Access::setDescription ( delay25Description delay25Values ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = delay25Values.clone() ;
}


/** Take a description value of a delay25 and download the values in a specific delay25
 * \param delay25Values - all the values for an laserdriver
 * \exception FecExceptionHandler
 */
void delay25Access::setValues ( delay25Description delay25Values ) {
 
  set40HzFrequencyMode();
  setDelay0 ( delay25Values.getDelay0() ) ;
  setDelay1 ( delay25Values.getDelay1() ) ;
  setDelay2 ( delay25Values.getDelay2() ) ;
  setDelay3 ( delay25Values.getDelay3() ) ;
  setDelay4 ( delay25Values.getDelay4() ) ;
                                  
  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = delay25Values.clone() ;
}

/** Build a description of a delay25 and return it
 * \return the delay25 description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
delay25Description *delay25Access::getValues ( ) throw (FecExceptionHandler) {

  delay25Description *delay25Values = 
    new delay25Description (accessKey_,
                            getDelay0( ),
                            getDelay1( ),
			    getDelay2( ),
                            getDelay3( ),
			    getDelay4( ) ) ;

  delay25Values->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

  return (delay25Values) ;
}


/** Set the DELAY25 (delay 0)
 * \param Value to be set
 * \exception FecExceptionHandler
 * \warning see the source for more explanation
 * \warning setDelayFine
 */
void delay25Access::setDelay0 ( tscType8 delay_step ) throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::setDelay0: Write CR0(0x" << std::hex << (int)CR0 << "<7>) = 0x" << std::hex << (int)delay_step << ")" << std::endl ;
#endif

  // ..and write 
  accessToFec_->writeOffset ( accessKey_, CR0, (delay_step | DELAY25_ENABLE) ) ;
}


/** Set the DELAY25 (delay 1)
 * \param Value to be set
 * \exception FecExceptionHandler
 * \warning see the source for more explanation
 * \warning setDelayFine
 */
void delay25Access::setDelay1 ( tscType8 delay_step ) throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::setDelay1: Write CR1(0x" << std::hex << (int)CR1 << "<7>) = 0x" << std::hex << (int)delay_step << ")" << std::endl ;
#endif

  // ..and write 
  accessToFec_->writeOffset ( accessKey_, CR1, (delay_step | DELAY25_ENABLE) ) ;
}

/** Set the DELAY25 (delay 2)
 * \param Value to be set
 * \exception FecExceptionHandler
 * \warning see the source for more explanation
 * \warning setDelayFine
 */

void delay25Access::setDelay2 ( tscType8 delay_step ) throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::setDelay2: Write CR2(0x" << std::hex << (int)CR2 << "<7>) = 0x" << std::hex << (int)delay_step << ")" << std::endl ;
#endif

  // ..and write 
  accessToFec_->writeOffset ( accessKey_, CR2, (delay_step | DELAY25_ENABLE) ) ;
}

/** Set the DELAY25 (delay 3)
 * \param Value to be set
 * \exception FecExceptionHandler
 * \warning see the source for more explanation
 * \warning setDelayFine
 */

void delay25Access::setDelay3 ( tscType8 delay_step ) throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::setDelay3: Write CR3(0x" << std::hex << (int)CR3 << "<7>) = 0x" << std::hex << (int)delay_step << ")" << std::endl ;
#endif

  // ..and write 
  accessToFec_->writeOffset ( accessKey_, CR3, (delay_step | DELAY25_ENABLE) ) ;
}

/** Set the DELAY25 (delay 4)
 * \param Value to be set
 * \exception FecExceptionHandler
 * \warning see the source for more explanation
 * \warning setDelayFine
 */

void delay25Access::setDelay4 ( tscType8 delay_step ) throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::setDelay4: Write CR4(0x" << std::hex << (int)CR4 << "<7>) = 0x" << std::hex << (int)delay_step << ")" << std::endl ;
#endif

  // ..and write 
  accessToFec_->writeOffset ( accessKey_, CR4, (delay_step | DELAY25_ENABLE) ) ;
}

/** Set the DELAY25 (frequency mode)
 * \param Value to be set
 * \exception FecExceptionHandler
 * \warning see the source for more explanation
 * \warning setDelayFine
 */

void delay25Access::set40HzFrequencyMode ( ) throw (FecExceptionHandler) {

  // Convert delay step
  tscType8 mode_val = getFrequencyModeMap ( PixelDelay25FrequencyMode ) ;

#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::setFrequencyMode: Write GCR(0x" << std::hex << (int)GCR << "<7>) = 0x" << std::hex << (int)mode_val << ")" << std::endl ;
#endif

  // ..and write 
  accessToFec_->writeOffset ( accessKey_, GCR, mode_val ) ;
}

/** Get the value of the DELAY25 (delay 0)
 * \return the value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
tscType8 delay25Access::getDelay0 ( ) throw (FecExceptionHandler) {

  tscType8 i2cdata    = accessToFec_->readOffset ( accessKey_, CR0 ) ; // Read CR0
  tscType8 delay_step = i2cdata & DELAY25_DELAY ; // Read CR0<5:0>
       
#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::getDelay0: Value read in the CR0 register = 0x" << std::hex << (int)i2cdata << std::endl ;
  std::cout << "delay25Access::getDelay0: Chosen delay step in the CR0 register = 0x" << std::hex << (int)delay_step << std::endl ;
#endif
  
  return (delay_step) ;
}

/** Get the value of the DELAY25 (delay 1)
 * \return the value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */

tscType8 delay25Access::getDelay1 ( ) throw (FecExceptionHandler) {

  tscType8 i2cdata    = accessToFec_->readOffset ( accessKey_, CR1 ) ; // Read CR1
  tscType8 delay_step = i2cdata & DELAY25_DELAY ; // Read CR1<5:0>
       
#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::getDelay1: Value read in the CR1 register = 0x" << std::hex << (int)i2cdata << std::endl ;
  std::cout << "delay25Access::getDelay1: Chosen delay step in the CR1 register = 0x" << std::hex << (int)delay_step << std::endl ;
#endif
  
  return (delay_step) ;
}

/** Get the value of the DELAY25 (delay 2)
 * \return the value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */

tscType8 delay25Access::getDelay2 ( ) throw (FecExceptionHandler) {

  tscType8 i2cdata    = accessToFec_->readOffset ( accessKey_, CR2 ) ; // Read CR2
  tscType8 delay_step = i2cdata & DELAY25_DELAY ; // Read CR2<5:0>
       
#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::getDelay2: Value read in the CR2 register = 0x" << std::hex << (int)i2cdata << std::endl ;
  std::cout << "delay25Access::getDelay2: Chosen delay step in the CR2 register = 0x" << std::hex << (int)delay_step << std::endl ;
#endif
  
  return (delay_step) ;
}

/** Get the value of the DELAY25 (delay 3)
 * \return the value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */

tscType8 delay25Access::getDelay3 ( ) throw (FecExceptionHandler) {

  tscType8 i2cdata    = accessToFec_->readOffset ( accessKey_, CR3 ) ; // Read CR3
  tscType8 delay_step = i2cdata & DELAY25_DELAY ; // Read CR3<5:0>
       
#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::getDelay3: Value read in the CR3 register = 0x" << std::hex << (int)i2cdata << std::endl ;
  std::cout << "delay25Access::getDelay3: Chosen delay step in the CR3 register = 0x" << std::hex << (int)delay_step << std::endl ;
#endif
  
  return (delay_step) ;
}

/** Get the value of the DELAY25 (delay 4)
 * \return the value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */

tscType8 delay25Access::getDelay4 ( ) throw (FecExceptionHandler) {

  tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CR4 ) ; // Read CR4
  tscType8 delay_step = i2cdata & DELAY25_DELAY ; // Read CR4<5:0>
       
#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::getDelay4: Value read in the CR4 register = 0x" << std::hex << (int)i2cdata << std::endl ;
  std::cout << "delay25Access::getDelay4: Chosen delay step in the CR4 register = 0x" << std::hex << (int)delay_step << std::endl ;
#endif
  
  return (delay_step) ;
}


/** Get the value of the DELAY25 frequency mode
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType8 delay25Access::getFrequencyMode ( ) throw (FecExceptionHandler) {

  tscType8 i2cdata  = accessToFec_->readOffset ( accessKey_, GCR ) ; // Read GCR
  tscType8 mode_val = i2cdata & DELAY25_CLOCKFREQUENCY ; //Read GCR<1:0> 

#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::getFrequencyMode: Value read in the GCR register = 0x" << std::hex << (int)i2cdata << std::endl ;
  std::cout << "delay25Access::getFrequencyMode: Chosen frequency mode in the GCR register = 0x" << std::hex << (int)mode_val << std::endl ;
#endif 
 
  tscType8 mode_ = getFrequencyModeMapBack ( mode_val ) ; // Convert the value in step

#ifdef DEBUGMSGERROR
  std::cout << "delay25Access::getFrequencyMode: Frequency mode set in GCR = 0x" << std::hex << (int)mode_val << std::endl ;
  std::cout << "delay25Access::getFrequencyMode: mode = " << std::hex << (int)mode_ << std::endl ;
#endif
  
  return (mode_) ;
 
}


/** Reset/Initialise the DELAY25
 * \exception FecExceptionHandler
 * \warning Check CTR1<0>:  Read-Only, =1 after a normal reset sequence
 * \warning Original code was: bool tsc_delay25_check_status(tsc_descriptor_index  delay25)
 * \warning DELAY25 control: comment: update to match the Official Fec Driver library and new Delay25 
 */
/*tscType8 delay25Access::delay25Reset ( ) throw (FecExceptionHandler) {


  tscType8 reset = 0;
 
  return reset;
  }*/


/** Routine converting the read-back coarse delay value in CTR3 to a delay step
 * \param k - frequency to be convert
 */
tscType8 delay25Access::getFrequencyModeMapBack ( tscType8 k ) {

  tscType8 mode = 40;
	
  if ( k == 0 )    mode = 40;
  else if (k == 1) mode = 80; 
  else if (k == 2) mode = 32;
  else if (k == 3) mode = 64;
  return(mode) ;
}

/** Routine convering the coarse delay step for input to CTR3 when CTR1<5>=1 
 * \param k - frequency mode
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
tscType8 delay25Access::getFrequencyModeMap ( tscType8 k ) throw (FecExceptionHandler) {

  tscType8 mode;
	
  if ( k == 40 )    mode = 0x00;
  else if (k == 80) mode = 0x01; 
  else if (k == 32) mode = 0x02;
  else if (k == 64) mode = 0x03;
  else {  
    RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR, "The frequency mode (orginal value) cannot be used (value must be 40, 80, 32 or 64)", ERRORCODE) ;
  }

  return(mode) ;
}


/** Get the value of the CR0 register (enable and delay0) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 delay25Access::getCR0 ( ) throw (FecExceptionHandler) {

  // Read CTR1
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CR0 ) ;
  return i2cdata;
}

/** Get the value of the CR1 register (enable and delay1) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 delay25Access::getCR1 ( ) throw (FecExceptionHandler) {

  // Read CTR1
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CR1 ) ;
  return i2cdata;
}

/** Get the value of the CR2 register (enable and delay2) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 delay25Access::getCR2 ( ) throw (FecExceptionHandler) {

  // Read CTR2
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CR2 ) ;
  return i2cdata;
}

/** Get the value of the CR3 register (enable and delay3) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 delay25Access::getCR3 ( ) throw (FecExceptionHandler) {

  // Read CTR3
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CR3 ) ;
  return i2cdata;
}

/** Get the value of the CR4 register (enable and delay4) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 delay25Access::getCR4 ( ) throw (FecExceptionHandler) {

  // Read CTR1
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CR4 ) ;
  return i2cdata;
}

/** Get the value of the GCR register (mode and idll) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 delay25Access::getGCR ( ) throw (FecExceptionHandler) {

  // Read CTR1
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, GCR ) ;
  return i2cdata;
}

/** Set the value of the CR0 register (enable and delay0) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void delay25Access::setCR0 ( tscType8 value ) throw (FecExceptionHandler) {

  // Write CR0
  accessToFec_->writeOffset ( accessKey_, CR0, value ) ;
}

/** Set the value of the CR1 register (enable and delay1) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void delay25Access::setCR1 ( tscType8 value ) throw (FecExceptionHandler) {

  // Write CR1
  accessToFec_->writeOffset ( accessKey_, CR1, value ) ;
}

/** Set the value of the CR2 register (enable and delay2) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void delay25Access::setCR2 ( tscType8 value ) throw (FecExceptionHandler) {

  // Write CR2
  accessToFec_->writeOffset ( accessKey_, CR2, value ) ;
}

/** Set the value of the CR3 register (enable and delay3) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void delay25Access::setCR3 ( tscType8 value ) throw (FecExceptionHandler) {

  // Write CR3
  accessToFec_->writeOffset ( accessKey_, CR3, value ) ;
}

/** Set the value of the CR4 register (enable and delay4) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void delay25Access::setCR4 ( tscType8 value ) throw (FecExceptionHandler) {

  // Write CR4
  accessToFec_->writeOffset ( accessKey_, CR4, value ) ;
}

/** Set the value of the GCR register (mode and idll) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void delay25Access::setGCR ( tscType8 value ) throw (FecExceptionHandler) {

  // Write GCR
  accessToFec_->writeOffset ( accessKey_, GCR, value ) ;
}
 

/** Display the different values of the registers for a given set of DELAY25
 * \param fecAccess - hardware access
 * \param delay25Set - set of DELAY25
 */
void delay25Access::displayDelay25Registers ( FecAccess &fecAccess, Sgi::hash_map<keyType, delay25Access *> &delay25Set ) {

  for ( Sgi::hash_map<keyType, delay25Access *>::iterator itDelay25 = delay25Set.begin() ; itDelay25 != delay25Set.end() ; itDelay25 ++ ) {

    try {
      int cr0     = itDelay25->second->getCR0() ;
      int cr1     = itDelay25->second->getCR1() ;
      int cr2     = itDelay25->second->getCR2() ;
      int cr3     = itDelay25->second->getCR3() ;
      int cr4     = itDelay25->second->getCR4() ;
      int gcr     = itDelay25->second->getGCR() ;
    
      std::cout << "FEC=0-Slot=" << std::dec << getFecKey(itDelay25->first) 
		<< "-Ring=" << getRingKey(itDelay25->first) 
		<< "-Ccu=" << getCcuKey(itDelay25->first) 
		<< "-Channel=" << getChannelKey(itDelay25->first) 
		<< ": " << std::hex << "\t0x" << cr0 << "\t0x" << cr1 << "\t0x" << cr2 << "\t0x" << cr3 << "\t0x" << cr4 << "\t0x" << gcr << std::dec << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      std::cout << "FEC=0-Slot=" << std::dec << getFecKey(itDelay25->first) 
		<< "-Ring=" << getRingKey(itDelay25->first) 
		<< "-Ccu=" << getCcuKey(itDelay25->first) 
		<< "-Channel=" << getChannelKey(itDelay25->first) 
		<< ": unable to read one of the DELAY25 register: " << e.what() << std::endl ;
    }
  }
 
}

/** Display the different values of the registers for a given set of DELAY25
 * \param fecAccess - hardware access
 * \param delay25Set - set of DELAY25
 */
void delay25Access::displayDelay25Registers ( ) {

    int cr0     = this->getCR0() ;
    int cr1     = this->getCR1() ;
    int cr2     = this->getCR2() ;
    int cr3     = this->getCR3() ;
    int cr4     = this->getCR4() ;
    int gcr     = this->getGCR() ;
    
    std::cout << "FEC=0-Slot=" << std::dec << getFecKey(getKey()) 
  	      << "-Ring=" << getRingKey(getKey()) 
	      << "-Ccu=" << getCcuKey(getKey()) 
	      << "-Channel=" << getChannelKey(getKey()) 
	      << ": " << std::hex << "\t0x" << cr0 << "\t0x" << cr1 << "\t0x" << cr2 << "\t0x" << cr3 << "\t0x" << cr4 << "\t0x" << gcr << std::dec << std::endl ;
}

/** Take a description value of a DELAY25 a build a block of frames to be set in the hardware
 * \param delay25Values - all the values for a delay25 
 * \param vAccess - block of frames
 */
void delay25Access::getBlockWriteValues ( delay25Description delay25Values, accessDeviceTypeList &vAccess ) {

  tscType8 delay_0       = ( delay25Values.getDelay0() | DELAY25_ENABLE ) ;
  tscType8 delay_1       = ( delay25Values.getDelay1() | DELAY25_ENABLE ) ;
  tscType8 delay_2       = ( delay25Values.getDelay2() | DELAY25_ENABLE ) ;
  tscType8 delay_3       = ( delay25Values.getDelay3() | DELAY25_ENABLE ) ;
  tscType8 delay_4       = ( delay25Values.getDelay4() | DELAY25_ENABLE ) ;
  
  accessDeviceType ctrl0 = { getKey(), NORMALMODE, MODE_WRITE, CR0, delay_0, false, 0, 0, 0, NULL} ;
  vAccess.push_back (ctrl0) ; 
  
  accessDeviceType ctrl1 = { getKey(), NORMALMODE, MODE_WRITE, CR1, delay_1, false, 0, 0, 0, NULL} ;
  vAccess.push_back (ctrl1) ;  

  accessDeviceType ctrl2 = { getKey(), NORMALMODE, MODE_WRITE, CR2, delay_2, false, 0, 0, 0, NULL} ;
  vAccess.push_back (ctrl2) ;  

  accessDeviceType ctrl3 = { getKey(), NORMALMODE, MODE_WRITE, CR3, delay_3, false, 0, 0, 0, NULL} ;
  vAccess.push_back (ctrl3) ; 

  accessDeviceType ctrl4 = { getKey(), NORMALMODE, MODE_WRITE, CR4, delay_4, false, 0, 0, 0, NULL} ;
  vAccess.push_back (ctrl4) ;  


}


/** This static method read out several DELAY25 at the same time
 * \param fecAccess - hardware access
 * \param delay25Set - all the DELAY25 to be readout
 * \param delay25Vector - list of the readout DELAY25 (suppose to be empty at the beginning). The delay25Description created must be delete by the remote method.
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */ 
unsigned int delay25Access::getDelay25ValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, delay25Access *> &delay25Set, deviceVector &delay25Vector, std::list<FecExceptionHandler *> &errorList ) 
  throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, delay25Description * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesDelay0 ;
  accessDeviceTypeListMap vAccessesDelay1 ;
  accessDeviceTypeListMap vAccessesDelay2 ;
  accessDeviceTypeListMap vAccessesDelay3 ;
  accessDeviceTypeListMap vAccessesDelay4 ;

  for ( Sgi::hash_map<keyType, delay25Access *>::iterator itDelay25 = delay25Set.begin() ; itDelay25 != delay25Set.end() ; itDelay25 ++ ) {

    // Read all CR0
    accessDeviceType ctrl0 = { itDelay25->second->getKey(), NORMALMODE, MODE_READ, CR0, 0, false, 0, 0, 0, NULL} ;
    vAccessesDelay0[getFecRingKey(itDelay25->second->getKey())].push_back(ctrl0) ;
    
    // Put it in a map and in the vector
    deviceDescriptionsMap[itDelay25->second->getKey()] = new delay25Description ( itDelay25->second->getKey() ) ;
    delay25Vector.push_back(deviceDescriptionsMap[itDelay25->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itDelay25->second->getKey()]->setFecHardwareId ( itDelay25->second->getFecHardwareId(), itDelay25->second->getCrateId() ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesDelay0, errorList ) ;

  // Collect the answer and fill the corresponding delay25Description
  for (accessDeviceTypeListMap::iterator itList = vAccessesDelay0.begin() ; itList != vAccessesDelay0.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // set the delay0 of the delay25
      deviceDescriptionsMap[itDevice->index]->setDelay0 ( (data & DELAY25_DELAY) | DELAY25_ENABLE ) ;

      // build the next frames to be sent only for correct delay0
      if (itDevice->e == NULL) {

	// build the part for the read of the control register 1
	accessDeviceType ctrl1 = { itDevice->index, NORMALMODE, MODE_READ, CR1, 0, false, 0, 0, 0, NULL} ;
	vAccessesDelay1[getFecRingKey(itDevice->index)].push_back (ctrl1) ;
      }
    }
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesDelay1, errorList ) ;

  // Collect the answer and fill the corresponding delay25Description
  for (accessDeviceTypeListMap::iterator itList = vAccessesDelay1.begin() ; itList != vAccessesDelay1.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // set the delay1 of the delay25
      deviceDescriptionsMap[itDevice->index]->setDelay1 ( (data & DELAY25_DELAY) | DELAY25_ENABLE ) ; 

      // build the next frames to be sent only for correct delay1
      if (itDevice->e == NULL) {

	// build the part for the read of the control register 2
	accessDeviceType ctrl2 = { itDevice->index, NORMALMODE, MODE_READ, CR2, 0, false, 0, 0, 0, NULL} ;
	vAccessesDelay2[getFecRingKey(itDevice->index)].push_back (ctrl2) ;
      }
    }
  } 


  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesDelay2, errorList ) ;

  // Collect the answer and fill the corresponding delay25Description
  for (accessDeviceTypeListMap::iterator itList = vAccessesDelay2.begin() ; itList != vAccessesDelay2.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // set the delay2 of the delay25
      deviceDescriptionsMap[itDevice->index]->setDelay2 ( (data & DELAY25_DELAY) | DELAY25_ENABLE ) ; 

      // build the next frames to be sent only for correct delay2
      if (itDevice->e == NULL) {

	// build the part for the read of the control register 3
	accessDeviceType ctrl3 = { itDevice->index, NORMALMODE, MODE_READ, CR3, 0, false, 0, 0, 0, NULL} ;
	vAccessesDelay3[getFecRingKey(itDevice->index)].push_back (ctrl3) ;
      }
    }
  } 


  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesDelay3, errorList ) ;

  // Collect the answer and fill the corresponding delay25Description
  for (accessDeviceTypeListMap::iterator itList = vAccessesDelay3.begin() ; itList != vAccessesDelay3.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // set the delay3 of the delay25
      deviceDescriptionsMap[itDevice->index]->setDelay3 ( (data & DELAY25_DELAY) | DELAY25_ENABLE ) ; 

      // build the next frames to be sent only for correct delay3
      if (itDevice->e == NULL) {

	// build the part for the read of the control register 4
	accessDeviceType ctrl4 = { itDevice->index, NORMALMODE, MODE_READ, CR4, 0, false, 0, 0, 0, NULL} ;
	vAccessesDelay4[getFecRingKey(itDevice->index)].push_back (ctrl4) ;
      }
    }
  } 


  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesDelay4, errorList ) ;

  // Collect the answer and fill the corresponding delay25Description
  for (accessDeviceTypeListMap::iterator itList = vAccessesDelay4.begin() ; itList != vAccessesDelay4.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      // set the delay4 of the delay25
      deviceDescriptionsMap[itDevice->index]->setDelay4 ( (data & DELAY25_DELAY) | DELAY25_ENABLE ) ; 

      
    }
  }

  return (error) ;
}


void delay25Access::forceDLLResynchronisation ( ) throw (FecExceptionHandler) {
 
  // Read GCR
  tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, GCR ) ; 
  // Read Frequency Mode
  tscType8 mode_val = i2cdata & DELAY25_CLOCKFREQUENCY ; 

  //write GCR bit 6
  accessToFec_->writeOffset ( accessKey_, GCR, mode_val | DELAY25_GCR_IDLL ) ;
}

void delay25Access::SetIDLLBitToZero ( ) throw (FecExceptionHandler) {
 
  // Read GCR
  tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, GCR ) ; 
  // Read Frequency Mode
  tscType8 mode_val = i2cdata & DELAY25_CLOCKFREQUENCY ; 

  //write GCR bit 6
  accessToFec_->writeOffset ( accessKey_, GCR, mode_val ) ;
}
