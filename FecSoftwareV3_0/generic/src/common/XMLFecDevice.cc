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

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/
#include "XMLFecDevice.h"

#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>

using namespace XERCES_CPP_NAMESPACE ;

//#define DISPLAY(x) {std::cout << x << std::endl ;}
#ifndef DISPLAY
#  define DISPLAY(x) {}
#endif

#define TK_EXPLICIT_COMPARISON

/**Create an XMLFec dedicated for devices such as Pll, Laserdriver, ApvFec, ApvMux, Philips,...<BR>
 * Call the <I>XMLFec::XMLFec()</I> constructor<BR>
 * Call the method <I>XMLFecDevice::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec()</I>
 * @see <I>XMLFecDevice::init()</I>
 */
XMLFecDevice::XMLFecDevice () throw (FecExceptionHandler) :
  XMLFec () {

  initParameterNames() ;
}

#ifdef DATABASE
/**Creates a XMLFecDevice with access to the database<BR>
 * Call the <I>XMLFec::XMLFec(DbAccess *dbAccess, std::string partitionName)</I> constructor<BR>
 * Call the method <I>XMLFecDevice::init()</I><BR>
 * @param dbAccess - class for the database access
 * @param partitionName - partition name
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec(DbAccess *dbAccess, std::string partitionName)</I>
 * @see <I>XMLFecDevice::init()</I>
 */
XMLFecDevice::XMLFecDevice ( DbFecAccess *dbAccess )  throw (FecExceptionHandler) : 
  XMLFec( (DbAccess *)dbAccess ) {

  initParameterNames() ;
}
#endif

/**Creates a XMLFecDevice from a buffer<BR>
 * Call the <I>XMLFec::XMLFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLFecDevice::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec(const XMLByte* xmlBuffer)</I>
 * @see <I>XMLFecDevice::init()</I>
 */
XMLFecDevice::XMLFecDevice (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLFec( xmlBuffer ){

  initParameterNames() ;
}

/**Creates a XMLFecDevice from a file<BR>
 * Call the <I>XMLFec::XMLFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLFecDevice::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec(std::string xmlFileName)</I>
 * @see <I>XMLFecDevice::init()</I>
 */
XMLFecDevice::XMLFecDevice ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLFec( xmlFileName ) {

  initParameterNames() ;
}

/** Nothing
 */
XMLFecDevice::~XMLFecDevice (){

  apvDescription::deleteParameterNames(apvParameterNames_) ; delete apvParameterNames_ ;
  muxDescription::deleteParameterNames(muxParameterNames_) ; delete muxParameterNames_ ;
  dcuDescription::deleteParameterNames(dcuParameterNames_) ; delete dcuParameterNames_ ;
  laserdriverDescription::deleteParameterNames(laserdriverParameterNames_) ; delete laserdriverParameterNames_ ;
  pllDescription::deleteParameterNames(pllParameterNames_) ; delete pllParameterNames_ ;

#ifdef PRESHOWER
  deltaDescription::deleteParameterNames(deltaParameterNames_); delete deltaParameterNames_ ;
  paceDescription::deleteParameterNames(paceParameterNames_); delete paceParameterNames_ ;
  kchipDescription::deleteParameterNames(kchipParameterNames_); delete kchipParameterNames_ ;
  gohDescription::deleteParameterNames(gohParameterNames_); delete gohParameterNames_ ;
#endif
#ifdef TOTEM
  vfatDescription::deleteParameterNames(vfatParameterNames_); delete vfatParameterNames_ ;
  totemCChipDescription::deleteParameterNames(totemCChipParameterNames_); delete totemCChipParameterNames_ ;
  totemBBDescription::deleteParameterNames(totemBBParameterNames_); delete totemBBParameterNames_ ;
#endif
}

/** To avoid multiple copy of the parameter names for the description
 */
void XMLFecDevice::initParameterNames ( ) {
  
  apvParameterNames_         = apvDescription::getParameterNames() ;
  muxParameterNames_         = muxDescription::getParameterNames() ;
  dcuParameterNames_         = dcuDescription::getParameterNames() ;
  laserdriverParameterNames_ = laserdriverDescription::getParameterNames() ;
  pllParameterNames_         = pllDescription::getParameterNames() ;
#ifdef PRESHOWER
  deltaParameterNames_       = deltaDescription::getParameterNames() ;
  paceParameterNames_        = paceDescription::getParameterNames() ;
  kchipParameterNames_       = kchipDescription::getParameterNames() ;
  gohParameterNames_         = gohDescription::getParameterNames() ;
#endif
#ifdef TOTEM
  vfatParameterNames_        = vfatDescription::getParameterNames() ;
  totemCChipParameterNames_  = totemCChipDescription::getParameterNames() ;
  totemBBParameterNames_     = totemBBDescription::getParameterNames() ;
#endif
}

/**
 * Empty and clear the dVector
 */
void XMLFecDevice::clearVector(){

  while (dVector_.size() > 0) {
    delete dVector_.back();
    dVector_.pop_back();
  }
  dVector_.clear() ;
}

/**Get the device Vector
 * @return the deviceVector <I>dVector_</I> attribute
 */
deviceVector XMLFecDevice::getDeviceVector () {
  return dVector_;
}

/**Set the device Vector
 * Empty the <I>dVector_</I> attribute
 * Delete all his elements
 * Copy all elements from <I>devices</I> parameter to <I>dVector_</I> attribute
 * @exception FecExceptionHandler : a FecExceptionHandler is raised, if <I>dVector_</I> is NULL
 */
void XMLFecDevice::setDeviceVector (deviceVector devices) throw (FecExceptionHandler) {
  // empty and delete previous deviceVector
  // delete all elements
  clearVector() ;

  if (devices.size()) {
    // set the deviceVector parameter as the deviceVector attribute
    for (deviceVector::iterator device = devices.begin() ; device != devices.end() ; device ++) {
      dVector_.push_back (*device) ;
    }
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  }
}

/**Parses the <I>xmlInputSource_</I> attribute using the SAX interface<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLFecDevice::parseXMLBuffer()
 */
deviceVector XMLFecDevice::getDevices () throw (FecExceptionHandler) {

  parseXMLBuffer();

  return dVector_;
}

#ifdef DATABASE

/**Send a request to the database with partition name as parameter, for the current version.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
deviceVector XMLFecDevice::getDevices (std::string partitionName) throw (FecExceptionHandler) {

  //unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseDatabaseResponse(partitionName);

  //unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "ParseDatabaseResponse = " << (endMillis-startMillis) << " ms" << std::endl ;

  //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseXMLBuffer();
  //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "parseXMLBuffer = " << (endMillis-startMillis) << " ms" << std::endl ;

  return dVector_;
}

/**Send a request to the database with partition name as parameter, for the a given version.<BR>
 * The result is converted as  <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param versionMajor - major version
 * @param versionMinor - minor version
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
deviceVector XMLFecDevice::getDevices (std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int pMaskVersionMajor, unsigned int pMaskVersionMinor) throw (FecExceptionHandler) {

  parseDatabaseResponse(partitionName, versionMajor, versionMinor, pMaskVersionMajor, pMaskVersionMinor);
  parseXMLBuffer();

  return dVector_;
}

/**Send a request to the database with Fec hardware identifier as parameter, for the current version.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
deviceVector XMLFecDevice::getDevices (std::string partitionName, std::string fecHardId) throw (FecExceptionHandler) {

  //unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseDatabaseResponse(partitionName, fecHardId);
  //unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "ParseDatabaseResponse = " << (endMillis-startMillis) << " ms" << std::endl ;

  //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseXMLBuffer();
  //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "parseXMLBuffer = " << (endMillis-startMillis) << " ms" << std::endl ;

  return dVector_;
}

/**Send a request to the database with Fec hardware identifier as parameter, for the a given version.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @param versionMajor - major version
 * @param versionMinor - minor version
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId, unsigned int versionMajor, unsigned int versionMinor)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
deviceVector XMLFecDevice::getDevices (std::string partitionName, std::string fecHardId, unsigned int versionMajor, unsigned int versionMinor) throw (FecExceptionHandler) {

  parseDatabaseResponse(partitionName, fecHardId, versionMajor, versionMinor);
  parseXMLBuffer();

  return dVector_;
}
#endif

/**Generates an XML buffer from the parameter <I>dVector</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param dVector - device list
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if
 *     - <I>dVector</I> is not initialized
 *     - the MemBufOutputSource created from the parameter <I>dVector</I> is not initialized 
 * @see XMLCommonFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 */
void XMLFecDevice::setDevices ( deviceVector dVector, std::string outputFileName) throw (FecExceptionHandler) {

  if (dVector.size()) {
    MemBufOutputSource memBufOS(dVector);
    XMLFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>dVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLFecDevice::setDevices ( deviceVector *dVector, std::string outputFileName)
 */
void XMLFecDevice::setDevices ( std::string outputFileName ) throw (FecExceptionHandler) {
  setDevices( dVector_, outputFileName);
}

/**Displays the contents of the attribute <I>dVector_</I>.
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if <I>dVector</I> is not initialized
 */
void XMLFecDevice::display() throw (FecExceptionHandler) {
  if (dVector_.size()) {
    for (deviceVector::iterator it = dVector_.begin() ; it != dVector_.end() ; it ++) {
      deviceDescription *deviced = *it;
      deviced->display();
    }
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  }
}

#ifdef DATABASE
/**Generates an XML buffer from the parameter <I>dVector</I><BR>.
 * Sends this buffer to the database as version <I>versionMajorId.versionMinorId</I>
 * @param dVector - device description vector to be stored to the database
 * @param partitionName - partition name
 * @param versionMajorId - version major
 * @param versionMinorId - version minor
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 * @see DbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
 */
void XMLFecDevice::setDevices (deviceVector dVector, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId)  throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS (dVector, true);
  std::string *xmlBuffer = NULL ;
  try {
    if (dataBaseAccess_){
      xmlBuffer = new std::string[5];
      xmlBuffer[0] = (memBufOS.getPllOutputBuffer())->str();
      xmlBuffer[1] = (memBufOS.getLaserdriverOutputBuffer())->str();
      xmlBuffer[2] = (memBufOS.getApvFecOutputBuffer())->str();
      xmlBuffer[3] = (memBufOS.getApvMuxOutputBuffer())->str();
      xmlBuffer[4] = (memBufOS.getDcuOutputBuffer())->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbFecAccess *)dataBaseAccess_)->setXMLClobWithVersion(xmlBuffer, partitionName, versionMajorId, versionMinorId);
      delete[] xmlBuffer;
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    if (xmlBuffer != NULL) delete[] xmlBuffer;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG, e), ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>dVector_</I>.<BR>
 * Sends this buffer to the database as version <I>versionMajorId.versionMinorId</I>.<BR>
 * @param partitionName - partition name
 * @param versionMajorId - version major
 * @param versionMinorId - version minor
 * @exception FecExceptionHandler
 * @see XMLFecDevice::setDevices (deviceVector *dVector, unsigned int versionMajorId, unsigned int versionMinorId)
 */
void XMLFecDevice::setDevices(std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (FecExceptionHandler) {
  setDevices(dVector_, partitionName, versionMajorId, versionMinorId);
}

/**Generates an XML buffer from the parameter <I>dVector</I><BR>.
 * Sends this buffer to the database as version <I>versionMajorId.versionMinorId</I>
 * The data are stored in the version number :
 *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
 *   - nextMajorVersion.0 if versionUpdate = 1
 *   - currentMajorVersion.0 if versionUpdate = 2
 *   - 0.nextMinor if versionUpdate = 3
 * @param dVector - device description vector to be stored to the database
 * @param partitionName - partition name
 * @param versionUpdate - select the version to be used 
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 * @see DbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
 */
void XMLFecDevice::setDevices (deviceVector dVector, std::string partitionName, unsigned int versionUpdate)  throw (FecExceptionHandler) {

  //unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  MemBufOutputSource memBufOS (dVector, true);
  //unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "MemBufOutputSource = " << (endMillis-startMillis) << " ms" << std::endl ;

  try {
    if (dataBaseAccess_){
      std::string *xmlBuffer = new std::string[5];
      xmlBuffer[0] = (memBufOS.getPllOutputBuffer())->str();
      xmlBuffer[1] = (memBufOS.getLaserdriverOutputBuffer())->str();
      xmlBuffer[2] = (memBufOS.getApvFecOutputBuffer())->str();
      xmlBuffer[3] = (memBufOS.getApvMuxOutputBuffer())->str();
      xmlBuffer[4] = (memBufOS.getDcuOutputBuffer())->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer[0] << std::endl;
      std::cout << xmlBuffer[1] << std::endl;
      std::cout << xmlBuffer[2] << std::endl;
      std::cout << xmlBuffer[3] << std::endl;
      std::cout << xmlBuffer[4] << std::endl;
#endif
      //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      ((DbFecAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer, partitionName, versionUpdate);
      //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      //std::cout << "setXMLClob = " << (endMillis-startMillis) << " ms" << std::endl ;
      delete[] xmlBuffer;
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    std::stringstream errorMessage ; errorMessage << e.what();
    std::string localMessage = dataBaseAccess_->getErrorMessage();
    if (localMessage.size()) errorMessage << std::endl << localMessage ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + errorMessage.str(), ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>dVector_</I>.<BR>
 * Sends this buffer to the database as version <I>versionMajorId.versionMinorId</I>.<BR>
 * The data are stored in the version number :
 *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
 *   - nextMajorVersion.0 if versionUpdate = 1
 *   - currentMajorVersion.0 if versionUpdate = 2
 * @param partitionName - partition name
 * @param versionUpdate - select the version to be used
 * @exception FecExceptionHandler
 * @see XMLFecDevice::setDevices (deviceVector *dVector, std::string partitionName, unsigned int versionUpdate)
 */
void XMLFecDevice::setDevices(std::string partitionName, unsigned int versionUpdate) throw (FecExceptionHandler) {
  setDevices(dVector_, partitionName, versionUpdate);
}

/**Configure the database : create device parameters into the database.<BR>
 * Create a new state from the last current state with new partition ... this new state is set as current state...<BR>
 * @param partitionName - name of the partition
 * @param newPartition - if <I>true</I> a new partition is created with <I>partitionName</I> as name, if <I>false</I> device parameters are stored in the already created partition <I>partitionName</I>
 * @return new Fec version major id
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
*/
// unsigned int XMLFecDevice::dbConfigure(std::string partitionName, boolean newPartition ) throw (FecExceptionHandler) {
//   unsigned int returnedVersionMajorId = 0;
//   MemBufOutputSource memBufOS (dVector_);

// #ifdef DATABASEDEBUG
//   std::cout << "create a new partition ? " << newPartition << std::endl;
//   std::cout << "partitionName : " << partitionName << std::endl;
// #endif
  
//   try {
//     if (dataBaseAccess_){
//       std::string xmlBuffer = (memBufOS.getOutputBuffer())->str();
// #ifdef DATABASEDEBUG
//       std::cout << xmlBuffer << std::endl;
// #endif
//       returnedVersionMajorId = ((DbFecAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer, partitionName, newPartition);
//     } else {
//       RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
//     }
//   } catch (oracle::occi::SQLException &e) {
//     std::stringstream errorMessage ; errorMessage << e.what();
//     std::string localMessage = dataBaseAccess_->getErrorMessage();
//     if (localMessage.size()) errorMessage << std::endl << localMessage ;
//     RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + errorMessage.str(), ERRORCODE) ;
//   }
//   return returnedVersionMajorId;
// }

/**Configure the database : create device parameters into the database.<BR>
 * Create a new state from the last current state with new partition ... this new state is set as current state...<BR>
 * @param partitionName - name of the partition
 * @param newPartition - if <I>true</I> a new partition is created with <I>partitionName</I> as name, if <I>false</I> device parameters are stored in the already created partition <I>partitionName</I>
 * @return new Fec version major id
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
*/
unsigned int XMLFecDevice::dbConfigure(std::string partitionName, boolean newPartition ) throw (FecExceptionHandler) {
  unsigned int returnedVersionMajorId = 0;
  MemBufOutputSource memBufOS (dVector_, true);
 
#ifdef DATABASEDEBUG
  std::cout << "XMLFecDevice::newDbConfigure create a new partition ? " << newPartition << std::endl;
  std::cout << "XMLFecDevice::newDbConfigure partitionName : " << partitionName << std::endl;
#endif

  std::string *xmlBuffer = NULL ;
  try {
    if (dataBaseAccess_){
      xmlBuffer = new std::string[5];
      xmlBuffer[0] = (memBufOS.getPllOutputBuffer())->str();
      xmlBuffer[1] = (memBufOS.getLaserdriverOutputBuffer())->str();
      xmlBuffer[2] = (memBufOS.getApvFecOutputBuffer())->str();
      xmlBuffer[3] = (memBufOS.getApvMuxOutputBuffer())->str();
      xmlBuffer[4] = (memBufOS.getDcuOutputBuffer())->str();

#ifdef DATABASEDEBUG
      std::cout << "----------------------------------------" << std::endl ;
      std::cout << xmlBuffer[0] << std::endl;
      std::cout << "----------------------------------------" << std::endl ;
      std::cout << xmlBuffer[1] << std::endl;
      std::cout << "----------------------------------------" << std::endl ;
      std::cout << xmlBuffer[2] << std::endl;
      std::cout << "----------------------------------------" << std::endl ;
      std::cout << xmlBuffer[3] << std::endl;
      std::cout << "----------------------------------------" << std::endl ;
      std::cout << xmlBuffer[4] << std::endl;
      std::cout << "----------------------------------------" << std::endl ;
#endif

      returnedVersionMajorId = ((DbFecAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer, partitionName, newPartition);
      delete[] xmlBuffer;
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {

    if (xmlBuffer != NULL) delete[] xmlBuffer;
    std::stringstream errorMessage ; errorMessage << e.what();
    std::string localMessage = dataBaseAccess_->getErrorMessage();
    //if (localMessage.size()) errorMessage << std::endl << localMessage ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + ": " + errorMessage.str(), ERRORCODE) ;
  }
  return returnedVersionMajorId;
}

#endif

// ---------------------------------------------------------------------------------------------
// Parser
// ---------------------------------------------------------------------------------------------

/** Parse the elements for a FEC buffer
 * \param DOMNode - DOM buffer (DOMDocument or DOMNode)
 */
unsigned int XMLFecDevice::parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {

      char *name = XMLString::transcode(n->getNodeName());
      //std::cout <<"----------------------------------------------------------"<<std::endl;
      //std::cout <<"Encountered Element : "<< name << std::endl;

      if(n->hasAttributes()) {

#ifdef TK_EXPLICIT_COMPARISON
	int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0, vmeControllerDaisyChainId = 0 ;
	char fecHardwareId[100] = "0" ;
	bool enabled = true ;
	int apvMode = 0, latency = 0, muxGain = 0, ipre = 0, ipcasc = 0, ipsf = 0 ;   // APV25
	int isha = 0, issf = 0, ipsp = 0, imuxin = 0, ical = 0, ispare = 0, vfp = 0 ; // APV25
	int vfs = 0, vpsp = 0, cdrv = 0, csel = 0, apvError = 0 ;                     // APV25
	int resistor = 0 ;                                                            // APV MUX
	unsigned int timeStamp = 0, dcuHardId = 0 ;                                   // DCU
	int channel0 = 0, channel1 = 0, channel2 = 0, channel3 = 0, channel4 = 0 ;    // DCU
	int channel5 = 0, channel6 = 0, channel7 = 0 ;                                // DCU
	char dcuType[100] ;                                                           // DCU
	bool dcuEnabled = true ;                                                      // DCU
	int bias0 = 0, bias1 = 0, bias2 = 0, gain0 = 0, gain1 = 0, gain2 = 0 ;        // Laserdriver (AOH or DOH)
	int delayFine = 0, delayCoarse = 0, pllDac = 255 ;                            // PLL
#endif

	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- APV25
	if (!strcmp(name,"APV25")) {

#ifdef TK_EXPLICIT_COMPARISON
	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();

	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
	    //std::cout << "\t" << name << "/" << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ;} 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::APVMODE])) { val ++ ;  sscanf (value,"%d",&apvMode) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::APVLATENCY])) { val ++ ;  sscanf (value,"%d",&latency) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::MUXGAIN])) { val ++ ;  sscanf (value,"%d",&muxGain) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IPRE])) { val ++ ;  sscanf (value,"%d",&ipre) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IPCASC])) { val ++ ;  sscanf (value,"%d",&ipcasc) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IPSF])) { val ++ ;  sscanf (value,"%d",&ipsf) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::ISHA])) { val ++ ;  sscanf (value,"%d",&isha) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::ISSF])) { val ++ ;  sscanf (value,"%d",&issf) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IPSP])) { val ++ ;  sscanf (value,"%d",&ipsp) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IMUXIN])) { val ++ ;  sscanf (value,"%d",&imuxin) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::ICAL])) { val ++ ;  sscanf (value,"%d",&ical) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::ISPARE])) { val ++ ;  sscanf (value,"%d",&ispare) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::VFP])) { val ++ ;  sscanf (value,"%d",&vfp) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::VFS])) { val ++ ;  sscanf (value,"%d",&vfs) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::VPSP])) { val ++ ;  sscanf (value,"%d",&vpsp) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::CDRV])) { val ++ ;  sscanf (value,"%d",&cdrv) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::CSEL])) { val ++ ;  sscanf (value,"%d",&csel) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::APVERROR])) { val ++ ;  sscanf (value,"%d",&apvError) ; }
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 26) std::cerr << "APV25: invalid number of parameters: " << std::dec << val << "/26" << std::endl ;

	  apvDescription *device = new apvDescription(buildCompleteKey(fec,ring,ccu,channel,address),
						      apvMode, latency, muxGain, ipre, ipcasc, ipsf,
						      isha, issf, ipsp, imuxin, ical, ispare, vfp,
						      vfs, vpsp, cdrv, csel,apvError) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(apvParameterNames_,pAttributes) ;
	  if ( (val != 26) && (val != 27) ) std::cerr << "APV: invalid number of parameters: " << std::dec << val << "/" << 26 << std::endl ;
	  apvDescription *device = new apvDescription (*apvParameterNames_) ;
#endif

	  dVector_.push_back(device) ;
	  countAPV25 ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- APVMUX
	else if (!strcmp(name,"APVMUX")) {

#ifdef TK_EXPLICIT_COMPARISON
	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,muxDescription::MUXPARAMETERNAMES[muxDescription::RESISTOR])) { val ++ ;  sscanf (value,"%d",&resistor) ; }
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 9) std::cerr << "APVMUX: invalid number of parameters: " << std::dec << val << "/9" << std::endl ;
	    
	  muxDescription *device = new muxDescription(buildCompleteKey(fec,ring,ccu,channel,address),resistor) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(muxParameterNames_,pAttributes) ;
	  if ( (val != 9) && (val != 10) ) std::cerr << "MUX: invalid number of parameters: " << std::dec << val << "/" << 9 << std::endl ;
	  muxDescription *device = new muxDescription (*muxParameterNames_) ;
#endif

	  dVector_.push_back(device) ;	    
	  countAPVMUX ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- DCU
	else if (!strcmp(name,"DCU")) {

#ifdef TK_EXPLICIT_COMPARISON
	  unsigned int val = 0 ;
	    
	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
	    //std::cout << "\t" << name << "=";
	      
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; DISPLAY("FEC hard ID"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; DISPLAY("crate"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; DISPLAY("fec"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; DISPLAY("ring"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; DISPLAY("ccu"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; DISPLAY("channel"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; DISPLAY("address"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; DISPLAY("enabled"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUTIMESTAMP])) { val ++ ;  sscanf (value,"%u",&timeStamp) ; DISPLAY("timeStamp"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUHARDID])) { val ++ ;  sscanf (value,"%u",&dcuHardId) ; DISPLAY("dcuHardId"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL0])) { val ++ ;  sscanf (value,"%d",&channel0) ; DISPLAY("channel0"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL1])) { val ++ ;  sscanf (value,"%d",&channel1) ; DISPLAY("channel1"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL2])) { val ++ ;  sscanf (value,"%d",&channel2) ; DISPLAY("channel2"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL3])) { val ++ ;  sscanf (value,"%d",&channel3) ; DISPLAY("channel3"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL4])) { val ++ ;  sscanf (value,"%d",&channel4) ; DISPLAY("channel4"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL5])) { val ++ ;  sscanf (value,"%d",&channel5) ; DISPLAY("channel5"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL6])) { val ++ ;  sscanf (value,"%d",&channel6) ; DISPLAY("channel6"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL7])) { val ++ ;  sscanf (value,"%d",&channel7) ; DISPLAY("channel7"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::EDCUTYPE])) { val ++ ;  strncpy(dcuType,value,100) ; DISPLAY("dcuType"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUREADOUTENABLED])) { val ++ ; if (value[0] == 'F') dcuEnabled = false ; DISPLAY("dcuEnabled");}
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	      
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }
	      
	  if (val != 20) std::cerr << "DCU: invalid number of parameters: " << std::dec << val << "/20" << std::endl ;
	    
	  dcuDescription *device = new dcuDescription(buildCompleteKey(fec,ring,ccu,channel,address),timeStamp, dcuHardId,
						      channel0, channel1, channel2, channel3, channel4, channel5, 
						      channel6, channel7, dcuType) ;
	  device->setEnabled(enabled) ;
	  device->setDcuReadoutEnabled(dcuEnabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(dcuParameterNames_,pAttributes) ;
	  if ( (val != 20) && (val != 21) ) std::cerr << "DCU: invalid number of parameters: " << std::dec << val << "/" << 20 << std::endl ;
	  dcuDescription *device = new dcuDescription (*dcuParameterNames_) ;
#endif

	  dVector_.push_back(device) ;	    
	  if (device->getDcuType() == DCUCCU) countDCUCCU ++ ;
	  else if (device->getDcuType() == DCUFEH) countDCUFEH ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- Laserdriver
	else if (!strcmp(name,"LASERDRIVER")) {

#ifdef TK_EXPLICIT_COMPARISON
	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS0])) { val ++ ;  sscanf (value,"%d",&bias0) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS1])) { val ++ ;  sscanf (value,"%d",&bias1) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS2])) { val ++ ;  sscanf (value,"%d",&bias2) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN0])) { val ++ ;  sscanf (value,"%d",&gain0) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN1])) { val ++ ;  sscanf (value,"%d",&gain1) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN2])) { val ++ ;  sscanf (value,"%d",&gain2) ; }
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 14) std::cerr << "Laserdriver: invalid number of parameters: " << std::dec << val << "/14" << std::endl ;

	  laserdriverDescription *device = new laserdriverDescription(buildCompleteKey(fec,ring,ccu,channel,address),
								      bias0, bias1, bias2, gain0, gain1, gain2) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(laserdriverParameterNames_,pAttributes) ;
	  if ( (val != 14) && (val != 15) ) std::cerr << "Laserdriver: invalid number of parameters: " << std::dec << val << "/" << 14 << std::endl ;
	  laserdriverDescription *device = new laserdriverDescription (*laserdriverParameterNames_) ;
#endif

	  dVector_.push_back(device) ;
	  if (device->getDeviceType() == LASERDRIVER) countAOH ++ ;
	  else if (device->getDeviceType() == DOH) countDOH ++ ;
	  else std::cerr << "Unknow device type, expecting laserdriver/DOH: " << device->getDeviceType() << std::endl ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- PLL
	else if (!strcmp(name,"PLL")) {

#ifdef TK_EXPLICIT_COMPARISON
	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; DISPLAY("FEC hard ID"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; DISPLAY("crate"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; DISPLAY("fec"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; DISPLAY("ring"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; DISPLAY("ccu"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; DISPLAY("channel"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; DISPLAY("address"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; DISPLAY("enabled"); }
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYFINE])) { val ++ ;  sscanf (value,"%d",&delayFine) ; DISPLAY("delayFine"); }
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYCOARSE])) { val ++ ;  sscanf (value,"%d",&delayCoarse) ; DISPLAY("delayCoarse");}
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::PLLDAC])) { val ++ ;  sscanf (value,"%d",&pllDac) ; DISPLAY("pllDac"); }
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 11) std::cerr << "PLL: invalid number of parameters: " << std::dec << val << "/11" << std::endl ;

	  pllDescription *device = new pllDescription(buildCompleteKey(fec,ring,ccu,channel,address),delayFine,delayCoarse,pllDac) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(pllParameterNames_,pAttributes) ;
	  if ( (val != 11) && (val != 12) ) std::cerr << "PLL: invalid number of parameters: " << std::dec << val << "/" << 11 << std::endl ;
	  pllDescription *device = new pllDescription (*pllParameterNames_) ;
#endif

	  dVector_.push_back(device) ;
	  countPLL ++ ;
	}
#ifdef PRESHOWER
	// *************************************************************************************
	// Please note that the explicit comparison done as the Tracker devices has a performance
	// improvement between 20 to 40% of the time
	// no counter has been added
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- DELTA
	else if (!strcmp(name,"DELTA")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(deltaParameterNames_,pAttributes) ;
	  if ( (val != DELTA_DESC_PAR_NUM) && (val != (DELTA_DESC_PAR_NUM+1)) ) std::cerr << "Delta description: invalid number of parameters: " << std::dec << val << "/" << DELTA_DESC_PAR_NUM << std::endl ;
	  deltaDescription *delta = new deltaDescription (*deltaParameterNames_) ;

	  dVector_.push_back(delta) ;
	}
	else if (!strcmp(name,"PACEAM")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(paceParameterNames_,pAttributes) ;
	  if ( (val != PACE_DESC_PAR_NUM) && (val != (PACE_DESC_PAR_NUM+1)) ) std::cerr << "Pace description: invalid number of parameters: " << std::dec << val << "/" << PACE_DESC_PAR_NUM << std::endl ;
	  paceDescription *pace = new paceDescription (*paceParameterNames_) ;
	  dVector_.push_back(pace) ;
	}
	else if (!strcmp(name,"KCHIP")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(kchipParameterNames_,pAttributes) ;
	  if ( (val != KCHIP_DESC_PAR_NUM) && (val != (KCHIP_DESC_PAR_NUM+1)) ) std::cerr << "Kchip description: invalid number of parameters: " << std::dec << val << "/" << KCHIP_DESC_PAR_NUM << std::endl ;
	  kchipDescription *kchip = new kchipDescription (*kchipParameterNames_) ;
	  dVector_.push_back(kchip) ;
	}
	else if (!strcmp(name,"GOH")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(gohParameterNames_,pAttributes) ;
	  if (val != GOH_DESC_PAR_NUM) std::cerr << "Goh description: invalid number of parameters: " << std::dec << val << "/" << GOH_DESC_PAR_NUM << std::endl ;
	  gohDescription *goh = new gohDescription (*gohParameterNames_) ;
	  dVector_.push_back(goh) ;
	}
#endif
#ifdef TOTEM
	// *************************************************************************************
	// Please note that the explicit comparison done as the Tracker devices has a performance
	// improvement between 20 to 40% of the time
	// no counter has been added
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- VFAT
	else if (!strcmp(name,"VFAT")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(vfatParameterNames_,pAttributes) ;
	  if ( (val != 149) && (val != 150) ) std::cerr << "Vfat description: invalid number of parameters: " << std::dec << val << "/" << 149 << std::endl ;
	  vfatDescription *vfat = new vfatDescription (*vfatParameterNames_) ;
	  dVector_.push_back(vfat) ;
	}
	else if (!strcmp(name,"CCHIP")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(totemCChipParameterNames_,pAttributes) ;
	  if ( (val != 149) && (val != 150) ) std::cerr << "Cchip description: invalid number of parameters: " << std::dec << val << "/" << 149 << std::endl ;
	  totemCChipDescription *cchip = new totemCChipDescription (*totemCChipParameterNames_) ;
	  dVector_.push_back(cchip) ;
	}
	else if (!strcmp(name,"TBB")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(totemBBParameterNames_,pAttributes) ;
	  if ( (val != 5) && (val != 6) ) std::cerr << "Tbb description: invalid number of parameters: " << std::dec << val << "/" << 5 << std::endl ;
	  totemBBDescription *tbb = new totemBBDescription (*totemBBParameterNames_) ;
	  dVector_.push_back(tbb) ;
	}
#endif
      }

      // Release name
      XMLString::release(&name);

      ++count;
    }

    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      count += parseAttributes(child) ;
  }

  return count;
}
