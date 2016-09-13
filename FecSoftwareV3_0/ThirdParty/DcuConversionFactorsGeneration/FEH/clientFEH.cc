/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Fec Software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fec Software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
                       Guido Magazzu - CERN - Geneva, Switzerland
*/

#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>

#include <list>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#include "DcuFehCalibration.h"

/********************************************************************************************************/
/* Put the hybrid ID in an error file when a DCU has problems                                           */
/* Put in an error file all the hybrids with no DCU                                                     */
/********************************************************************************************************/

//const char SABLIER[] = {'\\','|','/','-'} ;

using namespace std ;

//#define DEBUGMSGERROR
#define MAXCHAR 50
#define MAXREQUEST 1024

typedef std::list<std::string> stringList ;

/** Send a request to the construction database server
 */
std::string sendRequest ( std::string request, bool display = false ) ;

/** Error message
 */
void error (char *msg) ;

/** Container ID parsing
 */
void parseValueTags ( std::string str, stringList &listId, std::string valueBegStr = "<value>", std::string valueEndStr = "</value>" ) ;

/** Check that a string is a number
 */
bool numberStr ( std::string str ) ;

/** Parsing a list of parameters to retreive the doubles contained
 */
bool parseStringForDouble ( std::string parameterString, double *array, int siseExpected, std::string parameterName ) ;

/**  Parse a string and find the values between the character
 */
void parseValueCharacter ( std::string parameterString, stringList &listId, std::string character ) ;

/** Main program
 * \param argc - number of args
 * \param argv - 
 * <ul>
 * <li> argv[1] = output file name (cerr by default)
 * <li> argv[2] = number of requests (10 by default) => 0 means all values
 * <li> argv[3] = starting point for the linear regression (2 by default)
 * <li> argv[4] = end point for the linear regression (6 by default)
 * <li> argv[5] = inl overflow value (5 by default)
 * <li> argv[6] = table name (PRODUCTION OR RECEPTION), RECEPTION by default
 * </lu>
 */
int main(int argc, char *argv[]) {

  //int indexSablier = 0 ;

  unsigned int dcuMaxRequest = 10 ;
  unsigned int startPoint = 2, endPoint = 6 ;
  double inlOverflow = 5 ;
  std::string fileName = "/tmp/DcuFehCalibrationOutputDefault.txt" ;
  std::string tableName = "FHITRECEPTION_1_HYB_" ; // by default
  //std::string tableName = "FHITPRODUCTION_1_HYB_" ;

  if (argc == 2) {
    if (!strcmp (argv[1], "-help")) {

      std::cout << argv[0] << std::endl ;
      std::cout << "\t" << "output file name" << std::endl ;
      std::cout << "\t" << "number of requests (10 by default) => 0 means all values" << std::endl ;
      std::cout << "\t" << "starting point for the linear regression (2 by default)" << std::endl ;
      std::cout << "\t" << "end point for the linear regression (6 by default)" << std::endl ;
      std::cout << "\t" << "inl overflow value (5 by default)" << std::endl ;
      std::cout << "\t" << "table name (PRODUCTION OR RECEPTION), RECEPTION by default"  << std::endl ;

      exit (0) ;
    }
  }

  if (argc >= 2) {
    fileName = std::string(argv[1]) ;
    if (argc >= 3) {
      dcuMaxRequest = fromString<unsigned int>(argv[2]) ;
      if (argc >= 4) {
	startPoint = fromString<unsigned int>(argv[3]) ;
	if (argc >= 5) {
	  endPoint = fromString<unsigned int>(argv[4]) ;
	  if (argc >= 6) {
	    inlOverflow = fromString<double>(argv[5]) ;
	    if (argc >= 7) {
	      if (strcmp (argv[6], "PRODUCTION"))
		tableName = "FHITRECEPTION_1_HYB_" ;
	      else
		tableName = "FHITPRODUCTION_1_HYB_" ;
	    }
	  }
	}
      }
    }
  }

  if (dcuMaxRequest > 0)
    std::cout << "Retreive " << dcuMaxRequest << " DCUs from construction database starting from " << tableName << " between " << startPoint << " to " << endPoint << " (inl overflow = " << inlOverflow << ")" << std::endl ;
  else 
    std::cout << "Retreive all DCUs from construction database starting from table " << tableName << " between " << startPoint << " to " << endPoint << " (inl overflow = " << inlOverflow << ")" << std::endl ;
  std::cout << "Store the results in file " << fileName << std::endl ;

  std::cout << "Press <enter> to continue" ; getchar() ;

  // Vector of DCU calibration class
  std::vector<DcuFehCalibration *> vectorDcuCalib ;  
  std::string answer, request ;

  // ---------------------------------- Hybrids

  // List of the container ID
  stringList listContainerId ;
  
//#define SPECIFYCONTAINERID
#ifdef SPECIFYCONTAINERID

  // Bad hybrids
//   listContainerId.push_back("30216632016238") ;
//   listContainerId.push_back("30216632023269") ;
//   listContainerId.push_back("30216632023451") ;
//   listContainerId.push_back("30216652014795") ;
//   listContainerId.push_back("30216662028758") ;
//   listContainerId.push_back("30216711705723") ;
//   listContainerId.push_back("30216711911812") ;
//   listContainerId.push_back("30216752125734") ;
//   listContainerId.push_back("30216752125788") ;
//   listContainerId.push_back("30216772013863") ;

  // Some hybrids
//   listContainerId.push_back("30216631602342") ;
//   listContainerId.push_back("30216631602343") ;
//   listContainerId.push_back("30216631602344") ;
//   listContainerId.push_back("30216631602346") ;
//   listContainerId.push_back("30216631602348") ;

//  listContainerId.push_back("30216651604785") ;
  listContainerId.push_back("30216631604354") ;

#else
  request = "<?xml version=\"1.0\"?><select db=\"prod\">distinct OBJECT_ID from OBJECT_ASSEMBLY where object=\'HYB\' and OBJECT_ID is not null order by object_id</select>" ;

  // send the request
  answer = sendRequest ( request ) ;

  // Parse the answer
  if (answer.size()) parseValueTags (answer, listContainerId) ;
   
  // Display the value of the container ID
  std::cout << "Found " << listContainerId.size() << " Hybrids" << std::endl ;
  //for (stringList::iterator itr = listContainerId.begin() ; itr != listContainerId.end() ; itr ++) std::cout << (*itr) << std::endl ;
#endif
   
  // ---------------------------------- DCU_ID, ET_DCU_ID
  
  std::cout << "Retreiving from the construction database the DCU calibration values" << std::endl ;
  bool limitation = false ;
  for (stringList::iterator itr = listContainerId.begin() ; itr != listContainerId.end() && !limitation ; itr ++) {

//     std::cout << "\r" ;
//     if (indexSablier == 4) indexSablier = 0 ;
//     std::cout << SABLIER[indexSablier] ;
//     fflush (stdout) ;

    // To limit the number of requests
    if (dcuMaxRequest > 0) limitation = (vectorDcuCalib.size() == dcuMaxRequest) ;

    request = "<?xml version=\"1.0\"?><select db=\"prod\">ET_DCU_ID, DCU_ID, DCU_CAL_HIGHINPUT_MV, DCU_CAL_HIGH_VNOM, DCU_FHIT_MICROVPADC, DCU_0_RAW, DCU_1_RAW, DCU_2_RAW, DCU_3_RAW, DCU_4_RAW, DCU_7_RAW, FHIT_VREF_IN_MV, ET_V250_IN_MV, TOOL_ID, TEST_ID, INPUT_ID, FHIT_NUMBER, FHIT_FIRMWARE_VERSION, FHIT_BOARD_VERSION, DCU_6_RAW from " + tableName + " where OBJECT_ID=" + (*itr) + " AND status='reference'</select>" ;

    answer = sendRequest ( request ) ;

    //std::cout << request << std::endl ;
    //std::cout << answer << std::endl ;

    // Selection by row
    stringList rowList ;
    parseValueTags (answer, rowList, "<row>", "</row>") ;

    // For each row find the values
    for (stringList::iterator rowItr = rowList.begin() ; rowItr != rowList.end() ; rowItr ++) {

      //std::cout << *rowItr << std::endl ;

      // For each value
      stringList listParameters ;
      // Retreive the values for ET_DCU_ID and DCU id, etc.
      parseValueTags (*rowItr, listParameters, "<value>", "</value>") ;

      // Check the parameters
      if (listParameters.size() == 20) {

	// Compare the DCU_ID with the ET_DCU_ID
	stringList listEtDcuId ;
	stringList::iterator parameterIt = listParameters.begin() ;
	parseValueCharacter ( *parameterIt, listEtDcuId, " " ) ;

	if (listEtDcuId.size() == 3) {

	  // Convert the values in unsigned int and compare it with DCUID
	  stringList::iterator etDcuIdIt = listEtDcuId.begin() ;
	  unsigned int chipAddH = fromString<unsigned int>(*etDcuIdIt) ;
	  etDcuIdIt ++ ;
	  unsigned int chipAddM = fromString<unsigned int>(*etDcuIdIt) ;
	  etDcuIdIt ++ ;
	  unsigned int chipAddL = fromString<unsigned int>(*etDcuIdIt) ;

	  // Build the DCU ID
	  unsigned int chipadd = chipAddL | (chipAddM << 8) | (chipAddH << 16) ;
	  parameterIt ++ ;
	  unsigned int dcuId   = fromString<unsigned int>(*parameterIt) ;
	  if (chipadd == dcuId) {

	    // --
	    std::cout << vectorDcuCalib.size() << ": Find a DCU " << dcuId << std::endl ;

	    // Calculate the revert ID
	    chipadd = chipAddH | (chipAddM << 8) | (chipAddL << 16) ;

	    // Retreive the module ID, structure ID, subdetector
	    std::string requestOpt = "<?xml version=\"1.0\"?><select db=\"prod\">OA.container_id as \"Module\", OA2.container_id as \"Structure\", decode(substr(OA.type,1,1), 1, 'TIB', 2, 'TEC', 3, 'TOB', 4, 'TID', 'Unknown') as \"SubDetector\" from object_assembly OA, object_assembly OA2 where OA2.object_id=OA.container_id AND OA.object_id=" + (*itr) + "</select>" ;
	    std::string answerOpt = sendRequest ( requestOpt ) ;

	    //std::cout << requestOpt << std::endl ;
	    //std::cout << answerOpt << std::endl ;

	    // Retreive the module ID
	    //std::string request1 = "<?xml version=\"1.0\"?><select db=\"prod\">distinct CONTAINER_ID from OBJECT_ASSEMBLY where OBJECT_ID=" + (*itr) + "</select>" ;
	    //std::string answer1 = sendRequest ( request1 ) ;
	    stringList listId ;
	    if (answerOpt.size()) parseValueTags (answerOpt, listId) ;

	    // DCU calibration object
	    DcuFehCalibration *dcuCalib ;
	    std::string moduleId = "0" ;
	    std::string structureId = "0" ;
	    std::string subDetector = "Unknown" ;
	    if (listId.size() > 0) {
	      stringList::iterator itDes = listId.begin() ;
	      moduleId = *itDes ; itDes ++ ;
	      if (listId.size() > 1) {
		structureId = *itDes ; itDes ++ ;
		if (listId.size() > 2) {
		  subDetector = *itDes ; itDes ++ ;
		}
	      }
	    }

	    std::cout << "Module ID = " << moduleId << std::endl ;
	    std::cout << "Structure ID = " << structureId << std::endl ;
	    std::cout << "subDetector = " << subDetector << std::endl ;
	    std::cout << "object id = " << (*itr) << std::endl ;

	    dcuCalib = new DcuFehCalibration ( *itr, dcuId, chipadd, *listParameters.begin(), moduleId, structureId, subDetector ) ;

	    // Now take the parameters DCU_CAL_HIGHINPUT_MV
	    parameterIt ++ ; double DCU_CAL_HIGHINPUT_MV[13] ;
	    if (! parseStringForDouble (*parameterIt, DCU_CAL_HIGHINPUT_MV, 13, "DCU_CAL_HIGHINPUT_MV")) {

	      // --
	      // std::cout << "DCU_CAL_HIGHINPUT_MV parameters retreived" << std::endl ;

	      dcuCalib->setDcuCalHighInputMV ( DCU_CAL_HIGHINPUT_MV ) ;

	      // Now take the parameters DCU_CAL_HIGH_VNOM
	      parameterIt ++ ; double DCU_CAL_HIGH_VNOM[13] ;
	      if (! parseStringForDouble (*parameterIt, DCU_CAL_HIGH_VNOM, 13, "DCU_CAL_HIGH_VNOM")) {

		// --
		// std::cout << "DCU_CAL_HIGH_VNOM parameters retreived" << std::endl ;

		dcuCalib->setDcuCalHighVNOM ( DCU_CAL_HIGH_VNOM ) ;

		// Now take the parameters DCU_FHIT_MICROVPADC
		parameterIt ++ ; double DCU_FHIT_MICROVPADC[3] ;
		if (! parseStringForDouble (*parameterIt, DCU_FHIT_MICROVPADC, 3, "DCU_FHIT_MICROVPADC")) {

		  // --
		  // std::cout << "DCU_FHIT_MICROVPADC parameters retreived" << std::endl ;

		  dcuCalib->setDcuFhitMicroVPADC(DCU_FHIT_MICROVPADC) ;

		  // Now take the parameters DCU_0_RAW
		  parameterIt ++ ; double DCU_0_RAW[3] ;
		  if (! parseStringForDouble (*parameterIt, DCU_0_RAW, 3, "DCU_0_RAW")) {

		    // --
		    // std::cout << "DCU_0_RAW parameters retreived" << std::endl ;

		    dcuCalib->setDcu0Raw (DCU_0_RAW) ;
		    
		    // Now take the parameters DCU_1_RAW
		    parameterIt ++ ; double DCU_1_RAW[3] ;
		    if (! parseStringForDouble (*parameterIt, DCU_1_RAW, 3, "DCU_1_RAW")) {

		      // --
		      // std::cout << "DCU_1_RAW parameters retreived" << std::endl ;

		      dcuCalib->setDcu1Raw (DCU_1_RAW) ;

		      // Now take the parameters DCU_2_RAW
		      parameterIt ++ ; double DCU_2_RAW[3] ;
		      if (! parseStringForDouble (*parameterIt, DCU_2_RAW, 3, "DCU_2_RAW")) {

			// --
			// std::cout << "DCU_2_RAW parameters retreived" << std::endl ;

			dcuCalib->setDcu2Raw (DCU_2_RAW) ;

			// Now take the parameters DCU_3_RAW
			parameterIt ++ ; double DCU_3_RAW[3] ;
			if (! parseStringForDouble (*parameterIt, DCU_3_RAW, 3, "DCU_3_RAW")) {

			  // --
			  // std::cout << "DCU_3_RAW parameters retreived" << std::endl ;

			  dcuCalib->setDcu3Raw (DCU_3_RAW) ;

			  // Now take the parameters DCU_4_RAW
			  parameterIt ++ ; double DCU_4_RAW[3] ;
			  if (! parseStringForDouble (*parameterIt, DCU_4_RAW, 3, "DCU_4_RAW")) {

			    // --
			    // std::cout << "DCU_4_RAW parameters retreived" << std::endl ;

			    dcuCalib->setDcu4Raw (DCU_4_RAW) ;

			    // Now take the parameters DCU_7_RAW
			    parameterIt ++ ; double DCU_7_RAW[3] ;
			    if (! parseStringForDouble (*parameterIt, DCU_7_RAW, 3, "DCU_7_RAW")) {

			      // --
			      // std::cout << "DCU_7_RAW parameters retreived" << std::endl ;
			      dcuCalib->setDcu7Raw (DCU_7_RAW) ;

			      // -- FHIT_VREF_IN_MV
			      parameterIt ++ ;
			      double fhitVrefInMV = fromString<double>(*parameterIt) ;
			      dcuCalib->setFhitVrefInMV (fhitVrefInMV) ;
			      
			      // ET_V250_IN_MV
			      parameterIt ++ ; double etV250InMv[3] ;
			      parseStringForDouble (*parameterIt, etV250InMv, 3, "ET_V250_IN_MV") ;
			      dcuCalib->setEtV250InMv (etV250InMv[0]) ;
			      
			      // -- Add the toold id from the FHIT reception
			      parameterIt ++ ;
			      dcuCalib->setFRToolId ( *parameterIt ) ;

			      // -- Add the TEST_ID, INPUT_ID, FHIT_NUMBER, FHIT_FIRMWARE_VERSION, FHIT_BOARD_VERSION
			      parameterIt ++ ;
			      dcuCalib->setFRTestId ( *parameterIt ) ;
			      parameterIt ++ ;
			      dcuCalib->setFRInputId ( *parameterIt ) ;
			      parameterIt ++ ;
			      dcuCalib->setFhitNumber ( *parameterIt ) ;
			      parameterIt ++ ;
			      dcuCalib->setFhitFirmwareVersion ( *parameterIt ) ;
			      parameterIt ++ ;
			      dcuCalib->setFhitBoardVersion ( *parameterIt ) ;

			      // Add the DCU_6_RAW
			      parameterIt ++ ; double DCU_6_RAW[3] ;
			      if (! parseStringForDouble (*parameterIt, DCU_6_RAW, 3, "DCU_6_RAW")) {
				dcuCalib->setDcu6Raw0 (DCU_6_RAW[0]) ;
			      }
			      else {
				std::cerr << "ERROR when the DCU_6_RAW is retreived, set it to 0" << std::endl ;
				dcuCalib->setDcu6Raw0 (0) ;
			      }
			      
			      // Add the dcuCalib to the vector
			      vectorDcuCalib.push_back (dcuCalib) ;

			      // By default no values in the TESTWITHPA_1_HYB_
			      dcuCalib->setMissingTpa ( true ) ;

			      // Now send the request to retreive the values HYBTEMP, DCUH4, DCUH7
			      request = "<?xml version=\"1.0\"?><select db=\"prod\">HYBTEMP, DCUCH4, DCUCH7, TOOL_ID, TEST_ID, INPUT_ID from TESTWITHPA_1_HYB_ where OBJECT_ID=" + (*itr) + "AND status='reference'</select>" ;
			      // send the request
			      answer = sendRequest ( request ) ;

			      // parse the answer
			      stringList lastDcuHyb ;
			      if (answer.size()) {
				parseValueTags (answer, lastDcuHyb) ;

				if (lastDcuHyb.size() == 6) {

				  // --
				  // std::cout << "HYBTEMP, DCUCH4, DCUCH7 has been retreived" << std::endl ;
				  bool errorHDD = false ;
				  stringList::iterator lastPar = lastDcuHyb.begin() ;
				  if (*lastPar != "") {
				    double HYBTEMP = fromString<double>(*lastPar) ;
				    //std::cout << "HYBTEMP = " << *lastPar << std::endl ;
				    dcuCalib->setHybTemp (HYBTEMP) ;
				    lastPar ++ ;
				    if (*lastPar != "") {
				      double DCUCH4 = fromString<double>(*lastPar) ;
				      //std::cout << "DCUCH4 = " << *lastPar << std::endl ;
				      dcuCalib->setDcuCH4  (DCUCH4)  ;
				      lastPar ++ ;
				      if (*lastPar != "") {
					double DCUCH7 = fromString<double>(*lastPar) ;
					//std::cout << "DCUCH7 = " << string(*lastPar) << std::endl ;
					dcuCalib->setDcuCH7  (DCUCH7)  ;
					lastPar ++ ;
					if (*lastPar != "") {
					  dcuCalib->setPAToolId (*lastPar) ;
					  lastPar ++ ;
					  if (*lastPar != "") {
					    //std::cout << "TEST_ID = " << string (*lastPar) << std::endl ;
					    dcuCalib->setPATestId(*lastPar) ;
					    lastPar ++ ;
					    if (*lastPar != "") {
					      //std::cout << "INPUT_ID = " << string(*lastPar) << std::endl ;
					      dcuCalib->setPAInputId(*lastPar) ;
					    }
					    else errorHDD = true ;
					  }
					  else errorHDD = true ;
					}
					else errorHDD = true; 
				      }
				      else errorHDD = true ;
				    }
				    else errorHDD = true ;
				  }
				  else errorHDD = true ;

				  if (!errorHDD) 
				    dcuCalib->setMissingTpa ( false ) ;
				  else {
				    dcuCalib->setMissingTpa ( true ) ;
				    std::cerr << "One of the parameters for HYBTEMP, DCUCH4, DCUCH7 are empty" << std::endl ;
				    std::cerr << "Request = " << request << std::endl ;
				    std::cerr << "Request answer = " << answer << std::endl ;
				  }
				}
				else {
				  dcuCalib->setMissingTpa ( true ) ;
				  std::cerr << "ERROR: Incorrect number of parameters" << std::endl ;
				  std::cerr << "Request = " << request << std::endl ;
				  std::cerr << "Request answer = " << answer << std::endl ;
				  //getchar() ;
				}
			      }
			      else {
				dcuCalib->setMissingTpa ( true ) ;
				std::cerr << "ERROR: Incorrect number of parameters" << std::endl ;
				std::cerr << "Request = " << request << std::endl ;
				std::cerr << "Request answer = " << answer << std::endl ;
				//getchar() ;
			      }
			    }
			  }
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	  else {
	    std::cerr << "ERROR. the DCU ID and the ET DCU ID does not correspond (" << chipadd << " / " << dcuId << ")" << std::endl ;
	    std::cerr << "Request answer = " << answer << std::endl ;
	    //getchar() ;
	  }
	}
	else {
	  std::cerr << "ERROR: Incorrect number of parameters in ETDCUID" << std::endl ;
	  std::cerr << "Request answer = " << answer << std::endl ;
	  //getchar() ;
	}
      }
      else {
	std::cerr << "ERROR: Incorrect number of parameters" << std::endl ;
	std::cerr << "Request answer = " << answer << std::endl ;
	//getchar() ;
      }
    }
  }

  if (vectorDcuCalib.size()) {

    std::cout << "Found " << vectorDcuCalib.size() << " DCUs on " << listContainerId.size() << " hybrids" << std::endl ;

    std::ofstream *outputStream = NULL ; 
    outputStream = new std::ofstream (fileName.c_str()) ;
    if (!outputStream->is_open()) {
      std::cerr << "Error cannot open file " << fileName << ": exiting" << std::endl ;
      exit (-1) ;
    }

    *outputStream << "HybridId" << "\t" << "DCU Hard Id" << "\t" << "DCU Hard Revert Id" << "\t" << "Gain" << "\t" << "Offset" << "\t"
		  << "INL" << "\t" << "OW" << "\t" << "ADC0 Cal" << "\t"
		  << "I20" << "\t" << "I10" << "\t" << "KDiv" << "\t"
		  << "THybcal" << "\t" << "OffsetTScal" << "\t" << "TDCUcal" << "\t" 
		  << "THybtpa" << "\t" << "OffsetTStpa" << "\t" << "TDCUtpa" << "\t" 
		  << "HYBTEMP" << "\t" << "DCUCH4" << "\t" << "DCUCH7" << "\t" << "DCU_7_RAW[0]" << "\t" << "DCU_6_RAW[0]" << "\t"
		  << "FHIT_VREF_IN_MV" << "\t" << "ET_V250_IN_MV" << "\t"
		  << "FR Test ID" << "\t" << "FR tool ID" << "\t" << "FR input ID" << "\t"
		  << "PA Test ID" << "\t" << "PA tool ID" << "\t" << "PA input ID" << "\t"
		  << "FHIT_BOARD_VERSION" << "\t" << "FHIT_FIRMWARE_VERSION" << "\t"
		  << "Module ID" << "\t" << "structure ID" << "\t" << "subDetector" 
		  << std::endl ;

    //delete the vector of values
    for (vector<DcuFehCalibration *>::iterator itr = vectorDcuCalib.begin() ; itr != vectorDcuCalib.end() ; itr ++) {

      // Calculate the values for the conversion parameters
      (*itr)->evaluateConversionValues(startPoint,endPoint) ;

      *outputStream << (*itr)->getHybridId() << "\t" ;
      *outputStream << (*itr)->getDcuHardId() << "\t" ;
      *outputStream << (*itr)->getDcuHardRevertId() << "\t" ;

      *outputStream << (*itr)->getAdc0Gain() << "\t" ;
      *outputStream << (*itr)->getAdc0Offset() << "\t" ;
      *outputStream << (*itr)->getInl() << "\t" ;
      if ((*itr)->getInl() > inlOverflow) *outputStream << "INLOW" << "\t" ; //<< (*itr)->getIndexOW() ;
      else *outputStream << "NOINLOW" << "\t" ;
      if ((*itr)->getAdc0Cal()) *outputStream << "CALOK" << "\t" ;
      else *outputStream << "CALNOTOK" << "\t" ;

      *outputStream << (*itr)->getI20() << "\t" ;
      *outputStream << (*itr)->getI10() << "\t" ;
      *outputStream << (*itr)->getKDiv() << "\t" ;

      *outputStream << (*itr)->getThybcal() << "\t" ;
      *outputStream << (*itr)->getOffsetTScal() << "\t" ;

      if (! (*itr)->getMissingTpa()) {      
	*outputStream << (*itr)->getTdcucal() << "\t"; 
	*outputStream << (*itr)->getThybTpa() << "\t";
	*outputStream << (*itr)->getOffsetTStpa() << "\t" ;
	*outputStream << (*itr)->getTdcutpa() << "\t"; 
      }
      else {
	*outputStream << "NOTPA" << "\t" ;
	*outputStream << "NOTPA" << "\t" ;
	*outputStream << "NOTPA" << "\t" ;
	*outputStream << "NOTPA" << "\t" ;
      }

      if (! (*itr)->getMissingTpa()) {
	//*outputStream << (*itr)->getTref() << "\t"; // same as hybtemp
	*outputStream << (*itr)->getHybTemp() << "\t"; 
	*outputStream << (*itr)->getDcuCH4() << "\t" ;
	*outputStream << (*itr)->getDcuCH7() << "\t" ;
      }
      else {
	//*outputStream << "NOTPA" << "\t" ;
	*outputStream << "NOTPA" << "\t" ;
	*outputStream << "NOTPA" << "\t" ;
	*outputStream << "NOTPA" << "\t" ;
      }

      *outputStream << (*itr)->getDcu7Raw(0) << "\t" ;
      *outputStream << (*itr)->getDcu6Raw0() << "\t" ;

      *outputStream << (*itr)->getEtV250InMv() << "\t" ;
      *outputStream << (*itr)->getFhitVrefInMV() << "\t" ;

      *outputStream << (*itr)->getFRTestId() << "\t" ;
      *outputStream << (*itr)->getFRTooldId() << "\t" ;
      *outputStream << (*itr)->getFRInputId() << "\t" ;
      if (! (*itr)->getMissingTpa()) {
	*outputStream << (*itr)->getPATestId() << "\t" ;
	*outputStream << (*itr)->getPATooldId() << "\t" ;
	*outputStream << (*itr)->getPAInputId() << "\t" ;
      }
      else {
	*outputStream << "NOTPA" << "\t" ;
	*outputStream << "NOTPA" << "\t" ;
	*outputStream << "NOTPA" << "\t" ;
      }

      *outputStream << (*itr)->getFhitBoardVersion() << "\t" ;
      *outputStream << (*itr)->getFhitFirmwareVersion() << "\t" ;

      *outputStream << (*itr)->getModuleId() << "\t" ;
      *outputStream << (*itr)->getStructureId() << "\t" ;
      *outputStream << (*itr)->getSubDetector() << "\t" ;

      *outputStream << std::endl ;
       
      delete *itr ;
    }

    outputStream->flush();
    outputStream->close();
    delete outputStream ;
  }

  return 0;
}

/**  Parse a string and find the values between the character
 */
void parseValueCharacter ( std::string parameterString, stringList &listId, string character ) {

  //std::cout << parameterString << std::endl ;

  int index = 0 ;
  do {

    std::string::size_type begin = parameterString.find(character, index) + 1 ;    
    if (index == 0) begin = 0 ;
    if (begin != std::string::npos) {

      std::string::size_type end = parameterString.find(character, begin) ;
      if (end == std::string::npos) end = parameterString.size() ;

      std::string value = parameterString.substr(begin,end-begin) ;
      index = end ;

      //std::cout << begin << std::endl ;
      //std::cout << end << std::endl ;
      //std::cout << value << std::endl ;

      listId.push_back (value) ;
    }
    else index = parameterString.size() ;
  }
  while (index < parameterString.size()) ;
}

/** Retreive an array of double from a string
 */
bool parseStringForDouble ( std::string parameterString, double *arrayValues, int siseExpected, std::string parameterName ) {

  bool error = false ;

  stringList listOfParameters ;
  parseValueCharacter ( parameterString, listOfParameters, " ") ;
  if (listOfParameters.size() == siseExpected) {
    
    int i = 0 ;
    for (stringList::iterator itValue = listOfParameters.begin() ; itValue != listOfParameters.end() ; itValue ++){
      arrayValues[i] = fromString<double>(*itValue) ;
      i ++ ;
    }
    
  }
  else {
    std::cerr << "ERROR: invalid number of " << parameterName << std::endl ;
    std::cerr << "Parameters = " << parameterString << std::endl ;
    //getchar() ;
    error = true ;
  }
    
  return error ;
}

/** Send the request to the server
 * \param request to be sent
 * \return answer to the request
 */
std::string sendRequest ( std::string request, bool display ) {

    char buffer[MAXCHAR] ;
    std::string answer ;

#ifdef DEBUGMSGERROR
    if (display)
      std::cout << "Request = " << request << std::endl ;
#endif

    // --------------------------------------------
    // Open a connect to the server
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = 3615;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
      error("ERROR opening socket");
    server = gethostbyname("cmstrkdb.in2p3.fr");
    if (server == NULL) {
      std::cerr << "ERROR, no such host cmstrkdb.in2p3.fr" << std::endl ;
      exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serv_addr.sin_addr.s_addr,
	  server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
      std::cout << "The main server does not answer, using the backup server" << std::endl ;
      server = gethostbyname("lyopc72.in2p3.fr");
      if (server == NULL) {
	std::cerr << "ERROR, no such host: lyopc72.in2p3.fr" << std::endl ;
        exit(0);
      }
      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr, 
	    (char *)&serv_addr.sin_addr.s_addr,
	    server->h_length);
      serv_addr.sin_port = htons(portno);
      if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
	perror("Aucun serveur ne repond!\n");
      }
    }

    // ----------------------------------
    // Send the request
    n = send(sockfd,request.c_str(),strlen(request.c_str()),0);
    if (n < 0) 
         error("ERROR writing to socket");

    shutdown(sockfd,1);

    // -----------------------------------
    // Read the answer
    do {
      bzero(buffer,MAXCHAR);
      n = recv(sockfd,buffer,MAXCHAR-10,0);

#ifdef DEBUGMSGERROR
      if (display)
	std::cout << "Buffer = " << buffer << std::endl ;
#endif

      if (n < 0) 
	error("ERROR reading from socket");

      // Concat the buffer to the answer
      if (answer.size()!=0) answer += buffer;
      else answer = buffer;

#ifdef DEBUGMSGERROR
      if (display)
	std::cout << "answer = " << answer << std::endl ;
#endif
    }
    while (n > 0) ;

    // ----------------------------------------------------
    // Close the connection to the server
    close(sockfd);

    // Check the status of the request
    if (answer.find("<status>") != std::string::npos) {
      if (answer.find ("OK") == std::string::npos) {

	std::cerr << "ERROR: request has not correctly performed" << std::endl ;
	std::cerr << "Original request: " << request << std::endl ;
	std::cerr << "Answer is: " << answer.substr (0, 40) << std::endl ;
	answer = "" ;
	//getchar() ;
      }
#ifdef DEBUGMSGERROR
      else {
	std::cout << "Request correctly performed" << std::endl ;
      }
#endif
    }
    else {

      std::cerr << "ERROR: request has no status" << std::endl ;
      std::cerr << "Original request: " << request << std::endl ;
      std::cerr << "Answer is: " << answer.substr (0, 40) << std::endl ;
      answer = "" ;
      //getchar() ;
    }

    return (answer) ;
}

/** Find all the container ID values inside the buffer
 * \param str - string with answer
 * \param listId - list of the container ID
 * \param valueBegStr - begin tag
 * \param valueEndStr - end tag
 */
void parseValueTags ( std::string str, stringList &listId, std::string valueBegStr, std::string valueEndStr) {

  char buffer1[MAXCHAR];
  bzero(buffer1,MAXCHAR) ;

  bool found = true ;
  unsigned int index = 0 ;
  
  if (str.size() > 0) {
    
    do {

#ifdef DEBUGMSGERROR
      std::cout << "String to be analysed = " << str.substr(index,(str.size()-index)) << std::endl ;
#endif

      std::string::size_type debut = str.find ( valueBegStr, index ) ;
      std::string::size_type fin   = str.find ( valueEndStr, index ) ;
      
#ifdef DEBUGMSGERROR
      std::cout << "Debut = " << debut << std::endl ;
      std::cout << "Fin = " << fin << std::endl ;
      std::cout << "Number of char = " << (fin-debut) << std::endl ;
#endif

      if (debut == std::string::npos) {
	// No more values in the requests
	found = false ;
      }
      else if (fin == std::string::npos) {
	// a beginning exists but not the end, the end of the request is not complete

#ifdef DEBUGMSGERROR
	std::cout << "str = " << str.substr(debut,str.size()-debut-valueBegStr.size()) << std::endl ;
	std::cout << "Debut = " << debut << std::endl ;
	std::cout << "str.size() = " << str.size() << std::endl ;
#endif

	strncpy (buffer1, str.substr(debut,str.size()-debut-valueBegStr.size()).c_str(),MAXCHAR);
	bzero(&buffer1[str.size()-debut-valueBegStr.size()], MAXCHAR) ;
	found = false ;
      }
      else {

	// find the container ID
	std::string valueStr = str.substr(debut+valueBegStr.size(), (fin-debut-valueBegStr.size()))  ;
#ifdef DEBUGMSGERROR
	std::cout << "Value = " << valueStr << std::endl ;
#endif

	// Store it
	//if (numberStr(valueStr))
	listId.push_back(valueStr) ;
	//else {
	//std::cerr << "Invalid value in the container ID" << std::endl ;
	//std::cout << "str = " << str.substr(debut+valueBegStr.size(), (fin-debut-valueBegStr.size())) << std::endl ;
	//std::cout << "Value = " << valueStr << std::endl ;
	//getchar() ;
	//}

	// change the index to
	index = fin + valueEndStr.size() ;
      }
    }
    while (found) ;
  }
  else {
    std::cerr << "No data inside the request" << std::endl ;
  }

#ifdef DEBUGMSGERROR
  std::cout << "Buffer1 = " << str << std::endl ;
#endif
}

/** Check if the string is a number
 */
bool numberStr ( std::string str ) {

  bool numb = true ;
  for (unsigned int i = 0 ; (i < str.size()) && numb; i ++) {

    char c = str.at(i) ;
    switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      break ;
    default:
      numb = false ;
    }
  }

  return (numb) ;
}

/** Error message
 * \param msg - message to be displayed
 */
void error(char *msg)
{
    perror(msg);
    exit(0);
}

