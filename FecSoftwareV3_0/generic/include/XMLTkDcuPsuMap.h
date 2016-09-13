/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2007, Guillaume BAULIEU - IPNL/IN2P3
*/

#ifndef XMLTKDCUPSUMAP_H
#define XMLTKDCUPSUMAP_H

#ifdef DATABASE
#include "DbTkDcuPsuMapAccess.h"
#endif

#include "XMLCommonFec.h"
//#include "TkDcuPsuMapMemParseHandlers.h"
#include "MemBufOutputSource.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 */
class XMLTkDcuPsuMap : public XMLCommonFec {
 private:
  //
  // private attributes
  //
  /**
   * Vector of deviceDescriptions
   */
  tkDcuPsuMapVector dVector_;

  /** Count the number of dcu-psu found
   */
  unsigned int countDcuPsuMap_ ;

  /** Parameter name's for the parsing of DCU PSU map
   */
  parameterDescriptionNameType *dcuPsuMapParameterNames_ ;

  public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  XMLTkDcuPsuMap () throw (FecExceptionHandler);

  /** \brief Constructor with xml buffer
   */
  XMLTkDcuPsuMap ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Constructor with database access
   */
  XMLTkDcuPsuMap ( DbTkDcuPsuMapAccess *dbAccess )throw (FecExceptionHandler) ;
#endif

  /** \brief Constructor with file access
   */
  XMLTkDcuPsuMap ( std::string xmlFileName ) throw (FecExceptionHandler);
  
  /** \brief Deletes the XMLTkDcuPsuMap
   */
  ~XMLTkDcuPsuMap ();

  /** \brief return the number of DCU-PSU found in the XML buffer
   */
  inline unsigned int getCountDcuPsuMap() { return countDcuPsuMap_ ; }

  /** \brief clear the vector of elements
   */
  void clearVector();

  /** \brief Gets a pointer on the device vector private attribute
   */
  tkDcuPsuMapVector getDcuPsuMapVector ( );

  /** \brief Sets the device vector private attribute
   */
  void setDcuPsuMapVector (tkDcuPsuMapVector ) throw (FecExceptionHandler);

  /** \brief Parse the XML buffer
   */
  unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) ;

  /** \brief Parse the XML buffer and gets a pointer on the device vector private attribute
   */
  tkDcuPsuMapVector getDcuPsuMaps ( ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Retreive the XML clob from the database 
   */
  void parsePsuNameDatabaseResponse(std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0) throw (FecExceptionHandler) ;

  /** \brief Retreive the XML clob from the database 
   */
  void parseDcuPsuMapDatabaseResponse(std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0) throw (FecExceptionHandler) ;

  /** \brief Retreive the XML clob from the database 
   */
  void parsePsuNotConnectedDatabaseResponse(std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0) throw (FecExceptionHandler) ;

  /** \brief Retreive the PSU names from database
   */
  tkDcuPsuMapVector getPsuNames (std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0) throw (FecExceptionHandler) ;

  /** \brief Retreive the PSU names from database
   */
  tkDcuPsuMapVector getDcuPsuMap (std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0) throw (FecExceptionHandler) ;

  /** \brief Retreive the PSU names from database
   */
  tkDcuPsuMapVector getPsuNotConnected (std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0) throw (FecExceptionHandler) ;

#endif

  /** \brief Upload the DCU PSU map in a file
   */
  void setFileDcuPsuMap (tkDcuPsuMapVector dVector, std::string outputFileName) throw (FecExceptionHandler) ;

  /** \brief Upload the PSU name in a file
   */
  void setFilePsuName (tkDcuPsuMapVector dVector, std::string outputFileName) throw (FecExceptionHandler) ;

#ifdef DATABASE
  /** \brief Upload the DCU PSU map in database
   */
  void setDbDcuPsuMaps (tkDcuPsuMapVector dVector, std::string partitionName, unsigned int *versionMajorId = NULL, unsigned int *versionMinorId = NULL) throw (FecExceptionHandler) ;

  /** \brief Upload the PSU names in database
   */
  void setDbPsuNames (tkDcuPsuMapVector dVector, std::string partitionName, unsigned int *versionMajorId = NULL, unsigned int *versionMinorId = NULL) throw (FecExceptionHandler) ;
#endif
} ;

#endif
