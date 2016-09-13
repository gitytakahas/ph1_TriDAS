/*
   FileName : 		FEC_IOCTL.H

   Content : 		IOCTL magic number choosen for FEC operations
			IOCTL commands requested by needed FEC operations

   Used in :		DRIVER

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
#ifndef DD_FECIOCTL_H
#define DD_FECIOCTL_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
Inclusion needed in order to know if memory channels are used
*/
#include "defines.h"


/*!
choice of a magic number used for PLX IOCTL's commands
*/
#define DD_IOC_MAGIC_FEC 'p'



/*!
<b>Destination function : dd_write_to_fec</b>
- Job
	- User space send to kernel the value (unsigned 16) to write to FEC.
	- DD_FEC_ARRAY is a two values array defined as :
	(unsigned 16) array[2]={address offset to use on fec, value to write}
	- Rem : address offsets begins (offset zero) at remapped address of PLX-BA2,
	i.e. FEC Control register 0
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_WRITE
*/
#define DD_IOC_WRITE_TO_FEC_REGISTER _IOW(DD_IOC_MAGIC_FEC,0,DD_FEC_ARRAY)






/*!
<b>Destination function : dd_read_from_fec</b>
- Job
	- Kernel send to User space the value (unsigned 16) read from FEC
	- DD_FEC_ARRAY is a two values array defined as :
	(unsigned 16) array[2]={address offset to use on fec, value to read}
	- Rem : address offsets begins (offset zero) at remapped address of PLX-BA2,
	i.e. FEC Control register 0
	- If successful, the read value is stored in array[1](=value to read) ;
	this field have not to be initialised before ioctl().
- Error Management & Values returned
	- Operation successful
		- DD_RETURN_OK
	- Errors
		- DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_READ
		- DD_CANNOT_WRITE_VALUE_TO_US_FOR_FEC_READ
*/
#define DD_IOC_READ_FROM_FEC_REGISTER _IOR(DD_IOC_MAGIC_FEC,1,DD_FEC_ARRAY)




/*!
<b>Destination function : dd_get_base_address</b>
- Job
	- Kernel send to User space the value (unsigned 32) of raw
	(not remapped) PLX base address 0
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
*/
#define DD_IOC_FEC_GET_BASE0_ADDRESS _IOR(DD_IOC_MAGIC_FEC,2,DD_TYPE_PCI_BASE_ADDRESS)




/*!
<b>Destination function : dd_get_base_address</b>
- Job
	- Kernel send to User space the value (unsigned 32) of raw
	(not remapped) PLX base address 0
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
*/
#define DD_IOC_FEC_GET_BASE1_ADDRESS _IOR(DD_IOC_MAGIC_FEC,3,DD_TYPE_PCI_BASE_ADDRESS)




/*!
<b>Destination function : dd_get_base_address</b>
- Job
	- Kernel send to User space the value (unsigned 32) of raw
	(not remapped) PLX base address 2
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
*/
#define DD_IOC_FEC_GET_BASE2_ADDRESS _IOR(DD_IOC_MAGIC_FEC,4,DD_TYPE_PCI_BASE_ADDRESS)





/*!
<b>Destination function : dd_get_base_address</b>
- Job
	- Kernel send to User space the value (unsigned 32) of raw
	(not remapped) PLX base address 3
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
*/
#define DD_IOC_FEC_GET_BASE3_ADDRESS _IOR(DD_IOC_MAGIC_FEC,5,DD_TYPE_PCI_BASE_ADDRESS)





/*!
<b>Destination function : dd_get_base_address</b>
- Job
	- Kernel send to User space the value (unsigned 32) of raw
	(not remapped) PLX base address 0
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
*/
#define DD_IOC_FEC_GET_REMAPPED_BASE0_ADDRESS _IOR(DD_IOC_MAGIC_FEC,6,DD_TYPE_PCI_BASE_ADDRESS)




/*!
<b>Destination function : dd_get_base_address</b>
- Job
	- Kernel send to User space the value (unsigned 32) of raw
	(not remapped) PLX base address 0
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
*/
#define DD_IOC_FEC_GET_REMAPPED_BASE1_ADDRESS _IOR(DD_IOC_MAGIC_FEC,7,DD_TYPE_PCI_BASE_ADDRESS)




/*!
<b>Destination function : dd_get_base_address</b>
- Job
	- Kernel send to User space the value (unsigned 32) of raw
	(not remapped) PLX base address 2
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
*/
#define DD_IOC_FEC_GET_REMAPPED_BASE2_ADDRESS _IOR(DD_IOC_MAGIC_FEC,8,DD_TYPE_PCI_BASE_ADDRESS)





/*!
<b>Destination function : dd_get_base_address</b>
- Job
	- Kernel send to User space the value (unsigned 32) of raw
	(not remapped) PLX base address 3
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
*/
#define DD_IOC_FEC_GET_REMAPPED_BASE3_ADDRESS _IOR(DD_IOC_MAGIC_FEC,9,DD_TYPE_PCI_BASE_ADDRESS)





/*!
<b>DD_IOC_FEC_GET_RETURN_STATUS</b>
- Destination function : dd_get_return_status in file fastfec.c
- Can be called via function(s) : glue_fec_get_return_status in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_FEC_GET_RETURN_STATUS _IOR(DD_IOC_MAGIC_FEC,10,DD_FEC_STD_WORD)



/*!
<b>Destination function : dd_fec_init_ttcrx</b>
- Job
	- User space asks to kernel to init ttcrx
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_INIT_TTCRX_WATCHDOG
		- sub-functions EVT
*/
#define DD_IOC_FEC_INIT_TTCRX _IO(DD_IOC_MAGIC_FEC,11)




/*!
<b>Destination function : dd_fec_write_msg</b>
- Job
	- user space sends to kernel a frame to write on the CCU ring.
	The frame is passed to ioctl as an array of __u16 values, and
	should be terminated by 0x8000 OR mask.
	- If the (fake) transaction number in this frame is set to 0x00FE,
	the driver will wait for a frame acknowledge from the addressed device.
	Else, only the ring ACK will be checked.
	-If the operation is successful, value DD_RETURNED_OK is
	returned to user space. In this case, the transaction number
	(auto-assigned by the driver) is readeable in the first element
	of the ioctl array parameter. Next elements represent the BACK
	frame, i.e. the original frame with bits Address seen and
	Data Copied setted.
 	- If the operation is partially un-successful (frame corrupted),
	an error code DD_DATA_CORRUPT_ON_WRITE is returned to user space.
	In this case, the transaction status is readeable in the first
	element of the ioctl array parameter. Next elements represent
	the BACK frame, i.e. the original frame received corrupted after
	a transit on the ring.
	- If the operation is totally un-successful (no frame returned),
	an error code DD_WRITE_OPERATION_FAILED is returned. No additional
	informations are available in the ioctl array parameter.
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_TOO_MUCH_NC_ERRORS_IN_IRQ
		- DD_CANNOT_EXIT_FROM_IRQ_READ_DATA
		- DD_ERROR_MODULE_IS_RESETTING
		- DD_ERROR_IRQS_ARE_INVALIDATED
		- DD_KS_CANNOT_READ_FRAME_FROM_US_FOR_WRITE_FRAME
		- DD_TOO_LONG_FRAME_LENGTH
		- DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS0
		- DD_TIMEOUT_TRANS_NUMBER_MNGR_IN_USE
		- DD_NO_MORE_FREE_TRANSACTION_NUMBER
		- DD_TIMEOUT_FEC_IN_USE
		- DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1
		- DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS2
		- DD_DATA_CORRUPT_ON_WRITE
		- DD_WRITE_OPERATION_FAILED
		- DD_FACK_CORRUPTED
		- sub-functions EVT
*/
#define DD_IOC_FEC_WRITE_FRAME _IOR(DD_IOC_MAGIC_FEC,12,DD_FEC_STD_WORD [DD_USER_MAX_MSG_LENGTH])









/*!
<b>Destination function : dd_fec_read_msg</b>
- Job
	- user space asks to kernel the returned DATA frame (and not ACK frame
	or BACK frame) for the transaction number N.
	- If data is available (DD_RETURN_OK returned), the frame is returned
	in the ioctl array parameter. Else, an error code is returned.
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_TOO_MUCH_NC_ERRORS_IN_IRQ
		- DD_CANNOT_EXIT_FROM_IRQ_READ_DATA
		- DD_ERROR_MODULE_IS_RESETTING
		- DD_ERROR_IRQS_ARE_INVALIDATED
		- DD_FLAG_READ_COUNTER_UNCONSISTENT
		- DD_TIMEOUT_READ_REQUEST_IN_USE
		- DD_TOO_MUCH_STACKED_READ_REQUESTS
		- DD_KS_CANNOT_READ_FROM_US_FOR_READ_FRAME
		- DD_BAD_TRANSACTION_NUMBER_SENT_TO_READ
		- DD_CANNOT_READ_DATA
		- DD_KS_CANNOT_WRITE_TO_US_FOR_READ_FRAME
*/
#define DD_IOC_FEC_READ_FRAME _IOW(DD_IOC_MAGIC_FEC,13,DD_FEC_STD_WORD [DD_USER_MAX_MSG_LENGTH])




/*!
<b>Destination function : dd_fec_soft_reset_module</b>
- Job
	- a reset signal is sent to the driver ; FEC is re-initialised (soft
	reset) ; driver stays in its current state.
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_ERROR_MODULE_IS_RESETTING
		- DD_RESET_BLOCKED_BY_WRITE
		- DD_RESET_BLOCKED_BY_READ
		- sub-functions EVT
*/
#define DD_IOC_FEC_SOFT_RESET_MODULE _IO(DD_IOC_MAGIC_FEC, 14)




/*!
<b>Destination function : dd_force_count_to_one</b>
- Job
	- set USE_COUNT to ONE in module
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- Void
*/
#define DD_IOC_FEC_FORCE_COUNT_TO_ONE _IO(DD_IOC_MAGIC_FEC,15)




/*!
<b>DD_IOC_RING_ERROR_MANAGEMENT</b>
- Destination function : dd_ring_error_management in file fastfec.c
- Can be called via function(s) : glue_fec_ring_error_manager in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_RING_ERROR_MANAGEMENT _IOR(DD_IOC_MAGIC_FEC,16,DD_FEC_REM_TYPE[DD_NB_OF_ERRORS_TO_SPY])


/*!
<b>DD_IOC_GET_PLX_RESET_COUNTER</b>
- Destination function : dd_get_plx_reset_counter in file fastfec.c
- Can be called via function(s) : glue_fec_get_plx_reset_counter in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_GET_PLX_RESET_COUNTER _IOW(DD_IOC_MAGIC_FEC,17,DD_RESET_COUNTER_TYPE)


/*!
<b>DD_IOC_GET_FEC_RESET_COUNTER</b>
- Destination function : dd_get_fec_reset_counter in file fastfec.c
- Can be called via function(s) : glue_fec_get_fec_reset_counter in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_GET_FEC_RESET_COUNTER _IOW(DD_IOC_MAGIC_FEC,18,DD_RESET_COUNTER_TYPE)


/*!
<b>DD_IOC_GET_DRIVER_STATUS</b>
- Destination function : dd_get_driver_status in file fastfec.c
- Can be called via function(s) : glue_fec_get_driver_general_status in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_GET_DRIVER_STATUS     _IOW(DD_IOC_MAGIC_FEC,19,DD_DRIVER_STATUS_TYPE)




/*!
<b>DD_IOC_WAIT_WARNING</b>
- Destination function :  in file fastfec.c
- Can be called via function(s) : glue_fec_wait_warning in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_WAIT_WARNING     _IOW(DD_IOC_MAGIC_FEC,20,DD_FEC_STD_WORD [DD_USER_MAX_MSG_LENGTH])



/*!
<b>DD_IOC_SIMULATE_WARNING</b>
- Destination function :  in file fastfec.c
- Can be called via function(s) : glue_fec_simulate_warning in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_SIMULATE_WARNING     _IO(DD_IOC_MAGIC_FEC,21)



#define DD_IOC_WRITE_TO_FEC_FIFO _IOW(DD_IOC_MAGIC_FEC,22,DD_FEC_ARRAY)

#define DD_IOC_READ_FROM_FEC_FIFO _IOR(DD_IOC_MAGIC_FEC,23,DD_FEC_ARRAY)

#define DD_IOC_FEC_GET_USERS_COUNTER _IOR(DD_IOC_MAGIC_FEC,24,int)

#define DD_IOC_FEC_GET_DRIVER_VERSION _IOR(DD_IOC_MAGIC_FEC,25,char[DD_VERSION_STRING_SIZE])

/*
// returns 16 or 32
//#define DD_IOC_GET_FIFO_ITEM_SIZE _IOR(DD_IOC_MAGIC_FEC,26, sizeof(int))

// returns 0 or 1
#define DD_IOC_GET_FEC_TYPE _IOR(DD_IOC_MAGIC_FEC,27, sizeof(int))

#define DD_IOC_GET_VENDOR_ID _IOR(DD_IOC_MAGIC_FEC,28, sizeof(unsigned short))

#define DD_IOC_GET_DEVICE_ID _IOR(DD_IOC_MAGIC_FEC,29, sizeof(unsigned short))
*/

#define DD_IOC_GET_FIFO_ITEM_SIZE _IOR(DD_IOC_MAGIC_FEC,26, int)

#define DD_IOC_GET_FEC_TYPE _IOR(DD_IOC_MAGIC_FEC,27, int)

#define DD_IOC_GET_VENDOR_ID _IOR(DD_IOC_MAGIC_FEC,28, unsigned short)

#define DD_IOC_GET_DEVICE_ID _IOR(DD_IOC_MAGIC_FEC,29, unsigned short)


#ifdef __cplusplus
}
#endif



/* End Block - avoid multiple inclusions of this file */
#endif
