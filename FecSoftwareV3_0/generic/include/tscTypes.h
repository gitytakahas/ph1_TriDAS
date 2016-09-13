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
#ifndef TSCTYPES_H
#define TSCTYPES_H

#include <stdint.h>
#include <stdio.h>

/** To have the compatility with the version 15 of HAL and the 64 bits, please change this type in uint32_t
 */
typedef unsigned int haltype ;

/** hardware bus for the FEC
 */
//typedef enum{FECPCI, FECVME, FECUSB} enumFecBusType;
enum enumFecBusType {FECPCI, FECVME, FECUSB} ;

/** define which bus adapter for the VME FEC
 */
enum enumBusAdapter {SBS, CAENPCI, CAENUSB} ;

/** Enumeration for the SSID
 */
enum subSystemsEnum {SSIDRESERVED0,SSIDTRACKER,SSIDECAL,SSIDPRESHOWER,SSIDPIXELS,SSIDRPC,SSIDRESERVED1,SSIDRESERVED2} ;

/** Mode of i2c channel
 */
#define NORMALMODE   1
#define EXTENDEDMODE 2
#define RALMODE      3

/*!
These types are checked in beginning of TSC_CONNECT ; modifications here must
be reflected there. Checks are performed vs. first and last elements of enum_type
DEVICES_TYPE in defines.h, so keep here APV6 as first element and I2C as last
element or you will have to modify code in beginning of TSC_CONNECT.
*/

enum enumDeviceType {PLL, LASERDRIVER, DOH, DCU, PHILIPS, APVMUX, APV25, FOREXTENDED, PIARESET, FECMEMORY, DELTA, PACE, KCHIP,
                     GOH, VFAT, CCHIP, ESMBRESET, TBB, DELAY25}  ;

/** Tracker specific: two types of laserdriver driver DOH and AOH, so adding AOH
 */
#define AOH LASERDRIVER

const int deviceI2cMode[] = {
  NORMALMODE,   // PLL
  NORMALMODE,   // LASERDRIVER
  NORMALMODE,   // DOH
  NORMALMODE,   // DCU
  NORMALMODE,   // PHILIPS
  RALMODE,      // APVMUX
  RALMODE,      // APV25
  EXTENDEDMODE, // FOREXTENDED
  NORMALMODE,   // DUMMY ENTRY FOR PIARESET
  NORMALMODE,   // DUMMY ENTRY FOR FECMEMORY 
  RALMODE,      // DELTA
  RALMODE,      // PACE
  NORMALMODE,   // KCHIP
  NORMALMODE,   // GOH
  NORMALMODE,   // VFAT
  NORMALMODE,   // CCHIP
  NORMALMODE,    // DUMMY ENTRY FOR ESMBRESET
  NORMALMODE,    // TBB
  NORMALMODE     // DELAY25 
} ;

/*!
Operation read modify write
*/
enum logicalOperationType {CMD_OR,CMD_XOR,CMD_AND,CMD_EQUAL} ;

/*!
These types are checked in beginning of TSC_CONNECT ; modifications here must
be reflected there. Checks are performed vs. first and last elements of enum_type
DEVICES_TYPE in defines.h, so keep here TSC_MODE_READ as first element and
TSC_MODE_SHARE as last element or you will have to modify code in beginning of
TSC_CONNECT.
*/
enum enumAccessModeType {MODE_READ, MODE_WRITE, MODE_SHARE} ;

// FEC types
//typedef unsigned char  tscType8  ;
//typedef unsigned short tscType16 ;
//typedef unsigned long  tscType24 ;
//typedef unsigned long  tscType32 ;

typedef uint8_t tscType8  ;
typedef uint16_t tscType16 ;
typedef uint32_t tscType24 ;
typedef uint32_t tscType32 ;

/** To have direct access to the registers, see FecRingRegisters class
 */
enum fecRegisterItem {FECSR0, FECSR1, FECCR0} ;
enum ccuRegisterItem {CCUSRA, CCUSRB, CCUSRC, CCUSRD, CCUSRE, CCUSRF, CCUCRA, CCUCRB, CCUCRC, CCUCRD, CCUCRE} ;
enum i2cRegisterItem {I2CSRA, I2CSRB, I2CSRC, I2CSRD, I2CCRA} ;
enum memoryRegisterItem {MEMORYSRA, MEMORYCRA} ;
enum piaRegisterItem {PIASR, PIAGCR} ;

#endif
