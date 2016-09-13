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

#include "pllDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a PLL with all the values specified:
 * \param accessKey - device key
 */
pllDescription::pllDescription (keyType accessKey): deviceDescription ( PLL, accessKey ) { 
  clockPhase_   = 0 ;
  triggerDelay_ = 0 ;
  pllDac_       = 255 ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a PLL with all the values specified:
 * \param accessKey - key access of a device
 * \param clockPhase - fine delay
 * \param triggerDelay - coarse delay
 */
pllDescription::pllDescription ( keyType accessKey,
				 tscType8 clockPhase, 
				 tscType8 triggerDelay,
				 tscType8 pllDac):

  deviceDescription ( PLL, accessKey ) {

  setDescriptionValues (clockPhase, triggerDelay, pllDac) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a PLL with all the values specified:
 * \param fecSlot - FEC slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param clockPhase - fine delay
 * \param triggerDelay - coarse delay
 */
pllDescription::pllDescription ( tscType16 fecSlot,
				 tscType16 ringSlot,
				 tscType16 ccuAddress,
				 tscType16 i2cChannel,
				 tscType16 i2cAddress,
				 tscType8 clockPhase, 
				 tscType8 triggerDelay,
				 tscType8 pllDac):

  deviceDescription ( PLL, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setDescriptionValues (clockPhase, triggerDelay, pllDac) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a PLL with all the values specified:
 * \param clockPhase - fine delay
 * \param triggerDelay - coarse delay
 */
pllDescription::pllDescription ( tscType8 clockPhase, 
				 tscType8 triggerDelay,
				 tscType8 pllDac ):

  deviceDescription ( PLL ) {

  setDescriptionValues (clockPhase, triggerDelay, pllDac) ;
}

/* build a description of a PLL with a list of parameters name
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 */
pllDescription::pllDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( PLL, parameterNames) {

  setClockPhase   (*((tscType8 *) parameterNames[PLLPARAMETERNAMES[DELAYFINE]]->getValueConverted())) ;
  setTriggerDelay (*((tscType8 *) parameterNames[PLLPARAMETERNAMES[DELAYCOARSE]]->getValueConverted())) ;
  setPllDac (*((tscType8 *) parameterNames[PLLPARAMETERNAMES[PLLDAC]]->getValueConverted())) ;
}

/** \brief Method in order to set all the parameters
 * Build a description of a PLL with all the values specified:
 * \param clockPhase - fine delay
 * \param triggerDelay - coarse delay
 */
void pllDescription::setDescriptionValues ( tscType8 clockPhase, 
					    tscType8 triggerDelay,
					    tscType8 pllDac) {

  setClockPhase   (clockPhase)   ;
  setTriggerDelay (triggerDelay) ;
  setPllDac       (pllDac) ;
}

/** \brief return the value of the param
 * Return the value of the initialisation
 * \return the value
 * \warning return always 1 (so the initialisation is done)
 */
tscType8 pllDescription::getInit ( ) { return (1) ; }

/** \brief return the value of the param
 * \deprecated use now getClockPhase()
 * Return the value of PLL register
 * \return the value
 */
tscType8 pllDescription::getDelayFine ( ) { return (getClockPhase()) ; }

/** \brief return the value of the param
 * \deprecated use now getClockPhase()
 * Return the value of PLL register
 * \return the value
 */
tscType8 pllDescription::getClockPhase ( ) { return (clockPhase_) ; }

/** \brief return the value of the param
 * \deprecated use now getTriggerDelay
 * Return the value of PLL register
 * \return the value
 */
tscType8 pllDescription::getDelayCoarse ( ) { return (getTriggerDelay()) ; }

/** \brief return the value of the param
 * \deprecated use now getTriggerDelay
 * Return the value of PLL register
 * \return the value
 */
tscType8 pllDescription::getTriggerDelay ( ) { return (triggerDelay_) ; }

/** \brief return the value of the param
 * Return the value of PLL DAQ register
 * \return the value
 */
tscType8 pllDescription::getPllDac ( ) { return (pllDac_) ; }

/** \brief set the value for register
 * \brief deprecated use now setClockPhase
 * Set the value for a PLL description
 * \param clockPhase - value to be set
 */
void pllDescription::setDelayFine ( tscType8 clockPhase ) { setClockPhase(clockPhase) ; }

/** \brief set the value for register
 * Set the value for a PLL description
 * \param Delay_fine - value to be set
 */
void pllDescription::setClockPhase ( tscType8 clockPhase ) { clockPhase_ = clockPhase ; }

/** \brief set the value for register
 * \deprecated use now setTriggerDelay
 * Set the value for a PLL description
 * \param triggerDelay - value to be set
 */
void pllDescription::setDelayCoarse ( tscType8 triggerDelay ) { setTriggerDelay(triggerDelay) ; }

/** \brief set the value for register
 * Set the value for a PLL description
 * \param Delay_coarse - value to be set
 */
void pllDescription::setTriggerDelay ( tscType8 triggerDelay ) { triggerDelay_ = triggerDelay ; }

/** \brief set the value for register
 * Set the value for a PLL description
 * \param Delay_coarse - value to be set
 */
void pllDescription::setPllDac ( tscType8 pllDac ) { pllDac_ = pllDac ; }

/** \brief In order to compare two PLL descriptions
 * \param pll - pll description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool pllDescription::operator!= ( pllDescription &pll ) {

#ifdef DEBUGMSGERROR
  displayDifferences (pll) ;
#endif

  if ((getClockPhase    ( ) != pll.getClockPhase    ( )) ||
      (getTriggerDelay  ( ) != pll.getTriggerDelay  ( )) ||
      (getPllDac  ( ) != pll.getPllDac  ( )) )
    return true ;
  else
    return false ;
}

/** \brief In order to compare two PLL descriptions
 * \param pll - pll description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool pllDescription::operator== ( pllDescription &pll ) {

#ifdef DEBUGMSGERROR
  displayDifferences (pll) ;
#endif

  if ((getClockPhase    ( ) == pll.getClockPhase    ( )) &&
      (getTriggerDelay  ( ) == pll.getTriggerDelay  ( )) &&
      (getPllDac  ( ) == pll.getPllDac  ( )) )
    return true ;
  else
    return false ;
}

/** Clone a device description
 * \return the device description cloned
 */
pllDescription *pllDescription::clone ( ) {

  pllDescription* myPllDescription = new pllDescription( *this ) ;

  return myPllDescription;
}

/** \brief Display the APV values where the comparison is different
 * \param pll - uploaded values
 */
void pllDescription::displayDifferences ( pllDescription &pll ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "PLL " << msg << std::endl ;
  if (getClockPhase ( ) != pll.getClockPhase ( ))
    std::cout << "\tClock phase (Delay fine) is different (" << std::dec << (int)getClockPhase() << ", " << (int)pll.getClockPhase ( ) << ")" << std::endl ;
  if (getTriggerDelay( ) != pll.getTriggerDelay( ))
    std::cout << "\tTrigger delay (Delay coarse) is different (" << std::dec << (int)getTriggerDelay() << ", " << (int)pll.getTriggerDelay () << ")" << std::endl ;
  if (getPllDac( ) != pll.getPllDac( ))
    std::cout << "\tTrigger delay (Delay coarse) is different (" << std::dec << (int)getPllDac() << ", " << (int)pll.getPllDac () << ")" << std::endl ;
}

/** \brief Display the PLL values
 */
void pllDescription::display ( ) {

  std::cout << "PLL: "
	    << "0x"  << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;
 
  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
  std::cout << "\tclock phase (delay fine): " << std::dec << (tscType16)getClockPhase() << std::endl;
  std::cout << "\ttrigger delay (delay coarse): " << std::dec << (tscType16)getTriggerDelay() << std::endl;
  std::cout << "\tPLL DAQ: " << std::dec << (tscType16)getPllDac() << std::endl;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *pllDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;
  for (unsigned int i = 0 ; i < (sizeof(pllDescription::PLLPARAMETERNAMES)/sizeof(const char *)); i ++) {
    (*parameterNames)[PLLPARAMETERNAMES[i]] = new ParameterDescription(PLLPARAMETERNAMES[i], ParameterDescription::INTEGER8) ;
  }
  return parameterNames ;
}

/** Parameter's names
 */
const char *pllDescription::PLLPARAMETERNAMES[] = {"delayFine","delayCoarse", "pllDac"} ;
