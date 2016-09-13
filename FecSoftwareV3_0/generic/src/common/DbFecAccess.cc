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

#include "DbFecAccess.h"

/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess()
 */
DbFecAccess::DbFecAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbAccess (threaded) {
#ifdef DATABASESTAT
  this->traceSql(true);
#endif
}

/**Create an access to the database
 * @param user - user name
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess(std::string user, std::string passwd, std::string dbPath)
 */
DbFecAccess::DbFecAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbAccess (user, passwd, dbPath, threaded) {
}

/**Close the access to the database
 * @exception oracle::occi::SQLException
 * @see DbAccess::~DbAccess()
 */
DbFecAccess::~DbFecAccess ()  throw (oracle::occi::SQLException) {
#ifdef DATABASESTAT
  this->traceSql(false);
#endif
}

/**Get the current version for a given partition name
 * @param partitionName - Partition Name
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId) : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException
 * @see DbAccess::getDatabaseVersion (std::string sqlQuery)
 * @todo use a callable statement to execute a PL/SQL function with bind variable to avoid the query parsing at every call 
 */
std::list<unsigned int*> DbFecAccess::getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException){

  static std::string sqlQuery = "SELECT DISTINCT StateHistory.partitionId, StateHistory.fecVersionMajorId, StateHistory.fecVersionMinorId, StateHistory.maskVersionMajorId, StateHistory.maskVersionMinorId FROM CurrentState, StateHistory, Partition  WHERE StateHistory.stateHistoryId = CurrentState.stateHistoryId AND StateHistory.partitionId=Partition.partitionId AND Partition.partitionName = :partitionName" ;

  std::list<unsigned int*> listVersion = DbCommonAccess::getDatabaseVersionFromQuery(sqlQuery, partitionName);
  return listVersion ;
}

/**Create a new current state with partitions/versions. <BR>
 * Select the current state and check if it is registered (check its registered parameter value). <BR>
 * If this state is registered : a new state is create. <BR>
 * The state is updated with the data found in the parameter <I>partitionVersionsList</I>. <BR>
 * The current state is update to this new state. <BR>
 * @param partitionVersionsList : pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId)  : THE CONTENT OF THIS LIST OF POINTERS IS REMOVED BY THE FUNCTION !!
 * @exception FecExceptionHandler : an exception is raised in case of trouble in statement creation or execution
 * @see PkgStateHistory.createNewCurrentState RETURN NUMBER;
 * @see PkgStateHistory.setValues(paramStateId IN NUMBER, paramPartitionId IN NUMBER, fecVersionMajor IN NUMBER, fecVersionMinor IN NUMBER);	
 */
void DbFecAccess::setDatabaseVersion(std::list<unsigned int*> partitionVersionsList) throw (FecExceptionHandler){

  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ;
  static std::string sqlQuery = "SELECT DISTINCT StateHistoryId FROM CurrentState" ;
  static std::string queryString = "BEGIN :a := PkgStateHistory.createNewCurrentState();END;";  
  
  try {
    unsigned int currentStateId = 0;
    unsigned int *partitionVersions;
    
    stmt = dbConnection_->createStatement (sqlQuery);
    rset = stmt->executeQuery ();
    while (rset->next ()) {
      currentStateId = rset->getInt(1);
    }
    dbConnection_->terminateStatement(stmt);          
    
    // The state cannot be changed so a new one is created
    // as we could not modify this state, we need to
    // create a new state and set this state as current state
    stmt = dbConnection_->createStatement(queryString);
    
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(currentStateId));
      
    if (stmt->executeUpdate() == 1) {
      currentStateId = stmt->getInt(1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to create a new state history" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
    
    //sqlQuery = "BEGIN PkgStateHistory.setValues(:stateId, :partitionId, :fecVersionMajor, :fecVersionMinor);END;";
    //stmt = dbConnection_->createStatement (sqlQuery);
    //stmt->setUInt(1, currentStateId);    
    
    static std::string sqlQuery1 = "UPDATE StateHistory SET StateHistory.fecVersionMajorId = :a , StateHistory.fecVersionMinorId = :b WHERE StateHistory.partitionId = :c AND StateHistory.stateHistoryId = :d";

    stmt = dbConnection_->createStatement (sqlQuery1);
    stmt->setUInt(4, currentStateId);
    
    while (partitionVersionsList.size() > 0){
      partitionVersions = (unsigned int *)partitionVersionsList.back();
      
      stmt->setUInt(3, partitionVersions[0]);
      stmt->setUInt(1, partitionVersions[1]);
      stmt->setUInt(2, partitionVersions[2]);
      
      stmt->executeUpdate();
      
      //delete[] partitionVersions;
      //partitionVersionsList.pop_back();
    }
    dbConnection_->terminateStatement(stmt);      

    // Update the current state (not anymore needed, set by trigger)
    //sqlQuery = "UPDATE CurrentState SET stateId = :a";
    //stmt = dbConnection_->createStatement (sqlQuery);
    //stmt->setUInt(1, currentStateId);
    //stmt->executeUpdate();
    //dbConnection_->terminateStatement(stmt);
  }
  catch (oracle::occi::SQLException &ex){
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt); 
    std::stringstream msg ;
    msg << DB_PLSQLEXCEPTIONRAISED_MSG << " (" << queryString << "): " << "unable to create a new state history" ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(msg.str(), ex), ERRORCODE) ;
  }
}

/**Define the query to get the next major version and execute the request
 * @return the next available major version 
 * @exception SQLException
 * @see DbAccess::getVersion(std::string sqlQuery)
 */
unsigned int DbFecAccess::getNextMajorVersion() throw (oracle::occi::SQLException)
{
  static std::string sqlQuery = "SELECT MAX(versionMajorId)+1 FROM FecVersion";
  return DbAccess::getVersion(sqlQuery);
}

/**Define the query to get the next minor version with major version = majorId and execute the request
 * @return the greatest minor version with major version = majorId
 * @param majorId : major version number
 * @exception SQLException
 * @see DbAccess::getVersion(std::string sqlQuery)
 */
unsigned int DbFecAccess::getNextMinorVersion(unsigned int majorId) throw (oracle::occi::SQLException)
{
  static std::string sqlQuery = "SELECT MAX(versionMinorId)+1 FROM FecVersion where versionMajorId =" + toString(majorId);
  return DbAccess::getVersion(sqlQuery);
}

/**Create a configuration state from partition
 * @param partitionName - partition name
 * @param partitionId - new created partition identifier (output parameter)
 * @param fecVersionMajorId - returned new version major number for the Fec
 * @return new stateId
 * @exception FecExceptionHandler : a FecExceptionHandler is raised in case of trouble in creating or executing the statement.
 * @see PkgStateHistory.createConfigurationState(paramPartitionName IN VARCHAR2, paramPartitionNb OUT NUMBER, paramFecVersionMajorNumber OUT NUMBER) RETURN NUMBER; 
 */
unsigned int DbFecAccess::createNewStateHistory(std::string partitionName, unsigned int *partitionId, unsigned int *fecVersionMajorId)  throw (FecExceptionHandler) {

  unsigned int stateHistoryId = 0;
  unsigned int i = 0;
  static std::string queryString = "BEGIN :a := PkgStateHistory.createConfigurationState(:b, :c, :d);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(queryString);

    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(stateHistoryId));
    stmt->setString(++i, partitionName);
    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(partitionId));
    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(fecVersionMajorId));
    if (stmt->executeUpdate() == 1) {
      stateHistoryId = stmt->getInt(1);
      *partitionId = stmt->getInt(5);
      *fecVersionMajorId = stmt->getInt(7);
      dbConnection_->terminateStatement(stmt);
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to create a new state history for partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &ex){
    dbConnection_->terminateStatement(stmt);
    std::stringstream msg ;
    msg << DB_PLSQLEXCEPTIONRAISED_MSG << " (" << queryString << "): " << "unable to create a new state history for partition " << partitionName ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(msg.str(), ex), ERRORCODE) ;
  }

  return stateHistoryId;
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName)</I>.<BR>
 * @param partitionName - name of the partition to be accessed
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string partitionName)
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName)
 * @see PkgFecXML.getAllFecFromPartition ( partitionName IN VARCHAR2 ) RETURN CLOB;
 */
oracle::occi::Clob *DbFecAccess::getXMLClob(std::string partitionName ) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgFecXML.getAllFecFromPartition(:partitionName);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)</I>.<BR>
 * @param partitionName - partition name
 * @param versionMajor - major version number
 * @param versionMinor - minor version number 
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)
 * @see PkgFecXML.getAllFecFromPartition ( partitionName IN VARCHAR2, versionMajorId IN NUMBER, versionMinorId IN NUMBER ) RETURN CLOB;
 */
oracle::occi::Clob *DbFecAccess::getXMLClobWithVersion(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgFecXML.getAllFecFromPartition(:partitionName, :versionMajorId, :versionMinorId, :maskMajor, :maskMinor);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName, versionMajor, versionMinor, maskVersionMajor, maskVersionMinor);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, unsigned int id)</I>.<BR>
 * @param partitionName - partition name
 * @param fecId - fec identifier
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(unsigned int id, boolean hardId)
 * @see DbAccess::getXMLClob(std::string readString, unsigned int id)
 * @see PkgFecXML.getAllFecFromId ( fecHardId IN NUMBER ) RETURN CLOB;
 */
oracle::occi::Clob *DbFecAccess::getXMLClob(std::string partitionName, std::string fecId) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgFecXML.getAllFecFromPartitionAndId(:partitionName, :fecId);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName, fecId);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, unsigned int id, unsigned int versionMajorId, unsigned int versionMinorId)</I>.<BR>
 * @param partitionName - partition name
 * @param fecId - fec identifier
 * @param versionMajorId - major version number
 * @param versionMinorId - minor version number 
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(unsigned int id, boolean hardId, unsigned int versionMajorId, unsigned int versionMinorId)
 * @see DbAccess::getXMLClob(std::string readString, unsigned int id, unsigned int versionMajorId, unsigned int versionMinorId)
 * @see PkgFecXML.getAllFecFromId ( fecHardId IN NUMBER, versionMajorId IN NUMBER, versionMinorId IN NUMBER ) RETURN CLOB;
 */
oracle::occi::Clob *DbFecAccess::getXMLClobWithVersion(std::string partitionName, std::string fecId, unsigned int versionMajorId, unsigned int versionMinorId ) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgFecXML.getAllFecFromPartitionAndId(:partitionName, :fecId, :versionMajorId, :versionMinorId);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName, fecId, versionMajorId, versionMinorId);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, std::string partitionName</I>.<BR>
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param newPartition - boolean : true if you need to create a new partition
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string writeString, std::string buffer, std::string partitionName) 
 * @see PkgFecXML.configureXMLClob(xmlClob IN CLOB, partitionName IN VARCHAR2) RETURN NUMBER;
 * @see PkgFecXML.addXMLClob ( xmlClob IN CLOB, partitionName IN VARCHAR2) RETURN NUMBER;
 */
unsigned int DbFecAccess::setXMLClob(std::string* buffer, std::string partitionName, boolean newPartition) throw (oracle::occi::SQLException, FecExceptionHandler) 
{
  static std::string writeString("BEGIN :versionMajorId := PkgFecXML.configureXMLClob(:bufferPll, :bufferLaserdriver, :bufferApvFec, :bufferApvMux, :bufferDcu, :partitionName, :createNewPartition);END;");
  unsigned int versionMajorId = 0;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  
    unsigned int j = 0;  
    
#ifdef DATABASEDEBUG
    std::cerr << "DbFecAccess::setXMLClob writeString : " << writeString << std::endl;
    std::cerr << "DbFecAccess::setXMLClob partitionName : " << partitionName << std::endl;
    std::cerr << "DbFecAccess::setXMLClob newPartition : "<< newPartition << std::endl;
#endif

    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMajorId));
    for (j=0; j<5; j++) {
      //    if (buffer[j].size()) {
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "DbFecAccess::setXMLClob bufferSize["<<j<<"] =" << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "DbFecAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }
      
    
#ifdef DATABASEDEBUG
      std::cerr << "DbFecAccess::setXMLClob buffer["<<j<<"] : " << buffer[j] << std::endl;
      std::cerr << "DbFecAccess::setXMLClob bufferSize["<<j<<"] = " << bufferSize << std::endl;
#endif
    
      (xmlClobArray_[j]).trim(0);
      (xmlClobArray_[j]).write(bufferSize, (unsigned char*)buffer[j].c_str(), bufferSize);
      stmt->setClob (++i, (xmlClobArray_[j]));
    }
    
    stmt->setString (++i, partitionName);
    if (newPartition) {
      stmt->setUInt (++i, 1);
    } else {
      stmt->setUInt (++i, 0);
    }

    stmt->execute() ;
    versionMajorId = stmt->getInt(1);
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return versionMajorId;
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)</I>.<BR>
 * The data are stored in the version number :
 *   - nextMajorVersion.0 if versionUpdate = 1
 *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
 *   - currentMajorVersion.0 if versionUpdate = 2
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param versionUpdate - set this parameter to true for a next major version
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string buffer, boolean versionUpdate)  
 * @see DbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)  
 * @see PkgFecXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 */
void DbFecAccess::setXMLClobWithVersion(std::string* buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) 
  throw (oracle::occi::SQLException, FecExceptionHandler) {

  static std::string writeString("BEGIN PkgFecXML.uploadXMLClob(:bufferPll, :bufferLaserdriver, :bufferApvFec, :bufferApvMux, :bufferDcu, :partitionName, :versionMajor, :versionMinorId); END;");
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  
    unsigned int j = 0;  

#ifdef DATABASEDEBUG
    std::cerr << "DbFecAccess::setXMLClob writeString : " << writeString << std::endl;
    std::cerr << "DbFecAccess::setXMLClob partitionName : " << partitionName << std::endl;
    std::cerr << "DbFecAccess::setXMLClob versionMajorId : " << versionMajorId << std::endl;
    std::cerr << "DbFecAccess::setXMLClob versionMinorId : " << versionMinorId << std::endl;
#endif

    for (j=0; j<5; j++) {
      //    if (buffer[j].size()) {
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "DbFecAccess::setXMLClob bufferSize["<<j<<"] =" << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "DbFecAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }

#ifdef DATABASEDEBUG
      std::cerr << "DbFecAccess::setXMLClob buffer["<<j<<"] : " << buffer[j] << std::endl;
      std::cerr << "DbFecAccess::setXMLClob bufferSize["<<j<<"] = " << bufferSize << std::endl;
#endif
      
      (xmlClobArray_[j]).trim(0);
      (xmlClobArray_[j]).write(bufferSize, (unsigned char*)buffer[j].c_str(), bufferSize);
      stmt->setClob (++i, (xmlClobArray_[j]));
    }
    
    stmt->setString (++i, partitionName);
    stmt->setUInt (++i, versionMajorId);
    stmt->setUInt (++i, versionMinorId);
    
    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)</I>.<BR>
 * The data are stored in the version number :
 *   - nextMajorVersion.0 if versionUpdate = 1
 *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
 *   - currentMajorVersion.0 if versionUpdate = 2
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param versionUpdate - set this parameter to true for a next major version
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string buffer, boolean versionUpdate)  
 * @see DbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)  
 * @see PkgFecXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 */
void DbFecAccess::setXMLClob(std::string* buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler) {

  static std::string writeString("BEGIN PkgFecXML.uploadXMLClob(:bufferPll, :bufferLaserdriver, :bufferApvFec, :bufferApvMux, :bufferDcu, :partitionName, :versionUpdate); END;");
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  
    unsigned int j = 0;  

#ifdef DATABASEDEBUG
    std::cerr << "DbAccess::setXMLClob writeString : " << writeString << std::endl;
    std::cerr << "DbAccess::setXMLClob partitionName : " << partitionName << std::endl;
    std::cerr << "DbAccess::setXMLClob versionUpdate : " << versionUpdate << std::endl;
#endif
    
    for (j=0; j<5; j++) {
      //    if (buffer[j].size()) {
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "DbFecAccess::setXMLClob bufferSize["<<j<<"] =" << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "DbFecAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }
      
    
#ifdef DATABASEDEBUG
      std::cerr << "DbFecAccess::setXMLClob buffer["<<j<<"] : " << buffer[j] << std::endl;
      std::cerr << "DbFecAccess::setXMLClob bufferSize["<<j<<"] = " << bufferSize << std::endl;
#endif
    
      (xmlClobArray_[j]).trim(0);
      (xmlClobArray_[j]).write(bufferSize, (unsigned char*)buffer[j].c_str(), bufferSize);
      stmt->setClob (++i, (xmlClobArray_[j]));
    }
    
    stmt->setString (++i, partitionName);
    stmt->setUInt (++i, versionUpdate);
    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the Ring and CCU data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClobFromQuery(std::string readString, ...)</I>.<BR>
 * @param partitionName - partition name
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClobFromQuery(readString, partitionName, fecId, ring);
 * @see PkgCcuXML.getCcus (partitionName IN VARCHAR2, fecHardId IN VARCHAR2, ringSlot IN NUMBER) RETURN CLOB ;
 */
oracle::occi::Clob *DbFecAccess::getCcuXMLClob(std::string partitionName) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgCcuXML.getRingCcuFromPartition(:partitionName);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the Ring and CCU data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClobFromQuery(std::string readString, ...)</I>.<BR>
 * @param partitionName - partition name
 * @param fecId - fec identifier
 * @param ring - ringId identifier
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClobFromQuery(readString, partitionName, fecId, ring);
 * @see PkgCcuXML.getCcus (partitionName IN VARCHAR2, fecHardId IN VARCHAR2, ringSlot IN NUMBER) RETURN CLOB ;
 */
oracle::occi::Clob *DbFecAccess::getCcuXMLClob(std::string partitionName, std::string fecId, unsigned int ring) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgCcuXML.getCcus(:partitionName, :fecId, :ring);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName, fecId, ring);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the Ring and CCU data from the construction database tables.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClobFromQuery(std::string readString, ...)</I>.<BR>
 * @param fecId - fec identifier
 * @param ring - ringId identifier
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClobFromQuery(readString, partitionName, fecId, ring);
 * @see PkgCcuXML.getCcus (partitionName IN VARCHAR2, fecHardId IN VARCHAR2, ringSlot IN NUMBER) RETURN CLOB ;
 */
oracle::occi::Clob *DbFecAccess::getCcuXMLClob(std::string fecHardId, unsigned int ringSlot) throw (oracle::occi::SQLException) {

static std::string readString = "BEGIN :xmlClob := PkgCcuXML.getRingCcuConstructionTable(:fecHardId, :ring);END;";

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": readString " << readString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": fecHardId " << fecHardId << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": ring " << ringSlot << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i, fecHardId);
    stmt->setUInt(++i,ringSlot);
    stmt->execute();
    *xmlClob_ = stmt->getClob (1);
    dbConnection_->terminateStatement(stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;
}


/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the CCU data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, ...)</I>.<BR>
 * @param buffer - the RAW xml buffer to the database
 * @param partitionName - partition name
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see PkgFecXML.setXMLClob(stringRequest, buffer, partitionName);
 */
void DbFecAccess::setCcuXMLClob(std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {
  static std::string stringRequest = "BEGIN PkgCcuXML.uploadXMLClob(:xmlClob, :partitionName); END;" ;
  return setXMLClob(stringRequest, buffer, partitionName);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set 
 * two Clobs containing the RING and CCU data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, ...)</I>.<BR>
 * @param ringBuffer - the RAW RING xml buffer to the database
 * @param ccuBuffer  - the RAW CCU  xml buffer to the database
 * @param partitionName - partition name
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see PkgFecXML.setXMLClob(stringRequest, buffer, partitionName);
 */
void DbFecAccess::setRingCcuXMLClob(std::string ringBuffer, std::string ccuBuffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {
  static std::string stringRequest = "BEGIN PkgCcuXML.uploadXMLClob(:ringXmlClob,:ccuXmlClob, :partitionName); END;" ;
  return setXMLClob(stringRequest, ringBuffer, ccuBuffer, partitionName);
}


/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, boolean newPartition)</I>.<BR>
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param newPartition - set this parameter to true for a new partition
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string buffer, boolean newPartition)  
 * @see PkgFecXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 */
void DbFecAccess::setXMLClob(std::string stringRequest, std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {

  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (stringRequest);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  

#ifdef DATABASEDEBUG
    std::cerr << "DbFecAccess::setXMLClob stringRequest : " << stringRequest << std::endl;
    std::cerr << "DbFecAccess::setXMLClob partitionName : " << partitionName << std::endl;
#endif

    bufferSize = buffer.size();
   
#ifdef DATABASEDEBUG
    std::cerr << "DbFecAccess::setXMLClob bufferSize =" << bufferSize << std::endl;
#endif
    
    if ((xmlClob_)->isNull()) {
#ifdef DATABASEDEBUG
      std::cerr << "DbFecAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
      *xmlClob_ = initializeClob();     
    }
    
    
#ifdef DATABASEDEBUG
    std::cerr << "DbFecAccess::setXMLClob buffer : " << buffer << std::endl;
    std::cerr << "DbFecAccess::setXMLClob bufferSize = " << bufferSize << std::endl;
#endif
    
    (xmlClob_)->trim(0);
    (xmlClob_)->write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
    stmt->setClob (++i,*(xmlClob_));    
    stmt->setString (++i, partitionName);
    stmt->executeUpdate();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}
  

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set two Clobs of data containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method
 * <I>DbAccess::setXMLClob(std::string writeString, std::string bufferOne, std::string bufferTwo, boolean newPartition)</I>.<BR>
 * @param bufferOne - pointer on a buffer used for upload
 * @param bufferTwo - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param newPartition - set this parameter to true for a new partition
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string bufferOne, std::string bufferTwo, boolean newPartition)  
 * @see PkgCcuXML.uploadXMLClob(xmlClobRing IN CLOB, xmlClobCcu IN CLOB, nextMajor IN NUMBER)
 */
void DbFecAccess::setXMLClob(std::string stringRequest, std::string bufferOne, std::string bufferTwo, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {
  oracle::occi::Statement *stmt = NULL ;
  
  try {
    stmt = dbConnection_->createStatement (stringRequest);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;
    unsigned int j = 0;  
    std::string buffer[2];
  
    buffer[0] = bufferOne;
    buffer[1] = bufferTwo;

#ifdef DATABASEDEBUG
    std::cerr << "DbFecAccess::setXMLClob writeString : " << stringRequest << std::endl;
    std::cerr << "DbFecAccess::setXMLClob partitionName : " << partitionName << std::endl;
#endif
    
    for (j=0; j<2; j++) {
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "DbAccess::setXMLClob bufferSize["<<j<<"] =" << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "DbAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }
      
      
#ifdef DATABASEDEBUG
      std::cerr << "DbAccess::setXMLClobFromQuery buffer["<<j<<"] : " << buffer[j] << std::endl;
      std::cerr << "DbAccess::setXMLClobFromQuery bufferSize["<<j<<"] = " << bufferSize << std::endl;
#endif
      
      (xmlClobArray_[j]).trim(0);
      (xmlClobArray_[j]).write(bufferSize, (unsigned char*)buffer[j].c_str(), bufferSize);
      stmt->setClob (++i, (xmlClobArray_[j]));
    }
    
    stmt->setString (++i, partitionName);
    stmt->executeUpdate();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}
