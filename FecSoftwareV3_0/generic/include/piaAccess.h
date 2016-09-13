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

Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace,
Mulhouse-France
*/
#ifndef PIAACCESS_H
#define PIAACCESS_H

#include "tscTypes.h"
#include "keyType.h"

#include "ccuChannelAccess.h"

// From Farooq Ahmed <Farooq.Ahmed@cern.ch>
// The time delay between two reset signals on the six lines is important
// because when a module is sent a reset there is current change on the power
// lines and due to this rapid change of current there can be overshoot of
// voltage, so in order to reduce it we want reset one line at a time and then
// let it settle down which may take not more than a few microseconds, then we
// send the reset to other module and so on with a safe delay. The figure I
// mentioned 50 to 100 is not critical it can be from 10s of uSec to a
// reasonable time.

/**
 * \class piaAccess
 * This class enable PIA access to a device (read and write). It can
 * also enable/disable a PIA channel and initialise a channel to a specified value.
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class enable PIA access to a device
 * \warning the PIA channels are not developped for old CCUs
 */
class piaAccess: public ccuChannelAccess {

 public:
  /** \brief Call the constructor of ccuChannelAccess 
   */
  piaAccess ( keyType index,
              enumAccessModeType accessMask,
              FecRingDevice *fec ) ;

  /** \brief Call the destructor of ccuChannelAccess
   */
  virtual ~piaAccess ( ) ;

  /** \brief Read a value from the data register
   */
  tscType8 read ( ) ;

  /** \brief Write a value to the data register
   */
  void write (tscType8 value) ;

  /** \brief Enable or disable the PIA channel
   */
  void setChannelEnable (bool enable) ;

  /** \brief Initialise the channel
   */
  void setChannelInit ( ) ;
} ;

#endif

