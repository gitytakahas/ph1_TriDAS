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

#ifndef XMLESFECMBDCU_H
#define XMLESFECMBDCU_H

#include "XMLESFecDevice.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ) for the DCU
 */
class XMLESFecMbDcu : public XMLESFecDevice {

 public:
  /** \brief Default constructor
   */
  XMLESFecMbDcu ()  throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Retreive an XML input source from database
   */
  XMLESFecMbDcu ( ESDbFecAccess *dbAccess )  throw (FecExceptionHandler) ;
#endif

  /** \brief Constructor with xml buffer
   */
  XMLESFecMbDcu (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) ;

  /** \brief Constructor with file access
   */
  XMLESFecMbDcu ( std::string xmlFileName ) throw (FecExceptionHandler) ;

  /** \brief XMLESFecDevice destructor
   */
  ~XMLESFecMbDcu () ;

  /** \brief DOM parser
   */
  unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) ;
  
#ifdef DATABASE
  /** \brief retrieve the DCU from the database
   */
  deviceVector getDevices ( std::string partitionName, unsigned long timeStampStart, unsigned long timeStampStop ) throw (FecExceptionHandler) ;

  /** \brief retrieve the DCU from the database
   */
  deviceVector getDevices ( unsigned long dcuHardId, unsigned long timeStampStart, unsigned long timeStampStop ) throw (FecExceptionHandler) ;

  /** \brief retrieve the DCU from the database
   */
  deviceVector getDevices ( unsigned long timeStampStart, unsigned long timeStampStop ) throw (FecExceptionHandler) ;
	
	/** \brief Store DCU values in the database
	 */
	void setDcuValues (deviceVector dVector, std::string partitionName) throw (FecExceptionHandler);
#endif
} ;

#endif
