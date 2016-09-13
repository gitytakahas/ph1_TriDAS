/*
This file is part of Fec Software project.
It is used to test the performance of DB--FecSoftware

author: HUNG Chen-Chien(Dominique), NCU, Jhong-Li, Taiwan
based on DbAccess by
Frderic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "ESDbAccess.h"


/**Create a database access. <BR>
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * Retreive the connection parameters <I>user</I>, <I>password</I> and <I>database path</I> from the CONFDB environment variable, using <I>DbAccess::getDbConfiguration</I> method. <BR>
 * Connect to the database with the <I>DbAccess::DbConnect method</I>. <BR>
 * Instanciate the <I>xmlClob_</I> attribute with the default <I>oracle::occi::Clob</I> constructor. <BR>
 * Initialize the attributes <I>user_</I>, <I>passwd_</I> and <I>path_</I>. <BR>
 * @exception oracle::occi::SQLException thrown by <I>DbAccess::DbConnect</I>
 * @see <I>DbAccess::getDbConfiguration</I>, <I>DbAccess::DbConnect</I>
 */
ESDbAccess::ESDbAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbCommonAccess (threaded) {
}

/**Create a database access. <BR>
 * Connection parameters user, password and database path are given as constructor parameters. <BR>
 * Connect to the database with the DbAccess::DbConnect method. <BR>
 * Instanciate the <I>xmlClob_</I> attribute with the default <I>oracle::occi::Clob</I> constructor. <BR>
 * Initialize the attributes <I>user_</I>, <I>passwd_</I> and <I>path_</I>. <BR>
 * @param user - username
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException thrown by DbAccess::DbConnect
 * @see DbAccess::DbConnect
 */
ESDbAccess::ESDbAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbCommonAccess (user, passwd, dbPath, threaded) {
  
}

/**Delete the database access. <BR>
 * Deletes the <I>xmlClob_</I> object.<BR>
 * Closes the connection with the database.<BR>
 * Closes the environnement.<BR>
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in closing the environnement or the connection
 * */
ESDbAccess::~ESDbAccess () throw (oracle::occi::SQLException) {
}

/**Get the current version. <BR>
 * The parameter <I>sqlQuery</I> is defined, according the needed version (Fec, PiaReset). <BR>   
 * A statement is created and executed from this query string. <BR>
 * The result is a list of partition identifiers and corresponding version numbers. <BR> 
 * @param sqlQuery - request used to define the statement
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::list<unsigned int*> ESDbAccess::getDatabaseVersionFromQuery (std::string sqlQuery) throw (oracle::occi::SQLException) {

  std::list<unsigned int*> partitionVersionsListe;
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ;

  try {
    stmt = dbConnection_->createStatement (sqlQuery);  
    rset = stmt->executeQuery ();
    
    while (rset->next ()) {
      unsigned int *partitionVersions = new unsigned int[3] ;
      partitionVersions[0] = (unsigned int)rset->getUInt (1);
      partitionVersions[1] = (unsigned int)rset->getUInt (2);
      partitionVersions[2] = (unsigned int)rset->getUInt (3);
      partitionVersionsListe.push_back(partitionVersions);
    }

    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return partitionVersionsListe;
}

/**Get the current version. <BR>
 * The parameter <I>sqlQuery</I> is defined, according the needed version (Fec, PiaReset). <BR>   
 * A statement is created and executed from this query string. <BR>
 * The result is a list of partition identifiers and corresponding version numbers. <BR> 
 * @param sqlQuery - request used to define the statement
 * @param partitionName - partition name
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::list<unsigned int*> ESDbAccess::getDatabaseVersionFromQuery (std::string sqlQuery, std::string partitionName) throw (oracle::occi::SQLException) {
  
  std::list<unsigned int*> partitionVersionsListe;
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ;

  try {
  	unsigned int i=0;
    stmt = dbConnection_->createStatement (sqlQuery);
    stmt->setString(++i, partitionName);
    rset = stmt->executeQuery ();
    while (rset->next ()) {
      unsigned int *partitionVersions = new unsigned int[3] ;
      partitionVersions[0] = (unsigned int)rset->getUInt (1);
      partitionVersions[1] = (unsigned int)rset->getUInt (2);
      partitionVersions[2] = (unsigned int)rset->getUInt (3);
      partitionVersionsListe.push_back(partitionVersions);
    }
    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return partitionVersionsListe;
}

/**Get the partition name from partition identifier. <BR>
 * A query is defined to get the partition name from the partition identifier. <BR>
 * A statement is created and executed from this query string. <BR>
 * The result is the partition name. <BR> 
 * @param partitionId - the partition identifier
 * @return the corresponding partition name
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::string ESDbAccess::getPartitionName (unsigned int partitionId)  throw (oracle::occi::SQLException){
  
  std::string partitionName;
  static std::string queryString = "BEGIN :a := PkgPartition.getPartitionName(:b);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
  	unsigned int i=0;
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(++i,oracle::occi::OCCISTRING,110);
    stmt->setUInt(++i, partitionId);
    stmt->executeUpdate();
    partitionName = stmt->getString(1);
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return partitionName;
}

/** Get the partition ID from partition name. <BR>
 * A query is defined to get the partition ID from the partition name. <BR>
 * A statement is created and executed from this query string. <BR>
 * The result is the partition ID. <BR>
 * @param partitionName - the partition name
 * @return the corresponding partition name
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
unsigned int ESDbAccess::getPartitionId (std::string partitionName) throw (oracle::occi::SQLException){
	 
	 unsigned int partitionId;
	 static std::string queryString = "BEGIN :a := PkgPartition.getPartitionName(:b);END;";
	 oracle::occi::Statement *stmt = NULL;
	 
	 try {
	 	unsigned int i = 0;
	 	stmt = dbConnection_->createStatement(queryString);
	 	stmt->registerOutParam(++i, oracle::occi::OCCISTRING, sizeof(partitionId));
	 	stmt->setString(++i, partitionName);
	 	stmt->executeUpdate();
	 	partitionId = stmt->getUInt(1);
	 	dbConnection_->terminateStatement(stmt);
	 }
	 catch(oracle::occi::SQLException &e){
	 	dbConnection_->terminateStatement(stmt);
	 	throw e;
	 }
	 
	 return partitionId;	 
}


/**Get a version number. <BR>
 * The parameter <I>sqlQuery</I> is defined, according the needed version number (Fec, PiaReset). <BR>   
 * A statement is created and executed from this query string. <BR>
 * The result is a version number. <BR> 
 * @param sqlQuery - request used to define the statement
 * @return a version number
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement.
 */
unsigned int ESDbAccess::getVersion(std::string sqlQuery) throw (oracle::occi::SQLException)
{
  int versionNumber = -1;
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 

  try {
    stmt = dbConnection_->createStatement (sqlQuery);
    rset = stmt->executeQuery ();
    while (rset->next ()){   
      versionNumber = (unsigned int)rset->getUInt (1);
    }
    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return versionNumber;
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database, for the current version.<BR>
 * Criteria is partition name
 * A statement is created with the <I>readString</I> parameter and executed with partition name as criteria. <BR>
 * @param readString - PL/SQL request
 * @param partitionName - name of the partition to be accessed
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
oracle::occi::Clob *ESDbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName ) throw (oracle::occi::SQLException) {
	
#ifdef DATABASEDEBUG
  std::cerr << "ESDbAccess::getXMLClobFromQuery readString " << readString << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery partitionName " << partitionName << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    stmt->executeUpdate();
    *xmlClob_ = stmt->getClob (1);
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database for the version <I>versionMajorId.versionMinorId</I>.<BR>
 * A statement is created with the <I>readString</I> parameter and executed with partition name, version major and version minor as criteria. <BR>
 * @param readString - PL/SQL request
 * @param partitionName - name of the partition to be accessed
 * @param versionMajorId - major version number
 * @param versionMinorId - minor version number 
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
oracle::occi::Clob *ESDbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId ) throw (oracle::occi::SQLException) {
	
#ifdef DATABASEDEBUG
  std::cerr << "ESDbAccess::getXMLClobFromQuery readString " << readString << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery partitionName " << partitionName << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery versionMajorId " << versionMajorId << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery versionMinorId " << versionMinorId << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    stmt->setUInt(++i,versionMajorId);
    stmt->setUInt(++i,versionMinorId);
    stmt->executeUpdate();
    *xmlClob_ = stmt->getClob (1);
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data in the current state from the database.<BR>
 * A statement is created with the <I>readString</I> parameter and executed with an identifier as criteria. <BR>
 * @param readString - PL/SQL request
 * @param partitionName - partition name
 * @param id - identifier
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
oracle::occi::Clob *ESDbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName, std::string id) throw (oracle::occi::SQLException) {
	
#ifdef DATABASEDEBUG
  std::cerr << "ESDbAccess::getXMLClobFromQuery readString " << readString << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery partitionName " << partitionName << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery id " << id << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i, partitionName);
    stmt->setString(++i, id);
    stmt->executeUpdate();
    *xmlClob_ = stmt->getClob (1);
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database for the version <I>versionMajorId.versionMinorId</I>.<BR>
 * A statement is created with the <I>readString</I> parameter and executed with partition name, identifier, version major and version minor as criteria. <BR>
 * @param readString - PL/SQL request
 * @param partitionName - partition name
 * @param id - identifier
 * @param versionMajorId - major version number
 * @param versionMinorId - minor version number 
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
oracle::occi::Clob *ESDbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName, std::string id, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) {
	
#ifdef DATABASEDEBUG
  std::cerr << "ESDbAccess::getXMLClobFromQuery readString " << readString << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery partitionName " << partitionName << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery id " << id << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery versionMajorId " << versionMajorId << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery versionMinorId " << versionMinorId << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    stmt->setString(++i,id);
    stmt->setUInt(++i,versionMajorId);
    stmt->setUInt(++i,versionMinorId);
    stmt->executeUpdate();
    *xmlClob_ = stmt->getClob (1);
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database for the version <I>versionMajorId.versionMinorId</I>.<BR>
 * A statement is created with the <I>readString</I> parameter and executed with partition name, identifier, version major and version minor as criteria. <BR>
 * @param readString - PL/SQL request
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
oracle::occi::Clob *ESDbAccess::getXMLClobFromQuery(std::string readString) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << "ESDbAccess::getXMLClobFromQuery readString " << readString << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
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


/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data in the current state from the database.<BR>
 * A statement is created with the <I>readString</I> parameter and executed with an identifier as criteria. <BR>
 * @param readString - PL/SQL request
 * @param partitionName - partition name
 * @param id - identifier
 * @param r - ring
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
oracle::occi::Clob *ESDbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName, std::string fecHardId, unsigned int ringSlot) throw (oracle::occi::SQLException) {
 
#ifdef DATABASEDEBUG
  std::cerr << "ESDbAccess::getXMLClobFromQuery readString " << readString << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery partitionName " << partitionName << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery fecHardId " << fecHardId << std::endl;
  std::cerr << "ESDbAccess::getXMLClobFromQuery ring " << ringSlot << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i, partitionName);
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


/**Sends a request to the database to execute a PL/SQL stored procedure in order to put a Clob containing the data to the database.<BR>
 * A statement is created with the <I>writeString</I> parameter and the parameters are stored in version <I>versionMajorId.versionMinorId</I><BR>
 * @param writeString - PL/SQL request
 * @param buffer - buffer containing the data to be uploaded
 * @param partitionName - partition name
 * @param versionMajorId - version major number
 * @param versionMinorId - version minor number
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the buffer is empty
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
void ESDbAccess::setXMLClobFromQuery(std::string writeString, std::string buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException, FecExceptionHandler) {
	
#ifdef DATABASEDEBUG
  std::cerr << "ESDbAccess::setXMLClobFromQuery writeString : " << writeString << std::endl;
  std::cerr << "ESDbAccess::setXMLClobFromQuery buffer : " << buffer << std::endl;
  std::cerr << "ESDbAccess::setXMLClobFromQuery partitionName : " << partitionName << std::endl;
  std::cerr << "ESDbAccess::setXMLClobFromQuery versionMajorId : " << versionMajorId << std::endl;
  std::cerr << "ESDbAccess::setXMLClobFromQuery versionMinorId : " << versionMinorId << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;

    if (buffer.size()) {
      bufferSize = buffer.size();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery bufferSize = "  << bufferSize << std::endl;
#endif
      unsigned int i = 0;  
      if ((*xmlClob_).isNull()) {
#ifdef DATABASEDEBUG
      	std::cerr << "ESDbAccess::setXMLClobFromQuery xmlClob_ attribute is not initialized" << std::endl;
#endif
				*xmlClob_ = initializeClob();
      }
      
      xmlClob_->trim(0);
      xmlClob_->write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
      stmt->setClob (++i, (*xmlClob_));
      stmt->setString (++i, partitionName);
      stmt->setUInt (++i, versionMajorId);
      stmt->setUInt (++i, versionMinorId);
      stmt->execute();
      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt);
    } 
    else {
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery buffer is empty" << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Unable to perform an upload, buffer is empty", ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to put a Clob containing the data to the database.<BR>
 * A statement is created with the <I>writeString</I> parameter.<BR>
 * The data are stored in the version number :
 *   - nextMajorVersion.0 if versionUpdate = 1
 *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
 *   - currentMajorVersion.0 if versionUpdate = 2
 * @param writeString - PL/SQL request
 * @param buffer - buffer containing the data to be uploaded
 * @param partitionName - partition name
 * @param versionUpdate - set this parameter to true for a next major version
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the buffer is empty
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
void ESDbAccess::setXMLClobFromQuery(std::string writeString, std::string buffer, std::string partitionName, int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler){

#ifdef DATABASEDEBUG
  std::cerr << "ESDbAccess::setXMLClobFromQuery buffer : " << buffer << std::endl;
  std::cerr << "ESDbAccess::setXMLClobFromQuery writeString : " << writeString << std::endl;
  std::cerr << "ESDbAccess::setXMLClobFromQuery partitionName : " << partitionName << std::endl;
  std::cerr << "ESDbAccess::setXMLClobFromQuery versionMajorUpdate : " << versionUpdate << std::endl;
  std::cerr << xmlClob_ << ": " << xmlClob_->isNull() << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;
  
  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;

    if (buffer.size()) {
      bufferSize = buffer.size();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery bufferSize = "  << bufferSize << std::endl;
#endif
      if (xmlClob_->isNull()) {
#ifdef DATABASEDEBUG
      	std::cerr << "ESDbAccess::setXMLClobFromQuery xmlClob_ attribute is not initialized" << std::endl;
#endif
				*xmlClob_ = initializeClob();
      }
      
      unsigned int i = 0;
      
      xmlClob_->trim(0);
      xmlClob_->write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
      stmt->setClob (++i, (*xmlClob_));
      stmt->setString (++i, partitionName);
      stmt->setUInt (++i, versionUpdate);
      
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery ready to execute..."<< std::endl;
#endif
      
      stmt->executeUpdate();
      
      dbConnection_->commit();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery statement executed..."<< std::endl;
#endif

      dbConnection_->terminateStatement (stmt);
    } 
    else {
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery buffer is empty" << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Unable to perform an upload, buffer is empty", ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to put a Clob containing the data to the database.<BR>
 * A statement is created with the <I>writeString</I> parameter and the parameters are stored in the partition <I>partitionName</I> <BR>
 * @param writeString - PL/SQL request
 * @param buffer - buffer containing the data to be uploaded
 * @param partitionName - partition name
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the buffer is empty
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
unsigned int ESDbAccess::setXMLClobFromQuery(std::string writeString, std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler){

  oracle::occi::Statement *stmt = NULL ;
  unsigned int versionMajorId = 0;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    
    if (buffer.size()) {
      bufferSize = buffer.size();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery bufferSize =" << buffer.size() << " = " << bufferSize << std::endl;
#endif
      unsigned int i = 0;  
      if (xmlClob_->isNull()) {
#ifdef DATABASEDEBUG
      	std::cerr << "ESDbAccess::setXMLClobFromQuery xmlClob_ attribute is not initialized" << std::endl;
#endif
	*xmlClob_ = initializeClob();
      }
      
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery writeString : " << writeString << std::endl;
      std::cerr << "ESDbAccess::setXMLClobFromQuery buffer : " << buffer << std::endl;
      std::cerr << "ESDbAccess::setXMLClobFromQuery bufferSize : " << bufferSize << std::endl;
      std::cerr << "ESDbAccess::setXMLClobFromQuery partitionName : " << partitionName << std::endl;
#endif
      
      xmlClob_->trim(0);
      xmlClob_->write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
      stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMajorId));
      stmt->setClob (++i, (*xmlClob_));
      stmt->setString (++i, partitionName);
      
      stmt->execute();
      versionMajorId = stmt->getUInt(1);
      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt);
    } 
    else {
#ifdef DATABASEDEBUG
    std::cerr << "ESDbAccess::setXMLClobFromQuery buffer is empty" << std::endl;
#endif
    RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Unable to perform an upload, buffer is empty", ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
  
  return versionMajorId;
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to put a Clob containing the data to the database.<BR>
 * A statement is created with the <I>writeString</I> parameter and the parameters are stored in the partition <I>partitionName</I><BR>
 * @param writeString - PL/SQL request
 * @param buffer - buffer containing the data to be uploaded
 * @param partitionName - partition name
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the buffer is empty
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
unsigned int ESDbAccess::setXMLClobFromQuery(std::string writeString, std::string *buffer, std::string partitionName, bool newPartition) throw (oracle::occi::SQLException, FecExceptionHandler){
  
  unsigned int versionMajorId = 0;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  
    unsigned int j = 0;  

#ifdef DATABASEDEBUG
    std::cerr << "ESDbAccess::setXMLClobFromQuery writeString : " << writeString << std::endl;
    std::cerr << "ESDbAccess::setXMLClobFromQuery partitionName : " << partitionName << std::endl;
    std::cerr << "ESDbAccess::setXMLClobFromQuery newPartition : "<< newPartition << std::endl;
#endif

    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMajorId));
    for (j=0; j<5; j++) {
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbAccess::setXMLClobFromQuery bufferSize["<<j<<"] =" << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
      
      if ((xmlClobArray_[j]).isNull()) {
#ifdef DATABASEDEBUG
      	std::cerr << "ESDbAccess::setXMLClobFromQuery xmlClob_ attribute is not initialized" << std::endl;
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
 
    if (newPartition) {
      stmt->setUInt (++i, 1);
    } 
    else {
      stmt->setUInt (++i, 0);
    }
    stmt->execute();
    versionMajorId = stmt->getUInt(1);
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return versionMajorId;
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get the current run number.<BR>
 * @return the current run number
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement.
 * \warning to retreive the next run number, retreive the current run number and add 1 to it
 */
unsigned int ESDbAccess::getCurrentRunNumber() throw (FecExceptionHandler) {
  unsigned int nextRunId = 0;
  static std::string queryString = "BEGIN :a := PkgRun.GetCurrentRun(); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(nextRunId));
    if (stmt->executeUpdate() == 1) {
      nextRunId = stmt->getUInt(1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): unable to get the current run" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &ex) {
    dbConnection_->terminateStatement(stmt);
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, queryString + ": " + std::string(ex.what()), ERRORCODE) ;
  }

  return nextRunId;
}

void ESDbAccess::traceSql(boolean start){
		if (getDbPath() == "int2r_nolb" || getDbPath() == "int2r"){
			
			static std::string sqlQuery = "alter session set tracefile_identifier=\'CMS_PRESHOWER_HUNG_test\'";
			std::cout << sqlQuery << std::endl;
			oracle::occi::Statement* stmt1 = NULL;
    
    	try {
      	stmt1 = dbConnection_->createStatement(sqlQuery);
      	stmt1->execute();
      	dbConnection_->commit();
      	dbConnection_->terminateStatement (stmt1);
    	} catch (oracle::occi::SQLException &ex) {
      	dbConnection_->terminateStatement (stmt1);
      	std::cout << "unable to alter the session" << ex.what() << std::endl;
    	}
    	
    	sqlQuery = "BEGIN dbms_monitor.SESSION_TRACE_ENABLE; END;";
    	oracle::occi::Statement* stmt2 = NULL;
    	
    	std::cout << sqlQuery << std::endl;
    	try {
    		dbConnection_->createStatement(sqlQuery);
    		stmt2->executeUpdate();
    		dbConnection_->commit();
    		dbConnection_->terminateStatement(stmt2);
    	} catch (oracle::occi::SQLException &ex){
    		dbConnection_->terminateStatement(stmt2);
    		std::cout << "unable to alter the session" << ex.what() << std::endl;
    	}
  	}
}

