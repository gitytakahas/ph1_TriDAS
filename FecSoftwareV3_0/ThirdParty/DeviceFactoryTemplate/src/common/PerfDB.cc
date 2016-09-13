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

/** \param -partition <partition name>
 */
int main ( int argc, char **argv ) {

  time_t time0, time1 ;

  // Which partition you want to use ...
  std::string partitionName = "AgainOnTheRoadTest" ;
  std::string hardwareId = "" ;
  bool piaFlag = false ;
  bool fecFlag = false ;
  bool fedFlag = false ;
  bool connectionFlag = false ;
  bool downloadOnly = true ;
  bool display = false ;
  unsigned int major = 0, minor = 0 ;
  unsigned int maskMajor = 0, maskMinor = 0 ;
  unsigned int count = 1 ;

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
    else if (param == "-hardwareId") {

      if (i < argc) {
	hardwareId = std::string(argv[i+1]) ;
	i ++ ;
      }
      else 
	std::cerr << "Error: you must specify the FEC hardware ID after the option -hardwareId" << std::endl ;
    }
    else if (param == "-pia") {
      piaFlag = true ;
    }
    else if (param == "-fec") {
      fecFlag = true ;
    }
    else if (param == "-fed") {
      fedFlag = true ;
    }
    else if (param == "-display") {
      display = true ;
    }
    else if (param == "-connection") {
      connectionFlag = true ;
    }
    else if (param == "-upload") {
      downloadOnly = false ;
    }
    else if (param == "-version") {
      if (argc > i+1) {
	std::string versionStr = argv[i+1] ;
	std::string::size_type ipass = versionStr.find("o");
	if (ipass != std::string::npos) {
	  major=fromString<unsigned int>(versionStr.substr(0,ipass)) ;
	  minor=fromString<unsigned int>(versionStr.substr(ipass+1,versionStr.length())) ;
	}
	else {
	  std::cerr << "Version are not specified like X.Y" << std::endl ;
	}
      }
      else {
	std::cerr << "You need to specify a version after the tag -version" << std::endl ;
      }
      
    }
    else if (param == "-maskVersion") {
      if (argc > i+1) {
	std::string versionStr = argv[i+1] ;
	std::string::size_type ipass = versionStr.find("o");
	if (ipass != std::string::npos) {
	  maskMajor=fromString<unsigned int>(versionStr.substr(0,ipass)) ;
	  maskMinor=fromString<unsigned int>(versionStr.substr(ipass+1,versionStr.length())) ;
	}
	else {
	  std::cerr << "Mask version are not specified like X.Y" << std::endl ;
	}
      }
      else {
	std::cerr << "You need to specify a mask version after the tag -version" << std::endl ;
      }
      
    }
    else if (param == "-help") {

      std::cout << argv[0] << std::endl ;
      std::cout << "  -partition <partition name> [-hardwareId <FEC or FED hardware ID>] [-upload] <-fec | -fed | -pia> " << std::endl ; 
      std::cout << "  -fecfile [file name] [-dcuconvfile <filename>] [-detidfile <filename>]" << std::endl ; 
      return 0 ;
    }
    else {

      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
    }
  }

  if (fecFlag) std::cout << "----------------------------------> FEC" << std::endl ;
  if (piaFlag) std::cout << "----------------------------------> PIA" << std::endl ;
  if (fedFlag) std::cout << "----------------------------------> FEC" << std::endl ;
  if (connectionFlag) std::cout << "----------------------------------> Connection" << std::endl ;

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
  std::string login="nil", passwd="nil", path="nil" ;
  DbAccess::getDbConfiguration (login, passwd, path) ;

  if (login == "nil" || passwd=="nil" || path=="nil") {

    std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  // ---------------------------------------------------
  try {
    time0 = time(NULL) ;
    // create the database access
    deviceFactory = new DeviceFactory ( login, passwd, path ) ;
    time1 = time(NULL) ;
    std::cout << "Creation of the DeviceFactory = " << (time1-time0) << " seconds" << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

//   try {

//     std::cout << "partition name -> " << partitionName << std::endl ;

//     std::vector<std::pair<std::string, bool> > coolingLoopResult ;
//     deviceFactory->checkTKCCCoolingLoop ( partitionName, coolingLoopResult ) ;
//   }
//   catch (oracle::occi::SQLException &e) {
//     std::cerr << e.what() ;
//     std::cerr << "Exiting ..." << std::endl ;
//     return -1 ;
//   }
//   catch (FecExceptionHandler &e) {
//     std::cerr << e.what() ;
//     std::cerr << "Exiting ..." << std::endl ;
//     return -1 ;
//   }
//   return 0 ;

  // -----------------------------------------------------
  // PIA
  if (piaFlag) {
    try {
      piaResetVector mesDevices ;
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->getPiaResetDescriptions (partitionName, mesDevices) ; // retreive the parameters for the current version
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Found " << mesDevices.size() << " PIA reset in " << (endMillis-startMillis) << " ms (including XML parsing)" << std::endl ;
      
      unsigned int i = 0 ;
      for (piaResetVector::iterator p = mesDevices.begin() ; p != mesDevices.end() ; p ++) {
	
	((piaResetDescription *)(*p))->setMask(i) ;
	((piaResetDescription *)(*p))->setDelayActiveReset(i) ;
	((piaResetDescription *)(*p))->setIntervalDelayReset(i) ;
	
	i ++ ;
      }

      time0 = time(NULL) ;
      MemBufOutputSource* memBufOS = new MemBufOutputSource( mesDevices, true );
      time1 = time(NULL) ;
      std::cout << "Buffer of XML buffer created in " << (time1-time0) << " seconds" << std::endl ;
      std::string xmlBuffer = (memBufOS->getPiaResetOutputBuffer())->str();
    
      std::cout << xmlBuffer << std::endl ;
      std::cout << "Press a key to continue" ; getchar() ;

      if (!downloadOnly) {

	// download of the FEC buffer
	DbPiaResetAccess dbPiaResetAccess (login,passwd,path) ;
	for (unsigned int i = 0 ; i < count ; i ++) {
	  
	  //std::cout << "Starting the download of the FEC partition " << partitionName << std::endl ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  oracle::occi::Clob *xmlClob = dbPiaResetAccess.getXMLClob(partitionName) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Retreive the CLOB from database took " << (endMillis-startMillis) << " ms" << std::endl ;

	  if (display) XMLCommonFec::display(xmlClob) ;      
	  
	  // Upload in a major version
	  //std::cout << "Starting the upload of the FEC partition " << partitionName << std::endl ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  dbPiaResetAccess.setXMLClob(xmlBuffer, partitionName);
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Update for the PIA reset done for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
	}
      }

      delete memBufOS ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cerr << "Error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;

      
      delete deviceFactory ;
      return -1 ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Oracle error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;

      
      delete deviceFactory ;
      return -1 ;
    }
  }

  // ----------------------------------------------------------------------------
  // FEC

  if (fecFlag) {

    try {
      // Retreive all devices to have a buffer to send to the database
      deviceVector mesDevices ;
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->getFecDeviceDescriptions (partitionName, mesDevices) ; // retreive the parameters for the current version
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Found " << mesDevices.size() << " devices in " << (endMillis-startMillis) << " ms (including XML parsing)" << std::endl ;
      
      time0 = time(NULL) ;
      MemBufOutputSource* memBufOS = new MemBufOutputSource( mesDevices, true);
      time1 = time(NULL) ;
      std::cout << "Buffer of XML buffer created in " << (time1-time0) << " seconds" << std::endl ;
      
      std::string *xmlBuffer = new std::string[5];
      xmlBuffer[0] = (memBufOS->getPllOutputBuffer())->str();
      xmlBuffer[1] = (memBufOS->getLaserdriverOutputBuffer())->str();
      xmlBuffer[2] = (memBufOS->getApvFecOutputBuffer())->str();
      xmlBuffer[3] = (memBufOS->getApvMuxOutputBuffer())->str();
      xmlBuffer[4] = (memBufOS->getDcuOutputBuffer())->str();

      if (display) {
	std::cout << xmlBuffer[0] << std::endl ;
	std::cout << xmlBuffer[1] << std::endl ;
	std::cout << xmlBuffer[2] << std::endl ;
	std::cout << xmlBuffer[3] << std::endl ;
	std::cout << xmlBuffer[4] << std::endl ;
	std::cerr << "Press enter to contine" ; getchar() ;
      }

      if (!downloadOnly) {
	// download of the FEC buffer
	DbFecAccess dbFecAccess (login,passwd,path) ;

	for (unsigned int i = 0 ; i < count ; i ++) {
	  
	  //std::cout << "Starting the download of the FEC partition " << partitionName << std::endl ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  oracle::occi::Clob *xmlClob = dbFecAccess.getXMLClob(partitionName) ;
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Retreive the CLOB from database took " << (endMillis-startMillis) << " ms" << std::endl ;
	  
	  if (display) XMLCommonFec::display(xmlClob) ;
	  
	  // Upload in a major version
	  //std::cout << "Starting the upload of the FEC partition " << partitionName << std::endl ;
	  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  dbFecAccess.setXMLClob(xmlBuffer, partitionName, (unsigned int)1);
	  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Upload of the FEC version for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;
	}
      }
      delete[] xmlBuffer;
      delete memBufOS;
    }
    catch (FecExceptionHandler &e) {
    
      std::cerr << "Error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;

      delete deviceFactory ;
      return -1 ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Oracle error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;

      delete deviceFactory ;
      return -1 ;
    }
  }

  // ----------------------------------------------------------------------------
  // Connection

  if (connectionFlag) {

    try {
//       // Download the values
//       unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//       ConnectionVector connections;
//       deviceFactory->getConnectionDescriptions (partitionName, connections);
//       unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
//       std::cout << std::dec << "Download of " << connections.size() << " connections from the database for partition " << partitionName << " took " << (endMillis-startMillis) << " ms" << std::endl ;

//       unsigned int vm,vn ;
//       deviceFactory->setConnectionDescriptions (connections,partitionName,&vm,&vn,true);
//       std::cout << "Upload of " << connections.size() << " connections done: version " << vm << "." << vn << std::endl ;

//       MemBufOutputSource memBufOS ( connections, true ) ;

//       if (display) std::cout << memBufOS.getOutputBuffer()->str() << std::endl ;

//       // Upload the buffer
//       DbConnectionsAccess *connectionAccess = (DbConnectionsAccess *)((ConnectionFactory *)deviceFactory)->getDatabaseAccess() ;
//       connectionAccess->setXMLClob (memBufOS.getOutputBuffer()->str(),partitionName,0);    

       std::string connectionClob = "<?xml version=\"1.0\"?><ROWSET><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>35</FEDCHANNEL><APVADDRESS>36</APVADDRESS><DCUHARDID>3669611</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>34</FEDCHANNEL><APVADDRESS>32</APVADDRESS><DCUHARDID>3669611</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>33</FEDCHANNEL><APVADDRESS>36</APVADDRESS><DCUHARDID>4061682</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>32</FEDCHANNEL><APVADDRESS>32</APVADDRESS><DCUHARDID>4061682</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>31</FEDCHANNEL><APVADDRESS>32</APVADDRESS><DCUHARDID>3866215</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>30</FEDCHANNEL><APVADDRESS>36</APVADDRESS><DCUHARDID>3866215</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>29</FEDCHANNEL><APVADDRESS>36</APVADDRESS><DCUHARDID>14155367</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>28</FEDCHANNEL><APVADDRESS>32</APVADDRESS><DCUHARDID>14155367</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>27</FEDCHANNEL><APVADDRESS>36</APVADDRESS><DCUHARDID>14613195</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>26</FEDCHANNEL><APVADDRESS>32</APVADDRESS><DCUHARDID>14417649</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>25</FEDCHANNEL><APVADDRESS>36</APVADDRESS><DCUHARDID>14417649</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION><RAWCONNECTION><FEDID>55</FEDID><FEDCHANNEL>24</FEDCHANNEL><APVADDRESS>32</APVADDRESS><DCUHARDID>14613195</DCUHARDID><ENABLED>T</ENABLED></RAWCONNECTION></ROWSET>" ;
       DbConnectionsAccess *connectionAccess = (DbConnectionsAccess *)((ConnectionFactory *)deviceFactory)->getDatabaseAccess() ;
       connectionAccess->setXMLClob (connectionClob,partitionName,0);
    }
    catch (FecExceptionHandler &e) {
    
      std::cerr << "Error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      
      delete deviceFactory ;
      return -1 ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Oracle error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      
      delete deviceFactory ;
      return -1 ;
    }
  }

  if (fedFlag) {
    try {
      // DbFedAccess::getXMLClob(std::string partitionName) ;
      DbFedAccess dbAccess(login, passwd, path) ;
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

      oracle::occi::Clob *xmlClob = NULL ;
      if (major == 0) 
	xmlClob = dbAccess.getXMLClob(partitionName) ;
      else 
	xmlClob = dbAccess.getXMLClobWithVersion(partitionName,major,minor,maskMajor,maskMinor) ;
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Download of FEDs for the partition " << partitionName << " took " << (endMillis-startMillis) << " ms (without XML parsing)" << std::endl ;
      if (display) XMLCommonFec::display(xmlClob);
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

      delete deviceFactory ;
      return -1 ;
    }
  }

  if (deviceFactory != NULL) delete deviceFactory ;

  return 0 ;
}
