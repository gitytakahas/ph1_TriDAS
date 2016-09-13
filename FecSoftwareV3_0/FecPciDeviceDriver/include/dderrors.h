/*
   FileName : 		DDERRORS.H

   Content : 		Definition of error codes and mnemonics

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
#ifndef DDERRORS_H
#define DDERRORS_H


#include "datatypes.h"


#ifdef __cplusplus
extern "C" {
#endif



/*!
BEGIN_TAG
The function/command has been successfully executed.
END_TAG
*/
#define DD_NO_ERROR							DD_RETURN_OK


/*!
BEGIN_TAG
I was not able to raise the interlock management semaphore, when I entered the main ioctl() function. IOCTL() request have been rejected.
END_TAG
*/
#define DD_SIG_ON_IOCTL_SEM						1


/*!
BEGIN_TAG
IOCTL() destination (FEC | PLX | PCI) undefined. Looks like there is a problem with the magic numbers of the ioctl() calls defined in files pci_ioctl.h, plx_ioctl.h, fec_ioctl.
END_TAG
*/
#define DD_BAD_IOC_DESTINATION						2


/*!
BEGIN_TAG
Your OS does not provides software support for PCI. The kernel of your computer must be compiled with PCI support option activated.
END_TAG
*/
#define DD_NO_PCI_SOFTWARE_SUPPORT					3


/*!
BEGIN_TAG
Your computer does not appears to have, physically, a PCI bus.
END_TAG
*/
#define DD_NO_PCI_HARDWARE_SUPPORT					4


/*!
BEGIN_TAG
The PCI device identified by DEVICE_ID & VENDR_ID (values defined in file defines.h) has not been detected on the PCI bus of your computer.
END_TAG
*/
#define DD_PCI_DEVICE_NOT_FOUND						5


/*!
BEGIN_TAG
Error while reading the PCI COMMAND Register from the PCI board.
END_TAG
*/
#define DD_PCI_ERROR_READ_COMMAND_REG					6


/*!
BEGIN_TAG
Kernel space cannot write the previously read value COMMAND register of the PCI board to user space.
END_TAG
*/
#define DD_PCI_KS_CANNOT_WRITE_COMMAND_REG_TO_US			7



/*!
BEGIN_TAG
Kernel space cannot write the previously read value IRQ number of the PCI board to user space.
END_TAG
*/
#define DD_PCI_KS_CANNOT_WRITE_IRQ_NUMBER_TO_US				8



/*!
BEGIN_TAG
Kernel space cannot write the previously read value base addresses array to user space.
END_TAG
*/
#define DD_PCI_KS_CANNOT_WRITE_BA_ARRAY_TO_US				9


/*!
BEGIN_TAG
Kernel space cannot read the value of CONTROL register for the PCI board from user space.
END_TAG
*/
#define DD_PCI_KS_CANNOT_READ_CTRL_REGISTER_FROM_US			10


/*!
BEGIN_TAG
Error while writing in the CONTROL register of the PCI board.
END_TAG
*/
#define DD_PCI_ERROR_WRITE_CTRL_REGISTER				11



/*!
BEGIN_TAG
The ioctl() command has been recognized as a PCI command, but no action is associated to it.
END_TAG
*/
#define DD_PCI_COMMAND_NOT_ASSOCIATED					12


/*!
BEGIN_TAG
Remapping of PLX Base Addresse towards locale failed ; NULL pointer returned.
END_TAG
*/
#define DD_PLX_ERROR_REMAPPING_PLX_BA					13



/*!
BEGIN_TAG
The ioctl() command has been recognized as a PLX command, but no action is associated to it.
END_TAG
*/
#define DD_PLX_COMMAND_NOT_ASSOCIATED					14


/*!
BEGIN_TAG
The ioctl() command has been recognized as a FEC command, but no action is associated to it.
END_TAG
*/
#define DD_FEC_COMMAND_NOT_ASSOCIATED					15


/*!
BEGIN_TAG
The device driver can not register himself to the kernel ; may be a /dev/devicename problem.
END_TAG
*/
#define DD_ERROR_REGISTER_MODULE					16



/*!
BEGIN_TAG
A frame of size greater than DD_MAX_MSG_LEN items was read from fifo receive, and no EOFrame bitmask found. Aborting FIFO Receive readout.
END_TAG
*/
#define DD_RECEIVED_MSG_TOO_LONG					17



/*!
BEGIN_TAG
- motif : Transaction number of incoming frame is wrong (not in allowed limits [DD_MIN_TRANS_NUMBER,DD_MAX_TRANS_NUMBER]). This can mean that a corrupted frame have bypassed the software sanity checks, and is processed as if it was a good frame.
END_TAG
*/
#define DD_RECEIVED_WRONG_TRANS_NUMBER					18


/*!
BEGIN_TAG
Some noise occured on the ring. Frame trashed.
END_TAG
*/
#define DD_RING_NOISY							19


/*!
BEGIN_TAG
The COPIED field of a given transaction (identified by its transaction/frame number) is not set to Copied OK. Either the transaction is still running or have failed.
END_TAG
*/
#define DD_COPIED_NOT_READY						20


/*!
BEGIN_TAG
A write method have not been successful ; the fec_in_use flag was always set high, so I haven't been able to write the frame in the FEC FIFO transmit.
END_TAG
*/
#define DD_TIMEOUT_FEC_IN_USE						22



/*!
BEGIN_TAG
A write operation get a corrupted frame back (direct acknowledge).
END_TAG
*/
#define DD_DATA_CORRUPT_ON_WRITE					23


/*!
BEGIN_TAG
255 transactions are already in the wait queue of the write method ; impossible to add more because no more transaction number is available.
END_TAG
*/
#define DD_NO_MORE_FREE_TRANSACTION_NUMBER				25




/*!
BEGIN_TAG
Too much (more than DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS, value defined in file defines.h) transactions are already waiting for read access ; impossible to add more.
END_TAG
*/
#define DD_TOO_MUCH_STACKED_READ_REQUESTS				26


/*!
BEGIN_TAG
The transaction number manager can not be accessed ; flag is always busy. Aborting ioctl() call.
END_TAG
*/
#define DD_TIMEOUT_TRANS_NUMBER_MNGR_IN_USE				27


/*!
BEGIN_TAG
The variable used as flag/counter for stacking read requests is not consistent (<0 or >DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS). Software sanity problem, better unload/reload the driver.
END_TAG
*/
#define DD_FLAG_READ_COUNTER_UNCONSISTENT				28



/*!
BEGIN_TAG
The frame length extracted from data read in user space is inconsistent (greater than DD_MAX_MSG_LENGTH). Possibly, a wrong frame have been sent to me by a user. Aborting ioctl() call.
END_TAG
*/
#define DD_TOO_LONG_FRAME_LENGTH					29


/*!
BEGIN_TAG
The transaction number sent to the read method don't fit the limits [DD_MIN_TRANS_NUMBER,DD_MAX_TRANS_NUMBER]. Certainly an error from the user.
END_TAG
*/
#define DD_BAD_TRANSACTION_NUMBER_SENT_TO_READ				30



/*!
BEGIN_TAG
I can not install the interrupt manager routine.
END_TAG
*/
#define DD_CANNOT_INSTALL_IRQ_MANAGER					31


/*!
BEGIN_TAG
Too many messages are occuring too fast on the ring, I am trapped in the interrupt manager. Interrupts are disabled for the next 7 seconds, and will automatically be re-enabled after this delay.
END_TAG
*/
#define DD_CANNOT_EXIT_FROM_IRQ_READ_DATA				32



/*!
BEGIN_TAG
I can't perform any operations right now ; a system reset is on the run and has not ended yet.
END_TAG
*/
#define DD_ERROR_MODULE_IS_RESETTING					33


/*!
BEGIN_TAG
Reset function can not succeed, because one or more remaining write tasks are inactives but not ended. Better unload/reload the driver.
END_TAG
*/
#define DD_RESET_BLOCKED_BY_WRITE					34


/*!
BEGIN_TAG
Reset function can not succeed, because one or more remaining read tasks are inactives but not ended. Better unload/reload the driver.
END_TAG
*/
#define DD_RESET_BLOCKED_BY_READ					35



/*!
BEGIN_TAG
Watchdog was bypassed when waiting confirmation of ttcrx initialisation. Maybe your ttcrx has nevertheless been initialized, sometimes it takes a loooong time to confirm initialisation so this is more a warning rather than an error.
END_TAG
*/
#define DD_INIT_TTCRX_WATCHDOG						36


/*!
BEGIN_TAG
TOO MANY base address registers (>6) defined for this PCI device. Check value of DD_NBR_OF_PCI_BASE_ADDRESSES in file defines.h.
END_TAG
*/
#define DD_TOO_MANY_PCI_BA						37


/*!
BEGIN_TAG
The read method can not increment the read_tasks_waiting counter ; access flag is always busy. Aborting.
END_TAG
*/
#define DD_TIMEOUT_READ_REQUEST_IN_USE					38



/*!
BEGIN_TAG
Kernel space can not read from the user space the values to write to the PLX. Software problem.
END_TAG
*/
#define DD_KS_CANNOT_READ_FROM_US_FOR_PLX_WRITE				39


/*!
BEGIN_TAG
Kernel space can not read from the user space the parameters needed to perform a read operation on the PLX. Software problem.
END_TAG
*/
#define DD_KS_CANNOT_READ_FROM_US_FOR_PLX_READ				40


/*!
BEGIN_TAG
Kernel space can not write to the user space the values read from the PLX. Software problem.
END_TAG
*/
#define DD_KS_CANNOT_WRITE_TO_US_FOR_PLX_READ				41


/*!
BEGIN_TAG
Kernel space can not read from the user space the values to write to the FEC. Software problem.
END_TAG
*/
#define DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_WRITE			42


/*!
BEGIN_TAG
Kernel space can not read from the user space the parameters needed to perform a read operation on the FEC. Software problem.
END_TAG
*/
#define DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_READ			43


/*!
BEGIN_TAG
Kernel space can not write to the user space the values read from the FEC. Software problem.
END_TAG
*/
#define DD_CANNOT_WRITE_VALUE_TO_US_FOR_FEC_READ			44


/*!
BEGIN_TAG
Kernel space can not write to the user space the value of the requested PLX Base Address. Software problem.
END_TAG
*/
#define DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA			45


/*!
BEGIN_TAG
Kernel space can not read from the user space the values nedded to check the status of the transaction. Software problem.
END_TAG
*/
#define DD_FEC_CANNOT_READ_VALUE_FROM_US_FOR_RET_STATUS			46


/*!
BEGIN_TAG
Kernel space can not write to the user space the value of the status of the transaction. Software problem.
END_TAG
*/
#define DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_RET_STATUS			47


/*!
BEGIN_TAG
IRQ's are currently disabled. This method can not be used when IRQ's are disabled.
END_TAG
*/
#define DD_ERROR_IRQS_ARE_INVALIDATED					48


/*!
BEGIN_TAG
Kernel space can not read from the user space the frame to write on the ring. Software problem.
END_TAG
*/
#define DD_KS_CANNOT_READ_FRAME_FROM_US_FOR_WRITE_FRAME			49




/*!
BEGIN_TAG
Write operation successful, but Kernel space can not write to the user space the value of the returned direct acknowledge frame.
END_TAG
*/
#define DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1			50


/*!
BEGIN_TAG
Write operation un-successful (direct acknowledge frame corrupted), but Kernel space can not write to the user space the value of the returned direct acknowledge frame.
END_TAG
*/
#define DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS2			51


/*!
BEGIN_TAG
Kernel space can not read from the user space the informations nedded to execute the read_frame() method. Software problem.
END_TAG
*/
#define DD_KS_CANNOT_READ_FROM_US_FOR_READ_FRAME			52


/*!
BEGIN_TAG
Kernel space can not write to the user space the requested frame. Software problem.
END_TAG
*/
#define DD_KS_CANNOT_WRITE_TO_US_FOR_READ_FRAME				53




/*!
BEGIN_TAG
Kernel space is not allowed to write in the memory area you gave as argument to your ioctl() request. I need to be able to write there.
END_TAG
*/
#define DD_KS_CANNOT_WRITE_TO_US_IN_HUB					54


/*!
BEGIN_TAG
Kernel space is not allowed to read data from the memory area you gave as argument to your ioctl() request. I need to be able to read data from there.
END_TAG
*/
#define DD_KS_CANNOT_READ_FROM_US_IN_HUB				55



/*!
BEGIN_TAG
I can't perform any operations right now ; a system reset is on the run and has not ended yet.
END_TAG
*/
#define DD_MAIN_ERROR_MODULE_IS_RESETTING				56



/*!
BEGIN_TAG
A READ request have not been fully successful ; a frame has been received but is corrupted.
END_TAG
*/
#define DD_READ_FRAME_CORRUPTED						57


/*!
BEGIN_TAG
An invalid command code have been sent to the auto-tune-write-method function of the glue layer.
END_TAG
*/
#define DD_INVALID_WRITE_AUTOTUNE_CMD					58


/*!
BEGIN_TAG
An invalid command code have been sent to the auto-tune-read-method function of the glue layer.
END_TAG
*/
#define DD_INVALID_READ_AUTOTUNE_CMD					59


/*!
BEGIN_TAG
Kernel space can not read values from the user space. Software problem.
END_TAG
*/
#define DD_FEC_CANNOT_READ_VALUE_FROM_US				60


/*!
BEGIN_TAG
Kernel space can not write values to the user space. Software problem.
END_TAG
*/
#define DD_FEC_CANNOT_WRITE_VALUE_TO_US					61




/*!
BEGIN_TAG
A write operation failed ; no data at all was returned, not even a direct acknowledge returned frame.
END_TAG
*/
#define DD_WRITE_OPERATION_FAILED					64

/*!
BEGIN_TAG
System error. Analysis of received ack frame issued an unknown status. Please send a dump of frames and informations to fec-suport@ires.in2p3.fr.
END_TAG
*/
#define DD_EXIT_WRITE_METHOD_ON_UNSTABLE_CONDITION			65


/*!
BEGIN_TAG
A READ request have not been successful at all ; exit from request on timeout.
END_TAG
*/
#define DD_CANNOT_READ_DATA						68


/*!
BEGIN_TAG
System error. Analysis of received data frame issued an unknown status. Please send a dump of frames and informations to fec-suport@ires.in2p3.fr.
END_TAG
*/
#define DD_EXIT_READ_METHOD_ON_UNSTABLE_CONDITION			69



/*!
BEGIN_TAG
An incoming frame (direct ACK or data requested) have a size which overruns driver internals. Check file includes/defines.h for more specifications.
END_TAG
*/
#define DD_DATA_OVERFLOW						70



/*!
BEGIN_TAG
An invalid/unrecognized command has been sent to the ring error management function.
END_TAG
*/
#define DD_INVALID_REM_CMD						71


/*!
BEGIN_TAG
The FIFO TRANSMIT RUNNING bit of FEC register STATUS0 is always on. You will certainly have to reset your FEC.
END_TAG
*/
#define DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON					72




/*!
BEGIN_TAG
The FEC driver cannot be opened. Either the driver is not loaded, or your /dev/fecpmcxx file is has improper permissions.
END_TAG
*/
#define DD_CANNOT_OPEN_FEC_DRIVER						102



/*!
BEGIN_TAG
The FEC driver cannot be close. Either the driver is not loaded, or the driver USER_COUNT is not equal to zero (i.e., someone is still using the driver).
END_TAG
*/
#define DD_CANNOT_CLOSE_FEC_DRIVER						103


/*!
BEGIN_TAG
You are making ioctl() attempts, but no driver is loaded/matches the file descriptor you use.
END_TAG
*/
#define SYSTEM_DRIVER_NOT_LOADED						104

/*!
BEGIN_TAG
The current FEC driver has not found any physical PCI FEC board on this system - Driver access is disabled.
END_TAG
*/
#define DD_NO_FEC_BOARD_DETECTED						105


#ifdef __cplusplus
}
#endif


/* End Block - avoid multiple inclusions of this file */
#endif



