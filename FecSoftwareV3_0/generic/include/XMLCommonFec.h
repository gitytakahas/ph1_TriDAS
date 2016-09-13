/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/


#ifndef XMLCOMMONFEC_H
#define XMLCOMMONFEC_H

#ifdef DATABASE
#include "DbCommonAccess.h"
#endif

#include "FecExceptionHandler.h"

// use in XMLCommonFec to declare ofstream
#include <fstream>
#include <sstream>

#include <vector>
#include <string>

#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>

#include "deviceDescription.h"

class DOMCountErrorHandler ;

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).<BR>
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 *
 * SAX parser(<I>parser_</I>) and Sax handler(<I>handler_</I>) are XMLCommonFec attributes.<BR>
 * Otherwise, <I>handler_</I> will be instanciated in subclasses, depending what you need (Device, PiaReset,...).<BR> 
 * Features for <I>parser_</I> (error handler, document handler) will also be set in subclasses, once the <I>handler_</I> is created.<BR>
 */
class XMLCommonFec {

 private:
  static unsigned int nbInstance ;

 protected:

#ifdef DATABASE
  /**
   * Database access
   */
  DbCommonAccess *dataBaseAccess_;

  /**
   * SQL request
   */
  std::string stringRequest_ ;
#endif

  /**
   * DOM implementation
   */
  XERCES_CPP_NAMESPACE::DOMImplementation *domImplementation_ ;

  /**
   * DOM Parser
   */
   XERCES_CPP_NAMESPACE::DOMBuilder *parser_ ;

  /**
   * DOM Parser error handler
   */
  DOMCountErrorHandler *domCountErrorHandler_ ;

  /**
   * XML buffer
   */
  XMLByte *xmlBuffer_;

  /**
   * DOMDocument for parsing
   */
  XERCES_CPP_NAMESPACE::DOMDocument *domDocument_ ;

  /**
   * Number of elements found in the DOM
   */
  unsigned int countElement_ ;

  /**
   * The XML buffer should be deleted 
   */
  bool toBeDeleted_ ;

 protected :
  /**
   * \brief Initialize Xerces XMLPlatformUtils and create a parser
   */
  void init() throw (FecExceptionHandler);

  /**
   * \brief Reads the parameter values from an XML buffer
   */
  void readXMLBuffer(const XMLByte* buffer) throw (FecExceptionHandler);

  /**
   * \brief Reads the parameter values from an XML file
   */
  void readXMLFile(std::string xmlFileName) throw (FecExceptionHandler);

  /**
   * \brief Creates a SAX parser
   */
  void createParser() throw (FecExceptionHandler);
  
#ifdef DATABASE
  /**
   * \brief Creates a XERCES InputSource from a database Clob
   */
  XERCES_CPP_NAMESPACE::InputSource *createInputSource(oracle::occi::Clob *xmlClob, std::string xmlBufferId);
#endif
  
 public:
  /** \brief Default constructor
   */
  XMLCommonFec ();

#ifdef DATABASE
  /** \brief Constructor with the database
   */
  XMLCommonFec(DbCommonAccess* dbCommonAccess) ;
#endif

  /** \brief Constructor with xml buffer
   */
  XMLCommonFec ( const XMLByte* buffer, bool toBeDeleted = true ) throw (FecExceptionHandler);

  /** \brief Constructor with file access
   */
  XMLCommonFec ( std::string xmlFileName ) throw (FecExceptionHandler);

  /** \brief Deletes the device vector private attribute
   */
  virtual ~XMLCommonFec ();

  /** \brief clear the vector
   */
  virtual void clearVector() = 0;

  /** \brief DOM parser
   */
  virtual unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) = 0 ;
  
  /** \brief Parses the xml Buffer using the SAX interface
   */
  void parseXMLBuffer( ) throw (FecExceptionHandler);

  /** \brief Return the non fatal parsing error number
   */
  int getParseErrorCount( ) throw (FecExceptionHandler);

  /** \brief Return the non fatal parsing error messages
   */
  std::vector<std::string> *getParseErrorMessages( ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Sets the database access attribute
   */
  void setDatabaseAccess ( DbCommonAccess *dbAccess);

  /** \brief Sets the SQL request
   */
  void setDatabaseRequest ( std::string readString );

  /** \brief This method sends a request to the database and parse the input source retrieved from database
   */
  void parseDatabaseResponse ( ) throw (FecExceptionHandler);

  /** \brief Sets the SQL request
   */
  std::string getDatabaseRequest ();

  /** \brief return an error message from oracle with the user message
   */
  std::string what ( std::string message, oracle::occi::SQLException &ex ) {
    
    std::stringstream msgError ;

    if (dataBaseAccess_ != NULL) msgError << dataBaseAccess_->what(message,ex) ;
    else msgError << message << ": " << ex.what() ;
    
    return msgError.str() ;
  }
#endif

  /** \brief Parse elements with ParameterDescription type
   */
  static unsigned int parseAttributes(parameterDescriptionNameType *parameterNames, XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes) ;

  /** \brief Writes the parameter values into an XML file
   */
  static void writeXMLFile(std::string xmlBuffer, std::string xmlFileName) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Display an XML clob buffer
   */
  static void display ( oracle::occi::Clob *xmlClob ) {

    const XMLByte *xmlBuffer;
  
    unsigned int resultLength = (*xmlClob).length();
    if ((xmlBuffer = (XMLByte *) calloc(resultLength+1, sizeof(char)))) {
    if (resultLength != (*xmlClob).read(resultLength, (unsigned char *)xmlBuffer, resultLength+1)) {
      std::cerr << "Error during the display of a XML clob, wrong size of the buffer" << std::endl ;
    }
    else
      std::cout << "XMLFec::createInputSource ( ): " << xmlBuffer << std::endl ;
    }

    delete xmlBuffer ;
  }
#endif

} ;

// ---------------------------------------------------------------------------
//  Simple error handler deriviative to install on parser
// ---------------------------------------------------------------------------
class DOMCountErrorHandler: public XERCES_CPP_NAMESPACE::DOMErrorHandler {

public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    DOMCountErrorHandler();
    ~DOMCountErrorHandler();


    /**
     * \brief return the number of errors
     */
    unsigned int getSawErrors() const;

    /**
     * \brief return the list of messages
     */
    std::vector<std::string> *getErrorMessages() ;

    // -----------------------------------------------------------------------
    //  Implementation of the DOM ErrorHandler interface
    // -----------------------------------------------------------------------
    /**
     * \brief handle the errors in the parsing
     */
    bool handleError(const XERCES_CPP_NAMESPACE::DOMError& domError);

    /**
     * \brief reset all errors
     */
    void resetErrors();

private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    DOMCountErrorHandler(const DOMCountErrorHandler&);
    void operator=(const DOMCountErrorHandler&);


    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. Its used by the main code to suppress output if there are
    //      errors.
    // -----------------------------------------------------------------------

    /** Number of errors
     */
    unsigned int fSawErrors_ ;

    /** List of error messages
     */
    std::vector<std::string> *vErrorMessages_ ;

};

#endif // END OF FILE
