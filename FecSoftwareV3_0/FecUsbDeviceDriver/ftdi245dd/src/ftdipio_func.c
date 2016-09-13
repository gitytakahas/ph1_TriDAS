



#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "ftdipio_ioc.h"

#include "ftdipio_func.h"


int ftdipio_reset(int fd) { 
        int result = ioctl(fd,FTDIPIO_IOC_RESET,NULL);
        if (result) { printf("ftdipio_reset: Problems with reseting device.\n"); }
	return result;
}

int ftdipio_purge(int fd) { 
        int result = ioctl(fd,FTDIPIO_IOC_PURGE_BUFFERS,NULL);
        if (result) { printf("ftdipio_purge: Problems with purging device buffers.\n"); }
	return result ; 
}                

int ftdipio_get_latency_timer(int fd, unsigned char *timer) {
   int result = ioctl(fd,FTDIPIO_IOC_GET_LATENCY_TIMER,timer);
        if (result) { printf("ftdipio_get_latency_timer: Problems with getting latency timer from device.\n"); }
	return result ; 
}                                                    

int ftdipio_set_latency_timer(int fd, unsigned char timer) {
        int result = ioctl(fd,FTDIPIO_IOC_SET_LATENCY_TIMER,&timer);
        if (result) { printf("ftdipio_set_latency_timer: Problems with setting latency timer in device.\n"); }
        return result ; 
}
                                                                                
int ftdipio_open(char *devName) { 
	int retVal = open(devName, O_RDWR| O_NONBLOCK) ;
	if (retVal<0) { printf("ftdipio_open: Problems with device %s openning.\n",devName); }
	return retVal ; 
} 

int ftdipio_close(int fd) {
   int result = close(fd) ; 
        if (result) { printf("ftdipio_close: Problems with closing device.\n"); }
        return result ; 
}

int ftdipio_read(int fd, unsigned char *buffer, int count) {
   int result = 0;

   int maxfd = 0;
   struct timeval tv;
        int n ;
        int retval ;
	fd_set rfds;
	FD_ZERO(&rfds);
        FD_SET(fd,&rfds);
        maxfd = fd + 1 ;
	tv.tv_sec = 0;
        tv.tv_usec = 5000000;
	retval = select(maxfd, &rfds, NULL, NULL,&tv);

        if (retval) {    
	   if(FD_ISSET(fd,&rfds)) {
	      n = read(fd,buffer,512);
	      if(n>=0) {
/* 		 printf("ftdipio_read: %d bytes read.\n",n); */
		 if (n <= 512) {
/* 		    int i ; */
/* 		    for(i=0;i<n;i++) printf(" 0x%02x",(int)buffer[i]); */
/* 		      printf("\n"); */
		    return n ; 
		 } else {
		    printf("SSIZE_MAX too large\n");
		    return n ; 
		 }
	      } else {
		 printf("read returned: %d \n",n);
		 printf("errno is : %d message : %s\n",errno,strerror(errno));
		 return n ; 
	      }
	   }
	} else {
	   printf("ftdipio_read: timeout accured. (5 sec)\n");
	   return (-ETIME);
	}
	


        if (result) { printf("Problems with getting data from device.\n"); }
	return result ; 
}                                                    

int ftdipio_write(int fd, unsigned char *buffer, int count) {
	int result = 0 ; 
/*	int i;*/
/* 	printf("ftdipio_write: %d bytes writing\n",count); */
/*         for(i=0;i<count;i++) printf(" 0x%02x",(int)buffer[i]); */
/*         printf("\n"); */

        result = write(fd,buffer,count);

        if (result!=count) { printf("ftdipio_write: Problems with writing into device.\n"); }
        return result ; 
}
