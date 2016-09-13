/*
This file is part of Fec Software project.

author: Juha Petäjäjärvi
based on FecFactory by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include "TotemFecFactory.h"

/** Build a FecFactory to retreive information from a file
 * \exception Possible exceptions are:
 *     file not found
 *     bad XML format
 */
TotemFecFactory::TotemFecFactory ( ) : FecFactory( ) {}

/** Disconnect the database (if it is set) and delete the vector of each FecDeviceFactory and PiaResetFactory
 */  
TotemFecFactory::~TotemFecFactory ( ) {}


/** Retreive the descriptions for all devices from the XML file
 */
void TotemFecFactory::getFecDeviceDescriptions ( deviceVector &outVector, std::string filename ) throw (FecExceptionHandler ) {

  // Parse the input file

  // Devices to be deleted
  deviceVector deleteDevices ;

  // For FEC devices
  TotemXMLFecDevice xmlFecDevice ( filename ) ;

  // Retreive all the devices from the parsing class
  deviceVector vDevice = xmlFecDevice.getDevices ( ) ;

  // Merge the vector from the class and the new vector
  // vFecDevices_.merge (*vDevice) ;
  for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {
    
    bool equalS = false ;
    
    for (deviceVector::iterator deviceP = vFecDevices_.begin() ; (deviceP != vFecDevices_.end() && ! equalS ) ; deviceP ++) {
      if (*deviceP == *device) equalS= true ;
    }

    if (! equalS) 
      vFecDevices_.push_back (*device) ;
    else 
      deleteDevices.push_back(*device) ;
  }

  // All devices are deleted by the XMLFecDevice so nothing must be deleted
  // delete the devices not used
  TotemFecFactory::deleteVectorI(deleteDevices) ;

  // Copy the descriptions into the output for file
  outVector = TotemFecFactory::copy ( vFecDevices_, false ) ;
}

/** Upload the description in the output and upload it in a new version.
 * \warning Overwrites a function from FecDeviceFactory
 */
void TotemFecFactory::setFecDeviceDescriptions ( deviceVector devices, std::string filename ) throw (FecExceptionHandler ) {
  
  if (devices.empty()){ RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded ", ERRORCODE) ; }
    
  // Complete all the devices with the disabled device
  deviceVector toBeUploaded ;
  for (deviceVector::iterator itDev = devices.begin() ; itDev != devices.end() ; itDev ++) {
    toBeUploaded.push_back(*itDev) ;
  }
  for (deviceVector::iterator itVDev = vFecDevices_.begin() ; itVDev != vFecDevices_.end() ; itVDev ++) {
    bool toBeAdded = true ;
    for (deviceVector::iterator itDev = devices.begin() ; itDev != devices.end() ; itDev ++) {
      if ( ((*itVDev)->getKey()) == ((*itDev)->getKey()) ){ toBeAdded = false ; }
    }
    if (toBeAdded){ toBeUploaded.push_back(*itVDev) ; }
  }

  // Upload in file
  TotemXMLFecDevice xmlFecDevice ; 
  xmlFecDevice.setDevices (toBeUploaded, filename) ;
   
}
