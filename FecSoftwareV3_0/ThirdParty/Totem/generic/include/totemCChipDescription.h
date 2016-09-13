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
#ifndef TOTEMCCHIPDESCRIPTION_H
#define TOTEMCCHIPDESCRIPTION_H

#include "tscTypes.h"

#include "deviceDescription.h"
#include "totemCChipDefinition.h"

/**
 * \class totemCChipDescription
 * This class give a description of all the CHIP registers:
 * <ul>
 * <li>ControlReg<0>;
 * <li>ControlReg<1>;
 * <li>ControlReg<2>;
 * <li>ControlReg<3>;
 * <li>ControlReg<4>;
 * <li>ChipID<0>;
 * <li>ChipID<1>;
 * <li>HitCount0;
 * <li>HitCount1;
 * <li>HitCount2;
 * <li>ExtRegPointer;
 * <li>ExtRegData;
 * <li>Mask<0>;
 * <li>Mask<1>;
 * <li>Mask<2>;
 * <li>Mask<3>;
 * <li>Mask<4>;
 * <li>Mask<5>;
 * <li>Mask<6>;
 * <li>Mask<7>;
 * <li>Mask<8>;
 * <li>Mask<9>;
 * <li>Resistor;
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can also be set.
 * \see totemCChipDefinition.h that gives the definition of different registers.
 * \include "totemCChipDefinition.h"
 * \author Juha Petajajarvi
 * \date 2006
 * \brief This class give a description of all the CHIP registers
 */

class totemCChipDescription: public deviceDescription {

 private:
  //Principal registers
  tscType8 control0 ;
  tscType8 control1 ;
  tscType8 control2 ;
  tscType8 control3 ;
  tscType8 control4 ;
  tscType8 chipid0 ;
  tscType8 chipid1 ;
  tscType8 counter0 ;
  tscType8 counter1 ;
  tscType8 counter2 ;
  tscType8 mask0 ;
  tscType8 mask1 ;
  tscType8 mask2 ;
  tscType8 mask3 ;
  tscType8 extregpointer ;
  tscType8 extregdata ;

  //Extended registers
  tscType8 mask4 ;
  tscType8 mask5 ;
  tscType8 mask6 ;
  tscType8 mask7 ;
  tscType8 mask8 ;
  tscType8 mask9 ;
  tscType8 res ;
  
 public:

  /** Enumeration to access the list of parameter's names
   */
  enum TotemCChipType {CCHIPCONTROL0, CCHIPCONTROL1, CCHIPCONTROL2, CCHIPCONTROL3, CCHIPCONTROL4, CCHIPID0, CCHIPID1,
		       CCHIPCOUNTER0, CCHIPCOUNTER1, CCHIPCOUNTER2, CCHIPMASK0, CCHIPMASK1, CCHIPMASK2, CCHIPMASK3, CCHIPMASK4,
		       CCHIPMASK5, CCHIPMASK6, CCHIPMASK7, CCHIPMASK8, CCHIPMASK9, CCHIPRES} ;
  
  /** Parameter's names
   */
  static const char *CCHIPPARAMETERNAMES[CCHIPRES+1] ;
  
  /** \brief Constructor in order to set all the parameters
   * Build a description of a CHIP with all the values specified:
   * \param accessKey - device key
   */
  totemCChipDescription (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a cc with all the values specified:
   * \param accessKey - device key
   * \param control0_ - CHIP Register
   * \param control1_ - CHIP Register
   * \param control2_ - CHIP Register
   * \param control3_ - CHIP Register
   * \param control4_ - CHIP Register
   * \param chipid0_ - CHIP Register
   * \param chipid1_ - CHIP Register
   * \param counter0_ - CHIP Register
   * \param counter1_ - CHIP Register
   * \param counter2_ - CHIP Register
   * \param mask0_ - CHIP Register
   * \param mask1_ - CHIP Register
   * \param mask2_ - CHIP Register
   * \param mask3_ - CHIP Register
   * \param mask4_ - CHIP Register
   * \param mask5_ - CHIP Register
   * \param mask6_ - CHIP Register
   * \param mask7_ - CHIP Register
   * \param mask8_ - CHIP Register
   * \param mask9_ - CHIP Register
   * \param res_   - CHIP Register
   */
  totemCChipDescription ( keyType accessKey,
			  tscType8 control0_,
			  tscType8 control1_,
			  tscType8 control2_,
			  tscType8 control3_,
			  tscType8 control4_,
			  tscType8 chipid0_,
			  tscType8 chipid1_,
			  tscType8 counter0_,
			  tscType8 counter1_,
			  tscType8 counter2_,

			  tscType8 mask0_,
			  tscType8 mask1_,
			  tscType8 mask2_,
			  tscType8 mask3_,
			  tscType8 mask4_,
			  tscType8 mask5_,
			  tscType8 mask6_,
			  tscType8 mask7_,
			  tscType8 mask8_,
			  tscType8 mask9_,
			  tscType8 res_ ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a CCHIP with all the values specified:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   * \param control0_ - CHIP Register
   * \param control1_ - CHIP Register
   * \param control2_ - CHIP Register
   * \param control3_ - CHIP Register
   * \param control4_ - CHIP Register
   * \param chipid0_ - CHIP Register
   * \param chipid1_ - CHIP Register
   * \param counter0_ - CHIP Register
   * \param counter1_ - CHIP Register
   * \param counter2_ - CHIP Register
   * \param mask0_ - CHIP Register
   * \param mask1_ - CHIP Register
   * \param mask2_ - CHIP Register
   * \param mask3_ - CHIP Register
   * \param mask4_ - CHIP Register
   * \param mask5_ - CHIP Register
   * \param mask6_ - CHIP Register
   * \param mask7_ - CHIP Register
   * \param mask8_ - CHIP Register
   * \param mask9_ - CHIP Register
   * \param res_   - CHIP Register
   */
  totemCChipDescription ( tscType16 fecSlot,
			  tscType16 ringSlot,
			  tscType16 ccuAddress,
			  tscType16 i2cChannel,
			  tscType16 i2cAddress,
			  tscType8 control0_,
			  tscType8 control1_,
			  tscType8 control2_,
			  tscType8 control3_,
			  tscType8 control4_,
			  tscType8 chipid0_,
			  tscType8 chipid1_,
			  tscType8 counter0_,
			  tscType8 counter1_,
			  tscType8 counter2_,

			  tscType8 mask0_,
			  tscType8 mask1_,
			  tscType8 mask2_,
			  tscType8 mask3_,
			  tscType8 mask4_,
			  tscType8 mask5_,
			  tscType8 mask6_,
			  tscType8 mask7_,
			  tscType8 mask8_,
			  tscType8 mask9_,
			  tscType8 res_ ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a CCHIP with all the values specified:
   * \param control0_ - CHIP Register
   * \param control1_ - CHIP Register
   * \param control2_ - CHIP Register
   * \param control3_ - CHIP Register
   * \param control4_ - CHIP Register
   * \param chipid0_ - CHIP Register
   * \param chipid1_ - CHIP Register
   * \param counter0_ - CHIP Register
   * \param counter1_ - CHIP Register
   * \param counter2_ - CHIP Register
   * \param mask0_ - CHIP Register
   * \param mask1_ - CHIP Register
   * \param mask2_ - CHIP Register
   * \param mask3_ - CHIP Register
   * \param mask4_ - CHIP Register
   * \param mask5_ - CHIP Register
   * \param mask6_ - CHIP Register
   * \param mask7_ - CHIP Register
   * \param mask8_ - CHIP Register
   * \param mask9_ - CHIP Register
   * \param res_   - CHIP Register
   */
  totemCChipDescription ( tscType8 control0_,
			  tscType8 control1_,
			  tscType8 control2_,
			  tscType8 control3_,
			  tscType8 control4_,
			  tscType8 chipid0_,
			  tscType8 chipid1_,
			  tscType8 counter0_,
			  tscType8 counter1_,
			  tscType8 counter2_,

			  tscType8 mask0_,
			  tscType8 mask1_,
			  tscType8 mask2_,
			  tscType8 mask3_,
			  tscType8 mask4_,
			  tscType8 mask5_,
			  tscType8 mask6_,
			  tscType8 mask7_,
			  tscType8 mask8_,
			  tscType8 mask9_,
			  tscType8 res_  ) ;

  /* \brief Constructor to build a description of a CCHIP with a list of parameters names
   * \param parameterNames - list of parameters
   * \see ParameterDescription class
   */
  totemCChipDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of a CCHIP with all the values specified
   * \param control0_ - CHIP Register
   * \param control1_ - CHIP Register
   * \param control2_ - CHIP Register
   * \param control3_ - CHIP Register
   * \param control4_ - CHIP Register
   * \param chipid0_ - CHIP Register
   * \param chipid1_ - CHIP Register
   * \param counter0_ - CHIP Register
   * \param counter1_ - CHIP Register
   * \param counter2_ - CHIP Register
   * \param mask0_ - CHIP Register
   * \param mask1_ - CHIP Register
   * \param mask2_ - CHIP Register
   * \param mask3_ - CHIP Register
   * \param mask4_ - CHIP Register
   * \param mask5_ - CHIP Register
   * \param mask6_ - CHIP Register
   * \param mask7_ - CHIP Register
   * \param mask8_ - CHIP Register
   * \param mask9_ - CHIP Register
   * \param res_   - CHIP Register
   */
  void setDescriptionValues ( tscType8 control0_,
                              tscType8 control1_,
                              tscType8 control2_,
                              tscType8 control3_,
                              tscType8 control4_,
                              tscType8 chipid0_,
                              tscType8 chipid1_,
			      tscType8 counter0_,
			      tscType8 counter1_,
                              tscType8 counter2_,

                              tscType8 mask0_,
                              tscType8 mask1_,
                              tscType8 mask2_,
                              tscType8 mask3_,
                              tscType8 mask4_,
                              tscType8 mask5_,
                              tscType8 mask6_,
                              tscType8 mask7_,
                              tscType8 mask8_,
                              tscType8 mask9_,
                              tscType8 res_ ) ;

  //Set and Get functions - PRINCIPAL

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getControl0 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setControl0 ( tscType8 value ) ;
 
  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getControl1 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setControl1 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getControl2 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setControl2 ( tscType8 value ) ;
 
  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getControl3 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setControl3 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getControl4 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setControl4 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getChipid0 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setChipid0 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getChipid1 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setChipid1 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getCounter0 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setCounter0 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getCounter1 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setCounter1 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getCounter2 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setCounter2 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask0 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask0 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask1 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask1  ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask2 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask2 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask3 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask3 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getExtRegPointer ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setExtRegPointer ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getExtRegData ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setExtRegData  ( tscType8 value ) ;

  //Set and Get functions - EXTENDED

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask4 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask4  ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask5 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask5  ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask6 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask6  ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask7 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask7  ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask8 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask8  ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getMask9 ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setMask9 ( tscType8 value ) ;

  /** \brief Return the value of the CHIP register corresponding to a CHIP register
   * Return the value of the CHIP register corresponding to a CHIP register
   * \return the value
   */
  tscType8 getRes ( ) ;

  /** \brief set the value for register
   * Set the value for a CHIP description
   * \param value to be set
   */
  void setRes  ( tscType8 value ) ;

  /** \brief In order to compare two CHIP descriptions
   * \param cc - CHIP description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( totemCChipDescription &cc ) ;

  /** \brief In order to compare two CHIP descriptions
   * \param cc - CHIP description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator  == ( totemCChipDescription &cc ) ;

  /** \briefClone a device description
   * \return the device description cloned
   */
  totemCChipDescription *clone ( ) ;

  /** \brief Display the CCHIP values where the comparison is different
   * \param cc - CHIP values uploaded
   */

  void displayDifferences ( totemCChipDescription &cc ) ;
  
  /** \brief Display the CCHIP values
   */
  void display ( ) ;

  /** \brief Return a list of parameter names
   * \warning Must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;
 
};

#endif
