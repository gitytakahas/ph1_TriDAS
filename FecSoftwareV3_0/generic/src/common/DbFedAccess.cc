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
#include "DbFedAccess.h"
#include <sstream>
#include "stringConv.h"

//#define DATABASEDEBUG

/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess()
 */
DbFedAccess::DbFedAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbAccess (threaded) {
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
DbFedAccess::DbFedAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbAccess (user, passwd, dbPath, threaded) {
}

/**Close the access to the database
 * @exception oracle::occi::SQLException
 * @see DbAccess::~DbAccess()
 */
DbFedAccess::~DbFedAccess ()  throw (oracle::occi::SQLException){
#ifdef DATABASESTAT
  this->traceSql(false);
#endif
}

/**Get the current version for a given partition name
 * @param partitionName - Partition Name
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId, maskVersionMajorId, maskVersionMinorId) : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException
 * @see DbAccess::getDatabaseVersion (std::string sqlQuery)
 * @todo use a callable statement to execute a PL/SQL function with bind variable to avoid the query parsing at every call 
 */
std::list<unsigned int*> DbFedAccess::getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException)
{
  static std::string sqlQuery ;
  sqlQuery = "SELECT DISTINCT StateHistory.partitionId, StateHistory.fedVersionMajorId, StateHistory.fedVersionMinorId, StateHistory.maskVersionMajorId, StateHistory.maskVersionMinorId FROM CurrentState, StateHistory, Partition  WHERE StateHistory.stateHistoryId = CurrentState.stateHistoryId AND StateHistory.partitionId = Partition.partitionId AND Partition.partitionName= :partitionName" ;

  return DbCommonAccess::getDatabaseVersionFromQuery(sqlQuery, partitionName);
}

/**Create a new current state with partitions/versions. <BR>
 * Select the current state and check if it is registered (check its registered parameter value). <BR>
 * If this state is registered : a new state is create. <BR>
 * The state is updated with the data found in the parameter <I>partitionVersionsListe</I>. <BR>
 * The current state is update to this new state. <BR>
 * @param partitionVersionsListe : pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId)  : THE CONTENT OF THIS LIST OF POINTERS IS REMOVED BY THE FUNCTION !!
 * @exception FecExceptionHandler : an exception is raised in case of trouble in statement creation or execution
 * @see PkgState.createNewCurrentState RETURN NUMBER;
 */
void DbFedAccess::setDatabaseVersion(std::list<unsigned int*> partitionVersionsListe) throw (FecExceptionHandler){

  oracle::occi::Statement *stmt = NULL ;
  oracle::occi::ResultSet *rset = NULL ;
  static std::string sqlQuery = "SELECT DISTINCT StateHistoryId FROM CurrentState" ;
  
  try {
    unsigned int *partitionVersions;
    
    // get a last state id
    unsigned int currentStateId = 0;
    
    stmt = dbConnection_->createStatement (sqlQuery);
    rset = stmt->executeQuery ();
    while (rset->next ()) {
      currentStateId = rset->getUInt(1);
    }
    dbConnection_->terminateStatement(stmt);          
    
    // The state cannot be changed so a new one is created
    // as we could not modify this state, we need to
    // create a new state and set this state as current state
    
    static std::string queryString = "BEGIN :a := PkgState.createNewCurrentState();END;";
    
    stmt = dbConnection_->createStatement(queryString);
    
    stmt->registerOutParam(1,oracle::occi::OCCIINT,sizeof(currentStateId));
    
    if (stmt->executeUpdate() == 1) {
      currentStateId = stmt->getUInt(1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << queryString << "): " << "unable to set the versions / partitions in a new state history" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
    
    sqlQuery = "UPDATE StateHistory SET StateHistory.fedVersionMajorId = :a , StateHistory.fedVersionMinorId = :b WHERE StateHistory.partitionId = :c AND State.stateId = :d";
    
    stmt = dbConnection_->createStatement (sqlQuery);
    
    stmt->setUInt(4, currentStateId);
    
    while (partitionVersionsListe.size() > 0){
      partitionVersions = (unsigned int *)partitionVersionsListe.back();
      
      stmt->setUInt(3, partitionVersions[0]);
      stmt->setUInt(1, partitionVersions[1]);
      stmt->setUInt(2, partitionVersions[2]);
	
      stmt->executeUpdate();
	
      //delete[] partitionVersions;
      //partitionVersionsListe.pop_back();
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
    msg << DB_PLSQLEXCEPTIONRAISED_MSG << " (" << sqlQuery << "): " << "unable to set the versions / partitions in a new state history" ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(msg.str(), ex), ERRORCODE) ;
  }
}

/**Define the query to get the next major version and execute the request
 * @return the next available major version 
 * @exception SQLException
 * @see DbAccess::getVersion(std::string sqlQuery)
 */
unsigned int DbFedAccess::getNextMajorVersion() throw (FecExceptionHandler)
{
  static std::string sqlQuery = "SELECT MAX(versionMajorId)+1 FROM FedVersion";
  return (unsigned int)DbAccess::getVersion(sqlQuery);
}

/**Define the query to get the next minor version with major version = majorId and execute the request
 * @return the greatest minor version with major version = majorId
 * @param majorId : major version number
 * @exception SQLException
 * @see DbAccess::getVersion(std::string sqlQuery)
 */
unsigned int DbFedAccess::getNextMinorVersion(unsigned int majorId) throw (FecExceptionHandler)
{
  static std::string sqlQuery ;
  sqlQuery = "SELECT MAX(versionMinorId)+1 FROM FedVersion where versionMajorId =" + toString(majorId);
  return (unsigned int)DbAccess::getVersion(sqlQuery);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)</I>.<BR>
 * @param partitionName - partition name
 * @param versionMajor - major version number
 * @param versionMinor - minor version number 
 * @param maskVersionMajor - mask version major
 * @param maskVersionMinor - mask version minor
 * @param withStrip - boolean set to true if you need to download strip values from database, set to false otherwise
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)
 * @see PkgFedXML.getAllFedFromPartition ( partitionName IN VARCHAR2, versionMajor IN NUMBER, versionMinor IN NUMBER, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClobWithVersion(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor, boolean withStrip) throw (oracle::occi::SQLException) {
  static std::string readString;
  if (withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionVersion(:partitionName, :versionMajorId, :versionMinorId, :maskMajor, :maskMinor, 1);END;";
  } else {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionVersion(:partitionName, :versionMajorId, :versionMinorId, :maskMajor, :maskMinor, 0);END;";
  }

#ifdef DATABASEDEBUG
  std::cout << __PRETTY_FUNCTION__ << ": " << readString << " for  partititon " << partitionName << ", version " << versionMajor << "." << versionMinor << " and mask " << maskVersionMajor << "." << maskVersionMinor << std::endl ;
#endif

  return DbAccess::getXMLClobFromQuery(readString, partitionName, versionMajor, versionMinor, maskVersionMajor, maskVersionMinor);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)</I>.<BR>
 * @param partitionName - partition name
 * @param versionMajor - major version number
 * @param versionMinor - minor version number 
 * @param maskVersionMajor - mask version major
 * @param maskVersionMinor - mask version minor
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)
 * @see PkgFedXML.getAllFedFromPartition ( partitionName IN VARCHAR2, versionMajor IN NUMBER, versionMinor IN NUMBER, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClobWithVersion(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException) {
  return DbFedAccess::getXMLClobWithVersion(partitionName, versionMajor, versionMinor, maskVersionMajor, maskVersionMinor, true);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * Call the method <I>DbFedAccess::getXMLClob(std::string partitionName, boolean withStrip)</I> with withStrip = true.<BR>
 * @param partitionName - name of the partition to be accessed
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string partitionName)
 * @see DbFedAccess::getXMLClob(std::string partitionName, boolean withStrip)
 */
oracle::occi::Clob *DbFedAccess::getXMLClob(std::string partitionName) throw (oracle::occi::SQLException) {
  return DbFedAccess::getXMLClob(partitionName, true);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName)</I>.<BR>
 * @param partitionName - name of the partition to be accessed
 * @param withStrip - boolean set to true if you need to download strip values from database, set to false otherwise
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName)
 * @see PkgFedXML.getAllFedFromPartition ( partitionName IN VARCHAR2, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClob(std::string partitionName, boolean withStrip ) throw (oracle::occi::SQLException) {
  static std::string readString;
  if (withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartition(:partitionName);END;";
  } else {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartition(:partitionName, 0);END;";
  }
  return DbAccess::getXMLClobFromQuery(readString, partitionName);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int id)</I>.<BR>
 * @param partitionName - partition name
 * @param fedId - fed identifier
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int id)
 * @see PkgFedXML.getAllFedFromPartitionAndId ( partitionName IN VARCHAR2, fedNumber IN NUMBER, fedHardId IN BOOLEAN := true, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClob(std::string partitionName, unsigned int fedId) throw (oracle::occi::SQLException) {
  return DbFedAccess::getXMLClob(partitionName, fedId, true, true);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int id)</I>.<BR>
 * @param partitionName - partition name
 * @param fedId - fed identifier
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int id)
 * @see PkgFedXML.getAllFedFromPartitionAndId ( partitionName IN VARCHAR2, fedNumber IN NUMBER, fedHardId IN BOOLEAN := true, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClob(std::string partitionName, std::string fedId) throw (oracle::occi::SQLException) {
  return DbFedAccess::getXMLClob(partitionName, fromString<unsigned int>(((const std::string)fedId)));
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int id)</I>.<BR>
 * @param partitionName - partition name
 * @param fedId - fed identifier
 * @param hardId - is fed identifier an hardware or software identifier ?
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int id)
 * @see PkgFedXML.getAllFedFromPartitionAndId ( partitionName IN VARCHAR2, fedNumber IN NUMBER, fedHardId IN BOOLEAN := true, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClob(std::string partitionName, unsigned int fedId, boolean hardId) throw (oracle::occi::SQLException) {
  return DbFedAccess::getXMLClob(partitionName, fedId, hardId, true);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int id)</I>.<BR>
 * @param partitionName - partition name
 * @param fedId - fed identifier
 * @param hardId - boolean  : true if fedId is a hardware identifier, false if fedId is a software identifier
 * @param withStrip - boolean set to true if you need to download strip values from database, set to false otherwise
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int id)
 * @see PkgFedXML.getAllFedFromPartitionAndId ( partitionName IN VARCHAR2, fedNumber IN NUMBER, fedHardId IN BOOLEAN := true, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClob(std::string partitionName, unsigned int fedId, boolean hardId, boolean withStrip) throw (oracle::occi::SQLException) {
  static std::string readString;
  if (hardId && withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionAndId(:partitionName, :fedId, 1, 1);END;";
  } else if (hardId && !withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionAndId(:partitionName, :fedId, 1, 0);END;";
  } else if (!hardId && withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionAndId(:partitionName, :fedId, 0, 1);END;";
  } else if (!hardId && !withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionAndId(:partitionName, :fedId, 0, 0);END;";
  }
  return DbAccess::getXMLClobFromQuery(readString, partitionName, toString(fedId));
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int fedId, unsigned int versionMajor, unsigned int versionMinor)</I>.<BR>
 * @param partitionName - partition name
 * @param fedId - fed identifier
 * @param versionMajorId - major version number
 * @param versionMinorId - minor version number 
 * @param maskVersionMajor - mask major version
 * @param maskVersionMinor - mask minor version
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString,  std::string partitionName, unsigned int fedId, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)
 * @see PkgFedXML.getAllFedFromPartitionAndId ( partitionName IN VARCHAR2, fedNumber IN NUMBER, fedHardId IN BOOLEAN := true, versionMajor IN NUMBER, versionMinor IN NUMBER, pMaskMajorVersion IN NUMBER, pMaskMinorVersion IN NUMBER, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClobWithVersion(std::string partitionName, std::string fedId, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException) {
  return DbFedAccess::getXMLClobWithVersion(partitionName, fromString<unsigned int>(((const std::string&)fedId)), versionMajorId, versionMinorId, maskVersionMajor, maskVersionMinor);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int fedId, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)</I>.<BR>
 * @param partitionName - partition name
 * @param fedId - fed identifier
 * @param versionMajorId - major version number
 * @param versionMinorId - minor version number
 * @param maskVersionMajor - mask major version
 * @param maskVersionMinor - mask minor version 
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString,  std::string partitionName, unsigned int fedId, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)
 * @see PkgFedXML.getAllFedFromPartitionAndId ( partitionName IN VARCHAR2, fedNumber IN NUMBER, fedHardId IN BOOLEAN := true, versionMajor IN NUMBER, versionMinor IN NUMBER, pMaskMajorVersion IN NUMBER, pMaskMinorVersion IN NUMBER, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClobWithVersion(std::string partitionName, unsigned int fedId, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException) {
  return DbFedAccess::getXMLClobWithVersion( partitionName, fedId, true, versionMajorId, versionMinorId, maskVersionMajor, maskVersionMinor, true);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int fedId, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)</I>.<BR>
 * @param partitionName - partition name
 * @param fedId - fed identifier
 * @param hardId - boolean  : true if fedId is a hardware identifier, false if fedId is a software identifier
 * @param versionMajor - major version number
 * @param versionMinor - minor version number 
 * @param maskVersionMajor - mask major version
 * @param maskVersionMinor - mask minor version 
 * @param withStrip - boolean set to true if you need to download strip values from database, set to false otherwise
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString,  std::string partitionName, unsigned int fedId, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)
 * @see PkgFedXML.getAllFedFromPartitionAndId ( partitionName IN VARCHAR2, fedNumber IN NUMBER, fedHardId IN BOOLEAN := true, versionMajor IN NUMBER, versionMinor IN NUMBER, pMaskMajorVersion IN NUMBER, pMaskMinorVersion IN NUMBER, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClobWithVersion(std::string partitionName, unsigned int fedId, boolean hardId, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor, boolean withStrip ) throw (oracle::occi::SQLException) {
  static std::string readString;
  if (hardId && withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionIdAndId(:partitionId, :fedId, 1, :versionMajorId, :versionMinorId, :maskMajor, :maskMinor, 1);END;";
  } else if (hardId && !withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionIdAndId(:partitionId, :fedId, 1, :versionMajorId, :versionMinorId, :maskMajor, :maskMinor, 0);END;";
  } else if (!hardId && withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionIdAndId(:partitionId, :fedId, 0, :versionMajorId, :versionMinorId, :maskMajor, :maskMinor, 1);END;";
  } else if (!hardId && !withStrip) {
    readString = "BEGIN :xmlClob := PkgFedXML.getAllFedFromPartitionIdAndId(:partitionId, :fedId, 0, :versionMajorId, :versionMinorId, :maskMajor, :maskMinor, 0);END;";
  }

#ifdef DATABASEDEBUG
  std::cout << __PRETTY_FUNCTION__ << ": " << readString << " for fed " << (hardId ? "hardware id " : "soft id ") << fedId << ", partititon " << partitionName << ", version " << versionMajor << "." << versionMinor << " and mask " << maskVersionMajor << "." << maskVersionMinor << std::endl ;
#endif

  unsigned int partitionId = getPartitionId(partitionName) ;

#ifdef DATABASEDEBUG
  std::cerr <<  __PRETTY_FUNCTION__ << " readString " << readString << std::endl;
  std::cerr <<  __PRETTY_FUNCTION__ << " partitionName " << partitionName << "(id = " << partitionId << ")" << std::endl;
  std::cerr <<  __PRETTY_FUNCTION__ << " id " << fedId << std::endl;
  std::cerr <<  __PRETTY_FUNCTION__ << " versionMajorId " << versionMajor << std::endl;
  std::cerr <<  __PRETTY_FUNCTION__ << " versionMinorId " << versionMinor << std::endl;
  std::cerr <<  __PRETTY_FUNCTION__ << " maskVersionMajor " << maskVersionMajor << std::endl;
  std::cerr <<  __PRETTY_FUNCTION__ << " maskVersionMinor " << maskVersionMinor << std::endl;
  std::cerr <<  __PRETTY_FUNCTION__ << " withStrip " << withStrip << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setUInt(++i,partitionId);
    stmt->setUInt(++i,fedId);
    stmt->setUInt(++i,versionMajor);
    stmt->setUInt(++i,versionMinor);
    stmt->setUInt(++i,maskVersionMajor);
    stmt->setUInt(++i,maskVersionMinor);
    stmt->executeUpdate();
    *xmlClob_ = stmt->getClob (1);
    dbConnection_->terminateStatement(stmt);      
  }
  catch (oracle::occi::SQLException &e) {
#ifdef DATABASEDEBUG
    std::cerr << e.getMessage() << std::endl ;
#endif
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int fedId, unsigned int versionMajor, unsigned int versionMinor)</I>.<BR>
 * @param partitionName - partition name
 * @param fedId - fed identifier
 * @param hardId - boolean  : true if fedId is a hardware identifier, false if fedId is a software identifier
 * @param versionMajor - major version number
 * @param versionMinor - minor version number 
 * @param maskVersionMajor - mask major version
 * @param maskVersionMinor - mask minor version 
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string readString,  std::string partitionName, unsigned int fedId, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)
 * @see PkgFedXML.getAllFedFromPartitionAndId ( partitionName IN VARCHAR2, fedNumber IN NUMBER, fedHardId IN BOOLEAN := true, versionMajor IN NUMBER, versionMinor IN NUMBER, pMaskVersionMajor IN NUMBER, pMaskVersionMinor IN NUMBER, withStrip IN BOOLEAN := true ) RETURN CLOB;
 */
oracle::occi::Clob *DbFedAccess::getXMLClobWithVersion(std::string partitionName, unsigned int fedId, boolean hardId, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException) {
  return DbFedAccess::getXMLClobWithVersion( partitionName, fedId, hardId, versionMajor, versionMinor, maskVersionMajor, maskVersionMinor, true);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor)</I>.<BR>
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param versionMajor - major version number for upload
 * @param versionMinor - minor version number for upload
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string buffer, unsigned int versionMajor, unsigned int versionMinor)
 * @see DbAccess::setXMLClob(std::string writeString, std::string buffer, unsigned int versionMajor, unsigned int versionMinor)
 * @see PkgFedXML.uploadXMLClob ( xmlClob IN CLOB, versionMajor IN NUMBER, versionMinor IN NUMBER);
 */
void DbFedAccess::setXMLClobWithVersion(std::string buffer, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor) throw (oracle::occi::SQLException, FecExceptionHandler) 
{
  static std::string writeString = "BEGIN PkgFedXML.uploadXMLClob(:buffer, :partitionName, :versionMajorId, :versionMinorId);END;";
  DbAccess::setXMLClobFromQuery (writeString, buffer, partitionName, versionMajor, versionMinor);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, int versionUpdate)</I>.<BR>
 * The data are stored in the version number :
 *   - nextMajorVersion.0 if versionUpdate = 1
 *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
 *   - currentMajorVersion.0 if versionUpdate = 2
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param versionUpdate - set this parameter
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string buffer, boolean versionUpdate)  
 * @see DbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)  
 * @see PkgFedXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 */
void DbFedAccess::setXMLClob(std::string buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler) {
  static std::string writeString("BEGIN PkgFedXML.uploadXMLClob(:buffer, :partitionName, :versionUpdate);END;");
  DbAccess::setXMLClobFromQuery (writeString, buffer, partitionName, versionUpdate);
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
void DbFedAccess::setXMLClob(std::string* buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler) {
  static std::string writeString("BEGIN PkgFedXML.uploadXMLClob(:bufferFed, :bufferTtcrx, :bufferVoltageMonitor, :bufferFeFpga, :bufferChannelPair, :bufferChannel, :bufferApvFed, :bufferStrip, :partitionName, :versionUpdate); END;");
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  
    unsigned int j = 0;  

#ifdef DATABASEDEBUG
    std::cerr << "-----------------------------------------------------------------" << std::endl ;
    std::cerr << "DbFedAccess::setXMLClobFromQuery writeString : " << writeString << std::endl;
    std::cerr << "DbFedAccess::setXMLClobFromQuery partitionName : " << partitionName << std::endl;
    std::cerr << "DbFedAccess::setXMLClobFromQuery versionUpdate : "<< versionUpdate << std::endl;
    std::cerr << buffer[0] << std::endl ;
    std::cerr << buffer[1] << std::endl ;
    std::cerr << buffer[2] << std::endl ;
    std::cerr << buffer[3] << std::endl ;
    std::cerr << buffer[4] << std::endl ;
    std::cerr << buffer[5] << std::endl ;
    std::cerr << buffer[6] << std::endl ;
    std::cerr << buffer[7] << std::endl ;
    std::cerr << "-----------------------------------------------------------------" << std::endl ;
#endif

    for (j=0; j<8; j++) {
      //    if (buffer[j].size()) {
      bufferSize = buffer[j].size();
#ifdef DATABASEDEBUG
      std::cerr << "DbFedAccess::setXMLClobFromQuery bufferSize["<<j<<"] =" << buffer[j].size() << " = " << bufferSize << std::endl;
#endif
    
      if ((xmlClobArray_[j]).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "DbFedAccess::setXMLClobFromQuery xmlClob_ attribute is not initialized" << std::endl;
#endif
	xmlClobArray_[j] = initializeClob();
      }
    
#ifdef DATABASEDEBUG
      std::cerr << "DbFedAccess::setXMLClobFromQuery buffer["<<j<<"] : " << buffer[j] << std::endl;
      std::cerr << "DbFedAccess::setXMLClobFromQuery bufferSize["<<j<<"] = " << bufferSize << std::endl;
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
