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
#include <iostream>

#include <time.h>    // time
#include <unistd.h>  // usleep

#include "dcuAccess.h"

/** Constructor to store the access to the DCU (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */

dcuAccess::dcuAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress,
		      std::string dcuType ):

  deviceAccess (DCU,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress ) {

  // All channels available
  for (int i = 0 ; i < MAXDCUCHANNELS ; i ++) channelEnabled_[i] = true ;

  // Set the DCU type
  setDcuType ( dcuType ) ;

  // Reset the DCU
  reset () ;
}

/** Constructor to store the access to the APV (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
dcuAccess::dcuAccess (FecAccess *fec, 
                      keyType key,
		      std::string dcuType ) :

  deviceAccess (DCU,
                fec,
                key) {

  // All channels available
  for (int i = 0 ; i < MAXDCUCHANNELS ; i ++) channelEnabled_[i] = true ;

  // Set the DCU type
  setDcuType ( dcuType ) ;

  // Reset the DCU
  reset () ;
}

/** Set the DCU type of the DCU
 * \param dcuType - DCU type
 */
void dcuAccess::setDcuType ( std::string dcuType ) { strncpy (dcuType_, dcuType.c_str(), 4) ; }

/** Return the DCU type
 * \return DCU type
 */
std::string dcuAccess::getDcuType ( ) { return ( std::string(dcuType_) ) ; }

/**
 * This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */

dcuAccess::~dcuAccess ( ) {

  // ~deviceAccess ( ) ;
}

/** Set the DCU channel enable
 * \param channel - list of channel to be enabled
 * \param nbChannels - number of channels to be declared as enabled
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
void dcuAccess::setChannelEnabled ( bool *channels, unsigned int nbChannels ) {

  if (nbChannels > 8) RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,"invalid number for a DCU channel (0 .. 7)", ERRORCODE) ;

  // Set the channels available
  for (unsigned int i = 0 ; i < nbChannels ; i ++) channelEnabled_[i] = channels[i] ;
}

/** \return true if a channel is enabled or false if disabled
 */
bool dcuAccess::getChannelEnabled ( tscType8 channel ) {

  return (channelEnabled_[channel]) ;
}

/** Build a description of an DCU and return it
 * \return the DCU description values
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_REGISTERACCESS: if all DCU channels cannot be read
 * </ul>
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
dcuDescription *dcuAccess::getValues ( ) throw (FecExceptionHandler) {

  // error counter
  errorDigitisationCounter_ = 0 ;

  long timestamp = time(NULL) ;
  tscType32 hardId = getDcuHardId( ) ;

  // Channel values
  tscType16 chan[MAXDCUCHANNELS] = {0} ;

  bool tregSet = true ;
  for (int i = 0 ; i < MAXDCUCHANNELS ; i ++) {
    if (channelEnabled_[i]) chan[i] = getDcuChannel (i, tregSet) ;
    tregSet=false ;
  }

  dcuDescription *dcuValues =  new dcuDescription (accessKey_,
                                                   timestamp, hardId,
                                                   chan[0], chan[1], 
                                                   chan[2], chan[3], 
                                                   chan[4], chan[5], 
                                                   chan[6], chan[7],
						   dcuType_ ) ; 
  // Problem in the DCU ?
  if (errorDigitisationCounter_ == 8) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_REGISTERACCESS, 
					    "Cannot read channels of the DCUs",
					    ERRORCODE,
					    accessKey_ ) ;
  } 
//    else if (errorDigitisationCounter_) {
//      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_REGISTERACCESS, 
//   					  "DCU channel problem, error in digitisation for channels",
//					  ERRORCODE,
//					  accessKey_ ) ;
//    }

  // set the parameters to follow the description position
  dcuValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

  return (dcuValues) ;
}


/**
 * Get the hardware id in the DCU
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType32 dcuAccess::getDcuHardId ( ) throw (FecExceptionHandler) {

  // CHIPADD = 2^16 * CHIPADDH + 2^8 * CHIPADDM + CHIPADDL
  tscType32 chipaddh = accessToFec_->readOffset(accessKey_, CHIPADDH) ;
  tscType32 chipaddm = accessToFec_->readOffset(accessKey_, CHIPADDM) ;
  tscType32 chipaddl = accessToFec_->readOffset(accessKey_, CHIPADDL) ;

  tscType32 chipadd = chipaddl | (chipaddm << 8) | (chipaddh << 16) ;

  return (chipadd) ;
}

/** Get DCU channel
 * \param channel - channel to be red
 * \return return the value read
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_REGISTERACCESS: warning, error during the digitisation
 * </ul>
 * \warning Note from Guido Magazzu
 * To read channel 0 (the result of one acquisition on channel0) you
 * should read SHREG 	and LREG. The result is:
 * result[11:0] = SHREG[3:0],LREG[7:0]
 * In principle you should check if SHREG[7] = 1 (the result is available)
 * before reading the two registers. Reading CREG is useful for debugging
 * (you check what you have previously written), but not so important in the
 * final application.
 * \warning this method try to read several time the same channels (timeout)
 * and if the channel cannot be read, the value 0 is returned
 */
tscType16 dcuAccess::getDcuChannel ( tscType8 channel, bool setTreg ) throw (FecExceptionHandler) {

  //#define DEBUGMSGERROR
  volatile tscType16 shreg ;
  volatile tscType16 lreg  ;
  volatile tscType16 value ;

  int timeout = 0 ;

  // set the TREG
  if ( setTreg ) accessToFec_->writeOffset(accessKey_, TREG, 0x10);  

  // Start the digitization
  accessToFec_->writeOffset (accessKey_,CREG, 0x98 + channel);

  // Wait for the end of the i2c transaction
  // usleep (WAITTIME4DIGITOK) ;
  // Wait for the end of the ADC acquisition
  usleep (WAITTIME4DIGITOK) ;

  do {
    shreg = (accessToFec_->readOffset(accessKey_, SHREG));

    value = 0 ;
    timeout ++ ;

#ifdef DEBUGMSGERROR
    if (!isDigitisationOk(shreg))
      std::cerr << "The Digitisation is not ok for channel " << std::dec << (int)channel 
		<< " (timeout " << timeout << "): shreg = 0x" << std::hex << shreg << std::endl ;
    else
      std::cout << "Digisation ok shreg for channel " << std::dec << (int)channel 
		<< " shreg = 0x"  << std::hex << shreg << std::endl ;
#endif
    
  } while (!isDigitisationOk(shreg) && (timeout < DCUTIMEOUT)) ;

#ifdef DEBUGMSGERROR
  tscType16 creg = (accessToFec_->readOffset(accessKey_, CREG));
  lreg = (accessToFec_->readOffset(accessKey_, LREG));
  std::cout << "DCU access to the channel " << std::dec << (int)channel
	    << ": creg = 0x" << std::hex << creg
	    << "  lreg = 0x" << std::hex << lreg
	    << std::endl ;
#endif

  if (isDigitisationOk(shreg)) {

    lreg = (accessToFec_->readOffset(accessKey_, LREG));
    value = getValueShreg ( shreg ) + getValueLreg ( lreg ) ;
#ifdef DEBUGMSGERROR
    std::cout << "Channel " << std::dec << (int)channel << " = 0x"  << std::hex << value << std::endl ;
#endif
  }
  else {
    value = 0 ;
    errorDigitisationCounter_ ++ ;

    // ???????????????????????????????????????????????????????????
    //#ifdef DEBUGMSGERROR
    char dcuChar[100] ;
    decodeKey(dcuChar,accessKey_) ;
    std::cerr << "DCU " << dcuChar << " (" << getDcuHardId( ) << "): Warning channel " << std::dec << (int)channel 
	      << ": error in digitisation, shreg = 0x" << std::hex << (int)accessToFec_->readOffset(accessKey_, SHREG)
	      << ": error in digitisation, lreg  = 0x" << std::hex << (int)accessToFec_->readOffset(accessKey_, LREG)
	      << ": error in digitisation, creg  = 0x" << std::hex << (int)accessToFec_->readOffset(accessKey_, CREG)
	      << ": error in digitisation, treg  = 0x" << std::hex << (int)accessToFec_->readOffset(accessKey_, TREG)
              << std::endl ;
    //#endif
  }

  return value;
}

/** Reset
 * \param Values to be set
 * \exception FecExceptionHandler
 */
void dcuAccess::reset () {

#ifdef DEBUGMSGERROR
  std::cout << std::endl << "  Value of the registers before reset... CREG:0x" 
	    << std::hex << (int)accessToFec_->readOffset(accessKey_,CREG)
	    << " SHREG:0x"  << (int)accessToFec_->readOffset(accessKey_,SHREG)
	    << " LREG:0x"   << (int)accessToFec_->readOffset(accessKey_,LREG)
	    << " TREG:0x"   << (int)accessToFec_->readOffset(accessKey_,TREG)
	    << std::endl ;
#endif

  accessToFec_->writeOffset(accessKey_, CREG, 0x40);
  accessToFec_->writeOffset(accessKey_, TREG, 0x10);

  // Wait for the end of the end of the i2c transaction
  usleep (WAITTIME4DIGITOK) ;

#ifdef DEBUGMSGERROR
  std::cout << std::endl << "  Value of the registers before reset... CREG:0x" 
	    << std::hex << (int)accessToFec_->readOffset(accessKey_,CREG)
	    << " SHREG:0x"  << (int)accessToFec_->readOffset(accessKey_,SHREG)
	    << " LREG:0x"   << (int)accessToFec_->readOffset(accessKey_,LREG)
	    << " TREG:0x"   << (int)accessToFec_->readOffset(accessKey_,TREG)
	    << std::endl ;
#endif


}

/** Set DCU control register (CREG)
 * \param value - value to be written
 * \exception FecExceptionHandler:
 */
void dcuAccess::setDcuCREG( tscType8 value ) throw (FecExceptionHandler) {

  accessToFec_->writeOffset(accessKey_, CREG, value);
}

/** get the DCU control register (CREG)
 * \return the value read
 * \exception FecExceptionHandler:
 */
tscType8 dcuAccess::getDcuCREG() throw (FecExceptionHandler) {

  tscType8 readvalue=accessToFec_->readOffset(accessKey_,CREG);
  return readvalue;
}

/** Set DCU test register (TREG)
 * \param value - value to be written
 * \exception FecExceptionHandler:
 */
void dcuAccess::setDcuTREG( tscType8 value ) throw (FecExceptionHandler) {

  accessToFec_->writeOffset(accessKey_, TREG, value);
}

/** get the DCU test register (TREG)
 * \return the value read
 * \exception FecExceptionHandler:
 */
tscType8 dcuAccess::getDcuTREG() throw (FecExceptionHandler) {

  tscType8 readvalue=accessToFec_->readOffset(accessKey_,TREG);
  return readvalue;
}

/** Set DCU test register 
 * \param value - value to be written
 * \exception FecExceptionHandler:
 * Thanks to Wim Beaumont
 */
void dcuAccess::setDcuTestRegister( tscType8 value ) throw (FecExceptionHandler) {

  accessToFec_->writeOffset(accessKey_, TREG, value);
  //usleep (WAITTIME4DIGITOK) ;
}

/** get the DCU test register
 * \return the value read
 * \exception FecExceptionHandler:
 * Thanks to Wim Beaumont
 */
tscType8 dcuAccess::getDcuTestRegister() throw (FecExceptionHandler) {

  tscType8 readvalue=accessToFec_->readOffset(accessKey_,TREG);
  //usleep (WAITTIME4DIGITOK) ;
  return readvalue;
}

/** Set DCU auxilairy register (AREG)
 * \param value - value to be written
 * \exception FecExceptionHandler:
 */
void dcuAccess::setDcuAREG( tscType8 value ) throw (FecExceptionHandler) {

  accessToFec_->writeOffset(accessKey_, AREG, value);
}

/** get the DCU auxiliary register (AREG)
 * \return the value read
 * \exception FecExceptionHandler:
 */
tscType8 dcuAccess::getDcuAREG() throw (FecExceptionHandler) {

  tscType8 readvalue=accessToFec_->readOffset(accessKey_,AREG);
  return readvalue;
}

/** get the DCU status and data high register (SHREG)
 * \return the value read
 * \exception FecExceptionHandler:
 */
tscType8 dcuAccess::getDcuSHREG() throw (FecExceptionHandler) {

  tscType8 readvalue=accessToFec_->readOffset(accessKey_,SHREG);
  return readvalue;
}

/** Set DCU data low register (LREG)
 * \param value - value to be written
 * \exception FecExceptionHandler:
 */
void dcuAccess::setDcuLREG( tscType8 value ) throw (FecExceptionHandler) {

  accessToFec_->writeOffset(accessKey_, LREG, value);
}

/** get the DCU data low register (LREG)
 * \return the value read
 * \exception FecExceptionHandler:
 */
tscType8 dcuAccess::getDcuLREG() throw (FecExceptionHandler) {

  tscType8 readvalue=accessToFec_->readOffset(accessKey_,LREG);
  return readvalue;
}

/** \return true if as SEU is detected ( bit 6  of Satais & DataReg = 1 )
 * Thanks to Wim Beaumont
 */
bool dcuAccess::getSeuStatus() throw (FecExceptionHandler) {

  tscType16 readvalue=accessToFec_->readOffset(accessKey_,SHREG); 

  return (bool) (readvalue & DCUSEUDETECTED); // bit 6 
}


/** This static method read out several DCU at the same time
 * \param fecAccess - hardware access
 * \param dcuSet - all the DCU to be readout
 * \param dcuVector - list of the readout DCU (suppose to be empty at the beginning). The dcuDescription created must be delete by the remote method.
 * \param dcuHardIdOnly - readout only the DCU hard ID
 * \return number of errors
 * \warning if DCU hard id is not read (hardware problem) then the value 0xFFFFFFFF is set
 * \warning if a problem occurs in one channel then 0 is set in the corresponding channel
 */
unsigned int dcuAccess::getDcuValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, dcuAccess *> &dcuSet, deviceVector &dcuVector,
						     std::list<FecExceptionHandler *> &errorList, bool dcuHardIdOnly ) 
  throw (FecExceptionHandler) {

#define DCUDEBUG
  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, dcuDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // Read the DCU hard Id
#ifdef DCUDEBUG
  std::cerr << "DCUDEBUG  Start of Read the DCU hard Id" << std::endl;
#endif  
  accessDeviceTypeListMap vAccessesDcuHardId ;
  for ( Sgi::hash_map<keyType, dcuAccess *>::iterator itDcu = dcuSet.begin() ; itDcu != dcuSet.end() ; itDcu ++ ) {

    // tscType32 chipaddh = accessToFec_->readOffset(accessKey_, CHIPADDH) ;
    accessDeviceType chipaddh = { itDcu->second->getKey(), NORMALMODE, MODE_READ, CHIPADDH, 0, false, 0, 0, 0, NULL} ;
    vAccessesDcuHardId[getFecRingKey(itDcu->second->getKey())].push_back(chipaddh) ;
    // tscType32 chipaddm = accessToFec_->readOffset(accessKey_, CHIPADDM) ;
    accessDeviceType chipaddm = { itDcu->second->getKey(), NORMALMODE, MODE_READ, CHIPADDM, 0, false, 0, 0, 0, NULL} ;
    vAccessesDcuHardId[getFecRingKey(itDcu->second->getKey())].push_back(chipaddm) ;
    // tscType32 chipaddl = accessToFec_->readOffset(accessKey_, CHIPADDL) ;
    accessDeviceType chipaddl = { itDcu->second->getKey(), NORMALMODE, MODE_READ, CHIPADDL, 0, false, 0, 0, 0, NULL} ;
    vAccessesDcuHardId[getFecRingKey(itDcu->second->getKey())].push_back(chipaddl) ;

    deviceDescriptionsMap[itDcu->second->getKey()] = new dcuDescription ( itDcu->second->getKey(), itDcu->second->getDcuType() ) ;
    deviceDescriptionsMap[itDcu->second->getKey()]->setDcuHardId (0) ; // not filled
    deviceDescriptionsMap[itDcu->second->getKey()]->setTimeStamp (time(NULL)) ; // the time stamp
    //dcuVector.push_back(deviceDescriptionsMap[itDcu->second->getKey()]) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesDcuHardId, errorList ) ;

  // Collect the answer and fill the corresponding dcuDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesDcuHardId.begin() ; itList != vAccessesDcuHardId.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      if (itDevice->e != NULL) { // An error occurs
	//deviceDescriptionsMap[itDevice->index]->setDcuHardId (0xFFFFFFFF) ; // 0 means error
	dcuDescription *dcuToBeDeleted = deviceDescriptionsMap[itDevice->index] ;
	deviceDescriptionsMap.erase(itDevice->index) ;
	delete dcuToBeDeleted ;
	
#ifdef DEBUGMSGERROR
	std::cout << "Oups an error in the DCU Hard ID" << std::endl ;
#endif
      }
      else {

	if ( (deviceDescriptionsMap.find(itDevice->index) != deviceDescriptionsMap.end()) && 
	     (deviceDescriptionsMap[itDevice->index]->getDcuHardId() != 0xFFFFFFFF) ) {

	  // CHIPADD = 2^16 * CHIPADDH + 2^8 * CHIPADDM + CHIPADDL
	  tscType32 current = deviceDescriptionsMap[itDevice->index]->getDcuHardId() ;

	  switch (itDevice->offset) {
	  case CHIPADDH: current = (current | itDevice->data << 16) ; break ;
	  case CHIPADDM: current = (current | itDevice->data << 8)  ; break ;
	  case CHIPADDL: current = (current | itDevice->data)       ; break ;
	  }
	  // set the DCU hard id
	  deviceDescriptionsMap[itDevice->index]->setDcuHardId(current) ;
	  // set the FEC hardware id and crate slot
	  deviceDescriptionsMap[itDevice->index]->setFecHardwareId ( dcuSet[itDevice->index]->getFecHardwareId(), dcuSet[itDevice->index]->getCrateId() ) ;
	  
#ifdef DEBUGMSGERROR
	  char msg[80] ;
	  decodeKey(msg,itDevice->index) ;
	  std::cout << "Dcu hard id = 0x" << std::hex << current << " on " << msg << std::endl ;
#endif
	}
      }
    }
  }

#ifdef DCUDEBUG
  std::cerr << "DCUDEBUG  End of read the DCU hard Id" << std::endl;
#endif  

  // if the channels should be read
  if (!dcuHardIdOnly) {

#ifdef DCUDEBUG
  std::cerr << "DCUDEBUG  Start of !dcuHardIdOnly" << std::endl;
#endif  

    // -------------------------------------------------------------------
    // Set the TREG before reading a channel
    accessDeviceTypeListMap vAccessesTreg ;
    for ( Sgi::hash_map<keyType, dcuAccess *>::iterator itDcu = dcuSet.begin() ; itDcu != dcuSet.end() ; itDcu ++ ) {
    
      if (deviceDescriptionsMap.find(itDcu->second->getKey()) != deviceDescriptionsMap.end()) {
	// accessToFec_->writeOffset(accessKey_, TREG, 0x10); 
	accessDeviceType treg = { itDcu->second->getKey(), NORMALMODE, MODE_WRITE, TREG, 0x10, false, 0, 0, 0, NULL} ;
	vAccessesTreg[getFecRingKey(itDcu->second->getKey())].push_back(treg) ;
      }
    }

    // Send it over the ring and retreive the errors in a list
    error += fecAccess.setBlockDevices( vAccessesTreg, errorList ) ;
#ifdef DCUDEBUG
  std::cerr << "DCUDEBUG  After sending command vAccessesTreg" << std::endl;
#endif  

    // -------------------------------------------------------------------
    // Readout each channel
    for (tscType16 channel = 0 ; channel < MAXDCUCHANNELS ; channel ++) {
      
      // -------------------------------------------------------------------
      // Start the digitization
      accessDeviceTypeListMap vAccessesDigitisation ;
      
      for ( Sgi::hash_map<keyType, dcuAccess *>::iterator itDcu = dcuSet.begin() ; itDcu != dcuSet.end() ; itDcu ++ ) {
	if (itDcu->second->getChannelEnabled(channel)) {
	  
	  if (deviceDescriptionsMap.find(itDcu->second->getKey()) != deviceDescriptionsMap.end()) {
	    // Start the digitization: accessToFec_->writeOffset (accessKey_,CREG, 0x98 + channel);
	    accessDeviceType digit = { itDcu->second->getKey(), NORMALMODE, MODE_WRITE, CREG, (tscType16)(0x98 + channel), false, 0, 0, 0, NULL} ;
	    vAccessesDigitisation[getFecRingKey(itDcu->second->getKey())].push_back(digit) ;
	  }
	}
      }
      
      // Send it over the ring and retreive the errors in a list
      error += fecAccess.setBlockDevices( vAccessesDigitisation, errorList ) ;

      // -------------------------------------------------------------------
      // Wait for the end of the ADC acquisition
      usleep (WAITTIME4DIGITOK) ;
      
      // -------------------------------------------------------------------
      // Check that the digitisation is finished or not
      accessDeviceTypeListMap vAccessesFinished ;
      for ( Sgi::hash_map<keyType, dcuAccess *>::iterator itDcu = dcuSet.begin() ; itDcu != dcuSet.end() ; itDcu ++ ) {
	if (itDcu->second->getChannelEnabled(channel)) {
	  
	  if (deviceDescriptionsMap.find(itDcu->second->getKey()) != deviceDescriptionsMap.end()) {
	    // tscType16 shreg = accessToFec_->readOffset(accessKey_, SHREG) ;
	    accessDeviceType digit = { itDcu->second->getKey(), NORMALMODE, MODE_READ, SHREG, 0, false, 0, 0, 0, NULL} ;
	    vAccessesFinished[getFecRingKey(itDcu->second->getKey())].push_back(digit) ;
	  }
	}
      }

      // While the digitisation is not done loop on it
      bool digitFinished = true ;
      int timeout = 0 ;
      do {
	// Send it over the ring and retreive the errors in a list
	error += fecAccess.setBlockDevices( vAccessesFinished, errorList ) ;

	// Analyse it
	digitFinished = true ;

	// Collect the answer and fill the corresponding dcuDescription
	for (accessDeviceTypeListMap::iterator itList = vAccessesFinished.begin() ; itList != vAccessesFinished.end() ; itList ++) {
	  // for each list
	  for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {
	  
	    if (itDevice->e == NULL) {
	      if (!isDigitisationOk(itDevice->data)) {
		digitFinished = false ;
		// must send again
		itDevice->sent = false ;
		itDevice->e    = NULL  ;
		itDevice->tnum = 0     ;
		itDevice->dAck = 0     ;
		itDevice->fAck = 0     ;
	      }
	      else { // digitisation complete

#ifdef DEBUGMSGERROR
		char msg[80] ;
		decodeKey(msg,itDevice->index) ;
		std::cout << msg << ": ID = 0x" << std::hex << deviceDescriptionsMap[itDevice->index]->getDcuHardId() << std::endl ;
		//cout << "index = " << itDevice->index << endl ;
		std::cout << "Channel " << std::dec << (int)channel << " SHREG value = " << itDevice->data << " digitisation = " << isDigitisationOk(itDevice->data) << ", value = " << getValueShreg (itDevice->data) << std::endl ;
#endif	      

		switch (channel) {
		case 0: deviceDescriptionsMap[itDevice->index]->setDcuChannel0 ( getValueShreg (itDevice->data) ) ; break ;
		case 1: deviceDescriptionsMap[itDevice->index]->setDcuChannel1 ( getValueShreg (itDevice->data) ) ; break ;
		case 2: deviceDescriptionsMap[itDevice->index]->setDcuChannel2 ( getValueShreg (itDevice->data) ) ; break ;
		case 3: deviceDescriptionsMap[itDevice->index]->setDcuChannel3 ( getValueShreg (itDevice->data) ) ; break ;
		case 4: deviceDescriptionsMap[itDevice->index]->setDcuChannel4 ( getValueShreg (itDevice->data) ) ; break ;
		case 5: deviceDescriptionsMap[itDevice->index]->setDcuChannel5 ( getValueShreg (itDevice->data) ) ; break ;
		case 6: deviceDescriptionsMap[itDevice->index]->setDcuChannel6 ( getValueShreg (itDevice->data) ) ; break ;
		case 7: deviceDescriptionsMap[itDevice->index]->setDcuChannel7 ( getValueShreg (itDevice->data) ) ; break ;
		}

	      }
	    }
	    else { // (then error) set 0
	      switch (channel) {
	      case 0: deviceDescriptionsMap[itDevice->index]->setDcuChannel0 ( 0 ) ; break ;
	      case 1: deviceDescriptionsMap[itDevice->index]->setDcuChannel1 ( 0 ) ; break ;
	      case 2: deviceDescriptionsMap[itDevice->index]->setDcuChannel2 ( 0 ) ; break ;
	      case 3: deviceDescriptionsMap[itDevice->index]->setDcuChannel3 ( 0 ) ; break ;
	      case 4: deviceDescriptionsMap[itDevice->index]->setDcuChannel4 ( 0 ) ; break ;
	      case 5: deviceDescriptionsMap[itDevice->index]->setDcuChannel5 ( 0 ) ; break ;
	      case 6: deviceDescriptionsMap[itDevice->index]->setDcuChannel6 ( 0 ) ; break ;
	      case 7: deviceDescriptionsMap[itDevice->index]->setDcuChannel7 ( 0 ) ; break ; 
	      }
	    }
	  }
	}

	timeout ++ ;

      } while ((!digitFinished) && (timeout < DCUTIMEOUT)) ;
    
      // -------------------------------------------------------------------
      // Finally readout the LREG
      // if the digisation has been complete
      accessDeviceTypeListMap vAccessesLreg ;
      for (accessDeviceTypeListMap::iterator itList = vAccessesFinished.begin() ; itList != vAccessesFinished.end() ; itList ++) {
	// for each list
	for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {
	  accessDeviceType lreg = { itDevice->index, NORMALMODE, MODE_READ, LREG, 0, false, 0, 0, 0, NULL} ;
	  vAccessesLreg[getFecRingKey(itDevice->index)].push_back(lreg) ;
	}
      }
      // Send it over the ring and retreive the errors in a list
      error += fecAccess.setBlockDevices( vAccessesLreg, errorList ) ;

      // Collect the answer and fill the corresponding dcuDescription
      for (accessDeviceTypeListMap::iterator itList = vAccessesLreg.begin() ; itList != vAccessesLreg.end() ; itList ++) {
	// for each list
	for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {
	  
	  if (itDevice->e == NULL) {

#ifdef DEBUGMSGERROR
	    char msg[80] ;
	    decodeKey(msg,itDevice->index) ;
	    std::cout << msg << ": ID = 0x" << std::hex << deviceDescriptionsMap[itDevice->index]->getDcuHardId() << std::endl ;
	    //cout << "index = " << itDevice->index << endl ;
	    std::cout << "Channel " << std::dec << (int)channel << " LREG value = " << itDevice->data << ", value = " << getValueLreg (itDevice->data) << std::endl ;
#endif

	    tscType16 current = 0 ;
	    switch (channel) {
	    case 0: {
	      current = deviceDescriptionsMap[itDevice->index]->getDcuChannel0 () ;
	      current += getValueLreg ( itDevice->data ) ;
	      deviceDescriptionsMap[itDevice->index]->setDcuChannel0 ( current ) ; 
	      break ;
	    }
	    case 1: {
	      current = deviceDescriptionsMap[itDevice->index]->getDcuChannel1 () ;
	      current += getValueLreg ( itDevice->data ) ;
	      deviceDescriptionsMap[itDevice->index]->setDcuChannel1 ( current ) ; 
	      break ;
	    }
	    case 2: {
	      current = deviceDescriptionsMap[itDevice->index]->getDcuChannel2 () ;
	      current += getValueLreg ( itDevice->data ) ;
	      deviceDescriptionsMap[itDevice->index]->setDcuChannel2 ( current ) ; 
	      break ;
	    }
	    case 3: {
	      current = deviceDescriptionsMap[itDevice->index]->getDcuChannel3 () ;
	      current += getValueLreg ( itDevice->data ) ;
	      deviceDescriptionsMap[itDevice->index]->setDcuChannel3 ( current ) ; 
	      break ;
	    }
	    case 4: {
	      current = deviceDescriptionsMap[itDevice->index]->getDcuChannel4 () ;
	      current += getValueLreg ( itDevice->data ) ;
	      deviceDescriptionsMap[itDevice->index]->setDcuChannel4 ( current ) ; 
	      break ;
	    }
	    case 5: {
	      current = deviceDescriptionsMap[itDevice->index]->getDcuChannel5 () ;
	      current += getValueLreg ( itDevice->data ) ;
	      deviceDescriptionsMap[itDevice->index]->setDcuChannel5 ( current ) ; 
	      break ;
	    }
	    case 6: {
	      current = deviceDescriptionsMap[itDevice->index]->getDcuChannel6 () ;
	      current += getValueLreg ( itDevice->data ) ;
	      deviceDescriptionsMap[itDevice->index]->setDcuChannel6 ( current ) ; 
	      break ;
	    }
	    case 7: {
	      current = deviceDescriptionsMap[itDevice->index]->getDcuChannel7 () ;
	      current += getValueLreg ( itDevice->data ) ;
	      deviceDescriptionsMap[itDevice->index]->setDcuChannel7 ( current ) ; 
	      break ;
	    }
	    }
	  }
	  else  { // error
	    switch (channel) {
	    case 0: deviceDescriptionsMap[itDevice->index]->setDcuChannel0 ( 0 ) ; break ;
	    case 1: deviceDescriptionsMap[itDevice->index]->setDcuChannel1 ( 0 ) ; break ;
	    case 2: deviceDescriptionsMap[itDevice->index]->setDcuChannel2 ( 0 ) ; break ;
	    case 3: deviceDescriptionsMap[itDevice->index]->setDcuChannel3 ( 0 ) ; break ;
	    case 4: deviceDescriptionsMap[itDevice->index]->setDcuChannel4 ( 0 ) ; break ;
	    case 5: deviceDescriptionsMap[itDevice->index]->setDcuChannel5 ( 0 ) ; break ;
	    case 6: deviceDescriptionsMap[itDevice->index]->setDcuChannel6 ( 0 ) ; break ;
	    case 7: deviceDescriptionsMap[itDevice->index]->setDcuChannel7 ( 0 ) ; break ; 
	    }
	  }
	}
      }
#ifdef DCUDEBUG
      std::cerr << "DCUDEBUG  After readout channel " << channel << std::endl;
#endif  

    }
#ifdef DCUDEBUG
  std::cerr << "DCUDEBUG  End of !dcuHardIdOnly" << std::endl;
#endif  

  }

  // Build a vector of the elements
  for (Sgi::hash_map< keyType, dcuDescription * >::iterator it = deviceDescriptionsMap.begin() ; it != deviceDescriptionsMap.end() ; it ++) {
    if (it->second != NULL) {
      dcuVector.push_back(it->second) ;
    }
  }

  return (error) ;
}
