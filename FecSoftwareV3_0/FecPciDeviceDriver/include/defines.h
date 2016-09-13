/*
   FileName : 		DEFINES.H

   Content : 		Definition of compilation options and values used in device driver

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
#ifndef DD_DEFINES_H
#define DD_DEFINES_H


#ifdef __cplusplus
extern "C" {
#endif


//#include "version.h"
//#include "driver_conf.h"

#define FEC_DRIVER_VERSION      "Unified-6.0"


	/***********************************************************
				FRAMES SPYING
	***********************************************************/
	/*!
	Uncomment next line if you want to dump on-the-fly processed frames informations
	(i.e. dump frames content, etc...) in file /var/log/messages
	Rem : This debug define overcomes DD_OPTIMIZED_DRIVER silent mode.
	*/
	#ifdef DD_SPY_DEBUG
		#undef DD_SPY_DEBUG
	#endif
	//#define DD_SPY_DEBUG



	/***********************************************************
				DATA FRAMES SIZE
	***********************************************************/
	/*!
	!!!!!!!!   WARNING   !!!!!!!!!!
	DO NOT SET A SIZE GREATER THAN 196 for DD_USER_MAX_MSG_LENGTH
	or System ioctl() calls will fail.
	*/
	/*
	Rem : DD_USER_MAX_MSG_LENGTH is always used, independently of the fifo's elements size
	*/
	#define DD_USER_MAX_MSG_LENGTH                                  196

	/*
	Now, defines the physical properties of the FEC fifos
	*/
	#define DD_MAX_MSG_LENGTH_32			(int)((DD_USER_MAX_MSG_LENGTH/4) + 1)
	#define DD_MAX_MSG_LENGTH_16			DD_USER_MAX_MSG_LENGTH



	/***********************************************************
			FRAMES AUTO-TERMINATION
	***********************************************************/
	/*!
	If a frame arrives to the driver for a write operation, and if the last
	word of the frame is not OR masked with 0x8000, do we have to auto-mask
	this last word? If yes, uncomment the following line.
	*/
	#ifdef DD_SET_AUTO_EOF_BIT
		#undef DD_SET_AUTO_EOF_BIT
	#endif
	#define DD_SET_AUTO_EOF_BIT



	/***********************************************************
			BUILT-IN IRQ MANAGER
	***********************************************************/
	/*!
	comment next line for NOT installing an interrupt manager
	this option should only be used for debugging purposes
	*/
        //#define DD_ENABLE_IRQMANAGER




	/***********************************************************
			SPECIFIC ERROR MANAGEMENT
	***********************************************************/

	/*!
	returned when operation performed successfully
	*/
	#define DD_RETURN_OK (DD_TYPE_ERROR)		0

	/*!
	max allowed length of decoded error message used by error handler
	Rem : this value is used only in file dd_perror.c
	*/
	#define DD_MAX_DECODED_ERROR_MSG_LENGTH		512





	/***********************************************************
					SPECIFIC PCI
	***********************************************************/

	/*!
	Max number of base addresses existing on a PCI board.
	Rem :This value is declared as common because the common data storage
	structure struct_info, defined in datatypes.h, uses this value for
	dimensioning arrays.
	*/
	#define DD_NBR_OF_PCI_BASE_ADDRESSES		6

	/*!
	How many items of information will we store for one base address ?
	Rem :This value is declared as common because the common data storage
	structure struct_info, defined in datatypes.h, uses this value for
	dimensioning arrays.
	*/
	#define DD_NBR_OF_ITEM_BY_BASE_ADDRESS		4


	/*!
	default value to write to the PCI control register in order to enable
	address space, iospace, etc...
	*/
	#define DD_PCI_CTRL_CONFIG				0x0157

	/***********************************************************
					SPECIFIC PLX
	***********************************************************/

	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
	#define DD_PLX_LAS0RR_VALUE				0xfff00000

	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
	#define DD_PLX_LAS0BA_VALUE				0x00000001


	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
	#define DD_PLX_MARBR_VALUE				0x01200000

	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
	#define DD_PLX_BIGEND_VALUE				0x00000000

	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
//	#define DD_PLX_LBRD0_VALUE				0x4303014b
	#define DD_PLX_LBRD0_VALUE				0x4303010b

	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
	#define DD_PLX_CTRL_VALUE				0x9801767e


	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
	#define DD_PLX_CTRL_RESET_BIT			0x40000000


	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
	#define DD_PLX_IRQ_ENABLED_VALUE			0x00010900

	/*!
	Typical configuration values for the PLX bridge registers.
	Values are issued from the PLX 9080 documentation.
	These values are similar to the HardCoded ones, loaded after a reset.
	*/
	#define DD_PLX_IRQ_DISABLED_VALUE			0x00010000

	/*!
	Typical offset values for the PLX bridge registers.
	offsets are relatives to Base Address 0, PCIBAR0.
	BAR0 is the PCI Base Address Register for Memory Access to Local,
	Runtime and DMA Registers
	*/
	#define DD_PLX9080_LAS0RR_OFFSET			0x00


	/*!
	Typical offset values for the PLX bridge registers.
	offsets are relatives to Base Address 0, PCIBAR0.
	BAR0 is the PCI Base Address Register for Memory Access to Local,
	Runtime and DMA Registers
	*/
	#define DD_PLX9080_LAS0BA_OFFSET			0x04


	/*!
	Typical offset values for the PLX bridge registers.
	offsets are relatives to Base Address 0, PCIBAR0.
	BAR0 is the PCI Base Address Register for Memory Access to Local,
	Runtime and DMA Registers
	*/
	#define DD_PLX9080_MARBR_OFFSET			0x08


	/*!
	Typical offset values for the PLX bridge registers.
	offsets are relatives to Base Address 0, PCIBAR0.
	BAR0 is the PCI Base Address Register for Memory Access to Local,
	Runtime and DMA Registers
	*/
	#define DD_PLX9080_BIGEND_OFFSET			0x0c


	/*!
	Typical offset values for the PLX bridge registers.
	offsets are relatives to Base Address 0, PCIBAR0.
	BAR0 is the PCI Base Address Register for Memory Access to Local,
	Runtime and DMA Registers
	*/
	#define DD_PLX9080_LBRD0_OFFSET			0x18


	/*!
	Typical offset values for the PLX bridge registers.
	offsets are relatives to Base Address 0, PCIBAR0.
	BAR0 is the PCI Base Address Register for Memory Access to Local,
	Runtime and DMA Registers
	*/
	#define DD_PLX9080_CTRL_OFFSET			0x6c


	/*!
	Typical offset values for the PLX bridge registers.
	offsets are relatives to Base Address 0, PCIBAR0.
	BAR0 is the PCI Base Address Register for Memory Access to Local,
	Runtime and DMA Registers
	*/
	#define DD_PLX9080_IRQREG_OFFSET			0x68


	/*!
	size of the memory area to remap for PLX base address 0
	Rem : value issued from PLX9080 / FEC documentation.
	*/
	#define DD_BA0_SIZE_FOR_REMAP				0x1000


	/*!
	size of the memory area to remap for PLX base address 1
	Rem : value issued from PLX9080 / FEC documentation.
	*/
	#define DD_BA1_SIZE_FOR_REMAP				0x0000


	/*!
	size of the memory area to remap for PLX base address 2
	Rem : value issued from PLX9080 / FEC documentation.
	*/
	#define DD_BA2_SIZE_FOR_REMAP_EV0			0x4000
	#define DD_BA2_SIZE_FOR_REMAP_OV0			0x4000
	#define DD_BA2_SIZE_FOR_REMAP_EV1			0x2B
	#define DD_BA2_SIZE_FOR_REMAP_OV1			0x2B


	/*!
	size of the memory area to remap for PLX base address 3
	Rem : value issued from PLX9080 / FEC documentation.
	*/
	#define DD_BA3_SIZE_FOR_REMAP				0x0000


	/*!
	size of the memory area to remap for PLX base address 4
	Rem : value issued from PLX9080 / FEC documentation.
	*/
	#define DD_BA4_SIZE_FOR_REMAP				0x0000


	/*!
	size of the memory area to remap for PLX base address 5
	Rem : value issued from PLX9080 / FEC documentation.
	*/
	#define DD_BA5_SIZE_FOR_REMAP				0x0000


	/*!
	size of the memory area to remap for PLX base address 6
	Rem : value issued from PLX9080 / FEC documentation.
	*/
	#define DD_BA6_SIZE_FOR_REMAP				0x0000


	/***********************************************************
					SPECIFIC FEC
	***********************************************************/

	/*!
	how many elements do we need for operating (R/W) on one FEC register ?
	Rem :This value is declared as common because the common data storage
	structure struct_info, defined in datatypes.h, uses this value for
	dimensioning arrays.
	*/
	#define DD_FEC_ARRAY_SIZE				2

	/*!
	Typical offset values for the FEC registers.
	offsets are relatives to Base Address 2 of PLX bridge, PCIBAR2.
	*/
	#define DD_FEC_CTRL0_OFFSET    			0x0000

	/*!
	Typical offset values for the FEC registers.
	offsets are relatives to Base Address 2 of PLX bridge, PCIBAR2.
	*/
	#define DD_FEC_CTRL1_OFFSET    			0x0004

	/*!
	Typical offset values for the FEC registers.
	offsets are relatives to Base Address 2 of PLX bridge, PCIBAR2.
	*/
	#define DD_FEC_STAT0_OFFSET    			0x0008

	/*!
	Typical offset values for the FEC registers.
	offsets are relatives to Base Address 2 of PLX bridge, PCIBAR2.
	*/
	#define DD_FEC_STAT1_OFFSET    			0x000c


	/* Field added 17/01/2005 - exists only on 32 bits FECs */
	#define DD_FEC_FIRMWARE_VERSION_OFFSET			0x0010


	/* Field added 17/01/2005 - exists only on 32 bits FECs */
	//#define DD_FEC_FIRMWARE_DATE_OFFSET			0x0014
	
	
	/*!
	Typical offset values for the FEC registers.
	offsets are relatives to Base Address 2 of PLX bridge, PCIBAR2.
	*/
	#define DD_FEC_FIFOTRA_OFFSET			0x1000

	#define DD_FEC_FIFOTRA_OFFSET_EV0		0x1000
	#define DD_FEC_FIFOTRA_OFFSET_OV0		0x1000
	#define DD_FEC_FIFOTRA_OFFSET_EV1		0x20
	#define DD_FEC_FIFOTRA_OFFSET_OV1		0x20

	/*!
	Typical offset values for the FEC registers.
	offsets are relatives to Base Address 2 of PLX bridge, PCIBAR2.
	*/
	#define DD_FEC_FIFORET_OFFSET			0x2000

	#define DD_FEC_FIFORET_OFFSET_EV0		0x2000
	#define DD_FEC_FIFORET_OFFSET_OV0		0x2000
	#define DD_FEC_FIFORET_OFFSET_EV1		0x24
	#define DD_FEC_FIFORET_OFFSET_OV1		0x24

	/*!
	Typical offset values for the FEC registers.
	offsets are relatives to Base Address 2 of PLX bridge, PCIBAR2.
	*/
	#define DD_FEC_FIFOREC_OFFSET			0x3000

	#define DD_FEC_FIFOREC_OFFSET_EV0		0x3000
	#define DD_FEC_FIFOREC_OFFSET_OV0		0x3000
	#define DD_FEC_FIFOREC_OFFSET_EV1		0x28
	#define DD_FEC_FIFOREC_OFFSET_OV1		0x28



	/*
	Typical values/bitmasks for the FEC registers.
	Values are issued from the FEC documentation.
	*/

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_TRA_RUN					0x0001

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_REC_RUN					0x0002

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V0
		#define DD_FEC_REC_HALF_F			0x0004
	//#endif

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_REC_FULL					0x0008

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_REC_EMPTY				0x0010

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V0
		#define DD_FEC_RET_HALF_F			0x0020
	//#endif

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_RET_FULL					0x0040

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_RET_EMPTY				0x0080

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V0
		#define DD_FEC_TRA_HALF_F			0x0100
	//#endif

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_TRA_FULL					0x0200

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_TRA_EMPTY				0x0400

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_LINK_INITIALIZED				0x0800

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_PENDING_IRQ				0x1000

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_DATA_TO_FEC				0x2000

	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	/*!
	Typical values for FEC STATUS 0 register, issued from FEC documentation
	*/
	#define DD_FEC_TTCRX_READY				0x4000




	/*!
	Typical values for FEC STATUS 1 register, issued from FEC documentation
	*/
	#define DD_FEC_ILL_DATA				0x0001

	/*!
	Typical values for FEC STATUS 1 register, issued from FEC documentation
	*/
	#define DD_FEC_ILL_SEQ				0x0002

	/*!
	Typical values for FEC STATUS 1 register, issued from FEC documentation
	*/
	#define DD_FEC_CRC_ERROR				0x0004

	/*!
	Typical values for FEC STATUS 1 register, issued from FEC documentation
	*/
	#define DD_FEC_DATA_COPIED				0x0008

	/*!
	Typical values for FEC STATUS 1 register, issued from FEC documentation
	*/
	#define DD_FEC_ADDR_SEEN				0x0010

	/*!
	Typical values for FEC STATUS 1 register, issued from FEC documentation
	*/
	#define DD_FEC_ERROR				0x0020



	/*!
	Typical values for FEC STATUS 1 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V1
		#define DD_FEC_TIMEOUT				0x0040
	//#endif

	/*!
	Typical values for FEC STATUS 1 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V1
		#define DD_FEC_CLOCK_ERROR			0x0080
	//#endif







	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	#define DD_FEC_ENABLE_FEC				0x0001

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	#define DD_FEC_SEND					0x0002

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V0
		#define DD_FEC_RESEND				0x0004
	//#else
		//#ifdef IS_FECREGISTERS_V1
			#define DD_FEC_XTAL_CLOCK		0x0004
		//#endif
	//#endif

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	#define DD_FEC_SEL_SER_OUT				0x0008

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	#define DD_FEC_SEL_SER_IN				0x0010


	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V0
		#define DD_FEC_SEL_IRQ_MODE			0x0020
	//#endif


	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	#define DD_FEC_RESET_TTCRX				0x0040

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V1
		#define DD_FEC_POL					0x0080
	//#endif

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V1
		#define DD_FEC_DISABLE_RECEIVE				0x0100
	//#endif

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V1
		#define DD_FEC_RESET_LINK_B				0x0200
	//#endif

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	#define DD_FEC_RESET_OUT				0x8000

	/*!
	Typical values for FEC CONTROL 0 register, issued from FEC documentation
	*/
	/* NOTE : TO DELETE AS SOON AS A NEWER VERSION OF THE DEBUGGER WILL BE RELEASED */
	#define DD_FEC_CTRL0_INIT_TTCRX				0x0000


	/*!
	Typical values for FEC CONTROL 1 register, issued from FEC documentation
	*/
	#define DD_FEC_CLEAR_IRQ				0x0001

	/*!
	Typical values for FEC CONTROL 1 register, issued from FEC documentation
	*/
	#define DD_FEC_CLEAR_ERRORS				0x0002

	/*!
	Typical values for FEC CONTROL 1 register, issued from FEC documentation
	*/
	//#ifdef IS_FECREGISTERS_V1
		#define DD_FEC_RELEASE_FEC				0x0004
	//#endif



	/***********************************************************
				SPECIFIC IOCTL
	***********************************************************/
	/*!
	When an IOCTL command is decoded, allows to know who is addressed
	between generic PCI, PLX or FEC
	*/
	#define DD_IOC_COMMAND_KIND_PCI			1

	/*!
	When an IOCTL command is decoded, allows to know who is addressed
	between generic PCI, PLX or FEC
	*/
	#define DD_IOC_COMMAND_KIND_PLX			2

	/*!
	When an IOCTL command is decoded, allows to know who is addressed
	between generic PCI, PLX or FEC
	*/
	#define DD_IOC_COMMAND_KIND_FEC			3



	/***********************************************************
				DRIVER INTERNALS
	***********************************************************/

	/*--------------------------------------------------------------
			system constraints/ driver load
	---------------------------------------------------------------*/
	/*!
	lowest file descriptor number that can be returned when registering the
	module with system function.
	*/
	#define DD_REGISTER_MODULE_OK_LIMIT				0


	/*!
	Lowest value that can be returned when successfully un-registering the
	module with system function.
	REM : VERY STRANGE FOR A RETURNED ERROR VALUE ; MAYBE THIS IS A MISTAKE
	FROM ME? TO CHECK ...
	*/
	#define DD_UNREGISTER_MODULE_OK_LIMIT				0


	/*!
	major number that will be given to function if auto-assignation is used
	*/
	#define DD_AUTO_MAJOR						0


	/*--------------------------------------------------------------
			memory management
	---------------------------------------------------------------*/
	/*!
	value returned by system function __copy_to_user if copy is successful
	*/
	#define DD_UUCOPY_TO_USER_OK					0


	/*!
	value returned by system function __copy_from_user if copy is successful
	*/
	#define DD_UUCOPY_FROM_USER_OK					0


	/*!
	value returned by memory_check system function if destination is well
	readeable or writeable in case of (respectively) read or write command
	to perform.
	*/
	#define DD_MEMORY_ACCESS_OK					1



	/*--------------------------------------------------------------
			PCI management
	---------------------------------------------------------------*/
	/*!
	lowest value that can be returned when successfully checking if a PCI
	device is present.
	REM : VERY STRANGE FOR A RETURNED ERROR VALUE ; MAYBE THIS IS A MISTAKE
	FROM ME? TO CHECK ...
	*/
	#define DD_LIMIT_PCI_IS_PRESENT					0


	/*--------------------------------------------------------------
			PLX management
	---------------------------------------------------------------*/
	/*!
	How many elements do we need for operating (R/W) on one base address
	of the PLX ? Two, the offset and the value.
	Rem :This value is declared as common because the common data storage
	structure struct_info, defined in datatypes.h, uses this value for
	dimensioning arrays.
	*/
	#define DD_PLX_ARRAY_SIZE				2


	/*!
	first element of the array plx_offset_and_value is the offset added
	to the PLX base address BAR0
	*/
	#define DD_PLX_OFFSET_INDEX				0


	/*!
	second element is the value to read or write to/from the PLX
	*/
	#define DD_PLX_VALUE_INDEX				1


	/*!
	defines used in function dd_get_base_address function, passed
	as a parameter for indexing PLX array to retrieve the corresponding
	stored base address value
	*/
	#define DD_REQUEST_FOR_BASE_ADDRESS0			0


	/*!
	defines used in function dd_get_base_address function, passed
	as a parameter for indexing PLX array to retrieve the corresponding
	stored base address value
	*/
	#define DD_REQUEST_FOR_BASE_ADDRESS1			1


	/*!
	defines used in function dd_get_base_address function, passed
	as a parameter for indexing PLX array to retrieve the corresponding
	stored base address value
	*/
	#define DD_REQUEST_FOR_BASE_ADDRESS2			2


	/*!
	defines used in function dd_get_base_address function, passed
	as a parameter for indexing PLX array to retrieve the corresponding
	stored base address value
	*/
	#define DD_REQUEST_FOR_BASE_ADDRESS3			3


	/*!
	Specific PLX ; in the succession of base addresses array, which one
	corresponds to what ?
	First base address is used to configure the PLX bridge (Base Address 0).
	*/
	#define DD_PLX_COMMAND_INDEX				0


	/*!
	Third base address is used to access the FEC registers (Base Address 2)
	*/
	#define DD_PLX_FEC_INDEX				2



	/*--------------------------------------------------------------
			FEC management
	---------------------------------------------------------------*/
	/*!
	first element of field array fec_offset_and_value is the offset added
	to the PLX base address BAR2 (i.e. FEC) in order to address the targeted
	FEC register.
	*/
	#define DD_FEC_OFFSET_INDEX				0


	/*!
	second element of the array is the value to read or write to/from the FEC
	*/
	#define DD_FEC_VALUE_INDEX				1


	/*!
	watchdog limit while waiting (in a loop) that ttcrx is initialised.
	*/
	#define DD_FEC_TTCRX_INIT_LOOP_WATCHDOG				10


	/*!
	value used by sleep_on_timeout while waiting (in a loop) that ttcrx is initialised.
	*/
	#define DD_INIT_TTCRX_INLOOP_DELAY				20

	
	/*--------------------------------------------------------------
			Frames properties
	---------------------------------------------------------------*/
	/*!
	minimum allowed transaction number on the CCU ring
	*/
	#define DD_MIN_TRANS_NUMBER					1

	/*!
	maximum allowed transaction number on the CCU ring
	*/
	#define DD_MAX_TRANS_NUMBER					0x7F

	/*!
	used to check if the MSB of a word sent to fifo transmit is set to one
	(i.e. EOF)
	*/
	#define DD_EOF_BITMASK					0x8000

	/*!
	offset, in a frame(array of words), of the frame destination
	*/
	#define DD_DESTINATION_OFFSET					0

	/*!
	offset, in a frame(array of words), of the frame length
	*/
	#define DD_MSG_LEN_OFFSET					2


	/*!
	offset, in a frame(array of words), of the transaction number
	*/
	#define DD_TRANS_NUMBER_OFFSET					4


	/*!
	if fec is destination of a frame, then destination address is 0x0000
	*/
	#define DD_FEC_IS_MSG_DESTINATION				0x0000


	/*!
	 flags x-error-addressseen-datacopied of last word of a frame
	*/
	#define DD_FEC_STATUS_FLAGS					0x7000


	/*!
	if destination is FEC (addressseen not set, datacopied not set) and no
	flag of frame last word matches DD_FEC_STATUS_FLAGS (error not set),
	then frame is OK
	*/
	#define DD_FEC_NO_ERROR						0x0000


	/*!
	value of mask when these two bits are setted.
	*/
	#define DD_FEC_ADDR_SEEN_AND_DATA_COPIED			0x3000


	/*!
	invalid transaction number, used to set the value of a variable before
	scanning an array to found a valid transaction number.
	*/
	#define DD_INVALID_TRANSACTION_NUMBER_EXAMPLE			-1


	/*!
	Bit set in CCU SRA, returned when I2C ack's are forced, telling
	(if setted) that the last I2C command have been acknowledged by I2C slave.
	*/
	#define I2C_HAVE_ACKED_COMMAND					0x0004


	/*!
	Fake transaction number to use in the frame sent to the WRITE method of the
	driver when one want to force ACKs issued from the I2C devices.
	*/
	#define DD_FAKE_TNUM_PROVOKE_ACK                                        0x00FE

	/*--------------------------------------------------------------
			Transactions management
	---------------------------------------------------------------*/


	/*!
	Indexes of array of transactions number goes from zero to
	DD_MAX_TRANS_NUMBER. So, this array must be defined as array of
	DD_MAX_TRANS_NUMBER+1 elements.
	*/
	#define DD_TRANS_ARRAY_UPPER_LIMIT				(DD_MAX_TRANS_NUMBER+1)


	/*!
	How many READ transactions can wait at a same time ? It depends on the
	system ressources capacity.
	Rem : value here can be what you want, my define with MAX and MIN is just
	for fun...
	*/
	#define DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS	((DD_MAX_TRANS_NUMBER - DD_MIN_TRANS_NUMBER)+1)


	/*!
	Timeout values, in milliseconds (??? not sure...)
	WRITE task go to sleep -timeout ms- if a frame must be wrote on ring but
	fifotransmit is busy.
	*/
	#define DD_WAIT_TIMEOUT_IF_FIFOTRA_BUSY				1


	/*!
	 how many timeout must end before a WRITE operation returns an error
	FIFO_BUSY ?
	*/
	#define DD_NBR_OF_RETRY_IF_FIFOTRA_BUSY				15


	/*!
	On a READ request, if data is not present,wait -timeout ms and retry
	If after timeout data is still not present, return an error
	Rem : This timeout is used for the read back of Forced acknowledges, too.
	*/
	#define DD_TIMEOUT_WAIT_FOR_READ_ACK				300


	/*!
	On a write operation, wait -timeout ms trying to catch a direct
	acknowledge frame.
	If after timeout direct acknowledge frame is still not present, return an error
	*/
	#define DD_TIMEOUT_WAIT_FOR_WRITE_ACK				100


	/*!
	When requesting a transaction number, wait -timeout ms. if transaction
	manager is busy
	*/
	#define DD_WAIT_TIMEOUT_IF_TRANS_NUMBER_MNGR_BUSY		1


	/*!
	how many timeout must end before a WRITE operation returns an error
	CANNOT_GET_A_TRANSACTION_NUMBER ?
	*/
	#define DD_NBR_OF_RETRY_IF_TRANS_NUMBER_MNGR_BUSY		20


	/*!
	When trying to perform a read operation, wait xxx ms if op_counter is
	already accessed
	*/
	#define DD_WAIT_TIMEOUT_IF_READ_REQUEST_BUSY			2


	/*!
	how many timeout must end before a READ operation returns an error
	cannot_access_to op_counter ?
	*/
	#define DD_NBR_OF_RETRY_IF_READ_REQUEST_BUSY			20


	/*!
	flag ; did we read a word that fits &0x8000 ?
	*/
	#define DD_FIFOREC_EOFRAME_READ					1


	/*!
	flag ; did we read a word that do not fits &0x8000 ?
	*/
	#define DD_FIFOREC_EOFRAME_NOT_READ				0


	/*!
	if a frame is read on the ring and the length of the frame is less than
	DD_NOISE_LENGTH, it is certainly a noise that occurs on the ring.
	Rem : Should be defined as DD_TRANS_NUMBER_OFFSET - 1, but not sure of that
	*/
	#define DD_NOISE_LENGTH						4



	/*!
	binary define ; used to fill some arrays of the device structure.
	Value set to UNSUCCESSFUL at the beginning of irq manager, when a
	frame issued by the a CCU arrives on the FEC. The value is setted to
	SUCCESSFUL if the "read frame" operation is successfully achieved.
	Value set to UNSUCCESSFUL at the beginning of a write event.
	Value stored in stack READ of device structure
	*/
	#define DD_READ_DATA_UNSUCCESSFUL				1


	/*!
	binary define ; used to fill some arrays of the device structure.
	Value set to UNSUCCESSFUL at the beginning of irq manager, when a
	frame issued by the a CCU arrives on the FEC. The value is setted to
	SUCCESSFUL if the "read frame" operation is successfully achieved.
	Value set to UNSUCCESSFUL at the beginning of a write event.
	Value stored in stack READ of device structure
	*/
	#define DD_READ_DATA_SUCCESSFUL					2




	/*!
        binary define ; used to fill some arrays of the device structure.
        Value set to UNSUCCESSFUL at the beginning of irq manager, when a
        frame issued by the a CCU arrives on the FEC. The value is setted to
        SUCCESSFUL if the "read frame" operation is successfully achieved.
        Value set to UNSUCCESSFUL at the beginning of a write event.
        Value stored in stack READ of device structure
        */
        #define DD_READ_DATA_CORRUPTED                              	4


	/*!
        binary define ; used to fill some arrays of the device structure.
        Value set to UNSUCCESSFUL at the beginning of irq manager, when a
        frame issued by the a CCU arrives on the FEC. The value is setted to
        SUCCESSFUL if the "read frame" operation is successfully achieved.
        Value set to UNSUCCESSFUL at the beginning of a write event.
        Value stored in stack READ of device structure
        */
        #define DD_READ_DATA_OVERFLOWED					8


	/*!
        binary define ; used to fill some arrays of the device structure.
        Value set to UNSUCCESSFUL at the beginning of irq manager, when a
        frame issued by the a CCU arrives on the FEC. The value is setted to
        SUCCESSFUL if the "read frame" operation is successfully achieved.
        Value set to UNSUCCESSFUL at the beginning of a write event.
        Value stored in stack READ of device structure
        */
        #define DD_READ_DATA_TIMED_OUT					16


	/*!
	binary define ; used to fill some arrays of the device structure.
	Value set to UNSUCCESSFUL at the beginning of irq manager function.
	When the frame issued by the FEC on the ring comes back to the FEC with
	the ACK bits setted to mean that the CCU have well seen and understand
	the command, the value is set to SUCCESSFUL.
	Value set to set to UNSUCCESSFUL at the beginning of a write event.
	Value stored in stack COPY of device structure
	*/
	#define DD_COPY_DATA_UNSUCCESSFUL				1


	/*!
	binary define ; used to fill some arrays of the device structure.
	Value set to UNSUCCESSFUL at the beginning of irq manager function.
	When the frame issued by the FEC on the ring comes back to the FEC with
	the ACK bits setted to mean that the CCU have well seen and understand
	the command, the value is set to SUCCESSFUL.
	Value set to set to UNSUCCESSFUL at the beginning of a write event.
	Value stored in stack COPY of device structure
	*/
	#define DD_COPY_DATA_SUCCESSFUL					2


	/*!
	binary define ; used to fill some arrays of the device structure.
	Value set to UNSUCCESSFUL at the beginning of irq manager function.
	When the frame issued by the FEC on the ring comes back to the FEC with
	the ACK bits setted to mean that the CCU have well seen and understand
	the command, the value is set to SUCCESSFUL.
	Value set to set to UNSUCCESSFUL at the beginning of a write event.
	Value stored in stack COPY of device structure
	*/
	#define DD_COPY_DATA_CORRUPTED					4



	/*!
	binary define ; used to fill some arrays of the device structure.
	Value set to UNSUCCESSFUL at the beginning of irq manager function.
	When the frame issued by the FEC on the ring comes back to the FEC with
	the ACK bits setted to mean that the CCU have well seen and understand
	the command, the value is set to SUCCESSFUL.
	Value set to set to UNSUCCESSFUL at the beginning of a write event.
	Value stored in stack COPY of device structure
	*/
	#define DD_COPY_DATA_OVERFLOWED					8

	/*!
	binary define ; used to fill some arrays of the device structure.
	Value set to UNSUCCESSFUL at the beginning of irq manager function.
	When the frame issued by the FEC on the ring comes back to the FEC with
	the ACK bits setted to mean that the CCU have well seen and understand
	the command, the value is set to SUCCESSFUL.
	Value set to set to UNSUCCESSFUL at the beginning of a write event.
	Value stored in stack COPY of device structure
	*/
	#define DD_COPY_DATA_TIMED_OUT					16



	/*!
	binary define used to know if a given transaction number is free at a
	given time
	*/
	#define DD_TRANS_NUMBER_IS_FREE					0

	/*!
	binary define used to know if a given transaction number is free at a
	given time
	*/
	#define DD_TRANS_NUMBER_IS_IN_USE				1


	/*!
	BACK and RECEIVED stacks are filled with these values at driver init or
	module reset
	*/
	#define DD_BACK_STACK_INIT_VALUE				0x0000

	/*!
	BACK and RECEIVED stacks are filled with these values at driver init or
	module reset
	*/
	#define DD_RECEIVED_STACK_INIT_VALUE				0x0000


	/*!
	if more than DD_MAX_CONTINUOUS_FRAMES_READ_IN_IRQ frames are read on the
	ring, without exiting at least once the interrupt manager, the ring is
	certainly fled with data. Then, return an error.
	*/
	#define DD_MAX_CONTINUOUS_FRAMES_READ_IN_IRQ			1000


	/*!
	if more than DD_MAX_NC_ERRORS_IN_IRQ frames are received And no one of
	them is valid, then there is certainly a problem on the ring (typically,
	ring is open and you only get noise).
	*/
	#define DD_MAX_NC_ERRORS_IN_IRQ					1000


	/*!
	binary define Used to know if functions of the driver are called from an
	other internal function or via an ioctl.
	external is for calls via IOCTL, internal for calls from inside the driver
	*/
	#define DD_EXTERNAL_COMMAND					0

	/*!
	binary define Used to know if functions of the driver are called from an
	other internal function or via an ioctl.
	external is for calls via IOCTL, internal for calls from inside the driver
	*/
	#define DD_INTERNAL_COMMAND					1


	/*!
	During reset, we wait that all begun read and write methods ends.
	So, Reset go to sleep for DD_TIMEOUT_WAIT_ON_RESET ms. if some
	transactions are not ended, and check again.
	*/
	#define DD_TIMEOUT_WAIT_ON_RESET				25


	/*!
	After more than DD_MAX_WAIT_CYCLES_IN_RESET go to sleep if some R/W
	operations are not ended, the reset method ends on a watchdog error.
	*/
	#define DD_MAX_WAIT_CYCLES_IN_RESET				50


	/*!
	Bit number corresponding to the flag RESET_RUNNING of the bitmap used for
	semaphores.
	*/
	#define DD_SOFT_RESET_FLAG       				0


	/*!
	bitmask to use with a & to know if a reset is currently performed or not.
	*/
	#define DD_SOFTRESET_RUNNING					0x0001


	/*!
	Bit number corresponding to the flag FIFOTRANSMIT IN USE of the bitmap
	used for semaphores.
	*/
	#define DD_FIFOTRA_IN_USE_FLAG       				1


	/*!
	bitmask to use with a & to know if the fifo transmit is currently in use
	or not.
	*/
	#define DD_FIFOTRA_IN_USE					0x0002


	/*!
	Bit number corresponding to the flag TRANSACTION MANAGER IN USE of the
	bitmap used for semaphores.
	*/
	#define DD_TRANS_NUMBER_MNGR_IN_USE_FLAG       			2


	/*!
	bitmask to use with a & to know if the fifo transmit is currently in use
	or not.
	*/
	#define DD_TRANS_NUMBER_MNGR_IN_USE				0x0004


	/*!
	Bit number corresponding to the flag REQUEST_FOR_READ of the bitmap used
	for semaphores.
	*/
	#define DD_READ_REQUEST_FLAG       				3


	/*!
	bitmask to use with a & to know if the fifo transmit is currently in use
	or not.
	*/
	#define DD_READ_REQUEST_IN_USE					0x0008


	/*!
	value used to test if flag is set (!0) or not set (0)
	*/
	#define DD_FLAG_WAS_NOT_SET					0


	/*!
	delay to spend before freeing transaction number ; HZ is 1 second.
	*/
	#define DD_FREE_TRANS_DELAY                     			(HZ/10)


        /*!
	delay (in seconds) of 'interrupts disabled state' in case of flood on the ring
	*/
	//#define DD_REENABLE_PLX		                                	7

	/*!
	is a flag (whatever it is) enabled or disabled
	*/
	#define DD_FLAG_IS_DISABLED							0

	/*!
	is a flag (whatever it is) enabled or disabled
	*/
	#define DD_FLAG_IS_ENABLED							1

	/*!
	How many loops do we have to do when we wait for the bit fifo transmit running to raise down ?
	*/
	#define DD_LOOP_LIMIT_WHEN_POLLING_FIFOTRA_RUNNING					10

	/*!
	In each of these loops, how long do we wait (in microseconds) ?
	*/
	#define DD_INLOOP_DELAY_WHEN_POLLING_FIFOTRA_RUNNING					1

	/*!
	Bitmask applied on the third element of a frame in order to know if we are working with a long frame or with a short frame.
	*/
	#define DD_IS_LONGFRAME_BITMASK								0x0080

	/*!
	Bitmask applied on the third element of a frame, used to re-compute frame length, in case of long frame.
	*/
	#define DD_UPPER_BITMASK_FOR_LONGFRAME							0x007F

	/*!
	Short frame case : How many words do we have in the beginning of the frame until we reach the LENGTH word (included)
	*/
	#define DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME						3

	/*!
	Long frame case : How many words do we have in the beginning of the frame until we reach the LENGTH word (included)
	*/
	#define DD_FRAME_HEAD_LENGTH_IF_LONGFRAME						4




	/*--------------------------------------------------------------
		Ring Error Management - Driver general status
	---------------------------------------------------------------*/

	/*!
	Command sent to the dd_ring_error_management function : Set on the error counting
	*/
	#define DD_TURN_REM_ON								1

	/*
	Command sent to the dd_ring_error_management function : Set off the error counting
	*/
	#define DD_TURN_REM_OFF								2

	/*
	Command sent to the dd_ring_error_management function : reset error counters
	*/
	#define DD_RESET_REM_VALUE							3

	/*!
	Command sent to the dd_ring_error_management function : get error counter
	*/
	#define DD_GET_REM_VALUE							4

	/*!
	Command sent to the dd_ring_error_management function : get status
	*/
	#define DD_GET_REM_STATUS							5

	/*!
	How many different errors do we want to spy, i.e. Max Size of errors counter array
	*/
	#define DD_NB_OF_ERRORS_TO_SPY							4

	/*!
	Offset for -frame too long error- counter in array of errors counter
	*/
	#define DD_LONG_FRAME_ERROR_OFFSET						0

	/*!
	Offset for -frame too short error- counter in array of errors counter
	*/
	#define DD_SHORT_FRAME_ERROR_OFFSET						1

	/*!
	Offset for -wrong transaction number error- counter in array of errors counter
	*/
	#define DD_WRONG_TRANS_NUMBER_ERROR_OFFSET					2

	/*!
	Offset for -warning emitted by a device error- counter in array of errors counter
	*/
	#define DD_WARNING_FROM_DEVICE_ERROR_OFFSET					3


	/*!
	IRQ is ON/OFF bitmask for the driver general status bitfield
	*/
	#define DD_STATUS_FLAG_IRQ_IS_ON				0x0001

	/*!
	Did a long frame error exists bitmask for the driver general status bitfield
	*/
	#define DD_STATUS_FLAG_LONG_FRAME_ERROR				0x0002

	/*!
	Did a short frame error exists bitmask for the driver general status bitfield
	*/
	#define DD_STATUS_FLAG_SHORT_FRAME_ERROR			0x0004

	/*!
	Did a bad transaction number error exists bitmask for the driver general status bitfield
	*/
	#define DD_STATUS_FLAG_BAD_TRANS_NUMBER_ERROR			0x0008

	/*!
	Did a warning emitted by a device error exists bitmask for the driver general status bitfield
	*/
	#define DD_STATUS_FLAG_WARNING_EMITTED_BY_DEVICE_ERROR		0X0010

	/*!
	Ring Error Counting is ON/OFF bitmask for the driver general status bitfield
	*/
	#define DD_STATUS_FLAG_ERRORCOUNTING_IS_ON                0x0020



	/*--------------------------------------------------------------
				Macros Definition
	---------------------------------------------------------------*/


	/*!
	Macro used to decode the driver general status : Are IRQs on or OFF ?
	*/
	#define isIrqOn(x)						(x & DD_STATUS_FLAG_IRQ_IS_ON)

	/*!
	Macro used to decode the driver general status : Do we have at least one fault of kind : frame too long ?
	*/
	#define existsFaultOnLongFrames(x)				(x & DD_STATUS_FLAG_LONG_FRAME_ERROR)

	/*!
	Macro used to decode the driver general status : Do we have at least one fault of kind : frame too short ?
	*/
	#define existsFaultOnShortFrames(x)				(x & DD_STATUS_FLAG_SHORT_FRAME_ERROR)

	/*!
	Macro used to decode the driver general status : Do we have at least one fault of kind : wrong transaction number ?
	*/
	#define existsFaultOnTransNumber(x)				(x & DD_STATUS_FLAG_BAD_TRANS_NUMBER_ERROR)

	/*!
	Macro used to decode the driver general status : Do we have at least one fault of kind : warning emitted by a device ?
	*/
	#define existsWarningEmittedByDevice(x)		(x & DD_STATUS_FLAG_WARNING_EMITTED_BY_DEVICE_ERROR)

	/*!
	Macro used to decode the driver general status : Is ring errors management on or off ?
	*/
	#define isErrorCountingEnabled(x)         (x & DD_STATUS_FLAG_ERRORCOUNTING_IS_ON)

	/*!
	Macro used to decode the DD_MSG_LEN_OFFSET (third) word of a frame : do we have a long or a short frame ?
	*/
	#define isLongFrame(x)						(x & DD_IS_LONGFRAME_BITMASK)


	#define isOpticalFec(x)						(x & 1)



#define DD_VERSION_STRING_SIZE	100



#define DD_DEVICE_ID_ELECTRICAL_V0	0x9080 /*Fec_Type is 0 */
#define DD_DEVICE_ID_OPTICAL_V0		0x9054 /*Fec_Type is 1 */
#define DD_DEVICE_ID_OPTICAL_V1		0x0167 /*Fec_Type is 2 */
#define DD_DEVICE_ID_ELECTRICAL_V1	0x0167 /*Fec_Type is 2 */

#define DD_VENDOR_ID_ELECTRICAL_V0	0x10B5
#define DD_VENDOR_ID_OPTICAL_V0		0x10B5
#define DD_VENDOR_ID_OPTICAL_V1		0x10DC
#define DD_VENDOR_ID_ELECTRICAL_V1	0x10DC




#ifdef __cplusplus
}
#endif


/* End Block - avoid multiple inclusions of this file */
#endif

