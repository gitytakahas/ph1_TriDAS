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
#ifndef UNKNOWNFILE
#define UNKNOWNFILE "UNKNOWNFILE"
#endif

#include <iostream>

#include <string.h> // strncasecmp
#include <time.h>   // time

#include "stringConv.h"
#include "deviceType.h"
#include "FecExceptionHandler.h"
#include "XMLFecDevice.h"
#include "XMLFecPiaReset.h"
#include "TkDcuConversionFactory.h"
  
  //#include "DeviceFactoryInterface.h"
#include "FecFactory.h"
#include "ErrorReportLogger.h"

#include "DbInterface.h"

/** Brief retreive login, password and path from env. variable CONFDB
 */
void getDbConfiguration(std::string confdb, std::string &login, std::string &passwd, std::string &path) {

  std::string::size_type ipass = confdb.find("/");
  std::string::size_type ipath = confdb.find("@");
  if ((ipass != std::string::npos) && (ipath != std::string::npos)) {
    login.clear();
    login=confdb.substr(0,ipass); 
    passwd.clear();
    passwd=confdb.substr(ipass+1,ipath-ipass-1); 
    path.clear();
    path=confdb.substr(ipath+1,confdb.size());
  }
}

/** Retreive all runs from an account and upload it to the CONFDB output
 * command: -uploadRuns
 */
void uploadRuns ( std::string confdbInput, std::string confdbOutput ) {

  std::string loginInput = "", passwdInput = "", pathInput = "" ;
  getDbConfiguration (confdbInput,loginInput,passwdInput,pathInput) ;
  
  if (loginInput.size() && passwdInput.size() && pathInput.size() ) {

    DeviceFactory *DbInput = NULL, *DbOutput = NULL ;
    
    try {

      // Create the access
      DbInput = new DeviceFactory (loginInput,passwdInput,pathInput,false) ;
      if (confdbOutput == "file") {
	DbOutput = new DeviceFactory() ;
	DbOutput->setUsingFile(true) ;
      }
      else {
	std::string loginOutput = "", passwdOutput = "", pathOutput = "" ;
	getDbConfiguration (confdbOutput,loginOutput,passwdOutput,pathOutput) ;
	if (loginOutput.size() && passwdOutput.size() && pathOutput.size() ) {
	  DbOutput = new DeviceFactory (loginOutput,passwdOutput,pathOutput,true) ;
	  std::cout << "Creating an access to database " << confdbOutput << std::endl ;
	}
      }

      // -------------------------------------------------------------------
      // Retreive all runs
      Sgi::hash_map<unsigned int, TkRun *> runExisting ;
      tkRunVector allRuns = DbInput->getAllRuns() ;
      std::cout << "Found " << std::dec << allRuns.size() << " runs in the input database " << std::endl ;
      // Classify the run
      for (tkRunVector::iterator it = allRuns.begin() ; it != allRuns.end() ; it ++) {
	runExisting[(*it)->getRunNumber()] = (*it) ;
      }

      // Check if the run number is already uploaded to the DB
      tkRunVector runE  = DbOutput->getAllRuns() ;
      std::cout << "Found " << std::dec << runE.size() << " runs in the output database " << std::endl ;
      for (tkRunVector::iterator it = runE.begin() ; it != runE.end() ; it ++) {
	if (runExisting.find((*it)->getRunNumber()) != runExisting.end()) {
	  std::cout << "Output DB: Run " << (*it)->getRunNumber() 
		    << " already existing in output database: Partition " << (*it)->getPartitionName()
		    << " FEC " << (*it)->getFecVersionMajorId() << "." << (*it)->getFecVersionMinorId()
		    << " FED " << (*it)->getFedVersionMajorId() << "." << (*it)->getFedVersionMinorId() << std::endl ;
	  std::cout << "Input DB:  Run " << runExisting[(*it)->getRunNumber()]->getRunNumber() 
		    << " already existing in output database: Partition " << runExisting[(*it)->getRunNumber()]->getPartitionName()
		    << " FEC " << runExisting[(*it)->getRunNumber()]->getFecVersionMajorId() << "." << runExisting[(*it)->getRunNumber()]->getFecVersionMinorId()
		    << " FED " << runExisting[(*it)->getRunNumber()]->getFedVersionMajorId() << "." << runExisting[(*it)->getRunNumber()]->getFedVersionMinorId() << std::endl ;
	  
	  runExisting[(*it)->getRunNumber()] = NULL ;
	}
      }

      // Get all the partition names already register in the CONFDBOUTPUT
      std::list<std::string> partitionNames = DbOutput->getAllPartitionNames() ;
      Sgi::hash_map<const char *, bool, Sgi::hash<const char*>, eqstr> partitionExisting ;
      for (std::list<std::string>::iterator it = partitionNames.begin() ; it != partitionNames.end() ; it ++) {
	std::string partition = (*it);
	partitionExisting[partition.c_str()] = true ;
      }

      std::cout << "Press enter to continue" ; getchar() ;

      unsigned long startMillis, endMillis ;

      for (Sgi::hash_map<unsigned int, TkRun *>::iterator it = runExisting.begin() ; it != runExisting.end() ; it ++) {

	if (it->second != NULL) {
	  unsigned int fecVersionMinorId = 0, fecVersionMajorId = 0 ;
	  unsigned int fedVersionMinorId = 0, fedVersionMajorId = 0 ;
	  unsigned int connectionVersionMinorId = 0, connectionVersionMajorId = 0 ;
	  
	  TkRun *run = (TkRun *)(it->second) ;
	  fecVersionMajorId = run->getFecVersionMajorId() ;
	  fecVersionMinorId = run->getFecVersionMinorId() ;
	  fedVersionMajorId = run->getFedVersionMajorId() ;
	  fedVersionMinorId = run->getFedVersionMinorId() ;
	  connectionVersionMajorId = run->getConnectionVersionMajorId() ;
	  connectionVersionMinorId = run->getConnectionVersionMinorId() ;

	  std::string partitionName = run->getPartitionName() ;
	  std::cout << "-------------------------------------------------- Next run: " << run->getRunNumber() << std::endl ;
	  std::cout << "Found a run " << run->getRunNumber() << " Partition " << partitionName
		    << "\tFEC " << fecVersionMajorId << "." << fecVersionMinorId << std::endl 
		    << "\tFED " << fedVersionMajorId << "." << fedVersionMinorId << std::endl 
		    << "\tFED " << connectionVersionMajorId << "." << connectionVersionMinorId << std::endl ;

	  // --------------------------------------------------------------- FEC
	  // check if the partition is existing
	  if (partitionExisting[partitionName.c_str()] == true) {

	    deviceVector vDevices ;
	    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    DbInput->getFecDeviceDescriptions(partitionName,vDevices,fecVersionMajorId,fecVersionMinorId,true) ;
	    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    std::cout << "Found " << std::dec << vDevices.size() << " devices in partition " << partitionName  
		      << " in " << (endMillis-startMillis) << " ms" << std::endl ;

	    if (vDevices.size() != 0) {
	      unsigned int major, minor ;
	      major = 0, minor = 0 ;

	      std::cout << "--- The partition " << partitionName << " is already existing in the database, adding a version" << std::endl ;
	      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	      DbOutput->setFecDeviceDescriptions ( vDevices, partitionName, &major, &minor, (fecVersionMinorId == 0), false ) ;
	      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Upload done in database " << confdbOutput << " for the partition " 
			<< partitionName << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	      std::cout << "FEC devices version: " << std::dec << major << "." << minor << std::endl ;
	    }
	  }
	  // Create a partition
	  else {
	    std::cout << "Retreive the devices from partition " << partitionName 
		      << " for version " << fecVersionMajorId << "." << fecVersionMinorId << std::endl ;

	    deviceVector vDevices ;
	    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    DbInput->getFecDeviceDescriptions(partitionName,vDevices,fecVersionMajorId,fecVersionMinorId,true) ;
	    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    std::cout << "Found " << std::dec << vDevices.size() << " devices in partition " << partitionName  
		      << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    piaResetVector vPiaReset ;
	    DbInput->getPiaResetDescriptions(partitionName,vPiaReset);
	    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

	    if (vDevices.size() != 0) {
	      unsigned int major = 0, minor = 0 ;

	      std::cout << "--- Create the partition " << partitionName << " in the database" << std::endl ;
	      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	      DbOutput->createPartition (vDevices, vPiaReset, &major, &minor, partitionName, true) ;
	      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Upload done in database " << confdbOutput << " for the partition " 
			<< partitionName << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	      std::cout << "FEC devices version: " << std::dec << major << "." << minor << std::endl ;
	    }
	  }

	  // ----------------------------------------------------------- FED
	  // Retrieve the FED from the input
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::vector<Fed9U::Fed9UDescription*> *fedVector = DbInput->getFed9UDescriptions(partitionName,(unsigned short)fedVersionMajorId,(unsigned short)fedVersionMinorId) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  if (fedVector != NULL)
	    std::cout << "Found " << std::dec << fedVector->size() << " FEDs in database in " << (endMillis-startMillis) << " ms" << std::endl ;
	  else
	    std::cout << "Found " << std::dec << 0 << " FEDs in database in " << (endMillis-startMillis) << " ms" << std::endl ;

	  if ((fedVector != NULL) && fedVector->size()) {
	      unsigned short major, minor ;
	      major = 0, minor = 0 ;
	      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

	      if (partitionExisting[partitionName.c_str()] == false)
		DbOutput->setFed9UDescriptions(*fedVector,partitionName,&major,&minor,1) ;
	      else if (fedVersionMinorId == 0) 
		DbOutput->setFed9UDescriptions(*fedVector,partitionName,&major,&minor,2) ;
	      else
		DbOutput->setFed9UDescriptions(*fedVector,partitionName,&major,&minor,0) ;

	      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Upload done in database " << confdbOutput << " for the partition " << partitionName << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	      std::cout << "FED version: " << std::dec << major << "." << minor << std::endl ;
	  }

	  // ----------------------------------------------------------- Connections
	  // Retreive the connection for the input
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  ConnectionVector connections ; DbInput->getConnectionDescriptions (partitionName,connections,
									     connectionVersionMajorId,
									     connectionVersionMinorId,true,true) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Found " << std::dec << connections.size() << " connections in " 
		    << (endMillis-startMillis) << " ms for version " << connectionVersionMajorId << "." << connectionVersionMinorId << std::endl ;

	  if (connections.size()) {
	    unsigned int versionMajor = 0, versionMinor = 0 ;
	    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    DbOutput->setConnectionDescriptions(connections,partitionName,&versionMajor,&versionMinor,false) ; // in a minor version
	    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    std::cout << "Upload of the connection done in database " << confdbOutput << " for the partition " << partitionName << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	    std::cout << "Connection version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	  }
	    
#ifdef OLDVERSION 
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbInput->createInputDBAccess();
	  DbInput->setInputDBVersion(partitionName);
	  DbInput->getTrackerParser()->buildModuleList();
	  DbInput->getTrackerParser()->setPedestalFile("/tmp/lastpeddies.root");
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Found " << std::dec << DbInput->getTrackerParser()->getNumberOfFedChannel() << " connections in " << (endMillis-startMillis) << " ms" << std::endl ;
	  
	  if (DbInput->getTrackerParser()->getNumberOfFedChannel()) {
	    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    DbOutput->createInputDBAccess();
	    DbOutput->setInputDBVersion(partitionName);
	    for (int ich=0;ich<DbInput->getTrackerParser()->getNumberOfFedChannel();ich++) {
	      
	      FedChannelConnectionDescription* fedch= DbInput->getTrackerParser()->getFedChannelConnection(ich);
	      DbOutput->addFedChannelConnection(fedch);
	    }
	    std::cout << DbOutput->getTrackerParser()->getNumberOfFedChannel() << " will be uploaded in the output" << std::endl ;
	    DbOutput->upload();
	    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    std::cout << "Upload of the connection done in database " << confdbOutput << " for the partition " << partitionName << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  }
#endif
	    
	  // --------------------------------------------------------- Run
	  // Set the run number
	  DbOutput->setRun(partitionName,run->getRunNumber(),TkRun::getModeId(run->getMode()),run->getLocal(),run->getComments()) ;
	}
      }
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to connect the DB " << confdbInput << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Unable to connect the DB " << confdbInput << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (Fed9U::Fed9UDeviceFactoryException &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (ICUtils::ICException &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (exception &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (...) { //Catch all types of exceptions
      std::cerr << "ERROR: unknown exception catched" << std::endl ;
      return ;
    }

    // --------------------------------------------- Delete of the device factory
    std::cout << "Deleting the DB access" << std::endl ;
    delete DbInput ;
    std::cout << "Delete of the input done" << std::endl ;
    delete DbOutput ;
    std::cout << "Delete of the output done" << std::endl ;
  }
  else {
    std::cout << "No database given" << std::endl ;
  }
}

/** upload file into database or file without hardware access
 * command: -uploadFecPartition
 */
void uploadFecPartition ( std::string confdbInput, std::string confdbOutput, std::string partitionName, std::string partitionNameDest ) {

  FecFactory *fecInput = NULL, *fecOutput = NULL ;
  deviceVector vDevices ;
  piaResetVector vPiaFecReset ;

  // Input -------------------------------
  try {
    if (confdbInput == "file") {
     
      fecInput = new FecFactory ( ) ;
      fecInput->setInputFileName(partitionName) ;
      fecInput->getFecDeviceDescriptions(vDevices,true) ;
      fecInput->getPiaResetDescriptions(vPiaFecReset) ;
    }
    else {
      std::string loginInput = "", passwdInput = "", pathInput = "" ;
      getDbConfiguration (confdbInput,loginInput,passwdInput,pathInput) ;
  
      if (loginInput.size() && passwdInput.size() && pathInput.size() ) {
	fecInput = new FecFactory (loginInput,passwdInput,pathInput,false) ;
	fecInput->getFecDeviceDescriptions(partitionName,vDevices,0,0,true) ;
	fecInput->getPiaResetDescriptions(partitionName,vPiaFecReset,true) ;
	std::cout << "Creating an access to database " << confdbInput << std::endl ;
      }
      else {
	std::cerr << "No database corresponds to " << confdbInput << std::endl ;
	return ;
      }
    }
      

    // Output -----------------------------
    if (confdbOutput == "file") {
     
      fecOutput = new FecFactory ( ) ;
      fecOutput->setOutputFileName(partitionNameDest) ;
    }
    else {
      std::string loginInput = "", passwdInput = "", pathInput = "" ;
      getDbConfiguration (confdbOutput,loginInput,passwdInput,pathInput) ;
  
      if (loginInput.size() && passwdInput.size() && pathInput.size() ) {
	std::cout << "Creating an access to database " << confdbInput << std::endl ;
	fecOutput = new FecFactory (loginInput,passwdInput,pathInput,false) ;
      }
      else {
	std::cerr << "No database corresponds to " << confdbOutput << std::endl ;
	if (fecInput != NULL) delete fecInput ;
	return ;
      }
    }

    std::cout << "Found " << vDevices.size() << " devices in the input " << std::endl ;
    std::cout << "Found " << vPiaFecReset.size() << " PIA reset in the input" << std::endl ;

    // Upload it in the output
    if (fecOutput->getDbUsed()) {
      std::cout << "Upload in database " << confdbOutput << " for the partition " << partitionNameDest << std::endl ;
      unsigned int major = 0, minor = 0 ;
      fecOutput->createPartition (vDevices, vPiaFecReset, &major, &minor, partitionNameDest, true) ;

      std::cout << "Upload in database done, partition " << partitionNameDest << " version " << major << "." << minor << " created" << std::endl ;
    }
    else {
      fecOutput->setFecDevicePiaDescriptions ( vDevices, vPiaFecReset ) ;
      std::cout << "Upload in file " << partitionNameDest << " done" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to connect the DB (FecExceptionHandler): " << confdbInput << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Unable to connect the DB (SQLException): " << confdbInput << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
    std::cerr << "Error during the FED download or upload" << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
  catch (ICUtils::ICException &e) {
    std::cerr << "Error during the FED download or upload" << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
  catch (exception &e) {
    std::cerr << "Error during the FED download or upload" << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
  catch (...) { //Catch all types of exceptions
    std::cerr << "ERROR: unknown exception catched" << std::endl ;
  }

  //std::cout << "Delete the input" << std::endl ;
  if (fecInput != NULL) delete fecInput ;
  //std::cout << "Delete the output" << std::endl ;
  if (fecOutput != NULL) delete fecOutput ;
}

/** upload file into database or file without hardware access
 * command: -uploadPartition
 */
void uploadPartition ( std::string confdbInput, std::string confdbOutput, std::string partitionName, std::string partitionNameDest ) {

  std::string loginInput = "", passwdInput = "", pathInput = "" ;
  getDbConfiguration (confdbInput,loginInput,passwdInput,pathInput) ;
  
  if (loginInput.size() && passwdInput.size() && pathInput.size() ) {

    DeviceFactory *DbInput = NULL, *DbOutput = NULL ;
    
    try {
      // Create the access
      DbInput = new DeviceFactory (loginInput,passwdInput,pathInput,false) ;

      std::cout << "Creating an access to database " << confdbInput << std::endl ;

      if (confdbOutput == "file") {
	DbOutput = new DeviceFactory() ;
	DbOutput->setUsingFile(true) ;
      }
      else {
	std::string loginOutput = "", passwdOutput = "", pathOutput = "" ;
	getDbConfiguration (confdbOutput,loginOutput,passwdOutput,pathOutput) ;
	if (loginOutput.size() && passwdOutput.size() && pathOutput.size() ) {
	  if (confdbInput != confdbOutput) {
	    DbOutput = new DeviceFactory (loginOutput,passwdOutput,pathOutput,true) ;
	  }
	  else {
	    if (partitionNameDest == partitionName) {
	      std::cerr << "Cannot copy the partition on itself" << std::endl ;
	      delete DbInput ;
	      return ;
	    }
	    else DbOutput = new DeviceFactory (loginOutput,passwdOutput,pathOutput,false) ;
	  }

	  std::cout << "Creating an access to database " << confdbOutput << std::endl ;
	}
      }

      unsigned long startMillis, endMillis ;

      // ----------------------------------------------- FEC
      std::cout << "Accessing FEC devices" << std::endl ;
	
      // Download the FEC and upload it to a file
      deviceVector vDevices ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getFecDeviceDescriptions(partitionName,vDevices,0,0,true) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Found " << std::dec << vDevices.size() << " FEC devices (with the DCUs) in " << (endMillis-startMillis) << " ms" << std::endl ;
      
      piaResetVector vPiaReset ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getPiaResetDescriptions(partitionName,vPiaReset) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Found " << std::dec << vPiaReset.size() << " PIA resets in " << (endMillis-startMillis) << " ms" << std::endl ;

      //for (piaResetVector::iterator it = vPiaReset.begin() ; it != vPiaReset.end() ; it ++) {
      //std::cout << (*it)->getCrateSlot() << std::endl ;											       
      //}
	
      // Upload it in a file or database
      if (vPiaReset.size()) {
	if (confdbOutput == "file") {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setOutputFileName("/tmp/partitionNameFECPIA.xml") ;
	  DbOutput->setPiaResetDescriptions (vPiaReset) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the PIA reset in /tmp/partitionNameFECPIA.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
      }

      if (vDevices.size()) {
	if (confdbOutput == "file") {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setOutputFileName("/tmp/partitionNameFECDEVICES.xml") ;
	  DbOutput->setFecDeviceDescriptions (vDevices) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the FEC devices in /tmp/partitionNameFECDEVICES.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned int major, minor ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->createPartition (vDevices, vPiaReset, &major, &minor, partitionNameDest, true) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "FEC devices version: " << std::dec << major << "." << minor << std::endl ;
	}
      }

      // ----------------------------------------------- FED

      std::cout << "Accessing FED devices" << std::endl ;

      // Retrieve the FED from the input
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::vector<Fed9U::Fed9UDescription*> *fedVector = DbInput->getFed9UDescriptions(partitionName) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Found " << std::dec << fedVector->size() << " FEDs in database in " << (endMillis-startMillis) << " ms" << std::endl ;
      
      // Upload it in a file
      if (fedVector->size()) {
	if (confdbOutput == "file") {
	  unsigned short version ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setOutputFileName("/tmp/partitionNameFED.xml") ;
	  DbOutput->setFed9UDescriptions(*fedVector,std::string("NONE"),&version,&version,1) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of FED done in file /tmp/partitionNameFED.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned short versionMajor, versionMinor ;
	  DbOutput->setUsingStrips(true);
	  DbOutput->setDebugOutput(true);
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setFed9UDescriptions(*fedVector,partitionNameDest,&versionMajor,&versionMinor,1);
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "FED version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	}
      } 
      
      // ---------------------------------------------- Connection
      std::cout << "Accessing connection devices" << std::endl ;
	
      // Retreive the connection for the input
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      ConnectionVector connections ; DbInput->getConnectionDescriptions (partitionName,connections,0,0,true,true) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Found " << std::dec << connections.size() << " connections in " << (endMillis-startMillis) << " ms" << std::endl ;
	
      if (connections.size()) {
	// Upload it to the database or file
	if (confdbOutput == "file") {
	  DbOutput->setOutputFileName("/tmp/partitionNameConnection.xml");
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setConnectionDescriptions(connections);
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Connection uploaded in file /tmp/partitionNameConnection.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned int versionMajor = 0, versionMinor = 0 ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setConnectionDescriptions(connections,partitionNameDest,&versionMajor,&versionMinor,true) ; // in a major version
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the connection done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "Connection version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	}
      }

      // -----------------------------------------------------------------------
      // Two steps the PSU name then the PSU/DCU map
      // PSU name
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getPsuNamePartition(partitionName);
      tkDcuPsuMapVector psuNames = DbInput->getAllTkDcuPsuMaps() ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << psuNames.size() << " PSU names in partition " << partitionName << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      // Upload the PSU name to the DB
      if (psuNames.size()) {
	if (confdbOutput == "file") {
	  DbOutput->setOutputFileName("/tmp/partitionNamePsuNames.xml");
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setTkPsuNames(psuNames) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "PSU names uploaded in file /tmp/partitionNamePsuNames in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned int versionMajor = 0, versionMinor = 0 ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setTkPsuNames( psuNames, partitionNameDest,&versionMajor,&versionMinor ) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the PSU names done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "PSU names version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	}
      }

      // PSU / DCU map
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getDcuPsuMapPartition(partitionName);
      tkDcuPsuMapVector dcuPsu = DbInput->getAllTkDcuPsuMaps() ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << dcuPsu.size() << " PSU/DCU map in partition " << partitionName << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      // Upload the PSU/DCU map to the DB
      if (dcuPsu.size()) {
	if (confdbOutput == "file") {
	  DbOutput->setOutputFileName("/tmp/partitionNameDcuPsuMap.xml");
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setTkDcuPsuMap(dcuPsu) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "PSU DCU names uploaded in file /tmp/partitionNameDcuPsuMap.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned int versionMajor = 0, versionMinor = 0 ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setTkDcuPsuMap( dcuPsu, partitionNameDest,&versionMajor,&versionMinor ) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the DCU/PSU map done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "PSU/DCU map version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	}
      }

      // -----------------------------------------------------------------------
      // CCUs
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      tkringVector tkRingDescriptions ;
      if (confdbInput == "file")
	tkRingDescriptions = DbInput->getFileRingDescription ( partitionName ) ;
      else 
	tkRingDescriptions = DbInput->getDbRingDescription ( partitionName ) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << tkRingDescriptions.size() << " rings in partition " << partitionName << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      if (tkRingDescriptions.size() > 0) {

	if (confdbOutput == "file") {
	  for (tkringVector::iterator it = tkRingDescriptions.begin() ; it != tkRingDescriptions.end() ; it ++) { // one file per ring
	    std::stringstream fileName ; fileName << "/tmp/" ;
	    fileName << "FEC_" << (*it)->getFecHardwareId() << "_RING_" << (*it)->getRingSlot() << "_.xml" ;
	    DbOutput->setFileRingDescription(fileName.str(), *(*it)) ;
	    std::cout << "Upload done for FEC " << (*it)->getFecHardwareId() << "." << (*it)->getRingSlot() << " (" << (*it)->getNumberOfCcus() << " CCUs)" << " in file " << fileName.str() << std::endl ;
	  }
	}
	else {
	  for (tkringVector::iterator it = tkRingDescriptions.begin() ; it != tkRingDescriptions.end() ; it ++) { // ring after ring
	    DbOutput->setDbRingDescription(partitionNameDest, *(*it)) ;
	    std::cout << "Upload done for FEC " << (*it)->getFecHardwareId() << "." << (*it)->getRingSlot() << " (" << (*it)->getNumberOfCcus() << " CCUs)" << " in partition " << partitionNameDest << std::endl ;
	  }
	}
      }
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to connect the DB (FecExceptionHandler): " << confdbInput << std::endl ;
      std::cerr << e.what() << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Unable to connect the DB (SQLException): " << confdbInput << std::endl ;
      std::cerr << e.what() << std::endl ;
    }
    catch (Fed9U::Fed9UDeviceFactoryException &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
    }
    catch (ICUtils::ICException &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
    }
    catch (exception &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
    }
    catch (...) { //Catch all types of exceptions
      std::cerr << "ERROR: unknown exception catched" << std::endl ;
    }

    // --------------------------------------------- Delete of the device factory
    std::cout << "Deleting the DB access" << std::endl ;
    delete DbInput ;
    std::cout << "Delete of the input done" << std::endl ;
    delete DbOutput ;
    std::cout << "Delete of the output done" << std::endl ;
  }
  else {
    std::cout << "No database given" << std::endl ;
  }
}

/** upload file into database or file without hardware access
 * command: -uploadPartition
 */
void uploadPartition1 ( std::string confdbInput, std::string confdbOutput, std::string partitionName, std::string partitionNameDest ) {

  std::string loginInput = "", passwdInput = "", pathInput = "" ;
  getDbConfiguration (confdbInput,loginInput,passwdInput,pathInput) ;
  
  if (loginInput.size() && passwdInput.size() && pathInput.size() ) {

    DeviceFactory *DbInput = NULL, *DbOutput = NULL ;
    
    try {

      // Create the access
      DbInput = new DeviceFactory (loginInput,passwdInput,pathInput,false) ;
      if (confdbOutput == "file") {
	DbOutput = new DeviceFactory() ;
	DbOutput->setUsingFile(true) ;
      }
      else {
	std::string loginOutput = "", passwdOutput = "", pathOutput = "" ;
	getDbConfiguration (confdbOutput,loginOutput,passwdOutput,pathOutput) ;
	if (loginOutput.size() && passwdOutput.size() && pathOutput.size() ) {
	  if (confdbInput != confdbOutput)
	    DbOutput = new DeviceFactory (loginOutput,passwdOutput,pathOutput,true) ;
	  else {
	    if (partitionNameDest == partitionName) {
	      std::cerr << "Cannot copy the partition on itself" << std::endl ;
	      delete DbInput ;
	      return ;
	    }
	    else DbOutput = new DeviceFactory (loginOutput,passwdOutput,pathOutput,false) ;
	  }

	  std::cout << "Creating an access to database " << confdbOutput << std::endl ;
	}
      }

      unsigned long startMillis, endMillis ;

//       // ----------------------------------------------- FEC

//       // Download the FEC and upload it to a file
//       deviceVector vDevices ;
//       startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//       DbInput->getFecDeviceDescriptions(partitionName,vDevices,0,0,true) ;
//       endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//       std::cout << "Found " << std::dec << vDevices.size() << " FEC devices (with the DCUs) in " << (endMillis-startMillis) << " ms" << std::endl ;
      
//       piaResetVector vPiaReset ;
//       startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//       DbInput->getPiaResetDescriptions(partitionName,vPiaReset) ;
//       endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//       std::cout << "Found " << std::dec << vPiaReset.size() << " PIA resets in " << (endMillis-startMillis) << " ms" << std::endl ;

//       // Upload it in a file or database
//       if (vPiaReset.size()) {
// 	if (confdbOutput == "file") {
// 	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  DbOutput->setOutputFileName("/tmp/partitionNameFECPIA.xml") ;
// 	  DbOutput->setPiaResetDescriptions (vPiaReset) ;
// 	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  std::cout << "Upload of the PIA reset in /tmp/partitionNameFECPIA.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
// 	}
//       }

//       if (vDevices.size()) {
// 	if (confdbOutput == "file") {
// 	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  DbOutput->setOutputFileName("/tmp/partitionNameFECDEVICES.xml") ;
// 	  DbOutput->setFecDeviceDescriptions (vDevices) ;
// 	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  std::cout << "Upload of the FEC devices in /tmp/partitionNameFECDEVICES.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
// 	}
// 	else {
// 	  unsigned int major, minor ;
// 	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  DbOutput->createPartition (vDevices, vPiaReset, &major, &minor, partitionNameDest, true) ;
// 	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  std::cout << "Upload done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
// 	  std::cout << "FEC devices version: " << std::dec << major << "." << minor << std::endl ;
// 	}
//       }

      // ----------------------------------------------- FED

      // Retrieve the FED from the input
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::vector<Fed9U::Fed9UDescription*> *fedVector = DbInput->getFed9UDescriptions(partitionName) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Found " << std::dec << fedVector->size() << " FEDs in database in " << (endMillis-startMillis) << " ms" << std::endl ;

      // Upload it in a file
      if (fedVector->size()) {
	if (confdbOutput == "file") {
	  unsigned short version ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setOutputFileName("/tmp/partitionNameFED.xml") ;
	  DbOutput->setFed9UDescriptions(*fedVector,std::string("NONE"),&version,&version,1) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of FED done in file /tmp/partitionNameFED.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  char c = 'y' ;
	  do {
	    try {
	      unsigned short versionMajor, versionMinor ;
	      DbOutput->setUsingStrips(true);
	      DbOutput->setDebugOutput(true);
	      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	      DbOutput->setFed9UDescriptions(*fedVector,partitionNameDest,&versionMajor,&versionMinor,1);
	      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Upload done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	      std::cout << "FED version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "FecExceptionHandler: " << e.what() << std::endl ;
	    }
	    catch (oracle::occi::SQLException &e) {
	      std::cerr << "SQL exception: " << e.what() << std::endl ;
	    }
	    catch (Fed9U::Fed9UDeviceFactoryException &e) {
	      std::cerr << "Fed9UDeviceFactoryException: " << e.what() << std::endl ;
	    }
	    catch (ICUtils::ICException &e) {
	      std::cerr << "ICException: " << e.what() << std::endl ;
	    }
	    catch (exception &e) {
	      std::cerr << "Exception: " << e.what() << std::endl ;
	    }
	    catch (...) { //Catch all types of exceptions
	      std::cerr << "ERROR: unknown exception catched" << std::endl ;
	    }

	    std::cout << "Do yo want to continue ? (y/n)" ;
	    std::cin >> c ;
	  }
	  while (c != 'n') ;
	}
      } 
      
//       // ---------------------------------------------- Connection

//       // Retreive the connection for the input
//       startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//       ConnectionVector connections ; DbInput->getConnectionDescriptions (partitionName,connections,0,0,true,true) ;
//       endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//       std::cout << "Found " << std::dec << connections.size() << " connections in " << (endMillis-startMillis) << " ms" << std::endl ;

//       if (connections.size()) {
// 	// Upload it to the database or file
// 	if (confdbOutput == "file") {
// 	  DbOutput->setOutputFileName("/tmp/partitionNameConnection.xml");
// 	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  DbOutput->setConnectionDescriptions(connections);
// 	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  std::cout << "Connection uploaded in file /tmp/partitionNameConnection.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
// 	}
// 	else {
// 	  unsigned int versionMajor = 0, versionMinor = 0 ;
// 	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  DbOutput->setConnectionDescriptions(connections,partitionNameDest,&versionMajor,&versionMinor,false) ; // in a minor version
// 	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	  std::cout << "Upload of the connection done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
// 	  std::cout << "Connection version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
// 	}

// 	ConnectionFactory::deleteVector(connections) ;
//       }

#ifdef OLDVERSION      
      // Retreive the connection for the input
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->createInputDBAccess();
      DbInput->setInputDBVersion(partitionName);
      DbInput->getTrackerParser()->buildModuleList();
      DbInput->getTrackerParser()->setPedestalFile("/tmp/lastpeddies.root");
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Found " << std::dec << DbInput->getTrackerParser()->getNumberOfFedChannel() << " connections in " << (endMillis-startMillis) << " ms" << std::endl ;
      
      if (DbInput->getTrackerParser()->getNumberOfFedChannel()) {
	if (confdbOutput == "file") {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->createInputFileAccess();
	  DbOutput->setOutputFileName("/tmp/partitionNameConnection.xml");
	  for (int ich=0;ich<DbInput->getTrackerParser()->getNumberOfFedChannel();ich++) {
	    
	    FedChannelConnectionDescription* fedch = DbInput->getTrackerParser()->getFedChannelConnection(ich);
	    DbOutput->addFedChannelConnection(fedch);
	  }      
	  
	  DbOutput->write();
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << DbOutput->getTrackerParser()->getNumberOfFedChannel() << " will be uploaded in the output" << std::endl ;
	  std::cout << "Connection uploaded in file /tmp/partitionNameConnection.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->createInputDBAccess();
	  DbOutput->setInputDBVersion(partitionNameDest);
	  for (int ich=0;ich<DbInput->getTrackerParser()->getNumberOfFedChannel();ich++) {
	    
	    FedChannelConnectionDescription* fedch= DbInput->getTrackerParser()->getFedChannelConnection(ich);
	    DbOutput->addFedChannelConnection(fedch);
	  
	  }
	  std::cout << DbOutput->getTrackerParser()->getNumberOfFedChannel() << " will be uploaded in the output" << std::endl ;
	  DbOutput->upload();
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	}
	std::cout << "Upload of the connection done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
#endif
      
      // --------------------------------------------- Delete of the device factory
      std::cout << "Deleting the DB access" << std::endl ;
      delete DbInput ;
      std::cout << "Delete of the input done" << std::endl ;
      delete DbOutput ;
      std::cout << "Delete of the output done" << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to connect the DB " << confdbInput << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Unable to connect the DB " << confdbInput << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (Fed9U::Fed9UDeviceFactoryException &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (ICUtils::ICException &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (exception &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (...) { //Catch all types of exceptions
      std::cerr << "ERROR: unknown exception catched" << std::endl ;
      return ;
    }
  }
  else {
    std::cout << "No database given" << std::endl ;
  }
}

/** upload file into database or file without hardware access
 * command: -mergePartition
 */
void mergePartition ( std::string confdbInput, std::string confdbOutput, std::string partitionName1, std::string partitionName2, std::string partitionNameDest ) {

  std::string loginInput = "", passwdInput = "", pathInput = "" ;
  getDbConfiguration (confdbInput,loginInput,passwdInput,pathInput) ;
  
  if (loginInput.size() && passwdInput.size() && pathInput.size() ) {

    DeviceFactory *DbInput = NULL, *DbOutput = NULL ;
    
    try {

      // Create the access
      DbInput = new DeviceFactory (loginInput,passwdInput,pathInput,false) ;
      if (confdbOutput == "file") {
	DbOutput = new DeviceFactory() ;
	DbOutput->setUsingFile(true) ;
      }
      else {
	std::string loginOutput = "", passwdOutput = "", pathOutput = "" ;
	getDbConfiguration (confdbOutput,loginOutput,passwdOutput,pathOutput) ;
	if (loginOutput.size() && passwdOutput.size() && pathOutput.size() ) {
	  if (confdbInput != confdbOutput)
	    DbOutput = new DeviceFactory (loginOutput,passwdOutput,pathOutput,true) ;
	  else {
	    if (partitionName1 == partitionName2) {
	      std::cerr << "Cannot merge the same partition" << std::endl ;
	      delete DbInput ;
	      return ;
	    }
	    else DbOutput = new DeviceFactory (loginOutput,passwdOutput,pathOutput,false) ;
	  }
	}
      }

      // ----------------------------------------------- FEC

      // Download the FEC and upload it to a file
      deviceVector vDevices ;
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getFecDeviceDescriptions(partitionName1,vDevices,0,0,true) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << vDevices.size() << " FEC devices (with the DCUs) in partition " << partitionName1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      unsigned int value = vDevices.size() ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getFecDeviceDescriptions(partitionName2,vDevices,0,0,true) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << (vDevices.size()-value) << " FEC devices (with the DCUs) in partition " << partitionName2 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      std::cout << "----- Found " << std::dec << vDevices.size() << " FEC devices (with the DCUs) for the two partitions" << std::endl ;
      
      piaResetVector vPiaReset ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getPiaResetDescriptions(partitionName1,vPiaReset) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << vPiaReset.size() << " PIA resets in partition " << partitionName1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      value = vPiaReset.size() ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getPiaResetDescriptions(partitionName2,vPiaReset) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << (vPiaReset.size()-value) << " PIA resets in partition " << partitionName2 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      std::cout << "----- Found " << std::dec << vPiaReset.size() << " PIA resets for the two partitions" << std::endl ;

      // Upload it in a file or database
      if (vPiaReset.size()) {
	if (confdbOutput == "file") {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setOutputFileName("/tmp/partitionNameFECPIA.xml") ;
	  DbOutput->setPiaResetDescriptions (vPiaReset) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the PIA reset in /tmp/partitionNameFECPIA.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
      }

      if (vDevices.size()) {
	if (confdbOutput == "file") {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setOutputFileName("/tmp/partitionNameFECDEVICES.xml") ;
	  DbOutput->setFecDeviceDescriptions (vDevices) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the FEC devices in /tmp/partitionNameFECDEVICES.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned int major, minor ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->createPartition (vDevices, vPiaReset, &major, &minor, partitionNameDest, true) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "FEC devices version: " << std::dec << major << "." << minor << std::endl ;
	  std::cout << "PIA reset version: " << std::dec << major << "." << minor << std::endl ;
	}
      }

      // ----------------------------------------------- FED

      // Retrieve the FED from the input
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::vector<Fed9U::Fed9UDescription*> *fedVector1 = DbInput->getFed9UDescriptions(partitionName1) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << fedVector1->size() << " FEDs in database in partition " << partitionName1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      // clone the vectors
      std::vector<Fed9U::Fed9UDescription*> fedVector ;
      for (std::vector<Fed9U::Fed9UDescription*>::iterator it = fedVector1->begin() ; it != fedVector1->end() ; it ++) {
	fedVector.push_back(new Fed9U::Fed9UDescription(*(*it))) ;
      }
      
      // Retreive the vectors for the second partition
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::vector<Fed9U::Fed9UDescription*> *fedVector2 = DbInput->getFed9UDescriptions(partitionName2) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << fedVector2->size() << " FEDs in database in partition " << partitionName2 << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      // clone the vectors
      for (std::vector<Fed9U::Fed9UDescription*>::iterator it = fedVector2->begin() ; it != fedVector2->end() ; it ++) {
	fedVector.push_back(new Fed9U::Fed9UDescription(*(*it))) ;
      }

      // Upload it in a file
      if (fedVector.size()) {
	if (confdbOutput == "file") {
	  unsigned short version ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setOutputFileName("/tmp/partitionNameFED.xml") ;
	  DbOutput->setFed9UDescriptions(fedVector,std::string("NONE"),&version,&version,1) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of FED done in file /tmp/partitionNameFED1.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned short versionMajor, versionMinor ;
	  DbOutput->setUsingStrips(true);
	  //DbOutput->setDebugOutput(true);
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  // 0, 1, 2: 
	  //      - 0 minor version
	  //      - 1 is creation (major)
	  //      - 2 append to the same partition
	  DbOutput->setFed9UDescriptions(fedVector,partitionNameDest,&versionMajor,&versionMinor,1);
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload for " << fedVector.size() << " FEDs done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "FED version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	}
      } 

      // delete the vector of cloned FEDs
      // clone the vectors
      for (std::vector<Fed9U::Fed9UDescription*>::iterator it = fedVector.begin() ; it != fedVector.end() ; it ++) {
	delete (*it) ;
      }
      fedVector.clear() ;

      // ---------------------------------------------- Connection
      // Retreive the connection for the input
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      ConnectionVector connections ; DbInput->getConnectionDescriptions (partitionName1,connections,0,0,true,true) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << connections.size() << " connections in partition " << partitionName1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      unsigned int connectionSize = connections.size() ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getConnectionDescriptions (partitionName2,connections,0,0,true,true) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << (connections.size()-connectionSize) << " connections in partition " << partitionName2 << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      // Upload the connections to the DB
      if (connections.size()) {
	if (confdbOutput == "file") {
	  DbOutput->setOutputFileName("/tmp/partitionNameConnection1.xml");
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setConnectionDescriptions(connections) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Connection uploaded in file /tmp/partitionNameConnection.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned int versionMajor = 0, versionMinor = 0 ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setConnectionDescriptions(connections,partitionNameDest,&versionMajor,&versionMinor,false) ; // in a minor version
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the connection done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "Connection version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	}
      }
      
#ifdef OLDVERSION
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->createInputDBAccess();
      DbInput->setInputDBVersion(partitionName1);
      DbInput->getTrackerParser()->buildModuleList();
      DbInput->getTrackerParser()->setPedestalFile("/tmp/lastpeddies.root");
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << DbInput->getTrackerParser()->getNumberOfFedChannel() << " connections in partition " << partitionName1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      value = DbInput->getTrackerParser()->getNumberOfFedChannel() ;

      if (DbInput->getTrackerParser()->getNumberOfFedChannel()) {
	if (confdbOutput == "file") {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->createInputFileAccess();
	  DbOutput->setOutputFileName("/tmp/partitionNameConnection1.xml");
	  for (int ich=0;ich<DbInput->getTrackerParser()->getNumberOfFedChannel();ich++) {
	    
	    FedChannelConnectionDescription* fedch = DbInput->getTrackerParser()->getFedChannelConnection(ich);
	    DbOutput->addFedChannelConnection(fedch);
	  }      
	  
	  DbOutput->write();
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << DbOutput->getTrackerParser()->getNumberOfFedChannel() << " will be uploaded in the output" << std::endl ;
	  std::cout << "Connection uploaded in file /tmp/partitionNameConnection.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->createInputDBAccess();
	  DbOutput->setInputDBVersion(partitionNameDest);
	  for (int ich=0;ich<DbInput->getTrackerParser()->getNumberOfFedChannel();ich++) {
	    
	    FedChannelConnectionDescription* fedch= DbInput->getTrackerParser()->getFedChannelConnection(ich);
	    DbOutput->addFedChannelConnection(fedch);
	  
	  }
	  std::cout << DbOutput->getTrackerParser()->getNumberOfFedChannel() << " will be uploaded in the output" << std::endl ;
	  DbOutput->upload();
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	}
	std::cout << "Upload of the connection done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      }

      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->createInputDBAccess();
      DbInput->setInputDBVersion(partitionName2);
      DbInput->getTrackerParser()->buildModuleList();
      DbInput->getTrackerParser()->setPedestalFile("/tmp/lastpeddies.root");
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << DbInput->getTrackerParser()->getNumberOfFedChannel() << " connections in partition " << partitionName2 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      
      if (DbInput->getTrackerParser()->getNumberOfFedChannel()) {
	if (confdbOutput == "file") {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->createInputFileAccess();
	  DbOutput->setOutputFileName("/tmp/partitionNameConnection.xml");
	  for (int ich=0;ich<DbInput->getTrackerParser()->getNumberOfFedChannel();ich++) {
	    
	    FedChannelConnectionDescription* fedch = DbInput->getTrackerParser()->getFedChannelConnection(ich);
	    DbOutput->addFedChannelConnection(fedch);
	  }      
	  
	  DbOutput->write();
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << DbOutput->getTrackerParser()->getNumberOfFedChannel() << " will be uploaded in the output" << std::endl ;
	  std::cout << "Connection uploaded in file /tmp/partitionNameConnection.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->createInputDBAccess();
	  DbOutput->setInputDBVersion(partitionNameDest);
	  for (int ich=0;ich<DbInput->getTrackerParser()->getNumberOfFedChannel();ich++) {
	    
	    FedChannelConnectionDescription* fedch= DbInput->getTrackerParser()->getFedChannelConnection(ich);
	    DbOutput->addFedChannelConnection(fedch);
	  
	  }
	  std::cout << DbOutput->getTrackerParser()->getNumberOfFedChannel() << " will be uploaded in the database" << std::endl ;
	  DbOutput->upload();
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	}
	std::cout << "Upload of the connection done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
#endif

      // -----------------------------------------------------------------------
      // Two steps the PSU name then the PSU/DCU map
      // PSU name
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getPsuNamePartition(partitionName1);
      tkDcuPsuMapVector psuName1 = DbInput->getAllTkDcuPsuMaps() ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << psuName1.size() << " PSU names in partition " << partitionName1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      unsigned int psuDcuMapSize = psuName1.size() ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getPsuNamePartition(partitionName2) ;
      tkDcuPsuMapVector psuName2 = DbInput->getAllTkDcuPsuMaps() ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << (psuName2.size()-psuDcuMapSize) << " PSU names in partition " << partitionName2 << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      // Upload the connections to the DB
      if (psuName2.size()) {
	if (confdbOutput == "file") {
	  DbOutput->setOutputFileName("/tmp/partitionNamePsuNames.xml");
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setTkPsuNames(psuName2) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "PSU names uploaded in file /tmp/partitionNamePsuNames in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned int versionMajor = 0, versionMinor = 0 ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setTkPsuNames( psuName2, partitionNameDest,&versionMajor,&versionMinor ) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the PSU names done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "PSU names version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	}
      }

      // PSU / DCU map
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getDcuPsuMapPartition(partitionName1);
      tkDcuPsuMapVector dcuPsu1 = DbInput->getAllTkDcuPsuMaps() ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << dcuPsu1.size() << " PSU/DCU map in partition " << partitionName1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      psuDcuMapSize = dcuPsu1.size() ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      DbInput->getDcuPsuMapPartition(partitionName2) ;
      tkDcuPsuMapVector dcuPsu2 = DbInput->getAllTkDcuPsuMaps() ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "----- Found " << std::dec << (dcuPsu2.size()-psuDcuMapSize) << " PSU/DCU map in partition " << partitionName2 << " in " << (endMillis-startMillis) << " ms" << std::endl ;

      // Upload the connections to the DB
      if (dcuPsu2.size()) {
	if (confdbOutput == "file") {
	  DbOutput->setOutputFileName("/tmp/partitionNameDcuPsuMap.xml");
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setTkDcuPsuMap(dcuPsu2) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "PSU DCU names uploaded in file /tmp/partitionNameDcuPsuMap.xml in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  unsigned int versionMajor = 0, versionMinor = 0 ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  DbOutput->setTkDcuPsuMap( dcuPsu2, partitionNameDest,&versionMajor,&versionMinor ) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the DCU/PSU map done in database " << confdbOutput << " for the partition " << partitionNameDest << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  std::cout << "PSU/DCU map version: " << std::dec << versionMajor << "." << versionMinor << std::endl ;
	}
      }
      
      // --------------------------------------------- Delete of the device factory
      std::cout << "Deleting the DB access" << std::endl ;
      delete DbInput ;
      std::cout << "Delete of the input done" << std::endl ;
      if (DbInput != DbOutput) delete DbOutput ;
      std::cout << "Delete of the output done" << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to connect the DB " << confdbInput << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Unable to connect the DB " << confdbInput << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (Fed9U::Fed9UDeviceFactoryException &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (ICUtils::ICException &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (exception &e) {
      std::cerr << "Error during the FED download or upload" << std::endl ;
      std::cerr << e.what() << std::endl ;
      return ;
    }
    catch (...) { //Catch all types of exceptions
      std::cerr << "ERROR: unknown exception catched" << std::endl ;
      return ;
    }
  }
  else {
    std::cout << "No database given" << std::endl ;
  }
}


/** upload file into database or file without hardware access
 * command: -uploadInputOutput
 */
void uploadInputOutput ( std::string inputFile, std::string outputFile, std::string partitionName ) {

  //std::cout << "uploadInputOutput (" << inputFile << ", " << outputFile << ", " << partitionName << ")" << std::endl; 

  FecFactory *fecFactoryInput = NULL ;
  deviceVector vDeviceInput ;
  piaResetVector vPiaResetInput ;
  std::string login="nil", passwd="nil", path="nil" ;

  // read the file or databaseand get the devices
  try {

#ifdef DATABASE
    DbFecAccess::getDbConfiguration (login, passwd, path) ;
    if ( (inputFile == "database") && (login == "nil"))
      std::cerr << "The login, password, path is not set cannot upload in database" << std::endl ;
    if ( (inputFile == "database") && (login != "nil") && (partitionName != "nil") && (partitionName != "") ) {
      
      // Database connection
      std::cout << "Create a database access for " << login << "/" << passwd << "@" << path << " for the input" << std::endl ;
      fecFactoryInput = new FecFactory(login, passwd, path) ;

      fecFactoryInput->getFecDeviceDescriptions (partitionName,vDeviceInput,0,0,true) ;
      fecFactoryInput->getPiaResetDescriptions (partitionName,vPiaResetInput) ;
    }
    else
#endif
      {
	fecFactoryInput = new FecFactory ( ) ;
	fecFactoryInput->setInputFileName (inputFile) ;
	fecFactoryInput->getFecDeviceDescriptions ( vDeviceInput ) ;
	fecFactoryInput->getPiaResetDescriptions ( vPiaResetInput ) ;
      }
    
    // display it
    // FecFactory::display (vDeviceInput) ;
    
#ifdef DATABASE
    // Upload in database or file
    login="nil", passwd="nil", path="nil" ;
    DbFecAccess::getDbConfiguration (login, passwd, path) ;
    if ( (outputFile == "database") && ((login == "nil") || (partitionName == "nil") && (partitionName == "")) ) {
      std::cerr << "The login, password, path is not set cannot upload in database" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return ;
    }
    
    if ( (outputFile == "database") && (login != "nil") && (partitionName != "nil") && (partitionName != "") ) {
      
      // Database connection
      std::cout << "Create a database access for " << login << "/" << passwd << "@" << path << " for the upload" << std::endl ;
      FecFactory fecFactory (login, passwd, path) ;
      
      unsigned int dmajor = 0, dminor = 0 ;

      std::cout << "Create partition for " << vDeviceInput.size() << " devices and " << vPiaResetInput.size() << " PIA" << std::endl ;

      // Create partition
      if (!vPiaResetInput.empty() && !vDeviceInput.empty())
 	fecFactory.createPartition (vDeviceInput, vPiaResetInput, &dmajor, &dminor, partitionName, true) ;
      else if (!vDeviceInput.empty()) 
	fecFactory.createPartition (vDeviceInput, &dmajor, &dminor, partitionName) ;
      else if (!vPiaResetInput.empty())
	fecFactory.createPartition (vPiaResetInput, partitionName) ;

      if (dmajor && dminor) {
	std::cout << "-------------------------- Version ---------------------" << std::endl ;
	std::cout << "Upload the FEC device version " << std::dec << dmajor << "." << dminor << " in the database" << std::endl ;
	std::cout << "--------------------------------------------------------" << std::endl ;
      }
    }
    else
#endif
      {
	// Upload in output file
	FecFactory fecFactory ;
	fecFactory.setUsingFile() ;
	fecFactory.setOutputFileName (outputFile) ;

	if (! vPiaResetInput.empty())
	  fecFactory.setFecDevicePiaDescriptions (vDeviceInput, vPiaResetInput) ;
	else
	  fecFactory.setFecDeviceDescriptions (vDeviceInput);

	std::cout << "Upload done in file " << outputFile << std::endl ;
      }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "XML parsing for database error " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }
#ifdef DATABASE
  catch (oracle::occi::SQLException &e) {
    
    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "Unable to connect the database " << login << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }
#endif
 
  // Delete the FEC factory 
 if (fecFactoryInput != NULL) delete fecFactoryInput ;
 }

/** upload file into database or file without hardware access
 * command: -uploadConnection
 * select apvfed.DEVICEID,apvfed.ID,apvfed.CHANNELID from apvfed,mviewapvfec where apvfed.deviceid=mviewapvfec.deviceid and partitionname='work_crack_07';
 */
void uploadConnection ( std::string inputFile, std::string outputFile, std::string partitionName ) {

  DbInterface *dbInput = NULL ;
#ifdef OLVERSION
  TrackerParser* connectionParser = NULL ;
#endif

  // Database login
  std::string login = "", passwd = "", path = "" ;
  DbAccess::getDbConfiguration (login, passwd, path) ;

  try {
    // Download
    if ( login.size() && passwd.size() && path.size() ) {
      dbInput = new DbInterface (true,true,login,passwd,path) ;
      
      // Download the connections
      bool changed ;
      if (inputFile == "database") dbInput->downloadConnectionsFromDatabase(partitionName,changed) ;
      else dbInput->downloadConnectionsFromFile(inputFile,changed,true) ;
      ConnectionVector connections = dbInput->getConnections();
      if (connections.size() == 0) 
	std::cerr << "Error during the retreive of the connections" << std::endl ;
      else {

	std::cout << "Found " << std::dec << connections.size() << " connections in " << inputFile << std::endl ;
	if (outputFile == "database") {
	  unsigned int major,minor ;
	  dbInput->uploadConnections(partitionName,true,&major,&minor) ;
	  std::cout << "upload done in database for partition " << partitionName << " version: " << major << "." << minor << std::endl;
	}
	else {
	  dbInput->uploadConnections(outputFile) ;
	  std::cout << "Upload done in the file " << outputFile << std::endl ;
	}
      }
#ifdef OLDVERSION
      if (inputFile == "database") connectionParser = dbInput->downloadConnectionFromDatabase (partitionName) ;
      else connectionParser = dbInput->downloadConnectionFromFile(inputFile) ;

      if (connectionParser == NULL)
	std::cerr << "Error during the retreive of the connections" << std::endl ;
      else {
	
	std::cout << "Found " << std::dec << connectionParser->getNumberOfFedChannel() << " connections in " << inputFile << std::endl ;
	
	if (connectionParser->getNumberOfFedChannel()) {
	  if (outputFile == "database") {
	    if (true) {
	      dbInput->uploadConnectionIntoDatabase (partitionName) ;
	    std::cout << "Upload done in the database " << login << " for partition " << partitionName << std::endl ;
	    }
	    else std::cout << "Not yet implemented" << std::endl ;
	  }
	  else {
	    dbInput->uploadConnectionIntoFile(outputFile) ;
	    std::cout << "Upload done in the file " << outputFile << std::endl ;
	  }
	}
      }
#endif

      delete dbInput ;
    }
    else std::cerr << "No database account specified" << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to connect the DB " << login << std::endl ;
    std::cerr << e.what() << std::endl ;
    return ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Unable to connect the DB " << login << std::endl ;
    std::cerr << e.what() << std::endl ;
    return ;
  }
}

/** Help part
 */
void help ( std::string programName ) {

  std::cerr << "Error in usage: " << programName << " [OPTION] [COMMAND]" << std::endl ;
  std::cerr << std::endl ;

//   std::cerr << "  -input [database OR FILE] -output [database OR FILE] -partition [PARTITION NAME] -uploadInputOutput" << std::endl ;
//   std::cerr << "\t" << "Upload the input into the output" << std::endl ;
//   std::cerr << "\t" << "Input can be database or file" << std::endl ;
//   std::cerr << "\t" << "Output can be database or file" << std::endl ;
//   std::cerr << std::endl ;

  std::cerr << "  -input [CONFDB] -output [CONFDB OR file] -partition [PARTITION NAME SOURCE] -uploadPartition [PARTITION NAME DEST]" << std::endl ;
  std::cerr << "\t" << "Upload the CONFDB given as input into the CONFDB given as output" << std::endl ;
  std::cerr << std::endl ;

  std::cerr << "  -input [CONFDB] -output [CONFDB OR file] -partition [DESTINATION PARTITION] -mergePartition [PARTITION 1] [PARTITION 2]" << std::endl ;
  std::cerr << "\t" << "Upload the CONFDB given as input into the CONFDB given as output" << std::endl ;
  std::cerr << std::endl ;

//  std::cerr << " -input [database OR FILE] -output [database OR FILE] -partition [PARTITION NAME] -uploadConnection" << std::endl ;
//  std::cerr << "\t" << "Upload the CONFDB given as input into the CONFDB given as output" << std::endl ;
//  std::cerr << std::endl ;
}

/** main function
 */
int main ( int argc, char *argv[] ) {

  //std::string fecHardwareId = "0" ;

  // outputFile for the upload
  std::string outputFile ;
  std::string inputFile ;
  std::string partitionName, partitionNameDest, partitionName1 ;

  // Switch in the different commands
  int choix = 0 ;

  // ------------------------------------------------------------------------
  // Check the options
  if ( (argc <= 1) || !strncasecmp (argv[1], "-help", 5) || !strncasecmp (argv[1], "--help", 6)) {

    help( argv[0] ) ;
    exit (EXIT_FAILURE) ;
  }

  // ------------------------------------------------------------------------
  // retreive the options

  // Check the over options
  for (int i = 1 ; i < argc ; i ++) {

    if (strncasecmp (argv[i], "-input", 6) == 0) {
      if (i+1 < argc) {
	inputFile = std::string(argv[i+1]) ;
	i ++ ;
      }
      else {
	std::cerr << "Error in usage, bad values for the options, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }

    if (strncasecmp (argv[i], "-output", 7) == 0) {
      if (i+1 < argc) {
	outputFile = std::string(argv[i+1]) ;
	i ++ ;
      }
      else {
	std::cerr << "Error in usage, bad values for the options, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }

    if (strncasecmp (argv[i], "-partition", 7) == 0) {
      if (i+1 < argc) {
	partitionName = std::string(argv[i+1]) ;
	i ++ ;
      }
      else {
	std::cerr << "Error in usage, bad values for the options, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }

    if (strncasecmp (argv[i], "-uploadInputOutput", 18) == 0) {
      choix = 1 ;
    }

    if (strncasecmp (argv[i], "-uploadPartition", 15) == 0) {
      choix = 2 ;
      if (i+1 < argc) {
	partitionNameDest = std::string(argv[i+1]) ;
	i ++ ;
      }
    }

    if (strncasecmp (argv[i], "-uploadConnection", 16) == 0) {
      choix = 3 ;
    }

    if (strncasecmp (argv[i], "-mergePartition", strlen("-mergePartition") ) == 0) {
      choix = 4 ;
      if (i+1 < argc) {
	partitionName1 = std::string(argv[i+1]) ;
	i ++ ;
	if (i+1 < argc) {
	  partitionNameDest = std::string(argv[i+1]) ;
	  i ++ ;
	}
      }
    }

    if (strncasecmp (argv[i], "-uploadRuns", strlen("-uploadRuns")) == 0) {
      choix = 5 ;
    }

    if (strncasecmp (argv[i], "-uploadFecPartition", 15) == 0) {
      choix = 6 ;
      if (i+1 < argc) {
	partitionNameDest = std::string(argv[i+1]) ;
	i ++ ;
      }
    }

  }

  // Warning in case of problem
  char *basic = getenv ("ENV_TRACKER_DAQ") ;
  if (basic == NULL) {
    std::cerr << "FED cannot be upload without the definition of the variable ENV_TRACKER_DAQ" << std::endl ;
    std::cerr << "Do you want still to continue ? (y/n)" ;
    char c = 'n' ; std::cin >> c ;
    if (c == 'n') return -1 ;
  }

  // Create the access and call the correct methods
  try {
    switch (choix) {
      
    case 0: help(argv[0]) ;
      break ;

    case 1: // Upload an input to an output
      if (inputFile.size() && outputFile.size() && partitionName.size()) 
	uploadInputOutput (inputFile, outputFile, partitionName) ;
      else
	help(argv[0]) ;
      break ;
    case 2: // Upload a complete partition
      if (inputFile.size() && outputFile.size() && partitionName.size() && 
	  (partitionNameDest.size() || outputFile == "file") )
	uploadPartition ( inputFile, outputFile, partitionName, partitionNameDest ) ; 
      else
	help(argv[0]) ;
      break ;
    case 3: // Upload the connection
      if (inputFile.size() && outputFile.size() && partitionName.size()) 
	uploadConnection (inputFile,outputFile,partitionName) ;
      else
	help(argv[0]) ;
      break ;
    case 4: // Merge the partitions
      std::cout << "-- Merge the partitions " << partitionName1 << " and " << partitionNameDest << " into " << partitionName << std::endl ;
      if (inputFile.size() && outputFile.size() && partitionNameDest.size() && partitionName1.size() &&
	  (partitionName.size() || outputFile == "file") )
	mergePartition ( inputFile, outputFile, partitionName1, partitionNameDest, partitionName ) ;
      else
	help(argv[0]) ;
      break ;
    case 5: // Upload all runs to another account
      std::cout << "-- Retreive all runs from the database account " << inputFile << " into the account " << outputFile << std::endl ;
      if (inputFile.size() && outputFile.size() && (inputFile != outputFile))
	uploadRuns ( inputFile, outputFile ) ;
      else
	help(argv[0]) ;
      break ;
    case 6: // create a FEC partition
      uploadFecPartition ( inputFile, outputFile, partitionName, partitionNameDest ) ; 
      break ;
    }
  }

  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ;
    std::cerr << "An error occurs during database access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  exit (EXIT_SUCCESS) ;
}


