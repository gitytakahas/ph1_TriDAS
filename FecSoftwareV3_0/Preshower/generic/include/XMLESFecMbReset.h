/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/


#ifndef XMLESFECMBRESET_H
#define XMLESFECMBRESET_H

#ifdef DATABASE
#  include "ESDbMbResetAccess.h"
#endif
#include "XMLESFec.h"
//#include "ESMbResetMemParseHandlers.h"
#include "esMemBufOutputSource.h"


#include "deviceType.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 * 
 * Private attributes :
 *  - pVector_ : pointer on a vector of piaResetDescriptions
 *
 */
class XMLESFecMbReset : public XMLESFec {
 private:
  //
  // private attributes
  //
  /**
   * Vector of piaResetDescriptions
   */
  esMbResetVector pVector_;
  
  public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  XMLESFecMbReset () throw (FecExceptionHandler);

  /** \brief Constructor whith xml buffer
   */
  XMLESFecMbReset ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Constructor whith database access
   */
  XMLESFecMbReset ( ESDbMbResetAccess *dbAccess ) throw (FecExceptionHandler);
#endif
  
  /** \brief Constructor with file access
   */
  XMLESFecMbReset ( std::string xmlFileName ) throw (FecExceptionHandler);
  
  /** \brief Deletes the device vector private attribute 
   */
  ~XMLESFecMbReset ();

  /** \brief DOM parser
   */
  unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) ;
  
  /** \brief Gets a pointer on the device vector private attribute
   */
  esMbResetVector getEsMbResetVector ( );
  
  /** \brief Sets a pointer on the device vector private attribute
   */
  void setEsMbResetVector (esMbResetVector  ) throw (FecExceptionHandler);
  
  /** \brief Parse the InputSource and Gets a pointer on the device vector private attribute
   */
  esMbResetVector getEsMbResets ( ) throw (FecExceptionHandler);

  void clearVector();

#ifdef DATABASE
  /** \brief Parse the InputSource and Gets a pointer on the device vector private attribute
   */
  esMbResetVector getEsMbResets (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  esMbResetVector getEsMbResets (std::string partitionName, std::string fecHardId ) throw (FecExceptionHandler);
#endif
 
  /** \brief Writes the pVector parameter in a ASCII file
   */
  void setEsFileMbResets ( esMbResetVector pVector, std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Writes the pVector_ attribute in a ASCII file
   */
  void setEsFileMbResets ( std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Displays the contents of the vector of devices piaResetParameter
   */
  void display();

#ifdef DATABASE
	/** \brief Sends the parameter values to the database
	 */
  void setEsDbMbResets ( esMbResetVector pVector, std::string partitionName) throw (FecExceptionHandler);

  /** \brief Sends the parameter values to the database
   */
  void setEsDbMbResets ( std::string partitionName) throw (FecExceptionHandler);
#endif

} ;

#endif
