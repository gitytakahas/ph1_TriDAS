#include "ErrorsChecker.h"


ErrorsChecker::ErrorsChecker(void)
{
}
	
ErrorsChecker::~ErrorsChecker(void)
{
}



void ErrorsChecker::getCrateAndFecSlotsForFechardid(	unsigned int * crateId,
														unsigned int * fecSlot,
														TkDiagErrorAnalyser* tkDiagErrorAnalyser,
														std::string fecHardId)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getListOfModulesAsConnection();
	    for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if (connection->getFecHardwareId() == fecHardId)
			{
				*crateId = connection->getFecCrateId();
				*fecSlot = connection->getFecSlot();
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}



void ErrorsChecker::getCrateAndFedSlotsForFedid(	unsigned int * crateId,
															unsigned int * fedSlot,
															TkDiagErrorAnalyser* tkDiagErrorAnalyser,
															unsigned int fedId)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getListOfModulesAsConnection();
	    for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if (connection->getFedId() == fedId)
			{
				*crateId = connection->getFedCrateId();
				*fedSlot = connection->getFedSlot();
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}



unsigned int ErrorsChecker::getPsuIdRange(	unsigned int PsuId,
											TkDiagErrorAnalyser* tkDiagErrorAnalyser)
{
	try
	{
	    std::vector<unsigned int> listDetId = tkDiagErrorAnalyser->getListOfDetId() ;
		unsigned int counter = 0;
	    for (std::vector<unsigned int>::iterator it = listDetId.begin() ; it != listDetId.end() ; it ++)
		{
			counter++;
			if ( (*it) == PsuId ) return counter;
	   	}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
	return 0;
}



void ErrorsChecker::getFecViewErrorsForDetector(	unsigned int * fecErrors,
														unsigned int * fedErrors,
														unsigned int * psuErrors,
														TkDiagErrorAnalyser* tkDiagErrorAnalyser)
{
	try
	{
		ConnectionVector connections = tkDiagErrorAnalyser->getListOfModulesAsConnection();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
      		ConnectionDescription *connection = *it ;

      		tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
															connection->getRingSlot(),
															connection->getCcuAddress(),
															connection->getI2cChannel(),
															fecErrorCounter,
															fedErrorCounter,
															psuErrorCounter);

			*fecErrors += fecErrorCounter;
			*fedErrors += fedErrorCounter;
			*psuErrors += psuErrorCounter;
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}






void ErrorsChecker::getFecViewErrorsForCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getListOfModulesAsConnection();
	    for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;
			if (connection->getFecCrateId() == fecCrateNumber)
			{
				tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}







void ErrorsChecker::getFecViewErrorsForSlot(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int fecCrateNumber,
									unsigned int fecSlotNumber,
									std::string & fecHardId)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getListOfModulesAsConnection();
	    for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber) && (connection->getFecSlot() == fecSlotNumber) )
			{
				tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				fecHardId = connection->getFecHardwareId();
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}

	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}






void ErrorsChecker::getFecViewErrorsForPmc(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int fecCrateNumber,
									unsigned int fecSlotNumber,
									unsigned int fecPmcNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getListOfModulesAsConnection();
	    for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber) && (connection->getFecSlot() == fecSlotNumber) 
				&& (connection->getRingSlot() == fecPmcNumber)	)
			{
				tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}
	
	
	
	

void ErrorsChecker::getFecViewErrorsForCcu(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int fecCrateNumber,
									unsigned int fecSlotNumber,
									unsigned int fecPmcNumber,
									unsigned int fecCcuNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getListOfModulesAsConnection();
	    for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;
			if ( 	(connection->getFecCrateId() == fecCrateNumber)
					&& (connection->getFecSlot() == fecSlotNumber) 
					&& (connection->getRingSlot() == fecPmcNumber)
					&& (connection->getCcuAddress() == fecCcuNumber) )
			{
				tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}

}
	
	
	
	

void ErrorsChecker::getFecViewErrorsForChannel(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser,
										unsigned int fecCrateNumber,
										unsigned int fecSlotNumber,
										unsigned int fecPmcNumber,
										unsigned int fecCcuNumber,
										unsigned int fecChannelNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getListOfModulesAsConnection();
	    for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;
			if ( 	(connection->getFecCrateId() == fecCrateNumber)
					&& (connection->getFecSlot() == fecSlotNumber) 
					&& (connection->getRingSlot() == fecPmcNumber)
					&& (connection->getCcuAddress() == fecCcuNumber)
					&& (connection->getI2cChannel() == fecChannelNumber) )
			{
				tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}
	
	
	
	

void ErrorsChecker::getFecViewErrorsForI2cAddress(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser,
										unsigned int fecCrateNumber,
										unsigned int fecSlotNumber,
										unsigned int fecPmcNumber,
										unsigned int fecCcuNumber,
										unsigned int fecChannelNumber,
										unsigned int fecI2cAddress)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		unsigned int i2cAddress = 0x0;
		if (fecI2cAddress == 0) i2cAddress=0x20;
		else if (fecI2cAddress == 1) i2cAddress=0x22;
		else if (fecI2cAddress == 2) i2cAddress=0x24;
	    for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;


			if ( 	(connection->getFecCrateId() == fecCrateNumber)
					&& (connection->getFecSlot() == fecSlotNumber) 
					&& (connection->getRingSlot() == fecPmcNumber)
					&& (connection->getCcuAddress() == fecCcuNumber)
					&& (connection->getI2cChannel() == fecChannelNumber)
					&& (connection->getApvAddress() == i2cAddress) )
			{
				tkDiagErrorAnalyser->getConnectionErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																i2cAddress,
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;

			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}

}
	
	


void ErrorsChecker::getFecCratesList(bool * fecCratesList, TkDiagErrorAnalyser* tkDiagErrorAnalyser)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FEC_CRATES; i++) *(fecCratesList+i)=false;
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			*(fecCratesList + connection->getFecCrateId()) = true;
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}







void ErrorsChecker::getFecSlotsListForOneCrate(bool * fecSlotsList, TkDiagErrorAnalyser* tkDiagErrorAnalyser, unsigned int fecCrateNumber)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FEC_SLOTS; i++) *(fecSlotsList+i)=false;
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if (connection->getFecCrateId() == fecCrateNumber)
			{
				*(fecSlotsList + connection->getFecSlot()) = true;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getFecRingListForOneSlot(bool * fecPmcList, TkDiagErrorAnalyser* tkDiagErrorAnalyser, unsigned int fecCrateNumber, unsigned int fecSlotNumber)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FEC_RING; i++) *(fecPmcList+i)=false;
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber) && (connection->getFecSlot() == fecSlotNumber) )
			{
				*(fecPmcList + connection->getRingSlot()) = true;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}



void ErrorsChecker::getFecCcuListForOnePmc( bool * fecCcuList, TkDiagErrorAnalyser* tkDiagErrorAnalyser,
												unsigned int fecCrateNumber,
												unsigned int fecSlotNumber,
												unsigned int fecPmcNumber)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FEC_CCU; i++) *(fecCcuList+i)=false;
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( 	(connection->getFecCrateId() == fecCrateNumber)
					&& (connection->getFecSlot() == fecSlotNumber)
					&& (connection->getRingSlot() == fecPmcNumber) )
			{
				*(fecCcuList + connection->getCcuAddress()) = true;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}



void ErrorsChecker::getFecChannelListForOneCcu( bool * fecChannelList, TkDiagErrorAnalyser* tkDiagErrorAnalyser,
												unsigned int fecCrateNumber,
												unsigned int fecSlotNumber,
												unsigned int fecPmcNumber,
												unsigned int fecCcuNumber)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FEC_CHANNEL; i++) *(fecChannelList+i)=false;
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( 	(connection->getFecCrateId() == fecCrateNumber)
					&& (connection->getFecSlot() == fecSlotNumber)
					&& (connection->getRingSlot() == fecPmcNumber)
					&& (connection->getCcuAddress() == fecCcuNumber) )
			{
				*(fecChannelList + connection->getI2cChannel()) = true;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}



void ErrorsChecker::getFecI2cAddressListForOneChannel( bool * fecI2cAddrList, TkDiagErrorAnalyser* tkDiagErrorAnalyser,
												unsigned int fecCrateNumber,
												unsigned int fecSlotNumber,
												unsigned int fecPmcNumber,
												unsigned int fecCcuNumber,
												unsigned int fecChannelNumber)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FEC_I2CADDR; i++) *(fecI2cAddrList+i)=false;
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( 	(connection->getFecCrateId() == fecCrateNumber)
					&& (connection->getFecSlot() == fecSlotNumber)
					&& (connection->getRingSlot() == fecPmcNumber)
					&& (connection->getCcuAddress() == fecCcuNumber)
					&& (connection->getI2cChannel() == fecChannelNumber) )
			{
				int index = -1;
				if (connection->getApvAddress() == 0x20) index=0;
				else if (connection->getApvAddress() == 0x22) index=1;
				else if (connection->getApvAddress() == 0x24) index=2;
				if (index >= 0) *(fecI2cAddrList + index) = true;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}







void ErrorsChecker::getFedViewErrorsForDetector(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser)
{
	try
	{
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFed();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
      		ConnectionDescription *connection = *it ;
			tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
															connection->getFedSlot(),
															connection->getFedChannel(),
															fecErrorCounter,
															fedErrorCounter,
															psuErrorCounter);
			*fecErrors += fecErrorCounter;
			*fedErrors += fedErrorCounter;
			*psuErrors += psuErrorCounter;
		}

	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}	
}








void ErrorsChecker::getFedErrorsForOneFecCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if (connection->getFecCrateId() == fecCrateNumber)
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
																connection->getFedSlot(),
																connection->getFedChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}







void ErrorsChecker::getFedErrorsForOneFecSlot(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber) && (connection->getFecSlot() == fecSlotNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
																connection->getFedSlot(),
																connection->getFedChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getFedErrorsForOneFecPmc(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber) && (connection->getFecSlot() == fecSlotNumber)
			 && (connection->getRingSlot() == fecPmcNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
																connection->getFedSlot(),
																connection->getFedChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getFedErrorsForOneFecCcu(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber,
													unsigned int fecCcuNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber)
				&& (connection->getFecSlot() == fecSlotNumber)
			 	&& (connection->getRingSlot() == fecPmcNumber)
				&& (connection->getCcuAddress() == fecCcuNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
																connection->getFedSlot(),
																connection->getFedChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}






void ErrorsChecker::getFedErrorsForOneFecChannel(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber,
													unsigned int fecCcuNumber,
													unsigned int fecChannelNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber)
				&& (connection->getFecSlot() == fecSlotNumber)
			 	&& (connection->getRingSlot() == fecPmcNumber)
				&& (connection->getCcuAddress() == fecCcuNumber)
				&& (connection->getI2cChannel() == fecChannelNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
																connection->getFedSlot(),
																connection->getFedChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getFedCratesList(bool * fedCratesList, TkDiagErrorAnalyser* tkDiagErrorAnalyser)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FED_CRATES; i++) *(fedCratesList+i)=false;
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFed();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			*(fedCratesList + connection->getFedCrateId()) = true;
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}







void ErrorsChecker::getFedViewErrorsForCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFed();
	    for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;
			if (connection->getFedCrateId() == fedCrateNumber)
			{
				tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
																connection->getFedSlot(),
																connection->getFedChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getFecErrorsForOneFedCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if (connection->getFedCrateId() == fedCrateNumber)
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
	
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}




void ErrorsChecker::getFedSlotsListForOneCrate(bool * fedSlotsList, TkDiagErrorAnalyser* tkDiagErrorAnalyser, unsigned int fedCrateNumber)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FED_SLOTS; i++) *(fedSlotsList+i)=false;
		ConnectionVector fedconnections = tkDiagErrorAnalyser->getConnectionByFed();
		for (ConnectionVector::iterator it = fedconnections.begin() ; it != fedconnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if (connection->getFedCrateId() == fedCrateNumber)
			{
				*(fedSlotsList + connection->getFedSlot()) = true;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}



void ErrorsChecker::getFedViewErrorsForSlot(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int fedCrateNumber,
									unsigned int fedSlotNumber,
									unsigned int * fedId)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFed();
	    for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;
			if ( (connection->getFedCrateId() == fedCrateNumber) && (connection->getFedSlot() == fedSlotNumber) )
			{
				tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
																connection->getFedSlot(),
																connection->getFedChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fedId = connection->getFedId();
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}

	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}



void ErrorsChecker::getFecErrorsForOneFedSlot(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber,
													unsigned int fedSlotNumber)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFed();
		for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFedCrateId() == fedCrateNumber) && (connection->getFedSlot() == fedSlotNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}




void ErrorsChecker::getFedChannelsListForOneSlot(bool * fedConnectionList, TkDiagErrorAnalyser* tkDiagErrorAnalyser, unsigned int fedCrateNumber, unsigned int fedSlotNumber)
{
	try
	{
		for (int i=0; i<NUMBER_OF_FED_CHANNELS; i++) *(fedConnectionList+i)=false;
		ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFed();
		for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFedCrateId() == fedCrateNumber) && (connection->getFedSlot() == fedSlotNumber) )
			{
				*(fedConnectionList + connection->getFedChannel()) = true;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getFedViewErrorsForChannel(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int fedCrateNumber,
									unsigned int fedSlotNumber,
									unsigned int fedChannelNumber)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFed();
	    for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			ConnectionDescription *connection = *it;
			if ( (connection->getFedCrateId() == fedCrateNumber) && (connection->getFedSlot() == fedSlotNumber) 
				&& (connection->getFedChannel() == fedChannelNumber)	)
			{
				tkDiagErrorAnalyser->getFedChannelErrorCounter( connection->getFedCrateId(),
																connection->getFedSlot(),
																connection->getFedChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}




void ErrorsChecker::getFecErrorsForOneFedChannel(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber,
													unsigned int fedSlotNumber,
													unsigned int fedChannelNumber)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFed();
		for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFedCrateId() == fedCrateNumber) && (connection->getFedSlot() == fedSlotNumber)
			 && (connection->getFedChannel() == fedChannelNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getFecModuleErrorCounter(	connection->getFecHardwareId(),
																connection->getRingSlot(),
																connection->getCcuAddress(),
																connection->getI2cChannel(),
																fecErrorCounter,
																fedErrorCounter,
																psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}










void ErrorsChecker::getPsuViewErrorsForDetector(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser)
{
	try
	{
		std::vector<unsigned int> listDetId = tkDiagErrorAnalyser->getListOfDetId();
		for (std::vector<unsigned int>::iterator it = listDetId.begin() ; it != listDetId.end() ; it ++)
		{
			unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
			tkDiagErrorAnalyser->getDetIdErrorCounter(*it,fecErrorCounter,fedErrorCounter,psuErrorCounter);
			*fecErrors += fecErrorCounter;
			*fedErrors += fedErrorCounter;
			*psuErrors += psuErrorCounter;
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}	
}




void ErrorsChecker::getPsuErrorsForOneFecCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if (connection->getFecCrateId() == fecCrateNumber)
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter,fedErrorCounter,psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}




void ErrorsChecker::getPsuErrorsForOneFecSlot(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber) && (connection->getFecSlot() == fecSlotNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter,fedErrorCounter,psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getPsuErrorsForOneFecPmc(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber) && (connection->getFecSlot() == fecSlotNumber) 
			&& (connection->getRingSlot() == fecPmcNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter,fedErrorCounter,psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getPsuErrorsForOneFecCcu(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber,
													unsigned int fecCcuNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber)
				&& (connection->getFecSlot() == fecSlotNumber) 
				&& (connection->getRingSlot() == fecPmcNumber)
				&& (connection->getCcuAddress() == fecCcuNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter,fedErrorCounter,psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}







void ErrorsChecker::getPsuErrorsForOneFecChannel(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber,
													unsigned int fecCcuNumber,
													unsigned int fecChannelNumber)
{
	try
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFecCrateId() == fecCrateNumber)
				&& (connection->getFecSlot() == fecSlotNumber) 
				&& (connection->getRingSlot() == fecPmcNumber)
				&& (connection->getCcuAddress() == fecCcuNumber)
				&& (connection->getI2cChannel() == fecChannelNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter,fedErrorCounter,psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}




void ErrorsChecker::getPsuErrorsForOneFedCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFec();
		for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if (connection->getFedCrateId() == fedCrateNumber)
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter,fedErrorCounter,psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





void ErrorsChecker::getPsuErrorsForOneFedSlot(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber,
													unsigned int fedSlotNumber)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFed();
		for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFedCrateId() == fedCrateNumber) && (connection->getFedSlot() == fedSlotNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter,fedErrorCounter,psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}






void ErrorsChecker::getPsuErrorsForOneFedChannel(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber,
													unsigned int fedSlotNumber,
													unsigned int fedChannelNumber)
{
	try
	{
		ConnectionVector fedConnections = tkDiagErrorAnalyser->getConnectionByFed();
		for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
		{
			ConnectionDescription *connection = *it;
			if ( (connection->getFedCrateId() == fedCrateNumber) && (connection->getFedSlot() == fedSlotNumber) 
			&& (connection->getFedChannel() == fedChannelNumber) )
			{
				unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
				tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter,fedErrorCounter,psuErrorCounter);
				*fecErrors += fecErrorCounter;
				*fedErrors += fedErrorCounter;
				*psuErrors += psuErrorCounter;
			}
		}
	}
	catch (std::string &e) {}
	catch (oracle::occi::SQLException &e) {}
	catch (FecExceptionHandler &e) {}
}





