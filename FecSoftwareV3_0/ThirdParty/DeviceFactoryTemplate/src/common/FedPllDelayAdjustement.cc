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

#include <fstream>

#include "tscTypes.h"      // hash table of devices and PIA descriptions
#include "keyType.h"       // FEC/ring/CCU/channel/address are managed in this file (not a class)
#include "deviceType.h"

#include "FecExceptionHandler.h" // exceptions
#include "DeviceFactory.h"       // devicefactory
#include "MemBufOutputSource.h"  // XML buffer

/** \param -partition <partition name>
 */
int main ( int argc, char **argv ) {

  // Which partition you want to use ...
  std::string partitionName = "" ;
  int latency = -1 ;
  std::string fecFile = "" ;
  std::string fedFile = "" ;
  bool dryRun = true;

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
    else if (param == "-dbupdate") {
       dryRun = false;
    }
    else if (param == "-latency") {
      if (i < argc) {
	latency = fromString<int>(argv[i+1]) ;
	i ++ ;
      }
    }
    else if (param == "-help") {

      std::cerr << argv[0] << std::endl ;
      std::cerr << "  -partition <partition name> -latency <value> -fecfile <fileName> -fedfile >fileName> [-dbupdate]" << std::endl ; 
      return 0 ;
    }
    else if (param == "-fecfile") {
      if (i < argc) {
	fecFile = std::string(argv[i+1]) ;
	i ++ ;
      }
    }
    else if (param == "-fedfile") {
      if (i < argc) {
	fedFile = std::string(argv[i+1]) ;
	i ++ ;
      }
    }
    else {

      std::cerr << "Error: Unknow parameter " << param << ": ignoring" << std::endl ;
    }
  }

  // ---------------------------------------------------------------------------------
  // No latency parameter
  if (latency < 0) {
    std::cerr << argv[0] << ": error in usage, latency value is mandatory" << std::endl ;
    std::cerr << "  -partition <partition name> -latency <value> -fecfile <fileName> -fedfile >fileName> [-dbupdate]" << std::endl ; 
    return 0 ;
  }

  if (partitionName.length() == 0) {
    std::cerr << argv[0] << ": specify the partition name" << std::endl ;
    std::cerr << "  -partition <partition name> -latency <value> -fecfile <fileName> -fedfile >fileName> [-dbupdate]" << std::endl ; 
    return 0 ;
  }

  if (fecFile.length() == 0) {
    std::cerr << argv[0] << ": specify a FEC file name for the PLL" << std::endl ;
    std::cerr << "  -partition <partition name> -latency <value> -fecfile <fileName> -fedfile >fileName> [-dbupdate]" << std::endl ; 
    return 0 ;
  }

  if (fedFile.length() == 0) {
    std::cerr << argv[0] << ": specify a FED file name for the delays" << std::endl ;
    std::cerr << "  -partition <partition name> -latency <value> -fecfile <fileName> -fedfile >fileName> [-dbupdate]" << std::endl ; 
    return 0 ;
  }

  std::cout << "Apply: " << std::endl ;
  std::cout << "\t" << latency << " on the APVs from partition " << partitionName << std::endl ;
  std::cout << "\t" << fecFile << " on the PLLs from partition " << partitionName << std::endl ;
  std::cout << "\t" << fedFile << " on the FEDs from partition " << partitionName << std::endl ;
  std::cout << "Press <enter> to continue" ; getchar() ;

  // ---------------------------------------------------------------------------------
  // access to the database
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
    // create the database access
    deviceFactory = new DeviceFactory ( login, passwd, path ) ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }

  // ***************************************************************************************************
  // FEC devices modification
  // ***************************************************************************************************

  // ---------------------------------------------------
  // read file to modify the PLL delay. The map only contains shifts, not absolute values !
  std::map<unsigned int, pllDescription *> mapPllDevices ;
  std::ifstream fecInput(fecFile.c_str());
  unsigned int fec, ring, ccu, channel, coarse, fine;
  while(fecInput >> fec >> ring >> ccu >> channel >> coarse >> fine) {
    pllDescription *pll = new pllDescription ( fec, ring, ccu, channel, 0x44, fine, coarse ) ;
    mapPllDevices[buildCompleteKey(fec,ring,ccu,channel,0x44)] = pll ;
  }
  fecInput.close();

  // ---------------------------------------------------
  // modify the devices
  try {
    // Retreive all devices to have a buffer to send to the database
    deviceVector mesDevices ;
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    deviceFactory->getFecDeviceDescriptions (partitionName, mesDevices) ; // retreive the parameters for the current version
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Found " << mesDevices.size() << " devices in " << (endMillis-startMillis) << " ms (including XML parsing)" << std::endl ;

    // Update the PLL and latency values
    for (deviceVector::iterator it = mesDevices.begin() ; it != mesDevices.end() ; it ++) {
      switch ((*it)->getDeviceType()) {
      case PLL: {
	pllDescription *pll = dynamic_cast<pllDescription *>(*it) ;
	if (mapPllDevices.find(pll->getKey()) != mapPllDevices.end()) {
          std::cout << "PLL " << pll->getKey() << " is now " 
                    << (unsigned int) pll->getDelayCoarse() << ", " << (unsigned int) pll->getDelayFine() 
                    << " and adding " << (unsigned int)mapPllDevices[pll->getKey()]->getDelayCoarse() << ", " 
                    << (unsigned int)mapPllDevices[pll->getKey()]->getDelayFine() << std::endl;
	  pll->setDelayCoarse(mapPllDevices[pll->getKey()]->getDelayCoarse()+pll->getDelayCoarse()
                               +(mapPllDevices[pll->getKey()]->getDelayFine()+pll->getDelayFine())/24) ;
	  pll->setDelayFine((mapPllDevices[pll->getKey()]->getDelayFine()+pll->getDelayFine())%24) ;
          std::cout << "setting PLL " << pll->getKey() << " to " 
                    << (unsigned int) pll->getDelayCoarse() << ", " << (unsigned int) pll->getDelayFine() << std::endl;
	}
	break ;
      }
      case APV25: {
	apvDescription *apv = dynamic_cast<apvDescription *>(*it) ;
	apv->setLatency(latency) ;
	break ;
      }
      default: // not a PLL and not an APV
	break ;
      }
    }

    // All devices modified, sent it to the database in a new minor version
    if(!dryRun) {
      unsigned int major, minor ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->setFecDeviceDescriptions (mesDevices, partitionName, &major, &minor) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Version " << major << "." << minor << " has been added to the database in " 
                << std::dec << (endMillis-startMillis) 
                << " ms, please check that the version is in the current state with the TkConfigurationDb" << std::endl; 
    } else {
      // to upload in file
      FecDeviceFactory fecDeviceFactory;
      fecDeviceFactory.setOutputFileName("output_dryRun.xml") ;
      fecDeviceFactory.setUsingFile() ;
      fecDeviceFactory.setFecDeviceDescriptions (mesDevices) ;
      std::cout << "Dry run: output only to output_dryRun.xml." << std::endl;
   }

    // Delete the devices allocated
    for (std::map<unsigned int, pllDescription *>::iterator it = mapPllDevices.begin() ; it != mapPllDevices.end() ; it ++) {
      delete it->second ;
    }
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
    return -1 ;
  }
  
  // ***************************************************************************************************
  // FED devices modification
  // ***************************************************************************************************

  // read the FED file (fedFile)
  // first pair is: fedid, fedchannel (from connection point of view => calculation to be done)
  // second pair is: coarse, fine (increment!!)
  std::map<std::pair<unsigned short, unsigned short>, std::pair<unsigned short,unsigned short> > fedChannelList ;
  std::ifstream fedInput(fedFile.c_str());
  unsigned int fedid, fedch, fedcoarse, fedfine;
  while(fedInput >> fedid >> fedch >> fedcoarse >> fedfine) {
    fedChannelList[std::make_pair(fedid,fedch)]=std::make_pair(fedcoarse,fedfine);
  }
  fedInput.close();

  try {
    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::vector<Fed9U::Fed9UDescription*> *fedVector = deviceFactory->getFed9UDescriptions(partitionName) ;
    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (fedVector != NULL)
      std::cout << "Found " << std::dec << fedVector->size() << " FEDs in database in " << (endMillis-startMillis) << " ms" << std::endl ;
    else
      std::cout << "Found " << std::dec << 0 << " FEDs in database in " << (endMillis-startMillis) << " ms" << std::endl ;

    // Any FED in the database
    if ( (fedVector != NULL) && (fedVector->size() > 0) ) {

      // Build a map of Fed9UDescription
      std::map<unsigned short, Fed9U::Fed9UDescription *> mapFed ;
      for (std::vector<Fed9U::Fed9UDescription *>::iterator it = fedVector->begin() ; it != fedVector->end() ; it ++) 
	mapFed[(*it)->getFedId()] = *it ;

      // Loop on the FED channel list provided by the file
      int32_t minCoarse = 40;
      for (std::map<std::pair<unsigned short, unsigned short>, std::pair<unsigned short,unsigned short> >::iterator it = fedChannelList.begin() ; it != fedChannelList.end() ; it ++) {

	std::pair<unsigned short, unsigned short> fedChannel = it->first ;
	std::pair<unsigned short, unsigned short> delays = it->second ;

	Fed9U::Fed9UDescription *fed9U = mapFed[fedChannel.first] ;
	unsigned int myFedChannel = fedChannel.second ; // connection point of view 
        unsigned short DesiredSkew = delays.first * 25 - delays.second;
        Fed9U::Fed9UAddress channelAddress(myFedChannel); // note the internal channel number is sent 0-95 top to bottom!
        // get the original delay settings
        int32_t xmlChannelFineDelay = fed9U->getFineDelay(channelAddress);
        int32_t xmlChannelCoarseDelay = fed9U->getCoarseDelay(channelAddress);
        // Now we have to calculate the new skew values to load to the fed for this channel.
        // We have to be careful since fine delay for each channel is applied in the opposite sense to the desired Skew.
        int32_t coarseDelay  = xmlChannelCoarseDelay + ( ( ( (DesiredSkew - xmlChannelFineDelay)) <= 0 ) ? 0 : (DesiredSkew - xmlChannelFineDelay) / 25 + 1);
        int32_t fineDelay = ( (xmlChannelFineDelay - DesiredSkew%25) >= 0 ) ? (xmlChannelFineDelay - DesiredSkew % 25 ): ( xmlChannelFineDelay + (25 - DesiredSkew % 25) );
        std::cout << "FED " << std::dec << fedChannel.first << "... Adding skew on channel " << myFedChannel 
                  << ", of " << DesiredSkew <<  ", coarse Delay on fed = " << coarseDelay 
                  << ", fine delay on fed = " << fineDelay << std::endl;
        minCoarse = minCoarse<coarseDelay ? minCoarse : coarseDelay;
       	if(fed9U) {
          fed9U->setDelay( channelAddress, coarseDelay, fineDelay ); // apply the calculated skew to the fed channel
	}
      }
      // Final loop on FED devices to reduce as much as possible the FED coarse delay
      for (std::map<std::pair<unsigned short, unsigned short>, std::pair<unsigned short,unsigned short> >::iterator it = fedChannelList.begin() ; it != fedChannelList.end() ; it ++) {
	Fed9U::Fed9UDescription *fed9U = mapFed[it->first.first] ;
        if(fed9U) {
          Fed9U::Fed9UAddress channelAddress(it->first.second); 
          fed9U->setDelay( channelAddress, fed9U->getCoarseDelay(channelAddress) - minCoarse, fed9U->getFineDelay(channelAddress) ); 
        }
      }
    }
    // Upload it in a major version
    if(!dryRun) {
      unsigned short major, minor ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceFactory->setFed9UDescriptions(*fedVector,partitionName,&major,&minor,1);
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "Upload FED major version: " << major << "." << minor << " in " 
                << std::dec << (endMillis-startMillis) << " ms" << std::endl ;
    } else {
      std::cout << "Dry run. No update of FED delays." << std::endl;
    }
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "getFed9UDescriptions: " << e.what() << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "getFed9UDescriptions: " << e.what() << std::endl ;
  }
  catch (Fed9U::Fed9UDeviceFactoryException &e) {
    std::cerr << "getFed9UDescriptions: Error during the FED download or upload" ;
    std::cerr << e.what() << std::endl ;
  }
  catch (ICUtils::ICException &e) {
    std::cerr << "getFed9UDescriptions: Error during the FED download or upload" ;
    std::cerr << e.what() << std::endl ;
  }
  catch (exception &e) {
    std::cerr << "getFed9UDescriptions: Error during the FED download or upload" ;
    std::cerr << e.what() << std::endl ;
  }
  catch (...) { //Catch all types of exceptions
    std::cerr << "ERROR: unknown exception catched" << std::endl ;
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
