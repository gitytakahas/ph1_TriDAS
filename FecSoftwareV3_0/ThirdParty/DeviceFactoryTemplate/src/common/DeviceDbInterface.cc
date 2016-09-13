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

#include "DbInterface.h"

int main ( int argc, char **argv ) {

  unsigned long startMillis, endMillis ;

  bool fecUsed = false, fedUsed = false, connectionUsed = false, redundancyUsed = false, detIdUsed = false ;
  std::string partitionName = "AgainOnTheRoadTest" ;
  unsigned int fedId = 0 ;
  unsigned int versionMajorId = 0, versionMinorId = 0 ;
  unsigned int maskVersionMajorId = 0, maskVersionMinorId = 0 ;
  bool uploadDB = false, uploadFile = false ;
  bool fedStrip = true ;
  bool refresh = false ;
  bool o2oUsed = false ;
  bool changed ; // is the download has been performed from the local cache or from the database
  bool deleteCache = false ;
  std::string fecFileUpload  = "/tmp/FecOutput.xml" ;
  std::string fedFileUpload1 = "/tmp/OneFedOutput.xml" ;
  std::string fedFileUpload  = "/tmp/AllFedOutput.xml" ;
  std::string connectionFileUpload  = "/tmp/AllConnectionOutput.xml" ;
  std::string redundancyFileUpload  = "/tmp/redundancy" ;
  std::string detIdFileUpload  = "/tmp/detid.xml" ;

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
    else if (param == "-fedId") {
      
      if (i < argc) {
	std::string fedIdStr = std::string(argv[i+1]) ;
	fedId = fromString<unsigned int>(fedIdStr) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the FED ID name after the option -fedId" << std::endl ;
      
    }
    else if (param == "-fec") {

      fecUsed = true ;
    }
    else if (param == "-connection") {

      connectionUsed = true ;
    }
    else if (param == "-fed") {

      fedUsed = true ;
    }
    else if (param == "-redundancy") {

      redundancyUsed = true ;
    }
    else if (param == "-detId") {

      detIdUsed = true ;
    }
    else if (param == "-uploadDB") {

      uploadDB = true ;
    }
    else if (param == "-o2o") {

      o2oUsed = true ;
    }
    else if (param == "-uploadFile") {

      uploadFile = true ;
      std::cout << "Upload the FEC and PIA devices into the file " << fecFileUpload << std::endl ;
      std::cout << "If you specify a FED ID then upload the FED in " << fedFileUpload1 << std::endl ;
      std::cout << "Upload of all FEDs from a partition into " << fedFileUpload << std::endl ;
      std::cout << "Upload of all connections from a partition into " << connectionFileUpload << std::endl; 
      std::cout << "Upload of all redundancy rings from a partition into " << redundancyFileUpload << "_FECHARDID_Ring_#.xml" << std::endl; 
      std::cout << "Upload of all DET ID from a partition into " << detIdFileUpload << std::endl; 
    }
    else if (param == "-noFedStrip") {

      fedStrip = false ;
    }
    else if (param == "-version") {
      
      if (i < argc) {
	versionMajorId = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
	if (i < argc) {
	  versionMinorId = fromString<unsigned int>(argv[i+1]) ;
	  i ++ ;
	}
	else
	  std::cerr << "Error: you must specify the FED version you want to use as X Y" << std::endl ;
      }
      else
	std::cerr << "Error: you must specify the FED version you want to use as X Y" << std::endl ;
    }
    else if (param == "-maskVersion") {
      
      if (i < argc) {
	maskVersionMajorId = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
	if (i < argc) {
	  maskVersionMinorId = fromString<unsigned int>(argv[i+1]) ;
	  i ++ ;
	}
	else
	  std::cerr << "Error: you must specify the FED version you want to use as X Y" << std::endl ;
      }
      else
	std::cerr << "Error: you must specify the FED version you want to use as X Y" << std::endl ;
    }
    else if (param == "-deleteCache") {
      deleteCache = true ;
    }
    else if (param == "-refresh") {

      refresh = true ;
    }
    else
      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
  }

//   for (int i = 0 ; i < 100 ; i ++) {
//     DbInterface dbInterface1 ;
//     std::cout << "Download partition of FED for partition " << partitionName << std::endl ;
//     unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//     std::vector<Fed9U::Fed9UDescription *> fedV = dbInterface1.downloadFEDFromDatabase(partitionName,changed,versionMajorId,versionMinorId) ;
//     unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//     std::cout << "Download of " << fedV.size() << " FEDs for partition " << partitionName << " took " << std::dec << (endMillis-startMillis) << " ms" << std::endl ;
//   }

  // -----------------------------------------------------------------
  // Create a database cache
  //std::string login = "nil", passwd = "nil", path = "nil" ;
  //DeviceFatoryInterface::getDatabaseConfiguration(login,passwd,path);
  //DbInterface dbInterface(false,falselogin,passwd,path,true) ;
  DbInterface dbInterface ;

  if (refresh) {
    std::cout << "-------------------  Refresh the caching system for FEC/FED/connections" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    dbInterface.refreshCacheXMLClob() ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "The refresh took " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  if (deleteCache) {
    std::cout << "-------------------  Delete the caching system for FEC/FED/connections" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    ((FecDeviceFactory *)dbInterface.getDeviceFactory())->getDatabaseAccess()->deleteCacheXMLClob() ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "The delete of the cache system took " << (endMillis-startMillis) << " ms" << std::endl ;
  }

  // -----------------------------------------------------------------
  // Download the FEC and display it
  if (fecUsed) {
    std::cout << "------------------- FEC download" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.downloadFECDevicesFromDatabase (partitionName, changed, versionMajorId, versionMinorId, maskVersionMajorId, maskVersionMinorId) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error) {
      deviceVector devices = dbInterface.getCurrentDevices() ;
      std::cout << "Found " << std::dec << devices.size() << " FEC devices in " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
      
    // Download the PIA reset
    std::cout << "------------------- PIA download" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    error = dbInterface.downloadPIAResetFromDatabase(partitionName) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error) {
      piaResetVector piaV = dbInterface.getCurrentPia() ;
      std::cout << "Found " << std::dec << piaV.size() << " PIA reset in " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
      
    // Redownload all the devices
    std::cout << "------------------- Redownload FEC" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    error = dbInterface.downloadFECDevicesFromDatabase (partitionName, changed, versionMajorId, versionMinorId, maskVersionMajorId, maskVersionMinorId) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error) {
      deviceVector devices = dbInterface.getCurrentDevices() ;
      std::cout << "Found " << std::dec << devices.size() << " FEC devices in " << (endMillis-startMillis) << " ms" << std::endl ;

      unsigned int crateId = 0xFFFFFFFF ;
      unsigned int fecSlot = 0xFFFFFFFF ;
      //unsigned int ringSlot = 0xFFFFFFFF ;
	
      // Display the number of devices for each type of FEC
      unsigned int dcuCcuNumber = 0, dcuFehNumber = 0, apvNumber = 0, pllNumber = 0, muxNumber = 0, dohNumber = 0, aohNumber = 0 ;
      unsigned int dcuCcuNumberF = 0, dcuFehNumberF = 0, apvNumberF = 0, pllNumberF = 0, muxNumberF = 0, dohNumberF = 0, aohNumberF = 0 ;
      for (deviceVector::iterator itDevice = devices.begin() ; itDevice != devices.end() ; itDevice ++) {

	if (crateId == 0xFFFFFFFF) crateId = (*itDevice)->getCrateId() ;
	else {
	  if (crateId == 0) std::cerr << "Wrong crateId " << crateId << " please update it to 1" << std::endl ;
	  if (crateId != (*itDevice)->getCrateId()) std::cerr << "The device in the list belongs to different crates, should not" << std::endl ;
	}
	if (fecSlot == 0xFFFFFFFF) { fecSlot = (*itDevice)->getFecSlot() ; }
	//if (fecSlot == 0xFFFFFFFF) { fecSlot = (*itDevice)->getFecSlot() ; std::cout << fecSlot << std::endl ; }
	//else if (fecSlot != (*itDevice)->getFecSlot()) { std::cout << "New FEC slot: " << ((*itDevice)->getFecSlot()) << std::endl ; fecSlot = (*itDevice)->getFecSlot() ;}

	switch ((*itDevice)->getDeviceType()) {
	case DCU: {
	  dcuDescription *dcuD = dynamic_cast<dcuDescription *>(*itDevice) ;
	  if (dcuD->getDcuType() == DCUFEH) {
	    dcuFehNumber ++ ;
	    if (!dcuD->getEnabled()) dcuFehNumberF ++ ;
	  }
	  else if (dcuD->getDcuType() == DCUCCU) {
	    dcuCcuNumber ++ ;	  
	    if (!dcuD->getEnabled()) dcuCcuNumberF ++ ;
	  }
	  break ;
	}
	case APV25:
	  apvNumber ++ ;
	  if (!(*itDevice)->getEnabled()) apvNumberF ++ ;
	  break ;
	case APVMUX:
	  muxNumber ++ ;
	  if (!(*itDevice)->getEnabled()) muxNumberF ++ ;
	  break ;
	case LASERDRIVER: {
	  laserdriverDescription *ldD = dynamic_cast<laserdriverDescription *>(*itDevice) ;
	  if (getAddressKey(ldD->getKey()) == 0x60) {
	    aohNumber ++ ;
	    if (!(*itDevice)->getEnabled()) aohNumberF ++ ;
	  }
	  else if (getAddressKey(ldD->getKey()) == 0x70) {
	    std::cerr << "Device type not correctly set for DOH" << std::endl ;
	    dohNumber ++ ;
	    if (!(*itDevice)->getEnabled()) dohNumberF ++ ;
	  }
	  break ;
	}
	case DOH:
	  dohNumber ++ ;
	  if (!(*itDevice)->getEnabled()) dohNumberF ++ ;
	  break ;
	case PLL: 
	  pllNumber ++ ;
	  if (!(*itDevice)->getEnabled()) pllNumberF ++ ;
	  break ;
	default:
	  std::cerr << "Unknown device" << std::endl ;
	  break ;
	}
      }

      std::cout << "Found " << std::dec << apvNumber << " APVs" << /*" (" << apvNumberF << " are disabled" << ")" <<*/ std::endl ;
      std::cout << "Found " << std::dec << muxNumber << " APV MUX" << /*" (" << muxNumberF << " are disabled" << ")" <<*/ std::endl ;
      std::cout << "Found " << std::dec << pllNumber << " PLLs" << /*" (" << muxNumberF << " are disabled" << ")" <<*/ std::endl ;
      std::cout << "Found " << std::dec << aohNumber << " AOHs" << /*" (" << muxNumberF << " are disabled" << ")" <<*/ std::endl ;
      std::cout << "Found " << std::dec << dohNumber << " DOHs" << /*" (" << muxNumberF << " are disabled" << ")" <<*/ std::endl ;
      std::cout << "Found " << std::dec << dcuCcuNumber << " DCUs on CCU" << /*" (" << muxNumberF << " are disabled" << ")" <<*/ std::endl ;
      std::cout << "Found " << std::dec << dcuFehNumber << " DCUs on FEH" << /*" (" << muxNumberF << " are disabled" << ")" <<*/ std::endl ;

      double totalSize = apvNumber * sizeof(apvDescription) + muxNumber * sizeof(muxDescription) + pllNumber * sizeof(pllDescription) + aohNumber * sizeof(laserdriverDescription) + dohNumber * sizeof(laserdriverDescription) + dcuCcuNumber * sizeof(dcuDescription) + dcuFehNumber * sizeof(dcuDescription) ;

      std::cout << "FEC devices total size in memory: " << (totalSize/1024/1024) << " Mbytes" << std::endl ;
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload FEC and PIA in database for a major version (if asked)
  if (fecUsed && uploadDB) {

    unsigned int major = 0, minor = 0 ;
    std::cout << "------------------- FEC and PIA upload in database in major version" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadFec(partitionName,true,true,&major,&minor) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the FEC/PIA data into a major version in the database in " << (endMillis-startMillis) << " ms (version" << major << "." << minor << ")" << std::endl ;

    // -----------------------------------------------------------------
    // Download FEC from database (if asked)
    if (!error) {
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      error = dbInterface.downloadFECFromDatabase(partitionName,changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	
      if (!error) {
	deviceVector devices = dbInterface.getCurrentDevices() ;
	std::cout << "Found " << std::dec << devices.size() << " FEC devices in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload only FEC in database for a minor version (if asked)
  if (fecUsed && uploadDB) {

    std::cout << "------------------- FEC upload in database in minor version" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadFec(partitionName,false) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the FEC data into a minor version in the database in " << (endMillis-startMillis) << " ms" << std::endl ;

    // -----------------------------------------------------------------
    // Download FEC from database (if asked)
    if (!error) {
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      error = dbInterface.downloadFECFromDatabase(partitionName,changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	      
      if (!error) {
	deviceVector devices = dbInterface.getCurrentDevices() ;
	piaResetVector piaV = dbInterface.getCurrentPia() ;
	std::cout << "Download of the FEC (" << std::dec << devices.size() << ") and PIA (" << piaV.size() << ") = " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload FEC in file (if asked)
  if (fecUsed && uploadFile) {

    std::cout << "------------------- FEC and PIA upload in file" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadFec(fecFileUpload) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the FEC data into file " << fecFileUpload << " in " << (endMillis-startMillis) << " ms" << std::endl ;

    // -----------------------------------------------------------------
    // Download FEC from file (if asked)
    if (!error) {
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      error = dbInterface.downloadFECFromFile(fecFileUpload,changed) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

      if (!error) {
	deviceVector devices = dbInterface.getCurrentDevices() ;
	piaResetVector piaV = dbInterface.getCurrentPia() ;
	std::cout << "Download of the FEC (" << std::dec << devices.size() << ") and PIA (" << piaV.size() << ") = " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // download of one FED
  if (fedUsed) {

    if (fedId != 0) {
      std::cout << "Download of the FED " << std::dec << fedId << std::endl ;
      // FED download
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      Fed9U::Fed9UDescription *fed = dbInterface.downloadFEDFromDatabase(fedId,partitionName,changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      if (fed != NULL) {
	std::cout << "Download of the FED " << std::dec << fedId << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << "Error during the download of FED: " << std::dec << fedId << std::endl ;
      }
      
      // FED download
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      fed = dbInterface.downloadFEDFromDatabase(fedId,partitionName, changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      if (fed != NULL) {
	std::cout << "Download of the FED " << std::dec << fed->getFedId() << " in " << std::dec << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << "Error during the download of FED: " << std::dec << fedId << std::endl ;
      }
    }
    else {
      std::cout << "Download partition of FED for partition " << partitionName << std::endl ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::vector<Fed9U::Fed9UDescription *> fedV = dbInterface.downloadFEDFromDatabase(partitionName,changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Download of " << fedV.size() << " FEDs for partition " << partitionName << " took " << std::dec << (endMillis-startMillis) << " ms" << std::endl ;
      if (fedV.size() == 0) {
	std::string errorMessage = dbInterface.getErrorMessage() ;
	if (errorMessage != "") std::cerr << "Error during FED download: " << errorMessage << std::endl ;
      }
    }

    // -----------------------------------------------------------------
    // Upload FED partition in file (if asked)
    if (fedUsed && uploadFile && (fedId != 0)) {
      std::cout << "------------------- FED upload in file" << std::endl ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      int error = dbInterface.uploadFed(fedFileUpload1) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Upload the FED " << fedId << " data into file " << fedFileUpload1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	  
      // -----------------------------------------------------------------
      // Download FED from file
      if (!error) {
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	Fed9U::Fed9UDescription *fed = dbInterface.downloadFEDFromFile(fedId, fedFileUpload1, changed) ;
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	    
	if (fed != NULL) {
	  std::cout << "Download FED " << fed->getFedId() << " from file " << fedFileUpload1 << " in " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  std::cout << "Error during the download of FED: " << std::dec << fedId << std::endl ;
	}
      }
    }

    // ----------------------------------------------------------------------------
    // third download with a given FED ID (if it exists)
    if (fedId != 0) {
      std::cout << "Download of the FED " << std::dec << fedId << std::endl ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      Fed9U::Fed9UDescription *fed = dbInterface.downloadFEDFromDatabase(fedId,partitionName,changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      if (fed != NULL) {
	std::cout << "Download of the FED " << std::dec << fedId << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << "Error during the download of FED: " << std::dec << fedId << std::endl ;
      }
    }
  }
      
  // -----------------------------------------------------------------
  // Upload FED partition in database in a major version (if asked)
  if (fedUsed && uploadDB) {
    std::cout << "------------------- FED Partition upload in database in a major version" << ((fedStrip == true) ? " (including the strips)" : " (not including strip)") << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = 0 ; 
    if (fedId == 0) dbInterface.uploadFed(partitionName,fedStrip,true) ;
    else {
      std::cout << "------------------- FED " << fedId << " upload in database in a minor version" << std::endl ;
      dbInterface.uploadFed(fedId,partitionName,fedStrip,false) ;
    }
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (fedId == 0) {
      if (!error)
	std::cout << "Upload the FED partition into the database in a major version in " << (endMillis-startMillis) << " ms" << std::endl ;
      else
	std::cout << "Error during the upload of FEDs in database in a major verion" << std::endl ;
    }
    else {
      if (!error)
	std::cout << "Upload the FED " << fedId << " into the database in a minor version in " << (endMillis-startMillis) << " ms" << std::endl ;
      else
	std::cout << "Error during the upload of FED " << fedId << " in database in a minor verion" << std::endl ;
    }

    // -----------------------------------------------------------------
    // download of a partition of FED
    std::cout << "Download all FEDs from partition " << partitionName << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::vector<Fed9U::Fed9UDescription *> fedV = dbInterface.downloadFEDFromDatabase(partitionName,changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Found " << std::dec << fedV.size() << " FEDs in " << (endMillis-startMillis) << " ms" << std::endl ;

    if (fedId != 0) {
      // -----------------------------------------------------------------
      // download of a FED from database
      std::cout << "Download of the FED " << std::dec << fedId << std::endl ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      Fed9U::Fed9UDescription *fed = dbInterface.downloadFEDFromDatabase(fedId,partitionName,changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      if (fed != NULL) {
	std::cout << "Download of the FED " << std::dec << fedId << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << "Error during the download of FED: " << std::dec << fedId << std::endl ;
      }
    }
  }

  // -----------------------------------------------------------------
  // Upload FED partition in file (if asked)
  if (fedUsed && uploadFile) {
    std::cout << "------------------- FED Partition upload in file" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadFed(fedFileUpload) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error)
      std::cout << "Upload the FED partition into file " << fedFileUpload << " in " << (endMillis-startMillis) << " ms" << std::endl ;
    else
      std::cout << "Error during the upload of FEDs in file " << fedFileUpload << std::endl ;
    
    // -----------------------------------------------------------------
    // Download one FED partition from file
    if ( (!error) && (fedId != 0) ) {
      std::cout << "Download of the FED " << std::dec << fedId << " from file " << fedFileUpload << std::endl ;
      // FED download
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      Fed9U::Fed9UDescription *fed = dbInterface.downloadFEDFromFile(fedId,fedFileUpload,changed) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      if (fed != NULL) {
	std::cout << "Download of the FED " << std::dec << fedId << " in " <<  (endMillis-startMillis) << " ms from file " << fedFileUpload << std::endl ;
	delete fed;
      }
      else {
	std::cout << "Error during the download of FED: " << std::dec << fedId << " from file " << fedFileUpload << std::endl ;
      }
    }
    
    // -----------------------------------------------------------------
    if (!error) {
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::vector<Fed9U::Fed9UDescription *> fedV = dbInterface.downloadFEDFromFile(fedFileUpload,changed) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Download " << fedV.size() << " FEDs from file " << fedFileUpload << " in " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    return 0 ;
  }

  // -----------------------------------------------------------------
  // Download connections
  if (connectionUsed) {
    std::cout << "------------------- Connection download" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.downloadConnectionsFromDatabase (partitionName, changed, versionMajorId, versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error) {
      ConnectionVector connections = dbInterface.getConnections() ;
      double totalSize = connections.size()*sizeof(ConnectionDescription) ;
      std::cout << "Found " << std::dec << connections.size() << " connections in " << (endMillis-startMillis) << " ms " << "(size in memory = " << (totalSize/1024/1024) << " Mbytes)" << std::endl ;
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
      
    // Redownload all connections
    std::cout << "------------------- Redownload Connection" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    error = dbInterface.downloadConnectionsFromDatabase (partitionName, changed, versionMajorId, versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error) {
      ConnectionVector connections = dbInterface.getConnections() ;
      std::cout << "Found " << std::dec << connections.size() << " connections in " << (endMillis-startMillis) << " ms" << std::endl ;	
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload connections in database for a major version (if asked)
  if (connectionUsed && uploadDB) {

    unsigned int major = 0, minor = 0 ;
    std::cout << "------------------- Connections upload in database in major version" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadConnections(partitionName,true,&major,&minor) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the connections data into a major version in the database in " << (endMillis-startMillis) << " ms (version " << major << "." << minor << ")" << std::endl ;

    // -----------------------------------------------------------------
    // Download connections from database
    if (!error) {
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      error = dbInterface.downloadConnectionsFromDatabase(partitionName,changed,versionMajorId,versionMinorId,maskVersionMajorId,maskVersionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      
      if (!error) {
	ConnectionVector connections = dbInterface.getConnections() ;
	std::cout << "Found " << std::dec << connections.size() << " connections in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload FEC in file (if asked)
  if (connectionUsed && uploadFile) {

    std::cout << "------------------- Connections upload in file" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadConnections(connectionFileUpload) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the connections into file " << connectionFileUpload << " in " << (endMillis-startMillis) << " ms" << std::endl ;

    // -----------------------------------------------------------------
    // Download FEC from file (if asked)
    if (!error) {
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      error = dbInterface.downloadConnectionsFromFile(connectionFileUpload,changed) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

      if (!error) {
	ConnectionVector connections = dbInterface.getConnections() ;
	std::cout << "Download " << std::dec << connections.size() << " from file " << connectionFileUpload << " took " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Download redundancy
  if (redundancyUsed) {
    std::cout << "------------------- redundancy download" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.downloadFecRedundancyFromDatabase (partitionName) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error) {
      tkringVector ringDescription = dbInterface.getFecRedundancy() ;
      std::cout << "Found " << std::dec << ringDescription.size() << " rings in " << (endMillis-startMillis) << " ms" << std::endl ;
      for (tkringVector::iterator it = ringDescription.begin() ; it != ringDescription.end() ; it ++) {
	std::cout << "Ring on FEC " << (*it)->getFecHardwareId() << " on ring " << (*it)->getRingSlot() << " with " << (*it)->getCcuVector()->size() << " CCUs" << std::endl ;
      }

      void *memory = NULL ;
      try {
	// serialise the buffer in memory and extract it
	unsigned int sizeBuffer ;
	memory = FecFactory::writeTo ( ringDescription, sizeBuffer ) ;
	std::cout << "Serialise " << ringDescription.size() << " ring descriptions, buffer size = " << sizeBuffer << std::endl ;
	//char *coucou = (char *)memory ;
	//std::cout << coucou << std::endl ;
	tkringVector v = FecFactory::readFrom ( memory ) ;
	std::cout << "Found " << v.size() << " ring descriptions after deserialisation buffer size" << std::endl ;
	for (tkringVector::iterator it = v.begin() ; it != v.end() ; it ++) {
	  std::cout << "Ring on FEC " << (*it)->getFecHardwareId() << " on ring " << (*it)->getRingSlot() << " with " << (*it)->getCcuVector()->size() << " CCUs" << std::endl ;
	}
	FecFactory::deleteVectorI(v) ;
	free(memory) ;
      }
      catch (FecExceptionHandler &e) {
	std::cout << "Error during the serialisation or deserialisation: " << e.what() << std::endl ;
	if (memory != NULL) free(memory) ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload redundancy in file (if asked)
  if (redundancyUsed && uploadFile) {

    std::cout << "------------------- Redundancy upload in file" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::list<std::string> listFiles ;
    int error = dbInterface.uploadFecRedundancyToFile(redundancyFileUpload, listFiles) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the FEC redundancy into " << listFiles.size() << " file(s): " << redundancyFileUpload << "_FECHARDID_Ring_#.xml in " << (endMillis-startMillis) << " ms" << std::endl ;

    // -----------------------------------------------------------------
    // Download redundancy from file (if asked)
    if (!error) {

      dbInterface.clearFecRedundancyDescriptions() ;
      for (std::list<std::string>::iterator it = listFiles.begin() ; it != listFiles.end() ; it ++) {

	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	error = dbInterface.downloadFecRedundancyFile(*it, changed, true, true) ; 
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

	if (!error) {
	  tkringVector tkRingDescriptions = dbInterface.getFecRedundancy() ;
	  std::cout << "Download " << std::dec << tkRingDescriptions.size() << " ring from file " << *it << " took " << (endMillis-startMillis) << " ms" << std::endl ;
	}
	else {
	  std::cout << dbInterface.getErrorMessage() << std::endl ;
	}
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload redundancy in database
  if (redundancyUsed && uploadDB) {
    std::cout << "------------------- Redundancy upload in database" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadFecRedundancyToDb(partitionName) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the redudancy into the database in " << (endMillis-startMillis) << " ms" << std::endl ; 

    // -----------------------------------------------------------------
    // Download redundancy from database
    if (!error) {
      std::cout << "------------------- redundancy download" << std::endl ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      int error = dbInterface.downloadFecRedundancyFromDatabase (partitionName) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      if (!error) {
	tkringVector ringDescription = dbInterface.getFecRedundancy() ;
	std::cout << "Found " << std::dec << ringDescription.size() << " rings in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }


  // -----------------------------------------------------------------
  // Download det ID
  if (detIdUsed) {

//     try {
//       unsigned int versionMajor = 0, versionMinor = 0, partitionId = 0  ;
//       (dynamic_cast<TkDcuInfoFactory *>(dbInterface.getDeviceFactory()))->getPartitionVersion("ALL",&versionMajor,&versionMinor,&partitionId) ;
//       std::cout << "Current version for the DET ID is " << versionMajor << "." << versionMinor << std::endl ;
//     }
//     catch (FecExceptionHandler &e) {
//       std::cerr << "Error when retreive the version: " << e.what() << std::endl ;
//     }
//     catch (oracle::occi::SQLException &e) {
//       std::cerr << (dbInterface.getDeviceFactory()))->what("Error when retreive the version",e) << std::endl ;
//     }

    std::cout << "------------------- DET ID download" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.downloadDetIdFromDatabase (partitionName, changed,versionMajorId,versionMinorId) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error) {
      Sgi::hash_map<unsigned long, TkDcuInfo *> listTkDcuInfo = dbInterface.getDetIdList() ;
      std::cout << "Found " << std::dec << listTkDcuInfo.size() << " det ids in " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }

    // Redownload the det ids
    std::cout << "------------------- DET ID redownload" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    error = dbInterface.downloadDetIdFromDatabase (partitionName, changed,versionMajorId,versionMinorId) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (!error) {
      Sgi::hash_map<unsigned long, TkDcuInfo *> listTkDcuInfo = dbInterface.getDetIdList() ;
      std::cout << "Found " << std::dec << listTkDcuInfo.size() << " det ids in " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload det id in file (if asked)
  if (detIdUsed && uploadFile) {
    
    std::cout << "------------------- DET ID upload in file" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadDetItToFile(detIdFileUpload) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the DET ID in file " << detIdFileUpload << " done in " << (endMillis-startMillis) << " ms" << std::endl ;
    
    // -----------------------------------------------------------------
    // Download det id from file (if asked)
    if (!error) {
      
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      error = dbInterface.downloadDetIdFromFile(detIdFileUpload, changed, false) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      
      if (!error) {
	Sgi::hash_map<unsigned long, TkDcuInfo *> listTkDcuInfo = dbInterface.getDetIdList() ;
	std::cout << "Found " << std::dec << listTkDcuInfo.size() << " det ids  in file " << detIdFileUpload << " in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -----------------------------------------------------------------
  // Upload det id in database
  if (detIdUsed && uploadDB) {

    std::cout << "------------------- det id upload in database" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    int error = dbInterface.uploadDetIdToDB() ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Upload the DET ID into the database in " << (endMillis-startMillis) << " ms" << std::endl ; 

    // -----------------------------------------------------------------
    // Download det id from database
    if (!error) {
      std::cout << "------------------- DET ID redownload" << std::endl ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      error = dbInterface.downloadDetIdFromDatabase (partitionName, changed,versionMajorId,versionMinorId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      if (!error) {
	Sgi::hash_map<unsigned long, TkDcuInfo *> listTkDcuInfo = dbInterface.getDetIdList() ;
	std::cout << "Found " << std::dec << listTkDcuInfo.size() << " det ids in " << (endMillis-startMillis) << " ms" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  // -------------------------------------------------------------------
  // Set an o2o operation and xcheck that the current state is coherent
  if (o2oUsed) {
    int error = dbInterface.setO2OOperation (partitionName,"TOB",time(NULL)) ;
    if (!error) {
      int result = dbInterface.getO2OXchecked(partitionName) ;
      if (result == 0) {
	std::cout << "O2O and current state are not the same => o2o should be triggered again" << std::endl ;
      }
      else if (result == 1) {
	std::cout << "O2O and current state are in the same state, configuration can continue" << std::endl ;
      }
      else {
	std::cout << dbInterface.getErrorMessage() << std::endl ;
      }
    }
    else {
      std::cout << dbInterface.getErrorMessage() << std::endl ;
    }
  }

  return 0 ;
}
