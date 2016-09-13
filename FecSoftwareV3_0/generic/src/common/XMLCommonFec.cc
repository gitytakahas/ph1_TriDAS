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
#include "XMLCommonFec.h"
#include "stringConv.h"

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

pthread_mutex_t mutexXMLInit_=PTHREAD_MUTEX_INITIALIZER ;
unsigned int XMLCommonFec::nbInstance = 0 ;

//#define DATABASEDEBUG

/** \brief Default constructor
 */
XMLCommonFec::XMLCommonFec ():
#ifdef DATABASE      
  dataBaseAccess_(NULL),
#endif

  domImplementation_(NULL),
  parser_(NULL),
  domCountErrorHandler_(NULL),
  xmlBuffer_(NULL),
  domDocument_(NULL),
  toBeDeleted_(true)
{
  init();
}

#ifdef DATABASE
/** \brief Default constructor
 */
XMLCommonFec::XMLCommonFec (DbCommonAccess *dbAccess):
#ifdef DATABASE      
  dataBaseAccess_ (dbAccess),
#endif

  domImplementation_(NULL),
  parser_(NULL),
  domCountErrorHandler_(NULL),
  xmlBuffer_(NULL),
  domDocument_(NULL),
  toBeDeleted_(true)
{
  init();
}
#endif

/** \brief Constructor with xml buffer
 */
XMLCommonFec::XMLCommonFec ( const XMLByte* buffer, bool toBeDeleted ) throw (FecExceptionHandler):

  domImplementation_(NULL),
  parser_(NULL),
  domCountErrorHandler_(NULL),
  domDocument_(NULL),
  toBeDeleted_(toBeDeleted)
{
  init();
  readXMLBuffer(buffer);
}

  /** \brief Constructor with file access
   */
XMLCommonFec::XMLCommonFec ( std::string xmlFileName ) throw (FecExceptionHandler) :

  domImplementation_(NULL),
  parser_(NULL),
  domCountErrorHandler_(NULL),
  xmlBuffer_(NULL),
  domDocument_(NULL),
  toBeDeleted_(true)
{
  init();
  readXMLFile(xmlFileName);
}


  /** \brief Deletes the device vector private attribute
   */
XMLCommonFec::~XMLCommonFec (){

  if ( (toBeDeleted_) && (xmlBuffer_ != NULL) ) free((void*)xmlBuffer_) ;

  if (parser_ != NULL) {
    parser_->release() ;
  }

  if (domCountErrorHandler_ != NULL) delete domCountErrorHandler_ ;

  pthread_mutex_lock(&mutexXMLInit_);
  if (--nbInstance == 0) {
    XMLPlatformUtils::Terminate();
  }
  pthread_mutex_unlock(&mutexXMLInit_);

}

  /** \brief Parses the xml Buffer using DOM interface
   */
void XMLCommonFec::parseXMLBuffer( ) throw (FecExceptionHandler) {

  clearVector();
  // -----------------------------------------------------

  if (domDocument_) {

    countElement_ = parseAttributes ( (XERCES_CPP_NAMESPACE::DOMNode*)domDocument_->getDocumentElement() ) ;

    if (domCountErrorHandler_->getSawErrors()) {

      std::vector<std::string> *errors = domCountErrorHandler_->getErrorMessages() ;
      if (errors->size())
	RAISEFECEXCEPTIONHANDLER ( XML_PARSINGERROR, XML_PARSINGERROR_MSG + " saw " + toString(domCountErrorHandler_->getSawErrors()) + " errors in parsing: " + *(errors->begin()), ERRORCODE);
      else
      RAISEFECEXCEPTIONHANDLER ( XML_PARSINGERROR, XML_PARSINGERROR_MSG + " saw " + toString(domCountErrorHandler_->getSawErrors()) + " errors in the parsing", ERRORCODE);
    }
  }
  else
    RAISEFECEXCEPTIONHANDLER ( XML_PARSINGERROR, "unable to parse the XML Buffer, domDocument is not initialized", ERRORCODE);
}
  /** \brief Return the non fatal parsing error number
   */
int XMLCommonFec::getParseErrorCount( ) throw (FecExceptionHandler){

  if (domCountErrorHandler_ != NULL) 
    return domCountErrorHandler_->getSawErrors() ;
  else 
    RAISEFECEXCEPTIONHANDLER ( XML_PARSINGERROR, "unable to get the parser error count error handler not initialized", ERRORCODE);
}

  /** \brief Return the non fatal parsing error messages
   */
std::vector<std::string> *XMLCommonFec::getParseErrorMessages( ) throw (FecExceptionHandler){

  if (domCountErrorHandler_ != NULL) 
    return domCountErrorHandler_->getErrorMessages() ;
  else 
    RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR, "Unable to get the parser error count, handler not initialized", FATALERRORCODE );
}


  /** \brief Writes the parameter values into an XML file
   */
void XMLCommonFec::writeXMLFile(std::string xmlBuffer, std::string xmlFileName) throw (FecExceptionHandler){
  if (FileHandle toWriteFileHandle =  XMLPlatformUtils::openFileToWrite(xmlFileName.c_str())) {
      XMLPlatformUtils::writeBufferToFile(toWriteFileHandle, xmlBuffer.size(), (const XMLByte *)(xmlBuffer.c_str()));
      XMLPlatformUtils::closeFile(toWriteFileHandle);
  } else {
    RAISEFECEXCEPTIONHANDLER ( FILEPROBLEMERROR, "Unable to open XML file" + xmlFileName, ERRORCODE);
  }
}

#ifdef DATABASE
  /** \brief Sets the database access attribute
   */
void XMLCommonFec::setDatabaseAccess ( DbCommonAccess *dbAccess) {
  dataBaseAccess_ = dbAccess;
}

/** \brief Sets the SQL request
 */
void XMLCommonFec::setDatabaseRequest ( std::string request ) {
  stringRequest_ = request;
}

/** \brief Sets the SQL request
 */
std::string XMLCommonFec::getDatabaseRequest () {
  return stringRequest_ ;
}

#endif

void XMLCommonFec::init() throw (FecExceptionHandler){

  //std::cout << "Using DOM parser" << std::endl ;
  pthread_mutex_lock(&mutexXMLInit_);
  if (nbInstance++ == 0) {
    //  Initializes the XMLPlatformUtils
    XMLPlatformUtils::Initialize();
  }
  pthread_mutex_unlock(&mutexXMLInit_);
  createParser();
}

  /**
   * \brief Reads the parameter values from an XML buffer
   */
void XMLCommonFec::readXMLBuffer(const XMLByte* buffer) throw (FecExceptionHandler){

  std::string xmlBufferId = "theXMLBuffer";
  //std::cout<<"readXMLBuffer"<<std::endl;
  //XercesDefs.hpp typedef unsigned char XMLByte
  if (buffer) {

    try {
      InputSource *xmlInputSource =  new MemBufInputSource ( (const XMLByte*)(buffer), strlen((const char*)buffer), xmlBufferId.c_str());
      DOMInputSource *domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;
      domDocument_ = parser_->parse(*domInputSource) ;
      delete domInputSource ;
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

    domDocument_ = NULL ;
    RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_BUFFEREMPTY_MSG, ERRORCODE) ;
  }
}

/**
 * \brief Reads the parameter values from an XML file
 */
void XMLCommonFec::readXMLFile(std::string xmlFileName) throw (FecExceptionHandler){

  XMLCh* fileName = XMLString::transcode(xmlFileName.c_str());
  XMLURL urlFileName = XMLURL(fileName);

  try {
    domDocument_ = parser_->parseURI(fileName) ;
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
  XMLString::release(&fileName);
}

  /**
   * \brief Creates a SAX parser
   */
void XMLCommonFec::createParser() throw (FecExceptionHandler){

  try {
    // Instantiate the DOM parser.
    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    domImplementation_ = DOMImplementationRegistry::getDOMImplementation(gLS);
    parser_ = ((DOMImplementationLS*)domImplementation_)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    
    bool doNamespaces       = false;
    bool doSchema           = false;
    bool schemaFullChecking = false;
    parser_->setFeature(XMLUni::fgDOMNamespaces, doNamespaces);
    parser_->setFeature(XMLUni::fgXercesSchema, doSchema);
    parser_->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
    parser_->setFeature(XMLUni::fgDOMValidateIfSchema, true); 
    
    // enable datatype normalization - default is off
    parser_->setFeature(XMLUni::fgDOMDatatypeNormalization, true);
    // And create our error handler and install it
    domCountErrorHandler_ = new DOMCountErrorHandler() ;
    parser_->setErrorHandler(domCountErrorHandler_);
    
    // reset document pool
    domCountErrorHandler_->resetErrors() ;
    parser_->resetDocumentPool();
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
}

#ifdef DATABASE
/**
 * \brief Creates a XERCES InputSource from a database Clob
 */
InputSource *XMLCommonFec::createInputSource(oracle::occi::Clob *xmlClob, std::string xmlBufferId){

  InputSource* xmlInputSource;

  //const XMLByte *xmlBuffer;
  if (xmlBuffer_ != NULL) free((void*)xmlBuffer_) ;
  unsigned int resultLength = (*xmlClob).length();

  if ((xmlBuffer_ = (XMLByte *) calloc(resultLength+1, sizeof(char)))) {
    if (resultLength != (*xmlClob).read(resultLength, (unsigned char *)xmlBuffer_, resultLength+1)) {
      xmlInputSource = NULL;
      RAISEFECEXCEPTIONHANDLER ( XML_ALLOCATIONPROBLEM, XML_ALLOCATIONPROBLEM_MSG + ": unable to initialize xmlBuffer", FATALERRORCODE ) ;
    }
    else {
#ifdef DATABASEDEBUG
      std::cout << __PRETTY_FUNCTION__ << ": " << xmlBuffer_ << std::endl ;
#endif
      //this MemBufInpurSource object should not adopt the buffer (i.e. does not make a copy of it) : it just use it in place (default way)
      //the caller must insure that it remains valid until the input source object is destroyed.
      xmlInputSource = new MemBufInputSource ( (const XMLByte*)(xmlBuffer_), strlen((const char*)xmlBuffer_), xmlBufferId.c_str());
      
    } // end if (resultLength !=... 

  } else {

    xmlInputSource = NULL;
    RAISEFECEXCEPTIONHANDLER ( XML_ALLOCATIONPROBLEM, XML_ALLOCATIONPROBLEM_MSG, FATALERRORCODE ) ;
  } // end if ((xmlBuffer =...

  return xmlInputSource;
}

/**Send a request to the database with Fec hardware identifier as parameter, for the current version<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @see <I>DbAccess::getXMLClob(unsigned int id, boolean hardId)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess has not been intialized
 */
void XMLCommonFec::parseDatabaseResponse ()  throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromFecHardId";

  if (dataBaseAccess_ != NULL) {

    oracle::occi::Clob *xmlClob;

    try {

      //unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
      if ((xmlClob = dataBaseAccess_->getXMLClobFromQuery(stringRequest_)) != NULL){
	//unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();	
	//std::cout << "Retreive from database is = " << (endMillis-startMillis) << " ms" << std::endl ;

	try {
	  InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	  DOMInputSource *domInputSource = new Wrapper4InputSource(xmlInputSource) ;
	  domDocument_ = parser_->parse(*domInputSource) ;
	  delete domInputSource ;
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
	domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 
    catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG, e), ERRORCODE) ;
    }
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}
#endif

/** Take the element from parameterNames and associate the corresponding attributes
 * \param parameterNames - list of ParameterName elements (name, value)
 * \param pAttributes - attributes of the DOM
 * \warning Oracle does not mention attributes which have a NULL value into the XML buffer. So, the retrieved value is not changed and then, the old value will be get !!! The solution is basically to manually set a default value after getting the value
 */
unsigned int XMLCommonFec::parseAttributes(parameterDescriptionNameType *parameterNames, DOMNamedNodeMap *pAttributes) {
  
  unsigned int val = 0 ;
  for (unsigned int i=0; i<pAttributes->getLength(); i++){
    
    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
    char *name = XMLString::transcode(pAttributeNode->getName());
    char *value = XMLString::transcode(pAttributeNode->getValue());

#ifdef DATABASEDEBUG
    std::cerr << "name: " <<  name << "='" << value << "'" << std::endl;
#endif

    if ( (*parameterNames).find(name) != (*parameterNames).end() ) {
      (*parameterNames)[name]->setValue(value);
      val++ ;
    }
    else {
      std::cerr << "Online running> did not find the name in the parameter descriptions: " << name << std::endl ;
      std::cerr << "parameterNames size " << parameterNames->size();
      for (parameterDescriptionNameType::iterator it=(*parameterNames).begin(); it!=(*parameterNames).end(); it++) {
	std::cerr << (*it).first << " " <<(*it).second<<std::endl;
      }
    }
    XMLString::release(&name);
    XMLString::release(&value);
  }

  return val ;
}

// ---------------------------------------------------------------------------------------------
// DOM Error handler
// ---------------------------------------------------------------------------------------------
DOMCountErrorHandler::DOMCountErrorHandler() :

  fSawErrors_(0) {

  vErrorMessages_ = new std::vector<std::string> ;
}

DOMCountErrorHandler::~DOMCountErrorHandler() {
  
  // delete the error list
  delete vErrorMessages_ ;
}

// ---------------------------------------------------------------------------
//  DOMCountHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool DOMCountErrorHandler::handleError(const DOMError& domError) {

  std::ostringstream errorMessage ;

  fSawErrors_++ ;
  if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING) errorMessage << "Warning: " ;
  else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR) errorMessage << "Error: " ;
  else errorMessage << "Fatal error: " ;

  errorMessage << StrX(domError.getLocation()->getURI())
	       << ", line " << domError.getLocation()->getLineNumber()
	       << ", char " << domError.getLocation()->getColumnNumber()
	       << "\n  Message: " << StrX(domError.getMessage()) << XERCES_STD_QUALIFIER endl;

  vErrorMessages_->push_back(errorMessage.str()) ;

  return true;
}

void DOMCountErrorHandler::resetErrors() {

  fSawErrors_ = 0 ;
  vErrorMessages_->clear() ;
}

std::vector<std::string> *DOMCountErrorHandler::getErrorMessages() {
  return vErrorMessages_ ;
}

unsigned int DOMCountErrorHandler::getSawErrors() const {
  return fSawErrors_ ;
}

