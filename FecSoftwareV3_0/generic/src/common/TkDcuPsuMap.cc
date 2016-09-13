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
  
  Copyright 2002 - 2007, Guillaume BAULIEU - IPNL/IN2P3
*/
  //#define DEBUGMSGERROR

#include <iostream>
#include <string>
#include "TkDcuPsuMap.h"

/** Constructor
 */
TkDcuPsuMap::TkDcuPsuMap ( tscType32 dcuHardId, std::string psuName, std::string psuType, keyType fecIndex, std::string dcuType ) throw (FecExceptionHandler):
  dcuHardId_(dcuHardId),
  psuType_(psuType),
  fecIndex_(fecIndex),
  dcuType_(dcuType) {      

  setPsuName (psuName) ;
}

/** Constructor with the parameter definition extracted from the database or XML file
 */
TkDcuPsuMap::TkDcuPsuMap ( parameterDescriptionNameType parameterNames ) throw (FecExceptionHandler):
  dcuHardId_(0),
  dpName_(""),
  pvssName_(""),
  psuType_(""),
  fecIndex_(0),
  dcuType_("NONE") {

  setDcuHardId (*((tscType32 *) parameterNames[DCUPSUMAPNAMES[DCUPSUMAPHARDID]]->getValueConverted())) ;
  setPsuName (*((std::string *) parameterNames[DCUPSUMAPNAMES[DCUPSUMAPPSUNAME]]->getValueConverted())) ;
  setPsuType (*((std::string *) parameterNames[DCUPSUMAPNAMES[DCUPSUTYPE]]->getValueConverted())) ;
}
    
/** Set the DCU hard ID
 */
void TkDcuPsuMap::setDcuHardId ( tscType32 dcuHardId ) {

  dcuHardId_ = dcuHardId ;
}

/** Get the DCU hard ID
 */
tscType32 TkDcuPsuMap::getDcuHardId ( ) {
  
  return (dcuHardId_) ;
}

/** Set the PSU Name
 * The PSU name is built with two parts, the data point name used by the program to switch on/off the power supply
 * and the second name is the name need by PVSS DB to build the table. For example:
 *     TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard07,MYTK_TEC_B3
 * where TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard07 is the datapoint name and MYTK_TEC_B3 is for PVSS
 * DB. This method will separate in two parts this name. If one error is detected then an exception is raised.
 * \param psuName - complete PSU name
 */
void TkDcuPsuMap::setPsuName ( std::string psuName ) throw (FecExceptionHandler) {

  std::string::size_type loc = psuName.find( COMMA, 0 );
  if (loc == std::string::npos) {
    RAISEFECEXCEPTIONHANDLER(XML_INVALIDPSUNAME,XML_INVALIDPSUNAME + ", missing comma: " + psuName,ERRORCODE) ;
  }
  else {
    if (psuName.size()) {
      dpName_ = psuName.substr(0,loc) ;
      pvssName_ = psuName.substr(loc+1,psuName.size()) ;

      //std::cout << dpName_ << " and " << pvssName_ << std::endl ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER(XML_INVALIDPSUNAME,XML_INVALIDPSUNAME + ", the PSU name is empty",ERRORCODE) ;
    }
  }
}

/** Get the complete PSU Name
 */
std::string TkDcuPsuMap::getPsuName ( ) {
  
  return (dpName_ + "," + pvssName_) ;
}

/** Get the datapoint name
 */
std::string TkDcuPsuMap::getDatapointName ( ) {

  std::string dpName = dpName_ ;
  if (psuType_ == PSUDCUTYPE_PG) dpName += "/channel000" ;

  return (dpName) ;
}

/** Get the PVSS name
 */
std::string TkDcuPsuMap::getPVSSName ( ) {

  return (pvssName_) ;
}

/** Set the PSU type
 */
void TkDcuPsuMap::setPsuType ( std::string psuType ) {

  psuType_ = psuType ;
}

/** Get the PSU type
 */
std::string TkDcuPsuMap::getPsuType ( ) {

  return psuType_ ;
}

/** Set the PSU type
 * \param fecIndex - FEC key 
 */
void TkDcuPsuMap::setFecIndex ( keyType fecIndex ) {
  fecIndex_ = fecIndex ;
}

/** Get the PSU type
 */
keyType TkDcuPsuMap::getFecIndex ( ) {

  return fecIndex_ ;
}

/** \brief set the DCU type
 */
void TkDcuPsuMap::setDcuType ( std::string dcuType ) {

  dcuType_ = dcuType ;
}

/** \brief get the DCU type
 * \return DCU type
 */
std::string TkDcuPsuMap::getDcuType ( ) {

  return dcuType_ ;
}

/** brief Clone TK DCU-PSU map description
 * \return the PSU map cloned
 */
TkDcuPsuMap *TkDcuPsuMap::clone (){
  
  TkDcuPsuMap *dcuPsuMap = new TkDcuPsuMap( *this ) ; // ( dcuHardId_, getPsuName(), psuType_, fecIndex_, dcuType_ );
  
  return (dcuPsuMap) ;
}

/** \brief Display the Dcu Info
 */
void TkDcuPsuMap::display ( ) {
  std::cout << "DCU Hardware ID: " << dcuHardId_ << std::endl ;
  std::cout << "PSU Name: " << getPsuName() << std::endl ;
  std::cout << "PSU type: " << psuType_ << std::endl ;
  std::cout << "FEC key: " << std::hex << fecIndex_ << std::dec << std::endl ;
  std::cout << "DCU type: " << dcuType_ << std::endl ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *TkDcuPsuMap::getParameterNames ( ) {
  
  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;
    
  (*parameterNames)[DCUPSUMAPNAMES[DCUPSUMAPHARDID]] = new ParameterDescription(DCUPSUMAPNAMES[DCUPSUMAPHARDID], ParameterDescription::INTEGER32) ;
  (*parameterNames)[DCUPSUMAPNAMES[DCUPSUMAPPSUNAME]] = new ParameterDescription(DCUPSUMAPNAMES[DCUPSUMAPPSUNAME], ParameterDescription::STRING) ;
  (*parameterNames)[DCUPSUMAPNAMES[DCUPSUTYPE]] = new ParameterDescription(DCUPSUMAPNAMES[DCUPSUTYPE], ParameterDescription::STRING) ;
  
  return parameterNames ;
}

/** \brief Delete a list of parameter name and its content
 * \warning the parameter parameterNames is deleted by this method
 */
void TkDcuPsuMap::deleteParameterNames(parameterDescriptionNameType *parameterNames) {

  for (parameterDescriptionNameType::iterator p = parameterNames->begin() ; p != parameterNames->end() ; p ++) {

    ParameterDescription *val = p->second ;
    delete val ;
  }
  parameterNames->clear() ;
}

/** \brief sort a vector of DCU / PSU map
 * the sorting is done by the PSU type then by the PSU name so the control group will be the first part of the vector then the power group will be in the second part.
 */
bool TkDcuPsuMap::sortByPsuType ( TkDcuPsuMap *p1, TkDcuPsuMap *p2 ) {

  if (p1->getPsuType() < p2->getPsuType()) return true ;
  else if (p1->getPsuType() > p2->getPsuType()) return false ;
  else if (p1->getPVSSName() < p2->getPVSSName()) return true ;
  else return false ;
}


/** Parameter's name definition
 */
const char *TkDcuPsuMap::DCUPSUMAPNAMES[] = {"dcuHardId", "psuName", "psuType"} ;
