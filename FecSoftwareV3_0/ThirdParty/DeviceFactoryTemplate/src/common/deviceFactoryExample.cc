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

/** \param -partition <partition name>
 */
int maintest ( int argc, char **argv ) {

  time_t time0, time1 ;

  // Which partition you want to use ...
  std::string partitionName = "AgainOnTheRoadTest" ;
  std::string fecHardwareId = "" ;

  bool downloadOnly = true ;

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
    //else if (param == "-version") {
    //
    //}
    else if (param == "-fecHardwareId") {

      if (i < argc) {
	fecHardwareId = std::string(argv[i+1]) ;
	i ++ ;
      }
      else 
	std::cerr << "Error: you must specify the FEC hardware ID after the option -fecHardwareId" << std::endl ;
    }
    else if (param == "-help") {

      std::cout << argv[0] << std::endl ;
      std::cout << "  -partition <partition name> [-fecHardwareId <FEC hardware ID>]" << std::endl ; 
      std::cout << "  -fecfile [file name] [-dcuconvfile <filename>] [-detidfile <filename>]" << std::endl ; 
      return 0 ;
    }
    else if (param == "-test") {

      if (i < argc) {

	if (std::string(argv[i+1]) == "upload") {
	  i ++ ;
	  downloadOnly = false ;
	}
      }
    }
    else {

      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
    }
  }

  // Display the options
  if (fecHardwareId.size())
    std::cout << "Making the test for partition " << partitionName << " and FEC hardware ID " << fecHardwareId << std::endl ;
  else 
    std::cout << "Making the test for partition " << partitionName << std::endl ;

  if (downloadOnly) 
    std::cout << "Only download operations will be done" << std::endl ;
  else
    std::cout << "Both download/upload operations will be done" << std::endl ;

  // Versioning system
  // It is mandatory that you need to include ALL the devices (APV, PLL, MUX, DCU, Laserdriver, DOH) to create a new major version
  // In other case, create only a minor version

  // Test for the commissioning procedures:
  // Take care about the creation of the database access through DeviceFactory because it take times to create the connection
  // if you delete each time you use it, you will lost precious time for the commissioning
  // The best solution is to have a DeviceFactory implemented as an attribut of the class (as a pointer)
  DeviceFactory *deviceFactory ;

  // ***************************************************************************************************
  // Create the database access
  // ***************************************************************************************************
  // retreive the connection of the database through an environmental variable called CONFDB
  string login="nil", passwd="nil", path="nil" ;
  DbAccess::getDbConfiguration (login, passwd, path) ;

  if (login == "nil" || passwd=="nil" || path=="nil") {

    std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  unsigned long startMillis, endMillis ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    // create the database access
    deviceFactory = new DeviceFactory ( login, passwd, path ) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec <<"Creation of the DeviceFactory = " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  if (fecHardwareId.size() == 0) {
    // Wait until all connections opened
    //std::cout << "Waiting for <enter>" << std::endl ;
    //getchar() ;
  }

  // the database access is created

  // ***************************************************************************************************************************
  // Partition / version for database
  // ***************************************************************************************************************************

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    // Get the version for the partition / version
    unsigned int versionDevMajor, versionDevMinor, partitionNumber, maskMajor, maskMinor ;
    deviceFactory->getFecDevicePartitionVersion ( partitionName, &versionDevMajor, &versionDevMinor, &maskMajor, &maskMinor, &partitionNumber ) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec <<"\tFEC device Partition " << std::dec << deviceFactory->getPartitionName(partitionNumber) << " (" << partitionNumber << ")" 
	      << ": Version " << versionDevMajor << "." << versionDevMinor << " (mask = " << maskMajor << "." << maskMinor << ")" << std::endl ;
    std::cout << std::dec <<"--- Retreive the FEC devices partition version = " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "Partition / version error " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }

    try {
      //      ((DbFecAccess *)((FecDeviceFactory *)deviceFactory)->getDatabaseAccess())->getPartitionName(1000) ;  
      //      ((DbFecAccess *)((FecDeviceFactory *)deviceFactory)->getDatabaseAccess())->getPartitionId("COUCOU") ;  
    }
    catch (oracle::occi::SQLException &e) {
      std::cout << e.what() << std::endl ;
      std::cout << "Press a key to continue" ; getchar() ;
    }
  
  // ***************************************************************************************************************************
  // PIA descriptions / database
  // ***************************************************************************************************************************
  
  // The vector of PIA reset vectors
  piaResetVector piaV ;

  try {

    // ----------------------------------------------------------------------------------------------------
    // Retreive the parameters for the FEC: PIA parameters for the current version
    // For that part you need:
    // #include "HashTable.h"             // => hash table for the PIA and devices
    // #include "keyType.h"               // => all methods to retreive the information concerning the access (mainly the index)
    //                                    // => the index is a 32 bits where I encode the FEC/Ring/CCU/Channel/address
    //                                    // => you can retreive the values with macro define in the file mainly:
    //                                    //     getFecKey(index), getRingKey(index), getCcuKey(index), getChannelKey(index), getAddressKey(index)
    //                                    //     buildCompleteKey(fecSlot,ringSlot,ccuAddress,channelAddress,i2cAddress)
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (fecHardwareId.size()) {
      std::cout << std::dec <<"Use FEC hardware id " << fecHardwareId << std::endl ;
      deviceFactory->getPiaResetDescriptions (partitionName,fecHardwareId,piaV) ;
    }
    else {
      deviceFactory->getPiaResetDescriptions (partitionName,piaV) ;
    }
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec <<"--- Retreive the PIA resets = " << (endMillis-startMillis) << " ms" << std::endl ;
    
    // ----------------------------------------------------------------------------------------------------
    // display it
    std::cout << std::dec <<"Found " << piaV.size() << " PIA resets" << std::endl ;

    if ((piaV.size() > 0) && (fecHardwareId.size() == 0) && !downloadOnly) {

      for (piaResetVector::iterator it = piaV.begin() ; it != piaV.end() ; it ++) {
	(*it)->setMask(1) ;
	(*it)->setDelayActiveReset(1) ;
	(*it)->setIntervalDelayReset(1) ;
      }

      // ----------------------------------------------------------------------------------------------------
      // Upload in a new major version
      time0 = time(NULL) ;
      deviceFactory->setPiaResetDescriptions (piaV, partitionName) ;
      time1 = time(NULL) ;
      std::cout << std::dec <<"Update done for the PIA in the database" << std::endl ;
      // Automatically the version for the PIA is set as the new one to be used for the next download
      std::cout << std::dec <<"--- Upload time to submit a PIA major version = " << (time1-time0) << " ms" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "Error: parsing for PIA in database " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }
    
  // ***************************************************************************************************************************
  // FEC devices / database
  // ***************************************************************************************************************************

  // Devices filled with the descriptions
  deviceVector devices ;

  try {
    // ----------------------------------------------------------------------------------------------------
    // Retreive the parameters for the FEC: FEC devices for the current version
    // For that part you need:
    // #include "HashTable.h"             // => hash table for the PIA and devices
    // #include "keyType.h"               // => all methods to retreive the information concerning the access (mainly the index)
    //                                    // => the index is a 32 bits where I encode the FEC/Ring/CCU/Channel/address
    //                                    // => you can retreive the values with macro define in the file mainly:
    //                                    //     getFecKey(index), getRingKey(index), getCcuKey(index), getChannelKey(index), getAddressKey(index)
    //                                    //     buildCompleteKey(fecSlot,ringSlot,ccuAddress,channelAddress,i2cAddress)

    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (fecHardwareId.size()) {
      std::cout << "Use FEC hardware id " << fecHardwareId << std::endl ;
      deviceFactory->getFecDeviceDescriptions (partitionName, fecHardwareId, devices) ; 
      //deviceFactory->getFecDeviceDescriptions (partitionName,fecHardwareId,devices,1,0) ; // to retreive a specific version
    }
    else {
      deviceFactory->getFecDeviceDescriptions (partitionName, devices) ; // retreive the parameters for the current version
    }
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec <<"--- Retreive the FEC devices = " << (endMillis-startMillis) << " ms" << std::endl ;

    // ----------------------------------------------------------------------------------------------------
    // display it
    std::cout << std::dec <<"Found " << devices.size() << " descriptions for current version for partition " << partitionName << std::endl ;

    if ((devices.size() > 0) && (fecHardwareId.size() == 0) && !downloadOnly) {

      // ----------------------------------------------------------------------------------------------------
      // Upload in database => create a version major with a modification on the PLL
      // set all the devices with modification in the PLL
      for (int i = 0 ; i < 1 ; i ++) {
	unsigned int major, minor ;
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	deviceFactory->setFecDeviceDescriptions (devices, partitionName, &major, &minor, true) ;
	//                                                                               ^ major version, false = minor version
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	
	std::cout << std::dec <<"Upload the FEC devices major version " << dec << major << "." << minor << " in the database" << std::endl ;
	std::cout << std::dec <<"--- Upload of FEC devices major version takes = " << (endMillis-startMillis) << " ms" << std::endl ;

	return -1 ;

	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	deviceFactory->setFecDeviceDescriptions (devices, partitionName, &major, &minor, false) ;
	//                                                                               ^ major version, false = minor version
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	
	std::cout << std::dec <<"Upload the FEC devices major version " << dec << major << "." << minor << " in the database" << std::endl ;
	std::cout << std::dec <<"--- Upload of FEC devices major version takes = " << (endMillis-startMillis) << " ms" << std::endl ;
      }
    }

    if (devices.size() > 0) {
      
      // ----------------------------------------------------------------------------------------------------
      // display if the number of APV, PLL, MUX, DCU, laserdriver, DOH
      unsigned int apvCounter, pllCounter, muxCounter, dcuFeh, dcuCcu, laserdriverCounter, dohCounter ;
      apvCounter = pllCounter = muxCounter = dcuFeh = dcuCcu = laserdriverCounter = dohCounter = 0 ;
      for (deviceVector::iterator device = devices.begin() ; device != devices.end() ; device ++) {
      
	// a deviceDescription exists for all devices: apvDescription, pllDescription, muxDescription, laserdriverDescription (both for laserdriver and DOH)
	// all this classes inherits from deviceDescription class
	deviceDescription *deviceD = *device ;
	switch (deviceD->getDeviceType()) {
	case APV25: apvCounter ++ ; break ;
	case APVMUX: muxCounter ++ ; break ;
	case PLL: { pllCounter ++ ; break ;
	case AOH: //AOH
	  laserdriverCounter ++ ; break ;
	case DCU: {
	  dcuDescription *dcuD = (dcuDescription *)deviceD;
	  if (dcuD->getDcuType() == "FEH") dcuFeh ++ ;
	  else dcuCcu++ ;
	  break ;
	}
	case DOH: dohCounter ++ ; break ;
	}
	default: // it exists other devices but not used here
	  break ;
	}
      }
      // display the number of each device
      std::cout << std::dec <<"Found " << apvCounter << " APV " << std::endl ;
      std::cout << std::dec <<"Found " << pllCounter << " PLL " << std::endl ;
      std::cout << std::dec <<"Found " << dcuFeh << " DCU on FEH " << std::endl ;
      std::cout << std::dec <<"Found " << dcuCcu << " DCU on CCU " << std::endl ;
      std::cout << std::dec <<"Found " << muxCounter << " APV MUX " << std::endl ;
      std::cout << std::dec <<"Found " << laserdriverCounter << " laserdrivers " << std::endl ;
      std::cout << std::dec <<"Found " << dohCounter << " DOH " << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "Error: parsing for FEC devices in database " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }

  // ***************************************************************************************************************************
  // DCU / database only for values
  // ***************************************************************************************************************************

  // Devices filled with the descriptions
  deviceVector dcuDevices1, dcuDevices2, dcuDevices3 ;

  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    deviceFactory->getDcuValuesDescriptions (partitionName, dcuDevices1, 0, 0xFFFFFFFF) ; // retreive the parameters for the current version
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec <<"--- Retreive " << std::dec << dcuDevices1.size() << " DCU devices = " << (endMillis-startMillis) << " ms for the partition " << partitionName << std::endl ;

    if (dcuDevices1.size()) {
      unsigned int dcuHardId = ((dcuDescription *)(*dcuDevices1.begin()))->getDcuHardId() ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->getDcuValuesDescriptions (dcuHardId, dcuDevices2, 0, 0xFFFFFFFF) ; // retreive the parameters for the current version
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << std::dec <<"--- Retreive " << std::dec << dcuDevices2.size() << " DCU devices = " << (endMillis-startMillis) << " ms for the DCU " << dcuHardId << std::endl ;
    }

    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    deviceFactory->getDcuValuesDescriptions (dcuDevices3, 0, 0xFFFFFFFF) ; // retreive the parameters for the current version
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << std::dec <<"--- Retreive " << std::dec << dcuDevices3.size() << " DCU devices = " << (endMillis-startMillis) << " ms for a range of time" << std::endl ;

    // ----------------------------------------------------------------------------------------------------
    // Upload some values
    downloadOnly = false ;
    if (dcuDevices1.size() && !downloadOnly) {
      for (deviceVector::iterator it = dcuDevices1.begin() ; it != dcuDevices1.end() ; it ++) {
	((dcuDescription *)(*it))->setTimeStamp(time(NULL)) ;
	std::cout << ((dcuDescription *)(*it))->getTimeStamp() << std::endl ;
      }
//       dcuDescription *coucou = ((dcuDescription *)(*dcuDevices1.begin()))->clone() ;
//       coucou->setDcuChannel0(1) ;
//       coucou->setDcuChannel1(1) ;
//       coucou->setDcuChannel2(1) ;
//       coucou->setDcuChannel3(1) ;
//       coucou->setDcuChannel4(1) ;
//       coucou->setDcuChannel5(1) ;
//       coucou->setDcuChannel6(1) ;
//       coucou->setDcuChannel7(1) ;
//       dcuDevices1.push_back(coucou) ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->setDcuValuesDescriptions(partitionName, dcuDevices1) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "End of DCU upload, no errors" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "Error: parsing for DCU devices in database " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
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

/** \param -file <file name>
 */
int mainfile ( int argc, char **argv ) {
  
  // XML file name
  std::string fecFileName = "" ;
  std::string dcuConvFileName = "" ;
  std::string detIdFileName = "" ;

  // Find the options
  for (int i = 1 ; i < argc ; i ++) {

    std::string param ( argv[1] ) ;
    if (param == "-fecfile") {

      if (i < argc) {
	fecFileName = std::string(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the file name -fecfile" << std::endl ;
      
    }
    else if (param == "-dcuconvfile") {

      if (i < argc) {
	dcuConvFileName = std::string(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the file name -dcuconvfile" << std::endl ;

    }
    else if (param == "-detidfile") {

      if (i < argc) {
	detIdFileName = std::string(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the file name -detidfile" << std::endl ;
    }
    else if (param == "-help") {

      std::cout << std::dec <<argv[0] << std::endl ;
      std::cout << std::dec <<"  -partition <partition name> [-fecHardwareId <FEC hardware ID>]" << std::endl ; 
      std::cout << std::dec <<"  -fecfile [file name] [-dcuconvfile <filename>] [-detidfile <filename>]" << std::endl ; 
      return 0 ;
    }
    else {

      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
    }
  }

  if (fecFileName == "") {
    std::cerr << "Error: You need to specify a file name: " << argv[0] << " -help" << std::endl ;
    return 0 ;
  }

  // Display the options
  std::cout << std::dec <<"Making the test for file " << fecFileName << std::endl ;

  // Test for the commissioning procedures:
  // if you delete each time you use it, you will lost precious time for the commissioning
  // The best solution is to have a DeviceFactory implemented as an attribut of the class (as a pointer)
  DeviceFactory *deviceFactory ;

  // Create the device factory
  try {
    // create the database access
    deviceFactory = new DeviceFactory ( ) ;
    deviceFactory->setFecInputFileName (fecFileName) ;
  }
  catch (...) {
    std::cerr << "Cannot create the device factory" << std::endl ;
    return -1 ;
  }

  // ***************************************************************************************************************************
  // PIA descriptions / file
  // ***************************************************************************************************************************

  // The vector of PIA reset vectors
  piaResetVector piaV ;

  try {
    // ----------------------------------------------------------------------------------------------------
    // Retreive the parameters for the FEC: PIA parameters for the current version
    // For that part you need:
    // #include "HashTable.h"             // => hash table for the PIA and devices
    // #include "keyType.h"               // => all methods to retreive the information concerning the access (mainly the index)
    //                                    // => the index is a 32 bits where I encode the FEC/Ring/CCU/Channel/address
    //                                    // => you can retreive the values with macro define in the file mainly:
    //                                    //     getFecKey(index), getRingKey(index), getCcuKey(index), getChannelKey(index), getAddressKey(index)
    //                                    //     buildCompleteKey(fecSlot,ringSlot,ccuAddress,channelAddress,i2cAddress)

    deviceFactory->getPiaResetDescriptions (piaV) ;
    
    // ----------------------------------------------------------------------------------------------------
    // display it
    FecFactory::display (piaV) ;

    // pause
    std::cout << std::dec <<"Press enter to continue" ; getchar();    

    // at least one pia description
    if (piaV.size() > 0) {
      
      // -------------------------------------------------------------------------------------------------
      // modify the parameters
      for (piaResetVector::iterator piaD = piaV.begin() ; piaD != piaV.end() ; piaD++) {
	
	// ----------------------------------------------------------------------------------------------------
	// display the FEC ring CCU channel
	std::cout << std::dec <<"PIA reset on: FEC " << getFecKey((*piaD)->getKey()) 
		  << " ring " << getChannelKey((*piaD)->getKey())
		  << " CCU " << getCcuKey((*piaD)->getKey()) 
		  << " Channel " << getChannelKey((*piaD)->getKey()) << std::endl ;
	
	// ----------------------------------------------------------------------------------------------------
	// modify the values
	(*piaD)->setDelayActiveReset (255) ;
      }
      
      // ----------------------------------------------------------------------------------------------------
      // Upload in a new major version
      std::string piaFileName = "/tmp/PIAOutput.xml" ;
      deviceFactory->setOutputFileName (piaFileName) ;
      deviceFactory->setPiaResetDescriptions (piaV) ;
      std::cout << std::dec <<"-------------------------- Upload of PIA ------------------" << std::endl ;
      std::cout << std::dec <<"Upload the file " << piaFileName << std::endl ;
      std::cout << std::dec <<"-----------------------------------------------------------" << std::endl ;

      // pause
      std::cout << std::dec <<"Press enter to continue" ; getchar();          

    }
    else {
      std::cerr << "No PIA descriptions found in the file" << fecFileName << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "XML parsing for database error " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }

  // ***************************************************************************************************************************
  // FEC devices / database
  // ***************************************************************************************************************************

  // Devices filled with the descriptions
  deviceVector devices ;

  try {
    // ----------------------------------------------------------------------------------------------------
    // Retreive the parameters for the FEC: FEC devices for the current version
    // For that part you need:
    // #include "HashTable.h"             // => hash table for the PIA and devices
    // #include "keyType.h"               // => all methods to retreive the information concerning the access (mainly the index)
    //                                    // => the index is a 32 bits where I encode the FEC/Ring/CCU/Channel/address
    //                                    // => you can retreive the values with macro define in the file mainly:
    //                                    //     getFecKey(index), getRingKey(index), getCcuKey(index), getChannelKey(index), getAddressKey(index)
    //                                    //     buildCompleteKey(fecSlot,ringSlot,ccuAddress,channelAddress,i2cAddress)

    deviceFactory->getFecDeviceDescriptions (devices) ; // retreive the parameters for the current version

    // ----------------------------------------------------------------------------------------------------
    // display it
    FecFactory::display (devices) ;
    std::cout << std::dec <<"Found " << devices.size() << " descriptions in the file " << fecFileName << std::endl ;

    // pause
    std::cout << std::dec <<"Press enter to continue" ; getchar();    

    if (devices.size() > 0) {

      // ----------------------------------------------------------------------------------------------------
      // display if the number of APV, PLL, MUX, DCU, laserdriver, DOH
      unsigned int apvCounter, pllCounter, muxCounter, dcuFeh, dcuCcu, laserdriverCounter, dohCounter ;
      apvCounter = pllCounter = muxCounter = dcuFeh = dcuCcu = laserdriverCounter = dohCounter = 0 ;
      for (deviceVector::iterator device = devices.begin() ; device != devices.end() ; device ++) {
      
	// a deviceDescription exists for all devices: apvDescription, pllDescription, muxDescription, laserdriverDescription (both for laserdriver and DOH)
	// all this classes inherits from deviceDescription class
	deviceDescription *deviceD = *device ;
	switch (deviceD->getDeviceType()) {
	case APV25: apvCounter ++ ; break ;
	case APVMUX: muxCounter ++ ; break ;
	case PLL: pllCounter ++ ; break ;
	case LASERDRIVER:
	  if (getAddressKey(deviceD->getKey()) == 0x60) laserdriverCounter ++ ; // laserdriver
	  else if (getAddressKey(deviceD->getKey()) == 0x70) dohCounter ++ ;    // DOH
	  break ;
	case DCU: {
	  dcuDescription *dcuD = (dcuDescription *)deviceD;
	  if (dcuD->getDcuType() == "FEH") dcuFeh ++ ;
	  else dcuCcu++ ;
	  break ;
	}
	default: // it exists other devices but not used here
	  break ;
	}
      
	// display each device
	deviceD->display ( ) ;
      }
      // display the number of each device
      std::cout << std::dec <<"Found " << apvCounter << " APV " << std::endl ;
      std::cout << std::dec <<"Found " << pllCounter << " PLL " << std::endl ;
      std::cout << std::dec <<"Found " << dcuFeh << " DCU on FEH " << std::endl ;
      std::cout << std::dec <<"Found " << dcuCcu << " DCU on CCU " << std::endl ;
      std::cout << std::dec <<"Found " << muxCounter << " APV MUX " << std::endl ;
      std::cout << std::dec <<"Found " << laserdriverCounter << " laserdrivers " << std::endl ;
      std::cout << std::dec <<"Found " << dohCounter << " DOH " << std::endl ;

      // pause
      std::cout << std::dec <<"Press enter to continue" ; getchar();    

      // ----------------------------------------------------------------------------------------------------
      // First retreive all the PLLs, the device itself a pointer on the description that you will find both in devices and pllDevices vectors
      deviceVector pllDevices = FecFactory::getDeviceFromDeviceVector (devices, PLL) ;
      //                                                                        ^ Possible values are: PLL, LASERDRIVER, DOH, DCU, PHILIPS, APVMUX, APV25, FOREXTENDED, PIARESET, FECMEMORY, DELTA, PACE, KCHIP, GOH

      // ----------------------------------------------------------------------------------------------------
      // for each PLL change the delay fine and delay coarse
      for (deviceVector::iterator device = pllDevices.begin() ; device != pllDevices.end() ; device ++) {
      
	// The object returned is an abstract class that you have to convert into a real pllDescription
	pllDescription *pll = (pllDescription *)(*device) ;
      
	// Note that the pllDescrition inherists from the deviceDescription where you can find the FEC/Ring/CCU/channels/address
	// Display the FEC/Ring/CCU/channels/address   
	std::cout << std::dec <<"PLL reset on: FEC " << getFecKey(pll->getKey()) 
		  << " ring " << getChannelKey(pll->getKey())
		  << " CCU " << getCcuKey(pll->getKey()) 
		  << " Channel " << getChannelKey(pll->getKey()) 
		  << " Address " << getAddressKey(pll->getKey()) 
		  << std::endl ;
      
	// Change the value in the all PLLs
	pll->setDelayFine (0) ;
	pll->setDelayCoarse(0) ;
      }

      // ----------------------------------------------------------------------------------------------------
      // Upload in file
      // set all the devices with modification in the PLL
      std::string fecFileName = "/tmp/FECDeviceOuput.xml" ;
      deviceFactory->setOutputFileName (fecFileName) ;
      deviceFactory->setFecDeviceDescriptions (devices) ;
    
      std::cout << std::dec <<"---------------------- FEC devices / file ---------------------" << std::endl ;
      std::cout << std::dec <<"Upload the file " << fecFileName << std::endl ;
      std::cout << std::dec <<"---------------------------------------------------------------" << std::endl ;

      // pause
      std::cout << std::dec <<"Press enter to continue" ; getchar();        

   }
    else {
      std::cerr << "No devices found in the file " << fecFileName << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "XML parsing for file " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }

  // ***************************************************************************************************************************
  // PIA + devices into a file
  // ***************************************************************************************************************************

  try {
    // set all the devices with modification in the PLL
    std::string fecPiaFileName = "/tmp/PIAFECDeviceOuput.xml" ;
    deviceFactory->setOutputFileName (fecPiaFileName) ;
    deviceFactory->setFecDevicePiaDescriptions(devices,piaV) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "XML parsing for file " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }

  // *****************************************************************************************************************************
  // Retreive the DCU corresponding to an AOH
  // *****************************************************************************************************************************
  std::cout << std::dec <<"Retreive the DCU corresponding to an AOH" << std::endl ;
  for (deviceVector::iterator device = devices.begin() ; device != devices.end() ; device ++) {
    deviceDescription *deviceD = *device ;
    if ((deviceD->getDeviceType() == LASERDRIVER) && (getAddressKey(deviceD->getKey()) == 0x60)) {
      std::cout << std::dec <<"Retreive the corresponding DCU to an AOH" << std::endl ;
      dcuDescription *dcuD = FecFactory::getDcuFromIndex(deviceD, devices) ;
      char msg[80] ; decodeKey(msg,deviceD->getKey()) ;
      if (dcuD != NULL) {
	std::cout << std::dec <<std::dec << " DCU " << dcuD->getDcuHardId() << " corresponds to the AOH on " << msg << std::endl ;
      }
      else {
	std::cout << std::dec <<"No DCU correponds to the AOH on " << msg << std::endl ;
      }
    }
  }
  // pause
  std::cout << std::dec <<"Press enter to continue" ; getchar();

  // ***************************************************************************************************************************
  // DCU / file
  // ***************************************************************************************************************************

  // DCU vector filled with the descriptions
  deviceVector dcuDevices ;

  try {
    // ----------------------------------------------------------------------------------------------------
    // Retreive the parameters for the DCU
    deviceFactory->getDcuValuesDescriptions (dcuDevices,1,0xFFFFFFFF) ; // retreive the parameters for the current version

    // ----------------------------------------------------------------------------------------------------
    // display it
    FecFactory::display (dcuDevices) ;
    std::cout << std::dec <<"Found " << dcuDevices.size() << " descriptions for the file " << fecFileName << std::endl ;

    // pause
    std::cout << std::dec <<"Press enter to continue" ; getchar();    

    // DCU can be submitted in the same way than the FEC device parameters
    // but it is not needed in that use
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR *************" << std::endl ;
    std::cerr << "XML parsing for database error " << e.what() << std::endl ;
    std::cerr << "*******************************" << std::endl ;
  }

  // ***************************************************************************************************************************
  // DCU Conversion factors / DB
  //    - Download it by DCU Hard id
  // ***************************************************************************************************************************
  if (dcuConvFileName != "") {
    try {
    
      std::cout << std::dec <<"Get the conversion factors for each DCU in the file " << dcuConvFileName << std::endl ;
      deviceFactory->setDcuConversionInputFileName(dcuConvFileName) ;
      for (deviceVector::iterator device = dcuDevices.begin() ; device != dcuDevices.end() ; device ++) {
      
	dcuDescription *dcuD = (dcuDescription *)(*device) ;
	try {
	  std::cout <<"Get conversion factors for dcu_id : "<< dcuD->getDcuHardId() <<std::endl;
	  TkDcuConversionFactors *conversionFactors = deviceFactory->getTkDcuConversionFactors ( dcuD->getDcuHardId() ) ;
	  //std::cout<<"OK!"<<std::endl;
	  if (conversionFactors != NULL) {
	    // Set the values to be converted
	    conversionFactors->setDcuDescription (dcuD) ;
	    //conversionFactors->display(true) ;
	  }
	  else 
	    std::cerr << "No conversion factors (= NULL) for the DCU 0x" << std::hex << dcuD->getDcuHardId() << " (" << std::dec << dcuD->getDcuHardId() << ")" << std::endl ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "No conversion factors for the DCU 0x" << std::hex << dcuD->getDcuHardId() << "(" << std::dec << dcuD->getDcuHardId() << ")" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	}
      }
    }
    catch (FecExceptionHandler &e) {

      std::cerr << "*********** ERROR *************" << std::endl ;
      std::cerr << "XML parsing for database error " << e.what() << std::endl ;
      std::cerr << "*******************************" << std::endl ;
    }

    // pause
    std::cout << std::dec <<"Press enter to continue" ; getchar();    
  }

  // ***************************************************************************************************************************
  // Det ID information
  //    - Download it by partition: only possiblity !!!!
  // ***************************************************************************************************************************
  if (detIdFileName != "") {
    std::cout << std::dec <<"Get the Det ID from the file " << detIdFileName << std::endl ;
    
    try {
      deviceFactory->setTkDcuInfoInputFileName(detIdFileName) ;
      for (deviceVector::iterator device = dcuDevices.begin() ; device != dcuDevices.end() ; device ++) {
      
	dcuDescription *dcuD = (dcuDescription *)(*device) ;
	try {
	  TkDcuInfo *tkDcuInfo = deviceFactory->getTkDcuInfo ( dcuD->getDcuHardId() ) ;

	  if (tkDcuInfo != NULL) tkDcuInfo->display() ; 
	  else
	    std::cerr << "No DCU info (= NULL) for the DCU 0x" << std::hex << dcuD->getDcuHardId() << "(" << std::dec << dcuD->getDcuHardId() << ")" << std::endl ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "No DCU info for the DCU 0x" << std::hex << dcuD->getDcuHardId() << "(" << std::dec << dcuD->getDcuHardId() << ")" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	}
      }
    }
    catch (FecExceptionHandler &e) {
    
      std::cerr << "*********** ERROR *************" << std::endl ;
      std::cerr << "XML parsing for database error " << e.what() << std::endl ;
      std::cerr << "*******************************" << std::endl ;
    }
  }

  // ***************************************************************************************************************************
  // Update the coarse & fine delays of the channels according to the fibers length
  // THE DCU INFOS AND THE CONNECTIONS MUST BE UPLOADED FIRST!!!
  // Cannot be done if we are using files !!!!
  // ***************************************************************************************************************************
  //std::cout << std::dec <<"Update the channels delays according to the fibers length" << std::endl ;
  //deviceFactory->updateChannelDelays();

  // ------------------------------------------------------------------------------------------------------------
  // Retreive the parameters for the FED
  // Must be complete ...

  // ------------------------------------------------------------------------------------------------------------
  // Retreive the parameters for the connections
  // Must be complete ...

  // ------------------------------------------------------------------------------------------------------------
  // End of the class destroy the database access
  try {
    // this method handle the remove of the vectors
    delete deviceFactory ;
  }  
  catch (FecExceptionHandler &e) {
    std::cerr << "Problem in deleting the device factory" << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  return 0 ;
}

int main ( int argc, char **argv ) {

  int output = -1 ;

  if (argc > 1) {
    if (strcmp(argv[1],"-fecfile") == 0)
      output =  mainfile (argc,argv) ;
    else if (strcmp(argv[1], "-test") == 0) 
      output = maintest (argc, argv) ;
    else {
      std::cerr << "Error in usage " << argv[0] << std::endl ;
      std::cerr << "\t\t-partition PARTITION_NAME" << std::endl ;
      std::cerr << "\t\t-file FILE_NAME" << std::endl ;
    }
  }
  else {
    std::cerr << "Error in usage " << argv[0] << std::endl ;
    std::cerr << "\t\t-partition PARTITION_NAME" << std::endl ;
    std::cerr << "\t\t-file FILE_NAME" << std::endl ;
  }

  return output ;
}
