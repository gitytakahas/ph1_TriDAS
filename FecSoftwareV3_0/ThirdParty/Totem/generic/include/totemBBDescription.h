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
#ifndef TOTEMBBDESCRIPTION_H
#define TOTEMBBDESCRIPTION_H

#include "tscTypes.h"

#include "deviceDescription.h"
#include "totemBBDefinition.h"

/**
 * \class totemBBDescription
 * This class give a description of all the TBB registers:
 * <ul>
 * <li>ControlReg;
 * <li>Latency<1>;
 * <li>Latengy<2>;
 * <li>Repeat<1>;
 * <li>Repeat<2>;
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can also be set.
 * \see totemBeamBoardDefinition.h that gives the definition of different registers.
 * \include "totemBBDefinition.h"
 * \author Juha Petajajarvi
 * \date 2006
 * \brief This class give a description of all the TBB registers
 */

class totemBBDescription: public deviceDescription {

 private:
  //Principal registers
  tscType8 control ;
  tscType8 lat1 ;
  tscType8 lat2 ;
  tscType8 rep1 ;
  tscType8 rep2 ;
  
 public:

  /** Enumeration to access the list of parameter's names
   */
  enum TotemBBEnumType{TBBCONTROL, TBBLATENCY1, TBBLATENCY2, TBBREPEATS1, TBBREPEATS2} ;

  /** Parameter's names
   */
  static const char *TBBPARAMETERNAMES[TBBREPEATS2+1] ; // = { "tbbcontrol","tbblatency1","tbblatency2","tbbrepeats1","tbbrepeats2" } ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a TBB with all the values specified:
   * \param accessKey - device key
   */
  totemBBDescription (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a TBB with all the values specified:
   * \param accessKey - device key
   * \param control_ - TBB Register
   * \param lat1_ - TBB Register
   * \param lat2_ - TBB Register
   * \param rep1_ - TBB Register
   * \param rep2_ - TBB Register
   */
  totemBBDescription ( keyType accessKey,
                       tscType8 control_,
                       tscType8 lat1_,
		       tscType8 lat2_,
		       tscType8 rep1_,
		       tscType8 rep2_ ) ;

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
  totemBBDescription ( tscType16 fecSlot,
		       tscType16 ringSlot,
		       tscType16 ccuAddress,
		       tscType16 i2cChannel,
		       tscType16 i2cAddress,
                       tscType8 control_,
                       tscType8 lat1_,
		       tscType8 lat2_,
		       tscType8 rep1_,
		       tscType8 rep2_ ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a TBB with all the values specified:
   * \param control_ - TBB Register
   * \param lat1_ - TBB Register
   * \param lat2_ - TBB Register
   * \param rep1_ - TBB Register
   * \param rep2_ - TBB Register
   */
  totemBBDescription ( tscType8 control_,
                       tscType8 lat1_,
		       tscType8 lat2_,
		       tscType8 rep1_,
		       tscType8 rep2_ ) ;

  /* \brief Constructor to build a description of a TBB with a list of parameters names
   * \param parameterNames - list of parameters
   * \see ParameterDescription class
   */
  totemBBDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of a TBB with all the values specified
   * \param control_ - TBB Register
   * \param lat1_ - TBB Register
   * \param lat2_ - TBB Register
   * \param rep1_ - TBB Register
   * \param rep2_ - TBB Register
   */
  void setDescriptionValues ( tscType8 control_,
			      tscType8 lat1_,
			      tscType8 lat2_,
			      tscType8 rep1_,
			      tscType8 rep2_ ) ;

  //Set and Get functions - PRINCIPAL

  /** \brief Return the value of the TBB register corresponding to a TBB register
   * Return the value of the TBB register corresponding to a TBB register
   * \return the value
   */
  tscType8 getControl ( ) ;

  /** \brief set the value for register
   * Set the value for a TBB description
   * \param value to be set
   */
  void setControl ( tscType8 value ) ;
 
 /** \brief Return the value of the TBB register corresponding to a TBB register
   * Return the value of the TBB register corresponding to a TBB register
   * \return the value
   */
  tscType8 getLatency1 ( ) ;

  /** \brief set the value for register
   * Set the value for a TBB description
   * \param value to be set
   */
  void setLatency1 ( tscType8 value ) ;

 /** \brief Return the value of the TBB register corresponding to a TBB register
   * Return the value of the TBB register corresponding to a TBB register
   * \return the value
   */
  tscType8 getLatency2 ( ) ;

  /** \brief set the value for register
   * Set the value for a TBB description
   * \param value to be set
   */
  void setLatency2 ( tscType8 value ) ;

 /** \brief Return the value of the TBB register corresponding to a TBB register
   * Return the value of the TBB register corresponding to a TBB register
   * \return the value
   */
  tscType8 getRepeats1 ( ) ;

  /** \brief set the value for register
   * Set the value for a TBB description
   * \param value to be set
   */
  void setRepeats1 ( tscType8 value ) ;

 /** \brief Return the value of the TBB register corresponding to a TBB register
   * Return the value of the TBB register corresponding to a TBB register
   * \return the value
   */
  tscType8 getRepeats2 ( ) ;

  /** \brief set the value for register
   * Set the value for a TBB description
   * \param value to be set
   */
  void setRepeats2 ( tscType8 value ) ;

  /** \brief In order to compare two TBB descriptions
   * \param tbb - TBB description to be compared
   * \return bool - true if one value are different, false if not
   */
   bool operator!= ( totemBBDescription &tbb ) ;

  /** \brief In order to compare two TBB descriptions
   * \param tbb - TBB description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator  == ( totemBBDescription &tbb ) ;

  /** \briefClone a device description
   * \return the device description cloned
   */
   totemBBDescription *clone ( ) ;

  /** \brief Display the TBB values where the comparison is different
   * \param tbb - TBB values uploaded
   */
   void displayDifferences ( totemBBDescription &tbb ) ;
  
  /** \brief Display the TBB values
   */
   void display ( ) ;

  /** \brief Return a list of parameter names
   * \warning Must be delete by the owner of the description
   */
   static parameterDescriptionNameType *getParameterNames ( ) ;

};
#endif
