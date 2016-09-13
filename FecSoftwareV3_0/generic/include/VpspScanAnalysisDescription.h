/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef VPSPSCANANALYSISDESCRIPTION_H
#define VPSPSCANANALYSISDESCRIPTION_H

#include "VpspScanAnalysisDescription.h"
#include "ParameterDescription.h"
#include "CommissioningAnalysisDescription.h"



/**
   @class VpspScanAnalysisDescription
   @author Sebastien BEL
   @brief  VPSP scan.
*/
class VpspScanAnalysisDescription : public CommissioningAnalysisDescription {


private:

  // properties:

	/** VPSP settings for one APV. */
	uint16_t _vpsp;
	/** Signal levels [ADC] for VPSP settings. */
	uint16_t _adcLevel;
	/** Not used. */
	uint16_t _fraction;
	/** VPSP setting where baseline leaves D1 level. */
	uint16_t _topEdge;
	/** VPSP setting where baseline leaves D0 level. */
	uint16_t _bottomEdge;
	/** Signal level [ADC] for "digital one". */
	uint16_t _topLevel;
	/** Signal level [ADC] for "digital zero". */
	uint16_t _bottomLevel;



public:

  // parameters list:

	/** @brief Enum of parameter names */  
	enum{VPSP,ADCLEVEL,FRACTION,TOPEDGE,BOTTOMEDGE,TOPLEVEL,BOTTOMLEVEL};
	/** @brief List of parameter names */
	static const char *VPSPSCANANALYSISDESCRIPTION[ BOTTOMLEVEL+1 ];


  // con(de)structors:

	/** @brief default constructor */
	VpspScanAnalysisDescription();
	/**
	 * @brief upload contructor 
	 * @param vpsp - VPSP settings for one APV
	 * @param adcLevel - Signal levels [ADC] for VPSP settings
	 * @param fraction - Not used
	 * @param topEdge - VPSP setting where baseline leaves D1 level
	 * @param bottomEdge - VPSP setting where baseline leaves D0 level
	 * @param topLevel - Signal level [ADC] for "digital one"
	 * @param bottomLevel - Signal level [ADC] for "digital zero"
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
	VpspScanAnalysisDescription(uint16_t vpsp,uint16_t adcLevel,uint16_t fraction,uint16_t topEdge,uint16_t bottomEdge,uint16_t topLevel,uint16_t bottomLevel,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv);
	/**
	 * Build a description based on a XML parsing (from file or database)
 	 * @param parameterNames - parameter names extracted from XML
	 */
	VpspScanAnalysisDescription(parameterDescriptionNameType parameterNames);



  // methods:

	/** @brief human readable object content */
	virtual std::string toString();
	/** @brief clone method */
	virtual CommissioningAnalysisDescription *clone();
	/** @brief display object content in standard output */
	virtual void display();
	/** @brief get the commissioning analysis type (see commissioningType) */
	virtual CommissioningAnalysisDescription::commissioningType getType() const { return CommissioningAnalysisDescription::T_ANALYSIS_VPSPSCAN; };
	/** @brief get the list of parameter name (XML clob interface) */
	static parameterDescriptionNameType *getParameterNames();



  // getters / setters:

	/** @brief get human readable analysis type */
	std::string getAnalysisType() const;
	/** @brief set: VPSP settings for one APV. */
	void setVpsp( const uint16_t value );
	/** @brief get: VPSP settings for one APV. */
	uint16_t getVpsp() const;
	/** @brief set: Signal levels [ADC] for VPSP settings. */
	void setAdcLevel( const uint16_t value );
	/** @brief get: Signal levels [ADC] for VPSP settings. */
	uint16_t getAdcLevel() const;
	/** @brief set: Not used. */
	void setFraction( const uint16_t value );
	/** @brief get: Not used. */
	uint16_t getFraction() const;
	/** @brief set: VPSP setting where baseline leaves D1 level. */
	void setTopEdge( const uint16_t value );
	/** @brief get: VPSP setting where baseline leaves D1 level. */
	uint16_t getTopEdge() const;
	/** @brief set: VPSP setting where baseline leaves D0 level. */
	void setBottomEdge( const uint16_t value );
	/** @brief get: VPSP setting where baseline leaves D0 level. */
	uint16_t getBottomEdge() const;
	/** @brief set: Signal level [ADC] for "digital one". */
	void setTopLevel( const uint16_t value );
	/** @brief get: Signal level [ADC] for "digital one". */
	uint16_t getTopLevel() const;
	/** @brief set: Signal level [ADC] for "digital zero". */
	void setBottomLevel( const uint16_t value );
	/** @brief get: Signal level [ADC] for "digital zero". */
	uint16_t getBottomLevel() const;

};

#endif

