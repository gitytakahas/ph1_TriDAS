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

#include "totemCChipDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a CHIP with all the values specified:
 * \param accessKey - device key
 */
totemCChipDescription::totemCChipDescription (keyType accessKey): deviceDescription ( CCHIP, accessKey ) { }

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
totemCChipDescription::totemCChipDescription ( keyType accessKey,
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
					       tscType8 res_ ):

  deviceDescription ( CCHIP, accessKey ) {

  setControl0 (control0_) ;
  setControl1 (control1_) ;
  setControl2 (control2_) ;
  setControl3 (control3_) ;
  setControl4 (control4_) ;
  setChipid0 (chipid0_) ;
  setChipid1 (chipid1_) ;
  setCounter0 (counter0_) ;
  setCounter1 (counter1_) ;
  setCounter2 (counter2_) ;

  setMask0 (mask0_) ;
  setMask1 (mask1_) ;
  setMask2 (mask2_) ;
  setMask3 (mask3_) ;
  setMask4 (mask4_) ;
  setMask5 (mask5_) ;
  setMask6 (mask6_) ;
  setMask7 (mask7_) ;
  setMask8 (mask8_) ;
  setMask9 (mask9_) ;
  setRes   (res_) ;

}

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
totemCChipDescription::totemCChipDescription ( tscType16 fecSlot,
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
					       tscType8 res_ ):

  deviceDescription ( CCHIP, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setControl0 (control0_) ;
  setControl1 (control1_) ;
  setControl2 (control2_) ;
  setControl3 (control3_) ;
  setControl4 (control4_) ;
  setChipid0 (chipid0_) ;
  setChipid1 (chipid1_) ;
  setCounter0 (counter0_) ;
  setCounter1 (counter1_) ;
  setCounter2 (counter2_) ;

  setMask0 (mask0_) ;
  setMask1 (mask1_) ;
  setMask2 (mask2_) ;
  setMask3 (mask3_) ;
  setMask4 (mask4_) ;
  setMask5 (mask5_) ;
  setMask6 (mask6_) ;
  setMask7 (mask7_) ;
  setMask8 (mask8_) ;
  setMask9 (mask9_) ;
  setRes   (res_) ;
}

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
totemCChipDescription::totemCChipDescription ( tscType8 control0_,
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
					       tscType8 res_  ):

  deviceDescription ( CCHIP ) {

  setDescriptionValues ( control0_, 
			 control1_,
			 control2_,
			 control3_,
			 control4_,
			 chipid0_,
			 chipid1_,
			 counter0_,
			 counter1_,
			 counter2_,

			 mask0_,
			 mask1_,
			 mask2_,
			 mask3_,
			 mask4_,
			 mask5_,
			 mask6_,
			 mask7_,
			 mask8_,
			 mask9_,
			 res_ ) ;
}

/* \brief Constructor to build a description of a CCHIP with a list of parameters names
 * \param parameterNames - list of parameters
 * \see ParameterDescription class
 */
totemCChipDescription::totemCChipDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( CCHIP, parameterNames) {

  setControl0 (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPCONTROL0]]->getValueConverted())) ;
  setControl1 (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPCONTROL1]]->getValueConverted())) ;
  setControl2 (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPCONTROL2]]->getValueConverted())) ;
  setControl3 (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPCONTROL3]]->getValueConverted())) ;
  setControl4 (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPCONTROL4]]->getValueConverted())) ;
  setChipid0  (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPID0]]->getValueConverted())) ;
  setChipid1  (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPID1]]->getValueConverted())) ;
  setCounter0 (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPCOUNTER0]]->getValueConverted())) ;
  setCounter1 (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPCOUNTER1]]->getValueConverted())) ;
  setCounter2 (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPCOUNTER2]]->getValueConverted())) ;

  setMask0    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK0]]->getValueConverted())) ;
  setMask1    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK1]]->getValueConverted())) ;
  setMask2    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK2]]->getValueConverted())) ;
  setMask3    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK3]]->getValueConverted())) ;
  setMask4    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK4]]->getValueConverted())) ;
  setMask5    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK5]]->getValueConverted())) ;
  setMask6    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK6]]->getValueConverted())) ;
  setMask7    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK7]]->getValueConverted())) ;
  setMask8    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK8]]->getValueConverted())) ;
  setMask9    (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPMASK9]]->getValueConverted())) ;
  setRes      (*((tscType8 *) parameterNames[CCHIPPARAMETERNAMES[CCHIPRES]]->getValueConverted())) ;

}

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
void totemCChipDescription::setDescriptionValues ( tscType8 control0_,
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
						   tscType8 res_ ){

  setControl0 (control0_) ;
  setControl1 (control1_) ;
  setControl2 (control2_) ;
  setControl3 (control3_) ;
  setControl4 (control4_) ;
  setChipid0 (chipid0_) ;
  setChipid1 (chipid1_) ;
  setCounter0 (counter0_) ;
  setCounter1 (counter1_) ;
  setCounter2 (counter2_) ;

  setMask0 (mask0_) ;
  setMask1 (mask1_) ;
  setMask2 (mask2_) ;
  setMask3 (mask3_) ;
  setMask4 (mask4_) ;
  setMask5 (mask5_) ;
  setMask6 (mask6_) ;
  setMask7 (mask7_) ;
  setMask8 (mask8_) ;
  setMask9 (mask9_) ;
  setRes   (res_) ;
}

//Set and Get functions - PRINCIPAL

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getControl0 ( ) { return (control0) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setControl0   ( tscType8 value ) { control0 = value ; }
 
/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getControl1 ( ) { return (control1) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setControl1   ( tscType8 value ) { control1 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getControl2 ( ) { return (control2) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setControl2   ( tscType8 value ) { control2 = value ; }
 
/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getControl3 ( ) { return (control3) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setControl3   ( tscType8 value ) { control3 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getControl4 ( ) { return (control4) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setControl4   ( tscType8 value ) { control4 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getChipid0 ( ) { return (chipid0) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setChipid0  ( tscType8 value ) { chipid0 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getChipid1 ( ) { return (chipid1) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setChipid1  ( tscType8 value ) { chipid1 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getCounter0 ( ) { return (counter0) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setCounter0  ( tscType8 value ) { counter0 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getCounter1 ( ) { return (counter1) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setCounter1  ( tscType8 value ) { counter1 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getCounter2 ( ) { return (counter2) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setCounter2  ( tscType8 value ) { counter2 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask0 ( ) { return (mask0) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask0  ( tscType8 value ) { mask0 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask1 ( ) { return (mask1) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask1  ( tscType8 value ) { mask1 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask2 ( ) { return (mask2) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask2  ( tscType8 value ) { mask2 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask3 ( ) { return (mask3) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask3  ( tscType8 value ) { mask3 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getExtRegPointer ( ) { return (extregpointer) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setExtRegPointer  ( tscType8 value ) { extregpointer = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getExtRegData ( ) { return (extregdata) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setExtRegData  ( tscType8 value ) { extregdata = value ; }

//Set and Get functions - EXTENDED

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask4 ( ) { return (mask4) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask4  ( tscType8 value ) { mask4 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask5 ( ) { return (mask5) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask5  ( tscType8 value ) { mask5 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask6 ( ) { return (mask6) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask6  ( tscType8 value ) { mask6 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask7 ( ) { return (mask7) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask7  ( tscType8 value ) { mask7 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask8 ( ) { return (mask8) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask8  ( tscType8 value ) { mask8 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getMask9 ( ) { return (mask9) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setMask9  ( tscType8 value ) { mask9 = value ; }

/** \brief Return the value of the CHIP register corresponding to a CHIP register
 * Return the value of the CHIP register corresponding to a CHIP register
 * \return the value
 */
tscType8 totemCChipDescription::getRes ( ) { return (res) ; }

/** \brief set the value for register
 * Set the value for a CHIP description
 * \param value to be set
 */
void totemCChipDescription::setRes  ( tscType8 value ) { res = value ; }

/** \brief In order to compare two CHIP descriptions
 * \param cc - CHIP description to be compared
 * \return bool - true if one value are different, false if not
 */
bool totemCChipDescription::operator!= ( totemCChipDescription &cc ) {

#ifdef DEBUGMSGERROR
  displayDifferences(cc) ;
#endif

  if (
      (getControl0 ( ) != cc.getControl0 ( )) ||
      (getControl1 ( ) != cc.getControl1 ( )) ||
      (getControl2 ( ) != cc.getControl2 ( )) ||
      (getControl3 ( ) != cc.getControl3 ( )) ||
      (getControl4 ( ) != cc.getControl4 ( )) ||

      (getChipid0 ( ) != cc.getChipid0 ( )) ||
      (getChipid1 ( ) != cc.getChipid1 ( )) ||
      (getCounter0 ( ) != cc.getCounter0 ( )) ||
      (getCounter1 ( ) != cc.getCounter1 ( )) ||
      (getCounter2 ( ) != cc.getCounter2 ( )) ||

      (getMask0 ( ) != cc.getMask0 ( )) ||
      (getMask1 ( ) != cc.getMask1 ( )) ||
      (getMask2 ( ) != cc.getMask2 ( )) ||
      (getMask3 ( ) != cc.getMask3 ( )) ||
      (getMask4 ( ) != cc.getMask4 ( )) ||
      (getMask5 ( ) != cc.getMask5 ( )) ||
      (getMask6 ( ) != cc.getMask6 ( )) ||
      (getMask7 ( ) != cc.getMask7 ( )) ||
      (getMask8 ( ) != cc.getMask8 ( )) ||
      (getMask9 ( ) != cc.getMask9 ( )) ||
      (getRes ( ) != cc.getRes ( ))
      )
    return true ;
  else
    return false ;
}

/** \brief In order to compare two CHIP descriptions
 * \param cc - CHIP description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool totemCChipDescription::operator  == ( totemCChipDescription &cc ) {

#ifdef DEBUGMSGERROR
  displayDifferences(cc) ;
#endif

  if (
      (getControl0 ( ) == cc.getControl0 ( )) &&
      (getControl1 ( ) == cc.getControl1 ( )) &&
      (getControl2 ( ) == cc.getControl2 ( )) &&
      (getControl3 ( ) == cc.getControl3 ( )) &&
      (getControl4 ( ) == cc.getControl4 ( )) &&
      (getChipid0 ( ) == cc.getChipid0 ( )) &&
      (getChipid1 ( ) == cc.getChipid1 ( )) &&
      (getCounter0 ( ) == cc.getCounter0 ( )) &&
      (getCounter1 ( ) == cc.getCounter1 ( )) &&
      (getCounter2 ( ) == cc.getCounter2 ( )) &&

      (getMask0 ( ) == cc.getMask0 ( )) &&
      (getMask1 ( ) == cc.getMask1 ( )) &&
      (getMask2 ( ) == cc.getMask2 ( )) &&
      (getMask3 ( ) == cc.getMask3 ( )) &&
      (getMask4 ( ) == cc.getMask4 ( )) &&
      (getMask5 ( ) == cc.getMask5 ( )) &&
      (getMask6 ( ) == cc.getMask6 ( )) &&
      (getMask7 ( ) == cc.getMask7 ( )) &&
      (getMask8 ( ) == cc.getMask8 ( )) &&
      (getMask9 ( ) == cc.getMask9 ( )) &&
      (getRes ( ) == cc.getRes ( ))
      )
    return true ;
  else
    return false ;
}


/** \brief Clone a device description
 * \return the device description cloned
 */
totemCChipDescription *totemCChipDescription::clone ( ) {

  totemCChipDescription* myCChipDescription = new totemCChipDescription( *this ) ;

  return myCChipDescription ;
}

/** \brief Display the CCHIP values where the comparison is different
 * \param cc - CHIP values uploaded
 */

void totemCChipDescription::displayDifferences ( totemCChipDescription &cc ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "CCHIP " << msg << std::endl ;
  if (getControl0 ( ) != cc.getControl0 ( )) 
    std::cout << "\tCont.Reg<0> is different " << std::dec << (int)getControl0 ( ) << "/" << (int)cc.getControl0 ( ) << std::endl ;
  if (getControl1 ( ) != cc.getControl1 ( )) 
    std::cout << "\tCont.Reg<1> is different " << std::dec << (int)getControl1 ( ) << "/" << (int)cc.getControl1 ( ) << std::endl ;
  if (getControl2 ( ) != cc.getControl2 ( )) 
    std::cout << "\tCont.Reg<2> is different " << std::dec << (int)getControl2 ( ) << "/" << (int)cc.getControl2 ( ) << std::endl ;
  if (getControl3 ( ) != cc.getControl3 ( )) 
    std::cout << "\tCont.Reg<3> is different " << std::dec << (int)getControl3 ( ) << "/" << (int)cc.getControl3 ( ) << std::endl ;
  if (getControl4 ( ) != cc.getControl4 ( )) 
    std::cout << "\tCont.Reg<4> is different " << std::dec << (int)getControl4 ( ) << "/" << (int)cc.getControl4 ( ) << std::endl ;
  if (getChipid0 ( ) != cc.getChipid0 ( )) 
    std::cout << "\tChipID<0> is different " << std::dec << (int)getChipid0 ( ) << "/" << (int)cc.getChipid0 ( ) << std::endl ;
  if (getChipid1 ( ) != cc.getChipid1 ( )) 
    std::cout << "\tChipID<1> is different " << std::dec << (int)getChipid1 ( ) << "/" << (int)cc.getChipid1 ( ) << std::endl ;
  if (getCounter0 ( ) != cc.getCounter0 ( )) 
    std::cout << "\tCounter<0> is different " << std::dec << (int)getCounter0 ( ) << "/" << (int)cc.getCounter0 ( ) << std::endl ;
  if (getCounter1 ( ) != cc.getCounter1 ( )) 
    std::cout << "\tCounter<1> is different " << std::dec << (int)getCounter1 ( ) << "/" << (int)cc.getCounter1 ( ) << std::endl ;
  if (getCounter2 ( ) != cc.getCounter2 ( )) 
    std::cout << "\tCounter<2> is different " << std::dec << (int)getCounter2 ( ) << "/" << (int)cc.getCounter2 ( ) << std::endl ;
  if (getMask0 ( ) != cc.getMask0 ( )) 
    std::cout << "\tMask<0> is different " << std::dec << (int)getMask0 ( ) << "/" << (int)cc.getMask0 ( ) << std::endl ;
  if (getMask1 ( ) != cc.getMask1 ( )) 
    std::cout << "\tMask<1> is different " << std::dec << (int)getMask1 ( ) << "/" << (int)cc.getMask1 ( ) << std::endl ;
  if (getMask2 ( ) != cc.getMask2 ( )) 
    std::cout << "\tMask<2> is different " << std::dec << (int)getMask2 ( ) << "/" << (int)cc.getMask2 ( ) << std::endl ;
  if (getMask3 ( ) != cc.getMask3 ( )) 
    std::cout << "\tMask<3> is different " << std::dec << (int)getMask3 ( ) << "/" << (int)cc.getMask3 ( ) << std::endl ;
  if (getMask4 ( ) != cc.getMask4 ( )) 
    std::cout << "\tMask<4> is different " << std::dec << (int)getMask4 ( ) << "/" << (int)cc.getMask4 ( ) << std::endl ;
  if (getMask5 ( ) != cc.getMask5 ( )) 
    std::cout << "\tMask<5> is different " << std::dec << (int)getMask5 ( ) << "/" << (int)cc.getMask5 ( ) << std::endl ;
  if (getMask6 ( ) != cc.getMask6 ( )) 
    std::cout << "\tMask<6> is different " << std::dec << (int)getMask6 ( ) << "/" << (int)cc.getMask6 ( ) << std::endl ;
  if (getMask7 ( ) != cc.getMask7 ( )) 
    std::cout << "\tMask<7> is different " << std::dec << (int)getMask7 ( ) << "/" << (int)cc.getMask7 ( ) << std::endl ;
  if (getMask8 ( ) != cc.getMask8 ( )) 
    std::cout << "\tMask<8> is different " << std::dec << (int)getMask8 ( ) << "/" << (int)cc.getMask8 ( ) << std::endl ;
  if (getMask9 ( ) != cc.getMask9 ( )) 
    std::cout << "\tMask<9> is different " << std::dec << (int)getMask9 ( ) << "/" << (int)cc.getMask9 ( ) << std::endl ;
  if (getRes ( ) != cc.getRes ( )) 
    std::cout << "\tResistor is different " << std::dec << (int)getRes ( ) << "/" << (int)cc.getRes ( ) << std::endl ;

}
  
/** \brief Display the CCHIP values
 */
void totemCChipDescription::display ( ) {

  std::cout << "CCHIP: 0x" 
	    << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;
  std::cout << std::endl ;

  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
  std::cout << "\tChipID<0>: " << std::dec << (tscType16)getChipid0() << std::endl ;
  std::cout << "\tChipID<1>: " << std::dec << (tscType16)getChipid1() << std::endl ;
  std::cout << std::endl ;
   
  std::cout << "\tCont.Reg<0>: " << std::dec << (tscType16)getControl0() << std::endl ;
  std::cout << "\tCont.Reg<1>: " << std::dec << (tscType16)getControl1() << std::endl ;
  std::cout << "\tCont.Reg<2>: " << std::dec << (tscType16)getControl2() << std::endl ;
  std::cout << "\tCont.Reg<3>: " << std::dec << (tscType16)getControl3() << std::endl ;
  std::cout << "\tCont.Reg<4>: " << std::dec << (tscType16)getControl4() << std::endl ;
  std::cout << std::endl ;

  std::cout << "\tCounter0: " << std::dec << (tscType16)getCounter0() << std::endl ;
  std::cout << "\tCounter1: " << std::dec << (tscType16)getCounter1() << std::endl ;
  std::cout << "\tCounter2: " << std::dec << (tscType16)getCounter2() << std::endl ;
  std::cout << std::endl ;

  std::cout << "\tMask<0>: " << std::dec << (tscType16)getMask0() << std::endl ;
  std::cout << "\tMask<1>: " << std::dec << (tscType16)getMask1() << std::endl ;
  std::cout << "\tMask<2>: " << std::dec << (tscType16)getMask2() << std::endl ;
  std::cout << "\tMask<3>: " << std::dec << (tscType16)getMask3() << std::endl ;
  std::cout << "\tMask<4>: " << std::dec << (tscType16)getMask4() << std::endl ;
  std::cout << "\tMask<5>: " << std::dec << (tscType16)getMask5() << std::endl ;
  std::cout << "\tMask<6>: " << std::dec << (tscType16)getMask6() << std::endl ;
  std::cout << "\tMask<7>: " << std::dec << (tscType16)getMask7() << std::endl ;
  std::cout << "\tMask<8>: " << std::dec << (tscType16)getMask8() << std::endl ;
  std::cout << "\tMask<9>: " << std::dec << (tscType16)getMask9() << std::endl ;
  std::cout << std::endl ;

  std::cout << "\tResistor: " << std::dec << (tscType16)getRes() << std::endl ;
  std::cout << std::endl ;

}

/** \brief Return a list of parameter names
 * \warning Must be delete by the owner of the description
 */
parameterDescriptionNameType *totemCChipDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;

  for (unsigned int i = 0 ; i < (sizeof(CCHIPPARAMETERNAMES)/sizeof(const char *)) ; i ++)
    {
      (*parameterNames)[CCHIPPARAMETERNAMES[i]] = new ParameterDescription(CCHIPPARAMETERNAMES[i], ParameterDescription::INTEGER8) ;
    }

  return parameterNames ;
}
 
/** Parameter's names
 */
const char *totemCChipDescription::CCHIPPARAMETERNAMES[] = { "cchipcontrol0",
							     "cchipcontrol1",
							     "cchipcontrol2",
							     "cchipcontrol3",
							     "cchipcontrol4",
							     "cchipchipid0",
							     "cchipchipid1",
							     "cchipcounter0",
							     "cchipcounter1",
							     "cchipcounter2",
							     "cchipmask0",
							     "cchipmask1",
							     "cchipmask2",
							     "cchipmask3",
							     "cchipmask4",
							     "cchipmask5",
							     "cchipmask6",
							     "cchipmask7",
							     "cchipmask8",
							     "cchipmask9",
							     "cchipres" } ;


