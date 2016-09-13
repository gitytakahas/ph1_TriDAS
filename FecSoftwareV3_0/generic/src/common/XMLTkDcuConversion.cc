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
#include "XMLTkDcuConversion.h"

#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>

using namespace XERCES_CPP_NAMESPACE ;

/**Create an XMLCommonFec dedicated for devices such as Pll, Laserdriver, ApvFec, ApvMux, Philips,...<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec()</I> constructor<BR>
 * Call the method <I>XMLTkDcuConversion::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec()</I>
 * @see <I>XMLTkDcuConversion::init()</I>
 */
XMLTkDcuConversion::XMLTkDcuConversion () throw (FecExceptionHandler) :
  XMLCommonFec () 
{
  countDCUConversion_ = 0 ;
  parameterNames_ = TkDcuConversionFactors::getParameterNames() ;
}


/**Creates a XMLTkDcuConversion from a buffer<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLTkDcuConversion::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(const XMLByte* xmlBuffer)</I>
 * @see <I>XMLTkDcuConversion::init()</I>
 */
XMLTkDcuConversion::XMLTkDcuConversion (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLCommonFec( xmlBuffer ){
  countDCUConversion_ = 0 ;
  parameterNames_ = TkDcuConversionFactors::getParameterNames() ;
}

#ifdef DATABASE
/**Creates a XMLTkDcuConversion for database access<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLTkDcuConversion::init()</I><BR>
 * @param dbAccess - database accesss
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I>
 * @see <I>XMLTkDcuConversion::init()</I>
 */
XMLTkDcuConversion::XMLTkDcuConversion ( DbTkDcuConversionAccess *dbAccess ) throw (FecExceptionHandler) : XMLCommonFec( (DbCommonAccess *)dbAccess ) {
  countDCUConversion_ = 0 ;
  parameterNames_ = TkDcuConversionFactors::getParameterNames() ;
}
#endif

/**Creates a XMLTkDcuConversion from a file<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLTkDcuConversion::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I>
 * @see <I>XMLTkDcuConversion::init()</I>
 */
XMLTkDcuConversion::XMLTkDcuConversion ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLCommonFec( xmlFileName ) {
  countDCUConversion_ = 0 ;
  parameterNames_ = TkDcuConversionFactors::getParameterNames() ;
}

/**Delete the <I>handler_</I> attribute<BR>
 * Delete the device vector <I>dVector_</I> attribute<BR>
 */
XMLTkDcuConversion::~XMLTkDcuConversion () {
  TkDcuConversionFactors::deleteParameterNames(parameterNames_);
  delete parameterNames_ ;
}

/**
 * Empty and clear the dVector
 */
void XMLTkDcuConversion::clearVector(){

  while (dVector_.size() > 0) {
    delete dVector_.back();
    dVector_.pop_back();
  }
  dVector_.clear() ;
}

/** Parses the dom documents
 * \param n - DOM node 
 */
unsigned int XMLTkDcuConversion::parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {

    if (n->getNodeType() == XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE) {

      char *name = XERCES_CPP_NAMESPACE::XMLString::transcode(n->getNodeName());

      if(n->hasAttributes()) {

	if (!strcmp(name,"DCUCONVERSION")) {
	  
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(parameterNames_,pAttributes) ;
	  //if ( (val != 14) && (val != 21) ) std::cerr << "DCU conversion factors: invalid number of parameters: " << std::dec << val << "/14 or /21" << std::endl ;
	  if (val <= 0) std::cerr << "DCU conversion factors: invalid number of parameters: " << std::dec << val << "/14 or /21" << std::endl ;
	  TkDcuConversionFactors *dcuConversion = new TkDcuConversionFactors (*parameterNames_) ;
	  dVector_.push_back(dcuConversion) ;
	  
	  countDCUConversion_ ++ ;
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

/**Get the device Vector
 * @return the dcuConversionVector <I>dVector_</I> attribute
 */
dcuConversionVector XMLTkDcuConversion::getDcuConversionVector () {
  return dVector_;
}


/**Parses the <I>xmlInputSource_</I> attribute using the SAX interface<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLTkDcuConversion::parseXMLBuffer()
 */
dcuConversionVector XMLTkDcuConversion::getDcuConversions () throw (FecExceptionHandler) {

  parseXMLBuffer();
  return dVector_;
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
void XMLTkDcuConversion::parseDatabaseResponse(std::string partitionName) 
  throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromPartitionName";

#ifdef DATABASEDEBUG
 std::cout << "void XMLTkDcuConversion::parseDatabaseResponse ( partitionName ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuConversionAccess*)dataBaseAccess_)->getXMLClob(partitionName))){

#ifdef DATABASEDEBUG
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

/**Send a request to the database with dcu Hard ID as parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param dcuHardId - The DCU id
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess or the dcuHardId name have not been intialized
 */
void XMLTkDcuConversion::parseDatabaseResponse(tscType32 dcuHardId) 
  throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromDcuHardIdNumber";
#ifdef DATABASEDEBUG
 std::cout << "void XMLTkDcuConversion::parseDatabaseResponse ( dcuHardId ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuConversionAccess*)dataBaseAccess_)->getXMLClob(dcuHardId))){

#ifdef DATABASEDEBUG
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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + " for DCU " + toString(dcuHardId), e), ERRORCODE) ;
    }
    catch (const SAXException &ex) {
      RAISEFECEXCEPTIONHANDLER ( XML_SAXEXCEPTION, StrX(ex.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XMLException& toCatch) {
      RAISEFECEXCEPTIONHANDLER ( XML_XMLEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const DOMException& toCatch) {
      
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

/**Send a request to the database with the dcuHardId as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param dcuHardId - The DCU Hard ID
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
dcuConversionVector XMLTkDcuConversion::getDcuConversions (tscType32 dcuHardId) throw (FecExceptionHandler) {
    parseDatabaseResponse(dcuHardId);
    parseXMLBuffer();
    return dVector_;
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
dcuConversionVector XMLTkDcuConversion::getDcuConversions (std::string partitionName) throw (FecExceptionHandler) {
  parseDatabaseResponse(partitionName);
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
void XMLTkDcuConversion::setDcuConversions ( dcuConversionVector dVector, std::string outputFileName) throw (FecExceptionHandler) {

  if (dVector.size()) {
    MemBufOutputSource memBufOS (dVector );
    XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in file", ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>dVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLTkDcuConversion::setDevices ( dcuConversionVector dVector, std::string outputFileName)
 */
void XMLTkDcuConversion::setDcuConversions ( std::string outputFileName ) throw (FecExceptionHandler) {
  setDcuConversions( dVector_, outputFileName);
}

/**Set the dcuConversion Vector
 * Empty the <I>pVector_</I> attribute
 * Delete all his elements
 * Copy all elements from <I>dcuConversions</I> parameter to <I>pVector_</I> attribute
 * @exception FecExceptionHandler : a FecExceptionHandler is raised, if <I>pVector_</I> is NULL
 */
void XMLTkDcuConversion::setDcuConversionVector (dcuConversionVector dcuConversions) throw (FecExceptionHandler) {
  // empty and delete previous dVector_
  // delete all elements
  if (dVector_.size()) {
    clearVector() ;
    
    // set the dcuConversionVector parameter as the dcuConversionVector attribute
    for (dcuConversionVector::iterator device = dcuConversions.begin() ; device != dcuConversions.end() ; device++) {
      dVector_.push_back (*device) ;
    }
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  }
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
void XMLTkDcuConversion::setDcuConversions (dcuConversionVector dVector)  throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS(dVector, true);
  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = (memBufOS.getOutputBuffer())->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbTkDcuConversionAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer);
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE) ;
  }
}

/**Generates an XML buffer from the current <I>dVector</I><BR>.
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 * @see DbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
 */
void XMLTkDcuConversion::setDcuConversions ()  throw (FecExceptionHandler) {
  setDcuConversions(dVector_);
}

/**Configure the database : create device parameters into the database.<BR>
 * Create a new state from the last current state with new partition, new structure... this new state is set as current state...<BR>
 * @param structureName - name of the structure
 * @param partitionName - name of the partition
 * @param newPartition - if <I>true</I> a new partition is created with <I>partitionName</I> as name, if <I>false</I> device parameters are stored in the already created partition <I>partitionName</I>
 * @return new Fec version major id
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
*/
void XMLTkDcuConversion::dbConfigure() throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS (dVector_, true);

  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = memBufOS.getOutputBuffer()->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbTkDcuConversionAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer);
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG, e), ERRORCODE) ;
  }
}

#endif
