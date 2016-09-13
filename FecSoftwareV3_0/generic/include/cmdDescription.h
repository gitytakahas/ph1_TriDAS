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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef CMDDESCRIPTION_H
#define CMDDESCRIPTION_H

/** ---------------------------------------------------- **/
/** Commands needed
 */
// FEC trigger status 0
#define CCS_SR0_TTCRX_READY    0x1
#define CCS_SR0_QPLL_LOCKED    0x2
#define CCS_SR0_QPLL_ERROR     0x4

// FEC trigger control 0
#define CCS_QPLL_F0SELECT       0x00F
#define CCS_QPLL_AUTORESTART    0x010
#define CCS_QPLL_RESET          0x020
#define CCS_QPLL_EXTCTRL        0x040
#define CCS_TTCRX_RESET         0x080
#define CCS_FORCELOCALMODE      0x100

// CR0
#define FEC_CR0_ENABLEFEC       0x0001
#define FEC_CR0_SEND            0x0002
#define FEC_CR0_XTALCLOCK       0x0004  // Selection of the internal clock
#define FEC_CR0_SELSEROUT       0x0008
#define FEC_CR0_SELSERIN	0x0010
#define FEC_CR0_RESETTCRX       0x0020
#define FEC_CR0_POLARITY        0x0080
#define FEC_CR0_DISABLERECEIVE  0x0100
#define FEC_CR0_LOOPBACK        0x0200
#define FEC_CR0_RESETFSMFEC     0x2000
#define FEC_CR0_RESETRINGB      0x4000
#define FEC_CR0_RESETOUT        0x8000

// CR1
#define FEC_CR1_CLEARIRQ        0x0001
#define FEC_CR1_CLEARERRORS     0x0002
#define FEC_CR1_RELEASEFEC      0x0004

// SR0
#define FEC_SR0_TRARUN          0x0001
#define FEC_SR0_RECRUN          0x0002
#define FEC_SR0_RECFULL         0x0008
#define FEC_SR0_RECEMPTY        0x0010
#define FEC_SR0_RETFULL         0x0040
#define FEC_SR0_RETEMPTY        0x0080
#define FEC_SR0_TRAFULL         0x0200
#define FEC_SR0_TRAEMPTY        0x0400
#define FEC_SR0_LINKINITIALIZED 0x0800
#define FEC_SR0_PENDINGIRQ      0x1000
#define FEC_SR0_DATATOFEC       0x2000
#define FEC_SR0_TTCRXREADY      0x4000

// SR1
#define FEC_SR1_ILLDATA		0x0001
#define FEC_SR1_ILLSEQ		0x0002
#define FEC_SR1_CRCERROR	0x0004
#define FEC_SR1_DATACOPIED      0x0008
#define FEC_SR1_ADDRSEEN        0x0010
#define FEC_SR1_ERROR           0x0020
#define FEC_SR1_TIMEOUT		0x0040
#define FEC_SR1_CLOCKERROR	0x0080

// Command for the CCU
#define CMD_CCUWRITECRA 0x00
#define CMD_CCUWRITECRB 0x01
#define CMD_CCUWRITECRC 0x02
#define CMD_CCUWRITECRD 0x03
#define CMD_CCUWRITECRE 0x04  // Only for the CCU25
#define CMD_CCUREADCRA  0x10
#define CMD_CCUREADCRB  0x11
#define CMD_CCUREADCRC  0x12
#define CMD_CCUREADCRD  0x13
#define CMD_CCUREADCRE  0x14  // Only for the CCU25

#define CMD_CCUREADSRA  0x20
#define CMD_CCUREADSRB  0x21
#define CMD_CCUREADSRC  0x22
#define CMD_CCUREADSRD  0x23
#define CMD_CCUREADSRE  0x24
#define CMD_CCUREADSRF  0x25
#define CMD_CCUREADSRG  0x26 // Only for the old CCU
#define CMD_CCUREADSRH  0x27 // Only for the old CCU

// Command for i2c channel
#define CMD_SINGLEBYTEWRITENORMALMODE   0x00
#define CMD_SINGLEBYTEREADNORMALMODE    0x01
#define CMD_SINGLEBYTEWRITEEXTENDEDMODE 0x02
#define CMD_SINGLEBYTEREADEXTENDEDMODE  0x03
#define CMD_SINGLEBYTEWRITERALMODE      0x02
#define CMD_SINGLEBYTEREADRALMODE       0x11
#define CMD_CHANNELI2CWRITECRA          0xF0
#define CMD_CHANNELI2CREADCRA           0xF1
#define CMD_CHANNELI2CREADSRA           0xF2
#define CMD_CHANNELI2CREADSRB           0xF3
#define CMD_CHANNELI2CREADSRC           0xF4
#define CMD_CHANNELI2CREADSRD           0xF5
#define CMD_CHANNELRESETI2C             0xFF

// Command for pia channel
#define CMD_CHANNELPIAWRITEGCR          0x01
#define CMD_CHANNELPIAREADGCR           0x02
#define CMD_CHANNELPIAWRITEDDR          0x05
#define CMD_CHANNELPIAREADDDR           0x22
#define CMD_CHANNELPIAWRITEDATAREG      0x10
#define CMD_CHANNELPIAREADDATAREG       0x11
#define CMD_CHANNELPIAREADSTATUS        0x0F
#define CMD_CHANNELRESETPIA             0xFF

// Command for memory channel
#define CMD_MBUSRESETCHANNEL            0xFF
#define CMD_CHANNELMEMWRITECRA          0x01
#define CMD_CHANNELMEMREADCRA           0x02
#define CMD_CHANNELMEMWRITEWIN1LREG     0x03
#define CMD_CHANNELMEMREADWIN1LREG      0x04
#define CMD_CHANNELMEMWRITEWIN1HREG     0x05
#define CMD_CHANNELMEMREADWIN1HREG      0x06
#define CMD_CHANNELMEMWRITEWIN2LREG     0x07
#define CMD_CHANNELMEMREADWIN2LREG      0x08
#define CMD_CHANNELMEMWRITEWIN2HREG     0x09
#define CMD_CHANNELMEMREADWIN2HREG      0x0A
#define CMD_CHANNELMEMWRITEMASKREG      0x0B
#define CMD_CHANNELMEMREADMASKREG       0x0C
#define CMD_CHANNELMEMREADSTATUSREG     0x0F
#define CMD_CHANNELMEMSINGLEBYTEWRITE   0x10
#define CMD_CHANNELMEMSINGLEBYTEREAD    0x11
#define CMD_CHANNELMEMMULTIPLEBYTEREAD  0x14
#define CMD_CHANNELMEMMULTIPLEBYTEWRITE 0x15
#define CMD_CHANNELMEMSINGLEBYTERMW_AND 0x20
#define CMD_CHANNELMEMSINGLEBYTERMW_OR  0x21
#define CMD_CHANNELMEMSINGLEBYTERMW_XOR 0x22

// Command for trigger channel 
#define CMD_CHANNELTRIGGERWRITECRA      0x00	
#define CMD_CHANNELTRIGGERREADCRA       0x01
#define CMD_CHANNELTRIGGERWRITECRB      0x03	
#define CMD_CHANNELTRIGGERREADCRB       0x04
#define CMD_CHANNELTRIGGERREADSRA       0x02
#define CMD_CHANNELTRIGGERREADCNT0      0x05
#define CMD_CHANNELTRIGGERREADCNT1      0x06
#define CMD_CHANNELTRIGGERREADCNT2      0x07
#define CMD_CHANNELTRIGGERREADCNT3      0x08
#define CMD_CHANNELTRIGGERRESET         0xff

/** ---------------------------------------------------- **/
// Bits of the different CCU registers

// CRA for the CCU
#define CCU_CRA_EXTRES                  0x20   // Generated External reset 
#define CCU_CRA_CLRE                    0x40   // Clear error
#define CCU_CRA_RES                     0x80   // Reset all the channels

// CRB for the CCU
#define CCU_CRB_ENAL1                   0x1    // Enable alarm 1
#define CCU_CRB_ENAL2                   0x2    // Enable alarm 2
#define CCU_CRB_ENAL3                   0x4    // Enable alarm 3
#define CCU_CRB_ENAL4                   0x8    // Enable alarm 4
#define CCU_CRB_RTRY                    0x30   // Retry count for alarm interrupts

// CRC for the CCU
#define CCU_CRC_ALTIN                   0x1    // Input port A/B
#define CCU_CRC_SSP                     0x2    // Output port A/B

// CRD for the CCU
#define CCU_CRD_BCLS                    0x3F   // Broadcast class

// CCU SRA
#define CCU_SRA_CRC_ERROR               0x1    // CRC error
#define CCU_SRA_IN_ERROR                0x2    // internal node error
#define CCU_SRA_ALSET                   0x4    // alarm input active
#define CCU_SRA_PAR_ERROR               0x8    // parity error in CCU internal register
#define CCU_SRA_PARCH_ERROR             0x10   // parity error in any of the channels
#define CCU_SRA_ISEQ_ERROR              0x20   // illegal sequence of tokens detected
#define CCU_SRA_ICMD_ERROR              0x40   // Invalid command detected
#define CCU_SRA_GEN_ERROR               0x80   // error detected in one of the channels.

// CCU SRC
#define CCU_SRC_INPUTPORT               0x1    // Input port
#define CCU_SRC_OUTPUTPORT              0x2    // Output port

// i2c channel SRA
#define I2C_SRA_SUCC                    0x4    // Last transaction successfully executued
#define I2C_SRA_I2CLOW                  0x8    // I2C SDA line is pulled low
#define I2C_SRA_INVCOM                  0x20   // Invalid command sent to the i2c channel
#define I2C_SRA_NOACK                   0x40   // I2C transaction not acknowledged by the I2C slave
#define I2C_SRA_GE                      0x80   // An errors occurs on the i2c channel

// i2c channel CRA
#define I2C_CRA_SPEED                   0x3    // I2C speed (0 => 100Khz, 1 => 200Khz, 2 => 400 Khz, 3 => 1Mhz
#define I2C_CRA_EBRDCST                 0x20   // Enable broadcast operation
#define I2C_CRA_FACKW                   0x40   // Force acknowledge for write or RMW operation

/** ---------------------------------------------------- **/
// [1 to 127], 0 is not allowed
#define MAXTRANSACTIONNUMBER 127 

/** ---------------------------------------------------- **/
/** Status register needed
 */
// Bit to be set for the end of message
#define DD_FIFOTRA_EOFRAME_MASK 0x8000
// Last word of a message used only in FecSoftware V1_0)
#define FECACKERROR 0x4F00
// Last word of a message in 32 bits (used only in FecSoftware V2_0)
#define FECACKNOERROR32 0xb0
// Status register of the node controller of the CCU for i2c channel (pia ?)
#define CCUFACKERROR 0x00E8
// Bit => Link initialise on the FEC
#define FEC_LINK_INITIALISE 0x0800
// Word to signal that the length is two bytes and not one
#define FEC_LENGTH_2BYTES 0x80
// Upper limit to pass the frame length on two bytes (also used for memory
// and JTAG channel in CCU
#define FEC_UPPERLIMIT_LENGTH 127
// WB  modif
// size of the FIFO receive (in D32 words)
#define RECEIVEFIFODEPTH_V1400 512
#define RECEIVEFIFODEPTH_V1500 768
#define RECEIVEFIFODEPTH_MAX   768
  // size of transmit FIFO  (in D32 words) 
#define TRANSMITFIFODEPTH_V1400 512
#define TRANSMITFIFODEPTH_V1500 256
// size of the FIFO return (in D32 words, only 8 LSB bits valid)
#define RETURNFIFODEPTH_V1400 512
#define RETURNFIFODEPTH_V1500 512
#define RETURNFIFODEPTH_MAX   512



/** ---------------------------------------------------- **/
/** For FEC
 */
#define DEVICEDRIVERNAME "/dev/fecpmc%02d"
#define MAXRING          8
#define MAXCCU           127
#define FRAMEFECNUMBER   0x0
#define MAXFECFIFOWORD   1024 // for firmware 1.5 and FIFO of 32 bits

// Status / Control register correct ?
#define isFecSR0Correct(x) ((((x)&0x0ffff)==0x4C90) || (((x)&0x0ffff)==0x0C90))
#define wordsInReceiveFifo(x) (((x) & 0xffff0000 ) >> 16)  
//#define isFecSR1Correct(x) ((x==0x0) || (x==0x18))
//#define isFecCR0Correct(x) (x==0x1)

// Ring A or B (CR0)
#define isRingInputA(x)  (x & FEC_CR0_SELSERIN)
#define isRingOutputA(x) (x & FEC_CR0_SELSEROUT)

// Clock polarity inverted
#define isInvertClockPolarity(x) (x & FEC_CR0_POLARITY)

  // Internal clock
#define isInternalClock(x) (x & FEC_CR0_XTALCLOCK)

// FIFO receive empty (SR0)
#define isFifoReceiveEmpty(x)  (x & FEC_SR0_RECEMPTY)
#define isFifoTransmitEmpty(x) (x & FEC_SR0_TRAEMPTY)
#define isFifoReturnEmpty(x)   (x & FEC_SR0_RETEMPTY)

// Link initialise (SR0)
#define isLinkInit(x) (x & FEC_SR0_LINKINITIALIZED)

/** ---------------------------------------------------- **/
/** For CCU
 */

// Control register A of the CCU
#define INITCCU25CRA (CCU_CRA_RES | CCU_CRA_CLRE) // Not used for the time being (26/01/2005)
#define INITOLDCCUCRA 0x0F

// CCU Broadcast address
#define BROADCAST_ADDRESS 0x80

/** ---------------------------------------------------- **/
/** For i2c channels
 */
#define MAXI2CCHANNELS 16
#define MAXI2CADDRESS 127

// Force acknowledge bit
#define BITENABLECHANNELCRA 0x80
#define isChannelForceAck(x) (x&I2C_CRA_FACKW)
#define isChannelEnable(x) (x&BITENABLECHANNELCRA)

// Bus I2C speed => See in the makefile
#ifdef I2C1000
#define SPEEDI2C 0x3     // 3 run to 1MHz
#elif I2C400
#define SPEEDI2C 0x2     // 2 run to 400 KHz
#elif I2C200
#define SPEEDI2C 0x1     // 1 run to 200 KHz
#else
#define SPEEDI2C 0x0     // 0 run to 100 KHz
#endif

// Control register A of the I2C channel
#define INITCCU25I2CCRA  (0x20 | SPEEDI2C)   // Enable broadcast operation 
#define INITOLDCCUI2CCRA (0xA0 | SPEEDI2C)   // Enable I2C channel + Enable broadcast operation 

/** ---------------------------------------------------- **/
/** For PIA channels
 * \warning Note the software for the PIA channels on old CCU will not be developped
 */
#define MAXCCUPIACHANNELS     4   // MAX PIA channels
#define NBCCU25PIACHANNELS    4   // Number of PIA channels in the CCU25
#define NBOLDCCUPIACHANNELS   1   // Nubmer of PIA channels in old CCU

#define PIAADDRESSNUMBER   0xFF   // Address on the PIA (not existing but used for compatibility)

// PIA strobe signal on the stobe pad
// If this delay is not defined the strobe signal is not used
// Strobe delay      => bit 0 and bit 1
// STRB              => 1 (bit 3)
#ifdef PIA100
#define PIADURATIONSTROBESIGNAL (0x8 | 0x3)    //  100 ns
#elif PIA200
#define PIADURATIONSTROBESIGNAL (0x8 | 0x2)    //  200 ns
#elif PIA500
#define PIADURATIONSTROBESIGNAL (0x8 | 0x1)    //  500 ns
#elif PIA1000
#define PIADURATIONSTROBESIGNAL (0x8 | 0x0)    // 1000 ns
#else
#define PIADURATIONSTROBESIGNAL 0x0            // Nothing
#endif

// General control register of the PIA channel
// STRBOUTP          => 1 (bit 4) (negative polarity)
// STRB              => 1 (bit 3) (Active on level or transition)
#define INITCCU25PIAGCR  (0x00 | PIADURATIONSTROBESIGNAL)  // Init of the GCR of pia channels
#define INITCCU25PIADDR     0xFF                           // For the Tracker specification
#define INITCCU25PIADATAREG 0xFF                           // For the Tracher specification
#define PIAINITIALVALUE     0xFF                           // Value for the PIA data register

/** ---------------------------------------------------- **/
/** For memory channel
 * The memory channel is separated in two windows (WIN1 and WIN2). Each window
 * can define two separate memory space or an overlap of both.
 * \warning Note the software for the memory channel on old CCU will not be developped
 * \warning the size of a memory frame can be arrived until 0x7FFF. 
 * \warning the memory channel need a patch to the firmware of the FEC
 * \warning This kind of used need can need a change on the driver in order to increate 
 * the performances (see DeviceDriver/include/defines.h - DD_USER_MAX_MSG_LENGTH and
 * recompile all the software ie DeviceDriver, FecSupervisor and tools).
 */

// Number of memory channel
#define NBCCU25MEMORYCHANNELS 1

// The read WIN1H and WIN2L can performed a unwanted parity error
// The multiple write performe an unwanted error on the frame (lcl_err != DD_RETURN_OK)
#define BUGMEMORY2BYTES   // The size must be increased by 2

#define MEMORYADDRESSNUMBER   0xFF   // Address on the Memory (not existing but used for compatibility)

// Control register of the memory (cf. CCU 25 4.6.1)
// Speed of 20 Mhz and enable both windows
#define INITCCU25MEMORYCRA 0xF

// WIN 1H control register or WIN 2H control register
#define MAXMEMORYWINDOWENABLE 0xFFFF
// Due to the limitation of the address, the maximum of a memory is 2^16
#define MAXMEMORYBYTESADDRESS 65536

/** ---------------------------------------------------- **/
/** For trigger channel                                   */

// Number of trigger channel
#define NBCCU25TRIGGERCHANNELS 1
#define TRIGGERCHANNELNUMBER 0x50 // Only availabe for CCU 25

/** ---------------------------------------------------- **/
/** For JTAG channel                                      */

// Number of JTAG channel
#define NBCCU25JTAGCHANNELS 1
#define JTAGCHANNELNUMBER    0x60 // Only availabe for CCU 25

#endif
