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
  
  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef COMMISSIONINGANALYSISFACTORY_H
#define COMMISSIONINGANALYSISFACTORY_H

#ifndef UNKNOWNFILE
#define UNKNOWNFILE "Unknown"
#endif

#include <string>
#include <vector>
#include <map>

//#include "hashMapDefinition.h"

#ifdef DATABASE
#include "DbCommissioningAnalysisAccess.h"
#endif

#include "DeviceFactoryInterface.h"
#include "CommissioningAnalysisDescription.h"

typedef std::map<uint32_t, std::vector< std::pair<uint32_t, uint32_t> > > HashMapRunVersion;
typedef std::map<uint32_t, std::pair<uint32_t, uint32_t> > HashMapAnalysisVersions; // < analysisType, localAnalysisVersions > associated to a given global version

#include "XMLCommissioningAnalysis.h"


/** 
    @class CommissioningAnalysisFactory
    @author Sebastien BEL
    @brief This class manages all the commissionning analysis and provides a support for database (must be compiled with DATABASE flag) or files.
*/
class CommissioningAnalysisFactory : public DeviceFactoryInterface {


private:

	/**
	 * Vector of analysis descriptions
	 */
	std::vector<CommissioningAnalysisDescription*> _calibrationDescriptions;

	/**
	 * Map of Run / Vector of versions
	 */
	HashMapRunVersion _pairOfRunVersion;

	/** Map of analysis type / versions associated to a given global version */
	HashMapAnalysisVersions _analysisLocalVersions;

	/**
	 * @brief clear the vector of descriptions
	 */
	void deleteRunVersion();

	/**
	 * @brief clear the map of local versions
	 */
	void deleteAnalysisLocalVersions();

	/** 
	 * @brief clear the vector of elements
	 */
	void deleteCalibrationVector();




public:

 /** 
  * @brief Build a Commissioning Analysis factory and if 
  * database is set create a database access
  */
  CommissioningAnalysisFactory();


#ifdef DATABASE
 /** 
  * @brief Buid a Commissioning Analysis factory for database
  */
  CommissioningAnalysisFactory( std::string login, std::string password, std::string path, bool threaded = false )
				throw ( oracle::occi::SQLException );

 /** 
  * @brief Buid a Commissioning Analysis factory
  */
  CommissioningAnalysisFactory( DbCommissioningAnalysisAccess *dbAccess );
#endif

 /** 
  * @brief Disconnect the database (if it is set)
  */  
  ~CommissioningAnalysisFactory();


 /** \brief Copy a vector of Analysis descriptions into another one with a clone of the descriptions
  * \warning the remote method must delete the vector created
  */
  static void vectorCopy( std::vector<CommissioningAnalysisDescription*> &input, std::vector<CommissioningAnalysisDescription*> &output ) {
	for (std::vector<CommissioningAnalysisDescription*>::iterator it = input.begin() ; it != input.end() ; it ++) {
		CommissioningAnalysisDescription *desc = *it ;
		output.push_back ( desc->clone() ) ;
	}
  }


  // ------------------------------------------------------------------------------------------------------
  // 
  // XML file methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** 
   * @brief Add a new file name in the descriptions
   */
  void addFileName ( std::string fileName );

  /** 
   * @brief set a new input file
   */
  void setInputFileName ( std::string inputFileName );

  std::vector<CommissioningAnalysisDescription*> getDescriptions(std::string fileName);

#ifdef DATABASE


  // ------------------------------------------------------------------------------------------------------
  // 
  // Database methods
  //
  // ------------------------------------------------------------------------------------------------------


  /** 
   * @brief Create the database access
   */
  void setDatabaseAccess( std::string login, std::string password, std::string path ) 
    throw ( oracle::occi::SQLException );

  /** 
   * @brief Create the database access with the configuration given by the env. variable CONFDB
   */
  void setDatabaseAccess() throw ( oracle::occi::SQLException );

  /** 
   * @brief Set the database access
   */
  void setDatabaseAccess( DbCommissioningAnalysisAccess *dbAccess );


#endif



  // ------------------------------------------------------------------------------------------------------
  // 
  //                        CALIBRATION DATA UPLOAD
  //
  // ------------------------------------------------------------------------------------------------------



  /**
   * @brief Uploads a new analysis result. 
   * If updateCurrentState is true, then the partition state has to be updated and the returned 
   * value is the state analysis version. If updateCurrentState is false, the returned value 
   * is NULL.
   * @param runNumber - analyzed commissioning run number
   * @param partitionName - analyzed run partition
   * @param type - commissioning type (no matching performed between type and the run modetype)
   * @param descriptions - vector of descriptions to be uploaded
   * @param updateCurrentState - indicates whether a major version has to created or not
   * @return state analysis version. If updateCurrentState is false, the returned value is NULL
   */
  uint32_t uploadAnalysis( uint32_t runNumber, std::string partitionName,
			  CommissioningAnalysisDescription::commissioningType type,
			  std::vector<CommissioningAnalysisDescription*> &descriptions,
			  bool updateCurrentState ) throw ( FecExceptionHandler );




  /**
   * @brief This method has to be called by the device factory user whenever uploadAnalysis has been called with updateCurrentState is true. 
   * The uploadedVersion integer is the value returned by @uploadAnalysis method. 
   * @param uploadedVersion - resulting uploaded version (major version related to partition state)
   */
  void uploadAnalysisState( uint32_t uploadedVersion ) throw ( FecExceptionHandler );





  // ------------------------------------------------------------------------------------------------------
  // 
  //                 DOWNLOAD VERSIONS AND RESULTS LINKED TO PARTITION STATE
  // 
  // ------------------------------------------------------------------------------------------------------



  /**
   * @brief Retrieves analysis result descriptions (of the specified type) for a given runNumber and associated partition
   * Example:<BR>
   * Give me the timing analysis result used to upload parameters related to the partition abcd 
   * which has been used for the run xyz.
   * @param runNumber - run for which we get analysis descriptions
   * @param partitionName - partition run for which we get analysis descriptions
   * @param type - analysis type for which we get descriptions
   * @param descriptions - vector of analysis descriptions
   */
  std::vector<CommissioningAnalysisDescription*> getCalibrationData( uint32_t runNumber,
			  std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler );




  // ------------------------------------------------------------------------------------------------------
  // 
  //                 DOWNLOAD ALL LOGGED ANALYSIS VERSIONS AND RESULTS
  // 
  // ------------------------------------------------------------------------------------------------------




  /**
   * @brief Retrieves all analysis result pair of run/version of the specified type and for a given partition
   * Example:<BR>
   * Give me all gain analysis versions and associated run, performed over partition abcd.
   * @param partitionName - partition for which we want to get all versions of analyzed 
   * run performed over partition
   * @param type - analysis type
   * @return Vector of pair <run/ <versionMajor/versionMinor> > associated to the partition
   */
  HashMapRunVersion getAnalysisHistory( std::string partitionName,
			  CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler );




  /**
   * @brief Retrieves analysis result descriptions (of the specified type) associated to a given run and partition
   * Example:<BR>
   * Give me all latency analysis descriptions of partition xyz and version efg.
   * @param partitionName - partition name for which we get descriptions
   * @param versionMajorID - analysis version for which we get descriptions
   * @param versionMajorID - analysis version for which we get descriptions
   * @param type - analysis type for which we get descriptions
   * @return Vector of analysis descriptions
   */
  std::vector<CommissioningAnalysisDescription*> getAnalysisHistory( std::string partitionName,
			  uint32_t versionMajorID, uint32_t versionMinorID, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler );




  /**
   * @brief Retrieves local analysis versions associated to a global version number
   * @param globalAnalsysVersion - global version from which retreiving versions
   * @return Map of analysis type / analysis versions
   */
  HashMapAnalysisVersions getLocalAnalysisVersions( uint32_t globalAnalysisVersion ) throw ( FecExceptionHandler );


};


#endif
