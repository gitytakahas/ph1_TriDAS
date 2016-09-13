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
#ifndef DEVICEFRAMETYPE_H
#define DEVICEFRAMETYPE_H

#include <list>

#include "keyType.h"
#include "FecExceptionHandler.h"

#include "hashMapDefinition.h"

// For multiple frames access for the i2c devices or PIA channels
// This block must be filled by each <device>Access with the following description:
//      - index: input
//      - i2cType: mode of i2c access (not filled for PIA)
//      - accessType: read or write
//      - offset: for i2c access (not filled for PIA)
//      - data: input if it is a write command, output if it is a read command
//      - sent: internal management in the multiple frame block method
//      - tnum: transaction number used, assigned by the the multiple frame block method
//      - dAck: 
//            * direct acknowledge, assigned by the the multiple frame block method
//            * force acknowledge if is asked, an exception is given (next field) if the force ack is not correct
//      - e: FecExceptionHandler, output, NULL if ok, != NULL if it is not ok
//
// A display method for such list exits in deviceAccess.h
// static void displayAccessDeviceType ( list<accessDeviceType> vAccessDevices  )
typedef struct {
  keyType             index      ;  // Index
  unsigned short      i2cType    ;  // RALMODE, NORMALMODE, EXTENDEDMODE
  enumAccessModeType  accessType ;  // MODE_READ, MODE_WRITE
  unsigned short      offset     ;  // For RAL, EXTENDED and for laserdriver (the data is calculated with offset and data)
  unsigned short      data       ;  // fill it is a write, fill by the next algorithm if it is a read
  bool                sent       ;  // false (not sent)
  unsigned short      tnum       ;  // transaction number (not filled at the beginning)
  unsigned short      dAck       ;  // 0 at the beginning: direct acknowledge value 
  unsigned short      fAck       ;  // 0 at the beginning: force acknowledge value only for i2c device
  FecExceptionHandler *e         ;  // In case of error

} accessDeviceType ;

// Definition of the map of list of accessDeviceType
typedef std::list<accessDeviceType> accessDeviceTypeList ;
typedef Sgi::hash_map<keyType, std::list<accessDeviceType> > accessDeviceTypeListMap ;
typedef Sgi::hash_map<unsigned short, accessDeviceType *> accessTransactionFrameMap ;

#include <iostream>
#include <sstream>

/** This class is just define to avoid warning in the compilation
 * Only two statics methods has been defined:
 * <ul>
 * <li> decodeErrorFrame: for decoding error of accessDeviceType structure
 * <li> displayAccessDeviceType: display of accessDeviceType structure
 * </ul>
 */
class DeviceFrame {

 public:
  
  /** \brief Decode the error for the multiple frame methods
   * This method take a list of accessDeviceType and display error message related to the errors in the list so the error return by the multiple frame methods
   * \param vAccesses - list of accessDeviceType that contains the errors (cf deviceFrame.h data structure).
   * \return number of errors
   * \warning this method display the message on the console if the process is not a XDAQ process or display it through LOG4C if it is a XDAQ process
   * \warning this method delete the exception that was dynamically created
   */
  static unsigned int decodeErrorFrame ( accessDeviceTypeList vAccesses ) {

    unsigned int error = 0 ;
    Sgi::hash_map<keyType, bool> errorDevice ; // in order to avoid multiple display of errors
    
    // decode the errors here
    for (std::list<accessDeviceType>::iterator itAccessDevice = vAccesses.begin() ; itAccessDevice != vAccesses.end() ; itAccessDevice ++) {
      
      // display the errors
      if ((itAccessDevice->e != NULL) && (!errorDevice[itAccessDevice->index])) {

	errorDevice[itAccessDevice->index] = true ;

	error ++ ;
	
	std::cerr << "*********** ERROR *************" << std::endl ; 
	std::cerr << itAccessDevice->e->what() << std::endl ; 
	std::cerr << "*******************************" << std::endl ;
	
	// delete the exceptions
	delete itAccessDevice->e ;
	itAccessDevice->e = NULL ;
      }
      else {
	char msg[80] ;
	decodeKey(msg, itAccessDevice->index) ;
	std::cout << "No error found for the device 0x" << std::hex << itAccessDevice->index << ": " << msg << std::endl ;
      }
    }
    
    return (error) ;
  }

  /** \brief Decode the error for the multiple frame methods
   * This method take a list of accessDeviceType and display error message related to the errors in the list so the error return by the multiple frame methods
   * \param vAccesses - list of accessDeviceType that contains the errors (cf deviceFrame.h data structure).
   * \param errorList - list of exceptions, please note the exceptions insert in the list must be deleted by the remote methods
   * \return number of errors
   * \warning this method display the message on the console if the process is not a XDAQ process or display it through LOG4C if it is a XDAQ process
   * \warning this method delete the exception that was dynamically created
   */
  static unsigned int decodeErrorFrame ( accessDeviceTypeList vAccesses, std::list<FecExceptionHandler *> &errorList, bool debugMessageDisplay = false ) {

    unsigned int error = 0 ;
    //Sgi::hash_map<keyType, bool> errorDevice ; // in order to avoid multiple display of errors
    
    // decode the errors here
    for (std::list<accessDeviceType>::iterator itAccessDevice = vAccesses.begin() ; itAccessDevice != vAccesses.end() ; itAccessDevice ++) {
      
      // display the errors
      if (itAccessDevice->e != NULL) { //&& (!errorDevice[itAccessDevice->index])) {

	//errorDevice[itAccessDevice->index] = true ;
	
	error ++ ;

	// Put the exceptions in the list
	errorList.push_back (itAccessDevice->e) ;
      }
      else if (debugMessageDisplay) {
	char msg[80] ;
	decodeKey(msg, itAccessDevice->index) ;
	std::ostringstream logDebug ; logDebug << "No error found for the device 0x" << std::hex << itAccessDevice->index << ": " << msg << std::endl ;
	std::cout << "DEBUG Information: " << logDebug.str() << std::endl ;
      }
    }
    
    return (error) ;
  }

  /** Display the values for one access
   */
  static void displayAccessDeviceType ( accessDeviceType deviceAccess ) {

    char msg[80] ;
    decodeKey (msg, deviceAccess.index) ;
    std::cout << msg << ": " << std::endl ;
    if (deviceAccess.accessType == MODE_READ)
      std::cout << "\tRead access in " ;
    else if (deviceAccess.accessType == MODE_WRITE)
      std::cout << "\tWrite access in " ;
    else std::cout << "\tAccess not recognize in " ;
      
    if (isi2cChannelCcu25(deviceAccess.index)) {
      switch (deviceAccess.i2cType) {
      case RALMODE: std::cout << "RAL mode for " ; break ;
      case NORMALMODE: std::cout << "Normal mode for " ; break ;
      case EXTENDEDMODE: std::cout << "Extended mode for " ; break ;
      default: std::cout << " Unknown mode (" << std::dec << (int)deviceAccess.i2cType << ") for " ;
      }
      
      std::cout << "offset " << std::dec << (int)deviceAccess.offset << std::endl ;
    }
    
    if (deviceAccess.accessType == MODE_WRITE) std::cout << "\tWrite data = 0x" << std::hex << (int)deviceAccess.data ;
    else if (deviceAccess.sent) std::cout << "\tData read = " << std::dec << (int)deviceAccess.data ;
    
    std::cout << std::endl ;
    if (deviceAccess.sent) {
      std::cout << " \tThis transaction has been already sent" << std::endl ;
      std::cout << " \t\tDirect acknowledge = 0x" << std::hex << (int)deviceAccess.dAck << std::endl ;
      if (deviceAccess.fAck != 0) 
	std::cout << " \t\tForce acknowledge = 0x" << std::hex << (int)deviceAccess.fAck << std::endl ;
      if (deviceAccess.e != NULL)
	std::cout << "\t\tError during the transaction: " << deviceAccess.e->what() << std::endl ;
      else {
	if (deviceAccess.accessType == MODE_READ)
	  std::cout << "\t\tData readout = " << std::dec << (int)deviceAccess.data << std::endl ;
	else
	  std::cout << "\tWrite data = 0x" << std::dec << (int)deviceAccess.data << std::endl ;
	std::cout << "\t\tTransaction successfully performed" << std::endl ;
      }
    }
    else 
      std::cout << "\tThis transaction has not been already performed" << std::endl ;
  }
  
  /** Display a list of transaction to be performed in multiple frame
   * \param vAccessDevices - list of transaction to be performed
   */
  static void displayAccessDeviceType ( accessDeviceTypeList vAccessDevices  ) {
    
    for (accessDeviceTypeList::iterator itAccessDevice = vAccessDevices.begin() ; itAccessDevice != vAccessDevices.end() ; itAccessDevice++) {
      
      std::cout << "----------------------------------------------------------------------------------" << std::endl ;
      displayAccessDeviceType ( *itAccessDevice ) ;
    }
  }
};

#endif
