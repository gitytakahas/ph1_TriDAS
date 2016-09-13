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
#ifndef PHILIPSDESCRIPTION_H
#define PHILIPSDESCRIPTION_H

#include "tscTypes.h"
#include "philipsDefinition.h"

/**
 * \class philipsDescription
 * This class give a description of all the PHILIPS registers:
 * <ul>
 * <li>Register
 * </ul>
 * <p><b>Note that this description never access the hardware, it is used to
 * keep the values to be set or set.</b>
 * <p><b>This class is just used for test program with generic philips devices</b>
 * \see philipsDefinition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "philipsDefinition.h"
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class give a description of all the philips registers
 */
class philipsDescription: public deviceDescription {

 private:

  /** Value of the register
   */
  tscType8 value_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum{REGISTER} ;

  /** Parameter's names
   */
  static const char *PHILIPSPARAMETERNAMES[REGISTER+1] ; //= {"register"} ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a APV with all the values specified:
   * \param accessKey - device key
   */
  philipsDescription (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a Philips with all the values specified:
   * \param accessKey - key access of a device
   * \param Value - initialise the philips LED register
   */
  philipsDescription ( keyType accessKey,tscType8 value) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a Philips with all the values specified:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   * \param Value - initialise the philips LED register
   */
  philipsDescription ( tscType16 fecSlot,
                       tscType16 ringSlot,
                       tscType16 ccuAddress,
                       tscType16 i2cChannel,
                       tscType16 i2cAddress,
                       tscType8  value = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a Philips with all the values specified:
   * \param Value - initialise the philips LED register
   */
  philipsDescription ( tscType8 value ) ;

  /* build a description of a PHILIPS with a list of parameters name
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   */
  philipsDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of a Philips with all the values specified:
   * \param Value - initialise the philips LED register
   */
  void setDescriptionValues ( tscType8 value ) ;  

  /** \brief return the value of the param
   * Return the value
   * \return the value
   */
  tscType8 getRegister ( ) ;

  /** \brief Set the value for register
   * Set the value for a PLL description
   * \param Value - value to be set
   */
  void setRegister ( tscType8 value ) ;

  /** \brief In order to compare two Philips descriptions
   * \param philips - Philips description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( philipsDescription &philips ) ;

  /** \brief In order to compare two Philips descriptions
   * \param philips - Philips description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( philipsDescription &philips ) ;

  /** Clone a device description
   * \return the device description cloned
   */
  philipsDescription *clone ( ) ;

  /** \brief Display the PHILIPS values where the comparison is different
   * \param philips - Philips values uploaded
   */
  void displayDifferences ( philipsDescription &philips ) ;

  /** \brief Display PHILIPS values
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

} ;

#endif
