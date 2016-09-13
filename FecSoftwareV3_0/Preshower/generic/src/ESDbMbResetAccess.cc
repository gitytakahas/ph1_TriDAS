/*
  This file is part of Fec Software project.
  It is used to test the performance of DB--FecSoftware

  author: HUNG Chen-Chien(Dominique), NCU, Jhong-Li, Taiwan
  based on DbMbResetAccess by
  Frderic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include "ESDbMbResetAccess.h"

/** Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::ESDbAccess()
 */
ESDbMbResetAccess::ESDbMbResetAccess(bool threaded) throw (oracle::occi::SQLException) : ESDbAccess(threaded){
	
}

/** Create an access to the database
 * @param user - user name
 * @param passwd - password
 * @param dbPath - database path
 * @param threaded - this parameter define if you want or not to share the connections between all applications ( by default false)
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::ESDbAccess(std::string user, std::string passwd, std::string dbPath)
 */
ESDbMbResetAccess::ESDbMbResetAccess(std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : ESDbAccess(user, passwd, dbPath, threaded){
	
}

/** Close the access to the database
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::~ESDbAccess()
 */
ESDbMbResetAccess::~ESDbMbResetAccess() throw (oracle::occi::SQLException){
	
}


/** Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::getXMLClob(std::string readString, std::string partitionName)</I>.<BR>
 * @param partitionName - name of the partition to be accessed
 * @return a pointer on a Clob containing the data from the database
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::getXMLClob(std::string partitionName)
 * @see ESDbAccess::getXMLClob(std::string readString, std::string partitionName)
 * @see PkgEsMbResetXML.getAllEsMbFromPartition(partitionName IN VARCHAR2) RETURN CLOB;
 */
oracle::occi::Clob *ESDbMbResetAccess::getXMLClob(std::string partitionName) throw (oracle::occi::SQLException){
  static std::string readString = "BEGIN :xmlClob := PkgEsMbResetXML.getAllEsMbFromPartition(:partitionName);END;";
  return ESDbAccess::getXMLClobFromQuery(readString, partitionName);
}


/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::getXMLClob(std::string readString, std::string partitionName)</I>.<BR>
 * @param partitionName - name of the partition to be accessed
 * @param fecHardId - fec hardware ID
 * @return a pointer on a Clob containing the data from the database
 * @exception oracle::occi::SQLException
 * @see ESDbAccess::getXMLClob(std::string partitionName)
 * @see ESDbAccess::getXMLClob(std::string readString, std::string partitionName)
 * @see PkgEsMbResetXML.getAllEsMbFromPartitionAndId(partitionName IN VARCHAR2, fecHardId IN VARCHAR2) RETURN CLOB;
 */
oracle::occi::Clob *ESDbMbResetAccess::getXMLClob(std::string partitionName, std::string fecHardId) throw (oracle::occi::SQLException){
  static std::string readString = "BEGIN :xmlClob := PkgEsMbResetXML.getAllEsMbFromPartitionAndId(:partitionName, :fecHardId);END;";
  return ESDbAccess::getXMLClobFromQuery(readString, partitionName, fecHardId);
}



/** Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containg the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>ESDbAccess::setXMLClob(std::string writeStrng, std::string buffer, std::string partitionName)</I>.<BR>
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partitionName
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see ESDbAccess::setXMLClob(std::string writeString, std::string buffer, std::string partitionName)
 * @see PkgEsMbResetXML.addXMLClob(xmlClob IN CLOB, partitionName in VARCHAR2) RETURN NUMBER;
 */
void ESDbMbResetAccess::setXMLClob(std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler){
  static std::string writeString("BEGIN PkgEsMbResetXML.uploadXMLClob(:bufferMbReset, :partitionName);END;");
  oracle::occi::Statement *stmt = NULL ;
	
  try {
    stmt = dbConnection_->createStatement(writeString);
    stmt->setAutoCommit(true);
    size_t bufferSize = 0;
    unsigned int i = 0;
	
#ifdef DATABASEDEBUG
    std::cerr << "ESDbMbResetAccess::setXMLClob writeString : " << writeString << std::endl;
    std::cerr << "ESDbMbResetAccess::setXMLClob partitionName : " << partitionName << std::endl;
#endif
	  
    if (buffer.size()){
      bufferSize = buffer.size();
#ifdef DATABASEDEBUG
      std::cerr << "ESDbMbResetAccess::setXMLClob bufferSize = " << buffer.size() << " = " << bufferSize << std::endl;
#endif
	    
      if ((*xmlClob_).isNull()){
#ifdef DATABASEDEBUG
	std::cerr << "ESDbMbResetAccess::setXMLClob xmlClob_ attribute is not initialized" <<	std::endl;
#endif
	*xmlClob_ = initializeClob();
      }
	    
#ifdef DATABASEDEBUG
      std::cerr << "ESDbMbResetAccess::setXMLClob buffer : " << buffer << std::endl;
      std::cerr << "ESDbMbResetAccess::setXMLClob bufferSize = " << bufferSize << std::endl;
#endif
		
      (*xmlClob_).trim(0);
      (*xmlClob_).write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);
      stmt->setClob(++i, (*xmlClob_));
      stmt->setString (++i, partitionName);
	    
      stmt->execute();
      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt);
      
    }
    else {
#ifdef DATABASEDEBUG
      std::cerr << "ESDbMbResetAccess::setXMLClob buffer is empty " << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Unable to perform an upload, buffer is empty", ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }
}
