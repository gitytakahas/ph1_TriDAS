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


#ifndef DBTKIDVSHOSTNAMEACCESS_H
#define DBTKIDVSHOSTNAMEACCESS_H
#include "DbAccess.h"

// ostream use for int to string conversion 
#include <sstream>
#include "stringConv.h"

//declaration of the exception handler for the DCU
#include "FecExceptionHandler.h"

/** \brief This class is implemented to handle the communication between the DCU supervisor software and the database.
 *
 */
class DbTkIdVsHostnameAccess : public DbCommonAccess {

 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  DbTkIdVsHostnameAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Constructor with connection parameters
   */
  DbTkIdVsHostnameAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Destructor
   */
  ~DbTkIdVsHostnameAccess ( ) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the Id VS hostname from database
   */
  oracle::occi::Clob *getXMLClob( unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 ) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the Id VS hostname from database
   */
  oracle::occi::Clob *getXMLClobFromHostname ( std::string hostname, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 ) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the Id VS hostname from database
   */
  oracle::occi::Clob *getXMLClobFromHostnameSlot ( std::string hostname, unsigned int slot, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 ) throw (oracle::occi::SQLException);

  /** \brief Retreive the sub-detector for a given version
   */
  void getSubDetectorCrateNumber ( std::string hostname, std::string &subDetector, unsigned int &crateNumber, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 ) throw (oracle::occi::SQLException) ;

  /** \brief retreive the version for the ID versus hostname in the "current state"
   */
  void getTkIdVsHostnameVersion ( unsigned int &versionMajorId, unsigned int &versionMinorId ) throw (oracle::occi::SQLException) ;

  /** \brief Upload a Clob to the database for configuration
   */
  void setXMLClob (std::string buffer, bool major) throw (oracle::occi::SQLException, FecExceptionHandler);

};

#endif
