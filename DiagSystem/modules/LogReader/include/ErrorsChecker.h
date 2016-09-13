#ifndef _ErrorsChecker_h_
#define _ErrorsChecker_h_


#include "FecExceptionHandler.h" 
#include "TkDiagErrorAnalyser.h" 


//All numbers are +1
#define NUMBER_OF_FEC_CRATES	4
#define NUMBER_OF_FEC_SLOTS		22
#define NUMBER_OF_FEC_RING		9
#define NUMBER_OF_FEC_CCU		0x80
#define NUMBER_OF_FEC_CHANNEL	0x20
#define NUMBER_OF_FEC_CHANNEL	0x20
#define NUMBER_OF_FEC_I2CADDR	4

//All numbers are +1
#define NUMBER_OF_FED_CRATES	31
#define NUMBER_OF_FED_SLOTS		22
#define NUMBER_OF_FED_CHANNELS	97




#define NUMBER_OF_PSU_DETID			100





class ErrorsChecker
{
public:
	ErrorsChecker(void);

	~ErrorsChecker(void);


	void getCrateAndFecSlotsForFechardid(	unsigned int * crateSlot,
											unsigned int * fecSlot,
											TkDiagErrorAnalyser* tkDiagErrorAnalyser,
											std::string fecHardId);

	void getCrateAndFedSlotsForFedid(	unsigned int * crateSlot,
															unsigned int * fedSlot,
															TkDiagErrorAnalyser* tkDiagErrorAnalyser,
															unsigned int fedId);



	unsigned int getPsuIdRange(	unsigned int PsuId,
								TkDiagErrorAnalyser* tkDiagErrorAnalyser);



	void getFecViewErrorsForDetector(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser);


	void getFecViewErrorsForCrate(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int crateNumber);


	void getFecViewErrorsForSlot(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int crateNumber,
									unsigned int slotNumber,
									std::string & fecHardId);


	void getFecViewErrorsForPmc(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int crateNumber,
									unsigned int slotNumber,
									unsigned int pmcNumber);

	void getFecViewErrorsForCcu(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int crateNumber,
									unsigned int slotNumber,
									unsigned int pmcNumber,
									unsigned int ccuNumber);

	void getFecViewErrorsForChannel(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser,
										unsigned int crateNumber,
										unsigned int slotNumber,
										unsigned int pmcNumber,
										unsigned int ccuNumber,
										unsigned int channelNumber);


	void getFecViewErrorsForI2cAddress(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser,
										unsigned int crateNumber,
										unsigned int slotNumber,
										unsigned int pmcNumber,
										unsigned int ccuNumber,
										unsigned int channelNumber,
										unsigned int fecI2cAddress);


	void getFecCratesList(bool * fecCratesList, TkDiagErrorAnalyser* tkDiagErrorAnalyser);

/*
	void getFedCratesListAttachedToOneFecCrate(bool * fedCratesList,
												TkDiagErrorAnalyser* tkDiagErrorAnalyser,
												unsigned int fecCrateNumber);
*/

	void getFecSlotsListForOneCrate(	bool * fecSlotsList,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser,
										unsigned int fecCrateNumber);



	void getFecRingListForOneSlot(bool * fecPmcList,
												TkDiagErrorAnalyser* tkDiagErrorAnalyser,
												unsigned int fecCrateNumber,
												unsigned int fecSlotNumber);


	void getFecCcuListForOnePmc( bool * fecCcuList, TkDiagErrorAnalyser* tkDiagErrorAnalyser,
												unsigned int fecCrateNumber,
												unsigned int fecSlotNumber,
												unsigned int fecPmcNumber);



	void getFecChannelListForOneCcu( bool * fecChannelList, TkDiagErrorAnalyser* tkDiagErrorAnalyser,
												unsigned int fecCrateNumber,
												unsigned int fecSlotNumber,
												unsigned int fecPmcNumber,
												unsigned int fecCcuNumber);

	void getFecI2cAddressListForOneChannel( bool * fecI2cAddrList, TkDiagErrorAnalyser* tkDiagErrorAnalyser,
												unsigned int fecCrateNumber,
												unsigned int fecSlotNumber,
												unsigned int fecPmcNumber,
												unsigned int fecCcuNumber,
												unsigned int fecChannelNumber);









	void getFedViewErrorsForDetector(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser);


	void getFedErrorsForOneFecCrate(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int fedCrateNumber);


	void getFedErrorsForOneFecSlot(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber);


	void getFedErrorsForOneFecPmc(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber);


	void getFedErrorsForOneFecCcu(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber,
													unsigned int fecCcuNumber);


	void getFedErrorsForOneFecChannel(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber,
													unsigned int fecCcuNumber,
													unsigned int fecChannelNumber);


	void getFedCratesList(bool * fedCratesList, TkDiagErrorAnalyser* tkDiagErrorAnalyser);



	void getFedViewErrorsForCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber);



	void getFecErrorsForOneFedCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber);


	void getFedSlotsListForOneCrate(bool * fedSlotsList, TkDiagErrorAnalyser* tkDiagErrorAnalyser, unsigned int fedCrateNumber);


	void getFedViewErrorsForSlot(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int fedCrateNumber,
									unsigned int fedSlotNumber,
									unsigned int * fedId);


	void getFecErrorsForOneFedSlot(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber,
													unsigned int fedSlotNumber);



	void getFedChannelsListForOneSlot(bool * fedConnectionList, TkDiagErrorAnalyser* tkDiagErrorAnalyser, unsigned int fedCrateNumber, unsigned int fedSlotNumber);

	void getFedViewErrorsForChannel(	unsigned int * fecErrors,
									unsigned int * fedErrors,
									unsigned int * psuErrors,
									TkDiagErrorAnalyser* tkDiagErrorAnalyser,
									unsigned int fedCrateNumber,
									unsigned int fedSlotNumber,
									unsigned int fedChannelNumber);


	void getFecErrorsForOneFedChannel(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber,
													unsigned int fedSlotNumber,
													unsigned int fedChannelNumber);















	void getPsuViewErrorsForDetector(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser);


	void getPsuErrorsForOneFecCrate(	unsigned int * fecErrors,
										unsigned int * fedErrors,
										unsigned int * psuErrors,
										TkDiagErrorAnalyser* tkDiagErrorAnalyser,
										unsigned int fecCrateNumber);




	void getPsuErrorsForOneFecSlot(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber);



	void getPsuErrorsForOneFecPmc(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber);



	void getPsuErrorsForOneFecCcu(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber,
													unsigned int fecCcuNumber);


	void getPsuErrorsForOneFecChannel(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fecCrateNumber,
													unsigned int fecSlotNumber,
													unsigned int fecPmcNumber,
													unsigned int fecCcuNumber,
													unsigned int fecChannelNumber);


 	void getPsuErrorsForOneFedCrate(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber);



	void getPsuErrorsForOneFedSlot(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber,
													unsigned int fedSlotNumber);


	void getPsuErrorsForOneFedChannel(	unsigned int * fecErrors,
													unsigned int * fedErrors,
													unsigned int * psuErrors,
													TkDiagErrorAnalyser* tkDiagErrorAnalyser,
													unsigned int fedCrateNumber,
													unsigned int fedSlotNumber,
													unsigned int fedChannelNumber);



/*
	void getPsuViewErrorsForDetector(	unsigned int * fecErrors,
														unsigned int * fedErrors,
														unsigned int * psuErrors,
														TkDiagErrorAnalyser* tkDiagErrorAnalyser);

*/




















};

#endif

