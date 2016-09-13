/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#ifndef XMLCONNECTION_H
#define XMLCONNECTION_H

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

#include <vector>

#include "XMLCommonFec.h"
#include "FecExceptionHandler.h"
#ifdef DATABASE
#include "DbConnectionsAccess.h"
#endif
#include "deviceType.h"

/** Schema for the DTD
 */
#define CONNECTION_XML_SCHEME "http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/schema.xsd"

/** Class for the connection parsing, build a vector of connection description
 */
class XMLConnection: public XMLCommonFec {

 protected:

  /** vector of connection
   */
  ConnectionVector connectionVector_ ;

  /** vector of connection
   */
  ConnectionVector apvNotConnectedVector_ ;

  /** number of connection parsed
   */
  unsigned int countConnections_ ;

  /** Parameter name's for the parsing
   */
  parameterDescriptionNameType *parameterNames_ ;

 public:

  //
  // public functions
  //
  /** \brief Default constructor
   */
  XMLConnection () throw (FecExceptionHandler);

  /** \brief Constructor with xml buffer
   */
  XMLConnection ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Constructor with xml buffer
   */
  XMLConnection ( DbConnectionsAccess *dbAccess ) throw (FecExceptionHandler);
#endif

  /** \brief Constructor with file access
   */
  XMLConnection ( std::string xmlFileName ) throw (FecExceptionHandler);
  
  /** \brief Deletes the XMLConnection
   */
  ~XMLConnection ();

  /** \brief return the number of CCU found in the buffer XML
   */
  inline unsigned int getCountDCUConversion ( ) { return countConnections_ ; }

  /** \brief return the vector of connection
   */
  inline ConnectionVector getConnectionVector ( ) { return connectionVector_ ; } 

  /** \brief clear the vector
   */
  void clearVector ( ) ;

  /** Parse the attributs for a DOM document
   */
  //unsigned int parseConnectionElements( XERCES_CPP_NAMESPACE::DOMNode *n ) ;
  unsigned int parseAttributes( XERCES_CPP_NAMESPACE::DOMNode *n ) ;

#ifdef DATABASE
  /** retreive buffer from database for a given partition
   */
  void parseDatabaseResponse(std::string partitionName) throw (FecExceptionHandler);

  /** retreive buffer from database for a given partition and version
   */
  void parseDatabaseResponse(std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor, bool retreiveApvNotConnected = false) throw (FecExceptionHandler);

  /** retreive from the database the connection list for a given partition
   */
  ConnectionVector getConnections (std::string partitionName) throw (FecExceptionHandler) ;

  /** retreive from the database the connection list for a given partition and a given version
   */
  ConnectionVector getConnections (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (FecExceptionHandler) ;

  /** retreive from the database the APV which have no connection
   */
  ConnectionVector getApvNotConnected (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (FecExceptionHandler) ;
#endif

  /** Generate the XML buffer for DB or for file
   */
  void generateConnectionBuffer ( ConnectionVector connectionVector, std::ostringstream &connectionBuffer, bool forDb = false ) ; 

  /** return the vector of connection
   */
  ConnectionVector getConnections () throw (FecExceptionHandler) ;

  /** \brief Writes the device vector parameter in a ASCII file
   */
  void setFileConnections ( ConnectionVector cVector, std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Writes the device vector attribute in a ASCII file
   */
  void setFileConnections ( std::string outputFileName ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Sends the descriptions from the description to the database
   */
  void setDbConnectionDescription ( std::string partitionName, ConnectionVector cVector, unsigned int versionUpdate ) throw (FecExceptionHandler, oracle::occi::SQLException);

  /** \brief Sends the descriptions from the description to the database
   */
  void setDbConnectionDescription ( std::string partitionName, unsigned int versionUpdate ) throw (FecExceptionHandler, oracle::occi::SQLException);
#endif

};
#endif
