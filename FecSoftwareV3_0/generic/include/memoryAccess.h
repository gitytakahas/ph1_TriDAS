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
#ifndef MEMORYACCESS_H
#define MEMORYACCESS_H

#include "tscTypes.h"
#include "keyType.h"

#include "ccuChannelAccess.h"

/**
 * \class memoryAccess
 * This class enable memory access to a device (single and multiple read and write). 
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class enable memory access
 * \warning the memory channel are not developped for old CCUs
 * \warning You need an update of the FEC firmware before using it
 */
class memoryAccess: public ccuChannelAccess {

 public:

  /** \brief Call the constructor of ccuChannelAccess 
   */
  memoryAccess ( keyType index,
                 enumAccessModeType accessMask,
                 FecRingDevice *fec ) ;

  /** \brief Call the destructor of ccuChannelAccess
   */
  virtual ~memoryAccess ( ) ;

  /** \brief Read a value from from the memory in single byte mode
   */
  tscType8 read ( ) ;

  /** \brief Read a value from from the memory in single byte mode
   */
  tscType8 read ( tscType8 AH, tscType8 AL ) ;

  /** \brief Write a value to memory in single byte mode
   */
  void write ( tscType8 value ) ;

  /** \brief Write a value to memory in single byte mode
   */
  void write ( tscType8 AH, tscType8 AL, tscType8 value ) ;

  /** \brief Read values from the memory in mutliple byte mode
   */
  void read ( tscType8 AH, tscType8 AL,
              unsigned long size,
              tscType8 *values ) ;

  /** \brief Write values to the memory in mutliple byte mode
   */
  void write ( tscType8 AH, tscType8 AL,
               tscType8 *values,
               unsigned long size) ;

  /** \brief Read modify write a value from the memory specified by the key in single byte
   */
  void write ( tscType8 AH, tscType8 AL, 
               logicalOperationType op, 
               tscType8 mask ) ;

  /** \brief Enable or disable the PIA channel
   */
  void setChannelEnable (bool enable) ;

  /** \brief Initialise the channel
   */
  void setChannelInit ( ) ;
} ;

#endif

