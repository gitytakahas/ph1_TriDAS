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
#include <CCUDescription.h>

/** Set all the channel to not initialise
 * \param keytype - the key
 * \param arrangement - order in the ring
 * \param ccu25 - true if the CCU is a CCU 25
 * \brief Constructor that create a CCU 
 */
CCUDescription::CCUDescription ( tscType16 crateId, keyType key, int ccuOrder , bool ccu25, bool enabled) {

  isACcu25_  = ccu25 ;
  key_       = key ;
  enabled_   = enabled ;
  crateId_ = crateId ;
  ccuOrder_  = ccuOrder;

  for (tscType16 channel = 0 ; channel < MAXI2CCHANNELS ; channel ++) {

    i2cBitForceAck_[channel] = false ;
    i2cCRAValue_[channel] = 0; 
    channelI2CEnable_[channel] = false ;
  }


  if (isACcu25_) {
    for (tscType16 channel = 0 ; channel < NBCCU25PIACHANNELS ; channel ++)
      channelPiaEnable_[channel] = false ;
  } 
  else {
    for (tscType16 channel = 0 ; channel < NBOLDCCUPIACHANNELS ; channel ++)
      channelPiaEnable_[channel] = false ;
  }

  channelMemoryEnable_  = false ;
  channelJTAGEnable_    = false ;
  channelTriggerEnable_ = false ;

  for (tscType16 i = 0 ; i < 4 ; i ++) ccuAlarmsEnable_[i] = false; 

  //WB:
  usage_ = ccuUsed ; 
}

/** Set all the channel to not initialise
 * \param address - CCU address
 * \param ccu25 - true if the CCU is a CCU 25
 * \brief Constructor that create a CCU 
 */
CCUDescription::CCUDescription ( keyType key, bool ccu25 ): crateId_(0) {

  isACcu25_ = ccu25 ;

  key_       = key ;
  enabled_   = true ;
  ccuOrder_  = 0 ;

  for (tscType16 channel = 0 ; channel < MAXI2CCHANNELS ; channel ++) {

    i2cBitForceAck_[channel] = false ;
    i2cCRAValue_[channel] = 0; 
    channelI2CEnable_[channel] = false ;
  }


  if (isACcu25_) {
    for (tscType16 channel = 0 ; channel < NBCCU25PIACHANNELS ; channel ++)
      channelPiaEnable_[channel] = false ;
  } 
  else {
    for (tscType16 channel = 0 ; channel < NBOLDCCUPIACHANNELS ; channel ++)
      channelPiaEnable_[channel] = false ;
  }

  channelMemoryEnable_  = false ;
  channelJTAGEnable_    = false ;
  channelTriggerEnable_ = false ;

  for (tscType16 i = 0 ; i < 4 ; i ++) ccuAlarmsEnable_[i] = false; 

  //WB:
  usage_ = ccuUsed ; 
}

/** \brief Create a CCU description from XML buffer
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 * \warning an exception is raised if one the parameter is not correct
 */
CCUDescription::CCUDescription ( parameterDescriptionNameType parameterNames, bool isACcu25 ): crateId_(0) {

  isACcu25_ = isACcu25 ;

  setFecHardwareId ((std::string)parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID]]->getValue(),
		    *((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID]]->getValueConverted())) ;
  //setCrateId (*((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID]]->getValueConverted())) ;

  key_ = buildCompleteKey(*((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT]]->getValueConverted()),
			  *((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT]]->getValueConverted()),
			  *((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS]]->getValueConverted()),
			  0, 0) ;

  // enabled setting:
  // "0" can be used in place of "F" (false)
  // "1" can be used in place of "T" (true)
  std::string enabled = (std::string)parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED]]->getValue() ;
  if (enabled == "0" ) enabled = "F";
  if (enabled == "1") enabled ="T";

  bool tempEnable=true;
  if ( enabled == "F" ) tempEnable=false;
  setEnabled(tempEnable); 

  ccuOrder_ = 0 ;
  ccuOrder_ = *((tscType16 *) parameterNames[CCUPARAMETERNAMES[ARRANGEMENT]]->getValueConverted()) ;

  // RAZ of the channels
  for (tscType16 channel = 0 ; channel < MAXI2CCHANNELS ; channel ++) {

    i2cBitForceAck_[channel] = false ;
    i2cCRAValue_[channel] = 0; 
    channelI2CEnable_[channel] = false ;
  }

  if (isACcu25_) {
    for (tscType16 channel = 0 ; channel < NBCCU25PIACHANNELS ; channel ++)
      channelPiaEnable_[channel] = false ;
  } 
  else {
    for (tscType16 channel = 0 ; channel < NBOLDCCUPIACHANNELS ; channel ++)
      channelPiaEnable_[channel] = false ;
  }

  channelMemoryEnable_  = false ;
  channelJTAGEnable_    = false ;
  channelTriggerEnable_ = false ;

  for (tscType16 i = 0 ; i < 4 ; i ++) ccuAlarmsEnable_[i] = false; 

  //WB:
  usage_ = ccuUsed ; 
}

/** \brief Clone a CCU description
 * \return the CCUDescription cloned
 */
CCUDescription *CCUDescription::clone ( ) {
 
  CCUDescription* myCCUDescription = new CCUDescription ( *this ) ;

  return myCCUDescription;
}

/** \brief Return the order of the CCU in the ring
 * \return CCU order
 */
unsigned int CCUDescription::getOrder ( ) {
    
  return ccuOrder_ ;
}

/** \brief Return the current enabled parameter 
 * \return enabled
 */
bool CCUDescription::getEnabled ( ) {
  return (enabled_) ;
}

/** Return the address of the ccu
 * \return CCU address
 */
keyType CCUDescription::getKey ( ) {

  return (key_) ;
}

/** \brief return the crate slot
 * \return the crate slot
 */
// tscType16 CCUDescription::getCrateSlot ( ) {
    
//   std::cerr << "---------------------------> " << __PRETTY_FUNCTION__ << " DEPRECATED> use getCrateId" << std::endl ;
//   return (crateId_) ;
// }

/** \brief return the crate ID
 * \return the crate ID
 */
tscType16 CCUDescription::getCrateId ( ) {
    
  return (crateId_) ;
}


/** \brief set the crate slot
 * \param crateId - slot of the crate to be set
 */
// void CCUDescription::setCrateSlot ( tscType16 crateId ) {

//   std::cerr << "---------------------------> " << __PRETTY_FUNCTION__ << " DEPRECATED> use getCrateId" << std::endl ;
//   crateId_ = crateId ;
// }

/** \brief set the crate ID
 * \param crateId - ID of the crate to be set
 */
void CCUDescription::setCrateId ( tscType16 crateId ) {

   crateId_ = crateId ;
}

/** \brief Set the address of the ccu
 * \param address - start from 0x1 to 0x7f
 */
void CCUDescription::setKey ( keyType key ) {

  key_ = key ;
}

/** \brief Return the bit force ack for a channel
 * \param channel - CCU channel number
 * \return boolean to know if the force ack is enable or not
 * \warning this function work for old CCU and CCU 25
 */
bool CCUDescription::getBitForceAck ( tscType8 channel ) {

  if (isACcu25_)
    return (i2cBitForceAck_[channel-0x10]) ;
  else
    return (i2cBitForceAck_[channel-0x1]) ;
}

/** \brief return if the CCU is a CCU25 or no
 */
bool CCUDescription::isCcu25 ( ) {

  return (isACcu25_) ;
}

/** \brief in case of reset, put all values in the correct state
 */
void CCUDescription::reset ( ) {

  enabled_ = true;
  for (tscType16 i = 0 ; i < MAXI2CCHANNELS ; i ++) {

    i2cCRAValue_[i] = 0 ;
    i2cBitForceAck_[i] = false ;
    channelI2CEnable_[i] = false ;

  }
  for (tscType16 i = 0 ; i < MAXCCUPIACHANNELS ; i ++) {

    piaGCRValue_[i] = 0 ;
    channelPiaEnable_[i] = false ;
  }
  channelJTAGEnable_ = channelTriggerEnable_ = false ;
  for (tscType16 i = 0 ; i < 4 ; i ++) ccuAlarmsEnable_[i] = false ;
}     

/** \brief Enable or disable the Force acknowledge for a channel
 * \param channel - CCU channel number
 * \param enable - enable/disable
 */
void CCUDescription::setBitForceAck ( tscType8 channel, bool enable ) {

  if (isACcu25_)
    i2cBitForceAck_[channel-0x10] = enable ;
  else
    i2cBitForceAck_[channel-0x1] = enable ;
}

/** \brief Set the value of Enabled 
 * \param enabled (boolean)
 */
void CCUDescription::setEnabled ( bool enabled ) {
  enabled_ = enabled ;
}

/** \brief Store if a CCU alarms is enabled
 * \param enable1 - CCU alarm 1
 * \param enable2 - CCU alarm 2
 * \param enable3 - CCU alarm 3
 * \param enable4 - CCU alarm 4
 */
void CCUDescription::setCcuAlarmsEnable ( bool enable1, bool enable2, bool enable3, bool enable4 ) {

  ccuAlarmsEnable_[0] = enable1 ;
  ccuAlarmsEnable_[1] = enable2 ;
  ccuAlarmsEnable_[2] = enable3 ;
  ccuAlarmsEnable_[3] = enable4 ;
}

/** \brief Check if a CCU alarm is enable
 * \param ccuAlarm - CCU alarm number
 * \return boolean - true if the CCU alarm is enable
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED: wrong CCU alarm
 * </ul>
 */
bool CCUDescription::isCcuAlarmEnable ( tscType16 ccuAlarm ) throw (FecExceptionHandler) {

  if (ccuAlarm > 3) {
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FECPARAMETERNOTMANAGED, 
				       "CCU alarm is greater than 3", 
				       FATALERRORCODE, 
				       key_, 
				       "CCU alarm", ccuAlarm) ;
  }

  return (ccuAlarmsEnable_[ccuAlarm]) ;
}

/** \brief Store that a channel is enable or disable
 * \param channel - CCU channel number
 * \param enable - enable or disable
 */
void CCUDescription::setChannelEnable ( tscType8 channel, bool enable ) {

  if (isACcu25_) {
    if ( (channel >= 0x10) && (channel <= 0x1F) )
      channelI2CEnable_[channel-0x10] = enable ;
    else
      if ( (channel >= 0x30) && (channel <= 0x33) )
	channelPiaEnable_[channel-0x30] = enable ;
      else
	if (channel == 0x40)
	  channelMemoryEnable_ = enable ;
	else
	  if (channel == 0x50)
	    channelTriggerEnable_ = enable ;
	  else
	    if (channel == 0x60)
	      channelJTAGEnable_ = enable ;
  }
  else {
    if ( (channel >= 0x01) && (channel <= 0x10) )
      channelI2CEnable_[channel-1] = enable ;
    else
      if (channel == 0x12)
	channelPiaEnable_[0] = enable ;
      else
	if (channel == 0x11)
	  channelMemoryEnable_ = enable ;
	else
	  if (channel == 0x14)
	    channelTriggerEnable_ = enable ;
  }
}

/** \brief return true if a channel is enable, false if not
 * \param channel - CCU channel number
 * \return - return true if a channel is enable, false if not
 */
bool CCUDescription::getChannelEnable ( tscType8 channel ) {

  bool value = false ;
    
  if (isACcu25_) {
    if ( (channel >= 0x10) && (channel <= 0x1F) )
      value = channelI2CEnable_[channel-0x10] ;
    else
      if ( (channel >= 0x30) && (channel <= 0x33) )
	value = channelPiaEnable_[channel-0x30] ;
      else
	if (channel == 0x40)
	  value = channelMemoryEnable_ ;
	else
	  if (channel == 0x50)
	    value = channelTriggerEnable_ ;
	  else
	    if (channel == 0x60)
	      value = channelJTAGEnable_ ;
  }
  else {

    if ( (channel >= 0x01) && (channel <= 0x10) )
      value = channelI2CEnable_[channel-1] ;
    else
      if (channel == 0x12)
	value = channelPiaEnable_[0] ;
      else
	if (channel == 0x11)
	  value = channelMemoryEnable_ ;
	else
	  if (channel == 0x14)
	    value = channelTriggerEnable_ ;      
  }

  return (value) ;
}

/** \brief Set the control register A of an I2C channel
 * \param channel - CCU channel number 
 * \param value - value set in the CRA
 */
void CCUDescription::seti2cChannelCRA ( tscType8 channel, tscType8 value ) {

  if (isACcu25_)
    i2cCRAValue_[channel-0x10] = value ;
  else
    i2cCRAValue_[channel-0x1] = value ;

  if (isChannelForceAck(value))
    setBitForceAck (channel, true) ;
}
 
/** Make a logical operation on the CRA store in the array
 * \brief Set the control register A of an I2C channel
 * \param channel - CCU channel number 
 * \param value - value set in the CRA
 * \param op - operation for the read modify write
 */
void CCUDescription::seti2cChannelCRA ( tscType8 channel, tscType8 value, logicalOperationType op ) {

  tscType8 chan = channel - 0x10 ;

  if (! isACcu25_)
    chan = channel - 0x1 ;

  switch (op) {
  case CMD_OR:  i2cCRAValue_[chan] = i2cCRAValue_[chan] | value    ; break ;
  case CMD_XOR: i2cCRAValue_[chan] = i2cCRAValue_[chan] & (~value) ; break ;
  case CMD_AND: i2cCRAValue_[chan] = i2cCRAValue_[chan] & value    ; break ;
  case CMD_EQUAL: break ;
  }

  if (isChannelForceAck(value)) setBitForceAck (channel, true) ;
}

/** \brief Return the value of the control register A of an I2C channel
 * \param channel - CCU channel number 
 * \return Value of the control register A
 */
tscType8 CCUDescription::geti2cChannelCRA ( tscType8 channel ) {


  if (isACcu25_) channel -= 0x10 ;
  else channel -= 0x1 ;

  return (i2cCRAValue_[channel-0x10]) ;
}

/** \brief Set the value of the General control register of a PIA channel
 * \param channel - PIA channel. The number of PIA channels depends of the CCU type
 * \param value - value set in the GCR
 */
void CCUDescription::setPiaChannelGCR ( tscType8 channel, tscType8 value ) {

  if (isACcu25_) channel -= 0x30 ;
  else channel -= 0x12 ;

  piaGCRValue_[channel] = value ;
}
  
/** Make a logical operation on the GCR store in the array
 * \brief Set the value of the General control register of a PIA channel
 * \param channel - PIA channel. The number of PIA channels depends of the CCU type
 * \param value - value set in the GCR
 * \param op - operation to be applied for the read modify write
 */
void CCUDescription::setPiaChannelGCR ( tscType8 channel, tscType8 value, logicalOperationType op ) {

  if (isACcu25_) channel -= 0x30 ;
  else channel -= 0x12 ;

  switch (op) {
  case CMD_OR:  piaGCRValue_[channel] = piaGCRValue_[channel] | value    ; break ;
  case CMD_XOR: piaGCRValue_[channel] = piaGCRValue_[channel] & (~value) ; break ;
  case CMD_AND: piaGCRValue_[channel] = piaGCRValue_[channel] & value    ; break ;
  case CMD_EQUAL: break ;
  }
}

/** \brief Return the value of the General control register of the PIA channel
 * \param channel - PIA channel. The number of PIA channels depends of the CCU type
 * \return Value of the general control register
 */
tscType8 CCUDescription::getPiaChannelGCR ( tscType8 channel ) {

  if (isACcu25_) channel -= 0x30 ;
  else channel -= 0x12 ;

  return (piaGCRValue_[channel-0x30]) ;
}

/** \brief Set usage state of this ccu
 * \author W. Bialas
 */
void CCUDescription::setUsage(ccuUsageType usage) {
  usage_ = usage ; 
} 
  
/** \brief Get usage state of this ccu
 * \author W. Bialas
 */
CCUDescription::ccuUsageType CCUDescription::getUsage() {
  return usage_ ; 
} 

// ***********************************************************************************************
// FEC hardware ID
// ***********************************************************************************************

/** \brief Set the FEC hardware identification number
 */
void CCUDescription::setFecHardwareId ( std::string fecHardwareId, tscType16 crateId ) {

  strncpy (fecHardwareId_, fecHardwareId.c_str(), 100) ;
  crateId_ = crateId ;
  //fecHardwareId_ = fecHardwareId ;
}

/** \brief return the FEC hardware identification number
 */
std::string CCUDescription::getFecHardwareId ( ) {

  return (fecHardwareId_) ;
}

// ***********************************************************************************************
// Display
// ***********************************************************************************************

/** Display the placement of the CCU in the ring
 */
void CCUDescription::displayRedundancy ( ) {
    
  std::cout << "CCU 0x" << std::hex << getCcuKey(key_) << std::endl ;
  std::cout << "\t" << "Order in the ring " << std::dec << ccuOrder_ << std::endl ;
}

// ***********************************************************************************************
// Static method
// ***********************************************************************************************

/** Use to sort a vector of CCUs 
 * \param r1 - CCUDescription
 * \param r2 - CCUDescription
 */
bool CCUDescription::sortByOrder ( CCUDescription *r1, CCUDescription *r2 ) {
  if (r1->getOrder() < r2->getOrder()) return true ;
  else return false ;
}


/** Display the redundancy of a vector of CCU descriptions in the correct order
 * \param vCcuVector - vector of CCU descriptions
 */
void CCUDescription::displayRedundancy ( std::vector<CCUDescription *> vCcuVector ) {

}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *CCUDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ; // TODO: valgrind 792 (20 direct, 772 indirect) bytes in 1 blocks are definitely lost in loss record 106 of 154

  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID], ParameterDescription::STRING) ;
  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID], ParameterDescription::INTEGER16) ;
  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT], ParameterDescription::INTEGER16) ;
  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT], ParameterDescription::INTEGER16) ;
  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS], ParameterDescription::INTEGER16) ;

  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED], ParameterDescription::STRING) ;
  (*parameterNames)[CCUPARAMETERNAMES[ARRANGEMENT]] = new ParameterDescription(CCUPARAMETERNAMES[ARRANGEMENT], ParameterDescription::INTEGER16) ;

  return parameterNames ;
}

/** \brief Delete a list of parameter name but only its content
 * \warning the parameter parameterNames is deleted by this method
 */
void CCUDescription::deleteParameterNames(parameterDescriptionNameType *parameterNames){

  deviceDescription::deleteParameterNames ( parameterNames ) ;
}

#ifdef _OLD_CCU_INPUT_OUTPUT_

/** \brief Set the parameter usage in the CCUDescription
 * \param list of CCUDescription
 * \warning this method include the dummy CCU on ring B so last CCU output A -> FEC input A
 *          and last CCU output B -> Dummy CCU input A -> Dummy CCU output A -> FEC input B
 *          and Before last CCU output B -> Dummy CCU input B -> Dummy CCU output A -> FEC input B
 *          and Dummy CCU output B not connected
 */

void CCUDescription::setCcuTrackerUsage ( std::list<CCUDescription *> &listCcu ) throw (FecExceptionHandler) {

  // Array with the order of the CCU
  unsigned int ccuNumber = 0 ;
  CCUDescription *ccuArray[MAXCCU] = {NULL} ;

  // Classify the CCU
  for (std::list<CCUDescription *>::iterator itCcu = listCcu.begin() ; itCcu != listCcu.end() ; itCcu ++) {
    ccuArray[(*itCcu)->getOrder()] = (*itCcu) ;
    ccuNumber ++ ;
  }

  // Verify that all CCUs are in the list
  if (ccuNumber) 
    for (unsigned int i = 0 ; i < ccuNumber ; i ++) {
	
      if (ccuArray[i] == NULL) {
	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION, 
						"Missing one CCU in the order", 
						ERRORCODE, 
						buildFecRingKey(getFecKey(key_),getRingKey(key_))) ;
      }
    }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION, 
					    "no CCU in the ring",
					    ERRORCODE, 
					    buildFecRingKey(getFecKey(key_),getRingKey(key_))) ;
  }

  // Now set each CCU usage
  // FEC X-B CCU 2 B-X => the first CCU is out of the ring
  unsigned i = 1 ;
  if (ccuArray[1]->getInput()) { // so CCU 2 input B
    ccuArray[0]->setUsage(ccuSkipped) ;
    ccuArray[1]->setUsage(ccuUsed) ;
    i ++ ; // CCU is in the ring 
  }
  else ccuArray[0]->setUsage(ccuUsed) ;

  for ( ; i < (ccuNumber-1) ; i ++) { 

    if ( (ccuArray[i-1]->getOutput() && !ccuArray[i+1]->getInput()) || // CCU i-1 output B and CCU i+1 input A
	 (!ccuArray[i-1]->getOutput() && ccuArray[i+1]->getInput()) ) { // CCU i-1 output A and CCU i+1 input B

      std::stringstream str ;
      str << "Bad configuration between two CCUs: CCU 0x" << std::hex << getCcuKey(ccuArray[i-1]->getKey()) << " is on output " << ccuArray[i-1]->getOutput() 
	  << " and CCU 0x" << std::hex << getCcuKey(ccuArray[i+1]->getKey()) << " is on " << ccuArray[i+1]->getInput() << std::dec ;
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION (TSCFEC_INVALIDOPERATION, str.str(), ERRORCODE, buildFecRingKey(getFecKey(key_),getRingKey(key_))) ;
    }

    if ( ccuArray[i-1]->getOutput() && ccuArray[i+1]->getInput() )     // CCU i-1 output B and CCU i+1 input B
      ccuArray[i]->setUsage(ccuSkipped) ;
    else                                                               // CCU i-1 output A and CCU i+1 input A
      ccuArray[i]->setUsage(ccuUsed) ;
  }

  // Dummy CCU so ccuArray[ccuNumber-1]
  if (ccuArray[ccuNumber-2]->getOutput()) ccuArray[ccuNumber-1]->setUsage(ccuUsed) ; // Last CCU -> Dummy CCU
  else ccuArray[ccuNumber-1]->setUsage(ccuSkipped) ;
}


/** \brief Set the parameter usage in the CCUDescription
 * \param list of CCUDescription
 * \warning this method include the dummy CCU on ring A
 */

void CCUDescription::setCcuUsage ( std::list<CCUDescription *> &listCcu ) throw (FecExceptionHandler) {

  // Array with the order of the CCU
  unsigned int ccuNumber = 0 ;
  CCUDescription *ccuArray[MAXCCU] = {NULL} ;

  // Classify the CCU
  for (std::list<CCUDescription *>::iterator itCcu = listCcu.begin() ; itCcu != listCcu.end() ; itCcu ++) {
    ccuArray[(*itCcu)->getOrder()] = (*itCcu) ;
    ccuNumber ++ ;
  }

  // Verify that all CCUs are in the list
  if (ccuNumber) 
    for (unsigned int i = 0 ; i < ccuNumber ; i ++) {
	
      if (ccuArray[i] == NULL) {
	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION, 
						"Missing one CCU in the order", 
						ERRORCODE, 
						buildFecRingKey(getFecKey(key_),getRingKey(key_))) ;
      }
    }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION, 
					    "no CCU in the ring",
					    ERRORCODE, 
					    buildFecRingKey(getFecKey(key_),getRingKey(key_))) ;
  }

  // Now set each CCU usage
  // FEC X-B CCU 2 B-X => the first CCU is out of the ring
  unsigned i = 1 ;
  if (ccuArray[1]->getInput()) { // so CCU 2 input B
    ccuArray[0]->setUsage(ccuSkipped) ;
    ccuArray[1]->setUsage(ccuUsed) ;
    i ++ ; // CCU is in the ring 
  }

  for ( ; i < ccuNumber ; i ++) { 

    if ( (ccuArray[i-1]->getOutput() && !ccuArray[i+1]->getInput()) || // CCU i-1 output B and CCU i+1 input A
	 (!ccuArray[i-1]->getOutput() && ccuArray[i+1]->getInput()) )  { // CCU i-1 output A and CCU i+1 input B

      std::stringstream str ;
      str << "Bad configuration between two CCUs: CCU 0x" << std::hex << getCcuKey(ccuArray[i-1]->getKey()) << " is on output " << ccuArray[i-1]->getOutput() 
	  << " and CCU 0x" << std::hex << getCcuKey(ccuArray[i+1]->getKey()) << " is on " << ccuArray[i+1]->getInput() << std::dec ;
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION (TSCFEC_INVALIDOPERATION, str.str(), ERRORCODE, buildFecRingKey(getFecKey(key_),getRingKey(key_))) ;
    }

    if ( ccuArray[i-1]->getOutput() && ccuArray[i+1]->getInput() )     // CCU i-1 output B and CCU i+1 input B
      ccuArray[i]->setUsage(ccuSkipped) ;
    else                                                               // CCU i-1 output A and CCU i+1 input A
      ccuArray[i]->setUsage(ccuUsed) ;
  }
}
#endif

/** Enumeration name for the XML parsing
 */
const char *CCUDescription::CCUPARAMETERNAMES[] = {"arrangement"} ;

/** Number of parameters to be parsed
 */
const unsigned int CCUDescription::CCUNUMBEROFPARAMETERS = 7;
