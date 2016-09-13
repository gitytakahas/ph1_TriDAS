/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
//#define DEBUGMSGERROR

#include <string>
#include "TkDcuConversionFactors.h"

/** Constructor
 */
TkDcuConversionFactors::TkDcuConversionFactors ( tscType32 dcuHardId, std::string subDetector, std::string dcuType ):
  dcuHardId_(dcuHardId),
  subDetector_(subDetector),
  dcuType_(dcuType),
  detId_(0),
  adcGain0_(0),
  adcOffset0_(0),
  adcCal0_(true),
  adcInl0_(0),
  adcInl0OW_(true),
  i20_(0),
  i10_(0),
  iCal_(true),
  kDiv_(0),
  kDivCal_(true),
  tsGain_(0),
  tsOffset_(0),
  tsCal_(true),
  r68_(0),
  r68Cal_(true),
  adcGain2_(0),
  adcOffset2_(0),
  adcCal2_(true),
  adcGain3_(0),
  adcOffset3_(0),
  adcCal3_(true),
  dcuDescriptionDcuHardId_(0),
  dcuDescriptionTimeStamp_(0) {

  for (unsigned int i = 0 ; i < MAXDCUCHANNELS ; i ++) dcuDescriptionChannels_[i] = 0 ;
    
}
  
/** Constructor for a DCU on CCU
 */
  
/** Constructor for a DCU on FEH
 */

/** Constructor with the parameter definition extracted from the database or XML file
 */
TkDcuConversionFactors::TkDcuConversionFactors ( parameterDescriptionNameType parameterNames ):
  dcuHardId_(0),
  subDetector_("NONE"),
  dcuType_("NONE"),
  detId_(0),
  adcGain0_(0),
  adcOffset0_(0),
  adcCal0_(true),
  adcInl0_(0),
  adcInl0OW_(true),
  i20_(0),
  i10_(0),
  iCal_(true),
  kDiv_(0),
  kDivCal_(true),
  tsGain_(0),
  tsOffset_(0),
  tsCal_(true),
  r68_(0),
  r68Cal_(true),
  adcGain2_(0),
  adcOffset2_(0),
  adcCal2_(true),
  adcGain3_(0),
  adcOffset3_(0),
  adcCal3_(true),
  dcuDescriptionDcuHardId_(0),
  dcuDescriptionTimeStamp_(0) {

  for (unsigned int i = 0 ; i < MAXDCUCHANNELS ; i ++) dcuDescriptionChannels_[i] = 0 ;

  setDcuHardId (*((tscType32 *) parameterNames[DCUFACTORPARAMETERNAMES[DCUCONVERTHARDID]]->getValueConverted())) ;
  setSubDetector((std::string)parameterNames[DCUFACTORPARAMETERNAMES[SUBDETECTOR]]->getValue()) ;
  setDcuType((std::string)parameterNames[DCUFACTORPARAMETERNAMES[DCUTYPE]]->getValue()) ;
  setStructureId((std::string)parameterNames[DCUFACTORPARAMETERNAMES[STRUCTUREID]]->getValue()) ;
  setContainerId((std::string)parameterNames[DCUFACTORPARAMETERNAMES[CONTAINERID]]->getValue()) ;
  //setDetId((std::string)parameterNames[DCUFACTORPARAMETERNAMES[DETID]]->getValue()) ;
  setDetId(*((unsigned int *) parameterNames[DCUFACTORPARAMETERNAMES[DETID]]->getValueConverted())) ;

  if (parameterNames[DCUFACTORPARAMETERNAMES[ADCGAIN0]]->getValue() == "") setAdcGain0(0.0) ;
  else setAdcGain0(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[ADCGAIN0]]->getValueConverted())) ;

  if (parameterNames[DCUFACTORPARAMETERNAMES[ADCOFFSET0]]->getValue() == "") setAdcOffset0(0.0) ;
  else setAdcOffset0(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[ADCOFFSET0]]->getValueConverted())) ;

  if (parameterNames[DCUFACTORPARAMETERNAMES[ADCINL0]]->getValue() == "") setAdcInl0(0.0) ;
  else setAdcInl0(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[ADCINL0]]->getValueConverted())) ;

  if (((std::string)parameterNames[DCUFACTORPARAMETERNAMES[ADCINL0OW]]->getValue()) == STRTRUE) setAdcInl0OW ( true );
  else setAdcInl0OW ( false );
  if (((std::string)parameterNames[DCUFACTORPARAMETERNAMES[ADCCAL0]]->getValue()) == STRTRUE) setAdcCal0 ( true );
  else setAdcCal0 ( false );
    
  if (parameterNames[DCUFACTORPARAMETERNAMES[I20]]->getValue() == "") setI20(0.0) ;
  else setI20(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[I20]]->getValueConverted())) ;

  if (parameterNames[DCUFACTORPARAMETERNAMES[I10]]->getValue() == "") setI10(0.0) ;
  else setI10(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[I10]]->getValueConverted())) ;

  if (((std::string)parameterNames[DCUFACTORPARAMETERNAMES[IDCUCAL]]->getValue()) == STRTRUE) setICal ( true );
  else setICal ( false );

  if (parameterNames[DCUFACTORPARAMETERNAMES[KDIV]]->getValue() == "") setKDiv(0.0) ;
  else setKDiv(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[KDIV]]->getValueConverted())) ;

  if (((std::string)parameterNames[DCUFACTORPARAMETERNAMES[KDIVCAL]]->getValue()) == STRTRUE) setKDivCal ( true );
  else setKDivCal ( false );

  if (parameterNames[DCUFACTORPARAMETERNAMES[TSGAIN]]->getValue() == "") setTsGain(0.0) ;
  else setTsGain(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[TSGAIN]]->getValueConverted())) ;

  if (parameterNames[DCUFACTORPARAMETERNAMES[TSOFFSET]]->getValue() == "") setTsOffset(0.0) ;
  else setTsOffset(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[TSOFFSET]]->getValueConverted())) ;

  if (((std::string)parameterNames[DCUFACTORPARAMETERNAMES[TSCAL]]->getValue()) == STRTRUE) setTsCal ( true );
  else setTsCal ( false );

  if (parameterNames[DCUFACTORPARAMETERNAMES[R68]]->getValue() == "") setR68(0.0) ;
  else setR68(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[R68]]->getValueConverted())) ;

  if (((std::string)parameterNames[DCUFACTORPARAMETERNAMES[R68CAL]]->getValue()) == STRTRUE) setR68Cal ( true );
  else setR68Cal ( false );

  if (parameterNames[DCUFACTORPARAMETERNAMES[ADCGAIN2]]->getValue() == "") setAdcGain2(0.0) ;
  else setAdcGain2(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[ADCGAIN2]]->getValueConverted())) ;

  if (parameterNames[DCUFACTORPARAMETERNAMES[ADCOFFSET2]]->getValue() == "") setAdcOffset2(0.0) ;
  else setAdcOffset2(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[ADCOFFSET2]]->getValueConverted())) ;

  if (((std::string)parameterNames[DCUFACTORPARAMETERNAMES[ADCCAL2]]->getValue()) == STRTRUE) setAdcCal2 ( true );
  else setAdcCal2 ( false );

  if (parameterNames[DCUFACTORPARAMETERNAMES[ADCGAIN3]]->getValue() == "") setAdcGain3(0.0) ;
  else setAdcGain3(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[ADCGAIN3]]->getValueConverted())) ;

  if (parameterNames[DCUFACTORPARAMETERNAMES[ADCOFFSET3]]->getValue() == "") setAdcOffset3(0.0) ;
  else setAdcOffset3(*((double *) parameterNames[DCUFACTORPARAMETERNAMES[ADCOFFSET3]]->getValueConverted())) ;

  if (((std::string)parameterNames[DCUFACTORPARAMETERNAMES[ADCCAL3]]->getValue()) == STRTRUE) setAdcCal3 ( true );
  else setAdcCal3 ( false );
}

TkDcuConversionFactors::~TkDcuConversionFactors ( ) {

  //delete dcuDescription_ ;
}

/** Set the description to be converted
 */
void TkDcuConversionFactors::setDcuDescription ( dcuDescription *dcuDes ) {

  //dcuDescription_ = dcuDes.clone() ;
  dcuDescriptionDcuHardId_ = dcuDes->getDcuHardId() ;
  for (unsigned int i = 0 ; i < MAXDCUCHANNELS ; i ++) {
    dcuDescriptionChannels_[i] = dcuDes->getDcuChannel(i) ;
  }
  dcuDescriptionTimeStamp_ = dcuDes->getTimeStamp() ;
}
    
/** Set the DCU hard ID
 */
void TkDcuConversionFactors::setDcuHardId ( tscType32 dcuHardId ) {

  dcuHardId_ = dcuHardId ;
}

/** Get the DCU hard ID
 */
tscType32 TkDcuConversionFactors::getDcuHardId ( ) {

  return (dcuHardId_) ;
}

/** Set the sub detectors
 */
void TkDcuConversionFactors::setSubDetector ( std::string subDetector ) {

  subDetector_ = subDetector ;
}

/** Get the sub detectors
 */
std::string TkDcuConversionFactors::getSubDetector ( ) {

  return subDetector_ ;
}

/** Set the DCU type
 */
void TkDcuConversionFactors::setDcuType ( std::string dcuType ) {

  dcuType_ = dcuType ;
}

/** Get the DCU type
 */
std::string TkDcuConversionFactors::getDcuType ( ) {

  return dcuType_ ;
}

/** Set the structure ID
 */
void TkDcuConversionFactors::setStructureId ( std::string structureId ) {

  structureId_ = structureId ;
}

/** Get the structure ID
 */
std::string TkDcuConversionFactors::getStructureId ( ) {

  return structureId_ ;
}

/** Set the module ID
 */
void TkDcuConversionFactors::setContainerId ( std::string containerId ) {

  containerId_ = containerId ;
}

/** Get the Container ID
 */
std::string TkDcuConversionFactors::getContainerId ( ) {

  return containerId_ ;
}

/** Set the detector ID
 */
void TkDcuConversionFactors::setDetId ( unsigned int detId ) {

  detId_ = detId ;
}

/** Get the detector ID
 */
unsigned int TkDcuConversionFactors::getDetId ( ) {

  return detId_ ;
}

/** Set 
 */
void TkDcuConversionFactors::setAdcGain0 ( double adcGain0 ) {

  adcGain0_ = adcGain0 ;
}

/** Set 
 */
void TkDcuConversionFactors::setAdcOffset0 ( double adcOffset0 ) {

  adcOffset0_ = adcOffset0 ;
}

/** Set 
 */
void TkDcuConversionFactors::setAdcCal0( bool adcCal0 ) {

  adcCal0_ = adcCal0 ;
}

/** Set
 */
void TkDcuConversionFactors::setAdcInl0OW ( bool adcInl0OW ) {

  adcInl0OW_ = adcInl0OW ;
}

/** Set
 */
void TkDcuConversionFactors::setAdcInl0 ( double adcInl0 ) {

  adcInl0_ = adcInl0 ;
}

/** Set
 */
void TkDcuConversionFactors::setI20( double i20 ) {

  i20_ = i20 ;
}

/** Set
 */
void TkDcuConversionFactors::setI10( double i10 ) {

  i10_ = i10 ;
}

/** Set
 */
void TkDcuConversionFactors::setICal( bool iCal ) {

  iCal_ = iCal ;
}

/** Set
 */
void TkDcuConversionFactors::setKDiv( double kDiv ) {

  kDiv_ = kDiv ;
}

/** Set
 */
void TkDcuConversionFactors::setKDivCal( bool kDivCal ) {

  kDivCal_ = kDivCal ;
}
	
/** Set
 */
void TkDcuConversionFactors::setTsGain( double tsGain ) {

  tsGain_ = tsGain ;
}

/** Set
 */
void TkDcuConversionFactors::setTsOffset( double tsOffset ) {

  tsOffset_ = tsOffset ;
}

/** Set
 */
void TkDcuConversionFactors::setTsCal( bool tsCal ) {

  tsCal_ = tsCal ;
}

/** Set
 */
void TkDcuConversionFactors::setR68 ( double r68 ) {

  r68_ = r68 ;
}

/** Set
 */
void TkDcuConversionFactors::setR68Cal ( bool r68Cal ) {

  r68Cal_ = r68Cal ;
}

/** Set
 */
void TkDcuConversionFactors::setAdcGain2( double adcGain2 ) {

  adcGain2_ = adcGain2 ;
}


/** Set
 */
void TkDcuConversionFactors::setAdcOffset2( double adcOffset2 ) {

  adcOffset2_ = adcOffset2 ;
}

/** Set
 */
void TkDcuConversionFactors::setAdcCal2( bool adcCal2 ) {

  adcCal2_ = adcCal2 ;
}

/** Set
 */
void TkDcuConversionFactors::setAdcGain3( double adcGain3 ) {

  adcGain3_ = adcGain3 ;
}
  
/** Set
 */
void TkDcuConversionFactors::setAdcOffset3( double adcOffset3 ) {

  adcOffset3_ = adcOffset3 ;
}

/** Set 
 */
void TkDcuConversionFactors::setAdcCal3 ( bool adcCal3 ) {
    
  adcCal3_ = adcCal3 ;
}

/** Get 
 */
double TkDcuConversionFactors::getAdcGain0( ) {

  return (adcGain0_) ;
}

/** Get 
 */
double TkDcuConversionFactors::getAdcOffset0( ) {

  return (adcOffset0_) ;
}

/** Get 
 */
bool TkDcuConversionFactors::getAdcCal0( ) {

  return (adcCal0_) ;
}

/** Get
 */
bool TkDcuConversionFactors::getAdcInl0OW ( ) {

  return adcInl0OW_ ;
}

/** Get
 */
double TkDcuConversionFactors::getAdcInl0 ( ) {

  return adcInl0_ ;
}
	
/** Get
 */
double TkDcuConversionFactors::getI20( ) {

  return (i20_) ; 
}

/** Get
 */
double TkDcuConversionFactors::getI10( ) {

  return (i10_) ;
}

/** Get
 */
bool TkDcuConversionFactors::getICal( ) {

  return (iCal_) ;
}

/** Get
 */
double TkDcuConversionFactors::getKDiv( ) {

  return (kDiv_) ;
}

/** Get
 */
bool TkDcuConversionFactors::getKDivCal( ) {

  return (kDivCal_) ;
}
	
/** Get
 */
double TkDcuConversionFactors::getTsGain( ) {

  return (tsGain_) ;
}

/** Get
 */
double TkDcuConversionFactors::getTsOffset( ) {

  return (tsOffset_) ;
}

/** Get
 */
bool TkDcuConversionFactors::getTsCal( ) {

  return (tsCal_) ; 
}

/** Get
 */
double TkDcuConversionFactors::getR68 ( ) {

  return r68_ ;
}

/** Set
 */
bool TkDcuConversionFactors::getR68Cal ( ) {

  return r68Cal_ ;
}

/** Get
 */
double TkDcuConversionFactors::getAdcGain2( ) {

  return (adcGain2_) ;
}

/** Get
 */
double TkDcuConversionFactors::getAdcOffset2( ) {

  return (adcOffset2_) ;
}

/** Get
 */
bool TkDcuConversionFactors::getAdcCal2( ) {

  return (adcCal2_) ; 
}

/** Get
 */
double TkDcuConversionFactors::getAdcGain3( ) {

  return(adcGain3_) ;
}

/** Get
 */
double TkDcuConversionFactors::getAdcOffset3( ) {

  return (adcOffset3_) ;
}

/** Get 
 */
bool TkDcuConversionFactors::getAdcCal3 ( ) {
    
  return (adcCal3_) ;
}

/** brief Clone TK DCU conversion factors description
 * \return the device description cloned
 */
TkDcuConversionFactors *TkDcuConversionFactors::clone (){

  TkDcuConversionFactors *dcuConversion = new TkDcuConversionFactors (*this) ;

  /*     TkDcuConversionFactors *dcuConversion = new TkDcuConversionFactors ( dcuHardId_, subDetector_, dcuType_); */
  /*     dcuConversion->setStructureId(structureId_); */
  /*     dcuConversion->setContainerId(containerId_); */
  /*     dcuConversion->setDetId(detId_); */
  /*     dcuConversion->setAdcGain0(adcGain0_); */
  /*     dcuConversion->setAdcOffset0(adcOffset0_); */
  /*     dcuConversion->setAdcCal0(adcCal0_); */
  /*     dcuConversion->setAdcInl0(adcInl0_); */
  /*     dcuConversion->setAdcInl0OW(adcInl0OW_); */
  /*     dcuConversion->setI20(i20_); */
  /*     dcuConversion->setI10(i10_); */
  /*     dcuConversion->setICal(iCal_); */
  /*     dcuConversion->setKDiv(kDiv_); */
  /*     dcuConversion->setTsGain(tsGain_); */
  /*     dcuConversion->setTsOffset(tsOffset_); */
  /*     dcuConversion->setTsCal(tsCal_); */
  /*     dcuConversion->setR68(r68_); */
  /*     dcuConversion->setR68Cal(r68Cal_); */
  /*     dcuConversion->setAdcGain2(adcGain2_); */
  /*     dcuConversion->setAdcOffset2(adcOffset2_); */
  /*     dcuConversion->setAdcCal2(adcCal2_); */
  /*     dcuConversion->setAdcGain3(adcGain3_); */
  /*     dcuConversion->setAdcOffset3(adcOffset3_); */
  /*     dcuConversion->setAdcCal3(adcCal3_); */
    
  return (dcuConversion) ;
}


/** \brief Display the conversion factors
 * \param convertedValue - display the converted value with the DCU
 */
void TkDcuConversionFactors::display ( bool convertedValue ) throw (std::string) {

  std::string str = getDisplay( convertedValue ) ;
  std::cout << str ;
}

/** \brief Display the conversion factors
 * \param convertedValue - display the converted value with the DCU
 */
std::string TkDcuConversionFactors::getDisplay ( bool convertedValue ) throw (std::string) {

  std::ostringstream str ;

  // DEFAULT CONVERSION FACTORS
  if ( (dcuHardId_ == 0) && (dcuDescriptionDcuHardId_ != 0) ) {
    if ( dcuType_ == DCUCCU )
      str << "DCU Hardware ID " << std::dec << dcuDescriptionDcuHardId_ << " on the CCU " << "NOT KNOWN" << std::endl ;
    else
      str << "DCU Hardware ID " << std::dec << dcuDescriptionDcuHardId_ << " on the module " << "NOT KNOWN" << " (Det Id = " << detId_ << ")" << std::endl ;
  }
  // NORMAL CONVERSION FACTORS
  else {
    if ( dcuType_ == DCUCCU )
      str << "DCU Hardware ID " << std::dec << dcuHardId_ << " on the CCU " << containerId_ << std::endl ;
    else
      str << "DCU Hardware ID " << std::dec << dcuHardId_ << " on the module " << containerId_ << " (Det Id = " << detId_ << ")" << std::endl ;
  }

  // RING POSITION
  if (dcuDescriptionDcuHardId_ != 0) {
    char msg[80] ; decodeKey(msg,dcuDescriptionKey_) ;
    str << "DCU on " << msg << std::endl ;
  }

  // -------------------------------------------------------------------------------
  if (!convertedValue) {
    str << "\t" << "from " << subDetector_ << " on structure " << structureId_ << std::endl ;
    str << "\t" << "adcGain0 = " << adcGain0_ << std::endl ;
    str << "\t" << "adcOffset0 = " << adcOffset0_ << std::endl ;
    str << "\t" << "adcCal0 = " << (adcCal0_ == false ? "false" : "true") << std::endl ;
    str << "\t" << "adcInl0 = " << adcInl0_ << std::endl ;
    str << "\t" << "adcInl0OW = " << (adcInl0OW_ == false ? "false" : "true") << std::endl ;
    str << "\t" << "i20 = " << i20_ << std::endl ;
    str << "\t" << "i10 = " << i10_ << std::endl ;
    str << "\t" << "iCal = " << (iCal_ == false ? "false" : "true") << std::endl ;
    str << "\t" << "kDiv = " << kDiv_ << std::endl ;
    str << "\t" << "kDivCal = " << (kDivCal_ == false ? "false" : "true") << std::endl ;
    str << "\t" << "tsGain = " << tsGain_ << std::endl ;
    str << "\t" << "tsOffset = " << tsOffset_ << std::endl ;
    str << "\t" << "tsCal = " << (tsCal_ == false ? "false" : "true") << std::endl ;
    str << "\t" << "r68 = " << r68_ << std::endl ;
    str << "\t" << "r68Cal = " << (r68Cal_ == false ? "false" : "true") << std::endl ;
    str << "\t" << "adcGain2 = " << adcGain2_ << std::endl ;
    str << "\t" << "adcOffset2 = " << adcOffset2_ << std::endl ;
    str << "\t" << "adcCal2 = " << (adcCal2_ == false ? "false" : "true") << std::endl ;
    str << "\t" << "adcGain3 = " << adcGain3_ << std::endl ;
    str << "\t" << "adcOffset3 = " << adcOffset3_ << std::endl ;
    str << "\t" << "adcCal3 = " << (adcCal3_ == false ? "false" : "true") << std::endl ;
  }
  else if (dcuDescriptionDcuHardId_ != 0) { // Display the converted value

    if ( (dcuType_ != DCUCCU) && (dcuType_ != DCUFEH) &&
	 ( (dcuDescriptionDcuType_ == DCUCCU) ||
	   (dcuDescriptionDcuType_ == DCUFEH) ) )
      dcuType_ = dcuDescriptionDcuType_ ;
      
    str << "\t" << "from " << subDetector_ << " on structure " << structureId_ << std::endl ;
    bool quality ;
    if (getDcuType() == DCUFEH) {

      str << "\tTemperature on the silicon sensor (" << std::dec << dcuDescriptionChannels_[0] << "): " << getSiliconSensorTemperature(quality) << " C";
      if (quality) str << " (Quality = Good)" << std::endl ;
      else str << " (Quality = Bad)" << std::endl ;
      str << "\tV250                              (" << std::dec << dcuDescriptionChannels_[1] << "): " << getV250(quality) << " mV";
      if (quality) str << " (Quality = Good)" << std::endl ;
      else str << " (Quality = Bad)" << std::endl ;
      str << "\tV125                              (" << std::dec << dcuDescriptionChannels_[2] << "): " << getV125(quality) << " mV";
      if (quality) str << " (Quality = Good)" << std::endl ;
      else str << " (Quality = Bad)" << std::endl ;
      str << "\tLeakage current                   (" << std::dec << dcuDescriptionChannels_[3] << "): " << getILeak(quality) << " uA";
      if (quality) str << " (Quality = Good)" << std::endl ;
      else str << " (Quality = Bad)" << std::endl ;
      str << "\tTemperature of hybrid             (" << std::dec << dcuDescriptionChannels_[4] << "): " << getHybridTemperature(quality) << " C";
      if (quality) str << " (Quality = Good)" << std::endl ;
      else str << " (Quality = Bad)" << std::endl ;
      str << "\tTemperature of the DCU            (" << std::dec << dcuDescriptionChannels_[7] << "): " << getDcuTemperature(quality) << " C";
      if (quality) str << " (Quality = Good)" << std::endl ;
      else str << " (Quality = Bad)" << std::endl ;
    }
    else if (getDcuType() == DCUCCU) {

      if (subDetector_ == "TOB") {

	str << "\tTemperature on the silicon sensor(" << std::dec << dcuDescriptionChannels_[0] << "): " << getSiliconSensorTemperature(quality) << " C";
	if (quality) str << " (Quality = Good)" << std::endl ;
	else str << " (Quality = Bad)" << std::endl ;
	str << "\tV250                             (" << std::dec << dcuDescriptionChannels_[2] << "): " << getV250(quality) << " mV";
	if (quality) str << " (Quality = Good)" << std::endl ;
	else str << " (Quality = Bad)" << std::endl ;
	str << "\tV125                             (" << std::dec << dcuDescriptionChannels_[3] << "): " << getV125(quality) << " mV";
	if (quality) str << " (Quality = Good)" << std::endl ;
	else str << " (Quality = Bad)" << std::endl ;
      }
      else if (subDetector_ == "TEC") {
	str << "\tTemperature of the DCU              (" << std::dec << dcuDescriptionChannels_[7] << "): " << getDcuTemperature(quality) << " C";
	if (quality) str << " (Quality = Good)" << std::endl ;
	else str << " (Quality = Bad)" << std::endl ;
      }
      else if ( (subDetector_ == "TIB") || (subDetector_ == "TID") ) {
	str << "\tTemperature of the DCU              (" << std::dec << dcuDescriptionChannels_[7] << "): " << getDcuTemperature(quality) << " C";
	if (quality) str << " (Quality = Good)" << std::endl ;
	else str << " (Quality = Bad)" << std::endl ;
      }
      else {
	str << "The sub-detector (TIB, TID, TOB, TEC) is not known and no value can be printout" << std::endl ;
      }
    }
    else {
      str << "The DCU type (DCU on FEH or DCU on CCU) is not known and no value can be printout" << std::endl ;
    }
  }
  else {
    throw std::string("TkDcuConversionFactors::display: No DCU description specified") ;
  }

  return str.str() ;
}

/** \brief Evaluate the silicon sensor temperature from the channel 0 for a FEH or a DCU on CCU
 * \param quality - data quality (good or bad)
 * \return silicon sensor temperature
 * \warning return temperature are in degree celcuis
 */
double TkDcuConversionFactors::getSiliconSensorTemperature ( bool &quality ) throw (std::string) {

  double tSi = 0 ;

  // FEH
  if (dcuType_ == DCUFEH) {
    if ( (subDetector_ == "TOB") || (subDetector_ == "TEC") ) {
      double value = log((dcuDescriptionChannels_[0] - adcOffset0_) / (adcGain0_ * i20_ * (RTH0 / 2))) ;
      tSi = 1/(1/T0 + value/BETA) ;
      quality = adcCal0_ && iCal_ ;
    }
    else if ( (subDetector_ == "TIB") || (subDetector_ == "TID") ) {
      double value = log((dcuDescriptionChannels_[0] - adcOffset0_) / (adcGain0_ * i20_ * RTH0)) ;
      tSi = 1/(1/T0 + value/BETA) ;
      quality = adcCal0_ && iCal_ ;
    }
    else {
      double value = log((dcuDescriptionChannels_[0] - adcOffset0_) / (adcGain0_ * i20_ * (RTH0 / 2))) ;
      tSi = 1/(1/T0 + value/BETA) ;
      quality = adcCal0_ && iCal_ ;
    }
  }			 
  // DCU on CCU
  else if (dcuType_ == DCUCCU) {
    if ((subDetector_ == "TIB") || (subDetector_ == "TID")) throw std::string ("TkDcuConversionFactors::getSiliconSensorTemperature: the TIB/TID DCU on CCU is not used") ;
    else if (subDetector_ == "TEC") throw std::string ("TkDcuConversionFactors::getSiliconSensorTemperature: the TEC DCU on CCU is not used") ;
    else {
      double value = log((dcuDescriptionChannels_[0] - adcOffset0_) / (adcGain0_ * i20_ * ((RTH0 * r68_) / (RTH0 + r68_)))) ;
      tSi = 1/(1/T0 + value/BETA) ;
      quality = adcCal0_ && iCal_ && r68Cal_ ;
    }
  }
  else throw std::string ("TkDcuConversionFactors::getSiliconSensorTemperature: No type are specified for this DCU") ;

  // flag raw 0 
  if (dcuDescriptionChannels_[0] == 0) return -9999.;

  return (tSi - DIFFKELCEL) ;
}

/** \brief V250 from channel 1 for a FEH or from a channel 2 for a DCU on CCU
 * \param quality - data quality (good or bad)
 * \return V250 in mV
 */
double TkDcuConversionFactors::getV250 ( bool &quality ) throw (std::string) {

  double v250 = 0 ;
  if (dcuType_ == DCUFEH) {
    v250  = (dcuDescriptionChannels_[1] - adcOffset0_) / (adcGain0_ * kDiv_) ;
    quality = adcCal0_ && kDivCal_ ;
  }
  else if (dcuType_ == DCUCCU) {
    if ((subDetector_ == "TIB") || (subDetector_ == "TID")) throw std::string ("TkDcuConversionFactors::getV250: the TIB/TID DCU on CCU is not used") ;
    else if (subDetector_ == "TEC") throw std::string ("TkDcuConversionFactors::getV250: the TEC DCU on CCU is not used") ;
    else {
      v250 = (dcuDescriptionChannels_[2] - adcOffset2_)/ adcGain2_ ;
      quality = adcCal2_ ;
    }
  }
  else throw std::string ("TkDcuConversionFactors::getV250: No type are specified for this DCU") ;

  return v250 ;
}

/** \brief V125 from channel 2 for a FEH or from channel 3 for a DCU on CCU
 * \param quality - data quality (good or bad)
 * \return V125 in mV
 */
double TkDcuConversionFactors::getV125 ( bool &quality ) throw (std::string) {

  double v125 = 0 ;
  if (dcuType_ == DCUFEH) {
    v125 = (dcuDescriptionChannels_[2] - adcOffset0_)/ adcGain0_ ;
    quality = adcCal0_ ;
  }
  else if (dcuType_ == DCUCCU) {
    if ((subDetector_ == "TIB") || (subDetector_ == "TID")) throw std::string ("TkDcuConversionFactors::getV125: the TIB/TID DCU on CCU is not used") ;
    else if (subDetector_ == "TEC") throw std::string ("TkDcuConversionFactors::getV125: the TEC DCU on CCU is not used") ;
    else {
      v125 = (dcuDescriptionChannels_[3] - adcOffset3_)/ adcGain3_ ;
      quality = adcCal3_ ;
    }
  }
  else throw std::string ("TkDcuConversionFactors::getV125: No type are specified for this DCU") ;

  return v125 ;
}

/** \brief Leakage current from channel 3 for a FEH, cannot be recovered for a DCU on CCU
 * \param quality - data quality (good or bad)
 * \return Leakage current in uA
 */
double TkDcuConversionFactors::getILeak ( bool &quality ) throw (std::string) {

  double iLeak = 0 ;

  if (dcuType_ == DCUFEH) {
    iLeak = (dcuDescriptionChannels_[3] - adcOffset0_)/ (adcGain0_ * RLEAK) ;
    iLeak *= 1000 ;
  }
  else
    throw std::string ("TkDcuConversionFactors::getILeak: the Leakage current (I Leak) does not exists for a DCU on CCU") ;

  quality = adcCal0_ ;

  return iLeak ;
}

/** \brief Temperature of each hybrid from channel 4 of FEH, cannnot be recovered for a DCU on CCU
 * \param quality - data quality (good or bad)
 * \return Temperature of the corresponding hybrid
 * \warning return temperature are in degree celcuis
 */
double TkDcuConversionFactors::getHybridTemperature ( bool &quality ) throw (std::string) {

  double tHyb = 0 ;

  if (dcuType_ == DCUFEH) {
    double value = log((dcuDescriptionChannels_[4] - adcOffset0_) / (adcGain0_ * i10_ * RTH0)) ;
    tHyb = 1/(1/T0 + value/BETA) ;
  }
  else 
    throw std::string ("TkDcuConversionFactors::getHybridTemperature: the Temperature of each hybrid does not exists for a DCU on CCU") ;

  quality = adcCal0_ && iCal_ ;

  if (dcuDescriptionChannels_[4]==0) return -9999.;

  return (tHyb  - DIFFKELCEL) ;
}
      
/** \brief Temperature of the DCU from channel 7 for a FEH or from channel 7 for a DCU on CCU
 * \param quality - data quality (good or bad)
 * \return Temperature of the DCU
 */
double TkDcuConversionFactors::getDcuTemperature ( bool &quality ) throw (std::string) {

  double tDcu = 0 ;

  if (dcuType_ == DCUFEH) 
    tDcu = (dcuDescriptionChannels_[7] - tsOffset_) / tsGain_ + TZEROCELSIUS ;
  else if (dcuType_ == DCUCCU) {
    //if (subDetector_ == "TIB") throw std::string ("TkDcuConversionFactors::getDcuTemperature: the TIB DCU on CCU is not used") ; else
    tDcu = (dcuDescriptionChannels_[7] - tsOffset_) / tsGain_ + TZEROCELSIUS ;
  }
  else throw std::string ("TkDcuConversionFactors::getDcuTemperature: No type are specified for this DCU") ;

  quality = tsCal_ ;

  // flag raw 0
  if (dcuDescriptionChannels_[7] == 0) tDcu=-9999.;

  return tDcu ;
}


/** \brief check if the difference between two DCUs and return true if it reach a certain level of %
 * \param dcu - dcu to be compare
 * \param level - level in % to raise true
 * \return true or false if one of the values is greater than the level
 * \exception if the DCU hard id is different
 * \warning the DCU should be already associated !
 */
bool TkDcuConversionFactors::isPourcentageDifferent ( dcuDescription *dcu, double pourcent, std::string &dcuChannelChanged ) {

  if (dcu->getDcuHardId() != dcuDescriptionDcuHardId_) {
    throw std::string ("TkDcuConversionFactors::isPourcentageDifferent: cannot compare two different DCUs (the DCU hard id is not the same)") ;
  }
    
  TkDcuConversionFactors tkLocalConversion (*this) ;
  tkLocalConversion.setDcuDescription(dcu) ;

  if ( (tkLocalConversion.getSubDetector() != getSubDetector()) ||
       (tkLocalConversion.getDcuType() != getDcuType()) ) {
    throw std::string ("TkDcuConversionFactors::isPourcentageDifferent: cannot compare two different DCUs (or the subdetector is different or the DCU type is different)") ;
  }

  bool levelreached = false ;
  bool quality ;

  // Make the comparison for everything
  try {
    double val0 = getSiliconSensorTemperature(quality) ;
    double val1 = tkLocalConversion.getSiliconSensorTemperature(quality) ;

    double diff = val0 - val1 ;
    diff = diff < 0 ? -diff : diff ;
    double level = (diff / (val0 > val1 ? val0 : val1)) * 100 ;

    if (level >= pourcent) {
      std::stringstream diffString ;
      diffString << "Detect a difference of " << std::dec << level << "% on the silicon temperature for the DCU " << std::hex << dcuDescriptionDcuHardId_ << " (" << val0 << "/" << val1 << ")" ;
      dcuChannelChanged = diffString.str() ;
#ifdef DEBUGMSGERROR
      std::cout << dcuChannelChanged << std::endl ;
#endif
      return true ;
    }
  }
  catch (std::string e) { }

  try {
    double val0 = getV250(quality) ;
    double val1 = tkLocalConversion.getV250(quality) ;

    double diff = val0 - val1 ;
    diff = diff < 0 ? -diff : diff ;
    double level = (diff / (val0 > val1 ? val0 : val1)) * 100 ;

    if (level >= pourcent) {
      std::stringstream diffString ;
      diffString  << "Detect a difference of " << std::dec << level << "% on the V250 for the DCU " << std::hex << dcuDescriptionDcuHardId_ << " (" << val0 << "/" << val1 << ")" ;
      dcuChannelChanged = diffString.str() ;
#ifdef DEBUGMSGERROR
      std::cout << dcuChannelChanged << std::endl ;
#endif
      return true ;
    }
  }
  catch (std::string e) { }
    
  try {
    double val0 = getV125(quality) ;
    double val1 = tkLocalConversion.getV125(quality) ;

    double diff = val0 - val1 ;
    diff = diff < 0 ? -diff : diff ;
    double level = (diff / (val0 > val1 ? val0 : val1)) * 100 ;

    if (level >= pourcent) {
      std::stringstream diffString ;
      diffString << "Detect a difference of " << std::dec << level << "% on the V125 for the DCU " << std::hex << dcuDescriptionDcuHardId_ << " (" << val0 << "/" << val1 << ")" ;
      dcuChannelChanged = diffString.str() ;
#ifdef DEBUGMSGERROR
      std::cout << dcuChannelChanged << std::endl ;
#endif
      return true ;
    }
  }
  catch (std::string e) { }

  try {
    double val0 = getILeak(quality) ;
    double val1 = tkLocalConversion.getILeak(quality) ;

    double diff = val0 - val1 ;
    diff = diff < 0 ? -diff : diff ;
    double level = (diff / (val0 > val1 ? val0 : val1)) * 100 ;

    if (level >= pourcent) {
      std::stringstream diffString ;
      diffString << "Detect a difference of " << std::dec << level << "% on the ILEAK for the DCU " << std::hex << dcuDescriptionDcuHardId_ << " (" << val0 << "/" << val1 << ")" ;
      dcuChannelChanged = diffString.str() ;
#ifdef DEBUGMSGERROR
      std::cout << dcuChannelChanged << std::endl ;
#endif
      return true ;
    }
  }
  catch (std::string e) { }

  try {
    double val0 = getHybridTemperature(quality);
    double val1 = tkLocalConversion.getHybridTemperature(quality);

    double diff = val0 - val1 ;
    diff = diff < 0 ? -diff : diff ;
    double level = (diff / (val0 > val1 ? val0 : val1)) * 100 ;

    if (level >= pourcent) {
      std::stringstream diffString ;
      diffString << "Detect a difference of " << std::dec << level << "% on the hyrid temperature for the DCU " << std::hex << dcuDescriptionDcuHardId_ << " (" << val0 << "/" << val1 << ")" ;
      dcuChannelChanged = diffString.str() ;
#ifdef DEBUGMSGERROR
      std::cout << dcuChannelChanged << std::endl ;
#endif
      return true ;
    }
  }
  catch (std::string e) { }

  try {
    double val0 = getDcuTemperature(quality);
    double val1 = tkLocalConversion.getDcuTemperature(quality);

    double diff = val0 - val1 ;
    diff = diff < 0 ? -diff : diff ;
    double level = (diff / (val0 > val1 ? val0 : val1)) * 100 ;

    if (level >= pourcent) {
      std::stringstream diffString ;
      diffString << "Detect a difference of " << std::dec << level << "% on the DCU temperature for the DCU " << std::hex << dcuDescriptionDcuHardId_ << " (" << val0 << "/" << val1 << ")" << std::endl ;
      dcuChannelChanged = diffString.str() ;
#ifdef DEBUGMSGERROR
      std::cout << dcuChannelChanged << std::endl ;
#endif
      return true ;
    }
  }
  catch (std::string e) { }

  return levelreached ;
}

/** \brief return the corresponding subdetector from the det id
 * \return the string with TIB, TIB, TOB, TEC+, TEC- and an exception if the det id is not recognized
 */
std::string TkDcuConversionFactors::getSubDetectorFromDetId ( ) throw (std::string) {

  return TkDcuInfo::getSubDetectorFromDetId ( detId_ ) ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *TkDcuConversionFactors::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[DCUCONVERTHARDID]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[DCUCONVERTHARDID], ParameterDescription::INTEGER32) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[SUBDETECTOR]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[SUBDETECTOR], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[DCUTYPE]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[DCUTYPE], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[STRUCTUREID]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[STRUCTUREID], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[CONTAINERID]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[CONTAINERID], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[DETID]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[DETID], ParameterDescription::INTEGER32) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCGAIN0]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCGAIN0], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCOFFSET0]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCOFFSET0], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCINL0]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCINL0], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCINL0OW]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCINL0OW], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCCAL0]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCCAL0], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[I20]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[I20], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[I10]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[I10], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[IDCUCAL]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[IDCUCAL], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[KDIV]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[KDIV], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[KDIVCAL]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[KDIVCAL], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[TSGAIN]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[TSGAIN], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[TSOFFSET]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[TSOFFSET], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[TSCAL]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[TSCAL], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[R68]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[R68], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[R68CAL]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[R68CAL], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCGAIN2]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCGAIN2], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCOFFSET2]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCOFFSET2], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCCAL2]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCCAL2], ParameterDescription::STRING) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCGAIN3]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCGAIN3], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCOFFSET3]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCOFFSET3], ParameterDescription::REAL) ;
  (*parameterNames)[DCUFACTORPARAMETERNAMES[ADCCAL3]] = new ParameterDescription(DCUFACTORPARAMETERNAMES[ADCCAL3], ParameterDescription::STRING) ;
  return parameterNames ;
}

/** \brief Delete a list of parameter name and its content
 * \warning the parameter parameterNames is deleted by this method
 */
void TkDcuConversionFactors::deleteParameterNames(parameterDescriptionNameType *parameterNames) {
  deviceDescription::deleteParameterNames ( parameterNames ) ;
}

// Initialise of static variable
//const double TkDcuConversionFactors::RTH0       = 10000  ;
//const double TkDcuConversionFactors::BETA       = 3280   ;
//const double TkDcuConversionFactors::RLEAK      = 681    ;
//const double TkDcuConversionFactors::T0         = 298.15 ;
//const double TkDcuConversionFactors::GAINTS     = 8.901  ;
//const double TkDcuConversionFactors::DIFFKELCEL = 273.15 ;
// End of initialisation

const char *TkDcuConversionFactors::DCUFACTORPARAMETERNAMES[] = {"dcuHardId",
								 "subDetector",
								 "dcuType",
								 "structureId",
								 "containerId",
								 "detId",
								 "adcGain0",
								 "adcOffset0",
								 "adcInl0",
								 "adcInl0OW",
								 "adcCal0",
								 "i20",
								 "i10",
								 "iCal",
								 "kDiv",
								 "kCal",
								 "tsGain",
								 "tsOffset",
								 "tsCal",
								 "r68",
								 "r68Cal",
								 "adcGain2",
								 "adcOffset2",
								 "adcCal2",
								 "adcGain3",
								 "adcOffset3",
								 "adcCal3" } ;

