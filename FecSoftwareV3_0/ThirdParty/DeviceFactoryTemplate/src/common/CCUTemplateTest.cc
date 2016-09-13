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
#include "FecFactory.h"
#include "FecDeviceFactory.h"
#include "TkRingDescription.h"

/** Display the redundancy of a vector of CCU descriptions in the correct order
 * \param vCcuVector - vector of CCU descriptions
 */
void displayRedundancy ( std::vector<CCUDescription *> vCcuVector ) {

  int i, j;
  // sort vCcuVector
  int sizeVector = 0;
  sizeVector = vCcuVector.size();
  for(i=0;i<sizeVector;i++){
    CCUDescription *ccu1 = vCcuVector[i];
    for(j=i;j<sizeVector;j++){            
      if(ccu1->getOrder() > vCcuVector[j]->getOrder()){
	CCUDescription *ccu2 = vCcuVector[j];
	vCcuVector[j] = vCcuVector[i];
	vCcuVector[i] = ccu2;
	ccu1 = ccu2;
      }
    }
  }
  // Display vCcuVector
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "------ Redundancy of a vector of CCU descriptions in the correct order ------" << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::string input, output;
  int inout=0;
  
  for (ccuVector::iterator device = vCcuVector.begin() ; (device != vCcuVector.end()) ; device ++) {
    CCUDescription *ccuD = *device ;
   
    if (ccuD->getEnabled()) std::cout << "Enabled" << std::endl; 
    if (! (ccuD->getEnabled())) std::cout << "Disabled" << std::endl; 
    std::cerr << "FecHardwareId='" << ccuD->getFecHardwareId() << "'" << std::endl;
  }

  for (i=0; i<sizeVector; i++) {
    std::cout << "   ----   ";
    std::cout << "     ";
  }
  std::cout << "   " << std::endl;
  
  i = 1;

  for (ccuVector::iterator device = vCcuVector.begin() ; (device != vCcuVector.end()) ; device ++) {
    //    CCUDescription *ccuD = *device ;
    
    //    switch (ccuD->getInput()){
    switch (inout){
    case 0: input = "A"; 
      break;
    case 1: input = "B";
      break;
    default: input = "";
    }

    //    switch (ccuD->getOutput()){
    switch (inout){
    case 0: output = "A"; 
      break;
    case 1: output = "B";
      break;
    default: output = "";
    }
   
    if (input == "A") {     
      if (output == "A"){
	std::cout << "A |    | A";	
	if (i<sizeVector){
	  std::cout << " --- ";
	}
      }
      else {
	std::cout << "A |    |  ";
	std::cout << "     ";
      }
    } else {
      if (input == "B")
	if (output == "A"){
	  std::cout << "  |    | A";	
	  if (i<sizeVector){
	    std::cout << " --- ";
	  }
	}
	else {
	  std::cout << "  |    |  ";
	  std::cout << "     "; 
	}
    }
    
    i++;
  
  }
  std::cout << "" << std::endl;

  i = 1;
  for (ccuVector::iterator device = vCcuVector.begin() ; (device != vCcuVector.end()) ; device ++) {
    //    CCUDescription *ccuD = *device ;
    
    //    switch (ccuD->getInput()){
    switch(inout) {
    case 0: input = "A"; 
      break;
    case 1: input = "B";
      break;
    default: input = "";
    }

    //    switch (ccuD->getOutput()){
    switch(inout) {
    case 0: output = "A"; 
      break;
    case 1: output = "B";
      break;
    default: output = "";
    }
   
    if (input == "A")           
      if (output == "B"){
	std::cout << "  |    | B";	
	if (i<sizeVector){
	  std::cout << " --- ";
	}
      } else { 
	std::cout << "  |    |  ";
	std::cout << "     ";
      }
    else
      if (input == "B")
	if (output == "B"){
	  std::cout << "B |    | B";
	  if (i<sizeVector){
	    std::cout << " --- ";
	  }
	} else { 
	  std::cout << "B |    |  ";    
	  std::cout << "     ";
	}
  }
  
  std::cout << "" << std::endl;

  for (i=0; i<sizeVector; i++) {
    std::cout << "   ----   ";
    std::cout << "     ";
  }
  std::cout << "" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "" << std::endl;
  
}

/** Take the information from the input and upload to the output
 * \param deviceFactory - database or file access
 * \param inputFile - file or database input
 * \param outputFile - file or database output
 * \param fecHardwareId - FEC hardware ID
 * \param partitionName - partition name (for database)
 * command: -download
 */
void downloadCCUDescriptions(FecDeviceFactory &deviceFactory, std::string inputFile, std::string outputFile, std::string fecHardwareId, unsigned int ring, std::string partitionName) {
  
  tkringVector allRings ;
  TkRingDescription* myRing = NULL;
  try {
    // Retreive the CCU descriptions
    if (inputFile == "database") {
      if (fecHardwareId == "")
	allRings = deviceFactory.getDbRingDescription(partitionName);
      else
	myRing = deviceFactory.getDbRingDescription(partitionName,fecHardwareId,ring);

      if (allRings.size()) myRing = *(allRings.begin()) ;
    }
    else {
      myRing = deviceFactory.getFileRingDescription(inputFile, fecHardwareId, ring);
    }

    if (! myRing) {
      std::cout << "Sorry no ring matching your request" << std::endl;
      return;
    }

    if (fecHardwareId == "") {

      std::cout << "Got " << allRings.size() << " rings for " << partitionName << std::endl ;
      for (tkringVector::iterator device = allRings.begin() ; (device != allRings.end()) ; device ++) {
	myRing = *device ;
	std::cout << "I got one ring with " << myRing->getCcuVector()->size() << " CCUs for the partition " << partitionName << " for the FEC " << myRing->getFecHardwareId() << " ring " << myRing->getRingSlot() << std::endl ;

	(*device)->computeRedundancy() ;
	if ((*device)->isReconfigurable()) std::cout << (*device)->getReconfigurationProblem() << std::endl ;
	else std::cout << "Ring is ok" << std::endl ;
      }
    }
    else {
      std::cout << "I got one ring with " << myRing->getCcuVector()->size() << " CCUs for the partition " << partitionName << " for the FEC " << fecHardwareId << " ring " << ring << std::endl ;

      myRing->computeRedundancy() ;
      if (myRing->isReconfigurable()) std::cout << myRing->getReconfigurationProblem() << std::endl ;
      else std::cout << "Ring is ok" << std::endl ;
      
      if (myRing->getNumberOfCcus() > 0) {
	//       // Display it
	//       for (ccuVector::iterator device = vCcuVector.begin() ; (device != vCcuVector.end()) ; device ++) {
	// 	CCUDescription *ccuD = *device ;
	// 	ccuD->displayRedundancy() ;
	//       }
	//       // Display the real redundancy
	//       displayRedundancy (vCcuVector) ; 
	
	if (outputFile != "") {
	  
	  std::cout << "I will upload now the description in " << outputFile << std::endl ;
	  
	  try {
	    // Upload it
	    // TODO: first try to change some values in the ring
	    if (outputFile == "database") {
	      deviceFactory.setDbRingDescription (partitionName, *myRing);
	    }
	    else {
	      deviceFactory.setFileRingDescription (outputFile, *myRing);
	    }
	  }
	  catch (FecExceptionHandler &e) {
	    std::cerr << "Error during the upload of the CCU descriptions: " << std::endl ;
	    std::cerr << e.what() << std::endl ;
	  }
	  catch (oracle::occi::SQLException &e) {
	    //std::cerr << "Error during the upload of the CCU descriptions (oracle problem): " << std::endl ;
	    std::cerr << deviceFactory.what("Error during the upload of the CCU descriptions (oracle problem)", e) << std::endl ;
	  }
	}  
      }
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "Error during the download of the CCU descriptions: " << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Error during the download of the CCU descriptions (oracle problem): " << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
}


/** Take the information from the input and upload to the output
 * \param deviceFactory - database or file access
 * \param inputFile - file or database input
 * \param outputFile - file or database output
 * \param fecHardwareId - FEC hardware ID
 * \param partitionName - partition name (for database)
 * command: -downloadTkcc
 */
void downloadCCUDescriptions(FecDeviceFactory &deviceFactory, std::string inputFile, std::string outputFile, std::string fecHardwareId, unsigned int ring) {
  
  TkRingDescription* myRing = NULL;
  try {
    // Retreive the CCU descriptions
    if (inputFile == "database") {
      myRing = deviceFactory.getDbRingDescription(fecHardwareId,ring);
    }
    else {
      myRing = deviceFactory.getFileRingDescription(inputFile, fecHardwareId, ring);
    }

    if (! myRing) {
      std::cout << "Sorry no ring matching your request" << std::endl;
      return;
    }

    std::cout << "I got one ring with " << myRing->getCcuVector()->size() << " CCUs for the partition for FEC " << fecHardwareId << " ring " << ring << std::endl ;
    myRing->computeRedundancy() ;
    if (!myRing->isReconfigurable()) std::cout << myRing->getReconfigurationProblem() << std::endl ;
    else { myRing->display(std::cout,false,true) ; std::cout << std::endl ; }
      
    if (myRing->getNumberOfCcus() > 0) {
      //       // Display it
      //       for (ccuVector::iterator device = vCcuVector.begin() ; (device != vCcuVector.end()) ; device ++) {
      // 	CCUDescription *ccuD = *device ;
      // 	ccuD->displayRedundancy() ;
      //       }
      //       // Display the real redundancy
      //       displayRedundancy (vCcuVector) ; 
      
//       if (outputFile != "") {
	  
// 	std::cout << "I will upload now the description in " << outputFile << std::endl ;
	
// 	try {
// 	  // Upload it
// 	  // TODO: first try to change some values in the ring
// 	  if (outputFile == "database") {
// 	    deviceFactory.setDbRingDescription (partitionName, *myRing);
// 	  }
// 	  else {
// 	    deviceFactory.setFileRingDescription (outputFile, *myRing);
// 	  }
// 	}
// 	catch (FecExceptionHandler &e) {
// 	  std::cerr << "Error during the upload of the CCU descriptions: " << std::endl ;
// 	  std::cerr << e.what() << std::endl ;
// 	}
// 	catch (oracle::occi::SQLException &e) {
// 	  //std::cerr << "Error during the upload of the CCU descriptions (oracle problem): " << std::endl ;
// 	  std::cerr << deviceFactory.what("Error during the upload of the CCU descriptions (oracle problem)", e) << std::endl ;
// 	}
//       }  
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "Error during the download of the CCU descriptions: " << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Error during the download of the CCU descriptions (oracle problem): " << std::endl ;
    std::cerr << e.what() << std::endl ;
  }
}

  

/* ************************************************************************************************ */
/*                                                                                                  */
/*                                               Main                                               */
/* Option                                                                                           */
/* -input = input file or database                                                                  */
/* -output = database or output file                                                                */
/*                                                                                                  */
/* Command                                                                                          */
/* -download: read input and put the conversions into the output                                    */
/*                                                                                                  */
/* bin/linux/x86/CCUTemplate.exe -input database -output ccu.xml -download                          */
/* bin/linux/x86/CCUTemplate.exe -input ccu.xml -output database -download                          */
/* bin/linux/x86/CCUTemplate.exe -input ccu.xml -download                                           */
/* bin/linux/x86/CCUTemplate.exe -input database -download                                          */
/* bin/linux/x86/CCUTemplate.exe -input database -fecHardwareId <> -ring <> -downloadTKCC           */
/* ************************************************************************************************ */
int main ( int argc, char **argv ) {

  FecDeviceFactory *deviceFactory = NULL ;

  // ***************************************************************************************************
  // Create the database access
  // retreive the connection of the database through an environmental variable called CONFDB

  std::string login="nil", passwd="nil", path="nil" ;
  DbAccess::getDbConfiguration (login, passwd, path) ;

  if (login != "nil" && passwd !="nil" && path != "nil") {


    try {
      // create the database access

      deviceFactory = new FecDeviceFactory ( login, passwd, path ) ; 
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else {

    deviceFactory = new FecDeviceFactory ( ) ;

  }

  // *************************************************************************************************
  // Check the options
  int command = 0 ;
  std::string inputFile = "" ;
  std::string outputFile = "" ;
  std::string partitionName = "" ;
  std::string fecHardwareId = "" ;
  unsigned int ring = 0 ;

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

    else if (!strncasecmp (argv[i], "-fecHardwareId", strlen("-fecHardwareId"))) {

      i ++ ;
      if (i < argc) fecHardwareId = argv[i] ;

      else {
	std::cerr << "Error in usage, bad values for the option partition, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp (argv[i], "-ring", strlen("-ring"))) {
      
      i ++ ;
      if (i < argc) ring = fromString<unsigned int>(argv[i]) ;
      else {
	std::cerr << "Error in usage, bad values for the option input, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp(argv[i], "-downloadTKCC", strlen("-downloadTKCC"))) command = 3 ;
    else if (!strncasecmp(argv[i], "-download", strlen("-download"))) command = 1 ;
    //    else if (!strncasecmp(argv[i], "-upload", strlen("-upload"))) command = 2 ;
    else if (!strncasecmp(argv[i], "-help", strlen("-help"))) command = 0 ;
    else if (!strncasecmp(argv[i], "--help", strlen("--help"))) command = 0 ;
    else std::cerr << "Invalid argument: " << argv[i] << std::endl ;
  }


  // *************************************************************************************************
  // Error in options or command
  //std::cout << command << " " << inputFile << " " << deviceFactory->getDbUsed() << " " << fecHardwareId << " " << ring << " " << partitionName << std::endl ;
  if ( (command == 0) ||
       ((command == 1) && (inputFile == "")) ||
       ((command == 1) && (inputFile == "database") && (partitionName == "")) ||
       ((command == 1) && (inputFile == "database") && (!deviceFactory->getDbUsed())) ||
       ((command == 1) && (outputFile == "database") && (!deviceFactory->getDbUsed()))||
       ((command == 2) && (outputFile == "")) ||
       ((command == 2) && (outputFile == "database") && (!deviceFactory->getDbUsed())) ||
       ((command == 3) && (inputFile != "database") && (!deviceFactory->getDbUsed())) 
       ) {
	
	// std::cerr << "Command        : " << command << std::endl;
	// std::cerr << "Partition name : " << partitionName << std::endl;
	// std::cerr << "inputFile      : " << inputFile << std::endl;
	// std::cerr << "outputFile     : " << outputFile << std::endl;
	// std::cerr << "getDbUsed()    : " << ( deviceFactory->getDbUsed() ? "True" : "False" ) << std::endl; 

    std::cerr << "Error in usage: wrong option or command" << std::endl ;
    std::cerr << "\t" << argv[0] << " -input <file name | database> [-output <file name | database>] -fecHardwareId <FEC hardware ID> [-partition <Partition Name>] [-download | -downloadTKCC]" << std::endl ;
    std::cerr << "\t\tDownload the CCU descriptions from the input and upload to the output if it is specified" << std::endl ;
    std::cerr << "\t\t\t" << "Input: input" << std::endl ;
    std::cerr << "\t\t\t" << "Output: output if you want to upload to the output" << std::endl ;
    std::cerr << "\t\t\t" << "FEC hardware ID: FEC hardware ID" << std::endl ;
    std::cerr << "\t\t\t" << "Partition name: partition name only if the input or the output is database" << std::endl ;

    delete deviceFactory ;

    exit (EXIT_FAILURE) ;
  }

  // *************************************************************************************************
 
//   //Static vector to test the upload
//   ccuVector* vCcuVector = new ccuVector(); 
   
//   parameterDescriptionNameType* parameterNames = CCUDescription::getParameterNames(); 
//   (*parameterNames)["arrangement"]->setValue("1");
//   (*parameterNames)["fecHardwareId"]->setValue("BertaEsther1");
//   (*parameterNames)["fecSlot"]->setValue("1");
//   (*parameterNames)["ringSlot"]->setValue("1");
//   (*parameterNames)["ccuAddress"]->setValue("555");
  
//   CCUDescription* ccu = new CCUDescription(*parameterNames);
//   vCcuVector->push_back(ccu);

  // ************************************************************************************************
   
  // Apply command
  switch (command) {
  case 1: // download
    downloadCCUDescriptions(*deviceFactory, inputFile, outputFile, fecHardwareId, ring, partitionName) ;
    break ;
  case 2: // upload
    //uploadCCUDescriptions(*deviceFactory, outputFile, partitionName, *vCcuVector) ;
    break ;
  case 3: // download from construction/TKCC database
    downloadCCUDescriptions(*deviceFactory, inputFile, outputFile, fecHardwareId, ring) ;
    break ;
  }

//  FecFactory::deleteVectorI (*vCcuVector) ; delete vCcuVector ;
//  CCUDescription::deleteParameterNames(parameterNames) ; delete parameterNames ;

  delete deviceFactory ;

  return 0 ;
}
