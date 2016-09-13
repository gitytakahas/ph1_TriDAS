/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#include "DeviceFactoryInterface.h"
#include "CommissioningAnalysisFactory.h"
#include "ApvLatencyAnalysisDescription.h"
#include "CommissioningAnalysisDescription.h"




//---------------------------
CommissioningAnalysisFactory::CommissioningAnalysisFactory() : DeviceFactoryInterface() {
  setUsingFile();
}

#ifdef DATABASE
//---------------------------
CommissioningAnalysisFactory::CommissioningAnalysisFactory( std::string login, std::string password, std::string path, bool threaded )
  throw ( oracle::occi::SQLException ) : DeviceFactoryInterface ( login, password, path, threaded ) {
  setDatabaseAccess ( login, password, path );
}

//---------------------------
CommissioningAnalysisFactory::CommissioningAnalysisFactory( DbCommissioningAnalysisAccess *dbAccess ) : DeviceFactoryInterface ((DbCommonAccess *)dbAccess) {;}
#endif

//---------------------------
CommissioningAnalysisFactory::~CommissioningAnalysisFactory() {
  this->deleteCalibrationVector();
}

//------------------------------------
void CommissioningAnalysisFactory::deleteRunVersion() {
  this->_pairOfRunVersion.clear() ;
}

//------------------------------------
void CommissioningAnalysisFactory::deleteAnalysisLocalVersions() {
  this->_analysisLocalVersions.clear();
}

//----------------------------------
void CommissioningAnalysisFactory::deleteCalibrationVector() {
  while (this->_calibrationDescriptions.size() > 0) {
    delete this->_calibrationDescriptions.back();
    this->_calibrationDescriptions.pop_back();
  }
  this->_calibrationDescriptions.clear() ;
}




// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** 
 * @brief Add a new file name in the descriptions
 */
void CommissioningAnalysisFactory::addFileName ( std::string fileName ) {
  setUsingFile();
  //_input_xml_filename = fileName;
  //XMLCommissioningAnalysis xmlHwnd( fileName );
}

/**
 * @brief set a new input file
 */
void CommissioningAnalysisFactory::setInputFileName ( std::string inputFileName ) {
  CommissioningAnalysisFactory::addFileName( inputFileName );
}


/**
 * @brief retrieves descriptions from XML File
 */
std::vector<CommissioningAnalysisDescription*> CommissioningAnalysisFactory::getDescriptions(std::string fileName) {
  this->deleteCalibrationVector();
  setUsingFile();
  XMLCommissioningAnalysis xml(fileName);
  this->_calibrationDescriptions = xml.getDescriptions();
  return this->_calibrationDescriptions;
}



#ifdef DATABASE

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------


/** 
 * @brief Create the database access
 */
void CommissioningAnalysisFactory::setDatabaseAccess( std::string login, std::string password, std::string path ) throw ( oracle::occi::SQLException ) {

  // delete the old one if needed
  if (dbAccess_ != NULL) {
    if (login != dbAccess_->getDbLogin() || password != dbAccess_->getDbPassword() || path != dbAccess_->getDbPath()) {
      delete dbAccess_;
      dbAccess_ = NULL;
    }
  }
  if (dbAccess_ == NULL) {
#ifdef DEBUGMSGERROR
    std::cout << "Create a new access to the commissioning analysis database for " << login << "/" << password << "@" << path << std::endl ;
#endif
    dbAccess_ = (DbAccess *)new DbCommissioningAnalysisAccess (login, password, path, threaded_) ;
  }
  deleteDbAccess_ = true ;
  useDatabase_ = true ;
}

/** 
 * @brief Create the database access with the configuration given by the env. variable CONFDB
 */
void CommissioningAnalysisFactory::setDatabaseAccess() throw ( oracle::occi::SQLException ) {
  std::string login, password, path ;
  if (getDatabaseConfiguration(login, password, path)) {
    setDatabaseAccess (login, password, path) ;
  }
}

/** 
 * @brief Set the database access
 */
void CommissioningAnalysisFactory::setDatabaseAccess( DbCommissioningAnalysisAccess *dbAccess ) {
  // For FEC database delete the old one if needed
  DeviceFactoryInterface::setDatabaseAccess ((DbAccess *)dbAccess) ;
}

#endif





// ------------------------------------------------------------------------------------------------------
// 
//                        CALIBRATION DATA UPLOAD
//
// ------------------------------------------------------------------------------------------------------





//-----------------------------------
uint32_t CommissioningAnalysisFactory::uploadAnalysis( uint32_t runNumber, std::string partitionName, CommissioningAnalysisDescription::commissioningType type, 
						       std::vector<CommissioningAnalysisDescription*> &descriptions, bool updateCurrentState ) throw ( FecExceptionHandler ) {
  uint32_t versionId = 0;

  if ( descriptions.empty() ) RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;

#ifdef DATABASE
  if ( getDbUsed() ) {
    // XML parser creation
    XMLCommissioningAnalysis xmlHwnd;
    xmlHwnd.setDatabaseAccess( dbAccess_ );
    xmlHwnd.setDescriptions  ( descriptions, type );
    versionId = xmlHwnd.uploadAnalysis( updateCurrentState );
  }
  else
#endif
    {
      // Upload in file
      XMLCommissioningAnalysis xmlHwnd;
      xmlHwnd.setDescriptions(descriptions, type);
      xmlHwnd.uploadAnalysis(outputFileName_);
    }

  return versionId;
}


//-----------------------------------
void CommissioningAnalysisFactory::uploadAnalysisState( uint32_t uploadedVersion ) throw ( FecExceptionHandler ) {
#ifndef DATABASE
  return;
#else
  if ( ! getDbUsed() ) return;
  if ( uploadedVersion == 0 ) RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "uploadedVersion should not be NULL", ERRORCODE) ;

  ((DbCommissioningAnalysisAccess *)dbAccess_)->setPartitionState( uploadedVersion );
#endif
}



// ------------------------------------------------------------------------------------------------------
// 
//                 DOWNLOAD VERSIONS AND RESULTS LINKED TO PARTITION STATE
// 
// ------------------------------------------------------------------------------------------------------




//-----------------------------------
std::vector<CommissioningAnalysisDescription*> CommissioningAnalysisFactory::getCalibrationData( uint32_t runNumber,
												 std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler ) {
#ifdef DATABASE
  if ( getDbUsed() ) {
    this->deleteCalibrationVector();
    // XML parser creation
    XMLCommissioningAnalysis xmlHwnd;
    xmlHwnd.setDatabaseAccess( dbAccess_ );
    this->_calibrationDescriptions = xmlHwnd.getCalibrationData(runNumber, partitionName, type);
    return this->_calibrationDescriptions;
  }
  else
#endif
    {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }

}


// ------------------------------------------------------------------------------------------------------
// 
//                 DOWNLOAD ALL LOGGED ANALYSIS VERSIONS AND RESULTS
// 
// ------------------------------------------------------------------------------------------------------



//-----------------------------------
HashMapRunVersion CommissioningAnalysisFactory::getAnalysisHistory(std::string partitionName,
								   CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler ) {

#ifdef DATABASE
  if ( getDbUsed() ) {
    this->deleteRunVersion();
    // XML parser creation
    XMLCommissioningAnalysis xmlHwnd;
    xmlHwnd.setDatabaseAccess( dbAccess_ );
    this->_pairOfRunVersion = xmlHwnd.getAnalysisHistory( partitionName, type );
    return this->_pairOfRunVersion;
  }
  else
#endif
    {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
}


//-----------------------------------
std::vector<CommissioningAnalysisDescription*> CommissioningAnalysisFactory::getAnalysisHistory(std::string partitionName,
												uint32_t versionMajorID, uint32_t versionMinorID, 
												CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler ) {

#ifdef DATABASE
  if ( getDbUsed() ) {
    this->deleteCalibrationVector();
    // XML parser creation
    XMLCommissioningAnalysis xmlHwnd;
    xmlHwnd.setDatabaseAccess( dbAccess_ );
    this->_calibrationDescriptions = xmlHwnd.getAnalysisHistory( partitionName,versionMajorID,versionMinorID,type );
    return this->_calibrationDescriptions;
  }
  else
#endif
    {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
}


//-----------------------------------
HashMapAnalysisVersions CommissioningAnalysisFactory::getLocalAnalysisVersions( uint32_t globalAnalysisVersion ) throw ( FecExceptionHandler ) {
#ifdef DATABASE
  if ( getDbUsed() ) {
    this->deleteAnalysisLocalVersions();
    // XML parser creation
    XMLCommissioningAnalysis xmlHwnd;
    xmlHwnd.setDatabaseAccess( dbAccess_ );
    this->_analysisLocalVersions = xmlHwnd.getLocalAnalysisVersions( globalAnalysisVersion );
    return this->_analysisLocalVersions;
  }
  else
#endif
    {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
}


