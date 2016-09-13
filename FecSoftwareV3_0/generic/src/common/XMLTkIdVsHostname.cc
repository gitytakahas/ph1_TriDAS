/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>

using namespace XERCES_CPP_NAMESPACE ;

#ifdef DATABASE
#include "DbTkIdVsHostnameAccess.h"
#endif
#include "XMLTkIdVsHostname.h"

//#define DISPLAYBUFFER

/** Default constructor
 */
XMLTkIdVsHostname::XMLTkIdVsHostname () throw (FecExceptionHandler) :
  XMLCommonFec () {

  countTkIdVsHostname_ = 0 ;
  parameterNames_ = TkIdVsHostnameDescription::getParameterNames() ;
}

/** Constructor with xml buffer
 * \param xmlBuffer - XML buffer
 */
XMLTkIdVsHostname::XMLTkIdVsHostname ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLCommonFec( xmlBuffer ) {

  countTkIdVsHostname_ = 0 ;
  parameterNames_ = TkIdVsHostnameDescription::getParameterNames() ;
}

#ifdef DATABASE
/** Constructor with database access
 * \param dbAccess - database access
 */
XMLTkIdVsHostname::XMLTkIdVsHostname ( DbTkIdVsHostnameAccess *dbAccess )throw (FecExceptionHandler): 
  XMLCommonFec( (DbCommonAccess *)dbAccess ) {
  
  countTkIdVsHostname_ = 0 ;
  parameterNames_ = TkIdVsHostnameDescription::getParameterNames() ;
}
#endif

/** Constructor with file access
 * \param xmlFileName - XML file name
 */
XMLTkIdVsHostname::XMLTkIdVsHostname ( std::string xmlFileName ) throw (FecExceptionHandler):
  XMLCommonFec( xmlFileName ) {

  countTkIdVsHostname_ = 0 ;
  parameterNames_ = TkIdVsHostnameDescription::getParameterNames() ;
}
  

/** Deletes the XMLTkIdVsHostname
 */
XMLTkIdVsHostname::~XMLTkIdVsHostname () {

  TkIdVsHostnameDescription::deleteParameterNames(parameterNames_);
  delete parameterNames_ ;
}

/** Parse the XML buffer
 * n - DOM node containing the XML buffer
 */
unsigned int XMLTkIdVsHostname::parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {

    if (n->getNodeType() == XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE) {

      char *name = XERCES_CPP_NAMESPACE::XMLString::transcode(n->getNodeName());
      if(n->hasAttributes()) {

	if (!strcmp(name,"IDVSHOSTNAME")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(parameterNames_,pAttributes) ;
	  if (val != 6) std::cerr << "ID versus hostname (TkIdVsHostnameDescription): invalid number of parameters: " << std::dec << val << "/6" << std::endl ;
	  TkIdVsHostnameDescription *idVsHostname = new TkIdVsHostnameDescription (*parameterNames_) ;
	  dVector_.push_back(idVsHostname) ;
	  
	  countTkIdVsHostname_ ++ ;
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

/** clear the vector of elements
 */
void XMLTkIdVsHostname::clearVector() {

  while (dVector_.size() > 0) {
    delete dVector_.back();
    dVector_.pop_back();
  }
  dVector_.clear() ;
}

/** Gets a pointer on the device vector private attribute
 */
TkIdVsHostnameVector XMLTkIdVsHostname::getTkIdVsHostnameVectorAttribut ( ) {

  return dVector_;
}

/** Sets the device vector private attribute
 * \param tkIdVsHostname - vector of TkIdVsHostnameDescription
 */
void XMLTkIdVsHostname::setTkIdVsHostnameVector (TkIdVsHostnameVector tkIdVsHostname) throw (FecExceptionHandler) {

  clearVector() ;
  for (TkIdVsHostnameVector::iterator it = tkIdVsHostname.begin() ; it != tkIdVsHostname.end() ; it ++) {
    dVector_.push_back(*it) ;
  }
}

/** Gets a pointer on the device vector private attribute from database
 */
TkIdVsHostnameVector XMLTkIdVsHostname::getAllTkIdVsHostnameFromFile (  ) throw (FecExceptionHandler) {

  parseXMLBuffer();
  return dVector_;
}

#ifdef DATABASE
/** Parse for a given version or for the default/current version version
 * \param versionMajor - version major (default 0)
 * \param versionMinor - version minor (default 0)
 */
void XMLTkIdVsHostname::parseDatabaseResponse( unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferForAllIdVsHostname";
#ifdef DATABASEDEBUG
 std::cout << "void XMLTkIdVsHostname::parseDatabaseResponse ( verisonMajor, versionMinor ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkIdVsHostnameAccess*)dataBaseAccess_)->getXMLClob( versionMajor, versionMinor ))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE) ;
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

/** Parse for a given hostname and (version or for the default/current version version)
 * \param hostname - hostname
 * \param versionMajor - version major (default 0)
 * \param versionMinor - version minor (default 0)
 */
void XMLTkIdVsHostname::parseDatabaseResponse( std::string hostname, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferForAllIdVsHostname";

#ifdef DATABASEDEBUG
 std::cout << "void XMLTkIdVsHostname::parseDatabaseResponse ( hostname, verisonMajor, versionMinor ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkIdVsHostnameAccess*)dataBaseAccess_)->getXMLClobFromHostname( hostname, versionMajor, versionMinor ))) {

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE) ;
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

/** Parse for a given hostname, slot and (version or for the default/current version version)
 * \param hostname - hostname
 * \param slot - VME slot
 * \param versionMajor - version major (default 0)
 * \param versionMinor - version minor (default 0)
 */
void XMLTkIdVsHostname::parseDatabaseResponse( std::string hostname, unsigned int slot, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferForAllIdVsHostname";
#ifdef DATABASEDEBUG
 std::cout << "void XMLTkIdVsHostname::parseDatabaseResponse ( hostname, slot, verisonMajor, versionMinor ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkIdVsHostnameAccess*)dataBaseAccess_)->getXMLClobFromHostnameSlot( hostname, slot, versionMajor, versionMinor ))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE) ;
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

/** Gets a pointer on the device vector private attribute from database
 * \param versionMajor - version major (default 0)
 * \param versionMinor - version minor (default 0)
 */
TkIdVsHostnameVector XMLTkIdVsHostname::getAllTkIdVsHostname ( unsigned int versionMajor, unsigned int versionMinor  ) throw (FecExceptionHandler) {

  parseDatabaseResponse(versionMajor,versionMinor) ;
  parseXMLBuffer();
  return dVector_;
}

/** Gets a pointer on the device vector private attribute from database
 * \param hostname - hostname
 * \param versionMajor - version major (default 0)
 * \param versionMinor - version minor (default 0)
 */
TkIdVsHostnameVector XMLTkIdVsHostname::getAllTkIdFromHostname ( std::string hostname, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) {

  parseDatabaseResponse(hostname,versionMajor,versionMinor) ;
  parseXMLBuffer();
  return dVector_;
}

/** Gets a pointer on the device vector private attribute from database
 * \param hostname - hostname
 * \param slot - VME slot
 * \param versionMajor - version major (default 0)
 * \param versionMinor - version minor (default 0)
 */
TkIdVsHostnameVector XMLTkIdVsHostname::getTkIdFromHostnameSlot ( std::string hostname, unsigned int slot, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) {

  parseDatabaseResponse(hostname,slot,versionMajor,versionMinor) ;
  parseXMLBuffer();
  return dVector_;
}

#endif

/** Writes the device vector parameter in a ASCII file
 * \param dVector - vector of TkIdVsHostnameDescription
 * \param outputFileName - output file name
 */
void XMLTkIdVsHostname::setTkIdVsHostnameVector ( TkIdVsHostnameVector dVector, std::string outputFileName ) throw (FecExceptionHandler) {
  
  if (dVector.size()) {
    MemBufOutputSource memBufOS (dVector, false );
    XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in file", ERRORCODE) ;
  }
}

/** Writes the device vector attribute in a ASCII file
 * \param outputFileName - output file name
 */
void XMLTkIdVsHostname::setTkIdVsHostnameVector ( std::string outputFileName ) throw (FecExceptionHandler) {

  if (dVector_.size()) {
    MemBufOutputSource memBufOS (dVector_ );
    XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in file", ERRORCODE) ;
  }
}

#ifdef DATABASE
/** Sends the parameter value from device vector to the database
 * \param dVector - vector of TkIdVsHostnameDescription
 * \param major - major or minor version (major if all, minor if a part)
 * \warning the vector is set at the level of a hostname
 */
void XMLTkIdVsHostname::setTkIdVsHostnameVector ( TkIdVsHostnameVector dVector, bool major ) throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS(dVector, true);
  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = (memBufOS.getOutputBuffer())->str();
#ifdef DISPLAYBUFFER
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbTkIdVsHostnameAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer,major);
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE) ;
  }
}
#endif
