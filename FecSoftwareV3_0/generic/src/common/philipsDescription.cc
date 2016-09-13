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

#include "deviceDescription.h"
#include "philipsDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a APV with all the values specified:
 * \param accessKey - device key
 */
philipsDescription::philipsDescription (keyType accessKey): deviceDescription ( PHILIPS, accessKey ) { }

/** \brief Constructor in order to set all the parameters
 * Build a description of a Philips with all the values specified:
 * \param accessKey - key access of a device
 * \param Value - initialise the philips LED register
 */
philipsDescription::philipsDescription ( keyType accessKey,tscType8 value):

  deviceDescription ( PHILIPS, accessKey ) {

  setRegister (value) ;
}


/** \brief Constructor in order to set all the parameters
 * Build a description of a Philips with all the values specified:
 * \param fecSlot - FEC slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param Value - initialise the philips LED register
 */
philipsDescription::philipsDescription ( tscType16 fecSlot,
					 tscType16 ringSlot,
					 tscType16 ccuAddress,
					 tscType16 i2cChannel,
					 tscType16 i2cAddress,
					 tscType8  value ):

  deviceDescription ( PHILIPS, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setRegister (value) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a Philips with all the values specified:
 * \param Value - initialise the philips LED register
 */
philipsDescription::philipsDescription ( tscType8 value ):

  deviceDescription ( PHILIPS ) {

  setDescriptionValues (value) ;
}

/* build a description of a PHILIPS with a list of parameters name
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 */
philipsDescription::philipsDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( PHILIPS, parameterNames) {
      
  setRegister (*((tscType8 *) parameterNames[PHILIPSPARAMETERNAMES[REGISTER]]->getValueConverted())) ;
}

/** \brief Method in order to set all the parameters
 * Build a description of a Philips with all the values specified:
 * \param Value - initialise the philips LED register
 */
void philipsDescription::setDescriptionValues ( tscType8 value ) {

  setRegister (value) ;
}
  

/** \brief return the value of the param
 * Return the value
 * \return the value
 */
tscType8 philipsDescription::getRegister ( ) { return (value_) ; }

/** \brief Set the value for register
 * Set the value for a PLL description
 * \param Value - value to be set
 */
void philipsDescription::setRegister ( tscType8 value ) { 

  value_ = value ;
}

/** \brief In order to compare two Philips descriptions
 * \param philips - Philips description to be compared
 * \return bool - true if one value are different, false if not
 */
bool philipsDescription::operator!= ( philipsDescription &philips ) {

#ifdef DEBUGMSGERROR
  displayDifferences(philips) ;
#endif

  if ((getRegister ( ) != philips.getRegister ( )))
    return true ;
  else
    return false ;
}

/** \brief In order to compare two Philips descriptions
 * \param philips - Philips description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool philipsDescription::operator== ( philipsDescription &philips ) {

#ifdef DEBUGMSGERROR
  displayDifferences(philips) ;
#endif

  if ((getRegister ( ) == philips.getRegister ( )))
    return true ;
  else
    return false ;
}

/** Clone a device description
 * \return the device description cloned
 */
philipsDescription *philipsDescription::clone ( ) {

  philipsDescription* myPhilipsDescription = new philipsDescription(*this) ;

  return myPhilipsDescription;
}

/** \brief Display the PHILIPS values where the comparison is different
 * \param philips - Philips values uploaded
 */
void philipsDescription::displayDifferences ( philipsDescription &philips ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "Philips " << msg << std::endl ;
  if (getRegister ( ) != philips.getRegister ( )) 
    std::cout << "\t" << "Register is different " << std::dec << (int)getRegister ( ) << "/" << (int)philips.getRegister ( ) << std::endl ;
}

/** \brief Display PHILIPS values
 */
void philipsDescription::display ( ) {

  std::cout << "Philips: 0x" 
	    << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;
    
  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
  std::cout << "\tRegister: " << std::dec << (tscType16)getRegister() << std::endl ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *philipsDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;

  for (unsigned int i = 0 ; i < (sizeof(PHILIPSPARAMETERNAMES)/sizeof(const char *)) ; i ++) {
    (*parameterNames)[PHILIPSPARAMETERNAMES[i]] = new ParameterDescription(PHILIPSPARAMETERNAMES[i], ParameterDescription::INTEGER8) ;
  }

  return parameterNames ;
}

/** Parameter's names
 */
const char *philipsDescription::PHILIPSPARAMETERNAMES[] = {"register"} ;

  
