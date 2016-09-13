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

#include "DeviceFactory.h"
#include "MemBufOutputSource.h"

int main ( int argc, char **argv ) {

  unsigned long startMillis, endMillis ;
  std::string partitionName = "AgainOnTheRoadTest" ;
  bool major=false;
  bool minor=false;
  bool piaUpload=false;

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
    else if (param == "-major") {
      major=true ;
    }
    else if (param == "-minor") {
      minor=true ;
    }
    else if (param == "-piaUpload") {
      piaUpload=true ;
    }    
    else
      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
  }

  try {

    // Login / password / path 
    std::string login="nil", passwd="nil", path="nil" ;
    DbAccess::getDbConfiguration (login, passwd, path) ;

    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }

    // -------------------------------------------------------
    // Donwload the devices and PIA to have XML buffer
    DeviceFactory deviceFactory( login, passwd, path ) ;

    // PIA
    piaResetVector piaDevices ;
    //startMillis = XMLPlatformUtils::getCurrentMillis();
    deviceFactory.getPiaResetDescriptions (partitionName, piaDevices) ; // retreive the parameters for the current version
    //endMillis = XMLPlatformUtils::getCurrentMillis();
    //std::cout << "Found " << piaDevices.size() << " PIA reset in " << (endMillis-startMillis) << " ms (including XML parsing)" << std::endl ;
    //startMillis = XMLPlatformUtils::getCurrentMillis();
    MemBufOutputSource memBufOSPia( piaDevices, true );
    //endMillis = XMLPlatformUtils::getCurrentMillis();
    //std::cout << "Buffer of XML buffer created in " << (endMillis-startMillis) << " ms" << std::endl ;
    std::string xmlPiaBuffer = (memBufOSPia.getPiaResetOutputBuffer())->str();

    // FEC
    // Retreive all devices to have a buffer to send to the database
    deviceVector mesDevices ;
    //startMillis = XMLPlatformUtils::getCurrentMillis();
    deviceFactory.getFecDeviceDescriptions (partitionName, mesDevices) ; // retreive the parameters for the current version
    //endMillis = XMLPlatformUtils::getCurrentMillis();
    //std::cout << "Found " << mesDevices.size() << " devices in " << (endMillis-startMillis) << " ms (including XML parsing)" << std::endl ;
      
    //startMillis = XMLPlatformUtils::getCurrentMillis();
    MemBufOutputSource memBufOS ( mesDevices, true);
    //endMillis = XMLPlatformUtils::getCurrentMillis();
    // std::cout << "Buffer of XML buffer created in " << (endMillis-startMillis) << " ms" << std::endl ;
      
    std::string *xmlBuffer = new std::string[5];
    xmlBuffer[0] = (memBufOS.getPllOutputBuffer())->str();
    xmlBuffer[1] = (memBufOS.getLaserdriverOutputBuffer())->str();
    xmlBuffer[2] = (memBufOS.getApvFecOutputBuffer())->str();
    xmlBuffer[3] = (memBufOS.getApvMuxOutputBuffer())->str();
    xmlBuffer[4] = (memBufOS.getDcuOutputBuffer())->str();

    // -------------------------------------------------------
    // Creation of the database access
    DbPiaResetAccess dbPiaResetAccess (login,passwd,path) ;
    DbFecAccess dbFecAccess (login,passwd,path) ;

    // -----------------------------------------------------------------
    // 100 upload for 10 download
    unsigned int maxUpload = 1 ;
    if (major || minor) maxUpload = 100 ;
    for (unsigned int uploadI = 0 ; uploadI < maxUpload ; uploadI ++) {

      for (unsigned int downloadI = 0 ; downloadI < 10 ; downloadI ++) {

	// -----------------------------------------------------------------
	// Download the FEC and display it
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	oracle::occi::Clob *xmlClob = dbFecAccess.getXMLClob(partitionName) ;
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	if (xmlClob != NULL)
	  std::cerr << "FEC:" << std::dec << downloadI << ":" << (endMillis-startMillis) << std::endl ;
	else
	  std::cerr << "FEC:" << std::dec << downloadI << ":" << "ERROR" << std::endl ;
      }

      for (unsigned int downloadI = 0 ; downloadI < 10 ; downloadI ++) {

	// -----------------------------------------------------------------
	// Download the PIA and display it
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	oracle::occi::Clob *xmlClob = dbPiaResetAccess.getXMLClob(partitionName) ;
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	if (xmlClob != NULL)
	  std::cerr << "PIA:" << std::dec << downloadI << ":" << (endMillis-startMillis) << std::endl ;
	else
	  std::cerr << "PIA:" << std::dec << downloadI << ":" << "ERROR" << std::endl ;
      }

      // -----------------------------------------------------------------------------------------------
      // Upload
      if (major) {
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	dbFecAccess.setXMLClob(xmlBuffer, partitionName, (unsigned int)1);
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	std::cerr << "Major:" << std::dec << uploadI << ":" << (endMillis-startMillis) << " ms" << std::endl ;
      }
      if (minor) {
	startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	dbFecAccess.setXMLClob(xmlBuffer, partitionName, (unsigned int)0);
	endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	std::cerr << "Minor:" << std::dec << uploadI << ":" << (endMillis-startMillis) << " ms" << std::endl ;
      }
    }
    return 0 ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "FecExceptionHandler:  " << e.what() << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "SQLException: " << e.what() << std::endl ;

  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
    std::cerr << "Fed9UDeviceFactoryException: " << e.what() << std::endl ;
  }
  catch (ICUtils::ICException &e) {
    std::cerr << "ICException: " << e.what() << std::endl ;
  }
  catch (std::exception &e) {
    std::cerr << "Exception " << e.what() << std::endl ;
  }
  catch (...) {
    std::cerr << "Unknown Exception" << std::endl ;
  }

  return -1 ;  
}
