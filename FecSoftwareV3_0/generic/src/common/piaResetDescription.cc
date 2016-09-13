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

#include "piaResetDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a PIA reset with all the values specified:
 * \param accessKey - key access of a device
 * \param mask - value for the reset
 * \param delayActiveReset - time during the signal will be maintained
 * \param intervalDelayReset - time between two set
 */
piaResetDescription::piaResetDescription ( keyType accessKey,
					   unsigned long delayActiveReset,
					   unsigned long intervalDelayReset,
					   tscType8 mask ):

  deviceDescription ( PIARESET, accessKey ) {

  setDescriptionValues (delayActiveReset, intervalDelayReset, mask) ;
}

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
piaResetDescription::piaResetDescription ( tscType16 fecSlot,
					   tscType16 ringSlot,
					   tscType16 ccuAddress,
					   tscType16 i2cChannel,
					   tscType16 i2cAddress,
					   unsigned long delayActiveReset,
					   unsigned long intervalDelayReset,
					   tscType8 mask,
					   std::string fecHardwareId ):

  deviceDescription ( PIARESET, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress, fecHardwareId ) {

  setDescriptionValues (delayActiveReset, intervalDelayReset, mask) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a PIA reset with all the values specified:
 * \param mask - value for the reset
 * \param delayActiveReset - time during the signal will be maintained
 * \param intervalDelayReset - time between two set
 */
piaResetDescription::piaResetDescription ( unsigned long delayActiveReset,
					   unsigned long intervalDelayReset,
					   tscType8 mask ):

  deviceDescription ( PIARESET ) {

  setDescriptionValues (delayActiveReset, intervalDelayReset, mask) ;
}

/* build a description of a PIA reset description with a list of parameters name
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 */
piaResetDescription::piaResetDescription ( parameterDescriptionNameType parameterNames ):
  deviceDescription ( PIARESET ) {

  setFecHardwareId ((std::string)parameterNames[FECPARAMETERNAMES[FECHARDWAREID]]->getValue(),
		    *((tscType32 *) parameterNames[PIARESETPARAMETERNAMES[PIACRATEID]]->getValueConverted())) ;
  //setCrateSlot (*((tscType32 *) parameterNames[PIARESETPARAMETERNAMES[PIACRATEID]]->getValueConverted())) ;
  setFecSlot (*((tscType32 *) parameterNames[PIARESETPARAMETERNAMES[PIAFECSLOT]]->getValueConverted())) ;
  setRingSlot (*((tscType32 *) parameterNames[PIARESETPARAMETERNAMES[PIARINGSLOT]]->getValueConverted())) ;
  setCcuAddress (*((tscType32 *) parameterNames[PIARESETPARAMETERNAMES[PIACCUADDRESS]]->getValueConverted())) ;
  setChannel (*((tscType32 *) parameterNames[PIARESETPARAMETERNAMES[PIACHANNEL]]->getValueConverted())) ;
  setDelayActiveReset (*((tscType32 *) parameterNames[PIARESETPARAMETERNAMES[DELAYACTIVERESET]]->getValueConverted())) ;
  setIntervalDelayReset (*((tscType32 *) parameterNames[PIARESETPARAMETERNAMES[INTERVALDELAYRESET]]->getValueConverted())) ;
  setMask (*((tscType8 *) parameterNames[PIARESETPARAMETERNAMES[MASK]]->getValueConverted())) ;

  if ((std::string)parameterNames[FECPARAMETERNAMES[ENABLED]]->getValue() == STRFALSE) setEnabled(false) ;
  else setEnabled(true) ;
}

/** \brief Method in order to set all the parameters
 * Build a description of a MUX with all the values specified:
 * \param Resistor - resistor of the MUX
 */
void piaResetDescription::setDescriptionValues ( unsigned long delayActiveReset,
						 unsigned long intervalDelayReset,
						 tscType8 mask ) {

  setDelayActiveReset ( delayActiveReset ) ;
  setIntervalDelayReset ( intervalDelayReset ) ;
  setMask ( mask ) ;
}

/** \brief set the delay active reset
 * \param delayActiveReset - value to be set
 */
void piaResetDescription::setDelayActiveReset ( unsigned long delayActiveReset ) { 
  delayActiveReset_ = delayActiveReset ; 
}

/** \brief set the interval delay reset
 * \param intervalDelayReset - value to be set
 */
void piaResetDescription::setIntervalDelayReset ( unsigned long intervalDelayReset ) { 
  intervalDelayReset_ = intervalDelayReset ; 
}

/** \brief set the mask 
 * \param mask - value to be set
 */
void piaResetDescription::setMask ( tscType8 mask ) { 
  mask_ = mask ;
}

/** \brief return the mask for the PIA
 * \return Return the PIA mask
 */
tscType8 piaResetDescription::getMask ( ) { return (mask_) ; }

/** \brief return the value of the delay active reset
 * \return Return Delay active reset
 */
unsigned long piaResetDescription::getDelayActiveReset ( ) { return (delayActiveReset_) ; }

/** \brief return the value of the interval delay reset
 * \return Return Interval delay reset
 */
unsigned long piaResetDescription::getIntervalDelayReset ( ) { return (intervalDelayReset_) ; }

/** \brief In order to compare two pia reset description
 * In order to compare two pia mux description
 * \param piaReset - PIA reset description
 */
bool piaResetDescription::operator== ( piaResetDescription &piaReset ) {

  if ((getDelayActiveReset ( ) == piaReset.getDelayActiveReset ( )) &&
      (getIntervalDelayReset ( ) == piaReset.getIntervalDelayReset ( )) &&
      (getMask ( ) == piaReset.getMask ( )))
    return true ;
  else
    return false ;
}

/** brief Clone PIA reset description
 * \return the device description cloned
 */
piaResetDescription *piaResetDescription::clone ( ) {

  piaResetDescription *piaReset = new piaResetDescription ( *this ) ;

  return (piaReset) ;
}

/** \brief Display the PIA reset values
 */
void piaResetDescription::display ( ) {

  std::cout << "PIA channel: "
	    << std::hex << "0x" << getFecSlot()
	    << " 0x" << getRingSlot()
	    << " 0x" << getCcuAddress()
	    << " 0x" << getChannel() << std::dec << std::endl ;

  std::cout << "\tDelay active reset: " << (unsigned long)getDelayActiveReset() << std::endl;
  std::cout << "\tInterval delay reset: " << (unsigned long)getIntervalDelayReset() << std::endl;
  std::cout << "\tMask: " << (tscType16)getMask() << std::endl;

}

/** \brief Return a hash_map of parameter name
 * must be delete by the owner of the description or by the method deleteParameterNames
 */
parameterDescriptionNameType *piaResetDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;
  (*parameterNames)[FECPARAMETERNAMES[FECHARDWAREID]] = new ParameterDescription(FECPARAMETERNAMES[FECHARDWAREID], ParameterDescription::STRING) ;
  (*parameterNames)[FECPARAMETERNAMES[ENABLED]] = new ParameterDescription(FECPARAMETERNAMES[ENABLED], ParameterDescription::STRING) ;

  for (unsigned int i = 0 ; i < (sizeof(PIARESETPARAMETERNAMES)/sizeof(const char *)) ; i ++) {
    (*parameterNames)[PIARESETPARAMETERNAMES[i]] = new ParameterDescription (PIARESETPARAMETERNAMES[i], ParameterDescription::INTEGER32) ;
  }
  (*parameterNames)[PIARESETPARAMETERNAMES[MASK]]->setType(ParameterDescription::INTEGER8) ;

  return parameterNames ;
}

/** Parameter's names
 */
const char *piaResetDescription::PIARESETPARAMETERNAMES[] = {"crateSlot",
							     "fecSlot",
							     "ringSlot",
							     "ccuAddress",
							     "piaChannel",
							     "delayActiveReset",
							     "intervalDelayReset",
							     "mask"} ;
