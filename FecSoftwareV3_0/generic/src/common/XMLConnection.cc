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
#include "XMLConnection.h"

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
using namespace XERCES_CPP_NAMESPACE;

#include "MemBufOutputSource.h"

//#define DISPLAYBUFFER

/**Create an XMLCommonFec dedicated for connections
 * Call the <I>XMLCommonFec::XMLCommonFec()</I> constructor<BR>
 * Call the method <I>XMLConnection::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec()</I>
 */
XMLConnection::XMLConnection () throw (FecExceptionHandler) :
  XMLCommonFec () {

  countConnections_ = 0 ;
  parameterNames_ = ConnectionDescription::getParameterNames() ;
}

/**Creates a XMLConnection from a buffer<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLConnection::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(const XMLByte* xmlBuffer)</I>
 */
XMLConnection::XMLConnection ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLCommonFec( xmlBuffer ){

  countConnections_ = 0 ;
  parameterNames_ = ConnectionDescription::getParameterNames() ;
}

#ifdef DATABASE
/**Creates a XMLConnection for database access<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLConnection::init()</I><BR>
 * @param dbAccess - database accesss
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I>
 * @see <I>XMLConnection::init()</I>
 */
XMLConnection::XMLConnection ( DbConnectionsAccess *dbAccess ) throw (FecExceptionHandler) : XMLCommonFec( (DbCommonAccess *)dbAccess ) {

  countConnections_ = 0 ;
  parameterNames_ = ConnectionDescription::getParameterNames() ;
}
#endif

/**Creates a XMLConnection from a file<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLConnection::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I>
 * @see <I>XMLConnection::init()</I>
 */
XMLConnection::XMLConnection ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLCommonFec( xmlFileName ) {

  countConnections_ = 0 ;
  parameterNames_ = ConnectionDescription::getParameterNames() ;
}

/**Delete the <I>handler_</I> attribute<BR>
 * Delete the device vector <I>dVector_</I> attribute<BR>
 */
XMLConnection::~XMLConnection (){

  // empty the dVector_ attribute
  // delete all elements
  // delete the dVector_attribute
  // clearVector(); // delete by the factory
  ConnectionDescription::deleteParameterNames(parameterNames_);
  delete parameterNames_ ;
}

/**
 * Empty and clear the dVector
 */
void XMLConnection::clearVector() {

  while (connectionVector_.size() > 0) {
    delete connectionVector_.back();
    connectionVector_.pop_back();
  }
  connectionVector_.clear() ;
}

/** Parse the attributs for a DOM document
 */
unsigned int XMLConnection::parseAttributes( XERCES_CPP_NAMESPACE::DOMNode *n ) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;
  
  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {
      
      char *name = XMLString::transcode(n->getNodeName());
      //std::cout <<"Encountered Element : "<< name << std::endl;

      if(n->hasAttributes()) {

	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- Connection
	if (!strcmp(name,"FedChannelConnection")) {

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(parameterNames_,pAttributes) ;
	  if (val != 13) std::cerr << "Connection description: invalid number of parameters: " << std::dec << val << "/13" << std::endl ;
	  ConnectionDescription *connectionDescription = new ConnectionDescription(*parameterNames_) ;
	  connectionVector_.push_back(connectionDescription) ;

	  countConnections_ ++ ;
	}
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

#ifdef DATABASE
/**Send a request to the database with partition name as parameter, for the current version<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @see <I>DbAccess::getXMLClob(std::string partitionName)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess or the partition name have not been intialized
 */
void XMLConnection::parseDatabaseResponse(std::string partitionName) 
  throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionName";
  
#ifdef DATABASEDEBUG
  std::cout << __PRETTY_FUNCTION__ << std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbConnectionsAccess*)dataBaseAccess_)->getXMLClob(partitionName))){
	
#ifdef DISPLAYBUFFER
	display (xmlClob) ;
#endif

	XERCES_CPP_NAMESPACE::InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	XERCES_CPP_NAMESPACE::DOMInputSource *domInputSource = new XERCES_CPP_NAMESPACE::Wrapper4InputSource((XERCES_CPP_NAMESPACE::InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;
      } else {

	domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + " for partition " + partitionName, e), ERRORCODE) ;
    }
    catch (const XERCES_CPP_NAMESPACE::SAXException &ex) {
      RAISEFECEXCEPTIONHANDLER ( XML_SAXEXCEPTION, StrX(ex.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) {
      RAISEFECEXCEPTIONHANDLER ( XML_XMLEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException& toCatch) {
      
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];
      if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(errText).getString(), ERRORCODE ) ;
      else
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (...) {
      RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
    }
  } else {
     RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/**Send a request to the database with partition name as parameter, for the current version<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param versionMajorId - version major id
 * @param versionMinorId - version minor id
 * @param retrieveApvNotConnected - to get the APV not connected
 * @see <I>DbAccess::getXMLClob(std::string partitionName)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess or the partition name have not been intialized
 */
void XMLConnection::parseDatabaseResponse(std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor, bool retrieveApvNotConnected) 
  throw (FecExceptionHandler) {
  
  std::string xmlBufferId = "theXMLBufferFromPartitionName";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLConnection::parseDatabaseResponse ( partitionName, versionMajorId, versionMinorId, notConnected ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if (!retrieveApvNotConnected)
	xmlClob = ((DbConnectionsAccess*)dataBaseAccess_)->getXMLClobWithVersion(partitionName,versionMajorId,versionMinorId, maskVersionMajor, maskVersionMinor) ;
      else
	xmlClob = ((DbConnectionsAccess*)dataBaseAccess_)->getApvNotConnectedWithVersion(partitionName,versionMajorId,versionMinorId) ;

      if (xmlClob) {
	
#ifdef DISPLAYBUFFER
	display (xmlClob) ;
#endif

	XERCES_CPP_NAMESPACE::InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	XERCES_CPP_NAMESPACE::DOMInputSource *domInputSource = new XERCES_CPP_NAMESPACE::Wrapper4InputSource((XERCES_CPP_NAMESPACE::InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;
      } else {

	domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + " for partition " + partitionName, e), ERRORCODE) ;
    }
    catch (const XERCES_CPP_NAMESPACE::SAXException &ex) {
      RAISEFECEXCEPTIONHANDLER ( XML_SAXEXCEPTION, StrX(ex.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) {
      RAISEFECEXCEPTIONHANDLER ( XML_XMLEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException& toCatch) {
      
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];
      if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(errText).getString(), ERRORCODE ) ;
      else
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (...) {
      RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
    }
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/**Send a request to the database with the partitionName as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - The partitionName
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
ConnectionVector XMLConnection::getConnections (std::string partitionName) throw (FecExceptionHandler) {

#ifdef DATABASEDEBUG
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

  parseDatabaseResponse(partitionName);

  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "ParseDatabaseResponse = " << (endMillis-startMillis) << " ms" << std::endl ;

  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseXMLBuffer();
  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "parseXMLBuffer = " << (endMillis-startMillis) << " ms" << std::endl ;
#else
  parseDatabaseResponse(partitionName);
  parseXMLBuffer();
#endif

  return connectionVector_;
}

/**Send a request to the database with the partitionName as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - The partitionName
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
ConnectionVector XMLConnection::getConnections (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (FecExceptionHandler) {
  parseDatabaseResponse(partitionName,versionMajorId,versionMinorId,maskVersionMajor,maskVersionMinor,false);
  parseXMLBuffer();
  return connectionVector_;
}

/**Send a request to the database with the partitionName as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - The partitionName
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
ConnectionVector XMLConnection::getApvNotConnected (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (FecExceptionHandler) {
  parseDatabaseResponse(partitionName,versionMajorId,versionMinorId,0,0,true);
  parseXMLBuffer();
  return apvNotConnectedVector_;
}

#endif

/**Parses the <I>xmlInputSource_</I> attribute using the SAX interface<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLConnection::parseXMLBuffer()
 */
ConnectionVector XMLConnection::getConnections () throw (FecExceptionHandler) {

  parseXMLBuffer();
  return connectionVector_;
}

/**Generates an XML buffer from the parameter <I>dVector</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param dVector - device list
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if
 *     - <I>dVector</I> is not initialized
 *     - the MemBufOutputSource created from the parameter <I>dVector</I> is not initialized 
 * @see XMLCommonFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 */
void XMLConnection::setFileConnections ( ConnectionVector connectionVector, std::string outputFileName) throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS (connectionVector, false) ;

  XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
}

/**Generates an XML buffer from the attribute <I>dVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 */
void XMLConnection::setFileConnections ( std::string outputFileName ) throw (FecExceptionHandler) {
  setFileConnections( connectionVector_, outputFileName);
}


#ifdef DATABASE
/** \brief Sends the descriptions from the description to the database
 * \param partitionName - partition name
 * \param cVector - connection vector
 * \param versionUpdate - 1 for a new major version, 0 for a new minor version
   */
void XMLConnection::setDbConnectionDescription ( std::string partitionName, ConnectionVector connectionVector, unsigned int versionUpdate ) throw (FecExceptionHandler, oracle::occi::SQLException) {

  MemBufOutputSource memBufOS (connectionVector,true) ;

  if (dataBaseAccess_) {
    try {
      ((DbConnectionsAccess *)dataBaseAccess_)->setXMLClob(memBufOS.getOutputBuffer()->str(), partitionName, versionUpdate);
    }
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + " for partition " + partitionName, e), ERRORCODE) ;
    }
  }
  
}

/** \brief Sends the descriptions from the description to the database
 * \param partitionName - partition name
 * \param versionUpdate - 1 for a new major version, 0 for a new minor version
 */
void XMLConnection::setDbConnectionDescription ( std::string partitionName, unsigned int versionUpdate ) throw (FecExceptionHandler, oracle::occi::SQLException) {

  setDbConnectionDescription ( partitionName, connectionVector_, versionUpdate ) ;
}

#endif
