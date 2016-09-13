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

#include "TkDiagErrorAnalyser.h"

//#define DEBUGTIMING
//#define DEBUGMSGERROR

/** Create a database access with the CONFDB
 * \param partitionName - partition name
 * \param login - login to database (if "nil" then the string is retreive from the env. variable)
 * \param password - password to database
 * \param path - path to database
 */
TkDiagErrorAnalyser::TkDiagErrorAnalyser ( std::string partitionName, std::string login, std::string password, std::string path ) 
#ifdef DATABASE
  throw (oracle::occi::SQLException, FecExceptionHandler) {

  if (login == "nil") DbAccess::getDbConfiguration (login, password, path) ;

  buildDatabaseInformation (partitionName, login, password, path) ;
}
#else
  throw (FecExceptionHandler) {

  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Database support is not compiled, cannot use this class", FATALERRORCODE) ;
}
#endif

TkDiagErrorAnalyser::~TkDiagErrorAnalyser ( ) {

  FecFactory::deleteVectorI(listVectorDevices_) ;

#ifdef DATABASE
  // delete the database access
  try {
    delete deviceFactory_ ;
  }
  catch (oracle::occi::SQLException &e) {
  }
  catch (FecExceptionHandler &e) {
  }
#endif
}

#ifdef DATABASE

/** Build the PSU name to DCU hard ID
 * \warning the number of det id (detIdList_) and the number of modules (listModulesAsConnection_) should be the same
 */
void TkDiagErrorAnalyser::buildDatabaseInformation ( std::string partitionName, std::string login, std::string password, std::string path ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

  // -----------------------------------------------------------------
  // Database creation
  deviceFactory_ = new DeviceFactory( login, password, path ) ;

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  // -----------------------------------------------------------------
  // Download the connections
  connectionNotIdentified_ = deviceFactory_->getConnectionDescriptionDetId ( partitionName, connectionVector_, 0, 0, true, false) ;

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << std::dec << "Download of " << connectionVector_.size() << " connections from the database for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
  if (connectionNotIdentified_ != 0) std::cerr << "ERROR: " << connectionNotIdentified_ << " connection(s) have no correponding det id" << std::endl ; 
#endif

  // -----------------------------------------------------------------
  // Partition is fine
  partitionName_ = partitionName ;

  std::map<std::pair<unsigned int, keyType>, std::pair<std::string, unsigned int> > modules;
  std::map<std::pair<std::string, keyType>, ConnectionDescription *> listModules ;

#ifdef DEBUGTIMING
  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  // -----------------------------------------------------------------
  // Build the different maps and lists
  ErrorCounterStruct counterError = {0,0,0} ;
  for (ConnectionVector::iterator it = connectionVector_.begin() ; it != connectionVector_.end() ; it ++) {

    // By DCU
    dcuToConnection_[(*it)->getDcuHardId()] = (*it) ;

    // By DET ID
    detIdToConnection_[(*it)->getDetId()] = (*it) ;

    // By FEC
    fecConnectionVector_.push_back(*it) ;

    // By FED
    fedConnectionVector_.push_back(*it) ;

    // Initialise to zero the three counters
    errorCounters_[*it] = counterError ;

    // build the list of all devices
    keyType index = buildCompleteKey((*it)->getFecSlot(),(*it)->getRingSlot(),(*it)->getCcuAddress(),(*it)->getI2cChannel(),0);
    std::pair<unsigned int, keyType> maPair = std::make_pair((*it)->getFecCrateId(),index) ;
    std::pair<std::string, unsigned int> maPairFec = std::make_pair((*it)->getFecHardwareId(),(*it)->getNumberOfApvs()) ;
    modules[maPair] = maPairFec ;

    // Build the list of modules for only one connection (last one)
    std::pair<std::string, keyType> maPairModule =  std::make_pair((*it)->getFecHardwareId(),index) ;
    listModules[maPairModule] = (*it) ;
  }

#ifdef DEBUGTIMING
  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << std::dec << "Build of map for FEC/FED/DETID and building of FEC key for " << modules.size() << " modules took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

#ifdef DEBUGTIMING
  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  // -----------------------------------------------------------------
  // Build the list of det id
  for (std::map<unsigned int, ConnectionDescription *>::iterator it = detIdToConnection_.begin() ; it != detIdToConnection_.end() ; it ++) {
    detIdList_.push_back(it->first) ;
  }
  std::sort(detIdList_.begin(), detIdList_.end()) ;


  // -----------------------------------------------------------------
  // build the list of modules as connection list
  for (std::map<std::pair<std::string, keyType>, ConnectionDescription *>::iterator it = listModules.begin() ; it != listModules.end() ; it ++) {
    listModulesAsConnection_.push_back(it->second) ;
  }
  std::sort(listModulesAsConnection_.begin(),listModulesAsConnection_.end(), TkDiagErrorAnalyser::sortConnectionByFec) ;

  // -----------------------------------------------------------------
  // Create the device list
  for (std::map<std::pair<unsigned int, keyType>, std::pair<std::string, unsigned int> >::iterator it = modules.begin() ; it != modules.end() ; it ++) {

    std::pair<unsigned int, keyType> maPair = it->first ;
    unsigned int crateId = maPair.first ;
    keyType index = maPair.second;
    std::pair<std::string, unsigned int> maPairFec = it->second ;
    std::string fecHardwareId = maPairFec.first ;
    unsigned int nbApv = maPairFec.second ;

#ifdef DEBUGMSGERROR
    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << ": " << nbApv << " pairs of APVs on " 
	      << fecHardwareId <<  " "
	      << getFecKey(index) << " "
	      << getRingKey(index) << " "
	      << getCcuKey(index) << " "
	      << getChannelKey(index) << std::endl ;
#endif

    std::pair<unsigned int, keyType> maPair1 = std::make_pair(crateId,0) ;

    // keyType dcu = index ; maPair1.second = dcu ; listVectorDevices_.push_back(maPair1) ;
    dcuDescription *dcu = new dcuDescription ( index | setAddressKey(0) ) ;
    dcu->setFecHardwareId(fecHardwareId,crateId) ;
    //dcu->setCrateId(crateId) ;
    listVectorDevices_.push_back (dcu) ;
    errorOnDevices_[dcu] = 0 ;
    // keyType apv20 = index | setAddressKey(0x20) ; maPair1.second = apv20 ; listVectorDevices_.push_back(maPair1) ;
    apvDescription *apv20 = new apvDescription ( index | setAddressKey(0x20) ) ;
    apv20->setFecHardwareId(fecHardwareId,crateId) ;
    //apv20->setCrateId(crateId) ;
    listVectorDevices_.push_back (apv20) ;
    errorOnDevices_[apv20] = 0 ;
    // keyType apv21 = index | setAddressKey(0x21) ; maPair1.second = apv21 ; listVectorDevices_.push_back(maPair1) ;
    apvDescription *apv21 = new apvDescription ( index | setAddressKey(0x21) ) ;
    apv21->setFecHardwareId(fecHardwareId,crateId) ;
    //apv21->setCrateId(crateId) ;
    listVectorDevices_.push_back (apv21) ;
    errorOnDevices_[apv21] = 0; 
    // keyType apv22 = index | setAddressKey(0x22) ; maPair1.second = apv22 ; listVectorDevices_.push_back(maPair1) ;
    if (nbApv == 6) {
      apvDescription *apv22 = new apvDescription ( index | setAddressKey(0x22) ) ;
      apv22->setFecHardwareId(fecHardwareId,crateId) ;
      //apv22->setCrateId(crateId) ;
      listVectorDevices_.push_back (apv22) ;
      errorOnDevices_[apv22] = 0; 

      // keyType apv23 = index | setAddressKey(0x23) ; maPair1.second = apv23 ; listVectorDevices_.push_back(maPair1) ;
      apvDescription *apv23 = new apvDescription ( index | setAddressKey(0x23) ) ;
      apv23->setFecHardwareId(fecHardwareId,crateId) ;
      //apv23->setCrateId(crateId) ;
      listVectorDevices_.push_back (apv23) ; 
      errorOnDevices_[apv23] = 0; 
    }
    
    // keyType apv24 = index | setAddressKey(0x24) ; maPair1.second = apv24 ; listVectorDevices_.push_back(maPair1) ;
    apvDescription *apv24 = new apvDescription ( index | setAddressKey(0x24) ) ;
    apv24->setFecHardwareId(fecHardwareId,crateId) ;
    //apv24->setCrateId(crateId) ;
    listVectorDevices_.push_back (apv24) ; 
    errorOnDevices_[apv24] = 0; 

    // keyType apv25 = index | setAddressKey(0x25) ; maPair1.second = apv25 ; listVectorDevices_.push_back(maPair1) ;
    apvDescription *apv25 = new apvDescription ( index | setAddressKey(0x25) ) ;
    apv25->setFecHardwareId(fecHardwareId,crateId) ;
    //apv25->setCrateId(crateId) ;
    listVectorDevices_.push_back (apv25) ; 
    errorOnDevices_[apv25] = 0; 

    // keyType mux = index | setAddressKey(0x43) ; maPair1.second = mux ; listVectorDevices_.push_back(maPair1) ;
    muxDescription *mux = new muxDescription ( index | setAddressKey(0x43) ) ;
    mux->setFecHardwareId(fecHardwareId,crateId) ;
    //mux->setCrateId(crateId) ;
    listVectorDevices_.push_back (mux) ;
    errorOnDevices_[mux] = 0; 
    // keyType pll = index | setAddressKey(0x44) ; maPair1.second = pll ; listVectorDevices_.push_back(maPair1) ;
    pllDescription *pll = new pllDescription ( index | setAddressKey(0x44) ) ;
    pll->setFecHardwareId(fecHardwareId,crateId) ;
    //pll->setCrateId(crateId) ;
    listVectorDevices_.push_back (pll) ;
    errorOnDevices_[pll] = 0; 
    // keyType aoh = index | setAddressKey(0x60) ; maPair1.second = aoh ; listVectorDevices_.push_back(maPair1) ;
    laserdriverDescription *aoh = new laserdriverDescription ( index | setAddressKey(0x60) ) ;
    aoh->setFecHardwareId(fecHardwareId,crateId) ;
    //aoh->setCrateId(crateId) ;
    listVectorDevices_.push_back (aoh) ;
    errorOnDevices_[aoh] = 0; 
    // keyType doh = index | setAddressKey(0x70) ; maPair1.second = doh ; listVectorDevices_.push_back(maPair1) ;
    laserdriverDescription *doh = new laserdriverDescription ( index | setAddressKey(0x70) ) ;
    doh->setFecHardwareId(fecHardwareId,crateId) ;
    //doh->setCrateId(crateId) ;
    listVectorDevices_.push_back (doh) ;
    errorOnDevices_[doh] = 0; 
  }

#ifdef DEBUGTIMING
  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << std::dec << "Build of " << listVectorDevices_.size() << " FEC device descriptions took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

#ifdef DEBUGTIMING
  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  // -----------------------------------------------------------------
  // Download the PSU/DCU map
  deviceFactory_->getDcuPsuMapPartition(partitionName) ;
  tkDcuPsuMapVector v = deviceFactory_->getAllTkDcuPsuMaps() ;

  // -----------------------------------------------------------------
  // Build the map from PSU to DCU hard ID
  psuNotIdentified_ = 0 ;
  for (tkDcuPsuMapVector::iterator it = v.begin() ; it != v.end() ; it ++) {

    if (dcuToConnection_.find((*it)->getDcuHardId()) != dcuToConnection_.end()) {
      pvssNameToConnection_[(*it)->getPVSSName()] = dcuToConnection_[(*it)->getDcuHardId()] ;
      psuNameToConnection_[(*it)->getPsuName()] = dcuToConnection_[(*it)->getDcuHardId()] ;
      dpNameToConnection_[(*it)->getDatapointName()] = dcuToConnection_[(*it)->getDcuHardId()] ;
      pvssNameList_.push_back((*it)->getPVSSName()) ;
    }
    else if ((*it)->getDcuType() == PSUDCUTYPE_PG) {
#ifdef DEBUGTIMING
      std::cerr << "Online running PSU> The DCU " << (*it)->getDcuHardId() << " corresponding to " << (*it)->getPVSSName() << " does not exists in the map" << std::endl ;
#endif
      psuNotIdentified_ ++ ;
    }
  }

#ifdef DEBUGTIMING
  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << std::dec << "Download and building of " << pvssNameList_.size() << " PSU names took " << (endMillis-startMillis) << " ms" << std::endl ;
  if (psuNotIdentified_ != 0) std::cerr << "ERROR: " << psuNotIdentified_ << " PSU/DCU have not been identified with connection(s)" << std::endl ;
#endif

#ifdef DEBUGTIMING
  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  // Sort the elements for requests
  std::sort(fecConnectionVector_.begin(), fecConnectionVector_.end(), TkDiagErrorAnalyser::sortConnectionByFec) ;
  std::sort(fedConnectionVector_.begin(), fedConnectionVector_.end(), TkDiagErrorAnalyser::sortConnectionByFed) ;
  std::sort(pvssNameList_.begin(), pvssNameList_.end()) ;
  std::sort(listVectorDevices_.begin(),listVectorDevices_.end(),deviceDescription::sortByKey) ;

#ifdef DEBUGTIMING
  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << std::dec << "Sorting of all vectors for FEC/FED/DETID/PVSS name/FEC devices took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

}
#endif

/** Increase the error for a FEC
 * \param fecHardwareId - FEC hardware ID
 * \param ring - ring (if NOLIFEINPARAMETER is set (default value) then just looking for the parameters before)
 * \param ccu - CCU address (if NOLIFEINPARAMETER is set (default value) then just looking for the parameters before)
 * \param channel - i2c channel (if NOLIFEINPARAMETER is set (default value) then just looking for the parameters before)
 * \param i2cAddress - i2c address (if NOLIFEINPARAMETER is set (default value) then just looking for the parameters before)
 * \exception a string saying the given hardware is not known
 */
void TkDiagErrorAnalyser::setFecError ( std::string fecHardwareId, unsigned int ring, unsigned int ccu, unsigned int channel, unsigned int i2cAddress ) throw (std::string) {

#ifdef DEBUGMSGERROR
  std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << ": set error on " 
	    << fecHardwareId <<  " "
	    << ring << " "
	    << ccu << " "
	    << channel << " "
	    << i2cAddress << std::endl ;
#endif

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  if (fecConnectionVector_.empty()) throw std::string ("No connections in the map") ;

  if (ring == NOLIFEINPARAMETER) {

    for (ConnectionVector::iterator it = fecConnectionVector_.begin() ; it != fecConnectionVector_.end() ; it ++) {
	  
      if (fecHardwareId == (*it)->getFecHardwareId()) {
	ConnectionDescription *connection = *it ;
	errorCounters_[connection].fecErrorCounter ++ ;
	errorCounters_[connection].fedErrorCounter ++ ;
	errorCounters_[connection].psuErrorCounter ++ ;
      }
    }
  }
  else if (ccu == NOLIFEINPARAMETER) {

    for (ConnectionVector::iterator it = fecConnectionVector_.begin() ; it != fecConnectionVector_.end() ; it ++) {

      if ((fecHardwareId == (*it)->getFecHardwareId()) && (ring == (*it)->getRingSlot())) {
	ConnectionDescription *connection = *it ;
	errorCounters_[connection].fecErrorCounter ++ ;
	errorCounters_[connection].fedErrorCounter ++ ;
	errorCounters_[connection].psuErrorCounter ++ ;
      }
    }
  }
  else if (channel == NOLIFEINPARAMETER) {

    for (ConnectionVector::iterator it = fecConnectionVector_.begin() ; it != fecConnectionVector_.end() ; it ++) {
      
      if ((fecHardwareId == (*it)->getFecHardwareId()) && (ring == (*it)->getRingSlot()) && (ccu == (*it)->getCcuAddress())) {
	ConnectionDescription *connection = *it ;
	errorCounters_[connection].fecErrorCounter ++ ;
	errorCounters_[connection].fedErrorCounter ++ ;
	errorCounters_[connection].psuErrorCounter ++ ;
      }
    }
  }
  else if (i2cAddress == NOLIFEINPARAMETER) {

    for (ConnectionVector::iterator it = fecConnectionVector_.begin() ; it != fecConnectionVector_.end() ; it ++) {
      
      if ((fecHardwareId == (*it)->getFecHardwareId()) && (ring == (*it)->getRingSlot()) && (ccu == (*it)->getCcuAddress())  && (channel == (*it)->getI2cChannel())) {
	ConnectionDescription *connection = *it ;
	errorCounters_[connection].fecErrorCounter ++ ;
	errorCounters_[connection].fedErrorCounter ++ ;
	errorCounters_[connection].psuErrorCounter ++ ;
      }
    }
  }
  else { // A complete set of FEC key have been set

    if ( (i2cAddress != DCUADDRESS) && (i2cAddress != 0x70) ) {

      if ( (i2cAddress >= 0x20) && (i2cAddress <= 0x26) ) { // APV error so only one connection in error

	unsigned int apvAddress = i2cAddress ;
	if ( (apvAddress == 0x21) || (apvAddress == 0x23) || (apvAddress == 0x25) ) apvAddress -= 1 ; // first APV in the pair
	
	// Toggle the connections in error
	ConnectionVector::iterator itConnection = fecConnectionVector_.begin() ;
	while ( (itConnection != fecConnectionVector_.end()) && 
		(
		 (fecHardwareId != (*itConnection)->getFecHardwareId()) ||
		 (ring != (*itConnection)->getRingSlot()) ||
		 (ccu != (*itConnection)->getCcuAddress()) ||
		 (channel != (*itConnection)->getI2cChannel()) ||
		 (apvAddress != (*itConnection)->getApvAddress()) )
		) {

	  itConnection ++ ;
	}
	
	if (itConnection == fecConnectionVector_.end()) {
	  std::stringstream msgError ; msgError << "No connection found for DCU on FEC " << std::dec << fecHardwareId 
						<< " ring " << ring << " ccu 0x" << std::hex << ccu << " channel 0x" 
						<< channel << " i2c address 0x" << i2cAddress << std::dec ;
	  throw std::string (msgError.str()) ;
	}
	else {

#ifdef DEBUGMSGERROR
	  std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << ": found a connection on APV "
		    << (*itConnection)->getFecHardwareId() <<  " "
		    << (*itConnection)->getFecSlot() << " "
		    << (*itConnection)->getRingSlot() << " "
		    << (*itConnection)->getCcuAddress() << " "
		    << (*itConnection)->getI2cChannel() << " "
		    << (*itConnection)->getApvAddress() << std::endl ;
#endif

	  ConnectionDescription *connection = *itConnection ;
	  errorCounters_[connection].fecErrorCounter ++ ;
	  errorCounters_[connection].fedErrorCounter ++ ;
	  errorCounters_[connection].psuErrorCounter ++ ;
	}
      }
      else { // PLL MUX AOH
	
	for (ConnectionVector::iterator it = fecConnectionVector_.begin() ; it != fecConnectionVector_.end() ; it ++) {
	  
	  if ((fecHardwareId == (*it)->getFecHardwareId()) && (ring == (*it)->getRingSlot()) && (ccu == (*it)->getCcuAddress())  && (channel -= (*it)->getI2cChannel())) {
	    ConnectionDescription *connection = *it ;
	    errorCounters_[connection].fecErrorCounter ++ ;
	    errorCounters_[connection].fedErrorCounter ++ ;
	    errorCounters_[connection].psuErrorCounter ++ ;
	  }
	}
      }
    }

    // Toggle only the corresponding device
    deviceVector::iterator itDevice = listVectorDevices_.begin() ;
    while ( (itDevice != listVectorDevices_.end()) &&
	    ( 
	     (fecHardwareId != (*itDevice)->getFecHardwareId()) ||
	     (ring != (*itDevice)->getRingSlot()) ||
	     (ccu != (*itDevice)->getCcuAddress()) ||
	     (channel != (*itDevice)->getChannel()) ||
	     (i2cAddress != (*itDevice)->getAddress()) )
	    ) {

      itDevice ++ ;
    }

    if (itDevice != listVectorDevices_.end()) {

#ifdef DEBUGMSGERROR
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << ": Tag the device "
		<< (*itDevice)->getFecHardwareId() <<  " "
		<< (*itDevice)->getFecSlot() << " "
		<< (*itDevice)->getRingSlot() << " "
		<< (*itDevice)->getCcuAddress() << " "
		<< (*itDevice)->getChannel() << " "
		<< (*itDevice)->getAddress() << std::endl ;
#endif


      errorOnDevices_[(*itDevice)] ++ ; 
    }
    else {
      std::stringstream msgError ; msgError << std::dec << "Online running> Incoherence in the list of devices, FEC " << fecHardwareId << ", device on ring " << ring << " CCU " << ccu << " i2c channel " << channel << " i2c address " << i2cAddress << " is not existing in the map, device lists from connection was not built correctly" ;

#ifdef DEBUGMSGERROR
      std::cerr << msgError.str() << std::endl ;
#endif

      throw msgError.str() ;
    }
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::setFecError took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Increate the error for FED
 * \param fedSoftId - FED soft ID
 * \param fedChannel - FED channel (if NOLIFEINPARAMETER is set (default value) then just looking for the parameters before)
 * \exception a string saying the given hardware is not known
 */
void TkDiagErrorAnalyser::setFedSoftIdError ( unsigned int fedSoftId, unsigned int fedChannel ) throw (std::string) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  // Complete FED
  if (fedChannel == NOLIFEINPARAMETER) {

    for (ConnectionVector::iterator it = fedConnectionVector_.begin() ; it != fedConnectionVector_.end() ; it ++) {
	  
      if (fedSoftId == (*it)->getFedId()) {
	ConnectionDescription *connection = *it ;
	errorCounters_[connection].fecErrorCounter ++ ;
	errorCounters_[connection].fedErrorCounter ++ ;
	errorCounters_[connection].psuErrorCounter ++ ;
      }
    }
  }
  else { // FED,channel

    for (ConnectionVector::iterator it = fedConnectionVector_.begin() ; it != fedConnectionVector_.end() ; it ++) {

      //std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << " FED on " << (*it)->getFedId() << " " << (*it)->getFedChannel() << std::endl ;

      if ( (fedSoftId == (*it)->getFedId()) && (fedChannel == (*it)->getFedChannel()) ) {
	ConnectionDescription *connection = *it ;
	errorCounters_[connection].fecErrorCounter ++ ;
	errorCounters_[connection].fedErrorCounter ++ ;
	errorCounters_[connection].psuErrorCounter ++ ;

#ifdef DEBUGMSGERROR
	std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << ": found a connection " << std::endl 
		  << "\t on FED " << connection->getFedId() << " "
		  << connection->getFedChannel() << std::endl 
		  << "\t on FED " << connection->getFedCrateId() << " " 
		  << connection->getFedSlot() << " "
		  << connection->getFedChannel() << std::endl 
		  << "\t on FEC " << connection->getFecHardwareId() <<  " "
		  << connection->getFecSlot() << " "
		  << connection->getRingSlot() << " "
		  << connection->getCcuAddress() << " "
		  << connection->getI2cChannel() << " "
		  << connection->getApvAddress() << std::endl 
		  << "\t DET ID " << connection->getDetId() << std::endl ;
#endif

      }
    } 
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::setFedSoftIdError took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Set an error for a given FED
 * \param crateId - crate ID
 * \param slot - slot in the crate
 * \param fedChannel - FED channel (if NOLIFEINPARAMETER is set (default value) then just looking for the parameters before)
 * \exception a string saying the given hardware is not known
 */
void TkDiagErrorAnalyser::setFedCrateIdError ( unsigned int crateId, unsigned int slot, unsigned int fedChannel ) throw (std::string) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  // Complete FED
  if (fedChannel == NOLIFEINPARAMETER) {

    for (ConnectionVector::iterator it = fedConnectionVector_.begin() ; it != fedConnectionVector_.end() ; it ++) {
	  
      if ( (crateId == (*it)->getFedCrateId()) && (slot == (*it)->getFedSlot()) ) {
	ConnectionDescription *connection = *it ;
	errorCounters_[connection].fecErrorCounter ++ ;
	errorCounters_[connection].fedErrorCounter ++ ;
	errorCounters_[connection].psuErrorCounter ++ ;
      }
    }
  }
  else { // FED,channel

    for (ConnectionVector::iterator it = fedConnectionVector_.begin() ; it != fedConnectionVector_.end() ; it ++) {
	  
      if ( (crateId == (*it)->getFedCrateId()) && (slot == (*it)->getFedSlot()) && (fedChannel == (*it)->getFedChannel()) ) {
	ConnectionDescription *connection = *it ;
	errorCounters_[connection].fecErrorCounter ++ ;
	errorCounters_[connection].fedErrorCounter ++ ;
	errorCounters_[connection].psuErrorCounter ++ ;
      }
    } 
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::setFedCrateIdError took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Set an error for a given PSU channel
 * \param psuName - PVSS name (or dp name or Psu name)
 * \see TkDcuPsuMapDescription for more details
 */
void TkDiagErrorAnalyser::setPsuError ( std::string psuName ) throw (std::string) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  if (pvssNameToConnection_.find(psuName) != pvssNameToConnection_.end()) { // PVSS name
    ConnectionDescription *connection = pvssNameToConnection_[psuName] ;
    errorCounters_[connection].fecErrorCounter ++ ;
    errorCounters_[connection].fedErrorCounter ++ ;
    errorCounters_[connection].psuErrorCounter ++ ;
  }
  else if (dpNameToConnection_.find(psuName) != dpNameToConnection_.end()) { // DP name
    ConnectionDescription *connection = dpNameToConnection_[psuName] ;
    errorCounters_[connection].fecErrorCounter ++ ;
    errorCounters_[connection].fedErrorCounter ++ ;
    errorCounters_[connection].psuErrorCounter ++ ;
  }
  else if (psuNameToConnection_.find(psuName) != psuNameToConnection_.end()) { // PSU name
    ConnectionDescription *connection = psuNameToConnection_[psuName] ;
    errorCounters_[connection].fecErrorCounter ++ ;
    errorCounters_[connection].fedErrorCounter ++ ;
    errorCounters_[connection].psuErrorCounter ++ ;
  }
  else {
    throw std::string ("Invalid PSU name, no connection given for this name " + psuName) ;
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::setPsuError took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Set an error for a given DCU hard ID
 * \param dcuHardId - DCU hard ID
 */
void TkDiagErrorAnalyser::setDcuHardIdError ( unsigned int dcuHardId ) throw (std::string) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  if (dcuToConnection_.find(dcuHardId) != dcuToConnection_.end()) {
    ConnectionDescription *connection = dcuToConnection_[dcuHardId] ;
    errorCounters_[connection].fecErrorCounter ++ ;
    errorCounters_[connection].fedErrorCounter ++ ;
    errorCounters_[connection].psuErrorCounter ++ ;
  }
  else {
    throw std::string ("Invalid DCU hard id, no connection given for this id " + toString(dcuHardId)) ;
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::setDcuHardIdError took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Set an error for given det id 
 * \param detId - detector ID 
 */
void TkDiagErrorAnalyser::setDetIdError ( unsigned int detId ) throw (std::string) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  if (detIdToConnection_.find(detId) != detIdToConnection_.end()) {
    ConnectionDescription *connection = detIdToConnection_[detId] ;
    errorCounters_[connection].fecErrorCounter ++ ;
    errorCounters_[connection].fedErrorCounter ++ ;
    errorCounters_[connection].psuErrorCounter ++ ;
  }
  else {
    throw std::string ("Invalid det id, no connection given for this id " + toString(detId)) ;
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::setDetIdError took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Get the error counter for a given FEC. This method accumulate the errors on all the connections for a given module.
 * \param fecHardwareId - FEC hardware ID
 * \param ring - ring in the FEC
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param fecErrorCounter - FEC number of errors for the corresponding hardware (returned parameter)
 * \param fedErrorCounter - FED number of errors for the corresponding hardware (returned parameter)
 * \param psuErrorCounter - PSU number of errors for the corresponding hardware (returned parameter)
 */
void TkDiagErrorAnalyser::getFecModuleErrorCounter ( std::string fecHardwareId, unsigned int ring, unsigned int ccuAddress, unsigned int i2cChannel, 
						     unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  fecErrorCounter = 0 ; fedErrorCounter = 0 ; psuErrorCounter = 0 ;
  bool found = false ;
  for (std::map<ConnectionDescription *, ErrorCounterStruct>::iterator it = errorCounters_.begin() ; it != errorCounters_.end() ; it ++) {
    ConnectionDescription *connection = it->first ;

    if ( (fecHardwareId == connection->getFecHardwareId()) &&
 	 (ring == connection->getRingSlot()) &&
 	 (ccuAddress == connection->getCcuAddress()) &&
 	 (i2cChannel == connection->getI2cChannel()) ) {
      fecErrorCounter += it->second.fecErrorCounter ; 
      fedErrorCounter += it->second.fedErrorCounter ; 
      psuErrorCounter += it->second.psuErrorCounter ; 
      found = true ;
    }
  }

  if (!found) {
    std::stringstream msgError ; 
    msgError << "No connection found for FEC " << fecHardwareId << " ring " << ring << " CCU " << ccuAddress << " channel " << i2cChannel ;
    throw msgError.str() ;
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::getFecModuleErrorCounter took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Get the error counter for a given connection based on the APV address
 * \param fecHardwareId - FEC hardware ID
 * \param ring - ring in the FEC
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param apvAddress - APV address
 * \param fecErrorCounter - FEC number of errors for the corresponding hardware (returned parameter)
 * \param fedErrorCounter - FED number of errors for the corresponding hardware (returned parameter)
 * \param psuErrorCounter - PSU number of errors for the corresponding hardware (returned parameter)
 */
void TkDiagErrorAnalyser::getConnectionErrorCounter ( std::string fecHardwareId, unsigned int ring, unsigned int ccuAddress, unsigned int i2cChannel, 
						      unsigned int apvAddress, 
						      unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  fecErrorCounter = 0 ; fedErrorCounter = 0 ; psuErrorCounter = 0 ;
  bool found = false ;
  for (std::map<ConnectionDescription *, ErrorCounterStruct>::iterator it = errorCounters_.begin() ; (it != errorCounters_.end()) && !found ; it ++) {
    ConnectionDescription *connection = it->first ;

    if ( (fecHardwareId == connection->getFecHardwareId()) &&
 	 (ring == connection->getRingSlot()) &&
 	 (ccuAddress == connection->getCcuAddress()) &&
 	 (i2cChannel == connection->getI2cChannel()) &&
 	 (apvAddress == connection->getApvAddress()) ) {

#ifdef DEBUGMSGERROR
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << ": found connection on " 
		<< connection->getFecHardwareId() <<  " "
		<< connection->getFecSlot() << " "
		<< connection->getRingSlot() << " "
		<< connection->getCcuAddress() << " "
		<< connection->getI2cChannel() << " "
		<< connection->getApvAddress() << std::endl ;
#endif
      
      fecErrorCounter = it->second.fecErrorCounter ; 
      fedErrorCounter = it->second.fedErrorCounter ; 
      psuErrorCounter = it->second.psuErrorCounter ; 
      found = true ;
    }
  }

  if (!found) {
    std::stringstream msgError ; 
    msgError << "No connection found for FEC " << fecHardwareId << " ring " << ring << " CCU " << ccuAddress << " channel " << i2cChannel << " address " << apvAddress ;
    throw msgError.str() ;
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::getConnectionErrorCounter took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Get the error counter for a given FEC device
 * \param fecHardwareId - FEC hardware ID
 * \param ring - ring in the FEC
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param deviceErrorCounter - number of errors for the corresponding hardware (returned parameter)
 */
void TkDiagErrorAnalyser::getDeviceErrorCounter ( std::string fecHardwareId, unsigned int ring, unsigned int ccuAddress, unsigned int i2cChannel, 
						  unsigned int i2cAddress, 
						  unsigned int &deviceErrorCounter ) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  deviceErrorCounter = 0 ;
  deviceVector::iterator it = listVectorDevices_.begin() ;
  while ( (it != listVectorDevices_.end()) &&
	  ( 
	   ((*it)->getFecHardwareId() == fecHardwareId) ||
	   ((*it)->getRingSlot() == ring) ||
	   ((*it)->getCcuAddress() == ccuAddress) ||
	   ((*it)->getChannel() == i2cChannel) ||
	  ((*it)->getAddress() == i2cAddress) ) 
	  ) it ++ ;

  if (it != listVectorDevices_.end()) 
    deviceErrorCounter = errorOnDevices_[(*it)] ;
  else {
    std::stringstream msgError ; 
    msgError << "No device found for FEC " << fecHardwareId << " ring " << ring << " CCU 0x" << std::hex << ccuAddress 
	     << " channel 0x" << i2cChannel << " address 0x" << i2cAddress << std::dec ;
    throw msgError.str() ;
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::getDeviceErrorCounter took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Get the error counter for a FED channel
 * \param crateId - FED crate ID
 * \param slot - FED slot
 * \param fedChannel - FED channel
 * \param fecErrorCounter - FEC number of errors for the corresponding hardware (returned parameter)
 * \param fedErrorCounter - FED number of errors for the corresponding hardware (returned parameter)
 * \param psuErrorCounter - PSU number of errors for the corresponding hardware (returned parameter)
 */
void TkDiagErrorAnalyser::getFedChannelErrorCounter ( unsigned int crateId, unsigned slot, unsigned fedChannel, 
						      unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  fecErrorCounter = 0 ; fedErrorCounter = 0 ; psuErrorCounter = 0 ;
  bool found = false ;
  for (std::map<ConnectionDescription *, ErrorCounterStruct>::iterator it = errorCounters_.begin() ; (it != errorCounters_.end()) ; it ++) {
    ConnectionDescription *connection = it->first ;

    if ( (crateId == connection->getFedCrateId()) &&
 	 (slot == connection->getFedSlot()) &&
 	 (fedChannel == connection->getFedChannel()) ) {

      fecErrorCounter += it->second.fecErrorCounter ; 
      fedErrorCounter += it->second.fedErrorCounter ; 
      psuErrorCounter += it->second.psuErrorCounter ; 
      found = true ;
    }
  }

  if (!found) {
    std::stringstream msgError ; 
    msgError << "No connection found for FED on " << crateId << " slot " << slot << " channel " << fedChannel ;
    throw msgError.str() ;
  }


#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::getFedChannelErrorCounter took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Get the error counter for a FED channel
 * \param fedSoftId - FED software ID
 * \param fedChannel - FED channel
 * \param fecErrorCounter - FEC number of errors for the corresponding hardware (returned parameter)
 * \param fedErrorCounter - FED number of errors for the corresponding hardware (returned parameter)
 * \param psuErrorCounter - PSU number of errors for the corresponding hardware (returned parameter)
 */
void TkDiagErrorAnalyser::getFedChannelErrorCounter ( unsigned int fedSoftId, unsigned fedChannel, 
						      unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  fecErrorCounter = 0 ; fedErrorCounter = 0 ; psuErrorCounter = 0 ;
  bool found = false ;
  for (std::map<ConnectionDescription *, ErrorCounterStruct>::iterator it = errorCounters_.begin() ; (it != errorCounters_.end()) && !found ; it ++) {
    ConnectionDescription *connection = it->first ;

    if ( (fedSoftId == connection->getFedId() ) &&
 	 (fedChannel == connection->getFedChannel()) ) {
      
      fecErrorCounter += it->second.fecErrorCounter ; 
      fedErrorCounter += it->second.fedErrorCounter ; 
      psuErrorCounter += it->second.psuErrorCounter ; 
      found = true ;
    }
  }

  if (!found) {
    std::stringstream msgError ; 
    msgError << "No connection found for FED " << fedSoftId << " on channel " << fedChannel ;
    throw msgError.str() ;
  }


#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::getFedChannelErrorCounter took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}


/** Get the error counters for a given DET ID. This method accumulates all the errors for all connections
 * \param detId - detector geometrical identifier
 * \param fecErrorCounter - FEC number of errors for the corresponding hardware (returned parameter)
 * \param fedErrorCounter - FED number of errors for the corresponding hardware (returned parameter)
 * \param psuErrorCounter - PSU number of errors for the corresponding hardware (returned parameter)
 */
void TkDiagErrorAnalyser::getDetIdErrorCounter ( unsigned int detId, 
						 unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  fecErrorCounter = 0 ; fedErrorCounter = 0 ; psuErrorCounter = 0 ;
  if (detIdToConnection_.find(detId) != detIdToConnection_.end()) {

    fecErrorCounter = errorCounters_[detIdToConnection_[detId]].fecErrorCounter ;
    fedErrorCounter = errorCounters_[detIdToConnection_[detId]].fedErrorCounter ;
    psuErrorCounter = errorCounters_[detIdToConnection_[detId]].psuErrorCounter ;
  }
  else throw std::string("No connection for det Id " + toString(detId)) ;

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::getDetIdErrorCounter took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}

/** Get the error counters for a given PSU
 * \param psuName - name of the PSU (PVSS, datapoint or PSU name)
 * \param fecErrorCounter - FEC number of errors for the corresponding hardware (returned parameter)
 * \param fedErrorCounter - FED number of errors for the corresponding hardware (returned parameter)
 * \param psuErrorCounter - PSU number of errors for the corresponding hardware (returned parameter)
 */
void TkDiagErrorAnalyser::getPsuErrorCounter ( std::string psuName, 
					       unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) {

#ifdef DEBUGTIMING
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

  fecErrorCounter = 0 ; fedErrorCounter = 0 ; psuErrorCounter = 0 ;
  if (pvssNameToConnection_.find(psuName) != pvssNameToConnection_.end()) { // PVSS name
    
    fecErrorCounter = errorCounters_[pvssNameToConnection_[psuName]].fecErrorCounter ;
    fedErrorCounter = errorCounters_[pvssNameToConnection_[psuName]].fedErrorCounter ;
    psuErrorCounter = errorCounters_[pvssNameToConnection_[psuName]].psuErrorCounter ;
  }
  else if (dpNameToConnection_.find(psuName) != dpNameToConnection_.end()) { // DP name

    fecErrorCounter = errorCounters_[dpNameToConnection_[psuName]].fecErrorCounter ;
    fedErrorCounter = errorCounters_[dpNameToConnection_[psuName]].fedErrorCounter ;
    psuErrorCounter = errorCounters_[dpNameToConnection_[psuName]].psuErrorCounter ;
  }
  else if (psuNameToConnection_.find(psuName) != psuNameToConnection_.end()) { // PSU name

    fecErrorCounter = errorCounters_[psuNameToConnection_[psuName]].fecErrorCounter ;
    fedErrorCounter = errorCounters_[psuNameToConnection_[psuName]].fedErrorCounter ;
    psuErrorCounter = errorCounters_[psuNameToConnection_[psuName]].psuErrorCounter ;
  }
  else {
    throw std::string ("No connection for PSU name " + psuName) ;
  }

#ifdef DEBUGTIMING
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "TkDiagErrorAnalyser::getPsuErrorCounter took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
}


/** Get the connection sorted by FEC 
 * \return vector of connection description
 */
ConnectionVector &TkDiagErrorAnalyser::getConnectionByFec ( ) {

  return fecConnectionVector_ ;
}

/** Get the connection sorted by FED
 * \return vector of connection description
 */
ConnectionVector &TkDiagErrorAnalyser::getConnectionByFed ( ) {

  return fedConnectionVector_ ;
}

/** Get the connection by Det ID
 * \return vector of det id
 */
std::vector<unsigned int> &TkDiagErrorAnalyser::getListOfDetId ( ) {

  return detIdList_ ;
}

/** Get the PSU list sorted
 * \return vector of PSU name
 */
std::vector<std::string> &TkDiagErrorAnalyser::getListOfPVSSName ( ) {

  return pvssNameList_ ;
}

/** \brief Return the list of device sorted by device (crate,fec,ring,ccu,channel,address)
 */
deviceVector &TkDiagErrorAnalyser::getListOfDevices ( ) {

  return listVectorDevices_ ;
}

/** \brief Return the list of modules sorted by FEC/Ring/CCU/i2c channel
 */
ConnectionVector &TkDiagErrorAnalyser::getListOfModulesAsConnection ( ) {

  return listModulesAsConnection_ ;
}

/** Sort two elements of ConnectionDescription based on FEC crate/slot/ring/ccu/i2cchannel
 */
bool TkDiagErrorAnalyser::sortConnectionByFec ( ConnectionDescription *elt1, ConnectionDescription *elt2 ) {

  if (elt1->getFecCrateId() > elt2->getFecCrateId()) return false ;
  else if (elt1->getFecCrateId() < elt2->getFecCrateId()) return true ;
  else if (elt1->getFecSlot() > elt2->getFecSlot()) return false ;
  else if (elt1->getFecSlot() < elt2->getFecSlot()) return true ;
  else if (elt1->getRingSlot() > elt2->getRingSlot()) return false ;
  else if (elt1->getRingSlot() < elt2->getRingSlot()) return true ;
  else if (elt1->getCcuAddress() > elt2->getCcuAddress()) return false ;
  else if (elt1->getCcuAddress() < elt2->getCcuAddress()) return true ;
  else if (elt1->getI2cChannel() > elt2->getI2cChannel()) return false ;
  else if (elt1->getI2cChannel() < elt2->getI2cChannel()) return true ;
  else if (elt1->getApvAddress() > elt2->getApvAddress()) return false ;
  else if (elt1->getApvAddress() < elt2->getApvAddress()) return true ;
  else return false ;
}

/** Sort two elements of ConnectionDescription based on FED crate/slot/channel
 */
bool TkDiagErrorAnalyser::sortConnectionByFed ( ConnectionDescription *elt1, ConnectionDescription *elt2 ) {

  if (elt1->getFedCrateId() > elt2->getFedCrateId()) return false ;
  else if (elt1->getFedCrateId() < elt2->getFedCrateId()) return true ;
  else if (elt1->getFedSlot() > elt2->getFedSlot()) return false ;
  else if (elt1->getFedSlot() < elt2->getFedSlot()) return true ;
  else if (elt1->getFedChannel() > elt2->getFedChannel()) return false ;
  else if (elt1->getFedChannel() < elt2->getFedChannel()) return true ;
  else return false ;
}
