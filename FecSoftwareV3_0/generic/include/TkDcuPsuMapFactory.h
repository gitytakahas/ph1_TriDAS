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

  Copyright 2002 - 2007, Guillaume BAULIEU - IPNL/IN2P3
*/

#ifndef TKDCUPSUMAPFACTORY_H
#define TKDCUPSUMAPFACTORY_H

#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif

#include <string>
#include <algorithm> 

#include "hashMapDefinition.h"
#include "TkDcuPsuMap.h"

#ifdef DATABASE
#include "DbTkDcuPsuMapAccess.h"
#endif

#include "DeviceFactoryInterface.h"

/** This class manage all the DCU-PSU mapping
 * This class provide a support for database (must be compiled with DATABASE flag) or files
 * For database, you can retreive information with the following parameters:
 * <ul><li>DCU Hard ID
 * </ul>
 * The mapping is stored in a map. If you need to reload the parameters and if you are using a file, you just need to call the addFileName or setFileName. With the database, or you delete the Factory and you recreate it or you set it in the getTkDcuPsuMap a forceReload
 */
class TkDcuPsuMapFactory: public DeviceFactoryInterface {

 private:

  /** Vector of dcu-psu for control group
   */
  tkDcuPsuMapVector vCGDcuPsuMap_ ;

  /** Vector of dcu-psu for power group
   */
  tkDcuPsuMapVector vPGDcuPsuMap_ ;

  /** Vector of dcu-psu for control group
   */
  tkDcuPsuMapVector vCGPsuNotConnected_ ;

  /** Vector of dcu-psu for power group
   */
  tkDcuPsuMapVector vPGPsuNotConnected_ ;

 public:

  /** \brief Build a FEC factory and if database is set create a database access
   */
  TkDcuPsuMapFactory ( ) ;
  
#ifdef DATABASE

  /** \brief Build a FEC factory for database
   */
  TkDcuPsuMapFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) ;

  /** \brief Buid a FEC factory for FEC database
   */
  TkDcuPsuMapFactory ( DbTkDcuPsuMapAccess *dbAccess ) ;

#endif

  /** \brief Disconnect the database (if it is set)
   */  
  ~TkDcuPsuMapFactory ( ) ;

#ifdef DATABASE
  /** \brief retreive information concerning the partition versus the version depending of the parameters passed 
   */
  void getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *partitionNumber ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#endif

  /** \brief return the Dcu-Psu mapping for control group
   */
  inline tkDcuPsuMapVector getControlGroupDcuPsuMaps ( ) { return vCGDcuPsuMap_ ; }

  /** \brief return the Dcu-Psu mapping for power group
   */
  inline tkDcuPsuMapVector getPowerGroupDcuPsuMaps ( ) { return vPGDcuPsuMap_ ; }

  /** \brief return the PSU not connected (after a run of PSU/DCU map)
   */
  inline tkDcuPsuMapVector getControlGroupPsuNotConnected ( ) { return vCGPsuNotConnected_ ; }

  /** \brief return the PSU not connected (after a run of PSU/DCU map)
   */
  inline tkDcuPsuMapVector getPowerGroupPsuNotConnected ( ) { return vPGPsuNotConnected_ ; }

  /** \brief return all DCU PSU map (merge of the two vectors)
   */
  inline tkDcuPsuMapVector getAllTkDcuPsuMaps ( ) {
    tkDcuPsuMapVector v ;
    for (tkDcuPsuMapVector::iterator it = vCGDcuPsuMap_.begin() ; it != vCGDcuPsuMap_.end() ; it ++) v.push_back(*it) ;
    for (tkDcuPsuMapVector::iterator it = vPGDcuPsuMap_.begin() ; it != vPGDcuPsuMap_.end() ; it ++) v.push_back(*it) ;
    return v;
  }

  /** \brief return all PSU not connected (merge of the two vectors of not connected one)
   */
  inline tkDcuPsuMapVector getAllTkPsuNotConnected ( ) {
    tkDcuPsuMapVector v ;
    for (tkDcuPsuMapVector::iterator it = vCGPsuNotConnected_.begin() ; it != vCGPsuNotConnected_.end() ; it ++) v.push_back(*it) ;
    for (tkDcuPsuMapVector::iterator it = vPGPsuNotConnected_.begin() ; it != vPGPsuNotConnected_.end() ; it ++) v.push_back(*it) ;
    return v;
  }


  // ------------------------------------------------------------------------------------------------------
  // 
  // XML file methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Add a new file name in the descriptions
   */
  void addFileName ( std::string fileName ) ;

  /** \brief set a new input file
   */
  void setInputFileName ( std::string inputFileName ) ;

  /** \brief parse a text file to produce the datapoints and pvss name
   */
  void setInputTextFile ( std::string inputFileName ) throw (FecExceptionHandler) ;

  /** \brief Produce a file for PVSS
   */
  void setOutputPVSSFile ( tkDcuPsuMapVector dcuPsuMapResult, std::string outputFileName, std::string errorFile ) ;

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
  void setDatabaseAccess ( DbTkDcuPsuMapAccess *dbAccess ) ;

  /** \brief Retreive the data from the database
   */
  void getDcuPsuMapPartition ( std::string partitionName, unsigned int majorVersionId = 0, unsigned int minorVersionId = 0 ) throw (FecExceptionHandler);

  /** \brief Retreive the data from the database
   */
  void getPsuNotConnectedPartition ( std::string partitionName, unsigned int majorVersionId = 0, unsigned int minorVersionId = 0 ) throw (FecExceptionHandler);

  /** \brief Retreive the data from the database
   */
  void getPsuNamePartition ( std::string partitionName, unsigned int majorVersionId = 0, unsigned int minorVersionId = 0 ) throw (FecExceptionHandler);

#endif

  /** \brief Upload the description in the output
   */
  void setTkDcuPsuMap ( tkDcuPsuMapVector dcuPsuMap, std::string partitionName = "", unsigned int *versionMajorId = NULL, unsigned int *versionMinorId = NULL ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Upload only the psu names in the output for the current version
   */
  void setTkPsuNames ( tkDcuPsuMapVector dcuPsuMap, std::string partitionName = "", unsigned int *versionMajorId = NULL, unsigned int *versionMinorId = NULL ) 
    throw ( FecExceptionHandler ) ;

  // --------------------------------------------------------------------------------------------------------
  //
  // Static methods
  //
  // --------------------------------------------------------------------------------------------------------

#ifdef DATABASE
  /** \brief This method calls trhe validation of the cooling loops
   */
  bool checkTKCCCoolingLoop ( std::string partitionName, std::vector<std::pair<std::string, bool> > &coolingLoopResult ) throw ( FecExceptionHandler, oracle::occi::SQLException, std::string ) ;
#endif

  /** delete the contents of a vector of PSU map
   */
  static void deleteVectorI ( tkDcuPsuMapVector &dcuPsuMap ) {
    for (tkDcuPsuMapVector::iterator it = dcuPsuMap.begin() ; it != dcuPsuMap.end() ; it ++) {
      delete (*it) ;
    }
    dcuPsuMap.clear() ;
  }

} ;

#endif
