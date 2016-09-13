
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

int run = 1 ; 

int main() { 
   int fd = -1 ; 
   unsigned long result = 0; 
   int i ; 
   unsigned char buffer[256];
   unsigned char dbuf[512];
   int maxfd = 0; 
   struct timeval tv;
   	int n ; 
	int retval ;


   // unsigned char dbuf[512] ;
   fd_set rfds;
   FD_ZERO(&rfds);
   retval = 0 ;

	fd = open("/dev/usb/ftdipio0", O_RDWR | O_NONBLOCK) ; 

	if (fd==-1) { 
			printf("Can not open device.\n"); 
			printf("errno is : %d message : %s\n",errno,strerror(errno));
			return (-1); 
	} 


	printf("device opened\n") ; 
	/* result = read(fd,buffer,2);
	if (result<0) printf("Problems reading err: %d - %s \n",result,strerror(errno));
	printf("received: %d bytes\n",result); 
	for(i=0; i<result; i++ ) 
	   printf(" 0x%2x",(int)buffer[i]);
	   printf("\n");*/

	/*while(run)*/
	  { /* while run active */ 
           FD_ZERO(&rfds);
           FD_SET(fd,&rfds);
                                                                                
           maxfd = fd + 1 ;
	   /* Wait up to five seconds. */
           tv.tv_sec = 5;
           tv.tv_usec = 0;
           retval = select(maxfd, &rfds, NULL, NULL,&tv);
                                                                                
           //retval = 0 ;
           //this->pause();
                                                                                
           if (retval) {
                                                                                
                if(FD_ISSET(fd,&rfds)) {
                        n = read(fd,dbuf,512);
			if(n>=0) { 
		          printf("%d bytes read.\n",n);	
                          if (n <= 512) {
                             /*for(i=0;i<n;i++) printf(" 0x%2x",(int)dbuf[i]);
			      printf("\n");*/
                          } else {
			      printf("SSIZE_MAX too large\n");
                          }
			} else { 
				printf("read returned: %d \n",n);
				printf("errno is : %d message : %s\n",errno,strerror(errno));
			}
                        }
	   } else { 
	     printf("Time out accured.\n");
	   } 
	  } /* select statement block */




	printf("Now closing device..\n");
	close(fd);
	printf("Done.\n");

	return 0 ; 
}
