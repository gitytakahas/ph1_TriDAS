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
#include "DbTkIdVsHostnameAccess.h"

// ostream use for int to string conversion 
#include <sstream>
#include "stringConv.h"

//declaration of the exception handler for the DCU
#include "FecExceptionHandler.h"

/** Create an access to database
 * \param threaded - create a dedicated access
 */
DbTkIdVsHostnameAccess::DbTkIdVsHostnameAccess( bool threaded ) 
  throw (oracle::occi::SQLException) : 
  DbCommonAccess (threaded) {
}

/** Create an access to database
 * \param user - user name
 * \param passwd - password
 * \paran dbPath - path to database
 * \param threaded - create a dedicated access
 */
DbTkIdVsHostnameAccess::DbTkIdVsHostnameAccess ( std::string user, std::string passwd, std::string dbPath, bool threaded  ) 
  throw (oracle::occi::SQLException): 
  DbCommonAccess (user, passwd, dbPath, threaded) {
}

/** Nothing
 */
DbTkIdVsHostnameAccess::~DbTkIdVsHostnameAccess ( ) throw (oracle::occi::SQLException) {
}

/** Download all the ID versus hostname for a given version (option)
 * \param versionMajorId - version major ID, 0 for current state
 * \param versionMinorId - version major ID, 0 for current state
 * \return a clob with the XML buffer
 */
oracle::occi::Clob *DbTkIdVsHostnameAccess::getXMLClob( unsigned int versionMajorId, unsigned int versionMinorId ) 
  throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgIdVsHostnameXML.getAllIdVsHostname(:versionMajor,:versionMinor);END;";

#ifdef DATABASEDEBUG
  std::cerr << "readString " << readString << std::endl;
  if (versionMajorId != 0)
    std::cerr << " for version " << versionMajorId << "." << versionMinorId << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ; 

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setInt(++i,versionMajorId);
    stmt->setInt(++i,versionMinorId);
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
      std::cerr << __PRETTY_FUNCTION__ << ": unable to get the XML buffer from the database" << std::endl;
#endif
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/** Download all the ID versus hostname for a given hostname and version (option)
 * \param hostname - hostname
 * \param versionMajorId - version major ID, 0 for current state
 * \param versionMinorId - version major ID, 0 for current state
 * \return a clob with the XML buffer
 */
oracle::occi::Clob *DbTkIdVsHostnameAccess::getXMLClobFromHostname( std::string hostname, unsigned int versionMajorId, unsigned int versionMinorId ) 
  throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgIdVsHostnameXML.getAllIdFromHostname(:hostname,:versionMajor,:versionMinor);END;";

#ifdef DATABASEDEBUG
  std::cerr << "readString " << readString << " for hostname " << hostname << std::endl;
  if (versionMajorId != 0)
    std::cerr << " for version " << versionMajorId << "." << versionMinorId << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ; 

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,hostname);
    stmt->setInt(++i,versionMajorId);
    stmt->setInt(++i,versionMinorId);
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
      std::cerr << __PRETTY_FUNCTION__ << ": unable to get the XML buffer from the database" << std::endl;
#endif
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  
}

/** Download all the ID versus hostname for a given hostname, slot and version (option)
 * \param hostname - hostname
 * \param slot - VME slot
 * \param versionMajorId - version major ID, 0 for current state
 * \param versionMinorId - version major ID, 0 for current state
 * \return a clob with the XML buffer
 */
oracle::occi::Clob *DbTkIdVsHostnameAccess::getXMLClobFromHostnameSlot ( std::string hostname, unsigned int slot, unsigned int versionMajorId, unsigned int versionMinorId ) 
  throw (oracle::occi::SQLException) {

  static std::string readString = "BEGIN :xmlClob := PkgIdVsHostnameXML.getIdFromHostnameSlot(:hostname,:slot,:versionMajor,:versionMinor);END;";

#ifdef DATABASEDEBUG
  std::cerr << "readString " << readString << " for hostname " << hostname << " and slot " << slot << std::endl;
  if (versionMajorId != 0)
    std::cerr << " for version " << versionMajorId << "." << versionMinorId << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL ; 

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);
    stmt->registerOutParam(++i,oracle::occi::OCCICLOB,20000000,"");
    stmt->setString(++i,hostname);
    stmt->setInt(++i,slot);
    stmt->setInt(++i,versionMajorId);
    stmt->setInt(++i,versionMinorId);
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
      std::cerr << __PRETTY_FUNCTION__ << ": unable to get the XML buffer from the database" << std::endl;
#endif
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }

  return xmlClob_;  

}

/** Retreive the sub-detector for a given version for for a given hostname and version (option) 
 * \param hostname - hostname
 * \param subDetector - sub-detector for the given hostname
 * \param crateNumber - crate number for the given hostname
 * \param versionMajorId - version major ID, 0 for current state
 * \param versionMinorId - version major ID, 0 for current state
 */
void DbTkIdVsHostnameAccess::getSubDetectorCrateNumber ( std::string hostname, std::string &subDetector, unsigned int &crateNumber, unsigned int versionMajorId, unsigned int versionMinorId ) 
  throw (oracle::occi::SQLException) {

  static std::string writeString = "BEGIN PkgIdVsHostnameXML.getSubDetectorCrateNumber(:hostname,:subdetector,:crateNumber,:versionMajor,:versionMinor);END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(writeString);
    stmt->setString(++i,hostname);
    stmt->registerOutParam(++i,oracle::occi::OCCISTRING,110);
    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(crateNumber));
    stmt->setInt(++i,versionMajorId);
    stmt->setInt(++i,versionMinorId);

    if (stmt->executeUpdate() >= 1) {
      subDetector = stmt->getString(2) ;
      crateNumber = stmt->getInt(3);
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << ": Unable to retrieve the sub-detector / crate number for a given hostname and version for hostname " << hostname ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }

    dbConnection_->terminateStatement (stmt);
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}

/** Retreive the version of the TkIdVsHostname from database
 * \param versionMajorId - version major ID, output
 * \param versionMinorId - version major ID, output
 */
void DbTkIdVsHostnameAccess::getTkIdVsHostnameVersion ( unsigned int &versionMajorId, unsigned int &versionMinorId ) 
  throw (oracle::occi::SQLException) {

  static std::string writeString = "BEGIN PkgIdVsHostnameXML.getCurrentVersion(:versionMajor,:versionMinor);END;" ;
  oracle::occi::Statement *stmt = NULL ;

  try {
    unsigned int i = 0;
    stmt = dbConnection_->createStatement(writeString);
    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMajorId)) ;
    stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMinorId)) ;
    if (stmt->executeUpdate() >= 1) {
      versionMajorId = stmt->getInt(1);
      versionMinorId = stmt->getInt(2);
      dbConnection_->terminateStatement(stmt);      
    }
    else {
      dbConnection_->terminateStatement(stmt); 
      std::stringstream msg ;
      msg << DB_PLSQLCALLFAILED_MSG << " (" << writeString << "): Unable to retrieve the version for the ID versus hostname" ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}

/** Upload a clob containing a XML buffer to database
 * \param buffer - XML buffer
 */
void DbTkIdVsHostnameAccess::setXMLClob (std::string buffer, bool major) throw (oracle::occi::SQLException, FecExceptionHandler) {

  static std::string writeString = "BEGIN PkgIdVsHostnameXML.configureXMLClob(:buffer,:versionmajor,:versionminor,:major); END;";
  oracle::occi::Statement *stmt = NULL ;
  unsigned int versionMajor = 0, versionMinor = 0 ;
  unsigned majorValue = 0 ;
  if (major) majorValue = 1 ;

  try {
    stmt = dbConnection_->createStatement (writeString);
    size_t bufferSize = 0;

    if (buffer.size()) {
      bufferSize = buffer.size();
      unsigned int i = 0;  
      if ((*xmlClob_).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << __PRETTY_FUNCTION__ << ": xmlClob_ attribute is not initialized" << std::endl;
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
      stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMajor));
      stmt->registerOutParam(++i,oracle::occi::OCCIINT,sizeof(versionMinor));
      stmt->setInt(++i,majorValue);

      majorValue = stmt->executeUpdate() ;
#ifdef DATABASEDEBUG
      std::cout << "Result of execution = " << majorValue << std::endl ;
#endif

      if (majorValue >= 1) {
	versionMajor = stmt->getInt(2) ;
	versionMinor = stmt->getInt(3);

#ifdef DATABASEDEBUG
	std::cout << "Version " << versionMajor << "." << versionMinor << " have been uploaded" << std::endl ;
#endif
      }
      else {
  	dbConnection_->commit();
  	dbConnection_->terminateStatement(stmt); 
	std::stringstream msg ;
	msg << DB_PLSQLCALLFAILED_MSG << " (" << writeString << "): Unable to upload the ID versus hostname (version is not returned): " << getErrorMessage() ;
	RAISEFECEXCEPTIONHANDLER (DB_PLSQLCALLFAILED, msg.str(), ERRORCODE) ;
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
  catch (oracle::occi::SQLException &e) {
    dbConnection_->terminateStatement(stmt);      
    throw e ;
  }
}
