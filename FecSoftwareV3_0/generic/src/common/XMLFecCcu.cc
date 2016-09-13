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
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>

#include "XMLFecCcu.h"

using namespace XERCES_CPP_NAMESPACE ;


//  TODO: make all the getCcus calls private


/**Create an XMLFec dedicated for ccus such as Pll, Laserdriver, ApvFec, ApvMux, Philips,...<BR>
 * Call the <I>XMLFec::XMLFec()</I> constructor<BR>
 * Call the method <I>XMLFecCcu::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec()</I>
 * @see <I>XMLFecCcu::init()</I>
 */
XMLFecCcu::XMLFecCcu () throw (FecExceptionHandler) :
  XMLCommonFec () 
{
  countCCU_ = 0 ;
  countRing_ = 0 ;
  parameterCCUNames_  = CCUDescription::getParameterNames() ;
  parameterRingNames_ = TkRingDescription::getParameterNames() ;
}

#ifdef DATABASE
/**Creates a XMLFecCcu with access to the database<BR>
 * Call the <I>XMLFec::XMLFec(DbAccess *dbAccess, std::string ipNumber, unsigned int xdaqId, std::string xdaqClassName)</I> constructor<BR>
 * Call the method <I>XMLFecCcu::init()</I><BR>
 * @param dbAccess - class for the database access
 * @exception FecExceptionHandler
 * @see <I>XMLFecCcu::init()</I>
 */
XMLFecCcu::XMLFecCcu ( DbFecAccess *dbAccess ) throw (FecExceptionHandler)  : XMLCommonFec((DbCommonAccess *)dbAccess) {
  countCCU_ = 0 ;
  countRing_ = 0 ;
  parameterCCUNames_  = CCUDescription::getParameterNames() ;
  parameterRingNames_ = TkRingDescription::getParameterNames() ;
}
#endif

  /**Creates a XMLFecCcu from a buffer<BR>
 * Call the <I>XMLFec::XMLFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>XMLFecCcu::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec(const XMLByte* xmlBuffer)</I>
 * @see <I>XMLFecCcu::init()</I>
 */
XMLFecCcu::XMLFecCcu (const XMLByte *xmlBuffer, bool toBeDeleted) throw (FecExceptionHandler) : XMLCommonFec( xmlBuffer, toBeDeleted ) {
  
  countCCU_ = 0 ;
  countRing_ = 0 ;
  parameterCCUNames_  = CCUDescription::getParameterNames() ;
  parameterRingNames_ = TkRingDescription::getParameterNames() ;
}

/**Creates a XMLFecCcu from a file<BR>
 * Call the <I>XMLFec::XMLFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>XMLFecCcu::init()</I><BR>
 * @param xmlFileName - file name that contains xml ccu descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLFec::XMLFec(std::string xmlFileName)</I>
 * @see <I>XMLFecCcu::init()</I>
 */
XMLFecCcu::XMLFecCcu ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLCommonFec( xmlFileName ) {
  countCCU_ = 0 ;
  countRing_ = 0 ;
  parameterCCUNames_  = CCUDescription::getParameterNames() ;
  parameterRingNames_ = TkRingDescription::getParameterNames() ;
}

/** delete the parameter names used for the parsing
 */
XMLFecCcu::~XMLFecCcu (){

  CCUDescription::deleteParameterNames(parameterCCUNames_); delete parameterCCUNames_ ;
  CCUDescription::deleteParameterNames(parameterRingNames_); delete parameterRingNames_ ;
}

/** Deletes all the temporary vectors
 */
void  XMLFecCcu::clearVector ()
{
  clearCcuVector();
  clearRingVector();
}

/** \brief clear the temporary CCU vector
 */
void XMLFecCcu::clearCcuVector () {

  for (ccuVector::iterator it = cVector_.begin(); it != cVector_.end(); it ++ ) {
    CCUDescription *ccu = (*it) ;
    if (ccu) delete ccu;
  }
    
  cVector_.clear();
}


/** \brief clear the temporary ring vector
 */
void XMLFecCcu::clearRingVector() {

  for (tkringVector::iterator ringIt = rVector_.begin(); ringIt != rVector_.end(); ringIt ++ ) {
    TkRingDescription *ringD = (*ringIt);
    if (ringD) delete ringD;
  }
    
  rVector_.clear();
}

/**Get the ccu Vector
 * @return the ccuVector <I>cVector_</I> attribute
 */
ccuVector XMLFecCcu::getCcuVector () {
  return cVector_;
}

/** Parses the dom documents
 * \param n - DOM node 
 */
unsigned int XMLFecCcu::parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {

    if (n->getNodeType() == XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE) {

      char *name = XERCES_CPP_NAMESPACE::XMLString::transcode(n->getNodeName());

#ifdef DATABASEDEBUG
      std::cout << "Found a " << name << std::endl ;
#endif

      if(n->hasAttributes()) {
	if (!strcmp(name,"CCU")) {
	  
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(parameterCCUNames_,pAttributes) ;
	  if (val != CCUDescription::CCUNUMBEROFPARAMETERS) std::cerr << "CCU description: invalid number of parameters: " << std::dec << val << "/" << CCUDescription::CCUNUMBEROFPARAMETERS << std::endl ;
	  CCUDescription *ccuDes = new CCUDescription (*parameterCCUNames_) ;
	  cVector_.push_back(ccuDes) ;

	  countCCU_ ++ ;
	}
	if (!strcmp(name,"RING")) {
	  
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(parameterRingNames_,pAttributes) ;
	  
	  if (val != TkRingDescription::RINGNUMBEROFPARAMETERS) std::cerr << "tkRing description: invalid number of parameters: " << std::dec << val << "/" << TkRingDescription::RINGNUMBEROFPARAMETERS << std::endl ;

	  TkRingDescription *tkringDes = new TkRingDescription (*parameterRingNames_) ;
	  rVector_.push_back(tkringDes) ;
	  
	  countRing_ ++ ;
	}
      }

      // Release name
      XERCES_CPP_NAMESPACE::XMLString::release(&name);

      ++count;
    }

    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      count += parseAttributes(child) ;
  }

#ifdef DATABASEDEBUG
  std::cout << "End of the parsing, found " << rVector_.size() << " rings and " << cVector_.size() << " CCUs" << std::endl ;
#endif

  return count ;
}

/**Parses the <I>xmlInputSource_</I> attribute using the SAX interface<BR>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLFecCcu::parseXMLBuffer()
 */
ccuVector XMLFecCcu::getCcusFromBuffer () throw (FecExceptionHandler) {

  parseXMLBuffer();
  return cVector_;
}

#ifdef DATABASE
/**Send a request to the database with Fec hardware identifier as parameter, for the current version.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parpartitionName, fecHardId, ringSlosed using the SAX interface.<BR> 
 * The result is then stored in <I>cVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @return <I>cVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
ccuVector XMLFecCcu::getCcusFromDb (std::string partitionName) throw (FecExceptionHandler) {

  parseDatabaseResponse(partitionName, "", 0);
  parseXMLBuffer();
  return cVector_;
}

/**Send a request to the database with Fec hardware identifier as parameter, for the current version.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parpartitionName, fecHardId, ringSlosed using the SAX interface.<BR> 
 * The result is then stored in <I>cVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @param ringSlot - ring slot
 * @return <I>cVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
ccuVector XMLFecCcu::getCcusFromDb (std::string partitionName, std::string fecHardId, unsigned int ringSlot) throw (FecExceptionHandler) {

  parseDatabaseResponse(partitionName, fecHardId, ringSlot);
  parseXMLBuffer();
  return cVector_;
}
#endif

/** Scans all the ccus in the <I>cVector_</I> and stores them properly into the available
 *  rings from <I>rVector_</I>. Returns the number of unsortable ccus.
 *  return number of unsorted ccus.
 */
int XMLFecCcu::storeCcusIntoRings() {

  keyType ringKey, ccuRingKey;
  tscType16 ringCrate, ccuCrate;
  std::string ringFecHardId, ccuFecHardId;

  // Check the CCUs and put them in the (hopefully one) ring. Return that one ring
  for (tkringVector::iterator ringit = rVector_.begin() ; ringit != rVector_.end(); ringit++ ) {

    ccuVector thisRingCcus;
    ringKey = (*ringit)->getKey();
    ringKey = buildFecRingKey(getFecKey(ringKey), getRingKey(ringKey));
    ringCrate = (*ringit)->getCrateId();
    ringFecHardId = (*ringit)->getFecHardwareId();

    // Is it necessary ?
    for (ccuVector::iterator ccuit = cVector_.begin() ; ccuit != cVector_.end() ; ) {

      ccuCrate = (*ccuit)->getCrateId();
      ccuRingKey = (*ccuit)->getKey();
      ccuRingKey = buildFecRingKey(getFecKey(ccuRingKey), getRingKey(ccuRingKey));
      ccuFecHardId = (*ccuit)->getFecHardwareId();

      if ((ccuCrate==ringCrate)&&(ccuRingKey==ringKey)&&(ccuFecHardId==ringFecHardId)) {
	thisRingCcus.push_back(*ccuit);
	ccuit = cVector_.erase(ccuit);
      } else {
	ccuit++;
      }
    }

    if (thisRingCcus.size()) (*ringit)->setCcuVector(thisRingCcus);
  }

  return (cVector_.size());
}


/** Returns a pointer on the <I>ringDescription_</I> private attribute
 */
TkRingDescription *XMLFecCcu::getRing () throw (FecExceptionHandler) {

  TkRingDescription *ring = NULL ;

  if (rVector_.size() == 1) ring = (*rVector_.begin()) ;

  return ring ;
}

#ifdef DATABASE
/**Send a request to the database with Fec hardware identifier as parameter, for the current version.<BR>
 * @param partitionName - partition name
 * @return <I>cVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
tkringVector XMLFecCcu::getRingFromDb (std::string partitionName) throw (FecExceptionHandler) {

  getVectorRingFromDb (partitionName,"",0) ;
  return rVector_ ;
}

/**Send a request to the database with Fec hardware identifier as parameter, for the current version.<BR>
 * @param partitionName - partition name
 * @return <I>cVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
TkRingDescription *XMLFecCcu::getRingFromDb (std::string partitionName, std::string fecHardId, unsigned int ringSlot) throw (FecExceptionHandler) {

  TkRingDescription* resultRing = NULL;

  getVectorRingFromDb (partitionName,fecHardId,ringSlot) ;

  // If instead we found more than one ring, it means that the database is messed!!! => only one ring for a request: partitionName, fecHardId, ringSlot
  if (rVector_.size() > 1) {
    clearCcuVector();
    clearRingVector();
    RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR,"Apparently more than one ring was obtained from the db when only one was asked! (Db messed?)",FATALERRORCODE) ;
  }
  // This is the default behaviour: one ring found, no ccus out of this ring. Everybody's happy.
  else if (rVector_.size()==1) {
    resultRing = ((*rVector_.begin()));
  } 

  return resultRing ;
}
/**Send a request to the database with Fec hardware identifier as parameter<BR>
 * @param fecHardId - FEC hardware ID
 * @param ringSlot - ring slot
 * @return <I>cVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
 TkRingDescription *XMLFecCcu::getRingFromDb (std::string fecHardId, unsigned int ringSlot) throw (FecExceptionHandler) {

  TkRingDescription* resultRing = NULL;  

  // We clear both cVector and rVector
  clearCcuVector();
  clearRingVector();

  // We fill these vectors again by reading the database
  parseDatabaseResponse(fecHardId, ringSlot);
  parseXMLBuffer();
  // Now we have both cVector_ and rVector_

  // Let's try to match ccus to the ring:
  if (storeCcusIntoRings() != 0) {
    clearCcuVector();
    clearRingVector();
    // TODO: handle exception: one or more ccus do not fit into available rings
    RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR,"Apparently more than one ring was obtained from the db when only one was asked! (Db messed?)",FATALERRORCODE) ;
  }

  // If we have no ring in rVector. Well, this means that the tkring you
  // were looking for is not in the database. The result is: NULL
  if (rVector_.size() == 0) {
    clearCcuVector();
    clearRingVector();
    RAISEFECEXCEPTIONHANDLER(DB_NODATAAVAILABLE, DB_NODATAAVAILABLE_MSG + " for the ring on FEC " + fecHardId + " on ring " + toString(ringSlot), ERRORCODE) ;
  }
  else if (rVector_.size() > 1) { // invalid number of rings for this specification
    clearCcuVector();
    clearRingVector();
    RAISEFECEXCEPTIONHANDLER(DB_NODATAAVAILABLE, "Found several rings for the ring on FEC " + fecHardId + " on ring " + toString(ringSlot), ERRORCODE) ;
  }
  // This is the default behaviour: one ring found, no ccus out of this ring. Everybody's happy.
  else if (rVector_.size()==1) {
    resultRing = ((*rVector_.begin()));
  } 

  return resultRing ;
}


/**Send a request to the database with Fec hardware identifier as parameter, for the current version.<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @param ringSlot - ring slot
 * @exception FecExceptionHandler
 * @see <I>XMLFec::parseDatabaseResponse(unsigned int fecHardId)</I>
 * @see <I>XMLFec::parseXMLBuffer()</I>
 */
void XMLFecCcu::getVectorRingFromDb (std::string partitionName, std::string fecHardId, unsigned int ringSlot) throw (FecExceptionHandler) {
  
  // We clear both cVector and rVector
  clearCcuVector();
  clearRingVector();

  // We fill these vectors again by reading the database
  parseDatabaseResponse(partitionName, fecHardId, ringSlot);
  parseXMLBuffer();
  // Now we have both cVector_ and rVector_

  // Let's try to match ccus to the ring:
  if (storeCcusIntoRings() != 0) {
    clearCcuVector();
    clearRingVector();
    // TODO: handle exception: one or more ccus do not fit into available rings
    RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR,"Apparently more than one ring was obtained from the db when only one was asked! (Db messed?)",FATALERRORCODE) ;
  }
  // If we have no ring in rVector. Well, this means that the tkring you
  // were looking for is not in the database. The result is: NULL
  else if (rVector_.size() == 0) {
    clearCcuVector();
    clearRingVector();
    if (fecHardId != "")
      RAISEFECEXCEPTIONHANDLER(DB_NODATAAVAILABLE, DB_NODATAAVAILABLE_MSG + " for the ring on partition " + partitionName + " FEC " + fecHardId + " on ring " + toString(ringSlot), ERRORCODE) ;
    else
      RAISEFECEXCEPTIONHANDLER(DB_NODATAAVAILABLE, DB_NODATAAVAILABLE_MSG + " for the rings on partition " + partitionName, ERRORCODE) ;
  }
}
#endif


/**Parses the <I>xmlInputSource_</I> attribute 
 * @return the private tkring vecotor attribute <I>rVector_</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLFecCcu::parseXMLBuffer()
 */
TkRingDescription* XMLFecCcu::getRingFromBuffer (std::string fecHardwareId, unsigned int ringSlot) throw (FecExceptionHandler) {

  TkRingDescription* resultRing = NULL;

  // We clear both cVector and rVector
  clearCcuVector();
  clearRingVector();

  // We fill these vectors again by reading the buffer 
  parseXMLBuffer();
  // Now we have both cVector_ and rVector_
  
  if (rVector_.size()) {
    unsigned int thisRingSlot;
    bool foundOneRing = false; // Isildur's bane not yet collected by a fisher...
    for (tkringVector::iterator ringit=rVector_.begin(); ringit!=rVector_.end(); ringit++) {
      thisRingSlot = getRingKey((*ringit)->getKey());
      if (((*ringit)->getFecHardwareId() == fecHardwareId)&&(thisRingSlot == ringSlot)) {
	if (foundOneRing) {
	  clearCcuVector();
	  clearRingVector();
	  RAISEFECEXCEPTIONHANDLER(XML_INVALIDFILE,"Found more than one ring when only one was asked in file for FEC " + fecHardwareId + ", ring " + toString(ringSlot) ,FATALERRORCODE) ;
	} else {
	  foundOneRing = true;
	  resultRing = (*ringit);
	}
      }
    }
  } else {
    RAISEFECEXCEPTIONHANDLER(DB_NODATAAVAILABLE, DB_NODATAAVAILABLE_MSG + " for the ring on FEC " + fecHardwareId + " on ring " + toString(ringSlot), ERRORCODE) ;
    //std::cerr << "Warning: no ring at all was found in the buffer" << std::endl;
  }

  // In theory the parsed CCUs should be put inside the proper ring now!
  if (storeCcusIntoRings() != 0) {
    clearCcuVector();
    clearRingVector();
    // TODO: handle exception: one or more ccus do not fit into available rings
    RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR,"Apparently not all the CCUs match the one ring selected for FEC " + fecHardwareId + " on ring " + toString(ringSlot), FATALERRORCODE) ;
  }

  return (resultRing);

}

/**Parses the <I>xmlInputSource_</I> attribute 
 * @return the private tkring vecotor attribute <I>rVector_</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the attribute <I>xmlInputSource_</I> is NULL
 * @see XMLFecCcu::parseXMLBuffer()
 */

tkringVector XMLFecCcu::getRingFromBuffer ( ) throw (FecExceptionHandler) {

  // We clear both cVector and rVector
  clearCcuVector();
  clearRingVector();

  // We fill these vectors again by reading the buffer 
  parseXMLBuffer();

  // Now we have both cVector_ and rVector_  
  if (!rVector_.size()) 
    RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE,"no ring found in file",ERRORCODE) ;
  else 
    storeCcusIntoRings() ;

  return rVector_ ;
}

/**Generates an XML buffer from the parameter <I>cVector</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param cVector - ccu list
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if
 *     - <I>cVector</I> is not initialized
 *     - the MemBufOutputSource created from the parameter <I>cVector</I> is not initialized 
 * @see XMLCommonFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 */
/*
void XMLFecCcu::setCcusToFile ( ccuVector *cVector, std::string outputFileName) throw (FecExceptionHandler) {

  if (cVector->size()!=0) {
    MemBufOutputSource memBufOS (cVector,false);
    XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
  } else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in file", ERRORCODE) ;
  }

}
*/

/**Generates an XML buffer from the parameter <I>rVector</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param rVector - tkring list
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if
 *     - <I>rVector</I> is not initialized
 *     - the MemBufOutputSource created from the parameter <I>rVector</I> is not initialized 
 * @see XMLCommonFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 */
void XMLFecCcu::setRingToFile ( TkRingDescription ringD, std::string outputFileName) throw (FecExceptionHandler) {

  MemBufOutputSource memBufOS (ringD, false, true);
  XMLCommonFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
}

/**Generates an XML buffer from the attribute <I>cVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLFecCcu::setCcusToFile ( ccuVector *cVector, std::string outputFileName)
 */
/*
void XMLFecCcu::setCcusToFile ( std::string outputFileName ) throw (FecExceptionHandler) {

  setCcusToFile( cVector_, outputFileName);
}
*/

/**Generates an XML buffer from the attribute <I>ringDescription_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLFecCcu::setRingToFile ( tkringVector *rVector, std::string outputFileName)
 */
void XMLFecCcu::setRingToFile ( std::string outputFileName ) throw (FecExceptionHandler) {

  TkRingDescription *ringD = getRing() ;

  if (ringD != NULL)
    setRingToFile( *ringD, outputFileName );
  else
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in file", ERRORCODE) ;
}

#ifdef DATABASE
// /**Generates an XML buffer from the parameter <I>cVector</I><BR>.
//  * Sends this buffer to the database as version <I>versionMajorId.versionMinorId</I>
//  * @param partitionName - partition name
//  * @param cVector - ccu description vector to be stored to the database
//  * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
//  *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
//  *     - a SQLException is thrown during the PL/SQL statement creation or execution
//  * @see DbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
//  */
/*
void XMLFecCcu::setCcusToDb(std::string partitionName, ccuVector *cVector)  throw (FecExceptionHandler) {

  this->setCcuVector (cVector);  
  this->dbConfigure (partitionName, false);
}
*/

// /**Generates an XML buffer from the parameter <I>rVector</I><BR>.
//  * Sends this buffer to the database
//  * @param partitionName - partition name
//  * @param rVector - tkring description vector to be stored to the database
//  * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
//  *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
//  *     - a SQLException is thrown during the PL/SQL statement creation or execution
//  * @see DbAccess::setXMLClob(std::string xmlBuffer);
//  */
void XMLFecCcu::setRingToDb (std::string partitionName, TkRingDescription ringD )  throw (FecExceptionHandler) {

  this->dbConfigure (partitionName, ringD, false);
}


// /**Generates an XML buffer from the attribute <I>cVector_</I>.<BR>
//  * Sends this buffer to the database as version <I>versionMajorId.versionMinorId</I>.<BR>
//  * @param partitionName - partition name
//  * @exception FecExceptionHandler
//  * @see XMLFecCcu::setCcus (ccuVector *cVector, unsigned int versionMajorId, unsigned int versionMinorId)
//  */
// void XMLFecCcu::setCcus(std::string partitionName) throw (FecExceptionHandler) {
//   this->dbConfigure (partitionName, false);
// }

/**Configure the database : create ccu parameters into the database.<BR>
 * Create a new state from the last current state with new partition, new structure... this new state is set as current state...<BR>
 * @param partitionName - name of the partition
 * @param newPartition - if <I>true</I> a new partition is created with <I>partitionName</I> as name, if <I>false</I> ccu parameters are stored in the already created partition <I>partitionName</I>
 * @return new Fec version major id
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 */
void XMLFecCcu::dbConfigure(std::string partitionName, boolean newPartition ) throw (FecExceptionHandler) {

  TkRingDescription *ringD = getRing() ;
  dbConfigure(partitionName,*ringD,newPartition) ;
}

/**Configure the database : create ccu parameters into the database.<BR>
 * Create a new state from the last current state with new partition, new structure... this new state is set as current state...<BR>
 * @param partitionName - name of the partition
 * @param ringD - ring description
 * @param newPartition - if <I>true</I> a new partition is created with <I>partitionName</I> as name, if <I>false</I> ccu parameters are stored in the already created partition <I>partitionName</I>
 * @return new Fec version major id
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 */
void XMLFecCcu::dbConfigure(std::string partitionName, TkRingDescription ringD, boolean newPartition ) throw (FecExceptionHandler) {

  MemBufOutputSource memBufOSRing (ringD, true, false);
  MemBufOutputSource memBufOSCcu  (*(ringD.getCcuVector()), true );

  try {
    if (dataBaseAccess_) {
      std::string xmlBufferRing = (memBufOSRing.getOutputBuffer())->str();
      std::string xmlBufferCcu = (memBufOSCcu.getOutputBuffer())->str();
      ((DbFecAccess *)dataBaseAccess_)->setRingCcuXMLClob(xmlBufferRing, xmlBufferCcu, partitionName);
    }else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  }
  catch (oracle::occi::SQLException &e) {
    std::stringstream msg ; msg << DB_PLSQLEXCEPTIONRAISED_MSG << " for partition name : " << partitionName ;
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, XMLCommonFec::what(msg.str(), e), ERRORCODE) ;
  }
}

/**Send a request to the database with Fec hardware identifier as parameter, for the current version<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param partitionName - partition name
 * @param fecHardId - Fec hardware identifier
 * @param ringSlot - ring slot
 * @see <I>DbFecAccess::getCcuXMLClob(unsigned int id, boolean hardId)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess has not been intialized
 */
void XMLFecCcu::parseDatabaseResponse ( std::string partitionName, std::string fecHardId, unsigned int ringSlot ) 
  throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromFecHardId";

  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      if (fecHardId == "") {
	xmlClob = ((DbFecAccess *)dataBaseAccess_)->getCcuXMLClob(partitionName) ;
      }
      else {
	xmlClob = ((DbFecAccess *)dataBaseAccess_)->getCcuXMLClob(partitionName, fecHardId, ringSlot) ;
      }

      if (xmlClob) {       

#ifdef DATABASEDEBUG
	display(xmlClob) ;
#endif

	try {
	  XERCES_CPP_NAMESPACE::InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	  XERCES_CPP_NAMESPACE::DOMInputSource *domInputSource = new XERCES_CPP_NAMESPACE::Wrapper4InputSource((XERCES_CPP_NAMESPACE::InputSource*)xmlInputSource) ;
	  domDocument_ = parser_->parse(*domInputSource) ;
	  delete domInputSource ;
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
      } // end if ((xmlClob =...
    } catch (oracle::occi::SQLException &e) {
      std::stringstream msg ; msg << DB_PLSQLEXCEPTIONRAISED_MSG << " for partition name : " << partitionName << ", FEC " << fecHardId << " and ring " << ringSlot ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, XMLCommonFec::what(msg.str(), e), ERRORCODE) ;
    }
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}

/**Send a request to the database with Fec hardware identifier as parameter<BR>
 * Get a result as a oracle::occi::Clob variable<BR>
 * Convert this Clob as a Xerces InputSource<BR>
 * @param fecHardId - Fec hardware identifier
 * @param ringSlot - ring slot
 * @see <I>DbFecAccess::getCcuXMLClob(std::string, unsigned int)</I>
 * @exception FecExceptionHandler : a FecExceptionHandler is thrown if :
 *    - the sql request throws a oracle::occi::SQLException
 *    - the dataBaseAccess has not been intialized
 */
void XMLFecCcu::parseDatabaseResponse ( std::string fecHardId, unsigned int ringSlot ) 
  throw (FecExceptionHandler) {
  std::string xmlBufferId = "theXMLBufferFromFecHardId";

  if (dataBaseAccess_ != NULL) {
    oracle::occi::Clob *xmlClob;
    try {
      xmlClob = ((DbFecAccess *)dataBaseAccess_)->getCcuXMLClob(fecHardId, ringSlot) ;

      if (xmlClob) {       

#ifdef DATABASEDEBUG
	display(xmlClob) ;
#endif

	try {
	  XERCES_CPP_NAMESPACE::InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	  XERCES_CPP_NAMESPACE::DOMInputSource *domInputSource = new XERCES_CPP_NAMESPACE::Wrapper4InputSource((XERCES_CPP_NAMESPACE::InputSource*)xmlInputSource) ;
	  domDocument_ = parser_->parse(*domInputSource) ;
	  delete domInputSource ;
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
      } // end if ((xmlClob =...
    } catch (oracle::occi::SQLException &e) {
      std::stringstream msg ; msg << DB_PLSQLEXCEPTIONRAISED_MSG << " when the CCU is retreived from construction database for FEC " << fecHardId << " and ring " << ringSlot ;
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, XMLCommonFec::what(msg.str(), e), ERRORCODE) ;
    }
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}


#endif
