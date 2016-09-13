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
#include "HashTable.h"
#include "deviceType.h"
#include "FecExceptionHandler.h"

#include "FecAccess.h"
#include "FecAccessManager.h"
#include "XMLFecDevice.h"
#include "XMLFecPiaReset.h"

// For hardware access
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
#include "FecVmeRingDevice.h"
#endif

#include "TkDcuConversionFactory.h"
  
  //#include "DeviceFactoryInterface.h"
#include "FecFactory.h"
#include "FecDetectionUpload.h"
#include "ErrorReportLogger.h"

static int fecSlot_ = -1 ;
static int ringMin_ = 0, ringMax_ = 1 ;

#define MODULECORRECTFILE "/tmp/ModuleCorrectScanned.txt"
#define MODULEINCORRECTFILE "/tmp/ModuleIncorrectScanned.txt"
#define INSTANCEFECSLOTFECHARDWAREIDFILE "/tmp/InstanceFecHardwareIdFecSlot.txt"

static bool multiFrames_ = false ; // multiframes used not tested, use at your own risk
static bool blockMode_   = false ; // Use the block mode to download the parameters
static bool forceAck_    = true  ; // Force acknowledge used by default

/**
 * \author Frederic Drouhin, Damien Vintache
 * \version 4.0, include the new FecFactory class for the parsing
 */

/** Display the errors coming from FecAccessManager download and upload operation
 */
void displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList, Sgi::hash_map<keyType, bool> *deviceError =  NULL ) ;

/** Parse a file and download the parameters parsed in the hardware
 * <p>command: -download
 * \param accessManager - FecAccessManager: device access manager
 * \param filename - file to be parsed
 * \param outputfile - output file for the upload
 * \param fecHardwareId - FEC hardware ID in case of several configuration inside the same file
 */
void parseDownloadFile ( FecAccessManager *accessManager, 
                         char *filename, char *outputfile,
			 std::string fecHardwareId ) {

  FecFactory fecFactory ;
  fecFactory.setUsingFile() ;
  fecFactory.setOutputFileName (outputfile) ;

  // vector of descriptions
  deviceVector devices ;
  piaResetVector vPiaReset ;

  try {

    // Parse a file for PIA reset
    if (fecHardwareId != "") {
      std::cout << "-------------- Parse the file " << filename << " for PIA reset (fecHardwareID = " << fecHardwareId << ")" << std::endl ;
      fecFactory.setInputFileName (filename, fecHardwareId) ;
    }
    else {
      std::cout << "-------------- Parse the file " << filename << " for PIA reset" << std::endl ;
      fecFactory.setInputFileName (filename) ;
    }

    // Retreive the values this vector must not be deleted, it is up to the FecFactory
    fecFactory.getPiaResetDescriptions ( vPiaReset ) ;

    // Create the access and reset the modules
    if (! vPiaReset.empty()) {

      std::string message ;
      std::list<FecExceptionHandler *> errorListPia ;
      if (!multiFrames_) {
	std::cout << "-------------- Reset the modules througth the PIA channels" << std::endl ;
	message = "Error during Reset the modules througth the PIA channels" ;
	accessManager->resetPiaModules (&vPiaReset, errorListPia) ;
      } else {
	std::cout << "-------------- Reset the modules througth the PIA channels (multiple frames)" << std::endl ;
	message = "Error during Reset the modules througth the PIA channels (multiple frames)" ;
	accessManager->resetPiaModulesMultipleFrames (&vPiaReset, errorListPia) ;
      }
      displayErrorMessage (message, errorListPia) ;
    }
    else {
      std::cout << "-------------- No PIA reset tags found in file " << filename << std::endl ;
    }

    // Retreive the devices
    if (fecHardwareId != "")
      std::cout << "-------------- Parse the file " << filename << " for devices (fecHardwareID = " << fecHardwareId << ")" << std::endl ;
    else
      std::cout << "-------------- Parse the file " << filename << std::endl ;
 
    // Parse a file for devices
    fecFactory.getFecDeviceDescriptions( devices ) ;

    // Download it
    std::list<FecExceptionHandler *> errorListDevice ;
    if (!multiFrames_) {
      std::cout << "-------------- Download for " << devices.size() << " devices " << std::endl ;
      accessManager->downloadValues (&devices, errorListDevice, false) ;              // =>  263.637 for 10000
    }
    else {
      std::cout << "-------------- Download multiple frames for " << devices.size() << " devices " << std::endl ;
      accessManager->downloadValuesMultipleFrames(&devices, errorListDevice, false) ; // => 291.141 for 10000
    }
    // Display the errors
    displayErrorMessage ("Error during the Download of devices", errorListDevice) ;

    // for measurement
    //struct timeval time1, time2;
    //struct timezone zone1, zone2;
    //gettimeofday(&time1, &zone1);
    //gettimeofday(&time2, &zone2);
    //double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    //double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    //double timesec = timesec2 - timesec1 ;
    //std::cout << "Time for 10000 donwload = " << timesec << std::endl ;

    // Display all the values set
    deviceVector *deviceUploaded = NULL ;
    std::list<FecExceptionHandler *> errorListDeviceUpload ;
    if (!multiFrames_) {
      std::cout << "-------------- Upload the devices connected" << std::endl ;
      deviceUploaded = accessManager->uploadValues ( errorListDeviceUpload ); 
    }
    else {
      std::cout << "-------------- Upload the devices connected through block of frames" << std::endl ;
      deviceUploaded = accessManager->uploadValuesMultipleFrames ( errorListDeviceUpload ); 
    }
    displayErrorMessage ("Error during the upload of the devices", errorListDeviceUpload ) ;
  
    std::cout << "-------------- Display the devices connected and upload it in file " << outputfile << std::endl ;
    if ( (deviceUploaded != NULL) && (!deviceUploaded->empty()) ) {
    
      //FecFactory::display (*deviceUploaded) ;
      FecDetectionUpload::checkDevices (*deviceUploaded, "UNKNOWN") ;
      fecFactory.setFecDeviceDescriptions (*deviceUploaded) ;

      // For each device => delete it
      FecFactory::deleteVectorI (*deviceUploaded) ;
      delete deviceUploaded ;
    }
    else {
    
      std::cerr << "No devices was uploaded" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}

/** Parse a file and download the parameters parsed in the hardware
 * <p>command: -testi2c
 * \param accessManager - FecAccessManager: device access manager
 * \param filename - file to be parsed
 * \param outputfile - output file for the upload
 */
void parseDownloadFileTestI2c ( FecAccess *fecAccess,
				FecAccessManager *accessManager, 
				char *filename ) {

  std::cerr << "Start time = " << std::dec << time(NULL) << std::endl ;

  long double errorDownloadCounter = 0, downloadCounter = 0 ; 
  long double errorUploadCounter = 0, errorUploadComparisonCounter = 0, uploadCounter = 0 ;

  FecFactory fecFactory ;
  fecFactory.setUsingFile() ;
  
  try {

    // Parse a file for PIA reset
    std::cout << "-------------- Parse the file " << filename << " for PIA reset" << std::endl ;
    fecFactory.setInputFileName (filename) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot parse the file " << filename << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Vector of PIA reset descriptions
  piaResetVector vPiaReset ;

  try {
    fecAccess->fecHardReset() ;
    fecAccess->fecRingReset() ;

    // Retreive the values this vector must not be deleted, it is up to the FecFactory
    fecFactory.getPiaResetDescriptions ( vPiaReset ) ;

    // Create the access and reset the modules
    if (! vPiaReset.empty()) {

      std::string message ;
      std::list<FecExceptionHandler *> errorListPia ;
      if (!multiFrames_) {
	std::cout << "-------------- Reset the modules througth the PIA channels" << std::endl ;
	message = "Error during reset the modules througth the PIA channels" ;
	accessManager->resetPiaModules (&vPiaReset, errorListPia) ;
      }
      else {
	std::cout << "-------------- Reset the modules througth the PIA channels (multiple frames)" << std::endl ;
	message = "Error during reset the modules througth the PIA channels (multiple frames)" ;
	accessManager->resetPiaModulesMultipleFrames (&vPiaReset, errorListPia) ;
      }
      displayErrorMessage (message, errorListPia) ; 
      //FecFactory::deleteVector (vPiaReset) ;
    }
    else {
      std::cerr << "-------------- No PIA reset tags found in file " << filename << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot send a PIA reset" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // vector of device descriptions
  deviceVector devices, pllDevices ;
    
  try {
    // Retreive the devices
    std::cout << "-------------- Parse the file " << filename << std::endl ;
    
    // Parse a file for devices
    fecFactory.getFecDeviceDescriptions( devices ) ;

    for (deviceVector::iterator device = devices.begin() ; device != devices.end() ; device ++) {

      deviceDescription *deviced = *device ;

      if (deviced->getDeviceType() == PLL) {
	pllDevices.push_back(deviced) ;
      }
    }

    std::cout << "Found " << pllDevices.size() << " PLLs" << std::endl ;
  }
  catch (FecExceptionHandler e) {
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Unable to parse the file" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  while (true) {

    try {

      downloadCounter += 1 ;

      // Download it
      if (downloadCounter == 1) {
	std::cout << "-------------- Download " << devices.size() << " devices" ;
      }
      else {
	sleep(1) ;
	std::cout << "-------------- Download " << pllDevices.size() << " PLL" ;
      }

      unsigned int error = 0 ;
      std::list<FecExceptionHandler *> errorListDevice ;
      if (!multiFrames_) {
	std::cout << " in mono-frame mode" << std::endl ;
	if (downloadCounter == 1) {
	  error = accessManager->downloadValues (&devices, errorListDevice) ;
	}
	else {
	  error = accessManager->downloadValues (&pllDevices, errorListDevice) ;
	}
      }
      else {
	std::cout << " in multi-frames mode" << std::endl ;
	if (downloadCounter == 1) {
	  error = accessManager->downloadValuesMultipleFrames (&devices, errorListDevice) ;
	}
	else {
	  error = accessManager->downloadValuesMultipleFrames (&pllDevices, errorListDevice) ;
	}
      }

      // Display the errors
      displayErrorMessage ("Error during the Download of devices", errorListDevice) ;

      if (error) {
	
	errorDownloadCounter += 1 ;
	std::cerr << "ERRORTOBEANALYSED: Detect on error during the download (" << std::dec << errorDownloadCounter << "/" << downloadCounter << ")" << std::endl ;
	
	// Vector of PIA reset
	piaResetVector vPiaReset ;
	try {
	  
	  // Remove all the devices from the management class
	  accessManager->removeDevices() ;
	  
	  // Make a reset
	  fecAccess->fecHardReset() ;
	  fecAccess->fecRingReset() ;
	
	  // Retreive the values this vector must not be deleted, it is up to the FecFactory
	  fecFactory.getPiaResetDescriptions ( vPiaReset ) ;
	  
	  // Create the access and reset the modules
	  if (! vPiaReset.empty()) {
	    
	    std::string message ;
	    std::list<FecExceptionHandler *> errorListPia ;
	    if (!multiFrames_) {
	      std::cout << "-------------- Reset the modules througth the PIA channels" << std::endl ;
	      message = "Error during reset the modules througth the PIA channels" ;
	      accessManager->resetPiaModules (&vPiaReset, errorListPia) ;
	    }
	    else {
	      std::cout << "-------------- Reset the modules througth the PIA channels" << std::endl ;
	      message = "Error during reset the modules througth the PIA channels (multiple frames)" ;
	      accessManager->resetPiaModulesMultipleFrames (&vPiaReset, errorListPia) ;
	    }
	    // Display the errors
	    displayErrorMessage (message, errorListPia) ;
	  }
	  else {
	    std::cout << "-------------- No PIA reset tags found in file " << filename << std::endl ;
	  }
	}
	catch (FecExceptionHandler &e) {
	  
	  std::cerr << "*********** ERROR ********************************" << std::endl ; 
	  std::cerr << "Cannot send a PIA reset" << std::endl ;
	  std::cerr << e.what()  << std::endl ;
	  std::cerr << "**************************************************" << std::endl ;
	}
      }

//       // Display all the values set
//       std::cout << "-------------- Upload the devices connected" << std::endl ;
//       uploadCounter += 1 ;
//       deviceVector *deviceUploaded = NULL ;

//       std::list<FecExceptionHandler *> errorListDeviceUpload ;
//       if (!multiFrames_)
//   	deviceUploaded = accessManager->uploadValues ( errorListDeviceUpload, true ) ;
//       else
//   	deviceUploaded = accessManager->uploadValuesMultipleFrames ( errorListDeviceUpload, true ); 
//       // Display the errors
//       displayErrorMessage ("Error during the upload of devices", errorListDeviceUpload) ;

//       if ( (deviceUploaded != NULL) && (deviceUploaded->size() != 0) ) {
	
// 	//FecFactory::display (*deviceUploaded) ;

// 	// For each device, if it is not a DCU then display the following error
// 	for (deviceVector::iterator device = deviceUploaded->begin() ; device != deviceUploaded->end() ; device ++) {

// 	  deviceDescription *deviced = *device ;
// 	  bool displayError = false ;
// 	  if (deviced->getDeviceType() != DCU) {
// 	    std::cerr << "*********** ERRORCOMPARISON ********************************" << std::endl ; 
// 	    std::cerr << "Error in the comparison" << std::endl ;
// 	    std::cerr << "************************************************************" << std::endl ;
// 	    displayError = true ;
// 	  }

// 	  if (displayError) {
// 	    errorUploadComparisonCounter += 1 ;
// 	    std::cerr << "ERRORTOBEANALYSED: Detect a comparison during the upload (" << std::dec << errorUploadComparisonCounter << "/" << uploadCounter << ")" << std::endl ;
// 	  }
// 	}
//       }

//       if (accessManager->getLastErrorLastOperation () != 0) {

// 	errorUploadCounter += 1; 
// 	std::cerr << "ERRORTOBEANALYSED: Detect an error during the upload (" << std::dec << errorUploadCounter << "/" << uploadCounter << ")" << std::endl ;
	
// 	// Vector of PIA reset descriptions
// 	piaResetVector vPiaReset ;
// 	try {

// 	  // Remove all the devices
// 	  accessManager->removeDevices() ;

// 	  fecAccess->fecHardReset() ;
// 	  fecAccess->fecRingReset() ;

// 	  // Retreive the values this vector must not be deleted, it is up to the FecFactory
// 	  fecFactory.getPiaResetDescriptions ( vPiaReset ) ;
	  
// 	  // Create the access and reset the modules
// 	  if (! vPiaReset.empty()) {
	    
// 	    std::string message ;
// 	    std::list<FecExceptionHandler *> errorListPia ;
// 	    if (!multiFrames_) {
// 	      std::cout << "-------------- Reset the modules througth the PIA channels" << std::endl ;
// 	      message = "Error during Reset the modules througth the PIA channels" ;
// 	      accessManager->resetPiaModules (&vPiaReset, errorListPia) ;
// 	    }
// 	    else {
// 	      std::cout << "-------------- Reset the modules througth the PIA channels (multiple frames)" << std::endl ;
// 	      message = "Error during Reset the modules througth the PIA channels (multiple frames)" ;
// 	      accessManager->resetPiaModulesMultipleFrames (&vPiaReset, errorListPia) ;
// 	    }
// 	    displayErrorMessage (message, errorListPia) ;
// 	    //FecFactory::deleteVector (vPiaReset) ;      
// 	  }
// 	  else {
// 	    std::cerr << "-------------- No PIA reset tags found in file " << filename << std::endl ;
// 	  }
// 	}
// 	catch (FecExceptionHandler &e) {
	  
// 	  std::cerr << "*********** ERROR ********************************" << std::endl ; 
// 	  std::cerr << "Cannot send a PIA reset" << std::endl ;
// 	  std::cerr << e.what()  << std::endl ;
// 	  std::cerr << "**************************************************" << std::endl ;
// 	}
//       }

//       try {
// 	FecFactory::deleteVectorI (*deviceUploaded) ;
// 	delete deviceUploaded ;
//       }
//       catch (FecExceptionHandler &e) {

// 	  std::cerr << "*********** ERROR ********************************" << std::endl ; 
// 	  std::cerr << "Cannot delete accessed in FecAccess" << std::endl ;
// 	  std::cerr << e.what()  << std::endl ;
// 	  std::cerr << "**************************************************" << std::endl ;
//       }
  
//       std::cout << "-------------- Display the devices connected and upload it" << std::endl ;

#define VALUEPRINT 100
      if ((downloadCounter/VALUEPRINT) - (unsigned long)(downloadCounter/VALUEPRINT) == 0) {
	std::cerr << std::dec << time(NULL) << std::endl ;
	std::cerr << std::dec << errorDownloadCounter << " download errors on " << downloadCounter << " donwload operations" << std::endl ;
	std::cerr << std::dec << errorUploadCounter << " upload errors on " << uploadCounter << " upload operations" << std::endl ;
	std::cerr << std::dec << errorUploadComparisonCounter << " upload comparison errors on " << uploadCounter << " upload operations" << std::endl ;
      }
      else {
	std::cout << std::dec << errorDownloadCounter << " download errors on " << downloadCounter << " donwload operations" << std::endl ;
	std::cout << std::dec << errorUploadCounter << " upload errors on " << uploadCounter << " upload operations" << std::endl ;
	std::cout << std::dec << errorUploadComparisonCounter << " upload comparison errors on " << uploadCounter << " upload operations" << std::endl ;
      }
    }
    catch (FecExceptionHandler &e) {

      std::cerr << "*********** ERROR ********************************" << std::endl ; 
      std::cerr << "An error occurs during hardware access" << std::endl ;
      std::cerr << e.what()  << std::endl ;
      std::cerr << "**************************************************" << std::endl ;
    }
  }
}

/** Make an automatic detection of the Tracker devices, build the descriptions,
 * make the download through the FecAccessManager class (idem with FecSupervisor) 
 * and write the XML device values in the output file
 * <p>command: -tracker
 * <p>command: -createPartition
 * \param fecAccess - FecAccess class for detect all devices
 * \param accessManager - FecAccessManager: device access manager
 * \param outputfile - output file for the upload
 */
void uploadDetectedTrackerDevices ( FecAccess *fecAccess, 
				    FecAccessManager *fecAccessManager,
				    char *inputfile,
				    char *outputfile,
				    std::string partitionName,
				    std::string structureName) {

  // --------------------------------------------------------------
  // Check if some FECs exists
  std::list<keyType> *fecSlotList = fecAccess->getFecList ( );
  if ( (fecSlotList == NULL) || (fecSlotList->size() == 0) ) {  
    std::cout << "No FEC found, cannot continue" << std::endl ;
    return ;
  }

  // --------------------------------------------------------------
  // To display the errors
  ErrorReportLogger errorReportLogger("FecProfiler", true, true, LOGDEBUG) ;

  // --------------------------------------------------------------
  // Upload in file or database
  bool doUploadInDatabase = false, doUploadInFile = false ;
  std::string xmlFECFileName = "" ;
  // Factory
  FecFactory *fecFactory = NULL ;

#ifdef DATABASE
  std::string login="nil", passwd="nil", path="nil" ;
  DbFecAccess::getDbConfiguration (login, passwd, path) ;
  if ((strncasecmp (outputfile, "database", 8) == 0) && (login == "nil"))
    std::cerr << "The login, password, path is not set cannot upload in database" << std::endl ;

  if ((strncasecmp (outputfile, "database", 8) == 0) && (login != "nil") && (partitionName != "") && (partitionName != "nil") ) {
	
    // Database connection
    try {
      std::cout << "-------------- Display the devices connected and upload it in database" << std::endl ;
      std::cout << "Create a database access for " << login << "/" << passwd << "@" << path << std::endl ;
      fecFactory = new FecFactory (login, passwd, path) ;
      doUploadInDatabase = true ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cerr << "*********** ERROR *************" << std::endl ;
      std::cerr << "XML parsing for database error " << e.what() << std::endl ;
      std::cerr << "*******************************" << std::endl ;
      doUploadInDatabase = false ;
    }
    catch (oracle::occi::SQLException &e) {
      
      std::cerr << "*********** ERROR *************" << std::endl ;
      std::cerr << "Unable to connect the database " << login << "@" << path << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "*******************************" << std::endl ;
      doUploadInDatabase = false ;
    }
  }
  else 
#endif
    {
      try {
        fecFactory = new FecFactory () ;
	doUploadInFile = true ;
	if (strcmp(outputfile, UNKNOWNFILE)) xmlFECFileName = outputfile ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "*********** ERROR *************" << std::endl ;
	std::cerr << "XML upload in file error " << e.what() << std::endl ;
	std::cerr << "*******************************" << std::endl ;
	doUploadInFile = false ;
      }
    }

  // ----------------------------------------------------------
  // Template file
  std::string fecTemplateFileName = "" ;
  if (strcmp(inputfile, UNKNOWNFILE)) fecTemplateFileName = inputfile ;
  if (partitionName == "nil") partitionName = "" ;

  // ----------------------------------------------------------
  // Remove the bad modules
  int removeAOH = 3 ;
  char answer = 'r' ;
  do {
    std::cout << "Do you want to remove the module AOH or PLL or MUX (Y/N): " ;
    answer = getchar() ;
    if (answer == 'y' || answer == 'Y') removeAOH = 1 ;
    else if (answer == 'N' || answer == 'n') removeAOH = 0 ;
    else std::cout << "Please enter y/n or Y/N" << std::endl ;
  } while (removeAOH == 3) ;

  // ---------------------------------------------------------
  // range of ring
  int rangeMin = 0, rangeMax = 0 ;
  std::cout << "Please specify the first FEC slot: (0 to have all rings) => " ;
  std::cin >> rangeMin ;
  std::cout << "Please specify the last FEC slot: (0 to have all rings) => " ;
  std::cin >> rangeMax ;

  // ring 6 => FEC 0x6 ring 0x3 CCU 0x6b channel 0x1c I2C address 0x44
  // FEC 8 problem
  // FEC 13 problem
  // FEC 14 ring 0x2 => then the rest works
  // FEC 15 problem
  // FEC 16 problem

  // ----------------------------------------------------------
  // Detect and upload
  bool createPartition = true ;
  bool displayMessage = true ;
  std::list<keyType> moduleCorrect ;
  std::list<keyType> moduleIncorrect ;
  std::list<keyType> moduleCannotBeUsed ;
  std::list<keyType> dcuDohOnCcu ;
  Sgi::hash_map<keyType, bool> deviceMissing ;
  Sgi::hash_map<keyType, bool> deviceError ;
  std::list<std::string> listErrors ;

  //fecAccessManager->setDisplayDebugMessage(displayMessage) ;
  if ( (rangeMin) && (rangeMax) ) 
    FecDetectionUpload::uploadDetectFECs ( *fecAccess, *fecAccessManager, errorReportLogger, fecFactory,
					   MODULECORRECTFILE, MODULEINCORRECTFILE, INSTANCEFECSLOTFECHARDWAREIDFILE,
					   fecTemplateFileName, xmlFECFileName, partitionName, partitionName,
					   (removeAOH == 1), displayMessage, multiFrames_, doUploadInDatabase, doUploadInFile, createPartition, 
					   moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceError, deviceMissing, listErrors, "UNKNOWN", 
					   buildFecRingKey(rangeMin,1), buildFecRingKey(rangeMax,8)) ;
  else 
    FecDetectionUpload::uploadDetectFECs ( *fecAccess, *fecAccessManager, errorReportLogger, fecFactory,
					   MODULECORRECTFILE, MODULEINCORRECTFILE, INSTANCEFECSLOTFECHARDWAREIDFILE,
					   fecTemplateFileName, xmlFECFileName, partitionName, partitionName,
					   (removeAOH == 1), displayMessage, multiFrames_, doUploadInDatabase, doUploadInFile, createPartition, 
					   moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceError, deviceMissing, listErrors, "UNKNOWN") ;
  //buildFecRingKey(8,7),buildFecRingKey(8,7)) ;

  try {
    if (fecFactory != NULL) delete fecFactory ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "Unable to disconnect the database or file " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }
#ifdef DATABASE
  catch (oracle::occi::SQLException &e) {
    
    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "Unable to disconnect the database " << login << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }
#endif

//   // --------------------------------------------------------------
//   // Create the FEC instance for the FECs
//   keyType fecInstance[MAX_NUMBER_OF_SLOTS] ;
//   for (int i = 0 ; i < MAX_NUMBER_OF_SLOTS ; i ++) fecInstance[i] = 0xFFFFFFFF ;
//   std::ofstream *temp = NULL ;
//   std::ofstream instanceStream(INSTANCEFECSLOTFECHARDWAREIDFILE);
//   if (!instanceStream.is_open()) std::cerr << "Cannot create status file for instance, FEC ID and FEC slot" << std::endl ;
//   else temp = &instanceStream ;
//   FecDetectionUpload::getCrateFecSupervisorInstance (*fecAccess, fecInstance, partitionName, temp) ;

//   // -------------------------------------------------------------------
//   // Create the PIA reset for all the CCUs on all rings
//   piaResetVector vPiaReset ;

//   // If an input file as been specified then retreive the PIA reset to
//   // get the values for the channels, delay of reset, interval delay
//   tscType8 channel[4] = {0} ; // 0x30 -> 0x33
//   unsigned long delayActiveReset[4] = {0} ; // one delay per channel
//   unsigned long intervalDelayReset[4] = {0} ; // one delay per channel
//   tscType8 mask[4] = {0} ; // one mask per channel ;

//   // retreive the vector of descriptions
//   FecFactory fecFactory ;
//   fecFactory.setUsingFile() ;
//   // Parse a file for devices
//   std::cout << "-------------- Parse the file " << inputfile << " for i2c devices settings" << std::endl ;
  
//   // try to find the input file
//   if (strcmp(inputfile, UNKNOWNFILE)) {
//     fecFactory.addFileName ( inputfile ) ;

//     // Retreive the devices
//     piaResetVector inputPiaResets ;
//     fecFactory.getPiaResetDescriptions ( inputPiaResets ) ;

//     if (inputPiaResets.size() > 0) {

//       // For each input
//       for (piaResetVector::iterator device =inputPiaResets .begin() ; device != inputPiaResets.end() ; device ++) {
//         piaResetDescription *deviced = *device ;
// 	tscType8 channelPia = getChannelKey(deviced->getKey()) ;
// 	switch (channelPia) {
// 	case 0x30:
// 	case 0x31:
// 	case 0x32:
// 	case 0x33:
// 	  channel[channelPia-0x30] = channelPia ;
// 	  delayActiveReset[channelPia-0x30] = deviced->getDelayActiveReset() ;
// 	  intervalDelayReset[channelPia-0x30] = deviced->getIntervalDelayReset() ;
// 	  mask[channelPia-0x30] = deviced->getMask() ;
// 	  break ;
// 	default:
// 	  std::cerr << "Invalid PIA channel in the input file " << inputfile << " (PIA channel = 0x" << std::hex << (int)channelPia << ")" << std::endl ;
// 	  break ;
// 	}
//       }
//     }
//   }
//   else { // No input file
//     channel[0] = 0x30 ;
//     delayActiveReset[0] = 10 ;
//     intervalDelayReset[0] = 10000 ;
//     mask[0] = 255 ;
//   }

//   // For each channel create all the PIA access
//   std::list<std::string> listPiaError ;
//   for (int i = 0 ; i < 4 ; i ++) {
//     if (channel[i] != 0) {
//       FecDetectionUpload::getCratePiaReset (vPiaReset, listPiaError, *fecAccess, channel[i], delayActiveReset[i], intervalDelayReset[i], mask[i]) ;
//     }
//   }

//   // Display the errors
//   if (listPiaError.size() > 0) {
//     std::cerr << std::dec << listPiaError.size() << " errors on creating the PIA reset: " << std::endl ;

//     for ( std::list<std::string>::iterator it = listPiaError.begin() ; it != listPiaError.end() ; it ++) {
//       std::cerr << "\t" << *it << std::endl ;
//     }
//   }

//   // --------------------------------------------------------
//   // Download the PIA reset
//   if (vPiaReset.size() > 0) {

//     std::string message ;
//     std::list<FecExceptionHandler *> errorListPia ;
//     try {
//       if (!multiFrames_) {
// 	std::cout << "-------------- Reset modules through PIA resets (" << std::dec << vPiaReset.size() << ")" << std::endl ;
// 	message = "Error during Reset the modules througth the PIA channels" ;
// 	accessManager->resetPiaModules(&vPiaReset, errorListPia) ;
//       }
//       else {
// 	std::cout << "-------------- Reset modules through PIA resets in multiple frames (" << std::dec << vPiaReset.size() << ")" << std::endl ;
// 	message = "Error during Reset the modules througth the PIA channels (multiple frames)" ;
// 	accessManager->resetPiaModulesMultipleFrames(&vPiaReset, errorListPia) ;
//       }
//       displayErrorMessage (message, errorListPia) ;
//     }
//     catch (FecExceptionHandler &e) {
//       std::cerr << message << ": " << e.what() << std::endl ;
//     }
//   }
//   else {
//     std::cerr << "No PIA reset to be downloaded" << std::endl ;
//   }

//   // ---------------------------------------------------------
//   // Retreive the input files for the devices
//   apvDescription   *apvIn   = NULL ;
//   muxDescription   *muxIn   = NULL ;
//   pllDescription   *pllIn   = NULL ;
//   laserdriverDescription *laserIn = NULL ;
//   laserdriverDescription *dohIn   = NULL ;

//   // try to find the input file
//   if (strcmp(inputfile, UNKNOWNFILE)) {

//     // Vector of descriptions
//     deviceVector deviceInput ;

//     // retreive the devices
//     fecFactory.getFecDeviceDescriptions ( deviceInput ) ;

//     if (!deviceInput.empty()) {
      
//       // For each device => access it
//       for (deviceVector::iterator device = deviceInput.begin() ; device != deviceInput.end() ; device ++) {
	
// 	deviceDescription *deviced = *device ;
// 	switch (deviced->getDeviceType()) {
// 	case APV25:
// 	  apvIn = (apvDescription *)deviced->clone() ;
// 	  break ;
// 	case APVMUX:
// 	  muxIn = (muxDescription *)deviced->clone() ;
// 	  break ;
// 	case PLL:
// 	  pllIn = (pllDescription *)deviced->clone() ;
// 	  break ;
// 	case LASERDRIVER:
// 	  if (getAddressKey(deviced->getKey()) == 0x60) {
// 	    // laserdriver
// 	    laserIn = (laserdriverDescription *)deviced->clone() ;
// 	  }
// 	  else if (getAddressKey(deviced->getKey()) == 0x70) {
// 	    // DOH
// 	    dohIn = (laserdriverDescription *)deviced->clone() ;
// 	  }
// 	  break ;
// 	case DCU:
// 	  break ;
// 	default:
// 	  break ;
// 	}

// 	//deviced->display ( ) ;
//       }
//     }
//   }

//   if (apvIn == NULL) {
//     apvIn = new apvDescription((tscType8)0x2b,
// 			       (tscType8)0x64,
// 			       (tscType8)0x4,
// 			       (tscType8)0x73,
// 			       (tscType8)0x3c,
// 			       (tscType8)0x32,
// 			       (tscType8)0x32,
// 			       (tscType8)0x32,
// 			       (tscType8)0x50,
// 			       (tscType8)0x32,
// 			       (tscType8)0x50,
// 			       (tscType8)0,    // Ispare
// 			       (tscType8)0x43,
// 			       (tscType8)0x43,
// 			       (tscType8)0x14,
// 			       (tscType8)0xFB,
// 			       (tscType8)0xFE,
// 			       (tscType8)0) ;
//   }
//   if (muxIn == NULL) {
//     muxIn = new muxDescription((tscType16)0xFF) ;
//   }
//   if (pllIn == NULL) {
//     pllIn = new pllDescription((tscType8)6,(tscType8)1) ;
//   }
//   if (laserIn == NULL) {
//     tscType8 gainAOH = 2 ; 
//     tscType8 biasAOH[3] = {23, 23, 23} ;
//     laserIn = new laserdriverDescription(gainAOH, biasAOH) ;
//   }
//   if (dohIn == NULL) {
//     tscType8 gainDOH = 2 ;
//     tscType8 biasDOH[3] = {24, 24, 24} ;
//     dohIn = new laserdriverDescription(gainDOH, biasDOH) ;
//   }

//   // ---------------------------------------------------------------------------
//   // Create all the devices for the Tracker
//   std::cout << "-------------- Scanning rings for devices" << std::endl ;
//   deviceVector vDevices ;
//   std::list<std::string> listDeviceError ;
//   FecDetectionUpload::getCrateFecDevices (vDevices, listDeviceError, *fecAccess, *apvIn, *muxIn, *pllIn, *laserIn, *dohIn) ;

//   // ---------------------------------------------------------------------------
//   // Download and upload the devices
//   if (vDevices.size() == 0) {
//     FecFactory::deleteVector(vPiaReset) ;
//     delete apvIn ; delete muxIn ; delete pllIn ; delete laserIn ; delete dohIn ;  
//     return ;
//   }

//   std::cout << "-------------- Download the devices detected (" << std::dec << vDevices.size() << ")" << std::endl ;
//   std::list<FecExceptionHandler *> errorListDevice ;
//   if (!multiFrames_)
//     accessManager->downloadValues (&vDevices, errorListDevice) ;
//   else 
//     accessManager->downloadValuesMultipleFrames (&vDevices, errorListDevice) ;
//   // Display the errors
//   Sgi::hash_map<keyType, bool> deviceError ;
//   displayErrorMessage ("Error during the Download of devices", errorListDevice, &deviceError) ;

//   deviceVector *deviceUploaded = NULL ;
//   std::list<FecExceptionHandler *> errorListDeviceUpload ;
//   if (!multiFrames_) {
//     std::cout << "-------------- Upload the devices connected" << std::endl ;
//     deviceUploaded = accessManager->uploadValues ( errorListDeviceUpload ) ;
//   }
//   else {
//     std::cout << "-------------- Upload the devices connected through multiple frames" << std::endl ;
//     deviceUploaded = accessManager->uploadValuesMultipleFrames ( errorListDeviceUpload ) ;
//   }
//   displayErrorMessage ("Error during the upload of the devices", errorListDeviceUpload, &deviceError);

//   // Compare both vector (upload and download) and the devices to the new vector which are faulty
//   if (deviceUploaded->size() != vDevices.size()) {

//     for (deviceVector::iterator device = vDevices.begin() ; device != vDevices.end() ; device ++) {
      
//       bool notPresent = true ;
//       for (deviceVector::iterator deviceU = deviceUploaded->begin() ; deviceU != deviceUploaded->end() ; deviceU ++) {
// 	if ((*device)->getKey() == (*deviceU)->getKey()) notPresent = false ;
//       }

//       if (notPresent) {
// 	char msg[80] ; decodeKey(msg,(*device)->getKey()) ;
// 	std::cout << "Cannot upload the device on " << msg << std::endl ;
//       }
//     }
//   }
//   FecFactory::deleteVector(*deviceUploaded) ;

//   // --------------------------------------------------------------------------
//   // Output file
//   std::ofstream correctStream(MODULECORRECTFILE);
//   std::ofstream incorrectStream(MODULEINCORRECTFILE);
//   if (!correctStream.is_open() || !incorrectStream.is_open()) {
//     std::cerr << "Cannot create status file for correct and incorrect modules" << std::endl ;
    
//     if (correctStream.is_open()) correctStream.close() ;
//     if (incorrectStream.is_open()) incorrectStream.close() ;
//   }

//   // ---------------------------------------------------------------------------
//   // Check for problem in the devices
//   std::list<keyType> moduleCorrect ;
//   std::list<keyType> moduleIncorrect ;
//   std::list<keyType> moduleCannotBeUsed ;
//   std::list<keyType> dcuDohOnCcu ;
//   Sgi::hash_map<keyType, bool> deviceMissing ;
//   bool displayMessage = true ;
//   FecDetectionUpload::checkDevices(vDevices, moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceMissing, "UNKNOWN", displayMessage) ;
  
//   // ---------------------------------------------------------------------------
//   // Check for problem in the devices
//   std::cout << "-------------- Summary" << std::endl ;  
//   FecDetectionUpload::checkDevices(vDevices, moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceMissing, displayMessage) ;
//   if (correctStream.is_open()) {
//     if (displayMessage) std::cout << "Found " << std::dec << moduleCorrect.size() << " module(s) without missing devices" << std::endl ;
//     correctStream << "Found " << std::dec << moduleCorrect.size() << " module(s) without errors " << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (moduleCorrect, deviceError, deviceMissing, displayMessage, &correctStream) ;
//     if (displayMessage) std::cout << "Found " << std::dec << moduleIncorrect.size() << " module(s) with errors on APV or DCU missing" << std::endl ;
//     correctStream << "Found " << std::dec << moduleIncorrect.size() << " module(s) with errors on APV or DCU missing" << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (moduleIncorrect, deviceError, deviceMissing, displayMessage, &correctStream) ;
//     // DCU CCU and DOH
//     std::list<keyType> dcuCcuList, dohList ;
//     for (std::list<keyType> ::iterator it = dcuDohOnCcu.begin() ; it != dcuDohOnCcu.end() ; it ++) {
//       if (getAddressKey((*it)) == DCUADDRESS) dcuCcuList.push_back((*it)) ;
//       else //if (getAddressKey((*it)) == 0x70) 
// 	dohList.push_back((*it)) ;
//     }
//     if (displayMessage) std::cout << "Found " << std::dec << dcuCcuList.size() << " DCU(s) on CCU" << std::endl ;
//     correctStream << "Found " << std::dec << dcuCcuList.size() << " DCU(s) on CCU" << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (dcuCcuList, deviceError, deviceMissing, displayMessage, &correctStream) ;
//     if (displayMessage) std::cout << "Found " << std::dec << dcuCcuList.size() << " DOH(s)" << std::endl ;
//     correctStream << "Found " << std::dec << dcuCcuList.size() << " DOH(s)" << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (dohList, deviceError, deviceMissing, displayMessage, &correctStream) ;
//     correctStream.close() ;
//   }
//   else if (displayMessage) {
//     std::cout << "Found " << std::dec << moduleCorrect.size() << " module(s) without missing devices " << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (moduleCorrect, deviceError, deviceMissing, displayMessage) ;
//     std::cout << "Found " << std::dec << moduleIncorrect.size() << " module(s) with errors on APV or DCU missing" << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (moduleIncorrect, deviceError, deviceMissing, displayMessage) ;
//     // DCU CCU and DOH
//     std::list<keyType> dcuCcuList, dohList ;
//     for (std::list<keyType> ::iterator it = dcuDohOnCcu.begin() ; it != dcuDohOnCcu.end() ; it ++) {
//       if (getAddressKey((*it)) == DCUADDRESS) dcuCcuList.push_back((*it)) ;
//       else //if (getAddressKey((*it)) == 0x70) 
// 	dohList.push_back((*it)) ;
//     }
//     std::cout << "Found " << std::dec << dcuCcuList.size() << " DCU(s) on CCU" << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (dcuCcuList, deviceError, deviceMissing, displayMessage) ;
//     std::cout << "Found " << std::dec << dcuCcuList.size() << " DOH(s)" << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (dohList, deviceError, deviceMissing, displayMessage) ;
//   }
//   // Fatal problem for the modules
//   if (incorrectStream.is_open()) {
//     incorrectStream << "Found " << std::dec << moduleCannotBeUsed.size() << " module(s) with fatal error: PLL, MUX or AOH missing " << std::endl ;
//     if (displayMessage) std::cout << "Found " << std::dec << moduleCannotBeUsed.size() << " module with fatal error: PLL, MUX or AOH missing " << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (moduleCannotBeUsed, deviceError, deviceMissing, displayMessage, &incorrectStream) ;
//     incorrectStream.close() ;
//   }
//   else if (displayMessage) {
//     std::cout << "Found " << std::dec << moduleCannotBeUsed.size() << " module with fatal error: PLL, MUX or AOH missing " << std::endl ;
//     FecDetectionUpload::dumpModuleScreenFile (moduleCannotBeUsed, deviceError, deviceMissing, displayMessage) ;
//   }

//   // Close the file
//   if (instanceStream.is_open()) {
//     instanceStream.close() ;
//     std::cout << "Instance modules stored in " << INSTANCEFECSLOTFECHARDWAREIDFILE << std::endl ;
//   }
//   if (correctStream.is_open()) {
//     correctStream.close() ;
//     std::cout << "Correct modules stored in " << MODULECORRECTFILE << std::endl ;
//   }
//   if (incorrectStream.is_open()) {
//     incorrectStream.close() ;
//     std::cout << "Correct modules stored in " << MODULEINCORRECTFILE << std::endl ;
//   }
  
//   // Remove the bad devices if needed
//   deviceVector *deviceCorrect = &vDevices ;
//   int removeAOH = 3 ;

//   // Remove the bad modules (if needed)
//   if (moduleCannotBeUsed.size() > 0) {

//     char answer = '0' ;
//     do {
//       std::cout << "Do you want to remove the module AOH or PLL or MUX (Y/N): " ;
//       answer = getchar() ;
//       if (answer == 'y' || answer == 'Y') removeAOH = 1 ;
//       else if (answer == 'N' || answer == 'n') removeAOH = 0 ;
//       else std::cout << "Please enter y/n or Y/N" << std::endl ;
//     } while (removeAOH == 3) ;

//     if (removeAOH == 1) {

//       deviceCorrect = new deviceVector() ;
	
//       // Remove the corresponding device
//       for (deviceVector::iterator device = vDevices.begin() ; device != vDevices.end() ; device ++ ) {
	
// 	bool toBeRemoved = false ;
// 	for (std::list<keyType>::iterator it = moduleCannotBeUsed.begin() ; it != moduleCannotBeUsed.end() ; it ++) {
	  
// 	  if (getFecRingCcuChannelKey(*it) == getFecRingCcuChannelKey((*device)->getKey())) {
// 	    //char msg[80] ;
// 	    //decodeKey(msg,(*device)->getKey()) ;
// 	    //std::cout << "Remove the device on " << msg << std::endl ;
// 	    toBeRemoved = true ;
// 	  }
// 	}
// 	// If the module has an aoh => add it
// 	if (!toBeRemoved) deviceCorrect->push_back (*device) ;
//       }
//     }
//   }

//   // --------------------------------------------------------------
//   // Upload in file or database
// #ifdef DATABASE
//   std::string login="nil", passwd="nil", path="nil" ;
//   DbFecAccess::getDbConfiguration (login, passwd, path) ;
//   if ((strncasecmp (outputfile, "database", 8) == 0) && (login == "nil"))
//     std::cerr << "The login, password, path is not set cannot upload in database" << std::endl ;

//   if ((strncasecmp (outputfile, "database", 8) == 0) && (login != "nil") && (partitionName != "") && (partitionName != "nil") ) {
	
//     // Database connection
//     try {
//       std::cout << "-------------- Display the devices connected and upload it in database" << std::endl ;
//       std::cout << "Create a database access for " << login << "/" << passwd << "@" << path << std::endl ;
//       FecFactory fecFactory (login, passwd, path) ;
	  
//       unsigned int major, minor, piamajor = 0, piaminor = 0 ;

//       // Example to create new partition 
//       if (! vPiaReset.empty()) {

// 	//FecFactory::display(vPiaReset) ;
// 	//FecFactory::display(deviceUploaded) ;
// 	fecFactory.createPartition (*deviceCorrect, vPiaReset, &major, &minor, &piamajor, &piaminor, structureName, partitionName) ;
//       }
//       else {
// 	fecFactory.createPartition (*deviceCorrect, &major, &minor, structureName, partitionName) ;
//       }

//       // Set the database as the version to be downloaded (current state)
//       fecFactory.setFecDevicePartitionVersion ( partitionName, major, minor ) ;
      
//       std::cout << "-------------------------- Version ---------------------" << std::endl ;
//       std::cout << "Upload the device version " << std::dec << major << "." << minor << " in the database" << std::endl ;
//       if (piamajor || piaminor)
// 	std::cout << "Upload the PIA version " << std::dec << piamajor << "." << piaminor << " in the database" << std::endl ;
//       std::cout << "--------------------------------------------------------" << std::endl ;
//     }
//     catch (FecExceptionHandler &e) {
      
//       std::cerr << "*********** ERROR *************" << std::endl ;
//       std::cerr << "XML parsing for database error " << e.what() << std::endl ;
//       std::cerr << "*******************************" << std::endl ;
//     }
//     catch (oracle::occi::SQLException &e) {
      
//       std::cerr << "*********** ERROR *************" << std::endl ;
//       std::cerr << "Unable to connect the database " << login << "@" << path << std::endl ;
//       std::cerr << e.what() << std::endl ;
//       std::cerr << "*******************************" << std::endl ;
//     }
//   }
//   else 
// #endif
//     {
//       try {
// 	// Example to create a new major version with the device uploaded 
// 	// and input file it is set
// 	std::cout << "-------------- Upload it in file " << outputfile << std::endl ;
// 	FecFactory fecFactory ; 
// 	fecFactory.setUsingFile() ;
// 	fecFactory.setOutputFileName (outputfile) ;
// 	std::cout << "Upload in file: " << fecFactory.getOutputFileName ( )  << std::endl ; 
// 	if (! vPiaReset.empty())
// 	  fecFactory.setFecDevicePiaDescriptions ( *deviceCorrect, vPiaReset ) ;
// 	else
// 	  fecFactory.setFecDeviceDescriptions (*deviceCorrect) ;
//       }
//       catch (FecExceptionHandler &e) {
// 	std::cerr << "*********** ERROR *************" << std::endl ;
// 	std::cerr << "XML upload in file error " << e.what() << std::endl ;
// 	std::cerr << "*******************************" << std::endl ;
//       }
//     }

//   try {
//     // Delete PIA reset
//     FecFactory::deleteVector (vPiaReset) ;
//     delete apvIn ; delete muxIn ; delete pllIn ; delete laserIn ; delete dohIn ;  
//     FecFactory::deleteVector(vDevices) ;
//     if (deviceCorrect != &vDevices) delete deviceCorrect ;
//   }
//   catch (FecExceptionHandler &e) {
    
//     std::cerr << "*********** ERROR ********************************" << std::endl ; 
//     std::cerr << "An error occurs during hardware access" << std::endl ;
//     std::cerr << e.what()  << std::endl ;
//     std::cerr << "**************************************************" << std::endl ;
//   }
}


/** Retreive the CCU parameters from the input and upload it in the output
 * command: -uploadDcu
 */
void uploadDcu ( char *inputfile, char *outputfile, std::string partitionName, unsigned int dcuHardId, unsigned long timeStampStart, unsigned long timeStampStop ) {

  FecFactory *fecFactoryInput = NULL ;
  deviceVector vDeviceInput ;
  piaResetVector vPiaResetInput ;
  std::string login="nil", passwd="nil", path="nil" ;

  // read the file or database and get the devices
  try {

#ifdef DATABASE

    DbAccess::getDbConfiguration (login, passwd, path) ;
    if ((strncasecmp (inputfile, "database", 8) == 0) && (login == "nil"))
      std::cerr << "The login, password, path is not set cannot upload in database" << std::endl ;

    if ((strncasecmp (inputfile, "database", 8) == 0) && (login != "nil") ) {
      
      // Database connection
      std::cout << "Create a database access for " << login << "/" << passwd << "@" << path << " for the input" << std::endl ;
      fecFactoryInput = new FecFactory(login, passwd, path) ;
      
      if ( (partitionName != "nil") && (partitionName != "") ) {
	std::cout << "Retreive DCU values from " << timeStampStart << " to " << timeStampStop << " for partition " << partitionName << std::endl ;
	fecFactoryInput->getDcuValuesDescriptions (partitionName, vDeviceInput, timeStampStart, timeStampStop) ;
      }
      else if ( dcuHardId != 0) {
	std::cout << "Retreive DCU values for DCU " << std::dec << dcuHardId << " from " << timeStampStart << " to " << timeStampStop << " for partition " << partitionName << std::endl ;
	fecFactoryInput->getDcuValuesDescriptions (dcuHardId, vDeviceInput, timeStampStart, timeStampStop ) ;
      }
      else {
	std::cout << "Retreive DCU values from " << timeStampStart << " to " << timeStampStop << std::endl ;
	fecFactoryInput->getDcuValuesDescriptions (vDeviceInput, timeStampStart, timeStampStop) ;
      }
    }
    else
#endif
      {
	fecFactoryInput = new FecFactory( ) ;
	fecFactoryInput->setInputFileName (inputfile) ;
	fecFactoryInput->getDcuValuesDescriptions (vDeviceInput, timeStampStart, timeStampStop ) ;
      }

    // display it
    FecFactory::display (vDeviceInput) ;

#ifdef DATABASE
    // Upload in database or file
    login="nil", passwd="nil", path="nil" ;
    DbFecAccess::getDbConfiguration (login, passwd, path) ;
    if ((strncasecmp (outputfile, "database", 8) == 0) && (login == "nil"))
      std::cerr << "The login, password, path is not set cannot upload in database" << std::endl ;
    
    if ((strncasecmp (outputfile, "database", 8) == 0) && (login != "nil")) {
      
      // Database connection
      std::cout << "Create a database access for " << login << "/" << passwd << "@" << path << " for the upload" << std::endl ;
      FecFactory fecFactory (login, passwd, path) ;
      
      //unsigned int major, minor ;
      // Upload the devices
      fecFactory.setFecDeviceDescriptions (vDeviceInput) ;
    }
    else
#endif
      {
	std::cout << "Upload in file " << outputfile << " done" << std::endl ;

	// Upload in output file
	FecFactory fecFactory ;
	fecFactory.setUsingFile() ;
	fecFactory.setOutputFileName (outputfile) ;
	fecFactory.setFecDeviceDescriptions (vDeviceInput);
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

 if (fecFactoryInput != NULL) delete fecFactoryInput ;
}

/** Retreive the CCU parameters from the input and upload it in the output
 * command: -uploadFileDcu
 */
void uploadFileDcu ( char *inputfile, char *outputfile, std::string partitionName, unsigned long timeStampStart, unsigned long timeStampStop ) {

  FecFactory *fecFactoryInput = NULL ;
  deviceVector vDeviceInput ;
  piaResetVector vPiaResetInput ;
  std::string login="nil", passwd="nil", path="nil" ;

  // read the file or database and get the devices
  try {

#ifdef DATABASE

    DbFecAccess::getDbConfiguration (login, passwd, path) ;
    if ((strncasecmp (inputfile, "database", 8) == 0) && (login == "nil"))
      std::cerr << "The login, password, path is not set cannot upload in database" << std::endl ;

    if ((strncasecmp (inputfile, "database", 8) == 0) && (login != "nil") ) {
      
      // Database connection
      std::cout << "Create a database access for " << login << "/" << passwd << "@" << path << " for the input" << std::endl ;
      fecFactoryInput = new FecFactory(login, passwd, path) ;

      std::cout << "Retreive DCU values from " << timeStampStart << " to " << timeStampStop << std::endl ;
      if ( (partitionName != "nil") && (partitionName != "") )
	fecFactoryInput->getDcuValuesDescriptions ( partitionName, vDeviceInput, timeStampStart, timeStampStop) ;
      else
	fecFactoryInput->getDcuValuesDescriptions ( vDeviceInput, timeStampStart, timeStampStop ) ;
    }
    else
#endif
      {
	fecFactoryInput = new FecFactory( ) ;
	fecFactoryInput->setInputFileName (inputfile) ;
	fecFactoryInput->getDcuValuesDescriptions ( vDeviceInput, timeStampStart, timeStampStop ) ;
      }

    // display it
    // FecFactory::display (vDeviceInput) ;

    // Upload it to a file with the format:
    // DcuHardId Timestamp <Temperature from channel 0> <Temperature from channel 4> <Temperature from channel 7>
    std::ofstream fileOutStream(outputfile);
    if (! fileOutStream.is_open()) {
      std::cerr << "Cannot open the file " << outputfile << std::endl ;
      return ;
    }

    fileOutStream << "# FEC_index DCU_Hard_Id\tTimestamp\tChannel0\tChannel1\tChannel2\tChannel3\tChannel4\tChannel5\tChannel6\tChannel7" << std::endl ;
    for (deviceVector::iterator device = vDeviceInput.begin() ; device != vDeviceInput.end() ; device ++) {

      dcuDescription *deviced = (dcuDescription *)*device ;

      fileOutStream << toString(deviced->getKey()) << "\t" ;
      fileOutStream << toString(deviced->getDcuHardId()) << "\t" << toString(deviced->getTimeStamp()) << "\t" ;
      fileOutStream << toString(deviced->getDcuChannel0()) << "\t" ;
      fileOutStream << toString(deviced->getDcuChannel1()) << "\t" ;
      fileOutStream << toString(deviced->getDcuChannel2()) << "\t" ;
      fileOutStream << toString(deviced->getDcuChannel3()) << "\t" ;
      fileOutStream << toString(deviced->getDcuChannel4()) << "\t" ;
      fileOutStream << toString(deviced->getDcuChannel5()) << "\t" ;
      fileOutStream << toString(deviced->getDcuChannel6()) << "\t" ;
      fileOutStream << toString(deviced->getDcuChannel7()) << "\t" ;
      fileOutStream << std::endl ;
    }

    fileOutStream.flush();
    fileOutStream.close();
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

 if (fecFactoryInput != NULL) delete fecFactoryInput ;
}

/** Test performance only => not documented
 * -testPerf
 */
void testPerformance ( FecAccess *fecAccess,
		       FecAccessManager *accessManager, 
		       char *filename ) {

  //fecAccess->setForceAcknowledge (false) ;
  //fecAccess->seti2cChannelSpeed (100) ;

  FecFactory fecFactory ;
  fecFactory.setUsingFile() ;
  
  try {

    // Parse a file for PIA reset
    std::cout << "-------------- Parse the file " << filename << " for PIA reset" << std::endl ;
    fecFactory.setInputFileName (filename) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot parse the file " << filename << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Vector of PIA reset descriptions
  piaResetVector vPiaReset ;

  try {
    fecAccess->fecHardReset() ;
    fecAccess->fecRingReset() ;

    // Retreive the values this vector must not be deleted, it is up to the FecFactory
    fecFactory.getPiaResetDescriptions ( vPiaReset ) ;

    // Create the access and reset the modules
    if (! vPiaReset.empty()) {

      std::string message ;
      std::list<FecExceptionHandler *> errorListPia ;
      if (!multiFrames_) {
	std::cout << "-------------- Reset the modules througth the PIA channels" << std::endl ;
	message = "Error during Reset the modules througth the PIA channels" ;
	accessManager->resetPiaModules (&vPiaReset, errorListPia) ;
      }
      else {
	std::cout << "-------------- Reset the modules througth the PIA channels (multiple frames)" << std::endl ;
	message = "Error during Reset the modules througth the PIA channels (multiple frames)" ;
	accessManager->resetPiaModulesMultipleFrames (&vPiaReset, errorListPia) ;
      }
      displayErrorMessage (message, errorListPia) ;
      //FecFactory::deleteVector (vPiaReset) ;
    }
    else {
      std::cerr << "-------------- No PIA reset tags found in file " << filename << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot send a PIA reset" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Retreive the devices from file
  // vector of device descriptions
  deviceVector devices ;
  try {
    // Retreive the devices
    std::cout << "-------------- Parse the file " << filename << std::endl ;
    
    // Parse a file for devices
    fecFactory.getFecDeviceDescriptions( devices ) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot read the file " << filename << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  std::list<FecExceptionHandler *> errorListDevice ;

  // Here start the test for download or upload
  // int gettimeofday(struct timeval *tv, struct timezone *tz);
  unsigned long startTime = time(NULL) ;
  std::cerr << "Start time = " << std::dec << startTime << std::endl ;
  unsigned long value = 0 ;
#define TESTNUMBER 100
  for (int i = 0 ; i < TESTNUMBER ; i ++) {

    try {
      unsigned int error = 0 ;
      
      if (!multiFrames_) {
	error = accessManager->downloadValues (&devices, errorListDevice) ;
      }
      else {
	error = accessManager->downloadValuesMultipleFrames (&devices, errorListDevice) ;
      }
    }
    catch (FecExceptionHandler &e) {

      std::cerr << "*********** ERROR ********************************" << std::endl ; 
      std::cerr << "An error occurs during hardware access" << std::endl ;
      std::cerr << e.what()  << std::endl ;
      std::cerr << "**************************************************" << std::endl ;

      return ;
    }
  }

  std::cout << value << std::endl ;
  unsigned long stopTime = time(NULL) ;
  std::cerr << "Stop time = " << std::dec << stopTime << std::endl ;
  std::cerr << "The test takes " << std::dec << (stopTime-startTime) 
	    << " seconds for " << TESTNUMBER << " downloads" << std::endl ;

  // Display the errors
  displayErrorMessage ("Error during the Download of devices", errorListDevice) ;
}

/** Test performance only => not documented
 * -testTiming
 */
void testTiming ( FecAccess *fecAccess,
		  FecAccessManager *accessManager, 
		  char *filename ) {

  //fecAccess->setForceAcknowledge (false) ;
  //fecAccess->seti2cChannelSpeed (100) ;

  FecFactory fecFactory ;
  fecFactory.setUsingFile() ;
  
  try {
    // Parse a file for PIA reset
    std::cout << "-------------- Parse the file " << filename << " for PIA reset" << std::endl ;
    fecFactory.setInputFileName (filename) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot parse the file " << filename << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Vector of PIA reset descriptions
  piaResetVector vPiaReset ;

  try {
    fecAccess->fecHardReset() ;
    fecAccess->fecRingReset() ;

    // Retreive the values this vector must not be deleted, it is up to the FecFactory
    fecFactory.getPiaResetDescriptions ( vPiaReset ) ;

    // Create the access and reset the modules
    if (! vPiaReset.empty()) {

      std::string message ;
      std::list<FecExceptionHandler *> errorListPia ;
      if (!multiFrames_) {
	std::cout << "-------------- Reset the modules througth the PIA channels" << std::endl ;
	message = "Error during Reset the modules througth the PIA channels" ;
	accessManager->resetPiaModules (&vPiaReset, errorListPia) ;
      }
      else {
	std::cout << "-------------- Reset the modules througth the PIA channels (multiple frames)" << std::endl ;
	message = "Error during Reset the modules througth the PIA channels (multiple frames)" ;
	accessManager->resetPiaModulesMultipleFrames (&vPiaReset, errorListPia) ;
      }
      displayErrorMessage (message, errorListPia) ;
      //FecFactory::deleteVector (vPiaReset) ;
    }
    else {
      std::cerr << "-------------- No PIA reset tags found in file " << filename << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot send a PIA reset" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Retreive the devices from file
  // vector of device descriptions
  deviceVector devices ;
  try {
    // Retreive the devices
    std::cout << "-------------- Parse the file " << filename << std::endl ;
    
    // Parse a file for devices
    fecFactory.getFecDeviceDescriptions( devices ) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot read the file " << filename << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Here start the test for download or upload
  unsigned long startTime = time(NULL) ;
  unsigned long stopTime ;
  std::cerr << "Start time = " << std::dec << startTime << std::endl ;
#define TESTNUMBERTIMING 1
  for (int i = 0 ; i < TESTNUMBERTIMING ; i ++) {

    try {

      if (!multiFrames_) {
	// Create the access
	std::list<FecExceptionHandler *> errorListDownload ;
	unsigned int error = accessManager->downloadValues (&devices, errorListDownload) ;
	std::cout << "Found " << std::dec << error << " errors in the creation of the access" << std::endl ;

	// Retreive the different devices
	deviceVector vDeviceApv, vDeviceMux, vDevicePll, vDeviceAoh ;
	vDeviceApv = FecFactory::getDeviceFromDeviceVector(devices,APV25) ;
	vDeviceMux = FecFactory::getDeviceFromDeviceVector(devices,APVMUX) ;
	vDevicePll = FecFactory::getDeviceFromDeviceVector(devices,PLL) ;
	vDeviceAoh = FecFactory::getDeviceFromDeviceVector(devices,LASERDRIVER) ;

	// for each timeout
#define MAXTIMEOUT 100
	for (unsigned int timeout = 11 ; timeout <= MAXTIMEOUT ; timeout++) {
	  
	  // set the timeout
	  //fecAccess->setLoopInTimeWriteFrame(timeout) ;
	  //fecAccess->setLoopInTimeDirectAck(timeout) ;
	  fecAccess->setLoopInTimeReadFrame(timeout) ;

	  unsigned int errorWrite, errorRead ;

	  // ------------------------------------------------------------------------------
	  // APV
	  errorWrite = errorRead = 0 ;
	  // Download the devices
	  // Take each device and download it
	  unsigned long startTime = time(NULL) ;
	  for (deviceVector::iterator itDevice = vDeviceApv.begin() ; itDevice != vDeviceApv.end() ; itDevice ++) {
	    apvDescription *deviceD = (apvDescription *)(*itDevice) ;
	    try {
	      apvAccess *device = (apvAccess *)accessManager->getAccess(APV25,(*itDevice)->getKey());
	      try {
		device->setValues(*deviceD) ;
	      }
	      catch (FecExceptionHandler &e) {
		errorWrite++ ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "ERROR: No access created for the device: " << e.what() << std::endl ;
	    }
	  }
	  stopTime = time(NULL) ;
	  std::cout << "Download of " << std::dec << vDeviceApv.size() << " APV gives " << errorWrite 
		    << " errors with timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;

	  // Upload the devices
	  errorWrite = errorRead = 0 ;
	  // Take each device and download it
	  startTime = time(NULL) ;
	  for (deviceVector::iterator itDevice = vDeviceApv.begin() ; itDevice != vDeviceApv.end() ; itDevice ++) {
	    apvDescription *deviceD = (apvDescription *)(*itDevice) ;
	    try {
	      apvAccess *device = (apvAccess *)accessManager->getAccess(APV25,(*itDevice)->getKey()) ;
	      try {
		deviceD = device->getValues() ;
		delete deviceD ;
	      }
	      catch (FecExceptionHandler &e) {
		errorRead++ ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "ERROR: No access created for the device: " << e.what() << std::endl ;
	    }
	  }

	  stopTime = time(NULL) ;
	  std::cout << "Upload of " << std::dec << vDeviceApv.size() << " APV gives " << errorRead
		    << " errors with timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;
	    
	  // ------------------------------------------------------------------------------
	  // APV MUX
	  errorWrite = errorRead = 0 ;
	  // Download the devices
	  // Take each device and download it
	  startTime = time(NULL) ;
	  for (deviceVector::iterator itDevice = vDeviceMux.begin() ; itDevice != vDeviceMux.end() ; itDevice ++) {
	    muxDescription *deviceD = (muxDescription *)(*itDevice) ;
	    try {
	      muxAccess *device = (muxAccess *)accessManager->getAccess(APVMUX,(*itDevice)->getKey()) ;
	      try {
		device->setValues(*deviceD) ;
	      }
	      catch (FecExceptionHandler &e) {
		errorWrite++ ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "ERROR: No access created for the device: " << e.what() << std::endl ;
	    }
	  }
	  stopTime = time(NULL) ;
	  std::cout << "Download of " << std::dec << vDeviceMux.size() << " APV MUX gives " << errorWrite 
		    << " errors with timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;

	  // Upload the devices
	  errorWrite = errorRead = 0 ;
	  // Take each device and download it
	  startTime = time(NULL) ;
	  for (deviceVector::iterator itDevice = vDeviceMux.begin() ; itDevice != vDeviceMux.end() ; itDevice ++) {
	    muxDescription *deviceD = (muxDescription *)(*itDevice) ;
	    try {
	      muxAccess *device = (muxAccess *)accessManager->getAccess(APVMUX,(*itDevice)->getKey()) ;
	      try {
		deviceD = device->getValues() ;
		delete deviceD ;
	      }
	      catch (FecExceptionHandler &e) {
		errorRead++ ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "ERROR: No access created for the device: " << e.what() << std::endl ;
	    }
	  }
	  stopTime = time(NULL) ;
	  std::cout << "Upload of " << std::dec << vDeviceMux.size() << " APV MUX gives " << errorRead 
		    << " errors with timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;

	  // ------------------------------------------------------------------------------
	  // PLL
	  errorWrite = errorRead = 0 ;
	  // Download the devices
	  errorWrite = errorRead = 0 ;
	  // Take each device and download it
	  startTime = time(NULL) ;
	  for (deviceVector::iterator itDevice = vDevicePll.begin() ; itDevice != vDevicePll.end() ; itDevice ++) {
	    pllDescription *deviceD = (pllDescription *)(*itDevice) ;
	    try {
	      pllAccess *device = (pllAccess *)accessManager->getAccess(PLL,(*itDevice)->getKey()) ;
	      try {
		device->setValues(*deviceD) ;
	      }
	      catch (FecExceptionHandler &e) {
		errorWrite++ ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "ERROR: No access created for the device: " << e.what() << std::endl ;
	    }
	  }
	  stopTime = time(NULL) ;
	  std::cout << "Download of " << std::dec << vDevicePll.size() << " PLL gives " << errorWrite 
		    << " errors with timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;
	      
	  // Upload the devices
	  errorWrite = errorRead = 0 ;
	  // Take each device and download it
	  startTime = time(NULL) ;
	  for (deviceVector::iterator itDevice = vDevicePll.begin() ; itDevice != vDevicePll.end() ; itDevice ++) {
	    pllDescription *deviceD = (pllDescription *)(*itDevice) ;
	    try {
	      pllAccess *device = (pllAccess *)accessManager->getAccess(PLL,(*itDevice)->getKey()) ;
	      try {
		deviceD = device->getValues() ;
		delete deviceD ;
	      }
	      catch (FecExceptionHandler &e) {
		errorRead++ ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "ERROR: No access created for the device: " << e.what() << std::endl ;
	    }
	  }
	  stopTime = time(NULL) ;
	  std::cout << "Upload of " << std::dec << vDevicePll.size() << " PLL gives " << errorRead 
		    << " errors with timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;

	  // ------------------------------------------------------------------------------
	  // AOH
	  errorWrite = errorRead = 0 ;
	  // Download the devices
	  errorWrite = errorRead = 0 ;
	  // Take each device and download it
	  startTime = time(NULL) ;
	  for (deviceVector::iterator itDevice = vDeviceAoh.begin() ; itDevice != vDeviceAoh.end() ; itDevice ++) {
	    laserdriverDescription *deviceD = (laserdriverDescription *)(*itDevice) ;
	    try {
	      laserdriverAccess *device = (laserdriverAccess *)accessManager->getAccess(LASERDRIVER,(*itDevice)->getKey()) ;
	      try {
		device->setValues(*deviceD) ;
	      }
	      catch (FecExceptionHandler &e) {
		errorWrite++ ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "ERROR: No access created for the device: " << e.what() << std::endl ;
	    }
	  }
	  stopTime = time(NULL) ;
	  std::cout << "Download of " << std::dec << vDeviceAoh.size() << " AOH gives " << errorWrite 
		    << " errors with timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;

	  // Upload the devices
	  errorWrite = errorRead = 0 ;
	  // Take each device and download it
	  startTime = time(NULL) ;
	  for (deviceVector::iterator itDevice = vDeviceAoh.begin() ; itDevice != vDeviceAoh.end() ; itDevice ++) {
	    laserdriverDescription *deviceD = (laserdriverDescription *)(*itDevice) ;
	    try {
	      laserdriverAccess *device = (laserdriverAccess *)accessManager->getAccess(LASERDRIVER,(*itDevice)->getKey()) ;
	      try {
		deviceD = device->getValues() ;
		delete deviceD ;
	      }
	      catch (FecExceptionHandler &e) {
		errorRead++ ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "ERROR: No access created for the device: " << e.what() << std::endl ;
	    }
	  }
	  stopTime = time(NULL) ;
	  std::cout << "Upload of " << std::dec << vDeviceAoh.size() << " AOH gives " << errorRead 
		    << " errors with timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;
 	}
      }
      else {

	// for each timeout
	for (unsigned int timeout = 1 ; timeout <= MAXTIMEOUT ; timeout++) {

	  // set the timeout
	  //fecAccess->setLoopInTimeWriteFrame(timeout) ;
	  //fecAccess->setLoopInTimeDirectAck(timeout) ;
	  fecAccess->setLoopInTimeReadFrame(timeout) ;

	  std::list<FecExceptionHandler *> errorListDownload, errorListUpload ;
	  unsigned long startTime = time(NULL) ;
	  unsigned int error = accessManager->downloadValuesMultipleFrames (&devices, errorListDownload) ;
	  unsigned long stopTime = time(NULL) ;
	  std::cout << "Download of all devices in mutiple frames gives " << std::dec << error 
		    << "/" << errorListDownload.size() << " errors" << " for the timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;
	  startTime = time(NULL) ;
	  deviceVector *vDevice = accessManager->uploadValuesMultipleFrames(errorListUpload) ;
	  stopTime = time(NULL) ;
	  std::cout << "Upload of all devices in mutiple frames gives " << std::dec << error 
		    << "/" << errorListDownload.size() << " errors" << " for the timeout " << timeout 
		    << " in " << (stopTime-startTime) << " seconds" << std::endl ;
	  FecFactory::deleteVectorI(*vDevice) ;
	  delete vDevice ;
	}
      }
    }
    catch (FecExceptionHandler &e) {

      std::cerr << "*********** ERROR ********************************" << std::endl ; 
      std::cerr << "An error occurs during hardware access" << std::endl ;
      std::cerr << e.what()  << std::endl ;
      std::cerr << "**************************************************" << std::endl ;

      return ;
    }
  }
}


/** Parse the database version and download the parameters parsed in the 
 * hardware
 * <p>command: -database
 * \param accessManager - FecAccessManager: device access manager
 * \param outputfile - output file for the upload
 * \param versionMajor - major version
 * \param versionMinor - minor version
 */
void downloadUploadFromDatabase ( FecAccessManager *accessManager,
				  std::string partitionName,
				  std::string fecHardwareId,
				  char *outputfile,
				  unsigned int versionMajor, 
				  unsigned int versionMinor) {

#ifdef DATABASE

  // Upload in database or file
  std::string login="nil", passwd="nil", path="nil" ;
  DbFecAccess::getDbConfiguration (login, passwd, path) ;
  if (login == "nil") {

    std::cerr << "The login, password, path is not set cannot upload in database" << std::endl ;    
    return ;
  }

  if (partitionName == "nil" || partitionName == "") {

    std::cerr << "You need to specify a partition for the request" << std::endl ;
    return ;
  }

  try {

    // Create the FEC device factory
    FecFactory fecFactory (login, passwd, path) ;

    // Get the current version
    unsigned int devMajor, devMinor, maskMajor, maskMinor, partitionNumber ;
    fecFactory.getFecDevicePartitionVersion (partitionName,&devMajor,&devMinor,&maskMajor,&maskMinor,&partitionNumber) ;
    
    if (partitionNumber == 0) {
      
      std::cerr << "******************************** WARNING *****************************" << std::endl ;
      std::cerr << "No FEC devices found for partition " << partitionName << std::endl ;
      std::cerr << "**********************************************************************" << std::endl ;
      return ;
    }
    else {

      // Retreive the devices
      if (versionMajor != 0 || versionMinor != 0) 
	std::cout << "Retreive the devices for version " << std::dec << versionMajor << "." << versionMinor << " (mask = " << maskMajor << "." << maskMinor << ")" << std::endl ;

      std::cout << "\tPartition " << fecFactory.getPartitionName(partitionNumber) << " (" << partitionNumber << ")" << ": Version " << devMajor << "." << devMinor << std::endl ;
      
      // Retreive the values for PIA for the current version
      piaResetVector vPiaReset ;
      if (fecHardwareId != "0") 
	fecFactory.getPiaResetDescriptions(partitionName,fecHardwareId,vPiaReset) ;
      else
	fecFactory.getPiaResetDescriptions(partitionName,vPiaReset) ;

      if (! vPiaReset.empty()) {

	// Display
	FecFactory::display(vPiaReset) ;

        // Hardware reset
	unsigned int error = 0 ;
	std::string message ;
	std::list<FecExceptionHandler *> errorListPia ;
	if (!multiFrames_) {
	  std::cout << "-------------- Reset the modules througth the PIA channels" << std::endl ;
	  message = "Error during Reset the modules througth the PIA channels" ;
	  error = accessManager->resetPiaModules ( &vPiaReset, errorListPia ) ;
	}
	else {
	  std::cout << "-------------- Reset the modules througth the PIA channels (multiple frames)" << std::endl ;
	  message = "Error during Reset the modules througth the PIA channels (multiple frames)" ;
	  error = accessManager->resetPiaModulesMultipleFrames ( &vPiaReset, errorListPia ) ;
	}
	displayErrorMessage (message, errorListPia) ;

        if (error) {
	  
          std::cerr << "******************** ERROR *********************" << std::endl ;
          std::cerr << error << " errors during the download on the PIA" << std::endl ;
          std::cerr << "***********************************************" << std::endl ;
        }
      }
      else {
        std::cerr << "****************** WARNING *********************" << std::endl ;
        std::cerr << "No PIA tags found in database" << std::endl ;
        std::cerr << "***********************************************" << std::endl ;
      }

      deviceVector vDevice ;
      if (fecHardwareId != "0") 
	fecFactory.getFecDeviceDescriptions (partitionName, fecHardwareId, vDevice, versionMajor, versionMinor) ;
      else 
	fecFactory.getFecDeviceDescriptions (partitionName, vDevice, versionMajor, versionMinor) ;
      
      // Create the access and reset the modules
      if (! vDevice.empty()) {

	// display
        FecFactory::display (vDevice) ;
	
        // Download in the hardware
	unsigned int error = 0 ;
	std::list<FecExceptionHandler *> errorListDevice ;
	if (!multiFrames_)
	  error = accessManager->downloadValues (&vDevice, errorListDevice) ;
	else
	  error = accessManager->downloadValuesMultipleFrames (&vDevice, errorListDevice) ;
	// Display the errors
	displayErrorMessage ("Error during the Download of devices", errorListDevice) ;

	// Upload the devices from the hardware to the database
	deviceVector *deviceUploaded = NULL ;
	std::list<FecExceptionHandler *> errorListDeviceUpload ;
	if (!multiFrames_) {
	  std::cout << "-------------- Upload the devices connected" << std::endl ;
	  deviceUploaded = accessManager->uploadValues ( errorListDeviceUpload ) ;
	}
	else {
	  std::cout << "-------------- Upload the devices connected through multiple frames" << std::endl ;
	  deviceUploaded = accessManager->uploadValuesMultipleFrames ( errorListDeviceUpload ) ;
	}
	// Display the errors
	displayErrorMessage ("Error during the upload of the devices", errorListDeviceUpload) ;

	unsigned int major = 0, minor = 0 ;
	// Note that FEC hardware ID are embedded in the descriptions
	fecFactory.setFecDeviceDescriptions (*deviceUploaded, partitionName, &major, &minor, false, true) ;

	// display it
	// FecDetectionUpload::checkDevices (*deviceUploaded, "UNKNOWN") ;
	FecFactory::deleteVectorI (*deviceUploaded) ;
	delete deviceUploaded ;
        
        // Error check
        if (error) {
	  
          if (error >= accessManager->getMaxErrorAllowed()) {
            
            std::cerr << "********************************************************************" << std::endl ;
            std::cerr << "Too many errors (" << std::dec << error << "), the download must be redone" << std::endl ;
            std::cerr << "Halt and Configure state for the FecSupervisor (check documentation)" << std::endl ;
            std::cerr << "The FECs will be reseted (PLX)" << std::endl ;
            std::cerr << "********************************************************************" << std::endl ;
          }
          else {
            std::cerr << "************************** ERROR ***********************" << std::endl ;
            std::cerr << "Error during parsing or downloading (" << error << " error(s)), check previous error" << std::endl ;
            std::cerr << "********************************************************" << std::endl ;
          }
        }
        else {
          std::cout << "Parsing and downloading done successfully" << std::endl ;
        }
      }
      else {
        std::cerr << "****************** WARNING *********************" << std::endl ;
        std::cerr << "No device found in database" << std::endl ;
        std::cerr << "***********************************************" << std::endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Error during the download from database" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    
    std::cerr << "*********** ERROR *************" << std::endl ; 
    std::cerr << "Unable to connect the database " << login << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ; 
    std::cerr << "*******************************" << std::endl ;
  }
#endif
}

/** Readout the DCU and display the values converted if a conversion file is specified or a database
 * <p>command: -dcuConversion -dcuReadout
 * \param accessManager - FecAccessManager: device access manager
 * \param filename - file to be parsed for conversion factors
 * \param partitionName - partition name
 * \param output - output directory
 */
int dcuConversion ( FecAccessManager *accessManager,
		    std::string fileName,
		    std::string partitionName,
		    std::string outputDirectory ) {

  TkDcuConversionFactory *tkDcuConversionFactory = NULL ;
  std::string login="nil", passwd="nil", path="nil" ;

  try {
    accessManager->getFecAccess()->fecHardReset() ;
    accessManager->getFecAccess()->fecRingReset() ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************************************************" << std::endl ;
    std::cerr << "Cannot reset the FECs" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "*************************************************" << std::endl ;

    return -1 ;
  }

  // Extract the conversion factors
  try {
#ifdef DATABASE
    if (fileName == "database") {

      std::cout << "Create a database access for " << login << "/" << passwd << "@" << path << std::endl ;
      DbAccess::getDbConfiguration (login, passwd, path) ;

      if (login == "nil" || passwd=="nil" || path=="nil") {
      
	std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
	std::cerr << "Exiting ..." << std::endl ;
	return -1 ;
      }

      tkDcuConversionFactory = new TkDcuConversionFactory (login, passwd, path) ;
    }
    else 
#endif
      if (fileName != "nil") {
	std::cout << "Parse the file " << fileName << " for the conversion factors" << std::endl ;
	tkDcuConversionFactory = new TkDcuConversionFactory ( ) ;
	tkDcuConversionFactory->setInputFileName (fileName) ;

	std::cout << "Conversion factors retreived" << std::endl ;
      }
    
    std::cout << "Press enter to continue" ; getchar() ;

    int sizeValues = 1 ;
    keyType
    deviceValues[1][2] = { 
      {DCUADDRESS, NORMALMODE} // DCU
    } ;

    std::cout << "Scan all FEC for all CCU for all DCUs" << std::endl ;
    std::list<keyType> *deviceList = accessManager->getFecAccess()->scanRingForI2CDevice ( (keyType *)deviceValues, sizeValues ) ;
    
    if (deviceList == NULL) {

      if (fecSlot_ == -1) 
	std::cout << "No devices found, cannot continue" << std::endl ;
      else
	std::cout << "No devices found on FEC " << std::dec << fecSlot_ << ", cannot continue" << std::endl ;
      return -1 ;
    }
    else {
      std::map<keyType, unsigned int> dcuDevicesLists ;
      for (std::list<keyType>::iterator it = deviceList->begin() ; it != deviceList->end() ; it ++) {
	keyType index = buildFecRingKey(getFecKey(*it),getRingKey(*it)) ;
	dcuDevicesLists[index] ++ ;
      }
      for (std::map<keyType, unsigned int>::iterator it = dcuDevicesLists.begin() ; it != dcuDevicesLists.end() ; it ++) {
	std::cerr << std::dec << "FEC " << getFecKey(it->first) << "." << getRingKey(it->first) << ": " << it->second << " DCUs" << std::endl ;
      }
    }

    // Retreive all the devices from the parsing class
    deviceVector *vDevice = new deviceVector ( ) ;
    deviceVector *dcuWithConversion = new deviceVector ( ) ;

    for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) {
      
      keyType index = *p ;
      dcuDescription *dcud = NULL ;
      
      switch (getAddressKey(index)) {
      case DCUADDRESS:     // ------------- DCU
	dcud = new dcuDescription(index) ;
	// Insert it in the vector vDevice
	try {
	  if (accessManager->getFecAccess()->getFecBusType() == FECVME) dcud->setFecHardwareId(accessManager->getFecAccess()->getFecHardwareId(index),1) ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "Unable to read the FEC hardware ID, do not affect the readout of the DCUs" << std::endl ;
	}
	vDevice->push_back ( dcud ) ;
	if (fileName == "nil") dcuWithConversion->push_back( dcud->clone() ) ;
	else {
	  TkDcuConversionFactors *conversionFactors = NULL ;
	  try {
	    conversionFactors = tkDcuConversionFactory->getTkDcuConversionFactors ( dcud->getDcuHardId() ) ;
	    if (conversionFactors != NULL)
	      dcuWithConversion->push_back( dcud->clone() ) ;
	  }
	  catch (FecExceptionHandler &e) {
	    std::cerr << "No conversion factors for the DCU " << dcud->getDcuHardId() << std::endl ; //": " << e.what() << std::endl ;
	  }
	  break ;
	}
      }
    }
    // delete the list
    delete deviceList ;

    // if some DCU has no conversion factors
    if ( (vDevice->size() > 0) && (vDevice->size() != dcuWithConversion->size()) ) {

      //getchar() ; std::cout << "Press <enter> to continue" << std::endl ;
      int removeDCU = 3 ;
      do {
	std::cout << "Some of the DCUs has no conversion factors, do you want to read it anyway (Y/N): " ;
	char answer = getchar() ;
	if (answer == 'y' || answer == 'Y') removeDCU = 0 ;
	else if (answer == 'N' || answer == 'n') removeDCU = 1 ;
	else std::cout << "Please enter y/n or Y/N" << std::endl ;
      }
      while (removeDCU == 3) ;

      if (removeDCU == 1) {
	FecFactory::deleteVectorI (*vDevice) ;
	delete vDevice ;
	vDevice = dcuWithConversion ;
      }
      else {
	FecFactory::deleteVectorI (*dcuWithConversion) ;
	delete dcuWithConversion ;
      }
    }

    // Exiting if no DCU exists
    if (vDevice->size() == 0) {
      std::cerr << "No DCU to readout" << std::endl ;
      delete vDevice ;
      return -2 ;
    }

#ifdef DEBUGMSGERROR
    FecFactory::display (*vDevice) ;
#endif

    // Declare the DCU inside the FecAccessManager
    std::list<FecExceptionHandler *> errorListDevice ;
    if (!multiFrames_) {
      std::cout << "-------------- Download for " << std::dec << vDevice->size() << " DCU " << std::endl ;
      accessManager->downloadValues (vDevice, errorListDevice) ;
    }
    else {
      std::cout << "-------------- Download multiple frames for " << std::dec << vDevice->size() << " DCU " << std::endl ;
      accessManager->downloadValuesMultipleFrames(vDevice, errorListDevice) ;
    }
    // Display the errors
    displayErrorMessage ("Error during the declaration of the DCU", errorListDevice) ;

    // Delete the vDevice
    if (vDevice != NULL) {
      FecFactory::deleteVectorI (*vDevice) ;
      delete vDevice ;
    }

    // Upload the DCU
    deviceVector deviceUploaded ;
    do {
      FecFactory::deleteVectorI(deviceUploaded) ;
      std::list<FecExceptionHandler *> errorListDeviceUpload ;
      if (!multiFrames_) {
	std::cout << "-------------- Upload the DCU connected" << std::endl ;
	accessManager->uploadValues ( deviceUploaded, errorListDeviceUpload ) ;
      }
      else {
	std::cout << "-------------- Upload the DCU connected through multiple frames" << std::endl ;
	accessManager->uploadValuesMultipleFrames ( deviceUploaded, errorListDeviceUpload ) ;
      }
      displayErrorMessage ("Upload of the DCU", errorListDeviceUpload) ;
    
      std::cout << deviceUploaded.size() << " DCU uploaded" << std::endl ;

      // Store the raw data if the directory have been specified
      if (deviceUploaded.size() && outputDirectory.length() && outputDirectory != "nil") {
	try {
	  std::ostringstream fileName ; fileName << outputDirectory << "/DCURaw" << time(NULL) << ".xml" ;
	  FecDeviceFactory fecFactory ;
	  fecFactory.setOutputFileName(fileName.str()) ;
	  fecFactory.setDcuValuesDescriptions ("USELESS", deviceUploaded, true) ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "Unable to upload the DCU data to directory " << outputDirectory << ": " << e.what() << std::endl ;
	}
      }

#ifdef DEBUGMSGERROR
      FecFactory::display (deviceUploaded) ;
#endif

      if (fileName == "nil") {
	FecFactory::display (deviceUploaded) ;
      }
      else {
	std::cout << "Make the conversion and display the real values" << std::endl ;
      
	for (deviceVector::iterator device = deviceUploaded.begin() ; device != deviceUploaded.end() ; device ++) {
	  
	  dcuDescription *dcuD = (dcuDescription *)*device ;
	  
	  bool defaultConversionFlag = true ;
	  TkDcuConversionFactors *conversionFactors = NULL ;
	  try {
	    conversionFactors = tkDcuConversionFactory->getTkDcuConversionFactors ( dcuD->getDcuHardId() ) ;
	    defaultConversionFlag = false ;
	  }
	  catch (FecExceptionHandler &e) {
	    std::cerr << "No conversion factors for the DCU " << dcuD->getDcuHardId() << ": " << e.what() << std::endl ;
	  }
	  if (defaultConversionFlag) {
	    try {
	      conversionFactors = tkDcuConversionFactory->getTkDcuConversionFactors ( 0 ) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cerr << "No default conversion factors for the DCU " << dcuD->getDcuHardId() << ": " << e.what() << std::endl ;
	    }
	  }

	  if (conversionFactors != NULL) {

	    // Set the DCU values
	    conversionFactors->setDcuDescription (dcuD) ;
	    
	    // Then make the display
	    try {
	      conversionFactors->display(true) ;
	    }
	    catch (std::string e) {
	      std::cerr << "Error in conversion for the DCU " << dcuD->getDcuHardId() << ": " << e << std::endl ;
	    }
	  }
	}
      }
#ifdef DEBUGMSGERROR
      std::cout << "Press enter to continue" ; getchar() ;
#endif
      sleep((unsigned int)5) ;
    }
    while (true && deviceUploaded.size()) ;

    FecFactory::deleteVectorI (deviceUploaded) ;
    delete tkDcuConversionFactory ;
  }
#ifdef DATABASE
  catch (oracle::occi::SQLException &e) {
    
    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "Unable to connect the database " << login << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }
#endif
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "An error occurs: " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }

 return 0 ; 
}

/** Display the errors coming from FecAccessManager download and upload operation
 */
void displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList, Sgi::hash_map<keyType, bool> *deviceError ) {

  if (errorList.size() == 0) return ;
  
  Sgi::hash_map<keyType, bool> errorDevice ; // in order to avoid multiple display of errors
  Sgi::hash_map<unsigned int, FecExceptionHandler *> deleteException ; // to delete the exception
  
  // simple message
  // std::cout << message << ": found " << errorList.size() << " errors" << std::endl ;
  
  for (std::list<FecExceptionHandler *>::iterator p = errorList.begin() ; p != errorList.end() ; p ++) {
    
    FecExceptionHandler *e = *p ;
    unsigned long ptr = (unsigned long)e ;
    
    if (deleteException.find(ptr) == deleteException.end()) deleteException[ptr] = e ;
    
    if (e->getPositionGiven()) {
      
      if (!errorDevice[e->getHardPosition()]) {
	
	errorDevice[e->getHardPosition()] = true ;
	// Report the error
	std::cerr << "******************************** ERROR *************************" << std::endl ;
	std::cerr << e->what() << std::endl ;
	std::cerr << "****************************************************************" << std::endl ;
      }
    }
    else {
      
      // Report the error
      std::cerr << "******************************** ERROR *************************" << std::endl ;
      std::cerr << e->what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
  }
  
  // Delete the exception (not redundant)
  for (Sgi::hash_map<unsigned int, FecExceptionHandler *>::iterator p = deleteException.begin() ; p != deleteException.end() ; p ++) {
    delete p->second ;
  }
  
  // No more error
  errorList.clear() ;
}

/** Display Help
  */
void help ( char *programName) {

  std::cerr << "\t[-pci]\tPCI FEC" << std::endl ;
  std::cerr << "\t[-usb]\tUSB FEC (only for Preshower)" << std::endl ;
  std::cerr << "\t[-vmesbs | -vmecaenpci | -vmecaenusb] [filename]\tVME FEC" << std::endl ;
  std::cerr << "\t\tfilename: configuration file for the VME FEC" << std::endl ;
  std::cerr << "\t\t          by default the file used is FecSoftwareV3_0/config/FecAddressTable.dat" << std::endl ;
  std::cerr << "\t[-usb]\tUSB FEC" << std::endl ;
  std::cerr << std::endl ;

  std::cerr << "Error in usage: " << programName << " [OPTION] [COMMAND]" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << "" << "OPTIONS" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << " -intput=[FILENAME]" << std::endl ;
  std::cerr << "\t" << "Give the XML for the different devices and the parameters set will be used as default parameters" << std::endl ;
  std::cerr << " -output=[FILENAME | database]" << std::endl ;
  std::cerr << "\t" << "Set the output file for the upload." << std::endl ;
  std::cerr << "\t" << "If the output is \"database\" the upload will be done into the database" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << "COMMANDS" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << " --help: this help" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << " [XML file name]" << std::endl ;
  std::cerr << "\t" << "Parse the XML file given in arguments and download the" << std::endl ;
  std::cerr << "\t" << "retreiven values into the hardware" << std::endl ;
  std::cerr << "\t" << "If no output file is specified, the XML upload values are put in" << std::endl ;
  std::cerr << "\t" << "a default file /tmp/output.xml" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << " -database PartitionName [Version X.Y] [FEC hardware ID]" << std::endl ;
  std::cerr << "\t" << "Download and upload the values into the database" << std::endl ;
  std::cerr << "\t" << "The version used is the current version set in database" << std::endl ;
  std::cerr << "\t" << "Database must be set" << std::endl ;
  std::cerr << "\t" << "See in FecSoftware/FecHeader.linux and in" << std::endl ;
  std::cerr << "\t" << "FecSoftware/config/oracle.env for the settings" << std::endl ;
  std::cerr << "\t" << "If you don't specify any output file, the database will be used for" << std::endl ;
  std::cerr << "\t" << "the upload" << std::endl ;
  std::cerr << "\t" << "Remarks:" << std::endl ;
  std::cerr << "\t" << "\t" << "To use the FEC hardware id, you need to put a version (0.0 for default version)" << std::endl ;
  std::cerr << std::endl ;
  
  std::cerr << " -tracker" << std::endl ;
  std::cerr << "\t" << "Make an automatic detection of the Tracker devices" << std::endl ;
  std::cerr << "\t" << "and upload the values into the output file" << std::endl ;
  std::cerr << "\t" << "If the output is the \"database\" then the upload is done in a new version" << std::endl ;
  std::cerr << std::endl ;

  std::cerr << " -createPartition [PARTITION NAME]" << std::endl ;
  std::cerr << "\t" << "Make an automatic detection of the Tracker devices" << std::endl ;
  std::cerr << "\t" << "and upload the values into the output file" << std::endl ;
  std::cerr << "\t" << "If the output is the \"database\" then the upload is done" << std::endl ;
  std::cerr << "\t" << "into the database in a new partition given the name partition name." << std::endl ;
  std::cerr << std::endl ;

  std::cerr << " -uploadDcu [PARTITIONNAME | DCU HARD ID] [TIMESTAMPSTART] [TIMESTAMPSTOP]" << std::endl ;
  std::cerr << "\t" << "Upload the input into the output" << std::endl ;
  std::cerr << "\t" << "Input can be database or file" << std::endl ;
  std::cerr << "\t" << "Output can be database or file" << std::endl ;
  std::cerr << std::endl ;

  std::cerr << " -dcuReadout, please check the option -dcuConversion, same option without the input" << std::endl; 
  std::cerr << std::endl ;

  std::cerr << " -dcuConversion, <-input=DCUCONVERSIONFILE> <-output=DIRECTORY>" << std::endl ;
  std::cerr << "\t" << "Readout the DCU and convert it with the conversion factors extract from database or file" << std::endl ;
  std::cerr << "\t" << "input: DCU conversion file" ;
  std::cerr << "\t\t" << "Nothing will display the raw data, same option than -dcuReadout" << std::endl ;
  std::cerr << "\t\t" << "For file:" << std::endl ;
  char *basic = getenv ("ENV_TRACKER_DAQ") ;
  if (basic == NULL) basic=getenv ("ENV_CMS_TK_FEC_ROOT") ;
  if (basic != NULL) std::cerr << "\t\t\t" << "you can check for " << basic << "/FecSoftwareV3_0/config/DcuConversionFactors.xml" << std::endl ;
  else std::cerr << "\t\t\t" << "Generally in $ENV_CMS_TK_FEC_ROOT/FecSoftwareV3_0/config/DcuConversionFactors.xml" << std::endl ;
  std::cerr << "\t\t" << "database" << std::endl ;
  std::cerr << "\t" << "output=DIRECTORY: If you provide an output then raw data files will stored in the directory your provide" << std::endl ;
  std::cerr << std::endl ;

//   std::cerr << " -preshower [PARTITION NAME]" << std::endl ;
//   std::cerr << "\t" << "Make an automatic detection of the Preshower devices" << std::endl ;
//   std::cerr << "\t" << "and upload the values into the output file" << std::endl ;
//   std::cerr << "\t" << "If the output is the \"database\" then the upload is done" << std::endl ;
//   std::cerr << "\t" << "into the database in a new partition given the name partition name." << std::endl ;
//   std::cerr << "\t" << "Note that the partition name will complete with the current timestamp." << std::endl ;

  std::cerr << std::endl ;
}

/** Create the FEC Access class depending on the version of the FecSoftware and the FEC type
 */
FecAccess *createFecAccess ( int argc, char **argv, int *cnt ) {

  FecAccess *fecAccess = FecAccess::createFecAccess ( argc, argv, cnt, true) ;
  
  if (!fecAccess) {
    std::cerr << "Creation of FecAccess failed. fecAccess pointer null." << std::endl ; 
    exit (EXIT_FAILURE) ; ; 
  }

  // Create the FEC Access
  switch ( fecAccess->getFecBusType() ) {
  case FECUSB:
  case FECPCI:
    ringMin_ = 0 ; ringMax_ = 0 ;
    break ;
  case FECVME:
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
    ringMin_ = FecVmeRingDevice::getMinVmeFecRingValue() ;
    ringMax_ = FecVmeRingDevice::getMaxVmeFecRingValue() ;
#endif
    break ;
  }

  if ((*cnt < argc) && (strcasecmp (argv[*cnt],"-fec") == 0)) {
    *cnt = *cnt+2 ; // +1 for -fec and +1 for the FEC slot
  }
  
//   if ((*cnt < argc) && (strcasecmp (argv[*cnt],"-ring") == 0)) {
//     *cnt ++ ;
//     int ringSlot ;
//     if (! sscanf (argv[*cnt], "%d", &ringSlot)) {
//       delete fecAccess ;
//       std::cerr << "Bad FEC slot" << std::endl ;
//       //exit (EXIT_FAILURE) ;
//     }

//     ringMax_ = ringMin_ = ringSlot ;
//     *cnt ++ ;
//   }

  return (fecAccess) ;
}


/** main function
 * type FecProfiler.exe --help for help on options and commands
 */
int main ( int argc, char *argv[] ) {

  std::string fecHardwareId = "0" ;

  // outputfile for the upload
  char outputfile[1000] = "/tmp/output.xml" ;
  char inputfile[1000] = UNKNOWNFILE ;
  char partitionName[1000] = {0}, structureName[1000] = {0} ;
  bool outputfileFlag = false ;
  bool inputfileFlag = false ;

  unsigned timeStampStart = 1, timeStampStop = 999999999 ;
  unsigned int versionMajor = 0, versionMinor = 0 ;
  unsigned int dcuHardId = 0 ;

  //sprintf (partitionName, "Partition_%ld", time(NULL)) ;
  sprintf (structureName, "Structure_%ld", time(NULL)) ;

  // Hardware access to the FEC
  FecAccess *fecAccess = NULL ;
  FecAccessManager *accessManager = NULL ;

  int choix = 0 ;

  // ------------------------------------------------------------------------
  // Check the options
  if ( (argc <= 1) || !strncasecmp (argv[1], "-help", 5) || !strncasecmp (argv[1], "--help", 6)) {

    help( argv[0] ) ;
    exit (EXIT_FAILURE) ;
  }

  // ------------------------------------------------------------------------
  // retreive the options

  // Create the FEC access
  int cpt = 1 ;
  try {
    // FEC access
    fecAccess = createFecAccess (argc, argv, &cpt) ;

    // Access manager
    accessManager = new FecAccessManager ( fecAccess ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ;
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;

    exit (EXIT_FAILURE) ;
  }

  if ((cpt < argc) && (strncasecmp (argv[cpt], "-multiframes", 13) == 0)) {
    std::cout << "Using multiframe mode" << std::endl ;
    multiFrames_ = true ;
    cpt ++ ;
  }

  if ((cpt < argc) && (strncasecmp (argv[cpt], "-blockMode", 13) == 0)) {
    std::cout << "Using the VME block mode" << std::endl ;
    blockMode_ = true ;
    cpt ++ ;

    fecAccess->setFifoAccessMode(blockMode_) ;
  }

  if ((cpt < argc) && (strncasecmp (argv[cpt], "-noFack", strlen("-noFack")) == 0)) {
    std::cout << "Do not use the force acknowledge" << std::endl ;
    forceAck_ = false ;
    cpt ++ ;

    fecAccess->setFifoAccessMode(blockMode_) ;
  }

  // Check the over options
  if ((cpt < argc) && (strncasecmp (argv[cpt], "-input=", 7) == 0)) {

    if (argc < 3) {
      std::cerr << "Error in usage, bad values for the options, please type --help for help" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    char opt[1000] ;
    snprintf (opt, 1000, "%s", argv[cpt]) ;
    strtok (opt , "=") ;
    char *str2 = strtok (NULL, "=") ;

    if (str2 == NULL) {
      std::cerr << "Error in usage, bad values for the options, please type --help for help" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    strcpy (inputfile, str2) ;
    inputfileFlag = true ;

    cpt ++ ;
  }

  if ((cpt < argc) && (strncasecmp (argv[cpt], "-output=", 8) == 0)) {

    if ( ((argc < 4) && inputfileFlag) || (argc < 3)) {
      std::cerr << "Error in usage, bad values for the options, please type --help for help" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    char opt[100] ;
    snprintf (opt, 100, "%s", argv[cpt]) ;
    strtok (opt , "=") ;
    char *str2 = strtok (NULL, "=") ;

    if (str2 == NULL) {
      std::cerr << "Error in usage, bad values for the options, please type --help for help" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    strcpy (outputfile, str2) ;
    outputfileFlag = true ;

    cpt ++ ;
  }
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-output", 6) == 0)) {

    if (argc < 4) {
      std::cerr << "Error in usage, bad values for the options, please type --help for help" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    strcpy (outputfile, argv[cpt+1]) ;
    outputfileFlag = true ;

    cpt += 2 ;
  }

  // ------------------------------------------------------------------------
  // retreive the commands

  // Database
  if ((cpt < argc) && (strncasecmp (argv[cpt], "-database", 9) == 0)) {

#ifdef DATABASE
    // Check if parameters are given
    if ( (argc-cpt) >= 2) {

      // Retreive the partition name
      cpt++ ;
      strcpy (partitionName, argv[cpt]) ;

      if ( (argc-cpt) >= 2 ) {
	// Retreive the version
	cpt ++ ;
	std::string confdb(argv[cpt]);
	int ipass = confdb.find(".");
	std::string major = confdb.substr(0,ipass); 
	std::string minor = confdb.substr(ipass+1,confdb.size()); 

	// Convert the version into values
	versionMajor = fromString<unsigned int>(major) ;
	versionMinor = fromString<unsigned int>(minor) ;
      }

      if (argc-cpt >= 2) {
	// Retreive the FEC hardware ID
	cpt ++ ;
	fecHardwareId = std::string(argv[cpt]) ;
      }
    }
    else {
      std::cerr << "You need to specify a partition name" << std::endl ;
      std::cerr << argv[0] << " partition name [Version X.Y] [FEC hardware id]" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    choix = 1 ;    
#else
    std::cerr << "The database flag is not set, cannot use the database" << std::endl ;
    exit (EXIT_FAILURE) ;
#endif
  }
  // Tracker
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-tracker", 8) == 0)) {

    strcpy(partitionName, "nil") ;

    if ( (argc-cpt) != 1) {

      std::cerr << "Error in usage for the Tracker command, please type --help for help" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    choix = 2 ;
  }
  // Create partition
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-createPartition", 16) == 0)) {

    if ( (strncasecmp (outputfile, "database", 8) == 0) && ((argc-cpt) == 2) ) { 

      // A partition name is given
      cpt ++ ;
      //sprintf (partitionName, "%s_%d", argv[cpt], time(NULL)) ;
      sprintf (partitionName, "%s", argv[cpt]) ;
    } else
      if ( (argc-cpt) != 1) {

	std::cerr << "No partition name is specified" << std::endl ;
        std::cerr << "Error in usage, please type --help for help" << std::endl ;
        exit (EXIT_FAILURE) ;
      }

    choix = 5 ;
  }
  // Upload DCU from input to output
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-uploadDcu", 10) == 0)) {

    if (! inputfileFlag || ! outputfileFlag) {
      std::cerr << "Error in usage, please type --help for help" << std::endl ;
      std::cerr << "An input and an output must be give" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    strcpy (partitionName, "nil") ;
    if ( ( (strncasecmp (outputfile, "database", 8) == 0) || 
	   (strncasecmp (inputfile, "database", 8) == 0)) && 
	 ((argc-cpt) >= 2) ) { 

      // A partition name is given
      cpt ++ ;
      sprintf (partitionName, "%s", argv[cpt]) ;
      if (sscanf(partitionName, "%d", &dcuHardId)) strcpy (partitionName, "nil") ;

      if ((argc-cpt) >= 2) {
	// A timestamp start is given
	cpt ++ ;
	if (! sscanf (argv[cpt], "%u", &timeStampStart)) {
	  std::cerr << "Error in usage, please type --help for help" << std::endl ;
	  std::cerr << "The timestamp must be a number" << std::endl ;
	  exit (EXIT_FAILURE) ;
	}

	if ((argc-cpt) == 2) {
	  // A timestamp stop is given
	  cpt ++ ;
	  if (! sscanf (argv[cpt], "%u", &timeStampStop)) {
	    std::cerr << "Error in usage, please type --help for help" << std::endl ;
	    std::cerr << "The timestamp must be a number" << std::endl ;
	    exit (EXIT_FAILURE) ;
	  }
	}
      }
    }
    else if ( (argc-cpt) != 1) {

      std::cerr << "(1) Error in usage, please type --help for help" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    choix = 7 ;
  }
  // Upload DCU from input to output
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-uploadFileDcu", 14) == 0)) {

    if (! inputfileFlag || ! outputfileFlag) {
      std::cerr << "Error in usage, please type --help for help" << std::endl ;
      std::cerr << "An input and an output must be give" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    strcpy (partitionName, "nil") ;
    if ( ( (strncasecmp (outputfile, "database", 8) == 0) || 
	   (strncasecmp (inputfile, "database", 8) == 0)) && 
	 ((argc-cpt) >= 2) ) { 

      // A partition name is given
      cpt ++ ;
      sprintf (partitionName, "%s", argv[cpt]) ;

      if ((argc-cpt) >= 2) {
	// A timestamp start is given
	cpt ++ ;
	if (! sscanf (argv[cpt], "%u", &timeStampStart)) {
	  std::cerr << "Error in usage, please type --help for help" << std::endl ;
	  std::cerr << "The timestamp must be a number" << std::endl ;
	  exit (EXIT_FAILURE) ;
	}

	if ((argc-cpt) == 2) {
	  // A timestamp stop is given
	  cpt ++ ;
	  if (! sscanf (argv[cpt], "%u", &timeStampStop)) {
	    std::cerr << "Error in usage, please type --help for help" << std::endl ;
	    std::cerr << "The timestamp must be a number" << std::endl ;
	    exit (EXIT_FAILURE) ;
	  }
	}
      }
    } else
      if ( (argc-cpt) != 1) {

        std::cerr << "(2) Error in usage, please type --help for help" << std::endl ;
        exit (EXIT_FAILURE) ;
      }

    choix = 8 ;
  }
  // Test i2c device
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-testi2c", 8) == 0)) {

    if (! inputfileFlag) {
      std::cerr << "Error in usage, please type --help for help" << std::endl ;
      std::cerr << "An input file must be give" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    choix = 9 ;
  }
  // Test the performance
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-testPerf", 9) == 0)) {

    if (! inputfileFlag) {
      std::cerr << "Error in usage, please type --help for help" << std::endl ;
      std::cerr << "An input file must be give" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    choix = 10 ;
  }
  // Test the performance
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-testTiming", 11) == 0)) {

    if (! inputfileFlag) {
      std::cerr << "Error in usage, please type --help for help" << std::endl ;
      std::cerr << "An input file must be give" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    choix = 11 ;
  }
  // Upload DCU from FECs and convert it
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-dcuConversion", strlen("-dcuConversion")) == 0)) {

    if (! inputfileFlag) {
      std::cerr << "No conversion files will be applied, display the raw data" << std::endl ;
      strcpy (inputfile,"nil") ;
    }

    choix = 12 ;
  }
  // Upload DCU from FECs and convert it
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-dcuReadout", strlen("-dcuReadout")) == 0)) {

    strcpy (inputfile,"nil") ;
    choix = 12 ;
  }
  // Download a file into the hardware and upload the corresponding devices
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-download", strlen("-download")) == 0)) {

    if (! inputfileFlag) {
      std::cerr << "Error in usage, please type --help for help" << std::endl ;
      std::cerr << "An input must be given" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    // Check if the input file is there
    FILE *f = fopen (inputfile, "r") ;
    if (f == NULL) {

      std::cerr << "The file " << inputfile << " does not exists, please type --help for help" << std::endl ;

      exit (EXIT_FAILURE) ;
    }
    else {

      fclose (f) ;
      choix = 3 ;

      if ((argc-cpt) == 2) fecHardwareId = std::string(argv[cpt+1]) ;
      else fecHardwareId = "" ;
    }
  }
  else if ((cpt < argc) && (strncasecmp (argv[cpt], "-fec", strlen("-fec")) == 0)) {
  }
  else {

    if ( (argc-cpt) != 1) {

      std::cerr << "(3) Error in usage, please type --help for help" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    FILE *f = fopen (argv[cpt], "r") ;
    if (f == NULL) {

      std::cerr << "The file " << argv[cpt] << " does not exists, please type --help for help" << std::endl ;

      exit (EXIT_FAILURE) ;
    }
    else {

      fclose (f) ;
      choix = 3 ;
      strncpy (inputfile,argv[cpt],1000) ;
    }
  }

  // Create the access and call the correct methods
  try {
    switch (choix) {

    case 1: // Database
      if (outputfileFlag)
	downloadUploadFromDatabase ( accessManager,
				     partitionName,
				     fecHardwareId,
				     outputfile,
				     versionMajor,
				     versionMinor ) ;
      else
	downloadUploadFromDatabase ( accessManager,
				     partitionName,
				     fecHardwareId,
				     NULL,
				     versionMajor, 
				     versionMinor ) ;
      break ;
    case 2: // Tracker autodectection
      uploadDetectedTrackerDevices ( fecAccess,
				     accessManager,
				     inputfile,
				     outputfile,
				     partitionName,
				     structureName) ;
      break ;
    case 3: // Download a file into the hardware
      parseDownloadFile (accessManager, inputfile, outputfile, fecHardwareId) ;
      break ;
    case 5:// Tracker autodectection
      uploadDetectedTrackerDevices ( fecAccess,
				     accessManager,
				     inputfile,
				     outputfile,
				     partitionName,
				     structureName) ;
      break ;
    case 7: // Upload DCU from input to output
      uploadDcu ( inputfile, outputfile, partitionName, dcuHardId, timeStampStart, timeStampStop ) ;
      break ;
    case 8: // Upload DCU from input to output
      uploadFileDcu ( inputfile, outputfile, partitionName, timeStampStart, timeStampStop ) ;
      break ;
    case 9: // Test i2c devices
      parseDownloadFileTestI2c ( fecAccess, accessManager, inputfile ) ;
      break ;
    case 10: // Test the performance
      testPerformance ( fecAccess, accessManager, inputfile ) ;
      break ;
    case 11: // Test the performance
      testTiming ( fecAccess, accessManager, inputfile ) ;
      break ;
    case 12: // Readout the DCU and make the conversion
      dcuConversion ( accessManager, inputfile, partitionName, outputfile ) ;
      break ;
    }
  }

  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ;
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Delete all
  delete accessManager ;
  delete fecAccess ;

  exit (EXIT_SUCCESS) ;
}
