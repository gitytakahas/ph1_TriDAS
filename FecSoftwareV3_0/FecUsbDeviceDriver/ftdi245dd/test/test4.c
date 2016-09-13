
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc,char *argv[]) { 
   int fd = -1 ; 
   long result = 0; 
   int i,j ; 
   unsigned char buffer[256];
   int delay = 5 ;int turns = 1 ;  

	if(argc>1) { 
		delay = atoi(argv[1]);
		printf("delay set to %d sec.\n",delay);
		if(argc>2) {
                	turns = atoi(argv[2]);
                	printf("loops set to %d .\n",turns);
		}
	} 

	fd = open("/dev/ftdi_pio0", O_RDWR) ; 

	if (fd==-1) { 
			printf("Can not open device.\n"); 
			printf("errno is : %d message : %s\n",errno,strerror(errno));
			return (-1); 
	} 
	printf("device is openned..\n");
	for(j=0;j<turns;j++) { 
	memset(buffer,0,256);
	result = read(fd,buffer,20);
	printf("rs: %d ",result) ;
	if (result>=0) {  
		for(i=0;i<result;i++) printf(" %2X",(int)buffer[i]);
		printf("\n");
	} else { 
	  printf("errno is : %d message : %s\n",errno,strerror(errno));
		break ; 
	} 
	}
	sleep(delay); 


	printf("Now closing device..\n");
	close(fd);
	printf("Done.\n");

	return 0 ; 
}
