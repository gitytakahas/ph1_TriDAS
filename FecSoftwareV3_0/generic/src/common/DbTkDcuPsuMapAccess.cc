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

  Copyright 2002 - 2007, Guillaume BAULIEU - IPNL/IN2P3

*/

#include "DbTkDcuPsuMapAccess.h"

/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess()
 */
DbTkDcuPsuMapAccess::DbTkDcuPsuMapAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbCommonAccess (threaded) {
}

/**Create an access to the database
 * @param user - user name
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess(std::string user, std::string passwd, std::string dbPath)
 */
DbTkDcuPsuMapAccess::DbTkDcuPsuMapAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbCommonAccess (user, passwd, dbPath, threaded) {
}

/**Close the access to the database
 * @exception oracle::occi::SQLException
 * @see DbAccess::~DbAccess()
 */
DbTkDcuPsuMapAccess::~DbTkDcuPsuMapAccess ()  throw (oracle::occi::SQLException) {
}

/**Get the current version for a given partition name
 * @param partitionName - Partition Name
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId, maskVersionMajorId, maskVersionMinorId) : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!. Please note that the mask version has no incluence here. It is implemented like that in order to have a common interface with FEC/FED and connections.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getDatabaseVersion (std::string sqlQuery)
 * @todo use a callable statement to execute a PL/SQL function with bind variable to avoid the query parsing at every call 
 */
std::list<unsigned int*> DbTkDcuPsuMapAccess::getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException){

  static std::string sqlQuery = "SELECT DISTINCT StateHistory.partitionId, StateHistory.dcuPsuMapVersionMajorId, StateHistory.dcuPsuMapVersionMinorId, StateHistory.maskVersionMajorId, StateHistory.maskVersionMinorId FROM CurrentState, StateHistory, Partition  WHERE StateHistory.stateHistoryId = CurrentState.stateHistoryId AND StateHistory.partitionId=Partition.partitionId AND Partition.partitionName = :partitionName" ;

  std::list<unsigned int*> listVersion = DbCommonAccess::getDatabaseVersionFromQuery(sqlQuery, partitionName);
  return listVersion ;
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string supervisorIp, unsigned int xdaqInstance, std::string xdaqClassName)</I>.<BR>
 * @param onlyPsuName - only the PSU names will be downloaded (0, means all DCU psu map, 1 means only psu name, 2 means psu not connected)
 * @param partitionName - The name of the partition
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string supervisorIp, unsigned int xdaqInstance, std::string xdaqClassName)
 * @see DbAccess::getXMLClob(std::string readString, std::string supervisorIp, unsigned int xdaqInstance, std::string xdaqClassName)
 * @see PkgFecXML.getAllFecFromPartitionAndIp ( pcIpNumber IN VARCHAR2, xdaqInstanceNb IN NUMBER, xdaqClassName IN VARCHAR2 ) RETURN CLOB;
 */
oracle::occi::Clob *DbTkDcuPsuMapAccess::getXMLClob( int onlyPsuName, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId ) throw (oracle::occi::SQLException, FecExceptionHandler) {
  static std::string readString ;

  switch (onlyPsuName) {
  case 0: readString = "BEGIN :xmlClob := PkgDcuPsuMapXML.getPsuNames(:partitionName,:versionMajor,:versionMinor);END;";
    break ;
  case 1: readString = "BEGIN :xmlClob := PkgDcuPsuMapXML.getDcuPsuMap(:partitionName,:versionMajor,:versionMinor);END;";
    break ;
  case 2: readString = "BEGIN :xmlClob := PkgDcuPsuMapXML.getPsuNotConnected(:partitionName,:versionMajor,:versionMinor);END;";
    break ;
  }

  oracle::occi::Statement *stmt = NULL ;

#ifdef DATABASEDEBUG
  std::cerr << "readString " << readString << std::endl;
  std::cerr << "partitionName " << partitionName << " version " << versionMajorId << "." << versionMinorId << std::endl;
#endif

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    stmt->setInt(++i,versionMajorId);
    stmt->setInt(++i,versionMinorId);
    if (stmt->executeUpdate() >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": unable to get the XML buffer from the database" << std::endl;
#endif
    }
  }
  catch (oracle::occi::SQLException &ex){
    dbConnection_->terminateStatement(stmt); 
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, "unable to retreive the DCU/PSU map: " + readString + ": " + std::string(ex.what()), ERRORCODE) ;
  }
  
  return xmlClob_;
}

/** Request the PSU names from the database
 * @param onlyPsuName - only the PSU names will be downloaded
 * @param partitionName - The name of the partition
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @see the method DbTkDcuPsuMapAccess::getXMLClob( bool onlyPsuName, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId )
 */
oracle::occi::Clob *DbTkDcuPsuMapAccess::getPsuNameXMLClob( std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId ) throw (oracle::occi::SQLException, FecExceptionHandler) {

  return getXMLClob ( 0, partitionName, versionMajorId, versionMinorId ) ;
}

/** Request the DCU / PSU from the database
 * @param onlyPsuName - only the PSU names will be downloaded
 * @param partitionName - The name of the partition
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @see the method DbTkDcuPsuMapAccess::getXMLClob( bool onlyPsuName, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId )
 */
oracle::occi::Clob *DbTkDcuPsuMapAccess::getDcuPsuMapXMLClob( std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId ) throw (oracle::occi::SQLException, FecExceptionHandler) {

  return getXMLClob ( 1, partitionName, versionMajorId, versionMinorId ) ;
}

/** Request the DCU / PSU from the database
 * @param onlyPsuName - only the PSU names will be downloaded
 * @param partitionName - The name of the partition
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @see the method DbTkDcuPsuMapAccess::getXMLClob( bool onlyPsuName, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId )
 */
oracle::occi::Clob *DbTkDcuPsuMapAccess::getPsuNotConnectedXMLClob( std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId ) throw (oracle::occi::SQLException, FecExceptionHandler) {

  return getXMLClob ( 2, partitionName, versionMajorId, versionMinorId ) ;
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer)</I>.<BR>
 * @param psuNameOnly - only the PSU names
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - The name of the partition
 * @param versionMajor - version uploaded
 * @param versionMinor - version uploaded
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see PkgFecXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 */
void DbTkDcuPsuMapAccess::setXMLClob(bool psuNameOnly, std::string buffer, std::string partitionName, unsigned int *versionMajorId, unsigned int *versionMinorId ) throw (oracle::occi::SQLException, FecExceptionHandler) {

  static std::string writeString ;

  if (psuNameOnly) 
    writeString = "BEGIN PkgDcuPsuMapXML.uploadPsuNames(:partitionName,:buffer,:versionMajorId,:versionMinorId); END;";
  else 
    writeString = "BEGIN PkgDcuPsuMapXML.uploadDcuPsuMap(:partitionName,:buffer,:versionMajorId,:versionMinorId); END;";

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
      std::cout << "buffer : " << buffer << std::endl;
      std::cout << "writeString : " << writeString << std::endl;
      std::cout << "bufferSize : " << bufferSize << std::endl;
#endif
      
      (*xmlClob_).trim(0);
      (*xmlClob_).write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);

      unsigned int vm, vn ;

      stmt->setString(++i,partitionName);
      stmt->setClob (++i, (*xmlClob_));
      stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(vm)) ;
      stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(vn)) ;

      if (stmt->executeUpdate() >= 1) {

	vm = stmt->getInt(3);
	vn = stmt->getInt(4);
	if (versionMajorId != NULL) *versionMajorId = vm ;
	if (versionMinorId != NULL) *versionMinorId = vn ;
      }
      else {

	dbConnection_->terminateStatement(stmt); 
	std::string errorMsg = getErrorMessage();
	if (errorMsg.size() && (errorMsg != NOERRORUSERMESSAGE)) {
	  std::stringstream msg ;
	  msg << DB_PLSQLCALLFAILED_MSG << "Unable to retreive the version of PSU/DCU map for partition " << partitionName << ": " << writeString << ": " << errorMsg ;
	  RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
	}
	else {
	  std::stringstream msg ;
	  msg << DB_PLSQLCALLFAILED_MSG << "Unable to retreive the version of PSU/DCU map for partition " << partitionName << ": " << writeString ;
	  RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
	}
      }

      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt);
    } else {
#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": buffer is empty" << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Unable to perform an upload, buffer is empty", ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &ex){
    dbConnection_->terminateStatement(stmt); 

    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, writeString + ": " + std::string(ex.what()), ERRORCODE) ;
  }
}

/** Upload the PSU names in a new version
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - The name of the partition
 * @param versionMajor - version uploaded
 * @param versionMinor - version uploaded
 */
void DbTkDcuPsuMapAccess::uploadPsuNames ( std::string buffer, std::string partitionName, unsigned int *versionMajorId, unsigned int *versionMinorId ) throw (oracle::occi::SQLException, FecExceptionHandler) {

  setXMLClob(true, buffer, partitionName, versionMajorId, versionMinorId ) ;
}

/** Upload the PSU/DCU map in a new version
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - The name of the partition
 * @param versionMajor - version uploaded
 * @param versionMinor - version uploaded
 */
void DbTkDcuPsuMapAccess::uploadDcuPsuMap ( std::string buffer, std::string partitionName, unsigned int *versionMajorId, unsigned int *versionMinorId ) throw (oracle::occi::SQLException, FecExceptionHandler) {

  setXMLClob(false, buffer, partitionName, versionMajorId, versionMinorId ) ;
}

