/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any latser version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#ifndef DEVICEFACTORYINTERFACE_H
#define DEVICEFACTORYINTERFACE_H

// To retreive the IP address of the current machine
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

#ifdef DATABASE
#  include "DbCommonAccess.h"
#  include "DbAccess.h"
#endif

#ifndef UNKNOWNFILE
#  define UNKNOWNFILE "Unknown"
#endif

/** This class is used to give the interface for the different device factory classes:
 * FecDeviceFactory
 * PiaResetFactory
 * Fed9UDeviceFactory
 * FedFecConnectionFactory
 */
class DeviceFactoryInterface {

 protected:
#ifdef DATABASE

  /** Login
   */
  std::string login_ ;

  /** Password
   */
  std::string password_ ;

  /** path
   */
  std::string path_ ;

  /** Use or not a shared connection
   */
  bool threaded_ ;

  /** Database access
   */
  DbCommonAccess *dbAccess_ ;

  /** Use or not the database
   */
  bool useDatabase_ ;

  /** If true the dbFecAccess_ will be deleted in the desctructor of this class
   * See setDatabaseAccess (string, string, string) and setDatabaseAccess (DbFecAccess)
   */
  bool deleteDbAccess_ ;

#endif

  /** output file
   */
  std::string outputFileName_ ;

 public:

  /** Create a device factory without any requierements regarding the database or file
   * \param databaseAccess - by default true, try to create an access to the database, will be implemented in the child classes
   */
  DeviceFactoryInterface ( ):
#ifdef DATABASE
    login_("nil"), password_("nil"), path_("nil"), threaded_(false), dbAccess_(NULL), useDatabase_(false), deleteDbAccess_(true),
#endif
    outputFileName_ (UNKNOWNFILE) {
  }

#ifdef DATABASE
  /** Create a database access
   * \param login - login to the database
   * \param password - password to the database
   * \param path - path to the database
   * \exception oracle::occi:SQLException in case of problem regarding the connection
   */
  DeviceFactoryInterface ( std::string login, std::string password, std::string path, bool threaded = false )
    throw ( oracle::occi::SQLException ):
    login_(login), password_(password), path_(path), threaded_(threaded), dbAccess_(NULL), useDatabase_(false), deleteDbAccess_(true), outputFileName_(UNKNOWNFILE) {
  }


  /** Use a database access
   * \param dbAccess - database access
   */
  DeviceFactoryInterface ( DbCommonAccess *dbAccess ) 
    throw ( oracle::occi::SQLException ):
    threaded_(false), dbAccess_(dbAccess), useDatabase_(false), deleteDbAccess_(false), outputFileName_(UNKNOWNFILE) {
  }

#endif

  /** Disconnect the database (if it is set)
   */  
  virtual ~DeviceFactoryInterface ( ) {

#ifdef DATABASE
  if (dbAccess_ != NULL && deleteDbAccess_)
    delete dbAccess_ ;
#endif
  }

  // ------------------------------------------------------------------------------------------------------
  // 
  // XML file methods
  //
  // ------------------------------------------------------------------------------------------------------

  /** \brief set the output file
   */
  void setOutputFileName ( std::string outputFileName ) {

    outputFileName_ = outputFileName ;
  }

  /** \brief Return the output file name
   */
  std::string getOutputFileName ( ) {

    return outputFileName_ ;
  }

  /** \brief set a new input file
   * This method must be map in setFecInputFileName, setPiaResetInputFileName, setFed9UInputFileName, setConnectionInputFileName for the multi-inheritance in DeviceFactory class.
   */
  virtual void setInputFileName ( std::string inputFileName ) = 0 ;

  /** \brief Add a new file name in the descriptions
   * This method must be map in addFecFileName, addPiaResetFileName, addFed9UFileName, addConnectionFileName for the multi-inheritance in DeviceFactory class.
   */
  virtual void addFileName ( std::string fileName ) = 0 ;

  /** \brief Use or not the file
   */
  void setUsingFile ( bool useIt = true ) {

#ifdef DATABASE
    useDatabase_ = (! useIt) ;
#endif
  }

  /** \brief Check if the database is used
   */
  bool getDbUsed ( ) {
#ifdef DATABASE
    return (useDatabase_ && (dbAccess_ != NULL)) ;
#endif 
    return false ; 
  }


  // ------------------------------------------------------------------------------------------------------
  // 
  // Database methods
  //
  // ------------------------------------------------------------------------------------------------------

#ifdef DATABASE

  /** \brief Use or not the database access
   */
  void setUsingDb ( bool useIt = true ) throw ( std::string ) {

    if (dbAccess_ != NULL) {
      useDatabase_ = useIt ;
    }
    else {
      
      if (useIt) {
#ifdef DEBUGMSGERROR
	std::cerr << "*********************************** ERROR ****************************" << std::endl ;
	std::cerr << "Cannot use the database, the access is not created" << std::endl ;
	std::cerr << "**********************************************************************" << std::endl ;
#endif
	throw std::string ( "DeviceFactoryInterface::setUsingFecDb: Cannot use the database, the access is not created" ) ;
      }
    }
  }

  /** \brief Check if the database is connected
   */
  bool getDbConnected ( ) {

    return (dbAccess_ != NULL) ;
  }

  /** Create the database access with the parameter exported
   */
  virtual void setDatabaseAccess ( ) = 0 ;

  /** Create a database access with the parameter given by arguments
   */
  virtual void setDatabaseAccess ( std::string login, std::string password, std::string path ) = 0 ;

  /** Use a database access
   * \param dbAccess - FEC access to the database
   * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
   */
  void setDatabaseAccess ( DbCommonAccess *dbAccess ) {

    // delete the old access
    if (dbAccess_ != NULL && deleteDbAccess_)
      delete dbAccess_ ;
    
    dbAccess_ = dbAccess ;
    
    deleteDbAccess_ = false ;
    //initDbVersion_ = false ;
    useDatabase_ = true ;
  }

  /** \return the database access
   */
  DbCommonAccess *getDatabaseAccess ( ) {

    return dbAccess_ ;
  }

  /** \return the partition name
   * \bug this method must be only called by the FecDeviceFactory or PiaResetFactory or Fed9UDeviceFactory that inherits from this interface. In any other case so TkDcuConversionFactory or DetIdDcuFactory, this method will crash !
   */
  std::string getPartitionName ( unsigned int partitionId )  throw (oracle::occi::SQLException) {

    return (((DbAccess *)dbAccess_)->getPartitionName ( partitionId )) ;
  }

#endif

  // ------------------------------------------------------------------------------------------------------
  // 
  // Static methods
  //
  // ------------------------------------------------------------------------------------------------------

 /** Retreive the IP address of the machine
  */
/*  static std::string getSupervisorIp ( ) { */

/*    // IP number of this machine mainly for database access */
/*    std::string supervisorIp ; */
/*    char hostname[100] ; */
/*    if (! gethostname (hostname, 100)) { */
    
/*      struct hostent *ip = gethostbyname ( hostname ) ; */
/*      if ( (ip != NULL) && (ip->h_addrtype == AF_INET) ) { */
       
/*        struct in_addr *addr = (struct in_addr *)(*ip->h_addr_list) ; */
       
/*        supervisorIp.assign (inet_ntoa (*addr)) ; */
/*      } */
/*    } */
/*    else { */
     
/*      std::cerr << "*************************************************" << std::endl ; */
/*      std::cerr << "Could not retreive the IP address of the machine" << std::endl ; */
/*      std::cerr << "Please fill the export params (default value set)" << std::endl ; */
/*      std::cerr << "*************************************************" << std::endl ; */
/*      supervisorIp = "127.0.0.1"; */
/*    } */

/*    return (supervisorIp) ; */
/*  } */

#ifdef DATABASE
  /** \brief return the error message coming from oracle exception and get it if it exists the message set by the user exception in PL/SQL
   * \param message - message from caller side (mainly when the error appends)
   * \param ex - oracle exception raised
   * \return error message 
   */
  std::string what ( std::string message, oracle::occi::SQLException &ex ) {

    std::stringstream msgError ;
    
    if (getDatabaseAccess() != NULL) msgError << getDatabaseAccess()->what(message,ex) ;
    else msgError << message << ": " << ex.what() ;
    
    return msgError.str() ;
  }

 /** \brief Retrieve the configuration and return true if the login password and path is ok
  */
 static bool getDatabaseConfiguration ( std::string &login, std::string &password, std::string &path ) {

   login="nil" ; password="nil" ; path="nil" ;
   DbCommonAccess::getDbConfiguration (login, password, path) ;
   if ((login == "nil") || (password == "nil") || (path == "nil")) return false ;
   return (true) ;
 }
#endif

};

#endif
