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
  
  Copyright 2002 - 2007, G. BAULIEU - IPNL/IN2P3
*/
  
  //#define DEBUGMSGERROR

#include <iostream>
#include <fstream>
#include <map>

#include "stringConv.h"

#include "XMLTkDcuPsuMap.h"
#include "TkDcuPsuMapFactory.h"

#define NODCUID 0

/** Build a TkDcuPsuMapFactory to retreive information from database or file
 * \param databaseAccess - if true create a database access
 */
TkDcuPsuMapFactory::TkDcuPsuMapFactory ( ):
  DeviceFactoryInterface ( ) {

// #ifdef DATABASE
//   if (databaseAccess) {
//     try {
//       setDatabaseAccess ( ) ;
//     }
//     catch (oracle::occi::SQLException &e) {
      
//       std::cerr << "*********** ERROR *************" << std::endl ;
//       std::cerr << "Unable to connect the database: " << std::endl ;
//       std::cerr << e.what() << std::endl ;
//       std::cerr << "*******************************" << std::endl ;

//       databaseAccess = false ;
//     }
//   }
// #else
//   databaseAccess = false ;
// #endif

//   if (! databaseAccess) setUsingFile ( ) ;

  setUsingFile ( ) ;
}

#ifdef DATABASE

/** Create an access to the database FEC database
 * \param login - login to database
 * \param password - password to database
 * \param path - URL to acess the database
 * \param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 */
TkDcuPsuMapFactory::TkDcuPsuMapFactory ( std::string login, std::string password, std::string path, bool threaded ) 
  throw ( oracle::occi::SQLException ):
  DeviceFactoryInterface ( login, password, path, threaded ) {
  setDatabaseAccess ( login, password, path ) ;
}

/** Create an access to the FEC database
 * \param dbFecAccess - database access
 */
TkDcuPsuMapFactory::TkDcuPsuMapFactory ( DbTkDcuPsuMapAccess *dbAccess ):
  DeviceFactoryInterface ((DbAccess *)dbAccess) {
}

#endif

/** Disconnect the database (if it is set)
 */  
TkDcuPsuMapFactory::~TkDcuPsuMapFactory ( ) {
  
  // The database access is deleted in the DeviceFactoryInterface
  
  // Emptyied the list 
  deleteVectorI (vCGDcuPsuMap_) ;
  deleteVectorI (vPGDcuPsuMap_) ;
  deleteVectorI (vCGPsuNotConnected_) ;
  deleteVectorI (vPGPsuNotConnected_) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 */
void TkDcuPsuMapFactory::addFileName ( std::string fileName ) {

  XMLTkDcuPsuMap xmlTkDcuPsuMap ( fileName ) ;
  // Retreive the dcu-psu mapping
  tkDcuPsuMapVector vDcuPsuMapVector = xmlTkDcuPsuMap.getDcuPsuMaps() ;

#ifdef DEBUGMSGERROR
  std::cout << "TkDcuPsuMapFactory::addFileName: Found " << vDcuPsuMapVector.size() << " elements in the vector" << std::endl ;
  std::cout << "Number of conversion already in memory: " << (vCGDcuPsuMap_.size() + vPGDcuPsuMap_.size()) << std::endl ;
#endif

  // Sort it by control and power group
  for (tkDcuPsuMapVector::iterator it = vDcuPsuMapVector.begin() ; it != vDcuPsuMapVector.end() ; it ++) {
    
    if ((*it)->getPsuType() == PSUDCUTYPE_CG) vCGDcuPsuMap_.push_back(*it) ;
    if ((*it)->getPsuType() == PSUDCUTYPE_PG) vPGDcuPsuMap_.push_back(*it) ;
  }

  // Sort it
  std::sort (vCGDcuPsuMap_.begin(), vCGDcuPsuMap_.end(), TkDcuPsuMap::sortByPsuType) ;
  std::sort (vPGDcuPsuMap_.begin(), vPGDcuPsuMap_.end(), TkDcuPsuMap::sortByPsuType) ;

#ifdef DEBUGMSGERROR
  std::cout << "Number of conversion already in memory after the merge: " << (vCGDcuPsuMap_.size() + vPGDcuPsuMap_.size()) << std::endl ;
#endif

  // All devices are deleted by the XMLFecDevice so nothing must be deleted

#ifdef DATABASE
  useDatabase_ = false ;
#endif
}

/** Set a file as the new input
 * \param inputFileName - new input file
 */
void TkDcuPsuMapFactory::setInputFileName ( std::string inputFileName ) {
 
 // Emptyied the list 
  deleteVectorI (vCGDcuPsuMap_) ;
  deleteVectorI (vPGDcuPsuMap_) ;
  deleteVectorI (vCGPsuNotConnected_) ;
  deleteVectorI (vPGPsuNotConnected_) ;

  // Add new entries
  TkDcuPsuMapFactory::addFileName (inputFileName) ;
}

/** DCU/PSU are provided throw a file built with the following fields:
 * CONTROL CHANNELS
 * 
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard12/channel000,MYTK_TIB_Layer2
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard12/channel003,MYTK_TIB_Layer3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard08/channel000,MYTK_TOB_DOHM
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard09/channel000,MYTK_TEC_DOHM
 * 
 * POWER GROUPS
 * 
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard00,MYTK_TIB_L2_2_1_2_3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard01,MYTK_TIB_L2_2_1_2_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard02,MYTK_TIB_L2_2_2_2_3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard03,MYTK_TIB_L2_2_2_2_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard04,MYTK_TIB_L2_2_2_2_5
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard06,MYTK_TIB_L3_Int_1_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard07,MYTK_TIB_L3_Int_5_8
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard08,MYTK_TIB_L3_Ext_1_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard09,MYTK_TIB_L3_Ext_5_7
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard00,MYTK_TOB_1_1_2_1_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard01,MYTK_TOB_1_1_2_1_5
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard02,MYTK_TOB_1_5_1_2_3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard03,MYTK_TOB_1_5_1_2_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard04,MYTK_TEC_F2
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard05,MYTK_TEC_F3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard06,MYTK_TEC_B2
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard07,MYTK_TEC_B3
 *
 * This method will create the PSU/DCU map regarding this file
 *
 * \exception a FecExceptionHanlder is raised in two cases, the file is invalid or the PSU name is invalid.
*/
void TkDcuPsuMapFactory::setInputTextFile ( std::string inputFileName ) throw (FecExceptionHandler) {

#define CONTROLCHANNELSSTART "CONTROL CHANNELS"
#define POWERGROUPSSTART     "POWER GROUPS"
#define NODCUHARDID 0

  // Delete the previous data
  deleteVectorI (vCGDcuPsuMap_) ;
  deleteVectorI (vPGDcuPsuMap_) ;
  deleteVectorI (vCGPsuNotConnected_) ;
  deleteVectorI (vPGPsuNotConnected_) ;

  // Open the file
  std::ifstream fichierPSU (inputFileName.c_str()) ;  
  
  // File existing ?
  if (!fichierPSU) {
    std::stringstream msgError ; msgError << "the file " << inputFileName << " is empty" ;
    RAISEFECEXCEPTIONHANDLER (FILEPROBLEMERROR, msgError.str(), ERRORCODE) ;
  } 

  try {
    char coucou[1000] ;
    
    // Remove the empty line at the beginning of the files
    do {
      fichierPSU.getline(coucou,1000) ;
    }
    while ( (std::string(coucou) != CONTROLCHANNELSSTART) && (!fichierPSU.eof()) ) ;
    
    if (fichierPSU.eof()) {
      std::stringstream msgError ; msgError << "the file " << inputFileName << " does not contain the power group" ;
      RAISEFECEXCEPTIONHANDLER (FILEPROBLEMERROR, msgError.str(), ERRORCODE) ;
    }
    
    // Get all the control channels
    do {
      fichierPSU.getline(coucou,1000) ;
      if ( (strlen(coucou) != 0) && (std::string(coucou) != POWERGROUPSSTART) ) {
	TkDcuPsuMap *tkDcuPsuMap = new TkDcuPsuMap ( NODCUHARDID, coucou, PSUDCUTYPE_CG ) ;
	vCGDcuPsuMap_.push_back(tkDcuPsuMap) ;
      }
    }
    while ( (std::string(coucou) != POWERGROUPSSTART) && (!fichierPSU.eof()) ) ;

    if (fichierPSU.eof()) {
      std::stringstream msgError ; msgError << "the file " << inputFileName << " does not contain the power group" ;
      RAISEFECEXCEPTIONHANDLER (FILEPROBLEMERROR, msgError.str(), ERRORCODE) ;
    }
    
    // Get all the power groups
    do {
      fichierPSU.getline(coucou,1000) ;
      if (strlen(coucou) != 0) {
	TkDcuPsuMap *tkDcuPsuMap = new TkDcuPsuMap ( NODCUID, coucou, PSUDCUTYPE_PG ) ;
	vPGDcuPsuMap_.push_back(tkDcuPsuMap) ;
      }
    }
    while (!fichierPSU.eof()) ;
    // Continue the parsing until the end of the file

    // Sort it
    std::sort (vCGDcuPsuMap_.begin(), vCGDcuPsuMap_.end(), TkDcuPsuMap::sortByPsuType) ;
    std::sort (vPGDcuPsuMap_.begin(), vPGDcuPsuMap_.end(), TkDcuPsuMap::sortByPsuType) ;    

    // Close the file
    fichierPSU.close() ;
  }
  catch (FecExceptionHandler &e) {
    deleteVectorI (vCGDcuPsuMap_) ;
    deleteVectorI (vPGDcuPsuMap_) ;
    deleteVectorI (vCGPsuNotConnected_) ;
    deleteVectorI (vPGPsuNotConnected_) ;

    if (fichierPSU) fichierPSU.close() ;
    
    throw e ;
  }
}

/** This method write a file for PVSS based on the control and power group contains in this 
 */
void TkDcuPsuMapFactory::setOutputPVSSFile ( tkDcuPsuMapVector dcuPsuMapResult, std::string outputFileName, std::string errorFile ) {

  if (dcuPsuMapResult.size() == 0) {
    RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in file", ERRORCODE) ;
  }

  std::ofstream outputFile (outputFileName.c_str()) ;
  std::ofstream outputErrorFile (errorFile.c_str()) ;
  if (!outputFile) {
    std::stringstream msgError ; msgError << "cannot open the file " << outputFileName ;
    RAISEFECEXCEPTIONHANDLER (FILEPROBLEMERROR, msgError.str(), ERRORCODE) ;
  }
  if (!outputErrorFile) {
    std::stringstream msgError ; msgError << "cannot open the file " << outputFileName ;
    RAISEFECEXCEPTIONHANDLER (FILEPROBLEMERROR, msgError.str(), ERRORCODE) ;
  }
  
  std::map<std::string, std::list<unsigned int> > cgMap ;
  std::map<std::string, std::list<unsigned int> > pgMap ;

  for (tkDcuPsuMapVector::iterator it = dcuPsuMapResult.begin() ; it != dcuPsuMapResult.end() ; it ++) {
    //std::cout << (*it)->getDcuHardId() << std::endl ;
    if ((*it)->getPsuType() == PSUDCUTYPE_CG) cgMap[(*it)->getPVSSName()].push_back((*it)->getDcuHardId()) ;
  }

  for (tkDcuPsuMapVector::iterator it = dcuPsuMapResult.begin() ; it != dcuPsuMapResult.end() ; it ++) {
    //std::cout << (*it)->getDcuHardId() << std::endl ;
    if ((*it)->getPsuType() == PSUDCUTYPE_PG) pgMap[(*it)->getPVSSName()].push_back((*it)->getDcuHardId()) ;
  }

#ifdef DEBUGMSGERROR
  for (std::map<std::string, std::list<unsigned int> >::iterator it = cgMap.begin() ; it != cgMap.end() ; it ++) 
    std::cerr << it->first << COMMA << DCUCCU << COMMA << it->second.size() << std::endl ;

  for (std::map<std::string, std::list<unsigned int> >::iterator it = pgMap.begin() ; it != pgMap.end() ; it ++) 
    std::cerr << it->first << COMMA << DCUCCU << COMMA << it->second.size() << std::endl ;
#endif

  for (std::map<std::string, std::list<unsigned int> >::iterator it = cgMap.begin() ; it != cgMap.end() ; it ++) {
    if (it->second.empty()) {
      outputErrorFile << "Did not find any DCU power by " << it->first << std::endl ;
    }
    else {
      outputFile << it->first << COMMA << DCUCCU << COMMA ;
      for (std::list<unsigned int>::iterator itList = it->second.begin() ; itList != it->second.end() ; itList ++) {
	outputFile << toHEXString(*itList) << COMMA ;
      }
      outputFile << std::endl ;
    }
  }

  for (std::map<std::string, std::list<unsigned int> >::iterator it = pgMap.begin() ; it != pgMap.end() ; it ++) {
    if (it->second.empty()) {
      outputErrorFile << "Did not find any DCU power by " << it->first << std::endl ;
    }
    else {
      outputFile << it->first << COMMA << DCUFEH << COMMA ;
      for (std::list<unsigned int>::iterator itList = it->second.begin() ; itList != it->second.end() ; itList ++) {
	outputFile << toHEXString(*itList) << COMMA ;
      }
      outputFile << std::endl ;
    }
  }
  
  outputFile.close() ;
  outputErrorFile.close() ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** Retreive the version for DCU info given the partition name
 * \param partitionName - partitionName
 * \param major - major version returned
 * \param minor - minor version returned
 * \param partitionNumber - partition number returned
 * The parameters must be deleted by the remote method
 * Note that the method returns always a pointer allocated but the list can be empty
 */
void TkDcuPsuMapFactory::getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *partitionNumber ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

  if (dbAccess_ == NULL) RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

#ifdef DEBUGMSGERROR
  std::cout << "Retreive the current version for the partition " << partitionName << std::endl ;
#endif

  *major = *minor = *partitionNumber = 0 ;
  std::list<unsigned int*> partitionVersion = ((DbTkDcuPsuMapAccess *)dbAccess_)->getDatabaseVersion(partitionName) ;
  if (! partitionVersion.empty()) {
    std::list<unsigned int*>::iterator it = partitionVersion.begin() ;

    unsigned int *value = *it;
    *partitionNumber = value[0] ;
    *major = value[1] ;
    *minor = value[2] ;
    
    int error = partitionVersion.size() ;
    
    for (std::list<unsigned int*>::iterator it = partitionVersion.begin() ; it != partitionVersion.end() ; it ++) {
      delete[] (*it) ;
    }
    
    if (error > 1) {

      std::stringstream msgError ; msgError << "several version for the partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER (DB_INVALIDOPERATION, msgError.str(), ERRORCODE) ;
    }
  }
  else {

    std::stringstream msgError ; msgError << "no version found for the partition " << partitionName ;
    RAISEFECEXCEPTIONHANDLER (DB_INVALIDOPERATION, msgError.str(), ERRORCODE) ;
  }
}

/** 
 * \param login - login to database
 * \param password - password to database
 * \param path - path to database
 * \warning in the destructor of TkDcuPsuMapFactory, the dbAccess is deleted
 */
void TkDcuPsuMapFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) 
  throw ( oracle::occi::SQLException ) {

  // For FEC database delete the old one if needed
  if (dbAccess_ != NULL)
    if (login != dbAccess_->getDbLogin() || password != dbAccess_->getDbPassword() || path != dbAccess_->getDbPath()) {
      delete dbAccess_ ;
      dbAccess_ = NULL ;
    }
  
  if (dbAccess_ == NULL) {
#ifdef DEBUGMSGERROR
    std::cout << "Create a new access to the FEC database for " << login << "/" << password << "@" << path << std::endl ;
#endif
    dbAccess_ = (DbAccess *)new DbTkDcuPsuMapAccess (login, password, path, threaded_) ;
  }

  deleteDbAccess_ = true ;
  useDatabase_ = true ;
}

/** 
 * \param dbAccess - FEC access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void TkDcuPsuMapFactory::setDatabaseAccess ( ) 
  throw ( oracle::occi::SQLException ) {

  std::string login, password, path ;
  if (getDatabaseConfiguration(login, password, path)) {
    setDatabaseAccess (login, password, path) ;
  }
}

/** 
 * \param dbAccess - access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void TkDcuPsuMapFactory::setDatabaseAccess ( DbTkDcuPsuMapAccess *dbAccess ) {

  // For FEC database delete the old one if needed
  DeviceFactoryInterface::setDatabaseAccess ((DbAccess *)dbAccess) ;
}

#endif

// ------------------------------------------------------------------------------------------------------
// 
// DCU info methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** Retreive the DCU/PSU map from the database
 * \param partitionName - partition name
 * \param versionMajorId - version major
 * \param versionMinorId - version minor
 */
void TkDcuPsuMapFactory::getDcuPsuMapPartition (std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler) {

  // Empty the lists
  deleteVectorI (vCGDcuPsuMap_) ;
  deleteVectorI (vPGDcuPsuMap_) ;
  deleteVectorI (vCGPsuNotConnected_) ;
  deleteVectorI (vPGPsuNotConnected_) ;

  bool error = false ;
  std::stringstream psuNameInError ;

  XMLTkDcuPsuMap xmlTkDcuPsuMap;
  xmlTkDcuPsuMap.setDatabaseAccess(dbAccess_) ;
  tkDcuPsuMapVector vDcuPsuMapVector = xmlTkDcuPsuMap.getDcuPsuMap(partitionName,majorVersionId, minorVersionId) ;
  for (tkDcuPsuMapVector::iterator device = vDcuPsuMapVector.begin() ; device != vDcuPsuMapVector.end() ; device ++) {
    if ((*device)->getPsuType() == PSUDCUTYPE_PG) vPGDcuPsuMap_.push_back(*device) ;
    else if ((*device)->getPsuType() == PSUDCUTYPE_CG) vCGDcuPsuMap_.push_back(*device) ;
    else {
      error = true ;
      psuNameInError << "Invalid PSU type for PSU name " << (*device)->getPsuName() << " with type " + (*device)->getPsuType() << std::endl ;
    }
  }

  // Sort it
  std::sort (vCGDcuPsuMap_.begin(), vCGDcuPsuMap_.end(), TkDcuPsuMap::sortByPsuType) ;
  std::sort (vPGDcuPsuMap_.begin(), vPGDcuPsuMap_.end(), TkDcuPsuMap::sortByPsuType) ;    

  if (error) RAISEFECEXCEPTIONHANDLER ( DB_INVALIDOPERATION, psuNameInError.str(), ERRORCODE ) ;
}

/** Retreive the DCU/PSU map from the database
 * \param partitionName - partition name
 * \param versionMajorId - version major
 * \param versionMinorId - version minor
 */
void TkDcuPsuMapFactory::getPsuNotConnectedPartition (std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler) {

  // Empty the list 
  deleteVectorI (vCGPsuNotConnected_) ;
  deleteVectorI (vPGPsuNotConnected_) ;

  bool error = false ;
  std::stringstream psuNameInError ;

  XMLTkDcuPsuMap xmlTkDcuPsuMap;
  xmlTkDcuPsuMap.setDatabaseAccess(dbAccess_) ;
  tkDcuPsuMapVector vDcuPsuMapVector = xmlTkDcuPsuMap.getPsuNotConnected(partitionName,majorVersionId, minorVersionId) ;
  for (tkDcuPsuMapVector::iterator device = vDcuPsuMapVector.begin() ; device != vDcuPsuMapVector.end() ; device ++) {
    if ((*device)->getPsuType() == PSUDCUTYPE_PG) vPGPsuNotConnected_.push_back(*device) ;
    else if ((*device)->getPsuType() == PSUDCUTYPE_CG) vCGPsuNotConnected_.push_back(*device) ;
    else {
      error = true ;
      psuNameInError << "Invalid PSU type for PSU name " << (*device)->getPsuName() << " with type " + (*device)->getPsuType() << std::endl ;
    }
  }

  // Sort it
  std::sort (vCGPsuNotConnected_.begin(), vCGPsuNotConnected_.end(), TkDcuPsuMap::sortByPsuType) ;
  std::sort (vPGPsuNotConnected_.begin(), vPGPsuNotConnected_.end(), TkDcuPsuMap::sortByPsuType) ;    

  if (error) RAISEFECEXCEPTIONHANDLER ( DB_INVALIDOPERATION, psuNameInError.str(), ERRORCODE ) ;
}

/** Retreive the PSU names from the database
 * \param partitionName - partition name
 * \param versionMajorId - version major
 * \param versionMinorId - version minor
 */
void TkDcuPsuMapFactory::getPsuNamePartition (std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler) {

  bool error = false ;
  std::stringstream psuNameInError ;

  XMLTkDcuPsuMap xmlTkDcuPsuMap;
  xmlTkDcuPsuMap.setDatabaseAccess(dbAccess_) ;
  tkDcuPsuMapVector vDcuPsuMapVector = xmlTkDcuPsuMap.getPsuNames(partitionName,majorVersionId, minorVersionId) ;
  for (tkDcuPsuMapVector::iterator device = vDcuPsuMapVector.begin() ; device != vDcuPsuMapVector.end() ; device ++) {
    if ((*device)->getPsuType() == PSUDCUTYPE_PG) vPGDcuPsuMap_.push_back(*device) ;
    else if ((*device)->getPsuType() == PSUDCUTYPE_CG) vCGDcuPsuMap_.push_back(*device) ;
    else {
      error = true ;
      psuNameInError << "Invalid PSU type for PSU name " << (*device)->getPsuName() << " with type " + (*device)->getPsuType() << std::endl ;
    }
  }

  // Sort it
  std::sort (vCGDcuPsuMap_.begin(), vCGDcuPsuMap_.end(), TkDcuPsuMap::sortByPsuType) ;
  std::sort (vPGDcuPsuMap_.begin(), vPGDcuPsuMap_.end(), TkDcuPsuMap::sortByPsuType) ;    

  if (error) RAISEFECEXCEPTIONHANDLER ( DB_INVALIDOPERATION, psuNameInError.str(), ERRORCODE ) ;
}

#endif

/** Set descriptions in database
 * \param partitionName - partition name
 * \param versionMajor - output version major (only for database purpose)
 * \param versionMinor - output version minor (only for database purpose)
 */
void TkDcuPsuMapFactory::setTkDcuPsuMap ( tkDcuPsuMapVector dcuPsuMap, std::string partitionName, unsigned int *versionMajorId, unsigned int *versionMinorId ) throw ( FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if (getDbUsed())
    std::cout << "setTkDcuPsuName for the partition " << partitionName << std::endl ;
  else 
#  endif
    std::cout << "setTkDcuPsuName for the file " << outputFileName_ << std::endl ;
#endif
  
  if (dcuPsuMap.empty()) RAISEFECEXCEPTIONHANDLER ( NODATAAVAILABLE,"no PSU names to be uploaded",ERRORCODE) ;

#ifdef DATABASE
  if ( getDbUsed() && partitionName.size() ) {
    XMLTkDcuPsuMap xmlTkDcuPsuMap;
    xmlTkDcuPsuMap.setDatabaseAccess(dbAccess_) ;
    xmlTkDcuPsuMap.setDbDcuPsuMaps (dcuPsuMap,partitionName,versionMajorId,versionMinorId) ;
  }
  else 
#endif
    {
      XMLTkDcuPsuMap xmlTkDcuPsuMap;
      xmlTkDcuPsuMap.setFileDcuPsuMap(dcuPsuMap,outputFileName_) ;
    }
}

/** Set descriptions in database
 * \param partitionName - partition name
 * \param versionMajor - output version major (only for database purpose)
 * \param versionMinor - output version minor (only for database purpose)
 */
void TkDcuPsuMapFactory::setTkPsuNames ( tkDcuPsuMapVector dcuPsuMap, std::string partitionName, unsigned int *versionMajorId, unsigned int *versionMinorId ) throw ( FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if (getDbUsed())
    std::cout << "setTkDcuPsuName for the partition " << partitionName << std::endl ;
  else 
#  endif
    std::cout << "setTkDcuPsuName for the file " << outputFileName_ << std::endl ;
#endif
  
  if (dcuPsuMap.empty()) RAISEFECEXCEPTIONHANDLER ( NODATAAVAILABLE,"no PSU names to be uploaded",ERRORCODE) ;

#ifdef DATABASE
  if ( getDbUsed()  && partitionName.size() ) {
    XMLTkDcuPsuMap xmlTkDcuPsuMap;
    xmlTkDcuPsuMap.setDatabaseAccess(dbAccess_) ;
    xmlTkDcuPsuMap.setDbPsuNames(dcuPsuMap,partitionName,versionMajorId,versionMinorId) ;
  }
  else 
#endif
    {
      XMLTkDcuPsuMap xmlTkDcuPsuMap;
      xmlTkDcuPsuMap.setFileDcuPsuMap(dcuPsuMap,outputFileName_) ;
    }
}


#ifdef DATABASE

/** This method calls the validation of the cooling loops
 * \param partitionName - partition name
 * \param coolingLoopResult - result of cooling loops (pair with the name and the result (true = ok, false = not ok)
 * \return true if ok, false if not
 */
bool TkDcuPsuMapFactory::checkTKCCCoolingLoop ( std::string partitionName, std::vector<std::pair<std::string, bool> > &coolingLoopResult ) throw ( FecExceptionHandler, oracle::occi::SQLException, std::string ) {

  DbTkDcuPsuMapAccess *dbAccess = (DbTkDcuPsuMapAccess *)this->getDatabaseAccess() ;
  bool result = true ;
  if (dbAccess != NULL) {

    coolingLoopResult = dbAccess->setTKCCDcuPsuMapValidation(partitionName) ;

    for (std::vector<std::pair<std::string, bool> >::iterator it = coolingLoopResult.begin() ; (it != coolingLoopResult.end()) && result ; it ++) {
      std::pair<std::string, bool> maPair = *it ;
      if (!maPair.second) result = false ;
    }
  }
  else {
    throw std::string("No database access created") ;
  }

  return result ;
}

#endif
