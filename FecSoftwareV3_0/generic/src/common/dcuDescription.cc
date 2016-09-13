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

#include "dcuDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a DCU with all the values specified:
 * \param accessKey - device key
 */
dcuDescription::dcuDescription (keyType accessKey, std::string dcuType): deviceDescription ( DCU, accessKey ) { 

  setDcuType(dcuType) ;
  dcuReadoutEnabled_ = true ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of an APV with all the values specified:
 * \param accessKey - device key
 * \param timeStamp - timeStamp (0 is not set)
 * \param dcuHardId - hardware id set for the DCU X
 * \param chan0 - DCU channel 0
 * \param chan1 - DCU channel 1
 * \param chan2 - DCU channel 2
 * \param chan3 - DCU channel 3
 * \param chan4 - DCU channel 4
 * \param chan5 - DCU channel 5
 * \param chan6 - DCU channel 6
 * \param chan7 - DCU channel 7
 */
dcuDescription::dcuDescription (keyType accessKey,
				tscType32 timeStamp,
				tscType32 dcuHardId,
				tscType16 chan0,
				tscType16 chan1,
				tscType16 chan2,
				tscType16 chan3,
				tscType16 chan4,
				tscType16 chan5,
				tscType16 chan6,
				tscType16 chan7,
				std::string dcuType):

  deviceDescription ( DCU, accessKey ) {

  setDescriptionValues (timeStamp, dcuHardId,  
			chan0, chan1, 
			chan2, chan3, 
			chan4, chan5, 
			chan6, chan7,
			dcuType) ;
  dcuReadoutEnabled_ = true ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a DCU with all the values specified:
 * \param fecSlot - FEC slot
 * \param ringSlot - ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - i2c address
 * \param timeStamp - timeStamp (0 is not set)
 * \param dcuHardId - hardware id set for the DCU X
 * \param chan0 - DCU channel 0
 * \param chan1 - DCU channel 1
 * \param chan2 - DCU channel 2
 * \param chan3 - DCU channel 3
 * \param chan4 - DCU channel 4
 * \param chan5 - DCU channel 5
 * \param chan6 - DCU channel 6
 * \param chan7 - DCU channel 7
 */
dcuDescription::dcuDescription (tscType16 fecSlot,
				tscType16 ringSlot,
				tscType16 ccuAddress,
				tscType16 i2cChannel,
				tscType16 i2cAddress,
				tscType32 timeStamp,
				tscType32 dcuHardId,
				tscType16 chan0,
				tscType16 chan1,
				tscType16 chan2,
				tscType16 chan3,
				tscType16 chan4,
				tscType16 chan5,
				tscType16 chan6,
				tscType16 chan7,
				std::string dcuType):

  deviceDescription ( DCU, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setDescriptionValues (timeStamp, dcuHardId,  
			chan0, chan1, 
			chan2, chan3, 
			chan4, chan5, 
			chan6, chan7, 
			dcuType) ;
  dcuReadoutEnabled_ = true ;
}

/** \brief Constructor in order to set all the parameters
 * Build a description of a DCU with all the values specified:
 * \param timeStamp - timeStamp (0 is not set)
 * \param dcuHardId - Not used for the moment, Hardware address
 * \param chan0 - DCU channel 0
 * \param chan1 - DCU channel 1
 * \param chan2 - DCU channel 2
 * \param chan3 - DCU channel 3
 * \param chan4 - DCU channel 4
 * \param chan5 - DCU channel 5
 * \param chan6 - DCU channel 6
 * \param chan7 - DCU channel 7
 */
dcuDescription::dcuDescription (tscType32 timeStamp,
				tscType32 dcuHardId,
				tscType16 chan0,
				tscType16 chan1,
				tscType16 chan2,
				tscType16 chan3,
				tscType16 chan4,
				tscType16 chan5,
				tscType16 chan6,
				tscType16 chan7,
				std::string dcuType):

  deviceDescription ( DCU ) {

  setDescriptionValues (timeStamp, dcuHardId, 
			chan0, chan1, 
			chan2, chan3, 
			chan4, chan5, 
			chan6, chan7,
			dcuType) ;
  dcuReadoutEnabled_ = true ;
}

/* build a description of a DCU with a list of parameters name
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 */
dcuDescription::dcuDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( DCU, parameterNames) {

  setTimeStamp   (*((tscType32 *) parameterNames[DCUPARAMETERNAMES[DCUTIMESTAMP]]->getValueConverted())) ;
  setDcuHardId   (*((tscType32 *) parameterNames[DCUPARAMETERNAMES[DCUHARDID]]->getValueConverted())) ;
  setDcuChannel0 (*((tscType16 *) parameterNames[DCUPARAMETERNAMES[CHANNEL0]]->getValueConverted())) ;
  setDcuChannel1 (*((tscType16 *) parameterNames[DCUPARAMETERNAMES[CHANNEL1]]->getValueConverted())) ;
  setDcuChannel2 (*((tscType16 *) parameterNames[DCUPARAMETERNAMES[CHANNEL2]]->getValueConverted())) ;
  setDcuChannel3 (*((tscType16 *) parameterNames[DCUPARAMETERNAMES[CHANNEL3]]->getValueConverted())) ;
  setDcuChannel4 (*((tscType16 *) parameterNames[DCUPARAMETERNAMES[CHANNEL4]]->getValueConverted())) ;
  setDcuChannel5 (*((tscType16 *) parameterNames[DCUPARAMETERNAMES[CHANNEL5]]->getValueConverted())) ;
  setDcuChannel6 (*((tscType16 *) parameterNames[DCUPARAMETERNAMES[CHANNEL6]]->getValueConverted())) ;
  setDcuChannel7 (*((tscType16 *) parameterNames[DCUPARAMETERNAMES[CHANNEL7]]->getValueConverted())) ;
  setDcuType((std::string)parameterNames[DCUPARAMETERNAMES[EDCUTYPE]]->getValue()) ;

  if ((std::string)parameterNames[DCUPARAMETERNAMES[DCUREADOUTENABLED]]->getValue() == STRFALSE) setDcuReadoutEnabled(false) ;
  else setDcuReadoutEnabled(true) ;
}

/** \brief Method in order to set all the parameters
 * Build a description of a DCU with all the values specified:
 * \param timeStamp - timeStamp (0 is not set)
 * \param dcuHardId - Not used for the moment, Hardware address
 * \param chan0 - DCU channel 0
 * \param chan1 - DCU channel 1
 * \param chan2 - DCU channel 2
 * \param chan3 - DCU channel 3
 * \param chan4 - DCU channel 4
 * \param chan5 - DCU channel 5
 * \param chan6 - DCU channel 6
 * \param chan7 - DCU channel 7
 */
void dcuDescription::setDescriptionValues (tscType32 timeStamp,
					   tscType32 dcuHardId,
					   tscType16 chan0,
					   tscType16 chan1,
					   tscType16 chan2,
					   tscType16 chan3,
					   tscType16 chan4,
					   tscType16 chan5,
					   tscType16 chan6,
					   tscType16 chan7,
					   std::string dcuType ) {
  setTimeStamp   (timeStamp) ;
  setDcuHardId   (dcuHardId) ;
  setDcuChannel0 (chan0) ;
  setDcuChannel1 (chan1) ;
  setDcuChannel2 (chan2) ;
  setDcuChannel3 (chan3) ;
  setDcuChannel4 (chan4) ;
  setDcuChannel5 (chan5) ;
  setDcuChannel6 (chan6) ;
  setDcuChannel7 (chan7) ;
  setDcuType     (dcuType) ;
}

/** \brief Return the enable/disable in the DCU readout
 * \return true if the DCU can be used for the DCU readout
 */
bool dcuDescription::getDcuReadoutEnabled ( ) { return (dcuReadoutEnabled_) ; }

/** \brief Return the enable/disable in the DCU readout
 * \return true if the DCU can be used for the DCU readout
 * \see getEnabledDcuReadout (same method)
 */
bool dcuDescription::isDcuReadoutEnabled ( ) { return getDcuReadoutEnabled ( ) ; }

/** \brief set the DCU to be used or not in the DCU readout
 * param dcuReadoutEnabled - enable or disable in the DCU readout
 */
void dcuDescription::setDcuReadoutEnabled ( bool dcuReadoutEnabled ) { dcuReadoutEnabled_ = dcuReadoutEnabled ; }

/** \brief returns the DCU Hardware ID
 * \return the value
 */
tscType32 dcuDescription::getDcuHardId ( ) { return (dcuHardId_) ; }

/** \brief return the DCU type
 */
std::string dcuDescription::getDcuType ( ) { return (std::string(dcuType_)) ; }

/** \brief not used for the moment, returns the DCU Hardware ID
 * \returns the value
 */
tscType32 dcuDescription::getTimeStamp ( ) { return (timeStamp_) ; }

/** \brief returns the DCU channel value corresponding to the channel
 * \param channel - DCU channel 
 */
tscType16 dcuDescription::getDcuChannel( int channel ) {

  switch (channel) {
  case 0:
    return channel0_ ;
    break ;
  case 1:
    return channel1_ ;
    break ;
  case 2:
    return channel2_ ;
    break ;
  case 3:
    return channel3_ ;
    break ;
  case 4:
    return channel4_ ;
    break ;
  case 5:
    return channel5_ ;
    break ;
  case 6:
    return channel6_ ;
    break ;
  case 7:
    return channel7_ ;
    break ;
  }

  return 0 ;
}

/** \brief returns the DCU channel value corresponding to this description
 * \returns the value
 */
tscType16 dcuDescription::getDcuChannel0 ( ) { return (channel0_) ; }
/** \brief returns the DCU channel value corresponding to this description
 * \returns the value
 */
tscType16 dcuDescription::getDcuChannel1 ( ) { return (channel1_) ; }
/** \brief returns the DCU channel value corresponding to this description
 * \returns the value
 */
tscType16 dcuDescription::getDcuChannel2 ( ) { return (channel2_) ; }
/** \brief returns the DCU channel value corresponding to this description
 * \returns the value
 */
tscType16 dcuDescription::getDcuChannel3 ( ) { return (channel3_) ; }
/** \brief returns the DCU channel value corresponding to this description
 * \returns the value
 */
tscType16 dcuDescription::getDcuChannel4 ( ) { return (channel4_) ; }
/** \brief returns the DCU channel value corresponding to this description
 * \returns the value
 */
tscType16 dcuDescription::getDcuChannel5 ( ) { return (channel5_) ; }
/** \brief returns the DCU channel value corresponding to this description
 * \returns the value
 */
tscType16 dcuDescription::getDcuChannel6 ( ) { return (channel6_) ; }
/** \brief returns the DCU channel value corresponding to this description
 * \returns the value
 */
tscType16 dcuDescription::getDcuChannel7 ( ) { return (channel7_) ; }

/** \brief set the timeStamp
 */
void dcuDescription::setTimeStamp ( tscType32 timeStamp ) { timeStamp_ = timeStamp ; }

/** \brief set the hardware id
 */
void dcuDescription::setDcuHardId ( tscType32 dcuHardId ) { dcuHardId_ = dcuHardId ; }

/** \brief set the DCU type
 */
void dcuDescription::setDcuType ( std::string dcuType ) { strncpy (dcuType_, dcuType.c_str(), 4) ; }

/** \brief set a parameter in the corresponding channel
 */
void dcuDescription::setDcuChannel0 ( tscType16 chan0 ) { channel0_ = chan0 ; }
/** \brief set a parameter in the corresponding channel
 */
void dcuDescription::setDcuChannel1 ( tscType16 chan1 ) { channel1_ = chan1 ; }
/** \brief set a parameter in the corresponding channel
 */
void dcuDescription::setDcuChannel2 ( tscType16 chan2 ) { channel2_ = chan2 ; }
/** \brief set a parameter in the corresponding channel
 */
void dcuDescription::setDcuChannel3 ( tscType16 chan3 ) { channel3_ = chan3 ; }
/** \brief set a parameter in the corresponding channel
 */
void dcuDescription::setDcuChannel4 ( tscType16 chan4 ) { channel4_ = chan4 ; }
/** \brief set a parameter in the corresponding channel
 */
void dcuDescription::setDcuChannel5 ( tscType16 chan5 ) { channel5_ = chan5 ; }
/** \brief set a parameter in the corresponding channel
 */
void dcuDescription::setDcuChannel6 ( tscType16 chan6 ) { channel6_ = chan6 ; }
/** \brief set a parameter in the corresponding channel
 */
void dcuDescription::setDcuChannel7 ( tscType16 chan7 ) { channel7_ = chan7 ; }

/** \brief In order to compare two values
 */
bool dcuDescription::operator != ( dcuDescription &dcu ) {

#ifdef DEBUGMSGERROR
  displayDifferences(dcu) ;
#endif    

  if ((getDcuChannel0( ) != dcu.getDcuChannel0( )) || (getDcuChannel1 ( ) != dcu.getDcuChannel1 ( )) ||
      (getDcuChannel1( ) != dcu.getDcuChannel1( )) || (getDcuChannel3 ( ) != dcu.getDcuChannel3 ( )) ||
      (getDcuChannel4( ) != dcu.getDcuChannel4( )) || (getDcuChannel5 ( ) != dcu.getDcuChannel5 ( )) ||
      (getDcuChannel6( ) != dcu.getDcuChannel6( )) || (getDcuChannel7 ( ) != dcu.getDcuChannel7 ( )) ||
      (getDcuHardId( ) != dcu.getDcuHardId( )))
    return true ;
  else
    return false ;

}


/** \brief In order to compare two values
 * \param dcu - uploaded values
 * \return bool - true if all values are equals, false if not
 */
bool dcuDescription::operator== ( dcuDescription &dcu ) {

#ifdef DEBUGMSGERROR
  displayDifferences(dcu) ;
#endif    

  if ((getDcuChannel0( ) == dcu.getDcuChannel0( )) && (getDcuChannel1 ( ) == dcu.getDcuChannel1 ( )) &&
      (getDcuChannel1( ) == dcu.getDcuChannel1( )) && (getDcuChannel3 ( ) == dcu.getDcuChannel3 ( )) &&
      (getDcuChannel4( ) == dcu.getDcuChannel4( )) && (getDcuChannel5 ( ) == dcu.getDcuChannel5 ( )) &&
      (getDcuChannel6( ) == dcu.getDcuChannel6( )) && (getDcuChannel7 ( ) == dcu.getDcuChannel7 ( )) &&
      (getDcuHardId( ) == dcu.getDcuHardId( )))
    return true ;
  else
    return false ;

}

/** Clone an apv description
 * \return the apvDescription cloned
 */
dcuDescription *dcuDescription::clone ( ) {

  dcuDescription* myDcuDescription = new dcuDescription ( *this ) ;

  return myDcuDescription;
}
  
/** \brief return true if the level of the difference is > than the param sent
 * \param dcu - values to be cheched
 * \param level - absolute value of difference
 */
bool dcuDescription::isLevelDifferent ( dcuDescription dcu, long level ) {
    
#ifdef DEBUGMSGERROR
  displayDifferences(dcu) ;
#endif    

  if (getDcuHardId( ) != dcu.getDcuHardId( )) return true ;

  long val0 = getDcuChannel0( ) - dcu.getDcuChannel0( ) ; val0 = val0 > 0 ? val0 : -val0 ;
  long val1 = getDcuChannel1( ) - dcu.getDcuChannel1( ) ; val1 = val1 > 0 ? val1 : -val1 ;
  long val2 = getDcuChannel2( ) - dcu.getDcuChannel2( ) ; val2 = val2 > 0 ? val2 : -val2 ;
  long val3 = getDcuChannel3( ) - dcu.getDcuChannel3( ) ; val3 = val3 > 0 ? val3 : -val3 ;
  long val4 = getDcuChannel4( ) - dcu.getDcuChannel4( ) ; val4 = val4 > 0 ? val4 : -val4 ;
  long val5 = getDcuChannel5( ) - dcu.getDcuChannel5( ) ; val5 = val5 > 0 ? val5 : -val5 ;
  long val6 = getDcuChannel6( ) - dcu.getDcuChannel6( ) ; val6 = val6 > 0 ? val6 : -val6 ;
  long val7 = getDcuChannel7( ) - dcu.getDcuChannel7( ) ; val7 = val7 > 0 ? val7 : -val7 ;

#ifdef DEBUGMSGERROR    
  std::cout << "DCU hardware ID " << std::hex << getDcuHardId() << std::endl; 
  if (val0 > level) std::cout << std::dec << "Difference in channel 0: " << val0 << "("<< getDcuChannel0( ) << "/" << dcu.getDcuChannel0( ) << ")"<< std::endl ;
  if (val1 > level) std::cout << std::dec << "Difference in channel 1: " << val1 << "("<< getDcuChannel1( ) << "/" << dcu.getDcuChannel1( ) << ")"<< std::endl ;
  if (val2 > level) std::cout << std::dec << "Difference in channel 2: " << val2 << "("<< getDcuChannel2( ) << "/" << dcu.getDcuChannel2( ) << ")"<< std::endl ;
  if (val3 > level) std::cout << std::dec << "Difference in channel 3: " << val3 << "("<< getDcuChannel3( ) << "/" << dcu.getDcuChannel3( ) << ")"<< std::endl ;
  if (val4 > level) std::cout << std::dec << "Difference in channel 4: " << val4 << "("<< getDcuChannel4( ) << "/" << dcu.getDcuChannel4( ) << ")"<< std::endl ;
  if (val5 > level) std::cout << std::dec << "Difference in channel 5: " << val5 << "("<< getDcuChannel5( ) << "/" << dcu.getDcuChannel5( ) << ")"<< std::endl ;
  if (val6 > level) std::cout << std::dec << "Difference in channel 6: " << val6 << "("<< getDcuChannel6( ) << "/" << dcu.getDcuChannel6( ) << ")"<< std::endl ;
  if (val7 > level) std::cout << std::dec << "Difference in channel 7: " << val7 << "("<< getDcuChannel7( ) << "/" << dcu.getDcuChannel7( ) << ")"<< std::endl ;
#endif

  if ((val0 > level) || (val1 > level) || (val2 > level) || (val3 > level) ||
      (val4 > level) || (val5 > level) || (val6 > level) || (val7 > level)) return true ;

  return false ;
}

/** \brief Display the DCU values where the comparison is different
 * \param dcu - uploaded values
 */
void dcuDescription::displayDifferences ( dcuDescription &dcu ) {

  if (getDcuHardId( ) != dcu.getDcuHardId( ))
    std::cout << "\tHard ID is different " << std::dec << (unsigned int)getDcuHardId( ) << " / " << (unsigned int)dcu.getDcuHardId( ) << std::endl ;
  if (getDcuChannel0( ) != dcu.getDcuChannel0( ))
    std::cout << "\tChannel 0 is different " << std::dec << getDcuChannel0( ) << " / " <<  dcu.getDcuChannel0( ) << std::endl ;
  if (getDcuChannel1( ) != dcu.getDcuChannel1( ))
    std::cout << "\tChannel 1 is different " << std::dec << getDcuChannel1( ) << " / " <<  dcu.getDcuChannel1( ) << std::endl ;
  if (getDcuChannel2( ) != dcu.getDcuChannel2( ))
    std::cout << "\tChannel 2 is different " << std::dec << getDcuChannel2( ) << " / " <<  dcu.getDcuChannel2( ) << std::endl ;
  if (getDcuChannel3( ) != dcu.getDcuChannel3( ))
    std::cout << "\tChannel 3 is different " << std::dec << getDcuChannel3( ) << " / " <<  dcu.getDcuChannel3( ) << std::endl ;
  if (getDcuChannel4( ) != dcu.getDcuChannel4( ))
    std::cout << "\tChannel 4 is different " << std::dec << getDcuChannel4( ) << " / " <<  dcu.getDcuChannel4( ) << std::endl ;
  if (getDcuChannel5( ) != dcu.getDcuChannel5( ))
    std::cout << "\tChannel 5 is different " << std::dec << getDcuChannel5( ) << " / " <<  dcu.getDcuChannel5( ) << std::endl ;
  if (getDcuChannel6( ) != dcu.getDcuChannel6( ))
    std::cout << "\tChannel 6 is different " << std::dec << getDcuChannel6( ) << " / " <<  dcu.getDcuChannel6( ) << std::endl ;
  if (getDcuChannel7( ) != dcu.getDcuChannel7( ))
    std::cout << "\tChannel 7 is different " << std::dec << getDcuChannel7( ) << " / " <<  dcu.getDcuChannel7( ) << std::endl ;
}

/** \brief Display DCU values
 */
void dcuDescription::display ( ) {

  if (std::string(dcuType_) == DCUCCU) std::cout << "DCU on CCU: 0x" ;
  else std::cout << "DCU on FEH: 0x" ;

  std::cout << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;

  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
  std::cout << "\tenabled in the DCU readout: " << std::dec << isDcuReadoutEnabled() << std::endl;
  std::cout << "\tFEC Hardware ID: " << getFecHardwareId() << std::endl ;
  std::cout << "\tDCU hardware ID: " << std::dec << getDcuHardId() << std::endl ;
  std::cout << "\tTimestamp: " << std::dec << getTimeStamp() << std::endl ;
  std::cout << "\tchannel1: " << std::dec << (tscType16)getDcuChannel0() << std::endl;
  std::cout << "\tchannel2: " << std::dec << (tscType16)getDcuChannel1() << std::endl;
  std::cout << "\tchannel3: " << std::dec << (tscType16)getDcuChannel2() << std::endl;
  std::cout << "\tchannel4: " << std::dec << (tscType16)getDcuChannel3() << std::endl;
  std::cout << "\tchannel5: " << std::dec << (tscType16)getDcuChannel4() << std::endl;
  std::cout << "\tchannel6: " << std::dec << (tscType16)getDcuChannel5() << std::endl;
  std::cout << "\tchannel7: " << std::dec << (tscType16)getDcuChannel6() << std::endl ;
  std::cout << "\tchannel8: " << std::dec << (tscType16)getDcuChannel7() << std::endl;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *dcuDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;

  for (unsigned int i = 0 ; (i < sizeof(DCUPARAMETERNAMES)/sizeof(const char *)) ; i ++) {
    (*parameterNames)[DCUPARAMETERNAMES[i]] = new ParameterDescription(DCUPARAMETERNAMES[i], ParameterDescription::INTEGER16) ;
  }
  (*parameterNames)[DCUPARAMETERNAMES[DCUHARDID]]->setType(ParameterDescription::INTEGER32) ;
  (*parameterNames)[DCUPARAMETERNAMES[DCUTIMESTAMP]]->setType(ParameterDescription::INTEGER32) ;
  (*parameterNames)[DCUPARAMETERNAMES[EDCUTYPE]]->setType(ParameterDescription::STRING) ;
  (*parameterNames)[DCUPARAMETERNAMES[EDCUTYPE]]->setValue(DCUFEH) ; // default value
  (*parameterNames)[DCUPARAMETERNAMES[DCUREADOUTENABLED]]->setType(ParameterDescription::STRING) ;
  (*parameterNames)[DCUPARAMETERNAMES[DCUREADOUTENABLED]]->setValue("T") ; // default valuesetType(ParameterDescription::STRING) ;

  return parameterNames ;
}

#define LOCALGAMMA3 3192.
#define LOCALR25 10.
#define LOCALT25 298.1
#define LOCALT0 273.1
#define LOCALA_FACTOR -0.00056
#define LOCALB_FACTOR 20.
#define LOCALADCOFF -1.5
#define LOCALADCGAIN 2.22
#define LOCALINTOFF 2538.
#define LOCALINTGAIN 9.22

/** DCU Channel 0 or 4
 */
double dcuDescription::calcTempResTob(int ch, double ires) {

  double vin=((double)ch-LOCALADCOFF)/LOCALADCGAIN;
  double Rx=2*vin/ires;
  double Tx=1./(log(Rx/LOCALR25)/LOCALGAMMA3+1./LOCALT25)-LOCALT0;
  return (Tx+LOCALA_FACTOR*pow(Tx-LOCALB_FACTOR,2));
}
  
/** DCU Channel 0 or 4
 */
double dcuDescription::calcTempRes(int ch, double ires) {

  double vin=((double)ch-LOCALADCOFF)/LOCALADCGAIN;
  double Rx=vin/ires;
  double Tx=1./(log(Rx/LOCALR25)/LOCALGAMMA3+1./LOCALT25)-LOCALT0;
  return (Tx+LOCALA_FACTOR*pow(Tx-LOCALB_FACTOR,2));
}

/** DCU Channel 7
 */
double dcuDescription::calcTempInt( int ch ) {

  return ((((double) ch-LOCALINTOFF)/LOCALINTGAIN)+25);
}


/*   /\** Static method for the conversion of the channel 0 */
/*    * \param adcValue - ADC value coming from channel 0 */
/*    * \return the temperature */
/*    * \warning method given by the TOB people */
/*    *\/ */
/*   double dcuDescription::convertDcuChannel0ToTemperature ( tscType16 value ) { */

/*     double t0 = 25. + 273.15; */
/*     double B = 3090.; */
/*     double dcu0 = 220.; */
/*     double temperature = 1/t0 + 1/B * log (value/dcu0); */
/*     temperature = 1/temperature; */
/*     temperature = temperature -273.15; */

/* #ifdef DEBUGERRORMSG */
/*     std::cout << "Channel 0: temperature = " << temperature  */
/* 	 << " (ADC = " << value << ")" << std::endl ; */
/* #endif */

/*     return (temperature) ; */
/*   } */

/*   /\** Static method for the conversion of the channel 0 */
/*    * \param adcValue - ADC value coming from channel 0 */
/*    * \return the temperature */
/*    * \warning method given by the TOB people */
/*    *\/ */
/*   double dcuDescription::convertDcuChannel4ToTemperature ( tscType16 value ) { */

/*     double t0 = 24.7 + 273.15; */
/*     double B = 3090.; */
/*     double dcu0 = 201.; */
/*     double temperature = 1/t0 + 1/B * log (value/dcu0); */
/*     temperature = 1/temperature; */
/*     temperature = temperature -273.15; */

/* #ifdef DEBUGERRORMSG */
/*     std::cout << "Channel 4: temperature = " << temperature  */
/* 	 << " (ADC = " << value << ")" << std::endl ; */
/* #endif */

/*     return (temperature) ; */
/*   } */

/*   /\** Static method for the conversion of the channel 0 */
/*    * \param adcValue - ADC value coming from channel 0 */
/*    * \return the temperature */
/*    * \warning method given by the TOB people */
/*    *\/ */
/*   double dcuDescription::convertDcuChannel7ToTemperature ( tscType16 value ) { */

/*     double t0 = 24.; */
/*     double dcu0 = 2633.; */
/*     double temperature = (value - dcu0) / 9.22 + t0; */

/* #ifdef DEBUGERRORMSG */
/*     std::cout << "Channel 7: temperature = " << temperature  */
/* 	 << " (ADC = " << value << ")" << std::endl ; */
/* #endif */

/*     return (temperature) ; */
/*   } */

/** Parameter's names
 */
const char *dcuDescription::DCUPARAMETERNAMES[] = {"dcuTimeStamp",
						   "dcuHardId",
						   "channel0",
						   "channel1",
						   "channel2",
						   "channel3",
						   "channel4", 
						   "channel5", 
						   "channel6", 
						   "channel7",
						   "dcuType",
						   "dcuReadoutEnabled"} ;

