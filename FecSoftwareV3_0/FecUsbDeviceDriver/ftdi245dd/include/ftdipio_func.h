

#ifndef _FTDIPIO_FUNC_H
#define _FTDIPIO_FUNC_H


int ftdipio_reset(int fd); 
int ftdipio_purge(int fd); 
int ftdipio_get_latency_timer(int fd, unsigned char *timer);
int ftdipio_set_latency_timer(int fd, unsigned char timer);
int ftdipio_open(char *devName);
int ftdipio_close(int fd); 
int ftdipio_read(int fd, unsigned char *buffer, int count);
int ftdipio_write(int fd, unsigned char *buffer, int count);


#endif 
