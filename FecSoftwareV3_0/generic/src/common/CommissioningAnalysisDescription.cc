
#include <iostream>
#include "CommissioningAnalysisDescription.h"
#include "ParameterDescription.h"


const char *CommissioningAnalysisDescription::COMMISSIONINGANALYSIS[] = {"crate","slot","ring","ccuAdr","ccuChan","i2cAddr","partition","runNumber","analysisType","valid","comments","fedId","feUnit","fechan","fedApv","dcuHardId","detId"};

const char *CommissioningAnalysisDescription::COMMISSIONINGANALYSISTYPE[] = {"UNKNOWN","APVLATENCY","CALIBRATION","FASTFEDCABLING","FINEDELAY","OPTOSCAN","PEDESTALS","TIMING","VPSPSCAN"};


////////////////////// CON(DE)STRUCTOR ////////////////////////////


//------------------------------------
CommissioningAnalysisDescription::CommissioningAnalysisDescription() {
	this->setCrate(0);
	this->setSlot(0);
	this->setRing(0);
	this->setCcuAdr(0);
	this->setCcuChan(0);
	this->setI2cAddr(0);

	this->setPartition("");
	this->setRunNumber(0);
	this->isValid(false);

	this->setFedId(0);
	this->setFeUnit(0);
	this->setFeChan(0);
	this->setFedApv(0);
	this->setDcuHardId(0);
	this->setDetId(0);
}

//------------------------------------
CommissioningAnalysisDescription::CommissioningAnalysisDescription(uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments, uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv) {
	this->setCrate(crate);
	this->setSlot(slot);
	this->setRing(ring);
	this->setCcuAdr(ccuAdr);
	this->setCcuChan(ccuChan);
	this->setI2cAddr(i2cAddr);

	this->setPartition(partition);
	this->setRunNumber(runNumber);
	this->isValid(valid);
	this->setComments(comments);

	this->setFedId(fedId);
	this->setFeUnit(feUnit);
	this->setFeChan(feChan);
	this->setFedApv(fedApv);
	this->setDcuHardId(0);
	this->setDetId(0);
}

//------------------------------------
CommissioningAnalysisDescription::CommissioningAnalysisDescription(parameterDescriptionNameType parameterNames) {
	this->setCrate(     *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[CRATE]]->getValueConverted() );
	this->setSlot(      *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[SLOT]]->getValueConverted() );
	this->setRing(      *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[RING]]->getValueConverted() );
	this->setCcuAdr(    *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[CCUADR]]->getValueConverted() );
	this->setCcuChan(   *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[CCUCHAN]]->getValueConverted() );
	this->setI2cAddr(   *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[I2CADDR]]->getValueConverted() );

	this->setPartition(  (std::string)parameterNames[COMMISSIONINGANALYSIS[PARTITION]]->getValue() );
	this->setRunNumber( *(uint32_t * )parameterNames[COMMISSIONINGANALYSIS[RUNNUMBER]]->getValueConverted() );
	this->isValid(      *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[VALID]]->getValueConverted() );
	this->setComments(   (std::string)parameterNames[COMMISSIONINGANALYSIS[COMMENTS]]->getValue() );

	this->setFedId(     *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[FEDID]]->getValueConverted() );
	this->setFeUnit(    *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[FEUNIT]]->getValueConverted() );
	this->setFeChan(    *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[FECHAN]]->getValueConverted() );
	this->setFedApv(    *(uint16_t * )parameterNames[COMMISSIONINGANALYSIS[FEDAPV]]->getValueConverted() );
	this->setDcuHardId( *(uint32_t * )parameterNames[COMMISSIONINGANALYSIS[DCUHARDID]]->getValueConverted() );
	this->setDetId(     *(uint32_t * )parameterNames[COMMISSIONINGANALYSIS[DETID]]->getValueConverted() );

	parameterNames[COMMISSIONINGANALYSIS[COMMENTS]]->setValue("");

}



///////////////////////// METHODS ///////////////////////////////



//------------------------------------
std::string CommissioningAnalysisDescription::_toString() {
	std::stringstream ss;
	ss << " . Crate     : " << this->getCrate()     << std::endl;
	ss << " . Slot      : " << this->getSlot()      << std::endl;
	ss << " . Ring      : " << this->getRing()      << std::endl;
	ss << " . CcuAdr    : " << this->getCcuAdr()    << std::endl;
	ss << " . CcuChan   : " << this->getCcuChan()   << std::endl;
	ss << " . I2cAddr   : " << this->getI2cAddr()   << std::endl;
	ss << " . Fedid     : " << this->getFedId()     << std::endl;
	ss << " . FeUnit    : " << this->getFeUnit()    << std::endl;
	ss << " . FeChan    : " << this->getFeChan()    << std::endl;
	ss << " . FedApv    : " << this->getFedApv()    << std::endl;
	ss << " . Partition : " << this->getPartition() << std::endl;
	ss << " . RunNumber : " << this->getRunNumber() << std::endl;
	ss << " . DcuHardId : " << this->getDcuHardId() << std::endl;
	ss << " . DetId     : " << this->getDetId()     << std::endl;
	ss << " . Valid     : " << this->isValid()      << std::endl;
	ss << " . Comments  : " << std::endl;
//	for (std::vector<max_string>::iterator it=this->_comments.begin(); it!=this->_comments.end(); it ++) {
	std::vector<std::string> comments = this->getComments();
	for (std::vector<std::string>::iterator it=comments.begin(); it!=comments.end(); it ++) {
		ss << " . . " << (*it) << std::endl;
	}
	ss << " . . (" << this->getSerializedComments() << ")"<< std::endl;
	return ss.rdbuf()->str();
}

//------------------------------------
parameterDescriptionNameType *CommissioningAnalysisDescription::getParameterNames() {
	parameterDescriptionNameType *parameterNames           = new parameterDescriptionNameType;
	(*parameterNames)[COMMISSIONINGANALYSIS[CRATE]]        = new ParameterDescription(COMMISSIONINGANALYSIS[CRATE],        ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[SLOT]]         = new ParameterDescription(COMMISSIONINGANALYSIS[SLOT],         ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[RING]]         = new ParameterDescription(COMMISSIONINGANALYSIS[RING],         ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[CCUADR]]       = new ParameterDescription(COMMISSIONINGANALYSIS[CCUADR],       ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[CCUCHAN]]      = new ParameterDescription(COMMISSIONINGANALYSIS[CCUCHAN],      ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[I2CADDR]]      = new ParameterDescription(COMMISSIONINGANALYSIS[I2CADDR],      ParameterDescription::INTEGER16);

	(*parameterNames)[COMMISSIONINGANALYSIS[FEDID]]        = new ParameterDescription(COMMISSIONINGANALYSIS[FEDID],        ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[FEUNIT]]       = new ParameterDescription(COMMISSIONINGANALYSIS[FEUNIT],       ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[FECHAN]]       = new ParameterDescription(COMMISSIONINGANALYSIS[FECHAN],       ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[FEDAPV]]       = new ParameterDescription(COMMISSIONINGANALYSIS[FEDAPV],       ParameterDescription::INTEGER16);

	(*parameterNames)[COMMISSIONINGANALYSIS[PARTITION]]    = new ParameterDescription(COMMISSIONINGANALYSIS[PARTITION],    ParameterDescription::STRING   );
	(*parameterNames)[COMMISSIONINGANALYSIS[RUNNUMBER]]    = new ParameterDescription(COMMISSIONINGANALYSIS[RUNNUMBER],    ParameterDescription::INTEGER32);
	(*parameterNames)[COMMISSIONINGANALYSIS[ANALYSISTYPE]] = new ParameterDescription(COMMISSIONINGANALYSIS[ANALYSISTYPE], ParameterDescription::STRING   );

	(*parameterNames)[COMMISSIONINGANALYSIS[VALID]]        = new ParameterDescription(COMMISSIONINGANALYSIS[VALID],        ParameterDescription::INTEGER16);
	(*parameterNames)[COMMISSIONINGANALYSIS[COMMENTS]]     = new ParameterDescription(COMMISSIONINGANALYSIS[COMMENTS],     ParameterDescription::STRING   );

	(*parameterNames)[COMMISSIONINGANALYSIS[DCUHARDID]]    = new ParameterDescription(COMMISSIONINGANALYSIS[DCUHARDID],    ParameterDescription::INTEGER32);
	(*parameterNames)[COMMISSIONINGANALYSIS[DETID]]        = new ParameterDescription(COMMISSIONINGANALYSIS[DETID],        ParameterDescription::INTEGER32);

	return parameterNames;
}

//------------------------------------
void CommissioningAnalysisDescription::deleteParameterNames(parameterDescriptionNameType *parameterNames) {
	for (parameterDescriptionNameType::iterator p = parameterNames->begin() ; p != parameterNames->end() ; p ++) {
		ParameterDescription *val = p->second;
		delete val;
	}
	parameterNames->clear();
}




//////////////////// GETTERS / SETTERS //////////////////////////



/** @brief set Crate */
void CommissioningAnalysisDescription::setCrate( const uint16_t value ) {
	this->_crate = value;
}
/** @brief get Crate */
uint16_t CommissioningAnalysisDescription::getCrate() const {
	return this->_crate;
}
/** @brief set Slot */
void CommissioningAnalysisDescription::setSlot( const uint16_t value ) {
	this->_slot = value;
}
/** @brief get Slot */
uint16_t CommissioningAnalysisDescription::getSlot() const {
	return this->_slot;
}
/** @brief set Ring */
void CommissioningAnalysisDescription::setRing( const uint16_t value ) {
	this->_ring = value;
}
/** @brief get Ring */
uint16_t CommissioningAnalysisDescription::getRing() const {
	return this->_ring;
}
/** @brief set CcuAdr */
void CommissioningAnalysisDescription::setCcuAdr( const uint16_t value ) {
	this->_ccuAdr = value;
}
/** @brief get CcuAdr */
uint16_t CommissioningAnalysisDescription::getCcuAdr() const {
	return this->_ccuAdr;
}
/** @brief set CcuChan */
void CommissioningAnalysisDescription::setCcuChan( const uint16_t value ) {
	this->_ccuChan = value;
}
/** @brief get CcuChan */
uint16_t CommissioningAnalysisDescription::getCcuChan() const {
	return this->_ccuChan;
}
/** @brief set I2cAddr */
void CommissioningAnalysisDescription::setI2cAddr( const uint16_t value ) {
	this->_i2cAddr = value;
}
/** @brief get I2cAddr */
uint16_t CommissioningAnalysisDescription::getI2cAddr() const {
	return this->_i2cAddr;
}

/** @brief set Partition */
void CommissioningAnalysisDescription::setPartition( const std::string value ) {
	strcpy(this->_partition,value.c_str());
}
/** @brief get Partition */
std::string CommissioningAnalysisDescription::getPartition() const {
	return this->_partition;
}
/** @brief set Run number */
void CommissioningAnalysisDescription::setRunNumber( const uint32_t value ) {
	this->_runNumber=value;
}
/** @brief get Run number */
uint32_t CommissioningAnalysisDescription::getRunNumber() const {
	return this->_runNumber;
}

/** @brief get Human understandable analysis type */
std::string CommissioningAnalysisDescription::getAnalysisType( const CommissioningAnalysisDescription::commissioningType type ) {
	return CommissioningAnalysisDescription::COMMISSIONINGANALYSISTYPE[type];
}

/** @brief get: translate a human readable analysis type to an internal code */
CommissioningAnalysisDescription::commissioningType CommissioningAnalysisDescription::getAnalysisType( const std::string type ) {
	     if ( type == "APVLATENCY"     ) return T_ANALYSIS_APVLATENCY ;
	else if ( type == "CALIBRATION"    ) return T_ANALYSIS_CALIBRATION ;
	else if ( type == "FASTFEDCABLING" ) return T_ANALYSIS_FASTFEDCABLING ;
	else if ( type == "FINEDELAY"      ) return T_ANALYSIS_FINEDELAY ;
	else if ( type == "OPTOSCAN"       ) return T_ANALYSIS_OPTOSCAN ;
	else if ( type == "PEDESTALS"      ) return T_ANALYSIS_PEDESTALS ;
	else if ( type == "TIMING"         ) return T_ANALYSIS_TIMING ;
	else if ( type == "VPSPSCAN"       ) return T_ANALYSIS_VPSPSCAN ;
	else return T_UNKNOWN ;
}

/** @brief add: comment related to encountered problems during analysis */
void CommissioningAnalysisDescription::addComments( const std::string value ) {
	this->_comments.push_back( value );
}
/** @brief get: comments related to encountered problems during analysis */
std::vector<std::string> CommissioningAnalysisDescription::getComments() const {
	return _comments;
}

/** @brief get: string of serialized comments related to encountered problems during analysis */
std::string CommissioningAnalysisDescription::getSerializedComments() const {
	std::stringstream ss;
	std::string sep("");
	//for (std::vector<max_string>::iterator it=_comments.begin(); it!=_comments.end(); it ++) {
	for (size_t i=0; i<this->_comments.size(); i++) {
		ss << sep << this->_comments.at(i);
		sep=CONCATENED_STRING_CHARACTER_SEPARATOR;
	}
	return ss.rdbuf()->str();
}

/**
 * @brief set: comments related to encountered problems during analysis. 
 * Set comments related to encountered problems during analysis. Input string is 
 * a concatenation of all registered comments with a character separator defined by 
 * CONCATENED_STRING_CHARACTER_SEPARATOR constant
 */
void CommissioningAnalysisDescription::setComments(const std::string value) {
	this->_comments.clear();
	if ( value.empty() ) return;
	std::string buf;
	for (size_t i = 0, size = value.size(); i < size; i++) {
		if ( value[i]==CONCATENED_STRING_CHARACTER_SEPARATOR ) {
			this->addComments( buf );
			buf.erase();
		} else {
			buf.append(value, i, 1);
		}
	}
	if ( buf != "" ) {
		this->addComments( buf );
	}
}

