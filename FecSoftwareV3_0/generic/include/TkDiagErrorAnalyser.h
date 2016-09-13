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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#ifndef TKDIAGERRORANALYSER_H
#define TKDIAGERRORANALYSER_H

#include "DeviceFactory.h"
#include "map"
#include "string"

#define NOLIFEINPARAMETER 0xFFFFFFFF

/** Data type for error counters
 * \param fecErrorCounter - FEC error counter
 * \param fedErrorCounter - FED error counter
 * \param psuErrorCounter - PSU error counter
 */
typedef struct {
  unsigned int fecErrorCounter ;
  unsigned int fedErrorCounter ;
  unsigned int psuErrorCounter ;
} ErrorCounterStruct ;

/** This class will download from the database all the connectivity from FEC, FED, connections, PSU, det id, DCU ID
 * and will manage the error counters related to each part. 
 * For example, if a specific FEC device have an error then error counter from the FED channel related to that FED device will be increated
 * and the same for connections, PSU, det id, DCU ID.
 * If an error is raised on non known hardware part, a unknown error counter is managed.
 * The errors are listed following:
 * <lu>
 * <li> FEC device error (fechardwareId,ring,ccu,channel,address):</li>
 * <lu>
 * <li>+1 on the set of connections for PLL,MUX,AOH errors
 * <li>+1 on the connection for APV errors
 * <li>+1 on the device in error
 * <li>+1 on the corresponding FED channel for PLL,MUX,APV,AOH errors
 * <li>+1 on the corresponding PSU for PLL,MUX,APV,AOH errors
 * </li>
 * <li>FED error (crate,slot,channel or fedsoftid,channel):
 * <lu>
 * <li>+1 on the set of connections FEC and FED
 * <li>+1 on the set of PSU
 * </lu>
 * <li>PSU error
 * <lu>
 * <li>+1 on the set of connections FEC and FED
 * <li>+1 on the set of PSU
 * <li>+1 on the correponding connections
 * </lu>
 * </lu>
 * 
 * \version 1.0
 * \author Frederic Drouhin, Laurent Gross, Laurent Mirabito
 */
class TkDiagErrorAnalyser {

 private:

  /** Device Factory for database access
   */
  DeviceFactory *deviceFactory_ ;

  /** Partition name
   */
  std::string partitionName_ ;

  /** Connection description vector
   */
  ConnectionVector connectionVector_ ;

  /** DCU to connection description
   */
  std::map<unsigned int, ConnectionDescription *> dcuToConnection_ ;

  /** det id to connection description
   */
  std::map<unsigned int, ConnectionDescription *> detIdToConnection_ ;

  /** PSU / DCU map based on the PSU name (dp name, pvss name)
   */
  std::map<std::string, ConnectionDescription *> psuNameToConnection_ ;

  /** PSU / DCU map based on the PSU name (dp name)
   */
  std::map<std::string, ConnectionDescription *> dpNameToConnection_ ;

  /** PSU / DCU map based on the PSU name (PVSS name)
   */
  std::map<std::string, ConnectionDescription *> pvssNameToConnection_ ;

  /** Error counter
   */
  std::map<ConnectionDescription *, ErrorCounterStruct> errorCounters_ ;

  /** Vector of sorted elements for FEC by crate/slot/ring/ccu/i2cchannel/i2caddress
   */
  ConnectionVector fecConnectionVector_ ;

  /** Vector of sorted elements for FED by crate/slot/fedchannel
   */
  ConnectionVector fedConnectionVector_ ;

  /** Vector of det id sorted
   */
  std::vector<unsigned int> detIdList_ ;

  /** Vector of PVSS name sorted
   */
  std::vector<std::string> pvssNameList_ ;

  /** Vector of devices based on the previous map
   */
  deviceVector listVectorDevices_ ;

  /** List of modules as connections
   */
  ConnectionVector listModulesAsConnection_ ;

  /** Errors by device
   */
  std::map<deviceDescription *, unsigned int> errorOnDevices_ ;

  /** Connection not identified with a DET ID
   */
  unsigned int connectionNotIdentified_ ;

  /** PSU/DCU not identified with a connection
   */
  unsigned int psuNotIdentified_ ;

#ifdef DATABASE
  /** Build the PSU name to DCU hard ID
   */
  void buildDatabaseInformation ( std::string partitionName, std::string login, std::string password, std::string path ) throw (oracle::occi::SQLException, FecExceptionHandler ) ;
#endif

 public:

  /** \brief Create a database access with the CONFDB
   */
  TkDiagErrorAnalyser ( std::string partitionName, std::string login = "nil", std::string password = "nil", std::string path = "nil" ) 
#ifdef DATABASE
    throw (oracle::occi::SQLException, FecExceptionHandler) ;
#else 
  throw (FecExceptionHandler) ;
#endif

  /** \brief Destroy the database access
   */
  ~TkDiagErrorAnalyser ( ) ;

  /** \brief return the number of connections not identifed with a det ID
   * (14:54:37) isthar: getConnectionNotIdentified a appeller apres le constructeur
   * (14:54:52) isthar: ca te donne le nombre de connections sans detid
   * (14:55:28) isthar: et si getConnectionByFec.size() == getConnectionNotIdentified() alors y'a pas de det id pour toutes les connections
   */
  unsigned int getConnectionNotIdentified ( ) { return connectionNotIdentified_ ; }

  /** \brief return the number of connections not identifed with a det ID
   * (14:54:37) isthar: getPsuNotIdentified a appeller apres le constructeur
   * (14:54:52) isthar: ca te donne le nombre de PSU/DCU sans connection
   */
  unsigned int getPsuNotIdentified ( ) { return psuNotIdentified_ ; }

  /** \brief Set an error for a given FEC
   */
  void setFecError ( std::string fecHardwareId, unsigned int ring = NOLIFEINPARAMETER, unsigned int ccu = NOLIFEINPARAMETER, unsigned int channel = NOLIFEINPARAMETER, unsigned int i2cAddress = NOLIFEINPARAMETER ) throw (std::string) ;

  /** \brief Set an error for a given FED
   */
  void setFedSoftIdError ( unsigned int fedSoftId, unsigned int fedChannel = NOLIFEINPARAMETER ) throw (std::string) ;

  /** \brief Set an error for a given FED
   */
  void setFedCrateIdError ( unsigned int crateId, unsigned int slot, unsigned int fedChannel = NOLIFEINPARAMETER ) throw (std::string) ;

  /** \brief Set an error for a given PSU channel
   */
  void setPsuError ( std::string psuName ) throw (std::string) ;

  /** \brief Set an error for a given DCU hard ID
   */
  void setDcuHardIdError ( unsigned int dcuHardId ) throw (std::string) ;

  /** \brief Set an error for given det id 
   */
  void setDetIdError ( unsigned int detId ) throw (std::string) ;

  /** \brief Get the error counter by FEC
   */
  void getFecModuleErrorCounter ( std::string fecHardwareId, unsigned int ring, unsigned int ccuAddress, unsigned int i2cChannel, 
				  unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter  ) ;

  /** \brief Get the error counter for a given connection based on the APV address
   */
  void getConnectionErrorCounter ( std::string fecHardwareId, unsigned int ring, unsigned int ccuAddress, unsigned int i2cChannel, unsigned int apvAddress, 
				   unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) ;

  /** \brief Get the error counter by FEC device
   */
  void getDeviceErrorCounter ( std::string fecHardwareId, unsigned int ring, unsigned int ccuAddress, unsigned int i2cChannel, unsigned int i2cAddress, 
			       unsigned int &deviceErrorCounter ) ;

  /** \brief Get the error counter by FED
   */
  void getFedChannelErrorCounter ( unsigned int crateId, unsigned slot, unsigned fedChannel, 
				   unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) ;

  /** \brief Get the error counter by FED
   */
  void getFedChannelErrorCounter ( unsigned int fedSoftId, unsigned fedChannel, 
				   unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) ;

  /** \brief Get the error counters by DET ID
   */
  void getDetIdErrorCounter ( unsigned int detId, 
			      unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) ;

  /** \brief Get the error counters by PSU
   */
  void getPsuErrorCounter ( std::string psuName, 
			    unsigned int &fecErrorCounter, unsigned int &fedErrorCounter, unsigned int &psuErrorCounter ) ;

  /** \brief Get the connection sorted by FEC 
   */
  ConnectionVector &getConnectionByFec ( ) ;

  /** \brief Get the connection sorted by FED
   */
  ConnectionVector &getConnectionByFed ( ) ;

  /** \brief Get the connection by Det ID
   */
  std::vector<unsigned int> &getListOfDetId ( ) ;

  /** \brief Get the PSU list sorted
   */
  std::vector<std::string> &getListOfPVSSName ( ) ;

  /** \brief Return the list of device sorted by device (crate,fec,ring,ccu,channel,address)
   */
  deviceVector &getListOfDevices ( ) ;

  /** \brief Return the list of modules sorted by FEC/Ring/CCU/i2c channel
   */
  ConnectionVector &getListOfModulesAsConnection ( ) ;

  /** \brief Sort the vector of connection by FEC
   */
  static bool sortConnectionByFec ( ConnectionDescription *el1, ConnectionDescription *el2 ) ;

  /** \brief Sort the vector of connection by FED
   */
  static bool sortConnectionByFed ( ConnectionDescription *el1, ConnectionDescription *el2 ) ;
};

#endif

