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

#include "XMLTkDcuInfo.h"
#include "TkDcuInfoFactory.h"

/** Build a TkDcuInfoFactory to retreive information from database or file
 * \param databaseAccess - if true create a database access
 */
TkDcuInfoFactory::TkDcuInfoFactory ( ):
  DeviceFactoryInterface ( ),
  initDbVersion_(false), partitionName_("NONE"), versionMajor_(0), versionMinor_(0) {

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
TkDcuInfoFactory::TkDcuInfoFactory ( std::string login, std::string password, std::string path, bool threaded ) throw ( oracle::occi::SQLException ): 
  
  DeviceFactoryInterface ( login, password, path, threaded ), 
  initDbVersion_(false), partitionName_("NONE"), versionMajor_(0), versionMinor_(0) {

  setDatabaseAccess ( login, password, path ) ;
}

/** Create an access to the FEC database
 * \param dbFecAccess - database access
 */
TkDcuInfoFactory::TkDcuInfoFactory ( DbTkDcuInfoAccess *dbAccess ):
  DeviceFactoryInterface ((DbAccess *)dbAccess),
  initDbVersion_(false), partitionName_("NONE"), versionMajor_(0), versionMinor_(0) {
}

#endif

/** Disconnect the database (if it is set)
 */  
TkDcuInfoFactory::~TkDcuInfoFactory ( ) {
  
  // The database access is deleted in the DeviceFactoryInterface
  
  // Emptyied the list of convers factors
  deleteHashMapTkDcuInfo () ;
  deleteRunVectorI (runs_) ;
  deleteStateVectorI (dbStates_) ;
  deleteVersionVectorI (fedVersions_) ;
  deleteVersionVectorI (dcuInfoVersions_) ;
  deleteVersionVectorI (fecVersions_) ;
  deleteVersionVectorI (connectionVersions_) ;
  deleteVersionVectorI (dcuPsuVersions_) ;
  deleteVersionVectorI (maskVersions_) ;
}

/** Delete the hash_map that contains the conversion factor
 */
void TkDcuInfoFactory::deleteHashMapTkDcuInfo ( ) {

  for (Sgi::hash_map<unsigned long, TkDcuInfo *>::iterator itr = vDcuInfo_.begin() ; itr != vDcuInfo_.end() ; itr ++) {
    delete itr->second ;
  }
  vDcuInfo_.clear() ;
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 * \warning this method does not clean the cache so it will accumulate the det id from the two files. If you want to have only file, please use the method setInputFileName
 * \see TkDcuInfoFactory::setInputFileName
 */
void TkDcuInfoFactory::addFileName ( std::string fileName ) {

  // For conversion factors
  XMLTkDcuInfo xmlTkDcuInfo ( fileName ) ;
  
  // Retreive the conversion factors
  tkDcuInfoVector vDcuInfoVector = xmlTkDcuInfo.getDcuInfos() ;

#ifdef DEBUGMSGERROR
  std::cout << "TkDcuInfoFactory::addFileName: Found " << vDcuInfoVector.size() << " elements in the vector" << std::endl ;
  std::cout << "Number of det id already in memory: " << vDcuInfo_.size() << std::endl ;
#endif

  if ( !vDcuInfoVector.empty() ) {
    // Merge the vector from the class and the new vector
    // vDcuInfo_.merge (*piaReset) ;
    for (tkDcuInfoVector::iterator device = vDcuInfoVector.begin() ; device != vDcuInfoVector.end() ; device ++) {
      if (vDcuInfo_.find((*device)->getDcuHardId()) != vDcuInfo_.end()) delete vDcuInfo_[(*device)->getDcuHardId()] ;
      vDcuInfo_[(*device)->getDcuHardId()] = *device ;
    }
  }

#ifdef DEBUGMSGERROR
  std::cout << "Number of conversion already in memory after the merge: " << vDcuInfo_.size() << std::endl ;
#endif
  
  // All devices are deleted by the XMLFecDevice so nothing must be deleted
  
#ifdef DATABASE
  useDatabase_ = false ;
#endif
}

/** Set a file as the new input
 * \param inputFileName - new input file
 */
void TkDcuInfoFactory::setInputFileName ( std::string inputFileName ) {

  // delete the old vector that is not more usefull
  deleteHashMapTkDcuInfo() ;

  // Add new entries
  TkDcuInfoFactory::addFileName (inputFileName) ;
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
 * \warning in the destructor of TkDcuInfoFactory, the dbAccess is deleted
 */
void TkDcuInfoFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) 
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
    dbAccess_ = (DbAccess *)new DbTkDcuInfoAccess (login, password, path, threaded_) ;
  }

  deleteDbAccess_ = true ;
  useDatabase_ = true ;
}

/** 
 * \param dbAccess - FEC access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void TkDcuInfoFactory::setDatabaseAccess ( ) 
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
void TkDcuInfoFactory::setDatabaseAccess ( DbTkDcuInfoAccess *dbAccess ) {

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

/** Retreive the version for DCU info given the partition name
 * \param partitionName - partitionName
 * \param major - major version returned
 * \param minor - minor version returned
 * \param partitionNumber - partition number returned
 * The parameters must be deleted by the remote method
 * Note that the method returns always a pointer allocated but the list can be empty
 */
void TkDcuInfoFactory::getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *partitionNumber ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

  if (dbAccess_ == NULL) {
    RAISEFECEXCEPTIONHANDLER(DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE ) ;
  }

#ifdef DEBUGMSGERROR
  std::cout << "Retreive the current version for the partition " << partitionName << std::endl ;
#endif

  *major = *minor = *partitionNumber = 0 ;
  std::list<unsigned int*> partitionVersion ;
  if (partitionName == "ALL") // all det id from the last version
    partitionVersion = ((DbTkDcuInfoAccess *)dbAccess_)->getDatabaseVersion() ;
  else
    partitionVersion = ((DbTkDcuInfoAccess *)dbAccess_)->getDatabaseVersion(partitionName) ;

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

      std::stringstream msgError ; msgError << "Several version for the partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER ( DB_INVALIDOPERATION, msgError.str(), ERRORCODE ) ;
    }
  }
  else {

    std::stringstream msgError ; msgError << "No version found for the partition " << partitionName ;
    RAISEFECEXCEPTIONHANDLER ( DB_INVALIDOPERATION, msgError.str(), ERRORCODE) ;
  }
}

/** \brief Update the channel delays (coarse and fine) according to the fibre length between detector and FED
*/
void TkDcuInfoFactory::updateChannelDelays (std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler){

  if (getDbConnected()) {
    ((DbTkDcuInfoAccess *)dbAccess_)->updateChannelDelays(partitionName);
  }
  else 
    RAISEFECEXCEPTIONHANDLER( DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE ) ;
}

/** \brief Get all the current states
 *  \return An array of TkState. Do not delete the vector returned, it is managed by the current class
 */
tkStateVector &TkDcuInfoFactory::getCurrentStates () throw (FecExceptionHandler){

  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    // delete the previous state
    if (dbStates_.size()) deleteStateVectorI (dbStates_) ;

    dbStates_ = xmlTkDcuInfo.getAllCurrentStates();
    std::sort(dbStates_.begin(),dbStates_.end(),TkState::sortByPartitionName) ;
    return dbStates_;
  }
  else 
    RAISEFECEXCEPTIONHANDLER(DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE ) ;
}

/** \brief Create a new current state with the current State and the given version of the given partition
 *  \param partitionName The partition to add/update
 *  \param stateName the reference state for the partition
 *  \return The new currentStateId
 */
unsigned int TkDcuInfoFactory::setCurrentState(std::string partitionName, std::string stateName) throw (FecExceptionHandler){

  unsigned int updatedStateId;

  if (getDbConnected()) {
    try{
      unsigned int newCurrentState = dbAccess_->getStateHistoryId(stateName);
      
      updatedStateId = dbAccess_->setCurrentState(partitionName, newCurrentState);
      dbAccess_->commit();
      return updatedStateId;
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, what("Unable to set the current state for partition " + partitionName + " and state name " + stateName, e), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e){
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER(DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE ) ;
  }
}

/** \brief Create a new current state
 *  \param states A list of state
 *  \return The new currentStateId
 */
unsigned int TkDcuInfoFactory::setCurrentState(tkStateVector states) throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      unsigned int newStateId = dbAccess_->getNewStateHistoryId();

      for ( tkStateVector::iterator it = states.begin() ; it != states.end() ; it ++) {
	dbAccess_->createStateHistory(newStateId, (*it)->getStateName(), (*it)->getPartitionName(), 
				      (*it)->getFecVersionMajorId(), (*it)->getFecVersionMinorId(), 
				      (*it)->getFedVersionMajorId(), (*it)->getFedVersionMinorId(), 
				      (*it)->getConnectionVersionMajorId(), (*it)->getConnectionVersionMinorId(), 
				      (*it)->getDcuInfoVersionMajorId(), (*it)->getDcuInfoVersionMinorId(), 
				      (*it)->getDcuPsuMapVersionMajorId(), (*it)->getDcuPsuMapVersionMinorId(),
				      (*it)->getMaskVersionMajorId(), (*it)->getMaskVersionMinorId() ) ;
      }
      //dbAccess_->setCurrentState(newStateId);
      dbAccess_->commit();
      return newStateId;
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER(DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE ) ;
  }
}

/** \brief Set the given state as current
 *  \param stateName The name of the chosen state
 *  \return The new current state ID
 */
unsigned int TkDcuInfoFactory::setCurrentState(std::string stateName) throw (FecExceptionHandler){
  unsigned int updatedStateId;
  if (getDbConnected()) {
    try{
      unsigned int newCurrentState = dbAccess_->getStateHistoryId(stateName);
      updatedStateId = dbAccess_->setCurrentState(newCurrentState);
      dbAccess_->commit();
      return updatedStateId;
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** Copy the current state and update it following the state for the runnumber
 * \param runNumber - run number
 * \param allPartition - copy or update/insert of the state coming from the run number
 * \return The new current state ID
 * \see DbCommonAccess::copyStateForRunNumber(unsigned int runNumber, bool allPartition)
 */
unsigned int TkDcuInfoFactory::copyStateForRunNumber(unsigned int runNumber, bool allPartition) throw (FecExceptionHandler) {

  if (getDbConnected()) {
    try{
      unsigned int newCurrentState = dbAccess_->copyStateForRunNumber(runNumber,allPartition);
      dbAccess_->commit();
      return newCurrentState;
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** \brief Disable the devices linked to the dcuHardId in the given partition
 *  \param partitionName The name of the chosen partition
 *  \return The list of dcuHardId to disable
 */  
void TkDcuInfoFactory::disableDevice(std::string partitionName, std::vector<unsigned int> dcuHardId) throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      dbAccess_->setDeviceState(partitionName, dcuHardId, 0);//0-> disable the devices
      dbAccess_->commit();
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}


/** \brief Enable the devices linked to the dcuHardId in the given partition
 *  \param partitionName The name of the chosen partition
 *  \return The list of dcuHardId to disable
 */  
void TkDcuInfoFactory::enableDevice(std::string partitionName, std::vector<unsigned int> dcuHardId) throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      dbAccess_->setDeviceState(partitionName, dcuHardId, 1);//1-> enable the devices
      dbAccess_->commit();
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** \brief Create a new run
 *  \param partitionName The partition used for the run
 *  \param runNumber The number of the run
 *  \param runMode The mode of the run
 */

void TkDcuInfoFactory::setRun(std::string partitionName, unsigned int runNumber, int runMode, int local, std::string comment) throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      dbAccess_->setRun(partitionName, runNumber, runMode, local, comment);
      dbAccess_->commit();
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}


 /** \brief Stop the current run on the partition
   *  \param partitionName The partition used for the run
   *  \param comment Modify the comment on the run if filled
   */
void TkDcuInfoFactory::stopRun(std::string partitionName, std::string comment) throw (FecExceptionHandler){
if (getDbConnected()) {
    try{
      dbAccess_->stopRun(partitionName, comment);
      dbAccess_->commit();
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}


/** \brief Tag a run as transfered by O2O
 *  \param partitionName The partition used for the run
 *  \param runNumber The number of the run
 */

void TkDcuInfoFactory::setO2ORun(std::string partitionName, unsigned int runNumber) throw (FecExceptionHandler) {
  if (getDbConnected()) {
    try{
      dbAccess_->setO2ORun(partitionName, runNumber);
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** 
 * \return the database version
 */
double TkDcuInfoFactory::getDbVersion ( ) throw (FecExceptionHandler) {

  double dbversion = 0 ;
  if (getDbConnected()) {
    try{
      dbversion = dbAccess_->getDbVersion() ;
    }
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }

  return dbversion ;
}

/** 
 * \return the database size in Mbytes
 */
double TkDcuInfoFactory::getDbSize ( ) throw (FecExceptionHandler) {

  double dbSize = 0 ;
  if (getDbConnected()) {
    try{
      dbSize = dbAccess_->getDbSize() ;
    }
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }

  return dbSize ;
}

/* 
 * \param partitionName The partition used for the run
 * \param runNumber The number of the run
 * \param newComment - new comment in the database
 */
void TkDcuInfoFactory::updateRunComment(std::string partitionName, unsigned int runNumber, std::string newComment) throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      dbAccess_->setNewComment(partitionName, runNumber, newComment);
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}


/** \brief Get the last run for the given partition
 */
TkRun* TkDcuInfoFactory::getLastRun (std::string partitionName) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (runs_.size()) deleteRunVectorI (runs_) ;
    TkRun *run =  xmlTkDcuInfo.getLastRun(partitionName);
    runs_.push_back(run) ;

    return run ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

}

/** \brief Get the given run for the given partition
 *  \return a TkRun. YOU have to delete this object !
 */
TkRun* TkDcuInfoFactory::getRun (std::string partitionName, int runNumber) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (runs_.size()) deleteRunVectorI (runs_) ;
    TkRun *run = xmlTkDcuInfo.getRun(partitionName, runNumber);
    runs_.push_back(run) ;
    
    return run ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}

/** \brief Get all the runs from the DB.
 *  \return a tkRunVector. YOU have to delete this object !
 */
tkRunVector TkDcuInfoFactory::getAllRuns () throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (runs_.size()) deleteRunVectorI (runs_) ;
    runs_ = xmlTkDcuInfo.getAllRuns();
    std::sort(runs_.begin(),runs_.end(),TkRun::sortByReverseRunNumber);
    return runs_;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}

/** \brief Get the last run used by O2O for the given partition
 *  \return a TkRun. YOU have to delete this object !
 */
TkRun* TkDcuInfoFactory::getLastO2ORun (std::string partitionName) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (runs_.size()) deleteRunVectorI (runs_) ;
    TkRun *run = xmlTkDcuInfo.getLastO2ORun(partitionName);
    runs_.push_back(run) ;
    
    return run ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}

/** \brief Retrieve the partition names for a given run
 *  \return A list with all the partition names.
 */
std::list<std::string> TkDcuInfoFactory::getAllPartitionNames( unsigned int runNumber ) throw (FecExceptionHandler) {

  if (getDbConnected()) {
    try{
      return dbAccess_->getAllPartitionNames( runNumber );
    }
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** \brief Retrieve the partition names from the current state
 *  \return A list with all the partition names.
 */
std::list<std::string> TkDcuInfoFactory::getAllPartitionNamesFromCurrentState() throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      return dbAccess_->getAllPartitionNamesFromCurrentState();
    }
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** \brief Retrieve the state names
 *  \return A list with all the state names. YOU have to delete that list!
 */
std::list<std::string> TkDcuInfoFactory::getAllStateHistoryNames() throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      return dbAccess_->getAllStateHistoryNames();
    }
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** \brief Retrieve the state names
 *  \return A list with all the state names. YOU have to delete that list!
 */
unsigned int TkDcuInfoFactory::getCurrentStateHistoryId() throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      return dbAccess_->getCurrentStateHistoryId();
    }
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** \brief Retrieve the partition names
 *  \return A list with all the partition names.
 */
std::list<std::string> TkDcuInfoFactory::getAllPartitionNames() throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      return dbAccess_->getAllPartitionNames();
    }
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** \brief Get all the Fec versions
 *  \return An array of TkVersion. 
 */
tkVersionVector &TkDcuInfoFactory::getAllFecVersions (std::string partitionName) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (fecVersions_.size()) deleteVersionVectorI (fecVersions_) ;
    fecVersions_ = xmlTkDcuInfo.getAllFecVersions(partitionName);

    return fecVersions_ ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}

/** \brief Get all the Fed versions
 *  \return An array of TkVersion. 
 */
tkVersionVector &TkDcuInfoFactory::getAllFedVersions (std::string partitionName) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (fedVersions_.size()) deleteVersionVectorI (fedVersions_) ;
    fedVersions_ = xmlTkDcuInfo.getAllFedVersions(partitionName);

    return fedVersions_ ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}

/** \brief Get all the Dcu Info versions
 *  \return An array of TkVersion. 
 */
tkVersionVector &TkDcuInfoFactory::getAllDcuInfoVersions (std::string partitionName) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (dcuInfoVersions_.size()) deleteVersionVectorI (dcuInfoVersions_) ;
    dcuInfoVersions_ = xmlTkDcuInfo.getAllDcuInfoVersions(partitionName);

    return dcuInfoVersions_ ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}

/** \brief Get all the Connection versions
 *  \return An array of TkVersion. 
 */
tkVersionVector &TkDcuInfoFactory::getAllConnectionVersions (std::string partitionName) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (connectionVersions_.size()) deleteVersionVectorI (connectionVersions_) ;
    connectionVersions_ = xmlTkDcuInfo.getAllConnectionVersions(partitionName);

    return connectionVersions_ ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}

/** \brief Get all the DcuPsuMap versions
 *  \return An array of TkVersion. 
 */
tkVersionVector &TkDcuInfoFactory::getAllDcuPsuMapVersions (std::string partitionName) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (dcuPsuVersions_.size()) deleteVersionVectorI (dcuPsuVersions_) ;
    dcuPsuVersions_ = xmlTkDcuInfo.getAllDcuPsuMapVersions(partitionName);

    return dcuPsuVersions_ ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}


/** \brief Get all the Mask versions
 *  \return An array of TkVersion. 
 */
tkVersionVector &TkDcuInfoFactory::getAllMaskVersions (std::string partitionName) throw (FecExceptionHandler){
  if (getDbConnected()) {
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    if (maskVersions_.size()) deleteVersionVectorI (maskVersions_) ;
    maskVersions_ = xmlTkDcuInfo.getAllMaskVersions(partitionName);

    return maskVersions_ ;
  }
  else 
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
}



/** Retreive the descriptions for the given devices from the input
 * \param partitionName - The name of the partition
 * \param cleanCache - clean the cache if true (default) and keep the det id already downloaded if set to false. If you set it to true then if some dcu have been already downloaded then it checks if the partition/version is coherent then it delete the previous data. This parameter is used only if you force the reload from db or if you are downloading another partition or version.
 * \param forceDbReload - force the reload of the database
 */
void TkDcuInfoFactory::addDetIdPartition ( std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId, bool cleanCache, bool forceDbReload )
  throw (FecExceptionHandler) {

  // check if the partition should be re-downloaded
  if (forceDbReload) initDbVersion_ = false ;
  if (initDbVersion_) {
    if ((majorVersionId != 0) || (minorVersionId != 0)) {  // A version has been set
	
      if ((partitionName_ == partitionName) && ((uint)majorVersionId == versionMajor_) && ((uint)minorVersionId == versionMinor_)) 
	initDbVersion_ = true ;
      else 
	initDbVersion_ = false ;
    }
    else { // from current version
      unsigned int partitionNumber, versionMajorI, versionMinorI ;
      getPartitionVersion(partitionName, &versionMajorI, &versionMinorI, &partitionNumber) ;
      if ((partitionName_ == partitionName) && (versionMajorI == versionMajor_) && (versionMinorI == versionMinor_)) 
	initDbVersion_ = true ;
      else 
	initDbVersion_ = false ;
      
#ifdef DEBUGMSGERROR
      std::cout << "Partition " << partitionName << " / " << partitionName_ 
		<< " Version " << versionMajorI << "." << versionMinorI << "/" 
		<< versionMajor_ << "." << versionMinor_ << std::endl ;
#endif
    }
  }

  // retreive the data from database if needed
  if (!initDbVersion_) {

    // delete the previous data only if asked (default)
    if (cleanCache) deleteHashMapTkDcuInfo () ;

    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;
    
    // Retreive the DCU Infos
    tkDcuInfoVector vDcuInfoVector = xmlTkDcuInfo.getDcuInfos(partitionName,majorVersionId,minorVersionId) ;
    
#ifdef DEBUGMSGERROR
    std::cout << "TkDcuInfoFactory::getTkDcuInfo: Found " << vDcuInfoVector.size() << " elements in the vector" << std::endl ;
    std::cout << "Number of dcu infos already in memory: " << vDcuInfo_.size() << std::endl ;
#endif
    
    if ( !vDcuInfoVector.empty() ) {
      // Merge the vector from the class and the new vector
      for (tkDcuInfoVector::iterator device = vDcuInfoVector.begin() ; device != vDcuInfoVector.end() ; device ++) {
	if (vDcuInfo_.find((*device)->getDcuHardId()) != vDcuInfo_.end()) delete vDcuInfo_[(*device)->getDcuHardId()] ;
	vDcuInfo_[(*device)->getDcuHardId()] = *device ;
      }
    }

    // Register the version
    if ((majorVersionId != 0) || (minorVersionId != 0)) {
      versionMajor_  = majorVersionId ;
      versionMinor_  = minorVersionId ;
      partitionName_ = partitionName ;
      initDbVersion_ = true ;
    }
    else {
      // Current state, check the value
      unsigned int partitionNumber ;
      getPartitionVersion(partitionName, &versionMajor_, &versionMinor_, &partitionNumber) ;
      initDbVersion_ = true ;
      partitionName_ = partitionName ;
#ifdef DEBUGMSGERROR
      std::cout << "Partition " << partitionName_ << ": current version is " << versionMajor_ << "." << versionMinor_ << std::endl ;
#endif
      }
  }

#ifdef DEBUGMSGERROR
  std::cout << "Number of dcu info in memory: " << vDcuInfo_.size() << std::endl ;
#endif

  // All devices are deleted by the XMLFecDevice so nothing must be deleted
}

/** Retreive the descriptions for all the known dcu_ids of the given version
 * \param majorVersionId - version major id (0.0) means current state
 * \param minorVersionId - version minor id (0.0) means current state
 * \param cleanCache - clean the cache if true (default) and keep the det id already downloaded if set to false. If you set it to true then if some dcu have been already downloaded then it checks if the partition/version is coherent then it delete the previous data
 * \param forceDbReload - force the reload of the database
 * \warning the attribut partition name is set to ALL in that call
 */
void TkDcuInfoFactory::addAllDetId( unsigned int majorVersionId, unsigned int minorVersionId, bool cleanCache, bool forceDbReload )
  throw (FecExceptionHandler) {

  std::string partitionName = "ALL" ;

  // check if the partition should be re-downloaded
  if (forceDbReload) initDbVersion_ = false ;
  if (initDbVersion_) {
    if ((majorVersionId != 0) || (minorVersionId != 0)) {  // A version has been set
	
      if ((partitionName_ == partitionName) && ((uint)majorVersionId == versionMajor_) && ((uint)minorVersionId == versionMinor_)) 
	initDbVersion_ = true ;
      else 
	initDbVersion_ = false ;
    }
    else { // from current version
      unsigned int partitionNumber, versionMajorI, versionMinorI ;
      getPartitionVersion(partitionName, &versionMajorI, &versionMinorI, &partitionNumber) ;
      if ((partitionName_ == partitionName) && (versionMajorI == versionMajor_) && (versionMinorI == versionMinor_)) 
	initDbVersion_ = true ;
      else 
	initDbVersion_ = false ;
      
#ifdef DEBUGMSGERROR
      std::cout << "Partition " << partitionName << " / " << partitionName_ 
		<< " Version " << versionMajorI << "." << versionMinorI << "/" 
		<< versionMajor_ << "." << versionMinor_ << std::endl ;
#endif
    }
  }

  // retreive the data from database if needed
  if (!initDbVersion_) {

    // delete the previous data only if asked (default)
    if (cleanCache) deleteHashMapTkDcuInfo () ;
    XMLTkDcuInfo xmlTkDcuInfo;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;
  
    // Retreive the DCU Infos
    tkDcuInfoVector vDcuInfoVector ;

    if ( (majorVersionId == 0) && (minorVersionId == 0) )
      vDcuInfoVector = xmlTkDcuInfo.getAllDcuInfos() ;
    else
      vDcuInfoVector = xmlTkDcuInfo.getAllDcuInfos(majorVersionId, minorVersionId) ;

#ifdef DEBUGMSGERROR
    std::cout << "TkDcuInfoFactory::addAllDetId : Found " << vDcuInfoVector.size() << " elements in the vector" << std::endl ;
    std::cout << "Number of dcu infos already in memory: " << vDcuInfo_.size() << std::endl ;
#endif
    
    if ( !vDcuInfoVector.empty() ) {
      // Merge the vector from the class and the new vector
      for (tkDcuInfoVector::iterator device = vDcuInfoVector.begin() ; device != vDcuInfoVector.end() ; device ++) {
	if (vDcuInfo_.find((*device)->getDcuHardId()) != vDcuInfo_.end()) delete vDcuInfo_[(*device)->getDcuHardId()] ;
	vDcuInfo_[(*device)->getDcuHardId()] = *device ;
      }
    }
    
    // Register the version
    if ((majorVersionId != 0) || (minorVersionId != 0)) {
      versionMajor_  = majorVersionId ;
      versionMinor_  = minorVersionId ;
      partitionName_ = partitionName ;
      initDbVersion_ = true ;
    }
    else {

      // Current state, check the value
      unsigned int partitionNumber ;
      getPartitionVersion(partitionName, &versionMajor_, &versionMinor_, &partitionNumber) ;
      initDbVersion_ = true ;
      partitionName_ = partitionName ;
#ifdef DEBUGMSGERROR
      std::cout << "Partition " << partitionName_ << ": current version is " << versionMajor_ << "." << versionMinor_ << std::endl ;
#endif
    }
  }

#ifdef DEBUGMSGERROR
  std::cout << "Number of dcu info in memory after the merge: " << vDcuInfo_.size() << std::endl ;
#endif
  
  // All devices are deleted by the XMLFecDevice so nothing must be deleted
}
#endif

/** Retreive the descriptions for the given devices from the map. The database or file are not accessed
 * \param dcuHardId - The DCU ID of the detector
 * \return The TkDcuInfo object containing the informations
 */
TkDcuInfo *TkDcuInfoFactory::getTkDcuInfo ( unsigned long dcuHardId )
  throw (FecExceptionHandler) {

  // No Dcu info exists in the map
  if (vDcuInfo_.find(dcuHardId) == vDcuInfo_.end()) { 
    std::stringstream msgError ; msgError << "No DCU infos for the DCU " << dcuHardId << " (in the hash_map) " << std::endl ;
    RAISEFECEXCEPTIONHANDLER ( NODATAAVAILABLE, msgError.str(), ERRORCODE) ;
  }

  return (vDcuInfo_[dcuHardId]) ;
}

/** Upload the description in the output (database or file)
 * \param dcuInfoVector - a vector of conversion factor
 */
void TkDcuInfoFactory::setTkDcuInfo ( tkDcuInfoVector vDcuInfo ) 
  throw ( FecExceptionHandler ) {

  #ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if (getDbUsed())
    std::cout << "setTkDcuInfo: set " << vDcuInfo.size() << " DCU info in the database" << std::endl ;
  else 
#  endif
    std::cout << "setTkDcuInfo set " << vDcuInfo.size() << " DCU info in the file " << outputFileName_ << std::endl ;
  #endif

  if (vDcuInfo.empty())
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in file", ERRORCODE) ;

#ifdef DATABASE
  if (getDbUsed()) {

    // XML parser creation
    XMLTkDcuInfo xmlTkDcuInfo ;
    xmlTkDcuInfo.setDatabaseAccess(dbAccess_) ;

    xmlTkDcuInfo.setDcuInfos(vDcuInfo) ;
  }
  else
#endif
    {  // Upload in file
      XMLTkDcuInfo xmlTkDcuInfo ;
      xmlTkDcuInfo.setDcuInfos(vDcuInfo, outputFileName_) ;
    }
}

#ifdef DATABASE
/** \brief Create a new version for the dcu infos
 */
void TkDcuInfoFactory::createNewDcuInfoVersion() throw (FecExceptionHandler){
  if (getDbConnected()) {
    try{
      dbAccess_->createNewDcuInfoVersion();
      dbAccess_->commit();
    }
    catch (oracle::occi::SQLException &e) {
      dbAccess_->rollback();
      RAISEFECEXCEPTIONHANDLER ( DB_PLSQLEXCEPTIONRAISED, e.what(), ERRORCODE ) ;
    }
    catch (FecExceptionHandler &e) {
      dbAccess_->rollback();
      throw e ;
    }
  }
  else{
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}
#endif

/** Upload the description in the output (database or file)
 * \param vInfo - a hash_map of conversion factors
 */
void TkDcuInfoFactory::setTkDcuInfo ( Sgi::hash_map<unsigned long, TkDcuInfo *> vInfo ) 
  throw ( FecExceptionHandler ) {

  tkDcuInfoVector vDcuInfo ;
  for (Sgi::hash_map<unsigned long, TkDcuInfo *>::iterator itr = vInfo.begin() ; itr != vInfo.end() ; itr ++) {

    vDcuInfo.push_back (itr->second) ;
  }

  setTkDcuInfo (vDcuInfo) ;
}

/** Upload the description in the output (database or file) with the hash_map attribut of the class
 */
void TkDcuInfoFactory::setTkDcuInfo ( ) 
  throw ( FecExceptionHandler ) {

  setTkDcuInfo (vDcuInfo_) ;
}



