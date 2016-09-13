/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/


#include "TimingAnalysisDescription.h"


const char *TimingAnalysisDescription::TIMINGANALYSISDESCRIPTION[] = {"timetmre","refTime","delay","height","base","peak","frameFindingThreshold","optimumSamplingPoint","tickMarkHeightThreshold","kind"};



////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
TimingAnalysisDescription::TimingAnalysisDescription() {
	this->setTimetmre(0.0);
	this->setRefTime(0.0);
	this->setDelay(0.0);
	this->setHeight(0.0);
	this->setBase(0.0);
	this->setPeak(0.0);
	this->setFrameFindingThreshold(0.0);
	this->setOptimumSamplingPoint(0.0);
	this->setTickMarkHeightThreshold(0.0);
	this->setKind("");
}

//------------------------------------
TimingAnalysisDescription::TimingAnalysisDescription(float timetmre,float refTime,float delay,float height,float base,float peak,float frameFindingThreshold,float optimumSamplingPoint,float tickMarkHeightThreshold,bool kind,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setTimetmre(timetmre);
	this->setRefTime(refTime);
	this->setDelay(delay);
	this->setHeight(height);
	this->setBase(base);
	this->setPeak(peak);
	this->setFrameFindingThreshold(frameFindingThreshold);
	this->setOptimumSamplingPoint(optimumSamplingPoint);
	this->setTickMarkHeightThreshold(tickMarkHeightThreshold);
	this->setKind(kind);
}

//------------------------------------
TimingAnalysisDescription::TimingAnalysisDescription(parameterDescriptionNameType parameterNames) : CommissioningAnalysisDescription ( parameterNames ) {
	setTimetmre( *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[TIMETMRE]]->getValueConverted() );
	setRefTime(  *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[REFTIME]]->getValueConverted() );
	setDelay(    *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[DELAY]]->getValueConverted() );
	setHeight(   *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[HEIGHT]]->getValueConverted() );
	setBase(     *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[BASE]]->getValueConverted() );
	setPeak(     *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[PEAK]]->getValueConverted() );
	setFrameFindingThreshold(   *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[FRAMEFINDINGTHRESHOLD]]->getValueConverted() );
	setOptimumSamplingPoint(    *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[OPTIMUMSAMPLINGPOINT]]->getValueConverted() );
	setTickMarkHeightThreshold( *(double * )parameterNames[TIMINGANALYSISDESCRIPTION[TICKMARKHEIGHTTHRESHOLD]]->getValueConverted() );
	setKind(     *(bool * )parameterNames[TIMINGANALYSISDESCRIPTION[KIND]]->getValueConverted() );
}




///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string TimingAnalysisDescription::toString() {
	std::stringstream ss;
	ss << "Key:" << std::endl;
	ss << CommissioningAnalysisDescription::_toString();
	ss << "TimingAnalysis properties:" << std::endl;
	ss << " . Timetmre                : " << this->getTimetmre()                << std::endl;
	ss << " . RefTime                 : " << this->getRefTime()                 << std::endl;
	ss << " . Delay                   : " << this->getDelay()                   << std::endl;
	ss << " . Height                  : " << this->getHeight()                  << std::endl;
	ss << " . Base                    : " << this->getBase()                    << std::endl;
	ss << " . Peak                    : " << this->getPeak()                    << std::endl;
	ss << " . FrameFindingThreshold   : " << this->getFrameFindingThreshold()   << std::endl;
	ss << " . OptimumSamplingPoint    : " << this->getOptimumSamplingPoint()    << std::endl;
	ss << " . TickMarkHeightThreshold : " << this->getTickMarkHeightThreshold() << std::endl;
	ss << " . Kind                    : " << this->getKind()                    << std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
CommissioningAnalysisDescription *TimingAnalysisDescription::clone() {
	TimingAnalysisDescription * c = new TimingAnalysisDescription(
		this->getTimetmre(), this->getRefTime(), this->getDelay(), this->getHeight(), this->getBase(), 
		this->getPeak(), this->getFrameFindingThreshold(), this->getOptimumSamplingPoint(), 
		this->getTickMarkHeightThreshold(), this->getKind(),
		this->getCrate(),this->getSlot(),this->getRing(),this->getCcuAdr(),this->getCcuChan(),
		this->getI2cAddr(),this->getPartition(),this->getRunNumber(),this->isValid(),
		this->getSerializedComments(),this->getFedId(),this->getFeUnit(),this->getFeChan(),
		this->getFedApv()
	);
	return c;
}

//------------------------------------
void TimingAnalysisDescription::display() {
	std::cout << this->toString() << std::endl;
}

//------------------------------------
parameterDescriptionNameType *TimingAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames = CommissioningAnalysisDescription::getParameterNames();
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[TIMETMRE]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[TIMETMRE], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[REFTIME]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[REFTIME], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[DELAY]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[DELAY], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[HEIGHT]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[HEIGHT], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[BASE]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[BASE], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[PEAK]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[PEAK], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[FRAMEFINDINGTHRESHOLD]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[FRAMEFINDINGTHRESHOLD], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[OPTIMUMSAMPLINGPOINT]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[OPTIMUMSAMPLINGPOINT], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[TICKMARKHEIGHTTHRESHOLD]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[TICKMARKHEIGHTTHRESHOLD], ParameterDescription::REAL);
	(*parameterNames)[TIMINGANALYSISDESCRIPTION[KIND]] = new ParameterDescription(TIMINGANALYSISDESCRIPTION[KIND], ParameterDescription::STRING);
	return parameterNames;
}




//////////////////// GETTERS / SETTERS //////////////////////////




/** @brief get human readable analysis type */
std::string TimingAnalysisDescription::getAnalysisType() const {
	return CommissioningAnalysisDescription::getAnalysisType( this->getType() );
}

/** @brief set: Time of tick mark rising edge [ns]. */
void TimingAnalysisDescription::setTimetmre( const float value ) {
	this->_timetmre = value;
}
/** @brief get: Time of tick mark rising edge [ns]. */
float TimingAnalysisDescription::getTimetmre() const {
	return this->_timetmre;
}
/** @brief set: Sampling point of reference tick mark [ns]. */
void TimingAnalysisDescription::setRefTime( const float value ) {
	this->_refTime = value;
}
/** @brief get: Sampling point of reference tick mark [ns]. */
float TimingAnalysisDescription::getRefTime() const {
	return this->_refTime;
}
/** @brief set: Delay required to sync w.r.t. reference tick mark [ns]. */
void TimingAnalysisDescription::setDelay( const float value ) {
	this->_delay = value;
}
/** @brief get: Delay required to sync w.r.t. reference tick mark [ns]. */
float TimingAnalysisDescription::getDelay() const {
	return this->_delay;
}
/** @brief set: Height of tick mark [ADC]. */
void TimingAnalysisDescription::setHeight( const float value ) {
	this->_height = value;
}
/** @brief get: Height of tick mark [ADC]. */
float TimingAnalysisDescription::getHeight() const {
	return this->_height;
}
/** @brief set: Baseline level of tick mark [ADC]. */
void TimingAnalysisDescription::setBase( const float value ) {
	this->_base = value;
}
/** @brief get: Baseline level of tick mark [ADC]. */
float TimingAnalysisDescription::getBase() const {
	return this->_base;
}
/** @brief set: Level of tick mark top [ADC]. */
void TimingAnalysisDescription::setPeak( const float value ) {
	this->_peak = value;
}
/** @brief get: Level of tick mark top [ADC]. */
float TimingAnalysisDescription::getPeak() const {
	return this->_peak;
}
/** @brief set: FED frame-finding threshold [ADC]. */
void TimingAnalysisDescription::setFrameFindingThreshold( const float value ) {
	this->_frameFindingThreshold = value;
}
/** @brief get: FED frame-finding threshold [ADC]. */
float TimingAnalysisDescription::getFrameFindingThreshold() const {
	return this->_frameFindingThreshold;
}
/** @brief set: Static Optimum sampling point, defined w.r.t. rising edge [ns]. */
void TimingAnalysisDescription::setOptimumSamplingPoint( const float value ) {
	this->_optimumSamplingPoint = value;
}
/** @brief get: Static Optimum sampling point, defined w.r.t. rising edge [ns]. */
float TimingAnalysisDescription::getOptimumSamplingPoint() const {
	return this->_optimumSamplingPoint;
}
/** @brief set: Static Threshold defining minimum tick mark height [ADC]. */
void TimingAnalysisDescription::setTickMarkHeightThreshold( const float value ) {
	this->_tickMarkHeightThreshold = value;
}
/** @brief get: Static Threshold defining minimum tick mark height [ADC]. */
float TimingAnalysisDescription::getTickMarkHeightThreshold() const {
	return this->_tickMarkHeightThreshold;
}
/** @brief set: True=APV analysis, False=FED analysis. */
void TimingAnalysisDescription::setKind( const bool value ) {
	this->_kind = value;
}
/** @brief get: True=APV analysis, False=FED analysis. */
bool TimingAnalysisDescription::getKind() const {
	return this->_kind;
}
