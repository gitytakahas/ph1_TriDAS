/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef FASTFEDCABLINGANALYSISDESCRIPTION_H
#define FASTFEDCABLINGANALYSISDESCRIPTION_H

#include "FastFedCablingAnalysisDescription.h"
#include "ParameterDescription.h"
#include "CommissioningAnalysisDescription.h"



/**
   @class FastFedCablingAnalysisDescription
   @author Sebastien BEL
   @brief  Connection loop.
*/
class FastFedCablingAnalysisDescription : public CommissioningAnalysisDescription {


private:

  // properties:

	/** High light level [ADC]. */
	float _highLevel;
	/** Spread in high ligh level [ADC]. */
	float _highRms;
	/** Low light level [ADC]. */
	float _lowLevel;
	/** Spread in low ligh level [ADC]. */
	float _lowRms;
	/** Maximum light level in data [ADC]. */
	float _maxll;
	/** Minimum light level in data [ADC]. */
	float _minll;
	/** DCU hardware id 32-bits. */
	uint32_t _dcuId;
	/** Extracted LLD channel. */
	uint16_t _lldCh;
	/** Identifies if fibre is dirty or not. */
	bool _isDirty;

	/** (Static property) Threshold to identify digital high from digital low. */
	float _threshold;
	/** (Static property) Defines level [ADC] below which fibre is defined as "dirty". */
	float _dirtyThreshold;




public:

	/** @brief Enum of parameter names */
	enum{THRESHOLD,DIRTYTHRESHOLD,ISDIRTY,HIGHLEVEL,HIGHRMS,LOWLEVEL,LOWRMS,MAXLL,MINLL,DCUID,LLDCH};
	/** @brief List of parameter names */
	static const char *FASTFEDCABLINGANALYSISDESCRIPTION[ LLDCH+1 ];


  // con(de)structors:

	/** @brief default constructor */
	FastFedCablingAnalysisDescription();
	/**
	 * @brief upload contructor 
	 * @param highLevel - High light level [ADC]
	 * @param highRms - Spread in high ligh level [ADC]
	 * @param lowLevel - Low light level [ADC]
	 * @param lowRms - Spread in low ligh level [ADC]
	 * @param maxll - Maximum light level in data [ADC]
	 * @param minll - Minimum light level in data [ADC]
	 * @param dcuId - DCU hardware id 32-bits
	 * @param lldCh - Extracted LLD channel
	 * @param isDirty - Identifies if fibre is dirty or not
	 * @param threshold - Threshold to identify digital high from digital low
	 * @param dirtyThreshold - Defines level [ADC] below which fibre is defined as "dirty"
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
	FastFedCablingAnalysisDescription(float highLevel,float highRms,float lowLevel,float lowRms,float maxll,float minll,uint32_t dcuId,uint16_t lldCh,bool isDirty,float threshold,float dirtyThreshold,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv);
	/**
	 * Build a description based on a XML parsing (from file or database)
 	 * @param parameterNames - parameter names extracted from XML
	 */
	FastFedCablingAnalysisDescription(parameterDescriptionNameType parameterNames);



  // methods:

	/** @brief clone method */
	virtual CommissioningAnalysisDescription *clone();
	/** @brief human readable object content */
	virtual std::string toString();
	/** @brief display object content in standard output */
	virtual void display();
	/** @brief get the commissioning analysis type (see commissioningType) */
	virtual CommissioningAnalysisDescription::commissioningType getType() const { return CommissioningAnalysisDescription::T_ANALYSIS_FASTFEDCABLING; };
	/** @brief get the list of parameter name (XML clob interface) */
	static parameterDescriptionNameType *getParameterNames();



  // getters / setters:

	/** @brief get human readable analysis type */
	std::string getAnalysisType() const;
	/** @brief set: High light level [ADC]. */
	void setHighLevel( const float value );
	/** @brief get: High light level [ADC]. */
	float getHighLevel() const;
	/** @brief set: Spread in high ligh level [ADC]. */
	void setHighRms( const float value );
	/** @brief get: Spread in high ligh level [ADC]. */
	float getHighRms() const;
	/** @brief set: Low light level [ADC]. */
	void setLowLevel( const float value );
	/** @brief get: Low light level [ADC]. */
	float getLowLevel() const;
	/** @brief set: Spread in low ligh level [ADC]. */
	void setLowRms( const float value );
	/** @brief get: Spread in low ligh level [ADC]. */
	float getLowRms() const;
	/** @brief set: Maximum light level in data [ADC]. */
	void setMaxll( const float value );
	/** @brief get: Maximum light level in data [ADC]. */
	float getMaxll() const;
	/** @brief set: Minimum light level in data [ADC]. */
	void setMinll( const float value );
	/** @brief get: Minimum light level in data [ADC]. */
	float getMinll() const;
	/** @brief set: DCU hardware id 32-bits. */
	void setDcuId( const uint32_t value );
	/** @brief get: DCU hardware id 32-bits. */
	uint32_t getDcuId() const;
	/** @brief set: Extracted LLD channel. */
	void setLldCh( const uint16_t value );
	/** @brief get: Extracted LLD channel. */
	uint16_t getLldCh() const;

	/** Identifies if fibre is dirty or not. */
	void isDirty( const bool value ) { this->_isDirty = value; }
	/** Identifies if fibre is dirty or not. */
	bool isDirty() const { return this->_isDirty; }

	/** @brief set: Threshold to identify digital high from digital low. */
	void setThreshold( const float value ) { this->_threshold = value; }
	/** @brief get: Threshold to identify digital high from digital low. */
	float getThreshold() const { return this->_threshold; }
	/** @brief set: Defines level [ADC] below which fibre is defined as "dirty". */
	void setDirtyThreshold( const float value ) { this->_dirtyThreshold = value; }
	/** @brief get: Defines level [ADC] below which fibre is defined as "dirty". */
	float getDirtyThreshold() const { return this->_dirtyThreshold; }

};

#endif

