
#include <time.h>
#include <stdio.h>
#include <assert.h>

#include "fec_usb.h"

typedef unsigned char ubyte ; 
typedef unsigned short uword ; 

 
int main() { 

  int i,j,k,id; 
  int ftStatus = FECUSB_RETURN_OK; 
  
  id=0;
/*  for(i=0;i<1;i++) {  */

   ftStatus = fec_usb_open("CREPE007");
   id = ftStatus;
   //if (ftStatus!=FECUSB_RETURN_OK) { 
   if (ftStatus<0){
     printf("problems with fec usb device %d open.Status %d \n",i,ftStatus);
   } else { 
     printf("Device %d successfuly opened. \n",id) ; 
   }

/*  } */

 fecusbType16 value = 0x0080 ;
 fecusbType32 value2 = 0 ;   
	
 for(j=0;j<5;j++) { 
	  ftStatus = fec_usb_get_ctrl0(id,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("CR0 is : 0x%04x \n",value);
	  }

	  ftStatus = fec_usb_get_version(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with version register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("Firmware version is : 0x%04x \n",0x00ffff & value2);
	  }



	  value = 0x2000 ;
	  ftStatus = fec_usb_set_ctrl0(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl0 register write. ftStatus: %d ",ftStatus) ;
	    break ;
	  } else {
	     printf("loaded CR0 with : 0x%04x \n",value);
	  }



	  value = 0x8000 ;
	  ftStatus = fec_usb_set_ctrl0(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;
	    break ;
	  } else {
	     printf("loaded CR0 with : 0x%04x \n",value);
	  }



	  value = 0x4000 ;
	  ftStatus = fec_usb_set_ctrl0(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;
	    break ;
	  } else {
	     printf("loaded CR0 with : 0x%04x \n",value);
	  }




	  value = 0x0080 ;
	  ftStatus = fec_usb_set_ctrl0(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl0 register write. ftStatus: %d ",ftStatus) ;
	    break ;
	  } else {
	     printf("loaded CR0 with : 0x%04x \n",value);
	  }

	  value = 0x0010 ;
	  ftStatus = fec_usb_set_ctrl1(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) {
	    printf("Problems with Ctrl1 register write. ftStatus: %d ",ftStatus) ;
	    break ;
	  } else {
	     printf("loaded CR1 with : 0x%04x \n",value);
	  }

	  



	  // push 3 words to TRA fifo
	  value2 = 0x00010101 ; 
	  ftStatus = fec_usb_set_fifotra(id,value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with tra fifo ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded tra fifo with : 0x%08x \n",value2);
	  } 

	  value2 = 0x00020202 ; 
	  ftStatus = fec_usb_set_fifotra(id,value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with tra fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded tra fifo with : 0x%08x \n",value2);
	  } 	  

	  value2 = 0x00030303 ; 
	  ftStatus = fec_usb_set_fifotra(id,value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with tra fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded tra fifo with : 0x%08x \n",value2);
	  } 


	  // recv fifo 

	  // push 3 words to RECV fifo
	  value2 = 0x00040404 ; 
	  ftStatus = fec_usb_set_fiforec(id,value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with rcv fifo ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded rcv fifo with : 0x%08x \n",value2);
	  } 

	  value2 = 0x00050505 ; 
	  ftStatus = fec_usb_set_fiforec(id,value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with rcv fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded rcv fifo with : 0x%08x \n",value2);
	  } 	  

	  value2 = 0x00060606 ; 
	  ftStatus = fec_usb_set_fiforec(id,value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with rcv fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded rcv fifo with : 0x%08x \n",value2);
	  } 

	  value2 = 0xFFFFFFFF ; 
	  ftStatus = fec_usb_get_fifotra(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with tra fifo ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("got tra fifo : 0x%08x \n",value2);
	  } 

	  value2 = 0xFFFFFFFF ; 
	  ftStatus = fec_usb_get_fifotra(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with tra fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("got tra fifo : 0x%08x \n",value2);
	  } 	  

	  value2 = 0xFFFFFFFF ; 
	  ftStatus = fec_usb_get_fifotra(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with tra fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("got tra fifo : 0x%08x \n",value2);
	  } 

	  value2 = 0xFFFFFFFF ; 
	  ftStatus = fec_usb_get_fifotra(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with tra fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("got tra fifo : 0x%08x \n",value2);
	  } 




	  value2 = 0xFFFFFFFF ; 
	  ftStatus = fec_usb_get_fiforec(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with rec fifo ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("got rcv fifo : 0x%08x \n",value2);
	  } 

	  value2 = 0xFFFFFFFF ; 
	  ftStatus = fec_usb_get_fiforec(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with rcv fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("got rcv fifo : 0x%08x \n",value2);
	  } 	  

	  value2 = 0xFFFFFFFF ; 
	  ftStatus = fec_usb_get_fiforec(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with rcv fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("got rcv fifo : 0x%08x \n",value2);
	  } 

	  value2 = 0xFFFFFFFF ; 
	  ftStatus = fec_usb_get_fiforec(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with rcv fifo read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("got rcv fifo : 0x%08x \n",value2);
	  } 



/* 	  for(k=0;k<8;k++) { */
/* 	  value = 0x0000 | ( 1<<k) ; */
/* 	  ftStatus = fec_usb_set_ctrl0(id,value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) { */
/* 	    printf("Problems with Ctrl0 register write. ftStatus: %d ",ftStatus) ; */
/* 	    break ; */
/* 	  } else {  */
/* 	     printf("loaded CR0 with : 0x%04x \n",value); */
/* 	  }  */

/* 	  usleep(10000) ;  */

/* 	  ftStatus = fec_usb_get_ctrl0(id,&value); */
/* 	  if (ftStatus!=FECUSB_RETURN_OK) {  */
/* 	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ;  */
/* 	    break ;  */
/* 	  } else {  */
/* 	     printf("CR0 is : 0x%04x \n",(unsigned int)value); */
/* 	  } 	   */

/* 	  usleep(10000) ; */


/* 	  } */


	  ftStatus = fec_usb_get_ctrl0(id,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("CR0 is : 0x%04x \n",(unsigned int)value);
	  } 	  

	  ftStatus = fec_usb_get_ctrl1(id,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl1 register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("CR1 is : 0x%04x \n",(unsigned int)value);
	  } 	  


	  
	  value = 0x0080 ; 
	  ftStatus = fec_usb_set_ctrl0(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl0 register write. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded CR0 with : 0x%04x \n",value);
	  } 

	  value = 0x03 ; 
	  ftStatus = fec_usb_set_ctrl1(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl1 register write. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded CR1 with : 0x%04x \n",value);
	  }

	  value = 0x04 ; 
	  ftStatus = fec_usb_set_ctrl1(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl1 register write. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded CR1 with : 0x%04x \n",value);
	  }
	  

	  value = 0x03 ; 
	  ftStatus = fec_usb_set_ctrl1(id,value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("loaded CR1 with : 0x%04x \n",value);
	  }




	  ftStatus = fec_usb_get_ctrl0(id,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl0 register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("CR0 is : 0x%04x \n",value);
	  } 


	  ftStatus = fec_usb_get_ctrl1(id,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Ctrl1 register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("CR1 is : 0x%04x \n",value);
	  } 



	  ftStatus = fec_usb_get_sr0(id,&value2);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Status0 register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	     printf("SR0 is : 0x%04x \n",value2);
	  } 
 
	  ftStatus = fec_usb_get_sr1(id,&value);
	  if (ftStatus!=FECUSB_RETURN_OK) { 
	    printf("Problems with Status0 register read. ftStatus: %d ",ftStatus) ; 
	    break ; 
	  } else { 
	    printf("SR1 is : 0x%04x \n",value);
	  } 
	  printf(" ---  end of loop %d ---------------------------------------------- \n", j);

	  printf("\n");
 } // end of for loop

	ftStatus = fec_usb_close(id) ; 
	if(ftStatus!=0) {
	   /* cout << "Device: " << 0 << " closing problems." << endl ;*/
	   printf("Device %d closing problems. \n",id);   
	   return 1 ; 
	}

  printf("Device closed.\n");
  return 0 ; 
} 

void some_non_used_function() { 

  fecusbType32 value2 = 0 ;
  fecusbType16 value = 0 ; 
  int ftStatus = 0 ;
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
	    printf(" receive fifo is : 0x%08lx \n",value2);
	  } 

  }

} 
