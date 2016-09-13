/*
   FileName : 		DD_GLUE.H

   Content : 		Entry points for users low-level accesses to the
   			FEC device driver - exports of functions defined
			in glue.c

   Used in : 		Device Driver interface library (libfec_glue.a/o/so)
   			Client Programs

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


/*!
GENERAL NOTE :
All the values #defined in this API description can be found in the file defines.h
All the datatypes defined in this API description can be found in the file datatypes.h
*/


/*! Begin Block - avoid multiple inclusions of this file */
#ifndef DD_GLUE_H
#define DD_GLUE_H


#ifdef __cplusplus
extern "C" {
#endif



#include "datatypes.h"






/*!
Read the content of the command register of the PCI board, value returned in param_comreg.
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_COMMAN_REG is actually defined as __u16
*/
DD_TYPE_ERROR glue_pci_get_command_reg(int param_fd, DD_TYPE_PCI_COMMAND_REG *param_comreg);


/*!
Get raw PLX Base address 0, value returned in param_ba0
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_BASE_ADDRESS is actually defined as __u32
*/
DD_TYPE_ERROR glue_plx_get_raw_ba0(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba0);




/*!
Get raw PLX Base address 1, value returned in param_ba1
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_BASE_ADDRESS is actually defined as __u32
*/
DD_TYPE_ERROR glue_plx_get_raw_ba1(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba1);


/*!
Get raw PLX Base address 2, value returned in param_ba2
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_BASE_ADDRESS is actually defined as __u32
*/
DD_TYPE_ERROR glue_plx_get_raw_ba2(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba2);


/*!
Get raw PLX Base address 3, value returned in param_ba3
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_BASE_ADDRESS is actually defined as __u32
*/
DD_TYPE_ERROR glue_plx_get_raw_ba3(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba3);





/*!
Get remapped PLX Base address 0, value returned in param_ba0
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_BASE_ADDRESS is actually defined as __u32
*/
DD_TYPE_ERROR glue_plx_get_remapped_ba0(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba0);



/*!
Get remapped PLX Base address 1, value returned in param_ba1
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_BASE_ADDRESS is actually defined as __u32
*/
DD_TYPE_ERROR glue_plx_get_remapped_ba1(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba1);


/*!
Get remapped PLX Base address 2, value returned in param_ba2
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_BASE_ADDRESS is actually defined as __u32
*/
DD_TYPE_ERROR glue_plx_get_remapped_ba2(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba2);


/*!
Get remapped PLX Base address 3, value returned in param_ba3
DD_TYPE_ERROR is actually defined as int
DD_TYPE_PCI_BASE_ADDRESS is actually defined as __u32
*/
DD_TYPE_ERROR glue_plx_get_remapped_ba3(int param_fd, DD_TYPE_PCI_BASE_ADDRESS *param_ba3);



/*!
Performs a hard reset of the PLX, and resets all the driver internals.
DD_TYPE_ERROR is actually defined as int
*/
DD_TYPE_ERROR glue_plx_hard_reset(int param_fd);


/*!
Activate the interrupt line of the PLX9080 board.
DD_TYPE_ERROR is actually defined as int
*/
DD_TYPE_ERROR glue_plx_enable_irqs(int param_fd);


/*!
Desactivate the interrupt line of the PLX9080 board.
DD_TYPE_ERROR is actually defined as int
*/
DD_TYPE_ERROR glue_plx_disable_irqs(int param_fd);



/*!
Allow to write read value from any offset of the PLX9080.
Value is read from PLX9080 address : Remapped BAR0
DD_TYPE_ERROR is actually defined as int
DD_PLX_ARRAY is actually defined as __u32 my_array[2]
my_array[0] must contain the offset from where to read
my_array[1] will contain the read value
*/
//DD_TYPE_ERROR glue_plx_get_value(int param_fd, DD_PLX_ARRAY param_offset_and_data);
DD_TYPE_ERROR glue_plx_get_value(int param_fd, __u32 param_offset, __u32 *param_data);




/*!
Allow to write any value at any offset of the PLX9080.
Value is wrote at PLX9080 address : Remapped BAR0
DD_TYPE_ERROR is actually defined as int
DD_PLX_ARRAY is actually defined as __u32 my_array[2]
my_array[0] must contain the offset for writing
my_array[1] must contain the value to write
*/
//DD_TYPE_ERROR glue_plx_set_value(int param_fd, DD_PLX_ARRAY param_offset_and_data);
DD_TYPE_ERROR glue_plx_set_value(int param_fd, __u32 param_offset, __u32 param_data);

/*!
Remap all PLX base addresses towards usable remapped addresses
DD_TYPE_ERROR is actually defined as int
*/
DD_TYPE_ERROR glue_plx_remap_base_addresses(int param_fd);










/*!
Write the value of param_control0 into FEC control 0 register
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_set_ctrl0(int param_fd, DD_FEC_REGISTER_DATA param_control0);



/*!
Read the content of FEC control 0 register, value returned in param_control0
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_ctrl0(int param_fd, DD_FEC_REGISTER_DATA *param_control0);



/*!
Write the value of param_control1 into FEC control 1 register
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_set_ctrl1(int param_fd, DD_FEC_REGISTER_DATA param_control1);


DD_TYPE_ERROR glue_fec_get_ctrl1(int param_fd, DD_FEC_REGISTER_DATA *param_control1);



/*!
Read the content of FEC status 0 register, value returned in param_status0
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_status0(int param_fd, DD_FEC_REGISTER_DATA *param_status0);




/*!
Read the content of FEC status 1 register, value returned in param_status1
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_status1(int param_fd, DD_FEC_REGISTER_DATA *param_status1);


/* Field added 17/01/2005 - This function exists only on 32 bits FECs */
/*!
Read the content of FEC SOURCE register, value returned in param_source
DD_TYPE_ERROR is actually defined as int
DD_FEC_REGISTER_DATA is actually defined as __u16
*/
//DD_TYPE_ERROR glue_fec_get_source_register(int param_fd, DD_FEC_REGISTER_DATA *param_source);


/* Field added 17/01/2005 - This function exists only on 32 bits FECs */
/*!
Read the content of FEC VERSION register, value returned in param_version
DD_TYPE_ERROR is actually defined as int
DD_FEC_REGISTER_DATA is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_version_register(int param_fd, DD_FEC_REGISTER_DATA *param_version);


/*!
Write the value of *param_fifotra into FEC FIFOTRANSMIT fifo
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_set_frame_in_fifotra_32(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength);

DD_TYPE_ERROR glue_fec_set_frame_in_fifotra_16(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength);

/*!
Write the value of param_fifotra into FEC FIFOTRANSMIT fifo
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_set_native_fifotra_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fifotra);
DD_TYPE_ERROR glue_fec_set_native_fifotra_item_16(int param_fd, DD_FEC_FIFO_DATA_16 param_fifotra);


/*!
Read the content of FEC FIFOTRANSMIT fifo, value returned in param_fifotra
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_fifotra_item_32(int param_fd, DD_FEC_STD_WORD *param_fifotra);
DD_TYPE_ERROR glue_fec_get_fifotra_item_16(int param_fd, DD_FEC_STD_WORD *param_fifotra);
DD_TYPE_ERROR glue_fec_get_fifotra_item(int param_fd, DD_FEC_STD_WORD *param_fifotra);

/*!
Read the content of FEC FIFOTRANSMIT fifo, value returned in param_fifotra
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_native_fifotra_item_32(int param_fd, DD_FEC_FIFO_DATA_32 *param_fifotra);
DD_TYPE_ERROR glue_fec_get_native_fifotra_item_16(int param_fd, DD_FEC_FIFO_DATA_16 *param_fifotra);



/*!
Write the value of *param_fiforet into FEC FIFORETURN fifo
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_set_frame_in_fiforet_32(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength);
DD_TYPE_ERROR glue_fec_set_frame_in_fiforet_16(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength);


/*!
Write the value of param_fiforet into FEC FIFORETURN fifo
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_set_native_fiforet_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fiforet);
DD_TYPE_ERROR glue_fec_set_native_fiforet_item_16(int param_fd, DD_FEC_FIFO_DATA_16 param_fiforet);


/*!
Read the content of FEC FIFORETURN fifo, value returned in param_fiforet
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_fiforet_item_32(int param_fd, DD_FEC_STD_WORD *param_fiforet);
DD_TYPE_ERROR glue_fec_get_fiforet_item_16(int param_fd, DD_FEC_STD_WORD *param_fiforet);
DD_TYPE_ERROR glue_fec_get_fiforet_item(int param_fd, DD_FEC_STD_WORD *param_fiforet);

/*!
Read the content of FEC FIFORETURN fifo, value returned in param_fiforet
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_native_fiforet_item_32(int param_fd, DD_FEC_FIFO_DATA_32 *param_fiforet);
DD_TYPE_ERROR glue_fec_get_native_fiforet_item_16(int param_fd, DD_FEC_FIFO_DATA_16 *param_fiforet);



/*!
Write the value of *param_fiforec into FEC FIFORECEIVE fifo
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_set_frame_in_fiforec_32(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength);
DD_TYPE_ERROR glue_fec_set_frame_in_fiforec_16(int param_fd, DD_FEC_STD_WORD *param_stdframecontent, DD_FEC_STD_WORD param_stdframelength);


/*!
Write the value of param_fiforec into FEC FIFORECEIVE fifo
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_set_native_fiforec_item_32(int param_fd, DD_FEC_FIFO_DATA_32 param_fiforec);
DD_TYPE_ERROR glue_fec_set_native_fiforec_item_16(int param_fd, DD_FEC_FIFO_DATA_16 param_fiforec);


/*!
Read the content of FEC FIFORECEIVE fifo, value returned in param_fiforec
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_fiforec_item_32(int param_fd, DD_FEC_STD_WORD *param_fiforec);
DD_TYPE_ERROR glue_fec_get_fiforec_item_16(int param_fd, DD_FEC_STD_WORD *param_fiforec);
DD_TYPE_ERROR glue_fec_get_fiforec_item(int param_fd, DD_FEC_STD_WORD *param_fiforec);

/*!
Read the content of FEC FIFORECEIVE fifo, value returned in param_fiforec
DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
*/
DD_TYPE_ERROR glue_fec_get_native_fiforec_item_32(int param_fd, DD_FEC_FIFO_DATA_32 *param_fiforec);
DD_TYPE_ERROR glue_fec_get_native_fiforec_item_16(int param_fd, DD_FEC_FIFO_DATA_16 *param_fiforec);




DD_TYPE_ERROR glue_fec_get_return_status(int param_fd, DD_FEC_STD_WORD *param_status);


/*!
TTcRx initialisation and reset
DD_TYPE_ERROR is actually defined as int
*/
DD_TYPE_ERROR glue_fec_init_ttcrx(int param_fd);


/*!
Write a whole frame into fifo transmit, sends the frame on the ring,
and send report on the good/bad achievement of the transaction.

The frame, contained in the array param_stack, must be "ring ready" i.e. well formed.
Only the transaction number of this frame can be free, as this number will be
auto-assigned by the driver.

After call of this method, the returned values are :
param_stack[0] contains the transaction number that have been assigned by
the driver for this transaction.
Next elements of param_stack contains the acknowledged frame received after ring loop.

Rem : If this frame is addressing an i2c device, if ACKs from i2c devices are forced, and
if you want to be able to read back the acknowledgement frame further on with the
glue_fec_read_frame method, you must set the transaction number to 0x00FE in the sent frame.

DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
DD_MAX_MSG_LENGTH is actually defined as 16
*/
DD_TYPE_ERROR glue_fec_write_frame(int param_fd, DD_FEC_STD_WORD *param_stack);




/*!
Read a frame issued from a request for data on a device, or a frame issued
from a forced acknowledge sent by an i2c device.


Before call of this method, param_stack[0] must contain the number of the transaction
for which we want to read back a frame (data or ack).

After call, param_stack contains the received data or ack frame ; param_stack[0] is overwritten,
the received frame is stored beginning at param_stack[0], index included.

DD_TYPE_ERROR is actually defined as int
DD_FEC_WORD is actually defined as __u16
DD_MAX_MSG_LENGTH is actually defined as 16
*/
DD_TYPE_ERROR glue_fec_read_frame(int param_fd, DD_FEC_STD_WORD *param_stack);



/*!
Performs a soft reset of the FEC, and do not resets all the driver internals.
DD_TYPE_ERROR is actually defined as int
*/

DD_TYPE_ERROR glue_fec_soft_reset(int param_fd);


/* DEPRECIATED - DO NOT USE ANYMORE.
Rather use explicit functions (write to control register 0, etc.) */
DD_TYPE_ERROR glue_fec_write_register(int param_fd, DD_FEC_UNIFIED_DATA *param_array);


DD_TYPE_ERROR glue_fec_write_16(int param_fd, __u16 value, __u16 offset);

DD_TYPE_ERROR glue_fec_write_32(int param_fd, __u32 value, __u32 offset);

/* DEPRECIATED - DO NOT USE ANYMORE.
Rather use explicit functions (Read from control register 0, etc.) */
DD_TYPE_ERROR glue_fec_read_register(int param_fd, DD_FEC_UNIFIED_DATA *param_array);


DD_TYPE_ERROR glue_fec_read_16(int param_fd, __u16 *value, __u16 offset);
DD_TYPE_ERROR glue_fec_read_32(int param_fd, __u32 *value, __u32 offset);

/*!
private ; reset the driver USER_COUNT variable to 1
*/
DD_TYPE_ERROR glue_fecdriver_force_count_to_one(int param_fd);


DD_TYPE_ERROR glue_fecdriver_get_users_counter(int param_fd, int *param_count);


DD_TYPE_ERROR glue_fecdriver_get_driver_version(int param_fd, char *param_version_string);

/*
DD_TYPE_ERROR is actually of type : int
the message stack -msg- must be at least of : DD_MAX_DECODED_ERROR_MSG_LENGTH = 512 chars long, included terminating \0.
*/
DD_TYPE_ERROR glue_fecdriver_get_error_message(DD_TYPE_ERROR code, char *msg);


DD_TYPE_ERROR glue_fecdriver_print_error_message(DD_TYPE_ERROR code);


/*!
<b>FUNCTION 	: glue_fec_ring_error_manager</b>
- Job
	- Allow the tracking and keep the memory of low level errors like ring losses, ring noisy, etc...

- Inputs
	- int param_fd : File descriptor for the Device Driver
	- DD_TYPE_FEC_DATA32* param_cmd) : Pointer to an array of DD_NB_OF_ERRORS_TO_SPY elements.
	Each element of this array is a counter dedicated to a specific error.
- Outputs
	- Error Code
- Internals
	- Hidden ; Kernel side executive
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_BAD_IOC_DESTINATION
		- DD_KS_CANNOT_WRITE_TO_US_IN_HUB
		- DD_KS_CANNOT_READ_FROM_US_IN_HUB
		- DD_MAIN_ERROR_MODULE_IS_RESETTING
		- DD_FEC_COMMAND_NOT_ASSOCIATED
		- DD_FEC_CANNOT_READ_VALUE_FROM_US
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US
		- DD_INVALID_REM_CMD
- Notes
	- When calling this method, the first element of the stack must be
	setted in order to define the action to perform :
		- param_cmd[0]=DD_TURN_REM_ON -> enables/launch errors counting in the driver
		- param_cmd[0]=DD_TURN_REM_OFF -> disables/stops errors counting in the driver
		- param_cmd[0]=DD_RESET_REM_VALUE -> resets all error counters to zero
		- param_cmd[0]=DD_GET_REM_VALUE -> request to read out the error counters.
		Values are returned in *param_cmd[], and what you can actually read is :
			- param_cmd[DD_LONG_FRAME_ERROR_OFFSET] : A too long frame (more than DD_USER_MAX_MSG_LENGTH)
			has been received in fifo receive
			- param_cmd[DD_SHORT_FRAME_ERROR_OFFSET] : A too short frame (less than DD_NOISE_LENGTH,
			aka ring noisy) has been received in fifo receive
			- param_cmd[DD_WRONG_TRANS_NUMBER_ERROR_OFFSET] : A frame incoming in fifo receive
			had a wrong transaction number (>0x7F)
			- param_cmd[DD_WARNING_FROM_DEVICE_ERROR_OFFSET] : A device has emitted a warning frame.
			The last emitted warning frame can be read out by calling the glue_fec_read_frame method
			with a transaction number 0 (zero) as parameter.
		- param_cmd[0]=DD_GET_REM_STATUS -> reads out the current status of the ring error manager.
		Status is returned in param_cmd[0] and can be DD_FLAG_IS_DISABLED (error counting stopped)
		or DD_FLAG_IS_ENABLED (error counting enabled).
	- DD_TYPE_ERROR is actually defined as int
	- DD_TYPE_FEC_DATA32 is actually defined as __u32
*/
DD_TYPE_ERROR glue_fec_ring_error_manager(int param_fd, DD_FEC_REM_TYPE* param_cmd);




/*!
<b>FUNCTION 	: glue_fec_get_plx_reset_counter</b>
- Job
	- Returns the number of reset applied to the PLX since the driver was loaded.

- Inputs
	- int param_fd : File descriptor for the Device Driver
	- DD_TYPE_FEC_DATA32* param_cmd : Pointer to a word where the value
	of the plx resets counter will be stored.
- Outputs
	- Error Code
- Internals
	- Hidden ; Kernel side executive
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_BAD_IOC_DESTINATION
		- DD_KS_CANNOT_WRITE_TO_US_IN_HUB
		- DD_KS_CANNOT_READ_FROM_US_IN_HUB
		- DD_MAIN_ERROR_MODULE_IS_RESETTING
		- DD_FEC_COMMAND_NOT_ASSOCIATED
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US
- Notes
	- DD_TYPE_ERROR is actually defined as int
	- DD_TYPE_FEC_DATA32 is actually defined as __u32
	- This counter can not be resetted by users. Only way to reset it is to unload/reload the driver.
*/
DD_TYPE_ERROR glue_fec_get_plx_reset_counter(int param_fd, DD_RESET_COUNTER_TYPE* param_cmd);




/*!
<b>FUNCTION 	: glue_fec_get_fec_reset_counter</b>
- Job
	- Returns the number of reset applied to the FEC (toggle B15 of SR0[15..0])
	since the driver was loaded.

- Inputs
	- int param_fd : File descriptor for the Device Driver
	- DD_TYPE_FEC_DATA32* param_cmd : Pointer to a word where the value
	of the fec resets counter will be stored.
- Outputs
	- Error Code
- Internals
	- Hidden ; Kernel side executive
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_BAD_IOC_DESTINATION
		- DD_KS_CANNOT_WRITE_TO_US_IN_HUB
		- DD_KS_CANNOT_READ_FROM_US_IN_HUB
		- DD_MAIN_ERROR_MODULE_IS_RESETTING
		- DD_FEC_COMMAND_NOT_ASSOCIATED
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US
- Notes
	- DD_TYPE_ERROR is actually defined as int
	- DD_TYPE_FEC_DATA32 is actually defined as __u32
	- This counter can not be resetted by users. Only way to reset it is to unload/reload the driver.
*/
DD_TYPE_ERROR glue_fec_get_fec_reset_counter(int param_fd, DD_RESET_COUNTER_TYPE* param_cmd);




/*!
<b>FUNCTION 	: glue_fec_get_driver_general_status</b>
- Job
	- Returns a bitfield (word) which gives the current status of the driver
	regarding PLX IRQs status and error reports.

- Inputs
	- int param_fd : File descriptor for the Device Driver
	- DD_TYPE_FEC_DATA16* param_cmd : Pointer to a word where the bitfield
	representative of the driver status will be stored.
- Outputs
	- Error Code
- Internals
	- Hidden ; Kernel side executive
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_BAD_IOC_DESTINATION
		- DD_KS_CANNOT_WRITE_TO_US_IN_HUB
		- DD_KS_CANNOT_READ_FROM_US_IN_HUB
		- DD_MAIN_ERROR_MODULE_IS_RESETTING
		- DD_FEC_COMMAND_NOT_ASSOCIATED
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US
- Notes
	- DD_TYPE_ERROR is actually defined as int
	- DD_TYPE_FEC_DATA16 is actually defined as __u16
	- The word of status can be read as :
		- if bit DD_STATUS_FLAG_IRQ_IS_ON is raised (1), PLX IRQs are enabled in the FEC driver.
		Else,PLX IRQs are disabled in the FEC driver.
		You can test this with the macro isIrqOn(x), true if IRQs are on.
		- if bit DD_STATUS_FLAG_LONG_FRAME_ERROR is raised (1), some errors relatives
		to too long frames has occured.
		You can test this with the macro existsFaultOnLongFrames(x), true if at least one
		error of this kind has occured since last error counters reset.
		- if bit DD_STATUS_FLAG_SHORT_FRAME_ERROR is raised (1), some errors relatives
		to too short frames has occured.
		You can test this with the macro existsFaultOnShortFrames(x), true if at least one
		error of this kind has occured since last error counters reset.
		- if bit DD_STATUS_FLAG_BAD_TRANS_NUMBER_ERROR is raised (1), some errors relatives
		to frames incoming in fifo receive with bad transaction numbers has occured.
		You can test this with the macro existsFaultOnTransNumber(x), true if at least one
		error of this kind has occured since last error counters reset.
		- if bit DD_STATUS_FLAG_WARNING_EMITTED_BY_DEVICE_ERROR is raised (1), we have at least a
		device which has emitted a warning. See method glue_fec_ring_error_manager to know how
		to read back the warning frame.
		You can test this with the macro existsWarningEmittedByDevice(x), true if at least one
		error of this kind has occured since last error counters reset.
		- if bit DD_STATUS_FLAG_ERRORCOUNTING_IS_ON is raised (1), the ring error management
		system is enabled in the FEC driver. Else, the ring error management system is
		disabled in the FEC driver.
		You can test this with the macro isErrorCountingEnabled(x), true if error counting is enabled.
*/
DD_TYPE_ERROR glue_fec_get_driver_general_status(int param_fd, DD_DRIVER_STATUS_TYPE* param_cmd);



/*!
<b>FUNCTION : glue_fec_wait_warning</b>
- Job
	- Waits that a -warning emitted by a device- message occurs on the ring and sends it back to the user.
- I/O
	- Inputs
		- int fd : pointer to the file descriptor of the device driver to close.
		- DD_FEC_WORD *param_stack : An array in which the frame readed back will be stored.
		No initial value has to be setted for param_stack[0].
		See the notes section below about array content, size and frame format.

	- Outputs
		- Error Code returned by the function
- Internals
	- Void
- Global variables accessed
	- void
- Notes
	- DD_TYPE_ERROR is actually defined as int
	- For error code meanings and values, see file includes/dderrors.h
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_CANNOT_CLOSE_FEC_DRIVER
- Sub-functions calls
	- close		(system) (EVT)
*/
//BE CAREFUL ; IF YOU USE THIS FUNCTION SOMEWHERE, ALWAYS ENDS YOUR PROGRAM
//BY A CALL TO THE glue_fec_simulate_warning FUNCTION
DD_TYPE_ERROR glue_fec_wait_warning(int param_fd, DD_FEC_STD_WORD *param_stack);



DD_TYPE_ERROR glue_fec_simulate_warning(int param_fd);




/*!
<b>FUNCTION : glue_fec_open_device</b>
- Job
	- Opens the device driver via the associated /dev/XXX file.
- I/O
	- Inputs
		- char *device_name : string that must contain the name of the device driver to open
		- int device_mode : Mode in which the driver must be opened (blocking, non blocking, etc...)
		See your OS documentation for more informations about the values you can set here.
	- Outputs
		- int *fd : pointer to the device driver file descriptor, will be setted
		by the open method.
		- Error Code returned by the function
- Internals
	- Hidden ; Kernel side executive
- Global variables accessed
	- void
- Notes
	- DD_TYPE_ERROR is actually defined as int
	- For error code meanings and values, see file includes/dderrors.h
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_CANNOT_OPEN_FEC_DRIVER
- Sub-functions calls
	- open		(system) (EVT)
*/
DD_TYPE_ERROR  glue_fec_open_device(int *fd, char *device_name, int device_mode);




/*!
<b>FUNCTION : glue_fec_close_device</b>
- Job
	- Closes the FEC device driver designated by its file descriptor.
- I/O
	- Inputs
		- int fd : pointer to the file descriptor of the device driver to close.
	- Outputs
		- Error Code returned by the function
- Internals
	- Void
- Global variables accessed
	- void
- Notes
	- DD_TYPE_ERROR is actually defined as int
	- For error code meanings and values, see file includes/dderrors.h
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_CANNOT_CLOSE_FEC_DRIVER
- Sub-functions calls
	- close		(system) (EVT)
*/
DD_TYPE_ERROR glue_fec_close_device(int fd);


DD_TYPE_ERROR glue_fec_get_fifo_item_size(int fd, int *size_in_bits);

/* A returned value equal to 0 means : The driver is associated to an electrical FEC
A returned value equal to 1 means : The driver is associated to an optical FEC */
//DD_TYPE_ERROR glue_fec_get_driver_type(int param_fd, int *type);



DD_TYPE_ERROR glue_fec_get_firmware_version(int param_fd, DD_FEC_REGISTER_DATA *param_firmware_version);



//DD_TYPE_ERROR glue_fec_get_firmware_date(int param_fd, DD_FEC_REGISTER_DATA *param_firmware_date);


DD_TYPE_ERROR glue_fec_get_fec_type(int param_fd, int *param_fec_type);
DD_TYPE_ERROR glue_fec_get_vendor_id(int param_fd, unsigned short *param_vendor_id);
DD_TYPE_ERROR glue_fec_get_device_id(int param_fd, unsigned short *param_device_id);


#ifdef __cplusplus
}
#endif

#endif












