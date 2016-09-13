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


#include "tscTypes.h"      // hash table of devices and PIA descriptions
#include "keyType.h"       // FEC/ring/CCU/channel/address are managed in this file (not a class)
#include "deviceType.h"

#include "FecExceptionHandler.h" // exceptions
#include "DbFecAccess.h"
#include "XMLFecDevice.h"
#include "XMLFecPiaReset.h"
#include "XMLFecDcu.h"
#include "XMLFecCcu.h"
#include "XMLTkDcuConversion.h"
#include "XMLTkDcuInfo.h"

#include "DeviceFactory.h"

#include <xercesc/util/PlatformUtils.hpp>

/** test the run from the database only
 * -run
 */
int testRun ( bool databaseUsed, std::string partitionName, unsigned int run, bool o2o ) {

  if (databaseUsed) {
    if (run != 0)
      std::cout << "Retreive the Run information from database for the partition " << partitionName << " for the run " << run << std::endl ;      
    else if (o2o)
      std::cout << "Retrieve the latest O2O run done from database for the partition " << partitionName << std::endl ;
    else
      std::cout << "Retrieve the latest run done from database for the partition " << partitionName << std::endl ;
  }

  DbTkDcuInfoAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbTkDcuInfoAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbTkDcuInfoAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else { std::cerr << "Only available from the database" << std::endl ; return -1 ; }

  // ---------------------------------------------------
  // XML parsing
  XMLTkDcuInfo *xmlTkDcuInfo = NULL ;

  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

  try {
    xmlTkDcuInfo = new XMLTkDcuInfo (dbAccess) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }
  
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLTkDcuInfo = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    TkRun *runInfo = NULL ;
    tkRunVector allRuns ;
    if (run == 0xFFFFFFFF) allRuns  = xmlTkDcuInfo->getAllRuns() ;
    if (run != 0) runInfo = xmlTkDcuInfo->getRun(partitionName,run) ;
    else if (o2o) runInfo = xmlTkDcuInfo->getLastO2ORun(partitionName) ;
    else runInfo = xmlTkDcuInfo->getLastRun(partitionName) ;

    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLTkDcuInfo parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
#ifndef SAXPARSER
    std::cout << "Found " << xmlTkDcuInfo->getCountRun() << " run information" << std::endl ;
    std::cout << "Found " << allRuns.size() << " run information" << std::endl ;
#else
    std::cout << "-- Retreive " << 1 << " run info = " << duration << " ms" << std::endl ;
#endif

    if (runInfo != NULL) delete runInfo ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to retreive the CCUs: " << e.what() << std::endl ;
  }
    
  try {
    // this method handle the remove of the vectors
    if (dbAccess != NULL) delete dbAccess ;
    if (xmlTkDcuInfo != NULL) delete xmlTkDcuInfo ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }
  
  return 0 ;
} 


/** test the version for the different components
 */
int testVersion ( bool databaseUsed, std::string partitionName, bool fec, bool fed, bool connection, bool dcupsumap, bool detid ) {

  if (databaseUsed) {
    std::cout << "Retreive all the version for the partition " << partitionName << std::endl ;
  }

  DbTkDcuInfoAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbTkDcuInfoAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbTkDcuInfoAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else { std::cerr << "Only available from the database" << std::endl ; return -1 ; }

  // ---------------------------------------------------
  // XML parsing
  XMLTkDcuInfo *xmlTkDcuInfo = NULL ;

  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

  try {
    xmlTkDcuInfo = new XMLTkDcuInfo (dbAccess) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }
  
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLTkDcuInfo = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  tkVersionVector tkVersion ;
  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (fec) tkVersion =  xmlTkDcuInfo->getAllFecVersions(partitionName) ;
    else if (fed) tkVersion =  xmlTkDcuInfo->getAllFedVersions(partitionName) ;
    else if (connection) tkVersion =  xmlTkDcuInfo->getAllConnectionVersions(partitionName) ;
    else if (dcupsumap) tkVersion =  xmlTkDcuInfo->getAllDcuPsuMapVersions(partitionName) ;
    else if (detid) tkVersion =  xmlTkDcuInfo->getAllDcuInfoVersions(partitionName) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLTkDcuInfo parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
    std::cout << "-- Retreive " << tkVersion.size() << " versions = " << duration << " ms" << std::endl ;
  
#ifndef SAXPARSER
    std::cout << "Found " << xmlTkDcuInfo->getCountVersion() << " Versions for the partition " << partitionName << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to retreive the CCUs: " << e.what() << std::endl ;
  }
  TkDcuInfoFactory::deleteVersionVectorI(tkVersion) ;
    
  try {
    // this method handle the remove of the vectors
    if (dbAccess != NULL) delete dbAccess ;
    if (xmlTkDcuInfo != NULL) delete xmlTkDcuInfo ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }
  
  return 0 ;
} 

/** test the state from the database only
 * -state
 */
int testState ( bool databaseUsed ) {

  if (databaseUsed) {
    std::cout << "Retreive the state from database" << std::endl ;
  }

  DbTkDcuInfoAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbTkDcuInfoAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbTkDcuInfoAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else { std::cerr << "Only available from the database" << std::endl ; return -1 ; }

  // ---------------------------------------------------
  // XML parsing
  XMLTkDcuInfo *xmlTkDcuInfo = NULL ;

  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

  try {
    xmlTkDcuInfo = new XMLTkDcuInfo (dbAccess) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }
  
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLTkDcuInfo = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    tkStateVector dVector = xmlTkDcuInfo->getAllCurrentStates() ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLTkDcuInfo parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
    std::cout << "-- Retreive " << dVector.size() << " DCU info/Det ID = " << duration << " ms" << std::endl ;
  
#ifndef SAXPARSER
    std::cout << "Found " << xmlTkDcuInfo->getCountState() << " State information" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to retreive the states: " << e.what() << std::endl ;
  }
    
  try {

    // this method handle the remove of the vectors
    if (dbAccess != NULL) delete dbAccess ;
    if (xmlTkDcuInfo != NULL) delete xmlTkDcuInfo ;

  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }

  return 0 ;
} 

/** test the DCU Information
 * -dcuinfo
 * -partition ALL
 */
int testDCUInfo ( bool databaseUsed, std::string partitionName, std::string fileName ) {

  if (databaseUsed) {
    std::cout << "Retreive the DCU information from database " ;
    if (partitionName == "ALL")
      std::cout << "for all partitions in the database" << std::endl ;
    else
      std::cout << "for the partition " << partitionName << std::endl ;
  }
  else if (fileName != "") {
    std::cout << "Retreive the DCU info from file " << fileName << std::endl ;
  }

  DbTkDcuInfoAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbTkDcuInfoAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbTkDcuInfoAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else if (fileName == "") {
    
    std::cerr << "Please specify or <-partition [PARTITION NAME] -database> or <-file [FILENAME]>" << std::endl ;
    return -1 ;
  }

  // ---------------------------------------------------
  // XML parsing
  XMLTkDcuInfo *xmlTkDcuInfo = NULL ;

  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

  try {
    if (databaseUsed)
      xmlTkDcuInfo = new XMLTkDcuInfo (dbAccess) ;
    else if (fileName != "") 
      xmlTkDcuInfo = new XMLTkDcuInfo (fileName) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }
  
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLTkDcuInfo = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    tkDcuInfoVector dVector ;
    if (databaseUsed) 
      if (partitionName == "ALL")
	dVector = xmlTkDcuInfo->getAllDcuInfos() ;
      else
	dVector = xmlTkDcuInfo->getDcuInfos(partitionName) ;
    else if (fileName != "")
      dVector = xmlTkDcuInfo->getDcuInfos() ;

    TkDcuInfoFactory::deleteVectorI ( dVector ) ;

    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLTkDcuInfo parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
    std::cout << "-- Retreive " << dVector.size() << " DCU info/Det ID = " << duration << " ms" << std::endl ;
  
#ifndef SAXPARSER
    std::cout << "Found " << xmlTkDcuInfo->getCountDCUInfo() << " DCU Info/Det ID" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to retreive the CCUs: " << e.what() << std::endl ;
  }
    
  try {
    // this method handle the remove of the vectors
    if (dbAccess != NULL) delete dbAccess ;
    if (xmlTkDcuInfo != NULL) delete xmlTkDcuInfo ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }
  
  return 0 ;
}

/** test the DCU conversion
 */
int testDCUConversion ( bool databaseUsed, std::string partitionName, unsigned int dcuHardId, std::string fileName ) {

  if (databaseUsed) {
    std::cout << "Retreive the DCU conversion factors from database " ;
    if (dcuHardId == 0) {
      if (partitionName != "ALL")
	std::cout << "for the partition " << partitionName ;
      else
	std::cout << "all the partition " ;
    }
    else
      std::cout << "for the dcu hardware ID " << dcuHardId ;
    std::cout << std::endl ;
  }
  else if (fileName != "") {
    std::cout << "Retreive the DCU conversion factors from file " << fileName << std::endl ;
  }

  DbTkDcuConversionAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbTkDcuConversionAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbTkDcuConversionAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else if (fileName == "") {
    
    std::cerr << "Please specify or <-partition [PARTITION NAME] -database> or <-file [FILENAME]>" << std::endl ;
    return -1 ;
  }

  // ---------------------------------------------------
  // XML parsing
  XMLTkDcuConversion *xmlTkDcuConversion = NULL ;

  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

  try {
    if (databaseUsed)
      xmlTkDcuConversion = new XMLTkDcuConversion (dbAccess) ;
    else if (fileName != "") 
      xmlTkDcuConversion = new XMLTkDcuConversion (fileName) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }
  
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLTkDcuConversion = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    dcuConversionVector dVector ;
    if (databaseUsed) {
      if (dcuHardId == 0) 
	if (partitionName != "ALL")
	  dVector = xmlTkDcuConversion->getDcuConversions(partitionName) ;
	else
	  dVector = xmlTkDcuConversion->getDcuConversions() ;
      else
	dVector = xmlTkDcuConversion->getDcuConversions(dcuHardId) ;
    }
    else if (fileName != "")
      dVector = xmlTkDcuConversion->getDcuConversions() ;

    TkDcuConversionFactory::deleteVectorI(dVector) ;
    
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLTkDcuConversion parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
    std::cout << "-- Retreive " << dVector.size() << " conversion factors = " << duration << " ms" << std::endl ;
  
#ifndef SAXPARSER
    std::cout << "Found " << xmlTkDcuConversion->getCountDCUConversion() << " DCU conversion factors" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to retreive the CCUs: " << e.what() << std::endl ;
  }
    
  try {
    // this method handle the remove of the vectors
    if (xmlTkDcuConversion != NULL) delete xmlTkDcuConversion ;
    if (dbAccess != NULL) delete dbAccess ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }
  
  return 0 ;
}

/** test the DCU access
 */
int testCCU ( bool databaseUsed, std::string partitionName, std::string fecHardwareId, std::string fileName, unsigned int ring ) {

  if (databaseUsed) {
    std::cout << "Retreive the CCU descriptions from database " 
	      << "for the partition " << partitionName << ", the FEC " << fecHardwareId 
	      << " and the ring " << ring << std::endl ;
  }
  else if (fileName != "") {
    std::cout << "Retreive the CCU descriptions from database from file " << fileName 
	      << "for the FEC " << fecHardwareId 
	      << " and the ring " << ring << std::endl ;
  }

  DbFecAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbFecAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbFecAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else if (fileName == "") {
    
    std::cerr << "Please specify or <-partition [PARTITION NAME] -database> or <-file [FILENAME]>" << std::endl ;
    return -1 ;
  }

  // ---------------------------------------------------
  // XML parsing
  XMLFecCcu *xmlFecCcu = NULL ;

  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

  try {
    if (databaseUsed)
      xmlFecCcu = new XMLFecCcu (dbAccess) ;
    else if (fileName != "") 
      xmlFecCcu = new XMLFecCcu (fileName) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }
  
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLFecCcu = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    TkRingDescription* myRing = NULL;
    if (databaseUsed) {
      myRing = xmlFecCcu->getRingFromDb (partitionName,fecHardwareId,ring) ;
    }
    else if (fileName != "") 
      myRing = xmlFecCcu->getRingFromBuffer (fecHardwareId, ring) ;
    
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLFecCcu parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
    std::cout << "-- Retreive one ring : " << myRing->getNumberOfCcus() << " CCUs = " << duration << " ms" << std::endl ;
  
#ifndef SAXPARSER
    // std::cout << "Found " << xmlFecCcu->getCountCCU() << " CCU" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to retreive the Ring: " << e.what() << std::endl ;
  }
    
  try {
    // this method handle the remove of the vectors
    if (dbAccess != NULL) delete dbAccess ;
    if (xmlFecCcu != NULL) delete xmlFecCcu ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }
  
  return 0 ;
}

/** test the DCU access
 */
int testDCU ( bool databaseUsed, std::string partitionName, std::string fileName ) {

  if (databaseUsed) {
    std::cout << "Retreive the DCU values (timestamp > 0 and version = 0.0) from the database for the partition " << partitionName ;
    std::cout << std::endl ;
  }
  else if (fileName != "") {
    std::cout << "Retreive the DCU from the file " << fileName ;
    std::cout << std::endl ;
  }

  DbFecAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbFecAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbFecAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else if (fileName == "") {
    
    std::cerr << "Please specify or <-partition [PARTITION NAME] -database> or <-file [FILENAME]>" << std::endl ;
    return -1 ;
  }

  // ---------------------------------------------------
  // XML parsing
  XMLFecDcu *xmlFecDcu = NULL ;

  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  
  try {
    if (databaseUsed)
      xmlFecDcu = new XMLFecDcu (dbAccess) ; // from 1 cuase the DCU timestamp > 0 for the DCU values, if you want to download DCU with the version, try TestDevices method
    else if (fileName != "") 
      xmlFecDcu = new XMLFecDcu (fileName) ; // from 0 cause the DCU timestamp can be zero
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }

  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLFecDcu = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    deviceVector mesDevices ;
    if (databaseUsed) {
      mesDevices = xmlFecDcu->getDevices (partitionName,1,0xFFFFFFFF) ; // from 1 cause the DCU timestamp > 0 for the DCU values, if you want to download DCU with the version, try TestDevices method
    }
    else if (fileName != "") {
      mesDevices = xmlFecDcu->getDevices (0,0xFFFFFFFF) ;
    }
    
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLFecDcu parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
    std::cout << "-- Retreive " << mesDevices.size() << " DCUs = " << duration << " ms" << std::endl ;

#ifndef SAXPARSER
    std::cout << "Found " << xmlFecDcu->getCountPIA() << " PIA" << std::endl ;
    std::cout << "Found " << xmlFecDcu->getCountAPV25() << " APV25" << std::endl; 
    std::cout << "Found " << xmlFecDcu->getCountAPVMUX() << " APVMUX" << std::endl ;
    std::cout << "Found " << xmlFecDcu->getCountDCUCCU() << " DCU on CCU" << std::endl ;
    std::cout << "Found " << xmlFecDcu->getCountDCUFEH() << " DCU on FEH" << std::endl ;
    std::cout << "Found " << xmlFecDcu->getCountAOH() << " AOH" << std::endl ;
    std::cout << "Found " << xmlFecDcu->getCountDOH() << " DOH" << std::endl ;
    std::cout << "Found " << xmlFecDcu->getCountPLL() << " PLL" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Error during the parsing of the devices: " << e.what() << std::endl ;
  }

  try {
    // this method handle the remove of the vectors
    if (dbAccess != NULL) delete dbAccess ;
    if (xmlFecDcu != NULL) delete xmlFecDcu ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }
  
  return 0 ;
}

/** test the PIA reset
 */
int testPiaReset ( bool databaseUsed, std::string partitionName, std::string fecHardwareId, std::string fileName ) {

  if (databaseUsed) {
    std::cout << "Retreive the PIA reset from the database for the partition " << partitionName ;
    if (fecHardwareId != "") 
      std::cout << " and for the FEC " << fecHardwareId ;
    std::cout << std::endl ;
  }
  else if (fileName != "") {
    std::cout << "Retreive the PIA reset from the file " << fileName ;
    if (fecHardwareId != "") 
      std::cout << " and for the FEC " << fecHardwareId ;
    std::cout << std::endl ;
  }

  DbPiaResetAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
    
    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbPiaResetAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbPiaResetAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else if (fileName == "") {
    
    std::cerr << "Please specify or <-partition [PARTITION NAME] -database> or <-file [FILENAME]>" << std::endl ;
    return -1 ;
  }

  // ---------------------------------------------------
  // XML parsing
  XMLFecPiaReset *xmlFecPiaReset = NULL ;
  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  
  try {
    if (databaseUsed)
      xmlFecPiaReset = new XMLFecPiaReset (dbAccess) ;
    else if (fileName != "")
      xmlFecPiaReset = new XMLFecPiaReset (fileName) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }

  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLFecPiaReset = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  try {  
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    piaResetVector mesDevices ;
    if (databaseUsed) {
      if (fecHardwareId == "")
	mesDevices = xmlFecPiaReset->getPiaResets (partitionName) ;
      else 
	mesDevices = xmlFecPiaReset->getPiaResets (partitionName,fecHardwareId) ;
    }
    else if (fileName != "") 
      mesDevices = xmlFecPiaReset->getPiaResets () ;

    FecFactory::deleteVectorI(mesDevices) ;

    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLFecPiaReset parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
    std::cout << "-- Retreive " << mesDevices.size() << " PIA reset = " << duration << " ms" << std::endl ;
  
#ifndef SAXPARSER
    std::cout << "Found " << xmlFecPiaReset->getCountPIA() << " PIA" << std::endl ;
    std::cout << "Found " << xmlFecPiaReset->getCountAPV25() << " APV25" << std::endl; 
    std::cout << "Found " << xmlFecPiaReset->getCountAPVMUX() << " APVMUX" << std::endl ;
    std::cout << "Found " << xmlFecPiaReset->getCountDCUCCU() << " DCU on CCU" << std::endl ;
    std::cout << "Found " << xmlFecPiaReset->getCountDCUFEH() << " DCU on FEH" << std::endl ;
    std::cout << "Found " << xmlFecPiaReset->getCountAOH() << " AOH" << std::endl ;
    std::cout << "Found " << xmlFecPiaReset->getCountDOH() << " DOH" << std::endl ;
    std::cout << "Found " << xmlFecPiaReset->getCountPLL() << " PLL" << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Error during the parsing of the PIA devices: " << e.what() << std::endl ;
  }

  try {
    // this method handle the remove of the vectors
    if (dbAccess != NULL) delete dbAccess ;
    if (xmlFecPiaReset != NULL) delete xmlFecPiaReset ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }
  
  return 0 ;
}

/** test the devices
 * command: -devices
 */
int testDevices ( bool databaseUsed, std::string partitionName, std::string fecHardwareId, std::string fileName, bool upload ) {

  if (databaseUsed) {
    std::cout << "Retreive the FEC devices from the database for the partition " << partitionName ;
    if (fecHardwareId != "") 
      std::cout << " and for the FEC " << fecHardwareId ;
    std::cout << std::endl ;
  }
  else if (fileName != "") {
    std::cout << "Retreive the FEC devices from the file " << fileName ;
    if (fecHardwareId != "") 
      std::cout << " and for the FEC " << fecHardwareId ;
    std::cout << std::endl ;
  }

  DbFecAccess *dbAccess = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;
    
  if (databaseUsed) {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      // create the database access
      dbAccess = new DbFecAccess ( login, passwd, path ) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Creation of DbFecAccess = " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else if (fileName == "") {
    
    std::cerr << "Please specify or <-partition [PARTITION NAME] -database> or <-file [FILENAME]>" << std::endl ;
    return -1 ;
  }

  // ---------------------------------------------------
  // XML parsing
  XMLFecDevice *xmlFecDevice = NULL ;

  unsigned int duration = 0 ;
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

  try {
    if (databaseUsed)
      xmlFecDevice = new XMLFecDevice (dbAccess) ;
    else if (fileName != "") 
      xmlFecDevice = new XMLFecDevice (fileName) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to create the parser: " << e.what() << std::endl ;
    return -1 ;
  }

  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Creation of XMLFecDevice = " << (endMillis-startMillis) << " ms" << std::endl ;
  duration += (endMillis-startMillis) ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    deviceVector mesDevices ;
    if (databaseUsed) {
      if (fecHardwareId == "")
	mesDevices = xmlFecDevice->getDevices (partitionName) ;
      else 
	mesDevices = xmlFecDevice->getDevices (partitionName,fecHardwareId) ;
    }
    else if (fileName != "") 
      mesDevices = xmlFecDevice->getDevices () ;
    
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "XMLFecDevice parsing = "<< (endMillis-startMillis) << " ms" << std::endl ;
    duration += (endMillis-startMillis) ;
    std::cout << "-- Retreive " << mesDevices.size() << " devices = " << duration << " ms" << std::endl ;

    FecFactory::deleteVectorI (mesDevices) ;
    
#ifndef SAXPARSER
    std::cout << "Found " << xmlFecDevice->getCountPIA() << " PIA" << std::endl ;
    std::cout << "Found " << xmlFecDevice->getCountAPV25() << " APV25" << std::endl; 
    std::cout << "Found " << xmlFecDevice->getCountAPVMUX() << " APVMUX" << std::endl ;
    std::cout << "Found " << xmlFecDevice->getCountDCUCCU() << " DCU on CCU" << std::endl ;
    std::cout << "Found " << xmlFecDevice->getCountDCUFEH() << " DCU on FEH" << std::endl ;
    std::cout << "Found " << xmlFecDevice->getCountAOH() << " AOH" << std::endl ;
    std::cout << "Found " << xmlFecDevice->getCountDOH() << " DOH" << std::endl ;
    std::cout << "Found " << xmlFecDevice->getCountPLL() << " PLL" << std::endl ;
#endif

    // --------------------------------------------------------------------------------------------
    // Upload in the database
    if (upload) {
      if (databaseUsed) {
	if (fecHardwareId != "") std::cerr << "Upload is only available for partition" << std::endl ;
	else {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  xmlFecDevice->setDevices (partitionName,0) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload in database with devices coming from XMLFecDevice = " << (endMillis-startMillis) << " ms" << std::endl ;
	}
      }
      else {
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	xmlFecDevice->setDevices ("/tmp/XMLFecParseTestFileFromXML.xml") ;
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	std::cout << "Upload in file with devices coming from XMLFecDevice = " << (endMillis-startMillis) << " ms" << std::endl ;

	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	xmlFecDevice->setDevices (mesDevices, "/tmp/XMLFecParseTestFileFromLocal.xml") ;
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	std::cout << "Upload in file with devices coming from local = " << (endMillis-startMillis) << " ms" << std::endl ;
      }
    }
    // --------------------------------------------------------------------------------------------

  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Error during the parsing of the devices: " << e.what() << std::endl ;
  }
  
  try {
    // this method handle the remove of the vectors
    if (dbAccess != NULL) delete dbAccess ;
    if (xmlFecDevice != NULL) delete xmlFecDevice ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
    return -1 ;
  }
  
  return 0 ;
}

#ifdef OLDCONNECTIONVERSION
/** Test the connections
 */
int testConnectionOld ( std::string partitionName ) {
  
  // Database connection
  FedFecConnectionDeviceFactory *deviceFactory ;

  // ***************************************************************************************************
  // Create the database access
  // ***************************************************************************************************
  // retreive the connection of the database through an environmental variable called CONFDB
  std::string login="nil", passwd="nil", path="nil" ;
  DbAccess::getDbConfiguration (login, passwd, path) ;

  if (login == "nil" || passwd=="nil" || path=="nil") {

    std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  unsigned int i = 1 ;  
  // ---------------------------------------------------
  try {
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    // create the database access
    deviceFactory = new FedFecConnectionDeviceFactory ( login, passwd, path ) ;
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Creation of the FedFecConnectionDeviceFactory = " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  try {

    //while (true) {
    {
      
      deviceFactory->createInputDBAccess() ;
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->setInputDBVersion(partitionName) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Download of the connections took = " << (endMillis-startMillis) << " ms" << std::endl ;
      std::cout << "Access: " << i++ << std::endl ;
      sleep(1) ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "Error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Oracle error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  
  // ***************************************************************************************************************************
  // Delete the database access
  // ***************************************************************************************************************************
  try {
     
    // this method handle the remove of the vectors
    delete deviceFactory ;
  }  
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  return 0 ;
}
#endif

/** Test the connections
 */
int testConnection ( std::string partitionName, bool upload ) {
  
  // Database connection
  ConnectionFactory *deviceFactory ;

  // ***************************************************************************************************
  // Create the database access
  // ***************************************************************************************************
  // retreive the connection of the database through an environmental variable called CONFDB
  std::string login="nil", passwd="nil", path="nil" ;
  DbAccess::getDbConfiguration (login, passwd, path) ;

  if (login == "nil" || passwd=="nil" || path=="nil") {

    std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  // ---------------------------------------------------
  try {
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    // create the database access
    deviceFactory = new ConnectionFactory ( login, passwd, path ) ;
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Creation of the ConnectionFactory = " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  try {

    ConnectionVector connectionVector ;
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    deviceFactory->getConnectionDescriptions ( partitionName, connectionVector ) ;
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Download of the connections took = " << (endMillis-startMillis) << " ms for " << connectionVector.size() << " connections" << std::endl ;

//     for (ConnectionVector::iterator it = connectionVector.begin() ; it != connectionVector.end() ; it ++) {
//       (*it)->display() ;
//     }

    if (upload) {
      // Upload the connection in database
      deviceFactory->setOutputFileName ("/tmp/connection.xml") ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->setConnectionDescriptions (connectionVector) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Upload in file /tmp/connection.xml done in " << (endMillis-startMillis) << " ms" << std::endl ;

      // Read from file
      ConnectionFactory connectionF;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      connectionF.setInputFileName ("/tmp/connection.xml") ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      ConnectionVector c1 ; connectionF.getConnectionDescriptions(c1, true) ;
      std::cout << "Found " << c1.size() << " connections in file " << "/tmp/connection.xml in " << (endMillis-startMillis) << " ms" << std::endl ;

      // Partition / version
      unsigned int versionMajor = 0, versionMinor = 0, maskMajor = 0, maskMinor = 0, id ;
      deviceFactory->getPartitionVersion ( partitionName, &versionMajor, &versionMinor, &maskMajor, &maskMinor, &id) ;
      std::cout << "Current version is " << versionMajor << "." << versionMinor << " (mask is " << maskMajor << "." << maskMinor << ")" << " for partition " << partitionName << std::endl ;

      // Upload it in database in a new major version
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->setConnectionDescriptions (connectionVector, partitionName, &versionMajor, &versionMinor, true ) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Create a new connection version " << versionMajor << "." << versionMinor << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      
      // Upload it in database in a new minor version
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->setConnectionDescriptions (connectionVector, partitionName, &versionMajor, &versionMinor, false ) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Create a new connection version " << versionMajor << "." << versionMinor << " in " << (endMillis-startMillis) << " ms" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "Error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Oracle error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  
  // ***************************************************************************************************************************
  // Delete the database access
  // ***************************************************************************************************************************
  try {
     
    // this method handle the remove of the vectors
    delete deviceFactory ;
  }  
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  return 0 ;
}


/** 
 * -partition MORESLC4_1 -database -devices
 * 
 */
int main ( int argc, char **argv ) {

  // Which partition you want to use ...
  std::string partitionName = "AgainOnTheRoadTest" ;
  std::string fecHardwareId = "" ;
  std::string fileName = "" ;
  unsigned int ring = 0, dcuHardId = 0, runNumber = 0 ;

  bool databaseUsed = false ;

  bool pia = false, devices = false, dcu = false, ccu = false ;
  bool dcuConversion = false, dcuInfo = false ;
  bool state = false ;
  bool fecVersion = false, fedVersion = false, connectionVersion = false, dcupsumapVersion = false, detidVersion = false ;
  bool run = false, runO2O = false ;
  bool upload = false ;
  bool connection = false ;

  // Find the options
  for (int i = 1 ; i < argc ; i ++) {

    std::string param ( argv[i] ) ;
    if (param == "-partition") {

      if (i < argc) {
	partitionName = std::string(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the partition name after the option -partition" << std::endl ;
      
    }
    else if (param == "-fecHardwareId") {

      if (i < argc) {
	fecHardwareId = std::string(argv[i+1]) ;
	i ++ ;
      }
      else 
	std::cerr << "Error: you must specify the FEC hardware ID after the option -fecHardwareId" << std::endl ;
    }
    else if (param == "-ring") {

      if (i < argc) {
	ring = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the ring number to be used" << std::endl ;
    }
    else if (param == "-dcuHardId") {

      if (i < argc) {
	dcuHardId = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the ring number to be used" << std::endl ;
    }
    else if (param == "-runNumber") {

      if (i < argc) {
	runNumber = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the ring number to be used" << std::endl ;
    }
    else if (param == "-help") {

      std::cout << argv[0] << std::endl ;
      std::cout << "  -partition <partition name> [-fecHardwareId <FEC hardware ID>]" << std::endl ; 
      return 0 ;
    }
    else if (param == "-database") {
      databaseUsed = true ;
    }
    else if (param == "-file") {

      if (i < argc) {
	fileName = std::string(argv[i+1]) ;
	i ++ ;
      }
    }
    else if (param == "-all") {
      dcu = true ; pia = true ; devices = true ;
    }
    else if (param == "-dcu") {
      dcu = true ;
    }
    else if (param == "-devices") {
      devices = true ;
    }
    else if (param == "-pia") {
      pia = true ;
    }
    else if (param == "-ccu") {
      ccu = true ;
    }
    else if (param == "-dcuconversion") {
      dcuConversion = true ;
    }
    else if (param == "-dcuinfo") {
      dcuInfo = true ;
    }
    else if (param == "-state") {
      state = true ;
    }
    else if (param == "-fecversion") {
      fecVersion = true ;
    }
    else if (param == "-fedversion") {
      fedVersion = true ;
    }
    else if (param == "-connectionversion") {
      connectionVersion = true ;
    }
    else if (param == "-dcupsumapversion") {
      dcupsumapVersion = true ;
    }
    else if (param == "-detidversion") {
      detidVersion = true ;
    }
    else if (param == "-run") {
      run = true ;
    }
    else if (param == "-runO2O") {
      runO2O = true ;
    }
    else if (param == "-connection") {
      connection=true ;
    }
    else if (param == "-upload") {
      upload = true ;
    }
    else {

      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
    }
  }

  // ***************************************************************************************************
  if (devices) return testDevices  ( databaseUsed, partitionName, fecHardwareId, fileName, upload ) ;
  if (pia) return testPiaReset ( databaseUsed, partitionName, fecHardwareId, fileName ) ;
  if (dcu) return testDCU      ( databaseUsed, partitionName, fileName ) ;
  if (ccu) return testCCU      ( databaseUsed, partitionName, fecHardwareId, fileName, ring ) ;
  if (dcuConversion) return testDCUConversion ( databaseUsed, partitionName, dcuHardId, fileName ) ;
  if (dcuInfo) return testDCUInfo ( databaseUsed, partitionName, fileName ) ;
  if (state) return testState (databaseUsed) ;
  if (fecVersion || fedVersion || connectionVersion || dcupsumapVersion || detidVersion) return testVersion (databaseUsed,partitionName,fecVersion,fedVersion,connectionVersion, dcupsumapVersion, detidVersion) ;
  if (run || runO2O) return testRun (databaseUsed,partitionName,runNumber,runO2O) ;
  if (connection) return testConnection (partitionName,upload) ;

  return 0 ;
}
