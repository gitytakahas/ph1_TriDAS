
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "fec_usb.h"

typedef unsigned char ubyte ; 
typedef unsigned short uword ; 

 
int main() { 

 
 int status ; unsigned short setup_value ;
 int i;  char buffer[1024*1024*4] ; 
 unsigned long recv_counter,send_counter,garbage_counter ; 
 int chunk = 4096 ; int ftStatus ; 



 int dev;
 ftStatus = fec_usb_open("CREPE007");
 dev = ftStatus;
 //if (ftStatus!=FECUSB_RETURN_OK){//  && ftStatus != 6) {
 if (ftStatus<0){//  && ftStatus != 6) {
   printf("problems with fec usb device open.Status %d \n",ftStatus);
   return 1 ;
 } ;
 printf("Device %d successfuly opened. \n", dev) ; 


 fecusbType32 value = 0 ; 
	
	for(i=0;i<1;i++) { 
	  ftStatus = fec_usb_get_sr0(dev,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Status0 register read. ftStatus: %d ",ftStatus) ; 
	  } else { 
	    printf("SR0 is : 0x%04x \n",value);
	  } 

 	  //sleep(15); 

	  /* cout << "result of SR0 is : 0x" << hex << result << dec << endl ;*/ 
	  ftStatus = fec_usb_get_sr1(dev,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Status0 register read. ftStatus: %d ",ftStatus) ; 
	  } else { 
	    printf("SR1 is : 0x%04x \n",value);
	  } 
	}

/* 	printf(" ---  test another USB open routine --- \n " ); */
/* 	int ftStatus1 = fec_usb_open("CREPE008"); */
/* 	if (ftStatus1 < 0) { */
/* 	  printf("problems with another fec usb device open.Status %d \n",ftStatus1); */
/* 	  fec_usb_close(dev); */
/* 	  return 0; */
/* 	} ; */
/* 	printf(" -------------------------------------- \n " ); */


  ftStatus = fec_usb_close(dev) ; 
  if(ftStatus!=0) {
    /* cout << "Device: " << 0 << " closing problems." << endl ;*/
    printf("Device %d closing problems. \n",dev);   
    return 1 ; 
  }

 printf("Device closed.\n");
 
 

  

  return 0 ; 
} 

