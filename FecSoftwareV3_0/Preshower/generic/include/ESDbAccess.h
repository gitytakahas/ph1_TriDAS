/*
This file is part of Fec Software project.
It is used to test the performance of DB--FecSoftware

author: HUNG Chen-Chien(Dominique), NCU, Jhong-Li, Taiwan
based on DbAccess by
Frderic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/


#ifndef ESDBACCESS_H
#define ESDBACCESS_H
#include <cstring>
#include <occi.h>

#include "DbCommonAccess.h"
#include "FecExceptionHandler.h"

/** \brief This virtual class is implemented to handle the communication between the supervisor software and the database.
 *
 * This class provides some features like :
 *  - OCCI environnement and database connection opening and closing 
 *  - download and upload execution
 *  - database version request
 *
 * Private attributes :
 *  - dbEnvironment_ : pointer on an OCCI environnement descriptor
 *  - dbConnection_ : pointer on a database connection descriptor
 *  - xmlClob_ : pointer on a Clob
 */
class ESDbAccess : public DbCommonAccess {
 
 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  ESDbAccess(bool threaded = false) throw (oracle::occi::SQLException);

  /** \brief Creates the access to the database
   */
  ESDbAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);
  
  /** \brief Closes the connection with the database
   */
  virtual ~ESDbAccess ( ) throw (oracle::occi::SQLException);

  /** \brief Retreive the version for each partition in the current state
   */
  virtual std::list<unsigned int*> getDatabaseVersionFromQuery (std::string sqlQuery) throw (oracle::occi::SQLException);

  /** \brief Retreive the version for each partition in the current state
   */
  virtual std::list<unsigned int*> getDatabaseVersionFromQuery (std::string sqlQuery, std::string partitionName) throw (oracle::occi::SQLException);

  /** \brief Retreive the version for each partition in the current state
   */
  virtual std::list<unsigned int*> getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException) = 0;

  /** \brief Get version number
   */
  virtual unsigned int getVersion(std::string sqlQuery) throw (oracle::occi::SQLException);

  /** \brief Retrives the partitionName from the partition identifier
   */
  virtual std::string getPartitionName (unsigned int partitionId ) throw (oracle::occi::SQLException);
  
  /** \brief Retrives the partition id from a partition name
   */
  virtual unsigned int getPartitionId (std::string partitionName) throw (oracle::occi::SQLException);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClob (std::string partitionName) throw (oracle::occi::SQLException) = 0 ;

  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) = 0;

  /** \brief Download a Clob from the database
   */
  virtual oracle::occi::Clob *getXMLClob (std::string partitionName, std::string hardId) throw (oracle::occi::SQLException) = 0;

  /** \brief Download a Clob from the database
   */
  virtual oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, std::string hardId, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) = 0;

  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName) throw (oracle::occi::SQLException);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName, std::string id) throw (oracle::occi::SQLException);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName, std::string id, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException);
  
  /** \brief Download a Clob from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString) throw (oracle::occi::SQLException);

  /** \brief Download a Clob from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName, std::string id, unsigned int ri) throw (oracle::occi::SQLException);

  /** \brief Upload parameters to the database    
   */
  virtual void setXMLClobFromQuery (std::string writeString, std::string buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException, FecExceptionHandler);
  
  /** \brief Upload parameters to the database    
   */
  virtual void setXMLClobFromQuery (std::string writeString, std::string buffer, std::string partitionName, int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler);
  
  /** \brief Upload parameters to the database    
   */
  virtual unsigned int setXMLClobFromQuery (std::string writeString, std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);

  /** \brief Upload parameters to the database    
   */
  virtual unsigned int setXMLClobFromQuery (std::string writeString, std::string *buffer, std::string partitionName, bool newPartition) throw (oracle::occi::SQLException, FecExceptionHandler);
  
  /** \brief Get the current run number
   */
  virtual unsigned int getCurrentRunNumber() throw (FecExceptionHandler);

  void traceSql(boolean start);

};
#endif

