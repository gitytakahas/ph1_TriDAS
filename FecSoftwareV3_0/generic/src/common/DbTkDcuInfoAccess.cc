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

#include "DbTkDcuInfoAccess.h"

/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess()
 */
DbTkDcuInfoAccess::DbTkDcuInfoAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbCommonAccess (threaded) {
}

/**Create an access to the database
 * @param user - user name
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess(std::string user, std::string passwd, std::string dbPath)
 */
DbTkDcuInfoAccess::DbTkDcuInfoAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbCommonAccess (user, passwd, dbPath, threaded) {
}

/**Close the access to the database
 * @exception oracle::occi::SQLException
 * @see DbAccess::~DbAccess()
 */
DbTkDcuInfoAccess::~DbTkDcuInfoAccess ()  throw (oracle::occi::SQLException) {
}

/**Get the current version for a given partition name
 * @param partitionName - Partition Name
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId) : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException
 * @see DbAccess::getDatabaseVersion (std::string sqlQuery)
 * @todo use a callable statement to execute a PL/SQL function with bind variable to avoid the query parsing at every call 
 */
std::list<unsigned int*> DbTkDcuInfoAccess::getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException){

  static std::string sqlQuery = "SELECT DISTINCT StateHistory.partitionId, StateHistory.dcuInfoVersionMajorId, StateHistory.dcuInfoVersionMinorId, 0, 0 FROM CurrentState, StateHistory, Partition  WHERE StateHistory.stateHistoryId = CurrentState.stateHistoryId AND StateHistory.partitionId=Partition.partitionId AND Partition.partitionName = :partitionName" ;

  std::list<unsigned int*> listVersion = DbCommonAccess::getDatabaseVersionFromQuery(sqlQuery, partitionName);
  return listVersion ;
}

/**Get the last det id version
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId) : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException
 * @todo use a callable statement to execute a PL/SQL function with bind variable to avoid the query parsing at every call 
 */
std::list<unsigned int*> DbTkDcuInfoAccess::getDatabaseVersion ( ) throw (oracle::occi::SQLException, FecExceptionHandler){

  static std::string writeString = "BEGIN PkgDcuInfo.getLastVersion(:versionMajor, :versionMinor);END;";
  oracle::occi::Statement *stmt = NULL ;
  std::list<unsigned int*> partitionVersionsListe ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(writeString);
    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(unsigned int)) ;
    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(unsigned int)) ;

    i = stmt->executeUpdate() ;
#ifdef DATABASEDEBUG
    std::cout << __PRETTY_FUNCTION__ << "=> stmt->executeUpdate() = " << i << std::endl ;
#endif

    if (i >= 1) {
      unsigned int *partitionVersions = new unsigned int[3] ;
      partitionVersions[0] = 0 ;
      partitionVersions[1] = (unsigned int)stmt->getInt (1);
      partitionVersions[2] = (unsigned int)stmt->getInt (2);
      partitionVersionsListe.push_back(partitionVersions);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      RAISEFECEXCEPTIONHANDLER (DB_NOVERSIONAVAILABLE, "Unable to retreive the DET ID version: " + writeString, ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return partitionVersionsListe;
}


/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string supervisorIp, unsigned int xdaqInstance, std::string xdaqClassName)</I>.<BR>
 * @param dcuHardId - Dcu Hardware Identifier
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string supervisorIp, unsigned int xdaqInstance, std::string xdaqClassName)
 * @see DbAccess::getXMLClob(std::string readString, std::string supervisorIp, unsigned int xdaqInstance, std::string xdaqClassName)
 * @see PkgFecXML.getAllFecFromPartitionAndIp ( pcIpNumber IN VARCHAR2, xdaqInstanceNb IN NUMBER, xdaqClassName IN VARCHAR2 ) RETURN CLOB;
 */
oracle::occi::Clob *DbTkDcuInfoAccess::getXMLClob( std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId ) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgDcuInfoXML.getDcuInfoFromPartition(:partitionName,:versionMajor,:versionMinor);END;";
  oracle::occi::Statement *stmt = NULL ;
  
  try { 
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
    std::cerr << "partitionName " << partitionName << " for version " << majorVersionId << "." << minorVersionId << std::endl;
#endif
    
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    stmt->setInt(++i,majorVersionId);
    stmt->setInt(++i,minorVersionId);
    
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": " << msg.str() << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}



/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about all the dcu in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getXMLClob() throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgDcuInfoXML.getAllDcuInfo();END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif
    
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database" ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": " << msg.str() << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}


/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about all the dcu in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getXMLClob(unsigned int majorVersionId, unsigned int minorVersionId) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgDcuInfoXML.getAllDcuInfo(:majorId, :minorId);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setInt(++i,majorVersionId);
    stmt->setInt(++i,minorVersionId);
    
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for version " << majorVersionId << "." << minorVersionId ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": " << msg.str() << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the current states in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getCurrentStatesXMLClob( ) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgStateHistoryXML.getStates;END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database" ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": " << msg.str() << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the current Fec versions in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getFecVersionsXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgVersionXML.getAllFecVersions(:partitionName);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": " << msg.str() << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the current Fed versions in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getFedVersionsXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgVersionXML.getAllFedVersions(:partitionName);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " " << msg.str() << std::endl ;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the current dcu Infos versions in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getDcuInfoVersionsXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgVersionXML.getAllDcuInfoVersions(:partitionName);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif
    
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " " << msg.str() << std::endl ;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
  
  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the current connections versions in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getConnectionVersionsXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgVersionXML.getAllConnectionVersions(:partitionName);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif
    
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " " << msg.str() << std::endl ;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the current DcuPsuMap versions in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getDcuPsuMapVersionsXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgVersionXML.getAllDcuPsuMapVersions(:partitionName);END;";
  oracle::occi::Statement *stmt = NULL ;
  
  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " " << msg.str() << std::endl ;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the current DcuPsuMap versions in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getMaskVersionsXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgVersionXML.getAllMaskVersions(:partitionName);END;";
  oracle::occi::Statement *stmt = NULL ;
  
  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " " << msg.str() << std::endl ;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the last run of this partition in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getLastRunXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgRunXML.getRun(:partitionName);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " " << msg.str() << std::endl ;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about all the runs in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getAllRunsXMLClob( ) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgRunXML.getAllRuns();END;";
  oracle::occi::Statement *stmt = NULL ;

  try {

#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database" ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << " " << msg.str() << std::endl ;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the given run of this partition in the database.
 * @exception oracle::occi::SQLException
 */

oracle::occi::Clob *DbTkDcuInfoAccess::getRunXMLClob(std::string partitionName, int runNumber ) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgRunXML.getRunWithNumber(:partitionName, :runNumber);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    stmt->setInt(++i,runNumber);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName << " and run " << runNumber ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": " << msg.str() << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @return a pointer on a Clob containing the data about the last run used for O2O for this partition in the database.
 * @exception oracle::occi::SQLException
 */
oracle::occi::Clob *DbTkDcuInfoAccess::getLastO2ORunXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgRunXML.getLastO2ORun(:partitionName);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
#ifdef DATABASEDEBUG
    std::cerr << "readString " << readString << std::endl;
#endif

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << readString << "): " << "unable to get the XML buffer from the database for partition " << partitionName ;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": " << msg.str() << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}


/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer)</I>.<BR>
 * @param buffer - pointer on a buffer used for upload
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see PkgFecXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 */
void DbTkDcuInfoAccess::setXMLClob(std::string buffer) throw (oracle::occi::SQLException, FecExceptionHandler) 
{
  static std::string writeString = "BEGIN PkgDcuInfoXML.uploadClob(:buffer); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    size_t bufferSize = 0;
    if (buffer.size()) {
      bufferSize = buffer.size();
      unsigned int i = 0;  
      if ((*xmlClob_).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << __PRETTY_FUNCTION__ << " xmlClob_ attribute is not initialized" << std::endl;
#endif
	*xmlClob_ = initializeClob();
      }

#ifdef DATABASEDEBUG
      std::cout << "writeString : " << writeString << std::endl;
      std::cout << "buffer : " << buffer << std::endl;
      std::cout << "bufferSize : " << bufferSize << std::endl;
#endif
    
      (*xmlClob_).trim(0);
      (*xmlClob_).write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
      stmt->setClob (++i, (*xmlClob_));
      
      stmt->execute();
      
      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt);
    } else {
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": buffer is empty" << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Unable to perform an upload, buffer is empty", ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}


/**Update all the channel delays according to the fibre length from the detector to the FED.<BR>
 * \param partitionName - partition name to be used
 * @exception SQLException
 * @exception FecExceptionHandler
 */
void DbTkDcuInfoAccess::updateChannelDelays (std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler){
  static std::string writeString = "BEGIN PkgDcuInfo.updateChannelDelays(:paramPartitionName); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setString(1,partitionName);
    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}


/** Upload XML clob to database for diagnotic system
 * \param buffer - CLOB to be uploaded
 */
void DbTkDcuInfoAccess::uploadDiagnoticXMLClob ( std::string buffer ) throw (oracle::occi::SQLException, FecExceptionHandler) {

  static std::string writeString = "BEGIN PkgDiagnosticXML.uploadDiagnosticXMLClob(:buffer); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    size_t bufferSize = 0;
    if (buffer.size()) {
      bufferSize = buffer.size();
      unsigned int i = 0;  
      if ((*xmlClob_).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << __PRETTY_FUNCTION__ << " xmlClob_ attribute is not initialized" << std::endl;
#endif
	*xmlClob_ = initializeClob();
      }

#ifdef DATABASEDEBUG
      std::cout << "writeString : " << writeString << std::endl;
      std::cout << "buffer : " << buffer << std::endl;
      std::cout << "bufferSize : " << bufferSize << std::endl;
#endif
    
      (*xmlClob_).trim(0);
      (*xmlClob_).write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
      stmt->setClob (++i, (*xmlClob_));
      
      stmt->execute();
      
      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt);
    } else {
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": buffer is empty" << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Unable to perform an upload, buffer is empty", ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}
