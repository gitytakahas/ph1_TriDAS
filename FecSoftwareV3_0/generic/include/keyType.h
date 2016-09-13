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
#ifndef KEYTYPE_H
#define KEYTYPE_H

#include <asm/types.h>

/* This file define the key to define a unique device, channel, ...   */
/* The bits allowed are:                                              */
/* Device Address: 0x00 - 0xFF -> 8 bits [0, 255]                     */
/* Channel       : 0x00 - 0xFF -> 8 bits [0, 255]                     */
/* CCU Address   : 0x00 - 0xFF -> 7 bits [0, 127]                     */
/* Ring          : 0x00 - 0x07 -> 4 bits [0, 16] = [0 - 7] or [1 - 8] */
/* FEC           : 0x00 - 0x1F -> 5 bits [0, 31]                      */
/*    FEC   Ring       CCU    Channel  Device Address                 */
/* XXXX X  XXX X  XXX XXXX  XXXX XXXX      XXXX XXXXX                 */

/* Type of the key */
typedef __u32 keyType ;

/* Value for each part */
#define MASKFECKEY     0x1F
#define OFFFECKEY      27
#define MASKRINGKEY    0x0F
#define OFFRINGKEY     23
#define MASKCCUKEY     0x7F
#define OFFCCUKEY      16
#define MASKCHANNELKEY 0xFF
#define OFFCHANNELKEY  8
#define MASKADDRESSKEY 0xFF
#define OFFADDRESSKEY  0

#define MASKKEYCHANNEL 0xFFFFFF00

#define MAXCHARDECODEKEY 100

/* Macro in order to build a unique key for one device or for one channel */

/* 5 bits for the FEC                                            */
#define setFecSlotKey(x) ((keyType)((x & MASKFECKEY) << OFFFECKEY))

/* 3 bits for the RING                                           */
#define setRingKey(x) ((keyType)((x & MASKRINGKEY) << OFFRINGKEY))

/* 8 bits for the CCU                                            */
#define setCcuKey(x) ((keyType)((x & MASKCCUKEY) << OFFCCUKEY))

/* 8 bits for the channel                                        */
#define setChannelKey(x) ((keyType)((x & MASKCHANNELKEY) << OFFCHANNELKEY))

/* 8 bits for the address                                        */
#define setAddressKey(x) ((keyType)((x & MASKADDRESSKEY) << OFFADDRESSKEY))

// Not used
//#define setTypeKey(x) ((keyType)((x & 0x1) << 2))
//#define setTypeKey(x) ((keyType)(0))

/* 2 bits for the access mode (read, write, share)               */
//#define setModeKey(x) ((keyType)((x & 0x3) << 0))
//#define setModeKey(x) ((keyType)(0))

/* Build a key for the channel from a device key                 */
/* Remove the device address                                     */
#define buildChannelKey(x) ((keyType)(x & MASKKEYCHANNEL))

/* Retreive the FEC number from a key */
#define getFecKey(x) ((__u8)((x >> OFFFECKEY) & MASKFECKEY))

/* Retreive the Ring number from a key */
#define getRingKey(x) ((__u8)((x >> OFFRINGKEY) & MASKRINGKEY))

/* Retreive the CCU number from a key */
#define getCcuKey(x) ((__u8)((x >> OFFCCUKEY) & MASKCCUKEY))

/* Retreive the channel number from a key */
#define getChannelKey(x) ((__u8)((x >> OFFCHANNELKEY) & MASKCHANNELKEY))

/* Retreive the address from a key */
//#define getAddressKey(x) ((__u16)((x >> 3) & 0xFF))
#define getAddressKey(x) ((__u8)((x >> OFFADDRESSKEY) & MASKADDRESSKEY))

/* Retreive the FEC, ring key */
#define getFecRingKey(index) (setFecSlotKey(getFecKey(index)) | setRingKey(getRingKey(index)))

/* Retreive the FEC, ring, CCU, channel */
#define getFecRingCcuChannelKey(index) (setFecSlotKey(getFecKey(index)) | setRingKey(getRingKey(index)) | setCcuKey(getCcuKey(index)) | setChannelKey(getChannelKey(index)))

/* Retreive the access type from a key */
//#define getTypeKey(x) ((__u16)((x >> 2) & 0x1))
//#define getTypeKey(x) ((__u16)(x))

/* Retreive the access mode from a key */
//#define getModeKey(x) ((__u16)((x >> 0) & 0x3))
//#define getModeKey(x) (TSC_MODE_WRITE)

/** Check if an index is correct */
#define isIndexCorrectCcu25(x) ((getFecKey(x) <= 20) && (getRingKey(x) <= MAXRING) && (getCcuKey(x) > 0) && (getCcuKey(x) <= 0x7F) && (getChannelKey(x) >= 0x10) && ((getChannelKey(x) <= 0x20) || (getChannelKey(x) == 0x40) || (getChannelKey(x) == 0x50) || (getChannelKey(x) == 0x60) || ((getChannelKey(x) >= 0x30) && (getChannelKey(x) <= 0x33))) && (getAddressKey(x) <= 0xFF))
#define isi2cChannelCcu25(x) ((getChannelKey(x) >= 0x10) && (getChannelKey(x) <= 0x1F))
#define isPiaChannelCcu25(x) ((getChannelKey(x) >= 0x30) && (getChannelKey(x) <= 0x33))
#define isMemoryChannelCcu25(x) ((getChannelKey(x) == 0x40))
#define isTriggerChannelCcu25(x) ((getChannelKey(x) == 0x50))  
#define isJtagChannelCcu25(x) ((getChannelKey(x) == 0x60))
#define isBroadCastChannelCcu25(x) ((getChannelKey(x) == 0x20))
#define isNodeControllerChannelCcu25(x) ((getChannelKey(x) == 0x0))

#define isIndexCorrectOldCcu(x) ((getFecKey(x) <= 20) && (getRingKey(x) <= MAXRING) && (getCcuKey(x) > 0) && (getCcuKey(x) <= 0x7F) && (getChannelKey(x) >= 0x1) && (getChannelKey(x) <= 0x14) && (getAddressKey(x) <= 0xFF))
#define isi2cChannelOldCcu(x) ((getChannelKey(x) >= 0x01) && (getChannelKey(x) <= 0x10))
#define isPiaChannelOldCcu(x) ((getChannelKey(x) >= 0x12))
#define isMemoryChannelOldCcu(x) ((getChannelKey(x) == 0x11))
#define isTriggerChannelOldCcu(x) ((getChannelKey(x) == 0x14))  
#define isJTAGChannelOldCcu(x) (false)
#define isBroadCastChannelOldCcu(x) ((getChannelKey(x) == 0x13))
#define isNodeControllerChannelOldCcu(x) ((getChannelKey(x) == 0x0))

// Build the key for the FEC and ring 
#define buildFecRingKey(fecSlot,ringSlot) buildCompleteKey(fecSlot,ringSlot,0,0,0)

// Get the key from an index for the FEC and ring
#define getFecKeyRingKey(x) buildFecRingKey(getFecKey(x),getRingKey(x))

/** Buid a complete key */
//#define buildCompleteKey(fecSlot,ringSlot,ccuAddress,channel,address,type,mode) (setFecSlotKey(fecSlot) | setRingKey(ringSlot) | setCcuKey(ccuAddress) | setChannelKey(channel) | setAddressKey(address) | setTypeKey(type) | setModeKey(mode))
#define buildCompleteKey(fecSlot,ringSlot,ccuAddress,channel,address) ((setFecSlotKey(fecSlot)) | (setRingKey(ringSlot)) | (setCcuKey(ccuAddress)) | (setChannelKey(channel)) | (setAddressKey(address)))

/** Decode the key */
#define decodeKey(y,x) (sprintf(y, "Fec 0x%x Ring 0x%x Ccu 0x%x I2c channel 0x%x I2c address 0x%x", getFecKey(x), getRingKey(x), getCcuKey(x), getChannelKey(x), getAddressKey(x))) 

#endif
