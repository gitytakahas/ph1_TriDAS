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
#include "DeviceFactory.h"       // devicefactory
#include "MemBufOutputSource.h"  // XML buffer

#include <stdlib.h>

/** \brief display current state
 */
void displayCurrentState ( tkStateVector dbStates ) ;

/** -----------------------------------------------------------
 */
int main ( int argc, char **argv ) {

  // Which partition you want to use ...
  std::string partitionName = "ALL" ;
  unsigned int runNumber = 0, runMode = 0 ;
  std::string comment = "Inserted by hand" ;
  unsigned int choix = 0 ;
  std::string hostname = "" ;
  unsigned int slot = 0xFFFFFFFF ;

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
    else if (param == "-hostname") {

      if (i < argc) {
	hostname = std::string(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the partition name after the option -hostname" << std::endl ;
    } 
    else if (param == "-slot") {

      if (i < argc) {
	slot = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
      }
      else {
	std::cerr << "Error: you must specify the run number with the option -slot" << std::endl ;
	return 0 ;
      }
    }
    else if (param == "-runNumber") {

      if (i < argc) {
	runNumber = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
      }
      else {
	std::cerr << "Error: you must specify the run number with the option -runNumber" << std::endl ;
	return 0 ;
      }
    }
    else if (param == "-runMode") {

      if (i < argc) {
	std::string runModeStr = std::string(argv[i+1]) ;
	i ++ ;

	if (runModeStr == "PHYSIC") runMode = 1 ;
	else if (runModeStr == "PEDESTAL") runMode = 2 ;
	else if (runModeStr == "CALIBRATION") runMode = 3 ;
	else if (runModeStr == "CALIBRATION_DECO") runMode = 33 ;
	else if (runModeStr == "GAINSCAN") runMode = 4 ;
	else if (runModeStr == "TIMING") runMode = 5 ;
	else if (runModeStr == "LATENCY") runMode = 6 ;
	else if (runModeStr == "DELAY") runMode = 7 ;
	else if (runModeStr == "PHYSIC10") runMode = 10 ;
	else if (runModeStr == "CONNECTION") runMode = 11 ;
	else if (runModeStr == "DELAY_TTC") runMode = 8 ;
	else if (runModeStr == "TIMING_FED") runMode = 12 ;
	else if (runModeStr == "BARE_CONNECTION") runMode = 13 ;
	else if (runModeStr == "VPSPSCAN") runMode = 14 ;
	else if (runModeStr == "SCOPE") runMode = 15 ;
	else if (runModeStr == "FAST_CONNECTION") runMode = 16 ;
	else if (runModeStr == "DELAY_LAYER") runMode = 17 ;
	else if (runModeStr == "PHYSIC_ZERO_SUPPRESSION") runMode = 18 ;
	else {
	  std::cerr << "Error: run type unknown" << std::endl ;
	  return 0 ;
	}
      }
      else 
	std::cerr << "Error: you must specify the run mode with the option -runMode" << std::endl ;
    }
    else if (param == "-comment") {
      if (i < argc) {
	comment = std::string(argv[i+1]) ;
	i ++ ;
      }
      else {
	std::cerr << "Error: add a comment after the command -comment" << std::endl ;
	return 0 ;
      }
    }
    else if (param == "-setRun") {
      choix = 1 ; 
    }
    else if (param == "-displayPartition") {
      choix = 2 ;
    }
    else if (param == "-displayVersion") {
      choix = 3 ;
    }
    else if (param == "-displayStateName") {
      choix = 4 ;
    }
    else if (param == "-displayCurrentState") {
      choix = 5 ;
    }
    else if (param == "-setCurrentState") {
      choix = 6 ;
    }
    else if (param == "-displayRun") {
      choix = 7 ;
    }
    else if (param == "-idVsHostname") {
      choix = 8 ;
    }
    else if (param == "-dcupsumap") {
      choix = 9 ;
    }
    else if (param == "-displayPartitionFromCurrentState") {
      choix = 10 ;
    }
    else if (param == "-help") {

      std::cout << argv[0] << std::endl ;
      std::cout << "\t" << "-partition <partition name> -runNumber <value> -runMode <name> -comment <comment> -setRun" << std::endl ; 
      std::cout << "\t\t" << "runMode: PHYSIC, PEDESTAL, CALIBRATION, CALIBRATION_DECO, GAINSCAN, TIMING, LATENCY, DELAY, PHYSIC10, CONNECTION, DELAY_TTC, TIMING_FED, BARE_CONNECTION, VPSPSCAN, SCOPE, FAST_CONNECTION DELAY_LAYER PHYSIC_ZERO_SUPPRESSION" << std::endl ;
      std::cout << "\t" << "[-runNumber <value>] -displayPartition: display all partition names" << std::endl ;
      std::cout << "\t" << "-displayPartitionFromCurrentState: display all partition names from the current state" << std::endl ;
      std::cout << "\t" << "-displayStateName: display all state names" << std::endl ;
      std::cout << "\t" << "-displayCurrentState: display the current state" << std::endl ;
      std::cout << "\t" << "[-partition <partition name>] -setCurrentState: change the version for the first partition in the state" << std::endl ;
      std::cout << "\t" << "[-partition <partition name>] [-runNumber <RUN>] -displayRun: display the latest run and the number of runs for a given partition. Display only the version if you specify a run number." << std::endl ;
      std::cout << "\t" << "[-hostname <hostname>] [-slot <VME slot>] -idVsHostname: display the latest run and the number of runs" << std::endl ;
      std::cout << "\t" << "[-partition <partition name>] -dcupsumap: display the PSU DCU map" << std::endl ;
      return 0 ;
    }
    else {

      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
    }
  }

  // ***************************************************************************************************
  // Create the database access
  // ***************************************************************************************************
  // retreive the connection of the database through an environmental variable called CONFDB
  DeviceFactory *deviceFactory ;
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
    deviceFactory = new DeviceFactory ( login, passwd, path ) ;
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Creation of the DeviceFactory " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  try {
    switch (choix) {
    case 1: { // set a run number: -setRun
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->setRun(partitionName,runNumber,runMode,1,comment) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Set run took " << (endMillis-startMillis) << " ms" << std::endl ;

      // set the comment at the end of the run
      deviceFactory->stopRun(partitionName,"stop the run") ;
      break ;
    }
    case 2: { // display all partition names: -displayPartition with the run number if it is specified
      for (int i = 0 ; i < 2 ; i ++) {
	unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	std::list<std::string> partitionNames ;
	if (runNumber == 0) partitionNames = deviceFactory->getAllPartitionNames() ;
	else partitionNames = deviceFactory->getAllPartitionNames(runNumber) ;
	unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	if (i == 0) std::cout << "List of partitions: " << std::endl ;
	for (std::list<std::string>::iterator it = partitionNames.begin() ; it != partitionNames.end() ; it ++) {
	  if (i == 0) std::cout << "\t" << *it << std::endl ;
	}
	if (i == 1) std::cout << "Get the list of partitions took " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      break ;
    }
    case 3: { // display all partition names with the number of versions: -displayVersion
      for (int i = 0 ; i < 2 ; i ++) {
	std::list<std::string> partitionNames ;

	if (partitionName == "ALL") partitionNames = deviceFactory->getAllPartitionNames() ;
	else partitionNames.push_back(partitionName) ;
	
	if (i == 0) std::cout << "Partition name \t #FEC \t #FED \t #DCUInfo \t #DCUPSUMap \t #Mask" << std::endl ;
	unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	for (std::list<std::string>::iterator it = partitionNames.begin() ; it != partitionNames.end() ; it ++) {
	  tkVersionVector fecVersions, fedVersions, connectionVersions, dcuInfoVersions, dcuPsuMapVersions, maskVersions ;
	  fecVersions = deviceFactory->getAllFecVersions(*it) ;
	  fedVersions = deviceFactory->getAllFedVersions(*it) ;
	  connectionVersions = deviceFactory->getAllConnectionVersions(*it) ;
	  dcuInfoVersions = deviceFactory->getAllDcuInfoVersions(*it) ;
	  dcuPsuMapVersions = deviceFactory->getAllDcuPsuMapVersions(*it) ;
	  maskVersions = deviceFactory->getAllMaskVersions(*it);
	  if (i == 0) std::cout << *it << "\t\t" << fecVersions.size() << "\t" << fedVersions.size() << "\t" << connectionVersions.size() << "\t" << dcuInfoVersions.size() << "\t" << dcuPsuMapVersions.size() << "\t" << maskVersions.size() << std::endl ;

// 	  if (fecVersions.size()) {
// 	    tkVersionVector::iterator it = fecVersions.begin() ;
// 	    TkVersion *vers = *it ;
// 	    std::cout << vers->getVersionMajorId() << "." << vers->getVersionMinorId() << " created " << vers->getVersionDate() << std::endl ;
// 	  }
	}
	unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	//if (i == 0) std::cout << "Retreiving all versions for " << partitionNames.size() << " partitions took " << (endMillis-startMillis) << " ms" << std::endl ;
	if (i != 0) std::cout << "Retreiving all versions for " << partitionNames.size() << " partitions took " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      break ;
    }
    case 4: { // display state name: -displayStateName
      for (int i = 0 ; i < 2 ; i ++) {
	unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	std::list<std::string> stateNameList = deviceFactory->getAllStateHistoryNames();
	unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	if (i == 0) {
	  for (std::list<std::string>::iterator it = stateNameList.begin() ; it != stateNameList.end() ; it ++) 
	    std::cout << *it << std::endl ;
	}
	else std::cout << "Get the list of state names tooks " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      break ;
    }
    case 5: { // get current state: -displayCurrentState
      for (int i = 0 ; i < 2 ; i ++) {
	unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	tkStateVector dbStates = deviceFactory->getCurrentStates () ;
	unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	if (i == 0) {
	  std::cout << "Partition name: \t FEC version \t FED version \t Connection version \t DCU/DET ID version \t DCU/PSU map version \t Analysis version \t Mask version" << std::endl ;
	  displayCurrentState ( dbStates ) ;
	}
	else std::cout << "Get the current state tooks " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      break ;
    }
    case 6: { // set the current state: -setCurrentState
      // retreive the list of state name and choose the first one to create a name
      std::list<std::string> stateNameList = deviceFactory->getAllStateHistoryNames();
      std::string stateName = "StateNameForTest" ;
      if (stateNameList.size()) {
	stateName = *(stateNameList.begin()) ;
	srandom(time(NULL)) ;
	long randLib = random() ;
	stateName = stateName + toString(randLib) ;
      }

      // Retreive the current state
      tkStateVector dbStates = deviceFactory->getCurrentStates () ;

      // set a name for the current state
      for (tkStateVector::iterator it = dbStates.begin() ; it != dbStates.end() ; it ++) {
	TkState *tkState = *it ;
	tkState->setStateName(stateName) ;
      }
      deviceFactory->setCurrentState ( dbStates ) ;

      std::cout << "Current state is (state name = " << stateName << "): " << std::endl ;
      displayCurrentState ( dbStates ) ;
      std::cout << "Press enter to continue" ; getchar() ;

      // Change the state
      if (dbStates.empty())
	std::cerr << "No partition in the current state" << std::endl ;
      else {
	// Retreive the partition
	TkState *tkState = *(dbStates.begin()) ;
	if (partitionName == "ALL") partitionName = tkState->getPartitionName() ;

	std::cout << "Will change the version of partition " << partitionName << std::endl ;

	tkVersionVector fecVersions = deviceFactory->getAllFecVersions(partitionName) ;
	if (fecVersions.empty()) 
	  std::cerr << "No version found for the partition " << partitionName << std::endl ;
	else {

	  // Display the current state
	  displayCurrentState ( dbStates ) ;

	  // set the new current state
	  TkVersion *tkVersion = *(fecVersions.begin()) ;

	  std::cout << "Version of partition " << partitionName << " is " << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << std::endl ;
	  std::cout << "Press enter to continue" ; getchar() ;

	  tkStateVector dbStatesToBeSet ;
	  for (tkStateVector::iterator it = dbStates.begin() ; it != dbStates.end() ; it ++) {
	    if ((*it)->getPartitionName() == partitionName) {
	      TkState *tkState = new TkState (stateName + std::string(".1"), partitionName, 
					      tkVersion->getVersionMajorId(), tkVersion->getVersionMinorId(),
					      (*it)->getFedVersionMajorId(), (*it)->getFedVersionMinorId(),
					      (*it)->getConnectionVersionMajorId(), (*it)->getConnectionVersionMinorId(),
					      (*it)->getDcuInfoVersionMajorId(), (*it)->getDcuInfoVersionMinorId(),
					      (*it)->getDcuPsuMapVersionMinorId(), (*it)->getDcuPsuMapVersionMinorId(),
					      (*it)->getAnalysisVersionMapPointerId(),
					      (*it)->getMaskVersionMajorId(), (*it)->getMaskVersionMinorId() ) ;
	      dbStatesToBeSet.push_back(tkState) ;
	    }
	    else {
	      dbStatesToBeSet.push_back((*it)->clone()) ;
	    }
	  }

	  // Upload it
	  deviceFactory->setCurrentState(dbStatesToBeSet) ;
	  TkDcuInfoFactory::deleteStateVectorI(dbStatesToBeSet) ;

	  // display the current state
	  tkStateVector dbStatesSet = deviceFactory->getCurrentStates () ;	  
	  std::cout << "Display the current state just uploaded: " << std::endl ;
	  displayCurrentState(dbStatesSet) ;
	  std::cout << "Press enter to continue" ; getchar() ;

	  // go back to the previous state name
	  deviceFactory->setCurrentState(stateName) ;

	  // display the current state again
	  std::cout << "Go back to the state " << stateName << std::endl ;
	  tkStateVector dbStatesBis = deviceFactory->getCurrentStates () ;	  
	  displayCurrentState(dbStatesBis) ;
	  std::cout << "Press enter to continue" ; getchar() ;
	}
      }
      break ;
    }
    case 7: { // display all runs: -displayRun
      for (int i = 0 ; i < 2 ; i ++) {
	if (runNumber == 0) {
	  if (partitionName != "ALL") {
	    TkRun *tkRun = deviceFactory->getLastRun(partitionName) ;
	    if (i == 0) {
	      if (tkRun != NULL) {
		std::cout << "Latest run number used is: " << tkRun->getRunNumber() << " for the partition " << partitionName << std::endl ;
		std::cout << "State name = " << tkRun->getStateName() << " for partition " << tkRun->getPartitionName() << " stated the " << tkRun->getStartingTime() << " and terminated " << tkRun->getEndingTime() << std::endl ;
		std::cout << "Version used: "
			  << tkRun->getFedVersionMajorId() << "." << tkRun->getFedVersionMinorId() << "\t"
			  << tkRun->getFecVersionMajorId() << "." << tkRun->getFecVersionMinorId() << "\t"
			  << tkRun->getConnectionVersionMajorId() << "." << tkRun->getConnectionVersionMinorId() << "\t"
			  << tkRun->getDcuInfoVersionMajorId() << "." << tkRun->getDcuInfoVersionMinorId() << "\t"
			  << tkRun->getDcuPsuMapVersionMinorId() << "." << tkRun->getDcuPsuMapVersionMinorId() << std::endl ;
		tkRun = deviceFactory->getRun (partitionName,tkRun->getRunNumber()) ;
	      }
	      else std::cerr << "No runs found for the partition " << partitionName << std::endl ;
	    }
	  }
	  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  tkRunVector runV = deviceFactory->getAllRuns() ;
	  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  if (i == 1) std::cout << "Found " << runV.size() << " runs in the database tooks " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  std::list<std::string> partitionNames ;
	  if (partitionName == "ALL") {
	    partitionNames = deviceFactory->getAllPartitionNames(runNumber) ;
	    std::cout << "Found " << partitionNames.size() << " partitions for the run " << runNumber << std::endl ;
	  }
	  else partitionNames.push_back(partitionName) ;
	  for (std::list<std::string>::iterator it = partitionNames.begin() ; it != partitionNames.end() ; it ++) {
	    TkRun *tkRun = deviceFactory->getRun(*it,runNumber) ;
	    if (i == 0) {
	      if (tkRun != NULL) {
		std::cout << "Run number used is: " << tkRun->getRunNumber() << " for the partition " << *it << std::endl ;
		std::cout << "State name = " << tkRun->getStateName() << " for partition " << tkRun->getPartitionName() << " stated the " << tkRun->getStartingTime() << " and terminated " << tkRun->getEndingTime() << std::endl ;
		std::cout << "Version used: "
			  << tkRun->getFedVersionMajorId() << "." << tkRun->getFedVersionMinorId() << "\t"
			  << tkRun->getFecVersionMajorId() << "." << tkRun->getFecVersionMinorId() << "\t"
			  << tkRun->getConnectionVersionMajorId() << "." << tkRun->getConnectionVersionMinorId() << "\t"
			  << tkRun->getDcuInfoVersionMajorId() << "." << tkRun->getDcuInfoVersionMinorId() << "\t"
			  << tkRun->getDcuPsuMapVersionMinorId() << "." << tkRun->getDcuPsuMapVersionMinorId() << std::endl ;
		
		std::cout << "Run infos: "  << std::endl ;
		tkRun->display() ;
		
	      }
	      else std::cerr << "No information found in run " << runNumber << " for the partition " << partitionName << std::endl ;
	    }
	  }
	}
      }
      break ;
    }
    case 8: {
      for (int i = 0 ; i < 2 ; i ++) {

	unsigned int vM, vN ;
	((DbTkIdVsHostnameAccess *)((TkIdVsHostnameFactory *)deviceFactory)->getDatabaseAccess())->getTkIdVsHostnameVersion(vM,vN) ;
	std::cout << "Version: " << vM << "." << vN << std::endl ;

 	if (hostname != "") { // display the subdetector
	  try {
	    std::string subDetector = "" ;
	    unsigned int crateNumber = 0 ;
	    deviceFactory->getSubDetectorCrateNumberFromHostname(hostname,subDetector,crateNumber);
	    if (i == 0) 
	      std::cout << "The machine " << hostname << " is with the subdetector " << subDetector << " and the crate number is " << crateNumber << std::endl ;
	  }
	  catch (FecExceptionHandler &e) {
	    std::cerr << "FecExceptionHandler: Error in retreiving the subdetector and the crate number from hostname " << hostname << ": " << e.what() << std::endl ;
	  }
	  catch (oracle::occi::SQLException &e) {
	    std::cerr << "oracle::occi::SQLException: Error in retreiving the subdetector and the crate number from hostname " << hostname << ": " << e.what() << std::endl ;
	  }
 	}

	if ( (hostname == "") && (slot == 0xFFFFFFFF) ) { // All TkIdVsHostname
	  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  TkIdVsHostnameVector tkId = deviceFactory->getAllTkIdVsHostname ( ) ;
	  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  if (i == 0) std::cout << "Found " << tkId.size() << " ID vs hostname and it tooks " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else if ( (hostname != "") && (slot == 0xFFFFFFFF) ) { // All TkIdVsHostname for a given hostname
	  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  TkIdVsHostnameVector tkId = deviceFactory->getAllTkIdFromHostname ( hostname ) ;
	  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  if (i == 0) std::cout << "Found " << tkId.size() << " ID vs hostname for the hostname " << hostname << " and it tooks " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else { // ID for a given hostname & slot 
	  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  TkIdVsHostnameDescription *tkId = deviceFactory->getAllTkIdFromHostnameSlot ( hostname, slot ) ;
	  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  if (i == 0) {
	    if (tkId == NULL) {
	      std::cout << "No data for hostname " << hostname << " and slot " << slot << std::endl ;
	    }
	    else {
	      tkId->display() ;
	      std::cout << "It was retreived in " << (endMillis-startMillis) << " ms" << std::endl ;
	    }
	  }
	}

 	if (hostname != "") { // display the subdetector
	  std::string subDetector = "" ;
	  unsigned int crateNumber = 0 ;
	  deviceFactory->getSubDetectorCrateNumberFromHostname(hostname,subDetector,crateNumber);
	  if (i == 0) 
 	    std::cout << "The machine " << hostname << " is with the subdetector " << subDetector << " and the crate number is " << crateNumber << std::endl ;
 	}
      }
      break ;
    }
    case 9: { // -dcupsumap

      // --------------------------------------
      // Download PSU names
      std::cout << "Retreiving PSU name from database for partition " << partitionName << std::endl ;
      deviceFactory->getPsuNamePartition(partitionName) ;
      tkDcuPsuMapVector vDcuPsuCG = deviceFactory->getControlGroupDcuPsuMaps() ;
      tkDcuPsuMapVector vDcuPsuPG = deviceFactory->getPowerGroupDcuPsuMaps() ;

      std::cout << "Found " << vDcuPsuCG.size() << " control groups from database" << std::endl ;
      std::cout << "Found " << vDcuPsuPG.size() << " power groups from database" << std::endl ;    

      // --------------------------------------
      // Create fake DCU/PSU map
      
      if (vDcuPsuCG.size() && vDcuPsuPG.size()) {

	// Retreive DCU from 
	deviceVector vDevices ; deviceFactory->getFecDeviceDescriptions(partitionName, vDevices) ;
	deviceVector dcus = FecFactory::getDeviceFromDeviceVector(vDevices, DCU) ;
	std::cout << "Found " << dcus.size() << " DCUs over " << vDevices.size() << " FEC devices" << std::endl ;

	tkDcuPsuMapVector fakeDcuPsuMap ;
      
	// Loop over the DCUs
	tkDcuPsuMapVector::iterator itCG =  vDcuPsuCG.begin() ;
	tkDcuPsuMapVector::iterator itPG =  vDcuPsuPG.begin() ;
	for (deviceVector::iterator it = dcus.begin() ; it != dcus.end() ; it ++) {
	  unsigned dcuhardid = ((dcuDescription *)(*it))->getDcuHardId() ;
	  std::string dcuType = ((dcuDescription *)(*it))->getDcuType() ;
	  if (dcuType == DCUCCU) {
	    if (itCG == vDcuPsuCG.end()) itCG =  vDcuPsuCG.begin() ;
	    TkDcuPsuMap *dcuPsuMap = new TkDcuPsuMap ( dcuhardid, (*itCG)->getPsuName(), (*itCG)->getPsuType() ) ;
	    fakeDcuPsuMap.push_back(dcuPsuMap) ;
	    itCG ++ ;
	  }
	  else {
	    if (itPG == vDcuPsuPG.end()) itPG =  vDcuPsuPG.begin() ;
	    TkDcuPsuMap *dcuPsuMap = new TkDcuPsuMap ( dcuhardid, (*itPG)->getPsuName(), (*itPG)->getPsuType() ) ;
	    fakeDcuPsuMap.push_back(dcuPsuMap) ;
	    itPG ++ ;
	  }
	}
	
	// Upload it to database
	unsigned int versionMajor, versionMinor ;
	std::cout << "Upload of " << fakeDcuPsuMap.size() << " in database for partition " << partitionName << std::endl ;
	deviceFactory->setTkDcuPsuMap ( fakeDcuPsuMap, partitionName, &versionMajor, &versionMinor ) ;
	std::cout << "Version " << versionMajor << "." << versionMinor << " have been created" << std::endl ;
	
	// Download it from database
	deviceFactory->getDcuPsuMapPartition ( partitionName ) ;
	std::cout << "Found " << deviceFactory->getAllTkDcuPsuMaps().size() << " PSU/DCU maps in the database for partition " << partitionName << std::endl ;
	deviceFactory->getDcuPsuMapPartition ( partitionName, versionMajor, versionMinor ) ;
	std::cout << "Found " << deviceFactory->getAllTkDcuPsuMaps().size() << " PSU/DCU maps in the database for partition " << partitionName << std::endl ;
	
	// Generate text output for PVSS
	deviceFactory->setOutputPVSSFile ( fakeDcuPsuMap, "/tmp/" + partitionName + ".xml", "/tmp/" + partitionName + ".err" ) ;
	std::cout << "Upload in PVSS file done in " << "/tmp/" << partitionName << ".xml and errors in " << "/tmp/" << partitionName << ".err" << std::endl ;

	TkDcuPsuMapFactory::deleteVectorI(fakeDcuPsuMap) ;
      }
      else {
	std::cerr << "Please add control and power group in the partition to test DCU/PSU map" << std::endl ;
      }
      break ;
    }
    case 10: { // display all partition names: -displayPartitionFromCurrentState
      for (int i = 0 ; i < 2 ; i ++) {
	unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	std::list<std::string> partitionNames = deviceFactory->getAllPartitionNamesFromCurrentState() ;
	unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	if (i == 0) std::cout << "List of partitions: " << std::endl ;
	for (std::list<std::string>::iterator it = partitionNames.begin() ; it != partitionNames.end() ; it ++) {
	  if (i == 0) std::cout << "\t" << *it << std::endl ;
	}
	if (i == 1) std::cout << "Get the list of partitions took " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      break ;
    }
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "Error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Oracle error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
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
    std::cerr << e.what() << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  
  return 0 ;
}

/** Display the current state based on a vector of current state
 */
void displayCurrentState ( tkStateVector dbStates ) {

  for (tkStateVector::iterator it = dbStates.begin() ; it != dbStates.end() ; it ++) {
    TkState *tkState = *it ;
    std::cout << tkState->getPartitionName() << ": " 
	      << tkState->getFedVersionMajorId() << "." << tkState->getFedVersionMinorId() << "\t"
	      << tkState->getFecVersionMajorId() << "." << tkState->getFecVersionMinorId() << "\t"
	      << tkState->getConnectionVersionMajorId() << "." << tkState->getConnectionVersionMinorId() << "\t"
	      << tkState->getDcuInfoVersionMajorId() << "." << tkState->getDcuInfoVersionMinorId() << "\t"
	      << tkState->getDcuPsuMapVersionMinorId() << "." << tkState->getDcuPsuMapVersionMinorId() << "\t" 
	      << tkState->getAnalysisVersionMapPointerId() << "\t"
	      << tkState->getMaskVersionMajorId() << "." << tkState->getMaskVersionMinorId() << "\t" 
	      << std::endl ;

  }
}
