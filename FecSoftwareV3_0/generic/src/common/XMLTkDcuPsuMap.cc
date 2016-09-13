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

  Copyright 2002 - 2007, Guillaume BAULIEU - IPNL/IN2P3

*/
#include "XMLTkDcuPsuMap.h"

#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>

using namespace XERCES_CPP_NAMESPACE ;

//#define DISPLAYBUFFER

/**Create an XMLCommonFec dedicated for devices such as DcuConversion, DcuPsuMap...<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec()</I> constructor<BR>
 * Call the method <I>XMLTkDcuPsuMap::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec()</I>
 * @see <I>XMLTkDcuPsuMap::init()</I>
 */
XMLTkDcuPsuMap::XMLTkDcuPsuMap () throw (FecExceptionHandler) :
  XMLCommonFec () 
{
  countDcuPsuMap_ = 0 ;
  dcuPsuMapParameterNames_ = TkDcuPsuMap::getParameterNames() ;
}


/**Creates a XMLTkDcuPsuMap from a buffer<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLTkDcuPsuMap::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(const XMLByte* xmlBuffer)</I>
 * @see <I>XMLTkDcuPsuMap::init()</I>
 */
XMLTkDcuPsuMap::XMLTkDcuPsuMap (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLCommonFec( xmlBuffer ){

  countDcuPsuMap_ = 0 ;
  dcuPsuMapParameterNames_ = TkDcuPsuMap::getParameterNames() ;
}

#ifdef DATABASE
/**Creates a XMLTkDcuPsuMap from a file<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLTkDcuPsuMap::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I>
 * @see <I>XMLTkDcuPsuMap::init()</I>
 */
XMLTkDcuPsuMap::XMLTkDcuPsuMap ( DbTkDcuPsuMapAccess *dbAccess )throw (FecExceptionHandler) : XMLCommonFec( (DbCommonAccess *)dbAccess ) {

  countDcuPsuMap_ = 0 ;
  dcuPsuMapParameterNames_ = TkDcuPsuMap::getParameterNames() ;
}
#endif

/**Creates a XMLTkDcuPsuMap from a file<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLTkDcuPsuMap::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I>
 * @see <I>XMLTkDcuPsuMap::init()</I>
 */
XMLTkDcuPsuMap::XMLTkDcuPsuMap ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLCommonFec( xmlFileName ) {

  countDcuPsuMap_ = 0 ;
  dcuPsuMapParameterNames_ = TkDcuPsuMap::getParameterNames() ;
}

/**Delete the <I>handler_</I> attribute<BR>
 * Delete the device vector <I>dVector_</I> attribute<BR>
 */
XMLTkDcuPsuMap::~XMLTkDcuPsuMap (){

  // empty the dVector_ attribute
  // delete all elements
  // delete the dVector_attribute
  // clearVector(); // to avoid copy the delete is done in the Factory
  TkDcuPsuMap::deleteParameterNames(dcuPsuMapParameterNames_); delete dcuPsuMapParameterNames_ ;
}

/**
 * Empty and clear the dVector
 */
void XMLTkDcuPsuMap::clearVector(){
  while (dVector_.size() > 0) {
    delete dVector_.back();
    dVector_.pop_back();
  }
  dVector_.clear() ;
}

/**Get the device Vector
 * @return the tkDcuPsuMapVector <I>dVector_</I> attribute
 */
tkDcuPsuMapVector XMLTkDcuPsuMap::getDcuPsuMapVector () {
  return dVector_;
}

/**Set the dcuInfo Vector
 * Empty the <I>pVector_</I> attribute
 * Delete all his elements
 * Copy all elements from <I>dVector</I> parameter to <I>pVector_</I> attribute
 * @exception FecExceptionHandler : a FecExceptionHandler is raised, if <I>pVector_</I> is NULL
 */
void XMLTkDcuPsuMap::setDcuPsuMapVector (tkDcuPsuMapVector dVector) throw (FecExceptionHandler) {
  // empty and delete previous dVector_
#ifdef DATABASEDEBUG
  std::cout << "on vide le dVector !!!!!" << std::endl;
#endif
  clearVector() ;
  
  // set the tkDcuPsuMapVector parameter as the tkDcuPsuMapVector attribute
  for (tkDcuPsuMapVector::iterator device = dVector.begin() ; device != dVector.end() ; device++) {
    dVector_.push_back (*device) ;
  }
}

/** Parses the dom documents
 * \param n - DOM node 
 */
unsigned int XMLTkDcuPsuMap::parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) {
  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {

    if (n->getNodeType() == XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE) {

      char *name = XERCES_CPP_NAMESPACE::XMLString::transcode(n->getNodeName());
      if(n->hasAttributes()) {

	if (!strcmp(name,"DCUPSUMAP")) {
	  
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  XMLCommonFec::parseAttributes(dcuPsuMapParameterNames_,pAttributes) ;
	  TkDcuPsuMap *dcuInfo = new TkDcuPsuMap (*dcuPsuMapParameterNames_) ;
	  dVector_.push_back(dcuInfo) ;

	  countDcuPsuMap_ ++ ;
	}
      }

      // Release name
      XERCES_CPP_NAMESPACE::XMLString::release(&name);

      ++count;
    }

    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      count += parseAttributes(child) ;
  }

  return count ;
}

/**Parses the <I>xmlInputSource_</I> attribute using the SAX interface<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLTkDcuPsuMap::parseXMLBuffer()
 */
tkDcuPsuMapVector XMLTkDcuPsuMap::getDcuPsuMaps () throw (FecExceptionHandler) {
  parseXMLBuffer();
  return dVector_;
}

#ifdef DATABASE


/**Send a request to the database with partition name as parameter for PSU names<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @see <I>DbAccess::getXMLClob(std::string partitionName)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess or the partition name have not been intialized
 */
void XMLTkDcuPsuMap::parsePsuNameDatabaseResponse(std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) 
  throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionName";

#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuPsuMap::parsePsuNameDatabaseResponse ( " << partitionName << ", " <<  versionMajorId << ", " << versionMinorId << ")" <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuPsuMapAccess*)dataBaseAccess_)->getPsuNameXMLClob(partitionName,versionMajorId,versionMinorId))){

#ifdef DISPLAYBUFFER
	display(xmlClob) ;
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
    catch (std::exception &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + ": " + e.what() + " for partition " + partitionName, ERRORCODE) ;
    }
//  catch (...) {
//    RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
//  }
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}


/**Send a request to the database with partition name as parameter for DCU/PSU map<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @see <I>DbAccess::getXMLClob(std::string partitionName)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess or the partition name have not been intialized
 */
void XMLTkDcuPsuMap::parseDcuPsuMapDatabaseResponse(std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) 
  throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionName";

#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuPsuMap::parseDcuPsuMapDatabaseResponse ( " << partitionName << ", " <<  versionMajorId << ", " << versionMinorId << ")" <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuPsuMapAccess*)dataBaseAccess_)->getDcuPsuMapXMLClob(partitionName,versionMajorId,versionMinorId))){

#ifdef DISPLAYBUFFER
	display(xmlClob) ;
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
//  catch (...) {
//    RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
//  }
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/**Send a request to the database with partition name as parameter for DCU/PSU map<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @see <I>DbAccess::getXMLClob(std::string partitionName)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess or the partition name have not been intialized
 */
void XMLTkDcuPsuMap::parsePsuNotConnectedDatabaseResponse(std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) 
  throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionName";

#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuPsuMap::parsePsuNotConnectedDatabaseResponse ( " << partitionName << ", " <<  versionMajorId << ", " << versionMinorId << ")" <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuPsuMapAccess*)dataBaseAccess_)->getPsuNotConnectedXMLClob(partitionName,versionMajorId,versionMinorId))){

#ifdef DISPLAYBUFFER
	display(xmlClob) ;
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
//  catch (...) {
//    RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
//  }
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}



/**Send a request to the database with the partition name as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkDcuPsuMapVector XMLTkDcuPsuMap::getPsuNames (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (FecExceptionHandler) {
  parsePsuNameDatabaseResponse(partitionName,versionMajorId,versionMinorId);
  parseXMLBuffer();
  return dVector_;
}

/**Send a request to the database with the version as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkDcuPsuMapVector XMLTkDcuPsuMap::getDcuPsuMap (std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler) {
  parseDcuPsuMapDatabaseResponse(partitionName,majorVersionId, minorVersionId);
  parseXMLBuffer();
  return dVector_;
}

/**Send a request to the database with the version as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param versionMajor - version major (0.0 means current version)
 * @param versionMinor - version major (0.0 means current version)
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkDcuPsuMapVector XMLTkDcuPsuMap::getPsuNotConnected (std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler) {
  parsePsuNotConnectedDatabaseResponse(partitionName,majorVersionId, minorVersionId);
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
 * @see XMLFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 */
void XMLTkDcuPsuMap::setFileDcuPsuMap ( tkDcuPsuMapVector dVector, std::string outputFileName) throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS(dVector,false);
  XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
}

/**Generates an XML buffer from the parameter <I>dVector</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param dVector - device list
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if
 *     - <I>dVector</I> is not initialized
 *     - the MemBufOutputSource created from the parameter <I>dVector</I> is not initialized 
 * @see XMLFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 * @warning the formating is the same than for the DCU/PSU map
 */
void XMLTkDcuPsuMap::setFilePsuName ( tkDcuPsuMapVector dVector, std::string outputFileName) throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS(dVector,false);
  XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
}

#ifdef DATABASE

/**Generates an XML buffer from the parameter <I>dVector</I><BR>.
 * Sends this buffer to the database</I>
 * @param dVector - device description vector to be stored to the database
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 * @see DbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
 */
void XMLTkDcuPsuMap::setDbDcuPsuMaps (tkDcuPsuMapVector dVector, std::string partitionName, unsigned int *versionMajorId, unsigned int *versionMinorId)  throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS(dVector, true);
  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = (memBufOS.getOutputBuffer())->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbTkDcuPsuMapAccess *)dataBaseAccess_)->uploadDcuPsuMap(xmlBuffer,partitionName,versionMajorId,versionMinorId);
    } 
    else
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  } catch(oracle::occi::SQLException &e){
    std::stringstream errorMessage ; errorMessage << e.what();
    std::string localMessage = dataBaseAccess_->getErrorMessage();
    if (localMessage.size()) errorMessage << std::endl << localMessage ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + errorMessage.str(), ERRORCODE) ;
  }
}

/**Generates an XML buffer from the parameter <I>dVector</I><BR>.
 * Sends this buffer to the database</I>
 * @param dVector - device description vector to be stored to the database
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 * @see DbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
 * @warning same call than for the setDbDcuPsuMap, same XML formatting
 */
void XMLTkDcuPsuMap::setDbPsuNames (tkDcuPsuMapVector dVector, std::string partitionName, unsigned int *versionMajorId, unsigned int *versionMinorId)  throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS(dVector, true);
  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = (memBufOS.getOutputBuffer())->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbTkDcuPsuMapAccess *)dataBaseAccess_)->uploadPsuNames(xmlBuffer,partitionName,versionMajorId,versionMinorId);
    } 
    else
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  } catch(oracle::occi::SQLException &e){
    std::stringstream errorMessage ; errorMessage << e.what();
    std::string localMessage = dataBaseAccess_->getErrorMessage();
    if (localMessage.size()) errorMessage << std::endl << localMessage ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + errorMessage.str(), ERRORCODE) ;
  }
}

#endif
