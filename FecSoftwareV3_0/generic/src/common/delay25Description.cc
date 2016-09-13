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

#include "delay25Description.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a DELAY25 with all the values specified:
 * \param accessKey - device key
 */
delay25Description::delay25Description (keyType accessKey): deviceDescription ( DELAY25, accessKey ) { }

/** \brief Constructor in order to set all the parameters
 * Build a description of a DELAY25 with all the values specified:
 * \param accessKey - key access of a device 
 * \param delay0 - delay 0
 * \param delay1 - delay 1
 * \param delay2 - delay 2
 * \param delay3 - delay 3 
 * \param delay4 - delay 4
 */
delay25Description::delay25Description ( keyType accessKey,
                                         tscType8 delay0,
                                         tscType8 delay1,
                                         tscType8 delay2,
                                         tscType8 delay3,
                                         tscType8 delay4 ):

  deviceDescription ( DELAY25, accessKey ) {
  
  setDescriptionValues ( delay0, delay1, delay2, delay3, delay4 ) ;
}

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
delay25Description::delay25Description ( tscType16 fecSlot,
				         tscType16 ringSlot,
				         tscType16 ccuAddress,
				         tscType16 i2cChannel,
				         tscType16 i2cAddress,
                                         tscType8 delay0,
                                         tscType8 delay1,
                                         tscType8 delay2,
                                         tscType8 delay3,
                                         tscType8 delay4 ):

  deviceDescription ( DELAY25, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {
 
  setDescriptionValues ( delay0, delay1, delay2, delay3, delay4 ) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a DELAY25 with all the values specified:
 * \param delay0 - delay 0
 * \param delay1 - delay 1
 * \param delay2 - delay 2
 * \param delay3 - delay 3 
 * \param delay4 - delay 4
 */
delay25Description::delay25Description ( tscType8 delay0,
                                         tscType8 delay1,
                                         tscType8 delay2,
                                         tscType8 delay3,
                                         tscType8 delay4 ):

  deviceDescription ( DELAY25 ) {

  setDescriptionValues ( delay0, delay1, delay2, delay3, delay4 ) ;
}

/* build a description of a DELAY25 with a list of parameters name
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 */
delay25Description::delay25Description ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( DELAY25, parameterNames) { 
      
  setDelay0    (*((tscType8 *) parameterNames[DELAY25PARAMETERNAMES[DELAY0]]->getValueConverted())) ;
  setDelay1    (*((tscType8 *) parameterNames[DELAY25PARAMETERNAMES[DELAY1]]->getValueConverted())) ;
  setDelay2    (*((tscType8 *) parameterNames[DELAY25PARAMETERNAMES[DELAY2]]->getValueConverted())) ;
  setDelay3    (*((tscType8 *) parameterNames[DELAY25PARAMETERNAMES[DELAY3]]->getValueConverted())) ;
  setDelay4    (*((tscType8 *) parameterNames[DELAY25PARAMETERNAMES[DELAY4]]->getValueConverted())) ;
  
}

/** \brief Method in order to set all the parameters
 * Build a description of a DELAY25 with all the values specified: 
 * \param delay0 - delay 0
 * \param delay1 - delay 1
 * \param delay2 - delay 2
 * \param delay3 - delay 3 
 * \param delay4 - delay 4
 */
void delay25Description::setDescriptionValues ( tscType8 delay0,
                                                tscType8 delay1,
                                                tscType8 delay2,
                                                tscType8 delay3,
                                                tscType8 delay4 ) {
  

  setDelay0   (delay0)   ; 
  setDelay1   (delay1)   ; 
  setDelay2   (delay2)   ; 
  setDelay3   (delay3)   ; 
  setDelay4   (delay4)   ;
  
}

/** \brief return the value of the param
 * Return the value of the initialisation
 * \return the value
 * \warning return always 1 (so the initialisation is done)
 */
tscType8 delay25Description::getInit ( ) { return (1) ; }

/** \brief return the value of the param
 * Return the value of DELAY25 register
 * \return the value
 */

tscType8 delay25Description::getDelay0 ( ) { return (delay0_) ; }

/** \brief return the value of the param
 * Return the value of DELAY25 register
 * \return the value
 */
tscType8 delay25Description::getDelay1 ( ) { return (delay1_) ; }

/** \brief return the value of the param
 * Return the value of DELAY25 register
 * \return the value
 */
tscType8 delay25Description::getDelay2 ( ) { return (delay2_) ; }

/** \brief return the value of the param
 * Return the value of DELAY25 register
 * \return the value
 */
tscType8 delay25Description::getDelay3 ( ) { return (delay3_) ; }

/** \brief return the value of the param
 * Return the value of DELAY25 register
 * \return the value
 */
tscType8 delay25Description::getDelay4 ( ) { return (delay4_) ; }

/** \brief return the value of the param
 * Return the value of DELAY25 register
 * \return the value
 */

void delay25Description::setDelay0 ( tscType8 delay0 ) { delay0_ = delay0 ; }

/** \brief set the value for register
 * Set the value for a DELAY25 description
 * \param triggerDelay - value to be set
 */
void delay25Description::setDelay1 ( tscType8 delay1 ) { delay1_ = delay1 ; }

/** \brief set the value for register
 * Set the value for a DELAY25 description
 * \param triggerDelay - value to be set
 */
void delay25Description::setDelay2 ( tscType8 delay2 ) { delay2_ = delay2 ; }

/** \brief set the value for register
 * Set the value for a DELAY25 description
 * \param triggerDelay - value to be set
 */
void delay25Description::setDelay3 ( tscType8 delay3 ) { delay3_ = delay3 ; }

/** \brief set the value for register
  * Set the value for a DELAY25 description
 * \param triggerDelay - value to be set
 */
void delay25Description::setDelay4 ( tscType8 delay4 ) { delay4_ = delay4 ; }

/** \brief set the value for register
 * Set the value for a DELAY25 description
 * \param triggerDelay - value to be set
 */

bool delay25Description::operator!= ( delay25Description &delay25 ) {

#ifdef DEBUGMSGERROR
  displayDifferences (delay25) ;
#endif

  if ((getDelay0 ( ) != delay25.getDelay0  ( )) ||
      (getDelay1 ( ) != delay25.getDelay1  ( )) ||
      (getDelay2 ( ) != delay25.getDelay2  ( )) ||
      (getDelay3 ( ) != delay25.getDelay3  ( )) ||
      (getDelay4 ( ) != delay25.getDelay4  ( ))  )
    return true ;
  else
    return false ;
}

/** \brief In order to compare two DELAY25 descriptions
 * \param delay25 - delay25 description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool delay25Description::operator== ( delay25Description &delay25 ) {

#ifdef DEBUGMSGERROR
  displayDifferences (delay25) ;
#endif

  if ((getDelay0 ( ) == delay25.getDelay0 ( )) &&
      (getDelay1 ( ) == delay25.getDelay1 ( )) &&
      (getDelay2 ( ) == delay25.getDelay2 ( )) &&
      (getDelay3 ( ) == delay25.getDelay3 ( )) &&
      (getDelay4 ( ) == delay25.getDelay4 ( ))  )
    return true ;
  else
    return false ;
}

/** Clone a device description
 * \return the device description cloned
 */
delay25Description *delay25Description::clone ( ) {

  delay25Description* myDelay25Description = new delay25Description( *this ) ;

  return myDelay25Description;
}

/** \brief Display the APV values where the comparison is different
 * \param delay25 - uploaded values
 */
void delay25Description::displayDifferences ( delay25Description &delay25 ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "DELAY25 " << msg << std::endl ;
  if (getDelay0 ( ) != delay25.getDelay0 ( ))
    std::cout << "\tDelay0 is different (" << std::dec << (int)getDelay0() << ", " << (int)delay25.getDelay0 ( ) << ")" << std::endl ;
  if (getDelay1 ( ) != delay25.getDelay1 ( ))
    std::cout << "\tDelay1 is different (" << std::dec << (int)getDelay1() << ", " << (int)delay25.getDelay1 ( ) << ")" << std::endl ; 
  if (getDelay2 ( ) != delay25.getDelay2 ( ))
    std::cout << "\tDelay2 is different (" << std::dec << (int)getDelay2() << ", " << (int)delay25.getDelay2 ( ) << ")" << std::endl ;
  if (getDelay3 ( ) != delay25.getDelay3 ( ))
    std::cout << "\tDelay3 is different (" << std::dec << (int)getDelay3() << ", " << (int)delay25.getDelay3 ( ) << ")" << std::endl ; 
  if (getDelay4 ( ) != delay25.getDelay4 ( ))
    std::cout << "\tDelay4 is different (" << std::dec << (int)getDelay4() << ", " << (int)delay25.getDelay4 ( ) << ")" << std::endl ;


}

/** \brief Display the DELAY25 values
 */
void delay25Description::display ( ) {

  std::cout << "DELAY25: "
	    << "0x" << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;
 
  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
  std::cout << "\tDelay 0: " << std::dec << (tscType16)getDelay0() << std::endl; 
  std::cout << "\tDelay 1: " << std::dec << (tscType16)getDelay1() << std::endl;
  std::cout << "\tDelay 2: " << std::dec << (tscType16)getDelay2() << std::endl; 
  std::cout << "\tDelay 3: " << std::dec << (tscType16)getDelay3() << std::endl;
  std::cout << "\tDelay 4: " << std::dec << (tscType16)getDelay4() << std::endl; 
 
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *delay25Description::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;
  for (unsigned int i = 0 ; i < (sizeof(DELAY25PARAMETERNAMES)/sizeof(const char *)); i ++) {
    (*parameterNames)[DELAY25PARAMETERNAMES[i]] = new ParameterDescription(DELAY25PARAMETERNAMES[i], ParameterDescription::INTEGER8) ;
  }
  return parameterNames ;
}

/** Parameter's names
 */
const char *delay25Description::DELAY25PARAMETERNAMES[] = {"delay0","delay1","delay2","delay3","delay4"} ;
