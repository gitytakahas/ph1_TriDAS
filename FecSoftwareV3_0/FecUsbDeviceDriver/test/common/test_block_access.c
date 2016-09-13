
#include <sys/time.h>
#include <stdio.h>
#include <assert.h>

#include "fec_usb.h"

typedef unsigned char ubyte ; 
typedef unsigned short uword ; 

#define TESTNUM 20 
#define LOOPNUM 10
#define WORDSPERLOOP 10  
int main(int argc, char* argv[]) { 

  int i,j; int dev = 0;  
  int ftStatus = FECUSB_RETURN_OK;
  long counter = 1000000 ; 
  long loop_num = LOOPNUM ;
  long words_per_loop = WORDSPERLOOP ; 
  char board_serial[256] ;

  struct timeval res[TESTNUM],start[TESTNUM],stop[TESTNUM] ;

  strcpy(board_serial,"");

  if (argc>1) {
     strcpy(board_serial,argv[1]);
     if (strlen(board_serial) == 0) {
        printf("No board serial number.\n") ;
        return 1 ;
     }

  }

  if (argc>2) { 
     loop_num = atol(argv[2]);
     if (loop_num <= 0) { 
	printf("Wrong loop number.\n") ; 
	return 1 ; 
     }
     if (argc>3) { 
     words_per_loop = atol(argv[3]);
     if (words_per_loop <= 0 || words_per_loop >= 2000  ) { 
	printf("Wrong words per loop number.should be >0 and <2000 \n") ; 
	return 1 ; 
     }

     } 
     
  } 

  printf("loops: %ld  words per loop: %ld total words oper. : %ld \n",loop_num,
	 words_per_loop, words_per_loop*loop_num);




   dev = fec_usb_open(board_serial);
   if (dev < FECUSB_RETURN_OK) {
     printf("problems with fec usb device %d open.Status %d \n",i,ftStatus);
     return 1 ; 
   } else {
     printf("Device %d successfuly opened. \n",i) ;
   }



 i = 0 ; 

 fecusbType32 value[10000] ;
 for(i=0;i<10000;i++) value[i] = 0x0001 ; 

 fecusbType32  readvalue[10000]; 
 
 fecusbType32 value2 = 0 ;


 i= 0 ; 
 gettimeofday(start+i,NULL);
  for(j=0;j<loop_num;j++) {	
	  ftStatus = fec_usb_rw_block_dev(dev,2,0x0000,(unsigned long*)readvalue,words_per_loop);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl0 register block read. ftStatus: %d ",ftStatus) ;
	    break ; 
	  } else {
	  }
  }
  gettimeofday(stop+i,NULL);
    timersub(stop+i,start+i,res+i);
    {
    double time_res = (double) res[i].tv_sec + ((double)res[i].tv_usec * 1.0e-6) ;
    time_res /= (double) (loop_num*words_per_loop) ; 
    printf("  read 1 word access time: %7.3lf  msec. \n",1000.0*time_res);  
    } 
  i = 1; 
  gettimeofday(start+i,NULL);
 
  for(j=0;j<loop_num;j++) {	
    //value = 0x0001 ;
	  ftStatus = fec_usb_rw_block_dev(dev,1,0x0000,(unsigned long*)value,words_per_loop);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl0 register write. ftStatus: %d ",ftStatus) ;
	    break ;
	  } else {
	  }
  }
  gettimeofday(stop+i,NULL);
    timersub(stop+i,start+i,res+i);
    {
      double time_res = (double) res[i].tv_sec + (double) res[i].tv_usec * 1.0e-6 ;
       time_res /= (double) (loop_num*words_per_loop) ;
    printf(" write 1 word access time: %7.3lf  msec. \n",1000.0*time_res);       
    }

  printf("new emu methodes\n");
  i = 2 ; 
  ftStatus = fec_usb_set_emulator(dev,1);
  if (ftStatus!=FECUSB_RETURN_OK) { 
	printf("Can not enable emulator mode.\n");
	exit(1); 
	}
{
	int value_emu = 0 ; 
  ftStatus = fec_usb_is_emulator(dev,&value_emu);
  if (ftStatus!=FECUSB_RETURN_OK) {
        printf("Can not get emulator mode.\n");
        exit(1);
        }
  printf("Emulator mode : %d \n",value_emu);
  if (value_emu!=1) { 
	printf("Can not set emulator mode.\n");
	exit (2) ; 
  } 
	
}
 gettimeofday(start+i,NULL);
  for(j=0;j<loop_num;j++) {
          ftStatus = fec_usb_rw_block_dev(dev,4,0x0000,(unsigned long*)value,words_per_loop);
          if (ftStatus!=FECUSB_RETURN_OK) {
            printf("Problems with Ctrl0 register block read. ftStatus: %d ",ftStatus) ;
	    break ; 
          } else {
          }
  }
  gettimeofday(stop+i,NULL);
    timersub(stop+i,start+i,res+i);
    {
    double time_res = (double) res[i].tv_sec + ((double) res[i].tv_usec * 1.0e-6) ;
    time_res /= (double) (loop_num*words_per_loop) ;
    printf("  read 1 word access time: %7.3lf  msec. \n",1000.0*time_res);
    }
    i=3;                                                                       
  gettimeofday(start+i,NULL);
                                                                                
  for(j=0;j<loop_num;j++) {
    //value = 0x0001 ;
          ftStatus = fec_usb_rw_block_dev(dev,3,0x0000,(unsigned long*)value,words_per_loop);
          if (ftStatus!=FECUSB_RETURN_OK) {
            printf("Problems with Ctrl0 register write. ftStatus: %d ",ftStatus) ;
	    break ; 
          } else {
          }
  }
  gettimeofday(stop+i,NULL);
    timersub(stop+i,start+i,res+i);
    {
      double time_res = (double) res[i].tv_sec + (double) res[i].tv_usec * 1.0e-6 ;
       time_res /= (double) (loop_num*words_per_loop) ;
    printf(" write 1 word access time: %7.3lf  msec. \n",1000.0*time_res);                                                                                 
    }

/* 	  usleep(100000) ;  */


/* 	  value = 0x0001 ;  */
/* 	  ftStatus = fec_usb_set_ctrl0(0,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("loaded CR0 with : 0x%04x \n",value); */
/* 	  } 	   */
/* 	  value = 0x03 ;  */
/* 	  ftStatus = fec_usb_set_ctrl1(0,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("loaded CR1 with : 0x%04x \n",value); */
/* 	  } */

/* 	  value = 0x04 ;  */
/* 	  ftStatus = fec_usb_set_ctrl1(0,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("loaded CR1 with : 0x%04x \n",value); */
/* 	  } */
	  

/* 	  value = 0x03 ;  */
/* 	  ftStatus = fec_usb_set_ctrl1(0,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("loaded CR1 with : 0x%04x \n",value); */
/* 	  } */

/* 	  usleep(10000) ;  */


/* 	  ftStatus = fec_usb_get_ctrl0(0,&value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("CR0 is : 0x%04x \n",value); */
/* 	  }  */



/* 	  ftStatus = fec_usb_get_sr0(0,&value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Status0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("SR0 is : 0x%04x \n",value); */
/* 	  }  */
 
/* 	  ftStatus = fec_usb_get_sr1(0,&value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Status0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	    printf("SR1 is : 0x%04x \n",value); */
/* 	  }  */
	
	ftStatus = fec_usb_close(dev) ;
	if(ftStatus!=0) {
	   /* cout << "Device: " << 0 << " closing problems." << endl ;*/
	   printf("Device %d closing problems. \n",0);
	   return 1 ;
	}

/*   printf("Device closed.\n"); */
  return 0 ; 
} 

void some_non_used_function() { 

  fecusbType32 value2 = 0 ;
  fecusbType16 value = 0 ; 
  int ftStatus = 0 ;
  int dev = 0 ; 
  int i ; 

  for(i=0;i<0;i++) { 

	  ftStatus = fec_usb_get_fifotra(0,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with transmit fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	    printf("transmit fifo is : 0x%08x \n",value2);
	  } 
	  ftStatus = fec_usb_get_fiforet(0,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with return fifo register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	    printf("  return fifo is : 0x%08lx \n",value2);
	  } 
	  ftStatus = fec_usb_get_fiforec(0,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with receive fifo register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	    printf(" receive fifo is : 0x%08lx \n",value);
	  } 

  }

} 
