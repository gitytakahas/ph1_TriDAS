/*
   FileName : 		DEVICE.C

   Content : 		Device Driver Main File, running in Kernel Space

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


/*
will run in kernel mode
*/
#ifndef __KERNEL__
#  define __KERNEL__
#endif


/*
define used for dynamically resolving links with kernel when loading module
*/
#ifndef MODULE
#  define MODULE
#endif

//K26
#include <linux/init.h>
#include <linux/module.h>


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/smp_lock.h>
#include <linux/types.h>
#include <linux/fs.h>
//#ifdef IS_FIFO_32BITS
//#include <math.h>
//#endif

#include <linux/interrupt.h>

/*
standard includes
*/


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	#define NEXTPCISTRUCT(name)	((name)->next)
	#define PCIBASEADDRESS(name, offset)	(name)->base_address[offset]
	#define WAIT_QUEUE(name)	struct wait_queue *name=NULL
#else
	#define NEXTPCISTRUCT(name)	(pci_dev_g((*param_device_ptr)->global_list.next))
	#define PCIBASEADDRESS(name, offset)	pci_resource_start((name),offset)
        #define WAIT_QUEUE(name)	DECLARE_WAIT_QUEUE_HEAD(name)
#endif



/*
home made includes
*/
#include "driver_conf.h"
#include "defines.h"
#include "datatypes.h"
#include "dderrors.h"
#include "pci_ioctl.h"
#include "plx_ioctl.h"
#include "fec_ioctl.h"

/*
#ifdef IS_FIFO_32BITS
	#define DD_FEC_FIFO_DATA DD_FEC_FIFO_DATA_32
#else
	#define DD_FEC_FIFO_DATA DD_FEC_FIFO_DATA_16
#endif
*/


/*
On-load driver informations filling - requested by GPL licence
*/
MODULE_AUTHOR("Laurent Gross <laurent.gross@ires.in2p3.fr>");
MODULE_DESCRIPTION("FEC Unified Device Driver"	"U-6.0" "02 May 2005");
MODULE_LICENSE("GPL");




static DD_TYPE_ERROR dd_plx_hard_reset_module(void);




#if LINUX_VERSION_CODE > KERNEL_VERSION(2,4,0)
	static spinlock_t glb_irq_lock = SPIN_LOCK_UNLOCKED;
#endif

static unsigned long glb_irq_flags;

static struct semaphore glb_ioctl_lock;


/*!
var used for auto attribution of a major for the char device driver file /dev/XXX
*/
static int glb_module_major=DD_MAJOR;

/*!
System structure related to a generic PCI device
*/
static struct pci_dev *glb_device_ptr = NULL;


/*!
counter used to check how many consecutive errors
occur in interrupt manager
*/
static int glb_errors_in_irq;

/*!
bitmap of 32 bits ; each used bit is a flag used as semaphore.
Rem: Access to these flags must be performed ONLY with atomic system functions.
*/
static __u32 glb_atomic_flags = 0;


/*!
used as timeout before re-enabling PLX IRQ's
when PLX IRQ's have been disabled in interrupt manager.
Timer-related function is dd_plx_countdown in file fecrelated.c
*/
//static struct timer_list glb_manage_plx;


/*!
counter used to check how many seconds remains
before re-enabling the PLX IRQsDD_KERNEL_IS_24X
*/
//static int glb_timer_hits=0;


/*!
Timers associated to each transaction number, used as watchdog if
transaction number freeing must be forced.
*/
static struct timer_list glb_fec_trans_number_timers[DD_TRANS_ARRAY_UPPER_LIMIT];


static DD_PLX_BA_ARRAY glb_plx_ba_array;
static DD_PLX_BA_ARRAY glb_plx_remapped_ba_array;
static DD_PLX_BA_ARRAY glb_plx_rmapsize_ba_array;


static DD_TYPE_PCI_IRQ_NUMBER glb_irq_number;

static WAIT_QUEUE(glb_wq_write);
static WAIT_QUEUE(glb_wq_read);


static DD_FEC_STD_WORD glb_copied[DD_TRANS_ARRAY_UPPER_LIMIT];
static DD_FEC_STD_WORD glb_read[DD_TRANS_ARRAY_UPPER_LIMIT];
static int glb_fec_write_trans_in_use[DD_TRANS_ARRAY_UPPER_LIMIT];



static DD_TYPE_PCI_IDENT_DEVICE glb_devID;// = DD_DEVICE_ID_ELECTRICAL_V1;
static DD_TYPE_PCI_IDENT_VENDOR glb_vendID;// = DD_VENDOR_ID;




static DD_FEC_STD_WORD glb_back[DD_TRANS_ARRAY_UPPER_LIMIT][DD_USER_MAX_MSG_LENGTH];
static DD_FEC_STD_WORD glb_received[DD_TRANS_ARRAY_UPPER_LIMIT][DD_USER_MAX_MSG_LENGTH];

static int glb_fec_trans_must_be_f_acked[DD_TRANS_ARRAY_UPPER_LIMIT];


static DD_FEC_STD_WORD glb_currentTrans;
static int glb_fec_current_read_process_waiting;


/*!
Flag used to toggle ON/OFF the ring errors counter
*/
static int glb_enable_count_of_errors_on_ring;

/*!
ring errors counter
*/
static DD_FEC_REM_TYPE glb_rem_counter[DD_NB_OF_ERRORS_TO_SPY];


/*!
PLX resets counter
*/
static DD_RESET_COUNTER_TYPE glb_plx_reset_counter;

/*!
FEC resets counter
*/
static DD_RESET_COUNTER_TYPE glb_fec_reset_counter;


/*!
Bitfield used to store the current status of the driver
*/
static DD_DRIVER_STATUS_TYPE glb_driver_status;


/*!
Flag ON/OFF used to store the current interrupt status of the driver
*/
static int glb_irq_status;


static WAIT_QUEUE(glb_wq_devices_warning);

static DD_FEC_STD_WORD glb_warning_buf[DD_USER_MAX_MSG_LENGTH];


//static struct dd_information fec_driver_info;
static char glb_dd_version[DD_VERSION_STRING_SIZE];


// 0 is for electrical V1
// 1 is for optical V1
static int glb_fec_type = 0;


//Now, this is needed by the use of N fecs with different mappings ; allocations must be made dynamically.


static DD_FEC_OFFSET dd_fec_fifotra_offset;
static DD_FEC_OFFSET dd_fec_fiforec_offset;
static DD_FEC_OFFSET dd_fec_fiforet_offset;



static int fec_detected=0;






//#ifdef IS_FIFO_32BITS
/*
Beginning of block-define for STD to 32 conversion functions.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
WARNING   WARNING   WARNING
These blocks are valid ONLY if standard word is 16 bits length
as defined in very first FEC strings protocol.
(should always be the case, but well...)
WARNING   WARNING   WARNING
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/


static DD_TYPE_ERROR dd_cat_from_std_to_32(DD_FEC_STD_WORD *fstd_buffer, DD_FEC_FIFO_DATA_32 *f32_buffer, int fstd_length, int *f32_length)
{
//#define DEBUG_dd_cat_from_std_to_32

int fstd_index, f32_index;
DD_FEC_FIFO_DATA_32 one_32_word, full_32_word;

DD_FEC_STD_WORD one_std_word;


	#ifdef DEBUG_dd_cat_from_std_to_32
		printk("Processing dd_cat_from_std_to_32\n");
		printk("sizeof(one_32_word) = %d\n",sizeof(one_32_word));
	#endif
	fstd_index=0;
	f32_index=0;
	do
	{
		full_32_word = 0x0;
		if (fstd_index < fstd_length)
		{
			one_32_word = 0x0;
			one_std_word = fstd_buffer[fstd_index];
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Processing word 1 of fstd_buffer. Word is 0x%x\n",one_std_word);
			#endif
			fstd_index++;
			one_std_word = one_std_word & 0xFF;
			one_32_word = (DD_FEC_FIFO_DATA_32)one_std_word;
			one_32_word = one_32_word << 24;
			full_32_word = full_32_word | one_32_word;
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Full word is now 0x%x\n",full_32_word);
			#endif
		}

		if (fstd_index < fstd_length)
		{
			one_32_word = 0x0;
			one_std_word = fstd_buffer[fstd_index];
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Processing word 2 of fstd_buffer. Word is 0x%x\n",one_std_word);
			#endif
			fstd_index++;
			one_std_word = one_std_word & 0xFF;
			one_32_word = (DD_FEC_FIFO_DATA_32)one_std_word;
			one_32_word = one_32_word << 16;
			full_32_word = full_32_word | one_32_word;
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Full word is now 0x%x\n",full_32_word);
			#endif
		}


		if (fstd_index < fstd_length)
		{
			one_32_word = 0x0;
			one_std_word = fstd_buffer[fstd_index];
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Processing word 3 of fstd_buffer. Word is 0x%x\n",one_std_word);
			#endif
			fstd_index++;
			one_std_word = one_std_word & 0xFF;
			one_32_word = (DD_FEC_FIFO_DATA_32)one_std_word;
			one_32_word = one_32_word << 8;
			full_32_word = full_32_word | one_32_word;
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Full word is now 0x%x\n",full_32_word);
			#endif
		}

		if (fstd_index < fstd_length)
		{
			one_32_word = 0x0;
			one_std_word = fstd_buffer[fstd_index];
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Processing word 4 of fstd_buffer. Word is 0x%x\n",one_std_word);
			#endif
			fstd_index++;
			one_std_word = one_std_word & 0xFF;
			one_32_word = (DD_FEC_FIFO_DATA_32)one_std_word;
			full_32_word = full_32_word | one_32_word;
			#ifdef DEBUG_dd_cat_from_std_to_32
				printk("Full word is now 0x%x\n",full_32_word);
			#endif
		}
		f32_buffer[f32_index] = full_32_word;
		f32_index++;
	} while (fstd_index < fstd_length);
	(*f32_length)=f32_index;
	#ifdef DEBUG_dd_cat_from_std_to_32
		printk("The 32 bits frame is 0x%x words long\n",(*f32_length));
	#endif
return DD_RETURN_OK;
}







static DD_TYPE_ERROR dd_uncat_from_32_to_std(DD_FEC_STD_WORD *fstd_buffer, DD_FEC_FIFO_DATA_32 *f32_buffer, int *fstd_length, int f32_length)
{
//#define DEBUG_dd_uncat_from_32_to_std
int fstd_index, f32_index;
DD_FEC_FIFO_DATA_32 one_32_word;
DD_FEC_STD_WORD one_std_word;

	#ifdef DEBUG_dd_uncat_from_32_to_std
		printk("Processing dd_uncat_from_32_to_std\n");
	#endif
	fstd_index=0;
	f32_index=0;
	do
	{
		#ifdef DEBUG_dd_uncat_from_32_to_std
			printk("In loop\n");
		#endif

		if (f32_index < f32_length)
		{
			one_32_word =f32_buffer[f32_index];
			one_32_word = (one_32_word >> 24) & 0xFF;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("Rotated (24) 32 bits word is : 0x%x\n", one_32_word);
			#endif
			one_std_word = (DD_FEC_STD_WORD)one_32_word;
			fstd_buffer[fstd_index] = one_std_word;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("I have bufferized : 0x%x\n", one_std_word);
			#endif
			fstd_index++;


			one_32_word =f32_buffer[f32_index];
			one_32_word = (one_32_word >> 16) & 0xFF;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("Rotated (16) 32 bits word is : 0x%x\n", one_32_word);
			#endif
			one_std_word = (DD_FEC_STD_WORD)one_32_word;
			fstd_buffer[fstd_index] = one_std_word;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("I have bufferized : 0x%x\n", one_std_word);
			#endif
			fstd_index++;


			one_32_word =f32_buffer[f32_index];
			one_32_word = (one_32_word >> 8) & 0xFF;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("Rotated (8) 32 bits word is : 0x%x\n", one_32_word);
			#endif
			one_std_word = (DD_FEC_STD_WORD)one_32_word;
			fstd_buffer[fstd_index] = one_std_word;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("I have bufferized : 0x%x\n", one_std_word);
			#endif
			fstd_index++;


			one_32_word =f32_buffer[f32_index];
			one_32_word = one_32_word & 0xFF;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("Rotated (0) 32 bits word is : 0x%x\n", one_32_word);
			#endif
			one_std_word = (DD_FEC_STD_WORD)one_32_word;
			fstd_buffer[fstd_index] = one_std_word;
			#ifdef DEBUG_dd_uncat_from_32_to_std
				printk("I have bufferized : 0x%x\n", one_std_word);
			#endif
			fstd_index++;
		}
		f32_index++;
	} while (f32_index < f32_length);

	if (isLongFrame(fstd_buffer[DD_MSG_LEN_OFFSET]))
	{
		(*fstd_length) = ((((fstd_buffer[DD_MSG_LEN_OFFSET] & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + fstd_buffer[DD_MSG_LEN_OFFSET+1]) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
	}
	else
	{
		(*fstd_length) = (fstd_buffer[DD_MSG_LEN_OFFSET] + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);
	}

	#ifdef DEBUG_dd_uncat_from_32_to_std
		printk("Final standard frame length is : 0x%x\n", (*fstd_length));
	#endif

	/* Add statusk bits to the last data word */
	fstd_buffer[(*fstd_length)-1] = fstd_buffer[(*fstd_length)-1] | (fstd_buffer[(*fstd_length)]<<8);
	#ifdef DEBUG_dd_uncat_from_32_to_std
		printk("Status byte of reconstructed frame will be : 0x%x\n", (fstd_buffer[(*fstd_length)]<<8));
	#endif

return DD_RETURN_OK;
}

//End of optical block-define for 16-32 conversion functions
//#endif


//////////////////////
//PCI XXX
////////////////////



/*!
<b>FUNCTION : dd_ioc_check_if_system_is_pci_compliant</b>
- Job
	- Check if the system is hardware PCI compliant (does a PCI
	bus exist) and if the system is software PCI compliant (was PCI
	support compiled into Kernel ?)
- Inputs
	- Void
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Nothing
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_NO_PCI_SOFTWARE_SUPPORT
		- DD_NO_PCI_HARDWARE_SUPPORT
- Sub-functions calls :
	- dd_stack_msg
	- pci_present	(system)
*/

static DD_TYPE_ERROR dd_ioc_check_if_system_is_pci_compliant(void)
{

	// Independamment des cartes plugees, est ce qu'on a un system PCI ?
	/// Est ce que le kernel est compatible PCI ?
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)

		#ifdef CONFIG_PCI
			// Rem : ce define est sette au niveau kernel, a la compilation
			// est ce que la machine possede un bus PCI ?
			if ( pci_present() <= DD_LIMIT_PCI_IS_PRESENT ) return DD_NO_PCI_SOFTWARE_SUPPORT;
		#else
			return DD_NO_PCI_HARDWARE_SUPPORT;
		#endif
	#endif

return DD_RETURN_OK;
}



/*!
<b>FUNCTION 0004	: dd_ioc_find_pci_card</b>
- Job
	- Check if the PCI board identified by the pair (DEVICE_ID,
	VENDOR_ID) exists on the system
- Inputs
	- struct pointer to a DD_device_info struct.
		- <b>Fields used :</b>
		- ->ident_vendor
		- ->ident_device
	- address of a struct pointer to a PCI device (pci_dev**)
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- (pci_dev**) is needed here for writeback of PCI device
	pointer detection
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_DEVICE_NOT_FOUND
- Sub-functions calls :
	- dd_stack_msg
	- pci_find_device	(system)
*/

static DD_TYPE_ERROR dd_ioc_find_pci_card(struct pci_dev **param_device_ptr)
{
int lcl_fec_counter=0;
int lcl_electrical_fecs_v0_counter=0;
int lcl_optical_fecs_v0_counter=0;
//int lcl_electrical_fecs_v1_counter=0;
int lcl_optical_fecs_v1_counter=0;
int lcl_fec_type;


struct pci_dev *lcl_efecv0_ptr = NULL;
struct pci_dev *lcl_ofecv0_ptr = NULL;
struct pci_dev *lcl_ofecv1_ptr = NULL;

int lcl_ov0_counter, lcl_ov1_counter, lcl_ev0_counter;
int first_fec_type=0;
struct pci_dev *lcl_fec_ptr = NULL;
//#define DD_PCI_DEBUG


	#ifdef DD_PCI_DEBUG
		printk("Before functions - elec_v.0:%d - optical_v.0:%d - opto/elec_v.1:%d\n",lcl_electrical_fecs_v0_counter,lcl_optical_fecs_v0_counter,lcl_optical_fecs_v1_counter);
	#endif



	/****************************************************
	Now seek for electrical FECs - Old version (EV0)
	****************************************************/
	//Which Vendor ID do we seek ?
	glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;

	//(*param_device_ptr) = NULL;	
	//Check if we have electrical FECs
	//Set device_id for electrical fecs and probe pci list
	glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
	lcl_electrical_fecs_v0_counter=0;

	//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, (*param_device_ptr));
	(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);

	if ((*param_device_ptr) != NULL)
	{

		lcl_efecv0_ptr=(*param_device_ptr);

		lcl_electrical_fecs_v0_counter++;

		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_efecv0_ptr) )
		{


			if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_electrical_fecs_v0_counter++;
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);

		}
	}
	#ifdef DD_PCI_DEBUG
		printk("I have found %d electrical fec v.0 on this system.\n", lcl_electrical_fecs_v0_counter);
	#endif


	/****************************************************
	First, seek for optical FECs - Old version (OV0)
	****************************************************/
	//Which Vendor ID do we seek ?
	glb_vendID=DD_VENDOR_ID_OPTICAL_V0;

	//(*param_device_ptr) = NULL;
	//Check if we have optical FECs
	//Set device_id for optical fecs and probe pci list
	glb_devID=DD_DEVICE_ID_OPTICAL_V0;
	lcl_optical_fecs_v0_counter=0;
	//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, (*param_device_ptr));
	(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);
	if ((*param_device_ptr) != NULL)
	{
		lcl_ofecv0_ptr=(*param_device_ptr);
		lcl_optical_fecs_v0_counter++;

		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv0_ptr) )
		{
			if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_optical_fecs_v0_counter++;
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
		}
	}
	#ifdef DD_PCI_DEBUG
		printk("I have found %d optical fec v.0 on this system.\n", lcl_optical_fecs_v0_counter);
	#endif





	/****************************************************
	Now seek for electrical/optical FECs - New version (EV1/OV1)
	****************************************************/
	//Which Vendor ID do we seek ?
	glb_vendID=DD_VENDOR_ID_OPTICAL_V1;

	//(*param_device_ptr) = NULL;
	//Check if we have optical FECs
	//Set device_id for optical fecs and probe pci list
	glb_devID=DD_DEVICE_ID_OPTICAL_V1;
	lcl_optical_fecs_v1_counter=0;
	//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, (*param_device_ptr));
	(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);
	if ((*param_device_ptr) != NULL)
	{
		lcl_ofecv1_ptr=(*param_device_ptr);
		lcl_optical_fecs_v1_counter++;
		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv1_ptr) )
		{
			if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_optical_fecs_v1_counter++;
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
		}
	}
	#ifdef DD_PCI_DEBUG
		printk("I have found %d electrical/optical fec v.1 on this system.\n", lcl_optical_fecs_v1_counter);
	#endif



	/****************************************************
	Print probing results
	****************************************************/
	#ifdef DD_PCI_DEBUG
		printk("After functions - elec_v.0:%d - optical_v.0:%d - opto/elec_v.1:%d\n",lcl_electrical_fecs_v0_counter,lcl_optical_fecs_v0_counter,lcl_optical_fecs_v1_counter);
	#endif



	//if we do not have any FECs plugged in the system, exit on error
	if ((lcl_electrical_fecs_v0_counter==0) && (lcl_optical_fecs_v0_counter==0) && (lcl_optical_fecs_v1_counter==0))
	{
		#ifdef DD_PCI_DEBUG
			printk("no FEC found on this system ; exiting.\n");
		#endif
		return DD_PCI_DEVICE_NOT_FOUND;

	}


	/****************************************************
	If the system handles ELECTRICAL FECs V.0 only
	****************************************************/
	//If we have only electrical FECs v.0, re-set pointer to first occurence
	if ( (lcl_optical_fecs_v0_counter==0) && (lcl_optical_fecs_v1_counter==0) )
	{
		#ifdef DD_PCI_DEBUG
			printk("This system handles only electrical v.0 FECs.\n");
		#endif

		//Set pointer to first FEC
		glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
		glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, (*param_device_ptr));
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);
		(*param_device_ptr)=lcl_efecv0_ptr;
		#ifdef DD_PCI_DEBUG
			printk("I got the reference for the FEC number 0.\n");
		#endif


		//If we are in debug mode, list all FECs
		#ifdef DD_PCI_DEBUG
			printk("SCANNING FULL SYSTEM FOR FECs ...\n");

			lcl_fec_counter = 0;
			printk("FEC number %d is of type : Electrical v.0 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_ELECTRICAL_V0, DD_DEVICE_ID_ELECTRICAL_V0);

			while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_efecv0_ptr) )
			{
				if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_ELECTRICAL_V0) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_ELECTRICAL_V0))
				{
					lcl_fec_counter++;
					printk("FEC number %d is of type : Electrical v.0 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_ELECTRICAL_V0, DD_DEVICE_ID_ELECTRICAL_V0);
				}
				(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
			}
			printk("END OF FULL SYSTEM SCAN ...\n");
		#endif

		//Reset pointer to first FEC
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);
		(*param_device_ptr) = lcl_efecv0_ptr;
		#ifdef DD_PCI_DEBUG
			printk("I got (again) the reference for the FEC number 0.\n");
		#endif


		//Then, if requested, manage to find FEC nN
		lcl_fec_counter=0;
		if (DD_FEC_NUMBER > 0)
		{
			#ifdef DD_PCI_DEBUG
				printk("Seeking the reference for the FEC number %d in PCI list...\n", DD_FEC_NUMBER);
			#endif
			while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (lcl_fec_counter<DD_FEC_NUMBER) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_efecv0_ptr) )

			{
				if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_fec_counter++;
				(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
			}
			if ( (lcl_fec_counter != DD_FEC_NUMBER) || ((*param_device_ptr) == NULL) )
			{
				#ifdef DD_PCI_DEBUG
					printk("Unable to find the FEC number %d\n", DD_FEC_NUMBER);
				#endif
				return DD_PCI_DEVICE_NOT_FOUND;
			}
		}

		//Once the device detected, exit on success
		glb_fec_type = 0;
		#ifdef DD_PCI_DEBUG
			printk("Electrical FEC v.0 number %d successfully detected and configured.\n", DD_FEC_NUMBER);
		#endif
		return DD_RETURN_OK;
	}


	/****************************************************
	If the system handles OPTICAL FECs V.0 only
	****************************************************/
	//If we have only optical FECs v.0, re-set pointer to first occurence
	if ( (lcl_optical_fecs_v1_counter==0) && (lcl_electrical_fecs_v0_counter==0) )
	{
		#ifdef DD_PCI_DEBUG
			printk("This system handles only optical v.0 FECs.\n");
		#endif

		//Set pointer to first FEC
		glb_vendID=DD_VENDOR_ID_OPTICAL_V0;
		glb_devID=DD_DEVICE_ID_OPTICAL_V0;
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, (*param_device_ptr));
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);
		(*param_device_ptr) = lcl_ofecv0_ptr;
		#ifdef DD_PCI_DEBUG
			printk("I got the reference for the FEC number 0.\n");
		#endif


		//If we are in debug mode, list all FECs
		#ifdef DD_PCI_DEBUG
			printk("SCANNING FULL SYSTEM FOR FECs ...\n");

			lcl_fec_counter = 0;
			printk("FEC number %d is of type : Optical v.0 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_OPTICAL_V0, DD_DEVICE_ID_OPTICAL_V0);

			while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv0_ptr) )
			{
				if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V0) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V0))
				{
					lcl_fec_counter++;
					printk("FEC number %d is of type : Optical v.0 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_OPTICAL_V0, DD_DEVICE_ID_OPTICAL_V0);
				}
				(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
			}
			printk("END OF FULL SYSTEM SCAN ...\n");
		#endif

		//Reset pointer to first FEC
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);
		(*param_device_ptr) = lcl_ofecv0_ptr;
		#ifdef DD_PCI_DEBUG
			printk("I got (again) the reference for the FEC number 0.\n");
		#endif

		//Then, if requested, manage to find FEC n
		lcl_fec_counter=0;
		if (DD_FEC_NUMBER > 0)
		{
			#ifdef DD_PCI_DEBUG
				printk("Seeking the reference for the FEC number %d in PCI list...\n", DD_FEC_NUMBER);
			#endif
			while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL)&& (lcl_fec_counter<DD_FEC_NUMBER) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv0_ptr) )

			{
				if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_fec_counter++;
				(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
			}
			if ( (lcl_fec_counter != DD_FEC_NUMBER) || ((*param_device_ptr) == NULL) )
			{
				#ifdef DD_PCI_DEBUG
					printk("Unable to find the FEC number %d\n", DD_FEC_NUMBER);
				#endif
				return DD_PCI_DEVICE_NOT_FOUND;
			}
		}

		//Once the device detected, exit on success
		glb_fec_type = 1;
		#ifdef DD_PCI_DEBUG
			printk("Optical FEC v.0 number %d successfully detected and configured.\n", DD_FEC_NUMBER);
		#endif
		return DD_RETURN_OK;
	}


	/****************************************************
	If the system handles OPTICAL/ELECTRICAL FECs V.1 only
	****************************************************/
	//If we have only optical/electrical FECs v.1, re-set pointer to first occurence
	if ((lcl_optical_fecs_v0_counter==0) && (lcl_electrical_fecs_v0_counter==0) )
	{
		#ifdef DD_PCI_DEBUG
			printk("This system handles only optical/electrical v.1 FECs.\n");
		#endif

		//Set pointer to first FEC
		glb_vendID=DD_VENDOR_ID_OPTICAL_V1;
		glb_devID=DD_DEVICE_ID_OPTICAL_V1;
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, (*param_device_ptr));
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);
		(*param_device_ptr) = lcl_ofecv1_ptr;
		#ifdef DD_PCI_DEBUG
			printk("I got the reference for the FEC number 0.\n");
		#endif

		
		//If we are in debug mode, list all FECs
		#ifdef DD_PCI_DEBUG
			printk("SCANNING FULL SYSTEM FOR FECs ...\n");

			lcl_fec_counter = 0;
			printk("FEC number %d is of type : Electrical/Optical v.1 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_OPTICAL_V1, DD_DEVICE_ID_OPTICAL_V1);

			while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv1_ptr) )
			{
				if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V1) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V1))
				{
					lcl_fec_counter++;
					printk("FEC number %d is of type : Electrical/Optical v.1 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_OPTICAL_V1, DD_DEVICE_ID_OPTICAL_V1);
				}
				(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
			}
			printk("END OF FULL SYSTEM SCAN ...\n");
		#endif

		//Reset pointer to first FEC
		//(*param_device_ptr) = pci_find_device(glb_vendID, glb_devID, 0);
		(*param_device_ptr) = lcl_ofecv1_ptr;
		#ifdef DD_PCI_DEBUG
			printk("I got (again) the reference for the FEC number 0.\n");
		#endif

		//Then, if requested, manage to find FEC nN
		lcl_fec_counter=0;
		if (DD_FEC_NUMBER > 0)
		{
			#ifdef DD_PCI_DEBUG
				printk("Seeking the reference for the FEC number %d in PCI list...\n", DD_FEC_NUMBER);
			#endif
			while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL)&& (lcl_fec_counter<DD_FEC_NUMBER) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv1_ptr) )

			{
				if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_fec_counter++;
				(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
			}
			if ( (lcl_fec_counter != DD_FEC_NUMBER) || ((*param_device_ptr) == NULL) )
			{
				#ifdef DD_PCI_DEBUG
					printk("Unable to find the FEC number %d\n", DD_FEC_NUMBER);
				#endif
				return DD_PCI_DEVICE_NOT_FOUND;
			}
		}

		//Once the device detected, exit on success
		glb_fec_type = 2;
		#ifdef DD_PCI_DEBUG
			printk("Electrical/Optical FEC v.1 number %d successfully detected and configured.\n", DD_FEC_NUMBER);
		#endif
		return DD_RETURN_OK;
	}



	/***********************************************************
	If we have to handle different kind of FECs on one computer
	***********************************************************/
	//if we have both kind of FECs, then :
	/* Rem : No need to test counters here, because if we fall on this point
	of the code it just means that either no FEC has been detected, or that no
	"single" fec type condition has been met */


	/* If we do not have optical v.1 on the system, check which
	FEC pointer between electrical v.0 and optical v.0 is the
	first one in the chained PCI struct list */
	if (lcl_optical_fecs_v1_counter==0)
	{
		//Set IDs to electrical v.0
		glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
		glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
		
		//Set seekpoint to first optical v.0
		(*param_device_ptr) = lcl_ofecv0_ptr;
		
		//Set opticals counter to zero, then count
		lcl_ev0_counter = 0;
		
		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv0_ptr) )
		{
			if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_ev0_counter++;
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
		}

		//Now, check
		if (lcl_ev0_counter == lcl_electrical_fecs_v0_counter)
		{
			//In this case, first detected FEC is optical v0
			first_fec_type = 1;
		}
		else
		{
			//In this case, first detected FEC is electrical v0
			first_fec_type = 0;
		}
	}


	/* If we do not have optical v.0 on the system, check which
	FEC pointer between electrical v.0 and optical v.1 is the
	first one in the chained PCI struct list */
	if (lcl_optical_fecs_v0_counter==0)
	{
		//Set IDs to electrical v.0
		glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
		glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
		
		//Set seekpoint to first optical v.1
		(*param_device_ptr) = lcl_ofecv1_ptr;
		
		//Set opticals counter to zero, then count
		lcl_ev0_counter = 0;
		
		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv1_ptr) )
		{
			if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_ev0_counter++;
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
		}

		//Now, check
		if (lcl_ev0_counter == lcl_electrical_fecs_v0_counter)
		{
			//In this case, first detected FEC is optical v1
			first_fec_type = 2;
		}
		else
		{
			//In this case, first detected FEC is electrical v0
			first_fec_type = 0;
		}
	}




	/* If we do not have electrical v.0 on the system, check which
	FEC pointer between optical v.0 and optical v.1 is the
	first one in the chained PCI struct list */
	if (lcl_electrical_fecs_v0_counter==0)
	{
		//Set IDs to optical v.0
		glb_vendID=DD_VENDOR_ID_OPTICAL_V0;
		glb_devID=DD_DEVICE_ID_OPTICAL_V0;
		
		//Set seekpoint to first optical v.1
		(*param_device_ptr) = lcl_ofecv1_ptr;
		
		//Set opticals counter to zero, then count
		lcl_ov0_counter = 0;
		
		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv1_ptr) )
		{
			if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_ov0_counter++;
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
		}

		//Now, check
		if (lcl_ov0_counter == lcl_optical_fecs_v0_counter)
		{
			//In this case, first detected FEC is optical v1
			first_fec_type = 2;
		}
		else
		{
			//In this case, first detected FEC is optical v0
			first_fec_type = 1;
		}
	}





	/* If we have all three kind of FECs on the system, check which
	FEC pointer between electrical v.0 and optical v.0 and optical v.1 is the
	first one in the chained PCI struct list */
	if ( (lcl_electrical_fecs_v0_counter!=0) && (lcl_optical_fecs_v0_counter!=0) && (lcl_optical_fecs_v1_counter!=0) )
	{
		//Set IDs to electrical v.0
		//glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
		//glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
		
		//Set seekpoint to first optical v.0
		(*param_device_ptr) = lcl_ofecv0_ptr;
		
		//Set counters to zero, then count
		lcl_ev0_counter = 0;
		lcl_ov1_counter = 0;

		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv0_ptr) )
		{
			if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_ELECTRICAL_V0) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_ELECTRICAL_V0) ) lcl_ev0_counter++;
			if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V1) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V1) ) lcl_ov1_counter++;
			
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
		}

		//Now, check
		if ( (lcl_ev0_counter == lcl_electrical_fecs_v0_counter) && (lcl_ov1_counter == lcl_optical_fecs_v1_counter) )
		{
			//In this case, first detected FEC is optical v0
			first_fec_type = 1;
		}
		else
		{
			if ((lcl_ev0_counter < lcl_electrical_fecs_v0_counter) && (lcl_ov1_counter < lcl_optical_fecs_v1_counter))
			{
				//In this case, the game is played between electrical v.0 and optical v.1
				//Set IDs to electrical v.0
				glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
				glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
		
				//Set seekpoint to first optical v.1
				(*param_device_ptr) = lcl_ofecv1_ptr;
		
				//Set opticals counter to zero, then count
				lcl_ev0_counter = 0;
				while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv1_ptr) )
				{
					if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_ev0_counter++;
					(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
				}

				//Now, check
				if (lcl_ev0_counter == lcl_electrical_fecs_v0_counter)
				{
					//In this case, first detected FEC is optical v1
					first_fec_type = 2;
				}
				else
				{
					//In this case, first detected FEC is electrical v0
					first_fec_type = 0;
				}

			}
			
			if ((lcl_ev0_counter < lcl_electrical_fecs_v0_counter) && (lcl_ov1_counter == lcl_optical_fecs_v1_counter))
			{
				//In this case, the game is played between electrical v.0 and optical v.0
				//Set IDs to electrical v.0
				glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
				glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
		
				//Set seekpoint to first optical v.0
				(*param_device_ptr) = lcl_ofecv0_ptr;
		
				//Set opticals counter to zero, then count
				lcl_ev0_counter = 0;
				while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv0_ptr) )
				{
					if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_ev0_counter++;
					(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
				}

				//Now, check
				if (lcl_ev0_counter == lcl_electrical_fecs_v0_counter)
				{
					//In this case, first detected FEC is optical v0
					first_fec_type = 1;
				}
				else
				{
					//In this case, first detected FEC is electrical v0
					first_fec_type = 0;
				}

			}
			if ((lcl_ev0_counter == lcl_electrical_fecs_v0_counter) && (lcl_ov1_counter < lcl_optical_fecs_v1_counter))
			{
				//In this case, the game is played between optical v.0 and optical v.1
				//Set IDs to optical v.0
				glb_vendID=DD_VENDOR_ID_OPTICAL_V0;
				glb_devID=DD_DEVICE_ID_OPTICAL_V0;
		
				//Set seekpoint to first optical v.1
				(*param_device_ptr) = lcl_ofecv1_ptr;
		
				//Set opticals counter to zero, then count
				lcl_ov0_counter = 0;
				while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr)!=lcl_ofecv1_ptr) )
				{
					if ( (NEXTPCISTRUCT(*param_device_ptr)->vendor == glb_vendID) && (NEXTPCISTRUCT(*param_device_ptr)->device == glb_devID) ) lcl_ov0_counter++;
					(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
				}

				//Now, check
				if (lcl_ov0_counter == lcl_optical_fecs_v0_counter)
				{
					//In this case, first detected FEC is optical v1
					first_fec_type = 2;
				}
				else
				{
					//In this case, first detected FEC is optical v0
					first_fec_type = 1;
				}
			}
		}
		
	}



	//Now, set the start point pointer
	switch (first_fec_type)
	{
		case 0: 
			(*param_device_ptr) = lcl_efecv0_ptr;
			glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
			glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
			break;
		case 1:
			(*param_device_ptr) = lcl_ofecv0_ptr;
			glb_vendID=DD_VENDOR_ID_OPTICAL_V0;
			glb_devID=DD_DEVICE_ID_OPTICAL_V0;
			break;
		case 2:
			(*param_device_ptr) = lcl_ofecv1_ptr; 
			glb_vendID=DD_VENDOR_ID_OPTICAL_V1;
			glb_devID=DD_DEVICE_ID_OPTICAL_V1;
			break;
		default:
			printk("Fec detection has acted in a strange way - detection impossible..\n");
			printk("please signal this error at fec-support@ires.in2p3.fr.\n");
			return DD_PCI_DEVICE_NOT_FOUND;
			break;
	}

	//keep memory of startpoint
	lcl_fec_ptr = (*param_device_ptr);

	//If we are in debug mode, list all FECs
	#ifdef DD_PCI_DEBUG
		printk("SCANNING FULL SYSTEM FOR FECs ...\n");

		lcl_fec_counter = 0;

		if (((*param_device_ptr)->vendor == DD_VENDOR_ID_ELECTRICAL_V0) && ((*param_device_ptr)->device == DD_DEVICE_ID_ELECTRICAL_V0))
		{
			printk("FEC number %d is of type : Electrical v.0 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_ELECTRICAL_V0, DD_DEVICE_ID_ELECTRICAL_V0);
		}
		if (((*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V0) && ((*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V0))
		{
			printk("FEC number %d is of type : Optical v.0 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_OPTICAL_V0, DD_DEVICE_ID_OPTICAL_V0);
		}
		if (((*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V1) && ((*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V1))
		{
			printk("FEC number %d is of type : Electrical/Optical v.1 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_OPTICAL_V1, DD_DEVICE_ID_OPTICAL_V1);
		}

		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (NEXTPCISTRUCT(*param_device_ptr) != lcl_fec_ptr) )
		{
			//Set type
			if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_ELECTRICAL_V0) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_ELECTRICAL_V0))
			{
				lcl_fec_counter++;
				printk("FEC number %d is of type : Electrical v.0 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_ELECTRICAL_V0, DD_DEVICE_ID_ELECTRICAL_V0);
			}

			if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V0) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V0))
			{
				lcl_fec_counter++;
				printk("FEC number %d is of type : Optical v.0 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_OPTICAL_V0, DD_DEVICE_ID_OPTICAL_V0);
			}

			if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V1) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V1))
			{
				lcl_fec_counter++;
				printk("FEC number %d is of type : Electrical/Optical v.1 with VendorID=0x%x and DeviceID=0x%x\n",lcl_fec_counter, DD_VENDOR_ID_OPTICAL_V1, DD_DEVICE_ID_OPTICAL_V1);
			}
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
		}
		printk("END OF FULL SYSTEM SCAN ...\n");
	#endif

	/* And, finally, seek FEC number N
	Rem : At this point of code, (*param_device_ptr) already points to the
	first FEC allocated in PCI struct list */
	//Now, set the start point pointer
	switch (first_fec_type)
	{
		case 0: 
			(*param_device_ptr) = lcl_efecv0_ptr;
			glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
			glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
			lcl_fec_type=0;
			break;
		case 1:
			(*param_device_ptr) = lcl_ofecv0_ptr;
			glb_vendID=DD_VENDOR_ID_OPTICAL_V0;
			glb_devID=DD_DEVICE_ID_OPTICAL_V0;
			lcl_fec_type=1;
			break;
		case 2:
			(*param_device_ptr) = lcl_ofecv1_ptr; 
			glb_vendID=DD_VENDOR_ID_OPTICAL_V1;
			glb_devID=DD_DEVICE_ID_OPTICAL_V1;
			lcl_fec_type=2;
			break;
		default:
			printk("Fec detection step 2 has acted in a strange way - detection impossible..\n");
			printk("please signal this error at fec-support@ires.in2p3.fr.\n");
			return DD_PCI_DEVICE_NOT_FOUND;
			break;
	}

	//keep memory of startpoint
	lcl_fec_ptr = (*param_device_ptr);


	if (DD_FEC_NUMBER > 0)
	{
		#ifdef DD_PCI_DEBUG
			printk("Now seeking FEC number %d in the list.\n",DD_FEC_NUMBER);
		#endif
		lcl_fec_counter = 0;
			
		while ( (NEXTPCISTRUCT(*param_device_ptr) != NULL) && (lcl_fec_counter<DD_FEC_NUMBER) && (NEXTPCISTRUCT(*param_device_ptr) != lcl_fec_ptr) )
		{
			//detect FEC

			//Set type
			if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_ELECTRICAL_V0) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_ELECTRICAL_V0))
			{
				#ifdef DD_PCI_DEBUG
					printk("FEC number %d is of type : Electrical v.0\n",lcl_fec_counter);
				#endif
				glb_vendID=DD_VENDOR_ID_ELECTRICAL_V0;
				glb_devID=DD_DEVICE_ID_ELECTRICAL_V0;
				lcl_fec_type=0;
				lcl_fec_counter++;
			}

			if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V0) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V0))
			{
				#ifdef DD_PCI_DEBUG
					printk("FEC number %d is of type : Optical v.0\n",lcl_fec_counter);
				#endif
				glb_vendID=DD_VENDOR_ID_OPTICAL_V0;
				glb_devID=DD_DEVICE_ID_OPTICAL_V0;
				lcl_fec_type=1;
				lcl_fec_counter++;
			}

			if ((NEXTPCISTRUCT(*param_device_ptr)->vendor == DD_VENDOR_ID_OPTICAL_V1) && (NEXTPCISTRUCT(*param_device_ptr)->device == DD_DEVICE_ID_OPTICAL_V1))
			{
				#ifdef DD_PCI_DEBUG
					printk("FEC number %d is of type : Electrical/Optical v.1\n",lcl_fec_counter);
				#endif
				glb_vendID=DD_VENDOR_ID_OPTICAL_V1;
				glb_devID=DD_DEVICE_ID_OPTICAL_V1;
				lcl_fec_type=2;
				lcl_fec_counter++;
			}

				
			//Set pointer
			(*param_device_ptr) = NEXTPCISTRUCT(*param_device_ptr);
		}

		if ( (lcl_fec_counter != DD_FEC_NUMBER) || ((*param_device_ptr) == NULL) )
		{
			#ifdef DD_PCI_DEBUG
				printk("FEC number %d not found in PCI list.\n",DD_FEC_NUMBER);
			#endif
			return DD_PCI_DEVICE_NOT_FOUND;
		}
	}
		
	//Once the device detected, exit on success
	glb_fec_type = lcl_fec_type;

	#ifdef DD_PCI_DEBUG
		if (glb_fec_type==0)
		{
			printk("Electrical FEC v.0 number %d successfully detected and configured.\n", DD_FEC_NUMBER);
		}
		if (glb_fec_type==1)
		{
			printk("Optical FEC v.0 number %d successfully detected and configured.\n", DD_FEC_NUMBER);
		}
		if (glb_fec_type==2)
		{
			printk("Electrical/Optical FEC v.1 number %d successfully detected and configured.\n", DD_FEC_NUMBER);
		}
	#endif
	return DD_RETURN_OK;


}







/*!
<b>FUNCTION 0007	: dd_ioc_get_pci_card_command_reg</b>
- Job
	- Read CONTROL register value on the PCI board
- Inputs
	- struct pointer to a DD_device_info struct.
		- <b>Fields used :</b>
		- ->command_reg
		- ->ioc_container
	- address of a struct pointer to a PCI device (pci_dev**)
	- integer used to know if the function call is internal or
	external to the driver itself
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- (pci_dev**) is given as parameter rather than (pci_dev*),
	in order to stay coherent with function 004.
	- Job is related to the board first identified by (DEVICE_ID,
	VENDOR_ID), Cf. function 0004
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_ERROR_READ_COMMAND_REG
		- DD_PCI_KS_CANNOT_WRITE_COMMAND_REG_TO_US
- Sub-functions calls :
	- dd_stack_msg
	- pci_read_config_word	(system)
	- __copy_to_user		(system)
*/

static DD_TYPE_ERROR dd_ioc_get_pci_card_command_reg(struct pci_dev **param_device_ptr, struct dd_ioctl_data * param_ioctl_data, int param_source)
{

DD_TYPE_ERROR lcl_err;
DD_TYPE_PCI_COMMAND_REG lcl_command;

	/*! get info from hardware */
	lcl_err = pci_read_config_word((*param_device_ptr), PCI_COMMAND, &(lcl_command));
	if (lcl_err != DD_RETURN_OK) return DD_PCI_ERROR_READ_COMMAND_REG;

	if (param_source == DD_EXTERNAL_COMMAND)
	{
		/*! memoire checkee en debut d'ioctl */
		lcl_err = __copy_to_user((DD_TYPE_PCI_COMMAND_REG *)param_ioctl_data->ioctl_address, &(lcl_command), sizeof(DD_TYPE_PCI_COMMAND_REG));
		if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_PCI_KS_CANNOT_WRITE_COMMAND_REG_TO_US;
	}

return DD_RETURN_OK;
}






/*!
<b>FUNCTION 0018	: dd_ioc_get_pci_card_irq_number</b>
- Job
	- Read IRQ NUMBER value on the PCI board
	(fake, in fact IRQ number is read on the system PCI info
	chainedlist)
- Inputs
	- struct pointer to a DD_device_info struct.
		- <b>Fields used :</b>
		- ->irq_number
		- ->ioc_container
	- address of a struct pointer to a PCI device (pci_dev**)
	- integer used to know if the function call is internal or
	external to the driver itself
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- (pci_dev**) is given as parameter rather than (pci_dev*),
	in order to stay coherent with function 004.
	- Job is related to the board first identified by (DEVICE_ID,
	VENDOR_ID), Cf. function 0004
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_KS_CANNOT_WRITE_IRQ_NUMBER_TO_US
- Sub-functions calls
	- dd_stack_msg
	- __copy_to_user	(system)
*/
static DD_TYPE_ERROR dd_ioc_get_pci_card_irq_number(struct pci_dev **param_device_ptr, struct dd_ioctl_data * param_ioctl_data, int param_source)
{

DD_TYPE_ERROR lcl_err;


	/*! get irq number from system info */
	glb_irq_number = (*param_device_ptr)->irq;

	/*! is it an internal (from driver) or an external (call via IOCTL) command ? */
	/*! if external, return information to user */
	if (param_source == DD_EXTERNAL_COMMAND)
	{
		lcl_err = __copy_to_user((DD_TYPE_PCI_IRQ_NUMBER *)param_ioctl_data->ioctl_address, &(glb_irq_number), sizeof(DD_TYPE_PCI_IRQ_NUMBER));
		if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_PCI_KS_CANNOT_WRITE_IRQ_NUMBER_TO_US;
	}

return DD_RETURN_OK;
}




/*!
<b>FUNCTION 0019	: dd_ioc_get_pci_card_base_addresses_and_mask</b>
- Job
	- Read BASE ADDRESSES values and related informations
	(mask, ...) on the PCI board
	- Base addresse are, in fact, read on the system PCI info
	chainedlist.
	- Additional informations extracted from Base Addresses are read
	on the board.
- Inputs
	- struct pointer to a DD_device_info struct.
		- <b>Fields used :</b>
		- ->ba_array
		- ->plx_ba_infos
		- ->ioc_container
	- address of a struct pointer to a PCI device (pci_dev**)
	- integer used to know if the function call is internal or
	external to the driver itself
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- (pci_dev**) is given as parameter rather than (pci_dev*),
	in order to stay coherent with function 004.
	- Job is related to the board first identified by (DEVICE_ID,
	VENDOR_ID), Cf. function 0004
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
- Sub-functions calls
	- dd_stack_msg
	- pci_read_config_dword	(system)
	- cli			(system)
	- pci_write_config_dword	(system)
	- sti			(system)
	- __copy_to_user		(system)
*/
static DD_TYPE_ERROR dd_ioc_get_pci_card_base_addresses_and_mask(struct pci_dev **param_device_ptr, struct dd_ioctl_data * param_ioctl_data, int param_source)
{

/*! declarations */

/*! variables de controle */
DD_TYPE_ERROR lcl_err;
int lcl_counter;


	for (lcl_counter = 0 ; lcl_counter < DD_NBR_OF_PCI_BASE_ADDRESSES ; lcl_counter++)
	{
		/*! Recuperation des addresses de base dans la structure PCI systeme */
		/*! set des informations pour la structure PCI */

        		glb_plx_ba_array[lcl_counter] = PCIBASEADDRESS(*param_device_ptr, lcl_counter);
	}

	/*! if external command, return information to user */
	if (param_source == DD_EXTERNAL_COMMAND)
	{
		/*! transfert des donnees vers le user space */
		/*! memoire checkee en debut d'ioctl */
		lcl_err = __copy_to_user((DD_TYPE_PCI_BASE_ADDRESS *)param_ioctl_data->ioctl_address, &(glb_plx_ba_array[0]), (sizeof(glb_plx_ba_array)));
		if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_PCI_KS_CANNOT_WRITE_BA_ARRAY_TO_US;
	}


return DD_RETURN_OK;
}






/*!
<b>FUNCTION 0101	: dd_ioc_set_pci_card_command_reg</b>
- Job
	- Write a 16 bits value to CONTROL register of the PCI board
- Inputs
	- struct pointer to a DD_device_info struct.
		- <b>Fields used :</b>
		- ->command_reg
		- ->ioc_container
	- address of a struct pointer to a PCI device (pci_dev**)
	- integer used to know if the function call is internal or
	external to the driver itself
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- (pci_dev**) is given as parameter rather than (pci_dev*),
	in order to stay coherent with function 004.
	- Job is related to the board first identified by (DEVICE_ID,
	VENDOR_ID), Cf. function 0004
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_KS_CANNOT_READ_CTRL_REGISTER_FROM_US
		- DD_PCI_ERROR_WRITE_CTRL_REGISTER
- Sub-functions calls
	- dd_stack_msg
	- __copy_from_user	(system)
	- pci_write_config_word	(system)
*/
static DD_TYPE_ERROR dd_ioc_set_pci_card_command_reg(struct pci_dev **param_device_ptr, struct dd_ioctl_data * param_ioctl_data, int param_source)
{
DD_TYPE_PCI_COMMAND_REG lcl_command;

DD_TYPE_ERROR lcl_err;

	if (param_source == DD_EXTERNAL_COMMAND)
	{
		/*! memoire checkee en debut d'ioctl */
		/*! get data from user space */
		lcl_err = __copy_from_user(&(lcl_command), (DD_TYPE_PCI_COMMAND_REG *)param_ioctl_data->ioctl_address, sizeof(DD_TYPE_PCI_COMMAND_REG));
		if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_PCI_KS_CANNOT_READ_CTRL_REGISTER_FROM_US;
	}
	else lcl_command = DD_PCI_CTRL_CONFIG;

	/*! write data to hardware */
	lcl_err = pci_write_config_word((*param_device_ptr), PCI_COMMAND, lcl_command);

	if (lcl_err != DD_RETURN_OK) return DD_PCI_ERROR_WRITE_CTRL_REGISTER;

return DD_RETURN_OK;
}










/*!
<b>FUNCTION 0020	: dd_give_handle_to_pci_ioctl</b>
- Job
	- Call one of the previously defined PCI functions according
	to the sent IOCTL command
- Inputs
	- struct pointer to a DD_device_info struct.
	address of a struct pointer to a PCI device (pci_dev**)
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- This function is called from the HUB function
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PCI_COMMAND_NOT_ASSOCIATED
		- sub-functions EVT
- Sub-functions calls
	- dd_stack_msg
	- dd_0001_ioc_set_ident_device				(EVT)
	- dd_0002_ioc_set_ident_vendor				(EVT)
	- dd_ioc_check_if_system_is_pci_compliant		(EVT)
	- dd_ioc_find_pci_card				(EVT)
	- dd_0005_ioc_get_pci_card_device_id			(EVT)
	- dd_0006_ioc_get_pci_card_vendor_id			(EVT)
	- dd_ioc_get_pci_card_command_reg			(EVT)
	- dd_0008_ioc_get_pci_card_status_reg			(EVT)
	- dd_0009_ioc_get_pci_card_revision_id			(EVT)
	- dd_0010_ioc_get_pci_card_lpi_number			(EVT)
	- dd_0011_ioc_get_pci_card_class_number			(EVT)
	- dd_0012_ioc_get_pci_card_cache_size			(EVT)
	- dd_0013_ioc_get_pci_card_latency			(EVT)
	- dd_0014_ioc_get_pci_card_header_type			(EVT)
	- dd_0015_ioc_get_pci_card_bist				(EVT)
	- dd_0016_ioc_get_pci_card_bus_number			(EVT)
	- dd_0017_ioc_get_pci_card_slot_number			(EVT)
	- dd_ioc_get_pci_card_irq_number			(EVT)
	- dd_ioc_get_pci_card_base_addresses_and_mask	(EVT)
	- dd_ioc_set_pci_card_command_reg			(EVT)
*/
static DD_TYPE_ERROR dd_give_handle_to_pci_ioctl(struct pci_dev **param_device_ptr, struct dd_ioctl_data * param_ioctl_data)
{
	/*! Action en fonction de la commande ... */
	switch(param_ioctl_data->ioctl_command)
	{

		case DD_IOC_CHECK_IF_SYSTEM_IS_PCI:
			return dd_ioc_check_if_system_is_pci_compliant();
		break;


		case DD_IOC_FIND_PCI_CARD:
			return dd_ioc_find_pci_card(param_device_ptr);
		break;


		case DD_IOC_GET_PCI_COMMAND_REG:
			return dd_ioc_get_pci_card_command_reg(param_device_ptr, param_ioctl_data, DD_EXTERNAL_COMMAND);
		break;


		case DD_IOC_GET_PCI_IRQ_NUMBER:
			return dd_ioc_get_pci_card_irq_number(param_device_ptr, param_ioctl_data, DD_EXTERNAL_COMMAND);
		break;


		case DD_IOC_GET_PCI_BASE_ADDRESSES_AND_MASKS:
			return dd_ioc_get_pci_card_base_addresses_and_mask(param_device_ptr, param_ioctl_data, DD_EXTERNAL_COMMAND);
		break;


		case DD_IOC_SET_PCI_COMMAND_REG:
			return dd_ioc_set_pci_card_command_reg(param_device_ptr, param_ioctl_data, DD_EXTERNAL_COMMAND);
		break;


		default:
			return DD_PCI_COMMAND_NOT_ASSOCIATED;

	} /*! Fin du switch command */

return DD_RETURN_OK;
}




//////////////////////
///PLX XXX
////////////////////


/*!
<b>FUNCTION 0031	: dd_ioc_set_plx9080_addresses</b>
- Job
	- Remap Base address 0 and Base address 2 of PLX into memory,DD_FEC_WORD
	respectively towards Local0 and plx_ba_infos[ ((PLX_FEC_INDEX
	*DD_PLX_INFOCELL_PER_BA) + DD_PLX_REMAPPED_BA_INDEX) ],
	for further accesses to PLX and FEC.
- Inputs
	- struct pointer to a DD_device_info struct.
		- <b>Fields used :</b>
		- ->plx_ba_infos
- Outputs
	- Error CodeDD_FEC_WORD
- Internals
	- Void
- Notes
	- Nothing
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_PLX_ERROR_REMAPPING_PLX_BA
		- DD_PLX_ERROR_REMAPPING_PLX_BA0
		- DD_PLX_ERROR_REMAPPING_PLX_BA1
		- DD_PLX_ERROR_REMAPPING_PLX_BA2
		- DD_PLX_ERROR_REMAPPING_PLX_BA3
		- DD_PLX_ERROR_REMAPPING_PLX_BA4
		- DD_PLX_ERROR_REMAPPING_PLX_BA5
- Sub-functions calls
	- ioremap		(system)
	- dd_stack_msg
*/
static DD_TYPE_ERROR dd_ioc_set_plx9080_addresses(void)
{
/*! Rem : if you map something here, unmap it in function : dd_release_fecdriver */
int lcl_i;

	for (lcl_i = 0 ; lcl_i < (DD_NBR_OF_PCI_BASE_ADDRESSES) ; lcl_i++)
	{
		if (glb_plx_rmapsize_ba_array[lcl_i] > 0)
		{
	 	       glb_plx_remapped_ba_array[lcl_i] = (DD_TYPE_PCI_BASE_ADDRESS) ioremap( glb_plx_ba_array[lcl_i], glb_plx_rmapsize_ba_array[lcl_i] );
			if ((void *)(glb_plx_remapped_ba_array[lcl_i]) == NULL)
			{
				printk("Error while remapping PLX base address %i\n", lcl_i);
				return DD_PLX_ERROR_REMAPPING_PLX_BA;
			}
		}
	}

return DD_RETURN_OK;
}







/*!
<b>FUNCTION 	: dd_write_to_plx</b>
- Job
	- write an unsigned word to PLX
- Inputs
	- struct pointer to a DD_device_info struct.
		- <b>Fields used :</b>
		- ->plx_offset_and_value
		- ->ioc_container
		- ->plx_ba_infos
	- integer used to know if the function call is internal or
	external to the driver itself
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Nothing
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_KS_CANNOT_READ_FROM_US_FOR_PLX_WRITE
- Sub-functions calls :
	- dd_stack_msg
	- __copy_from_user	(system)
*/
static DD_TYPE_ERROR dd_external_write_to_plx(struct dd_ioctl_data * param_ioctl_data)
{

DD_TYPE_ERROR lcl_err;

	lcl_err = __copy_from_user(param_ioctl_data->plx_array, (DD_PLX_ADDRESS *)param_ioctl_data->ioctl_address, sizeof(DD_PLX_ARRAY));
	if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_KS_CANNOT_READ_FROM_US_FOR_PLX_WRITE;

	/*! ecriture de la valeur dans le registre */
	*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_COMMAND_INDEX] + param_ioctl_data->plx_array[DD_PLX_OFFSET_INDEX]) = param_ioctl_data->plx_array[DD_PLX_VALUE_INDEX];

return DD_RETURN_OK;
}






/*!
<b>FUNCTION 	: dd_read_from_plx</b>
- Job
	- read an unsigned word from PLX
- Inputs
	- struct pointer to a DD_device_info struct.
		- <b>Fields used :</b>
		- ->plx_offset_and_value
		- ->ioc_container
		- ->plx_ba_infos
	- integer used to know if the function call is internal or
	external to the driver itself
- OutputsDD_PLX_BA_ARRAY glb_plx_remapped_ba_array;

	- Error Code
- Internals
	- Void
- Notes
	- Nothing
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_KS_CANNOT_READ_FROM_US_FOR_PLX_READ
		- DD_KS_CANNOT_WRITE_TO_US_FOR_PLX_READ
- Sub-functions calls
	- dd_stack_msg
	- __copy_from_user	(system)
	- __copy_to_user		(system)
*/
static DD_TYPE_ERROR dd_external_read_from_plx(struct dd_ioctl_data * param_ioctl_data)
{

DD_TYPE_ERROR lcl_err;

	/*! get value from user space */
	lcl_err = __copy_from_user(param_ioctl_data->plx_array, (DD_PLX_ADDRESS *)param_ioctl_data->ioctl_address, sizeof(DD_PLX_ARRAY));
	if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_KS_CANNOT_READ_FROM_US_FOR_PLX_READ;


	/*! get value from PLX register */
	param_ioctl_data->plx_array[DD_PLX_VALUE_INDEX] = *(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_COMMAND_INDEX] + param_ioctl_data->plx_array[DD_PLX_OFFSET_INDEX]);

	/*! memoire checkee en debut d'ioctl */
	lcl_err = __copy_to_user((DD_PLX_ADDRESS *)param_ioctl_data->ioctl_address, param_ioctl_data->plx_array, sizeof(DD_PLX_ARRAY));
	if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_KS_CANNOT_WRITE_TO_US_FOR_PLX_READ;

return DD_RETURN_OK;
}




/*
Actually unused function ; coded just in case.
*/
/*
static void dd_internal_read_from_plx(DD_PLX_ADDRESS param_offset, DD_PLX_DATA *param_data)
{

	(*param_data) = *(DD_TYPE_ADDRESS *)((glb_plx_remapped_ba_array[DD_PLX_COMMAND_INDEX] + param_offset));
}
*/

static void dd_internal_write_to_plx(DD_PLX_ADDRESS param_offset, DD_PLX_DATA param_data)
{
	/*! ecriture de la valeur dans le registre */
	*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_COMMAND_INDEX] + param_offset) = param_data;
}




/*!
<b>FUNCTION	: dd_ioc_enable_plx_interrupts</b>
- Job
	- Enable or Re-enable the PLX interrupts line.
- I/O
	- Inputs
		- void
	- Outputs
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- glb_irq_status
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- void
	- Errors :
		- void
- Sub-functions calls
	- dd_internal_write_to_plx
*/
static void dd_ioc_enable_plx_interrupts(void)
{
#ifdef DD_ENABLE_IRQMANAGER
	/* Invalidation des interruptions PLX */
	dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_ENABLED_VALUE);

	/* No need to lock the handle on glb_irq_status here, because the whole
	ioctl() function is locked when called. */
	glb_irq_status = DD_FLAG_IS_ENABLED;
#endif
}




/*!
<b>FUNCTION	: dd_ioc_disable_plx_interrupts</b>
- Job
	- Disable the PLX interrupts line.
- I/O
	- Inputs
		- void
	- Outputs
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- glb_irq_status
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- void
	- Errors :
		- void
- Sub-functions calls
	- dd_internal_write_to_plx
*/
static void dd_ioc_disable_plx_interrupts(void)
{
#ifdef DD_ENABLE_IRQMANAGER
	/* Invalidation des interruptions PLX */
	dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_DISABLED_VALUE);

	/* No need to lock the handle on glb_irq_status here, because the whole
	ioctl() function is locked when called. */
	glb_irq_status = DD_FLAG_IS_DISABLED;
#endif
}






/*!
<b>FUNCTION 0021	: dd_give_handle_to_plx_ioctl</b>
- Job
	- Call one of the previously defined PLX9080 functions
	according to the sent IOCTL command
- Inputs
	- struct pointer to a DD_device_info struct.
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- This function is called from the HUB function
- Error Management & Values returned
- Operation successful :
	- DD_RETURN_OK
	- Errors :
		- DD_PLX_COMMAND_NOT_ASSOCIATEDglb_plx_array[DD_PLX_OFFSET_INDEX]
		- sub-functions EVT
- Sub-functions calls :
	- dd_stack_msg
	- dd_ioc_set_plx9080_addresses		(EVT)
	- dd_write_to_plx			(EVT)
	- dd_read_from_plx			(EVT)
*/

static DD_TYPE_ERROR dd_give_handle_to_plx_ioctl(struct dd_ioctl_data * param_ioctl_data)
{
	switch(param_ioctl_data->ioctl_command)
	{
		case DD_IOC_PLX_SET_ADDRESSES:

			return dd_ioc_set_plx9080_addresses();
		break;

		case DD_IOC_WRITE_TO_PLX:

			return dd_external_write_to_plx(param_ioctl_data);
		break;

		case DD_IOC_READ_FROM_PLX:
			return dd_external_read_from_plx(param_ioctl_data);
		break;

		case DD_IOC_ENABLE_PLX_INTERRUPTS:
			dd_ioc_enable_plx_interrupts();
		break;

		case DD_IOC_PLX_HARD_RESET_MODULE:
			return dd_plx_hard_reset_module();
		break;

		case DD_IOC_DISABLE_PLX_INTERRUPTS:
			dd_ioc_disable_plx_interrupts();
		break;


		default:
			return DD_PLX_COMMAND_NOT_ASSOCIATED;

	} /*! Fin du switch command */


return DD_RETURN_OK;
}




///////////////////
//FECRELATED XXX
////////////////////


static void dd_internal_write_to_fec_register(DD_FEC_OFFSET param_offset, DD_FEC_REGISTER_DATA param_value)
{
	/*! ecriture de la valeur dans le registre du FEC */
	*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_offset) = param_value;
}


static void dd_internal_read_from_fec_register(DD_FEC_OFFSET param_offset, DD_FEC_REGISTER_DATA *param_value)
{
	/*! Lecture de la valeur dans le registre du FEC */
	(*param_value) = *(DD_TYPE_ADDRESS *)((glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_offset));
}


static DD_TYPE_ERROR dd_external_write_to_fec_register(struct dd_ioctl_data * param_ioctl_data)
{

DD_TYPE_ERROR lcl_err;

		/*! get data from user space */
	lcl_err = __copy_from_user(param_ioctl_data->fec_array, (DD_FEC_UNIFIED_DATA *)param_ioctl_data->ioctl_address, sizeof(DD_FEC_ARRAY));
	if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_WRITE;


	/*! ecriture de la valeur dans le registre du FEC */
	*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX]) = (DD_FEC_REGISTER_DATA)param_ioctl_data->fec_array[DD_FEC_VALUE_INDEX];

return DD_RETURN_OK;
}



static DD_TYPE_ERROR dd_external_read_from_fec_register(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;

	/*! get data from user space */
	lcl_err = __copy_from_user(param_ioctl_data->fec_array, (DD_FEC_UNIFIED_DATA *)param_ioctl_data->ioctl_address, sizeof(DD_FEC_ARRAY));
	if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_READ;

	/*! get value from FEC register */
	param_ioctl_data->fec_array[DD_FEC_VALUE_INDEX] = (DD_FEC_REGISTER_DATA)*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX]);

	lcl_err = __copy_to_user((DD_FEC_UNIFIED_DATA *)param_ioctl_data->ioctl_address, param_ioctl_data->fec_array, sizeof(DD_FEC_ARRAY));
	if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_CANNOT_WRITE_VALUE_TO_US_FOR_FEC_READ;
return DD_RETURN_OK;
}







static void dd_internal_write_to_fec_fifo(DD_FEC_OFFSET param_offset, DD_FEC_UNIFIED_DATA param_value)
{
//printk("dd_internal_write_to_fec_fifo called\n");
	/*! ecriture de la valeur dans le registre du FEC */
//	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
	if (glb_fec_type == 0)
	{
		*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_offset) = (DD_FEC_FIFO_DATA_16)param_value;
	}
	else *(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_offset) = (DD_FEC_FIFO_DATA_32)param_value;

}


/*
static void dd_internal_write_to_fec_fifo_32(DD_FEC_OFFSET param_offset, DD_FEC_FIFO_DATA_32 param_value)
{
	// ecriture de la valeur dans le registre du FEC
	*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_offset) = param_value;
}
*/


/*
static void dd_internal_write_to_fec_fifo_32(DD_FEC_OFFSET param_offset, DD_FEC_FIFO_DATA_32 param_value)
{
	//ecriture de la valeur dans le registre du FEC
	*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_offset) = param_value;
}
*/

/*
static void dd_internal_read_from_fec_fifo(DD_FEC_OFFSET param_offset, DD_FEC_FIFO_DATA *param_value)
{
	// Lecture de la valeur dans le registre du FEC
	(*param_value) = *(DD_TYPE_ADDRESS *)((glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)param_offset));
}
*/

static DD_TYPE_ERROR dd_external_write_to_fec_fifo(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;
DD_FEC_OFFSET lcl_offset=0;
//DD_FEC_OFFSET lcl_fec_fiforec_offset;
//DD_FEC_OFFSET lcl_fec_fiforet_offset;

//printk("dd_external_write_to_fec_fifo called\n");

		/*! get data from user space */
	lcl_err = __copy_from_user(param_ioctl_data->fec_array, (DD_FEC_UNIFIED_DATA *)param_ioctl_data->ioctl_address, sizeof(DD_FEC_ARRAY));
	if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_WRITE;

	/* Cast FIFOs offsets according to the kind of FEC we are working with */
//	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
//	if (glb_fec_type == 0)
//	{
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFOTRA_OFFSET) lcl_offset=dd_fec_fifotra_offset;
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFOREC_OFFSET) lcl_offset=dd_fec_fiforec_offset;
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFORET_OFFSET) lcl_offset=dd_fec_fiforet_offset;
//	}
/*	else
	{
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFOTRA_OFFSET) lcl_offset=DD_FEC_FIFOTRA_OFFSET_OV1;
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFOREC_OFFSET) lcl_offset=DD_FEC_FIFOREC_OFFSET_OV1;
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFORET_OFFSET) lcl_offset=DD_FEC_FIFORET_OFFSET_OV1;
	}

*/

	/*! ecriture de la valeur dans le registre du FEC */
//	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
	if (glb_fec_type == 0)
	{
		*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)lcl_offset) =(DD_FEC_FIFO_DATA_16)param_ioctl_data->fec_array[DD_FEC_VALUE_INDEX];
	}
	else *(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)lcl_offset)=(DD_FEC_FIFO_DATA_32)param_ioctl_data->fec_array[DD_FEC_VALUE_INDEX];
return DD_RETURN_OK;
}



static DD_TYPE_ERROR dd_external_read_from_fec_fifo(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;
DD_FEC_OFFSET lcl_offset=0;
//printk("dd_external_read_from_fec_fifo called\n");
	/*! get data from user space */
	lcl_err = __copy_from_user(param_ioctl_data->fec_array, (DD_FEC_UNIFIED_DATA *)param_ioctl_data->ioctl_address, sizeof(DD_FEC_ARRAY));
	if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_READ;


	/* Cast FIFOs offsets according to the kind of FEC we are working with */
//	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
//	if (glb_fec_type == 0)
//	{
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFOTRA_OFFSET) lcl_offset=dd_fec_fifotra_offset;
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFOREC_OFFSET) lcl_offset=dd_fec_fiforec_offset;
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFORET_OFFSET) lcl_offset=dd_fec_fiforet_offset;
//	}
/*	else
	{
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFOTRA_OFFSET) lcl_offset=DD_FEC_FIFOTRA_OFFSET_OV1;
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFOREC_OFFSET) lcl_offset=DD_FEC_FIFOREC_OFFSET_OV1;
		if (param_ioctl_data->fec_array[DD_FEC_OFFSET_INDEX] == DD_FEC_FIFORET_OFFSET) lcl_offset=DD_FEC_FIFORET_OFFSET_OV1;
	}
*/

	/*! get value from FEC register */
//	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
	if (glb_fec_type == 0)
	{
		param_ioctl_data->fec_array[DD_FEC_VALUE_INDEX] = (DD_FEC_FIFO_DATA_16)*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)lcl_offset);
	}
	else param_ioctl_data->fec_array[DD_FEC_VALUE_INDEX] = (DD_FEC_FIFO_DATA_32)*(DD_TYPE_ADDRESS *)(glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)lcl_offset);
	
	lcl_err = __copy_to_user((DD_FEC_UNIFIED_DATA *)param_ioctl_data->ioctl_address, param_ioctl_data->fec_array, sizeof(DD_FEC_ARRAY));
	if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_CANNOT_WRITE_VALUE_TO_US_FOR_FEC_READ;
return DD_RETURN_OK;
}




/*!
<b>FUNCTION : dd_free_trans_number_on_timeout</b>
- Job
	Free transaction number if transaction failed on timeout
- Inputs
	- unsigned 16, transaction number to free
- Outputs
	- Void
- Internals
	- Write in global var : glb_device_info.fec_write_trans_in_use[]
- Notes
	- Internal function
- Error Management & Values returned
	- Void
- Sub-functions calls
	- dd_stack_msg
*/


static void dd_free_trans_number_on_timeout(DD_FEC_STD_WORD param_trans)
{
	glb_fec_write_trans_in_use[param_trans] = DD_TRANS_NUMBER_IS_FREE;

}




/*!
<b>FUNCTION 0022	:	dd_fec_write_msg</b>
- Job
	- Write user data in fifo transmit, send to ring, wait for ACK
	and return assigned transaction number.
	- If the (fake) Transaction number of the frame sent to this function
	is setted to DD_FAKE_TNUM_PROVOKE_ACK, then the driver will wait for an acknowledge frame
	sent back by the I2C devices, the F-ACK frame.
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->error_in_interrupt
		- ->ioc_container
		- ->currentTrans
		- ->fec_write_trans_in_use[]
		- ->fec_trans_number_timers[]
		- ->read[]
		- ->copied[]
		- ->f_ack[]
		- ->back[][]
		- ->received[][]
		- ->send[][]
		- ->fec_offset_and_value[]
		- ->trans_is_read_op[]
		- ->write_wait[]
		- ->back[]
		- ->fec_trans_must_be_f_acked[]
- Outputs
	- Error Code
- Internals
	- global var glb_atomic_flags accessed R/W
- Notes
	- Void
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
- Sub-functions calls
	- dd_stack_msg
	- __copy_from_user		(system)
	- access_ok			(system)
	- sleep_on_timeout		(system)
	- test_and_set_bit		(system)
	- clear_bit			(system)
	- add_timer			(system)
	- del_timer			(system)
	- dd_write_to_fec			(EVT)
	- interruptible_sleep_on_timeout	(system)
	- __copy_to_user			(system)
*/

static DD_TYPE_ERROR dd_fec_write_msg(struct dd_ioctl_data * param_ioctl_data)
{

//#define DEBUG_dd_fec_write_msg

DD_TYPE_ERROR lcl_err;
DD_FEC_STD_WORD lcl_trans_number, lcl_trans_number_offset;
DD_FEC_REGISTER_DATA lcl_fec_value;
DD_FEC_REGISTER_DATA lcl_fec_ctrl0_memory;
DD_FEC_STD_WORD lcl_buf[DD_USER_MAX_MSG_LENGTH];
int lcl_i,lcl_j,lcl_counter,lcl_count, lcl_count_out;
/*! local Wait Semaphore */
WAIT_QUEUE(lcl_wait);
int lcl_ctrl_loop;

/* lcl_trans MUST remains as int ; it's compared to
DD_INVALID_TRANSACTION_NUMBER_EXAMPLE which is -1 */
int lcl_trans;
//#ifdef IS_FIFO_32BITS
DD_FEC_FIFO_DATA_32 lcl_buf32[DD_MAX_MSG_LENGTH_32];
int lcl_count32;
//#endif
DD_FEC_UNIFIED_DATA lcl_fifo_word;



	/*! Copy data frame from user space */
	/*! First, copy the 4 first bits in order to get the frame length */
	lcl_err = (DD_TYPE_ERROR)__copy_from_user(&lcl_buf[0], (DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, (sizeof(DD_FEC_STD_WORD) * DD_FRAME_HEAD_LENGTH_IF_LONGFRAME));
	if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_KS_CANNOT_READ_FRAME_FROM_US_FOR_WRITE_FRAME;

	/* Now, compute frame length */
	/* If lenght is less than or equal to 127, than just copy frame length */
	/* Else, compute real frame length. */
	if (isLongFrame(lcl_buf[DD_MSG_LEN_OFFSET]))
	{
		lcl_count = ((((lcl_buf[DD_MSG_LEN_OFFSET] & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + lcl_buf[DD_MSG_LEN_OFFSET+1]) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		lcl_trans_number_offset = DD_TRANS_NUMBER_OFFSET+1;
	}
	else
	{
		lcl_count = (lcl_buf[DD_MSG_LEN_OFFSET] + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);
		lcl_trans_number_offset = DD_TRANS_NUMBER_OFFSET;
	}

	#ifdef DD_SPY_DEBUG
		printk("I see a frame of total length: 0x%x\n",lcl_count);
	#endif

	/*! get frame length and check consistency */
	if (lcl_count > DD_USER_MAX_MSG_LENGTH) return DD_TOO_LONG_FRAME_LENGTH;
	/*! Rem : no check performed on min length of the frame ; */
	/*! if needed, this check can be coded here. */

	/*! Now, get the whole frame in stack. */
	lcl_err = (DD_TYPE_ERROR)__copy_from_user(&lcl_buf[0], (DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, (sizeof(DD_FEC_STD_WORD) * lcl_count));
	if (lcl_err != DD_UUCOPY_FROM_USER_OK) return DD_KS_CANNOT_READ_FRAME_FROM_US_FOR_WRITE_FRAME;


	/*! Can we access the transaction number manager ? If no, */
	/*! how many times will we go to sleep before returning an error ? */
	lcl_counter = 0;
	while ( (glb_atomic_flags & DD_TRANS_NUMBER_MNGR_IN_USE) && (lcl_counter < DD_NBR_OF_RETRY_IF_TRANS_NUMBER_MNGR_BUSY) )
	/*! transaction number manager is already used by another write call */
	{
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	       		wait_event_timeout(lcl_wait, 0, DD_WAIT_TIMEOUT_IF_TRANS_NUMBER_MNGR_BUSY);
		#else
	       		sleep_on_timeout((&lcl_wait), DD_WAIT_TIMEOUT_IF_TRANS_NUMBER_MNGR_BUSY);		
		#endif
		lcl_counter++;
	}
	/*! exit loop when !(glb_atomic_flags & DD_TRANS_NUMBER_MNGR_IN_USE) OR if counter set*/

	/*! too much tries ; exit */
	if (lcl_counter >= DD_NBR_OF_RETRY_IF_TRANS_NUMBER_MNGR_BUSY) return DD_TIMEOUT_TRANS_NUMBER_MNGR_IN_USE;


	/*! set flag for atomic operation on transaction number manager */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	       	lcl_i = test_and_set_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, (void *)&glb_atomic_flags);
	#else
	       	lcl_i = test_and_set_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, &glb_atomic_flags);
	#endif



	/*! if we have set a flag previously setted, exit */
	if (lcl_i != DD_FLAG_WAS_NOT_SET) return DD_TIMEOUT_TRANS_NUMBER_MNGR_IN_USE;
	/*! now, our flag has been set safely */



	/*! search the first free transaction number occuring after the last used one.*/
	/*! First, keep memory of last used transaction number */
	lcl_i = glb_currentTrans;

	/*! fake invalid transaction number for loop control */
	lcl_trans = DD_INVALID_TRANSACTION_NUMBER_EXAMPLE;

	/*! search */
	do
	{
		/*! point to next of last used transaction number */
		lcl_i++;
		/*! if out of range, loop as circular buffer */
		if (lcl_i > DD_MAX_TRANS_NUMBER) { lcl_i = DD_MIN_TRANS_NUMBER;}
		/*! check if slot is free */
		if (glb_fec_write_trans_in_use[lcl_i] == DD_TRANS_NUMBER_IS_FREE) {lcl_trans=(DD_FEC_STD_WORD)lcl_i;}
		/*! if one full loop was performed and no free transaction was found, force exit */
		if (lcl_i == glb_currentTrans) {lcl_trans=(DD_FEC_STD_WORD)lcl_i;}

	} while (lcl_trans == DD_INVALID_TRANSACTION_NUMBER_EXAMPLE);


	/*! if no free transaction number found, return an error */
	/*! if the only free transaction number found is the last used one, return an error too */
	if (lcl_trans == glb_currentTrans)
	{
		/*! clear flag for end of atomic operation */
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		       	clear_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, (void *)&glb_atomic_flags);
		#else
		       	clear_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, &glb_atomic_flags);
		#endif
		return DD_NO_MORE_FREE_TRANSACTION_NUMBER;
	}

	/*! if one free transaction number is found, tag this number as currently used */
	glb_fec_write_trans_in_use[lcl_trans] = DD_TRANS_NUMBER_IS_IN_USE;

        /*! init and launch a timer for this transaction */
	init_timer(&(glb_fec_trans_number_timers[lcl_trans]));
	(glb_fec_trans_number_timers[lcl_trans]).function = (void *)dd_free_trans_number_on_timeout;
	(glb_fec_trans_number_timers[lcl_trans]).data=(int)lcl_trans;
	(glb_fec_trans_number_timers[lcl_trans]).expires = jiffies + DD_FREE_TRANS_DELAY;
	add_timer(&(glb_fec_trans_number_timers[lcl_trans]));

	/*! switch to unsigned 16 value */
	lcl_trans_number = lcl_trans;

	/*! keep memory of last used number */
	glb_currentTrans = lcl_trans_number;

	/*! clear flag for end of atomic operation */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, &glb_atomic_flags);
	#endif




	/*! RAZ indicateurs pour cette transaction */
	glb_read[lcl_trans] = DD_READ_DATA_UNSUCCESSFUL;
	glb_copied[lcl_trans] = DD_COPY_DATA_UNSUCCESSFUL;

	for (lcl_j=0 ; lcl_j < DD_USER_MAX_MSG_LENGTH ; lcl_j++)
	{
		glb_back[lcl_trans][lcl_j] = DD_BACK_STACK_INIT_VALUE;
		glb_received[lcl_trans][lcl_j] = DD_RECEIVED_STACK_INIT_VALUE;
	}


	/*! check, according to Fake Transaction number of frame, if F-ACK is awaited. */
	/*! if fake trans number is DD_FAKE_TNUM_PROVOKE_ACK, then F-ACK is requested. */
	/*! else, it is not. */
	if (lcl_buf[lcl_trans_number_offset] == DD_FAKE_TNUM_PROVOKE_ACK)
	{
		glb_fec_trans_must_be_f_acked[lcl_trans] = 1;
	}
	else glb_fec_trans_must_be_f_acked[lcl_trans] = 0;


	/*! set transaction number in frame */
	/*! Rem : frame previously read from user space directly in lcl_buf */
	lcl_buf[lcl_trans_number_offset] = lcl_trans;


	/*! Set EOF flag for last data of fifo transmit */
	#ifdef DD_SET_AUTO_EOF_BIT
		lcl_buf[lcl_count-1] = (lcl_buf[lcl_count-1] | DD_EOF_BITMASK);
	#endif




	/* Check if "fifo transmit running" flag  is down before writing in fifo */
	dd_internal_read_from_fec_register(DD_FEC_STAT0_OFFSET, &lcl_fec_value);
	lcl_ctrl_loop=0;
	while ( ((lcl_fec_value & DD_FEC_TRA_RUN)==1) && (lcl_ctrl_loop<DD_LOOP_LIMIT_WHEN_POLLING_FIFOTRA_RUNNING))
	{
		udelay(DD_INLOOP_DELAY_WHEN_POLLING_FIFOTRA_RUNNING);
        	dd_internal_read_from_fec_register(DD_FEC_STAT0_OFFSET, &lcl_fec_value);
        	lcl_ctrl_loop++;
	}
	if (lcl_ctrl_loop >= DD_LOOP_LIMIT_WHEN_POLLING_FIFOTRA_RUNNING)
	{
		printk("PROBLEM BEFORE FILLING FIFOTRA : FIFO TRANSMIT FLAG IS ALWAYS ON.\n");
		return DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON;
	}



	#ifdef DD_SPY_DEBUG
		printk("I will put in fifo :\n");
	#endif


	//#ifdef IS_FIFO_32BITS
//	if (glb_fec_type == 2)
	if (glb_fec_type != 0)
	{
		#ifdef DEBUG_dd_fec_write_msg
			printk("Before call to std->32 converter.\n");
			printk("    lcl_count is : %d\n",lcl_count);
			printk("    lcl_count32 is : %d\n",lcl_count32);
			printk("    transaction number is : %d\n",lcl_trans);
			printk("    size in bytes of lcl_buf is : %d\n",sizeof(lcl_buf));
			printk("    content of lcl_buf is :\n");
			for (lcl_i = 0 ; lcl_i < lcl_count ; lcl_i++)
			{
				printk("        0x%x\n",lcl_buf[lcl_i]);
			}
		#endif


		/*! convert from 16 bits to 32 bits word */
		dd_cat_from_std_to_32(lcl_buf, lcl_buf32, lcl_count, &lcl_count32);


		#ifdef DEBUG_dd_fec_write_msg
			printk("\n\nAfter call to std->32 converter.\n");
			printk("    lcl_count is : %d\n",lcl_count);
			printk("    lcl_count32 is : %d\n",lcl_count32);
			printk("    content of lcl_buf is :\n");
			for (lcl_i = 0 ; lcl_i < lcl_count ; lcl_i++)
			{
				printk("        0x%x\n",lcl_buf[lcl_i]);
			}
			printk("    content of lcl_buf32 is :\n");
			for (lcl_i = 0 ; lcl_i < lcl_count32 ; lcl_i++)
			{
				printk("        0x%x\n",lcl_buf32[lcl_i]);
			}
		#endif


		/*! copy send buffer to fifo transmit */
		for (lcl_j = 0 ; lcl_j < lcl_count32 ; lcl_j++)
		{
			lcl_fifo_word = lcl_buf32[lcl_j];
			dd_internal_write_to_fec_fifo(dd_fec_fifotra_offset, lcl_fifo_word);
			#ifdef DD_SPY_DEBUG
				printk("\t0x%x\n",lcl_buf32[lcl_j]);
			#endif
		}

	}
	//#else
	else
	{
		#ifdef DD_SPY_DEBUG
			printk("fifotra offset to use is : 0x%x\n",dd_fec_fifotra_offset);
		#endif
		
		/* copy send buffer to fifo transmit */
		for (lcl_j = 0 ; lcl_j < lcl_count ; lcl_j++)
		{
			lcl_fifo_word = lcl_buf[lcl_j];
			dd_internal_write_to_fec_fifo(dd_fec_fifotra_offset, lcl_fifo_word);
			#ifdef DD_SPY_DEBUG
				printk("\t0x%x\n",lcl_buf[lcl_j]);
			#endif
		}
	}
	//#endif


	/* Check if "fifo empty" flag is down (it should, we just wrote data in it) */
	dd_internal_read_from_fec_register(DD_FEC_STAT0_OFFSET, &lcl_fec_value);
	lcl_ctrl_loop=0;
	if ((lcl_fec_value & DD_FEC_TRA_EMPTY)==1)
	{
		printk("PROBLEM : FIFOTRA IS EMPTY, AND IT SHOULD BE FILLED WITH THE FRAME TO SEND.\n");
		return DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON;
		
	}

	/*! Read current CTRL0 value */
	dd_internal_read_from_fec_register(DD_FEC_CTRL0_OFFSET, &lcl_fec_value);
	/*! keep memory */
	lcl_fec_ctrl0_memory = lcl_fec_value;

	/*! Make OR */
	lcl_fec_value = (lcl_fec_ctrl0_memory | DD_FEC_ENABLE_FEC );
	/*! send fifotransmit to ring - step 1/3 */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, lcl_fec_value);

	/*! Make OR */
	lcl_fec_value = (lcl_fec_ctrl0_memory | DD_FEC_ENABLE_FEC | DD_FEC_SEND);
	/*! send fifotransmit to ring - step 2/3 */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, lcl_fec_value);

	/*! Make OR */
	lcl_fec_value = (lcl_fec_ctrl0_memory | DD_FEC_ENABLE_FEC);
	/*! send fifotransmit to ring - step 1/3 */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, lcl_fec_value);


	if (glb_copied[lcl_trans] == DD_COPY_DATA_UNSUCCESSFUL)
	{
		/* this block, if triggered, runs a full transaction in 880 micro seconds on my PC */
        	interruptible_sleep_on_timeout(&glb_wq_write, DD_TIMEOUT_WAIT_FOR_WRITE_ACK);
	}

	/* Check if "fifo transmit running" flag  is down before writing in fifo */
	dd_internal_read_from_fec_register(DD_FEC_STAT0_OFFSET, &lcl_fec_value);
	if ((lcl_fec_value & DD_FEC_TRA_RUN)==1)
	{
		printk("PROBLEM SENDING FRAME : FIFO TRANSMIT FLAG IS ALWAYS ON.\n");
		printk("The frame sent on the ring was :\n");		
		if (glb_fec_type != 0)
		{
			for (lcl_j = 0 ; lcl_j < lcl_count32 ; lcl_j++)
			{
				printk("\t0x%x\n",lcl_buf32[lcl_j]);
			}
		}
		else
		{
			for (lcl_j = 0 ; lcl_j < lcl_count ; lcl_j++)
			{
				printk("\t0x%x\n",lcl_buf[lcl_j]);
			}
		}
		return DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON;
	}

	/*! if backframe was successful, i.e. Adr. Seen. & Data Copied */
	if ( glb_copied[lcl_trans] == DD_COPY_DATA_SUCCESSFUL )
	{
		/* Now, compute frame length */
		/* If lenght is less than or equal to 127, than just copy frame length */
		/* Else, compute real frame length. */
		if (isLongFrame(glb_back[lcl_trans][DD_MSG_LEN_OFFSET]))
		{
			lcl_count_out = ((((glb_back[lcl_trans][DD_MSG_LEN_OFFSET] & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + glb_back[lcl_trans][DD_MSG_LEN_OFFSET+1]) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		}
		else
		{
			lcl_count_out = (glb_back[lcl_trans][DD_MSG_LEN_OFFSET] + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);
		}

		/* Security check ; maybe not necessary, already performed in IRQ, but who knows... */
		if (lcl_count_out > DD_USER_MAX_MSG_LENGTH) return DD_TOO_LONG_FRAME_LENGTH;


		/*! do we wait for more data on the same transaction number?
		(data request or forced-ack) ? If no, free transaction number.
		If yes, keep a hand on transaction number */
		if (glb_fec_trans_must_be_f_acked[lcl_trans]==0)
		{
			del_timer(&(glb_fec_trans_number_timers[lcl_trans]));
			glb_fec_write_trans_in_use[lcl_trans] = DD_TRANS_NUMBER_IS_FREE;
		}

		/*! send data to user space */
		lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(glb_back[lcl_trans][0]), (sizeof(DD_FEC_STD_WORD)*lcl_count_out));
		if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1;

		return DD_RETURN_OK;
	}


	/*! if backframe was NOT successful, i.e. Adr._NOT_Seen. OR Data_NOT_Copied */
	if ( glb_copied[lcl_trans] == DD_COPY_DATA_CORRUPTED )
	{
		/*! did not worked, but we have had a returned frame */
		if (isLongFrame(glb_back[lcl_trans][DD_MSG_LEN_OFFSET]))
		{
			lcl_count_out = ((((glb_back[lcl_trans][DD_MSG_LEN_OFFSET] & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + glb_back[lcl_trans][DD_MSG_LEN_OFFSET+1]) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		}
		else
		{
			lcl_count_out = (glb_back[lcl_trans][DD_MSG_LEN_OFFSET] + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);
		}

		/* Security check ; maybe not necessary, already performed in IRQ,  but who knows... */

		/* Hmmmm... Rather try to return the max infos given by the received
		corrupted frame. Furthermore, this will force the clearing of the timer
		and the freeing of the transaction number used in this frame.
		Next line has been commented, Next-Next line has been added. */
		//if (lcl_count_out > DD_USER_MAX_MSG_LENGTH) return DD_TOO_LONG_FRAME_LENGTH;
		if (lcl_count_out > DD_USER_MAX_MSG_LENGTH) lcl_count_out = DD_USER_MAX_MSG_LENGTH;


		/*! Anyway, free transaction number */
		del_timer(&(glb_fec_trans_number_timers[lcl_trans]));
		glb_fec_write_trans_in_use[lcl_trans] = DD_TRANS_NUMBER_IS_FREE;

		/*! send error data to user space */
		lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(glb_back[lcl_trans][0]), (sizeof(DD_FEC_STD_WORD)*lcl_count_out));
		if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS2;

		return DD_DATA_CORRUPT_ON_WRITE;
	}

	/*! if backframe overflowed Driver intyernal buffer */
	if ( glb_copied[lcl_trans] == DD_COPY_DATA_OVERFLOWED )
	{
		/*! Set usable frame length to max */
		lcl_count_out = DD_USER_MAX_MSG_LENGTH;

		/*! Anyway, free transaction number */
		del_timer(&(glb_fec_trans_number_timers[lcl_trans]));
		glb_fec_write_trans_in_use[lcl_trans] = DD_TRANS_NUMBER_IS_FREE;

		/*! send error data to user space */
		lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(glb_back[lcl_trans][0]), (sizeof(DD_FEC_STD_WORD)*lcl_count_out));
		if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS2;

		return DD_DATA_OVERFLOW;
	}

	/*! did not worked at all, exit occured on timeout */
	if (glb_copied[lcl_trans] == DD_COPY_DATA_UNSUCCESSFUL)
	{
		glb_copied[lcl_trans] = DD_COPY_DATA_TIMED_OUT;
		/*! free transaction number */
		del_timer(&(glb_fec_trans_number_timers[lcl_trans]));
		glb_fec_write_trans_in_use[lcl_trans] = DD_TRANS_NUMBER_IS_FREE;
		return DD_WRITE_OPERATION_FAILED;
	}
return DD_EXIT_WRITE_METHOD_ON_UNSTABLE_CONDITION;
}







/*!
<b>FUNCTION	: dd_fec_read_msg</b>
- Job
	- Read data stored from fifo receive
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->error_in_interrupt
		- ->fec_current_read_process_waiting
		- ->ioc_container
		- ->read[]
		- ->read_wait[]
		- ->received[][]
- Outputs
	- Error Code
- Internals
	- global var glb_atomic_flags accessed R/W
- Notes
	- Void
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
- Sub-functions calls
	- dd_stack_msg
	- sleep_on_timeout		(system)
	- test_and_set_bit		(system)
	- clear_bit			(system)
	- __copy_from_user		(system)
	- interruptible_sleep_on_timeout	(system)
	- __copy_to_user			(system)
*/

static DD_TYPE_ERROR dd_fec_read_msg(struct dd_ioctl_data * param_ioctl_data)
{

DD_TYPE_ERROR lcl_err;
DD_FEC_STD_WORD lcl_trans;
int lcl_j;
int lcl_count;
int lcl_counter;

/*! local Wait Semaphore */
WAIT_QUEUE(lcl_wait);

	/*! do we still have free system ressources for one more read operation ? */
	/*! check counter consistency */
	if ((glb_fec_current_read_process_waiting < 0) || (glb_fec_current_read_process_waiting > DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS)) return DD_FLAG_READ_COUNTER_UNCONSISTENT;

	/*! try to take a handle on the read manager */
	lcl_counter=0;
	while ( (glb_atomic_flags & DD_READ_REQUEST_IN_USE) && (lcl_counter < DD_NBR_OF_RETRY_IF_READ_REQUEST_BUSY) )
	/*! read request is already used by another read call */
	{
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			wait_event_timeout(lcl_wait, 0, DD_WAIT_TIMEOUT_IF_READ_REQUEST_BUSY);
		#else
			sleep_on_timeout((&lcl_wait), DD_WAIT_TIMEOUT_IF_READ_REQUEST_BUSY);
		#endif
		lcl_counter++;
	}
	/*! exit loop when !(glb_atomic_flags & DD_READ_REQUEST_IN_USE) OR if counter set*/


	/*! too much tries ; exit */
	if (lcl_counter >= DD_NBR_OF_RETRY_IF_READ_REQUEST_BUSY) return DD_TIMEOUT_READ_REQUEST_IN_USE;


	/*! set and test flag for atomic operation on read method increment */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		lcl_j = test_and_set_bit(DD_READ_REQUEST_FLAG, (void *)&glb_atomic_flags);
	#else
		lcl_j = test_and_set_bit(DD_READ_REQUEST_FLAG, &glb_atomic_flags);
	#endif

	

	/*! if we have set a flag previously setted, exit */
	if (lcl_j != DD_FLAG_WAS_NOT_SET) return DD_TIMEOUT_READ_REQUEST_IN_USE;
	/*! now, our flag has been set safely */

	/*! take decision according to counter value */
	/*! if too much read transactions are already waiting, exit */
	if (glb_fec_current_read_process_waiting == DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS)
	{
		/*! release flag and exit */
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			clear_bit(DD_READ_REQUEST_FLAG, (void *)&glb_atomic_flags);
		#else
			clear_bit(DD_READ_REQUEST_FLAG, &glb_atomic_flags);
		#endif

		
		return DD_TOO_MUCH_STACKED_READ_REQUESTS;
	}
	/*! else, increment the number of currently processed read transactions and release flag */
	glb_fec_current_read_process_waiting++;
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_READ_REQUEST_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_READ_REQUEST_FLAG, &glb_atomic_flags);
	#endif

	


	/*! Recup n transaction */
	lcl_err = __copy_from_user(&lcl_trans, (DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, sizeof(DD_FEC_STD_WORD));
	if (lcl_err != DD_RETURN_OK)
	{
		/*! decrement the number of currently processed read transactions before exit */
		glb_fec_current_read_process_waiting--;
		return DD_KS_CANNOT_READ_FROM_US_FOR_READ_FRAME;
	}

	/*! check transaction number consistency */
	if ( (lcl_trans < DD_MIN_TRANS_NUMBER) || (lcl_trans > DD_MAX_TRANS_NUMBER))
	{
		/*! decrement the number of currently processed read transactions before exit */
		glb_fec_current_read_process_waiting--;
		return DD_BAD_TRANSACTION_NUMBER_SENT_TO_READ;
	}

	/*! First try */
	if (glb_read[lcl_trans] == DD_READ_DATA_UNSUCCESSFUL)
	{
			interruptible_sleep_on_timeout(&glb_wq_read, DD_TIMEOUT_WAIT_FOR_READ_ACK);
	}
	/*! get out of here on IRQ (data received) or timeout */


	/*! Anyway, delete watchdog timer ; if data arrives on late, just consider it is lost for user.*/
	del_timer(&glb_fec_trans_number_timers[lcl_trans]);
	/* free transaction number */
	glb_fec_write_trans_in_use[lcl_trans] = DD_TRANS_NUMBER_IS_FREE;



	/*! Is read un-successful again ? */
	/*! if yes, return an error */
	if (glb_read[lcl_trans] == DD_READ_DATA_UNSUCCESSFUL)
	{
		glb_read[lcl_trans] = DD_READ_DATA_TIMED_OUT;
		/*! decrement the number of currently processed read transactions before exit */
		glb_fec_current_read_process_waiting--;
		return DD_CANNOT_READ_DATA;
	}


	 /*! if read was partially successful */
	if (glb_read[lcl_trans] == DD_READ_DATA_CORRUPTED)
        {
		if (isLongFrame(glb_received[lcl_trans][DD_MSG_LEN_OFFSET]))
		{
			lcl_count = ((((glb_received[lcl_trans][DD_MSG_LEN_OFFSET] & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + glb_received[lcl_trans][DD_MSG_LEN_OFFSET+1]) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		}
		else
		{
			lcl_count = (glb_received[lcl_trans][DD_MSG_LEN_OFFSET] + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);
		}

		/* Security check ; maybe not necessary, already performed in IRQ, but who knows... */
		/* Hmmmm... Rather try to return the max infos given by the received
		corrupted frame. Furthermore, this will force the clearing of the timer
		and the freeing of the transaction number used in this frame.
		Next line has been commented, Next-Next line has been added. */
//		if (lcl_count > DD_USER_MAX_MSG_LENGTH) return DD_TOO_LONG_FRAME_LENGTH;
		if (lcl_count > DD_USER_MAX_MSG_LENGTH) lcl_count = DD_USER_MAX_MSG_LENGTH;

		lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(glb_received[lcl_trans][0]), (sizeof(DD_FEC_STD_WORD) * lcl_count));
		/*! decrement the number of currently processed read transactions before exit */
		glb_fec_current_read_process_waiting--;
		if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_TO_US_FOR_READ_FRAME;

		return DD_READ_FRAME_CORRUPTED;
	}


	 /*! if read was partially successful */
	if (glb_read[lcl_trans] == DD_READ_DATA_OVERFLOWED)
        {

		lcl_count = DD_USER_MAX_MSG_LENGTH;

		lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(glb_received[lcl_trans][0]), (sizeof(DD_FEC_STD_WORD) * lcl_count));
		/*! decrement the number of currently processed read transactions before exit */
		glb_fec_current_read_process_waiting--;
		if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_TO_US_FOR_READ_FRAME;

		return DD_DATA_OVERFLOW;
	}


	/*! if read was fully successful */


	if (glb_read[lcl_trans] == DD_READ_DATA_SUCCESSFUL)
        {
		if (isLongFrame(glb_received[lcl_trans][DD_MSG_LEN_OFFSET]))
		{
			lcl_count = ((((glb_received[lcl_trans][DD_MSG_LEN_OFFSET] & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + glb_received[lcl_trans][DD_MSG_LEN_OFFSET+1]) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		}
		else
		{
			lcl_count = (glb_received[lcl_trans][DD_MSG_LEN_OFFSET] + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);
		}

		/* Security check ; maybe not necessary, already performed in IRQ, but who knows... */
		if (lcl_count > DD_USER_MAX_MSG_LENGTH)
		{
			lcl_count = DD_USER_MAX_MSG_LENGTH;
			lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(glb_received[lcl_trans][0]), (sizeof(DD_FEC_STD_WORD) * lcl_count));
			glb_fec_current_read_process_waiting--;
			if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_TO_US_FOR_READ_FRAME;
		 	return DD_TOO_LONG_FRAME_LENGTH;
		}

		lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(glb_received[lcl_trans][0]), (sizeof(DD_FEC_STD_WORD) * lcl_count));
		/*! decrement the number of currently processed read transactions before exit */
		glb_fec_current_read_process_waiting--;
		if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_TO_US_FOR_READ_FRAME;

		return DD_RETURN_OK;
	}
return DD_EXIT_READ_METHOD_ON_UNSTABLE_CONDITION;

}





/*!
<b>FUNCTION	: dd_plx_hard_reset_module</b>
- Job
	- performs a soft reset of the driver and a hard reset of the PLX
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->plx_offset_and_value (W)
		- ->fec_write_trans_in_use (R/W)
		- ->fec_current_read_process_waiting (R/W)
		- ->read_wait[] (W)
		- ->write_wait[] (W)
		- ->read[] (W)
		- ->copied[] (W)
		- ->currentTrans (W)
		- ->error_in_interrupt (W)
		- ->fec_offset_and_value (W)
- Outputs
	- Error Code
- Internals
	- global var glb_atomic_flags accessed R/W
	- global var glb_debug_buffer_head accessed W
	- global var glb_debug_buffer accessed W
	- global var glb_errors_in_irq accessed W
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors
		- DD_ERROR_MODULE_IS_RESETTING
		- DD_RESET_BLOCKED_BY_WRITE
		- DD_RESET_BLOCKED_BY_READ
		- sub-functions EVT
- Sub-functions calls
	- dd_stack_msg
	- dd_write_to_plx		(EVT)
	- strcpy			(system)
	- test_and_set_bit	(system)
	- sleep_on_timeout	(system)
	- clear_bit		(system)
	- dd_write_to_fec		(EVT)
*/

static DD_TYPE_ERROR dd_plx_hard_reset_module(void)
{
//#define DD_DEBUG_RESET
int lcl_i=0;
int lcl_write_ops;
int lcl_counter;
WAIT_QUEUE(lcl_wait);

#ifdef DD_DEBUG_RESET
DD_FEC_REGISTER_DATA lcl_fec_value;



	printk("DEBUG:fecpmc00 : Entering dd_plx_hard_reset_module() function\n");
#endif
	/*! Are we performing a reset ? */
	if (glb_atomic_flags & DD_SOFTRESET_RUNNING) return DD_ERROR_MODULE_IS_RESETTING;

	/*! set and test flag for atomic operation on read method increment */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		lcl_i = test_and_set_bit(DD_SOFT_RESET_FLAG, (void *)&glb_atomic_flags);
	#else
		lcl_i = test_and_set_bit(DD_SOFT_RESET_FLAG, &glb_atomic_flags);
	#endif

	

	/*! if we have set a flag previously setted, exit */
	if (lcl_i != DD_FLAG_WAS_NOT_SET) return DD_ERROR_MODULE_IS_RESETTING;
	/*! now, our flag has been set safely */

	/* increments the memory counter of how many PLX resets have been
	performed since the driver was loaded into memory */
	glb_plx_reset_counter++;


	/*! Invalidation des interruptions PLX */
	//IRQMOD
	#ifdef DD_ENABLE_IRQMANAGER
		dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_DISABLED_VALUE);
		glb_irq_status = DD_FLAG_IS_DISABLED;
	#endif


	/*! Reset PLX */
	dd_internal_write_to_plx(DD_PLX9080_CTRL_OFFSET, (DD_PLX_CTRL_VALUE | DD_PLX_CTRL_RESET_BIT));

	/* Security ; wait a little for PLX bridge to recover from reset */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		wait_event_timeout(lcl_wait, 0, 10);
	#else
		sleep_on_timeout(&lcl_wait, 10);
	#endif

	
		
	/*! Initialisation du pont PLX */
	dd_internal_write_to_plx(DD_PLX9080_CTRL_OFFSET, DD_PLX_CTRL_VALUE);

	dd_internal_write_to_plx(DD_PLX9080_BIGEND_OFFSET, DD_PLX_BIGEND_VALUE);

	dd_internal_write_to_plx(DD_PLX9080_MARBR_OFFSET, DD_PLX_MARBR_VALUE);

	dd_internal_write_to_plx(DD_PLX9080_LAS0RR_OFFSET, DD_PLX_LAS0RR_VALUE);

	dd_internal_write_to_plx(DD_PLX9080_LAS0BA_OFFSET, DD_PLX_LAS0BA_VALUE);

	dd_internal_write_to_plx(DD_PLX9080_LBRD0_OFFSET, DD_PLX_LBRD0_VALUE);

	/* Security ; wait a little for PLX bridge to recover from reset */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		wait_event_timeout(lcl_wait, 0, 10);
	#else
		sleep_on_timeout(&lcl_wait, 10);
	#endif


	/*! Re-Invalidation des interruptions PLX */
	//IRQMOD
	#ifdef DD_ENABLE_IRQMANAGER
		dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_DISABLED_VALUE);
	#endif


	/*! Wait that no more processes remains in WRITE wait queues */
	/*! init how_many_sleeps counter for watchdog */
	/*! all remaining WRITE and READ tasks should return a timeout SIG, */
	/*! because interrupts are invalidated! */
	lcl_counter = 0;
	do
	{
		/*! set current waiting write operations to zero */
		lcl_write_ops = 0;

		for (lcl_i = DD_MIN_TRANS_NUMBER ; lcl_i <= DD_MAX_TRANS_NUMBER ; lcl_i++)
		{
			if (glb_fec_write_trans_in_use[lcl_i] == DD_TRANS_NUMBER_IS_IN_USE)
			{
				lcl_write_ops++;
			}
		}

		if (lcl_write_ops > 0)
		{
			/*! Wait a little ... */
			lcl_counter++;
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
				wait_event_timeout(lcl_wait, 0, DD_TIMEOUT_WAIT_ON_RESET);
			#else
				sleep_on_timeout(&lcl_wait, DD_TIMEOUT_WAIT_ON_RESET);
			#endif
		}

	} while ( (lcl_write_ops > 0) && (lcl_counter <= DD_MAX_WAIT_CYCLES_IN_RESET) );



	/*! check if we have a timeout-error (blocking write task(s)) */
	/*! in this case, module must bin manually removed then reload in memory */
	if (lcl_counter > DD_MAX_WAIT_CYCLES_IN_RESET)
	{
		/*! release flag */
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			clear_bit(DD_SOFT_RESET_FLAG, (void *)&glb_atomic_flags);
		#else
			clear_bit(DD_SOFT_RESET_FLAG, &glb_atomic_flags);
		#endif
                return DD_RESET_BLOCKED_BY_WRITE;
	}


	/*! Wait that no more processes remains in READ wait queues */
	/*! init how_many_sleeps counter for watchdog */
	lcl_counter=0;
	while ( (glb_fec_current_read_process_waiting > 0) && (lcl_counter <= DD_MAX_WAIT_CYCLES_IN_RESET) )
	{
		/*! Wait a little ... */
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			wait_event_timeout(lcl_wait, 0, DD_TIMEOUT_WAIT_ON_RESET);
		#else
			sleep_on_timeout(&lcl_wait, DD_TIMEOUT_WAIT_ON_RESET);
		#endif

		
	}


	/*! check if we have a timeout-error (blocking read task(s)) */
	/*! in this case, module must bin manually removed then reload in memory */
	if (lcl_counter > DD_MAX_WAIT_CYCLES_IN_RESET)
	{

		/*! release flag */
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			clear_bit(DD_SOFT_RESET_FLAG, (void *)&glb_atomic_flags);
		#else
			clear_bit(DD_SOFT_RESET_FLAG, &glb_atomic_flags);
		#endif
		return DD_RESET_BLOCKED_BY_READ;
	}


	/*! Now, reset wait queues */
	wake_up_interruptible(&glb_wq_write);
	wake_up_interruptible(&glb_wq_read);
	wake_up_interruptible(&glb_wq_devices_warning);

	/* Now, clear timers*/
	for (lcl_i = 0; lcl_i <= DD_MAX_TRANS_NUMBER; lcl_i++)
	{
		del_timer(&(glb_fec_trans_number_timers[lcl_i]));
	}


	/*! Init Stacks status */
	for (lcl_i = 0 ; lcl_i <= DD_MAX_TRANS_NUMBER ; lcl_i++)
	{
		glb_read[lcl_i] = DD_READ_DATA_UNSUCCESSFUL;
		glb_copied[lcl_i] = DD_COPY_DATA_UNSUCCESSFUL;
		glb_fec_write_trans_in_use[lcl_i] = DD_TRANS_NUMBER_IS_FREE;
	}


	/*! Reset counter status */
	glb_currentTrans = DD_MIN_TRANS_NUMBER;

	/*! Reset global wait queue status */
	glb_fec_current_read_process_waiting = 0;


	/*! clear flag for freeing transaction number manager */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, &glb_atomic_flags);
	#endif

	

	/*! clear flag of hardware access */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_FIFOTRA_IN_USE_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_FIFOTRA_IN_USE_FLAG, &glb_atomic_flags);
	#endif

	

	/*! clear flag of read increment method */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_READ_REQUEST_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_READ_REQUEST_FLAG, &glb_atomic_flags);
	#endif

	

	/* Reset IRQ errors counter */
	glb_errors_in_irq = 0;


#ifdef DD_DEBUG_RESET
dd_internal_read_from_fec_register(DD_FEC_CTRL0_OFFSET, &lcl_fec_value);
printk("DEBUG:fecpmc00 : CTRL0 is : 0x%x\n",lcl_fec_value);
dd_internal_read_from_fec_register(DD_FEC_STAT0_OFFSET, &lcl_fec_value);
printk("DEBUG:fecpmc00 : STATUS0 is : 0x%x\n",lcl_fec_value);
#endif




	/*! reenable fec */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, DD_FEC_ENABLE_FEC);


	/* Is Inject Token needed here with new FECs ? */
	//#ifdef IS_FECREGISTERS_V0
//	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
	if (glb_fec_type == 0)
	{
		#ifdef DD_DEBUG_RESET
			printk("DEBUG:fecpmc00 : clearing ERRs and IRQs\n");
		#endif
		dd_internal_write_to_fec_register(DD_FEC_CTRL1_OFFSET, DD_FEC_CLEAR_ERRORS | DD_FEC_CLEAR_IRQ);
	}
	//#else
		//#ifdef IS_FECREGISTERS_V1
			else
			{
				dd_internal_write_to_fec_register(DD_FEC_CTRL1_OFFSET, DD_FEC_RELEASE_FEC | DD_FEC_CLEAR_ERRORS | DD_FEC_CLEAR_IRQ);
				#ifdef DD_DEBUG_RESET
					printk("DEBUG:fecpmc00 : clearing ERRs and IRQs && INJECT TOKEN\n");
				#endif
			}
			
		//#endif
	//#endif

	/* Security ; wait a little for PLX bridge to recover from reset */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		wait_event_timeout(lcl_wait, 0, 10);
	#else
		sleep_on_timeout(&lcl_wait, 10);
	#endif


	/*! Re-Validation des interruptions PLX */
	#ifdef DD_ENABLE_IRQMANAGER
		dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_ENABLED_VALUE);
		glb_irq_status = DD_FLAG_IS_ENABLED;
	#endif

	/*! release flag */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_SOFT_RESET_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_SOFT_RESET_FLAG, &glb_atomic_flags);
	#endif

return DD_RETURN_OK;
}







/*!
<b>FUNCTION	: dd_fec_soft_reset_module</b>
- Job
	- performs a soft reset of the FEC
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->fec_offset_and_value (W)
- Outputs
	- Error Code
- Internals
	- global var glb_atomic_flags accessed R/W
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors
		- DD_ERROR_MODULE_IS_RESETTING
		- sub-functions EVT
- Sub-functions calls
	- dd_stack_msg
	- test_and_set_bit	(system)
	- clear_bit		(system)
	- dd_write_to_fec		(EVT)
*/

static DD_TYPE_ERROR dd_fec_soft_reset_module(void)
{

int lcl_i=0;
//return DD_RETURN_OK;

	/*! Are we performing a reset ? */
	if (glb_atomic_flags & DD_SOFTRESET_RUNNING) return DD_ERROR_MODULE_IS_RESETTING;

	/*! set and test flag for atomic operation on read method increment */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		lcl_i = test_and_set_bit(DD_SOFT_RESET_FLAG, (void *)&glb_atomic_flags);
	#else
		lcl_i = test_and_set_bit(DD_SOFT_RESET_FLAG, &glb_atomic_flags);
	#endif

	

	/*! if we have set a flag previously setted, exit */
	if (lcl_i != DD_FLAG_WAS_NOT_SET) return DD_ERROR_MODULE_IS_RESETTING;
	/*! now, our flag has been set safely */

	/* increments the memory counter of how many FEC resets have been
	performed since the driver was loaded into memory */
	glb_fec_reset_counter++;

	/*! Reset FEC */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, (DD_FEC_ENABLE_FEC | DD_FEC_RESET_OUT));

	/*! Restore FEC status */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, DD_FEC_ENABLE_FEC);

	/*! release flag */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_SOFT_RESET_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_SOFT_RESET_FLAG, &glb_atomic_flags);
	#endif

	

return DD_RETURN_OK;
}








/*!
<b>FUNCTION	: dd_force_count_to_one</b>
- Job
	- force USER_COUNT to one, in order to be able to unload
	the module
- Inputs
	- Void
- Outputs
	- Error Code
- Internals
	- use system MOD_INC/DEC macros
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	Errors :
		- Void
- Sub-functions calls
	- dd_stack_msg
*/

static DD_TYPE_ERROR dd_force_count_to_one(void)
{
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)

int lcl_count;

	/*! get current number of users */
	lcl_count = MOD_IN_USE;

	/*! if more than current process is using the driver, decrement */
	/*! until only current process remains */
	if (lcl_count > 1)
	{
		do
		{
			MOD_DEC_USE_COUNT;
			lcl_count = MOD_IN_USE;
		} while (lcl_count > 1);
	}

	/*! if less than current process is using the driver, increment */
	/*! until only current process remains */
	if (lcl_count < 1)
	{
		do
		{
			MOD_INC_USE_COUNT;
			lcl_count = MOD_IN_USE;
		} while (lcl_count < 1);
	}

	#endif
return DD_RETURN_OK;
}



static DD_TYPE_ERROR dd_get_users_counter(struct dd_ioctl_data * param_ioctl_data)
{
int lcl_count;
DD_TYPE_ERROR lcl_err;

	/*! get current number of users */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		lcl_count = 0;
	#else
		lcl_count = MOD_IN_USE;
	#endif	

	/* send info to user */
	lcl_err = __copy_to_user((int *)param_ioctl_data->ioctl_address, &lcl_count, sizeof(int));
	if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_CANNOT_WRITE_VALUE_TO_US_FOR_FEC_READ;
return DD_RETURN_OK;
}




static DD_TYPE_ERROR dd_get_driver_version(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;

	/* send info to user */
	lcl_err = __copy_to_user((char *)param_ioctl_data->ioctl_address, &(glb_dd_version[0]), (sizeof(char)*DD_VERSION_STRING_SIZE));
	if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_CANNOT_WRITE_VALUE_TO_US_FOR_FEC_READ;
return DD_RETURN_OK;
}




/*!
<b>FUNCTION	: dd_get_base_address</b>
- Job
	- return value of PLX baseX address to user space
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->ioc_container (R)
		- ->plx_ba_infos (R)
	- integer, index of base address ([0..4]) to get
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
- Sub-functions calls
	- dd_stack_msg
	- __copy_to_user	(system)
*/

static DD_TYPE_ERROR dd_get_base_address(struct dd_ioctl_data * param_ioctl_data, int param_base_X)
{

DD_TYPE_ERROR lcl_err;


	/*! memoire checkee en debut d'ioctl */
	lcl_err = __copy_to_user((DD_TYPE_PCI_BASE_ADDRESS *)param_ioctl_data->ioctl_address, &(glb_plx_ba_array[param_base_X]), sizeof(DD_TYPE_PCI_BASE_ADDRESS));
	if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA;

return DD_RETURN_OK;
}



/*!
<b>FUNCTION	: dd_get_remapped_base_address</b>
- Job
	- return value of PLX remapped baseX address to user space
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->ioc_container (R)
		- ->plx_ba_infos (R)
	- integer, index of base address ([0..4]) to get
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA
- Sub-functions calls
	- dd_stack_msg
	- __copy_to_user	(system)
*/

static DD_TYPE_ERROR dd_get_remapped_base_address(struct dd_ioctl_data * param_ioctl_data, int param_base_X)
{

DD_TYPE_ERROR lcl_err;

	/*! memoire checkee en debut d'ioctl */
	lcl_err = __copy_to_user((DD_TYPE_PCI_BASE_ADDRESS *)param_ioctl_data->ioctl_address, &(glb_plx_remapped_ba_array[param_base_X]), sizeof(DD_TYPE_PCI_BASE_ADDRESS));
	if (lcl_err != DD_UUCOPY_TO_USER_OK) return DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA;


return DD_RETURN_OK;
}




static DD_TYPE_ERROR dd_get_return_status(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;
DD_FEC_STD_WORD lcl_trans;
DD_FEC_STD_WORD lcl_ncar;
DD_FEC_STD_WORD lcl_value;

	/*! get transaction number from user space */
	lcl_err = __copy_from_user(&lcl_trans,(DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, sizeof(DD_FEC_STD_WORD));
	if (lcl_err != DD_RETURN_OK) return DD_FEC_CANNOT_READ_VALUE_FROM_US_FOR_RET_STATUS;

	/*! get status of transaction number XXX */
	if (glb_copied[lcl_trans] != DD_COPY_DATA_UNSUCCESSFUL )
	{
		/*! get length of frame past the first three elements */
		lcl_ncar = glb_back[lcl_trans][DD_MSG_LEN_OFFSET];
		if (isLongFrame(glb_back[lcl_trans][DD_MSG_LEN_OFFSET]))
		{
			lcl_ncar = ((((glb_back[lcl_trans][DD_MSG_LEN_OFFSET] & DD_UPPER_BITMASK_FOR_LONGFRAME)<<8) + glb_back[lcl_trans][DD_MSG_LEN_OFFSET+1]) + DD_FRAME_HEAD_LENGTH_IF_LONGFRAME);
		}
		else
		{
			lcl_ncar = (glb_back[lcl_trans][DD_MSG_LEN_OFFSET] + DD_FRAME_HEAD_LENGTH_IF_SHORTFRAME);
		}
		/*! get last element of back stack ; trans status is on the 8 upper bits of the word */
		lcl_value = glb_back[lcl_trans][lcl_ncar];
	}
	else
	{
		lcl_value = DD_COPIED_NOT_READY;
	}

	/*! send status to user space */
	lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(lcl_value), sizeof(DD_FEC_STD_WORD));
	if (lcl_err != DD_RETURN_OK) return DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_RET_STATUS;


return DD_RETURN_OK;
}






/*!
<b>FUNCTION	: dd_fec_init_ttcrx</b>
- Job
	- ttcrx initialisation
- Inputs
	- none
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void

- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_INIT_TTCRX_WATCHDOG
- Sub-functions calls
	- dd_internal_write_to_fec_16
	- dd_internal_read_from_fec
	- udelay			(system)
*/



static DD_TYPE_ERROR dd_fec_init_ttcrx(void)
{

DD_FEC_REGISTER_DATA lcl_fec_status;
DD_FEC_REGISTER_DATA lcl_ctrl0_memory;

int lcl_counter;
WAIT_QUEUE(lcl_wait);

	/* Keep memory of current CTRL0 */
	dd_internal_read_from_fec_register(DD_FEC_CTRL0_OFFSET, &lcl_ctrl0_memory);

	/* Hook ttcrx bit */
	lcl_fec_status = (lcl_ctrl0_memory | DD_FEC_RESET_TTCRX);

	/* Write hooked ttcrx bit */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, lcl_fec_status);

	/*restore pre-hook ctrl0 value */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, lcl_ctrl0_memory);


	lcl_counter = 0;
	do
	{
		/*! get fec status */
		dd_internal_read_from_fec_register(DD_FEC_STAT0_OFFSET, &lcl_fec_status);

		/*! inc counter used as (one) exit condition */
		lcl_counter++;

		/* freeze all activity for 1 millisecond */
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			wait_event_timeout(lcl_wait, 0, DD_INIT_TTCRX_INLOOP_DELAY);
		#else
			sleep_on_timeout(&lcl_wait, DD_INIT_TTCRX_INLOOP_DELAY);
		#endif	

		
	}
	while ( (!(lcl_fec_status & DD_FEC_TTCRX_READY)) && (lcl_counter <= DD_FEC_TTCRX_INIT_LOOP_WATCHDOG ) );
	/* Rem : DD_FEC_TTCRX_INIT_LOOP_WATCHDOG is setted to 2000 by default,
	i.e. the full loop can wait TTCRX initialisation up to 2 seconds */

	/* ecriture de la valeur standard dans le registre CTRL0 du FEC */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, lcl_ctrl0_memory);

	if (lcl_counter > DD_FEC_TTCRX_INIT_LOOP_WATCHDOG) return DD_INIT_TTCRX_WATCHDOG;

return DD_RETURN_OK;
}






/*!
<b>FUNCTION	: dd_ring_error_management</b>
- Job
	- Can turn ON/OFF the increment of specific counters when errors occurs on the ring.
	- Can return the current status of the Ring Error Management system
	- Can return the current values of the Ring Error Management counters
	- Can reset to Zero the content of the Ring Error Management counters

- I/O
	- Inputs
		- param_ioctl_data : struct pointer to a dd_ioctl_data.
			- <b>Fields used :</b>
				- ->ioctl_address
	- Outputs
		- param_ioctl_data : struct pointer to a dd_ioctl_data.
			- <b>Fields used :</b>
				- ->ioctl_address
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- glb_enable_count_of_errors_on_ring
	- glb_rem_counter[]
- Notes
	- void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_READ_VALUE_FROM_US
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US
		- DD_INVALID_REM_CMD
- Sub-functions calls
	- __copy_from_user	(system)
	- __copy_to_user	(system)
*/
static DD_TYPE_ERROR dd_ring_error_management(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;
DD_FEC_REM_TYPE lcl_param[DD_NB_OF_ERRORS_TO_SPY];
int lcl_i;
DD_FEC_REM_TYPE lcl_command;


	/* get requested action from user space */
	lcl_err = __copy_from_user(lcl_param,(DD_FEC_REM_TYPE *)param_ioctl_data->ioctl_address, (sizeof(DD_FEC_REM_TYPE)*DD_NB_OF_ERRORS_TO_SPY));
	if (lcl_err != DD_RETURN_OK) return DD_FEC_CANNOT_READ_VALUE_FROM_US;
	lcl_command = lcl_param[0];

	switch (lcl_command)
	{
		case DD_TURN_REM_ON:
			glb_enable_count_of_errors_on_ring = DD_FLAG_IS_ENABLED;
			return DD_RETURN_OK;
		break;

		case DD_TURN_REM_OFF:
			glb_enable_count_of_errors_on_ring = DD_FLAG_IS_DISABLED;
			return DD_RETURN_OK;
		break;

		case DD_RESET_REM_VALUE:
			for (lcl_i=0; lcl_i < DD_NB_OF_ERRORS_TO_SPY; lcl_i++)
			{
				glb_rem_counter[lcl_i] = 0;
			}
			return DD_RETURN_OK;
		break;

		case DD_GET_REM_VALUE:
			lcl_err = __copy_to_user((DD_FEC_REM_TYPE *)param_ioctl_data->ioctl_address, glb_rem_counter, (sizeof(DD_FEC_REM_TYPE)*DD_NB_OF_ERRORS_TO_SPY));
			if (lcl_err != DD_RETURN_OK) return DD_FEC_CANNOT_WRITE_VALUE_TO_US;
			return DD_RETURN_OK;
		break;

		case DD_GET_REM_STATUS:
			glb_rem_counter[0] = glb_enable_count_of_errors_on_ring;
			lcl_err = __copy_to_user((DD_FEC_REM_TYPE *)param_ioctl_data->ioctl_address, glb_rem_counter, (sizeof(DD_FEC_REM_TYPE)*DD_NB_OF_ERRORS_TO_SPY));
			if (lcl_err != DD_RETURN_OK) return DD_FEC_CANNOT_WRITE_VALUE_TO_US;
			return DD_RETURN_OK;
		break;

		default:
			return DD_INVALID_REM_CMD;
		break;
	}
}







/*!
<b>FUNCTION	: dd_get_plx_reset_counter</b>
- Job
	- Returns how many times the PLX reset function has been called
	since driver has been loaded in memory.

- I/O
	- Inputs
		- param_ioctl_data : struct pointer to a dd_ioctl_data.
			- <b>Fields used :</b>
				- ->ioctl_address
	- Outputs
		- param_ioctl_data : struct pointer to a dd_ioctl_data.
			- <b>Fields used :</b>
				- ->ioctl_address
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- glb_plx_reset_counter
- Notes
	- void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US
- Sub-functions calls
	- __copy_to_user	(system)
*/
static DD_TYPE_ERROR dd_get_plx_reset_counter(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;
	lcl_err = __copy_to_user((DD_RESET_COUNTER_TYPE *)param_ioctl_data->ioctl_address, &glb_plx_reset_counter, sizeof(DD_RESET_COUNTER_TYPE));
	if (lcl_err != DD_RETURN_OK) return DD_FEC_CANNOT_WRITE_VALUE_TO_US;
	return DD_RETURN_OK;
}


/*!
<b>FUNCTION	: dd_get_fec_reset_counter</b>
- Job
	- Returns how many times the FEC reset function has been called
	since driver has been loaded in memory.

- I/O
	- Inputs
		- param_ioctl_data : struct pointer to a dd_ioctl_data.
			- <b>Fields used :</b>
				- ->ioctl_address
	- Outputs
		- param_ioctl_data : struct pointer to a dd_ioctl_data.
			- <b>Fields used :</b>
				- ->ioctl_address
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- glb_fec_reset_counter
- Notes
	- void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US
- Sub-functions calls
	- __copy_to_user	(system)
*/
static DD_TYPE_ERROR dd_get_fec_reset_counter(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;
	lcl_err = __copy_to_user((DD_RESET_COUNTER_TYPE *)param_ioctl_data->ioctl_address, &glb_fec_reset_counter, sizeof(DD_RESET_COUNTER_TYPE));
	if (lcl_err != DD_RETURN_OK) return DD_FEC_CANNOT_WRITE_VALUE_TO_US;
	return DD_RETURN_OK;
}





/*!
<b>FUNCTION	: dd_get_driver_status</b>
- Job
	- Returns a word (bitfield) that gives the current status
	(software status) of the driver.

- I/O
	- Inputs
		- void
- Outputs
		- param_ioctl_data : struct pointer to a dd_ioctl_data.
			- <b>Fields used :</b>
				- ->ioctl_address
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- glb_irq_status
	- glb_rem_counter[]
	- glb_enable_count_of_errors_on_ring
	- glb_driver_status
- Notes
	- void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_CANNOT_WRITE_VALUE_TO_US
- Sub-functions calls
	- __copy_to_user	(system)
*/
static DD_TYPE_ERROR dd_get_driver_status(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;
	glb_driver_status = 0x0000;
	if (glb_irq_status == DD_FLAG_IS_ENABLED) glb_driver_status = (glb_driver_status | DD_STATUS_FLAG_IRQ_IS_ON);
	if (glb_rem_counter[DD_LONG_FRAME_ERROR_OFFSET] != 0) glb_driver_status = (glb_driver_status | DD_STATUS_FLAG_LONG_FRAME_ERROR);
	if (glb_rem_counter[DD_SHORT_FRAME_ERROR_OFFSET] != 0) glb_driver_status = (glb_driver_status | DD_STATUS_FLAG_SHORT_FRAME_ERROR);
	if (glb_rem_counter[DD_WRONG_TRANS_NUMBER_ERROR_OFFSET] != 0) glb_driver_status = (glb_driver_status | DD_STATUS_FLAG_BAD_TRANS_NUMBER_ERROR);
	if (glb_rem_counter[DD_WARNING_FROM_DEVICE_ERROR_OFFSET] != 0) glb_driver_status = (glb_driver_status |
	DD_STATUS_FLAG_WARNING_EMITTED_BY_DEVICE_ERROR);
	if (glb_enable_count_of_errors_on_ring != DD_FLAG_IS_DISABLED) glb_driver_status = (glb_driver_status | DD_STATUS_FLAG_ERRORCOUNTING_IS_ON) ;
	lcl_err = __copy_to_user((DD_DRIVER_STATUS_TYPE *)param_ioctl_data->ioctl_address, &glb_driver_status, sizeof(DD_DRIVER_STATUS_TYPE));
	if (lcl_err != DD_RETURN_OK) return DD_FEC_CANNOT_WRITE_VALUE_TO_US;
	return DD_RETURN_OK;
}





static DD_TYPE_ERROR dd_wait_warning(struct dd_ioctl_data * param_ioctl_data)
{
DD_TYPE_ERROR lcl_err;

/* Le driver va entrer dans cette fonction, et partir en sleep
jusqu'a ce que un warning soit emis par un device.
Pendant l'attente de warning, il faut que d'autres ioctl() puissent
acceder au driver. Donc, AVANT de partir en sleep, il faut lever
le global_lock sur l'entree en ioctl()
SI d'autres WAIT_WARNING requests arrivent alors qu'une WAIT8WARNING request
est deja active, on l'empile dans la wait_queue dediee.
A l'arrivee d'un warning emitted by device, tous les processus dormant
dans cette queue seront reveilles.
*/

	//On travaille avec la wait queue : static WAIT_QUEUE(glb_wq_devices_warning);
	// Avant de partir en mode sleep, on unlock le driver
	up(&(glb_ioctl_lock));
	//Et on attends...



		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			wait_event_interruptible(glb_wq_devices_warning, 0);
		#else
			interruptible_sleep_on(&glb_wq_devices_warning);
		#endif	

	

	//Si on sors de cette attente, c'est qu'un warning emitted by a device a ete
	//detecte par l'IRQ manager. On retrouve la trame emise dans glb_warning_buf[]
	//--> on copie glb_warning_buf[] vers le jeu de donn�s locales (cote user)
	lcl_err = __copy_to_user((DD_FEC_STD_WORD *)param_ioctl_data->ioctl_address, &(glb_warning_buf[0]), (sizeof(DD_FEC_STD_WORD)*DD_USER_MAX_MSG_LENGTH));
	if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1;

return DD_RETURN_OK;
}



static DD_TYPE_ERROR dd_simulate_warning(struct dd_ioctl_data * param_ioctl_data)
{
int lcl_i;
	for (lcl_i=0; lcl_i<DD_USER_MAX_MSG_LENGTH; lcl_i++)
	{
		glb_warning_buf[lcl_i]=5;
	}
	wake_up_interruptible(&glb_wq_devices_warning);
return DD_RETURN_OK;
}



static DD_TYPE_ERROR dd_get_fifo_item_size(struct dd_ioctl_data * param_ioctl_data)
{
int lcl_size;
DD_TYPE_ERROR lcl_err;

	//lcl_size = sizeof(DD_FEC_FIFO_DATA);
	//if (sizeof(DD_FEC_FIFO_DATA) == 2)
//	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
	if (glb_fec_type == 0)
	{
		lcl_size = 16;
	}
	else
	{
		//if (sizeof(DD_FEC_FIFO_DATA) == 4)
		if ( (glb_fec_type == 2) || (glb_fec_type == 1) )
		{
			lcl_size = 32;
		}
		else lcl_size = 0;
	}
	lcl_err = __copy_to_user((int *)param_ioctl_data->ioctl_address, &(lcl_size), (sizeof(int)*1));
	if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1;

return DD_RETURN_OK;
}



static DD_TYPE_ERROR dd_get_fec_type(struct dd_ioctl_data * param_ioctl_data)
{

DD_TYPE_ERROR lcl_err;

	lcl_err = __copy_to_user((int *)param_ioctl_data->ioctl_address, &(glb_fec_type), (sizeof(int)*1));
	if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1;

return DD_RETURN_OK;
}


static DD_TYPE_ERROR dd_get_fec_vendor_id(struct dd_ioctl_data * param_ioctl_data)
{

DD_TYPE_ERROR lcl_err;

	lcl_err = __copy_to_user((unsigned short *)param_ioctl_data->ioctl_address, &(glb_vendID), (sizeof(unsigned short)*1));
	if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1;

return DD_RETURN_OK;
}

static DD_TYPE_ERROR dd_get_fec_device_id(struct dd_ioctl_data * param_ioctl_data)
{

DD_TYPE_ERROR lcl_err;

	lcl_err = __copy_to_user((unsigned short *)param_ioctl_data->ioctl_address, &(glb_devID), (sizeof(unsigned short)*1));
	if (lcl_err != DD_RETURN_OK) return DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1;

return DD_RETURN_OK;
}


/*!
<b>FUNCTION 0200	: dd_0200_give_handle_to_fec_ioctl</b>
- Job
	- Call one of the previously defined FEC functions according
	to the sent IOCTL command
- Inputs
	- struct pointer to a DD_device_info struct
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- This function is called from the HUB function
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_FEC_COMMAND_NOT_ASSOCIATED
		- sub-functions EVT
- Sub-functions calls
	- dd_stack_msg
	- dd_write_to_fec			(EVT)
	- dd_read_from_fec		(EVT)
	- dd_get_base_address		(EVT)
	- dd_get_return_status		(EVT)
	- dd_fec_init_ttcrx		(EVT)
	- dd_fec_write_msg		(EVT)
	- dd_fec_read_msg			(EVT)
	- dd_reset_module			(EVT)
	- dd_force_count_to_one		(EVT)
	- dd_dump_dbg_messages		(EVT)
	- dd_0201_ioc_set_fec_ctrl0	(EVT)
	- dd_0203_ioc_set_fec_ctrl1	(EVT)
	- dd_0202_ioc_get_fec_ctrl0	(EVT)
	- dd_0204_ioc_get_fec_stat0	(EVT)
	- dd_0205_ioc_get_fec_stat1	(EVT)
	- dd_ioc_enable_plx_interrupts	(EVT)
	- dd_ioc_disable_plx_interrupts	(EVT)
*/

static DD_TYPE_ERROR dd_0200_give_handle_to_fec_ioctl(struct dd_ioctl_data * param_ioctl_data)
{

	switch(param_ioctl_data->ioctl_command)
	{

		case DD_IOC_WRITE_TO_FEC_REGISTER:
			return dd_external_write_to_fec_register(param_ioctl_data);
		break;



		case DD_IOC_READ_FROM_FEC_REGISTER:
			return dd_external_read_from_fec_register(param_ioctl_data);
		break;


		case DD_IOC_WRITE_TO_FEC_FIFO:
			return dd_external_write_to_fec_fifo(param_ioctl_data);
		break;



		case DD_IOC_READ_FROM_FEC_FIFO:
			return dd_external_read_from_fec_fifo(param_ioctl_data);
		break;


	        case DD_IOC_FEC_GET_BASE0_ADDRESS:
			return dd_get_base_address(param_ioctl_data, DD_REQUEST_FOR_BASE_ADDRESS0);
		break;



	        case DD_IOC_FEC_GET_BASE1_ADDRESS:
			return dd_get_base_address(param_ioctl_data, DD_REQUEST_FOR_BASE_ADDRESS1);
		break;


	        case DD_IOC_FEC_GET_BASE2_ADDRESS:
			return dd_get_base_address(param_ioctl_data, DD_REQUEST_FOR_BASE_ADDRESS2);
		break;

	        case DD_IOC_FEC_GET_BASE3_ADDRESS:
			return dd_get_base_address(param_ioctl_data, DD_REQUEST_FOR_BASE_ADDRESS3);
		break;



	        case DD_IOC_FEC_GET_REMAPPED_BASE0_ADDRESS:
			return dd_get_remapped_base_address(param_ioctl_data, DD_REQUEST_FOR_BASE_ADDRESS0);
		break;

	        case DD_IOC_FEC_GET_REMAPPED_BASE1_ADDRESS:
			return dd_get_remapped_base_address(param_ioctl_data, DD_REQUEST_FOR_BASE_ADDRESS1);
		break;

	        case DD_IOC_FEC_GET_REMAPPED_BASE2_ADDRESS:
			return dd_get_remapped_base_address(param_ioctl_data, DD_REQUEST_FOR_BASE_ADDRESS2);
		break;

	        case DD_IOC_FEC_GET_REMAPPED_BASE3_ADDRESS:
			return dd_get_remapped_base_address(param_ioctl_data, DD_REQUEST_FOR_BASE_ADDRESS3);
		break;


		case DD_IOC_FEC_GET_RETURN_STATUS:
			return dd_get_return_status(param_ioctl_data);
		break;


		case DD_IOC_FEC_INIT_TTCRX:
			return dd_fec_init_ttcrx();
		break;


		case DD_IOC_FEC_WRITE_FRAME:
			return dd_fec_write_msg(param_ioctl_data);
		break;



		case DD_IOC_FEC_READ_FRAME:
			return dd_fec_read_msg(param_ioctl_data);
		break;



		case DD_IOC_FEC_SOFT_RESET_MODULE:
			return dd_fec_soft_reset_module();
		break;


		case DD_IOC_FEC_FORCE_COUNT_TO_ONE:
			return dd_force_count_to_one();
		break;

		case DD_IOC_FEC_GET_USERS_COUNTER:
			return dd_get_users_counter(param_ioctl_data);
		break;


		case DD_IOC_FEC_GET_DRIVER_VERSION:
			return dd_get_driver_version(param_ioctl_data);
		break;


		case DD_IOC_RING_ERROR_MANAGEMENT:
			return dd_ring_error_management(param_ioctl_data);
		break;

		case DD_IOC_GET_PLX_RESET_COUNTER:
			return dd_get_plx_reset_counter(param_ioctl_data);
		break;

		case DD_IOC_GET_FEC_RESET_COUNTER:
			return dd_get_fec_reset_counter(param_ioctl_data);
		break;

		case DD_IOC_GET_DRIVER_STATUS:
			return dd_get_driver_status(param_ioctl_data);
		break;

		case DD_IOC_WAIT_WARNING:
			return dd_wait_warning(param_ioctl_data);
		break;

		case DD_IOC_SIMULATE_WARNING:
			return dd_simulate_warning(param_ioctl_data);
		break;

		case DD_IOC_GET_FIFO_ITEM_SIZE:
			return dd_get_fifo_item_size(param_ioctl_data);
		break;


		case DD_IOC_GET_FEC_TYPE:
			return dd_get_fec_type(param_ioctl_data);
		break;


		case DD_IOC_GET_VENDOR_ID:
			return dd_get_fec_vendor_id(param_ioctl_data);
		break;


		case DD_IOC_GET_DEVICE_ID:
			return dd_get_fec_device_id(param_ioctl_data);
		break;


		default:
			return DD_FEC_COMMAND_NOT_ASSOCIATED;


	} /*! Fin du switch command */

return DD_RETURN_OK;
}


//////////////////////
//INTERRUPT XXX
//////////////





/*!
<b>FUNCTION 	: dd_read_fifo</b>
- Job
	- Read FIFO Receive, store data, and set flags according
	to read result
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->plx_ba_infos[] (R)
		- ->received
		- ->read[]
		- ->read_wait[]
		- ->fec_trans_number_timers[]
		- ->fec_write_trans_in_use[]
		- ->back[][]
		- ->copied[]
		- ->write_wait[]
		- ->f_ack[]
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_CANNOT_EXIT_FROM_IRQ_READ_DATA
		- DD_RECEIVED_MSG_TOO_LONG
		- DD_RING_NOISY
		- DD_RECEIVED_WRONG_TRANS_NUMBER
- Sub-functions calls
	- dd_stack_msg
	- wake_up_interruptible	(system)
*/

static DD_TYPE_ERROR dd_read_fifo(void)
{
//#ifdef IS_FIFO_32BITS
//	#define DEBUG_dd_read_fifo

volatile DD_FEC_FIFO_DATA_32 lcl_data32;
DD_FEC_FIFO_DATA_32 lcl_buf32[DD_MAX_MSG_LENGTH_32];
int lcl_ncar32 = 0;
int lcl_length;
DD_FEC_STD_WORD lcl_fl1, lcl_fl2, lcl_frame_length, lcl_frame_32_length=0;
//double lcl_ref, lcl_div;
DD_FEC_STD_WORD lcl_source, lcl_destination;
//#else
//#ifdef IS_FIFO_16BITS
volatile DD_FEC_FIFO_DATA_16 lcl_data;
int lcl_flag = 0;
//#endif
//#endif

DD_FEC_STD_WORD lcl_status=0;
DD_FEC_STD_WORD lcl_buf[DD_USER_MAX_MSG_LENGTH];
int lcl_ncar = 0,lcl_j,lcl_trans=0;
long lcl_watchdog_counter=0;
DD_TYPE_ERROR lcl_err=DD_RETURN_OK;
int lcl_limit;




	#ifdef DD_SPY_DEBUG
		printk("\n\nIn dd_read_fifo : interrupt detected.\n");
	#endif

	/*! init lcl_watchdog to zero when entering interrupt manager */
	lcl_watchdog_counter = 0;

	/*! while{} loop is here in case of a second frame occurs while processing a first one */
	/*! but if the ring is very busy, we could have messages everytime and not exit from interrupt */
	/*! this pb is solved here via a watchdog counter */
	while (!( (*(DD_TYPE_ADDRESS *) (glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)DD_FEC_STAT0_OFFSET)) & DD_FEC_REC_EMPTY))
	{
		/*! inc number of frames successfully read on ring, during one irq function */
		lcl_watchdog_counter++;

		/*! if we are stuck in interrupt, then return */
		if (lcl_watchdog_counter > DD_MAX_CONTINUOUS_FRAMES_READ_IN_IRQ) return DD_CANNOT_EXIT_FROM_IRQ_READ_DATA;

		/*! init flags and counter */
//		if (glb_fec_type == 2)
		if (glb_fec_type != 0)
		{
		//#ifdef IS_FIFO_32BITS
			lcl_ncar32 = 0;
//			lcl_frame_length=0; //here only to avoid warning cc message
		//#else
		}
		else
		{
			//#ifdef IS_FIFO_16BITS
				lcl_ncar = 0;
				lcl_flag = DD_FIFOREC_EOFRAME_NOT_READ;
			//#endif
		//#endif
		}


		#ifdef DD_SPY_DEBUG
			printk("In dd_read_fifo : Incoming frame detected.\n");
		#endif

//		if (glb_fec_type == 2)
		if (glb_fec_type != 0)
		{
//#ifdef IS_FIFO_32BITS
		do
		{
			lcl_data32 = (*(DD_TYPE_ADDRESS *)( glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)dd_fec_fiforec_offset));
			//udelay(1);
			#ifdef DD_SPY_DEBUG
				printk("\t0x%x : \n",lcl_data32);
			#endif
			//If buffer not full, store data. Else, avoid buffer overflow
			if (lcl_ncar32 < DD_MAX_MSG_LENGTH_32) lcl_buf32[lcl_ncar32] = lcl_data32;


			//length analysis
			if (lcl_ncar32 == 0)
			{
				//scr/dest analysis
				lcl_source = (DD_FEC_STD_WORD)((lcl_data32>>16) & 0xFF);
				lcl_destination = (DD_FEC_STD_WORD)((lcl_data32>>24) & 0xFF);


				/* How many DATA elements will I have in my standard frame ? */
				lcl_fl1 = (DD_FEC_STD_WORD)((lcl_data32>>8) & 0xFF);
				lcl_fl2 = (DD_FEC_STD_WORD)(lcl_data32 & 0xFF);
				#ifdef DEBUG_dd_read_fifo
					printk("In function : length analysis\n");
					printk("MSB 8 bits are lcl_fl1 = 0x%x\n",lcl_fl1);
					printk("LSB 8 bits are lcl_fl2 = 0x%x\n",lcl_fl2);
				#endif

				if ((lcl_fl1 & 0x0080)==0)
				{

					lcl_frame_length = lcl_fl1 +3;
					#ifdef DEBUG_dd_read_fifo
						printk("I'm in short frame case\n");
						printk("Partial computed length is : 0x%x\n",lcl_frame_length);
					#endif
				}
				else
				{
					lcl_frame_length = ((((lcl_fl1 & 0x007F) << 8) + lcl_fl2) +4);
					#ifdef DEBUG_dd_read_fifo
						printk("I'm in long frame case\n");
						printk("Partial computed length is : 0x%x\n",lcl_frame_length);
					#endif
				}
				/* increment data frame length of 1 item (+1) for the status byte to add
				because when splitted in 32 bits words, the last standard word
				status(8bits) -- data (8 bits) == 1 frame element is seen as 2 elements
				when read from fifo receive */
				lcl_frame_length++;

				/* now, check how many 32 bits elements we must extract from the
				 fifo receive to fill our standard frame */
				 /* DBG26
				lcl_ref = (double)( (double)lcl_frame_length / 4 );
				lcl_div = (int)(lcl_frame_length/4);
				lcl_frame_32_length = lcl_div;

				if ( lcl_ref > (double)lcl_div)
				{
					lcl_frame_32_length++;
				}
				*/
				lcl_frame_32_length = lcl_frame_length / 4;
				if ( (lcl_frame_length % 4) != 0 ) lcl_frame_32_length++;

				#ifdef DEBUG_dd_read_fifo
					printk("I have computed a frame length of 0x%x words of 32 bits.\n", lcl_frame_32_length);
				#endif
			}
			lcl_ncar32++;

		} while (lcl_ncar32 < lcl_frame_32_length);


		/* switch to 16 bits representation even if we have a memory overflow
		This can be useful for further analysis */
		if (lcl_ncar32 > DD_MAX_MSG_LENGTH_32)
		{
			dd_uncat_from_32_to_std(lcl_buf, lcl_buf32, &lcl_length, DD_MAX_MSG_LENGTH_32);
			lcl_ncar = (int)(lcl_ncar32*4);
		}
		else
		{

			//switch to 16 bits representation
			dd_uncat_from_32_to_std(lcl_buf, lcl_buf32, &lcl_length, lcl_ncar32);
			lcl_ncar = lcl_length;
			lcl_status = lcl_buf[lcl_length-1];
			#ifdef DEBUG_dd_read_fifo
				printk("Un-cated frame is :\n");
				for (lcl_j = 0 ; lcl_j < lcl_ncar ; lcl_j++)
				{
					printk("    0x%x\n", lcl_buf[lcl_j]);
				}
				printk("frame length : 0x%x   ;;;   Frame status : 0x%x\n",lcl_ncar, lcl_status);
			#endif
		}

//#else
//#ifdef IS_FIFO_16BITS
		}
		else
		{
		
		do
		{
			lcl_data = (*(DD_TYPE_ADDRESS *)( glb_plx_remapped_ba_array[DD_PLX_FEC_INDEX] + (DD_TYPE_PCI_BASE_ADDRESS)dd_fec_fiforec_offset));
			//dd_internal_read_from_fec(dd_fec_fiforec_offset,&lcl_data);
			#ifdef DD_SPY_DEBUG
				printk("\t0x%x : \n",lcl_data);
			#endif
			/*  buffer not full, store data. Else, avoid buffer overflow */
			if (lcl_ncar < DD_USER_MAX_MSG_LENGTH) lcl_buf[lcl_ncar] = lcl_data;
			lcl_ncar++;
			if (lcl_data & DD_EOF_BITMASK)
			{
				lcl_status = lcl_data;
				lcl_flag = DD_FIFOREC_EOFRAME_READ;
			}
		} while (lcl_flag == DD_FIFOREC_EOFRAME_NOT_READ);

//#endif
//#endif
		}

		/*! a priori, set lcl_err to noerror */
		lcl_err=DD_RETURN_OK;

		/*! check msg length consistency ; msg too long */
		/*! suite a cette erreur, on va relooper sur le while. */
		/*! soit on finit par lire au moins une bonne frame */
		/*! soit on ressort sur une erreur STUCK_IN_INTERRUPT */
		if (lcl_ncar > DD_USER_MAX_MSG_LENGTH)
		{
			//return DD_RECEIVED_MSG_TOO_LONG;
			//Store information somewhere...
			lcl_err = DD_RECEIVED_MSG_TOO_LONG;
			if (glb_enable_count_of_errors_on_ring == DD_FLAG_IS_ENABLED) glb_rem_counter[DD_LONG_FRAME_ERROR_OFFSET]++;
		}


		/*! check msg length consistency ; msg too short*/
		/*! suite a cette erreur, on va relooper sur le while. */
		/*! soit on finit par lire au moins une bonne frame */
		/*! soit on ressort sur une erreur STUCK_IN_INTERRUPT */
		if (lcl_ncar <= DD_NOISE_LENGTH)
		{
			#ifdef DD_SPY_DEBUG
				printk("Noise detected on ring. Frame trashed.\n");
			#endif
			//Trash frame
			//return DD_RING_NOISY;
			lcl_err = DD_RING_NOISY;
			if (glb_enable_count_of_errors_on_ring == DD_FLAG_IS_ENABLED) glb_rem_counter[DD_SHORT_FRAME_ERROR_OFFSET]++;
		}

		/*! get the transaction number */
		if (isLongFrame(lcl_buf[DD_MSG_LEN_OFFSET]))
		{
			lcl_trans = (int)lcl_buf[DD_TRANS_NUMBER_OFFSET+1];
		}
		else
		{
			lcl_trans = (int)lcl_buf[DD_TRANS_NUMBER_OFFSET];
		}

		/*! check transaction number consistency */
		/*! suite a cette erreur, on va relooper sur le while. */
		/*! soit on finit par lire au moins une bonne frame */
		/*! soit on ressort sur une erreur STUCK_IN_INTERRUPT */
		if ( (lcl_trans < DD_MIN_TRANS_NUMBER) || (lcl_trans > DD_MAX_TRANS_NUMBER) )
		{
			if (lcl_trans == 0)
			{
				printk("Warning emitted by a device ; see /var/log/messages for more detail.\n");
				printk("Received message is :\n");
				if (lcl_ncar > DD_USER_MAX_MSG_LENGTH) lcl_ncar = DD_USER_MAX_MSG_LENGTH;
				for (lcl_j = 0 ; lcl_j < lcl_ncar ; lcl_j++)
				{
					//copy warning frame to buffer
					glb_warning_buf[lcl_j]=lcl_buf[lcl_j];
					printk("Word %i : 0x%x\n", lcl_j, lcl_buf[lcl_j]);
				}
				/*! Rem : lcl_err not setted here, so param_device_info->received[lcl_trans] */
				/*! will be filled further on with this error frame */
				if (glb_enable_count_of_errors_on_ring == DD_FLAG_IS_ENABLED) glb_rem_counter[DD_WARNING_FROM_DEVICE_ERROR_OFFSET]++;
				//wake up wait_warning methods that could be waiting here
				wake_up_interruptible(&glb_wq_devices_warning);
			}
			else
			{
				lcl_err = DD_RECEIVED_WRONG_TRANS_NUMBER;
				if (glb_enable_count_of_errors_on_ring == DD_FLAG_IS_ENABLED) glb_rem_counter[DD_WRONG_TRANS_NUMBER_ERROR_OFFSET]++;
			}

			/*! Rem : Broadcast returns fall in this else block, */
			/*! and are seen as fake frames. Anyway, no useful checks */
			/*! can be performed on broadcast returns, so it doesn't matter. */
		}




		if ( (lcl_err == DD_RETURN_OK) || (lcl_err == DD_RECEIVED_MSG_TOO_LONG) )
		{

			/* Should be totally unuseful because this is hardly checked
			before, but ones never knows... */
			if (lcl_ncar > DD_USER_MAX_MSG_LENGTH)
			{
				lcl_limit=DD_USER_MAX_MSG_LENGTH;
			}
			else lcl_limit = lcl_ncar;


			if (lcl_buf[DD_DESTINATION_OFFSET] == DD_FEC_IS_MSG_DESTINATION)
			/*! i.e. lecture data frame */
			/*! or could be an ACK */
			{
				for (lcl_j = 0 ; lcl_j < lcl_limit ; lcl_j++)
				{
					glb_received[lcl_trans][lcl_j] = lcl_buf[lcl_j];
				}


				#ifdef DD_SPY_DEBUG
					printk("In dd_read_fifo : Received frame is answer to a read request or a f-ack.\n");
				#endif

				if (lcl_err == DD_RETURN_OK)
				{
					/*! if command is acknowledged by I2C device : */
					if ( ( (lcl_status & DD_FEC_STATUS_FLAGS) == DD_FEC_NO_ERROR ) || ((lcl_status & I2C_HAVE_ACKED_COMMAND)==I2C_HAVE_ACKED_COMMAND) )
					{
						/*! Rem : DD_FEC_NO_ERROR is 0x0000 */
						glb_read[lcl_trans] = DD_READ_DATA_SUCCESSFUL;
						#ifdef DD_SPY_DEBUG
							printk("\tRead request is sucessful.\n");
						#endif
					}
					else
					{
						glb_read[lcl_trans] = DD_READ_DATA_CORRUPTED;
						#ifdef DD_SPY_DEBUG
							printk("\tRead request is un-sucessful (data corrupted).\n");
						#endif
					}
				}
				else
				{
					glb_read[lcl_trans] = DD_READ_DATA_OVERFLOWED;
					#ifdef DD_SPY_DEBUG
						printk("\tRead request answer is too big ; internal buffer overflowed .\n");
					#endif
				}


				/*! wake up the READ METHOD task waiting for read DATA */
				wake_up_interruptible(&glb_wq_read);
				/*! Watchdog timer and transaction number are freed only when a READ
				request is issued from client side, for this given transaction number. */
			}
			else /*! lecture frame retour ; frame sent to a CCU and coming back modified */
			{
				#ifdef DD_SPY_DEBUG
					printk("In dd_read_fifo : Received frame is a direct acknowledge return.\n");
				#endif
				for (lcl_j = 0 ; lcl_j < lcl_limit ; lcl_j++)
				{
					glb_back[lcl_trans][lcl_j] = lcl_buf[lcl_j];
				}

				if (lcl_err == DD_RETURN_OK)
				{
					/*! if address seen and data copied */
					if ((lcl_status & DD_FEC_STATUS_FLAGS) == DD_FEC_ADDR_SEEN_AND_DATA_COPIED)
					{
						/*! Rem : DD_COPY_DATA_SUCCESSFUL is 0x0000 */
						glb_copied[lcl_trans] = DD_COPY_DATA_SUCCESSFUL;
						#ifdef DD_SPY_DEBUG
							printk("\tDirect acknowledge is sucessful.\n");
						#endif
					}
					/*! else, store received frame status in copied field */
					else
					{
						glb_copied[lcl_trans] = DD_COPY_DATA_CORRUPTED;
						#ifdef DD_SPY_DEBUG
							printk("\tDirect acknowledge request is un-sucessful (frame status corrupted).\n");
						#endif
					}
				}
				else
				{
						glb_copied[lcl_trans] = DD_COPY_DATA_OVERFLOWED;
						#ifdef DD_SPY_DEBUG
							printk("\tIncoming direct acknowledge is too big ; internal buffer overflowed .\n");
						#endif
				}

				/*! wake up the WRITE METHOD task waiting for ACK */
				wake_up_interruptible(&glb_wq_write);
			}
		} /*! end check sur lcl_err */

	} //end of while



return lcl_err;
}






/*!
<b>FUNCTION 	: fecpmc_Interrupt</b>
- Job
	- Interrupt manager
- Remark
	- use of device info with GLOBAL access ; not worth to fake
	a param_access for command lines
- Inputs
	- integer : interrupt number
	- *dev_id : identifier of the peripheral that raised the
	interrupt
	- regs* : system registers
- Outputs
	- Void
- Internals
	- global var glb_errors_in_irq used on read
	- global var glb_device_info transposed to pointer
	param_device_info
		- <b>Fields used</b>
		- ->error_in_interrupt
		- ->plx_ba_infos
		- ->plx_offset_and_value
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
- Error Management & Values returned
	- !!! no error returned ; glb_device_info->error_in_interrupt is set instead !!!
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_CANNOT_EXIT_FROM_IRQ_READ_DATA
		- DD_TOO_MUCH_NC_ERRORS_IN_IRQ
- Sub-functions calls
	- dd_stack_msg
	- save_flags	(system)
	- cli		(system)
	- dd_read_fifo
	- dd_write_to_plx
	- restore_flags	(system)
*/


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
irqreturn_t dd_fecpmc_Interrupt(int irq, void *dev_id, struct pt_regs *regs)
#else
static void dd_fecpmc_Interrupt(int irq, void *dev_id, struct pt_regs *regs)
#endif
{

DD_TYPE_ERROR lcl_err;
DD_FEC_REGISTER_DATA lcl_fec_value;


	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
		save_flags(glb_irq_flags);
		cli();
	#else
		spin_lock_irqsave(&glb_irq_lock, glb_irq_flags);
	#endif
	dd_internal_read_from_fec_register(DD_FEC_STAT0_OFFSET, &lcl_fec_value);

	if (!(lcl_fec_value & DD_FEC_PENDING_IRQ))
	{
		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
			restore_flags(glb_irq_flags);
		#else
			spin_unlock_irqrestore(&glb_irq_lock, glb_irq_flags);
		#endif
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			return IRQ_HANDLED;
		#else
			return;
		#endif
	}


	/* Patch added to bypass the irqs that can't be disbled by
	hardware on some fecs.
	This implies that to enable/disable irq's people MUST use
	the glue layer functions or the ioctl() calls.
	Hacking directly into the PLX registers will certainly cause a lot
	of problems (hardware status != software status).*/
	if (glb_irq_status == DD_FLAG_IS_DISABLED)
	{
		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
			restore_flags(glb_irq_flags);
		#else
			spin_unlock_irqrestore(&glb_irq_lock, glb_irq_flags);
		#endif
		dd_internal_write_to_fec_register(DD_FEC_CTRL1_OFFSET, (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS));
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			return IRQ_HANDLED;
		#else
			return;
		#endif

	}
	

	/*! read fifo receive */
	/*! Rem : current waiting processes are awaken from dd_read_fifo function */
	lcl_err=dd_read_fifo();

	/*! if a non critical error occurs, inc non-critical errors counter */
	if ( (lcl_err == DD_RECEIVED_WRONG_TRANS_NUMBER) || (lcl_err == DD_RECEIVED_MSG_TOO_LONG) || (lcl_err == DD_RING_NOISY) )
	{
		glb_errors_in_irq++;
	}


	/*! if we are stuck in interrupt because coherent frames came so fast on the ring that it is */
	/*! impossible to exit irq manager between two frames, it is a blocking error. */
	/*! Once detected, this situation have to be managed by user. */
	/*! all the waiting tasks (read and write) and future incoming tasks */
	/*! will return the error code DD_CANNOT_EXIT_FROM_IRQ_READ_DATA */
	if (lcl_err == DD_CANNOT_EXIT_FROM_IRQ_READ_DATA)
	{
		/*! disable PLX interrupt line until probleme is solved. */
		/*! In this case, PLX interrupt line must be reinitialised by user */
		#ifdef DD_ENABLE_IRQMANAGER
			dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_DISABLED_VALUE);
		#endif

		/* clear fec flags */
		dd_internal_write_to_fec_register(DD_FEC_CTRL1_OFFSET, (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS));

		/* Print a warning in /var/log/messages */
		printk("Arggghhhh! Stop hammering me with interrupts!\n");
		printk("I have disabled the PLX interrupts line.\n");
		printk("You will have to manually restore the system health.\n");

		/* set flag for PLX-IRQ status management */
		/* Rem : no need to check if we had previously setted the flag */
		/* this is not a flip-flop but a global stable state */
		glb_irq_status = DD_FLAG_IS_DISABLED;

		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
			restore_flags(glb_irq_flags);
		#else
			spin_unlock_irqrestore(&glb_irq_lock, glb_irq_flags);
		#endif
		/*! exit from interrupt */
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			return IRQ_HANDLED;
		#else
			return;
		#endif

	}


	if (glb_errors_in_irq >= DD_MAX_NC_ERRORS_IN_IRQ)
	{
		/*! disable PLX interrupt line until probleme is solved. */
		/*! In this case, PLX interrupt line must be reinitialised by user */
		#ifdef DD_ENABLE_IRQMANAGER
			dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_DISABLED_VALUE);
		#endif

		/* clear fec flags */
		dd_internal_write_to_fec_register(DD_FEC_CTRL1_OFFSET, (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS));

		/* Print a warning in /var/log/messages */
		printk("Looks like you have a problem!\n");
		printk("I have seen more than %d consecutive errors on the ring.\n",DD_MAX_NC_ERRORS_IN_IRQ);
		printk("I have disabled the PLX interrupts line.\n");
		printk("You will have to manually restore the system health.\n");

		/* set flag for PLX-IRQ status management */
		/* Rem : no need to check if we had previously setted the flag */
		/* this is not a flip-flop but a global stable state */
		glb_irq_status = DD_FLAG_IS_DISABLED;

		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
			restore_flags(glb_irq_flags);
		#else
			spin_unlock_irqrestore(&glb_irq_lock, glb_irq_flags);
		#endif
		/*! exit from interrupt */
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
			return IRQ_HANDLED;
		#else
			return;
		#endif

	}


	/*! if transaction was well performed, reset non-critical errors counter */
	if (lcl_err == DD_RETURN_OK) glb_errors_in_irq = 0;
	/* Clear interrupt */
	dd_internal_write_to_fec_register(DD_FEC_CTRL1_OFFSET, (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS));
	/* restore system state */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
		restore_flags(glb_irq_flags);
	#else
		spin_unlock_irqrestore(&glb_irq_lock, glb_irq_flags);
	#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	return IRQ_HANDLED;
#else
	return;
#endif

}



/*!
<b>FUNCTION 	: dd_install_interrupt_manager</b>
- Job
	- Install the Interrupt manager ; called when insmod
	is invoked on command line
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->irq_number
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_CANNOT_INSTALL_IRQ_MANAGER
- Sub-functions calls
	- dd_stack_msg
	- request_irq	(system)
*/
static DD_TYPE_ERROR dd_install_interrupt_manager(void)
{
DD_TYPE_ERROR lcl_err = DD_RETURN_OK;

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	/*! Assignation de la routine d'interruption */
	if (glb_fec_type == 0)
	{
	        lcl_err = request_irq(glb_irq_number, dd_fecpmc_Interrupt, (SA_INTERRUPT | SA_SHIRQ), "FEC", (void *)(DD_DEVICE_ID_ELECTRICAL_V0));
	}
	if (glb_fec_type == 1)
	{
	        lcl_err = request_irq(glb_irq_number, dd_fecpmc_Interrupt, (SA_INTERRUPT | SA_SHIRQ), "FEC", (void *)(DD_DEVICE_ID_OPTICAL_V0));
	}
	if (glb_fec_type == 2)
	{
	        lcl_err = request_irq(glb_irq_number, dd_fecpmc_Interrupt, (SA_INTERRUPT | SA_SHIRQ), "FEC", (void *)(DD_DEVICE_ID_OPTICAL_V1));
	}

	#else
	/*! Assignation de la routine d'interruption */
	if (glb_fec_type == 0)
	{
	        lcl_err = request_irq(glb_irq_number, &dd_fecpmc_Interrupt, (SA_INTERRUPT | SA_SHIRQ), "FEC", (void *)(DD_DEVICE_ID_ELECTRICAL_V0));
	}
	if (glb_fec_type == 1)
	{
	        lcl_err = request_irq(glb_irq_number, &dd_fecpmc_Interrupt, (SA_INTERRUPT | SA_SHIRQ), "FEC", (void *)(DD_DEVICE_ID_OPTICAL_V0));
	}
	if (glb_fec_type == 2)
	{
	        lcl_err = request_irq(glb_irq_number, &dd_fecpmc_Interrupt, (SA_INTERRUPT | SA_SHIRQ), "FEC", (void *)(DD_DEVICE_ID_OPTICAL_V1));
	}

	#endif

	if (lcl_err != DD_RETURN_OK) return DD_CANNOT_INSTALL_IRQ_MANAGER;

return DD_RETURN_OK;
}



/*!
<b>FUNCTION 	: dd_remove_interrupt_manager</b>
- Job
	- Remove the Interrupt manager ; called when rmmod is
	invoked on command line
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->irq_number
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void
- Error Management & Values returned
	- Void
- Sub-functions calls
	- dd_stack_msg
	- free_irq	(system)

*/

static void dd_remove_interrupt_manager(void)
{
	/*! Suppression de la routine d'interruption */
	if (glb_fec_type == 0)
	{
	        free_irq(glb_irq_number, (void *)(DD_DEVICE_ID_ELECTRICAL_V0));
	}
	if (glb_fec_type == 1)
	{
	        free_irq(glb_irq_number, (void *)(DD_DEVICE_ID_OPTICAL_V0));
	}
	if (glb_fec_type == 2)
	{
	        free_irq(glb_irq_number, (void *)(DD_DEVICE_ID_OPTICAL_V1));
	}

}






//////////////////////
//DEVICE XXX
//////////////////////




/*!
<b>FUNCTION 0000	: dd_check_ioc_requests</b>

- Job
	- Check IOCTL commands consistency, and find destination
	sub-routine (PCI | PLX | FEC)
- Inputs
	- struct pointer to a DD_device_info struct
		- <b>Fields used :</b>
		- ->ioc_command
		- ->ioc_container
		- ->ioc_command_kind
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- DD_BAD_IOC_DESTINATION
		- DD_KS_CANNOT_WRITE_TO_US_IN_HUB
		- DD_KS_CANNOT_READ_FROM_US_IN_HUB
- Sub-functions calls
	- dd_stack_msg
	- access_ok	(system)
*/

static DD_TYPE_ERROR dd_check_ioc_requests(struct dd_ioctl_data * param_ioctl_data)
{

/*! Declarations */
int lcl_resu;

	/*! check if the IOCTL command is for this driver */
	if((_IOC_TYPE(param_ioctl_data->ioctl_command) != DD_IOC_MAGIC_PCI) && (_IOC_TYPE(param_ioctl_data->ioctl_command) != DD_IOC_MAGIC_PLX) && (_IOC_TYPE(param_ioctl_data->ioctl_command) != DD_IOC_MAGIC_FEC)) return DD_BAD_IOC_DESTINATION;

	/*! check if the IOCTL command is a request for PCI, PLX or FEC */
	if ((_IOC_TYPE(param_ioctl_data->ioctl_command) == DD_IOC_MAGIC_PCI)) param_ioctl_data->command_kind = DD_IOC_COMMAND_KIND_PCI;
	if ((_IOC_TYPE(param_ioctl_data->ioctl_command) == DD_IOC_MAGIC_PLX)) param_ioctl_data->command_kind = DD_IOC_COMMAND_KIND_PLX;
	if ((_IOC_TYPE(param_ioctl_data->ioctl_command) == DD_IOC_MAGIC_FEC)) param_ioctl_data->command_kind = DD_IOC_COMMAND_KIND_FEC;

	/*! if we have a READ request (KS->US), check if destination is writeable */
	if (_IOC_DIR(param_ioctl_data->ioctl_command) & _IOC_READ)
	{
		lcl_resu = (int)access_ok(VERIFY_WRITE, (void *)param_ioctl_data->ioctl_address, (_IOC_SIZE(param_ioctl_data->ioctl_command)));
		if (lcl_resu != DD_MEMORY_ACCESS_OK) return DD_KS_CANNOT_WRITE_TO_US_IN_HUB;
	}

	/*! if we have a WRITE request (US->KS), check if source is readeable */
	if (_IOC_DIR(param_ioctl_data->ioctl_command) & _IOC_WRITE)
	{
		lcl_resu=(int)access_ok(VERIFY_READ, (void *)param_ioctl_data->ioctl_address, (_IOC_SIZE(param_ioctl_data->ioctl_command)));
		if (lcl_resu != DD_MEMORY_ACCESS_OK) return DD_KS_CANNOT_READ_FROM_US_IN_HUB;
	}

return DD_RETURN_OK;
}







/*!printk("IN WRITE\n");

<b>FUNCTION 0022	:	dd_ioctl_entrypoint</b>

- Job
	- Check IOCTL requests consistency, and call HUB function for
	work dispatch between PCI, PLX and FEC
	- Rem : EntryPoint in Kernel Space for almost all my User Space functs.
- Inputs
	- inode + file_pointer , i.e. file descriptor
	- IOCTL Command
	- IOCTL R/W address (pointer)
- Outputs
	- Error Code
- Internals
	- global var glb_device_info transposed to *param_device_info ;
	Fields setted :
		- ->ioc_command (setted to IOCTL Command)
		- ->ioc_container (setted to IOCTL R/W address)
- Notes
	- Nothing
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors
		- DD_MAIN_ERROR_MODULE_IS_RESETTING
		- sub-functions EVT
- Sub-functions calls
	- dd_stack_msg
	- dd_check_ioc_requests		(EVT)
	- dd_give_handle_to_pci_ioctl	(EVT)
	- dd_give_handle_to_plx_ioctl	(EVT)
	- dd_0200_give_handle_to_fec_ioctl	(EVT)
*/

DD_TYPE_IOCTL_RETURN dd_ioctl_entrypoint(struct inode *iNode, struct file *filep, DD_TYPE_IOC_COMMAND param_cmd, DD_TYPE_ADDRESS param_arg)
{

DD_TYPE_ERROR lcl_err;
struct dd_ioctl_data lcl_ioctl_data;
struct dd_ioctl_data *param_ioctl_data;

if (fec_detected == 0)
{
	printk("FATAL : The FEC driver is loaded but no PCI FEC board has been detected on this PC. Driver access denied.\n");
	return DD_NO_FEC_BOARD_DETECTED;
}
	/* This command should always work and must not be blocked by the ioctl() entrypoint semaphore */
	if ( ((_IOC_TYPE(param_cmd) == DD_IOC_MAGIC_FEC)) && (param_cmd == DD_IOC_FEC_FORCE_COUNT_TO_ONE) )
	{
		lcl_err = dd_force_count_to_one();
		return (DD_TYPE_IOCTL_RETURN)lcl_err;
	}

	/* Now, lock the entrypoint with a semaphore */
	if (down_interruptible(&(glb_ioctl_lock)) ) return DD_SIG_ON_IOCTL_SEM;

	/*! Are we in a driver reset phase ? If yes, exit */
	if (glb_atomic_flags & DD_SOFTRESET_RUNNING)
	{
		up(&(glb_ioctl_lock));
		return DD_MAIN_ERROR_MODULE_IS_RESETTING;
	}


	/*! function-parameter notation compatibility */
	/*! global vars glb_device_info and glb_device_ptr will now be accessed */
	/*! via respective pointers param_device_info and param_device_ptr */
	lcl_ioctl_data.ioctl_address = param_arg;
	lcl_ioctl_data.ioctl_command = param_cmd;

	param_ioctl_data = &lcl_ioctl_data;

	/*! check request consistency */
	lcl_err = dd_check_ioc_requests(param_ioctl_data);
	if (lcl_err != DD_RETURN_OK)
	{
		up(&(glb_ioctl_lock));
		return (DD_TYPE_IOCTL_RETURN)lcl_err;
	}


	/*! If IOCTL request is dedicated to PCI, switch to PCI handler */
	if (param_ioctl_data->command_kind == DD_IOC_COMMAND_KIND_PCI)
	{
		lcl_err = dd_give_handle_to_pci_ioctl(&glb_device_ptr, param_ioctl_data);
		if (lcl_err != DD_RETURN_OK)
		{
			up(&(glb_ioctl_lock));
			return (DD_TYPE_IOCTL_RETURN)lcl_err;
		}
	}

	/*! If IOCTL request is dedicated to PLX, switch to PLX handler */
	if (param_ioctl_data->command_kind == DD_IOC_COMMAND_KIND_PLX)
	{
		lcl_err = dd_give_handle_to_plx_ioctl(param_ioctl_data);
		if (lcl_err != DD_RETURN_OK)
		{
			up(&(glb_ioctl_lock));
			return (DD_TYPE_IOCTL_RETURN)lcl_err;
		}
	}

	/*! STEP 4 */
	/*! If IOCTL request is dedicated to FEC, switch to FEC handler */
	if (param_ioctl_data->command_kind == DD_IOC_COMMAND_KIND_FEC)
	{
		lcl_err = dd_0200_give_handle_to_fec_ioctl(param_ioctl_data);
		if (lcl_err != DD_RETURN_OK)
		{
			up(&(glb_ioctl_lock));
			return (DD_TYPE_IOCTL_RETURN)lcl_err;
		}
	}

	up(&(glb_ioctl_lock));

return (DD_TYPE_IOCTL_RETURN)DD_RETURN_OK;

}






/*!
<b>FUNCTION 0023	: dd_open_fecdriver</b>
- Job
	- Open the device driver, and increment Current Users counter
- Inputs
	- inode + file_pointer , i.e. file descriptor
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void
- Error Management & Values returned
	- Operation successful
		- DD_RETURN_OK
	- Errors
		- None
- Sub-functions calls
	- dd_stack_msg
*/

DD_TYPE_OPEN_RETURN dd_open_fecdriver(struct inode *iNode, struct file *filep)
{
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		/*! increment number-of-users counter */
		MOD_INC_USE_COUNT;
	#endif
	

return (DD_TYPE_OPEN_RETURN)DD_RETURN_OK;
}



/*!
<b>FUNCTION 0024	: dd_release_fecdriver</b>
- Job
	- Close the device driver, and decrement Current Users counter
- Inputs
	- inode + file_pointer , i.e. file descriptor
- Outputs
	- Error Code
- Internals
	- Void
- Notes
	- Void
- Error Management & Values returned
	- Operation successful
		- DD_RETURN_OK
	- Errors
		- None
- Sub-functions calls
	- dd_stack_msg
*/

DD_TYPE_RELEASE_RETURN dd_release_fecdriver(struct inode *iNode, struct file *filep)
{
	/*! decrement number-of-users counter */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		MOD_DEC_USE_COUNT;
	#endif

return (DD_TYPE_RELEASE_RETURN)DD_RETURN_OK;
}





/*! Declare system functions OPEN, IOCTL and RELEASE redefined for this driver */
struct file_operations fecpmc_fops={
	open:dd_open_fecdriver,
	ioctl:dd_ioctl_entrypoint,
	release:dd_release_fecdriver,
};












//void cleanup_module(void)
static void fastfec_exit(void)
{

/*! declarations */
DD_TYPE_ERROR lcl_err;
int lcl_i;


	/*! DEBUG INFO */
	printk("Begin to clean up module...\n");

	/*! Invalidate PLX interrupts */
	#ifdef DD_ENABLE_IRQMANAGER
		dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_DISABLED_VALUE);
		printk("PLX interrupts disabled.\n");
	#endif


	/*! Liberation de la routine d'interruption - void returned */
	#ifdef DD_ENABLE_IRQMANAGER
		dd_remove_interrupt_manager();
		printk("Interrupt manager uninstalled.\n");
	#endif

	/* Now, reset wait queues  and clear timers*/
	for (lcl_i = 0; lcl_i <= DD_MAX_TRANS_NUMBER; lcl_i++)
	{
		del_timer(&(glb_fec_trans_number_timers[lcl_i]));
	}


	/*! unmap memory if needed */
	for (lcl_i = 0 ; lcl_i < (DD_NBR_OF_PCI_BASE_ADDRESSES) ; lcl_i++)
	{
		if (glb_plx_rmapsize_ba_array[lcl_i] > 0)
		{
			iounmap((DD_TYPE_PCI_BASE_ADDRESS *)glb_plx_remapped_ba_array[lcl_i]);
		}
	}
	printk("PLX addresses unmapped\n");


	/*! D�enregistrement Kernel du Major du module */
	lcl_err = (DD_TYPE_ERROR)unregister_chrdev(glb_module_major, DD_MODULE);
	/*! si pb, on previens */
	if ( lcl_err < DD_UNREGISTER_MODULE_OK_LIMIT )
	{
		printk("Error while un-registering module in kernel\n");
	}
	else printk("Module ungeristered and successfully unloaded from memory.\n");


}




/*!
<b>FUNCTION	: init_module</b>
- Job
	- Hardware detection + Initialisation of module properties
	when loaded
- Inputs
	- Void
- Outputs
	- Error Code
- Internals
	- global var glb_atomic_flags is setted
	- global var glb_errors_in_irq is setted
	- global var glb_module_major is setted
	- global var pci device_pointer is used
	- global var struct_info is wrapped to a pointer struct_info*
		- <b>Fields used :</b>
		- ->ident_vendor
		- ->ident_device
		- ->plx_offset_and_value
		- ->fec_offset_and_value
		- ->plx_ba_sizes[0]
		- ->plx_ba_infos
		- ->read_wait[]
		- ->write_wait[]
		- ->read[]
		- ->copied[]
		- ->fec_write_trans_in_use[]
		- ->currentTrans
		- ->fec_current_read_process_waiting
		- ->error_in_interrupt
- Notes
	- Void
- Error Management & Values returned
	- Operation successful
		- DD_RETURN_OK
	- Errors
		- DD_ERROR_REGISTER_MODULE
		- DD_TOO_MANY_PCI_BA
		- sub-functions EVT
- Sub-functions calls :
	- dd_stack_msg
	- strcpy							(system)
	- dd_ioc_check_if_system_is_pci_compliant		(EVT)
	- register_chrdev						(system)
	- dd_ioc_find_pci_card				(EVT)
	- dd_ioc_get_pci_card_base_addresses_and_mask	(EVT)
	- dd_ioc_get_pci_card_irq_number			(EVT)
	- dd_ioc_set_plx9080_addresses			(EVT)
	- dd_write_to_plx						(EVT)
	- dd_ioc_set_pci_card_command_reg			(EVT)
	- dd_install_interrupt_manager				(EVT)
	- dd_write_to_fec						(EVT)
*/

//DD_TYPE_INITMOD_RETURN init_module(void)
//int init_module(void)
static int fastfec_init(void)
{
DD_TYPE_ERROR lcl_err;
int lcl_i=0;
struct dd_ioctl_data lcl_ioctl_data;
DD_FEC_REGISTER_DATA lcl_fec_value;

	/*! System Output */
	printk("Initialising Module...\n");
	/* resets driver status to 0 */
	glb_driver_status = 0x0000;
	glb_irq_status = DD_FLAG_IS_DISABLED;

	/*! check if system is PCI compliant */
	lcl_err = dd_ioc_check_if_system_is_pci_compliant();
	if (lcl_err != DD_RETURN_OK) return (DD_TYPE_INITMOD_RETURN)lcl_err;
	printk("System is PCI Compliant.\n");

	/*! STEP 2 */
	/*! Is the device major number forced, or do wet let the system find one ? */
	#ifdef DD_MAJOR
		glb_module_major = DD_MAJOR;
	#else
		glb_module_major = DD_AUTO_MAJOR;
	#endif

	/*! Kernel registration of driver ; link the driver with its /dev/file */
	/*!  -DD_MAJOR- , forced or ZERO(auto-attrib), is used for /dev/driver_file */
	lcl_err = (DD_TYPE_ERROR)register_chrdev(glb_module_major, DD_MODULE, &fecpmc_fops);
	/*! Si pb, on arrete tout - si pas pb, c'est enregistre */
	if (lcl_err < DD_REGISTER_MODULE_OK_LIMIT)
	{
		printk("Error registering module ; Driver MAJOR assignation error.\n");
		return (DD_TYPE_INITMOD_RETURN)DD_ERROR_REGISTER_MODULE;
	}


	/*! If we are on auto-attrib-major mode, */
	/*! set -DD_MAJOR- to the value attributed by the system */
	if (glb_module_major == DD_AUTO_MAJOR) {glb_module_major = lcl_err;}


	/*! Auto detect the device identified by DEVICE_ID & VENDOR_ID */
	lcl_err = dd_ioc_find_pci_card(&glb_device_ptr);
	//return 1;
	if (lcl_err != DD_RETURN_OK)
	{
		printk("No FEC device number %d found on this system.\n",DD_FEC_NUMBER);
		fec_detected = 0;
		return (DD_TYPE_INITMOD_RETURN)lcl_err;
	}
	else fec_detected = 1;
	
	if (glb_fec_type == 0)
	{
		printk("Electrical FEC Device number %d, version 0 (VENDORID/DEVICEID == %x/%x) detected.\n", DD_FEC_NUMBER, glb_vendID, glb_devID);

	}
	if (glb_fec_type == 1)
	{
		printk("Optical FEC Device number %d, version 0 (VENDORID/DEVICEID == %x/%x) detected.\n", DD_FEC_NUMBER, glb_vendID, glb_devID);

	}
	if (glb_fec_type == 2)
	{
		printk("Electrical/Optical FEC Device number %d, version 1 (VENDORID/DEVICEID == %x/%x) detected.\n", DD_FEC_NUMBER, glb_vendID, glb_devID);

	}


	/*! Get PCI board base addresses */
	lcl_err = dd_ioc_get_pci_card_base_addresses_and_mask(&glb_device_ptr, &lcl_ioctl_data, DD_INTERNAL_COMMAND);
	if (lcl_err != DD_RETURN_OK)
	{
		printk("Cannot read FEC/PLX base addresses.\n");
		fec_detected = 0;
		return (DD_TYPE_INITMOD_RETURN)lcl_err;
	} else fec_detected = 1;


	/*! Get PCI board Interrupt line number */
	lcl_err = dd_ioc_get_pci_card_irq_number(&glb_device_ptr, &lcl_ioctl_data, DD_INTERNAL_COMMAND);
	if (lcl_err != DD_RETURN_OK)
	{
		printk("Cannot get FEC interrupt line number.\n");
		fec_detected = 0;
		return (DD_TYPE_INITMOD_RETURN)lcl_err;
	} else fec_detected = 1;



/* First, make a minimal remap of PLX BARs.
This will be used to check which firmware is embedded into the FEC
According to this, a permanent remap will be done. */

	/*********************************************/
	/*! Pre-assignation des tailles de remapping */
	/*********************************************/
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 0) glb_plx_rmapsize_ba_array[0] = DD_BA0_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 1) glb_plx_rmapsize_ba_array[1] = DD_BA1_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 2) glb_plx_rmapsize_ba_array[2] = DD_BA2_SIZE_FOR_REMAP_OV1;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 3) glb_plx_rmapsize_ba_array[3] = DD_BA3_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 4) glb_plx_rmapsize_ba_array[4] = DD_BA4_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 5) glb_plx_rmapsize_ba_array[5] = DD_BA5_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 6) glb_plx_rmapsize_ba_array[6] = DD_BA6_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 7)
	{
		printk("Too much base addresses (>7) defined for the PCI device.\n");
		return (DD_TYPE_INITMOD_RETURN)DD_TOO_MANY_PCI_BA;
	}

	/*! Remap PCI board base addresses for R/W/CONFIG operations */
	lcl_err = dd_ioc_set_plx9080_addresses();
	if (lcl_err != DD_RETURN_OK)
	{
		printk("Cannot Pre-remap FEC base addresses.\n");
		return (DD_TYPE_INITMOD_RETURN)lcl_err;
	}
	printk("FEC base addresses successfully pre-remapped.\n");

	/*! HardReset of PLX */
	dd_internal_write_to_plx(DD_PLX9080_CTRL_OFFSET, (DD_PLX_CTRL_VALUE | DD_PLX_CTRL_RESET_BIT));

	/*! Initialisation of PLX bridge */
	dd_internal_write_to_plx(DD_PLX9080_CTRL_OFFSET, DD_PLX_CTRL_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_BIGEND_OFFSET, DD_PLX_BIGEND_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_MARBR_OFFSET, DD_PLX_MARBR_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_LAS0RR_OFFSET, DD_PLX_LAS0RR_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_LAS0BA_OFFSET, DD_PLX_LAS0BA_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_LBRD0_OFFSET, DD_PLX_LBRD0_VALUE);

	/*! Invalidate PLX interrupts */
	//ONLY CASE where we will touch the PLX IRQ line if #ifdef DD_ENABLE_IRQMANAGER
	dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_DISABLED_VALUE);
	printk("PLX bridge resetted and configured.\n");


	/*! initialisation PCI control register */
	/*! autorise les acces I/O space et memory space */
	lcl_err = dd_ioc_set_pci_card_command_reg(&glb_device_ptr, &lcl_ioctl_data, DD_INTERNAL_COMMAND);
	if (lcl_err != DD_RETURN_OK)
	{
		printk("Error during PCI command register initialisation.\n");
		return (DD_TYPE_INITMOD_RETURN)lcl_err;
	}



	/*********************************************/
	/* Check the FEC firmware Version            */
	/*********************************************/


	printk("FEC type seems to be %d\n",glb_fec_type);
	printk("Reading FEC Firmware Version ...\n");
	dd_internal_read_from_fec_register(DD_FEC_FIRMWARE_VERSION_OFFSET, &lcl_fec_value);
	if (glb_fec_type != 0)
	{
		printk("...Firmware Version read from FEC is : 0x%x\n", lcl_fec_value);
	}
	else
	{
		printk("...oh yes, old electrical FECs do not have Firmware Versions!\n");
		printk("INFO : This situation is OK, you have nothing to do.\n");
	}


	if ((lcl_fec_value < 0x0300) && (glb_fec_type == 1))
	{
		printk("WARNING : Your FEC is an Old Optical FEC (v.0).\n");
		printk("WARNING : It's firmware version should be at least 0x300.\n");
		printk("WARNING : This is not critical, but think to upgrade your firmware.\n");
	}
	
	
	if ((lcl_fec_value == 0x0300) && (glb_fec_type == 1))
	{
		printk("INFO : Your FEC is an Old Optical FEC (v.0).\n");
		printk("INFO : It's firmware version is correct.\n");
		printk("INFO : This situation is OK, you have nothing to do.\n");
	}

	if ((lcl_fec_value == 0x1000) && (glb_fec_type == 2))
	{
		printk("INFO : Your FEC is a New Optical FEC (v.1).\n");
		printk("INFO : It's firmware version is correct.\n");
		printk("INFO : This situation is OK, you have nothing to do.\n");
	}

	if ((lcl_fec_value < 0x1000) && (glb_fec_type == 2))
	{
		printk("CRITICAL : Your FEC is a new Optical FEC (v.1).\n");
		printk("CRITICAL : It's firmware version should be at least 0x1000.\n");
		printk("CRITICAL : This could be a critical problem.\n");
		printk("CRITICAL : I will try to remap your FEC addresses according to your firmware version\n");		
		printk("CRITICAL : but think to upgrade your firmware asap !!!\n");
		glb_fec_type = 1;
	}


	if ((lcl_fec_value >= 0x1000) && (glb_fec_type == 1))
	{
		printk("INFO : Your FEC is an Old Optical FEC (v.0).\n");
		printk("INFO : But it's firmware version is more recent.\n");
		printk("INFO : I will try to remap your FEC addresses according to your firmware version\n");		
		printk("INFO : This situation should be OK, you have nothing to do.\n");
		glb_fec_type = 2;
	}

	/*********************************************/
	/*! De-assignation des tailles de remapping  */
	/* et Re-mapping definitif des BARs PLX      */
	/*********************************************/

	/*! unmap memory if needed */
	for (lcl_i = 0 ; lcl_i < (DD_NBR_OF_PCI_BASE_ADDRESSES) ; lcl_i++)
	{
		if (glb_plx_rmapsize_ba_array[lcl_i] > 0)
		{
			iounmap((DD_TYPE_PCI_BASE_ADDRESS *)glb_plx_remapped_ba_array[lcl_i]);
		}
	}
	printk("PLX addresses pre-mapping unmapped\n");


	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 0) glb_plx_rmapsize_ba_array[0] = DD_BA0_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 1) glb_plx_rmapsize_ba_array[1] = DD_BA1_SIZE_FOR_REMAP;
	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
	{
		if (DD_NBR_OF_PCI_BASE_ADDRESSES > 2) glb_plx_rmapsize_ba_array[2] = DD_BA2_SIZE_FOR_REMAP_EV0;
	}
	else if (DD_NBR_OF_PCI_BASE_ADDRESSES > 2) glb_plx_rmapsize_ba_array[2] = DD_BA2_SIZE_FOR_REMAP_OV1;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 3) glb_plx_rmapsize_ba_array[3] = DD_BA3_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 4) glb_plx_rmapsize_ba_array[4] = DD_BA4_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 5) glb_plx_rmapsize_ba_array[5] = DD_BA5_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 6) glb_plx_rmapsize_ba_array[6] = DD_BA6_SIZE_FOR_REMAP;
	if (DD_NBR_OF_PCI_BASE_ADDRESSES > 7)
	{
		printk("Too much base addresses (>7) defined for the PCI device.\n");
		return (DD_TYPE_INITMOD_RETURN)DD_TOO_MANY_PCI_BA;
	}

	/*! Remap PCI board base addresses for R/W/CONFIG operations */
	lcl_err = dd_ioc_set_plx9080_addresses();
	if (lcl_err != DD_RETURN_OK)
	{
		printk("Cannot remap FEC base addresses.\n");
		return (DD_TYPE_INITMOD_RETURN)lcl_err;
	}
	printk("FEC base addresses successfully remapped.\n");

	/*! HardReset of PLX */
	dd_internal_write_to_plx(DD_PLX9080_CTRL_OFFSET, (DD_PLX_CTRL_VALUE | DD_PLX_CTRL_RESET_BIT));

	/*! Initialisation of PLX bridge */
	dd_internal_write_to_plx(DD_PLX9080_CTRL_OFFSET, DD_PLX_CTRL_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_BIGEND_OFFSET, DD_PLX_BIGEND_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_MARBR_OFFSET, DD_PLX_MARBR_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_LAS0RR_OFFSET, DD_PLX_LAS0RR_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_LAS0BA_OFFSET, DD_PLX_LAS0BA_VALUE);
	dd_internal_write_to_plx(DD_PLX9080_LBRD0_OFFSET, DD_PLX_LBRD0_VALUE);

	/*! Invalidate PLX interrupts */

	#ifdef DD_ENABLE_IRQMANAGER
		dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_DISABLED_VALUE);
	#endif
	printk("PLX bridge resetted and configured.\n");


	/*! initialisation PCI control register */
	/*! autorise les acces I/O space et memory space */
	lcl_err = dd_ioc_set_pci_card_command_reg(&glb_device_ptr, &lcl_ioctl_data, DD_INTERNAL_COMMAND);
	if (lcl_err != DD_RETURN_OK)
	{
		printk("Error during PCI command register initialisation.\n");
		return (DD_TYPE_INITMOD_RETURN)lcl_err;
	}


	/* initialise the mono-user semaphore used to lock the ioctl() main entrypoint */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
		glb_ioctl_lock = MUTEX;
	#else
		init_MUTEX (&(glb_ioctl_lock));
	#endif


	/*! Installation of IRQ manager */
	#ifdef DD_ENABLE_IRQMANAGER
	        lcl_err = dd_install_interrupt_manager();
		if (lcl_err != DD_RETURN_OK)
		{
			printk("Error during interrupt manager installation\n");
			return (DD_TYPE_INITMOD_RETURN)lcl_err;
		} else printk("IRQ manager installed.\n");
		glb_irq_status = DD_FLAG_IS_DISABLED; //disabled via previous PLX settings
	#endif


	/* Now, reset wait queues  and clear timers*/
/*
	for (lcl_i = 0; lcl_i <= DD_MAX_TRANS_NUMBER; lcl_i++)
	{
		del_timer(&(glb_fec_trans_number_timers[lcl_i]));

	}
*/

	/*! Init Stacks status */
	for (lcl_i = 0 ; lcl_i <= DD_MAX_TRANS_NUMBER ; lcl_i++)
	{
		glb_read[lcl_i] = DD_READ_DATA_UNSUCCESSFUL;
		glb_copied[lcl_i] = DD_COPY_DATA_UNSUCCESSFUL;
		glb_fec_write_trans_in_use[lcl_i] = DD_TRANS_NUMBER_IS_FREE;
	}

	/*! Init counter status */
	glb_currentTrans = DD_MIN_TRANS_NUMBER;

	/*! Init global wait queue status */
	glb_fec_current_read_process_waiting = 0;

	/*! init errors counter */
	glb_errors_in_irq = 0;

	/*! clear flag for freeing transaction number manager */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_TRANS_NUMBER_MNGR_IN_USE_FLAG, &glb_atomic_flags);
	#endif

	

	/*! clear flag of hardware access */
	//clear_bit(DD_FIFOTRA_IN_USE_FLAG, &glb_atomic_flags);

	/*! init system state vs. reset statement */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_SOFT_RESET_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_SOFT_RESET_FLAG, &glb_atomic_flags);
	#endif


	/*! clear flag of read increment method */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		clear_bit(DD_READ_REQUEST_FLAG, (void *)&glb_atomic_flags);
	#else
		clear_bit(DD_READ_REQUEST_FLAG, &glb_atomic_flags);
	#endif

	


	/* set enable/disable flag for ring errors counting */
	glb_enable_count_of_errors_on_ring = DD_FLAG_IS_ENABLED;

	/* Initialize plx reset counter at 0 for load-time */
	glb_plx_reset_counter = 0;

	/* Initialize fec reset counter at 0 for load-time */
	glb_fec_reset_counter = 0;





	/* Set FIFOs offsets according to FEC type */
	if ( (glb_fec_type == 0) || (glb_fec_type == 1) )
	{
		dd_fec_fifotra_offset = DD_FEC_FIFOTRA_OFFSET_EV0;
		dd_fec_fiforec_offset = DD_FEC_FIFOREC_OFFSET_EV0;
		dd_fec_fiforet_offset = DD_FEC_FIFORET_OFFSET_EV0;
	}
	else
	{
		dd_fec_fifotra_offset = DD_FEC_FIFOTRA_OFFSET_OV1;
		dd_fec_fiforec_offset = DD_FEC_FIFOREC_OFFSET_OV1;
		dd_fec_fiforet_offset = DD_FEC_FIFORET_OFFSET_OV1;
	}

	/*! DEBUG INFO */
	printk("Driver Internal variables initialised.\n");


	/*! enable fec (CTRL0:0=1) */
	dd_internal_write_to_fec_register(DD_FEC_CTRL0_OFFSET, DD_FEC_ENABLE_FEC);



	/*! Validate PLX interrupts */
	glb_irq_status = DD_FLAG_IS_DISABLED;
	#ifdef DD_ENABLE_IRQMANAGER
		dd_internal_write_to_plx(DD_PLX9080_IRQREG_OFFSET, DD_PLX_IRQ_ENABLED_VALUE);
		printk("PLX IRQ's are now enabled.\n");
		glb_irq_status = DD_FLAG_IS_ENABLED;
	#endif

	/* fill the information fields for the driver */
	strcpy(glb_dd_version,FEC_DRIVER_VERSION);


	/*! DEBUG INFO */
	printk("Driver successfully loaded and initialised.\n");

return (DD_TYPE_INITMOD_RETURN)DD_RETURN_OK;
}






/*!
<b>FUNCTION	: cleanup_module</b>
- Job
	- Cleanup Kernel of module properties when unloaded
- Inputs
	- Void
- Outputs
	- Void
- Internals
	- global var glb_module_major is used
	- global var struct_info is wrapped to a pointer struct_info*
		- <b>Fields used :</b>
		- ->fec_offset_and_value
		- ->plx_ba_infos
- Notes
	-Void
- Error Management - Values returned
	- Void
- Sub-functions calls
	- dd_stack_msg
	- iounmap				(system)
	- dd_write_to_plx
	- dd_remove_interrupt_manager
	- unregister_chrdev		(system)
*/



module_init(fastfec_init);
module_exit(fastfec_exit);




