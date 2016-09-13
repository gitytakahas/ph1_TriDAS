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
#ifndef APIACCESS_H
#define APIACCESS_H

#ifndef DCUADDRESS
#  define DCUADDRESS 0x0
#endif

#include "keyType.h"

#include "FecExceptionHandler.h"

#include "i2cAccess.h"
#include "piaAccess.h"

#include "FecAccess.h"

#include "philipsDescription.h"
#include "philipsAccess.h"

#include "apvDescription.h"
#include "apvAccess.h"

#include "PiaResetAccess.h"

#include "pllDescription.h"
#include "pllAccess.h"

#include "muxDescription.h"
#include "muxAccess.h"

#include "laserdriverDescription.h"
#include "laserdriverAccess.h"
#include "DohAccess.h"

#include "muxDescription.h"
#include "muxAccess.h"

#include "dcuDescription.h"
#include "dcuAccess.h"

#ifdef TOTEM
#include "vfatDescription.h"
#include "vfatAccess.h"

#include "totemCChipDescription.h"
#include "totemCChipAccess.h"

#include "totemBBDescription.h"
#include "totemBBAccess.h"
#endif

#include "FecRingRegisters.h"

/** to set or the getchar, default false getchar are displayed
 */
void setNoGetchar(bool) ;
bool getNoGetchar() ;

/** To display error
 */
void setErrorCounterFlag ( bool val ) ;
bool getErrorCounterFlag ( ) ;

/** For output
 */
void setStdchan ( FILE *chan ) ;
FILE *getStdchan ( ) ; 

/** For different FEC bus
*/
void setFecType ( enumFecBusType fecBusType ) ;

/** This is test program for the different layers of the FEC supervisor.
 * It is able to test access to the devices, the reconfiguration through different ways
 * and the access to pia channels.
 * \version 3.3
 * \author Frederic Drouhin & Julien Moine
 * \date November 2002
 * \warning the test function for DCU is not implemented
 * \warning All the hash table use a key defined in the file keyType.h:
 * \include keyType.h
 * \brief This test program for access to the devices, for access to the pia channels and
 * for the reconfiguration
 */

/** \brief set the SSID for all the system
 */
void setSSID ( FecAccess *fecAccess, std::string ssidSystem, bool display = true, bool resetCCSTTCRx = false ) ;

/** \brief open a socket and wait for command
 */
int socketCommand ( FecAccess *fecAccess,
		    int port,
		    tscType8 fecAddress,
		    tscType8 ringAddress) ;

/** \brief Open a file and send all the commands to the ring
 */
int readFile ( FecAccess *fecAccess,
	       char *filename,
	       tscType8 fecAddress,
	       tscType8 ringAddress) ;

/** \brief This method wait for CCU alarms
 */
void waitCCUAlarms ( FecAccess *fecAccess,
                     tscType8 fecAddress,
		     tscType8 ringAddress,
                     long loop, unsigned long tms ) ;

/** \brief Test the tracker devices specified
 */
int testTrackerDevices ( FecAccess *fecAccess,
                         tscType8 fecAddress, 
                         tscType8 ringAddress,
                         tscType8 ccuAddress, 
                         tscType8 channelAddress,
                         bool sixApvsF,
                         bool fourApvsF,
                         bool apvmuxF,
                         bool dcuF,
                         bool laserdriverF,
                         bool dohF,
                         bool pllF,
                         long loop, unsigned long tms,
			 std::vector<std::string> & results = *(new std::vector<std::string>) ) ;

/** \brief Test the tracker devices specified
 */
int testTrackerDevicesI2CTest(  FecAccess *fecAccess,
				tscType8 fecAddress, 
				tscType8 ringAddress,
				tscType8 ccuAddress, 
				tscType8 channelAddressSt,
				tscType8 channelAddressEd,
				bool sixApvsF,
				bool fourApvsF,
				bool apvmuxF,
				bool dcuF,
				bool laserdriverF,
				bool dohF,
				bool pllF,
				long loop, unsigned long tms,
				std::vector<std::string> &results = *(new std::vector<std::string>) ) ;

#ifdef TOTEM
/** \brief Basic test for the VFAT
 */
int testVFAT ( FecAccess *fecAccess,
	       tscType8 fecAddress,
	       tscType8 ringAddress,
	       tscType8 ccuAddress,
	       tscType8 channelAddress,
	       tscType8 deviceAddress,
	       long loop, unsigned long tms ) ;

/** \brief Basic test for the CCHIP
 */
int testTotemCChip ( FecAccess *fecAccess,
		     tscType8 fecAddress,
		     tscType8 ringAddress,
		     tscType8 ccuAddress,
		     tscType8 channelAddress,
		     tscType8 deviceAddress,
		     long loop, unsigned long tms ) ;

/** \brief Basic test for the TTB
 */
int testTotemBB ( FecAccess *fecAccess,
		   tscType8 fecAddress,
		   tscType8 ringAddress,
		   tscType8 ccuAddress,
		   tscType8 channelAddress,
		   tscType8 deviceAddress,
		   long loop, unsigned long tms ) ;

/** \brief Access by write/read a register on i2c channel
 */
void testI2CRegister (FecAccess *fecAccess,
		      tscType8 fecAddress,
		      tscType8 ringAddress,
		      tscType8 ccuAddress,
		      tscType8 channelAddress,
		      tscType8 deviceAddress,
		      tscType8 value,
		      enumDeviceType modeType,
		      long loop, unsigned long tms ) ;
#endif

/**  \brief Basic test for the DCU
 */
int testDcu ( FecAccess *fec,
              tscType8 fecAddress,
              tscType8 ringAddress,
              tscType8 ccuAddress,
              tscType8 channelAddress,
              tscType8 deviceAddress, 
              long loop, unsigned long tms ) ;


/** \brief Basic test for accessing a Philips chip
 */
int testPhilips ( FecAccess *fec,
                  tscType8 fecAddress,
                  tscType8 ringAddress,
                  tscType8 ccuAddress,
                  tscType8 channelAddress,
                  tscType8 deviceAddress,
                  long loop, unsigned long tms) ;

/** \brief Basic test for accessing a Mux
 */
int testMux ( FecAccess *fec,
              tscType8 fecAddress, 
              tscType8 ringAddress,
              tscType8 ccuAddress, 
              tscType8 channelAddress,
              tscType8 deviceAddress,
              long loop, unsigned long tms ) ;

/** \brief Basic test for accessing four APVs
 */
int test4Apvs ( FecAccess *fec,
                tscType8 fecAddress, 
                tscType8 ringAddress,
                tscType8 ccuAddress, 
                tscType8 channelAddress,
                tscType8 deviceAddress1,
                tscType8 deviceAddress2,
                tscType8 deviceAddress3,
                tscType8 deviceAddress4,
                long loop, unsigned long tms) ;

/** \brief Basic test for accessing six APVs
 */
int test6Apvs ( FecAccess *fec,
                tscType8 fecAddress, 
                tscType8 ringAddress,
                tscType8 ccuAddress, 
                tscType8 channelAddress,
                tscType8 deviceAddress1,
                tscType8 deviceAddress2,
                tscType8 deviceAddress3,
                tscType8 deviceAddress4,
                tscType8 deviceAddress5,
                tscType8 deviceAddress6,
                long loop, unsigned long tms) ;

/** \brief Basic test for accessing a PLL
 */
int testPll ( FecAccess *fec,
              tscType8 fecAddress, 
              tscType8 ringAddress,
              tscType8 ccuAddress, 
              tscType8 channelAddress,
              tscType8 deviceAddress,
              long loop, unsigned long tms ) ;

/** \brief test all PLL for locking
 */
int pllLockCheck ( FecAccess *fecAccess,
		   tscType8 fecAddress, 
		   tscType8 ringAddress,
		   tscType8 ccuAddress, 
		   tscType8 channelAddress,
		   tscType8 deviceAddress,
		   long loop, unsigned long tms) ;

/** \brief Basic test for accessing a LaserDriver
 */
int testLaserdriver ( FecAccess *fec,
                      tscType8 fecAddress, 
                      tscType8 ringAddress,
                      tscType8 ccuAddress, 
                      tscType8 channelAddress,
                      tscType8 deviceAddress,
                      long loop, unsigned long tms,
		      bool readOnly = true) ;

/** \brief Basic test for accessing a DOH
 */
int testDoh ( FecAccess *fec,
              tscType8 fecAddress, 
              tscType8 ringAddress,
              tscType8 ccuAddress, 
              tscType8 channelAddress,
              tscType8 deviceAddress,
              long loop, unsigned long tms) ;

/** \brief write the gain to the laserdriver
 */
int testLaserGain (FecAccess *fec,
		  tscType8 fecAddress, 
		  tscType8 ringAddress,
		  tscType8 ccuAddress, 
		  tscType8 channelAddress,
		  long loop, unsigned long tms) ;

/** \brief Make test on the AOH and APVs
 */
int testApvLaser (FecAccess *fec,
		  tscType8 fecAddress, 
		  tscType8 ringAddress,
		  tscType8 ccuAddress, 
		  tscType8 channelAddress,
		  long loop, unsigned long tms) ;

/** \brief try to find the good values for the doh channel per channel
 */
int calibrateDoh ( FecAccess *fecAccess,
		   tscType8 fecAddress, 
		   tscType8 ringAddress,
		   tscType8 ccuAddress, 
		   tscType8 channelAddress,
		   tscType8 deviceAddress ) ;

/** \brief test the i2c speed for each device
 */
void calibrateI2CSpeed (FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress) ;

/** \brief scan the ring for CCUs
 */
void testScanCCU ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress, bool noBroadcast = false ) ;

/** \brief scan the ring for CCUs and find all the tracker devices attached
 */
std::vector<std::string> testScanTrackerDevice ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress, long loop, unsigned long tms, bool onlyDCU = false, bool testI2CSpeed = false ) ;

/** \brief scan the ring for CCUs and find all the i2c registers
 */
void testScanRingEDevice ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress, bool noBroadcast ) ;

/** \brief Retreive all the status for the FECs / CCUs / Device driver
 */
void testStatus ( FecAccess *fecAccess, bool noBroadcast = false ) ;

/** \brief Display the CCU status for a given CCU
 */
int testCCU ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress, tscType8 ccuAddress, long loop, unsigned long tms ) ;

/** \brief display all the counters
 */
void displayStatus ( FecExceptionHandler *e, unsigned int count, FecAccess *fecAccess, FILE *stdchan ) ;

/** \brief display all FEC/CCU registers in case of error
 */
void displayStatus ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress, tscType8 ccuAddress, tscType8 channelAddress, FecExceptionHandler *e) ;

/** \brief Do Singe access for N writes in single byte write
 */
void testMemWriteSingle ( FecRingDevice *fec,
                          keyType myKey,
                          unsigned long maxVal) ;

/** \brief Do single access for N reads in single byte read
 */
void testMemReadSingle ( FecRingDevice *fec,
                         keyType myKey,
                         unsigned long maxVal) ;

/** \brief Do multiple access for N writes in multiple byte write
 */
void testMemWriteMultiple ( FecRingDevice *fec,
                            keyType myKey,
                            unsigned long maxVal) ;

/** \brief Do multiple access for N reads in multiple byte read
 */
void testMemReadMultiple ( FecRingDevice *fec,
                           keyType myKey,
                           unsigned long maxVal) ;

/** \brief Do Singe access for N writes in single byte write
 */
void testMemWriteSingle ( FecAccess *fecAccess,
                          keyType myKey,
                          unsigned long maxVal) ;

/** \brief Do single access for N reads in single byte read
 */
void testMemReadSingle ( FecAccess *fecAccess,
                         keyType myKey,
                         unsigned long maxVal) ;

/** \brief Do multiple access for N writes in multiple byte write
 */
void testMemWriteMultiple ( FecAccess *fecAccess,
                            keyType myKey,
                            unsigned long maxVal) ;

/** \brief Do multiple access for N reads in multiple byte read
 */
void testMemReadMultiple ( FecAccess *fecAccess,
                           keyType myKey,
                           unsigned long maxVal) ;


/** \brief test the memory channel access 
 */
void testMemoryFunctions (FecAccess *fecAccess,
                          tscType8 fecAddress,
                          tscType8 ringAddress,
                          tscType8 ccuAddress,       
                          tscType8 channelAddress,
                          unsigned long maxVal ) ;

/** \brief test the memory channel access without maxVal
 */
void testMemoryFunctions (FecAccess *fecAccess,
                          tscType8 fecAddress,
                          tscType8 ringAddress,
                          tscType8 ccuAddress,       
                          tscType8 channelAddress ) ;

/** \brief Basic test for accessing pia channel with FecAccess class
 */
void testPIAfunctions (FecAccess *fecAccess,
                       tscType8 fecAddress,
                       tscType8 ringAddress,
                       tscType8 ccuAddress,
                       tscType8 channelAddress,
                       long loop, unsigned long tms ) ;

/** \brief Test the PIA reset functions
 */
std::string testPIAResetfunctions (FecAccess *fecAccess,
				   tscType8 fecAddress,
				   tscType8 ringAddress,
				   tscType8 ccuAddress,
				   tscType8 channelAddress,
				   tscType16 value,
				   unsigned long delayActiveReset,
				   unsigned long intervalDelayReset,
				   long loop, unsigned long tms ) ;

/** \brief Wait for any PIA interrupt
 */
void waitPiaInterrupt ( FecAccess *fecAccess, 
                        tscType8 fecAddress,
			tscType8 ringAddress,
                        tscType16 piaChannel,
                        long loop, unsigned long tms ) ;

/** \brief Display the FEC/RING/CCU/Channel/Address path for a given index
 */
int testKey ( keyType index, tscType32 wordSR0 ) ;

/** \brief use to test different methods
 */
void justATest ( FecAccess *fecAccess, keyType index ) ;

/** \brief Reset all firmwares on the FEC board
 */
void uFecReload ( FecAccess *fecAccess, 
		  tscType8 fecAddress ) ;


/** \brief Reset all PLX/FEC
 */
void resetPlxFec ( FecAccess *fecAccess, 
		   tscType8 fecAddress,
		   tscType8 ringAddress,
		   long loop, unsigned long tms ) ;

/** \brief Reset all FECs on ring B
 */
void resetRingB ( FecAccess *fecAccess, long loop, unsigned long tms ) ;

/** \brief Reset all FECs on ring A
 */
void resetRingA ( FecAccess *fecAccess, long loop, unsigned long tms ) ;

/** \brief Crate reset for VME FEC
 */
void crateReset ( FecAccess *fecAccess, bool testCrateReset, long loop, unsigned long tms ) ;

/** \brief create the plug and play FEC file
 */
void createPlugAndPlayFile ( FecAccess *fecAccess, std::string pnpFileName ) ;

/** \brief test the FEC
 */
void fecTest ( FecAccess *fecAccess, long loop, unsigned long tms );

/** \brief toggle recv. clock polarity on all FECs
 */
void toggleReceiverClockPolarity ( FecAccess *fecAccess, long loop, unsigned long tms );

/** \brief reset TTCRx on CCS board
 */
void commandResetCCSTTCRx ( FecAccess *fecAccess, tscType8 fecAddress, std::string ssidSystem, long loop, unsigned long tms ) ;

/** \brief display version register of all FECs
 */
void displayFecVersion( FecAccess *fecAccess, long loop, 
			unsigned long tms, 
			bool trigger = false, std::string ssid="TRACKER", 
                        bool resetCCSTTCRx = false );

/** \brief read status register 0 of the FEC and device driver status 
 */
void readFecRegister0 (tscType8 fecAddress,
		       tscType8 ringAddress,
                       long loop, unsigned long tms ) ;

/** \brief Access by write/read a register on i2c channel
 */
void testI2CDevice (FecAccess *fecAccess,
                    tscType8 fecAddress,
                    tscType8 ringAddress,
                    tscType8 ccuAddress,
                    tscType8 channelAddress,
                    tscType8 deviceAddress,
                    enumDeviceType modeType,
                    long loop, unsigned long tms ) ;

/** \brief Change ring A to ring B
 */
void testRedundancyRing ( FecAccess *fecAccess, 
			  tscType8 fecAddress,
			  tscType8 ringAddress,
			  uint ccuAddress[][3], 
			  uint numberOfCCU ) ;

/** Make an automatic test of the redundancy
 */
std::vector<std::string> autoRedundancyRing ( FecAccess *fecAccess,
					      tscType8 fecAddress,
					      tscType8 ringAddress,
					      tscType8 dcuChannel, 
					      int dcuTest,
					      long loop, unsigned long tms ) ;

/** Bypass one CCU
 */
void bypass ( FecAccess *fecAccess, unsigned int fecSlot_, unsigned int ring, unsigned short ccu1, unsigned short ccu3, unsigned short dummy = 0x1) ;

/** \brief Make an automatic test of the redundancy ring for the Tracker
 */
std::vector<std::string> autoTrackerRedundancyRing ( FecAccess *fecAccess,
						     tscType8 fecAddress,
						     tscType8 ringAddress,
						     tscType8 dcuChannel, 
						     int dcuTest,
						     long loop, unsigned long tms ) ;

/** \brief Bypass a list of CCU
 */
void ccuBypassed ( FecAccess *fecAccess, 
                   tscType8 fecAddress,
                   tscType8 ringAddress,
                   uint ccuAddress) ;

/** \brief Wait for any spurious reset
 */
int checkSpuriousReset ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress ) ;

/** \brief Wait for any parasitic frame and try to solve it
 */
int checkParasiticFrame ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress ) ;

/** \brief Default test, just read the FEC CR0
 */
void readFecRegisterSR0 (FecAccess *fecAccess, 
			 tscType8 fecAddress, 
			 tscType8 ringAddress,
                         long loop, unsigned long tms ) ;

/** \brief display all FEC status
 */
void displayFecStatus ( FecAccess *fecAccess, keyType index ) ;

/** \brief This method try to recover the ring until the status register 0 is ok
 */
void emptyFIFOs ( FecAccess *fecAccess, keyType index, bool display=false ) ;

/** \brief Try to recover a ring
 */
void testRecover ( FecAccess *fecAccess,
		   tscType8 fecAddress,
		   tscType8 ringAddress ) ;

/** \brief display a FEC status register
 */
void displayFECSR0 ( tscType16 fecSR0 ) ;

/** \brief display a FEC status register
 */
void displayFECSR1 ( tscType16 fecSR1 ) ;

/** \brief display a FEC control register
 */
void displayFECCR0 ( tscType16 fecCR0 ) ;

/** \brief display a FEC control register
 */
void displayFECCR1 ( tscType16 fecCR1 ) ;

/** \brief display a CCU status register
 */
void displayCCUSRA ( tscType8 SRA ) ;

/** \brief display a CCU status register
 */
void displayCCUSRB ( tscType8 SRB ) ;

/** \brief display a CCU status register
 */
void displayCCUSRC ( tscType8 SRC ) ;

/** \brief display a CCU status register
 */
void displayCCUSRD ( tscType8 SRD ) ;

/** \brief display a CCU status register
 */
void displayCCUSRE ( tscType32 SRE ) ;

/** \brief display a CCU status register
 */
void displayCCUSRF ( tscType16 SRF ) ;

/** \brief display a CCU control register
 */
void displayCCUCRA ( tscType8 CRA ) ;

/** \brief display a CCU control register
 */
void displayCCUCRB ( tscType8 CRB ) ;

/** \brief display a CCU control register
 */
void displayCCUCRC ( tscType8 CRC ) ;

/** \brief display a CCU control register
 */
void displayCCUCRD ( tscType8 CRD ) ;

/** \brief display a CCU control register
 */
void displayCCUCRE ( tscType32 CRE ) ;

/** \brief display all registers
 */
void displayAllRegisters ( FecRingRegisters fecRingRegisters ) ;

/** \brief get the FEC temperature
 */
void fecTempTest ( FecAccess *fecAccess, unsigned int fecAddress,
		   long loop, unsigned long tms ) ;

#endif

