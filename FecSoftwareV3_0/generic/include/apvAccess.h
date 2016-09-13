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
#ifndef APVACCESS_H
#define APVACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "FecAccess.h"
#include "deviceAccess.h"
#include "apvDescription.h"

/**
 * \class apvAccess
 * This class defined an APV and make possible the downloading or
 * uploading of the values related to a specific register.
 * It also implements all the method specific to the APV device.
 * \author Frederic Drouhin
 * \date April 2002
 * \warning All the I2C accesses are done in extended/ral mode
 * \warning An offset of 1 is added to the address when a read is performed
 * \brief This class define all the hardware accesses for the APV througth the FecAccess class
 */
class apvAccess: public deviceAccess {

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  apvAccess (FecAccess *fec, 
             tscType16 fecSlot,
             tscType16 ringSlot,
             tscType16 ccuAddress,
             tscType16 i2cChannel,
             tscType16 i2cAddress ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  apvAccess (FecAccess *fec, 
             keyType key ) ;

  /** Remove the access 
   */
  ~apvAccess () ;

  /** \brief set a description for future comparison
   */
  void setDescription ( apvDescription apvValues ) ;

  /** \brief Set all values from an APV
   */
  void setValues ( apvDescription apvValues ) ;

  /** \brief Set all values for an APV in multiple frames
   */
  void getBlockWriteValues ( apvDescription apvValues, std::list<accessDeviceType> &vAccess ) ;

  /** \brief Set all values for an APV in multiple frames
   */
  void getBlockWriteValues ( apvDescription apvValues, std::list<accessDeviceType> &vAccess, 
			     bool apvModeF, bool latencyF, bool muxGainF,
			     bool ipreF, bool ipcascF, bool ipsfF,
			     bool ishaF, bool issfF, bool ipspF,
			     bool imuxinF, bool icalF, bool ispareF,
			     bool vfpF, bool vfsF, bool vpspF,
			     bool cdrvF, bool cselF, bool apvErrorF ) ;

  /** \brief Get all values from an APV
   */
  apvDescription *getValues ( ) ;

  /** \brief Set the value in the specified register
   */
  void setApvError( tscType8 ApvError );

  /** \brief Set the value in the specified register
   */
  void setApvMode ( tscType8 ApvMode  );

  /** \brief Set the value in the specified register
   */
  void setLatency ( tscType8 Latency  );

  /** \brief Set the value in the specified register
   */
  void setMuxGain ( tscType8 MuxGain  );

  /** \brief Set the value in the specified register
   */
  void setIpre    ( tscType8 Ipre     );

  /** \brief Set the value in the specified register
   */
  void setIpcasc  ( tscType8 Ipcasc   );

  /** \brief Set the value in the specified register
   */
  void setIpsf    ( tscType8 Ipsf     );

  /** \brief Set the value in the specified register
   */
  void setIsha    ( tscType8 Isha     );

  /** \brief Set the value in the specified register
   */
  void setIssf    ( tscType8 Issf     );

  /** \brief Set the value in the specified register
   */
  void setIpsp    ( tscType8 Ipsp     );

  /** \brief Set the value in the specified register
   */
  void setImuxin  ( tscType8 Imuxin   );

  /** \brief Set the value in the specified register
   */
  void setIcal    ( tscType8 Ical     );

  /** \brief Set the value in the specified register
   */
  void setIspare  ( tscType8 Ispare   );

  /** \brief Set the value in the specified register
   */
  void setVfp     ( tscType8 Vfp      );

  /** \brief Set the value in the specified register
   */
  void setVfs     ( tscType8 Vfs      );

  /** \brief Set the value in the specified register
   */
  void setVpsp    ( tscType8 Vpsp     );

  /** \brief Set the value in the specified register
   */
  void setCdrv    ( tscType8 Cdrv     );

  /** \brief Set the value in the specified register
   */
  void setCsel    ( tscType8 Csel     );
  
  /** \brief Get the value in the specified register
   */
  tscType8 getApvError ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getApvMode  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getLatency  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getMuxGain  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIpre     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIpcasc   ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIpsf     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIsha     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIssf     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIpsp     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getImuxin   ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIcal     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIspare     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfp      ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVfs      ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVpsp     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getCdrv     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getCsel     ( ) ;
  
  /** \brief static method to upload from the hardware the devices
   */
  static unsigned int getApvValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, apvAccess *> &apvSet, deviceVector &apvVector,
						   std::list<FecExceptionHandler *> &errorList ) 
    throw (FecExceptionHandler) ;

} ;

#endif
