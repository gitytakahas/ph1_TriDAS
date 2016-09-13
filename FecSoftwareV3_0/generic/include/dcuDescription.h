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
#ifndef DCUDESCRIPTION_H
#define DCUDESCRIPTION_H

#include <math.h>  // for the conversion

#include "dcuDefinition.h"

#include "tscTypes.h"
#include "deviceDescription.h"

#define DCUCCU "CCU"
#define DCUFEH "FEH"

/**
 * \class dcuDescription
 * This class give a description of all the DCU channels:
 * <ul>
 * <li>timeStamp
 * <li>DCU Hardware ID;
 * <li>Channel 0;
 * <li>Channel 1;
 * <li>Channel 2;
 * <li>Channel 3;
 * <li>Channel 4;
 * <li>Channel 5;
 * <li>Channel 6;
 * <li>Channel 7;
 * </ul>
 * <B>Note that this description never access the hardware, it is used to
 * keep the values to be read.</B>
 * \see dcuDefinition.h that gives the definition of the version of the dcu
 * \include "dcuDefinition.h"
 * \author Frederic Drouhin and Joao Fernandes
 * \date July 2002
 * \brief This class give a description of all the DCU channels
 */
class dcuDescription: public deviceDescription {

 private:
  tscType32 timeStamp_ ;
  tscType32 dcuHardId_ ;
  tscType16 channel0_  ;
  tscType16 channel1_  ;
  tscType16 channel2_  ;
  tscType16 channel3_  ;
  tscType16 channel4_  ;
  tscType16 channel5_  ;
  tscType16 channel6_  ;
  tscType16 channel7_  ;

  /** DCU type (DCU on CCU or DCU on FEH) as it is detected
   */
  char dcuType_[4] ;

  /** DCU included in the DCU readout (a DCU can be disabled in the DAQ and can be enabled in the DCU readout and the reverse is also possible)
   */
  bool dcuReadoutEnabled_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum DcuEnumType {DCUTIMESTAMP, DCUHARDID, CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL5, CHANNEL6, CHANNEL7, EDCUTYPE, DCUREADOUTENABLED} ;

  /** Parameter's names
   */
  static const char *DCUPARAMETERNAMES[DCUREADOUTENABLED+1] ; // = {"dcuTimeStamp","dcuHardId","channel0","channel1","channel2","channel3","channel4","channel5","channel6","channel7","dcuType","dcuReadoutEnabled"} ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a DCU with all the values specified:
   * \param accessKey - device key
   */
  dcuDescription (keyType accessKey = 0, std::string dcuType = DCUFEH ) ;

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
  dcuDescription (keyType accessKey,
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
		  std::string dcuType = DCUFEH) ;

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
  dcuDescription (tscType16 fecSlot,
                  tscType16 ringSlot,
                  tscType16 ccuAddress,
                  tscType16 i2cChannel,
                  tscType16 i2cAddress,
                  tscType32 timeStamp = 0,
                  tscType32 dcuHardId = 0,
                  tscType16 chan0 = 0,
                  tscType16 chan1 = 0,
                  tscType16 chan2 = 0,
                  tscType16 chan3 = 0,
                  tscType16 chan4 = 0,
                  tscType16 chan5 = 0,
                  tscType16 chan6 = 0,
                  tscType16 chan7 = 0,
		  std::string dcuType = DCUFEH) ;

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
  dcuDescription (tscType32 timeStamp,
                  tscType32 dcuHardId,
                  tscType16 chan0,
                  tscType16 chan1,
                  tscType16 chan2,
                  tscType16 chan3,
                  tscType16 chan4,
                  tscType16 chan5,
                  tscType16 chan6,
                  tscType16 chan7,
		  std::string dcuType = DCUFEH) ;

  /* build a description of a DCU with a list of parameters name
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   */
  dcuDescription ( parameterDescriptionNameType parameterNames ) ;

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
  void setDescriptionValues (tscType32 timeStamp,
                             tscType32 dcuHardId,
                             tscType16 chan0,
                             tscType16 chan1,
                             tscType16 chan2,
                             tscType16 chan3,
                             tscType16 chan4,
                             tscType16 chan5,
                             tscType16 chan6,
                             tscType16 chan7,
			     std::string dcuType ) ;

  /** \brief Return the enable/disable in the DCU readout
   * \return true if the DCU can be used for the DCU readout
   */
  bool getDcuReadoutEnabled ( ) ;

  /** \brief Return the enable/disable in the DCU readout
   * \return true if the DCU can be used for the DCU readout
   * \see getEnabledDcuReadout (same method)
   */
  bool isDcuReadoutEnabled ( ) ;

  /** \brief set the DCU to be used or not in the DCU readout
   * param dcuReadoutEnabled - enable or disable in the DCU readout
   */
  void setDcuReadoutEnabled ( bool dcuReadoutEnabled ) ;

  /** \brief returns the DCU Hardware ID
   * \return the value
   */
  tscType32 getDcuHardId ( ) ;

  /** \brief return the DCU type
   */
  std::string getDcuType ( ) ;

  /** \brief not used for the moment, returns the DCU Hardware ID
   * \returns the value
   */
  tscType32 getTimeStamp ( ) ;

  /** \brief returns the DCU channel value corresponding to the channel
   * \param channel - DCU channel 
   */
  tscType16 getDcuChannel( int channel ) ;

  /** \brief returns the DCU channel value corresponding to this description
   * \returns the value
   */
  tscType16 getDcuChannel0 ( ) ;
  /** \brief returns the DCU channel value corresponding to this description
   * \returns the value
   */
  tscType16 getDcuChannel1 ( ) ;
  /** \brief returns the DCU channel value corresponding to this description
   * \returns the value
   */
  tscType16 getDcuChannel2 ( ) ;
  /** \brief returns the DCU channel value corresponding to this description
   * \returns the value
   */
  tscType16 getDcuChannel3 ( ) ;
  /** \brief returns the DCU channel value corresponding to this description
   * \returns the value
   */
  tscType16 getDcuChannel4 ( ) ;
  /** \brief returns the DCU channel value corresponding to this description
   * \returns the value
   */
  tscType16 getDcuChannel5 ( ) ;
  /** \brief returns the DCU channel value corresponding to this description
   * \returns the value
   */
  tscType16 getDcuChannel6 ( ) ;
  /** \brief returns the DCU channel value corresponding to this description
   * \returns the value
   */
  tscType16 getDcuChannel7 ( ) ;

  /** \brief set the timeStamp
   */
  void setTimeStamp ( tscType32 timeStamp ) ;

  /** \brief set the hardware id
   */
  void setDcuHardId ( tscType32 dcuHardId ) ;

  /** \brief set the DCU type
   */
  void setDcuType ( std::string dcuType ) ;

  /** \brief set a parameter in the corresponding channel
   */
  void setDcuChannel0 ( tscType16 chan0 ) ;
  /** \brief set a parameter in the corresponding channel
   */
  void setDcuChannel1 ( tscType16 chan1 ) ;
  /** \brief set a parameter in the corresponding channel
   */
  void setDcuChannel2 ( tscType16 chan2 ) ;
  /** \brief set a parameter in the corresponding channel
   */
  void setDcuChannel3 ( tscType16 chan3 ) ;
  /** \brief set a parameter in the corresponding channel
   */
  void setDcuChannel4 ( tscType16 chan4 ) ;
  /** \brief set a parameter in the corresponding channel
   */
  void setDcuChannel5 ( tscType16 chan5 ) ;
  /** \brief set a parameter in the corresponding channel
   */
  void setDcuChannel6 ( tscType16 chan6 ) ;
  /** \brief set a parameter in the corresponding channel
   */
  void setDcuChannel7 ( tscType16 chan7 ) ;

  /** \brief In order to compare two values
   */
  bool operator != ( dcuDescription &dcu ) ;

  /** \brief In order to compare two values
   * \param dcu - uploaded values
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( dcuDescription &dcu ) ;

  /** Clone an apv description
   * \return the apvDescription cloned
   */
  dcuDescription *clone ( ) ;
  
  /** \brief return true if the level of the difference is > than the param sent
   * \param dcu - values to be cheched
   * \param level - absolute value of difference
   */
  bool isLevelDifferent ( dcuDescription dcu, long level ) ;

  /** \brief Display the DCU values where the comparison is different
   * \param dcu - uploaded values
   */
  void displayDifferences ( dcuDescription &dcu ) ;

  /** \brief Display DCU values
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

  /** DCU Channel 0 or 4
   */
  static double calcTempResTob(int ch, double ires) ;
  
  /** DCU Channel 0 or 4
   */
  static double calcTempRes(int ch, double ires) ;

  /** DCU Channel 7
   */
  static double calcTempInt( int ch ) ;

/*   /\** Static method for the conversion of the channel 0 */
/*    * \param adcValue - ADC value coming from channel 0 */
/*    * \return the temperature */
/*    * \warning method given by the TOB people */
/*    *\/ */
/*   static double convertDcuChannel0ToTemperature ( tscType16 value ) ; */

/*   /\** Static method for the conversion of the channel 0 */
/*    * \param adcValue - ADC value coming from channel 0 */
/*    * \return the temperature */
/*    * \warning method given by the TOB people */
/*    *\/ */
/*   static double convertDcuChannel4ToTemperature ( tscType16 value ) ; */

/*   /\** Static method for the conversion of the channel 0 */
/*    * \param adcValue - ADC value coming from channel 0 */
/*    * \return the temperature */
/*    * \warning method given by the TOB people */
/*    *\/ */
/*   static double convertDcuChannel7ToTemperature ( tscType16 value ) ; */
} ;

#endif
