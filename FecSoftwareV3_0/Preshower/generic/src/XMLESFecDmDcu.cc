/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE, Frederic DROUHIN - IReS/IN2P3, Universite de Haute-Alsace
*/

#include "stringConv.h"
#include "XMLESFecDmDcu.h"

#include <xercesc/framework/Wrapper4InputSource.hpp>

using namespace XERCES_CPP_NAMESPACE ;

/**Create an XMLESFecDmDcu dedicated for DCU
 * @exception FecExceptionHandler
 * @see <I>XMLESFecDevice:XMLESFecDevice()</I>
 */
XMLESFecDmDcu::XMLESFecDmDcu () throw (FecExceptionHandler) : XMLESFecDevice () { }

#ifdef DATABASE

/**Creates a XMLESFecDmDcu with access to the database<BR>
 * Call the <I>XMLESFecDevice::XMLESFecDevice( ESDbFecAccess * )</I><BR>
 */
XMLESFecDmDcu::XMLESFecDmDcu ( ESDbFecAccess *dbAccess )  throw (FecExceptionHandler) : XMLESFecDevice(dbAccess) { }

#endif

/**Creates a XMLESFecDmDcu from a buffer<BR>
 * Call the <I>XMLESFecDevice::XMLESFecDevice(const XMLByte* xmlBuffer)</I> constructor<BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>XMLESFecDevice::XMLESFecDevice(const XMLByte* xmlBuffer)</I>
 */
XMLESFecDmDcu::XMLESFecDmDcu ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLESFecDevice( xmlBuffer ){ }

/**Creates a XMLESFecDmDcu from a file<BR>
 * Call the <I>XMLESFecDevice::XMLESFecDevice(std::string xmlFileName)</I> constructor<BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>XMLESFecDevice::XMLESFecDevice(std::string xmlFileName)</I>
 */
XMLESFecDmDcu::XMLESFecDmDcu ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLESFecDevice( xmlFileName ) { }


/**Delete the <I>handler_</I> attribute<BR>
 * Delete the device vector <I>dVector_</I> attribute<BR>
 */
XMLESFecDmDcu::~XMLESFecDmDcu (){ }

#ifdef DATABASE
/**Send a request to the database with partition name as parameter, for the current version.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param partitionName - partition name
 * @param timeStampStart - start timestamp
 * @param timeStampStop - stop timestamp
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLESFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
deviceVector XMLESFecDmDcu::getDevices (std::string partitionName, unsigned long timeStampStart, unsigned long timeStampStop) throw (FecExceptionHandler) {

  unsigned long min = timeStampStart > timeStampStop ? timeStampStop : timeStampStart ;
  unsigned long max = timeStampStart > timeStampStop ? timeStampStart : timeStampStop ;
  std::string request = "BEGIN :xmlClob := PkgDcuXML.getAllDcuFromPartition ('" + partitionName + "'," + toString(min) + ", " + toString(max) + "); END;" ;
  setDatabaseRequest(request) ;
  XMLCommonFec::parseDatabaseResponse();

  parseXMLBuffer();

  return dVector_;
}


/**Send a request to the database with partition name as parameter.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param dcuHardId - dcu hardware identifier
 * @param timeStampStart - start timestamp
 * @param timesStampStop - stop timestamp
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
deviceVector XMLESFecDmDcu::getDevices (unsigned long dcuHardId, unsigned long timeStampStart, unsigned long timeStampStop) throw (FecExceptionHandler) {
	
	unsigned long min = timeStampStart > timeStampStop ? timeStampStop : timeStampStart;
	unsigned long max = timeStampStart > timeStampStop ? timeStampStart : timeStampStop;
  std::string request = "BEGIN :xmlClob := PkgDcuXML.getAllDcuFromDcuId ('" + toString(dcuHardId) + "', " + toString(min) + ", " + toString(max) + "); END;" ;
  setDatabaseRequest(request) ;
  XMLCommonFec::parseDatabaseResponse();

  parseXMLBuffer();

  return dVector_;
}



/**Send a request to the database with partition name as parameter, for the current version.<BR>
 * The result is converted as <I>xmlInputSource_</I> attribute.<BR>
 * <I>xmlInputSource_</I> attribute is then parsed using the SAX interface.<BR> 
 * The result is then stored in <I>dVector_</I> attribute.<BR>
 * @param timeStampStart - start timestamp
 * @param timeStampStop - stop timestamp
 * @return <I>dVector_</I> attribute
 * @exception FecExceptionHandler
 * @see <I>XMLCommonFec::parseDatabaseResponse(std::string partitionName)</I>
 * @see <I>XMLESFec::parseXMLBuffer()</I>
 */
deviceVector XMLESFecDmDcu::getDevices (unsigned long timeStampStart, unsigned long timeStampStop) throw (FecExceptionHandler) {

  unsigned long min = timeStampStart > timeStampStop ? timeStampStop : timeStampStart ;
  unsigned long max = timeStampStart > timeStampStop ? timeStampStart : timeStampStop ;
  std::string request = "BEGIN :xmlClob := PkgDcuXML.getAllDcu (" + toString(min) + ", " + toString(max) +"); END;" ;
  setDatabaseRequest(request) ;
  XMLCommonFec::parseDatabaseResponse();

  parseXMLBuffer();

  return dVector_;
}

#endif


/** Parse the elements for a FEC buffer
 * \param DOMNode - DOM buffer (DOMDocument or DOMNode)
 */
unsigned int XMLESFecDmDcu::parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n) {

  return XMLESFecDevice::parseAttributes ( n ) ;
}


#ifdef DATABASE
/* Generates an XML buffer from the parameter <I>dVector</I><BR>.
 * Sends this buffer to the database as version <I>versionMajorId.versionMinorId</I>
 * @param dVector - device description vector to be stored to the database
 * @param partitionName - partition name
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if 
 *     - the buffer to be sent <I>memBufOS</I> or <I>dataBaseAccess_</I> is not initialized
 *     - a SQLException is thrown during the PL/SQL statement creation or execution
 * @see ESDbAccess::setXMLClob(std::string xmlBuffer, unsigned int versionMajor, unsigned int versionMinor);
 * The upload is done for a timestamp > 0 in the version 0.0
 */
void XMLESFecDmDcu::setDcuValues (deviceVector dVector, std::string partitionName) throw (FecExceptionHandler){
	
	esMemBufOutputSource memBufOS (dVector, true);
	try {
		if (dataBaseAccess_){
			std::string xmlBuffer = (memBufOS.getDcuOutputBuffer())->str();
			
		#ifdef DATABASEDEBUG
			std::cout << xmlBuffer << std::endl;
		#endif
			std::string writeString("BEGIN PkgDmDcuXML.uploadXMLClob(:bufferDcu, :partitionName); END;") ;
			((ESDbFecAccess *)dataBaseAccess_)->setXMLClob(writeString, xmlBuffer, partitionName);
		} else {
		  RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
		}
	}
	catch (oracle::occi::SQLException &e){
		std::stringstream errorMessage; errorMessage << e.what();
		std::string localMessage = dataBaseAccess_->getErrorMessage();
		if (localMessage.size()) errorMessage << std::endl << localMessage;
		RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, DB_PLSQLEXCEPTIONRAISED_MSG + errorMessage.str(), ERRORCODE) ;
	}
}
#endif
