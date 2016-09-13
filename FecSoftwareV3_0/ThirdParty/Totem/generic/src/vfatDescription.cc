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

#include "vfatDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a VFAT with all the values specified:
 * \param accessKey - device key
 */
vfatDescription::vfatDescription (keyType accessKey): deviceDescription ( VFAT, accessKey ) { }

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
vfatDescription::vfatDescription ( keyType accessKey,
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
				   tscType8 control3_ ):

  deviceDescription ( VFAT, accessKey ) {

  setControl0 (control0_) ;
  setControl1 (control1_) ;
  setIPreampIn (ipreampin_) ;
  setIPreampFeed (ipreampfeed_) ;
  setIPreampOut (ipreampout_) ;
  setIShaper (ishaper_) ;
  setIShaperFeed (ishaperfeed_) ;
  setIComp (icomp_) ;

  setChipid0 (chipid0_) ;
  setChipid1 (chipid1_) ;
  setUpset (upset_) ;
  setHitCount0 (hitcount0_) ;
  setHitCount1 (hitcount1_) ;
  setHitCount2 (hitcount2_) ;

  setLat (lat_) ;

  for (int i=1;i<=128;i++) { setChanReg(i, chanreg_) ; }
 
  setVCal (vcal_) ;
  setVThreshold1 (vthreshold1_) ;
  setVThreshold2 (vthreshold2_) ;
  setCalPhase (calphase_) ;
  setControl2 (control2_) ;
  setControl3 (control3_) ;
}

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
vfatDescription::vfatDescription ( keyType accessKey,
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
				   tscType8 control3_ ):

  deviceDescription ( VFAT, accessKey ) {

  setControl0 (control0_) ;
  setControl1 (control1_) ;
  setIPreampIn (ipreampin_) ;
  setIPreampFeed (ipreampfeed_) ;
  setIPreampOut (ipreampout_) ;
  setIShaper (ishaper_) ;
  setIShaperFeed (ishaperfeed_) ;
  setIComp (icomp_) ;

  setChipid0 (chipid0_) ;
  setChipid1 (chipid1_) ;
  setUpset (upset_) ;
  setHitCount0 (hitcount0_) ;
  setHitCount1 (hitcount1_) ;
  setHitCount2 (hitcount2_) ;

  setLat (lat_) ;
  setAllChanReg (chanreg_) ;
  setVCal (vcal_) ;
  setVThreshold1 (vthreshold1_) ;
  setVThreshold2 (vthreshold2_) ;
  setCalPhase (calphase_) ;
  setControl2 (control2_) ;
  setControl3 (control3_) ;
}


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
vfatDescription::vfatDescription ( tscType16 fecSlot,
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
				   tscType8 control3_ ):

  deviceDescription ( VFAT, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setControl0 (control0_) ;
  setControl1 (control1_) ;
  setIPreampIn (ipreampin_) ;
  setIPreampFeed (ipreampfeed_) ;
  setIPreampOut (ipreampout_) ;
  setIShaper (ishaper_) ;
  setIShaperFeed (ishaperfeed_) ;
  setIComp (icomp_) ;

  setChipid0 (chipid0_) ;
  setChipid1 (chipid1_) ;
  setUpset (upset_) ;
  setHitCount0 (hitcount0_) ;
  setHitCount1 (hitcount1_) ;
  setHitCount2 (hitcount2_) ;

  setLat (lat_) ;
  setAllChanReg (chanreg_) ;
  setVCal (vcal_) ;
  setVThreshold1 (vthreshold1_) ;
  setVThreshold2 (vthreshold2_) ;
  setCalPhase (calphase_) ;
  setControl2 (control2_) ;
  setControl3 (control3_) ;
}

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
vfatDescription::vfatDescription ( tscType8 control0_,
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
				   tscType8 control3_ ):

  deviceDescription ( VFAT ) {

  setControl0 (control0_) ;
  setControl1 (control1_) ;
  setIPreampIn (ipreampin_) ;
  setIPreampFeed (ipreampfeed_) ;
  setIPreampOut (ipreampout_) ;
  setIShaper (ishaper_) ;
  setIShaperFeed (ishaperfeed_) ;
  setIComp (icomp_) ;

  setChipid0 (chipid0_) ;
  setChipid1 (chipid1_) ;
  setUpset (upset_) ;
  setHitCount0 (hitcount0_) ;
  setHitCount1 (hitcount1_) ;
  setHitCount2 (hitcount2_) ;

  setLat (lat_) ;

  for (int i=1;i<=128;i++) { setChanReg(i, chanreg_) ; }

  setVCal (vcal_) ;
  setVThreshold1 (vthreshold1_) ;
  setVThreshold2 (vthreshold2_) ;
  setCalPhase (calphase_) ;
  setControl2 (control2_) ;
  setControl3 (control3_) ;
}

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
vfatDescription::vfatDescription ( tscType8 control0_,
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
				   tscType8 control3_ ):

  deviceDescription ( VFAT ) {

  setDescriptionValues ( control0_, 
			 control1_, 
			 ipreampin_, 
			 ipreampfeed_,
			 ipreampout_,
			 ishaper_,
			 ishaperfeed_,
			 icomp_,

			 chipid0_,
			 chipid1_,
			 upset_,
			 hitcount0_,
			 hitcount1_,
			 hitcount2_,

			 lat_,
			 chanreg_,
			 vcal_,
			 vthreshold1_,
			 vthreshold2_,
			 calphase_,
			 control2_,
			 control3_ );
}



/* \brief Constructor to build a description of a vfat with a list of parameters names
 * \param parameterNames - list of parameters
 * \see ParameterDescription class
 */
vfatDescription::vfatDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( VFAT, parameterNames) {

  setControl0     (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[CONTROL0]]->getValueConverted())) ;
  setControl1     (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[CONTROL1]]->getValueConverted())) ;
  setIPreampIn    (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[IPREAMPIN]]->getValueConverted())) ;
  setIPreampFeed  (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[IPREAMPFEED]]->getValueConverted())) ;
  setIPreampOut   (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[IPREAMPOUT]]->getValueConverted())) ;
  setIShaper      (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[ISHAPER]]->getValueConverted())) ;
  setIShaperFeed  (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[ISHAPERFEED]]->getValueConverted())) ;
  setIComp        (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[ICOMP]]->getValueConverted())) ;
  setChipid0      (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHIPID0]]->getValueConverted())) ;
  setChipid1      (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHIPID1]]->getValueConverted())) ;
  setUpset        (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[UPSET]]->getValueConverted())) ;
  setHitCount0    (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[HITCOUNT0]]->getValueConverted())) ;
  setHitCount1    (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[HITCOUNT1]]->getValueConverted())) ;
  setHitCount2    (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[HITCOUNT2]]->getValueConverted())) ;
  setLat          (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[LAT]]->getValueConverted())) ;

  setChanReg      (1, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG1]]->getValueConverted())) ;
  setChanReg      (2, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG2]]->getValueConverted())) ;
  setChanReg      (3, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG3]]->getValueConverted())) ;
  setChanReg      (4, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG4]]->getValueConverted())) ;
  setChanReg      (5, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG5]]->getValueConverted())) ;
  setChanReg      (6, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG6]]->getValueConverted())) ;
  setChanReg      (7, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG7]]->getValueConverted())) ;
  setChanReg      (8, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG8]]->getValueConverted())) ;
  setChanReg      (9, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG9]]->getValueConverted())) ;
  setChanReg      (10, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG10]]->getValueConverted())) ;
  setChanReg      (11, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG11]]->getValueConverted())) ;
  setChanReg      (12, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG12]]->getValueConverted())) ;
  setChanReg      (13, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG13]]->getValueConverted())) ;
  setChanReg      (14, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG14]]->getValueConverted())) ;
  setChanReg      (15, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG15]]->getValueConverted())) ;
  setChanReg      (16, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG16]]->getValueConverted())) ;
  setChanReg      (17, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG17]]->getValueConverted())) ;
  setChanReg      (18, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG18]]->getValueConverted())) ;
  setChanReg      (19, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG19]]->getValueConverted())) ;
  setChanReg      (20, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG20]]->getValueConverted())) ;
  setChanReg      (21, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG21]]->getValueConverted())) ;
  setChanReg      (22, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG22]]->getValueConverted())) ;
  setChanReg      (23, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG23]]->getValueConverted())) ;
  setChanReg      (24, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG24]]->getValueConverted())) ;
  setChanReg      (25, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG25]]->getValueConverted())) ;
  setChanReg      (26, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG26]]->getValueConverted())) ;
  setChanReg      (27, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG27]]->getValueConverted())) ;
  setChanReg      (28, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG28]]->getValueConverted())) ;
  setChanReg      (29, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG29]]->getValueConverted())) ;
  setChanReg      (30, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG30]]->getValueConverted())) ;
  setChanReg      (31, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG31]]->getValueConverted())) ;
  setChanReg      (32, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG32]]->getValueConverted())) ;
  setChanReg      (33, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG33]]->getValueConverted())) ;
  setChanReg      (34, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG34]]->getValueConverted())) ;
  setChanReg      (35, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG35]]->getValueConverted())) ;
  setChanReg      (36, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG36]]->getValueConverted())) ;
  setChanReg      (37, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG37]]->getValueConverted())) ;
  setChanReg      (38, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG38]]->getValueConverted())) ;
  setChanReg      (39, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG39]]->getValueConverted())) ;
  setChanReg      (40, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG40]]->getValueConverted())) ;
  setChanReg      (41, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG41]]->getValueConverted())) ;
  setChanReg      (42, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG42]]->getValueConverted())) ;
  setChanReg      (43, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG43]]->getValueConverted())) ;
  setChanReg      (44, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG44]]->getValueConverted())) ;
  setChanReg      (45, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG45]]->getValueConverted())) ;
  setChanReg      (46, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG46]]->getValueConverted())) ;
  setChanReg      (47, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG47]]->getValueConverted())) ;
  setChanReg      (48, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG48]]->getValueConverted())) ;
  setChanReg      (49, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG49]]->getValueConverted())) ;
  setChanReg      (50, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG50]]->getValueConverted())) ;
  setChanReg      (51, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG51]]->getValueConverted())) ;
  setChanReg      (52, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG52]]->getValueConverted())) ;
  setChanReg      (53, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG53]]->getValueConverted())) ;
  setChanReg      (54, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG54]]->getValueConverted())) ;
  setChanReg      (55, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG55]]->getValueConverted())) ;
  setChanReg      (56, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG56]]->getValueConverted())) ;
  setChanReg      (57, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG57]]->getValueConverted())) ;
  setChanReg      (58, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG58]]->getValueConverted())) ;
  setChanReg      (59, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG59]]->getValueConverted())) ;
  setChanReg      (60, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG60]]->getValueConverted())) ;
  setChanReg      (61, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG61]]->getValueConverted())) ;
  setChanReg      (62, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG62]]->getValueConverted())) ;
  setChanReg      (63, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG63]]->getValueConverted())) ;
  setChanReg      (64, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG64]]->getValueConverted())) ;
  setChanReg      (65, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG65]]->getValueConverted())) ;
  setChanReg      (66, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG66]]->getValueConverted())) ;
  setChanReg      (67, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG67]]->getValueConverted())) ;
  setChanReg      (68, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG68]]->getValueConverted())) ;
  setChanReg      (69, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG69]]->getValueConverted())) ;
  setChanReg      (70, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG70]]->getValueConverted())) ;
  setChanReg      (71, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG71]]->getValueConverted())) ;
  setChanReg      (72, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG72]]->getValueConverted())) ;
  setChanReg      (73, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG73]]->getValueConverted())) ;
  setChanReg      (74, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG74]]->getValueConverted())) ;
  setChanReg      (75, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG75]]->getValueConverted())) ;
  setChanReg      (76, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG76]]->getValueConverted())) ;
  setChanReg      (77, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG77]]->getValueConverted())) ;
  setChanReg      (78, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG78]]->getValueConverted())) ;
  setChanReg      (79, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG79]]->getValueConverted())) ;
  setChanReg      (80, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG80]]->getValueConverted())) ;
  setChanReg      (81, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG81]]->getValueConverted())) ;
  setChanReg      (82, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG82]]->getValueConverted())) ;
  setChanReg      (83, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG83]]->getValueConverted())) ;
  setChanReg      (84, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG84]]->getValueConverted())) ;
  setChanReg      (85, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG85]]->getValueConverted())) ;
  setChanReg      (86, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG86]]->getValueConverted())) ;
  setChanReg      (87, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG87]]->getValueConverted())) ;
  setChanReg      (88, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG88]]->getValueConverted())) ;
  setChanReg      (89, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG89]]->getValueConverted())) ;
  setChanReg      (90, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG90]]->getValueConverted())) ;
  setChanReg      (91, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG91]]->getValueConverted())) ;
  setChanReg      (92, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG92]]->getValueConverted())) ;
  setChanReg      (93, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG93]]->getValueConverted())) ;
  setChanReg      (94, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG94]]->getValueConverted())) ;
  setChanReg      (95, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG95]]->getValueConverted())) ;
  setChanReg      (96, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG96]]->getValueConverted())) ;
  setChanReg      (97, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG97]]->getValueConverted())) ;
  setChanReg      (98, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG98]]->getValueConverted())) ;
  setChanReg      (99, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG99]]->getValueConverted())) ;
  setChanReg      (100, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG100]]->getValueConverted())) ;
  setChanReg      (101, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG101]]->getValueConverted())) ;
  setChanReg      (102, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG102]]->getValueConverted())) ;
  setChanReg      (103, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG103]]->getValueConverted())) ;
  setChanReg      (104, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG104]]->getValueConverted())) ;
  setChanReg      (105, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG105]]->getValueConverted())) ;
  setChanReg      (106, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG106]]->getValueConverted())) ;
  setChanReg      (107, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG107]]->getValueConverted())) ;
  setChanReg      (108, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG108]]->getValueConverted())) ;
  setChanReg      (109, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG109]]->getValueConverted())) ;
  setChanReg      (110, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG110]]->getValueConverted())) ;
  setChanReg      (111, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG111]]->getValueConverted())) ;
  setChanReg      (112, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG112]]->getValueConverted())) ;
  setChanReg      (113, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG113]]->getValueConverted())) ;
  setChanReg      (114, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG114]]->getValueConverted())) ;
  setChanReg      (115, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG115]]->getValueConverted())) ;
  setChanReg      (116, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG116]]->getValueConverted())) ;
  setChanReg      (117, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG117]]->getValueConverted())) ;
  setChanReg      (118, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG118]]->getValueConverted())) ;
  setChanReg      (119, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG119]]->getValueConverted())) ;
  setChanReg      (120, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG120]]->getValueConverted())) ;
  setChanReg      (121, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG121]]->getValueConverted())) ;
  setChanReg      (122, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG122]]->getValueConverted())) ;
  setChanReg      (123, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG123]]->getValueConverted())) ;
  setChanReg      (124, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG124]]->getValueConverted())) ;
  setChanReg      (125, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG125]]->getValueConverted())) ;
  setChanReg      (126, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG126]]->getValueConverted())) ;
  setChanReg      (127, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG127]]->getValueConverted())) ;
  setChanReg      (128, *((tscType8 *) parameterNames[VFATPARAMETERNAMES[CHANREG128]]->getValueConverted())) ;

  setVCal         (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[VCAL]]->getValueConverted())) ;
  setVThreshold1  (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[VTHRESHOLD1]]->getValueConverted())) ;
  setVThreshold2  (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[VTHRESHOLD2]]->getValueConverted())) ;
  setCalPhase     (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[CALPHASE]]->getValueConverted())) ;
  setControl2     (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[CONTROL2]]->getValueConverted())) ;
  setControl3     (*((tscType8 *) parameterNames[VFATPARAMETERNAMES[CONTROL3]]->getValueConverted())) ;
}

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
void vfatDescription::setDescriptionValues ( tscType8 control0_,
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
					     tscType8 control3_ ){

  setControl0 (control0_) ;
  setControl1 (control1_) ;
  setIPreampIn (ipreampin_) ;
  setIPreampFeed (ipreampfeed_) ;
  setIPreampOut (ipreampout_) ;
  setIShaper (ishaper_) ;
  setIShaperFeed (ishaperfeed_) ;
  setIComp (icomp_) ;

  setChipid0 (chipid0_) ;
  setChipid1 (chipid1_) ;
  setUpset (upset_) ;
  setHitCount0 (hitcount0_) ;
  setHitCount1 (hitcount1_) ;
  setHitCount2 (hitcount2_) ;

  setLat (lat_) ;
  setAllChanReg (chanreg_) ;
  setVCal (vcal_) ;
  setVThreshold1 (vthreshold1_) ;
  setVThreshold2 (vthreshold2_) ;
  setCalPhase (calphase_) ;
  setControl2 (control2_) ;
  setControl3 (control3_) ;
}
  
/** \brief Method in order to set default values to vfat registers
 * Set default values to vfat registers
 * \warning readonly registers are not set
 */
void vfatDescription::setDescriptionDefaultValues ( ){

  setControl0 (VFAT_CONTROL0_DEFAULT) ;
  setControl1 (VFAT_CONTROL1_DEFAULT) ;
  setIPreampIn (VFAT_IPREAMPIN_DEFAULT) ;
  setIPreampFeed (VFAT_IPREAMPFEED_DEFAULT) ;
  setIPreampOut (VFAT_IPREAMPOUT_DEFAULT) ;
  setIShaper (VFAT_ISHAPER_DEFAULT) ;
  setIShaperFeed (VFAT_ISHAPERFEED_DEFAULT) ;
  setIComp (VFAT_ICOMP_DEFAULT) ;
  setChipid0 (VFAT_CHIPID0_DEFAULT) ;
  setChipid1 (VFAT_CHIPID1_DEFAULT) ;
  setUpset (VFAT_UPSET_DEFAULT) ;
  setHitCount0 (VFAT_HITCOUNT0_DEFAULT) ;
  setHitCount1 (VFAT_HITCOUNT1_DEFAULT) ;
  setHitCount2 (VFAT_HITCOUNT2_DEFAULT) ;
  setLat (VFAT_LATENCY_DEFAULT) ;

  for (int i=1;i<=128;i++) { setChanReg(i, VFAT_CHANREG_DEFAULT) ; }

  setVCal (VFAT_VCAL_DEFAULT) ;
  setVThreshold1 (VFAT_THRESHOLD1_DEFAULT) ;
  setVThreshold2 (VFAT_THRESHOLD2_DEFAULT) ;
  setCalPhase (VFAT_CALPHASE_DEFAULT) ;
  setControl2 (VFAT_CONTROL2_DEFAULT) ;
  setControl3 (VFAT_CONTROL3_DEFAULT) ;
}

//Set and Get functions - PRINCIPAL

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getControl0 ( ) { return (control0) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setControl0   ( tscType8 value ) { control0 = value ; }
 
/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getControl1 ( ) { return (control1) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setControl1   ( tscType8 value ) { control1 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getIPreampIn ( ) { return (ipreampin) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setIPreampIn  ( tscType8 value ) { ipreampin = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getIPreampFeed ( ) { return (ipreampfeed) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setIPreampFeed  ( tscType8 value ) { ipreampfeed = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getIPreampOut ( ) { return (ipreampout) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setIPreampOut  ( tscType8 value ) { ipreampout = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getIShaper ( ) { return (ishaper) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setIShaper  ( tscType8 value ) { ishaper = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getIShaperFeed ( ) { return (ishaperfeed) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setIShaperFeed  ( tscType8 value ) { ishaperfeed = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getIComp ( ) { return (icomp) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setIComp  ( tscType8 value ) { icomp = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getChipid0 ( ) { return (chipid0) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setChipid0  ( tscType8 value ) { chipid0 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getChipid1 ( ) { return (chipid1) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setChipid1  ( tscType8 value ) { chipid1 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getUpset ( ) { return (upset) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setUpset  ( tscType8 value ) { upset = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getHitCount0 ( ) { return (hitcount0) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setHitCount0  ( tscType8 value ) { hitcount0 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getHitCount1 ( ) { return (hitcount1) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setHitCount1  ( tscType8 value ) { hitcount1 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getHitCount2 ( ) { return (hitcount2) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setHitCount2  ( tscType8 value ) { hitcount2 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getExtRegPointer ( ) { return (extregpointer) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setExtRegPointer  ( tscType8 value ) { extregpointer = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getExtRegData ( ) { return (extregdata) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setExtRegData  ( tscType8 value ) { extregdata = value ; }

//Set and Get functions - EXTENDED

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getLat ( ) { return (lat) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setLat  ( tscType8 value ) { lat = value ; }

/** \brief Return the value of the VFAT channel register corresponding to a VFAT register
 * Return the value of the VFAT channel register corresponding to a VFAT register
 * \param index - index
 * \return the value
 */
tscType8 vfatDescription::getChanReg ( int index ) { 

  if(index>0 && index<129){ return (chanreg[index]) ; }
  else { std::cout << "Index given was out of array boundaries." << std::endl ; return((tscType8)0x00) ; }
}

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param index - index
 * \param value - value to be set
 */
void vfatDescription::setChanReg ( int index, tscType8 value ) { 

  if(index>0 && index<129){ chanreg[index] = value ; }
  else { std::cout << "Index given was out of array boundaries." << std::endl ; }
}

/** \brief Return the pointer of the VFAT channel registers corresponding to a VFAT channel registers' values
 * Return the pointer of the VFAT channel registers corresponding to a VFAT channel registers' values
 * \return the pointer
 */
tscType8 *vfatDescription::getAllChanReg () { return (chanreg); }

/** \brief set the values for all channel registers
 * Set the values for all channel registers
 * \param *value - pointer to values to be set
 */
void vfatDescription::setAllChanReg ( tscType8 *value )
{
  for (int i=1;i<129;i++){ chanreg[i] = value[i] ; }
}

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getVCal ( ) { return (vcal) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setVCal  ( tscType8 value ) { vcal = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getVThreshold1 ( ) { return (vthreshold1) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setVThreshold1  ( tscType8 value ) { vthreshold1 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getVThreshold2 ( ) { return (vthreshold2) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setVThreshold2  ( tscType8 value ) { vthreshold2 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getCalPhase ( ) { return (calphase) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setCalPhase  ( tscType8 value ) { calphase = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getControl2 ( ) { return (control2) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setControl2   ( tscType8 value ) { control2 = value ; }

/** \brief Return the value of the VFAT register corresponding to a VFAT register
 * Return the value of the VFAT register corresponding to a VFAT register
 * \return the value
 */
tscType8 vfatDescription::getControl3 ( ) { return (control3) ; }

/** \brief set the value for register
 * Set the value for a VFAT description
 * \param value to be set
 */
void vfatDescription::setControl3   ( tscType8 value ) { control3 = value ; }


/** \brief In order to compare two Vfat descriptions
 * \param vfat - Vfat description to be compared
 * \return bool - true if one value are different, false if not
 */
bool vfatDescription::operator!= ( vfatDescription &vfat ) {

#ifdef DEBUGMSGERROR
  displayDifferences(vfat) ;
#endif

  if (
      (getControl0 ( ) != vfat.getControl0 ( )) ||
      (getControl1 ( ) != vfat.getControl1 ( )) ||

      (getIPreampIn ( ) != vfat.getIPreampIn ( )) ||
      (getIPreampFeed ( ) != vfat.getIPreampFeed ( )) ||
      (getIPreampOut ( ) != vfat.getIPreampOut ( )) ||
      (getIShaper ( ) != vfat.getIShaper ( )) ||
      (getIShaperFeed ( ) != vfat.getIShaperFeed ( )) ||
      (getIComp ( ) != vfat.getIComp ( )) ||

      (getChipid0 ( ) != vfat.getChipid0 ( )) ||
      (getChipid1 ( ) != vfat.getChipid1 ( )) ||
      (getUpset ( ) != vfat.getUpset ( )) ||
      (getHitCount0 ( ) != vfat.getHitCount0 ( )) ||
      (getHitCount1 ( ) != vfat.getHitCount1 ( )) ||
      (getHitCount2 ( ) != vfat.getHitCount2 ( )) ||

      (getLat ( ) != vfat.getLat ( )) ||
      (getVCal ( ) != vfat.getVCal ( )) ||
      (getVThreshold1 ( ) != vfat.getVThreshold1 ( )) ||
      (getVThreshold2 ( ) != vfat.getVThreshold2 ( )) ||
      (getCalPhase( ) != vfat.getCalPhase ( )) ||

      (getControl2 ( ) != vfat.getControl2 ( )) ||
      (getControl3 ( ) != vfat.getControl3 ( )) 
      )
    {
      //Check of channel registers
      for (int i=1;i<129;i++)
	{
	  if(getChanReg(i)!=vfat.getChanReg(i)){return false ;}
	}
      return true;
    }
  else { return false ; }
}

/** \brief In order to compare two Vfat descriptions
 * \param vfat - vfat description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool vfatDescription::operator  == ( vfatDescription &vfat ) {

#ifdef DEBUGMSGERROR
  displayDifferences(vfat) ;
#endif

  if (
      (getControl0 ( ) == vfat.getControl0 ( )) &&
      (getControl1 ( ) == vfat.getControl1 ( )) &&

      (getIPreampIn ( ) == vfat.getIPreampIn ( )) &&
      (getIPreampFeed ( ) == vfat.getIPreampFeed ( )) &&
      (getIPreampOut ( ) == vfat.getIPreampOut ( )) &&
      (getIShaper ( ) == vfat.getIShaper ( )) &&
      (getIShaperFeed ( ) == vfat.getIShaperFeed ( )) &&
      (getIComp ( ) == vfat.getIComp ( )) &&

      (getLat ( ) == vfat.getLat ( )) &&
      (getVCal ( ) == vfat.getVCal ( )) &&
      (getVThreshold1 ( ) == vfat.getVThreshold1 ( )) &&
      (getVThreshold2 ( ) == vfat.getVThreshold2 ( )) &&
      (getCalPhase( ) == vfat.getCalPhase ( )) &&

      (getControl2 ( ) == vfat.getControl2 ( )) &&
      (getControl3 ( ) == vfat.getControl3 ( )) 
      )
    {
      // Check of channel registers
      for (int i=1;i<129;i++)
	{
	  if(getChanReg(i)!=vfat.getChanReg(i)){return false ;}
	}
      return true ;
    }
  else { return false ; }
}


/** Clone a device description
 * \return the device description cloned
 */
vfatDescription *vfatDescription::clone ( ) {

  vfatDescription* myVfatDescription = new vfatDescription(accessKey_, control0, control1, ipreampin, ipreampfeed, ipreampout,
							   ishaper, ishaperfeed, icomp, chipid0, chipid1, upset, hitcount0, hitcount1, hitcount2, 
							   //tscType8 extregpointer_, tscType8 extregdata_,
							   lat, chanreg, vcal, vthreshold1, vthreshold2, calphase, control2, control3);
  myVfatDescription->setFecHardwareId(fecHardwareId_,crateId_) ;
  myVfatDescription->setEnabled(enabled_) ;
  return myVfatDescription;
}

/** \brief Display the vfat values where the comparison is different
 * \param vfat - vfat values uploaded
 */

void vfatDescription::displayDifferences ( vfatDescription &vfat ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "Vfat " << msg << std::endl ;
  if (getControl0 ( ) != vfat.getControl0 ( )) 
    std::cout << "\tCont.Reg<0> is different " << std::dec << (int)getControl0 ( ) << "/" << (int)vfat.getControl0 ( ) << std::endl ;
  if (getControl1 ( ) != vfat.getControl1 ( )) 
    std::cout << "\tCont.Reg<1> is different " << std::dec << (int)getControl1 ( ) << "/" << (int)vfat.getControl1 ( ) << std::endl ;
  if (getIPreampIn ( ) != vfat.getIPreampIn ( )) 
    std::cout << "\tIPreampIn is different " << std::dec << (int)getIPreampIn ( ) << "/" << (int)vfat.getIPreampIn ( ) << std::endl ;
  if (getIPreampFeed ( ) != vfat.getIPreampFeed ( )) 
    std::cout << "\tIPreampFeed is different " << std::dec << (int)getIPreampFeed ( ) << "/" << (int)vfat.getIPreampFeed ( ) << std::endl ;
  if (getIPreampOut ( ) != vfat.getIPreampOut ( )) 
    std::cout << "\tIPreampOut is different " << std::dec << (int)getIPreampOut ( ) << "/" << (int)vfat.getIPreampOut ( ) << std::endl ;
  if (getIShaper ( ) != vfat.getIShaper ( )) 
    std::cout << "\tIShaper is different " << std::dec << (int)getIShaper ( ) << "/" << (int)vfat.getIShaper ( ) << std::endl ;
  if (getIShaperFeed ( ) != vfat.getIShaperFeed ( )) 
    std::cout << "\tIShaperFeed is different " << std::dec << (int)getIShaperFeed ( ) << "/" << (int)vfat.getIShaperFeed ( ) << std::endl ;
  if (getIComp ( ) != vfat.getIComp ( )) 
    std::cout << "\tIComp is different " << std::dec << (int)getIComp ( ) << "/" << (int)vfat.getIComp ( ) << std::endl ;
  if (getControl2 ( ) != vfat.getControl2 ( )) 
    std::cout << "\tCont.Reg<2> is different " << std::dec << (int)getControl2 ( ) << "/" << (int)vfat.getControl2 ( ) << std::endl ;
  if (getControl3 ( ) != vfat.getControl3 ( )) 
    std::cout << "\tCont.Reg<3> is different " << std::dec << (int)getControl3 ( ) << "/" << (int)vfat.getControl3 ( ) << std::endl ;  

  for (int i=1;i<129;i++)
    {
      if(getChanReg(i)!=vfat.getChanReg(i))
	{
          std::cout << "\tChanReg<" << i << "> is different " << std::dec << (int)getChanReg( i ) << "/" << (int)vfat.getChanReg( i )  <<  std::endl ;
          if(i%10==0){ std::cout << std::endl ; }
	}      
    }
}
  
/** \brief Display the vfat values
 */
void vfatDescription::display ( ) {

  std::cout << "Vfat: 0x" 
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

  std::cout << "\tIPreampIn:   " << std::dec << (tscType16)getIPreampIn() << std::endl ;
  std::cout << "\tIPreampFeed: " << std::dec << (tscType16)getIPreampFeed() << std::endl ;
  std::cout << "\tIPreampOut:  " << std::dec << (tscType16)getIPreampOut() << std::endl ;
  std::cout << "\tIShaper:     " << std::dec << (tscType16)getIShaper() << std::endl ;
  std::cout << "\tIShaperFeed: " << std::dec << (tscType16)getIShaperFeed() << std::endl ;
  std::cout << "\tIComp:       "<< std::dec << (tscType16)getIComp() << std::endl ;
  std::cout << std::endl ;
   
  std::cout << "\tCont.Reg<0>: " << std::dec << (tscType16)getControl0() << std::endl ;
  std::cout << "\tCont.Reg<1>: " << std::dec << (tscType16)getControl1() << std::endl ;
  std::cout << "\tCont.Reg<2>: " << std::dec << (tscType16)getControl2() << std::endl ;
  std::cout << "\tCont.Reg<3>: " << std::dec << (tscType16)getControl3() << std::endl ;
  std::cout << std::endl ;

  std::cout << "\tUpsetReg:  " << std::dec << (tscType16)getUpset() << std::endl ;
  std::cout << "\tHitCount0: " << std::dec << (tscType16)getHitCount0() << std::endl ;
  std::cout << "\tHitCount1: " << std::dec << (tscType16)getHitCount1() << std::endl ;
  std::cout << "\tHitCount2: " << std::dec << (tscType16)getHitCount2() << std::endl ;
  std::cout << std::endl ;

  std::cout << "\tLat:         " << std::dec << (tscType16)getLat() << std::endl ;
  std::cout << "\tVCal:        " << std::dec << (tscType16)getVCal() << std::endl ;
  std::cout << "\tVThreshold1: " << std::dec << (tscType16)getVThreshold1() << std::endl ;
  std::cout << "\tVThreshold2: " << std::dec << (tscType16)getVThreshold2() << std::endl ;
  std::cout << "\tCalPhase:    " << std::dec << (tscType16)getCalPhase() << std::endl ;
  std::cout << std::endl ;

  std::cout << "\tChannel Registers: " << std::endl << "\t" ;
  for(int i=1;i<129;i++)
    {
      std::cout << std::dec << (tscType16)getChanReg(i) <<" " ;
      if(i%10==0){ std::cout << std::endl << "\t" ; }
    }
  std::cout << std::endl ;
}

/** \brief Return a list of parameter names
 * must be delete by the owner of the description
 */

parameterDescriptionNameType *vfatDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;

  for (unsigned int i = 0 ; i < (sizeof(VFATPARAMETERNAMES)/sizeof(const char *)) ; i ++)
    {
      (*parameterNames)[VFATPARAMETERNAMES[i]] = new ParameterDescription(VFATPARAMETERNAMES[i], ParameterDescription::INTEGER8) ;
    }

  return parameterNames ;
}
 
/** Parameter's name
 */
const char *vfatDescription::VFATPARAMETERNAMES[] = { "control0", "control1", "ipreampin", "ipreampfeed", "ipreampout", "ishaper", "ishaperfeed", "icomp",
  "chipid0", "chipid1", "upset", "hitcount0", "hitcount1", "hitcount2", "lat",
  "chanreg1", "chanreg2", "chanreg3", "chanreg4", "chanreg5", "chanreg6", "chanreg7", "chanreg8", "chanreg9", "chanreg10",
  "chanreg11", "chanreg12", "chanreg13", "chanreg14", "chanreg15", "chanreg16", "chanreg17", "chanreg18", "chanreg19", "chanreg20",
  "chanreg21", "chanreg22", "chanreg23", "chanreg24", "chanreg25", "chanreg26", "chanreg27", "chanreg28", "chanreg29", "chanreg30",
  "chanreg31", "chanreg32", "chanreg33", "chanreg34", "chanreg35", "chanreg36", "chanreg37", "chanreg38", "chanreg39", "chanreg40",
  "chanreg41", "chanreg42", "chanreg43", "chanreg44", "chanreg45", "chanreg46", "chanreg47", "chanreg48", "chanreg49", "chanreg50",
  "chanreg51", "chanreg52", "chanreg53", "chanreg54", "chanreg55", "chanreg56", "chanreg57", "chanreg58", "chanreg59", "chanreg60",
  "chanreg61", "chanreg62", "chanreg63", "chanreg64", "chanreg65", "chanreg66", "chanreg67", "chanreg68", "chanreg69", "chanreg70",
  "chanreg71", "chanreg72", "chanreg73", "chanreg74", "chanreg75", "chanreg76", "chanreg77", "chanreg78", "chanreg79", "chanreg80",
  "chanreg81", "chanreg82", "chanreg83", "chanreg84", "chanreg85", "chanreg86", "chanreg87", "chanreg88", "chanreg89", "chanreg90",
  "chanreg91", "chanreg92", "chanreg93", "chanreg94", "chanreg95", "chanreg96", "chanreg97", "chanreg98", "chanreg99", "chanreg100",
  "chanreg101", "chanreg102", "chanreg103", "chanreg104", "chanreg105", "chanreg106", "chanreg107", "chanreg108", "chanreg109", 
  "chanreg110", "chanreg111", "chanreg112", "chanreg113", "chanreg114", "chanreg115", "chanreg116", "chanreg117", "chanreg118",
  "chanreg119", "chanreg120", "chanreg121", "chanreg122", "chanreg123", "chanreg124", "chanreg125", "chanreg126", "chanreg127", 
  "chanreg128", "vcal", "vthreshold1", "vthreshold2", "calphase", "control2", "control3" } ;

