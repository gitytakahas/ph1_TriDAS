/*
   FileName : 		DATATYPES.H

   Content : 		All types definitions
			If a data type must be changed, the only place to do it is here
			and nowhere else

   Used in : 		DRIVER
   			Client programs


   Programmer : 	Laurent GROSS

   Version : 		Unified-6.0
   
   Date of last modification : 03/05/2005

   Support : 		mail to : fec-support@ires.in2p3.fr
*/


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

Copyright 2002 - 2005, Laurent GROSS - IReS/IN2P3
*/


/* Begin Block - avoid multiple inclusions of this file */
#ifndef DD_DATATYPES_H
#define DD_DATATYPES_H


/*!
Include home-made values definition
*/
#include "./defines.h"
//#include "./version.h"

/*!
Include system defines
*/
#include <linux/kd.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif



/*
physical size, in bits, of the accessed registers
*/

	typedef __u32 DD_FEC_FIFO_DATA_32;
	typedef __u16 DD_FEC_FIFO_DATA_16;


	typedef __u16 DD_FEC_REGISTER_DATA;


	typedef __u16 DD_FEC_OFFSET;


	/*
	16 bits word used for internal storage and computations.
	Standart world type is the format that the end user will always
	see and use for frames manipulations.
	*/
	typedef __u16 DD_FEC_STD_WORD;

	/*
	Biggest of these three defines will be used as unified data.
	A combination between the three previous defines must be done here.
	*/
	typedef __u32 DD_FEC_UNIFIED_DATA;


/*
All Ring Errors Management variables will be defined on 32 bits.
*/
typedef __u32 DD_FEC_REM_TYPE;

/*
All Reset Counters will work on 32 bits
*/
typedef __u32 DD_RESET_COUNTER_TYPE;

/*
All Driver Status words and counters will work on 32 bits
*/
typedef __u16 DD_DRIVER_STATUS_TYPE;

/*
		DEVICE DRIVER TYPES DEFINITIONS
*/

/************************************************
	SYSTEM NATIVE TYPES TRANSLATION
************************************************/

/*!
Define format of returned error codes ; type of errors returned (internally)
by all functions.
Rem : this typedef do not bypass system error codes return types.
*/
typedef int DD_TYPE_ERROR;


/*!
Used in IOCTL command decoding by system macro
*/
typedef unsigned int DD_TYPE_IOC_COMMAND;


/*!
used in IOCTL command decoding by home made function
*/
typedef unsigned char DD_TYPE_IOC_COMMAND_KIND;


/*!
Used to convert pointers to numeric format. On Intel based systems + OS Linux,
addresses (i.e. pointers) are unsigned 32 bits words == unsigned long.
*/
typedef unsigned long DD_TYPE_ADDRESS;


/*!
PCI typedef issued from PCI rev. 2.1 documentation
*/
typedef __u16 DD_TYPE_PCI_IDENT_DEVICE;


/*!
PCI typedef issued from PCI rev. 2.1 documentation
*/
typedef __u16 DD_TYPE_PCI_IDENT_VENDOR;


/*!
PCI typedef issued from PCI rev. 2.1 documentation
Storage type of data for PCI command register.
*/
typedef __u16 DD_TYPE_PCI_COMMAND_REG;


/*!
PCI typedef issued from PCI rev. 2.1 documentation
Storage type for PCI IRQ's numbers.
*/
typedef unsigned int DD_TYPE_PCI_IRQ_NUMBER;


/*!
Size in bits of an address on the PCI bus
*/
typedef __u32 DD_TYPE_PCI_BASE_ADDRESS;



/*!
PLX address is, basically, a PCI address
*/
typedef __u32 DD_PLX_ADDRESS;

/*!
PLX data is a 32 bits word
*/
typedef __u32 DD_PLX_DATA;


/*!
Data type used by TSC High Level functions.
*/
typedef __u32 DD_TYPE_FEC_DATA32;


/*!
Data type used by FEC FIFO's and registers. FEC do not use any other format.
*/
typedef __u16 DD_TYPE_FEC_DATA16;


/*!
Data type used by CCU Control Register E (specific register).
*/
typedef __u8 DD_TYPE_FEC_DATA8;



/*!
type of value returned by function init module
Rem : type redefinition is here to allow internal functions to have any return
type on error without trashing the system with a wrong error type on returns
from system calls.
*/
typedef int DD_TYPE_INITMOD_RETURN;

/*!
type of value returned by function open module
Rem : type redefinition is here to allow internal functions to have any return
type on error without trashing the system with a wrong error type on returns
from system calls.
*/
typedef int DD_TYPE_OPEN_RETURN;

/*!
type of value returned by function release module
Rem : type redefinition is here to allow internal functions to have any return
type on error without trashing the system with a wrong error type on returns
from system calls.
*/
typedef int DD_TYPE_RELEASE_RETURN;

/*!
type of value returned by function ioctl function
Rem : type redefinition is here to allow internal functions to have any return
type on error without trashing the system with a wrong error type on returns
from system calls.
*/
typedef int DD_TYPE_IOCTL_RETURN;


/************************************************
	PRIVATE TYPES OVERLOADING
************************************************/



/*!
Array of addresses, of size = number of base adresses present on the PCI board
*/
typedef DD_TYPE_PCI_BASE_ADDRESS DD_PLX_BA_ARRAY[DD_NBR_OF_PCI_BASE_ADDRESSES];


/*!
PLX array is a 2 elements array ; An offset(u32 bits) and a value (u16 bits)
This array is used for writing VALUE, at OFFSET of a given base address.
Rem : arrays and commands prefixed by PLX are dedicated to PLX BAR0 base address;
offset 0 begins at (remapped) PLX BAR0.
Rem : maybe an error here ; isn't the offset u16 defined? to check...
*/
typedef DD_PLX_DATA DD_PLX_ARRAY[DD_PLX_ARRAY_SIZE];


/*!
FEC array is a 2 elements array ; An offset(u16 bits) and a value (u16 bits).
This array is used for writing VALUE, at OFFSET of the FEC.
Rem : arrays and commands prefixed by FEC are dedicated to PLX BAR2 base address;
offset 0 begins at (remapped) PLX BAR2.
*/
typedef DD_FEC_UNIFIED_DATA DD_FEC_ARRAY[DD_FEC_ARRAY_SIZE];


/*!
array used to store informations relatives to PCI base addresses.
- Rem : DD_NBR_OF_ITEM_BY_BASE_ADDRESS is used here, because for each base
address we store :
	- raw base address value
	- mask used for reading PCI BA info
	- PCI BA current configuration infos
	- PCI BA current mask
- Rem 1 : This array is redundant with DD_PLX_BA_ARRAY; this is due to back-
	compatibility necessities.
*/
typedef DD_TYPE_PCI_BASE_ADDRESS dd_device_base_addresses_array[DD_NBR_OF_PCI_BASE_ADDRESSES*DD_NBR_OF_ITEM_BY_BASE_ADDRESS];



/*!
Structure used as container for ioctl() calls.
*/
typedef struct dd_ioctl_data {
	DD_TYPE_ADDRESS ioctl_address;
	DD_TYPE_IOC_COMMAND ioctl_command;
	DD_TYPE_IOC_COMMAND_KIND command_kind;
	DD_FEC_ARRAY fec_array;
	DD_PLX_ARRAY plx_array;
} fake_dd_ioctl_data;






#ifdef __cplusplus
}
#endif

/* End Block - avoid multiple inclusions of this file */
#endif



