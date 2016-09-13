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

  Copyright 2002 - 2007, Guillaume BAULIEU - IPNL/IN2P3

*/


#ifndef DBTKDCUPSUMAPACCESS_H
#define DBTKDCUPSUMAPACCESS_H
#include "DbAccess.h"

// ostream use for int to string conversion 
#include <sstream>
#include "stringConv.h"

//declaration of the exception handler for the DCU
#include "FecExceptionHandler.h"

/** \brief This class is implemented to handle the communication between the DCU supervisor software and the database.
 *
 */
class DbTkDcuPsuMapAccess : public DbCommonAccess {

 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  DbTkDcuPsuMapAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Constructor with connection parameters
   */
  DbTkDcuPsuMapAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Destructor
   */
  ~DbTkDcuPsuMapAccess ( ) throw (oracle::occi::SQLException);

  /** \brief get the DCU/PSU map version for the given partition
   */
  std::list<unsigned int*> getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException) ;

  /**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
   */
  oracle::occi::Clob *getXMLClob( int onlyPsuName, std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 ) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** Request the PSU names from the database
   */
  oracle::occi::Clob *getPsuNameXMLClob( std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 ) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** Request the DCU / PSU from the database
   */
  oracle::occi::Clob *getDcuPsuMapXMLClob( std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 ) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** Request the DCU / PSU from the database
   */
  oracle::occi::Clob *getPsuNotConnectedXMLClob( std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 ) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
   */
  void setXMLClob(bool psuNameOnly, std::string buffer, std::string partitionName, unsigned int *versionMajorId = NULL, unsigned int *versionMinorId = NULL ) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** Upload the PSU names in a new version
   */
  void uploadPsuNames ( std::string buffer, std::string partitionName, unsigned int *versionMajorId = NULL, unsigned int *versionMinorId = NULL ) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** Upload the PSU/DCU map in a new version
   */
  void uploadDcuPsuMap ( std::string buffer, std::string partitionName, unsigned int *versionMajorId = NULL, unsigned int *versionMinorId = NULL) throw (oracle::occi::SQLException, FecExceptionHandler) ;

};

#endif
