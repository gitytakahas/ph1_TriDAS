/*
   FileName : 		DEBUGGER.C

   Content : 		Device Driver Debugger / Low level accesses

   Used in : 		Debugger only

   Programmer : 	Laurent GROSS

   Version : 		Unified-6.0

   Date of last modification : 17/05/2005

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
standard includes
*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <unistd.h>


/*
Driver-specific includes
*/
#include "glue.h"
#include "debugger.h"
#include "driver_conf.h"

/*
some defines...
*/
/* min-max CCUs addresses, for old and new CCUS */
#define MIN_CCU		0x0001
#define MAX_CCU		0x007f

/* min-max I2C channels, for old CCUS */
#define MIN_I2C_CH	0x0001
#define MAX_I2C_CH	0x0006

/* min-max I2C channels, for new CCUS */
#define MIN_I2C_CH25	0x0010
#define MAX_I2C_CH25	0x001F

/* min-max I2C addresses, for old and new CCUS */
#define MIN_I2C_ADR	0x0000
#define MAX_I2C_ADR	0x007F


/*!
Internal functions ; names are explicit.
*/
void dbg_decode_control0(DD_FEC_REGISTER_DATA param_control0)
{
	if (DD_FEC_ENABLE_FEC & param_control0) fprintf(stdout,"\t FEC enabled\n");
	if (DD_FEC_SEND & param_control0) fprintf(stdout,"\t Send data to ring\n");
//	if (DD_FEC_RESEND & param_control0) fprintf(stdout,"\t FEC resend option enabled\n");
	if (DD_FEC_SEL_SER_OUT & param_control0) fprintf(stdout,"\t FEC select serial out selected\n");
	if (DD_FEC_SEL_SER_IN & param_control0) fprintf(stdout,"\t FEC select serial in selected\n");
//	if (DD_FEC_SEL_IRQ_MODE & param_control0) fprintf(stdout,"\t FEC select IRQ mode selected\n");
	if (DD_FEC_RESET_TTCRX & param_control0) fprintf(stdout,"\t FEC TTCRX reset flag setted\n");
	if (DD_FEC_RESET_OUT & param_control0) fprintf(stdout,"\t FEC soft reset flag setted\n");
	if (DD_FEC_CTRL0_INIT_TTCRX & param_control0) fprintf(stdout,"\t FEC init TTCRX flag setted\n");
}




/*!
Internal functions ; names are explicit.
*/
void dbg_decode_control1(DD_FEC_REGISTER_DATA param_control1)
{
	if (DD_FEC_CLEAR_IRQ & param_control1) fprintf(stdout,"\t Clear IRQ's\n");
	if (DD_FEC_CLEAR_ERRORS & param_control1) fprintf(stdout,"\t Clear errors\n");
}




/*!
Internal functions ; names are explicit.
*/
void dbg_decode_status0(DD_FEC_REGISTER_DATA param_status0)
{
	if (DD_FEC_TRA_RUN & param_status0) fprintf(stdout,"\t fifo transmit running\n");
	if (DD_FEC_REC_RUN & param_status0) fprintf(stdout,"\t fifo receive running\n");
//	if (DD_FEC_REC_HALF_F & param_status0) fprintf(stdout,"\t fifo receive half full\n");
	if (DD_FEC_REC_FULL & param_status0) fprintf(stdout,"\t fifo receive full\n");
	if (DD_FEC_REC_EMPTY & param_status0) fprintf(stdout,"\t fifo receive empty\n");
//	if (DD_FEC_RET_HALF_F & param_status0) fprintf(stdout,"\t fifo return half full\n");
	if (DD_FEC_RET_FULL & param_status0) fprintf(stdout,"\t fifo return full\n");
	if (DD_FEC_RET_EMPTY & param_status0) fprintf(stdout,"\t fifo return empty\n");
//	if (DD_FEC_TRA_HALF_F & param_status0) fprintf(stdout,"\t fifo transmit half full\n");
	if (DD_FEC_TRA_FULL & param_status0) fprintf(stdout,"\t fifo transmit full\n");
	if (DD_FEC_TRA_EMPTY & param_status0) fprintf(stdout,"\t fifo transmit empty\n");
	if (DD_FEC_LINK_INITIALIZED & param_status0) fprintf(stdout,"\t Link initialized\n");
	if (DD_FEC_PENDING_IRQ & param_status0) fprintf(stdout,"\t pending IRQ\n");
	if (DD_FEC_DATA_TO_FEC & param_status0) fprintf(stdout,"\t data to fec\n");
}




/*!
Internal functions ; names are explicit.
*/
void dbg_decode_status1(DD_FEC_REGISTER_DATA param_status1)
{
	if (DD_FEC_ILL_DATA & param_status1) fprintf(stdout,"\t Illegal data received\n");
	if (DD_FEC_ILL_SEQ & param_status1) fprintf(stdout,"\t Illegal sequence received\n");
	if (DD_FEC_CRC_ERROR & param_status1) fprintf(stdout,"\t CRC error\n");
	if (DD_FEC_DATA_COPIED & param_status1) fprintf(stdout,"\t Data copied\n");
	if (DD_FEC_ADDR_SEEN & param_status1) fprintf(stdout,"\t address seen\n");
	if (DD_FEC_ERROR & param_status1) fprintf(stdout,"\t Error\n");
}




/*!
Main Function
*/

int main(void)
{
/* file descriptor for the device driver */
int fd;

/* control of the main loop */
int lcl_choix = 999;

/* generic error container */
DD_TYPE_ERROR lcl_err;

/* bunch of containers, possibly covering all the interesting hardware registers */
DD_FEC_REGISTER_DATA lcl_control0;
DD_FEC_REGISTER_DATA lcl_control1;
DD_FEC_REGISTER_DATA lcl_status0;
DD_FEC_REGISTER_DATA lcl_status1;
DD_FEC_FIFO_DATA_16 lcl_fifotra_16;
DD_FEC_FIFO_DATA_32 lcl_fifotra_32;
DD_FEC_FIFO_DATA_16 lcl_fiforet_16;
DD_FEC_FIFO_DATA_32 lcl_fiforet_32;
DD_FEC_FIFO_DATA_16 lcl_fiforec_16;
DD_FEC_FIFO_DATA_32 lcl_fiforec_32;
DD_PLX_DATA lcl_plx_data;
DD_TYPE_PCI_COMMAND_REG lcl_pci_com_reg;
DD_TYPE_PCI_BASE_ADDRESS lcl_pci_addr;

/* on error, get a text message associated to the error code */
char lcl_error_msg[DD_MAX_DECODED_ERROR_MSG_LENGTH];

/* Init CCU frame, used for old ccu detection */
DD_FEC_STD_WORD init_ccu_stack[DD_USER_MAX_MSG_LENGTH] = {0xFFFF, 0x0000, DD_CCU_INIT_FRAME_LENGTH, DD_CCU_NODE_CONTROLLER_CHANNEL, DD_FAKE_TRANSACTION_NUMBER, DD_CCU_COMMAND_WRITE_CTRL_A, (DD_CCU_DATA_ENABLE_ALL | DD_FRAME_EOF_BITMASK)};

/* init ccu25 CTRL A frame, used for CCU25 detection */
DD_FEC_STD_WORD i2c01_talk_ccu25[DD_USER_MAX_MSG_LENGTH] = {0xFFFF, 0x0000, DD_CCU25_WRITE_CRA_FRAME_LENGTH, DD_CCU_NODE_CONTROLLER_CHANNEL, DD_FAKE_TRANSACTION_NUMBER, DD_CCU25_COMMAND_WRITE_CTRL_A, (DD_CCU25_DATA_WCRA_ON_INIT | DD_FRAME_EOF_BITMASK)};

/* Frame used to READ BACK ccu25 CTRL A */
DD_FEC_STD_WORD i2c02_talk_ccu25[DD_USER_MAX_MSG_LENGTH] = {0xFFFF, 0x0000, DD_CCU25_READ_CRA_FRAME_LENGTH, DD_CCU_NODE_CONTROLLER_CHANNEL, DD_FAKE_TRANSACTION_NUMBER, (DD_CCU25_COMMAND_READ_CTRL_A | DD_FRAME_EOF_BITMASK)};

/* Frame used to initialize ccu25 CTRL E */
DD_FEC_STD_WORD i2c03_talk_ccu25[DD_USER_MAX_MSG_LENGTH] = {0xFFFF, 0x0000, 0x0006, 0x0000, 0x0000, 0x0004, 0x0000, 0x00FF, 0x80FF};

/* Frame used to READ BACK ccu25 CTRL E */
DD_FEC_STD_WORD i2c04_talk_ccu25[DD_USER_MAX_MSG_LENGTH] = {0xFFFF, 0x0000, DD_CCU25_READ_CRE_FRAME_LENGTH, DD_CCU_NODE_CONTROLLER_CHANNEL, DD_FAKE_TRANSACTION_NUMBER, (DD_CCU25_COMMAND_READ_CTRL_E | DD_FRAME_EOF_BITMASK)};

/* Frame used to Write I2C CRA on CCU25 (iniot I2C channels)*/
DD_FEC_STD_WORD i2c05_talk_ccu25[DD_USER_MAX_MSG_LENGTH] = {0xFFFF, 0x0000, 0x0004, 0x0010, DD_FAKE_TRANSACTION_NUMBER, 0x00F0, (0x0040 | DD_FRAME_EOF_BITMASK)};

/* Frame used to Read back the CCU25 I2C CRA */
DD_FEC_STD_WORD i2c06_talk_ccu25[DD_USER_MAX_MSG_LENGTH] = {0xFFFF, 0x0000, 0x0003, 0x0010, DD_FAKE_TRANSACTION_NUMBER, (0x00F1 | DD_FRAME_EOF_BITMASK)};

/* Just in case we need to forge a frame on the fly, container will be ready */
DD_FEC_STD_WORD onfly_stack[DD_USER_MAX_MSG_LENGTH];

/* generic container used to read back one word from fec (Ctrl, Status, etc...) */
DD_FEC_STD_WORD lcl_fecdata;

/* incremental counters used in full scanring function */
DD_FEC_STD_WORD ccu_counter;
DD_FEC_STD_WORD channel_counter;
DD_FEC_STD_WORD i2c_counter;

/* timers used to estimate context switch delays */
struct timeval time1, time2;
struct timezone zone1, zone2;

/* typical write_then_read_answer stacks */
DD_FEC_STD_WORD write_stack[DD_USER_MAX_MSG_LENGTH];
DD_FEC_STD_WORD read_stack[DD_USER_MAX_MSG_LENGTH];



/* used for frames-issued-from-file feeding the ring */
FILE* filein;
char frames_filename[128];
char frame_asc_value[128];
int file_loops, loops_counter=0;
DD_FEC_STD_WORD frame_hex_value;

/* error management array */
DD_TYPE_FEC_DATA32 lcl_ring_errors_manager[DD_NB_OF_ERRORS_TO_SPY];


/* crap */
int lcl_counter;
int lcl_input;
DD_TYPE_FEC_DATA32 lcl_u32_value;
DD_TYPE_FEC_DATA16 lcl_u16_value;
int lcl_i, lcl_j;
unsigned int lcl_scanme;


char lcl_driver_version[DD_VERSION_STRING_SIZE];


DD_FEC_ARRAY lcl_fec_array;

int lcl_fifo_depth=0;
int lcl_fec_type;
unsigned short lcl_dev_id;
unsigned short lcl_vend_id;

DD_FEC_REGISTER_DATA lcl_firmware_version;


	/***********************************************************************
	Ouverture du driver
	***********************************************************************/
	lcl_err=glue_fec_open_device(&fd, DD_DEVICE, (O_RDWR | O_NONBLOCK));
	if ( lcl_err != DD_RETURN_OK)
	{
		glue_fecdriver_get_error_message(lcl_err,lcl_error_msg);
		fprintf(stderr,lcl_error_msg);
	}
	

	//Get Driver version tag
	lcl_err = glue_fecdriver_get_driver_version(fd, lcl_driver_version);
	if ( lcl_err != DD_RETURN_OK)
	{
		glue_fecdriver_get_error_message(lcl_err,lcl_error_msg);
		fprintf(stderr,lcl_error_msg);
	}
	else printf("Driver version tag is : %s\n", lcl_driver_version);


	//get fec vendor_id
	lcl_err = glue_fec_get_vendor_id(fd, &lcl_vend_id);
	if ( lcl_err != DD_RETURN_OK)
	{
		glue_fecdriver_get_error_message(lcl_err,lcl_error_msg);
		fprintf(stderr,lcl_error_msg);
	}
	else printf("FEC VENDOR_ID is : 0x%x\n", lcl_vend_id);
			
	//get fec device_id
	lcl_err = glue_fec_get_device_id(fd, &lcl_dev_id);
	if ( lcl_err != DD_RETURN_OK)
	{
		glue_fecdriver_get_error_message(lcl_err,lcl_error_msg);
		fprintf(stderr,lcl_error_msg);
	}
	else printf("FEC DEVICE_ID is : 0x%x\n", lcl_dev_id);



	//Get FEC type
	lcl_err = glue_fec_get_fec_type(fd, &lcl_fec_type);
	if ( lcl_err != DD_RETURN_OK)
	{
		glue_fecdriver_get_error_message(lcl_err,lcl_error_msg);
		fprintf(stderr,lcl_error_msg);
	}
	else printf("Your FEC is defined, internally to the driver, as a type %d FEC\n", lcl_fec_type);


	if (lcl_fec_type != 0)
	{
		//get fec firmware version
		lcl_err = glue_fec_get_firmware_version(fd, &lcl_firmware_version);
		if ( lcl_err != DD_RETURN_OK)
		{
			glue_fecdriver_get_error_message(lcl_err,lcl_error_msg);
			fprintf(stderr,lcl_error_msg);
		}
		else printf("FEC firmware version is : 0x%x\n", lcl_firmware_version);
	}
	else printf("No firmware version available, your FEC is too old for this.\n");



	//Get Driver fifo's depth
	lcl_err = glue_fec_get_fifo_item_size(fd, &lcl_fifo_depth);
	if ( lcl_err != DD_RETURN_OK)
	{
		glue_fecdriver_get_error_message(lcl_err,lcl_error_msg);
		fprintf(stderr,lcl_error_msg);
	}
	else printf("Driver will run with %d bits fifo\n", lcl_fifo_depth);


	/***********************************************************************
	Main Menu
	***********************************************************************/
	while (lcl_choix != DBG_QUIT)
	{
		fprintf(stdout,"\nMENU\n");
		fprintf(stdout,"%d : Quit\n",DBG_QUIT);
		fprintf(stdout,"%d : Write to control 0\n",DBG_WCR0);
		fprintf(stdout,"%d : Read control 0\n",DBG_RCR0);
		fprintf(stdout,"%d : Write control 1\n",DBG_WCR1);
		fprintf(stdout,"%d : Read status 0\n",DBG_RSR0);
		fprintf(stdout,"%d : Read status 1\n",DBG_RSR1);
		fprintf(stdout,"%d : Write to fifotransmit\n",DBG_WFIFOTRA);
		fprintf(stdout,"%d : Read from fifotransmit\n",DBG_RFIFOTRA);
		fprintf(stdout,"%d : Write to fiforeturn\n",DBG_WFIFORET);
		fprintf(stdout,"%d : Read from fiforeturn\n",DBG_RFIFORET);
		fprintf(stdout,"%d : Write to fiforeceive\n",DBG_WFIFOREC);
		fprintf(stdout,"%d : Read from fiforeceive\n",DBG_RFIFOREC);
		fprintf(stdout,"%d : Dump PLX config\n",DBG_DUMPPLX);
		fprintf(stdout,"%d : Dump PCI config\n",DBG_DUMPPCI);
		fprintf(stdout,"%d : Reset users counter to 1\n",DBG_RESETTOONE);
		fprintf(stdout,"%d : Enable (or Re-enable) PLX interrupts\n",DBG_ENABLEIRQ);
		fprintf(stdout,"%d : Disable PLX interrupts\n",DBG_DISABLEIRQ);
		fprintf(stdout,"%d : init TTCRX\n",DBG_INITTTCRX);
		fprintf(stdout,"%d : Resets PLX\n",DBG_RESETPLX);
		fprintf(stdout,"%d : Resets FEC\n",DBG_RESETFEC);
		fprintf(stdout,"%d : OLD CCU ONLY : Full Scan Ring - WRITE to devices\n",DBG_OCCU_SR_WRITE);
		fprintf(stdout,"%d : OLD CCU ONLY : Full Scan Ring - READ from devices\n",DBG_OCCU_SR_READ);
		fprintf(stdout,"%d : OLD CCU ONLY : Context switch delay measurement\n",DBG_OCCU_CSW);
		fprintf(stdout,"%d : NEW CCU (i.e. CCU 25) ONLY : Full Scan Ring - WRITE to devices\n",DBG_NCCU_SR_WRITE);
		fprintf(stdout,"%d : NEW CCU (i.e. CCU 25) ONLY : Full Scan Ring - READ from devices\n",DBG_NCCU_SR_READ);
		fprintf(stdout,"%d : NEW CCU (i.e. CCU 25) ONLY : Context switch delay measurement\n",DBG_NCCU_CSW);
		fprintf(stdout,"%d : Dump PLX raw base addresses\n",DBG_DUMP_BA);
		fprintf(stdout,"%d : Dump PLX remapped base addresses\n",DBG_DUMP_RBA);
		fprintf(stdout,"%d : Enter a frame and send it to the ring ; RAW write method used\n",DBG_WF_RAW);
		fprintf(stdout,"%d : Enter a frame and send it to the ring ; glue_fec_write_frame write method used\n",DBG_WF_GLUE);
		fprintf(stdout,"%d : Read back a forced-ack or data frame (transaction number requested)\n",DBG_RBF);
		fprintf(stdout,"%d : OLD CCU ONLY : Quick scan ; detect only CCU's on ring\n",DBG_OCCU_QS);
		fprintf(stdout,"%d : NEW CCU ONLY : Quick scan ; detect only CCU's on ring\n",DBG_NCCU_QS);
		fprintf(stdout,"%d : Write frame and read back direct ack and forced ack / data\n",DBG_WF_RBF);
		fprintf(stdout,"%d : Transcript error code into clear message\n",DBG_DECODE_ERR);
		fprintf(stdout,"%d : Loop a write/readback sequence of command issued from a file.\n",DBG_LOOP_FILE_WF_RBF);
		fprintf(stdout,"%d : Ring Error Management sub-menu.\n",DBG_REM);
		fprintf(stdout,"%d : Read current PLX reset counter.\n",DBG_RPLXCOUNTER);
		fprintf(stdout,"%d : Read current FEC reset counter.\n",DBG_RFECCOUNTER);
		fprintf(stdout,"%d : Read Driver General Status Word.\n",DBG_GETSTATUS);
		fprintf(stdout,"%d : Put the debugger in -wait for warning from device- mode.\n",DBG_WAITWARNING);
		fprintf(stdout,"%d : Simulate a warning emitted by a device.\n",DBG_EMULATEWARNING);
//		fprintf(stdout,"%d : Readout date of Firmware release.\n",DBG_READ_FIRMWARE_DATE);
		fprintf(stdout,"%d : Readout Firmware version number.\n",DBG_READ_FIRMWARE_VERSION);
		fprintf(stdout,"Your choice ? : ");
		scanf("%d",&lcl_choix);

		switch(lcl_choix)
		{


			case DBG_QUIT:
			break;




			/*******************************************************
			Write control 0
			*******************************************************/
			case DBG_WCR0:
				fprintf(stdout,"Value to write to CONTROL 0 (HEXA) : ");
				scanf("%x", &lcl_scanme);
				lcl_control0 = (DD_FEC_REGISTER_DATA)lcl_scanme;
				lcl_err = glue_fec_set_ctrl0(fd, lcl_control0);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"Value wrote in FEC CTRL0 : 0x%x\n",lcl_control0);
			break;




			/*******************************************************
			Read control 0
			*******************************************************/
			case DBG_RCR0:
				lcl_err = glue_fec_get_ctrl0(fd, &lcl_control0);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					fprintf(stdout,"CONTROL 0 current value : 0x%x\n",lcl_control0);
					dbg_decode_control0(lcl_control0);
				}
			break;


			/*******************************************************
			Write control 1
			*******************************************************/
			case DBG_WCR1:
				fprintf(stdout,"Value to write to CONTROL 1 (HEXA) : ");
				scanf("%x", &lcl_scanme);
				lcl_control1 = (DD_FEC_REGISTER_DATA)lcl_scanme;
				lcl_err = glue_fec_set_ctrl1(fd, lcl_control1);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"Value wrote in FEC CTRL1 : 0x%x\n",lcl_control1);
			break;



			/*******************************************************
			Read status 0
			*******************************************************/
			case DBG_RSR0:
				lcl_err = glue_fec_get_status0(fd, &lcl_status0);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					fprintf(stdout,"STATUS 0 value : 0x%x\n",lcl_status0);
					dbg_decode_status0(lcl_status0);
				}
			break;



			/*******************************************************
			Read status  1
			*******************************************************/
			case DBG_RSR1:
				lcl_err = glue_fec_get_status1(fd, &lcl_status1);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					fprintf(stdout,"STATUS 1 value : 0x%x\n",lcl_status1);
					dbg_decode_status1(lcl_status1);
				}
			break;



			/*******************************************************
			Write to fifotransmit
			*******************************************************/
			case DBG_WFIFOTRA:
				fprintf(stdout,"Value to write to FIFO TRANSMIT (HEXA) : ");
				scanf("%x", &lcl_scanme);
				if (lcl_fifo_depth == 16)
				{
					lcl_fifotra_16 = (DD_FEC_FIFO_DATA_16)lcl_scanme;
					lcl_err = glue_fec_set_native_fifotra_item_16(fd, lcl_fifotra_16);
				}
				else
				{
					lcl_fifotra_32 = (DD_FEC_FIFO_DATA_32)lcl_scanme;
					lcl_err = glue_fec_set_native_fifotra_item_32(fd, lcl_fifotra_32);
				}

				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					if (lcl_fifo_depth == 16)
					{
						fprintf(stdout,"Value wrote in FEC Fifo Transmit : 0x%x\n",lcl_fifotra_16);
					}
					else fprintf(stdout,"Value wrote in FEC Fifo Transmit : 0x%x\n",lcl_fifotra_32);
				}

				
				
				
				
				
				 
			break;


			/*******************************************************
			Read from fifotransmit
			*******************************************************/
			case DBG_RFIFOTRA:
				if (lcl_fifo_depth == 16)
				{
					lcl_err = glue_fec_get_native_fifotra_item_16(fd, &lcl_fifotra_16);
				}
				else lcl_err = glue_fec_get_native_fifotra_item_32(fd, &lcl_fifotra_32);
				
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					if (lcl_fifo_depth == 16)
					{
						fprintf(stdout,"fifo transmit value : 0x%x\n",lcl_fifotra_16);
					}
					else fprintf(stdout,"fifo transmit value : 0x%x\n",lcl_fifotra_32);
				}
			break;


			/*******************************************************
			Write to fiforeturn
			*******************************************************/
			case DBG_WFIFORET:
				fprintf(stdout,"Value to write to FIFO RETURN (HEXA) : ");
				scanf("%x", &lcl_scanme);
				if (lcl_fifo_depth == 16)
				{
					lcl_fiforet_16 = (DD_FEC_FIFO_DATA_16)lcl_scanme;
					lcl_err = glue_fec_set_native_fiforet_item_16(fd, lcl_fiforet_16);
				}
				else
				{
					lcl_fiforet_32 = (DD_FEC_FIFO_DATA_32)lcl_scanme;
					lcl_err = glue_fec_set_native_fiforet_item_32(fd, lcl_fiforet_32);
				}
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					if (lcl_fifo_depth == 16)
					{
						fprintf(stdout,"Value wrote in FEC Fifo Return : 0x%x\n",lcl_fiforet_16);
					}
					else fprintf(stdout,"Value wrote in FEC Fifo Return : 0x%x\n",lcl_fiforet_32);
				}
			break;



			/*******************************************************
			Read from fiforeturn
			*******************************************************/
			case DBG_RFIFORET:
				if (lcl_fifo_depth == 16)
				{
					lcl_err = glue_fec_get_native_fiforet_item_16(fd, &lcl_fiforet_16);
				}
				else lcl_err = glue_fec_get_native_fiforet_item_32(fd, &lcl_fiforet_32);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					if (lcl_fifo_depth == 16)
					{
						fprintf(stdout,"fifo return value : 0x%x\n",lcl_fiforet_16);
					}
					else fprintf(stdout,"fifo return value : 0x%x\n",lcl_fiforet_32);
				}
			break;



			/*******************************************************
			Write to fiforeceive
			*******************************************************/
			case DBG_WFIFOREC:
				fprintf(stdout,"Value to write to FIFO RECEIVE (HEXA) : ");
				scanf("%x", &lcl_scanme);
				if (lcl_fifo_depth == 16)
				{
					lcl_fiforec_16 = (DD_FEC_FIFO_DATA_16)lcl_scanme;
					lcl_err = glue_fec_set_native_fiforec_item_16(fd, lcl_fiforec_16);
				}
				else
				{
					lcl_fiforec_32 = (DD_FEC_FIFO_DATA_32)lcl_scanme;
					lcl_err = glue_fec_set_native_fiforec_item_32(fd, lcl_fiforec_32);
				}

				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					if (lcl_fifo_depth == 16)
					{
						fprintf(stdout,"Value wrote in FEC Fifo receive : 0x%x\n",lcl_fiforec_16);
					}
					else fprintf(stdout,"Value wrote in FEC Fifo receive : 0x%x\n",lcl_fiforec_32);
				}
			break;



			/*******************************************************
			Read from fiforeceive
			*******************************************************/
			case DBG_RFIFOREC:
				if (lcl_fifo_depth == 16)
				{
					printf("Calling 16\n");
					lcl_err = glue_fec_get_native_fiforec_item_16(fd, &lcl_fiforec_16);
				}
				else
				{
					printf("Calling 32\n");
					lcl_err = glue_fec_get_native_fiforec_item_32(fd, &lcl_fiforec_32);
				}

				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					if (lcl_fifo_depth == 16)
					{
						fprintf(stdout,"fifo return value : 0x%x\n",lcl_fiforec_16);
					}
					else fprintf(stdout,"fifo return value : 0x%x\n",lcl_fiforec_32);
				}
			break;


			/*******************************************************
			Dump PLX config
			*******************************************************/
			case DBG_DUMPPLX:
				lcl_err = glue_plx_get_value(fd, DD_PLX9080_LAS0RR_OFFSET, &lcl_plx_data);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"value of PLX LAS0RR : 0x%x\n",lcl_plx_data);


				lcl_err = glue_plx_get_value(fd, DD_PLX9080_LAS0BA_OFFSET, &lcl_plx_data);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"value of PLX LAS0BA : 0x%x\n", lcl_plx_data);


				lcl_err = glue_plx_get_value(fd, DD_PLX9080_MARBR_OFFSET, &lcl_plx_data);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"value of PLX MARBR : 0x%x\n", lcl_plx_data);


				lcl_err = glue_plx_get_value(fd, DD_PLX9080_BIGEND_OFFSET, &lcl_plx_data);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"value of PLX BIGEND : 0x%x\n", lcl_plx_data);



				lcl_err = glue_plx_get_value(fd, DD_PLX9080_LBRD0_OFFSET, &lcl_plx_data);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"value of PLX LBRD0 : 0x%x\n", lcl_plx_data);



				lcl_err = glue_plx_get_value(fd, DD_PLX9080_CTRL_OFFSET, &lcl_plx_data);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"value of PLX CTRL : 0x%x\n", lcl_plx_data);



				lcl_err = glue_plx_get_value(fd, DD_PLX9080_IRQREG_OFFSET, &lcl_plx_data);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"value of PLX IRQREG : 0x%x\n", lcl_plx_data);
			break;


			/*******************************************************
			Dump PCI config
			*******************************************************/
			case DBG_DUMPPCI:

				lcl_err = glue_pci_get_command_reg(fd, &lcl_pci_com_reg);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"value of PCI command reg : 0x%x\n",lcl_pci_com_reg);
			break;


			/*******************************************************
			Reset users counter to 1
			*******************************************************/
			case DBG_RESETTOONE:
				lcl_err = glue_fecdriver_force_count_to_one(fd);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"Module count is now 1\n");
			break;



			/*******************************************************
			Enable PLX interrupts
			*******************************************************/
			case DBG_ENABLEIRQ:
				lcl_err = glue_plx_enable_irqs(fd);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"PLX interrupts enabled\n");
			break;


			/*******************************************************
			Disable PLX interrupts
			*******************************************************/
			case DBG_DISABLEIRQ:
				lcl_err = glue_plx_disable_irqs(fd);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"PLX interrupts disabled\n");
			break;


			/*******************************************************
			Init TTCRX
			*******************************************************/
			case DBG_INITTTCRX:
				lcl_err = glue_fec_init_ttcrx(fd);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"TTCRX Initialised\n");
			break;


			/*******************************************************
			Reset PLX
			*******************************************************/
			case DBG_RESETPLX:
				lcl_err = glue_plx_hard_reset(fd);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"PLX Resetted\n");
			break;






			/*******************************************************
			Reset FEC
			*******************************************************/
			case DBG_RESETFEC:
				lcl_err = glue_fec_soft_reset(fd);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"FEC Soft-Resetted\n");
			break;


			/*******************************************************
			COMPLETE SCANRING FOR OLD CCU's  - probe devices via WRITE method
			*******************************************************/
			case DBG_OCCU_SR_WRITE:
				/* print header */
				fprintf(stdout,"Scanning ring for OLD CCU's   - Devices are probed via WRITE method\n");
				fprintf(stdout,"\tscanned CCU's range is [0x%x .. 0x%x]\n", MIN_CCU, MAX_CCU);
				fprintf(stdout,"\tscanned i2c channels range is [0x%x .. 0x%x]\n", MIN_I2C_CH, MAX_I2C_CH);
				fprintf(stdout,"\tscanned i2c addresses (per channel) range is [0x%x .. 0x%x]\n", MIN_I2C_ADR, MAX_I2C_ADR);

				/* do we want to exclude one I2C channel ? */
				fprintf(stdout,"Warning : This scan function writes 0x0044 at ALL addresses of ALL I2C channels of ALL CCUs.\n");
				fprintf(stdout,"Is there one I2C channel you want to exclude from the scan? (The one who manages the PIA reset, for example...)\n");
				fprintf(stdout,"Exclude I2C channel number (enter FF for no exclusion) : (HEXA) ? : ");
				scanf("%x", &lcl_scanme);
				lcl_fecdata = (DD_FEC_STD_WORD)lcl_scanme;

				/* now, scan the ring */
				for (ccu_counter = MIN_CCU; ccu_counter <= MAX_CCU ; ccu_counter++)
				{
					//fprintf(stdout,"Probing ccu 0x%x \r", ccu_counter);
					write_stack[0] = ccu_counter;
					for (lcl_j=1 ; lcl_j<(sizeof(init_ccu_stack)/sizeof(__u16)); lcl_j++)
					{
						write_stack[lcl_j] = init_ccu_stack[lcl_j];
					}

					lcl_err = glue_fec_write_frame(fd, write_stack);


					if (lcl_err == DD_RETURN_OK)
					{
						fprintf(stdout,"\nCCU number 0x%x detected.\n",ccu_counter);
						for (channel_counter = MIN_I2C_CH; channel_counter <= MAX_I2C_CH ; channel_counter++)
						{
							if (channel_counter != lcl_fecdata)
							{
								write_stack[0] = ccu_counter;
								write_stack[1] = 0x0000;
								write_stack[2] = DD_CHANNEL_INIT_FRAME_LENGTH;
								write_stack[3] = channel_counter;
								write_stack[4] = DD_FAKE_TRANSACTION_NUMBER;
								write_stack[5] = DD_CHANNEL_COMMAND_WRITE_CTRL_A;
								write_stack[6] = (DD_CHANNEL_DATA_ENABLE_I2C_AND_ACK | DD_FRAME_EOF_BITMASK);
								lcl_err = glue_fec_write_frame(fd, write_stack);
								if (lcl_err != DD_RETURN_OK)
								{
									glue_fecdriver_print_error_message(lcl_err);
								}

								//fprintf(stdout,"\tNow probing for i2c devices attached to channel 0x%x of CCU 0x%x ; be patient ...\n", channel_counter, ccu_counter);
								for (i2c_counter = MIN_I2C_ADR; i2c_counter <= MAX_I2C_ADR ; i2c_counter++)
								{
									//fprintf(stdout,"\t\tprobing i2c address 0x%x on channel 0x%x of CCU 0x%x ; be patient ...\r", i2c_counter, channel_counter, ccu_counter);

									//Build stack for Normal Mode testing
									write_stack[0] = ccu_counter;
									write_stack[1] = 0x0000;
									write_stack[2] = DD_CCU_WRITE_NORMAL_MODE_FRAME_LENGTH;
									write_stack[3] = channel_counter;
									write_stack[4] = DD_FAKE_TNUM_PROVOKE_ACK;
									write_stack[5] = DD_SINGLE_BYTE_WRITE_NORMAL_MODE;
									write_stack[6] = i2c_counter;
									write_stack[7] = (0x0044 | DD_FRAME_EOF_BITMASK);
									lcl_err = glue_fec_write_frame(fd, write_stack);
									if (lcl_err == DD_RETURN_OK)
									{
										if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
										{
											read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
										}
										else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

										lcl_err = glue_fec_read_frame(fd, read_stack);
										if (lcl_err == DD_RETURN_OK)
										{
											if (read_stack[read_stack[2]+2] == 0x8004)
											{
												fprintf(stdout,"\t\tI2C device detected at address 0x%x on channel 0x%x of CCU 0x%x          \n",i2c_counter, channel_counter, ccu_counter);
											}
										}
									}
								}
							}

						}

					}
				}
				fprintf(stdout,"END of Scan.\n");
    			break;


			
			/*******************************************************
			COMPLETE SCANRING FOR OLD CCU's  - probe devices via READ method
			*******************************************************/
			case DBG_OCCU_SR_READ:
				/* print header */
				fprintf(stdout,"Scanning ring for OLD CCU's   - Devices are probed via READ method\n");
				fprintf(stdout,"\tscanned CCU's range is [0x%x .. 0x%x]\n", MIN_CCU, MAX_CCU);
				fprintf(stdout,"\tscanned i2c channels range is [0x%x .. 0x%x]\n", MIN_I2C_CH, MAX_I2C_CH);
				fprintf(stdout,"\tscanned i2c addresses (per channel) range is [0x%x .. 0x%x]\n", MIN_I2C_ADR, MAX_I2C_ADR);

				/* now, scan the ring */
				for (ccu_counter = MIN_CCU; ccu_counter <= MAX_CCU ; ccu_counter++)
				{
					//fprintf(stdout,"Probing ccu 0x%x \r", ccu_counter);
					write_stack[0] = ccu_counter;
					for (lcl_j=1 ; lcl_j<(sizeof(init_ccu_stack)/sizeof(__u16)); lcl_j++)
					{
						write_stack[lcl_j] = init_ccu_stack[lcl_j];
					}

					lcl_err = glue_fec_write_frame(fd, write_stack);


					if (lcl_err == DD_RETURN_OK)
					{
						fprintf(stdout,"\nCCU number 0x%x detected.\n",ccu_counter);
						for (channel_counter = MIN_I2C_CH; channel_counter <= MAX_I2C_CH ; channel_counter++)
						{
							write_stack[0] = ccu_counter;
							write_stack[1] = 0x0000;
							write_stack[2] = DD_CHANNEL_INIT_FRAME_LENGTH;
							write_stack[3] = channel_counter;
							write_stack[4] = DD_FAKE_TRANSACTION_NUMBER;
							write_stack[5] = DD_CHANNEL_COMMAND_WRITE_CTRL_A;
							write_stack[6] = (DD_CHANNEL_DATA_ENABLE_I2C_AND_ACK | DD_FRAME_EOF_BITMASK);
							lcl_err = glue_fec_write_frame(fd, write_stack);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}

							//fprintf(stdout,"\tNow probing for i2c devices attached to channel 0x%x of CCU 0x%x ; be patient ...\n", channel_counter, ccu_counter);
							for (i2c_counter = MIN_I2C_ADR; i2c_counter <= MAX_I2C_ADR ; i2c_counter++)
							{
								//fprintf(stdout,"\t\tprobing i2c address 0x%x on channel 0x%x of CCU 0x%x ; be patient ...\r", i2c_counter, channel_counter, ccu_counter);

								//Build stack for Normal Mode testing
								write_stack[0] = ccu_counter;
								write_stack[1] = 0x0000;
								write_stack[2] = 0x0004;
								write_stack[3] = channel_counter;
								write_stack[4] = DD_FAKE_TNUM_PROVOKE_ACK;
								write_stack[5] = 0x0001;
								write_stack[6] = (i2c_counter | DD_FRAME_EOF_BITMASK);
								lcl_err = glue_fec_write_frame(fd, write_stack);
								if (lcl_err == DD_RETURN_OK)
								{
									if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
									{
										read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
									}
									else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

									lcl_err = glue_fec_read_frame(fd, read_stack);
									if (lcl_err == DD_RETURN_OK)
									{
										if (read_stack[read_stack[2]+2] == 0x8004)
										{
											fprintf(stdout,"\t\tI2C device detected at address 0x%x on channel 0x%x of CCU 0x%x          \n",i2c_counter, channel_counter, ccu_counter);
										}
									}
								}
							}


						}

					}
				}
				fprintf(stdout,"END of Scan.\n");
    			break;



			/*******************************************************
			Context switch delay measurement for old ccu's
			*******************************************************/

			case DBG_OCCU_CSW:
				fprintf(stdout,"Context switch delay measurement for OLD CCU's only...\n");
				fprintf(stdout,"CCU address to use for test ? (HEXA) : ");
				scanf("%x", &lcl_scanme);
				lcl_control1 = (DD_FEC_STD_WORD)lcl_scanme;

				fprintf(stdout,"Time measurement WITHOUT context switch (gettimeofday call x2): \n");
				gettimeofday(&time1, &zone1);
				gettimeofday(&time2, &zone2);
				fprintf(stdout,"TIME1 : seconds : %d\n", (int)time1.tv_sec);
				fprintf(stdout,"TIME2 : seconds : %d\n", (int)time2.tv_sec);
				fprintf(stdout,"TIME1 : microseconds : %d\n",(int) time1.tv_usec);
				fprintf(stdout,"TIME2 : microseconds : %d\n",(int) time2.tv_usec);

				write_stack[0] = lcl_control1;
				for (lcl_j=1 ; lcl_j<(sizeof(init_ccu_stack)/sizeof(__u16)); lcl_j++)
				{
					write_stack[lcl_j] = init_ccu_stack[lcl_j];
				}
				gettimeofday(&time1, &zone1);
				lcl_err = glue_fec_write_frame(fd, write_stack);
				gettimeofday(&time2, &zone2);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					fprintf(stdout,"Successful write performed on ring ; CCU seen.\n");
					fprintf(stdout,"Time measurement for full write operation, WITH context switch : \n");
					fprintf(stdout,"TIME1 : seconds : %d\n", (int)time1.tv_sec);
					fprintf(stdout,"TIME2 : seconds : %d\n", (int)time2.tv_sec);
					fprintf(stdout,"TIME1 : microseconds : %d\n",(int) time1.tv_usec);
					fprintf(stdout,"TIME2 : microseconds : %d\n",(int) time2.tv_usec);
				}
			break;




			/*******************************************************
			COMPLETE SCANRING FOR NEW CCU's - probe devices via WRITE method
			*******************************************************/
			case DBG_NCCU_SR_WRITE:

				/* print header */
				fprintf(stdout,"\n\nScanning ring for NEW CCU's  - Devices are probed via WRITE method\n");
				fprintf(stdout,"scanned CCU's range is [0x%x .. 0x%x]\n", MIN_CCU, MAX_CCU);
				fprintf(stdout,"scanned i2c channels range is [0x%x .. 0x%x]\n", MIN_I2C_CH25, MAX_I2C_CH25);
				fprintf(stdout,"scanned i2c addresses (per channel) range is [0x%x .. 0x%x]\n", MIN_I2C_ADR, MAX_I2C_ADR);

				/* do we want to exclude one I2C channel ? */
				fprintf(stdout,"Warning : This scan function writes 0x0044 at ALL addresses of ALL I2C channels of ALL CCUs.\n");
				fprintf(stdout,"Is there one I2C channel you want to exclude from the scan? (The one who manages the PIA reset, for example...)\n");
				fprintf(stdout,"Exclude I2C channel number (enter FF for no exclusion) : (HEXA) ? : ");
				scanf("%x", &lcl_scanme);
				lcl_fecdata = (DD_FEC_STD_WORD)lcl_scanme;

				/* now, scan the ring */
				for (ccu_counter = MIN_CCU; ccu_counter <= MAX_CCU ; ccu_counter++)
				{
					write_stack[0] = ccu_counter;
					//fprintf(stdout,"Probing ccu 0x%x \r", ccu_counter);
					for (lcl_j=1 ; lcl_j<(sizeof(i2c01_talk_ccu25)/sizeof(__u16)); lcl_j++)
					{
						write_stack[lcl_j] = i2c01_talk_ccu25[lcl_j];
					}

					lcl_err = glue_fec_write_frame(fd, write_stack);
					if (lcl_err == DD_RETURN_OK)
					{
						fprintf(stdout,"\nCCU number 0x%x detected.\n",ccu_counter);
						//fprintf(stdout,"Reading back CCU25 CTRL-A\n");

						write_stack[0] = ccu_counter;
						for (lcl_j=1 ; lcl_j<(sizeof(i2c02_talk_ccu25)/sizeof(__u16)); lcl_j++)
						{
							write_stack[lcl_j] = i2c02_talk_ccu25[lcl_j];
						}
						lcl_err = glue_fec_write_frame(fd, write_stack);
						if (lcl_err == DD_RETURN_OK)
						{
							if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
							{
								read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
							}
							else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

							lcl_err = glue_fec_read_frame(fd, read_stack);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							//else fprintf(stdout,"CTRLA really read back.\n");

						}
						else glue_fecdriver_print_error_message(lcl_err);


						//fprintf(stdout,"Initializing CCU25 CTRL-E\n");
						write_stack[0] = ccu_counter;
						for (lcl_j=1 ; lcl_j<(sizeof(i2c03_talk_ccu25)/sizeof(__u16)); lcl_j++)
						{
							write_stack[lcl_j] = i2c03_talk_ccu25[lcl_j];
						}
						lcl_err = glue_fec_write_frame(fd, write_stack);
						if (lcl_err != DD_RETURN_OK)
						{
							glue_fecdriver_print_error_message(lcl_err);
						}


						//fprintf(stdout,"Reading back CCU25 CTRL-E\n");
						write_stack[0] = ccu_counter;
						for (lcl_j=1 ; lcl_j<(sizeof(i2c04_talk_ccu25)/sizeof(__u16)); lcl_j++)
						{
							write_stack[lcl_j] = i2c04_talk_ccu25[lcl_j];
						}
						lcl_err = glue_fec_write_frame(fd, write_stack);
						if (lcl_err == DD_RETURN_OK)
						{
							if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
							{
								read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
							}
							else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

							lcl_err = glue_fec_read_frame(fd, read_stack);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							//else fprintf(stdout,"CTRL E readback OK.\n");


						}
						else glue_fecdriver_print_error_message(lcl_err);


						//now, i2c channels are initialised and switched on ; let's try to probe them
						for (channel_counter = MIN_I2C_CH25; channel_counter <= MAX_I2C_CH25 ; channel_counter++)
						{
							if (channel_counter != lcl_fecdata)
							{
								//fprintf(stdout,"\tNow probing for i2c devices attached to channel 0x%x of CCU 0x%x ; be patient ...\n", channel_counter, ccu_counter);
								//fprintf(stdout,"\tInitializing I2C channel CRA\n");
								write_stack[0] = ccu_counter;
								for (lcl_j=1 ; lcl_j<(sizeof(i2c05_talk_ccu25)/sizeof(__u16)); lcl_j++)
								{
									write_stack[lcl_j] = i2c05_talk_ccu25[lcl_j];
								}
								write_stack[3] = channel_counter;
								lcl_err = glue_fec_write_frame(fd, write_stack);
								if (lcl_err != DD_RETURN_OK)
								{
									glue_fecdriver_print_error_message(lcl_err);
								}

								//fprintf(stdout,"\tReading back I2C channel CRA\n");
								write_stack[0] = ccu_counter;
								for (lcl_j=1 ; lcl_j<(sizeof(i2c06_talk_ccu25)/sizeof(__u16)); lcl_j++)
								{
									write_stack[lcl_j] = i2c06_talk_ccu25[lcl_j];
								}
								write_stack[3] = channel_counter;
								lcl_err = glue_fec_write_frame(fd, write_stack);
								if (lcl_err == DD_RETURN_OK)
								{
									if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
									{
										read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
									}
									else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

									lcl_err = glue_fec_read_frame(fd, read_stack);
									if (lcl_err != DD_RETURN_OK)
									{
										glue_fecdriver_print_error_message(lcl_err);
									}
									//else fprintf(stdout,"I2C CTRL A readback OK.\n");

								}
								else glue_fecdriver_print_error_message(lcl_err);


								// Now, scan for devices on this channel.
								for (i2c_counter = MIN_I2C_ADR; i2c_counter <= MAX_I2C_ADR ; i2c_counter++)
								{
									//fprintf(stdout,"\t\tprobing i2c address 0x%x on channel 0x%x of CCU 0x%x ; be patient ...\r", i2c_counter, channel_counter, ccu_counter);
									//Build stack for Normal Mode testing
									write_stack[0] = ccu_counter;
									write_stack[1] = 0x0000;
									write_stack[2] = 0x0005;
									write_stack[3] = channel_counter;
									write_stack[4] = DD_FAKE_TNUM_PROVOKE_ACK;
									write_stack[5] = 0x0000;
									write_stack[6] = i2c_counter;
									write_stack[7] = (0x0044 | DD_FRAME_EOF_BITMASK);
									lcl_err = glue_fec_write_frame(fd, write_stack);
									if (lcl_err == DD_RETURN_OK)
									{
										if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
										{
											read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
										}
										else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

										lcl_err = glue_fec_read_frame(fd, read_stack);
										if (lcl_err == DD_RETURN_OK)
										{
											if (read_stack[read_stack[2]+2] == 0x8004)
											{
												fprintf(stdout,"\tI2C device detected at address 0x%x on channel 0x%x of CCU 0x%x        \n",i2c_counter, channel_counter, ccu_counter);
											}
										}
									}
	                                                	}
							}
						}

					}
				}
				fprintf(stdout,"END of Scan.\n");
			break;


			
			
			
			
			
			
			
			/*******************************************************
			COMPLETE SCANRING FOR NEW CCU's - probe devices via READ method
			*******************************************************/
			case DBG_NCCU_SR_READ:

				/* print header */
				fprintf(stdout,"\n\nScanning ring for NEW CCU's - Devices are probed via READ method\n");
				fprintf(stdout,"scanned CCU's range is [0x%x .. 0x%x]\n", MIN_CCU, MAX_CCU);
				fprintf(stdout,"scanned i2c channels range is [0x%x .. 0x%x]\n", MIN_I2C_CH25, MAX_I2C_CH25);
				fprintf(stdout,"scanned i2c addresses (per channel) range is [0x%x .. 0x%x]\n", MIN_I2C_ADR, MAX_I2C_ADR);


				/* now, scan the ring */
				for (ccu_counter = MIN_CCU; ccu_counter <= MAX_CCU ; ccu_counter++)
				{
					write_stack[0] = ccu_counter;
					//fprintf(stdout,"Probing ccu 0x%x \r", ccu_counter);
					for (lcl_j=1 ; lcl_j<(sizeof(i2c01_talk_ccu25)/sizeof(__u16)); lcl_j++)
					{
						write_stack[lcl_j] = i2c01_talk_ccu25[lcl_j];
					}

					lcl_err = glue_fec_write_frame(fd, write_stack);
					if (lcl_err == DD_RETURN_OK)
					{
						fprintf(stdout,"\nCCU number 0x%x detected.\n",ccu_counter);
						//fprintf(stdout,"Reading back CCU25 CTRL-A\n");

						write_stack[0] = ccu_counter;
						for (lcl_j=1 ; lcl_j<(sizeof(i2c02_talk_ccu25)/sizeof(__u16)); lcl_j++)
						{
							write_stack[lcl_j] = i2c02_talk_ccu25[lcl_j];
						}
						lcl_err = glue_fec_write_frame(fd, write_stack);
						if (lcl_err == DD_RETURN_OK)
						{
							if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
							{
								read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
							}
							else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

							lcl_err = glue_fec_read_frame(fd, read_stack);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							//else fprintf(stdout,"CTRLA really read back.\n");

						}
						else glue_fecdriver_print_error_message(lcl_err);


						//fprintf(stdout,"Initializing CCU25 CTRL-E\n");
						write_stack[0] = ccu_counter;
						for (lcl_j=1 ; lcl_j<(sizeof(i2c03_talk_ccu25)/sizeof(__u16)); lcl_j++)
						{
							write_stack[lcl_j] = i2c03_talk_ccu25[lcl_j];
						}
						lcl_err = glue_fec_write_frame(fd, write_stack);
						if (lcl_err != DD_RETURN_OK)
						{
							glue_fecdriver_print_error_message(lcl_err);
						}


						//fprintf(stdout,"Reading back CCU25 CTRL-E\n");
						write_stack[0] = ccu_counter;
						for (lcl_j=1 ; lcl_j<(sizeof(i2c04_talk_ccu25)/sizeof(__u16)); lcl_j++)
						{
							write_stack[lcl_j] = i2c04_talk_ccu25[lcl_j];
						}
						lcl_err = glue_fec_write_frame(fd, write_stack);
						if (lcl_err == DD_RETURN_OK)
						{
							if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
							{
								read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
							}
							else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

							lcl_err = glue_fec_read_frame(fd, read_stack);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							//else fprintf(stdout,"CTRL E readback OK.\n");


						}
						else glue_fecdriver_print_error_message(lcl_err);


						//now, i2c channels are initialised and switched on ; let's try to probe them
						for (channel_counter = MIN_I2C_CH25; channel_counter <= MAX_I2C_CH25 ; channel_counter++)
						{
								
							//fprintf(stdout,"\tNow probing for i2c devices attached to channel 0x%x of CCU 0x%x ; be patient ...\n", channel_counter, ccu_counter);
							//fprintf(stdout,"\tInitializing I2C channel CRA\n");
							write_stack[0] = ccu_counter;
							for (lcl_j=1 ; lcl_j<(sizeof(i2c05_talk_ccu25)/sizeof(__u16)); lcl_j++)
							{
								write_stack[lcl_j] = i2c05_talk_ccu25[lcl_j];
							}
							write_stack[3] = channel_counter;
							lcl_err = glue_fec_write_frame(fd, write_stack);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}

							//fprintf(stdout,"\tReading back I2C channel CRA\n");
							write_stack[0] = ccu_counter;
							for (lcl_j=1 ; lcl_j<(sizeof(i2c06_talk_ccu25)/sizeof(__u16)); lcl_j++)
							{
								write_stack[lcl_j] = i2c06_talk_ccu25[lcl_j];
							}
							write_stack[3] = channel_counter;
							lcl_err = glue_fec_write_frame(fd, write_stack);
							if (lcl_err == DD_RETURN_OK)
							{
								if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
								{
									read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
								}
								else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

								lcl_err = glue_fec_read_frame(fd, read_stack);
								if (lcl_err != DD_RETURN_OK)
								{
									glue_fecdriver_print_error_message(lcl_err);
								}
								//else fprintf(stdout,"I2C CTRL A readback OK.\n");
							}
							else glue_fecdriver_print_error_message(lcl_err);


							// Now, scan for devices on this channel.
							for (i2c_counter = MIN_I2C_ADR; i2c_counter <= MAX_I2C_ADR ; i2c_counter++)
							{
								//fprintf(stdout,"\t\tprobing i2c address 0x%x on channel 0x%x of CCU 0x%x ; be patient ...\r", i2c_counter, channel_counter, ccu_counter);
								//Build stack for Normal Mode testing
								write_stack[0] = ccu_counter;
								write_stack[1] = 0x0000;
								write_stack[2] = 0x0004;
								write_stack[3] = channel_counter;
								write_stack[4] = DD_FAKE_TNUM_PROVOKE_ACK;
								write_stack[5] = 0x0001;
								write_stack[6] = (i2c_counter | DD_FRAME_EOF_BITMASK);
								lcl_err = glue_fec_write_frame(fd, write_stack);
								if (lcl_err == DD_RETURN_OK)
								{
									if (isLongFrame(write_stack[DD_MSG_LEN_OFFSET]))
									{
										read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET+1];
									}
									else read_stack[0] = write_stack[DD_TRANS_NUMBER_OFFSET];

									lcl_err = glue_fec_read_frame(fd, read_stack);
									if (lcl_err == DD_RETURN_OK)
									{
										if (read_stack[read_stack[2]+2] == 0x8004)
										{
											fprintf(stdout,"\tI2C device detected at address 0x%x on channel 0x%x of CCU 0x%x        \n",i2c_counter, channel_counter, ccu_counter);
										}
									}
								}
	                                               	}
						}

					}
				}
				fprintf(stdout,"END of Scan.\n");
			break;






			/*******************************************************
			Context switch delay measurement for new ccu's (25)
			*******************************************************/

			case DBG_NCCU_CSW:
				fprintf(stdout,"Context switch delay measurement for NEW CCU's (CCU 25) only...\n");
				fprintf(stdout,"CCU address to use for test ? (HEXA) : ");
				scanf("%x", &lcl_scanme);
				lcl_control1 = (DD_FEC_STD_WORD)lcl_scanme;
				fprintf(stdout,"Time measurement WITHOUT context switch (gettimeofday call x2): \n");
				gettimeofday(&time1, &zone1);
				gettimeofday(&time2, &zone2);
				fprintf(stdout,"TIME1 : seconds : %d\n", (int)time1.tv_sec);
				fprintf(stdout,"TIME2 : seconds : %d\n", (int)time2.tv_sec);
				fprintf(stdout,"TIME1 : microseconds : %d\n",(int) time1.tv_usec);
				fprintf(stdout,"TIME2 : microseconds : %d\n",(int) time2.tv_usec);

				write_stack[0] = lcl_control1;
				for (lcl_j=1 ; lcl_j<(sizeof(i2c01_talk_ccu25)/sizeof(__u16)); lcl_j++)
				{
					write_stack[lcl_j] = i2c01_talk_ccu25[lcl_j];
				}
				gettimeofday(&time1, &zone1);
				lcl_err = glue_fec_write_frame(fd, write_stack);
				gettimeofday(&time2, &zone2);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					fprintf(stdout,"Successful write performed on ring ; CCU seen.\n");
					fprintf(stdout,"Time measurement for full write operation, WITH context switch : \n");
					fprintf(stdout,"TIME1 : seconds : %d\n", (int)time1.tv_sec);
					fprintf(stdout,"TIME2 : seconds : %d\n", (int)time2.tv_sec);
					fprintf(stdout,"TIME1 : microseconds : %d\n",(int) time1.tv_usec);
					fprintf(stdout,"TIME2 : microseconds : %d\n",(int) time2.tv_usec);
				}
			break;


			/*******************************************************
			Dump PLX raw base addresses
			*******************************************************/
			case DBG_DUMP_BA:
				glue_plx_get_raw_ba0(fd, &lcl_pci_addr);
				fprintf(stdout,"PLX raw BA0 address is : 0x%x\n", lcl_pci_addr);
				glue_plx_get_raw_ba1(fd, &lcl_pci_addr);
				fprintf(stdout,"PLX raw BA1 address is : 0x%x\n", lcl_pci_addr);
				glue_plx_get_raw_ba2(fd, &lcl_pci_addr);
				fprintf(stdout,"PLX raw BA2 address is : 0x%x\n", lcl_pci_addr);
				glue_plx_get_raw_ba3(fd, &lcl_pci_addr);
				fprintf(stdout,"PLX raw BA3 address is : 0x%x\n", lcl_pci_addr);
			break;


			/*******************************************************
			Dump PLX remapped base addresses
			*******************************************************/
			case DBG_DUMP_RBA:
				glue_plx_get_remapped_ba0(fd, &lcl_pci_addr);
				fprintf(stdout,"PLX remapped BA0 address is : 0x%x\n", lcl_pci_addr);
				glue_plx_get_remapped_ba1(fd, &lcl_pci_addr);
				fprintf(stdout,"PLX remapped BA1 address is : 0x%x\n", lcl_pci_addr);
				glue_plx_get_remapped_ba2(fd, &lcl_pci_addr);
				fprintf(stdout,"PLX remapped BA2 address is : 0x%x\n", lcl_pci_addr);
				glue_plx_get_remapped_ba3(fd, &lcl_pci_addr);
				fprintf(stdout,"PLX remapped BA3 address is : 0x%x\n", lcl_pci_addr);
			break;



			/*******************************************************
			Enter a frame and send it to the ring - RAW write method
			*******************************************************/
			case DBG_WF_RAW:
				fprintf(stdout,"Enter frame to send to the ring\n");
				fprintf(stdout,"Frame will be wrote in fifotransmit as rawdata ; no preprocessing.\n");
				fprintf(stdout,"Last element (not stored in fifotra) must be 0xFFFF\n");
				lcl_i = 0;
				lcl_fifotra_16 = 0x0000;
				lcl_fifotra_32 = 0xffff;
				do
				{
					fprintf(stdout,"Enter value for element %i of frame : (HEXA) : ", lcl_i);
					scanf("%x", &lcl_scanme);

	
					if (lcl_fifo_depth == 16)
					{
						lcl_fifotra_16 = (DD_FEC_FIFO_DATA_16)lcl_scanme;
						lcl_err = glue_fec_set_native_fifotra_item_16(fd, lcl_fifotra_16);
					}
					else
					{
						lcl_fifotra_32 = (DD_FEC_FIFO_DATA_32)lcl_scanme;
						lcl_err = glue_fec_set_native_fifotra_item_32(fd, lcl_fifotra_32);
					}
					if (lcl_err != DD_RETURN_OK)
					{
						glue_fecdriver_print_error_message(lcl_err);
					}
					else
					{
						if (lcl_fifo_depth == 16)
						{
							fprintf(stdout,"Value wrote in FEC Fifo Transmit : 0x%x\n",lcl_fifotra_16);
						}
						else fprintf(stdout,"Value wrote in FEC Fifo Transmit : 0x%x\n",lcl_fifotra_32);
					}
				} while ( (lcl_fifotra_16 != 0xffff) && (lcl_fifotra_32 != 0xffff) );
				fprintf(stdout,"Sending frame on ring...\n");
				lcl_fecdata = 0x0001;
				lcl_err = glue_fec_set_ctrl0(fd, lcl_fecdata);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				lcl_fecdata = 0x0003;
				lcl_err = glue_fec_set_ctrl0(fd, lcl_fecdata);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				lcl_fecdata = 0x0001;
				lcl_err = glue_fec_set_ctrl0(fd, lcl_fecdata);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				fprintf(stdout,"Frame Sent.\n");
			break;



			/*******************************************************
			Enter a frame and send it to the ring - glue_fec_write_frame write method
			*******************************************************/
			case DBG_WF_GLUE:
				fprintf(stdout,"Enter frame to send to the ring\n");
				fprintf(stdout,"glue_fec_write_frame method will be used.\n");
				fprintf(stdout,"Last element must be OR-masked by 0x8000\n");
				lcl_i = 0;
				do
				{
					fprintf(stdout,"Enter value for element %i of frame : (HEXA) : ", lcl_i);
					scanf("%x", &lcl_scanme);
					lcl_fifotra_16 = (DD_FEC_STD_WORD)lcl_scanme;
					onfly_stack[lcl_i]=lcl_fifotra_16;
					lcl_i++;
				} while( !(lcl_fifotra_16 & 0x8000) );
				fprintf(stdout,"Sending frame on ring...\n");
				lcl_err = glue_fec_write_frame(fd, onfly_stack);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"Frame Sent ; Transaction number set to : 0x%x\n", onfly_stack[0]);



			break;

			/*******************************************************
			Read back a forced acknowledge or a data frame
			*******************************************************/
			case DBG_RBF:

				fprintf(stdout,"Reading back forced-ack or data frame\n");
				fprintf(stdout,"Enter transaction number for data/ack readback : (HEXA) : ");
				scanf("%x", &lcl_scanme);
				lcl_fifotra_16 = (DD_FEC_STD_WORD)lcl_scanme;
				read_stack[0] = lcl_fifotra_16;
				lcl_err = glue_fec_read_frame(fd, read_stack);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{

					lcl_i = -1;
					fprintf(stdout,"Data/ACK frame is :\n");
					do
					{
						lcl_i++;
						fprintf(stdout,"\t0x%x\n", read_stack[lcl_i]);
					} while( !(read_stack[lcl_i] & 0x8000) );

				}
			break;




			/*******************************************************
			QUICK SCANRING FOR OLD CCU's
			*******************************************************/
			case DBG_OCCU_QS:
				fprintf(stdout,"Scanning ring for OLD CCU's\n");
				fprintf(stdout,"\tscanned CCU's range is [0x%x .. 0x%x]\n", MIN_CCU, MAX_CCU);

				for (ccu_counter = MIN_CCU; ccu_counter <= MAX_CCU ; ccu_counter++)
				{
					write_stack[0] = ccu_counter;
					fprintf(stdout,"Probing ccu 0x%x \r", ccu_counter);
					for (lcl_j=1 ; lcl_j<(sizeof(init_ccu_stack)/sizeof(__u16)); lcl_j++)
					{
						write_stack[lcl_j] = init_ccu_stack[lcl_j];
					}

					lcl_err = glue_fec_write_frame(fd, write_stack);
					if (lcl_err == DD_RETURN_OK) fprintf(stdout,"\nCCU number 0x%x detected.\n",ccu_counter);
				}
    			break;


			/*******************************************************
			QUICK SCANRING FOR NEW CCU's (25)
			*******************************************************/
			case DBG_NCCU_QS:
				fprintf(stdout,"Scanning ring for NEW CCU's\n");
				fprintf(stdout,"\tscanned CCU's range is [0x%x .. 0x%x]\n", MIN_CCU, MAX_CCU);
				for (ccu_counter = MIN_CCU; ccu_counter <= MAX_CCU ; ccu_counter++)
				{
					write_stack[0] = ccu_counter;
					fprintf(stdout,"Probing ccu 0x%x \r", ccu_counter);
					for (lcl_j=1 ; lcl_j<(sizeof(i2c01_talk_ccu25)/sizeof(__u16)); lcl_j++)
					{
						write_stack[lcl_j] = i2c01_talk_ccu25[lcl_j];
					}

					lcl_err = glue_fec_write_frame(fd, write_stack);
					if (lcl_err == DD_RETURN_OK) fprintf(stdout,"\nCCU number 0x%x detected.\n",ccu_counter);
				}
    			break;


			/*******************************************************
			Write frame and read back direct and forced/data result
			*******************************************************/
			case DBG_WF_RBF:
				fprintf(stdout,"Enter frame to send to the ring\n");
				fprintf(stdout,"glue_fec_write_frame method will be used.\n");
				fprintf(stdout,"Last element must be OR-masked by 0x8000\n");
				lcl_i = 0;
				do
				{
					fprintf(stdout,"Enter value for element %i of frame : (HEXA) : ", lcl_i);
					scanf("%x", &lcl_scanme);
					lcl_fifotra_16 = (DD_FEC_STD_WORD)lcl_scanme;
					onfly_stack[lcl_i]=lcl_fifotra_16;
					lcl_i++;
				} while( !(lcl_fifotra_16 & 0x8000) );
				fprintf(stdout,"Sending frame on ring...\n");
				lcl_err = glue_fec_write_frame(fd, onfly_stack);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					fprintf(stdout,"Frame Sent ; Transaction number set to : 0x%x\n", onfly_stack[0]);
					fprintf(stdout,"Direct acknowledge frame is :\n");
					lcl_i=1;
					while (!(onfly_stack[lcl_i] & 0x8000))
					{
						fprintf(stdout,"\t0x%x\n", onfly_stack[lcl_i]);
						lcl_i++;
					}
					fprintf(stdout,"\t0x%x\n", onfly_stack[lcl_i]);
				}
				fprintf(stdout,"Forced acknowledge or data frame is : \n");
				if (isLongFrame(onfly_stack[DD_MSG_LEN_OFFSET]))
				{
					read_stack[0] = onfly_stack[DD_TRANS_NUMBER_OFFSET+1];
				}
				else read_stack[0] = onfly_stack[DD_TRANS_NUMBER_OFFSET];
				lcl_err = glue_fec_read_frame(fd, read_stack);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					lcl_i=0;
					while (!(read_stack[lcl_i] & 0x8000))
					{
						fprintf(stdout,"\t0x%x\n", read_stack[lcl_i]);
						lcl_i++;
					}
					fprintf(stdout,"\t0x%x\n", read_stack[lcl_i]);
				}

			break;



			/*******************************************************
			ERROR CODE TRANSCRIPTION
			*******************************************************/
			case DBG_DECODE_ERR:
				fprintf(stdout,"Enter error code to decode ? : ");
				scanf("%d", &lcl_err);
				glue_fecdriver_get_error_message(lcl_err, lcl_error_msg);
				fprintf(stdout,"Attached Message : %s\n", lcl_error_msg);
			break;



			/*******************************************************
			Loop a write/readback sequence of command issued from a file
			*******************************************************/
			case DBG_LOOP_FILE_WF_RBF:
				fprintf(stdout,"Enter filename where frames are stored : ");
				scanf("%s",frames_filename);
				filein=fopen(frames_filename,"r");
				if (filein == NULL)
				{
					fprintf(stdout,"Erreur lors de l'ouverture du fichier %s\n", frames_filename);
					break;
				}

				fprintf(stdout,"How many loops do you want to perform on this block of frames? : ");
				scanf("%d",&file_loops);
				for (loops_counter = 0; loops_counter < file_loops; loops_counter++)
				{

					lcl_counter=0;
					while (!(feof(filein)))
					{

						lcl_i = 0;
						fprintf(stdout,"I WILL PROCESS THIS FRAME : \n");
						do
						{
							fscanf(filein,"%s",frame_asc_value);
							if (feof(filein)) goto FILE_REWIND;
							if (frame_asc_value[0] != '#')
							{
								sscanf(frame_asc_value, "%x", &lcl_scanme);
								frame_hex_value = (DD_FEC_STD_WORD)lcl_scanme;
								fprintf(stdout,"\t0x%x\n", frame_hex_value);
								onfly_stack[lcl_i]=frame_hex_value;
								lcl_i++;
							}
							else
							{
								//fprintf(stdout,"Frame INFO : %s\n", frame_asc_value);
								frame_hex_value = 0x0000;
							}
						} while( !(frame_hex_value & 0x8000) );
						fprintf(stdout,"Now sending frame on ring...\n");


						lcl_err = glue_fec_write_frame(fd, onfly_stack);
						if (lcl_err != DD_RETURN_OK)
						{
							glue_fecdriver_print_error_message(lcl_err);
							goto EXIT_IF_ERROR;
						}
						else
						{
							fprintf(stdout,"Direct acknowledge frame is :\n");
							lcl_i=0;
							while (!(onfly_stack[lcl_i] & 0x8000))
							{
								fprintf(stdout,"\t0x%x\n", onfly_stack[lcl_i]);
								lcl_i++;
							}
							fprintf(stdout,"\t0x%x\n", onfly_stack[lcl_i]);
						}

						fprintf(stdout,"Forced acknowledge or data frame is : \n");

						if (isLongFrame(onfly_stack[DD_MSG_LEN_OFFSET]))
						{
							read_stack[0] = onfly_stack[DD_TRANS_NUMBER_OFFSET+1];
						}
						else read_stack[0] = onfly_stack[DD_TRANS_NUMBER_OFFSET];

						lcl_err = glue_fec_read_frame(fd, read_stack);
						if (lcl_err != DD_RETURN_OK)
						{
							glue_fecdriver_print_error_message(lcl_err);
							goto EXIT_IF_ERROR;
						}
						else
						{
							lcl_i=0;
							while (!(read_stack[lcl_i] & 0x8000))
							{
								fprintf(stdout,"\t0x%x\n", read_stack[lcl_i]);
								lcl_i++;
							}
							fprintf(stdout,"\t0x%x\n", read_stack[lcl_i]);
						}
						lcl_counter++;
					}
					FILE_REWIND:
					fprintf(stdout,"***********************************************************\n");
					fprintf(stdout,"Loop %i/%i achieved - successful.\n", loops_counter+1, file_loops);
					fprintf(stdout,"***********************************************************\n\n\n");
					rewind(filein);

				}
				fprintf(stdout,"End of loop.\n");
				EXIT_IF_ERROR:
				fclose(filein);

			break;




			/*******************************************************
			Ring Error Management sub-menu
			*******************************************************/

			case DBG_REM:
				lcl_input=1;
				while (lcl_input != 0)
				{
					fprintf(stdout,"\n\tRing Error Management SUB-MENU\n");
					fprintf(stdout,"\t%d : Turn Ring Error Management ON\n",DD_TURN_REM_ON);
					fprintf(stdout,"\t%d : Turn Ring Error Management OFF\n",DD_TURN_REM_OFF);
					fprintf(stdout,"\t%d : Reset Ring Error Management counters\n",DD_RESET_REM_VALUE);
					fprintf(stdout,"\t%d : Get Ring Error Management counters values\n",DD_GET_REM_VALUE);
					fprintf(stdout,"\t%d : Get Ring Error Management current status\n",DD_GET_REM_STATUS);
					fprintf(stdout,"\t%d : Exit sub-menu\n", DBG_QUIT);
					fprintf(stdout,"\tYour choice : ");
					scanf("%d",&lcl_input);
					switch (lcl_input)
					{
						case DD_TURN_REM_ON:
							lcl_ring_errors_manager[0]=DD_TURN_REM_ON;
							lcl_err = glue_fec_ring_error_manager(fd, lcl_ring_errors_manager);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							else fprintf(stdout,"\t\tRing Error Management is now : ON\n");
						break;

						case DD_TURN_REM_OFF:
							lcl_ring_errors_manager[0]=DD_TURN_REM_OFF;
							lcl_err = glue_fec_ring_error_manager(fd, lcl_ring_errors_manager);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							else fprintf(stdout,"\t\tRing Error Management is now : OFF\n");
						break;
						
						case DD_RESET_REM_VALUE:
							lcl_ring_errors_manager[0]=DD_RESET_REM_VALUE;
							lcl_err = glue_fec_ring_error_manager(fd, lcl_ring_errors_manager);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							else fprintf(stdout,"\t\tRing Error Management counters resetted\n");

						break;
							
						case DD_GET_REM_VALUE:
							lcl_ring_errors_manager[0]=DD_GET_REM_VALUE;
							lcl_err = glue_fec_ring_error_manager(fd, lcl_ring_errors_manager);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							else
							{
								fprintf(stdout,"\t\tFrame too long errors :%d\n",lcl_ring_errors_manager[DD_LONG_FRAME_ERROR_OFFSET]);
								fprintf(stdout,"\t\tFrame too short (ring noisy) :%d\n",lcl_ring_errors_manager[DD_SHORT_FRAME_ERROR_OFFSET]);
								fprintf(stdout,"\t\tWrong transaction number received :%d\n",lcl_ring_errors_manager[DD_WRONG_TRANS_NUMBER_ERROR_OFFSET]);
								fprintf(stdout,"\t\tWarnings emitted by devices :%d\n",lcl_ring_errors_manager[DD_WARNING_FROM_DEVICE_ERROR_OFFSET]);
							}
						break;

						case DD_GET_REM_STATUS:
							lcl_ring_errors_manager[0]=DD_GET_REM_STATUS;
							lcl_err = glue_fec_ring_error_manager(fd, lcl_ring_errors_manager);
							if (lcl_err != DD_RETURN_OK)
							{
								glue_fecdriver_print_error_message(lcl_err);
							}
							else
							{
								if (lcl_ring_errors_manager[0]==DD_FLAG_IS_ENABLED)
								{
									fprintf(stdout,"\t\tRing Error Management status is : ON\n");
								}
								else fprintf(stdout,"\t\tRing Error Management status is : OFF\n");
							}
						break;

						default:
							fprintf(stdout,"\tError : invalid choice.\n");
						break;

					} //end of switch
				} //end of while
					
			break;




			/*******************************************************
			get PLX resets counter
			*******************************************************/

		    case DBG_RPLXCOUNTER:
        			lcl_err = glue_fec_get_plx_reset_counter(fd, &lcl_u32_value);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"The PLX has performed %d resets until now.\n",(int)lcl_u32_value);
			break;


			/*******************************************************
			get FEC resets counter
			*******************************************************/

			case DBG_RFECCOUNTER:
				lcl_err = glue_fec_get_fec_reset_counter(fd, &lcl_u32_value);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"The FEC has performed %d resets until now.\n",(int)lcl_u32_value);
			break;


			/*******************************************************
			get Driver General Status
			*******************************************************/

			case DBG_GETSTATUS:
				lcl_err = glue_fec_get_driver_general_status(fd, &lcl_u16_value);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					if isIrqOn(lcl_u16_value)
					{
						fprintf(stdout,"IRQs are ON\n");
					}
					else fprintf(stdout,"IRQs are OFF\n");

					if existsFaultOnLongFrames(lcl_u16_value)
					{
						fprintf(stdout,"You have SOME errors of kind : long frame\n");
					}
					else fprintf(stdout,"You have NO errors of kind : long frame\n");


					if (!existsFaultOnShortFrames(lcl_u16_value))
					{
						fprintf(stdout,"You have NO errors of kind : short frame\n");
					}
					else fprintf(stdout,"You have SOME errors of kind : short frame\n");


					if (!existsFaultOnTransNumber(lcl_u16_value))
					{
						fprintf(stdout,"You have NO errors of kind : bad transaction number\n");
					}
					else fprintf(stdout,"You have SOME errors of kind : bad transaction number\n");


					if (!existsWarningEmittedByDevice(lcl_u16_value))
					{
						fprintf(stdout,"You have NO errors of kind : warning emitted by device\n");
					}
					else fprintf(stdout,"You have SOME errors of kind : warning emitted by device\n");

				}
			break;


			case DBG_WAITWARNING:
				fprintf(stdout,"Waiting to see a warning emitted by a device...\n");
				lcl_err = glue_fec_wait_warning(fd, read_stack);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else
				{
					fprintf(stdout,"Warning catched as frame :\n");
					for (lcl_i=0; lcl_i<DD_USER_MAX_MSG_LENGTH; lcl_i++)
					{
						fprintf(stdout,"\t0x%x\n", read_stack[lcl_i]);
					}
					fprintf(stdout,"End of warning printing.\n");
				}

			break;


			case DBG_EMULATEWARNING:
				fprintf(stdout,"Sending warning emulation signal...\n");
				lcl_err = glue_fec_simulate_warning(fd);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"Signal sent.\n");

			break;



			case 100:
				fprintf(stdout,"Reading FEC versionning string...\n");
				lcl_err = glue_fecdriver_get_driver_version(fd, lcl_driver_version);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"Driver version string is : %s\n", lcl_driver_version);

			break;


			case 101:
				fprintf(stdout,"Writing to ctrl0 with glue_fec_write_register\n");
				printf("Size of value container is : %d\n", sizeof(lcl_fec_array[DD_FEC_VALUE_INDEX]));
				printf("Size of offset container is : %d\n", sizeof(lcl_fec_array[DD_FEC_OFFSET_INDEX]));

				lcl_fec_array[DD_FEC_VALUE_INDEX] = 0x0001;
				lcl_fec_array[DD_FEC_OFFSET_INDEX] = DD_FEC_CTRL0_OFFSET;
				lcl_err = glue_fec_write_register(fd, lcl_fec_array);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"Value wrote to FEC CTRL 0\n");
			break;


			case 102:
				fprintf(stdout,"Reading from ctrl0 with glue_fec_read_register\n");
				printf("Size of value container is : %d\n", sizeof(lcl_fec_array[DD_FEC_VALUE_INDEX]));
				printf("Size of offset container is : %d\n", sizeof(lcl_fec_array[DD_FEC_OFFSET_INDEX]));

				lcl_fec_array[DD_FEC_VALUE_INDEX] = 0x0001;
				lcl_fec_array[DD_FEC_OFFSET_INDEX] = DD_FEC_CTRL0_OFFSET;
				lcl_err = glue_fec_read_register(fd, lcl_fec_array);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"Value read from FEC CTRL 0 : 0x%x\n", lcl_fec_array[DD_FEC_VALUE_INDEX]);
			break;



			/*******************************************************
			Read firmware version
			*******************************************************/
			case DBG_READ_FIRMWARE_VERSION:
				lcl_err = glue_fec_get_firmware_version(fd, &lcl_control0);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"FEC Firmware version is : 0x%x\n",lcl_control0);
			break;


			/*******************************************************
			Read firmware date
			*******************************************************/
/*
			case DBG_READ_FIRMWARE_DATE:
				lcl_err = glue_fec_get_firmware_date(fd, &lcl_control0);
				if (lcl_err != DD_RETURN_OK)
				{
					glue_fecdriver_print_error_message(lcl_err);
				}
				else fprintf(stdout,"FEC Firmware date is : 0x%x\n",lcl_control0);
			break;

*/


			/*******************************************************
			Default gate
			*******************************************************/
			default:
				fprintf(stdout,"Error : invalid choice.\n");
			break;



		}

	}


	/***********************************************************************
	Fermeture du driver
	***********************************************************************/
	if (close(fd)) fprintf(stdout,"Erreur while closing the driver.\n");


return DD_RETURN_OK;
}


