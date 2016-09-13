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
  
  Copyright 2002 - 2007, Guillaume BAULIEU - IPNL/IN2P3
*/
  //#define DEBUGMSGERROR

#ifndef TKDCUPSUMAP_H
#define TKDCUPSUMAP_H

#include <string>

#include "ParameterDescription.h"
#include "FecExceptionHandler.h"

// power group
#define PSUDCUTYPE_PG "PG" 
// control group
#define PSUDCUTYPE_CG "CG" 
// ,
#define COMMA ","

/**
 * \class TkDcuPsuMap
 * Interface for the DCU/PSU mapping
 * 
 * Information
 *
 * dcuHardId: DCU hardware ID as read by ProgramTest.exe
 *
 */
class TkDcuPsuMap {

 private:

  /** DCU Hard ID
   */
  tscType32 dcuHardId_ ;

  /** Datapoint name
   */
  std::string dpName_ ;

  /** Datapoint name
   */
  std::string pvssName_ ;

  /** type of the PSU: control / power group: PSUDCUTYPE_PG or PSUDCUTYPE_CG
   */
  std::string psuType_ ;

  /** Key type from FEC point of view
   */
  keyType fecIndex_ ;

  /** DCU type
   */
  std::string dcuType_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum{DCUPSUMAPHARDID, DCUPSUMAPPSUNAME, DCUPSUTYPE};

  /** Parameter's names
   */
  static const char *DCUPSUMAPNAMES[DCUPSUTYPE+1] ; // = {"dcuHardId", "psuName", "psuType"} ;
  
  /** Constructor
   */
  TkDcuPsuMap ( tscType32 dcuHardId, std::string psuName, std::string psuType, keyType fecIndex = 0, std::string dcuType = "NONE" ) throw (FecExceptionHandler) ;

  /** Constructor with the parameter definition extracted from the database or XML file
   */
  TkDcuPsuMap ( parameterDescriptionNameType parameterNames ) throw (FecExceptionHandler) ;
  
  /** Set the DCU hard ID
   */
  void setDcuHardId ( tscType32 dcuHardId ) ;

  /** Get the DCU hard ID
   */
  tscType32 getDcuHardId ( ) ;

  /** Set the PSU Name
   */
  void setPsuName ( std::string psuName ) throw (FecExceptionHandler) ;

  /** Get the PSU Name
   */
  std::string getPsuName ( ) ;

  /** \brief return the dp name
   */
  std::string getDatapointName ( ) ;

  /** \brief return the pvss name
   */
  std::string getPVSSName ( ) ;

  /** Set the PSU type
   */
  void setPsuType ( std::string psuType ) ;

  /** Get the PSU type
   */
  std::string getPsuType ( ) ;

  /** Set the PSU type
   */
  void setFecIndex ( keyType fecIndex ) ;

  /** Get the PSU type
   */
  keyType getFecIndex ( ) ;

  /** \brief set the DCU type
   */
  void setDcuType ( std::string dcuType ) ;

  /** \brief get the DCU type
   * \return DCU type
   */
  std::string getDcuType ( ) ;

  /** \brief Clone TK DCU-PSU map description
   * \return the device description cloned
   */
  TkDcuPsuMap *clone () ;

  /** \brief Display the Dcu Info
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

  /** \brief Delete a list of parameter name and its content
   * \warning the parameter parameterNames is deleted by this method
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) ;

  /** \brief sort a vector of DCU / PSU map
   */
  static bool sortByPsuType ( TkDcuPsuMap *p1, TkDcuPsuMap *p2 ) ;
} ;

#endif
