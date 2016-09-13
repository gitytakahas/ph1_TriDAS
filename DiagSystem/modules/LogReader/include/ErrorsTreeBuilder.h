#ifndef _ErrorsTreeBuilder_h_
#define _ErrorsTreeBuilder_h_

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>


#define UNDEFINEDPARAMETER	0xFFFFFFFF

//All numbers are +1
#define ETB_NUMBER_OF_FEC_CRATES	5
#define ETB_NUMBER_OF_FEC_SLOTS		22
#define ETB_NUMBER_OF_FEC_RING		9
#define ETB_NUMBER_OF_FEC_CCU		0x80
#define ETB_NUMBER_OF_FEC_CHANNEL	0x20
#define ETB_NUMBER_OF_FEC_I2CPOSSIBLEADD	10

//All numbers are +1
#define ETB_NUMBER_OF_FED_CRATES	31
#define ETB_NUMBER_OF_FED_SLOTS		22
#define ETB_NUMBER_OF_FED_CHANNELS	97


#define ETB_NUMBER_OF_PSU_DETID			1000


//Build tree for FEC
typedef struct fecI2cAddresses {
unsigned int errCounter;
};

typedef struct fecChannels {
struct fecI2cAddresses fecI2cAddress[ETB_NUMBER_OF_FEC_I2CPOSSIBLEADD];
unsigned int errCounter;
};

typedef struct fecCcus {
struct fecChannels fecChannel[ETB_NUMBER_OF_FEC_CHANNEL];
unsigned int errCounter;
};

typedef struct fecRings {
struct fecCcus fecCcu[ETB_NUMBER_OF_FEC_CCU];
unsigned int errCounter;
};

typedef struct fecSlots {
struct fecRings fecRing[ETB_NUMBER_OF_FEC_RING];
unsigned int errCounter;
};

typedef struct fecCrates {
struct fecSlots fecSlot[ETB_NUMBER_OF_FEC_SLOTS];
unsigned int errCounter;
};

typedef struct FecNodeTree {
struct fecCrates fecCrate[ETB_NUMBER_OF_FEC_CRATES];
unsigned int errCounter;
};



//Build tree for FED
typedef struct fedChannels {
unsigned int errCounter;
};

typedef struct fedSlots {
struct fedChannels fedChannel[ETB_NUMBER_OF_FED_CHANNELS];
unsigned int errCounter;
};

typedef struct fedCrates {
struct fedSlots fedSlot[ETB_NUMBER_OF_FED_SLOTS];
unsigned int errCounter;
};

typedef struct FedNodeTree {
struct fedCrates fedCrate[ETB_NUMBER_OF_FED_CRATES];
unsigned int errCounter;
};




//Build tree for PSUs
typedef struct psuIds {
unsigned int errCounter;
};

typedef struct PsuNodeTree {
struct psuIds psuId[ETB_NUMBER_OF_PSU_DETID];
unsigned int errCounter;
};





class ErrorsTreeBuilder
{
public:
	struct FecNodeTree fecNodeTree;
	struct FedNodeTree fedNodeTree;
	struct PsuNodeTree psuNodeTree;


	ErrorsTreeBuilder(void);

	~ErrorsTreeBuilder(void);
	
	void printAlive(void);


	void initArrays(void);

//Build FEC NODE TREE access tools
	void setFecNodeError(void);
	unsigned int getFecNodeError(void);

	void setFecNodeError(unsigned int crateNumber);
	unsigned int getFecNodeError(unsigned int crateNumber);

	void setFecNodeError(unsigned int crateNumber, unsigned int slotNumber);
	unsigned int getFecNodeError(unsigned int crateNumber, unsigned int slotNumber);

	void setFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber);
	unsigned int getFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber);

	void setFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber);
	unsigned int getFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber);

	void setFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber, unsigned int channelNumber);
	unsigned int getFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber, unsigned int channelNumber);

	void setFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber, unsigned int channelNumber, unsigned int i2cNumber);
	unsigned int getFecNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int ringNumber, unsigned int ccuNumber, unsigned int channelNumber, unsigned int i2cNumber);



//Build FED NODE TREE access tools
	void setFedNodeError(void);
	unsigned int getFedNodeError(void);

	void setFedNodeError(unsigned int crateNumber);
	unsigned int getFedNodeError(unsigned int crateNumber);

	void setFedNodeError(unsigned int crateNumber, unsigned int slotNumber);
	unsigned int getFedNodeError(unsigned int crateNumber, unsigned int slotNumber);

	void setFedNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int channelNumber);
	unsigned int getFedNodeError(unsigned int crateNumber, unsigned int slotNumber, unsigned int channelNumber);


//Build PSU NODE TREE access tools
	void setPsuNodeError(void);
	unsigned int getPsuNodeError(void);

	void setPsuNodeError(unsigned int psuIdNumber);
	unsigned int getPsuNodeError(unsigned int psuIdNumber);









};

#endif





































