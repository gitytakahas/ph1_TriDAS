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
#include <iostream>
#include <fstream>

#include "Fed9UEventStreamLineException.hh"
#include "DeviceFactory.h"

#define DEBUGTIMINGDISABLE

/** Build a DeviceFactory to retreive information from database or file
 */
DeviceFactory::DeviceFactory ( ) :
  FecFactory ( ), 
  Fed9U::Fed9UDeviceFactory ( ),
  ConnectionFactory ( ),
  TkDcuConversionFactory ( ),
  TkDcuInfoFactory ( ),
  TkDcuPsuMapFactory ( ),
  TkIdVsHostnameFactory ( ),
  CommissioningAnalysisFactory ( )
{

  runNumberFile_ = UNKNOWNFILE ;
}

/** Create an access to the database to both FEC and FED database
 * \param login - login to database
 * \param password - password to database
 * \param path - URL to acess the database
 * \param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * \warning open both FEC and FED access
 */
#ifdef DATABASE

DeviceFactory::DeviceFactory ( std::string login, std::string password, std::string path, bool threaded )
  throw ( oracle::occi::SQLException ):
  FecFactory(login, password, path, threaded),
  Fed9U::Fed9UDeviceFactory(login, password, path, threaded),
  ConnectionFactory (login, password, path, threaded),
  TkDcuConversionFactory (login, password, path, threaded),
  TkDcuInfoFactory (login, password, path, threaded),
  TkDcuPsuMapFactory (login, password, path, threaded),
  TkIdVsHostnameFactory (login, password, path, threaded),
  CommissioningAnalysisFactory(login, password, path, threaded)
{

  runNumberFile_ = UNKNOWNFILE ;
}

#endif

/** Disconnect the database (if it is set)
 */  
DeviceFactory::~DeviceFactory ( ) { }

/** Set the run number file if the database is not used
 * The current run number is the number in the file,
 * the next run number is the run number in the file + 1
 * Nothing is done in this method. Only the method get
 */
void DeviceFactory::setRunNumberFile ( std::string filename ) {

  runNumberFile_ = filename ;
}

/** Retreive from the database the next run number from the database, if the database is not set the next run method return the current timestamp and store it for the method getCurrentRunNumber
 * \return the next run number
 * \warning, if the database is not set, a file must be set with the method setFileRunNumber and if this file is not set, an exception is thrown. The file for this method must read/writable at least for this method.
 */
unsigned int DeviceFactory::getNextRunNumber ( ) 
  throw (FecExceptionHandler) {

  unsigned int runNumber ;

#ifdef DATABASE
  DbCommonAccess *dbAccess = getAnyDbAccess() ;
  if (dbAccess != NULL) {
    runNumber = dbAccess->getCurrentRunNumber() ;
    runNumber ++ ;
  }
#endif
    {
      runNumber = getCurrentRunNumber() ;
      runNumber ++ ;
      
      ofstream fileOutStream((const char *)(runNumberFile_.c_str()));
      if (fileOutStream.is_open()) {
	fileOutStream << toString(runNumber) ;
	fileOutStream.flush();
	fileOutStream.close();
      }
      else {
	RAISEFECEXCEPTIONHANDLER (FILEPROBLEMERROR, "cannot write in the file " + runNumberFile_ + " for the run number", ERRORCODE) ;
      }
    }
  
  return runNumber ;
}

/** Retreive the run number from the database if it is set
 * \return the current run number
 */
unsigned int DeviceFactory::getCurrentRunNumber ( ) 
  throw (FecExceptionHandler) {

  unsigned int runNumber ;

#ifdef DATABASE
  DbCommonAccess *dbAccess = getAnyDbAccess() ;
  if (dbAccess != NULL) {
    runNumber = dbAccess->getCurrentRunNumber() ;
    runNumber ++ ;
  }
  else
#endif
    {
      if (runNumberFile_ != UNKNOWNFILE) {

	ifstream fileInStream((const char *)(runNumberFile_.c_str()));
        if (fileInStream.is_open()) {
	  std::string str ;
	  fileInStream >> str ;
	  fileInStream.close() ;
	  runNumber = fromString<unsigned int>(str) ;
	}
	else
 	  RAISEFECEXCEPTIONHANDLER (FILEPROBLEMERROR, "cannot read in the file " + runNumberFile_ + " for the run number", ERRORCODE) ;
      }
      else
	RAISEFECEXCEPTIONHANDLER (FILEPROBLEMERROR, "no file for the run number is set", ERRORCODE) ;
    }
  
  return runNumber ;
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Set this file as the new output
 * \param outputFileName - file name for the output
 */
void DeviceFactory::setOutputFileName ( std::string outputFileName ) {

  FecFactory::setOutputFileName (outputFileName) ;
  Fed9U::Fed9UDeviceFactory::setOutputFileName (outputFileName) ;
  ConnectionFactory::setOutputFileName (outputFileName) ;
  TkDcuConversionFactory::setOutputFileName (outputFileName) ;
  TkDcuInfoFactory::setOutputFileName (outputFileName) ;
  TkDcuPsuMapFactory::setOutputFileName (outputFileName) ;
  TkIdVsHostnameFactory::setOutputFileName (outputFileName) ;
  CommissioningAnalysisFactory::setOutputFileName (outputFileName) ;
}

/** 
 * \param inputFileName - new input file for the FEC
 */
void DeviceFactory::setFecInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  FecFactory::setInputFileName (inputFileName) ;
}

/**
 * \param fileName - name of the XML file for the FEC
 */
void DeviceFactory::addFecFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  FecFactory::addFileName (fileName) ;
}

/** 
 * \param inputFileName - new input file for the FED
 */ 
void DeviceFactory::setFedInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  Fed9U::Fed9UDeviceFactory::setInputFileName (inputFileName) ;
}

/**
 * \param fileName - name of the XML file for the FED
 */
void DeviceFactory::addFedFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  Fed9U::Fed9UDeviceFactory::addFileName (fileName) ;
}

/** 
 * \param inputFileName - new input file for the connections between FEC and FED
 */ 
void DeviceFactory::setFedFecConnectionInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  ConnectionFactory::setInputFileName (inputFileName) ;
}

/** 
 * \param inputFileName - new input file for the connections between FEC and FED
 */ 
void DeviceFactory::setConnectionInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  ConnectionFactory::setInputFileName (inputFileName) ;
}

/**
 * \param fileName - name of the XML file for the connections between FEC and FED
 */
void DeviceFactory::addFedFecConnectionFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  ConnectionFactory::addFileName (fileName) ;
}

/**
 * \param fileName - name of the XML file for the connections between FEC and FED
 */
void DeviceFactory::addConnectionFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  ConnectionFactory::addFileName (fileName) ;
}

/** 
 * \param inputFileName - new input file for the DCU conversion factors
 */ 
void DeviceFactory::setDcuConversionInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  TkDcuConversionFactory::setInputFileName (inputFileName) ;
}

/**
 * \param fileName - name of the XML file for the DCU conversion factors
 */
void DeviceFactory::addDcuConversionFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  TkDcuConversionFactory::addFileName (fileName) ;
}

/** 
 * \param inputFileName - new input file for the DCU / det ID information
 */ 
void DeviceFactory::setTkDcuInfoInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  TkDcuInfoFactory::setInputFileName (inputFileName) ;
}


/**
 * \param fileName - name of the XML file for the DCU / det ID information
 */
void DeviceFactory::addTkDcuInfoFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  TkDcuInfoFactory::addFileName (fileName) ;
}

/** 
 * \param inputFileName - new input file for the DCU / det ID information
 */ 
void DeviceFactory::setTkDcuPsuMapFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  TkDcuPsuMapFactory::setInputFileName (inputFileName) ;
}

/**
 * \param fileName - name of the XML file for the DCU / det ID information
 */
void DeviceFactory::addTkDcuPsuMapFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  TkDcuPsuMapFactory::addFileName (fileName) ;
}

/** 
 * \param inputFileName - new input file for the DCU / det ID information
 */ 
void DeviceFactory::setTkIdVsHostnameInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  TkIdVsHostnameFactory::setInputFileName (inputFileName) ;
}

/**
 * \param fileName - name of the XML file for the DCU / det ID information
 */
void DeviceFactory::addTkIdVsHostnameFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  TkIdVsHostnameFactory::addFileName (fileName) ;
}


/** 
 * \param useIt - use file, mandatory if the database is not set !
 */
void DeviceFactory::setUsingFile ( bool useIt ) {
  FecFactory::setUsingFile(useIt) ;
  Fed9U::Fed9UDeviceFactory::setUsingFile(useIt) ;
  ConnectionFactory::setUsingFile(useIt) ;
  TkDcuConversionFactory::setUsingFile(useIt) ;
  TkDcuInfoFactory::setUsingFile(useIt) ;
  TkDcuPsuMapFactory::setUsingFile(useIt) ;
  TkIdVsHostnameFactory::setUsingFile(useIt) ;
  CommissioningAnalysisFactory::setUsingFile(useIt) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** 
 * \param useIt - use the database, the database must be created, if not an exception is raised
 */
void DeviceFactory::setUsingDb ( bool useIt ) throw (std::string) {
  FecFactory::setUsingDb(useIt) ;
  Fed9U::Fed9UDeviceFactory::setUsingDb(useIt) ;
  ConnectionFactory::setUsingDb(useIt) ;
  TkDcuConversionFactory::setUsingDb(useIt) ;
  TkDcuInfoFactory::setUsingDb(useIt) ;
  TkDcuPsuMapFactory::setUsingDb(useIt) ;
  TkIdVsHostnameFactory::setUsingDb(useIt) ;
  CommissioningAnalysisFactory::setUsingDb(useIt) ;
}

/**
 * \return true if the database is connected and used both for PIA and FEC access
 */
bool DeviceFactory::getDbUsed ( ) {

#ifdef DEBUGMSGERROR
  std::cout << "FecFactory: " << FecFactory::getDbUsed() << std::endl ;
  std::cout << "Connection: " << ConnectionFactory::getDbUsed() << std::endl ;
  std::cout << "Fed9u: " << Fed9U::Fed9UDeviceFactory::getDbUsed() << std::endl ;
  std::cout << "Dcu conversion: " << TkDcuConversionFactory::getDbUsed() << std::endl ;
  std::cout << "Det id: " << TkDcuInfoFactory::getDbUsed() << std::endl ;
  std::cout << "PSU map: " << TkDcuPsuMapFactory::getDbUsed() << std::endl ;
  std::cout << "hostname: " << TkIdVsHostnameFactory::getDbUsed() << std::endl ;
  std::cout << "Comm: " << CommissioningAnalysisFactory::getDbUsed() << std::endl ;
#endif

  return ( FecFactory::getDbUsed() && 
	   ConnectionFactory::getDbUsed() && 
	   Fed9U::Fed9UDeviceFactory::getDbUsed() &&
	   TkDcuConversionFactory::getDbUsed() &&
	   TkDcuInfoFactory::getDbUsed() &&
	   TkDcuPsuMapFactory::getDbUsed() &&
	   TkIdVsHostnameFactory::getDbUsed() &&
	   CommissioningAnalysisFactory::getDbUsed()) ;
}

/**
 * \return true if the database is connected
 */
bool DeviceFactory::getDbConnected ( ) {

#ifdef DEBUGMSGERROR
  std::cout << "FecFactory: " << FecFactory::getDbConnected() << std::endl ;
  std::cout << "Connection: " << ConnectionFactory::getDbConnected() << std::endl ;
  std::cout << "Fed9u: " << Fed9U::Fed9UDeviceFactory::getDbConnected() << std::endl ;
  std::cout << "Dcu conversion: " << TkDcuConversionFactory::getDbConnected() << std::endl ;
  std::cout << "Det id: " << TkDcuInfoFactory::getDbConnected() << std::endl ;
  std::cout << "PSU map: " << TkDcuPsuMapFactory::getDbConnected() << std::endl ;
  std::cout << "hostname: " << TkIdVsHostnameFactory::getDbConnected() << std::endl ;
  std::cout << "Comm: " << CommissioningAnalysisFactory::getDbConnected() << std::endl ;
#endif

  return ( FecFactory::getDbConnected() && 
	   ConnectionFactory::getDbConnected() &&
	   Fed9U::Fed9UDeviceFactory::getDbConnected() &&
	   TkDcuConversionFactory::getDbConnected() &&
	   TkDcuInfoFactory::getDbConnected() &&
	   TkDcuPsuMapFactory::getDbConnected() &&
	   TkIdVsHostnameFactory::getDbConnected() &&
	   CommissioningAnalysisFactory::getDbConnected()) ;
}

/** 
 */
void DeviceFactory::setDatabaseAccess ( ) throw ( oracle::occi::SQLException ) {

  FecFactory::setDatabaseAccess () ;
  Fed9U::Fed9UDeviceFactory::setDatabaseAccess () ;
  ConnectionFactory::setDatabaseAccess () ;
  TkDcuConversionFactory::setDatabaseAccess () ;
  TkDcuInfoFactory::setDatabaseAccess () ;
  TkDcuPsuMapFactory::setDatabaseAccess () ;
  TkIdVsHostnameFactory::setDatabaseAccess () ;
  CommissioningAnalysisFactory::setDatabaseAccess () ;
}

/** 
 * \param login - login to database
 * \param password - password to database
 * \param path - path to database
 * \warning in the destructor of FecFactory, the dbFecAccess is deleted
 */
void DeviceFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) throw ( oracle::occi::SQLException ) {

  FecFactory::setDatabaseAccess (login, password, path ) ;
  Fed9U::Fed9UDeviceFactory::setDatabaseAccess (login, password, path ) ;
  ConnectionFactory::setDatabaseAccess (login, password, path ) ;
  TkDcuConversionFactory::setDatabaseAccess (login, password, path ) ;
  TkDcuInfoFactory::setDatabaseAccess (login, password, path ) ;
  TkDcuPsuMapFactory::setDatabaseAccess (login, password, path ) ;
  TkIdVsHostnameFactory::setDatabaseAccess (login, password, path ) ;
  CommissioningAnalysisFactory::setDatabaseAccess (login, password, path ) ;
}

/** \return the partition name
 */
std::string DeviceFactory::getPartitionName ( unsigned int partitionId )  throw (oracle::occi::SQLException) {
  
  return (FecFactory::getPartitionName (partitionId)) ;
}

/** Refresh the cache of the connections, FEC, FED for the current version
 * \param deleteValues - delete the values before doing the update: 0 do not delete, 1 delete the partitions which are not in the current state, 2 delete everything
 * \param withStrips - update the FED with strips
 */
void DeviceFactory::refreshCacheXMLClob ( int deleteValues, bool withStrips ) throw (oracle::occi::SQLException) {

  DbCommonAccess *dbAccess = getAnyDbAccess() ;
  if (dbAccess != NULL) dbAccess->refreshCacheXMLClob(deleteValues, withStrips) ;
  else RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

}

/** This method download the connections for a given partition and identify the det id corresponding to it. 
 * This method is made only for database. The Det id downloaded are coming from the current state.
 * \param partitionName - partition name
 * \param connectionVector - vector of connections
 * \param versionMajorId - version major
 * \param versionMinorId - version minor
 * \param maskVersionMajor - mask version major
 * \param maskVersionMinor - mask version minor
 * \param allConnections - enabled / disabled connections
 * \return unsigned integer which gives you how much connection were NOT identified with the det id (the det id is set to zero in that case, and if integer returned is zero then all connections have a corresponding det id)
 */
unsigned int DeviceFactory::getConnectionDescriptionDetId ( std::string partitionName, ConnectionVector &connectionVector, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor, bool allConnections, bool forceDbReload ) throw (FecExceptionHandler, oracle::occi::SQLException) {

  unsigned int connectionNotIdentified = 0 ;

  if (ConnectionFactory::getDbUsed() && TkDcuInfoFactory::getDbUsed()) {

    // Download the connections
    getConnectionDescriptions ( partitionName, connectionVector, versionMajorId, versionMinorId, maskVersionMajor, maskVersionMinor, allConnections, forceDbReload ) ;

    // Retreive the det id from the current version
    this->addDetIdPartition ( partitionName ) ;

    // For each connections identify the corresponding det id
    for (ConnectionVector::iterator it = connectionVector.begin() ; it != connectionVector.end() ; it ++) {
      try {
	TkDcuInfo *tkDcuInfo = this->getTkDcuInfo ((*it)->getDcuHardId()) ;
	(*it)->setDetId (tkDcuInfo->getDetId()) ;
	(*it)->setNumberOfAvps (tkDcuInfo->getApvNumber()) ;
	(*it)->setFiberLength (tkDcuInfo->getFibreLength()) ;
      }
      catch (FecExceptionHandler &e) {
	(*it)->setDetId (0) ;
	connectionNotIdentified ++ ;
      }
    }
  }
  else RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;

  return connectionNotIdentified;
}

/** return the error message coming from oracle exception and get it if it exists the message set by the user exception in PL/SQL
 * \param message - message from caller side (mainly when the error appends)
 * \param ex - oracle exception raised
 * \return error message 
 */
std::string DeviceFactory::what ( std::string message, oracle::occi::SQLException &ex ) {

  std::stringstream msgError ;
  DbCommonAccess *dbAccess = getAnyDbAccess() ;
   
  if (dbAccess != NULL) msgError << dbAccess->what(message,ex) ;
  else msgError << message << ": " << ex.what() ;
    
  return msgError.str() ;
}

/** \brief Disable/enable the devices linked to the dcuHardId in the given partition
 * \param partitionName The name of the chosen partition
 * \return The list of dcuHardId to disable
 * \warning this method create an new version of FEC/FED and connections so you should re-download everything afterwards
 */  
void DeviceFactory::setEnableModules(std::string partitionName, std::vector<unsigned int> dcuHardIdList, bool enabled) 
  throw (FecExceptionHandler, oracle::occi::SQLException, Fed9U::Fed9UDeviceFactoryException, ICUtils::ICException) {

#ifdef DEBUGTIMINGDISABLE
  unsigned long startMillis, endMillis ;
#endif

  if (dcuHardIdList.size()) {

    if (getDbConnected()) {
      
      // ------------------------
      // download all FEC devices
      deviceVector allDevices ;
      getFecDeviceDescriptions(partitionName,allDevices,0,0,true) ;
      // Upload all FEC devices
      if (allDevices.size()) {
	unsigned int vMajor = 0, vMinor = 0 ;
#ifdef DEBUGTIMINGDISABLE
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis(); 
#endif

	setFecDeviceDescriptions(allDevices,partitionName,&vMajor,&vMinor,false) ;

#ifdef DEBUGTIMINGDISABLE
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis(); 
	std::cout << std::dec << "Create FEC minor version " << vMajor << "." << vMinor << " tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
      }
      // ------------------------
      // download all FED devices
      std::vector<Fed9U::Fed9UDescription*> *ptrVector = getFed9UDescriptions(partitionName) ;
      // upload FED devices
      if (ptrVector->size() > 0) {
	uint16_t vMajor = 0, vMinor = 0 ;
#ifdef DEBUGTIMINGDISABLE
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis(); 
#endif

	setFed9UDescriptions(*ptrVector,partitionName,&vMajor,&vMinor,1);

#ifdef DEBUGTIMINGDISABLE
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis(); 
	std::cout << std::dec << "Create FED major version " << vMajor << "." << vMinor << " tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
      }
      
      // ------------------------
      // download connections
      ConnectionVector allConnections ;
      getConnectionDescriptions (partitionName,allConnections,0,0,0,0,true) ;
      // upload connections
      if (allConnections.size() > 0) {
	unsigned int vMajor = 0, vMinor = 0 ;
#ifdef DEBUGTIMINGDISABLE
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis(); 
#endif

	setConnectionDescriptions (allConnections,partitionName,&vMajor,&vMinor,false) ;

#ifdef DEBUGTIMINGDISABLE
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis(); 
	std::cout << std::dec << "Create connection version " << vMajor << "." << vMinor << " tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
      }
      
      // ---------------------------
      // Disable the corresponding dcus
#ifdef DEBUGTIMINGDISABLE
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis(); 
#endif

      if (enabled) getAnyDbAccess()->setDeviceState(partitionName, dcuHardIdList, 1);//1-> disable the devices
      else getAnyDbAccess()->setDeviceState(partitionName, dcuHardIdList, 0);//0-> disable the devices

#ifdef DEBUGTIMINGDISABLE
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis(); 
      std::cout << std::dec << "Disable of devices tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif
    }
    else{
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  }
}

/** this method access the database in order to set the o2o operation performed
 * \param partitionName - partition name
 * \param subDetector - the corresponding subdetector
 * \param o2oTimestamp - a given timestamp (o2o time operation)
 */
void DeviceFactory::setO2OOperation ( std::string partitionName, std::string subDetector, unsigned int runNumber ) 
  throw (FecExceptionHandler) {

  DbCommonAccess *dbAccess = getAnyDbAccess() ;
   
  if (dbAccess != NULL) {
    dbAccess->setO2OOperation ( partitionName, subDetector, runNumber ) ;
  }
  else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/** this method checked that the o2o operation is coherent with the current state
 * \param partitionName - partition name
 * \return true if the versions for the partition is coherent, false if it is not coherent
 */
bool DeviceFactory::getO2OXchecked ( std::string partitionName ) throw (FecExceptionHandler) {

  bool result = false ;

  DbCommonAccess *dbAccess = getAnyDbAccess() ;
   
  if (dbAccess != NULL) {
    if (dbAccess->getO2OXchecked ( partitionName )) result = true ;
  }
  else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }

  return result ;
}

#endif
