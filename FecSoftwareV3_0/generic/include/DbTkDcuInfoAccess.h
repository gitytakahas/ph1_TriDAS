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


#ifndef DBTKDCUINFOACCESS_H
#define DBTKDCUINFOACCESS_H
#include "DbAccess.h"

// ostream use for int to string conversion 
#include <sstream>
#include "stringConv.h"

//declaration of the exception handler for the DCU
#include "FecExceptionHandler.h"

/** \brief This class is implemented to handle the communication between the DCU supervisor software and the database.
 *
 */
class DbTkDcuInfoAccess : public DbCommonAccess {

 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  DbTkDcuInfoAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Constructor with connection parameters
   */
  DbTkDcuInfoAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Destructor
   */
  ~DbTkDcuInfoAccess ( ) throw (oracle::occi::SQLException);

  /** \brief get the DCU info version for the given partition
   */
  std::list<unsigned int*> getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException) ;

  /** \brief get the DCU info version
   */
  std::list<unsigned int*> getDatabaseVersion () throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** \brief Download a Clob from database
   */
  oracle::occi::Clob *getXMLClob(std::string partitionName, unsigned int majorVersionId = 0, unsigned int minorVersionId = 0 ) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the dcu infos for the given version from database
   */
  oracle::occi::Clob *getXMLClob(unsigned int majorVersionId, unsigned int minorVersionId) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the dcu infos for the last version from database
   */
  oracle::occi::Clob *getXMLClob() throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the current states informations
   */
  oracle::occi::Clob *getCurrentStatesXMLClob() throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the current Fec versions informations
   */
  oracle::occi::Clob *getFecVersionsXMLClob(std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the current Fed versions informations
   */
  oracle::occi::Clob *getFedVersionsXMLClob(std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the current Dcu Info versions informations
   */
  oracle::occi::Clob *getDcuInfoVersionsXMLClob(std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the Connection versions informations
   */
  oracle::occi::Clob *getConnectionVersionsXMLClob(std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the current Dcu PSU Map versions informations
   */
  oracle::occi::Clob *getDcuPsuMapVersionsXMLClob(std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with all the current Dcu PSU Map versions informations
   */
  oracle::occi::Clob *getMaskVersionsXMLClob(std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with the last run for the given partition
   */
  oracle::occi::Clob *getLastRunXMLClob(std::string partitionName ) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with the last run for the given partition
   */
  oracle::occi::Clob *getAllRunsXMLClob( ) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with the given run for the given partition
  */
  oracle::occi::Clob *getRunXMLClob(std::string partitionName, int runNumber ) throw (oracle::occi::SQLException);

  /** \brief Download a Clob with the last run used by O2Ofor the given partition
   */
  oracle::occi::Clob *getLastO2ORunXMLClob(std::string partitionName ) throw (oracle::occi::SQLException);

  /** \brief Upload a Clob to the database for configuration
   */
  void setXMLClob (std::string buffer) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Update the channel delays according to the fiber length
   */
  void updateChannelDelays (std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Upload XML clob to database for diag system
   */
  void uploadDiagnoticXMLClob ( std::string buffer ) throw (oracle::occi::SQLException, FecExceptionHandler);

};

#endif
