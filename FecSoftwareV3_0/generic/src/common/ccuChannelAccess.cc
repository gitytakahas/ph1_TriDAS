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

#include "ccuChannelAccess.h"

/**
 * \param param_fec_number - fec number
 * \param param_ring_number - ring number
 * \param param_ccu_number - CCU address
 * \param param_address - Device address
 * \param param_device_type - ral or philips mode
 * \param param_access_mask - access mode (read, write, read-write)
 * \param fec - FecDevice class for the hardware access
 * \exception FecExceptionHandler:
 * <ul>
 * <li>
 * </ul>
 */
ccuChannelAccess::ccuChannelAccess ( keyType index,
                                     enumDeviceType deviceType,
                                     enumAccessModeType accessMask,
                                     FecRingDevice *fec) 
  throw (FecExceptionHandler) {
    
  /** Build the key for the device */
  accessKey_ = index ;

#ifdef T3TDLEVELCHECK
  /** Check the values of each param in order to see if it's correct
   */
  if ( (! isIndexCorrectCcu25(accessKey_)) && (! isIndexCorrectOldCcu(accessKey_)) ) {
    accessKey_ = 0 ;
    RAISEFECEXCEPTIONHANDLER ( TSCFEC_FECPARAMETERNOTMANAGED, "Wrong index for a channel", FATALERRORCODE ) ;
  }
#endif

  /** Make a pointer to the fec in order to access it 
   */
  fecRingDevice_ = fec ;

  /** Store the device type in order to use the correct i2c mode (extended, normal, ral)
   */
  deviceType_ = deviceType ;
}

/**
 * Destructor: remove the access into the FecDevice
 */
ccuChannelAccess::~ccuChannelAccess ( ) {

  /** Decrement an access for the device driver
   */
  if (accessKey_ != 0) fecRingDevice_->removeAccess ( ) ;
}

/**
 * Retreive the key store in the class
 */
keyType ccuChannelAccess::getKey ( ) {
  return (accessKey_) ;
}
