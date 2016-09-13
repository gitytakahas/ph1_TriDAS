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
#ifndef DELAY25ACCESS_H
#define DELAY25ACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "deviceAccess.h"
#include "delay25Description.h"
#include "deviceType.h"

#include "FecAccess.h"


class delay25Access: public deviceAccess {

 public:

  /** \brief Call the deviceAccess constructor, in order to build 
   * an access to an I2C channel
   */
  delay25Access (FecAccess *fec, 
                 tscType16 fecSlot,
                 tscType16 ringSlot,
                 tscType16 ccuAddress,
                 tscType16 i2cChannel,
                 tscType16 i2cAddress ) ; 

  /** \brief Call the deviceAccess constructor, in order to build 
   * an access to an I2C channel
   */
  delay25Access (FecAccess *fec, 
                 keyType key) ;

  /** \brief Remove the access
   */
  ~delay25Access ( ) ;

  /** \brief set a description for future comparison
   */
  void setDescription ( delay25Description delay25Values ) ;

  /** \brief Set all values from a DELAY25
   */
  void setValues ( delay25Description delay25Values ) ;

  /** \brief Get all values from a DELAY25
   */
  delay25Description *getValues ( ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setDelay0 ( tscType8 delay_step ) throw (FecExceptionHandler) ;

  /** \brief Set the value in the specified register
   */
  void setDelay1 ( tscType8 delay_step ) throw (FecExceptionHandler) ; 

  /** \brief Set the value in the specified register
   */
  void setDelay2 ( tscType8 delay_step ) throw (FecExceptionHandler) ; 

  /** \brief Set the value in the specified register
   */
  void setDelay3 ( tscType8 delay_step ) throw (FecExceptionHandler) ; 

  /** \brief Set the value in the specified register
   */
  void setDelay4 ( tscType8 delay_step ) throw (FecExceptionHandler) ;
 
  /** \brief Set the value in the specified register
   */
  void set40HzFrequencyMode ( ) throw (FecExceptionHandler) ;

  /** \brief Get the delay
   */
  tscType8 getDelay0 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the delay
   */
  tscType8 getDelay1 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the delay
   */
  tscType8 getDelay2 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the delay
   */
  tscType8 getDelay3 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the delay
   */
  tscType8 getDelay4 ( ) throw (FecExceptionHandler) ;

  /** \brief Get the frequency mode (has to be 40Hz)
   */
  tscType8 getFrequencyMode ( ) throw (FecExceptionHandler) ;

  /** \brief return the contents of the CR0 register
   */
  tscType8 getCR0 ( ) throw (FecExceptionHandler) ; 

  /** \brief return the contents of the CR1 register
   */
  tscType8 getCR1 ( ) throw (FecExceptionHandler) ; 

  /** \brief return the contents of the CR2 register
   */
  tscType8 getCR2 ( ) throw (FecExceptionHandler) ;

  /** \brief return the contents of the CR3 register
   */
  tscType8 getCR3 ( ) throw (FecExceptionHandler) ; 

  /** \brief return the contents of the CR4 register
   */
  tscType8 getCR4 ( ) throw (FecExceptionHandler) ; 

  /** \brief return the contents of the GCR register
   */
  tscType8 getGCR ( ) throw (FecExceptionHandler) ;

  /** \brief set the register 0 of the DELAY25
   */
  void setCR0 ( tscType8 value ) throw (FecExceptionHandler) ; 

  /** \brief set the register 1 of the DELAY25
   */
  void setCR1 ( tscType8 value ) throw (FecExceptionHandler) ; 

  /** \brief set the register 2 of the DELAY25
   */
  void setCR2 ( tscType8 value ) throw (FecExceptionHandler) ; 

  /** \brief set the register 3 of the DELAY25
   */
  void setCR3 ( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief set the register 4 of the DELAY25
   */
  void setCR4 ( tscType8 value ) throw (FecExceptionHandler) ; 

  /** \brief set the register 5 of the DELAY25
   */
  void setGCR ( tscType8 value ) throw (FecExceptionHandler) ;

  /** Array to make a correspondance between the frequency and the mode to set
   */
  tscType8 getFrequencyModeMap ( tscType8 k ) throw (FecExceptionHandler) ;

  /** Array to make a correspondance between the frequency and the mode to set
   */
  tscType8 getFrequencyModeMapBack ( tscType8 k ) ;

  /** \brief Display the register for a given set of DELAY25
   */
  void displayDelay25Registers ( FecAccess &fecAccess, Sgi::hash_map<keyType, delay25Access *> &delay25Set ) ;
 
  /** \brief Display the registers of the DELAY25
   */
  void displayDelay25Registers ( ) ;

  /** \brief Create a block of frames to be downloaded into the hardware
   */
  void getBlockWriteValues ( delay25Description delay25Values, accessDeviceTypeList &vAccess ) ;

  /** \brief static method to upload from the hardware the devices
   */
  unsigned int getDelay25ValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, delay25Access *> &delay25Set, deviceVector &delay25Vector, std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) ;

  /** \brief method for DLL resynchronisation
   */
  void forceDLLResynchronisation ( ) throw (FecExceptionHandler) ; 

  /** \brief method to reset IDLL bit
   */
  void SetIDLLBitToZero ( ) throw (FecExceptionHandler) ;

};

#endif
