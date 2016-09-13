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

  Copyright 2006 Juha Petajajarvi -  Rovaniemi University of Applied Sciences, Rovaniemi-Finland
*/

#include "totemBBDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a TBB with all the values specified:
 * \param accessKey - device key
 */
totemBBDescription::totemBBDescription (keyType accessKey): deviceDescription ( TBB, accessKey ) { }

/** \brief Constructor in order to set all the parameters
 * Build a description of a TBB with all the values specified:
 * \param accessKey - device key
 * \param control_ - TBB Register
 * \param lat1_ - TBB Register
 * \param lat2_ - TBB Register
 * \param rep1_ - TBB Register
 * \param rep2_ - TBB Register
 */
totemBBDescription::totemBBDescription ( keyType accessKey,
					 tscType8 control_,
					 tscType8 lat1_,
					 tscType8 lat2_,
					 tscType8 rep1_,
					 tscType8 rep2_ ):

  deviceDescription ( TBB, accessKey ) {

  setControl (control_) ;
  setLatency1 (lat1_) ;
  setLatency2 (lat2_) ;
  setRepeats1 (rep1_) ;
  setRepeats2 (rep2_) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a TBB with all the values specified:
 * \param fecSlot - FEC slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param control_ - TBB Register
 * \param lat1_ - TBB Register
 * \param lat2_ - TBB Register
 * \param rep1_ - TBB Register
 * \param rep2_ - TBB Register
 */
totemBBDescription::totemBBDescription ( tscType16 fecSlot,
					 tscType16 ringSlot,
					 tscType16 ccuAddress,
					 tscType16 i2cChannel,
					 tscType16 i2cAddress,
					 tscType8 control_,
					 tscType8 lat1_,
					 tscType8 lat2_,
					 tscType8 rep1_,
					 tscType8 rep2_ ):

  deviceDescription ( TBB, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setControl (control_) ;
  setLatency1 (lat1_) ;
  setLatency2 (lat2_) ;
  setRepeats1 (rep1_) ;
  setRepeats2 (rep2_) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a TBB with all the values specified:
 * \param control_ - TBB Register
 * \param lat1_ - TBB Register
 * \param lat2_ - TBB Register
 * \param rep1_ - TBB Register
 * \param rep2_ - TBB Register
 */
totemBBDescription::totemBBDescription ( tscType8 control_,
					 tscType8 lat1_,
					 tscType8 lat2_,
					 tscType8 rep1_,
					 tscType8 rep2_ ):


  deviceDescription ( TBB ) {

  setDescriptionValues ( control_, 
			 lat1_,
			 lat2_,
			 rep1_,
			 rep2_ ) ;
}

/* \brief Constructor to build a description of a TBB with a list of parameters names
 * \param parameterNames - list of parameters
 * \see ParameterDescription class
 */
totemBBDescription::totemBBDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( CCHIP, parameterNames) {

  setControl  (*((tscType8 *) parameterNames[TBBPARAMETERNAMES[TBBCONTROL]]->getValueConverted())) ;
  setLatency1 (*((tscType8 *) parameterNames[TBBPARAMETERNAMES[TBBLATENCY1]]->getValueConverted())) ;
  setLatency2 (*((tscType8 *) parameterNames[TBBPARAMETERNAMES[TBBLATENCY2]]->getValueConverted())) ;
  setRepeats1 (*((tscType8 *) parameterNames[TBBPARAMETERNAMES[TBBREPEATS1]]->getValueConverted())) ;
  setRepeats2 (*((tscType8 *) parameterNames[TBBPARAMETERNAMES[TBBREPEATS2]]->getValueConverted())) ;
}

/** \brief Method in order to set all the parameters
 * Build a description of a TBB with all the values specified
 * \param control_ - TBB Register
 * \param lat1_ - TBB Register
 * \param lat2_ - TBB Register
 * \param rep1_ - TBB Register
 * \param rep2_ - TBB Register
 */
void totemBBDescription::setDescriptionValues ( tscType8 control_,
						tscType8 lat1_,
						tscType8 lat2_,
						tscType8 rep1_,
						tscType8 rep2_ ){

  setControl (control_) ;
  setLatency1 (lat1_) ;
  setLatency2 (lat2_) ;
  setRepeats1 (rep1_) ;
  setRepeats2 (rep2_) ;
}

//Set and Get functions - PRINCIPAL

/** \brief Return the value of the TBB register corresponding to a TBB register
 * Return the value of the TBB register corresponding to a TBB register
 * \return the value
 */
tscType8 totemBBDescription::getControl ( ) { return (control) ; }

/** \brief set the value for register
 * Set the value for a TBB description
 * \param value to be set
 */
void totemBBDescription::setControl   ( tscType8 value ) { control = value ; }
 
/** \brief Return the value of the TBB register corresponding to a TBB register
 * Return the value of the TBB register corresponding to a TBB register
 * \return the value
 */
tscType8 totemBBDescription::getLatency1 ( ) { return (lat1) ; }

/** \brief set the value for register
 * Set the value for a TBB description
 * \param value to be set
 */
void totemBBDescription::setLatency1   ( tscType8 value ) { lat1 = value ; }

/** \brief Return the value of the TBB register corresponding to a TBB register
 * Return the value of the TBB register corresponding to a TBB register
 * \return the value
 */
tscType8 totemBBDescription::getLatency2 ( ) { return (lat2) ; }

/** \brief set the value for register
 * Set the value for a TBB description
 * \param value to be set
 */
void totemBBDescription::setLatency2   ( tscType8 value ) { lat2 = value ; }

/** \brief Return the value of the TBB register corresponding to a TBB register
 * Return the value of the TBB register corresponding to a TBB register
 * \return the value
 */
tscType8 totemBBDescription::getRepeats1 ( ) { return (rep1) ; }

/** \brief set the value for register
 * Set the value for a TBB description
 * \param value to be set
 */
void totemBBDescription::setRepeats1   ( tscType8 value ) { rep1 = value ; }

/** \brief Return the value of the TBB register corresponding to a TBB register
 * Return the value of the TBB register corresponding to a TBB register
 * \return the value
 */
tscType8 totemBBDescription::getRepeats2 ( ) { return (rep2) ; }

/** \brief set the value for register
 * Set the value for a TBB description
 * \param value to be set
 */
void totemBBDescription::setRepeats2   ( tscType8 value ) { rep2 = value ; }

/** \brief In order to compare two TBB descriptions
 * \param tbb - TBB description to be compared
 * \return bool - true if one value are different, false if not
 */
bool totemBBDescription::operator!= ( totemBBDescription &tbb ) {

#ifdef DEBUGMSGERROR
  displayDifferences(tbb) ;
#endif

  if (
      (getControl ( ) != tbb.getControl ( )) ||
      (getLatency1 ( ) != tbb.getLatency1 ( )) ||
      (getLatency2 ( ) != tbb.getLatency2 ( )) ||
      (getRepeats1 ( ) != tbb.getRepeats1 ( )) ||
      (getRepeats2 ( ) != tbb.getRepeats2 ( ))
      )
    return true ;
  else
    return false ;
}

/** \brief In order to compare two TBB descriptions
 * \param tbb - TBB description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool totemBBDescription::operator  == ( totemBBDescription &tbb ) {

#ifdef DEBUGMSGERROR
  displayDifferences(tbb) ;
#endif

  if (
      (getControl ( ) == tbb.getControl ( )) &&
      (getLatency1 ( ) == tbb.getLatency1 ( )) &&
      (getLatency2 ( ) == tbb.getLatency2 ( )) &&
      (getRepeats1 ( ) == tbb.getRepeats1 ( )) &&
      (getRepeats2 ( ) == tbb.getRepeats2 ( ))
      )
    return true ;
  else
    return false ;
}


/** \briefClone a device description
 * \return the device description cloned
 */
totemBBDescription *totemBBDescription::clone ( ) {

  totemBBDescription* myTBBDescription = new totemBBDescription( *this ) ;

  return myTBBDescription;
}

/** \brief Display the TBB values where the comparison is different
 * \param tbb - TBB values uploaded
 */

void totemBBDescription::displayDifferences ( totemBBDescription &tbb ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "TBB " << msg << std::endl ;
  if (getControl ( ) != tbb.getControl ( )) 
    std::cout << "\tCont.Reg is different " << std::dec << (int)getControl ( ) << "/" << (int)tbb.getControl ( ) << std::endl ;
  if (getLatency1 ( ) != tbb.getLatency1 ( )) 
    std::cout << "\tLatency<1> is different " << std::dec << (int)getLatency1 ( ) << "/" << (int)tbb.getLatency1 ( ) << std::endl ;
  if (getLatency2 ( ) != tbb.getLatency2 ( )) 
    std::cout << "\tLatency<2> is different " << std::dec << (int)getLatency2 ( ) << "/" << (int)tbb.getLatency2 ( ) << std::endl ;
  if (getRepeats1 ( ) != tbb.getRepeats1 ( )) 
    std::cout << "\tRepeats<1> is different " << std::dec << (int)getRepeats1 ( ) << "/" << (int)tbb.getRepeats1 ( ) << std::endl ;
  if (getRepeats2 ( ) != tbb.getRepeats2 ( )) 
    std::cout << "\tRepeats<2> is different " << std::dec << (int)getRepeats2 ( ) << "/" << (int)tbb.getRepeats2 ( ) << std::endl ;
}
  
/** \brief Display the TBB values
 */
void totemBBDescription::display ( ) {

  std::cout << "TBB: 0x" 
	    << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;
  std::cout << std::endl ;

  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
   
  std::cout << "\tCont.Reg: " << std::dec << (tscType16)getControl() << std::endl ;
  std::cout << "\tLatency<1>: " << std::dec << (tscType16)getLatency1() << std::endl ;
  std::cout << "\tLatency<2>: " << std::dec << (tscType16)getLatency2() << std::endl ;
  std::cout << "\tRepeats<1>: " << std::dec << (tscType16)getRepeats1() << std::endl ;
  std::cout << "\tRepeats<2>: " << std::dec << (tscType16)getRepeats2() << std::endl ;
  std::cout << std::endl ;
}

/** \brief Return a list of parameter names
 * \warning Must be delete by the owner of the description
 */
parameterDescriptionNameType *totemBBDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;

  for (unsigned int i = 0 ; i < (sizeof(TBBPARAMETERNAMES)/sizeof(const char *)) ; i ++)
    {
      (*parameterNames)[TBBPARAMETERNAMES[i]] = new ParameterDescription(TBBPARAMETERNAMES[i], ParameterDescription::INTEGER8) ;
    }

  return parameterNames ;
}
 
/** Parameter's names
 */
const char *totemBBDescription::TBBPARAMETERNAMES[] = { "tbbcontrol",
							"tbblatency1",
							"tbblatency2",
							"tbbrepeats1",
							"tbbrepeats2" } ;

