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

#ifndef FECDEVICEFACTORY_H
#define FECDEVICEFACTORY_H

#include <string>
#include <map>

#include "apvDescription.h"
#include "dcuDescription.h"
#include "laserdriverDescription.h"
#include "muxDescription.h"
#include "philipsDescription.h"
#include "piaResetDescription.h"
#include "pllDescription.h"

#ifdef PRESHOWER
#include "deltaDescription.h"
#include "kchipDescription.h"
#include "paceDescription.h"
#include "gohDescription.h"
#include "esMbResetDescription.h"
#endif

#ifdef TOTEM
#include "vfatDescription.h"
#include "totemCChipDescription.h"
#include "totemBBDescription.h"
#endif

#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif

#ifdef DATABASE
#include "DbFecAccess.h"
#endif

#include "DeviceFactoryInterface.h"
#include "TkRingDescription.h"

typedef struct {

  std::string partitionName ;
  unsigned int versionMajorId ;
  unsigned int versionMinorId ;
  deviceVector vDevices ;

} FecDeviceFactoryVersionValues ;

/** This class manage all FEC descriptions:
 *      FEC device description (APV, PLL, MUX, AOH, DOH, ...)
 * This class provide a support for database (must be compiled with DATABASE flag) or files
 * For database, you can retreive information with the following parameters:
 * <lu><li> FEC hardware ID: only available for the VME FEC and Partition name
 * <li> partition name: name of the partition (to retreive all information for a given partition)
 * \example: for each kind of download / upload you must specifed
 * For files, you only use the methods without any parameters, so you do not put the previous parameters
 * \Deprecated the methods setApvDescriptions, getApvDescriptions, ..., getPaceDescriptions, setPaceDescriptions is no longer exists (so pll, mux, laserdriver, doh, pace, kchip, delta) but not the DCU
 * To retreive the corresponding devices, you need to use:
 * <ul><li>getApvDescriptions:
 * deviceVector all = fecFactory.getFecDeviceDescriptions ( ..., versionMajor, versionMinor ) ; // version is optional
 * deviceVector apv = FecFactory::getDeviceFromDeviceVector ( all, APV ) ;
 * ...
 * Fectory::deleteVector (all) ; // do not delete the apv deviceVector
 * <li>setApvDescriptions
 * deviceVector all ; // that must contains all the device you want to upload
 * // You can of course filter it with the method FecFactory::getDeviceFromDeviceVector
 * fecFactory.setFecDeviceDescriptions ( all, <1>, unsigned int *versionMajor, unsigned int *versionMinor, bool majorVersion, bool uploadVersion ) ;
 * where <1> is one of the parameters: (FEC hardware ID) and/or (partition name )
 * and <2> is optional parameter for the database
 * </ul>
 */
class FecDeviceFactory: public DeviceFactoryInterface {

 private:

  /** Map between the FEC / partition / version
   */
  std::map<std::string, FecDeviceFactoryVersionValues > mapPartitionVersion ;

  /** FEC version
   */
  unsigned int versionMajor_, versionMinor_ ;

  /** Mask version
   */
  unsigned int maskVersionMajor_, maskVersionMinor_ ;

  /** partition name
   */
  std::string partitionName_ ;
  
  /** Version already initialised
   */
  bool initDbVersion_ ;

  /** FEC devices vector
   */
  deviceVector vFecDevices_ ;

  /** Ring description
   */
  tkringVector tkRingDescription_ ;

  /** \brief retreive the information from the input for one of the following parameters
   */
  void getFecDeviceDescriptions ( bool fileUsed, unsigned int versionMajor, unsigned int versionMinor, unsigned int pMaskVersionMajorId, unsigned int pMaskVersionMinorId, std::string partitionName, std::string fecHardwareId, deviceVector &outVector, bool allDevices = false, bool forceDbReload = false ) 
#ifdef DATABASE
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#else
    throw (FecExceptionHandler ) ;
#endif

  /** \brief return the DCU descriptions values
   */
  void getDcuValuesDescriptions ( std::string partitionName, unsigned long dcuHardId, unsigned long timeStampStart, unsigned long timeStampStop, deviceVector &outVector )
    throw (FecExceptionHandler ) ;

#ifdef DATABASE
  /** \brief retreive information concerning the partition versus the version depending of the parameters passed 
   */
  std::list<unsigned int *> getPartitionVersion ( std::string partitionName ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#endif

 public:

  /** \brief Build a FEC factory and a database access
   */
  FecDeviceFactory ( ) ;

#ifdef DATABASE
  
  /** \brief Buid a FEC factory for database
   */
  FecDeviceFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) ;
  
  /** \brief Buid a FEC factory for FEC database
   */
  FecDeviceFactory ( DbFecAccess *dbFecAccess )  ;

  /** \brief uploads the tkRing description to the datavase
    */
  void setDbRingDescription ( std::string partitionName, TkRingDescription ringD )  throw ( FecExceptionHandler, oracle::occi::SQLException ) ;

#endif

  /** \brief Disconnect the database (if it is set)
   */  
  virtual ~FecDeviceFactory ( ) ;

  /** \brief return the vector in the memory
   */
  inline deviceVector getFecDevices ( ) { return vFecDevices_ ; } 

  /** \brief return the ring descriptions in the memory
   */
  inline tkringVector getTkRingDescriptions ( ) { return tkRingDescription_ ; } 


  // ------------------------------------------------------------------------------------------------------
  // 
  // XML file methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Add a new file name in the descriptions
   */
  void addFileName ( std::string fileName ) ;

  /** \brief Add a new file name in the descriptions
   */
  void addFileName ( std::string fileName, std::string fecHardwareId ) ;

  /** \brief set a new input file
   */
  void setInputFileName ( std::string inputFileName ) ;

  /** \brief set a new input file
   */
  void setInputFileName ( std::string inputFileName, std::string fecHardwareId ) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // Database methods
  //
  // ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

  /** \brief Create the database access
   */
  void setDatabaseAccess ( std::string login, std::string password, std::string path ) 
    throw ( oracle::occi::SQLException ) ;
  
  /** \brief Create the database access with the configuration given by the env. variable CONFDB
   */
  void setDatabaseAccess ( ) 
    throw ( oracle::occi::SQLException ) ;

  /** \brief Set the database access
   */
  void setDatabaseAccess ( DbFecAccess *dbFecAccess ) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // Partition / version methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief retreive information concerning the partition versus the version depending of the parameters passed 
   */
  void getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *maskVersionMajor, unsigned int *maskVersionMinor, unsigned int *partitionNumber ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;

  /** \brief set version for the partition retreive from database with partition name
   */
  void setPartitionVersion ( std::string partitionName, unsigned int versionMajor, unsigned int versionMinor ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;

  /** \brief Upload the description in the output and create new partition
   */
  void createPartition ( deviceVector devices, unsigned int *versionMajor = NULL, unsigned int *versionMinor = NULL, std::string partitionName="", bool createPartitionFlag = true )  
    throw (FecExceptionHandler) ;

#endif

  // ------------------------------------------------------------------------------------------------------
  // 
  // Correction on FEC devices
  // 
  // ------------------------------------------------------------------------------------------------------

  /** \brief Apply correction on FEC devices
   */
  std::string setDeviceCorrection ( deviceVector &devices, std::string subDetector ) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // Tracker FEC devices methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Retreive the descriptions for all devices from the input for partition name, FEC hardware ID
   */ 
  void getFecDeviceDescriptions ( std::string partitionName, std::string fecHardwareId, deviceVector &outVector, unsigned int versionMajor = 0, unsigned int versionMinor = 0, bool allDevices = false, bool forceDbReload = false )
#ifdef DATABASE
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#else
    throw (FecExceptionHandler ) ;
#endif

  /** \brief Retreive the descriptions for all devices from the input for a given partition
   */ 
    void getFecDeviceDescriptions ( std::string partitionName, deviceVector &outVector, unsigned int versionMajor = 0, unsigned int versionMinor = 0, unsigned int pMaskVersionMajorId = 0, unsigned int pMaskVersionMinorId = 0, bool allDevices = false, bool forceDbReload = false )
#ifdef DATABASE
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#else
    throw (FecExceptionHandler ) ;
#endif

  /** \brief Retreive the descriptions for all devices from a file
   */
  void getFecDeviceDescriptions ( deviceVector &outVector, bool allDevices = false )
#ifdef DATABASE
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#else
    throw (FecExceptionHandler ) ;
#endif

  /** \brief Upload the description in the output
   */
  void setFecDeviceDescriptions ( deviceVector devices, std::string partitionName = "nil", unsigned int *versionMajor = NULL, unsigned int *versionMinor = NULL , bool majorVersion = false, bool uploadVersion = false ) 
#ifdef DATABASE
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#else
    throw (FecExceptionHandler ) ;
#endif

  /** \brief Retreive the DCU values for a given partition and timestamp
   */
  void getDcuValuesDescriptions ( std::string partitionName, deviceVector &outVector, unsigned long timeStampStart, unsigned long timeStampStop )   
    throw ( FecExceptionHandler ) ;

   /** \brief Retreive the DCU values for a given timestamp
   */
  void getDcuValuesDescriptions ( deviceVector &outVector, unsigned long timeStampStart, unsigned long timeStampStop )
    throw ( FecExceptionHandler ) ;

  /** \brief Retreive the descriptions for a given DCU hard id for a range of time
   */
  void getDcuValuesDescriptions ( unsigned long dcuHardId, deviceVector &outVector, unsigned long timeStampStart, unsigned long timeStampStop )
    throw ( FecExceptionHandler ) ;

  /** \brief Upload the DCU descriptions to the output
   */
  void setDcuValuesDescriptions ( std::string partitionName, deviceVector dcuDevices, bool fileUpload = false ) 
    throw ( FecExceptionHandler ) ;

  // ----------------------------- CCU descriptions

#ifdef DATABASE
  /** \brief Retreive the tkRing description along with the corresponding CCU descriptions for a given partition name
   */
  tkringVector getDbRingDescription ( std::string partitionName ) throw ( FecExceptionHandler, oracle::occi::SQLException ) ;

  /** \brief Retreive the tkRing description along with the corresponding CCU descriptions for a given partition name and given FEC hardware ID/ring
   */
  TkRingDescription* getDbRingDescription ( std::string partitionName, std::string fecHardwareId, unsigned int ring ) throw ( FecExceptionHandler, oracle::occi::SQLException ) ;

  /** \brief Retreive the tkRing description along with the corresponding CCU descriptions for a given FEC hardware ID/ring from the construction database
   */
  TkRingDescription* getDbRingDescription ( std::string fecHardwareId, unsigned int ring ) throw ( FecExceptionHandler, oracle::occi::SQLException ) ;
#endif

  /** \brief Retreive the CCU descriptions from a file
   */
  TkRingDescription* getFileRingDescription ( std::string fileName, std::string fecHardwareId, unsigned int ring ) throw ( FecExceptionHandler ) ;
  
  /** \brief Retreive the CCU descriptions from a file
   */
  tkringVector getFileRingDescription ( std::string fileName ) throw ( FecExceptionHandler ) ;
  
  /** \brief Upload the tkring descriptions to a file
   */
  void setFileRingDescription ( std::string fileName, TkRingDescription ringD ) 
    throw ( FecExceptionHandler ) ;
} ;

#endif
