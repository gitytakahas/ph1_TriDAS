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

#ifndef TKDCUINFOFACTORY_H
#define TKDCUINFOFACTORY_H

#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif

#include <string>
#include <algorithm> 
#include "hashMapDefinition.h"
#include "TkDcuInfo.h"

#include "TkState.h"

#ifdef DATABASE
#include "DbTkDcuInfoAccess.h"
#endif

#include "DeviceFactoryInterface.h"

/** This class manage all the DCU conversion factors
 * This class provide a support for database (must be compiled with DATABASE flag) or files
 * For database, you can retreive information with the following parameters:
 * <ul><li>DCU Hard ID
 * </ul>
 * The conversion factors are stored in a map. If you need to reload the parameters and if you are using a file, you just need to call the addFileName or setFileName. With the database, or you delete the Factory and you recreate it or you set in the getTkDcuInfo a forceReload
 */
class TkDcuInfoFactory: public DeviceFactoryInterface {

 private:

  /** Vector of conversion factors
   */
  Sgi::hash_map<unsigned long, TkDcuInfo *> vDcuInfo_ ;

  /** list of runs
   */
  tkRunVector runs_ ;

  /** list of states
   */
  tkStateVector dbStates_ ;

  /** list of FED versions
   */
  tkVersionVector fedVersions_ ;

  /** list of FEC versions
   */
  tkVersionVector fecVersions_ ;

  /** list of connection versions
   */
  tkVersionVector connectionVersions_ ;

  /** list of DCU info versions
   */
  tkVersionVector dcuInfoVersions_ ;

  /** list of DCU PSU versions
   */
  tkVersionVector dcuPsuVersions_ ;

  /** If the DET id was already downloaded (only for database)
   */
  bool initDbVersion_ ;

  /** Partition name already downloaded (only for database)
   */
  std::string partitionName_ ;

  /** Major version already downloaded (only for database)
   */
  unsigned int versionMajor_ ;

  /** Minor version already downloaded (only for database)
   */
  unsigned int versionMinor_ ;

  /** List of mask versions
   */
  tkVersionVector maskVersions_ ;

 public:

  /** \brief Build a FEC factory and if database is set create a database access
   */
  TkDcuInfoFactory ( ) ;
  
#ifdef DATABASE

  /** \brief Buid a FEC factory for database
   */
  TkDcuInfoFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) ;

  /** \brief Buid a FEC factory for FEC database
   */
  TkDcuInfoFactory ( DbTkDcuInfoAccess *dbAccess ) ;

#endif

  /** \brief Disconnect the database (if it is set)
   */  
  ~TkDcuInfoFactory ( ) ;

  /** \brief delete the hash_map
   */
  void deleteHashMapTkDcuInfo ( ) ;

  /** \brief return the Dcu infos
   */
  inline Sgi::hash_map<unsigned long, TkDcuInfo *> getInfos ( ) {

    return vDcuInfo_ ;
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
  void setDatabaseAccess ( DbTkDcuInfoAccess *dbAccess ) ;

  /** \brief retreive information concerning the partition versus the version depending of the parameters passed 
   */
  void getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *partitionNumber ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;

  /** \brief Add the DCU infos for the given partition (the data are extracted from the database)
   */
  void addDetIdPartition ( std::string partitionName, unsigned int majorVersionId = 0, unsigned int minorVersionId = 0, bool cleanCache = true, bool forceDbReload = false ) throw (FecExceptionHandler);

  /** \brief Add all the DCU infos that are in the database for the given version
   */
  void addAllDetId (unsigned int majorVersionId = 0, unsigned int minorVersionId = 0, bool cleanCache = true, bool forceDbReload = false) throw (FecExceptionHandler);

  /** \brief Update the channel delays (coarse and fine) according to the fibre length between detector and FED
   */
  void updateChannelDelays ( std::string partitionName ) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Get all the current states
   *  \return An array of TkState. YOU have to delete all TkState objects + the vector!
   */
  tkStateVector &getCurrentStates () throw (FecExceptionHandler);

  /** \brief Create a new current state
   *  \param states A list of state
   *  \return The new currentStateId
   */
  unsigned int setCurrentState(tkStateVector states) throw (FecExceptionHandler);

  /** \brief Set the given state as current
   *  \param stateName The name of the chosen state
   *  \return The new current state ID
   */
  unsigned int setCurrentState(std::string stateName) throw (FecExceptionHandler);

  /** \brief Create a new state, copy of the current one. Update the partition with its values in the given state
   *  \param stateName The name of the regerence state
   *  \param partitionName The name of the partition to update
   *  \return The new current state ID
   */
  unsigned int setCurrentState(std::string partitionName, std::string stateName) throw (FecExceptionHandler);

  /** \brief update the current state based on the state coming from a run number
   */
  unsigned int copyStateForRunNumber(unsigned int runNumber, bool allPartition) throw (FecExceptionHandler) ;

  /** \brief Disable the devices linked to the dcuHardId in the given partition. 
   *  The currentState must not be used in a Run or it will generate an exception (protection of the run history). 
   *
   *  \param partitionName The name of the chosen partition
   */  
  void disableDevice(std::string partitionName, std::vector<unsigned int> dcuHardId) throw (FecExceptionHandler);

  /** \brief Enable the devices linked to the dcuHardId in the given partition
   *  \param partitionName The name of the chosen partition
   */  
  void enableDevice(std::string partitionName, std::vector<unsigned int> dcuHardId) throw (FecExceptionHandler);

  /** \brief Get the last run for the given partition
   *  \param partitionName The partition used for the run
   *  \return A TkRun object containing the last run
   */
  TkRun* getLastRun (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Get the given run for the given partition
   *  \param partitionName The partition used for the run
   *  \param runNumber The number of the run
   *  \return A TkRun object containing the run
   */
  TkRun* getRun (std::string partitionName, int runNumber) throw (FecExceptionHandler);

  /** \brief Get all the runs from the DB
   *  \return A tkRunVector object containing the run. You have to delete this vector!
   */
  tkRunVector getAllRuns () throw (FecExceptionHandler);

  /** \brief Get the last run used by O2O for the given partition
   *  \param partitionName The partition used for the run
   *  \return A TkRun object containing the last run or NULL if no record was found. You have to delete the object!
   */
  TkRun* getLastO2ORun (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Create a new run
   *  \param partitionName The partition used for the run
   *  \param runNumber The number of the run
   *  \param runMode The mode of the run
   *  \param local 1 if local run, 0 otherwise
   */
  void setRun(std::string partitionName, unsigned int runNumber, int runMode, int local, std::string comment = "") throw (FecExceptionHandler);

   /** \brief Stop the current run on the partition
   *  \param partitionName The partition used for the run
   *  \param comment Modify the comment on the run if filled
   */
  void stopRun(std::string partitionName, std::string comment = "") throw (FecExceptionHandler);

#ifdef DATABASE
   /** \brief Create a new version for the dcu infos
   */
  void createNewDcuInfoVersion() throw (FecExceptionHandler);
#endif

  /** \brief Tag a run as transfered by O2O
   *  \param partitionName The partition used for the run
   *  \param runNumber The number of the run
   */
  void setO2ORun(std::string partitionName, unsigned int runNumber) throw (FecExceptionHandler);

  /** \brief return the database version
   */
  double getDbVersion ( ) throw (FecExceptionHandler) ;

  /** \brief return the database size
   */
  double getDbSize ( ) throw (FecExceptionHandler) ;

  /** \brief Update the comment of the run
   *  \param partitionName The partition used for the run
   *  \param runNumber The number of the run
   *  \param newComment The new comment
   */
  void updateRunComment(std::string partitionName, unsigned int runNumber, std::string newComment) throw (FecExceptionHandler);

  /** \brief Retrieve the partition names
   *  \return A list with all the partition names.
   */
  std::list<std::string> getAllPartitionNames() throw (FecExceptionHandler);

  /** \brief Retrieve the partition names for a given run
   *  \return A list with all the partition names.
   */
  std::list<std::string> getAllPartitionNames( unsigned int runNumber ) throw (FecExceptionHandler) ;

  /** \brief Retrieve the partition names from the current state
   *  \return A list with all the partition names.
   */
  std::list<std::string> getAllPartitionNamesFromCurrentState() throw (FecExceptionHandler);

  /** \brief Retrieve the state names
   *  \return A list with all the state names. YOU have to delete that list!
   */
  std::list<std::string> getAllStateHistoryNames() throw (FecExceptionHandler);

  /** \brief Get all the Fec versions
   *  \return An array of TkVersion. 
   */
  tkVersionVector &getAllFecVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Get all the Fed versions
   *  \return An array of TkVersion. 
   */
  tkVersionVector &getAllFedVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Get all the Dcu Info versions
   *  \return An array of TkVersion. 
   */
  tkVersionVector &getAllDcuInfoVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Get all the Connection versions
   *  \return An array of TkVersion. 
   */
  tkVersionVector &getAllConnectionVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Get all the DcuPsuMap versions
   *  \return An array of TkVersion. 
   */
  tkVersionVector &getAllDcuPsuMapVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Get all the mask versions
   *  \return An array of TkVersion. 
   */
  tkVersionVector &getAllMaskVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief get the state history for the current state
   */
  unsigned int getCurrentStateHistoryId() throw (FecExceptionHandler) ;

#endif

  // ------------------------------------------------------------------------------------------------------
  // 
  // Conversion Factors download and upload
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Retreive the descriptions for the given devices from the input
   */
  TkDcuInfo *getTkDcuInfo ( unsigned long dcuHardId ) throw ( FecExceptionHandler ) ;

  /** \brief Upload the description in the output
   */
  void setTkDcuInfo ( tkDcuInfoVector vInfo ) 
    throw ( FecExceptionHandler ) ;

  /** \brief upload an hash_map in the output
   */
  void setTkDcuInfo ( Sgi::hash_map<unsigned long, TkDcuInfo *> vInfo ) 
    throw ( FecExceptionHandler ) ;

  /** \brief upload the hash_map (attribut of the class) in the output
   */
  void setTkDcuInfo ( ) 
    throw ( FecExceptionHandler ) ;

  /** Delete the components of a vector
   */
  static void deleteVersionVectorI ( tkVersionVector &v ) {
    for ( tkVersionVector::iterator it = v.begin() ; it != v.end() ; it ++) {
      delete(*it) ;
    }
    v.clear() ;
  } 

  /** Delete the components of a vector
   */
  static void deleteRunVectorI ( tkRunVector &v ) {
    for ( tkRunVector::iterator it = v.begin() ; it != v.end() ; it ++) {
      delete(*it) ;
    }
    v.clear() ;
  } 

  /** \brief delete a vector of state
   */
  static void deleteStateVectorI ( tkStateVector &v ) {
    for ( tkStateVector::iterator it = v.begin() ; it != v.end() ; it ++) {
      delete(*it) ;
    }
    v.clear() ;
  } 

  /** Delete a vector of DCU info
   */
  static void deleteVectorI ( tkDcuInfoVector &dVector ) {
    for (tkDcuInfoVector::iterator it = dVector.begin() ; it != dVector.end() ; it ++) {
      delete *it ;
    }
    dVector.clear() ;
  }

  /** \brief Copy a vector of descriptions into another one with a clone of the descriptions
   * \warning the remote method must delete the vector created
   */
  static void vectorCopyI ( tkDcuInfoVector &dst, tkDcuInfoVector &src ) {

    if (! src.empty()) {
      for (tkDcuInfoVector::iterator device = src.begin() ; device != src.end() ; device ++) {       
	dst.push_back ( (*device)->clone() ) ;
      }
    }
  }
} ;

#endif
