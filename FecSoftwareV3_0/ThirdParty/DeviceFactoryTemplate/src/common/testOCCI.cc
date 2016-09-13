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

#include <iostream>
#include <string>

#include "DeviceFactory.h"

/**
 */
int flatXML ( oracle::occi::Connection *dbConnection, std::string partitionName ) ;

/**
 */
int indentXML ( oracle::occi::Connection *dbConnection, std::string partitionName ) ;

/**
 */
int main ( int argc, char *argv[] ) {

  unsigned long startMillis, endMillis ;
  unsigned int choix = 0 ;

  // Which partition you want to use ...
  std::string partitionName = "ALL" ;

  // Find the options
  for (int i = 1 ; i < argc ; i ++) {

    std::string param ( argv[i] ) ;

    if (param == "-partition") {

      if (i < argc) {
	partitionName = std::string(argv[i+1]) ;
	i ++ ;
      }
      else
	std::cerr << "Error: you must specify the partition name after the option -partition" << std::endl ;
      
    }
    else if (param == "-flat") {
      choix = 1 ;
    }
    else if (param == "-indent") {
      choix = 2 ;
    }
    else {
      std::cerr << "Error: you must specify the run number with the option -ident | -flat" << std::endl ;
      return -1 ;
    }
  }

  if (choix == 0) {
    std::cerr << "Error: you must specify the run number with the option -ident | -flat" << std::endl ;
    return -1 ;
  }

  // Find the login/pass/path
  std::string login = "nil" ;
  std::string passwd = "nil" ;
  std::string dbPath = "nil" ;
  DbCommonAccess::getDbConfiguration (login, passwd, dbPath) ;

  if (login == "nil" || passwd == "nil" || dbPath == "nil") {

    std::cerr << "CONFDB not defined" << std::endl ;
    return -1 ;
  }

  // ------------------------------------------------------------
  // -------------------- Creating the DB access
  oracle::occi::Environment *dbEnvironment = NULL ;
  oracle::occi::Connection *dbConnection = NULL ;
  try {
    dbEnvironment = oracle::occi::Environment::createEnvironment (oracle::occi::Environment::THREADED_MUTEXED) ;
    dbConnection = dbEnvironment->createConnection(login, passwd, dbPath);
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Unable to connect the database: " << e.what() << std::endl ;
    return -1 ;
  }

  std::cout << "Database " << login << "@" << dbPath << " connected " << std::endl ;

  // ------------------------------------------------------------
  // Manage the different access
  switch (choix) {
  case 1:
    flatXML ( dbConnection, partitionName ) ;
    break ;
  case 2:
    indentXML ( dbConnection, partitionName ) ;
    break ;
  }

  // ------------------------------------------------------------
  // -------------------- Close connection
  try {
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    dbEnvironment->terminateConnection(dbConnection);
    oracle::occi::Environment::terminateEnvironment(dbEnvironment);
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Deleting the connection tooks " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  catch (oracle::occi::SQLException &e) {
    std::cerr << "Unable to disconnect the database: " << e.what() << std::endl ;
    return -1 ;
  }

  return 0 ;
}

/** -----------------------------------------------------------
 * Produce a flat XML buffer by APV strip
 */
int flatXML ( oracle::occi::Connection *dbConnection, std::string partitionName ) {

  std::cout << "Making request for partition " << partitionName << std::endl ;

  static std::string sqlQueryPid = "(SELECT partitionId FROM Partition WHERE partitionName = :a)" ;
  static std::string sqlQueryFed = "SELECT fedId, idFed, fedHardId, name, crate, crateSlot, partitionId, fedValueState, fedMode, superMode, optoRxResistor, scopeLength, clockSource, triggerSource, halBusAdaptor, readRoute, testRegister, maxTempCriticalBeFpga, maxTempFpgaBeFpga, maxTempLm82BeFpga,feFirmwareVersion, beFirmwareVersion, vmeFirmwareVersion, delayFirmwareVersion, fedVersion, epromVersion, globalClockCoarseShift, globalClockFineShift,eventType, headerType, fakeEventFile, fakeEventTriggerDelay, fov,standBy, statusRegisterReset, min2point5V, max2point5V,min3point3V, max3point3V, min5V, max5V,	min12V,	max12V,		minCoreV, maxCoreV, minSupplyV,	maxSupplyV, minExternalTemp, maxExternalTemp, minInternalTemp, maxInternalTemp,offsetTemp, whichOffsetTemp, l1AcceptCoarseDelay, l1AcceptFineDelay,brcstStrTwoCoarseDelay,	brcstStrTwoFineDelay,dllPumpCurrent,	pllPumpCurrent,	iacId, i2cId,hammingChecking, bunchCounter, eventCounter,clock40DeskwedTwo, clock40DeskwedTwoOutput, clockL1AcceptOutput,parallelOutputBus, serialBOutput, nonDeskwedClock40Output,feFpgaId, idFeFpga, feFpgaValuesState, maxTempCritical, maxTempFpga, maxTempLm82,	optoRxInputOffset, optoRxOutputOffset, optoRxCapacitor,channelPairId, idChannelPair,fakeEventRandomSeed, fakeEventRandomMask,channelId, idChannel,coarseDelay, fineDelay,	threshold, trimDacOffset,	adcInputRangeMode, dataComplement,deviceId, idApvFed,state, medianOverride, value, versionmajorid, versionminorid FROM ViewStrip_new where partitionId = :a" ; 
  //static std::string sqlQueryFed = "SELECT count(*) FROM ViewApvFed_new where partitionId = :a" ;

  oracle::occi::Statement *stmtPid = NULL ;
  oracle::occi::ResultSet *rsetPid = NULL ;
  oracle::occi::Statement *stmtFed = NULL ;
  oracle::occi::ResultSet *rsetFed = NULL ;

  unsigned int i = 0, partitionId = 0 ;
  unsigned long startMillis, endMillis ;
  try {

    // -------------- retreive the partitionId
    std::cout << sqlQueryPid << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    stmtPid = dbConnection->createStatement (sqlQueryPid);
    stmtPid->setString(1,partitionName) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "The preparation of the request tooks: " << (endMillis-startMillis) << " ms" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    rsetPid = stmtPid->executeQuery ();
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "The executeQuery tooks: " << (endMillis-startMillis) << " ms" << std::endl ;
    while (rsetPid->next ()) {
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      partitionId = (unsigned int)rsetPid->getUInt (++i);
    }
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (partitionId != 0) {
      std::cout << "Retreive partition ID " << partitionId << " for partition " << partitionName << " tooks: " << (endMillis-startMillis) << " ms" << std::endl ;

      // -------------- Request FED data
      std::cout << sqlQueryFed << std::endl ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      stmtFed = dbConnection->createStatement (sqlQueryFed);
      stmtFed->setUInt(1,partitionId) ;
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "The preparation of the request tooks: " << (endMillis-startMillis) << " ms" << std::endl ;

      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      rsetFed = stmtFed->executeQuery ();
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      std::cout << "The executeQuery tooks: " << (endMillis-startMillis) << " ms" << std::endl ;

      unsigned long completeDuration = 0 ;
      unsigned long rowsAnalysed = 0 ;
      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      while (rsetFed->next ()) {
	//startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	i = 1 ;
	unsigned int fedId = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int idFed = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int fedHardId = (unsigned int)rsetFed->getUInt (i); i++;
	std::string name = rsetFed->getString (i); i++;
	unsigned int crate = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int crateSlot = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int partitionId = (unsigned int)rsetFed->getUInt (i); i++;
	std::string fedValueState = rsetFed->getString (i); i++;
	std::string fedMode = rsetFed->getString (i); i++;
	std::string superMode = rsetFed->getString (i); i++;
	unsigned int optoRxResistor = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int scopeLength = (unsigned int)rsetFed->getUInt (i); i++;
	std::string clockSource = rsetFed->getString (i); i++;
	std::string triggerSource = rsetFed->getString (i); i++;
	std::string halBusAdaptor = rsetFed->getString (i); i++;
	std::string readRoute = rsetFed->getString (i); i++;
	unsigned int testRegister = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int maxTempCriticalBeFpga = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int maxTempFpgaBeFpga = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int maxTempLm82BeFpga = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int feFirmwareVersion = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int beFirmwareVersion = (unsigned int)rsetFed->getUInt (i); i++;
	std::string vmeFirmwareVersion = rsetFed->getString (i); i++;
	std::string delayFirmwareVersion = rsetFed->getString (i); i++;
	unsigned int fedVersion = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int epromVersion = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int globalClockCoarseShift = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int globalClockFineShift = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int eventType = (unsigned int)rsetFed->getUInt (i); i++;
	std::string headerType = rsetFed->getString (i); i++;
	std::string fakeEventFile = rsetFed->getString (i); i++;
	std::string fakeEventTriggerDelay = rsetFed->getString (i); i++;
	unsigned int fov = (unsigned int)rsetFed->getUInt (i); i++;
	std::string standBy = rsetFed->getString (i); i++;
	std::string statusRegisterReset = rsetFed->getString (i); i++;
	unsigned int min2point5V = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int max2point5V = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int min3point3V = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int max3point3V = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int min5V = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int max5V = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int min12V = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int max12V = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int minCoreV = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int maxCoreV = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int minSupplyV = (unsigned int)rsetFed->getUInt (i); i++;
        unsigned int maxSupplyV = (unsigned int)rsetFed->getUInt (i); i++;
	std::string minExternalTemp = rsetFed->getString (i); i++;
	unsigned int maxExternalTemp = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int minInternalTemp = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int maxInternalTemp = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int offsetTemp = (unsigned int)rsetFed->getUInt (i); i++;
	std::string whichOffsetTemp = rsetFed->getString (i); i++;
	unsigned int l1AcceptCoarseDelay = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int l1AcceptFineDelay = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int brcstStrTwoCoarseDelay = (unsigned int)rsetFed->getUInt (i); i++;
        unsigned int brcstStrTwoFineDelay = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int dllPumpCurrent = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int pllPumpCurrent = (unsigned int)rsetFed->getUInt (i); i++;
        unsigned int iacId = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int i2cId = (unsigned int)rsetFed->getUInt (i); i++;
	std::string hammingChecking = rsetFed->getString (i); i++;
	std::string bunchCounter = rsetFed->getString (i); i++;
	std::string eventCounter = rsetFed->getString (i); i++;
	std::string clock40DeskwedTwo = rsetFed->getString (i); i++;
	std::string clock40DeskwedTwoOutput = rsetFed->getString (i); i++;
	std::string clockL1AcceptOutput = rsetFed->getString (i); i++;
	std::string parallelOutputBus = rsetFed->getString (i); i++;
	std::string serialBOutput = rsetFed->getString (i); i++;
	std::string nonDeskwedClock40Output = rsetFed->getString (i); i++;
	unsigned int feFpgaId = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int idFeFpga = (unsigned int)rsetFed->getUInt (i); i++;
	std::string feFpgaValuesState = rsetFed->getString (i); i++;
	unsigned int maxTempCritical = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int maxTempFpga = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int maxTempLm82 = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int optoRxInputOffset = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int optoRxOutputOffset = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int optoRxCapacitor = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int channelPairId = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int idChannelPair = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int fakeEventRandomSeed = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int fakeEventRandomMask = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int channelId = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int idChannel = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int coarseDelay = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int fineDelay = (unsigned int)rsetFed->getUInt (i); i++;
        unsigned int threshold = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int trimDacOffset = (unsigned int)rsetFed->getUInt (i); i++;
	std::string adcInputRangeMode = rsetFed->getString (i); i++;
	std::string dataComplement = rsetFed->getString (i); i++;
	unsigned int deviceId = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int idApvFed = (unsigned int)rsetFed->getUInt (i); i++;
	std::string state = rsetFed->getString (i); i++; // 94
	unsigned int medianOverride = (unsigned int)rsetFed->getUInt (i); i++;
	std::string value = rsetFed->getString (i); i ++ ;
	//if (rowsAnalysed == 0) std::cout << "Strips are: " << value << std::endl ;
	unsigned int versionMajorId = (unsigned int)rsetFed->getUInt (i); i++;
	unsigned int versionMinorId = (unsigned int)rsetFed->getUInt (i); i++;
	//endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	//std::cout << "Analysing 1 row tooks: " << (endMillis-startMillis) << " ms" << std::endl ;
	//completeDuration += (endMillis-startMillis) ;
	rowsAnalysed ++ ;

	std::stringstream valueFed ; 
	valueFed << fedId << " | " << idFed << " | " << fedHardId << " | " << name << " | " << crate << " | " << crateSlot << " | " << partitionId
		 << " | " << fedValueState << " | " << fedMode << " | " << superMode << " | " << optoRxResistor << " | " << scopeLength 
		 << " | " << clockSource << " | " << triggerSource << " | " << halBusAdaptor << " | " << readRoute << " | " << testRegister 
		 << " | " << maxTempCriticalBeFpga << " | " << maxTempFpgaBeFpga << " | " << maxTempLm82BeFpga
		 << " | " << feFirmwareVersion << " | " << beFirmwareVersion 
		 << " | " << vmeFirmwareVersion << " | " << delayFirmwareVersion 
		 << " | " << fedVersion << " | " << epromVersion 
		 << " | " << globalClockCoarseShift << " | " << globalClockFineShift
		 << " | " << eventType << " | " << headerType << " | " << fakeEventFile << " | " << fakeEventTriggerDelay << " | " << fov
		 << " | " << standBy << " | " << statusRegisterReset << " | " << min2point5V << " | " << max2point5V
		 << " | " << min3point3V << " | " << max3point3V << " | " << min5V << " | " << max5V << " | " << min12V << " | " << max12V	
		 << " | " << minCoreV << " | " << maxCoreV << " | " << minSupplyV << " | " << maxSupplyV 
		 << " | " << minExternalTemp << " | " << maxExternalTemp << " | " << minInternalTemp << " | " << maxInternalTemp
		 << " | " << offsetTemp << " | " << whichOffsetTemp 
		 << " | " << l1AcceptCoarseDelay << " | " << l1AcceptFineDelay
		 << " | " << brcstStrTwoCoarseDelay << " | " << brcstStrTwoFineDelay
		 << " | " << dllPumpCurrent << " | " << pllPumpCurrent << " | " << iacId << " | " << i2cId
		 << " | " << hammingChecking << " | " << bunchCounter << " | " << eventCounter
		 << " | " << clock40DeskwedTwo << " | " << clock40DeskwedTwoOutput << " | " << clockL1AcceptOutput
		 << " | " << parallelOutputBus << " | " << serialBOutput << " | " << nonDeskwedClock40Output
		 << " | " << feFpgaId << " | " << idFeFpga 
		 << " | " << feFpgaValuesState << " | " << maxTempCritical << " | " << maxTempFpga << " | " << maxTempLm82
		 << " | " << optoRxInputOffset << " | " << optoRxOutputOffset << " | " << optoRxCapacitor
		 << " | " << channelPairId << " | " << idChannelPair
		 << " | " << fakeEventRandomSeed << " | " << fakeEventRandomMask
		 << " | " << channelId << " | " << idChannel
		 << " | " << coarseDelay << " | " << fineDelay << " | " << threshold << " | " << trimDacOffset
		 << " | " << adcInputRangeMode << " | " << dataComplement
		 << " | " << deviceId << " | " << idApvFed
		 << " | " << state << " | " << medianOverride
		 << " | " << versionMajorId << " | " << versionMinorId 
		 << std::endl << value ;

	
	if (rowsAnalysed == 1) std::cout << valueFed.str() << std::endl ;
      }
      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      completeDuration = (endMillis-startMillis) ;
      std::cout << "Analysing " << rowsAnalysed << " row tooks: " << completeDuration << " ms" << std::endl ;
    }
    else std::cerr << "No partition ID for the partition " << partitionName << ", does it exist ?" << std::endl ;
  }
  catch (oracle::occi::SQLException &ex) {

    if (i == 0) {
      std::cerr << "Unable to query the database" << std::endl ;
    }
    else {
      //std::cout << "Failed on " << i << " parameter" << std::endl ;
      std::cerr << "Unable to analyse the " << i << " parameter: " << ex.what() << std::endl ;  
    }
  }

  // Delete statement and result
  if (stmtPid != NULL) {
    if (rsetPid != NULL) stmtPid->closeResultSet (rsetPid);
    dbConnection->terminateStatement(stmtPid); 
  }
  if (stmtFed != NULL) {
    if (rsetFed != NULL) stmtFed->closeResultSet (rsetFed);
    dbConnection->terminateStatement(stmtFed); 
  }

  return 0 ;
}

/** -----------------------------------------------------------
 * Produce a flat XML buffer by identation
 */
int indentXML ( oracle::occi::Connection *dbConnection, std::string partitionName ) {

  std::cout << "Making request for partition " << partitionName << std::endl ;
  unsigned long startMillis, endMillis ;
  unsigned long position = 0, i = 0 ;

  // Statement and result set
  oracle::occi::Statement *stmtPid = NULL ;
  oracle::occi::ResultSet *rsetPid = NULL ;

  oracle::occi::Statement *stmtFedid = NULL ;
  oracle::occi::ResultSet *rsetFedid = NULL ;

  oracle::occi::Statement *stmtFedData = NULL ;
  oracle::occi::ResultSet *rsetFedData = NULL ;

  try {
    // ---------------------------------------------
    // -------------- retreive the partitionId
    static std::string sqlQueryPid = "SELECT Partition.partitionId, StateHistory.fedVersionMajorId, StateHistory.fedVersionMinorId FROM Partition,StateHistory,CurrentState WHERE Partition.partitionName = :a AND Partition.partitionId=StateHistory.partitionId AND StateHistory.stateHistoryId = CurrentState.stateHistoryId" ;
    unsigned int partitionId = 0 ;
    unsigned int versionMajorId = 0 ;
    unsigned int versionMinorId = 0 ;

    std::cout << sqlQueryPid << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    stmtPid = dbConnection->createStatement (sqlQueryPid);
    stmtPid->setString(1,partitionName) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "The preparation of the request tooks: " << (endMillis-startMillis) << " ms" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    rsetPid = stmtPid->executeQuery ();
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "The executeQuery tooks: " << (endMillis-startMillis) << " ms" << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    while (rsetPid->next ()) {
      partitionId = (unsigned int)rsetPid->getUInt (1);
      versionMajorId = (unsigned int)rsetPid->getUInt (2);
      versionMinorId = (unsigned int)rsetPid->getUInt (3);
    }
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    if (partitionId != 0) {
      std::cout << "Retreive partition ID " << partitionId << " version " << versionMajorId << "." << versionMinorId << " for partition " << partitionName << " tooks: " << (endMillis-startMillis) << " ms" << std::endl ;
    }
    else {
      std::cerr << "No partition ID for partition " << partitionName << std::endl ;
      return -1 ;
    }

    // ---------------------------------------------------------------------
    // Find all FED ID
    position = 1 ;
    static std::string sqlQueryFedId = "SELECT fedId FROM Fed WHERE Fed.partitionId = :paramPartitionId" ;
    unsigned int fedIdList[500] = {0} ;

    std::cout << sqlQueryFedId << std::endl ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    stmtFedid = dbConnection->createStatement (sqlQueryFedId);
    stmtFedid->setUInt(1,partitionId) ;
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "The preparation of the request tooks: " << (endMillis-startMillis) << " ms" << std::endl ;

    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    rsetFedid = stmtFedid->executeQuery ();
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "The executeQuery tooks: " << (endMillis-startMillis) << " ms" << std::endl ;

    unsigned long rowsAnalysed = 0 ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    while (rsetFedid->next ()) {
      fedIdList[rowsAnalysed] = (unsigned int)rsetFedid->getUInt (1);
      rowsAnalysed ++ ;
    }
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Analysing " << rowsAnalysed << " row tooks: " << (endMillis-startMillis) << " ms" << std::endl ;

    // ---------------------------------------------------------------------
    // For each FED find the data for FED only
    static std::string sqlQueryFedData = "SELECT id, fedHardId, name, crate, crateSlot, state, fedMode, superMode, optoRxResistor, scopeLength, clockSource, triggerSource, halBusAdaptor, readRoute, testRegister, maxTempCriticalBeFpga, maxTempFpgaBeFpga, maxTempLm82BeFpga, feFirmwareVersion, beFirmwareVersion, vmeFirmwareVersion, delayFirmwareVersion, fedVersion, epromVersion, globalClockCoarseShift, globalClockFineShift, eventType, headerType, fakeEventFile, fakeEventTriggerDelay, fov FROM ViewFed where fedId = :a and versionMajorId = :paramVersionMajorId and versionMinorId = :paramVersionMinorId" ;
    std::cout << sqlQueryFedData << std::endl ;

    rowsAnalysed = 0 ;
    unsigned int fedListCounter = 0 ;
    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    position = 2 ;
    while (fedIdList[fedListCounter] != 0) {

      //std::cout << "FED " << fedIdList[fedListCounter] << " version " << versionMajorId << "." << versionMinorId << std::endl ;
      //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      stmtFedData = dbConnection->createStatement (sqlQueryFedData) ;
      stmtFedData->setUInt(1,fedIdList[fedListCounter]) ;
      stmtFedData->setUInt(2,versionMajorId) ;
      stmtFedData->setUInt(3,versionMinorId) ;
      //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      //std::cout << "The preparation of the request tooks: " << (endMillis-startMillis) << " ms" << std::endl ;
      
      //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      rsetFedData = stmtFedData->executeQuery ();
      //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      //std::cout << "The executeQuery tooks: " << (endMillis-startMillis) << " ms" << std::endl ;

      //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      i = 1 ;
      while (rsetFedData->next ()) {
	unsigned int id = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int fedHardId = (unsigned int)rsetFedData->getUInt (i); i++;
	std::string name = rsetFedData->getString (i); i++;
	unsigned int crate = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int crateSlot = (unsigned int)rsetFedData->getUInt (i); i++;
	std::string state = rsetFedData->getString (i); i++;
	std::string fedMode = rsetFedData->getString (i); i++;
	std::string superMode = rsetFedData->getString (i); i++;
	unsigned int optoRxResistor = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int scopeLength = (unsigned int)rsetFedData->getUInt (i); i++;
	std::string clockSource = rsetFedData->getString (i); i++;
	std::string triggerSource = rsetFedData->getString (i); i++;
	std::string halBusAdaptor = rsetFedData->getString (i); i++;
	std::string readRoute = rsetFedData->getString (i); i++;
	unsigned int testRegister = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int maxTempCriticalBeFpga = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int maxTempFpgaBeFpga = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int maxTempLm82BeFpga = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int feFirmwareVersion = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int beFirmwareVersion = (unsigned int)rsetFedData->getUInt (i); i++;
	std::string vmeFirmwareVersion = rsetFedData->getString (i); i++;
	std::string delayFirmwareVersion = rsetFedData->getString (i); i++;
	unsigned int fedVersion = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int epromVersion = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int globalClockCoarseShift = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int globalClockFineShift = (unsigned int)rsetFedData->getUInt (i); i++;
	unsigned int eventType = (unsigned int)rsetFedData->getUInt (i); i++;
	std::string headerType = rsetFedData->getString (i); i++;
	std::string fakeEventFile = rsetFedData->getString (i); i++;
	std::string fakeEventTriggerDelay = rsetFedData->getString (i); i++;
	unsigned int fov = (unsigned int)rsetFedData->getUInt (i); i++;
	rowsAnalysed ++ ;

	if (rowsAnalysed == 1) {
	  std::stringstream values ; 
	  values << "| " << id << " | " << fedHardId << " | " << name << " | " << crate 
		 << " | " << crateSlot << " | " << state << " | " << fedMode 
		 << " | " << superMode << " | " << optoRxResistor 
		 << " | " << scopeLength << " | " << clockSource 
		 << " | " << triggerSource << " | " << halBusAdaptor 
		 << " | " << readRoute << " | " << testRegister << " | " << maxTempCriticalBeFpga 
		 << " | " << maxTempFpgaBeFpga << " | " << maxTempLm82BeFpga 
		 << " | " << feFirmwareVersion << " | " << beFirmwareVersion 
		 << " | " << vmeFirmwareVersion << " | " << delayFirmwareVersion 
		 << " | " << fedVersion << " | " << epromVersion << " | " << globalClockCoarseShift 
		 << " | " << globalClockFineShift << " | " << eventType << " | " << headerType 
		 << " | " << fakeEventFile << " | " << fakeEventTriggerDelay << " | " << fov ;
	  std::cout << values.str() << std::endl ;
	}
      }
      //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      //std::cout << "Analysing " << rowsAnalysed << " row for the FED " << fedIdList[fedListCounter] << " tooks: " << (endMillis-startMillis) << " ms" << std::endl ;

      //startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();      
      stmtFedData->closeResultSet (rsetFedData);
      dbConnection->terminateStatement(stmtFedData); 
      //endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
      //std::cout << "Deleting the statement tooks " << (endMillis-startMillis) << " ms" << std::endl ;

      fedListCounter ++ ;
    }
    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    std::cout << "Complete request tooks " << (endMillis-startMillis) << " ms" << std::endl ;
  }
  catch (oracle::occi::SQLException &ex) {

    switch (position) {
    case 0:
      std::cerr << "Unable to query the database for partition ID: " ;
      break ;
    case 1:
      std::cerr << "Unable to query the database for FED id: " ;
    case 2:
      std::cerr << "Unable to analyse the " << i << " parameter: " << ex.what() << std::endl ;  
      break ;
    }
    std::cerr << ex.what() << std::endl ;
  }

  // Delete statement and result
  if (stmtPid != NULL) {
    if (rsetPid != NULL) stmtPid->closeResultSet (rsetPid);
    dbConnection->terminateStatement(stmtPid); 
  }
  if (stmtFedid != NULL) {
    if (rsetFedid != NULL) stmtFedid->closeResultSet (rsetFedid);
    dbConnection->terminateStatement(stmtFedid); 
  }

  return 0 ;
}
