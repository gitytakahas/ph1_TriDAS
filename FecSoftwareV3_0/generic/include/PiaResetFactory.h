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

#ifndef PIARESETFACTORY_H
#define PIARESETFACTORY_H

#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif

#include <string>

// To retreive the IP address of the current machine => FecDeviceFactory
#include "deviceType.h"
#include "piaResetDescription.h"

#ifdef DATABASE
#include "DbPiaResetAccess.h"
#endif

#include "DeviceFactoryInterface.h"

/** This class manage all FEC PIA reset descriptions:
 * This class provide a support for database (must be compiled with DATABASE flag) or files
 * For database, you can retreive information with the following parameters:
 * <ul><li> FEC hardware ID: only available for the VME FEC
 * <li> partition name: name of the partition (to retreive all information for a given partition)
 * \example: for each kind of download / upload you must specifed
 * For files, you only use the methods without any parameters, so you do not put the previous parameters
 * To retreive the corresponding devices, you need to use:
 */
class PiaResetFactory: public DeviceFactoryInterface {

 private:

  /** PIA reset vector
   */
  piaResetVector vPiaFecReset_ ;

  /** \brief retreive the information from the input for one of the following parameters
   */
  void getPiaResetDescriptions ( bool fileUsed, std::string partitionName, std::string fecHardwareId, piaResetVector &outVector, bool allDevices = false, bool forceDbReload = false ) 
    throw (FecExceptionHandler) ;

#ifdef DATABASE
  /** \brief retreive information concerning the partition versus the version
   */
  std::list<unsigned int *> getPartitionVersion ( std::string partitionName ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#endif

 public:

  /** \brief Build a FEC factory and if database is set create a database access
   */
  PiaResetFactory ( ) ;
  
#ifdef DATABASE

  /** \brief Buid a FEC factory for database
   */
  PiaResetFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) ;

  /** \brief Buid a FEC factory for FEC database
   */
  PiaResetFactory ( DbPiaResetAccess *dbPiaResetAccess ) ;

#endif

  /** \brief Disconnect the database (if it is set)
   */  
  ~PiaResetFactory ( ) ;

  /** \brief return the vector in the memory
   */
  inline piaResetVector getPiaResets ( ) { return vPiaFecReset_ ; } 

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
  void setDatabaseAccess ( DbPiaResetAccess *dbPiaResetAccess ) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // Partition / version methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Create new PIA reset descriptions
   */
  void createPartition ( piaResetVector devices, std::string partitionName = "" )
    throw ( FecExceptionHandler ) ;

#endif

  // ------------------------------------------------------------------------------------------------------
  // 
  // PIA reset methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Retreive the descriptions for the given devices from the input
   */
  void getPiaResetDescriptions ( std::string partitionName, std::string fecHardwareId, piaResetVector &outVector, bool allDevices = false, bool forceDbReload = false ) throw ( FecExceptionHandler ) ;

    /** \brief Retreive the descriptions for the given devices from the input
   */
  void getPiaResetDescriptions ( std::string partitionName, piaResetVector &outVector, bool allDevices = false, bool forceDbReload = false ) throw ( FecExceptionHandler ) ;

  /** \brief Retreive the descriptions for the given devices from a file
   */
  void getPiaResetDescriptions ( piaResetVector &outVector, bool allDevices = false ) throw ( FecExceptionHandler ) ;

  /** \brief Upload the description in the output
   */
  void setPiaResetDescriptions ( piaResetVector devices, std::string partitionName = "nil" ) 
    throw ( FecExceptionHandler ) ;
} ;

#endif
