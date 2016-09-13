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
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "DbAccess.h"

//#define DATABASEDEBUG

/**Create a database access. <BR>
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * Retreive the connection parameters <I>user</I>, <I>password</I> and <I>database path</I> from the CONFDB environment variable, using <I>DbAccess::getDbConfiguration</I> method. <BR>
 * Connect to the database with the <I>DbAccess::DbConnect method</I>. <BR>
 * Instanciate the <I>xmlClob_</I> attribute with the default <I>oracle::occi::Clob</I> constructor. <BR>
 * Initialize the attributes <I>user_</I>, <I>passwd_</I> and <I>path_</I>. <BR>
 * @exception oracle::occi::SQLException thrown by <I>DbAccess::DbConnect</I>
 * @see <I>DbAccess::getDbConfiguration</I>, <I>DbAccess::DbConnect</I>
 */
DbAccess::DbAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbCommonAccess (threaded) {
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
DbAccess::DbAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbCommonAccess (user, passwd, dbPath, threaded) {
}

/**Delete the database access. <BR>
* Deletes the <I>xmlClob_</I> object.<BR>
 * Closes the connection with the database.<BR>
 * Closes the environnement.<BR>
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in closing the environnement or the connection
 * */
DbAccess::~DbAccess () throw (oracle::occi::SQLException) { }

/**Get the partition name from partition identifier. <BR>
 * A query is defined to get the partition name from the partition identifier. <BR>
 * A statement is created and executed from this query string. <BR>
 * The result is the partition name. <BR> 
 * @param partitionId - the partition identifier
 * @return the corresponding partition name
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::string DbAccess::getPartitionName (unsigned int partitionId)  throw (oracle::occi::SQLException) {

  std::string partitionName;
  static std::string queryString = "BEGIN :a := PkgPartition.getPartitionName(:b);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
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

/**Get the partition ID from partition name. <BR>
 * A query is defined to get the partition ID from the partition name. <BR>
 * A statement is created and executed from this query string. <BR>
 * The result is the partition ID. <BR> 
 * @param partitionName - the partition name
 * @return the corresponding partition name
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
unsigned int DbAccess::getPartitionId (std::string partitionName)  throw (oracle::occi::SQLException){

  unsigned int partitionId;
  static std::string queryString = "BEGIN :a := PkgPartition.getPartitionId(:b);END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(partitionId));
    stmt->setString(2, partitionName);
    stmt->executeUpdate();
    partitionId = stmt->getInt(1);
    dbConnection_->terminateStatement(stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return partitionId;
}


/**Get a version number. <BR>
 * The parameter <I>sqlQuery</I> is defined, according the needed version number (Fec, Fed, PiaReset). <BR>   
 * A statement is created and executed from this query string. <BR>
 * The result is a version number. <BR> 
 * @param sqlQuery - request used to define the statement
 * @return a version number
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement.
 */
unsigned int DbAccess::getVersion(std::string sqlQuery) throw (oracle::occi::SQLException) {

  int versionNumber = -1;
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL; 

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
oracle::occi::Clob *DbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName ) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": readString " << readString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName " << partitionName << std::endl;
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
 * @param maskVersionMajor - mask major version
 * @param maskVersionMinor - mask minor version
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
oracle::occi::Clob *DbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor ) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": readString " << readString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName " << partitionName << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMajorId " << versionMajorId << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMinorId " << versionMinorId << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": maskVersionMajor " << maskVersionMajor << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": maskVersionMinor " << maskVersionMinor << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    stmt->setUInt(++i,versionMajorId);
    stmt->setUInt(++i,versionMinorId);
    stmt->setUInt(++i,maskVersionMajor);
    stmt->setUInt(++i,maskVersionMinor);
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
oracle::occi::Clob *DbAccess::getXMLClobFromQueryWithoutMask(std::string readString, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId ) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": readString " << readString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName " << partitionName << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMajorId " << versionMajorId << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMinorId " << versionMinorId << std::endl;
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
oracle::occi::Clob *DbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName, std::string id) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": readString " << readString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName " << partitionName << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": id " << id << std::endl;
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
oracle::occi::Clob *DbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName, std::string id, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": readString " << readString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName " << partitionName << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": id " << id << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMajorId " << versionMajorId << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMinorId " << versionMinorId << std::endl;
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
oracle::occi::Clob *DbAccess::getXMLClobFromQuery(std::string readString) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": readString " << readString << std::endl;
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
oracle::occi::Clob *DbAccess::getXMLClobFromQuery(std::string readString, std::string partitionName, std::string fecHardId, unsigned int ringSlot) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": readString " << readString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName " << partitionName << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": fecHardId " << fecHardId << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": ring " << ringSlot << std::endl;
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
void DbAccess::setXMLClobFromQuery(std::string writeString, std::string buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException, FecExceptionHandler) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": writeString : " << writeString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": buffer : " << buffer << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName : " << partitionName << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMajorId : " << versionMajorId << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMinorId : " << versionMinorId << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    
    if (buffer.size()) {
      bufferSize = buffer.size();
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": bufferSize = "  << bufferSize << std::endl;
#endif

      unsigned int i = 0;  
      prepareXmlClob();

#ifdef DATABASEDEBUG
std::cerr << __PRETTY_FUNCTION__ << ": write into xmlClob_" << std::endl;
#endif

      xmlClob_->write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
      
      stmt->setClob (++i, (*xmlClob_));
      stmt->setString (++i, partitionName);
      stmt->setUInt (++i, versionMajorId);
      stmt->setUInt (++i, versionMinorId);

#ifdef DATABASEDEBUG      
      std::cerr << __PRETTY_FUNCTION__ << ": execute statement" << std::endl;
#endif

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
void DbAccess::setXMLClobFromQuery(std::string writeString, std::string buffer, std::string partitionName, int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": buffer : " << buffer << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": writeString : " << writeString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName : " << partitionName << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": versionMajorUpdate : " << versionUpdate << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": xml clob null ?: " << xmlClob_->isNull() << std::endl ;
#endif

  oracle::occi::Statement *stmt = NULL ;

  try {

    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;

    if (buffer.size()) {
      bufferSize = buffer.size();
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": bufferSize = "  << bufferSize << std::endl;
#endif

      unsigned int i = 0;  
      prepareXmlClob();

#ifdef DATABASEDEBUG
std::cerr << __PRETTY_FUNCTION__ << ": write into xmlClob_" << std::endl;
#endif
      xmlClob_->write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);

      stmt->setClob (++i, (*xmlClob_));
      stmt->setString (++i, partitionName);
      stmt->setUInt (++i, versionUpdate);
      
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": ready to execute..."<< std::endl;
#endif

      stmt->executeUpdate();

      dbConnection_->commit();
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": statement executed..."<< std::endl;
#endif
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

/**Sends a request to the database to execute a PL/SQL stored procedure in order to put a Clob containing the data to the database.<BR>
 * A statement is created with the <I>writeString</I> parameter and the parameters are stored in the partition <I>partitionName</I><BR>
 * @param writeString - PL/SQL request
 * @param buffer - buffer containing the data to be uploaded
 * @param partitionName - partition name
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the buffer is empty
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
unsigned int DbAccess::setXMLClobFromQuery(std::string writeString, std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": writeString : " << writeString << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": buffer : " << buffer << std::endl;
  std::cerr << __PRETTY_FUNCTION__ << ": partitionName : " << partitionName << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;
  unsigned int versionMajorId = 0;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    
    if (buffer.size()) {
      bufferSize = buffer.size();
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": bufferSize =" << buffer.size() << " = " << bufferSize << std::endl;
#endif
      unsigned int i = 0;  
      prepareXmlClob();

#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": write into xmlClob_" << std::endl;
#endif
      xmlClob_->write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
      stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMajorId));
      stmt->setClob (++i, (*xmlClob_));
      stmt->setString (++i, partitionName);

#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": ready to execute..."<< std::endl;
#endif

      stmt->execute();
      versionMajorId = stmt->getUInt(1);
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
unsigned int DbAccess::setXMLClobFromQuery(std::string writeString, std::string *buffer, std::string partitionName, bool newPartition) throw (oracle::occi::SQLException, FecExceptionHandler) {

#ifdef DATABASEDEBUG
    std::cerr << __PRETTY_FUNCTION__ << ": writeString : " << writeString << std::endl;
    std::cerr << __PRETTY_FUNCTION__ << ": partitionName : " << partitionName << std::endl;
    std::cerr << __PRETTY_FUNCTION__ << ": newPartition : "<< newPartition << std::endl;
#endif

  unsigned int versionMajorId = 0;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  
    unsigned int j = 0;  

    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMajorId));

    for (j=0; j<5; j++) {
      
      bufferSize = buffer[j].size();
      
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": buffer["<<j<<"] : " << buffer[j] << std::endl;
      std::cerr << __PRETTY_FUNCTION__ << ": bufferSize["<<j<<"] = " << bufferSize << std::endl;
#endif
      
      prepareXmlClobArray(j);
      
      (xmlClobArray_[j]).write(bufferSize, (unsigned char*)buffer[j].c_str(), bufferSize);
      stmt->setClob (++i, (xmlClobArray_[j]));
    }
    
    stmt->setString (++i, partitionName);
    if (newPartition) {
      stmt->setUInt (++i, 1);
    } else {
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

/**
 */
void DbAccess::traceSql(boolean start) {

  if (getDbPath() == "int2r_nolb" || getDbPath() == "int2r") {

//   std::string sqlQuery;
//   if (start) {
//     sqlQuery = "alter session set sql_trace=true";
//   } else {
//     sqlQuery = "alter session set sql_trace=false";
//   }
//   std::cout << sqlQuery << std::endl;
//   try {
//     oracle::occi::Statement* stmt = dbConnection_->createStatement(sqlQuery);
//     stmt->execute();
//     dbConnection_->commit();
//     dbConnection_->terminateStatement (stmt);
//   } catch (oracle::occi::SQLException &ex){
//     std::cout << "unable to alter the session" << ex.what() << std::endl;
//   }



    static std::string sqlQuery = "alter session set tracefile_identifier=\'CMS_TRACKER_FRED_test1\'";
    std::cout << sqlQuery << std::endl;
    oracle::occi::Statement* stmt1 = NULL ;

    try {
      stmt1 = dbConnection_->createStatement(sqlQuery);
      stmt1->execute();
      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt1);
    } catch (oracle::occi::SQLException &ex){
      dbConnection_->terminateStatement(stmt1);  
      std::cout << "unable to alter the session" << ex.what() << std::endl;
    }
    
    sqlQuery="BEGIN dbms_monitor.SESSION_TRACE_ENABLE; END;";
    oracle::occi::Statement* stmt2 = NULL ;

    std::cout << sqlQuery << std::endl;
    try {
      dbConnection_->createStatement(sqlQuery);
      stmt2->executeUpdate();
      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt2);
    } catch (oracle::occi::SQLException &ex){
      dbConnection_->terminateStatement(stmt2);  
      std::cout << "unable to alter the session" << ex.what() << std::endl;
    }
  }
}

