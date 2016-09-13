/*
This file is part of Fec Software project.
It is used to test the performance of DB--FecSoftware

author: HUNG Chen-Chien(Dominique), NCU, Jhong-Li, Taiwan
based on XMLFec by
Frderic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include "XMLESFec.h"
#include "stringConv.h"

#include "FecExceptionHandler.h"
#include "keyType.h"
#include "deviceDescription.h"
#include "esMbResetDescription.h"
#include "kchipDescription.h"
#include "deltaDescription.h"
#include "dcuDescription.h"
#include "paceDescription.h"
#include "pllDescription.h"

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

/**Default constructor
 */
XMLESFec::XMLESFec () :
  XMLCommonFec(),
  countPLL(0), countKchip(0), countDCUCCU(0), countDCUFEH(0), countDelta(0), countESMB(0), countPace(0)
{
};

#ifdef DATABASE
/**Constructor with Database and Partition parameters<BR>
 	* Creates a XMLESFec with access to the database :<BR>
 	* - set the database access<BR>
  * - call the <I>init</I> method<BR>
  * - download data from database and create a inputSource<BR>
  * @param dbAccess - class for the database access
  * @param partitionName - partition name
  * @see init()
  * @exception FecExceptionHandler
  */
XMLESFec::XMLESFec (ESDbAccess *dbAccess) throw (FecExceptionHandler) : 
	XMLCommonFec((DbCommonAccess *)dbAccess),
  countPLL(0), countKchip(0), countDCUCCU(0), countDCUFEH(0), countDelta(0), countESMB(0), countPace(0)
{
}
#endif

/**Constructor with Buffer parameter<BR>
 * Creates a XMLESFec from a buffer<BR>
 * - call the <I>init</I> method<BR>
 * - create a inputSource from the <I>buffer</I> parameter<BR>
 * @param buffer - xml buffer
 * @see init()
 * @exception FecExceptionHandler
 */
XMLESFec::XMLESFec (const XMLByte* buffer) throw (FecExceptionHandler) : 
	XMLCommonFec(buffer),
  countPLL(0), countKchip(0), countDCUCCU(0), countDCUFEH(0), countDelta(0), countESMB(0), countPace(0)
{
}

/**Constructor with File name parameter<BR>
 * Creates a XMLESFec from a file<BR>
 * - call the <I>init</I> method<BR>
 * - create a inputSource from the <I>xmlFileName</I> parameter<BR>
 * @param xmlFileName - xml file name
 * @see init()
 * @exception FecExceptionHandler
 */
XMLESFec::XMLESFec (std::string xmlFileName) throw (FecExceptionHandler) :
  XMLCommonFec(xmlFileName),
  countPLL(0), countKchip(0), countDCUCCU(0), countDCUFEH(0), countDelta(0), countESMB(0), countPace(0)
{
}

/**Delete the <I>parser_</I> attribute<BR>
 * Delete the <I>domDocument_</I> attribute<BR>
 * Activates the terminate function of XMLPlatformUtils, for the last occurence<BR>
 */
XMLESFec::~XMLESFec () {
}

#ifdef DATABASE
/**Send a request to the database with partition name as parameter, for the current version<BR>
 	* Get a result as a oracle::occi::Clob variable<BR>
 	* Convert this Clob as a Xerces InputSource<BR>
 	* @param partitionName - partition name
 	* @see <I>ESDbAccess::getXMLClob(std::string partitionName)</I>
 	* @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 	*    - the sql request throws a oracle::occi::SQLException
 	*    - the dataBaseAccess or the partition name have not been intialized
 	*/
void XMLESFec::parseDatabaseResponse (std::string partitionName ) throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionName";

	#ifdef DATABASEDEBUG
 		std::cout << "void XMLESFec::parseDatabaseResponse ( partitionName ) " <<std::endl ;
	#endif
  
  if (dataBaseAccess_ != NULL && partitionName != "nil") {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((ESDbAccess *)dataBaseAccess_)->getXMLClob(partitionName))){

				#ifdef DATABASEDEBUG
					display(xmlClob) ;
				#endif

				InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
				DOMInputSource *domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;
				domDocument_ = parser_->parse(*domInputSource) ;
				delete domInputSource;
      } 
      else {
				domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 
    catch (oracle::occi::SQLException &e) {
      std::stringstream msgError ;
      msgError << "unable to read XMLClob for partition name : " << partitionName << ": " << e.what() ;
      try {
	std::string errormsg = dataBaseAccess_->getErrorMessage() ;
	if (errormsg.size()) msgError << ", " << errormsg ;
      }
      catch (oracle::occi::SQLException &e) { }
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, msgError.str(), ERRORCODE) ;
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
  else {
    if (dataBaseAccess_ == NULL) {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "A partition name should be set", FATALERRORCODE) ;
    }
  }
}

/**Send a request to the database with Fec hardware identifier as parameter, for the current version<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @see <I>ESDbAccess::getXMLClob(unsigned int id, boolean hardId)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess has not been intialized
 */
void XMLESFec::parseDatabaseResponse ( std::string partitionName, std::string fecHardId ) throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromFecHardId";

	#ifdef DATABASEDEBUG
  	std::cout << "void XMLESFec::parseDatabaseResponse (" + partitionName + ", " + fecHardId + " )" <<std::endl ;
	#endif

  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((ESDbAccess *)dataBaseAccess_)->getXMLClob(partitionName, fecHardId))){
				#ifdef DATABASEDEBUG
					display(xmlClob) ;
				#endif

				InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
				DOMInputSource *domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;
				domDocument_ = parser_->parse(*domInputSource) ;
				delete domInputSource;
      } 
      else {
				domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 
    catch (oracle::occi::SQLException &e) {
      std::stringstream msgError ;
      msgError << "unable to read XMLClob for partition name : " << partitionName << ": " << e.what() ;
      try {
	std::string errormsg = dataBaseAccess_->getErrorMessage() ;
	if (errormsg.size()) msgError << ", " << errormsg ;
      }
      catch (oracle::occi::SQLException &e) { }
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, msgError.str(), ERRORCODE) ;
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

/**Send a request to the database with partition name as parameter, for a given version<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param versionMajor - version major
 * @param versionMinor - version minor
 * @see <I>ESDbAccess::getXMLClob(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess or the partition name have not been intialized
 */
void XMLESFec::parseDatabaseResponse (std::string partitionName, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionNameVersion";

	#ifdef DATABASEDEBUG
  	std::cout << "void XMLESFec::parseDatabaseResponse ( " + partitionName +  ", " + toString(versionMajor) + ", " + toString(versionMinor) + ") " <<std::endl ;
	#endif

  if (dataBaseAccess_ != NULL && partitionName != "nil") {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((ESDbAccess *)dataBaseAccess_)->getXMLClobWithVersion(partitionName, versionMajor, versionMinor))){
				#ifdef DATABASEDEBUG
					display(xmlClob) ;
				#endif
				
				InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
				DOMInputSource *domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;
				domDocument_ = parser_->parse(*domInputSource) ;
				delete domInputSource;
      } 
      else {
				domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 
    catch (oracle::occi::SQLException &e) {
      std::stringstream msgError ;
      msgError << "unable to read XMLClob for partition name : " << partitionName << ": " << e.what() ;
      try {
	std::string errormsg = dataBaseAccess_->getErrorMessage() ;
	if (errormsg.size()) msgError << ", " << errormsg ;
      }
      catch (oracle::occi::SQLException &e) { }
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, msgError.str(), ERRORCODE) ;
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
    if (dataBaseAccess_ == NULL) {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "A partition name should be set", FATALERRORCODE) ;
    }
  }
}
 

/**Send a request to the database with Fec hardware identifier as parameter, for a given version<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @param versionMajor - version major
 * @param versionMinor - version minor
 * @see <I>ESDbAccess::getXMLClob(unsigned int id, boolean hardId, unsigned int versionMajor, unsigned int versionMinor)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess has not been intialized
 */
void XMLESFec::parseDatabaseResponse ( std::string partitionName, std::string fecHardId, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromIpNumberXdaqInstanceClassName";

	#ifdef DATABASEDEBUG
  	std::cout << "void XMLESFec::parseDatabaseResponse ( " + partitionName + ", " + fecHardId + ", " + toString(versionMajor) + ", " + toString(versionMinor) + " ) " <<std::endl ;
	#endif

  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((ESDbAccess *)dataBaseAccess_)->getXMLClobWithVersion(partitionName, fecHardId, versionMajor, versionMinor))){
				#ifdef DATABASEDEBUG
					display(xmlClob) ;
				#endif

				InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
				DOMInputSource *domInputSource = new Wrapper4InputSource(xmlInputSource) ;
				domDocument_ = parser_->parse(*domInputSource) ;
				delete domInputSource;
      } 
      else {
				domDocument_ = NULL ;
      } // end if ((xmlClob =...
    }
    catch (oracle::occi::SQLException &e) {
      std::stringstream msgError ;
      msgError << "unable to read XMLClob for partition name : " << partitionName << ": " << e.what() ;
      try {
	std::string errormsg = dataBaseAccess_->getErrorMessage() ;
	if (errormsg.size()) msgError << ", " << errormsg ;
      }
      catch (oracle::occi::SQLException &e) { }
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, msgError.str(), ERRORCODE) ;
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

#endif

