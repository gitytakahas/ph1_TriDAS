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


#ifndef DBCOMMISSIONINGANALYSISACCESS_H
#define DBCOMMISSIONINGANALYSISACCESS_H

#include "DbAccess.h"


// ostream use for int to string conversion 
#include <sstream>

//declaration of the exception handler for the DCU
#include "FecExceptionHandler.h"
#include "CommissioningAnalysisDescription.h"

/**
   @class DbCommissioningAnalysisAccess
   @author Sebastien BEL
   @brief  Commissioning analysis database access management
*/

class DbCommissioningAnalysisAccess : public DbCommonAccess {


 public:


	////////////////////// CON(DE)STRUCTOR ////////////////////////////


	/** @brief Default constructor */
	DbCommissioningAnalysisAccess(bool threaded = false) throw ( oracle::occi::SQLException );

	/** @brief Constructor with connection parameters */
	DbCommissioningAnalysisAccess(std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw ( oracle::occi::SQLException );

	/** @brief Destructor */
	~DbCommissioningAnalysisAccess() throw ( oracle::occi::SQLException );



	///////////////////////// METHODS ///////////////////////////////


	/** 
	 * @brief Upload a clob containing a XML buffer to database
	 * @param buffer - XML content
	 * @param updateCurrentState - Indicates whether current partition state has to be updated or not
	 */
	void setXMLClob( std::string buffer, bool updateCurrentState, unsigned int &partitionStateVersionId ) throw ( oracle::occi::SQLException, FecExceptionHandler );

	/**
	 * @brief Update the current partition state in order to set the last analysis state get
	 * @param uploadedVersion - The last global analysis state get
	 */
	void setPartitionState( uint32_t uploadedVersion ) throw ( oracle::occi::SQLException );

	/**
	 * Retrieves analysis result descriptions (of the specified type) for a given runNumber and one of its associated partition
	 */
	oracle::occi::Clob* getCalibrationData( uint32_t runNumber,
				std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( oracle::occi::SQLException );

	/**
	 * @brief Retrieves all analysis result pair of run/version of the specified type and for a given partition
	 */
	oracle::occi::Clob* getAnalysisHistory( std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( oracle::occi::SQLException );

	/**
	 * @brief Retrieves an analysis result by its partition, type and version
	 */
	oracle::occi::Clob* getAnalysisHistory( std::string partitionName, uint32_t versionMajorID, uint32_t versionMinorID, CommissioningAnalysisDescription::commissioningType type ) throw ( oracle::occi::SQLException );

	/**
	 * @brief Retrieves local analysis versions associated to a global version number
	 * @param globalAnalsysVersion - global version from which retreiving versions
	 * @return clob corresponding to the map of analysis type / analysis versions
	 */
	oracle::occi::Clob* getLocalAnalysisVersions( uint32_t globalAnalysisVersion ) throw ( oracle::occi::SQLException );

};

#endif
