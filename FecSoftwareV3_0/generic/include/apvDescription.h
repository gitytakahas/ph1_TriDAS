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
//#define DEBUGMSGERROR

#ifndef APVDESCRIPTION_H
#define APVDESCRIPTION_H

#include "tscTypes.h"
#include "deviceDescription.h"
#include "apvDefinition.h"

/**
 * \class apvDescription
 * This class give a description of all the APV registers:
 * <ul>
 * <li>Mode;
 * <li>Latency;
 * <li>MuxGain;
 * <li>Ipre;
 * <li>Ipcasc;
 * <li>Ipsf;
 * <li>Isha;
 * <li>Issf;
 * <li>Ipsp;
 * <li>Imuxin;
 * <li>Ical;
 * <li>Ispare;
 * <li>Vfp;
 * <li>Vfs;
 * <li>Vpsp;
 * <li>Cdrv;
 * <li>Csel;
 * <li>Err;   
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can be set also
 * \see apvDefinition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "apvDefinition.h"
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class give a description of all the APV registers
 */
class apvDescription: public deviceDescription {

 private:
  tscType8 apvMode_ ;
  tscType8 latency_ ;
  tscType8 muxGain_ ;
  tscType8 ipre_ ;
  tscType8 ipcasc_ ;
  tscType8 ipsf_ ;
  tscType8 isha_ ;
  tscType8 issf_ ;
  tscType8 ipsp_ ;
  tscType8 imuxin_ ;
  tscType8 ical_ ;
  tscType8 ispare_ ;
  tscType8 vfp_ ;
  tscType8 vfs_ ;
  tscType8 vpsp_ ;
  tscType8 cdrv_ ;
  tscType8 csel_ ;
  tscType8 apvError_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum ApvEnumType {APVMODE, APVLATENCY, MUXGAIN, IPRE, IPCASC,
		    IPSF, ISHA, ISSF, IPSP, IMUXIN, ICAL, ISPARE,
		    VFP, VFS, VPSP, CDRV, CSEL, APVERROR} ;

  /** Parameter's names
   */
  static const char *APVPARAMETERNAMES[APVERROR+1] ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a APV with all the values specified:
   * \param accessKey - device key
   */
  apvDescription (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of an APV with all the values specified:
   * \param accessKey - key access of a device
   * \param apvMode - Apv Register
   * \param latency - Apv Register
   * \param muxGain - Apv Register
   * \param ipre - Apv Register
   * \param ipcasc - Apv Register
   * \param ipsf - Apv Register
   * \param isha - Apv Register
   * \param issf - Apv Register
   * \param ipsp - Apv Register
   * \param imuxin - Apv Register
   * \param ical - Apv Register
   * \param ispare - Apv Register
   * \param vfp - Apv Register
   * \param vfs - Apv Register
   * \param vpsp - Apv Register
   * \param cdrv - Apv Register
   * \param csel - Apv Register
   * \param err - Apv Register
   */
  apvDescription (keyType accessKey,
                  tscType8 apvMode,
                  tscType8 latency,
                  tscType8 muxGain,
                  tscType8 ipre,
                  tscType8 ipcasc,
                  tscType8 ipsf,
                  tscType8 isha,
                  tscType8 issf,
                  tscType8 ipsp,
                  tscType8 imuxin,
                  tscType8 ical,
                  tscType8 ispare,
                  tscType8 vfp,
                  tscType8 vfs,
                  tscType8 vpsp,
                  tscType8 cdrv,
                  tscType8 csel,
                  tscType8 err) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of an APV with all the values specified:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   * \param apvMode - Apv Register
   * \param latency - Apv Register
   * \param muxGain - Apv Register
   * \param ipre - Apv Register
   * \param ipcasc - Apv Register
   * \param ipsf - Apv Register
   * \param isha - Apv Register
   * \param issf - Apv Register
   * \param ipsp - Apv Register
   * \param imuxin - Apv Register
   * \param ical - Apv Register
   * \param ispare - Apv Register
   * \param vfp - Apv Register
   * \param vfs - Apv Register
   * \param vpsp - Apv Register
   * \param cdrv - Apv Register
   * \param csel - Apv Register
   * \param err - Apv Register
   */
  apvDescription (tscType16 fecSlot,
                  tscType16 ringSlot,
                  tscType16 ccuAddress,
                  tscType16 i2cChannel,
                  tscType16 i2cAddress,
                  tscType8 apvMode = 0,
                  tscType8 latency = 0,
                  tscType8 muxGain = 0,
                  tscType8 ipre = 0,
                  tscType8 ipcasc = 0,
                  tscType8 ipsf = 0,
                  tscType8 isha = 0,
                  tscType8 issf = 0,
                  tscType8 ipsp = 0,
                  tscType8 imuxin = 0,
                  tscType8 ical = 0,
                  tscType8 ispare = 0,
                  tscType8 vfp = 0,
                  tscType8 vfs = 0,
                  tscType8 vpsp = 0,
                  tscType8 cdrv = 0,
                  tscType8 csel = 0,
                  tscType8 err = 0) ;
  
  /** \brief Constructor in order to set all the parameters
   * Build a description of an APV with all the values specified:
   * \param apvMode - Apv Register
   * \param latency - Apv Register
   * \param muxGain - Apv Register
   * \param ipre - Apv Register
   * \param ipcasc - Apv Register
   * \param ipsf - Apv Register
   * \param isha - Apv Register
   * \param issf - Apv Register
   * \param ipsp - Apv Register
   * \param imuxin - Apv Register
   * \param ical - Apv Register
   * \param ispare - Apv register
   * \param vfp - Apv Register
   * \param vfs - Apv Register
   * \param vpsp - Apv Register
   * \param cdrv - Apv Register
   * \param csel - Apv Register
   * \param err - Apv Register
   */
  apvDescription (tscType8 apvMode,
		  tscType8 latency,
		  tscType8 muxGain,
		  tscType8 ipre,
		  tscType8 ipcasc,
		  tscType8 ipsf,
		  tscType8 isha,
		  tscType8 issf,
		  tscType8 ipsp,
		  tscType8 imuxin,
		  tscType8 ical,
		  tscType8 ispare,
		  tscType8 vfp,
		  tscType8 vfs,
		  tscType8 vpsp,
		  tscType8 cdrv,
		  tscType8 csel,
		  tscType8 err) ;

  /* build a description of an apv with a list of parameters name
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   */
  apvDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of an APV with all the values specified:
   * \param apvMode - Apv Register
   * \param latency - Apv Register
   * \param muxGain - Apv Register
   * \param ipre - Apv Register
   * \param ipcasc - Apv Register
   * \param ipsf - Apv Register
   * \param isha - Apv Register
   * \param issf - Apv Register
   * \param ipsp - Apv Register
   * \param imuxin - Apv Register
   * \param ical - Apv Register
   * \param ispare - Apv Register
   * \param vfp - Apv Register
   * \param vfs - Apv Register
   * \param vpsp - Apv Register
   * \param cdrv - Apv Register
   * \param csel - Apv Register
   * \param err - Apv Register
   */
  void setDescriptionValues (tscType8 apvMode,
                             tscType8 latency,
                             tscType8 muxGain,
                             tscType8 ipre,
                             tscType8 ipcasc,
                             tscType8 ipsf,
                             tscType8 isha,
                             tscType8 issf,
                             tscType8 ipsp,
                             tscType8 imuxin,
                             tscType8 ical,
                             tscType8 ispare,
                             tscType8 vfp,
                             tscType8 vfs,
                             tscType8 vpsp,
                             tscType8 cdrv,
                             tscType8 csel,
                             tscType8 err) ;

  /** \brief Return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getApvMode    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV registe
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getLatency ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getMuxGain ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getIpre    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getIpcasc  ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getIpsf    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getIsha    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getIssf    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getIpsp    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getImuxin  ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getIcal    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getIspare  ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getVfp     ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getVfs     ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getVpsp    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getCdrv    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getCsel    ( ) ;

  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
  tscType8 getApvError( ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param apvMode - value to be set
   */
  void setApvMode    ( tscType8 apvMode ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param latency - value to be set
   */
  void setLatency ( tscType8 latency ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param muxGain - value to be set
   */
  void setMuxGain ( tscType8 muxGain ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param ipre - value to be set
   */
  void setIpre    ( tscType8 ipre ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param ipcasc - value to be set
   */
  void setIpcasc  ( tscType8 ipcasc  ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param ipsf - value to be set
   */
  void setIpsf    ( tscType8 ipsf    ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param isha - value to be set
   */
  void setIsha    ( tscType8 isha    ) ;
  /** \brief set the value for register

  * Set the value for an APV description
  * \param issf - value to be set
  */
  void setIssf    ( tscType8 issf    ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
  void setIpsp    ( tscType8 ipsp    ) ;
  /** \brief set the value for register
   * Set the value for an APV description
   * \param ipsp - value to be set
   */
  void setImuxin  ( tscType8 imuxin  ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param ical - value to be set
   */
  void setIcal    ( tscType8 ical    ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param ipsare - value to be set
   */
  void setIspare    ( tscType8 ispare) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param vfp - value to be set
   */
  void setVfp     ( tscType8 vfp     ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param vfs - value to be set
   */
  void setVfs     ( tscType8 vfs     ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param vpsp - value to be set
   */
  void setVpsp    ( tscType8 vpsp    ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param cdrv - value to be set
   */
  void setCdrv    ( tscType8 cdrv    ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param csel - value to be set
   */
  void setCsel    ( tscType8 csel    ) ;

  /** \brief set the value for register
   * Set the value for an APV description
   * \param apvError - value to be set
   */
  void setApvError( tscType8 apvError) ;

  /** \brief In order to compare two APV descriptions
   * \param apv - description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( apvDescription &apv ) ;

  /** \brief In order to compare two APV descriptions
   * \param apv - description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( apvDescription &apv ) ;

  /** Clone a device description
   * \return the device description cloned
   * \warning to not forget to delete the result once you have finished with it
   */
  apvDescription *clone ( ) ;

  /** \brief Display the APV values where the comparison is different
   * \param apv - uploaded values
   */
  void displayDifferences ( apvDescription &apv ) ;

  /** \brief Display the APV values
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

};

#endif
