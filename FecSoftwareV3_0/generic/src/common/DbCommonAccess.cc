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

// #define DATABASEDEBUG

#include "DbCommonAccess.h"

#include <xercesc/framework/URLInputSource.hpp>
#include "stringConv.h"

/**Create a database access. <BR>
 * Retreive the connection parameters <I>user</I>, <I>password</I> and <I>database path</I> from the CONFDB environment variable, using <I>DbCommonAccess::getDbConfiguration</I> method. <BR>
 * Connect to the database with the <I>DbCommonAccess::dbConnect method</I>. <BR>
 * Instanciate the <I>xmlClob_</I> attribute with the default <I>oracle::occi::Clob</I> constructor. <BR>
 * Initialize the attributes <I>user_</I>, <I>passwd_</I> and <I>path_</I>. <BR>
 * \exception oracle::occi::SQLException thrown by <I>DbCommonAccess::dbConnect</I>
 * \see <I>DbCommonAccess::getDbConfiguration</I>, <I>DbCommonAccess::dbConnect</I>
 * \param threaded - if the process is thread then a connection is created for these process. In the other case then the process used a static variables
 */
DbCommonAccess::DbCommonAccess ( bool threaded ) throw (oracle::occi::SQLException) {
  // Try to find the access through environment variables
  std::string login = "nil" ;
  std::string passwd = "nil" ;
  std::string dbPath = "nil" ;
  DbCommonAccess::getDbConfiguration (login, passwd, dbPath) ;

  dbConnect (login, passwd, dbPath, threaded) ;
  xmlClob_ = new oracle::occi::Clob();
  xmlClobArray_ = new oracle::occi::Clob[8];
  
  user_ = login ; passwd_ = passwd ; path_ = dbPath ;
}

/**Create a database access. <BR>
 * Connection parameters user, password and database path are given as constructor parameters. <BR>
 * Connect to the database with the DbCommonAccess::dbConnect method. <BR>
 * Instanciate the <I>xmlClob_</I> attribute with the default <I>oracle::occi::Clob</I> constructor. <BR>
 * Initialize the attributes <I>user_</I>, <I>passwd_</I> and <I>path_</I>. <BR>
 * \param user - username
 * \param passwd - password
 * \param dbPath - database path
 * \exception oracle::occi::SQLException thrown by DbCommonAccess::dbConnect
 * \see DbCommonAccess::dbConnect
 */
DbCommonAccess::DbCommonAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) {
  dbConnect (user, passwd, dbPath, threaded) ;
  xmlClob_ = new oracle::occi::Clob();
  xmlClobArray_ = new oracle::occi::Clob[8];
  user_ = user ; passwd_ = passwd ; path_ = dbPath ;
}

/**Connect to the database. <BR>
 * Initializes the <I>dbEnvironment_</I> and <I>dbConnection_</I> attributes :
 * - <I>dbEnvironment_</I> is the OCCI environment used for our application.<BR>
 * - <I>dbConnection_</I> enables users to connect to the database.<BR>
 * \param user - login for the database
 * \param passwd - password for the database
 * \param dbPath - path for the database
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating the environnement or the connection
 */
void DbCommonAccess::dbConnect (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException){

  dbEnvironment_ = NULL ; xmlClob_ = NULL ; xmlClobArray_ = NULL ;

  try {
    if (threaded) {

      useLocalConnection_ = true ;

      //Mode valid values are:
      //DEFAULT not thread safe, not in object mode
      //THREADED_MUTEXED thread safe, mutexed internally by OCCI
      //THREADED_UN-MUTEXTED thread safe, client responsible for mutexing
      //OBJECT uses object features
      if ((dbEnvironment_ = oracle::occi::Environment::createEnvironment (oracle::occi::Environment::THREADED_MUTEXED))) {

	//std::cout << "Create the dynamic variable for " << user << "@" << dbPath << ": " << std::hex << dbEnvironment_ << std::endl ;
	dbConnection_ = dbEnvironment_->createConnection(user, passwd, dbPath);
	dbConnection_->setStmtCacheSize(50);
      }
#ifdef DATABASEDEBUG
      else
	std::cerr << "DbCommonAccess::DbCommonAccess unable to create an OCCI environnement" << std::endl;
#endif
    }
    else {

      // If the creation of the connection is not done then create it
      if (dbEnvironmentStatic_ == NULL) {
	
	if ((dbEnvironmentStatic_ = oracle::occi::Environment::createEnvironment (oracle::occi::Environment::THREADED_MUTEXED))) {
	  dbConnectionStatic_ = dbEnvironmentStatic_->createConnection(user, passwd, dbPath);
	  dbConnectionStatic_->setStmtCacheSize(50);
	}
      }

      dbEnvironment_ = dbEnvironmentStatic_ ;
      dbConnection_ = dbConnectionStatic_ ;      
      connectionInUsed_ ++ ;
      useLocalConnection_ = false ;
    }

    //std::cout << "dbEnvironment = " << dbEnvironment_ << std::endl ;
    //std::cout << "dbConnection = " << dbConnection_ << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {

    dbEnvironment_ = NULL ; xmlClob_ = NULL ; xmlClobArray_ = NULL ;
    if (connectionInUsed_ == 0) dbEnvironmentStatic_ = NULL ;

    throw e ;
  }
}

/**Delete the database access. <BR>
 * Deletes the <I>xmlClob_</I> object.<BR>
 * Closes the connection with the database.<BR>
 * Closes the environnement.<BR>
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in closing the environnement or the connection
 * */
DbCommonAccess::~DbCommonAccess () throw (oracle::occi::SQLException) {

  if (xmlClob_) { delete xmlClob_; xmlClob_ = NULL ; }
  if (xmlClobArray_) { delete[] xmlClobArray_;  xmlClobArray_ = NULL ; }
  if (dbConnection_ != NULL) {
    //dbConnection_->flushCache();
    dbConnection_->commit();
  }

  // Local env or global env ?
  if (useLocalConnection_) {

    if (dbEnvironment_ != NULL) {
      //std::cout << "Delete the dynamic variable for " << std::hex << dbEnvironment_ << std::endl ;
      if (dbConnection_ != NULL) dbEnvironment_->terminateConnection(dbConnection_);
      oracle::occi::Environment::terminateEnvironment(dbEnvironment_);
    }

    // Not needed
    //delete dbEnvironment_ ;
    //delete dbConnection_ ;
  }
  else {

    // One connection less
    connectionInUsed_ -- ;

    // No more used
    if (connectionInUsed_ == 0) {

      if (dbEnvironmentStatic_ != NULL) {
	if (dbConnectionStatic_ != NULL) dbEnvironmentStatic_->terminateConnection(dbConnectionStatic_);
	oracle::occi::Environment::terminateEnvironment(dbEnvironmentStatic_);
      }

      // Not needed
      //delete dbEnvironmentStatic_ ;
      //delete dbConnectionStatic_ ;
      dbEnvironmentStatic_ = NULL ;
      dbConnectionStatic_  = NULL ;
    }
  }

  // Not anymore used by this session
  dbEnvironment_ = NULL ;
  dbConnection_  = NULL ;
}


/** Initialise the XML clob if needed, if an exception during the intialisation of the clob is raised then
 * delete the previous clob and reinitialise it
 */
void DbCommonAccess::prepareXmlClob() {
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::prepareXmlClob @@BELS: prepare xmlClob_" << std::endl;
#endif
  try {
    if ( xmlClob_->isNull() ) {
      *xmlClob_ = initializeClob();
    } else {
      xmlClob_->trim(0);
    }
  }
  catch (oracle::occi::SQLException &e) {
#ifdef DATABASEDEBUG
    std::cerr << "DbCommonAccess::prepareXmlClob @@BELS: prepare xmlClob_ " << e.what() << std::endl;
#endif
    delete xmlClob_; xmlClob_ = NULL ;
    xmlClob_ = new oracle::occi::Clob();
    *xmlClob_ = initializeClob();
  }
}

/** Initialise the XML clob if needed, if an exception during the intialisation of the clob is raised then
 * delete the previous clob and reinitialise it
 * \param index - index in the array of XML clob
 */
void DbCommonAccess::prepareXmlClobArray( int index ) {

#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::prepareXmlClob @@BELS: prepare prepareXmlClobArray[" << index << "]" << std::endl;
#endif

  try {
    if ((xmlClobArray_[index]).isNull()) {
      xmlClobArray_[index] = initializeClob();
    } else {
      (xmlClobArray_[index]).trim(0);
    }
  }
  catch (oracle::occi::SQLException &e) {
    xmlClobArray_[index] = initializeClob();
  }	
}



/**Initialize a Clob <BR>
 * <I>oracle::occi::Clob</I> has to be initialized before writing data. <BR> 
 * A dummy query is defined and a statement is executed from this dummy query, to get a temporary Clob from database. <BR>
 * \return a pointer to a initialized Clob
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in statement creation or statement execution
 */
oracle::occi::Clob DbCommonAccess::initializeClob () throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::getXMLClob initialize a Clob" << std::endl;
#endif

  static std::string dummyQuery = "BEGIN DBMS_LOB.CREATETEMPORARY(:1, TRUE, DBMS_LOB.CALL);END;";
  //DBMS_LOB.createTemporary(lob_locator, cache, duration);
  //duration == 2 the temporary LOB is cleaned up at the end of the call
  //duration == 1 the temporary LOB is cleaned up at the end of the session 

#ifdef DATABASEDEBUG
  std::cerr << "Initialiaze clob: " << dummyQuery << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(dummyQuery);
    stmt->registerOutParam(1,oracle::occi::OCCICLOB,20000000,"");

#ifdef DATABASEDEBUG
    int returnedVal = 
#endif
      stmt->executeUpdate();
    
#ifdef DATABASEDEBUG
    std::cerr << "DbAccess::getXMLClob => executeUpdate returns: " << returnedVal << std::endl;
#endif      

    *xmlClob_ = stmt->getClob(1);
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
#ifdef DATABASEDEBUG
    std::cout << "BELS. TKCC -> DbCommonAccess::initializeClob() throws an exception:" << e.what() << std::endl;
#endif
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return *xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get the current run number.<BR>
 * @return the current run number
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement.
 * \warning to retreive the next run number, retreive the current run number and add 1 to it
 */
unsigned int DbCommonAccess::getCurrentRunNumber() throw (FecExceptionHandler) {

  static std::string queryString = "BEGIN :a := PkgRun.GetCurrentRun(); END;";
  unsigned int nextRunId = 0;
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
  catch (oracle::occi::SQLException &ex){
    dbConnection_->terminateStatement(stmt);
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DbCommonAccess::what(queryString, ex), ERRORCODE) ;
  }

  return nextRunId;
}

/**Get the current version. <BR>
 * The parameter <I>sqlQuery</I> is defined, according the needed version (Fec, Fed, det id, etc.). <BR>   
 * A statement is created and executed from this query string. <BR>
 * The result is a list of partition identifiers and corresponding version numbers. <BR> 
 * @param sqlQuery - request used to define the statement
 * @param partitionName - partition name
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId, maskVersionMajorId, maskVersionMinorId : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::list<unsigned int*> DbCommonAccess::getDatabaseVersionFromQuery (std::string sqlQuery, std::string partitionName) throw (oracle::occi::SQLException) {

  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 
  std::list<unsigned int*> partitionVersionsListe ;

  try {
    unsigned int i = 0 ;
    stmt = dbConnection_->createStatement (sqlQuery);
    stmt->setString(++i, partitionName);
    rset = stmt->executeQuery ();

    while (rset->next ()) {
      unsigned int *partitionVersions = new unsigned int[5] ;
      partitionVersions[0] = (unsigned int)rset->getUInt (1);
      partitionVersions[1] = (unsigned int)rset->getUInt (2);
      partitionVersions[2] = (unsigned int)rset->getUInt (3);
      partitionVersions[3] = (unsigned int)rset->getUInt (4);
      partitionVersions[4] = (unsigned int)rset->getUInt (5);
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


/**Create a new state <BR>
 * \return a pointer to a initialized Clob
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in statement creation or statement execution
 */
void DbCommonAccess::createStateHistory (unsigned int stateHistoryId, std::string stateHistoryName, std::string partitionName, 
					 unsigned int fecVersionMajorId, unsigned int fecVersionMinorId, 
					 unsigned int fedVersionMajorId, unsigned int fedVersionMinorId, 
					 unsigned int connectionVersionMajorId, unsigned int connectionVersionMinorId,
					 unsigned int dcuInfoVersionMajorId, unsigned int dcuInfoVersionMinorId, 
					 unsigned int dcuPsuMapVersionMajorId, unsigned int dcuPsuMapVersionMinorId,
					 unsigned int maskVersionMajorId, unsigned int maskVersionMinorId ) throw (oracle::occi::SQLException) {
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::createState(...)" << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;
  std::string sqlQuery;
  unsigned int partitionId;
  static std::string queryString = "BEGIN :a := PkgPartition.getPartitionId(:partitionName);END;";

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(partitionId));
    stmt->setString(2, partitionName);
    if (stmt->executeUpdate() >= 1) {
      partitionId = stmt->getInt(1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to retreive the partition ID for partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  try {
    sqlQuery = "INSERT INTO STATEHISTORY (statehistoryid, statehistoryname, partitionId, fecVersionMajorId, fecVersionMinorId, fedVersionMajorId, fedVersionMinorId, connectionVersionMajorId, connectionVersionMinorId, dcuInfoVersionMajorId, dcuInfoVersionMinorId, dcuPsuMapVersionMajorId, dcuPsuMapVersionMinorId, maskVersionMajorId, maskVersionMinorId) VALUES (:paramStateHistoryId, :paramStateHistoryName, :paramPartitionId, :paramFecVersionMajorId, :paramFecVersionMinorId, :paramFedVersionMajorId, :paramFedVersionMinorId, :paramConnectionVersionMajorId, :paramConnectionVersionMinorId, :paramDcuInfoVersionMajorId, :paramDcuInfoVersionMinorId, :paramDcuPsuMapVersionMajorId, :paramdcuPsuMapVersionMinorId, :parammaskVersionMajorId, :paramMaskVersionMinorId)";
    
    int i = 0;
    stmt = dbConnection_->createStatement (sqlQuery);
    stmt->setInt(++i, stateHistoryId);
    stmt->setString(++i, stateHistoryName);
    stmt->setInt(++i, partitionId);
    stmt->setInt(++i, fecVersionMajorId);
    stmt->setInt(++i, fecVersionMinorId);
    stmt->setInt(++i, fedVersionMajorId);
    stmt->setInt(++i, fedVersionMinorId);
    stmt->setInt(++i, connectionVersionMajorId);
    stmt->setInt(++i, connectionVersionMinorId);
    stmt->setInt(++i, dcuInfoVersionMajorId);
    stmt->setInt(++i, dcuInfoVersionMinorId);
    stmt->setInt(++i, dcuPsuMapVersionMajorId);
    stmt->setInt(++i, dcuPsuMapVersionMinorId);
    stmt->setInt(++i, maskVersionMajorId);
    stmt->setInt(++i, maskVersionMinorId);
    //stmt->setNull(++i, oracle::occi::OCCIINT);
    //stmt->setNull(++i, oracle::occi::OCCIINT);
    
    stmt->executeUpdate();
    dbConnection_->terminateStatement(stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}

/**Create a new run <BR>
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in statement creation or statement execution
 */
void DbCommonAccess::setRun(std::string partitionName, unsigned int runNumber, int runMode, int local, std::string comment) throw (oracle::occi::SQLException, FecExceptionHandler){
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::setRun(" << partitionName << "," << runNumber << ", "<< runMode <<")"<<std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;
  static std::string queryString = "";

  if(comment.compare("")==0)
    queryString = "BEGIN PkgRun.setRun(:partitionName, :runNumber, :runMode, :local);END;";
  else{
    queryString = "BEGIN PkgRun.setRun(:partitionName, :runNumber, :runMode, :local, :comment);END;";
  }

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->setString(1, partitionName);
    stmt->setUInt(2, runNumber);
    stmt->setUInt(3, runMode);
    stmt->setUInt(4, local);
    if(!comment.compare("")==0)
      stmt->setString(5, comment);
    if (stmt->executeUpdate() >= 1) {
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to create a new run for partition " << partitionName << " and run " << runNumber ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/** \brief Stop a run 
   */
void DbCommonAccess::stopRun(std::string partitionName, std::string comment) throw (oracle::occi::SQLException,FecExceptionHandler){
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::stopRun(" << partitionName << ")"<<std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;
  static std::string queryString = "";

  if(comment.compare("")==0)
    queryString = "BEGIN PkgRun.stopRun(:partitionName);END;";
  else{
    queryString = "BEGIN PkgRun.stopRun(:partitionName, :comment);END;";
  }
  
  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->setString(1, partitionName);
    if(!comment.compare("")==0)
      stmt->setString(2, comment);
    if (stmt->executeUpdate() >= 1) {
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to set the end of run for partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/**Tag the run as transfered by O2O <BR>
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in statement creation or statement execution
 */
void DbCommonAccess::setO2ORun(std::string partitionName, unsigned int runNumber) throw (oracle::occi::SQLException, FecExceptionHandler){
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::setO2ORun(" << partitionName << "," << runNumber << ")"<<std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;
  static std::string queryString = "BEGIN PkgRun.tagRunO2O(:partitionName, :runNumber);END;";

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->setString(1, partitionName);
    stmt->setUInt(2, runNumber);
    if (stmt->executeUpdate() >= 1) {
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to set the o2o information for partition " << partitionName << " and run " << runNumber ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/** \brief Change the comment of the Run
 * \param partitionName The partition name
 * \param runNumber The number of the run
 * \param newComment The new value of the comment
 */
void DbCommonAccess::setNewComment(std::string partitionName, unsigned int runNumber, std::string newComment) throw (oracle::occi::SQLException, FecExceptionHandler){
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::setNewComment(" << partitionName << "," << runNumber << "," << newComment << " )"<<std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;  
  static std::string queryString = "BEGIN PkgRun.updateRunComment(:partitionName, :runNumber, :runComment);END;";
  
  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->setString(1, partitionName);
    stmt->setUInt(2, runNumber);
    stmt->setString(3, newComment);
    if (stmt->executeUpdate() >= 1) {
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to update the comment for partition " << partitionName << " and run " << runNumber ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/** \brief Retreive the database version
 * \return version of the database as a double
 */
double DbCommonAccess::getDbVersion( ) throw (oracle::occi::SQLException, FecExceptionHandler){

  static std::string query = "select version from dbversion" ;

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": " << query << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ;
  double id;

  try {
    stmt =  dbConnection_->createStatement (query);
    rset = stmt->executeQuery ();
    if (rset!=NULL && rset->next ()){
      id = (double)rset->getDouble (1);
    }
    else{
      stmt->closeResultSet (rset);
      dbConnection_->terminateStatement (stmt);
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << query << "): " << "unable to retreive the DB model version" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }

    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return id;
}

/** \brief Retreive the database size
 * \return the database size in Mbytes
 */
double DbCommonAccess::getDbSize( ) throw (oracle::occi::SQLException, FecExceptionHandler){

  static std::string query = "select sum(bytes)/1024/1024 as space_used_in_MB from user_extents" ;

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": " << query << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ;
  double id;

  try {
    stmt =  dbConnection_->createStatement (query);
    rset = stmt->executeQuery ();
    if (rset!=NULL && rset->next ()){
      id = (double)rset->getDouble (1);
    }
    else{
      stmt->closeResultSet (rset);
      dbConnection_->terminateStatement (stmt);
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << query << "): " << "unable to retreive the DB size" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }

    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return id;
}


/** \brief Get the state ID from the current state
 *  \return The ID of the given state   
 */
unsigned int DbCommonAccess::getCurrentStateHistoryId( ) throw (oracle::occi::SQLException, FecExceptionHandler){

  static std::string query = "" ;
  query = "select stateHistoryId from CurrentState";
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 
  unsigned int id = 0 ;

  try {
    stmt = dbConnection_->createStatement (query);
    rset = stmt->executeQuery ();
    if (rset!=NULL && rset->next ()){
      id = (unsigned int)rset->getUInt (1);
    }
    else{
      stmt->closeResultSet (rset);
      dbConnection_->terminateStatement (stmt);
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << query << "): " << "unable the current state history" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }

    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return id;
}

/**Get a new state ID from the sequence<BR>
 * \return a pointer to a initialized Clob
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in statement creation or statement execution
 */
unsigned int DbCommonAccess::getNewStateHistoryId () throw (oracle::occi::SQLException) {
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::getNewStateHistoryId()" << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;
  unsigned int stateHistoryId = 0 ;
  static std::string queryString = "BEGIN :a := PkgStateHistory.getNewStateHistoryId;END;";  

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(stateHistoryId));

    stmt->executeQuery() ;
    stateHistoryId = stmt->getInt(1);
    dbConnection_->terminateStatement(stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return stateHistoryId;
}

/** \brief Get the state ID from the state name
 *  \param stateName The state name
 *  \return The ID of the given state   
 */
unsigned int DbCommonAccess::getStateHistoryId(std::string stateHistoryName) throw (oracle::occi::SQLException, FecExceptionHandler){

  static std::string query = "" ;
  query = "select stateHistoryId from StateHistory where stateHistoryName='"+stateHistoryName+"'";
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 
  unsigned int id = 0 ;

  try {
    stmt = dbConnection_->createStatement (query);
    rset = stmt->executeQuery ();
    if (rset!=NULL && rset->next ()){
      id = (unsigned int)rset->getUInt (1);
    }
    else{
      stmt->closeResultSet (rset);
      dbConnection_->terminateStatement (stmt);
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << query << "): " << "unable to get the state history ID for the state history name " << stateHistoryName ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }

    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return id;
}

/** \brief Create a new version of dcu psu mapping 
   */
void DbCommonAccess::createNewDcuPsuMapVersion(unsigned int *majorVersion, unsigned int *minorVersion) throw (oracle::occi::SQLException,FecExceptionHandler){
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::createNewDcuPsuMapVersion(:major, :minor)"<<std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;
  static std::string queryString = "BEGIN PkgDcuPsuMap.createNewVersion(:major, :minor);END;";

  try {
    int major, minor;
  
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(major));
    stmt->registerOutParam(2,oracle::occi::OCCIINT,sizeof(minor));
    //  stmt->setUInt(1, major);
    //  stmt->setUInt(2, minor);
    if (stmt->executeUpdate() >= 1) {
      major = stmt->getInt(1);
      minor = stmt->getInt(2);
      dbConnection_->terminateStatement(stmt);      
      *majorVersion = major;
      *minorVersion = minor;
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to create a new version of DCU/PSU map" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}

/** \brief Create a new Version of dcu infos 
   */
void DbCommonAccess::createNewDcuInfoVersion() throw (oracle::occi::SQLException,FecExceptionHandler){
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::createNewDcuInfoVersion(:major, :minor)"<<std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  static std::string queryString = "" ;

  try {
    int majorVersion=0;
    int minorVersion=0;
    
    queryString = "BEGIN PkgDcuInfo.createNewVersion(:major, :minor);END;";
    
    stmt = dbConnection_->createStatement(queryString);
    stmt->setUInt(1, majorVersion);
    stmt->setUInt(2, minorVersion);
    if (stmt->executeUpdate() >= 1) {
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to create a new version of DCU info" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/** 
    \brief Enable the devices corresponding to the dcuHardId in the given partition
    \param partitionName The name of the concerned partition
    \param dcuHardId The list of the dcuHardId to change
**/
void DbCommonAccess::setDeviceState(std::string partitionName, std::vector<unsigned int> dcuHardId, int newState) throw (oracle::occi::SQLException){
  oracle::occi::Statement *stmt = NULL ;
  static std::string sqlQuery = "BEGIN PkgDcu.setState(:partitionName, :state);END;";

  try {

#define DISPLAYTEMPS
#ifdef DISPLAYTEMPS
    unsigned int i = 0 ;
    unsigned long endMillis, startMillis ;
    unsigned long totalStart =  XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
    
#ifdef DISPLAYTEMPS
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
#endif
    // We record all the dcu_ids in the table dcuid
    for (std::vector<unsigned int>::iterator it=dcuHardId.begin() ; it!=dcuHardId.end() ; it++ ) {
      
      stmt = dbConnection_->createStatement("INSERT INTO RAWDCUID VALUES (:paramDcuId)");
      stmt->setAutoCommit(false);
      stmt->setUInt(1, (unsigned int)(*it));
      stmt->executeUpdate();
      dbConnection_->terminateStatement(stmt);
      i++;
    }
    
#ifdef DISPLAYTEMPS
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Insertion of the DCU_IDs in the table " << std::dec << i << " took " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

    // Set the state of all the dcuIds in the DCUID table (the table will be automatically cleaned at the end)
    stmt = dbConnection_->createStatement(sqlQuery);
    stmt->setString(1, partitionName);
    stmt->setUInt(2, newState);
    stmt->executeUpdate();
    dbConnection_->terminateStatement(stmt);

#ifdef DISPLAYTEMPS
    unsigned long totalEnd =  XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Disable " << std::dec << i << " DCUs in " << (totalEnd-totalStart) << " ms" << std::endl ;
#endif
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

}

/** Copy the given state and set it as currentState
 */
unsigned int DbCommonAccess::setCurrentState(unsigned int stateHistoryId) throw (oracle::occi::SQLException){
#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::setCurrentState(" << stateHistoryId << ")"<<std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;
  static std::string queryString = "BEGIN :a := PkgStateHistory.copyState(:referenceState);END;";
  unsigned int newStateId = 0 ;

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(newStateId));
    stmt->setUInt(2, stateHistoryId);
    if (stmt->executeUpdate() >= 1) {
      newStateId = stmt->getInt(1);
      dbConnection_->terminateStatement(stmt);
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to set the current state ID " << stateHistoryId ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return newStateId;
}

/** Take the state given run number and update the current state with this information.
 * <br>Behaviour and example:
 * <ul>
 * <li>run 1000 with state history 100 has the following information:
 * <ul>
 *   <li> partition1, 7.0
 *   <li> partition4, 4.0
 * </ul>
 * <li>State history 100
 * <ul>
 *   <li> partition1, 7.0  << used in the run 1000
 *   <li> partition2, 2.0  << not used in the run 1000
 *   <li> partition4, 4.0  << used in the run 1000
 * </ul>
 * <li>Current state is:
 * <ul>
 *   <li> partition1, 1.0
 *   <li> partition2, 2.0
 *   <li> partition3, 3.0
 * </ul>
 * </ul>
 * In the case where you want to update the current with the information in the state history coming from the run (allPartition = false). If the partition is present in the current state then the version is updated, if the partition is not present in the current state then it is added. In the example, the current state result will be:
 * <ul>
 * <li> partition1, 7.0 << version updated
 * <li> partition2, 2.0 << no change
 * <li> partition3, 3.0 << no change
 * <li> partition4, 4.0 << partition added to the current state
 * </ul>
 * In the case you want to change the current state with the state in the run number (allPartition = true).In the example, you will find the state history from the run 1000 ie. state history 100, current state result is:
 * <ul>
 * <li> partition1, 7.0
 * <li> partition2, 2.0
 * <li> partition4, 4.0
 * </ul>
 * Please note here that the partition2 that was not in the run is anyway since we copy the run. This part can be modified if you want only the partition in the run.
 * \param runNumber - run number
 * \param allPartition - copy or update/insert of the state coming from the run number
 * \param return the new state ID created
 */
unsigned int DbCommonAccess::copyStateForRunNumber(unsigned int runNumber, bool allPartition) throw (oracle::occi::SQLException) {
#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": for the run " << runNumber ;
  if (allPartition) std::cerr << " for all partitions in the run (" << allPartition << ")" << std::endl ;
  else std::cerr << " for all partitions in the run (" << allPartition << ")" << std::endl ;
#endif
  oracle::occi::Statement *stmt = NULL ;
  static std::string queryString = "BEGIN :a := PkgStateHistory.copyStateForRunNumber(:runNumber, :allPartition);END;";
  unsigned int newStateId = 0 ;
  unsigned int allValuePartition = allPartition ? 1 : 0 ;

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(newStateId));
    stmt->setUInt(2, runNumber);
    stmt->setUInt(3, allValuePartition);
    if (stmt->executeUpdate() >= 1) {
      newStateId = stmt->getInt(1);
      dbConnection_->terminateStatement(stmt);
    }
    else {
      dbConnection_->terminateStatement(stmt);  
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to copy a state from a run for the number " << runNumber ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
   }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return newStateId;
}

/** Copy the current state, update the given partition with its values in the given state set the state as current
 */
unsigned int DbCommonAccess::setCurrentState(std::string partitionName, unsigned int stateHistoryId) throw (oracle::occi::SQLException){

#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::setCurrentState(" << partitionName << ", "<< stateHistoryId << ")"<<std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;
  unsigned int newStateId = 0 ;
  static std::string queryString = "BEGIN :a := PkgStateHistory.copyAndUpdateCurrentState(:partitionName, :referenceState);END;";
 
  try { 
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(newStateId));
    stmt->setString(2, partitionName);
    stmt->setUInt(3, stateHistoryId);
    if (stmt->executeUpdate() >= 1) {
      dbConnection_->terminateStatement(stmt); 
      newStateId = stmt->getInt(1);
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to set the current state for the partition " << partitionName << " and state ID " << stateHistoryId ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return newStateId;
}


// /** 
//     \brief Enable the devices corresponding to the dcuHardId in the given partition
//     \param partitionName The name of the concerned partition
//     \param dcuHardId The list of the dcuHardId to change
// **/
// void DbCommonAccess::setDeviceState(std::string partitionName, std::vector<unsigned int> dcuHardId, int newState) throw (oracle::occi::SQLException){
//   oracle::occi::Statement *stmt;
//   static std::string sqlQuery = "BEGIN PkgDcu.setDevicesState(:partitionName, :dcuHardId, :state);END;";

// #define DISPLAYTEMPS
// #ifdef DISPLAYTEMPS
//   unsigned int i = 0 ;
//   unsigned long endMillis, startMillis ;
//   unsigned long totalStart =  XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// #endif

//   for (std::vector<unsigned int>::iterator it=dcuHardId.begin() ; it!=dcuHardId.end() ; it++ ) {

// #ifdef DISPLAYTEMPS
//     startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// #endif

//     stmt = dbConnection_->createStatement(sqlQuery);
//     stmt->setAutoCommit(false);
//     stmt->setString(1, partitionName);
//     stmt->setUInt(2, (unsigned int)(*it));
//     stmt->setUInt(3, newState);
//     stmt->executeUpdate();
//     dbConnection_->terminateStatement(stmt);

// #ifdef DISPLAYTEMPS
//     endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//     std::cout << "DCU " << std::dec << ++i << " took " << (endMillis-startMillis) << " ms" << std::endl ;
// #endif
//   }

// #ifdef DISPLAYTEMPS
//   unsigned long totalEnd =  XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//   std::cout << "Disable " << std::dec << i << " DCUs in " << (totalEnd-totalStart) << " ms" << std::endl ;
// #endif

//   dbConnection_->commit();

//   // Refresh the MVs
//   sqlQuery = "BEGIN PkgFecXML.refreshAllMV();END;";
//   stmt = dbConnection_->createStatement(sqlQuery);
//   stmt->setAutoCommit(true);
//   stmt->executeUpdate();
//   dbConnection_->terminateStatement(stmt);

// }

/**Get the partition names. <BR>
 * The results are stored in a list. <BR> 
 * @return The list containing the partition names - YOU have to delete that list!!!!!
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::list<std::string> DbCommonAccess::getAllPartitionNames ()  throw (oracle::occi::SQLException){

  std::list<std::string> partitionNameList ;
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 

  try {
    stmt = dbConnection_->createStatement ("select partitionName from partition order by partitionName");
    rset = stmt->executeQuery ();
    while (rset->next ()) {
      std::string name = (std::string)rset->getString (1);
      partitionNameList.push_back(name);
    }
    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);

    partitionNameList.sort() ;
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return partitionNameList;
}

/**Get the partition names. <BR>
 * The results are stored in a list. <BR> 
 * @return The list containing the partition names
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::list<std::string> DbCommonAccess::getAllPartitionNames ( unsigned int runNumber )  throw (oracle::occi::SQLException) {

  std::list<std::string> partitionNameList ;
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 

  try {

    std::stringstream request ;
    request << "SELECT partitionName FROM Run, Partition WHERE Run.partitionId=Partition.partitionId and runNumber = " << runNumber ;

#ifdef DATABASEDEBUG
    std::cout << "DbCommonAccess::getAllPartitionNames for run " << runNumber << ": " << request << std::endl ;
#endif

    stmt = dbConnection_->createStatement (request.str());
    rset = stmt->executeQuery ();
    while (rset->next ()) {
      std::string name = (std::string)rset->getString (1);
      partitionNameList.push_back(name);
    }
    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);

    partitionNameList.sort() ;
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return partitionNameList;
}

/**Get the partition names from the current state. <BR>
 * The results are stored in a list. <BR> 
 * @return The list containing the partition names
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::list<std::string> DbCommonAccess::getAllPartitionNamesFromCurrentState ( )  throw (oracle::occi::SQLException) {

  std::list<std::string> partitionNameList ;
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 

  try {

    std::stringstream request ;
    request << "SELECT partitionName FROM Partition,StateHistory,CurrentState WHERE Partition.partitionId=StateHistory.partitionId and StateHistory.stateHistoryId=CurrentState.stateHistoryId";

#ifdef DATABASEDEBUG
    std::cout << __PRETTY_FUNCTION__ << ": getting the partition names from the current state" << std::endl ;
#endif

    stmt = dbConnection_->createStatement (request.str());
    rset = stmt->executeQuery ();
    while (rset->next ()) {
      std::string name = (std::string)rset->getString (1);
      partitionNameList.push_back(name);
    }
    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);

    partitionNameList.sort() ;
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return partitionNameList;
}

/**Get the state names. <BR>
 * The results are stored in a list. <BR> 
 * @return The list containing the state names - YOU have to delete that list!!!!!
 * @exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
std::list<std::string> DbCommonAccess::getAllStateHistoryNames ()  throw (oracle::occi::SQLException){
  std::list<std::string> stateHistoryNameList ;
  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ; 

  try {
    stmt = dbConnection_->createStatement ("select distinct stateHistoryName from StateHistory where stateHistoryName is not null order by stateHistoryName");
    rset = stmt->executeQuery ();
    while (rset->next ()) {
      std::string name = (std::string)rset->getString (1);
      stateHistoryNameList.push_back(name);
    }
    stmt->closeResultSet (rset);
    dbConnection_->terminateStatement (stmt);

    stateHistoryNameList.sort() ;
  }
  catch (oracle::occi::SQLException &e) {
    if (rset != NULL) stmt->closeResultSet (rset);
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return stateHistoryNameList;
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A statement is created with the <I>readString</I> parameter and no bind variables are used.<BR>
 * \param readString - PL/SQL request
 * \return a pointer on a Clob containing the data from the database.
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement
 */
oracle::occi::Clob *DbCommonAccess::getXMLClobFromQuery (std::string readString ) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << "readString " << readString << std::endl;
#endif
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(1,oracle::occi::OCCICLOB,20000000,"");

    int returnedVal = stmt->executeUpdate();
    if (returnedVal == 1) {

#ifdef DATABASEDEBUG
      std::cerr << "DbAccess::getXMLClob => executeUpdate returns: " << returnedVal << std::endl;
#endif      
      stmt->executeUpdate();
      *xmlClob_ = stmt->getClob(1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
#ifdef DATABASEDEBUG
      std::cerr << "DbAccess::getXMLClob unable to get the XML buffer from the database" << std::endl;
#endif
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get the current user error message.<BR>
 * \return the current user errorMessage
 * \exception oracle::occi::SQLException : an SQLException is raised in case of trouble in creating or executing the statement.
 */
std::string DbCommonAccess::getErrorMessage() {

  std::string errorMessage;
  static std::string queryString = "BEGIN :a := PkgErrorHandler.getLastError(); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCISTRING,2048);
    stmt->executeQuery();
    errorMessage = stmt->getString(1) ;
    dbConnection_->terminateStatement(stmt);      

    return errorMessage;
  }
  catch (oracle::occi::SQLException &ex) {
    dbConnection_->terminateStatement(stmt); 
  }

  return (NOERRORUSERMESSAGE) ;
}

/** Refresh the cache of the connections, FEC, FED for the current version
 * \param deleteValues - delete the values before doing the update: 0 do not delete, 1 delete the partitions which are not in the current state, 2 delete everything
 * \param withStrips - update the FED with strips
 */
void DbCommonAccess::refreshCacheXMLClob ( int deleteValues, bool withStrips ) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::refreshCacheXMLClob()" << std::endl;
#endif

  static std::string sqlQuery = "BEGIN PkgFecXML.refreshCacheXMLClob(:deleteValues, :withStrips);END;";
  oracle::occi::Statement *stmt = NULL ;
 
  try {
    stmt = dbConnection_->createStatement(sqlQuery);
    stmt->setUInt(1, deleteValues);
    if (withStrips) stmt->setUInt(2, 1);
    else stmt->setUInt(2, 0);
    stmt->executeUpdate() ;
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/** Delete the cache of the connections, FEC, FED
 */
void DbCommonAccess::deleteCacheXMLClob ( ) throw (oracle::occi::SQLException) {

#ifdef DATABASEDEBUG
  std::cerr << "DbCommonAccess::deleteCacheXMLClob()" << std::endl;
#endif

  static std::string sqlQuery = "DELETE FECCACHEXMLCLOB";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(sqlQuery);
    stmt->executeUpdate() ;
    dbConnection_->terminateStatement(stmt);      
    sqlQuery = "DELETE FEDCACHEXMLCLOB";
    stmt = dbConnection_->createStatement(sqlQuery);
    stmt->executeUpdate() ;
    dbConnection_->terminateStatement(stmt);      
    sqlQuery = "DELETE CONNECTIONCACHEXMLCLOB";
    stmt = dbConnection_->createStatement(sqlQuery);
    stmt->executeUpdate() ;
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}

/** TKCC procedure: validate the DCU/PSU map and retreive the cooling loop with a flag
 * \param partitionName - partition name
 * \return a list of pairs containing the name of the cooling and if DCU/PSU map detected is the one expected
 */
std::vector<std::pair<std::string, bool> > DbCommonAccess::setTKCCDcuPsuMapValidation(std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {

  // ----------------------------------------
  // Get the partition ID


  /* SQL> desc view_tk_checkout_daq_ready;
     Name                                      Null?    Type
     ----------------------------------------- --------
     ---------------------------- 
     PARTITION_NAME                                     VARCHAR2(100)
     PARTITIONID                                        NUMBER
     FEDSOFTID                                          NUMBER
     FECID                                              VARCHAR2(15)
     MACHINE_NAME                                       VARCHAR2(100)
     DETECTOR                                           VARCHAR2(3)
     TTC_SLOT                                           NUMBER
     ID                                                 CHAR(3)
  */

  unsigned int partitionId = 0 ;
  //static std::string sqlQueryPart = "SELECT DISTINCT partitionId FROM view_tk_checkout_daq_ready WHERE partition_name = :a" ;
  static std::string sqlQueryPart = "SELECT partitionId FROM Partition WHERE partitionName = :a" ;
  oracle::occi::Statement *stmtPart = NULL ;
  oracle::occi::ResultSet *rsetPart = NULL ;

  try {
    stmtPart = dbConnection_->createStatement (sqlQueryPart);
    unsigned int i = 0;  
    stmtPart->setString(++i, partitionName) ;

    rsetPart = stmtPart->executeQuery ();

    while (rsetPart->next ()) {
      partitionId = (unsigned int)rsetPart->getUInt (1);

#ifdef DATABASEDEBUG
      std::cout << partitionName << " is partition ID " << partitionId << std::endl ;
#endif
    }
    
    stmtPart->closeResultSet (rsetPart);
    dbConnection_->terminateStatement (stmtPart);
  }
  catch (oracle::occi::SQLException &e) {
    if (rsetPart != NULL) stmtPart->closeResultSet (rsetPart);
    dbConnection_->terminateStatement(stmtPart);      
    throw e ;
  }

  // no result from DB raise an exception
  if (partitionId == 0) {
    std::stringstream msgError ; msgError << __PRETTY_FUNCTION__ << ": unable to retrieve the partition ID for partition " << partitionName ;
    RAISEFECEXCEPTIONHANDLER ( DB_PLSQLCALLFAILED, msgError.str(), ERRORCODE ) ;
  }

  // ----------------------------------------
  // Start the validation of the DCU/PSU map
  static std::string sqlQueryValidate = "BEGIN Daq_Partition_Pkg.Acquire_Partition(:a) ; END;" ;
  oracle::occi::Statement *stmtValid = NULL ;
  try {
    stmtValid = dbConnection_->createStatement (sqlQueryValidate) ;
    stmtValid->setAutoCommit(true);
    unsigned int i = 0;  
    stmtValid->setUInt(++i, partitionId) ;
    
#ifdef DATABASEDEBUG
    std::cerr << "DbAccess::setTKCCDcuPsuMapValidation sqlQuery for DCU/PSU map validation : " << sqlQueryValidate << std::endl;
    std::cerr << "DbAccess::setTKCCDcuPsuMapValidation partitionName : " << partitionName << std::endl;
#endif

    stmtValid->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmtValid);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmtValid);      
    throw e ;
  }

  // ---------------------------------------------------------
  // Retreive the cooling loops from the view in TKCC db

  /* The partitionid is the same. The view with the complete and bad cooling
     loops is view_tk_cl_pvss_result the columns are
     SQL> desc view_tk_cl_pvss_result
     Name                                      Null?    Type
     ----------------------------------------- -------- ----------------------------
     CL                                                 VARCHAR2(15)   
     CL_PVSS                                            VARCHAR2(30)   <- cooling loop from PVSS naming
     LAST_PARTITION                                     VARCHAR2(50)
     TEST_STATUS                                        VARCHAR2(4000) <- 3 outputs (Not Tested,Test Failed,Complete)
  */

#define TESTFAILED "Test Failed"
  std::string testFailed = TESTFAILED;

  std::vector<std::pair<std::string, bool> > coolingLoops ;
  static std::string sqlQueryCooling = "SELECT CL_PVSS,TEST_STATUS FROM view_tk_cl_pvss_result WHERE last_partition = :a" ;
  oracle::occi::Statement *stmtCool = NULL ;
  oracle::occi::ResultSet *rsetCool = NULL ;

  try {
    stmtCool = dbConnection_->createStatement (sqlQueryCooling);
    unsigned int i = 0;  
    stmtCool->setString(++i, partitionName) ;

    rsetCool = stmtCool->executeQuery ();

    while (rsetCool->next ()) {
      std::string coolingName = (std::string)rsetCool->getString(1) ;
      std::string testStatus  = (std::string)rsetCool->getString(2);
      bool result = true ;
      if (StrX::StringToLower(testStatus) == StrX::StringToLower(testFailed)) result = false ;

#ifdef DATABASEDEBUG
      std::cout << coolingName << ": " << (result==true?"ok":"not ok") << std::endl ;
#endif

      std::pair<std::string, bool> maPair = std::make_pair(coolingName,result) ;
      coolingLoops.push_back(maPair) ;
    }
    
    stmtCool->closeResultSet (rsetCool);
    dbConnection_->terminateStatement (stmtCool);
  }
  catch (oracle::occi::SQLException &e) {
    if (rsetCool != NULL) stmtCool->closeResultSet (rsetCool);
    dbConnection_->terminateStatement(stmtCool);      
    throw e ;
  }

  return coolingLoops ;
}

/** \brief send a XML buffer to the database to be parsed
 * \param buffer - XML buffer to be sent
 */
int DbCommonAccess::setGenericXMLClob(std::string buffer, std::string tableName) throw (oracle::occi::SQLException, FecExceptionHandler){

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << ": " << buffer <<std::endl;
#endif

  static std::string queryString = "BEGIN :a := PkgXMLTools.insertInRawTable(:b, :c); END;";
  unsigned int nbRows = 0;
  oracle::occi::Statement *stmt = NULL ;

  try {

    unsigned int i = 0 ;

    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(nbRows));

    if ((xmlClob_)->isNull()) {
#ifdef DATABASEDEBUG
      std::cerr << "DbCommonAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
      *xmlClob_ = initializeClob();     
    }
    
    
    size_t bufferSize = buffer.size();
    (xmlClob_)->trim(0);
    (xmlClob_)->write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
    stmt->setClob (++i,*(xmlClob_));    

    stmt->setString (++i, tableName) ;

    nbRows = stmt->executeUpdate() ;
    if (nbRows >= 1) {
      //nbRows = stmt->getNumber(1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): unable to get the number of rows for a generic upload" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &ex){
    dbConnection_->terminateStatement(stmt);
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DbCommonAccess::what(queryString, ex), ERRORCODE) ;
  }

  return nbRows;

}

/**
 * \param message - message from caller side (mainly when the error appends)
 * \param ex - oracle exception raised
 * \return error message 
 */
std::string DbCommonAccess::what ( std::string message, oracle::occi::SQLException &ex ) {

  std::stringstream msgError ;
  msgError << message << ": " << ex.what() ;
  
  try {
    std::string errormsg = getErrorMessage() ;
    if (errormsg.size()) msgError << "\t\t" << errormsg ;
  }
  catch (oracle::occi::SQLException &e) { }
  
  return msgError.str() ;
}

/** this method access the database in order to set the o2o operation performed
 * \param partitionName - partition name
 * \param subDetector - the corresponding subdetector
 * \param o2oTimestamp - a given timestamp (o2o time operation)
 */
void DbCommonAccess::setO2OOperation ( std::string partitionName, std::string subDetector, unsigned int runNumber ) 
  throw (FecExceptionHandler) {

#ifdef DATABASEDEBUG
  std::cerr << __PRETTY_FUNCTION__ << " for partition " << partitionName << " and subdetector " << subDetector << " at " << runNumber << std::endl;
#endif

  static std::string queryString = "BEGIN PkgO2OPartition.insertO2OOperation(:a, :b, :c); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0 ;
    stmt = dbConnection_->createStatement(queryString);
    stmt->setString(++i,partitionName);
    stmt->setString(++i,subDetector);
    stmt->setInt(++i,runNumber);
    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement(stmt); 
  }
  catch (oracle::occi::SQLException &ex){
    dbConnection_->terminateStatement(stmt);
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DbCommonAccess::what(queryString, ex), ERRORCODE) ;
  }
}

/** this method checked that the o2o operation is coherent with the current state
 * \param partitionName - partition name
 * \return true if the versions for the partition is coherent, false if it is not coherent
 */
int DbCommonAccess::getO2OXchecked ( std::string partitionName ) throw (FecExceptionHandler) {

  static std::string queryString = "BEGIN :a := PkgO2OPartition.getO2OXChecked(:b); END;";
  int result = 0;
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement(queryString);
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(result));
    stmt->setString(2,partitionName);
    if (stmt->executeUpdate() == 1) {
      result = stmt->getInt(1);
      dbConnection_->terminateStatement(stmt);
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): unable to get the current run" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &ex){
    dbConnection_->terminateStatement(stmt);
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DbCommonAccess::what(queryString, ex), ERRORCODE) ;
  }

  return result;
}

/** Static variable of oracle::occi::Environment in case of shared connection
 */
oracle::occi::Environment *DbCommonAccess::dbEnvironmentStatic_ = NULL ;

/** Static variable of oracle::occi::Connection in case of shared connection
 */
oracle::occi::Connection *DbCommonAccess::dbConnectionStatic_ = NULL ;

/** Number of connections on the shared connections
 */
unsigned int DbCommonAccess::connectionInUsed_ = 0 ;
