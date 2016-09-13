/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/


#include "FastFedCablingAnalysisDescription.h"


const char *FastFedCablingAnalysisDescription::FASTFEDCABLINGANALYSISDESCRIPTION[] = {"threshold","dirtyThreshold","isDirty","highLevel","highRms","lowLevel","lowRms","maxLl","minLl","dcuId","lldCh"};



////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
FastFedCablingAnalysisDescription::FastFedCablingAnalysisDescription() {
	this->setHighLevel(0.0);
	this->setHighRms(0.0);
	this->setLowLevel(0.0);
	this->setLowRms(0.0);
	this->setMaxll(0.0);
	this->setMinll(0.0);
	this->setDcuId(0);
	this->setLldCh(0);
	this->isDirty(0);
	this->setThreshold(0);
	this->setDirtyThreshold(0);
}

//------------------------------------
FastFedCablingAnalysisDescription::FastFedCablingAnalysisDescription(float highLevel,float highRms,float lowLevel,float lowRms,float maxll,float minll,uint32_t dcuId,uint16_t lldCh,bool isDirty,float threshold,float dirtyThreshold,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setHighLevel(highLevel);
	this->setHighRms(highRms);
	this->setLowLevel(lowLevel);
	this->setLowRms(lowRms);
	this->setMaxll(maxll);
	this->setMinll(minll);
	this->setDcuId(dcuId);
	this->setLldCh(lldCh);
	this->isDirty(isDirty);
	this->setThreshold(threshold);
	this->setDirtyThreshold(dirtyThreshold);
}

//------------------------------------
FastFedCablingAnalysisDescription::FastFedCablingAnalysisDescription(parameterDescriptionNameType parameterNames) : CommissioningAnalysisDescription ( parameterNames ) {
	setHighLevel( *(double * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[HIGHLEVEL]]->getValueConverted() );
	setHighRms( *(double * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[HIGHRMS]]->getValueConverted() );
	setLowLevel( *(double * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[LOWLEVEL]]->getValueConverted() );
	setLowRms( *(double * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[LOWRMS]]->getValueConverted() );
	setMaxll( *(double * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[MAXLL]]->getValueConverted() );
	setMinll( *(double * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[MINLL]]->getValueConverted() );
	setDcuId( *(uint32_t * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[DCUID]]->getValueConverted() );
	setLldCh( *(uint16_t * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[LLDCH]]->getValueConverted() );
	isDirty( *(uint16_t * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[ISDIRTY]]->getValueConverted() );
	setThreshold( *(double * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[THRESHOLD]]->getValueConverted() );
	setDirtyThreshold( *(double * )parameterNames[FASTFEDCABLINGANALYSISDESCRIPTION[DIRTYTHRESHOLD]]->getValueConverted() );
}





///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string FastFedCablingAnalysisDescription::toString() {
	std::stringstream ss;
	ss << "Key:" << std::endl;
	ss << CommissioningAnalysisDescription::_toString();
	ss << "FastFedCablingAnalysis properties:" << std::endl;
	ss << " . HighLevel     : " << this->getHighLevel()      << std::endl;
	ss << " . HighRms       : " << this->getHighRms()        << std::endl;
	ss << " . LowLevel      : " << this->getLowLevel()       << std::endl;
	ss << " . LowRms        : " << this->getLowRms()         << std::endl;
	ss << " . Maxll         : " << this->getMaxll()          << std::endl;
	ss << " . Minll         : " << this->getMinll()          << std::endl;
	ss << " . DcuId         : " << this->getDcuId()          << std::endl;
	ss << " . LldCh         : " << this->getLldCh()          << std::endl;
	ss << " . IsDirty       : " << this->isDirty()           << std::endl;
	ss << " . GetThreshold  : " << this->getThreshold()      << std::endl;
	ss << " . DirtyThreshold: " << this->getDirtyThreshold() << std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
CommissioningAnalysisDescription *FastFedCablingAnalysisDescription::clone() {
	FastFedCablingAnalysisDescription * c = new FastFedCablingAnalysisDescription(
		this->getHighLevel(),this->getHighRms(),this->getLowLevel(),
		this->getLowRms(),this->getMaxll(),this->getMinll(),
		this->getDcuId(),this->getLldCh(),this->isDirty(),
		this->getThreshold(),this->getDirtyThreshold(),
		this->getCrate(),this->getSlot(),this->getRing(),this->getCcuAdr(),this->getCcuChan(),
		this->getI2cAddr(),this->getPartition(),this->getRunNumber(),this->isValid(),
		this->getSerializedComments(),this->getFedId(),this->getFeUnit(),this->getFeChan(),
		this->getFedApv()
	);
	return c;
}

//------------------------------------
void FastFedCablingAnalysisDescription::display() {
	std::cout << this->toString() << std::endl;
}

//------------------------------------
parameterDescriptionNameType *FastFedCablingAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames = CommissioningAnalysisDescription::getParameterNames();
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[HIGHLEVEL]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[HIGHLEVEL], ParameterDescription::REAL);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[HIGHRMS]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[HIGHRMS], ParameterDescription::REAL);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[LOWLEVEL]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[LOWLEVEL], ParameterDescription::REAL);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[LOWRMS]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[LOWRMS], ParameterDescription::REAL);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[MAXLL]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[MAXLL], ParameterDescription::REAL);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[MINLL]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[MINLL], ParameterDescription::REAL);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[DCUID]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[DCUID], ParameterDescription::INTEGER32);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[LLDCH]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[LLDCH], ParameterDescription::INTEGER16);
	
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[ISDIRTY]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[ISDIRTY], ParameterDescription::INTEGER16);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[THRESHOLD]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[THRESHOLD], ParameterDescription::REAL);
	(*parameterNames)[FASTFEDCABLINGANALYSISDESCRIPTION[DIRTYTHRESHOLD]] = new ParameterDescription(FASTFEDCABLINGANALYSISDESCRIPTION[DIRTYTHRESHOLD], ParameterDescription::REAL);

	return parameterNames;
}




//////////////////// GETTERS / SETTERS //////////////////////////




/** @brief get human readable analysis type */
std::string FastFedCablingAnalysisDescription::getAnalysisType() const {
	return CommissioningAnalysisDescription::getAnalysisType( this->getType() );
}


/** @brief set: High light level [ADC]. */
void FastFedCablingAnalysisDescription::setHighLevel( const float value ) {
	this->_highLevel = value;
}
/** @brief get: High light level [ADC]. */
float FastFedCablingAnalysisDescription::getHighLevel() const {
	return this->_highLevel;
}
/** @brief set: Spread in high ligh level [ADC]. */
void FastFedCablingAnalysisDescription::setHighRms( const float value ) {
	this->_highRms = value;
}
/** @brief get: Spread in high ligh level [ADC]. */
float FastFedCablingAnalysisDescription::getHighRms() const {
	return this->_highRms;
}
/** @brief set: Low light level [ADC]. */
void FastFedCablingAnalysisDescription::setLowLevel( const float value ) {
	this->_lowLevel = value;
}
/** @brief get: Low light level [ADC]. */
float FastFedCablingAnalysisDescription::getLowLevel() const {
	return this->_lowLevel;
}
/** @brief set: Spread in low ligh level [ADC]. */
void FastFedCablingAnalysisDescription::setLowRms( const float value ) {
	this->_lowRms = value;
}
/** @brief get: Spread in low ligh level [ADC]. */
float FastFedCablingAnalysisDescription::getLowRms() const {
	return this->_lowRms;
}
/** @brief set: Maximum light level in data [ADC]. */
void FastFedCablingAnalysisDescription::setMaxll( const float value ) {
	this->_maxll = value;
}
/** @brief get: Maximum light level in data [ADC]. */
float FastFedCablingAnalysisDescription::getMaxll() const {
	return this->_maxll;
}
/** @brief set: Minimum light level in data [ADC]. */
void FastFedCablingAnalysisDescription::setMinll( const float value ) {
	this->_minll = value;
}
/** @brief get: Minimum light level in data [ADC]. */
float FastFedCablingAnalysisDescription::getMinll() const {
	return this->_minll;
}
/** @brief set: DCU hardware id 32-bits. */
void FastFedCablingAnalysisDescription::setDcuId( const uint32_t value ) {
	this->_dcuId = value;
}
/** @brief get: DCU hardware id 32-bits. */
uint32_t FastFedCablingAnalysisDescription::getDcuId() const {
	return this->_dcuId;
}
/** @brief set: Extracted LLD channel. */
void FastFedCablingAnalysisDescription::setLldCh( const uint16_t value ) {
	this->_lldCh = value;
}
/** @brief get: Extracted LLD channel. */
uint16_t FastFedCablingAnalysisDescription::getLldCh() const {
	return this->_lldCh;
}
