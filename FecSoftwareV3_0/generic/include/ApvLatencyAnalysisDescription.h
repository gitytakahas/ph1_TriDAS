/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef APVLATENCYANALYSISDESCRIPTION_H
#define APVLATENCYANALYSISDESCRIPTION_H

#include "ApvLatencyAnalysisDescription.h"
#include "ParameterDescription.h"
#include "CommissioningAnalysisDescription.h"



/**
   @class ApvLatencyAnalysisDescription
   @author Sebastien BEL
   @brief  APV latency scan
*/
class ApvLatencyAnalysisDescription : public CommissioningAnalysisDescription {


private:

  // properties:

	/** APV latency setting. */
	uint16_t _latency;



public:

	/** @brief Enum of parameter names */
	enum{LATENCY};
	/** @brief List of parameter names */
	static const char *APVLATENCYANALYSISDESCRIPTION[ LATENCY+1 ];



  // con(de)structors:

	/** @brief default constructor */
	ApvLatencyAnalysisDescription();
	/**
	 * @brief upload contructor 
	 * @param latency - APV latency
	 * @param crate - device FEC crate
	 * @param slot - device FEC slot
	 * @param ring - device FEC ring
	 * @param ccuAdr - device CCU Module
	 * @param ccuChan - device FE Module
	 * @param i2cAddr - device APV I2C Address
	 * @param partition - analysis partition
	 * @param runNumber - analysis runNumber
	 * @param valid - analysis description is valid (true) or not (false)
	 * @param comments - concatened comments about encountered errors or warnings
	 * @param fedId - FED id [0,50-489,invalid]
	 * @param feUnit - FE unit [0,1-8,invalid]
	 * @param feChan - FE channel [0,1-12,invalid]
	 * @param fedApv - APV [0,1-2,invalid]
	 */
	ApvLatencyAnalysisDescription(uint16_t latency,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments, uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv );
	/**
	 * Build a description based on a XML parsing (from file or database)
 	 * @param parameterNames - parameter names extracted from XML
	 */
	ApvLatencyAnalysisDescription(parameterDescriptionNameType parameterNames);


  // methods:

	/** @brief human readable object content */
	virtual std::string toString();
	/** @brief clone method */
	virtual CommissioningAnalysisDescription *clone();
	/** @brief display object content in standard output */
	virtual void display();
	/** @brief get the commissioning analysis type (see CommissioningAnalysisDescription::commissioningType) */
	virtual CommissioningAnalysisDescription::commissioningType getType() const { return CommissioningAnalysisDescription::T_ANALYSIS_APVLATENCY; };
	/** @brief get the list of parameter name (XML clob interface) */
	static parameterDescriptionNameType *getParameterNames();



  // getters / setters:

	/** @brief get human readable analysis type */
	std::string getAnalysisType() const;
	/** @brief set: APV latency setting. */
	void setLatency( const uint16_t value );
	/** @brief get: APV latency setting. */
	uint16_t getLatency() const;

};

#endif

