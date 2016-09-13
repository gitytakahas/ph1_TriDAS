#include "ErrorsTreeBuilder.h"


ErrorsTreeBuilder::ErrorsTreeBuilder(void)
{
}
	
ErrorsTreeBuilder::~ErrorsTreeBuilder(void)
{
}



void ErrorsTreeBuilder::printAlive(void)
{
	std::cout << "ErrorsTreeBuilder instance is alive" << std::endl;
}



void ErrorsTreeBuilder::initArrays(void)
{


	fecNodeTree.errCounter = 0;
	for (int i=0; i<ETB_NUMBER_OF_FEC_CRATES; i++)
	{
		fecNodeTree.fecCrate[i].errCounter = 0;
		for (int j=0; j<ETB_NUMBER_OF_FEC_SLOTS; j++)
		{
			fecNodeTree.fecCrate[i].fecSlot[j].errCounter = 0;
			for (int k=0; k<ETB_NUMBER_OF_FEC_RING; k++)
			{
				fecNodeTree.fecCrate[i].fecSlot[j].fecRing[k].errCounter = 0;
				for (int l=0; l<ETB_NUMBER_OF_FEC_CCU; l++)
				{
					fecNodeTree.fecCrate[i].fecSlot[j].fecRing[k].fecCcu[l].errCounter = 0;
					for (int m=0; m<ETB_NUMBER_OF_FEC_CHANNEL; m++)
					{
						fecNodeTree.fecCrate[i].fecSlot[j].fecRing[k].fecCcu[l].fecChannel[m].errCounter = 0;
						for (int n=0; n<ETB_NUMBER_OF_FEC_I2CPOSSIBLEADD; n++)
						{
							fecNodeTree.fecCrate[i].fecSlot[j].fecRing[k].fecCcu[l].fecChannel[m].fecI2cAddress[n].errCounter = 0;
						}
					}
				}
			}
		}
	}
//	std::cout << "ErrorsTreeBuilder::FecNode array initialized" << std::endl;



	fedNodeTree.errCounter = 0;
	for (int i=0; i<ETB_NUMBER_OF_FED_CRATES; i++)
	{
		fedNodeTree.fedCrate[i].errCounter = 0;
		for (int j=0; j<ETB_NUMBER_OF_FED_SLOTS; j++)
		{
			fedNodeTree.fedCrate[i].fedSlot[j].errCounter = 0;
			for (int k=0; k<ETB_NUMBER_OF_FED_CHANNELS; k++)
			{
				fedNodeTree.fedCrate[i].fedSlot[j].fedChannel[k].errCounter = 0;
			}
		}
	}
//	std::cout << "ErrorsTreeBuilder::FedNode array initialized" << std::endl;


	psuNodeTree.errCounter = 0;
	for (int i=0; i<ETB_NUMBER_OF_PSU_DETID; i++)
	{
		psuNodeTree.psuId[i].errCounter = 0;
	}
//	std::cout << "ErrorsTreeBuilder::PsuNode array initialized" << std::endl;


}



///////////////////////////////////
//Build FEC NODE TREE access tools
///////////////////////////////////

void ErrorsTreeBuilder::setFecNodeError(void)
{
	fecNodeTree.errCounter++;
}
unsigned int ErrorsTreeBuilder::getFecNodeError(void)
{
	return fecNodeTree.errCounter;
}


void ErrorsTreeBuilder::setFecNodeError (unsigned int crateNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES) )
	{
		fecNodeTree.fecCrate[crateNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getFecNodeError (unsigned int crateNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES) )
	{
		return fecNodeTree.fecCrate[crateNumber].errCounter;
	}
	else return 0;
}


void ErrorsTreeBuilder::setFecNodeError(unsigned int crateNumber, unsigned int slotNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS) )
	{
		fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getFecNodeError(unsigned int crateNumber, unsigned int slotNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS) )
	{
		return fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].errCounter;
	}
	else return 0;
}


void ErrorsTreeBuilder::setFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS)
	  && (ringNumber >=0) && (ringNumber < ETB_NUMBER_OF_FEC_RING) )
	{
		fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].fecRing[ringNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS)
	  && (ringNumber >=0) && (ringNumber < ETB_NUMBER_OF_FEC_RING) )
	{
		return fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].fecRing[ringNumber].errCounter;
	}
	else return 0;
}

void ErrorsTreeBuilder::setFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS)
	  && (ringNumber >=0) && (ringNumber < ETB_NUMBER_OF_FEC_RING)
	  && (ccuNumber >=0) && (ccuNumber < ETB_NUMBER_OF_FEC_CCU) )
	{
		fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].fecRing[ringNumber].fecCcu[ccuNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS)
	  && (ringNumber >=0) && (ringNumber < ETB_NUMBER_OF_FEC_RING)
	  && (ccuNumber >=0) && (ccuNumber < ETB_NUMBER_OF_FEC_CCU) )
	{
		return fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].fecRing[ringNumber].fecCcu[ccuNumber].errCounter;
	}
	else return 0;
}

void ErrorsTreeBuilder::setFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber, unsigned int channelNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS)
	  && (ringNumber >=0) && (ringNumber < ETB_NUMBER_OF_FEC_RING)
	  && (ccuNumber >=0) && (ccuNumber < ETB_NUMBER_OF_FEC_CCU)
	  && (channelNumber >=0) && (channelNumber < ETB_NUMBER_OF_FEC_CHANNEL) )
	{
		fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].fecRing[ringNumber].fecCcu[ccuNumber].fecChannel[channelNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber, unsigned int channelNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS)
	  && (ringNumber >=0) && (ringNumber < ETB_NUMBER_OF_FEC_RING)
	  && (ccuNumber >=0) && (ccuNumber < ETB_NUMBER_OF_FEC_CCU)
	  && (channelNumber >=0) && (channelNumber < ETB_NUMBER_OF_FEC_CHANNEL) )
	{
		return fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].fecRing[ringNumber].fecCcu[ccuNumber].fecChannel[channelNumber].errCounter;
	}
	else return 0;
}

void ErrorsTreeBuilder::setFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber, unsigned int channelNumber, unsigned int i2cNumber)
{
	int lclRef = -1;
	if ( (i2cNumber == 0x20) || (i2cNumber == 0x21) ) lclRef=0;
	else if ( (i2cNumber == 0x22) || (i2cNumber == 0x23) ) lclRef=1;
	else if ( (i2cNumber == 0x24) || (i2cNumber == 0x25) ) lclRef=2;

	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS)
	  && (ringNumber >=0) && (ringNumber < ETB_NUMBER_OF_FEC_RING)
	  && (ccuNumber >=0) && (ccuNumber < ETB_NUMBER_OF_FEC_CCU)
	  && (channelNumber >=0) && (channelNumber < ETB_NUMBER_OF_FEC_CHANNEL)
	  && (lclRef >=0) && (lclRef < 3) )
	 {
		fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].fecRing[ringNumber].fecCcu[ccuNumber].fecChannel[channelNumber].fecI2cAddress[lclRef].errCounter++;
	 }
}



unsigned int ErrorsTreeBuilder::getFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber, unsigned int channelNumber, unsigned int i2cNumber)
{
/*
	int lclRef = -1;
	if ( (i2cNumber == 0x20) || (i2cNumber == 0x21) ) lclRef=0;
	else if ( (i2cNumber == 0x22) || (i2cNumber == 0x23) ) lclRef=1;
	else if ( (i2cNumber == 0x24) || (i2cNumber == 0x25) ) lclRef=2;
*/
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FEC_CRATES)
	  && (slotNumber >=0) && (slotNumber < ETB_NUMBER_OF_FEC_SLOTS)
	  && (ringNumber >=0) && (ringNumber < ETB_NUMBER_OF_FEC_RING)
	  && (ccuNumber >=0) && (ccuNumber < ETB_NUMBER_OF_FEC_CCU)
	  && (channelNumber >=0) && (channelNumber < ETB_NUMBER_OF_FEC_CHANNEL)
	  && (i2cNumber >=0) && (i2cNumber < ETB_NUMBER_OF_FEC_I2CPOSSIBLEADD) )
	{
		return fecNodeTree.fecCrate[crateNumber].fecSlot[slotNumber].fecRing[ringNumber].fecCcu[ccuNumber].fecChannel[channelNumber].fecI2cAddress[i2cNumber].errCounter;
	}
	else return 0;
}





///////////////////////////////////
//Build FED NODE TREE access tools
///////////////////////////////////

void ErrorsTreeBuilder::setFedNodeError(void)
{
	fedNodeTree.errCounter++;
}
unsigned int ErrorsTreeBuilder::getFedNodeError(void)
{
	return fedNodeTree.errCounter;
}


void ErrorsTreeBuilder::setFedNodeError(unsigned int crateNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_CRATES) )
	{
		fedNodeTree.fedCrate[crateNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getFedNodeError(unsigned int crateNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_CRATES) )
	{
		return fedNodeTree.fedCrate[crateNumber].errCounter;
	}
	else return 0;
}


void ErrorsTreeBuilder::setFedNodeError(unsigned int crateNumber, unsigned int slotNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_CRATES)
	  && (slotNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_SLOTS) )
	{
		fedNodeTree.fedCrate[crateNumber].fedSlot[slotNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getFedNodeError(unsigned int crateNumber, unsigned int slotNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_CRATES)
	  && (slotNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_SLOTS) )
	{
		return fedNodeTree.fedCrate[crateNumber].fedSlot[slotNumber].errCounter;
	}
	else return 0;
}


void ErrorsTreeBuilder::setFedNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int channelNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_CRATES)
	  && (slotNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_SLOTS)
	  && (channelNumber >=0) && (channelNumber < ETB_NUMBER_OF_FED_CHANNELS) )
	{
		fedNodeTree.fedCrate[crateNumber].fedSlot[slotNumber].fedChannel[channelNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getFedNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int channelNumber)
{
	if ( (crateNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_CRATES)
	  && (slotNumber >=0) && (crateNumber < ETB_NUMBER_OF_FED_SLOTS)
	  && (channelNumber >=0) && (channelNumber < ETB_NUMBER_OF_FED_CHANNELS) )
	{
		return fedNodeTree.fedCrate[crateNumber].fedSlot[slotNumber].fedChannel[channelNumber].errCounter;
	}
	else return 0;
}



///////////////////////////////////
//Build PSU NODE TREE access tools
///////////////////////////////////

void ErrorsTreeBuilder::setPsuNodeError(void)
{
	psuNodeTree.errCounter++;
}
unsigned int ErrorsTreeBuilder::getPsuNodeError(void)
{
	return psuNodeTree.errCounter;
}


void ErrorsTreeBuilder::setPsuNodeError(unsigned int psuIdNumber)
{
	if ( (psuIdNumber >=0) && (psuIdNumber < ETB_NUMBER_OF_PSU_DETID) )
	{
		psuNodeTree.psuId[psuIdNumber].errCounter++;
	}
}
unsigned int ErrorsTreeBuilder::getPsuNodeError(unsigned int psuIdNumber)
{
	if ( (psuIdNumber >=0) && (psuIdNumber < ETB_NUMBER_OF_PSU_DETID) )
	{
		return psuNodeTree.psuId[psuIdNumber].errCounter;
	}
	else return 0;
}





































