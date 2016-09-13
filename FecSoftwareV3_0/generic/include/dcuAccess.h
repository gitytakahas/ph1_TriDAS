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
#ifndef DCUACCESS_H
#define DCUACCESS_H

#include "keyType.h"

#include "FecExceptionHandler.h"

#include "dcuDescription.h"
#include "dcuDefinition.h"

#include "FecAccess.h"
#include "deviceAccess.h"

#include "deviceType.h"

/**
 * \class dcuAccess
 * This class defined an DCU and make possible the downloading
 * of the values related to the channels.
 * It also implements all the method related to reset of the DCU device.
 * \author Frederic Drouhin and Joao Fernandes
 * \date July 2002
 * \brief This class define all the hardware accesses for the DCU througth the FecAccess class
 * \warning If you have the error message:
 * <ul>
 * <li> DCU Fec 0x0 Ring 0x0 Ccu 0x7f I2c channel 0x1b I2c address 0x0
 * Warning channel 0:  error in digitisation, shreg = 0x38
 * </ul>
 * It means that the channel for this DCU can not be
 * connected. It is the case for the channel 0 in current modules in the Tracker. In this case
 * the value returned for this channel is 0.
 * \warning The hardware identifier in the DCU are not the same for DCUX and DCU 2.
 */
class dcuAccess: public deviceAccess {

 private:

  /** Is an error in digitisation occurs ?
   */
  int errorDigitisationCounter_        ;

  /** Channel in the DCU to be read
   */
  bool channelEnabled_[MAXDCUCHANNELS] ;

  /** DCU type
   */
  char dcuType_[4] ;

 public:

  /**\brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  dcuAccess (FecAccess *fec,
	     tscType16 fecSlot,
	     tscType16 ringSlot,
	     tscType16 ccuAddress,
	     tscType16 i2cChannel,
	     tscType16 i2cAddress,
	     std::string dcuType = DCUFEH ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  dcuAccess (FecAccess *fec, 
             keyType index,
	     std::string dcuType = DCUFEH ) ;

  /**\brief Remove the access
   */
  ~dcuAccess () ;

  /** \brief set the DCU type
   */
  void setDcuType ( std::string dcuType ) ;

  /** \brief get the DCU type
   */
  std::string getDcuType ( ) ;

  /** \brief set the channel to be read or not
   */
  void setChannelEnabled ( bool *channels, unsigned int nbChannel ) ;

  /** \brief return true if a channel is readout of not
   */
  bool getChannelEnabled ( tscType8 channel ) ;

  /**\brief Get all values from an DCU
   */
  dcuDescription *getValues ( ) throw (FecExceptionHandler) ;

  /**\brief get the value of DCU hardware address
   */
  tscType32 getDcuHardId ( ) throw (FecExceptionHandler) ;

  /**\brief get the value of DCU channels
   */
  tscType16 getDcuChannel ( tscType8 channel, bool setTreg=true ) throw (FecExceptionHandler) ;

  /** \brief Set DCU control register (CREG)
   */
  void setDcuCREG( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief get the DCU control register (CREG)
   */
  tscType8 getDcuCREG() throw (FecExceptionHandler) ;

  /** \brief Set DCU test register (TREG)
   */
  void setDcuTREG( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief get the DCU test register (TREG)
   */
  tscType8 getDcuTREG() throw (FecExceptionHandler) ;

  /**\brief set the DCU test register 
   */
  void setDcuTestRegister( tscType8 value)throw (FecExceptionHandler) ;
  
  /**\brief read  the DCU test register 
   */
  tscType8 getDcuTestRegister() throw (FecExceptionHandler) ;

  /** \brief Set DCU auxilairy register (AREG)
   */
  void setDcuAREG( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief get the DCU auxiliary register (AREG)
   */
  tscType8 getDcuAREG() throw (FecExceptionHandler) ;

  /** \brief get the DCU status and data high register (SHREG)
   */
  tscType8 getDcuSHREG() throw (FecExceptionHandler) ;

  /** \brief Set DCU data low register (LREG)
   */
  void setDcuLREG( tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief get the DCU data low register (LREG)
   */
  tscType8 getDcuLREG() throw (FecExceptionHandler) ;

  /** \brief returns true if as SEU is detected ( bit 6  of Satais & DataReg = 1 )
   */
  bool getSeuStatus() throw (FecExceptionHandler);
  
  /** DCU Reset function
   */
  void reset () ;

  /** \brief readout a set of DCU
   */
  static unsigned int getDcuValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, dcuAccess *> &dcuSet, deviceVector &dcuVector,
						   std::list<FecExceptionHandler *> &errorList, bool ducHardIdOnly = false ) 
    throw (FecExceptionHandler) ;

};

#endif
