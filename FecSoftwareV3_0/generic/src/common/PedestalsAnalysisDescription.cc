/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/


#include "PedestalsAnalysisDescription.h"
#include <iostream>
#include <vector>

const char *PedestalsAnalysisDescription::PEDESTALSANALYSISDESCRIPTION[] = {"dead","noisy","pedsMean","pedsSpread","noiseMean","noiseSpread","rawMean","rawSpread","pedsMax","pedsMin","noiseMax","noiseMin","rawMax","rawMin"};



////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
PedestalsAnalysisDescription::PedestalsAnalysisDescription() {
	this->setPedsMean(0.0);
	this->setPedsSpread(0.0);
	this->setNoiseMean(0.0);
	this->setNoiseSpread(0.0);
	this->setRawMean(0.0);
	this->setRawSpread(0.0);
	this->setPedsMax(0.0);
	this->setPedsMin(0.0);
	this->setNoiseMax(0.0);
	this->setNoiseMin(0.0);
	this->setRawMax(0.0);
	this->setRawMin(0.0);
}

//------------------------------------
PedestalsAnalysisDescription::PedestalsAnalysisDescription(VInt16 dead,VInt16 noisy,float pedsMean,float pedsSpread,float noiseMean,float noiseSpread,float rawMean,float rawSpread,float pedsMax,float pedsMin,float noiseMax,float noiseMin,float rawMax,float rawMin,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setDead(dead);
	this->setNoisy(noisy);
	this->setPedsMean(pedsMean);
	this->setPedsSpread(pedsSpread);
	this->setNoiseMean(noiseMean);
	this->setNoiseSpread(noiseSpread);
	this->setRawMean(rawMean);
	this->setRawSpread(rawSpread);
	this->setPedsMax(pedsMax);
	this->setPedsMin(pedsMin);
	this->setNoiseMax(noiseMax);
	this->setNoiseMin(noiseMin);
	this->setRawMax(rawMax);
	this->setRawMin(rawMin);
}

//------------------------------------
PedestalsAnalysisDescription::PedestalsAnalysisDescription(float pedsMean,float pedsSpread,float noiseMean,float noiseSpread,float rawMean,float rawSpread,float pedsMax,float pedsMin,float noiseMax,float noiseMin,float rawMax,float rawMin,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setPedsMean(pedsMean);
	this->setPedsSpread(pedsSpread);
	this->setNoiseMean(noiseMean);
	this->setNoiseSpread(noiseSpread);
	this->setRawMean(rawMean);
	this->setRawSpread(rawSpread);
	this->setPedsMax(pedsMax);
	this->setPedsMin(pedsMin);
	this->setNoiseMax(noiseMax);
	this->setNoiseMin(noiseMin);
	this->setRawMax(rawMax);
	this->setRawMin(rawMin);
}


//------------------------------------
PedestalsAnalysisDescription::PedestalsAnalysisDescription(parameterDescriptionNameType parameterNames) : CommissioningAnalysisDescription ( parameterNames ) {
	setDead(        parameterNames[PEDESTALSANALYSISDESCRIPTION[DEAD]]->getValue() );
	parameterNames[PEDESTALSANALYSISDESCRIPTION[DEAD]]->setValue("");
	setNoisy(       parameterNames[PEDESTALSANALYSISDESCRIPTION[NOISY]]->getValue() );
	parameterNames[PEDESTALSANALYSISDESCRIPTION[NOISY]]->setValue("");
	setPedsMean(    *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[PEDSMEAN]]->getValueConverted() );
	setPedsSpread(  *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[PEDSSPREAD]]->getValueConverted() );
	setNoiseMean(   *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[NOISEMEAN]]->getValueConverted() );
	setNoiseSpread( *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[NOISESPREAD]]->getValueConverted() );
	setRawMean(     *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[RAWMEAN]]->getValueConverted() );
	setRawSpread(   *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[RAWSPREAD]]->getValueConverted() );
	setPedsMax(     *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[PEDSMAX]]->getValueConverted() );
	setPedsMin(     *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[PEDSMIN]]->getValueConverted() );
	setNoiseMax(    *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[NOISEMAX]]->getValueConverted() );
	setNoiseMin(    *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[NOISEMIN]]->getValueConverted() );
	setRawMax(      *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[RAWMAX]]->getValueConverted() );
	setRawMin(      *(double * )parameterNames[PEDESTALSANALYSISDESCRIPTION[RAWMIN]]->getValueConverted() );
}




///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string PedestalsAnalysisDescription::toString() {
	std::stringstream ss;
	ss << "Key:" << std::endl;
	ss << CommissioningAnalysisDescription::_toString();
	ss << "PedestalsAnalysis properties:" << std::endl;
	ss << " . Dead        : " << this->getDead()        << std::endl;
	ss << " . Noisy       : " << this->getNoisy()       << std::endl;
	ss << " . PedsMean    : " << this->getPedsMean()    << std::endl;
	ss << " . PedsSpread  : " << this->getPedsSpread()  << std::endl;
	ss << " . NoiseMean   : " << this->getNoiseMean()   << std::endl;
	ss << " . NoiseSpread : " << this->getNoiseSpread() << std::endl;
	ss << " . RawMean     : " << this->getRawMean()     << std::endl;
	ss << " . RawSpread   : " << this->getRawSpread()   << std::endl;
	ss << " . PedsMax     : " << this->getPedsMax()     << std::endl;
	ss << " . PedsMin     : " << this->getPedsMin()     << std::endl;
	ss << " . NoiseMax    : " << this->getNoiseMax()    << std::endl;
	ss << " . NoiseMin    : " << this->getNoiseMin()    << std::endl;
	ss << " . RawMax      : " << this->getRawMax()      << std::endl;
	ss << " . RawMin      : " << this->getRawMin()      << std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
CommissioningAnalysisDescription *PedestalsAnalysisDescription::clone() {	
	PedestalsAnalysisDescription * c = new PedestalsAnalysisDescription(
		this->getPedsMean(), this->getPedsSpread(), this->getNoiseMean(), this->getNoiseSpread(), this->getRawMean(), this->getRawSpread(), 
		this->getPedsMax(), this->getPedsMin(), this->getNoiseMax(), this->getNoiseMin(), this->getRawMax(), this->getRawMin(),
		this->getCrate(),this->getSlot(),this->getRing(),this->getCcuAdr(),this->getCcuChan(),
		this->getI2cAddr(),this->getPartition(),this->getRunNumber(),this->isValid(),
		this->getSerializedComments(),this->getFedId(),this->getFeUnit(),this->getFeChan(),
		this->getFedApv()
	);
	c->setDead( this->getDead() );
	c->setNoisy( this->getNoisy() );
	return c;
}

//------------------------------------
void PedestalsAnalysisDescription::display() {
	std::cout << this->toString() << std::endl;
}

//------------------------------------
parameterDescriptionNameType *PedestalsAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames = CommissioningAnalysisDescription::getParameterNames();
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[DEAD]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[DEAD], ParameterDescription::INTEGER16);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[NOISY]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[NOISY], ParameterDescription::INTEGER16);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[PEDSMEAN]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[PEDSMEAN], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[PEDSSPREAD]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[PEDSSPREAD], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[NOISEMEAN]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[NOISEMEAN], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[NOISESPREAD]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[NOISESPREAD], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[RAWMEAN]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[RAWMEAN], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[RAWSPREAD]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[RAWSPREAD], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[PEDSMAX]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[PEDSMAX], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[PEDSMIN]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[PEDSMIN], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[NOISEMAX]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[NOISEMAX], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[NOISEMIN]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[NOISEMIN], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[RAWMAX]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[RAWMAX], ParameterDescription::REAL);
	(*parameterNames)[PEDESTALSANALYSISDESCRIPTION[RAWMIN]] = new ParameterDescription(PEDESTALSANALYSISDESCRIPTION[RAWMIN], ParameterDescription::REAL);
	return parameterNames;
}





//////////////////// GETTERS / SETTERS //////////////////////////





/** @brief get human readable analysis type */
std::string PedestalsAnalysisDescription::getAnalysisType() const {
	return CommissioningAnalysisDescription::getAnalysisType( this->getType() );
}

/** @brief set: Dead strips values are strip numbers. */
void PedestalsAnalysisDescription::setDead( const VInt16 value ) {
	this->_dead = value;
}
/** @brief set: Dead strips values are strip numbers concatened and separated by '|'. */
void PedestalsAnalysisDescription::setDead( const std::string &value ) {
	this->_dead.clear();
	if ( value.empty() ) return;
	std::string buf;
	for (size_t i = 0, size = value.size(); i < size; i++) {
		if ( value[i]=='|' ) {
			this->_dead.push_back( atoi(buf.c_str() ) );
			buf.erase();
		} else {
			buf.append(value, i, 1);
		}
	}
	if ( buf != "" )
		this->_dead.push_back( atoi(buf.c_str() ) );
}

/** @brief get: Dead strips values are strip numbers. */
void PedestalsAnalysisDescription::getDead( VInt16 &value ) {
	value = this->_dead;
}
/** @brief get: Dead strips values are strip numbers concatened and separated by '|'. */
std::string PedestalsAnalysisDescription::getDead() const {
	std::stringstream ss;
	std::string sep("");
	for (size_t i = 0, size = this->_dead.size(); i < size; ++i) {
		ss << sep << this->_dead[i] ;
		sep="|";
	}
	return ss.rdbuf()->str();
}


/** @brief set: Noisy strips values are strip numbers. */
void PedestalsAnalysisDescription::setNoisy( const VInt16 value ) {
	this->_noisy = value;
}
/** @brief set: Noisy strips values are strip numbers concatened and separated by '|'. */
void PedestalsAnalysisDescription::setNoisy( const std::string &value ) {
	this->_noisy.clear();
	if ( value.empty() ) return;
	std::string buf;
	for (size_t i = 0, size = value.size(); i < size; i++) {
		if ( value[i]=='|' ) {
			this->_noisy.push_back( atoi(buf.c_str() ) );
			buf.erase();
		} else {
			buf.append(value, i, 1);
		}
	}
	if ( buf != "" )
		this->_noisy.push_back( atoi(buf.c_str() ) );
}

/** @brief get: Noisy strips values are strip numbers. */
void PedestalsAnalysisDescription::getNoisy( VInt16 &value ) {
	value = this->_noisy;
}
/** @brief get: Noisy strips values are strip numbers concatened and separated by '|'. */
std::string PedestalsAnalysisDescription::getNoisy() const {
	std::stringstream ss;
	std::string sep("");
	for (size_t i = 0, size = this->_noisy.size(); i < size; ++i) {
		ss  << sep << this->_noisy[i];
		sep="|";
	}
	return ss.rdbuf()->str();	
}


/** @brief set: Mean peds value. */
void PedestalsAnalysisDescription::setPedsMean( const float value ) {
	this->_pedsMean = value;
}
/** @brief get: Mean peds value. */
float PedestalsAnalysisDescription::getPedsMean() const {
	return this->_pedsMean;
}
/** @brief set: Rms spread in peds. */
void PedestalsAnalysisDescription::setPedsSpread( const float value ) {
	this->_pedsSpread = value;
}
/** @brief get: Rms spread in peds. */
float PedestalsAnalysisDescription::getPedsSpread() const {
	return this->_pedsSpread;
}
/** @brief set: Mean noise value. */
void PedestalsAnalysisDescription::setNoiseMean( const float value ) {
	this->_noiseMean = value;
}
/** @brief get: Mean noise value. */
float PedestalsAnalysisDescription::getNoiseMean() const {
	return this->_noiseMean;
}
/** @brief set: Rms spread in noise. */
void PedestalsAnalysisDescription::setNoiseSpread( const float value ) {
	this->_noiseSpread = value;
}
/** @brief get: Rms spread in noise. */
float PedestalsAnalysisDescription::getNoiseSpread() const {
	return this->_noiseSpread;
}
/** @brief set: Mean raw noise value. */
void PedestalsAnalysisDescription::setRawMean( const float value ) {
	this->_rawMean = value;
}
/** @brief get: Mean raw noise value. */
float PedestalsAnalysisDescription::getRawMean() const {
	return this->_rawMean;
}
/** @brief set: Rms spread in raw noise. */
void PedestalsAnalysisDescription::setRawSpread( const float value ) {
	this->_rawSpread = value;
}
/** @brief get: Rms spread in raw noise. */
float PedestalsAnalysisDescription::getRawSpread() const {
	return this->_rawSpread;
}
/** @brief set: Max peds value. */
void PedestalsAnalysisDescription::setPedsMax( const float value ) {
	this->_pedsMax = value;
}
/** @brief get: Max peds value. */
float PedestalsAnalysisDescription::getPedsMax() const {
	return this->_pedsMax;
}
/** @brief set: Min peds value. */
void PedestalsAnalysisDescription::setPedsMin( const float value ) {
	this->_pedsMin = value;
}
/** @brief get: Min peds value. */
float PedestalsAnalysisDescription::getPedsMin() const {
	return this->_pedsMin;
}
/** @brief set: Max noise value. */
void PedestalsAnalysisDescription::setNoiseMax( const float value ) {
	this->_noiseMax = value;
}
/** @brief get: Max noise value. */
float PedestalsAnalysisDescription::getNoiseMax() const {
	return this->_noiseMax;
}
/** @brief set: Min noise value. */
void PedestalsAnalysisDescription::setNoiseMin( const float value ) {
	this->_noiseMin = value;
}
/** @brief get: Min noise value. */
float PedestalsAnalysisDescription::getNoiseMin() const {
	return this->_noiseMin;
}
/** @brief set: Max raw noise value. */
void PedestalsAnalysisDescription::setRawMax( const float value ) {
	this->_rawMax = value;
}
/** @brief get: Max raw noise value. */
float PedestalsAnalysisDescription::getRawMax() const {
	return this->_rawMax;
}
/** @brief set: Min raw noise value. */
void PedestalsAnalysisDescription::setRawMin( const float value ) {
	this->_rawMin = value;
}
/** @brief get: Min raw noise value. */
float PedestalsAnalysisDescription::getRawMin() const {
	return this->_rawMin;
}
