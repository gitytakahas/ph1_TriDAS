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

#include "DbTkDcuConversionAccess.h"

/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess()
 */
DbTkDcuConversionAccess::DbTkDcuConversionAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbCommonAccess (threaded) {
}

/**Create an access to the database
 * @param user - user name
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess(std::string user, std::string passwd, std::string dbPath)
 */
DbTkDcuConversionAccess::DbTkDcuConversionAccess (std::string user, std::string passwd, std::string dbPath, bool threaded ) throw (oracle::occi::SQLException) : DbCommonAccess (user, passwd, dbPath, threaded) {
}

/**Close the access to the database
 * @exception oracle::occi::SQLException
 * @see DbAccess::~DbAccess()
 */
DbTkDcuConversionAccess::~DbTkDcuConversionAccess ()  throw (oracle::occi::SQLException) {
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
oracle::occi::Clob *DbTkDcuConversionAccess::getXMLClob(tscType32 dcuHardId ) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgDcuConversionXML.getDcuConversionFromDcuHardId(:dcuHardId);END;";
#ifdef DATABASEDEBUG
  std::cerr << "readString " << readString << std::endl;
  std::cerr << "dcuHardId " << dcuHardId << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ; 

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setInt(++i,dcuHardId);
    int result = stmt->executeUpdate();
    //std::cout<<"resultat : "<<result<<std::endl;
    if (result >= 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
#ifdef DATABASEDEBUG
      std::cerr << "DbTkDcuConversionAccess::getXMLClob unable to get the XML buffer from the database" << std::endl;
#endif
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * @param partitionName - The partition name
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string supervisorIp, unsigned int xdaqInstance, std::string xdaqClassName)
 * @see DbAccess::getXMLClob(std::string readString, std::string supervisorIp, unsigned int xdaqInstance, std::string xdaqClassName)
 * @see PkgFecXML.getAllFecFromPartitionAndIp ( pcIpNumber IN VARCHAR2, xdaqInstanceNb IN NUMBER, xdaqClassName IN VARCHAR2 ) RETURN CLOB;
 */
oracle::occi::Clob *DbTkDcuConversionAccess::getXMLClob( std::string partitionName ) throw (oracle::occi::SQLException) {

  static std::string readString("BEGIN :xmlClob := PkgDcuConversionXML.getDcuConversionFromPartition(:partitionName);END;") ;
  oracle::occi::Statement *stmt = NULL ;

#ifdef DATABASEDEBUG
  std::cerr << "readString " << readString << std::endl;
  std::cerr << "PartitionName " << partitionName << std::endl;
#endif

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,partitionName);
    
    if (stmt->executeUpdate() == 1) {
      *xmlClob_ = stmt->getClob (1);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      xmlClob_ = NULL;
#ifdef DATABASEDEBUG
      std::cerr << "DbTkDcuConversionAccess::getXMLClob unable to get the XML buffer from the database" << std::endl;
#endif
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
void DbTkDcuConversionAccess::setXMLClob(std::string buffer) throw (oracle::occi::SQLException, FecExceptionHandler) 
{
  //std::cout<<buffer<<std::endl;
  static std::string writeString = "BEGIN PkgDcuConversionXML.uploadClob(:buffer); END;";
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    size_t bufferSize = 0;

    if (buffer.size()) {
      bufferSize = buffer.size();
      unsigned int i = 0;  
      if ((*xmlClob_).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "DbTkDcuConversionAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
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
