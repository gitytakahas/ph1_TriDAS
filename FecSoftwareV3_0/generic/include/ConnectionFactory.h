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

#ifndef CONNECTIONFACTORY_H
#define CONNECTIONFACTORY_H

#include <string>
#include <map>

#include "DetGeo.h"
#include "ConnectionDescription.h"

#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif

#ifdef DATABASE
#include "DbConnectionsAccess.h"
#endif

#include "XMLConnection.h"
#include "DeviceFactoryInterface.h"

typedef struct {

  unsigned int versionMajorId ;
  unsigned int versionMinorId ;

} ConnectionFactoryVersionValues ;

typedef std::pair<unsigned int, unsigned int> pairUnsigned_t ;

/** This class manage all connection descriptions from the retreive from file or database to the parsing and finally the descriptions
 */
class ConnectionFactory: public DeviceFactoryInterface {

 private:

  /** Connection version
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

  /** connection description vector
   */
  ConnectionVector connectionVector_ ;

  /** classification by FED, FED channel
   */
  std::map< pairUnsigned_t, ConnectionDescription * > fedChannelConnectionDescription_ ;

  /** classification by DCU hard ID
   */
  std::map<unsigned int, ConnectionVector> dcuHardIdConnectionDescriptions_ ;

  /** Detector geometry map
   */
  std::vector<DetGeo *> detGeoMap_ ;

#ifdef DATABASE
  /** \brief retreive information concerning the partition versus the version depending of the parameters passed 
   */
  std::list<unsigned int *> getPartitionVersion ( std::string partitionName ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#endif

  /** \brief Retreive the descriptions for all connections from the input
   */
  void getConnectionDescriptions ( bool fileUsed, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor, std::string partitionName, ConnectionVector &outVector, bool allConnections = false, bool forceDbReload = false ) throw (FecExceptionHandler ) ;

 public:

  /** \brief Build a factory and a database access
   */
  ConnectionFactory ( ) ;

#ifdef DATABASE
  
  /** \brief Build a factory for database
   */
  ConnectionFactory ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ) ;
  
  /** \brief Buid a FEC factory for FEC database
   */
  ConnectionFactory ( DbConnectionsAccess *dbConnectionsAccess )  ;

#endif

  /** \brief Disconnect the database (if it is set)
   */  
  ~ConnectionFactory ( ) ;

  /** \brief clear the connections and the map associated to it
   */
  void clear ( std::string partitionName = "nil" ) ;

  /** \brief return the connection vector from the memory
   */
  inline ConnectionVector getConnectionVector ( ) { return connectionVector_ ; } 

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
  void setDatabaseAccess ( DbConnectionsAccess *dbConnectionsAccess ) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // Partition / version methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief retreive information concerning the partition versus the version depending of the parameters passed 
   */
  void getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *maskVersionMajor, unsigned int *maskVersionMinor, unsigned int *partitionNumber ) 
    throw (oracle::occi::SQLException, FecExceptionHandler ) ;

#endif

  // ------------------------------------------------------------------------------------------------------
  // 
  // Get / Set methods from/into file or database
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief Build the map of modules
   */
  void buildModuleList( ) ;

  /** \brief Retreive the descriptions for all connections from the input for partition name
   */ 
  void getConnectionDescriptions ( std::string partitionName, ConnectionVector &outVector, unsigned int versionMajor = 0, unsigned int versionMinor = 0, unsigned int maskVersionMajor = 0, unsigned int maskVersionMinor = 0, bool allConnections = false, bool forceDbReload = false )
    throw (FecExceptionHandler) ;

  /** \brief return the connection descriptions from file
   */
  void getConnectionDescriptions ( ConnectionVector &outVector, bool allConnections = false ) throw (FecExceptionHandler ) ;

  /** \brief Upload the description in the output
   */
  void setConnectionDescriptions ( ConnectionVector connectionVector, std::string partitionName = "nil", unsigned int *versionMajor = NULL, unsigned int *versionMinor = NULL, bool majorVersion = false ) 
    throw ( FecExceptionHandler ) ;

  /** Delete a ConnectionVector
   * \param connectionVector - vector of connection descriptions
   * \warning this method only delete all devices not the vector (just clear method is called)
   */
  static void deleteVectorI ( ConnectionVector &connectionVector ) {
    
    if (! connectionVector.empty() ) {
      for (ConnectionVector::iterator it = connectionVector.begin() ; (it != connectionVector.end()) ; it ++) {
	delete *it ;
      }
      
      connectionVector.clear() ;
    }
  }

 /** \brief display a vector of connections
  * \param connectionVector - vector of connections
  */
  static void display ( ConnectionVector connectionVector ) {
    
    if (!connectionVector.empty()) {
      
      // For each device => access it
      for (ConnectionVector::iterator device = connectionVector.begin() ; device != connectionVector.end() ; device ++) {
        
	ConnectionDescription *connection = *device ;
	connection->display() ;
      }
    }
  }
 
  
  /** \brief Copy a vector of device descriptions into another one with a clone of the descriptions
   * \warning the remote method must delete the vector created
   */
  static void vectorCopyI ( ConnectionVector &dst, ConnectionVector &src, bool allConnections = false ) {
    
    if (! src.empty()) {
      
      for (ConnectionVector::iterator it = src.begin() ; it != src.end() ; it ++) {
	
	ConnectionDescription *itd = *it ;
	
	if (allConnections || itd->isEnabled()) dst.push_back ( itd->clone() ) ;
      }
    }
  }

  /** \brief Copy a vector of device descriptions into another one WITHOUT cloning the descriptions
   */
  static ConnectionVector copy ( ConnectionVector src, bool allConnections = false ) {

    ConnectionVector dst ;
    for (ConnectionVector::iterator it = src.begin() ; it != src.end() ; it ++) {
	
      ConnectionDescription *itd = *it ;	
      if (allConnections || itd->isEnabled()) dst.push_back ( itd ) ;
    }

    return dst ;
  }

  /** Return the number of connection descriptions downloaded
   * \return number of connections
   */
  inline unsigned int getNumberOfFedChannel(){ return connectionVector_.size(); }

  /** Return the i ConnectionDescription
   * \param i - ieme connection description
   * \return pointer to the connection description (should not be deleted, it is not cloned)
   * \deprecated you should retreive the connection vector and use an iterator
   * \see FedChannelConnectionDescription* getFedChannelConnection(int i)
   */
  inline ConnectionDescription *getFedChannelConnection(int i) { return (connectionVector_)[i];}

  /** Return the ConnectionDescription for the given FED and FED channel 
   * \see FedChannelConnectionDescription* getFedChannelConnection(int fedid,unsigned int fedch)
   * \return pointer to ConnectionDescription (that can be NULL)
   */
  inline ConnectionDescription *getFedChannelConnection(unsigned int fedId, unsigned int fedChannel) {

    pairUnsigned_t maPair ; maPair.first = fedId ; maPair.second = fedChannel ;
    return fedChannelConnectionDescription_[maPair] ;
  }

  /** Return the number of modules/detgeo
   * \see int getNumberOfModule()
   */
  inline unsigned int getNumberOfModule ( ) { return detGeoMap_.size(); }
  
  /** Return the ieme detgeo 
   * \param i - ieme getgeo to be returned
   * \return pointer to DetGeo
   * \see DetGeo* TrackerParser::getDetGeo(int i);
   */
  inline DetGeo* getDetGeo(unsigned int i){ return (detGeoMap_)[i]; }

  /** Add a new detgeo in the module list
   * \param g - DetGeo to be added
   */
  inline void addDetGeo(DetGeo *g){detGeoMap_.push_back(g);}

  /** Retreive the connection for a given DCU Hardware ID
   */
  void getFedChannelListByDcu (unsigned int dcuHardId, ConnectionVector &connections ) ;

  /** Return the first connection description for that DCU hardware ID
   * \see FedChannelConnectionDescription* getFedChannelConnectionByDcu(int dcu)
   */
  ConnectionDescription *getFedChannelListByDcu (unsigned int dcuHardId) ;
} ;

#endif
