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


#include "FecExceptionHandler.h" 
#include "TkDiagErrorAnalyser.h" 

/** -----------------------------------------------------------
 * To test the class
 */
void TestPrg ( TkDiagErrorAnalyser &tkDiagErrorAnalyser ) {

  bool testFec = true ;
  bool testFed = true ;
  unsigned int fedSoftId = 124, fedChannel = 72 ;
  unsigned int fedCrateNumber = 0, fedCrateId = 5 ;
  std::string fecHardId = "3020140B35928A" ; 
  unsigned int ringSlot = 7, ccuAddress = 96,i2cChannel = 24, i2cAddress = 36 ;
  unsigned int fecErrorCounter = 0, fedErrorCounter = 0, psuErrorCounter = 0 ;
  unsigned int detId = 436330536 ;


  if (testFec) {

    //std::string fecHardId = "3020140B35928A" ; // 6 APVs
    //unsigned int ringSlot = 7, ccuAddress = 2, i2cChannel = 19, i2cAddress = 0x20 ;
    
    //std::string fecHardId = "3020140B35928A" ; // 4 APVs
    //unsigned int ringSlot = 8, ccuAddress = 14,i2cChannel = 17, i2cAddress = 0x20 ;
    
    unsigned int fecErrorCounter = 0, fedErrorCounter = 0, psuErrorCounter = 0 ;
    // set one error
    try {
      tkDiagErrorAnalyser.setFecError(fecHardId, ringSlot, ccuAddress, i2cChannel, i2cAddress) ;
      
      try {
	std::cout << "on module 0x20:" << std::endl ;
	tkDiagErrorAnalyser.getFecModuleErrorCounter(fecHardId,ringSlot,ccuAddress,i2cChannel,
						     fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
	
	std::cout << "FEC error counter = " << fecErrorCounter << std::endl ;
	std::cout << "FED error counter = " << fedErrorCounter << std::endl ;
	std::cout << "PSU error counter = " << psuErrorCounter << std::endl ;
      }
      catch (std::string &e) {
	std::cerr << e << std::endl ;
      }
      
      try {
	std::cout << "on APV 0x20:" << std::endl ;
	tkDiagErrorAnalyser.getConnectionErrorCounter(fecHardId,ringSlot,ccuAddress,i2cChannel,i2cAddress,
						      fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
	
	std::cout << "FEC error counter = " << fecErrorCounter << std::endl ;
	std::cout << "FED error counter = " << fedErrorCounter << std::endl ;
	std::cout << "PSU error counter = " << psuErrorCounter << std::endl ;
      }
      catch (std::string &e) {
	std::cerr << e << std::endl ;
      }
      
      try {
	std::cout << "on APV 0x22:" << std::endl ;
	tkDiagErrorAnalyser.getConnectionErrorCounter(fecHardId,ringSlot,ccuAddress,i2cChannel,0x22,
						      fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
	
	std::cout << "FEC error counter = " << fecErrorCounter << std::endl ;
	std::cout << "FED error counter = " << fedErrorCounter << std::endl ;
	std::cout << "PSU error counter = " << psuErrorCounter << std::endl ;
      }
      catch (std::string &e) {
	std::cout << e << std::endl ;
      }
      
      try {
	std::cout << "on APV 0x24:" << std::endl ;
	tkDiagErrorAnalyser.getConnectionErrorCounter(fecHardId,ringSlot,ccuAddress,i2cChannel,0x24,
						      fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
	std::cout << "FEC error counter = " << fecErrorCounter << std::endl ;
	std::cout << "FED error counter = " << fedErrorCounter << std::endl ;
	std::cout << "PSU error counter = " << psuErrorCounter << std::endl ;
      }
      catch (std::string &e) {
	std::cerr << e << std::endl ;
      }
      
      // All connections
      ConnectionVector connections = tkDiagErrorAnalyser.getListOfModulesAsConnection() ;
      std::cout << "FEC: Number of modules " << connections.size() << " found in DB for " << tkDiagErrorAnalyser.getConnectionByFec().size() << " connections" << std::endl ;
      for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++) {
	unsigned int fecErrorCounter, fedErrorCounter, psuErrorCounter ;
	ConnectionDescription *connection = *it ;
	tkDiagErrorAnalyser.getFecModuleErrorCounter(connection->getFecHardwareId(),connection->getRingSlot(),connection->getCcuAddress(),
						     connection->getI2cChannel(),fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
	
	if (fecErrorCounter || fedErrorCounter || psuErrorCounter) {
	  std::cout << connection->getFecCrateId() << ":" << connection->getFecSlot() << ":" 
		    << connection->getRingSlot() << std::hex << ":0x" << connection->getCcuAddress() << ":0x"
		    << connection->getI2cChannel() << ":0x"<< connection->getApvAddress() << std::dec 
		    << " has " << fecErrorCounter << ":" << fedErrorCounter << ":" << psuErrorCounter << std::endl ;
	  
	  //unsigned int fecErrorCounter1, fedErrorCounter1, psuErrorCounter1 ;
	  //tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter1,fedErrorCounter1,psuErrorCounter1) ;
	}
      }
    }
    catch (std::string &e) {
      std::cerr << e << std::endl ;
    }
  }

  if (testFed) {
    // FED point of view

    try {
      tkDiagErrorAnalyser.setFedSoftIdError(fedSoftId, fedChannel);
      
      try {
	std::cout << "on the corresponding FEC:" << std::endl ;
	tkDiagErrorAnalyser.getConnectionErrorCounter(fecHardId,ringSlot,ccuAddress,i2cChannel,i2cAddress,
						      fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
	std::cout << "FEC error counter = " << fecErrorCounter << std::endl ;
	std::cout << "FED error counter = " << fedErrorCounter << std::endl ;
	std::cout << "PSU error counter = " << psuErrorCounter << std::endl ;
      }
      catch (std::string &e) {
	std::cerr << e << std::endl ;
      }

      try {
	std::cout << "on corresponding FED by crate and slot:" << std::endl ;
	tkDiagErrorAnalyser.getFedChannelErrorCounter( fedCrateNumber, fedCrateId, fedChannel,
							fecErrorCounter, fedErrorCounter, psuErrorCounter);

	std::cout << "FEC error counter = " << fecErrorCounter << std::endl ;
	std::cout << "FED error counter = " << fedErrorCounter << std::endl ;
	std::cout << "PSU error counter = " << psuErrorCounter << std::endl ;
      }
      catch (std::string &e) {
	std::cerr << e << std::endl ;
      }

      try {
	std::cout << "on corresponding DET ID:" << std::endl ;
	tkDiagErrorAnalyser.getDetIdErrorCounter( detId,
						  fecErrorCounter, fedErrorCounter, psuErrorCounter);

	std::cout << "FEC error counter = " << fecErrorCounter << std::endl ;
	std::cout << "FED error counter = " << fedErrorCounter << std::endl ;
	std::cout << "PSU error counter = " << psuErrorCounter << std::endl ;
      }
      catch (std::string &e) {
	std::cerr << e << std::endl ;
      }


      // All connections
      ConnectionVector connections = tkDiagErrorAnalyser.getListOfModulesAsConnection() ;
      std::cout << "FEC: Number of modules " << connections.size() << " found in DB for " << tkDiagErrorAnalyser.getConnectionByFec().size() << " connections" << std::endl ;
      for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++) {
	unsigned int fecErrorCounter, fedErrorCounter, psuErrorCounter ;
	ConnectionDescription *connection = *it ;
	tkDiagErrorAnalyser.getFecModuleErrorCounter(connection->getFecHardwareId(),connection->getRingSlot(),connection->getCcuAddress(),
						     connection->getI2cChannel(),fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
	
	if (fecErrorCounter || fedErrorCounter || psuErrorCounter) {
	  std::cout << connection->getFecCrateId() << ":" << connection->getFecSlot() << ":" 
		    << connection->getRingSlot() << std::hex << ":0x" << connection->getCcuAddress() << ":0x"
		    << connection->getI2cChannel() << ":0x"<< connection->getApvAddress() << std::dec 
		    << " has " << fecErrorCounter << ":" << fedErrorCounter << ":" << psuErrorCounter << std::endl ;
	  
	  //unsigned int fecErrorCounter1, fedErrorCounter1, psuErrorCounter1 ;
	  //tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter1,fedErrorCounter1,psuErrorCounter1) ;
	}
      }
    }
    catch (std::string &e) {
      std::cerr << e << std::endl ;
    }
  }
}

/** -----------------------------------------------------------
 */
int main ( int argc, char **argv ) {

  // Which partition you want to use ...
  std::string partitionName = "AgainOnTheRoadTest" ;
  bool testTest = false ;

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
    else if (param == "-help") {
      
      std::cout << argv[0] << std::endl ;
      std::cout << "\t" << "-partition <partition name>" << std::endl ;
      return 0 ;
    }
    else if (param == "-test") {

      testTest = true ;
    }
    else {

      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
    }
  }

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
  TkDiagErrorAnalyser *tkDiagErrorAnalyser ;
  try {
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    // create the database access
    tkDiagErrorAnalyser = new TkDiagErrorAnalyser ( partitionName ) ; //, login, passwd, path ) ;

    if (tkDiagErrorAnalyser->getConnectionNotIdentified() != 0) 
      std::cerr << "Found " << tkDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
    if (tkDiagErrorAnalyser->getPsuNotIdentified() != 0) 
      std::cerr << "Found " << tkDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;

    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Creation of the TkDiagErrorAnalyser " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "Cannot create the access to DB: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  // --------------------------------------------------------------------------------------------------------------------
  // Test program
  if (testTest) {
    TestPrg ( *tkDiagErrorAnalyser ) ;
  }
  else {

    // --------------------------------------------------------------------------------------------------------------------
    // Display all DET IDs with errors
    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::vector<unsigned int> listDetId = tkDiagErrorAnalyser->getListOfDetId() ;
      std::cout << "Number of det id " << listDetId.size() << " found in DB" << std::endl ;
      unsigned int i = 0 ;
      for (std::vector<unsigned int>::iterator it = listDetId.begin() ; it != listDetId.end() ; it ++) {
	unsigned int fecErrorCounter, fedErrorCounter, psuErrorCounter = 8 ;
	tkDiagErrorAnalyser->getDetIdErrorCounter(*it,fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
	if (fecErrorCounter || fedErrorCounter || psuErrorCounter)
	  std::cout << (*it) << " has " << fecErrorCounter << ":" << fedErrorCounter << ":" << psuErrorCounter << std::endl ;

	if (i < 10) { tkDiagErrorAnalyser->setDetIdError((*it)) ; i ++ ; }
      }
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Call took " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (std::string &e) {
      std::cerr << "Error during the operation " << e << std::endl ;
    }
  
    std::cout << "Press a key to continue" ; getchar() ;

    // --------------------------------------------------------------------------------------------------------------------
    // Display all errors by FEC modules
    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      ConnectionVector connections = tkDiagErrorAnalyser->getListOfModulesAsConnection() ;
      std::cout << "FEC: Number of modules " << connections.size() << " found in DB for  for " << tkDiagErrorAnalyser->getConnectionByFec().size() << " connections" << std::endl ;
      for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++) {
	unsigned int fecErrorCounter, fedErrorCounter, psuErrorCounter ;
	ConnectionDescription *connection = *it ;
	tkDiagErrorAnalyser->getFecModuleErrorCounter(connection->getFecHardwareId(),connection->getRingSlot(),connection->getCcuAddress(),
						      connection->getI2cChannel(),fecErrorCounter,fedErrorCounter,psuErrorCounter) ;

	if (fecErrorCounter || fedErrorCounter || psuErrorCounter) {
	  std::cout << connection->getFecCrateId() << ":" << connection->getFecSlot() << ":" 
		    << connection->getRingSlot() << std::hex << ":0x" << connection->getCcuAddress() << ":0x"
		    << connection->getI2cChannel() << ":0x"<< connection->getApvAddress() << std::dec 
		    << " has " << fecErrorCounter << ":" << fedErrorCounter << ":" << psuErrorCounter << std::endl ;
	
	  unsigned int fecErrorCounter1, fedErrorCounter1, psuErrorCounter1 ;
	  tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter1,fedErrorCounter1,psuErrorCounter1) ;
	}
      }
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Call took " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (std::string &e) {
      std::cerr << "Error during the operation " << e << std::endl ;
    }

    std::cout << "Press a key to continue" ; getchar() ;


    // --------------------------------------------------------------------------------------------------------------------
    // Display all errors by FED
    try {
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFed() ;
      std::cout << "FED: Number of connections " << connections.size() << " found in DB" << std::endl ;
      for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++) {
	unsigned int fecErrorCounter, fedErrorCounter, psuErrorCounter ;
	ConnectionDescription *connection = *it ;
	tkDiagErrorAnalyser->getFedChannelErrorCounter(connection->getFedCrateId(),connection->getFedSlot(),connection->getFedChannel(),
						       fecErrorCounter,fedErrorCounter,psuErrorCounter) ;

	if (fecErrorCounter || fedErrorCounter || psuErrorCounter) {
	  std::cout << connection->getFedCrateId() << ":" << connection->getFedSlot() << ":" << connection->getFedChannel()
		    << " has " << fecErrorCounter << ":" << fedErrorCounter << ":" << psuErrorCounter << std::endl ;
	}
      }
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Call took " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    catch (std::string &e) {
      std::cerr << "Error during the operation " << e << std::endl ;
    }
  }

  // ***************************************************************************************************************************
  // Delete the database access
  // ***************************************************************************************************************************
  try {     
    // this method handle the remove of the vectors
    delete tkDiagErrorAnalyser ;
  }  
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  
  return 0 ;
}
