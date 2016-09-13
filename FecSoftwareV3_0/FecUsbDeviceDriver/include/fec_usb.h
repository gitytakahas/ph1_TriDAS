
#ifndef _FECUSB_H
#define _FECUSB_H

#include <stdint.h>

#define FECUSB_TYPE_ERROR int 
#define FECUSB_RETURN_OK 0

#define CREPE_TYPE_ERROR int 
#define CREPE_RETURN_OK 0

#define FECUSB_ERROR_DEVICE_CONTEXT_NOT_NULL -11
#define FECUSB_ERROR_DEVICE_CONTEXT_NULL -12
#define FECUSB_ERROR_DEVICE_CONTEXT_NO_MEMORY -13
#define FECUSB_ERROR_NO_DEVICE_FOUND -14
#define FECUSB_ERROR_OPER_NOT_PERMITTED -5
#define FECUSB_ERROR_DEVICE_ALLREADY_OPENED -6
#define FECUSB_ERROR_TIMEOUT -7
#define FECUSB_ERROR_UNKNOWN -8

/* typedef  unsigned char fecusbType8 ;  */
/* typedef  unsigned short fecusbType16 ;  */
/* typedef  unsigned long fecusbType32 ; */

/* typedef  unsigned char crepeType8 ;  */
/* typedef  unsigned short crepeType16 ;  */
/* typedef  unsigned long crepeType32 ; */

typedef  uint8_t fecusbType8 ; 
typedef  uint16_t fecusbType16 ; 
typedef  uint32_t fecusbType32 ;

typedef  uint8_t crepeType8 ; 
typedef  uint16_t crepeType16 ; 
typedef  uint32_t crepeType32 ;


#ifdef __cplusplus
  extern "C" {
#endif

FECUSB_TYPE_ERROR fec_usb_heartbeat(int dev);

FECUSB_TYPE_ERROR fec_usb_set_latency(int dev,fecusbType8);
FECUSB_TYPE_ERROR fec_usb_set_read_chunk_size(int dev, int val);

FECUSB_TYPE_ERROR fec_usb_set_ctrl0(int, fecusbType16); 
FECUSB_TYPE_ERROR fec_usb_set_ctrl1(int, fecusbType16);
FECUSB_TYPE_ERROR fec_usb_set_control2(int, fecusbType32);
FECUSB_TYPE_ERROR fec_usb_get_ctrl0(int, fecusbType16*); 
FECUSB_TYPE_ERROR fec_usb_get_ctrl1(int, fecusbType16*);
FECUSB_TYPE_ERROR fec_usb_get_control2(int, fecusbType32*);
FECUSB_TYPE_ERROR fec_usb_get_sr0(int, fecusbType32*); 
FECUSB_TYPE_ERROR fec_usb_get_sr1(int, fecusbType16*);

/* new add-on for version readouts */ 

FECUSB_TYPE_ERROR fec_usb_get_version(int dev, fecusbType32 *val);




FECUSB_TYPE_ERROR fec_usb_set_fiforec(int, fecusbType32);
FECUSB_TYPE_ERROR fec_usb_set_fiforet(int, fecusbType32);
FECUSB_TYPE_ERROR fec_usb_set_fifotra(int, fecusbType32);

FECUSB_TYPE_ERROR fec_usb_get_fiforec(int, fecusbType32*);
FECUSB_TYPE_ERROR fec_usb_get_fiforet(int, fecusbType32*);
FECUSB_TYPE_ERROR fec_usb_get_fifotra(int, fecusbType32*);

FECUSB_TYPE_ERROR fec_usb_rw_block_dev(int dev,int cmd,fecusbType16 addr,
                                            fecusbType32 *val,
                                            int count);

FECUSB_TYPE_ERROR  fec_usb_read_raw(int dev, fecusbType8 *val, 
				    int count,
				    int *bytes_read);

FECUSB_TYPE_ERROR  fec_usb_write_raw(int dev, fecusbType8 *val, 
					 int count);

FECUSB_TYPE_ERROR fec_usb_set_block_fiforec(int dev, 
					    fecusbType32 *val,
					    int count);
FECUSB_TYPE_ERROR fec_usb_set_block_fiforet(int dev, 
					    fecusbType32 *val,
					    int count);
FECUSB_TYPE_ERROR fec_usb_set_block_fifotra(int dev, 
				      fecusbType32 *val,
					    int count);
FECUSB_TYPE_ERROR fec_usb_get_block_fiforec(int dev, 
				      fecusbType32 *val,
					    int count);
FECUSB_TYPE_ERROR fec_usb_get_block_fiforet(int dev, 
					    fecusbType32 *val,
					    int count);
FECUSB_TYPE_ERROR fec_usb_get_block_fifotra(int dev, 
					    fecusbType32 *val,
					    int count);
FECUSB_TYPE_ERROR fec_usb_get_serial(int dev,char *val, int buflen);

    CREPE_TYPE_ERROR  crepe_heartbeat(int dev);

    CREPE_TYPE_ERROR  crepe_set_trigger_ram(int dev, crepeType32 *val, int count);
    CREPE_TYPE_ERROR  crepe_get_trigger_ram(int dev, crepeType32 *val, int count);

    CREPE_TYPE_ERROR  crepe_set_control1(int dev, crepeType32 val);
    CREPE_TYPE_ERROR  crepe_set_control2(int dev, crepeType32 val);
    CREPE_TYPE_ERROR  crepe_set_control3(int dev, crepeType32 val);

    CREPE_TYPE_ERROR  crepe_get_control1(int dev, crepeType32 *val);
    CREPE_TYPE_ERROR  crepe_get_control2(int dev, crepeType32 *val);
    CREPE_TYPE_ERROR  crepe_get_control3(int dev, crepeType32 *val);

    CREPE_TYPE_ERROR  crepe_get_version(int dev, crepeType32 *val);
    CREPE_TYPE_ERROR  crepe_get_status(int dev, crepeType32 *val);

    CREPE_TYPE_ERROR  crepe_set_latency(int dev, crepeType8 val);
    CREPE_TYPE_ERROR  crepe_get_latency(int dev, crepeType8 *val);

    CREPE_TYPE_ERROR  crepe_set_trigger_enable_period(int dev, crepeType32 val);
    CREPE_TYPE_ERROR  crepe_get_trigger_enable_period(int dev, crepeType32 *val);    
    
    CREPE_TYPE_ERROR  crepe_set_trigger_disable_period(int dev, crepeType32 val);
    CREPE_TYPE_ERROR  crepe_get_trigger_disable_period(int dev, crepeType32 *val);

    CREPE_TYPE_ERROR  crepe_set_bc0_period(int dev, crepeType32 val);
    CREPE_TYPE_ERROR  crepe_get_bc0_period(int dev, crepeType32 *val);    

    CREPE_TYPE_ERROR  crepe_get_data(int dev, int fifo, crepeType32 *val, int count);    

    CREPE_TYPE_ERROR  crepe_get_event_fifo(int dev, crepeType32 *val, int count);
    CREPE_TYPE_ERROR  crepe_get_bc_fifo(int dev, crepeType32 *val, int count);    

    CREPE_TYPE_ERROR  crepe_set_control_ram(int dev, crepeType32 *val, int count);
    CREPE_TYPE_ERROR  crepe_get_control_ram(int dev, crepeType32 *val, int count);

    CREPE_TYPE_ERROR  crepe_get_access_version(int dev, crepeType32 *val);
    CREPE_TYPE_ERROR  crepe_get_reg_mapping(int dev, crepeType32 *val);
    
    /*FECUSB_TYPE_ERROR fec_usb_open(int devnum, char *serial) ;*/ 
    int fec_usb_open(char *serial) ; 
    FECUSB_TYPE_ERROR fec_usb_close(int devnum) ; 
    FECUSB_TYPE_ERROR fec_usb_is_emulator(int dev, int *emu) ; 
    FECUSB_TYPE_ERROR fec_usb_set_emulator(int dev, int emu) ; 







/* FECUSB_TYPE_ERROR fec_usb_read_write(int devnum, int rw, fecusbType16 address, fecusbType32 *data); */
/* FECUSB_TYPE_ERROR fec_usb_read_write_block(int devnum,  */
/* 					   int rw,  */
/* 					   fecusbType16 address,  */
/* 					   fecusbType32 *data,  */
/* 					   int words); */
/* FECUSB_TYPE_ERROR fec_usb_read_write_emu_block(int devnum, int cmd,  */
/* 				   fecusbType16 address,  */
/* 				   fecusbType32 *data,  */
/* 				   int words); */



#ifdef __cplusplus
  }
#endif



#endif 
