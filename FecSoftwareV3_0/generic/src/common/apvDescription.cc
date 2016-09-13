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

#include "apvDescription.h"

/** \brief Constructor in order to set all the parameters
 * Build a description of a APV with all the values specified:
 * \param accessKey - device key
 */
apvDescription::apvDescription (keyType accessKey): deviceDescription ( APV25, accessKey ) { }

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
apvDescription::apvDescription (keyType accessKey,
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
				tscType8 err):
  
  deviceDescription ( APV25, accessKey ) {

  setDescriptionValues (apvMode,
			latency,
			muxGain,
			ipre,
			ipcasc,
			ipsf,
			isha,
			issf,
			ipsp,
			imuxin,
			ical,
			ispare,
			vfp,
			vfs,
			vpsp,
			cdrv,
			csel,
			err);
}

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
apvDescription::apvDescription (tscType16 fecSlot,
				tscType16 ringSlot,
				tscType16 ccuAddress,
				tscType16 i2cChannel,
				tscType16 i2cAddress,
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
				tscType8 err):

  deviceDescription ( APV25, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

  setDescriptionValues (apvMode,
			latency,
			muxGain,
			ipre,
			ipcasc,
			ipsf,
			isha,
			issf,
			ipsp,
			imuxin,
			ical,
			ispare,
			vfp,
			vfs,
			vpsp,
			cdrv,
			csel,
			err);
}
  
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
apvDescription::apvDescription (tscType8 apvMode,
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
				tscType8 err):

  deviceDescription ( APV25 ) {

  setDescriptionValues (apvMode,
			latency,
			muxGain,
			ipre,
			ipcasc,
			ipsf,
			isha,
			issf,
			ipsp,
			imuxin,
			ical,
			ispare,
			vfp,
			vfs,
			vpsp,
			cdrv,
			csel,
			err);
}

/* build a description of an apv with a list of parameters name
 * \param parameterNames - list of parameter
 * \see ParameterDescription class
 */
apvDescription::apvDescription ( parameterDescriptionNameType parameterNames ) :
  deviceDescription ( APV25, parameterNames) {

  setApvMode (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::APVMODE]]->getValueConverted())) ;
  setLatency (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::APVLATENCY]]->getValueConverted())) ;
  setMuxGain (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::MUXGAIN]]->getValueConverted())) ;
  setIpre    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::IPRE]]->getValueConverted())) ;
  setIpcasc  (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::IPCASC]]->getValueConverted())) ;
  setIpsf    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::IPSF]]->getValueConverted())) ;
  setIsha    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::ISHA]]->getValueConverted())) ;
  setIssf    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::ISSF]]->getValueConverted())) ;
  setIpsp    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::IPSP]]->getValueConverted())) ;
  setImuxin  (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::IMUXIN]]->getValueConverted())) ;
  setIcal    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::ICAL]]->getValueConverted())) ;
  setIspare  (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::ISPARE]]->getValueConverted())) ;
  setVfp     (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::VFP]]->getValueConverted())) ;
  setVfs     (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::VFS]]->getValueConverted())) ;
  setVpsp    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::VPSP]]->getValueConverted())) ;
  setCdrv    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::CDRV]]->getValueConverted())) ;
  setCsel    (*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::CSEL]]->getValueConverted())) ;
  setApvError(*((tscType8 *) parameterNames[apvDescription::APVPARAMETERNAMES[apvDescription::APVERROR]]->getValueConverted())) ;
}

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
void apvDescription::setDescriptionValues (tscType8 apvMode,
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
					   tscType8 err) {


  setApvMode (apvMode) ;
  setLatency (latency) ;
  setMuxGain (muxGain) ;
  setIpre    (ipre)    ;
  setIpcasc  (ipcasc)  ;
  setIpsf    (ipsf)    ;
  setIsha    (isha)    ;
  setIssf    (issf)    ;
  setIpsp    (ipsp)    ;
  setImuxin  (imuxin)  ;
  setIcal    (ical)    ;
  setIspare  (ispare)  ;
  setVfp     (vfp)     ;
  setVfs     (vfs)     ;
  setVpsp    (vpsp)    ;
  setCdrv    (cdrv)    ;
  setCsel    (csel)    ;
  setApvError(err)     ;
}

/** \brief Return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getApvMode    ( ) { return (apvMode_); }

/** \brief return the value of the APV register corresponding to an APV registe
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getLatency ( ) { return (latency_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getMuxGain ( ) { return (muxGain_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getIpre    ( ) { return (ipre_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getIpcasc  ( ) { return (ipcasc_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getIpsf    ( ) { return (ipsf_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getIsha    ( ) { return (isha_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getIssf    ( ) { return (issf_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getIpsp    ( ) { return (ipsp_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getImuxin  ( ) { return (imuxin_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getIcal    ( ) { return (ical_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getIspare  ( ) { return (ispare_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getVfp     ( ) { return (vfp_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getVfs     ( ) { return (vfs_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getVpsp    ( ) { return (vpsp_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getCdrv    ( ) { return (cdrv_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getCsel    ( ) { return (csel_) ; }

/** \brief return the value of the APV register corresponding to an APV register
 * Return the value of the APV register corresponding to an APV register
 * \return the value
 */
tscType8 apvDescription::getApvError( ) { return (apvError_) ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param apvMode - value to be set
 */
void apvDescription::setApvMode    ( tscType8 apvMode    ) { apvMode_ = apvMode ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param latency - value to be set
 */
void apvDescription::setLatency ( tscType8 latency ) { latency_ = latency ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param muxGain - value to be set
 */
void apvDescription::setMuxGain ( tscType8 muxGain ) { muxGain_ = muxGain ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param ipre - value to be set
 */
void apvDescription::setIpre    ( tscType8 ipre    ) { ipre_ = ipre ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param ipcasc - value to be set
 */
void apvDescription::setIpcasc  ( tscType8 ipcasc  ) { ipcasc_ = ipcasc ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param ipsf - value to be set
 */
void apvDescription::setIpsf    ( tscType8 ipsf    ) { ipsf_ = ipsf ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param isha - value to be set
 */
void apvDescription::setIsha    ( tscType8 isha    ) { isha_ = isha ; }
/** \brief set the value for register

* Set the value for an APV description
* \param issf - value to be set
*/
void apvDescription::setIssf    ( tscType8 issf    ) { issf_ = issf ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param Status - value to be set
 */
void apvDescription::setIpsp    ( tscType8 ipsp    ) { ipsp_ = ipsp ; }
/** \brief set the value for register
 * Set the value for an APV description
 * \param ipsp - value to be set
 */
void apvDescription::setImuxin  ( tscType8 imuxin  ) { imuxin_ = imuxin ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param ical - value to be set
 */
void apvDescription::setIcal    ( tscType8 ical    ) { ical_ = ical ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param ipsare - value to be set
 */
void apvDescription::setIspare    ( tscType8 ispare) { ispare_ = ispare ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param vfp - value to be set
 */
void apvDescription::setVfp     ( tscType8 vfp     ) { vfp_ = vfp ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param vfs - value to be set
 */
void apvDescription::setVfs     ( tscType8 vfs     ) { vfs_ = vfs ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param vpsp - value to be set
 */
void apvDescription::setVpsp    ( tscType8 vpsp    ) { vpsp_ = vpsp ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param cdrv - value to be set
 */
void apvDescription::setCdrv    ( tscType8 cdrv    ) { cdrv_ = cdrv ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param csel - value to be set
 */
void apvDescription::setCsel    ( tscType8 csel    ) { csel_ = csel ; }

/** \brief set the value for register
 * Set the value for an APV description
 * \param apvError - value to be set
 */
void apvDescription::setApvError( tscType8 apvError) { apvError_ = apvError ; }

/** \brief In order to compare two APV descriptions
 * \param apv - description to be compared
 * \return bool - true if one value are different, false if not
 */
bool apvDescription::operator!= ( apvDescription &apv ) {

#ifdef DEBUGMSGERROR
  displayDifferences ( apv ) ;
#endif

  if (
      (getApvMode ( ) != apv.getApvMode ( )) ||
      (getLatency ( ) != apv.getLatency ( )) ||
      (getMuxGain ( ) != apv.getMuxGain ( )) ||
      (getIpre    ( ) != apv.getIpre    ( )) ||
      (getIpcasc  ( ) != apv.getIpcasc  ( )) ||
      (getIpsf    ( ) != apv.getIpsf    ( )) ||
      (getIsha    ( ) != apv.getIsha    ( )) ||
      (getIssf    ( ) != apv.getIssf    ( )) ||
      (getIpsp    ( ) != apv.getIpsp    ( )) ||
      (getImuxin  ( ) != apv.getImuxin  ( )) ||
      (getIcal    ( ) != apv.getIcal    ( )) ||
      (getIspare  ( ) != apv.getIspare  ( )) ||
      (getVfp     ( ) != apv.getVfp     ( )) ||
      (getVfs     ( ) != apv.getVfs     ( )) ||
      (getVpsp    ( ) != apv.getVpsp    ( )) ||
      (getCdrv    ( ) != apv.getCdrv    ( )) ||
      (getCsel    ( ) != apv.getCsel    ( )) 
      //      ||  (getApvError( ) == apv.getApvError( )) 
      )
    return true ;
  else
    return false ;
}
  
/** \brief In order to compare two APV descriptions
 * \param apv - description to be compared
 * \return bool - true if all values are equals, false if not
 */
bool apvDescription::operator== ( apvDescription &apv ) {

#ifdef DEBUGMSGERROR
  displayDifferences ( apv ) ;
#endif

  if (
      (getApvMode ( ) == apv.getApvMode ( )) &&
      (getLatency ( ) == apv.getLatency ( )) &&
      (getMuxGain ( ) == apv.getMuxGain ( )) &&
      (getIpre    ( ) == apv.getIpre    ( )) &&
      (getIpcasc  ( ) == apv.getIpcasc  ( )) &&
      (getIpsf    ( ) == apv.getIpsf    ( )) &&
      (getIsha    ( ) == apv.getIsha    ( )) &&
      (getIssf    ( ) == apv.getIssf    ( )) &&
      (getIpsp    ( ) == apv.getIpsp    ( )) &&
      (getImuxin  ( ) == apv.getImuxin  ( )) &&
      (getIcal    ( ) == apv.getIcal    ( )) &&
      (getIspare  ( ) == apv.getIspare  ( )) &&
      (getVfp     ( ) == apv.getVfp     ( )) &&
      (getVfs     ( ) == apv.getVfs     ( )) &&
      (getVpsp    ( ) == apv.getVpsp    ( )) &&
      (getCdrv    ( ) == apv.getCdrv    ( )) &&
      (getCsel    ( ) == apv.getCsel    ( )) 
      //      &&  (getApvError( ) == apv.getApvError( )) 
      )
    return true ;
  else
    return false ;
}

/** Clone a device description
 * \return the device description cloned
 * \warning to not forget to delete the result once you have finished with it
 */
apvDescription *apvDescription::clone ( ) {
  apvDescription* myApvDescription =  new apvDescription ( *this ) ;

  return myApvDescription;
}

/** \Brief Display the APV values where the comparison is different
 * \param apv - uploaded values
 */
void apvDescription::displayDifferences ( apvDescription &apv ) {

  char msg[80] ;
  decodeKey (msg, accessKey_) ;
  std::cout << "APV " << msg << std::endl ;
  if (getApvMode ( ) != apv.getApvMode ( ))
    std::cout << "\tApvMode is different (" << std::dec <<(unsigned int) getApvMode ( ) << " / " << (unsigned int) apv.getApvMode ( ) << ")" << std::endl ;
  if ( getLatency( ) != apv.getLatency ( ))
    std::cout << "\tLatency is different (" <<(unsigned int) getLatency( ) << " / " << (unsigned int) apv.getLatency ( ) << ")" << std::endl ;
  if ( getMuxGain( ) != apv.getMuxGain ( ))
    std::cout << "\tMuxGain is different " <<(unsigned int) getMuxGain( ) << " / " << (unsigned int) apv.getMuxGain ( ) << ")" << std::endl ;
  if ( getIpre( ) != apv.getIpre ( ))
    std::cout << "\tIpre is different " <<(unsigned int) getIpre( ) << " / " << (unsigned int) apv.getIpre ( ) << ")" << std::endl ;
  if ( getIpcasc( ) != apv.getIpcasc ( ))
    std::cout << "\tIpcasc is different " <<(unsigned int) getIpcasc( ) << " / " << (unsigned int) apv.getIpcasc ( ) << ")" << std::endl ;
  if ( getIpsf( ) != apv.getIpsf ( ))
    std::cout << "\tIpsf is different " <<(unsigned int) getIpsf( ) << " / " << (unsigned int) apv.getIpsf ( ) << ")" << std::endl ;
  if ( getIsha( ) != apv.getIsha ( ))
    std::cout << "\tIsha is different " <<(unsigned int) getIsha( ) << " / " << (unsigned int) apv.getIsha ( ) << ")" << std::endl ;
  if ( getIssf( ) != apv.getIssf ( ))
    std::cout << "\tIssf is different " <<(unsigned int) getIssf( ) << " / " << (unsigned int) apv.getIssf ( ) << ")" << std::endl ;
  if ( getIpsp( ) != apv.getIpsp ( ))
    std::cout << "\tIpsp is different " <<(unsigned int) getIpsp( ) << " / " << (unsigned int) apv. getIpsp( ) << ")" << std::endl ;
  if ( getImuxin( ) != apv.getImuxin( ))
    std::cout << "\tImuxin is different " << (unsigned int) getImuxin( ) << " / " << (unsigned int) apv. getImuxin( ) << ")" << std::endl ;
  if ( getIcal( ) != apv.getIcal ( ))
    std::cout << "\tIcal is different " << (unsigned int) getIcal( ) << " / " << (unsigned int) apv.getIcal ( ) << ")" << std::endl ;
  if ( getIspare( ) != apv.getIspare ( ))
    std::cout << "\tIspare is different " << (unsigned int) getIspare( ) << " / " << (unsigned int) apv.getIspare ( ) << ")" << std::endl ;
  if ( getVfp( ) != apv.getVfp ( ))
    std::cout << "\tVfp is different " << (unsigned int) getVfp( ) << " / " << (unsigned int) apv.getVfp ( ) << ")" << std::endl ;
  if ( getVfs( ) != apv.getVfs ( ))
    std::cout << "\tVfs is different " << (unsigned int) getVfs( ) << " / " << (unsigned int) apv. getVfs( ) << ")" << std::endl ;
  if ( getVpsp( ) != apv.getVpsp ( ))
    std::cout << "\tVpsp is different " << (unsigned int) getVpsp( ) << " / " << (unsigned int) apv.getVpsp ( ) << ")" << std::endl ;
  if ( getCdrv( ) != apv.getCdrv ( ))
    std::cout << "\tCdrv is different " << (unsigned int) getCdrv( ) << " / " << (unsigned int) apv.getCdrv ( ) << ")" << std::endl ;
  if ( getCsel( ) != apv.getCsel ( ))
    std::cout << "\tCsel is different " << (unsigned int) getCsel( ) << " / " << (unsigned int) apv.getCsel ( ) << ")" << std::endl ;
  //if ( getApvError( ) != apv.getApvError ( ))
  //std::cout << "\tApvError is different " << getApvError( ) << " / " << apv.getApvError ( ) << ")" << std::endl ;
}

/** \brief Display the APV values
 */
void apvDescription::display ( ) {
  std::cout << "APV: 0x" 
	    << std::hex << getFecSlot() 
	    << ":0x" << std::hex << getRingSlot()
	    << ":0x" << std::hex << getCcuAddress()
	    << ":0x" << std::hex << getChannel()
	    << ":0x" << std::hex << getAddress() << std::endl ;

  std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;
  std::cout << "\tmuxgain: " << std::dec << (tscType16)getMuxGain() << std::endl;
  std::cout << "\tlatency: " << std::dec << (tscType16)getLatency() << std::endl;
  std::cout << "\tmode: " << std::dec << (tscType16)getApvMode() << std::endl;
  std::cout << "\tisha: " << std::dec << (tscType16)getIsha() << std::endl;
  std::cout << "\tipsp: " << std::dec << (tscType16)getIpsp() << std::endl;
  std::cout << "\tipcasc: " << std::dec << (tscType16)getIpcasc() << std::endl;
  std::cout << "\tvpsp: " << std::dec << (tscType16)getVpsp() << std::endl;
  std::cout << "\tipsf: " << std::dec << (tscType16)getIpsf() << std::endl;
  std::cout << "\tipre: " << std::dec << (tscType16)getIpre() << std::endl;
  std::cout << "\timuxin: " << std::dec << (tscType16)getImuxin() << std::endl;
  std::cout << "\tvfp: " << std::dec << (tscType16)getVfp() << std::endl;
  std::cout << "\tissf: " << std::dec << (tscType16)getIssf() << std::endl;
  std::cout << "\tvfs: " << std::dec << (tscType16)getVfs() << std::endl;
  std::cout << "\tical: " << std::dec << (tscType16)getIcal() << std::endl;
  //std::cout << "\tispare: " << std::dec << (tscType16)getIspare() << std::endl;
  std::cout << "\tcdrv: " << std::dec << (tscType16)getCdrv() << std::endl;
  std::cout << "\tcsel: " << std::dec << (tscType16)getCsel() << std::endl;
}

/** \brief Return a list of parameter name
 * must be delete by the owner of the description
 */
parameterDescriptionNameType *apvDescription::getParameterNames ( ) {

  parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;

  for (unsigned int i = 0 ; i < (sizeof(apvDescription::APVPARAMETERNAMES)/sizeof(const char *)) ; i ++) {
    (*parameterNames)[apvDescription::APVPARAMETERNAMES[i]] = new ParameterDescription (apvDescription::APVPARAMETERNAMES[i], ParameterDescription::INTEGER8) ;
  }

  return parameterNames ;
}

/** Parameter's names
 */
const char *apvDescription::APVPARAMETERNAMES[] = {"apvMode",
						   "latency",
						   "muxGain",
						   "ipre",
						   "ipcasc",
						   "ipsf",
						   "isha",
						   "issf",
						   "ipsp",
						   "imuxin",
						   "ical",
						   "ispare",
						   "vfp",
						   "vfs",
						   "vpsp",
						   "cdrv",
						   "csel",
						   "apvError"} ;
