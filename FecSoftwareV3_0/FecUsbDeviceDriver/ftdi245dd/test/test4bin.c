
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

FILE *outfile ; 


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
                	printf("loops set to %d sec.\n",turns);
		}
	} 

	fd = open("/dev/ftdi_pio0", O_RDWR) ;

	outfile = fopen("./test4bin.dat","w");
 

	if (fd==-1) { 
			printf("Can not open device.\n"); 
			printf("errno is : %d message : %s\n",errno,strerror(errno));
			return (-1); 
	} 

	outfile = fopen("./test4bin.dat","w");
	if (!outfile) {
                        printf("Can not open bin file.\n");
                        printf("errno is : %d message : %s\n",errno,strerror(errno));
                        return (-2);
        }

	printf("device is openned..\n");
	for(j=0;j<turns;j++) { 
	memset(buffer,0,256);
	result = read(fd,buffer,20);
	if (result>=0) {  
		fwrite(buffer,result,1,outfile);
	} else { 
		printf("read returned: %d \n",result);
		printf("errno is : %d message : %s\n",errno,strerror(errno));
		break ; 
	} 
	}
	sleep(delay); 

	fclose(outfile);
	printf("Now closing device..\n");
	close(fd);
	printf("Done.\n");

	return 0 ; 
}
