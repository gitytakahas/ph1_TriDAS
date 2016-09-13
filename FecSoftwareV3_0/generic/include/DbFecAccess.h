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

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3

*/


#ifndef DBFECACCESS_H
#define DBFECACCESS_H
#include "DbAccess.h"

// ostream use for int to string conversion 
#include <sstream>
#include "stringConv.h"

//declaration of the exception handler for the FEC
#include "FecExceptionHandler.h"



/** \brief This class is implemented to handle the communication between the FEC supervisor software and the database.
 *
 */
class DbFecAccess : public DbAccess {

 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  DbFecAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Constructor with connection parameters
   */
  DbFecAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Destructor
   */
  ~DbFecAccess ( ) throw (oracle::occi::SQLException);

  /** \brief Retreive the version, in the current state, for the given partition name
   */
  std::list<unsigned int*> getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException) ;

  /** \brief Create a new current state with a set of partitions-versions
   */
  void setDatabaseVersion(std::list<unsigned int*> partitionVersionsList) throw (FecExceptionHandler);
 
  /** \brief Retrives the next minor version with major version = majorId for database upload
   */
  unsigned int getNextMinorVersion(unsigned int majorId) throw (oracle::occi::SQLException);
 
  /** \brief Retrives the next major version for database upload
   */
  unsigned int getNextMajorVersion() throw (oracle::occi::SQLException);
 
  /** \brief Creates a new state
   */
  unsigned int createNewStateHistory(std::string partitionName, unsigned int *partitionId, unsigned int *fecVersionMajorId) throw (FecExceptionHandler);
 
  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException);

  /** \brief Download parameters from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName, std::string id) throw (oracle::occi::SQLException);

  /** \brief Download parameters from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, std::string id, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException);


  /** \brief Download CCU parameters from the database for a given partition
   */
  oracle::occi::Clob *getCcuXMLClob(std::string partitionName)
    throw (oracle::occi::SQLException);  

  /** \brief Download CCU parameters from the database for a given Fec/Ring
   */
  oracle::occi::Clob *getCcuXMLClob(std::string partitionName, std::string fecId, unsigned int ring)
    throw (oracle::occi::SQLException);

  /** \brief Download CCU parameters from the database for a given Fec/Ring
   */
  oracle::occi::Clob *getCcuXMLClob(std::string fecHardId, unsigned int ringSlot) 
    throw (oracle::occi::SQLException) ;

  /** \brief Upload a Clob to the database to configure the CCUs
   */
  void setCcuXMLClob(std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** \brief Upload a Clob to the database to configure a ring containing CCUs
   */
  void setRingCcuXMLClob(std::string ringBuffer, std::string ccuBuffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** \brief Upload a Clob to the database for configuration
   */
  unsigned int setXMLClob(std::string* buffer, std::string partitionName, boolean newPartition) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Upload a Clob from the database
   */
  void setXMLClobWithVersion (std::string* buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Upload a Clob to the database for configuration
   */
  void setXMLClob (std::string* buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Upload a Clob to the database for configuration
   */
  void setXMLClob(std::string stringRequest, std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Upload two Clobs at a time to the database for configuration
   */
  void setXMLClob(std::string stringRequest, std::string bufferOne, std::string bufferTwo, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);

};

#endif
