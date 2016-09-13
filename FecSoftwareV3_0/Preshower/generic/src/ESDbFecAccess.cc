/*
  This file is part of Fec Software project.
  It is used to test the performance of DB--FecSoftware

  author: HUNG Chen-Chien(Dominique), NCU, Jhong-Li, Taiwan
  based on DbFecAccess by
  Frderic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include "ESDbFecAccess.h"

/** Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all apllications( by default false)
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::ESDbAccess()
 */
ESDbFecAccess::ESDbFecAccess(bool threaded) throw (oracle::occi::SQLException) : ESDbAccess(threaded){
#ifdef DATABASESTAT
  this->traceSql(true);
#endif
}

/** Create an access to the database
 * @param user - user name
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::ESDbAccess(std::string user, std::string passwd, std::string dbPath)
 */
ESDbFecAccess::ESDbFecAccess(std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : ESDbAccess (user, passwd, dbPath, threaded){
}

/** Close the access to the database
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::~ESDbAccess()
 */
ESDbFecAccess::~ESDbFecAccess() throw (oracle::occi::SQLException){
#ifdef DATABASESTAT
  this->traceSql(false);
#endif
}

/** Get the current version for a given partition name
 * @param partitionName - Partition Name
 * @return a pointer to a list of unsigned int pointers(partitionId, versionMajorId, versionMinorId): THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::getDatabaseVersion (std::string sqlQuery)
 * @todo use a callable statement to execute a PL/SQL function with bind variable to avoid the query parsing at every call
 */
std::list<unsigned int*> ESDbFecAccess::getDatabaseVersion(std::string partitionName) throw (oracle::occi::SQLException){
  
  static std::string sqlQuery = "SELECT DISTINCT StateHistory.partitionId, StateHistory.fecVersionMajorId, StateHistory.fecVersionMinorId FROM CurrentState, StateHistory, Partition WHERE StateHistory.stateHistoryId = CurrentState.stateHistoryId AND StateHistory.partitionId = Partition.partitionId AND Partition.partitionName = :partitionName";
  
  std::list<unsigned int*> listVersion = ESDbAccess::getDatabaseVersionFromQuery(sqlQuery, partitionName); 
  return listVersion;
}

/** Create a new current state with partitions/versions. <BR>
 * Select the current state and check if it is registered (check its registered parameter value). <BR>
 * If this state is registered : a new state is created <BR>
 * The state is updated with the data found in the parameter <I>partitionVersionsList</I>. <BR>
 * The current state is updated to this new state. <BR>
 * @param partitionVersionsList : pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId): THE CONTENT OF THIS LIST OF POINTERS IS REMOVED BY THE FUNCTION !!
 * @exception FecExceptionHandler : an exception is raised in case of trouble in statement creation or execution
 * @see PkgStateHistory.createNewCurrentState RETURN NUMBER;
 * @see PkgStateHistory.setValues(paramstateHistoryId IN NUMBER, paramPartitionId IN NUMBER, fecVersionMajor IN NUMBER, fecVersionMinor IN NUMBER);
 */
void ESDbFecAccess::setDatabaseVersion(std::list<unsigned int*> partitionVersionsList) throw (FecExceptionHandler){

  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ;
  static std::string sqlQuery = "SELECT DISTINCT StateHistoryId FROM CurrentState";
  static std::string queryString = "BEGIN :a := PkgStateHistory.createNewCurrentState();END;";
  
  try {
    unsigned int currentStateId = 0;
    unsigned int *partitionVersions;
		
    stmt = dbConnection_->createStatement (sqlQuery);
    rset = stmt->executeQuery();
    while (rset->next()){
      currentStateId = rset->getInt(1);
    }
    dbConnection_->terminateStatement(stmt);
		
		// The state cannot be changed so a new one is created
    // as we could not modify this state, we need to 
    //create a new state and set this state as current state
    stmt = dbConnection_->createStatement(queryString);
			
    stmt->registerOutParam(1, oracle::occi::OCCIINT, sizeof(currentStateId));
			
    if (stmt->executeUpdate() == 1){
			currentStateId = stmt->getInt(1);
			dbConnection_->terminateStatement(stmt);
    }
    else {
      dbConnection_->terminateStatement(stmt);
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to create a new state history" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
		
    static std::string sqlQuery1 = "UPDATE StateHistory SET StateHistory.fecVersionMajorId, = :a, StateHistory.fecVersionMinorId = :b WHERE StateHistory.partitionId = :c AND StateHistory.stateHistoryId = :d";
    
    stmt = dbConnection_->createStatement(sqlQuery1);
    stmt->setUInt(4, currentStateId);
    
    while (partitionVersionsList.size() > 0){
      partitionVersions = (unsigned int *)partitionVersionsList.back();
			
      stmt->setUInt(3, partitionVersions[0]);
      stmt->setUInt(1, partitionVersions[1]);
      stmt->setUInt(2, partitionVersions[2]);
      stmt->executeUpdate();
			
    }
    dbConnection_->terminateStatement(stmt);
		
  }	
  catch (oracle::occi::SQLException &ex) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);
    std::stringstream msg ;
    msg << DB_PLSQLEXCEPTIONRAISED_MSG << " (" << queryString << "): " << "unable to create a new state history: " << ex.what() ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, msg.str(), ERRORCODE) ;
  }	
}


/** Define the query to get the next major version and execute the request
 * @return the next available major version
 * @exception SQLException
 * @see ESDbAccess::getVersion(std::string sqlQuery)
 */
unsigned int ESDbFecAccess::getNextMajorVersion() throw (oracle::occi::SQLException){
	static std::string sqlQuery = "SELECT MAX(versionMajorId) + 1 FROM FecVersion";
  return ESDbAccess::getVersion(sqlQuery);
}


/** Define the query to get the next minor version with major version = majorId and execute the request
 * @return the greatest minor version with major version = majorId
 * @param majorId : major version number
 * @exception SQLException
 * @see ESDbAccess::getVersion(std::string sqlQuery)
 */
unsigned int ESDbFecAccess::getNextMinorVersion(unsigned int majorId) throw (oracle::occi::SQLException){
  static std::string sqlQuery = "SELECT MAX(versionMinorId) + 1 FROM FecVersion WHERE versionMajorId =" + toString(majorId);
  return ESDbAccess::getVersion(sqlQuery);
}


/** Create a configuration StateHistory from partition
 * @param partitionName - partition name
 * @param partitionId - new created partition identifier(output parameter)
 * @param fecVersionMajorId - returned new version major number for the Fec
 * @return new stateHistoryId
 * @exception FecExceptionHandler : a FecExceptionHandler is raised in case of trouble in creating or executing the statement.
 * @see PkgStateHistory.createConfigurationState(paramPartitionName IN VARCHAR2, paramPartitionNb OUT NUMBER, paramFecVersionMajorNumber OUT NUMBER) RETURN NUMBER;
 */
unsigned int ESDbFecAccess::createNewStateHistory(std::string partitionName, unsigned int *partitionId, unsigned int * fecVersionMajorId) throw (FecExceptionHandler){
  
  unsigned int stateHistoryId = 0;
  unsigned int i = 0;
  static std::string queryString = "BEGIN :a := PkgStateHistory.createConfigurationState(:b, :c, :d);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(queryString);
		
    stmt->registerOutParam(++i, oracle::occi::OCCIINT, sizeof(stateHistoryId));
    stmt->setString(++i, partitionName);
    stmt->registerOutParam(++i, oracle::occi::OCCIINT, sizeof(partitionId));
    stmt->registerOutParam(++i, oracle::occi::OCCIINT, sizeof(fecVersionMajorId));
		
    if (stmt->executeUpdate() == 1){
      stateHistoryId = stmt->getInt(1);
      *partitionId = stmt->getInt(3);
      *fecVersionMajorId = stmt->getInt(4);
      dbConnection_->terminateStatement(stmt);
    }
    else {
      dbConnection_->terminateStatement(stmt);
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to create a new state history" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &ex) {
    dbConnection_->terminateStatement(stmt);
    std::stringstream msg ;
    msg << DB_PLSQLEXCEPTIONRAISED_MSG << " (" << queryString << "): " << "unable to create a new state history: " << ex.what() ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, msg.str(), ERRORCODE) ;
  }
  return stateHistoryId;
}

/** Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::getXMLClob(std::string readString, std::string partitionName)</I>.<BR>
 * @param partitionName - name of the partition to be accessed
 * @return a pointer on a Clob containing the data from the database
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::getXMLClob(std::string partitionName)
 * @see ESDbAccess::getXMLClob(std::string readString, std::string partitionName)
 * @see PkgFecXML.getAllFecFromPartition(partitionName IN VARCHAR2) RETURN CLOB;
 */
oracle::occi::Clob *ESDbFecAccess::getXMLClob(std::string partitionName) throw (oracle::occi::SQLException){
  static std::string readString = "BEGIN :xmlClob := PkgFecXML.getAllFecFromPartition(:partitionName);END;";
  return ESDbAccess::getXMLClobFromQuery(readString, partitionName);
}

/** Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob	containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)</I>.<BR>
 * @param partitionName - partition name
 * @param versionMajor - major version number
 * @param versionMinor - minor version number
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::getXMLClob(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)
 * @see ESDbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)
 * @see PkgFecXML.getAllFecFromPartition(partitionName IN VARCHAR2, versionMajorId IN NUMBER, versionMinorId IN NUMBER) RETURN CLOB;
 */
oracle::occi::Clob *ESDbFecAccess::getXMLClobWithVersion(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor) throw (oracle::occi::SQLException){
  static std::string readString = "BEGIN :xmlClob := PkgFecXML.getAllFecFromPartition(:partitionName, :versionMajorId, :versionMinorId);END;";
  return ESDbAccess::getXMLClobFromQuery(readString, partitionName, versionMajor, versionMinor);
}


/** Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::getXMLClob(std::string readString, unsigned int id)</I>.<BR>
 * @param partitionName - partition name
 * @param fecId - fec identifier
 * @return a pointer on a Clob containing the data from the database
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::getXMLClob(unsigned int id, boolean hardId)
 * @see ESDbAccess::getXMLClob(std::string readString, unsigned int id)
 * @see PkgFecXML.getAllFecFromId(fecHardId IN VARCHAR2) RETURN CLOB;
 */
oracle::occi::Clob *ESDbFecAccess::getXMLClob(std::string partitionName, std::string fecId) throw (oracle::occi::SQLException){
  static std::string readString = "BEGIN :xmlClob := PkgFecXML.getAllFecFromPartitionAndId(:partitionName, :fecId);END;";
  return ESDbAccess::getXMLClobFromQuery(readString, partitionName, fecId);
}
		
	
/** Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::getXMLClob(std::string readString, unsigned int id, unsigned int versionMajorId, unsigned int versionMinorId)</I>.<BR>
 * @param partitionName - partition name
 * @param fecId - fec identifier
 * @param versionMajorId - major version number
 * @param versionMinorId - minor version number
 * @return a pointer on a Clob containing the data from the database
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::getXMLClob(unsigned int id, boolean hardId, unsigned int versionMajorId, unsigned int versionMinorId)
 * @see ESDbAccess::getXMLClob(std::string readString, unsigned int id, unsigned int versionMajorId, unsigned int versionMinorId)
 * @see PkgFecXML.getAllFecFromId(fecHardId IN VARCHAR2, versionMajorId IN NUMBER, versionMinorId IN NUMBER) RETURN CLOB;
 */			
oracle::occi::Clob *ESDbFecAccess::getXMLClobWithVersion(std::string partitionName, std::string fecId, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException){
  static std::string readString = "BEGIN :xmlClob := PkgFecXML.getAllFecFromPartitionAndId(:partitionName, :fecId, :versionMajorId, :versionMinorId);END;";
  return ESDbAccess::getXMLClobFromQuery(readString, partitionName, fecId, versionMajorId, versionMinorId);
}


/** Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::setXMLClob(std::string writeString, std::string buffer, std::string partitionName)</I>.<BR>
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param newPartition - boolean : true if you need to create a new partition
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see ESDbAccess::setXMLClob(std::string writeString, std::string buffer, std:: string partitionName)
 * @see PkgFecXML.configureXMLClob (xmlClob IN CLOB, partitionName IN VARCHAR2) RETURN NUMBER; 
 * @see PkgFecXML.addXMLClob(xmlClob IN CLOB, partitionName IN VARCHAR2) RETURN NUMBER;
 */	
unsigned int ESDbFecAccess::setXMLClob(std::string* buffer, std::string partitionName, boolean newPartition) throw (oracle::occi::SQLException, FecExceptionHandler){
  std::string writeString("BEGIN :versionMajorId := PkgFecXML.configureXMLClob(:bufferPll, :bufferKchip, :bufferMbDcu, :bufferPaceam, :bufferDelta, :bufferDmDcu, :partitionName, :createNewPartition);END;");  
	
  unsigned int versionMajorId = 0;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;
    unsigned int j = 0;
	
#ifdef DATABASEDEBUG
    std::cerr << "ESDbFecAccess::setXMLClob writeString : " << writeString << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClob partitionName : " << partitionName << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClob newPartition : " << newPartition << std::endl;
#endif
    
    stmt->registerOutParam(++i, oracle::occi::OCCIINT, sizeof(versionMajorId));
    for (j=0; j<6; j++){
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbFecAccess::setXMLClob bufferSize[" << j << "] =" << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()){
#ifdef DATABASEDEBUG
	std::cerr << "ESDbFecAccess::setXMLClob xmlClob_attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }
		
#ifdef DATABASEDEBUG
      std::cerr << "ESDbFecAccess::setXMLClob buffer[" << j << "] : " << buffer[j] << std::endl;
      std::cerr << "ESDbFecAccess::setXMLClob bufferSize[" << j << "] = " << bufferSize << std::endl;
#endif
      
      (xmlClobArray_[j]).trim(0);
      (xmlClobArray_[j]).write(bufferSize, (unsigned char*)buffer[j].c_str(), bufferSize);
      stmt->setClob(++i, (xmlClobArray_[j]));
    }//for (j=0; j<6; j++)
    
    stmt->setString(++i, partitionName);
    if (newPartition){
      stmt->setUInt(++i, 1);
    }
    else {
      stmt->setUInt(++i, 0);
    }
    stmt->execute();
    versionMajorId = stmt->getInt(1);
    dbConnection_->commit();
    dbConnection_->terminateStatement(stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return versionMajorId;
}


/** Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::setXMLClob(std:string writeString, std::string buffer, boolean versionUpdate)</I>.<BR>
 * The data are stored in the version number :
 * 	- nextMajorVersion.0 if versionUpdate = 1
 * 	- currentMajorVersion.nextMinorVersion if versionUpdate = 0
 * 	- currentMajorVersion.0 if versionUpdate = 2
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param versionUpdate - set this parameter to true for a next major version
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see ESDbAccess::setXMLClob(std::string buffer, boolean versionUpdate)
 * @see ESDbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)
 * @see PkgFecXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 */
void ESDbFecAccess::setXMLClobWithVersion(std::string* buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException, FecExceptionHandler){
  std::string writeString("BEGIN PkgFecXML.uploadXMLClob(:bufferPll, :bufferKchip, :bufferMbDcu, :bufferPaceam, :bufferDelta, :bufferDmDcu, :partitionName, :versionMajor, :versionMinorId); END;");
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;
    unsigned int j = 0;
	
#ifdef DATABASEDEBUG
    std::cerr << "ESDbFecAccess::setXMLClobWithVersion writeString: " << writeString << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClobWithVersion partitionName : " << partitionName << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClobWithVersion versionMajorId : " << versionMajorId << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClobWithVersion versionMinorId : " << versionMinorId << std::endl;
#endif
    
    for (j=0; j<6; j++){
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbFecAccess::setXMLClobWithVersion bufferSize[" << j << "] = " << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()){
#ifdef DATABASEDEBUG
	std::cerr << "ESDbFecAccess::setXMLClobWithVersion xmlClob_attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }
		
#ifdef DATABASEDEBUG
      std::cerr << "ESDbFecAccess::setXMLClobWithVersion buffer[" << j << "] : " << buffer[j] << std::endl;
      std::cerr << "ESDbFecAccess::setXMLClobWithVersion bufferSize[" << j << "] = " << bufferSize << std::endl;
#endif
		
      (xmlClobArray_[j]).trim(0);
      (xmlClobArray_[j]).write(bufferSize, (unsigned char*)buffer[j].c_str(), bufferSize);
      stmt->setClob(++i, (xmlClobArray_[j]));
    }
    
    stmt->setString(++i, partitionName);
    stmt->setUInt(++i, versionMajorId);
    stmt->setUInt(++i, versionMinorId);
    
    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement(stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

		
/** Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::setXMLClob(std:string writeString, std::string buffer, boolean versionUpdate)</I>.<BR>
 * The data are stored in the version number :
 * 	- nextMajorVersion.0 if versionUpdate = 1
 * 	- currentMajorVersion.nextMinorVersion if versionUpdate = 0
 * 	- currentMajorVersion.0 if versionUpdate = 2
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param versionUpdate - set this parameter to true for a next major version
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see ESDbAccess::setXMLClob(std::string buffer, boolean versionUpdate)
 * @see ESDbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)
 * @see PkgFecXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 */
void ESDbFecAccess::setXMLClob(std::string* buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler){
  static std::string writeString("BEGIN PkgFecXML.uploadXMLClob(:bufferPll, :bufferKchip, :bufferMbDcu, :bufferPaceam, :bufferDelta, :bufferDmDcu, :partitionName, :versionUpdate); END;");
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;
    unsigned int j = 0;
  
#ifdef DATABASEDEBUG
    std::cerr << "ESDbFecAccess::setXMLClob writeString : " << writeString << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClob partitionName : " << partitionName << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClob versionUpdate : " << versionUpdate << std::endl;
#endif
	
    for (j=0; j<6; j++){
      bufferSize = buffer[j].size();	
#ifdef DATABASEDEBUG
      std::cerr << "ESDbFecAccess::setXMLClob bufferSize[" << j << "] = " << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()){
#ifdef DATABASEDEBUG
	std::cerr << "ESDbFecAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }
      
#ifdef DATABASEDEBUG
      std::cerr << "ESDbFecAccess::setXMLClob buffer[" << j << "] : " << buffer[j] << std::endl;
      std::cerr << "ESDbFecAccess::setXMLClob bufferSize[" << j << "] = " << bufferSize << std::endl;
#endif
      
      (xmlClobArray_[j]).trim(0);
      (xmlClobArray_[j]).write(bufferSize, (unsigned char*)buffer[j].c_str(), bufferSize);
      stmt->setClob(++i, (xmlClobArray_[j]));
      
    }// for(j=0; j<6; j++)	
    
    stmt->setString(++i, partitionName);
    stmt->setUInt(++i, versionUpdate);
    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement(stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
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
oracle::occi::Clob *ESDbFecAccess::getCcuXMLClob(std::string partitionName, std::string fecId, unsigned int ring) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgCcuXML.getCcus(:partitionName, :fecId, :ring);END;";
  return ESDbAccess::getXMLClobFromQuery(readString, partitionName, fecId, ring);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the CCU data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, ...)</I>.<BR>
 * @param buffer - the RAW xml buffer to the database
 * @param partitionName - partition name
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see PkgFecXML.setXMLClob(stringRequest, buffer, partitionName);
 */
void ESDbFecAccess::setCcuXMLClob(std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {
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
void ESDbFecAccess::setRingCcuXMLClob(std::string ringBuffer, std::string ccuBuffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {
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
void ESDbFecAccess::setXMLClob(std::string stringRequest, std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {

  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (stringRequest);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  

#ifdef DATABASEDEBUG
    std::cerr << "ESDbFecAccess::setXMLClob stringRequest : " << stringRequest << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClob partitionName : " << partitionName << std::endl;
#endif

    bufferSize = buffer.size();
   
#ifdef DATABASEDEBUG
    std::cerr << "ESDbFecAccess::setXMLClob bufferSize =" << bufferSize << std::endl;
#endif
    
    if ((xmlClob_)->isNull()) {
#ifdef DATABASEDEBUG
      std::cerr << "ESDbFecAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
      *xmlClob_ = initializeClob();     
    }
    
    
#ifdef DATABASEDEBUG
    std::cerr << "ESDbFecAccess::setXMLClob buffer : " << buffer << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClob bufferSize = " << bufferSize << std::endl;
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
void ESDbFecAccess::setXMLClob(std::string stringRequest, std::string bufferOne, std::string bufferTwo, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {
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
    std::cerr << "ESDbFecAccess::setXMLClob writeString : " << stringRequest << std::endl;
    std::cerr << "ESDbFecAccess::setXMLClob partitionName : " << partitionName << std::endl;
#endif
    
    for (j=0; j<2; j++) {
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClob bufferSize["<<j<<"] =" << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "ESDbAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }
      
      
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery buffer["<<j<<"] : " << buffer[j] << std::endl;
      std::cerr << "ESDbAccess::setXMLClobFromQuery bufferSize["<<j<<"] = " << bufferSize << std::endl;
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

			
