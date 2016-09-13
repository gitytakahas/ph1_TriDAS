/* File auto-generated - Do not modify */
/*
	FileName :		DD_PERROR.C

	Content :		Transcription of numeric error code towards text report

	Used in :		Client program

	Programmer :		Laurent GROSS

	Version :		Unified-6.0

	Support :		mail to : fec-support@ires.in2p3.fr

	Remarks :		This file should not be modified by users ;
				File auto - generated from dderrors.h descriptions.
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


/*
Standard includes
*/
#include <stdio.h>
#include <string.h>


/*
device driver dependant includes
*/
#include "../include/datatypes.h"
#include "../include/dderrors.h"


/*
specific includes
*/
#include "../include/dd_perror.h"


/*!
See the public file dd_perror.h for this API description.
*/
DD_TYPE_ERROR dd_get_error_msg ( DD_TYPE_ERROR code, char *msg )
{
	strcpy(msg,"");
	dd_decode_error ( code, msg);

return DD_RETURN_OK;
}


/*!
See the public file dd_perror.h for this API description.
*/
DD_TYPE_ERROR dd_print_error_msg ( DD_TYPE_ERROR code)
{
char lcl_error_msg[DD_MAX_DECODED_ERROR_MSG_LENGTH];

	dd_decode_error ( code, lcl_error_msg);
	fprintf(stderr, "Error %d : %s\n", code, lcl_error_msg);

return DD_RETURN_OK;
}


/*!
See the public file dd_perror.h for this API description.
*/
DD_TYPE_ERROR dd_decode_error ( DD_TYPE_ERROR param_code, char *param_errormessage )
{
	switch (param_code)
	{
		case DD_NO_ERROR:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The function/command has been successfully executed.");
		break;
		case DD_SIG_ON_IOCTL_SEM:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"I was not able to raise the interlock management semaphore, when I entered the main ioctl() function. IOCTL() request have been rejected.");
		break;
		case DD_BAD_IOC_DESTINATION:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"IOCTL() destination (FEC | PLX | PCI) undefined. Looks like there is a problem with the magic numbers of the ioctl() calls defined in files pci_ioctl.h, plx_ioctl.h, fec_ioctl.");
		break;
		case DD_NO_PCI_SOFTWARE_SUPPORT:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Your OS does not provides software support for PCI. The kernel of your computer must be compiled with PCI support option activated.");
		break;
		case DD_NO_PCI_HARDWARE_SUPPORT:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Your computer does not appears to have, physically, a PCI bus.");
		break;
		case DD_PCI_DEVICE_NOT_FOUND:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The PCI device identified by DEVICE_ID & VENDR_ID (values defined in file defines.h) has not been detected on the PCI bus of your computer.");
		break;
		case DD_PCI_ERROR_READ_COMMAND_REG:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Error while reading the PCI COMMAND Register from the PCI board.");
		break;
		case DD_PCI_KS_CANNOT_WRITE_COMMAND_REG_TO_US:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space cannot write the previously read value COMMAND register of the PCI board to user space.");
		break;
		case DD_PCI_KS_CANNOT_WRITE_IRQ_NUMBER_TO_US:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space cannot write the previously read value IRQ number of the PCI board to user space.");
		break;
		case DD_PCI_KS_CANNOT_WRITE_BA_ARRAY_TO_US:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space cannot write the previously read value base addresses array to user space.");
		break;
		case DD_PCI_KS_CANNOT_READ_CTRL_REGISTER_FROM_US:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space cannot read the value of CONTROL register for the PCI board from user space.");
		break;
		case DD_PCI_ERROR_WRITE_CTRL_REGISTER:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Error while writing in the CONTROL register of the PCI board.");
		break;
		case DD_PCI_COMMAND_NOT_ASSOCIATED:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The ioctl() command has been recognized as a PCI command, but no action is associated to it.");
		break;
		case DD_PLX_ERROR_REMAPPING_PLX_BA:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Remapping of PLX Base Addresse towards locale failed ; NULL pointer returned.");
		break;
		case DD_PLX_COMMAND_NOT_ASSOCIATED:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The ioctl() command has been recognized as a PLX command, but no action is associated to it.");
		break;
		case DD_FEC_COMMAND_NOT_ASSOCIATED:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The ioctl() command has been recognized as a FEC command, but no action is associated to it.");
		break;
		case DD_ERROR_REGISTER_MODULE:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The device driver can not register himself to the kernel ; may be a /dev/devicename problem.");
		break;
		case DD_RECEIVED_MSG_TOO_LONG:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"A frame of size greater than DD_MAX_MSG_LEN items was read from fifo receive, and no EOFrame bitmask found. Aborting FIFO Receive readout.");
		break;
		case DD_RECEIVED_WRONG_TRANS_NUMBER:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"- motif : Transaction number of incoming frame is wrong (not in allowed limits [DD_MIN_TRANS_NUMBER,DD_MAX_TRANS_NUMBER]). This can mean that a corrupted frame have bypassed the software sanity checks, and is processed as if it was a good frame.");
		break;
		case DD_RING_NOISY:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Some noise occured on the ring. Frame trashed.");
		break;
		case DD_COPIED_NOT_READY:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The COPIED field of a given transaction (identified by its transaction/frame number) is not set to Copied OK. Either the transaction is still running or have failed.");
		break;
		case DD_TIMEOUT_FEC_IN_USE:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"A write method have not been successful ; the fec_in_use flag was always set high, so I haven't been able to write the frame in the FEC FIFO transmit.");
		break;
		case DD_DATA_CORRUPT_ON_WRITE:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"A write operation get a corrupted frame back (direct acknowledge).");
		break;
		case DD_NO_MORE_FREE_TRANSACTION_NUMBER:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"255 transactions are already in the wait queue of the write method ; impossible to add more because no more transaction number is available.");
		break;
		case DD_TOO_MUCH_STACKED_READ_REQUESTS:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Too much (more than DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS, value defined in file defines.h) transactions are already waiting for read access ; impossible to add more.");
		break;
		case DD_TIMEOUT_TRANS_NUMBER_MNGR_IN_USE:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The transaction number manager can not be accessed ; flag is always busy. Aborting ioctl() call.");
		break;
		case DD_FLAG_READ_COUNTER_UNCONSISTENT:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The variable used as flag/counter for stacking read requests is not consistent (<0 or >DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS). Software sanity problem, better unload/reload the driver.");
		break;
		case DD_TOO_LONG_FRAME_LENGTH:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The frame length extracted from data read in user space is inconsistent (greater than DD_MAX_MSG_LENGTH). Possibly, a wrong frame have been sent to me by a user. Aborting ioctl() call.");
		break;
		case DD_BAD_TRANSACTION_NUMBER_SENT_TO_READ:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The transaction number sent to the read method don't fit the limits [DD_MIN_TRANS_NUMBER,DD_MAX_TRANS_NUMBER]. Certainly an error from the user.");
		break;
		case DD_CANNOT_INSTALL_IRQ_MANAGER:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"I can not install the interrupt manager routine.");
		break;
		case DD_CANNOT_EXIT_FROM_IRQ_READ_DATA:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Too many messages are occuring too fast on the ring, I am trapped in the interrupt manager. Interrupts are disabled for the next 7 seconds, and will automatically be re-enabled after this delay.");
		break;
		case DD_ERROR_MODULE_IS_RESETTING:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"I can't perform any operations right now ; a system reset is on the run and has not ended yet.");
		break;
		case DD_RESET_BLOCKED_BY_WRITE:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Reset function can not succeed, because one or more remaining write tasks are inactives but not ended. Better unload/reload the driver.");
		break;
		case DD_RESET_BLOCKED_BY_READ:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Reset function can not succeed, because one or more remaining read tasks are inactives but not ended. Better unload/reload the driver.");
		break;
		case DD_INIT_TTCRX_WATCHDOG:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Watchdog was bypassed when waiting confirmation of ttcrx initialisation. Maybe your ttcrx has nevertheless been initialized, sometimes it takes a loooong time to confirm initialisation so this is more a warning rather than an error.");
		break;
		case DD_TOO_MANY_PCI_BA:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"TOO MANY base address registers (>6) defined for this PCI device. Check value of DD_NBR_OF_PCI_BASE_ADDRESSES in file defines.h.");
		break;
		case DD_TIMEOUT_READ_REQUEST_IN_USE:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The read method can not increment the read_tasks_waiting counter ; access flag is always busy. Aborting.");
		break;
		case DD_KS_CANNOT_READ_FROM_US_FOR_PLX_WRITE:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not read from the user space the values to write to the PLX. Software problem.");
		break;
		case DD_KS_CANNOT_READ_FROM_US_FOR_PLX_READ:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not read from the user space the parameters needed to perform a read operation on the PLX. Software problem.");
		break;
		case DD_KS_CANNOT_WRITE_TO_US_FOR_PLX_READ:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not write to the user space the values read from the PLX. Software problem.");
		break;
		case DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_WRITE:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not read from the user space the values to write to the FEC. Software problem.");
		break;
		case DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_READ:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not read from the user space the parameters needed to perform a read operation on the FEC. Software problem.");
		break;
		case DD_CANNOT_WRITE_VALUE_TO_US_FOR_FEC_READ:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not write to the user space the values read from the FEC. Software problem.");
		break;
		case DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not write to the user space the value of the requested PLX Base Address. Software problem.");
		break;
		case DD_FEC_CANNOT_READ_VALUE_FROM_US_FOR_RET_STATUS:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not read from the user space the values nedded to check the status of the transaction. Software problem.");
		break;
		case DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_RET_STATUS:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not write to the user space the value of the status of the transaction. Software problem.");
		break;
		case DD_ERROR_IRQS_ARE_INVALIDATED:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"IRQ's are currently disabled. This method can not be used when IRQ's are disabled.");
		break;
		case DD_KS_CANNOT_READ_FRAME_FROM_US_FOR_WRITE_FRAME:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not read from the user space the frame to write on the ring. Software problem.");
		break;
		case DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Write operation successful, but Kernel space can not write to the user space the value of the returned direct acknowledge frame.");
		break;
		case DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS2:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Write operation un-successful (direct acknowledge frame corrupted), but Kernel space can not write to the user space the value of the returned direct acknowledge frame.");
		break;
		case DD_KS_CANNOT_READ_FROM_US_FOR_READ_FRAME:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not read from the user space the informations nedded to execute the read_frame() method. Software problem.");
		break;
		case DD_KS_CANNOT_WRITE_TO_US_FOR_READ_FRAME:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not write to the user space the requested frame. Software problem.");
		break;
		case DD_KS_CANNOT_WRITE_TO_US_IN_HUB:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space is not allowed to write in the memory area you gave as argument to your ioctl() request. I need to be able to write there.");
		break;
		case DD_KS_CANNOT_READ_FROM_US_IN_HUB:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space is not allowed to read data from the memory area you gave as argument to your ioctl() request. I need to be able to read data from there.");
		break;
		case DD_MAIN_ERROR_MODULE_IS_RESETTING:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"I can't perform any operations right now ; a system reset is on the run and has not ended yet.");
		break;
		case DD_READ_FRAME_CORRUPTED:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"A READ request have not been fully successful ; a frame has been received but is corrupted.");
		break;
		case DD_INVALID_WRITE_AUTOTUNE_CMD:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"An invalid command code have been sent to the auto-tune-write-method function of the glue layer.");
		break;
		case DD_INVALID_READ_AUTOTUNE_CMD:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"An invalid command code have been sent to the auto-tune-read-method function of the glue layer.");
		break;
		case DD_FEC_CANNOT_READ_VALUE_FROM_US:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not read values from the user space. Software problem.");
		break;
		case DD_FEC_CANNOT_WRITE_VALUE_TO_US:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"Kernel space can not write values to the user space. Software problem.");
		break;
		case DD_WRITE_OPERATION_FAILED:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"A write operation failed ; no data at all was returned, not even a direct acknowledge returned frame.");
		break;
		case DD_EXIT_WRITE_METHOD_ON_UNSTABLE_CONDITION:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"System error. Analysis of received ack frame issued an unknown status. Please send a dump of frames and informations to fec-suport@ires.in2p3.fr.");
		break;
		case DD_CANNOT_READ_DATA:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"A READ request have not been successful at all ; exit from request on timeout.");
		break;
		case DD_EXIT_READ_METHOD_ON_UNSTABLE_CONDITION:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"System error. Analysis of received data frame issued an unknown status. Please send a dump of frames and informations to fec-suport@ires.in2p3.fr.");
		break;
		case DD_DATA_OVERFLOW:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"An incoming frame (direct ACK or data requested) have a size which overruns driver internals. Check file includes/defines.h for more specifications.");
		break;
		case DD_INVALID_REM_CMD:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"An invalid/unrecognized command has been sent to the ring error management function.");
		break;
		case DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The FIFO TRANSMIT RUNNING bit of FEC register STATUS0 is always on. You will certainly have to reset your FEC.");
		break;
		case DD_CANNOT_OPEN_FEC_DRIVER:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The FEC driver cannot be opened. Either the driver is not loaded, or your /dev/fecpmcxx file is has improper permissions.");
		break;
		case DD_CANNOT_CLOSE_FEC_DRIVER:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The FEC driver cannot be close. Either the driver is not loaded, or the driver USER_COUNT is not equal to zero (i.e., someone is still using the driver).");
		break;
		case SYSTEM_DRIVER_NOT_LOADED:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"You are making ioctl() attempts, but no driver is loaded/matches the file descriptor you use.");
		break;
		case DD_NO_FEC_BOARD_DETECTED:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),"The current FEC driver has not found any physical PCI FEC board on this system - Driver access is disabled.");
		break;
		default:
			snprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1)," The error code %d is not defined in the function dd_perror", param_code) ;
		break;
	}
	return DD_RETURN_OK;
}
