/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2007, Sebastien BEL - CERN GENEVA
*/


#include "OptoScanAnalysisDescription.h"


const char *OptoScanAnalysisDescription::OPTOSCANANALYSISDESCRIPTION[] = {"baseLineSlop0","baseLineSlop1","baseLineSlop2","baseLineSlop3","gain","bias0","bias1","bias2","bias3","measGain0","measGain1","measGain2","measGain3","zeroLight0","zeroLight1","zeroLight2","zeroLight3","linkNoise0","linkNoise1","linkNoise2","linkNoise3","liftOff0","liftOff1","liftOff2","liftOff3","threshold0","threshold1","threshold2","threshold3","tickHeight0","tickHeight1","tickHeight2","tickHeight3"};



////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
OptoScanAnalysisDescription::OptoScanAnalysisDescription() {
	this->setBaseLineSlop0(0);
	this->setBaseLineSlop1(0);
	this->setBaseLineSlop2(0);
	this->setBaseLineSlop3(0);
	this->setGain(0);
	this->setBias0(0);
	this->setBias1(0);
	this->setBias2(0);
	this->setBias3(0);
	this->setMeasGain0(0.0);
	this->setMeasGain1(0.0);
	this->setMeasGain2(0.0);
	this->setMeasGain3(0.0);
	this->setZeroLight0(0.0);
	this->setZeroLight1(0.0);
	this->setZeroLight2(0.0);
	this->setZeroLight3(0.0);
	this->setLinkNoise0(0.0);
	this->setLinkNoise1(0.0);
	this->setLinkNoise2(0.0);
	this->setLinkNoise3(0.0);
	this->setLiftOff0(0.0);
	this->setLiftOff1(0.0);
	this->setLiftOff2(0.0);
	this->setLiftOff3(0.0);
	this->setThreshold0(0.0);
	this->setThreshold1(0.0);
	this->setThreshold2(0.0);
	this->setThreshold3(0.0);
	this->setTickHeight0(0.0);
	this->setTickHeight1(0.0);
	this->setTickHeight2(0.0);
	this->setTickHeight3(0.0);
}

//------------------------------------
OptoScanAnalysisDescription::OptoScanAnalysisDescription(float baseLineSlop0,float baseLineSlop1,float baseLineSlop2,float baseLineSlop3,uint16_t gain,uint16_t bias0,uint16_t bias1,uint16_t bias2,uint16_t bias3,float measGain0,float measGain1,float measGain2,float measGain3,float zeroLight0,float zeroLight1,float zeroLight2,float zeroLight3,float linkNoise0,float linkNoise1,float linkNoise2,float linkNoise3,float liftOff0,float liftOff1,float liftOff2,float liftOff3,float threshold0,float threshold1,float threshold2,float threshold3,float tickHeight0,float tickHeight1,float tickHeight2,float tickHeight3,uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments,uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv) : CommissioningAnalysisDescription (crate,slot,ring,ccuAdr,ccuChan,i2cAddr,partition,runNumber,valid,comments,fedId,feUnit,feChan,fedApv) {
	this->setBaseLineSlop0(baseLineSlop0);
	this->setBaseLineSlop1(baseLineSlop1);
	this->setBaseLineSlop2(baseLineSlop2);
	this->setBaseLineSlop3(baseLineSlop3);
	this->setGain(gain);
	this->setBias0(bias0);
	this->setBias1(bias1);
	this->setBias2(bias2);
	this->setBias3(bias3);
	this->setMeasGain0(measGain0);
	this->setMeasGain1(measGain1);
	this->setMeasGain2(measGain2);
	this->setMeasGain3(measGain3);
	this->setZeroLight0(zeroLight0);
	this->setZeroLight1(zeroLight1);
	this->setZeroLight2(zeroLight2);
	this->setZeroLight3(zeroLight3);
	this->setLinkNoise0(linkNoise0);
	this->setLinkNoise1(linkNoise1);
	this->setLinkNoise2(linkNoise2);
	this->setLinkNoise3(linkNoise3);
	this->setLiftOff0(liftOff0);
	this->setLiftOff1(liftOff1);
	this->setLiftOff2(liftOff2);
	this->setLiftOff3(liftOff3);
	this->setThreshold0(threshold0);
	this->setThreshold1(threshold1);
	this->setThreshold2(threshold2);
	this->setThreshold3(threshold3);
	this->setTickHeight0(tickHeight0);
	this->setTickHeight1(tickHeight1);
	this->setTickHeight2(tickHeight2);
	this->setTickHeight3(tickHeight3);
}

//------------------------------------
OptoScanAnalysisDescription::OptoScanAnalysisDescription(parameterDescriptionNameType parameterNames) : CommissioningAnalysisDescription ( parameterNames ) {
	this->setGain( *(uint16_t * )parameterNames[OPTOSCANANALYSISDESCRIPTION[GAIN]]->getValueConverted() );
	this->setBias0( *(uint16_t * )parameterNames[OPTOSCANANALYSISDESCRIPTION[BIAS0]]->getValueConverted() );
	this->setBias1( *(uint16_t * )parameterNames[OPTOSCANANALYSISDESCRIPTION[BIAS1]]->getValueConverted() );
	this->setBias2( *(uint16_t * )parameterNames[OPTOSCANANALYSISDESCRIPTION[BIAS2]]->getValueConverted() );
	this->setBias3( *(uint16_t * )parameterNames[OPTOSCANANALYSISDESCRIPTION[BIAS3]]->getValueConverted() );
	this->setMeasGain0( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[MEASGAIN0]]->getValueConverted() );
	this->setMeasGain1( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[MEASGAIN1]]->getValueConverted() );
	this->setMeasGain2( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[MEASGAIN2]]->getValueConverted() );
	this->setMeasGain3( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[MEASGAIN3]]->getValueConverted() );
	this->setZeroLight0( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT0]]->getValueConverted() );
	this->setZeroLight1( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT1]]->getValueConverted() );
	this->setZeroLight2( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT2]]->getValueConverted() );
	this->setZeroLight3( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT3]]->getValueConverted() );
	this->setLinkNoise0( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[LINKNOISE0]]->getValueConverted() );
	this->setLinkNoise1( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[LINKNOISE1]]->getValueConverted() );
	this->setLinkNoise2( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[LINKNOISE2]]->getValueConverted() );
	this->setLinkNoise3( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[LINKNOISE3]]->getValueConverted() );
	this->setLiftOff0( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[LIFTOFF0]]->getValueConverted() );
	this->setLiftOff1( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[LIFTOFF1]]->getValueConverted() );
	this->setLiftOff2( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[LIFTOFF2]]->getValueConverted() );
	this->setLiftOff3( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[LIFTOFF3]]->getValueConverted() );
	this->setThreshold0( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[THRESHOLD0]]->getValueConverted() );
	this->setThreshold1( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[THRESHOLD1]]->getValueConverted() );
	this->setThreshold2( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[THRESHOLD2]]->getValueConverted() );
	this->setThreshold3( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[THRESHOLD3]]->getValueConverted() );
	this->setTickHeight0( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT0]]->getValueConverted() );
	this->setTickHeight1( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT1]]->getValueConverted() );
	this->setTickHeight2( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT2]]->getValueConverted() );
	this->setTickHeight3( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT3]]->getValueConverted() );
	this->setBaseLineSlop0( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[BASELINESLOP0]]->getValueConverted() );
	this->setBaseLineSlop1( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[BASELINESLOP1]]->getValueConverted() );
	this->setBaseLineSlop2( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[BASELINESLOP2]]->getValueConverted() );
	this->setBaseLineSlop3( *(double * )parameterNames[OPTOSCANANALYSISDESCRIPTION[BASELINESLOP3]]->getValueConverted() );
}





///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string OptoScanAnalysisDescription::toString() {
	std::stringstream ss;
	ss << "Key:" << std::endl;
	ss << CommissioningAnalysisDescription::_toString();
	ss << "OptoScanAnalysis properties:" << std::endl;
	ss << " . Gain         : " << this->getGain()         << std::endl;
	ss << " . Bias0        : " << this->getBias0()        << std::endl;
	ss << " . Bias1        : " << this->getBias1()        << std::endl;
	ss << " . Bias2        : " << this->getBias2()        << std::endl;
	ss << " . Bias3        : " << this->getBias3()        << std::endl;
	ss << " . MeasGain0    : " << this->getMeasGain0()    << std::endl;
	ss << " . MeasGain1    : " << this->getMeasGain1()    << std::endl;
	ss << " . MeasGain2    : " << this->getMeasGain2()    << std::endl;
	ss << " . MeasGain3    : " << this->getMeasGain3()    << std::endl;
	ss << " . ZeroLight0   : " << this->getZeroLight0()   << std::endl;
	ss << " . ZeroLight1   : " << this->getZeroLight1()   << std::endl;
	ss << " . ZeroLight2   : " << this->getZeroLight2()   << std::endl;
	ss << " . ZeroLight3   : " << this->getZeroLight3()   << std::endl;
	ss << " . LinkNoise0   : " << this->getLinkNoise0()   << std::endl;
	ss << " . LinkNoise1   : " << this->getLinkNoise1()   << std::endl;
	ss << " . LinkNoise2   : " << this->getLinkNoise2()   << std::endl;
	ss << " . LinkNoise3   : " << this->getLinkNoise3()   << std::endl;
	ss << " . LiftOff0     : " << this->getLiftOff0()     << std::endl;
	ss << " . LiftOff1     : " << this->getLiftOff1()     << std::endl;
	ss << " . LiftOff2     : " << this->getLiftOff2()     << std::endl;
	ss << " . LiftOff3     : " << this->getLiftOff3()     << std::endl;
	ss << " . Threshold0   : " << this->getThreshold0()   << std::endl;
	ss << " . Threshold1   : " << this->getThreshold1()   << std::endl;
	ss << " . Threshold2   : " << this->getThreshold2()   << std::endl;
	ss << " . Threshold3   : " << this->getThreshold3()   << std::endl;
	ss << " . TickHeight0  : " << this->getTickHeight0()  << std::endl;
	ss << " . TickHeight1  : " << this->getTickHeight1()  << std::endl;
	ss << " . TickHeight2  : " << this->getTickHeight2()  << std::endl;
	ss << " . TickHeight3  : " << this->getTickHeight3()  << std::endl;
	ss << " . Baselineslop0: " << this->getBaseLineSlop0()<< std::endl;
	ss << " . Baselineslop1: " << this->getBaseLineSlop1()<< std::endl;
	ss << " . Baselineslop2: " << this->getBaseLineSlop2()<< std::endl;
	ss << " . Baselineslop3: " << this->getBaseLineSlop3()<< std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
CommissioningAnalysisDescription *OptoScanAnalysisDescription::clone() {
	OptoScanAnalysisDescription * c = new OptoScanAnalysisDescription(
		this->getBaseLineSlop0(),this->getBaseLineSlop1(),this->getBaseLineSlop2(),this->getBaseLineSlop3(),
		this->getGain(), 
		this->getBias0(), this->getBias1(), this->getBias2(), this->getBias3(), 
		this->getMeasGain0(), this->getMeasGain1(), this->getMeasGain2(), this->getMeasGain3(), 
		this->getZeroLight0(), this->getZeroLight1(), this->getZeroLight2(), this->getZeroLight3(), 
		this->getLinkNoise0(), this->getLinkNoise1(), this->getLinkNoise2(), this->getLinkNoise3(), 
		this->getLiftOff0(), this->getLiftOff1(), this->getLiftOff2(), this->getLiftOff3(), 
		this->getThreshold0(), this->getThreshold1(), this->getThreshold2(), this->getThreshold3(), 
		this->getTickHeight0(), this->getTickHeight1(), this->getTickHeight2(), this->getTickHeight3(),
		this->getCrate(),this->getSlot(),this->getRing(),this->getCcuAdr(),this->getCcuChan(),
		this->getI2cAddr(),this->getPartition(),this->getRunNumber(),this->isValid(),
		this->getSerializedComments(),this->getFedId(),this->getFeUnit(),this->getFeChan(),
		this->getFedApv()
	);
	return c;
}

//------------------------------------
void OptoScanAnalysisDescription::display() {
	std::cout << this->toString() << std::endl;
}

//------------------------------------
parameterDescriptionNameType *OptoScanAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames = CommissioningAnalysisDescription::getParameterNames();
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[GAIN]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[GAIN], ParameterDescription::INTEGER16);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[BIAS0]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[BIAS0], ParameterDescription::INTEGER16);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[BIAS1]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[BIAS1], ParameterDescription::INTEGER16);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[BIAS2]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[BIAS2], ParameterDescription::INTEGER16);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[BIAS3]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[BIAS3], ParameterDescription::INTEGER16);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[MEASGAIN0]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[MEASGAIN0], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[MEASGAIN1]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[MEASGAIN1], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[MEASGAIN2]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[MEASGAIN2], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[MEASGAIN3]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[MEASGAIN3], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT0]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT0], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT1]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT1], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT2]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT2], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT3]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[ZEROLIGHT3], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[LINKNOISE0]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[LINKNOISE0], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[LINKNOISE1]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[LINKNOISE1], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[LINKNOISE2]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[LINKNOISE2], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[LINKNOISE3]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[LINKNOISE3], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[LIFTOFF0]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[LIFTOFF0], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[LIFTOFF1]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[LIFTOFF1], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[LIFTOFF2]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[LIFTOFF2], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[LIFTOFF3]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[LIFTOFF3], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[THRESHOLD0]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[THRESHOLD0], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[THRESHOLD1]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[THRESHOLD1], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[THRESHOLD2]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[THRESHOLD2], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[THRESHOLD3]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[THRESHOLD3], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT0]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT0], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT1]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT1], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT2]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT2], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT3]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[TICKHEIGHT3], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[BASELINESLOP0]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[BASELINESLOP0], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[BASELINESLOP1]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[BASELINESLOP1], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[BASELINESLOP2]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[BASELINESLOP2], ParameterDescription::REAL);
	(*parameterNames)[OPTOSCANANALYSISDESCRIPTION[BASELINESLOP3]] = new ParameterDescription(OPTOSCANANALYSISDESCRIPTION[BASELINESLOP3], ParameterDescription::REAL);
	return parameterNames;
}




//////////////////// GETTERS / SETTERS //////////////////////////




/** @brief get human readable analysis type */
std::string OptoScanAnalysisDescription::getAnalysisType() const {
	return CommissioningAnalysisDescription::getAnalysisType( this->getType() );
}

/** @brief set: Base line slop [ADC/I2C] */
void OptoScanAnalysisDescription::setBaseLineSlop0( const float value) {
	this->_baseLineSlop0=value;
}
/** @brief get: Base line slop [ADC/I2C] */
float OptoScanAnalysisDescription::getBaseLineSlop0() const {
	return this->_baseLineSlop0;
}

/** @brief set: Base line slop [ADC/I2C] */
void OptoScanAnalysisDescription::setBaseLineSlop1( const float value) {
	this->_baseLineSlop1=value;
}
/** @brief get: Base line slop [ADC/I2C] */
float OptoScanAnalysisDescription::getBaseLineSlop1() const {
	return this->_baseLineSlop1;
}

/** @brief set: Base line slop [ADC/I2C] */
void OptoScanAnalysisDescription::setBaseLineSlop2( const float value) {
	this->_baseLineSlop2=value;
}
/** @brief get: Base line slop [ADC/I2C] */
float OptoScanAnalysisDescription::getBaseLineSlop2() const {
	return this->_baseLineSlop2;
}

/** @brief set: Base line slop [ADC/I2C] */
void OptoScanAnalysisDescription::setBaseLineSlop3( const float value) {
	this->_baseLineSlop3=value;
}
/** @brief get: Base line slop [ADC/I2C] */
float OptoScanAnalysisDescription::getBaseLineSlop3() const {
	return this->_baseLineSlop3;
}

/** @brief set: Optimum LLD gain setting. */
void OptoScanAnalysisDescription::setGain( const uint16_t value ) {
	this->_gain = value;
}
/** @brief get: Optimum LLD gain setting. */
uint16_t OptoScanAnalysisDescription::getGain() const {
	return this->_gain;
}
/** @brief set: LLD bias value for each gain setting. */
void OptoScanAnalysisDescription::setBias0( const uint16_t value ) {
	this->_bias0 = value;
}
/** @brief get: LLD bias value for each gain setting. */
uint16_t OptoScanAnalysisDescription::getBias0() const {
	return this->_bias0;
}
/** @brief set: LLD bias value for each gain setting. */
void OptoScanAnalysisDescription::setBias1( const uint16_t value ) {
	this->_bias1 = value;
}
/** @brief get: LLD bias value for each gain setting. */
uint16_t OptoScanAnalysisDescription::getBias1() const {
	return this->_bias1;
}
/** @brief set: LLD bias value for each gain setting. */
void OptoScanAnalysisDescription::setBias2( const uint16_t value ) {
	this->_bias2 = value;
}
/** @brief get: LLD bias value for each gain setting. */
uint16_t OptoScanAnalysisDescription::getBias2() const {
	return this->_bias2;
}
/** @brief set: LLD bias value for each gain setting. */
void OptoScanAnalysisDescription::setBias3( const uint16_t value ) {
	this->_bias3 = value;
}
/** @brief get: LLD bias value for each gain setting. */
uint16_t OptoScanAnalysisDescription::getBias3() const {
	return this->_bias3;
}
/** @brief set: Measured gains for each setting [V/V]. */
void OptoScanAnalysisDescription::setMeasGain0( const float value ) {
	this->_measGain0 = value;
}
/** @brief get: Measured gains for each setting [V/V]. */
float OptoScanAnalysisDescription::getMeasGain0() const {
	return this->_measGain0;
}
/** @brief set: Measured gains for each setting [V/V]. */
void OptoScanAnalysisDescription::setMeasGain1( const float value ) {
	this->_measGain1 = value;
}
/** @brief get: Measured gains for each setting [V/V]. */
float OptoScanAnalysisDescription::getMeasGain1() const {
	return this->_measGain1;
}
/** @brief set: Measured gains for each setting [V/V]. */
void OptoScanAnalysisDescription::setMeasGain2( const float value ) {
	this->_measGain2 = value;
}
/** @brief get: Measured gains for each setting [V/V]. */
float OptoScanAnalysisDescription::getMeasGain2() const {
	return this->_measGain2;
}
/** @brief set: Measured gains for each setting [V/V]. */
void OptoScanAnalysisDescription::setMeasGain3( const float value ) {
	this->_measGain3 = value;
}
/** @brief get: Measured gains for each setting [V/V]. */
float OptoScanAnalysisDescription::getMeasGain3() const {
	return this->_measGain3;
}
/** @brief set: "Zero light" levels [ADC]. */
void OptoScanAnalysisDescription::setZeroLight0( const float value ) {
	this->_zeroLight0 = value;
}
/** @brief get: "Zero light" levels [ADC]. */
float OptoScanAnalysisDescription::getZeroLight0() const {
	return this->_zeroLight0;
}
/** @brief set: "Zero light" levels [ADC]. */
void OptoScanAnalysisDescription::setZeroLight1( const float value ) {
	this->_zeroLight1 = value;
}
/** @brief get: "Zero light" levels [ADC]. */
float OptoScanAnalysisDescription::getZeroLight1() const {
	return this->_zeroLight1;
}
/** @brief set: "Zero light" levels [ADC]. */
void OptoScanAnalysisDescription::setZeroLight2( const float value ) {
	this->_zeroLight2 = value;
}
/** @brief get: "Zero light" levels [ADC]. */
float OptoScanAnalysisDescription::getZeroLight2() const {
	return this->_zeroLight2;
}
/** @brief set: "Zero light" levels [ADC]. */
void OptoScanAnalysisDescription::setZeroLight3( const float value ) {
	this->_zeroLight3 = value;
}
/** @brief get: "Zero light" levels [ADC]. */
float OptoScanAnalysisDescription::getZeroLight3() const {
	return this->_zeroLight3;
}
/** @brief set: Noise value at zero light levels [ADC]. */
void OptoScanAnalysisDescription::setLinkNoise0( const float value ) {
	this->_linkNoise0 = value;
}
/** @brief get: Noise value at zero light levels [ADC]. */
float OptoScanAnalysisDescription::getLinkNoise0() const {
	return this->_linkNoise0;
}
/** @brief set: Noise value at zero light levels [ADC]. */
void OptoScanAnalysisDescription::setLinkNoise1( const float value ) {
	this->_linkNoise1 = value;
}
/** @brief get: Noise value at zero light levels [ADC]. */
float OptoScanAnalysisDescription::getLinkNoise1() const {
	return this->_linkNoise1;
}
/** @brief set: Noise value at zero light levels [ADC]. */
void OptoScanAnalysisDescription::setLinkNoise2( const float value ) {
	this->_linkNoise2 = value;
}
/** @brief get: Noise value at zero light levels [ADC]. */
float OptoScanAnalysisDescription::getLinkNoise2() const {
	return this->_linkNoise2;
}
/** @brief set: Noise value at zero light levels [ADC]. */
void OptoScanAnalysisDescription::setLinkNoise3( const float value ) {
	this->_linkNoise3 = value;
}
/** @brief get: Noise value at zero light levels [ADC]. */
float OptoScanAnalysisDescription::getLinkNoise3() const {
	return this->_linkNoise3;
}
/** @brief set: Baseline "lift-off" values [mA]. */
void OptoScanAnalysisDescription::setLiftOff0( const float value ) {
	this->_liftOff0 = value;
}
/** @brief get: Baseline "lift-off" values [mA]. */
float OptoScanAnalysisDescription::getLiftOff0() const {
	return this->_liftOff0;
}
/** @brief set: Baseline "lift-off" values [mA]. */
void OptoScanAnalysisDescription::setLiftOff1( const float value ) {
	this->_liftOff1 = value;
}
/** @brief get: Baseline "lift-off" values [mA]. */
float OptoScanAnalysisDescription::getLiftOff1() const {
	return this->_liftOff1;
}
/** @brief set: Baseline "lift-off" values [mA]. */
void OptoScanAnalysisDescription::setLiftOff2( const float value ) {
	this->_liftOff2 = value;
}
/** @brief get: Baseline "lift-off" values [mA]. */
float OptoScanAnalysisDescription::getLiftOff2() const {
	return this->_liftOff2;
}
/** @brief set: Baseline "lift-off" values [mA]. */
void OptoScanAnalysisDescription::setLiftOff3( const float value ) {
	this->_liftOff3 = value;
}
/** @brief get: Baseline "lift-off" values [mA]. */
float OptoScanAnalysisDescription::getLiftOff3() const {
	return this->_liftOff3;
}
/** @brief set: Laser thresholds [mA]. */
void OptoScanAnalysisDescription::setThreshold0( const float value ) {
	this->_threshold0 = value;
}
/** @brief get: Laser thresholds [mA]. */
float OptoScanAnalysisDescription::getThreshold0() const {
	return this->_threshold0;
}
/** @brief set: Laser thresholds [mA]. */
void OptoScanAnalysisDescription::setThreshold1( const float value ) {
	this->_threshold1 = value;
}
/** @brief get: Laser thresholds [mA]. */
float OptoScanAnalysisDescription::getThreshold1() const {
	return this->_threshold1;
}
/** @brief set: Laser thresholds [mA]. */
void OptoScanAnalysisDescription::setThreshold2( const float value ) {
	this->_threshold2 = value;
}
/** @brief get: Laser thresholds [mA]. */
float OptoScanAnalysisDescription::getThreshold2() const {
	return this->_threshold2;
}
/** @brief set: Laser thresholds [mA]. */
void OptoScanAnalysisDescription::setThreshold3( const float value ) {
	this->_threshold3 = value;
}
/** @brief get: Laser thresholds [mA]. */
float OptoScanAnalysisDescription::getThreshold3() const {
	return this->_threshold3;
}
/** @brief set: Tick mark heights [ADC]. */
void OptoScanAnalysisDescription::setTickHeight0( const float value ) {
	this->_tickHeight0 = value;
}
/** @brief get: Tick mark heights [ADC]. */
float OptoScanAnalysisDescription::getTickHeight0() const {
	return this->_tickHeight0;
}
/** @brief set: Tick mark heights [ADC]. */
void OptoScanAnalysisDescription::setTickHeight1( const float value ) {
	this->_tickHeight1 = value;
}
/** @brief get: Tick mark heights [ADC]. */
float OptoScanAnalysisDescription::getTickHeight1() const {
	return this->_tickHeight1;
}
/** @brief set: Tick mark heights [ADC]. */
void OptoScanAnalysisDescription::setTickHeight2( const float value ) {
	this->_tickHeight2 = value;
}
/** @brief get: Tick mark heights [ADC]. */
float OptoScanAnalysisDescription::getTickHeight2() const {
	return this->_tickHeight2;
}
/** @brief set: Tick mark heights [ADC]. */
void OptoScanAnalysisDescription::setTickHeight3( const float value ) {
	this->_tickHeight3 = value;
}
/** @brief get: Tick mark heights [ADC]. */
float OptoScanAnalysisDescription::getTickHeight3() const {
	return this->_tickHeight3;
}
