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
#include "XMLESFecMbReset.h"

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

/**Create an XMLESFec dedicated for MbReset<BR>
 * Call the <I>XMLESFec::XMLESFec()</I> constructor<BR>
 * Call the method <I>XMLESFecMbReset::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::XMLESFec()</I>
 * @see <I>XMLESFecMbReset::init()</I>
 */
XMLESFecMbReset::XMLESFecMbReset () throw (FecExceptionHandler) : XMLESFec() {
}

#ifdef DATABASE
/**Creates a XMLESFecMbReset with access to the database<BR>
 * Call the <I>XMLESFec::XMLESFec(DbAccess *dbAccess, std::string partitionName)</I> constructor<BR>
 * Call the method <I>XMLESFecMbReset::init()</I><BR>
 * @param dbAccess - class for the database access
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::XMLESFec(DbAccess *dbAccess, std::string partitionName)</I>
 * @see <I>XMLESFecMbReset::init()</I>
 */
XMLESFecMbReset::XMLESFecMbReset ( ESDbMbResetAccess *dbAccess ) throw (FecExceptionHandler) : XMLESFec( dbAccess ){
}
#endif

/**Creates a XMLESFecMbReset from a buffer<BR>
 * Call the <I>XMLESFec::XMLESFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLESFecMbReset::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::XMLESFec(const XMLByte* xmlBuffer)</I>
 * @see <I>XMLESFecMbReset::init()</I>
 */
XMLESFecMbReset::XMLESFecMbReset (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLESFec( xmlBuffer ){
  
}

/**Creates a XMLESFecMbReset from a file<BR>
 * Call the <I>XMLESFec::XMLESFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLESFecMbReset::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::XMLESFec(std::string xmlFileName)</I>
 * @see <I>XMLESFecMbReset::init()</I>
 */
XMLESFecMbReset::XMLESFecMbReset ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLESFec( xmlFileName ) {
}

/**Delete the <I>handler_</I> attribute<BR>
 * Delete the device vector <I>pVector_</I> attribute<BR>
 */
XMLESFecMbReset::~XMLESFecMbReset (){
}

/**
 * Empty and clear the dVector
 */
void XMLESFecMbReset::clearVector(){
   while (pVector_.size() > 0) {
     delete pVector_.back();
     pVector_.pop_back();
   }
    
   pVector_.clear() ;
}


/**Get the device Vector
 * @return the esMbResetVector <I>pVector_</I> attribute
 */
esMbResetVector XMLESFecMbReset::getEsMbResetVector () {
  return pVector_;
}

/**Set the device Vector
 * Empty the <I>pVector_</I> attribute
 * Delete all his elements
 * Copy all elements from <I>devices</I> parameter to <I>pVector_</I> attribute
 * @exception FecExceptionHandler : a FecExceptionHandler is raised, if <I>pVector_</I> is NULL
 */
void XMLESFecMbReset::setEsMbResetVector (esMbResetVector piaResets) throw (FecExceptionHandler) {
  // empty and delete previous esMbResetVector
  // delete all elements
  clearVector();
  
 	// set the esMbResetVector parameter as the esMbResetVector attribute
  for (esMbResetVector::iterator device = piaResets.begin() ; device != piaResets.end() ; device++) {
      pVector_.push_back ((*device)) ;
  }
}

/**Parses the <I>xmlInputSource_</I> attribute using the SAX interface<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLESFecMbReset::parseXMLBuffer()
 */
esMbResetVector XMLESFecMbReset::getEsMbResets () throw (FecExceptionHandler) {

  parseXMLBuffer();

  return pVector_;
}

#ifdef DATABASE

/**Send a request to the database with partition name as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>pVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @return <I>pVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
esMbResetVector XMLESFecMbReset::getEsMbResets (std::string partitionName) throw (FecExceptionHandler) {

  //unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseDatabaseResponse(partitionName);
  //unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "ParseDatabaseResponse = " << (endMillis-startMillis) << " ms" << std::endl ;

  //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseXMLBuffer();
  //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "parseXMLBuffer = " << (endMillis-startMillis) << " ms" << std::endl ;

  return pVector_;
}


/**Send a request to the database with Fec hardware identifier as parameter, for the current version.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>pVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @return <I>pVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
esMbResetVector XMLESFecMbReset::getEsMbResets (std::string partitionName, std::string fecHardId) throw (FecExceptionHandler) {

  //unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseDatabaseResponse(partitionName, fecHardId);
  //unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "ParseDatabaseResponse = " << (endMillis-startMillis) << " ms" << std::endl ;
  //std::cout << "!!! XMLESFecMbReset::getEsMbResets after parseDatabaseRespons " << std::endl;
  //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseXMLBuffer();
  //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "parseXMLBuffer = " << (endMillis-startMillis) << " ms" << std::endl ;
  //std::cout << "!!! XMLESFecMbReset::getEsMbResets after parseXMLBuffer " << std::endl;
  return pVector_;
}
#endif


/**Generates an XML buffer from the parameter <I>pVector</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param pVector - device list
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if
 *     - <I>pVector</I> is not initialized
 *     - the MemBufOutputSource created from the parameter <I>pVector</I> is not initialized 
 * @see XMLCommonFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 */
void XMLESFecMbReset::setEsFileMbResets ( esMbResetVector pVector, std::string outputFileName) throw (FecExceptionHandler) {

  if (pVector.size()) {
    esMemBufOutputSource memBufOS(pVector);
    XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName); 
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>pVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLESFecMbReset::setEsFileMbResets ( esMbResetVector pVector, std::string outputFileName)
 */
void XMLESFecMbReset::setEsFileMbResets ( std::string outputFileName) throw (FecExceptionHandler) {
  setEsFileMbResets( pVector_, outputFileName);
}

/**Displays the contents of the attribute <I>pVector_</I>.
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if <I>pVector</I> is not initialized
 */
void XMLESFecMbReset::display() {
	
	for (esMbResetVector::iterator it = pVector_.begin() ; it != pVector_.end() ; it ++) {
  	esMbResetDescription *piaResetd = *it;
    piaResetd->display();
  }
}

#ifdef DATABASE
/**Generates an XML buffer from the parameter <I>pVector</I><BR>.
* \param pVector - device description vector to be stored to the database
 * \param partitionName - partition name
 * \exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 * \see ESDbAccess::setXMLClob(std::string xmlBuffer, std::string partitionName);
 */
void XMLESFecMbReset::setEsDbMbResets (esMbResetVector pVector, std::string partitionName)  throw (FecExceptionHandler) {

  esMemBufOutputSource memBufOS(pVector, true);

  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = memBufOS.getEsMbResetOutputBuffer()->str();
#ifdef DATABASEDEBUG
      std::cout << xmlBuffer << std::endl;
#endif
      ((ESDbMbResetAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer, partitionName);
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch(oracle::occi::SQLException &e) {
    std::stringstream errorMessage; errorMessage << e.what();
    std::string localMessage = dataBaseAccess_->getErrorMessage();
    if (localMessage.size()) errorMessage << std::endl << localMessage;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + errorMessage.str(), ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>pVector_</I>.<BR>
 * @param partitionName - partition name
 * @exception FecExceptionHandler
 * @see XMLESFecMbReset::setEsDbMbResets (esMbResetVector, std::string partitionName)
 */
void XMLESFecMbReset::setEsDbMbResets(std::string partitionName) throw (FecExceptionHandler) {
  setEsDbMbResets(pVector_, partitionName);
}
#endif


// ---------------------------------------------------------------------------------------------
// Parser
// ---------------------------------------------------------------------------------------------

/** Parse the elements for a FEC buffer
 * \param DOMNode - DOM buffer (DOMDocument or DOMNode)
 */
unsigned int XMLESFecMbReset::parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {

      char *name = XMLString::transcode(n->getNodeName());
      //std::cout <<"----------------------------------------------------------"<<std::endl;
      //std::cout <<"Encountered Element NodeName in XMLESFecMbReset::parseAttributes : "<< name << std::endl;

      if(n->hasAttributes()) {

	int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
	char fecHardwareId[100] = "0" ;
	bool enabled = true ;
	unsigned long delayActiveReset = 0, intervalDelayReset = 0, mask = 0 ;        // ESMB


	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- ESMB reset
	if (!strcmp(name,"ESMBRESET")) {

	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes size is" << std::endl;
	  //std::cout <<"----------" << nSize << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	   // std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ;} 
	    else if (!strcmp(name,esMbResetDescription::ESMBRESETPARAMETERNAMES[esMbResetDescription::ESMBFECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,esMbResetDescription::ESMBRESETPARAMETERNAMES[esMbResetDescription::ESMBRINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,esMbResetDescription::ESMBRESETPARAMETERNAMES[esMbResetDescription::ESMBCCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,esMbResetDescription::ESMBRESETPARAMETERNAMES[esMbResetDescription::ESMBCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,esMbResetDescription::ESMBRESETPARAMETERNAMES[esMbResetDescription::DELAYACTIVERESET])) { val ++ ;  sscanf (value,"%ld",&delayActiveReset) ; }
	    else if (!strcmp(name,esMbResetDescription::ESMBRESETPARAMETERNAMES[esMbResetDescription::INTERVALDELAYRESET])) { val ++ ;  sscanf (value,"%ld",&intervalDelayReset) ; }
	    else if (!strcmp(name,esMbResetDescription::ESMBRESETPARAMETERNAMES[esMbResetDescription::MASK])) { val ++ ;  sscanf (value,"%ld",&mask) ; }

	    XMLString::release(&name);
	    XMLString::release(&value);
	  }
	    
	  if (val != 8 && val != 9) std::cerr << "\tESMB reset: invalid number of parameters: " << val << "/8,9 with the ENABLED tag\t" << std::endl ;

	  esMbResetDescription *piaDevice = new esMbResetDescription(buildCompleteKey(fec,ring,ccu,channel,address),
								   delayActiveReset, intervalDelayReset, mask) ;
	  piaDevice->setEnabled(enabled) ;
	  piaDevice->setFecHardwareId(fecHardwareId,crate) ;
	  pVector_.push_back(piaDevice) ;

	  countESMB ++ ;
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
