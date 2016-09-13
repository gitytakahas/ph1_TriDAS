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
#include "TkIdVsHostnameFactory.h"
#include "XMLTkIdVsHostname.h"
//#include "stringConv.h"

/** Build a TkIdVsHostname factory and if database is set create a database access
 */
TkIdVsHostnameFactory::TkIdVsHostnameFactory ( ): DeviceFactoryInterface(), versionMajorId_(0), versionMinorId_(0), hostname_("") {

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

/** Build a TkIdVsHostname factory from database
 */
TkIdVsHostnameFactory::TkIdVsHostnameFactory ( std::string login, std::string password, std::string path, bool threaded )
  throw ( oracle::occi::SQLException ): 
  DeviceFactoryInterface ( login, password, path, threaded ), versionMajorId_(0), versionMinorId_(0), hostname_("") {
  
  setDatabaseAccess ( login, password, path ) ;
}

/** Build a TkIdVsHostname factory for database
 */
TkIdVsHostnameFactory::TkIdVsHostnameFactory ( DbTkIdVsHostnameAccess *dbAccess ):
  DeviceFactoryInterface ((DbCommonAccess *)dbAccess), versionMajorId_(0), versionMinorId_(0), hostname_("") {
}

#endif

/** Disconnect the database (if it is set)
 */  
TkIdVsHostnameFactory::~TkIdVsHostnameFactory ( ) {

  // Delete the corresponding hash_map
  deleteHashMapTkIdVsHostname() ;
}

/** delete the hash_map
 */
void TkIdVsHostnameFactory::deleteHashMapTkIdVsHostname ( ) {

  for ( HashMapTkIdVsHostnameType::iterator it = vIdVsHostname_.begin() ; it != vIdVsHostname_.end() ; it ++) delete it->second ;
  vIdVsHostname_.clear() ;
}

/** Generate the the hash_map
 * \param v - vector of TkIdVsHostnameDescription
 */
void TkIdVsHostnameFactory::generateHashMapFromVector ( TkIdVsHostnameVector &v ) {

  for ( TkIdVsHostnameVector::iterator it = v.begin() ; it != v.end() ; it ++ )  {

    TkIdVsHostnameDescription *id = *it ;
    std::pair<std::string, unsigned int> mk = make_pair(StrX::StringToLower(id->getHostname()), id->getSlot()) ;
    if (vIdVsHostname_.find(mk) != vIdVsHostname_.end()) delete vIdVsHostname_[mk] ;
    vIdVsHostname_[mk] = id ;
//     // Add the sub-detector in 0 for all the hostname 
//     std::pair<std::string, unsigned int> mk1 = make_pair(StrX::StringToLower(id->getHostname()), 0) ;
//     if (vIdVsHostname_.find(mk1) == vIdVsHostname_.end()) 
//       vIdVsHostname_[mk1] = new TkIdVsHostnameDescription (id->getHostname(),id->getCrateId(),0,id->getSubDetector(),0, id->getCrateNumber()) ;
  }
}

/** Generate the the hash_map
 * \param v - vector of TkIdVsHostnameDescription
 * \param hostname - the information can be filtered by hostname (default ALL)
 */
TkIdVsHostnameVector TkIdVsHostnameFactory::generateVectorFromHashMap ( HashMapTkIdVsHostnameType &h, std::string hostname ) {

  TkIdVsHostnameVector v ;
  for ( HashMapTkIdVsHostnameType::iterator it = h.begin() ; it != h.end() ; it ++) {
    if ( (it->second != NULL) &&                                // not NULL
	 ((hostname == "ALL") || (hostname_ == hostname)) )     // all hostname or only the hostname given by argument
      v.push_back(it->second) ;
  }

  return v ;
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Add a new file name in the descriptions
 */
void TkIdVsHostnameFactory::addFileName ( std::string fileName ) throw (FecExceptionHandler) {

  // Parsing
  XMLTkIdVsHostname xmlTkIdVsHostname ( fileName ) ;

  // Retreive from the file
  TkIdVsHostnameVector v = xmlTkIdVsHostname.getAllTkIdVsHostnameFromFile() ;
  generateHashMapFromVector (v) ;

#ifdef DEBUGMSGERROR
  std::cout << "Number of ID vs hostname already in memory after the merge: " << v.size() << std::endl ;
#endif

  // All devices are deleted by the XMLFecDevice so nothing must be deleted

#ifdef DATABASE
  useDatabase_ = false ;
#endif
}

/** set a new input file
 */
void TkIdVsHostnameFactory::setInputFileName ( std::string inputFileName ) throw (FecExceptionHandler) {

  // delete the old data
  deleteHashMapTkIdVsHostname() ;

  // Add new entries
  TkIdVsHostnameFactory::addFileName (inputFileName) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** Create the database access
 */
void TkIdVsHostnameFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) 
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
    dbAccess_ = (DbCommonAccess *)new DbTkIdVsHostnameAccess (login, password, path, threaded_) ;
  }
  
  deleteDbAccess_ = true ;
  useDatabase_ = true ;
}
  
/** Create the database access with the configuration given by the env. variable CONFDB
 */
void TkIdVsHostnameFactory::setDatabaseAccess ( ) throw ( oracle::occi::SQLException ) {

  std::string login, password, path ;
  if (getDatabaseConfiguration(login, password, path)) {
    setDatabaseAccess (login, password, path) ;
  }
}

/** Set the database access
 */
void TkIdVsHostnameFactory::setDatabaseAccess ( DbTkIdVsHostnameAccess *dbAccess ) {

  // For database delete the old one if needed
  DeviceFactoryInterface::setDatabaseAccess ((DbCommonAccess*)dbAccess) ;
}

#endif

// ------------------------------------------------------------------------------------------------------
// 
// Conversion Factors download and upload
//
// ------------------------------------------------------------------------------------------------------

/** Retreive the descriptions for the given devices from the input
 * \param versionMajorId - version major (0 by default)
 * \param versionMajorId - version minor (0 by default)
 * \param forceDbReload - reload from DB
 */
TkIdVsHostnameVector TkIdVsHostnameFactory::getAllTkIdVsHostname ( unsigned int versionMajorId, unsigned int versionMinorId, bool forceDbReload ) 
#ifdef DATABASE
  throw ( FecExceptionHandler, oracle::occi::SQLException  ) {
#else
  throw ( FecExceptionHandler ) {
#endif

#ifdef DEBUGMSGERROR
  std::cout << "TkIdVsHostnameFactory::getAllTkIdVsHostname: retreive all ID/VS hostname for version " << versionMajorId << "." << versionMinorId << std::endl ;
#endif

  TkIdVsHostnameVector v ;

#ifdef DATABASE
  if (getDbUsed()) {

    bool downloadFromDb = true ;

    // retreive the version from database
    unsigned int currVersionMajor = 0, currVersionMinor = 0 ;

    ((DbTkIdVsHostnameAccess *)dbAccess_)->getTkIdVsHostnameVersion(currVersionMajor,currVersionMinor) ;
    // check the version
    if (!forceDbReload) {
      if (hostname_ == "ALL") {
	// Same version in memory and in DB
	if (versionMajorId == 0) {
	  if ((currVersionMajor == versionMajorId_) && (currVersionMinor == versionMinorId_)) downloadFromDb = false ;
	}
	else if ( (versionMajorId_ == versionMajorId) && (versionMinorId_ == versionMinorId) ) downloadFromDb = false ;
      }
    }

    // Download from DB
    if (downloadFromDb) {

      // in case of error reset the parameters
      versionMajorId_ = versionMinorId_ = 0 ;
      hostname_ = "" ; 
      deleteHashMapTkIdVsHostname() ; // clear all

      XMLTkIdVsHostname xmlTkIdVsHostname ;
      xmlTkIdVsHostname.setDatabaseAccess (dbAccess_) ;

      v = xmlTkIdVsHostname.getAllTkIdVsHostname ( versionMajorId, versionMinorId ) ;
      deleteHashMapTkIdVsHostname() ; // clear all
      generateHashMapFromVector ( v ) ;

      if (versionMajorId != 0) {
	versionMajorId_ = versionMajorId ;
	versionMinorId_ = versionMinorId ;
      }
      else {
	versionMajorId_ = currVersionMajor ;
	versionMinorId_ = currVersionMinor ;
      }

      hostname_ = "ALL" ;
    }
    else {

      // Build a vector based on the hash_map and return it
      v = generateVectorFromHashMap ( vIdVsHostname_ ) ;
    }
  }
  else 
#endif
    {
      // Build a vector based on the hash_map and return it
      v = generateVectorFromHashMap ( vIdVsHostname_ ) ;
    }

  return v ;
}

/** Retreive the descriptions for the given devices from the input
 * \param versionMajorId - version major (0 by default)
 * \param versionMajorId - version minor (0 by default)
 * \param forceDbReload - reload from DB
 */
TkIdVsHostnameVector TkIdVsHostnameFactory::getAllTkIdFromHostname ( std::string hostname, unsigned int versionMajorId, unsigned int versionMinorId, bool forceDbReload ) 
#ifdef DATABASE
  throw ( FecExceptionHandler, oracle::occi::SQLException  ) {
#else
  throw ( FecExceptionHandler ) {
#endif

  TkIdVsHostnameVector v ;

#ifdef DATABASE
  if (getDbUsed()) {

    bool downloadFromDb = true ;

    // retreive the version from database
    unsigned int currVersionMajor = 0, currVersionMinor = 0 ;

    ((DbTkIdVsHostnameAccess *)dbAccess_)->getTkIdVsHostnameVersion(currVersionMajor,currVersionMinor) ;
    // check the version
    if (!forceDbReload) {
      // same hostname or all hosts have been downloaded
      if ((hostname_ == "ALL") || (hostname_ == hostname)) {
	// Same version in memory and in DB
	if (versionMajorId == 0) {
	  if ((currVersionMajor == versionMajorId_) && (currVersionMinor == versionMinorId_)) downloadFromDb = false ;
	}
	else if ( (versionMajorId_ == versionMajorId) && (versionMinorId_ == versionMinorId) ) downloadFromDb = false ;
      }
    }

    // Download from DB
    if (downloadFromDb) {

      // in case of error reset the parameters
      versionMajorId_ = versionMinorId_ = 0 ;
      hostname_ = "" ; 
      deleteHashMapTkIdVsHostname() ; // clear all
      
      XMLTkIdVsHostname xmlTkIdVsHostname ;
      xmlTkIdVsHostname.setDatabaseAccess (dbAccess_) ;

      v = xmlTkIdVsHostname.getAllTkIdFromHostname ( hostname, versionMajorId, versionMinorId ) ;
      generateHashMapFromVector ( v ) ;

      if (versionMajorId != 0) {
	versionMajorId_ = versionMajorId ;
	versionMinorId_ = versionMinorId ;
	hostname_ = hostname ;
      }
      else {
	versionMajorId_ = currVersionMajor ;
	versionMinorId_ = currVersionMinor ;
	hostname_ = hostname ;
      }
    }
    else {

      // Build a vector based on the hash_map and return it
      v = generateVectorFromHashMap ( vIdVsHostname_, hostname ) ;
    }
  }
  else 
#endif
    {
      // Build a vector based on the hash_map and return it
      v = generateVectorFromHashMap ( vIdVsHostname_, hostname ) ;
    }

  return v ;
}

/** Retreive the descriptions for the given devices from the input
 * \param versionMajorId - version major (0 by default)
 * \param versionMajorId - version minor (0 by default)
 * \param forceDbReload - reload from DB
 * \warning this method download all the ID versus hostname for the hostname specified
 */
TkIdVsHostnameDescription *TkIdVsHostnameFactory::getAllTkIdFromHostnameSlot ( std::string hostname, unsigned int slot, unsigned int versionMajorId, unsigned int versionMinorId, bool forceDbReload ) 
#ifdef DATABASE
  throw ( FecExceptionHandler, oracle::occi::SQLException  ) {
#else
  throw ( FecExceptionHandler ) {
#endif

  TkIdVsHostnameDescription *tkIdVsHostnameDescription = NULL ;

#ifdef DATABASE
  if (getDbUsed()) {

    bool downloadFromDb = true ;

    // retreive the version from database
    unsigned int currVersionMajor = 0, currVersionMinor = 0 ;

    ((DbTkIdVsHostnameAccess *)dbAccess_)->getTkIdVsHostnameVersion(currVersionMajor,currVersionMinor) ;
    // check the version
    if (!forceDbReload) {
      // same hostname or all hosts have been downloaded
      if ((hostname_ == "ALL") || (hostname_ == hostname)) {
	// Same version in memory and in DB
	if (versionMajorId == 0) {
	  if ((currVersionMajor == versionMajorId_) && (currVersionMinor == versionMinorId_)) downloadFromDb = false ;
	}
	else if ( (versionMajorId_ == versionMajorId) && (versionMinorId_ == versionMinorId) ) downloadFromDb = false ;
      }
    }

    // Download from DB
    if (downloadFromDb) getAllTkIdFromHostname (hostname, versionMajorId, versionMinorId, forceDbReload) ;

    std::pair<std::string, unsigned int> mk = make_pair(StrX::StringToLower(hostname), slot) ;
    tkIdVsHostnameDescription = vIdVsHostname_[mk] ;
  }
  else 
#endif
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  return tkIdVsHostnameDescription ;
}

/** Retreive the descriptions for the given devices from the input
 * \param versionMajorId - version major (0 by default)
 * \param versionMajorId - version minor (0 by default)
 * \param forceDbReload - reload from DB
 */
void TkIdVsHostnameFactory::getSubDetectorCrateNumberFromHostname ( std::string hostname, std::string &subDetector, unsigned int &crateNumber, unsigned int versionMajorId, unsigned int versionMinorId, bool forceDbReload ) 
#ifdef DATABASE
  throw ( FecExceptionHandler, oracle::occi::SQLException  ) {
#else
  throw ( FecExceptionHandler ) {
#endif

  // We consider here that the subdetector cannot change between two uploads
  std::pair<std::string, unsigned int> mk = make_pair(StrX::StringToLower(hostname), 0) ;
  if ( (vIdVsHostname_.find(mk) != vIdVsHostname_.end()) && !forceDbReload ) {
    subDetector = vIdVsHostname_[mk]->getSubDetector() ;
    crateNumber = vIdVsHostname_[mk]->getCrateNumber() ;
  }
  else 
#ifdef DATABASE
    if (getDbUsed()) {

      // Download it from database
      ((DbTkIdVsHostnameAccess *)dbAccess_)->getSubDetectorCrateNumber(hostname,subDetector,crateNumber,versionMajorId,versionMinorId) ;
    }  
    else 
#endif
      RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, "No download has been done, cannot retreive the sub-detector", ERRORCODE) ;
  }

/** Upload a new version of TkIdVsHostnameDescriptions
 */
void TkIdVsHostnameFactory::setTkIdVsHostnameDescription ( TkIdVsHostnameVector tkDcuConversionFactors, bool major ) 
  throw ( FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if (getDbUsed())
    std::cout << "setTkIdVsHostnameDescription: set " << tkDcuConversionFactors.size() << " DCU conversion factors in the database" << std::endl ;
  else 
#  endif
    std::cout << "setTkIdVsHostnameDescription set " << tkDcuConversionFactors.size() << " DCU conversion factors in the file " << outputFileName_ << std::endl ;
#endif

  if (tkDcuConversionFactors.empty()) RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, "No ID/hostnane information found to be uploaded", ERRORCODE) ;

#ifdef DATABASE
  if (getDbUsed()) {
    
    // XML parser creation
    XMLTkIdVsHostname xmlTkIdVsHostname ;
    xmlTkIdVsHostname.setDatabaseAccess(dbAccess_) ;
    
    xmlTkIdVsHostname.setTkIdVsHostnameVector( tkDcuConversionFactors, major ) ;
  }
  else
#endif
    { // Upload in file
      XMLTkIdVsHostname xmlTkIdVsHostname ;
      xmlTkIdVsHostname.setTkIdVsHostnameVector ( tkDcuConversionFactors, outputFileName_) ;
    }
}

/** upload the hash_map (attribut of the class) in the output
 */
void TkIdVsHostnameFactory::setTkIdVsHostnameDescription ( bool major  ) 
  throw ( FecExceptionHandler ) {

  TkIdVsHostnameVector v = generateVectorFromHashMap ( vIdVsHostname_ ) ;
  TkIdVsHostnameFactory::setTkIdVsHostnameDescription ( v ) ;
}
 
/** Find the crate id for a given FED software ID
 * fedId - FED software ID
 * \return the FED crate
 * \warning the information should be present in the memory before calling it
 */
unsigned int TkIdVsHostnameFactory::getFedCrate ( unsigned int fedId ) throw ( FecExceptionHandler ) {

  unsigned int crateId = 0xFFFFFFFF ;

  for (HashMapTkIdVsHostnameType::iterator it = vIdVsHostname_.begin() ; (it != vIdVsHostname_.end()) && (crateId == 0xFFFFFFFF) ; it ++) {
    TkIdVsHostnameDescription *id = it->second ;
    if (id->getFedId() == fedId) crateId = id->getCrateNumber() ;
  }

  if (crateId == 0xFFFFFFFF) {
    std::stringstream invalidValue ; 
    invalidValue << "Unknown FED for FED software Id " << fedId ;
    RAISEFECEXCEPTIONHANDLER (DB_INVALIDOPERATION, invalidValue.str(), ERRORCODE) ;
  }

  return crateId ;
}
 
/** Find the crate id for a given FED software ID
 * fedId - FED software ID
 * \return the FED slot
 * \warning the information should be present in the memory before calling it
 */
unsigned int TkIdVsHostnameFactory::getFedSlot ( unsigned int fedId ) throw ( FecExceptionHandler ) {

  unsigned int crateSlot = 0xFFFFFFFF ;

  for (HashMapTkIdVsHostnameType::iterator it = vIdVsHostname_.begin() ; (it != vIdVsHostname_.end()) && (crateSlot == 0xFFFFFFFF) ; it ++) {
    TkIdVsHostnameDescription *id = it->second ;
    if (id->getFedId() == fedId) crateSlot = id->getSlot() ;
  }

  if (crateSlot == 0xFFFFFFFF) {
    std::stringstream invalidValue ; 
    invalidValue << "Unknown FED for FED software Id " << fedId ;
    RAISEFECEXCEPTIONHANDLER (DB_INVALIDOPERATION, invalidValue.str(), ERRORCODE) ;
  }

  return crateSlot ;
}
 
