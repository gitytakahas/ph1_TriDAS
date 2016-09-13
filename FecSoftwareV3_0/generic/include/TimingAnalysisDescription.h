/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef TIMINGANALYSISDESCRIPTION_H
#define TIMINGANALYSISDESCRIPTION_H

#include "TimingAnalysisDescription.h"
#include "ParameterDescription.h"
#include "CommissioningAnalysisDescription.h"



/**
   @class TimingAnalysisDescription
   @author Sebastien BEL
   @brief  Analysis for timing APV and FED run using APV tick marks.
*/
class TimingAnalysisDescription : public CommissioningAnalysisDescription {


private:
  // properties:
	/** Time of tick mark rising edge [ns]. */
	float _timetmre;
	/** Sampling point of reference tick mark [ns]. */
	float _refTime;
	/** Delay required to sync w.r.t. reference tick mark [ns]. */
	float _delay;
	/** Height of tick mark [ADC]. */
	float _height;
	/** Baseline level of tick mark [ADC]. */
	float _base;
	/** Level of tick mark top [ADC]. */
	float _peak;
	/** FED frame-finding threshold [ADC]. */
	float _frameFindingThreshold;
	/** Static Optimum sampling point, defined w.r.t. rising edge [ns]. */
	float _optimumSamplingPoint;
	/** Static Threshold defining minimum tick mark height [ADC]. */
	float _tickMarkHeightThreshold;
	/** True=APV analysis, False=FED analysis. */
	bool _kind;



public:

  // parameters list:

	/** @brief Enum of parameter names */  
	enum{TIMETMRE,REFTIME,DELAY,HEIGHT,BASE,PEAK,FRAMEFINDINGTHRESHOLD,OPTIMUMSAMPLINGPOINT,TICKMARKHEIGHTTHRESHOLD,KIND};
	/** @brief List of parameter names */
	static const char *TIMINGANALYSISDESCRIPTION[ KIND+1 ];


  // con(de)structors:

	/** @brief default constructor */
	TimingAnalysisDescription();
	/**
	 * @brief upload contructor 
	 * @param timetmre - Time of tick mark rising edge [ns]
	 * @param refTime - Sampling point of reference tick mark [ns]
	 * @param delay - Delay required to sync w.r.t. reference tick mark [ns]
	 * @param height - Height of tick mark [ADC]
	 * @param base - Baseline level of tick mark [ADC]
	 * @param peak - Level of tick mark top [ADC]
	 * @param frameFindingThreshold - FED frame-finding threshold [ADC]
	 * @param optimumSamplingPoint - Static Optimum sampling point, defined w.r.t. rising edge [ns]
	 * @param tickMarkHeightThreshold - Static Threshold defining minimum tick mark height [ADC]
	 * @param kind - True=APV analysis, False=FED analysis
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
	TimingAnalysisDescription(float timetmre,float refTime,float delay,float height,float base,float peak,float frameFindingThreshold,float optimumSamplingPoint,float tickMarkHeightThreshold,bool kind,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv);
	/**
	 * Build a description based on a XML parsing (from file or database)
 	 * @param parameterNames - parameter names extracted from XML
	 */
	TimingAnalysisDescription(parameterDescriptionNameType parameterNames);



  // methods:

	/** @brief human readable object content */
	virtual std::string toString();
	/** @brief clone method */
	virtual CommissioningAnalysisDescription *clone();
	/** @brief display object content in standard output */
	virtual void display();
	/** @brief get the commissioning analysis type (see commissioningType) */
	virtual CommissioningAnalysisDescription::commissioningType getType() const { return CommissioningAnalysisDescription::T_ANALYSIS_TIMING; };
	/** @brief get the list of parameter name (XML clob interface) */
	static parameterDescriptionNameType *getParameterNames();



  // getters / setters:

	/** @brief get human readable analysis type */
	std::string getAnalysisType() const;
	/** @brief set: Time of tick mark rising edge [ns]. */
	void setTimetmre( const float value );
	/** @brief get: Time of tick mark rising edge [ns]. */
	float getTimetmre() const;
	/** @brief set: Sampling point of reference tick mark [ns]. */
	void setRefTime( const float value );
	/** @brief get: Sampling point of reference tick mark [ns]. */
	float getRefTime() const;
	/** @brief set: Delay required to sync w.r.t. reference tick mark [ns]. */
	void setDelay( const float value );
	/** @brief get: Delay required to sync w.r.t. reference tick mark [ns]. */
	float getDelay() const;
	/** @brief set: Height of tick mark [ADC]. */
	void setHeight( const float value );
	/** @brief get: Height of tick mark [ADC]. */
	float getHeight() const;
	/** @brief set: Baseline level of tick mark [ADC]. */
	void setBase( const float value );
	/** @brief get: Baseline level of tick mark [ADC]. */
	float getBase() const;
	/** @brief set: Level of tick mark top [ADC]. */
	void setPeak( const float value );
	/** @brief get: Level of tick mark top [ADC]. */
	float getPeak() const;
	/** @brief set: FED frame-finding threshold [ADC]. */
	void setFrameFindingThreshold( const float value );
	/** @brief get: FED frame-finding threshold [ADC]. */
	float getFrameFindingThreshold() const;
	/** @brief set: Static Optimum sampling point, defined w.r.t. rising edge [ns]. */
	void setOptimumSamplingPoint( const float value );
	/** @brief get: Static Optimum sampling point, defined w.r.t. rising edge [ns]. */
	float getOptimumSamplingPoint() const;
	/** @brief set: Static Threshold defining minimum tick mark height [ADC]. */
	void setTickMarkHeightThreshold( const float value );
	/** @brief get: Static Threshold defining minimum tick mark height [ADC]. */
	float getTickMarkHeightThreshold() const;
	/** @brief set: True=APV analysis, False=FED analysis. */
	void setKind( const bool value );
	/** @brief get: True=APV analysis, False=FED analysis. */
	bool getKind() const;

};

#endif

