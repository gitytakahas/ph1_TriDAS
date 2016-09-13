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


#ifndef DBCONNECTIONSACCESS_H
#define DBCONNECTIONSACCESS_H
#include "DbAccess.h"

/** \brief This class is implemented to handle the communication between the FEC supervisor software and the database.
 *
 * This class provides some features like :
 *  - OCCI environnement and database connection opening and closing 
 *  - download and upload execution
 *  - database version request
 *
 */
class DbConnectionsAccess : public DbAccess {
 private:
  // 
  // private attributes
  //

 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  DbConnectionsAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Constructor with connection parameters
   */
  DbConnectionsAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Destructor
   */
  ~DbConnectionsAccess ( ) throw (oracle::occi::SQLException);

  /** \brief Retreive the version for each partition in the current state
   */
  std::list<unsigned int*> getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Retrieves the next minor version with major version = majorId for database upload
   */
  unsigned int getNextMinorVersion(unsigned int majorId) throw (FecExceptionHandler);
 
  /** \brief Retrives the next major version for database upload
   */
  unsigned int getNextMajorVersion() throw (FecExceptionHandler);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException) ;

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getApvNotConnectedWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) ;

  /** \brief Upload a Clob to the database
   */
  void setXMLClobWithVersion (std::string buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException);

  /** \brief Upload a Clob to the database
   */
  void setXMLClob (std::string buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Deprecated method not used and not need, an exception is raised on a call on this method
   * \exception FecExceptionHandler
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, std::string hardId, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) {
    RAISEFECEXCEPTIONHANDLER( CODECONSISTENCYERROR, 
			      "deprecated, no version is available for a partition and an FEC hard ID", 
			      FATALERRORCODE) ;
    return NULL ;
  }

  /** \brief Deprecated method not used and not need, an exception is raised on a call on this method
   * \exception FecExceptionHandler
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName, std::string hardId) throw (oracle::occi::SQLException) {
    RAISEFECEXCEPTIONHANDLER( CODECONSISTENCYERROR, 
			      "deprecated, no version is available for a partition and an FEC hard ID", 
			      FATALERRORCODE) ;
    return NULL ;
  }
};

#endif
