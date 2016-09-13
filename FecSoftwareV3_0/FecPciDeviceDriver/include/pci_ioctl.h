/*
   FileName : 		PCI_IOCTL.H

   Content : 		IOCTL magic number choosen for PCI operations
			IOCTL commands requested by needed PCI operations
			One value used for PCI Bus initialisation

   Used in : 		DRIVER

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


/* Begin block - avoid multiple inclusions of this file */
#ifndef DD_PCIIOCTL_H
#define DD_PCIIOCTL_H


#ifdef __cplusplus
extern "C" {
#endif

#include "defines.h"

/*!
choice of a magic number used for PLX IOCTL's commands
*/
#define DD_IOC_MAGIC_PCI 'k'


/*!
<b>Destination function : dd_0003_ioc_check_if_system_is_pci_compliant</b>
- Job
	- Ask Kernel to Check if system is PCI compliant ; no arg passed
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_NO_PCI_SOFTWARE_SUPPORT
		- DD_NO_PCI_HARDWARE_SUPPORT
*/
#define DD_IOC_CHECK_IF_SYSTEM_IS_PCI _IO(DD_IOC_MAGIC_PCI,0)




/*!
<b>Destination function : dd_0004_ioc_find_pci_card</b>
- Job
	- Ask Kernel to Check if PCI board identified by ID_DEVICE and ID_VENDOR exists
	on the system ; no arg passed
- Error Management & Values returned
	- Operation successful
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_DEVICE_NOT_FOUND
*/
#define DD_IOC_FIND_PCI_CARD _IO(DD_IOC_MAGIC_PCI,1)




/*!
<b>Destination function : dd_0007_ioc_get_pci_card_command_reg</b>
- Job
	- Send from Kernel to user space the value (unsigned 16) of command register
	- The board (previously) detected is qualified by VENDOR_ID and DEVICE_ID
	- Rem : this value comes from a direct hardware read
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_ERROR_READ_COMMAND_REG
		- DD_PCI_KS_CANNOT_WRITE_COMMAND_REG_TO_US
*/
#define DD_IOC_GET_PCI_COMMAND_REG _IOR(DD_IOC_MAGIC_PCI,2,DD_TYPE_PCI_COMMAND_REG)





/*!
<b>Destination function : dd_0018_ioc_get_pci_card_irq_number</b>
- Job
	- Send from Kernel to user space the value (unsigned 8) of IRQ
	- The board (previously) detected is qualified by VENDOR_ID and DEVICE_ID
	- Rem : this value comes from an indirect hardware read, in system info
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_KS_CANNOT_WRITE_IRQ_NUMBER_TO_US
*/
#define DD_IOC_GET_PCI_IRQ_NUMBER _IOR(DD_IOC_MAGIC_PCI,3,DD_TYPE_PCI_IRQ_NUMBER)






/*!
<b>Destination function : dd_0019_ioc_get_pci_card_base_addresses_and_mask</b>
- Job
	- Send from Kernel to user space the value (array of unsigned 16) of base addresses
	current configurations
	- The board (previously) detected is qualified by VENDOR_ID and DEVICE_ID
	- Rem : this value comes from an indirect hardware read, in system info
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_0
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_1
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_2
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_3
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_4
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_5
		- DD_PCI_ERROR_WRITE_BASE_ADDRESS_FAKE_CONFIG
		- DD_PCI_ERROR_WRITE_BASE_ADDRESS_FAKE_CONFIG_0
		- DD_PCI_ERROR_WRITE_BASE_ADDRESS_FAKE_CONFIG_1
		- DD_PCI_ERROR_WRITE_BASE_ADDRESS_FAKE_CONFIG_2
		- DD_PCI_ERROR_WRITE_BASE_ADDRESS_FAKE_CONFIG_3
		- DD_PCI_ERROR_WRITE_BASE_ADDRESS_FAKE_CONFIG_4
		- DD_PCI_ERROR_WRITE_BASE_ADDRESS_FAKE_CONFIG_5
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_AFTER_FAKE
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_AFTER_FAKE_0
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_AFTER_FAKE_1
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_AFTER_FAKE_2
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_AFTER_FAKE_3
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_AFTER_FAKE_4
		- DD_PCI_ERROR_READ_BASE_ADDRESS_CONFIG_AFTER_FAKE_5
		- DD_PCI_ERROR_RESTORE_BASE_ADDRESS_CONFIG
		- DD_PCI_ERROR_RESTORE_BASE_ADDRESS_CONFIG_0
		- DD_PCI_ERROR_RESTORE_BASE_ADDRESS_CONFIG_1
		- DD_PCI_ERROR_RESTORE_BASE_ADDRESS_CONFIG_2
		- DD_PCI_ERROR_RESTORE_BASE_ADDRESS_CONFIG_3
		- DD_PCI_ERROR_RESTORE_BASE_ADDRESS_CONFIG_4
		- DD_PCI_ERROR_RESTORE_BASE_ADDRESS_CONFIG_5
		- DD_PCI_KS_CANNOT_WRITE_BA_ARRAY_TO_US

*/
#define DD_IOC_GET_PCI_BASE_ADDRESSES_AND_MASKS _IOR(DD_IOC_MAGIC_PCI,4,dd_device_base_addresses_array)








/*!
<b>Destination function : dd_0101_ioc_set_pci_card_command_reg</b>
- Job
	- User space send to kernel the value (unsigned 16) to write to PCI board control
	register.
	- The board (previously) detected is qualified by VENDOR_ID and DEVICE_ID
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_KS_CANNOT_READ_CTRL_REGISTER_FROM_US
		- DD_PCI_ERROR_WRITE_CTRL_REGISTER
*/
#define DD_IOC_SET_PCI_COMMAND_REG _IOW(DD_IOC_MAGIC_PCI,5,DD_TYPE_PCI_COMMAND_REG)


#ifdef __cplusplus
}
#endif



/* End block - avoid multiple inclusions of this file */
#endif




