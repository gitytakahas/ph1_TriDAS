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

#ifndef TKIDVSHOSTNAMEFACTORY_H
#define TKIDVSHOSTNAMEFACTORY_H

#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif

#include <string>
#include <map>

#include "hashMapDefinition.h"
#include "TkIdVsHostnameDescription.h"
#include "deviceType.h"

#ifdef DATABASE
#include "DbTkIdVsHostnameAccess.h"
#endif

#include "DeviceFactoryInterface.h"

/** Definition of the type of hash_map
 */
typedef std::map<std::pair<std::string, unsigned int>, TkIdVsHostnameDescription *> HashMapTkIdVsHostnameType ;

/** This class manage the hostname versus the FED software ID and the sub-detector
 * This class provide a support for database (must be compiled with DATABASE flag) or files.
 * The conversion factors are stored in a map. If you need to reload the parameters and if you are using a file, you just need to call the addFileName or setFileName. With the database, or you delete the Factory and you recreate it or you set in the get methods a forceReload.
 */
class TkIdVsHostnameFactory: public DeviceFactoryInterface {

 private:

  /** Vector of conversion factors
   */
  HashMapTkIdVsHostnameType vIdVsHostname_ ;

  /** Version major to avoid redownload of parameters
   */
  unsigned int versionMajorId_ ;

  /** Version minor to avoid redownload of parameters
   */
  unsigned int versionMinorId_ ;

  /** Current hostname in database
   */
  std::string hostname_ ;

 public:

  /** \brief Build a TkIdVsHostname factory and if database is set create a database access
   */
  TkIdVsHostnameFactory ( ) ;
  
#ifdef DATABASE

  /** \brief Build a TkIdVsHostname factory for database
   */
  TkIdVsHostnameFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) ;

  /** \brief Build a TkIdVsHostname factory for database
   */
  TkIdVsHostnameFactory ( DbTkIdVsHostnameAccess *dbAccess ) ;

#endif

  /** \brief Disconnect the database (if it is set)
   */  
  ~TkIdVsHostnameFactory ( ) ;

  /** \brief delete the hash_map
   */
  void deleteHashMapTkIdVsHostname ( ) ;

  /** \brief create the hash_map used in this class from a vector
   */
  void generateHashMapFromVector ( TkIdVsHostnameVector &v ) ;

  /** \brief create a vector from the hash_map
   */
  TkIdVsHostnameVector generateVectorFromHashMap ( HashMapTkIdVsHostnameType &h, std::string hostname = "ALL" ) ;

  /** \brief return the conversion factors
   */
  inline HashMapTkIdVsHostnameType getAllTkIdVsHostnameDescriptions ( ) {

    return vIdVsHostname_ ;
  }

  // ------------------------------------------------------------------------------------------------------
  // 
  // XML file methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Add a new file name in the descriptions
   */
  void addFileName ( std::string fileName ) throw (FecExceptionHandler) ;

  /** \brief set a new input file
   */
  void setInputFileName ( std::string inputFileName ) throw (FecExceptionHandler) ;

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
  void setDatabaseAccess ( DbTkIdVsHostnameAccess *dbAccess ) ;

#endif

  // ------------------------------------------------------------------------------------------------------
  // 
  // Conversion Factors download and upload
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Retreive the descriptions for the given devices from the input
   */
  TkIdVsHostnameVector getAllTkIdVsHostname ( unsigned int versionMajorId = 0, unsigned int versionMinorId = 0, bool forceDbReload = false ) 
#ifdef DATABASE
    throw ( FecExceptionHandler, oracle::occi::SQLException ) ;
#else
    throw ( FecExceptionHandler ) ;
#endif

  /** \brief Retreive the descriptions for the given devices from the input
   */
  TkIdVsHostnameVector getAllTkIdFromHostname ( std::string hostname, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0, bool forceDbReload = false ) 
#ifdef DATABASE
    throw ( FecExceptionHandler, oracle::occi::SQLException ) ;
#else
    throw ( FecExceptionHandler ) ;
#endif

  /** \brief Retreive the descriptions for the given devices from the input
   */
  TkIdVsHostnameDescription *getAllTkIdFromHostnameSlot ( std::string hostname, unsigned int slot, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0, bool forceDbReload = false ) 
#ifdef DATABASE
    throw ( FecExceptionHandler, oracle::occi::SQLException ) ;
#else
    throw ( FecExceptionHandler ) ;
#endif

  /** \brief Retreive the descriptions for the given devices from the input
   */
  void getSubDetectorCrateNumberFromHostname ( std::string hostname, std::string &subDetector, unsigned int &crateNumber, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0, bool forceDbReload = false ) 
#ifdef DATABASE
    throw ( FecExceptionHandler, oracle::occi::SQLException ) ;
#else
    throw ( FecExceptionHandler ) ;
#endif

  /** \brief Upload a new version of TkIdVsHostnameDescriptions
   */
  void setTkIdVsHostnameDescription ( TkIdVsHostnameVector tkDcuConversionFactors, bool major = false) throw ( FecExceptionHandler ) ;

  /** \brief upload the hash_map (attribut of the class) in the output
   */
  void setTkIdVsHostnameDescription ( bool major = false ) throw ( FecExceptionHandler ) ;

  /** \brief return the crate ID
   */
  unsigned int getFedCrate ( unsigned int fedId ) throw ( FecExceptionHandler ) ;

  /** \brief return the crate slot
   */
  unsigned int getFedSlot ( unsigned int fedId ) throw ( FecExceptionHandler ) ;

  /** \brief delete a vector of TkIdVsHostnameDescription
   */
  static void deleteVectorI ( TkIdVsHostnameVector &dVector ) {

    for (TkIdVsHostnameVector::iterator it = dVector.begin() ; it != dVector.end() ; it ++) {
      delete *it ;
    }
    dVector.clear() ;
  }
} ;

#endif
