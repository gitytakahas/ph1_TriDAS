
#include <sys/time.h>
#include <stdio.h>
#include <assert.h>

#include "fec_usb.h"

typedef unsigned char ubyte ; 
typedef unsigned short uword ; 

#define TESTNUM 20 

#define LOOPNUM 10

int main(int argc, char *argv[]) { 

  int i,j,dev = 0 ; 
  int ftStatus = FECUSB_RETURN_OK;
  long counter = 1000000 ; 
  long loop_num = LOOPNUM ; 
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

  } 




  printf("loop number: %ld\n",loop_num) ;

   dev = fec_usb_open(board_serial);
   if (dev < FECUSB_RETURN_OK) {
     printf("problems with fec usb open device: %s .Status %d \n",board_serial,ftStatus);
   } else {
     printf("Device : %s successfuly opened. \n",board_serial) ;
   }

 i = 0 ; 

 fecusbType16 value = 0 ;
 fecusbType32 value2 = 0 ;

 gettimeofday(start+i,NULL);
 

  for(j=0;j<loop_num;j++) {	
	
	  ftStatus = fec_usb_get_ctrl0(dev,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;
	     
	  } else {
	     
	  }

  }

  gettimeofday(stop+i,NULL);

    timersub(stop+i,start+i,res+i);
    {
        

    double time_res = (double) res[i].tv_sec + (res[i].tv_usec * 1.0e-6) ;

    time_res /= (double) loop_num ; 

    printf("  read ctrl0 access time: %5.1lf  msec. \n",1000.0*time_res);  
    }

  gettimeofday(start+i,NULL);
 
  for(j=0;j<loop_num;j++) {	

	  value = 0x0001 ;
	  ftStatus = fec_usb_set_ctrl0(dev,value);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;
	    
	  } else {
	     
	  }

  }

  gettimeofday(stop+i,NULL);

    timersub(stop+i,start+i,res+i);

    {
      double time_res = (double) res[i].tv_sec + res[i].tv_usec * 1.0e-6 ;
       time_res /= loop_num ; 
    printf(" write ctrl0 access time: %5.1lf  msec. \n",1000.0*time_res);       
   
    }

/* 	  usleep(100000) ;  */


/* 	  value = 0x0001 ;  */
/* 	  ftStatus = fec_usb_set_ctrl0(dev,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("loaded CR0 with : 0x%04x \n",value); */
/* 	  } 	   */
/* 	  value = 0x03 ;  */
/* 	  ftStatus = fec_usb_set_ctrl1(dev,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("loaded CR1 with : 0x%04x \n",value); */
/* 	  } */

/* 	  value = 0x04 ;  */
/* 	  ftStatus = fec_usb_set_ctrl1(dev,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("loaded CR1 with : 0x%04x \n",value); */
/* 	  } */
	  

/* 	  value = 0x03 ;  */
/* 	  ftStatus = fec_usb_set_ctrl1(dev,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("loaded CR1 with : 0x%04x \n",value); */
/* 	  } */

/* 	  usleep(10000) ;  */


/* 	  ftStatus = fec_usb_get_ctrl0(dev,&value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("CR0 is : 0x%04x \n",value); */
/* 	  }  */



/* 	  ftStatus = fec_usb_get_sr0(dev,&value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Status0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	     printf("SR0 is : 0x%04x \n",value); */
/* 	  }  */
 
/* 	  ftStatus = fec_usb_get_sr1(dev,&value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Status0 register read. ftStatus: %d ",ftStatus) ;  */
	     
/* 	  } else {  */
/* 	    printf("SR1 is : 0x%04x \n",value); */
/* 	  }  */
	
	ftStatus = fec_usb_close(dev) ;
	if(ftStatus!=0) {
	   /* cout << "Device: " << 0 << " closing problems." << endl ;*/
	   printf("Device %d closing problems. \n",dev);
	   return 1 ;
	}

/*   printf("Device closed.\n"); */
  return 0 ; 
} 

void some_non_used_function() { 

  fecusbType32 value2 = 0 ;
  fecusbType16 value = 0 ; 
  int ftStatus = 0 ;
  int dev =0; 
  int i ; 

  for(i=0;i<0;i++) { 

	  ftStatus = fec_usb_get_fifotra(dev,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with transmit fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	    printf("transmit fifo is : 0x%08x \n",value2);
	  } 
	  ftStatus = fec_usb_get_fiforet(dev,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with return fifo register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	    printf("  return fifo is : 0x%08lx \n",value2);
	  } 
	  ftStatus = fec_usb_get_fiforec(dev,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with receive fifo register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	    printf(" receive fifo is : 0x%08lx \n",value);
	  } 

  }

} 
