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
#include "XMLESFecDevice.h"

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

#ifndef DISPLAY
# define DISPLAY(x){}
#endif

#define ES_EXPLICIT_COMPARISON
/**Create an XMLESFec dedicated for devices such as Pll, Laserdriver, ApvFec, ApvMux, Philips,...<BR>
 * Call the <I>XMLESFec::XMLESFec()</I> constructor<BR>
 * Call the method <I>XMLESFecDevice::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::XMLESFec()</I>
 * @see <I>XMLESFecDevice::init()</I>
 */
XMLESFecDevice::XMLESFecDevice () throw (FecExceptionHandler) :
  XMLESFec () 
{
	initParameterNames(); 

}

#ifdef DATABASE
/**Creates a XMLESFecDevice with access to the database<BR>
 * Call the <I>XMLESFec::XMLESFec(ESDbAccess *dbAccess, std::string partitionName)</I> constructor<BR>
 * Call the method <I>XMLESFecDevice::init()</I><BR>
 * @param dbAccess - class for the database access
 * @param partitionName - partition name
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::XMLESFec(ESDbAccess *dbAccess, std::string partitionName)</I>
 * @see <I>XMLESFecDevice::init()</I>
 */
XMLESFecDevice::XMLESFecDevice ( ESDbFecAccess *dbAccess )  throw (FecExceptionHandler) : 
  XMLESFec( (ESDbAccess *)dbAccess )
{
	initParameterNames();
}
#endif

/**Creates a XMLESFecDevice from a buffer<BR>
 * Call the <I>XMLESFec::XMLESFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLESFecDevice::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::XMLESFec(const XMLByte* xmlBuffer)</I>
 * @see <I>XMLESFecDevice::init()</I>
 */
XMLESFecDevice::XMLESFecDevice (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLESFec( xmlBuffer ){
  
  initParameterNames();
}

/**Creates a XMLESFecDevice from a file<BR>
 * Call the <I>XMLESFec::XMLESFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLESFecDevice::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::XMLESFec(std::string xmlFileName)</I>
 * @see <I>XMLESFecDevice::init()</I>
 */
XMLESFecDevice::XMLESFecDevice ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLESFec( xmlFileName ) {
  
  initParameterNames();
}

/** Nothing
 */
XMLESFecDevice::~XMLESFecDevice (){

  deltaDescription::deleteParameterNames(deltaParameterNames_); delete deltaParameterNames_ ;
  paceDescription::deleteParameterNames(paceParameterNames_); delete paceParameterNames_ ;
  kchipDescription::deleteParameterNames(kchipParameterNames_); delete kchipParameterNames_ ;
  dcuDescription::deleteParameterNames(dcuParameterNames_); delete dcuParameterNames_;
  gohDescription::deleteParameterNames(gohParameterNames_); delete gohParameterNames_ ;
}


/** To avoid multiple copy of the parameter names for the description
 */
void XMLESFecDevice::initParameterNames(){
	
	deltaParameterNames_       = deltaDescription::getParameterNames() ;
  paceParameterNames_        = paceDescription::getParameterNames() ;
  kchipParameterNames_       = kchipDescription::getParameterNames() ;
  dcuParameterNames_				 = dcuDescription::getParameterNames();
  gohParameterNames_         = gohDescription::getParameterNames() ;
}

/**
 * Empty and clear the dVector
 */
void XMLESFecDevice::clearVector(){
	
	while (dVector_.size() > 0){
		delete dVector_.back();
		dVector_.pop_back();
	}
	dVector_.clear();
}


/**Get the device Vector
 * @return the deviceVector <I>dVector_</I> attribute
 */
deviceVector XMLESFecDevice::getDeviceVector () {
  return dVector_;
}

/**Set the device Vector
 * Empty the <I>dVector_</I> attribute
 * Delete all his elements
 * Copy all elements from <I>devices</I> parameter to <I>dVector_</I> attribute
 * @exception FecExceptionHandler : a FecExceptionHandler is raised, if <I>dVector_</I> is NULL
 */
void XMLESFecDevice::setDeviceVector (deviceVector devices) throw (FecExceptionHandler) {
  // empty and delete previous deviceVector
  // delete all elements
  clearVector();
  
  if (devices.size()){
  	//set the deviceVector parameter as the deviceVector attribute
  	for (deviceVector::iterator device = devices.begin(); device != devices.end(); device ++){
  		dVector_.push_back(*device);
  	}
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  }
}

/**Parses the <I>xmlInputSource_</I> attribute using the SAX interface<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLESFecDevice::parseXMLBuffer()
 */
deviceVector XMLESFecDevice::getDevices () throw (FecExceptionHandler) {

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
 * @see <I>XMLESFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
deviceVector XMLESFecDevice::getDevices (std::string partitionName) throw (FecExceptionHandler) {

  parseDatabaseResponse(partitionName);

  parseXMLBuffer();

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
 * @see <I>XMLESFec::parseDatabaseResponse(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
deviceVector XMLESFecDevice::getDevices (std::string partitionName, unsigned int versionMajor, unsigned int versionMinor) throw (FecExceptionHandler) {

  parseDatabaseResponse(partitionName, versionMajor, versionMinor);
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
 * @see <I>XMLESFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
deviceVector XMLESFecDevice::getDevices (std::string partitionName, std::string fecHardId) throw (FecExceptionHandler) {

  parseDatabaseResponse(partitionName, fecHardId);

  parseXMLBuffer();

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
 * @see <I>XMLESFec::parseDatabaseResponse(unsigned int fecHardId, unsigned int versionMajor, unsigned int versionMinor)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
deviceVector XMLESFecDevice::getDevices (std::string partitionName, std::string fecHardId, unsigned int versionMajor, unsigned int versionMinor) throw (FecExceptionHandler) {

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
 * @see XMLESFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 */
void XMLESFecDevice::setDevices ( deviceVector dVector, std::string outputFileName) throw (FecExceptionHandler) {

  if (dVector.size()) {
    esMemBufOutputSource memBufOS(dVector);
    XMLESFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded", ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>dVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLESFecDevice::setDevices ( deviceVector *dVector, std::string outputFileName)
 */
void XMLESFecDevice::setDevices ( std::string outputFileName ) throw (FecExceptionHandler) {
  setDevices( dVector_, outputFileName);
}

/**Displays the contents of the attribute <I>dVector_</I>.
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if <I>dVector</I> is not initialized
 */
void XMLESFecDevice::display() throw (FecExceptionHandler) {
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
 * @see ESDbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
 */
void XMLESFecDevice::setDevices (deviceVector dVector, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId)  throw (FecExceptionHandler) {
  
  esMemBufOutputSource memBufOS(dVector, true);
  
  try {
    if (dataBaseAccess_){
      std::string *xmlBuffer = new std::string[5];
      xmlBuffer[0] = (memBufOS.getPllOutputBuffer())->str();
      xmlBuffer[1] = (memBufOS.getKchipOutputBuffer())->str();
      xmlBuffer[2] = (memBufOS.getDeltaOutputBuffer())->str();
      xmlBuffer[3] = (memBufOS.getPaceOutputBuffer())->str();
      xmlBuffer[4] = (memBufOS.getDcuOutputBuffer())->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer << std::endl;
#endif
      ((ESDbFecAccess *)dataBaseAccess_)->setXMLClobWithVersion(xmlBuffer, partitionName, versionMajorId, versionMinorId);
      delete[] xmlBuffer;
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    std::stringstream errorMessage; errorMessage << e.what();
    std::string localMessage = dataBaseAccess_->getErrorMessage();
    if (localMessage.size()) errorMessage << std::endl << localMessage;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + errorMessage.str(), ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>dVector_</I>.<BR>
 * Sends this buffer to the database as version <I>versionMajorId.versionMinorId</I>.<BR>
 * @param partitionName - partition name
 * @param versionMajorId - version major
 * @param versionMinorId - version minor
 * @exception FecExceptionHandler
 * @see XMLESFecDevice::setDevices (deviceVector *dVector, unsigned int versionMajorId, unsigned int versionMinorId)
 */
void XMLESFecDevice::setDevices(std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (FecExceptionHandler) {
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
 * @see ESDbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
 */
void XMLESFecDevice::setDevices (deviceVector dVector, std::string partitionName, unsigned int versionUpdate)  throw (FecExceptionHandler) {
  esMemBufOutputSource memBufOS(dVector, true);
  
  try {
    if (dataBaseAccess_){
      std::string *xmlBuffer = new std::string[5];
      xmlBuffer[0] = (memBufOS.getPllOutputBuffer())->str();
      xmlBuffer[1] = (memBufOS.getKchipOutputBuffer())->str();
      xmlBuffer[2] = (memBufOS.getDeltaOutputBuffer())->str();
      xmlBuffer[3] = (memBufOS.getPaceOutputBuffer())->str();
      xmlBuffer[4] = (memBufOS.getDcuOutputBuffer())->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer[0] << std::endl;
      std::cout << xmlBuffer[1] << std::endl;
      std::cout << xmlBuffer[2] << std::endl;
      std::cout << xmlBuffer[3] << std::endl;
      std::cout << xmlBuffer[4] << std::endl;
#endif
      ((ESDbFecAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer, partitionName, versionUpdate);
      delete[] xmlBuffer;
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    std::stringstream errorMessage ; errorMessage << e.what();
    std::string localMessage = dataBaseAccess_->getErrorMessage();
    if (localMessage.size()) errorMessage << std::endl << localMessage;
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
 * @see XMLESFecDevice::setDevices (deviceVector *dVector, std::string partitionName, unsigned int versionUpdate)
 */
void XMLESFecDevice::setDevices(std::string partitionName, unsigned int versionUpdate) throw (FecExceptionHandler) {
  setDevices(dVector_, partitionName, versionUpdate);
}


/**Configure the database : create device parameters into the database.<BR>
 * Create a new state from the last current state with new partition, new structure... this new state is set as current state...<BR>
 * @param partitionName - name of the partition
 * @param newPartition - if <I>true</I> a new partition is created with <I>partitionName</I> as name, if <I>false</I> device parameters are stored in the already created partition <I>partitionName</I>
 * @return new Fec version major id
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
*/
unsigned int XMLESFecDevice::dbConfigure(std::string partitionName, boolean newPartition ) throw (FecExceptionHandler) {
  unsigned int returnedVersionMajorId = 0;
  esMemBufOutputSource memBufOS(dVector_, true);
 
#ifdef DATABASEDEBUG
  std::cout << "XMLESFecDevice::newDbConfigure create a new partition ? " << newPartition << std::endl;
  std::cout << "XMLESFecDevice::newDbConfigure partitionName : " << partitionName << std::endl;
#endif
  
  try {
    if (dataBaseAccess_){
      std::string *xmlBuffer = new std::string[5];
      xmlBuffer[0] = (memBufOS.getPllOutputBuffer())->str();
      xmlBuffer[1] = (memBufOS.getKchipOutputBuffer())->str();
      xmlBuffer[2] = (memBufOS.getDeltaOutputBuffer())->str();
      xmlBuffer[3] = (memBufOS.getPaceOutputBuffer())->str();
      xmlBuffer[4] = (memBufOS.getDcuOutputBuffer())->str();
#ifdef DATABASEDEBUG
			std::cout << "--------------------------------------------------" << std::endl;
      std::cout << xmlBuffer[0] << std::endl;
      std::cout << "--------------------------------------------------" << std::endl;
      std::cout << xmlBuffer[1] << std::endl;
      std::cout << "--------------------------------------------------" << std::endl;
      std::cout << xmlBuffer[2] << std::endl;
      std::cout << "--------------------------------------------------" << std::endl;
      std::cout << xmlBuffer[3] << std::endl;
      std::cout << "--------------------------------------------------" << std::endl;
      std::cout << xmlBuffer[4] << std::endl;
#endif
      returnedVersionMajorId = ((ESDbFecAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer, partitionName, newPartition);
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
  return returnedVersionMajorId;
}

#endif


// ---------------------------------------------------------------------------------------------
// Parser
// ---------------------------------------------------------------------------------------------

/** Parse the elements for a FEC buffer
 * \param DOMNode - DOM buffer (DOMDocument or DOMNode)
 */
unsigned int XMLESFecDevice::parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {

      char *name = XMLString::transcode(n->getNodeName());
      //std::cout <<"----------------------------------------------------------"<<std::endl;
      //std::cout <<"Encountered Element : "<< name << std::endl;

      if(n->hasAttributes()) {

	int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
	char fecHardwareId[100] = "0" ;
	bool enabled = true ;
	unsigned int timeStamp = 0, dcuHardId = 0;					//DCU
	int channel0 = 0, channel1 = 0, channel2 = 0, channel3 = 0, channel4 = 0;
	int channel5 = 0, channel6 = 0, channel7 = 0 ;                                // DCU
	char dcuType[100] ;                                                           // DCU
	bool dcuEnabled = true ;                                                      // DCU
	int delayFine = 0,delayCoarse = 0, pllDac = 255 ;                                           // PLL

	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- DCU
	 if (!strcmp(name,"DCU")) {
	 	
#ifdef ES_EXPLICIT_COMPARISON
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
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; DISPLAY("crate");} 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; DISPLAY("fec"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; DISPLAY("ring"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; DISPLAY("ccu"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; DISPLAY("channel"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; DISPLAY("address"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; DISPLAY("enabled");}
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUTIMESTAMP])) { val ++ ;  sscanf (value,"%u",&timeStamp) ; DISPLAY("timeStamp"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUHARDID])) { val ++ ;  sscanf (value,"%u",&dcuHardId) ; DISPLAY("dcuHardId"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL0])) { val ++ ;  sscanf (value,"%d",&channel0) ; DISPLAY("channel0");}
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL1])) { val ++ ;  sscanf (value,"%d",&channel1) ; DISPLAY("channel1"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL2])) { val ++ ;  sscanf (value,"%d",&channel2) ; DISPLAY("channel2");}
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL3])) { val ++ ;  sscanf (value,"%d",&channel3) ; DISPLAY("channel3");}
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL4])) { val ++ ;  sscanf (value,"%d",&channel4) ; DISPLAY("channel4"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL5])) { val ++ ;  sscanf (value,"%d",&channel5) ; DISPLAY("channel5");}
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL6])) { val ++ ;  sscanf (value,"%d",&channel6) ; DISPLAY("channel6");}
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL7])) { val ++ ;  sscanf (value,"%d",&channel7) ; DISPLAY("channel7");}
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::EDCUTYPE])) { val ++ ;  strncpy(dcuType,value,100) ; DISPLAY("dcuType"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUREADOUTENABLED])) { val ++ ; if (value[0] == 'F') dcuEnabled = false ; DISPLAY("dcuEnabled");}
	    else std::cerr << "Unknown tag: " << name << std::endl;
	      
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }
	      
	  if (val != 20) std::cerr << "DCU: invalid number of parameters: " << val << "/20" << std::endl ;
	    
	  dcuDescription *device = new dcuDescription(buildCompleteKey(fec,ring,ccu,channel,address),timeStamp, dcuHardId,
						      channel0, channel1, channel2, channel3, channel4, channel5, 
						      channel6, channel7, dcuType) ;
	  device->setEnabled(enabled) ;
	  device->setDcuReadoutEnabled(dcuEnabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
#else
		// get all the attributes of the node
		XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
		unsigned int val = XMLCommonFec::parseAttributes(dcuParameterNames_, pAttributes);
		if (val != 20) std::cerr << "DCU: invalid number of parameters: " << val << "/" << 20 << std::endl;
		dcuDescription *device = new dcuDescription(*dcuParameterNames_);
#endif
		
		dVector_.push_back(device);
	  if (device->getDcuType() == DCUCCU) countDCUCCU ++ ;
	  else if (device->getDcuType() == DCUFEH) countDCUFEH ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- PLL
	else if (!strcmp(name,"PLL")) {
		
#ifdef ES_EXPLICIT_COMPARISON
	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	 // std::cout <<"\t----------" << std::endl;
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
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; DISPLAY("fec"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; DISPLAY("ring"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; DISPLAY("ccu"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; DISPLAY("channel"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; DISPLAY("address"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; DISPLAY("enabled");}
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYFINE])) { val ++ ;  sscanf (value,"%d",&delayFine) ; DISPLAY("delayFine"); }
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYCOARSE])) { val ++ ;  sscanf (value,"%d",&delayCoarse) ; DISPLAY("delayCoarse");}
			else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::PLLDAC])) { val ++ ;  sscanf (value,"%d",&pllDac) ; DISPLAY("pllDac"); }
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 11) std::cerr << "PLL: invalid number of parameters: " << val << "/11" << std::endl ;

	  pllDescription *device = new pllDescription(buildCompleteKey(fec,ring,ccu,channel,address),delayFine,delayCoarse, pllDac) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
#else
		// get all the attributes of the node
		XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(pllParameterNames_,pAttributes) ;
	  if (val != 11) std::cerr << "PLL: invalid number of parameters: " << val << "/" << 11 << std::endl ;
	  pllDescription *device = new pllDescription (*pllParameterNames_) ;
#endif

	  dVector_.push_back(device) ;
	  countPLL ++ ;
	}
//#ifdef PRESHOWER
	// *************************************************************************************
	// Please note that the explicit comparison done as the Tracker devices has a performance
	// improvement between 20 to 40% of the time
	// no counter has been added
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- DELTA
	else if (!strcmp(name,"DELTA")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(deltaParameterNames_, pAttributes);
	  if (val != DELTA_DESC_PAR_NUM) std::cerr << "XMLESFecDevice::parseAttributes: Delta description: invalid number of parameters: " << val << "/" << DELTA_DESC_PAR_NUM << std::endl;
	  deltaDescription *delta = new deltaDescription(*deltaParameterNames_);
	  
	  dVector_.push_back(delta);
	
	  countDelta ++;
	}
	else if (!strcmp(name,"PACEAM")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(paceParameterNames_, pAttributes) ;
	  if (val != PACE_DESC_PAR_NUM) std::cerr << "Pace description: invalid number of parameters: " << val << "/" << PACE_DESC_PAR_NUM << std::endl ;
	  paceDescription *pace = new paceDescription (*paceParameterNames_) ;
	  dVector_.push_back(pace) ;

	  countPace ++;
	}
	else if (!strcmp(name,"KCHIP")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(kchipParameterNames_, pAttributes) ;
	  if (val != KCHIP_DESC_PAR_NUM) std::cerr << "Kchip description: invalid number of parameters: " << val << "/" << KCHIP_DESC_PAR_NUM << std::endl ;
	  kchipDescription *kchip = new kchipDescription (*kchipParameterNames_) ;
	  dVector_.push_back(kchip) ;
	
	  countKchip ++;
	}
	else if (!strcmp(name,"GOH")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(gohParameterNames_, pAttributes) ;
	  if (val != GOH_DESC_PAR_NUM) std::cerr << "Goh description: invalid number of parameters: " << val << "/" << GOH_DESC_PAR_NUM << std::endl ;
	  gohDescription *goh = new gohDescription (*gohParameterNames_) ;
	  dVector_.push_back(goh) ;
	}
//#endif
    } //if (n->hasAttributes)

      // Release name
      XMLString::release(&name);

      ++count;
    }

    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      count += parseAttributes(child) ;
  }

  return count;
}

