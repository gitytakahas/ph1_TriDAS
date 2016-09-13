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
  based on philipsDescription made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef VFATDESCRIPTION_H
#define VFATDESCRIPTION_H

#include "tscTypes.h"

#include "deviceDescription.h"
#include "vfatDefinition.h"

/**
 * \class vfatDescription
 * This class give a description of all the VFAT registers:
 * <ul>
 * <li>ControlReg<0>;
 * <li>ControlReg<1>;
 * <li>IPreampIn;
 * <li>IPreampFeed;
 * <li>IPreampOut;
 * <li>IShaper;
 * <li>IShaperFeed;
 * <li>IComp;
 * <li>ChipID<0>;
 * <li>ChipID<1>;
 * <li>UpsetReg;
 * <li>HitCount0;
 * <li>HitCount1;
 * <li>HitCount2;
 * <li>ExtRegPointer;
 * <li>ExtRegData;
 * <li>Latency;
 * <li>ChanReg<1 : 128>;
 * <li>VCalipration;
 * <li>VThreshold1;
 * <li>VThreshold2;
 * <li>CaliprationPhase;
 * <li>ControlReg<2>;
 * <li>ControlReg<3>;
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can also be set.
 * \see vfatDefinition.h that gives the definition of different registers.
 * \include "vfatDefinition.h"
 * \author Juha Petajajarvi
 * \date June 2006
 * \brief This class give a description of all the VFAT registers
 */

class vfatDescription: public deviceDescription {

 private:
  //Principal registers
  tscType8 control0 ;
  tscType8 control1 ;

  tscType8 ipreampin ;
  tscType8 ipreampfeed ;
  tscType8 ipreampout ;
  tscType8 ishaper ;
  tscType8 ishaperfeed ;
  tscType8 icomp ;

  tscType8 chipid0 ;
  tscType8 chipid1 ;

  tscType8 upset ;

  tscType8 hitcount0 ;
  tscType8 hitcount1 ;
  tscType8 hitcount2 ;

  tscType8 extregpointer ;
  tscType8 extregdata ;

  //Extended registers
  tscType8 lat ;
  tscType8 chanreg[129] ;
  tscType8 vcal ;
  tscType8 vthreshold1 ;
  tscType8 vthreshold2 ;
  tscType8 calphase ;
  tscType8 control2 ;
  tscType8 control3 ;
  
 public:

  enum{CONTROL0, CONTROL1, IPREAMPIN, IPREAMPFEED, IPREAMPOUT, ISHAPER,
       ISHAPERFEED, ICOMP, CHIPID0, CHIPID1, UPSET, HITCOUNT0, HITCOUNT1, HITCOUNT2, LAT, 
       CHANREG1, CHANREG2, CHANREG3, CHANREG4, CHANREG5, CHANREG6, CHANREG7, CHANREG8, CHANREG9, CHANREG10,
       CHANREG11, CHANREG12, CHANREG13, CHANREG14, CHANREG15, CHANREG16, CHANREG17, CHANREG18, CHANREG19, CHANREG20,
       CHANREG21, CHANREG22, CHANREG23, CHANREG24, CHANREG25, CHANREG26, CHANREG27, CHANREG28, CHANREG29, CHANREG30,
       CHANREG31, CHANREG32, CHANREG33, CHANREG34, CHANREG35, CHANREG36, CHANREG37, CHANREG38, CHANREG39, CHANREG40,
       CHANREG41, CHANREG42, CHANREG43, CHANREG44, CHANREG45, CHANREG46, CHANREG47, CHANREG48, CHANREG49, CHANREG50,
       CHANREG51, CHANREG52, CHANREG53, CHANREG54, CHANREG55, CHANREG56, CHANREG57, CHANREG58, CHANREG59, CHANREG60,
       CHANREG61, CHANREG62, CHANREG63, CHANREG64, CHANREG65, CHANREG66, CHANREG67, CHANREG68, CHANREG69, CHANREG70,
       CHANREG71, CHANREG72, CHANREG73, CHANREG74, CHANREG75, CHANREG76, CHANREG77, CHANREG78, CHANREG79, CHANREG80,
       CHANREG81, CHANREG82, CHANREG83, CHANREG84, CHANREG85, CHANREG86, CHANREG87, CHANREG88, CHANREG89, CHANREG90,
       CHANREG91, CHANREG92, CHANREG93, CHANREG94, CHANREG95, CHANREG96, CHANREG97, CHANREG98, CHANREG99, CHANREG100,
       CHANREG101, CHANREG102, CHANREG103, CHANREG104, CHANREG105, CHANREG106, CHANREG107, CHANREG108, CHANREG109, CHANREG110,
       CHANREG111, CHANREG112, CHANREG113, CHANREG114, CHANREG115, CHANREG116, CHANREG117, CHANREG118, CHANREG119, CHANREG120,
       CHANREG121, CHANREG122, CHANREG123, CHANREG124, CHANREG125, CHANREG126, CHANREG127, CHANREG128,
       VCAL, VTHRESHOLD1, VTHRESHOLD2, CALPHASE, CONTROL2, CONTROL3} ;

  /** Parameter's names
   */
  static const char *VFATPARAMETERNAMES[CONTROL3+1] ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a VFAT with all the values specified:
   * \param accessKey - device key
   */
  vfatDescription (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of an VFAT with all the values specified:
   * \param accessKey - key access of a device
   * \param control0_ - Vfat Register
   * \param control1_ - Vfat Register
   * \param ipreampin_ - Vfat Register
   * \param ipreampfeed_ - Vfat Register
   * \param ipreampout_ - Vfat Register
   * \param ishaper_ - Vfat Register
   * \param ishaperfeed_ - Vfat Register
   * \param icomp_ - Vfat Register
   * \param chipid0_ - Vfat Register
   * \param chipid1_ - Vfat Register
   * \param upset_ - Vfat Register
   * \param hitcount0_ - Vfat Register
   * \param hitcount1_ - Vfat Register
   * \param hitcount2_ - Vfat Register
   * \param lat_ - Vfat Register
   * \param chanreg_ - All Vfat Channel Registers
   * \param vcal - Vfat Register
   * \param vthreshold1_ - Vfat Register
   * \param vthreshold2_ - Vfat Register
   * \param calphase_ - Vfat Register
   * \param control2_ - Vfat Register
   * \param control3_ - Vfat Register
   * \warning all channel registers are initialized with same value
   */
  vfatDescription ( keyType accessKey,
                    tscType8 control0_,
                    tscType8 control1_,
                    tscType8 ipreampin_,
                    tscType8 ipreampfeed_,
                    tscType8 ipreampout_,
                    tscType8 ishaper_,
                    tscType8 ishaperfeed_,
                    tscType8 icomp_,

                    tscType8 chipid0_,
                    tscType8 chipid1_,
                    tscType8 upset_,
                    tscType8 hitcount0_,
                    tscType8 hitcount1_,
                    tscType8 hitcount2_,

                    tscType8 lat_,
                    tscType8 chanreg_,
                    tscType8 vcal_,
                    tscType8 vthreshold1_,
                    tscType8 vthreshold2_,
                    tscType8 calphase_,
                    tscType8 control2_,
                    tscType8 control3_ ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of an VFAT with all the values specified:
   * \param accessKey - key access of a device
   * \param control0_ - Vfat Register
   * \param control1_ - Vfat Register
   * \param ipreampin_ - Vfat Register
   * \param ipreampfeed_ - Vfat Register
   * \param ipreampout_ - Vfat Register
   * \param ishaper_ - Vfat Register
   * \param ishaperfeed_ - Vfat Register
   * \param icomp_ - Vfat Register
   * \param chipid0_ - Vfat Register
   * \param chipid1_ - Vfat Register
   * \param upset_ - Vfat Register
   * \param hitcount0_ - Vfat Register
   * \param hitcount1_ - Vfat Register
   * \param hitcount2_ - Vfat Register
   * \param lat_ - Vfat Register
   * \param *chanreg_ - Vfat Channel Registers
   * \param vcal - Vfat Register
   * \param vthreshold1_ - Vfat Register
   * \param vthreshold2_ - Vfat Register
   * \param calphase_ - Vfat Register
   * \param control2_ - Vfat Register
   * \param control3_ - Vfat Register
   */
  vfatDescription ( keyType accessKey,
                    tscType8 control0_,
                    tscType8 control1_,
                    tscType8 ipreampin_,
                    tscType8 ipreampfeed_,
                    tscType8 ipreampout_,
                    tscType8 ishaper_,
                    tscType8 ishaperfeed_,
                    tscType8 icomp_,

                    tscType8 chipid0_,
                    tscType8 chipid1_,
                    tscType8 upset_,
                    tscType8 hitcount0_,
                    tscType8 hitcount1_,
                    tscType8 hitcount2_,

                    tscType8 lat_,
                    tscType8 *chanreg_,
                    tscType8 vcal_,
                    tscType8 vthreshold1_,
                    tscType8 vthreshold2_,
                    tscType8 calphase_,
                    tscType8 control2_,
                    tscType8 control3_ ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a vfat with all the values specified:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   * \param control0_ - Vfat Register
   * \param control1_ - Vfat Register
   * \param ipreampin_ - Vfat Register
   * \param ipreampfeed_ - Vfat Register
   * \param ipreampout_ - Vfat Register
   * \param ishaper_ - Vfat Register
   * \param ishaperfeed_ - Vfat Register
   * \param icomp_ - Vfat Register
   * \param chipid0_ - Vfat Register
   * \param chipid1_ - Vfat Register
   * \param upset_ - Vfat Register
   * \param hitcount0_ - Vfat Register
   * \param hitcount1_ - Vfat Register
   * \param hitcount2_ - Vfat Register
   * \param lat_ - Vfat Register
   * \param *chanreg_ - Vfat Channel Registers
   * \param vcal - Vfat Register
   * \param vthreshold1_ - Vfat Register
   * \param vthreshold2_ - Vfat Register
   * \param calphase_ - Vfat Register
   * \param control2_ - Vfat Register
   * \param control3_ - Vfat Register
   */
  vfatDescription ( tscType16 fecSlot,
                    tscType16 ringSlot,
                    tscType16 ccuAddress,
                    tscType16 i2cChannel,
                    tscType16 i2cAddress,
                    tscType8 control0_,
                    tscType8 control1_,
                    tscType8 ipreampin_,
                    tscType8 ipreampfeed_,
                    tscType8 ipreampout_,
                    tscType8 ishaper_,
                    tscType8 ishaperfeed_,
                    tscType8 icomp_,

                    tscType8 chipid0_,
                    tscType8 chipid1_,
                    tscType8 upset_,
                    tscType8 hitcount0_,
                    tscType8 hitcount1_,
                    tscType8 hitcount2_,

                    tscType8 lat_,
                    tscType8 *chanreg_,
                    tscType8 vcal_,
                    tscType8 vthreshold1_,
                    tscType8 vthreshold2_,
                    tscType8 calphase_,
                    tscType8 control2_,
                    tscType8 control3_ ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of an VFAT with all the values specified:
   * \param control0_ - Vfat Register
   * \param control1_ - Vfat Register
   * \param ipreampin_ - Vfat Register
   * \param ipreampfeed_ - Vfat Register
   * \param ipreampout_ - Vfat Register
   * \param ishaper_ - Vfat Register
   * \param ishaperfeed_ - Vfat Register
   * \param icomp_ - Vfat Register
   * \param chipid0_ - Vfat Register
   * \param chipid1_ - Vfat Register
   * \param upset_ - Vfat Register
   * \param hitcount0_ - Vfat Register
   * \param hitcount1_ - Vfat Register
   * \param hitcount2_ - Vfat Register
   * \param lat_ - Vfat Register
   * \param chanreg_ - All Vfat Channel Registers
   * \param vcal - Vfat Register
   * \param vthreshold1_ - Vfat Register
   * \param vthreshold2_ - Vfat Register
   * \param calphase_ - Vfat Register
   * \param control2_ - Vfat Register
   * \param control3_ - Vfat Register
   * \warning all channel registers are initialized with same value
   */
  vfatDescription ( tscType8 control0_,
                    tscType8 control1_,
                    tscType8 ipreampin_,
                    tscType8 ipreampfeed_,
                    tscType8 ipreampout_,
                    tscType8 ishaper_,
                    tscType8 ishaperfeed_,
                    tscType8 icomp_,

                    tscType8 chipid0_,
                    tscType8 chipid1_,
                    tscType8 upset_,
                    tscType8 hitcount0_,
                    tscType8 hitcount1_,
                    tscType8 hitcount2_,

                    tscType8 lat_,
                    tscType8 chanreg_,
                    tscType8 vcal_,
                    tscType8 vthreshold1_,
                    tscType8 vthreshold2_,
                    tscType8 calphase_,
                    tscType8 control2_,
                    tscType8 control3_ ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a vfat with all the values specified:
   * \param control0_ - Vfat Register
   * \param control1_ - Vfat Register
   * \param ipreampin_ - Vfat Register
   * \param ipreampfeed_ - Vfat Register
   * \param ipreampout_ - Vfat Register
   * \param ishaper_ - Vfat Register
   * \param ishaperfeed_ - Vfat Register
   * \param icomp_ - Vfat Register
   * \param chipid0_ - Vfat Register
   * \param chipid1_ - Vfat Register
   * \param upset_ - Vfat Register
   * \param hitcount0_ - Vfat Register
   * \param hitcount1_ - Vfat Register
   * \param hitcount2_ - Vfat Register
   * \param lat_ - Vfat Register
   * \param *chanreg_ - Vfat Channel Registers
   * \param vcal - Vfat Register
   * \param vthreshold1_ - Vfat Register
   * \param vthreshold2_ - Vfat Register
   * \param calphase_ - Vfat Register
   * \param control2_ - Vfat Register
   * \param control3_ - Vfat Register
   */
  vfatDescription ( tscType8 control0_,
                    tscType8 control1_,
                    tscType8 ipreampin_,
                    tscType8 ipreampfeed_,
                    tscType8 ipreampout_,
                    tscType8 ishaper_,
                    tscType8 ishaperfeed_,
                    tscType8 icomp_,

                    tscType8 chipid0_,
                    tscType8 chipid1_,
                    tscType8 upset_,
                    tscType8 hitcount0_,
                    tscType8 hitcount1_,
                    tscType8 hitcount2_,

                    tscType8 lat_,
                    tscType8 *chanreg_,
                    tscType8 vcal_,
                    tscType8 vthreshold1_,
                    tscType8 vthreshold2_,
                    tscType8 calphase_,
                    tscType8 control2_,
                    tscType8 control3_ ) ;



  /* \brief Constructor to build a description of a vfat with a list of parameters names
   * \param parameterNames - list of parameters
   * \see ParameterDescription class
   */
  vfatDescription ( parameterDescriptionNameType parameterNames ) ;
   
  /** \brief Method in order to set all the parameters
   * Build a description of a vfat with all the values specified
   * \param control0_ - Vfat Register
   * \param control1_ - Vfat Register
   * \param ipreampin_ - Vfat Register
   * \param ipreampfeed_ - Vfat Register
   * \param ipreampout_ - Vfat Register
   * \param ishaper_ - Vfat Register
   * \param ishaperfeed_ - Vfat Register
   * \param icomp_ - Vfat Register
   * \param chipid0_ - Vfat Register
   * \param chipid1_ - Vfat Register
   * \param upset_ - Vfat Register
   * \param hitcount0_ - Vfat Register
   * \param hitcount1_ - Vfat Register
   * \param hitcount2_ - Vfat Register
   * \param lat_ - Vfat Register
   * \param *chanreg_ - Vfat Channel Registers
   * \param vcal - Vfat Register
   * \param vthreshold1_ - Vfat Register
   * \param vthreshold2_ - Vfat Register
   * \param calphase_ - Vfat Register
   * \param control2_ - Vfat Register
   * \param control3_ - Vfat Register
   */
  void setDescriptionValues ( tscType8 control0_,
                              tscType8 control1_,
                              tscType8 ipreampin_,
                              tscType8 ipreampfeed_,
                              tscType8 ipreampout_,
                              tscType8 ishaper_,
                              tscType8 ishaperfeed_,
                              tscType8 icomp_,

                              tscType8 chipid0_,
                              tscType8 chipid1_,
                              tscType8 upset_,
			      tscType8 hitcount0_,
			      tscType8 hitcount1_,
                              tscType8 hitcount2_,

                              tscType8 lat_,
                              tscType8 *chanreg_,
                              tscType8 vcal_,
                              tscType8 vthreshold1_,
                              tscType8 vthreshold2_,
                              tscType8 calphase_,
                              tscType8 control2_,
                              tscType8 control3_ ) ;
  
  /** \brief Method in order to set default values to vfat registers
   * Set default values to vfat registers
   * \warning readonly registers are not set
   */
  void setDescriptionDefaultValues ( ) ;

  //Set and Get functions - PRINCIPAL

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getControl0 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setControl0 ( tscType8 value ) ;
 
 /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getControl1 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setControl1   ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getIPreampIn ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setIPreampIn  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getIPreampFeed ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setIPreampFeed  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getIPreampOut ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setIPreampOut  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getIShaper ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setIShaper  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getIShaperFeed ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setIShaperFeed  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getIComp ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setIComp  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getChipid0 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setChipid0  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getChipid1 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setChipid1  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getUpset ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setUpset  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getHitCount0 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setHitCount0  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getHitCount1 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setHitCount1  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getHitCount2 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setHitCount2  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getExtRegPointer ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setExtRegPointer  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getExtRegData ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setExtRegData  ( tscType8 value ) ;

  //Set and Get functions - EXTENDED

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getLat ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setLat  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT channel register corresponding to a VFAT register
   * Return the value of the VFAT channel register corresponding to a VFAT register
   * \param index - index
   * \return the value
   */
  tscType8 getChanReg ( int index ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param index - index
   * \param value - value to be set
   */
  void setChanReg ( int index, tscType8 value ) ;

  /** \brief Return the pointer of the VFAT channel registers corresponding to a VFAT channel registers' values
   * Return the pointer of the VFAT channel registers corresponding to a VFAT channel registers' values
   * \return the pointer
   */
  tscType8 *getAllChanReg () ;

  /** \brief set the values for all channel registers
   * Set the values for all channel registers
   * \param *value - pointer to values to be set
   */
  void setAllChanReg ( tscType8 *value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getVCal ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setVCal  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getVThreshold1 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setVThreshold1  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getVThreshold2 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setVThreshold2  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getCalPhase ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setCalPhase  ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getControl2 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setControl2 ( tscType8 value ) ;

  /** \brief Return the value of the VFAT register corresponding to a VFAT register
   * Return the value of the VFAT register corresponding to a VFAT register
   * \return the value
   */
  tscType8 getControl3 ( ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param value to be set
   */
  void setControl3 ( tscType8 value ) ;

  /** \brief In order to compare two Vfat descriptions
   * \param vfat - Vfat description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( vfatDescription &vfat ) ;

  /** \brief In order to compare two Vfat descriptions
   * \param vfat - vfat description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator  == ( vfatDescription &vfat ) ;


  /** Clone a device description
   * \return the device description cloned
   */
  vfatDescription *clone ( ) ;

  /** \brief Display the vfat values where the comparison is different
   * \param vfat - vfat values uploaded
   */
  void displayDifferences ( vfatDescription &vfat ) ;
  
  /** \brief Display the vfat values
   */
  void display ( ) ;

  /** \brief Return a list of parameter names
   * must be delete by the owner of the description
   */

  static parameterDescriptionNameType *getParameterNames ( ) ;
};

#endif
