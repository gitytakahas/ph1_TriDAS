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

#ifndef FECFACTORY_H
#define FECFACTORY_H

#include <string>

// To retreive the IP address of the current machine
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "deviceType.h"
#include "FecDeviceFactory.h"
#include "PiaResetFactory.h"

#include "pllDescription.h"
#include "laserdriverDescription.h"
#include "dcuDescription.h"
#include "philipsDescription.h"
#include "muxDescription.h"
#include "apvDescription.h"
#include "deltaDescription.h"
#include "paceDescription.h"
#include "kchipDescription.h"
#include "gohDescription.h"


/** This class manage all FEC descriptions and PIA through the class FecDeviceFactory and PiaResetFactory
 */
class FecFactory: public FecDeviceFactory, public PiaResetFactory {

  //private:

 public:

  /** \brief Build a FEC factory both for pia and devices and create a database access if needed
   */
  FecFactory ( ) ;

#ifdef DATABASE
  /** \brief Buid a FEC factory for database
   */
  FecFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) ;
#endif

  /** \brief Disconnect the database (if it is set)
   */  
  virtual ~FecFactory ( ) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // XML file methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief set the output file both for PIA and FEC
   */
  void setOutputFileName ( std::string outputFileName ) ;

  /** \brief return the output file name
   */
  std::string getOutputFileName ( ) ;

  /** \brief set a new input file
   */
  void setInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief set a new input file
   */
  void setInputFileName ( std::string inputFileName, std::string fecHardwareId ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name in the descriptions
   */
  void addFileName ( std::string fileName ) throw (FecExceptionHandler) ;

  /** \brief Add a new file name in the descriptions
   */
  void addFileName ( std::string fileName, std::string fecHardwareId ) throw (FecExceptionHandler) ;

  /** \brief Use or not the file
   */
  void setUsingFile ( bool useIt = true ) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // Database methods
  //
  // ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

  /** \brief Use or not the database access
   */
  void setUsingDb ( bool useIt = true ) throw ( std::string ) ;

  /** \brief Check if the database is used
   */
  bool getDbUsed ( ) ;

  /** \brief Check if the database is connected
   */
  bool getDbConnected ( ) ;

  /** \brief Set a database access
   */
  void setDatabaseAccess ( ) 
    throw ( oracle::occi::SQLException ) ;

  /** \brief Set a database access
   */
  void setDatabaseAccess ( std::string login, std::string password, std::string path ) 
    throw ( oracle::occi::SQLException ) ;

  /** \brief return the FEC database access
   */
  DbFecAccess *getDatabaseAccess ( ) ;

  /** \brief return the partition name
   */
  std::string getPartitionName ( unsigned int partitionId )  throw (oracle::occi::SQLException) ;

#endif

  // ------------------------------------------------------------------------------------------------------
  // 
  // PIA and FEC device upload methods in same file or database
  //
  // ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

  /** \brief retreive information concerning the partition versus the version for the FEC devices 
   */
  void getFecDevicePartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *maskMajor, unsigned int *maskMinor, unsigned int *partitionNumber ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;

  /** \brief set version for the partition retreive from database with the couple 
   */
  void setFecDevicePartitionVersion ( std::string partitionName, unsigned int versionMajor, unsigned int versionMinor ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;

  /** \brief Upload a device description vector and a PIA description vector in the output and create new partition
   */
  void createPartition ( deviceVector devices, piaResetVector piadevices, unsigned int *deviceMajor = NULL, unsigned int *deviceMinor = NULL, std::string partitionName="", bool createPartitionFlag = true ) 
    throw (FecExceptionHandler) ;

  /** \brief In order to give the FecFactory to call the method FecDeviceFactory::createPartition ( ... ) ;
   */
  void createPartition ( deviceVector devices, unsigned int *versionMajor = NULL, unsigned int *versionMinor = NULL, std::string partitionName="", bool createPartitionFlag = true ) 
    throw (FecExceptionHandler) ;

  /** \brief In order to give the FecFactory to call the method PiaResetFactory::createPartition ( ... ) ;
   */
  void createPartition ( piaResetVector devices, std::string partitionName="" ) throw (FecExceptionHandler) ;

#endif

  /** \brief Upload in file or database a device description vector and a PIA description vector in the output
   */
  void setFecDevicePiaDescriptions ( deviceVector devices, piaResetVector piaDevices, std::string partitionName, unsigned int *deviceMajor = NULL, unsigned int *deviceMinor = NULL ) 
    throw (FecExceptionHandler) ;

  /** \brief Upload in file or database a device description vector and a PIA description vector in the output
   */
  void setFecDevicePiaDescriptions ( deviceVector devices, piaResetVector piaDevices ) throw (FecExceptionHandler) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // Static methods
  //
  // ------------------------------------------------------------------------------------------------------

 /** retreive the given devices from the given vector of devices
  * \param vDevices - vector of device descriptions
  * \param deviceType - device type to be retreived
  * \return vector of the current device
  * \warning the descriptions are not cloned
  */
 static deviceVector getDeviceFromDeviceVector ( deviceVector vDevices, enumDeviceType deviceType ) {

   deviceVector givenDeviceVector ;

   // sort it
   for (deviceVector::iterator device = vDevices.begin() ; (device != vDevices.end()) ; device ++) {
     deviceDescription *deviced = *device ;

     if (deviced->getDeviceType() == deviceType) givenDeviceVector.push_back (deviced) ;
   }

   /* -------------------------------------------------------------------
    * No more needed
    if (deviced->getDeviceType() == deviceType) {
    switch (deviceType) {
    case PLL: {
    pllDescription *pllD = dynamic_cast<pllDescription *>(deviced) ; //dynamic_cast<pllDescription *>deviced ;
    givenDeviceVector.push_back(pllD) ;
    break ;
    }
    case DOH:
    case LASERDRIVER: {
    laserdriverDescription *laserd = dynamic_cast<laserdriverDescription *>(deviced) ;
    givenDeviceVector.push_back(laserd) ;
    break ;
    }
    case DCU: {
    dcuDescription *dcud = dynamic_cast<dcuDescription *>(deviced) ;
    givenDeviceVector.push_back(dcud) ;
    break ;
    }
    case PHILIPS: {
    philipsDescription *philipsd = dynamic_cast<philipsDescription *>(deviced) ;
    givenDeviceVector.push_back(philipsd) ;
    break ;
    }
    case APVMUX: {
    muxDescription *muxd = dynamic_cast<muxDescription *>(deviced) ;
    givenDeviceVector.push_back(muxd) ;
    break ;
    }
    case APV25: {
    apvDescription *apvd = dynamic_cast<apvDescription *>(deviced) ;
    givenDeviceVector.push_back(apvd) ;
    break ;
    }
    case DELTA: {
    deltaDescription *deltad = dynamic_cast<deltaDescription *>(deviced) ;
    givenDeviceVector.push_back(deltad) ;
    break ;
    }
    case PACE: {
    paceDescription *paced = dynamic_cast<paceDescription *>(deviced) ;
    givenDeviceVector.push_back(paced) ;
    break ;
    }
    case KCHIP: {
    kchipDescription *kchipd = dynamic_cast<kchipDescription *>(deviced) ;
    givenDeviceVector.push_back(kchipd) ;
    break ;
    }
    case GOH: {
    gohDescription *gohd = dynamic_cast<gohDescription *>(deviced) ;
    givenDeviceVector.push_back(gohd) ;
    break ;
    }
    default:
    givenDeviceVector.push_back(deviced) ;
    }
    }
    }
    ------------------------------------------------------------------- */

   return givenDeviceVector ;
 }


 /** Delete a deviceVector
  * \param vDevices - vector of device descriptions
  * \warning this method only delete all devices not the vector (just clear method is called)
  */
 static void deleteVectorI ( deviceVector &vDevices ) {

   if (! vDevices.empty() ) {
     for (deviceVector::iterator device = vDevices.begin() ; (device != vDevices.end()) ; device ++) {
       delete *device ;
     }
     
     vDevices.clear() ;
   }
 }

 /** Delete a PIA reset vector
  * \param vPiaReset - PIA reset vector
  * \warning this method only delete all devices not the vector (just clear method is called)
  */
 static void deleteVectorI ( piaResetVector &vPiaReset ) {

   if (! vPiaReset.empty()) {
     // Emptyied the list of description (FEC PIA resets)
     for (piaResetVector::iterator device = vPiaReset.begin() ; (device != vPiaReset.end()) ; device ++) 
       delete *device ;
     
     vPiaReset.clear() ;
   }
 }

 /** \brief display a vector of devices
  * \param vDevice - vector of deviceAccess
  */
 static void display ( deviceVector vDevice ) throw (FecExceptionHandler) {
    
   if (!vDevice.empty()) {

     // For each device => access it
     for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {
        
       deviceDescription *deviced = *device ;
       deviced->display() ;
     }
   }
 }
  
 /** \brief display a vector of devices
  * \param vDevice - vector of deviceAccess
  */
 static void display ( piaResetVector vDevice ) throw (FecExceptionHandler) {
    
   if (!vDevice.empty()) {

     // For each device => access it
     for (piaResetVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {
        
       piaResetDescription *deviced = *device ;
       deviced->display() ;
     }
   }
 }

 /** \brief copy the pointer from a vector to another one (please note that the descriptions are not cloned)
  */
 static deviceVector copy ( deviceVector vDevice, bool allDevices = false ) {

   deviceVector outVector ;

   for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {	  
     deviceDescription *deviced = *device ;
     if (allDevices || deviced->isEnabled()) outVector.push_back ( deviced ) ;
   }

   return (outVector) ;
 }

 /** \brief clone a vector of device
  * This method create a deviceVector and clone it
  * \warning the remote method must delete the vector created
  */
 static deviceVector *cloneI ( deviceVector vDevice, bool allDevices = false ) {

   deviceVector *vectorClone = NULL ;
   if (! vDevice.empty()) {

     vectorClone = new deviceVector () ;
     vectorCopyI(*vectorClone,vDevice,allDevices) ;
   }

   return (vectorClone) ;
 }

 /** \brief Copy a vector of device descriptions into another one with a clone of the descriptions
  * \warning the remote method must delete the vector created
  */
 static void vectorCopyI ( deviceVector &dst, deviceVector &src, bool allDevices = false ) {

   if (! src.empty()) {

     for (deviceVector::iterator device = src.begin() ; device != src.end() ; device ++) {

       deviceDescription *deviced = *device ;
	
       if (allDevices || deviced->isEnabled()) dst.push_back ( deviced->clone() ) ;
     }
   }
 }

 /** \brief Copy a vector of PIA reset descriptions into another one with a clone of the descriptions
  * \warning the remote method must delete the vector created
  */
 static void vectorCopyI ( piaResetVector &dst, piaResetVector &src, bool allDevices = false ) {

   if (! src.empty()) {

     for (piaResetVector::iterator piaI = src.begin() ; piaI != src.end() ; piaI ++) {

       piaResetDescription *piaReset = *piaI ;
	
       if (allDevices || piaReset->isEnabled()) dst.push_back ( piaReset->clone() ) ;
     }
   }
 }

 /** \brief Copy a vector of PIA reset descriptions into another one with a clone of the descriptions
  * \warning the remote method must delete the vector created
  */
 static piaResetVector copy ( piaResetVector vPiaReset, bool allDevices = false ) {

   piaResetVector outVector ;

   for (piaResetVector::iterator piaI = vPiaReset.begin() ; piaI != vPiaReset.end() ; piaI ++) {
     
     piaResetDescription *piaReset = *piaI ;
     
     if (allDevices || piaReset->isEnabled()) outVector.push_back ( piaReset ) ;
   }

   return outVector ;
 }

 /** \brief return the DCU description that is related to an index
  * \param fecHardwareId - FEC hardware ID
  * \param index - to find the DCU
  * \param vDevices - vector of devices
  * \return DCU description for the given index and given FEC hardware ID
  */
 static dcuDescription *getDcuFromIndex ( std::string fecHardwareId, keyType index, deviceVector vDevices ) {

   dcuDescription *output = NULL ;
   keyType index1 = index & ~MASKADDRESSKEY ;

   bool found = false ;
   for (deviceVector::iterator device = vDevices.begin() ; (device != vDevices.end()) && !found ; device ++) {

     deviceDescription *deviced = *device ;

     if ( (deviced->getDeviceType() == DCU) && 
	  (deviced->getFecHardwareId() == fecHardwareId) &&
	  ((deviced->getKey() & ~MASKADDRESSKEY) == index1) ) {
       output = dynamic_cast<dcuDescription *>(deviced) ;
       found = true ;
     }
   }
    
   return (output) ;
 }

 /** \brief return the DCU description that is related to another device
  * \param idevice - device where to find the DCU
  * \param vDevices - vector of devices
  * \return DCU description for the given index and given FEC hardware ID
  */
 static dcuDescription *getDcuFromIndex ( deviceDescription *idevice, deviceVector vDevices ) {

   return (getDcuFromIndex ( idevice->getFecHardwareId(), idevice->getKey(), vDevices) ) ;
 }
  
 /** \brief delete the corresponding ccuVector
  * \param vCcuVector - vector of CCUDescriptions
  * \warning this method only delete all devices not the vector (just clear method is called)
  */
 static void deleteVectorI ( ccuVector &vCcuVector ) {
   
   if (! vCcuVector.empty()) {
     for (ccuVector::iterator device = vCcuVector.begin() ; (device != vCcuVector.end()) ; device ++) 
       delete *device ;
     
     vCcuVector.clear() ;
   }
 }

 /** \brief delete the corresponding ccuVector
  * \param ringVector - vector of ring descriptions
  * \warning this method only delete all devices not the vector (just clear method is called)
  */
 static void deleteVectorI ( tkringVector &ringVector ) {
   
   if (! ringVector.empty()) {
     for (tkringVector::iterator device = ringVector.begin() ; (device != ringVector.end()) ; device ++) 
       delete *device ;
     
     ringVector.clear() ;
   }
 }
 
 /** \brief Copy a vector of CCU descriptions into another one with a clone of the descriptions
  * \warning the remote method must delete the vector created
  */
 static void vectorCopyI ( ccuVector &dst, ccuVector &src ) {

   if (! src.empty()) {

     for (ccuVector::iterator device = src.begin() ; device != src.end() ; device ++) {

       CCUDescription *deviced = *device ;
	
       dst.push_back ( new CCUDescription(*deviced) ) ;
     }
   }
 }

 /** \brief Copy a vector of PIA reset descriptions into another one with a clone of the descriptions
  * \warning the remote method must delete the vector created
  */
 static ccuVector copy ( ccuVector src ) {

   ccuVector outVector ;

   for (ccuVector::iterator it = src.begin() ; it != src.end() ; it ++) {
     
     CCUDescription *ccu = *it ;
     
     outVector.push_back ( ccu ) ;
   }

   return outVector ;
 }

 // ***********************************************************************************************
 // Serialisation of TkRingDescription
 // ***********************************************************************************************

 /** \brief this method write into a memory to serialise it
  */
 static void *writeTo ( std::vector<TkRingDescription *> &rings, unsigned int &sizeOfAllocation ) throw (FecExceptionHandler) ;
 
 /** \brief this method read from a memory (deserialisation)
  */
 static std::vector<TkRingDescription *> readFrom ( const void *memory ) throw (FecExceptionHandler) ;

} ;

#endif
