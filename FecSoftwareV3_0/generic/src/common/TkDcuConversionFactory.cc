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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
  
  //#define DEBUGMSGERROR

#include <iostream>

#include "stringConv.h"

#include "XMLTkDcuConversion.h"
#include "TkDcuConversionFactory.h"

/** Build a TkDcuConversionFactory to retreive information from database or file
 * \param databaseAccess - if true create a database access
 */
TkDcuConversionFactory::TkDcuConversionFactory ( ):
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
TkDcuConversionFactory::TkDcuConversionFactory ( std::string login, std::string password, std::string path, bool threaded ) 
  throw ( oracle::occi::SQLException ):
  DeviceFactoryInterface ( login, password, path, threaded ) {

  setDatabaseAccess ( login, password, path ) ;
}

/** Create an access to the FEC database
 * \param dbFecAccess - database access
 */
TkDcuConversionFactory::TkDcuConversionFactory ( DbTkDcuConversionAccess *dbAccess ):
  DeviceFactoryInterface ((DbAccess *)dbAccess) {
}

#endif

/** Disconnect the database (if it is set)
 */  
TkDcuConversionFactory::~TkDcuConversionFactory ( ) {
  
  // The database access is deleted in the DeviceFactoryInterface
  // Emptyied the list of convers factors
  deleteHashMapTkDcuConversionFactors () ;
}

/** Delete the hash_map that contains the conversion factor
 */
void TkDcuConversionFactory::deleteHashMapTkDcuConversionFactors ( ) {

  for (Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator itr = vConversionFactors_.begin() ; itr != vConversionFactors_.end() ; itr ++) {

    delete itr->second ;
    itr->second = NULL ;
  }
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 */
void TkDcuConversionFactory::addFileName ( std::string fileName ) {
   // For conversion factors
  XMLTkDcuConversion xmlTkDcuConversion ( fileName ) ;
   
  // Retreive the conversion factors
  dcuConversionVector vDcuConversionVector = xmlTkDcuConversion.getDcuConversions() ;

#ifdef DEBUGMSGERROR
  std::cout << __PRETTY_FUNCTION__ << ": Found " << vDcuConversionVector.size() << " elements in the vector" << std::endl ;
  std::cout << __PRETTY_FUNCTION__ << ": Number of conversion already in memory: " << vConversionFactors_.size() << std::endl ;
#endif

  if ( !vDcuConversionVector.empty() ) {
    // Merge the vector from the class and the new vector
    // vConversionFactors_.merge (*piaReset) ;
    for (dcuConversionVector::iterator device = vDcuConversionVector.begin() ; device != vDcuConversionVector.end() ; device ++) {
      
      if (vConversionFactors_.find((*device)->getDcuHardId()) != vConversionFactors_.end()) delete vConversionFactors_[(*device)->getDcuHardId()] ;
      vConversionFactors_[(*device)->getDcuHardId()] = (*device) ;
    }
  }

#ifdef DEBUGMSGERROR
  std::cout << "Number of conversion already in memory after the merge: " << vConversionFactors_.size() << std::endl ;
#endif

  // All devices are deleted by the XMLFecDevice so nothing must be deleted

#ifdef DATABASE
  useDatabase_ = false ;
#endif
}

/** Set a file as the new input
 * \param inputFileName - new input file
 */
void TkDcuConversionFactory::setInputFileName ( std::string inputFileName ) {

  // delete the old vector that is not more usefull
  deleteHashMapTkDcuConversionFactors() ;

  // Add new entries
  TkDcuConversionFactory::addFileName (inputFileName) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** 
 * \param login - login to database
 * \param password - password to database
 * \param path - path to database
 * \warning in the destructor of TkDcuConversionFactory, the dbAccess is deleted
 */
void TkDcuConversionFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) 
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
    dbAccess_ = (DbAccess *)new DbTkDcuConversionAccess (login, password, path, threaded_) ;
  }

  deleteDbAccess_ = true ;
  useDatabase_ = true ;
}

/** 
 * \param dbAccess - FEC access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void TkDcuConversionFactory::setDatabaseAccess ( ) 
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
void TkDcuConversionFactory::setDatabaseAccess ( DbTkDcuConversionAccess *dbAccess ) {

  // For FEC database delete the old one if needed
  DeviceFactoryInterface::setDatabaseAccess ((DbAccess *)dbAccess) ;
}

#endif

// ------------------------------------------------------------------------------------------------------
// 
// DCU conversion methods
//
// ------------------------------------------------------------------------------------------------------
#  ifdef DATABASE
/** \brief Add the descriptions of the devices from the partitionName
 */
void TkDcuConversionFactory::addConversionPartition ( std::string partitionName )
  throw (FecExceptionHandler) {

  // For conversion factors
  XMLTkDcuConversion xmlTkDcuConversion;
  xmlTkDcuConversion.setDatabaseAccess(dbAccess_) ;
  
  // Retreive the DCU Infos
  dcuConversionVector vDcuConversionVector = xmlTkDcuConversion.getDcuConversions(partitionName) ;
  
#ifdef DEBUGMSGERROR
  std::cout << __PRETTY_FUNCTION__ << ": Found " << vDcuConversionVector.size() << " elements in the vector" << std::endl ;
  std::cout << __PRETTY_FUNCTION__ << ": Number of conversion already in memory: " << vConversionFactors_.size() << std::endl ;
#endif

  if ( !vDcuConversionVector.empty() ) {
    // Merge the vector from the class and the new vector
    for (dcuConversionVector::iterator device = vDcuConversionVector.begin() ; device != vDcuConversionVector.end() ; device ++) {
      if (vConversionFactors_.find((*device)->getDcuHardId()) != vConversionFactors_.end()) delete vConversionFactors_[(*device)->getDcuHardId()] ;
      vConversionFactors_[(*device)->getDcuHardId()] = (*device) ;
    }
  }

#ifdef DEBUGMSGERROR
  std::cout << "Number of dcu conversion factors in memory after the merge: " << vConversionFactors_.size() << std::endl ;
#endif
}

#endif

/** Retreive the descriptions for the given devices from the input
 * \param dcuHardId - DCU hardware ID
 * \param forceDbReload - if these parameter is true then the database is access. if this parameter is false and if the DCU conversion factors is retrieved then no access to the database is done
 * \return return the conversion factors, do not delete the conversion factors returned
 */
TkDcuConversionFactors *TkDcuConversionFactory::getTkDcuConversionFactors ( unsigned long dcuHardId, bool forceDbReload )
  throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if (getDbUsed()) {
    if (forceDbReload || vConversionFactors_.find(dcuHardId) == vConversionFactors_.end())
      std::cout << "Retreive the DCU conversion factors from database for the DCU " << dcuHardId << std::endl ;
    else
      std::cout << "Retreive the DCU conversion factors from the hash_map for the DCU " << dcuHardId << std::endl ;
  }
  else
#  endif
    std::cout << "Retreive the DCU conversion factors from the hash_map for the DCU " << dcuHardId << std::endl ;
#endif

#ifndef DATABASE
  // No database so no reload !
  forceDbReload = false ;
#endif

  // File is used but no conversion factors exist in the map
  if (!getDbUsed() && vConversionFactors_.find(dcuHardId) == vConversionFactors_.end()) {
    std::stringstream msgError ; msgError << "No DCU conversion factors for the DCU " << dcuHardId << " (from file)" << std::endl ;
    RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, msgError.str(), ERRORCODE) ;
  }

  // Database used
#ifdef DATABASE
  // retreive the information from database
  if (getDbUsed() && (forceDbReload || vConversionFactors_.find(dcuHardId) == vConversionFactors_.end())) {

    if (forceDbReload && (vConversionFactors_.find(dcuHardId) != vConversionFactors_.end())) { // delete the orginal value
      delete vConversionFactors_[dcuHardId] ;
      vConversionFactors_[dcuHardId] = NULL ;
    }

    // XML parser creation
    XMLTkDcuConversion xmlTkDcuConversion ;
    xmlTkDcuConversion.setDatabaseAccess(dbAccess_) ;

    // Retreive the vector
    dcuConversionVector v = xmlTkDcuConversion.getDcuConversions(dcuHardId) ;

    if (v.size() == 0 || v.size() > 1) { // An error is normmally set the DCU database access and XML parser

      std::stringstream msgError ; 
      if (v.size() == 0)
	msgError << "No DCU conversion factors for the DCU " << dcuHardId ;
      else {

	// delete it
	while (v.size() > 0) { delete v.back(); v.pop_back(); }

	// error message
	msgError << "Several conversion factors exists for the DCU " << dcuHardId ;	msgError << "Several conversion factors exists for the DCU " << dcuHardId << std::endl ;
      }
      
      RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, msgError.str(), ERRORCODE) ;
    }

    dcuConversionVector::iterator element = v.begin() ;
    
    vConversionFactors_[dcuHardId] = (*element) ;
  }
#endif

  return (vConversionFactors_[dcuHardId]) ;
}

/** This method upload a new conversion factors in the current map
 * \param tkConversionFactors - the conversion factors
 * \warning this method create a clone of the conversion factors passed by argument
 * \exception an exception is raised if the conversion factors is already present in the map
 */
void TkDcuConversionFactory::setTkDcuConversionFactors ( TkDcuConversionFactors tkDcuConversionFactors ) 
  throw ( FecExceptionHandler ) {

  if (vConversionFactors_.find(tkDcuConversionFactors.getDcuHardId()) == vConversionFactors_.end()) 
    vConversionFactors_[tkDcuConversionFactors.getDcuHardId()] = tkDcuConversionFactors.clone() ;
  else {
    std::stringstream msgError ; 
    msgError << "A DCU conversion factor is already exiting for DCU " << std::dec << tkDcuConversionFactors.getDcuHardId() ;
    RAISEFECEXCEPTIONHANDLER (DUPLICATEDINFORMATION, msgError.str(), ERRORCODE) ;
  }
}


/** Upload the description in the output (database or file)
 * \param dcuConversionVector - a vector of conversion factor
 */
void TkDcuConversionFactory::setTkDcuConversionFactors ( dcuConversionVector vDcuConversion ) 
  throw ( FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if (getDbUsed())
    std::cout << "setTkDcuConversionFactors: set " << vDcuConversion.size() << " DCU conversion factors in the database" << std::endl ;
  else 
#  endif
    std::cout << "setTkDcuConversionFactors set " << vDcuConversion.size() << " DCU conversion factors in the file " << outputFileName_ << std::endl ;
#endif

  if (vDcuConversion.empty()) RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded ", ERRORCODE) ;

#ifdef DATABASE
  if (getDbUsed()) {

    // XML parser creation
    XMLTkDcuConversion xmlTkDcuConversion ;
    xmlTkDcuConversion.setDatabaseAccess(dbAccess_) ;

    xmlTkDcuConversion.setDcuConversions(vDcuConversion) ;
  }
  else
#endif
    {  // Upload in file
      XMLTkDcuConversion xmlTkDcuConversion ;
      xmlTkDcuConversion.setDcuConversions(vDcuConversion, outputFileName_) ;
    }
}

/** Upload the description in the output (database or file)
 * \param vConversionFactors - a hash_map of conversion factors
 */
void TkDcuConversionFactory::setTkDcuConversionFactors ( Sgi::hash_map<unsigned long, TkDcuConversionFactors *> vConversionFactors ) 
  throw ( FecExceptionHandler ) {

  dcuConversionVector vDcuConversion ;
  for (Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator itr = vConversionFactors.begin() ; itr != vConversionFactors.end() ; itr ++) {
    if (itr->second != NULL) vDcuConversion.push_back (itr->second) ;
  }

  setTkDcuConversionFactors (vDcuConversion) ;
}

/** Upload the description in the output (database or file) with the hash_map attribut of the class
 */
void TkDcuConversionFactory::setTkDcuConversionFactors ( ) 
  throw ( FecExceptionHandler ) {
  setTkDcuConversionFactors (vConversionFactors_) ;
}



