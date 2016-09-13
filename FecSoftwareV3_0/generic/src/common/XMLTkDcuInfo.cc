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
#include "XMLTkDcuInfo.h"

#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>

// #define DISPLAYBUFFER

using namespace XERCES_CPP_NAMESPACE ;

/**Create an XMLCommonFec dedicated for devices such as DcuConversion, DcuInfo...<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec()</I> constructor<BR>
 * Call the method <I>XMLTkDcuInfo::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec()</I>
 * @see <I>XMLTkDcuInfo::init()</I>
 */
XMLTkDcuInfo::XMLTkDcuInfo () throw (FecExceptionHandler) :
  XMLCommonFec ()  {
  countDCUInfo_ = countState_ = countRun_ = countVersion_ = 0 ;
  initParameterNames() ;
}


/**Creates a XMLTkDcuInfo from a buffer<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLTkDcuInfo::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(const XMLByte* xmlBuffer)</I>
 * @see <I>XMLTkDcuInfo::init()</I>
 */
XMLTkDcuInfo::XMLTkDcuInfo (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLCommonFec( xmlBuffer ){

  countDCUInfo_ = countState_ = countRun_ = countVersion_ = 0 ;
  initParameterNames() ;
}

#ifdef DATABASE
/**Creates a XMLTkDcuInfo from a file<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLTkDcuInfo::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I>
 * @see <I>XMLTkDcuInfo::init()</I>
 */
XMLTkDcuInfo::XMLTkDcuInfo ( DbTkDcuInfoAccess *dbAccess )throw (FecExceptionHandler) : XMLCommonFec( (DbCommonAccess *)dbAccess ) {

  countDCUInfo_ = countState_ = countRun_ = countVersion_ = 0 ;
  initParameterNames() ;
}
#endif

/**Creates a XMLTkDcuInfo from a file<BR>
 * Call the <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLTkDcuInfo::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::XMLCommonFec(std::string xmlFileName)</I>
 * @see <I>XMLTkDcuInfo::init()</I>
 */
XMLTkDcuInfo::XMLTkDcuInfo ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLCommonFec( xmlFileName ) {

  countDCUInfo_ = countState_ = countRun_ = countVersion_ = 0 ;
  initParameterNames() ;
}

/** delete the parameter's names
 */
XMLTkDcuInfo::~XMLTkDcuInfo (){

    TkDcuInfo::deleteParameterNames(dcuInfoParameterNames_); delete dcuInfoParameterNames_ ;
    TkState::deleteParameterNames(stateParameterNames_); delete stateParameterNames_ ;
    TkVersion::deleteParameterNames(versionParameterNames_); delete versionParameterNames_ ;
    TkRun::deleteParameterNames(runParameterNames_); delete runParameterNames_ ;
}

/** Initialise the parameter names
 */
void XMLTkDcuInfo::initParameterNames() {
  dcuInfoParameterNames_ = TkDcuInfo::getParameterNames() ;
  stateParameterNames_   = TkState::getParameterNames() ;
  versionParameterNames_ = TkVersion::getParameterNames() ;
  runParameterNames_     = TkRun::getParameterNames() ;
}

/**
 * Empty and clear the dVector
 */
void XMLTkDcuInfo::clearVector(){

  while (dVector_.size() > 0) {
    delete dVector_.back();
    dVector_.pop_back();
  }
  dVector_.clear() ;

  while (dStateVector_.size() > 0) {
    delete dStateVector_.back();
    dStateVector_.pop_back();
  }
  dStateVector_.clear() ;

  while (dVersionVector_.size() > 0) {
    delete dVersionVector_.back();
    dVersionVector_.pop_back();
  }
  dVersionVector_.clear() ;

  while (dRunVector_.size() > 0) {
    delete dRunVector_.back();
    dRunVector_.pop_back();
  }
  dRunVector_.clear() ;
}

/**Get the device Vector
 * @return the tkDcuInfoVector <I>dVector_</I> attribute
 */
tkDcuInfoVector XMLTkDcuInfo::getDcuInfoVector () {
  return dVector_;
}

/** Parses the dom documents
 * \param n - DOM node 
 */
unsigned int XMLTkDcuInfo::parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {

    if (n->getNodeType() == XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE) {

      char *name = XERCES_CPP_NAMESPACE::XMLString::transcode(n->getNodeName());

      if(n->hasAttributes()) {

	if (!strcmp(name,"DCUINFO")) {
	  
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  //unsigned int val = 
	  XMLCommonFec::parseAttributes(dcuInfoParameterNames_,pAttributes) ;
	  //if ( (val != 4) || (val != 5) ) std::cerr << "DCU Info: invalid number of parameters: " << std::dec << val << "/4" << std::endl ;
	  TkDcuInfo *dcuInfo = new TkDcuInfo (*dcuInfoParameterNames_) ;
	  dVector_.push_back(dcuInfo) ;
	  
	  countDCUInfo_ ++ ;
	}
	else if (!strcmp(name,"STATE")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  //unsigned int val = 
	  XMLCommonFec::parseAttributes(stateParameterNames_,pAttributes) ;
	  //if (val > 9) std::cerr << "State: invalid number of parameters: " << std::dec << val << "/9" << std::endl ;
	  TkState *state = new TkState (*stateParameterNames_) ;
	  dStateVector_.push_back(state) ;
	  
	  countState_ ++ ;
	}
	else if (!strcmp(name,"VERSION")) {
	  
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  //unsigned int val = 
	  XMLCommonFec::parseAttributes(versionParameterNames_,pAttributes) ;
	  //if (val != 5) std::cerr << "Version information: invalid number of parameters: " << std::dec << val << "/5" << std::endl ;
	  TkVersion *version = new TkVersion (*versionParameterNames_) ;
	  dVersionVector_.push_back(version) ;
	  
	  countVersion_ ++ ;
	}
	else if (!strcmp(name,"RUN")) {
	  
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  //unsigned int val = 
	  XMLCommonFec::parseAttributes(runParameterNames_,pAttributes) ;
	  //if (val != 11) std::cerr << "Run information: invalid number of parameters: " << std::dec << val << "/11" << std::endl ;
	  TkRun *run = new TkRun (*runParameterNames_) ;
	  dRunVector_.push_back(run) ;
	  
	  countRun_ ++ ;
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
 * @see XMLTkDcuInfo::parseXMLBuffer()
 */
tkDcuInfoVector XMLTkDcuInfo::getDcuInfos () throw (FecExceptionHandler) {

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
void XMLTkDcuInfo::parseDatabaseResponse(std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId ) 
  throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionName";

#ifdef DATABASEDEBUG
 std::cout << "void XMLTkDcuInfo::parseDatabaseResponse ( partitionName, majorVersionId, minorVersionId ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getXMLClob(partitionName,majorVersionId,minorVersionId))){

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
    catch (...) {
      RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
    }
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}



/**Send a request to the database with no parameter. Returns the last version of dcuInfos<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseDatabaseResponse() 
  throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromPartitionName";

#ifdef DATABASEDEBUG
 std::cout << "void XMLTkDcuInfo::parseDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getXMLClob())){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG, e), ERRORCODE);
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




/**Send a request to the database with no parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseDatabaseResponse(unsigned int majorVersionId, unsigned int minorVersionId) 
  throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromPartitionName";

#ifdef DATABASEDEBUG
 std::cout << "void XMLTkDcuInfo::parseDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getXMLClob(majorVersionId, minorVersionId))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG, e), ERRORCODE);
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

/**Send a request to the database with no parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseStateDatabaseResponse() throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForState";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseStateDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getCurrentStatesXMLClob())){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG, e), ERRORCODE);
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

/**Send a request to the database with no parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseFecVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForFecVersion";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseFecVersionsDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getFecVersionsXMLClob(partitionName))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG, e), ERRORCODE);
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

/**Send a request to the database with no parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseFedVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForFedVersion";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseFedVersionsDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getFedVersionsXMLClob(partitionName))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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

/**Send a request to the database with no parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseDcuInfoVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForDcuInfoVersion";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseDcuInfoVersionsDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getDcuInfoVersionsXMLClob(partitionName))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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

/**Send a request to the database with no parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseConnectionVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForConnectionVersion";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseConnectionVersionsDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getConnectionVersionsXMLClob(partitionName))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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

/**Send a request to the database with no parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseDcuPsuMapVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForDcuPsuMapVersion";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseDcuPsuMapVersionsDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getDcuPsuMapVersionsXMLClob(partitionName))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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

/**Send a request to the database with no parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseMaskVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForMaskVersion";
  
#ifdef DATABASEDEBUG
  std::cout << __PRETTY_FUNCTION__ << std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getMaskVersionsXMLClob(partitionName))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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


/**Send a request to the database<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseRunDatabaseResponse(std::string partitionName) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForRun";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseRunDatabaseResponse ( partitionName ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getLastRunXMLClob(partitionName))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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

/**Send a request to the database<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseRunDatabaseResponse() throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForRun";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseRunDatabaseResponse ( ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getAllRunsXMLClob())){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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


/**Send a request to the database<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseRunDatabaseResponse(std::string partitionName, int runNumber) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForRun";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseRunDatabaseResponse ( partitionName, runNumber ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getRunXMLClob(partitionName, runNumber))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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

/**Send a request to the database<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess have not been intialized
 */
void XMLTkDcuInfo::parseLastRunO2ODatabaseResponse(std::string partitionName) throw (FecExceptionHandler){
  std::string xmlBufferId = "theXMLBufferForRun";
  
#ifdef DATABASEDEBUG
  std::cout << "void XMLTkDcuInfo::parseLastRunO2ODatabaseResponse ( partitionName ) " <<std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbTkDcuInfoAccess*)dataBaseAccess_)->getLastO2ORunXMLClob(partitionName))){

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
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE);
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

/**Send a request to the database with the partition name as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param dcuHardId - The DCU Hard ID
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkDcuInfoVector XMLTkDcuInfo::getDcuInfos (std::string partitionName, unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler) {

  parseDatabaseResponse(partitionName, majorVersionId, minorVersionId);
  parseXMLBuffer();
  return dVector_;
}

/**Send a request to the database with the version as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkDcuInfoVector XMLTkDcuInfo::getAllDcuInfos (unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler) {
  parseDatabaseResponse(majorVersionId, minorVersionId);
  parseXMLBuffer();
  return dVector_;
}

/**Send a request to the database with no parameter. Returns the last version of DcuInfos<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkDcuInfoVector XMLTkDcuInfo::getAllDcuInfos () throw (FecExceptionHandler) {
  parseDatabaseResponse();
  parseXMLBuffer();
  return dVector_;
}

/**Send a request to the database with no parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dStateVector_</I> attribute.<BR>
 * @return <I>a COPY of dStateVector_</I> attribute. You have to delete all TkState objects + the vector!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkStateVector XMLTkDcuInfo::getAllCurrentStates () throw (FecExceptionHandler){
  parseStateDatabaseResponse();
  parseXMLBuffer();

  return dStateVector_ ;
}

/**Send a request to the database with no parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVersionVector_</I> attribute.<BR>
 * @return <I>a COPY of dVersionVector_</I> attribute. You have to delete all TkVersion objects + the vector!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkVersionVector XMLTkDcuInfo::getAllFecVersions (std::string partitionName) throw (FecExceptionHandler){
  parseFecVersionsDatabaseResponse(partitionName);
  parseXMLBuffer();

  return dVersionVector_ ;
}

/**Send a request to the database with no parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVersionVector_</I> attribute.<BR>
 * @return <I>a COPY of dVersionVector_</I> attribute. You have to delete all TkVersion objects + the vector!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkVersionVector XMLTkDcuInfo::getAllDcuInfoVersions (std::string partitionName) throw (FecExceptionHandler){
  parseDcuInfoVersionsDatabaseResponse(partitionName);
  parseXMLBuffer();

  return dVersionVector_ ;
}

/**Send a request to the database with no parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVersionVector_</I> attribute.<BR>
 * @return <I>a COPY of dVersionVector_</I> attribute. You have to delete all TkVersion objects + the vector!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkVersionVector XMLTkDcuInfo::getAllConnectionVersions (std::string partitionName) throw (FecExceptionHandler){
  parseConnectionVersionsDatabaseResponse(partitionName);
  parseXMLBuffer();

  return dVersionVector_ ;
}

/**Send a request to the database with no parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVersionVector_</I> attribute.<BR>
 * @return <I>a COPY of dVersionVector_</I> attribute. You have to delete all TkVersion objects + the vector!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkVersionVector XMLTkDcuInfo::getAllDcuPsuMapVersions (std::string partitionName) throw (FecExceptionHandler){
  parseDcuPsuMapVersionsDatabaseResponse(partitionName);
  parseXMLBuffer();

  return dVersionVector_ ;
}

/**Send a request to the database with no parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVersionVector_</I> attribute.<BR>
 * @return <I>a COPY of dVersionVector_</I> attribute. You have to delete all TkVersion objects + the vector!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkVersionVector XMLTkDcuInfo::getAllMaskVersions (std::string partitionName) throw (FecExceptionHandler){
  parseMaskVersionsDatabaseResponse(partitionName);
  parseXMLBuffer();

  return dVersionVector_ ;
}


/**Send a request to the database with no parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVersionVector_</I> attribute.<BR>
 * @return <I>a COPY of dVersionVector_</I> attribute. You have to delete all TkVersion objects + the vector!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkVersionVector XMLTkDcuInfo::getAllFedVersions (std::string partitionName) throw (FecExceptionHandler){
  parseFedVersionsDatabaseResponse(partitionName);
  parseXMLBuffer();

  return dVersionVector_ ;
}

/**Send a request to the database.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored a TkRun attribute.<BR>
 * @return a TkRun object. You have to delete this object!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
TkRun *XMLTkDcuInfo::getLastRun (std::string partitionName) throw (FecExceptionHandler){
  parseRunDatabaseResponse(partitionName);
  parseXMLBuffer();

  TkRun *run = NULL ;
  if (dRunVector_.size()) run = (*(dRunVector_.begin()))->clone() ;

  while (dRunVector_.size() > 0) {
    delete dRunVector_.back();
    dRunVector_.pop_back();
  }
  dRunVector_.clear() ;

  return run ;
}

/**Send a request to the database.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored a TkRun attribute.<BR>
 * @return a TkRun object. You have to delete this object!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
TkRun *XMLTkDcuInfo::getRun (std::string partitionName, unsigned int runNumber) throw (FecExceptionHandler){
  parseRunDatabaseResponse(partitionName, runNumber);
  parseXMLBuffer();

  TkRun *run = NULL ;
  if (dRunVector_.size()) run = (*(dRunVector_.begin()))->clone() ;

  while (dRunVector_.size() > 0) {
    delete dRunVector_.back();
    dRunVector_.pop_back();
  }
  dRunVector_.clear() ;

  return run ;
}

/**Send a request to the database.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored a TkRunVector attribute.<BR>
 * @return a TkRunVector object. You have to delete this object!
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
tkRunVector XMLTkDcuInfo::getAllRuns () throw (FecExceptionHandler){
  parseRunDatabaseResponse();
  parseXMLBuffer();

  return dRunVector_ ;
}

/**Send a request to the database.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dRunVector_</I> attribute.<BR>
 * @return a TkRun object. <I>You have to delete this object!</I>
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseXMLBuffer()</I>
 */
TkRun *XMLTkDcuInfo::getLastO2ORun (std::string partitionName) throw (FecExceptionHandler){
  parseLastRunO2ODatabaseResponse(partitionName);
  parseXMLBuffer();

  TkRun *run = NULL ;
  if (dRunVector_.size()) run = (*(dRunVector_.begin()))->clone() ;

  while (dRunVector_.size() > 0) {
    delete dRunVector_.back();
    dRunVector_.pop_back();
  }
  dRunVector_.clear() ;

  return run ;
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
void XMLTkDcuInfo::setDcuInfos ( tkDcuInfoVector dVector, std::string outputFileName) throw (FecExceptionHandler) {

  if (dVector.size()) {
    MemBufOutputSource memBufOS(dVector );
    XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);

  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in file", ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>dVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLTkDcuInfo::setDevices ( tkDcuInfoVector dVector, std::string outputFileName)
 */
void XMLTkDcuInfo::setDcuInfos ( std::string outputFileName ) throw (FecExceptionHandler) {
  setDcuInfos( dVector_, outputFileName);
}


/**Set the dcuInfo Vector
 * Empty the <I>pVector_</I> attribute
 * Delete all his elements
 * Copy all elements from <I>dcuInfos</I> parameter to <I>pVector_</I> attribute
 * @exception FecExceptionHandler : a FecExceptionHandler is raised, if <I>pVector_</I> is NULL
 */
void XMLTkDcuInfo::setDcuInfoVector (tkDcuInfoVector dcuInfos) throw (FecExceptionHandler) {
  // empty and delete previous dVector_
  // delete all elements
  if (dVector_.size()) {

    while (dVector_.size() > 0) {
      delete dVector_.back();
      dVector_.pop_back();
    }
    dVector_.clear() ;
    
    // set the tkDcuInfoVector parameter as the tkDcuInfoVector attribute
    for (tkDcuInfoVector::iterator device = dcuInfos.begin() ; device != dcuInfos.end() ; device++) {
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
void XMLTkDcuInfo::setDcuInfos (tkDcuInfoVector dVector)  throw (FecExceptionHandler) {
  MemBufOutputSource memBufOS(dVector, true);
  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = memBufOS.getOutputBuffer()->str();
#ifdef DISPLAYBUFFER
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbTkDcuInfoAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer);
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
void XMLTkDcuInfo::setDcuInfos ()  throw (FecExceptionHandler) {
  setDcuInfos(dVector_);
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
void XMLTkDcuInfo::dbConfigure() throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS(dVector_, true);

  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = memBufOS.getOutputBuffer()->str();
#ifdef DISPLAYBUFFER
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbTkDcuInfoAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer);
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG,e), ERRORCODE) ;
  }
}

#endif
