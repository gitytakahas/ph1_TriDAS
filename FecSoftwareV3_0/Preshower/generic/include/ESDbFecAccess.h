/*
This file is part of Fec Software project.
It is used to test the performance of DB--FecSoftware

author: HUNG Chen-Chien(Dominique), NCU, Jhong-Li, Taiwan
based on DbFecAccess by
Frderic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#ifndef ESDBFECACCESS_H
#define ESDBFECACCESS_H
#include "ESDbAccess.h"

// ostream use for int to string conversion
#include <sstream>
#include "stringConv.h"

//declaraion of the exception handler for the FEC
#include "FecExceptionHandler.h"


/** \brief This class is implemented to handle the communication between the FEC supervisor software and the database.
 *
 */
class ESDbFecAccess : public ESDbAccess {

	public:
		//
		//public functions
		//
		/** \brief Default constructor
		 */
		ESDbFecAccess(bool threaded = false) throw (oracle::occi::SQLException);
		
		/** \brief Constructor with connection parameters
		 */
		ESDbFecAccess(std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);
		
		/** \brief Destructor
		 */
		~ESDbFecAccess() throw (oracle::occi::SQLException);
		
		/** \brief Retreive the version, in the current state, for the given partition name
		 */
		std::list<unsigned int*> getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException);
		
		/** \brief Create a new current state with a set of partitions-versions
		 */
		void setDatabaseVersion(std::list<unsigned int*> partitionVersionsList) throw (FecExceptionHandler);
		
		/** \brief Retrives the next minor version with major version= majorId for database upload
		 */
		unsigned int getNextMinorVersion(unsigned int majorId) throw (oracle::occi::SQLException);
		
		/** \brief Retrives the next major version for database upload
		 */
		unsigned int getNextMajorVersion() throw (oracle::occi::SQLException);
		
		/** \brief Creates a new state
		 */
		unsigned int createNewStateHistory(std::string partitionName, unsigned int *partitionId, unsigned int *fecVersionMajorId) throw (FecExceptionHandler);
		
		
		/** \brief Download a Clob from the database
		 */
		oracle::occi::Clob *getXMLClob (std::string partitionName) throw (oracle::occi::SQLException);
		
		/** \brief Download a Clob from the database
		 */
		oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException);
		
		/** \brief Download parameters from the database
		 */
		oracle::occi::Clob *getXMLClob (std::string partitionName, std::string id) throw (oracle::occi::SQLException);
		
		/** \brief Download parameters from the database
		 */
		oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, std::string id, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException);
		
		/** \brief Download CCU parameters from the database for a given Fec/Ring
		 */
		oracle::occi::Clob *getCcuXMLClob(std::string partitionName, std::string fecId, unsigned int ring)
		throw (oracle::occi::SQLException);
		
		/** \brief Upload a Clob to the database to configure the CCUs
		 */
		void setCcuXMLClob(std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);
		
		/** \brief Upload a Clob to the database to configure a ring containing CCUs
		 */
		void setRingCcuXMLClob(std::string ringBuffer, std::string ccuBuffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);
		
		/** \brief Upload a Clob to the database for configuration
		 */
		unsigned int setXMLClob (std::string* buffer, std::string partitionName, boolean newPartition) throw (oracle::occi::SQLException, FecExceptionHandler);
		
		/** \brief Upload a Clob from the database
		 */
		void setXMLClobWithVersion (std::string* buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException, FecExceptionHandler);
		
		/** \brief Upload a Clob to the database for configuration
		 */
		void setXMLClob (std::string* buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler);
		
		/** \brief Upload a Clob to the database for configuration
		 */
		void setXMLClob(std::string stringRequest, std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);
		
		/** \brief Upload two Clobs at a time to the database for configuration
		 */
		void setXMLClob(std::string stringRequest, std::string bufferOne, std::string bufferTwo, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);
		
	};
	
#endif
		
		
