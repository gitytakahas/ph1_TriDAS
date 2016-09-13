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
#ifndef DELAY25DESCRIPTION_H
#define DELAY25DESCRIPTION_H

#include "tscTypes.h"
#include "deviceDescription.h"
#include "delay25Definition.h"

#include "PixelDeviceAddresses.h"

/**
 * \class delay25Description
 * This class give a description of all the DELAY25 registers:
 * <ul>
 * <li>Delay 0
 * <li>Delay 1
 * <li>Delay 2
 * <li>Delay 3
 * <li>Delay 4
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can be set also
 * \see delay25Definition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "delay25Definition.h"
 * \author Frederic Drouhin
 * \date October 2005
 * \brief This class give a description of all the delay25 registers
 */
class delay25Description: public deviceDescription {

 private:
  tscType8 delay0_ ;
  tscType8 delay1_ ;
  tscType8 delay2_ ;
  tscType8 delay3_ ;
  tscType8 delay4_ ; 
  
 public:

  /** Enumeration to access the list of parameter's names
   */
  enum Delay25EnumType {DELAY0,DELAY1,DELAY2,DELAY3,DELAY4} ;
  
  /** Parameter's names
   */
  //static const char *DELAY25PARAMETERNAMES[sizeof(Delay25EnumType)+1] ; //= {"delay0","delay1","delay2","delay3","delay4"} ;
  static const char *DELAY25PARAMETERNAMES[DELAY4+1] ; //= {"delay0","delay1","delay2","delay3","delay4"} ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a DELAY25 with all the values specified:
   * \param accessKey - device key
   */
  delay25Description (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a DELAY25 with all the values specified:
   * \param accessKey - key access of a device
   * \param delay0 - delay 0
   * \param delay1 - delay 1
   * \param delay2 - delay 2
   * \param delay3 - delay 3 
   * \param delay4 - delay 4
   */
  delay25Description ( keyType accessKey,
                       tscType8 delay0,
                       tscType8 delay1,
                       tscType8 delay2,
                       tscType8 delay3,
                       tscType8 delay4 ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a DELAY25 with all the values specified:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   * \param delay0 - delay 0
   * \param delay1 - delay 1
   * \param delay2 - delay 2
   * \param delay3 - delay 3 
   * \param delay4 - delay 4
  
   */
  delay25Description ( tscType16 fecSlot,
                       tscType16 ringSlot,
                       tscType16 ccuAddress,
                       tscType16 i2cChannel,
                       tscType16 i2cAddress = delay25deviceAddress,
		       tscType8 delay0 = 0,
                       tscType8 delay1 = 0,
                       tscType8 delay2 = 0,
                       tscType8 delay3 = 0,
                       tscType8 delay4 = 0 ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a DELAY25 with all the values specified: 
   * \param delay0 - delay 0
   * \param delay1 - delay 1
   * \param delay2 - delay 2
   * \param delay3 - delay 3 
   * \param delay4 - delay 4
   */
  delay25Description ( tscType8 delay0,
                       tscType8 delay1,
                       tscType8 delay2,
                       tscType8 delay3,
                       tscType8 delay4 ) ;

  /* build a description of a DELAY25 with a list of parameters name
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   */
  delay25Description ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of a DELAY25 with all the values specified:
   * \param delay0 - delay 0
   * \param delay1 - delay 1
   * \param delay2 - delay 2
   * \param delay3 - delay 3 
   * \param delay4 - delay 4
   */
  void setDescriptionValues ( tscType8 delay0,
                              tscType8 delay1,
                              tscType8 delay2,
                              tscType8 delay3,
                              tscType8 delay4 ) ;

  /** \brief return the value of the param
   * Return the value of the initialisation
   * \return the value
   * \warning return always 1 (so the initialisation is done)
   */
  tscType8 getInit ( ) ;

  /** \brief return the value of the param
    * Return the value of DELAY25 register
   * \return the value
   */
  tscType8 getDelay0 ( ) ;

  /** \brief return the value of the param
   * Return the value of DELAY25 register
   * \return the value
   */
  tscType8 getDelay1 ( ) ;

  /** \brief return the value of the param
   * Return the value of DELAY25 register
   * \return the value
   */  
  tscType8 getDelay2 ( ) ;

  /** \brief return the value of the param
   * Return the value of DELAY25 register
   * \return the value
   */
  tscType8 getDelay3 ( ) ;

  /** \brief return the value of the param
   * Return the value of DELAY25 register
   * \return the value
   */
  tscType8 getDelay4 ( ) ;

  /** \brief return the value of the param
   * Return the value of DELAY25 register
   * \return the value
   */

  void setDelay0 ( tscType8 delay0 ) ;

  /** \brief set the value for register
   * Set the value for a DELAY25 description
   */
  void setDelay1 ( tscType8 delay1 ) ;

  /** \brief set the value for register
   * Set the value for a DELAY25 description
   */ 
  void setDelay2 ( tscType8 delay2 ) ;

  /** \brief set the value for register
   * Set the value for a DELAY25 description
   */
  void setDelay3 ( tscType8 delay3 ) ;

  /** \brief set the value for register
   * Set the value for a DELAY25 description
   */
  void setDelay4 ( tscType8 delay4 ) ;

  /** \brief set the value for register
   * \deprecated use now setTriggerDelay
   * Set the value for a DELAY25 description
   * \param triggerDelay - value to be set
   */
 
  bool operator!= ( delay25Description &delay25 ) ;

  /** \brief In order to compare two DELAY25 descriptions
   * \param delay25 - delay25 description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( delay25Description &delay25 ) ;

  /** Clone a device description
   * \return the device description cloned
   */
  delay25Description *clone ( ) ;

  /** \brief Display the APV values where the comparison is different
   * \param delay25 - uploaded values
   */
  void displayDifferences ( delay25Description &delay25 ) ;

  /** \brief Display the DELAY25 values
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;
} ;

#endif
