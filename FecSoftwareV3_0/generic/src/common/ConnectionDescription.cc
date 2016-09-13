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
#include "ConnectionDescription.h"

/** Constructor for ConnectionDescription
 * \param fedCrateId - FED crate ID
 * \param fedSlot - FED slot
 * \param fedId - FED software ID
 * \param fedChannel -  FED channel
 * \param fecCrateId - crate ID
 * \param fecSlot - FEC Slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - I2C Channel
 * \param apvAddress - I2C Channel
 * \param dcuHardId - DCU hardware ID
 * \param enabled - Enabled connection
 * \param fecCrateId - FED crate number
 * \param fedSlot - VME FED slot
 */
ConnectionDescription::ConnectionDescription ( unsigned int fedId, unsigned int fedChannel,
					       std::string fecHardwareId,
					       unsigned int fecCrateId, unsigned int fecSlot, 
					       unsigned int ringSlot, unsigned int ccuAddress, 
					       unsigned int i2cChannel, unsigned int apvAddress,
					       unsigned int dcuHardId, bool enabled, 
					       unsigned int fedCrateId, unsigned int fedSlot ): 
  fedCrateId_(fedCrateId), fedSlot_(fedSlot), fedId_(fedId), fedChannel_(fedChannel), fecCrateId_(fecCrateId), fecSlot_(fecSlot), ringSlot_(ringSlot),
  ccuAddress_(ccuAddress), i2cChannel_(i2cChannel), apvAddress_(apvAddress), dcuHardId_(dcuHardId), enabled_(enabled) {

  setFecHardwareId (fecHardwareId) ;  


}

/** Constructor for ConnectionDescription with the parameter names and values
 * \param parameterNames - description of the parameters (fec, ring, ccu, channel, address)
 */
ConnectionDescription::ConnectionDescription ( parameterDescriptionNameType parameterNames ):
  fecCrateId_(0) {

  setFedCrateId(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[FEDCRATEID]]->getValueConverted())) ;
  setFedSlot(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[FEDSLOT]]->getValueConverted())) ;
  setFedId(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[FEDID]]->getValueConverted())) ;
  setFedChannel(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[FEDCHANNEL]]->getValueConverted())) ;
  setFecHardwareId ((std::string)parameterNames[CONNECTIONPARAMETERNAMES[FECHARDWAREID]]->getValue()) ;
  setFecCrateId ( *((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[FECCRATEID]]->getValueConverted()) ) ;
  setFecSlot(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[FECSLOT]]->getValueConverted())) ;
  setRingSlot(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[FECRING]]->getValueConverted())) ;
  setCcuAddress(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[CCUADDRESS]]->getValueConverted())) ;
  setI2cChannel(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[CCUCHANNEL]]->getValueConverted())) ;
  setApvAddress(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[APVADDRESS]]->getValueConverted())) ;
  setDcuHardId(*((unsigned int *) parameterNames[CONNECTIONPARAMETERNAMES[DCUHARDID]]->getValueConverted())) ;
  
  if ((std::string)parameterNames[CONNECTIONPARAMETERNAMES[ENABLED]]->getValue() == STRFALSE) setEnabled(false) ;
  else setEnabled(true) ;

}

/** \brief display a connection description
 * \param of - stream
 */
void ConnectionDescription::display ( ) {
  
  std::cerr << "Connection between the FED " << std::dec << fedId_ << " channel " << fedChannel_
	    << " on crate " << std::dec << fedCrateId_ << " VME slot " << fedSlot_ 
	    << " and the APV on crate " << fecCrateId_ << " VME slot " << fecSlot_ << " ring " << ringSlot_ << " CCU 0x" << std::hex << ccuAddress_
	    << " channel " << std::dec << i2cChannel_ << " address 0x" << std::hex << apvAddress_ 
	    << " for the DCU " << std::dec << dcuHardId_ << (enabled_ ? " (connection enabled)": " (connection disabled)") << std::endl ;
}

/** \brief In order to compare two connection descriptions
 * \param connection - description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool ConnectionDescription::operator== ( ConnectionDescription &connection ) {

  if (
      (getFedCrateId ( ) == connection.getFedCrateId ( )) &&
      (getFedSlot ( ) == connection.getFedSlot ( )) &&
      (getFedId ( ) == connection.getFedId ( )) &&
      (getFedChannel ( ) == connection.getFedChannel ( )) &&
      (getFecHardwareId ( ) == connection.getFecHardwareId ( )) &&
      (getFecCrateId ( ) == connection.getFecCrateId ( )) &&
      (getFecSlot ( ) == connection.getFecSlot ( )) &&
      (getRingSlot( ) == connection.getRingSlot( )) &&
      (getCcuAddress( ) == connection.getCcuAddress( )) &&
      (getI2cChannel( ) == connection.getI2cChannel( )) &&
      (getApvAddress( ) == connection.getApvAddress( )) &&
      (getDcuHardId( ) == connection.getDcuHardId( ))
      ) 
    return true ;
  else
    return false ;
}

/** Use to sort a vector of connections description FEC, ring, ccu, channel
 * \param r1 - connection description
 * \param r2 - connection description
 */
bool ConnectionDescription::sortByKey ( ConnectionDescription *r1, ConnectionDescription *r2 ) {

  if (r1->getKey() < r2->getKey()) return true ;
  else return false ;
}

/** Use to sort a vector of connections description by DCU hard id
 * \param r1 - connection description
 * \param r2 - connection description
 */
bool ConnectionDescription::sortByDcuHardId ( ConnectionDescription *r1, ConnectionDescription *r2 ) {

  if (r1->getDcuHardId() < r2->getDcuHardId()) return true ;
  else return false ;
}

/** \brief clone the connection
 * \return ConnectionDescription cloned
 */
ConnectionDescription *ConnectionDescription::clone ( ) {

  ConnectionDescription *connection = new ConnectionDescription (*this) ;
  return connection ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *ConnectionDescription::getParameterNames ( ) {
  
  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;
  
  // For the rest of the parameters
  for (unsigned int i = 0 ; i < (ConnectionDescription::ENABLED+1) ; i ++) {

    (*parameterNames)[CONNECTIONPARAMETERNAMES[i]] = new ParameterDescription(CONNECTIONPARAMETERNAMES[i], ParameterDescription::INTEGER32) ;
  }

  (*parameterNames)[CONNECTIONPARAMETERNAMES[ENABLED]]->setType(ParameterDescription::STRING) ;
  (*parameterNames)[CONNECTIONPARAMETERNAMES[ENABLED]]->setValue("T") ;

  return parameterNames ;
}

/** \brief Delete a list of parameter name and its content
 */
void ConnectionDescription::deleteParameterNames(parameterDescriptionNameType *parameterNames){
  
  deviceDescription::deleteParameterNames ( parameterNames ) ;
}

/** Parameter's name
 */
const char *ConnectionDescription::CONNECTIONPARAMETERNAMES[ConnectionDescription::ENABLED+1] = {"fedCrateSlot","fedSlot","fedId","fedChannel","fecHardId","fecCrateSlot","fecSlot","fecRing","ccuAddress","ccuChannel","apvAddress","dcuHardId","enabled"} ;

