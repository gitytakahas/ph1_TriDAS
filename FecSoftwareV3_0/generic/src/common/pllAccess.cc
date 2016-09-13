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
#include <unistd.h>

#include <iostream>

#include "tscTypes.h"
#include "deviceType.h"
#include "deviceFrame.h"

#include "pllDefinition.h"
#include "pllAccess.h"

// for nanosleep between two loop on the going bit in pllReset and pllInit methods
//int nanosleep(const struct timespec *req, struct timespec *rem);
static struct timespec PLLGOINGBITDELAY = { 0, 5000000 } ; // seconds, nanoseconds (5 ms)

/** Constructor to store the access to the PLL
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
pllAccess::pllAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress ) : 

  deviceAccess (PLL,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress ) {

  pllDac_ = PLLDAC ;

}

/** Constructor to store the access to the APV (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
pllAccess::pllAccess (FecAccess *fec, 
                      keyType key) :

  deviceAccess (PLL,
                fec,
                key) {

  pllDac_ = PLLDAC ;

}


/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
pllAccess::~pllAccess ( ) {

  // ~deviceAccess ( ) ;
}

/** 
 * \param pllValues - description of the PLL
 */
void pllAccess::setDescription ( pllDescription pllValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = pllValues.clone() ;

  pllDac_ = pllValues.getPllDac() ;
}

tscType8 pllAccess::getTimeValue ( ) {

  pllDescription *pllD = (pllDescription *)deviceValues_ ;
  tscType8 value = pllD->getDelayFine() + pllD->getDelayCoarse()*24 ;
  
  return value ;
}

/** Take a description value of a pll and download the values in a specific pll
 * \param pllValues - all the values for an laserdriver
 * \exception FecExceptionHandler
 */
void pllAccess::setValues ( pllDescription pllValues ) {

  setClockPhase ( pllValues.getClockPhase() ) ;
  setTriggerDelay ( pllValues.getTriggerDelay () ) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = pllValues.clone() ;

  pllDac_ = pllValues.getPllDac() ;
}

/** Build a description of a pll and return it
 * \return the pll description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
pllDescription *pllAccess::getValues ( ) throw (FecExceptionHandler) {

  pllDescription *pllValues = 
    new pllDescription (accessKey_,
                        getClockPhase( ),
                        getTriggerDelay( ),
			pllDac_) ;

  pllValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

  return (pllValues) ;
}

/** Set the PLL daq parameter (no hardware access is done here)
 * \param pllDac - PLL DAC value
 */
void pllAccess::setPllDac ( tscType8 pllDac ) { pllDac_ = pllDac ; }

/** Fet the PLL daq parameter (no hardware access is done here)
 * \return PLL DAQ value
 */
tscType8 pllAccess::getPllDac ( ) { return pllDac_ ; }

/** Set the PLL clock phase (delay fine)
 * \param Value to be set
 * \exception FecExceptionHandler
 * \warning see the source for more explanation
 * \warning setDelayFine
 */
void pllAccess::setClockPhase ( tscType8 delay_step ) throw (FecExceptionHandler) {

  // Convert delay step
  tscType8 delay_val = getClockPhaseMap ( delay_step ) ;

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::setClockPhase ( " << std::dec << delay_val << ")" << std::endl ;
#endif

  // Choose manual delay and value.
  tscType8 i2cdata = PLL_CTRL2_I2CGOING | delay_val ;

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::setClockPhase: Write CNTRL_2(0x" << std::hex << (int)CNTRL_2 << "<7>) = 0x" << std::hex << (int)i2cdata << ")" << std::endl ;
#endif

  // ..and write 
  accessToFec_->writeOffset ( accessKey_, CNTRL_2, i2cdata ) ;
}

/** Set the PLL trigger delay (delay coarse)
 * \param Value to be set
 * \exception FecExceptionHandler
 * \warning see the source for more explanation
 * \warning setDelayCoarse
 */
void pllAccess::setTriggerDelay ( tscType8 delay_step ) throw (FecExceptionHandler) {

  // Convert delay step
  tscType8 delay_val = getTriggerDelayMap ( delay_step ) ;

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::setTriggerDelay ( " << std::dec << delay_val << ")" << std::endl ;
#endif

  // Read CTR2
  tscType8 ctrl2Orig = accessToFec_->readOffset ( accessKey_, CNTRL_2 ) ;
  // Set CTR2<5>=1..
  tscType8 ctrl2= ctrl2Orig | PLL_CTRL2_RSEL ;

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::setTriggerDelay: Write CNTRL_2(0x" << std::hex << (int)CNTRL_2 << "<5>) = 0x" << std::hex << (int)ctrl2 << ")" << std::endl ;
#endif

  // ..and write 
  accessToFec_->writeOffset ( accessKey_, CNTRL_2, ctrl2 ) ;

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::setTriggerDelay: Write CNTRL_4(0x" << std::hex << (int)CNTRL_4 << ") = 0x" << std::hex << (int)delay_val << ")" << std::endl ;
#endif

  // ...and write the CTRL4
  accessToFec_->writeOffset ( accessKey_, CNTRL_4, delay_val ) ;

  // set the original value in the cntrl2 => not necessary
  // accessToFec_->writeOffset (accessKey_, CNTRL_2, cntrl2Orig) ; // and write it
}

/** Get the value of the PLL clock phase (delay fine)
 * \return the value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
tscType8 pllAccess::getClockPhase ( ) throw (FecExceptionHandler) {

  tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_2 ) ; // Read CTR2
  tscType8 delay_val = i2cdata & PLL_CLOCKPHASE_READ ; // Read CRT2<4:0>
       
#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::getClockPhase: Value read in the CNTRL_2 register = 0x" << std::hex << i2cdata << std::endl ;
  std::cout << "pllAccess::getClockPhase: Chosen fine delay in the CNTRL_1 register = 0x" << std::dec << delay_val << std::endl ;
#endif

  tscType8 delay_step = getClockPhaseMapBack ( delay_val ) ; // Convert the value in step
  if (delay_step == 99) {

#ifdef DEBUGMSGERROR
    std::cerr << "pllAccess::getClockPhase: ERROR: Invalid delay value read from CTR<4:0> = 0x" << std::hex << delay_val << std::endl ;
    std::cerr << "pllAccess::getClockPhase: ERROR: Error code in conversion = " << std::dec << delay_step << std::endl ;
#endif
    
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
				       "Invalid delay value read from CTR<4:0> (Error code in conversion)",
				       ERRORCODE,
				       accessKey_, 
				       "delay step value", delay_step ) ;
  } 
  
#ifdef DEBUGMSGERROR
  else {
    std::cout << "pllAccess::getClockPhase: Chosen FINE delay step (1-24) = " << std::dec << delay_step << " -> 0x" << std::hex << delay_val << std::endl ;
  }
#endif
  
  return (delay_step) ;
}

/** Get the value of the PLL trigger delay (delay coarse)
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType8 pllAccess::getTriggerDelay ( ) throw (FecExceptionHandler) {

  // Read CTR2
  tscType8 cntrl2Orig = accessToFec_->readOffset ( accessKey_, CNTRL_2 ) ;
  // Set CTR2<5>=1..
  tscType8 cntrl2 = cntrl2Orig | PLL_CTRL2_RSEL ;

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::getTriggerDelay: Write CNTRL_2(0x" << std::hex << (int)CNTRL_2 << "<5>) = 0x" << std::hex << (int)cntrl2 << ")" << std::endl ;
#endif

  // ...and write it
  accessToFec_->writeOffset (accessKey_, CNTRL_2, cntrl2) ;

  // Read CTR4
  tscType8 cntrl4 = accessToFec_->readOffset (accessKey_, CNTRL_4) ;

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::getTriggerDelay: Data read from CNTRL_4 = 0x" << std::hex << (int)cntrl4 << std::endl ;
#endif

  tscType8 delay_val = cntrl4 & PLL_TRIGGERDELAY_READ;		// Read CTR4<3:0>

  // Convert value
  tscType8 delay_step = getTriggerDelayMapBack (delay_val) ;

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::getTriggerDelay: Trigger delay set in CNTRL4 = 0x" << std::hex << delay_val << std::endl ;
  std::cout << "pllAccess::getTriggerDelay: delay step = " << std::dec << delay_step << std::endl ;
#endif

  // set the original value in the cntrl2 => not necessary
  // accessToFec_->writeOffset (accessKey_, CNTRL_2, cntrl2Orig) ; // and write it
  
  return (delay_step) ;
}

/** Check the going bit and make a PLL reset or a PLL init depending of the following parameters
 * \param noCheck - if noCheck is false then the reset is apply even if the GOING is there by default true so a reset is issued
 * \param coldReset - issue a cold reset if it is true else issue a PLL reset (default false)
 * \see pllReset and pllInit
 */
void pllAccess::setPllCheckGoingBit ( bool noCheck, bool coldReset ) throw (FecExceptionHandler) {

  bool resetPll = true ;
  if (! noCheck) 
    if (getCNTRL1() & PLL_CTRL1_GOING) resetPll = false ;
  
#ifdef DEBUGMSGERROR
  char msg[80] ;
  decodeKey (msg,getKey()) ;
  std::cout << "PLL " << msg << " control register 1 " << getCNTRL1() << " going bit = " << (getCNTRL1() & PLL_CTRL1_GOING) << std::endl ;
#endif

  if (resetPll) pllInit ( ) ;
}

/** Reset/Initialise the PLL
 * \exception FecExceptionHandler
 * \warning Check CTR1<0>:  Read-Only, =1 after a normal reset sequence
 * \warning Original code was: bool tsc_pll_check_status(tsc_descriptor_index  pll)
 * \warning PLL control: comment: update to match the Official Fec Driver library and new Pll 
 */
tscType8 pllAccess::pllReset ( ) throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << "pllAccess::pllReset: Write CNTRL_1(0x" << std::hex << (int)CNTRL_1 << "<3>) = RST_ON (0x" << std::hex << (int)PLL_RESET_ON << ")" << std::endl ;
#endif

  // Write CTR1<3>=1
  accessToFec_->writeOffset ( accessKey_, CNTRL_1, PLL_RESET_ON ) ;

#ifdef DEBUGMSGERROR
  std::cout << "Write CNTRL_1(0x" << std::hex << (int)CNTRL_1 << "<3>) = RST_ON (0x" << std::hex << (int)PLL_RESET_OFF << ")" << std::endl ;
#endif

  // Write CTR1<3>=0
  accessToFec_->writeOffset ( accessKey_, CNTRL_1, PLL_RESET_OFF ) ;

  // Wait for 200 ms
  usleep (PLLDELAY) ;

  // Wait for the GOING bit for normal operation
  tscType8 cntrl1 = getCNTRL1 ( ) ;
  unsigned int timeout = 0 ;
  while ( !(cntrl1 & PLL_CTRL1_GOING) && (timeout++ < PLLTIMEOUT) ) {
    cntrl1 = getCNTRL1 ( ) ;
    nanosleep ( &PLLGOINGBITDELAY, NULL ) ; // 5 ms
  }

  if (!(cntrl1 & PLL_CTRL1_GOING)) 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( XDAQFEC_PLLOPERATIONAL,
				       XDAQFEC_PLLOPERATIONAL_MSG,
				       ERRORCODE,
				       accessKey_, 
				       "Control register 1", cntrl1 ) ;

  return cntrl1 ;
}

/** Cold reset for PLL and waiting for  wait for the going bit in PLL control register 1
 * \exception FecExceptionHandler
 * \author W. Beaumont, S. Marchioro, P. Moreira
 *           ORG  V1   V3   V4
 * 1 - write 0x80 0x80 0x80 0x80 to reg 1
 * 2 - write 0x80 0xD0 0xC0 0xC0 to reg 2    
 * 3 - write 0x84 0x84 0x84 0x84 to reg 1    
 * 4 - write 0xFF 0xFF 0xFF 0xFF to reg 3    
 * 5 - write 0x00 0x00 0x00 0x00 to reg 4    
 *                          5 ms
 * 6 - write 0x80 0x80 0x80 0x80 to reg 2    
 * 7 - write 0x80 0x80 0x00 0x80 to reg 1  
 *           V5
 * 1 - write 0x80                to reg 1
 * 2 - write 0x40                to reg 2
 * 3 - write 0xFF                to reg 3
 * 4 - write 0x00                to reg 4
 *     wait 5 ms
 * 5 - write 0x80                to reg 2
 *     wait 5 ms
 * ----------------------------------------------------------
 *           V6
 * 1) write 0x80 to reg 1
 *       => Sets the test mode
 *       => Enables T1
 *       => Does NOT force PFD
 * 2) write 0x00 to reg 2
 *       => Not going: that is, the PFD is used to acquire lock (with the correct phase)
 *       => Low gain (If high gain was to be used than the code would be 0x40 instead of 0x00)
 * 3) write PLLDAC to reg 3 (see the define in pllAccess.h)
 *       => Sets the VCO gain (low bits)
 * 4) write 0x00 to reg 4 and wait 5 ms (please read COMPLETELY the comment below)
 *       => Sets the VCO gain (high bits)
 * W) Wait 5000 us
 * 5) write 0x80 to reg 2
 *       => Going: that is, the PD is used for normal operation (with the correct phase)
 *       => Low gain (If high gain was to be used than the code would be 0xC0 instead of 0x80) 
 * Implementation of V6 is done here
 */
tscType8 pllAccess::pllInit ( ) throw (FecExceptionHandler) {

#ifdef PLLCOLDINITVERSION3
  std::cout << "Version 3 of the PLL reset mono frame" << std::endl ;
  // "Special sequence"
  // 1 - write 0x80 to reg 1
  accessToFec_->writeOffset ( accessKey_, CNTRL_1, 0x80);
  // 2 - write 0xC0 to reg 2
  accessToFec_->writeOffset ( accessKey_, CNTRL_2, 0xC0) ;
  // 3 - write 0x84 to reg 1
  accessToFec_->writeOffset ( accessKey_, CNTRL_1, 0x84 );
  // 4 - write 0xFF to reg 3
  accessToFec_->writeOffset ( accessKey_, CNTRL_3, pllDac_ );
  // 5 - write 0xFF to reg 4
  accessToFec_->writeOffset ( accessKey_, CNTRL_4, 0x00 );
  // 6 - write 0xFF to reg 4
  accessToFec_->writeOffset ( accessKey_, CNTRL_2, 0x80 );
  // 6 - write 0xFF to reg 4
  accessToFec_->writeOffset ( accessKey_, CNTRL_1, 0x00 );
#endif

#ifdef PLLCOLDINITVERSION6
    std::cout << "Version 6 of the PLL reset mono frame" << std::endl ;
  // "Special sequence"
  // 1 - write 0x80 to reg 1
  accessToFec_->writeOffset ( accessKey_, CNTRL_1, PLL_CTRL1_MODE);
  // 2 - write 0x00 to reg 2
  accessToFec_->writeOffset ( accessKey_, CNTRL_2, 0x00) ;
    
  // tscType8 dacbyte=0xFF & pllDac ;
  
  // 3 - write 0xFF to reg 3
  accessToFec_->writeOffset ( accessKey_, CNTRL_3, pllDac_ );

  // 4 - write 0x00 to reg 4
  accessToFec_->writeOffset ( accessKey_, CNTRL_4, 0x00 );

  // W) Wait for 5000 ms
  usleep (PLLDELAY) ;

  // 5 - write 0x80 to reg 2
  accessToFec_->writeOffset ( accessKey_, CNTRL_2, PLL_CTRL2_I2CGOING ) ;
    
  // W) Wait for 5000 us
  usleep (PLLDELAY) ;
#endif
    
  // Wait for the GOING bit for normal operation
  tscType8 cntrl1 = getCNTRL1 ( ) ; 
  unsigned int timeout = 0 ;
  
  //char msg[80] ; decodeKey(msg, accessKey_) ;
  
  while ( !(cntrl1 & PLL_CTRL1_GOING) && (timeout++ < PLLTIMEOUT) ) {
  
    nanosleep ( &PLLGOINGBITDELAY, NULL ) ; // 5 ms
    //std::cout << "Timeout = " << std::dec << timeout << " (cntrl1 = " << std::hex << (int)cntrl1 << ") for PLL " << msg << std::endl ;
    
    cntrl1 = getCNTRL1 ( ) ;
  }

#ifdef DEBUGMSGERROR
  char msg[80] ;
  decodeKey (msg,getKey()) ;
  std::cout << "PLL " << msg << " control register 1 " << getCNTRL1() << " going bit = " << (getCNTRL1() & PLL_CTRL1_GOING) << std::endl ;
#endif
  
  if (!(cntrl1 & PLL_CTRL1_GOING)) 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( XDAQFEC_PLLOPERATIONAL,
				       XDAQFEC_PLLOPERATIONAL_MSG,
				       ERRORCODE,
				       accessKey_, 
				       "Control register 1", cntrl1 ) ;

  //std::cout << "PLL cntrl1 = 0x" << std::hex << (int)cntrl1 << std::endl ;
  
  return cntrl1 ;
}

/** Routine converting the read-back coarse delay value in CTR3 to a delay step
 * \param k - coarse delay to be convert
 * \warning Original code was: unsigned short tsc_pll_coarse_delay_map_back(tscType8 k)
 */
tscType8 pllAccess::getTriggerDelayMapBack ( tscType8 k ) {

  tscType8 dly_back[]={0, 1, 2, 3, 
		       4, 5, 6, 7,         
		       8, 9, 10, 11, 
		       12, 13, 14, 15};
  return(dly_back[k]);
}

/** Routine converting the read-back fine delay value in CTR1 to a delay step
 * \param k - fine delay to be convert
 * \warning Original code was: unsigned short tsc_pll_fine_delay_map_back(int k)
 */
tscType8 pllAccess::getClockPhaseMapBack ( tscType8 k ) {

  tscType8 dly_back[]={0, 1, 2, 3, 4,  5,
		       6, 7, 8, 9, 10, 11,            
		       99, 99, 99, 99, 12, 13,
		       14, 15, 16, 17, 18, 19, 
		       20, 21, 22, 23};
	
  return(dly_back[k]);
}

/** Routine converting fine delay step for input to CTR2 
 * \param k - fine delay step
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning Original code was: unsigned short tsc_pll_fine_delay_map(tscType8 k)
 */
tscType8 pllAccess::getClockPhaseMap ( tscType8 k ) throw (FecExceptionHandler) {
  
  tscType8 dly[] = {0x00,0x01,0x02,0x03,0x04,0x05,
		    0x06,0x07,0x08,0x09,0x0a,0x0b,            
		    0x10,0x11,0x12,0x13,0x14,0x15,
		    0x16,0x17,0x18,0x19,0x1a,0x1b};
  
  if ( k > 23 ) {
    RAISEFECEXCEPTIONHANDLER ( TSCFEC_FECPARAMETERNOTMANAGED,
			       "The delay fine (orginal value) cannot be used (index out of the map [0,0x1B]): " + toString((int)k),
			       ERRORCODE ) ;
  }
  
  tscType8 delay=dly[k];

  return(delay) ;
}

/** Routine convering the coarse delay step for input to CTR3 when CTR1<5>=1 
 * \param k - coarse delay step
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning Original code was: unsigned short tsc_pll_coarse_delay_map(tscType8 k)
 */
tscType8 pllAccess::getTriggerDelayMap ( tscType8 k ) throw (FecExceptionHandler) {
	
  tscType8 dly[]={0x00,0x01,0x02,0x03,0x04,0x05,
		  0x06,0x07,0x08,0x09,0x0a,0x0b,            
		  0x0c,0x0d,0x0e,0x0f};

  if ( k > 15 ) {
    RAISEFECEXCEPTIONHANDLER ( TSCFEC_FECPARAMETERNOTMANAGED,
			       "The trigger coarse (orginal value) cannot be used (index out of the map [0,0xF]): " + toString((int)k),
			       ERRORCODE ) ;
  }

  
  tscType8 delay=dly[k];

  return(delay) ;
}


/** Get the value of the CTR1 register (status info) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 pllAccess::getCNTRL1 ( ) throw (FecExceptionHandler) {

  // Read CTR1
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_1 ) ;
  return i2cdata;
}

/** Get the value of the CTR2 register
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 pllAccess::getCNTRL2 ( ) throw (FecExceptionHandler) {

  // Read CTR2
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_2 ) ;
  return i2cdata;
}

/** Get the value of the CTR3 register
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
tscType8 pllAccess::getCNTRL3 ( ) throw (FecExceptionHandler) {

  // Read CTR3
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_3 ) ;
  return i2cdata;
}

/** Get the value of the CTR4 register
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 * \warning the register should be set before getting this register (PLL_CTRL2_RSEL should be set in the CTRL2)
 */
tscType8 pllAccess::getCNTRL4 ( ) throw (FecExceptionHandler) {

  // Force the RSEL in CTRL2 to 0
  tscType8 ctrl2 = getCNTRL2() ;
  setCNTRL2(ctrl2 & ~PLL_CTRL2_RSEL) ;

  // Read CTR4
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_4 ) ;

  // Put back the original value
  setCNTRL2(ctrl2) ;

  return i2cdata;
}

/** Get the value of the CTR5 register
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 * \warning the register should be set before getting this register (PLL_CTRL2_RSEL should be set in the CTRL2)
 */
tscType8 pllAccess::getCNTRL5 ( ) throw (FecExceptionHandler) {

  // Force the RSEL in CTRL2 to 0
  tscType8 ctrl2 = getCNTRL2() ;
  setCNTRL2(ctrl2 | PLL_CTRL2_RSEL) ;

  // Read CTR4
  volatile tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_4 ) ;

  // Put back the original value
  setCNTRL2(ctrl2) ;

  return i2cdata;
}

/** Get the value of the CTR1 register (status info) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void pllAccess::setCNTRL1 ( tscType8 value ) throw (FecExceptionHandler) {

  // Write CTR1
  accessToFec_->writeOffset ( accessKey_, CNTRL_1, value ) ;
}

/** Get the value of the CTR2 register (status info) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void pllAccess::setCNTRL2 ( tscType8 value ) throw (FecExceptionHandler) {

  // Write CTR2
  accessToFec_->writeOffset ( accessKey_, CNTRL_2, value ) ;
}

/** Get the value of the CTR3 register (status info) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
void pllAccess::setCNTRL3 ( tscType8 value ) throw (FecExceptionHandler) {

  // Write CTR3
  accessToFec_->writeOffset ( accessKey_, CNTRL_3, value ) ;
}

/** Set the value of the CTR4 register (status info) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 * \warning the register should be set before getting this register (PLL_CTRL2_RSEL should be set in the CTRL2)
 * \warning this method change the value of the CTRL2 but put the original back
 */
void pllAccess::setCNTRL4 ( tscType8 value ) throw (FecExceptionHandler) {

  // Force the RSEL in CTRL2
  tscType8 ctrl2 = getCNTRL2() ;
  setCNTRL2(ctrl2 & ~PLL_CTRL2_RSEL) ;

  // Write CTR4
  accessToFec_->writeOffset ( accessKey_, CNTRL_4, value ) ;

  // Put back the values
  setCNTRL2(ctrl2) ;
}

/** Set the value of the CTR5 register (status info) 
 * \return the value read
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 * \warning the register should be set before getting this register (PLL_CTRL2_RSEL should be set in the CTRL2)
 * \warning this method change the value of the CTRL2 but put the original back
 */
void pllAccess::setCNTRL5 ( tscType8 value ) throw (FecExceptionHandler) {

  // Force the RSEL in CTRL2
  tscType8 ctrl2 = getCNTRL2() ;
  setCNTRL2(ctrl2 | PLL_CTRL2_RSEL) ;

  // Write CTR4
  accessToFec_->writeOffset ( accessKey_, CNTRL_4, value ) ;

  // Put back the values
  setCNTRL2(ctrl2) ;
}


/** Get the value of the CTR1 register and checks if bitGOING_BIT   0 is set 
 * \return true if bit  GOING_BIT is set else false 
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
bool  pllAccess::getGoingStatus ( ) throw (FecExceptionHandler) {
  tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_1 ) ;
  return ( (i2cdata & GOING_BIT)  != 0);
}

/** Get the value of the CTR1 register and checks if SEU_BIT (3)  is set 
 * \return true if bit SEU_BIT is set else false 
 * \exception FecExceptionHandler
 * Thanks to Wim Beaumont
 */
bool  pllAccess::getSeuStatus ( ) throw (FecExceptionHandler) {
  tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_1 ) ;
  return ( (i2cdata & SEU_BIT)  != 0);
}

/** Take a vector of PLL and check if the control register 1 GOING bit is ok or not. If it is not set then reset the PLL with cold reset if it is specified.
 * \param fecAccess - hardware access
 * \param pllSet - PLL access to be set
 * \param errorGoingBit - check if a PLL was not correctly reseted
 * \param pllErrorBefore - it is the PLL that are not in a correct state before the reset or have an error during the access
 * \param pllErrorAfter - it is the PLL that are not in a correct state after the reset
 * \param noCheck - if noCheck is false then the reset is apply even if the GOING is there by default true so a reset is issued
 * \param coldReset - issue a cold reset if it is true else issue a PLL reset (default false)
 * \return number of errors
 * \see pllReset and pllInit
 */
unsigned int pllAccess::setPllCheckGoingBitMultipleFrames ( FecAccess &fecAccess, std::list<FecExceptionHandler *> &errorList,
							    Sgi::hash_map<keyType, pllAccess *> &pllSet, bool *errorGoingBit, 
							    std::list<keyType> &pllErrorBefore, std::list<keyType> &pllErrorAfter, 
							    bool noCheck, bool coldReset ) throw (FecExceptionHandler) {

  // no error for the time being
  *errorGoingBit = false ;

  // Number of errors   
  unsigned int error = 0 ; 

  // list of the PLL to be reseted
  std::list<keyType> pllToBeReseted ;
  std::map<keyType, tscType8> pllDacValue ;

  if (!noCheck) {

    // ----------------------------------------------------------------------------------------------
    // Read the control register 1 of all PLLsInit of the PLL
    accessDeviceTypeListMap vAccessesCtrl1 ;
    for ( Sgi::hash_map<keyType, pllAccess *>::iterator itPll = pllSet.begin() ; itPll != pllSet.end() ; itPll ++ ) {
      if (((pllDescription *)(itPll->second->getDownloadedValues()))->getInit()) {
	accessDeviceType readCtrl1 = { itPll->second->getKey(), NORMALMODE, MODE_READ, CNTRL_1, 0, false, 0, 0, 0, NULL} ;
	vAccessesCtrl1[getFecRingKey(itPll->second->getKey())].push_back (readCtrl1) ;
	pllDacValue[itPll->second->getKey()] = itPll->second->getPllDac() ;
      }
    }

    // Make the download and retreive the errors in a list
    error += fecAccess.setBlockDevices( vAccessesCtrl1, errorList ) ;

    // Check the answer
    for (accessDeviceTypeListMap::iterator itList = vAccessesCtrl1.begin() ; itList != vAccessesCtrl1.end() ; itList ++) {
      // for each list
      for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {
	
	if (itDevice->e == NULL) { // No error so check the going bit
	  if (!(itDevice->data & PLL_CTRL1_GOING)) { // so to be reseted
#ifdef DEBUGMSGERROR
	    char msg[80] ;
	    decodeKey(msg,itDevice->index) ;
	    std::cout << "CTRL1 = 0x" << std::hex << itDevice->data << " => reset the PLL " << msg << std::endl ;
#endif
	    pllToBeReseted.push_back (itDevice->index) ;
	    pllErrorBefore.push_back (itDevice->index) ;
	  }
#ifdef DEBUGMSGERROR
	  else {
	    char msg[80] ;
	    decodeKey(msg,itDevice->index) ;
	    std::cout << "CTRL1 = 0x" << std::hex << itDevice->data << " => GOING bit ok for PLL " << msg << std::endl ;
	  }
#endif
	}
	else { // Error during the access on the PLL => nothing to be done
#ifdef DEBUGMSGERROR
	  char msg[80] ;
	  decodeKey(msg,itDevice->index) ;
	  std::cout << "Hardware access error on CTRL1 " << msg << std::endl ;
#endif
	  //pllToBeReseted.push_back (itDevice->index) ;
	  //pllErrorBefore.push_back (itDevice->index) ;
	}
      }
    }
  }
  // ------------------------------------------------ reset all PLLs
  else {
    for ( Sgi::hash_map<keyType, pllAccess *>::iterator itPll = pllSet.begin() ; itPll != pllSet.end() ; itPll ++ ) {
      pllToBeReseted.push_back (itPll->second->getKey()) ;
    }
  }

  // -------------------------------------------------- Reset the corresponding PLL
  if (coldReset && pllToBeReseted.size()) {

    //#ifdef DEBUGMSGERROR
    if (pllToBeReseted.size()) { 
      std::cout << "Applying PLL cold init"
#ifdef PLLCOLDINITVERSION3
		<< " V3 (multiframes) on "
#endif
#ifdef PLLCOLDINITVERSION6
		<< " V6 (multiframes) on "
#endif
		<< pllToBeReseted.size() << " PLLs over " << pllSet.size() << " PLL in the system" << std::endl ;
    }
    //#endif


#ifdef PLLCOLDINITVERSION3
    accessDeviceTypeListMap vAccessesReset1 ;
    for (std::list<keyType>::iterator itPll = pllToBeReseted.begin() ; itPll != pllToBeReseted.end() ; itPll ++  ) {

      // 1 - write 0x80 to reg 1
      accessDeviceType op1 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_1, 0x80, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op1) ;
      // 2 - write 0xC0 to reg 2
      accessDeviceType op2 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_2, 0xC0, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op2) ;      
      // 3 - write 0x84 to reg 1
      accessDeviceType op3 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_1, 0x84, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op3) ;
      // 4 - write 0xFF to reg 3
      accessDeviceType op4 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_3, pllDacValue[*itPll], false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op4) ;
      // 5 - write 0x00 to reg 4
      accessDeviceType op5 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_4, 0x00, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op5) ;
      // 6 - write 0x80 to reg 2
      accessDeviceType op6 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_2, 0x80, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op6) ;
      // 7 - write 0x00 to reg 1
      accessDeviceType op7 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_1, 0x00, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op7) ;
    }

    // Make the download and retreive the errors in a list
    error += fecAccess.setBlockDevices( vAccessesReset1, errorList ) ;
#endif

#ifdef PLLCOLDINITVERSION6
    accessDeviceTypeListMap vAccessesReset1 ;
    accessDeviceTypeListMap vAccessesReset2 ;
    for (std::list<keyType>::iterator itPll = pllToBeReseted.begin() ; itPll != pllToBeReseted.end() ; itPll ++  ) {

      // 1 - write 0x80 to reg 1
      accessDeviceType op1 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_1, PLL_CTRL1_MODE, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op1) ;
      // 2 - write 0x00 to reg 2
      accessDeviceType op2 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_2, 0x00, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op2) ;      
      // 3 - write 0xFF to reg 3
      accessDeviceType op3 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_3, pllDacValue[*itPll], false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op3) ;      
      // 4 - write 0x00 to reg 4
      accessDeviceType op4 = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_4, 0x00, false, 0, 0, 0, NULL} ;
      vAccessesReset1[getFecRingKey(*itPll)].push_back (op4) ;

      // W) Here a sleep should be done

      // 5 - write 0x80 to reg 2
      accessDeviceType ctrl2Going = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_2, PLL_CTRL2_I2CGOING, false, 0, 0, 0, NULL} ;
      vAccessesReset2[getFecRingKey(*itPll)].push_back (ctrl2Going) ;

      // Here a sleep should be done
    }

    // Make the download and retreive the errors in a list
    error += fecAccess.setBlockDevices( vAccessesReset1, errorList ) ;

    // wait for 5000 us
    usleep (PLLDELAY) ;

    // Make the download and retreive the errors in a list
    error += fecAccess.setBlockDevices( vAccessesReset2, errorList ) ;
#endif
  }
  else if (pllToBeReseted.size()) {

    accessDeviceTypeListMap vAccessesReset ;

    for (std::list<keyType>::iterator itPll = pllToBeReseted.begin() ; itPll != pllToBeReseted.end() ; itPll ++  ) {
      // Write CTR1<3>=1: accessToFec_->writeOffset ( accessKey_, CNTRL_1, PLL_RESET_ON ) ;
      accessDeviceType pllResetOn = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_1, PLL_RESET_ON, false, 0, 0, 0, NULL} ;
      vAccessesReset[getFecRingKey(*itPll)].push_back (pllResetOn) ;
      // Write CTR1<3>=0: accessToFec_->writeOffset ( accessKey_, CNTRL_1, PLL_RESET_OFF ) ;
      accessDeviceType pllResetOff = { *itPll, NORMALMODE, MODE_WRITE, CNTRL_1, PLL_RESET_OFF, false, 0, 0, 0, NULL} ;
      vAccessesReset[getFecRingKey(*itPll)].push_back (pllResetOff) ;
    }

    // Make the download and retreive the errors in a list
    error += fecAccess.setBlockDevices( vAccessesReset, errorList ) ;
  }

  // wait for 5000 us
  usleep (PLLDELAY) ;

  // ------------------------------------------------- Check the GOING bit inside the PLL
  accessDeviceTypeListMap vAccessesGoingBit ;
  for (std::list<keyType>::iterator itPll = pllToBeReseted.begin() ; itPll != pllToBeReseted.end() ; itPll ++  ) {

    // Read CTR1: tscType8 i2cdata = accessToFec_->readOffset ( accessKey_, CNTRL_1 ) ;
    accessDeviceType pllReadCtrl1 = { *itPll, NORMALMODE, MODE_READ, CNTRL_1, 0, false, 0, 0, 0, NULL} ;
    vAccessesGoingBit[getFecRingKey(*itPll)].push_back (pllReadCtrl1) ;
  }

  bool finished = false ;
  unsigned long timeout = 0 ;
  do {

    // Make the download and retreive the errors in a list
    error += fecAccess.setBlockDevices( vAccessesGoingBit, errorList ) ;

    //int nanosleep(const struct timespec *req, struct timespec *rem);
    nanosleep ( &PLLGOINGBITDELAY, NULL ) ; // 5 ms

    finished = true ;

    // Check the answer
    for (accessDeviceTypeListMap::iterator itList = vAccessesGoingBit.begin() ; itList != vAccessesGoingBit.end() ; itList ++) {
      // for each list
      for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

#ifdef DEBUGMSGERROR
	char msg[80] ;
	decodeKey(msg,itDevice->index) ;
	if (itDevice->e == NULL) {
	  std::cout << "CTRL1 = 0x" << std::hex << itDevice->data << " => reset the PLL " << msg << std::endl ;
	}
	else std::cout << msg << ": error in reading the CTRL1" << std::endl ;
#endif
	
	if ( ((itDevice->e == NULL) && (!(itDevice->data & PLL_CTRL1_GOING))) || (itDevice->e != NULL) ) {
	  
	  finished = false ; // not finished
	  // resend it
	  itDevice->sent = false ;
	  itDevice->e    = NULL  ;
	  itDevice->tnum = 0     ;
	  itDevice->dAck = 0     ;
	  itDevice->fAck = 0     ;
	}
      }
    } 
  } while ( (! finished) && (timeout++ < PLLTIMEOUT) ) ;

  if ((! finished) || (timeout >= PLLTIMEOUT)) {
    *errorGoingBit = true ;
    // Check the answer to store which PLL had a problem in the going bit (reset does not help)
    for (accessDeviceTypeListMap::iterator itList = vAccessesGoingBit.begin() ; itList != vAccessesGoingBit.end() ; itList ++) {
      for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++)
	if ( ((itDevice->e == NULL) && (!(itDevice->data & PLL_CTRL1_GOING))) || (itDevice->e != NULL) ) pllErrorAfter.push_back(itDevice->index) ;
    }
  }

#ifdef DEBUGMSGERROR
  displayPllRegisters ( fecAccess, pllSet ) ;
#endif

  return (error) ;
}

/** Display the different values of the registers for a given set of PLL
 * \param fecAccess - hardware access
 * \param pllSet - set of PLL
 */
void pllAccess::displayPllRegisters ( FecAccess &fecAccess, Sgi::hash_map<keyType, pllAccess *> &pllSet ) {

  for ( Sgi::hash_map<keyType, pllAccess *>::iterator itPll = pllSet.begin() ; itPll != pllSet.end() ; itPll ++ ) {

    try {
      int ctrl1 = itPll->second->getCNTRL1() ;
      int ctrl2 = itPll->second->getCNTRL2() ;
      int ctrl3 = itPll->second->getCNTRL3() ;
      int ctrl4 = itPll->second->getCNTRL4() ;
      int ctrl5 = itPll->second->getCNTRL5() ;
    
      std::cout << "FEC=0-Slot=" << std::dec << getFecKey(itPll->first) 
		<< "-Ring=" << getRingKey(itPll->first) 
		<< "-Ccu=" << getCcuKey(itPll->first) 
		<< "-Channel=" << getChannelKey(itPll->first) 
		<< ": " << std::hex << "\t0x" << ctrl1 << "\t0x" << ctrl2 << "\t0x" << ctrl3 << "\t0x" << ctrl4 << "\t0x" << ctrl5 << std::dec << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      std::cout << "FEC=0-Slot=" << std::dec << getFecKey(itPll->first) 
		<< "-Ring=" << getRingKey(itPll->first) 
		<< "-Ccu=" << getCcuKey(itPll->first) 
		<< "-Channel=" << getChannelKey(itPll->first) 
		<< ": unable to read one of the PLL register: " << e.what() << std::endl ;
    }
  }
  //#endif
}

/** Display the different values of the registers for a given set of PLL
 * \param fecAccess - hardware access
 * \param pllSet - set of PLL
 */
void pllAccess::displayPllRegisters ( ) {

    int ctrl1 = this->getCNTRL1() ;
    int ctrl2 = this->getCNTRL2() ;
    int ctrl3 = this->getCNTRL3() ;
    int ctrl4 = this->getCNTRL4() ;
    int ctrl5 = this->getCNTRL5() ;
    
    std::cout << "FEC=0-Slot=" << std::dec << getFecKey(getKey()) 
  	      << "-Ring=" << getRingKey(getKey()) 
	      << "-Ccu=" << getCcuKey(getKey()) 
	      << "-Channel=" << getChannelKey(getKey()) 
	      << ": " << std::hex << "\t0x" << ctrl1 << "\t0x" << ctrl2 << "\t0x" << ctrl3 << "\t0x" << ctrl4 << "\t0x" << ctrl5 << std::dec << std::endl ;
}

/** Take a description value of a PLL a build a block of frames to be set in the hardware
 * \param pllValues - all the values for an laserdriver
 * \param vAccess - block of frames
 */
void pllAccess::getBlockWriteValues ( pllDescription pllValues, accessDeviceTypeList &vAccess ) throw (FecExceptionHandler) {

  tscType8 delay_fine_val   = getClockPhaseMap   ( pllValues.getClockPhase() ) ;
  tscType8 delay_coarse_val = getTriggerDelayMap ( pllValues.getTriggerDelay() ) ;

  // ----------------------------------------------------- Clock phase
  // Choose manual delay and value.
  tscType8 ctrl2Val = PLL_CTRL2_I2CGOING | delay_fine_val ;

  // Done in the next step
  // ..and write: accessToFec_->writeOffset ( accessKey_, CNTRL_2, i2cdata ) ;
  // accessDeviceType ctrl2 = { getKey(), NORMALMODE, MODE_WRITE, CNTRL_2, ctrl2Val, false, 0, 0, 0, NULL} ;
  // vAccess.push_back (ctrl2) ;

  // ----------------------------------------------------- Trigger delay
  // tscType8 ctrl2= ctrl2Orig | PLL_CTRL2_RSEL ; and set it
  // accessToFec_->writeOffset ( accessKey_, CNTRL_2, ctrl2 ) ;
  // Is this part can be done at the same time than the previous command
  accessDeviceType ctrl2Rsel = { getKey(), NORMALMODE, MODE_WRITE, CNTRL_2, (tscType16)(ctrl2Val |  PLL_CTRL2_RSEL), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ctrl2Rsel) ;

  // ...and write the CTRL4: accessToFec_->writeOffset ( accessKey_, CNTRL_4, delay_val ) ;
  accessDeviceType ctrl4 = { getKey(), NORMALMODE, MODE_WRITE, CNTRL_4, delay_coarse_val, false, 0, 0, 0, NULL} ;
  vAccess.push_back (ctrl4) ;

}

/** This static method read out several PLL at the same time
 * \param fecAccess - hardware access
 * \param pllSet - all the PLL to be readout
 * \param pllVector - list of the readout PLL (suppose to be empty at the beginning). The pllDescription created must be delete by the remote method.
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int pllAccess::getPllValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, pllAccess *> &pllSet, deviceVector &pllVector,
						      std::list<FecExceptionHandler *> &errorList ) 
  throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, pllDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesClockPhase ;
  accessDeviceTypeListMap vAccessesTriggerDelay ; 

  for ( Sgi::hash_map<keyType, pllAccess *>::iterator itPll = pllSet.begin() ; itPll != pllSet.end() ; itPll ++ ) {

    // Read all CTRL2
    accessDeviceType ctrl2 = { itPll->second->getKey(), NORMALMODE, MODE_READ, CNTRL_2, 0, false, 0, 0, 0, NULL} ;
    vAccessesClockPhase[getFecRingKey(itPll->second->getKey())].push_back(ctrl2) ;
    
    // Put it in a map and in the vector
    deviceDescriptionsMap[itPll->second->getKey()] = new pllDescription ( itPll->second->getKey() ) ;
    pllVector.push_back(deviceDescriptionsMap[itPll->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itPll->second->getKey()]->setFecHardwareId ( itPll->second->getFecHardwareId(), itPll->second->getCrateId() ) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesClockPhase, errorList ) ;

  // Collect the answer and fill the corresponding pllDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesClockPhase.begin() ; itList != vAccessesClockPhase.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      try {
	// set the clock phae of the pll
	deviceDescriptionsMap[itDevice->index]->setClockPhase ( getClockPhaseMapBack(data & PLL_CLOCKPHASE_READ) ) ;
      }
      catch (FecExceptionHandler &e) {
	if (itDevice->e == NULL) itDevice->e = new FecExceptionHandler(e) ;
      }

      // build the next frames to be sent only for correct clock phase
      if (itDevice->e == NULL) {
	  
	// In a second step, build the part for the selection of the register
	accessDeviceType ctrl2Rsel = { itDevice->index, NORMALMODE, MODE_WRITE, CNTRL_2, (tscType16)(data | PLL_CTRL2_RSEL), false, 0, 0, 0, NULL} ;
	vAccessesTriggerDelay[getFecRingKey(itDevice->index)].push_back (ctrl2Rsel) ;
	
	// build the part for the read of the control register 4
	accessDeviceType ctrl4 = { itDevice->index, NORMALMODE, MODE_READ, CNTRL_4, 0, false, 0, 0, 0, NULL} ;
	vAccessesTriggerDelay[getFecRingKey(itDevice->index)].push_back (ctrl4) ;
      }
    }
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesTriggerDelay, errorList ) ;

  // Collect the answer and fill the corresponding pllDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesTriggerDelay.begin() ; itList != vAccessesTriggerDelay.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      try {
	// set the clock phase of the pll
	deviceDescriptionsMap[itDevice->index]->setTriggerDelay ( getTriggerDelayMapBack(data & PLL_TRIGGERDELAY_READ) ) ;
      }
      catch (FecExceptionHandler &e) {
	if (itDevice->e == NULL) itDevice->e = new FecExceptionHandler(e) ;
      }
    }
  }

  return (error) ;
}

/** Take a description value of a PLL a build a block of frames to be set in the hardware
 * \param delay - delay to be added into the hardware
 * \param vAccess - block of frames
 */
void pllAccess::getBlockWriteValues ( tscType8 delay, accessDeviceTypeList &vAccess ) {

  tscType8 value = getTimeValue () ;
  value += delay ;

  tscType8 delayFine = value % 24 ;
  tscType8 delayCoarse = (int)(value / 24) ;

  pllDescription pllD ( delayFine, delayCoarse ) ;

  getBlockWriteValues ( pllD, vAccess ) ;
}
