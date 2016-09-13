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

#include "laserdriverDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a Laserdriver with all the values specified:
 * \param accessKey - device key
 */
laserdriverDescription::laserdriverDescription (keyType accessKey): deviceDescription ( LASERDRIVER, accessKey ) { }

/** \brief Constructor in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param Gain - Gain for all the channels (0 < value < 3)
 * \param Bias - Bias for all the channels (1 < value < 25)
 */
laserdriverDescription::laserdriverDescription (tscType8 gain, tscType8 bias):

  deviceDescription ( LASERDRIVER ) {

  tscType8 fGain[MAXLASERDRIVERCHANNELS] ;
  tscType8 fBias[MAXLASERDRIVERCHANNELS] ;
    
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    fGain[i] = ((gain >> i*2) & 0x3) ;
    fBias[i] = bias ;
  }

  setDescriptionValues (fGain, fBias) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param fecSlot - FEC slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param Gain - Gain for all the channels (0 < value < 3)
 * \param Bias - Array of bias for all the channels (1 < value < 25)
 */
laserdriverDescription::laserdriverDescription (tscType8 gain, tscType8 bias[MAXLASERDRIVERCHANNELS]):

  deviceDescription ( LASERDRIVER ) {

  tscType8 fGain[MAXLASERDRIVERCHANNELS] ;
    
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    fGain[i] = ((gain >> i*2) & 0x3) ;
  }

  setDescriptionValues (fGain, bias) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param accessKey - key access of a device
 * \param Gain - Gain for all the channels (0 < value < 3)
 * \param Bias - Array of bias for all the channels (1 < value < 25)
 */
laserdriverDescription::laserdriverDescription ( keyType accessKey,
						 tscType8 gain, 
						 tscType8 bias[MAXLASERDRIVERCHANNELS]):

  deviceDescription ( LASERDRIVER, accessKey ) {

  tscType8 fGain[MAXLASERDRIVERCHANNELS] ;
    
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    fGain[i] = ((gain >> i*2) & 0x3) ;
  }

  setDescriptionValues (fGain, bias) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param Gain - Gain for all the channels (0 < value < 3)
 * \param Bias - Array of bias for all the channels (1 < value < 25)
 */
laserdriverDescription::laserdriverDescription ( tscType16 fecSlot,
						 tscType16 ringSlot,
						 tscType16 ccuAddress,
						 tscType16 i2cChannel,
						 tscType16 i2cAddress,
						 tscType8 gain, 
						 tscType8 bias[MAXLASERDRIVERCHANNELS]):

  deviceDescription ( LASERDRIVER, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  tscType8 fGain[MAXLASERDRIVERCHANNELS] ;
    
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    fGain[i] = ((gain >> i*2) & 0x3) ;
  }

  setDescriptionValues (fGain, bias) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param fecSlot - FEC slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param Gain - Gain for all the channels (0 < value < 3)
 * \param Bias - Array of bias for all the channels (1 < value < 25)
 */
laserdriverDescription::laserdriverDescription ( tscType16 fecSlot,
						 tscType16 ringSlot,
						 tscType16 ccuAddress,
						 tscType16 i2cChannel,
						 tscType16 i2cAddress,
						 tscType8 gain[MAXLASERDRIVERCHANNELS], 
						 tscType8 bias[MAXLASERDRIVERCHANNELS]):

  deviceDescription ( LASERDRIVER, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setDescriptionValues (gain, bias) ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param accessKey - key access of a device   
 * \param Gain0 - Gain for channel 0
 * \param Gain1 - Gain for channel 1
 * \param Gain2 - Gain for channel 2
 * \param Bias0 - Bias for channel 0
 * \param Bias1 - Bias for channel 1
 * \param Bias2 - Bias for channel 2
 */
laserdriverDescription::laserdriverDescription ( keyType accessKey,
						 tscType8 bias0,
						 tscType8 bias1,
						 tscType8 bias2,
						 tscType8 gain0,
						 tscType8 gain1,
						 tscType8 gain2
						 ):

  deviceDescription ( LASERDRIVER, accessKey ) {

  tscType8 fGain[MAXLASERDRIVERCHANNELS] ;
  tscType8 fBias[MAXLASERDRIVERCHANNELS] ;

  fGain[0] = gain0 ;
  fGain[1] = gain1 ;
  fGain[2] = gain2 ;

  fBias[0] = bias0 ;
  fBias[1] = bias1 ;
  fBias[2] = bias2 ;

  setDescriptionValues (fGain, fBias) ;
}


/** \brief Constructor in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param fecSlot - FEC slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param Gain0 - Gain for channel 0
 * \param Gain1 - Gain for channel 1
 * \param Gain2 - Gain for channel 2
 * \param Bias0 - Bias for channel 0
 * \param Bias1 - Bias for channel 1
 * \param Bias2 - Bias for channel 2
 */
laserdriverDescription::laserdriverDescription ( tscType16 fecSlot,
						 tscType16 ringSlot,
						 tscType16 ccuAddress,
						 tscType16 i2cChannel,
						 tscType16 i2cAddress,
						 tscType8 bias0,
						 tscType8 bias1,
						 tscType8 bias2,
						 tscType8 gain0,
						 tscType8 gain1,
						 tscType8 gain2
						 ):

  deviceDescription ( LASERDRIVER, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  tscType8 fGain[MAXLASERDRIVERCHANNELS] ;
  tscType8 fBias[MAXLASERDRIVERCHANNELS] ;

  fGain[0] = gain0 ;
  fGain[1] = gain1 ;
  fGain[2] = gain2 ;

  fBias[0] = bias0 ;
  fBias[1] = bias1 ;
  fBias[2] = bias2 ;

  setDescriptionValues (fGain, fBias) ;
}


/** \brief Constructor in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param Gain - Gain for all the channels (0 < value < 3)
 * \param Bias - Array of bias for all the channels (1 < value < 25)
 */
laserdriverDescription::laserdriverDescription ( tscType8 gain[MAXLASERDRIVERCHANNELS], tscType8 bias[MAXLASERDRIVERCHANNELS]):

  deviceDescription ( LASERDRIVER ) {

  setDescriptionValues (gain, bias) ;
}

/* build a description of a laserdriver with a list of parameters name
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 */
laserdriverDescription::laserdriverDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( LASERDRIVER, parameterNames) {
      
  setGain (0, *((tscType8 *) parameterNames[LASERDRIVERPARAMETERNAMES[GAIN0]]->getValueConverted())) ;
  setGain (1, *((tscType8 *) parameterNames[LASERDRIVERPARAMETERNAMES[GAIN1]]->getValueConverted())) ;
  setGain (2, *((tscType8 *) parameterNames[LASERDRIVERPARAMETERNAMES[GAIN2]]->getValueConverted())) ;
  setBias (0, *((tscType8 *) parameterNames[LASERDRIVERPARAMETERNAMES[BIAS0]]->getValueConverted())) ;
  setBias (1, *((tscType8 *) parameterNames[LASERDRIVERPARAMETERNAMES[BIAS1]]->getValueConverted())) ;
  setBias (2, *((tscType8 *) parameterNames[LASERDRIVERPARAMETERNAMES[BIAS2]]->getValueConverted())) ;
}

/** \brief Method in order to set all the parameters
 * Build a description of a laserdriver with all the values specified:
 * \param Gain - Gain for all the channels (0 < value < 3)
 * \param Bias - Array of bias for all the channels (1 < value < 25)
 */
void laserdriverDescription::setDescriptionValues ( tscType8 gain[MAXLASERDRIVERCHANNELS], 
						    tscType8 bias[MAXLASERDRIVERCHANNELS]) {

  setGain (gain) ;
  setBias (bias) ;

}

/** \brief return the value of the param
 * Return the value of laserdriver register
 * \return the value
 */
tscType8 laserdriverDescription::getGain ( ) {  
    
  tscType8 Gain[MAXLASERDRIVERCHANNELS] ;
    
  Gain[0] = gain0_ ;
  Gain[1] = gain1_ ;
  Gain[2] = gain2_ ;

  tscType8 value = 0 ;

  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {

    value |= ((Gain[i] & 0x3) << (i*2)) ;
  }

  return (value) ;
}

/** \brief return the value of the param
 * Return the value of laserdriver register for a given channel
 * \param channel - channel value
 * \return the value
 * \exception FecExceptionHandler in case of bad channel number
 */
tscType8 laserdriverDescription::getGain ( tscType8 channel ) throw (FecExceptionHandler) {

  tscType8 gain = 0 ;

  switch (channel) {
  case 0: gain = gain0_ ; break ;
  case 1: gain = gain1_ ; break ;
  case 2: gain = gain2_ ; break ;
  default:
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( CODECONSISTENCYERROR,
				       "Channel number incoherent",
				       FATALERRORCODE,
				       accessKey_, 
				       "channel", channel) ;
  }

  return (gain) ;
}

/** \brief set the gain value for a given channel
 * \param channel - channel value
 * \param gain - gain value
 */
void laserdriverDescription::setGain ( tscType8 channel, tscType8 gain) throw (FecExceptionHandler) {

  switch (channel) {
  case 0: gain0_ = gain ; break ;
  case 1: gain1_ = gain  ; break ;
  case 2: gain2_ = gain  ; break ;
  default:
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( CODECONSISTENCYERROR,
				       "Channel number incoherent",
				       FATALERRORCODE,
				       accessKey_, 
				       "channel", channel) ;
  }
}

/** \brief return the value of the param
 * Return the value of laserdriver register
 * \return the value
 */
tscType8 laserdriverDescription::getGain0 ( ) { return (gain0_) ; }

/** \brief return the value of the param
 * Return the value of laserdriver register
 * \return the value
 */
tscType8 laserdriverDescription::getGain1 ( ) { return (gain1_) ; }

/** \brief return the value of the param
 * Return the value of laserdriver register
 * \return the value
 */
tscType8 laserdriverDescription::getGain2 ( ) { return (gain2_) ; }

/** \brief Give the values of the param
 * Give the values of laserdriver register
 * \param bias - pointer to an array of bias
 */
void laserdriverDescription::getBias ( tscType8 *bias ) {  
    
  bias[0] = bias0_ ;
  bias[1] = bias1_ ;
  bias[2] = bias2_ ;
}

/** \brief Give the values of the param
 * Give the values of laserdriver register
 * \param bias - pointer to an array of bias
 */
tscType8 laserdriverDescription::getBias0 ( ) { return (bias0_) ; }

/** \brief Give the values of the param
 * Give the values of laserdriver register
 * \param bias - pointer to an array of bias
 */
tscType8 laserdriverDescription::getBias1 ( ) { return (bias1_) ; }

/** \brief Give the values of the param
 * Give the values of laserdriver register
 * \param bias - pointer to an array of bias
 */
tscType8 laserdriverDescription::getBias2 ( ) { return (bias2_) ; }

/** \brief return the value of the param
 * Return the value of laserdriver register for a given channel
 * \param channel - channel value
 * \return the value
 * \exception FecExceptionHandler in case of bad channel number
 */
tscType8 laserdriverDescription::getBias ( tscType8 channel ) throw (FecExceptionHandler) {

  tscType8 bias = 0 ;

  switch (channel) {
  case 0: bias = bias0_ ; break ;
  case 1: bias = bias1_ ; break ;
  case 2: bias = bias2_ ; break ;
  default:
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( CODECONSISTENCYERROR,
				       "Channel number incoherent",
				       FATALERRORCODE,
				       accessKey_, 
				       "channel", channel) ;
  }

  return (bias) ;
}

/** \brief set the bias value for a given channel
 * \param channel - channel value
 * \param bias - bias value
 */
void laserdriverDescription::setBias ( tscType8 channel, tscType8 bias) throw (FecExceptionHandler) {

  switch (channel) {
  case 0: bias0_ = bias ; break ;
  case 1: bias1_ = bias  ; break ;
  case 2: bias2_ = bias  ; break ;

  default:
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( CODECONSISTENCYERROR,
				       "Channel number incoherent",
				       FATALERRORCODE,
				       accessKey_, 
				       "channel", channel) ;
  }
}

/** \brief set the value for register
 * Set the value for a laserdriver description
 * \param gain - value to be set
 */
void laserdriverDescription::setGain ( tscType8 gain ) { 

  gain0_ = ((gain >> 0*2) & 0x3) ;
  gain1_ = ((gain >> 1*2) & 0x3) ;
  gain2_ = ((gain >> 2*2) & 0x3) ;

}

/** \brief set the value for register
 * Set the value for a laserdriver description
 * \param gain - array to be set
 */
void laserdriverDescription::setGain ( tscType8 gain[MAXLASERDRIVERCHANNELS] ) { 

  gain0_ = gain[0] ;
  gain1_ = gain[1] ;
  gain2_ = gain[2] ;

}

/** \brief set the value for register
 * Set the value for a laserdriver description
 * \param bias - pointer to an array of bias
 */
void laserdriverDescription::setBias ( tscType8 *Bias ) { 

  bias0_ = Bias[0] ;
  bias1_ = Bias[1] ;
  bias2_ = Bias[2] ;

}

/** \brief In order to compare laserdriver description
 * \param laserdriver - description to be compared
 * \return bool - true if one value are different, false if not
 */
bool laserdriverDescription::operator!= ( laserdriverDescription &laserdriver ) {

  bool val = true ;
    
  tscType8 bias[MAXLASERDRIVERCHANNELS] ;
  laserdriver.getBias(bias) ;

  tscType8 fBias[MAXLASERDRIVERCHANNELS] ;
  getBias(fBias) ;

  for (tscType8 i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {

    val = val && (fBias[i] != bias[i]) ;
  }

  if ((getGain ( ) != laserdriver.getGain ( )))
    return val ;
  else
    return false ;
}

/** \brief In order to compare laserdriver description
 * \param laserdriver - description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool laserdriverDescription::operator== ( laserdriverDescription &laserdriver ) {

  bool val = true ;
    
  tscType8 bias[MAXLASERDRIVERCHANNELS] ;
  laserdriver.getBias(bias) ;

  tscType8 fBias[MAXLASERDRIVERCHANNELS] ;
  getBias(fBias) ;

  for (tscType8 i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {

    val = val && (fBias[i] == bias[i]) ;
  }

  if ((getGain ( ) == laserdriver.getGain ( )))
    return val ;
  else
    return false ;
}

/** Clone a device description
 * \return the device description cloned
 */
laserdriverDescription *laserdriverDescription::clone ( ) {

  laserdriverDescription* myLaserdriverDescription =  
    new laserdriverDescription ( *this ) ;

  return myLaserdriverDescription;
}

/** \brief Display the APV values where the comparison is different
 * \param laserdriver - uploaded values
 */
void laserdriverDescription::displayDifferences ( laserdriverDescription &laserdriver ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "Laserdriver " << msg << std::endl ;
  if ((getGain ( ) != laserdriver.getGain ( )))
    std::cout << "\tGain is different (" << std::dec << (unsigned int) getGain() << " / " << (unsigned int) laserdriver.getGain ( ) << std::endl ;

  tscType8 ebias[MAXLASERDRIVERCHANNELS] ;
  laserdriver.getBias(ebias) ;

  tscType8 dBias[MAXLASERDRIVERCHANNELS] ;
  getBias(dBias) ;

  for (tscType8 i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {

    if (dBias[i] != ebias[i])
      std::cout << "\tBias " << std::dec << " is different " << (unsigned int) dBias[i] << " / " << (unsigned int) ebias[i] << std::endl ;
  }
}

/** \brief Display laserdriver values
 */
void laserdriverDescription::display ( ) {

  if (getAddress() == 0x60)
    std::cout << "Laserdriver: 0x" ;
  else
    std::cout << "DOH: 0x" ;

  std::cout << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;

  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
  std::cout << "\tGain 0: " << std::dec << (tscType16)getGain0() << std::endl ;
  std::cout << "\tGain 1: " << std::dec << (tscType16)getGain1() << std::endl ;
  std::cout << "\tGain 2: " << std::dec << (tscType16)getGain2() << std::endl ;
  std::cout << "\tBias 0: " << std::dec << (tscType16)getBias0() << std::endl ;
  std::cout << "\tBias 1: " << std::dec << (tscType16)getBias1() << std::endl ;
  std::cout << "\tBias 2: " << std::dec << (tscType16)getBias2() << std::endl ;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *laserdriverDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;

  for (unsigned int i = 0 ; i < (sizeof(LASERDRIVERPARAMETERNAMES)/sizeof(const char *)) ; i ++) {
    (*parameterNames)[LASERDRIVERPARAMETERNAMES[i]] = new ParameterDescription(LASERDRIVERPARAMETERNAMES[i],ParameterDescription::INTEGER8) ;
  }

  return parameterNames ;
}

/** Parameter's names
 */
const char *laserdriverDescription::LASERDRIVERPARAMETERNAMES[] = {"bias0",
  "bias1",
  "bias2",
  "gain0",
  "gain1",
  "gain2"} ;

