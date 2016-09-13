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
#include "DeviceFactory.h"

#include "XMLTkDcuConversion.h"
#include "TkDcuInfo.h"
#include "TkDcuConversionFactors.h"
#include "deviceDescription.h"
#include "apvDescription.h"
#include "muxDescription.h"
#include "pllDescription.h"
#include "dcuDescription.h"
#include "laserdriverDescription.h" // AOH and DOH

/** Reverse the DCU hard id
 */
unsigned int reverseDcuHardId ( unsigned int dcuHardId ) {

  unsigned int dcuHardIdRevert = 0 ;

  unsigned char dcuL = (dcuHardId >> 0) & 0xFF ;
  unsigned char dcuM = (dcuHardId >> 8) & 0xFF ;
  unsigned char dcuH = (dcuHardId >> 16) & 0xFF ;

  dcuHardIdRevert = dcuH | (dcuM << 8) | (dcuL << 16) ;

  return (dcuHardIdRevert) ;
}

/** Disable a set of dcus
 * -disableModule
 */
void disableModule ( DeviceFactory &deviceFactory, std::string partitionName ) {

  unsigned int dcuHardId = 0 ;
  std::vector<unsigned int> listDcus ;

  do {
    std::cout << "Please enter the DCU hard Id: " ;
    std::cin >> dcuHardId ;
    if (dcuHardId != 0) listDcus.push_back(dcuHardId) ;
  }
  while (dcuHardId != 0) ;

  std::cout << "Will disable " << listDcus.size() << " DCUs" << std::endl ;

  if (listDcus.size()) {
    try {
      deviceFactory.setEnableModules(partitionName,listDcus,false) ;
    }
    catch (FecExceptionHandler &e) {
      std::cout << e.what() << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cout << e.getMessage() << std::endl ;
    }
    catch (Fed9U::Fed9UDeviceFactoryException &e) {
      std::cout << e.what() << std::endl ;
    }
    catch (ICUtils::ICException &e) {
      std::cout << e.what() << std::endl ;
    }
  }
}

/** Reverse the DCU hard in a file and put it in an output
 */
void reverseFile (TkDcuConversionFactory &deviceFactory, std::string outputFile, std::string inputFile ) {

  std::cout << "Parsing the file " << inputFile << " for DCU conversion factors" << std::endl ;

  deviceFactory.setInputFileName(inputFile) ;
  if (deviceFactory.getConversionFactors().size() != 0) {
    std::cout << "Found " << std::dec << deviceFactory.getConversionFactors().size() << " DCU conversions factors" << std::endl ;
    std::cout << "Reversing the DCU hard id" << std::endl ;
    Sgi::hash_map<unsigned long, TkDcuConversionFactors *> conversionFactors = deviceFactory.getConversionFactors() ;
    for (Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator itDcu = conversionFactors.begin() ; itDcu != conversionFactors.end() ; itDcu ++) {
      itDcu->second->setDcuHardId(reverseDcuHardId(itDcu->second->getDcuHardId())) ;
    }

    // Starting
    if (outputFile != "") {
      if ((outputFile == "database") && !deviceFactory.getDbConnected()) 
	std::cout << "Cannot upload the data to the database, the database is not connected" << std::endl ;
      else {

	if (outputFile == "database") deviceFactory.setUsingDb() ;
	else {
	  deviceFactory.setUsingFile() ;
	  deviceFactory.setOutputFileName(outputFile) ;
	}
      
	try {
	  deviceFactory.setTkDcuConversionFactors() ;
	  std::cout << "Upload terminated" << std::endl ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "Error during the upload of the DCUs" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	}    
      }
    }
  }
  else std::cerr << "No DCU conversion factors found" << std::endl ;
}

/** Compare two dcu conversion factors and display the differences
 */
void comparisonOf2Dcus ( TkDcuConversionFactors *dcu1, TkDcuConversionFactors *dcu2, unsigned int detId ) {

  // compare the parameters
  std::cout << "DCU on " << dcu1->getDcuType() << " " << dcu1->getDcuHardId() << std::endl ;
  
  // 	  if (dcu1->getBLABLA() != dcu2->getBLABLA()) {
  // 	    std::cout << "\tConversion factors BLABLA is different " << dcu1->getBLABLA() << " / " << dcu2->getBLABLA() << std::endl ;
  // 	  }
  if (dcu1->getDcuHardId() != dcu2->getDcuHardId()) {
    std::cout << "\tConversion factors DcuHardId is different " << dcu1->getDcuHardId() << " / " << dcu2->getDcuHardId() << std::endl ;
  }

  // Found a correct det ID and set the subdetector to the correct value
  if (detId != 0) {

    // (detid << 25)&7 => 
    //          TIB 3
    //          TOB 5
    //          TEC 6
    //               (detid << 18)&3 => 1(TEC-), 2(TEC+)
    //          TID 4

    int det = (detId << 25)&7 ;
    std::string detector ;
    switch (det) {
    case 3: // TIB
      detector = "TIB" ;
      break ;
    case 4: // TID
      detector = "TID" ;
      break ;
    case 5: // TOB
      detector = "TOB" ;
      break ;
    case 6: // TEC
      detector = "TEC" ;
      // int pos = (detId << 18)&3 ;
      // if (pos == 1) detector = "TEC-" ;
      // if (pos == 2) detector = "TEC+" ;
      break ;
    default:
      detector = "none" ;
    }
    if (detector != "none") dcu2->setSubDetector(detector) ;

    dcu2->setDetId(detId) ;
    dcu2->setDcuType("FEH") ;
  }

  // unknown
  if ( (dcu1->getSubDetector() == "UNK" && dcu1->getSubDetector() == "Unknown") &&
       (dcu2->getSubDetector() == "UNK" || dcu2->getSubDetector() == "Unknown") ) {
    dcu1->setSubDetector("UNK") ;
    dcu2->setSubDetector("UNK") ;
  }

  // keep the one from second file
  if (dcu1->getSubDetector() != dcu2->getSubDetector()) {
    std::cout << "\tConversion factors SubDetector is different " << dcu1->getSubDetector() << " / " << dcu2->getSubDetector() << std::endl ;
  }

  // should be equal
  if (dcu1->getDcuType() != dcu2->getDcuType()) {
    std::cout << "\tConversion factors DcuType is different " << dcu1->getDcuType() << " / " << dcu2->getDcuType() << std::endl ;
  }

  if (dcu1->getStructureId() != dcu2->getStructureId()) {
    std::cout << "\tConversion factors StructureId is different " << dcu1->getStructureId() << " / " << dcu2->getStructureId() << std::endl ;

    // set from dcu1 to dcu2
    if (dcu1->getStructureId() != "none") dcu2->setStructureId(dcu1->getStructureId()) ;
  }
  if (dcu1->getContainerId() != dcu2->getContainerId()) {
    std::cout << "\tConversion factors ContainerId is different " << dcu1->getContainerId() << " / " << dcu2->getContainerId() << std::endl ;

    // set from dcu1 to dcu2
    if (dcu1->getContainerId() != "none") dcu2->setContainerId(dcu1->getContainerId()) ;
  }

  if (dcu1->getDetId() != dcu2->getDetId()) {
    std::cout << "\tConversion factors DetId is different " << dcu1->getDetId() << " / " << dcu2->getDetId() << std::endl ;
  }

  //if ( (dcu1->getAdcGain0() == 0) && (dcu2->getAdcGain0() < 0.0000000001) ) dcu2->setAdcGain0(0) ;
  if (dcu1->getAdcGain0() != dcu2->getAdcGain0()) {
    std::cout << "\tConversion factors AdcGain0 is different " << dcu1->getAdcGain0() << " / " << dcu2->getAdcGain0() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setAdcGain0(dcu1->getAdcGain0()) ;
  }

  //if ( (dcu1->getAdcOffset0() == 0) && (dcu2->getAdcOffset0() < 0.0000000001) ) dcu2->setAdcOffset0(0) ;
  if (dcu1->getAdcOffset0() != dcu2->getAdcOffset0()) {
    std::cout << "\tConversion factors AdcOffset0 is different " << dcu1->getAdcOffset0() << " / " << dcu2->getAdcOffset0() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setAdcOffset0(dcu1->getAdcOffset0()) ;
  }
  if (dcu1->getAdcInl0() != dcu2->getAdcInl0()) {
    std::cout << "\tConversion factors AdcInl0 is different " << dcu1->getAdcInl0() << " / " << dcu2->getAdcInl0() << " = " << (dcu1->getAdcInl0() != dcu2->getAdcInl0()) << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setAdcInl0(dcu1->getAdcInl0()) ;
  }
  //if ( (dcu1->getI20() == 0) && (dcu2->getI20() < 0.0000000001) ) dcu2->setI20(0) ;
  if (dcu1->getI20() != dcu2->getI20()) {
    std::cout << "\tConversion factors I20 is different " << dcu1->getI20() << " / " << dcu2->getI20() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setI20(dcu1->getI20()) ;
  }
  //if ( (dcu1->getI10() == 0) && (dcu2->getI10() < 0.0000000001) ) dcu2->setI10(0) ;
  if (dcu1->getI10() != dcu2->getI10()) {
    std::cout << "\tConversion factors I10 is different " << dcu1->getI10() << " / " << dcu2->getI10() << std::endl ;

     // set from dcu1 to dcu2
    dcu2->setI10(dcu1->getI10()) ;
  }

  if (dcu1->getICal() != dcu2->getICal()) {
    std::cout << "\tConversion factors ICal is different " << dcu1->getICal() << " / " << dcu2->getICal() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setICal(dcu1->getICal()) ;
  }

  //if ( (dcu1->getKDiv() == 0) && (dcu2->getKDiv() < 0.0000000001) ) dcu2->setKDiv(0) ;
  if (dcu1->getKDiv() != dcu2->getKDiv()) {
    std::cout << "\tConversion factors KDiv is different " << dcu1->getKDiv() << " / " << dcu2->getKDiv() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setKDiv(dcu1->getKDiv()) ;
  }
  if (dcu1->getKDivCal() != dcu2->getKDivCal()) {
    std::cout << "\tConversion factors KDivCal is different " << dcu1->getKDivCal() << " / " << dcu2->getKDivCal() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setKDivCal(dcu1->getKDivCal()) ;
  }

  //if ( (dcu1->getTsGain() == 0) && (dcu2->getTsGain() < 0.0000000001) ) dcu2->setTsGain(0) ;
  if (dcu1->getTsGain() != dcu2->getTsGain()) {
    std::cout << "\tConversion factors TsGain is different " << dcu1->getTsGain() << " / " << dcu2->getTsGain() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setTsGain(dcu1->getTsGain()) ;
  }
  //if ( (dcu1->getTsOffset() == 0) && (dcu2->getTsOffset() < 0.0000000001) ) dcu2->setTsOffset(0) ;
  if (dcu1->getTsOffset() != dcu2->getTsOffset()) {
    std::cout << "\tConversion factors TsOffset is different " << dcu1->getTsOffset() << " / " << dcu2->getTsOffset() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setTsOffset(dcu1->getTsOffset()) ;
  }

  if (dcu1->getTsCal() != dcu2->getTsCal()) {
    std::cout << "\tConversion factors TsCal is different " << dcu1->getTsCal() << " / " << dcu2->getTsCal() << std::endl ;
    
    // set from dcu1 to dcu2
    dcu2->setTsCal(dcu1->getTsCal()) ;
  }

  //if ( (dcu1->getR68() == 0) && (dcu2->getR68() < 0.0000000001) ) dcu2->setR68(0) ;
  if (dcu1->getR68() != dcu2->getR68()) {
    std::cout << "\tConversion factors R68 is different " << dcu1->getR68() << " / " << dcu2->getR68() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setR68(dcu1->getR68()) ;
  }

  if (dcu1->getR68Cal() != dcu2->getR68Cal()) {
    std::cout << "\tConversion factors R68Cal is different " << dcu1->getR68Cal() << " / " << dcu2->getR68Cal() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setR68Cal(dcu1->getR68Cal()) ;
  }

  //if ( (dcu1->getAdcGain2() == 0) && (dcu2->getAdcGain2() < 0.0000000001) ) dcu2->setAdcGain2(0) ;
  if (dcu1->getAdcGain2() != dcu2->getAdcGain2()) {
    std::cout << "\tConversion factors AdcGain2 is different " << dcu1->getAdcGain2() << " / " << dcu2->getAdcGain2() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setAdcGain2(dcu1->getAdcGain2()) ;
  }

  //if ( (dcu1->getAdcOffset2() == 0) && (dcu2->getAdcOffset2() < 0.0000000001) ) dcu2->setAdcOffset2(0) ;
  if (dcu1->getAdcOffset2() != dcu2->getAdcOffset2()) {
    std::cout << "\tConversion factors AdcOffset2 is different " << dcu1->getAdcOffset2() << " / " << dcu2->getAdcOffset2() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setAdcOffset2(dcu1->getAdcOffset2()) ;
  }

  if (dcu1->getAdcCal2() != dcu2->getAdcCal2()) {
    std::cout << "\tConversion factors AdcCal2 is different " << dcu1->getAdcCal2() << " / " << dcu2->getAdcCal2() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setAdcCal2(dcu1->getAdcCal2()) ;
  }

  //if ( (dcu1->getAdcGain3() == 0) && (dcu2->getAdcGain3() < 0.0000000001) ) dcu2->setAdcGain3(0) ;
  if (dcu1->getAdcGain3() != dcu2->getAdcGain3()) {
    std::cout << "\tConversion factors AdcGain3 is different " << dcu1->getAdcGain3() << " / " << dcu2->getAdcGain3() << std::endl ;
    
    // set from dcu1 to dcu2
    dcu2->setAdcGain3(dcu1->getAdcGain3()) ;
  }

  //if ( (dcu1->getAdcOffset3() == 0) && (dcu2->getAdcOffset3() < 0.0000000001) ) dcu2->setAdcOffset3(0) ;
  if (dcu1->getAdcOffset3() != dcu2->getAdcOffset3()) {
    std::cout << "\tConversion factors AdcOffset3 is different " << dcu1->getAdcOffset3() << " / " << dcu2->getAdcOffset3() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setAdcOffset3(dcu1->getAdcOffset3()) ;
  }
  if (dcu1->getAdcCal3() != dcu2->getAdcCal3()) {
    std::cout << "\tConversion factors AdcCal3 is different " << dcu1->getAdcCal3() << " / " << dcu2->getAdcCal3() << std::endl ;

    // set from dcu1 to dcu2
    dcu2->setAdcCal3(dcu1->getAdcCal3()) ;
  }
}	  


/** Compare two set of conversions
 * -compareDcuConversion
 */
void compareDcuConversionFactors ( std::string xmlFile1, std::string xmlFile2, DeviceFactory &deviceFactory ) {

  try {
    bool detIdFlag = false ;

    TkDcuConversionFactory tkDcuConversionFactory1 ;
    std::cout << "Parsing file " << xmlFile1 << " for conversion factors" << std::endl ;
    tkDcuConversionFactory1.setInputFileName(xmlFile1) ;
    TkDcuConversionFactory tkDcuConversionFactory2 ;
    std::cout << "Parsing file " << xmlFile2 << " for conversion factors" << std::endl ;
    tkDcuConversionFactory2.setInputFileName(xmlFile2) ;

    if (deviceFactory.getDbUsed()) {
      std::cout << "Retreive the DCU / det id map" << std::endl ;
      deviceFactory.addAllDetId() ;
      detIdFlag = true ;
    }


    // Sort the conversion for factors in file 1
    XMLTkDcuConversion xmlTkDcuConversion1 (xmlFile1) ;
    dcuConversionVector vDcuConversionVector1 = xmlTkDcuConversion1.getDcuConversions() ;

    // Check the duplicated DCU in file 1 and sort it out
    dcuConversionVector unique, duplicated ;
    Sgi::hash_map<unsigned long, bool> duplicatedChecked ;
    for (dcuConversionVector::iterator it = vDcuConversionVector1.begin() ; it != vDcuConversionVector1.end() ; it ++) {

//       unsigned int detId = 0 ;
//       try {
// 	detId = (deviceFactory.getTkDcuInfo((*it)->getDcuHardId()))->getDetId() ;
// 	if (detId != 0) {
// 	  (*it)->setDetId(detId) ;
// 	  (*it)->setDcuType("FEH") ;
// 	  int det = (detId >> 25)&7 ;
// 	  std::string detector ;
// 	  switch (det) {
// 	  case 3: // TIB
// 	    detector = "TIB" ;
// 	    break ;
// 	  case 4: // TID
// 	    detector = "TID" ;
// 	    break ;
// 	  case 5: // TOB
// 	    detector = "TOB" ;
// 	    break ;
// 	  case 6: // TEC
// 	    detector = "TEC" ;
// 	    // int pos = (detId << 18)&3 ;
// 	    // if (pos == 1) detector = "TEC-" ;
// 	    // if (pos == 2) detector = "TEC+" ;
// 	    break ;
// 	  default:
// 	    detector = "none" ;
// 	  }
// 	  if (detector != "none") (*it)->setSubDetector(detector) ;
// 	  else std::cout << (*it)->getDcuHardId() << " not able to retreive the subdetector for det id " << detId << std::endl ;
// 	}
//       }
//       catch (FecExceptionHandler &e) {
// 	if ((*it)->getDcuType() == "FEH") {
// 	  std::cout << (*it)->getDcuHardId() << " is marked as FEH but get error on DETID retrieval" << std::endl ;
// 	  (*it)->setDetId(12345678) ;
// 	}
//       }

      if (duplicatedChecked.find((*it)->getDcuHardId()) == duplicatedChecked.end()) {
	  
	bool duplicatedFlag = false ;
	for (dcuConversionVector::iterator it1 = it ; it1 != vDcuConversionVector1.end() ; it1 ++) {

	  if (((*it1) != (*it)) && ((*it)->getDcuHardId() == (*it1)->getDcuHardId())) {
	    if ( ((*it1)->getSubDetector() == (*it)->getSubDetector()) &&
		 ((*it1)->getDcuType() == (*it)->getDcuType()) &&
		 ((*it1)->getStructureId() == (*it)->getStructureId()) &&
		 ((*it1)->getContainerId() == (*it)->getContainerId()) &&
		 ((*it1)->getDetId() == (*it)->getDetId()) &&
		 ((*it1)->getAdcGain0() == (*it)->getAdcGain0()) &&
		 ((*it1)->getAdcOffset0() == (*it)->getAdcOffset0()) &&
		 ((*it1)->getAdcInl0() == (*it)->getAdcInl0()) &&
		 ((*it1)->getAdcInl0OW() == (*it)->getAdcInl0OW()) &&
		 ((*it1)->getAdcCal0() == (*it)->getAdcCal0()) &&
		 ((*it1)->getI20() == (*it)->getI20()) &&
		 ((*it1)->getI10() == (*it)->getI10()) &&
		 ((*it1)->getICal() == (*it)->getICal()) &&
		 ((*it1)->getKDiv() == (*it)->getKDiv()) &&
		 ((*it1)->getKDivCal() == (*it)->getKDivCal()) &&
		 ((*it1)->getTsGain() == (*it)->getTsGain()) &&
		 ((*it1)->getTsOffset() == (*it)->getTsOffset()) &&
		 ((*it1)->getTsCal() == (*it)->getTsCal()) &&
		 ((*it1)->getR68() == (*it)->getR68()) &&
		 ((*it1)->getR68Cal() == (*it)->getR68Cal()) &&
		 ((*it1)->getAdcGain2() == (*it)->getAdcGain2()) &&
		 ((*it1)->getAdcOffset2() == (*it)->getAdcOffset2()) &&
		 ((*it1)->getAdcCal2() == (*it)->getAdcCal2()) &&
		 ((*it1)->getAdcGain3() == (*it)->getAdcGain3()) &&
		 ((*it1)->getAdcOffset3() == (*it)->getAdcOffset3()) &&
		 ((*it1)->getAdcCal3() == (*it)->getAdcCal3()) ) {
	      std::cout << (*it)->getDcuHardId() << " present several times but same values" << std::endl ;		
	    }
	    else {
	      if ( ((*it1)->getDcuType() != (*it)->getDcuType()) ||
		   ((*it1)->getAdcGain0() != (*it)->getAdcGain0()) ||
		   ((*it1)->getAdcOffset0() != (*it)->getAdcOffset0()) ||
		   ((*it1)->getAdcInl0() != (*it)->getAdcInl0()) ||
		   ((*it1)->getAdcInl0OW() != (*it)->getAdcInl0OW()) ||
		   ((*it1)->getAdcCal0() != (*it)->getAdcCal0()) ||
		   ((*it1)->getI20() != (*it)->getI20()) ||
		   ((*it1)->getI10() != (*it)->getI10()) ||
		   ((*it1)->getICal() != (*it)->getICal()) ||
		   ((*it1)->getKDiv() != (*it)->getKDiv()) ||
		   ((*it1)->getKDivCal() != (*it)->getKDivCal()) ||
		   ((*it1)->getTsGain() != (*it)->getTsGain()) ||
		   ((*it1)->getTsOffset() != (*it)->getTsOffset()) ||
		   ((*it1)->getTsCal() != (*it)->getTsCal()) ||
		   ((*it1)->getR68() != (*it)->getR68()) ||
		   ((*it1)->getR68Cal() != (*it)->getR68Cal()) ||
		   ((*it1)->getAdcGain2() != (*it)->getAdcGain2()) ||
		   ((*it1)->getAdcOffset2() != (*it)->getAdcOffset2()) ||
		   ((*it1)->getAdcCal2() != (*it)->getAdcCal2()) ||
		   ((*it1)->getAdcGain3() != (*it)->getAdcGain3()) ||
		   ((*it1)->getAdcOffset3() != (*it)->getAdcOffset3()) ||
		   ((*it1)->getAdcCal3() != (*it)->getAdcCal3()) ) {

		std::cout << (*it)->getDcuHardId() << " duplicated" << std::endl ;
		duplicatedFlag = true ;
		duplicated.push_back(*it1) ;
	      }
	      else {
		if ((*it1)->getSubDetector() != (*it)->getSubDetector()) {
		  std::cout << (*it)->getDcuHardId() << " duplicated for subdetector but it will be set by the det id afterwards" << std::endl ;
		}
		if ((*it1)->getStructureId() != (*it)->getStructureId()) {
		  std::cout << (*it)->getDcuHardId() << " duplicated for the structure ID, remove one of the two" << std::endl ;
		}
		if ((*it1)->getContainerId() != (*it)->getContainerId()) {
		  std::cout << (*it)->getDcuHardId() << " duplicated for the container ID, remove one of the two" << std::endl; 
		}
		if ((*it1)->getDetId() != (*it)->getDetId()) {
		  std::cout << (*it)->getDcuHardId() << " duplicated for the DET ID but will be fixed" << std::endl; 
		}
	      }
	    }
	  }
	}

	if (!duplicatedFlag) {
	  //	    std::cout << "Adding " << (*it)->getDcuHardId() << " as unique DCU" << std::endl ;
	  unique.push_back(*it) ;
	  duplicatedChecked[(*it)->getDcuHardId()] = true ;
	}
	else {
	  duplicated.push_back(*it) ;
	  duplicatedChecked[(*it)->getDcuHardId()] = true ;
	}
      }
    }

    tkDcuConversionFactory2.setOutputFileName ("/tmp/unique.xml") ;
    tkDcuConversionFactory2.setTkDcuConversionFactors(unique) ;
    tkDcuConversionFactory2.setOutputFileName ("/tmp/duplicated.xml") ;
    tkDcuConversionFactory2.setTkDcuConversionFactors(duplicated) ;

    //       // try to sort the one which are duplicated
    //       for (dcuConversionVector::iterator it = duplicated.begin() ; it != duplicated.end() ; it ++) {
    // 	for (dcuConversionVector::iterator it1 = it ; it1 != duplicated.end() ; it1 ++) {
    // 	  if (((*it1) != (*it)) && ((*it)->getDcuHardId() == (*it1)->getDcuHardId())) {
    // 	    if ( ((*it1)->getI10() > 1) && ((*it)->getI10() < 1) ) {
    // 	      unique.push_back(*it) ;
    // 	    }
    // 	    else if ( ((*it1)->getI10() < 1) && ((*it)->getI10() > 1) ) {
    // 	      unique.push_back(*it1) ;
    // 	    }
    // 	  }
    // 	}
    //       }

    // Make a map of good DCU conversion factors
    Sgi::hash_map<unsigned long, TkDcuConversionFactors *> dcuConversion1 ;
    for (dcuConversionVector::iterator it = unique.begin() ; it != unique.end() ; it ++) {
      dcuConversion1[(*it)->getDcuHardId()] = *it ;
    }

    // Find default conversions for DCU on FEH
    Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator it1 = dcuConversion1.find(0) ;
    TkDcuConversionFactors *dcuDefaultFEH = NULL ;
    if (it1 != dcuConversion1.end()) dcuDefaultFEH = it1->second;
    // Find default conversions for DCU on CCU
    Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator it2 = dcuConversion1.find(1) ;
    TkDcuConversionFactors *dcuDefaultCCU = NULL ;
    if (it2 != dcuConversion1.end()) dcuDefaultCCU = it2->second;

    // Loop on the conversion factors from file 2 and replace the parameters with good ones
    dcuConversionVector goodValues ;
    goodValues.push_back(dcuDefaultFEH->clone()) ;
    goodValues.push_back(dcuDefaultCCU->clone()) ;
    Sgi::hash_map<unsigned long, TkDcuConversionFactors *> dcuConversion2 = tkDcuConversionFactory2.getConversionFactors ( ) ;
    for (Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator it = dcuConversion2.begin() ; it != dcuConversion2.end() ; it ++) {

      if ( (it->second->getDcuHardId() != 0) && (it->second->getDcuHardId() != 1) ) {

	bool notAdded = 1 ;

	// -------------------------------------------------------------------------
	// Found in unique DCUs
	if (dcuConversion1.find(it->second->getDcuHardId()) != dcuConversion1.end()) {

	  if (it->second->getDcuType() == dcuConversion1[it->second->getDcuHardId()]->getDcuType()) { // DCU type coherent
	    goodValues.push_back(dcuConversion1[it->second->getDcuHardId()]->clone()) ;
	    notAdded = 0 ;
	  }
	  else {
	    std::cerr << it->second->getDcuHardId() << " has an incoherent DCU type (" 
		      << dcuConversion1[it->second->getDcuHardId()]->getDcuType() << ") in file " << xmlFile1 
		      << " it should be " << it->second->getDcuType() << std::endl ;
	    notAdded = 2 ;
	  }
	}

	// -------------------------------------------------------------------------
	// Take default parameters or find it in the duplicated DCUs or wrong DCU type from original file
	if (notAdded) {

	  // -------------------------------------------------------------------------
	  // Try to Find the det id and subdetector
	  unsigned int detId = 0 ;
	  try {
	    detId = (deviceFactory.getTkDcuInfo(it->second->getDcuHardId()))->getDetId() ;
	  }
	  catch (FecExceptionHandler &e) {
	    if (it->second->getDcuType() == "FEH") {
	      std::cerr << it->second->getDcuHardId() << " is marked as FEH but get error on DETID retrieval" << std::endl ;
	      it->second->setDetId(12345678) ;
	    }
	  }
	  int det = (detId >> 25)&7 ;
	  std::string detector ;
	  switch (det) {
	  case 3: // TIB
	    detector = "TIB" ;
	    break ;
	  case 4: // TID
	    detector = "TID" ;
	    break ;
	  case 5: // TOB
	    detector = "TOB" ;
	    break ;
	  case 6: // TEC
	    detector = "TEC" ;
	    // int pos = (detId << 18)&3 ;
	    // if (pos == 1) detector = "TEC-" ;
	    // if (pos == 2) detector = "TEC+" ;
	    break ;
	  default:
	    detector = "none" ;
	  }

	  // -------------------------------------------------------------------------
	  // Check in duplicated DCUs

	  // Use the default conversions with with 
	  TkDcuConversionFactors *newValue = NULL ;

	  // in list of duplicated DCUs
	  for (dcuConversionVector::iterator it1 = duplicated.begin() ; it1 != duplicated.end() ; it1 ++) {
	    if (it->second->getDcuHardId() == (*it1)->getDcuHardId()) {
	      // found one 
	      if (newValue == NULL) {
		if ( (detId != 0) && ((*it1)->getDcuType() == "CCU") ) { // value duplicated is incoherent => forget it => use default values
		  std::cerr << "Found incoherent entry in duplicated DCU conversion factors for " << it->second->getDcuHardId() << " on " << it->second->getDcuType() << " marked as CCU and detId != 0" << std::endl ;
		}
		else {
		  if ( (((*it1)->getDcuType() == "CCU") && (it->second->getDcuType() != "CCU")) ||
		       (((*it1)->getDcuType() == "FEH") && (it->second->getDcuType() != "FEH")) ) { // value duplicated is incoherent => forget it => use default values
		    std::cerr << "Found incoherent entry in duplicated DCU conversion factors for " << it->second->getDcuHardId() << " on " << it->second->getDcuType() << " marked as " << (*it1)->getDcuType() << " but set in TKCC DB as " << it->second->getDcuType() << std::endl ;
		  }
		  else {
		    std::cout << "Found a first value in duplicated DCU for DCU " << it->second->getDcuHardId() << " on " << it->second->getDcuType() << std::endl ;
		    newValue = (*it1)->clone() ;
		  }
		}
	      }
	      else { // Already found one so duplicated
		if ( (detId != 0) && ((*it1)->getDcuType() == "CCU") ) { // value duplicated is incoherent => forget it
		  std::cerr << "Found incoherent entry in duplicated DCU conversion factors for " << it->second->getDcuHardId() << " on " << it->second->getDcuType() << " marked as CCU and detId != 0" << std::endl ;
		}
		else {
		  if ( (((*it1)->getDcuType() == "CCU") && (it->second->getDcuType() != "CCU")) ||
		       (((*it1)->getDcuType() == "FEH") && (it->second->getDcuType() != "FEH")) ) { // value duplicated is incoherent => forget it => use default values
		    std::cerr << "Found incoherent entry in duplicated DCU conversion factors for " << it->second->getDcuHardId() << " on " << it->second->getDcuType() << " marked as " << (*it1)->getDcuType() << " but set in TKCC DB as " << it->second->getDcuType() << std::endl ;
		  }
		  else {
		    if (it->second->getDcuType() == "FEH") {
		      if ( (newValue->getI10() > 1) && ((*it1)->getI10() < 1) ) { // the value here seems to be more coherent
			std::cout << "Found another value in duplicated DCU for DCU " << it->second->getDcuHardId() << " on " << it->second->getDcuType() << " and I10 seems to be better => take this new value" << std::endl ;
			delete newValue ;
			newValue = (*it1)->clone() ;
		      }
		      else {
			std::cout << "Found another value in duplicated DCU for DCU " << it->second->getDcuHardId() << " on " << it->second->getDcuType() << " and I10 seems to be worse => keep the previous value" << std::endl ;
		      }
		    }
		  }
		}
	      }
	    }
	  }
	    
	  // -------------------------------------------------------------------------
	  // Not found in duplicated list create it on default values
	  if (newValue == NULL) {
	    std::cout << "Take default parameters for DCU " << it->second->getDcuHardId() << " for DCU on " << it->second->getDcuType() << std::endl ;
	    if (it->second->getDcuType() == "FEH") { // no det id ... another DCU on FEH ?
	      if (dcuDefaultFEH != NULL) {
		newValue = dcuDefaultFEH->clone() ;
		newValue->setDcuHardId(it->second->getDcuHardId()) ;
		newValue->setDetId(12345678) ;
		newValue->setDcuType("FEH") ;
		newValue->setSubDetector("UNK") ;
		newValue->setContainerId("DEFAULTDATA") ;
		newValue->setStructureId("DEFAULTDATA") ;
//		if (dcuConversion1.find(it->second->getDcuHardId()) != dcuConversion1.end()) { // DCU was existing in file but wrong dcu type
//		  newValue->setContainerId(dcuConversion1[it->second->getDcuHardId()]->getContainerId()) ;
//		  newValue->setStructureId(dcuConversion1[it->second->getDcuHardId()]->getStructureId()) ;
//		}
	      }
	      else std::cout << "No default FEH conversion factors for DCU " << it->second->getDcuHardId() << std::endl ;
	    }
	    else {
	      if (dcuDefaultCCU != NULL) {
		// DCU on CCU but subdetector not known
		newValue = dcuDefaultCCU->clone() ;
		newValue->setDcuHardId(it->second->getDcuHardId()) ;
		newValue->setDetId(0) ;
		newValue->setDcuType("CCU") ;
		newValue->setSubDetector("UNK") ;
		newValue->setContainerId("DEFAULTDATA") ;
		newValue->setStructureId("DEFAULTDATA") ;
	      }
	      else std::cout << "No default DCU on CCU conversion factors for DCU " << it->second->getDcuHardId() << std::endl ;
	    }
	  }

	  // -------------------------------------------------------------------------
	  // Set the correct values known
	  if (detId != 0) {
	    newValue->setDetId(detId) ;
	    newValue->setSubDetector(detector) ;
	    newValue->setDcuType("FEH") ;
	  }

	  // -------------------------------------------------------------------------
	  // add it to the list
	  if (newValue != NULL) goodValues.push_back(newValue) ;
	}
      }
    }
      
    tkDcuConversionFactory2.setOutputFileName ("/tmp/newfactors.xml") ;
    tkDcuConversionFactory2.setTkDcuConversionFactors(goodValues) ;

    TkDcuConversionFactory::deleteVectorI(goodValues) ;

    std::cout << "-------------------------------------------------------" << std::endl ;
    std::cout << "upload unique DCU conversion factors from file " << xmlFile1 << " in file " << "/tmp/unique.xml" << std::endl ;
    std::cout << "upload duplicated DCU conversion factors from file " << xmlFile1 << " in file " << "/tmp/duplicated.xml" << std::endl ;
    std::cout << "upload merge DCU conversion factors from file " << xmlFile1 << " and file " << xmlFile2 << " in file " << "/tmp/newfactors.xml" << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
  }

}


/** Merge different input (one is database) and put it in a the output
 * -uploadDcuConversion
 */
void uploadConversionFactors (TkDcuConversionFactory &deviceFactory, std::string outputFile, std::list<std::string> listFileName, std::string partitionName ) {

  int duplicated = 0 ;

  for (std::list<std::string>::iterator itFileName = listFileName.begin() ; itFileName != listFileName.end() ; itFileName ++ ) {

    std::string fileName = *itFileName ;
    if (fileName != "database") {
      std::cout << "Parsing the file " << fileName << " for DCU conversion factors" << std::endl ;
    }
    else {
      std::cout << "Parsing the database for DCU conversion factors for partition " << partitionName << std::endl ;
    }

    try {

      if (deviceFactory.getConversionFactors().size() == 0) {
	if (fileName != "database") { // file
	  deviceFactory.setInputFileName(fileName) ;
	}
	else { // database

	  // try to check if the conversion 0 is existing, default parameter for FEH
	  TkDcuConversionFactors *val = NULL ;
	  try {
	    val = deviceFactory.getTkDcuConversionFactors(0) ;
	    if (val != NULL) std::cout << "FEH default DCU conversion factors existing in DB" << std::endl ;
	    else std::cerr << "Error: FEH default DCU conversion factors is NOT existing in DB" << std::endl ;
	  }
	  catch (FecExceptionHandler e) {
	    std::cerr << "Error: FEH default DCU conversion factors is NOT existing in DB: " << e.what() << std::endl ;
	  }
	  // try to check if the conversion 1 is existing, default parameter for DCU on CCU
	  try {
	    val = deviceFactory.getTkDcuConversionFactors(1) ;
	    if (val != NULL) std::cout << "DCU on CCU default DCU conversion factors existing in DB" << std::endl ;
	    else std::cerr << "Error: DCU on CCU default DCU conversion factors is NOT existing in DB" << std::endl ;
	  }
	  catch (FecExceptionHandler e) {
	    std::cerr << "Error: DCU on CCU default DCU conversion factors is NOT existing in DB: " << e.what() << std::endl ;
	  }

	  // Download of everything
	  deviceFactory.addConversionPartition(partitionName) ;
	}
	if (deviceFactory.getConversionFactors().size() != 0) {
	  std::cout << "Found " << std::dec << deviceFactory.getConversionFactors().size() << " DCU conversions factors" << std::endl ;
	}
	else std::cerr << "No DCU conversion factors found" << std::endl ;
      }
      else {
	TkDcuConversionFactory tkDcuConversionFactory ;
	tkDcuConversionFactory.setInputFileName(fileName) ;
	  
	// Cross check
	if (tkDcuConversionFactory.getConversionFactors().size() > 0) {
	  std::cout << "Found " << std::dec << tkDcuConversionFactory.getConversionFactors().size() << " DCU conversions factors" << std::endl ;
	  std::cout << "Start the cross check with the data already set" << std::endl ;
	  Sgi::hash_map<unsigned long, TkDcuConversionFactors *> conversionFactors = tkDcuConversionFactory.getConversionFactors() ;
	  Sgi::hash_map<unsigned long, TkDcuConversionFactors *> conversionFactorsNotDuplicated ;

	  for (Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator itDcu = conversionFactors.begin() ; itDcu != conversionFactors.end() ; itDcu ++) {

	    if (itDcu->second->getDetId() == 0) std::cout << "DET ID is zero: " << itDcu->second->getDetId() << std::endl ;

	    unsigned int dcuHardId = itDcu->second->getDcuHardId() ;
	    try {
	      deviceFactory.getTkDcuConversionFactors(dcuHardId) ;

	      std::cerr << "Found a duplicated DCU " << dcuHardId << ", removing from the list: " << std::endl ;
	      std::cerr << "DCU Hard Id  | " << deviceFactory.getTkDcuConversionFactors(dcuHardId)->getDcuHardId() << " | " << itDcu->second->getDcuHardId() << std::endl ;
	      std::cerr << "Sub detector | " << deviceFactory.getTkDcuConversionFactors(dcuHardId)->getSubDetector() << " | " << itDcu->second->getSubDetector() << std::endl ;
	      std::cerr << "DCU Type     | " << deviceFactory.getTkDcuConversionFactors(dcuHardId)->getDcuType() << " | " << itDcu->second->getDcuType() << std::endl ;
	      std::cerr << "Structure ID | " << deviceFactory.getTkDcuConversionFactors(dcuHardId)->getStructureId() << " | " << itDcu->second->getStructureId() << std::endl ;
	      std::cerr << "Container ID | " << deviceFactory.getTkDcuConversionFactors(dcuHardId)->getContainerId() << " | " << itDcu->second->getContainerId() << std::endl ;
	      //std::cerr << "DET ID       | " << deviceFactory.getTkDcuConversionFactors(dcuHardId)->getDetId() << " | " << itDcu->second->getDetId() << std::endl ;

	      duplicated ++ ;
	    }
	    catch (FecExceptionHandler &e) {
 	      conversionFactorsNotDuplicated[dcuHardId] = itDcu->second ;
	    }
	  }

 	  // Adding the conversion factors not duplicated in the list
	  std::cout << "Adding " << conversionFactorsNotDuplicated.size() << " DCUs not duplicated" << std::endl ;
 	  for (Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator itDcu = conversionFactorsNotDuplicated.begin() ; itDcu != conversionFactorsNotDuplicated.end() ; itDcu ++) {
	    if (itDcu->second != NULL) {
	      deviceFactory.setTkDcuConversionFactors ( *(itDcu->second) ) ;
	      //std::cout << "Adding " << itDcu->second->getDcuHardId() << "/" << conversionFactorsOrigin[itDcu->second->getDcuHardId()]->getDcuHardId() << std::endl ;
	    }
 	  }
	}
	else std::cerr << "No DCU conversion factors found" << std::endl ;
      }
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Error during the parsing of the file " << fileName << ": " << std::endl << e.what() << std::endl ;
    }
  }

  if (duplicated) 
    std::cout << "Found " << std::dec << duplicated << " duplicated DCUs" << std::endl ;
  
  std::cout << "Found " << deviceFactory.getConversionFactors().size() << " DCU conversions" << std::endl ;

  // Starting
  if (outputFile != "") {
    if ((outputFile == "database") && !deviceFactory.getDbConnected()) 
      std::cout << "Cannot upload the data to the database, the database is not connected" << std::endl ;
    else {

      if (outputFile == "database") deviceFactory.setUsingDb() ;
      else {
	deviceFactory.setUsingFile() ;
	deviceFactory.setOutputFileName(outputFile) ;
      }
      
      // Check the subdetector and parameters
      Sgi::hash_map<unsigned long, TkDcuConversionFactors *> conversionFactorsFinal = deviceFactory.getConversionFactors() ;
      dcuConversionVector vDcuConversion ;

      for (Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator itDcu = conversionFactorsFinal.begin() ; itDcu != conversionFactorsFinal.end() ; itDcu ++) {
	
	TkDcuConversionFactors *values = itDcu->second ;
	if (values != NULL) {

	  if ( (values->getSubDetector() == "TIB") ||
	       (values->getSubDetector() == "TOB") ||
	       (values->getSubDetector() == "TEC") ||
	       (values->getSubDetector() == "TID") ) {

	  }
	  else if (values->getDcuHardId() == 0) { // Default parameters
	    values->setSubDetector("DEF") ;
	  }
	  else {
	    std::cerr << "Invalid subdetector " << values->getSubDetector() << " for DCU " << values->getDcuHardId() ;
	    if (deviceFactory.getDbUsed()) {
	      std::cerr << ": DCU added with subdetector as UNK" << std::endl ;
	      values->setSubDetector("UNK") ;
	    }
	    else std::cerr << ": DCU added with this error" << std::endl ;
	  }

	  vDcuConversion.push_back(values) ;	  
	}
      }
	
      try {
	deviceFactory.setTkDcuConversionFactors(vDcuConversion) ;
	std::cout << "Upload terminated" << std::endl ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "Error during the upload of the DCU conversion factors" << std::endl ;
	std::cerr << e.what() << std::endl ;
	//values->display() ;
	//getchar() ;
      }
      vDcuConversion.clear() ;
    }
  }
}
/** Analyse different files of raw data with the conversion factors specified
 * -analysis
 */
void dcuAnalysis (TkDcuConversionFactory &deviceFactory, std::string inputFile, std::string dcuType, std::list<std::string> listFileName, std::string partitionName ) {

  deviceVector dcuVector ;

  if (inputFile == "database") {
    if (!deviceFactory.getDbConnected()) {
      std::cerr << "You cannot use the database: the database is not connected" << std::endl ;
      return ;
    }
    else {
      deviceFactory.addConversionPartition(partitionName) ;
    }
  }
  else {
    try {
      std::cout << "Read the conversion factors from " << inputFile << std::endl ;
      deviceFactory.setInputFileName(inputFile) ;
      std::cout << "Conversion factors read" << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to read conversion factors from " << inputFile << std::endl ;
      std::cerr << e.what() << std::endl ;
    }
  }

  for (std::list<std::string>::iterator itFileName = listFileName.begin() ; itFileName != listFileName.end() ; itFileName ++ ) {

    std::string dataFile = *itFileName ;

    FecDeviceFactory fecDeviceFactory ;
    try {
      std::cout << "Read the file " << dataFile << std::endl ;
      fecDeviceFactory.setInputFileName (dataFile) ;
      fecDeviceFactory.getDcuValuesDescriptions (dcuVector,0,0xFFFFFFFF) ;
    }
    catch (FecExceptionHandler &e) {

      std::cerr << "Unable to read the file " << dataFile << std::endl ;
      std::cerr << e.what() << std::endl ;
    }
  }

  std::cout << "Found " << std::dec << dcuVector.size() << " DCUs in file(s)" << std::endl ;

  // Retreive the default conversion factors
  TkDcuConversionFactors *conversionDefaultFactorsFEH = NULL ;
  try {
    conversionDefaultFactorsFEH = deviceFactory.getTkDcuConversionFactors(0) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "NO DEFAULT CONVERSION FACTORS FOR DCU on FEH" << std::endl ;
  }
  TkDcuConversionFactors *conversionDefaultFactorsCCU = NULL ;
  try {
    conversionDefaultFactorsCCU = deviceFactory.getTkDcuConversionFactors(1) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "NO DEFAULT CONVERSION FACTORS FOR DCU on CCU" << std::endl ;
  }

  for (deviceVector::iterator itDcu = dcuVector.begin() ; itDcu != dcuVector.end() ; itDcu ++) {
    dcuDescription *dcuD = dynamic_cast<dcuDescription *>(*itDcu) ;

    try {
      TkDcuConversionFactors *conversionFactors = deviceFactory.getTkDcuConversionFactors(dcuD->getDcuHardId()) ;
      conversionFactors->setDcuDescription (dcuD) ;
      bool display = true ;
      if ( (dcuType != "") && ((dcuType == DCUCCU) || (dcuType == DCUFEH)) &&
	   (conversionFactors->getDcuType() != dcuType) ) 
	display = false ;

      if ( (dcuType != "") && ((dcuType == "TOB") || (dcuType == "TEC") || (dcuType == "TIB") || (dcuType == "TID")) &&
	   (conversionFactors->getSubDetector() != dcuType) ) 
	display = false ;

      if (display) {
	//std::cout << "Conversion factors found" << std::endl ;
#define GUIDO
#ifndef GUIDO
	conversionFactors->display(true) ;
#else
	bool quality ;
	std::cout << getRingKey(dcuD->getKey()) << "\t" << getCcuKey(dcuD->getKey()) << "\t" << getChannelKey(dcuD->getKey()) ;
	std::cout << "\t" << conversionFactors->getDcuType() << "\t" << conversionFactors->getSubDetector() ;
	if ( (conversionFactors->getDcuType() == DCUFEH) || (conversionFactors->getSubDetector() == "TOB") ) 
	  std::cout << "\t" << conversionFactors->getSiliconSensorTemperature(quality) ;
	else 
	  std::cout << "\t" << "NA" ;

	if (conversionFactors->getDcuType() == DCUFEH)
	  std::cout << "\t" << conversionFactors->getHybridTemperature(quality) ;
	else 
	  std::cout << "\t" << "NA" ;

	std::cout << "\t" << conversionFactors->getDcuTemperature(quality) ;

	if ( (conversionFactors->getDcuType() == DCUFEH) || (conversionFactors->getSubDetector() == "TOB") ) 
	  std::cout << "\t" << conversionFactors->getV250(quality) ;
	else 
	  std::cout << "\t" << "NA" ;

	if ( (conversionFactors->getDcuType() == DCUFEH) || (conversionFactors->getSubDetector() == "TOB") ) 
	  std::cout << "\t" << conversionFactors->getV125(quality)  ;
	else 
	  std::cout << "\t" << "NA" ;

	std::cout << std::endl ;
#endif
      }
    }
    catch (FecExceptionHandler &e) {
      if ( (conversionDefaultFactorsFEH != NULL) && (dcuD->getDcuType() == DCUFEH) ) {
	conversionDefaultFactorsFEH->setDcuDescription (dcuD) ;
	conversionDefaultFactorsFEH->display(true) ;
      }
      else if ( (conversionDefaultFactorsCCU != NULL) && (dcuD->getDcuType() == DCUCCU) ) {
	conversionDefaultFactorsCCU->setDcuDescription (dcuD) ;
	conversionDefaultFactorsCCU->display(true) ;
      }
      else
	std::cout << "No Conversion factors for DCU " << dcuD->getDcuHardId() << std::endl ;      
    }
  }
}

/** Check all the DCU found in the database and check if the conversion factors is existing
 * -completion
 */
void dcuCompletion (TkDcuConversionFactory &deviceFactory, std::string partitionName, std::string outputFile ) {

  if (!deviceFactory.getDbConnected()) {
    std::cerr << "No database available: please set your database before using this method" << std::endl ;
    return ;
  }

  std::string login="nil", passwd="nil", path="nil" ;
  try {
    // Open a FEC database
    DbAccess::getDbConfiguration (login, passwd, path) ;
    FecFactory fecFactory(login,passwd,path) ;

    // Retreive the conversion factors
    deviceFactory.addConversionPartition(partitionName) ;

    // Retreive the default conversion factors for FEH
    TkDcuConversionFactors *conversionDefaultFactorsFEH = deviceFactory.getTkDcuConversionFactors(0) ;
    TkDcuConversionFactors *conversionDefaultFactorsCCU = deviceFactory.getTkDcuConversionFactors(1) ;

    // Retreive the DCU for the partition
    deviceVector vDevice ;
    deviceVector dcuVector ;
    try {
      fecFactory.getFecDeviceDescriptions (partitionName,vDevice) ;
      dcuVector = FecFactory::getDeviceFromDeviceVector(vDevice,DCU) ;
      
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to retreive the DCU for the partition " << partitionName << std::endl ;
      std::cerr << e.what() << std::endl ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Unable to retreive the DCU for the partition " << partitionName << std::endl ;
      std::cerr << e.what() << std::endl ;
    }

    if (dcuVector.size() == 0) {
      std::cerr << "Did not found any DCU in the database for partition " << partitionName << std::endl ;
      return ;
    }

    // For output if MTCC is set
    dcuConversionVector vDcuConversion ;
    unsigned int counterFEH = 0, counterCCU = 0, counterCCUWithout = 0, counterFEHWithout = 0, counterCCUWith = 0, counterFEHWith = 0, dcuFEHAdded = 0, dcuCCUAdded = 0 ;

    // For each DCU try to find the corresponding conversion factors
    for (deviceVector::iterator itDcu = dcuVector.begin() ; itDcu != dcuVector.end() ; itDcu ++) {
      dcuDescription *dcuD = dynamic_cast<dcuDescription *>(*itDcu) ;

      if (dcuD->getDcuType() == DCUCCU) counterCCU ++ ;
      else if (dcuD->getDcuType() == DCUFEH) counterFEH ++ ;
      else {
	std::cout << "-----> ERROR on auto detection: DCU " << dcuD->getDcuHardId() << ": has an invalid DCU type: " << dcuD->getDcuType() << std::endl ;
	getchar() ;
      }

      try {
	TkDcuConversionFactors *conversionFactors = deviceFactory.getTkDcuConversionFactors(dcuD->getDcuHardId()) ;
#define MTCC
#ifndef MTCC
	std::cout << "Conversion factors found for DCU " << dcuD->getDcuHardId() << std::endl ;
#endif
	conversionFactors->setDcuDescription (dcuD) ;
	if (dcuD->getDcuType() != conversionFactors->getDcuType()) {
	  std::cout << "--------------------------------------------------------------" << std::endl ;
	  conversionFactors->display(true) ;
	  std::cout << "Should be a DCU on " << dcuD->getDcuType() << " / " << conversionFactors->getDcuType() << std::endl;
	  getchar() ;
	}

	if (conversionFactors->getDcuType() == DCUCCU) counterCCUWith ++ ;
	else if (conversionFactors->getDcuType() == DCUFEH) counterFEHWith ++ ;
	else {
	  std::cout << "-----> ERROR on conversion factors: DCU " << conversionFactors->getDcuHardId() << ": has an invalid DCU type: " << conversionFactors->getDcuType() << std::endl ;
	  getchar() ;
	}

#ifdef MTCC
	// This part is for the MTCC tests
	bool wait = false ;
	if ( (getRingKey(dcuD->getKey()) == 0) || (getRingKey(dcuD->getKey()) == 1)) {

	  if (conversionFactors->getSubDetector() != "TIB") { //&& (conversionFactors->getSubDetector() != "TID") ) {
	    std::cout << "----> ERROR: DCU Conversion: This DCU should be TIB DCU" << std::endl ;
	    conversionFactors->setSubDetector("TIB") ;
	    wait = true ;
	  }
	  if (getChannelKey(dcuD->getKey()) == 0x10) {
	    if (dcuD->getDcuType() != DCUCCU) {
	      std::cout << "----> ERROR: DCU auto: This DCU should be TIB DCU on CCU" << std::endl ;
	      wait = true ;
	    }
	    if (conversionFactors->getDcuType() != DCUCCU) {
	      std::cout << "----> ERROR: DCU Conversion: This DCU should be TIB DCU on CCU" << std::endl ;
	      conversionFactors->setDcuType(DCUCCU) ;
	      wait = true ;
	    }
	  }
	  else {
	    if (dcuD->getDcuType() != DCUFEH) {
	      std::cout << "----> ERROR: DCU auto: This DCU should be TIB DCU on FEH" << std::endl ;
	      wait = true ;
	    }
	    if (conversionFactors->getDcuType() != DCUFEH) {
	      std::cout << "----> ERROR: DCU Conversion: This DCU should be TIB DCU on CCU" << std::endl ;
	      conversionFactors->setDcuType(DCUFEH) ;
	      wait = true ;
	    }
	  }
	}
	else if (getRingKey(dcuD->getKey()) == 2) {
	  if (conversionFactors->getSubDetector() != "TOB") {
	    std::cout << "----> ERROR: DCU Conversion: This DCU should be TOB DCU" << std::endl ;
	    conversionFactors->setSubDetector("TOB") ;
	    wait = true ;
	  }
	  if (getChannelKey(dcuD->getKey()) == 0x1E) {
	    if (dcuD->getDcuType() != DCUCCU) {
	      std::cout << "----> ERROR: DCU auto: This DCU should be TOB DCU on CCU" << std::endl ;
	      wait = true ;
	    }
	    if (conversionFactors->getDcuType() != DCUCCU) {
	      std::cout << "----> ERROR: DCU Conversion: This DCU should be TOB DCU on CCU" << std::endl ;
	      conversionFactors->setDcuType(DCUCCU) ;
	      wait = true ;
	    }
	  }
	  else {
	    if (dcuD->getDcuType() != DCUFEH) {
	      std::cout << "----> ERROR: DCU auto: This DCU should be TOB DCU on FEH" << std::endl ;
	      wait = true ;
	    }
	    if (conversionFactors->getDcuType() != DCUFEH) {
	      std::cout << "----> ERROR: DCU Conversion: This DCU should be TOB DCU on CCU" << std::endl ;
	      conversionFactors->setDcuType(DCUFEH) ;
	      wait = true ;
	    }
	  }
	}
	else if (getRingKey(dcuD->getKey()) == 3) {
	  if (conversionFactors->getSubDetector() != "TEC") {
	    std::cout << "----> ERROR: DCU Conversion: This DCU should be TEC DCU" << std::endl ;
	    conversionFactors->setSubDetector("TEC") ;
	    wait = true ;
	  }
	  if (getChannelKey(dcuD->getKey()) == 0x10) {
	    if (dcuD->getDcuType() != DCUCCU) {
	      std::cout << "----> ERROR: DCU auto: This DCU should be TEC DCU on CCU" << std::endl ;
	      wait = true ;
	    }
	    if (conversionFactors->getDcuType() != DCUCCU) {
	      std::cout << "----> ERROR: DCU Conversion: This DCU should be TEC DCU on CCU" << std::endl ;
	      conversionFactors->setDcuType(DCUCCU) ;
	      wait = true ;
	    }
	  }
	  else {
	    if (dcuD->getDcuType() != DCUFEH) {
	      std::cout << "----> ERROR: DCU auto: This DCU should be TEC DCU on FEH" << std::endl ;
	      wait = true ;
	    }
	    if (conversionFactors->getDcuType() != DCUFEH) {
	      std::cout << "----> ERROR: DCU Conversion: This DCU should be TEC DCU on CCU" << std::endl ;
	      conversionFactors->setDcuType(DCUFEH) ;
	      wait = true ;
	    }
	  }
	}

	if (wait) {
	  conversionFactors->display(true) ;
	  getchar() ;
	}
#endif

	TkDcuConversionFactors *conversionFactorsF = new TkDcuConversionFactors(*conversionFactors) ;
	vDcuConversion.push_back(conversionFactorsF) ;
      }
      catch (FecExceptionHandler &e) {

	if (dcuD->getDcuType() == DCUCCU) counterCCUWithout ++ ;
	else if (dcuD->getDcuType() == DCUFEH) counterFEHWithout ++ ;
	else {
	  std::cout << "-----> ERROR on conversion factors: DCU " << dcuD->getDcuHardId() << ": has an invalid DCU type: " << dcuD->getDcuType() << std::endl ;
	  getchar() ;
	}

#ifndef MTCC
	std::cout << "No Conversion factors for DCU " << dcuD->getDcuHardId() << std::endl ;      
#else
	if (dcuD->getDcuType() == DCUFEH) {

	  TkDcuConversionFactors *conversionFactors = new TkDcuConversionFactors(*conversionDefaultFactorsFEH) ;
	  std::string subDetector, dcuType = DCUFEH ;
	  switch (getRingKey(dcuD->getKey())) {
	  case 0:
	  case 1:
	    subDetector = "TIB" ;
	    if (getChannelKey(dcuD->getKey()) == 0x10) {
	      dcuType = DCUCCU ;
	      std::cerr << "----> ERROR: DCU " << dcuD->getDcuType() << ": should a DCU on FEH" << std::endl ;
	      getchar() ;
	    }
	    break;
	  case 2:
	    subDetector = "TOB" ;
	    if (getChannelKey(dcuD->getKey()) == 0x1e) {
	      dcuType = DCUCCU ;
	      std::cerr << "----> ERROR: DCU " << dcuD->getDcuType() << ": should a DCU on FEH" << std::endl ;
	      getchar() ;
	    }
	    break ;
	  case 3:
	    subDetector = "TEC" ;
	    if (getChannelKey(dcuD->getKey()) == 0x10) {
	      dcuType = DCUCCU ;
	      std::cerr << "----> ERROR: DCU " << dcuD->getDcuType() << ": should a DCU on FEH" << std::endl ;
	      getchar() ;
	    }
	    break ;
	  }
	  if (dcuType == DCUFEH) {
	    conversionFactors->setSubDetector(subDetector) ;
	    conversionFactors->setDcuHardId(dcuD->getDcuHardId()) ;
	    conversionFactors->setDcuType(dcuType) ;
	    vDcuConversion.push_back(conversionFactors) ;
	    dcuFEHAdded ++ ;
	  }
	  else delete conversionFactors ;
	}
	else {
	  
	  TkDcuConversionFactors *conversionFactors = new TkDcuConversionFactors(*conversionDefaultFactorsCCU) ;
	  std::string subDetector, dcuType = DCUCCU ;
	  switch (getRingKey(dcuD->getKey())) {
	  case 0:
	  case 1:
	    subDetector = "TIB" ;
	    if (getChannelKey(dcuD->getKey()) != 0x10) {
	      dcuType = DCUFEH ;
	      std::cerr << "----> ERROR: DCU " << dcuD->getDcuType() << ": should a DCU on CCU" << std::endl ;
	      getchar() ;
	    }
	    break;
	  case 2:
	    subDetector = "TOB" ;
	    if (getChannelKey(dcuD->getKey()) != 0x1e) {
	      dcuType = DCUFEH ;
	      std::cerr << "----> ERROR: DCU " << dcuD->getDcuType() << ": should a DCU on CCU" << std::endl ;
	      getchar() ;
	    }
	    break ;
	  case 3:
	    subDetector = "TEC" ;
	    if (getChannelKey(dcuD->getKey()) != 0x10) {
	      dcuType = DCUFEH ;
	      std::cerr << "----> ERROR: DCU " << dcuD->getDcuType() << ": should a DCU on CCU" << std::endl ;
	      getchar() ;
	    }
	    break ;
	  }
	  if (dcuType == DCUCCU) {
	    conversionFactors->setSubDetector(subDetector) ;
	    conversionFactors->setDcuHardId(dcuD->getDcuHardId()) ;
	    conversionFactors->setDcuType(dcuType) ;
	    vDcuConversion.push_back(conversionFactors) ;
	    dcuCCUAdded ++ ;
	  }
	  else delete conversionFactors ;
	}
#endif
      }
    }

    std::cout << "Found " << std::dec << vDcuConversion.size() << " DCU conversion factors for " << dcuVector.size() << " DCUs" << std::endl ;
    std::cout << "Autodection gives: " << std::endl ;
    std::cout << "\t" << counterFEH << " DCU on FEHs with " << counterFEHWith << " conversion factors and " << counterFEHWithout << " without conversion factors" << std::endl ;
    std::cout << "\t" << counterCCU << " DCU on CCUs with " << counterCCUWith << " conversion factors and " << counterCCUWithout << " without conversion factors" << std::endl ;

#ifdef MTCC
    std::cout << "Conversion factors added" << std::endl ;
    std::cout << "\t" << "Add " << dcuFEHAdded << " DCU on FEH conversion factors" << std::endl ;
    std::cout << "\t" << "Add " << dcuCCUAdded << " DCU on CCU conversion factors" << std::endl ;

    if ((vDcuConversion.size() > 0) && (outputFile != "")) {
      try {
	TkDcuConversionFactory tkDcuConversionFactory ;
	tkDcuConversionFactory.setOutputFileName(outputFile) ;
	tkDcuConversionFactory.setTkDcuConversionFactors(vDcuConversion) ;
	std::cout << "Upload terminated" << std::endl ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "Error during the upload in file " << outputFile << std::endl ;
	std::cerr << e.what() << std::endl ;
      }
      catch (oracle::occi::SQLException &e) {
	std::cerr << "Error during the upload in file " << outputFile << std::endl ;
	std::cerr << e.what() ;
      }
    }
#endif
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
  }
}

/** Upload the DET ID from a source to a destination
 * -uploadDetId
 */
void uploadDetID (TkDcuInfoFactory &deviceFactory, std::string inputFile, std::string outputFile, bool databaseUse ) {

  Sgi::hash_map<unsigned long, TkDcuInfo *> vDcuInfo ;
  TkDcuInfoFactory *tkDcuInfoFactoryInput  = NULL ;

  // Read the input
  if (databaseUse) {

    try {
      if (inputFile == "all" || inputFile == "ALL") {
	std::cout << "Read all the det id from the database" << std::endl ;
	deviceFactory.addAllDetId() ;
      }
      else {
	std::cout << "Read all the det id from the database for the partition " << inputFile << std::endl ;
	deviceFactory.addDetIdPartition(inputFile,2,0) ;
      }
      vDcuInfo = deviceFactory.getInfos() ;

      // for debug
      if (vDcuInfo.empty()) std::cerr << "No det id retreived from database" << std::endl ;
      //else {
      //unsigned int dcuId = 6156659 ;
      //if (vDcuInfo.find(dcuId) == vDcuInfo.end()) std::cerr << "No DET ID for DCU " << dcuId << "(0x" << std::hex << dcuId << ")" << std::endl ;
      //else std::cout << "The DCU ID " << dcuId << " (0x" << std::hex << dcuId << ")" << " have DET ID " << std::dec << vDcuInfo[dcuId]->getDetId() << std::endl ;
      //}
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to read the information from the database, partition " << inputFile << std::endl ;
      std::cerr << e.what() << std::endl;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Unable to read the information from the database, partition " << inputFile << std::endl ;
      std::cerr << e.what() << std::endl;
    }
  }
  else {

    try {
      std::cout << "Read all the det id from the file " << inputFile << std::endl ;
      tkDcuInfoFactoryInput = new TkDcuInfoFactory() ;
      tkDcuInfoFactoryInput->setInputFileName(inputFile) ;
      vDcuInfo = tkDcuInfoFactoryInput->getInfos() ; 
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to read the input file " << inputFile << std::endl ;
      std::cerr << e.what() << std::endl;
    }
  }

  // Upload part
  if (vDcuInfo.size()) {

    std::cout << "Found " << std::dec << vDcuInfo.size() << " DET IDs elements" << std::endl ;

//     for (Sgi::hash_map<unsigned long, TkDcuInfo *>::iterator it = vDcuInfo.begin() ; it != vDcuInfo.end() ; it++) {
//       it->second->display() ;
//     }

    // Write the output
    if (outputFile == "database") {
      try {
	deviceFactory.createNewDcuInfoVersion();
	deviceFactory.setUsingDb() ;
	deviceFactory.setTkDcuInfo(vDcuInfo) ;
	std::cout << "Upload in the database done" << std::endl ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "Unable to write the information in the database" << std::endl ;
	std::cerr << e.what() << std::endl;
      }
      catch (oracle::occi::SQLException &e) {
	std::cerr << "Unable to write the information in the database" << std::endl ;
	std::cerr << e.what() << std::endl;
      }
//       catch (XERCES_CPP_NAMESPACE::SAXException &e) {
// 	std::cerr << "Unable to write the information in the database" << std::endl ;
// 	std::cerr << e.what() << std::endl;
//       }
      catch (std::string e) {
	std::cerr << "Unable to write the information in the database" << std::endl ;
	std::cerr << e << std::endl;
      }
      catch (...) {
	std::cerr << "Unknown exception" << std::endl ;
      }
    }
    else {
      try {
	TkDcuInfoFactory tkDcuInfoFactoryOutput ;
	tkDcuInfoFactoryOutput.setOutputFileName(outputFile) ;
	tkDcuInfoFactoryOutput.setTkDcuInfo(vDcuInfo) ;
	std::cout << "Upload in the file " << outputFile << " done" << std::endl ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "Unable to write in the file " << outputFile << std::endl ;
	std::cerr << e.what() << std::endl;
      }
    }
  }
  else {
    if (databaseUse)
      std::cerr << "No DCU info found in the database for the partition " << inputFile << std::endl;
    else
      std::cerr << "No DCU info found in the file " << inputFile << std::endl;
  }

  if (tkDcuInfoFactoryInput != NULL) delete tkDcuInfoFactoryInput ;
}

/** Upload the ID vs hgostname from a source to a destination
 * -uploadIdVsHostname
 */
void uploadIdVsHostname (DeviceFactory &deviceFactory, std::string inputFile, std::string outputFile ) {

  try {
    TkIdVsHostnameVector tkId ;
    if (inputFile != "database") {

      std::cout << "Retreiving ID vs hostname from file " << inputFile << std::endl ;

      deviceFactory.setUsingFile() ;
      deviceFactory.setTkIdVsHostnameInputFileName (inputFile) ;
      tkId = deviceFactory.getAllTkIdVsHostname() ;
    }
    else {

      std::cout << "Retreiving ID vs hostname from database" << std::endl ;

      tkId = deviceFactory.getAllTkIdVsHostname() ;
    }

    std::cout << "Found " << tkId.size() << " ID vs hostname from " << inputFile << std::endl ;
    
    // Upload it
    if (tkId.size()) {

      std::cout << "Sending the ID versus hostname to " << outputFile << std::endl ;

      if (outputFile != "database") { // upload in file
	deviceFactory.setOutputFileName(outputFile) ;
	deviceFactory.setUsingFile() ;
	deviceFactory.setTkIdVsHostnameDescription ( tkId ) ;
	
	std::cout << "Upload in file " << outputFile << " done" << std::endl ;
      }
      else { // upload in database
	deviceFactory.setUsingDb() ;
	deviceFactory.setTkIdVsHostnameDescription ( tkId, true ) ;
	
	std::cout << "Upload in database done in a major version" << std::endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl;
  }    
  catch (oracle::occi::SQLException &e) {
    std::cerr << e.what() << std::endl ;
  }
  catch (std::string &e) {
    std::cerr << e << std::endl ;
  }
}

/** Upload the ID vs hgostname from a source to a destination
 * -uploadDcuPsuMap
 * -uploadPsuName
 */
void uploadDcuPsuMap (DeviceFactory &deviceFactory, std::string inputFile, std::string outputFile, std::string partitionName, bool onlyPsuName ) {

  try {
    tkDcuPsuMapVector vDcuPsuCG ;
    tkDcuPsuMapVector vDcuPsuPG ;

    if (inputFile != "database") {

      std::string::size_type loc = inputFile.find( ".xml", 0 );
      if (loc == std::string::npos) { // Formatted file from PVSS
	std::cout << "Retreiving PSU/DCU map from text file " << inputFile << std::endl ;
	deviceFactory.setUsingFile() ;
	deviceFactory.setInputTextFile (inputFile) ;
      }
      else {  // XML file
	std::cout << "Retreiving PSU/DCU map from xml file " << inputFile << std::endl ;
	deviceFactory.setUsingFile() ;
	deviceFactory.setTkDcuPsuMapFileName (inputFile) ;
      }

      vDcuPsuCG = deviceFactory.getControlGroupDcuPsuMaps() ;
      vDcuPsuPG = deviceFactory.getPowerGroupDcuPsuMaps() ;
    }
    else {

      if (onlyPsuName) {
	std::cout << "Retreiving PSU names from database for partition " << partitionName << std::endl ;
	deviceFactory.getPsuNamePartition(partitionName) ;
      }
      else {
	std::cout << "Retreiving DCU/PSU name from database for partition " << partitionName << std::endl ;
	deviceFactory.getDcuPsuMapPartition(partitionName) ;
      }
      vDcuPsuCG = deviceFactory.getControlGroupDcuPsuMaps() ;
      vDcuPsuPG = deviceFactory.getPowerGroupDcuPsuMaps() ;
    }

    std::cout << "Found " << vDcuPsuCG.size() << " control groups from " << inputFile << std::endl ;
    std::cout << "Found " << vDcuPsuPG.size() << " power groups from " << inputFile << std::endl ;    

    // Set it to the output as PsuNames
    if ( (outputFile == "database") && (partitionName != "") ) {
      deviceFactory.setUsingDb() ;
      unsigned int versionMajorId, versionMinorId ;
      std::cout << "Upload " << deviceFactory.getAllTkDcuPsuMaps().size() << " PSU names in database" << std::endl ;

      if (onlyPsuName) {
	deviceFactory.setTkPsuNames (deviceFactory.getAllTkDcuPsuMaps(), partitionName, &versionMajorId, &versionMinorId) ;
	std::cout << "Upload PSU names in version " << versionMajorId << "." << versionMinorId << " for partition " << partitionName << std::endl ;
      }
      else {
	deviceFactory.setTkDcuPsuMap (deviceFactory.getAllTkDcuPsuMaps(), partitionName, &versionMajorId, &versionMinorId) ;
	std::cout << "Upload DCU/PSU map in version " << versionMajorId << "." << versionMinorId << " for partition " << partitionName << std::endl ;
      }
    }
    else {
      deviceFactory.setUsingFile() ;
      deviceFactory.setOutputFileName (outputFile) ;
      deviceFactory.setTkPsuNames(deviceFactory.getAllTkDcuPsuMaps()) ;

      std::cout << "Output done in file " << outputFile << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl;
  }    
  catch (oracle::occi::SQLException &e) {
    std::cerr << e.what() << std::endl ;
  }
}


/* ************************************************************************************************ */
/*                                                                                                  */
/*                                               Main                                               */
/*                                                                                                  */
/* ************************************************************************************************ */
int main ( int argc, char **argv ) {

  DeviceFactory *deviceFactory = NULL ;
  std::string partitionName = "" ;

  std::list<std::string> listFileName ;

  // *************************************************************************************************
  // Check the options
  int command = 0 ;
  std::string inputFile = "" ;
  std::string outputFile = "" ;
  for (int i = 1 ; i < argc ; i ++) {

    if (!strncasecmp(argv[i], "-input", strlen("-input"))) {

      i ++ ;
      if (i < argc) inputFile = std::string(argv[i]) ;
      else {
	std::cerr << "Error in usage, bad values for the option input, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp(argv[i], "-output", strlen("-output"))) {

      i ++ ;
      if (i < argc) outputFile = std::string(argv[i]) ;
      else {
	std::cerr << "Error in usage, bad values for the option output, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp (argv[i], "-partition", strlen("-partition"))) {

      i ++ ;
      if (i < argc) partitionName = argv[i] ;
      else {
	std::cerr << "Error in usage, bad values for the option partition, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp(argv[i], "-reverse", strlen("-reverse"))) command = 3 ;
    else if (!strncasecmp(argv[i], "-uploadDcuConversion", strlen("-uploadDcuConversion"))) {

      command = 2 ;
      i ++ ;

      for ( ; i < argc ; i ++) listFileName.push_back(argv[i]) ;

      if (listFileName.size() == 0) {
	std::cerr << "Error in usage, bad values for the merge upload command, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp(argv[i], "-uploadDetId", strlen("-uploadDetId"))) {
      command = 6 ;
      i ++ ;

      if (i < argc) {
	partitionName = argv[i] ;
	i ++ ;
      }
    }
    else if (!strncasecmp(argv[i], "-analysis", strlen("-analysis"))) {
      command = 4 ;
      i ++ ;
      for ( ; i < argc ; i ++) listFileName.push_back(argv[i]) ;

      if (listFileName.size() == 0) {
	std::cerr << "Error in usage, bad values for the merge command, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }    
    else if (!strncasecmp(argv[i], "-completion", strlen("-completion"))) {
      command = 5 ;
      inputFile = "database" ;
    }    
    else if (!strncasecmp(argv[i], "-uploadIdVsHostname", strlen("-uploadIdVsHostname"))) {
      command = 7 ;
    }
    else if (!strncasecmp(argv[i], "-uploadDcuPsuMap", strlen("-uploadDcuPsuMap"))) {
      command = 8 ;
    }
    else if (!strncasecmp(argv[i], "-uploadPsuName", strlen("-uploadDcuPsuMap"))) {
      command = 9 ;
    }
    else if (!strncasecmp(argv[i], "-compareDcuConversion", strlen("-uploadDcuPsuMap"))) {
      command = 10 ;
    }
    else if (!strncasecmp(argv[i], "-disableModule", strlen("-disableModule"))) {
      command = 11 ;
    }
    else if (!strncasecmp(argv[i], "-help", strlen("-help"))) command = 0 ;
    else if (!strncasecmp(argv[i], "--help", strlen("--help"))) command = 0 ;
    else std::cerr << "Invalid argument: " << argv[i] << std::endl ;
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
  // Error in options or command
  int helpF = 0 ;
  if (command == 0) helpF = 1 ;
  if ((command == 3) && (inputFile == "")) helpF = 2 ;
  if ((command == 4) && (inputFile == "")) helpF = 3 ;
  if ((command == 4) && (inputFile == "database") && (partitionName == "")) helpF = 4 ;
  if ((command == 5) && (inputFile == "database") && (partitionName == "")) helpF = 5 ;
  if ((command == 6) && (inputFile == "database") && (partitionName == "")) helpF = 6 ;
  if ((command == 6) && ((inputFile == "") || (outputFile == ""))) helpF = 7 ;
  if ((command == 7) && ((inputFile == "") && (outputFile == ""))) helpF = 8 ;
  if ((command == 11) && (partitionName == "")) helpF = 11 ;

  if (helpF) {

    std::cerr << "Error in usage: wrong option or command (" << helpF << ")" << std::endl ;
    std::cerr << "\t" << argv[0] << " -output [file name | database] [-partition [<partition name> | ALL]] -uploadDcuConversion [<list of files> | database]" << std::endl ;
    std::cerr << "\t\tTake the input file read the data and download it to the output" << std::endl ;
    std::cerr << "\t" << argv[0] << " -input [file name | database] -output [file name | database] -reverse" << std::endl ;
    std::cerr << "\t\tReverse the DCU hard ID into the file and download it to the output" << std::endl ;
    std::cerr << "\t" << argv[0] << " -input [file name | database] -partition [Partition name] -analysis [list of files]" << std::endl ;
    std::cerr << "\t\t" << "Take the conversion factors from intput and analyse/convert the data from the list of files" << std::endl ;
    std::cerr << "\t\t" << "If you specify a database input then you should give a partition name" << std::endl ;
    std::cerr << "\t" << argv[0] << " -input database -partition [Partition name] -completion" << std::endl ;
    std::cerr << "\t\t" << "Retreive the DCU from the database and display the convesion factors together with the conversions" << std::endl ;
    std::cerr << "\t" <<  argv[0] << " -input [file name | database] -output [file name | database] -uploadDetId [partition name | ALL]" << std::endl ;
    std::cerr << "\t\t" << "Upload the DET id from the input to the output" << std::endl ;
    exit (EXIT_FAILURE) ;
  }

  // *************************************************************************************************
  // Apply command
  switch (command) {
  case 2: // Upload DCU Conversion factors
    uploadConversionFactors (*deviceFactory, outputFile, listFileName, partitionName ) ;
    break ;
  case 3: // reverse DCU hard id in file
    reverseFile (*deviceFactory, outputFile, inputFile ) ;
    break ;
  case 4: // Analyse the DCU from files
    dcuAnalysis (*deviceFactory, inputFile, outputFile, listFileName, partitionName ) ;
    break ;
  case 5:
    dcuCompletion(*deviceFactory, partitionName, outputFile) ;
    break ;
  case 6: // Upload DET ID
    if (inputFile == "database")
      uploadDetID (*deviceFactory,partitionName,outputFile, true) ;
    else
      uploadDetID (*deviceFactory,inputFile,outputFile, false) ;
    break ;
  case 7: // Upload ID vs hostname
    uploadIdVsHostname (*deviceFactory,inputFile,outputFile) ;
    break ;
  case 8: // Upload PSU / DCU map
    uploadDcuPsuMap (*deviceFactory,inputFile,outputFile,partitionName,false) ;
    break ;
  case 9: // Upload PSU names only
    uploadDcuPsuMap (*deviceFactory,inputFile,outputFile,partitionName,true) ;
    break ;
  case 10: // DCU conversion factors comparison
    compareDcuConversionFactors (inputFile,outputFile,*deviceFactory) ;
    break ;
  case 11: // disable DCUs
    disableModule ( *deviceFactory, partitionName ) ;
    break ;
  }

  if (deviceFactory != NULL) delete deviceFactory ;

  return 1 ;
}
