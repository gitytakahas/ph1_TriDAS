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
#include "XMLFec.h"
#include "stringConv.h"

#include "FecExceptionHandler.h"
#include "keyType.h"
#include "deviceDescription.h"
#include "piaResetDescription.h"
#include "apvDescription.h"
#include "muxDescription.h"
#include "dcuDescription.h"
#include "laserdriverDescription.h"
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
XMLFec::XMLFec () :
  XMLCommonFec(),
  countPLL(0), countAPV25(0), countDCUCCU(0), countDCUFEH(0), countAPVMUX(0), countPIA(0), countAOH(0), countDOH(0)
{
};

#ifdef DATABASE
/**Constructor with Database and Partition parameters<BR>
 * Creates a XMLFec with access to the database :<BR>
 * - set the database access<BR>
 * - call the <I>init</I> method<BR>
 * - download data from database and create a inputSource<BR>
 * @param dbAccess - class for the database access
 * @param partitionName - partition name
 * @see init()
 * @exception FecExceptionHandler
 */
XMLFec::XMLFec (DbAccess *dbAccess) throw (FecExceptionHandler) :
  XMLCommonFec((DbCommonAccess *)dbAccess),
  countPLL(0), countAPV25(0), countDCUCCU(0), countDCUFEH(0), countAPVMUX(0), countPIA(0), countAOH(0), countDOH(0)
{
}
#endif

/**Constructor with Buffer parameter<BR>
 * Creates a XMLFec from a buffer<BR>
 * - call the <I>init</I> method<BR>
 * - create a inputSource from the <I>buffer</I> parameter<BR>
 * @param buffer - xml buffer
 * @see init()
 * @exception FecExceptionHandler
 */
XMLFec::XMLFec (const XMLByte* buffer) throw (FecExceptionHandler) :
  XMLCommonFec(buffer),countPLL(0), countAPV25(0), countDCUCCU(0), countDCUFEH(0), countAPVMUX(0), countPIA(0), countAOH(0), countDOH(0)
{
}

/**Constructor with File name parameter<BR>
 * Creates a XMLFec from a file<BR>
 * - call the <I>init</I> method<BR>
 * - create a inputSource from the <I>xmlFileName</I> parameter<BR>
 * @param xmlFileName - xml file name
 * @see init()
 * @exception FecExceptionHandler
 */
XMLFec::XMLFec (std::string xmlFileName) throw (FecExceptionHandler) :
  XMLCommonFec(xmlFileName),
  countPLL(0), countAPV25(0), countDCUCCU(0), countDCUFEH(0), countAPVMUX(0), countPIA(0), countAOH(0), countDOH(0)
{
}

/**Delete the <I>parser_</I> attribute<BR>
 * Delete the <I>domDocument_</I> attribute<BR>
 * Activates the terminate function of XMLPlatformUtils, for the last occurence<BR>
 */
XMLFec::~XMLFec () {
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
void XMLFec::parseDatabaseResponse ( std::string partitionName ) 
  throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionName";

#ifdef DATABASEDEBUG
  std::cout << __PRETTY_FUNCTION__ << " => " << partitionName << std::endl ;
#endif
  
  if (dataBaseAccess_ != NULL && partitionName != "nil") {
    oracle::occi::Clob *xmlClob;
    try {

      if ((xmlClob = ((DbAccess *)dataBaseAccess_)->getXMLClob(partitionName))){

#ifdef DATABASEDEBUG
	display(xmlClob) ;
#endif

	InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	DOMInputSource *domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;
      } else {
	domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 

    catch (oracle::occi::SQLException &e) {
      std::stringstream msg ; msg << "unable to read XMLClob for partition name : " << partitionName ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, XMLCommonFec::what(msg.str(), e), ERRORCODE) ;
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

/**Send a request to the database with Fec hardware identifier as parameter, for the current version<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @see <I>DbAccess::getXMLClob(unsigned int id, boolean hardId)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess has not been intialized
 */
void XMLFec::parseDatabaseResponse ( std::string partitionName, std::string fecHardId ) 
  throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromFecHardId";

#ifdef DATABASEDEBUG
  std::cout << "void XMLFec::parseDatabaseResponse (" + partitionName + ", " + fecHardId + " )" <<std::endl ;
#endif

  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbAccess *)dataBaseAccess_)->getXMLClob(partitionName, fecHardId))){

#ifdef DATABASEDEBUG
	display(xmlClob) ;
#endif

	InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	DOMInputSource *domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;
      } else {
	domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 
    catch (oracle::occi::SQLException &e) {
      std::stringstream msg ; msg << DB_PLSQLEXCEPTIONRAISED_MSG << " for partition name : " << partitionName << " and FEC " << fecHardId ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, XMLCommonFec::what(msg.str(), e), ERRORCODE) ;
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
 * @see <I>DbAccess::getXMLClob(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess or the partition name have not been intialized
 */
void XMLFec::parseDatabaseResponse (std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor ) 
  throw (FecExceptionHandler) {

  std::string xmlBufferId = "theXMLBufferFromPartitionNameVersion";

#ifdef DATABASEDEBUG
  std::cout << "void XMLFec::parseDatabaseResponse ( " + partitionName +  ", " + toString(versionMajor) + ", " + toString(versionMinor) +  ", " + toString(maskVersionMajor) + ", " + toString(maskVersionMinor) + ") " << std::endl ;
#endif

  if (dataBaseAccess_ != NULL && partitionName != "nil") {
    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbAccess *)dataBaseAccess_)->getXMLClobWithVersion(partitionName, versionMajor, versionMinor, maskVersionMajor, maskVersionMinor))){

#ifdef DATABASEDEBUG
	display(xmlClob) ;
#endif

	InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	DOMInputSource *domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;
      } else {
	domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } 
    catch (oracle::occi::SQLException &e) {
      std::stringstream msg ; msg << DB_PLSQLEXCEPTIONRAISED_MSG << " for partition name : " << partitionName << " and version " << versionMajor << "." << versionMinor ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, XMLCommonFec::what(msg.str(), e), ERRORCODE) ;
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
      RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR, "A partition should be set", FATALERRORCODE) ;
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
 * @see <I>DbAccess::getXMLClob(unsigned int id, boolean hardId, unsigned int versionMajor, unsigned int versionMinor)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess has not been intialized
 */
void XMLFec::parseDatabaseResponse ( std::string partitionName, std::string fecHardId, unsigned int versionMajor, unsigned int versionMinor ) 
  throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromIpNumberXdaqInstanceClassName";

#ifdef DATABASEDEBUG
  std::cout << "void XMLFec::parseDatabaseResponse ( " + partitionName + ", " + fecHardId + ", " + toString(versionMajor) + ", " + toString(versionMinor) + " ) " <<std::endl ;
#endif

  if (dataBaseAccess_ != NULL) {

    oracle::occi::Clob *xmlClob;
    try {
      if ((xmlClob = ((DbAccess *)dataBaseAccess_)->getXMLClobWithVersion(partitionName, fecHardId, versionMajor, versionMinor))){

#ifdef DATABASEDEBUG
	display(xmlClob) ;
#endif

	InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	DOMInputSource *domInputSource = new Wrapper4InputSource(xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;
      } else {
	domDocument_ = NULL ;
      } // end if ((xmlClob =...
    } catch (oracle::occi::SQLException &e) {
      std::stringstream msg ; msg << DB_PLSQLEXCEPTIONRAISED_MSG << " for partition name : " << partitionName << ", FEC " << fecHardId << " and version " << versionMajor << "." << versionMinor ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, XMLCommonFec::what(msg.str(), e), ERRORCODE) ;
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

