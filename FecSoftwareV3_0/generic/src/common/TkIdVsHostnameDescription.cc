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

#include "TkIdVsHostnameDescription.h"

#include "stringConv.h"

/** Create a TkIdVsHostnameDescription for a FED
 * \param hostname - hostname
 * \param crateId - crate ID (or crate name)
 * \param slot - VME slot
 * \param subDetector - sub-detector
 * \param fedSoftId - FED software ID
 * \param crateNumber - crate number
 */
TkIdVsHostnameDescription::TkIdVsHostnameDescription ( std::string hostname, std::string crateId, unsigned int slot, std::string subDetector, unsigned int fedSoftId, unsigned int crateNumber ) {

  strncpy (hostname_, hostname.c_str(), MAXCHAR) ;
  strncpy (crateId_, crateId.c_str(), MAXCHAR) ;
  slot_ = slot ;
  strncpy (subDetector_, subDetector.c_str(), MAXCHAR) ;
  snprintf (id_, MAXCHAR, "%u", fedSoftId) ;
  crateNumber_ = crateNumber ;
}

/** Create a TkIdVsHostnameDescription for a FEC
 * \param hostname - hostname
 * \param crateId - crate ID (or crate name)
 * \param slot - VME slot
 * \param subDetector - sub-detector
 * \param fecHardId - FEC hardware ID
 * \param crateNumber - crate number
 */
TkIdVsHostnameDescription::TkIdVsHostnameDescription ( std::string hostname, std::string crateId, unsigned int slot, std::string subDetector, std::string fecHardId, unsigned int crateNumber ) {

  strncpy (hostname_, hostname.c_str(), MAXCHAR) ;
  strncpy (crateId_, crateId.c_str(), MAXCHAR) ;
  slot_ = slot ;
  strncpy (subDetector_, subDetector.c_str(), MAXCHAR) ;
  strncpy (id_, fecHardId.c_str(), MAXCHAR) ;
  crateNumber_ = crateNumber ;
}

/** Build a TkIdVsHostnameDescription based on a XML parsing (from file or database)
 * \param parameterNames - parameter names extracted from XML
 */
TkIdVsHostnameDescription::TkIdVsHostnameDescription ( parameterDescriptionNameType parameterNames ):
  slot_(0) {

  memset(hostname_,0,MAXCHAR) ;
  memset(crateId_,0,MAXCHAR) ;
  memset(subDetector_,0,MAXCHAR) ;
  memset(id_,0,MAXCHAR) ;
  
  setHostname((std::string)parameterNames[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSHOSTNAME]]->getValue()) ;
  setCrateId((std::string)parameterNames[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSCRATEID]]->getValue()) ;
  setSlot(*((tscType32 *) parameterNames[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSSLOT]]->getValueConverted())) ;
  setSubDetector((std::string)parameterNames[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSSUBDETECTOR]]->getValue()) ;
  setId((std::string)parameterNames[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSID]]->getValue()) ;
  setCrateNumber(*((tscType32 *) parameterNames[TKIDVSHOSTNAMEPARAMETERNAMES[CRATENUMBER]]->getValueConverted())) ;
}

/** Set hostname
 */
void TkIdVsHostnameDescription::setHostname ( std::string hostname ) {

  strncpy (hostname_, hostname.c_str(), 100) ;
}

/** Get hostname
 */
std::string TkIdVsHostnameDescription::getHostname ( ) {

  return std::string (hostname_) ;
}

/** Set crateId
 */
void TkIdVsHostnameDescription::setCrateId ( std::string crateId ) {

  strncpy (crateId_, crateId.c_str(), 100) ;
}

/** Get crateId
 */
std::string TkIdVsHostnameDescription::getCrateId ( ) {

  return std::string (crateId_) ;
}

/** Set subDetector
 */
void TkIdVsHostnameDescription::setSubDetector ( std::string subDetector ) {

  strncpy (subDetector_, subDetector.c_str(), 100) ;
}

/** Get subDetector
 */
std::string TkIdVsHostnameDescription::getSubDetector ( ) {

  return std::string (subDetector_) ;
}

/** Set id
 */
void TkIdVsHostnameDescription::setId ( std::string id ) {

  strncpy (id_, id.c_str(), 100) ;
}

/** Get id
 */
std::string TkIdVsHostnameDescription::getId ( ) {

  return std::string (id_) ;
}

/** Set FEC id
 */
void TkIdVsHostnameDescription::setFecId ( std::string id ) {

  strncpy (id_, id.c_str(), 100) ;
}

/** Get FEC id
 */
std::string TkIdVsHostnameDescription::getFecId ( ) {

  return std::string (id_) ;
}

/** Set FED id
 */
void TkIdVsHostnameDescription::setFedId ( unsigned int id ) {

  snprintf (id_, 100, "%u", id) ;
}

/** Get FED id
 */
unsigned int TkIdVsHostnameDescription::getFedId ( ) {

  return fromString<unsigned int>(std::string (id_)) ;
}

/** set slot
 */
void TkIdVsHostnameDescription::setSlot ( unsigned int slot ) {
  slot_ = slot ;
}

/** get slot
 */
unsigned int TkIdVsHostnameDescription::getSlot ( ) {
  return slot_ ;
}

/** brief get the crate number
 * \return crate number
 */
unsigned int TkIdVsHostnameDescription::getCrateNumber ( ) {
  return crateNumber_ ;
}

/** set the crate number 
 * \param crateNumber - crate number
 */
void TkIdVsHostnameDescription::setCrateNumber ( unsigned int crateNumber ) {
  crateNumber_ = crateNumber ;
}

/** \brief Clone TkIdVsHostnameDescription
 * \return the description cloned
 */
TkIdVsHostnameDescription *TkIdVsHostnameDescription::clone (){
  
  TkIdVsHostnameDescription  *tkIdVsHostnameDescription = new TkIdVsHostnameDescription(hostname_,crateId_,slot_,subDetector_,id_,crateNumber_);
  
  return (tkIdVsHostnameDescription) ;
}

/** \brief Display the TkIdVsHostnameDescription
 */
void TkIdVsHostnameDescription::display ( ) {

  std::cout << "Hostname : " << hostname_ << std::endl ;
  std::cout << "Crate Id: " << crateId_ << std::endl ;
  std::cout << "VME slot: " << slot_ << std::endl ;
  std::cout << "sub-detector : " << subDetector_ << std::endl ;
  std::cout << "FEC/FED id: " << id_ << std::endl ;
  std::cout << "Crate number: " << crateNumber_ << std::endl ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *TkIdVsHostnameDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;
  (*parameterNames)[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSHOSTNAME]] = new ParameterDescription(TKIDVSHOSTNAMEPARAMETERNAMES[IDVSHOSTNAME], ParameterDescription::STRING) ;
  (*parameterNames)[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSCRATEID]] = new ParameterDescription(TKIDVSHOSTNAMEPARAMETERNAMES[IDVSCRATEID], ParameterDescription::STRING) ;
  (*parameterNames)[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSSLOT]] = new ParameterDescription(TKIDVSHOSTNAMEPARAMETERNAMES[IDVSSLOT], ParameterDescription::INTEGER32) ;
  (*parameterNames)[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSSUBDETECTOR]] = new ParameterDescription(TKIDVSHOSTNAMEPARAMETERNAMES[IDVSSUBDETECTOR], ParameterDescription::STRING) ;
  (*parameterNames)[TKIDVSHOSTNAMEPARAMETERNAMES[IDVSID]] = new ParameterDescription(TKIDVSHOSTNAMEPARAMETERNAMES[IDVSID], ParameterDescription::STRING) ;
  (*parameterNames)[TKIDVSHOSTNAMEPARAMETERNAMES[CRATENUMBER]] = new ParameterDescription(TKIDVSHOSTNAMEPARAMETERNAMES[CRATENUMBER], ParameterDescription::INTEGER32) ;

  return parameterNames ;
}

/** \brief Delete a list of parameter name but only its content
 * this method is available for any delete in any description class
 */
void TkIdVsHostnameDescription::deleteParameterNames(parameterDescriptionNameType *parameterNames){

  for (parameterDescriptionNameType::iterator p = parameterNames->begin() ; p != parameterNames->end() ; p ++) {

    ParameterDescription *val = p->second ;
    delete val ;
  }
  parameterNames->clear() ;
}

/** Parameter's name
 */
const char *TkIdVsHostnameDescription::TKIDVSHOSTNAMEPARAMETERNAMES[] =  {"hostname", "crateId", "slot", "subDetector", "id", "crateNumber"} ;
