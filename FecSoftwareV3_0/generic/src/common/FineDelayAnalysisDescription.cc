/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/


#include "FineDelayAnalysisDescription.h"


const char *FineDelayAnalysisDescription::FINEDELAYANALYSISDESCRIPTION[] = {"maximum","error"};



////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
FineDelayAnalysisDescription::FineDelayAnalysisDescription() {
	this->setMaximum(0.0);
	this->setError(0.0);
}

//------------------------------------
FineDelayAnalysisDescription::FineDelayAnalysisDescription(float maximum,float error,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setMaximum(maximum);
	this->setError(error);
}

//------------------------------------
FineDelayAnalysisDescription::FineDelayAnalysisDescription(parameterDescriptionNameType parameterNames) : CommissioningAnalysisDescription ( parameterNames ) {
	setMaximum( *(double * )parameterNames[FINEDELAYANALYSISDESCRIPTION[MAXIMUM]]->getValueConverted() );
	setError(   *(double * )parameterNames[FINEDELAYANALYSISDESCRIPTION[ERROR]]->getValueConverted() );
}





///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string FineDelayAnalysisDescription::toString() {
	std::stringstream ss;
	ss << "Key:" << std::endl;
	ss << CommissioningAnalysisDescription::_toString();
	ss << "FineDelayAnalysis properties:" << std::endl;
	ss << " . Maximum : " << this->getMaximum() << std::endl;
	ss << " . Error   : " << this->getError()   << std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
CommissioningAnalysisDescription *FineDelayAnalysisDescription::clone() {
	FineDelayAnalysisDescription * c = new FineDelayAnalysisDescription(
		this->getMaximum(), this->getError(),
		this->getCrate(),this->getSlot(),this->getRing(),this->getCcuAdr(),this->getCcuChan(),
		this->getI2cAddr(),this->getPartition(),this->getRunNumber(),this->isValid(),
		this->getSerializedComments(),this->getFedId(),this->getFeUnit(),this->getFeChan(),
		this->getFedApv()
	);
	return c;
}

//------------------------------------
void FineDelayAnalysisDescription::display() {
	std::cout << this->toString() << std::endl;
}

//------------------------------------
parameterDescriptionNameType *FineDelayAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames = CommissioningAnalysisDescription::getParameterNames();
	(*parameterNames)[FINEDELAYANALYSISDESCRIPTION[MAXIMUM]] = new ParameterDescription(FINEDELAYANALYSISDESCRIPTION[MAXIMUM], ParameterDescription::REAL);
	(*parameterNames)[FINEDELAYANALYSISDESCRIPTION[ERROR]] = new ParameterDescription(FINEDELAYANALYSISDESCRIPTION[ERROR], ParameterDescription::REAL);
	return parameterNames;
}




//////////////////// GETTERS / SETTERS //////////////////////////




/** @brief get human readable analysis type */
std::string FineDelayAnalysisDescription::getAnalysisType() const {
	return CommissioningAnalysisDescription::getAnalysisType( this->getType() );
}

/** @brief set: Delay corresponding to the maximum of the pulse shape. */
void FineDelayAnalysisDescription::setMaximum( const float value ) {
	this->_maximum = value;
}
/** @brief get: Delay corresponding to the maximum of the pulse shape. */
float FineDelayAnalysisDescription::getMaximum() const {
	return this->_maximum;
}
/** @brief set: Error on the position from the fit. */
void FineDelayAnalysisDescription::setError( const float value ) {
	this->_error = value;
}
/** @brief get: Error on the position from the fit. */
float FineDelayAnalysisDescription::getError() const {
	return this->_error;
}
