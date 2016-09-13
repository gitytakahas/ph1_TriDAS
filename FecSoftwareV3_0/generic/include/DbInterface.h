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

#ifndef DbInterface_h
#define DbInterface_h

#include "DeviceFactory.h"

#define DEBUGMSGERROR

/** Class for the download of the database to make a caching system.
 * This interface returns only the devices enabled.
 * The DeviceFactory should remains since the beginning until the end of this class since the descriptions will be deleted with the delete of the Factory.
 */
class DbInterface {

 public:

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Constructor and destructor                                                                     */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Constructor
   */
  DbInterface( bool allFecDevices = false, bool allConnections = false, std::string login = "nil", std::string passwd = "nil", std::string path = "nil", bool threaded = false )  ;

  /** \brief Delete the attributs of the class
   */
  ~DbInterface() ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Some parameters for the class                                                                  */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Set if you all devices or only the enabled should be downloaded
   */
  void setAllDevices ( bool allFecDevices ) ;


  /** \brief Return if all the devices have been download or not
   */
  bool getAllDevices ( ) ;

  /** \brief Set if you all devices or only the enabled should be downloaded
   */
  void setAllConnections ( bool allConnections ) ;

  /** \brief Return if all the devices have been download or not
   */
  bool getAllConnections ( ) ;

  /** \brief Return the access to the database
   */
  DeviceFactory *getDeviceFactory ( ) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Get the current devices / connections / redundancy / FED / Det id                              */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Return the different values
   */
  deviceVector getCurrentDevices() ;

  /** \brief Return the pia values
   */
  piaResetVector getCurrentPia() ;

  /** \brief Return the connections values
   */
  ConnectionVector getConnections() ;

  /** \brief Return the redundancy values
   */
  tkringVector getFecRedundancy() ;

  /** \brief Return the list of det id downloaded
   */
  Sgi::hash_map<unsigned long, TkDcuInfo *> getDetIdList() ;

  /** \brief Return the error message
   */
  std::string getErrorMessage( ) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Clear the maps and vectors                                                                     */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Clear the FED descriptions
   */
  void clearFEDDescriptions ( ) ;

  /** \brief Clear the FEC descriptions (PIA and devices)
   */
  void clearFECDeviceDescriptions ( ) ;

  /** \brief Clear the PIA reset description (only PIA)
   */
  void clearPiaResetDescriptions ( ) ;

  /** \brief Clear the connection descriptions
   */
  void clearConnectionDescriptions ( ) ;

  /** \brief Clear the vector of ring descriptions
   */
  void clearFecRedundancyDescriptions ( ) ;

  /** \brief Clear the DET ID descriptions 
   */
  void clearDetIdDescriptions ( ) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Partition / version                                                                              */
  /*                                                                                                */
  /* ********************************************************************************************** */
  
  /** \brief Retreive the FED version for the partition specified
   */
  void getFedPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *maskVersionMajor, unsigned int *maskVersionMinor, unsigned int *partitionNumber ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Download / upload of the redundancy                                                            */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Download the FEC redundancy for a partition
   */
  int downloadFecRedundancyFromDatabase ( std::string partitionName ) ;

  /** \brief Download the FEC redundancy from a file
   */
  int downloadFecRedundancyFile ( std::string filename, bool &changed, bool redownload = true, bool addIt = true ) ;

  /** \brief Upload data to the database
   */
  int uploadFecRedundancyToDb(std::string partitionName) ;

  /** \brief Upload the current data to a file
   */
  int uploadFecRedundancyToFile(std::string filename, std::list<std::string> &listOfFiLes) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Download / upload of the FEC/PIA devices                                                       */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Download the FEC devices
   */
  int downloadFECDevicesFromDatabase ( std::string partitionName, bool &changed, unsigned int fecMajor = 0, unsigned int fecMinor = 0, unsigned int maskVersionMajor = 0, unsigned int maskVersionMinor = 0 ) ;

  /** \brief Download the FEC devices
   */
  int downloadPIAResetFromDatabase ( std::string partitionName ) ;

  /** \brief download the FEC data for a given partition name and version
   */
  int downloadFECFromDatabase(std::string partitionName, bool &changed, unsigned int fecMajor = 0, unsigned int fecMinor = 0, unsigned int maskVersionMajor = 0, unsigned int maskVersionMinor = 0, bool piaDownload = true ) ;

  /** \brief Download the FEC devices and the PIA reset from a file
   */
  int downloadFECFromFile (std::string filename, bool &changed, bool redownload = true ) ;

  /** \brief Upload data to the database
   */
  int uploadFec(std::string partitionName, bool ismajor, bool piaUpload = false, 
		unsigned int *fecVersionMajor = NULL, unsigned int *fecVersionMinor = NULL ) ;

  /** \brief Upload the current data to a file
   */
  int uploadFec(std::string filename) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Download / upload of FED                                                                       */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Download a specific FED from the database from partition
   */
  std::vector<Fed9U::Fed9UDescription*> downloadFEDFromDatabase ( std::string partitionName, bool &changed, unsigned int fedMajor = 0, unsigned int fedMinor = 0, unsigned int maskVersionMajor = 0, unsigned int maskVersionMinor = 0 ) ;
  
  /** \brief Download a specific FED from the database
   */
  Fed9U::Fed9UDescription* downloadFEDFromDatabase (int fedid, std::string partitionName, bool &changed, unsigned int fedMajor = 0, unsigned int fedMinor = 0, unsigned int maskVersionMajor = 0, unsigned int maskVersionMinor = 0 ) ;

  /** \brief Download a FED from a FILE
   */
  Fed9U::Fed9UDescription* downloadFEDFromFile ( int fedid, std::string filename, bool &changed, bool redownload = true ) ;
  
  /** \brief Download FEDs from a FILE
   */
  std::vector<Fed9U::Fed9UDescription *> downloadFEDFromFile ( std::string filename, bool &changed, bool redownload = true ) ;

  /** \brief Upload from the FED into the database
   */
  int uploadFed(std::string partitionName, bool strip, bool ismajor, bool debug = false, unsigned int *fedMajorVersion = NULL, unsigned int *fedMinorVersion = NULL) ;

  /** \brief Upload from the FED into the database
   */
  int uploadFed(int fedid, std::string partitionName, bool strip, bool debug = false, unsigned int *fedMajorVersion = NULL, unsigned int *fedMinorVersion = NULL) ;

  /** \brief Upload the FED into a file
   */
  int uploadFed(std::string filename) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Download / upload of the FEC/FED connections                                                   */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Download the connections
   */
  int downloadConnectionsFromDatabase ( std::string partitionName, bool &changed, unsigned int connectionMajor = 0, unsigned int connectionMinor = 0, unsigned int maskVersionMajor = 0, unsigned int maskVersionMinor = 0 ) ;

  /** \brief Download the connections from a file
   */
  int downloadConnectionsFromFile (std::string filename, bool &changed, bool redownload = true ) ;

  /** \brief Upload connections to the database
   */
  int uploadConnections(std::string partitionName, bool ismajor, unsigned int *connectionVersionMajor = NULL, unsigned int *connectionVersionMinor = NULL ) ;

  /** \brief Upload the current data to a file
   */
  int uploadConnections(std::string filename) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Download / upload of the DET ID                                                                */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Download the DET ID for a given partition name
   */
  int downloadDetIdFromDatabase ( std::string partitionDetIdName, bool &changed, unsigned int detIdMajor = 0, unsigned int detIdMinor = 0 ) ;

  /** \brief Download the DET ID from file
   */
  int downloadDetIdFromFile (std::string filename, bool &changed, bool redownload = true ) ;

  /** \brief Upload data to the database
   */
  int uploadDetIdToDB( ) ;

  /** \brief Upload the current data to a file
   */
  int uploadDetItToFile (std::string filename) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Cache refreshing of the DB                                                                     */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief Refresh the caching system
   */
  int refreshCacheXMLClob ( ) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* O2O operations                                                                                 */
  /*                                                                                                */
  /* ********************************************************************************************** */

  /** \brief insert a new o2o operation
   */
  int setO2OOperation ( std::string partitionName, std::string subDetector, unsigned int o2oRunNumber ) ;

  /** \brief crosscheck the o2o operation with the current state
   */
  int getO2OXchecked ( std::string partitionName ) ;

  /* ********************************************************************************************** */
  /*                                                                                                */
  /* Attributs of the class                                                                         */
  /*                                                                                                */
  /* ********************************************************************************************** */

 private:

  /** \brief Database connection
   */
  DeviceFactory *deviceFactory_ ;

  /** \brief FED 9U description
   */
  Fed9U::Fed9UHashMapType theFed9UDescription_ ;

  /** \brief FED to be deleted
   */
  std::vector<Fed9U::Fed9UDescription*> fedToBeDeleted_ ;

  /** \brief FEC devices
   */
  deviceVector currentFecDevices_;

  /** \brief PIA devices
   */
  piaResetVector currentPiaReset_;

  /** \brief Connection descriptions
   */
  ConnectionVector connectionVector_ ;

  /** \brief Ring/CCU descriptions
   */
  tkringVector tkRingDescriptions_ ;

  /** \brief FEC major version
   */
  unsigned int fecMajorVersion_ ;

  /** \brief FEC minor version
   */
  unsigned int fecMinorVersion_ ;

  /** \brief Connection major version
   */
  unsigned int connectionMajorVersion_ ;

  /** \brief Connection minor version
   */
  unsigned int connectionMinorVersion_ ;

  /** \brief FED major version
   */
  unsigned int fedMajorVersion_ ;

  /** \brief FED minor version
   */
  unsigned int fedMinorVersion_ ;

  /** \brief DET ID major version
   */
  unsigned int detIdMajorVersion_ ;

  /** \brief DET ID minor version
   */
  unsigned int detIdMinorVersion_ ;

  /** \brief Mask major version
   */
  unsigned int maskMajorVersion_ ;

  /** \brief Mask minor version
   */
  unsigned int maskMinorVersion_ ;

  /** \brief Partition name
   */
  std::string partitionName_ ;

  /** \brief Partition name for the det id (can be ALL)
   */
  std::string partitionDetIdName_ ;

  /** \brief Partition name for the det id (can be ALL)
   */
  std::string partitionRedundancyName_ ;

  /** \brief All devices
   */
  bool allFecDevices_ ;

  /** \brief All connections
   */
  bool allConnections_ ;

  /** \brief FEC file
   */
  std::string redundancyFileName_ ;

  /** \brief FEC file
   */
  std::string fecFileName_ ;

  /** \brief DET ID file
   */
  std::string detIdFileName_ ;

  /** \brief FED file
   */
  std::string fedFileName_ ;

  /** \brief connection file
   */
  std::string connectionFileName_ ;

  /** \brief Partition download for the FED
   */
  bool partitionFEDDownload_ ;

  /** \brief To avoid use of exception, this message is the last message stored
   */
  std::string errorMessage_ ;
};

#endif

	  
	
	
      
    
