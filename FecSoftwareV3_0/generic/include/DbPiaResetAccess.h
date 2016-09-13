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


#ifndef DBPIARESETACCESS_H
#define DBPIARESETACCESS_H
#include "DbAccess.h"

#include "stringConv.h"

#include "piaResetDescription.h"

/** \brief This class is implemented to handle the communication between the FEC supervisor software and the database.
 *
 * This class provides some features like :
 *  - OCCI environnement and database connection opening and closing 
 *  - download and upload execution
 *  - database version request
 *
 */
class DbPiaResetAccess : public DbAccess {
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
  DbPiaResetAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Constructor with connection parameters
   */
  DbPiaResetAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Destructor
   */
  ~DbPiaResetAccess ( ) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  oracle::occi::Clob *getXMLClob (std::string partitionName, std::string fecHardId) throw (oracle::occi::SQLException);

  /** \brief Upload a Clob from the database
   */
  void setXMLClob (std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Deprecated method not used and not need, an exception is raised on a call on this method
   * \exception FecExceptionHandler
   */
  virtual std::list<unsigned int*> getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException) {
    RAISEFECEXCEPTIONHANDLER( CODECONSISTENCYERROR, 
			       "deprecated, no version is available for PIA reset",
			       FATALERRORCODE ) ;
  }

  /** \brief Deprecated method not used and not need, an exception is raised on a call on this method
   * \exception FecExceptionHandler
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskMajorVersion, unsigned int maskVersionMinor) throw (oracle::occi::SQLException) {
    RAISEFECEXCEPTIONHANDLER( CODECONSISTENCYERROR, 
			       "deprecated, no version is available for PIA reset",
			       FATALERRORCODE ) ;
    return NULL ;
  }

  /** \brief Deprecated method not used and not need, an exception is raised on a call on this method
   * \exception FecExceptionHandler
   */
  oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, std::string hardId, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) {
    RAISEFECEXCEPTIONHANDLER( CODECONSISTENCYERROR, 
			       "deprecated, no version is available for PIA reset",
			       FATALERRORCODE ) ;
    return NULL ;
  }
};

#endif
