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

  Copyright 2002 - 2007, Frederic Drouhin - UHA

*/
#ifndef DBTKMASKMODULESACCESS_H
#define DBTKMASKMODULESACCESS_H
#include "DbCommonAccess.h"

// ostream use for int to string conversion 
#include <sstream>
#include "stringConv.h"

//declaration of the exception handler for the DCU
#include "FecExceptionHandler.h"

/** \brief This class is implemented to handle the communication between C++ layers and the database.
 * It is dedicated to all masking issues.
 * This class maps exactly the package PkgMaskModules (file Database/Packages/spec/PkgMaskModules).
 */
class DbTkMaskModulesAccess : public DbCommonAccess {

 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  DbTkMaskModulesAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Constructor with connection parameters
   */
  DbTkMaskModulesAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Destructor
   */
  ~DbTkMaskModulesAccess ( ) throw (oracle::occi::SQLException);

  /**
   * \brief Return the mask version for partition
   */
  void getCurrentMaskVersion ( std::string pPartitionName, unsigned int *pPartitionId, unsigned int *pVersionMajor, unsigned int *pVersionMinor ) throw (oracle::occi::SQLException) ;

  /**\brief copy a mask from a version for a given partition to a new major version
   */
  void copyMask ( std::string pPartitionName, unsigned int pVersionMajor, unsigned int pVersionMinor, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;

  /** 
   * \brief copy a mask from the current version for a given partition to a new major version
   */
  void copyMask ( std::string pPartitionName, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;

  /** 
   * \brief Reset the masking version for a given partition. This method retreive the versions before the mask and put it to the StateHistory/CurrentState
   */
  void resetMask ( std::string pPartitionName ) throw (oracle::occi::SQLException) ;

  /**
   * \brief Disable all modules for a FED
   */
  void maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;

  /**
   * \brief Disable all modules for a FED channel
   */
  void maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int pFedChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;

  /**
   * \brief Disable all modules for a FED channel and a given APV (1 or 2)
   */
  void maskFedModules ( std::string pPartitionName, unsigned int pFedSoftId, unsigned int pFedChannel, unsigned int pApvFed, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;

  /** 
   * \brief Disable all modules for a FEC
   */
  void maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC
   */
  void maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC, ring
   */
  void maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC, ring
   */
  void maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC, ring, CCU
   */
  void maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC, ring, CCU
   */
  void maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC, ring, CCU, channel
   */
  void maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC, ring, CCU, channel
   */
  void maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC, ring, CCU, channel, device
   */
  void maskFecModules ( std::string pPartitionName, unsigned int pFecSlot, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int pAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /** 
   * \brief Disable all modules for a FEC, ring, CCU, channel, device
   */
  void maskFecModules ( std::string pPartitionName, std::string pFecHardId, unsigned int pRing, unsigned int pCcuAddress, unsigned int pChannel, unsigned int pAddress, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /**
   * \brief mask a module with DCU hard ID
   */
  void maskDcuModule ( std::string pPartitionName, unsigned int pDcuHardId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /**
   * \brief mask a module with DET ID
   */
  void maskDetIdModule ( std::string pPartitionName, unsigned int pDetId, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
  
  /**
   * \brief mask all modules related to PSU name
   */
  void maskPsuNameModules ( std::string pPartitionName, std::string pPsuName, unsigned int newMajorVersion, unsigned int *outMaskVersionMajor, unsigned int *outMaskVersionMinor ) throw (oracle::occi::SQLException) ;
};

#endif
