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
#include "MemBufOutputSource.h"
#include "XMLFec.h"
#include "FecFactory.h"
#include "MemBufOutputSource.h"
#include "XMLFecCcu.h"

/** Build a FecFactory to retreive information from a file
 * \exception Possible exceptions are:
 *     file not found
 *     bad XML format
 */
FecFactory::FecFactory (  ):
  FecDeviceFactory (),
  PiaResetFactory  () {
}

#ifdef DATABASE

/** Create an access to the database FEC database
 * \param login - login to database
 * \param password - password to database
 * \param path - URL to acess the database
 * \param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 */
FecFactory::FecFactory ( std::string login, std::string password, std::string path, bool threaded ) 
  throw ( oracle::occi::SQLException ):
  FecDeviceFactory (login, password, path, threaded),
  PiaResetFactory (login, password, path, threaded) {
}

#endif

/** Disconnect the database (if it is set) and delete the vector of each FecDeviceFactory and PiaResetFactory
 */  
FecFactory::~FecFactory ( ) {
}

// ------------------------------------------------------------------------------------------------------
// 
// XML file methods
//
// ------------------------------------------------------------------------------------------------------

/** Set this file as the new output
 * \param outputFileName - file name for the output
 */
void FecFactory::setOutputFileName ( std::string outputFileName ) {

  FecDeviceFactory::setOutputFileName (outputFileName) ;
  PiaResetFactory::setOutputFileName (outputFileName) ;
}

/** Set a file as the new input, same method than addFecFileName but the previous devices and pia are deleted
 * \param inputFileName - new input file
 */
void FecFactory::setInputFileName ( std::string inputFileName ) throw ( FecExceptionHandler ) {

  FecDeviceFactory::setInputFileName (inputFileName) ;
  PiaResetFactory::setInputFileName (inputFileName) ;
}

/** Set a file as the new input, same method than addFecFileName but the previous devices and pia are deleted
 * This method make a sort on the FecHardwareId
 * \param inputFileName - new input file
 * \param fecHardwareId - FEC hardware ID
 */
void FecFactory::setInputFileName ( std::string inputFileName, std::string fecHardwareId ) throw ( FecExceptionHandler ) {

  FecDeviceFactory::setInputFileName (inputFileName, fecHardwareId) ;
  PiaResetFactory::setInputFileName (inputFileName, fecHardwareId) ;
}

/** return the output file name of the FecDeviceFactory 
 * \return the file name
 */
std::string FecFactory::getOutputFileName ( ) {

  return (FecDeviceFactory::getOutputFileName()) ;
}

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 */
void FecFactory::addFileName ( std::string fileName ) throw ( FecExceptionHandler ) {

  FecDeviceFactory::addFileName (fileName) ;
  PiaResetFactory::addFileName (fileName) ;
}

/** Add a new file name and parse it to retreive the information needed
 * \param fileName - name of the XML file
 */
void FecFactory::addFileName ( std::string fileName, std::string fecHardwareId ) throw ( FecExceptionHandler ) {

  FecDeviceFactory::addFileName (fileName, fecHardwareId) ;
  PiaResetFactory::addFileName (fileName, fecHardwareId) ;
}

/** 
 * \param useIt - use file, mandatory if the database is not set !
 */
void FecFactory::setUsingFile ( bool useIt ) {
  FecDeviceFactory::setUsingFile(useIt) ;
  PiaResetFactory::setUsingFile(useIt) ;
}

// ------------------------------------------------------------------------------------------------------
// 
// Database methods
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** 
 * \param useIt - use the database, the database must be created, if not an exception is raised
 */
void FecFactory::setUsingDb ( bool useIt ) throw (std::string) {
  FecDeviceFactory::setUsingDb(useIt) ;
  PiaResetFactory::setUsingDb(useIt) ;
}

/**
 * \return true if the database is connected and used both for PIA and FEC access
 */
bool FecFactory::getDbUsed ( ) {

  return ( PiaResetFactory::getDbUsed() && FecDeviceFactory::getDbUsed() ) ;
}

/**
 * \return true if the database is connected
 */
bool FecFactory::getDbConnected ( ) {

  return ( PiaResetFactory::getDbConnected() && FecDeviceFactory::getDbConnected() ) ;
}

/** 
 * \param login - login to database
 * \param password - password to database
 * \param path - path to database
 * \warning in the destructor of FecFactory, the dbFecAccess is deleted
 */
void FecFactory::setDatabaseAccess ( ) throw ( oracle::occi::SQLException ) {

  FecDeviceFactory::setDatabaseAccess () ;
  PiaResetFactory::setDatabaseAccess () ;
}

/** 
 * \param login - login to database
 * \param password - password to database
 * \param path - path to database
 * \warning in the destructor of FecFactory, the dbFecAccess is deleted
 */
void FecFactory::setDatabaseAccess ( std::string login, std::string password, std::string path ) throw ( oracle::occi::SQLException ) {

  FecDeviceFactory::setDatabaseAccess (login, password, path ) ;
  PiaResetFactory::setDatabaseAccess (login, password, path ) ;
}

/** 
 * \return FEC database access
 */
DbFecAccess *FecFactory::getDatabaseAccess ( ) {

  return ((DbFecAccess *)FecDeviceFactory::getDatabaseAccess()) ;
}

/** \return the partition name
 */
std::string FecFactory::getPartitionName ( unsigned int partitionId )  throw (oracle::occi::SQLException) {

  return (FecDeviceFactory::getPartitionName (partitionId)) ;
}
#endif

// ------------------------------------------------------------------------------------------------------
// 
// PIA and FEC device upload methods in same file or database
//
// ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

/** \param partitionName - name of the partition
 * \param major - major version returned
 * \param minor - minor version returned
 * \param partitionNumber - partition number returned
 */
void FecFactory::getFecDevicePartitionVersion ( std::string partitionName, unsigned int *major, unsigned int *minor, unsigned int *maskMajor, unsigned int *maskMinor, unsigned int *partitionNumber ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {

  return (FecDeviceFactory::getPartitionVersion ( partitionName, major, minor, maskMajor, maskMinor, partitionNumber )) ;
}

/** \param partitionName - name of the partition
 * \return the information about partition versus version
 */
void FecFactory::setFecDevicePartitionVersion ( std::string partitionName, unsigned int versionMajor, unsigned int versionMinor ) 
  throw (oracle::occi::SQLException, FecExceptionHandler ) {
  
  FecDeviceFactory::setPartitionVersion ( partitionName, versionMajor, versionMinor ) ;
}

/** \param devices - a vector of device description and create a partition for the given modules
 * \param piaDevices - a vector of pia reset
 * \param versionMajor - version set in the database (output)
 * \param versionMinor - version set in the database (output)
 * \param partitionName - partition name to be given
 * \param createPartitionFlag - if true then a new partition is created, if false then the PIA reset and devices are are added to the partitinon
 * \warning the version parameters are set by the database (there are output parameters)
 * \warning the version can be NULL if the output is a file
 * \see XMLFecPiaReset::dbConfigure ( ) and XMLFecDevice::dbConfigure(std::string, std::string)
 */
void FecFactory::createPartition ( deviceVector devices, piaResetVector piaDevices, unsigned int *deviceMajor, unsigned int *deviceMinor, std::string partitionName, bool createPartitionFlag ) 
  throw (FecExceptionHandler) {

  if ( (FecDeviceFactory::getDatabaseAccess() != NULL) &&
       (PiaResetFactory::getDatabaseAccess() != NULL)  &&
       FecDeviceFactory::getDbUsed() &&
       PiaResetFactory::getDbUsed() &&
       (deviceMajor != NULL) && 
       (deviceMinor != NULL) ) {

    //if (!devices.empty())
    FecDeviceFactory::createPartition (devices, deviceMajor, deviceMinor, partitionName, createPartitionFlag) ;

    //if (!piaDevices.empty())
    PiaResetFactory::createPartition (piaDevices, partitionName) ;
      
#ifdef DEBUGMSGERROR
    std::cout << "Create a new FEC devices version:   " << *deviceMajor << "." << *deviceMinor << std::endl ;
#endif
  }
  else
    setFecDevicePiaDescriptions ( devices, piaDevices ) ;
}

/** \param devices - vector of PIA reset description
 * \param versionMajor - major version created
 * \param versionMinor - minor version created
 */
void FecFactory::createPartition ( deviceVector devices, unsigned int *versionMajor, unsigned int *versionMinor, std::string partitionName, bool createPartitionFlag ) 
  throw (FecExceptionHandler) {

  FecDeviceFactory::createPartition (devices, versionMajor, versionMinor, partitionName, createPartitionFlag ) ;
}

/** \param devices - vector of PIA reset description
 * \param partitionName - partition name
 */
void FecFactory::createPartition ( piaResetVector devices, std::string partitionName )
  throw (FecExceptionHandler) {

  PiaResetFactory::createPartition (devices, partitionName ) ;
}

#endif

/** Upload the vector in a new version (for database)
 * \param fecDevices - a vector of device description
 * \param fecPiaDevices - a vector of pia reset
 * \param partitionName - partition name
 * \param versionMajor - value for the version if the database is used as output.
 * \param versionMinor - value for the version if the database is used as input.
 * \warning the version parameters are set by the database (there are output parameters) and the version parameters are given for the FecDeviceFactory not for the PIA reset
 * \warning to use the database both PIA reset factory and FEC device factory must be used, if not a the output file is used
 * \warning Note that devices must exist before made this call. 
 * \warning a new major version is created for the FEC devices
 * \warning if a file is used, the output file name comes from the FecDeviceFactory
 * \See creationPartition (deviceVector, piaResetVector, int *, int *, std::string, std::string)
 */
void FecFactory::setFecDevicePiaDescriptions ( deviceVector devices, piaResetVector piaDevices, std::string partitionName, unsigned int *deviceMajor, unsigned int *deviceMinor ) 
  throw (FecExceptionHandler) {

#ifdef DATABASE
  if ( FecDeviceFactory::getDbUsed() && PiaResetFactory::getDbUsed() && deviceMajor != NULL && deviceMinor != NULL ) {

    // Upload the FEC devices => create a new minor version
    //if (!devices.empty())
    setFecDeviceDescriptions ( devices, partitionName, deviceMajor, deviceMinor ) ; // , true ) ; // for a major version

    // Upload the PIA reset 
    //if (!piaDevices.empty())
    setPiaResetDescriptions ( piaDevices, partitionName ) ;

#ifdef DEBUGMSGERROR
    std::cout << "Create a new FEC devices version:   " << *deviceMajor << "." << *deviceMinor << std::endl ;
#endif
  }
  else
#endif
    setFecDevicePiaDescriptions ( devices, piaDevices ) ;
}

/** Upload the vector in a new version (for database)
 * \param fecDevices - a vector of device description
 * \param fecPiaDevices - a vector of pia reset
 */
void FecFactory::setFecDevicePiaDescriptions ( deviceVector devices, piaResetVector piaDevices ) 
  throw (FecExceptionHandler) {

  if (!devices.empty() && !piaDevices.empty()) {
    
    MemBufOutputSource memBufOS (devices, piaDevices) ; 
#ifdef DEBUGERRORMSG
    std::cout << memBufOS.getOutputBuffer()->str() << std::endl ;
#endif
    XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), FecDeviceFactory::getOutputFileName());
  }
  else {
    
    if (!devices.empty()) setFecDeviceDescriptions (devices) ;
    if (!piaDevices.empty()) setPiaResetDescriptions (piaDevices) ;
  }
}


// ***********************************************************************************************
// Serialisation of the TkRingDescription
// ***********************************************************************************************

/** this method write into a memory to serialise it
 * \param sizeOfAllocation - size of the memory allocated
 * \return pointer to the memory (NULL in case of problem)
 * \warning this method simply produce a memory with an XML buffer. The pointer allocated with new (so use delete to delete it) should be deleted by the caller of this method.
 */
void *FecFactory::writeTo ( std::vector<TkRingDescription *> &rings, unsigned int &sizeOfAllocation ) throw (FecExceptionHandler) {

  std::stringstream buffer ;

  MemBufOutputSource memBuf ( rings, false, true ) ;
  buffer << memBuf.getOutputBuffer()->str() << std::endl ;
  
#ifdef DEBUGMSGERROR
  std::cout << buffer.str() << std::endl ;
#endif

  sizeOfAllocation = buffer.str().length()+1*sizeof(char) ;
  void *memory = malloc (sizeOfAllocation) ;
  memset(memory,0,sizeOfAllocation) ;
  char *buffChar = (char *)memory ;
  memcpy ((void *)buffChar, (void *)buffer.str().c_str(), sizeOfAllocation);

#ifdef DEBUGMSGERROR
  std::cout << buffChar << std::endl ;
#endif

  return (void *)buffChar ;
}

/** this method read from a memory (deserialisation)
 * \param memory - memory pointer
 * \param sizeOfAllocation - size of the buffer
 * \return pointer to the TkRingDescription allocated (NULL in case of problem)
 * \warning memory will not be deleted here
 * \warning it is up the caller to delete the TkRingDescription returned
 * \warning this method is parsing the memory passed based on XML buffer
 */
std::vector<TkRingDescription *> FecFactory::readFrom ( const void *memory ) throw (FecExceptionHandler) {

  //XMLByte *xmlBuffer = new XMLByte[sizeOfAllocation];
  //memcpy((void *)xmlBuffer,memory,sizeOfAllocation) ;

  XMLByte *xmlBuffer = (XMLByte *)memory ;

#ifdef DEBUGMSGERROR
  std::cout << xmlBuffer << std::endl ;
#endif

  XMLFecCcu xmlFecCcu(xmlBuffer,false) ; // false means do not delete the memory
  tkringVector v = xmlFecCcu.getRingFromBuffer() ;

  return v ;
}

