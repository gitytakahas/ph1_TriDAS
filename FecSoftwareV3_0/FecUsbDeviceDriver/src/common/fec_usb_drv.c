

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>

#ifdef FECUSELIBUSB
#include "ftdi.h"
#else 
#include "ftdipio_func.h"
#endif 

#include "fec_usb.h"
#include "fec_usb_defines.h"


#define FECUSB_READ_MAX_RETRY 10
#define FECUSB_MAX_DEV 4 
#define LOOPDELAY 1

struct fec_usb_pnp { 
  fecusbType16 vid ; 
  fecusbType16 pid ; 
  char *description ; 
  char *serial; 
} ; 

#define FECUSB_VALID_DEV 4

struct fec_usb_pnp validDevTable[] = { 
  {0x0403,0x6006,"D2XX Recovery PID for XP",NULL } , 
  {0x0403,0x6006,"FECUSB",NULL },
  {0x1556,0x0168,"CREPE",NULL},
  {0x1556,0x0182,"TTP",NULL}
} ; 

struct devInfo { 
  int desc ; 
#ifdef FECUSELIBUSB
  struct ftdi_context *ftdi;
#endif  
  pthread_mutex_t access_sem ;
  unsigned int counter ; 
  unsigned int version ; 
  unsigned int fifoTransmitAddress ; 
  unsigned int fifoReceiveAddress ;
  unsigned int fifoReturnAddress ;
  int  crepe ;
  int  fec ;
  int  crepe_detection_done ; 
  int  fec_detection_done ; 
  int  access_version; 
  int  reg_mapping; 
  int  fpga_timeout; // in ms
  int  dev;
  char serial[256];
}; 


pthread_mutex_t  dev_table_sem = PTHREAD_MUTEX_INITIALIZER ;

pthread_mutex_t  global_access_sem = PTHREAD_MUTEX_INITIALIZER ;
                                                                                
pthread_mutex_t  initialized_sem = PTHREAD_MUTEX_INITIALIZER ; 

struct devInfo *devtab[ FECUSB_MAX_DEV ] = { NULL, NULL, NULL, NULL }; 

int devnum = 0 ; 

static int initialized = 0 ;

//static bool emulator = false ;   


FECUSB_TYPE_ERROR fec_usb_rw(struct devInfo *ptr, int rw, fecusbType16 address, fecusbType32 *data);
FECUSB_TYPE_ERROR fec_usb_rw_block(struct devInfo *ptr, int rw, fecusbType16 address, fecusbType32 *data, int words);
FECUSB_TYPE_ERROR fec_usb_rw_internal(struct devInfo *ptr, int rw, fecusbType16 address, fecusbType32 *data);
FECUSB_TYPE_ERROR fec_usb_rw_block_internal(struct devInfo *ptr, int rw, fecusbType16 address, fecusbType32 *data, int words);

FECUSB_TYPE_ERROR fec_usb_rw_block_emu_internal(struct devInfo *ptr, int cmd, 
						fecusbType16 address, 
						fecusbType32 *data, int words); 



/* carefull with this function !!!  */ 
/* not compatible with FECUSELIBUSB */
int fec_usb_reinit() { 
  int i ; 

  pthread_mutex_lock(&dev_table_sem);
  for(i=0;i<FECUSB_MAX_DEV;i++) { 

    if (devtab[i]!=NULL) {
      
      int desc = devtab[i]->desc ; 
      close(desc); 
      free(devtab[i]) ; 
      devtab[i] = NULL ; } 

  } 
  pthread_mutex_unlock(&dev_table_sem);
}


FECUSB_TYPE_ERROR fec_usb_is_emulator(int dev, int *emu) { 
  int localVal ; 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ;
  assert(dev<4 && dev>=0); 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
    localVal = ptr->crepe ;
  (*emu) = (localVal) ? 1  : 0 ; 
  retVal = FECUSB_RETURN_OK ; 
  } 
  return retVal ;
} 

FECUSB_TYPE_ERROR fec_usb_is_crepe(int dev, int *emu) { 
  int localVal ; 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ;
  assert(dev<4 && dev>=0); 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
    localVal = ptr->crepe ;
  (*emu) = (localVal) ? 1  : 0 ; 
  retVal = FECUSB_RETURN_OK ; 
  } 
  return retVal ;
} 

FECUSB_TYPE_ERROR fec_usb_set_emulator(int dev, int emu) { 
  int localVal ; 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ;
  assert(dev<4 && dev>=0); 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
   ptr->crepe = emu ;
   retVal = FECUSB_RETURN_OK ; 
  } 
  return retVal ;
} 




int fec_usb_get_max_dev() { return FECUSB_MAX_DEV; } 

int fec_usb_is_initilized() {
  int retVal = 0 ; 
  pthread_mutex_lock(&initialized_sem); 
  retVal = initialized; 
  pthread_mutex_unlock(&initialized_sem);
  return retVal ; 
}

/* FECUSB_TYPE_ERROR fec_usb_open(int dev, char *serial) { */
  
/*   return FECUSB_ERROR_OPER_NOT_PERMITTED;  */

/* } */

int fec_usb_open(char *serial) { 

  int dev = 0;
  int i; unsigned char latency ; int fd = 0 ;
  struct devInfo *ptr ; 
  unsigned char buffer[5] = { 0xff, 0xff, 0xff, 0xff, 0xff } ; 
  FECUSB_TYPE_ERROR status ;
  CREPE_TYPE_ERROR crepe_status;  
  fecusbType32 fec_version ; 
  crepeType32 crepe_version ; 

  assert(dev>=0 && dev<FECUSB_MAX_DEV); 

  pthread_mutex_lock(&dev_table_sem) ;
  for(i=0; i<3; i++){
    if(devtab[i]) {
      if ( strcmp(devtab[i]->serial, serial) == 0) {
	printf(" device  %s has been assinged at dev[%d] \n", serial, i);
	devtab[i]->counter++; 
	pthread_mutex_unlock(&dev_table_sem) ;
	return i;
      }
    }else {
      dev = i;
      break;
    }
  }


/*   if (devtab[dev]) {  */
/*     pthread_mutex_unlock(&dev_table_sem) ; */
/*     return FECUSB_ERROR_DEVICE_ALLREADY_OPENED ;   */
/*   } else {  */

    ptr  = (struct devInfo*) malloc(sizeof(struct devInfo));
    
    if (ptr) {
      
      char devName[256]; 
      memset(ptr,0x00,sizeof(struct devInfo));
#ifdef FECUSELIBUSB 
      ptr->ftdi  = (struct ftdi_context*) malloc(sizeof(struct ftdi_context));
      if (ptr->ftdi == NULL) fd = -1 ; 
      fd = ftdi_init(ptr->ftdi);

      ptr->dev=dev; // added by RSLU, Oct29, 2007
      strcpy(ptr->serial, serial);

      if (fd>-1) { 
	int i; 

	ftdi_read_data_set_chunksize(ptr->ftdi,16384);
	/* defaults */
	ptr->ftdi->usb_read_timeout = 1500 ; /* 1.5 sec */
	ptr->ftdi->usb_write_timeout = 1500 ; /* 1.5 sec */
	ptr->fpga_timeout = 5000 ; /* 5.0 sec */

	for(i=0;i<FECUSB_VALID_DEV;i++) { 

/*   	  printf("Trying vID: 0x%02X  , pID: 0x%02X , serial: %s \n",validDevTable[i].vid,   */
/*   		 validDevTable[i].pid, validDevTable[i].serial);   */
	  fd = ftdi_usb_open(ptr->ftdi,
			     validDevTable[i].vid,
			     validDevTable[i].pid,
			     NULL, serial );
	  
	  /*    	printf("result: %d \n",fd) ;     */
	  if (fd==0) break ; /* if device found go out of search..*/
	}
      } else { 
	printf("Can not initialize ftdi access structure.");
      }

#else 
      sprintf(devName,"/dev/usb/ftdipio%1d",dev);
      fd = ftdipio_open(devName) ;
#endif 	


        if (fd>-1) { /* it is OK */
          int ftStatus = 0 ;
	  int address = 0;

          /* TO DO here: initialization of ftdi245bm device*/
          /* if OK then : */  
	  ptr->desc = fd ; ptr->counter = 0 ;
	  strcpy(ptr->serial, serial);
 	  devtab[dev] = ptr ;  
	  
#ifdef USE_CLAIM
	  printf("multi thread locks enabled.\n"); 
#else
	  printf("WARNING: multi thread locks disabled.\n");
	  ftdi_usb_claim(ptr->ftdi); 
#endif 

	  /* bring down the timeouts, for first access..*/
          ptr->ftdi->usb_read_timeout = 400 ; /* 0.1 sec */
          ptr->ftdi->usb_write_timeout = 400 ; /* 0.1 sec */
          ptr->fpga_timeout = 600 ; /* 0.25 sec */

	  

	  fecusbType32 value2 = 0 ;
	  ftStatus = crepe_get_control2(dev,&value2);

	  if (ftStatus) { 
	    ftStatus = crepe_get_control2(dev,&value2);
	    if (ftStatus) { 
	      ftStatus = crepe_get_control2(dev,&value2);	    
	    }
	  } 

	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Crepe Control 2 register read. ftStatus: %d \n",ftStatus) ;
	    goto error; 
	  } else {
	    printf("Crepe Control 2 register is : 0x%08x . Go for usb setting ...\n",value2);
	  }

	  pthread_mutex_init(&ptr->access_sem,NULL);
	  
	  if ( !(value2&0x00010000) ) { // device was not initialiased
	  /* if (1) { */
	    
#ifdef FECUSELIBUSB
	    ftStatus = ftdi_usb_reset(ptr->ftdi);
	    if (ftStatus<0) printf("ftdi245: %s \n",ptr->ftdi->error_str);
#else
	    ftdipio_reset(fd) ;
#endif 
	    usleep(20000);
#ifdef FECUSELIBUSB
	    ftStatus = ftdi_usb_purge_buffers(ptr->ftdi);
	    if (ftStatus<0) printf("ftdi245: %s \n",ptr->ftdi->error_str);
#else
	    ftdipio_purge(fd) ; 
#endif 
	    usleep(20000);
	    
	    latency = 0 ; 
	    
#ifdef FECUSELIBUSB
	    ftStatus = ftdi_set_latency_timer(ptr->ftdi,5);
	    if (ftStatus<0) printf("ftdi245: %s \n",ptr->ftdi->error_str);
#else 
	    ftdipio_set_latency_timer(fd,2);
#endif 
#ifdef FECUSELIBUSB
	    ftStatus = ftdi_get_latency_timer(ptr->ftdi,&latency);
	    if (ftStatus<0) printf("ftdi245: %s \n",ptr->ftdi->error_str);
#else 
	    ftdipio_get_latency_timer(fd,&latency);
#endif 
	    printf("latency is now set to : %u \n",(unsigned int) latency);
	    
	    value2 |= 0x00010000;
	    ftStatus = crepe_set_control2(dev,value2);
	    if (ftStatus!=FECUSB_RETURN_OK) {
	      printf("Problems with Crepe Control 2 register write. ftStatus: %d ",ftStatus) ;
	    } else {
	      printf("loaded Crepe Control 2 with : 0x%08x \n",value2);
	    }
	    
	  }else {
	    printf(" The CREPE board has been initialized ... \n");
	  }


	  ptr->counter++;


	  crepe_status = crepe_get_version(dev,&crepe_version);
	  if (crepe_status!=CREPE_RETURN_OK) {
	    crepe_status = crepe_get_version(dev,&crepe_version);
	    if (crepe_status!=CREPE_RETURN_OK) {
	      crepe_status = crepe_get_version(dev,&crepe_version);
	    }
	  }

	   if (crepe_status==CREPE_RETURN_OK) { 
	      printf("Crepe version: 0x%04x\n",crepe_version);  
	      ptr->crepe = 1 ; 
	      if ((crepe_version&0x000f)>0x0009) 
		ptr->access_version = 1 ; 
	      else 
		ptr->access_version = 0 ; 
	      printf("packet access version: %d \n",ptr->access_version); 
	      if ((crepe_version&0x00ff)>0x000a) 
		 ptr->reg_mapping = 1 ; 
	      else 
		 ptr->reg_mapping = 0 ; 
	      printf("control register mapping version: %d \n",ptr->reg_mapping);	      
	   } else { 
	     printf("crepe_status: %d \n",crepe_status); 
	      crepe_version = 0x0000 ; 
	      printf("This is not a crepe board.\n");         
	      ptr->crepe = 0 ;
	      ptr->access_version = 0 ; 
	   } 
	        
	   ptr->crepe_detection_done = 1 ; 

	   status = fec_usb_get_version(dev,&fec_version);

	   if (status==FECUSB_RETURN_OK) { 
	      printf("FecCore version: 0x%04x\n",fec_version);  
	      ptr->fec = 1; 
	   } else { 
	      fec_version = 0x0000 ; 
	      printf("This is not a fec usb board.\n"); 
	      ptr->fec = 0 ; 
	   } 

	   ptr->fec_detection_done = 1 ;

	   /* restore default timeouts */
	   ptr->ftdi->usb_read_timeout = 1500 ; /* 1.5 sec */
	   ptr->ftdi->usb_write_timeout = 1500 ; /* 1.5 sec */
	   ptr->fpga_timeout = 5000 ; /* 5.0 sec */	   

	   fec_version &= 0x0000ff00 ; 

/* 	   switch (fec_version) {  */
/* 	   case 0x0300 :  */
/* 	   case 0x1000 :  */
/* 	   case 0x1100 :  */
/* 	      ptr->fifoTransmitAddress = FECUSB_FIFOTRA_OFFSET_UNIQUE ;  */
/* 	      ptr->fifoReceiveAddress = FECUSB_FIFOREC_OFFSET_UNIQUE ; */
/* 	      ptr->fifoReturnAddress = FECUSB_FIFORET_OFFSET_UNIQUE ; */
/* 	      break ;  */
/* 	   case 0xff00 : */
/* 	   default :  */
/* 	      ptr->fifoTransmitAddress = FECUSB_FIFOTRA_OFFSET_OLD ;  */
/* 	      ptr->fifoReceiveAddress = FECUSB_FIFOREC_OFFSET_OLD ; */
/* 	      ptr->fifoReturnAddress = FECUSB_FIFORET_OFFSET_OLD ;  */
	   
/* 	   }  */


	  if (fec_version > 0x0200) { 
	    ptr->fifoTransmitAddress = FECUSB_FIFOTRA_OFFSET_UNIQUE ;
	    ptr->fifoReceiveAddress = FECUSB_FIFOREC_OFFSET_UNIQUE ;
	    ptr->fifoReturnAddress = FECUSB_FIFORET_OFFSET_UNIQUE ; 
	    
	  } else { 
	    ptr->fifoTransmitAddress = FECUSB_FIFOTRA_OFFSET_OLD ;
	    ptr->fifoReceiveAddress = FECUSB_FIFOREC_OFFSET_OLD ;
	    ptr->fifoReturnAddress = FECUSB_FIFORET_OFFSET_OLD ;
	    
	  } 
	  
	  printf("crepe mode: %d \n",ptr->crepe) ;
	  printf("fec   mode: %d \n",ptr->fec) ;
	  
	  printf("fec_usb_open: returning %d\n",FECUSB_RETURN_OK); 
	  printf("fec_usb_open: software ver.  unified %d.%d.%d\n",
		 1,2,2);
	  

/* 	  return FECUSB_RETURN_OK ; /\* OK *\/  */

	  pthread_mutex_unlock(&dev_table_sem);
	  return dev; /* OK */ 
	} else { 
	  
       /*nothing found, free memory : */
	error: 
#ifdef FECUSELIBUSB 
	  if (ptr->ftdi) ftdi_deinit(ptr->ftdi);
	  if (ptr->ftdi) free(ptr->ftdi) ; 
	  ptr->ftdi = NULL ; 
#endif 

         free(ptr) ; 
         pthread_mutex_unlock(&dev_table_sem);
         return FECUSB_ERROR_NO_DEVICE_FOUND ;
	}
    } else { 
      free(ptr) ; 
      pthread_mutex_unlock(&dev_table_sem);
      return FECUSB_ERROR_DEVICE_CONTEXT_NO_MEMORY ;
    }    
    //} /* else else from if (devtab[dev]!=NULL...*/
  
  /* should be never reached...*/

    //return FECUSB_RETURN_OK ; 
    return dev;

}

FECUSB_TYPE_ERROR fec_usb_close(int dev) { 

  struct devInfo *ptr = NULL ; 

  assert(dev>=0 && dev<FECUSB_MAX_DEV);  

  pthread_mutex_lock(&dev_table_sem) ; 

  ptr = devtab[dev] ; 

  if (ptr==NULL) { 
    pthread_mutex_unlock(&dev_table_sem) ;
    return FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  } else {
    if (--(ptr->counter) >  0) {
      pthread_mutex_unlock(&dev_table_sem) ;
      /*printf("not real close, counter after -- is : %d \n",ptr->counter) ; */
    } else { /* real close */
      /*printf("real close, counter after -- is : %d \n",ptr->counter) ;*/
#ifdef FECUSELIBUSB
      if (ptr->ftdi) { 
	/*printf("real close, destroying ftdi context.\n") ;*/
#ifdef USE_CLAIM 
#else
	  ftdi_usb_release(ptr->ftdi); 
#endif
	ftdi_usb_close(ptr->ftdi);
	ftdi_deinit(ptr->ftdi);
	free (ptr->ftdi); 
	ptr->ftdi = NULL; 
/* 	printf("done.\n") ; */
      }      
#else 
    int fd  = ptr->desc ; 
    close(fd);  
#endif 
/*     printf("real close, destroying device entry in the context table.\n") ; */

    free(ptr) ; 
    devtab[dev] = NULL ; 
/*     printf("done.\n") ; */

    }
    pthread_mutex_unlock(&dev_table_sem) ;
    return FECUSB_RETURN_OK ; 
  }
  /* should be never reached: */
  
  return FECUSB_RETURN_OK ;
} 

FECUSB_TYPE_ERROR fec_usb_set_latency(int dev,fecusbType8 val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  fecusbType32 localVal = (fecusbType32) val ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) { 
#ifdef FECUSELIBUSB
	  retVal = ftdi_set_latency_timer(ptr->ftdi,val);
	  if (retVal<0) printf("ftdi245: %s \n",ptr->ftdi->error_str);
#else 
	  retVal = ftdipio_set_latency_timer(fd,val);
#endif
  }


  return retVal ; 

}
FECUSB_TYPE_ERROR fec_usb_set_read_chunk_size(int dev, int val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  fecusbType32 localVal = (fecusbType32) val ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) { 
#ifdef FECUSELIBUSB
	  retVal = ftdi_read_data_set_chunksize(ptr->ftdi,val);
	  if (retVal<0) printf("ftdi245: %s \n",ptr->ftdi->error_str);
#else 
	  //not implemented : do nothing... 
	  //retVal = ftdipio_set_latency_timer(fd,val);
#endif
  }


  return retVal ; 

} 
// ftdi_read_data_set_chunksize 




//new unique transfer function


FECUSB_TYPE_ERROR fec_usb_rw_unified(struct devInfo *ptr, int cmd, 
				     fecusbType16 address, 
				     fecusbType32 *data, int words ) {


        int  ft_status = 0 ;
        int  dwBytesToWrite = 0 ;
        unsigned char buffer[FECUSB_TXBUFLENGTH];
        unsigned char rxbuffer[FECUSB_RXBUFLENGTH];
	int debug = 0 ;
	int a_lire = 0  ; 
	int rx_count = 0 ; 
	int i,j ; 

	long delayCount ;
	int  index = 0 ; 
	unsigned char *tmp = NULL ; 

	int paddle_with_ff = 0 ; 
	struct timeval start_time, stop_time, diff_time ;
	int cmd_local ; 
 
  assert(ptr!=NULL);


  if (!(words>0)) return FECUSB_ERROR_OPER_NOT_PERMITTED ; 
  if (data==NULL) return FECUSB_ERROR_OPER_NOT_PERMITTED ;



  /* if it is a crepe*/
      if (ptr->crepe || !ptr->crepe_detection_done) { 
	 cmd_local =  cmd ; /* by default is the same*/
	 if (cmd==FECUSB_OPER_WRITE) { 
	   cmd_local = FECUSB_OPER_WRITE_EMU ; /* for crepe: we profit for new access type*/
	 } 
	 
	 if (cmd==FECUSB_OPER_READ) { 
	    cmd_local = FECUSB_OPER_READ_EMU ; /* for crepe: we profit for new access type*/
	 } 	 
      
      }
     else { 

       if (ptr->fec || !ptr->fec_detection_done) { 

       cmd_local = cmd ; /* by default is the same*/
         if (cmd==FECUSB_OPER_WRITE_EMU) { 
	   cmd_local = FECUSB_OPER_WRITE ; /* no crepe:  old access type*/
	 } 
	 
	 if (cmd==FECUSB_OPER_READ_EMU) { 
	    cmd_local = FECUSB_OPER_READ ; /* no crepe: old access type*/
	 } 

	 if (cmd_local==FECUSB_OPER_WRITE || cmd_local==FECUSB_OPER_READ) {
	   /* ok, do nothing*/
	 } else  { /* we are not crepe but request was for crepe cmd.. and not fec access*/
		return  FECUSB_ERROR_OPER_NOT_PERMITTED ;
	}

       } else { 
	 return  FECUSB_ERROR_OPER_NOT_PERMITTED ;
       } 
     }
  
  

  /* assembling the packet frame */ 
  if (cmd_local==FECUSB_OPER_WRITE || cmd_local==FECUSB_OPER_READ) { 
    /* treat like collated old block transfer.. */
    for(j=0;j<(words);j++) { 

        buffer[index+0] = 0;  // sync byte
        buffer[index+2] = cmd_local;  //cmd  == 1  -> write , == 2 => read
        buffer[index+3] = address & 0xff;
        buffer[index+4] = (address & 0xff00) >> 8;
 
        switch (cmd_local) {
        case FECUSB_OPER_WRITE :
          buffer[index+1] = 9 ; // writing
          buffer[index+5] = 0;  //address
          buffer[index+6] = 0;  //address
          buffer[index+7] = (*(data+j)) & 0xff ;
          buffer[index+8] = ((*(data+j)) & 0xff00) >> 8;
          buffer[index+9] = ((*(data+j)) & 0xff0000) >> 16 ;
          buffer[index+10] = ((*(data+j)) & 0xff000000) >> 24 ; // data
          dwBytesToWrite += 11 ;
	  index += 11 ; 
          break ;
        case FECUSB_OPER_READ :
          dwBytesToWrite += 7;    // reading
          buffer[index+1] = 5;
	  buffer[index+5] = 0;  //address
          buffer[index+6] = 0;  //address
	  index += 7 ; 
          break ;
        default : return FECUSB_ERROR_OPER_NOT_PERMITTED ;
 
        }

    }
    a_lire = (4*words); /* old methode: reply is always words*4 bytes */
  } else { 
    /* use new emualtor/crepe access */
        buffer[index+0] = 0;  // sync byte
        buffer[index+2] = cmd_local;  //cmd  == 1  -> write , == 2 => read
        buffer[index+3] = address & 0xff;
        buffer[index+4] = (address & 0xff00) >> 8;

 
        switch (cmd_local) {
	case FECUSB_OPER_HEARTBEAT:
        case FECUSB_OPER_WRITE_EMU :
	case FECUSB_OPER_WRITE_EMU_LOCAL:
	case FECUSB_OPER_WRITE_EMU_LOCAL_INC:

	   a_lire = (ptr->access_version>0) ? 4 : (4*words) ; 
	   /* answer for write is 1 word only for crepe > 0x2008*/
	 
          buffer[index+1] = 9; // writing
          buffer[index+5] = 0;  //address
          buffer[index+6] = 0;  //address
	  index += 7 ; 
	  dwBytesToWrite += 7 ;
	  buffer[index++] = ( words & 0x00ff) ; 
	  buffer[index++] = (words & 0xff00) >> 8 ; 
	  dwBytesToWrite += 2 ;

	  for(j=0;j<words;j++) { 
          buffer[index++] = (*(data+j)) & 0xff ;
          buffer[index++] = ((*(data+j)) & 0xff00) >> 8;
          buffer[index++] = ((*(data+j)) & 0xff0000) >> 16 ;
          buffer[index++] = ((*(data+j)) & 0xff000000) >> 24 ; // data
	  dwBytesToWrite += 4 ;
	  }

 
          break ;
        case FECUSB_OPER_READ_EMU :
        case FECUSB_OPER_READ_EMU_LOCAL:
        case FECUSB_OPER_READ_EMU_LOCAL_INC:
	   a_lire = (words*4); /* answer is words*4 bytes long*/
          dwBytesToWrite += 7;    // reading
          buffer[index+1] = 5;
	  buffer[index+5] = 0;  //address
          buffer[index+6] = 0;  //address
	  index += 7 ;
	  buffer[index++] = ( words & 0x00ff) ; 
	  buffer[index++] = (words & 0xff00) >> 8 ; 
	  dwBytesToWrite += 2 ;	  
          break ;
        default : return FECUSB_ERROR_OPER_NOT_PERMITTED ;
 
        }

  }

	if (paddle_with_ff) { 

	  int bursts = dwBytesToWrite / 64 ; 
	  int rest = dwBytesToWrite % 64 ; 
	  for (i=0;i<64-rest;i++) { 
	    if (index >= FECUSB_TXBUFLENGTH) break ; 
	    buffer[index++] = 0xff ; 
	  } 
	  dwBytesToWrite = index ;

	} 


    	if (debug) { 
	   printf("sending: ") ; 
	   for(i=0;i<dwBytesToWrite;i++) 
	      printf(" 0x%02x",buffer[i]); 
	   printf("\n");
	}


#ifdef USE_CLAIM                                    
	/* sending request */
	// try 300 times within 3 seconds
	int fd = -1; int jj;
	for (jj=0; jj<300; jj++ ) {
	  //claim usb - RSLU Oct 24, 2007  
	  int dev = ptr->dev;

	  fd = ftdi_usb_claim(ptr->ftdi);

	  //usleep(10000);
	  if (fd==0) break;
	}
	if ( jj != 0 ) {
	  printf( "   ************* had to try %d times to open usb for rw \n ", jj );
	}
	//---
#endif


#ifdef FECUSELIBUSB
	ft_status = ftdi_write_data(ptr->ftdi,buffer,dwBytesToWrite);
#else
	ft_status = ftdipio_write(ptr->desc,buffer,dwBytesToWrite); 
#endif 
	if (ft_status<0) {
		 printf("fec_usb_rw_unified: direct usb error in write: %d \n",ft_status);
		 printf("while sending: ") ; 
		 for(i=0;i<dwBytesToWrite;i++) 
		    printf(" 0x%02x",buffer[i]); 
		 printf("\n");
#ifdef USE_CLAIM
 		 ftdi_usb_release(ptr->ftdi);  //RSLU Oct 24 2007 
#endif
		 return ft_status ;
	}

	if (ft_status!=dwBytesToWrite) {
	   if (debug) { 
	      printf("Problems in write. %d bytes\n",ft_status);
	      printf("fec_usb_rw_unified: write done. %d/%d \n",ft_status,dwBytesToWrite);
 
	   }
#ifdef USE_CLAIM
 	   ftdi_usb_release(ptr->ftdi);  //RSLU Oct 24 2007 
#endif
	   return ft_status ; 
	}


 
	// testing performance with padding 0xff

	if (cmd==FECUSB_OPER_HEARTBEAT) a_lire = 62 ; 

        tmp = rxbuffer  ;
	rx_count = 0 ; 
	gettimeofday(&start_time,NULL);
	do { 


	  // give a breath to the cpu... 
	  sched_yield();
	  gettimeofday(&stop_time,NULL);
	  timersub(&stop_time,&start_time,&diff_time);
	  /*timeout break: */

	  if ((diff_time.tv_sec * 1000 + diff_time.tv_usec/1000) > ptr->fpga_timeout) { 
	   printf("fpga read: timeout occurred. (%d msec)\n",ptr->fpga_timeout);
	   printf("sent cmd: ") ; 
	     for(i=0;i<dwBytesToWrite;i++) 
	      printf(" 0x%02x",buffer[i]); 
	    printf("\n");
	    printf("recv: ") ; 
	    for(i=0;i<(rx_count);i++) 
	      printf(" 0x%02x",rxbuffer[i]); 
	    printf("\n");	   
#ifdef USE_CLAIM
	    ftdi_usb_release(ptr->ftdi);  //RSLU Oct 24 2007 
#endif
	    return (-ETIME);
	  }



	   if (debug) printf("fec_usb_rw_unified: about to read.\n");
#ifdef FECUSELIBUSB
	   ft_status = ftdi_read_data(ptr->ftdi,tmp,a_lire);
#else
	   ft_status = ftdipio_read(ptr->desc,tmp,a_lire);
#endif 

	   if (ft_status<0) { 
	      printf("fec_usb_rw_unified: direct usb error in read: %d \n",ft_status);
	      printf("sent cmd: ") ; 
	      for(i=0;i<dwBytesToWrite;i++) 
		 printf(" 0x%02x",buffer[i]); 
	      printf("\n");
	      printf("recv: ") ; 
	      for(i=0;i<(rx_count);i++) 
		 printf(" 0x%02x",rxbuffer[i]); 
	      printf("\n");
#ifdef USE_CLAIM
 	      ftdi_usb_release(ptr->ftdi);  //RSLU Oct 24 2007 
#endif
	      return ft_status ; 
	   }



	   if (debug) {
	      if (ft_status!=a_lire) 
		 printf("fec_usb_rw_unified: problems in read. %d bytes\n",ft_status);
	      printf("fec_usb_rw_unified: read done. %d/%d \n",ft_status,a_lire);
	   }

	   a_lire -= ft_status ; 
	   tmp += ft_status ;
	   rx_count += ft_status; 

	   

	} while (a_lire>0) ; 

        switch (cmd_local) {
	case FECUSB_OPER_HEARTBEAT:
        case FECUSB_OPER_WRITE_EMU :
	case FECUSB_OPER_WRITE_EMU_LOCAL:
	case FECUSB_OPER_WRITE_EMU_LOCAL_INC: 

	  if (ptr->access_version>0) { 
/* 	    unsigned long readData = ( */
/* 					    rxbuffer[3] */
/* 					    + ( rxbuffer[2] << 8) */
/* 					    + ( rxbuffer[1] << 16) */
/* 					    + ( rxbuffer[0] << 24) */
/* 					    ) ; */
/* 	    if (readData==0xf000)  */
/* 	      return  FECUSB_RETURN_OK; */
/* 	    else  */
/* 	      return  -FECUSB_ERROR_UNKNOWN; */
#ifdef USE_CLAIM
 	    ftdi_usb_release(ptr->ftdi);  //RSLU Oct 24 2007 
#endif
	    return  FECUSB_RETURN_OK;
	  }; 
	  
	default : 
	 
	{

	   index = 0 ; 

	   for(j=0;j<(rx_count>>2); j++ ) { 
                                                                                
	      (*(data+j)) = (unsigned long)(
					    rxbuffer[index+3]
					    + ( rxbuffer[index+2] << 8)
					    + ( rxbuffer[index+1] << 16)
					    + ( rxbuffer[index+0] << 24)
					    ) ;

	      index += 4 ; 

	   } 

	   if (debug) { 
	      printf("recv: ") ; 
	      for(i=0;i<(rx_count);i++) 
		 printf(" 0x%02x",rxbuffer[i]); 
	      printf("\n");
	   }

	}; 
	}

	

/* 	{/\* see if there is something left in ftdi chip ...*\/ */
/* 	   do {   */
/* 	      a_lire = 384 ;  */
/* 	      tmp = rxbuffer ; */
/* 	      rx_count = 0 ;  */
/* #ifdef FECUSELIBUSB */
/* 	   ft_status = ftdi_read_data(ptr->ftdi,tmp,a_lire); */
/* #else */
/* 	   ft_status = ftdipio_read(ptr->desc,tmp,a_lire); */
/* #endif */
/* 	   if (ft_status>0) {  */
/* 	      printf("extra recv: ") ;  */
/* 	      for(i=0;i<(ft_status);i++)  */
/* 		 printf(" 0x%02x",rxbuffer[i]);  */
/* 	      printf("\n"); */

/* 	   }  */
	      

/* 	   } while (ft_status>0);  */
	   
/* 	} */

/* 	{/\* purge ftdi buffers *\/ */

/* #ifdef FECUSELIBUSB */
/* 	  ft_status = ftdi_usb_purge_buffers(ptr->ftdi); */
/* 	  if (ft_status<0) printf("fec_usb_rw_unified: ftdi245: %s \n",ptr->ftdi->error_str); */
/* #else */
/* 	  ftdipio_purge(fd) ;  */
/* #endif	    */


/* 	} */

#ifdef USE_CLAIM
 	ftdi_usb_release(ptr->ftdi);  //RSLU Oct 24 2007 
#endif
	return FECUSB_RETURN_OK ; 
       

} 



// end of unified function

FECUSB_TYPE_ERROR  fec_usb_write_raw(int dev, fecusbType8 *val, int count) { 
        int  ft_status = 0 ;
	int  debug = 0 ; 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;

  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) { 
	/* sending request */
#ifdef FECUSELIBUSB
	ft_status = ftdi_write_data(ptr->ftdi,val,count);
#else
	ft_status = ftdipio_write(ptr->desc,val,count); 
#endif 
	if (ft_status<0) {
	  int i; 
		 printf("crepe_write_raw: direct usb error in write: %d \n",ft_status);
		 printf("while sending: ") ; 
		 for(i=0;i<count;i++) 
		    printf(" 0x%02x",val[i]); 
		 printf("\n");
		 return (FECUSB_TYPE_ERROR) ft_status ;
	}

	if (ft_status!=count) {
	   if (debug) { 
	      printf("Problems in write. %d bytes\n",ft_status);
	      printf("fec_write_raw: write done. %d/%d \n",ft_status,count);
	   }
	   return (FECUSB_TYPE_ERROR) ft_status ; 
	}
	return FECUSB_RETURN_OK;
  }
  return retVal ; 

} 

FECUSB_TYPE_ERROR  fec_usb_read_raw(int dev, fecusbType8 *val, 
				    int count,
				    int *bytes_read) { 
        int  ft_status = 0 ;
	int  debug = 0 ; 
	struct timeval start_time, stop_time, diff_time ;
	int a_lire = 0  ;
	int rx_count = 0 ;
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;

  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL && bytes_read) { 

	rx_count = 0 ; 
	a_lire = count ; 
	gettimeofday(&start_time,NULL);
	do { 

	  int i;
	   // give a breath to the cpu... 
	  sched_yield();
	  gettimeofday(&stop_time,NULL);
	  timersub(&stop_time,&start_time,&diff_time);
	  /*timeout break: */
	  if (diff_time.tv_sec > 5) { 
	   printf("ftdipio_read: timeout occurred. (2 sec)\n");
	   printf("recv: ") ; 
	      for(i=0;i<(rx_count);i++) 
		 printf(" 0x%02x",val[i]); 
	      printf("\n");	   
	   
	   return (-ETIME);
	  }

	   if (debug) printf("fec_usb_read_raw: about to read.\n");
#ifdef FECUSELIBUSB
	   ft_status = ftdi_read_data(ptr->ftdi,val,a_lire);
#else
	   ft_status = ftdipio_read(ptr->desc,val,a_lire);
#endif 

	   if (ft_status<0) { 
	      printf("fec_usb_read_raw: direct usb error in read: %d \n",ft_status);

	      printf("recv: ") ; 
	      for(i=0;i<(rx_count);i++) 
		 printf(" 0x%02x",val[i]); 
	      printf("\n");
	      return (FECUSB_TYPE_ERROR) ft_status ; 
	   }

	   if (debug) {
	      if (ft_status!=a_lire) 
		 printf("fec_usb_read_raw: problems in read. %d bytes\n",ft_status);
	      printf("fec_usb_read_raw: read done. %d/%d \n",ft_status,a_lire);
	   }

	   a_lire -= ft_status ; 
	   val += ft_status ;
	   rx_count += ft_status; 

	   

	} while (a_lire>0) ;

	(*bytes_read) = rx_count; 

	return (FECUSB_TYPE_ERROR) FECUSB_RETURN_OK ; 


  }
  return retVal ; 

} 





//new cmd function:
FECUSB_TYPE_ERROR fec_usb_rw(struct devInfo *ptr, int cmd, 
				   fecusbType16 address, 
				   fecusbType32 *data) 
{
   int i ; 
   FECUSB_TYPE_ERROR status = FECUSB_RETURN_OK ; 

      status = fec_usb_rw_unified(ptr,cmd,address,data,1);
       

   return status;
}
FECUSB_TYPE_ERROR fec_usb_rw_block(struct devInfo *ptr, int cmd, 
				   fecusbType16 address, 
				   fecusbType32 *data, 
				   int words) 
{
  
   FECUSB_TYPE_ERROR status = FECUSB_RETURN_OK;
   status = fec_usb_rw_unified(ptr,cmd,address,data,words);
   return status;
}

/* FECUSB_TYPE_ERROR fec_usb_heartbeat(int dev) {  */
/*   FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; */
/*   fecusbType32 localVal ; */
/*   struct devInfo *ptr = NULL ;  */
/*   ptr = devtab[dev] ; */
/*   if (ptr!=NULL)  */
/*      retVal = fec_usb_rw(ptr,FECUSB_OPER_HEARTBEAT,FECUSB_CTRL0_OFFSET,&localVal);  */
/*   return retVal ;  */
/* }   */

 


FECUSB_TYPE_ERROR fec_usb_set_ctrl0(int dev, fecusbType16 val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  fecusbType32 localVal = (fecusbType32) val ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw(ptr,FECUSB_OPER_WRITE,FECUSB_CTRL0_OFFSET,&localVal); 
  return retVal ; 
} 

FECUSB_TYPE_ERROR fec_usb_set_ctrl1(int dev, fecusbType16 val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  fecusbType32 localVal = (fecusbType32) val ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL)  
     retVal = fec_usb_rw(ptr,FECUSB_OPER_WRITE,FECUSB_CTRL1_OFFSET,&localVal); 
  return retVal ; 

} 


FECUSB_TYPE_ERROR fec_usb_get_ctrl0(int dev, fecusbType16 *val ) { 
  fecusbType32 localVal ; 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw(ptr,FECUSB_OPER_READ,FECUSB_CTRL0_OFFSET,&localVal);

  if (retVal==FECUSB_RETURN_OK) { 
    (*val) = (fecusbType32) ( localVal ) ; 
  } 
  return retVal ;

} 

FECUSB_TYPE_ERROR fec_usb_get_ctrl1(int dev, fecusbType16 *val) { 
  fecusbType32 localVal ; 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw(ptr,FECUSB_OPER_READ,FECUSB_CTRL1_OFFSET,&localVal);

  if (retVal==FECUSB_RETURN_OK) { 
    (*val) = (fecusbType32) ( localVal ) ; 
  } 
  return retVal ;

} 


FECUSB_TYPE_ERROR fec_usb_get_sr0(int dev, fecusbType32 *val) { 
  fecusbType32 localVal ; 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;

  if (ptr!=NULL)  
     retVal = fec_usb_rw(ptr,FECUSB_OPER_READ,FECUSB_STAT0_OFFSET,&localVal);
  if (retVal==FECUSB_RETURN_OK) { 
    (*val) = (fecusbType32) ( localVal ) ; 
  }

  return retVal ;

} 
 
FECUSB_TYPE_ERROR fec_usb_get_sr1(int dev, fecusbType16 *val) { 

  fecusbType32 localVal ; 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;

  if (ptr!=NULL) 
     retVal = fec_usb_rw(ptr,FECUSB_OPER_READ,FECUSB_STAT1_OFFSET,&localVal);

  if (retVal==FECUSB_RETURN_OK) { 
    (*val) = (fecusbType32) ( localVal ) ; 
  } 


  return retVal ;
} 


FECUSB_TYPE_ERROR fec_usb_set_fiforec(int dev, fecusbType32 val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  fecusbType32 localVal = val ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw(ptr,FECUSB_OPER_WRITE,ptr->fifoReceiveAddress,&localVal); 
  return retVal ; 
} 

FECUSB_TYPE_ERROR fec_usb_set_fiforet(int dev, fecusbType32 val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  fecusbType32 localVal =  val ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw(ptr,FECUSB_OPER_WRITE,ptr->fifoReturnAddress,&localVal); 
  return retVal ; 

} 

FECUSB_TYPE_ERROR fec_usb_set_fifotra(int dev, fecusbType32 val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  fecusbType32 localVal = val ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;  
  if (ptr!=NULL)  
     retVal = fec_usb_rw(ptr,FECUSB_OPER_WRITE,ptr->fifoTransmitAddress,&localVal); 
  return retVal ; 
} 

FECUSB_TYPE_ERROR fec_usb_get_fiforec(int dev, fecusbType32 *val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL)
     retVal = fec_usb_rw(ptr,FECUSB_OPER_READ,ptr->fifoReceiveAddress,val); 
  return retVal ;
} 


FECUSB_TYPE_ERROR fec_usb_get_fiforet(int dev, fecusbType32 *val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL)
     retVal = fec_usb_rw(ptr,FECUSB_OPER_READ,ptr->fifoReturnAddress,val); 
  return retVal ;
} 

FECUSB_TYPE_ERROR fec_usb_get_fifotra(int dev, fecusbType32 *val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL)  
     retVal = fec_usb_rw(ptr,FECUSB_OPER_READ,ptr->fifoTransmitAddress,val); 
  return retVal ;
} 

/* new add-on for version readouts */ 


FECUSB_TYPE_ERROR fec_usb_get_version(int dev, fecusbType32 *val) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL)   
     retVal = fec_usb_rw(ptr,FECUSB_OPER_READ,FECUSB_VERSION_OFFSET,val); 
  return retVal ;
} 

/* test block transfer */
FECUSB_TYPE_ERROR fec_usb_rw_block_dev(int dev,int cmd,fecusbType16 addr,
                                            fecusbType32 *val,
                                            int count) {
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  struct devInfo *ptr = NULL ;
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,cmd,
                               addr,
                               val,count);
  }
  return retVal ;
}







/* new add-on for block trasfers */

FECUSB_TYPE_ERROR fec_usb_set_block_fiforec(int dev, 
					    fecusbType32 *val,
					    int count) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE,
			       ptr->fifoReceiveAddress,
			       val,count); 
  }
  return retVal ; 
} 

FECUSB_TYPE_ERROR fec_usb_set_block_fiforet(int dev, 
					    fecusbType32 *val,
					    int count) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL)
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE,
			       ptr->fifoReturnAddress,
			       val,count); 
  return retVal ; 

} 

FECUSB_TYPE_ERROR fec_usb_set_block_fifotra(int dev, 
				      fecusbType32 *val,
				      int count) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE,
			       ptr->fifoTransmitAddress,
			       val,count); 
  return retVal ; 
} 

FECUSB_TYPE_ERROR fec_usb_get_block_fiforec(int dev, 
				      fecusbType32 *val,
				      int count) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ,
			       ptr->fifoReceiveAddress,
			       val,count); 
  return retVal ;
} 


FECUSB_TYPE_ERROR fec_usb_get_block_fiforet(int dev, 
					    fecusbType32 *val,
					    int count) { 
  FECUSB_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ,
			       ptr->fifoReturnAddress,
			       val,count); 
  return retVal ;
} 

FECUSB_TYPE_ERROR fec_usb_get_block_fifotra(int dev, 
					    fecusbType32 *val,
					    int count) { 
  FECUSB_TYPE_ERROR retVal =  FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) 
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ,
			       ptr->fifoTransmitAddress,
			       val,count); 
  return retVal ;
} 

FECUSB_TYPE_ERROR fec_usb_get_serial(int dev,
                                     char *val, int buflen) {
  FECUSB_TYPE_ERROR retVal =  FECUSB_ERROR_DEVICE_CONTEXT_NULL ;
  struct devInfo *ptr = NULL ;
  ptr = devtab[dev] ;
  if (ptr!=NULL) { 
     retVal = ftdi_get_serial_string(ptr->ftdi, val, buflen);
  }
/*   printf(" in fec_usb_get_serial, val = %s \n", val); */
  return retVal ;
}



CREPE_TYPE_ERROR  crepe_heartbeat(int dev) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  crepeType32 val[1] = { 0x0 } ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_HEARTBEAT,
			       0x00,
			       val,1); 
  }
  return retVal ; 
}

CREPE_TYPE_ERROR  crepe_set_trigger_ram(int dev, crepeType32 *val, int count) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (count>CREPE_TRIGGER_RAM_LENGTH) return FECUSB_ERROR_OPER_NOT_PERMITTED; 
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL_INC,
			       CREPE_TRIGGER_RAM_OFFSET,
			       val,count); 
  }
  return retVal ; 
}

CREPE_TYPE_ERROR  crepe_get_trigger_ram(int dev, crepeType32 *val, int count) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (count>CREPE_TRIGGER_RAM_LENGTH) return FECUSB_ERROR_OPER_NOT_PERMITTED;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL_INC,
			       CREPE_TRIGGER_RAM_OFFSET,
			       val,count); 
  }
  return retVal ;
}

CREPE_TYPE_ERROR  crepe_set_control1(int dev, crepeType32 val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL,
			       CREPE_CONTROL1_OFFSET,
			       &val,1); 
  }
  return retVal ;
} 

CREPE_TYPE_ERROR  crepe_set_control2(int dev, crepeType32 val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL,
			       CREPE_CONTROL2_OFFSET,
			       &val,1); 
  }
  return retVal ;


} 
CREPE_TYPE_ERROR  crepe_set_control3(int dev, crepeType32 val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL,
			       CREPE_CONTROL3_OFFSET,
			       &val,1); 
  }
  return retVal ;
}

CREPE_TYPE_ERROR  crepe_get_control1(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_CONTROL1_OFFSET,
			       val,1); 
  }
  return retVal ;
}
CREPE_TYPE_ERROR  crepe_get_control2(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_CONTROL2_OFFSET,
			       val,1); 
  }
  return retVal ;
}
CREPE_TYPE_ERROR  crepe_get_control3(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_CONTROL3_OFFSET,
			       val,1); 
  }
  return retVal ;
}


CREPE_TYPE_ERROR  crepe_get_version(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_VERSION_OFFSET,
			       val,1); 
  }
  return retVal ;
}

CREPE_TYPE_ERROR  crepe_get_status(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_STATUS_OFFSET,
			       val,1); 
  }
  return retVal ;
}

CREPE_TYPE_ERROR  crepe_set_latency(int dev, crepeType8 val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  crepeType32 localVal = val ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL,
			       CREPE_STATUS_OFFSET,
			       &localVal,1); 
  }
  return retVal ;
}
CREPE_TYPE_ERROR  crepe_get_latency(int dev, crepeType8 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  crepeType32 localVal = 0x00 ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_STATUS_OFFSET,
			       &localVal,1);
     (*val) = (crepeType8) (0x000000ff & localVal) ; 
  }
  return retVal ;
}

CREPE_TYPE_ERROR  crepe_set_trigger_enable_period(int dev, crepeType32 val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL,
			       CREPE_TRIGGER_ENABLE_PERIOD_OFFSET,
			       &val,1); 
  }
  return retVal ;
}

CREPE_TYPE_ERROR  crepe_get_trigger_enable_period(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_TRIGGER_ENABLE_PERIOD_OFFSET,
			       val,1); 
  }
  return retVal ;
}   
    
CREPE_TYPE_ERROR  crepe_set_trigger_disable_period(int dev, crepeType32 val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL,
			       CREPE_TRIGGER_DISABLE_PERIOD_OFFSET,
			       &val,1); 
  }
  return retVal ;
} 
CREPE_TYPE_ERROR  crepe_get_trigger_disable_period(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_TRIGGER_DISABLE_PERIOD_OFFSET,
			       val,1); 
  }
  return retVal ;
}

CREPE_TYPE_ERROR  crepe_set_bc0_period(int dev, crepeType32 val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL,
			       CREPE_BC0_PERIOD_OFFSET,
			       &val,1); 
  }
  return retVal ;
} 

CREPE_TYPE_ERROR  crepe_get_bc0_period(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_BC0_PERIOD_OFFSET,
			       val,1); 
  }
  return retVal ;
} 

  

CREPE_TYPE_ERROR  crepe_get_data(int dev, int fifo, crepeType32 *val, int count) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  //assert(fifo>=0 && fifo<4);
  if (fifo>=0 && fifo<4) return  -FECUSB_ERROR_OPER_NOT_PERMITTED ;
  if (count>CREPE_DATA_FIFO_LENGTH) return FECUSB_ERROR_OPER_NOT_PERMITTED ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_DATA_FIFO_1_OFFSET+fifo*0x10,
			       val,count); 
  }
  return retVal ;

}






CREPE_TYPE_ERROR  crepe_get_event_fifo(int dev, crepeType32 *val, int count) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (count>CREPE_EVENT_FIFO_LENGTH) return FECUSB_ERROR_OPER_NOT_PERMITTED ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_EVENT_FIFO_OFFSET,
			       val,count); 
  }
  return retVal ;   
} 
CREPE_TYPE_ERROR  crepe_get_bc_fifo(int dev, crepeType32 *val, int count) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (count>CREPE_BC_FIFO_LENGTH) return FECUSB_ERROR_OPER_NOT_PERMITTED ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL,
			       CREPE_BC_FIFO_OFFSET,
			       val,count); 
  }
  return retVal ;  
}     
 
CREPE_TYPE_ERROR  crepe_set_control_ram(int dev, crepeType32 *val, int count) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  if (count>CREPE_CONTROL_RAM_LENGTH) return FECUSB_ERROR_OPER_NOT_PERMITTED ;  
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_WRITE_EMU_LOCAL_INC,
			       CREPE_CONTROL1_OFFSET,
			       val,count); 
  }
  return retVal ; 
}

CREPE_TYPE_ERROR  crepe_get_control_ram(int dev, crepeType32 *val, int count) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ;
  if (count>CREPE_CONTROL_RAM_LENGTH) return FECUSB_ERROR_OPER_NOT_PERMITTED ;  
  ptr = devtab[dev] ;
  if (ptr!=NULL) {
     retVal = fec_usb_rw_block(ptr,FECUSB_OPER_READ_EMU_LOCAL_INC,
			       CREPE_CONTROL1_OFFSET,
			       val,count); 
  }
  return retVal ;
}

CREPE_TYPE_ERROR  crepe_get_access_version(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL && val!=NULL) {

     (*val) = (crepeType32) ptr->access_version ; 

     retVal = CREPE_RETURN_OK;  
  }
  return retVal ;

}


CREPE_TYPE_ERROR  crepe_get_reg_mapping(int dev, crepeType32 *val) { 
  CREPE_TYPE_ERROR retVal = FECUSB_ERROR_DEVICE_CONTEXT_NULL ; 
  struct devInfo *ptr = NULL ; 
  ptr = devtab[dev] ;
  if (ptr!=NULL) {

     (*val) = (crepeType32) ptr->reg_mapping ; 

     retVal = CREPE_RETURN_OK; 
  }
  return retVal ;
}

   
/* these function are CC of libusb 0.1.7 original functions , as ver. 0.1.6 
does not implement them */

int usb_get_string(usb_dev_handle *dev, int index, int langid, char *buf, size_t buflen)
{
  return usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR,
                        (USB_DT_STRING << 8) + index, langid, buf, buflen, 1000);
}


int usb_get_string_simple(usb_dev_handle *dev, int index, char *buf, size_t buflen)
{
  char tbuf[256];
  int ret, langid, si, di;
 
  ret = usb_get_string(dev, index, 0, tbuf, sizeof(tbuf));
  if (ret < 0)
    return ret;
 
  if (ret < 4)
    return -EIO;
 
  langid = tbuf[2] | (tbuf[3] << 8);
 
  ret = usb_get_string(dev, index, langid, tbuf, sizeof(tbuf));
  if (ret < 0)
    return ret;
 
  for (di = 0, si = 2; si < ret; si += 2) {
    if (di >= (buflen - 1))
      break;
 
    if (tbuf[si + 1])   /* high byte */
      buf[di++] = '?';
    else
      buf[di++] = tbuf[si];
  }
                                                                                
  buf[di] = 0;
                                                                                
  return di;
}
