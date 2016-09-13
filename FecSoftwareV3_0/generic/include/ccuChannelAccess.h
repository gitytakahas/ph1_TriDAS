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
#ifndef CCUCHANNELACCESS_H
#define CCUCHANNELACCESS_H

#include "tscTypes.h"
#include "keyType.h"

#include "FecRingDevice.h"

/**
 * \class ccuChannelAccess
 * <p>This class is an interface that define differents methods 
 * for all the CCU channels. 
 * It defines the methods for the read - write access on every channels.
 * <BR>The channels can be:
 * <ul>
 * <li>I2C channels</li>
 * <li>PIA channels</li>
 * <li>Memory channel</li>
 * </ul>
 * <p><b>Note that the other CCU channels are not yet implemented</b>
 * <p><b>Note that all the class that access a CCU channel must be inherit from
 * this class.</b>
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class is an interface for all the CCU channels.
 */
class ccuChannelAccess {

 protected:

  /**
   * \attribut Key of the access
   * \see keytype.h that defines whath is a key.
   */
  keyType accessKey_ ;

  /**
   * \attribut In order to make the basic access to a device
   * \see FecRingDevice class
   */
  FecRingDevice *fecRingDevice_ ;

  /** Device type
   */
  enumDeviceType deviceType_ ;

 public:

  /** \brief Constructor
   */
  ccuChannelAccess ( keyType index,
                     enumDeviceType deviceType,
                     enumAccessModeType accessMask,
                     FecRingDevice *fec) 
    throw (FecExceptionHandler) ;

  /** \brief Destructor: remove the access into the FecRingDevice class
   */
  virtual ~ccuChannelAccess ( ) ;   

  /** \brief Retreive the key store in the class
   */
  keyType getKey ( ) ;

  /** \brief read into a channel
   */
  virtual tscType8 read () = 0 ;

  /** \brief write into a channel
   */
  virtual void write (tscType8 value) = 0 ;

  /** \brief In order to enable or the channel corresponding.
   */
  virtual void setChannelEnable ( bool enable ) = 0 ;

  /** \brief Initialise the corresponding channel.
   */
  virtual void setChannelInit ( ) = 0 ;
};

#endif
