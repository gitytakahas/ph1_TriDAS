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

  Copyright 2002 - 2007, Frederic Drouhin, UHA

*/

#include "DbTkMaskModulesAccess.h"

/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess()
 */
DbTkMaskModulesAccess::DbTkMaskModulesAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbCommonAccess (threaded) {
}

/**Create an access to the database
 * @param user - user name
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess(std::string user, std::string passwd, std::string dbPath)
 */
DbTkMaskModulesAccess::DbTkMaskModulesAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbCommonAccess (user, passwd, dbPath, threaded) {
}

/**Close the access to the database
 * @exception oracle::occi::SQLException
 * @see DbAccess::~DbAccess()
 */
DbTkMaskModulesAccess::~DbTkMaskModulesAccess ()  throw (oracle::occi::SQLException) {
}

/**
 * Return the mask version for partition
 * \param pPartitionName - partition name
 * \param pPartitionId - partition ID
 * \param pVersionMajor - mask version major ID
 * \param pVersionMinor - mask version minor ID
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 */
void DbTkMaskModulesAccess::getCurrentMaskVersion ( std::string pPartitionName, unsigned int *pPartitionId, unsigned int *pVersionMajor, unsigned int *pVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string sqlQuery = "SELECT DISTINCT Partition.partitionId, StateHistory.maskVersionMajorId, StateHistory.maskVersionMinorId FROM CurrentState, StateHistory, Partition  WHERE StateHistory.stateHistoryId = CurrentState.stateHistoryId AND StateHistory.partitionId=Partition.partitionId AND Partition.partitionName = :partitionName" ;

  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 
  std::list<unsigned int*> partitionVersionsListe ;

  try {
    unsigned int i = 0 ;
    stmt = dbConnection_->createStatement (sqlQuery);
    stmt->setString(++i, pPartitionName);
    rset = stmt->executeQuery ();

    while (rset->next ()) {

      *pPartitionId = (unsigned int)rset->getUInt (1);
      *pVersionMajor = (unsigned int)rset->getUInt (2);
      *pVersionMinor = (unsigned int)rset->getUInt (3);
    }
    
    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}

/**\brief copy a mask from a version for a given partition to a new major version
 * \param pPartitionName - partition name
 * \param pVersionMajor - mask version major ID
 * \param pVersionMinor - mask version minor ID
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::copyMask
 */
void DbTkMaskModulesAccess::copyMask ( std::string pPartitionName, unsigned int pVersionMajor, unsigned int pVersionMinor, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.copyMask(:pPartitionName, :pVersionMajor, :pVersionMinor); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pVersionMajor);
    stmt->setUInt (++i, pVersionMinor);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}

/** 
 * \brief copy a mask from the current version for a given partition to a new major version
 * \param pPartitionName - partition name*
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::copyMask
 */
void DbTkMaskModulesAccess::copyMask ( std::string pPartitionName, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.copyMask(:pPartitionName); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}

/** 
 * Reset the masking version for a given partition. This method retreive the versions before the mask and put it to the StateHistory/CurrentState
 * \param pPartitionName - partition name
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::resetMask
 */
void DbTkMaskModulesAccess::resetMask ( std::string pPartitionName) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.resetMask(:pPartitionName); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/**
 * Disable all modules for a FED
 * \param pFedSoftId - FED soft ID
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFedModules
 */
void DbTkMaskModulesAccess::maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFedModules(:pPartitionName, :pFedSoftId, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pFedSoftId);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}

/**
 * Disable all modules for a FED channel
 * \param pFedSoftId - 	FED soft ID
 * \param pFedChannel - FED channel (calculation is done to retreive in DB view => FED, FEFPGA, CHANNELPAIR, CHANNEL)
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFedModules
 */
void DbTkMaskModulesAccess::maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int pFedChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFedModules(:pPartitionName, :pFedSoftId, :pFedChannel, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pFedSoftId);
    stmt->setUInt (++i, pFedChannel);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}

/**
 * Disable all modules for a FED channel and a given APV (1 or 2)
 * \param pFedSoftId - 	FED soft ID
 * \param pFedChannel - FED channel (calculation is done to retreive in DB view => FED, FEFPGA, CHANNELPAIR, CHANNEL)
 * \param pApvFed - FED APV number
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFedModules
 */
void DbTkMaskModulesAccess::maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int pFedChannel, unsigned int pApvFed, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFedModules(:pPartitionName, :pFedSoftId, :pFedChannel, :pApvFed, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pFedSoftId);
    stmt->setUInt (++i, pFedChannel);
    stmt->setUInt (++i, pApvFed);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}

/** 
 * Disable all modules for a FEC
 * \param pPartitionName - partition name
 * \param pFecSlot - FEC slot
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecSlot, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pFecSlot);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}
  
/** 
 * Disable all modules for a FEC
 * \param pPartitionName - partition name
 * \param pFecHardId - FEC hardware ID
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecHardId, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setString (++i, pFecHardId);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}
  
/** 
 * Disable all modules for a FEC, ring
 * \param pPartitionName - partition name
 * \param pFecSlot - FEC slot
 * \param pRing	- ring number
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecSlot, :pRing, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pFecSlot);
    stmt->setUInt (++i, pRing);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}
  
/** 
 * Disable all modules for a FEC, ring
 * \param pPartitionName - partition name
 * \param pFecHardId - FEC hardware ID
 * \param pRing - ring number
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecHardId, :pRing, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setString (++i, pFecHardId);
    stmt->setUInt (++i, pRing);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ;
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor); 
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
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecSlot, :pRing, :pCcuAddress, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pFecSlot);
    stmt->setUInt (++i, pRing);
    stmt->setUInt (++i, pCcuAddress);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
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
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecHardId, :pRing, :pCcuAddress, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setString (++i, pFecHardId);
    stmt->setUInt (++i, pRing);
    stmt->setUInt (++i, pCcuAddress);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
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
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecSlot, :pRing, :pCcuAddress, :pChannel, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pFecSlot);
    stmt->setUInt (++i, pRing);
    stmt->setUInt (++i, pCcuAddress);
    stmt->setUInt (++i, pChannel);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
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
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecHardId, :pRing, :pCcuAddress, :pChannel, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setString (++i, pFecHardId);
    stmt->setUInt (++i, pRing);
    stmt->setUInt (++i, pCcuAddress);
    stmt->setUInt (++i, pChannel);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
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
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int pAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecSlot, :pRing, :pCcuAddress, :pChannel, :pAddress, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pFecSlot);
    stmt->setUInt (++i, pRing);
    stmt->setUInt (++i, pCcuAddress);
    stmt->setUInt (++i, pChannel);
    stmt->setUInt (++i, pAddress);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
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
void DbTkMaskModulesAccess::maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int pAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskFecModules(:pPartitionName, :pFecHardId, :pRing, :pCcuAddress, :pChannel, :pAddress, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setString (++i, pFecHardId);
    stmt->setUInt (++i, pRing);
    stmt->setUInt (++i, pCcuAddress);
    stmt->setUInt (++i, pChannel);
    stmt->setUInt (++i, pAddress);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}
  
/**
 * mask a module with DCU hard ID
 * \param pPartitionName - partition name
 * \param dcuHardId - dcu hard id
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see maskFecModules ( std::string pPartitionName, pFecHardId, pRing, pCcuAddress, pChannel, unsigned int newMajorVersion )
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void DbTkMaskModulesAccess::maskDcuModule ( std::string pPartitionName, unsigned int pDcuHardId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskDcuModule(:pPartitionName, :pDcuHardId, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pDcuHardId);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}
  
/**
 * mask a module with DET ID
 * \param pPartitionName - partition name
 * \param detId - DET ID
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see maskDcuModule ( std::string pPartitionName, dcuHardId, unsigned int newMajorVersion);
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void DbTkMaskModulesAccess::maskDetIdModule ( std::string pPartitionName, unsigned int pDetId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskDetIdModule(:pPartitionName, :pDetId, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setUInt (++i, pDetId);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}
  
/**
 * mask all modules related to PSU name
 * \param pPartitionName - partition name
 * \param pPsuName	- PSU name that should be a complete name
 * \param newMajorVersion - 1 means that you create a new version, 0 means you add to the current state the devices to be disabled
 * \see maskDcuModule ( std::string pPartitionName, dcuHardId, unsigned int newMajorVersion);
 * \see Database/Packages/src/PkgMaskModules.sql, PkgMaskModules::maskFecModules
 */
void DbTkMaskModulesAccess::maskPsuNameModules ( std::string pPartitionName, std::string pPsuName, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor) throw (oracle::occi::SQLException) { 

  static std::string queryString = "BEGIN PkgMaskModules.maskPsuNameModules(:pPartitionName, :pPsuName, :newMajorVersion); END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (queryString);

    unsigned int i = 0;
    stmt->setString (++i, pPartitionName);
    stmt->setString (++i, pPsuName);
    stmt->setUInt (++i, newMajorVersion);

    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  // Get the current version that is updated when the mask is done
  unsigned int partitionId ; 
  getCurrentMaskVersion(pPartitionName,&partitionId,outMaskVersionMajor,outMaskVersionMinor) ;
}

