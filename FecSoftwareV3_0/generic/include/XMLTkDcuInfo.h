/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#ifndef XMLTKDCUINFO_H
#define XMLTKDCUINFO_H

#ifdef DATABASE
#include "DbTkDcuInfoAccess.h"
#endif

#include "XMLCommonFec.h"
//#include "TkDcuInfoMemParseHandlers.h"
#include "MemBufOutputSource.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 */
class XMLTkDcuInfo : public XMLCommonFec {
 private:
  //
  // private attributes
  //
  /**
   * Vector of deviceDescriptions
   */
  tkDcuInfoVector dVector_;

  /**
   * Vector of stateDescriptions
   */
  tkStateVector dStateVector_;

  /**
   * Vector of versionDescriptions
   */
  tkVersionVector dVersionVector_;

  /**
   * Vector of runDescriptions
   */
  tkRunVector dRunVector_;

  /** Count the number of det id found
   */
  unsigned int countDCUInfo_ ;

  /** Count the state number
   */
  unsigned int countState_ ;

  /** Count the number of version found
   */
  unsigned int countVersion_ ;

  /** Count the number of run found
   */
  unsigned int countRun_ ;

  /** Parameter name's for the parsing of DCU info
   */
  parameterDescriptionNameType *dcuInfoParameterNames_ ;

  /** Parameter name's for the parsing of DB state
   */
  parameterDescriptionNameType *stateParameterNames_ ;

  /** Parameter name's for the parsing of versions
   */
  parameterDescriptionNameType *versionParameterNames_ ;

  /** Parameter name's for the parsing of runs
   */
  parameterDescriptionNameType *runParameterNames_ ;

  public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  XMLTkDcuInfo () throw (FecExceptionHandler);

  /** \brief Constructor with xml buffer
   */
  XMLTkDcuInfo ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Constructor with database access
   */
  XMLTkDcuInfo ( DbTkDcuInfoAccess *dbAccess )throw (FecExceptionHandler) ;
#endif

  /** \brief Constructor with file access
   */
  XMLTkDcuInfo ( std::string xmlFileName ) throw (FecExceptionHandler);
  
  /** \brief Deletes the XMLTkDcuInfo
   */
  ~XMLTkDcuInfo ();

  /** \brief initialise the different parameter names for the parsing
   */
  void initParameterNames ( ) ;

  /** \brief return the number of DCU Info found in the XML buffer
   */
  inline unsigned int getCountDCUInfo() { return countDCUInfo_ ; }

  /** \brief return the number of state found in the XML buffer
   */
  inline unsigned int getCountState() { return countState_ ; }

  /** \brief return the number of version found in the XML buffer
   */
  inline unsigned int getCountVersion() { return countVersion_ ; }

  /** \brief return the number of run found in the XML buffer
   */
  inline unsigned int getCountRun() { return countRun_ ; }

  /** \brief Gets a pointer on the device vector private attribute
   */
  tkDcuInfoVector getDcuInfoVector ( );

  /** \brief Sets the device vector private attribute
   */
  void setDcuInfoVector (tkDcuInfoVector) throw (FecExceptionHandler);

  /** \brief Parse the XML buffer
   */
  unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) ;

  /** \brief Parse the XML buffer and gets a pointer on the device vector private attribute
   */
  tkDcuInfoVector getDcuInfos ( ) throw (FecExceptionHandler);

  /** \brief clear the vector of elements
   */
  void clearVector();

#ifdef DATABASE
  void parseDatabaseResponse(std::string partitionName, unsigned int majorVersionId = 0, unsigned int minorVersionId = 0) throw (FecExceptionHandler);
  void parseDatabaseResponse(unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler);
  void parseDatabaseResponse() throw (FecExceptionHandler);
  void parseStateDatabaseResponse() throw (FecExceptionHandler);
  void parseFecVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler);
  void parseFedVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler);
  void parseDcuInfoVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler);
  void parseConnectionVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler);
  void parseDcuPsuMapVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler);
  void parseMaskVersionsDatabaseResponse(std::string partitionName) throw (FecExceptionHandler);
  void parseRunDatabaseResponse(std::string partitionName) throw (FecExceptionHandler);
  void parseRunDatabaseResponse() throw (FecExceptionHandler);
  void parseRunDatabaseResponse(std::string partitionName, int runNumber) throw (FecExceptionHandler);
  void parseLastRunO2ODatabaseResponse(std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  tkDcuInfoVector getDcuInfos (std::string partitionName, unsigned int majorVersionId = 0, unsigned int minorVersionId = 0) throw (FecExceptionHandler);
  tkDcuInfoVector getAllDcuInfos (unsigned int majorVersionId, unsigned int minorVersionId) throw (FecExceptionHandler);
  tkDcuInfoVector getAllDcuInfos () throw (FecExceptionHandler);//last version only

  /** \brief Gets a vector of TkState containing all the current states.
   *  YOU have to delete all TkState objects + the vector!
   */
  tkStateVector getAllCurrentStates () throw (FecExceptionHandler);

  /** \brief Gets a vector of TkVersion containing all the Fec versions.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  tkVersionVector getAllFecVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a vector of TkVersion containing all the Fec versions.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  tkVersionVector getAllDcuInfoVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a vector of TkVersion containing all the Fec versions.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  tkVersionVector getAllConnectionVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a vector of TkVersion containing all the Fec versions.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  tkVersionVector getAllDcuPsuMapVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a vector of TkVersion containing all the Fec versions.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  tkVersionVector getAllMaskVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a vector of TkVersion containing all the Fed versions.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  tkVersionVector getAllFedVersions (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a TkRun containing the last run.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  TkRun *getLastRun(std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a TkRun containing the given run.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  TkRun *getRun(std::string partitionName, unsigned int runNumber) throw (FecExceptionHandler);

  /** \brief Gets a TkRunVector containing all the Runs of the account.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  tkRunVector getAllRuns() throw (FecExceptionHandler);

   /** \brief Gets a TkRun containing the last run of O2O.
   *  YOU have to delete all TkVersion objects + the vector!
   */
  TkRun *getLastO2ORun(std::string partitionName) throw (FecExceptionHandler);
 
#endif


/** \brief Writes the device vector parameter in a ASCII file
   */
  void setDcuInfos ( tkDcuInfoVector dVector, std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Writes the device vector attribute in a ASCII file
   */
  void setDcuInfos ( std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Displays the contents of the device vector attribute
   */
  //void display() throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Sends the parameter value from device vector to the database
   */
  void setDcuInfos ( tkDcuInfoVector dVector ) throw (FecExceptionHandler);

  /** \brief Sends the parameter value from device vector to the database
   */
  void setDcuInfos ( ) throw (FecExceptionHandler);

  /** \brief Configures the database
   */
  void dbConfigure() throw (FecExceptionHandler);
#endif
} ;

#endif
