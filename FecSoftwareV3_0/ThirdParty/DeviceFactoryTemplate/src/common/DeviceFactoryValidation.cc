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
#include "keyType.h"        // FEC/ring/CCU/channel/address are managed in this file (not a class)
#include "deviceType.h"

#include "FecExceptionHandler.h"    // exceptions
#include "DeviceFactory.h"          // devicefactory

// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************
// 
//                                           Download from a file or database
//
// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************

void testDonwloadFile ( ) {

}

void testDownloadDatabase ( ) {

}

// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************
// 
//                                           Upload in file or database
//
// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************

/** Upload in the database the devices passed by arguments
 * \param vDevices - vector of devices
 * \param vPiaReset - vector of PIA reset
 */
void testUploadDatabase ( std::string partitionName, deviceVector vDevices, piaResetVector vPiaReset ) {
  
  std::cout << "testTkDcuConversionFactors from database" << std::endl ;
  
  std::string login="nil", passwd="nil", path="nil" ;
  
  try {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return ;
    }

    FecFactory fecFactory (login, passwd, path) ;

    unsigned int vMajor, vMinor ;
    fecFactory.setFecDevicePiaDescriptions (vDevices, vPiaReset, partitionName, &vMajor, &vMinor) ;

    std::cout << "Upload " << vDevices.size() << " in database version " << vMajor << "." << vMinor << std::endl ;
    std::cout << "Upload " << vPiaReset.size() << " in database" << std::endl ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "An error happends: " << e.what() << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {

    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
  }
}

/** Upload in a file the devices passed by arguments
 * \param outputFilane - output file name
 * \param vDevices - vector of devices
 * \param vPiaReset - vector of PIA reset
 */
void testUploadFile ( std::string outputFileName, deviceVector vDevices, piaResetVector vPiaReset ) {

  try {

    FecFactory fecFactory ;
    fecFactory.setOutputFileName (outputFileName) ;
    
    fecFactory.setFecDevicePiaDescriptions (vDevices, vPiaReset) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "Error in uploading the file " << outputFileName << std::endl ;
    std::cerr << e.what() << std::endl ;
  }

}

// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************
// 
//                                           Simulated data
//
// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************

/** Produce the data for the upload
 * \param vDevices - vector of devices
 * \param piaReset - vector of PIA reset
 */
void produceDeviceAndPiaData ( unsigned int fecSlot,
			       unsigned int ccuNumber,
			       unsigned int modulePerCcu,
			       unsigned int apvNumber,
			       std::string fecHardwareId,
			       unsigned int crateId,
			       deviceVector &vDevices, piaResetVector &vPiaReset ) {

  tscType8 bias[MAXLASERDRIVERCHANNELS] = {24,24,24} ;
  apvDescription apvE  ( (keyType)0, 47, 10, 2, 98, 52, 34, 80, 34, 55, 34, 40, 0, 30, 30, 37, 0, 8, 0) ;  // APV
  muxDescription muxE  ( (keyType)0, 255 ) ;                                     // MUX
  dcuDescription dcuE ( (keyType)0, time(NULL), 0, 0, 0, 0, 0, 0, 0, 0, 0) ;  // DCU on CCU
  laserdriverDescription aohE ( (keyType)0, 0, bias) ;    // AOH
  pllDescription pllE  ( (keyType)0, 10, 1) ;           // PLL

  for (unsigned int ring = 0 ; ring < 8 ; ring ++) {

    for (unsigned int ccu = 1 ; ccu <= ccuNumber ; ccu ++) {

      // Add the PIA reset
      keyType indexP = buildCompleteKey(fecSlot, ring, ccu, 0x30, 0) ;
      piaResetDescription *piaReset = new piaResetDescription (indexP, 10, 10000, 0xFF) ;
      //** change the fechardwareid Damien 17/03/06
      piaReset->setFecHardwareId (fecHardwareId,crateId) ;
      //**
      vPiaReset.push_back (piaReset) ;

      keyType indexDA = buildCompleteKey(fecSlot, ring, ccu, 0x10, 0) ;
      if (ccu == 1) {
	// Add the DOH ring A
	laserdriverDescription *dohD = aohE.clone() ;
	dohD->setAccessKey(indexDA | setAddressKey (0x70)) ;
	dohD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (dohD) ;
      }
      else if (ccu == 2) {
	// Add the DOH ring B
	laserdriverDescription *dohD = aohE.clone() ;
	dohD->setAccessKey(indexDA | setAddressKey (0x70)) ;
	dohD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (dohD) ;
      }
      // Add the DCU on CCU
      dcuDescription *dcuDC = dcuE.clone() ;
      dcuDC->setAccessKey(indexDA | setAddressKey (0x0)) ;
      dcuDC->setFecHardwareId (fecHardwareId,crateId) ;
      dcuDC->setDcuHardId (indexDA | setAddressKey(0x0)) ;
      vDevices.push_back (dcuDC) ;

      for (unsigned int channel = 0 ; channel < modulePerCcu ; channel ++) {

	keyType indexD = buildCompleteKey(fecSlot, ring, ccu, channel+0x11, 0) ;

	// Add the APV
	apvDescription *apvD = apvE.clone() ;
	apvD->setAccessKey(indexD | setAddressKey (0x20)) ;
	apvD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (apvD) ;

	// Add the APV
	apvD = apvE.clone() ;
	apvD->setAccessKey(indexD | setAddressKey (0x21)) ;
	apvD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (apvD) ;

	if (apvNumber == 6) {
	  // Add the APV
	  apvD = apvE.clone() ;
	  apvD->setAccessKey(indexD | setAddressKey (0x22)) ;
	  apvD->setFecHardwareId (fecHardwareId,crateId) ;
	  vDevices.push_back (apvD) ;

	  // Add the APV
	  apvD = apvE.clone() ;
	  apvD->setAccessKey(indexD | setAddressKey (0x23)) ;
	  apvD->setFecHardwareId (fecHardwareId,crateId) ;
	  vDevices.push_back (apvD) ;
	}

	// Add the APV
	apvD = apvE.clone() ;
	apvD->setAccessKey(indexD | setAddressKey (0x24)) ;
	apvD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (apvD) ;

	// Add the APV
	apvD = apvE.clone() ;
	apvD->setAccessKey(indexD | setAddressKey (0x25)) ;
	apvD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (apvD) ;

	// Add the MUX
	muxDescription *muxD = muxE.clone() ;
	muxD->setAccessKey(indexD | setAddressKey (0x43)) ;
	muxD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (muxD) ;

	// Add the DCU on FEH
	dcuDescription *dcuD = dcuE.clone() ;
	dcuD->setAccessKey(indexD | setAddressKey (0x0)) ;
	dcuD->setFecHardwareId (fecHardwareId,crateId) ;
	dcuD->setDcuHardId (indexD | setAddressKey(0x0)) ;
	vDevices.push_back (dcuD) ;

	// Add the laserdriver
	laserdriverDescription *aohD = aohE.clone() ;
	aohD->setAccessKey(indexD | setAddressKey (0x60)) ;
	aohD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (aohD) ;

	// Add the PLL
	pllDescription *pllD = pllE.clone() ;
	pllD->setAccessKey(indexD | setAddressKey (0x44)) ;
	pllD->setFecHardwareId (fecHardwareId,crateId) ;
	vDevices.push_back (pllD) ;
      }
    }
  }
}

/** Ask some questions to fill the vectors of PIA and devices
 */
int question ( unsigned int &fecSlot,
	       unsigned int &ringNumber,
	       unsigned int &ccuNumber,
	       unsigned int &modulePerCcu,
	       unsigned int &apvNumber,
	       std::string &fecHardwareId,
	       unsigned int &crateId ) {

  unsigned int moduleNumber, modulePerRing ;
  std::cout << "Before starting you need to fill some data: " << std::endl ;
  std::cout << "How many modules you want to produce for the FEC: " ;
  std::cin >> moduleNumber ;
  if (moduleNumber < 8) {
    std::cout << "You will have 1 ring with " << moduleNumber << " modules" << std::endl ;
    ringNumber = 1 ;
  }
  else {
    // Calculate the number of modules per ring
    modulePerRing = (unsigned int)(moduleNumber / 8) ;
    std::cout << "You will have 8 rings with " << modulePerRing << " modules on each ring" << std::endl ;
  }
  
  
  // Calculate the number of CCUs needed
  if (modulePerRing > 1905) {
    std::cerr << "Too many modules for a ring" << std::endl ;
    return -1; 
  }
  else {
    
    ccuNumber = (modulePerRing / 10) + 1 ; // 10 modules per CCU
    modulePerCcu = 10 ;
    if (modulePerCcu > 127) {
      ccuNumber = modulePerRing / 15 ;
      modulePerCcu = 15 ;
    }

    if (modulePerCcu > 127) {
      std::cerr << "Too many modules for a CCU" << std::endl ;
      return -1 ;
    }
  }

  std::cout << "Please enter the number of APV per modules: " ;
  std::cin >> apvNumber ;
  std::cout << "Please enter the FEC hardware ID: " ;
  std::cin >> fecHardwareId ;
  std::cout << "Please enter the FEC slot: " ;
  std::cin >> fecSlot ;
  std::cout << "Please enter the crate slot: " ;
  std::cin >> crateId ;

  return 0 ;
}


void simulatedData ( std::string outputFileName, std::string partitionName ) {

  unsigned int fecSlot ;
  unsigned int ringNumber ;
  unsigned int ccuNumber ;
  unsigned int modulePerCcu ;
  unsigned int apvNumber ;
  std::string fecHardwareId ;
  unsigned int crateId ;
  deviceVector vDevices ; 
  piaResetVector vPiaReset ;

  if (!question(fecSlot, ringNumber, ccuNumber, modulePerCcu, apvNumber, fecHardwareId, crateId )) {

    std::cout << "Will create " << (modulePerCcu * 8) << " modules on " << ccuNumber << " CCUs" << std::endl ;
    produceDeviceAndPiaData ( fecSlot, ccuNumber, modulePerCcu, apvNumber, fecHardwareId, crateId,
			      vDevices, vPiaReset ) ;

    std::cout << "Found " << vDevices.size() << " devices and " << vPiaReset.size() << " PIA reset and upload it in file " << outputFileName << std::endl ;

    if (outputFileName == "database") testUploadDatabase(partitionName, vDevices, vPiaReset) ;
    else testUploadFile (outputFileName, vDevices, vPiaReset) ;

    FecFactory::deleteVectorI (vDevices) ;
    FecFactory::deleteVectorI (vPiaReset) ; 
  }
}


// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************
// 
//                                           Merge several files in the same file
//
// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************
int mergeFiles ( int argc, char **argv ) {

  if (argc <= 3 || !strncmp(argv[1], "-help", strlen("-help"))) {
    
    std::cerr << "Error in usage " << argv[0] << " [Output File Name] [List of input File Name]" << std::endl ;
    std::cerr << "\tInstance are changed for each process, so the first input file will be instance 0, then the second file will instance 1, etc." << std::endl ;
    return -1 ;
  }

  deviceVector vDevices ;
  piaResetVector piaReset ;
  FecFactory fecFactory ;
  for (int i = 2 ; i < argc ; i ++) {

    // Retreive the vector from files for devices
    fecFactory.setInputFileName (argv[i]) ;
    deviceVector td, td1 ;
    fecFactory.getFecDeviceDescriptions ( td ) ;

    // Copy the vector of the files for devices
    FecFactory::vectorCopyI(td1, td) ;

    // Change the instance to 0 for the 1st, 1 for the second file, etc.
    // Add it to the previous one
    if (td1.size() > 0) {
      for ( deviceVector::iterator it = td1.begin() ; it != td1.end() ; it ++) {
	//(*it)->setInstance(i-2) ;
	vDevices.push_back((*it)) ;
      }
    }

    // Retreive the vector for the PIA
    piaResetVector tp, tp1 ;
    fecFactory.getPiaResetDescriptions ( tp ) ;

    // Copy the vector of the files for PIA resets
    FecFactory::vectorCopyI(tp1, tp) ;

    // Change the instance to 0 for the 1st, 1 for the second file, etc.
    // Add it to the previous one
    if (tp1.size() > 0) {
      for ( piaResetVector::iterator it = tp1.begin() ; it != tp1.end() ; it ++) {
	//(*it)->setInstance(i-2) ;
	piaReset.push_back((*it)) ;
      }
    }

    std::cout << "Found " << td1.size() << " devices and " << tp1.size() << " PIA rests, for the file " << argv[i] << " adding the instance " << (i-2) << std::endl ;

    // delete the vectors
    FecFactory::deleteVectorI(tp1) ;
    FecFactory::deleteVectorI(td1) ;
  }

  fecFactory.setOutputFileName (argv[1]) ;
  std::cout << "Upload " << vDevices.size() << " devices and " << piaReset.size()  << " PIA resets in file " << fecFactory.getOutputFileName() << std::endl ;

  fecFactory.setFecDevicePiaDescriptions (vDevices, piaReset) ;

  return 0 ;
}

// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************
// 
//                                            
//
// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************


/** Test of the FecFactory::getDcuFromIndex
 * \param vDevice - vector of devices
 */
void testFecFactoryGetDcuFromIndex ( ) {

  std::cout << "testFecFactoryGetDcuFromIndex for FecFactory::getDcuFromIndex" << std::endl ;

//   // Find a dcu regarding a description
//   if (vDevice.size() > 0) {
//     deviceDescription *deviceD = *(vDevice.begin()) ;

//     dcuDescription *dcuDescription = FecFactory::getDcuFromIndex ( deviceD, vDevice ) ;

//     char msg[80] ;
//     decodeKey(msg,deviceD->getKey()) ;

//     if (dcuDescription != NULL) 
//       std::cout << "Found the DCU " << dcuDescription->getDcuHardId() << " on index " << msg << std::endl ;
//     else 
//       std::cerr << "Did not find a DCU on index " << msg << std::endl ;
//   }
//   else {

//     // No device in the vector
//     std::cerr << "No device in the vector" << std::endl ;
//   }
}


// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************
// 
//                              Tracker DCU conversion factors
//
// *****************************************************************************************************
// *****************************************************************************************************
// *****************************************************************************************************

/** Convert a DCU values to real values
 */
void testConversionDcuToRealValues ( TkDcuConversionFactors conversionFactors ) {

  std::cout << "testConversionDcuToRealValues: not yet implemented" << std::endl ;

}

/** Test the TkDcuConversion retreive from file and upload it in another file or the database
 * \param fileName - file name 
 */
int testTkDcuConversionFactors ( unsigned long dcuHardId, std::string fileName, std::string outputFileName ) {

  std::cout << "testTkDcuConversionFactors from file " << fileName << std::endl ;
  std::string login="nil", passwd="nil", path="nil" ;

  try {
    TkDcuConversionFactory tkDcuConversionFactory (false) ;
    tkDcuConversionFactory.setInputFileName (fileName) ;

    TkDcuConversionFactors *conversionFactors = tkDcuConversionFactory.getTkDcuConversionFactors ( dcuHardId ) ;
  
    conversionFactors->display() ;
    std::cout << "Press <enter> to continue" ; getchar() ;

    //testConversionDcuToRealValues ( *conversionFactors ) ;
    //std::cout << "Press <enter> to continue" ; getchar() ;

    if (outputFileName != "nil") {
      if (outputFileName != "database") {

	std::cout << "Upload in file " << outputFileName << std::endl ;
	tkDcuConversionFactory.setUsingFile() ;
	tkDcuConversionFactory.setOutputFileName(outputFileName) ;
	
	// Update the value
	conversionFactors->setAdcGain0(2.1) ;
	dcuConversionVector vConversionFactors ;
	vConversionFactors.push_back(conversionFactors) ;
	
	// Add a value
	TkDcuConversionFactors *conversionFactors1 = conversionFactors->clone() ;
	conversionFactors1->setDcuHardId(dcuHardId + 100) ;
	vConversionFactors.push_back(conversionFactors1) ;
	
	// Update and add the values
	tkDcuConversionFactory.setTkDcuConversionFactors(vConversionFactors) ;
	delete conversionFactors1 ;
	vConversionFactors.clear();

	// Comment
	std::cout << "Upload the conversion factor to the file " << outputFileName << std::endl ;
	std::cout << "The file will contains 2 entry for DCU " << dcuHardId << " and DCU " << (dcuHardId + 100) << std::endl ;
	std::cout << "The ADC gain 0 for the DCUs must be " << 2.1 << std::endl ;
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Force the reload of the file
	tkDcuConversionFactory.addFileName ( outputFileName ) ;
	TkDcuConversionFactors *conversionFactors2 = tkDcuConversionFactory.getTkDcuConversionFactors ( dcuHardId, true ) ;

	// Comment
	std::cout << "Reload the file just created and display the DCU " << dcuHardId << std::endl ;
	conversionFactors2->display() ;
	// check
	if (conversionFactors2->getAdcGain0() != 2.1) {
	  std::cerr << "Error during the upload the gain is not correct for DCU " << dcuHardId << std::endl ;
	}
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Ask for the new DCU added
	TkDcuConversionFactors *conversionFactors3 = tkDcuConversionFactory.getTkDcuConversionFactors (dcuHardId + 100, true ) ;
	std::cout << "Display now the DCU " << (dcuHardId+100) << std::endl ;
	conversionFactors3->display() ;
	// check
	if (conversionFactors3->getAdcGain0() != 2.1) {
	  std::cerr << "Error during the upload the gain is not correct for DCU " << (dcuHardId+100) << std::endl ;
	}
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Update all the values stored 
	std::cout << "Upload the conversion factor to the file " << outputFileName << ".all.xml" << std::endl ;
	std::string allFileName = outputFileName + ".all.xml" ;
	tkDcuConversionFactory.setOutputFileName(allFileName) ;
	tkDcuConversionFactory.setTkDcuConversionFactors() ;

	// Comment
	std::cout << "The file will contains all the DCU from the file, the DCU " << dcuHardId << " and DCU " <<  (dcuHardId+100) << " are updated for the gain 0" << std::endl ;
	std::cout << "Press <enter> to continue" ; getchar() ;	
      }
      else {

	std::cout << "Upload in the database" << std::endl ;
	DbAccess::getDbConfiguration (login, passwd, path) ;
	if (login == "nil" || passwd=="nil" || path=="nil") {
      
	  std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
	  std::cerr << "Exiting ..." << std::endl ;
	  return -1 ;
	}
	tkDcuConversionFactory.setDatabaseAccess (login, passwd, path) ;
	tkDcuConversionFactory.setUsingDb() ;

	// Update the value
	conversionFactors->setAdcGain0(2.1) ;
	dcuConversionVector vConversionFactors ;
	vConversionFactors.push_back(conversionFactors) ;

	// Add a value
	TkDcuConversionFactors *conversionFactors1 = conversionFactors->clone() ;
	conversionFactors1->setDcuHardId(dcuHardId + 100) ;
	vConversionFactors.push_back(conversionFactors1) ;
	
	// Update and add the values
	tkDcuConversionFactory.setTkDcuConversionFactors(vConversionFactors) ;
	delete conversionFactors1 ;
	vConversionFactors.clear();

	// Comment
	std::cout << "Upload the conversion factor to the database" << std::endl ;
	std::cout << "The database upload will contains 2 entry for DCU " << dcuHardId << " and DCU " << (dcuHardId + 100) << std::endl ;
	std::cout << "The ADC gain 0 for the DCUs must be " << 2.1 << std::endl ;
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Force the reload of the database for both DCU
	TkDcuConversionFactors *conversionFactors2 = tkDcuConversionFactory.getTkDcuConversionFactors ( dcuHardId, true ) ;

	// Comment
	std::cout << "Reload the value from the database and display the DCU " << dcuHardId << std::endl ;
	conversionFactors2->display() ;

	// check
	if (conversionFactors2->getAdcGain0() != 2.1) {
	  std::cerr << "Error during the upload the gain is not correct for DCU " << dcuHardId << std::endl ;
	}
	std::cout << "Press <enter> to continue" ; getchar() ;	
	
	// Ask for the new DCU added
	TkDcuConversionFactors *conversionFactors3 = tkDcuConversionFactory.getTkDcuConversionFactors (dcuHardId + 100, true) ;
	std::cout << "Reload the value form the database and display now the DCU " << (dcuHardId+100) << std::endl ;
	conversionFactors3->display() ;
	// check
	if (conversionFactors3->getAdcGain0() != 2.1) {
	  std::cerr << "Error during the upload the gain is not correct for DCU " << (dcuHardId+100) << std::endl ;
	}
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Upload all the DCU into the database
	tkDcuConversionFactory.setTkDcuConversionFactors() ;
	std::cout << "Upload the database with all values already displayed (so the one extracted from file) and the DCU " << dcuHardId << ", DCU " << (dcuHardId+100) << std::endl ;
	std::cout << "Press <enter> to continue" ; getchar() ;	
      }
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "An error happends: " << e.what() << std::endl ;
    return -1 ;
  }
  catch (oracle::occi::SQLException &e) {

    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  return 0 ;
}

/** Test the TkDcuConversion retreive from database
 */
int testTkDcuConversionFactors ( unsigned long dcuHardId, std::string outputFileName ) {

  std::cout << "testTkDcuConversionFactors from database" << std::endl ;

  std::string login="nil", passwd="nil", path="nil" ;

  try {
    DbAccess::getDbConfiguration (login, passwd, path) ;

    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    TkDcuConversionFactory tkDcuConversionFactory (login, passwd, path) ;
    
    TkDcuConversionFactors *conversionFactors = tkDcuConversionFactory.getTkDcuConversionFactors ( dcuHardId ) ;
    
    conversionFactors->display() ;
    std::cout << "Press <enter> to continue" ; getchar() ;

    testConversionDcuToRealValues ( *conversionFactors ) ;

    if (outputFileName != "nil") {
      if (outputFileName != "database") {

	std::cout << "Upload in file " << outputFileName << std::endl ;
	tkDcuConversionFactory.setUsingFile() ;
	tkDcuConversionFactory.setOutputFileName(outputFileName) ;

	// Update the value
	conversionFactors->setAdcGain0(2.1) ;
	dcuConversionVector vConversionFactors ;
	vConversionFactors.push_back(conversionFactors) ;

	// Add a value
	TkDcuConversionFactors *conversionFactors1 = conversionFactors->clone() ;
	conversionFactors1->setDcuHardId(dcuHardId + 100) ;
	vConversionFactors.push_back(conversionFactors1) ;

	// Update and add the values
	tkDcuConversionFactory.setTkDcuConversionFactors(vConversionFactors) ;
	delete conversionFactors1 ;
	vConversionFactors.clear();

	// Comment
	std::cout << "Upload the conversion factor to the file " << outputFileName << std::endl ;
	std::cout << "The file will contains 2 entry for DCU " << dcuHardId << " and DCU " << (dcuHardId + 100) << std::endl ;
	std::cout << "The ADC gain 0 for the DCUs must be " << 2.1 << std::endl ;
	std::cout << "Press <enter> to continue" ; getchar() ; 
	
	// Force the reload of the file
	tkDcuConversionFactory.addFileName ( outputFileName ) ;
	TkDcuConversionFactors *conversionFactors2 = tkDcuConversionFactory.getTkDcuConversionFactors ( dcuHardId, true ) ;

	// Comment
	std::cout << "Reload the file just created and display the DCU " << dcuHardId << std::endl ;
	conversionFactors2->display() ;
	// check
	if (conversionFactors2->getAdcGain0() != 2.1) {
	  std::cerr << "Error during the upload the gain is not correct for DCU " << dcuHardId << std::endl ;
	}
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Ask for the new DCU added
	TkDcuConversionFactors *conversionFactors3 = tkDcuConversionFactory.getTkDcuConversionFactors (dcuHardId + 100, true) ;
	std::cout << "Display now the DCU " << (dcuHardId+100) << std::endl ;
	conversionFactors3->display() ;
	// check
	if (conversionFactors3->getAdcGain0() != 2.1) {
	  std::cerr << "Error during the upload the gain is not correct for DCU " << (dcuHardId+100) << std::endl ;
	}
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Update all the values stored 
	std::cout << "Upload the conversion factor to the file " << outputFileName << ".all.xml" << std::endl ;
	std::string allFileName = outputFileName + ".all.xml" ;
	tkDcuConversionFactory.setOutputFileName(allFileName) ;
	tkDcuConversionFactory.setTkDcuConversionFactors() ;

	// Comment
	std::cout << "The file will contains all the DCU from the file, the DCU " << dcuHardId << " and DCU " <<  (dcuHardId+100) << " are updated for the gain 0" << std::endl ;
	std::cout << "Press <enter> to continue" ; getchar() ;
      }
      else {

	std::cout << "Upload in the database" << std::endl ;

	// Update of the value
	conversionFactors->setAdcGain0(2.1) ;
	dcuConversionVector vConversionFactors ;
	vConversionFactors.push_back(conversionFactors) ;

	// Add a value
	TkDcuConversionFactors *conversionFactors1 = conversionFactors->clone() ;
	conversionFactors1->setDcuHardId(dcuHardId + 100) ;
	vConversionFactors.push_back(conversionFactors1) ;

	// Update and add the values
	tkDcuConversionFactory.setTkDcuConversionFactors(vConversionFactors) ;
	delete conversionFactors1 ;
	vConversionFactors.clear();

	// Comment
	std::cout << "Upload the conversion factor to the database" << std::endl ;
	std::cout << "The database upload will contains 2 entry for DCU " << dcuHardId << " and DCU " << (dcuHardId + 100) << std::endl ;
	std::cout << "The ADC gain 0 for the DCUs must be " << 2.1 << std::endl ;
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Force the reload for all the values and get it again
	TkDcuConversionFactors *conversionFactors2 = tkDcuConversionFactory.getTkDcuConversionFactors ( dcuHardId, true ) ;

	// Comment
	std::cout << "Reload the value from the database and display the DCU " << dcuHardId << std::endl ;
	conversionFactors2->display() ;

	// check
	if (conversionFactors2->getAdcGain0() != 2.1) {
	  std::cerr << "Error during the upload the gain is not correct for DCU " << dcuHardId << std::endl ;
	}
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Ask for the new DCU added
	TkDcuConversionFactors *conversionFactors3 = tkDcuConversionFactory.getTkDcuConversionFactors (dcuHardId + 100, true) ;
	std::cout << "Reload the value form the database and display now the DCU " << (dcuHardId+100) << std::endl ;
	conversionFactors3->display() ;
	// check
	if (conversionFactors3->getAdcGain0() != 2.1) {
	  std::cerr << "Error during the upload the gain is not correct for DCU " << (dcuHardId+100) << std::endl ;
	}
	std::cout << "Press <enter> to continue" ; getchar() ;	

	// Upload all the DCU into the database
	tkDcuConversionFactory.setTkDcuConversionFactors() ;
	std::cout << "Upload the database with all values already displayed (so the one extracted from file) and the DCU " << dcuHardId << ", DCU " << (dcuHardId+100) << std::endl ;
	std::cout << "Press <enter> to continue" ; getchar() ;	
      }
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "An error happends: " << e.what() << std::endl ;
    return -1 ;
  }
  catch (oracle::occi::SQLException &e) {

    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  return 0 ;
}

/* ************************************************************************************************ */
/*                                                                                                  */
/*                                               Main                                               */
/*                                                                                                  */
/* ************************************************************************************************ */

void help ( int argc, char **argv ) {

  std::cerr << "Error in usage: " << argv[0] << std::endl ;
  std::cerr << "[Option] <command>" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << "\t" << "[-input=[database | filename]" << std::endl ;
  std::cerr << "\t" << "[-output=[database | filename | nil]" << std::endl ;
  std::cerr << "\t\t" << "nil means no upload" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << "\t" << "-dcuHardId=<value> -dcuconversion" << std::endl ;
  std::cerr << "\t" << "\t\tinput file" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << "\t" << "[-partitionName=<value>] -simulatedData" << std::endl ;
  std::cerr << "\t" << "\t\toutput file or database" << std::endl ;
  std::cerr << "\t" << "\t\tIf the database is set you need to specify the partition name" << std::endl ;
  std::cerr << std::endl ;
  std::cerr << "\t" << "-merge <output file name> <list of input file name>" << std::endl ;
  std::cerr << std::endl ;
}

int main ( int argc, char **argv ) {

  std::string inputFileName = "" ; bool inputSet = false ;
  std::string outputFileName = "nil" ; bool outputSet = false ;
  std::string partitionName = "" ; bool partitionSet = false ;
  unsigned long dcuHardId = 0 ; bool dcuHardIdSet = false ;
  int choix = 0 ;

  if (argc == 1 || (strncasecmp(argv[1], "-help", 5) == 0)) {
    help (argc, argv) ;
    exit (EXIT_FAILURE) ;
  }

  for (int i = 1 ; i < argc ; i ++) {

    if (strncasecmp(argv[i], "-input=", strlen("-input=")) == 0) {
    
      char opt[1000] ;
      snprintf (opt, 1000, "%s", argv[i]) ;
      strtok (opt , "=") ;
      char *str2 = strtok (NULL, "=") ;
      if (str2 == NULL) {
	std::cerr << "Error in usage, bad values for the option input, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }

      inputFileName = str2 ;
      inputSet = true ;
    }
    else if (strncasecmp(argv[i], "-output=", strlen("-output=")) == 0) {

      char opt[1000] ;
      snprintf (opt, 1000, "%s", argv[i]) ;
      strtok (opt , "=") ;
      char *str2 = strtok (NULL, "=") ;
      if (str2 == NULL) {
	std::cerr << "Error in usage, bad values for the option output, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }

      outputFileName = str2 ;
      outputSet = true ;
    }
    else if (strncasecmp(argv[i], "-dcuHardId=", strlen("-dcuHardId=")) == 0) {

      char opt[1000] ;
      snprintf (opt, 1000, "%s", argv[i]) ;
      strtok (opt , "=") ;
      char *str2 = strtok (NULL, "=") ;
      if (str2 == NULL) {
	std::cerr << "Error in usage, bad values for the option output, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }

      dcuHardId = fromString<unsigned long>(std::string(str2)) ;
      dcuHardIdSet = true ;
    }
    else if ((strncasecmp(argv[i], "-partitionName=", strlen("-partitionName=")) == 0)) {
      
      char opt[1000] ;
      snprintf (opt, 1000, "%s", argv[i]) ;
      strtok (opt , "=") ;
      char *str2 = strtok (NULL, "=") ;
      if (str2 == NULL) {
	std::cerr << "Error in usage, bad values for the option output, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }

      partitionName = std::string(str2) ;
      partitionSet = true ;
    }
    else if (strncasecmp(argv[i], "-dcuConversion", strlen("-dcuConversion")) == 0) {

      choix = 1 ;
    }
    else if (strncasecmp(argv[i], "-simulatedData", strlen("-simulatedData")) == 0) {

      choix = 2 ;
    }
    else if (strncasecmp(argv[i], "-mergeFiles", strlen("-mergeFiles")) == 0) {

      choix = 3 ;
    }
  }

  int error = -1 ;
  switch (choix) {

  case 0: // Error no command 
    std::cerr << "No command has been applied, please type --help for help" << std::endl ;
    exit (EXIT_FAILURE) ;
    break ;
  case 1: // DCU Conversion tests
    if (inputSet && dcuHardIdSet) {

      if (inputFileName == "database") error = testTkDcuConversionFactors ( dcuHardId, outputFileName ) ;
      else error = testTkDcuConversionFactors ( dcuHardId, inputFileName, outputFileName ) ;
    }
    else {
      if (!inputSet) std::cerr << "Error in usage for command \"-duconversion\": you need to specify an input file" << std::endl ;
      if (!dcuHardIdSet) std::cerr << "Error in usage for command \"-duconversion\": you need to specify a DCU hard id" << std::endl ;
    }
    break ;
  case 2: // Create simulated data and upload it in a file or database
    if (outputSet) {
      if ( (outputFileName == "database" && partitionSet) || (outputFileName != "database") ) simulatedData (outputFileName, partitionName) ;
      else std::cerr << "Error in usage for command \"-simulatedData\": you need a partition name when you set the database as output" << std::endl ;
    }
    else
      std::cerr << "Error in usage for command \"-simulatedData\": you need to specify an output file or database" << std::endl ;
    break ;
  case 3: // merge a file in another
    mergeFiles ( argc-1, &argv[1] ) ;
    break ;
  }

  return error ;
}
