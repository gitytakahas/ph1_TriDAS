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

//#define DEBUGMSGERROR

#include <time.h>  // for time function

#include "XMLConnection.h"
#include "ConnectionFactory.h"

/** Build a ConnectionFactory to retreive information from database or file
 * \param outputFileName - name of the XML output file
 * \exception Possible exceptions are:
 *     file not found
 *     bad XML format
 * \see addFileName to define the input file
 */
ConnectionFactory::ConnectionFactory ( ): 
  DeviceFactoryInterface ( ) {

// #ifdef DATABASE
//   if (databaseAccess) {
//     try {
//       setDatabaseAccess ( ) ;
//     }
//     catch (oracle::occi::SQLException &e) {
      
//       std::cerr << "*********** ERROR *************" << std::endl ;
//       std::cerr << "Unable to connect the database: " << std::endl ;
//       std::cerr << e.what() << std::endl ;
//       std::cerr << "*******************************" << std::endl ;

//       databaseAccess = false ;
//     }
//   }
// #else
//   databaseAccess = false ;
// #endif
//  if (! databaseAccess) setUsingFile ( ) ;

  setUsingFile ( ) ;
}

#ifdef DATABASE

/** Create an access to database
 * \param login - login to database
 * \param password - password to database
 * \param path - URL to acess the database
 * \param className - class name to access the database
 * \param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 */
ConnectionFactory::ConnectionFactory ( std::string login, std::string password, std::string path, bool threaded ) 
    throw ( oracle::occi::SQLException ): 
  DeviceFactoryInterface ( login, password, path, threaded ) {

  setDatabaseAccess ( login, password, path ) ;
}

/** Create an access to the connection database
 * \param dbAccess - database access
 */
ConnectionFactory::ConnectionFactory ( DbConnectionsAccess *dbAccess ):
  DeviceFactoryInterface ((DbCommonAccess *)dbAccess) {
}

#endif

/** Disconnect the database (if it is set)
 */  
ConnectionFactory::~ConnectionFactory ( ) {

  // The database access is deleted in the DeviceFactoryInterface
  
  // clear map of connections
  clear() ;

}

/** Destroy all connections and related map
 * \param partitionName - only clear the corresponding partition name except if it is nil
 */
void ConnectionFactory::clear ( std::string partitionName ) {

  //if ( (partitionName == "nil") || partitionName.size() == 0) { 
  // delete the old vector that is not more usefull
  ConnectionFactory::deleteVectorI (connectionVector_) ;
  fedChannelConnectionDescription_.clear() ;
  dcuHardIdConnectionDescriptions_.clear() ;
  for (std::vector<DetGeo *>::iterator it = detGeoMap_.begin() ; it != detGeoMap_.end() ; it ++) delete *it ;
  detGeoMap_.clear() ;
  //}
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 */
void ConnectionFactory::addFileName ( std::string fileName ) {

  // devices not used (not put in the vector in connection attribut of this class)
  // XMLConnection does not delete anymore the devices
  ConnectionVector deleteVector ;

  // For connections
  XMLConnection xmlConnection ( fileName ) ;

  // Retreive all the devices from the parsing class
  ConnectionVector connectionVector = xmlConnection.getConnections ( ) ;

  if ( ! connectionVector.empty() ) {

    // Merge the vector from the class and the new vector
    for (ConnectionVector::iterator it = connectionVector.begin() ; it != connectionVector.end() ; it ++) {

      bool equalS = false ;

      for (ConnectionVector::iterator itP = connectionVector_.begin() ; (itP != connectionVector_.end() && ! equalS ) ; itP ++) {
	if (*itP == *it) equalS= true ;
      }

      if (! equalS) 
	connectionVector_.push_back (*it) ;
      else
	deleteVector.push_back(*it) ;
    }
  }

  // All devices are deleted by the XMLConnection so nothing must be deleted

#ifdef DATABASE
  initDbVersion_ = false ;
  useDatabase_ = false ;
#endif

  // Delete the devices not used
  ConnectionFactory::deleteVectorI(deleteVector) ;
}


/** Set a file as the new input, same method than addFileName but the previous devices and pia are deleted
 * \param inputFileName - new input file
 */
void ConnectionFactory::setInputFileName ( std::string inputFileName ) {

  // delete the old vector that is not more usefull
  clear ( ) ;

  // Add new entries
  ConnectionFactory::addFileName (inputFileName) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** 
 * \param login - login to database
 * \param password - password to database
 * \param path - path to database
 * \warning in the destructor of ConnectionFactory, the dbAccess is deleted
 */
void ConnectionFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) 
  throw ( oracle::occi::SQLException ) {

  // For connection database delete the old one if needed
  if (dbAccess_ != NULL)
    if (login != dbAccess_->getDbLogin() || password != dbAccess_->getDbPassword() || path != dbAccess_->getDbPath()) {
      delete dbAccess_ ;
      dbAccess_ = NULL ;
    }
  
  if (dbAccess_ == NULL) {
#ifdef DEBUGMSGERROR
    std::cout << "Create a new access to the connection database for " << login << "/" << password << "@" << path << std::endl ;
#endif

    dbAccess_ = new DbConnectionsAccess (login, password, path, threaded_) ;
  }

  deleteDbAccess_ = true ;
  useDatabase_ = true ;

  // No version intialised
  initDbVersion_ = false ;
}

/** 
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void ConnectionFactory::setDatabaseAccess ( ) 
  throw ( oracle::occi::SQLException ) {

  // Retreive the parameters
  std::string login, password, path ;
  if (getDatabaseConfiguration(login, password, path)) {
    setDatabaseAccess (login, password, path) ;
  }

  // No version intialised
  initDbVersion_ = false ;
}

/** 
 * \param dbAccess - access to the database
 * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
 */
void ConnectionFactory::setDatabaseAccess ( DbConnectionsAccess *dbAccess ) {

  // For connection database delete the old one if needed
  DeviceFactoryInterface::setDatabaseAccess ((DbCommonAccess *)dbAccess) ;

  // No version intialised
  initDbVersion_ = false ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Partition / version methods
//
// ------------------------------------------------------------------------------------------------------

/**
 * \param partitionName - partitionName
 * \return the information about partition versus version (with mask version)
 * The parameters must be deleted by the remote method
 * Note that the method returns always a pointer allocated but the list can be empty
 */
std::list<unsigned int *> ConnectionFactory::getPartitionVersion ( std::string partitionName ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
  std::cout << "Retreive the current version for the partition " << partitionName << std::endl ;
#endif

  std::list<unsigned int*> partitionVersion ;

  if (dbAccess_ != NULL) {
    
    // Retreive the current version for the given partition
    partitionVersion = ((DbConnectionsAccess *)dbAccess_)->getDatabaseVersion(partitionName) ;

    if (partitionVersion.empty()) {
      std::stringstream msgError ; msgError << "No version for the partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER ( DB_NOVERSIONAVAILABLE, msgError.str(), ERRORCODE) ;
    }
#ifdef DEBUGMSGERROR
    else {
      for (std::list<unsigned int*>::iterator it = partitionVersion.begin() ; it != partitionVersion.end() ; it ++) {
	
	unsigned int *value = *it;
	
	std::cout << "\tCurrent version: Partition " << value[0] << ": Version " << value[1] << "." << value[2] << "(mask = " << value[3] << "." << value[4] <<")" ;
	if ((value[1] == 0) && (value[2] == 0)) std::cout << " <= partition not used" << std::endl ;
	else std::cout << std::endl ;
      }
    }
#endif
  }
  else {
#  ifdef DEBUGMSGERROR
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Unable to retreive the partition/version, the database is not set" << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
#  endif

    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
  
  return (partitionVersion) ;
}

/**
 * \param partitionName - partitionName
 * \param major - major version returned
 * \param minor - minor version returned
 * \param maskVersionMajor - mask version returned
 * \param maskVersionMinor - mask version returned
 * \param partitionNumber - partition number returned
 */
void ConnectionFactory::getPartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *maskVersionMajor, unsigned int *maskVersionMinor, unsigned int *partitionNumber ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
  std::cout << "Retreive the current version for the partition " << partitionName << std::endl ;
#endif

  *major = *minor = *partitionNumber = 0 ;
  std::list<unsigned int*> partitionVersion = getPartitionVersion ( partitionName ) ;
  if (! partitionVersion.empty()) {
    std::list<unsigned int*>::iterator it = partitionVersion.begin() ;
    unsigned int *value = *it;
    *partitionNumber = value[0] ;
    *major = value[1] ;
    *minor = value[2] ;
    *maskVersionMajor = value[3] ;
    *maskVersionMinor = value[4] ;
    int error = partitionVersion.size() ;
    for (std::list<unsigned int*>::iterator it = partitionVersion.begin() ; it != partitionVersion.end() ; it ++) {
      delete[] *it ;
    }
    if (error > 1) {
      std::stringstream msgError ; msgError << "Several versions exists for the partition " << partitionName ;
      RAISEFECEXCEPTIONHANDLER ( DB_SEVERALVERSIONSERROR, msgError.str(), ERRORCODE ) ;
    }
  }
}

#endif // Database

/** Retreive the descriptions for all connections from the input
 * \param fileUsed - use file
 * \param versionMajor - major version for database
 * \param versionMinor - minor version for database
 * \param partitionName - partition name
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are NOT cloned and put into this output
 * \param allConnections - return all devices including the disable device, by default return only the enabled devices
 * \param forceDbReload - force the database reload
 * One the following parameter must be specified and it is checked in the following order
 * <ul>
 * <li> partitionName
 * </ul>
 * \return a vector of connections descriptions
 * \warning if no parameters, forceReload are given then the devices are retreived from files given by addFileName
 * \warning if the version is 0, 0 then the devices is coming from the current version
 * \warning if the version is different than 0.0 then the values are retreived from the version given. Note that this version is set as the current version.
 * \warning if the input if a file whatever the version is, the device description is retreived from the input file.
 * \warning You must delete the vector you pass to the method with ConnectionFactory::deleteVector method
 */ 
void ConnectionFactory::getConnectionDescriptions ( bool fileUsed, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor, std::string partitionName, ConnectionVector &outVector, bool allConnections, bool forceDbReload ) 
  throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << versionMajor << "." << versionMinor << std::endl ;
  std::cout << "partition = " << partitionName << std::endl ;
#endif

#ifdef DEBUGMSGERROR
  if ( fileUsed )
    std::cout << "getConnectionDescriptions from the files set" << std::endl ;
  else {
    std::cout << "getConnectionDescriptions for the partition " << partitionName << std::endl ;

    if ( (versionMajor == 0) && (versionMinor == 0) ) std::cout << "  Retreive from the current version" << std::endl ;
    else std::cout << "  Retreive from the version " << versionMajor << "." << versionMinor << " & mask " << maskVersionMajor << "." << maskVersionMinor << std::endl ;
  }
#endif

#ifdef DATABASE
  // retreive the information from database
  if ( getDbUsed() && !fileUsed ) {

    // Check the version
    if (forceDbReload) initDbVersion_ = false ;
    if (initDbVersion_) {
      if ((versionMajor != 0) || (versionMinor != 0)) {  // A version has been set

	if ((partitionName_ == partitionName) && ((uint)versionMajor == versionMajor_) && ((uint)versionMinor == versionMinor_) &&  ((uint)maskVersionMajor == maskVersionMajor_) && ((uint)maskVersionMinor == maskVersionMinor_))
	  initDbVersion_ = true ;
	else 
	  initDbVersion_ = false ;
      }
      else { // from current version
	unsigned int partitionNumber, versionMajorI, versionMinorI, maskMajorI, maskMinorI ;
	getPartitionVersion(partitionName, &versionMajorI, &versionMinorI, &maskMajorI, &maskMinorI, &partitionNumber) ;
	if ((partitionName_ == partitionName) && (versionMajorI == versionMajor_) && (versionMinorI == versionMinor_) && (maskMajorI == maskVersionMajor_) && (maskMinorI == maskVersionMinor_)) 
	  initDbVersion_ = true ;
	else 
	  initDbVersion_ = false ;

#ifdef DEBUGMSGERROR
	std::cout << "Partition " << partitionName << " / " << partitionName_ 
		  << " Version " << versionMajorI << "." << versionMinorI << " / " 
		  << versionMajor_ << "." << versionMinor_ << 
		  << " Mask " << maskMajorI << "." << maskMinorI << " / " 
		  << maskVersionMajor_ << "." << maskVersionMinor_
		  << std::endl ;
#endif
      }
    }

    // Check if the downlaod must be done again
    if (!initDbVersion_) {

      // delete the old vector that is not more usefull
      clear(partitionName) ;

      // Now retreive the information from database
      XMLConnection xmlConnection ( (DbConnectionsAccess *)dbAccess_ ) ;
      ConnectionVector connectionVector ;
      
      if ((versionMajor != 0) || (versionMinor != 0)) {

#ifdef DEBUGMSGERROR
	std::cout << "Try to retreive version " << versionMajor << "." << versionMinor << " & mask " << maskVersionMajor << "." << maskVersionMinor << std::endl ;
#endif

	connectionVector = xmlConnection.getConnections ( partitionName, versionMajor, versionMinor, maskVersionMajor, maskVersionMinor ) ;
      }
      else {
	
	connectionVector = xmlConnection.getConnections ( partitionName ) ;
      }
      
      // Copy the vector in the vector of the class and in the output
      if (! connectionVector.empty()) {
	for (ConnectionVector::iterator it = connectionVector.begin() ; it != connectionVector.end() ; it ++) {	  
	  ConnectionDescription *connection = *it ;
	  connectionVector_.push_back(connection) ;
	}
      }

      // Copy the devices in the output vector => no clone!!!
      outVector = ConnectionFactory::copy(connectionVector_,allConnections) ;
      
      // Register the version
      if ((versionMajor != 0) || (versionMinor != 0)) {
	versionMajor_  = versionMajor ;
	versionMinor_  = versionMinor ;
	partitionName_ = partitionName ;
	maskVersionMajor_ = maskVersionMajor ;
	maskVersionMinor_ = maskVersionMinor ;
	initDbVersion_ = true ;
      }
      else {
	// Current state, check the value
	unsigned int partitionNumber ;
	getPartitionVersion( partitionName, &versionMajor_, &versionMinor_, &maskVersionMajor_, &maskVersionMinor_, &partitionNumber) ;
	partitionName_ = partitionName ;
	initDbVersion_ = true ;
      }
    }
    else {
      // Copy the descriptions into the output for file
      outVector = ConnectionFactory::copy ( connectionVector_, allConnections ) ;
    }
  }
  else 
#endif
    {
      // Copy the descriptions into the output for file
      outVector = ConnectionFactory::copy ( connectionVector_, allConnections ) ;
    }

  // -------------------------------------------------------------------
  // Clear and rebuild the different maps based on the connections given
  fedChannelConnectionDescription_.clear() ;
  dcuHardIdConnectionDescriptions_.clear() ;
  for (ConnectionVector::iterator it = connectionVector_.begin() ; it != connectionVector_.end() ; it ++) {
    ConnectionDescription *connection = (*it) ;

    // classification by FED, FED channel
    pairUnsigned_t maPair = std::make_pair(connection->getFedId(),connection->getFedChannel());
    fedChannelConnectionDescription_[maPair] = connection ;

    // classification by DCU hard ID
    dcuHardIdConnectionDescriptions_[connection->getDcuHardId()].push_back(connection) ;
  }
  buildModuleList ( ) ;
}

/** Build the module list, one detgeo pointer by DCU hardware ID
 */
void ConnectionFactory::buildModuleList( ) {
  
  // Clean the table for all partitions
  for (std::vector<DetGeo *>::iterator it = detGeoMap_.begin() ; it != detGeoMap_.end() ; it ++) delete *it ;
  detGeoMap_.clear() ;
  
  // For each set of connections per DCU hardware ID
  for (std::map<unsigned int, ConnectionVector>::iterator it=dcuHardIdConnectionDescriptions_.begin() ; it != dcuHardIdConnectionDescriptions_.end() ; it ++) {
    
    ConnectionVector connections = it->second ;
    // If connections is existing for DCU hardware ID
    if (connections.size()) {

      unsigned int napv = connections.size() * 2;
      unsigned int enabled = 0 ;
      for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++) {
	if ( ((*it)->getApvAddress() == 34) || ((*it)->getApvAddress() == 35) ) napv = 6 ;
	if ((*it)->isEnabled()) enabled ++ ;
      }

      // ------------------------------------------------------------
      //if ( ((enabled != 4) && (napv != 4)) || ((enabled != 6) && (napv != 6)) ) {
      //}
      // ------------------------------------------------------------

      ConnectionDescription *connection = (*connections.begin()) ;

      // Build the module
      DetGeo* g =  new DetGeo();
	 
      unsigned long key = DetGeo::BuildModuleKey(0, connection->getFecSlot(), connection->getRingSlot(), connection->getCcuAddress(), connection->getI2cChannel() ) ;
      g->SetChipSize(280);
      g->Set1st(1);
      g->SetLast(napv*128);
      g->SetType(1);
      g->SetTilt(0);
      g->SetDx(0);
      g->SetDy(0);
      g->SetDz(0);
      g->SetNChips(napv/2);     
      
      if (napv == 4) {
	int id1= DetGeo::BuildFedChannelKey(key,1,1);
	int id3= DetGeo::BuildFedChannelKey(key,3,3);
	g->SetIdAcq(0,id1);
	g->SetIdAcq(1,id3);
	g->SetIdAcq(2,0);

#ifdef DEBUGMSGERROR
	char msg[80] ;
	decodeKey(msg,key) ;
	std::cout << msg << "(" << napv<< " APVs): " << std::dec << id1 << ", " << id3 << std::endl ;
#endif
      }
      else {
	int id1= DetGeo::BuildFedChannelKey(key,1,1);
	int id2= DetGeo::BuildFedChannelKey(key,2,2);
	int id3= DetGeo::BuildFedChannelKey(key,3,3);
	
	g->SetIdAcq(0,id1);
	g->SetIdAcq(1,id2);
	g->SetIdAcq(2,id3);

#ifdef DEBUGMSGERROR
	char msg[80] ;
	decodeKey(msg,key) ;
	std::cout << msg << "(" << napv<< " APVs): " << std::dec << id1 << ", " << id3 << std::endl ;
#endif
      }                

      g->SetPolarity (1);
      g->SetPitch (0.0180);
      g->SetDirection (1);
      g->SetNcm (128);
      g->SetCluSeed(4.);
      g->SetCluCut(2.);
      g->SetHighCut(3.);
      
      g->SetPedDead(0.1);
      g->SetPedNoise(100.);
      
      g->SetNoiDead(0.1);
      g->SetNoiNoise(100.);
      g->SetDcuId(0); // Set it to 0 to keep local id as tag in TBMonitor (LM 19/07/07)
      
      addDetGeo(g);	  
    }
  }
  
#ifdef DEBUGMSGERROR
    std::cout << "Number of enabled modules is " << detGeoMap_.size() << std::endl ;
#endif
}

/** Retreive the connection descriptions for a given partition name
 * \param partitionName - name of the partition
 * \param versionMajor - major version for database
 * \param versionMinor - minor version for database
 * \param maskVersionMajor - mask major version
 * \param maskVersionMinor - mask minor version
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are cloned and put into this output.
 * \param allConnections - return all devices including the disable device, by default return only the enabled devices
 * \param forceDbReload - force the database reload
 * \warning if no parameters are given then the devices are retreived from files given by addFileName
 * \warning if the version is -1, -1 the devices are retreived from files given by addFileName
 * \warning if the version is 0, 0 then the devices is coming from the current version
 * \warning if the version is different than 0.0 or -1.-1 then the values are retreived from the version given. Note that this version is set as the current version.
 * \warning if the input if a file whatever the version is, the device description is retreived from the input file.
 * \warning You must delete the vector you pass to the method with ConnectionFactory::deleteVector method
 * \warning this method is the same than getConnectionDescriptionsPartitionName, it is maintained for backward compatilibility
 */ 
void ConnectionFactory::getConnectionDescriptions ( std::string partitionName, ConnectionVector &outVector, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor, bool allConnections, bool forceDbReload ) 
  throw (FecExceptionHandler ) {

  getConnectionDescriptions ( false, versionMajor, versionMinor, maskVersionMajor, maskVersionMinor, partitionName, outVector, allConnections, forceDbReload ) ;
}

/** Retreive the connection description from a file
 * \param outVector - output vector of descriptions. The descriptions from the original vector (attribut of that class) are cloned and put into this output
 * \param allDevices - return all devices including the disable device, by default return only the enabled devices
 * \warning You must delete the vector you pass to the method
 */
void ConnectionFactory::getConnectionDescriptions ( ConnectionVector &outVector, bool allConnections )
  throw (FecExceptionHandler ) {

  getConnectionDescriptions ( true, 0, 0, 0, 0, "nil", outVector, allConnections ) ;
}


/** Upload the description in the output and upload it in a new version. 
 * \param connectionVector - a vector of connection descriptions
 * \param partitionName - if a partition name is specified, try to retreive the version for this given partition name
 * \param versionMajor - value for the version if the database is used as output.
 * \param versionMinor - value for the version if the database is used as input.
 * \param majorVersion - if true (default parameter) then a major version is created, if false a minor version is created. In this case, the version minor created is a child of the current version major set in the current state.
 * \warning the version parameters are set by the database (there are output parameters)
 * \warning the version can be NULL if the output is a file
 * \warning Note that devices must exist before made this call. 
 * \See creationPartition (deviceVector, int *, int *, std::string, std::string)
 * \warning if you create a minor version, the process will try to get the version for the partition if it is set
 * \warning the version uploaded is set automatically as the next version to be downloaded
 */
void ConnectionFactory::setConnectionDescriptions ( ConnectionVector connectionVector, std::string partitionName, unsigned int *versionMajor, unsigned int *versionMinor, bool majorVersion )
  throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
#  ifdef DATABASE
  if ( getDbUsed() && (versionMajor != NULL) && (versionMinor != NULL) ) 
    std::cout << "setConnectionDescriptions for the partition " << partitionName << std::endl ;
  else 
#  endif
    std::cout << "setConnectionDescriptions for the file " << outputFileName_ << std::endl ;
#endif
  
  if (connectionVector.empty()) RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  
#ifdef DATABASE
  if ( getDbUsed() && (versionMajor != NULL) && (versionMinor != NULL) ) {

    // Create an empty XML connection
    XMLConnection xmlConnection ;
    // Upload in database
    xmlConnection.setDatabaseAccess((DbConnectionsAccess *)dbAccess_);

#ifdef DEBUGMSGERROR
    if (majorVersion) std::cout << "New version major" << std::endl ;
    else std::cout << "New version minor" << std::endl ;
#endif

    unsigned int versionUpdate = 0 ;
    if (majorVersion) versionUpdate = 1 ;        // upload in next major
    else versionUpdate = 0 ;                     // current major . next minor
    // If the version to be created is a major version then all the devices should be added (even the disabled one)
    if (majorVersion && (connectionVector_.size() != 0) && (connectionVector.size() != connectionVector_.size())) {
      ConnectionVector toBeUploaded ;
      for (ConnectionVector::iterator itDev = connectionVector.begin() ; itDev != connectionVector.end() ; itDev ++) {
	toBeUploaded.push_back(*itDev) ;
      }
      for (ConnectionVector::iterator itVDev = connectionVector_.begin() ; itVDev != connectionVector_.end() ; itVDev ++) {
	bool toBeAdded = true ;
	for (ConnectionVector::iterator itDev = connectionVector.begin() ; itDev != connectionVector.end() ; itDev ++) {
	  if ( *(*itVDev) == *(*itDev) ) toBeAdded = false ;
	}
	if (toBeAdded) toBeUploaded.push_back(*itVDev) ;
      }
      xmlConnection.setDbConnectionDescription(partitionName, toBeUploaded, versionUpdate) ;
    }
    else {
      xmlConnection.setDbConnectionDescription(partitionName, connectionVector, versionUpdate) ;
    }

#ifdef DEBUGMSGERROR
    std::cout << "upload terminated" << std::endl ;
#endif

    // Retreive the version that has been uploaded
    if ( (versionMajor != NULL) && (versionMinor != NULL) ) {
      unsigned int partitionId ;
      unsigned int maskMajor, maskMinor ;
      getPartitionVersion (partitionName,versionMajor,versionMinor,&maskMajor,&maskMinor,&partitionId) ;
    }
  }
  else
#endif
    {  
      // Complete all the devices with the disabled device
      ConnectionVector toBeUploaded ;
      for (ConnectionVector::iterator itDev = connectionVector.begin() ; itDev != connectionVector.end() ; itDev ++) {
	toBeUploaded.push_back(*itDev) ;
      }
      for (ConnectionVector::iterator itVDev = connectionVector_.begin() ; itVDev != connectionVector_.end() ; itVDev ++) {
	bool toBeAdded = true ;
	for (ConnectionVector::iterator itDev = connectionVector.begin() ; itDev != connectionVector.end() ; itDev ++) {
	  if ( *(*itVDev) == *(*itDev) ) toBeAdded = false ;
	}
	if (toBeAdded) toBeUploaded.push_back(*itVDev) ;
      }

      // Upload in file
      XMLConnection xmlConnection ; 

#ifdef DEBUMSGERROR
      std::cout << "Found " << toBeUploaded.size() << " connections and upload in file " << outputFileName_ << std::endl ;
#endif

      xmlConnection.setFileConnections (toBeUploaded, outputFileName_) ;
    }
}


/** Retreive the connection for a given DCU Hardware ID
 * \param dcuHardId - DCU hardware ID
 * \param connections - vector connection descriptions for the the DCU hardware ID specified
 * \see vector<FedChannelConnectionDescription*>getFedChannelListByDcu(dcu->getDcuHardId())
 */
void ConnectionFactory::getFedChannelListByDcu (unsigned int dcuHardId, ConnectionVector &connections ) {
  
  for (ConnectionVector::iterator it = dcuHardIdConnectionDescriptions_[dcuHardId].begin() ; it != dcuHardIdConnectionDescriptions_[dcuHardId].end() ; it ++) {
    connections.push_back(*it) ;
  }
}

/** Return the first connection description for that DCU hardware ID
 * \see FedChannelConnectionDescription* getFedChannelConnectionByDcu(int dcu)
 * \param dcuHardId - DCU hardware ID
 */
ConnectionDescription *ConnectionFactory::getFedChannelListByDcu (unsigned int dcuHardId) {
  
  ConnectionDescription *connection = NULL ;
  
  for (ConnectionVector::iterator it = dcuHardIdConnectionDescriptions_[dcuHardId].begin() ; it != dcuHardIdConnectionDescriptions_[dcuHardId].end() ; it ++) {
    if ( (connection == NULL) || (connection->getApvAddress() > (*it)->getApvAddress()) )
      connection = (*it) ;
  }
  
  return connection ;
}
