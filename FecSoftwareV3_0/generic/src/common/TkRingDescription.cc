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
#include <list>

#include "TkRingDescription.h"
#include "deviceType.h"

/** Set all the channel to not initialise
 * \brief Constructor that create a Ring
 */
TkRingDescription::TkRingDescription ( tscType16 crateId, keyType key, bool enabled, bool inAused, bool outAused) {

  crateId_   = crateId ;
  key_         = key ;
  enabled_     = enabled ;
  inputAUsed_  = inAused ;
  outputAUsed_ = outAused ;

}

/** \brief Create a CCU description from XML buffer
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 * \warning an exception is raised if one the parameter is not correct
 */
TkRingDescription::TkRingDescription ( parameterDescriptionNameType parameterNames ): crateId_(0) {

  setFecHardwareId ((std::string)parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID]]->getValue(),
		    *((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID]]->getValueConverted())) ;
  //setCrateId (*((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID]]->getValueConverted())) ;

  key_ = buildCompleteKey(*((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT]]->getValueConverted()),
			  *((tscType16 *) parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT]]->getValueConverted()),
			  0, 0, 0) ;

  // enabled setting:
  // "0" can be used in place of "F" (false)
  // "1" can be used in place of "T" (true)
  std::string enabled = (std::string)parameterNames[deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED]]->getValue() ;
  if ( enabled == "0" ) enabled = "F";
  if ( enabled == "1" ) enabled = "T";

  bool tempEnable=true;
  if ( enabled == "F" ) tempEnable=false;
  setEnabled(tempEnable); 

  std::string inoutAUsed ; 
  inputAUsed_ = true ;
  outputAUsed_ = true ;

  inoutAUsed = (std::string)parameterNames[RINGPARAMETERNAMES[INPUTAUSED]]->getValue() ;
  if ( inoutAUsed == "F" ) inputAUsed_ = false;
  inoutAUsed = (std::string)parameterNames[RINGPARAMETERNAMES[OUTPUTAUSED]]->getValue() ;
  if ( inoutAUsed == "F" ) outputAUsed_ = false;

}

/** Copy constructor defined here explicitely and should be due to the vector of pointer on CCUs
 */
TkRingDescription::TkRingDescription ( TkRingDescription &sourceRing ) {
  
  inputAUsed_  = sourceRing.getInputAUsed();
  outputAUsed_ = sourceRing.getOutputAUsed();
  enabled_     = sourceRing.getEnabled();
  crateId_   = sourceRing.getCrateId();
  key_         = sourceRing.getKey();
  dummyCcu_    = sourceRing.getDummyCcu();
  memset( fecHardwareId_, '\0', 100); // Really needed to get a properly-ended string
  sourceRing.getFecHardwareId().copy(fecHardwareId_, 99);
    
  ccuVector::iterator cIt;
  CCUDescription* ccuD;
  ccuVector* sourceVector = sourceRing.getCcuVector();
  for (cIt = sourceVector->begin(); cIt != sourceVector->end(); cIt++) {
    ccuD = (*cIt);
    ccuVector_.push_back(ccuD->clone());
  }
}
  
/** Destructor, destroy the vector of CCUs
 */
TkRingDescription::~TkRingDescription ( ) {

  clearCcuVector() ;
}


/** \brief Sets the CCUDescription vector
 */
void TkRingDescription::setCcuVector (ccuVector &newCcuVector) {
  clearCcuVector();
  ccuVector::iterator ccuit;

  if (newCcuVector.size()) {
    for (ccuit=newCcuVector.begin(); ccuit!=newCcuVector.end(); ccuit++) {
      ccuVector_.push_back(*ccuit);
    }
    std::sort(ccuVector_.begin(), ccuVector_.end(), CCUDescription::sortByOrder) ;
    
    ccuit = ccuVector_.end();
    ccuit--;
    
    if ((((*ccuit)->getOrder()) & DUMMYCCUARRANGEMENT) == DUMMYCCUARRANGEMENT ) { 
      dummyCcu_ = (*ccuit);
    } else {
      dummyCcu_ = NULL;
    }
  }
  else {
    dummyCcu_ = NULL ;
  }
}

/* \brief retrives the (supposedly ordered) vector of
 * CCUDescription in the ring
 */
ccuVector* TkRingDescription::getCcuVector( ) {
  // TODO: maybe decide if it's worth to order it here.
  return (&ccuVector_);
}

/* \brief checks if the dummy ccu connection is of TIB inverted type
 */
bool TkRingDescription::isDummyInverted( ) {
  if (dummyCcu_) {
    return ((dummyCcu_->getOrder()& TIBINVERTEDDUMMY)==TIBINVERTEDDUMMY);
  } else {
    return false;
  }
}

/** \brief Clone a tkring description
 * \return the tkring cloned
 */
TkRingDescription *TkRingDescription::clone ( ) {
 
  TkRingDescription* myTkRingDescription = new TkRingDescription ( *this ) ;

  return myTkRingDescription;
}

/** \brief Return the current enabled parameter 
 * \return enabled
 */
bool TkRingDescription::getEnabled ( ) {
  return (enabled_) ;
}

/** Return the address of the tkring 
 * \return tkring address
 */
keyType TkRingDescription::getKey ( ) {
  return (key_) ;
}

/** \brief return the crate slot
 * \return the crate slot
 */
tscType16 TkRingDescription::getCrateId ( ) {
  return (crateId_) ;
}

/** \brief set the crate ID
 * \param crateId - ID of the crate to be set
 */
void TkRingDescription::setCrateId ( tscType16 crateId ) {
  crateId_ = crateId ;
}

/** \brief Set the address of the ring 
 * \param address - start from 0x1 to 0x7f
 */
void TkRingDescription::setKey ( keyType key ) {
  key_ = key ;
}

/** \brief Set the value of Enabled 
 * \param enabled (boolean)
 */
void TkRingDescription::setEnabled ( bool enabled ) {
  enabled_ = enabled ;
}

/** \brief Set the value of InputAUsed 
 * \param used (boolean) = true
 */
void TkRingDescription::setInputAUsed ( bool used ) {
  inputAUsed_ = used ;
}

/** \brief Set the value of outputAUsed 
 * \param used (boolean) = true
 */
void TkRingDescription::setOutputAUsed ( bool used ) {
  outputAUsed_ = used ;
}

/** \brief Set the value of InputAUsed 
 * opposite to the given value
 * \param used (boolean) = true
 */
void TkRingDescription::setInputBUsed ( bool used ) {
  inputAUsed_ = (! used) ;
}

/** \brief Set the value of outputAUsed 
 * opposite to the given value
 * \param used (boolean) = true
 */
void TkRingDescription::setOutputBUsed ( bool used ) {
  outputAUsed_ = (! used) ;
}

/** \brief Returns the value of InputAUsed 
 */
bool TkRingDescription::getInputAUsed () {
  return inputAUsed_ ;
}

/** \brief Returns the value of OutputAUsed 
 */
bool TkRingDescription::getOutputAUsed () {
  return outputAUsed_ ;
}

/** \brief Returns the opposite of the value of InputAUsed 
 */
bool TkRingDescription::getInputBUsed () {
  return ( ! inputAUsed_ ) ;
}

/** \brief Returns the opposite of the value of OutputAUsed 
 */
bool TkRingDescription::getOutputBUsed () {
  return ( ! outputAUsed_ ) ;
}

/** \brief Tells if the current configuration is possible
 */
unsigned int TkRingDescription::getNumberOfCcus() {
  return ( ccuVector_.size() );
}

bool TkRingDescription::isReconfigurable() {
  whyNotReconfigurable_ = "";
  //ccuVector::iterator ccuit;
  CCUDescription *thisCcu;
  CCUDescription *nextCcu;
  int nNormalCCUs;

  nNormalCCUs = getNumberOfCcus();
  if (dummyCcu_) nNormalCCUs--;

  // Now we will check that there is no condition
  // forbidding the ring reconfiguration

  // We need at lease 3 CCUs in a ring
  if (nNormalCCUs<3) {
    whyNotReconfigurable_ = "Number of regular CCUs in ring is two or less";
    return false;
  }

  // No two consecutive disabled ccus in the ring please
  for (int i=0; i<nNormalCCUs-1; i++) {
    thisCcu = ccuVector_[i];
    nextCcu = ccuVector_[i+1];
    if (! (thisCcu->getEnabled()||nextCcu->getEnabled()) ) {
      whyNotReconfigurable_ = "Two consecutive CCUs are marked 'disabled'";
      return false;
    }
  }

  // If we use ring input A, then the first CCU must be enabled
  if (getInputAUsed()&&(!ccuVector_[0]->getEnabled())) {
    whyNotReconfigurable_ = "FEC A output is selected, but the first CCU is disabled";
    return false;
  }

  // If we use ring output A, then the last normal CCU must be enabled
  // and the dummy must be disabled
  if (getOutputAUsed()) {
    if (!ccuVector_[nNormalCCUs-1]->getEnabled()) {
      whyNotReconfigurable_ = "FEC A input is selected, but the last CCU is disabled";
      return false;
    }
    if ((dummyCcu_)&&(dummyCcu_->getEnabled())) {
      whyNotReconfigurable_ = "FEC A input is selected, but the dummy CCU is enabled";
      return false;
    }
  }

  // If we are using the ring B output, then the Dummy CCU is compulsory
  if (getOutputBUsed()) {
    if (!dummyCcu_) {
      whyNotReconfigurable_ = "FEC B input is selected, but the dummy CCU is not present";
      return false;
    }
    if (!dummyCcu_->getEnabled()) {
      whyNotReconfigurable_ = "FEC B input is selected, but the dummy CCU is disabled";
      return false;
    }
  }

  // If we are skipping the last CCU we need the ring output B
  if (!(ccuVector_[nNormalCCUs-1]->getEnabled() )) {
    if (!getOutputBUsed()) {
      whyNotReconfigurable_ = "One of the last two CCUs is disabled, but the FEC input B is not selected";
      return false;
    }
  }

  return true;
}


/** \brief Computes the reconfiguration path if possible
 *  otherwise it returns false
 */
bool TkRingDescription::computeRedundancy() {

  // Try to compute the redundancy... only if it's possible!
  if (!isReconfigurable()) return false;
  
  // Count CCUs and clean the redundancy commands foreseen
  int nNormalCCUs;
  nNormalCCUs = getNumberOfCcus();
  if (dummyCcu_) {
    nNormalCCUs--;
    dummyCcu_->setInputA();
    dummyCcu_->setOutputA();
  }
  for (int i=0; i<nNormalCCUs; i++) {
    ccuVector_[i]->setInputA();
    ccuVector_[i]->setOutputA();
  }

  if (getInputBUsed()) {
    if (ccuVector_[0]->getEnabled()) {
    	ccuVector_[0]->setInputB();
    } else {
    	ccuVector_[1]->setInputB();
    }
  }

  for (int i=0; i<nNormalCCUs-2; i++) {
    if (!(ccuVector_[i+1]->getEnabled())) {
    	ccuVector_[i]->setOutputB();
    	ccuVector_[i+2]->setInputB();
    }
  }
  
  if (!isDummyInverted()) {
    if (!ccuVector_[nNormalCCUs-1]->getEnabled()) {
      ccuVector_[nNormalCCUs-2]->setOutputB();
      dummyCcu_->setInputB();
      setOutputBUsed(); // TODO: this line should be useless!
    } else {
      if (getOutputBUsed()) {
        ccuVector_[nNormalCCUs-1]->setOutputB();
      }
    }
  } else {
    if (!ccuVector_[nNormalCCUs-1]->getEnabled()) {
      ccuVector_[nNormalCCUs-2]->setOutputB();
      setOutputBUsed(); // TODO: this line should be useless!
    } else {
      if (getOutputBUsed()) {
        ccuVector_[nNormalCCUs-1]->setOutputB();
        dummyCcu_->setInputB();
      }
    }
  }

  return true;
}


// ***********************************************************************************************
// FEC hardware ID
// ***********************************************************************************************

/** \brief Set the FEC hardware identification number
 */
void TkRingDescription::setFecHardwareId ( std::string fecHardwareId, tscType16 crateId ) {
  strncpy (fecHardwareId_, fecHardwareId.c_str(), 100) ;
  crateId_ = crateId ;
  //fecHardwareId_ = fecHardwareId ;
}

/** \brief return the FEC hardware identification number
 */
std::string TkRingDescription::getFecHardwareId ( ) {
  return (fecHardwareId_) ;
}

// ***********************************************************************************************
// Display
// ***********************************************************************************************

/** Display the reconfiguration in a stream
 * \param flux - ostream 
 * \param all - display all CCUs or only the CCUs which are in the ring (so not the one in bypass)
 */

void TkRingDescription::display ( std::ostream &flux, bool all /* = false */ , bool redundancyDetails /*=false=*/) {

  if (!ccuVector_.size()) flux << "No CCUs in the ring" ;
  else {
    if (!enabled_) flux << "Ring disbled: " ;
    flux << "FEC->" << (inputAUsed_ ? "A " : "B ");
    for (std::vector<CCUDescription *>::iterator it = ccuVector_.begin() ; it != ccuVector_.end() ; it ++) {
      if ((*it)->getEnabled()||(all)) {
	flux << "CCU_" << std::dec << getCcuKey((*it)->getKey());
 	if (redundancyDetails) {
 	  flux << "-" << ((*it)->getInputA() ? "A" : "B")
	       << "-" << ((*it)->getOutputA() ? "A" : "B");
 	}
	flux << " " ;
      }
    }
    flux << (outputAUsed_ ? "A" : "B") << "->FEC" ;
  }
}

// ***********************************************************************************************
// Static method
// ***********************************************************************************************

/** Return the dummy CCU
 */
CCUDescription* TkRingDescription::getDummyCcu() { return dummyCcu_ ; }

/** Clear the vector of CCUs
 */
void TkRingDescription::clearCcuVector ( ) {
  
  for (ccuVector::iterator ccuit=ccuVector_.begin(); ccuit!=ccuVector_.end(); ccuit++) {
    delete (*ccuit);
  }
  ccuVector_.clear();
  dummyCcu_ = NULL;
}


/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *TkRingDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;

  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID], ParameterDescription::STRING) ;
  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID], ParameterDescription::INTEGER16) ;
  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT], ParameterDescription::INTEGER16) ;
  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT], ParameterDescription::INTEGER16) ;
  (*parameterNames)[deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED]] = new ParameterDescription(deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED], ParameterDescription::STRING) ;

  (*parameterNames)[RINGPARAMETERNAMES[INPUTAUSED]] = new ParameterDescription(RINGPARAMETERNAMES[INPUTAUSED], ParameterDescription::STRING) ;
  (*parameterNames)[RINGPARAMETERNAMES[OUTPUTAUSED]] = new ParameterDescription(RINGPARAMETERNAMES[OUTPUTAUSED], ParameterDescription::STRING) ;

  return parameterNames ;
}

/** \brief Delete a list of parameter name but only its content
 * \warning the parameter parameterNames is deleted by this method
 */
void TkRingDescription::deleteParameterNames(parameterDescriptionNameType *parameterNames){
  deviceDescription::deleteParameterNames ( parameterNames ) ;
}

/** Parameter's name
 */
const char *TkRingDescription::RINGPARAMETERNAMES[] = {"inputAUsed", "outputAUsed"} ;

/** Number of parameters
 */ 
const unsigned int TkRingDescription::RINGNUMBEROFPARAMETERS = 7;

