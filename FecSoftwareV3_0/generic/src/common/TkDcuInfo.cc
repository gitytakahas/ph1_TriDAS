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
#include "TkDcuInfo.h"
#include "deviceDescription.h"

/** Constructor
 */
TkDcuInfo::TkDcuInfo ( tscType32 dcuHardId, tscType32 detId, double fibreLength, tscType32 apvNumber, double timeOfFlight ):
  dcuHardId_(dcuHardId),
  detId_(detId),
  fibreLength_(fibreLength),
  apvNumber_(apvNumber),
  timeOfFlight_(timeOfFlight){
      
}
  
/** Constructor with the parameter definition extracted from the database or XML file
 */
TkDcuInfo::TkDcuInfo ( parameterDescriptionNameType parameterNames ):
  dcuHardId_(0),
  detId_(0),
  fibreLength_(0.0),
  apvNumber_(0),
  timeOfFlight_(0.0) {
    
  setDcuHardId (*((tscType32 *) parameterNames[DCUINFOPARAMETERNAMES[DCUINFOHARDID]]->getValueConverted())) ;
  setDetId (*((tscType32 *) parameterNames[DCUINFOPARAMETERNAMES[DCUINFODETID]]->getValueConverted())) ;
  setFibreLength (*((double *) parameterNames[DCUINFOPARAMETERNAMES[DCUINFOFIBRELENGTH]]->getValueConverted())) ;
  setApvNumber (*((tscType32 *) parameterNames[DCUINFOPARAMETERNAMES[DCUINFOAPVNUMBER]]->getValueConverted())) ;
  setTimeOfFlight (*((double *) parameterNames[DCUINFOPARAMETERNAMES[DCUINFOTIMEOFFLIGHT]]->getValueConverted())) ;
}

/** Nothing
 */
TkDcuInfo::~TkDcuInfo ( ) {
}
    
/** Set the DCU hard ID
 */
void TkDcuInfo::setDcuHardId ( tscType32 dcuHardId ) {

  dcuHardId_ = dcuHardId ;
}

/** Get the DCU hard ID
 */
tscType32 TkDcuInfo::getDcuHardId ( ) {

  return (dcuHardId_) ;
}

/** Set the Det ID
 */
void TkDcuInfo::setDetId ( tscType32 detId) {
  detId_ = detId ;
}

/** Get the det ID
 */
tscType32 TkDcuInfo::getDetId ( ) {
  return detId_ ;
}

/** Set the Fibre length
 */
void TkDcuInfo::setFibreLength ( double fibreLength ) {

  fibreLength_ = fibreLength ;
}

/** Get the fibre length
 */
double TkDcuInfo::getFibreLength ( ) {

  return fibreLength_ ;
}

/** Set the APV number
 */
void TkDcuInfo::setApvNumber ( tscType32 apvNumber ) {

  apvNumber_ = apvNumber ;
}

/** Get the APV number
 */
tscType32 TkDcuInfo::getApvNumber( ) {

  return apvNumber_ ;
}

/** Set the time of flight number
 */
void TkDcuInfo::setTimeOfFlight ( double timeOfFlight ) {

  timeOfFlight_ = timeOfFlight ;
}

/** Get the time of flight number
 */
double TkDcuInfo::getTimeOfFlight( ) {

  return timeOfFlight_ ;
}

/** brief Clone TK DCU info description
 * \return the device description cloned
 */
TkDcuInfo *TkDcuInfo::clone (){
    
  TkDcuInfo *dcuInfo = new TkDcuInfo( *this ) ; // dcuHardId_, detId_, fibreLength_, apvNumber_, timeOfFlight_);

  return (dcuInfo) ;
}

/** \brief Display the Dcu Info
 */
void TkDcuInfo::display ( ) {

  std::cout << "DCU Hardware ID : " << dcuHardId_ << std::endl ;
  std::cout << "Geometrical ID : " << detId_ << std::endl ;
  std::cout << "FibreLength : " << fibreLength_ << std::endl ;
  std::cout << "Number of APV : " << apvNumber_ << std::endl ;
  std::cout << "Time of flight : " << timeOfFlight_ << std::endl ;
}

/** \brief return the corresponding subdetector from the det id
 * \return the string with TIB, TIB, TOB, TEC+, TEC- and an exception if the det id is not recognized
 */
std::string TkDcuInfo::getSubDetectorFromDetId ( ) throw (std::string) {

  return TkDcuInfo::getSubDetectorFromDetId ( detId_ ) ;
}

/** \brief return the corresponding subdetector from the det id
 * \param detId - detector ID
 * \return the string with TIB, TIB, TOB, TEC+, TEC- and an exception if the det id is not recognized
 */
std::string TkDcuInfo::getSubDetectorFromDetId ( unsigned int detId ) throw (std::string) {

  if (ISTOB(detId)) return "TOB" ;
  if (ISTIB(detId)) return "TIB" ;
  if (ISTID(detId)) return "TIB" ;
  if (ISTEC(detId) && ISPLUS(detId)) return "TEC+" ;
  if (ISTEC(detId) && ISMINUS(detId)) return "TEC-" ;

  throw std::string ("TkDcuInfo::getSubDetectorFromDetId: unknown subdetector for the DET ID " + toString(detId)) ;

  return "" ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *TkDcuInfo::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;

  (*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFOHARDID]] = new ParameterDescription(DCUINFOPARAMETERNAMES[DCUINFOHARDID], ParameterDescription::INTEGER32) ;
  (*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFODETID]] = new ParameterDescription(DCUINFOPARAMETERNAMES[DCUINFODETID], ParameterDescription::INTEGER32) ;
  (*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFOFIBRELENGTH]] = new ParameterDescription(DCUINFOPARAMETERNAMES[DCUINFOFIBRELENGTH], ParameterDescription::REAL) ;
  (*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFOAPVNUMBER]] = new ParameterDescription(DCUINFOPARAMETERNAMES[DCUINFOAPVNUMBER], ParameterDescription::INTEGER32) ;
  (*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFOTIMEOFFLIGHT]] = new ParameterDescription(DCUINFOPARAMETERNAMES[DCUINFOTIMEOFFLIGHT], ParameterDescription::REAL) ;
    
  //(*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFOHARDID]]->setValue(0);
  //(*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFODETID]]->setValue(0);
  //(*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFOFIBRELENGTH]]->setValue(0);
  //(*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFOAPVNUMBER]]->setValue(0);
  //(*parameterNames)[DCUINFOPARAMETERNAMES[DCUINFOTIMEOFFLIGHT]]->setValue(0);
    
  return parameterNames ;
}

/** \brief Delete a list of parameter name and its content
 * \warning the parameter parameterNames is deleted by this method
 */
void TkDcuInfo::deleteParameterNames(parameterDescriptionNameType *parameterNames) {
  deviceDescription::deleteParameterNames ( parameterNames ) ;
}


/** Parameter's names
 */
const char *TkDcuInfo::DCUINFOPARAMETERNAMES[] = {"dcuHardId", "detId", "fibreLength", "apvNumber", "timeOfFlight"} ;
