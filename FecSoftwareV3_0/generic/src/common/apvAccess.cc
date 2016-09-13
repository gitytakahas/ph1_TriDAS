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
#include <iostream>

#include "tscTypes.h"

#include "apvDefinition.h"
#include "apvAccess.h"

/** Constructor to store the access to the APV (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
apvAccess::apvAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress ) : 

  deviceAccess (APV25,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress ) {

}

/** Constructor to store the access to the APV (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
apvAccess::apvAccess (FecAccess *fec, 
                      keyType key) :

  deviceAccess (APV25,
                fec,
                key) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
apvAccess::~apvAccess ( ) {

  // ~deviceAccess ( ) ;
}

/** 
 * apvValues - descriptions
 */
void apvAccess::setDescription ( apvDescription apvValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = apvValues.clone() ;
}

/** Take a description value of an APV and download the values in a specific APV
 * \param apvValues - all the values for an APV
 * \exception FecExceptionHandler
 */
void apvAccess::setValues ( apvDescription apvValues ) {

  // setApvError( apvValues.getErr     () ) ; // Read only

  setApvMode ( apvValues.getApvMode () ) ;
  setLatency ( apvValues.getLatency () ) ;
  setMuxGain ( apvValues.getMuxGain () ) ;
  setIpre    ( apvValues.getIpre    () ) ;
  setIpcasc  ( apvValues.getIpcasc  () ) ;
  setIpsf    ( apvValues.getIpsf    () ) ;
  setIsha    ( apvValues.getIsha    () ) ;
  setIssf    ( apvValues.getIssf    () ) ;
  setIpsp    ( apvValues.getIpsp    () ) ;
  setImuxin  ( apvValues.getImuxin  () ) ;
  setIcal    ( apvValues.getIcal    () ) ;
  setIspare  ( apvValues.getIspare  () ) ;
  setVfp     ( apvValues.getVfp     () ) ;
  setVfs     ( apvValues.getVfs     () ) ;
  setVpsp    ( apvValues.getVpsp    () ) ;
  setCdrv    ( apvValues.getCdrv    () ) ;
  setCsel    ( apvValues.getCsel    () ) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = apvValues.clone() ;
}

/** Build a description of an APV and return it
 * \return the APV description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
apvDescription *apvAccess::getValues ( ) {

  apvDescription *apvValues = 
    new apvDescription (accessKey_,
                        getApvMode ( ),
                        getLatency ( ),
                        getMuxGain ( ),
                        getIpre    ( ),
                        getIpcasc  ( ),
                        getIpsf    ( ),
                        getIsha    ( ),
                        getIssf    ( ),
                        getIpsp    ( ),
                        getImuxin  ( ),
                        getIcal    ( ),
                        getIspare  ( ),
                        getVfp     ( ),
                        getVfs     ( ),
                        getVpsp    ( ),
                        getCdrv    ( ),
                        getCsel    ( ),
                        getApvError( )) ;

  apvValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

  return (apvValues) ;
}

/** Set the APV Error
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setApvError( tscType8 ApvError ) {

  accessToFec_->write (accessKey_, APV25_ERR_REG, ApvError) ;
}

/** Set the APV Mode
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setApvMode ( tscType8 ApvMode ) {

  accessToFec_->write (accessKey_, APV25_MOD_REG, ApvMode) ;
}

/** Set the APV Latency
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setLatency ( tscType8 Latency ) {

  accessToFec_->write (accessKey_, APV25_LAT_REG, Latency) ;
}

/** Set the APV Mux Gain
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setMuxGain ( tscType8 MuxGain  ) {

  accessToFec_->write (accessKey_, APV25_MUXGAIN, MuxGain) ;
}

/** Set the APV Ipre
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setIpre ( tscType8 Ipre ) {

  accessToFec_->write (accessKey_, APV25_IPRE, Ipre) ;
}

/** Set the APV Ipcasc
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setIpcasc ( tscType8 Ipcasc ) {

  accessToFec_->write (accessKey_, APV25_IPCASC, Ipcasc) ;
}

/** Set the APV Ipsf
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setIpsf ( tscType8 Ipsf ) {

  accessToFec_->write (accessKey_, APV25_IPSF, Ipsf) ;
}

/** Set the APV Isha
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setIsha ( tscType8 Isha ) {

  accessToFec_->write (accessKey_, APV25_ISHA, Isha) ;
}

/** Set the APV Issf
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setIssf ( tscType8 Issf ) {

  accessToFec_->write (accessKey_, APV25_ISSF, Issf) ;
}

/** Set the APV Ipsp
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setIpsp ( tscType8 Ipsp ) {

  accessToFec_->write (accessKey_, APV25_IPSP, Ipsp) ;
}

/** Set the APV Imuxin
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setImuxin ( tscType8 Imuxin ) {

  accessToFec_->write (accessKey_, APV25_IMUXIN, Imuxin) ;
}

/** Set the APV Ical
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setIcal ( tscType8 Ical ) {

  accessToFec_->write (accessKey_, APV25_ICAL, Ical) ;
}

/** Set the APV Ispare
 * \param Value to be set
 * \warning not used this register is not used in the APV
 */
void apvAccess::setIspare ( tscType8 Ispare ) {

}

/** Set the APV Vfp
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setVfp ( tscType8 Vfp ) {

  accessToFec_->write (accessKey_, APV25_VFP, Vfp) ;
}

/** Set the APV Vfs
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setVfs ( tscType8 Vfs ) {

  accessToFec_->write (accessKey_, APV25_VFS, Vfs) ;
}

/** Set the APV Vpsp
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setVpsp ( tscType8 Vpsp ) {

  accessToFec_->write (accessKey_, APV25_VPSP, Vpsp) ;
}
/** Set the APV Cdrv
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setCdrv ( tscType8 Cdrv ) {

  accessToFec_->write (accessKey_, APV25_CDRV, Cdrv) ;
}

/** Set the APV Csel
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void apvAccess::setCsel ( tscType8 Csel ) {

  accessToFec_->write (accessKey_, APV25_CSEL, Csel) ;
}


/** Get APV Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getApvError ( )  {

  return (accessToFec_->read (accessKey_, APV25_ERR_REG + APV25_READ)) ;
}

/** Get APV Mode
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getApvMode ( ) {

  return (accessToFec_->read (accessKey_, APV25_MOD_REG + APV25_READ)) ;
}

/** Get APV Latency
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getLatency ( )  {

  return (accessToFec_->read (accessKey_, APV25_LAT_REG + APV25_READ)) ;
}

/** Get APV Mux Gain
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getMuxGain ( )  {

  return (accessToFec_->read (accessKey_, APV25_MUXGAIN + APV25_READ)) ;
}

/** Get APV Ipre
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getIpre ( ) {

  return (accessToFec_->read (accessKey_, APV25_IPRE + APV25_READ)) ;
}

/** Get APV Ipcasc
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getIpcasc ( )  {

  return (accessToFec_->read (accessKey_, APV25_IPCASC + APV25_READ)) ;
}

/** Get APV Ipsf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getIpsf ( ) {

  return (accessToFec_->read (accessKey_, APV25_IPSF + APV25_READ)) ;
}

/** Get APV Isha
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getIsha ( )  {

  return (accessToFec_->read (accessKey_, APV25_ISHA + APV25_READ)) ;
}

/** Get APV Issf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getIssf ( ) {

  return (accessToFec_->read (accessKey_, APV25_ISSF + APV25_READ)) ;
}

/** Get APV Ipsp
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getIpsp ( ) {

  return (accessToFec_->read (accessKey_, APV25_IPSP + APV25_READ)) ;
}

/** Get APV Imuxin
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getImuxin ( ) {

  return (accessToFec_->read (accessKey_, APV25_IMUXIN + APV25_READ)) ;
}

/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getIcal ( ) {

  return (accessToFec_->read (accessKey_, APV25_ICAL + APV25_READ)) ;
}

/** Get APV Ispare
 * \return 0
 * \warning not used, this register is not used in the APV
 */
tscType8 apvAccess::getIspare ( ) {

  return (0) ;
}


/** Get APV Vfp
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getVfp ( ) {

  return (accessToFec_->read (accessKey_, APV25_VFP + APV25_READ)) ;
}

/** Get APV Vfs
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getVfs ( ) {

  return (accessToFec_->read (accessKey_, APV25_VFS + APV25_READ)) ;
}

/** Get APV Vpsp
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getVpsp ( ) {

  return (accessToFec_->read (accessKey_, APV25_VPSP + APV25_READ)) ;
}

/** Get APV Cdrv
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getCdrv ( ) {

  return (accessToFec_->read (accessKey_, APV25_CDRV + APV25_READ)) ;
}

/** Get APV Csel
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 apvAccess::getCsel ( ) {

  return (accessToFec_->read (accessKey_, APV25_CSEL + APV25_READ)) ;
}

/** Take a description of an APV and build all the frames (through a dedicated structure) to be sent
 * \param apvValues - all the values for an APV
 * \param vAccess - block of frames
 */
void apvAccess::getBlockWriteValues ( apvDescription apvValues, std::list<accessDeviceType> &vAccess ) {

  // Buffer of multiple frame block transfer

  // setApvError( apvValues.getErr     () ) ; // Read only
  //accessDeviceType apvMode = { getKey(), RALMODE, MODE_WRITE, APV25_ERR_REG, apvValues.getErr (), false, 0, 0, 0, NULL} ;
  //vAccess.push_back (apvMode) ;
  //setApvMode ( apvValues.getApvMode () ) ;
  accessDeviceType apvMode = { getKey(), RALMODE, MODE_WRITE, APV25_MOD_REG, apvValues.getApvMode (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (apvMode) ;

  // setLatency ( apvValues.getLatency () ) ;
  accessDeviceType apvLatency = { getKey(), RALMODE, MODE_WRITE, APV25_LAT_REG, apvValues.getLatency (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (apvLatency) ;
  // setMuxGain ( apvValues.getMuxGain () ) ;
  accessDeviceType muxGain = { getKey(), RALMODE, MODE_WRITE, APV25_MUXGAIN, apvValues.getMuxGain (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (muxGain) ;
  //setIpre    ( apvValues.getIpre    () ) ;
  accessDeviceType ipre = { getKey(), RALMODE, MODE_WRITE, APV25_IPRE, apvValues.getIpre (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ipre) ;
  //setIpcasc  ( apvValues.getIpcasc  () ) ;
  accessDeviceType ipcasc  = { getKey(), RALMODE, MODE_WRITE, APV25_IPCASC, apvValues.getIpcasc (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ipcasc) ;
  //setIpsf    ( apvValues.getIpsf    () ) ;
  accessDeviceType ipsf = { getKey(), RALMODE, MODE_WRITE, APV25_IPSF, apvValues.getIpsf (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ipsf) ;
  //setIsha    ( apvValues.getIsha    () ) ;
  accessDeviceType isha = { getKey(), RALMODE, MODE_WRITE, APV25_ISHA, apvValues.getIsha (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (isha) ;
  //setIssf    ( apvValues.getIssf    () ) ;
  accessDeviceType issf = { getKey(), RALMODE, MODE_WRITE, APV25_ISSF, apvValues.getIssf (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (issf) ;
  //setIpsp    ( apvValues.getIpsp    () ) ;
  accessDeviceType ipsp = { getKey(), RALMODE, MODE_WRITE, APV25_IPSP, apvValues.getIpsp (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ipsp) ;
  //setImuxin  ( apvValues.getImuxin  () ) ;
  accessDeviceType imuxin = { getKey(), RALMODE, MODE_WRITE, APV25_IMUXIN, apvValues.getImuxin (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (imuxin) ;
  //setIcal    ( apvValues.getIcal    () ) ;
  accessDeviceType ical = { getKey(), RALMODE, MODE_WRITE, APV25_ICAL, apvValues.getIcal (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (ical) ;
  //setIspare  ( apvValues.getIspare  () ) ;
  //accessDeviceType ispare = { getKey(), RALMODE, MODE_WRITE, APV25_ISPARE, apvValues.getIspare (), false, 0, 0, 0, NULL} ;
  //vAccess.push_back (ispare) ;
  //setVfp     ( apvValues.getVfp     () ) ;
  accessDeviceType vfp = { getKey(), RALMODE, MODE_WRITE, APV25_VFP, apvValues.getVfp (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vfp) ;
  //setVfs     ( apvValues.getVfs     () ) ;
  accessDeviceType vfs = { getKey(), RALMODE, MODE_WRITE, APV25_VFS, apvValues.getVfs (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vfs) ;
  //setVpsp    ( apvValues.getVpsp    () ) ;
  accessDeviceType vpsp = { getKey(), RALMODE, MODE_WRITE, APV25_VPSP, apvValues.getVpsp (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (vpsp) ;
  //setCdrv    ( apvValues.getCdrv    () ) ;
  accessDeviceType cdrv = { getKey(), RALMODE, MODE_WRITE, APV25_CDRV, apvValues.getCdrv (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (cdrv) ;
  //setCsel    ( apvValues.getCsel    () ) ;
  accessDeviceType csel = { getKey(), RALMODE, MODE_WRITE, APV25_CSEL, apvValues.getCsel (), false, 0, 0, 0, NULL} ;
  vAccess.push_back (csel) ;

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = apvValues.clone() ;
}

/** Take a description of an APV and build all the frames (through a dedicated structure) to be sent but for only certain registers
 * \param apvValues - all the values for an APV
 * \param vAccess - block of frames
 * \param apvMode - register of the APV (true = set it, false do not set it)
 * \param latency - register of the APV (true = set it, false do not set it)
 * \param muxGain - register of the APV (true = set it, false do not set it)
 * \param ipre - register of the APV (true = set it, false do not set it)
 * \param ipcasc - register of the APV (true = set it, false do not set it)
 * \param ipsf - register of the APV (true = set it, false do not set it)
 * \param isha - register of the APV (true = set it, false do not set it)
 * \param issf - register of the APV (true = set it, false do not set it)
 * \param ipsp - register of the APV (true = set it, false do not set it)
 * \param imuxin - register of the APV (true = set it, false do not set it)
 * \param ical - register of the APV (true = set it, false do not set it)
 * \param ispare - register of the APV (true = set it, false do not set it) => not set
 * \param vfp - register of the APV (true = set it, false do not set it)
 * \param vfs - register of the APV (true = set it, false do not set it)
 * \param vpsp - register of the APV (true = set it, false do not set it)
 * \param cdrv - register of the APV (true = set it, false do not set it)
 * \param csel - register of the APV (true = set it, false do not set it)
 * \param apvError - register of the APV (true = set it, false do not set it) => not set
 */
void apvAccess::getBlockWriteValues ( apvDescription apvValues, std::list<accessDeviceType> &vAccess, 
				      bool apvModeF, bool latencyF, bool muxGainF,
				      bool ipreF, bool ipcascF, bool ipsfF,
				      bool ishaF, bool issfF, bool ipspF,
				      bool imuxinF, bool icalF, bool ispareF,
				      bool vfpF, bool vfsF, bool vpspF,
				      bool cdrvF, bool cselF, bool apvErrorF ) {
  

  // Buffer of multiple frame block transfer

  //if (apvErrorF) {
  //setApvError( apvValues.getErr     () ) ; // Read only
  //accessDeviceType apvMode = { getKey(), RALMODE, MODE_WRITE, APV25_ERR_REG, apvValues.getErr (), false, 0, 0, 0, NULL} ;
  //vAccess.push_back (apvMode) ;
  //}
  if (apvModeF) {
    //setApvMode ( apvValues.getApvMode () ) ;
    accessDeviceType apvMode = { getKey(), RALMODE, MODE_WRITE, APV25_MOD_REG, apvValues.getApvMode (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (apvMode) ;
  }
  if (latencyF) {
    // setLatency ( apvValues.getLatency () ) ;
    accessDeviceType apvLatency = { getKey(), RALMODE, MODE_WRITE, APV25_LAT_REG, apvValues.getLatency (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (apvLatency) ;
  }
  if (muxGainF) {
    // setMuxGain ( apvValues.getMuxGain () ) ;
    accessDeviceType muxGain = { getKey(), RALMODE, MODE_WRITE, APV25_MUXGAIN, apvValues.getMuxGain (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (muxGain) ;
  }
  if (ipreF) {
    //setIpre    ( apvValues.getIpre    () ) ;
    accessDeviceType ipre = { getKey(), RALMODE, MODE_WRITE, APV25_IPRE, apvValues.getIpre (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (ipre) ;
  }
  if (ipcascF) {
    //setIpcasc  ( apvValues.getIpcasc  () ) ;
    accessDeviceType ipcasc  = { getKey(), RALMODE, MODE_WRITE, APV25_IPCASC, apvValues.getIpcasc (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (ipcasc) ;
  }
  if (ipsfF) {
    //setIpsf    ( apvValues.getIpsf    () ) ;
    accessDeviceType ipsf = { getKey(), RALMODE, MODE_WRITE, APV25_IPSF, apvValues.getIpsf (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (ipsf) ;
  }
  if (ishaF) {
    //setIsha    ( apvValues.getIsha    () ) ;
    accessDeviceType isha = { getKey(), RALMODE, MODE_WRITE, APV25_ISHA, apvValues.getIsha (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (isha) ;
  }
  if (issfF) {
    //setIssf    ( apvValues.getIssf    () ) ;
    accessDeviceType issf = { getKey(), RALMODE, MODE_WRITE, APV25_ISSF, apvValues.getIssf (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (issf) ;
  }
  if (ipspF) {
    //setIpsp    ( apvValues.getIpsp    () ) ;
    accessDeviceType ipsp = { getKey(), RALMODE, MODE_WRITE, APV25_IPSP, apvValues.getIpsp (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (ipsp) ;
  }
  if (imuxinF) {
    //setImuxin  ( apvValues.getImuxin  () ) ;
    accessDeviceType imuxin = { getKey(), RALMODE, MODE_WRITE, APV25_IMUXIN, apvValues.getImuxin (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (imuxin) ;
  }
  if (icalF) {
    //setIcal    ( apvValues.getIcal    () ) ;
    accessDeviceType ical = { getKey(), RALMODE, MODE_WRITE, APV25_ICAL, apvValues.getIcal (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (ical) ;
  }
  //if (ispareF) {
  //setIspare  ( apvValues.getIspare  () ) ;
  //accessDeviceType ispare = { getKey(), RALMODE, MODE_WRITE, APV25_ISPARE, apvValues.getIspare (), false, 0, 0, 0, NULL} ;
  //vAccess.push_back (ispare) ;
  //}
  if (vfpF) {
    //setVfp     ( apvValues.getVfp     () ) ;
    accessDeviceType vfp = { getKey(), RALMODE, MODE_WRITE, APV25_VFP, apvValues.getVfp (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (vfp) ;
  }
  if (vfsF) {
    //setVfs     ( apvValues.getVfs     () ) ;
    accessDeviceType vfs = { getKey(), RALMODE, MODE_WRITE, APV25_VFS, apvValues.getVfs (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (vfs) ;
  }
  if (vpspF) {
    //setVpsp    ( apvValues.getVpsp    () ) ;
    accessDeviceType vpsp = { getKey(), RALMODE, MODE_WRITE, APV25_VPSP, apvValues.getVpsp (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (vpsp) ;
  }
  if (cdrvF) {
    //setCdrv    ( apvValues.getCdrv    () ) ;
    accessDeviceType cdrv = { getKey(), RALMODE, MODE_WRITE, APV25_CDRV, apvValues.getCdrv (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (cdrv) ;
  }
  if (cselF) {
    //setCsel    ( apvValues.getCsel    () ) ;
    accessDeviceType csel = { getKey(), RALMODE, MODE_WRITE, APV25_CSEL, apvValues.getCsel (), false, 0, 0, 0, NULL} ;
    vAccess.push_back (csel) ;
  }

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = apvValues.clone() ;  
}


/** This static method read out several APV at the same time
 * \param fecAccess - hardware access
 * \param apvSet - all the APV to be readout
 * \param apvVector - list of the readout APV (suppose to be empty at the beginning). The apvDescription created must be delete by the remote method.
 * \return number of errors
 * \warning if a problem occurs in one channel then 0 is set in the corresponding registers
 */
unsigned int apvAccess::getApvValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, apvAccess *> &apvSet, deviceVector &apvVector,
						     std::list<FecExceptionHandler *> &errorList ) 
  throw (FecExceptionHandler) {

  // hash_map with the classification of the devices per ring
  Sgi::hash_map< keyType, apvDescription * > deviceDescriptionsMap ;

  // Number of errors   
  unsigned int error = 0 ; 

  // -------------------------------------------------------------------
  // read all the registers
  accessDeviceTypeListMap vAccessesApv ;
  for ( Sgi::hash_map<keyType, apvAccess *>::iterator itApv = apvSet.begin() ; itApv != apvSet.end() ; itApv ++ ) {

    accessDeviceType apvErr = { itApv->second->getKey(), RALMODE, MODE_READ, APV25_ERR_REG + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(apvErr) ;

    accessDeviceType apvMode = { itApv->second->getKey(), RALMODE, MODE_READ, APV25_MOD_REG + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(apvMode) ;

    accessDeviceType apvLatency = { itApv->second->getKey(), RALMODE, MODE_READ, APV25_LAT_REG + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(apvLatency) ;

    accessDeviceType muxGain = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_MUXGAIN + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(muxGain) ;

    accessDeviceType ipre = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_IPRE + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(ipre) ;

    accessDeviceType ipcasc  = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_IPCASC + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(ipcasc) ;

    accessDeviceType ipsf = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_IPSF + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(ipsf) ;

    accessDeviceType isha = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_ISHA + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(isha) ;

    accessDeviceType issf = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_ISSF + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(issf) ;

    accessDeviceType ipsp = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_IPSP + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(ipsp) ;

    accessDeviceType imuxin = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_IMUXIN + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(imuxin) ;

    accessDeviceType ical = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_ICAL + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(ical) ;

    //accessDeviceType ispare = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_ISPARE + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    //vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(ispare);

    accessDeviceType vfp = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_VFP + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(vfp) ;

    accessDeviceType vfs = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_VFS + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(vfs) ;

    accessDeviceType vpsp = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_VPSP + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(vpsp) ;

    accessDeviceType cdrv = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_CDRV + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(cdrv) ;

    accessDeviceType csel = {  itApv->second->getKey(), RALMODE, MODE_READ, APV25_CSEL + APV25_READ, 0, false, 0, 0, 0, NULL} ;
    vAccessesApv[getFecRingKey(itApv->second->getKey())].push_back(csel) ;

    // Put it in a map and in the vector
    deviceDescriptionsMap[itApv->second->getKey()] = new apvDescription ( itApv->second->getKey() ) ;
    apvVector.push_back(deviceDescriptionsMap[itApv->second->getKey()]) ;
    // Set the XDAQ instance & FEC hardware ID
    deviceDescriptionsMap[itApv->second->getKey()]->setFecHardwareId ( itApv->second->getFecHardwareId(), itApv->second->getCrateId() ) ;
    // Not used so set to 0
    deviceDescriptionsMap[itApv->second->getKey()]->setIspare (0) ;
  }

  // Send it over the ring and retreive the errors in a list
  error += fecAccess.setBlockDevices( vAccessesApv, errorList ) ;

  // Collect the answer and fill the corresponding apvDescription
  for (accessDeviceTypeListMap::iterator itList = vAccessesApv.begin() ; itList != vAccessesApv.end() ; itList ++) {

    // for each list
    for (accessDeviceTypeList::iterator itDevice = itList->second.begin() ; itDevice != itList->second.end() ; itDevice ++) {

      // Error or not errors
      tscType8 data = 0xFF ;
      if (itDevice->e != NULL) data = 0xFF ;
      else data = itDevice->data ;

      switch ((itDevice->offset-APV25_READ)) {
      case APV25_ERR_REG:
	deviceDescriptionsMap[itDevice->index]->setApvError (data) ;
	break ;
      case APV25_MOD_REG:
	deviceDescriptionsMap[itDevice->index]->setApvMode (data) ;
	break ;
      case APV25_LAT_REG:
	deviceDescriptionsMap[itDevice->index]->setLatency (data) ;
	break ;
      case APV25_MUXGAIN:
	deviceDescriptionsMap[itDevice->index]->setMuxGain (data) ;
	break ;
      case APV25_IPRE:
	deviceDescriptionsMap[itDevice->index]->setIpre (data) ;
	break ;
      case APV25_IPCASC:
	deviceDescriptionsMap[itDevice->index]->setIpcasc (data) ;
	break ;
      case APV25_IPSF:
	deviceDescriptionsMap[itDevice->index]->setIpsf (data) ;
	break ;
      case APV25_ISHA:
	deviceDescriptionsMap[itDevice->index]->setIsha (data) ;
	break ;
      case APV25_ISSF:
	deviceDescriptionsMap[itDevice->index]->setIssf (data) ;
	break ;
      case APV25_IPSP:
	deviceDescriptionsMap[itDevice->index]->setIpsp (data) ;
	break ;
      case APV25_IMUXIN:
	deviceDescriptionsMap[itDevice->index]->setImuxin (data) ;
	break ;
      case APV25_ICAL:
	deviceDescriptionsMap[itDevice->index]->setIcal (data) ;
	break ;
      //case APV25_ISPARE:
	//break ;
      case APV25_VFP:
	deviceDescriptionsMap[itDevice->index]->setVfp (data) ;
	break ;
      case APV25_VFS:
	deviceDescriptionsMap[itDevice->index]->setVfs (data) ;
	break ;
      case APV25_VPSP:
	deviceDescriptionsMap[itDevice->index]->setVpsp (data) ;
	break ;
      case APV25_CDRV:
	deviceDescriptionsMap[itDevice->index]->setCdrv (data) ;
	break ;
      case APV25_CSEL:
	deviceDescriptionsMap[itDevice->index]->setCsel (data) ;
	break ;
      }
    }
  }

  return (error) ;
}
