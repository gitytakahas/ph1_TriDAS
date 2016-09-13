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

#ifndef TKDCUCONVERSIONFACTORY_H
#define TKDCUCONVERSIONFACTORY_H

#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif

#include <string>

#include "hashMapDefinition.h"
#include "TkDcuConversionFactors.h"

#ifdef DATABASE
#include "DbTkDcuConversionAccess.h"
#endif

#include "DeviceFactoryInterface.h"

/** This class manage all the DCU conversion factors
 * This class provide a support for database (must be compiled with DATABASE flag) or files
 * For database, you can retreive information with the following parameters:
 * <ul><li>DCU Hard ID
 * </ul>
 * The conversion factors are stored in a map. If you need to reload the parameters and if you are using a file, you just need to call the addFileName or setFileName. With the database, or you delete the Factory and you recreate it or you set in the getTkDcuConversionFactors a forceReload
 */
class TkDcuConversionFactory: public DeviceFactoryInterface {

 private:

  /** Vector of conversion factors
   */
  Sgi::hash_map<unsigned long, TkDcuConversionFactors *> vConversionFactors_ ;

 public:

  /** \brief Build a FEC factory and if database is set create a database access
   */
  TkDcuConversionFactory ( ) ;
  
#ifdef DATABASE

  /** \brief Buid a FEC factory for database
   */
  TkDcuConversionFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) ;

  /** \brief Buid a FEC factory for FEC database
   */
  TkDcuConversionFactory ( DbTkDcuConversionAccess *dbAccess ) ;

#endif

  /** \brief Disconnect the database (if it is set)
   */  
  ~TkDcuConversionFactory ( ) ;

  /** \brief delete the hash_map
   */
  void deleteHashMapTkDcuConversionFactors ( ) ;

  /** \brief return the conversion factors
   */
  inline Sgi::hash_map<unsigned long, TkDcuConversionFactors *> getConversionFactors ( ) {

    return vConversionFactors_ ;
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
  void setDatabaseAccess ( DbTkDcuConversionAccess *dbAccess ) ;

#endif

  // ------------------------------------------------------------------------------------------------------
  // 
  // Conversion Factors download and upload
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Retreive the descriptions for the given devices from the input
   */
  TkDcuConversionFactors *getTkDcuConversionFactors ( unsigned long dcuHardId, bool forceDbReload = false ) throw ( FecExceptionHandler ) ;

#ifdef DATABASE
  /** \brief Add the descriptions of the devices from the partitionName
   */
  void addConversionPartition ( std::string partitionName )  throw (FecExceptionHandler);
#endif
  /** \brief Retreive the descriptions for the given devices from the input
   */
  //Sgi::hash_map<unsigned long, TkDcuConversionFactors *> getTkDcuConversionFactors ( std::string partitionName, bool forceDbReload = false ) throw ( FecExceptionHandler ) ;

  /** \brief Upload new conversion factors for a given DCU
   */
  void setTkDcuConversionFactors ( TkDcuConversionFactors tkDcuConversionFactors ) throw ( FecExceptionHandler ) ;

  /** \brief Upload the description in the output
   */
  void setTkDcuConversionFactors ( dcuConversionVector vConversionFactors ) 
    throw ( FecExceptionHandler ) ;

  /** \brief upload an hash_map in the output
   */
  void setTkDcuConversionFactors ( Sgi::hash_map<unsigned long, TkDcuConversionFactors *> vConversionFactors ) 
    throw ( FecExceptionHandler ) ;

  /** \brief upload the hash_map (attribut of the class) in the output
   */
  void setTkDcuConversionFactors ( ) 
    throw ( FecExceptionHandler ) ;

  /** \brief delete a vector of DCU conversions
   */
  static void deleteVectorI ( dcuConversionVector &dVector ) {

    for (dcuConversionVector::iterator it = dVector.begin() ; it != dVector.end() ; it ++) {
      delete *it ;
    }
    dVector.clear() ;
  }
} ;

#endif
