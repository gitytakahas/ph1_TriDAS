/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/


#ifndef XMLFEC_H
#define XMLFEC_H

#ifdef DATABASE
#include "DbAccess.h"
#endif

// use in XMLFec to declare ofstream
#include <fstream>
#include <sstream>

#include "deviceType.h"
#include "XMLCommonFec.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).<BR>
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 *
 * SAX parser(<I>parser_</I>) and Sax handler(<I>handler_</I>) are XMLFec attributes.<BR>
 * Otherwise, <I>handler_</I> will be instanciated in subclasses, depending what you need (Device, PiaReset,...).<BR> 
 * Features for <I>parser_</I> (error handler, document handler) will also be set in subclasses, once the <I>handler_</I> is created.<BR>
 */
class XMLFec: public XMLCommonFec {
 protected:

  /** Counter of elements
   */
  unsigned int countPLL, countAPV25, countDCUCCU, countDCUFEH, countAPVMUX, countPIA, countAOH, countDOH ;

 public:

  /** \brief Default constructor
   */
  XMLFec ();

  /** \brief Constructor with xml buffer
   */
  XMLFec ( const XMLByte* buffer ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Constructor whith database access
   */
  XMLFec ( DbAccess *dbAccess ) throw (FecExceptionHandler);
#endif

  /** \brief Constructor with file access
   */
  XMLFec ( std::string xmlFileName ) throw (FecExceptionHandler);

  /** \brief Deletes the device vector private attribute
   */
  virtual ~XMLFec ();

  /** \brief return the number of devices
   * \param number of devices
   */
  inline unsigned int getCountPLL() { return countPLL ; }

  /** \brief return the number of devices
   * \param number of devices
   */
  inline unsigned int getCountAPV25() { return countAPV25 ; }

  /** \brief return the number of devices
   * \param number of devices
   */
  inline unsigned int getCountDCUCCU() { return countDCUCCU ; }

  /** \brief return the number of devices
   * \param number of devices
   */
  inline unsigned int getCountDCUFEH() { return countDCUFEH ; }

  /** \brief return the number of devices
   * \param number of devices
   */
  inline unsigned int getCountAPVMUX() { return countAPVMUX ; }

  /** \brief return the number of devices
   * \param number of devices
   */
  inline unsigned int getCountPIA() { return countPIA ; }

  /** \brief return the number of devices
   * \param number of devices
   */
  inline unsigned int getCountAOH() { return countAOH ; }

  /** \brief return the number of devices
   * \param number of devices
   */
  inline unsigned int getCountDOH() { return countDOH ; }

  /** \brief clear the vector
   */
  virtual void clearVector() = 0 ;

#ifdef DATABASE
  /** \brief This method sends a request to the database and parse the input source retrieved from database
   */
  void parseDatabaseResponse ( std::string partitionName ) throw (FecExceptionHandler);

  /** \brief This method sends a request to the database and parse the input source retrieved from database
   */
  void parseDatabaseResponse ( std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor ) throw (FecExceptionHandler) ;

  /** \brief This method sends a request to the database and parse the input source retrieved from database
   */
  void parseDatabaseResponse ( std::string partitionName,  std::string fecHardId ) throw (FecExceptionHandler);

  /** \brief This method sends a request to the database and parse the input source retrieved from database
   */
  void parseDatabaseResponse ( std::string partitionName,  std::string fecHardId, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) ;
#endif

  /** \brief prototype for the parser
   */
  virtual unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) = 0 ;

/*   /\** \brief Parse the elements for a FEC buffer */
/*    *\/ */
/*   unsigned int parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n, deviceVector &vDevices ) ; */

/*   /\** \brief Parse the elements for a FEC buffer */
/*    *\/ */
/*   unsigned int parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n, piaResetVector &vPiaReset ) ; */

/*   /\** \brief Parse the elements for a FEC buffer */
/*    *\/ */
/*   unsigned int parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n, deviceVector &vDevices, piaResetVector &vPiaReset) ; */
} ;

#endif
