/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/


#include "CalibrationAnalysisDescription.h"


const char *CalibrationAnalysisDescription::CALIBRATIONANALYSISDESCRIPTION[] = {"calChan","isha","vfs","amplitude","tail","riseTime","timeConstant","smearing","chi2","deconvMode"};



////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
CalibrationAnalysisDescription::CalibrationAnalysisDescription() {
	this->setAmplitude(0.0);
	this->setTail(0.0);
	this->setRiseTime(0.0);
	this->setTimeConstant(0.0);
	this->setSmearing(0.0);
	this->setChi2(0.0);
	this->setDeconvMode("");
	this->setCalChan(0);
	this->setIsha(0);
	this->setVfs(0);
}

//------------------------------------
CalibrationAnalysisDescription::CalibrationAnalysisDescription(float amplitude,float tail,float riseTime,float timeConstant,float smearing,float chi2,bool deconvMode,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv, uint16_t calChan, uint16_t isha, uint16_t vfs) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setAmplitude(amplitude);
	this->setTail(tail);
	this->setRiseTime(riseTime);
	this->setTimeConstant(timeConstant);
	this->setSmearing(smearing);
	this->setChi2(chi2);
	this->setDeconvMode(deconvMode);
	this->setCalChan(calChan);
	this->setIsha(isha);
	this->setVfs(vfs);
}

//------------------------------------
CalibrationAnalysisDescription::CalibrationAnalysisDescription(parameterDescriptionNameType parameterNames) : CommissioningAnalysisDescription ( parameterNames ) {
	this->setAmplitude( *(double * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[AMPLITUDE]]->getValueConverted() );
	this->setTail( *(double * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[TAIL]]->getValueConverted() );
	this->setRiseTime( *(double * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[RISETIME]]->getValueConverted() );
	this->setTimeConstant( *(double * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[TIMECONSTANT]]->getValueConverted() );
	this->setSmearing( *(double * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[SMEARING]]->getValueConverted() );
	this->setChi2( *(double * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[CHI2]]->getValueConverted() );
	this->setDeconvMode( *(bool * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[DECONVMODE]]->getValueConverted() );
	this->setCalChan(*(uint16_t * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[CALCHAN]]->getValueConverted());
	this->setIsha(*(uint16_t * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[ISHA]]->getValueConverted());
	this->setVfs(*(uint16_t * )parameterNames[CALIBRATIONANALYSISDESCRIPTION[VFS]]->getValueConverted());
}



///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string CalibrationAnalysisDescription::toString() {
	std::stringstream ss;
	ss << "Key:" << std::endl;
	ss << CommissioningAnalysisDescription::_toString();
	ss << "CalibrationAnalysis properties:" << std::endl;
	ss << " . Amplitude    : " << this->getAmplitude()    << std::endl;
	ss << " . Tail         : " << this->getTail()         << std::endl;
	ss << " . RiseTime     : " << this->getRiseTime()     << std::endl;
	ss << " . TimeConstant : " << this->getTimeConstant() << std::endl;
	ss << " . Smearing     : " << this->getSmearing()     << std::endl;
	ss << " . Chi2         : " << this->getChi2()         << std::endl;
	ss << " . DeconvMode   : " << this->getDeconvMode()   << std::endl;
	ss << " . CalChan      : " << this->getCalChan()      << std::endl;
	ss << " . Isha         : " << this->getIsha()         << std::endl;
	ss << " . Vfs          : " << this->getVfs()          << std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
CommissioningAnalysisDescription *CalibrationAnalysisDescription::clone() {
	CalibrationAnalysisDescription * c = new CalibrationAnalysisDescription(
		this->getAmplitude(),this->getTail(),this->getRiseTime(),this->getTimeConstant(),
		this->getSmearing(),this->getChi2(),this->getDeconvMode(),this->getCrate(),
		this->getSlot(),this->getRing(),this->getCcuAdr(),this->getCcuChan(),this->getI2cAddr(),
		this->getPartition(),this->getRunNumber(),this->isValid(),this->getSerializedComments(),
		this->getFedId(),this->getFeUnit(),this->getFeChan(),this->getFedApv(),
		this->getCalChan(),this->getIsha(),this->getVfs()
	);
	return c;
}

//------------------------------------
void CalibrationAnalysisDescription::display() {
	std::cout << this->toString() << std::endl;
}

//------------------------------------
parameterDescriptionNameType *CalibrationAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames = CommissioningAnalysisDescription::getParameterNames();
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[AMPLITUDE]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[AMPLITUDE], ParameterDescription::REAL);
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[TAIL]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[TAIL], ParameterDescription::REAL);
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[RISETIME]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[RISETIME], ParameterDescription::REAL);
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[TIMECONSTANT]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[TIMECONSTANT], ParameterDescription::REAL);
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[SMEARING]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[SMEARING], ParameterDescription::REAL);
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[CHI2]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[CHI2], ParameterDescription::REAL);
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[DECONVMODE]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[DECONVMODE], ParameterDescription::STRING);
	
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[CALCHAN]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[CALCHAN], ParameterDescription::INTEGER16);
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[ISHA]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[ISHA], ParameterDescription::INTEGER16);
	(*parameterNames)[CALIBRATIONANALYSISDESCRIPTION[VFS]] = new ParameterDescription(CALIBRATIONANALYSISDESCRIPTION[VFS], ParameterDescription::INTEGER16);

	return parameterNames;
}




//////////////////// GETTERS / SETTERS //////////////////////////




/** @brief get human readable analysis type */
std::string CalibrationAnalysisDescription::getAnalysisType() const {
	return CommissioningAnalysisDescription::getAnalysisType( this->getType() );
}

/** @brief set: Measured calibration pulse amplitude at maximum. */
void CalibrationAnalysisDescription::setAmplitude( const float value ) {
	this->_amplitude = value;
}
/** @brief get: Measured calibration pulse amplitude at maximum. */
float CalibrationAnalysisDescription::getAmplitude() const {
	return this->_amplitude;
}
/** @brief set: Measured amplitude of the calibration pulse tail. */
void CalibrationAnalysisDescription::setTail( const float value ) {
	this->_tail = value;
}
/** @brief get: Measured amplitude of the calibration pulse tail. */
float CalibrationAnalysisDescription::getTail() const {
	return this->_tail;
}
/** @brief set: Time needed to reach the maximum. */
void CalibrationAnalysisDescription::setRiseTime( const float value ) {
	this->_riseTime = value;
}
/** @brief get: Time needed to reach the maximum. */
float CalibrationAnalysisDescription::getRiseTime() const {
	return this->_riseTime;
}
/** @brief set: Time constant of the CR-RC from the fit of the pulse. */
void CalibrationAnalysisDescription::setTimeConstant( const float value ) {
	this->_timeConstant = value;
}
/** @brief get: Time constant of the CR-RC from the fit of the pulse. */
float CalibrationAnalysisDescription::getTimeConstant() const {
	return this->_timeConstant;
}
/** @brief set: Smearing of the CR-RC from the fit of the pulse. */
void CalibrationAnalysisDescription::setSmearing( const float value ) {
	this->_smearing = value;
}
/** @brief get: Smearing of the CR-RC from the fit of the pulse. */
float CalibrationAnalysisDescription::getSmearing() const {
	return this->_smearing;
}
/** @brief set: Chi2 of the fit of the pulse. */
void CalibrationAnalysisDescription::setChi2( const float value ) {
	this->_chi2 = value;
}
/** @brief get: Chi2 of the fit of the pulse. */
float CalibrationAnalysisDescription::getChi2() const {
	return this->_chi2;
}
/** @brief set: Fit mode: deconv or not ? */
void CalibrationAnalysisDescription::setDeconvMode( const bool value ) {
	this->_deconvMode = value;
}
/** @brief get: Fit mode: deconv or not ? */
bool CalibrationAnalysisDescription::getDeconvMode() const {
	return this->_deconvMode;
}

/** @brief set: (static) calibration output mask [CDRV] */
void CalibrationAnalysisDescription::setCalChan( const uint16_t calChan ) {
	this->_calChan=calChan;
}
/** @brief get: (static) calibration output mask [CDRV] */
uint16_t CalibrationAnalysisDescription::getCalChan() const {
	return this->_calChan;
}

/** @brief set: (static) ISHA */
void CalibrationAnalysisDescription::setIsha( const uint16_t isha ) {
	this->_isha=isha;
}
/** @brief get: (static) ISHA */
uint16_t CalibrationAnalysisDescription::getIsha() const {
	return this->_isha;
}

/** @brief set: (static) VFS */
void CalibrationAnalysisDescription::setVfs( const uint16_t vfs ) {
	this->_vfs=vfs;
}
/** @brief get: (static) VFS */
uint16_t CalibrationAnalysisDescription::getVfs() const {
	return this->_vfs;
}
