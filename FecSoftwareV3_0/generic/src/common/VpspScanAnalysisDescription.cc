/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/


#include "VpspScanAnalysisDescription.h"


const char *VpspScanAnalysisDescription::VPSPSCANANALYSISDESCRIPTION[] = {"vpsp","adcLevel","fraction","topEdge","bottomEdge","topLevel","bottomLevel"};



////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
VpspScanAnalysisDescription::VpspScanAnalysisDescription() {
	this->setVpsp(0);
	this->setAdcLevel(0);
	this->setFraction(0);
	this->setTopEdge(0);
	this->setBottomEdge(0);
	this->setTopLevel(0);
	this->setBottomLevel(0);
}

//------------------------------------
VpspScanAnalysisDescription::VpspScanAnalysisDescription(uint16_t vpsp,uint16_t adcLevel,uint16_t fraction,uint16_t topEdge,uint16_t bottomEdge,uint16_t topLevel,uint16_t bottomLevel,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setVpsp(vpsp);
	this->setAdcLevel(adcLevel);
	this->setFraction(fraction);
	this->setTopEdge(topEdge);
	this->setBottomEdge(bottomEdge);
	this->setTopLevel(topLevel);
	this->setBottomLevel(bottomLevel);
}

//------------------------------------
VpspScanAnalysisDescription::VpspScanAnalysisDescription(parameterDescriptionNameType parameterNames) : CommissioningAnalysisDescription ( parameterNames ) {
	setVpsp( *(uint16_t * )parameterNames[VPSPSCANANALYSISDESCRIPTION[VPSP]]->getValueConverted() );
	setAdcLevel( *(uint16_t * )parameterNames[VPSPSCANANALYSISDESCRIPTION[ADCLEVEL]]->getValueConverted() );
	setFraction( *(uint16_t * )parameterNames[VPSPSCANANALYSISDESCRIPTION[FRACTION]]->getValueConverted() );
	setTopEdge( *(uint16_t * )parameterNames[VPSPSCANANALYSISDESCRIPTION[TOPEDGE]]->getValueConverted() );
	setBottomEdge( *(uint16_t * )parameterNames[VPSPSCANANALYSISDESCRIPTION[BOTTOMEDGE]]->getValueConverted() );
	setTopLevel( *(uint16_t * )parameterNames[VPSPSCANANALYSISDESCRIPTION[TOPLEVEL]]->getValueConverted() );
	setBottomLevel( *(uint16_t * )parameterNames[VPSPSCANANALYSISDESCRIPTION[BOTTOMLEVEL]]->getValueConverted() );
}




///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string VpspScanAnalysisDescription::toString() {
	std::stringstream ss;
	ss << "Key:" << std::endl;
	ss << CommissioningAnalysisDescription::_toString();
	ss << "VpspScanAnalysis properties:" << std::endl;
	ss << " . Vpsp        : " << this->getVpsp()        << std::endl;
	ss << " . AdcLevel    : " << this->getAdcLevel()    << std::endl;
	ss << " . Fraction    : " << this->getFraction()    << std::endl;
	ss << " . TopEdge     : " << this->getTopEdge()     << std::endl;
	ss << " . BottomEdge  : " << this->getBottomEdge()  << std::endl;
	ss << " . TopLevel    : " << this->getTopLevel()    << std::endl;
	ss << " . BottomLevel : " << this->getBottomLevel() << std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
CommissioningAnalysisDescription *VpspScanAnalysisDescription::clone() {
	VpspScanAnalysisDescription * c = new VpspScanAnalysisDescription(
		this->getVpsp(), this->getAdcLevel(), this->getFraction(), this->getTopEdge(), 
		this->getBottomEdge(), this->getTopLevel(), this->getBottomLevel(),
		this->getCrate(),this->getSlot(),this->getRing(),this->getCcuAdr(),this->getCcuChan(),
		this->getI2cAddr(),this->getPartition(),this->getRunNumber(),this->isValid(),
		this->getSerializedComments(),this->getFedId(),this->getFeUnit(),this->getFeChan(),
		this->getFedApv()
	);
	return c;
}

//------------------------------------
void VpspScanAnalysisDescription::display() {
	std::cout << this->toString() << std::endl;
}

//------------------------------------
parameterDescriptionNameType *VpspScanAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames = CommissioningAnalysisDescription::getParameterNames();
	(*parameterNames)[VPSPSCANANALYSISDESCRIPTION[VPSP]] = new ParameterDescription(VPSPSCANANALYSISDESCRIPTION[VPSP], ParameterDescription::INTEGER16);
	(*parameterNames)[VPSPSCANANALYSISDESCRIPTION[ADCLEVEL]] = new ParameterDescription(VPSPSCANANALYSISDESCRIPTION[ADCLEVEL], ParameterDescription::INTEGER16);
	(*parameterNames)[VPSPSCANANALYSISDESCRIPTION[FRACTION]] = new ParameterDescription(VPSPSCANANALYSISDESCRIPTION[FRACTION], ParameterDescription::INTEGER16);
	(*parameterNames)[VPSPSCANANALYSISDESCRIPTION[TOPEDGE]] = new ParameterDescription(VPSPSCANANALYSISDESCRIPTION[TOPEDGE], ParameterDescription::INTEGER16);
	(*parameterNames)[VPSPSCANANALYSISDESCRIPTION[BOTTOMEDGE]] = new ParameterDescription(VPSPSCANANALYSISDESCRIPTION[BOTTOMEDGE], ParameterDescription::INTEGER16);
	(*parameterNames)[VPSPSCANANALYSISDESCRIPTION[TOPLEVEL]] = new ParameterDescription(VPSPSCANANALYSISDESCRIPTION[TOPLEVEL], ParameterDescription::INTEGER16);
	(*parameterNames)[VPSPSCANANALYSISDESCRIPTION[BOTTOMLEVEL]] = new ParameterDescription(VPSPSCANANALYSISDESCRIPTION[BOTTOMLEVEL], ParameterDescription::INTEGER16);
	return parameterNames;
}




//////////////////// GETTERS / SETTERS //////////////////////////




/** @brief get human readable analysis type */
std::string VpspScanAnalysisDescription::getAnalysisType() const {
	return CommissioningAnalysisDescription::getAnalysisType( this->getType() );
}

/** @brief set: VPSP settings for one APV. */
void VpspScanAnalysisDescription::setVpsp( const uint16_t value ) {
	this->_vpsp = value;
}
/** @brief get: VPSP settings for one APV. */
uint16_t VpspScanAnalysisDescription::getVpsp() const {
	return this->_vpsp;
}
/** @brief set: Signal levels [ADC] for VPSP settings. */
void VpspScanAnalysisDescription::setAdcLevel( const uint16_t value ) {
	this->_adcLevel = value;
}
/** @brief get: Signal levels [ADC] for VPSP settings. */
uint16_t VpspScanAnalysisDescription::getAdcLevel() const {
	return this->_adcLevel;
}
/** @brief set: Not used. */
void VpspScanAnalysisDescription::setFraction( const uint16_t value ) {
	this->_fraction = value;
}
/** @brief get: Not used. */
uint16_t VpspScanAnalysisDescription::getFraction() const {
	return this->_fraction;
}
/** @brief set: VPSP setting where baseline leaves D1 level. */
void VpspScanAnalysisDescription::setTopEdge( const uint16_t value ) {
	this->_topEdge = value;
}
/** @brief get: VPSP setting where baseline leaves D1 level. */
uint16_t VpspScanAnalysisDescription::getTopEdge() const {
	return this->_topEdge;
}
/** @brief set: VPSP setting where baseline leaves D0 level. */
void VpspScanAnalysisDescription::setBottomEdge( const uint16_t value ) {
	this->_bottomEdge = value;
}
/** @brief get: VPSP setting where baseline leaves D0 level. */
uint16_t VpspScanAnalysisDescription::getBottomEdge() const {
	return this->_bottomEdge;
}
/** @brief set: Signal level [ADC] for "digital one". */
void VpspScanAnalysisDescription::setTopLevel( const uint16_t value ) {
	this->_topLevel = value;
}
/** @brief get: Signal level [ADC] for "digital one". */
uint16_t VpspScanAnalysisDescription::getTopLevel() const {
	return this->_topLevel;
}
/** @brief set: Signal level [ADC] for "digital zero". */
void VpspScanAnalysisDescription::setBottomLevel( const uint16_t value ) {
	this->_bottomLevel = value;
}
/** @brief get: Signal level [ADC] for "digital zero". */
uint16_t VpspScanAnalysisDescription::getBottomLevel() const {
	return this->_bottomLevel;
}
