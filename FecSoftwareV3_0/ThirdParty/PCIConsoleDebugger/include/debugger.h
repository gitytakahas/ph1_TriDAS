/*
   FileName : 		FRAME_VALUES.H

   Content : 		Ring Frames prerequisites and fixed values

   Used in : 		DEBUGGER

   Programmer : 	Laurent GROSS

   Version : 		Unified-6.0

   Date of last modification : 17/05/2005

   Support : 		mail to : fec-support@ires.in2p3.fr

   Remarks : 		This file have not to be modified by users
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
#ifndef DD_FRAMEVALUES_H
#define DD_FRAMEVALUES_H


/**************************************************/
/**************************************************/
/* 		main menu related defines 	  */
/**************************************************/
/**************************************************/
#define DBG_QUIT			0
#define DBG_WCR0			1
#define DBG_RCR0			2
#define DBG_WCR1			3
#define DBG_RSR0			4
#define DBG_RSR1			5
#define DBG_WFIFOTRA			6
#define DBG_RFIFOTRA			7
#define DBG_WFIFORET			8
#define DBG_RFIFORET			9
#define DBG_WFIFOREC			10
#define DBG_RFIFOREC			11
#define DBG_DUMPPLX			12
#define DBG_DUMPPCI			13
#define DBG_RESETTOONE			14
#define DBG_ENABLEIRQ			15
#define DBG_DISABLEIRQ			16
#define DBG_INITTTCRX			17
#define DBG_RESETPLX			18
#define DBG_RESETFEC			19
#define DBG_OCCU_SR_WRITE		20
#define DBG_OCCU_SR_READ		21
#define DBG_OCCU_CSW			22
#define DBG_NCCU_SR_WRITE		23
#define DBG_NCCU_SR_READ		24
#define DBG_NCCU_CSW			25
#define DBG_DUMP_BA			26
#define DBG_DUMP_RBA			27
#define DBG_WF_RAW			28
#define DBG_WF_GLUE			29
#define DBG_RBF				30
#define DBG_OCCU_QS			31
#define DBG_NCCU_QS			32
#define DBG_WF_RBF			33
#define DBG_DECODE_ERR			34
#define DBG_LOOP_FILE_WF_RBF		35
#define DBG_REM				36
#define DBG_RPLXCOUNTER			37
#define DBG_RFECCOUNTER			38
#define DBG_GETSTATUS			39
#define DBG_WAITWARNING			40
#define DBG_EMULATEWARNING		41
#define DBG_READ_FIRMWARE_VERSION	42
#define DBG_READ_FIRMWARE_DATE		43



/**************************************************/
/**************************************************/
/* frames constants and ccus commands definitions */
/**************************************************/
/**************************************************/

/*!
LENGTH (+ the 3 first elements) of an INIT_CCU frame
*/
#define DD_CCU_INIT_FRAME_LENGTH				0x0004
/*!
CHANNEL number of the node controller of a CCU
*/
#define DD_CCU_NODE_CONTROLLER_CHANNEL				0x0000
/*!
COMMAND corresponding to  : write in CCU control register A
*/
#define DD_CCU_COMMAND_WRITE_CTRL_A				0x0000
/*!
DATA corresponding to : Enable all channels kind (I2C, ...) of a CCU
*/
#define DD_CCU_DATA_ENABLE_ALL					0x000F
/*!
LENGTH (+ the 3 first elements) of an INIT_CHANNEL frame
*/
#define DD_CHANNEL_INIT_FRAME_LENGTH				0x0004
/*!
COMMAND corresponding to  : For a given channel, write in control register A
*/
#define DD_CHANNEL_COMMAND_WRITE_CTRL_A				0x00F0
/*!
DATA corresponding to : For a given channel, enable I2C and force ACK's
*/
#define DD_CHANNEL_DATA_ENABLE_I2C_AND_ACK			0x00C0
/*!
EOF mask for ring frames
*/
#define DD_FRAME_EOF_BITMASK					0x8000
/*!
fake transaction number ; this number is automatically changed in the driver
*/
#define DD_FAKE_TRANSACTION_NUMBER				0x0001
/*!
LENGTH (+ the 3 first elements) of a normal_mode_syntax frame for a write op.
*/
#define DD_CCU_WRITE_NORMAL_MODE_FRAME_LENGTH			0x0005
/*!
COMMAND corresponding to  : write to CCU in single byte, normal mode
*/
#define DD_SINGLE_BYTE_WRITE_NORMAL_MODE			0x0000


/* DEFINES FOR CCU25 - names should be explicit enough */
#define DD_CCU25_WRITE_CRA_FRAME_LENGTH           	 	0x0004
#define DD_CCU25_READ_CRA_FRAME_LENGTH            	 	0x0003
#define DD_CCU25_READ_CRE_FRAME_LENGTH            	 	0x0003
#define DD_CCU25_COMMAND_WRITE_CTRL_A             	 	0x0000
#define DD_CCU25_COMMAND_READ_CTRL_A				0x0010
#define DD_CCU25_COMMAND_READ_CTRL_E				0x0014
#define DD_CCU25_DATA_WCRA_ON_INIT                 		0x00A0






/* End Block - avoid multiple inclusions of this file */
#endif



