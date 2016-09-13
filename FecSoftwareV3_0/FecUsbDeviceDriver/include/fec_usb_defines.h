
#ifndef _FECUSB_DEFINES_H
#define _FECUSB_DEFINES_H


/*! 
Fec operations: read  
*/
#define FECUSB_OPER_WRITE 1
#define FECUSB_OPER_READ  2
#define FECUSB_OPER_WRITE_EMU 3
#define FECUSB_OPER_READ_EMU  4
#define FECUSB_OPER_WRITE_EMU_LOCAL 5
#define FECUSB_OPER_READ_EMU_LOCAL  6
#define FECUSB_OPER_WRITE_EMU_LOCAL_INC 7
#define FECUSB_OPER_READ_EMU_LOCAL_INC  8
#define FECUSB_OPER_HEARTBEAT           9
#define FECUSB_TXBUFLENGTH (4*1024*32) 
#define FECUSB_RXBUFLENGTH (4*1024*32) 
#define FECUSB_RETRY 3

        /*!
        Typical offset values for the FEC registers.
	Control 0 
        */
        #define FECUSB_CTRL0_OFFSET                     0x0000
 
        /*!
        Typical offset values for the FEC registers.
        Control 1
        */
        #define FECUSB_CTRL1_OFFSET                     0x0004
 
        /*!
        Typical offset values for the FEC registers.
        Status 0 
        */
        #define FECUSB_STAT0_OFFSET                     0x0008
                                                                                
        /*!
        Typical offset values for the FEC registers.
        Status 1 
        */
        #define FECUSB_STAT1_OFFSET                     0x000c
                                                                                
                                                                                
                                                                                
        /* Field added 17/01/2005 - exists only on 32 bits FECs */
        #define FECUSB_VERSION_OFFSET                   0x0010

        /*!
        Typical offset values for the FEC registers.
        FIFO transmit 
        */
        #define FECUSB_FIFOTRA_OFFSET_UNIQUE    0x0020
                                                                                
        /*!
        Typical offset values for the FEC registers.
        FIFO return 
        */
        #define FECUSB_FIFORET_OFFSET_UNIQUE    0x0024
                                                                                
        /*!
        Typical offset values for the FEC registers.
        FIFO receive
        */
        #define FECUSB_FIFOREC_OFFSET_UNIQUE    0x0028
 

        /*!
        Typical offset values for the FEC registers.
        FIFO transmit 
        */
        #define FECUSB_FIFOTRA_OFFSET_OLD       0x1000
                                                                                
        /*!
        Typical offset values for the FEC registers.
        FIFO return 
        */
        #define FECUSB_FIFORET_OFFSET_OLD       0x2000
                                                                                
        /*!
        Typical offset values for the FEC registers.
        FIFO receive
        */
        #define FECUSB_FIFOREC_OFFSET_OLD       0x3000


#define CREPE_TRIGGER_RAM_OFFSET 0x0000
#define CREPE_CONTROL1_OFFSET 0x0400
#define CREPE_CONTROL2_OFFSET 0x0401
#define CREPE_CONTROL3_OFFSET 0x0402
#define CREPE_VERSION_OFFSET 0x0403
#define CREPE_STATUS_OFFSET 0x0404
#define CREPE_LATENCY_OFFSET 0x0405
#define CREPE_TRIGGER_ENABLE_PERIOD_OFFSET 0x0406
#define CREPE_TRIGGER_DISABLE_PERIOD_OFFSET 0x0407
#define CREPE_BC0_PERIOD_OFFSET 0x0408

#define CREPE_DATA_FIFO_1_OFFSET 0x0420
#define CREPE_DATA_FIFO_2_OFFSET 0x0430
#define CREPE_DATA_FIFO_3_OFFSET 0x0440
#define CREPE_DATA_FIFO_4_OFFSET 0x0450

#define CREPE_DATA_FIFO_LENGTH 2048 

#define CREPE_EVENT_FIFO_OFFSET 0x0460
#define CREPE_EVENT_FIFO_LENGTH 256

#define CREPE_BC_FIFO_OFFSET 0x0470
#define CREPE_BC_FIFO_LENGTH 256
 

#define CREPE_CONTROL_RAM_LENGTH  16 
#define CREPE_TRIGGER_RAM_LENGTH  1024










#endif // _FECUSB_DEFINES_H

