#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "TShare.h"

# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/sem.h>
# include <errno.h>

#include <iostream>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
        int val;                    /* value for SETVAL */
        struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array for GETALL, SETALL */
        struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

void clrshm(int ie,void *arg)
{
    int *c = (int*) arg;int ic=*c;
    
    printf("Clear Share I got an exit %d with %d as arg in Pid %d \n",ie,ic,getpid());
      struct shmid_ds *shm_ds=0;
      if (shmctl(ic, IPC_RMID, shm_ds) <0)
        {
          printf("Y a un problem \n");
          perror("shmctl");
        }
}

//void rmsem(int ie,void *arg)
//{
 //   int *c = (int*) arg;int ic=*c;
 //   printf("Rem Sem I got an exit %d with %d as arg \n",ie,ic);
 //     struct semid_ds *shm_ds;
 //     if (semctl(ic, 0,IPC_RMID, 0) <0)
 //       {
 //         printf("Y a un problem \n");
 //         perror("semctl");
 ////       }
//}


TShare::TShare(char *cdev,int type,int size) throw (std::string)
{
  fSize=size;
  memset(fName,0,256);
  strncpy(fName,cdev,strlen(cdev));

  isServer = (type == TShare_CREATE);
  printf("Shar memory name %s \n",fName);
  printf("Shar memory name %d %d %d \n",type,TShare_CREATE,isServer);
  if (isServer)
   {
     printf("Files Creation \n");
     char cmd[256];
     sprintf(cmd,"touch /tmp/%s.shm",fName);
     system(cmd);
     sprintf(cmd,"touch /tmp/%s.sem",fName);
     system(cmd);
   }
  sprintf(fShmName,"/tmp/%s.shm",fName);
  sprintf(fSemName,"/tmp/%s.sem",fName);
  /* Get a key from the file shm_devname */
  if((fShmkey = ftok(fShmName, '\x01')) <0)
    {
      throw std::string("TShare: ftok problem");

    }
  int flag = (isServer)?IPC_CREAT:0;
  flag |=SHM_W|SHM_R;
  if((fShmid = shmget( fShmkey, fSize, flag)) <0)
    {
      printf("TShare: shmget problem : %d %d\n", errno,flag);
      perror("TShare: shmget");
      throw std::string("TShare: shmget problem");
    }
  //  if (isServer) on_exit(clrshm,(void*)&fShmid);
  /* Get a key from the file shm_devname */
  if((fSemkey = ftok(fSemName, '\x01')) <0)
    {
      perror("TShare: ftok problem");

      throw std::string("TShare: ftok problem");
    }

  if((fSemid = semget( fSemkey, 1, flag)) <0)
    {
      printf("TShare: semget problem : %d %d\n", errno,flag);
      perror("TShare: semget");

      throw std::string("TShare: shmget problem 2nd ");
    }
  if (isServer) 
    {
      printf("TShare : lock the semaphore\n");
      (fSembuf[0]).sem_num = 0;
      (fSembuf[0]).sem_op = 1;
      (fSembuf[0]).sem_flg = 0;

      if((semop( fSemid, fSembuf, 1) <0))
        {
          perror("TShare: semop problem");

	  throw std::string("TShare: semop problem");
        }

      //  on_exit(rmsem,(void*)&fSemid);
    }
} 
TShare::~TShare() throw (std::string)
{
  if (isServer)
    {
      struct shmid_ds *shm_ds=0;
      if (shmctl(fShmid, IPC_RMID, shm_ds) <0)
        {
          perror("shmctl");

	  throw std::string("TShare: shmctl problem");
        }
      //struct semid_ds *sem_ds;
      if (semctl(fSemid, 0,IPC_RMID, 0) <0)
        {
          perror("semctl");
	  throw std::string("TShare: shmctl problem");
        }
    }


}

void TShare::Lock() throw (std::string)
{
  int nretry = 0;
 start:
  bool retry = (nretry>1);
  (fSembuf[0]).sem_num = 0;
  (fSembuf[0]).sem_op = -1;
  (fSembuf[0]).sem_flg = 0;
  
  if((semop( fSemid, fSembuf, 1) <0))
    {
      perror("TShare: Lock problem");
      printf("Error code : %d \n",errno);
      if (retry)       
	throw std::string("TShare: lock problem");
      usleep(1);
      nretry++;
      goto start;
    }
}
void TShare::Unlock() throw (std::string)
{
  union semun arg={0};
  int nretry=0;

 start:
  bool retry = (nretry>1);
  (fSembuf[0]).sem_num = 0;
  (fSembuf[0]).sem_op = 1;
  (fSembuf[0]).sem_flg = 0;

 
  if((semop( fSemid, fSembuf, 1) <0))
    {
      perror("TShare: Unlock problem");
     printf("Error code : %d \n",errno);
     semctl(fSemid,0,GETALL,arg);
     printf("Semaphore value %d \n",arg.array[0]);
     arg.array[0] =0;
     semctl(fSemid,0,SETALL,arg);
      if (retry)     throw std::string("TShare: Unlock problem");   
      usleep(1);
      nretry++;
      goto start;


    }

}
void* TShare::Attach() throw (std::string)
{
  
  void* Shared = shmat(fShmid, 0, 0);
  void* sserror = (void*) -1;
  //printf("Attach %x \n",Shared);
  std::cout << "Attach " << fShmid<<" to " <<std::hex << Shared << std::dec << std::endl ;
  if(sserror == Shared){
    perror("Error in TShare::Attach() --> ");
    throw std::string("TShare::Attach() shmat problem found ");
  }
  return Shared;
}

void TShare::Detach(const void* ad) throw (std::string)
{
  
 int err= shmdt(ad);
 if (err == -1)
   {
    perror("Error in TShare::Detach() --> ");
    throw std::string("TShare::Detach(const void* ) shmdt problem found ");
   }
}
