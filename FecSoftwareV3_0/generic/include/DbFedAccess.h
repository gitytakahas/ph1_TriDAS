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


#ifndef DBFEDACCESS_H
#define DBFEDACCESS_H
#include "DbAccess.h"

// ostream use for int to string conversion 
#include <sstream>
#include "stringConv.h"

/** \brief This class is implemented to handle the communication between the FEC supervisor software and the database.
 *
 * This class provides some features like :
 *  - OCCI environnement and database connection opening and closing 
 *  - download and upload execution
 *  - database version request
 *
 */
class DbFedAccess : public virtual DbAccess {
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
  DbFedAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Constructor with connection parameters
   */
  DbFedAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Destructor
   */
  ~DbFedAccess ( ) throw (oracle::occi::SQLException);

  /** \brief Retreive the version, in the current state, for the given partition name
   */
  std::list<unsigned int*> getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException) ;

  /** \brief Create a new current state with a set of partitions-versions
   */
  void setDatabaseVersion(std::list<unsigned int*> partitionVersionsListe) throw (FecExceptionHandler) ;

  /** \brief Retrieves the next minor version with major version = majorId for database upload
   */
  unsigned int getNextMinorVersion(unsigned int majorId) throw (FecExceptionHandler);
 
  /** \brief Retrives the next major version for database upload
   */
  unsigned int getNextMajorVersion() throw (FecExceptionHandler);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName)throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName, boolean withStrip )throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor, boolean withStrip) throw (oracle::occi::SQLException);

  /** \brief Download parameters from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName, std::string id) throw (oracle::occi::SQLException);

  /** \brief Download parameters from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, std::string id, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName, unsigned int fedId) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName, unsigned int fedId, boolean hardId) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion(std::string partitionName, unsigned int fedId, boolean hardId, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob(std::string partitionName, unsigned int fedId, boolean hardId, boolean withStrip) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion(std::string partitionName, unsigned int fedId, boolean hardId, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor, boolean withStrip) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClobWithVersion(std::string partitionName, unsigned int fedId, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException);

  /** \brief Upload a Clob from the database
   */
   void setXMLClob (std::string buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Upload a Clob from the database
   */
  void setXMLClobWithVersion (std::string buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException, FecExceptionHandler);

  void setXMLClob(std::string* buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Download a Clob from the database, not usec by FED
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, std::string hardId, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) {
    RAISEFECEXCEPTIONHANDLER( CODECONSISTENCYERROR, 
			       "Incorrect FED database call",
			       FATALERRORCODE ) ;
    return NULL ;
  }

};

#endif
