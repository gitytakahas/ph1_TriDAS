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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
//#define DEBUGMSGERROR

#ifndef TKIDVSHOSTNAMRDESCRIPTION_H
#define TKIDVSHOSTNAMRDESCRIPTION_H

#include <string>

#include "keyType.h"
#include "tscTypes.h"

#include "deviceDescription.h"

#define MAXCHAR 100

/**
 * \class TkIdVsHostnameDescription
 * Interface for the FED ID versus slot and hostname
 */
class TkIdVsHostnameDescription {

 private:

  /** Hostname
   */
  char hostname_[MAXCHAR] ;

  /** crate ID
   */
  char crateId_[MAXCHAR] ;
  
  /** slot
   */
  unsigned int slot_ ;

  /** subdetector
   */
  char subDetector_[MAXCHAR] ;

  /** ID: stored in string for FEC/FED
   */
  char id_[MAXCHAR] ;

  /** Crate number
   */
  unsigned int crateNumber_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum{IDVSHOSTNAME, IDVSCRATEID, IDVSSLOT, IDVSSUBDETECTOR, IDVSID, CRATENUMBER};

  /** Parameter's name
   */
  static const char *TKIDVSHOSTNAMEPARAMETERNAMES[CRATENUMBER+1] ; // = {"hostname", "crateId", "slot", "subDetector", "id", "crateNumber"} ;
  
  /** FED ID versus hostname constructor
   */
  TkIdVsHostnameDescription ( std::string hostname, std::string crateId, unsigned int slot, std::string subDetector, unsigned int fedSoftId, unsigned int crateNumber ) ;

  /** \brief FEC ID versus hostname constructor
   */
  TkIdVsHostnameDescription ( std::string hostname, std::string crateId, unsigned int slot, std::string subDetector, std::string fecHardId, unsigned int crateNumber ) ;
  
  /** \brief Constructor with the parameter definition extracted from the database or XML file
   */
  TkIdVsHostnameDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Nothing
   */
  ~TkIdVsHostnameDescription ( ) { }

  /** \brief Set hostname
   */
  void setHostname ( std::string hostname ) ;

  /** \brief Get hostname
   */
  std::string getHostname ( ) ;

  /** \brief Set crateId
   */
  void setCrateId ( std::string crateId ) ;

  /** \brief Get crateId
   */
  std::string getCrateId ( ) ;

  /** \brief Set subDetector
   */
  void setSubDetector ( std::string subDetector ) ;

  /** \brief Get subDetector
   */
  std::string getSubDetector ( ) ;

  /** \brief Set id
   */
  void setId ( std::string id ) ;

  /** \brief Get id
   */
  std::string getId ( ) ;

  /** \brief Set FEC id
   */
  void setFecId ( std::string id ) ;

  /** \brief Get FEC id
   */
  std::string getFecId ( ) ;

  /** \brief Set FED id
   */
  void setFedId ( unsigned int id ) ;

  /** \brief Get FED id
   */
  unsigned int getFedId ( ) ;

  /** \brief set slot
   */
  void setSlot ( unsigned int slot ) ;

  /** \brief get slot
   */
  unsigned int getSlot ( ) ;  

  /** \brief get the crate number
   */
  unsigned int getCrateNumber ( ) ;

  /** \brief set the crate number 
   */
  void setCrateNumber ( unsigned int crateNumber ) ;

  /** \brief Clone TkIdVsHostnameDescription
   */
  TkIdVsHostnameDescription *clone () ;

  /** \brief Display the TkIdVsHostnameDescription
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

  /** \brief Delete a list of parameter name but only its content
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) ;
} ;

#endif
