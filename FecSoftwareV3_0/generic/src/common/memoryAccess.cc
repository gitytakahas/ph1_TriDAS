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
#include "cmdDescription.h" 

#include "FecRingDevice.h"
#include "memoryAccess.h"

/**
 * Call the constructor of ccuChannelAccess in order to set
 * a key for this device and the hardware access
 * \param index - key for the memory channel access
 * \param accessMask - access mode (read, write, read-write)
 * \exception FecExceptionHandler
 */
memoryAccess::memoryAccess ( keyType index, 
                             enumAccessModeType accessMask,
                             FecRingDevice *fec ):

  ccuChannelAccess ( index, PHILIPS, accessMask, fec ) {
}

/**
 * disconnect the chip via the destructor of ccuChannelAccess, just
 * decrement a counter of the number of access. See the destructor of
 * ccuChannelAccess class
 */
memoryAccess::~memoryAccess ( ) { }

/**
 * Read a value from the memory in single byte mode at 0, 0 offset position
 * \return the value read in data register
 * \exception FecExceptionHandler
 */
tscType8 memoryAccess::read ( ) {

  return (read (0,0)) ;
}

/**
 * Read a value from the memory in single byte mode
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \return the value read in data register
 * \exception FecExceptionHandler
 */
tscType8 memoryAccess::read ( tscType8 AH, tscType8 AL ) {

  int value = 0 ; 
  
  value = fecRingDevice_->readFromMemory (accessKey_, AH, AL) ;

  return (value) ;
}

/** 
 * Write a value into the memory in single byte mode at 0, 0 offset position
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void memoryAccess::write ( tscType8 value) {
 
  write (0, 0, value) ;
}

/** 
 * Write a value into the memory in single byte mode
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void memoryAccess::write ( tscType8 AH, tscType8 AL, tscType8 value) {
 
  fecRingDevice_->writeIntoMemory (accessKey_, AH, AL, value) ;
}

/**
 * Read values from the memory in multiple byte mode
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param size - number of values to be read
 * \param values - pointer to an array of values
 * \exception FecExceptionHandler
 * \warning the size of the values array must be coherent with size argument
 */
void memoryAccess::read ( tscType8 AH, tscType8 AL,
                          unsigned long size,
                          tscType8 *values ) {

  fecRingDevice_->readFromMemory (accessKey_, AH, AL, size, values) ;
}

/** 
 * Write a value into the memory in multiple byte mode
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param values - values to be written
 * \param size - size of the buffer values
 * \exception FecExceptionHandler
 * \warning the size of the values array must be coherent with size argument
 */
void memoryAccess::write ( tscType8 AH, tscType8 AL, 
                           tscType8 *values,
                           unsigned long size ) {

  fecRingDevice_->writeIntoMemory (accessKey_, AH, AL, values, size) ;
}

/** Read modify write a value from the memory specified by the key in single byte
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR)
 * \param mask - mask to be done during the read modify write
 */
void memoryAccess::write ( tscType8 AH, tscType8 AL, 
                           logicalOperationType op, 
                           tscType8 mask ) {

  fecRingDevice_->writeIntoMemory (accessKey_, AH, AL, op, mask) ;
}

/**
 * Enable or disable the PIA channel corresponding to the argument
 * \param enable - boolean for enable (true) or disable (false)
 * \exception FecExceptionHandler
 */
void memoryAccess::setChannelEnable (bool enable) {

  fecRingDevice_->setChannelEnable (accessKey_, enable) ;
}

/**
 * In order to initialise the channel control register A: 
 * Value is 0x00
 * \exception FecExceptionHandler
 * \bug the second memory window is not used
 */
void memoryAccess::setChannelInit ( ) {

  fecRingDevice_->setMemoryChannelCRA (accessKey_, INITCCU25MEMORYCRA) ;
  fecRingDevice_->setMemoryChannelWin1HReg (accessKey_, MAXMEMORYWINDOWENABLE) ;
}
