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

#ifndef DEVICEFACTORY_H
#define DEVICEFACTORY_H

#include <string>

#include "FecFactory.h"
#include "Fed9UDeviceFactoryLib.hh" 
#include "ConnectionFactory.h"
#include "TkDcuConversionFactory.h"
#include "TkDcuInfoFactory.h"
#include "TkDcuPsuMapFactory.h"
#include "TkIdVsHostnameFactory.h"
#include "CommissioningAnalysisFactory.h"
#include "TkMaskModulesFactory.h"


/** This class manage all FED and FEC descriptions:
 *      FEC device description (APV, PLL, MUX, AOH, DOH, ...)
 *      FED device description
 *      FED / FEC APV description
 * It is mandatory the XML schema merge all the XML description already defined for the FEC and the FED
 */
class DeviceFactory: public FecFactory, 
 public Fed9U::Fed9UDeviceFactory,
 public ConnectionFactory,
 public TkDcuConversionFactory,
 public TkDcuInfoFactory,
 public TkDcuPsuMapFactory,
 public TkIdVsHostnameFactory,
 public CommissioningAnalysisFactory,
 public TkMaskModulesFactory
{

 private:
  /** run number file
   */
  std::string runNumberFile_ ;

 public:

  /** \brief Build a Device Factory
   */
  DeviceFactory ( ) ;

#ifdef DATABASE
  /** \brief Buid a FEC factory for database
   */
  DeviceFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) 
    ;
#endif

  /** \brief Disconnect the database (if it is set)
   */  
  ~DeviceFactory ( ) ;

  /** \brief set the file for the current run number
   */
  void setRunNumberFile ( std::string fileName ) ;

  /** \brief Retreive from the database the next run number
   */
  unsigned int getNextRunNumber ( )  
    throw (FecExceptionHandler) ;

  /** \brief Retreive from the database the current run number
   */
  unsigned int getCurrentRunNumber ( )  
    throw (FecExceptionHandler) ;

  /** \brief Set this file as the new output
   */
  void setOutputFileName ( std::string outputFileName ) ;

  /** \brief Set a file as the new input
   */
  void setFecInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name and parse it to retreive the information needed
   */
  void addFecFileName ( std::string fileName ) throw ( FecExceptionHandler ) ;

  /** \brief Set a file as the new input
   */
  void setFedInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name and parse it to retreive the information needed
   */
  void addFedFileName ( std::string fileName ) throw ( FecExceptionHandler ) ;

  /** \brief Set a file as the new input
   */
  void setFedFecConnectionInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name and parse it to retreive the information needed
   */
  void addFedFecConnectionFileName ( std::string fileName ) throw ( FecExceptionHandler ) ;

  /** \brief Set a file as the new input
   */
  void setConnectionInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name and parse it to retreive the information needed
   */
  void addConnectionFileName ( std::string fileName ) throw ( FecExceptionHandler ) ;

  /** \brief Set a file as the new input
   */
  void setDcuConversionInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name and parse it to retreive the information needed
   */
  void addDcuConversionFileName ( std::string fileName ) throw ( FecExceptionHandler ) ;

  /** \brief Set a file as the new input
   */
  void setTkDcuInfoInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name and parse it to retreive the information needed
   */
  void addTkDcuInfoFileName ( std::string fileName ) throw ( FecExceptionHandler ) ;

  /** \brief Set a file as the new input
   */
  void setTkDcuPsuMapFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name and parse it to retreive the information needed
   */
  void addTkDcuPsuMapFileName ( std::string fileName ) throw ( FecExceptionHandler ) ;

  /** \brief Set a file as the new input
   */
  void setTkIdVsHostnameInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) ;

  /** \brief Add a new file name and parse it to retreive the information needed
   */
  void addTkIdVsHostnameFileName ( std::string fileName ) throw ( FecExceptionHandler ) ;

  /** \brief Use file
   */
  void setUsingFile ( bool useIt = true ) ;

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE
  /** \brief use database
   */
  void setUsingDb ( bool useIt = true ) throw (std::string) ;

  /** \brief All databases used for FEC, FED, PIA and connection
   */
  bool getDbUsed ( ) ;

  /** \brief All databases connected for FEC, FED, PIA and connection
   */
  bool getDbConnected ( ) ;

  /** \brief create the database access for default environement variable
   */
  void setDatabaseAccess ( ) throw ( oracle::occi::SQLException ) ;

  /** \brief create database access
   */
  void setDatabaseAccess ( std::string login, std::string password, std::string path ) throw ( oracle::occi::SQLException ) ;

  /** \brief return the partition name
   */
  std::string getPartitionName ( unsigned int partitionId )  throw (oracle::occi::SQLException) ;

  /** \brief Refresh the cache of the connection, fec and fed
   */
  void refreshCacheXMLClob ( int deleteValues = 1, bool withStrips = true ) throw (oracle::occi::SQLException) ;

  /** \brief disable a set of DCus
   */
  void setEnableModules(std::string partitionName, std::vector<unsigned int> dcuHardIdList, bool enabled) 
    throw (FecExceptionHandler, oracle::occi::SQLException, Fed9U::Fed9UDeviceFactoryException, ICUtils::ICException) ;

  /** \brief download the connections and identify the det id 
   */
  unsigned int getConnectionDescriptionDetId ( std::string partitionName, ConnectionVector &connectionVector, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0, unsigned int maskVersionMajor = 0, unsigned int maskVersionMinor = 0, bool allConnections = false, bool forceDbReload = false ) throw (FecExceptionHandler, oracle::occi::SQLException) ;

  /** \brief return the error message coming from oracle exception
   */
  std::string what ( std::string message, oracle::occi::SQLException &ex ) ;

  /** \brief insert an o2o operation
   */
  void setO2OOperation ( std::string partitionName, std::string subDetector, unsigned int runNumber ) throw (FecExceptionHandler) ;

  /** \brief crosscheck the last o2o operation
   */
  bool getO2OXchecked ( std::string partitionName ) throw (FecExceptionHandler) ;

  /** \brief return a database access from any factory class (first found)
   * \return dbAccess - database access (NULL if not found)
   */
  DbCommonAccess *getAnyDbAccess ( ) {

    DbCommonAccess *dbAccess = NULL ;
    if (FecFactory::getDatabaseAccess() != NULL) dbAccess = FecFactory::getDatabaseAccess() ;
    else if (Fed9U::Fed9UDeviceFactory::getDatabaseAccess() != NULL) dbAccess = Fed9U::Fed9UDeviceFactory::getDatabaseAccess() ;
    else if (ConnectionFactory::getDatabaseAccess() != NULL) dbAccess = ConnectionFactory::getDatabaseAccess() ;
    else if (TkDcuConversionFactory::getDatabaseAccess() != NULL) dbAccess = TkDcuConversionFactory::getDatabaseAccess() ;
    else if (TkDcuInfoFactory::getDatabaseAccess() != NULL) dbAccess = TkDcuInfoFactory::getDatabaseAccess() ;
    else if (TkDcuPsuMapFactory::getDatabaseAccess() != NULL) dbAccess = TkDcuPsuMapFactory::getDatabaseAccess() ;
    else if (TkIdVsHostnameFactory::getDatabaseAccess() != NULL) dbAccess = TkIdVsHostnameFactory::getDatabaseAccess() ;
    else if (CommissioningAnalysisFactory::getDatabaseAccess() != NULL) dbAccess = CommissioningAnalysisFactory::getDatabaseAccess() ;
    
    return dbAccess ;
  }
#endif

} ;

#endif
