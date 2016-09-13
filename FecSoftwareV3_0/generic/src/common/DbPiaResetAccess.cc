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
#include "DbPiaResetAccess.h"


/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess()
 */
DbPiaResetAccess::DbPiaResetAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbAccess (threaded) {
}

/**Create an access to the database
 * @param user - user name
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess(std::string user, std::string passwd, std::string dbPath)
 */
DbPiaResetAccess::DbPiaResetAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbAccess (user, passwd, dbPath, threaded) {
}

/**Close the access to the database
 * @exception oracle::occi::SQLException
 * @see DbAccess::~DbAccess()
 */
DbPiaResetAccess::~DbPiaResetAccess ()  throw (oracle::occi::SQLException) {
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName)</I>.<BR>
 * @param partitionName - name of the partition to be accessed
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string partitionName)
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName)
 * @see PkgPiaResetXML.getAllFecFromPartition ( partitionName IN VARCHAR2 ) RETURN CLOB;
 */
oracle::occi::Clob *DbPiaResetAccess::getXMLClob (std::string partitionName) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgPiaResetXML.getAllPiaFromPartition(:partitionName);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName);  

}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName)</I>.<BR>
 * @param partitionName - name of the partition to be accessed
 * @param fecHardId - FEC hardware ID
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string partitionName)
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName)
 * @see PkgPiaResetXML.getAllFecFromPartition ( partitionName IN VARCHAR2 ) RETURN CLOB;
 */
oracle::occi::Clob *DbPiaResetAccess::getXMLClob (std::string partitionName, std::string fecHardId) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgPiaResetXML.getAllPiaFromPartitionAndId(:partitionName, :fecHardId);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName, fecHardId);  

}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, std::string partitionName.<BR>
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string writeString, std::string buffer, std::string partitionName) 
 * @see PkgPiaResetXML.addXMLClob ( xmlClob IN CLOB, partitionName IN VARCHAR2) RETURN NUMBER;
 */
void DbPiaResetAccess::setXMLClob(std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler) {
  static std::string writeString("BEGIN PkgPiaResetXML.uploadXMLClob(:bufferPiaReset, :partitionName);END;");
  oracle::occi::Statement *stmt = NULL ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;  
    
#ifdef DATABASEDEBUG
    std::cerr << "DbPiaResetAccess::setXMLClob writeString : " << writeString << std::endl;
    std::cerr << "DbPiaResetAccess::setXMLClob partitionName : " << partitionName << std::endl;
    std::cerr << "DbPiaResetAccess::setXMLClob " << buffer << std::endl ;
#endif
    
    if (buffer.size()) {
      bufferSize = buffer.size();
#ifdef DATABASEDEBUG
      std::cerr << "DbPiaResetAccess::setXMLClob bufferSize =" << buffer.size() << " = " << bufferSize << std::endl;
#endif
      
      if ((*xmlClob_).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "DbPiaResetAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
	*xmlClob_ = initializeClob();
      }
      
      
#ifdef DATABASEDEBUG
      std::cerr << "DbPiaResetAccess::setXMLClob buffer : " << buffer << std::endl;
    std::cerr << "DbPiaResetAccess::setXMLClob bufferSize = " << bufferSize << std::endl;
#endif
    
    (*xmlClob_).trim(0);
    (*xmlClob_).write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
    stmt->setClob (++i, (*xmlClob_));
    stmt->setString (++i, partitionName);
    
    stmt->execute();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);
    
    } else {

#ifdef DATABASEDEBUG
      std::cerr << __PRETTY_FUNCTION__ << ": buffer is empty" << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
				"Unable to perform an upload, buffer is empty",
				ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}
