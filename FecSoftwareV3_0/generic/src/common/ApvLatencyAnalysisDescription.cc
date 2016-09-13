/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/


#include "ApvLatencyAnalysisDescription.h"


const char *ApvLatencyAnalysisDescription::APVLATENCYANALYSISDESCRIPTION[] = {"latency"};



////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
ApvLatencyAnalysisDescription::ApvLatencyAnalysisDescription() {
	this->setLatency(0);
}

//------------------------------------
ApvLatencyAnalysisDescription::ApvLatencyAnalysisDescription(uint16_t latency,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments, uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv ) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setLatency(latency);
}

//------------------------------------
ApvLatencyAnalysisDescription::ApvLatencyAnalysisDescription(parameterDescriptionNameType parameterNames) : CommissioningAnalysisDescription ( parameterNames ) {
	uint16_t v = (*(uint16_t * )parameterNames[APVLATENCYANALYSISDESCRIPTION[LATENCY]]->getValueConverted());
	setLatency( v );
}




///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string ApvLatencyAnalysisDescription::toString() {
	std::stringstream ss;
	ss << "Key:" << std::endl;
	ss << CommissioningAnalysisDescription::_toString();
	ss << "ApvLatencyAnalysis properties:" << std::endl;
	ss << " . Latency : " << this->getLatency() << std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
CommissioningAnalysisDescription *ApvLatencyAnalysisDescription::clone() {
	ApvLatencyAnalysisDescription * c = new ApvLatencyAnalysisDescription(
		this->getLatency(),
		this->getCrate(),this->getSlot(),this->getRing(),this->getCcuAdr(),this->getCcuChan(),
		this->getI2cAddr(),this->getPartition(),this->getRunNumber(),this->isValid(),
		this->getSerializedComments(),this->getFedId(),this->getFeUnit(),this->getFeChan(),
		this->getFedApv()
	);
	return c;

}

//------------------------------------
void ApvLatencyAnalysisDescription::display() {
	std::cout << this->toString() << std::endl;
}

//------------------------------------
parameterDescriptionNameType *ApvLatencyAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames = CommissioningAnalysisDescription::getParameterNames();
	(*parameterNames)[APVLATENCYANALYSISDESCRIPTION[LATENCY]] = new ParameterDescription(APVLATENCYANALYSISDESCRIPTION[LATENCY], ParameterDescription::INTEGER16);
	return parameterNames;
}




//////////////////// GETTERS / SETTERS //////////////////////////




/** @brief get human readable analysis type */
std::string ApvLatencyAnalysisDescription::getAnalysisType() const {
	return CommissioningAnalysisDescription::getAnalysisType( this->getType() );
}

/** @brief set: APV latency setting. */
void ApvLatencyAnalysisDescription::setLatency( const uint16_t value ) {
	this->_latency = value;
}
/** @brief get: APV latency setting. */
uint16_t ApvLatencyAnalysisDescription::getLatency() const {
	return this->_latency;
}
