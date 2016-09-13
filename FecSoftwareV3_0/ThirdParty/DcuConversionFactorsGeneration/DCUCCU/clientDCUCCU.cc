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

#include "DcuCcuCalibration.h"

/********************************************************************************************************/
/* Put the hybrid ID in an error file when a DCU has problems                                           */
/* Put in an error file all the hybrids with no DCU                                                     */
/********************************************************************************************************/

// Compatibility with the G++ version 3.X
// Retreive from http://gcc.gnu.org/onlinedocs/libstdc++/faq/
#ifdef __GNUC__
#if __GNUC__ < 3
#include <hash_map>
namespace Sgi { using ::hash_map; }; // inherit globals
#else
#include <ext/hash_map>
#if __GNUC_MINOR__ == 0
namespace Sgi = std;               // GCC 3.0
#else
namespace Sgi = ::__gnu_cxx;       // GCC 3.1 and later
#endif
#endif
#else      // ...  there are other compilers, right?
namespace Sgi = std;
#endif


#define DUPLICATEDDCUFILE "DcuCcuDuplicated.txt"

using namespace std ;

#define MAXCHAR 50
typedef std::list<std::string> stringList ;

/** Send a request to the construction database server
 */
std::string sendRequest ( std::string request ) ;

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

/** Check the duplicated DCU to see if a problem exists
 */
DcuCcuCalibration *compareDcuCalibration(DcuCcuCalibration *test1, DcuCcuCalibration *test2) {

  std::ofstream fileDuplicated(DUPLICATEDDCUFILE) ;

  if ((test1->getSubDetector() == test2->getSubDetector()) &&
      (test1->getCcuId() == test2->getCcuId()) &&
      (test1->getStructureId() == test2->getStructureId()) ) {

    int goodflag1 = 0, goodflag2 = 0 ;
    if (test1->getTsCal()) goodflag1 ++ ;
    if (test1->getAdc0Cal()) goodflag1 ++ ;
    if (test1->getICal()) goodflag1 ++ ;
    if (test1->getR68Cal()) goodflag1 ++ ;
    if (test1->getAdc2Cal()) goodflag1 ++ ;
    if (test1->getAdc3Cal()) goodflag1 ++ ;

    if (test2->getTsCal()) goodflag2 ++ ;
    if (test2->getAdc0Cal()) goodflag2 ++ ;
    if (test2->getICal()) goodflag2 ++ ;
    if (test2->getR68Cal()) goodflag2 ++ ;
    if (test2->getAdc2Cal()) goodflag2 ++ ;
    if (test2->getAdc3Cal()) goodflag2 ++ ;

    if (goodflag1 == goodflag2) {

      if (fileDuplicated.is_open()) {
	fileDuplicated << "--------------------------------------------------------------------" << std::endl ;
	fileDuplicated << "DCU " << test1->getDcuHardId() << " duplicated: " << " but same values" << std::endl ;
      }
      else {
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "DCU " << test1->getDcuHardId() << " duplicated: " << " but same values" << std::endl ;
      }
    } else if (goodflag1 < goodflag2) { 

      DcuCcuCalibration *temp ;
      temp = test1 ;
      test1 = test2 ;
      test2 = temp ;
    }
  }

  if (!fileDuplicated.is_open()) {
    std::cerr << "--------------------------------------------------------------------" << std::endl ;
    std::cerr << "DCU " << test1->getDcuHardId() << " duplicated: " << " but same values" << std::endl ;
    std::cerr << test1->getSubDetector() << "\t\t" ;
    std::cerr << test1->getCcuId() << "\t\t" ;
    std::cerr << test1->getStructureId() << "\t\t" ;
    std::cerr << test1->getDcuHardId() << "\t\t" ;
    std::cerr << test1->getDcuHardRevertId() << "\t\t" ;
    std::cerr << test1->getAdcGain0() << "\t\t" ;
    std::cerr << test1->getAdcOffset0() << "\t\t" ;
    std::cerr << test1->getAdc0Cal() << "\t\t" ;
    std::cerr << test1->getAdcGain2() << "\t\t" ;
    std::cerr << test1->getAdcOffset2() << "\t\t" ;
    std::cerr << test1->getAdc2Cal() << "\t\t" ;
    std::cerr << test1->getAdcGain3() << "\t\t" ;
    std::cerr << test1->getAdcOffset3() << "\t\t" ;
    std::cerr << test1->getAdc3Cal() << "\t\t" ;
    std::cerr << test1->getI20() << "\t\t" ;
    std::cerr << test1->getI10() << "\t\t" ;
    std::cerr << test1->getICal() << "\t\t" ;
    std::cerr << test1->getR68() << "\t\t" ;
    std::cerr << test1->getR68Cal() << "\t\t" ;
    std::cerr << test1->getTsGain() << "\t\t" ;
    std::cerr << test1->getTsOffset() << "\t\t" ;
    std::cerr << test1->getTsCal() << "\t\t" ;
    std::cerr << std::endl ;
      
    std::cerr << test2->getSubDetector() << "\t\t" ;
    std::cerr << test2->getCcuId() << "\t\t" ;
    std::cerr << test2->getStructureId() << "\t\t" ;
    std::cerr << test2->getDcuHardId() << "\t\t" ;
    std::cerr << test2->getDcuHardRevertId() << "\t\t" ;
    std::cerr << test2->getAdcGain0() << "\t\t" ;
    std::cerr << test2->getAdcOffset0() << "\t\t" ;
    std::cerr << test2->getAdc0Cal() << "\t\t" ;
    std::cerr << test2->getAdcGain2() << "\t\t" ;
    std::cerr << test2->getAdcOffset2() << "\t\t" ;
    std::cerr << test2->getAdc2Cal() << "\t\t" ;
    std::cerr << test2->getAdcGain3() << "\t\t" ;
    std::cerr << test2->getAdcOffset3() << "\t\t" ;
    std::cerr << test2->getAdc3Cal() << "\t\t" ;
    std::cerr << test2->getI20() << "\t\t" ;
    std::cerr << test2->getI10() << "\t\t" ;
    std::cerr << test2->getICal() << "\t\t" ;
    std::cerr << test2->getR68() << "\t\t" ;
    std::cerr << test2->getR68Cal() << "\t\t" ;
    std::cerr << test2->getTsGain() << "\t\t" ;
    std::cerr << test2->getTsOffset() << "\t\t" ;
    std::cerr << test2->getTsCal() << "\t\t" ;
    std::cerr << std::endl ;
  }
  else {
    fileDuplicated << "--------------------------------------------------------------------" << std::endl ;
    fileDuplicated << "DCU " << test1->getDcuHardId() << " duplicated: " << " values are different: choose the first display of values" << std::endl ;
    fileDuplicated << test1->getSubDetector() << "\t\t" ;
    fileDuplicated << test1->getCcuId() << "\t\t" ;
    fileDuplicated << test1->getStructureId() << "\t\t" ;
    fileDuplicated << test1->getDcuHardId() << "\t\t" ;
    fileDuplicated << test1->getDcuHardRevertId() << "\t\t" ;
    fileDuplicated << test1->getAdcGain0() << "\t\t" ;
    fileDuplicated << test1->getAdcOffset0() << "\t\t" ;
    fileDuplicated << test1->getAdc0Cal() << "\t\t" ;
    fileDuplicated << test1->getAdcGain2() << "\t\t" ;
    fileDuplicated << test1->getAdcOffset2() << "\t\t" ;
    fileDuplicated << test1->getAdc2Cal() << "\t\t" ;
    fileDuplicated << test1->getAdcGain3() << "\t\t" ;
    fileDuplicated << test1->getAdcOffset3() << "\t\t" ;
    fileDuplicated << test1->getAdc3Cal() << "\t\t" ;
    fileDuplicated << test1->getI20() << "\t\t" ;
    fileDuplicated << test1->getI10() << "\t\t" ;
    fileDuplicated << test1->getICal() << "\t\t" ;
    fileDuplicated << test1->getR68() << "\t\t" ;
    fileDuplicated << test1->getR68Cal() << "\t\t" ;
    fileDuplicated << test1->getTsGain() << "\t\t" ;
    fileDuplicated << test1->getTsOffset() << "\t\t" ;
    fileDuplicated << test1->getTsCal() << "\t\t" ;
    fileDuplicated << std::endl ;
    
    fileDuplicated << test2->getSubDetector() << "\t\t" ;
    fileDuplicated << test2->getCcuId() << "\t\t" ;
    fileDuplicated << test2->getStructureId() << "\t\t" ;
    fileDuplicated << test2->getDcuHardId() << "\t\t" ;
    fileDuplicated << test2->getDcuHardRevertId() << "\t\t" ;
    fileDuplicated << test2->getAdcGain0() << "\t\t" ;
    fileDuplicated << test2->getAdcOffset0() << "\t\t" ;
    fileDuplicated << test2->getAdc0Cal() << "\t\t" ;
    fileDuplicated << test2->getAdcGain2() << "\t\t" ;
    fileDuplicated << test2->getAdcOffset2() << "\t\t" ;
    fileDuplicated << test2->getAdc2Cal() << "\t\t" ;
    fileDuplicated << test2->getAdcGain3() << "\t\t" ;
    fileDuplicated << test2->getAdcOffset3() << "\t\t" ;
    fileDuplicated << test2->getAdc3Cal() << "\t\t" ;
    fileDuplicated << test2->getI20() << "\t\t" ;
    fileDuplicated << test2->getI10() << "\t\t" ;
    fileDuplicated << test2->getICal() << "\t\t" ;
    fileDuplicated << test2->getR68() << "\t\t" ;
    fileDuplicated << test2->getR68Cal() << "\t\t" ;
    fileDuplicated << test2->getTsGain() << "\t\t" ;
    fileDuplicated << test2->getTsOffset() << "\t\t" ;
    fileDuplicated << test2->getTsCal() << "\t\t" ;
    fileDuplicated << std::endl ;
  }
  //delete test2 ;
  return test1 ;
}

/** Parse the DCU for the TOB DCU on CCU
 */
void parseTOBDCUCCU ( unsigned int dcuMaxRequest, std::vector<DcuCcuCalibration *> &vectorDcuCalib, Sgi::hash_map<unsigned int, DcuCcuCalibration *> &hashMapDcuCcuCalibration ) {

  std::string answer, request ;

  // -----------------------------------------------------------------------------------------------------------------------------
  // ---------------------------------- TOB
  
  std::cout << "Retreiving the data for the TOB" << std::endl ; 
  //std::cout << "Press <enter> to continue" ; getchar() ;
  // "select OBJECT_ID, DCUID_H, DCUID_M, DCUID_L, G0, D0, G2, D2, G3, D3, I0, R68, TSOUT, TSROUT from DCUCALIB_1_CCUM_" ;
  //         ccuId      ....
  request = "<?xml version=\"1.0\"?><select db=\"prod\">OBJECT_ID, DCUID_H, DCUID_M, DCUID_L, G0, D0, G2, D2, G3, D3, I0, R68, TSOUT, TSROUT from DCUCALIB_1_CCUM_</select>" ;

  // ??????????????????????????????????????????????????????????????????????????????????????????????
  // If we want to evaluate the error (TS_ERROR) on TOB, we need to read a file with temp and tempr
  // ??????????????????????????????????????????????????????????????????????????????????????????????

  // send the request
  answer = sendRequest ( request ) ;

  // Here I must have the following values
  // 30242000000096  	89  	254  	123  	2.0931  	3.3889  	0.84444  	0.85828  	1.5778  	0.57397  	20.2502  	67.78  	2704  	2717
  // ccuId              ...

  // Selection by row
  stringList rowList ;
  parseValueTags (answer, rowList, "<row>", "</row>") ;

  // For each row find the values
  unsigned int maxRequest = 0 ;
  for (stringList::iterator rowItr = rowList.begin() ; (rowItr != rowList.end()) && ( (dcuMaxRequest == 0) || (maxRequest < dcuMaxRequest) ) ; rowItr ++) {

    //std::cout << *rowItr << std::endl ;
    // Retreive the values for OBJECT_ID, DCUID_H, DCUID_M, DCUID_L, G0, D0, G2, D2, G3, D3, I0, R68, TSOUT, TSROUT
    stringList listParameters ;
    parseValueTags (*rowItr, listParameters, "<value>", "</value>") ;
    if (listParameters.size() == 14) {

      // Retreive the parameters
      stringList::iterator parameterIt = listParameters.begin() ;
      std::string ccuId = *parameterIt ; parameterIt ++ ;
      std::string etDcuIdH = *parameterIt ; parameterIt ++ ;
      std::string etDcuIdM = *parameterIt ; parameterIt ++ ;
      std::string etDcuIdL = *parameterIt ; parameterIt ++ ;
      double G0 = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double D0 = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double G2 = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double D2 = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double G3 = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double D3 = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double I0 = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double R68 = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double TSOUT = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double TSROUT = fromString<double>(*parameterIt) ; parameterIt ++ ;

      // "select TYPE, CONTAINER_ID from OBJECT_ASSEMBLY where OBJECT_ID = " + VALUE READ IN THE PREVIOUS REQUEST AS OBJECT_ID OR ccuID ;
      request = "<?xml version=\"1.0\"?><select db=\"prod\">TYPE, CONTAINER_ID from OBJECT_ASSEMBLY where OBJECT_ID = " + ccuId + "</select>" ;
      // send the request
      answer = sendRequest ( request ) ;

      // Selection by row
      stringList rowListType ;
      parseValueTags (answer, rowListType, "<row>", "</row>") ;

      if (rowListType.size() == 1) {

	// Retreive the values for TYPE, CONTAINER_ID
	stringList listParametersType ;
	stringList::iterator rowItrType = rowListType.begin() ;
	parseValueTags (*rowItrType, listParametersType, "<value>", "</value>") ;

	if (listParametersType.size() == 2) {

	  stringList::iterator parameterItType = listParametersType.begin() ;
	  unsigned int subDetector = fromString<unsigned int>(*parameterItType) ; parameterItType ++ ;
	  std::string structureId = *parameterItType ; parameterItType ++ ;

	  //std::cout << structureId.at(0) << std::endl ;
	  //if (structureId.at(0) != '3') std::cout << *rowItrType << std::endl ;

	  try {
	    /*          added by Guido - June 7th 2006 */
	    if (ccuId == "") ccuId = "0" ;             
	    /*          added by Guido - June 7th 2006 */
	    if (structureId == "") structureId = "0" ; 
	    DcuCcuCalibration *dcuCcuCalibration = new DcuCcuCalibration (subDetector, ccuId, etDcuIdH, etDcuIdM, etDcuIdL, structureId, 

									  G0, D0, G2, D2, G3, D3, I0, R68, TSOUT, TSROUT) ;
	    
	    if (hashMapDcuCcuCalibration.find(dcuCcuCalibration->getDcuHardId()) != hashMapDcuCcuCalibration.end()) {
	      hashMapDcuCcuCalibration[dcuCcuCalibration->getDcuHardId()] = compareDcuCalibration(hashMapDcuCcuCalibration[dcuCcuCalibration->getDcuHardId()],dcuCcuCalibration) ;
	    }
	    else {
	      hashMapDcuCcuCalibration[dcuCcuCalibration->getDcuHardId()] = dcuCcuCalibration ;
	    }
	    vectorDcuCalib.push_back(dcuCcuCalibration) ;

	    maxRequest ++ ;
	  }
	  catch (std::string e) {

	    std::cerr << "Cannot create a DCU on CCU for " << ccuId << ": " << e << std::endl ;
	  }
	}
	else std::cerr << "Invalid number of TYPE, CONTAINER_ID (must be 2) = " << listParametersType.size() << std::endl ;
      }
      else std::cerr << "Invalid number of type, (must be 1) = " << rowListType.size() << std::endl ;
      
    }
    else std::cerr << "Invalid number of OBJECT_ID, DCUID_H, DCUID_M, DCUID_L, G0, D0, G2, D2, G3, D3, I0, R68, TSOUT, TSROUT (must be 14) = " << listParameters.size() << std::endl ;
  }
}

/** Parse the TIB/TEC DCU on CCU
 */
void parseTECDCUCCU ( unsigned int dcuMaxRequest, std::vector<DcuCcuCalibration *> &vectorDcuCalib, Sgi::hash_map<unsigned int, DcuCcuCalibration *> &hashMapDcuCcuCalibration ) {

  std::string answer, request ;

  // -----------------------------------------------------------------------------------------------------------------------------
  // ---------------------------------- TIB, TEC
  std::cout << "Retreiving the data for the TEC" << std::endl ; 
  //std::cout << "Press <enter> to continue" ; getchar() ;
  //"select OBJECT_ID, A_MICROVPADC, B_MICROV, I20_MICROA, I10_MICROA, TS, TS_REF, TEMP_DEGC, TEMP_REF_DEGC from DCUCALIB_2_CCUM_" ;
  //        ccuId       ....
  request = "<?xml version=\"1.0\"?><select db=\"prod\">OBJECT_ID, DCUID_H, DCUID_M, DCUID_L, A_MICROVPADC, B_MICROV, I20_MICROA, I10_MICROA, TS, TS_REF, TEMP_DEGC, TEMP_REF_DEGC from DCUCALIB_2_CCUM_</select>" ;

  // send the request
  answer = sendRequest ( request ) ;

  // Here I must have the following values
  // 30252000000802  	466.01  	-3494.67  	19.08  	9.41  	2630  	2698  	29.5  	26
  // ccuId              ...

  // Selection by row
  stringList rowList ;
  parseValueTags (answer, rowList, "<row>", "</row>") ;

  // For each row find the values
  unsigned int maxRequest = 0 ;
  for (stringList::iterator rowItr = rowList.begin() ; (rowItr != rowList.end()) && ( (dcuMaxRequest == 0) || (maxRequest < dcuMaxRequest) ) ; rowItr ++) {

    //std::cout << *rowItr << std::endl ;
    // Retreive the values for OBJECT_ID,  A_MICROVPADC, B_MICROV, I20_MICROA, I10_MICROA, TS, TS_REF, TEMP_DEGC, TEMP_REF_DEGC
    stringList listParameters ;
    parseValueTags (*rowItr, listParameters, "<value>", "</value>") ;
    if (listParameters.size() == 12) {

      // Retreive the parameters
      stringList::iterator parameterIt = listParameters.begin() ;
      std::string ccuId = *parameterIt ; parameterIt ++ ;
      std::string etDcuIdH = *parameterIt ; parameterIt ++ ;
      std::string etDcuIdM = *parameterIt ; parameterIt ++ ;
      std::string etDcuIdL = *parameterIt ; parameterIt ++ ;
      double A_MICROVPADC = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double B_MICROV = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double I20_MICROA = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double I10_MICROA = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double TS = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double TS_REF = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double TEMP_DEGC = fromString<double>(*parameterIt) ; parameterIt ++ ;
      double TEMP_REF_DEGC = fromString<double>(*parameterIt) ; parameterIt ++ ;

      // "select TYPE, CONTAINER_ID from OBJECT_ASSEMBLY where OBJECT_ID = " + VALUE READ IN THE PREVIOUS REQUEST AS OBJECT_ID OR ccuID ;
      request = "<?xml version=\"1.0\"?><select db=\"prod\">TYPE, CONTAINER_ID from OBJECT_ASSEMBLY where OBJECT_ID = " + ccuId + "</select>" ;
      // send the request
      answer = sendRequest ( request ) ;

      // Selection by row
      stringList rowListType ;
      parseValueTags (answer, rowListType, "<row>", "</row>") ;
      
      if (rowListType.size() == 1) {
	
	// Retreive the values for TYPE, CONTAINER_ID
	stringList listParametersType ;
	stringList::iterator rowItrType = rowListType.begin() ;
	parseValueTags (*rowItrType, listParametersType, "<value>", "</value>") ;

	if (listParametersType.size() == 2) {

	  // Type and DOHMID or petal ID
	  stringList::iterator parameterItType = listParametersType.begin() ;
	  unsigned int subDetector = fromString<unsigned int>(*parameterItType) ; parameterItType ++ ;
	  std::string dohMID = *parameterItType ; parameterItType ++ ;
	  std::string structureId = dohMID ;

	  // Structure ID for back petal
	  if (subDetector == 8) {
	    std::string requestStruct = "<?xml version=\"1.0\"?><select db=\"prod\">CONTAINER_ID from OBJECT_ASSEMBLY where OBJECT_ID = " + dohMID + "</select>" ;
	    std::string answerStruct = sendRequest ( requestStruct ) ;
	    stringList listParametersStruct ;
	    parseValueTags (answerStruct, listParametersStruct, "<value>", "</value>") ;
	    if (listParametersStruct.size() == 1) {
	      stringList::iterator structType = listParametersStruct.begin() ;
	      std::string structureId = *structType ; structType ++ ;
	    }
	    else {
	      std::cerr << "Invalid number of structure id (must be 1) = " << listParametersStruct.size() << std::endl ;
	      structureId = "Unknown" ;
	    }
	  }

	  //if (structureId != "Unknown") {
	    try {
	      if (ccuId == "") ccuId = "0" ;
	      if (structureId == "") structureId = "0" ;
	      DcuCcuCalibration *dcuCcuCalibration = new DcuCcuCalibration (subDetector, ccuId, etDcuIdH, etDcuIdM, etDcuIdL, structureId, 
									    A_MICROVPADC, B_MICROV, I20_MICROA, I10_MICROA, TS, TS_REF, TEMP_DEGC, TEMP_REF_DEGC) ;

	      if (hashMapDcuCcuCalibration.find(dcuCcuCalibration->getDcuHardId()) != hashMapDcuCcuCalibration.end()) {
		hashMapDcuCcuCalibration[dcuCcuCalibration->getDcuHardId()] =  compareDcuCalibration(hashMapDcuCcuCalibration[dcuCcuCalibration->getDcuHardId()],dcuCcuCalibration) ;
	      }
	      else {
		hashMapDcuCcuCalibration[dcuCcuCalibration->getDcuHardId()] = dcuCcuCalibration ;
	      }
	      vectorDcuCalib.push_back(dcuCcuCalibration) ;
	      
	      maxRequest ++ ;
	    }
	    catch (std::string e) {
	    
	      std::cerr << "Cannot create a DCU on CCU for " << ccuId << ": " << e << std::endl ;
	    }
	  //}
	}
	else std::cerr << "Invalid number of TYPE, CONTAINER_ID (must be 2) = " << listParametersType.size() << std::endl ;
      }
      else std::cerr << "Invalid number of type, (must be 1) = " << rowListType.size() << std::endl ;
      
    }
    else std::cerr << "Invalid number of OBJECT_ID, DCUID_H, DCUID_M, DCUID_L, G0, D0, G2, D2, G3, D3, I0, R68, TSOUT, TSROUT (must be 14) = " << listParameters.size() << std::endl ;
  }
}

/** Main program
 * \param argc - number of args
 * \param argv - 
 * <ul>
 * <li> argv[1] = output file name (default)
 * <li> argv[2] = number of requests (10 by default) => 0 means all values
 * <li> argv[3] = threshold for tscal
 * </lu>
 */
int main(int argc, char *argv[]) {

  Sgi::hash_map<unsigned int, DcuCcuCalibration *> hashMapDcuCcuCalibration ;
  std::vector<DcuCcuCalibration *> vectorDcuCalib ;

  unsigned int dcuMaxRequest = 10 ;
  double ts_error = 10 ;

  if (argc >= 2) {
    std::cout << "Store the results in file " << argv[1] << std::endl ;
    if (argc >= 3) {
      dcuMaxRequest = fromString<unsigned int>(argv[2]) ;
      if (argc >= 4) {
	ts_error = fromString<double>(argv[3]) ;
      }
    }
  }

  std::cout << "Retreive " << dcuMaxRequest << " DCUs from construction database starting with a threshold on TS error = " << ts_error << std::endl ;
  std::cout << "Press <enter> to continue" ; getchar() ;

  // ---------------------------------- TIB, TEC
  parseTECDCUCCU ( dcuMaxRequest, vectorDcuCalib, hashMapDcuCcuCalibration ) ;
  
  // ---------------------------------- TOB
  parseTOBDCUCCU ( dcuMaxRequest, vectorDcuCalib, hashMapDcuCcuCalibration ) ;

  // -----------------------------------------------------------------------------------------------------------------------------
  // Output
  if (vectorDcuCalib.size()) {

    std::cout << "Found " << vectorDcuCalib.size() << " DCUs on CCUs" << std::endl ;

    std::ofstream *outputStream = NULL ; 
    if (argc >= 2) {
      outputStream = new std::ofstream (argv[1]) ;
      if (!outputStream->is_open()) {
	std::cerr << "Error cannot open file " << argv[1] << ": display on console" << std::endl ;
      }
    }

    if (outputStream == NULL) {
      std::cerr << "Sub detector" << "\t\t" << "CCU ID" << "\t\t" << "Structure ID" << "\t\t"
		<< "DCU Hard Id" << "\t\t" << "DCU Revert hard Id" << "\t\t" 
		<< "ADCGain0" << "\t\t" << "ADCOffset0" << "\t\t" << "ADC0Cal" << "\t\t" 
		<< "ADCGain2" << "\t\t" << "ADCOffset2" << "\t\t" << "ADC2Cal" << "\t\t" 
		<< "ADCGain3" << "\t\t" << "ADCOffset3" << "\t\t" << "ADC3Cal" << "\t\t" 
		<< "I20" << "\t\t" << "I10" << "\t\t" << "ICal" << "\t\t" 
		<< "R68" << "\t\t" << "R68Cal" << "\t\t" 
		<< "TSGain" << "\t\t" << "TSOffset" << "\t\t" << "TSCal" ;

      std::cerr << std::endl ;
    }
    else {
      *outputStream << "Sub detector" << "\t" << "CCU ID" << "\t" << "Structure ID" << "\t"
		    << "DCU Hard Id" << "\t" << "DCU Revert hard Id" << "\t" 
		    << "ADCGain0" << "\t" << "ADCOffset0" << "\t" << "ADC0Cal" << "\t" 
		    << "ADCGain2" << "\t" << "ADCOffset2" << "\t" << "ADC2Cal" << "\t" 
		    << "ADCGain3" << "\t" << "ADCOffset3" << "\t" << "ADC3Cal" << "\t" 
		    << "I20" << "\t" << "I10" << "\t" << "ICal" << "\t" 
		    << "R68" << "\t" << "R68Cal" << "\t" 
		    << "TSGain" << "\t" << "TSOffset" << "\t" << "TSCal" ;

      *outputStream << std::endl ;
    }

    //delete the vector of values
    for (vector<DcuCcuCalibration *>::iterator itr = vectorDcuCalib.begin() ; itr != vectorDcuCalib.end() ; itr ++) {

      try {
	// Calculate the values for the conversion parameters
	(*itr)->evaluateConversionValues( ts_error ) ; // default parameters are used for evaluation
      
	if (outputStream == NULL) {
	  
	  std::cerr << (*itr)->getSubDetector() << "\t\t" ;
	  std::cerr << (*itr)->getCcuId() << "\t\t" ;
	  std::cerr << (*itr)->getStructureId() << "\t\t" ;
	  std::cerr << (*itr)->getDcuHardId() << "\t\t" ;
	  std::cerr << (*itr)->getDcuHardRevertId() << "\t\t" ;
	  std::cerr << (*itr)->getAdcGain0() << "\t\t" ;
	  std::cerr << (*itr)->getAdcOffset0() << "\t\t" ;
	  std::cerr << (*itr)->getAdc0Cal() << "\t\t" ;
	  std::cerr << (*itr)->getAdcGain2() << "\t\t" ;
	  std::cerr << (*itr)->getAdcOffset2() << "\t\t" ;
	  std::cerr << (*itr)->getAdc2Cal() << "\t\t" ;
	  std::cerr << (*itr)->getAdcGain3() << "\t\t" ;
	  std::cerr << (*itr)->getAdcOffset3() << "\t\t" ;
	  std::cerr << (*itr)->getAdc3Cal() << "\t\t" ;
	  std::cerr << (*itr)->getI20() << "\t\t" ;
	  std::cerr << (*itr)->getI10() << "\t\t" ;
	  std::cerr << (*itr)->getICal() << "\t\t" ;
	  std::cerr << (*itr)->getR68() << "\t\t" ;
	  std::cerr << (*itr)->getR68Cal() << "\t\t" ;
	  std::cerr << (*itr)->getTsGain() << "\t\t" ;
	  std::cerr << (*itr)->getTsOffset() << "\t\t" ;
	  std::cerr << (*itr)->getTsCal() << "\t\t" ;

	  std::cerr << std::endl ;
	}
	else {

	  *outputStream << (*itr)->getSubDetector() << "\t" ;
	  *outputStream << (*itr)->getCcuId() << "\t" ;
	  *outputStream << (*itr)->getStructureId() << "\t" ;
	  *outputStream << (*itr)->getDcuHardId() << "\t" ;
	  *outputStream << (*itr)->getDcuHardRevertId() << "\t" ;
	  *outputStream << (*itr)->getAdcGain0() << "\t" ;
	  *outputStream << (*itr)->getAdcOffset0() << "\t" ;
	  *outputStream << (*itr)->getAdc0Cal() << "\t" ;
	  *outputStream << (*itr)->getAdcGain2() << "\t" ;
	  *outputStream << (*itr)->getAdcOffset2() << "\t" ;
	  *outputStream << (*itr)->getAdc2Cal() << "\t" ;
	  *outputStream << (*itr)->getAdcGain3() << "\t" ;
	  *outputStream << (*itr)->getAdcOffset3() << "\t" ;
	  *outputStream << (*itr)->getAdc3Cal() << "\t" ;
	  *outputStream << (*itr)->getI20() << "\t" ;
	  *outputStream << (*itr)->getI10() << "\t" ;
	  *outputStream << (*itr)->getICal() << "\t" ;
	  *outputStream << (*itr)->getR68() << "\t" ;
	  *outputStream << (*itr)->getR68Cal() << "\t" ;
	  *outputStream << (*itr)->getTsGain() << "\t" ;
	  *outputStream << (*itr)->getTsOffset() << "\t" ;
	  *outputStream << (*itr)->getTsCal() << "\t" ;
	  
	  *outputStream << std::endl ;
	}
      }
      catch (std::string e) {

	std::cerr << "Problem in the evaluation: " << e << std::endl ;
      }

      delete *itr ;
    }

    if (outputStream != NULL) {
      outputStream->flush();
      outputStream->close();
      delete outputStream ;
    }
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
std::string sendRequest ( std::string request ) {

    char buffer[MAXCHAR] ;
    std::string answer ;

#ifdef DEBUGMSGERROR
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
      std::cout << "Buffer = " << buffer << std::endl ;
#endif

      if (n < 0) 
	error("ERROR reading from socket");

      // Concat the buffer to the answer
      if (answer.size()!=0) answer += buffer;
      else answer = buffer;

#ifdef DEBUGMSGERROR
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

