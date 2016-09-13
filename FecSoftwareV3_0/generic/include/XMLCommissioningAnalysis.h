/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef XMLCOMMISSIONINGANALYSIS_H
#define XMLCOMMISSIONINGANALYSIS_H

#ifdef DATABASE
#include "DbCommissioningAnalysisAccess.h"
#endif

#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>


#include "XMLCommonFec.h"
#include "MemBufOutputSource.h"
#include "ParameterDescription.h"

#include "CommissioningAnalysisDescription.h"
#include "ApvLatencyAnalysisDescription.h"
#include "CalibrationAnalysisDescription.h"
#include "FastFedCablingAnalysisDescription.h"
#include "FineDelayAnalysisDescription.h"
#include "OptoScanAnalysisDescription.h"
#include "PedestalsAnalysisDescription.h"
#include "TimingAnalysisDescription.h"
#include "VpspScanAnalysisDescription.h"
#include "CommissioningAnalysisFactory.h"

/** 
 * @brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 * @warning the pointers allocated by the class should be deleted
 */
class XMLCommissioningAnalysis : public XMLCommonFec {


private:

  //
  // private attributes
  //

	/**
	 * Vector of analysis descriptions
	 */
	std::vector<CommissioningAnalysisDescription*> _dVector;

	/**
	 * Map of Run / Vector of versions
	 */
	HashMapRunVersion _pairOfRunVersion;

	/** Map of analysis type / versions associated to a given global version */
	HashMapAnalysisVersions _analysisLocalVersions;


	/** 
	 * Count the number of analysis description found
	 */
	unsigned int _counter;

	/** 
	 * Parameter names for the parsing
	 */
	parameterDescriptionNameType *_parameterNames;

	/**
	 * Current analysis type
	 */
	CommissioningAnalysisDescription::commissioningType _currentCommissioningType;
	
	/**
	 * XML node attributes
	 */
	XERCES_CPP_NAMESPACE::DOMNamedNodeMap *_pAttributes;

	/**
	 * Initialization method
	 */
	void _init();


public:

  // parameters list:

	/** @brief Enum of parameter names related to the pair of run / version of all anlaysis associated to a partition */
	enum {RUNVERSION_RUN, RUNVERSION_VERSIONMAJOR, RUNVERSION_VERSIONMINOR};
	/** @brief List of parameter names related to the pair of run / version of all anlaysis associated to a partition */
	static const char *RUNVERSION[ RUNVERSION_VERSIONMINOR+1 ];


  //
  // public functions
  //

	/** 
	 * @brief Default constructor
	 */
	XMLCommissioningAnalysis() throw ( FecExceptionHandler );

	/** 
	 * @brief Constructor with xml buffer
	 */
	XMLCommissioningAnalysis( const XMLByte* xmlBuffer ) throw ( FecExceptionHandler );

#ifdef DATABASE
	/** 
	 * @brief Constructor with database access
	 */
	XMLCommissioningAnalysis( DbCommissioningAnalysisAccess *dbAccess ) throw ( FecExceptionHandler ) ;
#endif

	/** 
	 * @brief Constructor with file access
	 */
	XMLCommissioningAnalysis( std::string xmlFileName ) throw ( FecExceptionHandler );

	/** 
	 * @brief Deletes the XMLCommissioningAnalysis
	 */
	~XMLCommissioningAnalysis();

	/**
	 * @brief clear the vector of descriptions
	 */
	void deleteRunVersion();

	/**
	 * @brief clear the map of local versions
	 */
	void deleteAnalysisLocalVersions();

	/** 
	 * @brief get the list of parameter name related to the pair of run / version of all anlaysis associated to a partition (XML clob interface) 
	 */
	static parameterDescriptionNameType *getRunVersionParameterNames();

	/** 
	 * @brief Parse the XML buffer
	 */
	unsigned int parseAttributes( XERCES_CPP_NAMESPACE::DOMNode *n ) ;

	/** 
	 * @brief clear the vector of elements
	 */
	void clearVector();

	/** 
	 * @brief return the number of analysis record found in the XML buffer
	 */
	inline unsigned int getLength() { return _counter; }

	/** 
	 * @brief Gets the vector of descriptions
	 */
	std::vector<CommissioningAnalysisDescription*> getDescriptions();

	/** 
	 * @brief Sets the vector of descriptions
	 */
	void setDescriptions( std::vector<CommissioningAnalysisDescription*> &inVector, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler );


	/**
	 * @brief Uploads analysis into a ASCII XML file
	 * @param outputFileName - Ouput filename to be filled
	 */
	void uploadAnalysis( std::string outputFileName ) throw ( FecExceptionHandler );


	/**
	 * @brief Uploads a new analysis result. If updateCurrentState=true, then the partition state has 
	 *        to be updated and the returned value is the state analysis version. It may be done by 
	 *        calling @uploadAnalysisState.<BR>
	 *        If updateCurrentState=false, the returned value is 0.
	 * @param updateCurrentState - Indicates whether partition state has to be updated or not
	 * @return The global state analysis version to be associated to the current state
	 */
	uint32_t uploadAnalysis( bool updateCurrentState ) throw ( FecExceptionHandler );

	/**
	 * @brief This method has to be called by the device factory user whenever @uploadAnalysis has 
	 *        been called with updateCurrentState=true. <BR>
	 * @param uploadedVersion - Integer returned by @uploadAnalysis method.
	 */
	void uploadAnalysisState( uint32_t uploadedVersion );

	std::vector<CommissioningAnalysisDescription*> getCalibrationData( uint32_t runNumber,
			std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler );


  /**
   * @brief Retrieves all analysis result pair of run/version of the specified type and for a given partition
   * Example:<BR>
   * Give me all gain analysis versions and associated run, performed over partition abcd.
   * @param partitionName - partition for which we want to get all versions of analyzed 
   * run performed over partition
   * @param type - analysis type
   * @return Vector of pair <run/ <versionMajor/versionMinor> > associated to the partition
   */
  HashMapRunVersion getAnalysisHistory( std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler );

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



} ;

#endif
