/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef CALIBRATIONANALYSISDESCRIPTION_H
#define CALIBRATIONANALYSISDESCRIPTION_H

#include "CalibrationAnalysisDescription.h"
#include "ParameterDescription.h"
#include "CommissioningAnalysisDescription.h"



/**
   @class CalibrationAnalysisDescription
   @author Sebastien BEL
   @brief  Analysis for calibration runs.
*/
class CalibrationAnalysisDescription : public CommissioningAnalysisDescription {


private:

  // properties:

	/** Measured calibration pulse amplitude at maximum. */
	float _amplitude;
	/** Measured amplitude of the calibration pulse tail. */
	float _tail;
	/** Time needed to reach the maximum. */
	float _riseTime;
	/** Time constant of the CR-RC from the fit of the pulse. */
	float _timeConstant;
	/** Smearing of the CR-RC from the fit of the pulse. */
	float _smearing;
	/** Chi2 of the fit of the pulse. */
	float _chi2;
	/** Fit mode: deconv or not ? */
	bool _deconvMode;

	/** (static) calibration output mask [CDRV] */
	uint16_t _calChan;
	/** (static) ISHA */
	uint16_t _isha;
	/** (static) VFS */
	uint16_t _vfs;


public:

	/** @brief Enum of parameter names */
	enum{CALCHAN,ISHA,VFS,AMPLITUDE,TAIL,RISETIME,TIMECONSTANT,SMEARING,CHI2,DECONVMODE}; 
	/** @brief List of parameter names */
	static const char *CALIBRATIONANALYSISDESCRIPTION[ DECONVMODE+1 ];



  // con(de)structors:

	/** @brief default constructor */
	CalibrationAnalysisDescription();
	/**
	 * @brief upload contructor 
	 * @param amplitude - Measured calibration pulse amplitude at maximum
	 * @param tail - Measured amplitude of the calibration pulse tail
	 * @param riseTime - Time needed to reach the maximum
	 * @param timeConstant - Time constant of the CR-RC from the fit of the pulse
	 * @param smearing - Smearing of the CR-RC from the fit of the pulse
	 * @param chi2 - Chi2 of the fit of the pulse
	 * @param deconvMode - Fit mode: deconv or not ?
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
	 * @param calChan - calibration output mask [CDRV]
	 * @param isha - ISHA
	 * @param vfs - VFS
	 */
	CalibrationAnalysisDescription(float amplitude,float tail,float riseTime,float timeConstant,float smearing,float chi2,bool deconvMode,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv, uint16_t calChan, uint16_t isha, uint16_t vfs);
	/**
	 * Build a description based on a XML parsing (from file or database)
 	 * @param parameterNames - parameter names extracted from XML
	 */
	CalibrationAnalysisDescription(parameterDescriptionNameType parameterNames);



  // methods:

	/** @brief human readable object content */
	virtual std::string toString();
	/** @brief clone method */
	virtual CommissioningAnalysisDescription *clone();
	/** @brief display object content in standard output */
	virtual void display();
	/** @brief get the commissioning analysis type (see CommissioningAnalysisDescription::commissioningType) */
	virtual CommissioningAnalysisDescription::commissioningType getType() const { return CommissioningAnalysisDescription::T_ANALYSIS_CALIBRATION; };
	/** @brief get the list of parameter name (XML clob interface) */
	static parameterDescriptionNameType *getParameterNames();



  // getters / setters:

	/** @brief get human readable analysis type */
	std::string getAnalysisType() const;
	/** @brief set: Measured calibration pulse amplitude at maximum. */
	void setAmplitude( const float value );
	/** @brief get: Measured calibration pulse amplitude at maximum. */
	float getAmplitude() const;
	/** @brief set: Measured amplitude of the calibration pulse tail. */
	void setTail( const float value );
	/** @brief get: Measured amplitude of the calibration pulse tail. */
	float getTail() const;
	/** @brief set: Time needed to reach the maximum. */
	void setRiseTime( const float value );
	/** @brief get: Time needed to reach the maximum. */
	float getRiseTime() const;
	/** @brief set: Time constant of the CR-RC from the fit of the pulse. */
	void setTimeConstant( const float value );
	/** @brief get: Time constant of the CR-RC from the fit of the pulse. */
	float getTimeConstant() const;
	/** @brief set: Smearing of the CR-RC from the fit of the pulse. */
	void setSmearing( const float value );
	/** @brief get: Smearing of the CR-RC from the fit of the pulse. */
	float getSmearing() const;
	/** @brief set: Chi2 of the fit of the pulse. */
	void setChi2( const float value );
	/** @brief get: Chi2 of the fit of the pulse. */
	float getChi2() const;
	/** @brief set: Fit mode: deconv or not ? */
	void setDeconvMode( const bool value );
	/** @brief get: Fit mode: deconv or not ? */
	bool getDeconvMode() const;

	/** @brief set: (static) calibration output mask [CDRV] */
	void setCalChan( const uint16_t calChan );
	/** @brief get: (static) calibration output mask [CDRV] */
	uint16_t getCalChan() const;
	/** @brief set: (static) ISHA */
	void setIsha( const uint16_t isha );
	/** @brief get: (static) ISHA */
	uint16_t getIsha() const;
	/** @brief set: (static) VFS */
	void setVfs( const uint16_t vfs );
	/** @brief get: (static) VFS */
	uint16_t getVfs() const;


};

#endif

