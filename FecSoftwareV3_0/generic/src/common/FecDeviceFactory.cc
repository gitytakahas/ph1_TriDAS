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

//#define DEBUGMSGERROR

#include "XMLFecDevice.h"
#include "XMLFecDcu.h"
#include "XMLFecCcu.h"

#include <time.h>  // for time function

#include "FecFactory.h" // for static methods
#include "FecDeviceFactory.h"

#ifdef DATABASE
#include "DbFecAccess.h"
#endif

// TODO: hide (comment, remove, make private) the SetCCU... GetCCU...
// So that the user may only retrive/set a tkring at a time
// Also change the (Set/Get)Ring... in order to save also the ccus inside it
// The decoupling between CCUs and Ring must be done before the database (even if
// it is not compulsory for the file).
// By now all the decoupling must be done by hand by the user of these classes.


/** Build a FecDeviceFactory to retreive information from database or file
 * \param outputFileName - name of the XML output file
 * \exception Possible exceptions are:
 *     file not found
 *     bad XML format
 * \see addFileName to define the input file
 */
FecDeviceFactory::FecDeviceFactory ( ): 
  DeviceFactoryInterface ( ) {

// #ifdef DATABASE
//   if (databaseAccess) {
//     try {
//       setDatabaseAccess ( ) ;
//     }
//     catch (oracle::occi::SQLException &e) {
      
//       std::cerr << "*********** ERROR *************" << std::endl ;
//       std::cerr << "Unable to connect the database: " << std::endl ;
//       std::cerr << e.what() << std::endl ;
//       std::cerr << "*******************************" << std::endl ;

//       databaseAccess = false ;
//     }
//   }
// #else
//   databaseAccess = false ;
// #endif
//  if (! databaseAccess) setUsingFile ( ) ;

  setUsingFile ( ) ;
  //tkRingDescription_.clear() ;
}

#ifdef DATABASE

/** Create an access to the database FEC database
 * \param login - login to database
 * \param password - password to database
 * \param path - URL to acess the database
 * \param className - class name to access the database
 * \param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 */
FecDeviceFactory::FecDeviceFactory ( std::string login, std::string password, std::string path, bool threaded ) 
    throw ( oracle::occi::SQLException ): 
  DeviceFactoryInterface ( login, password, path, threaded ) {

  setDatabaseAccess ( login, password, path ) ;
  //tkRingDescription_.clear() ;
}

/** Create an access to the FEC database
 * \param dbAccess - database access
 */
FecDeviceFactory::FecDeviceFactory ( DbFecAccess *dbAccess ):
  DeviceFactoryInterface ((DbCommonAccess *)dbAccess) {
  //tkRingDescription_.clear() ;
}

#endif

/** Disconnect the database (if it is set)
 */  
FecDeviceFactory::~FecDeviceFactory ( ) {

  // The database access is deleted in the DeviceFactoryInterface

  // Emptyied the list of description (FEC devices)
  FecFactory::deleteVectorI (vFecDevices_) ;

  // Emptyied the list of rings
  FecFactory::deleteVectorI (tkRingDescription_) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 */
void FecDeviceFactory::addFileName ( std::string fileName ) {

  // Devices to be deleted
  deviceVector deleteDevices ;

  // For FEC devices
  XMLFecDevice xmlFecDevice ( fileName ) ;

  // Retreive all the devices from the parsing class
  deviceVector vDevice = xmlFecDevice.getDevices ( ) ;

  // Merge the vector from the class and the new vector
  // vFecDevices_.merge (*vDevice) ;
  for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {
    
    bool equalS = false ;
    
    for (deviceVector::iterator deviceP = vFecDevices_.begin() ; (deviceP != vFecDevices_.end() && ! equalS ) ; deviceP ++) {
      if (*deviceP == *device) equalS= true ;
    }

    if (! equalS) 
      vFecDevices_.push_back (*device) ;
    else 
      deleteDevices.push_back(*device) ;
  }

  // All devices are deleted by the XMLFecDevice so nothing must be deleted

#ifdef DATABASE
  initDbVersion_ = false ;
  useDatabase_ = false ;
#endif

  // delete the devices not used
  FecFactory::deleteVectorI(deleteDevices) ;
}

/** Add a new file name and parse it to retreive the information needed and make a sort on the FEC hardware ID
 * \param fileName - name of the XML file
 * \param fecHardwareId - FEC hardware ID
 */
void FecDeviceFactory::addFileName ( std::string fileName, std::string fecHardwareId ) {

  // Devices to be deleted
  deviceVector deleteDevices ;  

  // Add new entries
  FecDeviceFactory::addFileName (fileName) ;

  // Copy the vector
  deviceVector vDevices = vFecDevices_ ;
  vFecDevices_.clear() ;

  // Now check the fecHardwareId and then remove the devices not declared
  for (deviceVector::iterator device = vDevices.begin() ; (device != vDevices.end()) ; device ++) {

    deviceDescription *dev = *device ;
    if (dev->getFecHardwareId() == fecHardwareId) 
      vFecDevices_.push_back(dev) ;
    else 
      deleteDevices.push_back(dev) ;
  }

  // Delete the intermediate vector
  FecFactory::deleteVectorI (deleteDevices) ;
}

/** Set a file as the new input, same method than addFileName but the previous devices and pia are deleted
 * \param inputFileName - new input file
 */
void FecDeviceFactory::setInputFileName ( std::string inputFileName ) {

  // delete the old vector that is not more usefull
  FecFactory::deleteVectorI (vFecDevices_) ;

  // Add new entries
  FecDeviceFactory::addFileName (inputFileName) ;
}

/** Set a file as the new input, same method than addFileName but the previous devices are deleted
 * This method make a sort on the FEC hardware ID
 * \param inputFileName - new input file
 * \param fecHardwareId - FEC hardware ID
 */
void FecDeviceFactory::setInputFileName ( std::string inputFileName, std::string fecHardwareId ) {

  // delete the old vector that is not more usefull
  FecFactory::deleteVectorI (vFecDevices_) ;

  // Add new entries
  FecDeviceFactory::addFileName (inputFileName, fecHardwareId) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** 
 * \param login - login to database
 * \param password - password to database
 * \param path - path to database
 * \warning in the destructor of FecDeviceFactory, the dbAccess is deleted
 */
void FecDeviceFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) 
  throw ( oracle::occi::SQLException ) {

  // For FEC database delete the old one if needed
  if (dbAccess_ != NULL)
    if (login != dbAccess_->getDbLogin() || password != dbAccess_->getDbPassword() || path != dbAccess_->getDbPath()) {
      delete dbAccess_ ;
      dbAccess_ = NULL ;
    }
  
  if (dbAccess_ == NULL) {
#ifdef DEBUGMSGERROR
    std::cout << "Create a new access to the FEC database for " << login << "/" << password << "@" << path << std::endl ;
#endif

    dbAccess_ = new DbFecAccess (login, password, path, threaded_) ;
  }

  deleteDbAccess_ = true ;
  useDatabase_ = true ;

  // No version intialised
  initDbVersion_ = false ;
}

/** 
 * \param dbAccess - FEC access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void FecDeviceFactory::setDatabaseAccess ( ) 
  throw ( oracle::occi::SQLException ) {

  // Retreive the parameters
  std::string login, password, path ;
  if (getDatabaseConfiguration(login, password, path)) {
    setDatabaseAccess (login, password, path) ;
  }

  // No version intialised
  initDbVersion_ = false ;
}

/** 
 * \param dbAccess - FEC access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void FecDeviceFactory::setDatabaseAccess ( DbFecAccess *dbAccess ) {

  // For FEC database delete the old one if needed
  DeviceFactoryInterface::setDatabaseAccess ((DbCommonAccess *)dbAccess) ;

  // No version intialised
  initDbVersion_ = false ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Partition / version methods
//
// ------------------------------------------------------------------------------------------------------

/**
 * \param partitionName - partitionName
 * \return the information about partition versus version depends on which parameter was set, in the order of partition, FEC hardware ID, (instance, class name, IP)
 * The parameters must be deleted by the remote method
 * Note that the method returns always a pointer allocated but the list can be empty
 */
std::list<unsigned int *> FecDeviceFactory::getPartitionVersion ( std::string partitionName ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
  std::cout << "Retreive the current version for the partition " << partitionName << std::endl ;
#endif

  std::list<unsigned int*> partitionVersion ;

  if (dbAccess_ != NULL) {

    // Retreive the current version for the given partition
    partitionVersion = ((DbFecAccess *)dbAccess_)->getDatabaseVersion(partitionName) ;

    if (partitionVersion.empty()) {    

      std::stringstream msgError ; msgError << "no version for the partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER (DB_NOVERSIONAVAILABLE, msgError.str(), ERRORCODE) ;
    }
#ifdef DEBUGMSGERROR
    else {

      for (std::list<unsigned int*>::iterator it = partitionVersion.begin() ; it != partitionVersion.end() ; it ++) {
	unsigned int *value = *it;
	std::cout << "\tCurrent version: Partition " << value[0] << ": Version " << value[1] << "." << value[2] << "(mask = " << value[3] << "." << value[4] << ")" ;
	if ((value[1] == 0) && (value[2] == 0)) std::cout << " <= partition not used" << std::endl ;
	else std::cout << std::endl ;
      }
    }
#endif
  }
  else {
#  ifdef DEBUGMSGERROR
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << __PRETTY_FUNCTION__ << ": unable to retreive the partition/version, the database is not set" << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
#  endif

    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
  
  return (partitionVersion) ;
}

/**
 * \param partitionName - partitionName
 * \param major - major version returned
 * \param minor - minor version returned
 * \param partitionNumber - partition number returned
 * The parameters must be deleted by the remote method
 * Note that the method returns always a pointer allocated but the list can be empty
 */
void FecDeviceFactory::getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *maskMajor, unsigned int *maskMinor, unsigned int *partitionNumber ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
  std::cout << "Retreive the current version for the partition " << partitionName << std::endl ;
#endif

  *major = *minor = *partitionNumber = 0 ;
  std::list<unsigned int*> partitionVersion = getPartitionVersion ( partitionName ) ;
  if (! partitionVersion.empty()) {
    std::list<unsigned int*>::iterator it = partitionVersion.begin() ;

    unsigned int *value = *it;
    *partitionNumber = value[0] ;
    *major = value[1] ;
    *minor = value[2] ;
    *maskMajor = value[3] ;
    *maskMinor = value[4] ;
    
    int error = partitionVersion.size() ;
    
    for (std::list<unsigned int*>::iterator it = partitionVersion.begin() ; it != partitionVersion.end() ; it ++) {
      delete[] (*it) ;
    }
    
    if (error > 1) {

      std::stringstream msgError ; msgError << "several version for the partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER (DB_SEVERALVERSIONSERROR ,msgError.str(), ERRORCODE) ;
    }
  }
}

/**
 * \param versionMajor - major version of the database
 * \param versionMinor - minor version of the database
 * \param partitionName - partition name to be set
 * \param fecHardwareId - FEC hardware id, the check is done on the id only if the partitionName is "nil"
 * One the following parameter must be specified and it is checked in the following order
 * <ul>
 * <li> partitionName
 * <li> fecHardwareId and partition name
 * </ul>
 */
void FecDeviceFactory::setPartitionVersion ( std::string partitionName, unsigned int versionMajor, unsigned int versionMinor ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

  if (dbAccess_ != NULL) {

    // Retreive the current version for the partition name specified
    std::list<unsigned int*> partitionVersion = getPartitionVersion ( partitionName ) ;

    if (partitionVersion.empty()) {
      
#ifdef DEBUGMSGERROR
      std::cerr << "*********************************** ERROR ****************************" << std::endl ;
      std::cerr << "No partition is allowed for the partition " << partitionName << std::endl ;
      std::cerr << "**********************************************************************" << std::endl ;
#endif
      std::stringstream msgError ; msgError << "no version found for the partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER (DB_NOVERSIONAVAILABLE, msgError.str(), ERRORCODE) ;
    }
    else {
      std::list<unsigned int *> partitionVersionBis ;
      for (std::list<unsigned int*>::iterator it = partitionVersion.begin() ; it != partitionVersion.end() ; it ++) {
	
	unsigned int *value = *it;

#  ifdef DEBUGMSGERROR
	std::cout << "\tOld version: Partition " << value[0] << ": Version " << value[1] << "." << value[2] << std::endl ;
#  endif
	
	if ((versionMajor != value[1]) || (versionMinor != value[2])) {
	  
	  unsigned int *val = new unsigned int[3] ;
	  val[0] = value[0] ; val[1] = versionMajor ; val[2] = versionMinor ;
	  partitionVersionBis.push_back (val) ;
	}

	delete[] value ;
      }

#  ifdef DEBUGMSGERROR
      for (std::list<unsigned int*>::iterator it = partitionVersionBis.begin() ; it != partitionVersionBis.end() ; it ++) {
	unsigned int *value = *it;
	
	std::cout << "\tNew version: Partition " << value[0] << ": Version " << value[1] << "." << value[2] << std::endl ;
      }
#  endif

      if (! partitionVersionBis.empty()) ((DbFecAccess *)dbAccess_)->setDatabaseVersion(partitionVersionBis);

      for (std::list<unsigned int*>::iterator it = partitionVersionBis.begin() ; it != partitionVersionBis.end() ; it ++) {
	delete[] *it ;
      }
    }
  }
  else {
#  ifdef DEBUGMSGERROR
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Unable to set the partition/version, the database is not set" << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
#  endif

    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** Upload the description in the output and create a partition for the given modules
 * \param devices - a vector of device description
 * \param versionMajor - version set in the database (output)
 * \param versionMinor - version set in the database (output)
 * \param partitionName - a new partition name
 * \param createPartitionFlag - if true then a new partition is created, if false then the device reset are added to the partition
 * \warning the version parameters are set by the database (there are output parameters)
 * \warning the version can be NULL if the output is a file
 * \see XMLFecDevice::dbConfigure ( std::string, std::string )
 */
void FecDeviceFactory::createPartition ( deviceVector devices, unsigned int *versionMajor, unsigned int *versionMinor, std::string partitionName, bool createPartitionFlag ) 
  throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << "FecDeviceFactory::createPartition" << std::endl ;
#endif

  if ( dbAccess_ != NULL && versionMajor != NULL && versionMinor != NULL) {

    // Upload in database
    // Create an XMLFecDevice for the upload in database
    XMLFecDevice xmlFecDevice ;
    xmlFecDevice.setDeviceVector (devices) ;
    xmlFecDevice.setDatabaseAccess((DbFecAccess *)dbAccess_);

#ifdef DEBUGMSGERROR
    FecFactory::display (devices) ;
    std::cout << "Create a new partition " << partitionName << std::endl ;
#endif

    *versionMajor = xmlFecDevice.dbConfigure ( partitionName, createPartitionFlag ) ;
    *versionMinor = 0 ;
  }
  else 
    setFecDeviceDescriptions (devices) ;
}


#endif // Database

// ------------------------------------------------------------------------------------------------------
// 
// Correction on FEC devices
// 
// ------------------------------------------------------------------------------------------------------

/** Change two DCU hard id which are incoherent and faulty in hardware detection
 * The two DCUs are : 
 *     9564762 for TEC- (hardware readout is 16777215)
 *     16777214 for TOB (hardware readout is 16777215)
 * \return the correction(s) applied in string text
 */
std::string FecDeviceFactory::setDeviceCorrection ( deviceVector &devices, std::string subDetector ) {

  std::stringstream outputMessage ;

  // Find out the two DCUs in the corresponding subdectector
  if ((subDetector == "TEC-") || (subDetector == "TECM")) {

    for (deviceVector::iterator it = devices.begin() ; it != devices.end() ; it ++) {
      if ((*it)->getDeviceType() == DCU) {
	dcuDescription *dcu = dynamic_cast<dcuDescription *>(*it) ;
	if (dcu->getDcuHardId() == 16777215) {
	  dcu->setDcuHardId(9564762) ;
	  outputMessage << "Found DCU 16777215 for TEC-, change it to 9564762" ;
	}
      }
    }
  }
  else
    if (subDetector == "TOB") {

      for (deviceVector::iterator it = devices.begin() ; it != devices.end() ; it ++) {
	if ((*it)->getDeviceType() == DCU) {
	  dcuDescription *dcu = dynamic_cast<dcuDescription *>(*it) ;
	  if (dcu->getDcuHardId() == 16777215) {
	    dcu->setDcuHardId(16777214) ;
	    outputMessage << "Found DCU 16777215 for TOB, change it to 16777214" ;
	  }
	}
      }
    }
    else {

      outputMessage << "NA" ;
    }

  return outputMessage.str() ;
}


// ------------------------------------------------------------------------------------------------------
// 
// Tracker FEC device methods
//
// ------------------------------------------------------------------------------------------------------

/** Retreive the descriptions for all devices from the input
 * \param fileUsed - use file
 * \param versionMajor - major version for database
 * \param versionMinor - minor version for database
 * \param pMaskVersionMajor - mask major version for database
 * \param pMaskVersionMinor - mask minor version for database
 * \param partitionName - partition name
 * \param fecHardwareId - FEC hardware ID
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned so do not delete the devices
 * \param allDevices - return all devices including the disable device, by default return only the enabled devices
 * \param forceDbReload - force the database reload
 * One the following parameter must be specified and it is checked in the following order
 * <ul>
 * <li> partitionName
 * <li> fecHardwareId and partition name
 * </ul>
 * \return a vector of device description
 * \warning if no parametersforceReload are given then the devices are retreived from files given by addFileName
 * \warning if the version is 0, 0 then the devices is coming from the current version
 * \warning if the version is different than 0.0 then the values are retreived from the version given. Note that this version is set as the current version.
 * \warning if the input if a file whatever the version is, the device description is retreived from the input file.
 */ 
void FecDeviceFactory::getFecDeviceDescriptions ( bool fileUsed, unsigned int versionMajor, unsigned int versionMinor, unsigned int pMaskVersionMajor, unsigned int pMaskVersionMinor, std::string partitionName, std::string fecHardwareId, deviceVector &outVector, bool allDevices, bool forceDbReload ) 
#ifdef DATABASE
  throw (oracle::occi::SQLException, FecExceptionHandler ) {
#else
  throw (FecExceptionHandler ) {
#endif

#ifdef DEBUGMSGERROR
  std::cout << __PRETTY_FUNCTION__ << std::endl ;
  std::cout << versionMajor << "." << versionMinor << std::endl ;
  std::cout << pMaskVersionMajor << "." << pMaskVersionMinor << std::endl ;
  std::cout << "partition = " << partitionName << std::endl ;
  std::cout << "fechardwareid = " << fecHardwareId << std::endl ;
#endif

#ifdef DEBUGMSGERROR
  if ( fileUsed )
    std::cout << "getFecDeviceDescriptions from the files set" << std::endl ;
  else {
    std::cout << "getFecDeviceDescriptions for the partition " << partitionName ;
    if (fecHardwareId != "0")
      std::cout << " and for the FEC hardware id " << fecHardwareId << std::endl ;
    else std::cout << std::endl ;

    if ( (versionMajor == 0) && (versionMinor == 0) ) std::cout << "  Retreive from the current version" << std::endl ;
    else std::cout << "  Retreive from the version " << versionMajor << "." << versionMinor << " & mask " << pMaskVersionMajor << "." << pMaskVersionMinor << std::endl ;
  }
#endif

#ifdef DATABASE
  // retreive the information from database
  if ( getDbUsed() && !fileUsed ) {

    // Check the version
    if (forceDbReload) initDbVersion_ = false ;
    if (initDbVersion_) {
      if ((versionMajor != 0) || (versionMinor != 0)) {  // A version has been set

	if ((partitionName_ == partitionName) && ((uint)versionMajor == versionMajor_) && ((uint)versionMinor == versionMinor_) && ((uint)maskVersionMajor_ == pMaskVersionMajor) && ((uint)maskVersionMinor_ == pMaskVersionMinor))
	  initDbVersion_ = true ;
	else 
	  initDbVersion_ = false ;
      }
      else { // from current version
	unsigned int partitionNumber, versionMajorI, versionMinorI, maskMajorI, maskMinorI ;
	getPartitionVersion(partitionName, &versionMajorI, &versionMinorI, &partitionNumber, &maskMajorI, &maskMinorI) ;
	if ((partitionName_ == partitionName) && (versionMajorI == versionMajor_) && (versionMinorI == versionMinor_) && ((uint)maskVersionMajor_ == maskMajorI) && ((uint)maskVersionMinor_ == maskMinorI))
	  initDbVersion_ = true ;
	else 
	  initDbVersion_ = false ;

#ifdef DEBUGMSGERROR
	std::cout << "Partition " << partitionName << " / " << partitionName_ 
		  << " Version " << versionMajorI << "." << versionMinorI << "/" 
		  << versionMajor_ << "." << versionMinor_ 
		  << " Mask " << maskMajorI << "." << maskMinorI << "/" 
		  << maskVersionMajor_ << "." << maskVersionMinor_
		  << std::endl ;
#endif
      }
    }

    // Check if the downlaod must be done again
    if (!initDbVersion_) {

      // delete the old vector that is not more usefull
      FecFactory::deleteVectorI (vFecDevices_) ;

      // Now retreive the information from database
      XMLFecDevice xmlFecDevice ( (DbFecAccess *)dbAccess_ ) ;
      deviceVector vDevice ;

      if ((versionMajor != 0) || (versionMinor != 0)) {

#ifdef DEBUGMSGERROR
	if (fecHardwareId != "0")
	  std::cout << "Try to retreive version " << versionMajor << "." << versionMinor << std::endl ;
	else
	  std::cout << "Try to retreive version " << versionMajor << "." << versionMinor << " & mask " << pMaskVersionMajor << "." << pMaskVersionMinor << std::endl ;
#endif

	if (fecHardwareId != "0")
	  vFecDevices_ = xmlFecDevice.getDevices ( partitionName, fecHardwareId, versionMajor, versionMinor ) ;
	else 
	  vFecDevices_ = xmlFecDevice.getDevices ( partitionName, versionMajor, versionMinor, pMaskVersionMajor, pMaskVersionMinor ) ;
      }
      else { // without the version
	
	if (fecHardwareId != "0")
	  vFecDevices_ = xmlFecDevice.getDevices ( partitionName, fecHardwareId ) ;
	else
	  vFecDevices_ = xmlFecDevice.getDevices ( partitionName ) ;
      }

      // Register the version
      if ((versionMajor != 0) || (versionMinor != 0)) {
	versionMajor_  = versionMajor ;
	versionMinor_  = versionMinor ;
	maskVersionMajor_ = pMaskVersionMajor ;
	maskVersionMinor_ = pMaskVersionMinor ;
	partitionName_ = partitionName ;
	initDbVersion_ = true ;
      }
      else {

	// Current state, check the value
	unsigned int partitionNumber ;
	getPartitionVersion(partitionName, &versionMajor_, &versionMinor_, &maskVersionMajor_, &maskVersionMinor_, &partitionNumber) ;
	initDbVersion_ = true ;
	partitionName_ = partitionName ;
#ifdef DEBUGMSGERROR
	std::cout << "Partition " << partitionName_ << ": current version is " << versionMajor_ << "." << versionMinor_ << " & mask " << maskVersionMajor_ << "." << maskVersionMinor_ << std::endl ;
#endif
      }
    }

    // Copy the descriptions into the output for database if already initialised
    outVector = FecFactory::copy ( vFecDevices_, allDevices ) ;
  }
  else 
#endif
    {
      // Copy the descriptions into the output for file
      outVector = FecFactory::copy ( vFecDevices_, allDevices ) ;
    }
}


/** Retreive the descriptions for all devices for a given FEC hardware id
 * \param partitionName - partition name
 * \param fecHardwareId - FEC hardware ID
 * \param versionMajor - major version for database
 * \param versionMinor - minor version for database
 * \param maskVersionMajor - mask version major
 * \param maskVersionMinor - mask version minor
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned
  * \param allDevices - return all devices including the disable device, by default return only the enabled devices
 * \param forceDbReload - force the database reload
 * \warning if no parameters are given then the devices are retreived from files given by addFileName
 * \warning if the version is -1, -1 the devices are retreived from files given by addFileName
 * \warning if the version is 0, 0 then the devices is coming from the current version
 * \warning if the version is different than 0.0 or -1.-1 then the values are retreived from the version given. Note that this version is set as the current version.
 * \warning if the input if a file whatever the version is, the device description is retreived from the input file.
 */ 
  void FecDeviceFactory::getFecDeviceDescriptions ( std::string partitionName, std::string fecHardwareId, deviceVector &outVector, unsigned int versionMajor, unsigned int versionMinor, bool allDevices, bool forceDbReload  ) 
#ifdef DATABASE
  throw (oracle::occi::SQLException, FecExceptionHandler ) {
#else
  throw (FecExceptionHandler ) {
#endif

    getFecDeviceDescriptions ( false, versionMajor, versionMinor, 0, 0, partitionName, fecHardwareId, outVector, allDevices, forceDbReload ) ;
}

/** Retreive the descriptions for all devices for a given partition name
 * \param partitionName - name of the partition
 * \param versionMajor - major version for database
 * \param versionMinor - minor version for database
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned
 * \param allDevices - return all devices including the disable device, by default return only the enabled devices
 * \param forceDbReload - force the database reload
 * \warning if no parameters are given then the devices are retreived from files given by addFileName
 * \warning if the version is -1, -1 the devices are retreived from files given by addFileName
 * \warning if the version is 0, 0 then the devices is coming from the current version
 * \warning if the version is different than 0.0 or -1.-1 then the values are retreived from the version given. Note that this version is set as the current version.
 * \warning if the input if a file whatever the version is, the device description is retreived from the input file.
 * \warning this method is the same than getFecDeviceDescriptionsPartitionName, it is maintained for backward compatilibility
 */ 
  void FecDeviceFactory::getFecDeviceDescriptions ( std::string partitionName, deviceVector &outVector, unsigned int versionMajor, unsigned int versionMinor, unsigned int pMaskVersionMajor, unsigned int pMaskVersionMinor, bool allDevices, bool forceDbReload ) 
#ifdef DATABASE
  throw (oracle::occi::SQLException, FecExceptionHandler ) {
#else
  throw (FecExceptionHandler ) {
#endif

    getFecDeviceDescriptions ( false, versionMajor, versionMinor, pMaskVersionMajor, pMaskVersionMinor, partitionName, "0", outVector, allDevices, forceDbReload ) ;
}

/** Retreive the FEC description from a file
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned
 * \param allDevices - return all devices including the disable device, by default return only the enabled devices
 */
void FecDeviceFactory::getFecDeviceDescriptions ( deviceVector &outVector, bool allDevices )
#ifdef DATABASE
  throw (oracle::occi::SQLException, FecExceptionHandler ) {
#else
  throw (FecExceptionHandler ) {
#endif

    getFecDeviceDescriptions ( true, 0, 0, 0, 0, "nil", "0", outVector, allDevices ) ;
}


/** Upload the description in the output and upload it in a new version. 
 * \param deviceVector - a vector of device description
 * \param partitionName - if a partition name is specified, try to retreive the version for this given partition name
 * \param versionMajor - value for the version if the database is used as output.
 * \param versionMinor - value for the version if the database is used as input.
 * \param majorVersion - if true (default parameter) then a major version is created, if false a minor version is created. In this case, the version minor created is a child of the current version major set in the current state.
 * \param uploadVersion - if this parameter is set then the version used is 0 for the major version
 * \warning the version parameters are set by the database (there are output parameters)
 * \warning the version can be NULL if the output is a file
 * \warning Note that devices must exist before made this call. 
 * \See creationPartition (deviceVector, int *, int *, std::string, std::string)
 * \warning if you create a minor version, the process will try to get the version for the partition if it is set
 * \warning the version uploaded is set automatically as the next version to be downloaded
 */
void FecDeviceFactory::setFecDeviceDescriptions ( deviceVector devices, std::string partitionName, unsigned int *versionMajor, unsigned int *versionMinor, bool majorVersion, bool uploadVersion )
#ifdef DATABASE
  throw (oracle::occi::SQLException, FecExceptionHandler ) {
#else
  throw (FecExceptionHandler ) {
#endif

#ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if ( getDbUsed() && (versionMajor != NULL) && (versionMinor != NULL) ) 
    std::cout << "setFecDeviceDescriptions for the partition " << partitionName << std::endl ;
  else 
#  endif
    std::cout << "setFecDeviceDescriptions for the file " << outputFileName_ << std::endl ;
#endif
  
  if (devices.empty()) RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded ", ERRORCODE) ;
  
#ifdef DATABASE
  if ( getDbUsed() && (versionMajor != NULL) && (versionMinor != NULL) ) {
    
    // Create an empty XML Fec Device
    XMLFecDevice xmlFecDevice ;
    // Upload in database
    xmlFecDevice.setDatabaseAccess((DbFecAccess *)dbAccess_);

#ifdef DEBUMSGERROR
    if (uploadVersion) std::cout << "New upload version" << std::endl ;
    else if (majorVersion) std::cout << "New version major" << std::endl ;
    else std::cout << "New version minor" << std::endl ;
#endif

    unsigned int versionUpdate = 0 ;
    if (majorVersion) versionUpdate = 1 ;        // upload in next major
    else if (uploadVersion) versionUpdate = 3 ;  // upload in 0.next minor
    else versionUpdate = 0 ;                     // current major . next minor
    // If the version to be created is a major version then all the devices should be added (even the disabled one)
    if (majorVersion && (devices.size() != vFecDevices_.size())) {
      deviceVector toBeUploaded ;
      for (deviceVector::iterator itDev = devices.begin() ; itDev != devices.end() ; itDev ++) {
	toBeUploaded.push_back(*itDev) ;
      }
      for (deviceVector::iterator itVDev = vFecDevices_.begin() ; itVDev != vFecDevices_.end() ; itVDev ++) {
	bool toBeAdded = true ;
	for (deviceVector::iterator itDev = devices.begin() ; itDev != devices.end() ; itDev ++) {
	  if ( ((*itVDev)->getKey()) == ((*itDev)->getKey()) ) toBeAdded = false ;
	}
	if (toBeAdded) toBeUploaded.push_back(*itVDev) ;
      }
      xmlFecDevice.setDevices(toBeUploaded, partitionName, versionUpdate) ;
    }
    else {
      xmlFecDevice.setDevices(devices, partitionName, versionUpdate) ;
    }

    if ( (versionMajor != NULL) && (versionMinor != NULL) ) {
      // Retreive the version that has been uploaded
      unsigned int partitionId ;
      unsigned int maskMajor, maskMinor ;
      getPartitionVersion (partitionName,versionMajor,versionMinor,&maskMajor,&maskMinor,&partitionId) ;
#ifdef DEBUMSGERROR
      std::cout << "Create a new version " << *versionMajor << "." << *versionMinor << std::endl ;
#endif
    }
  }
  else
#endif
    {  
      // Complete all the devices with the disabled device
      deviceVector toBeUploaded ;
      for (deviceVector::iterator itDev = devices.begin() ; itDev != devices.end() ; itDev ++) {
	toBeUploaded.push_back(*itDev) ;
      }
      for (deviceVector::iterator itVDev = vFecDevices_.begin() ; itVDev != vFecDevices_.end() ; itVDev ++) {
	bool toBeAdded = true ;
	for (deviceVector::iterator itDev = devices.begin() ; itDev != devices.end() ; itDev ++) {
	  if ( ((*itVDev)->getKey()) == ((*itDev)->getKey()) ) toBeAdded = false ;
	}
	if (toBeAdded) toBeUploaded.push_back(*itVDev) ;
      }

      // Upload in file
      XMLFecDevice xmlFecDevice ; 
      xmlFecDevice.setDevices (toBeUploaded, outputFileName_) ;
    }
}

/** Retreive the descriptions for the given devices from the input
 * \param partitionName - partition name to be accessed
 * \param dcuHardId - DCU hardware id
 * \param timeStampStart - timestamp start
 * \param timeStampStop - timestamp stop
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned
 * \warning if the input if a file whatever the timestamps are, the device description is retreived from the input file.
 */
void FecDeviceFactory::getDcuValuesDescriptions ( std::string partitionName, 
						  unsigned long dcuHardId,
						  unsigned long timeStampStart, unsigned long timeStampStop, 
						  deviceVector &outVector ) 
  throw (FecExceptionHandler ) {

  // Contrarily for the DCU coming from the files, the DCU values are not 
  // retrieve from the database, a specific method is implemented for it

#ifdef DEBUGMSGERROR
#ifdef DATABASE
  if (getDbUsed()) {
    if (partitionName != "nil") std::cout << "FecDeviceFactory::getDcuValuesDescriptions for the partition " << partitionName 
					  << " for the time range between " << timeStampStart << " and " << timeStampStop << std::endl ;
    else if (dcuHardId != 0) std::cout << "FecDeviceFactory::getDcuValuesDescriptions for the DCU " << dcuHardId 
				       << " for the time range between " << timeStampStart << " and " << timeStampStop << std::endl ;
    else 
      std::cout << "FecDeviceFactory::getDcuValuesDescriptions "
		<< " for the time range between " << timeStampStart << " and " << timeStampStop << std::endl ;
  }
  else
#endif
    std::cout << "Retreive the DCU values from files" << std::endl ;
#endif
  
#ifdef DATABASE
  // retreive the information from database
  if ( getDbUsed() ) {
    
    XMLFecDcu xmlFecDcu ((DbFecAccess *)dbAccess_) ;
    deviceVector vDevice ;
    
    if (partitionName != "nil") {
      vDevice = xmlFecDcu.getDevices ( partitionName, timeStampStart, timeStampStop ) ;
    }
    else if (dcuHardId != 0) {
      vDevice = xmlFecDcu.getDevices ( dcuHardId, timeStampStart, timeStampStop ) ;
    }
    else {
      vDevice = xmlFecDcu.getDevices ( timeStampStart, timeStampStop ) ;
    }
    
    // Copy the DCU into the output device vector
    if (! vDevice.empty()) {
      // For each device, put it in the vector
      for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {
	
	bool equalS = false ;
	
	for (deviceVector::iterator deviceP = vFecDevices_.begin() ; (deviceP != vFecDevices_.end() && ! equalS ) ; deviceP ++) {
	  if (*deviceP == *device) equalS = true ;
	}
	
	if (! equalS) {
	   dcuDescription *dcuD = dynamic_cast<dcuDescription *>(*device) ;
	   outVector.push_back (dcuD) ;
	}
      }
    }
  }
  else
#endif
    {
      // Retreive all devices
      deviceVector allDevices ;
      getFecDeviceDescriptions ( allDevices ) ;
      
      // sort the vFecDevices_ to retreive only the DCU
      // deviceVector dcuVector = FecFactory::getDeviceFromDeviceVector ( allDevices, DCU ) ;
      
      // Copy it only if the settings are ok
      for (deviceVector::iterator device = allDevices.begin() ; device != allDevices.end() ; device ++) {
	if ((*device)->getDeviceType() == DCU) {
	  dcuDescription *dcuD = dynamic_cast<dcuDescription *>(*device) ;
	  outVector.push_back (dcuD) ;
	}
      }
    }
}

/** Retreive the descriptions for the given devices from the input
 * \param partitionName - partition name to be accessed
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned
 * \param timeStampStart - timestamp start
 * \param timeStampStop - timestamp stop
 * \warning if the input if a file whatever the timestamps are, the device description is retreived from the input file.
 */
void FecDeviceFactory::getDcuValuesDescriptions ( std::string partitionName, deviceVector &outVector, 
						  unsigned long timeStampStart, unsigned long timeStampStop ) 
  throw (FecExceptionHandler) {

  getDcuValuesDescriptions ( partitionName, 0, timeStampStart, timeStampStop, outVector ) ;
}

/** Retreive the descriptions for the given devices from the input
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned
 * \param timeStampStart - timestamp start
 * \param timeStampStop - timestamp stop
 * \param allDevices - return all the DCU whatever their status are (enable/disable in the DAQ, enable/disable in the DCU readout). By default, it returns all the DCU
 * \param onlyDcuReadout - return all the DCU that are in the DCU readout, by default return all the devices (do not apply a selection on the enabled DCU readout)
 * \warning if the input if a file whatever the timestamps are, the device description is retreived from the input file.
 */
void FecDeviceFactory::getDcuValuesDescriptions ( deviceVector &outVector, unsigned long timeStampStart, unsigned long timeStampStop ) 
  throw (FecExceptionHandler) {

  getDcuValuesDescriptions ( "nil", 0, timeStampStart, timeStampStop, outVector ) ;
  
}

/** Retreive the descriptions for the given devices from the input
 * \param dcuHardId - dcu hardware id
 * \param timeStampStart - timestamp start
 * \param timeStampStop - timestamp stop
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned
 * \param allDevices - return all the DCU whatever their status are (enable/disable in the DAQ, enable/disable in the DCU readout). By default, it returns all the DCU
 * \param onlyDcuReadout - return all the DCU that are in the DCU readout, by default return all the devices (do not apply a selection on the enabled DCU readout)
 * \warning if the input if a file whatever the timestamps are, the device description is retreived from the input file.
 */
void FecDeviceFactory::getDcuValuesDescriptions ( unsigned long dcuHardId, deviceVector &outVector, unsigned long timeStampStart, unsigned long timeStampStop ) 
  throw (FecExceptionHandler) {

  getDcuValuesDescriptions ( "nil", dcuHardId, timeStampStart, timeStampStop, outVector ) ;
}

/** Upload the description in the output
 * \param deviceVector - a vector of DCU device description
 * \param fileUpload - force the upload into a file
 */
void FecDeviceFactory::setDcuValuesDescriptions ( std::string partitionName, deviceVector dcuDevices, bool fileUpload ) 
  throw (FecExceptionHandler) {

  if (dcuDevices.empty()) RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded", ERRORCODE) ;

#ifdef DATABASE
  if (getDbUsed() && !fileUpload) {

    // Create an empty XML Fec Device
    XMLFecDcu xmlFecDcu ;
    // Upload in database
    xmlFecDcu.setDatabaseAccess((DbFecAccess *)dbAccess_);
    // Upload the values
    xmlFecDcu.setDcuValues(dcuDevices,partitionName) ;
  }
  else
#endif
    {  
      // Upload in file
      XMLFecDevice xmlFecDevice ; 
      xmlFecDevice.setDevices (dcuDevices, outputFileName_) ;
    }
}

// -------------------------------------------------------------
//
//                       CCU descriptions
//
// -------------------------------------------------------------

#ifdef DATABASE

/** Retreive the tkRing description along with the corresponding tkring descriptions for a given partition name
 * \param partitionName - name of the partition in the database
 * \param fecHardwareId - FEC hardware ID
 * \param ring - ring slot
 * \return the resulting TkRingDescription (the memory is kept in the factory)
 */
tkringVector FecDeviceFactory::getDbRingDescription ( std::string partitionName ) 
  throw ( FecExceptionHandler, oracle::occi::SQLException ) {
 
  if (!getDbUsed()) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  if (partitionName == "") RAISEFECEXCEPTIONHANDLER (DB_INVALIDOPERATION, "Partition name is empty", ERRORCODE) ;

  // delete the vector based on a partition
  FecFactory::deleteVectorI(tkRingDescription_) ;

  // retreive the information from database
  
  // DB and XML parsing
  // database access is an attribut of this class called dbAccess_
  // to use it, please use the following variable
  DbFecAccess *dbFecAccess = (DbFecAccess *)dbAccess_ ;
  XMLFecCcu xmlFecCcu (dbFecAccess);

  // Vector of descriptions
  tkRingDescription_ = xmlFecCcu.getRingFromDb( partitionName );

  return tkRingDescription_ ;
}

/** Retreive the tkRing description along with the corresponding tkring descriptions for the given partition name and given FEC hardware ID and Ring
 * \param partitionName - name of the partition in the database
 * \param fecHardwareId - FEC hardware ID
 * \param ring - ring slot
 * \return the resulting TkRingDescription (the memory is kept in the factory)
 */
TkRingDescription* FecDeviceFactory::getDbRingDescription ( std::string partitionName, std::string fecHardwareId, unsigned int ring ) 
  throw ( FecExceptionHandler, oracle::occi::SQLException ) {
 
  if (!getDbUsed()) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  if (partitionName == "") RAISEFECEXCEPTIONHANDLER (DB_INVALIDOPERATION, "Partition name is empty", ERRORCODE) ;

  // delete the vector based on a partition
  FecFactory::deleteVectorI(tkRingDescription_) ;

  // retreive the information from database
  
  // DB and XML parsing
  // database access is an attribut of this class called dbAccess_
  // to use it, please use the following variable
  DbFecAccess *dbFecAccess = (DbFecAccess *)dbAccess_ ;
  XMLFecCcu xmlFecCcu (dbFecAccess);

  TkRingDescription *tkRing = xmlFecCcu.getRingFromDb( partitionName, fecHardwareId, ring );
  tkRingDescription_.push_back(tkRing) ;

  return tkRing ;
}

/** Retreive the tkRing description along with the corresponding tkring descriptions for a given FEC hardware ID and Ring from construction database
 * \param fecHardwareId - FEC hardware ID
 * \param ring - ring slot
 * \return the resulting TkRingDescription (the memory is kept in the factory)
 * \warning if the views/tables from construction TKCC database does not exists then an exception is raised
 */
TkRingDescription* FecDeviceFactory::getDbRingDescription ( std::string fecHardwareId, unsigned int ring ) 
  throw ( FecExceptionHandler, oracle::occi::SQLException ) {
 
  if (!getDbUsed()) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  // delete the vector based on a partition
  FecFactory::deleteVectorI(tkRingDescription_) ;

  // retreive the information from database
  
  // DB and XML parsing
  // database access is an attribut of this class called dbAccess_
  // to use it, please use the following variable
  DbFecAccess *dbFecAccess = (DbFecAccess *)dbAccess_ ;
  XMLFecCcu xmlFecCcu (dbFecAccess);

  TkRingDescription *tkRing = xmlFecCcu.getRingFromDb( fecHardwareId, ring );
  tkRingDescription_.push_back(tkRing) ;

  return tkRing ;
}

/** Upload the tkring descriptions to the database
 * \param partitionName - name of the partition in the database
 * \param ring - ring description
 */
void FecDeviceFactory::setDbRingDescription  ( std::string partitionName, TkRingDescription ringD )  throw ( FecExceptionHandler, oracle::occi::SQLException ) {

  // Set in database
  if ( getDbUsed() && (partitionName != "") ) {

    // DB and XML parsing
    // database access is an attribut of this class called dbAccess_
    // to use it, please use the following variable
    DbFecAccess *dbFecAccess = (DbFecAccess *)dbAccess_ ;
    XMLFecCcu xmlFecCcu;
    
    // Upload in database
    xmlFecCcu.setDatabaseAccess(dbFecAccess);
    
    // Upload the values => raise an exception in case of missing data
    // it also uploads contained CCUs
    xmlFecCcu.setRingToDb(partitionName, ringD );
  }
  else {
    if (!getDbUsed()) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    
    if (partitionName == "") RAISEFECEXCEPTIONHANDLER (DB_INVALIDOPERATION, "Partition name is empty", ERRORCODE) ;
  }
}

#endif

/** Retreive the TkRingDescription from a file
 *  along with the corresponding CCU descriptions
 *  for the given partition name and given FEC hardware ID and Ring
 * \param fileName - file name for the CCU descriptions
 * \param fecHardwareId - FEC hardware ID
 * \param ring - ring slot
 * \return the resulting TkRingDescription (the memory is kept in the factory)
 */
TkRingDescription* FecDeviceFactory::getFileRingDescription ( std::string fileName, std::string fecHardwareId, unsigned int ring )
  throw ( FecExceptionHandler ) {

  // delete the vector of ring descriptions
  FecFactory::deleteVectorI(tkRingDescription_) ;
  
  // retreive the information from file
  XMLFecCcu xmlFecCcu(fileName);
  TkRingDescription *tkRing = xmlFecCcu.getRingFromBuffer ( fecHardwareId, ring );
  tkRingDescription_.push_back(tkRing) ;

  return tkRing ;
}

/** Retreive the TkRingDescription from a file
 *  along with the corresponding CCU descriptions
 *  for the given partition name and given FEC hardware ID and Ring
 * \param fileName - file name for the CCU descriptions
 * \return a vector of TkRingDescription (the memory is kept in the factory)
 * \warning the upload is done only one ring at a time so only one ring will be returned here
 */
tkringVector FecDeviceFactory::getFileRingDescription ( std::string fileName )
  throw ( FecExceptionHandler ) {

  // delete the vector of ring descriptions
  FecFactory::deleteVectorI(tkRingDescription_) ;
  
  // retreive the information from file
  XMLFecCcu xmlFecCcu(fileName);
  tkRingDescription_ = xmlFecCcu.getRingFromBuffer ( );

  return tkRingDescription_ ;
}

/** Upload the tkring descriptions to a file
 * \param fileName - file name of the output
 * \param inVector - the input vector
 */
void FecDeviceFactory::setFileRingDescription ( std::string fileName, TkRingDescription ringD )
  throw ( FecExceptionHandler ) {

  XMLFecCcu xmlFecCcu;
  // Upload to File
  xmlFecCcu.setRingToFile(ringD, fileName);  
}

