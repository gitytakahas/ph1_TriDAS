#ifndef UPLOADTHEAD_H
#define UPLOADTHEAD_H

#include "xdaq.h"

#ifdef DATABASE
#include "DbFecAccess.h"
#endif

#include "HashTable.h"

class UploadThread: public Task
{
 private:

  /** Just a counter of thread
   */
  unsigned int threadId_ ;

#ifdef DATABASE
  /** Access to the database
   */
  DbFecAccess *dbFecAccess_ ;
#endif

  /** File name for the output
   */
  std::string xmlFileName_ ;
	
  /** database access possible
   */
  bool databaseAccess_ ;

  /** database used
   */
  bool databaseDownload_ ;

  /** Vector of the last values of the DCU
   */
  deviceVector *dcuDevices_ ;

  /** TID of the caller
   */
  unsigned int instance_ ;

  /** Supervisor IP of the remote machine
   */
  std::string supervisorIp_ ;

  /** Class name
   */
  std::string className_ ;

  /** Halt the upload thread
   */
  bool halted_ ;

  /** Is the thread is running
   */
  bool isRunning_ ;
  
 public:
  
  /** \brief Constructor
   */
  UploadThread( unsigned int ) ;

  /** \breif Destructor
   */
  ~UploadThread ( ) ;

  /** \brief Set XDAQ description
   */
  void setXDaqDescription ( std::string supervisorIp, std::string className, unsigned int instance, deviceVector devices ) ;

  /** \brief set the output file
   */
  void setOutputFileName ( std::string fileName ) ;

#ifdef DATABASE
  /** \brief set the database access
   */
  void setDatabaseAccess ( DbFecAccess *dbFecAccess ) ;
#endif
  
  /** \brief upload the data into a file or database
   */
  void uploadData( ) ;

  /** \brief return true if the thread is running
   */
  bool isAlive() ;

  /** \brief stop the thread if needed
   */
  bool setStopThread() ;

  /** Thread for the upload
   */
  int svc() ;
} ;

#endif


