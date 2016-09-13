/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#include "DbConnectionsAccess.h"

// ostream use for int to string conversion 
#include <sstream>
#include "stringConv.h"

/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess()
 */
DbConnectionsAccess::DbConnectionsAccess ( bool threaded ) throw (oracle::occi::SQLException) : DbAccess (threaded) {
}

/**Create an access to the database
 * @param threaded - this parameter define if you want or not to share the connections between all applications (by default false)
 * @exception oracle::occi::SQLException
 * @see DbAccess::DbAccess(std::string user, std::string passwd, std::string dbPath)
 */
DbConnectionsAccess::DbConnectionsAccess (std::string user, std::string passwd, std::string dbPath, bool threaded) throw (oracle::occi::SQLException) : DbAccess (user, passwd, dbPath, threaded) {
}

/**Close the access to the database
 * @exception oracle::occi::SQLException
 * @see DbAccess::~DbAccess()
 */
DbConnectionsAccess::~DbConnectionsAccess ()  throw (oracle::occi::SQLException) {
}

/**Get the current version for a given partition name
 * @param partitionName - Partition Name
 * @return a pointer to a list of unsigned int pointers (partitionId, versionMajorId, versionMinorId) : THIS LIST OF POINTERS HAS TO BE REMOVED BY THE USER !!
 * @exception oracle::occi::SQLException
 * @see DbAccess::getDatabaseVersion (std::string sqlQuery)
 * @todo use a callable statement to execute a PL/SQL function with bind variable to avoid the query parsing at every call 
 */
std::list<unsigned int*> DbConnectionsAccess::getDatabaseVersion (std::string partitionName) throw (oracle::occi::SQLException){

  static std::string sqlQuery ;
  sqlQuery = "SELECT DISTINCT StateHistory.partitionId, StateHistory.connectionVersionMajorId, StateHistory.connectionVersionMinorId, StateHistory.maskVersionMajorId, StateHistory.maskVersionMinorId FROM CurrentState, StateHistory, Partition  WHERE StateHistory.stateHistoryId = CurrentState.stateHistoryId AND StateHistory.partitionId = Partition.partitionId AND Partition.partitionName= :paramPartitionName";

  return DbCommonAccess::getDatabaseVersionFromQuery(sqlQuery, partitionName);
}

/**Define the query to get the next major version and execute the request
 * @return the next available major version 
 * @exception SQLException
 * @see DbAccess::getVersion(std::string sqlQuery)
 */
unsigned int DbConnectionsAccess::getNextMajorVersion() throw (FecExceptionHandler)
{
  static std::string sqlQuery = "SELECT MAX(versionMajorId)+1 FROM ConnectionVersion";
  return (unsigned int)DbAccess::getVersion(sqlQuery);
}

/**Define the query to get the next minor version with major version = majorId and execute the request
 * @return the greatest minor version with major version = majorId
 * @param majorId : major version number
 * @exception SQLException
 * @see DbAccess::getVersion(std::string sqlQuery)
 */
unsigned int DbConnectionsAccess::getNextMinorVersion(unsigned int majorId) throw (FecExceptionHandler)
{
  static std::string sqlQuery ;
  sqlQuery = "SELECT MAX(versionMinorId)+1 FROM ConnectionVersion where versionMajorId =" + toString(majorId);
  return (unsigned int)DbAccess::getVersion(sqlQuery);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName)</I>.<BR>
 * @param partitionName - name of the partition to be accessed
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string partitionName)
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName)
 * @see PkgConnectionsXML.getAllConnections ( partitionName IN VARCHAR2 ) RETURN CLOB;
 */
oracle::occi::Clob *DbConnectionsAccess::getXMLClob( std::string partitionName ) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :result := PkgConnectionsXML.getAllConnectionFromPartition(:partitionName);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)</I>.<BR>
 * @param partitionName - XDAQ class name 
 * @param versionMajor - major version number
 * @param versionMinor - minor version number 
 * @param maskVersionMajor - mask major version
 * @param maskVersionMinor - mask minor version
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)
 * @see PkgConnectionsXML.getAllConnections ( partitionName IN VARCHAR2, versionMajorId IN NUMBER, versionMinorId IN NUMBER ) RETURN CLOB;
 * @todo : implement this method... if connections versionning is requiered...
 */
oracle::occi::Clob *DbConnectionsAccess::getXMLClobWithVersion (std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int maskVersionMajor, unsigned int maskVersionMinor) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgConnectionsXML.getAllConnectionFromPartition(:partitionName, :versionMajorId, :versionMinorId, :maskMajor, :maskMinor);END;";
  return DbAccess::getXMLClobFromQuery(readString, partitionName, versionMajor, versionMinor, maskVersionMajor, maskVersionMinor);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to get a Clob containing the data from the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)</I>.<BR>
 * @param partitionName - XDAQ class name 
 * @param versionMajor - major version number
 * @param versionMinor - minor version number 
 * @return a pointer on a Clob containing the data from the database.
 * @exception oracle::occi::SQLException
 * @see DbAccess::getXMLClob(std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)
 * @see DbAccess::getXMLClob(std::string readString, std::string partitionName, unsigned int versionMajor, unsigned int versionMinor)
 * @see PkgConnectionsXML.getAllConnections ( partitionName IN VARCHAR2, versionMajorId IN NUMBER, versionMinorId IN NUMBER ) RETURN CLOB;
 * @todo : implement this method... if connections versionning is requiered...
 */
oracle::occi::Clob *DbConnectionsAccess::getApvNotConnectedWithVersion (std::string partitionName, unsigned int versionMajor, unsigned int versionMinor) throw (oracle::occi::SQLException) {
  static std::string readString = "BEGIN :xmlClob := PkgConnectionsXML.getAllApvNotConnected(:partitionName, :versionMajorId, :versionMinorId);END;";
  return DbAccess::getXMLClobFromQueryWithoutMask(readString, partitionName, versionMajor, versionMinor);  
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, unsigned int versionMajorId, unsigned int versionMinorId)</I>.<BR>
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param versionMajorId - major version number for upload
 * @param versionMinorId - minor version number for upload
 * @exception oracle::occi::SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string buffer, unsigned int versionMajorId, unsigned int versionMinorId)
 * @see DbAccess::setXMLClob(std::string writeString, std::string buffer, unsigned int versionMajorId, unsigned int versionMinorId)
 * @see PkgConnectionsXML.uploadXMLClob ( xmlClob IN CLOB, versionMajor IN NUMBER, versionMinor IN NUMBER);
 * @todo : connections versionning is not yet implemented
 */
void DbConnectionsAccess::setXMLClobWithVersion (std::string buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) throw (oracle::occi::SQLException) 
{
  static std::string writeString = "BEGIN PkgConnectionsXML.uploadXMLClob(:buffer, :partitionName, :versionMajorId, :versionMinorId);END;";
  DbAccess::setXMLClobFromQuery  (writeString, buffer, partitionName, versionMajorId, versionMinorId);
}

/**Sends a request to the database to execute a PL/SQL stored procedure in order to set a Clob containing the data to the database.<BR>
 * A queryString is defined and used as parameter for the method <I>DbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)</I>.<BR>
 * The data are stored in the version number :
 *   - nextMajorVersion.0 if versionUpdate = 1
 *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
 *   - currentMajorVersion.0 if versionUpdate = 2
 * @param buffer - pointer on a buffer used for upload
 * @param partitionName - partition name
 * @param versionUpdate - set this parameter to true for a next major version
 * @exception SQLException
 * @exception FecExceptionHandler
 * @see DbAccess::setXMLClob(std::string buffer, boolean versionUpdate)  
 * @see DbAccess::setXMLClob(std::string writeString, std::string buffer, boolean versionUpdate)  
 * @see PkgConnectionsXML.uploadXMLClob(xmlClob IN CLOB, nextMajor IN NUMBER)
 * @todo : connections versionning is not yet implemented
 */
void DbConnectionsAccess::setXMLClob(std::string buffer, std::string partitionName, unsigned int versionUpdate) throw (oracle::occi::SQLException, FecExceptionHandler) {
  static std::string writeString = "BEGIN PkgConnectionsXML.uploadXMLClob(:buffer, :partitionName, :versionUpdate);END;";
  DbAccess::setXMLClobFromQuery (writeString, buffer, partitionName, versionUpdate);
}

