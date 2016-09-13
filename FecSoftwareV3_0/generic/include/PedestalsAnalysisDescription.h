/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/

#ifndef PEDESTALSANALYSISDESCRIPTION_H
#define PEDESTALSANALYSISDESCRIPTION_H

#include "PedestalsAnalysisDescription.h"
#include "ParameterDescription.h"
#include "CommissioningAnalysisDescription.h"


/**
   @class PedestalsAnalysisDescription
   @author Sebastien BEL
   @brief  Pedestal run.
*/
class PedestalsAnalysisDescription : public CommissioningAnalysisDescription {


private:

  // properties:

	/** Dead strips values are strip numbers. */
	VInt16 _dead;
	/** Noisy strips values are strip numbers. */
	VInt16 _noisy;
	/** Mean peds value. */
	float _pedsMean;
	/** Rms spread in peds. */
	float _pedsSpread;
	/** Mean noise value. */
	float _noiseMean;
	/** Rms spread in noise. */
	float _noiseSpread;
	/** Mean raw noise value. */
	float _rawMean;
	/** Rms spread in raw noise. */
	float _rawSpread;
	/** Max peds value. */
	float _pedsMax;
	/** Min peds value. */
	float _pedsMin;
	/** Max noise value. */
	float _noiseMax;
	/** Min noise value. */
	float _noiseMin;
	/** Max raw noise value. */
	float _rawMax;
	/** Min raw noise value. */
	float _rawMin;



public:

  // parameters list:

	/** @brief Enum of parameter names */  
	enum{DEAD,NOISY,PEDSMEAN,PEDSSPREAD,NOISEMEAN,NOISESPREAD,RAWMEAN,RAWSPREAD,PEDSMAX,PEDSMIN,NOISEMAX,NOISEMIN,RAWMAX,RAWMIN};
	/** @brief List of parameter names */
	static const char *PEDESTALSANALYSISDESCRIPTION[ RAWMIN+1 ];


  // con(de)structors:

	/** @brief default constructor */
	PedestalsAnalysisDescription();
	/**
	 * @brief upload contructor 
	 * @param dead - Dead strips values are strip numbers
	 * @param noisy - Noisy strips values are strip numbers
	 * @param pedsMean - Mean peds value
	 * @param pedsSpread - Rms spread in peds
	 * @param noiseMean - Mean noise value
	 * @param noiseSpread - Rms spread in noise
	 * @param rawMean - Mean raw noise value
	 * @param rawSpread - Rms spread in raw noise
	 * @param pedsMax - Max peds value
	 * @param pedsMin - Min peds value
	 * @param noiseMax - Max noise value
	 * @param noiseMin - Min noise value
	 * @param rawMax - Max raw noise value
	 * @param rawMin - Min raw noise value
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
	PedestalsAnalysisDescription(VInt16 dead,VInt16 noisy,float pedsMean,float pedsSpread,float noiseMean,float noiseSpread,float rawMean,float rawSpread,float pedsMax,float pedsMin,float noiseMax,float noiseMin,float rawMax,float rawMin,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv);
	
		/**
	 * @brief clone contructor 
	 * @param pedsMean - Mean peds value
	 * @param pedsSpread - Rms spread in peds
	 * @param noiseMean - Mean noise value
	 * @param noiseSpread - Rms spread in noise
	 * @param rawMean - Mean raw noise value
	 * @param rawSpread - Rms spread in raw noise
	 * @param pedsMax - Max peds value
	 * @param pedsMin - Min peds value
	 * @param noiseMax - Max noise value
	 * @param noiseMin - Min noise value
	 * @param rawMax - Max raw noise value
	 * @param rawMin - Min raw noise value
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
	PedestalsAnalysisDescription(float pedsMean,float pedsSpread,float noiseMean,float noiseSpread,float rawMean,float rawSpread,float pedsMax,float pedsMin,float noiseMax,float noiseMin,float rawMax,float rawMin,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv);
	
	/**
	 * Build a description based on a XML parsing (from file or database)
 	 * @param parameterNames - parameter names extracted from XML
	 */
	PedestalsAnalysisDescription(parameterDescriptionNameType parameterNames);



  // methods:

	/** @brief human readable object content */
	virtual std::string toString();
	/** @brief clone method */
	virtual CommissioningAnalysisDescription *clone();
	/** @brief display object content in standard output */
	virtual void display();
	/** @brief get the commissioning analysis type (see commissioningType) */
	virtual CommissioningAnalysisDescription::commissioningType getType() const { return CommissioningAnalysisDescription::T_ANALYSIS_PEDESTALS; }
	/** @brief get the list of parameter name (XML clob interface) */
	static parameterDescriptionNameType *getParameterNames();


  // getters / setters:

	/** @brief get human readable analysis type */
	std::string getAnalysisType() const;
	/** @brief set: Dead strips values are strip numbers. */
	void setDead( const VInt16 value );
	/** @brief set: Dead strips values are strip numbers concatened and separated by '|'. */
	void setDead( const std::string &value );
	/** @brief get: Dead strips values are strip numbers concatened and separated by '|'. */
	std::string getDead() const;
	/** @brief get: Dead strips values are strip numbers. */
	void getDead( VInt16 &value );
	/** @brief set: Noisy strips values are strip numbers. */
	void setNoisy( const VInt16 value );
	/** @brief set: Noisy strips values are strip numbers concatened and separated by '|'. */
	void setNoisy( const std::string &value );
	/** @brief get: Noisy strips values are strip numbers concatened and separated by '|'. */
	std::string getNoisy() const;
	/** @brief get: Noisy strips values are strip numbers. */
	void getNoisy( VInt16 &value );
	/** @brief set: Mean peds value. */
	void setPedsMean( const float value );
	/** @brief get: Mean peds value. */
	float getPedsMean() const;
	/** @brief set: Rms spread in peds. */
	void setPedsSpread( const float value );
	/** @brief get: Rms spread in peds. */
	float getPedsSpread() const;
	/** @brief set: Mean noise value. */
	void setNoiseMean( const float value );
	/** @brief get: Mean noise value. */
	float getNoiseMean() const;
	/** @brief set: Rms spread in noise. */
	void setNoiseSpread( const float value );
	/** @brief get: Rms spread in noise. */
	float getNoiseSpread() const;
	/** @brief set: Mean raw noise value. */
	void setRawMean( const float value );
	/** @brief get: Mean raw noise value. */
	float getRawMean() const;
	/** @brief set: Rms spread in raw noise. */
	void setRawSpread( const float value );
	/** @brief get: Rms spread in raw noise. */
	float getRawSpread() const;
	/** @brief set: Max peds value. */
	void setPedsMax( const float value );
	/** @brief get: Max peds value. */
	float getPedsMax() const;
	/** @brief set: Min peds value. */
	void setPedsMin( const float value );
	/** @brief get: Min peds value. */
	float getPedsMin() const;
	/** @brief set: Max noise value. */
	void setNoiseMax( const float value );
	/** @brief get: Max noise value. */
	float getNoiseMax() const;
	/** @brief set: Min noise value. */
	void setNoiseMin( const float value );
	/** @brief get: Min noise value. */
	float getNoiseMin() const;
	/** @brief set: Max raw noise value. */
	void setRawMax( const float value );
	/** @brief get: Max raw noise value. */
	float getRawMax() const;
	/** @brief set: Min raw noise value. */
	void setRawMin( const float value );
	/** @brief get: Min raw noise value. */
	float getRawMin() const;

};

#endif

