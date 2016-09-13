/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef OPTOSCANANALYSISDESCRIPTION_H
#define OPTOSCANANALYSISDESCRIPTION_H

#include "OptoScanAnalysisDescription.h"
#include "ParameterDescription.h"
#include "CommissioningAnalysisDescription.h"



/**
   @class OptoScanAnalysisDescription
   @author Sebastien BEL
   @brief  Opto bias/gain scan.
*/
class OptoScanAnalysisDescription : public CommissioningAnalysisDescription {


private:

  // properties:

// add parameter Baseline slope [ADC/I2C] : 36.47

	/** Optimum LLD gain setting. */
	uint16_t _gain;
	/** LLD bias value for each gain setting. */
	uint16_t _bias0;
	/** LLD bias value for each gain setting. */
	uint16_t _bias1;
	/** LLD bias value for each gain setting. */
	uint16_t _bias2;
	/** LLD bias value for each gain setting. */
	uint16_t _bias3;
	/** Measured gains for each setting [V/V]. */
	float _measGain0;
	/** Measured gains for each setting [V/V]. */
	float _measGain1;
	/** Measured gains for each setting [V/V]. */
	float _measGain2;
	/** Measured gains for each setting [V/V]. */
	float _measGain3;
	/** "Zero light" levels [ADC]. */
	float _zeroLight0;
	/** "Zero light" levels [ADC]. */
	float _zeroLight1;
	/** "Zero light" levels [ADC]. */
	float _zeroLight2;
	/** "Zero light" levels [ADC]. */
	float _zeroLight3;
	/** Noise value at zero light levels [ADC]. */
	float _linkNoise0;
	/** Noise value at zero light levels [ADC]. */
	float _linkNoise1;
	/** Noise value at zero light levels [ADC]. */
	float _linkNoise2;
	/** Noise value at zero light levels [ADC]. */
	float _linkNoise3;
	/** Baseline "lift-off" values [mA]. */
	float _liftOff0;
	/** Baseline "lift-off" values [mA]. */
	float _liftOff1;
	/** Baseline "lift-off" values [mA]. */
	float _liftOff2;
	/** Baseline "lift-off" values [mA]. */
	float _liftOff3;
	/** Laser thresholds [mA]. */
	float _threshold0;
	/** Laser thresholds [mA]. */
	float _threshold1;
	/** Laser thresholds [mA]. */
	float _threshold2;
	/** Laser thresholds [mA]. */
	float _threshold3;
	/** Tick mark heights [ADC]. */
	float _tickHeight0;
	/** Tick mark heights [ADC]. */
	float _tickHeight1;
	/** Tick mark heights [ADC]. */
	float _tickHeight2;
	/** Tick mark heights [ADC]. */
	float _tickHeight3;
	/** Base line slop [ADC/I2C] */
	float _baseLineSlop0;
	/** Base line slop [ADC/I2C] */
	float _baseLineSlop1;
	/** Base line slop [ADC/I2C] */
	float _baseLineSlop2;
	/** Base line slop [ADC/I2C] */
	float _baseLineSlop3;




public:

  // parameters list:

	/** @brief Enum of parameter names */  
	enum{BASELINESLOP0,BASELINESLOP1,BASELINESLOP2,BASELINESLOP3,GAIN,BIAS0,BIAS1,BIAS2,BIAS3,MEASGAIN0,MEASGAIN1,MEASGAIN2,MEASGAIN3,ZEROLIGHT0,ZEROLIGHT1,ZEROLIGHT2,ZEROLIGHT3,LINKNOISE0,LINKNOISE1,LINKNOISE2,LINKNOISE3,LIFTOFF0,LIFTOFF1,LIFTOFF2,LIFTOFF3,THRESHOLD0,THRESHOLD1,THRESHOLD2,THRESHOLD3,TICKHEIGHT0,TICKHEIGHT1,TICKHEIGHT2,TICKHEIGHT3};
	/** @brief List of parameter names */
	static const char *OPTOSCANANALYSISDESCRIPTION[ TICKHEIGHT3+1 ];


  // con(de)structors:

	/** @brief default constructor */
	OptoScanAnalysisDescription();
	/**
	 * @brief upload contructor 
	 * @param baseLineSlop0 - Base line slop [ADC/I2C]
	 * @param baseLineSlop1 - Base line slop [ADC/I2C]
	 * @param baseLineSlop2 - Base line slop [ADC/I2C]
	 * @param baseLineSlop3 - Base line slop [ADC/I2C]
	 * @param gain - Optimum LLD gain setting
	 * @param bias0 - LLD bias value for each gain setting
	 * @param bias1 - LLD bias value for each gain setting
	 * @param bias2 - LLD bias value for each gain setting
	 * @param bias3 - LLD bias value for each gain setting
	 * @param measGain0 - Measured gains for each setting [V/V]
	 * @param measGain1 - Measured gains for each setting [V/V]
	 * @param measGain2 - Measured gains for each setting [V/V]
	 * @param measGain3 - Measured gains for each setting [V/V]
	 * @param zeroLight0 - "Zero light" levels [ADC]
	 * @param zeroLight1 - "Zero light" levels [ADC]
	 * @param zeroLight2 - "Zero light" levels [ADC]
	 * @param zeroLight3 - "Zero light" levels [ADC]
	 * @param linkNoise0 - Noise value at zero light levels [ADC]
	 * @param linkNoise1 - Noise value at zero light levels [ADC]
	 * @param linkNoise2 - Noise value at zero light levels [ADC]
	 * @param linkNoise3 - Noise value at zero light levels [ADC]
	 * @param liftOff0 - Baseline "lift-off" values [mA]
	 * @param liftOff1 - Baseline "lift-off" values [mA]
	 * @param liftOff2 - Baseline "lift-off" values [mA]
	 * @param liftOff3 - Baseline "lift-off" values [mA]
	 * @param threshold0 - Laser thresholds [mA]
	 * @param threshold1 - Laser thresholds [mA]
	 * @param threshold2 - Laser thresholds [mA]
	 * @param threshold3 - Laser thresholds [mA]
	 * @param tickHeight0 - Tick mark heights [ADC]
	 * @param tickHeight1 - Tick mark heights [ADC]
	 * @param tickHeight2 - Tick mark heights [ADC]
	 * @param tickHeight3 - Tick mark heights [ADC]
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
	OptoScanAnalysisDescription(float baseLineSlop0,float baseLineSlop1,float baseLineSlop2,float baseLineSlop3,uint16_t gain,uint16_t bias0,uint16_t bias1,uint16_t bias2,uint16_t bias3,float measGain0,float measGain1,float measGain2,float measGain3,float zeroLight0,float zeroLight1,float zeroLight2,float zeroLight3,float linkNoise0,float linkNoise1,float linkNoise2,float linkNoise3,float liftOff0,float liftOff1,float liftOff2,float liftOff3,float threshold0,float threshold1,float threshold2,float threshold3,float tickHeight0,float tickHeight1,float tickHeight2,float tickHeight3,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv);
	/**
	 * Build a description based on a XML parsing (from file or database)
 	 * @param parameterNames - parameter names extracted from XML
	 */
	OptoScanAnalysisDescription(parameterDescriptionNameType parameterNames);



  // methods:

	/** @brief human readable object content */
	virtual std::string toString();
	/** @brief clone method */
	virtual CommissioningAnalysisDescription *clone();
	/** @brief display object content in standard output */
	virtual void display();
	/** @brief get the commissioning analysis type (see commissioningType) */
	virtual CommissioningAnalysisDescription::commissioningType getType() const { return CommissioningAnalysisDescription::T_ANALYSIS_OPTOSCAN; };
	/** @brief get the list of parameter name (XML clob interface) */
	static parameterDescriptionNameType *getParameterNames();



  // getters / setters:

	/** @brief get human readable analysis type */
	std::string getAnalysisType() const;
	/** @brief set: Base line slop [ADC/I2C] */
	void setBaseLineSlop0( const float value);
	/** @brief get: Base line slop [ADC/I2C] */
	float getBaseLineSlop0() const;
	/** @brief set: Base line slop [ADC/I2C] */
	void setBaseLineSlop1( const float value);
	/** @brief get: Base line slop [ADC/I2C] */
	float getBaseLineSlop1() const;
	/** @brief set: Base line slop [ADC/I2C] */
	void setBaseLineSlop2( const float value);
	/** @brief get: Base line slop [ADC/I2C] */
	float getBaseLineSlop2() const;
	/** @brief set: Base line slop [ADC/I2C] */
	void setBaseLineSlop3( const float value);
	/** @brief get: Base line slop [ADC/I2C] */
	float getBaseLineSlop3() const;
	/** @brief set: Optimum LLD gain setting. */
	void setGain( const uint16_t value );
	/** @brief get: Optimum LLD gain setting. */
	uint16_t getGain() const;
	/** @brief set: LLD bias value for each gain setting. */
	void setBias0( const uint16_t value );
	/** @brief get: LLD bias value for each gain setting. */
	uint16_t getBias0() const;
	/** @brief set: LLD bias value for each gain setting. */
	void setBias1( const uint16_t value );
	/** @brief get: LLD bias value for each gain setting. */
	uint16_t getBias1() const;
	/** @brief set: LLD bias value for each gain setting. */
	void setBias2( const uint16_t value );
	/** @brief get: LLD bias value for each gain setting. */
	uint16_t getBias2() const;
	/** @brief set: LLD bias value for each gain setting. */
	void setBias3( const uint16_t value );
	/** @brief get: LLD bias value for each gain setting. */
	uint16_t getBias3() const;
	/** @brief set: Measured gains for each setting [V/V]. */
	void setMeasGain0( const float value );
	/** @brief get: Measured gains for each setting [V/V]. */
	float getMeasGain0() const;
	/** @brief set: Measured gains for each setting [V/V]. */
	void setMeasGain1( const float value );
	/** @brief get: Measured gains for each setting [V/V]. */
	float getMeasGain1() const;
	/** @brief set: Measured gains for each setting [V/V]. */
	void setMeasGain2( const float value );
	/** @brief get: Measured gains for each setting [V/V]. */
	float getMeasGain2() const;
	/** @brief set: Measured gains for each setting [V/V]. */
	void setMeasGain3( const float value );
	/** @brief get: Measured gains for each setting [V/V]. */
	float getMeasGain3() const;
	/** @brief set: "Zero light" levels [ADC]. */
	void setZeroLight0( const float value );
	/** @brief get: "Zero light" levels [ADC]. */
	float getZeroLight0() const;
	/** @brief set: "Zero light" levels [ADC]. */
	void setZeroLight1( const float value );
	/** @brief get: "Zero light" levels [ADC]. */
	float getZeroLight1() const;
	/** @brief set: "Zero light" levels [ADC]. */
	void setZeroLight2( const float value );
	/** @brief get: "Zero light" levels [ADC]. */
	float getZeroLight2() const;
	/** @brief set: "Zero light" levels [ADC]. */
	void setZeroLight3( const float value );
	/** @brief get: "Zero light" levels [ADC]. */
	float getZeroLight3() const;
	/** @brief set: Noise value at zero light levels [ADC]. */
	void setLinkNoise0( const float value );
	/** @brief get: Noise value at zero light levels [ADC]. */
	float getLinkNoise0() const;
	/** @brief set: Noise value at zero light levels [ADC]. */
	void setLinkNoise1( const float value );
	/** @brief get: Noise value at zero light levels [ADC]. */
	float getLinkNoise1() const;
	/** @brief set: Noise value at zero light levels [ADC]. */
	void setLinkNoise2( const float value );
	/** @brief get: Noise value at zero light levels [ADC]. */
	float getLinkNoise2() const;
	/** @brief set: Noise value at zero light levels [ADC]. */
	void setLinkNoise3( const float value );
	/** @brief get: Noise value at zero light levels [ADC]. */
	float getLinkNoise3() const;
	/** @brief set: Baseline "lift-off" values [mA]. */
	void setLiftOff0( const float value );
	/** @brief get: Baseline "lift-off" values [mA]. */
	float getLiftOff0() const;
	/** @brief set: Baseline "lift-off" values [mA]. */
	void setLiftOff1( const float value );
	/** @brief get: Baseline "lift-off" values [mA]. */
	float getLiftOff1() const;
	/** @brief set: Baseline "lift-off" values [mA]. */
	void setLiftOff2( const float value );
	/** @brief get: Baseline "lift-off" values [mA]. */
	float getLiftOff2() const;
	/** @brief set: Baseline "lift-off" values [mA]. */
	void setLiftOff3( const float value );
	/** @brief get: Baseline "lift-off" values [mA]. */
	float getLiftOff3() const;
	/** @brief set: Laser thresholds [mA]. */
	void setThreshold0( const float value );
	/** @brief get: Laser thresholds [mA]. */
	float getThreshold0() const;
	/** @brief set: Laser thresholds [mA]. */
	void setThreshold1( const float value );
	/** @brief get: Laser thresholds [mA]. */
	float getThreshold1() const;
	/** @brief set: Laser thresholds [mA]. */
	void setThreshold2( const float value );
	/** @brief get: Laser thresholds [mA]. */
	float getThreshold2() const;
	/** @brief set: Laser thresholds [mA]. */
	void setThreshold3( const float value );
	/** @brief get: Laser thresholds [mA]. */
	float getThreshold3() const;
	/** @brief set: Tick mark heights [ADC]. */
	void setTickHeight0( const float value );
	/** @brief get: Tick mark heights [ADC]. */
	float getTickHeight0() const;
	/** @brief set: Tick mark heights [ADC]. */
	void setTickHeight1( const float value );
	/** @brief get: Tick mark heights [ADC]. */
	float getTickHeight1() const;
	/** @brief set: Tick mark heights [ADC]. */
	void setTickHeight2( const float value );
	/** @brief get: Tick mark heights [ADC]. */
	float getTickHeight2() const;
	/** @brief set: Tick mark heights [ADC]. */
	void setTickHeight3( const float value );
	/** @brief get: Tick mark heights [ADC]. */
	float getTickHeight3() const;

};

#endif

