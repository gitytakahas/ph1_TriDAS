
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
unsigned long usb_old_rw(int fd, int rw, long address, long data)
{
        //int a;
        int canal=0;
        unsigned long  ft_status=0;
        unsigned long byte_ecrit=0;
        unsigned long  dwBytesToWrite=0; //dwByteToRead;
    //  LPDWORD lpdwBytesWritten;
        //LPVOID lpBuffer;
        unsigned char buffer[80];
        unsigned char RxBuffer[80];
        unsigned long BytesReceived=0,a_lire=0;
        unsigned long  txBytes=0xffff,rxBytes=0xffff,eventWord=0xffff;
  
    canal = 1;
        memset(buffer,0,sizeof(buffer)); 
	memset(RxBuffer,0,sizeof(RxBuffer));                                                                        
        buffer[0] = 0;  // sync byte
//      buffer[1] = 9; // nombre de byte
                                                                                
        buffer[2] = rw;  //canal  == 1  -> ecriture
        buffer[3] = address & 0xff; buffer[4] = (address & 0xff00) >> 8;
//address
        buffer[5] = 0; buffer[6] = 0;  //address
        buffer[7] = data & 0xff ; buffer[8] = (data &0xff00) >> 8;
        buffer[9] = (data & 0xff0000) >> 16 ; buffer[10] = (data & 0xff000000) >> 24 ; // data
                                                                                
        if(rw == 1) {
                        dwBytesToWrite = 11;   // ecriture
                        buffer[1] = 9;
        }
        if(rw == 2) {
                        dwBytesToWrite = 5;    // lecture
                        buffer[1] = 3;
        }
        printf("usb_old_rw: about to write.\n");
 

        ft_status = write(fd,buffer,dwBytesToWrite);
        if (ft_status != dwBytesToWrite) printf("Problems in write. %d - %s\n",
						ft_status,strerror(errno));
 
        printf("usb_old_rw: write done. %d/%d bytes \n",ft_status,dwBytesToWrite);
                                                                                
// lecture



    a_lire = 4;
        printf("usb_old_rw: about to read.\n");
        ft_status = read(fd,RxBuffer,a_lire);
	if(ft_status != a_lire) printf("Problems in read. %d - %s\n",
						ft_status,strerror(errno));
        printf("usb_old_rw: read done. %d/%d \n",ft_status,a_lire);
                                                                                
	/*printf("Status bytes: 0x%2x 0x%2x \n",(int)RxBuffer[0],(int)RxBuffer[1]); */                                                     
                                                                                
                                                                                
    return (unsigned long)(RxBuffer[3] + ( RxBuffer[2] << 8) + ( RxBuffer[1] <<
16) + (RxBuffer[0] << 24));
                                                                                
}



int main() { 
   int fd = -1 ; 
   long result = 0; 
   int i ; 
   unsigned char buffer[256];
	fd = open("/dev/ftdi_pio0", O_RDWR) ; 

	if (fd==-1) { 
			printf("Can not open device.\n"); 
			printf("errno is : %d message : %s\n",errno,strerror(errno));
			return (-1); 
	} 
	/* result = read(fd,buffer,2);
	if (result<0) printf("Problems reading err: %d - %s \n",result,strerror(errno));
	printf("received: %d bytes\n",result); 
	for(i=0; i<result; i++ ) 
	   printf(" 0x%2x",(int)buffer[i]);
	   printf("\n");*/
	/*for(i=0;i<10;i++) { 
	result = read(fd,buffer,256);
	if (result<0) break ; 
	}*/
	for(i=0;i<500;i++) { 
	result = usb_old_rw(fd,2,8,1);
	printf("CR0 result is : %x \n",result);
	result = usb_old_rw(fd,2,0xc,1);
	printf("CR1 result is : %x \n",result);
	}
	result = read(fd,buffer,4);
	if (result<0) printf("Problems reading err: %d - %s \n",result,strerror(errno));
	printf("received: %d bytes\n",result); 
	for(i=0; i<result; i++ ) 
	   printf(" 0x%2x",(int)buffer[i]);
	printf("\n");


	printf("Now closing device..\n");
	close(fd);
	printf("Done.\n");

	return 0 ; 
}
