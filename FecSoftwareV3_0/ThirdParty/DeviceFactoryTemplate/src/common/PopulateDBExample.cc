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

#include "TkDcuInfo.h"
#include "TkDcuConversionFactors.h"
#include "deviceDescription.h"
#include "apvDescription.h"
#include "muxDescription.h"
#include "pllDescription.h"
#include "dcuDescription.h"
#include "laserdriverDescription.h" // AOH and DOH

/* ************************************************************************************************ */
/*                                                                                                  */
/*                             Det ID, DCU Hard ID, Fiber length, APV number                        */
/*                                                                                                  */
/* ************************************************************************************************ */
/** Set the complete information about det id: Det ID, DCU Hard ID, Fiber length, APV number
 * \param vDcuInfo - vector of TkDcuInfo
 * \param deviceFactory - database access
 * \param partitionName - partition name
 */
void setConversionFactors ( tkDcuInfoVector vDcuInfoPartition, DeviceFactory &deviceFactory, std::string partitionName ) 
  throw (FecExceptionHandler) {

  // Create the conversion factors
  dcuConversionVector vConversionFactors ;

  // Create the corresponding parameters for the conversion factors
  std::string subDetector = "None" ;
  if (partitionName.find("TEC",0) != std::string::npos) subDetector = "TEC" ;
  if (partitionName.find("TIB",0) != std::string::npos) subDetector = "TIB" ;
  if (partitionName.find("TID",0) != std::string::npos) subDetector = "TID" ;
  if (partitionName.find("TOB",0) != std::string::npos) subDetector = "TOB" ;
  if (subDetector == "None") {
    std::cerr << "Warning: Unknown partition name, the sub detector for the DCU conversion factors should be TEC,TIB,TOB or TID, set the subdetector as TEC" << std::endl ;
    subDetector = "TEC" ;
  }

  TkDcuConversionFactors tkDcuConversionFactorsStatic ( 0, subDetector, DCUFEH ) ;
  tkDcuConversionFactorsStatic.setAdcGain0(2.144) ;
  tkDcuConversionFactorsStatic.setAdcOffset0(0) ;
  tkDcuConversionFactorsStatic.setAdcCal0(false) ;
  tkDcuConversionFactorsStatic.setAdcInl0(0) ;
  tkDcuConversionFactorsStatic.setAdcInl0OW(true) ;
  tkDcuConversionFactorsStatic.setI20(0.02122);
  tkDcuConversionFactorsStatic.setI10(0.01061);
  tkDcuConversionFactorsStatic.setICal(false) ;
  tkDcuConversionFactorsStatic.setKDiv(0.56) ;
  tkDcuConversionFactorsStatic.setKDivCal(false) ;
  tkDcuConversionFactorsStatic.setTsGain(8.9) ;
  tkDcuConversionFactorsStatic.setTsOffset(2432) ;
  tkDcuConversionFactorsStatic.setTsCal(false) ;
  tkDcuConversionFactorsStatic.setR68(0) ;
  tkDcuConversionFactorsStatic.setR68Cal(false) ;
  tkDcuConversionFactorsStatic.setAdcGain2(0) ;
  tkDcuConversionFactorsStatic.setAdcOffset2(0) ;
  tkDcuConversionFactorsStatic.setAdcCal2(false) ;
  tkDcuConversionFactorsStatic.setAdcGain3(0) ;
  tkDcuConversionFactorsStatic.setAdcCal3(false) ;

  // For each det id create the conversion factors
  for (tkDcuInfoVector::iterator it = vDcuInfoPartition.begin() ; it != vDcuInfoPartition.end() ; it ++) {
    
    TkDcuInfo *tkDcuInfo = *it ;

    TkDcuConversionFactors *tkDcuConversionFactors = new TkDcuConversionFactors ( tkDcuConversionFactorsStatic ) ;
    tkDcuConversionFactors->setDetId(tkDcuInfo->getDetId()) ;
    tkDcuConversionFactors->setDcuHardId(tkDcuInfo->getDcuHardId()) ;
    vConversionFactors.push_back(tkDcuConversionFactors) ;
  }

  // Display
  //for (dcuConversionVector::iterator it = vConversionFactors.begin() ; it != vConversionFactors.end() ; it ++)
  //(*it)->display() ;

  if (deviceFactory.getDbUsed()) {
    // Submit to the database
    std::cout << "Upload database with the conversion factors for " << vConversionFactors.size() << " devices" << std::endl ;
  }
  else {
    std::cout << "Upload the conversion factors in file " << "/tmp/conversionFactors.xml" << std::endl ;
    deviceFactory.setOutputFileName ("/tmp/conversionFactors.xml") ;
  }

  // Upload
  deviceFactory.setTkDcuConversionFactors ( vConversionFactors ) ;

  // Delete the conversion factors
  for (dcuConversionVector::iterator iti = vConversionFactors.begin() ; iti != vConversionFactors.end() ; iti ++) 
    delete *iti ;
}

/** Give an index for each module
 * \param vDcuInfo - list of module for one partition
 * \param vModuleInfo - Module with the index (inherits from TkDcuInfo)
 * \param partitionName - name of the partition
 */
void fillIndex ( tkDcuInfoVector vDcuInfoPartition, Sgi::hash_map<unsigned long, keyType> &detIdPosition, std::string partitionName ) {

  // 10 modules per CCU, number of modules per FEC, then per Ring, then per CCU
  unsigned int numberFEC  = (unsigned int)((vDcuInfoPartition.size() / 11) + 1) ;
  unsigned int numberRing = (unsigned int)((numberFEC / 8) + 1) ;
  unsigned int numberCCU  = (unsigned int)((numberRing / 10) + 1) ;

  // Index
  unsigned int fecSlot = 1 ;        // from 1 to 11
  unsigned int fecRing = 0 ;        // from 0 to 7
  unsigned int ccuAddress = 0x1 ;   // CCU number are linear
  unsigned int i2cChannel = 0x11 ;  // 0x10 is kept for DOH and DCU on CCU

  // For each det id
  for (tkDcuInfoVector::iterator it = vDcuInfoPartition.begin() ; it != vDcuInfoPartition.end() ; it ++) {

    TkDcuInfo *tkDcuInfo = *it ;

    // A switch case can be introduced at that level to switch between TID, TIB, TOB, TEC
    keyType index = buildCompleteKey(fecSlot,fecRing,ccuAddress,i2cChannel,0) ;
    detIdPosition[tkDcuInfo->getDetId()] = index ;

    char msg[80] ; decodeKey(msg, index) ; std::cout << tkDcuInfo->getDetId() << ": " << msg << std::endl ;
    
    // Next index: 
    // 10 modules on every CCU
    i2cChannel ++ ;
    if (i2cChannel == 0x1B) { // next CCU
      i2cChannel = 0x11 ;
      ccuAddress ++ ; 
      if (ccuAddress > numberCCU) { // next ring
	ccuAddress = 0x1 ;
	fecRing ++ ; 
	if (fecRing == 8) {
	  fecRing = 0 ;
	  fecSlot ++ ;
	  if (fecSlot == 12) {
	    std::cerr << "Error: the number of FEC, ring, CCU, channel cannot handle the number of modules:" << std::endl ;
	    std::cerr << "Number of modules in the partition " << partitionName << ": " << vDcuInfoPartition.size() << std::endl ;
	    std::cerr << "Repartition : " << std::endl ;
	    std::cerr << "\t Number of modules per FEC: " << numberFEC << std::endl ;
	    //std::cerr << "\t Number of rings: " << numberRing << std::endl ;
	    std::cerr << "\t Number of modules per CCU: " << numberCCU << std::endl ;
	    std::cerr << "Press a key to continue" ; getchar() ;
	    return ;
	  }
	}
      }
    }
  }
}

/** Create and upload the devices to the DB
 * \param vDcuInfo - list of module for one partition
 * \param vModuleInfo - Module with the index (inherits from TkDcuInfo)
 * \param partitionName - name of the partition
 * \param crateNumber - crate number useed to change the value of the DCU hard id for the DCU on CCU
 */
void uploadDevices ( tkDcuInfoVector vDcuInfoPartition, Sgi::hash_map<unsigned long, keyType> detIdPosition, 
		     std::string partitionName, DeviceFactory &deviceFactory,
		     unsigned int crateNumber )
  throw (FecExceptionHandler) {

  // Default parameters
  // Values for DOH
  tscType8 gainDOH = 2 ;
  tscType8 biasDOH[3] = {24, 24, 24} ;
  tscType8 gainAOH = 2 ; 
  tscType8 biasAOH[3] = {23, 23, 23} ;
  apvDescription apvStatic ((tscType8)0x2b,
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
  laserdriverDescription dohStatic (gainDOH,biasDOH) ;
  laserdriverDescription aohStatic (gainAOH,biasAOH) ;
  muxDescription muxStatic ((tscType16)0xFF) ;
  pllDescription pllStatic ((tscType8)6,(tscType8)1) ;

  // Create the corresponding parameters for the conversion factors
  std::string subDetector = "None" ;
  if (partitionName.find("TEC",0) != std::string::npos) subDetector = "TEC" ;
  if (partitionName.find("TIB",0) != std::string::npos) subDetector = "TIB" ;
  if (partitionName.find("TID",0) != std::string::npos) subDetector = "TID" ;
  if (partitionName.find("TOB",0) != std::string::npos) subDetector = "TOB" ;
  if (subDetector == "None") {
    std::cerr << "Warning: Unknown partition name, the sub detector for the DCU conversion factors should be TEC,TIB,TOB or TID, set the subdetector as TEC" << std::endl ;
    subDetector = "TEC" ;
  }

  TkDcuConversionFactors tkDcuConversionFactorsStatic ( 0, subDetector, DCUCCU ) ;
  tkDcuConversionFactorsStatic.setAdcGain0(2.144) ;
  tkDcuConversionFactorsStatic.setAdcOffset0(0) ;
  tkDcuConversionFactorsStatic.setAdcCal0(false) ;
  tkDcuConversionFactorsStatic.setAdcInl0(0) ;
  tkDcuConversionFactorsStatic.setAdcInl0OW(true) ;
  tkDcuConversionFactorsStatic.setI20(0.02122);
  tkDcuConversionFactorsStatic.setI10(.01061);
  tkDcuConversionFactorsStatic.setICal(false) ;
  tkDcuConversionFactorsStatic.setKDiv(0.56) ;
  tkDcuConversionFactorsStatic.setKDivCal(false) ;
  tkDcuConversionFactorsStatic.setTsGain(8.9) ;
  tkDcuConversionFactorsStatic.setTsOffset(2432) ;
  tkDcuConversionFactorsStatic.setTsCal(false) ;
  tkDcuConversionFactorsStatic.setR68(0) ;
  tkDcuConversionFactorsStatic.setR68Cal(false) ;
  tkDcuConversionFactorsStatic.setAdcGain2(0) ;
  tkDcuConversionFactorsStatic.setAdcOffset2(0) ;
  tkDcuConversionFactorsStatic.setAdcCal2(false) ;
  tkDcuConversionFactorsStatic.setAdcGain3(0) ;
  tkDcuConversionFactorsStatic.setAdcCal3(false) ;

  // Vector
  dcuConversionVector vConversionFactors ;
  deviceVector vDevice ;
  piaResetVector vPiaReset  ;

  unsigned int oldCCU = 0, oldRing = 10, oldFec = 1 ;
  unsigned int dcuHardId = vDcuInfoPartition.size() + 1 ;
  if (crateNumber != 0) dcuHardId += (crateNumber << 16) ;

  // time 
  unsigned long timetotal = 0 ;
  bool createPartitionFlag = true ;

  // For each det id
  for (tkDcuInfoVector::iterator it = vDcuInfoPartition.begin() ; it != vDcuInfoPartition.end() ; it ++) {

    TkDcuInfo *tkDcuInfo = *it ;
    keyType index = detIdPosition[tkDcuInfo->getDetId()] ;
    //std::cout << "FEC hardware Id = " << partitionName << toString(getFecKey(index)) << std::endl ;
    std::string fecHardwareId = partitionName + toString(getFecKey(index)) ;
    unsigned int crateId = 1 ;

    char msg[80] ; decodeKey(msg,index) ; std::cout << msg << std::endl ;

    // For each CCU create the corresponding DCU and add the conversion factors for it
    // For each CCU create the corresponding PIA reset
    if ( (oldCCU != getCcuKey(index)) || (oldRing != getRingKey(index)) ) {

      // if CCU == 1 or 2 then create the corresponding DOH
      if (getCcuKey(index) == 0x1) {
	
	//std::cout << "Adding DOH on CCU 0x1 0x10 0x70" << std::endl ;

	laserdriverDescription *dohd = new laserdriverDescription(dohStatic) ;
	dohd->setAccessKey(buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),0x10,0x70)) ;
	//unsigned int fecHardId = getFecKey(index) ; //+ ((crateNumber & 0xF) << 28) ;
	dohd->setFecHardwareId(fecHardwareId,crateId) ;
	vDevice.push_back(dohd) ;
      }
      if (getCcuKey(index) == 0x2) {
	
	//std::cout << "Adding DOH on CCU 0x2 0x10 0x70" << std::endl ;
	
	laserdriverDescription *dohd = new laserdriverDescription(dohStatic) ;
	dohd->setAccessKey(buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),0x10,0x70)) ;
	dohd->setFecHardwareId(fecHardwareId,crateId) ;
	vDevice.push_back(dohd) ;
      }

      //std::cout << "Adding DCU on CCU on FEC " << std::dec << getFecKey(index) << " Ring " << getRingKey(index) << " CCU 0x" << std::hex << getCcuKey(index) << " 0x10 0x0: " << std::dec << buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),0x10,0x0) << std::endl ;
      dcuDescription *dcud = new dcuDescription (buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),0x10,0x0),
						 0,
 						 dcuHardId,
 						 0,0,0,0,0,0,0,0,
						 DCUCCU) ;
      dcud->setFecHardwareId(fecHardwareId,crateId) ;
      vDevice.push_back(dcud) ;
      TkDcuConversionFactors *tkDcuConversionFactors = new TkDcuConversionFactors ( tkDcuConversionFactorsStatic ) ;
      tkDcuConversionFactors->setDetId(0) ;
      tkDcuConversionFactors->setDcuHardId(dcuHardId) ;
      vConversionFactors.push_back(tkDcuConversionFactors) ;
      dcuHardId ++ ;

      //std::cout << "Adding PIA on CCU 0x" << std::hex << getCcuKey(index) << " 0x30 0x0" << std::endl ;

      piaResetDescription *piad = new piaResetDescription (buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),0x30,0x0),
							   10, 10000, 0xFF) ;
      piad->setFecHardwareId(fecHardwareId,crateId) ;
      vPiaReset.push_back(piad) ;

      oldCCU = getCcuKey(index) ;
    }

    // For each ring add the corresponding dummy CCU with a DCU
    if (oldRing != getRingKey(index)) {
      
      //std::cout << "Adding DCU on FEC " << std::dec << getFecKey(index) << " Ring " << getRingKey(index) << " on CCU 0x7F 0x10 0x0: " << std::dec << buildCompleteKey(getFecKey(index),getRingKey(index),0x7F,0x10,0x0) << std::endl ;
      dcuDescription *dcud = new dcuDescription (buildCompleteKey(getFecKey(index),getRingKey(index),0x7F,0x10,0x0),
						 0, 
						 dcuHardId,
						 0,0,0,0,0,0,0,0,
						 DCUFEH) ; 
      dcud->setFecHardwareId(fecHardwareId,crateId) ;
      vDevice.push_back(dcud) ;
      TkDcuConversionFactors *tkDcuConversionFactors = new TkDcuConversionFactors ( tkDcuConversionFactorsStatic ) ;
      tkDcuConversionFactors->setDetId(0) ; // No DET ID
      tkDcuConversionFactors->setDcuHardId(dcuHardId) ;
      vConversionFactors.push_back(tkDcuConversionFactors) ;
      dcuHardId ++ ;


      oldRing = getRingKey(index) ;
    }

    // Create the corresponding devices for the module

    // APV
    apvDescription *apv = new apvDescription (apvStatic) ;
    apv->setAccessKey(index | setAddressKey(0x20)) ;
    apv->setFecHardwareId(fecHardwareId,crateId) ;
    vDevice.push_back(apv) ;

    //std::cout << "Adding APV on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x20" << std::endl ;

    apv = new apvDescription (apvStatic) ;
    apv->setAccessKey(index | setAddressKey(0x21)) ;
    apv->setFecHardwareId(fecHardwareId,crateId) ;
    vDevice.push_back(apv) ;

    //std::cout << "Adding APV on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x21" << std::endl ;

    apv = new apvDescription (apvStatic) ;
    apv->setAccessKey(index | setAddressKey(0x24)) ;
    apv->setFecHardwareId(fecHardwareId,crateId) ;
    vDevice.push_back(apv) ;

    //std::cout << "Adding APV on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x24" << std::endl ;

    apv = new apvDescription (apvStatic) ;
    apv->setAccessKey(index | setAddressKey(0x25)) ;
    apv->setFecHardwareId(fecHardwareId,crateId) ;
    vDevice.push_back(apv) ;

    //std::cout << "Adding APV on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x25" << std::endl ;

    if (tkDcuInfo->getApvNumber() == 6) {
      apv = new apvDescription (apvStatic) ;
      apv->setAccessKey(index | setAddressKey(0x22)) ;
      apv->setFecHardwareId(fecHardwareId,crateId) ;
      vDevice.push_back(apv) ;

      //std::cout << "Adding APV on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x22" << std::endl ;

      apv = new apvDescription (apvStatic) ;
      apv->setAccessKey(index | setAddressKey(0x23)) ;
      apv->setFecHardwareId(fecHardwareId,crateId) ;

      //std::cout << "Adding APV on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x23" << std::endl ;

      vDevice.push_back(apv) ;
    }

    // DCU
    dcuDescription *dcud = new dcuDescription (index,
					       0,
					       tkDcuInfo->getDcuHardId(),
					       0,0,0,0,0,0,0,0) ;
    dcud->setFecHardwareId(fecHardwareId,crateId) ;
    vDevice.push_back(dcud) ;
    
    //std::cout << "Adding DCU on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x0" << std::endl ;

    // APV MUX
    muxDescription *muxd = new muxDescription(muxStatic) ;
    muxd->setAccessKey(index | setAddressKey(0x43)) ;
    muxd->setFecHardwareId(fecHardwareId,crateId) ;
    vDevice.push_back(muxd) ;

    //std::cout << "Adding APV MUX on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x43" << std::endl ;

    // AOH
    laserdriverDescription *aohd = new laserdriverDescription (aohStatic) ;
    aohd->setAccessKey(index | setAddressKey(0x60)) ;
    aohd->setFecHardwareId(fecHardwareId,crateId) ;
    vDevice.push_back(aohd) ;

    //std::cout << "Adding AOH on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x43" << std::endl ;

    // PLL
    pllDescription *plld = new pllDescription (pllStatic) ;
    plld->setAccessKey(index | setAddressKey(0x44)) ;
    plld->setFecHardwareId(fecHardwareId,crateId) ;
    vDevice.push_back(plld) ;

    //std::cout << "Adding PLL on CCU 0x" << std::hex << getCcuKey(index) << " 0x" << getChannelKey(index) << " 0x44" << std::endl ;

    tkDcuInfoVector::iterator it1 = (it + 1) ;
    // Submit to the database for each FEC
    //if ( (it1 == vDcuInfoPartition.end()) || 
    //(deviceFactory.getDbUsed() && (getFecKey(index) != oldFec)) ) {
    if (it1 == vDcuInfoPartition.end()) {

      //checkDevices (vDevice) ;

      // Upload in DB
      if (deviceFactory.getDbUsed()) {

	unsigned int devMinor = 0, devMajor = 0 ;
	oldFec = getFecKey(index) ;

	// Display
	//std::cout << "Upload the FEC " << std::dec << oldFec << std::endl ;
	std::cout << "Upload " << std::dec << vPiaReset.size() << " PIA reset descriptions" << std::endl ;
	std::cout << "Upload " << std::dec << vDevice.size() << " device descriptions" << std::endl ;
	
	createPartitionFlag = true ;
	unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	deviceFactory.createPartition(vDevice, vPiaReset, &devMajor, &devMinor, partitionName, createPartitionFlag) ;
	//deviceFactory.createPartition(vPiaReset, partitionName) ;
	//deviceFactory.createPartition(vDevice, &devMajor, &devMinor, partitionName, createPartitionFlag) ;
	unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	timetotal += (endMillis-startMillis) ;
	if (createPartitionFlag) createPartitionFlag = false ;

	// Display
	std::cout << "Upload " << std::dec << vPiaReset.size() << " in the database for partition " << partitionName << std::endl ;
	std::cout << "Upload " << std::dec << vDevice.size() << " devices version " << devMajor << "." << devMinor << " for partition " << partitionName << std::endl ;

// 	// Test if a conversion factors for these exists
// 	for (dcuConversionVector::iterator iti = vConversionFactors.begin() ; iti != vConversionFactors.end() ; iti ++) {
//  	  try {
//  	    //std::cout << (*iti)->getDcuHardId() << std::endl ;
//  	    TkDcuConversionFactors *conversionFactors = deviceFactory.getTkDcuConversionFactors ( (*iti)->getDcuHardId() ) ;
//  	    std::cout << (*iti)->getDcuHardId() << " has conversion factors (problem, the DCU hard id cannot be duplicated)" << std::endl ;
//  	    delete conversionFactors ;
//  	    getchar() ;
//  	  }
//  	  catch (FecExceptionHandler &e) {
//  	    //std::cout << "No conversion factors for DCU " << (*iti)->getDcuHardId() << std::endl ;
//  	  }
// 	}

	// ????????????????????????????????????????????
	// Submit the conversion factors
	//std::cout << "Upload " << std::dec << vConversionFactors.size() << " conversions factors" << std::endl ;
	//if (vConversionFactors.size()) deviceFactory.setTkDcuConversionFactors ( vConversionFactors ) ;
      }
      else { // Upload in FILE

	deviceFactory.setFecDevicePiaDescriptions (vDevice, vPiaReset) ;
	std::cout << "Upload of the devices done in file " << std::endl ;
      }
      
      // Delete the conversion factors
      for (dcuConversionVector::iterator iti = vConversionFactors.begin() ; iti != vConversionFactors.end() ; iti ++) 
	delete *iti ;
      vConversionFactors.clear() ;
      
      // Delete the PIA and devices
      FecFactory::deleteVectorI(vPiaReset) ; vPiaReset.clear() ;
      FecFactory::deleteVectorI(vDevice) ; vDevice.clear() ;
    }
  }

  std::cout << "The upload of the FEC devices took " << std::dec << timetotal << " ms" << std::endl ;
}

/* ************************************************************************************************ */
/*                                                                                                  */
/*                                               Main                                               */
/* -module <number of modules> */
/* -partition <partition name> */
/* -crate <crate number> */
/*                                                                                                  */
/* ************************************************************************************************ */

void help ( int argc, char **argv ) {

}

int main ( int argc, char **argv ) {

  DeviceFactory *deviceFactory = NULL ;
  std::string partitionName = "nil" ;

  tkDcuInfoVector vDcuInfoPartition ;
  Sgi::hash_map<unsigned long, keyType> detIdPosition ;

  unsigned int numberModule = 4000 ;
  unsigned int crateNumber  = 0    ;
  if (argc > 2) {
    for (int i = 1 ; i < argc ; i ++) {
      if (strncasecmp (argv[i], "-module", strlen("-module")) == 0) {
	numberModule = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
      }
      else if (strncasecmp (argv[i], "-partition", strlen("-partition")) == 0) {
	partitionName = argv[i+1] ;
	i ++ ;
      }
      else if (strncasecmp (argv[i], "-crate", strlen("-crate")) == 0) {
	crateNumber = fromString<unsigned int>(argv[i+1]) ;
	i ++ ;
      }
      else 
	std::cerr << "Invalid argument: " << argv[i] << std::endl ;
    }
  }

  // ***************************************************************************************************
  // Create the database access
  // retreive the connection of the database through an environmental variable called CONFDB
  std::string login="nil", passwd="nil", path="nil" ;
  DbAccess::getDbConfiguration (login, passwd, path) ;

  if (login != "nil" && passwd !="nil" && path != "nil") {

    try {
      // create the database access
      deviceFactory = new DeviceFactory ( login, passwd, path ) ;
      std::cout << "Press <enter> to continue" ; getchar() ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else {
    deviceFactory = new DeviceFactory ( ) ;
  }

  // *************************************************************************************************
  // Now read or retreive the det id

  // Create 4000 TkDcuInfo (det-id, fiber length, number of APV)
  // TkDcuInfo ( tscType32 dcuHardId, tscType32 detId, double fibreLength, tscType32 apvNumber )
  for (unsigned int module = 1 ; module <= numberModule ; module ++) {
    unsigned int value = module + ((crateNumber & 0xF) << 28) ;
    TkDcuInfo *tkDcuInfo = new TkDcuInfo(value,value,1,4) ;
    vDcuInfoPartition.push_back(tkDcuInfo) ;
  }

  try {
    // ***************************************************************************************************
    // Submit the detid for all partitions in DB
    std::cout << "Do you want to upload the DCU info ? " ; char r1 = getchar() ;
    if (r1 == 'y' || r1 == 'Y') {
      std::cout << "Upload the TK DCU info" << std::endl ;
      deviceFactory->setTkDcuInfo(vDcuInfoPartition) ;
    }
    
    // ***************************************************************************************************
    // Classify the DET id per partition
    
    // ***************************************************************************************************
    // For each partition
    
    // ***************************************************************************************************
    // Submit the DCU conversion factors for all partition in DB
    std::cout << "Do you want to upload the DCU conversion factors ? " ; char r2 = getchar() ;
    if (r2 == 'y' || r2 == 'Y') {
      std::cout << "Upload the conversion factors" << std::endl ;
      setConversionFactors (vDcuInfoPartition, *deviceFactory, partitionName ) ; 
    }
    
    // ***************************************************************************************************
    // Create the index for one partition
    fillIndex ( vDcuInfoPartition, detIdPosition, partitionName ) ;

    std::cout << "Found index for " << detIdPosition.size() << " modules (" << vDcuInfoPartition.size() 
	      << " det-id)" << std::endl ;

    // ***************************************************************************************************
    // Create and submit the devices to the DB
    uploadDevices ( vDcuInfoPartition, detIdPosition, partitionName, *deviceFactory, crateNumber ) ;

    // ***************************************************************************************************
    // Display some information
    //for (Sgi::hash_map<unsigned long, keyType>::iterator it = detIdPosition.begin() ; it != detIdPosition.end() ; it ++) {    
    //char msg[80] ;
    //decodeKey(msg,it->second) ;
    //std::cout << it->first << " on position " << msg << std::endl ;
    //}
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "Unable to upload information to the DB: " << e.what() << std::endl ;
    exit (EXIT_FAILURE) ;
  }

  // Delete the det id
  for (tkDcuInfoVector::iterator it = vDcuInfoPartition.begin() ; it != vDcuInfoPartition.end() ; it ++) {
    delete *it ;
  }

  if (deviceFactory != NULL) delete deviceFactory ;

  return 1 ;
}
