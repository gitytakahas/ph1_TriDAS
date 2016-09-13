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
  
  Copyright 2002 - 2007, G. BAULIEU - IPNL/IN2P3
*/
  
#include "TkMaskModulesFactory.h"

/** Build a TkMaskModulesFactory to retreive information from database or file
 * \param databaseAccess - if true create a database access
 */
TkMaskModulesFactory::TkMaskModulesFactory ( ):
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

  //   if (! databaseAccess) setUsingFile ( ) ;

  setUsingFile ( ) ;
}

#ifdef DATABASE

/** Create an access to the database FEC database
 * \param login - login to database
 * \param password - password to database
 * \param path - URL to acess the database
 * \param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 */
TkMaskModulesFactory::TkMaskModulesFactory ( std::string login, std::string password, std::string path, bool threaded ) 
  throw ( oracle::occi::SQLException ):
  DeviceFactoryInterface ( login, password, path, threaded ) {
  setDatabaseAccess ( login, password, path ) ;
}

/** Create an access to the FEC database
 * \param dbFecAccess - database access
 */
TkMaskModulesFactory::TkMaskModulesFactory ( DbTkMaskModulesAccess *dbAccess ):
  DeviceFactoryInterface ((DbAccess *)dbAccess) {
}

#endif

/** Disconnect the database (if it is set)
 */  
TkMaskModulesFactory::~TkMaskModulesFactory ( ) {
  
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 */
void TkMaskModulesFactory::addFileName ( std::string fileName ) {

  RAISEFECEXCEPTIONHANDLER( CODECONSISTENCYERROR, "This method cannot be used since the masking is only usable with database",FATALERRORCODE ) ;
}

/** Set a file as the new input
 * \param inputFileName - new input file
 */
void TkMaskModulesFactory::setInputFileName ( std::string inputFileName ) {
 
  RAISEFECEXCEPTIONHANDLER( CODECONSISTENCYERROR, "This method cannot be used since the masking is only usable with database",FATALERRORCODE ) ;
}


// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** Retreive the version for DCU info given the partition name
 * \param partitionName - partitionName
 * \param major - major version returned
 * \param minor - minor version returned
 * \param partitionNumber - partition number returned
 * The parameters must be deleted by the remote method
 * Note that the method returns always a pointer allocated but the list can be empty
 */
void TkMaskModulesFactory::getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *partitionNumber ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->getCurrentMaskVersion(partitionName,partitionNumber,major,minor) ;
}

/** 
 * \param login - login to database
 * \param password - password to database
 * \param path - path to database
 * \warning in the destructor of TkMaskModulesFactory, the dbAccess is deleted
 */
void TkMaskModulesFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) 
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
    dbAccess_ = (DbAccess *)new DbTkMaskModulesAccess (login, password, path, threaded_) ;
  }

  deleteDbAccess_ = true ;
  useDatabase_ = true ;
}

/** 
 * \param dbAccess - FEC access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void TkMaskModulesFactory::setDatabaseAccess ( ) 
  throw ( oracle::occi::SQLException ) {

  std::string login, password, path ;
  if (getDatabaseConfiguration(login, password, path)) {
    setDatabaseAccess (login, password, path) ;
  }
}

/** 
 * \param dbAccess - access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void TkMaskModulesFactory::setDatabaseAccess ( DbTkMaskModulesAccess *dbAccess ) {

  // For FEC database delete the old one if needed
  DeviceFactoryInterface::setDatabaseAccess ((DbAccess *)dbAccess) ;
}

#endif

// ------------------------------------------------------------------------------------------------------
// 
// Methods to mask modules (directly mapped from DbTkMaskModulesAccess
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/**
 * Return the mask version for partition
 * \param pPartitionName - partition name
 * \param pPartitionId - partition ID
 * \param pVersionMajor - mask version major ID
 * \param pVersionMinor - mask version minor ID
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 */
void TkMaskModulesFactory::getCurrentMaskVersion ( std::string pPartitionName, unsigned int *pPartitionId, unsigned int *pVersionMajor, unsigned int *pVersionMinor ) throw (oracle::occi::SQLException, FecExceptionHandler) {

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->getCurrentMaskVersion(pPartitionName,pPartitionId,pVersionMajor,pVersionMinor);
}

/**\brief copy a mask from a version for a given partition to a new major version
 * \param pPartitionName - partition name
 * \param pVersionMajor - mask version major ID
 * \param pVersionMinor - mask version minor ID
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::copyMask
 */
void TkMaskModulesFactory::copyMask ( std::string pPartitionName, unsigned int pVersionMajor, unsigned int pVersionMinor, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->copyMask(pPartitionName,pVersionMajor,pVersionMinor,outMaskVersionMajor,outMaskVersionMinor);
}

/** 
 * \brief copy a mask from the current version for a given partition to a new major version
 * \param pPartitionName - partition name*
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::copyMask
 */
void TkMaskModulesFactory::copyMask ( std::string pPartitionName, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->copyMask(pPartitionName,outMaskVersionMajor,outMaskVersionMinor);
}

/** 
 * Reset the masking version for a given partition. This method retreive the versions before the mask and put it to the StateHistory/CurrentState
 * \param pPartitionName - partition name
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::resetMask
 */
void TkMaskModulesFactory::resetMask ( std::string pPartitionName) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->resetMask(pPartitionName);
}

/**
 * Disable all modules for a FED
 * \param pFedSoftId - FED soft ID
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFedModules
 */
void TkMaskModulesFactory::maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFedModules(pPartitionName,pFedSoftId,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}

/**
 * Disable all modules for a FED channel
 * \param pFedSoftId - 	FED soft ID
 * \param pFedChannel - FED channel (calculation is done to retreive in DB view => FED, FEFPGA, CHANNELPAIR, CHANNEL)
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFedModules
 */
void TkMaskModulesFactory::maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int pFedChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFedModules(pPartitionName,pFedSoftId,pFedChannel,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}

/**
 * Disable all modules for a FED channel and a given APV (1 or 2)
 * \param pFedSoftId - 	FED soft ID
 * \param pFedChannel - FED channel (calculation is done to retreive in DB view => FED, FEFPGA, CHANNELPAIR, CHANNEL)
 * \param pApvFed - FED APV number
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFedModules
 */
void TkMaskModulesFactory::maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int pFedChannel, unsigned int pApvFed, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFedModules(pPartitionName,pFedSoftId,pFedChannel,pApvFed,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}

/** 
 * Disable all modules for a FEC
 * \param pPartitionName - partition name
 * \param pFecSlot - FEC slot
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecSlot,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC
 * \param pPartitionName - partition name
 * \param pFecHardId - FEC hardware ID
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecHardId,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC, ring
 * \param pPartitionName - partition name
 * \param pFecSlot - FEC slot
 * \param pRing	- ring number
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecSlot,pRing,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC, ring
 * \param pPartitionName - partition name
 * \param pFecHardId - FEC hardware ID
 * \param pRing - ring number
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecHardId,pRing,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC, ring, CCU
 * \param pPartitionName - partition name
 * \param pFecSlot - FEC slot
 * \param pRing - ring number
 * \param pCcuAddress - CCU address
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecSlot,pRing,pCcuAddress,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC, ring, CCU
 * \param pPartitionName - partition name
 * \param pFecHardId - FEC hardware ID
 * \param pRing	- ring number
 * \param pCcuAddress - CCU address
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecHardId,pRing,pCcuAddress,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC, ring, CCU, channel
 * \param pPartitionName - partition name
 * \param pFecSlot - FEC slot
 * \param pRing - ring number
 * \param pCcuAddress - CCU address
 * \param pChannel - I2C channel
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecSlot,pRing,pCcuAddress,pChannel,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC, ring, CCU, channel
 * \param pPartitionName - partition name
 * \param pFecHardId - FEC hardware ID
 * \param pRing	- ring number
 * \param pCcuAddress - CCU address
 * \param pChannel - i2c channel
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecHardId,pRing,pCcuAddress,pChannel,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC, ring, CCU, channel, device
 * \param pPartitionName - partition name
 * \param pFecSlot - FEC slot
 * \param pRing - ring number
 * \param pCcuAddress - CCU address
 * \param pChannel - i2c channel
 * \param pAddress - i2c address
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int pAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecSlot,pRing,pCcuAddress,pChannel,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/** 
 * Disable all modules for a FEC, ring, CCU, channel, device
 * \param pPartitionName - partition name
 * \param pFecHardId - FEC hardware ID
 * \param pRing	- ring number
 * \param pCcuAddress - CCU address
 * \param pChannel - i2c channel
 * \param pAddress - i2c address
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int pAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskFecModules(pPartitionName,pFecHardId,pRing,pCcuAddress,pChannel,pAddress,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor);
}
  
/**
 * mask a module with DCU hard ID
 * \param pPartitionName - partition name
 * \param dcuHardId - dcu hard id
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see maskFecModules ( std::string pPartitionName, pFecHardId, pRing, pCcuAddress, pChannel, unsigned int newMajorVersion )
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskDcuModule ( std::string pPartitionName, unsigned int pDcuHardId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskDcuModule(pPartitionName,pDcuHardId,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor) ;
}
  
/**
 * mask a module with DET ID
 * \param pPartitionName - partition name
 * \param detId - DET ID
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see maskDcuModule ( std::string pPartitionName, dcuHardId, unsigned int newMajorVersion);
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskDetIdModule ( std::string pPartitionName, unsigned int pDetId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) { 

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  ((DbTkMaskModulesAccess *)dbAccess_)->maskDetIdModule(pPartitionName,pDetId,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor) ;
}

/**
 * mask all modules related to PSU name
 * \param pPartitionName - partition name
 * \param pPsuName	- PSU name that should be a complete name
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see maskDcuModule ( std::string pPartitionName, dcuHardId, unsigned int newMajorVersion);
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void TkMaskModulesFactory::maskPsuNameModules ( std::string pPartitionName, std::string pPsuName, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) {
  
  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  
  ((DbTkMaskModulesAccess *)dbAccess_)->maskPsuNameModules(pPartitionName,pPsuName,newMajorVersion,outMaskVersionMajor,outMaskVersionMinor) ;
}

#endif
