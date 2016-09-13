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

#ifndef TKDCUINFO_H
#define TKDCUINFO_H

#include <string>

#include "keyType.h"
#include "tscTypes.h"
#include "ParameterDescription.h"

#define ISTOB(x)   ((x & (13 << 25))==(13<<25))
#define ISTIB(x)   ((x & (11 << 25))==(11<<25))
#define ISTID(x)   ((x & (12 << 25))==(12<<25))
#define ISTEC(x)   ((x & (14 << 25))==(14<<25))
#define ISPLUS(x)  (x &  (2 << 23))
#define ISMINUS(x) (x &  (1 << 23))

/**
 * \class TkDcuInfo
 * Interface for the DCU/DET ids
 * 
 * Information
 *
 * dcuHardId: DCU hardware ID as read by ProgramTest.exe
 *
 */
class TkDcuInfo {

 private:

  /** DCU Hard ID
   */
  tscType32 dcuHardId_ ;

  /** Det ID
   */
  tscType32 detId_ ;
  
  /** Fibre length
   */
  double fibreLength_ ;

  /** 
      Number of APVs
  */
  tscType32 apvNumber_;

  /**
     Time of flight from IP to detector
  */
  double timeOfFlight_;


 public:

  /** Enumeration List of name of parameter
   */
  enum{DCUINFOHARDID, DCUINFODETID, DCUINFOFIBRELENGTH, DCUINFOAPVNUMBER, DCUINFOTIMEOFFLIGHT};

  /** List of name of parameter
   */
  static const char *DCUINFOPARAMETERNAMES[DCUINFOTIMEOFFLIGHT+1] ; // = {"dcuHardId","detId","fibreLength","apvNumber","timeOfFlight"} ;
  
  /** Constructor
   */
  TkDcuInfo ( tscType32 dcuHardId, tscType32 detId, double fibreLength, tscType32 apvNumber, double timeOfFlight = 0) ;
  
  /** Constructor with the parameter definition extracted from the database or XML file
   */
  TkDcuInfo ( parameterDescriptionNameType parameterNames ) ;

  /** Nothing
   */
  ~TkDcuInfo ( ) ;
    
  /** Set the DCU hard ID
   */
  void setDcuHardId ( tscType32 dcuHardId ) ;

  /** Get the DCU hard ID
   */
  tscType32 getDcuHardId ( ) ;

  /** Set the Det ID
   */
  void setDetId ( tscType32 detId) ;

  /** Get the det ID
   */
  tscType32 getDetId ( ) ;

  /** Set the Fibre length
   */
  void setFibreLength ( double fibreLength ) ;

  /** Get the fibre length
   */
  double getFibreLength ( ) ;

  /** Set the APV number
   */
  void setApvNumber ( tscType32 apvNumber ) ;

  /** Get the APV number
   */
  tscType32 getApvNumber( ) ;

  /** Set the time of flight number
   */
  void setTimeOfFlight ( double timeOfFlight ) ;

  /** Get the time of flight number
   */
  double getTimeOfFlight( ) ;

  /** brief Clone TK DCU info description
   * \return the device description cloned
   */
  TkDcuInfo *clone () ;

  /** \brief Display the Dcu Info
   */
  void display ( ) ;

  /** \brief return the corresponding subdetector from the det id
   * \return the string with TIB, TIB, TOB, TEC+, TEC- and an exception if the det id is not recognized
   */
  std::string getSubDetectorFromDetId ( ) throw (std::string) ;

  // ------------------------------------------------------------------------------------------------------
  // 
  // Static methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief return the corresponding subdetector from the det id
   * \param detId - detector ID
   * \return the string with TIB, TIB, TOB, TEC+, TEC- and an exception if the det id is not recognized
   */
  static std::string getSubDetectorFromDetId ( unsigned int detId ) throw (std::string) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

  /** \brief Delete a list of parameter name and its content
   * \warning the parameter parameterNames is deleted by this method
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) ;

} ;

#endif
