#ifndef TShare_INCLUDE
#define TShare_INCLUDE
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string>
/**
 \enum TShareType
 */
enum TShareType { 
  TShare_CREATE, ///< Create the Share Memory
  TShare_OPEN    ///< Open the Share Memory 
};

/**
   \class TShare
   \author L.Mirabito
   \date March 2001
   \version 1.0
   \brief <b> Essential </b>. It is the main communication class

   <h2> Usage </h2>
   The TShare class implements both a Share memory definition and access and
   a semaphore to access it.
   <p> The memory is create by the following code:
   <pre>
   TShare *ms = new TShare("MyShare",TShare_CREATE,1000);
   </pre>
   <p> It then can be access and locked:
   <pre>
    // Mapp the meory to a given class
    MyClass* mc = (MyClass*) ms->Attach();
    // Lock the access with a semaphore
    ms->Lock();
    // access data (fantaisy)
    My_Histo_Fill(mc->getData());
    mc->setDataStatus(0);
    // Unlock access for other user
    ms->Unlock();
   </pre>
   <p> Finally access to the Share memory should be released
   <pre>
   ms->Detach(mc);
   </pre>
   <h5>note</h5> 
   Two empty files are created in /tmp by TShare: \\
    name.sem and name.shm 
   <h5>Warning</h5> 

   The Share memory and the semaphore are still registered in the system even 
   when the Object is destroyed. This is wanted to avoid problem with forked
   processes. Please clean all share memories and semaphores with script 
   clearshm you find in Daq/Acquisition/bin/clearshm
   <pre>
   #!/bin/csh
   ipcs -s | grep `whoami` | awk '{if ($6 != "0") print "ipcrm sem " $2}' >/tmp/tt.
   $$
   ipcs -m | grep `whoami` | awk '{if ($6 == "0") print "ipcrm shm " $2}' >>/tmp/tt
   .$$
   source /tmp/tt.$$
   rm /tmp/tt.$$
   ipcs -a

   </pre>

 */
class TShare {
private:
  char fName[256]; ///< Name
  char fShmName[256]; ///< Share memory file name
  char fSemName[256]; ///< Semaphore  file name
  key_t fShmkey; ///< Share memory key
  key_t fSemkey; ///< Semaphore key
  int fShmid; ///< Share memory Id 
  int fSemid; ///< Semaphore id
  int fSize; ///< Sahre memory size

  struct sembuf fSembuf[1]; 
 protected:
  bool isServer; ///< True if open in create mode
public:
  /**
     Create a TShare object:
     @param name Share memory name
     @param mode TShare_CREATE or TShare_OPEN (see TShare.h)
     @param size Share memory size
   */
  TShare(char* name,int mode,int size) throw (std::string);
  ~TShare() throw (std::string);
  /** Lock the Semaphore to access safely the Share memory */
  void Lock() throw (std::string);
  /** Unlock the Semaphore  */
  void Unlock() throw (std::string);
  /** Attach the Share memory 
      @return pointer to the Share memory
   */ 
  void* Attach() throw (std::string);
  /** Detach the Share memory 
      @param ad  pointer to the Share memory
   */
  void Detach(const void* ad) throw (std::string);
  /** Return Share memory name */
  inline const char* GetName() { return fName;}
};
#endif





