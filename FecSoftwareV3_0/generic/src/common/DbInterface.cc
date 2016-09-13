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

#include "DbInterface.h"

#define DEBUGMSGERROR

/** Constructor
 * \param allFecDevices - download all devices even the disabled devices
 * \param login - login on the database (if nothing is specified then the database is taken from CONFDB if existing)
 * \param passwd - password on the database (if nothing is specified then the database is taken from CONFDB if existing)
 * \param path - path on the database (if nothing is specified then the database is taken from CONFDB if existing)
 * \param threaded - to have or not shared connection (threaded = true then no shared connection)
 */
DbInterface::DbInterface( bool allFecDevices, bool allConnections, std::string login, 
			  std::string passwd, std::string path, bool threaded ) {

  if ( (login == "nil") || (passwd == "nil") || (path == "nil") ) {
    DeviceFactoryInterface::getDatabaseConfiguration(login,passwd,path);
  }

  if ( (login != "nil") && (passwd != "nil") && (path != "nil") ) 
    deviceFactory_ = new DeviceFactory(login,passwd,path,threaded) ; // for database
  else
    deviceFactory_ = new DeviceFactory () ;

  // initially we set using strips to true, if the user wants to ignore strips he must explicity set it to false
  deviceFactory_->setUsingStrips ( true );
  // same goes for using binary strips!
  deviceFactory_->setUsingBinaryStrips ( true );
    
  fecMajorVersion_ = fecMinorVersion_ = 0 ;
  fedMajorVersion_ = fedMinorVersion_ = 0 ;
  detIdMajorVersion_ = detIdMinorVersion_ = 0 ;
  connectionMajorVersion_ = connectionMinorVersion_ = 0 ;
  maskMajorVersion_ = maskMinorVersion_ = 0 ;
  partitionName_ = "" ;
  partitionDetIdName_ = "" ;
  redundancyFileName_ = "" ;
  fecFileName_ = "" ;
  detIdFileName_ = "" ;
  fedFileName_ = "" ;
  connectionFileName_ = "" ;
  allFecDevices_ = allFecDevices ;
  allConnections_ = allConnections ;
  partitionFEDDownload_ = false ;
}

/** Delete the attributs of the class
 */
DbInterface::~DbInterface() {

  try {
    // Delete all FEDs
    clearFEDDescriptions() ;

    // Delete the database connection that delete all related vectors
    delete deviceFactory_ ;
  }
  catch (FecExceptionHandler &e) {
    //#ifdef DEBUGMSGERROR
    std::cerr << "Error during the delete of the database (FecExceptionHandler): " << e.what() << std::endl ;
    //#endif
  }
  catch (oracle::occi::SQLException &e) {
    //#ifdef DEBUGMSGERROR
    std::cerr << "Error during the delete of the database (SQLException): " << e.what() << std::endl ;
    //#endif
  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
    //#ifdef DEBUGMSGERROR
    std::cerr << "Error during the delete of the database (Fed9UDeviceFactoryException): " << e.what() << std::endl ;
    //#endif
  }
  catch (ICUtils::ICException &e) {
    //#ifdef DEBUGMSGERROR
    std::cerr << "Error during the delete of the database (ICException): " << e.what() << std::endl ;
    //#endif
  }
  catch (exception &e) {
    //#ifdef DEBUGMSGERROR
    std::cerr << "Error during the delete of the database (exception): " << e.what() << std::endl ;
    //#endif
  }
  catch (...) { //Catch all types of exceptions
    //#ifdef DEBUGMSGERROR
    std::cerr << "Error during the delete of the database: Unknown excception" << std::endl ;
    //#endif
  }
}

/* ********************************************************************************************** */
/*                                                                                                */
/* Some parameters for the class                                                                  */
/*                                                                                                */
/* ********************************************************************************************** */

/** Set if you all devices or only the enabled should be downloaded
 * \param allFecDevices - true if all the devices should be downloaded (including the disable devices (FEC or PIA))
 */
void DbInterface::setAllDevices ( bool allFecDevices ) {
  allFecDevices_ = allFecDevices ;
}

/** Return if all the devices have been download or not
 * \return false if only the enabled one have been downloaded
 */
bool DbInterface::getAllDevices ( ) {
  return allFecDevices_ ;
}

/** Set if you all devices or only the enabled should be downloaded
 * \param allConnections - true if all the devices should be downloaded (including the disable devices (FEC or PIA))
 */
void DbInterface::setAllConnections ( bool allConnections ) {
  allConnections_ = allConnections ;
}

/** Return if all the devices have been download or not
 * \return false if only the enabled one have been downloaded
 */
bool DbInterface::getAllConnections ( ) {
  return allConnections_ ;
}

/** Return the access to the database
 * do not delete this factory or you kill this interface (segfault on any access)
 */
DeviceFactory *DbInterface::getDeviceFactory ( ) {
  return deviceFactory_ ;
}

/* ********************************************************************************************** */
/*                                                                                                */
/* Get the current devices / connections / redundancy / FED / Det id                              */
/*                                                                                                */
/* ********************************************************************************************** */

/** Return the different values
 */
deviceVector DbInterface::getCurrentDevices() { return currentFecDevices_ ; }

/** Return the pia values
 */
piaResetVector DbInterface::getCurrentPia() { return currentPiaReset_ ; }

/** Return the connections values
 */
ConnectionVector DbInterface::getConnections() { return connectionVector_ ; }

/** Return the redundancy values
 */
tkringVector DbInterface::getFecRedundancy() { return tkRingDescriptions_ ; }

/** Return the list of det id downloaded
 */
Sgi::hash_map<unsigned long, TkDcuInfo *> DbInterface::getDetIdList() { return deviceFactory_->getInfos() ; }

/** Return the error message
 */
std::string DbInterface::getErrorMessage( ) { return errorMessage_ ; }

/* ********************************************************************************************** */
/*                                                                                                */
/* Clear the maps and vectors                                                                     */
/*                                                                                                */
/* ********************************************************************************************** */

/** Clear the FED descriptions
 */
inline void DbInterface::clearFEDDescriptions ( ) {

//   for (Fed9U::Fed9UHashMapType::iterator it = theFed9UDescription_.begin() ; it != theFed9UDescription_.end() ; it ++) {
//     //std::cerr << "DbInterface: delete ptr " << std::hex << it->second << std::endl ; 
//     if (it->second != NULL) delete (it->second) ;
//   }

  fedMajorVersion_ = fedMinorVersion_ = 0 ;
  partitionFEDDownload_ = false ;
  fedFileName_ = "" ;

   for (std::vector<Fed9U::Fed9UDescription*>::iterator it = fedToBeDeleted_.begin() ; it != fedToBeDeleted_.end() ; it ++) {
     //std::cerr << "delete ptr " << std::hex << *it << std::endl ; 
     delete *it ;
   }
    
   fedToBeDeleted_.clear() ;
   theFed9UDescription_.clear() ;
}

/** Clear the FEC descriptions (PIA and devices)
 */
void DbInterface::clearFECDeviceDescriptions ( ) {

  currentFecDevices_.clear() ;
  fecMajorVersion_ = fecMinorVersion_ = 0 ;
  fecFileName_ = "" ;
}

/** Clear the PIA reset description (only PIA) 
 */
void DbInterface::clearPiaResetDescriptions ( ) {
  currentPiaReset_.clear() ;
  fecFileName_ = "" ;
}

/** Clear the connection descriptions
 */
void DbInterface::clearConnectionDescriptions ( ) {
  connectionVector_.clear() ;
  connectionMajorVersion_ = connectionMinorVersion_ = 0 ;
  connectionFileName_ = "" ;
}

/** Clear the vector of ring descriptions
 */
void DbInterface::clearFecRedundancyDescriptions ( ) {
  tkRingDescriptions_.clear() ;
  redundancyFileName_ = "" ;
}

/** Clear the DET ID descriptions 
 */
void DbInterface::clearDetIdDescriptions ( ) {

  detIdMajorVersion_ = detIdMinorVersion_ = 0 ;
  detIdFileName_ = "" ;
  deviceFactory_->deleteHashMapTkDcuInfo() ;
}

/* ********************************************************************************************** */
/*                                                                                                */
/* Partition / version                                                                              */
/*                                                                                                */
/* ********************************************************************************************** */
  
/** Retreive the FED version for the partition specified
 * \param partitionName - partition name
 * \param major - output for the major version
 * \param minor - output for the minor version
 * \param maskVersionMajor - output for the mask major version
 * \param maskVersionMinor - output for the mask minor version
 * \param partitionNumber - output partition ID in the database
 */
void DbInterface::getFedPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *maskVersionMajor, unsigned int *maskVersionMinor, unsigned int *partitionNumber ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

  tkStateVector dbStates = deviceFactory_->getCurrentStates () ;
  tkStateVector::iterator itState = dbStates.begin() ;
  while (itState != dbStates.end() && (*itState)->getPartitionName() != partitionName) itState ++ ;
  if (itState != dbStates.end()) {
    *major = (*itState)->getFedVersionMajorId() ;
    *minor = (*itState)->getFedVersionMinorId() ;
    *maskVersionMajor = (*itState)->getMaskVersionMajorId() ;
    *maskVersionMinor = (*itState)->getMaskVersionMinorId() ;
  }

#ifdef DBSTATE
  DbFedAccess *dbAccess = ((Fed9U::Fed9UDeviceFactory *)deviceFactory_)->getDatabaseAccess() ;
  std::list<unsigned int*> partitionVersion = (((Fed9U::Fed9UDeviceFactory *)deviceFactory_)->getDatabaseAccess())->getDatabaseVersion ( partitionName ) ;

  if (partitionVersion.size() > 0) {
    if (partitionVersion.size() > 1) {
#ifdef DEBUGMSGERROR
      std::cerr << "Several versions for the same partition in the current state, incoherence in the database for the partition " << partitionName << std::endl ;
#endif 
      errorMessage_ = "Several versions for the same partition in the current state, incoherence in the database for the partition " + partitionName;
    }
    unsigned int *value = *partitionVersion.begin() ;
    *partitionNumber = value[0] ;
    *major = value[1] ;
    *major = value[2] ;
    *maskVersionMajor = value[3] ;
    *maskVersionMinor = value[4] ;

    for (std::list<unsigned int*>::iterator it = partitionVersion.begin() ; it != partitionVersion.end() ; it ++) {
      value = *it;
      delete[] value ;
    }
  }
  else {
    *major = *minor = *partitionNumber = 0 ;
#ifdef DEBUGMSGERROR
    std::cerr << "No version for the partition " << partitionName << std::endl ;
#endif 
  }
#endif
}

/* ********************************************************************************************** */
/*                                                                                                */
/* Download / upload of the redundancy                                                            */
/*                                                                                                */
/* ********************************************************************************************** */

/** Download the FEC redundancy for a partition
 * \param partitionName - partition name
 * \warning this method download each time from the database
 */
int DbInterface::downloadFecRedundancyFromDatabase ( std::string partitionName ) {
    
  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  bool downloadCCUDone = false ;
  if (partitionRedundancyName_ == partitionName) downloadCCUDone = true ;

  // If data not found then go to the database
  if (!downloadCCUDone) {

    clearFecRedundancyDescriptions ( ) ;
      
#ifdef DEBUGMSGERROR
    std::cout << "--> Download from the database since no version for the FEC/Ring/CCU redundancy" << std::endl ;
#endif

    try {
#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      tkRingDescriptions_ = deviceFactory_->getDbRingDescription ( partitionName ) ;

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of " << tkRingDescriptions_.size() <<  " rings from the database for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      partitionRedundancyName_ = partitionName ;
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FEC devices for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
      errorMessage_ =  "Unable to download the FEC devices for the partition " + partitionName + ": " + e.what() ;
      return -2 ;
    }
    catch (oracle::occi::SQLException &e) {
      errorMessage_ = deviceFactory_->what("Unable to download the FEC devices for the partition (Oracle exception) " + partitionName, e) ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif

      return -3 ;
    }
#ifdef DEBUGMSGERROR
  }
  else {
    std::cout << "Redundancy already downloaded for partition " << partitionName << std::endl ;
  }
#endif 

  return 0 ;
}

/** Download the FEC redundancy from a file
 * \param filename - file name for the devices
 * \param changed - is any changed occurs
 * \param redownload - re-download it
 * \param addIt - add to the current vector if true, if false, delete the previous one(s)
 */
int DbInterface::downloadFecRedundancyFile ( std::string filename, bool &changed, bool redownload, bool addIt ) {

  changed = false ;

  if ( (filename != redundancyFileName_) || redownload ) {

    bool db = deviceFactory_->getDbUsed() ;
    try {

      // clear the previous data
      if (!addIt) clearFecRedundancyDescriptions () ;

      // changed
      changed = true ;

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      deviceFactory_->setUsingFile() ;
      tkringVector v = deviceFactory_->getFileRingDescription ( filename ) ;
      if (addIt) {
	for (tkringVector::iterator it = v.begin() ; it != v.end() ; it ++) {
	  tkRingDescriptions_.push_back(*it) ;
	}
      }
      else tkRingDescriptions_ = v ;

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Parsing of the file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
      redundancyFileName_ = filename ;
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cout << "Error of the download of redundancy from file " << filename << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Error of the download of redundancy from file " + filename + ": " + e.what() ;

      fecFileName_ = "" ;
      return -2 ;
    }

    if (db) deviceFactory_->setUsingDb() ;
  }

  return 0 ;
}

/** Upload data to the database
 * \param partitionName - partition name to be used
 * \warning the ring are uploaded one by one
 */
int DbInterface::uploadFecRedundancyToDb(std::string partitionName) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }
    
  try {

    // FEC devices
    if (tkRingDescriptions_.size()) {

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      // Upload the different rings
      for (tkringVector::iterator it = tkRingDescriptions_.begin() ; it != tkRingDescriptions_.end() ; it ++) {
	  
	TkRingDescription *ring = *it ;
	deviceFactory_->setDbRingDescription ( partitionName, *ring) ;
      }
	    
#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << tkRingDescriptions_.size() << " rings have been uploaded in partition " << partitionName << " and it took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
	
      partitionName_ = partitionName ;
    }
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the FEC devices for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the FEC devices for the partition " + partitionName + ": " + e.what() ;

    return -2 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to upload the FEC devices for the partition (Oracle exception) " + partitionName, e) ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif

    return -3 ;
  }

  return 0 ;
}

/** Upload the current data to a file
 * \param filename - file name used for the creation of the file
 * \param listOfFiles - is the real list of files produced
 * \warning each of the ring will be uploaded in a different files (filename.fechardwareid.xml)
 */
int DbInterface::uploadFecRedundancyToFile(std::string filename, std::list<std::string> &listOfFiLes) {

  std::string filenamefec ;
  std::string::size_type ipass = filename.find(".xml") ;
  if (ipass != std::string::npos) filenamefec = filename.substr(0,ipass) ;
  else filenamefec = filename ;

  try {

#ifdef DEBUGMSGERROR
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

    // Upload the different rings
    for (tkringVector::iterator it = tkRingDescriptions_.begin() ; it != tkRingDescriptions_.end() ; it ++) {
	
      TkRingDescription *ring = *it ;

      std::string fileFec = filenamefec + "_" + ring->getFecHardwareId() + "_Ring_" + toString(ring->getRingSlot()) + ".xml" ;

      deviceFactory_->setFileRingDescription ( fileFec, *ring ) ;

      listOfFiLes.push_back(fileFec) ;
    }

#ifdef DEBUGMSGERROR
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec << "Upload of " << tkRingDescriptions_.size() << " rings in file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the FEC devices in a file: " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the FEC devices in a file: " + e.what() ;
      
    return -2 ;
  }
    
  return 0 ;
}

/* ********************************************************************************************** */
/*                                                                                                */
/* Download / upload of the FEC/PIA devices                                                       */
/*                                                                                                */
/* ********************************************************************************************** */

/** Download the FEC devices
 * \param partitionName - partition name
 * \param changed - true if the FEC devices (not the PIA reset) have been changed
 * \param fecMajor - FEC device major version
 * \param fecMinor - FEC device minor version
 */
int DbInterface::downloadFECDevicesFromDatabase ( std::string partitionName, bool &changed, unsigned int fecMajor, unsigned int fecMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor ) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  changed = true ;
  bool downloadFECDone = false ;

#ifdef DEBUGMSGERROR
  std::cout << std::dec << "Current FEC version is: " << fecMajorVersion_ << "." << fecMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << " for the partition " << partitionName_  << "/" << partitionName << std::endl ;
#endif

  // FEC device version
  if ( (partitionName_ == partitionName) && (fecMajor != 0) ) {
    if ( (fecMajor == fecMajorVersion_) && (fecMinor == fecMinorVersion_) && (maskMajorVersion_ == maskMinorVersion_) && (maskMinorVersion_ == maskVersionMinor) ) {
#ifdef DEBUGMSGERROR
      std::cout << std::dec << "--> Download already done for the partition " << partitionName_ << " version " << fecMajorVersion_ << "." << fecMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << std::endl;
#endif
      downloadFECDone = true ;
      changed = false ;
    }
  }

  try {
    // Check the version in the database
    if ( (!downloadFECDone) && (fecMajorVersion_ != 0) && (partitionName == partitionName_) ) { // one version has been already downloaded for this partition

      try {
	// Retreive the current FEC version for this partition
	unsigned int major, minor, maskMajor, maskMinor, partitionId ;
	((FecDeviceFactory *)deviceFactory_)->getPartitionVersion ( partitionName, &major, &minor, &maskMajor, &maskMinor, &partitionId ) ;
      
	// Same version ?
	if ( (major == fecMajorVersion_) && (minor == fecMinorVersion_) && (maskMajorVersion_ == maskMajor) && (maskMinorVersion_ == maskMinor) ) {
#ifdef DEBUGMSGERROR
	  std::cout << std::dec << "--> Download already done for the partition " << partitionName_ << " version " << fecMajorVersion_ << "." << fecMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << std::endl;
#endif
	  downloadFECDone = true ;
	  changed = false ;
	}
#ifdef DEBUGMSGERROR
	else {
	  std::cout << "Version has been changed, download from the database" << std::endl ;
	}
#endif
      }
      catch (FecExceptionHandler &e) {	// perhaps it is not in the current state so plan to re-download it

#ifdef DEBUGMSGERROR
	std::cout << "The partition is not in the current state" << std::endl ;
#endif

	changed = true ;
	downloadFECDone = false ;
      }
    }
      
    // If data not found then go to the database
    if (!downloadFECDone) {
	
      clearFECDeviceDescriptions ( ) ;

#ifdef DEBUGMSGERROR
      std::cout << "--> Download from database" << std::endl ;
#endif
	
#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
	
      deviceFactory_->getFecDeviceDescriptions ( partitionName, currentFecDevices_, fecMajor, fecMinor, maskVersionMajor, maskVersionMinor, allFecDevices_) ;

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of " << currentFecDevices_.size() << " devices from the database for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      // Retreive the current FEC version for this partition
      partitionName_ = partitionName ;
      if (fecMajor != 0) {
	fecMajorVersion_ = fecMajor ;
	fecMinorVersion_ = fecMinor ;
	maskMajorVersion_ = maskVersionMajor ;
	maskMinorVersion_ = maskVersionMinor ;
      }
      else {
	unsigned int partitionId ;
	((FecDeviceFactory *)deviceFactory_)->getPartitionVersion ( partitionName, &fecMajorVersion_, &fecMinorVersion_, &maskVersionMajor, &maskVersionMinor, &partitionId ) ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FEC devices for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FEC devices for the partition " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fecMajorVersion_ = fecMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
      
    return -2 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to download the FEC devices for the partition (Oracle exception) " + partitionName, e) ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif
    partitionName_ = "" ; 
    fecMajorVersion_ = fecMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;

    return -3 ;
  }

  return 0 ;
}

/** Download the FEC devices
 * \param partitionName - partition name
 * \warning this method download each time from the database
 */
int DbInterface::downloadPIAResetFromDatabase ( std::string partitionName ) {
    
  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  bool downloadPIADone = false ;

  // If data not found then go to the database
  if (!downloadPIADone) {

    clearPiaResetDescriptions ( ) ;
      
#ifdef DEBUGMSGERROR
    std::cout << "--> Download from the database since no version for the PIA" << std::endl ;
#endif

    try {
#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      deviceFactory_->getPiaResetDescriptions ( partitionName, currentPiaReset_, allFecDevices_) ;

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of " << currentPiaReset_.size() <<  " PIA reset from the database for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FEC devices for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
      errorMessage_ =  "Unable to download the FEC devices for the partition " + partitionName + ": " + e.what() ;
      return -2 ;
    }
    catch (oracle::occi::SQLException &e) {
      errorMessage_ = deviceFactory_->what("Unable to download the FEC devices for the partition (Oracle exception) " + partitionName, e) ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif
      return -3 ;
    }
  }

  return 0 ;
}

/** download the FEC data for a given partition name and version
 * \param partitionName - partition name
 * \param changed - true if the FEC devices (not the PIA reset) have been changed
 * \param fecMajor - FEC device major version
 * \param fecMinor - FEC device minor version
 * \param piaDownload - download the PIA reset (should be done once)
 */
int DbInterface::downloadFECFromDatabase(std::string partitionName, bool &changed, unsigned int fecMajor, unsigned int fecMinor, unsigned int maskVersionMajor, unsigned maskVersionMinor, bool piaDownload ) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  int error = 0 ;
  if (piaDownload) error = downloadPIAResetFromDatabase (partitionName) ;
  error += downloadFECDevicesFromDatabase(partitionName,changed,fecMajor,fecMinor, maskVersionMajor, maskVersionMinor) ;

  return error ;
}

/** Download the FEC devices and the PIA reset from a file
 * \param filename - file name for the devices
 * \param changed - is any changed occurs
 * \param redownload - re-download it
 */
int DbInterface::downloadFECFromFile (std::string filename, bool &changed, bool redownload ) {

  changed = false ;

  if ( (filename != fecFileName_) || redownload ) {

    bool db = deviceFactory_->getDbUsed() ;
    try {

      changed = true ;

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      deviceFactory_->setUsingFile() ;
      deviceFactory_->setFecInputFileName(filename) ;

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Parsing of the file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      // Retreive the values for PIA reset
      clearPiaResetDescriptions() ;
      deviceFactory_->getPiaResetDescriptions (currentPiaReset_,allFecDevices_) ;
      clearFECDeviceDescriptions() ;
      deviceFactory_->getFecDeviceDescriptions (currentFecDevices_,allFecDevices_) ;

      fecFileName_ = filename ;
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cout << "Error of the download of FEC/PIA from file " << filename << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Error of the download of FEC/PIA from file " + filename + ": " + e.what() ;

      fecFileName_ = "" ;
      return -2 ;
    }

    if (db) deviceFactory_->setUsingDb() ;
  }
  return 0 ;
}

/** Upload data to the database
 * \param partitionName - partition name to be used
 * \param isMajor - upload a major (true) or a minor (false) version 
 * \param fecVersionMajor - if not NULL then the major version is returned
 * \param fecVersionMinor - if not NULL then the minor version is returned
 */
int DbInterface::uploadFec(std::string partitionName, bool ismajor, bool piaUpload, 
	      unsigned int *fecVersionMajor, unsigned int *fecVersionMinor ) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  try {

    // FEC devices
    if (currentFecDevices_.size()) {

      unsigned int major,minor;

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      // Upload the devices
      deviceFactory_->setFecDeviceDescriptions (deviceFactory_->getFecDevices(), partitionName, &major, &minor, ismajor, false) ;
	    
#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Version " << major << "." << minor << " has been uploaded for " << currentFecDevices_.size() << " in partition " << partitionName << " and it took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
	
      if (fecVersionMajor != NULL) *fecVersionMajor = major ;
      if (fecVersionMinor != NULL) *fecVersionMajor = minor ;

      // set the version as to be used
      fecMajorVersion_ = major ;
      fecMinorVersion_ = minor ;
      partitionName_ = partitionName ;
    }

    // PIA reset
    if (currentPiaReset_.size() && piaUpload) {

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      // Update PIA resets
      deviceFactory_->setPiaResetDescriptions (currentPiaReset_, partitionName) ;

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Update of " << currentPiaReset_.size() << " PIA resets in partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
    }
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the FEC devices for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the FEC devices for the partition " + partitionName + ": " + e.what() ;

    return -2 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ =  "Unable to upload the FEC devices for the partition (Oracle exception) " + partitionName + ": " + e.what() ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif
    return -3 ;
  }

  return 0 ;
}

/** Upload the current data to a file
 * \param filename - output file
 * \warning PIA and FEC will be uploaded in the file
 */
int DbInterface::uploadFec(std::string filename) {

  try {

    // Retreive all the devices even the one disabled
    deviceVector vDevices = deviceFactory_->getFecDevices() ;
    piaResetVector pDevices = deviceFactory_->getPiaResets() ;

    FecFactory deviceFactory ;
    deviceFactory.setOutputFileName(filename);

#ifdef DEBUGMSGERROR
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

    if (vDevices.size() && pDevices.size())
      deviceFactory.setFecDevicePiaDescriptions(vDevices,pDevices) ;
    else if (pDevices.size()) 
      deviceFactory.setPiaResetDescriptions(pDevices) ;
    else if (vDevices.size())
      deviceFactory.setFecDeviceDescriptions(vDevices) ;
    else {	
#ifdef DEBUGMSGERROR
      std::cout << "FEC and PIA vectors are empty, cannot upload it in file" << std::endl ;
#endif
      errorMessage_ = "FEC and PIA vectors are empty, cannot upload it in file" ;
      return -1 ;
    }

#ifdef DEBUGMSGERROR
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec << "Upload of " << vDevices.size() << " devices in file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the FEC devices in a file: " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the FEC devices in a file: " + e.what() ;
      
    return -2 ;
  }
    
  return 0 ;
}

/* ********************************************************************************************** */
/*                                                                                                */
/* Download / upload of FED                                                                       */
/*                                                                                                */
/* ********************************************************************************************** */

/** Download a specific FED from the database from partition
 * \param partitionName - partition name
 * changed - true if the FED descriptions have been changed
 * \param major - major version
 * \param minor - minor version
 * \warning the output is cloned so you must delete it when you finish with it
 */
std::vector<Fed9U::Fed9UDescription*> DbInterface::downloadFEDFromDatabase ( std::string partitionName, bool &changed, unsigned int fedMajor, unsigned int fedMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor) {

  //std::cout << __PRETTY_FUNCTION__ << ": begin" << std::endl ;

  errorMessage_ = "" ;

  std::vector<Fed9U::Fed9UDescription*> fedVector ;

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return fedVector ;
  }

  bool downloadFEDDone = false ;
  changed = true ;
  partitionFEDDownload_ = false ;

  try {
      
#ifdef DEBUGMSGERROR
    if (partitionFEDDownload_)
      std::cout << std::dec << "--> Current FED version is: " << fedMajorVersion_ << "." << fedMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << " for the partition " << partitionName_ << "/" << partitionName << std::endl ;
    else 
      if (theFed9UDescription_.size()) std::cout << "--> Download should be redone, perhaps one of the FED was not downloaded" << std::endl ;
#endif
      
    // already downloaded 
    if ( partitionFEDDownload_ && (partitionName_ == partitionName) ) {
	
      if (fedMajor != 0) {
	if ( (fedMajor == fedMajorVersion_) && (fedMinor == fedMinorVersion_) && (maskMajorVersion_ == maskVersionMajor) && (maskMinorVersion_ == maskVersionMinor) ) {
#ifdef DEBUGMSGERROR
	  std::cout << std::dec << "--> Download already done for the partition " << partitionName_ << " version " << fedMajorVersion_ << "." << fedMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << std::endl;
#endif
	  downloadFEDDone = true ;
	  changed = false ;
	}
      }
    }

    if ( !downloadFEDDone && (fedMajorVersion_ != 0) && (partitionName_ == partitionName) ) { // one version has been already downloaded for that partition

      try {
	unsigned int major = 0, minor = 0, maskMajor = 0, maskMinor = 0, partitionId = 0 ;
	getFedPartitionVersion (partitionName,&major,&minor,&maskMajor,&maskMinor,&partitionId) ;
	
	if ( (fedMajorVersion_ == major) && (fedMinorVersion_ == minor) && (maskMajorVersion_ == maskMajor) && (maskMinorVersion_ == maskMinor) ) {
#ifdef DEBUGMSGERROR
	  std::cout << std::dec << "--> Download already done for the partition " << partitionName_ << " version " << fedMajorVersion_ << "." << fedMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << std::endl;
#endif
	  downloadFEDDone = true ;
	  changed = false ;
	}
#ifdef DEBUGMSGERROR
	else {
	  std::cout << "--> Version has been changed for the partition " << partitionName << ", download from the database" << std::endl ;
	}
#endif
      }
      catch (FecExceptionHandler &e) {	// perhaps it is not in the current state so plan to re-download it

#ifdef DEBUGMSGERROR
	std::cout << "The partition is not in the current state" << std::endl ;
#endif

	changed = true ;
	downloadFEDDone = false ;
      }
    }

    // Build the vector for the output
    if (downloadFEDDone) {

      for (Fed9U::Fed9UHashMapType::iterator it = theFed9UDescription_.begin() ; it != theFed9UDescription_.end() ; it ++) {
	if (it->second != NULL) {
	  fedVector.push_back(it->second) ;
	}
      }
    }
    // Download from database has to be done
    else if (!downloadFEDDone) {

      changed = true ;

      // Delete the previous download
      clearFEDDescriptions () ;

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      //std::cout << fedVector << std::endl ;
      // Never delete the ptrVector ... it is managed by the factory
      std::vector<Fed9U::Fed9UDescription*> *ptrVector ;

      if ( (fedMajor == 0) && (fedMinor == 0) )
	ptrVector = deviceFactory_->getFed9UDescriptions(partitionName) ;
      else {
	std::cerr << "-------------> This method should be changed : " << __PRETTY_FUNCTION__ << " at line " << __LINE__ << " to take care about the mask" << std::endl ;
	//ptrVector = deviceFactory_->getFed9UDescriptions(partitionName, fedMajor, fedMinor, maskVersionMajor, maskVersionMinor) ;
	ptrVector = deviceFactory_->getFed9UDescriptions(partitionName, fedMajor, fedMinor) ;
      }

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of " << ptrVector->size() << " FEDs from the database for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      //std::cout << fedVector << std::endl ;
      if (ptrVector != NULL) {

	// clone and add to the list
	for (std::vector<Fed9U::Fed9UDescription *>::iterator it = ptrVector->begin() ; it != ptrVector->end() ; it ++) {
	  if (*it != NULL) {
	    theFed9UDescription_[(*it)->getFedId()] = (*it) ; // (*it)->clone() ; 
	    //fedToBeDeleted_.push_back(theFed9UDescription_[fedid]) ; => not used since we are not cloning the method
	    fedVector.push_back(theFed9UDescription_[(*it)->getFedId()]) ;
	  }
	}
   
	// Partition
	partitionName_ = partitionName ;
	partitionFEDDownload_ = true ;
	if (fedMajor != 0) {
	  fedMajorVersion_ = fedMajor ;
	  fedMinorVersion_ = fedMinor ;
	  maskMajorVersion_ = maskVersionMajor ;
	  maskMinorVersion_ = maskVersionMinor ;
	}
	else {
	  // retrieve the version
	  unsigned int major = 0, minor = 0, maskMajor = 0, maskMinor = 0, partitionId = 0 ;
	  getFedPartitionVersion (partitionName_, &major, &minor, &maskMajor, &maskMinor, &partitionId) ;
	  fedMajorVersion_ = major ;
	  fedMinorVersion_ = minor ;
	  maskMajorVersion_ = maskMajor ;
	  maskMinorVersion_ = maskMinor ;
	}
      }
      else {
#ifdef DEBUGMSGERROR
	std::cerr << "No data found for the partition " << partitionName << std::endl ;
#endif
	fedMajorVersion_ = fedMinorVersion_ = 0 ;
	maskMajorVersion_ = maskMinorVersion_ = 0 ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;

    return fedVector ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to download the FEC descriptions for the partition (Oracle exception) " + partitionName, e) ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif

    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
  
    return fedVector ;
  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (FED9U exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (FED9U exception) " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0  ;
     
    return fedVector ;
  }
  catch (ICUtils::ICException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (ICUtils exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (FED9U exception) " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
      
    return fedVector ;
  }
  catch (exception &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (exception) " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;

    return fedVector ;
  }
  catch (...) { //Catch all types of exceptions
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (no specific exception) " << partitionName << ": " <<  __FILE__  << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (no specific exception) " + partitionName ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
  
    return fedVector ;
  }

  if (fedVector.size() == 0) { fedMajorVersion_ = fedMinorVersion_ = 0 ; maskMajorVersion_ = maskMinorVersion_ = 0 ; }
    
  return fedVector ;
}
  
/** Download a specific FED from the database
 * \param fedid - the corresponding FED
 * \param partitionName - partition name
 * \param changed - true if the FED descriptions have been changed
 * \param major - major version
 * \param minor - minor version
 */
Fed9U::Fed9UDescription* DbInterface::downloadFEDFromDatabase (int fedid, std::string partitionName, bool &changed, unsigned int fedMajor, unsigned int fedMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor ) {

  errorMessage_ = "" ;

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return NULL ;
  }

  bool downloadFEDDone = false ;
  changed = true ;

#ifdef DEBUGMSGERROR
  std::cout << std::dec << "Current FED version is: " << fedMajorVersion_ << "." << fedMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << " for the partition " << partitionName_ << "/" << partitionName << " for FED " << fedid ;
  if (theFed9UDescription_.find(fedid) != theFed9UDescription_.end()) std::cout << std::dec << " / " << theFed9UDescription_[fedid]->getFedId() << std::endl ;
  else std::cout << " (no FED found for that id)" << std::endl ;
#endif

  try {

    // already downloaded 
    if ( (partitionName_ == partitionName) && (theFed9UDescription_.find(fedid) != theFed9UDescription_.end()) && (theFed9UDescription_[fedid]->getFedId() == fedid) ) {
      
      if (fedMajor != 0) {
	if ( (fedMajor == fedMajorVersion_) && (fedMinor == fedMinorVersion_) && (maskMajorVersion_ == maskVersionMajor) && (maskMinorVersion_ == maskVersionMinor) ) {
#ifdef DEBUGMSGERROR
	  std::cout << std::dec << "--> Download already done for the partition " << partitionName_ << " version " << fedMajorVersion_ << "." << fedMinorVersion_ << " for FED " << fedid << std::endl;
#endif
	  downloadFEDDone = true ;
	  changed = false ;
	}
      }
      else if (fedMajorVersion_ != 0) {

	try {
	  unsigned int major = 0, minor = 0, maskMajor = 0, maskMinor = 0, partitionId = 0 ;
	  getFedPartitionVersion (partitionName, &major, &minor, &maskMajor, &maskMinor, &partitionId ) ;
	  if ( (fedMajorVersion_ == major) && (fedMinorVersion_ == minor) && (maskMajorVersion_ == maskMajor) && (maskMinorVersion_ == maskMinor) ) {
	    
#ifdef DEBUGMSGERROR
	    std::cout << std::dec << "--> Download already done for the partition " << partitionName_ << " version " << fedMajorVersion_ << "." << fedMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << " for FED " << fedid << std::endl;
#endif
	    downloadFEDDone = true ;
	    changed = false ;
	  }
	}
	catch (FecExceptionHandler &e) {	// perhaps it is not in the current state so plan to re-download it

#ifdef DEBUGMSGERROR
	  std::cout << "The partition is not in the current state" << std::endl ;
#endif
	  
	  changed = true ;
	  downloadFEDDone = false ;
	}
      }
    }
      
    // Download from database has to be done
    if (!downloadFEDDone) {
	
      // Delete the previous download
      delete theFed9UDescription_[fedid] ;
      theFed9UDescription_[fedid] = NULL ;
	
#ifdef DEBUGMSGERROR
      std::cout << "--> Download from the database (FED)" << std::endl ;
#endif
	  
#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
	
      // Software ID
      std::cerr << "-------------> This method should be changed : " << __PRETTY_FUNCTION__ << " at line " << __LINE__ << " to take care about the mask" << std::endl ;
      //Fed9U::Fed9UDescription f = deviceFactory_->getFed9UDescription(fedid,false,partitionName,fedMajor,fedMinor,maskVersionMajor,maskVersionMinor);
      Fed9U::Fed9UDescription f = deviceFactory_->getFed9UDescription(fedid,false,partitionName,fedMajor,fedMinor);
	  
#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of the FED " << fedid << " from the database for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
	
      theFed9UDescription_[fedid] = new Fed9U::Fed9UDescription(f); // Should be deleted by the interface
      fedToBeDeleted_.push_back(theFed9UDescription_[fedid]) ;

      partitionName_ = partitionName ;
      partitionFEDDownload_ = false ;
      if (fedMajor != 0) {
	fedMajorVersion_ = fedMajor ;
	fedMinorVersion_ = fedMinor ;
	  maskMajorVersion_ = maskVersionMajor ;
	  maskMinorVersion_ = maskVersionMinor ;
      }
      else {
	// retrieve the version
	unsigned int major = 0, minor = 0, maskMajor = 0, maskMinor = 0, partitionId = 0 ;
	getFedPartitionVersion (partitionName_,&major,&minor,&maskMajor,&maskMinor,&partitionId) ;
	fedMajorVersion_ = major ;
	fedMinorVersion_ = minor ;
	maskMajorVersion_ = maskMajor ;
	maskMinorVersion_ = maskMinor ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition " << partitionName << " and the FED " << fedid << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition " + partitionName + " and the FED " + toString(fedid) + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;

    return NULL ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to download the FEC descriptions for the partition (Oracle exception) " + partitionName + " and the FED " + toString(fedid), e) ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif

    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
      
    return NULL ;
  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (FED9U exception) " << partitionName << " and the FED " << fedid << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (FED9U exception) " + partitionName + " and the FED " + toString(fedid) + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
  
    return NULL ;
  }
  catch (ICUtils::ICException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (ICUtils exception) " << partitionName << " and the FED " << fedid << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (FED9U exception) " + partitionName + " and the FED " + toString(fedid) + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
  
    return NULL ;
  }
  catch (exception &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (exception) " << partitionName << " and the FED " << fedid << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (exception) " + partitionName + " and the FED " + toString(fedid) + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
  
    return NULL ;
  }
  catch (...) { //Catch all types of exceptions
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (no specific exception) " << partitionName << " and the FED " << fedid << ": " <<  __FILE__  << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (no specific exception) " + partitionName + " and the FED " + toString(fedid) + partitionName ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
    
    return NULL ;
  }

  return theFed9UDescription_[fedid] ;
}

/** Download a FED from a FILE
 * \param fedid - the corresponding FED
 * \param filename - file name where the FED is stored
 * \param redownload - re-download from the file
 */
Fed9U::Fed9UDescription* DbInterface::downloadFEDFromFile ( int fedid, std::string filename, bool &changed, bool redownload ) {

  bool db = deviceFactory_->getDbUsed() ;
  errorMessage_ = "" ;
  changed = false ;

  if ( (filename != fedFileName_) || redownload ) {

    changed = true ;
    delete theFed9UDescription_[fedid] ;
    theFed9UDescription_[fedid] = NULL ;
      
    try {
      deviceFactory_->setUsingFile() ;

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      ((Fed9U::Fed9UDeviceFactory *)(deviceFactory_))->addFileName(filename);
      Fed9U::Fed9UDescription f = deviceFactory_->getFed9UDescription(fedid,false,0,0);

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of the FED " << fedid << " from the file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      theFed9UDescription_[fedid] = new Fed9U::Fed9UDescription(f);
      fedFileName_ = filename ;
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file " << filename << " and the FED " << fedid << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file " + filename + " and the FED " + toString(fedid) + ": " + e.what() ;
      fedFileName_ = "" ; 
      
      return NULL ;
    }
    catch (Fed9U::Fed9UDeviceFactoryException &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file (FED9U exception) " << filename << " and the FED " << fedid << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file (FED9U exception) " + filename + " and the FED " + toString(fedid) + ": " + e.what() ;
      fedFileName_ = "" ; 
      
      return NULL ;
    }
    catch (ICUtils::ICException &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file (ICUtils exception) " << filename << " and the FED " << fedid << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file (FED9U exception) " + filename + " and the FED " + toString(fedid) + ": " + e.what() ;
      fedFileName_ = "" ; 

      return NULL ;
    }
    catch (exception &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file (exception) " << filename << " and the FED " << fedid << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file (exception) " + filename + " and the FED " + toString(fedid) + ": " + e.what() ;
      fedFileName_ = "" ; 

      return NULL ;
    }
    catch (...) { //Catch all types of exceptions
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file (no specific exception) " << filename << " and the FED " << fedid << ": " <<  __FILE__  << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file (no specific exception) " + filename + " and the FED " + toString(fedid) ;
      fedFileName_ = "" ; 

      return NULL ;
    }
  }

  if (db) deviceFactory_->setUsingFile() ;

  return theFed9UDescription_[fedid] ;
}
  
/** Download FEDs from a FILE
 * \param filename - file name where the FED is stored
 * \param redownload - re-download from the file
 */
std::vector<Fed9U::Fed9UDescription *> DbInterface::downloadFEDFromFile ( std::string filename, bool &changed, bool redownload ) {
    
  bool db = deviceFactory_->getDbUsed() ;
  errorMessage_ = "" ;
  std::vector<Fed9U::Fed9UDescription*> *fedVector = NULL ;
  std::vector<Fed9U::Fed9UDescription*> fedVectorResult ;
  changed = false ;
    
  if ( (filename != fedFileName_) || redownload ) {

    // Delete all FEDs
    clearFEDDescriptions() ;      

    // data has changed
    changed = true ;
      
    try {
      deviceFactory_->setUsingFile() ;
	
#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
	
      ((Fed9U::Fed9UDeviceFactory *)deviceFactory_)->setInputFileName(filename);
      fedVector = deviceFactory_->getFed9UDescriptions("",-1,-1) ;

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of " << ((fedVector==NULL) ? 0: fedVector->size()) <<" FEDs from the file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      if (fedVector != NULL) {
	for (std::vector<Fed9U::Fed9UDescription*>::iterator it = fedVector->begin() ; it != fedVector->end() ; it ++) {
	  theFed9UDescription_[(*it)->getFedId()] = (*it) ;
	}
	fedFileName_ = filename ;
      }
      else fedFileName_ = "" ;
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file " << filename << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file " + filename + ": " + e.what() ;
      fedFileName_ = "" ; 
	
      return fedVectorResult ;
    }
    catch (Fed9U::Fed9UDeviceFactoryException &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file (FED9U exception) " << filename << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file (FED9U exception) " + filename + ": " + e.what() ;
      fedFileName_ = "" ; 
	
      return fedVectorResult ;
    }
    catch (ICUtils::ICException &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file (ICUtils exception) " << filename << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file (FED9U exception) " + filename + ": " + e.what() ;
      fedFileName_ = "" ; 
	
      return fedVectorResult ;
    }
    catch (exception &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file (exception) " << filename << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file (exception) " + filename + ": " + e.what() ;
      fedFileName_ = "" ; 
	
      return fedVectorResult ;
    }
    catch (...) { //Catch all types of exceptions
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to download the FED descriptions for the file (no specific exception) " << filename << ": " <<  __FILE__  << std::endl ;
#endif
      errorMessage_ = "Unable to download the FED descriptions for the file (no specific exception) " + filename ;
      fedFileName_ = "" ; 
	
      return fedVectorResult ;
    }
  }
    
  if (db) deviceFactory_->setUsingFile() ;
    
  // make a clone for the output
  for (Fed9U::Fed9UHashMapType::iterator it = theFed9UDescription_.begin() ; it != theFed9UDescription_.end() ; it ++) {
    if (it->second != NULL) {
      fedVectorResult.push_back(it->second) ;
    }
  }

  return fedVectorResult ;
}


/** Upload from the FED into the database
 * \param partitionName - partition name
 * \param strip - with or without strip
 * \param ismajor - upload into a major (true) or minor (false) version
 * \param fedVersionMajor - if not NULL then the major version is returned
 * \param fedVersionMinor - if not NULL then the minor version is returned
 * \warning if you create a new major version please be-aware that you should have downloaded all the FEDs before !
 */
int DbInterface::uploadFed(std::string partitionName, bool strip, bool ismajor, bool debug, unsigned int *fedMajorVersion, unsigned int *fedMinorVersion) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  if (theFed9UDescription_.size() == 0) {
#ifdef DEBUGMSGERROR
    std::cout << "uploadFed: No data to be uploaded" << std::endl ;
#endif
    errorMessage_ = "uploadFed: No data to be uploaded" ;
    return -1 ;
  }

  // Warning
  if (!partitionFEDDownload_ && ismajor) {
    std::cout << "WARNING: are you sure that all FEDs for the partition " << partitionName << " are there ?" << std::endl ;
    std::cout << "         In any other case, the version will not be coherent with the definition of the major (download will be done anyway)." << std::endl ;
  }

  try {
    // initially we set using strips to true, if the user wants to ignore strips he must explicity set it to false
    deviceFactory_->setUsingStrips ( strip );
    // same goes for using binary strips!
    deviceFactory_->setUsingBinaryStrips ( strip );
    // debug messages
    deviceFactory_->setDebugOutput(debug);
      
    unsigned short versionMajor = 0, versionMinor = 0 ;
    // Create a vector of FED 9U description for the upload
    std::vector<Fed9U::Fed9UDescription*> v ;
    for (Fed9U::Fed9UHashMapType::iterator it = theFed9UDescription_.begin() ; it != theFed9UDescription_.end() ; it ++) {
      if (it->second != NULL) v.push_back(it->second) ;
    }
    if (v.size() == 0) {
      std::cout << "uploadFed: No data to be uploaded" << std::endl ;
      return -2 ; 
    }
    int updateVersion = ismajor ? 1 : 0 ; // 0 is minor, 1 is major, 2 is append

#ifdef DEBUGMSGERROR
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

    deviceFactory_->setFed9UDescriptions(v,partitionName,&versionMajor,&versionMinor,updateVersion);

#ifdef DEBUGMSGERROR
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec << "Version " << versionMajor << "." << versionMinor << " has been uploaded for " << v.size() << " FEDs in partition " << partitionName << " and it took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
      
    if (fedMajorVersion != NULL) *fedMajorVersion = versionMajor ;
    if (fedMinorVersion != NULL) *fedMinorVersion = versionMinor ;
      
    fedMajorVersion_ = versionMajor ;
    fedMinorVersion_ = versionMinor ;
    partitionName_ = partitionName ;
      
    return 0;
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
      
    return -2 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to download the FEC descriptions for the partition (Oracle exception) " + partitionName, e) ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
      
    return -3 ;
  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (FED9U exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (FED9U exception) " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
      
    return -4 ;
  }
  catch (ICUtils::ICException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (ICUtils exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (FED9U exception) " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
      
    return -5 ;
  }
  catch (exception &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (exception) " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
      
    return -6 ;
  }
  catch (...) { //Catch all types of exceptions
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the FED descriptions for the partition (no specific exception) " << partitionName << ": " <<  __FILE__  << std::endl ;
#endif
    errorMessage_ = "Unable to download the FED descriptions for the partition (no specific exception) " + partitionName ;
    partitionName_ = "" ; 
    fedMajorVersion_ = fedMinorVersion_ = 0 ;
      
    return -6 ;
  }

  return 0 ;
}

/** Upload from the FED into the database
 * \param fedid - FED ID
 * \param partitionName - partition name
 * \param strip - with or without strip
 * \param ismajor - upload into a major (true) or minor (false) version
 * \param fedVersionMajor - if not NULL then the major version is returned
 * \param fedVersionMinor - if not NULL then the minor version is returned
 * \warning only minor version will be created to avoid the violation of the definition of a major version
 */
int DbInterface::uploadFed(int fedid, std::string partitionName, bool strip, bool debug, unsigned int *fedMajorVersion, unsigned int *fedMinorVersion) {

  errorMessage_ = "" ;

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  if (theFed9UDescription_.find(fedid) == theFed9UDescription_.end()) {
    std::cout << "uploadFed: No data to be uploaded" << std::endl ;
    return -2 ;
  }

  try {
    deviceFactory_->setUsingStrips(strip);
    deviceFactory_->setDebugOutput(debug);
      
    unsigned short versionMajor = 0, versionMinor = 0 ;
    int updateVersion = 0 ; // minor version

#ifdef DEBUGMSGERROR
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

    deviceFactory_->setFed9UDescription(*theFed9UDescription_[fedid],partitionName,&versionMajor,&versionMinor,updateVersion);

#ifdef DEBUGMSGERROR
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec << "Version " << versionMajor << "." << versionMinor << " has been uploaded for FED " << fedid  << " in partition " << partitionName << " and it took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
      
    if (fedMajorVersion != NULL) *fedMajorVersion = versionMajor ;
    if (fedMinorVersion != NULL) *fedMinorVersion = versionMinor ;
      
    fedMajorVersion_ = versionMajor ;
    fedMinorVersion_ = versionMinor ;
    partitionName_ = partitionName ;
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the FED descriptions for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the FED descriptions for the partition " + partitionName + ": " + e.what() ;
      
    return -1 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to upload the FEC descriptions for the partition (Oracle exception) " + partitionName, e) ;
#ifdef DEBUGMSGERROR
    std::cerr << errorMessage_ << std::endl ;
#endif      
    return -2 ;
  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "unable to upload the FED descriptions for the partition (FED9U exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "unable to upload the FED descriptions for the partition (FED9U exception) " + partitionName + ": " + e.what() ;
 
    return -3 ;
  }
  catch (ICUtils::ICException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "unable to upload the FED descriptions for the partition (ICUtils exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "unable to upload the FED descriptions for the partition (FED9U exception) " + partitionName + ": " + e.what() ;

    return -4 ;
  }
  catch (exception &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "unable to upload the FED descriptions for the partition (exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "unable to upload the FED descriptions for the partition (exception) " + partitionName + ": " + e.what() ;
      
    return -5 ;
  }
  catch (...) { //Catch all types of exceptions
#ifdef DEBUGMSGERROR
    std::cerr << "unable to upload the FED descriptions for the partition (no specific exception) " << partitionName << ": " <<  __FILE__  << std::endl ;
#endif
    errorMessage_ = "unable to upload the FED descriptions for the partition (no specific exception) " + partitionName ;
      
    return -6 ;
  }

  return 0 ;
}

/** Upload the FED into a file
 * \param filename - file name
 * \warning the upload is done in one file for all FEDs
 */
int DbInterface::uploadFed(std::string filename) {

  if (theFed9UDescription_.size() == 0) {
    std::cout << "uploadFed: No data to be uploaded" << std::endl ;
    return -2 ;
  }

  std::vector<Fed9U::Fed9UDescription*> v ;
  for (Fed9U::Fed9UHashMapType::iterator it = theFed9UDescription_.begin() ; it != theFed9UDescription_.end() ; it ++) {
    if (it->second != NULL) v.push_back(it->second) ;
  }
  if (v.size() == 0) {
    std::cout << "uploadFed: No data to be uploaded" << std::endl ;
    return -2 ; 
  }
  try {
    Fed9U::Fed9UDeviceFactory deviceFactory ;
    deviceFactory.setUsingFile() ;
    deviceFactory.setOutputFileName(filename) ;
    unsigned short version ;

#ifdef DEBUGMSGERROR
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

    deviceFactory.setFed9UDescriptions(v,"NONE",&version,&version,1) ;

#ifdef DEBUGMSGERROR
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec << "Upload of " << v.size() << " devices in file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the FED descriptions: " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the FED descriptions: " + e.what() ;
      
    return -1 ;
  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "unable to upload the FED descriptions for the partition (FED9U exception): " << e.what() << std::endl ;
#endif
    std::stringstream of ; of << "unable to upload the FED descriptions for the partition (FED9U exception): " << e.what() ;
    errorMessage_ = of.str() ;
      
    return -3 ;
  }
  catch (ICUtils::ICException &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "unable to upload the FED descriptions for the partition (ICUtils exception): " << e.what() << std::endl ;
#endif
    std::stringstream of ; of << "unable to upload the FED descriptions for the partition (IC exception): " << e.what() ;
    errorMessage_ = of.str() ;

    return -4 ;
  }
  catch (exception &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "unable to upload the FED descriptions for the partition (exception): " << e.what() << std::endl ;
#endif
    std::stringstream of ; of << "unable to upload the FED descriptions for the partition (exception): " << e.what() ;
    errorMessage_ = of.str() ;
      
    return -5 ;
  }
  catch (...) { //Catch all types of exceptions
#ifdef DEBUGMSGERROR
    std::cerr << "unable to upload the FED descriptions for the partition (no specific exception): " <<  __FILE__  << std::endl ;
#endif
    errorMessage_ = "unable to upload the FED descriptions for the partition (no specific exception)" ;
      
    return -6 ;
  }

  return 0 ;
}

/* ********************************************************************************************** */
/*                                                                                                */
/* Download / upload of the FEC/FED connections                                                   */
/*                                                                                                */
/* ********************************************************************************************** */

/** Download the connections
 * \param partitionName - partition name
 * \param changed - true if the connection descriptions have been changed
 * \param connectionMajor - connection major version
 * \param connectionMinor - connection minor version
 */
int DbInterface::downloadConnectionsFromDatabase ( std::string partitionName, bool &changed, unsigned int connectionMajor, unsigned int connectionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor ) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  changed = true ;
  bool downloadConnectionDone = false ;

#ifdef DEBUGMSGERROR
  std::cout << std::dec << "Current connection version is: " << connectionMajorVersion_ << "." << connectionMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" <<  " for the partition " << partitionName_  << "/" << partitionName << std::endl ;
#endif

  // connection version
  if ( (partitionName_ == partitionName) && (connectionMajor != 0) ) {
    if ( (connectionMajor == connectionMajorVersion_) && (connectionMinor == connectionMinorVersion_) && (maskMajorVersion_ == maskVersionMinor) && (maskMinorVersion_ == maskVersionMinor) ) {
#ifdef DEBUGMSGERROR
      std::cout << std::dec << "--> Download already done for the partition " << partitionName_ << " version " << connectionMajorVersion_ << "." << connectionMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << std::endl;
#endif
      downloadConnectionDone = true ;
      changed = false ;
    }
  }

  try {
    // Check the version in the database
    if ( (!downloadConnectionDone) && (connectionMajorVersion_ != 0) && (partitionName == partitionName_) ) { // one version has been already downloaded for this partition

      // Retreive the current connection version for this partition
      unsigned int major, minor, maskMajor, maskMinor, partitionId ;
      ((ConnectionFactory *)deviceFactory_)->getPartitionVersion ( partitionName, &major, &minor, &maskMajor, &maskMinor, &partitionId) ;
	
      // Same version ?
      if ( (major == connectionMajorVersion_) && (minor == connectionMinorVersion_) && (maskMajor == maskMajorVersion_) && (maskMinor == maskMinorVersion_) ) {
#ifdef DEBUGMSGERROR
	std::cout << std::dec << "--> Download already done for the partition " << partitionName_ << " version " << connectionMajorVersion_ << "." << connectionMinorVersion_ << " (mask = " << maskMajorVersion_ << "." << maskMinorVersion_ << ")" << std::endl;
#endif
	downloadConnectionDone = true ;
	changed = false ;
      }
#ifdef DEBUGMSGERROR
      else {
	std::cout << "Version has been changed, download from the database" << std::endl ;
      }
#endif
    }
      
    // If data not found then go to the database
    if (!downloadConnectionDone) {
	
#ifdef DEBUGMSGERROR
      std::cout << "--> Download from the database (Connections)" << std::endl ;
#endif
	
#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
	
      //deviceFactory_->getConnectionDescriptions ( partitionName, connectionVector_, connectionMajor, connectionMinor, maskVersionMajor, maskVersionMinor, allConnections_) ;
      unsigned int connectionNotIdentified = deviceFactory_->getConnectionDescriptionDetId ( partitionName, connectionVector_, connectionMajor, connectionMinor, maskVersionMajor, maskVersionMinor, allConnections_) ;

      if (connectionNotIdentified) {
	errorMessage_ = toString (connectionNotIdentified) + std::string (" connection(s) have no correponding det id") ;
#ifdef DEBUGMSGERROR
	std::cerr << "ERROR: " << connectionNotIdentified << " connection(s) have no correponding det id" << std::endl ; 
#endif
      }
	
#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of " << connectionVector_.size() << " connections from the database for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif


      // Retreive the current connection version for this partition
      partitionName_ = partitionName ;
      if (connectionMajor != 0) {
	connectionMajorVersion_ = connectionMajor ;
	connectionMinorVersion_ = connectionMinor ;
	maskMajorVersion_ = maskVersionMajor ;
	maskMajorVersion_ = maskVersionMinor ;

      }
      else {
	unsigned int partitionId ;
	((ConnectionFactory *)deviceFactory_)->getPartitionVersion ( partitionName, &connectionMajorVersion_, &connectionMinorVersion_, &maskMajorVersion_, &maskMinorVersion_, &partitionId) ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the connection descriptions for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the connection descriptions for the partition " + partitionName + ": " + e.what() ;
    partitionName_ = "" ; 
    connectionMajorVersion_ = connectionMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;
      
    return -2 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to download the connection descriptions for the partition (Oracle exception) " + partitionName, e) ;
#ifdef DEBUGMSGERROR
    std::cerr << errorMessage_ << std::endl ;
#endif
    partitionName_ = "" ; 
    connectionMajorVersion_ = connectionMinorVersion_ = 0 ;
    maskMajorVersion_ = maskMinorVersion_ = 0 ;      

    return -3 ;
  }

  return 0 ;
}

/** Download the connections from a file
 * \param filename - file name for the devices
 */
int DbInterface::downloadConnectionsFromFile (std::string filename, bool &changed, bool redownload ) {

  changed = false ;

  if ( (filename != connectionFileName_) || redownload ) {

    bool db = deviceFactory_->getDbUsed() ;
      
    try {
	
      changed = true ;
	
#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
	
      deviceFactory_->setUsingFile() ;
      deviceFactory_->setConnectionInputFileName(filename) ;

#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Parsing of the file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
	
      // Retreive the values for PIA reset
      connectionVector_.clear() ;
      deviceFactory_->getConnectionDescriptions (connectionVector_,allConnections_) ;

      connectionFileName_ = filename ;
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cout << "Error of the download of connections from file " << filename << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Error of the download of connections from file " + filename + ": " + e.what() ;

      connectionFileName_ = "" ;
      return -2 ;
    }

    if (db) deviceFactory_->setUsingDb() ;
  }

  return 0 ;
}

/** Upload connections to the database
 * \param partitionName - partition name to be used
 * \param isMajor - upload a major (true) or a minor (false) version 
 * \param connectionVersionMajor - if not NULL then the major version is returned
 * \param connectionVersionMinor - if not NULL then the minor version is returned
 */
int DbInterface::uploadConnections(std::string partitionName, bool ismajor, unsigned int *connectionVersionMajor, unsigned int *connectionVersionMinor ) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  try {

    // connection vector
    if (connectionVector_.size()) {

      unsigned int major,minor;

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      // Upload the devices
      deviceFactory_->setConnectionDescriptions (deviceFactory_->getConnectionVector(), partitionName, &major, &minor, ismajor) ;
	    
#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Version " << major << "." << minor << " has been uploaded for " << connectionVector_.size() << " in partition " << partitionName << " and it took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
	
      if (connectionVersionMajor != NULL) *connectionVersionMajor = major ;
      if (connectionVersionMinor != NULL) *connectionVersionMajor = minor ;

      // set the version as to be used
      connectionMajorVersion_ = major ;
      connectionMinorVersion_ = minor ;
      partitionName_ = partitionName ;
    }
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the connections for the partition " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the connections for the partition " + partitionName + ": " + e.what() ;

    return -2 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to upload the connections for the partition (Oracle exception)" + partitionName, e) ;
#ifdef DEBUGMSGERROR
    std::cerr << errorMessage_ << std::endl ;
#endif
      
    return -3 ;
  }
  catch (std::exception &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the connections for the partition (std::exception) " << partitionName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the connections for the partition (std::exception) " + partitionName + ": " + e.what() ;
      
    return -3 ;
  }
  catch (...) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the connections for the partition (unknown exception) " << partitionName << std::endl ;
#endif
    errorMessage_ = "Unable to upload the connections for the partition (unknown) " + partitionName ;
      
    return -4 ;
  }

  return 0 ;
}

/** Upload the current data to a file
 * \param filename - output file
 */
int DbInterface::uploadConnections(std::string filename) {

  try {

    // Retreive all the devices even the one disabled
    ConnectionVector vConn = deviceFactory_->getConnectionVector() ;

    ConnectionFactory deviceFactory ;
    deviceFactory.setOutputFileName(filename);

#ifdef DEBUGMSGERROR
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

    if (vConn.size()) deviceFactory.setConnectionDescriptions(vConn) ;
    else {	
#ifdef DEBUGMSGERROR
      std::cout << "Connection vector is empty, cannot upload it in file" << std::endl ;
#endif
      errorMessage_ = "Connection vector is empty, cannot upload it in file" ;
      return -1 ;
    }

#ifdef DEBUGMSGERROR
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec << "Upload of " << vConn.size() << " connections in file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the connections in a file: " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the connections in a file: " + e.what() ;
      
    return -2 ;
  }

  return 0 ;
}


/* ********************************************************************************************** */
/*                                                                                                */
/* Download / upload of the DET ID                                                                */
/*                                                                                                */
/* ********************************************************************************************** */

/** Download the DET ID for a given partition name
 * \param partitionDetIdName - partition name
 * \param changed - true if the DET ID have been changed
 * \param detIdMajor - DET ID device major version
 * \param detIdMinor - DET ID device minor version
 */
int DbInterface::downloadDetIdFromDatabase ( std::string partitionDetIdName, bool &changed, unsigned int detIdMajor, unsigned int detIdMinor ) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  changed = true ;
  bool downloadDetIdDone = false ;

#ifdef DEBUGMSGERROR
  std::cout << std::dec << "Current DET ID version is: " << detIdMajorVersion_ << "." << detIdMinorVersion_ <<  " for the partition " << partitionDetIdName_  << "/" << partitionDetIdName << std::endl ;
#endif

  // DET ID version
  if ( (partitionDetIdName_ == partitionDetIdName) && (detIdMajor != 0) ) {
    if ( (detIdMajor == detIdMajorVersion_) && (detIdMinor == detIdMinorVersion_) ) {
#ifdef DEBUGMSGERROR
      std::cout << std::dec << "--> Download already done for the partition " << partitionDetIdName_ << " version " << detIdMajorVersion_ << "." << detIdMinorVersion_ << std::endl;
#endif
      downloadDetIdDone = true ;
      changed = false ;
    }
  }

  try {
    // Check the version in the database
    if ( (!downloadDetIdDone) && (detIdMajorVersion_ != 0) && (partitionDetIdName == partitionDetIdName_) ) { // one version has been already downloaded for this partition

      try {
	// Retreive the current det ID version for this partition
	unsigned int major, minor, partitionId ;
	((TkDcuInfoFactory *)deviceFactory_)->getPartitionVersion ( partitionDetIdName, &major, &minor, &partitionId) ;
	
	// Same version ?
	if ( (major == detIdMajorVersion_) && (minor == detIdMinorVersion_) ) {
#ifdef DEBUGMSGERROR
	  std::cout << std::dec << "--> Download already done for the partition " << partitionDetIdName_ << " version " << detIdMajorVersion_ << "." << detIdMinorVersion_ << std::endl;
#endif
	  downloadDetIdDone = true ;
	  changed = false ;
	}
#ifdef DEBUGMSGERROR
	else {
	  std::cout << "Version has been changed, download from the database" << std::endl ;
	}
#endif
      }
      catch (FecExceptionHandler &e) {	// perhaps it is not in the current state so plan to re-download it

#ifdef DEBUGMSGERROR
	std::cout << "The partition is not in the current state" << std::endl ;
#endif

	changed = true ;
	downloadDetIdDone = false ;
      }
    }
      
    // If data not found then go to the database
    if (!downloadDetIdDone) {
	
      clearDetIdDescriptions ( ) ;

#ifdef DEBUGMSGERROR
      std::cout << "--> Download from database" << std::endl ;
#endif
	
#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
	
      if ( (partitionDetIdName == "") || (partitionDetIdName == "ALL") ) {
	deviceFactory_->addAllDetId ( detIdMajor, detIdMinor ) ;
      }
      else {
	deviceFactory_->addDetIdPartition ( partitionDetIdName, detIdMajor, detIdMinor ) ;
      }
	
#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Download of " << deviceFactory_->getInfos().size() << " from the database for partition " << partitionDetIdName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      // Retreive the current det id version for this partition
      partitionDetIdName_ = partitionDetIdName ;
      if (detIdMajor != 0) {
	detIdMajorVersion_ = detIdMajor ;
	detIdMinorVersion_ = detIdMinor ;
      }
      else if ( (partitionDetIdName != "") && (partitionDetIdName != "ALL") ) {
 	unsigned int partitionId ;
	((TkDcuInfoFactory *)deviceFactory_)->getPartitionVersion ( partitionDetIdName, &detIdMajorVersion_, &detIdMinorVersion_, &partitionId) ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to download the det id for the partition " << partitionDetIdName << ": " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to download the det id for the partition " + partitionDetIdName + ": " + e.what() ;
    partitionDetIdName_ = "" ;
    detIdMajorVersion_ = detIdMinorVersion_ = 0 ;
      
    return -2 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to download the det id for the partition (Oracle exception) " + partitionDetIdName, e) ;
#ifdef DEBUGMSGERROR
    std::cerr << errorMessage_ << std::endl ;
#endif
    partitionDetIdName_ = "" ;
    detIdMajorVersion_ = detIdMinorVersion_ = 0 ;
      
    return -3 ;
  }

  return 0 ;
}

/** Download the DET ID from file
 * \param filename - file name for the devices
 * \param changed - is any changed occurs
 * \param redownload - re-download it
 */
int DbInterface::downloadDetIdFromFile (std::string filename, bool &changed, bool redownload ) {

  changed = false ;

  if ( (filename != fecFileName_) || redownload ) {

    bool db = deviceFactory_->getDbUsed() ;
    try {

      changed = true ;

#ifdef DEBUGMSGERROR
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

      // Retreive the det id
      clearDetIdDescriptions() ;
      deviceFactory_->setUsingFile() ;
      deviceFactory_->setTkDcuInfoInputFileName(filename) ;
	
#ifdef DEBUGMSGERROR
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec << "Parsing of the file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      fecFileName_ = filename ;
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cout << "Error of the download of FEC/PIA from file " << filename << ": " << e.what() << std::endl ;
#endif
      errorMessage_ = "Error of the download of FEC/PIA from file " + filename + ": " + e.what() ;

      fecFileName_ = "" ;
      return -2 ;
    }

    if (db) deviceFactory_->setUsingDb() ;
  }
  return 0 ;
}

/** Upload data to the database
 * \param partitionName - partition name to be used
 * \param isMajor - upload a major (true) or a minor (false) version
 * \param detIdVersionMajor - if not NULL then the major version is returned
 * \param detIdVersionMinor - if not NULL then the minor version is returned
 * \warning no partition DET ID upload is permitted to avoid problem in the rest of the partitions
 */
int DbInterface::uploadDetIdToDB( ) {

  if (!deviceFactory_->getDbUsed()) {
#ifdef DEBUGMSGERROR
    std::cerr << "No database is set for the device factory" << std::endl ;
#endif
    errorMessage_ = "No database is set for the device factory" ;
    return -1 ;
  }

  if ( (partitionDetIdName_ != "") && (partitionDetIdName_ != "ALL") ) {
#ifdef DEBUGMSGERROR
    std::cerr << "You cannot upload det id for a given partition, should be done for all partitions" << std::endl ;
#endif
    errorMessage_ = "You cannot upload det id for a given partition, should be done for all partitions" ;
    return -1 ;
  }

  try {
#ifdef DEBUGMSGERROR
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
      
    // Upload the devices
    deviceFactory_->setTkDcuInfo ( ) ;

#ifdef DEBUGMSGERROR
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec << deviceFactory_->getInfos().size() << " DCU infos have been uploaded in database and it took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the DET ID for the partition: " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the DET ID for the partition: " + e.what() ;

    return -2 ;
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = deviceFactory_->what("Unable to upload the DET ID for the partition (Oracle exception)", e) ;
#ifdef DEBUGMSGERROR
      std::cerr << errorMessage_ << std::endl ;
#endif
      
    return -3 ;
  }

  return 0 ;
}

/** Upload the current data to a file
 * \param filename - output file
 */
int DbInterface::uploadDetItToFile (std::string filename) {

  bool db = deviceFactory_->getDbUsed() ;

  try {

    // Output 
    deviceFactory_->setUsingFile() ;
    deviceFactory_->setOutputFileName(filename);

#ifdef DEBUGMSGERROR
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif

    // Upload the devices
    deviceFactory_->setTkDcuInfo ( ) ;

#ifdef DEBUGMSGERROR
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec << "Upload of " << deviceFactory_->getInfos().size() << " devices in file " << filename << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to upload the DET ID in a file: " << e.what() << std::endl ;
#endif
    errorMessage_ = "Unable to upload the DET ID in a file: " + e.what() ;
      
    return -2 ;
  }
    
  if (db) deviceFactory_->setUsingDb() ;

  return 0 ;
}

/* ********************************************************************************************** */
/*                                                                                                */
/* Cache refreshing of the DB                                                                     */
/*                                                                                                */
/* ********************************************************************************************** */

/** Refresh the caching system
 */
int DbInterface::refreshCacheXMLClob ( ) {

  try {
    deviceFactory_->refreshCacheXMLClob ( ) ; 
    // default parameters so delete the partitions which are not anymore in the current state
    //                       update the FEDs with the strips
  }
  catch (oracle::occi::SQLException &e) {
    errorMessage_ = e.what() ;
#ifdef DEBUGMSGERROR
    std::cerr << errorMessage_ << std::endl ;
#endif
    return -1 ;
  }

  return 0 ;
}


/* ********************************************************************************************** */
/*                                                                                                */
/* O2O operation                                                                                  */
/*                                                                                                */
/* ********************************************************************************************** */

/** this method access the database in order to set the o2o operation performed
 * \param partitionName - partition name
 * \param subDetector - the corresponding subdetector
 * \param o2oRunNumber - next Run Number (o2o operation)
 * \param 0 if the operation is fine, -1 is the operation failed
 */
int DbInterface::setO2OOperation ( std::string partitionName, std::string subDetector, unsigned int o2oRunNumber ) {

  try {
    deviceFactory_->setO2OOperation ( partitionName, subDetector, o2oRunNumber ) ;
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to set an o2o operation: " << e.what() << std::endl ;
#endif
    errorMessage_ =  "Unable to set an o2o operation: " + e.what() ;
      
    return -1 ;
  }

  return 0 ;
}

/** this method checked that the o2o operation is coherent with the current state
 * \param partitionName - partition name
 * \return 0 if the version are not ok, 1 if the version are coherent with current state, -1 if the operation in database fails
 */
int DbInterface::getO2OXchecked ( std::string partitionName ) {

  int result ;

  try {
    if (deviceFactory_->getO2OXchecked ( partitionName )) result = 1 ;
    else result = 0 ;
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to set an o2o operation: " << e.what() << std::endl ;
#endif
    errorMessage_ =  "Unable to set an o2o operation: " + e.what() ;
      
    return -1 ;
  }

  return result ;
}
