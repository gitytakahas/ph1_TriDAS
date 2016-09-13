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

#ifndef DBCOMMONACCESS_H
#define DBCOMMONACCESS_H

#include <cstring>
#include <occi.h>

#include "FecExceptionHandler.h"

// Name of the environment variables for connection to the database as format loing/passwd@path
#define CONFDB "CONFDB"

#define NOERRORUSERMESSAGE "unable to retrieve the last user error message"

/** \brief This virtual class is implemented to handle the communication between the supervisor software and the database.
 *
 * This class provides some features like :
 *  - OCCI environnement and database connection opening and closing 
 *  - download and upload execution
 *  - database version request
 *
 * Private attributes :
 *  - dbEnvironment_ : pointer on an OCCI environnement descriptor
 *  - dbConnection_ : pointer on a database connection descriptor
 *  - xmlClob_ : pointer on a Clob
 * Some OCCI documentation:
 *  - oracle::occi::Statement has status that you can find in http://download-west.oracle.com/docs/cd/B10501_01/appdev.920/a96583/cci08r20.htm#1072768
 */
class DbCommonAccess {
 private:
  // 
  // private attributes
  //
  /** user login access
   */
  std::string user_ ;

  /** password access
   */
  std::string passwd_ ;

  /** DB path access
   */
  std::string path_ ;

  /**
   * Environment descriptor
   */
  oracle::occi::Environment *dbEnvironment_;

  /** Static variable of oracle::occi::Connection in case of shared connection
   */
  static oracle::occi::Connection *dbConnectionStatic_;

  /** Is local env. is used
   */
  bool useLocalConnection_ ;

  /** Number of connections on the shared connections
   */
  static unsigned int connectionInUsed_ ;

  /** \brief Creates the access to the database
   */
  virtual void dbConnect (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);

 protected:

  /** Static variable of oracle::occi::Environment in case of shared connection
   */
  static oracle::occi::Environment *dbEnvironmentStatic_ ;

  /**
   * Connection descriptor
   */
  oracle::occi::Connection *dbConnection_;

  /**
   * Pointer on a Clob used for download and upload
   */
  oracle::occi::Clob* xmlClobArray_;
  oracle::occi::Clob* xmlClob_;

 // BELS. TKCC 2008
  void prepareXmlClob();
  void prepareXmlClobArray( int index );


 public:

  //
  // public functions
  //
  /** \brief Default constructor
   */
  DbCommonAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Creates the access to the database
   */
  DbCommonAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);
  
  /** \brief Closes the connection with the database
   */
  virtual ~DbCommonAccess ( ) throw (oracle::occi::SQLException);

  /** \brief Initialize a Clob 
   */
  virtual oracle::occi::Clob initializeClob () throw (oracle::occi::SQLException);
 
 /** \brief Retreive the login name of the connexion
   */
  virtual std::string getDbLogin() {
    return user_ ;
  }
  
  /** \brief Retreive the password name of the connexion
   */
  virtual std::string getDbPassword ( ) {
    return passwd_ ;
  }
  
  /** \brief Retreive the database path of the connexion
   */
  virtual std::string getDbPath ( ) {
    return path_ ;
  }

  /** \brief rollback
   */
  void rollback(){
    dbConnection_->rollback();
  }

  /** \brief Commit
   */
  void commit(){
    dbConnection_->commit();
  }

  /** \brief Get the current run number
   */
  virtual unsigned int getCurrentRunNumber() throw (FecExceptionHandler);
  
  /** \brief Retreive the version for each partition in the current state
   */
  std::list<unsigned int*> getDatabaseVersionFromQuery (std::string sqlQuery, std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Retreive all partition names
   */
  std::list<std::string> getAllPartitionNames ()  throw (oracle::occi::SQLException);

  /** \brief Retreive all partition names for a given runnumber
   */
  std::list<std::string> getAllPartitionNames ( unsigned int ruNumber )  throw (oracle::occi::SQLException);

  /** \brief Retrieve all partition names from current state
   */
  std::list<std::string> getAllPartitionNamesFromCurrentState ( )  throw (oracle::occi::SQLException) ;

  /** \brief Retreive all state history names
   */
  std::list<std::string> getAllStateHistoryNames ()  throw (oracle::occi::SQLException);

  /** \brief Retreive the state history ID for the current state from the database
   */
  unsigned int getCurrentStateHistoryId() throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief get a new history state name
   */
  unsigned int getNewStateHistoryId() throw (oracle::occi::SQLException);

  /** \brief Get the state ID from the state name
   *  \param stateName The state name
   *  \return The ID of the given state   
   */
  unsigned int getStateHistoryId(std::string stateHistoryName) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Retrive an old state
   */
  unsigned int setCurrentState(unsigned int stateId) throw (oracle::occi::SQLException);

  /** \brief update the current state based on the state coming from a run number
   */
  unsigned int copyStateForRunNumber(unsigned int runNumber, bool allPartition) throw (oracle::occi::SQLException) ;

  /** \brief Copy the current state and update the given partition with its value in the given state
   */
  unsigned int setCurrentState(std::string partitionName, unsigned int stateHistoryId) throw (oracle::occi::SQLException);

  /** \brief Change the state of the devices corresponding to the dcuHardId in the given partition 
   **/
  void setDeviceState(std::string partitionName, std::vector<unsigned int> dcuHardId, int state) throw (oracle::occi::SQLException);

  /** \brief Register a run 
   */
  void setRun(std::string partitionName, unsigned int runNumber, int runMode, int local, std::string comment) throw (oracle::occi::SQLException,FecExceptionHandler);

  /** \brief Stop a run 
   */
  void stopRun(std::string partitionName, std::string comment) throw (oracle::occi::SQLException,FecExceptionHandler);

  /** \brief Toggle the flag for o2o operation performed
   */
  void setO2ORun(std::string partitionName, unsigned int runNumber) throw (oracle::occi::SQLException,FecExceptionHandler);

  /** \brief Retreive the database version
   */
  double getDbVersion( ) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** \brief Retreive the size of data in database
   */
  double getDbSize( ) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** \brief Change the comment of the Run
   */
  void setNewComment(std::string partitionName, unsigned int runNumber, std::string newComment) throw (oracle::occi::SQLException,FecExceptionHandler);

  /** \brief Create a new Version of dcu psu map
   */ 
  void createNewDcuPsuMapVersion(unsigned int *majorVersion, unsigned int *minorVersion) throw (oracle::occi::SQLException,FecExceptionHandler) ;

  /** \brief Create a new Version of dcu infos 
   */
  void createNewDcuInfoVersion() throw (oracle::occi::SQLException,FecExceptionHandler);

  /** \brief retreive the database version
   */
  void createStateHistory(unsigned int stateId, std::string stateName, std::string partitionName, 
			  unsigned int fecVersionMajorId,  unsigned int fecVersionMinorId, 
			  unsigned int fedVersionMajorId, unsigned int fedVersionMinorId, 
			  unsigned int connectionVersionMajorId, unsigned int connectionVersionMinorId,
			  unsigned int dcuInfoVersionMajorId, unsigned int dcuInfoVersionMinorId, 
			  unsigned int dcuPsuMapVersionMajorId, unsigned int dcuPsuMapVersionMinorId,
			  unsigned int maskVersionMajorId, unsigned int maskVersionMinorId
			  ) throw (oracle::occi::SQLException);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString) throw (oracle::occi::SQLException);

  /** \brief Refresh the cache of the connection, fec and fed
   */
  virtual void refreshCacheXMLClob ( int deleteValues = 1, bool withStrips = true ) throw (oracle::occi::SQLException) ;

  /** \brief Delete the cash
   */
  virtual void deleteCacheXMLClob ( ) throw (oracle::occi::SQLException) ;
  
  /** \brief Upload a Clob to the database
   */
  //  virtual void setXMLClob (std::string buffer) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Get the last error message
   */
  virtual std::string getErrorMessage();

  /** \brief validate the DCU/PSU map and retreive the cooling loop with a flag
   */
  virtual std::vector<std::pair<std::string, bool> > setTKCCDcuPsuMapValidation(std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** Brief retreive login, password and path from env. variable CONFDB
   */
  static void getDbConfiguration(std::string &login, std::string &passwd, std::string &path) {
    char* cpath=getenv(CONFDB);
    if (cpath == NULL) return ;
    
    std::string confdb(cpath);
    std::string::size_type ipass = confdb.find("/");
    std::string::size_type ipath = confdb.find("@");
    if ((ipass != std::string::npos) && (ipath != std::string::npos)) {
      login.clear();
      login=confdb.substr(0,ipass); 
      passwd.clear();
      passwd=confdb.substr(ipass+1,ipath-ipass-1); 
      path.clear();
      path=confdb.substr(ipath+1,confdb.size());
    }
  }

  /** \brief send a XML buffer to the database to be parsed
   */
  int setGenericXMLClob(std::string buffer, std::string tableName) throw (oracle::occi::SQLException, FecExceptionHandler) ;

  /** \brief return the error message coming from oracle exception and get it if it exists the message set by the user exception in PL/SQL
   */
  std::string what ( std::string message, oracle::occi::SQLException &ex ) ;

  /** \brief insert an o2o operation
   */
  void setO2OOperation ( std::string partitionName, std::string subDetector, unsigned int o2oTimeStamp ) throw (FecExceptionHandler) ;

  /** \brief crosscheck the last o2o operation
   */
  int getO2OXchecked ( std::string partitionName ) throw (FecExceptionHandler) ;

};
#endif

