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
#ifndef ESMBRESETDESCRIPTION_H
#define ESMBRESETDESCRIPTION_H

#include "tscTypes.h"

#include "deviceDescription.h"
#include "esMbResetDefinition.h"

/**
 * \class esMbResetDescription
 * This class give a description of reset througth PIA description
 * <ul>
 * <li> mask - send bit per bit this value
 * <li> delayActiveReset - time during the signal will be maintained
 * <li> intervalDelayReset - time between two set
 * </ul>
 * <b>Note that this description never access the hardware, it is used to
 * keep the values to be set or set.</b>
 * \author Frederic Drouhin
 * \date Febrary 2003
 * \brief This class give a description of PIA reset
 */
class esMbResetDescription: public deviceDescription {

 private:

  /** Delay during the reset is maintained
   */
  unsigned long delayActiveReset_ ;

  /** Delay between two resets on the bit
   */
  unsigned long intervalDelayReset_ ;

  /** Mask to be applied bit per bit
   */
  tscType8 mask_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum EsMbEnumType {ESMBCRATEID,ESMBFECSLOT,ESMBRINGSLOT,ESMBCCUADDRESS,ESMBCHANNEL,DELAYACTIVERESET,INTERVALDELAYRESET,MASK} ;
  
  /** Parameter's names
   */
  static const char *ESMBRESETPARAMETERNAMES[MASK+1] ; //= {"crateId","fecSlot","ringSlot","ccuAddress","esMbChannel","delayActiveReset","intervalDelayReset","mask"} ;
  



  /** \brief Constructor in order to set all the parameters
   * Build a description of a PIA reset with all the values specified:
   * \param accessKey - key access of a device
   * \param mask - value for the reset
   * \param delayActiveReset - time during the signal will be maintained
   * \param intervalDelayReset - time between two set
   */
  esMbResetDescription ( keyType accessKey,
                        unsigned long delayActiveReset,
                        unsigned long intervalDelayReset,
                        tscType8 mask ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a PIA reset with all the values specified:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   * \param mask - value for the reset
   * \param delayActiveReset - time during the signal will be maintained
   * \param intervalDelayReset - time between two set
   */
  esMbResetDescription ( tscType16 fecSlot,
                        tscType16 ringSlot,
                        tscType16 ccuAddress,
                        tscType16 i2cChannel,
                        tscType16 i2cAddress = 0,
                        unsigned long delayActiveReset = 0,
                        unsigned long intervalDelayReset = 0,
                        tscType8 mask = 0,
			std::string fecHardwareId = "0" ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a PIA reset with all the values specified:
   * \param mask - value for the reset
   * \param delayActiveReset - time during the signal will be maintained
   * \param intervalDelayReset - time between two set
   */
  esMbResetDescription ( unsigned long delayActiveReset,
			unsigned long intervalDelayReset,
			tscType8 mask ) ;

  /* build a description of a PIA reset description with a list of parameters name
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   */
  esMbResetDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of a MUX with all the values specified:
   * \param Resistor - resistor of the MUX
   */
  void setDescriptionValues ( unsigned long delayActiveReset,
			      unsigned long intervalDelayReset,
			      tscType8 mask ) ;

  /** \brief set the delay active reset
   * \param delayActiveReset - value to be set
   */
  void setDelayActiveReset ( unsigned long delayActiveReset ) ;

  /** \brief set the interval delay reset
   * \param intervalDelayReset - value to be set
   */
  void setIntervalDelayReset ( unsigned long intervalDelayReset ) ;

  /** \brief set the mask 
   * \param mask - value to be set
   */
  void setMask ( tscType8 mask ) ;

  /** \brief return the mask for the PIA
   * \return Return the PIA mask
   */
  tscType8 getMask ( ) ;

  /** \brief return the value of the delay active reset
   * \return Return Delay active reset
   */
  unsigned long getDelayActiveReset ( ) ;

  /** \brief return the value of the interval delay reset
   * \return Return Interval delay reset
   */
  unsigned long getIntervalDelayReset ( ) ;

  /** \brief In order to compare two pia reset description
   * In order to compare two pia mux description
   * \param esMbReset - PIA reset description
   */
  bool operator== ( esMbResetDescription &esMbReset ) ;

  /** brief Clone PIA reset description
   * \return the device description cloned
   */
  esMbResetDescription *clone ( ) ;

  /** \brief Display the PIA reset values
   */
  void display ( ) ;

  /** \brief Return a hash_map of parameter name
   * must be delete by the owner of the description or by the method deleteParameterNames
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;
} ;

#endif
