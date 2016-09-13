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

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

//#define DATABASEDEBUG

#include "DbCommissioningAnalysisAccess.h"


#include <sstream>
#include "stringConv.h"

//declaration of the exception handler for the DCU
#include "FecExceptionHandler.h"



//---------------------------------
DbCommissioningAnalysisAccess::DbCommissioningAnalysisAccess(bool threaded) 
  throw (oracle::occi::SQLException) : DbCommonAccess (threaded) {;}


//---------------------------------
DbCommissioningAnalysisAccess::DbCommissioningAnalysisAccess(std::string user, std::string passwd, std::string dbPath, bool threaded) 
  throw (oracle::occi::SQLException): DbCommonAccess (user, passwd, dbPath, threaded) {;}


//---------------------------------
DbCommissioningAnalysisAccess::~DbCommissioningAnalysisAccess() throw (oracle::occi::SQLException) {;}






//---------------------------------
void DbCommissioningAnalysisAccess::setXMLClob( std::string buffer, bool updateCurrentState, unsigned int &partitionStateVersionId ) throw ( oracle::occi::SQLException, FecExceptionHandler ) {
  //std::cout<<buffer<<std::endl;
  static std::string writeString = "BEGIN PkgAnalysisXML.configureXMLClob(:buffer,:updateState,:partitionStateVersion); END;";
  oracle::occi::Statement *stmt = NULL;

  try {

    size_t bufferSize = 0;

    if ( buffer.size() ) {
      bufferSize = buffer.size();
      unsigned int i = 0;
      if ((*xmlClob_).isNull()) {
#ifdef DATABASEDEBUG
	std::cerr << "DbCommissioningAnalysisAccess::setXMLClob xmlClob_ attribute is not initialized" << std::endl;
#endif
	*xmlClob_ = initializeClob();
#ifdef DATABASEDEBUG
	std::cout << "DbCommissioningAnalysisAccess::setXMLClob xmlClob_ should be now initialized: " << xmlClob_ << std::endl;
#endif
      }

#ifdef DATABASEDEBUG
      std::cout << "writeString: " << writeString << std::endl;
      std::cout << "buffer:      " << buffer      << std::endl;
      std::cout << "bufferSize:  " << bufferSize  << std::endl;
#endif

      stmt = dbConnection_->createStatement(writeString);
      stmt->setAutoCommit(false);


      (*xmlClob_).trim(0);
      (*xmlClob_).write(bufferSize, (unsigned char*)buffer.c_str(), bufferSize);

      stmt->setClob         ( ++i, (*xmlClob_)                    ); // 1
      stmt->setInt          ( ++i, (updateCurrentState ? 1 : 0)   ); // 2
      stmt->registerOutParam( ++i, oracle::occi::OCCIUNSIGNED_INT ); // 3

      if (stmt->executeUpdate() >= 1 && updateCurrentState ) {
	partitionStateVersionId = stmt->getUInt(3);
#ifdef DATABASEDEBUG
	std::cout << "partitionStateVersionId: " << partitionStateVersionId << std::endl;
#endif
      }

      dbConnection_->commit();
      dbConnection_->terminateStatement (stmt);

    } else {
#ifdef DATABASEDEBUG
      std::cerr << "DbCommissioningAnalysisAccess::setXMLClob buffer is empty" << std::endl;
#endif
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "Unable to perform an upload, buffer is empty", ERRORCODE) ;
    }

  } catch (oracle::occi::SQLException &e) {
    dbConnection_->rollback();
    dbConnection_->terminateStatement(stmt);
    throw e;
  }

}


//---------------------------------
void DbCommissioningAnalysisAccess::setPartitionState( uint32_t uploadedVersion ) throw ( oracle::occi::SQLException ) {

  static std::string writeString = "BEGIN PkgAnalysisXML.setPartitionState(:partitionStateVersion); END;";
  oracle::occi::Statement *stmt = NULL;

  try {

    stmt = dbConnection_->createStatement(writeString);
    stmt->setUInt( 1, uploadedVersion ); // 1
    stmt->executeUpdate();
    dbConnection_->commit();
    dbConnection_->terminateStatement (stmt);

  } catch (oracle::occi::SQLException &e) {
    dbConnection_->rollback();
    dbConnection_->terminateStatement(stmt);
    throw e;

  }

}

//---------------------------------
oracle::occi::Clob* DbCommissioningAnalysisAccess::getCalibrationData( uint32_t runNumber,
								       std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( oracle::occi::SQLException ) {

  static std::string readString = "BEGIN :xmlClob := PkgAnalysisXML.getDowloadAnalysisData(:runNumber, :partitionName, :analysisType); END;";

#ifdef DATABASEDEBUG
  std::cerr << "DbCommissioningAnalysisAccess::getCalibrationData -> readString: " << readString
	    << " for runNumber: " << runNumber 
	    << ", partitionName: " << partitionName 
	    << ", type: " << type << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL;

  try {

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);

    stmt->registerOutParam( ++i, oracle::occi::OCCICLOB, 20000000, "" );
    stmt->setUInt  ( ++i, runNumber     );
    stmt->setString( ++i, partitionName );
    stmt->setString( ++i, CommissioningAnalysisDescription::getAnalysisType( type ) );
    int result = stmt->executeUpdate();

    if ( result >= 1 ) {
      *xmlClob_ = stmt->getClob(1);
      dbConnection_->terminateStatement(stmt);
    } else {
      dbConnection_->terminateStatement(stmt);
      xmlClob_ = NULL;
#ifdef DATABASEDEBUG
      std::cerr << "DbCommissioningAnalysisAccess::getCalibrationData( runNumber, partitionName, type ) unable to get the XML buffer from the database" << std::endl;
#endif
    }

  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->rollback();
    dbConnection_->terminateStatement(stmt);
    throw e;
  }

  return xmlClob_;

}

//---------------------------------
oracle::occi::Clob* DbCommissioningAnalysisAccess::getAnalysisHistory( std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( oracle::occi::SQLException ) {

  static std::string readString = "BEGIN :xmlClob := PkgAnalysisXML.getDownloadAnalysisHistory(:partitionName,:analysisType); END;";

#ifdef DATABASEDEBUG
  std::cerr << "DbCommissioningAnalysisAccess::getAnalysisHistory -> readString: " << readString
	    << " for partitionName: " << partitionName 
	    << ", type: " << type << std::endl;
  std::cerr << " Readable analysis type is: " << CommissioningAnalysisDescription::getAnalysisType( type ) << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL;

  try {

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);

    stmt->registerOutParam( ++i, oracle::occi::OCCICLOB, 20000000, "" );
    stmt->setString( ++i, partitionName );
    stmt->setString( ++i, CommissioningAnalysisDescription::getAnalysisType( type ) );
    int result = stmt->executeUpdate();

    if ( result >= 1 ) {
      *xmlClob_ = stmt->getClob(1);
      dbConnection_->terminateStatement(stmt);
    } else {
      dbConnection_->terminateStatement(stmt);
      xmlClob_ = NULL;
#ifdef DATABASEDEBUG
      std::cerr << "DbCommissioningAnalysisAccess::getDownloadAnalysisHistory( partitionName, type ) unable to get the XML buffer from the database" << std::endl;
#endif
    }

  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->rollback();
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return xmlClob_;

}

//---------------------------------
oracle::occi::Clob* DbCommissioningAnalysisAccess::getAnalysisHistory( std::string partitionName, uint32_t versionMajorID, uint32_t versionMinorID, CommissioningAnalysisDescription::commissioningType type ) throw ( oracle::occi::SQLException ) {

  static std::string readString = "BEGIN :xmlClob := PkgAnalysisXML.getDownloadAnalysisHistory(:partitionName, :versionMajorID, :versionMinorID, :analysisType); END;";

#ifdef DATABASEDEBUG
  std::cerr << "DbCommissioningAnalysisAccess::getAnalysisHistory -> readString: " << readString
	    << ", partitionName: "  << partitionName 
	    << ", versionMajorID: " << versionMajorID 
	    << ", versionMinorID: " << versionMinorID 
	    << ", type: " << type << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL;

  try {

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);

    stmt->registerOutParam( ++i, oracle::occi::OCCICLOB, 20000000, "" );
    stmt->setString( ++i, partitionName );
    stmt->setUInt  ( ++i, versionMajorID);
    stmt->setUInt  ( ++i, versionMinorID);
    stmt->setString( ++i, CommissioningAnalysisDescription::getAnalysisType( type ) );

    int result = stmt->executeUpdate();

    if ( result >= 1 ) {
      *xmlClob_ = stmt->getClob(1);
      dbConnection_->terminateStatement(stmt);
    } else {
      dbConnection_->terminateStatement(stmt);
      xmlClob_ = NULL;
#ifdef DATABASEDEBUG
      std::cerr << "DbCommissioningAnalysisAccess::getAnalysisHistory( partitionName, versionMajorId, versionMinorId, type ) unable to get the XML buffer from the database" << std::endl;
#endif
    }

  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->rollback();
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return xmlClob_;	
}

//---------------------------------
oracle::occi::Clob* DbCommissioningAnalysisAccess::getLocalAnalysisVersions( uint32_t globalAnalysisVersion ) throw ( oracle::occi::SQLException ) {
  static std::string readString = "BEGIN :xmlClob := PkgAnalysisXML.getLocalAnalysisVersions(:globalVersionID); END;";

#ifdef DATABASEDEBUG
  std::cerr << "DbCommissioningAnalysisAccess::getLocalAnalysisVersions -> readString: " << readString
	    << ", globalVersionID: "  << globalAnalysisVersion 
	    << std::endl;
#endif

  oracle::occi::Statement *stmt = NULL;

  try {

    unsigned int i = 0;
    stmt = dbConnection_->createStatement(readString);

    stmt->registerOutParam( ++i, oracle::occi::OCCICLOB, 20000000, "" );
    stmt->setUInt  ( ++i, globalAnalysisVersion);

    int result = stmt->executeUpdate();

    if ( result >= 1 ) {
      *xmlClob_ = stmt->getClob(1);
      dbConnection_->terminateStatement(stmt);
    } else {
      dbConnection_->terminateStatement(stmt);
      xmlClob_ = NULL;
#ifdef DATABASEDEBUG
      std::cerr << "DbCommissioningAnalysisAccess::getLocalAnalysisVersions( globalAnalysisVersion ) unable to get the XML buffer from the database" << std::endl;
#endif
    }

  }
  catch (oracle::occi::SQLException &e) {
    dbConnection_->rollback();
    dbConnection_->terminateStatement(stmt);
    throw e ;
  }

  return xmlClob_;	

}

