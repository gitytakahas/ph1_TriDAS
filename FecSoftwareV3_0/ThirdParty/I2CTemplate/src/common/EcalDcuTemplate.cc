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
  
  Copyright 2002 - 2003, James Lamb - CERN, Geneva, Switzerland
*/
#include <iostream>

#include "FecAccess.h"
#include "FecAccessManager.h"

/** Display the errors coming from FecAccessManager download and upload operation
 */
void displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList ) {

  if (errorList.size() == 0) return ;

  Sgi::hash_map<keyType, bool> errorDevice ; // in order to avoid multiple display of errors
  std::list<FecExceptionHandler *> deleteException ; // to delete the exception

  // simple message
  std::cout << message << ": found " << errorList.size() << " errors" << std::endl ;

  for (std::list<FecExceptionHandler *>::iterator p = errorList.begin() ; p != errorList.end() ; p ++) {

    FecExceptionHandler *e = *p ;

    if (e->getPositionGiven()) {

      if (!errorDevice[e->getHardPosition()]) {

	errorDevice[e->getHardPosition()] = true ;

	std::cerr << "******************************** ERROR *************************" << std::endl ;
	std::cerr << e->what() << std::endl ;
	std::cerr << "****************************************************************" << std::endl ;

	// Add to be deleted
	deleteException.push_back (e) ;	
      }
    }
    else {
      
      std::cerr << "******************************** ERROR *************************" << std::endl ;
      std::cerr << e->what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
      
      // Add to be deleted
      deleteException.push_back (e) ;	
    }
  }

  // Delete the exception (not redundant)
  for (std::list<FecExceptionHandler *>::iterator p = deleteException.begin() ; p != deleteException.end() ; p ++) {
    delete *p ;
  }
    
  // No more error
  errorList.clear() ;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       main function                                     */
/*                                                                         */
/* ----------------------------------------------------------------------- */

int main ( int argc, char *argv[] ) {

  try {
    // Create the hardware access
    int cnt = 0 ;
    FecAccess *fecAccess = FecAccess::createFecAccess ( argc, argv, &cnt, true) ;

    if (fecAccess == NULL) {
      std::cerr << "Unable to create the access to the hardware" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    // Access manager
    FecAccessManager *fecAccessManager = new FecAccessManager ( fecAccess ) ;

    std::cout << "Hardware access created" << std::endl ;

    std::list<keyType> *fecList = fecAccess->getFecList ( ) ;
    if ((fecList == NULL) || (fecList->size() == 0)) {
      std::cout << "No FEC detected" << std::endl ;
      delete fecAccessManager ;
      delete fecAccess ;
      exit (EXIT_FAILURE) ;
    }

    deviceVector dcuList ;

    // for each ring
    for (std::list<keyType>::iterator itFec = fecList->begin() ; itFec != fecList->end() ; itFec++ ) {
      try {
	// scan the CCU 
	std::list<keyType> *ccuList = fecAccess->getCcuList(*itFec) ;

	if ( (ccuList == NULL) || (ccuList->size() == 0) ) {
	  std::cout << "Did not find CCU on FEC " << (int)getFecKey(*itFec) << "." << (int)getRingKey(*itFec) << std::endl ;
	}
	else {

#ifdef TRACKERDCU
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x19
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x16
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x13
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x10
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x1a
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x17
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x14
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x11
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x1b
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x18
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x15
	  // Found a module on FEC 6 Ring 0 CCU 0x2 channel 0x12

	  std::cout << "Creation DCU on FEC " << 0x6 << "." << 0x0 << " CCU 0x" << std::hex << 0x2 << std::dec << std::endl ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x19,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x16,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x13,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x10,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x1a,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x17,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x14,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x11,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x1b,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x18,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x15,0x0))) ;
	  dcuList.push_back(new dcuDescription(buildCompleteKey(0x6,0x0,0x2,0x12,0x0))) ;
#else
	  // for each CCU, create the DCU following the table
	  for (std::list<keyType>::iterator itCcu = ccuList->begin() ; itCcu != ccuList->end() ; itCcu++ ) {


	    //#define ECALDCU
	    if (getCcuKey(*itCcu) != 0x7F) {

	      // > > > > 1       0x18          0x10
	      dcuList.push_back(new dcuDescription(buildCompleteKey(getFecKey(*itCcu),getRingKey(*itCcu),getCcuKey(*itCcu),0x18,0x10))) ;
	      // > > > > 2       0x19          0x10
	      dcuList.push_back(new dcuDescription(buildCompleteKey(getFecKey(*itCcu),getRingKey(*itCcu),getCcuKey(*itCcu),0x19,0x10))) ;
	      // > > > > 3       0x1a          0x10
	      dcuList.push_back(new dcuDescription(buildCompleteKey(getFecKey(*itCcu),getRingKey(*itCcu),getCcuKey(*itCcu),0x1a,0x10))) ;
	      // > > > > 4       0x1b          0x10
	      dcuList.push_back(new dcuDescription(buildCompleteKey(getFecKey(*itCcu),getRingKey(*itCcu),getCcuKey(*itCcu),0x1b,0x10))) ;
	      // > > > > 5       0x1c          0x10
	      dcuList.push_back(new dcuDescription(buildCompleteKey(getFecKey(*itCcu),getRingKey(*itCcu),getCcuKey(*itCcu),0x1c,0x10))) ;
	      // > > > > 6       0x1e          0x10
	      dcuList.push_back(new dcuDescription(buildCompleteKey(getFecKey(*itCcu),getRingKey(*itCcu),getCcuKey(*itCcu),0x1e,0x10))) ;
	      // > > > > 7       0x1e          0x20
	      dcuList.push_back(new dcuDescription(buildCompleteKey(getFecKey(*itCcu),getRingKey(*itCcu),getCcuKey(*itCcu),0x1e,0x20))) ;
	      // > > > > 8       0x1e          0x40
	      dcuList.push_back(new dcuDescription(buildCompleteKey(getFecKey(*itCcu),getRingKey(*itCcu),getCcuKey(*itCcu),0x1e,0x40))) ;
	    }
	  }
	  
	  delete ccuList ;
#endif
	}
      }
      catch (FecExceptionHandler &e) {

	std::cerr << "*************** Warning *******************" << std::endl ;
	std::cerr << e.what() << std::endl ;
	std::cerr << "*******************************************" << std::endl ;
      }      
    }
    delete fecList ;
    try {
      std::cout << "Enabling channel" << std::endl ;
      std::list<FecExceptionHandler *> errorList ;
      fecAccessManager->downloadValuesMultipleFrames(&dcuList,errorList) ;
      displayErrorMessage("Errors during the declaration of the DCU", errorList) ;
    }
    catch (FecExceptionHandler &e) {

      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "*******************************************" << std::endl ;

      delete fecAccessManager ;
      delete fecAccess ;
      exit (EXIT_FAILURE) ;
    }

    std::cout << "Readout " << dcuList.size() << " DCUs in total for ever" << std::endl ;
    std::cout << "Press enter to start the readout" ; getchar() ;

    // Read out the DCU in multiple frames
    unsigned int loop = 1 ;
    for ( ; ; loop ++ ) {
      try {
	deviceVector dcuDevices ;
	std::list<FecExceptionHandler *> errorList ;
	fecAccessManager->uploadValuesMultipleFrames ( dcuDevices, errorList ) ;

	//std::cout << "Found " << errorList.size() << " errors" << std::endl ;
	std::cout << dcuDevices.size() << " DCU were readout" << std::endl ;
	displayErrorMessage("Errors during the declaration of the upload of the DCU", errorList) ;

	std::cout << std::dec << loop << " times" << std::endl ;
      }
      catch (FecExceptionHandler &e) {
	delete fecAccessManager ;
	delete fecAccess ;
	exit (EXIT_FAILURE) ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
      
    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  return 0 ;
}

    
