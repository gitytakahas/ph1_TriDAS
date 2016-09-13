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

#include <time.h>  // for time function

#include "FecFactory.h" // for static methods
#include "XMLFecPiaReset.h"
#include "PiaResetFactory.h"

#ifdef DATABASE
#include "DbPiaResetAccess.h"
#endif

/** Build a PiaResetFactory to retreive information from database or file
 * \param databaseAccess - if true create a database access
 */
PiaResetFactory::PiaResetFactory ( ):
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

  setUsingFile() ;
}

#ifdef DATABASE

/** Create an access to the database FEC database
 * \param login - login to database
 * \param password - password to database
 * \param path - URL to acess the database
 * \param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 */
PiaResetFactory::PiaResetFactory ( std::string login, std::string password, std::string path, bool threaded ) 
  throw ( oracle::occi::SQLException ):
  DeviceFactoryInterface ( login, password, path, threaded ) {

  setDatabaseAccess ( login, password, path ) ;
}

/** Create an access to the FEC database
 * \param dbFecAccess - database access
 */
PiaResetFactory::PiaResetFactory ( DbPiaResetAccess *dbAccess ):
  DeviceFactoryInterface ((DbCommonAccess *)dbAccess) {
}

#endif

/** Disconnect the database (if it is set)
 */  
PiaResetFactory::~PiaResetFactory ( ) {
  
  // The database access is deleted in the DeviceFactoryInterface
  
  // Emptyied the list of description (FEC PIA resets)
  FecFactory::deleteVectorI (vPiaFecReset_) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 */
void PiaResetFactory::addFileName ( std::string fileName ) {

  // XMLFecPiaReset does not delete the devices
  piaResetVector deleteVector ;

  // For FEC PIA devices
  XMLFecPiaReset xmlFecPiaReset ( fileName ) ;
  
  // Retreive the PIA reset devices
  piaResetVector piaReset = xmlFecPiaReset.getPiaResets() ;
  
  // Merge the vector from the class and the new vector
  // vPiaFecReset_.merge (*piaReset) ;
  for (piaResetVector::iterator device = piaReset.begin() ; device != piaReset.end() ; device ++) {
      
    bool equalS = false ;
      
    for (piaResetVector::iterator deviceP = vPiaFecReset_.begin() ; (deviceP != vPiaFecReset_.end() && ! equalS ) ; deviceP ++) {
      if (*deviceP == *device) equalS= true ;
    }
      
    if (! equalS) 
      vPiaFecReset_.push_back (*device) ;
    else 
      deleteVector.push_back(*device) ;
  }

  // All devices are deleted by the XMLFecDevice so nothing must be deleted

#ifdef DATABASE
  useDatabase_ = false ;
#endif

  // delete the devices not used
  FecFactory::deleteVectorI (deleteVector) ;
}

/** Add a new file name and parse it to retreive the information needed and make a sort on the FEC hardware ID
 * \param fileName - name of the XML file
 * \param fecHardwareId - FEC hardware ID
 */
void PiaResetFactory::addFileName ( std::string fileName, std::string fecHardwareId ) {

  // XMLFecPiaReset does not delete the devices
  piaResetVector deleteVector ;

  // Add new entries
  PiaResetFactory::addFileName (fileName) ;

  // Copy the vector
  piaResetVector vPiaReset = vPiaFecReset_ ;
  vPiaFecReset_.clear() ;

  // Now check the fecHardwareId and then remove the devices not declared
  for (piaResetVector::iterator device = vPiaReset.begin() ; (device != vPiaReset.end()) ; device ++) {

    piaResetDescription *pia = *device ;
    if (pia->getFecHardwareId() == fecHardwareId) 
      vPiaFecReset_.push_back(pia) ;
    else 
      deleteVector.push_back(pia) ;
  }

  // Delete the intermediate vector
  FecFactory::deleteVectorI (deleteVector) ;
}


/** Set a file as the new input
 * \param inputFileName - new input file
 */
void PiaResetFactory::setInputFileName ( std::string inputFileName ) {

  // delete the old vector that is not more usefull
  FecFactory::deleteVectorI (vPiaFecReset_) ;

  // Add new entries
  PiaResetFactory::addFileName (inputFileName) ;
}

/** Set a file as the new input and make a sort on the FEC hardware ID
 * \param inputFileName - new input file
 * \param fecHardwareId - FEC hardware ID
 */
void PiaResetFactory::setInputFileName ( std::string inputFileName, std::string fecHardwareId ) {

  // delete the old vector that is not more usefull
  FecFactory::deleteVectorI (vPiaFecReset_) ;

  // Add new entries
  PiaResetFactory::addFileName (inputFileName, fecHardwareId) ;
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
 * \warning in the destructor of PiaResetFactory, the dbAccess is deleted
 */
void PiaResetFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) 
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
    dbAccess_ = new DbPiaResetAccess (login, password, path, threaded_) ;
  }

  deleteDbAccess_ = true ;
  useDatabase_ = true ;
}

/** 
 * \param dbAccess - FEC access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void PiaResetFactory::setDatabaseAccess ( ) 
  throw ( oracle::occi::SQLException ) {

  std::string login, password, path ;
  if (getDatabaseConfiguration(login, password, path)) {
    setDatabaseAccess (login, password, path) ;
  }
}

/** 
 * \param dbAccess - FEC access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void PiaResetFactory::setDatabaseAccess ( DbPiaResetAccess *dbAccess ) {

  // For FEC database delete the old one if needed
  DeviceFactoryInterface::setDatabaseAccess ((DbCommonAccess *)dbAccess) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Partition / version methods
//
// ------------------------------------------------------------------------------------------------------

/** Upload the description in the output
 * \param deviceVector - a vector of device description
 * \warning the PIA reset must not exists in the database
 * \bug is this method is usefull. In the FecDeviceFactory, it is mandatory to create or not the partition but is it necessary in the PiaResetFactory
 */
void PiaResetFactory::createPartition ( piaResetVector devices, std::string partitionName ) 
  throw ( FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
  std::cout << "PiaResetFactory::createPartition ( piaResetVector devices, std::string partitionName = \"\" )"  << std::endl ;
#endif

  if (devices.empty()) RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded ", ERRORCODE) ;

#ifdef DATABASE
  if (dbAccess_ != NULL) {

    // Upload in database
    // Create an XMLFecDevice for the upload in database
    XMLFecPiaReset xmlFecPiaReset ;
    xmlFecPiaReset.setDatabaseAccess ((DbPiaResetAccess *)dbAccess_);
    xmlFecPiaReset.setPiaResetVector (devices) ;

#ifdef DEBUGMSGERROR
    FecFactory::display (devices) ;
#endif

    xmlFecPiaReset.setDbPiaResets ( partitionName ) ;
  }
  else
#endif
    {  // Upload in file
      setPiaResetDescriptions ( devices ) ;
    }
}

#endif

// ------------------------------------------------------------------------------------------------------
// 
// PIA reset methods
//
// ------------------------------------------------------------------------------------------------------

/** Retreive the descriptions for the given devices from the input
 * \param fileUsed - use files
 * \param partitionName - partition name
 * \param fecHardwareId - FEC hardware ID
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned, the pointers are put in the output vector
 * \param allDevices - return all devices including the disable device
 * \param forceDbReload - force the database reload
 * One the following parameter must be specified and it is checked in the following order
 * <ul>
 * <li> partitionName
 * <li> fecHardwareId and partition name
 * </ul>
 * \warning if the version is -1, -1 and the input is database, then the current state
 * is retreived.
 * \warning if the input if a file whatever the version is, the device description is
 * retreived from the input file.
 */
void PiaResetFactory::getPiaResetDescriptions ( bool fileUsed, std::string partitionName, std::string fecHardwareId, piaResetVector &outVector, bool allDevices, bool forceDbReload  ) 
  throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  if (fileUsed) 
    std::cout << "getPiaResetDescriptions from the files set" << std::endl ;
  else {
    std::cout << "getPiaResetDescriptions for the partition " << partitionName ;
    if (fecHardwareId != "0") 
      std::cout << " and for the FEC hardware id " << fecHardwareId << std::endl ;
    else std::cout << std::endl ;
  }
#endif

#ifdef DATABASE
  // retreive the information from database
  if ( getDbUsed() && !fileUsed ) {

    // delete the old vector that is not more usefull
    FecFactory::deleteVectorI (vPiaFecReset_) ;

    // For FEC PIA devices
    XMLFecPiaReset xmlFecPiaReset( (DbPiaResetAccess *)dbAccess_ ) ;
    if (fecHardwareId != "0" ) 
      vPiaFecReset_ = xmlFecPiaReset.getPiaResets(partitionName, fecHardwareId) ;
    else 
      vPiaFecReset_ = xmlFecPiaReset.getPiaResets(partitionName) ;
  }
#endif

#ifdef DEBUGMSGERROR
  std::cout << "end of getPiaResetDescriptions with " << std::dec << vPiaFecReset_.size() << " PIA reset" << std::endl ;
#endif

  // Copy the descriptions into the output
  outVector = FecFactory::copy ( vPiaFecReset_, allDevices ) ;
}

/** Retreive the descriptions for the given devices from the input for a FEC hardware id
 * \param partitionName - partition name
 * \param fecHardwareId - FEC hardware ID
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned, the pointers are put in the output vector is retreived.
 * \param allDevices - return all devices including the disable device
 * \param forceDbReload - force the database reload
 * \warning if the input if a file whatever the version is, the device description is
 * retreived from the input file.
 */
void PiaResetFactory::getPiaResetDescriptions ( std::string partitionName, std::string fecHardwareId, piaResetVector &outVector, bool allDevices, bool forceDbReload )
  throw (FecExceptionHandler) {

  getPiaResetDescriptions(false, partitionName, fecHardwareId, outVector, allDevices, forceDbReload) ;
}

/** Retreive the descriptions for the given devices from the input for a given partition name
 * \param partitionName - name of the partition
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned, the pointers are put in the output vector
 * \param allDevices - return all devices including the disable device
 * \param forceDbReload - force the database reload
 * \warning if the version is -1, -1 and the input is database, then the current state
 * is retreived.
 * \warning if the input if a file whatever the version is, the device description is
 * retreived from the input file.
 * \warning this method is the same than getPiaResetDescriptionsPartitionName, it is maintained for backward compatibility
 */
void PiaResetFactory::getPiaResetDescriptions ( std::string partitionName, piaResetVector &outVector, bool allDevices, bool forceDbReload )
  throw (FecExceptionHandler) {

  getPiaResetDescriptions(false, partitionName, "0", outVector, allDevices, forceDbReload) ;
}

/** Retreive the descriptions from a file
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned, the pointers are put in the output vector
 * \param allDevices - return all devices including the disable device
 */
void PiaResetFactory::getPiaResetDescriptions ( piaResetVector &outVector, bool allDevices )
  throw (FecExceptionHandler) {

  getPiaResetDescriptions(true, "nil", "0", outVector, allDevices ) ;
}


/** Upload the description in the output and upload it in a new version. 
 * \param deviceVector - a vector of device description
 * \param partitionName - if a partition name is specified, try to retreive the version for this given partition name
 * \warning the version parameters are set by the database (there are output parameters)
 * \warning the version can be NULL if the output is a file
 * \warning the PIA reset must not exists in the database
 * \warning if you create a minor version, the process will try to get the version for the partition if it is set or FEC hardware ID and partition name
 */
void PiaResetFactory::setPiaResetDescriptions ( piaResetVector devices, std::string partitionName ) 
  throw ( FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if ( getDbUsed() ) 
    std::cout << "setPiaResetDescriptions for the partition " << partitionName << std::endl ;
  else 
#  endif
    std::cout << "setPiaResetDescriptions for the file " << outputFileName_ << std::endl ;
#endif

  if (devices.empty()) RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded ", ERRORCODE) ;

#ifdef DATABASE
  if ( getDbUsed() ) {

#ifdef DEBUGMSGERROR
    FecFactory::display (devices) ;
#endif    
    
    // Create an empty XML Fec Device and upload in DB
    XMLFecPiaReset xmlFecPiaReset ;
    xmlFecPiaReset.setDatabaseAccess((DbPiaResetAccess *)dbAccess_);
    xmlFecPiaReset.setDbPiaResets(devices, partitionName) ;
    
#ifdef DEBUMSGERROR
    std::cout << "PIA reset has been updated" << std::endl ;
#endif
  }
  else
#endif
    {  // Upload in file

      // Put all the PIA even the disable one
      if (devices.size() != vPiaFecReset_.size()) {

	piaResetVector toBeUploaded ;
	for (piaResetVector::iterator itDev = devices.begin() ; itDev != devices.end() ; itDev ++) {
	  toBeUploaded.push_back(*itDev) ;
	}
	for (piaResetVector::iterator itVDev = vPiaFecReset_.begin() ; itVDev != vPiaFecReset_.end() ; itVDev ++) {
	  bool toBeAdded = true ;
	  for (piaResetVector::iterator itDev = devices.begin() ; itDev != devices.end() ; itDev ++) {
	    if ( ((*itVDev)->getKey()) == ((*itDev)->getKey()) ) toBeAdded = false ;
	  }
	  if (toBeAdded) toBeUploaded.push_back(*itVDev) ;
	}

	XMLFecPiaReset xmlPiaReset ;
	xmlPiaReset.setFilePiaResets (toBeUploaded, outputFileName_) ;
      }
      else {
	XMLFecPiaReset xmlPiaReset ;
	xmlPiaReset.setFilePiaResets (devices, outputFileName_) ;
      }
    }
}
