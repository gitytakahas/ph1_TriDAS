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

/** -----------------------------------------------------------
 */
int main ( int argc, char **argv ) {

  // Which partition you want to use ...
  std::string partitionName = "AgainOnTheRoadTest" ;
  unsigned int runNumber = 0, runMode = 0 ;
  std::string comment = "Inserted by hand" ;

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
      else if (param == "-comment") {
	comment = param ;
      }
      else 
	std::cerr << "Error: you must specify the run mode with the option -runMode" << std::endl ;
    }
    else if (param == "-help") {

      std::cout << argv[0] << std::endl ;
      std::cout << "\t" << "-partition <partition name> -runNumber <value> -runMode <name>" << std::endl ; 
      std::cout << "\t\t" << "runMode: PHYSIC, PEDESTAL, CALIBRATION, CALIBRATION_DECO, GAINSCAN, TIMING, LATENCY, DELAY, PHYSIC10, CONNECTION, DELAY_TTC, TIMING_FED, BARE_CONNECTION, VPSPSCAN, SCOPE, FAST_CONNECTION DELAY_LAYER PHYSIC_ZERO_SUPPRESSION" << std::endl ;
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
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    deviceFactory->setRun(partitionName,runNumber,runMode,1,comment) ;
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Set run took " << (endMillis-startMillis) << " ms" << std::endl ;
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
