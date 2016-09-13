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
#ifndef PLLACCESS_H
#define PLLACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "deviceAccess.h"
#include "pllDescription.h"
#include "deviceType.h"

#include "FecAccess.h"

/** Version 3 of the COLD PLL init (see pllInit method)
 */
//#define PLLCOLDINITVERSION3
/** Version 6 of the COLD PLL init (see pllInit method)
 */
#define PLLCOLDINITVERSION6
/** Value set in the CTRL3 register during the cold PLL init
 */
#define PLLDAC 0xFF

/**
 * \class pllAccess
 * This class defined a pll and make possible the downloading or
 * uploading of the values related to a specific register.
 * It also implements all the method specific to the laserdevice device.
 * \author Frederic Drouhin, Nancy Marinelli, Peter Walsham 
 * \date April 2007
 * \warning all the I2C accesses are done in normal mode with an offset on the address
 * \warning
 * \brief This class define all the hardware access for the pll througth the FecAccess class
 * PLL reset:
 * <ul>
 * <li>CTRL1 = RESET_ON  (0x08)
 * <li>CTRL1 = RESET_OFF (0x00)
 * <li>Wait and timeout for PLL_CTRL1_GOING (CTRL1<0>)
 * <li>return the CTRL1 value
 * </ul>
 * PLL init 
 * <ul>
 * <li>CTRL1 = RESET_ON  (0x08)
 * <li>CTRL1 = RESET_OFF (0x00)
 * <li>CTRL1 = PLL_CTRL1_MODE (0x80)
 * <li>CTRL2 = PLL_CTRL2_I2CGOING (0x80)
 * <li>CTRL1 = PLL_CTRL1_MODE | PLL_CTRL1_FORCEPFD (0x80 OR 0x04 = 0x84)
 * <li>CTRL3 = 0xFF
 * <li>CTRL4 = 0x00
 * <li>CTRL2 = PLL_CTRL2_I2CGOING (0x80)
 * <li>CTRL1 = PLL_CTRL1_MODE (0x80)
 * <li>wait for 200 ms
 * <li>Wait and timeout for PLL_CTRL1_GOING (CTRL1<0>)
 * <li>return the CTRL1 value
 * </ul>
 * <BR>
 * set Clock Phase ( delay_step ): old delay fine method
 * <ul>
 *  <li>Convert delay_step into delay_val through the array:
 *         {0x00,0x01,0x02,0x03,0x04,0x05,
 *          0x06,0x07,0x08,0x09,0x0a,0x0b,            
 *           0x10,0x11,0x12,0x13,0x14,0x15,
 *           0x16,0x17,0x18,0x19,0x1a,0x1b};
 *  <li>CTRL2 = delay_val OR PLL_CTRL2_I2CGOING (delay_val OR 0x80)
 * </ul>
 * set Trigger delay ( delay_step ): old delay coarse method
 * <ul>
 * <li>Convert delay_step into delay_val through the array:
 *          {0x00,0x01,0x02,0x03,0x04,0x05,
 *           0x06,0x07,0x08,0x09,0x0a,0x0b,            
 *           0x0c,0x0d,0x0e,0x0f};
 * <li>CTRL2 = CTRL2 OR PLL_CTRL2_RSEL (RMW with 0x20)
 * <li>CTRL4 = delay_val
 * </ul>
 * <BR>
 * get Clock Phase: old delay fine method
 * <ul>
 * <li>delay_val = CTRL2 AND PLL_CLOCKPHASEREAD ( AND 0x1F)
 * <li>convert delay_val into delay_step through the array:
 * <verbatim>
 *           {0, 1, 2, 3, 4,  5,
 *           6, 7, 8, 9, 10, 11,            
 *           99, 99, 99, 99, 12, 13,
 *           14, 15, 16, 17, 18, 19, 
 *           20, 21, 22, 23};
 * </verbatim>
 * <li>return delay_step
 * </ul>
 * <BR>
 * get Trigger Delay: old delay coarse method
 * <ul>
 * <li>CTRL2 = CTRL2 OR PLL_CTRL2_RSEL (RMW 0x20)
 * <li>delay_val = CTRL4 AND PLL_TRIGGERDELAY_READ ( AND 0x0F)
 * <li>convert delay_val into delay_step through the array:
 * <verbatim>
 *           {0, 1, 2, 3, 
 *            4, 5, 6, 7,         
 *            8, 9, 10, 11, 
 *            12, 13, 14, 15};
 * </verbatim>
 * </ul>
 * Mode of operation:
 * <ul>
 * <li> create an access to the PLL: nothing is done on the PLL
 * <li> set the values of the PLL:
 * <li> reset PLL if the PLL_CTRL1_GOING is not on
 * <li> setClockPhase
 * <li> setTriggerDelay
 * <li> read back the values (optionnal)
 * <li> getClockPhase
 * <li> getTriggerDelay
 * </ul>
 * \warning
 * <ul>
 * <li>Please avoid using any PLL reset/cold reset on the PLL except if this one is failing. In warm temperature, following the design and explanation given by hardware people, the PLL should lock without any intervention on the hardware. In cold temperature, few PLL are not locking and then only on this PLL a cold init should be apply. 
 * <li>PLL reset is done on request or if the CTRL1_GOING (CTRL1<0>) is not correct. For that purpose, I add method called pllCheck that do the check and reset the PLL (so PLL reset method) if the bit is not there and wait for it.
 * <li>the pllAccess:setValues check also the CTRL1_GOING and reset it if needed
 * <li>The method for conversion between the step and the value are called now:
 * <ul>
 * <li> getTriggerDelayMap and getTriggerDelayMapBack
 * <li> getClockPhaseMap and getClockPhaseMapBack
 * </ul>
 * </ul>
 */
class pllAccess: public deviceAccess {

 private: 

  /** PLL DAQ register
   */
  tscType8 pllDac_ ;

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  pllAccess (FecAccess *fec, 
             tscType16 Fec_slot,
             tscType16 Ring_slot,
             tscType16 Ccu_slot,
             tscType16 channelId,
             tscType16 I2C_address ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  pllAccess (FecAccess *fec, 
             keyType key ) ;

  /** \brief Remove the access
   */
  ~pllAccess ( ) ;

  /** \brief set a description for future comparison
   */
  void setDescription ( pllDescription pllValues ) ;

  /** \brief return the time in ns of the PLL regarding the value downloaded
   */
  tscType8 getTimeValue ( ) ;

  /** \brief Set all values from a PLL
   */
  void setValues ( pllDescription pllValues ) ;

  /** \brief Create a block of frames to be downloaded into the hardware
   */
  void getBlockWriteValues ( pllDescription pllValues, std::list<accessDeviceType> &vAccess ) throw (FecExceptionHandler) ;

  /** \brief Set a block of frames to be download into the hardware from a delay to be added regarding the previous values
   */
  void getBlockWriteValues ( tscType8 delay, std::list<accessDeviceType> &vAccess ) ;

  /** \brief Get all values from a PLL
   */
  pllDescription *getValues ( ) throw ( FecExceptionHandler ) ;

  /** \brief set the PLL DAQ
   */
  void setPllDac ( tscType8 pllDac ) ;

  /** \brief get the PLL DAQ
   */
  tscType8 getPllDac ( ) ;

  /** \brief Set the value in the specified register
   * \deprecated use now the method setClockPhase()
   */
  void setDelayFine ( tscType8 delay_step ) throw ( FecExceptionHandler ) { setClockPhase(delay_step) ; }

  /** \brief Set the clock phase
   */
  void setClockPhase ( tscType8 delay_step ) throw ( FecExceptionHandler ) ;

  /** \brief Set the trigger delay
   * \deprecated use now the method setTriggerDelay()
   */
  void setDelayCoarse ( tscType8 delay_step ) throw ( FecExceptionHandler ) { setTriggerDelay(delay_step) ; }

  /** \brief Set the trigger delay
   */
  void setTriggerDelay ( tscType8 delay_step ) throw ( FecExceptionHandler ) ;

  /** \brief Get the clock phase
   * \deprecated use now getClockPhase () 
   */
  tscType8 getDelayFine ( ) throw (FecExceptionHandler) { return (getClockPhase()) ; }

  /** \brief Get the clock phase
   */
  tscType8 getClockPhase ( ) throw (FecExceptionHandler) ;

  /** \brief Get the trigger delay
   * \deprecated use now getTriggerDelay () 
   */
  tscType8 getDelayCoarse ( ) throw (FecExceptionHandler) { return (getTriggerDelay()) ; }

  /** \brief Get the trigger delay  
   */
  tscType8 getTriggerDelay ( ) throw (FecExceptionHandler) ;

  /** \brief check the going bit and make a PLL reset or a PLL init
   */
  void setPllCheckGoingBit ( bool noCheck = false, bool coldReset = true ) throw (FecExceptionHandler) ;

  /** \brief Specific function on the PLL: reset or initialise the PLL
   */
  tscType8 pllReset ( ) throw (FecExceptionHandler) ; // Init ...

  /** \brief Specific function on the PLL for cold reset
   */
  tscType8 pllInit  ( ) throw (FecExceptionHandler) ; // Init ...

  /** \brief return the contents of the CTR1 register 
   */
  tscType8 getCNTRL1()throw (FecExceptionHandler);

  /** \brief return the contents of the CTR2 register 
   */
  tscType8 getCNTRL2()throw (FecExceptionHandler);

  /** \brief return the contents of the CTR3 register 
   */
  tscType8 getCNTRL3()throw (FecExceptionHandler);

  /** \brief return the contents of the CTR4 register 
   */
  tscType8 getCNTRL4()throw (FecExceptionHandler);

  /** \brief return the contents of the CTR4 register 
   */
  tscType8 getCNTRL5()throw (FecExceptionHandler);

  /** \brief set the register 1 of the PLL
   */
  void setCNTRL1( tscType8 value )throw (FecExceptionHandler);

  /** \brief set the register 2 of the PLL
   */
  void setCNTRL2( tscType8 value )throw (FecExceptionHandler);

  /** \brief set the register 3 of the PLL
   */
  void setCNTRL3( tscType8 value )throw (FecExceptionHandler);

  /** \brief set the register 4 of the PLL
   */
  void setCNTRL4( tscType8 value )throw (FecExceptionHandler);

  /** \brief set the register 5 of the PLL
   */
  void setCNTRL5( tscType8 value )throw (FecExceptionHandler);

  /** \brief returns true if the PLL is running ( bit 0 of CNTRL1 =1)
   */
  bool getGoingStatus()throw (FecExceptionHandler);

  /** \brief returns true if as SEU is detected ( bit 3 of CTNRL1=1)
   */
  bool getSeuStatus()throw (FecExceptionHandler);

  /** Array to make a correspondance between the delay and the step to set
   */
  static tscType8 getClockPhaseMap ( tscType8 k ) throw (FecExceptionHandler) ;

  /** Array to make a correspondance between the delay and the step to set
   */
  static tscType8 getClockPhaseMapBack ( tscType8 k ) ;

  /** Array to make a correspondance between the delay and the step to set
   */
  static tscType8 getTriggerDelayMap ( tscType8 k ) throw (FecExceptionHandler) ;

  /** Array to make a correspondance between the delay and the step to set
   */
  static tscType8 getTriggerDelayMapBack ( tscType8 k ) ;

  /** \brief reset all PLL in parallel
   */
  static unsigned int setPllCheckGoingBitMultipleFrames ( FecAccess &fecAccess, std::list<FecExceptionHandler *> &errorList,
							  Sgi::hash_map<keyType, pllAccess *> &pllSet, bool *errorGoingBit, 
							  std::list<keyType> &pllErrorBefore, std::list<keyType> &pllErrorAfter, 
							  bool noCheck = false, bool coldReset = true ) throw (FecExceptionHandler) ;

  /** \brief Display the register for a given set of PLL
   */
  static void displayPllRegisters ( FecAccess &fecAccess, Sgi::hash_map<keyType, pllAccess *> &pllSet ) ;

  /** \brief Display the registers of the PLL
   */
  void displayPllRegisters (  ) ;

  /** \brief static method to upload from the hardware the devices
   */
  static unsigned int getPllValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, pllAccess *> &pllSet, deviceVector &pllVector,
						   std::list<FecExceptionHandler *> &errorList ) 
    throw (FecExceptionHandler) ;

  /** \brief return the version of the PLL cold init procedure
   */
  static std::string getPllColdInitVersion ( ) {
#ifdef PLLCOLDINITVERSION6
    return ("6") ;
#elif PLLCOLDINITVERSION3
    return ("3") ;
#else
#  WARNING "NO PLL COLD INIT PROCEDURE VERSION SET"
#endif
  }
};

#endif
