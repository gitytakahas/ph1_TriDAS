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
#include "XMLFecPiaReset.h"

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

//#define DISPLAYBUFFER

/**Create an XMLFec dedicated for PiaReset<BR>
 * Call the <I>XMLFec::XMLFec()</I> constructor<BR>
 * Call the method <I>XMLFecPiaReset::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec()</I>
 * @see <I>XMLFecPiaReset::init()</I>
 */
XMLFecPiaReset::XMLFecPiaReset () throw (FecExceptionHandler) : XMLFec() {
}

#ifdef DATABASE
/**Creates a XMLFecPiaReset with access to the database<BR>
 * Call the <I>XMLFec::XMLFec(DbAccess *dbAccess, std::string partitionName)</I> constructor<BR>
 * Call the method <I>XMLFecPiaReset::init()</I><BR>
 * @param dbAccess - class for the database access
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec(DbAccess *dbAccess, std::string partitionName)</I>
 * @see <I>XMLFecPiaReset::init()</I>
 */
XMLFecPiaReset::XMLFecPiaReset ( DbPiaResetAccess *dbAccess ) throw (FecExceptionHandler) : XMLFec( dbAccess ) {
}
#endif

/**Creates a XMLFecPiaReset from a buffer<BR>
 * Call the <I>XMLFec::XMLFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLFecPiaReset::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec(const XMLByte* xmlBuffer)</I>
 * @see <I>XMLFecPiaReset::init()</I>
 */
XMLFecPiaReset::XMLFecPiaReset (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLFec( xmlBuffer ){

}

/**Creates a XMLFecPiaReset from a file<BR>
 * Call the <I>XMLFec::XMLFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLFecPiaReset::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec(std::string xmlFileName)</I>
 * @see <I>XMLFecPiaReset::init()</I>
 */
XMLFecPiaReset::XMLFecPiaReset ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLFec( xmlFileName ) {
}

/**Delete the <I>handler_</I> attribute<BR>
 * Delete the device vector <I>pVector_</I> attribute<BR>
 */
XMLFecPiaReset::~XMLFecPiaReset (){
}

/**
 * Empty and clear the dVector
 */
void XMLFecPiaReset::clearVector(){

  while (pVector_.size() > 0) {
    delete pVector_.back();
    pVector_.pop_back();
  }
    
  pVector_.clear() ;
}

/**Get the device Vector
 * @return the piaResetVector <I>pVector_</I> attribute
 */
piaResetVector XMLFecPiaReset::getPiaResetVector () {
  return pVector_;
}

/**Set the device Vector
 * Empty the <I>pVector_</I> attribute
 * Delete all his elements
 * Copy all elements from <I>devices</I> parameter to <I>pVector_</I> attribute
 * @exception FecExceptionHandler : a FecExceptionHandler is raised, if <I>pVector_</I> is NULL
 */
void XMLFecPiaReset::setPiaResetVector (piaResetVector piaResets) throw (FecExceptionHandler) {
  // empty and delete previous piaResetVector
  // delete all elements
  clearVector() ;
    
  // set the piaResetVector parameter as the piaResetVector attribute
  for (piaResetVector::iterator device = piaResets.begin() ; device != piaResets.end() ; device++) {
    pVector_.push_back ((*device)) ;
  }
}

/**Parses the <I>xmlInputSource_</I> attribute using the SAX interface<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLFecPiaReset::parseXMLBuffer()
 */
piaResetVector XMLFecPiaReset::getPiaResets () throw (FecExceptionHandler) {

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
 * @see <I>XMLFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
piaResetVector XMLFecPiaReset::getPiaResets (std::string partitionName) throw (FecExceptionHandler) {

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

/**Send a request to the database with Fec hardware identifier as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>pVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @return <I>pVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
piaResetVector XMLFecPiaReset::getPiaResets (std::string partitionName, std::string fecHardId) throw (FecExceptionHandler) {

  //unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseDatabaseResponse(partitionName, fecHardId);
  //unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "ParseDatabaseResponse = " << (endMillis-startMillis) << " ms" << std::endl ;

  //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  parseXMLBuffer();
  //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  //std::cout << "parseXMLBuffer = " << (endMillis-startMillis) << " ms" << std::endl ;

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
void XMLFecPiaReset::setFilePiaResets ( piaResetVector pVector, std::string outputFileName ) throw (FecExceptionHandler) {

  if (pVector.size()) {
    MemBufOutputSource memBufOS(pVector);
    XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>pVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLFecPiaReset::setFilePiaResets ( piaResetVector pVector, std::string outputFileName)
 */
void XMLFecPiaReset::setFilePiaResets ( std::string outputFileName ) throw (FecExceptionHandler) {
  setFilePiaResets( pVector_, outputFileName );
}

/**Displays the contents of the attribute <I>pVector_</I>.
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if <I>pVector</I> is not initialized
 */
void XMLFecPiaReset::display() {

  for (piaResetVector::iterator it = pVector_.begin() ; it != pVector_.end() ; it ++) {
    piaResetDescription *piaResetd = *it;
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
 * \see DbAccess::setXMLClob(std::string xmlBuffer, std::string partitionName);
 */
void XMLFecPiaReset::setDbPiaResets (piaResetVector pVector, std::string partitionName)  throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS(pVector, true);

  try {
    if (dataBaseAccess_){
      std::string xmlBuffer = memBufOS.getPiaResetOutputBuffer()->str();
#ifdef DISPLAYBUFFER
      std::cout << xmlBuffer << std::endl;
#endif
      ((DbPiaResetAccess *)dataBaseAccess_)->setXMLClob(xmlBuffer, partitionName);
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch(oracle::occi::SQLException &e) {
    std::stringstream errorMessage ; errorMessage << e.what();
    std::string localMessage = dataBaseAccess_->getErrorMessage();
    if (localMessage.size()) errorMessage << std::endl << localMessage ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + errorMessage.str(), ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>pVector_</I>.<BR>
 * @Param 
 * @param partitionName - partition name
 * @exception FecExceptionHandler
 * @see XMLFecPiaReset::setDbPiaResets ((piaResetVector , std::string partitionName)
 */
void XMLFecPiaReset::setDbPiaResets(std::string partitionName) throw (FecExceptionHandler) {

  setDbPiaResets(pVector_, partitionName);
}
#endif

// ---------------------------------------------------------------------------------------------
// Parser
// ---------------------------------------------------------------------------------------------

/** Parse the elements for a FEC buffer
 * \param DOMNode - DOM buffer (DOMDocument or DOMNode)
 */
unsigned int XMLFecPiaReset::parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {

      char *name = XMLString::transcode(n->getNodeName());
      //std::cout <<"----------------------------------------------------------"<<std::endl;
      //std::cout <<"Encountered Element : "<< name << std::endl;

      if(n->hasAttributes()) {

	int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
	char fecHardwareId[100] = "0" ;
	bool enabled = true ;
	unsigned long delayActiveReset = 0, intervalDelayReset = 0, mask = 0 ;        // PIA


	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- PIA reset
	if (!strcmp(name,"PIARESET")) {

	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; } 
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIAFECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIARINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::DELAYACTIVERESET])) { val ++ ;  sscanf (value,"%ld",&delayActiveReset) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::INTERVALDELAYRESET])) { val ++ ;  sscanf (value,"%ld",&intervalDelayReset) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::MASK])) { val ++ ;  sscanf (value,"%ld",&mask) ; }

	    XMLString::release(&name);
	    XMLString::release(&value);
	  }
	    
	  if (val != 10) std::cerr << "PIA reset: invalid number of parameters: " << std::dec << val << " / 10 with the ENABLED tag" << std::endl ;

	  piaResetDescription *piaDevice = new piaResetDescription(buildCompleteKey(fec,ring,ccu,channel,address),
								   delayActiveReset, intervalDelayReset, mask) ;
	  piaDevice->setEnabled(enabled) ;
	  piaDevice->setFecHardwareId(fecHardwareId,crate) ;
	  //piaDevice->setCrateSlot(crate) ;
	  pVector_.push_back(piaDevice) ;

	  countPIA ++ ;
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
