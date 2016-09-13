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

#include "muxDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a MUX with all the values specified:
 * \param accessKey - device key
 */
muxDescription::muxDescription (keyType accessKey): deviceDescription ( APVMUX, accessKey ) { }

/** \brief Constructor in order to set all the parameters
 * Build a description of a MUX with all the values specified:
 * \param accessKey - key access of a device
 * \param Resistor - resistor of the MUX
 */
muxDescription::muxDescription ( keyType accessKey,
				 tscType16 resistor):

  deviceDescription ( APVMUX, accessKey ) {

  setDescriptionValues (resistor) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a MUX with all the values specified:
 * \param fecSlot - FEC slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param Resistor - resistor of the MUX
 */
muxDescription::muxDescription ( tscType16 fecSlot,
				 tscType16 ringSlot,
				 tscType16 ccuAddress,
				 tscType16 i2cChannel,
				 tscType16 i2cAddress,
				 tscType16 resistor):

  deviceDescription ( APVMUX, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setDescriptionValues (resistor) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a MUX with all the values specified:
 * \param Resistor - resistor of the MUX
 */
muxDescription::muxDescription ( tscType16 resistor ):

  deviceDescription ( APVMUX ) {

  setDescriptionValues (resistor) ;
}

/* build a description of a MUX with a list of parameters name
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 */
muxDescription::muxDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( APVMUX, parameterNames) {
      
  setResistor (*((tscType8 *) parameterNames[MUXPARAMETERNAMES[RESISTOR]]->getValueConverted())) ;
}

/** \brief Method in order to set all the parameters
 * Build a description of a MUX with all the values specified:
 * \param Resistor - resistor of the MUX
 */
void muxDescription::setDescriptionValues ( tscType16 resistor ) {

  setResistor (resistor) ;
}


/** \brief return the value of the resistor
 * Return the value of MUX resistor
 * \return the value
 */
tscType16 muxDescription::getResistor ( ) { return (resistor_) ; }

/** \brief set the value for MUX register
 * Set the resistor for a MUX description
 * \param Status - value to be set
 */
void muxDescription::setResistor ( tscType16 resistor ) { 

  resistor_ = resistor ;
}

/** \brief In order to compare two MUX description
 * \param mux - mux description to be compared
 * \return bool - true if one value are different, false if not
 */
bool muxDescription::operator!= ( muxDescription &mux ) {

#ifdef DEBUGMSGERROR
  displayDifferences ( mux ) ;
#endif

  if ((getResistor ( ) != mux.getResistor ( )))
    return true ;
  else
    return false ;
}

/** \brief In order to compare two MUX description
 * \param mux - mux description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool muxDescription::operator== ( muxDescription &mux ) {

#ifdef DEBUGMSGERROR
  displayDifferences ( mux ) ;
#endif

  if ((getResistor ( ) == mux.getResistor ( )))
    return true ;
  else
    return false ;
}

/** Clone a device description
 * \return the device description cloned
 */
muxDescription *muxDescription::clone ( ) {

  muxDescription* myMuxDescription =  new muxDescription ( *this ) ;

  return myMuxDescription;

}

/** \brief Display the APV values where the comparison is different
 * \param mux - uploaded values
 */
void muxDescription::displayDifferences ( muxDescription &mux ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "APVMUX " << msg << std::endl ;
  if ((getResistor ( ) != mux.getResistor ( )))
    std::cout << "\tResistor is different (" << (unsigned int) getResistor() << " / " << (unsigned int) mux.getResistor ( ) << ")" << std::endl ;
}

/** \brief Display MUX values
 */
void muxDescription::display ( ) {
    
  std::cout << "MUX: 0x" 
	    << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;

  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
  std::cout << "\tResistor: " << std::dec << (tscType16)getResistor() << std::endl ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *muxDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;

  for (unsigned int i = 0 ; i < (sizeof(MUXPARAMETERNAMES)/sizeof(const char *)) ; i ++) {
    (*parameterNames)[MUXPARAMETERNAMES[i]] = new ParameterDescription(MUXPARAMETERNAMES[i], ParameterDescription::INTEGER16) ;
  }

  return parameterNames ;
}

/** Parameter's names
 */
const char *muxDescription::MUXPARAMETERNAMES[] = {"resistor"} ;

