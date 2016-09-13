/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#ifndef XMLFECCCU_H
#define XMLFECCCU_H

#ifdef DATABASE
#include "DbFecAccess.h"
#endif

#include "XMLCommonFec.h"
//#include "CcuMemParseHandlers.h"
#include "MemBufOutputSource.h"
#include "TkRingDescription.h"

#include <xercesc/dom/DOMNamedNodeMap.hpp>

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 */
class XMLFecCcu: public XMLCommonFec {
 private:
  //
  // private attributes
  //
  /**
   * Vector of ccuDescriptions
   */
  ccuVector cVector_;
  tkringVector rVector_;
  TkRingDescription* ringDescription_;

  /** Parameter name's for the CCU parsing
   */
  parameterDescriptionNameType *parameterCCUNames_ ;

  /** Parameter name's for the ring parsing
   */
  parameterDescriptionNameType *parameterRingNames_ ;

  /** \brief Parse the inputSource and gets a pointer on the ccu vector private attribute
   */
  ccuVector getCcusFromBuffer ( ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Gets a pointer on the ccu vector private attribute from database
   */
  ccuVector getCcusFromDb (std::string partitionName ) throw (FecExceptionHandler);

  /** \brief Gets a pointer on the ccu vector private attribute from database
   */
  ccuVector getCcusFromDb (std::string partitionName, std::string fecHardId, unsigned int ringSlot ) throw (FecExceptionHandler);
#endif

  /** \brief clear the temporary CCU vector
   */
  void clearCcuVector();
  /** \brief clear the temporary ring vector
   */
  void clearRingVector();
  /** \brief clear all the temporary vectors
   */
  void clearVector();

  /** Number of CCU parsed
   */
  unsigned int countCCU_ ; 

  /** Number of ring parsed
   */
  unsigned int countRing_ ; 

  public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  XMLFecCcu () throw (FecExceptionHandler);

  /** \brief Constructor with xml buffer
   */
  XMLFecCcu ( const XMLByte* xmlBuffer, bool toBeDeleted = true ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Constructor whith database access
   */
  XMLFecCcu ( DbFecAccess *dbAccess ) throw (FecExceptionHandler) ;

  /** \brief Constructor whith database access
   */
  XMLFecCcu ( DbFecAccess *dbAccess, std::string partitionName, std::string fecHardId, unsigned int ringSlot ) throw (FecExceptionHandler);
#endif

  /** \brief Constructor with file access
   */
  XMLFecCcu ( std::string xmlFileName ) throw (FecExceptionHandler);

  /** \brief Deletes the XMLFecCcu
   */
  ~XMLFecCcu ();

  /** \brief return the number of CCU found in the buffer XML
   */
  inline unsigned int getCountCCU ( ) { return countCCU_ ; }

  /** \brief Gets a pointer on the ccu vector private attribute
   */ 
  ccuVector getCcuVector ( ); // TODO: this should be made private, if not removed

  /** \brief Sets the ccu vector private attribute
   */
  //  void setCcuVector (ccuVector *cVector) throw (FecExceptionHandler);

  /** \brief parse the buffer
   */
  unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) ;

  /** \brief Scans all the ccus in the <I>cVector_</I> and stores them properly into the available rings
   */
  int storeCcusIntoRings();

#ifdef DATABASE
  /** \brief Gets a pointer on a new TkRingDescription object from database (with its CCUs inside)
   */
  tkringVector getRingFromDb (std::string partitionName ) throw (FecExceptionHandler);

  /** \brief Gets a pointer on a new TkRingDescription object from construction database (with its CCUs inside)
   */
  TkRingDescription *getRingFromDb (std::string fecHardId, unsigned int ringSlot ) throw (FecExceptionHandler);

  /** \brief Gets a pointer on a new TkRingDescription object from database (with its CCUs inside)
   */
  TkRingDescription *getRingFromDb (std::string partitionName, std::string fecHardId, unsigned int ringSlot ) throw (FecExceptionHandler);

  /** \brief called to get all the rings for partition or partition/fec/ring
   */
  void getVectorRingFromDb (std::string partitionName, std::string fecHardId, unsigned int ringSlot) throw (FecExceptionHandler) ;

#endif

  /** \brief Returns a pointer on the <I>ringDescription_</I> private attribute
   */
  TkRingDescription *getRing () throw (FecExceptionHandler);

  /** \brief Parse the inputSource and gets a pointer on the <I>ringDescription_</I> private attribute
   */
  TkRingDescription *getRingFromBuffer (std::string fecHardwareId, unsigned int ringSlot) throw (FecExceptionHandler);

  /** \brief Parse the inputSource and gets a pointer on the <I>ringDescription_</I> private attribute
   */
  tkringVector getRingFromBuffer () throw (FecExceptionHandler);

  /** \brief Writes the ccu vector parameter in an ASCII file
   */
  //  void setCcusToFile ( ccuVector *cVector, std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Writes the <I>ringDescription_</I> attribute in an ASCII file
   */
  void setRingToFile ( std::string outputFileName) throw (FecExceptionHandler);

  /** \brief Writes the tkring vector parameter in an ASCII file
   */
  void setRingToFile ( TkRingDescription ringD, std::string outputFileName) throw (FecExceptionHandler);

  /** \brief Writes the ccu vector attribute in an ASCII file
   *  It should be used only for DEBUG purposes
   */
  //  void setCcusToFile ( std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Writes the tkring vector attribute in an ASCII file
   *  It should be used only for DEBUG purposes
   */
  void setRingsToFile ( std::string outputFileName ) throw (FecExceptionHandler);

#ifdef DATABASE
   /* \brief Sends the parameter value from ccu vector to the database
    */ 
  //  void setCcusToDb ( std::string partitionName, ccuVector *dVector ) throw (FecExceptionHandler);

/*   /\** \brief Sends the parameter value from ccu vector to the database */
/*    *\/ */
//  void setCcus ( std::string partitionName ) throw (FecExceptionHandler);

  /** \brief Sends the parameter value from TkRingDescription to the database
   */
  void setRingToDb (std::string partitionName, TkRingDescription ringD )  throw (FecExceptionHandler) ;

  /** \brief Configures the database 
   */
  void dbConfigure(std::string partitionName, boolean newPartition = true) throw (FecExceptionHandler); 

  /** \brief Configures the database 
   */
  void dbConfigure(std::string partitionName, TkRingDescription ringD, boolean newPartition = true) throw (FecExceptionHandler); 

  /** \brief retreive XML Clob from database
   */
  void parseDatabaseResponse ( std::string partitionName, std::string fecHardId, unsigned int ringSlot ) throw (FecExceptionHandler) ;

  /** \brief retreive XML Clob from TKCC database
   */
  void parseDatabaseResponse ( std::string fecHardId, unsigned int ringSlot ) throw (FecExceptionHandler) ;

#endif


} ;

#endif
