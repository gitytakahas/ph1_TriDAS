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
#ifndef LASERDRIVERDESCRIPTION_H
#define LASERDRIVERDESCRIPTION_H

#include "laserdriverDefinition.h"

#include "tscTypes.h"
#include "deviceDescription.h"
#include "laserdriverDefinition.h"

#include "FecExceptionHandler.h"

/**
 * \class laserdriverDescription
 * This class give a description of all the laserdescription registers:
 * <ul>
 * <li>Gain - gain for the channels.
 * <li>Bias - bias for the channels.
 * </ul>
 * <p><b>Note that the number of channels depend of the laserdriver version.</b>
 * <p><b>Note that this description never access the hardware, it is used to
 * keep the values to be set or set.</b>
 * \see laserdriverDefinition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "laserdriverDefinition.h"
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class give a description of all the laserdriver registers
 */
class laserdriverDescription: public deviceDescription {

 private:
  tscType8 bias0_ ;
  tscType8 bias1_ ;
  tscType8 bias2_ ;

  tscType8 gain0_ ;
  tscType8 gain1_ ;
  tscType8 gain2_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum LaserdriverEnumType {BIAS0, BIAS1, BIAS2, GAIN0, GAIN1, GAIN2} ;

  /** Parameter's names
   */
  static const char *LASERDRIVERPARAMETERNAMES[GAIN2+1] ; // = {"bias0","bias1","bias2","gain0","gain1","gain2"} ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a Laserdriver with all the values specified:
   * \param accessKey - device key
   */
  laserdriverDescription (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a laserdriver with all the values specified:
   * \param Gain - Gain for all the channels (0 < value < 3)
   * \param Bias - Bias for all the channels (1 < value < 25)
   */
  laserdriverDescription (tscType8 gain, tscType8 bias) ;

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
  laserdriverDescription (tscType8 gain, tscType8 bias[MAXLASERDRIVERCHANNELS]) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a laserdriver with all the values specified:
   * \param accessKey - key access of a device
   * \param Gain - Gain for all the channels (0 < value < 3)
   * \param Bias - Array of bias for all the channels (1 < value < 25)
   */
  laserdriverDescription ( keyType accessKey,
                           tscType8 gain, 
                           tscType8 bias[MAXLASERDRIVERCHANNELS]) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a laserdriver with all the values specified:
   * \param Gain - Gain for all the channels (0 < value < 3)
   * \param Bias - Array of bias for all the channels (1 < value < 25)
   */
  laserdriverDescription ( tscType16 fecSlot,
                           tscType16 ringSlot,
                           tscType16 ccuAddress,
                           tscType16 i2cChannel,
                           tscType16 i2cAddress,
                           tscType8 gain, 
                           tscType8 bias[MAXLASERDRIVERCHANNELS]) ;

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
  laserdriverDescription ( tscType16 fecSlot,
                           tscType16 ringSlot,
                           tscType16 ccuAddress,
                           tscType16 i2cChannel,
                           tscType16 i2cAddress,
                           tscType8 gain[MAXLASERDRIVERCHANNELS], 
                           tscType8 bias[MAXLASERDRIVERCHANNELS]) ;

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
  laserdriverDescription ( keyType accessKey,
                           tscType8 bias0,
                           tscType8 bias1,
                           tscType8 bias2,
                           tscType8 gain0,
                           tscType8 gain1,
                           tscType8 gain2) ;

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
  laserdriverDescription ( tscType16 fecSlot,
                           tscType16 ringSlot,
                           tscType16 ccuAddress,
                           tscType16 i2cChannel,
                           tscType16 i2cAddress = 0x60,
                           tscType8 bias0 = 0,
                           tscType8 bias1 = 0,
                           tscType8 bias2 = 0,
                           tscType8 gain0 = 0,
                           tscType8 gain1 = 0,
                           tscType8 gain2 = 0 ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a laserdriver with all the values specified:
   * \param Gain - Gain for all the channels (0 < value < 3)
   * \param Bias - Array of bias for all the channels (1 < value < 25)
   */
  laserdriverDescription ( tscType8 gain[MAXLASERDRIVERCHANNELS], tscType8 bias[MAXLASERDRIVERCHANNELS]) ;

  /* build a description of a laserdriver with a list of parameters name
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   */
  laserdriverDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of a laserdriver with all the values specified:
   * \param Gain - Gain for all the channels (0 < value < 3)
   * \param Bias - Array of bias for all the channels (1 < value < 25)
   */
  void setDescriptionValues ( tscType8 gain[MAXLASERDRIVERCHANNELS], 
			      tscType8 bias[MAXLASERDRIVERCHANNELS]) ;

  /** \brief return the value of the param
   * Return the value of laserdriver register
   * \return the value
   */
  tscType8 getGain ( ) ;

  /** \brief return the value of the param
   * Return the value of laserdriver register for a given channel
   * \param channel - channel value
   * \return the value
   * \exception FecExceptionHandler in case of bad channel number
   */
  tscType8 getGain ( tscType8 channel ) throw (FecExceptionHandler) ;

  /** \brief set the gain value for a given channel
   * \param channel - channel value
   * \param gain - gain value
   */
  void setGain ( tscType8 channel, tscType8 gain ) throw (FecExceptionHandler) ;

  /** \brief return the value of the param
   * Return the value of laserdriver register
   * \return the value
   */
  tscType8 getGain0 ( ) ;

  /** \brief return the value of the param
   * Return the value of laserdriver register
   * \return the value
   */
  tscType8 getGain1 ( ) ;

  /** \brief return the value of the param
   * Return the value of laserdriver register
   * \return the value
   */
  tscType8 getGain2 ( ) ;

  /** \brief Give the values of the param
   * Give the values of laserdriver register
   * \param bias - pointer to an array of bias
   */
  void getBias ( tscType8 *bias ) ;

  /** \brief Give the values of the param
   * Give the values of laserdriver register
   * \param bias - pointer to an array of bias
   */
  tscType8 getBias0 ( ) ;

  /** \brief Give the values of the param
   * Give the values of laserdriver register
   * \param bias - pointer to an array of bias
   */
  tscType8 getBias1 ( ) ;

  /** \brief Give the values of the param
   * Give the values of laserdriver register
   * \param bias - pointer to an array of bias
   */
  tscType8 getBias2 ( ) ;

  /** \brief return the value of the param
   * Return the value of laserdriver register for a given channel
   * \param channel - channel value
   * \return the value
   * \exception FecExceptionHandler in case of bad channel number
   */
  tscType8 getBias ( tscType8 channel ) throw (FecExceptionHandler) ;

  /** \brief set the bias value for a given channel
   * \param channel - channel value
   * \param bias - bias value
   */
  void setBias ( tscType8 channel, tscType8 bias) throw (FecExceptionHandler) ;

  /** \brief set the value for register
   * Set the value for a laserdriver description
   * \param gain - value to be set
   */
  void setGain ( tscType8 gain ) ;

  /** \brief set the value for register
   * Set the value for a laserdriver description
   * \param gain - array to be set
   */
  void setGain ( tscType8 gain[MAXLASERDRIVERCHANNELS] ) ;

  /** \brief set the value for register
   * Set the value for a laserdriver description
   * \param bias - pointer to an array of bias
   */
  void setBias ( tscType8 *Bias ) ;

  /** \brief In order to compare laserdriver description
   * \param laserdriver - description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( laserdriverDescription &laserdriver ) ;

  /** \brief In order to compare laserdriver description
   * \param laserdriver - description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( laserdriverDescription &laserdriver ) ;

  /** Clone a device description
   * \return the device description cloned
   */
  laserdriverDescription *clone ( ) ;

  /** \brief Display the APV values where the comparison is different
   * \param laserdriver - uploaded values
   */
  void displayDifferences ( laserdriverDescription &laserdriver ) ;

  /** \brief Display laserdriver values
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

} ;

#endif
