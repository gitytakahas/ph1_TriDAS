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
#ifndef FECDETECTIONUPLOAD_H
#define FECDETECTIONUPLOAD_H

#include <iostream>

#include "stringConv.h"
#include "HashTable.h"
#include "deviceType.h"
#include "keyType.h"

#include "FecExceptionHandler.h"

#include "FecAccess.h"
#include "FecAccessManager.h"

#include "deviceType.h"
#include "TkRingDescription.h"

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
#include "FecVmeRingDevice.h"
#endif

#include "ErrorReportLogger.h"

#define NOFECRING 0xFFFFFFFF

/** Class FecDetectionUpload
 * This class detects the FEC into a crate and upload the definition in a file or in the TK configuration database.
 * The creation of the hardware access (FecAccess object) has been done before and the system will "just" detect the devices
 */
class FecDetectionUpload {

  // private:

 public:

  /** Check if all the devices detected are correct / the module
   * \param deviceList - list of indexes
   * \param moduleCorrect - the modules with no error
   * \param moduleIncorrect - the modules which have problem with one of the devices DCU, APV
   * \param moduleCannotBeUsed - the modules which have problem with or the AOH, PLL or MUX
   * \param dohDcuCcu - the index of all the DCU on CCU and the DOH
   * \param deviceMissing - missing devices (true = missing)
   * \param subDetector - can be UNKNOWN, TIB, TOB, TEC. On this value the DCU on CCU is checked correctly with the following definition:
   * <lu>
   * <li> TEC, TIB = 0x10 for all DCU on CCU
   * <li> TOB = 0x1E for all DCU on CCU
   * <ul>
   * \param displayMessage - display the messages on the screen
   */
  static bool checkDevices ( std::list<keyType> deviceList, 
			     std::list<keyType> &moduleCorrect, 
			     std::list<keyType> &moduleIncorrect, 
			     std::list<keyType> &moduleCannotBeUsed, 
			     std::list<keyType> &dcuDohOnCcu, 
			     Sgi::hash_map<keyType, bool> &deviceMissing,
			     std::string subDetector,
			     bool displayMessage = false ) {

#define FECDETECTIONUPLOADUNKNOWNSUBDETECTOR "UNKNOWN"

    // this information is used to determine the DCU on CCU i2c channel
    if ( (subDetector != "TOB") && 
	 (subDetector != "TIB") && (subDetector != "TID") &&
	 (subDetector != "TEC-") && (subDetector != "TEC+") && (subDetector != "TEC") ) {
      subDetector = FECDETECTIONUPLOADUNKNOWNSUBDETECTOR ;
    }
    else {
      if (subDetector == "TID") subDetector = "TIB" ; 
      if ( (subDetector == "TEC+") || (subDetector == "TEC-") ) subDetector = "TEC" ;
    }

    //#define TEST
#ifdef TEST
/*     * Module on Fec 0x6 Ring 0x0 Ccu 0x60 I2c channel 0x19 I2c address 0x0 */
/*                 Missing APV 0x20                                           */
/*                 Missing APV 0x25                                           */
/*     * Module on Fec 0x6 Ring 0x0 Ccu 0x60 I2c channel 0x13 I2c address 0x0 */
/*                 Missing AOH                                                */
/*     * Module on Fec 0x6 Ring 0x0 Ccu 0x60 I2c channel 0x1a I2c address 0x0 */
/*                 Missing PLL                                                */
/*     * Module on Fec 0x6 Ring 0x0 Ccu 0x60 I2c channel 0x17 I2c address 0x0 */
/*                 Missing APV 0x21                                           */
/*                 Missing DCU                                                */
/*     * Module on Fec 0x6 Ring 0x0 Ccu 0x60 I2c channel 0x14 I2c address 0x0 */
/*                 Missing DCU                                                */
/*                 Missing APV MUX                                            */
/*     * Module on Fec 0x6 Ring 0x0 Ccu 0x60 I2c channel 0x11 I2c address 0x0 */
#endif

    // Make the analysis of the module
    Sgi::hash_map<keyType, bool *> trackerModule ;
    bool *module ;
    
    // Fill a table with all devices
    for (std::list<keyType>::iterator p=deviceList.begin();p!=deviceList.end();p++) {
      
      keyType index = *p ;
      
      char msg[100] ;
      decodeKey (msg, index) ;

      keyType indexChannel = buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),getChannelKey(index),0) ;
      if (trackerModule.find(indexChannel) == trackerModule.end()) {
	trackerModule[indexChannel] = new bool[12] ;
	module = trackerModule[indexChannel] ;
	for (int i = 0 ; i < 12 ; i ++) module[i] = false ;
      }
      else module = trackerModule[indexChannel] ;
      
      switch (getAddressKey(index)) {
      case DCUADDRESS:
#ifdef TEST
	if ( (getChannelKey(indexChannel) != 0x17) && (getChannelKey(indexChannel) != 0x14) )
#endif
	module[10] = true ;
	//std::cout << "DCU " << msg << " found" << std::endl ;
	break ;
      case 0x20:
#ifdef TEST
	if (getChannelKey(indexChannel) != 0x19) 
#endif
	module[0] = true ;
	//std::cout << "APV " << msg << " found" << std::endl ;
	break ;
      case 0x21:
#ifdef TEST
	if (getChannelKey(indexChannel) != 0x17) 
#endif
	module[1] = true ;
	//std::cout << "APV " << msg << " found" << std::endl ;
	break ;
      case 0x22:
	module[2] = true; 
	//std::cout << "APV " << msg << " found" << std::endl ;
	break ;
      case 0x23:
	module[3] = true ;
	//std::cout << "APV " << msg << " found" << std::endl ;
	break ;
      case 0x24:
	module[4] = true; 
	//std::cout << "APV " << msg << " found" << std::endl ;
	break ;
      case 0x25:
#ifdef TEST
	if (getChannelKey(indexChannel) != 0x19) 
#endif
	module[5] = true ;
	//std::cout << "APV " << msg << " found" << std::endl ;
	break ;
      case 0x43:
#ifdef TEST
	if (getChannelKey(indexChannel) != 0x14) 
#endif
	module[6] = true ;
	//std::cout << "MUX " << msg << " found" << std::endl ;
	break ;
      case 0x44:
#ifdef TEST
	if (getChannelKey(indexChannel) != 0x1a) 
#endif
	module[7] = true ;
	//std::cout << "PLL " << msg << " found" << std::endl ;
	break ;
      case 0x60:
#ifdef TEST
	if (getChannelKey(indexChannel) != 0x13) 
#endif
	module[8] = true ;
	//std::cout << "AOH " << msg << " found" << std::endl ;
	break ;
      case DOHI2CADDRESS:
	module[9] = true ;
	//std::cout << "DOH " << msg << " found" << std::endl ;
	break ;
      }
    }
    
    // Check the devices
    for (Sgi::hash_map<keyType, bool *>::iterator p=trackerModule.begin();p!=trackerModule.end();p++) {
      
      keyType indexF = p->first ;
      module = p->second ;
      if (module != NULL) {
	  
	// 4 APVs
	bool apv4F = module[0] && module[1] && module[4] && module[5] ;
	bool pllF  = module[7] ;
	bool muxF  = module[6] ;
	bool aohF  = module[8] ;
	bool dohF  = module[9] ;
	bool dcu0F  = module[10] ;
      
	bool apv6F  = false ;
	if (module[2] || module[3]) apv6F = true ;
	bool inserted = false ;
	
	// Is it a module
	if (apv6F || apv4F || pllF || muxF || aohF) {
	  bool error = false ;
	  if (displayMessage)
	    std::cout << "Found a module on FEC " << std::dec << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << std::hex << getCcuKey(indexF) << " channel 0x" << std::hex << getChannelKey(indexF) << std::dec << std::endl ;

	  // Check each part
	  if (!module[0]) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss an APV at address 0x20" << std::endl ; 
	    // Insert in the list
	    if (!inserted) {
	      if (apv6F)       // 6 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x22)) ; 
	      else             // 4 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x20)] = true ;
	    error = true ;
	  }
	  if (!module[1]) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss an APV at address 0x21" << std::endl ; 
	    if (!inserted) {
	      if (apv6F)      // 6 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x22)) ; 
	      else            // 4 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x21)] = true ;
	    error = true ;
	  }
	  if (apv6F && !module[2]) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss an APV at address 0x22" << std::endl ; 
	    if (!inserted) {
	      if (apv6F)     // 6 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x22)) ; 
	      else           // 4 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x22)] = true ;
	    error = true ;
	  }
	  if (apv6F && !module[3]) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss an APV at address 0x23" << std::endl ; 
	    if (!inserted) {
	      if (apv6F)         // 6 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x22)) ; 
	      else               // 4 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x23)] = true ;
	    error = true ;
	  }
	  if (!module[4]) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss an APV at address 0x24" << std::endl ; 
	    if (!inserted) {
	      if (apv6F)         // 6 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x22)) ; 
	      else               // 4 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x24)] = true ;
	    error = true ;
	  }
	  if (!module[5]) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss an APV at address 0x25" << std::endl ;
	    if (!inserted) {
	      if (apv6F)         // 6 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x22)) ; 
	      else               // 4 APVs module
		moduleIncorrect.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x25)] = true ;
	    error = true ;
	  }
	  if (!muxF) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss a  MUX at address 0x43" << std::endl ;
	    if (!inserted) {
	      if (apv6F)         // 6 APVs module
		moduleCannotBeUsed.push_back(indexF | setAddressKey(0x22)) ; 
	      else               // 4 APVs module
		moduleCannotBeUsed.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x43)] = true ;
	    error = true ;
	  }
	  if (!pllF) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss a  PLL at address 0x44" << std::endl ;
	    if (!inserted) {
	      if (apv6F)         // 6 APVs module
		moduleCannotBeUsed.push_back(indexF | setAddressKey(0x22)) ; 
	      else               // 4 APVs module
		moduleCannotBeUsed.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x44)] = true ;
	    error = true ;
	  }
	  if (!aohF) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss an AOH at address 0x60" << std::endl ;
	    if (!inserted) {
	      if (apv6F)         // 6 APVs module
		moduleCannotBeUsed.push_back(indexF | setAddressKey(0x22)) ; 
	      else               // 4 APVs module
		moduleCannotBeUsed.push_back(indexF | setAddressKey(0x20)) ; 
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x60)] = true ;
	    error = true ;
	  }
	  if (!dcu0F) {
	    if (displayMessage)
	      std::cout << "\t" << "Miss a  DCU at address 0x0" << std::endl ;
	    if (!inserted) {
	      if (apv6F) {        // 6 APVs module
		moduleCannotBeUsed.push_back(indexF | setAddressKey(0x22)) ; 
		//moduleIncorrect.push_back(indexF | setAddressKey(0x22)) ; 
	      }
	      else {              // 4 APVs module
		moduleCannotBeUsed.push_back(indexF | setAddressKey(0x20)) ; 
		//moduleIncorrect.push_back(indexF | setAddressKey(0x20)) ; 
	      }
	      inserted = true ; 
	    }
	    deviceMissing[indexF|setAddressKey(0x0)] = true ;
	    error = true ;
	  }
	  
	  if (!error) {
	    if (apv6F)         // 6 APVs module
	      moduleCorrect.push_back(indexF | setAddressKey(0x22)) ; 
	    else               // 4 APVs module
	      moduleCorrect.push_back(indexF | setAddressKey(0x20)) ; 
	  }
	}
	else {
	  // Is it a DCU on CCU
	  if ( dcu0F && !apv6F && !apv4F && !pllF && !muxF && !aohF ) {
	    
	    std::ostringstream outputDcu ;
	    bool dcuOnCcuTag = true ;
	    if (subDetector != FECDETECTIONUPLOADUNKNOWNSUBDETECTOR) {
	      // TIB DCU on CCU is 0x10  
	      if ( (subDetector == "TIB") && (getChannelKey(indexF) != 0x10) ) {
		dcuOnCcuTag = false ;
	      }
	      // TOB DCU on CCU is 0x1E
	      else if ( (subDetector == "TOB") && (getChannelKey(indexF) != 0x1E) ) {
		dcuOnCcuTag = false ;
	      }
	      // TEC DCU on CCU is 0x10
	      else if ( (subDetector == "TEC") && (getChannelKey(indexF) != 0x10) ) {
		dcuOnCcuTag = false ;
	      }
	    }

	    // It is a a DCU on CCU or the subdetector is not set
	    if (dcuOnCcuTag) {

	      // getchar() ;

	      if (displayMessage) {
		std::cout << "Found a DCU on CCU on FEC " << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << std::hex << getCcuKey(indexF) << " channel 0x" << std::hex << getChannelKey(indexF) << std::dec << std::endl;
		
	      }
	      //if (!inserted) { 
	      dcuDohOnCcu.push_back(indexF) ; 
	      //inserted = true ; }
	    }
	    // it is a module with all devices faulty
	    else {
	      if (displayMessage) {
		std::cout << "Found a module on FEC " << std::dec << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << std::hex << getCcuKey(indexF) << " channel 0x" << std::hex << getChannelKey(indexF) << std::dec << std::endl ;
		std::cout << "\t" << "Miss an APV at address 0x20" << std::endl ;
		std::cout << "\t" << "Miss an APV at address 0x21" << std::endl ; 
		std::cout << "\t" << "Miss an APV at address 0x24" << std::endl ; 
		std::cout << "\t" << "Miss an APV at address 0x25" << std::endl ; 
		std::cout << "\t" << "Miss a  MUX at address 0x43" << std::endl ;
		std::cout << "\t" << "Miss a  PLL at address 0x44" << std::endl ;
		std::cout << "\t" << "Miss an AOH at address 0x60" << std::endl ;
	      }
	      moduleCannotBeUsed.push_back(indexF | setAddressKey(0x20)) ; 
	    }
	  }
	  // Is it a DOH
	  if (dohF) {
	    //if (!inserted) { 
	    dcuDohOnCcu.push_back(indexF | setAddressKey(DOHI2CADDRESS)) ; 
	    //inserted = true ; }
	  }
	}
      }
    }

    // sort all the devices
    deviceList.sort() ; moduleCorrect.sort() ; moduleIncorrect.sort() ; 
    moduleCannotBeUsed.sort() ; dcuDohOnCcu.sort() ; 

    // Delete the hash_map
    for (Sgi::hash_map<keyType, bool *>::iterator p=trackerModule.begin();p!=trackerModule.end();p++) delete[] p->second ;

    return (true) ;
  }
  
  /** Set the DCU type for the DCUs
   * \warning this method also assign the DCU type of each DCU (mainly change the DCU type of the DCU on CCU)
   */
  static void setDcuType ( deviceVector &vDevice, std::list<keyType> dcuDohOnCcu ) {
    
    deviceVector dcuDevices = FecFactory::getDeviceFromDeviceVector(vDevice, DCU) ;
    for (std::list<keyType>::iterator it = dcuDohOnCcu.begin() ; it != dcuDohOnCcu.end() ; it ++) {
      
      keyType index = (*it) ;
      if (getAddressKey(index) == DCUADDRESS) {
	
	for (deviceVector::iterator itDcu = vDevice.begin() ; itDcu != vDevice.end() ; itDcu ++) {
	  if ( ((*itDcu)->getDeviceType() == DCU) && (index == (*itDcu)->getKey()) ) {
	    dcuDescription *dcuD = dynamic_cast<dcuDescription *>(*itDcu) ;
	    dcuD->setDcuType(std::string(DCUCCU)) ;
	  }
	}
      }
    }
  }

  /** Check if a vector of device descriptions has all the devices needed for each module
   * \param vDevices - vector of device description
   * \param moduleCorrect - the modules with no error
   * \param moduleIncorrect - the modules which have problem with one of the devices DCU, APV
   * \param moduleCannotBeUsed - the modules which have problem with or the AOH, PLL or MUX
   * \param dohDcuCcu - the index of all the DCU on CCU and the DOH
   * \param deviceMissing - missing devices (true = missing)
   * \param subDetector - can be UNKNOWN, TIB, TOB, TEC. On this value the DCU on CCU is checked correctly with the following definition:
   * <lu>
   * <li> TEC, TIB = 0x10 for all DCU on CCU
   * <li> TOB = 0x1E for all DCU on CCU
   * <ul>
   * \param displayMessage - display the messages on the screen
   * \warning this method also assign the DCU type of each DCU (mainly change the DCU type of the DCU on CCU)
   */
  static bool checkDevices ( deviceVector &vDevice, 
			     std::list<keyType> &moduleCorrect, 
			     std::list<keyType> &moduleIncorrect, 
			     std::list<keyType> &moduleCannotBeUsed, 
			     std::list<keyType> &dcuDohOnCcu, 
			     Sgi::hash_map<keyType, bool> &deviceMissing,
			     std::string subDetector,
			     bool displayMessage = false ) {
    
    std::list<keyType> deviceList ;
    for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {
      
      deviceDescription *deviced = *device ;
      deviceList.push_back (deviced->getKey()) ;
    }
    
    bool returnFlag = checkDevices (deviceList, moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceMissing, subDetector, displayMessage) ;
    setDcuType ( vDevice, dcuDohOnCcu ) ;
    
    return (returnFlag) ;
  }

  /** Check if a vector of device descriptions has all the devices needed for each module
   * \param vDevices - vector of device description
   * \param withDeviceErrors - return the list of the index where an AOH or a MUX or a PLL is missing
   * \param dumpFile - if the dumpFile was true and the output is false then the files was not created
   * \param subDetector - can be UNKNOWN, TIB, TOB, TEC. On this value the DCU on CCU is checked correctly with the following definition:
   * <lu>
   * <li> TEC, TIB = 0x10 for all DCU on CCU
   * <li> TOB = 0x1E for all DCU on CCU
   * <ul>
   */
  static bool checkDevices ( deviceVector vDevice,
			     std::string subDetector,
			     bool displayMessage = true ) {
    
    std::list<keyType> moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceList ;
    Sgi::hash_map<keyType, bool> deviceMissing ;
    for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {
      
      deviceDescription *deviced = *device ;
      deviceList.push_back (deviced->getKey()) ;
    }

    bool returnFlag = checkDevices (deviceList, moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceMissing, subDetector, displayMessage) ;
    setDcuType ( vDevice, dcuDohOnCcu ) ;

    return (returnFlag) ;
  }

  /** Check if a vector of device descriptions has all the devices needed for each module
   * \param vDevices - vector of device description
   * \param withDeviceErrors - return the list of the index where an AOH or a MUX or a PLL is missing
   * \param dumpFile - if the dumpFile was true and the output is false then the files was not created
   * \param subDetector - can be UNKNOWN, TIB, TOB, TEC. On this value the DCU on CCU is checked correctly with the following definition:
   * <lu>
   * <li> TEC, TIB = 0x10 for all DCU on CCU
   * <li> TOB = 0x1E for all DCU on CCU
   * <ul>
   */
  static bool checkDevices ( std::list<keyType> deviceList,
			     std::string subDetector,
			     bool displayMessage = true ) {

    std::list<keyType> moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu ;
    Sgi::hash_map<keyType, bool> deviceMissing ;
    bool returnFlag = checkDevices (deviceList, moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceMissing, subDetector, displayMessage) ;
    //setDcuType ( vDevice, dcuDohOnCcu ) ;

    return (returnFlag) ;
  }

  /** Dump the information on screen and/or in file
   * \param 
   */
  static void dumpModuleScreenFile ( std::list<keyType> moduleList, 
				     Sgi::hash_map<keyType, bool> deviceError,
				     Sgi::hash_map<keyType, bool> deviceMissing,
				     bool displayMessage = false,
				     std::ofstream *outputStream = NULL ) {

    // Number of each type
    unsigned int module4Number = 0, module6Number = 0, dcuNumber = 0, dohNumber = 0 ;
    
    for (std::list<keyType> ::iterator it = moduleList.begin() ; it != moduleList.end() ; it ++) {
	  
      keyType index = getFecRingCcuChannelKey((*it)) ;
      unsigned int address = getAddressKey((*it)) ;
      
      // Check if it is a module
      if (address == 0x20 | address == 0x22) {
	
	char msg[80] ; decodeKey(msg,index) ;
	if (address == 0x20) {
	  module4Number ++ ;
	  if (displayMessage) std::cout << "\tModule with 4 APVs on " << msg << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\tModule with 4 APVs on " << msg << std::endl ;
	}
	else {
	  module6Number ++ ;
	  if (displayMessage) std::cout << "\tModule with 6 APVs on " << msg << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\tModule with 6 APVs on " << msg << std::endl ;
	}
	
	if (deviceError[index|setAddressKey(0x20)]) {
	  if (displayMessage) std::cout << "\t\tAPV 0x20 access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tAPV 0x20 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x20)]) {
	  if (displayMessage) std::cout << "\t\tMissing APV 0x20" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing APV 0x20" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x21)]) {
	  if (displayMessage) std::cout << "\t\tAPV 0x21 access problem" << std::endl ;
	if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tAPV 0x21 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x21)]) {
	  if (displayMessage) std::cout << "\t\tMissing APV 0x21" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing APV 0x21" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x22)]) {
	  if (displayMessage) std::cout << "\t\tAPV 0x22 access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tAPV 0x22 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x22)]) {
	  if (displayMessage) std::cout << "\t\tMissing APV 0x22" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing APV 0x22" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x23)]) {
	  if (displayMessage) std::cout << "\t\tAPV 0x23 access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tAPV 0x23 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x23)]) {
	  if (displayMessage) std::cout << "\t\tMissing APV 0x23" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing APV 0x23" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x24)]) {
	  if (displayMessage) std::cout << "\t\tAPV 0x24 access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tAPV 0x24 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x24)]) {
	  if (displayMessage) std::cout << "\t\tMissing APV 0x24" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing APV 0x24" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x25)]) {
	  if (displayMessage) std::cout << "\t\tAPV 0x25 access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tAPV 0x25 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x25)]) {
	  if (displayMessage) std::cout << "\t\tMissing APV 0x25" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing APV 0x25" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x43)]) {
	  if (displayMessage) std::cout << "\t\tAPV MUX 0x43 access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tAPV MUX 0x43 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x43)]) {
	  if (displayMessage) std::cout << "\t\tMissing APV Mux 0x43" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing APV Mux 0x43" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x0)]) {
	  if (displayMessage) std::cout << "\t\tDCU access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tDCU access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x0)]) {
	  if (displayMessage) std::cout << "\t\tMissing DCU" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing DCU" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x60)]) {
	  if (displayMessage) std::cout << "\t\tAOH access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tAOH 0x60 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x60)]) {
	  if (displayMessage) std::cout << "\t\tMissing AOH" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing AOH" << std::endl ;
	}
	if (deviceError[index|setAddressKey(0x44)]) {
	  if (displayMessage) std::cout << "\t\tPLL access problem" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tPLL 0x44 access problem" << std::endl ;
	}
	if (deviceMissing[index|setAddressKey(0x44)]) {
	  if (displayMessage) std::cout << "\t\tMissing PLL" << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing PLL" << std::endl ;
	}
      }
      else {
	// DCU on CCU
	if (address == 0x0) {
	  dcuNumber ++ ;
	  char msg[80] ; decodeKey(msg,index) ;
	  if (displayMessage) std::cout << "\tDCU on " << msg << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\tDCU on " << msg << std::endl ;
	  if (deviceError[index|setAddressKey(0x0)]) {
	    if (displayMessage) std::cout << "\t\tDCU access problem" << std::endl ;
	    if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tDCU access problem" << std::endl ;
	  }
	  if (deviceMissing[index|setAddressKey(0x0)]) {
	    if (displayMessage) std::cout << "\t\tMissing DCU" << std::endl ;
	    if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing DCU" << std::endl ;
	  }
	}
	else if (address == DOHI2CADDRESS) {
	  dohNumber ++ ;
	  char msg[80] ; decodeKey(msg,index) ;
	  if (displayMessage) std::cout << "\tDOH on " << msg << std::endl ;
	  if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\tDOH on " << msg << std::endl ;
	  if (deviceError[index|setAddressKey(DOHI2CADDRESS)]) {
	    if (displayMessage) std::cout << "\t\tDOH access problem" << std::endl ;
	    if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tDOH access problem" << std::endl ;
	  }
	  if (deviceMissing[index|setAddressKey(DOHI2CADDRESS)]) {
	    if (displayMessage) std::cout << "\t\tMissing DOH" << std::endl ;
	    if ((outputStream != NULL) && (outputStream->is_open())) *outputStream << "\t\tMissing DOH" << std::endl ;
	  }
	}
      }
    }

//     if ((outputStream != NULL) && (outputStream->is_open())) { 
//       if (dcuNumber) 
// 	*outputStream << "Found " << std::dec << dcuNumber << " DCUs on CCU" << std::endl ; 
//       if (dohNumber) 
// 	*outputStream << "Found " << std::dec << dohNumber << " DOHs" << std::endl ; 
//       if (module4Number) 
// 	*outputStream << "Found " << std::dec << dohNumber << " Modules with 4 APVs" << std::endl ; 
//       if (module6Number) 
// 	*outputStream << "Found " << std::dec << dohNumber << " Modules with 6 APVs" << std::endl ; 
//     } 
//     if (displayMessage) { 
//       if (dcuNumber) 
// 	std::cout << "Found " << std::dec << dcuNumber << " DCUs on CCU" << std::endl ; 
//       if (dohNumber) 
// 	std::cout << "Found " << std::dec << dohNumber << " DOHs" << std::endl ; 
//       if (module4Number) 
// 	std::cout << "Found " << std::dec << dohNumber << " Modules with 4 APVs" << std::endl ; 
//       if (module6Number) 
// 	std::cout << "Found " << std::dec << dohNumber << " Modules with 6 APVs" << std::endl ; 
//     } 
  }

  /** This method check the rings and return all the instance corresponding to each FEC in the crate. 
   * Mainly the first slot with a FEC will be the instance 0, then the second instance 1, etc.
   * \param fecAccess - access to the FEC
   * \param fecInstance - array from 1 to 21 maximum for the instance of each slot
   * \param fileOutput - output stream (dump the output in the corresponding file if != NULL)
   * \exception in case of problem in getting the list of the FECs or if the list is empty
   * \warning the list is initialise with 0xFFFFFFFF in order to differenciate the instance set
   */
  static void getCrateFecSupervisorInstance ( FecAccess &fecAccess, keyType fecInstance[MAX_NUMBER_OF_SLOTS], 
					      std::string partitionName = "", std::ofstream *instanceStream = NULL )
    throw (FecExceptionHandler) {

    // Initialise the list of instance
    for (int i = 0 ; i < MAX_NUMBER_OF_SLOTS ; i ++) fecInstance[i] = NOFECRING ;

    // Retreive the list of FECs
    std::list<keyType> *fecSlotList = fecAccess.getFecList ( ) ;
      
    if (fecSlotList != NULL) {
      
      unsigned int instance = 0 ;
      Sgi::hash_map<keyType, bool> instanceDone ;
      for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
	keyType index = *p ;
	instanceDone[getFecKey(index)] = false ;
      }
      
      for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
	
	keyType index = *p ;
	
	// Store the instance for the given slot
	if (!instanceDone[getFecKey(index)]) {
	  fecInstance[instance] = index ;
	  instance ++ ;
	  instanceDone[getFecKey(index)] = true ;
	  //std::cout << "Add an instance " << fecSlotInstance[getFecKey(index)] << " for FEC on slot " << getFecKey(index) << std::endl ;
	}
      }
	
      // Delete = already store in the table of FEC Access class
      delete fecSlotList ;

      // Dump in file
      if (instanceStream != NULL) {

	for (int i = 0 ; i < MAX_NUMBER_OF_SLOTS ; i ++) {
	  if (fecInstance[i] != NOFECRING) {

	    std::string fecHardwareId = "0" ;
	    if (fecAccess.getFecBusType() == FECVME) {
	      FecVmeRingDevice *fecVmeRing = (FecVmeRingDevice *)fecAccess.getFecRingDevice (fecInstance[i]) ;
	      fecHardwareId = fecVmeRing->getFecHardwareId() ; // FEC hardware ID on the FEC
	    }
	    else fecHardwareId = toString(getFecKey(fecInstance[i])) ;

	    if ((partitionName != "") && (partitionName != "nil"))
	      *instanceStream << "FEC hardware ID" << "\t" << "FEC slot" << "\t" << "Instance" << "\t" << "Partition Name" << std::endl ;
	    else
	      *instanceStream << "FEC hardware ID" << "\t" << "FEC slot" << "\t" << "Instance" << std::endl ;
	    
	    if ((partitionName != "") && (partitionName != "nil"))
	      *instanceStream << fecHardwareId << "\t" << getFecKey(fecInstance[i]) << "\t" << i << "\t" << partitionName << std::endl ;
	    else
	      *instanceStream << fecHardwareId << "\t" << getFecKey(fecInstance[i]) << "\t" << i << std::endl ;
	  }
	}
      }
    }
    else {
      RAISEFECEXCEPTIONHANDLER ( TSCFEC_INVALIDOPERATION, "Unable to find FECs in the crate", FATALERRORCODE ); 
    }
  }

  /** This method will check all the rings on all the FECs in the crate and return a vector of PIA reset.
   * \param vPiaReset - vector of PIA reset (output)
   * \param listError - error list during the get of the CCUs (output)
   * \param fecAccess - access to the FEC (input)
   * \param piaChannel - PIA channel (0x30 by default)
   * \param delayActiveReset - Delay to maintain the active reset (10 us)
   * \param intervalDealyReset - interval between two resets (10000 us)
   * \param mask - PIA mask to be applied (0xFF)
   * \param fecRingSlotStart - specify a start slot if needed or a specific ring
   * \param fecRingSlotStop - specify a end slot if needed or a specific ring (same as start if you want to check for a ring). If a range is specify then all the rings are detected (from slot state to slot stop and from ring min (0 or 1) and ring max (7 or 8)
   * \warning this method does not download the PIA reset
   */
  static void getCratePiaReset ( piaResetVector &vPiaReset,
				 std::list<std::string> &listError,
				 FecAccess &fecAccess,
				 tscType8 piaChannel = 0x30,
				 unsigned long delayActiveReset = 10,
				 unsigned long intervalDelayReset = 10000,
				 tscType8 mask = 255,
				 keyType fecRingSlotStart = NOFECRING,
				 keyType fecRingSlotStop = NOFECRING,
				 unsigned int crateId = 1 ) 
    throw (FecExceptionHandler) {

    // Get the FEC list or get the index of the corresponding FEC
    std::list<keyType> *fecSlotList = NULL ;
    if (fecRingSlotStart == NOFECRING) fecSlotList = fecAccess.getFecList ( );
    else if (fecRingSlotStart == fecRingSlotStop) {
      // Check if the FEC exists
      fecAccess.getFecRingDevice ( fecRingSlotStart ) ;
      fecSlotList = new std::list<keyType> ;
      fecSlotList->push_back(fecRingSlotStart) ;
      //std::cout << "Look on the FEC " << getFecKey(fecRingSlot) << "." << getRingKey(fecRingSlot) << std::endl ;
    } else {

      // Slot min, slot max
      unsigned int fecSlotStart = getFecKey(fecRingSlotStart) > getFecKey (fecRingSlotStop) ? getFecKey (fecRingSlotStop) : getFecKey(fecRingSlotStart) ;
      unsigned int fecSlotStop  = getFecKey(fecRingSlotStart) > getFecKey (fecRingSlotStop) ? getFecKey (fecRingSlotStart) : getFecKey(fecRingSlotStop) ;

      std::list<keyType> *listoffecring = fecAccess.getFecList() ;
      for (std::list<keyType>::iterator it = listoffecring->begin() ; it != listoffecring->end() ; it ++) {
	if (getFecKey(*it) >= fecSlotStart && getFecKey(*it) <= fecSlotStop) {
	  if (fecSlotList == NULL) fecSlotList = new std::list<keyType> ;
	  fecSlotList->push_back(*it) ;
	}
      }
      delete listoffecring ;
    }

    if (fecSlotList != NULL) {

      for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {

	keyType indexFEC = *p ;

	try {
	  // Get the CCU list
	  std::list<keyType> *ccuList = fecAccess.getCcuList ( indexFEC ) ;

	  if (ccuList != NULL) {

	    for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {

	      keyType index = *p ;

	      keyType piaChannelResetIndex = buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),piaChannel,0) ;	      
	      piaResetDescription *piaResetDes = new piaResetDescription ( piaChannelResetIndex, delayActiveReset, intervalDelayReset, mask) ;
	      piaResetDes->setCrateId(crateId) ;
	      
	      if (piaResetDes != NULL) {
		
		if (fecAccess.getFecBusType() == FECVME) {
		  FecVmeRingDevice *fecVmeRing = (FecVmeRingDevice *)fecAccess.getFecRingDevice (index) ;
		  piaResetDes->setFecHardwareId(fecVmeRing->getFecHardwareId(), crateId) ; // FEC hardware ID on the FEC
		}
		else piaResetDes->setFecHardwareId("0", crateId) ; // No FEC hard id
		vPiaReset.push_back(piaResetDes) ;
	      }
	    }
	    
	    delete ccuList ;
	  }
	  else {
	    std::ostringstream msgError ; msgError << "Unable to get the list of CCUs for FEC " << std::dec << (int)getFecKey(indexFEC) << "." << (int)getRingKey(indexFEC) ;
	    listError.push_back (msgError.str()) ;
	  }
	}
	catch (FecExceptionHandler &e) {

	  std::ostringstream msgError ; msgError << "Unable to get the list of CCUs for FEC " << std::dec << (int)getFecKey(indexFEC) << "." << (int)getRingKey(indexFEC) << ": " << e.what() ;
	  listError.push_back (msgError.str()) ;
	}
      }

      // delete the table
      delete fecSlotList ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER ( TSCFEC_INVALIDOPERATION, "Unable to find FECs in the crate", FATALERRORCODE ) ;
    }    
  }

  /** This method will check all the rings on all the FECs in the crate and return a vector of all devices
   * \param vDevices - device detected (contain all devices even the DOH descriptions)
   * \param listError - list of the errors
   * \param fecAccess - hardware access to the rings
   * \param apvIn - default values
   * \param muxIn - default values
   * \param pllIn - default values
   * \param laserIn - default values
   * \param dohIn - default values
   * \param displayMessage - display message during the detection
   * \param fecRingSlotStart - specify a start slot if needed or a specific ring
   * \param fecRingSlotStop - specify a end slot if needed or a specific ring (same as start if you want to check for a ring). If a range is specify then all the rings are detected (from slot state to slot stop and from ring min (0 or 1) and ring max (7 or 8)
   * \param noBroadcast - CCU broadcast mode not used (true) or used (false, default value): do not change this parameter, it affects in some indetermine way the CCU CRE so the PIA reset scanning before
   */
  static void getCrateFecDevices ( deviceVector &vDevices,
				   std::list<std::string> &listError,
				   FecAccess &fecAccess,
				   apvDescription apvIn,
				   muxDescription muxIn,
				   pllDescription pllIn,
				   laserdriverDescription laserIn,
				   laserdriverDescription dohIn,
				   bool displayMessage = true,
				   keyType fecRingSlotStart = NOFECRING,
				   keyType fecRingSlotStop = NOFECRING,
				   unsigned int crateId = 1,
				   bool noBroadcast = false ) {
    
    // Device to be found
    int sizeValues = 11 ;
    keyType
      deviceValues[11][2] = { 
	{0x20, RALMODE   }, // APV
	{0x21, RALMODE   }, // APV
	{0x22, RALMODE   }, // APV
	{0x23, RALMODE   }, // APV
	{0x24, RALMODE   }, // APV
	{0x25, RALMODE   }, // APV
	{0x43, RALMODE   }, // APVMUX
	{DCUADDRESS, NORMALMODE}, // DCU
	{0x44, NORMALMODE}, // PLL
	{0x60, NORMALMODE}, // Laserdriver
	{DOHI2CADDRESS, NORMALMODE}  // DOH
    } ;

    // Retreive all devices
    std::list<keyType> *deviceList = NULL ;
    if (fecRingSlotStart == NOFECRING) 
      deviceList = fecAccess.scanRingForI2CDevice ( (keyType *)deviceValues, sizeValues, false, false) ; //displayMessage ) ;
    else if (fecRingSlotStart == fecRingSlotStop) {
      deviceList = fecAccess.scanRingForI2CDevice ( fecRingSlotStart, (keyType *)deviceValues, sizeValues, noBroadcast, false) ; //displayMessage ) ;
      //std::cout << "Look on the FEC " << getFecKey(fecRingSlot) << "." << getRingKey(fecRingSlot) << std::endl ;
    }
    else {
      // For each FEC, for each ring
      // Slot min, slot max
      unsigned int fecSlotStart = getFecKey(fecRingSlotStart) > getFecKey (fecRingSlotStop) ? getFecKey (fecRingSlotStop) : getFecKey(fecRingSlotStart) ;
      unsigned int fecSlotStop  = getFecKey(fecRingSlotStart) > getFecKey (fecRingSlotStop) ? getFecKey (fecRingSlotStart) : getFecKey(fecRingSlotStop) ;

      std::list<keyType> *listoffecring = fecAccess.getFecList() ;
      for (std::list<keyType>::iterator it = listoffecring->begin() ; it != listoffecring->end() ; it ++) {
	if (getFecKey(*it) >= fecSlotStart && getFecKey(*it) <= fecSlotStop) {
	  std::list<keyType> *tempList =  fecAccess.scanRingForI2CDevice ( *it, (keyType *)deviceValues, sizeValues, noBroadcast, displayMessage ) ;
	  if (tempList != NULL) {
	    if (deviceList == NULL) deviceList = new std::list<keyType> ;
	    deviceList->merge(*tempList) ;
	    delete tempList ;
	  }
	}
      }
      delete listoffecring ;
    }

    if (deviceList == NULL) {
      std::ostringstream msgError ; msgError << "Unable to detect any devices on all rings" ;
      listError.push_back (msgError.str()) ;
    }
    else {
/*       long sizeBytes = 0 ; */
/*       for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) { */
/* 	keyType index = *p ; */
/* 	switch (getAddressKey(index)) { */
/* 	case 0x20: */
/* 	case 0x21: */
/* 	case 0x22: */
/* 	case 0x23: */
/* 	case 0x24: */
/* 	case 0x25: sizeBytes += sizeof(apvDescription) ; break ; */
/* 	case 0x43: sizeBytes += sizeof(muxDescription) ; break ; */
/* 	//case DCUADDRESS: sizeBytes += sizeof(dcuDescription) ; break ; => should be readout */
/* 	case DOHI2CADDRESS: */
/* 	case 0x60: sizeBytes += sizeof(laserdriverDescription) ; break ; */
/* 	case 0x44: sizeBytes += sizeof(pllDescription) ; break ; */
/* 	} */
/*       } */
/*       std::cout << "Allocate " << std::dec << sizeBytes << " bytes" << std::endl ; */
/*       void *memDevices = malloc (sizeBytes) ; */
/*       if (memDevices == NULL) { */
/* 	std::ostringstream msgError ; msgError << "Unable to allocate the space needed for the devices" ; */
/* 	listError.push_back (msgError.str()) ; */
/* 	return ; */
/*       } */
/*       std::cout << "ptr = " << memDevices << std::endl ; */

      // ????????????????????????????????????????????????????????????????
      std::cout << "-------------------------------------- Creation of the deviceDescription" << std::endl ;
      unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      deviceList->sort() ;
      std::string fecHardwareIdArray[22] ;  // = ""

      for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) {
	
	keyType index = *p ;

#ifdef DEBUGMSGERROR
	if (displayMessage) {
	  char msg[100] ;
	  decodeKey (msg, index) ;
	  std::cout << "Device " << msg << " found" << std::endl ;
	}
#endif
        
	// Description
	apvDescription *apvd = NULL ;
	pllDescription *plld = NULL ;
	muxDescription *muxd = NULL ;
	dcuDescription *dcud = NULL ;
	laserdriverDescription *ld = NULL ;
	laserdriverDescription *dohd = NULL ;

	// FEC hardware id (cache)
	std::string fecHardwareId = "0" ;
	if (fecAccess.getFecBusType() == FECVME) {
	  if (fecHardwareIdArray[getFecKey(index)] == "") {
	    try {
	      FecVmeRingDevice *fecVmeRing = (FecVmeRingDevice *)fecAccess.getFecRingDevice (index) ;
	      fecHardwareIdArray[getFecKey(index)] = fecVmeRing->getFecHardwareId() ; // FEC hardware ID on the FEC
	    }
	    catch (FecExceptionHandler &e) {
	      std::ostringstream msgError ;
	      msgError << "Cannot read the FEC Hardware id from FEC " << std::dec << getFecKey(index) << ": " << e.what() ;
	      listError.push_back(msgError.str()) ;
	    }
	  }

	  fecHardwareId = fecHardwareIdArray[getFecKey(index)] ;
	}

	//enumDeviceType deviceType = deviceAccess::getDeviceType (index) ;
	switch (getAddressKey(index)) {
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24:
	case 0x25:     // ------------- APV
	  //apvd = (apvDescription *)memDevices ; memDevices += sizeof(apvDescription) ; sizeBytes -= sizeof(apvDescription) ;  apvd = apvIn ;
	  apvd = new apvDescription(apvIn) ; // .clone() ;
	  apvd->setAccessKey(index) ;
	  apvd->setFecHardwareId(fecHardwareId,crateId) ;
	  //apvd->setCrateId(crateId) ;
	  // Insert it in the vector vDevice
	  vDevices.push_back ( apvd ) ;
	  break ;
	case 0x43:     // ------------ MUX
	  //muxd = (muxDescription *)memDevices ; memDevices += sizeof(muxDescrition) ; sizeBytes -= sizeof(muxDescription) ; muxd = muxIn ;
	  muxd = new muxDescription (muxIn) ; //.clone() ;
	  muxd->setAccessKey(index) ;
	  muxd->setFecHardwareId(fecHardwareId,crateId) ;
	  //muxd->setCrateId(crateId) ;
	  // Insert it in the vector vDevice
	  vDevices.push_back ( muxd ) ;
	  break ;
	case DCUADDRESS:     // ------------- DCU
	  // Insert it in the vector vDevice
	  try {
	    dcuAccess dcuA(&fecAccess, index) ;
	    dcud = new dcuDescription (index,0,dcuA.getDcuHardId(),0,0,0,0,0,0,0,0,DCUFEH) ;
	    dcud->setFecHardwareId(fecHardwareId,crateId) ;
	    //dcud->setCrateId(crateId) ;
	    dcud->setTimeStamp(0) ;
	    vDevices.push_back ( dcud ) ;
	  }
	  catch (FecExceptionHandler &e) {
	    char msg[80] ; decodeKey(msg,index) ;
	    std::ostringstream msgError ;
	    msgError << "Cannot read the DCU Hardware id on " << msg << ": " << e.what() ;
	    listError.push_back(msgError.str()) ;
	  }
	  break ;
	case DOHI2CADDRESS:     // ------------- DOH
	  //dohd = (laserdriverDescription *)memDevices ; memDevices += sizeof(laserdriverDescrition) ; sizeBytes -= sizeof(laserdriverDescription) ; dohd = dohIn ;
	  dohd = new laserdriverDescription(dohIn) ; //.clone() ;
	  dohd->setAccessKey(index) ;
	  dohd->setFecHardwareId(fecHardwareId,crateId) ;
	  //dohd->setCrateId(crateId) ;
	  // Insert it in the vector vDevice
	  vDevices.push_back ( dohd ) ;
	  break ;
	case 0x60:    // -------------- Laserdriver
	  //ld = (laserdriverDescription *)memDevices ; memDevices += sizeof(laserdriverDescrition) ; sizeBytes -= sizeof(laserdriverDescription) ; ld = laserIn ;
	  ld = new laserdriverDescription (laserIn) ; //.clone() ;
	  ld->setAccessKey(index) ;
	  ld->setFecHardwareId(fecHardwareId,crateId) ;
	  //ld->setCrateId(crateId) ;
	  // Insert it in the vector vDevice
	  vDevices.push_back ( ld ) ;
	  break ;
	case 0x44:    // ------------- PLL
	  //plld = (pllDescription *)memDevices ; memDevices += sizeof(pllDescrition) ; sizeBytes -= sizeof(pllDescription) ; plld = pllIn ;
	  plld = new pllDescription (pllIn) ; //.clone() ;
	  plld->setAccessKey(index) ;
	  plld->setFecHardwareId(fecHardwareId,crateId) ;
	  //plld->setCrateId(crateId) ;
	  // Insert it in the vector vDevice
	  vDevices.push_back ( plld ) ;
	  break ;
	}
      }

      // ????????????????????????????????????????????????????????????????
      unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "-------------------------------------- End of the creation in " << std::dec << (endMillis-startMillis) << " ms" << std::endl ;
      
      // delete the list
      delete deviceList ;
    }
  }

  /** Display the errors coming from FecAccessManager download and upload operation
   * \param message - general message
   * \param errorList - error list
   * \param errorReportLogger - report logger
   * \param deviceError - return a boolean in the list
   * \param partitionName - partition name (can be empty)
   */
  static void displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList, ErrorReportLogger &errorReportLogger, 
				    Sgi::hash_map<keyType, bool> *deviceError, std::string partitionName ) {

    if (errorList.size() == 0) return ;

    Sgi::hash_map<keyType, bool> errorDevice ; // in order to avoid multiple display of errors
    Sgi::hash_map<unsigned int, FecExceptionHandler *> deleteException ; // to delete the exception

    // simple message
    std::ostringstream msgInfo ; msgInfo << message << ": found " << errorList.size() << " errors" << std::endl ;
    errorReportLogger.errorReport (msgInfo.str(), LOGINFO) ;

    for (std::list<FecExceptionHandler *>::iterator p = errorList.begin() ; p != errorList.end() ; p ++) {
      
      FecExceptionHandler *e = *p ;

      unsigned long ptr = (unsigned long)e ;
      if (deleteException.find(ptr) == deleteException.end()) deleteException[ptr] = e ;

      if (e->getPositionGiven()) {
	
	if (!errorDevice[e->getHardPosition()]) {
	  
	  errorDevice[e->getHardPosition()] = true ;
	  
	  // Signal the errors
	  if (partitionName != "")
	    errorReportLogger.errorReport (message, *e, LOGERROR, 0, partitionName) ;
	  else 
	    errorReportLogger.errorReport (message, *e, LOGERROR) ;	

	  // Device in error
	  if (deviceError != NULL) (*deviceError)[e->getHardPosition()] = true ;
	}
      }
      else {

	// Signal the errors
	if (partitionName != "")
	  errorReportLogger.errorReport (message, *e, LOGERROR, 0, partitionName) ;
	else
	  errorReportLogger.errorReport (message, *e, LOGERROR) ;
      }
    }
    
    // Delete the exception (not redundant)
    for (Sgi::hash_map<unsigned int, FecExceptionHandler *>::iterator p = deleteException.begin() ; p != deleteException.end() ; p ++) {
      delete p->second ;
    }

    // No more error
    errorList.clear() ;
  }

  /** So detects the FEC and the devices, then upload the results to files: correct modules, incorrect modules, instance and upload the devices to XML file or database
   * \param fecAccess - hardware access to the rings
   * \param fecAccessManager - device access manager
   * \param errorReportLogger - to report the error
   * \param fecFactory - can be NULL if no upload is requiered
   * \param moduleCorrectFile - file output for the correct modules
   * \param moduleIncorrectFile - file output for the incorrect modules
   * \param instanceFecHardwareIdFecSlotFile - file output for the instance
   * \param templateXMLFecFileName - template file for the XML devices
   * \param outputXMLFecFileName - output file for the XML devices
   * \param partitionName - partition for database upload
   * \param structureName - structure name
   * \param removeBadModules - remove the bad modules (module without PLL or MUX or AOH)
   * \param displayMessage - display the message
   * \param multiFrames - use the multi-frames mode
   * \param doUploadInDatabase - upload in database
   * \param doUploadInFile - upload in file
   * \param createPartition - create a partition in the database
   * \param moduleCorrect - index of the correct modules
   * \param moduleIncorrect - index of the incorrect modules
   * \param moduleCannotBeUsed - index of the modules with fatal errors (module without PLL or MUX or AOH)
   * \param dcuDohOnCcu - DCU and DOH on CCU
   * \param deviceError - index of the devices with error on downlaoding
   * \param deviceMissing - index of the devices that are missing (for example, a module with 3 APVs, etc.)
   * \param listErrors - list of the errors
   * \param subDetector - can be UNKNOWN, TIB, TOB, TEC. On this value the DCU on CCU is checked correctly with the following definition:
   * <lu>
   * <li> TEC, TIB = 0x10 for all DCU on CCU
   * <li> TOB = 0x1E for all DCU on CCU
   * <ul>
   * \param fecRingSlotStart - specify a start slot if needed or a specific ring
   * \param fecRingSlotStop - specify a end slot if needed or a specific ring (same as start if you want to check for a ring). If a range is specify then all the rings are detected (from slot state to slot stop and from ring min (0 or 1) and ring max (7 or 8)
   * \param piaVersMajor - version major if the PIA upload
   * \param piaVersMinor - version minor if the PIA upload
   * \param devVersMajor - version major of the device upload
   * \param devVersMajor - version minor of the device upload
   * \param crateId - crate ID 
   * \param reloadFirmware - reload firmware on the FEC
   * \warning the FecAccess and FecAccessManager must not be NULL or the program will crash
   * \return true if no fatal error
   */
  static bool uploadDetectFECs ( 
				// Parameters for the access hard + DB + error repot
				FecAccess &fecAccess,
				FecAccessManager &fecAccessManager, 
				ErrorReportLogger &errorReportLogger,
				FecFactory *fecFactory,

				// All text output Files
				std::string moduleCorrectFile,
				std::string moduleIncorrectFile,
				std::string instanceFecHardwareIdFecSlotFile,
				
				// All XML output files 
				std::string templateXMLFecFileName,
				std::string outputXMLFecFileName,
				std::string partitionName,
				std::string structureName,
				
				// Configuration
				bool removeBadModules,
				bool displayMessage,
				bool multiFrames,
				bool doUploadInDatabase,
				bool doUploadInFile,
				bool createPartition,
				
				// Output, list of modules, list of device in error, list of errors
				std::list<keyType> &moduleCorrect, 
				std::list<keyType> &moduleIncorrect, 
				std::list<keyType> &moduleCannotBeUsed, 
				std::list<keyType> &dcuDohOnCcu, 
				Sgi::hash_map<keyType, bool> &deviceError,
				Sgi::hash_map<keyType, bool> &deviceMissing,
				std::list<std::string> &listErrors,

				// Sub detector
				std::string subDetector,

				// Should be used to have one configuration file per FEC
				keyType fecRingSlotStart = NOFECRING, 
				keyType fecRingSlotStop  = NOFECRING, 
				unsigned int *devVersMajor = NULL,
				unsigned int *devVersMinor = NULL,

				// Crate slot
				unsigned int crateId = 1,

				// Firmware reload
				bool reloadFirmware = false,

				// Force the redundancy
				bool forceApplyRedundancy = true
			      ) {

    //if (reloadFirmware) std::cout << "A reload of the firmware will be issued" << std::endl ;
    //else std::cout << "No reload of firmware will be done" << std::endl ;

    bool uploadOk = true ;

    // --------------------------------------------------------------------------------
    // Check if a ring has been specified
    std::list<keyType> *fecList = NULL ;
    if (fecRingSlotStart == NOFECRING) fecList = fecAccess.getFecList() ;
    else if (fecRingSlotStart == fecRingSlotStop) {
      fecAccess.getFecRingDevice ( fecRingSlotStart ) ;
      fecList = new std::list<keyType> ;
      fecList->push_back(fecRingSlotStart) ;
      //std::cout << "Look on the FEC " << getFecKey(fecRingSlot) << "." << getRingKey(fecRingSlot) << std::endl ;
    }
    else {

      // Slot min, slot max
      unsigned int fecSlotStart = getFecKey(fecRingSlotStart) > getFecKey (fecRingSlotStop) ? getFecKey (fecRingSlotStop) : getFecKey(fecRingSlotStart) ;
      unsigned int fecSlotStop  = getFecKey(fecRingSlotStart) > getFecKey (fecRingSlotStop) ? getFecKey (fecRingSlotStart) : getFecKey(fecRingSlotStop) ;

      if (displayMessage) 
	std::cout << "Checking FEC between slot " << std::dec << fecSlotStart << " and " << fecSlotStop << std::endl ;

      std::list<keyType> *listoffecring = fecAccess.getFecList() ;
      for (std::list<keyType>::iterator it = listoffecring->begin() ; it != listoffecring->end() ; it ++) {
	if (getFecKey(*it) >= fecSlotStart && getFecKey(*it) <= fecSlotStop) {
	  if (fecList == NULL) fecList = new std::list<keyType> ;
	  fecList->push_back(*it) ;
	}
      }
      delete listoffecring ;
      
      if (displayMessage)
	if (fecList != NULL)
	  std::cout << "Number of FEC,ring " << std::dec << fecList->size() << std::endl; 
	else 
	  std::cout << "Number of FEC,ring " << std::dec << 0 << std::endl; 
    }

    // --------------------------------------------------------------------------------
    // reload the firmware on the FEC
    if (reloadFirmware && (fecList != NULL) && !fecList->empty()) fecAccess.fecHardReset() ;

    // --------------------------------------------------------------------------------
    // Reset all rings
    if ((fecList != NULL) && !fecList->empty()) {
      fecAccessManager.removeDevices() ;
      for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
	try {
	  fecAccess.fecRingResetFSM((*it)) ;
	  fecAccess.fecRingReset((*it)) ;
	  fecAccess.fecRingResetB((*it)) ;
	  std::ostringstream msgInfo ; msgInfo << "Reset the FEC on ring " << std::dec << getFecKey((*it)) << "." << getRingKey((*it)) ;
	  errorReportLogger.errorReport (msgInfo.str(), LOGDEBUG) ;
	}
	catch (FecExceptionHandler &e) {
	  std::ostringstream msgError ; msgError << "FEC: Unable to reset the FEC on ring " << std::dec << getFecKey((*it)) << "." << getRingKey((*it)) ;
	  errorReportLogger.errorReport (msgError.str(), e, LOGFATAL, 1, partitionName) ;
	  listErrors.push_back("FATAL Error: " + msgError.str()) ;
	}
      }
      //delete fecList ;
    }
    else {
      errorReportLogger.errorReport ("FATAL Error: FEC: No FEC has been detected",LOGFATAL) ;
      listErrors.push_back("FATAL Error: No FEC has been detected") ;
      if (fecList != NULL) delete fecList ;
      return false ;
    }

    // --------------------------------------------------------------
    // Redo the redundancy for the rings which needs it
    // the upload of the rings will be done only at the end since the CCU is not created
    tkringVector ringsToBeUploaded ;
    if ((fecFactory != NULL) && (fecFactory->getDbUsed())) {

      for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
	// try to find the redundancy configuration for the rings
	try {
	  if (forceApplyRedundancy || !isFecSR0Correct(fecAccess.getFecRingSR0(*it))) {
	    TkRingDescription* myRing = fecFactory->getDbRingDescription(fecAccess.getFecHardwareId(*it),getRingKey(*it));
	    setRedundancy ( fecAccess, *it, *myRing, forceApplyRedundancy, errorReportLogger ) ;
	    ringsToBeUploaded.push_back(myRing->clone()) ;
	  }
	}
	catch (FecExceptionHandler &e) {
	 
	  errorReportLogger.errorReport ("Error during the reconfiguration",e,LOGERROR) ;
	}
	catch (oracle::occi::SQLException &e) {

	  errorReportLogger.errorReport (((FecDeviceFactory *)fecFactory)->what("Error during the reconfiguration (OCCI exception) for partition", e), LOGFATAL) ;
	}
      }
    }
    else { 
      errorReportLogger.errorReport ("No reconfiguration possible with no access to database", LOGWARNING) ;
    }

    // --------------------------------------------------------------
    // Clean the previous detection
    deviceError.clear() ;
    deviceMissing.clear() ;
    moduleCorrect.clear() ;
    moduleIncorrect.clear() ;
    moduleCannotBeUsed.clear() ;
    dcuDohOnCcu.clear() ;

    // --------------------------------------------------------------
    // Create the FEC instance for the FECs
    // Only if it is the crate controller
    if ( (fecRingSlotStart == NOFECRING) || (fecRingSlotStart != fecRingSlotStop) ) {
      keyType fecInstance[21] ;
      std::ofstream *temp = NULL ;
      std::ofstream instanceStream(instanceFecHardwareIdFecSlotFile.c_str()) ;
      if (!instanceStream.is_open()) {
	errorReportLogger.errorReport ("Cannot create status file for instance, FEC ID and FEC slot", LOGWARNING) ;
	listErrors.push_back("Warning: Cannot create status file for instance, FEC ID and FEC slot") ;
      }
      else temp = &instanceStream ;
      
      try {
	// Retreive the information and dump in a file if requested
	FecDetectionUpload::getCrateFecSupervisorInstance (fecAccess, fecInstance, partitionName, temp) ;
      }
      catch (FecExceptionHandler &e) {
	// Should not happends
	errorReportLogger.errorReport ("FATAL Error: FEC: No FEC has been detected",LOGFATAL) ;
	listErrors.push_back("FATAL Error: No FEC has been detected") ;
      }
    }

    // -------------------------------------------------------------------
    // Create the PIA reset for all the CCUs on all rings
    piaResetVector vPiaReset ;

    // If an input file as been specified then retreive the PIA reset to
    // get the values for the channels, delay of reset, interval delay
    tscType8 channel[4] = {0} ; // 0x30 -> 0x33
    unsigned long delayActiveReset[4] = {0} ; // one delay per channel
    unsigned long intervalDelayReset[4] = {0} ; // one delay per channel
    tscType8 mask[4] = {0} ; // one mask per channel ;

    // retreive the vector of descriptions
    FecFactory fecFactoryTemp ;
    fecFactoryTemp.setUsingFile() ;

    // Parse a file for devices
    bool inputGiven = false ;
    if (templateXMLFecFileName != "") {
      errorReportLogger.errorReport (std::string("Parse the file ") + templateXMLFecFileName + std::string(" for i2c devices settings"), LOGDEBUG) ;
      std::ifstream inputFile(templateXMLFecFileName.c_str()) ;
      if (inputFile.is_open()) {
	inputGiven = true ;
	inputFile.close() ;
      }
    }

    // try to find the input file
    if (inputGiven) {
      fecFactoryTemp.addFileName ( templateXMLFecFileName ) ;

      // Retreive the devices
      piaResetVector inputPiaResets ;
      fecFactoryTemp.getPiaResetDescriptions ( inputPiaResets ) ;

      if (inputPiaResets.size() > 0) {

	// For each input
	for (piaResetVector::iterator device = inputPiaResets .begin() ; device != inputPiaResets.end() ; device ++) {
	  piaResetDescription *deviced = *device ;
	  tscType8 channelPia = getChannelKey(deviced->getKey()) ;
	  switch (channelPia) {
	  case 0x30:
	  case 0x31:
	  case 0x32:
	  case 0x33:
	    channel[channelPia-0x30] = channelPia ;
	    delayActiveReset[channelPia-0x30] = deviced->getDelayActiveReset() ;
	    intervalDelayReset[channelPia-0x30] = deviced->getIntervalDelayReset() ;
	    mask[channelPia-0x30] = deviced->getMask() ;
	    break ;
	  default: {
	    std::ostringstream msgWarn ; msgWarn << "Invalid PIA channel in the input file " << templateXMLFecFileName << " (PIA channel = 0x" << std::hex << (int)channelPia << ")" ;
	    errorReportLogger.errorReport (msgWarn.str(), LOGWARNING) ;
	    listErrors.push_back("Warning: " + msgWarn.str()) ;
	    break ;
	  }
	  }
	}
      }
    }
    else { // No input file
      channel[0] = 0x30 ;
      delayActiveReset[0] = 10 ;
      intervalDelayReset[0] = 10000 ;
      mask[0] = 255 ;
    }

    // For each channel create all the PIA access
    std::list<std::string> listPiaError ;
    for (int i = 0 ; i < 4 ; i ++) {
      if (channel[i] != 0) {
	FecDetectionUpload::getCratePiaReset (vPiaReset, listPiaError, fecAccess, channel[i], delayActiveReset[i], intervalDelayReset[i], mask[i], fecRingSlotStart, fecRingSlotStop,crateId) ;
      }
    }

    // Display the errors
    if (listPiaError.size() > 0) {
      std::ostringstream msgError ;
      msgError << std::dec << listPiaError.size() << " errors on creating the PIA reset: " ;
      errorReportLogger.errorReport (msgError.str(), LOGERROR) ;
      listErrors.push_back(msgError.str()) ;

      for ( std::list<std::string>::iterator it = listPiaError.begin() ; it != listPiaError.end() ; it ++) {
	std::ostringstream msgError1 ;
	msgError1 << "PIA reset error: " << (*it) ;
	errorReportLogger.errorReport (msgError1.str(), LOGERROR) ;
	listErrors.push_back("Error: " + msgError1.str()) ;
      }
    }

    // --------------------------------------------------------
    // Download the PIA reset
    if (vPiaReset.size() > 0) {

      std::string message ;
      std::list<FecExceptionHandler *> errorListPia ;
      try {
	if (!multiFrames) {
	  errorReportLogger.errorReport (std::string("Reset modules through PIA resets (" + toString(vPiaReset.size()) + ")"), LOGDEBUG) ;
	  message = "FEC: Error during Reset the modules througth the PIA channels" ;
	  fecAccessManager.resetPiaModules(&vPiaReset, errorListPia) ;
	}
	else {
	  errorReportLogger.errorReport (std::string("Reset modules through PIA resets in multiple frames (" + toString(vPiaReset.size()) + ")"), LOGDEBUG) ;
	  message = "FEC: Error during Reset the modules througth the PIA channels (multiple frames)" ;
	  fecAccessManager.resetPiaModulesMultipleFrames(&vPiaReset, errorListPia) ;
	}
	Sgi::hash_map<keyType, bool> piaError ;
	displayErrorMessage (message, errorListPia, errorReportLogger, &piaError, partitionName) ;
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger.errorReport (message, e, LOGFATAL,1,partitionName) ;
	listErrors.push_back("FATAL Error: " + message) ;
      }
    }
    else {
      errorReportLogger.errorReport ("No PIA reset to be downloaded", LOGERROR) ;
      listErrors.push_back ("Error: No PIA reset to be downloaded") ;
    }

    // ---------------------------------------------------------
    // Retreive the input files for the devices
    apvDescription   *apvIn   = NULL ;
    muxDescription   *muxIn   = NULL ;
    pllDescription   *pllIn   = NULL ;
    laserdriverDescription *laserIn = NULL ;
    laserdriverDescription *dohIn   = NULL ;

    // try to find the input file
    if (inputGiven) {

      // Vector of descriptions
      deviceVector deviceInput ;

      // retreive the devices
      fecFactoryTemp.getFecDeviceDescriptions ( deviceInput ) ;

      if (!deviceInput.empty()) {
      
	// For each device => access it
	for (deviceVector::iterator device = deviceInput.begin() ; device != deviceInput.end() ; device ++) {
	
	  deviceDescription *deviced = *device ;
	  switch (deviced->getDeviceType()) {
	  case APV25:
	    apvIn = (apvDescription *)deviced->clone() ;
	    apvIn->setCrateId ( crateId ) ;
	    break ;
	  case APVMUX:
	    muxIn = (muxDescription *)deviced->clone() ;
	    muxIn->setCrateId ( crateId ) ;
	    break ;
	  case PLL:
	    pllIn = (pllDescription *)deviced->clone() ;
	    pllIn->setCrateId ( crateId ); 
	    break ;
	  case LASERDRIVER:
	    if (getAddressKey(deviced->getKey()) == 0x60) {
	      // laserdriver
	      laserIn = (laserdriverDescription *)deviced->clone() ;
	      laserIn->setCrateId ( crateId ); 
	    }
	    else if (getAddressKey(deviced->getKey()) == DOHI2CADDRESS) {
	      // DOH
	      dohIn = (laserdriverDescription *)deviced->clone() ;
	      dohIn->setCrateId ( crateId ); 
	    }
	    break ;
	  case DCU:
	    // ADD CrateId here
	    break ;
	  default:
	    break ;
	  }

	  //deviced->display ( ) ;
	}
      }
    }

    if (apvIn == NULL) {
      apvIn = new apvDescription((tscType8)0x2b,
				 (tscType8)0x64,
				 (tscType8)0x4,
				 (tscType8)0x73,
				 (tscType8)0x3c,
				 (tscType8)0x32,
				 (tscType8)0x32,
				 (tscType8)0x32,
				 (tscType8)0x50,
				 (tscType8)0x32,
				 (tscType8)0x50,
				 (tscType8)0,    // Ispare
				 (tscType8)0x43,
				 (tscType8)0x43,
				 (tscType8)0x14,
				 (tscType8)0xFB,
				 (tscType8)0xFE,
				 (tscType8)0) ;
      apvIn->setCrateId ( crateId ); 
    }
    if (muxIn == NULL) {
      muxIn = new muxDescription((tscType16)0xFF) ;
      muxIn->setCrateId ( crateId ); 
    }
    if (pllIn == NULL) {
      pllIn = new pllDescription((tscType8)6,(tscType8)1) ;
      pllIn->setCrateId ( crateId ); 
    }
    if (laserIn == NULL) {
      tscType8 gainAOH = 2 ; 
      tscType8 biasAOH[3] = {23, 23, 23} ;
      laserIn = new laserdriverDescription(gainAOH, biasAOH) ;
      laserIn->setCrateId ( crateId ); 
    }
    if (dohIn == NULL) {
      tscType8 gainDOH = 2 ;
      tscType8 biasDOH[3] = {24, 24, 24} ;
      dohIn = new laserdriverDescription(gainDOH, biasDOH) ;
      dohIn->setCrateId ( crateId ); 
    }

    // ---------------------------------------------------------------------------
    // Create all the devices for the Tracker
    errorReportLogger.errorReport ("Scanning rings for devices", LOGDEBUG) ;
    deviceVector vDevices ;
    std::list<std::string> listDeviceError ;
    FecDetectionUpload::getCrateFecDevices (vDevices, listDeviceError, fecAccess, *apvIn, *muxIn, *pllIn, *laserIn, *dohIn, displayMessage, fecRingSlotStart, fecRingSlotStop, crateId) ;

    // ---------------------------------------------------------------------------
    // Download and upload the devices
    if (vDevices.size() == 0) {
      errorReportLogger.errorReport ("No devices detected, end of the detection", LOGFATAL) ;
      FecFactory::deleteVectorI (vPiaReset) ;
      delete apvIn ; delete muxIn ; delete pllIn ; delete laserIn ; delete dohIn ;  
      listErrors.push_back("FATAL Error: No devices detected, end of the detection") ;
      if (fecList != NULL) delete fecList ;
      return false ;
    }

    // Then display the list of the errors
    if (listDeviceError.size() > 0) {
      std::ostringstream msgError ;
      msgError << std::dec << listDeviceError.size() << " errors on detecting devices: " ;
      errorReportLogger.errorReport (msgError.str(), LOGERROR) ;
      listErrors.push_back("Error: " + msgError.str()) ;

      for ( std::list<std::string>::iterator it = listPiaError.begin() ; it != listPiaError.end() ; it ++) {
	std::ostringstream msgError1 ;
	msgError1 << "Device error: " << (*it) ;
	errorReportLogger.errorReport (msgError1.str(), LOGERROR) ;
	listErrors.push_back("Error: " + msgError1.str()) ;
      }
    }

    // List of error for the next operations
    std::list<FecExceptionHandler *> errorListDeviceUpload ;
    std::list<FecExceptionHandler *> errorListDevice ;

    // ------------------------------------ Download devices
    errorReportLogger.errorReport ("Download the devices detected (" + toString(vDevices.size()) + ")", LOGDEBUG) ;
    if (!multiFrames)                                              // PLL RESET and DOH set
      fecAccessManager.downloadValues (&vDevices, errorListDevice, false, false) ;
    else                                                                       // PLL RESET and DOH set
      fecAccessManager.downloadValuesMultipleFrames (&vDevices, errorListDevice, false, false) ;
    // Display the errors
    displayErrorMessage ("Error during the Download of devices", errorListDevice, errorReportLogger, &deviceError, partitionName) ;

    // ------------------------------------ Upload devices
    deviceVector *deviceUploaded = NULL ;
    if (!multiFrames) {
      errorReportLogger.errorReport ("Upload the devices connected", LOGDEBUG) ;
      deviceUploaded = fecAccessManager.uploadValues ( errorListDeviceUpload ) ;
    }
    else {
      errorReportLogger.errorReport ("Upload the devices connected through multiple frames", LOGDEBUG) ;
      deviceUploaded = fecAccessManager.uploadValuesMultipleFrames ( errorListDeviceUpload ) ;
    }

    if (errorListDeviceUpload.size()) {
      for (std::list<FecExceptionHandler *>::iterator it = errorListDeviceUpload.begin() ; it != errorListDeviceUpload.end() ; it ++) {
	std::ostringstream uploadError ; uploadError << (*it)->what() << " for the partition " << partitionName << std::endl ;
	listErrors.push_back(uploadError.str()) ;
      }
      //displayErrorMessage ("Error during the upload of the devices", errorListDeviceUpload, errorReportLogger, &deviceError, partitionName);
      for (std::list<FecExceptionHandler *>::iterator it = errorListDeviceUpload.begin() ; it != errorListDeviceUpload.end() ; it ++) {
	//std::cout << (*it)->what() << std::endl ;
	delete *it ;
      }
    }

    //displayErrorMessage ("Error during the upload of the devices", errorListDeviceUpload, errorReportLogger, &deviceError, partitionName);

    std::stringstream strE ; strE <<  vDevices.size() << " devices were downloaded and " << deviceUploaded->size() << " were uploaded" ;
    errorReportLogger.errorReport (strE.str(), LOGDEBUG) ;

    // Compare both vector (upload and download) and the devices to the new vector which are faulty
    if (deviceUploaded->size() != vDevices.size()) {

      for (deviceVector::iterator device = vDevices.begin() ; device != vDevices.end() ; device ++) {
      
	bool notPresent = true ;
	for (deviceVector::iterator deviceU = deviceUploaded->begin() ; deviceU != deviceUploaded->end() ; deviceU ++) {
	  if ((*device)->getKey() == (*deviceU)->getKey()) notPresent = false ;
	}
	
	if (notPresent) {
	  char msg[80] ; decodeKey(msg,(*device)->getKey()) ;
	  errorReportLogger.errorReport ("Error during the upload on device " + std::string(msg), LOGERROR) ;
	  deviceError[(*device)->getKey()] = true ;
	  listErrors.push_back("Error during the upload on device " + std::string(msg)) ;
	}
      }
    }
    FecFactory::deleteVectorI (*deviceUploaded) ;

    // --------------------------------------------------------------------------
    // Output file
    std::ofstream correctStream(moduleCorrectFile.c_str());
    std::ofstream incorrectStream(moduleIncorrectFile.c_str());

    if ((moduleCorrectFile == "") || (moduleIncorrectFile == "")) {
      if (correctStream.is_open()) correctStream.close() ;
      if (incorrectStream.is_open()) incorrectStream.close() ;
    }

    if (!correctStream.is_open() || !incorrectStream.is_open()) {
      errorReportLogger.errorReport ("Cannot create status file for correct and incorrect modules", LOGERROR) ;
      
      if (correctStream.is_open()) correctStream.close() ;
      if (incorrectStream.is_open()) incorrectStream.close() ;
    }
    
    // ---------------------------------------------------------------------------
    // Check for problem in the devices
    FecDetectionUpload::checkDevices(vDevices, moduleCorrect, moduleIncorrect, moduleCannotBeUsed, dcuDohOnCcu, deviceMissing, subDetector, displayMessage) ;
    if (correctStream.is_open()) {
      if (displayMessage) std::cout << "Found " << std::dec << moduleCorrect.size() << " module(s) without missing devices" << std::endl ;
      correctStream << "Found " << std::dec << moduleCorrect.size() << " module(s) without errors " << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (moduleCorrect, deviceError, deviceMissing, displayMessage, &correctStream) ;
      if (displayMessage) std::cout << "Found " << std::dec << moduleIncorrect.size() << " module(s) with errors on APV or DCU missing" << std::endl ;
      correctStream << "Found " << std::dec << moduleIncorrect.size() << " module(s) with errors on APV or DCU missing" << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (moduleIncorrect, deviceError, deviceMissing, displayMessage, &correctStream) ;
      // DCU CCU and DOH

      std::list<keyType> dcuCcuList, dohList ;
      for (std::list<keyType> ::iterator it = dcuDohOnCcu.begin() ; it != dcuDohOnCcu.end() ; it ++) {
	
	if (getAddressKey((*it)) == DCUADDRESS) dcuCcuList.push_back((*it)) ;
	else //if (getAddressKey((*it)) == DOHI2CADDRESS) 
	  dohList.push_back((*it)) ;
      }

      if (displayMessage) std::cout << "Found " << std::dec << dcuCcuList.size() << " DCU(s) on CCU" << std::endl ;
      correctStream << "Found " << std::dec << dcuCcuList.size() << " DCU(s) on CCU" << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (dcuCcuList, deviceError, deviceMissing, displayMessage, &correctStream) ;
      if (displayMessage) std::cout << "Found " << std::dec << dohList.size() << " DOH(s)" << std::endl ;
      correctStream << "Found " << std::dec << dohList.size() << " DOH(s)" << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (dohList, deviceError, deviceMissing, displayMessage, &correctStream) ;
      correctStream.close() ;
    }
    else if (displayMessage) {
      std::cout << "Found " << std::dec << moduleCorrect.size() << " module(s) without missing devices " << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (moduleCorrect, deviceError, deviceMissing, displayMessage) ;
      std::cout << "Found " << std::dec << moduleIncorrect.size() << " module(s) with errors on APV or DCU missing" << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (moduleIncorrect, deviceError, deviceMissing, displayMessage) ;
      // DCU CCU and DOH
      std::list<keyType> dcuCcuList, dohList ;
      for (std::list<keyType> ::iterator it = dcuDohOnCcu.begin() ; it != dcuDohOnCcu.end() ; it ++) {
	if (getAddressKey((*it)) == DCUADDRESS) dcuCcuList.push_back((*it)) ;
	else //if (getAddressKey((*it)) == DOHI2CADDRESS) 
	  dohList.push_back((*it)) ;
      }
      std::cout << "Found " << std::dec << dcuCcuList.size() << " DCU(s) on CCU" << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (dcuCcuList, deviceError, deviceMissing, displayMessage) ;
      std::cout << "Found " << std::dec << dohList.size() << " DOH(s)" << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (dohList, deviceError, deviceMissing, displayMessage) ;
    }
    // Fatal problem for the modules
    if (incorrectStream.is_open()) {
      incorrectStream << "Found " << std::dec << moduleCannotBeUsed.size() << " module(s) with fatal error: PLL, MUX or AOH missing " << std::endl ;
      if (displayMessage) std::cout << "Found " << std::dec << moduleCannotBeUsed.size() << " module with fatal error: PLL, MUX or AOH missing " << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (moduleCannotBeUsed, deviceError, deviceMissing, displayMessage, &incorrectStream) ;
      incorrectStream.close() ;
    }
    else if (displayMessage) {
      std::cout << "Found " << std::dec << moduleCannotBeUsed.size() << " module with fatal error: PLL, MUX or AOH missing " << std::endl ;
      FecDetectionUpload::dumpModuleScreenFile (moduleCannotBeUsed, deviceError, deviceMissing, displayMessage) ;
    }

    // Remove the bad devices if needed
    deviceVector *deviceCorrect = &vDevices ;

    // Remove the bad modules (if needed)
    if (moduleCannotBeUsed.size() > 0) {

      if (removeBadModules) {

	deviceCorrect = new deviceVector() ;
	
	// Remove the corresponding device
	for (deviceVector::iterator device = vDevices.begin() ; device != vDevices.end() ; device ++ ) {
	
	  bool toBeRemoved = false ;
	  for (std::list<keyType>::iterator it = moduleCannotBeUsed.begin() ; it != moduleCannotBeUsed.end() ; it ++) {
	  
	    if (getFecRingCcuChannelKey(*it) == getFecRingCcuChannelKey((*device)->getKey())) {
	      //char msg[80] ;
	      //decodeKey(msg,(*device)->getKey()) ;
	      //std::cout << "Remove the device on " << msg << std::endl ;
	      toBeRemoved = true ;
	    }
	  }
	  // If the module has an aoh => add it
	  if (!toBeRemoved) deviceCorrect->push_back (*device) ;
	}
      }
    }

    // Add the missing devices
    if (!removeBadModules && deviceMissing.size()) {

      for (Sgi::hash_map<keyType, bool>::iterator it = deviceMissing.begin() ; it != deviceMissing.end() ; it ++) {
	
	deviceDescription *device = NULL ;
	std::string fecHardwareId = "0" ;
	if (fecAccess.getFecBusType() == FECVME) {
	  try {
	    FecVmeRingDevice *fecVmeRing = (FecVmeRingDevice *)fecAccess.getFecRingDevice (it->first) ;
	    fecHardwareId = fecVmeRing->getFecHardwareId() ; // FEC hardware ID on the FEC
	  }
	  catch (FecExceptionHandler &e) {
	    std::ostringstream msgError ;
	    msgError << "Cannot read the FEC Hardware id from FEC " << std::dec << getFecKey(it->first) << ": " << e.what() ;
	    listErrors.push_back(msgError.str()) ;
	  }
	}
	switch (getAddressKey(it->first)) {
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24:
	case 0x25: {
	  apvDescription *apv = new apvDescription(*apvIn) ; // .clone() ;
	  apv->setAccessKey(it->first) ;
	  apv->setFecHardwareId(fecHardwareId,crateId) ;
	  device = (deviceDescription *) apv ;

	  // ADD CrateId here
	  break ;
	}
	case 0x43: {
	  muxDescription *mux = new muxDescription (*muxIn) ; //.clone() ;
	  mux->setAccessKey(it->first) ;
	  mux->setFecHardwareId(fecHardwareId,crateId) ;
	  device = (deviceDescription *) mux ;

	  // ADD CrateId here
	  break ;
	} 
	case 0x44: {
	  pllDescription *pll = new pllDescription (*pllIn) ; //.clone() ;
	  pll->setAccessKey(it->first) ;
	  pll->setFecHardwareId(fecHardwareId,crateId) ;
	  device = (deviceDescription *) pll ;

	  // ADD CrateId here
	  break ;
	}
	case 0x60: {
	  laserdriverDescription *aoh = new laserdriverDescription (*laserIn) ; //.clone() ;
	  aoh->setAccessKey(it->first) ;
	  aoh->setFecHardwareId(fecHardwareId,crateId) ;
	  device = (deviceDescription *) aoh ;

	  // ADD CrateId here
	  break ;
	}
	case DOHI2CADDRESS: {
	  laserdriverDescription *doh = new laserdriverDescription(*dohIn) ; //.clone() ;
	  doh->setAccessKey(it->first) ;
	  doh->setFecHardwareId(fecHardwareId,crateId) ;
	  device = (deviceDescription *) doh ;

	  // ADD CrateId here
	  break ;
	}
	case DCUADDRESS: {
	  dcuDescription *dcu = new dcuDescription (it->first) ;
	  dcu->setAccessKey(it->first) ;
	  dcu->setFecHardwareId(fecHardwareId,crateId) ;
	  device = (deviceDescription *) dcu ;

	  // ADD CrateId here
	  break ;
	}
	default:
	  char msg[80] ;
	  decodeKey(msg,it->first) ;
	  std::cout << msg << std::endl ;
	}
	
	if (device != NULL) {
	  deviceCorrect->push_back (device) ;
	  if (displayMessage) {
	    char msg[80] ;
	    decodeKey(msg,it->first) ;
	    std::cout << "Adding the missing device " << msg << " in the list of devices to be uploaded" << std::endl ;
	  }
	}
      }
    }

    // --------------------------------------------------------------
    // Upload in file or database
    if (fecFactory != NULL) {

      // --------------------------------------------------------------
      // Apply correction on FEC devices
      // for the time being make correction on dcu hard id for TOB/TEC- DCU hard id = 16777215
      std::string changeIdMsg = fecFactory->setDeviceCorrection (*deviceCorrect, subDetector) ;
      if ( (changeIdMsg.length() > 0) && (changeIdMsg != "NA") ) {
	errorReportLogger.errorReport (changeIdMsg, LOGUSERINFO) ;
      }

      // ---------------------------------------------------------------
      // Real upload
#ifdef DATABASE
      if (doUploadInDatabase && fecFactory->getDbConnected()) {

	fecFactory->setUsingDb() ;

	// Database connection
	try {
	  unsigned int major, minor ;
	  
	  // Example to create new partition 
	  if (! vPiaReset.empty()) {

	    //FecFactory::display(vPiaReset) ;
	    //FecFactory::display(deviceUplaoded) ;
	    fecFactory->createPartition (*deviceCorrect, vPiaReset, &major, &minor, partitionName, createPartition) ;
	  }
	  else {
	    fecFactory->createPartition (*deviceCorrect, &major, &minor, partitionName, createPartition) ;
	  }

	  // Set the database as the version to be downloaded (current state)
	  fecFactory->setFecDevicePartitionVersion ( partitionName, major, minor ) ;

	  if (devVersMajor != NULL && devVersMinor != NULL) {
	    *devVersMajor = major ;
	    *devVersMinor = minor ;
	  }

	  //if (displayMessage) {
	  std::cout << "-------------------------- Version ---------------------" << std::endl ;
	  std::cout << "Upload the device version " << std::dec << major << "." << minor << " in the database" << std::endl ;
	  std::cout << "--------------------------------------------------------" << std::endl ;
	  //}

	  uploadOk = true ;
	}
	catch (FecExceptionHandler &e) {
	  if (displayMessage) 
	    std::cerr << "Error during the creation of the partition in the database (FecExceptionHandler exception):" <<  e.what() << std::endl ;
	  
	  errorReportLogger.errorReport ("Error during the creation of the partition in the database (FecExceptionHandler exception)", e, LOGFATAL,1,partitionName) ;
	  uploadOk = false ;
	}
	catch (oracle::occi::SQLException &e) {
	  if (displayMessage) 
	    std::cerr << ((FecDeviceFactory *)fecFactory)->what("Error during the upload in the database (OCCI exception) for partition" + partitionName, e) ;
	  
	  errorReportLogger.errorReport (((FecDeviceFactory *)fecFactory)->what("Error during the upload in the database (OCCI exception) for partition" + partitionName, e), LOGFATAL) ;
	  uploadOk = false ;
	}


	// ---------------------------------------------------------------
	// Upload the ring reconfiguration
	if (ringsToBeUploaded.size()) {
	  if (displayMessage) std::cout << "--------------------------- Ring reconfiguration upload ---------------" << std::endl ;
	  for (tkringVector::iterator it = ringsToBeUploaded.begin() ; it != ringsToBeUploaded.end() ; it ++) {
	    try {
	      fecFactory->setDbRingDescription ( partitionName, *(*it) ) ;
	      if (displayMessage) std::cout << "Reconfiguration on FEC " << (*it)->getFecSlot() << "." << (*it)->getRingSlot() << " uploaded to database for partition " << partitionName << std::endl ;
	    }
	    catch (FecExceptionHandler &e) {
	      if (displayMessage) std::cerr << "Error during the reconfiguration on FEC " << (*it)->getFecSlot() << "." << (*it)->getRingSlot() << " to database for partition " << partitionName << ": " << e.what() << std::endl ;
	      errorReportLogger.errorReport ("Error during the upload of CCU configuration in the database for partition" + partitionName, e, LOGERROR) ;
	    }
	    catch (oracle::occi::SQLException &e) {
	      std::stringstream msgError ;
	      msgError << "Error during the reconfiguration on FEC " << (*it)->getFecSlot() << "." << (*it)->getRingSlot() << " to database for partition " << partitionName ;
	      if (displayMessage) ((FecDeviceFactory *)fecFactory)->what(msgError.str(),e) ;

	      errorReportLogger.errorReport (((FecDeviceFactory *)fecFactory)->what(msgError.str(), e), LOGFATAL) ;
	    }
	  }
	  if (displayMessage) std::cout << "---------------------- End of Ring reconfiguration upload --------------" << std::endl ;
	}

      }
      else 
#endif
	{
	  if (doUploadInFile) {
	    try {
	      // Example to create a new major version with the device uploaded 
	      // and input file it is set
	      fecFactory->setUsingFile(true) ;
	      fecFactory->setOutputFileName(outputXMLFecFileName) ;
	      if (! vPiaReset.empty())
		fecFactory->setFecDevicePiaDescriptions ( *deviceCorrect, vPiaReset ) ;
	      else
		fecFactory->setFecDeviceDescriptions (*deviceCorrect) ;
	      
	      //if (displayMessage) 
	      std::cout << "-------------- Upload it in file " << outputXMLFecFileName << " done" << std::endl ;

	      uploadOk = true ;
	    }
	    catch (FecExceptionHandler &e) {
	      
	      errorReportLogger.errorReport ("FEC: XML upload in file error", e, LOGFATAL,1,partitionName) ;
	      listErrors.push_back("FEC: XML upload in file error") ;
	      uploadOk = false ;
	    }
	  }
	}
    }

    // --------------------------------------------------------
    // Download the PIA reset to put the devices to 0 (reset)
    if (vPiaReset.size() > 0) {

      std::list<FecExceptionHandler *> errorListPia ;
      std::string message ;
      try {
	if (!multiFrames) {
	  errorReportLogger.errorReport (std::string("Reset modules through PIA resets (" + toString(vPiaReset.size()) + ")"), LOGDEBUG) ;
	  message = "FEC: Error during Reset the modules througth the PIA channels" ;
	  fecAccessManager.resetPiaModules(&vPiaReset, errorListPia) ;
	}
	else {
	  errorReportLogger.errorReport (std::string("Reset modules through PIA resets in multiple frames (" + toString(vPiaReset.size()) + ")"), LOGDEBUG) ;
	  message = "FEC: Error during Reset the modules througth the PIA channels (multiple frames)" ;
	  fecAccessManager.resetPiaModulesMultipleFrames(&vPiaReset, errorListPia) ;
	}
	Sgi::hash_map<keyType, bool> piaError ;
	displayErrorMessage (message, errorListPia, errorReportLogger, &piaError, partitionName) ;
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger.errorReport (message, e, LOGERROR) ;
	listErrors.push_back(message) ;
      }

      bool error = false ;
      for (piaResetVector::iterator it = vPiaReset.begin() ; it != vPiaReset.end() ; it ++) {
	piaResetDescription *pia = (piaResetDescription *)(*it) ;
	try {
	  if (!fecAccess.isChannelEnabled(pia->getKey())) {
	    char msg[80] ; decodeKey(msg,pia->getKey()) ;
	    errorReportLogger.errorReport ("Channel on " + std::string(msg) + " is not enabled, corresponding PIA reset will not be issued: CCU CRE = 0x" + toHexString(fecAccess.getCcuCRE(pia->getKey())), LOGERROR) ;
	    if (displayMessage) 
	      std::cerr << "Channel on " << msg << " is not enabled, corresponding PIA reset will not be issued CCU CRE = 0x" << std::hex << fecAccess.getCcuCRE(pia->getKey()) << std::endl ;
	  }
//	  else {
//	    char msg[80] ; decodeKey(msg,pia->getKey()) ;
//	    std::cout << "Channel ok on " << msg << ": CRE = 0x" << std::hex << fecAccess.getCcuCRE(pia->getKey()) << std::endl ;
//	  }
	}
	catch (FecExceptionHandler &e) {
	  error = true ;
	  if (displayMessage) 
	    std::cerr << "Error during the check of the CCU CRE, corresponding PIA reset will not be issued: " << e.what() << std::endl ;
	  errorReportLogger.errorReport ("Error during the check of the CCU CRE, corresponding PIA may not be issued !!!!", e, LOGERROR) ;
	}
      }
      if (error && displayMessage) {
	std::cerr << "One of the PIA channel is in error, not all PIA reset be issued" << std::endl ;
      }
    }
    else {
      
      errorReportLogger.errorReport ("No PIA reset to be downloaded", LOGERROR) ;
      listErrors.push_back("No PIA reset to be downloaded") ;
    }
    
    // Delete the PIA reset and the devices
    try {
      // Delete PIA reset
      FecFactory::deleteVectorI (vPiaReset) ;
      delete apvIn ; delete muxIn ; delete pllIn ; delete laserIn ; delete dohIn ;  
      // Delete Devices
      FecFactory::deleteVectorI (vDevices) ;
      if (deviceCorrect != &vDevices) delete deviceCorrect ;
    }
    catch (FecExceptionHandler &e) {
      
      errorReportLogger.errorReport ("FEC: Unable to delete some devices", e, LOGFATAL,1,partitionName) ;
      listErrors.push_back("Error: FEC: Unable to delete some devices") ;
    }

    // Delete the list of the FECs
    if (fecList != NULL) delete fecList ;

    // Ok
    return uploadOk ;
  }


  /** Calibrate the DOH and upload it to the database or file 
   * \param fecAccess - hardware access to the rings
   * \param fecAccessManager - device access manager
   * \param errorReportLogger - to report the error
   * \param fecFactory - download of the devices is requiered
   * \param inputXMLFecFileName - input file for the DOH devices
   * \param outputXMLFecFileName - output file for the XML devices
   * \param partitionName - partition for database upload
   * \param displayMessage - display the message
   * \param multiFrames - use the multi-frames mode
   * \param doUploadInDatabase - upload in database
   * \param doUploadInFile - upload in file
   * \param gain - gain value for the DOH (by default 1)
   * \param fecRingSlotStart - specify a start slot if needed or a specific ring
   * \param fecRingSlotStop - specify a end slot if needed or a specific ring (same as start if you want to check for a ring). If a range is specify then all the rings are detected (from slot state to slot stop and from ring min (0 or 1) and ring max (7 or 8)
   * \param devVersMajor - version major of the device upload
   * \param devVersMajor - version minor of the device upload
   * \warning the FecAccess, FecAccessManager, FecFactory must not be NULL or the program will crash
   * \return true if no fatal error
   * \warning The DOH are downloaded from a file or database
   * \warning Please note the rings which have are in fault (have been reconfigured) cannot have a calibration of the DOH, the procedure to have it included is really hard to implement
   */
  static bool uploadDOHCalibrated ( 
				   // Parameters for the access hard + DB + error repot
				   FecAccess &fecAccess,
				   FecAccessManager &fecAccessManager, 
				   ErrorReportLogger &errorReportLogger,
				   FecFactory &fecFactory,

				   // File name
				   std::string inputXMLFecFileName,
				   std::string outputXMLFecFileName,
				   std::string partitionName,

				   // Configuration
				   bool displayMessage,
				   bool multiFrames,
				   bool doUploadInDatabase,
				   bool doUploadInFile,

				   // Gain value for calibration of the DOH
				   unsigned char gain = 1,

				   // Start ring and stop ring
				   keyType fecRingSlotStart = NOFECRING, 
				   keyType fecRingSlotStop  = NOFECRING, 

				   // Versions
				   unsigned int *devVersMajor = NULL,
				   unsigned int *devVersMinor = NULL,

				   // force the redundancy reconfiguration
				   bool forceApplyRedundancy = false
				   ) {
    
    bool uploadOk = true ;

    // Reset all rings and delete the devices
    // --------------------------------------------------------------------------------
    // Check if a ring has been specified
    std::list<keyType> *fecList = NULL ;
    if (fecRingSlotStart == NOFECRING) fecList = fecAccess.getFecList() ;
    else if (fecRingSlotStart == fecRingSlotStop) {
      fecAccess.getFecRingDevice ( fecRingSlotStart ) ;
      fecList = new std::list<keyType> ;
      fecList->push_back(fecRingSlotStart) ;
      //std::cout << "Look on the FEC " << getFecKey(fecRingSlot) << "." << getRingKey(fecRingSlot) << std::endl ;
    }
    else {

      // Slot min, slot max
      unsigned int fecSlotStart = getFecKey(fecRingSlotStart) > getFecKey (fecRingSlotStop) ? getFecKey (fecRingSlotStop) : getFecKey(fecRingSlotStart) ;
      unsigned int fecSlotStop  = getFecKey(fecRingSlotStart) > getFecKey (fecRingSlotStop) ? getFecKey (fecRingSlotStart) : getFecKey(fecRingSlotStop) ;

      if (displayMessage) 
	std::cout << "Checking FEC between slot " << std::dec << fecSlotStart << " and " << fecSlotStop << std::endl ;

      std::list<keyType> *listoffecring = fecAccess.getFecList() ;
      for (std::list<keyType>::iterator it = listoffecring->begin() ; it != listoffecring->end() ; it ++) {
	if (getFecKey(*it) >= fecSlotStart && getFecKey(*it) <= fecSlotStop) {
	  if (fecList == NULL) fecList = new std::list<keyType> ;
	  fecList->push_back(*it) ;
	}
      }
      delete listoffecring ;
      
      if (displayMessage)
	if (fecList != NULL)
	  std::cout << "Number of FEC,ring " << std::dec << fecList->size() << std::endl; 
	else 
	  std::cout << "Number of FEC,ring " << std::dec << 0 << std::endl; 
    }

    // --------------------------------------------------------------------------------
    // Reset all rings
    if ((fecList != NULL) && !fecList->empty()) {
      fecAccessManager.removeDevices() ;
      for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
	try {
	  fecAccess.fecRingResetFSM((*it)) ;
	  fecAccess.fecRingReset((*it)) ;
	  fecAccess.fecRingResetB((*it)) ;
	  std::ostringstream msgInfo ; msgInfo << "Reset the FEC on ring " << std::dec << getFecKey((*it)) << "." << getRingKey((*it)) ;
	  errorReportLogger.errorReport (msgInfo.str(), LOGDEBUG) ;
	}
	catch (FecExceptionHandler &e) {
	  std::ostringstream msgError ; msgError << "FEC: Unable to reset the FEC on ring " << std::dec << getFecKey((*it)) << "." << getRingKey((*it)) ;
	  errorReportLogger.errorReport (msgError.str(), e, LOGFATAL, 1, partitionName) ;
	}
      }
      //delete fecList ;
    }
    else {
      errorReportLogger.errorReport ("FATAL Error: FEC: No FEC has been detected",LOGFATAL) ;
      if (fecList != NULL) delete fecList ;
      return false ;
    }

    // --------------------------------------------------------------
    // Redo the redundancy for the rings which needs it
    if (fecFactory.getDbUsed()) {

      for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
	// try to find the redundancy configuration for the rings
	try {
	  if (forceApplyRedundancy || !isFecSR0Correct(fecAccess.getFecRingSR0(*it))) {
	    TkRingDescription* myRing = fecFactory.getDbRingDescription(fecAccess.getFecHardwareId(*it),getRingKey(*it));
	    setRedundancy ( fecAccess, *it, *myRing, forceApplyRedundancy, errorReportLogger ) ;
	  }
	}
	catch (FecExceptionHandler &e) {
	 
	  errorReportLogger.errorReport ("Error during the reconfiguration",e,LOGERROR) ;
	}
	catch (oracle::occi::SQLException &e) {

	  FecDeviceFactory *fecDeviceFactory = (FecDeviceFactory *)(&fecFactory) ;
	  errorReportLogger.errorReport (fecDeviceFactory->what("Error during the reconfiguration (OCCI exception) for partition", e), LOGFATAL) ;
	}
      }
    }
    else { 
      errorReportLogger.errorReport ("No reconfiguration possible with no access to database", LOGWARNING) ;
    }

    // ---------------------------------------------------------------------------------------------------
    // Download devices from file or database (file is coming from the output file of the scan of the FECs)
    deviceVector vDevices, dohDevices ;
    piaResetVector vPiaReset ;
    try {
#ifdef DATABASE
      if (fecFactory.getDbUsed() && partitionName.size()) { // Database download
	fecFactory.setUsingDb() ;
	fecFactory.getFecDeviceDescriptions ( partitionName, vDevices, 0, 0, true ) ;
      }
      else 
#endif
	{
	errorReportLogger.errorReport (std::string("Parse the file ") + inputXMLFecFileName + std::string(" for DOH devices"), LOGDEBUG) ;
	std::ifstream inputFile(inputXMLFecFileName.c_str()) ;
	if (inputFile.is_open()) {
	  inputFile.close() ;
	}
	else {
	  errorReportLogger.errorReport (std::string("The file ") + inputXMLFecFileName + std::string(" is not existing, cannot find the DOH devices"), LOGERROR) ;
	  return false ;
	}
	
	fecFactory.setUsingFile() ;
	fecFactory.setInputFileName(inputXMLFecFileName) ;
	fecFactory.getFecDeviceDescriptions (vDevices,true) ;
	fecFactory.getPiaResetDescriptions (vPiaReset,true) ; 
      }
    }
#ifdef DATABASE
    catch (oracle::occi::SQLException &e) {
      if (displayMessage) 
	std::cerr << ((FecDeviceFactory)fecFactory).what("Error during the parsing for the DOH devices in the database (OCCI exception) for partition" + partitionName, e) ;
      
      errorReportLogger.errorReport (((FecDeviceFactory)fecFactory).what("Error during the parsing for the DOH devices in the database (OCCI exception) for partition" + partitionName, e), LOGFATAL) ;

      return false ;
    }
#endif
    catch (FecExceptionHandler &e) {
      if (displayMessage) 
	std::cerr << "Error during the parsing for the DOH devices " << e.what() << std::endl ;
      errorReportLogger.errorReport ("Error during the parsing of the DOH devices", e, LOGERROR) ;
      return false ;
    }

    // Sort it to have only the DOH
    dohDevices = FecFactory::getDeviceFromDeviceVector(vDevices,DOH) ;
    if (!dohDevices.size()) {
      if (displayMessage) 
	std::cerr << "No DOH devices found for the DOH calibration" << std::endl ;
      errorReportLogger.errorReport ("No DOH devices found for the DOH calibration", LOGERROR) ;
      return false ;
    }

    // Calibrate the DOH
    std::list<FecExceptionHandler *> errorListDevice ;
    Sgi::hash_map<keyType, bool> dohError ;
    try {
      fecAccessManager.calibrateDOH ( dohDevices, errorListDevice, gain ) ;
      displayErrorMessage ("DOH calibration", errorListDevice, errorReportLogger, &dohError, partitionName) ;
    }
    catch (FecExceptionHandler &e) {
      if (displayMessage) 
	std::cerr << "An error occurs during the DOH calibration: " << e.what() << std::endl ;
      errorReportLogger.errorReport ("An error occurs during the DOH calibration", e, LOGERROR) ;
      return false ;
    }

    // --------------------------------------------------------------------------------
    // Reset the rings
    for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
      try {
	fecAccess.fecRingReset((*it)) ;
	fecAccess.fecRingResetB((*it)) ;
      }
      catch (FecExceptionHandler &e) {
	std::ostringstream msgError ; msgError << "FEC: Unable to reset the FEC on ring " << std::dec << getFecKey((*it)) << "." << getRingKey((*it)) ;
	errorReportLogger.errorReport (msgError.str(), e, LOGFATAL, 1, partitionName) ;
      }
    }

    // --------------------------------------------------------------
    // Redo the redundancy for the rings which needs it
    if (fecFactory.getDbUsed()) {

      for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
	// try to find the redundancy configuration for the rings
	try {
	  if (forceApplyRedundancy || !isFecSR0Correct(fecAccess.getFecRingSR0(*it))) {
	    TkRingDescription* myRing = fecFactory.getDbRingDescription(fecAccess.getFecHardwareId(*it),getRingKey(*it));
	    setRedundancy ( fecAccess, *it, *myRing, forceApplyRedundancy, errorReportLogger ) ;
	  }
	}
	catch (FecExceptionHandler &e) {
	 
	  errorReportLogger.errorReport ("Error during the reconfiguration",e,LOGERROR) ;
	}
	catch (oracle::occi::SQLException &e) {

	  FecDeviceFactory *fecDeviceFactory = (FecDeviceFactory *)(&fecFactory) ;
	  errorReportLogger.errorReport (fecDeviceFactory->what("Error during the reconfiguration (OCCI exception) for partition", e), LOGFATAL) ;
	}
      }
    }
    else { 
      errorReportLogger.errorReport ("No reconfiguration possible with no access to database", LOGWARNING) ;
    }

    // ----------------------------------------------------------------------
    // Read back the values
    std::list<FecExceptionHandler *> errorListDeviceUpload ;
    deviceVector *deviceUploaded = NULL ;
    try {
      if (!multiFrames) {
	errorReportLogger.errorReport ("Upload the DOH devices", LOGDEBUG) ;
	deviceUploaded = fecAccessManager.uploadValues ( errorListDeviceUpload ) ;
      }
      else {
	errorReportLogger.errorReport ("Upload the DOH devices through multiple frames", LOGDEBUG) ;
	deviceUploaded = fecAccessManager.uploadValuesMultipleFrames ( errorListDeviceUpload ) ;
      }
    }
    catch (FecExceptionHandler &e) {
      if (displayMessage) 
	std::cerr << "An error occurs during the DOH upload: " << e.what() << std::endl ;
      errorReportLogger.errorReport ("An error occurs during the DOH upload", e, LOGERROR) ;
      return false ;
    }
    displayErrorMessage ("DOH upload", errorListDevice, errorReportLogger, &dohError, partitionName) ;

    // Upload it to the database or to the file
#ifdef DATABASE
    if (doUploadInDatabase && fecFactory.getDbConnected()) {
     
      try {
	fecFactory.setUsingDb() ;
	unsigned int major = 0, minor = 0 ;
	fecFactory.setFecDeviceDescriptions (vDevices,partitionName,&major,&minor) ;
	
	if (devVersMajor != NULL && devVersMinor != NULL) {
	  *devVersMajor = major ;
	  *devVersMinor = minor ;
	}
	
	//if (displayMessage) {
	std::cout << "-------------------------- Version ---------------------" << std::endl ;
	std::cout << "Upload the device version " << std::dec << major << "." << minor << " in the database" << std::endl ;
	std::cout << "--------------------------------------------------------" << std::endl ;
	//}
	
	uploadOk = true ;
      }
      catch (FecExceptionHandler &e) {
	if (displayMessage) 
	  std::cerr << "Error during the upload in the database (FecExceptionHandler exception):" <<  e.what() << std::endl ;

	errorReportLogger.errorReport ("FEC: XML parsing for database error", e, LOGFATAL,1,partitionName) ;
	uploadOk = false ;
      }
      catch (oracle::occi::SQLException &e) {
	if (displayMessage) 
	  std::cerr << ((FecDeviceFactory)fecFactory).what("Error during the upload in the database (OCCI exception) for partition" + partitionName, e) ;      

	errorReportLogger.errorReport (((FecDeviceFactory)fecFactory).what("Error during the upload in the database (OCCI exception) for partition" + partitionName, e), LOGFATAL) ;

	uploadOk = false ;
      }
    }
    else 
#endif
      {
	if (doUploadInFile) {
	  try {
	    // Example to create a new major version with the device uploaded 
	    // and input file it is set
	    fecFactory.setUsingFile(true) ;
	    fecFactory.setOutputFileName(outputXMLFecFileName) ;
	    if (! vPiaReset.empty())
	      fecFactory.setFecDevicePiaDescriptions ( vDevices, vPiaReset ) ;
	    else
	      fecFactory.setFecDeviceDescriptions (vDevices) ;
	    
	    //if (displayMessage) 
	    std::cout << "-------------- Upload it in file " << outputXMLFecFileName << " done" << std::endl ;
	    
	    uploadOk = true ;
	  }
	  catch (FecExceptionHandler &e) {
	    
	    errorReportLogger.errorReport ("FEC: XML upload in file error", e, LOGFATAL,1,partitionName) ;
	    uploadOk = false ;
	  }
	}
      }

    // Delete the list of the FECs
    if (fecList != NULL) delete fecList ;

    return uploadOk ;
  }

  /** This method apply the reconfiguration if needed or it is forced to the ring
   * \param fecAccess - access to the FEC
   * \param index - index of the ring
   * \param myRing - ring description retreived from database or file
   * \param forceApplyRedundancy - force the redundancy even if the ring is working
   * \param errorReportLogger - to report the error
   */
  static bool setRedundancy ( FecAccess &fecAccess, keyType index, TkRingDescription &tkRing, bool forceApplyRedundancy, ErrorReportLogger &errorReportLogger ) {

    bool error = true ;

    try {
      tscType16 fecSR0 = fecAccess.getFecRingSR0(index) ;
      if (forceApplyRedundancy || !isFecSR0Correct(fecSR0)) {
	
	// -------------------------------------------------------------------------
	// if the ring is closed then try to recover it
	// 3 times to see if the ring is occiliating
	if (isLinkInit(fecAccess.getFecRingSR0(index)) && 
	    isLinkInit(fecAccess.getFecRingSR0(index)) && 
	    isLinkInit(fecAccess.getFecRingSR0(index))) {
	  
	  // -------------------------------------------------------------------------
	  // clean the FIFOs
	  fecSR0 = fecAccess.getFecRingSR0(index) ;
	  try {
	    if (!isFifoReceiveEmpty(fecSR0)) fecAccess.emptyFifoReceive(index) ;
	    if (!isFifoTransmitEmpty(fecSR0)) fecAccess.emptyFifoTransmit(index) ;
	    if (!isFifoReturnEmpty(fecSR0)) fecAccess.emptyFifoReturn(index) ;
	  } 
	  catch (FecExceptionHandler &e) { 
	  } // mersi patch
	  
	  fecSR0 = fecAccess.getFecRingSR0(index) ;
	  if (isFecSR0Correct(fecSR0)) {
	    std::stringstream msgInfo ; msgInfo << "Ring on FEC " << getFecKey(index) << "." << getRingKey(index) << " SR0 = 0x" << std::hex << fecSR0 << std::dec ;
	    errorReportLogger.errorReport (msgInfo.str(), LOGUSERINFO) ;
	  }
	}
	
	// -------------------------------------------------------------------------
	// apply the redundancy if the ring is not correctly defined
	if (forceApplyRedundancy || !isFecSR0Correct(fecSR0)) {

	  if (!tkRing.isReconfigurable()) {
	    std::stringstream msgInfo ; 
	    msgInfo << "The configuration given for FEC " << getFecKey(index) << " ring " << getRingKey(index) << " is not correct, please check it: " ;
	    tkRing.display(msgInfo) ;
	    std::string detailedError = tkRing.getReconfigurationProblem();
	    msgInfo << ": " << detailedError ;
	    errorReportLogger.errorReport (msgInfo.str(), LOGERROR) ;
	    error = true ;
	  }
	  else {
	    try {
	      std::stringstream msgInfo ;
	      msgInfo << "Apply redundancy on FEC " << getFecKey(index) << "." << getRingKey(index) << ": " ; tkRing.display(msgInfo) ;
	      errorReportLogger.errorReport (msgInfo.str(), LOGUSERINFO) ;
	      fecSR0 = fecAccess.fecRingReconfigure (index, tkRing) ;
	      if (!isFecSR0Correct(fecSR0)) {
		std::stringstream msgError ; 
		msgError << "The configuration given for FEC " << getFecKey(index) << " ring " << getRingKey(index) 
			 << " returns the status 0x" << std::hex << fecSR0 << std::dec ;
		errorReportLogger.errorReport (msgError.str(), LOGERROR) ;
		error = true ;
	      }
	      else {
		std::stringstream msgInfo ; 
		msgInfo << "The configuration given for FEC " << getFecKey(index) << " ring " << getRingKey(index) 
			<< " returns the status 0x" << std::hex << fecSR0 << std::dec << " and was applied correctly" ;
		errorReportLogger.errorReport (msgInfo.str(), LOGUSERINFO) ;
		error = false ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::stringstream msgError ; msgError << "Unable to apply the redundancy on FEC " << getFecKey(index) << " ring " << getRingKey(index) << ": " << e.what() ;
	      errorReportLogger.errorReport (msgError.str(), LOGERROR) ;
	      error = true ;
	    }
	  }
	}
      }
    }
    catch (FecExceptionHandler &e) {
      std::stringstream msgError ; msgError << "Unable to retreive FEC status on FEC " << getFecKey(index) << " ring " << getRingKey(index) << ": " << e.what() ;
      errorReportLogger.errorReport (msgError.str(), LOGERROR) ;
      error = true ;
    }

    return error ;
  }
} ;

#endif
