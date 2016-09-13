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
#ifndef MUXDESCRIPTION_H
#define MUXDESCRIPTION_H

#include "tscTypes.h"
#include "deviceDescription.h"
#include "muxDefinition.h"

/**
 * \class muxDescription
 * This class give a description of MUX register:
 * <ul>
 * <li> Resistor
 * </ul>
 * <b>Note that this description never access the hardware, it is used to
 * keep the values to be set or set.</b>
 * \see muxDefinition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "muxDefinition.h"
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class give a description of all the MUX registers
 */
class muxDescription: public deviceDescription {

 private:

  /** MUX resistor
   */
  tscType16 resistor_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum MuxEnumType {RESISTOR} ;

  /** Parameter's names
   */
  static const char *MUXPARAMETERNAMES[RESISTOR+1] ; // = {"resistor"} ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a MUX with all the values specified:
   * \param accessKey - device key
   */
  muxDescription (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a MUX with all the values specified:
   * \param accessKey - key access of a device
   * \param Resistor - resistor of the MUX
   */
  muxDescription ( keyType accessKey,
                   tscType16 resistor) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a MUX with all the values specified:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   * \param Resistor - resistor of the MUX
   */
  muxDescription ( tscType16 fecSlot,
                   tscType16 ringSlot,
                   tscType16 ccuAddress,
                   tscType16 i2cChannel,
                   tscType16 i2cAddress = 0x43,
                   tscType16 resistor = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a MUX with all the values specified:
   * \param Resistor - resistor of the MUX
   */
  muxDescription ( tscType16 resistor ) ;

  /* build a description of a MUX with a list of parameters name
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   */
  muxDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of a MUX with all the values specified:
   * \param Resistor - resistor of the MUX
   */
  void setDescriptionValues ( tscType16 resistor ) ;

  /** \brief return the value of the resistor
   * Return the value of MUX resistor
   * \return the value
   */
  tscType16 getResistor ( ) ;

  /** \brief set the value for MUX register
   * Set the resistor for a MUX description
   * \param Status - value to be set
   */
  void setResistor ( tscType16 resistor ) ;

  /** \brief In order to compare two MUX description
   * \param mux - mux description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( muxDescription &mux ) ;

  /** \brief In order to compare two MUX description
   * \param mux - mux description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( muxDescription &mux ) ;

  /** Clone a device description
   * \return the device description cloned
   */
  muxDescription *clone ( ) ;

  /** \brief Display the APV values where the comparison is different
   * \param mux - uploaded values
   */
  void displayDifferences ( muxDescription &mux ) ;

  /** \brief Display MUX values
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

} ;

#endif
