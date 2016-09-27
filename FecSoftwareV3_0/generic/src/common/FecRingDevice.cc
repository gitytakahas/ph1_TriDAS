/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

/** C include
 */ 
#include <stdio.h>      // For snprintf function
#include <sys/types.h>  // For open function
#include <sys/stat.h>   // For open function
#include <sys/time.h>   // For timestamps
#include <fcntl.h>      // For open function
#include <unistd.h>     // For close function
#include <string.h>     // For memcpy function
#include <iostream> 
#include <iomanip>

#include "stringConv.h"

/** Type definition
 */
#include "cmdDescription.h"
#include "dderrors.h"

/** Class
 */
#include "FecExceptionHandler.h"
#include "FecRingRegisters.h"
#include "FecRingDevice.h"

/** In order to display the frames send to the driver
 * Only for debugging
 */
#ifdef DEBUGGETREGISTERS
#  define DEBUGFRAMES
#endif

//#define DEBUGMSGERROR_DISPLAYFRAME
//#define DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES
#ifdef DEBUGMSGERROR
#  ifndef DEBUGMSGERROR_DISPLAYFRAME
#    define DEBUGMSGERROR_DISPLAYFRAME
#  endif 
#endif

// Display bad frames in case of problem on rings
//#define DISPLAYBADFRAMES

// Wait for hard problem ?!
//   - during the first initialisation in the constructor
//   - after a FEC reset
//   - during a scan ring
//   - to read a frame
//   - the minimum time is 0 especially for the i2c scanning
//   - the time is now done in nanoseconds
//#define TIMETOWAIT 10000000 
// Timeout for waiting or pooling for a frame
#define TIMEOUT           500
#define TIMEOUTFORCEACK 40000

// Calculate the timeout to feet more with the system
//#define RECALCULATETIMEOUT
//#define DEBUGTIMING

// -------------------------------------------------------------------------------------
//
//                              Constructors and destructor
//
// -------------------------------------------------------------------------------------

/** Initialise the parameter for the FEC:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot on the corresponding FEC
   * \param fecType - type of the FEC (PCI, VME, USB)
   * \warning the call of setInitFecRingDevice must be done after the call of the constructor. It is done now in the FecPciRingDevice, FecVmeRingDevice, FecUsbRingDevice
   */
FecRingDevice::FecRingDevice ( tscType8 fecSlot, tscType8 ringSlot, enumFecBusType fecType ) {

  fecSlot_ = fecSlot ;
  ringSlot_ = ringSlot ;
  fecType_ = fecType ;
  transmitFifoDepth_ = 0 ; // no fifo ..
  receiveFifoDepth_  = 0 ; // no transmit fifo..
  returnFifoDepth_ = 0 ; // no return fifo
  firmwareVersion_ = 0x0 ; // unknown
}

/**
 * Open the device driver corresponding to the slot given by argument
 * and initialise all the attributs (CCU hash table).
 * \param init - initialise the FEC. If the SR0 is not correct then
 * \param invertClockPolarity - invert the clock polaritry
 * reset the PLX and the FEC and try to emptyied the FIFO
 * \exception FecExceptionHandler
 */
void FecRingDevice::setInitFecRingDevice ( bool init, bool invertClockPolarity ) 
  throw (FecExceptionHandler) {

  firmwareVersion_ = getFecFirmwareVersion() ;

  if (firmwareVersion_ <= 0x11) {
    transmitFifoDepth_ = 512 ; // in D32 word units
    receiveFifoDepth_  = 512 ; // in D32 word units
    returnFifoDepth_   = 128 ; // in D8 words units
  } 
  else if (firmwareVersion_ <= 0x14) { 
    transmitFifoDepth_ = 512 ; // in D32 word units
    receiveFifoDepth_  = 512 ; // in D32 word units
    returnFifoDepth_   = 128 ; // same as above    
  } 
  else { // if (getFecFirmwareVersion() <= 0x1500) { 

    transmitFifoDepth_ =  256 ; // in D32 word units
    receiveFifoDepth_  =  400 ; // in D32 word units
    returnFifoDepth_   = 1024 ; // in D8 words units
  } 
  
  // Initialise the transaction number
  transactionNumber_ = 0x1 ;
  for (int i = 0 ; i <= MAXTRANSACTIONNUMBER ; i ++) {
    busyTransactionNumber[i] = false ;
    timeTransactionNumber[transactionNumber_] = 0 ;
  }

  // Reconfiguration
  reconfigurationRunning_ = false ;

#ifdef DEBUGGETREGISTERS
  // readout of the registers in case of error
  readRegistersError_ = true ;
#endif

  // Access
  accessNumber_ = 1 ;

  // Clock polarity
  invertClockPolarity_ = invertClockPolarity ;
  // RMW to set the clock polarity
  if (invertClockPolarity_) setFecRingCR0(FEC_CR0_POLARITY, CMD_OR) ;
  else setFecRingCR0(FEC_CR0_POLARITY, CMD_XOR) ;

  // Read SR0
  tscType16 fecSR0 = getFecRingSR0() ;    

  if (isFecSR0Correct(fecSR0)) {
#ifdef DEBUGMSGERROR
    std::cout << "Ring status correct " << std::hex << fecSR0 << std::endl ;
#endif
  }
  else {

    if (init) {

      // Clear errors
      // setFecRingCR0 (FEC_CR0_ENABLEFEC) ; // Done in the FEC Reset method
      setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ; // Clear errors 
      
      // Reset the FEC
      fecRingReset ( ) ;

      // Try to empty the FIFO is the status is not correct
      fecSR0 = getFecRingSR0() ;
      if (!isFifoReceiveEmpty(fecSR0)) checkFifoReceive ( ) ;  

      int timeout = 0 ;
      fecSR0 = getFecRingSR0() ;
      while ((! isFecSR0Correct(fecSR0)) && (timeout < 10) ) {
        // Check the SR0 of the FEC
	fecSR0 = getFecRingSR0() ;
        if (! isFecSR0Correct(fecSR0)) {
          
          // Set the control register of the FEC
          fecRingReset ( ) ;
        
          setFecRingCR0 (FEC_CR0_ENABLEFEC) ; // Done in the FEC Reset method
          setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ; // Clear errors    
        }

#ifdef TIMETOWAIT
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
	nanosleep (&req,NULL) ;
	//usleep (TIMETOWAIT) ;
#endif

	fecSR0 = getFecRingSR0() ;
        timeout ++ ;
      }

      fecSR0 = getFecRingSR0() ;
      if (! isFecSR0Correct(fecSR0)) {
      
	std::cerr << "The status register 0 of the FEC " << std::dec << (int)getFecSlot() << ", ring " << (int)getRingSlot() << " is not correct " << std::hex << getFecRingSR0() << std::endl ;
// 	RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
// 			                   TSCFEC_SR0NOTNOMINAL_MSG,
// 				           FATALERRORCODE,
// 				           buildFecRingKey(getFecSlot(), getRingSlot()),
// 				           "FEC status register 0", fecSR0) ;
      }
    }
#ifdef DEBUGMSGERROR
    else {
      fecSR0 = getFecRingSR0() ;
      if (! isFecSR0Correct(fecSR0))
	std::cerr << "The status register 0 of the FEC " << std::dec << (int)getFecSlot() << ", ring " << (int)getRingSlot() << " is not correct " << std::hex << getFecRingSR0() << std::endl ;
    }
#endif
  }

  // Scan the ring in order to find the CCUs and their status
  // For each CCU => initialise the values
  for (int j = 0 ; j < MAXCCU ; j ++) {
      
    ccuMapAccess_[j] = NULL ;
    ccuMapOrder_[j] = NULL ;
  }

  // number of loops before the answer to a frame
  loopInTimeWriteFrame_      = TIMEOUT ;  // the final timeout will be multiplied by 2 => it represents the time to send the frame when the toggle on the bit send is done until the FIFO transmit is empty
  loopInTimeDirectAck_       = TIMEOUT ; // the final timeout will be multiplied by 2 => it represents the time to wait the direct acknowledge so the time once the frame is sent until the time when the pending IRQ is arrived
  loopInTimeReadFrame_       = TIMEOUTFORCEACK ;  // the final timeout will be multiplied by 10 => it represents the time between the direct acknowledge is received until the the time when the data to FEC is arrived

#ifdef DEBUGFRAMES
  // Position in the history table
  frameHistoryCounter_ = 0 ;
  // Check if the history goes from 0 to frameHistoryCounter_
  // or from frameHistoryCounter_+1 to MAXHISTORYFRAME and from 0 to frameHistoryCounter_
  turnOne_ = false ;
#endif
}

/** Remove all the CCU allocated
 */
FecRingDevice::~FecRingDevice ( ) {

  // Delete all CCUs
  for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();p!=ccuMapAccess_.end();p++)
    delete p->second ; // Remove all the accesses for the CCU device

  ccuMapAccess_.clear() ;
}

// -------------------------------------------------------------------------------------
//
//                                Software methods
//
// -------------------------------------------------------------------------------------

/** \brief To retreive the FEC slot
 * \return FEC slot
 */
tscType8 FecRingDevice::getFecSlot ( ) {

  return (fecSlot_) ;
}

/** \brief To retreive the RING slot
 * \return RING slot
 */
tscType8 FecRingDevice::getRingSlot ( ) {

  return (ringSlot_) ;
}

/** \brief To retreive the FEC type
 * \return FEC type (PCI, VME, USB)
 */
tscType8 FecRingDevice::getFecType ( ) {
  
  return (fecType_) ;
}

/** return the free next transaction number, if no transaction number is available then try to find if a transaction has a timeout so existing since more than 1 second
 */
tscType8 FecRingDevice::getNextTransactionNumber ( ) {

  // Try to find a transaction number [1, MAXTRANSACTIONNUMBER]
  transactionNumber_ ++ ;
  transactionNumber_ = transactionNumber_ > MAXTRANSACTIONNUMBER ? 1 : transactionNumber_ ;
  for (int i = 0 ; (i < MAXTRANSACTIONNUMBER) && (busyTransactionNumber[transactionNumber_]) ; i ++) {
    transactionNumber_ ++ ;
    transactionNumber_ = transactionNumber_ > MAXTRANSACTIONNUMBER ? 1 : transactionNumber_ ;
  }

  // if no transaction number found, try to clear the transaction number not correctly released
  if (busyTransactionNumber[transactionNumber_]) {
    time_t currentTime = time(NULL) ;
    for (tscType8 tnum = 1 ; tnum <= MAXTRANSACTIONNUMBER ; tnum ++) {
      if (currentTime - timeTransactionNumber[tnum] > 1) {
	busyTransactionNumber[tnum] = false ;
	std::cerr << "Warning: Transaction number " << (int)tnum << " timeout reached, automatic clear done" << std::endl ;
      }
    }

    // Try to find a transaction number [1, MAXTRANSACTIONNUMBER]
    transactionNumber_ ++ ;
    transactionNumber_ = transactionNumber_ > MAXTRANSACTIONNUMBER ? 1 : transactionNumber_ ;
    for (int i = 0 ; (i < MAXTRANSACTIONNUMBER) && (busyTransactionNumber[transactionNumber_]) ; i ++) {
      transactionNumber_ ++ ;
      transactionNumber_ = transactionNumber_ > MAXTRANSACTIONNUMBER ? 1 : transactionNumber_ ;
    }
  }

  // No more transaction number
  if (busyTransactionNumber[transactionNumber_]) {
#ifdef DEBUGMSGERROR
    std::cerr << "No more transaction number available" << std::endl ;
#endif
    return 0 ;
  }

  // Transaction is now busy
  busyTransactionNumber[transactionNumber_] = true ;
  timeTransactionNumber[transactionNumber_] = time(NULL) ;

#ifdef DEBUGMSGERROR
  std::cout << "Transaction number used = " << (int)transactionNumber_ << std::endl ;
#endif

  return (transactionNumber_) ;
}

/** Release the transaction number to be used by another frame
 */
void FecRingDevice::releaseTransactionNumber ( tscType8 transactionNumber ) {

  busyTransactionNumber[transactionNumber] = false ;
#ifdef DEBUGMSGERROR
  std::cout << "Transaction number released = " << (int)transactionNumber << std::endl ;
#endif
}

/** Store the number of access for this device driver
 */
void FecRingDevice::addNewAccess ( ) {

  accessNumber_ ++ ;
}

/** Store the number of access for this device driver
 */
void FecRingDevice::removeAccess ( ) {
  
  accessNumber_ -- ;
}

/** Check all the values given: see the exception
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning no hardware access is performed in this method if the ring is scanned at the beginning
 */
void FecRingDevice::basicCheckError ( keyType index ) throw (FecExceptionHandler) {

#ifdef S2NDLEVELCHECK

  // Check the FEC slot
  if ( (getFecSlot() != getFecKey(index)) || (getRingSlot() != getRingKey(index)) ) {
    RAISEFECEXCEPTIONHANDLER_HARSPOSITION ( TSCFEC_INVALIDOPERATION,
					    "Wrong FEC slot or wrong ring slot",
					    ERRORCODE,
					    index) ;
  }

#endif

  // ifdef F1STLEVELCHECK and ifndef SCANCCUATSTARTING then do the instructions
#ifdef F1STLEVELCHECK
  bool isCcu25 = connectCCU (index) ;
#else
  //#    ifndef SCANCCUATSTARTING
  connectCCU (index) ;
  //#    endif
#endif

#ifdef S2NDLEVELCHECK

  // Check the channel number
  if (isCcu25) {           // CCU 25

    if (! isi2cChannelCcu25(index)       &&   // I2C channels + Broadcast
        ! isPiaChannelCcu25(index)       &&   // PIA channels
        ! isMemoryChannelCcu25(index)    &&   // Memory channel
        ! isTriggerChannelCcu25(index)   &&   // Trigger channel
        ! isJtagChannelCcu25(index)      &&   // JTAG channel
        ! isBroadCastChannelCcu25(index)&&    // Broadcast
        ! isNodeControllerChannelCcu25(index))// Node controller
      {

	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
						"Wrong channel slot for CCU 25",
						ERRORCODE,
						index) ;
      }
  }
  else {                                         // Old CCU

    if (! isi2cChannelOldCcu(index)      &&      // I2C channels 
        ! isPiaChannelOldCcu(index)      &&      // PIA channel
        ! isMemoryChannelOldCcu(index)   &&      // Memory channel
        ! isTriggerChannelOldCcu(index)  &&      // Trigger channel
        ! isBroadCastChannelOldCcu(index)&&      // Broadcast
        ! isNodeControllerChannelOldCcu(index) ) // Node controller
      {

	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
						"Wrong channel slot for old CCU",
						ERRORCODE,
						index) ;
      }
  }

#endif

}

/** This method is able to set the read the registers in case of error in the write and read frame methods
 * by default that program readout all registers in case of error [by default true], in some case this features must be disactivated in the cae, 
 * for example of scanring, scantrackerdevice, etc. This readout can be remove from the compilation by editing the FecSoftwareV3_0/FecHeader.linux 
 * and put no for the tag DO_DEBUGGETREGISTERS. 
 * If the tag DO_DEBUGGETREGISTERS/DEBUGGETREGISTERS is not set then this method does nothing
 */
void FecRingDevice::setReadRegistersError ( bool readRegisters ) {

#ifdef DEBUGGETREGISTERS
  readRegistersError_ = readRegisters ;
#endif
}

/** This method return the value of the read registers in case of error
 * by default that program readout all registers in case of error [by default true], in some case this features must be disactivated in the cae, 
 * for example of scanring, scantrackerdevice, etc. This readout can be remove from the compilation by editing the FecSoftwareV3_0/FecHeader.linux 
 * and put no for the tag DO_DEBUGGETREGISTERS
 * If the tag DO_DEBUGGETREGISTERS/DEBUGGETREGISTERS is not set then this method return always false
 */
bool FecRingDevice::getReadRegistersError ( ) {

#ifdef DEBUGGETREGISTERS
  return (readRegistersError_) ;
#else
  return (false) ;
#endif
}

/** \return transmit fifo size (depth) in D32 words 
 */
unsigned int FecRingDevice::getTransmitFifoDepth () {

  return transmitFifoDepth_ ;
}

/** \return receive fifo size (depth) in D32 words 
 */
unsigned int FecRingDevice::getReceiveFifoDepth () {
  
  return receiveFifoDepth_ ;
}

/** \return fifo size (depth) in D32 words 
 */
unsigned int FecRingDevice::getReturnFifoDepth () {

  return returnFifoDepth_ ;
}

// -------------------------------------------------------------------------------------
//
//                          Methods to write and read a frame over the ring
//
// -------------------------------------------------------------------------------------

/** Send a frame over the ring. The frame is defined in the CCU documentation for
 * all the channels and CCU. The direct acknowledge is checked and returned in case of
 * error.
 * The frame is 8 bits size array and the conversion to the 32 bits is done in this method.
 * \param frame - frame to be sent. The size of the frame is DD_USER_MAX_MSG_LENGTH. At the
 * end of the method the frame returned is the direct acknoledge.
 * \param ignoreDirectAck - ignore the direct acknowledge is true, by default false [OPTION parameter]
 * \return Error code
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 * \warning The direct acknowledge is checked in this method, in case of bad status an exception is raised.
 * \warning The maximum size in the frame can not greater than 0x7F for one byte length or
 * 0x7FFF for two bytes length. The word 0x80 means that the length of the frame is two 
 * bytes long.
 * \warning the hard code of the length is given by the lines below:
 * <ul>
 * <li> Longueur   Codage de la taille  Lenh Lenl
 * <li> 0 - 127    1 mot                -    0x7F
 * <li> 0 - 255    2 mots               0x80 0xFF
 * <li> 256 - 1024 2 mots               0x9F 0xFF
 * </ul>
 * \author Laurent Gross for the sending and receiving frame
 */
void FecRingDevice::writeFrame ( tscType8 *frame, bool ignoreDirectAck ) 
  throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  tscType16 fecSR0D = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice(" << std::dec << (int)getFecSlot() << "." << (int)getRingSlot() << ")::writeFrame: begin and the SR0 is " << std::hex << fecSR0D << std::endl ;
#endif

  // Force acknowledge set in order to wait for the data to FEC
  bool forceAck = true ;

  // --------------------------------------------------------------------------------------
  // Index to the FEC/ring/CCU/channel/device
  keyType indexOrig = buildCompleteKey(getFecSlot(), getRingSlot(), frame[0], frame[2] &  FEC_LENGTH_2BYTES ? frame[4] : frame[3], frame[2] &  FEC_LENGTH_2BYTES ? frame[7] : frame[6]) ;
  if (!isi2cChannelCcu25 (indexOrig))
    indexOrig = buildCompleteKey(getFecSlot(), getRingSlot(), frame[0], frame[2] &  FEC_LENGTH_2BYTES ? frame[4] : frame[3], 0) ;

  // --------------------------------------------------------------------------------------
  // next transaction number
  tscType8 tnum = getNextTransactionNumber();

  // No more transaction number available
  if (tnum == 0) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( DD_NO_MORE_FREE_TRANSACTION_NUMBER,
					    "No more transaction number available, all are in used",
					    ERRORCODE,
					    buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }

  // --------------------------------------------------------------------------------------
  // Check the size of the frame
  bool readCommand = false ;
  tscType16 realSize = frame[2] ;
  if (realSize & FEC_LENGTH_2BYTES) {
  
    realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1 ;
    // +1 for the two byte words for length
 
    // Put the transaction number
    // Dst, Src, Length1, Length2, Channel, Transaction, Cmd, data

    if (frame[5] == 0xFE) {
#ifdef DEBUGMSGERROR
      std::cout << "Read command" << std::endl ;
#endif
      readCommand = true ;
    }

    frame[5] = tnum ;
  }
  else {

    // Check if it is a read command
    if (frame[4] == 0xFE) {
#ifdef DEBUGMSGERROR
      std::cout << "Read command" << std::endl ;
#endif
      readCommand = true ;
    }

    // Put the transaction number
    // Dst, Src, Length, Channel, Transaction, Cmd, data
    frame[4] = tnum ;
  }

  // --------------------------------------------------------------------------------------
  // make a copy of the frame for future error
#ifdef DEBUGGETREGISTERS
  if (readRegistersError_) 
#endif
  if (!reconfigurationRunning_) {

#ifdef DEBUGFRAMES
    memcpy (frameHistory_[frameHistoryCounter_], frameRequest_, DD_USER_MAX_MSG_LENGTH) ;
    frameHistoryCounter_ ++ ;
    if (frameHistoryCounter_ == MAXHISTORYFRAME) turnOne_ = true ;
    frameHistoryCounter_ = frameHistoryCounter_ % MAXHISTORYFRAME ;
#endif

    memcpy (frameRequest_, frame, (realSize+3)*sizeof(tscType8)) ;
    memset (dAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    memset (fAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
  }

  // --------------------------------------------------------------------------------------
#ifdef S2NDLEVELCHECK
  // Too many words ?
  if ((realSize+3) > (DD_MAX_MSG_LENGTH_32*4)) {

#ifdef DEBUGMSGERROR_DISPLAYFRAME
    std::cerr << "RING WRITE ERROR: size in frame is bigger than the FIFO size => cannot be !" << std::endl ;
#endif

    // Release the transaction number
    releaseTransactionNumber(tnum) ;

    RAISEFECEXCEPTIONHANDLER_FECRING ( DD_TOO_LONG_FRAME_LENGTH,
				       "Size in frame is bigger than the FIFO size",
				       CRITICALERRORCODE,
				       buildFecRingKey(getFecSlot(), getRingSlot()),
				       "Size of the frame received", realSize+3,
				       NULL, frameRequest_, NULL, NULL) ;
  }
#endif

#ifdef DEBUGMSGERROR_DISPLAYFRAME
  std::cout << __func__ << " ( { " ;
  for (int i = 0 ; i < (realSize+3) ; i ++ )
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  std::cout << "} )" << std::dec << std::endl ;
#endif

  // --------------------------------------------------------------------------------------
  // Check the FEC ring SR0
  tscType16 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0)) {

    if (reconfigurationRunning_) {

      std::cerr << "The SR0 0x" << std::hex << fecSR0 << " is not nominal" << std::dec << std::endl ;
    }
    else {

      // filter the frames to try to recover it if the ring is closed
      if (fecSR0 & FEC_SR0_LINKINITIALIZED) {

	if (!(fecSR0 & FEC_SR0_RECEMPTY)) {
	  checkFifoReceive ( ) ;
	  // Check again the SR0
	  fecSR0 = getFecRingSR0() ;
	}
      }

      // if the IRQ is on: The IRQ must be cleared at this point if the readFrame was not called
      // This method does clear the IRQ after this clear
      if ((fecSR0 & FEC_SR0_PENDINGIRQ) || (fecSR0 & FEC_SR0_DATATOFEC)) {
	setFecRingCR1 (FEC_CR1_CLEARIRQ);
	// Check again the SR0
	fecSR0 = getFecRingSR0() ;
      }

      // Check again the SR0
      if (! isFecSR0Correct(fecSR0)) {
#ifdef DEBUGMSGERROR_DISPLAYFRAME
	std::cerr << "The SR0 0x" << std::hex << fecSR0 << " is not nominal, the frame is not sent" 
		  << std::dec << std::endl ;
	
	tscType16 fecSR1 = getFecRingSR1() ;
	tscType16 fecCR0 = getFecRingCR0() ;
	std::cerr << "The SR1 0x" << std::hex << fecSR1 << std::dec << std::endl ;
	std::cerr << "The CR0 0x" << std::hex << fecCR0 << std::dec << std::endl ;
#endif    
	
	// Release the transaction number
	releaseTransactionNumber(tnum) ;	

	// Retreive if needed the registers of FEC and CCU
	FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
	if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

	  std::cerr << "--------------------------------------------------------------------" << std::endl ;
	  std::cerr << "Error starts here with that display" << std::endl ;
	  std::cerr << __func__ << ": The SR0 0x" << std::hex 
		    << fecSR0 << " is not nominal, the frame is not sent" 
		    << std::dec << std::endl ;
	  
#  ifdef DEBUGFRAMES
	  // For debugging purpose display the frames already sent
	  std::cerr << "Display previous frames (history)" << std::endl ;
	  if (turnOne_) {
	    for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	      std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	    }
	  }
	  for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
#endif

	  // Read the registers for debugging
	  fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
	}
#endif
	
	RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_SR0NOTNOMINAL,
					   "Fails on sending a frame, ring lost",
					   CRITICALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters,
					   frameRequest_, NULL, NULL) ;
      }
    }
  }

  // --------------------------------------------------------------------------------------
  // Channel for error and check if the channel is busy only if the CCU i2c channel is configure
  // without force acknwoledge
  ccuMapAccessedType::iterator ccu =  ccuMapAccess_.find(getCcuKey(indexOrig)) ;
  if ( ((ccu == ccuMapAccess_.end()) || (ccu->second == NULL) || (!ccu->second->getBitForceAck(getChannelKey(indexOrig)))) &&
       (isi2cChannelCcu25 (indexOrig) || isi2cChannelOldCcu(indexOrig))
       ) {
    
    forceAck = false ;
    unsigned long watchdog = 0 ;
    while ( isChannelBusy(indexOrig) && (watchdog++ < TIMEOUT) ) {

#ifdef DEBUGMSGERROR
      std::cout << "Channel was busy, waiting ..." << std::endl ;
      std::cout << __func__ << " ( { " ;
      for (int i = 0 ; i < (realSize+3) ; i ++ )
	std::cout << "0x" << std::hex << (int)frame[i] << " " ;
      std::cout << "} )" << std::dec << std::endl ;
#endif
    }
  }

  // --------------------------------------------------------------------------------------
  // Build the frame in 32 bits and send it over the ring
  tscType32 c[DD_USER_MAX_MSG_LENGTH], realSize32 = 0, cpt = 0 ;

#ifdef DEBUGFRAMES
  for (int i = 0 ; i < DD_USER_MAX_MSG_LENGTH ; i ++) c[i] = 0 ;
#endif

  realSize32 = (realSize+3)/4 ;
  if ( ((realSize+3) % 4) != 0 ) realSize32 += 1 ;

  for (tscType32 i = 0 ; i < realSize32 ; i ++) {

    c[i] = (frame[cpt] << 24) | (frame[cpt+1] << 16) | (frame[cpt+2] << 8) | frame[cpt+3] ;
    
#ifdef DEBUGMSGERROR
    std::cout << "c[" << std::dec << i << "] = 0x" << std::hex << c[i] << ", frame[" << std::dec << cpt << "] =  0x" << std::hex << (int)frame[cpt] << std::endl ;
#endif

    // Next word
    cpt += 4 ;
  }

  // --------------------------------------------------------------------------------------
  // Clear the error bit
  setFecRingCR1 ( DD_FEC_CLEAR_ERRORS ) ;

  // --------------------------------------------------------------------------------------
  // WB add-on: but here in block mode
  setFifoTransmit(c,realSize32); 

#ifdef S2NDLEVELCHECK
  // --------------------------------------------------------------------------------------
  // Check that the FIFO transmit is not empty
  if (getFecRingSR0() & FEC_SR0_TRAEMPTY) {
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    std::cerr << __func__ << ": FIFO transmit empty, cannot send the frame" << std::endl ;
#endif    
    
    // Release the transaction number
    releaseTransactionNumber(tnum) ;

    // Retreive if needed the registers of FEC and CCU
    FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
    if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

      std::cerr << "--------------------------------------------------------------------" << std::endl ;
      std::cerr << "Error starts here with that display" << std::endl ;
      std::cerr << __func__ << ": FIFO transmit empty, cannot send the frame" << std::endl ;
      
#  ifdef DEBUGFRAMES
      // For debugging purpose display the frames already sent
      std::cerr << "Display previous frames (history)" << std::endl ;
      if (turnOne_) {
	for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
      }
      for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
      }
#  endif

      // Read the registers for debugging
      fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
    }
#endif

    RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_INVALIDOPERATION,
				       "FIFO transmit empty and you try to toggle the bit send (CR0[3])"
				       ERRORCODE,
				       buildFecRingKey(getFecSlot(), getRingSlot()),
				       &fecRingRegisters, 
				       frameRequest_, NULL, NULL) ;
  }
#endif

  // --------------------------------------------------------------------------------------
  // Toggle the send bit of CTRLO with RMW operation
  setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_OR ) ;
  setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_XOR ) ;

  // --------------------------------------------------------------------------------------
  // Wait for the FIFO transmit is not running only if no reconfiguration is currently done
  unsigned long watchdog = 0;
  if (reconfigurationRunning_) {
#ifdef DEBUGMSGERROR
    std::cout << "Reconfiguration running" << std::endl ;
#endif
    watchdog = TIMEOUT ;
  }

  fecSR0 = getFecRingSR0() ;
  while ( ((fecSR0 & FEC_SR0_TRARUN) || (!(fecSR0 & FEC_SR0_TRAEMPTY))) && (watchdog++ < loopInTimeWriteFrame_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
    struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
    nanosleep (&req,NULL) ;
    //usleep (TIMETOWAIT) ;
#endif
    fecSR0 = getFecRingSR0() ;
  }

  // --------------------------------------------------------------------------------------
  // Check that the FIFO transmit is not running
  fecSR0 = getFecRingSR0() ;
  if ( (fecSR0 & FEC_SR0_TRARUN) || !(fecSR0 & FEC_SR0_LINKINITIALIZED) ) {

#ifdef DEBUGMSGERROR_DISPLAYFRAME 
      std::cerr << __func__ << ": FIFO transmit running bit is always ON (SR0 = " 
		<< std::hex << fecSR0 << ")" << std::dec << std::endl ;
#endif    

      // Release the transaction number
      releaseTransactionNumber(tnum) ;

      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ":  FIFO transmit running bit is always ON (SR0 = " 
		  << std::hex << fecSR0 << ")" << std::dec << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif

      if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
	RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_SR0NOTNOMINAL,
					   "Fails on sending a frame, ring lost",
					   CRITICALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters,
					   frameRequest_, NULL, NULL) ;
      }
      else {
	RAISEFECEXCEPTIONHANDLER_FECRING ( DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON,
					   "FIFO transmit running bit is always on (CR0[3])",
					   CRITICALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters, 
					   frameRequest_, NULL, NULL) ;
      }
  }

#ifdef DEBUGTIMING
  if ( (getChannelKey(indexOrig) >= 0x10) && (getChannelKey(indexOrig) <= 0x1F) ) {
    std::cout << "# WAIT FIFO TRANSMIT for FEC " << std::dec << (int)fecSlot_ << "." << (int)ringSlot_ << ": loopInTimeWriteFrame: " << watchdog << "/" << loopInTimeWriteFrame_ << " for the device 0x" << std::hex << indexOrig ;

    if ( (loopInTimeWriteFrame_ == TIMEOUT) && (watchdog < TIMEOUT) ) {
      if (!watchdog) watchdog = 1000 ;
      std::cout << " (recalculcation time is " << std::dec << (watchdog*2) << ")" << std::endl ;
    }
  }
#endif
  
#ifdef RECALCULATETIMEOUT
  // --------------------------------------------------------------------------------------
  // Update the timeout
  if ( (loopInTimeWriteFrame_ == TIMEOUT) && (watchdog < TIMEOUT) ) {
    if (!watchdog) watchdog = 1000 ;
    loopInTimeWriteFrame_ = watchdog*2 ;
    
#  ifdef DEBUGMSGERROR
    std::cout << "loopInTimeWriteFrame_ = " << std::dec << loopInTimeWriteFrame_ << std::endl ;
#  endif
  }
#endif
  
  // ---------------------------------------------------------------------------
  // Wait for the direct ack only if 
  if (!reconfigurationRunning_) {
    
    // now, wait for the direct ack ..
    watchdog = 0 ;
    fecSR0 = getFecRingSR0() ;
    
    // ---------------------------------------------------------------------------
    // Waiting on the pending IRQ
    //while ( !(fecSR0 & FEC_SR0_PENDINGIRQ) && (watchdog++ < loopInTimeDirectAck_) ) {
    //if (!isi2cChannelCcu25(indexOrig) || !forceAck) {
    if (true) {
      while ( !(fecSR0 & FEC_SR0_PENDINGIRQ) && (watchdog++ < loopInTimeDirectAck_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
	nanosleep (&req,NULL) ;
	//usleep (TIMETOWAIT) ;
#endif
	fecSR0 = getFecRingSR0() ;
      }

#ifdef DEBUGTIMING
      if ( (getChannelKey(indexOrig) >= 0x10) && (getChannelKey(indexOrig) <= 0x1F) ) {
	std::cout << "# WAIT DIRECT ACK for FEC " << std::dec << (int)fecSlot_ << "." << (int)ringSlot_ << ": loopInTimeDirectAck: " << std::dec << watchdog << "/" << loopInTimeDirectAck_ << " for the device 0x" << std::hex << indexOrig ;

	if ( (loopInTimeWriteFrame_ == TIMEOUT) && (watchdog < TIMEOUT) ) {
	  if (!watchdog) watchdog = 1000 ;
	  std::cout << " (recalculcation time is " << std::dec << (watchdog*2) << ")" << std::endl ;
	}
      }
#endif

#ifdef RECALCULATETIMEOUT
      // --------------------------------------------------------------------------------------
      // Update the timeout
      if ( (loopInTimeDirectAck_ == TIMEOUT) && (watchdog < TIMEOUT) ) {
	if (!watchdog) watchdog = 1000 ;
	loopInTimeDirectAck_ = watchdog*2 ;
	
#  ifdef DEBUGMSGERROR
	std::cout << "loopInTimeDirectAck_ = " << std::dec << loopInTimeDirectAck_ << std::endl ;
#  endif
      }
#endif
    }
    else {
      while ( (!(fecSR0 & FEC_SR0_PENDINGIRQ) || !(fecSR0 & FEC_SR0_DATATOFEC)) && (watchdog++ < loopInTimeDirectAck_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
	fecSR0 = getFecRingSR0() ;
      }
    }
    
#ifdef DEBUGMSGERROR
    std::cout << "SR0 = 0x" << std::hex << (int)fecSR0 << std::endl ;
    std::cout << "fecSR0 & FEC_SR0_DATATOFEC = " << (fecSR0 & FEC_SR0_DATATOFEC) << std::endl ;
    std::cerr << FecRingRegisters::decodeFECSR0 (fecSR0) << std::endl ;
#endif
    
#ifdef DEBUGGETREGISTERS
    // --------------------------------------------------------------------------------------
    // Just a warning
    if (watchdog >= TIMEOUT) {
      char msg[80] ;
      decodeKey(msg, indexOrig) ;
#ifdef DEBUGGETREGISTERS
      if (readRegistersError_) 
#endif
	std::cerr << "Warning: FecRingDevice::writeFrame: timeout on the direct acknowledge on index " << msg << std::endl ;
    }
#endif
    
    // --------------------------------------------------------------------------------------
    // No anwer
    fecSR0 = getFecRingSR0() ;
    if ( (fecSR0 & FEC_SR0_RECEMPTY) || !(fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
      
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
      std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
#endif    

      // Release the transaction number
      releaseTransactionNumber(tnum) ;

      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
#endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif

      if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
	RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_SR0NOTNOMINAL,
					   "Fails on sending a frame, ring lost",
					   CRITICALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters,
					   frameRequest_, NULL, NULL) ;
      }
      else {
	RAISEFECEXCEPTIONHANDLER_FECRING ( DD_WRITE_OPERATION_FAILED,
					   "Timeout reached on the direct acknowledge",
					   ERRORCODE, 
					   indexOrig,
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters, 
					   frameRequest_, NULL, NULL) ;
      }   
    }

    // --------------------------------------------------------------------------------------
    // Calculate the size to be readout in the FIFO receive
    // Size of dir ACK is same as frame itself plus status byte , like this:
    realSize32 = (realSize+3+1)/4 ;
    if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;

    // Check the REAL size of the received frame
    realSize = (c[0] >> 8) & 0xFF ;
    tscType8 word2 = c[0]  & 0xFF ;
    if (realSize & FEC_LENGTH_2BYTES) {
      
      realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
      // +1 for the two byte words for length
    }
    
    // +3 => Dst, Src, Length  ; +1 => status after the frame
    realSize32 = (realSize+3+1)/4 ;
    if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
    
#ifdef S2NDLEVELCHECK
    // --------------------------------------------------------------------------------------
    // Too many words ?
    if ((realSize+3) > (DD_MAX_MSG_LENGTH_32*4)) {
      
      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << "size in the direct acknowledge is bigger than the FIFO size" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
	
	// For debugging purpose display the values already read
	std::cerr << "Value read in the FIFO receive:" << std::endl ;
	for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	  std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif
      
      // Release the transaction number
      releaseTransactionNumber(tnum) ;

      RAISEFECEXCEPTIONHANDLER_FECRING ( DD_RECEIVED_MSG_TOO_LONG,
					 "Size in the direct acknowledge is bigger than the FIFO size",
					 CRITICALERRORCODE,
					 buildFecRingKey(getFecSlot(), getRingSlot()),
					 &fecRingRegisters, 
					 frameRequest_, frame, NULL) ;
    }
#endif
    
    // --------------------------------------------------------------------------------------
    // Read the frame returned
    getFifoReceive(c,realSize32);
    
    // --------------------------------------------------------------------------------------
    // Convert the 32 bits in a frame of 8 bits
    // The status is just after the frame
    cpt = 0 ;
    for (tscType32 i = 0 ; i < realSize32 ; i ++) {
      
#ifdef DEBUGMSGERROR
      std::cout << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
#endif
      
      frame[cpt++] = (c[i] >> 24) & 0xFF ;
      frame[cpt++] = (c[i] >> 16) & 0xFF ;
      frame[cpt++] = (c[i] >>  8) & 0xFF ;
      frame[cpt++] = (c[i])       & 0xFF ;
    }
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    std::cout << __func__ << ": Direct acknowledge ( { " ;
    for (int i = 0 ; i < ((realSize+3)+1) ; i ++ )
      std::cout << "0x" << std::hex << (int)frame[i] << " " ;
    std::cout << "} )" << std::dec << std::endl ;
#endif
    
    // --------------------------------------------------------------------------------------
    // Retreive the size and the transaction number
    tscType8 tnumR ;
    tscType16 realSize1 = frame[2] ;
    if (frame[2] & FEC_LENGTH_2BYTES) {
      
      realSize1 = ((frame[2] & 0x7F) << 8) + frame[3] + 1 ;
      // +1 for the two byte words for length
	
      // get the transaction number
      // Dst, Src, Length1, Length2, Channel, Transaction, Cmd, data
      tnumR = frame[5] ;
    }
    else {

      // get the transaction number
      // Dst, Src, Length, Channel, Transaction, Cmd, data
      tnumR = frame[4] ;
    }
    
#ifdef DEBUGGETREGISTERS
    if (readRegistersError_) // make a copy of the frame for future error
      memcpy (dAck_, frame, (realSize1+3+1)*sizeof(tscType8)) ;
#endif
    
#ifdef S2NDLEVELCHECK
    // --------------------------------------------------------------------------------------
    // Problem in the size the frame, it cannot be !!!!!
    if (realSize1 != realSize) {
      
#ifdef DEBUGMSGERROR_DISPLAYFRAME
      std::cerr << "size between the original frame and the direct acknowledge is changed => cannot be !" << std::endl ;
#endif
      
      // Release the transaction number
      releaseTransactionNumber(tnum) ;	
      
      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ": size between the original frame and the direct acknowledge is changed => cannot be !" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
	
	// For debugging purpose display the values already read
	std::cerr << "Value read in the FIFO receive:" << std::endl ;
	for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	  std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif
      
      RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_PROBLEMINFRAME,
					 "Corrupted direct acknowledge status: size between the original frame and the direct acknowledge is changed",
					 CRITICALERRORCODE,
					 indexOrig,
					 "Size received", realSize1,
					 &fecRingRegisters,
					 frameRequest_, dAck_, NULL) ;
    }
#endif
    
    // --------------------------------------------------------------------------------------
    // Problem in transaction number
    if (tnum != tnumR) {
      
#ifdef DEBUGMSGERROR_DISPLAYFRAME
      std::cerr << "The original transaction number and the direct ack. transction number is different" << std::endl ;
#endif
      
      // Release the transaction number
      releaseTransactionNumber(tnum) ;	
      
      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ": The original transaction number and the direct ack. transction number is different" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
	// For debugging purpose display the values already read
	std::cerr << "Value read in the FIFO receive:" << std::endl ;
	for (tscType32 i = 0 ; i < 3 ; i ++) {
	  std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif
      
      RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_PROBLEMINFRAME,
					 "Corrupted direct acknowledge status: the original transaction number and the direct ack. transaction number is different", 
					 CRITICALERRORCODE,
					 indexOrig,
					 "transaction number received (orig = " + toString(tnum) + ")", tnumR,
					 &fecRingRegisters,
					 frameRequest_, dAck_, NULL) ;
    }
    
    // --------------------------------------------------------------------------------------
    // Check the status of the frame
#ifdef DEBUGMSGERROR
   std::cout << "Direct ack = 0x" << std::hex << (int)frame[realSize + 3] << " (" << std::dec << (int)(frame[realSize + 3] & FECACKNOERROR32) << ")" << std::endl ;
#endif

    // Check the direct acknowledge
    if (! ignoreDirectAck && (frame[realSize + 3] != FECACKNOERROR32)) {
      
      // Release the transaction number
      releaseTransactionNumber(tnum) ;
      
      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ": Direct ack = 0x" << std::hex << (int)frame[realSize + 3] << " (" << std::dec << (int)(frame[realSize + 3] & FECACKNOERROR32) << ")" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
	// For debugging purpose display the values already read
	std::cerr << "Value read in the FIFO receive:" << std::endl ;
	for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	  std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif
      RAISEFECEXCEPTIONHANDLER_FECRING ( DD_DATA_CORRUPT_ON_WRITE,
					 "Bad direct acknowledge status",
					 ERRORCODE,
					 indexOrig,
					 "direct acknowledge", frame[realSize +3],
					 &fecRingRegisters,
					 frameRequest_, dAck_, NULL) ;
    }
  }
  // End of check of the direct acknowledge
  // --------------------------------------------------------------------------------

  if (! readCommand) {
    // Release the transaction number
    releaseTransactionNumber(tnum) ;
  }

#ifdef DEBUGMSGERROR
  fecSR0 = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice::writeFrame: end and the SR0 is " << std::hex << fecSR0 << std::endl ;
#endif

}

/** Send a frame over the ring. The frame is defined in the CCU documentation for
 * all the channels and CCU. The direct acknowledge is checked and returned in case of
 * error.
 * The frame is 8 bits size array and the conversion to the 32 bits is done in this method.
 * \param frame - frame to be sent. The size of the frame is DD_USER_MAX_MSG_LENGTH. At the
 * end of the method the frame returned is the direct acknoledge.
 * \param ignoreDirectAck - ignore the direct acknowledge is true, by default false [OPTION parameter]
 * \return Error code
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 * \warning The direct acknowledge is checked in this method, in case of bad status an exception is raised.
 * \warning The maximum size in the frame can not greater than 0x7F for one byte length or
 * 0x7FFF for two bytes length. The word 0x80 means that the length of the frame is two 
 * bytes long.
 * \warning the hard code of the length is given by the lines below:
 * <ul>
 * <li> Longueur   Codage de la taille  Lenh Lenl
 * <li> 0 - 127    1 mot                -    0x7F
 * <li> 0 - 255    2 mots               0x80 0xFF
 * <li> 256 - 1024 2 mots               0x9F 0xFF
 * </ul>
 * \author Laurent Gross for the sending and receiving frame
 */
void FecRingDevice::writeFrameDelayed ( tscType8 *frame, bool ignoreDirectAck ) 
  throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  tscType16 fecSR0D = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice(" << std::dec << (int)getFecSlot() << "." << (int)getRingSlot() << ")::writeFrameDelayed: begin and the SR0 is " << std::hex << fecSR0D << std::endl ;
#endif

  // Force acknowledge set in order to wait for the data to FEC
  bool forceAck = true ;

  // --------------------------------------------------------------------------------------
  // Index to the FEC/ring/CCU/channel/device
  keyType indexOrig = buildCompleteKey(getFecSlot(), getRingSlot(), frame[0], frame[2] &  FEC_LENGTH_2BYTES ? frame[4] : frame[3], frame[2] &  FEC_LENGTH_2BYTES ? frame[7] : frame[6]) ;
  if (!isi2cChannelCcu25 (indexOrig))
    indexOrig = buildCompleteKey(getFecSlot(), getRingSlot(), frame[0], frame[2] &  FEC_LENGTH_2BYTES ? frame[4] : frame[3], 0) ;

  // --------------------------------------------------------------------------------------
  // next transaction number
  tscType8 tnum = getNextTransactionNumber();

  // No more transaction number available
  if (tnum == 0) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( DD_NO_MORE_FREE_TRANSACTION_NUMBER,
					    "No more transaction number available, all are in used",
					    ERRORCODE,
					    buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }

  // --------------------------------------------------------------------------------------
  // Check the size of the frame
  bool readCommand = false ;
  tscType16 realSize = frame[2] ;
  if (realSize & FEC_LENGTH_2BYTES) {
  
    realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1 ;
    // +1 for the two byte words for length
 
    // Put the transaction number
    // Dst, Src, Length1, Length2, Channel, Transaction, Cmd, data

    if (frame[5] == 0xFE) {
#ifdef DEBUGMSGERROR
      std::cout << "Read command" << std::endl ;
#endif
      readCommand = true ;
    }

    frame[5] = tnum ;
  }
  else {

    // Check if it is a read command
    if (frame[4] == 0xFE) {
#ifdef DEBUGMSGERROR
      std::cout << "Read command" << std::endl ;
#endif
      readCommand = true ;
    }

    // Put the transaction number
    // Dst, Src, Length, Channel, Transaction, Cmd, data
    frame[4] = tnum ;
  }

  // --------------------------------------------------------------------------------------
  // make a copy of the frame for future error
#ifdef DEBUGGETREGISTERS
  if (readRegistersError_) 
#endif
  if (!reconfigurationRunning_) {

#ifdef DEBUGFRAMES
    memcpy (frameHistory_[frameHistoryCounter_], frameRequest_, DD_USER_MAX_MSG_LENGTH) ;
    frameHistoryCounter_ ++ ;
    if (frameHistoryCounter_ == MAXHISTORYFRAME) turnOne_ = true ;
    frameHistoryCounter_ = frameHistoryCounter_ % MAXHISTORYFRAME ;
#endif

    memcpy (frameRequest_, frame, (realSize+3)*sizeof(tscType8)) ;
    memset (dAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    memset (fAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
  }

  // --------------------------------------------------------------------------------------
#ifdef S2NDLEVELCHECK
  // Too many words ?
  if ((realSize+3) > (DD_MAX_MSG_LENGTH_32*4)) {

#ifdef DEBUGMSGERROR_DISPLAYFRAME
    std::cerr << "RING WRITE ERROR: size in frame is bigger than the FIFO size => cannot be !" << std::endl ;
#endif

    // Release the transaction number
    releaseTransactionNumber(tnum) ;

    RAISEFECEXCEPTIONHANDLER_FECRING ( DD_TOO_LONG_FRAME_LENGTH,
				       "Size in frame is bigger than the FIFO size",
				       CRITICALERRORCODE,
				       buildFecRingKey(getFecSlot(), getRingSlot()),
				       NULL, frameRequest_, NULL, NULL) ;
  }
#endif

#ifdef DEBUGMSGERROR_DISPLAYFRAME
  std::cout << __func__ << " ( { " ;
  for (int i = 0 ; i < (realSize+3) ; i ++ )
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  std::cout << "} )" << std::dec << std::endl ;
#endif

  // --------------------------------------------------------------------------------------
  // Check the FEC ring SR0
  tscType16 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0)) {

    if (reconfigurationRunning_) {

      std::cerr << "The SR0 0x" << std::hex << fecSR0 << " is not nominal" << std::dec << std::endl ;
    }
    else {

      // filter the frames to try to recover it if the ring is closed
      if (fecSR0 & FEC_SR0_LINKINITIALIZED) {

	if (!(fecSR0 & FEC_SR0_RECEMPTY)) {
	  checkFifoReceive ( ) ;
	  // Check again the SR0
	  fecSR0 = getFecRingSR0() ;
	}
      }

      // if the IRQ is on: The IRQ must be cleared at this point if the readFrame was not called
      // This method does clear the IRQ after this clear
      if ((fecSR0 & FEC_SR0_PENDINGIRQ) || (fecSR0 & FEC_SR0_DATATOFEC)) {
	setFecRingCR1 (FEC_CR1_CLEARIRQ);
	// Check again the SR0
	fecSR0 = getFecRingSR0() ;
      }

      // Check again the SR0
      if (! isFecSR0Correct(fecSR0)) {
#ifdef DEBUGMSGERROR_DISPLAYFRAME
	std::cerr << "The SR0 0x" << std::hex << fecSR0 << " is not nominal, the frame is not sent" 
		  << std::dec << std::endl ;
	
	tscType16 fecSR1 = getFecRingSR1() ;
	tscType16 fecCR0 = getFecRingCR0() ;
	std::cerr << "The SR1 0x" << std::hex << fecSR1 << std::dec << std::endl ;
	std::cerr << "The CR0 0x" << std::hex << fecCR0 << std::dec << std::endl ;
#endif    
	
	// Release the transaction number
	releaseTransactionNumber(tnum) ;	

	// Retreive if needed the registers of FEC and CCU
	FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
	if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

	  std::cerr << "--------------------------------------------------------------------" << std::endl ;
	  std::cerr << "Error starts here with that display" << std::endl ;
	  std::cerr << __func__ << ": The SR0 0x" << std::hex 
		    << fecSR0 << " is not nominal, the frame is not sent" 
		    << std::dec << std::endl ;
	  
#  ifdef DEBUGFRAMES
	  // For debugging purpose display the frames already sent
	  std::cerr << "Display previous frames (history)" << std::endl ;
	  if (turnOne_) {
	    for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	      std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	    }
	  }
	  for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
#endif

	  // Read the registers for debugging
	  fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
	}
#endif

	RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_SR0NOTNOMINAL,
					   "Fails on sending a frame, ring lost",
					   FATALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters,
					   frameRequest_, NULL, NULL) ;
      }
    }
  }

  // --------------------------------------------------------------------------------------
  // Channel for error and check if the channel is busy only if the CCU i2c channel is configure
  // in force acknwoledge
  ccuMapAccessedType::iterator ccu =  ccuMapAccess_.find(getCcuKey(indexOrig)) ;
  if ( ((ccu == ccuMapAccess_.end()) || (ccu->second == NULL) || (!ccu->second->getBitForceAck(getChannelKey(indexOrig)))) &&
       (isi2cChannelCcu25 (indexOrig) || isi2cChannelOldCcu(indexOrig))
       ) {
    
    forceAck = false ;
    unsigned long watchdog = 0 ;
    while ( isChannelBusy(indexOrig) && (watchdog++ < TIMEOUT) ) {

#ifdef DEBUGMSGERROR
      std::cout << "Channel was busy, waiting ..." << std::endl ;
      std::cout << __func__ << " ( { " ;
      for (int i = 0 ; i < (realSize+3) ; i ++ )
	std::cout << "0x" << std::hex << (int)frame[i] << " " ;
      std::cout << "} )" << std::dec << std::endl ;
#endif
    }
  }

  // --------------------------------------------------------------------------------------
  // Build the frame in 32 bits and send it over the ring
  tscType32 c[DD_USER_MAX_MSG_LENGTH], realSize32 = 0, cpt = 0 ;

#ifdef DEBUGFRAMES
  for (int i = 0 ; i < DD_USER_MAX_MSG_LENGTH ; i ++) c[i] = 0 ;
#endif

  realSize32 = (realSize+3)/4 ;
  if ( ((realSize+3) % 4) != 0 ) realSize32 += 1 ;

  for (tscType32 i = 0 ; i < realSize32 ; i ++) {

    c[i] = (frame[cpt] << 24) | (frame[cpt+1] << 16) | (frame[cpt+2] << 8) | frame[cpt+3] ;
    
#ifdef DEBUGMSGERROR
    std::cout << "c[" << std::dec << i << "] = 0x" << std::hex << c[i] << ", frame[" << std::dec << cpt << "] =  0x" << std::hex << (int)frame[cpt] << std::endl ;
#endif

    // Next word
    cpt += 4 ;
  }

  // --------------------------------------------------------------------------------------
  // Clear the error bit
  setFecRingCR1 ( DD_FEC_CLEAR_ERRORS ) ;

  // --------------------------------------------------------------------------------------
  // WB add-on: but here in block mode
  setFifoTransmit(c,realSize32); 

#ifdef S2NDLEVELCHECK
  // --------------------------------------------------------------------------------------
  // Check that the FIFO transmit is not empty
  if (getFecRingSR0() & FEC_SR0_TRAEMPTY) {
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    std::cerr << __func__ << ": FIFO transmit empty, cannot send the frame" << std::endl ;
#endif    
    
    // Release the transaction number
    releaseTransactionNumber(tnum) ;

    // Retreive if needed the registers of FEC and CCU
    FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
    if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

      std::cerr << "--------------------------------------------------------------------" << std::endl ;
      std::cerr << "Error starts here with that display" << std::endl ;
      std::cerr << __func__ << ": FIFO transmit empty, cannot send the frame" << std::endl ;
      
#  ifdef DEBUGFRAMES
      // For debugging purpose display the frames already sent
      std::cerr << "Display previous frames (history)" << std::endl ;
      if (turnOne_) {
	for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
      }
      for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
      }
#  endif

      // Read the registers for debugging
      fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
    }
#endif

    RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_INVALIDOPERATION,
				       "FIFO transmit empty and you try to toggle the bit send (CR0[3])"
				       ERRORCODE,
				       buildFecRingKey(getFecSlot(), getRingSlot()),
				       &fecRingRegisters, 
				       frameRequest_, NULL, NULL) ;
  }
#endif

  // --------------------------------------------------------------------------------------
  // Toggle the send bit of CTRLO with RMW operation
  setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_OR ) ;
  setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_XOR ) ;

  // --------------------------------------------------------------------------------------
  // Wait for the FIFO transmit is not running only if no reconfiguration is currently done
  unsigned long watchdog = 0;
  if (reconfigurationRunning_) {
#ifdef DEBUGMSGERROR
    std::cout << "Reconfiguration running" << std::endl ;
#endif
    watchdog = TIMEOUT ;
  }

  fecSR0 = getFecRingSR0() ;
  while ( ((fecSR0 & FEC_SR0_TRARUN) || (!(fecSR0 & FEC_SR0_TRAEMPTY))) && (watchdog++ < loopInTimeWriteFrame_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
    struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 50000000 ;
    nanosleep (&req,NULL) ;
    //usleep (TIMETOWAIT) ;
    fecSR0 = getFecRingSR0() ;
  }

  // --------------------------------------------------------------------------------------
  // Check that the FIFO transmit is not running
  fecSR0 = getFecRingSR0() ;
  if ( (fecSR0 & FEC_SR0_TRARUN) || !(fecSR0 & FEC_SR0_LINKINITIALIZED) ) {

#ifdef DEBUGMSGERROR_DISPLAYFRAME 
      std::cerr << __func__ << ": FIFO transmit running bit is always ON (SR0 = " 
		<< std::hex << fecSR0 << ")" << std::dec << std::endl ;
#endif    

      // Release the transaction number
      releaseTransactionNumber(tnum) ;

      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ":  FIFO transmit running bit is always ON (SR0 = " 
		  << std::hex << fecSR0 << ")" << std::dec << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif

      if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
	RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_SR0NOTNOMINAL,
					   "Fails on sending a frame, ring lost",
					   CRITICALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters,
					   frameRequest_, NULL, NULL) ;
      }
      else {
	RAISEFECEXCEPTIONHANDLER_FECRING ( DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON,
					   "FIFO transmit running bit is always on (CR0[3])",
					   CRITICALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters, 
					   frameRequest_, NULL, NULL) ;
      }
  }
  
#ifdef DEBUGMSGERROR
  if ( (getChannelKey(indexOrig) >= 0x10) && (getChannelKey(indexOrig) <= 0x1F) )
    std::cout << "# WAIT FIFO TRANSMIT: loopInTimeWriteFrame: " << std::dec << watchdog << "/" << loopInTimeWriteFrame_ << " for the device 0x" << std::hex << indexOrig << std::endl ;
#endif
  
#ifdef RECALCULATETIMEOUT
  // --------------------------------------------------------------------------------------
  // Update the timeout
  if ( (loopInTimeWriteFrame_ == TIMEOUT) && (watchdog < TIMEOUT) ) {
    if (!watchdog) watchdog = 1000 ;
    loopInTimeWriteFrame_ = watchdog*2 ;
    
#  ifdef DEBUGMSGERROR
    std::cout << "loopInTimeWriteFrame_ = " << std::dec << loopInTimeWriteFrame_ << std::endl ;
#  endif
  }
#endif
  
  // ---------------------------------------------------------------------------
  // Wait for the direct ack only if 
  if (!reconfigurationRunning_) {
    
    // now, wait for the direct ack ..
    watchdog = 0 ;
    fecSR0 = getFecRingSR0() ;
    
    // ---------------------------------------------------------------------------
    // Waiting on the pending IRQ
    //while ( !(fecSR0 & FEC_SR0_PENDINGIRQ) && (watchdog++ < loopInTimeDirectAck_) ) {
    //if (!isi2cChannelCcu25(indexOrig) || !forceAck) {
    if (true) {
      while ( !(fecSR0 & FEC_SR0_PENDINGIRQ) && (watchdog++ < loopInTimeDirectAck_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 50000000 ;
	nanosleep (&req,NULL) ;
	//usleep (TIMETOWAIT) ;
	fecSR0 = getFecRingSR0() ;
      }
      
#ifdef DEBUGMSGERROR
      if ( (getChannelKey(indexOrig) >= 0x10) && (getChannelKey(indexOrig) <= 0x1F) )
	std::cout << "# WAIT DIRECT ACK: loopInTimeDirectAck: " << std::dec << watchdog << "/" << loopInTimeDirectAck_ << " for the device 0x" << std::hex << indexOrig << std::endl ;
#endif
      
#ifdef RECALCULATETIMEOUT
      // --------------------------------------------------------------------------------------
      // Update the timeout
      if ( (loopInTimeDirectAck_ == TIMEOUT) && (watchdog < TIMEOUT) ) {
	if (!watchdog) watchdog = 1000 ;
	loopInTimeDirectAck_ = watchdog*2 ;
	
#  ifdef DEBUGMSGERROR
	std::cout << "loopInTimeDirectAck_ = " << std::dec << loopInTimeDirectAck_ << std::endl ;
#  endif
      }
#endif
    }
    else {
      while ( (!(fecSR0 & FEC_SR0_PENDINGIRQ) || !(fecSR0 & FEC_SR0_DATATOFEC)) && (watchdog++ < loopInTimeDirectAck_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
	fecSR0 = getFecRingSR0() ;
      }
    }
    
#ifdef DEBUGMSGERROR
    std::cout << "SR0 = 0x" << std::hex << (int)fecSR0 << std::endl ;
    std::cout << "fecSR0 & FEC_SR0_DATATOFEC = " << (fecSR0 & FEC_SR0_DATATOFEC) << std::endl ;
    std::cerr << FecRingRegisters::decodeFECSR0 (fecSR0) << std::endl ;
#endif
    
#ifdef DEBUGGETREGISTERS
    // --------------------------------------------------------------------------------------
    // Just a warning
    if (watchdog >= TIMEOUT) {
      char msg[80] ;
      decodeKey(msg, indexOrig) ;
#ifdef DEBUGGETREGISTERS
      if (readRegistersError_) 
#endif
	std::cerr << "Warning: FecRingDevice::writeFrameDelayed: timeout on the direct acknowledge on index " << msg << std::endl ;
    }
#endif
    
    // --------------------------------------------------------------------------------------
    // No anwer
    fecSR0 = getFecRingSR0() ;
    if ( (fecSR0 & FEC_SR0_RECEMPTY) || !(fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
      
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
      std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
#endif    

      // Release the transaction number
      releaseTransactionNumber(tnum) ;

      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
#endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif

      if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
	RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_SR0NOTNOMINAL,
					   "Fails on sending a frame, ring lost",
					   CRITICALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters,
					   frameRequest_, NULL, NULL) ;
      }
      else {
	RAISEFECEXCEPTIONHANDLER_FECRING ( DD_WRITE_OPERATION_FAILED,
					   "Timeout reached on the direct acknowledge",
					   ERRORCODE,
					   indexOrig,
					   "FEC status register 0", fecSR0,
					   &fecRingRegisters, 
					   frameRequest_, NULL, NULL) ;
      }      
    }

    // --------------------------------------------------------------------------------------
    // Calculate the size to be readout in the FIFO receive
    // Size of dir ACK is same as frame itself plus status byte , like this:
    realSize32 = (realSize+3+1)/4 ;
    if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;

    // Check the REAL size of the received frame
    realSize = (c[0] >> 8) & 0xFF ;
    tscType8 word2 = c[0]  & 0xFF ;
    if (realSize & FEC_LENGTH_2BYTES) {
      
      realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
      // +1 for the two byte words for length
    }
    
    // +3 => Dst, Src, Length  ; +1 => status after the frame
    realSize32 = (realSize+3+1)/4 ;
    if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
    
#ifdef S2NDLEVELCHECK
    // --------------------------------------------------------------------------------------
    // Too many words ?
    if ((realSize+3) > (DD_MAX_MSG_LENGTH_32*4)) {
      
      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << "size in the direct acknowledge is bigger than the FIFO size" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
	
	// For debugging purpose display the values already read
	std::cerr << "Value read in the FIFO receive:" << std::endl ;
	for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	  std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif
      
      // Release the transaction number
      releaseTransactionNumber(tnum) ;

      RAISEFECEXCEPTIONHANDLER_FECRING ( DD_RECEIVED_MSG_TOO_LONG,
					 "Size in the direct acknowledge is bigger than the FIFO size",
					 CRITICALERRORCODE,
					 buildFecRingKey(getFecSlot(), getRingSlot()),
					 "Size", realSize+3,
					 &fecRingRegisters, 
					 frameRequest_, frame, NULL);
    }
#endif
    
    // --------------------------------------------------------------------------------------
    // Read the frame returned
    getFifoReceive(c,realSize32);
    
    // --------------------------------------------------------------------------------------
    // Convert the 32 bits in a frame of 8 bits
    // The status is just after the frame
    cpt = 0 ;
    for (tscType32 i = 0 ; i < realSize32 ; i ++) {
      
#ifdef DEBUGMSGERROR
      std::cout << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
#endif
      
      frame[cpt++] = (c[i] >> 24) & 0xFF ;
      frame[cpt++] = (c[i] >> 16) & 0xFF ;
      frame[cpt++] = (c[i] >>  8) & 0xFF ;
      frame[cpt++] = (c[i])       & 0xFF ;
    }
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    std::cout << __func__ << ": Direct acknowledge ( { " ;
    for (int i = 0 ; i < ((realSize+3)+1) ; i ++ )
      std::cout << "0x" << std::hex << (int)frame[i] << " " ;
    std::cout << "} )" << std::dec << std::endl ;
#endif
    
    // --------------------------------------------------------------------------------------
    // Retreive the size and the transaction number
    tscType8 tnumR ;
    tscType16 realSize1 = frame[2] ;
    if (frame[2] & FEC_LENGTH_2BYTES) {
      
      realSize1 = ((frame[2] & 0x7F) << 8) + frame[3] + 1 ;
      // +1 for the two byte words for length
	
      // get the transaction number
      // Dst, Src, Length1, Length2, Channel, Transaction, Cmd, data
      tnumR = frame[5] ;
    }
    else {

      // get the transaction number
      // Dst, Src, Length, Channel, Transaction, Cmd, data
      tnumR = frame[4] ;
    }
    
#ifdef DEBUGGETREGISTERS
    if (readRegistersError_) // make a copy of the frame for future error
      memcpy (dAck_, frame, (realSize1+3+1)*sizeof(tscType8)) ;
#endif
    
#ifdef S2NDLEVELCHECK
    // --------------------------------------------------------------------------------------
    // Problem in the size the frame, it cannot be !!!!!
    if (realSize1 != realSize) {
      
#ifdef DEBUGMSGERROR_DISPLAYFRAME
      std::cerr << "size between the original frame and the direct acknowledge is changed => cannot be !" << std::endl ;
#endif
      
      // Release the transaction number
      releaseTransactionNumber(tnum) ;	
      
      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ": size between the original frame and the direct acknowledge is changed => cannot be !" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
	
	// For debugging purpose display the values already read
	std::cerr << "Value read in the FIFO receive:" << std::endl ;
	for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	  std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif
      
     RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_PROBLEMINFRAME,
					"Corrupted direct acknowledge status: size between the original frame and the direct acknowledge is changed",
					CRITICALERRORCODE,
					indexOrig,
					"Size received", realSize1,
					&fecRingRegisters,
					frameRequest_, dAck_, NULL) ;
    }
#endif
    
    // --------------------------------------------------------------------------------------
    // Problem in transaction number
    if (tnum != tnumR) {
      
#ifdef DEBUGMSGERROR_DISPLAYFRAME
      std::cerr << "The original transaction number and the direct ack. transction number is different" << std::endl ;
#endif
      
      // Release the transaction number
      releaseTransactionNumber(tnum) ;	
      
      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ": The original transaction number and the direct ack. transction number is different" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
	// For debugging purpose display the values already read
	std::cerr << "Value read in the FIFO receive:" << std::endl ;
	for (tscType32 i = 0 ; i < 3 ; i ++) {
	  std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif

      RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_PROBLEMINFRAME,
					 "Corrupted direct acknowledge status: the original transaction number and the direct ack. transaction number is different", 
					 CRITICALERRORCODE,
					 indexOrig,
					 "transaction number received (orig = " + toString(tnum) + ")", tnumR,
					 &fecRingRegisters,
					 frameRequest_, dAck_, NULL) ;
    }
    
    // --------------------------------------------------------------------------------------
    // Check the status of the frame
#ifdef DEBUGMSGERROR
    std::cout << "Direct ack = 0x" << std::hex << (int)frame[realSize + 3] << " (" << std::dec << (int)(frame[realSize + 3] & FECACKNOERROR32) << ")" << std::endl ;
#endif

    // Check the direct acknowledge
    if (! ignoreDirectAck && (frame[realSize + 3] != FECACKNOERROR32)) {
      
      // Release the transaction number
      releaseTransactionNumber(tnum) ;
      
      // Retreive if needed the registers of FEC and CCU
      FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
      if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
	
	std::cerr << "--------------------------------------------------------------------" << std::endl ;
	std::cerr << "Error starts here with that display" << std::endl ;
	std::cerr << __func__ << ": Direct ack = 0x" << std::hex << (int)frame[realSize + 3] << " (" << std::dec << (int)(frame[realSize + 3] & FECACKNOERROR32) << ")" << std::endl ;
	
#  ifdef DEBUGFRAMES
	// For debugging purpose display the frames already sent
	std::cerr << "Display previous frames (history)" << std::endl ;
	if (turnOne_) {
	  for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	    std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	  }
	}
	for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
	// For debugging purpose display the values already read
	std::cerr << "Value read in the FIFO receive:" << std::endl ;
	for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	  std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
	}
#  endif
	
	// Read the registers for debugging
	fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
      }
#endif
	
      RAISEFECEXCEPTIONHANDLER_FECRING ( DD_DATA_CORRUPT_ON_WRITE,
					 "bad direct acknowledge status",
					 ERRORCODE,
					 indexOrig,
					 "direct acknowledge", frame[realSize +3],
					 &fecRingRegisters,
					 frameRequest_, dAck_, NULL) ;
      
    }
  }
  // End of check of the direct acknowledge
  // --------------------------------------------------------------------------------

  if (! readCommand) {
    // Release the transaction number
    releaseTransactionNumber(tnum) ;
  }

#ifdef DEBUGMSGERROR
  fecSR0 = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice::writeFrameDelayed: end and the SR0 is " << std::hex << fecSR0 << std::endl ;
#endif

}


/** Read a frame in the FEC FIFO received. This frame can be a the answer to
 * a read command or the force acknowledge frame.
 * \param transaction - transaction number (value returned by the device driver)
 * \param frame - output frame (if an error appears, the frame must contain the original request frame)
 * \param expectedSize - this option (default 0 so no check) is dedicated to i2c command in order to check if the size is correct and if the status of the SRA is also correct
 * \return local error from the device driver
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 * \author Laurent Gross for the sending and receiving frame
 */
void FecRingDevice::readFrame ( tscType8 transaction, tscType8 *frame, tscType8 expectedSize )
  throw (FecExceptionHandler) {

  tscType16 fecSR0 ;

#ifdef DEBUGMSGERROR
  fecSR0 = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice::readFrame begin and the SR0 is " 
	    << std::hex << fecSR0 << std::dec << std::endl ;
#endif

  // --------------------------------------------------------------------------------------
  // Index to the FEC/ring/CCU/channel/device
  keyType indexOrig = buildCompleteKey(getFecSlot(), getRingSlot(), frame[0], frame[2] &  FEC_LENGTH_2BYTES ? frame[4] : frame[3], frame[2] &  FEC_LENGTH_2BYTES ? frame[7] : frame[6]) ;
  if (!isi2cChannelCcu25 (indexOrig))
    indexOrig = buildCompleteKey(getFecSlot(), getRingSlot(), frame[0], frame[2] &  FEC_LENGTH_2BYTES ? frame[4] : frame[3], 0) ;

  // --------------------------------------------------------------------------------------
  // Build the frame in 32 bits
  tscType32 c[DD_USER_MAX_MSG_LENGTH], realSize32 = 0, realSize = 0 ;

#ifdef DEBUGFRAMES
  for (int i = 0 ; i < DD_USER_MAX_MSG_LENGTH ; i ++) c[i] = 0 ;
#endif

  // --------------------------------------------------------------------------------------
  // Wait for incoming data (with a timeout)
  unsigned long watchdog = 0 ;
  fecSR0 = getFecRingSR0() ;
  while ( !(fecSR0 & FEC_SR0_DATATOFEC) && (watchdog++ < loopInTimeReadFrame_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
    struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 50000000 ;
    nanosleep (&req,NULL) ;
    //usleep (TIMETOWAIT) ;
#endif
    fecSR0 = getFecRingSR0() ;
  }

#ifdef DEBUGTIMING
  if ( (getChannelKey(indexOrig) >= 0x10) && (getChannelKey(indexOrig) <= 0x1F) ) {
    std::cout << "# WAIT FORCE ACK for FEC " << std::dec << (int)fecSlot_ << "." << (int)ringSlot_ << ": loopInTimeReadFrame: " << std::dec << watchdog << "/" << loopInTimeReadFrame_ << " for the device 0x" << std::hex << indexOrig ;

    if ( (loopInTimeReadFrame_ == TIMEOUT) && (watchdog < TIMEOUT) && isi2cChannelCcu25(indexOrig) ) {
      if (!watchdog) watchdog = 1000 ;
      std::cout << " (recalculcation time is " << std::dec << (watchdog*10) << ")" << std::endl ;
    }
  }
#endif

#ifdef RECALCULATETIMEOUT
  // --------------------------------------------------------------------------------------
  // Update the timeout
  if ( (loopInTimeReadFrame_ == TIMEOUT) && (watchdog < TIMEOUT) && isi2cChannelCcu25(indexOrig) ) {
    if (!watchdog) watchdog = 1000 ;
    loopInTimeReadFrame_ = watchdog*10 ;
    
#  ifdef DEBUGMSGERROR
    std::cout << "loopInTimeReadFrame_ = " << std::dec << loopInTimeReadFrame_ << std::endl ;
#  endif
  }
#endif
  
  // --------------------------------------------------------------------------------------
  //clean up interrupts and unset the DATATOFEC bit in SR0
  setFecRingCR1 (FEC_CR1_CLEARIRQ);

  // Just a warning
  if (watchdog >= TIMEOUT) 
#ifdef DEBUGGETREGISTERS
    if (readRegistersError_)
#endif
      { 
	std::cerr << "Warning: FecRingDevice::readFrame: timeout on the read anwser of the force "; 
	std::cerr << "acknowledge (SR0 = 0x" << std::hex << fecSR0 << ")" << std::dec << std::endl ;
      }

  // read the SR0
  fecSR0 = getFecRingSR0() ;

  // Check the receive FIFO if it is not empty or if the ring brakes during waiting the answer
  if ((fecSR0 & FEC_SR0_RECEMPTY) || !(fecSR0 & FEC_SR0_LINKINITIALIZED)) {

#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    std::cerr << __func__ << ": Cannot read the frame, timeout has been reached for transaction " << (int)transaction << std::endl ;
    std::cout << __func__ << ": Orginal request frame ( { " ;
    for (int i = 0 ; i < ((frame[2]+3)+1) ; i ++ )
      std::cout << "0x" << std::hex << (int)frame[i] << " " ;
    std::cout << "} )" << std::dec << std::endl ;
#endif

    // Retreive if needed the registers of FEC and CCU
    FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
    if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

      std::cerr << "--------------------------------------------------------------------" << std::endl ;
      std::cerr << "Error starts here with that display" << std::endl ;
      std::cerr << __func__ << ": Cannot read the frame, timeout has been reached for transaction " << (int)transaction << std::endl ;
    
#  ifdef DEBUGFRAMES
      // For debugging purpose display the frames already sent
      std::cerr << "Display previous frames (history)" << std::endl ;
      if (turnOne_) {
	for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
      }
      for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
      }
#endif

      // Read the registers for debugging
      fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
    }
#endif

    // Release the transaction number
    releaseTransactionNumber(transaction) ;

    if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
      RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_SR0NOTNOMINAL,
					 "Fails on sending a frame, ring lost",
					 CRITICALERRORCODE,
					 buildFecRingKey(getFecSlot(), getRingSlot()),
					 "FEC status register 0", fecSR0,
					 &fecRingRegisters,
					 frameRequest_, dAck_, NULL) ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_FECRING ( DD_CANNOT_READ_DATA,
					 "Unable to read a frame or the force acknowledge, timeout reached",
					 ERRORCODE,
					 indexOrig,
					 "FEC status register 1", getFecRingSR1(),
					 &fecRingRegisters,
					 frameRequest_, dAck_, NULL) ;
    }
  }

  // Read the answer
  // Retreive the size
  c[0] = getFifoReceive() ;

  // Check the size of the frame
  realSize = (c[0] >> 8) & 0xFF ;
  tscType8 word2 = c[0]  & 0xFF ;
  if (realSize & FEC_LENGTH_2BYTES) {
  
    realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
    // +1 for the two byte words for length
  }

  // +3 => Dst, Src, Length  ; +1 => status after the frame
  realSize32 = (realSize+3+1)/4 ;
  if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;

#ifdef DEBUGGETREGISTERS
  if (readRegistersError_) // make a copy of the frame for future error
    memcpy (fAck_, frame, (realSize+3)*sizeof(tscType8)) ;
#endif

  // Too many words ?
  if (realSize32 > DD_MAX_MSG_LENGTH_32) {
    
    // Retreive if needed the registers of FEC and CCU
    FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
    if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

      std::cerr << "--------------------------------------------------------------------" << std::endl ;
      std::cerr << "Error starts here with that display" << std::endl ;
      std::cerr << __func__ << ": size in frame is bigger than the FIFO size" << std::endl ;

#  ifdef DEBUGFRAMES
      // For debugging purpose display the frames already sent
      std::cerr << "Display previous frames (history)" << std::endl ;
      if (turnOne_) {
	for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
      }
      for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
      }
      // For debugging purpose display the values already read
      std::cerr << "Value read in the FIFO receive:" << std::endl ;
      for (tscType32 i = 0 ; i < 1 ; i ++) {
	std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
      }
      std::cerr << std::dec ; 
#  endif

      // Read the registers for debugging
      fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
    }
#endif

    // Release the transaction number
    releaseTransactionNumber(transaction) ;
  
    RAISEFECEXCEPTIONHANDLER_FECRING ( DD_TOO_LONG_FRAME_LENGTH,
				       "Size in frame is bigger than the FIFO size",
				       CRITICALERRORCODE,
				       indexOrig,
				       "Size of the frame received", (realSize+3+1),
				       &fecRingRegisters,
				       frameRequest_, dAck_, fAck_) ;
  }

  // Read back the frame
  getFifoReceive(c+1,realSize32-1);

  int cpt = 0 ;
  for (tscType32 i = 0 ; i < realSize32 ; i ++) {
      
#ifdef DEBUGMSGERROR
    std::cout << "c[" << std::dec << i << "] = 0x" << std::hex 
	      << c[i] << std::dec << std::endl ;
#endif
    
    frame[cpt++] = (c[i] >> 24) & 0xFF ;
    frame[cpt++] = (c[i] >> 16) & 0xFF ;
    frame[cpt++] = (c[i] >>  8) & 0xFF ;
    frame[cpt++] = (c[i])       & 0xFF ;
  }

#ifdef DEBUGGETREGISTERS
  if (readRegistersError_) // make a copy of the frame for future error
    memcpy (fAck_, frame, (realSize+3)*sizeof(tscType8)) ;
#endif

#ifdef DEBUGMSGERROR_DISPLAYFRAME 
  std::cout << __func__ << " ( { " ;
  for (tscType32 i = 0 ; i < (realSize+3+1) ; i ++ )
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  std::cout << "} )" << std::dec << std::endl ;
#endif

  // Problem in transaction number ?
  tscType8 tnum ;
  if (frame[2] &  FEC_LENGTH_2BYTES) {
    // Dst, Src, Length1, Length2, Channel, Transaction, Cmd, data
    tnum = frame[5] ;
  }
  else {
    // Dst, Src, Length, Channel, Transaction, Cmd, data
    tnum = frame[4] ;
  }

  if (tnum != transaction) {

#ifdef DEBUGMSGERROR_DISPLAYFRAME
    std::cerr << "The frame transaction number does not match with the transaction number given in argument" << std::endl ;
#endif
    
    // Retreive if needed the registers of FEC and CCU
    FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
    if ( (readRegistersError_) && (!reconfigurationRunning_) ) {
      
      std::cerr << "--------------------------------------------------------------------" << std::endl ;
      std::cerr << "Error starts here with that display" << std::endl ;
      std::cerr << __func__ << ": The frame transaction number does not match with the transaction number given in argument (" << std::dec << (int)tnum << "/" << std::dec << (int)transaction << ")" << std::endl ;
      
#  ifdef DEBUGFRAMES
      // For debugging purpose display the frames already sent
      std::cerr << "Display previous frames (history)" << std::endl ;
      if (turnOne_) {
	for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
      }
      for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
      }
      // For debugging purpose display the values already read
      std::cerr << "Value read in the FIFO receive:" << std::endl ;
      for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
      }
#  endif
      
      // Read the registers for debugging
      fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
    }
#endif

    RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_PROBLEMINFRAME,
				       "Corrupted direct acknowledge status: the original transaction number and the direct ack. transaction number is different", 
				       CRITICALERRORCODE,
				       indexOrig,
				       "transaction number received (orig = " + toString(tnum) + ")", transaction,
				       &fecRingRegisters,
				       frameRequest_, dAck_, fAck_) ;
  }
  
  // Release the transaction number
  releaseTransactionNumber(tnum) ;

  // Check the status of the frame
#ifdef DEBUGMSGERROR
  std::cout << "Status of read frame = 0x" << std::hex << (int)frame[realSize + 3] 
	    << " (" << std::dec << (frame[realSize + 3] != FECACKNOERROR32) << ")" 
	    << std::endl ;
#endif

  if ( (expectedSize != 0) && ( (frame[2] != expectedSize) || (frame[expectedSize+2] & CCUFACKERROR) ) ) {

    // Retreive if needed the registers of FEC and CCU
    FecRingRegisters fecRingRegisters ;
#ifdef DEBUGGETREGISTERS
    if ( (readRegistersError_) && (!reconfigurationRunning_) ) {

      std::cerr << "--------------------------------------------------------------------" << std::endl ;
      std::cerr << "Error starts here with that display" << std::endl ;
      std::cerr << __func__ << ": Status of read frame = 0x" << std::hex 
		<< (int)frame[realSize + 3] << " (" << std::dec 
		<< (frame[realSize + 3] != FECACKNOERROR32) << ")" << std::endl ;
      
#  ifdef DEBUGFRAMES
      // For debugging purpose display the frames already sent
      std::cerr << "Display previous frames (history)" << std::endl ;
      if (turnOne_) {
	for (unsigned int i = frameHistoryCounter_+1 ; i < MAXHISTORYFRAME ; i ++) {
	  std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
	}
      }
      for (unsigned int i = 0 ; i < frameHistoryCounter_ ; i ++) {
	std::cerr << FecRingRegisters::decodeFrame (frameHistory_[i]) << std::endl ;
      }
      // For debugging purpose display the values already read
      std::cerr << "Value read in the FIFO receive:" << std::endl ;
      for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
      }
      std::cerr << std::dec ; 
#  endif

      // Read the registers for debugging
      fecRingRegisters = getFecRingRegisters ( indexOrig ) ;
    }
#endif

    // Not an error from the device driver
    if (frame[2] != expectedSize) {
      
      RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_PROBLEMINFRAME,
					 "The size expected is different than the size received",
					 CRITICALERRORCODE,
					 indexOrig,
					 "Size in frame", frame[2],
					 &fecRingRegisters,
					 frameRequest_, dAck_, fAck_) ;
    }
    else {
      RAISEFECEXCEPTIONHANDLER_FECRING ( TSCFEC_BADFACKORREADANS,
					 "The status at the end of the frame is not correct",
					 ERRORCODE,
					 indexOrig,
					 "Answer status acknowledge", frame[expectedSize+2],
					 &fecRingRegisters,
					 frameRequest_, dAck_, fAck_) ;
    }
  }

#ifdef DEBUGMSGERROR
  fecSR0 = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice::readFrame end and the SR0 is " << std::hex <<  fecSR0 << std::endl ;
#endif

}

// -------------------------------------------------------------------------------------
//
//                                For the FEC registers 
//
// -------------------------------------------------------------------------------------

/** Set the input of the FEC to A (= false) or B (= true)
 * \param inputB - true if you want to use B, false if you want to use input A
 * \warning any change in the CR0 will keep the value set
 */
void FecRingDevice::setFecInputB ( bool inputB ) 
  throw (FecExceptionHandler) {

  if (inputB) setFecRingCR0 ( FEC_CR0_SELSERIN, CMD_OR ) ;
  else setFecRingCR0 ( FEC_CR0_SELSERIN, CMD_XOR ) ;
}

/** Set the output of the FEC to A (= false) or B (= true)
 * \param outputB - true if you want to use B, false if you want to use output A
 * \warning any change in the CR0 will keep the value set
 */
void FecRingDevice::setFecOutputB ( bool outputB ) 
  throw (FecExceptionHandler) {

  if (outputB) setFecRingCR0 ( FEC_CR0_SELSEROUT, CMD_OR ) ;
  else setFecRingCR0 ( FEC_CR0_SELSEROUT, CMD_XOR ) ;
}

/** Set the control register 0 for clock polarity (SEL_RECEIVE_CLK_POL)
 * \param invertPolarity - inverse or not the polarity
 * \warning any change in the CR0 will keep the value set
 */
void FecRingDevice::setInvertClockPolarity ( bool invertPolarity ) 
  throw (FecExceptionHandler) {

  if (invertPolarity) {
    invertClockPolarity_ = 1 ;
    setFecRingCR0 ( FEC_CR0_POLARITY, CMD_OR ) ;
  }
  else {
    invertClockPolarity_ = 0 ;
    setFecRingCR0 ( FEC_CR0_POLARITY, CMD_XOR ) ;
  }
}

/** Set the control register 0 for internal clock
 * \param internal - internal clock
 * \warning any change in the CR0 will keep the value set
 */
void FecRingDevice::setInternalClock ( bool internal )
  throw (FecExceptionHandler) {

  if (internal) setFecRingCR0 ( FEC_CR0_XTALCLOCK, CMD_OR ) ;
  else setFecRingCR0 ( FEC_CR0_XTALCLOCK, CMD_XOR ) ;
}

/** 
 * \param mask - mask to be set
 * \param op - operation to be performed
 */
 void FecRingDevice::setFecRingCR0 ( tscType16 mask, logicalOperationType op ) 
   throw (FecExceptionHandler) {

   // READ modify write
   tscType16 fecCR0 = getFecRingCR0() ;

   switch (op) {
   case CMD_OR:
#ifdef DEBUGMSGERROR //_DISPLAYFRAME
     std::cout << __func__ << " (0x" << std::hex << (int)fecCR0 << " OR 0x" << (int)mask << " = " << (int)(fecCR0 | mask) << ")" << std::endl ;
#endif
     // OR
     fecCR0 |= mask ;
     break ;
   case CMD_XOR:
#ifdef DEBUGMSGERROR //_DISPLAYFRAME
     std::cout << __func__ << " (0x" << std::hex << (int)fecCR0 << " XOR 0x" << (int)mask << " = " << (int)(fecCR0 & (~mask)) << ")" << std::endl ;
#endif
     // XOR
     fecCR0 &= (~mask) ;
     break ;
   case CMD_AND:
#ifdef DEBUGMSGERROR //_DISPLAYFRAME
     std::cout << __func__ << " (0x" << std::hex << (int)fecCR0 << " AND 0x" << (int)mask << " = " << (int)(fecCR0 & mask) << ")" << std::endl ;
#endif
     // AND
     fecCR0 &= mask ;
     break;
   case CMD_EQUAL:
#ifdef DEBUGMSGERROR //_DISPLAYFRAME
     std::cout << __func__ << " (0x" << std::hex << (int)fecCR0 << ")" << std::endl ;
#endif
     break ;
   }

   // Set the FEC register by read modify write operation
   setFecRingCR0 ( fecCR0 ) ;
 }

/** FEC reset the finite state machine of the firmware
 * \exception FecExceptionHandler
 */
void FecRingDevice::fecRingResetFSM ( ) 
  throw (FecExceptionHandler) {

  setFecRingCR0 ( FEC_CR0_RESETFSMFEC, CMD_OR ) ;
  setFecRingCR0 ( FEC_CR0_RESETFSMFEC, CMD_XOR ) ;

  // Empty all the FIFOs
  tscType16 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0)) {
    try {
      // Disable the receive FIFO
      //setCR0Receive(false);
      // Empty all the FIFOs
      emptyFifoReceive () ;
      emptyFifoTransmit() ;
      emptyFifoReturn() ;
    }
    catch (FecExceptionHandler &e) { }
    try {
      // Enable the receive FIFO
      //setCR0Receive(true);
    }
    catch (FecExceptionHandler &e) { }
  }
}

/** FEC Reset
 * \exception FecExceptionHandler
 * \warning The FEC reset FEC/CCU (but not the modules done via PIA channels for the CMS Tracker)
 */
void FecRingDevice::fecRingReset ( ) 
  throw (FecExceptionHandler) {

  setFecRingCR0 ( FEC_CR0_RESETOUT, CMD_OR ) ;
  setFecRingCR0 ( FEC_CR0_RESETOUT, CMD_XOR ) ;

  // Empty all the FIFOs
  tscType16 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0)) {
    try {
      // Empty all the FIFOs
      emptyFifoReceive () ;
      emptyFifoTransmit() ;
      emptyFifoReturn() ;
    }
    catch (FecExceptionHandler &e) { }
  }

  // Wait for the correct status
  fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0) ) usleep(1) ;

  // Delete all CCUs
  for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();p!=ccuMapAccess_.end();p++)
    delete p->second ; // Remove all the accesses for the CCU device

  ccuMapAccess_.clear() ;

  tscType16 fecCR0 = getFecRingCR0() ;  

  // Clock polarity
  invertClockPolarity_ = isInvertClockPolarity(fecCR0) ;
  
  //setFecRingCR0 (FEC_CR0_ENABLEFEC) ; // Enable FEC
  setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ; // Clear errors

  // no more reconfiguration running
  reconfigurationRunning_ = false ;

  try {
    int timeout = 0 ;
    // wait until the status is correct
    while (!isFecSR0Correct(getFecRingSR0()) && (timeout < 5)) {
      struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ; 
      nanosleep (&req,NULL) ;
      timeout ++ ;
      //usleep (TIMETOWAIT) ;
    }
  }
  catch (FecExceptionHandler &e) { }
}

/** FEC Reset on the ring B, the CCU are reseted
 * \exception FecExceptionHandler
 * \warning The FEC reset FEC/CCU (but not the modules done via PIA channels for the CMS Tracker)
 */
void FecRingDevice::fecRingResetB ( ) 
  throw (FecExceptionHandler) {

  setFecRingCR0 ( FEC_CR0_RESETRINGB, CMD_OR ) ;
  setFecRingCR0 ( FEC_CR0_RESETRINGB, CMD_XOR ) ;

  // Empty all the FIFOs
  tscType16 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0) ) {
    try {
      // Disable the receive FIFO
      //setCR0Receive(false);
      // Empty all the FIFOs
      emptyFifoReceive () ;
      emptyFifoTransmit() ;
      emptyFifoReturn() ;
    }
    catch (FecExceptionHandler &e) { }
    try {
      // Enable the receive FIFO
      //setCR0Receive(true);
    }
    catch (FecExceptionHandler &e) { }
  }

  // Wait for the correct status
  fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0) ) usleep(1) ;

  // Delete all CCUs
  for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();p!=ccuMapAccess_.end();p++)
    delete p->second ; // Remove all the accesses for the CCU device

  ccuMapAccess_.clear() ;

  tscType16 fecCR0 = getFecRingCR0() ;  

  // Clock polarity
  invertClockPolarity_ = isInvertClockPolarity(fecCR0) ;
  
  setFecRingCR0 (FEC_CR0_ENABLEFEC) ; // Enable FEC
  setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ; // Clear errors

  // no more reconfiguration running
  reconfigurationRunning_ = false ;
}

/** Initialise the FEC after a hard reset
 * \exception FecExceptionHandler
 * \warning The FEC reset FEC/CCU (but not the modules done via PIA channels for the CMS Tracker)
 */
void FecRingDevice::initAfterHardReset ( ) 
  throw (FecExceptionHandler) {

  // Change the clock polarity
  if (invertClockPolarity_) setFecRingCR0(FEC_CR0_POLARITY,CMD_OR) ;
  else setFecRingCR0(FEC_CR0_POLARITY,CMD_XOR) ;

  // Empty all the FIFOs
  tscType16 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0) ) {
    try {
      // Empty all the FIFOs
      emptyFifoReceive () ;
      emptyFifoTransmit() ;
      emptyFifoReturn() ;
    }
    catch (FecExceptionHandler &e) { }
  }

  // Delete all CCUs
  for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();p!=ccuMapAccess_.end();p++)
    delete p->second ; // Remove all the accesses for the CCU device

  ccuMapAccess_.clear() ;

  setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ; // Clear errors

  // no more reconfiguration running
  reconfigurationRunning_ = false ;
}

/** Check the validity of the ring
 * <lu>
 * <li> Check the SR0 is nominal
 * <li> Check if the ring is closed
 * <li> Clear the error
 * <li> Check if the FIFO receive is empty 
 * </lu>
 */
void FecRingDevice::checkRing ( ) throw (FecExceptionHandler) {

  // Check the status register 0 of the FEC
  tscType32 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0)) {
    
    if (reconfigurationRunning_) {
      std::cerr << "The SR0 0x" << std::hex << fecSR0 << " is int reconfiguration" << std::dec << std::endl ;

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
					      "A reconfiguration of the ring is running, cannot check the ring",
					      ERRORCODE,
					      buildFecRingKey(getFecSlot(),getRingSlot()) ) ;
    }
    else {

      // filter the frames to try to recover it if the ring is closed
      if (fecSR0 & FEC_SR0_LINKINITIALIZED) {
	checkFifoReceive ( ) ;
	// Check again the SR0
	fecSR0 = getFecRingSR0() ;
      }

      // if the IRQ is on: The IRQ must be cleared at this point if the readFrame was not called
      // This method does clear the IRQ after this clear
      if ((fecSR0 & FEC_SR0_PENDINGIRQ) || (fecSR0 & FEC_SR0_DATATOFEC)) {
	setFecRingCR1 (FEC_CR1_CLEARIRQ);
	// Check again the SR0
	fecSR0 = getFecRingSR0() ;
      }

      if (! isFecSR0Correct(fecSR0)) {
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES
	std::cerr << "The SR0 0x" << std::hex << fecSR0 << " is not nominal, the frame is not sent" << std::dec << std::endl ;
	
	tscType16 fecSR1 = getFecRingSR1() ;
	tscType16 fecCR0 = getFecRingCR0() ;
	std::cerr << "The SR1 0x" << std::hex << fecSR1 << std::dec << std::endl ;
	std::cerr << "The CR0 0x" << std::hex << fecCR0 << std::dec << std::endl ;
#endif    

	RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
					   TSCFEC_SR0NOTNOMINAL_MSG,
					   FATALERRORCODE,
					   buildFecRingKey(getFecSlot(),getRingSlot()),
					   "FEC status register 0", fecSR0) ;
      }
    }
  }
}

/** Create a new token in the FEC after a timeout. If a ring is opened the FEC release make possible the send of the frame. So for example if you want to recover a badly configure ring, you must write the frame and after make a FEC release to forsee the send of the frame over the ring. When the frame is sent, you do not have any garentee that at the end of the release the frame is sent. You must wait on the FIFO transmit is not running. This operation is done in this method.
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 * \warning the FEC release is made two times due to the hardware (?)
 */
void FecRingDevice::fecRingRelease ( ) 
  throw (FecExceptionHandler) {

  // Generate the token
  setFecRingCR1 ( FEC_CR1_RELEASEFEC ) ;
  setFecRingCR1 ( FEC_CR1_RELEASEFEC ) ;

  // Wait for the end of the send of the frane (FIFO transmit running)
  tscType16 fecSR0 = getFecRingSR0() ; unsigned long watchdog = 0 ;
  while ( (fecSR0 & FEC_SR0_TRARUN) && (watchdog++ < loopInTimeWriteFrame_) ) {
    //#ifdef TIMETOWAIT
    struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 50000000 ;
    nanosleep (&req,NULL) ;
    //usleep (TIMETOWAIT) ;
    //#endif
    fecSR0 = getFecRingSR0() ;
  }
  if (fecSR0 & FEC_SR0_TRARUN) {
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    std::cerr << __func__ << ": FIFO transmit running bit is always ON (SR0 = " 
	      << std::hex << fecSR0 << ")" << std::dec << std::endl ;
#endif    

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON,
				       "FIFO transmit running bit is always on (CR0[3])",
				       CRITICALERRORCODE,
				       buildFecRingKey(getFecSlot(), getRingSlot()),
				       "FEC status register 0", fecSR0 ) ;
  }
}

/** empty the FIFO receive by reading all the values
 * \warning a timeout is implemented (4096 maximum)
 * \warning if the PLX interrupts are enable, the driver in normal case
 * will take care about all values. In some case of errors, the driver 
 * will not emptyied the FIFO due for example to a flood of interrupts 
 * and in some strange indermine cases.
 * \exception FecExceptionHandler
 * \thanks Wojciech BIALAS
 */
void FecRingDevice::emptyFifoReceive ( )
  throw (FecExceptionHandler) {

  tscType16 fecSR0 ; 

  unsigned int loopCnt = 0 ; 

  while (
         (!((fecSR0=getFecRingSR0()) & FEC_SR0_RECEMPTY) || (fecSR0 & FEC_SR0_RECRUN)) 
         && (loopCnt < receiveFifoDepth_) ) {

    // if (value==0xffff) fifo is empty : go out of loop  
    getFifoReceive() ;
     
    loopCnt++; 

  } // end of while loop 
  
  if (loopCnt >= receiveFifoDepth_) { 

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FIFONOTEMPTIED,
			       "FEC recv. fifo could not be emptied.",
			       CRITICALERRORCODE,
			       buildFecRingKey(getFecSlot(), getRingSlot()),
			       "FEC status register 0", fecSR0 ) ;
  } 
}

/** empty the FIFO receive by reading all the values
 * \warning a timeout is implemented (4096 maximum)
 * \warning if the PLX interrupts are enable, the driver in normal case
 * will take care about all values. In some case of errors, the driver 
 * will not emptyied the FIFO due for example to a flood of interrupts 
 * and in some strange indermine cases.
 * \exception FecExceptionHandler
 * \thanks Wojciech BIALAS
 */
void FecRingDevice::emptyFifoReturn ( )
  throw (FecExceptionHandler) {

  tscType16 fecSR0 ; 

  unsigned int loopCnt = 0 ; 

  while (
         !((fecSR0=getFecRingSR0()) & FEC_SR0_RETEMPTY)
         && (loopCnt < returnFifoDepth_) ) {

    // if (value==0xffff) fifo is empty : go out of loop  
    getFifoReturn() ;
     
    loopCnt++; 

  } // end of while loop 
  
  if (loopCnt >= returnFifoDepth_) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FIFONOTEMPTIED,
			       "FEC return fifo could not be emptied.",
			       CRITICALERRORCODE,
			       buildFecRingKey(getFecSlot(), getRingSlot()),
			       "FEC status register 0", fecSR0 ) ;
  } 
}

/** empty the FIFO receive by reading all the values
 * \warning a timeout is implemented (4096 maximum)
 * \warning if the PLX interrupts are enable, the driver in normal case
 * will take care about all values. In some case of errors, the driver 
 * will not emptyied the FIFO due for example to a flood of interrupts 
 * and in some strange indermine cases.
 * \exception FecExceptionHandler
 * \thanks Wojciech BIALAS
 */
void FecRingDevice::emptyFifoTransmit ( )
  throw (FecExceptionHandler) {

  tscType16 fecSR0 ; 

  unsigned int loopCnt = 0 ; 

  while (
         (!((fecSR0=getFecRingSR0()) & FEC_SR0_TRAEMPTY) || (fecSR0 & FEC_SR0_TRARUN)) 
         && (loopCnt < transmitFifoDepth_) ) {

    // if (value==0xffff) fifo is empty : go out of loop  
    getFifoTransmit() ;
     
    loopCnt++; 

  } // end of while loop 
  
  if (loopCnt >= transmitFifoDepth_) { 

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FIFONOTEMPTIED,
			       "FEC transmit fifo could not be emptied.",
			       CRITICALERRORCODE,
			       buildFecRingKey(getFecSlot(), getRingSlot()),
			       "FEC status register 0", fecSR0 ) ;
  } 
}

/** Check the status register 0 of the FEC, if the FIFO receive is not empty try to recover it.
 * \param display - 0 no display, 1 display, -1 depend of the lINK INIT, if link init then display if not then no display (default value)
 */
void FecRingDevice::checkFifoReceive ( int display ) {

  tscType16 fecSR0 = getFecRingSR0() ;
  unsigned int loopCnt = 0 ; 
  tscType32 c[DD_USER_MAX_MSG_LENGTH] ;

#ifdef DISPLAYBADFRAMES
  bool displayFlag = false ;

  if (display == 0) displayFlag = false ;
  else displayFlag = true ;

  if (display == -1) { // check the SR0
    // Clear errors
    setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ; // Clear errors  
    //fecSR0 = getFecRingSR0() ;
    //tscType16 fecSR1 = getFecRingSR1() ;
    //displayFlag = ( (fecSR0 & FEC_SR0_LINKINITIALIZED) && (!(fecSR1 & FEC_SR1_CLOCKERROR)) ) ;
    if (getFecRingSR0() & FEC_SR0_LINKINITIALIZED) {
      usleep(100) ;
      if (getFecRingSR0() & FEC_SR0_LINKINITIALIZED) {
	usleep(100) ;
	if (getFecRingSR0() & FEC_SR0_LINKINITIALIZED) displayFlag = true ; 
      }
    }
  }
  else if (display == 0) displayFlag = false ;
  else displayFlag = true ;
#else
  bool displayFlag = false ;
#endif

  if (! (fecSR0 & FEC_SR0_RECEMPTY) || (fecSR0 & FEC_SR0_RECRUN) ) {

    try {
      // Disable receiver 
      setCR0Receive(false);

      while ((!((fecSR0=getFecRingSR0()) & FEC_SR0_RECEMPTY) || (fecSR0 & FEC_SR0_RECRUN)) 
	     && (loopCnt < receiveFifoDepth_) ) {
	
	if (displayFlag) {
	  std::cerr << "FEC " << std::dec << (int)getFecSlot() << " ring " << (int)getRingSlot() << std::endl ;
	  std::cerr << "CR0 = 0x" << std::hex << getFecRingCR0() << std::endl ;
	  std::cerr << "SR0 = 0x" << std::hex << getFecRingSR0() << std::endl ;
	  std::cerr << "SR1 = 0x" << std::hex << getFecRingSR1() << std::endl ;
	}
	
	// read the word in the FIFO receive
	c[0] = getFifoReceive() ;

	// Check the REAL size of the received frame
	tscType16 realSize = (c[0] >> 8) & 0xFF ;
	tscType8 word2 = c[0] & 0xFF ;
	if (realSize & FEC_LENGTH_2BYTES) {
	  
	  realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
	  // +1 for the two byte words for length
	}

	// Check the size of the frame
	if ( ((realSize+3) < 6) || ((realSize+3) > DD_MAX_MSG_LENGTH_32) ) {
	  if (displayFlag) {
	    switch (c[0]) {
	    case 0x184: 
	      std::cerr << "Warning: found in the FIFO receive an Illegal sequence" << std::endl ;
	      break ;
	    case 0x182:
	      std::cerr << "Warning: found in the FIFO receive an Illegal data" << std::endl ;
	    default: {
	      std::cerr << "Warning: found in the FIFO receive an illegal word: " << std::hex << c[0] << " " ;
	      int dst = ((c[0] >> 24) & 0xFF) ;
	      int src = ((c[0] >> 16) & 0xFF) ;
	      int siwe = ((c[0] >> 8) & 0xFF) ;
	      std::cerr << "(dst = "  << std::hex << dst ;
	      std::cerr << ", src = " << std::hex << src ;
	      std::cerr << ", size = "  << std::dec << realSize << "/" << siwe << ")" << std::endl ;
	      break ;
	    }
	    }
	  }
	}
	else {

	  // Found a frame in the FIFO receive display it
	  if (displayFlag) std::cerr << "Warning: found a frame not requested: " ;
	  tscType32 realSize32 = (realSize+3+1)/4 ;
	  if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
	  
	  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ;
	  getFifoReceive(c+1,realSize32-1);
	  
	  // The status is just after the frame
	  tscType32 cpt = 0 ;
	  for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	    
	    if (displayFlag) std::cerr << " 0x" << std::hex << c[i] << ", " ;
	    
	    frame[cpt++] = (c[i] >> 24) & 0xFF ;
	    frame[cpt++] = (c[i] >> 16) & 0xFF ;
	    frame[cpt++] = (c[i] >>  8) & 0xFF ;
	    frame[cpt++] = (c[i])       & 0xFF ;
	  }

	  // Found a frame in the FIFO receive display it
	  if (displayFlag) {
	    std::cerr << std::endl << "Warning: found a frame not requested: " ;
	    for (int i = 0 ; i < ((realSize+3)+1) ; i ++ )
	      std::cerr << "0x" << std::hex << (int)frame[i] << " " ;
	    std::cerr << std::endl ;
	  }

	  loopCnt ++ ;
	}
      }
    }
    catch (FecExceptionHandler &e) {
      // re-enable receiver 
      setCR0Receive(true);
      throw e ;
    }

    try {
      // re-enable receiver 
      setCR0Receive(true);

      if (getFecRingSR0() & FEC_SR0_LINKINITIALIZED) {
	int timeout = 0 ;
	// wait until the status is correct
	while (!isFecSR0Correct(getFecRingSR0()) && (timeout < 5)) {
	  struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ; 
	  nanosleep (&req,NULL) ;
	  timeout ++ ;
	  //usleep (TIMETOWAIT) ;
	}
      }
    }
    catch (FecExceptionHandler &e) { }
  }
}

// -------------------------------------------------------------------------------------
//
//                         Configuration of a CCU
//
// -------------------------------------------------------------------------------------

/** After the detection of a CCU. The CCU is add to the hashtable of the CCU. The CRA is written
 * and the corresponding channel is set.
 * \param index - index of the device
 * \return boolean - is a ccu 25 ?
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning This method is only used in the basic check error in case of the ring
 * was not scanned at the beginning
 */
bool FecRingDevice::connectCCU ( keyType index ) throw (FecExceptionHandler) {

  // is a CCU 25 ?
  bool isCcu25 ;
  
  // Try to see if a CCU is connected 
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if ((ccu == ccuMapAccess_.end()) || (ccu->second == NULL)) {

// #ifdef SCANCCUATSTARTING

//     // All the CCU are scanned in the constructor
//    RAISEFECEXCEPTIONHANDLER_HARDPOSITIION ( TSCFEC_FECPARAMETERNOTMANAGED,
//			       "The CCU is not present on the ring",
//			       ERRORCODE,
//			       setFecSlotKey(getFecKey(index)) |
//			       setRingKey(getRingKey(index)) |
//			       setCcuKey(getCcuKey(index))) ;

// #else

    // Find the CCU type => old CCU or CCU 25
    // Try to get the value of the CRE to recognise an Old CCU and a CCU 25
    isCcu25 = getCcuType ( index ) ;

    // Not connected => connect it 
    ccuMapAccess_[getCcuKey(index)] = new CCUDescription(index,isCcu25) ;

    // but the ccu aren't initialised and haven't any broadcast address 
    if (isCcu25) {

      // Init the CRA of the CCU
      // setCcuCRA (index, INITCCU25CRA) ;
      
      // The initialisation of the channel register of the I2C channels / Pia channels
      // is done in the method that calls this method      
    }
    else {

      setCcuCRA(index, INITOLDCCUCRA) ;
      // CCU channel
      setCcuRegister ( index, CMD_CCUWRITECRA, 0x1, CMD_OR) ;

      // The initialisation of the channel register of the I2C channels / Pia channels
      // is done in the method that calls this method      
    }
    
    // Set the broadcast address, not that after a reset the value is 0x80
    // setCcuCRD(index, BROADCAST_ADDRESS) ;

//#endif

  }
  else { // CCU Found is a CCU 25 ?
    isCcu25 = ccu->second->isCcu25() ;
  }

  return (isCcu25) ;
}

// -------------------------------------------------------------------------------------
//
//                                For the ring reconfiguration/detection
//
// -------------------------------------------------------------------------------------

/** This method scan the ring connected to the fec (by broadcast operation)
 * and store the different value of the CCU in the map table.
 * The order of the CCUs are store in the ccuMapOrder_.
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>
 * </ul>
 * \warning This method is done normally when the FecRingDevice is created
 * \warning When you use this method all the CCU must be CCU 25
 */
void FecRingDevice::fecScanRingBroadcast ( )
  throw (FecExceptionHandler) {

#ifdef DEBUGGETREGISTERS
  bool readRegisters = readRegistersError_ ;
  readRegistersError_ = false ;
#endif

  // Delete the CCU previously stored in the ring
  for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();p!=ccuMapAccess_.end();p++)  
    delete p->second ; // Remove all the accesses for the CCU device
  ccuMapAccess_.clear() ;

  // Frame to be sent
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = {BROADCAST_ADDRESS,FRAMEFECNUMBER,0x03,0x00,0x01,CMD_CCUREADCRA} ;

  tscType8 temporary[RECEIVEFIFODEPTH_MAX] ;

  // Write the broadcast frame (the status of the direct ack is always bad)
  writeFrameDelayed ( frame, true ) ;

  // Wait for incoming data (with a timeout): on fast computer problem with the time on the last CCUs
  unsigned long watchdog = 0 ;
  tscType16 fecSR0 = getFecRingSR0() ;
  do {
    //#ifdef TIMETOWAIT
    struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 20000000 ;
    nanosleep (&req,NULL) ;
    //usleep (TIMETOWAIT) ;
    //#endif
    fecSR0 = getFecRingSR0() ;
  }
  while ( !(fecSR0 & FEC_SR0_DATATOFEC) && (fecSR0 & FEC_SR0_LINKINITIALIZED) && (watchdog++ < loopInTimeReadFrame_) ) ;

  //clean up interrupts and unset the DATATOFEC bit in SR0
  setFecRingCR1 (FEC_CR1_CLEARIRQ);

  // Just a warning
  if (watchdog >= TIMEOUT) std::cerr << "Warning: FecRingDevice::fecScanRingBroadcast " << std::dec << (int)getFecSlot() << "." << (int)getRingSlot() << ": timeout on the read anwser of the force acknowledge (SR0 = 0x" << std::hex << fecSR0 << ")" << std::endl ;

  // Number of CCU
  int nbccu = 0 ;

  // timeout
  watchdog = 0 ;

  // All the CCUs found
  tscType8 ccuAddresses[MAXCCU] ; // table where will be store the position of each ccu in the ring (index 0 <=> position 1) 

  // Is the FIFO is not empty
  fecSR0 = getFecRingSR0() ;

  // Start to see the CCUs
  while ( !(fecSR0 & FEC_SR0_RECEMPTY) ) { 

    try {

      // The size of the frame is 3
      // 0x0 CCU 0x3 0x40 TRANS VALUE
      
      // Read first word
      tscType32 value = getFifoReceive ( ) ;
      temporary[0] = (value & 0xFF000000) >> 24 ; // dest.
      temporary[1] = (value & 0x00FF0000) >> 16 ; // src
      temporary[2] = (value & 0x0000FF00) >> 8  ; // size (3)
      temporary[3] = (value & 0x000000FF)       ; // channel

      // Read next
      value = getFifoReceive ( ) ;   
      temporary[4] = (value & 0xFF000000) >> 24 ; // status
      temporary[5] = (value & 0x00FF0000) >> 16 ; // transaction number
      temporary[6] = (value & 0x0000FF00) >> 8  ; // data
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME
      std::cout << " FecRingDevice::fecScanRingBroadcast( { " ;
      for (int i = 0 ; i < (3+3) ; i ++ )
	std::cout << "0x" << std::hex << (int)temporary[i] << " " ;
      std::cout << "} )" << std::dec << std::endl ;
#endif  

      // Retreive the destination
      if (temporary[0] == FRAMEFECNUMBER) { // Direct ack of the broadcast frame sent

#ifdef DEBUGMSGERROR
	std::cout << "Found a CCU at address 0x" << std::hex << (int)temporary[1] << std::endl ;
#endif
        ccuAddresses[nbccu] = temporary[1] ;
        nbccu += 1 ;
      }
      else watchdog ++ ;

#ifdef TIMETOWAIT
    struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
    nanosleep (&req,NULL) ;
    //usleep (TIMETOWAIT) ;
#endif

      fecSR0 = getFecRingSR0() ;

    }
    catch (FecExceptionHandler &e) {
      // An error but continue
    }
  } 

  // timeout problem
  if (watchdog == receiveFifoDepth_) {
    
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
					     "Receive too much data in the FIFO receive",
					     ERRORCODE,
					     buildFecRingKey(getFecSlot(),getRingSlot()) ) ;
  }

 
  // Clear errors
  setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ; // Clear errors    
  

  // Check that we have no the same address for the CCUs
  for (int i = 0 ; i < nbccu-1 ; i ++) 
    for (int j = i + 1 ; j < nbccu ; j++) 
      if (ccuAddresses[i] == ccuAddresses[j]) {
#ifdef DEBUGMSGERROR
        std::cerr << "Several CCU have the same address " << std::hex << ccuAddresses[i] << std::dec << std::endl ;
#endif

	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_CCUADDRESSDUPLICATEDERROR,
				   "Several CCUs have the same address in the ring",
				   CRITICALERRORCODE,
				   buildFecRingKey(getFecSlot(),getRingSlot()) | setCcuKey(ccuAddresses[i])) ;
      }

 

  // delete the CCU that was bypassed
  for (int i = 0 ; i < nbccu ; i ++) {
    bool dle = true ;    
    for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();dle && (p!=ccuMapAccess_.end());p++) {
      if ((p->second != NULL) && (getCcuKey(p->second->getKey()) == ccuAddresses[i])) dle = false ;
    }

    if (dle && (ccuMapAccess_.find(ccuAddresses[i]) != ccuMapAccess_.end())) {
      //std::cout << "Delete ccu " << ccuAddresses[i] << " / " << getCcuKey(ccuMapAccess_[ccuAddresses[i]]->getKey()) << std::endl ;
      //ccuMapAccess_[ccuAddresses[i]] = NULL ;
      CCUDescription *ccu = ccuMapAccess_[ccuAddresses[i]] ;
      ccuMapAccess_.erase((keyType)ccuAddresses[i]) ;
      delete ccu ;
    }
  }

  // Add the CCU to the list
  for (int i = 0 ; i < nbccu ; i ++) {
    
    // Find the CCU type => old CCU or CCU 25
    // Try to get the value of the CRE to recognise an Old CCU and a CCU 25
    bool isCcu25 = getCcuType ( buildCompleteKey(getFecSlot(), getRingSlot(), ccuAddresses[i], 0, 0) ) ;
    
#ifdef DEBUGMSGERROR
    if (isCcu25)
      std::cout << "Found a CCU 25 at address 0x" << std::hex << (int)ccuAddresses[i] << std::endl ;
    else 
      std::cout << "Found an old CCU at address 0x" << std::hex << (int)ccuAddresses[i] << std::endl ;
#endif

    // Check if the CCU was found before
    if (ccuMapAccess_.find(ccuAddresses[i]) == ccuMapAccess_.end()) {
      
      // Must be a CCU 25 for the broadcast mode
      CCUDescription *ccu = new CCUDescription (buildCompleteKey(getFecSlot(),getRingSlot(),ccuAddresses[i],0,0),isCcu25) ;
      ccuMapAccess_[ccuAddresses[i]] = ccu ;
      ccuMapOrder_[i] = ccu ;
      
    }
  
  }
  

  nbCcuOnTheRing_ = nbccu ;

#ifdef DEBUGGETREGISTERS
  // re-enable if it was enable the read registers in case of error
  readRegistersError_ = readRegisters ;
#endif
}

/** This method scan the ring connected to the FEC 
 * and store the different value of the CCU in the map table.
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>
 * </ul>
 * \warning This method is done normally when the FecRingDevice is created
 * \warning When you use this method all the CCU must be old CCUs
 */
void FecRingDevice::fecScanRingNoBroadcast ( ) 
  throw (FecExceptionHandler) {

#ifdef DEBUGGETREGISTERS
  bool readRegisters = readRegistersError_ ;
  readRegistersError_ = false ;
#endif

  // Delete the CCU previously stored in the ring
  for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();p!=ccuMapAccess_.end();p++)  
    delete p->second ; // Remove all the accesses for the CCU device
  ccuMapAccess_.clear() ;

  // frame which initialise the CCU
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = {0x01, 0x00, 0x04, 0x00, 0x00, 0x00, INITOLDCCUCRA} ;

  // table where will be store the position of each ccu in the ring (index 0 <=> position 1) 
  tscType8 ccuAddresses[MAXCCU]; 

  int nbccu = 0 ;  
  
  // Try to init CCU with the address i
  for( tscType8 i = 0x1 ; i < 0x80 ; i++ ) {
      
    // Destination of the frame
    frame[0] = i ;
    
    try {

      // Send the frame
      writeFrame (frame);

      ccuAddresses[nbccu] = i;
      nbccu++;
    }
    catch (FecExceptionHandler &e) {
      // The error is not catch, perhaps no CCU at this address
#ifdef DEBUGMSGERROR_DISPLAYFRAME
      std::cerr << e.what() << std::endl ;
#endif
    }
  }

  // delete the CCU that was bypassed
  for (int i = 0 ; i < nbccu ; i ++) {
    bool dle = true ;    
    for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();dle && (p!=ccuMapAccess_.end());p++) {
      if ((p->second != NULL) && (getCcuKey(p->second->getKey()) == ccuAddresses[i])) dle = false ;
    }
    
    if (dle && (ccuMapAccess_.find(ccuAddresses[i]) != ccuMapAccess_.end())) {
      //std::cout << "Delete ccu " << ccuAddresses[i] << " / " << ccuMapAccess_[getCcuKey(ccuAddresses[i]]->getKey())] << std::endl ;
      CCUDescription *ccu = ccuMapAccess_[ccuAddresses[i]] ;
      ccuMapAccess_.erase((keyType)ccuAddresses[i]) ;
      delete ccu ;
    }
  }
  
  // Add the CCU to the list
  for (int i = 0 ; i < nbccu ; i ++) {
    
    // Find the CCU type => old CCU or CCU 25
    // Try to get the value of the CRE to recognise an Old CCU and a CCU 25
    bool isCcu25 = getCcuType ( buildCompleteKey(getFecSlot(), getRingSlot(), ccuAddresses[i], 0, 0) ) ;
    
#ifdef DEBUGMSGERROR
    if (isCcu25)
      std::cout << "Found a CCU 25 at address 0x" << std::hex << (int)ccuAddresses[i] << std::endl ;
    else 
      std::cout << "Found an old CCU at address 0x" << std::hex << (int)ccuAddresses[i] << std::endl ;
#endif

    // Check if the CCU was found before
    if (ccuMapAccess_.find(ccuAddresses[i]) == ccuMapAccess_.end()) {
      
      // Must be a CCU 25 for the broadcast mode
      CCUDescription *ccu = new CCUDescription (buildCompleteKey(getFecSlot(), getRingSlot(), ccuAddresses[i], 0, 0),isCcu25) ;
      ccuMapAccess_[ccuAddresses[i]] = ccu ;
      ccuMapOrder_[i] = ccu ;
    }
  }
  
  nbCcuOnTheRing_ = nbccu ;

#ifdef DEBUGMSGERROR
  if ( nbccu == 0 )
    std::cout << __func__ << ": no CCU found" << std::endl ;
  else {
    std::cout << "Scan Ring:CCU found at address(es): " ;
    for (int z = 0 ; z < nbccu ; z ++) 
      std::cout << "0x" << std::hex << (int)ccuAddresses[z] << std::endl ;
    std::cout << std::endl ;
  }
#endif

#ifdef DEBUGGETREGISTERS
  // re-enable if it was enable the read registers in case of error
  readRegistersError_ = readRegisters ;
#endif
}


/** This method reconfigure the ring due to a CCU fault.
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_SR0NOTNOMINAL
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 * \bug this method does not remove the fault CCU in the hash table.
 * \bug this method must be debug for the 1st and the last CCU
 * \warning this method need the order of the CCU
 */
tscType16 FecRingDevice::fecRingReconfigure ( ) 
  throw (FecExceptionHandler) {

  // Ring closed
  tscType16 fecSR0 = getFecRingSR0() ;
  if (fecSR0 & FEC_SR0_LINKINITIALIZED) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
			       "ring reconfiguration is useless, the ring is closed",
			       ERRORCODE,
			       buildFecRingKey(getFecSlot(), getRingSlot()),
			       "FEC status register 0", fecSR0) ;
  }

  // Find the type for the ring of CCUs (assume that the ring manage the same type of CCUs
  bool isCcu25 = ccuMapAccess_.begin()->second->isCcu25( ) ;

  if (! isCcu25) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "Invalid command for old CCUs, the ring cannot be reconfigured",
			       ERRORCODE,
			       buildFecRingKey(getFecSlot(),getRingSlot())) ;
  }
  
  if (nbCcuOnTheRing_ <= 1) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Not enough CCUs (0 or 1 CCU is available)",
			       ERRORCODE,
			       buildFecRingKey(getFecSlot(),getRingSlot()),
			       "Number of CCU", nbCcuOnTheRing_) ;
  }

  return (getFecRingSR0()) ;
}

/** This method reconfigure the ring due to a CCU fault.
 * \param TkRingDescription - ring description containing the redundancy
 * \return status of the ring after reconfiguration
 * \exception FecExceptionHandler
 * \warning this method apply bindly the redundancy given
 */
tscType16 FecRingDevice::fecRingReconfigure ( TkRingDescription &tkRing ) 
  throw (FecExceptionHandler) {

  unsigned long msStart;
  unsigned long msCurrent;
  bool waitmore;

  std::vector<CCUDescription *>* ccuVector;
  std::vector<CCUDescription *>::iterator ccuIt;

  tscType16 fecRedundancyCommand;
  tscType8  redundancyCommand;

  // ------------------------------------------------------------------
  // In case it is not possible to reconfigure, we throw an exception
  if (!tkRing.isReconfigurable()) {
    std::string detailedError = tkRing.getReconfigurationProblem();
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
					    "It is not possible to reconfigure a ring with the given pattern of enabled CCUs/DOHs: "
					    + detailedError,
					    ERRORCODE,
					    buildFecRingKey(getFecSlot(),getRingSlot()) ) ;
  }
  
  // Compute the redundancy to have A/B rings populated
  tkRing.computeRedundancy();

  // -----------------------------------------------------------
  // Reset the rings
  fecRingReset() ;
  fecRingResetB() ;
  fecRingResetFSM() ;

  // Clean the FIFOs
  try {
    tscType32 fecSR0 = getFecRingSR0() ;
    if (!isFifoReceiveEmpty(fecSR0)) emptyFifoReceive() ;
  }
  catch (FecExceptionHandler &e) {
    // ????????????????????????????????????????
    //#ifdef DEBUGMSGERROR
    std::cout << __PRETTY_FUNCTION__ << e.what() << std::endl ;
    //#endif
  }

  try {
    tscType32 fecSR0 = getFecRingSR0() ;
    if (!isFifoTransmitEmpty(fecSR0)) emptyFifoTransmit() ;
  }
  catch (FecExceptionHandler &e) {
    // ????????????????????????????????????????
    //#ifdef DEBUGMSGERROR
    std::cout << __PRETTY_FUNCTION__ << e.what() << std::endl ;
    //#endif
  }

  try {
    tscType32 fecSR0 = getFecRingSR0() ;
    if (!isFifoReturnEmpty(fecSR0)) emptyFifoReturn() ;
  }
  catch (FecExceptionHandler &e) {
    // ????????????????????????????????????????
    //#ifdef DEBUGMSGERROR
    std::cout << __PRETTY_FUNCTION__ << e.what() << std::endl ;
    //#endif
  }

  // ????????????????????????????????????????
  //#ifdef DEBUGMSGERROR
  std::cout << __PRETTY_FUNCTION__ << " SR0 = 0x" << std::hex << getFecRingSR0() << std::dec << std::endl ;
  //#endif

  // Wait 1 ms
  usleep(10000);

  // -----------------------------------------------------------
  // Apply the reconfiguration starting by and
  // ending with the FEC

  // Prepare to start the reconfiguration
  //setCR0Receive(false);

#ifdef DEBUGMSGERROR
  std::cout << "Found to reconfigure the ring like this: " << std::endl;
  tkRing.display(std::cout, false, true);
  std::cout << std::endl;
#endif

  // Start the reconfiguration from the FEC ring input
  // Which corresponds to the proper output configuration for the FEC
  fecRedundancyCommand = 0;
  redundancyCommand = 0;
  if (tkRing.getInputBUsed()) {
    fecRedundancyCommand |= FEC_CR0_SELSEROUT;
    std::cout << "Reconfiguring FEC ring " << ": command " << (int)fecRedundancyCommand << std::endl ;
    setFecRingCR0(fecRedundancyCommand);
  }

  // Change the CR of all the involved CCUs
  ccuVector = tkRing.getCcuVector();
  for (ccuIt = ccuVector->begin(); ccuIt != ccuVector->end(); ccuIt++) {
    if ((*ccuIt)->getEnabled()) {
      redundancyCommand = 0;
      if  ((*ccuIt)->getInputB() ) redundancyCommand = CCU_CRC_ALTIN;
      if  ((*ccuIt)->getOutputB() ) redundancyCommand |= CCU_CRC_SSP;

      if (redundancyCommand != 0) {
        setCcuCRC( (*ccuIt)->getKey(), redundancyCommand);

        // ????????????????????????????????????????
        //#ifdef DEBUGMSGERROR
        std::cout << "Reconfiguring CCU " << std::dec << (*ccuIt)->getOrder() << ", address " << getCcuKey((*ccuIt)->getKey()) 
		<< " (0x" << std::hex << (*ccuIt)->getKey() << std::dec << ") in the ring " << getRingKey((*ccuIt)->getKey()) 
		<< ": command " << (int)redundancyCommand << std::endl ;
      //#endif

        setFecRingCR1 ( FEC_CR1_RELEASEFEC ) ;
        setFecRingCR1 ( FEC_CR1_RELEASEFEC ) ;
      }
    }
  }

  // End the reconfiguration with the FEC ring output
  // Which corresponds to the proper input configuration for the FEC
  if (tkRing.getOutputBUsed()) { 
    fecRedundancyCommand |= FEC_CR0_SELSERIN;
    std::cout << "Reconfiguring FEC ring " << ": command " << (int)fecRedundancyCommand << std::endl ;
    setFecRingCR0(fecRedundancyCommand);
  }

  // Generate the token (again yes again)
  setFecRingCR1 ( FEC_CR1_RELEASEFEC ) ;
  setFecRingCR1 ( FEC_CR1_RELEASEFEC ) ;
  // Re-activate the ring
  //setCR0Receive(true);
  
  // Then wait for the ring to be closed again (100 ms at most...)                                                                                  
  msStart = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  waitmore=true;
  do {
    usleep(100);
    msCurrent = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    waitmore = ((msCurrent-msStart) < 100);
    if (((getFecRingSR0())&FEC_SR0_LINKINITIALIZED)==FEC_SR0_LINKINITIALIZED) {
      waitmore=false;
    }
  } while (waitmore);


  // -----------------------------------------------------------
  // Clean the buffers
  try {
    emptyFifoReceive();
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Cannot empty the FIFO receive: " << e.what() << std::endl ;
#endif
  }
  try {
    emptyFifoTransmit();
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Cannot empty the FIFO transmit: " << e.what() << std::endl ;
#endif
  }
  try {
    emptyFifoReturn();
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Cannot empty the FIFO return: " << e.what() << std::endl ;
#endif
  }

  // -----------------------------------------------------------
  // Make a scan ccu to clean up the ring
  // TODO: does the ScanRing cleans up the things just because it 
  // empties FifoReceive/FifoTransmit/FifoReturn?
  try {
    fecScanRingBroadcast () ;
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "First ring broadcast, got an error (can be correct): " << e.what() << std::endl ;
#endif
  }
  try {
    fecScanRingBroadcast () ;
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Second ring broadcast, should work if the configuration is correct: " << e.what() << std::endl ;
#endif
  }

  // -----------------------------------------------------------
  // Clean the buffers
  try {
    emptyFifoReceive();
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Cannot empty the FIFO receive: " << e.what() << std::endl ;
#endif
  }
  try {
    emptyFifoTransmit();
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Cannot empty the FIFO transmit: " << e.what() << std::endl ;
#endif
  }
  try {
    emptyFifoReturn();
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Cannot empty the FIFO return: " << e.what() << std::endl ;
#endif
  }

  // Then wait for the ring to be closed again (100 ms at most...)                                                                                  
  msStart = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  waitmore=true;
  do {
    usleep(100);
    msCurrent = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    waitmore = ((msCurrent-msStart) < 100);
    if (((getFecRingSR0())&FEC_SR0_LINKINITIALIZED)==FEC_SR0_LINKINITIALIZED) {
      waitmore=false;
    }
  } while (waitmore);

  // ????????????????????????????????????????????????????????????????
  //#ifdef DEBUGMSGERROR
  std::cout << __PRETTY_FUNCTION__ << " end, SR0 = 0x" << std::hex << getFecRingSR0() << std::dec << std::endl ;
  //#endif

  // -----------------------------------------------------------
  // Return the status
  return (getFecRingSR0()) ;
}


// -------------------------------------------------------------------------------------
//
//                                For the CCU registers 
//
// -------------------------------------------------------------------------------------

/** Method in order to retreive the type of the CCU
 * \param index - index of the CCU
 * \param CRE - value of the control register E
 * \return bool - true if it's a CCU25 and false if it's an old CCU
 * \exception FecExceptionHandler
 */   
bool FecRingDevice::getCcuType ( keyType index,
                                 tscType32 *CRE ) 
  throw (FecExceptionHandler) {

  // read the register and write the new one with the correct value 
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x03, 0x00, 0xFE, CMD_CCUREADCRE } ;

  bool isCcu25 = true ;
 
  // Write the FRAME
  writeFrame (frame) ;
    
  // A read operation is performed */
  readFrame(frame[4], frame) ;

#ifdef DEBUGMSGERROR_DISPLAYFRAME
  for (int i = 0 ; i < 8 ; i ++) {
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  }
  std::cout << std::endl ;
#endif

  // The read value is 24 bits seperated in frame[5] to frame[7]

  // Check the size of the frame
  // With the data read you find the status, eof, crc, addr seen, ...
  if (frame[2] != 5) {
    
    isCcu25 = false ;
    *CRE = frame[5] ;
  }
  else {
    // Get the value without the end of frame and status 
    *CRE = (frame[7] || (frame[6] << 8) || (frame[5] << 16)) ;
#ifdef DEBUGMSGERROR
    std::cout << "Is a CCU 25: CCU " << std::hex << getCcuKey(index) << ": CRE = 0x" << std::hex << *CRE << std::endl ;
#endif
  }

  return (isCcu25) ;  
}


/** Method in order to retreive the type of the CCU
 * \param index - index of the CCU
 * \return bool - true if it's a CCU25 and false if it's an old CCU
 * \exception FecExceptionHandler
 */   
 bool FecRingDevice::getCcuType ( keyType index ) 
  throw (FecExceptionHandler) {

  tscType32 CRE = 0 ;
  bool isCcu25 = getCcuType (index, &CRE) ;

  return (isCcu25) ;  
}

/** Method in order to set a CCU control register 
 * \param index - key of the corresponding CCU
 * \param ccuRegister - register to be accessed (command for the CCU)
 * \param value - value to be written
 * \exception FecExceptionHandler
 * \warning If the CCU 25 CRE must be changed, the method setCcuCRE must used
 */
void FecRingDevice::setCcuRegister ( keyType index, 
                                     tscType8 ccuRegister, 
                                     tscType8 value ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;
 
#ifdef T3TDLEVELCHECK
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if ((ccu == ccuMapAccess_.end()) || (ccu->second == NULL)) {

    // All the CCU are scanned in the constructor
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "The CCU is not present on the ring",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) );
  }

  switch (ccuRegister) {
  case CMD_CCUWRITECRA: break ;
  case CMD_CCUWRITECRB: break ;
  case CMD_CCUWRITECRC: break ;
  case CMD_CCUWRITECRD: break ;
  case CMD_CCUWRITECRE: // Only for old CCUs
    if (! ccu->second->isCcu25()) break ;
  default: 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid command or wrong CCU type for this command",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "CCU register", ccuRegister );
  }
#endif

  // Prepare the frame and write it to the ring
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x04, 0x00,
    0x00, ccuRegister, value } ;
      
  writeFrame (frame) ;
  
  if (reconfigurationRunning_ == false ) {
    
    // -------------------------------------------------------------------------------------------- 
    // Due to the small bug in the CCU 25, the control register must read and write for each access
    
    tscType8 readcommand = 0xFF ;
    switch (ccuRegister) {
      //case CMD_CCUWRITECRA: readcommand = CMD_CCUREADCRA ; break ;
      //case CMD_CCUWRITECRB: readcommand = CMD_CCUREADCRB ; break ;
      //case CMD_CCUWRITECRC: readcommand = CMD_CCUREADCRC ; break ;
      //case CMD_CCUWRITECRD: readcommand = CMD_CCUREADCRD ; break ;
      case CMD_CCUWRITECRE: readcommand = CMD_CCUREADCRE ; break ;
    }
    
    // Read back the value
    if (readcommand != 0xFF) {
#ifdef DEBUGMSGERROR
      tscType8 CR = 
#endif
	getCcuRegister (index, readcommand) ;
    
#ifdef DEBUGMSGERROR
      std::cout << __func__ << ": Read back value is 0x" << std::hex << (int)CR << "  for command 0x" << (int)readcommand << std::endl ;
#endif
    
//      if ( CR != value ) {
//      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
//				  "CCU register access error: values are different from the set to the get"
//				  ERRORCODE,
//				  setFecSlotKey(getFecKey(index)) |
//				  setRingKey(getRingKey(index)) |
//				  setCcuKey(getCcuKey(index)),
//				  "Command",  readcommand ) ;
//      }
    }
    
    // --------------------------------------------------------------------------------------------
  }
}

/** Method in order to retreive a control/status register from the CCU 
 * (Thanks to Jean Fay)
 * \param index - key of the corresponding CCU
 * \param ccuRegister - register to be accessed (command for the CCU)
 * \return value read
 * \exception FecExceptionHandler
 */   
tscType32 FecRingDevice::getCcuRegister ( keyType index, 
                                          tscType8 ccuRegister ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if ((ccu == ccuMapAccess_.end()) || (ccu->second == NULL)) {

    // All the CCU are scanned in the constructor
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "The CCU is not present on the ring",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) );
  }

  switch (ccuRegister) {
  case CMD_CCUREADCRA: break ;
  case CMD_CCUREADCRB: break ;
  case CMD_CCUREADCRC: break ;
  case CMD_CCUREADCRD: break ;
  case CMD_CCUREADCRE:  // Only for the CCU 25
    if (ccu->second->isCcu25()) break ; 
  case CMD_CCUREADSRA: break ;
  case CMD_CCUREADSRB: break ;
  case CMD_CCUREADSRC: break ;
  case CMD_CCUREADSRD: break ;
  case CMD_CCUREADSRE: break ;
  case CMD_CCUREADSRF: break ;
  case CMD_CCUREADSRG:   // Only for old CCU
    if (! ccu->second->isCcu25()) break ; 
  case CMD_CCUREADSRH:   // Only for old CCU
    if (! ccu->second->isCcu25()) break ; 
  default: 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid command or wrong CCU type for this command",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "CCU register", ccuRegister );
  }
#endif
  
  // read the register and write the new one with the correct value
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { getCcuKey(index), FRAMEFECNUMBER, 
                                               0x03, 0x00, 0xFE, ccuRegister } ;

  writeFrame (frame) ;
 
  tscType32 CR = 0 ;

  if (reconfigurationRunning_ == false ) {
    
    // A read operation is performed
    readFrame(frame[4], frame) ;
    
    // Specific call for CRE or SRE for CCU 25    
    if ( ccuRegister == CMD_CCUREADCRE ||
         ccuRegister == CMD_CCUREADSRE) {

      // The read value is 24 bits seperated in frame[5] to frame[7]

      // Check the size of the frame
      // With the data read you find the status, eof, crc, addr seen, ...
      if (frame[2] != 5) {

        RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
					   "Cannot read back a CCU register",
					   ERRORCODE,
					   setFecSlotKey(getFecKey(index)) | 
					   setRingKey(getRingKey(index)) | 
					   setCcuKey(getCcuKey(index)), 
					   "Frame size", frame[2]) ;
      }
      
      // Get the value
      CR = ( frame[7] | (frame[6] << 8) | (frame[5] << 16) ) ;
    }
    // Specific call for SRF for the CCU 25
    else if ( ccuRegister == CMD_CCUREADSRF ) {

      // The read value is 16 bits separated in frame[5] to frame[6]
      
      // Check the size of the frame 
      // With the data read you find the status, eof, crc, addr seen, ... 
      if (frame[2] != 4) {
        RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
					   "Cannot read back CCU register (bad size in frame, should be 4)",
					   ERRORCODE,
					   setFecSlotKey(getFecKey(index)) | 
					   setRingKey(getRingKey(index)) | 
					   setCcuKey(getCcuKey(index)),  
					   "Frame size", frame[2] ) ;
      }

      // Get the value without the end of frame and status 
      CR = ( frame[6] | (frame[5] << 8) ) ;
    }
    else {
      // Check the size of the frame 
      // With the data read you find the status, eof, crc, addr seen, ...
      if (frame[2] != 3) {

        RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
					   "Cannot read back CCU register (bad size in frame, should be 3)",
					   ERRORCODE,
					   setFecSlotKey(getFecKey(index)) | 
					   setRingKey(getRingKey(index)) | 
					   setCcuKey(getCcuKey(index)),  
					   "Frame size", frame[2] ) ;
      }
        
      // Get the value without the end of frame and status
      CR = frame[5] ;
    }
  }

  return (CR) ;  
}
  
/** Method in order to set a control register from the CCU by read modify write operation
 * \param index - key of the corresponding CCU
 * \param ccuRegister - register to be accessed (command for the CCU)
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * \warning If the CCU 25 CRE must be changed, the method setCcuCRE must used
 */
void FecRingDevice::setCcuRegister ( keyType index, 
                                     tscType8 ccuRegister, 
                                     tscType8 value,
                                     logicalOperationType op ) throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  switch (ccuRegister) {
  case CMD_CCUWRITECRA:
    if (ccu->second->isCcu25()) {

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
				 "Setting the CCU CRA by read-modify-write cannot be used",
				 setFecSlotKey(getFecKey(index)) | 
				 setRingKey(getRingKey(index)) | 
				 setCcuKey(getCcuKey(index)),
				 "Register command", ccuRegister) ;
    }
    break ;
  case CMD_CCUWRITECRB: break ;
  case CMD_CCUWRITECRC: break ;
  case CMD_CCUWRITECRD: break ;
  case CMD_CCUWRITECRE: // Only for old CCUs
    if (! ccu->second->isCcu25()) break ;
  default: 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid command or wrong CCU type for this command",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "CCU register", ccuRegister );
  }
#endif

  // Check the command
  tscType8 readcommand = 0 ;

  switch (ccuRegister) {
  case CMD_CCUWRITECRA: readcommand = CMD_CCUREADCRA ; break ;
  case CMD_CCUWRITECRB: readcommand = CMD_CCUREADCRB ; break ;
  case CMD_CCUWRITECRC: readcommand = CMD_CCUREADCRC ; break ;
  case CMD_CCUWRITECRD: readcommand = CMD_CCUREADCRD ; break ;
  case CMD_CCUWRITECRE: readcommand = CMD_CCUREADCRE ; break ;
  }

  // read the register and write the new one with the correct value
  tscType8 CR = getCcuRegister (index, readcommand) ;

  // Write the value with the operation done 
  switch (op) {
  case CMD_OR:  value = CR | value    ; break ;
  case CMD_XOR: value = CR & (~value) ; break ;
  case CMD_AND: value = CR & value    ; break ;
  case CMD_EQUAL: break ;
  default: 
    RAISEFECEXCEPTIONHANDLER ( TSCFEC_INVALIDOPERATION,
			       "Invalid RMW command",
			       ERRORCODE ) ;
  }

  if (CR != value) setCcuRegister (index, ccuRegister, value) ;
}

/** Method in order to set CCU control register A
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 */
void FecRingDevice::setCcuCRA ( keyType index, tscType8 value ) 
  throw (FecExceptionHandler) {
  
  setCcuRegister( index, CMD_CCUWRITECRA, value ) ;
}

//setting CRA with ReadModifyWrite is useless as the CRA's bits are ever toggled to 0x0
//void FecRingDevice::setCcuCRA ( keyType index, tscType8 value, logicalOperationType op ) {
//  setCcuRegister( index, CMD_CCUWRITECRA, value, op ) ;
//}

/** Method in order to set CCU control register B
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 */
void FecRingDevice::setCcuCRB ( keyType index, tscType8 value ) 
  throw (FecExceptionHandler) {

  setCcuRegister( index, CMD_CCUWRITECRB, value ) ;
}

/** Method in order to set CCU control register B by read modify write operation
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 */
void FecRingDevice::setCcuCRB ( keyType index, tscType8 value, logicalOperationType op )
  throw (FecExceptionHandler) {

  setCcuRegister( index, CMD_CCUWRITECRB, value, op ) ;
}

/** Method in order to set CCU control register C
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
void FecRingDevice::setCcuCRC ( keyType index, tscType8 value ) 
  throw (FecExceptionHandler) {
  
  reconfigurationRunning_ = true ;

#ifdef T3TDLEVELCHECK
  if (value > 0x3) 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid value to be set on a CCU register C",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "value to be set", value );
#endif

  // Send directly the frame over the ring
  //setCcuRegister( index, CMD_CCUWRITECRC, value ) ;
  // Prepare the frame and write it to the ring
  tscType8 sizet = 4 ;
  tscType8 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
					      sizet, 0x0,
					      0, CMD_CCUWRITECRC, value } ;

  try {
    writeFrame (frame) ;
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Error in setCcuCRC: redundancy control:" << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }


//   // Try to see if a CCU is connected 
//   ccuMapAccessedType::iterator it = ccuMapAccess_.find(getCcuKey(index)) ;
//   if ((it != ccuMapAccess_.end()) && (it->second != NULL)) {
//     CCUDescription *ccu = it->second ;
//     if ( value & 0x1 )
//       ccu->setInput ( 1 ) ;
//     else
//       ccu->setInput ( 0 ) ;
//     if ( value & 0x2 )
//       ccu->setOutput ( 1 ) ;
//     else
//       ccu->setOutput ( 0 ) ; 
//   }
    
  reconfigurationRunning_ = false ;
}

/** Method in order to set CCU control register C by read modify write operation
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 */
void FecRingDevice::setCcuCRC ( keyType index, tscType8 value, logicalOperationType op ) throw (FecExceptionHandler) {
  
  reconfigurationRunning_ = true ;

  // Check the index
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  if (value >= 0x3) 
   RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			      "Invalid value to be set on a CCU register C",
			      ERRORCODE,
			      setFecSlotKey(getFecKey(index)) |
			      setRingKey(getRingKey(index)) |
			      setCcuKey(getCcuKey(index)),
			      "value to be set", value );
#endif

  // Try to see if a CCU is connected 
  // Note that the basic check is done if all flags (compiler) is set so
  // the test must redone there
  ccuMapAccessedType::iterator it = ccuMapAccess_.find(getCcuKey(index)) ;
  if ((it == ccuMapAccess_.end()) || (it->second == NULL)) {

    // All the CCU are scanned in the constructor
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "The CCU is not present on the ring",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) );
  }


  // Set the CCU register
  setCcuRegister ( index, CMD_CCUWRITECRC, value, op ) ;  

  // Retreive the value in order to store in the virtual CCU class
  value = getCcuRegister ( index, CMD_CCUREADCRC ) ;

//  // CCU found and write to the CCU register ok
//  CCUDescription *ccu = it->second ;
//   if ( value & 0x1 )
//     ccu->setInput ( 1 ) ;
//   else
//     ccu->setInput ( 0 ) ;
//   if ( value & 0x2 )
//     ccu->setOutput ( 1 ) ;
//   else
//     ccu->setOutput ( 0 ) ;

  reconfigurationRunning_ = false ;
}

/** Method in order to set CCU control register D
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \warning The broadcast register is set to 0x80 after a reset
 */
void FecRingDevice::setCcuCRD ( keyType index, tscType8 value ) 
  throw (FecExceptionHandler) {

  setCcuRegister( index, CMD_CCUWRITECRD, value ) ;
}

/** Method in order to set CCU control register D by read modify write operation
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \warning The broadcast register is set to 0x80 after a reset
 */
void FecRingDevice::setCcuCRD ( keyType index, tscType8 value, logicalOperationType op ) 
  throw (FecExceptionHandler) {

  setCcuRegister( index, CMD_CCUWRITECRD, value, op ) ;
}


/** Method in order to set CCU control register E (only for CCU 25)
 * The word is 24 bits long:
 * <ul>
 * <li>bit 0-15: enable I2C channels
 * <li>bit 16-19: enable PIA channels
 * <li>bit 20: enable memory controller
 * <li>bit 21: enable trigger controller
 * <li>bit 22: enable JTAG controller
 * <li>bit 23: not used
 * </ul>
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecRingDevice::setCcuCRE ( keyType index, tscType32 value ) 
  // tscType8 value0,  // 16 - 23
  // tscType8 value1,  // 8 - 15
  // tscType8 value2)  // 0 - 7   
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if ((ccu->second == NULL) || (! ccu->second->isCcu25())) { 
    // All the CCU are scanned in the constructor
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       (ccu->second == NULL) ? "The CCU is not present on the ring" : "Invalid operation on old CCU (set CCU CRE)"
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) );
  }
#endif

  // Split the value into three words of 8 bits 
  tscType8 value0 = (value & 0xff) ;
  tscType8 value1 = (value & 0xff00) >> 8 ;
  tscType8 value2 = (value & 0xff0000) >> 16 ;

  // Prepare the frame and write it to the ring
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x06, 0x00,
    0X00, CMD_CCUWRITECRE, 
    value2, value1, value0 } ;
      
  writeFrame (frame) ;
   
  // Due to the small bug in the CCU 25, the control register must read and write for each access 
  // tscType24 CR = 
  getCcuCRE (index) ;
  
//    if ( CR != value ) {
//      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
//				  "CCU register access error: values are different from the set to the get"
//				  ERRORCODE,
//				  setFecSlotKey(getFecKey(index)) |
//				  setRingKey(getRingKey(index)) |
//				  setCcuKey(getCcuKey(index)),
//				  "Command",  readcommand ) ;
//    }
}

/** Method in order to set CCU control register E (only for CCU 25).
 * The word is 24 bits long:
 * <ul>
 * <li>bit 0-15: enable I2C channels
 * <li>bit 16-19: enable PIA channels
 * <li>bit 20: enable memory controller
 * <li>bit 21: enable trigger controller
 * <li>bit 22: enable JTAG controller
 * <li>bit 23: not used
 * </ul>
 * \param index - key of the corresponding CCU
 * \param valueI2C - 0 or 1 for each I2C channel
 * \param valuePIA - 0 or 1 for each PIA channel
 * \param valueMemory - 0 or 1 for the memory controller
 * \param valueTrigger - 0 or 1 for the trigger controller
 * \param valueJTAG - 0 or 1 for the JTAG controller
 */
void FecRingDevice::setCcuCRE ( keyType index, 
                            tscType8 valueI2C, 
                            tscType8 valuePIA,
                            tscType8 valueMemory,
                            tscType8 valueTrigger,
                            tscType8 valueJTAG)
  throw (FecExceptionHandler) {

  setCcuCRE ( index, 
              (valueI2C & 0xFF) | 
              ((valuePIA & 0x7) << 16)|
              ((valueMemory & 0x1) << 20)| 
              ((valueTrigger & 0x1) << 21) |
              ((valueJTAG & 0x1) << 22) ) ;
}

/** Method in order to set a control register from the CCU by read modify write operation
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
void FecRingDevice::setCcuCRE ( keyType index, 
                                tscType24 value,
                                logicalOperationType op ) throw (FecExceptionHandler) {

  // read the register and write the new one with the correct value 
  tscType24 CR = getCcuCRE ( index ) ;

  // Write the value with the operation done 
  switch (op) {
  case CMD_OR:  value = CR | value    ; break ;
  case CMD_XOR: value = CR & (~value) ; break ;
  case CMD_AND: value = CR & value    ; break ;
  case CMD_EQUAL: break ;
  default: 
    RAISEFECEXCEPTIONHANDLER ( TSCFEC_INVALIDOPERATION,
			       "Invalid RMW command",
			       ERRORCODE ) ;
  }

  if (CR != value) setCcuCRE ( index, value ) ;
}

/** Method in order to retreive control register A from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */   
tscType8 FecRingDevice::getCcuCRA ( keyType index ) 
  throw (FecExceptionHandler) {
 
  return ( getCcuRegister( index, CMD_CCUREADCRA )) ;
}

/** Method in order to retreive status register B from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType8 FecRingDevice::getCcuCRB ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getCcuRegister( index, CMD_CCUREADCRB )) ;
}

/** Method in order to retreive control register C from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType8 FecRingDevice::getCcuCRC ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getCcuRegister( index, CMD_CCUREADCRC )) ;
}

/** Method in order to retreive control register D from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType8 FecRingDevice::getCcuCRD ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getCcuRegister( index, CMD_CCUREADCRD )) ;
}

/** Method in order to retreive control register E from the CCU (only for CCU 25)
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
tscType24 FecRingDevice::getCcuCRE ( keyType index ) 
  throw (FecExceptionHandler) {

  tscType32 CR = getCcuRegister( index, CMD_CCUREADCRE ) ;

  return ( CR ) ;
}

/** Method in order to retreive status register A from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType8 FecRingDevice::getCcuSRA ( keyType index ) 
  throw ( FecExceptionHandler ) {

  return ( getCcuRegister( index, CMD_CCUREADSRA )) ;
}

/** Method in order to retreive status register B from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType8 FecRingDevice::getCcuSRB ( keyType index ) 
  throw ( FecExceptionHandler ) {

  return ( getCcuRegister( index, CMD_CCUREADSRB )) ;
}

/** Method in order to retreive status register C from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType8 FecRingDevice::getCcuSRC ( keyType index ) 
  throw ( FecExceptionHandler ) {

  return ( getCcuRegister( index, CMD_CCUREADSRC )) ;
}

/** Method in order to retreive status register D from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType8 FecRingDevice::getCcuSRD ( keyType index ) 
  throw ( FecExceptionHandler ) {

  return ( getCcuRegister( index, CMD_CCUREADSRD )) ;
}

/** Method in order to retreive status register E from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType24 FecRingDevice::getCcuSRE ( keyType index ) 
  throw ( FecExceptionHandler ) {

  return ( getCcuRegister( index, CMD_CCUREADSRE )) ;
}

/** Method in order to retreive status register F from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecRingDevice::getCcuSRF ( keyType index ) 
  throw ( FecExceptionHandler ) {

  return ( getCcuRegister( index, CMD_CCUREADSRF )) ;
}

/** Method in order to retreive status register G from the CCU (only for the old CCU)
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
tscType8 FecRingDevice::getCcuSRG ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getCcuRegister( index, CMD_CCUREADSRG )) ;
}

/** Method in order to retreive status register H from the CCU (only for the old CCU)
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
tscType8 FecRingDevice::getCcuSRH ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getCcuRegister( index, CMD_CCUREADSRH )) ;
}

/** Retreive all the CCU scanned or connected at the time when the function is called 
 * (for all rings)
 * \param noBroadcast - if true all the CCU from 0x1 to 0x79 is checked (fecScanRingNoBroadcast)
 * else the CCU broadcast mode is used
 * \return a list of keyType
 */
std::list<keyType> *FecRingDevice::getCcuList ( bool noBroadcast, bool scan) 
  throw ( FecExceptionHandler ) {

  //#ifndef SCANCCUATSTARTING
  // If the scan order was not done in the FecRingDevice constructor => do it
  if (scan) {
    if (noBroadcast)       
      fecScanRingNoBroadcast ( ) ;
    else {
      fecScanRingBroadcast ( ) ;
    }
  }
  //#endif

  std::list<keyType> *ccuList = NULL ;
  
  // For each CCU
  for (unsigned int i = 0 ; i < nbCcuOnTheRing_ ; i ++) {
    CCUDescription *ccu = ccuMapOrder_[i] ;

    if (ccu != NULL) {

      if (ccuList == NULL) ccuList = new std::list<keyType> ;
      ccuList->push_back (ccu->getKey()) ; //buildCompleteKey (getFecSlot(), getRingSlot(), getCcuKey(ccu->getAddress()), 0, 0)) ;
    }
  }

  return (ccuList) ;
}

/** Enable the channel corresponding to the key
 * \param index - key of the channel (FEC/RING/CCU/CHANNEL)
 * \param enable - boolean for enable or disable the channel
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 * \warning the initialisation of the channel (CRA) for the CCU25 is not performed in this method
 * \warning the initialisation of the channel (CRA) for the old CCU is performed in this method due
 * to the fact that the high bit in CRA is the enable channel
 * \warning For the CCU 25, all the channels can enable, for the old CCU only the i2c channel and pia channel can be enable (pia channel are not yet developpeed for the PIA)
 */
void FecRingDevice::setChannelEnable ( keyType index, bool enable ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Check if the channel is enable
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
 
  tscType8 channel = getChannelKey(index) ;
  if (ccu->second->getChannelEnable (getChannelKey(index)) != enable) {

    if (ccu->second->isCcu25()) { // CCU 25

      // For the CCU 25,
      // - Only the node controller of the CCU is set for enabling a channel
      // Node controller 
      if (enable) { 

        // Enable the channel
        // I2C     - setCcuCRE (index, (0x1 << (channel - 0x10))) ;
        // PIA     - setCcuCRE (index, (0x1 << (16 + channel - 0x30))) ;
        // MEMORY  - setCcuCRE (index, (0x1 << 20)) ; ou (20 + channel - 0x40))) ;
        // TRIGGER - setCcuCRE (index, (0x1 << 21)) ; ou (21 + channel - 0x50))) ;
        // JTAG    - setCcuCRE (index, (0x1 << 22)) ; ou (22 + channel - 0x60))) ;

        if (isi2cChannelCcu25 (index)) {

          // Enable the I2C channel bit 0 - 15
          setCcuCRE (index, (0x1 << (channel - 0x10)), CMD_OR) ;

          // Must be done in the upper layer (enable - initialise the CRA)
          // Initialise the I2C channel CRA
          // The problem comes from that if you do 2 setInitI2cChannelCRA
          // the CCU configuration disapear.
          // setInitI2cChannelCRA (index) ;
          
        } else if (isPiaChannelCcu25 (index)) {

          // Enable PIA channel bit 16-19
          setCcuCRE (index, (0x1 << (16 + channel - 0x30)), CMD_OR) ;

        } else if (isMemoryChannelCcu25 (index)) {

          // Enable memory channel bit 20
          // setCcuCRE (index, (0x1 << 20), CMD_OR) ;
          setCcuCRE (index, 0x100000, CMD_OR) ;
          
        } else if (isTriggerChannelCcu25 (index)) {

          // Enable the trigger channel bit 21
          // setCcuCRE (index, (0x1 << 21), CMD_OR) ;
          setCcuCRE (index, 0x200000, CMD_OR) ;

        } else if (isJtagChannelCcu25 (index)) {

          // Enable the JTAG channel bit 22
          // setCcuCRE (index, (0x1 << 22), CMD_OR) ;
          setCcuCRE (index, 0x400000, CMD_OR) ;

        }

      }
      else { // Disable the channel

        if (isi2cChannelCcu25 (index)) {

          // Disable the I2C channel
          setCcuCRE (index, (0x1 << (channel - 0x10)), CMD_XOR) ;

        } else if (isPiaChannelCcu25 (index)) {

          // Disable the PIA channel
          setCcuCRE (index, (0x1 << (16 + channel - 0x30)), CMD_XOR) ;
          
        } else if (isMemoryChannelCcu25 (index)) {
          
          // Enable memory channel bit 20
          // setCcuCRE (index, (0x1 << 20), CMD_XOR) ;
          setCcuCRE (index, 0x100000, CMD_XOR) ;

        } else if (isTriggerChannelCcu25 (index)) {

          // Enable the trigger channel bit 21
          // setCcuCRE (index, (0x1 << 21), CMD_XOR) ;
          setCcuCRE (index, 0x200000, CMD_XOR) ;

        } else if (isJtagChannelCcu25 (index)) {
          
          // Enable the JTAG channel bit 22
          // setCcuCRE (index, (0x1 << 22), CMD_XOR) ;
          setCcuCRE (index, 0x400000, CMD_XOR) ;

        } else {

	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
				     "Wrong CCU channel for CCU 25",
				     ERRORCODE,
				     setFecSlotKey(getFecKey(index)) |
				     setRingKey(getRingKey(index)) |
				     setCcuKey(getCcuKey(index)) |
				     setChannelKey(getChannelKey(index)) ) ;
        }
      }
    }
    else {

      // For the OLD CCU, 
      // - the node controller of the CCU is enable for this channel
      // - enable the channel i2c in the channel

      // Node controller
      
      if ( isi2cChannelOldCcu(index) ) { // I2C channel

        if (enable) {
          
          // CCU channel
          setCcuRegister ( index, CMD_CCUWRITECRA, 0x1, CMD_OR) ;
          
          // Channel Control register A => the high bit is the enable bit
          setInitI2cChannelCRA (index) ;

        }
        else {
          
          // The CCU CRA cannot be set to 0 because you disable all the possible
          // communication with i2c channels
          // setCcuRegister ( index, CMD_CCUWRITECRA, 0x1, CMD_XOR) ;
        }
        
      } else if ( isPiaChannelOldCcu(index) ) { // PIA channel

	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
				     "PIA channels cannot be used on old CCUs",
				     ERRORCODE,
				     setFecSlotKey(getFecKey(index)) |
				     setRingKey(getRingKey(index)) |
				     setCcuKey(getCcuKey(index)) |
				     setChannelKey(getChannelKey(index)) ) ;

        // Not usefull but keep compatibility in case of ...
        // Channel Control register A 
        if (enable) {

          // CCU channel 
          setCcuRegister ( index, CMD_CCUWRITECRA, 0x4, CMD_OR) ;
          
          // Initialise the PIA channel
          setInitPiaChannel (index) ;
        }
        else {

          setCcuRegister ( index, CMD_CCUWRITECRA, 0x4, CMD_XOR) ;
          setPiaChannelGCR (index, 0x80, CMD_XOR) ;
          ccu->second->setPiaChannelGCR (channel, 0x80, CMD_XOR) ;
        }
      } else if ( isMemoryChannelOldCcu(index) ) { // Memory channel

	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
				   "Memory channels cannot be used on old CCUs",
				   ERRORCODE,
				   setFecSlotKey(getFecKey(index)) |
				   setRingKey(getRingKey(index)) |
				   setCcuKey(getCcuKey(index)) |
				   setChannelKey(getChannelKey(index)) ) ;

      } else if ( isTriggerChannelOldCcu(index) ) { // Trigger channel

	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
				   "Trigger channel cannot be used on old CCUs",
				   ERRORCODE,
				   setFecSlotKey(getFecKey(index)) |
				   setRingKey(getRingKey(index)) |
				   setCcuKey(getCcuKey(index)) |
				   setChannelKey(getChannelKey(index)) ) ;

      } else if ( isJTAGChannelOldCcu(index) ) { // JTAG channel

	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
				   "JTAG channel cannot be used on old CCUs",
				   ERRORCODE,
				   setFecSlotKey(getFecKey(index)) |
				   setRingKey(getRingKey(index)) |
				   setCcuKey(getCcuKey(index)) |
				   setChannelKey(getChannelKey(index)) ) ;

      } else {

	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
				   "Wrong CCU channel on old CCUs",
				   ERRORCODE,
				   setFecSlotKey(getFecKey(index)) |
				   setRingKey(getRingKey(index)) |
				   setCcuKey(getCcuKey(index)) |
				   setChannelKey(getChannelKey(index)) ) ;
      }
    }
  }

  // Set the channel as enable in the CCU class
  ccu->second->setChannelEnable (channel, enable) ;
}

/** 
 * \param index - index of the CCU (FEC/RING/CCU). 
 * \param enable1 - enable CCU alarm 1
 * \param enable2 - enable CCU alarm 2
 * \param enable3 - enable CCU alarm 3
 * \param enable4 - enable CCU alarm 4
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning Alarms works with CCU 25 and not old CCU
 * \warning The source must be CLEARED before reenable it.
 */
void FecRingDevice::setCcuAlarmsEnable ( keyType index, 
                                         bool enable1, bool enable2, 
                                         bool enable3, bool enable4 ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

    tscType8 value = 0 ;

    if (enable1) value |= 1 ;
    if (enable2) value |= 2 ;
    if (enable3) value |= 4 ;
    if (enable4) value |= 8 ;

    setCcuCRB ( index, value, CMD_OR ) ;
    ccu->second->setCcuAlarmsEnable (enable1, enable2, enable3, enable4) ;
  }
}

/** 
 * \param index - index of the CCU (FEC/RING/CCU)
 * \param frame - frame received by the warning in 8 bits length
 * \exception FecExceptionHandler
 * \warning Alarms works with CCU 25 and not old CCU
 * \warning This method is a blocked command no interruption can be done
 * \warning The size of the frame must be DD_USER_MAX_MSG_LENGTH*4
 * \warning FIFO return are 8 bits word lenght
 * \warning When a warning is emitted then the transaction number must be zero
 */
void FecRingDevice::waitForAnyCcuAlarms ( keyType index, 
                                          tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

#ifdef T3TDLEVELCHECK

    // Check if the CRB is enable for CCU alarms
    if (! ccu->second->isCcuAlarmEnable (1) && 
        ! ccu->second->isCcuAlarmEnable (2) && 
        ! ccu->second->isCcuAlarmEnable (3) && 
        ! ccu->second->isCcuAlarmEnable (4) ) {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
				 "Cannot wait for alarms, they are not enabled",
				 FECERRORCODE,
				 setFecSlotKey(getFecKey(index)) | 
				 setRingKey(getRingKey(index)) | 
				 setCcuKey(getCcuKey(index)) ) ;
    }
        
#endif

    RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR,
			       "Method not yet implemented, please contact us for the implementation !",
			       ERRORCODE ) ;

    // Build the frame in 32 bits
    tscType32 c[DD_USER_MAX_MSG_LENGTH], realSize32 = 0, realSize = 0 ;

    // Wait on any message coming in FIFO return
    tscType16 fecSR0 = getFecRingSR0() ;
    while (fecSR0 & FEC_SR0_RETEMPTY) {
#ifdef TIMETOWAIT
      struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
      nanosleep (&req,NULL) ;
      //usleep (TIMETOWAIT) ;
#endif
      fecSR0 = getFecRingSR0() ;
    }
    
    // read frame in fifo return
    fecSR0 = getFecRingSR0() ;
    if (fecSR0 & FEC_SR0_RETEMPTY) {
      
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
      std::cerr << __func__ << ": cannot read the frame in FIFO return" << std::endl ;
#endif    

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_READ_FRAME_CORRUPTED,
					 "Unable to read a frame in FIFO return, FIFO return empty",
					 ERRORCODE,
					 buildFecRingKey(getFecSlot(),getRingSlot()),
					 "FEC status register 0", fecSR0) ;
    }
    
    // Read the answer
    // Retreive the size
    c[0] = getFifoReturn() ;
  
    // Check the size of the frame
    realSize = (c[0] >> 8) & 0xFF ;
    tscType8 word2 = c[0]  & 0xFF ;
    if (realSize & FEC_LENGTH_2BYTES) {
      
      realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
      // +1 for the two byte words for length
    }

    // +3 => Dst, Src, Length  ; +1 => status after the frame
    realSize32 = (realSize+3+1)/4 ;
    if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
    
#ifdef S2NDLEVELCHECK
    // Too many words ?
    if (realSize32 > DD_MAX_MSG_LENGTH_32)

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_TOO_LONG_FRAME_LENGTH,
				 "Size in frame is bigger than the FIFO return size",
				 CRITICALERRORCODE,
				 buildFecRingKey(getFecSlot(), getRingSlot()),
				 "Size of the frame received", realSize32 ) ;
#endif

    for (tscType32 i = 1 ; i < realSize32 ; i ++) c[i] = getFifoReturn() ;

    //clean up interrupts
    setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS);
  
    int cpt = 0 ;
    for (tscType32 i = 0 ; i < realSize32 ; i ++) {
      
#ifdef DEBUGMSGERROR
      std::cout << "c[" << std::dec << (unsigned long)i << "] = 0x" << std::hex << (unsigned long)c[i] << std::endl ;
#endif
      
      frame[cpt++] = (c[i] >> 24) & 0xFF ;
      frame[cpt++] = (c[i] >> 16) & 0xFF ;
      frame[cpt++] = (c[i] >>  8) & 0xFF ;
      frame[cpt++] = (c[i])       & 0xFF ;
    }
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    std::cout << __func__ << " ( { " ;
    for (tscType32 i = 0 ; i < (realSize+3+1) ; i ++ )
      std::cout << std::hex << "0x" << std::hex << frame[i] ;
    std::cout << "} )" << std::endl ;
#endif
    // End of read in FIFO return
    
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "Invalid operation on old CCU",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) |
			       setChannelKey(getChannelKey(index)) );
  }
}

/** 
 * \param frame - frame received by the warning
 * \exception FecExceptionHandler
 * \warning Alarms works with CCU 25 and not old CCU
 * \warning This method is a blocked command no interruption can be done
 * \warning The size of the frame must be DD_USER_MAX_MSG_LENGTH*4
 * \warning When a warning is emitted then the transaction number must be zero
 */
void FecRingDevice::waitForAnyCcuAlarms ( tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ) 
  throw (FecExceptionHandler) {

    // Build the frame in 32 bits
    tscType32 c[DD_USER_MAX_MSG_LENGTH], realSize32 = 0, realSize = 0 ;

    // Wait on any message coming in FIFO return
    tscType16 fecSR0 = getFecRingSR0() ;
    while (fecSR0 & FEC_SR0_RETEMPTY) {
#ifdef TIMETOWAIT
      struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
      nanosleep (&req,NULL) ;
      //usleep (TIMETOWAIT) ;
#endif
      fecSR0 = getFecRingSR0() ;
    }
    
    // read frame in fifo return
    fecSR0 = getFecRingSR0() ;
    if (fecSR0 & FEC_SR0_RETEMPTY) {
      
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
      std::cerr << __func__ << ": cannot read the frame in FIFO return" << std::endl ;
#endif    
      
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_READ_FRAME_CORRUPTED,
					 "Unable to read a frame in FIFO return, FIFO return empty",
					 ERRORCODE,
					 buildFecRingKey(getFecSlot(),getRingSlot()),
					 "FEC status register 0", fecSR0) ;
    }
    
    // Read the answer
    // Retreive the size
    c[0] = getFifoReturn() ;
  
    // Check the size of the frame
    realSize = (c[0] >> 8) & 0xFF ;
    tscType8 word2 = c[0]  & 0xFF ;
    if (realSize & FEC_LENGTH_2BYTES) {
      
      realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
      // +1 for the two byte words for length
    }

    // +3 => Dst, Src, Length  ; +1 => status after the frame
    realSize32 = (realSize+3+1)/4 ;
    if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
    
#ifdef S2NDLEVELCHECK
    // Too many words ?
    if (realSize32 > DD_MAX_MSG_LENGTH_32)

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_TOO_LONG_FRAME_LENGTH,
				 "Size in frame is bigger than the FIFO return size",
				 CRITICALERRORCODE,
				 buildFecRingKey(getFecSlot(), getRingSlot()),
				 "Size of the frame received", realSize32 ) ;
#endif

    for (tscType32 i = 1 ; i < realSize32 ; i ++) c[i] = getFifoReturn() ;

    //clean up interrupts
    setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS);
  
    int cpt = 0 ;
    for (tscType32 i = 0 ; i < realSize32 ; i ++) {
      
#ifdef DEBUGMSGERROR
      std::cerr << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
#endif
      
      frame[cpt++] = (c[i] >> 24) & 0xFF ;
      frame[cpt++] = (c[i] >> 16) & 0xFF ;
      frame[cpt++] = (c[i] >>  8) & 0xFF ;
      frame[cpt++] = (c[i])       & 0xFF ;
    }
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    std::cout << __func__ << " ( { " ;
    for (tscType32 i = 0 ; i < (realSize+3+1) ; i ++ )
      std::cout << std::hex << "0x" << std::hex << frame[i] ;
    std::cout << "} )" << std::endl ;
#endif
    // End of read in FIFO return
    
}

/** Read all the registers (status and control) from FEC and CCUs. If an errors occurs then the register set is 0xFFFF.
 * \return FecRingRegisters class with all the values
 * \warning for the time being only the registers from the CCU concerned by the index is returned
 * The read of the registers are done in the following order:
 * <ul>
 * <li> FEC SR0
 * <li> FEC SR1
 * <li> FEC CR0
 * <li> FEC SRB: last correctly transaction number to any CCU channel
 * <li> FEC SRE: Busy channels
 * <li> FEC SRF: Parity error counter
 * <li> FEC SRA: Node controller status
 * <li> FEC SRC: Redundancy configuration register
 * <li> FEC SRD: Source field of the last transaction addressed to that CCU
 * <li> CCU Control registers: CRA, CRB, CRC, CRD, CRE
 * </ul>
 * depending of the channels the following registers are read:
 * <ul>
 * <li> I2C SRC
 * <li> I2C SRB
 * <li> I2C SRD
 * <li> I2C SRA
 * <li> I2C CRA
 * </ul>
 * or
 * <ul>
 * <li> PIA SR
 * <li> PIA GCR
 * </ul>
 */
FecRingRegisters FecRingDevice::getFecRingRegisters ( keyType index ) {

#ifdef DEBUGGETREGISTERS
  bool readRegisters = readRegistersError_ ;
  readRegistersError_ = false ; // no readout of the registers in case of error to avoid the loop
#endif

  FecRingRegisters fecRingRegisters ( index ) ;

#ifdef DEBUGMSGERROR
  char msg[80] ;
  decodeKey(msg,index) ;
  std::cout << msg << std::endl ;
  decodeKey(msg,fecRingRegisters.getIndex()) ;
  std::cout << msg << std::endl ;
#endif

  // -------------------------------------------------------------------
  // FEC control and status registers

  try {
    fecRingRegisters.setFecSR0 ( getFecRingSR0() ) ;
#ifdef DEBUGMSGERROR
    std::cout << "SR0 = 0x" << std::hex << (int)fecRingRegisters.getFecSR0() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the SR0 from the FEC " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setFecSR1 ( getFecRingSR1() ) ;
#ifdef DEBUGMSGERROR
    std::cout << "SR1 = 0x" << std::hex << (int)fecRingRegisters.getFecSR1() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the SR1 from the FEC " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setFecCR0 ( getFecRingCR0() ) ;
#ifdef DEBUGMSGERROR
    std::cout << "CR0 = 0x" << std::hex << (int)fecRingRegisters.getFecCR0() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the CR0 from the FEC " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }

  // Check the SR0
  tscType16 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0)) {
    if (fecSR0 & FEC_SR0_LINKINITIALIZED) { // try to empty the FIFO is the link is init
      try {
	if (!(fecSR0 & FEC_SR0_RECEMPTY)) {
	  std::cerr << "------------------------------------------------------------" << std::endl ;
	  std::cerr << __func__ << " => FIFO receive not empty" << std::endl ;
	  checkFifoReceive() ;
	}
	if (!(fecSR0 & FEC_SR0_TRAEMPTY)) {
	  emptyFifoTransmit() ;
	  std::cerr << "FIFO transmit not empty" << std::endl ;
	}
	if (!(fecSR0 & FEC_SR0_RETEMPTY)) {
	  std::cerr << "FIFO return not empty" << std::endl ;
	  emptyFifoReturn() ;
	}
      }
      catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
	std::cerr << "Unable to emptyed the FIFO receive for FEC " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " (SR0 = 0x" << std::hex << getFecRingSR0() << ")" << std::endl ;
	std::cerr << e.what() << std::endl ;
#endif
      }
      try {
	if (!(fecSR0 & FEC_SR0_TRAEMPTY)) emptyFifoTransmit() ;
      }
      catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
	std::cerr << "Unable to emptyed the FIFO transmit for FEC " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " (SR0 = 0x" << std::hex << getFecRingSR0() << ")" << std::endl ;
	std::cerr << e.what() << std::endl ;
#endif
      }
      try {
	if (!(fecSR0 & FEC_SR0_RETEMPTY)) emptyFifoReturn() ;
      }
      catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
	std::cerr << "Unable to emptyed the FIFO return for FEC " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " (SR0 = 0x" << std::hex << getFecRingSR0() << ")" << std::endl ;
	std::cerr << e.what() << std::endl ;
#endif
      }
    }
  }

  // Check again the SR0, if error does not continue
  fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0)) return fecRingRegisters ;

  // -------------------------------------------------------------------
  // the ring here is correct
  // CCU status and control registers in a given order

  try {
    fecRingRegisters.setCcuSRB ( getCcuSRB(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "SRB(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuSRB() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the SRB for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuSRE ( getCcuSRE(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "SRE(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuSRE() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the SRE for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuSRF ( getCcuSRF(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "SRF(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuSRF() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the SRF for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuSRA ( getCcuSRA(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "SRA(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuSRA() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the SRA for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuSRC ( getCcuSRC(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "SRC(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuSRC() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the SRC for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuSRD ( getCcuSRD(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "SRD(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuSRD() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the SRD for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuCRA ( getCcuCRA(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "CRA(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuCRA() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the CRA for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuCRB ( getCcuCRB(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "CRB(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuCRB() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the CRB for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuCRC ( getCcuCRC(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "CRC(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuCRC() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the CRC for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuCRD ( getCcuCRD(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "CRD(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuCRD() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the CRD for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }
  try {
    fecRingRegisters.setCcuCRE ( getCcuCRE(index) ) ;
#ifdef DEBUGMSGERROR
    std::cout << "CRE(0x" << std::hex << getCcuKey(index) << ") = 0x" << (int)fecRingRegisters.getCcuCRE() << std::endl ;
#endif
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "Unable to read the CRE for the CCU " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << getCcuKey(index) << std::endl ;
    std::cerr << e.what() << std::endl ;
#endif
  }

  // ----------------------------------------------------------------------
  // i2c channels registers
  if (isi2cChannelCcu25(index)) {

    if (! isChannelEnabled (index)) {
      try {
#ifdef DEBUGMSGERROR
	std::cerr << "Enable the channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
#endif
	setChannelEnable ( index, true ) ;
      }
      catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
	std::cerr << "Unable to enable the i2c channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
	std::cerr << e.what() << std::endl ;
#endif
      }
    }

    try {
      fecRingRegisters.seti2cChannelSRD ( geti2cChannelSRD(index) ) ;
#ifdef DEBUGMSGERROR
      std::cout << "I2CSRD(0x" << std::hex << getChannelKey(index) << ") = 0x" << (int)fecRingRegisters.geti2cChannelSRD() << std::endl ;
#endif
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to read the i2c SRD for the channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
      std::cerr << e.what() << std::endl ;
#endif
    }

    try {
      fecRingRegisters.seti2cChannelSRC ( geti2cChannelSRC(index) ) ;
#ifdef DEBUGMSGERROR
      std::cout << "I2CSRC(0x" << std::hex << getChannelKey(index) << ") = 0x" << (int)fecRingRegisters.geti2cChannelSRC() << std::endl ;
#endif
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to read the i2c SRC for the channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
      std::cerr << e.what() << std::endl ;
#endif
    }

    try {
      fecRingRegisters.seti2cChannelSRB ( geti2cChannelSRB(index) ) ;
#ifdef DEBUGMSGERROR
      std::cout << "I2CSRB(0x" << std::hex << getChannelKey(index) << ") = 0x" << (int)fecRingRegisters.geti2cChannelSRB() << std::endl ;
#endif
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to read the i2c SRB for the channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
      std::cerr << e.what() << std::endl ;
#endif
    }

    try {
      fecRingRegisters.seti2cChannelSRA ( geti2cChannelSRA(index) ) ;
#ifdef DEBUGMSGERROR
      std::cout << "I2CSRA(0x" << std::hex << getChannelKey(index) << ") = 0x" << (int)fecRingRegisters.geti2cChannelSRA() << std::endl ;
#endif
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to read the i2c SRA for the channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
      std::cerr << e.what() << std::endl ;
#endif
    }

    try {
      fecRingRegisters.seti2cChannelCRA ( geti2cChannelCRA(index) ) ;
#ifdef DEBUGMSGERROR
      std::cout << "I2CCRA(0x" << std::hex << getChannelKey(index) << ") = 0x" << (int)fecRingRegisters.geti2cChannelCRA() << std::endl ;
#endif
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to read the i2c CRA for the channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
      std::cerr << e.what() << std::endl ;
#endif
    }
  }
  else if (isPiaChannelCcu25(index)) {

    try {
      fecRingRegisters.setPiaChannelSR ( getPiaChannelStatus(index) ) ;
#ifdef DEBUGMSGERROR
      std::cout << "PIASR(0x" << std::hex << getChannelKey(index) << ") = 0x" << (int)fecRingRegisters.getPiaChannelSR() << std::endl ;
#endif
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to read the PIA SR for the channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
      std::cerr << e.what() << std::endl ;
#endif
    }

    try {
      fecRingRegisters.setPiaChannelGCR ( getPiaChannelGCR(index) ) ;
#ifdef DEBUGMSGERROR
      std::cout << "PIAGCR(0x" << std::hex << getChannelKey(index) << ") = 0x" << (int)fecRingRegisters.getPiaChannelGCR() << std::endl ;
#endif
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "Unable to read the i2c GCR for the channel " << std::dec << (int)getFecKey(index) << " ring " << (int)getRingKey(index) << " ccu 0x" << std::hex << (int)getCcuKey(index) << " channel 0x" << (int)getChannelKey(index) << std::endl ;
      std::cerr << e.what() << std::endl ;
#endif
    }
  }

#ifdef DEBUGGETREGISTERS
  // re-enable if it was enable the read registers in case of error
  readRegistersError_ = readRegisters ;
#endif

  return (fecRingRegisters) ;
}

// -------------------------------------------------------------------------------------
//
//                                For the channel registers 
//
// -------------------------------------------------------------------------------------

/** Method in order to set a channel control register 
 * \param index - key of the corresponding channel
 * \param channelRegister - register to be accessed (command for the channel)
 * \param value - value to be written
 * \exception FecExceptionHandler
 * \warning the value can be 16 bits
 */
void FecRingDevice::setChannelRegister ( keyType index, 
                                         tscType8 channelRegister, 
                                         tscType8 value )
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  switch (channelRegister) {
  case CMD_CHANNELTRIGGERWRITECRA: break ;
  case CMD_CHANNELTRIGGERWRITECRB: break ;
  case CMD_CHANNELI2CWRITECRA: break ;
  //case CMD_CHANNELMEMWRITECRA: // Same value than next command
  case CMD_CHANNELPIAWRITEGCR: break ;
  case CMD_CHANNELPIAWRITEDDR: break ;
  case CMD_CHANNELPIAWRITEDATAREG: break ;
  case CMD_CHANNELMEMWRITEMASKREG: break ;
  default: 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid command",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "Channel register", channelRegister );
  }
#endif

  // Prepare the frame and write it to the ring
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x04, getChannelKey(index), 
    0x00, channelRegister, value } ;

  // Write frame
  writeFrame (frame) ;
}



/** Method in order to set a control register from the channel by read modify write operation
 * \param index - key of the corresponding channel
 * \param channelRegister - register to be accessed (command for the channel)
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecRingDevice::setChannelRegister ( keyType index, 
                                         tscType8 channelRegister, 
                                         tscType8 value,
                                         logicalOperationType op )
  throw (FecExceptionHandler) {
  
  // Check the index
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  switch (channelRegister) {
  case CMD_CHANNELTRIGGERWRITECRA: break ;
  case CMD_CHANNELTRIGGERWRITECRB: break ;
  case CMD_CHANNELI2CWRITECRA: break ;
  //case CMD_CHANNELMEMWRITECRA: // Same value than next command
  case CMD_CHANNELPIAWRITEGCR: break ;
  case CMD_CHANNELPIAWRITEDDR: break ;
  case CMD_CHANNELPIAWRITEDATAREG: break ;
  default: 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid command",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "Channel register", channelRegister );
  }
#endif

  // Check the command
  tscType8 readcommand = 0 ;

  switch (channelRegister) {
  case CMD_CHANNELTRIGGERWRITECRA: readcommand = CMD_CHANNELTRIGGERREADCRA ; break ;
  case CMD_CHANNELTRIGGERWRITECRB: readcommand = CMD_CHANNELTRIGGERREADCRB ; break ;
  case CMD_CHANNELI2CWRITECRA: readcommand = CMD_CHANNELI2CREADCRA ; break ;
  //case CMD_CHANNELMEMWRITECRA: // Same value than next command
  case CMD_CHANNELPIAWRITEGCR: readcommand = CMD_CHANNELPIAREADGCR ; break ;
  case CMD_CHANNELPIAWRITEDDR: readcommand = CMD_CHANNELPIAREADDDR ; break ;
  case CMD_CHANNELPIAWRITEDATAREG: readcommand = CMD_CHANNELPIAREADDATAREG ; break ;
  }

  // Read the register 
  tscType8 CR = getChannelRegister ( index, readcommand ) ;

  // Write the value with the operation done
  switch (op) {
  case CMD_OR:  value = CR | value    ; break ;
  case CMD_XOR: value = CR & (~value) ; break ;
  case CMD_AND: value = CR & value    ; break ;
  case CMD_EQUAL: break ;
  default: 
    RAISEFECEXCEPTIONHANDLER ( TSCFEC_INVALIDOPERATION,
			       "Invalid RMW command",
			       ERRORCODE ) ;
  }

  if (CR != value) setChannelRegister ( index, channelRegister, value ) ;
}

/** Method in order to retreive a channel control/status register from a CCU
 * \param index - key of the corresponding channel
 * \param channelRegister - register to be accessed (command for the channel)
 * \return value read
 * \exception FecExceptionHandler
 */   
tscType8 FecRingDevice::getChannelRegister ( keyType index,
					     tscType8 channelRegister ) 
  throw (FecExceptionHandler) {
  
  // Check the index
  basicCheckError (index); 

#ifdef T3TDLEVELCHECK
  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  switch (channelRegister) {
  case CMD_CHANNELTRIGGERREADCRA: break ;
  case CMD_CHANNELTRIGGERREADCRB: break ;
  case CMD_CHANNELTRIGGERREADSRA: break ;
  case CMD_CHANNELI2CREADCRA    : break ;
  case CMD_CHANNELI2CREADSRA    : break ;
  case CMD_CHANNELI2CREADSRB    : break ;
  case CMD_CHANNELI2CREADSRC    : break ; 
  case CMD_CHANNELI2CREADSRD    : break ;
  //case CMD_CHANNELMEMREADCRA: : break ;   // Same value than CHANNELTRIGGERREADSRA
  //case CMD_CHANNELPIAREADGCR  : break ;   // Same value than CHANNELTRIGGERREADSRA
  case CMD_CHANNELPIAREADDDR    : break ;
  case CMD_CHANNELPIAREADDATAREG: break ;
  //case CMD_CHANNELMEMREADSTATUSREG:       // Same value than next command
  case CMD_CHANNELPIAREADSTATUS : break ;
  case CMD_CHANNELMEMREADMASKREG:
    if (ccu->second->isCcu25()) break ;
  default: 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid command or wrong CCU type for this command",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "CCU register", ccuRegister );
  }
#endif

  // read the register
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x03, getChannelKey(index), 
    0xFE, channelRegister } ;

  // Write frame
  writeFrame (frame) ;
  
  // The bit force acknowledge is set or a read operation is performed
  readFrame(frame[4], frame) ;

  // Control register
  tscType8 CR = frame[5] ;
  
  return (CR) ;
}

// -------------------------------------------------------------------------------------
//
//                                For the PIA channel registers 
//
// ------------------------------------------------------------------------------------

/** Method in order to initialise PIA general control register 
 * \param index - key of the corresponding channel
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 * \warning for the CCU 25, the data direction register and the data register are also initialise
 * \warning for the old CCU, the software is not developped (if you need it, please contact fec-support@ires.in2p3.fr
 */
void FecRingDevice::setInitPiaChannel ( keyType index ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

    setChannelRegister ( index, CMD_CHANNELPIAWRITEGCR, INITCCU25PIAGCR ) ;
    ccu->second->setPiaChannelGCR (getChannelKey(index), INITCCU25PIAGCR ) ;

    // Not anymore done in this part, it must be done through the call of the 2 methods:
    //   - setPiaChannelDDR
    //   - setPiaChannelDataReg
    //setChannelRegister ( index, CMD_CHANNELPIAWRITEDDR, INITCCU25PIADDR ) ;
    //setChannelRegister ( index, CMD_CHANNELPIAWRITEDATAREG, INITCCU25PIADATAREG ) ;
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
					    "PIA channels cannot be used on old CCUs",
					    ERRORCODE,
					    index) ;

    // Channel GCR
    // setPiaChannelGCR (index, INITOLDCCUPIAGCR) ; // , CMD_OR) ;
    // Make a map of the current system
    // ccu->second->setPiaChannelGCR (getChannelKey(index), INITOLDCCUPIAGCR) ; //, CMD_OR) ;
  }
}


/** Method in order to set PIA general control register 
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecRingDevice::setPiaChannelGCR ( keyType index, tscType8 value ) throw ( FecExceptionHandler ) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEGCR, value ) ;
}

/** Method in order to set PIA general control register by read modify write operation
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 */
void FecRingDevice::setPiaChannelGCR ( keyType index, tscType8 value, logicalOperationType op ) throw ( FecExceptionHandler ) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEGCR, value, op ) ;
}

/** Method in order to retreive PIA general control register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType8 FecRingDevice::getPiaChannelGCR ( keyType index ) throw ( FecExceptionHandler ) {

  return ( getChannelRegister ( index, CMD_CHANNELPIAREADGCR)) ;
}

/** Method in order to retreive PIA status register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */ 
tscType8 FecRingDevice::getPiaChannelStatus ( keyType index ) throw ( FecExceptionHandler ) {

  return ( getChannelRegister ( index, CMD_CHANNELPIAREADSTATUS)) ;
}

/** Method in order to set PIA Data Direction Register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecRingDevice::setPiaChannelDDR ( keyType index, tscType8 value ) throw (FecExceptionHandler) {

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEDDR, value) ;

  // Check the force ack of the channel
  ccuMapAccessedType::iterator ccu =  ccuMapAccess_.find(getCcuKey(index)) ;
  if (ccu == ccuMapAccess_.end()) {

    // All the CCU are scanned in the constructor
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "The CCU is not present on the ring",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) );
  }
}

/**  Method in order to set PIA Data Direction Register by read modify write operation
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecRingDevice::setPiaChannelDDR ( keyType index, 
                                       tscType8 value, 
                                       logicalOperationType op ) 
  throw (FecExceptionHandler) {
 
  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEDDR, value, op) ;

  // Check the force ack of the channel
  ccuMapAccessedType::iterator ccu =  ccuMapAccess_.find(getCcuKey(index)) ;
  if (ccu == ccuMapAccess_.end()) {
    // All the CCU are scanned in the constructor
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "The CCU is not present on the ring",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) );
  }
}

/** Method in order to retreive PIA Data Direction Register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */ 
tscType8 FecRingDevice::getPiaChannelDDR ( keyType index ) throw ( FecExceptionHandler ) {

  return ( getChannelRegister ( index, CMD_CHANNELPIAREADDDR)) ;
}

/** Method in order to set PIA data register 
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecRingDevice::setPiaChannelDataReg ( keyType index, tscType8 value ) throw ( FecExceptionHandler ) {

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEDATAREG, value) ;
}

/** Method in order to set PIA Data Register by read modify write operation
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecRingDevice::setPiaChannelDataReg ( keyType index, 
                                       tscType8 value, 
                                       logicalOperationType op) throw (FecExceptionHandler) {

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEDATAREG, value, op) ;

#ifdef DEBUGMSGERROR
  std::cout << "PIA channel Data reg are 0x" << std::hex << (int)value << " / 0x" << std::hex << (int)getPiaChannelDataReg(index) << std::endl ;

  if (value != getPiaChannelDataReg(index)) {

    std::cout << "PIA channel Data reg different = 0x" << std::hex << (int)value << " / 0x" << std::hex << (int)getPiaChannelDataReg(index) << std::endl ;
  }
#endif
}

/** Method in order to retreive PIA data register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType8 FecRingDevice::getPiaChannelDataReg ( keyType index ) throw ( FecExceptionHandler ) {

  return ( getChannelRegister ( index, CMD_CHANNELPIAREADDATAREG)) ;
}

/** Method in order to reset a PIA channel
 * \param index - key of the corresponding channel
 * \exception FecExceptionHandler
 */
void FecRingDevice::piaChannelReset( keyType index) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Prepare the frame and write it to the ring
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x03, getChannelKey(index), 
    0x00, CMD_CHANNELRESETPIA } ;
  
  // Write frame
  writeFrame (frame) ;
}

/**
 * \param index - index of the CCU (FEC/RING/CCU/PIA channels). If channel key is equal
 * to 0 then all PIA channels will be enabled.
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning Alarms works with CCU 25 and not old CCU
 * \warning The source must be CLEARED before reenable it.
 */
void FecRingDevice::setPiaInterruptEnable ( keyType index ) throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

    if (getChannelKey(index) == 0) {

      for ( tscType8 channel = 0x30 ; channel <= 0x33 ; channel ++ ) {
	
        // Enable the channel
        if (! isChannelEnabled (index | setChannelKey(channel))) {

	  // The GCR in not initialise in this method
	  setChannelEnable (index | setChannelKey(channel), true) ;
 
	  // Write enable PIA interrupt by writing the PIA GCR<5> = 1
	  setPiaChannelGCR ( index | setChannelKey(channel), INITCCU25PIAGCR | 0x20) ;
	  ccu->second->setPiaChannelGCR ( channel - 0x30, INITCCU25PIAGCR | 0x20) ;
	}
	else {

	  // The GCR is already written in this case
	  // Write enable PIA interrupt by writing the PIA GCR<5> = 1
	  setPiaChannelGCR ( index | setChannelKey(channel), 0x20, CMD_OR ) ;
	  ccu->second->setPiaChannelGCR ( channel - 0x30, 0x20, CMD_OR ) ;
	}
      }
    }
    else {

      // Enable the channel
      if (! isChannelEnabled (index)) {
	
	// The GCR in not initialise in this method
	setChannelEnable (index, true) ;
	
	// Write enable PIA interrupt by writing the PIA GCR<5> = 1
	setPiaChannelGCR ( index, INITCCU25PIAGCR | 0x20) ;
	ccu->second->setPiaChannelGCR ( getChannelKey(index) - 0x30, INITCCU25PIAGCR | 0x20) ;
      }
      else {
	    
	// The GCR is already written in this case
	// Write enable PIA interrupt by writing the PIA GCR<5> = 1
	setPiaChannelGCR ( index, 0x20, CMD_OR ) ;
	ccu->second->setPiaChannelGCR (  getChannelKey(index) - 0x30, 0x20, CMD_OR ) ;
      }
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "CCU alarms cannot be used on old CCUs",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) |
			       setChannelKey(getChannelKey(index)) ) ;
  }  
}

/** 
 * \param index - index of the CCU (FEC/RING/CCU/PIA channels). If channel key is equal
 * to 0 then all PIA channels will be enabled.
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning Alarms works with CCU 25 and not old CCU
 */
void FecRingDevice::setPiaClearInterrupts ( keyType index ) throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

    if (getChannelKey(index) == 0) {

      for ( tscType8 channel = 0x30 ; channel <= 0x33 ; channel ++ ) {

        // Enable the channel => must be enable if the method setCcuAlarmsEnable
        // if (! isChannelEnabled (index | setChannelKey(channel))) setChannelEnable (index | setChannelKey(channel), true) ;
        // Write enable CCU by writing the PIA GCR<2> = 1
        setPiaChannelGCR ( index | setChannelKey(channel), 0x04, CMD_OR ) ;
        ccu->second->setPiaChannelGCR ( channel - 0x30, 0x04, CMD_OR ) ;
      }
    }
    else {

      // Enable the channel => must be enable if the method setCcuAlarmsEnable
      // if (! isChannelEnabled (index)) setChannelEnable (index, true) ;
      // Write enable CCU by writing the PIA GCR<5> = 1
      setPiaChannelGCR ( index, 0x04, CMD_OR ) ;
      ccu->second->setPiaChannelGCR ( getChannelKey(index) - 0x30, 0x04, CMD_OR ) ;
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "CCU alarms cannot be used on old CCUs",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) |
			       setChannelKey(getChannelKey(index)) ) ;
  }  
}

// -------------------------------------------------------------------------------------
//
//                                For the I2C channel registers 
//
// ------------------------------------------------------------------------------------

/** Method in order to initialise the I2C control register A
 * Possible values can be (h)C0 or (h)00.
 * \param index - key of the corresponding channel
 * \param forceAck - the force ack bit
 */
void FecRingDevice::setInitI2cChannelCRA ( keyType index, bool forceAck, tscType16 i2cSpeed ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index);

  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;

  tscType8 channelCRA ;
  if (ccu->second->isCcu25())
    channelCRA = INITCCU25I2CCRA ;
  else
    channelCRA = INITOLDCCUI2CCRA ;

  // I2C speed
  switch (i2cSpeed) {
  case 100:
    channelCRA &= 0xFC ;
    channelCRA |= 0x0 ;
    break ;
  case 200:
    channelCRA &= 0xFC ;
    channelCRA |= 0x1 ;
    break ;
  case 400:
    channelCRA &= 0xFC ;
    channelCRA |= 0x2 ;
    break ;
  case 1000:
    channelCRA &= 0xFC ;
    channelCRA |= 0x3 ;
    break;
  }

  // Force acknowledge
  if (forceAck) channelCRA |= I2C_CRA_FACKW ;

#ifdef DEBUGERRORMSG
  std::cout << "I2C Channel: FEC " << std::dec << getFecKey(index) 
       << " Ring " << std::dec << getRingKey(index) 
       << " CCU 0x" << std::hex << getCcuKey(index) 
       << " Channel 0x" << std::hex << getChannelKey(index) 
       << ": speed to " << std::dec << i2cSpeed 
       << " (CRA = 0x" << (int)channelCRA << ")"
       << std::endl ;
#endif

  // Set the channel CRA and set the bit force ack if needed
  // Make a map of the current system in CCUDescription class
  seti2cChannelCRA (index, channelCRA) ;
  ccu->second->seti2cChannelCRA(getChannelKey(index), forceAck) ;
}

/** Method in order to set I2C control register A
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecRingDevice::seti2cChannelCRA ( keyType index, tscType8 value) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELI2CWRITECRA, value) ;

  // Check the force ack of the channel
  ccuMapAccessedType::iterator ccu =  ccuMapAccess_.find(getCcuKey(index)) ;
  if (ccu == ccuMapAccess_.end()) {

    // All the CCU are scanned in the constructor
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "The CCU is not present on the ring",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) );
  }
  else {
    ccu->second->seti2cChannelCRA ( getChannelKey(index), value ) ;

    if ( isChannelEnable(value) )
      ccu->second->setChannelEnable ( getChannelKey(index), true ) ;
    else
      ccu->second->setChannelEnable ( getChannelKey(index), false ) ;
  }
}

/** Method in order to retreive I2C control register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */ 
tscType8 FecRingDevice::geti2cChannelCRA ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADCRA)) ;
}

/** Method in order to retreive I2C status register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */ 
tscType8 FecRingDevice::geti2cChannelSRA ( keyType index )  
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADSRA)) ;
}

/** Method in order to retreive I2C status register B from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */ 
tscType8 FecRingDevice::geti2cChannelSRB ( keyType index )  
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADSRB) ) ;
}

/** Method in order to retreive I2C status register C from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */ 
tscType8 FecRingDevice::geti2cChannelSRC ( keyType index )  
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADSRC)) ;
}

/** Method in order to retreive I2C status register D from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */ 
tscType8 FecRingDevice::geti2cChannelSRD ( keyType index )   
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADSRD)) ;
}

/** Method in order to reset an I2C channel
 * \param index - key of the corresponding channel
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
void FecRingDevice::i2cChannelReset ( keyType index ) 
  throw (FecExceptionHandler) {

  // Same command than PIA
  piaChannelReset (index) ;
}

/** Method in order to reset all the I2C channel and reconfigure the channels
 * \param index - key of the corresponding channel
 */
void FecRingDevice::i2cChannelReset ( keyType index, bool reconf )  
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  i2cChannelReset ( index ) ;

  if (reconf) {  
    
    tscType8 CRAValue ;

    ccuMapAccessedType::iterator p = ccuMapAccess_.find(getCcuKey(index)) ;
    CCUDescription *ccu = p->second ;

    for (int i = 0 ; i < MAXI2CCHANNELS ; i ++) {
      int channel = i + 0x1 ;

      CRAValue = ccu->geti2cChannelCRA ( i ) ;
      seti2cChannelCRA ( ccu->getKey() | setChannelKey(channel), CRAValue ) ; //setFecSlotKey(getFecSlot()) | setRingKey(0) | setCcuKey(ccu->getAddress()) | setChannelKey(channel), CRAValue ) ;
    }
  }
}

/** Test the CCU control register E in order to know if a channel is enable or not
 * \param index - index of the channel
 * \return true if the channel is enable, false if not
 * \warning this method is only available for CCU 25
 */
bool FecRingDevice::isChannelEnabled ( keyType index )  
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Check if the channel is enable
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
 
  if (ccu->second->isCcu25()) { // CCU 25

    // Read the CRE
    tscType24 CRE = getCcuCRE (index) ;
    tscType32 channel = getChannelKey (index) ;

    if (isi2cChannelCcu25 (index)) {
      
      // Enable the I2C channel bit 0 - 15
      if (CRE & (0x1 << (channel - 0x10))) {
        return true ;
      }
      //else std::cout << "Channel 0x" << std::hex << channel << " = false" << std::endl ;
      
    } else if (isPiaChannelCcu25 (index)) {

      // Enable PIA channel bit 16-19
      if (CRE & (0x1 << (16 + channel - 0x30))) {
          
        return true ;
      }
        
    } else if (isMemoryChannelCcu25 (index)) {
      
      if (CRE & 0x100000) {
        
        return true ;
      }
      
    } else if (isTriggerChannelCcu25 (index)) {
      
      // Enable the trigger channel bit 21
      if (CRE & 0x200000) {
        
        return true ;
      }
      
    } else if (isJtagChannelCcu25 (index)) {
      
      // Enable the JTAG channel bit 22
      if (CRE & 0x400000) {
        
        return true ;
      }
    }
  }
  else { // Old CCU

    // For the OLD CCU, 
    // - the node controller of the CCU is enable for this channel
    // - enable the channel i2c in the channel
    tscType8 CRA = getCcuCRA ( index ) ;
    
    if ( isi2cChannelOldCcu(index) ) { // I2C channel
      
      if ((CRA & 0x1) && isChannelEnable(geti2cChannelCRA(index))) {
        
        return true ;
      }
    }
    else if ( isPiaChannelOldCcu(index) ) { // PIA channel

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
				 "PIA channels cannot be used on old CCUs",
				 ERRORCODE,
				 setFecSlotKey(getFecKey(index)) |
				 setRingKey(getRingKey(index)) |
				 setCcuKey(getCcuKey(index)) |
				 setChannelKey(getChannelKey(index)) ) ;
    }
  }

  return false ;
}

/** Test the CCU status register E in order to know if a channel is busy or not
 * \param index - index of the channel
 * \return true if the channel is busy, false if not
 * \warning this method is only available for CCU 25
 */
bool FecRingDevice::isChannelBusy ( keyType index )  
  throw (FecExceptionHandler) {

  //char msg[80] ;
  //decodeKey(msg,index) ;
  //std::cout << __func__ << " begin with " << msg << std::endl; ;

  // Check the index
  basicCheckError (index) ;

  // Check if the channel is enable 
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  
  if (ccu->second->isCcu25()) { // CCU 25

    // Read the CRE
    tscType24 SRE = getCcuSRE (index) ;
    tscType32 channel = getChannelKey (index) ;

    if (isi2cChannelCcu25 (index)) {
      
      // Enable the I2C channel bit 0 - 15
      if (SRE & (0x1 << (channel - 0x10))) {
        
        return true ;
      }
      
    } else if (isPiaChannelCcu25 (index)) {
      
      // Enable PIA channel bit 16-19
      if (SRE & (0x1 << (16 + channel - 0x30))) {
          
        return true ;
      }
        
    } else if (isMemoryChannelCcu25 (index)) {
      
      if (SRE & 0x100000) {
        
        return true ;
      }
      
    } else if (isTriggerChannelCcu25 (index)) {
      
      // Enable the trigger channel bit 21
      if (SRE & 0x200000) {
        
        return true ;
      }
      
    } else if (isJtagChannelCcu25 (index)) {
      
      // Enable the JTAG channel bit 22
      if (SRE & 0x400000) {
        
        return true ;
      }
    }
  }

  return false ;
}

/** Force the acknowledge bit corresponding to the channel in the key
 * \param index - key of the channel
 * \param enable - boolean for enable or disable the force ack bit
 */
void FecRingDevice::setChannelForceAck ( keyType index, bool enable )   
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Check if the channel is forced
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;

  tscType8 channel = getChannelKey(index) ;

  if (ccu->second->getBitForceAck (getChannelKey(index)) != enable) {

    // Channel Control register A 
    if (ccu->second->isCcu25 ()) { // CCU 25

      if (enable) {
        setChannelRegister (index, CMD_CHANNELI2CWRITECRA, 0x40, CMD_OR) ;
      }
      else {
        setChannelRegister (index, CMD_CHANNELI2CWRITECRA, 0x40, CMD_XOR) ;
      }
    }
    else {                         // Old CCUs

      // It seems that for old CCUs the I2C channel CRA cannot be read back
      tscType8 value = INITOLDCCUI2CCRA ;
      if (enable) value |= 0x40 ;
      else value &= ~0x40 ;

      seti2cChannelCRA (index, value) ;
      ccu->second->seti2cChannelCRA (channel, value) ;
    }
  }

  // Set the channel as enable in the CCU class
  ccu->second->setBitForceAck(channel, enable) ;
}

/** Write a value to the device speicied by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecRingDevice::writei2cDevice ( keyType index, tscType8 value ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame
  //  Dest        = index, 
  //  Src         = 0x0, 
  // Length      = 5, 
  // Channel     = index
  // Transaction = Assisgn by the device driver
  // Command     = single byte write normal mode
  // Address     = index
  // DW          = value

  // Check if the force acknowledge is set
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;

  tscType8 transaction = 0 ;  
  tscType8 channel = getChannelKey(index) ;

  if (ccu->second->getBitForceAck(channel)) transaction = 0xFE ;

  // Build the frame
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x05, getChannelKey(index), 
    transaction, CMD_SINGLEBYTEWRITENORMALMODE,
    getAddressKey(index), value } ;

  // Write Frame
  writeFrame (frame) ;
  
  // Check if the force acknowledge is set and must be retreive
  if (transaction == 0xFE) {

    // The bit force acknowledge is set
    readFrame(frame[4], frame, 3) ;
  }
}

/** Write a value to the device specified by the key in extended mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecRingDevice::writei2cDevice ( keyType index, 
                                     tscType8 offset, 
                                     tscType8 value ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame
  //  Dest        = index, 
  // Src         = 0x0, 
  // Length      = 5, 
  // Channel     = index
  // Transaction = Assisgn by the device driver
  // Command     = single byte write ral mode
  // Address     = index
  // Offset      = offset
  // DW          = value

  // Check if the force acknowledge is set
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;

  tscType8 transaction = 0 ;
  tscType8 channel = getChannelKey(index) ;

  if (ccu->second->getBitForceAck(channel)) transaction = 0xFE ;

  // Build the frame
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x06, getChannelKey(index), 
    transaction, CMD_SINGLEBYTEWRITERALMODE,
    getAddressKey(index), offset, value } ;
  
  // Write it
  writeFrame (frame) ;
  
  // Check if the force acknowledge is set and must be retreive
  if (transaction == 0xFE) {
    
    // The bit force acknowledge is set 
    readFrame(frame[4], frame, 3) ;
  }
}

/** Write a value to the device specified by the key in RAL mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param value - value to be written
 */
void FecRingDevice::writei2cRalDevice ( keyType index, 
                                        tscType8 offset, 
                                        tscType8 value )   
  throw (FecExceptionHandler) {

  // Same as write extended mode
  writei2cDevice (index, offset, value) ;
}

/** Write a value to the device speiciied by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param decal - shift the value. Use only for the laserdriver
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecRingDevice::writei2cDeviceOffset ( keyType index, 
                                           tscType8 offset, 
                                           tscType8 decal, 
                                           tscType8 value ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame
  // Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 5, 
  //  Channel     = index
  //  Transaction = Assign by the device driver
  //  Command     = single byte write normal mode
  //  Address     = index
  //  DW          = value

  // Check if the force acknowledge is set
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;

  tscType8 transaction = 0 ;
  tscType8 channel = getChannelKey(index) ;

  if (ccu->second->getBitForceAck(channel)) transaction = 0xFE ;

  // Build the frame
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x05, getChannelKey(index), 
    transaction, CMD_SINGLEBYTEWRITENORMALMODE,
    (tscType8)((getAddressKey(index) << decal) | offset), value } ;

  // Write it
  writeFrame (frame) ;
  
  // Check if the force acknowledge is set and must be retreive
  if (transaction == 0xFE) {

    // The bit force acknowledge is set
    readFrame(frame[4], frame, 3) ;    
  }
}

/** Write a value to the device speiciied by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param value - value to be written
 */
void FecRingDevice::writei2cDeviceOffset ( keyType index, 
                                           tscType8 offset, tscType8 value )   
  throw (FecExceptionHandler) {

  writei2cDeviceOffset (index, offset, 0) ;
}

/** Read a value to the device specified by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \return the value read in the register
 * \exception FecExceptionHandler
 */
tscType8 FecRingDevice::readi2cDevice ( keyType index ) 
  throw (FecExceptionHandler) {
  
  // Check the index
  basicCheckError (index) ;
  
  // Build the frame and send it via the method write frame
  //  Dest        = index, 
  // Src         = 0x0, 
  // Length      = 4, 
  // Channel     = index
  // Transaction = Assisgn by the device driver
  // Command     = single byte read normal mode
  // Address     = index
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x04, getChannelKey(index), 
    0xFE, CMD_SINGLEBYTEREADNORMALMODE,
    getAddressKey(index) } ;
  
  // Write it
  writeFrame (frame) ;

  // Read back the value asked
  readFrame(frame[4], frame, 4) ;

  return (frame[5]) ;
}

/** Read a value to the device specified by the key in extended mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param addressMSB - give an offset to the address set in the constructor
 * \return the value read in the register
 * \exception FecExceptionHandler
 */
tscType8 FecRingDevice::readi2cDevice ( keyType index, 
                                        tscType8 addressMSB ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame 
  // Dest        = index, 
  // Src         = 0x0, 
  // Length      = 4, 
  // Channel     = index
  // Transaction = Assisgn by the device driver
  // Command     = single byte read normal mode
  // Address     = index (base address for bits 7-0
  // AddressMSB  = base address for bits 15-8
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x05, getChannelKey(index), 
    0xFE, CMD_SINGLEBYTEREADEXTENDEDMODE,
    getAddressKey(index), addressMSB } ;

  // Write it
  writeFrame (frame) ;

  // Read back the value asked 
  readFrame(frame[4], frame, 4) ;

  return (frame[5]) ;
}

/** Read a value to the device specified by the key in RAL mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \return the value read in the register
 * \exception FecExceptionHandler
 * \bug It seems that an error on read back doesn't correspond to a device driver error
 */
tscType8 FecRingDevice::readi2cRalDevice ( keyType index, 
                                           tscType8 offset ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame
  // Dest        = index, 
  // Src         = 0x0, 
  // Length      = 4, 
  // Channel     = index
  // Transaction = Assisgn by the device driver
  // Command     = single byte read normal mode
  // Address     = index
  // Offset      = offset
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x05, getChannelKey(index), 
    0xFE, CMD_SINGLEBYTEREADRALMODE,
    getAddressKey(index), offset } ;

  writeFrame (frame) ;

  // Read back the value asked
  readFrame(frame[4], frame, 4) ;


  return (frame[5]) ;
}

/** Read a value to the device specified by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param decal - shift the value. Use only for the laserdriver
 * \return the value read in the register
 * \exception FecExceptionHandler
 */
tscType8 FecRingDevice::readi2cDeviceOffset ( keyType index, 
                                              tscType8 offset, 
                                              tscType8 decal ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;
  
  // Build the frame and send it via the method write frame
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 4, 
  //  Channel     = index
  //  Transaction = Assisgn by the device driver
  //  Command     = single byte read normal mode
  //  Address     = index
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x04, getChannelKey(index), 
    0xFE, CMD_SINGLEBYTEREADNORMALMODE,
    (tscType8)((getAddressKey(index) << decal) | offset) } ;
  
  // Write it
  writeFrame (frame) ;

  // Read back the value asked
  readFrame(frame[4], frame, 4) ;  

  return (frame[5]) ;
}

/** Read a value to the device specified by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \return the value read in the register
 */
tscType8 FecRingDevice::readi2cDeviceOffset ( keyType index, tscType8 offset ) 
  throw (FecExceptionHandler) {

  return (readi2cDeviceOffset (index, offset, 0)) ;
}

/** Scan the ring for each FEC and create a list associated
 * for all the I2C devices found
 * \param deviceValues - array of device address with deviceValues[0] = a key with device address and possible channel to be tested
 * and deviceValues[1] = mode (NORMALMODE, EXTENDEDMODE, RALMODE)
 * \param size - number of devices
 * \param noBroadcast - if true all the CCU from 0x1 to 0x79 is checked (fecScanRingNoBroadcast)
 * \param display - display a message for each channel scanned
 * else the CCU broadcast mode is used
 * \param display messages
 * \return a list of keyType
 * \exception FecExceptionHandler
 * \warning this method is dedicated to the i2c devices
 * \warning this method is dedicated to the CCU 25 but can work for old CCUs
 * \warning if the channel part of deviceValues is zero then all the devices are tested on all channel. 
 * if the channel part is different from 0 then a mask is done with all the possible channels and the
 * mack given in the key.
 */
std::list<keyType> *FecRingDevice::scanRingForI2CDevice ( keyType *deviceValues, 
							  tscType32 sizeDevices,
							  bool noBroadcast,
							  bool display ) 
  throw (FecExceptionHandler) {

  // --------------------------------------------------------------------------------------
  // Check the FEC ring SR0
  tscType16 fecSR0 = getFecRingSR0() ;
  if (! isFecSR0Correct(fecSR0)) {
    
    //std::cerr << "The " << (int)getFecSlot() << "." << (int)getRingSlot() << " SR0 0x" << std::hex << fecSR0 << " is not nominal, the frame is not sent" 
    //<< std::dec << std::endl ;

    // filter the frames to try to recover it if the ring is closed
    if (fecSR0 & FEC_SR0_LINKINITIALIZED) {

      if (!(fecSR0 & FEC_SR0_RECEMPTY)) {
	checkFifoReceive ( ) ;
	// Check again the SR0
	fecSR0 = getFecRingSR0() ;
      }

      // if the IRQ is on: The IRQ must be cleared at this point if the readFrame was not called
      // This method does clear the IRQ after this clear
      if ((fecSR0 & FEC_SR0_PENDINGIRQ) || (fecSR0 & FEC_SR0_DATATOFEC)) {
	setFecRingCR1 (FEC_CR1_CLEARIRQ);
	// Check again the SR0
	fecSR0 = getFecRingSR0() ;
      }
    }

    // Check again the SR0
    if (! isFecSR0Correct(fecSR0)) {
#ifdef DEBUGMSGERROR
      std::cerr << "The " << (int)getFecSlot() << "." << (int)getRingSlot() << " SR0 0x" << std::hex << fecSR0 << " is not nominal, the frame is not sent" 
		<< std::dec << std::endl ;
#endif    
      
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
				  "fails on sending a frame, ring lost",
				  FATALERRORCODE,
				  buildFecRingKey(getFecSlot(),getRingSlot()),
				  "FEC status register 0", fecSR0) ;
    }
  }

#ifdef DEBUGGETREGISTERS
  bool readRegisters = readRegistersError_ ;
  readRegistersError_ = false ;
#endif

  std::list<keyType> *deviceList = NULL ;

  //#ifndef SCANCCUATSTARTING
  // If the scan order was not done in the FecRingDevice constructor => do it
  if (noBroadcast)
    fecScanRingNoBroadcast ( ) ;
  else
    fecScanRingBroadcast ( ) ;
  //#endif

  // For each CCU
  for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();p!=ccuMapAccess_.end();p++) {

    CCUDescription *ccu = p->second ;

    if (ccu != NULL) {

      tscType8 offset = ccu->isCcu25() ? 0x10 : 0x1 ;

      for (int ci = 0 ; ci < MAXI2CCHANNELS ; ci ++) {

        tscType8 channel = ci + offset ;

        // build a correct index
	keyType index = buildCompleteKey ( getFecSlot(), getRingSlot(), getCcuKey(ccu->getKey()), channel, 0) ;

        // Check the force ack
        bool forceAck = ccu->getBitForceAck(channel) ;

        // Check if the channel is already enabled
        bool channelEnable = false ;

        try {
          // Check if channel is enabled
          channelEnable = isChannelEnabled (index) ;

// 	  if (channelEnable == true) {
// 	    std::cout << std::endl << "CCU 0x" << std::hex << getCcuKey(index) << " Channel 0x" << std::hex << getChannelKey(index) << " already enable" << std::endl ;
// 	    int cra = (int)geti2cChannelCRA(index) ;
// 	    std::cout << "CCU 0x" << std::hex << getCcuKey(index) << " Channel 0x" << std::hex << getChannelKey(index) << ": CRA = " << cra << std::endl ;
// 	    if (isChannelForceAck(cra) && !forceAck) {
// 	      char msg[80] ; decodeKey(msg,index) ;
// 	      std::cout << "Incoherence in the system for the CCU " << msg << std::endl ;
// 	    }
// 	  }
        }
        catch (FecExceptionHandler &e) {
            
          std::cerr << e.what() << std::endl ;
          std::cerr << "Continue with this error ...\n" << std::endl ;
        }          

        try {            
          ccu->setChannelEnable (channel, channelEnable) ;
        }
        catch (FecExceptionHandler &e) {
            
          std::cerr << e.what() << std::endl ;
          std::cerr << "Continue with this error ...\n" << std::endl ;
        }
        
        try {
          // Enable the channel
          if (!channelEnable) setChannelEnable (index, true) ;
 
          // Force the acknowledge
          if (!forceAck) setChannelForceAck (index, true) ;
        }
        catch (FecExceptionHandler &e) {
            
          std::cerr << e.what() << std::endl ;
          std::cerr << "Continue with this error ...\n" << std::endl ;
        }
	
        // Try to write/read to each device given
        for (tscType32 di = 0 ; di < (sizeDevices*2) ; di += 2) {
	  
	  // Test if a channel is set in the sizeDevices
	  // If yes, the test is only performed on this channels
	  bool test = true ;
	  if (getChannelKey(deviceValues[di]) != 0)
	    test = (getChannelKey(deviceValues[di]) == channel) ;
            
	  if (test) {

	    tscType8 address = getAddressKey(deviceValues[di]) ;
	    
	    try {

	      index = buildCompleteKey ( getFecSlot(), getRingSlot(), getCcuKey(ccu->getKey()), channel, address) ;

#ifndef DEBUGMSGERROR
	    if (display) {
#endif
	      std::cout << "Probing on FEC " << std::dec << (int)getFecSlot() << " ring " << (int)getRingSlot() << " CCU 0x" << std::hex << (int)getCcuKey(ccu->getKey()) << " channel " << std::dec << (int)channel << " address 0x" << std::hex << (int)address << "\r" ;
	      fflush (stdout) ;
#ifndef DEBUGMSGERROR
	    }
#endif      

	    switch (deviceValues[di+1]) {
	    case NORMALMODE:
	      readi2cDevice ( index ) ;
	      break ;
	    case RALMODE:
	      readi2cRalDevice ( index, 0 ) ;
	      break ;
	    case EXTENDEDMODE:
	      readi2cDevice ( index, 0 ) ;
	      break ;
	    default: // Normal mode by default
	      readi2cDevice ( index ) ;
	      break ;
	    }
	    
#ifndef DEBUGMSGERROR
	    if (display) {
#endif
	      std::cout << "Probing on FEC " << std::dec << (int)getFecSlot() << " ring " << (int)getRingSlot() << " CCU 0x" << std::hex << (int)getCcuKey(ccu->getKey()) << " channel " << std::dec << (int)channel << " address 0x" << std::hex << (int)address << " ==> Found" << std::endl ;;
	      fflush (stdout) ;
#ifndef DEBUGMSGERROR
	    }
#endif 

	      // Insert it in the list
	      if (deviceList == NULL) deviceList = new std::list<keyType> ;
	      deviceList->push_back (index) ;
	    }
	    catch (FecExceptionHandler &e) {// nothing => no device 
#ifdef DEBUGMSGERROR
	      std::cerr << e.what() << std::endl ;
#endif
	    }
	  }
	}

        try {

          // Set the force ack
          if (!forceAck) 
	    setChannelForceAck (index, false) ;
          
          // Enable the channel
          if (!channelEnable)
	    setChannelEnable (index, false) ;
        }
        catch (FecExceptionHandler &e) {
	    
          std::cerr << e.what() << std::endl ;
          std::cerr << "Continue with this error ...\n" << std::endl ;
        }
      }
    }
  }

#ifdef DEBUGGETREGISTERS
  // re-enable if it was enable the read registers in case of error
  readRegistersError_ = readRegisters ;
#endif

  return (deviceList) ;
}

/** scan the ring for each FEC and create a vector associated
 * for all the tracker devices (I2C bus)
 * \param noBroadcast - use (false) or not (true) the CCU broadcast => (default false)
 * \param display - display a message for each channel scanned => (default true)
 * \return a list of keyType
 * \exception FecExceptionHandler
 * \warning this method is dedicated to the i2c devices
 * \warning this method is dedicated to the CCU 25 but can work for old CCUs
 */
std::list<keyType> *FecRingDevice::scanRingForI2CDevice ( bool noBroadcast, bool display ) 
  throw (FecExceptionHandler) {

#ifdef DEBUGGETREGISTERS
  bool readRegisters = readRegistersError_ ;
  readRegistersError_ = false ;
#endif

  std::list<keyType> *deviceList = NULL ;

  //#ifndef SCANCCUATSTARTING
  // If the scan order was not done in the FecRingDevice constructor => do it
  if (noBroadcast)
    fecScanRingNoBroadcast ( ) ;
  else
    fecScanRingBroadcast ( ) ;
  //#endif

  // For each CCU
  for (ccuMapAccessedType::iterator p=ccuMapAccess_.begin();p!=ccuMapAccess_.end();p++) {

    CCUDescription *ccu = p->second ;
    
    if (ccu != NULL) {

      tscType8 offset = ccu->isCcu25() ? 0x10 : 0x1 ;

      // For each channel
      for (int ci = 0 ; ci < MAXI2CCHANNELS ; ci ++) {

        tscType8 channel = ci + offset ;

        // build a correct index
        keyType index = buildCompleteKey ( getFecSlot(), getRingSlot(), getCcuKey(ccu->getKey()), channel, 0) ;
        
        // Check the force ack
        bool forceAck = ccu->getBitForceAck(channel) ;

        // Enable the channel
        setChannelEnable (index, true) ;

        // Force the acknowledge
        if (!forceAck) setChannelForceAck (index, true) ;           

        // Try to write/read to each device given
        for (tscType8 address = 0 ; address <= MAXI2CADDRESS ; address += 1) {

          try {

#ifndef DEBUGMSGERROR
	    if (display) {
#endif
	      std::cout << "Probing on FEC " << std::dec << (int)getFecSlot() << " ring " << (int)getRingSlot() << " CCU 0x" << std::hex << (int)getCcuKey(ccu->getKey()) << " channel " << std::dec << (int)channel << " address 0x" << std::hex << (int)address << "\r" ;
	      fflush (stdout) ;
#ifndef DEBUGMSGERROR
	    }
#endif

            readi2cDevice ( index | setAddressKey(address) ) ;

#ifndef DEBUGMSGERROR
	    if (display)
#endif
	      std::cout << "Probing on FEC " << std::dec << (int)getFecSlot() << " ring " << (int)getRingSlot() << " CCU 0x" << std::hex << (int)getCcuKey(ccu->getKey()) << " channel " << std::dec << (int)channel << " address 0x" << std::hex << (int)address << " ==> Found a register" << std::endl ;

            // Insert it in the list
            if (deviceList == NULL) deviceList = new std::list<keyType> ;
            deviceList->push_back (index | setAddressKey(address)) ;
            
          }
          catch (FecExceptionHandler &e) {// nothing => no device 
          }
        }

        // Set the force ack
        if (!forceAck) setChannelForceAck (index, false) ;
      }
    }
  }

#ifdef DEBUGGETREGISTERS
  // re-enable if it was enable the read registers in case of error
  readRegistersError_ = readRegisters ;
#endif

  return (deviceList) ;
}

// -------------------------------------------------------------------------------------
//
//                                For the memory channel
//
// Attention, si on veut ecrire un mot en multiple byte write et multiple byte read, il 
// faut faire attention aux tailles de la longueur de la trame (Src/Dest/Length) et la
// la longueur demandee pour la lecture:
//       Longueur   Codage de la taille  Lenh Lenl     Remarque
//       0 - 127    1 mot                0x0  0x807F   La taille doit etre augmenter de 2
//       0 - 255    2 mots               0x80 0x80FF   La taille doit etre augmenter de 2
//       256 - 1024 2 mots               0x9F 0x80FF   La taille doit etre augmenter de 2
// Attention ces tailles tiennent compte de l'augmentation de 2
//
// Note that the write on the memory channel is more longer than the read on the memory
// channel.In fact, a write need two (very) long frames data - direct ack ; a read
// need one small frame (request) and one long frame (answer) (+ direct ack)..
// -------------------------------------------------------------------------------------

/** Method in order to reset the memory channel
 * \param index - key of the corresponding channel
 */
void FecRingDevice::memoryChannelReset( keyType index ) 
  throw (FecExceptionHandler) {

  piaChannelReset (index) ;
}


/** Method in order to set the memory control register A
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecRingDevice::setMemoryChannelCRA ( keyType index, tscType8 value )  
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELMEMWRITECRA, value ) ;
}

/** Method in order to set the memory control register A
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecRingDevice::setMemoryChannelCRA ( keyType index, tscType8 value,
                                          logicalOperationType op ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELMEMWRITECRA, value, op ) ;
}

/** Method in order to set a memory channel WIN?? register 
 * \param index - key of the corresponding channel
 * \param memoryRegister - register to be accessed (command for the channel)
 * \param value - value to be written (16 bits)
 * \exception FecExceptionHandler
 */
 void FecRingDevice::setMemoryChannelWinReg ( keyType index, 
                                              tscType8 memoryRegister, 
                                              tscType16 value ) 
   throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // The basic check is done in the setChannelRegister

  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;

  switch (memoryRegister) {
  case CMD_CHANNELMEMWRITEWIN1LREG: 
  case CMD_CHANNELMEMWRITEWIN1HREG: 
  case CMD_CHANNELMEMWRITEWIN2LREG: 
  case CMD_CHANNELMEMWRITEWIN2HREG: // 16 bits register
    if (ccu->second->isCcu25()) break ;
  default: 
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid command or wrong CCU type for this command",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "Memory register", memoryRegister );
  }

  // Prepare the frame and write it to the ring
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x05, getChannelKey(index), 
    0x00, memoryRegister, (tscType8)(value >> 8),(tscType8) (value & 0xFF)} ;

  // Write it!
  writeFrame (frame) ;
}

/** Method in order to set the memory WIN1L register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecRingDevice::setMemoryChannelWin1LReg ( keyType index, tscType16 value ) 
  throw (FecExceptionHandler) {

  // The basic check is done in setMemoryChannelWinReg
  setMemoryChannelWinReg ( index, CMD_CHANNELMEMWRITEWIN1LREG, value ) ;
}

/** Method in order to set the memory WIN1H register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecRingDevice::setMemoryChannelWin1HReg ( keyType index, tscType16 value ) 
  throw (FecExceptionHandler) {

  // The basic check is done in setMemoryChannelWinReg
  setMemoryChannelWinReg ( index, CMD_CHANNELMEMWRITEWIN1HREG, value ) ;
}

/** Method in order to set the memory WIN2L register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecRingDevice::setMemoryChannelWin2LReg ( keyType index, tscType16 value ) 
  throw (FecExceptionHandler) {
  // The basic check is done in setMemoryChannelWinReg
  setMemoryChannelWinReg ( index, CMD_CHANNELMEMWRITEWIN2LREG, value ) ;
}

/** Method in order to set the memory WIN2H register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecRingDevice::setMemoryChannelWin2HReg ( keyType index, tscType16 value ) 
  throw (FecExceptionHandler) {

  // The basic check is done in setMemoryChannelWinReg
  setMemoryChannelWinReg ( index, CMD_CHANNELMEMWRITEWIN2HREG, value ) ;
}

/** Method in order to set the memory mask register
 * \param index - key of the corresponding channel
 * \param value - value used during a RMW operation (see below)
 * \param op - logical operation (and / or / xor)
 */
void FecRingDevice::setMemoryChannelMaskReg ( keyType index, tscType8 value ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELMEMWRITEMASKREG, value ) ;
}

/** Method in order to retreive memory control register A
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType8 FecRingDevice::getMemoryChannelCRA ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELMEMREADCRA)) ;
}

/** Method in order to retreive memory mask register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType8 FecRingDevice::getMemoryChannelMaskReg ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELMEMREADMASKREG)) ;
}

/** Method in order to retreive a memory control register (WIN??)
 * \param index - key of the corresponding channel
 * \param memoryRegister - register to be accessed (command for the channel)
 * \return value read
 * \exception FecExceptionHandler
 */
tscType16 FecRingDevice::getMemoryChannelWinReg ( keyType index,
                                                  tscType8 memoryRegister ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index); 

#ifdef T3TDLEVELCHECK
  // The basic check is done in the getChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  if (! ccu->second->isCcu25()) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "PIA channels cannot be used on old CCUs",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)) |
			       setChannelKey(getChannelKey(index)) ) ;
  }

  switch (memoryRegister) {
  case CMD_CHANNELMEMREADWIN1LREG: break ;
  case CMD_CHANNELMEMREADWIN1HREG: break ;
  case CMD_CHANNELMEMREADWIN2LREG: break ;
  case CMD_CHANNELMEMREADWIN2HREG: break ;
  default:     
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Invalid command or wrong CCU type for this command",
			       ERRORCODE,
			       setFecSlotKey(getFecKey(index)) |
			       setRingKey(getRingKey(index)) |
			       setCcuKey(getCcuKey(index)),
			       "Memory register", memoryRegister );
  }
#endif

  // read the register
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x03, getChannelKey(index), 
    0xFE, memoryRegister } ;

  // Write it
  writeFrame (frame) ;
 
  // The bit force acknowledge is set or a read operation is performed
  readFrame(frame[4], frame) ;

  // Check the size of the frame
  if (frame[2] != 4) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
				       "Cannot read back memory channel register (bad size in frame, should be 4)",
				       ERRORCODE,
				       buildFecRingKey(getFecSlot(),getRingSlot()),
				       "Frame size", frame[2]) ;
  }

  tscType8 CR = (frame[5] << 8) | frame[6] ;
  
  return (CR) ;
}

/** Method in order to retreive memory WIN1L register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecRingDevice::getMemoryChannelWin1LReg ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getMemoryChannelWinReg ( index, CMD_CHANNELMEMREADWIN1LREG)) ;
}

/** Method in order to retreive memory WIN1H register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecRingDevice::getMemoryChannelWin1HReg ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getMemoryChannelWinReg ( index, CMD_CHANNELMEMREADWIN1HREG)) ;
}

/** Method in order to retreive memory WIN2L register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecRingDevice::getMemoryChannelWin2LReg ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getMemoryChannelWinReg ( index, CMD_CHANNELMEMREADWIN2LREG)) ;
}

/** Method in order to retreive memory WIN2H register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecRingDevice::getMemoryChannelWin2HReg ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getMemoryChannelWinReg ( index, CMD_CHANNELMEMREADWIN2HREG)) ;
}

/** Method in order to retreive memory mask register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType8 FecRingDevice::getMemoryChanneMaskReg ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELMEMREADMASKREG)) ;
}

/** Method in order to retreive memory status register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType8 FecRingDevice::getMemoryChannelStatus ( keyType index ) 
  throw (FecExceptionHandler) {

  return ( getChannelRegister ( index, CMD_CHANNELMEMREADSTATUSREG)) ;
}

/** Write a value to the memory specified by the key (single byte mode)
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecRingDevice::writeIntoMemory ( keyType index, 
                                      tscType8 AH, tscType8 AL, 
                                      tscType8 value ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame
  // Dest        = index, 
  // Src         = 0x0, 
  // Length      = 6, 
  // Channel     = index
  // Transaction = Assign by the device driver
  // Command     = single byte write
  // AH          = Position in the high memory
  // AL          = Position in the low memory
  // DW          = value
  
  // Build the frame
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x06, getChannelKey(index), 
    0x00, CMD_CHANNELMEMSINGLEBYTEWRITE,
    AH, AL, value } ;

  // Write it
  writeFrame (frame) ;
}

/** Write a value to the memory specified by the key (multiple byte mode)
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param values - values to be written
 * \param size - size of the buffer values
 * \warning no exception will be managed due to the fact that multiple write returns always an error
 * \warning if the frame size of the memory size is greater than 127, the length word must be
 * separated in two words (with source, destination and length frame values):
 * <ul>
 * <li>For frame length < 128 => size
 * <li>For frame length > 127 and < 255 => 0x80 0xSIZE
 * <li>For frame length > 255 and < FIFO size => (0x80 | LENGTH > 255) 0xLENGTH
 * </ul>
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecRingDevice::writeIntoMemory ( keyType index, 
                                      tscType8 AH, tscType8 AL, 
                                      tscType8 *values,
                                      unsigned long size ) 
  throw (FecExceptionHandler) {



  // 127 means FEC_UPPERLIMIT_LENGTH
  // 119 means FEC_UPPERLIMIT_LENGTH - 5 - 3

  // Check the index
  basicCheckError (index) ;

  if (size >= MAXMEMORYBYTESADDRESS) {
    
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Too many bytes to be written in the memory channel",
			       ERRORCODE,
			       index,
			       "Size", size) ;
  }

  tscType16 sizeMaxFrame = DD_USER_MAX_MSG_LENGTH*4 ;

  if ( sizeMaxFrame > FEC_UPPERLIMIT_LENGTH && size > (FEC_UPPERLIMIT_LENGTH-5-3) ) { 
    // 119 data => 3 words are reserved for the Dest / Src / Length
    //          => 5 words for the reset of the frame: channel / Transaction / Command / AH / AL

    writeIntoMemory2BytesLength ( index, AH, AL, values, size ) ;

    // Finished
    return ;
  }

  // Build the frame and send it via the method write frame
  // Dest        = index, 
  // Src         = 0x0, 
  // Length      = sizeMaxFrame - 3 (maximum)
  // Channel     = index
  // Transaction = Assign by the device driver
  // Command     = multiple byte write
  // AH          = Position in the high memory
  // AL          = Position in the low memory
  // DW[size]    = values

  // Counter of values
  unsigned long counter = 0, downloaded = 0 ;
  tscType8 al = AL, ah = AH ;

  while (counter < size) {

    // Build the frame
    tscType8 sizet = sizeMaxFrame < (size-counter+5+3) ? sizeMaxFrame : (size-counter+5+3) ;
    
    // src/dest/length
    sizet -= 3 ;

    tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
      getCcuKey(index), FRAMEFECNUMBER, 
      sizet, getChannelKey(index), 
      0x00, CMD_CHANNELMEMMULTIPLEBYTEWRITE,
      AH, AL } ;

    // Insert in the frame the values
    // constant 8 is Dest / Src / Length / Channel / Transaction / Command / AH / AL
    memcpy (&frame[counter-downloaded+8], &values[counter], sizeof(tscType8) * (sizet-5)) ; counter += (sizet-5) ;

    // Already downloaded
    downloaded = counter ;
    AL = (al+ah*255+downloaded) & 0xFF ;
    AH = ((al+ah*255+downloaded) & 0xFF00) >> 8 ;

    try {
      // Write it
      writeFrame (frame) ;
    }
    catch (FecExceptionHandler &e) {
      // Multiple byte write access generate always an error (EOF = 0xa0??)
      // Display error
    }

//  if ((frame[2+sizet] & 0xF000) != 0xa000) {
//    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
//			         "Memory channel access error (write multiple bytes => possible error)",
//			         ERRORCODE,
//			         index,
//			         "FEC direct acknowledge", frame[2+sizet] ) ;
//      }
  }

  // Clear errors on FEC and CCU
  setFecRingCR1 ( FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS ) ;
}

/** Write a value to the memory specified by the key (multiple byte mode)
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param values - values to be written
 * \param size - size of the buffer values
 * \warning no exception will be managed due to the fact that multiple write returns always an error
 * \warning if the frame size of the memory size is greater than 127, the length word must be
 * separated in two words (with source, destination and length frame values):
 * <ul>
 * <li>For frame length < 128 => size
 * <li>For frame length > 127 and < 255 => 0x80 0xSIZE
 * <li>For frame length > 255 and < FIFO size => (0x80 | LENGTH > 255) 0xLENGTH
 * </ul>
 * \warning this method write a value in the memory for size greater than 127. The previous method
 * call automatically this method for size > 127 and if the maximum number of word in frame is > 127.
 */
void FecRingDevice::writeIntoMemory2BytesLength ( keyType index, 
                                                  tscType8 AH, tscType8 AL, 
                                                  tscType8 *values,
                                                  unsigned long size ) 
  throw (FecExceptionHandler) {

  // 127 means FEC_UPPERLIMIT_LENGTH
  // 119 means FEC_UPPERLIMIT_LENGTH - 5 - 3
  
  // Check the index
  basicCheckError (index) ;

  tscType16 sizeMaxFrame = DD_USER_MAX_MSG_LENGTH*4 ;

  if (sizeMaxFrame <= FEC_UPPERLIMIT_LENGTH || size <= (FEC_UPPERLIMIT_LENGTH-5-3)) {
    // 119 data => 3 words are reserved for the Dest / Src / Length
    //          => 5 words for the reset of the frame: channel / Transaction / Command / AH / AL

    writeIntoMemory (index, AH, AL, values, size) ;
  }

  // Build the frame and send it via the method write frame
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 0x80 | ((length & 0xFF00) >> 8)
  //  Length      = (length & 0xFF)
  //  Channel     = index
  //  Transaction = Assign by the device driver
  //  Command     = multiple byte write
  //  AH          = Position in the high memory
  //  AL          = Position in the low memory
  //  DW[size]    = values

  // Counter of values
  unsigned long counter = 0, downloaded = 0 ;
  tscType8 al = AL, ah = AH ;

  while (counter < size) {

    // Constitution de la frame
    tscType16 sizet = sizeMaxFrame < (size-counter+5+4) ? sizeMaxFrame : (size-counter+5+4) ;

    // If the sizet of frame does not need a 2 words for the length call the 1st method
    if ( (sizet) < FEC_UPPERLIMIT_LENGTH ) {

      writeIntoMemory ( index, AH, AL, &values[counter], size-counter ) ;
      counter = size ;
    }
    else {

      // Minus Src / Dest / LengthH /LengthL
      sizet -= 4 ;

      tscType8 sizetH = 0x80 | ((sizet & 0xFF00) >> 8) ;
      tscType8 sizetL = sizet & 0xFF ;
    
      tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                   sizetH, sizetL, getChannelKey(index), 
                                                   0x1, CMD_CHANNELMEMMULTIPLEBYTEWRITE,
                                                   AH, AL } ;

      // Insert in the frame the values 
      // constant 9 is Dest / Src / LengthH / LengthL / Channel / Transaction / Command / AH / AL
      memcpy (&frame[counter-downloaded+9], &values[counter], sizeof(tscType8) * (sizet-5)) ; counter += (sizet-5) ;
      
      // Already downloaded
      downloaded = counter ;
      AL = (al+ah*255+downloaded) & 0xFF ;
      AH = ((al+ah*255+downloaded) & 0xFF00) >> 8 ;

      // Write it
      try {
        writeFrame (frame) ;
      }
      catch (FecExceptionHandler &e) {
        // Multiple byte write access generate always an error (EOF = 0xa0??)
        // Display error
      }

//    if ((frame[3+sizet] & 0xF000) != 0xa000) {
//      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
//	  		           "Memory channel access error (write multiple bytes => possible error)",
//			           ERRORCODE,
//			           index,
//			           "FEC direct acknowledge", frame[2+sizet] ) ;
//        }
    }
  }

  // Clear errors on FEC and CCU
  setFecRingCR1 ( FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS ) ;
}

/** Read modify write a value from the memory specified by the key in single byte
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR)
 * \param mask - mask to be done during the read modify write
 */
void FecRingDevice::writeIntoMemory ( keyType index, 
                                      tscType8 AH, tscType8 AL, 
                                      logicalOperationType op, 
                                      tscType8 mask ) 
  throw (FecExceptionHandler) {

  // Set the mask register with the value
  setMemoryChannelMaskReg ( index, mask ) ;

  writeIntoMemory (index, AH, AL, op) ;
}
 
/** Read modify write a value from the memory specified by the key in single byte
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR)
 * \param mask - mask to be done during the read modify write
 * \return the value read in the memory
 * \exception FecExceptionHandler
 */
void FecRingDevice::writeIntoMemory ( keyType index, 
                                      tscType8 AH, tscType8 AL, 
                                      logicalOperationType op) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  // Operation
  tscType8 command ;

  switch (op) {
  case CMD_AND:
    command = CMD_CHANNELMEMSINGLEBYTERMW_AND ;
    break;
  case CMD_OR:
    command = CMD_CHANNELMEMSINGLEBYTERMW_OR  ;
    break;
  case CMD_XOR:
    command = CMD_CHANNELMEMSINGLEBYTERMW_XOR ;
    break ;
  case CMD_EQUAL:
  default:
    RAISEFECEXCEPTIONHANDLER ( TSCFEC_INVALIDOPERATION,
			       "Invalid RMW command",
			       ERRORCODE ) ;
  }

  // Build the frame and send it via the method write frame
  // Dest        = index, 
  // Src         = 0x0, 
  // Length      = 5, 
  // Channel     = index
  // Transaction = Assign by the device driver
  // Command     = command
  // AH          = Position in the high memory
  // AL          = Position in the low memory
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x05, getChannelKey(index), 
    0x00, command,
    AH, AL } ;

  // Write it
  writeFrame (frame) ;
}


/** Read a value from the memory specified by the key in single byte
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \return the value read in the memory
 * \exception FecExceptionHandler
 */
tscType8 FecRingDevice::readFromMemory ( keyType index, 
                                         tscType8 AH, tscType8 AL ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;
  
  // Build the frame and send it via the method write frame
  // Dest        = index, 
  // Src         = 0x0, 
  // Length      = 5, 
  // Channel     = index
  // Transaction = Assign by the device driver
  // Command     = single byte read from memory
  // AH          = Position in the high memory
  // AL          = Position in the low memory
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(index), FRAMEFECNUMBER, 
    0x05, getChannelKey(index), 
    0xFE, CMD_CHANNELMEMSINGLEBYTEREAD,
    AH, AL } ;

  // Write it
  writeFrame (frame) ;

  // Read back the value asked
  readFrame(frame[4], frame) ;

  // Check the frame
  if (frame[2] != 3) {
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
				       "Memory channel read error in single byte mode, cannot read back the value (bad size in frame should be 3)",
				       ERRORCODE,
				       buildFecRingKey(getFecSlot(),getRingSlot()),
				       "Frame size", frame[2]) ;
  }

  return (frame[5]) ;
}

/** Read values from the memory specified by the key in multiple byte
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param size - number of values to be read
 * \param values - pointer to an array of values
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecRingDevice::readFromMemory ( keyType index, 
                                     tscType8 AH, tscType8 AL,
                                     unsigned long size,
                                     tscType8 *values ) 
  throw (FecExceptionHandler) {

  // Check the index
  basicCheckError (index) ;

  if (size >= MAXMEMORYBYTESADDRESS) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_INVALIDOPERATION,
			       "Too many bytes to be written in the memory channel",
			       ERRORCODE,
			       index,
			       "Size", size) ;
  }

  tscType16 sizeMaxFrame = DD_USER_MAX_MSG_LENGTH ;

#ifdef BUGMEMORY2BYTES
  sizeMaxFrame -= 2 ; // Memory bus channel error, lenh-lenl has to be increased by 2
#endif

  // Counter of values
  unsigned long counter = 0 ;
  tscType8 al = AL, ah = AH ;

  // Build the frame and send it via the method write frame
  // Dest        = index, 
  // Src         = 0x0, 
  // Length      = 6 or 7
  // Channel     = index
  // Transaction = Assign by the device driver
  // Command     = multiple byte read from memory
  // AH          = Position in the high memory
  // AL          = Position in the low memory
  // LENGTH      = length (that can be put in two separated words)
  
  while (counter < size) { 

    // constant 5 or 6 => read back value
    //                 => 3, 4 for Src / Dest / Length
    //                 => 2 for command (in read)
    tscType8 val ;
    if (sizeMaxFrame > FEC_UPPERLIMIT_LENGTH) val = 6 ;  // 2 words for length
    else val = 5 ;

    // Size max is
    tscType8 length = (size-counter) > (unsigned long)(sizeMaxFrame-val) ? (sizeMaxFrame-val) : (size-counter) ;
    
#ifdef BUGMEMORY2BYTES
    length += 2 ; // Memory bus channel error, lenh-lenl has to be increased by 2
#endif

    // Determine the size and the length words
    tscType8 sizet ;
    tscType8 LENH, LENL ;
    if (length > FEC_UPPERLIMIT_LENGTH) {

      sizet = 7 ;
      LENL = length & 0xFF ;
      LENH = ((length & 0xFF00) >> 8) | FEC_LENGTH_2BYTES ;
    }
    else {

      sizet = 6 ;
      LENH = length ;
      LENL = 0 ;      // Not used
    }

    tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
      getCcuKey(index), FRAMEFECNUMBER, 
      sizet, getChannelKey(index), 
      0xFE, CMD_CHANNELMEMMULTIPLEBYTEREAD,
      AH, AL, LENH, LENL } ;
    
    // Write it
    writeFrame (frame) ;

    // Read back the values asked
    readFrame(frame[4], frame) ;

    // Error ?
    // if (lcl_err == DD_RETURN_OK) {

    // Check the size of the frame
    tscType8 deb = 5 ;
    tscType8 realSize = frame[2] ;
    if (realSize & FEC_LENGTH_2BYTES) {
        
      realSize = (frame[2] & 0x7F) << 8 | frame[3] ;
      deb += 1 ;
      // +1 for the two bytes length
    }

    // Copy the values the output buffer
    // realSize is Channel / Transaction / DATA
    memcpy (&values[counter], &frame[deb], (realSize-2)*sizeof(tscType8)) ;
    counter += (realSize - 2) ;
    
    // The end value have the EOF frame bit (0x8000)
    values[counter-1] &= ~DD_FIFOTRA_EOFRAME_MASK ;

    // New position in memory
    AL = (al+ah*255+counter) & 0xFF ;
    AH = ((al+ah*255+counter) & 0xFF00) >> 8 ;
  }
}

// -------------------------------------------------------------------------------------
//
//                                For the trigger channel
//              Author: Wojciech BIALAS
//
// -------------------------------------------------------------------------------------
/** Method in order to set trigger control register A in CCU
 * \param index - key of the corresponding channel
 * \param value - value to set
 * \no return  value
 */
void FecRingDevice::setTriggerChannelCRA ( keyType index, tscType8 value ) {

  // Check the access
  basicCheckError (index);

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELTRIGGERWRITECRA, value ) ;
}

/** Method in order to set trigger control register B in CCU
 * \param index - key of the corresponding channel
 * \param value - value to set
 * \no return  value
 */
void FecRingDevice::setTriggerChannelCRB ( keyType index, tscType8 value ) {

  // Check the access 
  basicCheckError (index);

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELTRIGGERWRITECRB, value ) ;
}

/** Method in order to retreive trigger control register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */
tscType8 FecRingDevice::getTriggerChannelCRA ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELTRIGGERREADCRA)) ;
}

/** Method in order to retreive trigger control register B from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */
tscType8 FecRingDevice::getTriggerChannelCRB ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELTRIGGERREADCRB)) ;
}

/** Method in order to retreive trigger status register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */
tscType8 FecRingDevice::getTriggerChannelSRA ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELTRIGGERREADSRA)) ;
}

/**
 */
tscType32 FecRingDevice::getTriggerChannelCounter ( keyType index, 
                                                    int cnt ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  ccuMapAccessedType::iterator ccu = ccuMapAccess_.find(getCcuKey(index)) ;
  
  if (! ccu->second->isCcu25()) {  
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_INVALIDOPERATION,
			       "Invalid command for old CCUs",
			       ERRORCODE,
			       buildFecRingKey(getFecSlot(),getRingSlot())) ;
  }
#endif
  
  // read the register and write the new one with the correct value
  tscType8 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                             3, TRIGGERCHANNELNUMBER, 
                                             0xFE, (tscType8)(CMD_CHANNELTRIGGERREADCNT0 + cnt) } ;

  // Write it
  writeFrame (frame) ;
 
  DD_TYPE_FEC_DATA32 CR = 0 ;
    
  // A read operation is performed
  readFrame(frame[4], frame) ;

  // The read value is 32 bits seperated in frame[5] to frame[8]

  // With the data read you find the status, eof, crc, addr seen, ...
  if (frame[2] != 6) {
    RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_REGISTERACCESS,
				       "Cannot read back a trigger counter register (bad size in frame, should be 6)",
				       ERRORCODE,
				       index,
				       "Frame size", frame[2] ) ;
  }
    
  // Get the value without the end of frame and status
  CR = ( frame[8] | (frame[7] << 8) | (frame[6] << 16) | (frame[5] << 24) ) ;
    
  return (CR) ;  
}



// -------------------------------------------------------------------------------------
//
//                                For the FEC/TTCRx
//
// -------------------------------------------------------------------------------------

/** Disable or enable the receive in FEC optical link
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 */
void FecRingDevice::setCR0Receive ( bool enable ) 
  throw (FecExceptionHandler) {

  // Enable the receive of clock for the optical link of the FEC
  tscType16 fecCR0Value = getFecRingCR0() ;

  if (enable) 
    setFecRingCR0 ( fecCR0Value & (~FEC_CR0_DISABLERECEIVE)) ;
  else
    setFecRingCR0 ( fecCR0Value | FEC_CR0_DISABLERECEIVE ) ;
}

/** initialise the TTCRx
 * \exception FecExceptionHandler
 */
void FecRingDevice::initTTCRx ( ) throw (FecExceptionHandler) {

  setFecRingCR0 ( FEC_CR0_RESETTCRX ) ;
  setFecRingCR0 ( FEC_CR0_ENABLEFEC ) ;
}

// -------------------------------------------------------------------------------------
//
//                                Block transfer in the FIFOs
//
// These methods are used to be compatible with the block transfer.
// If the block transfer capatibility exists on the FEC, you must re-implement these methods
// in the child class
//
// -------------------------------------------------------------------------------------

/** write a set of words intoto the FIFO receive
 * \param value - values to be written (array)
 * \param count - number of values
 */
void FecRingDevice::setFifoReceive ( tscType32 *value, int count) throw (FecExceptionHandler) {
  int i;
  for(i=0;i<count;i++) setFifoReceive(value[i]) ;
}

/** read a set of words from the FIFO receive
 * \param value - value to be read
 * \param count - number of value to be read
 * \return pointer to the array of value ( = value )
 */
tscType32* FecRingDevice::getFifoReceive (  tscType32 *value, int count ) throw (FecExceptionHandler) {
  int i;
  for(i=0;i<count;i++) value[i] = getFifoReceive() ;
  return value ;
}
  
/** write a set of words into the FIFO return
 * \param value - values to be written (array)
 * \param count - number of values
 */
void FecRingDevice::setFifoReturn ( tscType8 *value, int count ) throw (FecExceptionHandler) {
  int i;
  for(i=0;i<count;i++) setFifoReturn(value[i]) ;
}

/** read a set of words from the FIFO return
 * \param value - value to be read
 * \param count - number of value to be read
 * \return pointer to the array of value ( = value )
 */
tscType8* FecRingDevice::getFifoReturn (  tscType8 *value,int count ) throw (FecExceptionHandler) {
  int i;
  for(i=0;i<count;i++) value[i]  = getFifoReturn() ;
  return value ;
}
  
/** write a set of words into the FIFO Transmit
 * \param value - values to be written (array)
 * \param count - number of values
 */
void FecRingDevice::setFifoTransmit ( tscType32 *value, int count ) throw (FecExceptionHandler) {
  int i;
  for(i=0;i<count;i++) setFifoTransmit(value[i]) ;
}
  
/** read a set of words from the FIFO transmit
 * \param value - value to be read
 * \param count - number of value to be read
 * \return pointer to the array of value ( = value )
 */
tscType32* FecRingDevice::getFifoTransmit (  tscType32 *value,int count ) throw (FecExceptionHandler) {
  int i;
  for(i=0;i<count;i++) value[i]  = getFifoTransmit() ;
  return value ;
}

/* ************************************************************************************************************************ */
/* ************************************************************************************************************************ */

/** This method build an i2c frame to be put in the FIFO transmit
 * \param b - structure containing the access
 * \param toBeTransmited - output buffer
 * \param forceAcknowledge - is the force acknowledge has been set, not used in the method. The size always count the force acknowledge, in case of error a frame is sent back by the devices 
 * \param fifoRecWord - number of words that will be returned in the FIFO receive in case of errors (direct ack + force ack)
 * \param nWordFifo - number of words that will be returned in the FIFO receive with no errors (direct ack + if (forceAck) forceack)
 * \return the number of words written in the FIFO receive
 * \warning the address must be modified directly in the original address for the RAL mode read. This method does not handle it.
 */
unsigned int FecRingDevice::buildI2CFrame ( accessDeviceType b, tscType32 *toBeTransmited, bool forceAcknowledge, unsigned int *fifoRecWord, unsigned int *nWordFifo ) {

  // define the frame
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { getCcuKey(b.index), FRAMEFECNUMBER } ;
  unsigned int wordCount = 0 ;

  // fill it with the correct words
  switch (b.i2cType) {
  case RALMODE:
    if (b.accessType == MODE_WRITE) {
      frame[2] = 0x6 ;
      frame[3] = getChannelKey(b.index) ;
      frame[4] = b.tnum ;
      frame[5] = CMD_SINGLEBYTEWRITERALMODE ;
      frame[6] = getAddressKey(b.index) ;
      frame[7] = b.offset ;
      frame[8] = b.data ;

      toBeTransmited[0] = (frame[0] << 24) | (frame[1] << 16) | (frame[2] << 8) | frame[3] ;
      toBeTransmited[1] = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) | frame[7] ;
      toBeTransmited[2] = (frame[8] << 24) ;
      wordCount += 3 ;    // written in FIFO transmit
      *fifoRecWord = 3 ;  // direct acknowledge
      *fifoRecWord += 2 ; // force acknowledge
      *nWordFifo = 3 ; // total excepted without error
      if (forceAcknowledge) *nWordFifo += 2 ; // total excepted without error
    }
    else {
      frame[2] = 0x5 ;
      frame[3] = getChannelKey(b.index) ;
      frame[4] = b.tnum ;
      frame[5] = CMD_SINGLEBYTEREADRALMODE ;
      frame[6] = getAddressKey(b.index) ;
      frame[7] = b.offset ;

      toBeTransmited[0] = (frame[0] << 24) | (frame[1] << 16) | (frame[2] << 8) | frame[3] ;
      toBeTransmited[1] = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) | frame[7] ;
      wordCount += 2 ;    // written in FIFO transmit
      *fifoRecWord = 3 ;  // direct acknowledge
      *fifoRecWord += 2 ; // read answer
      *nWordFifo = 5 ; // total excepted without error
    }
    break ;
  case NORMALMODE:
    if (b.accessType == MODE_WRITE) {

      frame[2] = 0x5 ;
      frame[3] = getChannelKey(b.index) ;
      frame[4] = b.tnum ;
      frame[5] = CMD_SINGLEBYTEWRITENORMALMODE ;
      frame[6] = getAddressKey(b.index) | b.offset ;
      frame[7] = b.data ;

      toBeTransmited[0] = (frame[0] << 24) | (frame[1] << 16) | (frame[2] << 8) | frame[3] ;
      toBeTransmited[1] = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) | frame[7] ;
      wordCount += 2 ;    // written in FIFO transmit
      *fifoRecWord = 3 ;  // direct acknowledge
      *fifoRecWord += 2 ; // force acknowledge

      *nWordFifo = 3 ; // total excepted without error
      if (forceAcknowledge) *nWordFifo += 2 ; // total excepted without error with fack
    }
    else {

      frame[2] = 0x4 ;
      frame[3] = getChannelKey(b.index) ;
      frame[4] = b.tnum ;
      frame[5] = CMD_SINGLEBYTEREADNORMALMODE ;
      frame[6] = getAddressKey(b.index) | b.offset ;

      toBeTransmited[0] = (frame[0] << 24) | (frame[1] << 16) | (frame[2] << 8) | frame[3] ;
      toBeTransmited[1] = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) ;
      wordCount += 2 ;    // written in FIFO transmit
      *fifoRecWord = 2 ;  // direct acknowledge
      *fifoRecWord += 2 ; // read answer
      *nWordFifo = 4 ; // total excepted without error
    }
    break ;
  case EXTENDEDMODE:
    if (b.accessType == MODE_WRITE) {

      frame[2] = 0x6 ;
      frame[3] = getChannelKey(b.index) ;
      frame[4] = b.tnum ;
      frame[5] = CMD_SINGLEBYTEWRITERALMODE ;
      frame[6] = getAddressKey(b.index) ;
      frame[7] = b.offset ;
      frame[8] = b.data ;

      toBeTransmited[0] = (frame[0] << 24) | (frame[1] << 16) | (frame[2] << 8) | frame[3] ;
      toBeTransmited[1] = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) | frame[7] ;
      toBeTransmited[2] = (frame[8] << 24) ;
      wordCount += 3 ;    // written in FIFO transmit
      *fifoRecWord = 3 ;  // direct acknowledge
      *fifoRecWord += 2 ; // force acknowledge

      *nWordFifo = 3 ;    // total excepted without error
      if (forceAcknowledge) *nWordFifo += 2 ; // total excepted without error with fack
    }
    else {

      frame[2] = 0x5 ;
      frame[3] = getChannelKey(b.index) ;
      frame[4] = b.tnum ;
      frame[5] = CMD_SINGLEBYTEREADRALMODE ;
      frame[6] = getAddressKey(b.index) ;
      frame[7] = b.offset ;

      toBeTransmited[0] = (frame[0] << 24) | (frame[1] << 16) | (frame[2] << 8) | frame[3] ;
      toBeTransmited[1] = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) | frame[7] ;
      wordCount += 2 ;    // written in FIFO transmit
      *fifoRecWord = 3 ;  // direct acknowledge
      *fifoRecWord += 2 ; // read answer
      *nWordFifo = 5 ;    // total excepted without error
    }    
    break ;
  }

#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES
  std::cout << __func__ << " ( { " ;
  for (int i = 0 ; i < (frame[2]+3) ; i ++ )
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  std::cout << "} )" << std::dec << std::endl ;
  //std::cout << "Force ack = " << forceAcknowledge << std::endl ;
  //std::cout << "Wait for " << *nWordFifo << std::endl ;
#endif

  return (wordCount) ;
}

/** This build a PIA reset frame to be put in the FIFO transmit
 * \param b - structure containing the access
 * \param toBeTransmited - output buffer
 * \param fifoRecWord - number of words that will be returned in the FIFO receive
 * \return the number of words written in the FIFO receive
 */
unsigned int FecRingDevice::buildPIAFrame ( accessDeviceType b, tscType32 *toBeTransmited, unsigned int *fifoRecWord ) {

  // define the frame
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { getCcuKey(b.index), FRAMEFECNUMBER } ;
  unsigned int wordCount = 0 ;

  if (b.accessType == MODE_WRITE) {

    frame[2] = 0x4 ;
    frame[3] = getChannelKey(b.index) ;
    frame[4] = b.tnum ;
    frame[5] = CMD_CHANNELPIAWRITEDATAREG ;
    frame[6] = b.data ;

    toBeTransmited[0] = (frame[0] << 24) | (frame[1] << 16) | (frame[2] << 8) | frame[3] ;
    toBeTransmited[1] = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) ;
    wordCount = 2 ;
    *fifoRecWord = 2 ;
  }
  else {

    frame[2] = 0x3 ;
    frame[3] = getChannelKey(b.index) ;
    frame[4] = b.tnum ;
    frame[5] = CMD_CHANNELPIAREADDATAREG ;

    toBeTransmited[0] = (frame[0] << 24) | (frame[1] << 16) | (frame[2] << 8) | frame[3] ;
    toBeTransmited[1] = (frame[4] << 24) | (frame[5] << 16) ;
    wordCount = 2 ;
    *fifoRecWord = 2 ;
  }

#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES
  std::cout << __func__ << " ( { " ;
  for (int i = 0 ; i < (frame[2]+3) ; i ++ )
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  std::cout << "} )" << std::dec << std::endl ;
#endif

  return (wordCount) ;
}


/** This methods takes an array of values to be set and send it over the ring. It collects afterwards the direct acknoledge and return an array of exceptions or null or no problems arrives. Please note that the array of exceptions must be deleted by the remote user of the method. This methods make also a sort in the array depending of the CCU/channel to be accessed.
 * \warning All the accessDeviceType must be initialise with dAck = 0 and sent = false
 * \warning if this method encounters a register problem, the rest of the registers will be sent and the same exception (pointer point of view) is set in all the request to that device. So do not delete directly all the exception from the list returned
 */
void FecRingDevice::setBlockDevices ( std::list<accessDeviceType> &vAccessDevices, bool forceAcknowledge ) throw (FecExceptionHandler) {

  if (firmwareVersion_ >= MINFIRMWAREVERSION) {
    setBlockDevicesBltMode(vAccessDevices, forceAcknowledge) ;
    return ;
  }

  //#define DEBUGMSGERRORMF

  // number of loops before the answer to a frame
  unsigned int loopInTimeWriteFrame = TIMEOUT ;
  unsigned int loopInTimeDirectAck  = TIMEOUT ;
  unsigned int loopInTimeReadFrame  = TIMEOUTFORCEACK ;
    
#ifdef DEBUGMSGERRORMF
  tscType16 fecSR0D = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice(" << std::dec << (int)getFecSlot() << "." << (int)getRingSlot() << ")::setBlockDevices: begin and the SR0 is " << std::hex << fecSR0D << std::endl ;
#endif

  // Check the status register 0 of the FEC
  checkRing() ;

  // -----------------------------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------------------------

  tscType32 fecSR0 ;

  bool finished            = false ;          // true if all operation read/write has been done or if a timeout occurs
  unsigned int wordFifo    = 0     ;          // number of allowed in 32 bits in the FIFO transmit
  unsigned int fifoRecWord = 0     ;          // number of words expected in the FIFO receive with errors
  unsigned int nWordFifo   = 0     ;          // number of words expected in the FIFO receive with no errors
  accessTransactionFrameMap tnumSent ;        // frames already sent
  unsigned int cptRead    = 0      ;          // number of read request
  unsigned int countFrame = 0      ;          // number of frame sent
  unsigned int itemSent   = 0      ;          // number of frame sent for the current block of frames
  unsigned short tnum     = 1      ;          // current transaction number
  std::list<unsigned short> tnumUsed    ;          // store all transaction number used
  bool noMoreTransaction = false   ;          // Anymore transaction to be sent ?
  int  timeoutReadFrame  = 0       ;          // Timeout on read request
  Sgi::hash_map<keyType, unsigned long> busy    ;  // busy FEC/Ring/CCU/channel
  tscType32 toBeTransmited[MAXFECFIFOWORD] ;  // size of the frame
  tscType32 frameReceived[MAXFECFIFOWORD]  ;  // frame to be received
  std::list<accessDeviceType>::iterator itAccessDevice ; // next frame to be sent
  Sgi::hash_map<keyType, FecExceptionHandler *> errorDevice ; // in order to avoid multiple send of frames to a faulty device

  // Check if the channels are PIA or I2C
  itAccessDevice = vAccessDevices.begin() ;

  // No busy channel
  for (std::list<accessDeviceType>::iterator itAccessDevice = vAccessDevices.begin() ; itAccessDevice != vAccessDevices.end() ; itAccessDevice ++) {
    busy[getFecRingCcuChannelKey(itAccessDevice->index)] = 0 ;
    errorDevice[itAccessDevice->index] = NULL ;

#ifdef DEBUGMSGERRORMF
    char msg[80] ;
    decodeKey(msg, itAccessDevice->index) ;
    std::cerr << "Device " << msg << std::endl ;
    std::cerr << "Channel busy since " << std::dec << busy[getFecRingCcuChannelKey(itAccessDevice->index)] << " (timestamp " << time(NULL) << ")" << std::endl ;
#endif

#ifdef S2NDLEVELCHECK
    if (! isi2cChannelCcu25(itAccessDevice->index) &&
	! isPiaChannelCcu25(itAccessDevice->index)) {
      std::cerr << "*************************************** ERROR ****************************************" << std::endl ;
      std::cerr << __func__ << ": this method is forseen only for PIA or I2C channels" << std::endl ;
      std::cerr << "**************************************************************************************" << std::endl ;

      RAISEFECEXCEPTIONHANDLER ( TSCFEC_INVALIDOPERATION,
				 "This method is forseen only for PIA or I2C channels",
				 ERRORCODE) ;
    }
#endif
  }

  // -------------------------------------------- starts
  // Until the end
  while (!finished) {

    /* **************************************************************************************************************************** */ 
    /*                                           Select the frames to be sent over each channel                                     */
    /*                                           Build the frame (see method buildPiaFrame and buildI2CFrame)                       */
    /*                                           Build a buffer with all the frames to sent (depending of the size of the FIFOs)    */
    /* **************************************************************************************************************************** */ 
    // is an item has been sent
    itemSent = 0 ;
    bool itemLoop = false ;
    fifoRecWord = wordFifo = nWordFifo = 0 ;
    tnum = 1 ;

    // The -10 is given for the last transaction if the answer or the force ack. is not arrived
    while (!noMoreTransaction && !itemLoop && (fifoRecWord < receiveFifoDepth_) && (tnum != 0)) {

//       keyType index2 = buildCompleteKey(fecSlot_, ringSlot_, 0x2, 0, 0) ;
//       std::cout << "CCU 0x2 CRE = 0x" << std::hex << getCcuCRE(index2) << std::endl ;
//       if (getCcuCRE(index2) == 0x0) std::cout << "------------------------------------- CCU CRE reseted" << std::endl ;

      // Find the next transaction to be sent
      itAccessDevice = vAccessDevices.begin() ;
      bool occ = true ;
      noMoreTransaction = true ;
      while ( (itAccessDevice != vAccessDevices.end()) && occ) {

	if (!itAccessDevice->sent) {

	  // if the device has already an error
	  if (errorDevice.find(itAccessDevice->index) != errorDevice.end()) {

	    itAccessDevice->sent = true ;
	    itAccessDevice->e = errorDevice[itAccessDevice->index] ;
	    countFrame ++ ;

#ifdef DEBUGMSGERRORMF
	    char msg[80] ;
	    decodeKey(msg, itAccessDevice->index) ;
	    std::cerr << "The device " << msg << " has already an error, no frames are sent anymore to it" << std::endl ;
#endif
	  }
	  else { // No error on this device
#ifdef DEBUGMSGERRORMF
	    char msg[80] ;
	    decodeKey(msg, itAccessDevice->index) ;
	    std::cerr << "Device " << msg << std::endl ;
	    std::cerr << "Channel busy since " << std::dec << busy[getFecRingCcuChannelKey(itAccessDevice->index)] << " (timestamp " << time(NULL) << ")" << std::endl ;
#endif

	    noMoreTransaction = false ;
	    if (!busy[getFecRingCcuChannelKey(itAccessDevice->index)]) {

#ifdef DEBUGMSGERRORMF
	      std::cout << "And channel not busy" << std::endl ;
#endif
	      
	      occ = false ;
	    }
	    else {
	      
	      // check the timestamp to see if the frame has been sent since a long time
	      if ((time(NULL) - busy[getFecRingCcuChannelKey(itAccessDevice->index)]) > 2) { // 2 sec
		
		if (itAccessDevice->dAck == 0) {  // timeout on the force acknowledge
		  
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
		  char msg[80] ;
		  decodeKey(msg, itAccessDevice->index) ;
		  std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
		  std::cerr << "Device " << msg << std::endl ;
#endif
		  
		  itAccessDevice->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( DD_CANNOT_READ_DATA,
							       "Unable to read a frame or the direct acknowledge, timeout reached",
							       ERRORCODE,
							       itAccessDevice->index ) ;

#ifdef DEBUGMSGERRORMF
		  std::cerr << itAccessDevice->e->what() << std::endl ;
#endif
		}
		else { // timeout on the read frame or the force acknowledge
		  
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
		  char msg[80] ;
		  decodeKey(msg, itAccessDevice->index) ;
		  std::cerr << __func__ << ": Cannot read the frame, timeout has been reached for transaction " << std::dec << (int)itAccessDevice->tnum << std::endl ;
		  std::cerr << "Device = " << msg << std::endl ;
#endif
		
		  itAccessDevice->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( DD_CANNOT_READ_DATA,
									    "Unable to read a frame or the force acknowledge, timeout reached",
									    ERRORCODE,
									    itAccessDevice->index ) ;

#ifdef DEBUGMSGERRORMF
		  std::cerr << itAccessDevice->e->what() << std::endl ;
#endif
		}
	      
		// busy channel is now release on a timeout
		busy[getFecRingCcuChannelKey(itAccessDevice->index)] = 0 ;
		occ = false ;
	      }
	      // next transaction
	      else itAccessDevice ++ ;
	    }
	  }
	}
	else itAccessDevice ++ ;
      }

      if (itAccessDevice == vAccessDevices.end()) itemLoop = true ;
      else {

	// Next transaction number, 0 if no more transaction number exists
	tnum = getNextTransactionNumber ( ) ;

#ifdef DEBUGMSGERRORMF
	// display
	char msg[80] ;
	decodeKey(msg,itAccessDevice->index) ;
	std::cout << "Index = " << msg << std::endl ;
	std::cout << "Channel busy = " << busy[getFecRingCcuChannelKey(itAccessDevice->index)] << std::endl ;
	std::cout << "Sent = " << itAccessDevice->sent << std::endl ;
	std::cout << "Transaction number used = " << tnum << std::endl ;
#endif

	// if a transaction number exists
	if (tnum != 0) {
	  
          // Transaction number
          itAccessDevice->tnum = tnum ;
	  tnumUsed.push_back(tnum) ;
	  
          // Write the i2c frame in the FIFO tra. (nothing else) and return the number of word written
	  unsigned int fifoTraRecWord = 0,  nWordTemp = 0 ;
	  if (isi2cChannelCcu25(itAccessDevice->index)) {
	    wordFifo += buildI2CFrame ( *itAccessDevice, &toBeTransmited[wordFifo], forceAcknowledge, &fifoTraRecWord, &nWordTemp ) ;
	    nWordFifo += nWordTemp ;
	    // Counter for read request
	    if ( (itAccessDevice->accessType == MODE_READ) || (forceAcknowledge)) cptRead ++ ;
	  }	    
	  else if (isPiaChannelCcu25(itAccessDevice->index)) {
	    wordFifo += buildPIAFrame ( *itAccessDevice, &toBeTransmited[wordFifo], &fifoTraRecWord ) ;
	    // Counter for read request
	    if (itAccessDevice->accessType == MODE_READ) cptRead ++ ;
	  }
	      
	  fifoRecWord += fifoTraRecWord ;
	  
#ifdef DEBUGMSGERRORMF
	  char msg[80] ;
	  decodeKey(msg,itAccessDevice->index) ;
	  std::cout << "Add a transaction " << (int)tnum << "(0x" << std::hex << tnum << ") on index " << msg << std::endl ;
#endif
	  
          // This frame wait for its DACK
          tnumSent[tnum] = &(*itAccessDevice) ;
          itAccessDevice->sent = true ;  // sent

          // Channel is busy
          busy[getFecRingCcuChannelKey(itAccessDevice->index)] = time(NULL) ;
	  
	  // an item has been sent
	  itemSent ++ ;
	}
	else std::cout << __func__ << ": No transaction available" << std::endl ;
      }

#ifdef DEBUGMSGERRORMF
      std::cout << "while (!noMoreTransaction && !itemLoop && (fifoRecWord < " << receiveFifoDepth_ << " && (tnum != 0)) {" << std::endl ;
      std::cout << "noMoreTransaction = " << std::dec << noMoreTransaction << std::endl ;
      std::cout << "itemLoop = " << std::dec << itemLoop << std::endl ;
      std::cout << "Words = " << std::dec << fifoRecWord << "/" << receiveFifoDepth_ << std::endl ;
      std::cout << "Transaction number = " << std::dec << (int)tnum << std::endl ;
      std::cout << "Cond = " << (!noMoreTransaction && !itemLoop && (fifoRecWord < receiveFifoDepth_) && (tnum != 0)) << std::endl ;
#endif
    }

    /* **************************************************************************************************************************** */ 
    /*                                           Fill the FIFO transmit                                                             */
    /*                                           clear the errors and send the frames over to the ring                              */
    /*                                           wait until the FIFO transmit are sent                                              */
    /*                                           and wait for the pending IRQ                                                       */
    /* **************************************************************************************************************************** */ 
    if (itemSent) {

      // Fill the FIFO transmit
      setFifoTransmit(toBeTransmited,wordFifo); 

#ifdef DEBUGMSGERRORMF
      for (int frameS = 0 ; frameS < wordFifo ; frameS ++) {
	std::cout << std::hex << toBeTransmited[frameS] << std::endl ;
      }
#endif

      // Clear the error bit
      setFecRingCR1 ( DD_FEC_CLEAR_ERRORS ) ;
      // Toggle the send bit of CTRLO with RMW operation
      setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_OR ) ;
      setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_XOR ) ;

#ifdef DEBUGMSGERRORMF
      std::cout << "Send " << itemSent << " frames over the ring" << std::endl ;
#endif

      // Wait for the FIFO transmit is not running
      unsigned long watchdog = 0;
      fecSR0 = getFecRingSR0() ;
      while ( ((fecSR0 & FEC_SR0_TRARUN) || (!(fecSR0 & FEC_SR0_TRAEMPTY))) && (watchdog++ < loopInTimeWriteFrame)  && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
	nanosleep (&req,NULL) ;
	//usleep (TIMETOWAIT) ;
#endif
	fecSR0 = getFecRingSR0() ;
      }

#ifdef RECALCULATETIMEOUT
      // --------------------------------------------------------------------------------------
      // Update the timeout
      if ( (loopInTimeWriteFrame == TIMEOUT) && (watchdog < TIMEOUT) ) {
	if (!watchdog) watchdog = 1000 ;
	loopInTimeWriteFrame = watchdog*2 ;
	
#  ifdef DEBUGMSGERROR
	std::cout << "loopInTimeWriteFrame local to setBlockDevices = " << std::dec << loopInTimeWriteFrame << std::endl ;
#  endif
      }
#endif
            
      // Wait for the end of the send of the frame (FIFO transmit running)
      fecSR0 = getFecRingSR0() ;
      if ( (fecSR0 & FEC_SR0_TRARUN) || !(fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
	
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
	std::cerr << __func__ << ": FIFO transmit running bit is always ON (SR0 = " << std::hex << fecSR0 << ")" << std::dec << std::endl ;
#endif    
	
	// Release all the transaction number used
	for (std::list<unsigned short>::iterator it = tnumUsed.begin() ; it != tnumUsed.end() ; it ++) {
	  releaseTransactionNumber(*it) ;
	}
	
#ifdef DEBUGMSGERRORMF
	for (unsigned int i = 0 ; i < wordFifo ; i ++)
	  std::cerr << "Word (" << i << ") = 0x" << std::hex << toBeTransmited[i] << std::endl ;
#endif

	if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
	  RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
					     "Fails on sending a frame, ring lost",
					     CRITICALERRORCODE,
					     buildFecRingKey(getFecSlot(), getRingSlot()),
					     "FEC status register 0", fecSR0 ) ;
	}
	else {
	  RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON,
					     "FIFO transmit running bit is always on (CR0[3])",
					     CRITICALERRORCODE,
					     buildFecRingKey(getFecSlot(), getRingSlot()),
					     "FEC status register 0", fecSR0 ) ;
	}
      }

      // Wait for direct ack delay
      // now, wait for the direct ack for:
      //   pending IRQ (direct ack receive) but some frames can arrives also at the same time so
      //   wait also on the FIFO FEC RUN
      watchdog = 0 ;
      fecSR0 = getFecRingSR0() ;

      while ( ((fecSR0 & FEC_SR0_RECRUN) || !(fecSR0 & FEC_SR0_PENDINGIRQ)) && (watchdog++ < loopInTimeDirectAck) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
	nanosleep (&req,NULL) ;
	//usleep (TIMETOWAIT) ;
#endif
	fecSR0 = getFecRingSR0() ;
      }

#ifdef RECALCULATETIMEOUT
      // --------------------------------------------------------------------------------------
      // Update the timeout
      if ( (loopInTimeDirectAck == TIMEOUT) && (watchdog < TIMEOUT) ) {
	if (!watchdog) watchdog = 1000 ;
	loopInTimeDirectAck = watchdog*2 ;
	
#  ifdef DEBUGMSGERROR
	std::cout << "loopInTimeDirectAck local to setBlockDevices = " << std::dec << loopInTimeDirectAck << std::endl ;
#  endif
      }
#endif

      // No anwer
      fecSR0 = getFecRingSR0() ;
      if ( (fecSR0 & FEC_SR0_RECEMPTY) || !(fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
      
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
	std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
#endif

	// The frames are sent again
	for (accessTransactionFrameMap::iterator p=tnumSent.begin() ; p != tnumSent.end() ; p ++) {
	  
	  if (p->second != NULL) {

	    // Is the was already sent ?
	    if (p->second->dAck == 0) // No then re-send it
	      p->second->sent = false ; // not sent
	    else {
	      // permanent error
	      FecExceptionHandler *e ;
	      if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
		e = NEWFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
						     "Fails on sending a frame, ring lost",
						     CRITICALERRORCODE,
						     buildFecRingKey(getFecSlot(), getRingSlot()),
						     "FEC status register 0", fecSR0 ) ;
	      }
	      else {
		e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( DD_WRITE_OPERATION_FAILED,
							  "Timeout reached on the direct acknowledge",
							  ERRORCODE,
							  p->second->index ) ;
	      }

	      p->second->e = e ;

#ifdef DEBUGMSGERRORMF
	      std::cerr << p->second->e->what() << std::endl ;
#endif
	    }
	    // Channel is free to sent a frame
	    busy[getFecRingCcuChannelKey(tnumSent[tnum]->index)] = 0 ;
	    
	    // release the hash_map, transaction number and busy channel
	    tnumSent.erase(tnum) ;
	    releaseTransactionNumber(tnum) ;
	  }
	}	
      }
      
#ifdef DEBUGMSGERRORMF
      std::cout << "Wait for the direct ack. done and ok: SR0 = " << std::hex << fecSR0 << std::endl ;
#endif
    }
    
    /* **************************************************************************************************************************** */ 
    /*                                                                                                                              */
    /*                                           Read the FIFO receive to analyse the frames received                               */
    /*                                                                                                                              */
    /* **************************************************************************************************************************** */ 
    // if frame are in the buffer then read it
    timeoutReadFrame            = 0 ;
    unsigned int countFrameTemp = 0 ;
    while (!((fecSR0=getFecRingSR0()) & FEC_SR0_RECEMPTY) || 
	   (((countFrameTemp != tnumSent.size()) || (cptRead != 0)) && (timeoutReadFrame < 2) ) ) {

#ifdef DEBUGMSGERRORMF
      std::cout << "Begin = Size in tnum = " << std::dec << tnumSent.size() << std::endl ;
#endif

      // --------------------------------------------------------------------
      // Check if values exist in the FIFO receive
      if (! (fecSR0 & FEC_SR0_RECEMPTY)) {

	// read the three first word in the FIFO receive to have the size => the words are 32 bits => must be converted to 8 bits word length
	// Retreive the size
	frameReceived[0] = getFifoReceive() ;

#ifdef DEBUGMSGERRORMF
	std::cerr << "Frame[0] = 0x" << std::hex << frameReceived[0] << std::endl ;
#endif      

	// Check the size of the frame
	int realSize = (frameReceived[0] >> 8) & 0xFF ;
	tscType8 word2 = frameReceived[0]  & 0xFF ;
	if (realSize & FEC_LENGTH_2BYTES) {
	
	  realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
	  // +1 for the two byte words for length
	}

	// +3 => Dst, Src, Length  ; +1 => status after the frame
	int realSize32 = (realSize+3+1)/4 ;
	if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;

	/* **************************************************************************************************************************** */ 
	/*                                           Check the size of the frames                                                       */
	/*                                           and read the frame if it is correct                                                */
	/*                                           In case of size problem, the frames are sent again                                 */
	/* **************************************************************************************************************************** */ 
	tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = {0} ; // frame received
	// if the size is incoherent, greater than the FIFO size then nothing is read and the frame is not read from the FIFO
	// the FIFO is emptyied. the transaction is set to 0 to raise an error.
	if (realSize32 > MAXFECFIFOWORD) {

	  std::cerr << __func__ << ": the frame received has an incorrect size (size = " << std::dec << (int)((frameReceived[0] >> 8) & 0xFF) << ")" << std::endl ;

	  realSize32 = 0 ; // nothing is read
	  tnum = 0 ; // => to have an error
	  // emptyied the FIFO
	  checkFifoReceive ( ) ;

	  // The frames are sent again
	  for (accessTransactionFrameMap::iterator p=tnumSent.begin() ; p != tnumSent.end() ; p ++) {
	  
	    if (p->second != NULL) {

	      // Was the frame already sent ?
	      if (p->second->dAck == 0) // No then re-send it
		p->second->sent = false ; // not sent
	      else {
		// permanent error
		p->second->e = NEWFECEXCEPTIONHANDLER_INFOSUP ( DD_TOO_LONG_FRAME_LENGTH,
								"Size in frame is bigger than the FIFO size",
								CRITICALERRORCODE,
								buildFecRingKey(getFecSlot(), getRingSlot()),
								"Size of the frame received", realSize32 ) ;
#ifdef DEBUGMSGERRORMF
		std::cerr << p->second->e->what() << std::endl ;
#endif
	      }
	    }
	  }
	}
	else {
	
	  // Read back the frame
	  getFifoReceive(frameReceived+1,realSize32-1);
	
	  // Convert it into a frame of 8 bits / The status is just after the frame
	  unsigned int cpt = 0 ;
	  for (int i = 0 ; i < realSize32 ; i ++) {
	  
#ifdef DEBUGMSGERRORMF
	    std::cout << "frameReceived[" << i << "] = " << std::hex << frameReceived[i] << std::endl ;
#endif
	  
	    frame[cpt++] = (frameReceived[i] >> 24) & 0xFF ;
	    frame[cpt++] = (frameReceived[i] >> 16) & 0xFF ;
	    frame[cpt++] = (frameReceived[i] >>  8) & 0xFF ;
	    frame[cpt++] = (frameReceived[i])       & 0xFF ;
	  }
	
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
	  std::cout << __func__ << ": Frame received ( { " ;
	  for (int i = 0 ; i < ((frame[2]+3)+1) ; i ++ )
	    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
	  std::cout << "} )" << std::dec << std::endl ;
#endif

	  // Check the size that must be or 2 or 3 (in 32 bits words)
	  if ((realSize32 == 3) || (realSize32 == 2)) {
	    // Find the transaction number, check only for transaction number
	    //keyType index = buildCompleteKey(getFecSlot(), getRingSlot(), frame[0], frame[3], frame[5] & 0xF0) ;
	    tnum = frame [4] ;
	  } 
	  else tnum = 0 ; // 0 to have an error
	
#ifdef DEBUGMSGERRORMF
	  std::cout << "Transaction number = " << (int)frame[4] << "/" << tnum << " (0x" << std::hex << tnum << ")" << std::endl ;
	  std::cout << "tnumSent[tnum] = " << std::hex << tnumSent[tnum] << std::endl ;
#endif
	}

	/* **************************************************************************************************************************** */ 
	/*                                           Frame treatement                                                                   */
	/*                                           Check the transaction number                                                       */
	/*                                           And check if it is a direct ack. or force ack. or read answer                      */
	/* **************************************************************************************************************************** */ 	
	// Check if a frame with this transaction number has been sent and check if the index received is the same than the index expected (sent)
	keyType checkIndex = 0 ;
	if (frame[0] != FRAMEFECNUMBER)
	  checkIndex = buildCompleteKey(getFecSlot(),getRingSlot(),frame[0],frame[2] & FEC_LENGTH_2BYTES ? frame[4] : frame[3], 0) ;
	else
	  checkIndex = buildCompleteKey(getFecSlot(),getRingSlot(),frame[1],frame[2] & FEC_LENGTH_2BYTES ? frame[4] : frame[3], 0) ;

	if ((tnumSent.find(tnum) != tnumSent.end()) && (checkIndex == getFecRingCcuChannelKey(tnumSent[tnum]->index))) {

#ifdef DEBUGMSGERRORMF
	  char msg[80] ;
	  decodeKey(msg,tnumSent[tnum]->index) ;
	  std::cout << "Transaction " << (int)tnum << " (0x" << std::hex << tnum << ") on index " << msg << std::endl ;
#endif
	
	  // if the direct ack is not arrived (so dest != FEC) and dAck must be zero
	  //if ( (tnumSent[tnum]->dAck == 0) || (frame[0] != FRAMEFECNUMBER) ) {
	  if (frame[0] != FRAMEFECNUMBER) {

#ifdef DEBUGMSGERRORMF
	    std::cout << "Direct Acknowledge = 0x" << std::hex << (int)frame[frame[2] + 3] << std::endl ;
#endif

	    // Analyse the frame
	    if (frame[frame[2] + 3] != FECACKNOERROR32) { // bad direct acknowledge
	    
#ifdef DEBUGMSGERRORMF
	      std::cout << "Bad Direct Acknowledge = 0x" << std::hex << (int)frame[frame[2] + 3] << std::endl ;
#endif
	    
// 	      // if the direct acknowledge is 0xA0, due to timing, the channel can be busy so a retry is done
// 	      if (tnumSent[tnum]->dAck == 0) {
// #ifdef DEBUGMSGERRORMF
// 		char msga[80] ;
// 		decodeKey(msga,tnumSent[tnum]->index) ;
// 		std::cout << "A retry is done for index " << msga << std::endl ;
// #endif
// 		tnumSent[tnum]->sent = false ;
// 		tnumSent[tnum]->dAck = 1 ;     // count number, next time an error is raised

// 		// decrement the number of frames sent
// 		countFrame -- ;
// 	      }
// 	      else 
	      {
		// Frame not ok => error
		tnumSent[tnum]->e = NEWFECEXCEPTIONHANDLER_INFOSUP ( DD_DATA_CORRUPT_ON_WRITE,
								     "Bad status in read frame",
								     ERRORCODE,
								     tnumSent[tnum]->index,
								     "Status of read frame", frame[frame[2] +3] ) ;

#ifdef DEBUGMSGERRORMF
		std::cerr << tnumSent[tnum]->e->what() << std::endl ;
#endif
	      }

	      // decrement the counter of sent
	      if ( ((tnumSent[tnum]->accessType == MODE_WRITE) && (forceAcknowledge)) ||
		   (tnumSent[tnum]->accessType == MODE_READ) ) cptRead -- ;
	    }

	    // one frame sent
	    countFrame ++ ;
	    countFrameTemp ++ ;

	    // store the DACK in the frame
	    tnumSent[tnum]->dAck = frame[frame[2] + 3] ;	 

	    // If no force and no read then free the transaction number and busy channel
	    if ( (frame[frame[2]+3] != FECACKNOERROR32) ||
		 ((tnumSent[tnum]->accessType == MODE_WRITE) && (!forceAcknowledge)) ) {
	    
#ifdef DEBUGMSGERRORMF
	      char msgr[80] ;
	      decodeKey(msgr,tnumSent[tnum]->index) ;
	      std::cout << "Free the transaction number " << std::dec << tnum << " and busy channel for index " << msgr << std::endl ;
#endif   
	    }
	  }
	  else {  // the DACK is already arrive so it is a read request
	  
	    // Read answer depend of what was the request: if it is not a read answer or it is a force acknowledge or it is an error related to a device
	    if (tnumSent[tnum]->accessType == MODE_WRITE) {
	      //if ((tnumSent[tnum]->accessType == MODE_WRITE) && (forceAcknowledge)) {

#ifdef DEBUGMSGERRORMF
	      std::cout << "Force acknowledge = 0x" << std::hex << (int)frame[5] << std::endl ;
#endif  
	    
	      // Read answer is a force acknoledge
	      // Whatever the request was, the answer is always DST SRC LEN CHAN TRA ACK
	      tnumSent[tnum]->fAck = frame[5] ;
	    
	      // Check the frame and the SRA in the i2c interface CCU 25 page 29 
	      if ( (frame[2] != 3) || (frame[5] & CCUFACKERROR) ) {
	      
		// Error in size
		if (frame[2] != 3) {
		  tnumSent[tnum]->e = NEWFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_PROBLEMINFRAME,
								       "The size expected is different than the size received (bad size in frame, should be 3)",
								       ERRORCODE,
								       tnumSent[tnum]->index,
								       "Frame size", frame[2] ) ;

#ifdef DEBUGMSGERRORMF
		  std::cerr << tnumSent[tnum]->e->what() << std::endl ;
#endif
		}
		else {
		  // Error in status
		  tnumSent[tnum]->e = NEWFECEXCEPTIONHANDLER_INFOSUP ( DD_DATA_CORRUPT_ON_WRITE,
								       "Write mode, the force acknowledge is not correct",
								       ERRORCODE,
								       tnumSent[tnum]->index,
								       "Status of read frame", frame[5] ) ;

#ifdef DEBUGMSGERRORMF
		  std::cerr << tnumSent[tnum]->e->what() << std::endl ;
#endif

		  // Add an error on the status:
		  if (errorDevice.find(tnumSent[tnum]->index) == errorDevice.end()) errorDevice[tnumSent[tnum]->index] = tnumSent[tnum]->e ;
		}
	      }
#ifdef DEBUGMSGERRORMF
	      else std::cout << "Force acknowledge correct" << std::endl ;
#endif
	    }
	    else { // Read answer

#ifdef DEBUGMSGERRORMF
	      std::cout << "Read answer = 0x" << std::hex << (int)frame[5] << " (fack = 0x" << (int)frame[6] << ")" << std::endl ;
#endif  

	      // Whatever the answer is, the answer is always: DST SRC LEN CHAN TRA DR ACK
	      // the acknowledge of the frame is stored in the dAck and the read value in the data
	      tnumSent[tnum]->fAck = frame[6] ;
	    
	      if ( (frame[2] != 4) || (frame[6] & CCUFACKERROR) ) {
	      
		// Not an error from the device driver
		if (frame[2] != 4) {
		  tnumSent[tnum]->e = NEWFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_PROBLEMINFRAME,
								       "The size expected is different than the size received (bad size in frame, should be 4)",
								       ERRORCODE,
								       tnumSent[tnum]->index,
								       "Frame size", frame[2] ) ;
		}
		else {
		  tnumSent[tnum]->e =  NEWFECEXCEPTIONHANDLER_INFOSUP ( DD_DATA_CORRUPT_ON_WRITE,
									"Bad direct acknowledge status",
									ERRORCODE,
									tnumSent[tnum]->index,
									"direct acknowledge", frame[realSize +3] ) ;
		}

#ifdef DEBUGMSGERRORMF
		std::cerr << tnumSent[tnum]->e->what() << std::endl ;
#endif
	      }
	      else {
#ifdef DEBUGMSGERRORMF
		std::cout << "Read answer correct and data = 0x" << std::hex << (int)frame[5] << std::endl ;
#endif

		// data read
		tnumSent[tnum]->data = frame[5] ;
	      }
	    }
	    
	    // one read less ==> only in case of read request or when a force ack is set
	    // decrement the counter of sent
	    if ( ((tnumSent[tnum]->accessType == MODE_WRITE) && (forceAcknowledge)) ||
		 (tnumSent[tnum]->accessType == MODE_READ) ) cptRead -- ;

#ifdef DEBUGMSGERRORMF
	    char msgr[80] ;
	    decodeKey(msgr,tnumSent[tnum]->index) ;
	    std::cout << "Free the transaction number " << std::dec << tnum << " and busy channel for index " << msgr << std::endl ;
#endif   
	  }
	}
	else {

	  if (tnumSent.find(tnum) == tnumSent.end()) {

	    std::cerr << "*************************************** ERROR ****************************************" << std::endl ;
	    std::cerr << __func__ << ":" << std::hex << "FEC slot "<<(int)getFecSlot() <<" ring "<<(int)getRingSlot()<<std::dec<< std::endl ;
	    std::cerr << "Bad frame received, the transaction number does not correspond to any transaction sent" << std::endl ;
	    std::cerr << "Transaction number = " << std::dec << (int)tnum << " (0x" << std::hex << (int)tnum << ")" << std::endl ;
	    std::cerr << "**************************************************************************************" << std::endl ;

	  } else {

	    char msgOrig[80], msgReceived[80] ;
	    decodeKey(msgOrig,tnumSent[tnum]->index) ;
	    decodeKey(msgReceived,checkIndex) ;
	    std::cerr << "*************************************** ERROR ****************************************" << std::endl ;
	    std::cerr << __func__ << ":" << std::endl ;
	    std::cerr << "The index given in the transaction sent is not the same than the transaction received" << std::endl ;
	    std::cerr << "Original index/Received index = 0x" << std::hex << tnumSent[tnum]->index << " / 0x" << checkIndex << std::endl ;
	    std::cerr << "Original index = " << msgOrig << std::endl ;
	    std::cerr << "Received index = " << msgReceived << std::endl ;
	    std::cerr << "**************************************************************************************" << std::endl ;	  
	  }
	}
      }
      // ------------------------------------------------------------------------------
      // Wait for a timeout
      else {

	timeoutReadFrame ++ ;
	if (timeoutReadFrame < 2) {

	  // Wait for incoming data (with a timeout)
	  unsigned long watchdog = 0 ;
	  fecSR0 = getFecRingSR0() ;
	  while ( !(fecSR0 & FEC_SR0_DATATOFEC) && (watchdog++ < loopInTimeReadFrame) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
	    struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
	    nanosleep (&req,NULL) ;
	    //usleep (TIMETOWAIT) ;
#endif
	    fecSR0 = getFecRingSR0() ;
	  }

#ifdef RECALCULATETIMEOUT
	  // --------------------------------------------------------------------------------------
	  // Update the timeout
	  if ( (loopInTimeReadFrame == TIMEOUT) && (watchdog < TIMEOUT) ) {
	    if (!watchdog) watchdog = 1000 ;
	    loopInTimeReadFrame_ = watchdog*2 ;
	    
#  ifdef DEBUGMSGERROR
	    std::cout << "loopInTimeReadFrame  local to setBlockDevices = " << std::dec << loopInTimeReadFrame << std::endl ;
#  endif
	  }
#endif
	}
      }

#ifdef DEBUGMSGERRORMF
      std::cout << "End: Size = " << tnumSent.size() << std::endl ;
#endif
    }

    /* **************************************************************************************************************************** */ 
    /*                          Release the different channels and the frames sent                                                  */
    /*                          Check if all the frames sent has been correctly managed                                             */
    /*                          If not, then raise an error                                                                         */
    /* **************************************************************************************************************************** */ 
    
#ifdef DEBUGMSGERRORMF
    std::cout << "Number of transaction not acknowledge " << tnumSent.size() << std::endl ;
#endif

    for (accessTransactionFrameMap::iterator p=tnumSent.begin() ; p != tnumSent.end() ; p ++) {

      if (p->second != NULL) {

	if (p->second->dAck == 0) {  // timeout on the read anser of the force acknowledge

#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
	  char msg[80] ;
	  decodeKey(msg, p->second->index) ;
	  std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
	  std::cerr << "Device " << msg << std::endl ;
#endif    

	  p->second->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( DD_WRITE_OPERATION_FAILED,
							       "Timeout reached on the direct acknowledge",
							       ERRORCODE,
							       p->second->index ) ;
	
#ifdef DEBUGMSGERRORMF
	  std::cerr << p->second->e->what() << std::endl ;
#endif
	}
	else { // timeout on the read frame or the force acknowledge
	
	  // If a read request has not been correctly handle so:
	  // if a frame is waited and (no force ack or read answer) has been receive and the direct ack was correct then
	  // a timeout on the (force ack/read answer) is raised
	  if ( (((p->second->accessType == MODE_WRITE) && (forceAcknowledge)) ||
		(p->second->accessType == MODE_READ)) &&
	       (p->second->fAck == 0) && (p->second->dAck == FECACKNOERROR32) ) {
	    
	    
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
	    char msg[80] ;
	    decodeKey(msg, p->second->index) ;
	    std::cerr << __func__ << ": Cannot read the frame, timeout has been reached for transaction " << std::dec << (int)p->second->tnum << std::endl ;
	    std::cerr << "Device = " << msg << std::endl ;
#endif
	    
	    p->second->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( DD_READ_FRAME_CORRUPTED,
								 "Unable to read a frame, timeout reached",
								 ERRORCODE,
								 p->second->index ) ;
	    
#ifdef DEBUGMSGERRORMF
	    std::cerr << p->second->e->what() << std::endl ;
#endif

	    // One read done
	    cptRead -- ;
	  }
	}
	
	// Channel is free to sent a frame
	busy[getFecRingCcuChannelKey(p->second->index)] = 0 ;
	
	// release the hash_map, transaction number and busy channel
	if (p->second->tnum != 0) {
	  //tnumSent.erase(p->second->tnum) ;
	  releaseTransactionNumber(p->second->tnum) ;	
	}
      }
    }

    // End of the transaction, clear the table
    tnumSent.clear() ;

    /* **************************************************************************************************************************** */ 
    /*                                           This part concerns all the frames not the frames filled in the current buffer      */
    /*                                           Check that all the frames has been sent to finish the methods                      */
    /*                                           If not all the frames has been sent then wait for a timeout                        */
    /* **************************************************************************************************************************** */ 	
#ifdef DEBUGMSGERRORMF
    std::cout << "----------------------------> number of frames: " << std::dec << countFrame << " / " << vAccessDevices.size() << " (number of read = " << cptRead << ")" << std::endl ;
#endif

    if (noMoreTransaction && !itemSent) {
      //DeviceFrame::displayAccessDeviceType(vAccessDevices) ;
      cptRead = 0 ;
      countFrame = vAccessDevices.size() ;
    }

    if (countFrame == vAccessDevices.size()) {
      // is it the end, no more frame to send and no more read request
      //std::cout << "Number of frames = " << countFrame << "/" << vAccessDevices.size() << std::endl ;
      //std::cout << "Number of read to be done = " << cptRead << std::endl ;
      if (cptRead == 0) finished = true ;
#ifdef F1STLEVELCHECK
      else {
	RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR,
				   "Some transactions has been not managed correctly (software problem)",
				   ERRORCODE ) ;
      }
#endif
    }

    /* **************************************************************************************************************************** */ 
    /*                                           Check the rings and clear errors and IRQ                                           */
    /* **************************************************************************************************************************** */ 
    fecSR0 = getFecRingSR0() ;
    // Clear the IRQ
    //if ((fecSR0 & FEC_SR0_PENDINGIRQ) || (fecSR0 & FEC_SR0_DATATOFEC))
    setFecRingCR1 (FEC_CR1_CLEARIRQ);
    // Check the status 0 of the FEC, if it is not correct then an error occurs
    if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
      
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
				 TSCFEC_SR0NOTNOMINAL_MSG + ": the next frames are not sent",
				 FATALERRORCODE,
				 buildFecRingKey(getFecSlot(),getRingSlot()),
				 "FEC status register 0", getFecRingSR0()) ;
    }
  }

#ifdef DEBUGMSGERRORMF
  tscType16 fecSR0E = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice(" << std::dec << (int)getFecSlot() << "." << (int)getRingSlot() << ")::setBlockDevices: end and the SR0 is " << std::hex << fecSR0E << std::endl ;
#endif
}


/** Receive all the frames specified by the different arguments
 * \param vAccessDevices - complete list of frames
 * \param frameSent - number of frames send
 * \param forceChannelAck - is the force acknowledge set
 * \param tnumSent - all the transaction number used
 * \param busy - busy channels
 * \param cptRead - number of read expected (or force acknowledge or read requests)
 */
void FecRingDevice::getBlockFrames ( bool forceChannelAck, accessTransactionFrameMap &tnumSent,
				     Sgi::hash_map<keyType, int> & busy, unsigned int & cptRead )
  throw (FecExceptionHandler) {
  
  //#define DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
  //#define DEBUGMSGERRORMF

  // *********************************************************************
  // *********************************************************************
  //
  // Wait for the frames
  //
  // *********************************************************************
  // *********************************************************************
  /*
  struct timeval mst;
  gettimeofday(&mst,0);
  std::cerr << "Start of FecRingDevice::getBlockFrames at " << mst.tv_sec << " " << mst.tv_usec <<" Fec slot " << (int) getFecSlot() << " Ring slot " << (int) getRingSlot() << std::endl;
  */

  // Wait for the FIFO transmit bit running
  unsigned long watchdog = 0;
  tscType32 fecSR0 = getFecRingSR0() ;
  while ( ((fecSR0 & FEC_SR0_TRARUN) || (!(fecSR0 & FEC_SR0_TRAEMPTY))) && (watchdog++ < loopInTimeWriteFrame_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
    struct timespec req ; req.tv_sec = 0 ; 
    req.tv_nsec = TIMETOWAIT + 100000*(int)getFecSlot();
    nanosleep (&req,NULL) ;
    //usleep (TIMETOWAIT) ;
#endif
    fecSR0 = getFecRingSR0() ;
  }

  /*
  gettimeofday(&mst,0);
  std::cerr << "After Wait for the FIFO transmit bit running " << mst.tv_sec << " " << mst.tv_usec <<" Fec slot " << (int) getFecSlot() << " Ring slot " << (int) getRingSlot() << std::endl;
  */

  // if the FIFO transmit bit is running raise an error
  fecSR0 = getFecRingSR0() ;
  if ((fecSR0 & FEC_SR0_TRARUN) || !(fecSR0 & FEC_SR0_TRAEMPTY) || !(fecSR0 & FEC_SR0_LINKINITIALIZED) ) {

#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
    std::cerr << __func__ << ": FIFO transmit running bit is always ON (SR0 = " 
	      << std::hex << fecSR0 << ")" << std::dec << std::endl ;
#endif

    if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
					 "Fails on sending a frame, ring lost",
					 CRITICALERRORCODE,
					 buildFecRingKey(getFecSlot(), getRingSlot()),
					 "FEC status register 0", fecSR0 ) ;
    } 
    else {
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON,
					 "FIFO transmit running bit is always on (CR0[3])",
					 CRITICALERRORCODE,
					 buildFecRingKey(getFecSlot(), getRingSlot()),
					 "FEC status register 0", fecSR0 ) ;
    }
  }

  // Wait for the FIFO receive not running and FIFO receive not empty
  while ( ((fecSR0 & FEC_SR0_RECRUN) || !(fecSR0 & FEC_SR0_PENDINGIRQ)) && (watchdog++ < loopInTimeDirectAck_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
    struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000  + 100000*(int)getFecSlot(); //TIMETOWAIT ;
    nanosleep (&req,NULL) ;
    //usleep (TIMETOWAIT) ;
#endif
    fecSR0 = getFecRingSR0() ;
  }
  /*
  gettimeofday(&mst,0);
  std::cerr << "After Wait for the FIFO receive not running and FIFO receive not empty " << mst.tv_sec << " " << mst.tv_usec <<" Fec slot " << (int) getFecSlot() << " Ring slot " << (int) getRingSlot() << std::endl;
  */

  // if the FIFO receive is empty raise an error
  fecSR0 = getFecRingSR0() ;
  if ( (fecSR0 & FEC_SR0_RECEMPTY) || !(fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
      
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
    std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
#endif

    if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
				  "Fails on sending a frame, ring lost",
				  CRITICALERRORCODE,
				  buildFecRingKey(getFecSlot(), getRingSlot()),
				  "FEC status register 0", fecSR0 ) ;
    } 
    else {
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_CANNOT_READ_DATA,
				 "Unable to read a frame or the force acknowledge, timeout reached",
				 ERRORCODE,
				 buildFecRingKey(getFecSlot(), getRingSlot()),
				 "FEC status register 0", fecSR0 ) ;
    }
  }

  // *********************************************************************
  // *********************************************************************
  //
  // Manage frames
  //
  // *********************************************************************
  // *********************************************************************
  unsigned int countFrameTemp   = 0 ;
  unsigned int position         = 0 ;
  unsigned int timeoutReadFrame = 0 ;
  tscType32 frameReceived[MAXFECFIFOWORD] ;  // frame to be received
  while (!((fecSR0=getFecRingSR0()) & FEC_SR0_RECEMPTY) || 
	 (((countFrameTemp != tnumSent.size()) || (cptRead != 0)) && (timeoutReadFrame < 5) ) ) {

    // --------------------------------------------------------------------
    // Check if values exist in the FIFO receive
    if (! (fecSR0 & FEC_SR0_RECEMPTY)) {

      // Read all words from the FIFO
      unsigned int sr0Word = ((fecSR0 & 0xFFFF0000) >> 16) ;
      //std::cout << "Size = " << std::dec << sr0Word << std::endl ;
      //std::cout << "SR0 = 0x" << std::hex << (fecSR0 & 0xFFFF) << std::endl ;
      /*
      gettimeofday(&mst,0);
      std::cerr << "Before getFifoReceive " << mst.tv_sec << " " << mst.tv_usec <<" Fec slot " << (int) getFecSlot() << " Ring slot " << (int) getRingSlot() << std::endl;
      */
      if (sr0Word==0 || sr0Word>MAXFECFIFOWORD) {
	std::cerr << "FEC slot "<< std::hex<< (int)getFecSlot()<<" Ring Slot "<<(int)getRingSlot()<<std::dec<<": Found sr0Word = "<< sr0Word <<" ! Emptying receive FIFO the old way" << std::endl;
	sr0Word=0;
	while ( (!((fecSR0=getFecRingSR0())&  FEC_SR0_RECEMPTY) ) && (sr0Word <MAXFECFIFOWORD) )
	  frameReceived[sr0Word++]= getFifoReceive() ;
	std::cerr << "Extracted "<<sr0Word << " words from receive FIFO"<<std::endl;
      }
      else getFifoReceive(frameReceived, sr0Word) ;
      /*
      gettimeofday(&mst,0);
      std::cerr << "After getFifoReceive got " << sr0Word << "words "<< mst.tv_sec << " " << mst.tv_usec <<" Fec slot " << (int) getFecSlot() << " Ring slot " << (int) getRingSlot() << std::endl;
      */

      //for (unsigned int pos = 0 ; pos < sr0Word ; pos ++) {
      //std::cout << std::dec << pos << ": " << std::hex << frameReceived[pos] << std::endl ;
      //}

      // Position in use in case of problems in the number of words for the FIFO
      position = 0 ;

      //      while (position != sr0Word) {
      // if frames are corrupted there is no garantee that position == sr0Word 
      while (position < sr0Word) {
	// ---------------------------------------------------------------
	// Retreive the size of the frame and check the size
	int realSize = (frameReceived[position] >> 8) & 0xFF ;
	tscType8 word2 = frameReceived[position]  & 0xFF ;
	if (realSize & FEC_LENGTH_2BYTES) {
	  realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
	  // +1 for the two byte words for length
	}
	
	// +3 => Dst, Src, Length  ; +1 => status after the frame
	int realSize32 = (realSize+3+1)/4 ;
	if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;

	// The size of the frame is too important
	if (realSize32 > MAXFECFIFOWORD) {

	  //#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES
	  std::cerr << __func__ << ": the size expected is not coherent: " 
		    << realSize32 << " > " << MAXFECFIFOWORD << std::endl ;
	  //#endif

	  RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_TOO_LONG_FRAME_LENGTH,
				     "Size in frame is bigger than the FIFO size",
				     CRITICALERRORCODE,
				     buildFecRingKey(getFecSlot(), getRingSlot()),
				     "Size of the frame received", realSize32 ) ;
	}

	// -------------------------------------------------------------------
	// Convert it into a frame of 8 bits / The status is just after the frame
	//	tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = {0} ; // frame received
        tscType8 frame[MAXFECFIFOWORD*4] = {0} ; //same as in if (realSize32 > MAXFECFIFOWORD)
	unsigned int cpt = 0 ;
	for ( int i = 0 ; i < realSize32 ; i++, position ++) {
	  frame[cpt++] = (frameReceived[position] >> 24) & 0xFF ;
	  frame[cpt++] = (frameReceived[position] >> 16) & 0xFF ;
	  frame[cpt++] = (frameReceived[position] >>  8) & 0xFF ;
	  frame[cpt++] = (frameReceived[position])       & 0xFF ;

	  //std::cout << __func__ << ": 0x" << std::hex << std::setw(8) << std::setfill('0') << frameReceived[position] << std::endl ;
	}

#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
	std::cout << __func__ << ": Frame received ( { " ;
	for (int i = 0 ; i < ((frame[2]+3)+1) ; i ++ )
	  std::cout << "0x" << std::hex << (int)frame[i] << " " ;
	std::cout << "} )" << std::dec << std::endl ;
#endif

	// -------------------------------------------------------------------
	// Retreive the transaction number
	// Check the size that must be or 2 or 3 (in 32 bits words)
	tscType8 tnum ;
	if ((realSize32 == 3) || (realSize32 == 2)) {
	  // Find the transaction number, check only for transaction number
	  //keyType index = buildCompleteKey(getFecSlot(), getRingSlot(), frame[0], frame[3], frame[5] & 0xF0) ;
	  tnum = frame [4] ;
	}
	else tnum = 0 ; // 0 to have an error

	// *********************************************************************
	// *********************************************************************
	//
	// Frame treatment
	// Transaction and direct acknoledge or force acknowledge or read answer
	//
	// *********************************************************************
	// *********************************************************************
	// Check if a frame with this transaction number has been sent 
	// and check if the index received is the same than the index expected (sent)
	keyType checkIndex = 0 ;
	if (frame[0] != FRAMEFECNUMBER)
	  checkIndex = buildCompleteKey(getFecSlot(),getRingSlot(),frame[0],frame[2] & FEC_LENGTH_2BYTES ? frame[4] : frame[3], 0) ;
	else
	  checkIndex = buildCompleteKey(getFecSlot(),getRingSlot(),frame[1],frame[2] & FEC_LENGTH_2BYTES ? frame[4] : frame[3], 0) ;
	
	// Is the frame related to a transaction already sent and is the index is coherent
	if ((tnumSent.find(tnum) != tnumSent.end()) && (checkIndex == getFecRingCcuChannelKey(tnumSent[tnum]->index))) {

	  // -------------------------------------------------
	  // Direct acknowledge analysis
	  // -------------------------------------------------
	  // if the frame is a direct ack then the frame[1] is the FEC number and frame[0] is the CCU number
	  if (frame[0] != FRAMEFECNUMBER) {
	    
#ifdef DEBUGMSGERRORMF
	    std::cout << "Direct Acknowledge = 0x" << std::hex << (int)frame[frame[2] + 3] << std::endl ;
#endif

	    // ------------------------------------------------------------------------
	    // Analyse the frame and in case of bad direct acknowledge
	    if (frame[frame[2] + 3] != FECACKNOERROR32) { // bad direct acknowledge
	    
	      // Frame not ok => error
	      tnumSent[tnum]->e = NEWFECEXCEPTIONHANDLER_INFOSUP ( DD_DATA_CORRUPT_ON_WRITE,
								   "Bad status in frame",
								   ERRORCODE,
								   tnumSent[tnum]->index,
								   "Status of frame", frame[frame[2] +3] ) ;
		  
#ifdef DEBUGMSGERRORMF
	      std::cerr << "Bad Direct Acknowledge = 0x" << std::hex << (int)frame[frame[2] + 3] << std::endl ;
	      std::cerr << tnumSent[tnum]->e->what() << std::endl ;
#endif

	      // decrement the counter of sent, no answer will arrived
	      if ( ((tnumSent[tnum]->accessType == MODE_WRITE) && forceChannelAck) ||
		   (tnumSent[tnum]->accessType == MODE_READ) ) cptRead -- ;
	    }
	    // Analyse the frame and in case of correct direct acknowledge
	    countFrameTemp ++ ;
	    // store the DACK in the frame
	    tnumSent[tnum]->dAck = frame[frame[2] + 3] ;

	    // ------------------------------------------------------------------------------------
	    // If no force acknowledge expected then release the transaction number and the channel
	    // If no read request then release the transaction number and the channel
	    if ( ((!(tnumSent[tnum]->accessType == MODE_READ) && !forceChannelAck)) ||
		 (tnumSent[tnum]->e != NULL) ) {
	      // Release the transaction number used
	      releaseTransactionNumber (tnum) ;
	      // Release the channel busy
	      busy[getFecRingCcuChannelKey(tnumSent[tnum]->index)] = 0 ;
	    }
	  }
	  // ------------------------------------------------------------------------
	  // Analyse the frame if it is a force acknowledge
	  // ------------------------------------------------------------------------
	  else { // force ack or read answer

	    // Read answer depend of what was the request: 
	    // if it is not a read answer or it is a force acknowledge or it is an error related to a device	    // Read answer: frame[5] => data read, frame[6] => status
	    // force ack or error: frame[5] => status
	    int positionFrameStatus = 5 ;
	    int sizeExpected        = 3 ;
	    if (tnumSent[tnum]->accessType == MODE_READ) {
	      positionFrameStatus = 6 ;
	      sizeExpected  = 4 ;
	    }

#ifdef DEBUGMSGERRORMF
	    std::cout << "Force acknowledge = 0x" << std::hex << (int)frame[positionFrameStatus] << std::endl ;
#endif  

	    // Store the status of the force ack or read answer
	    tnumSent[tnum]->fAck = frame[positionFrameStatus] ;

	    // -------------------------------------------------------------------------------
	    // Check that the size is correct and the status is correct, if not raise an error
	    if ((frame[2] != sizeExpected) || (frame[positionFrameStatus] & CCUFACKERROR)) {

	      // Not an error from the device driver
	      if (frame[2] != sizeExpected) {
		std::stringstream msgError ; msgError << "Size expected is not correct: " << std::dec << (int)frame[2] << " / " << sizeExpected ;
		tnumSent[tnum]->e = NEWFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_PROBLEMINFRAME,
								    msgError.str(),
								    ERRORCODE,
								    tnumSent[tnum]->index,
								    "Size in frame", frame[2]) ;
	      }
	      else {
		tnumSent[tnum]->e = NEWFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_BADFACKORREADANS,
								    "Status of the force acknowledge is not correct",
								    ERRORCODE,
								    tnumSent[tnum]->index,
								    "Status of the FACK", frame[positionFrameStatus]) ;

	      }

#ifdef DEBUGMSGERRORMF
	      std::cerr << tnumSent[tnum]->e->what() << std::endl ;
#endif
	    }
	    // Check that the size is correct and the status is correct
	    // and it is correct
	    else if (tnumSent[tnum]->accessType == MODE_READ) {
#ifdef DEBUGMSGERRORMF
	      std::cout << "Read answer correct and data = 0x" << std::hex << (int)frame[5] << std::endl ;
#endif
	      // data read
	      tnumSent[tnum]->data = frame[5] ;
	    }

	    // one read less ==> only in case of read request or when a force ack is set
	    // decrement the counter of sent
	    if ( ((tnumSent[tnum]->accessType == MODE_WRITE) && forceChannelAck) ||
		 (tnumSent[tnum]->accessType == MODE_READ) ) cptRead -- ;

	    // Release the transaction number used
	    releaseTransactionNumber (tnum) ;
	    // Release the channel busy
	    busy[getFecRingCcuChannelKey(tnumSent[tnum]->index)] = 0 ;
	  }
	}
	// ----------------------------------------------------------------
	// if not frame is associated with this transaction
	// or the index is incoherent
	// ---------------------------------------------------------------
	else {
	  if (tnumSent.find(tnum) == tnumSent.end()) {
	    std::cerr << "*************************************** ERROR ****************************************" << std::endl ;
	    std::cerr << __func__ << ":" << std::endl ;
	    std::cerr << "Bad frame received, the transaction number does not correspond to any transaction sent" << std::endl ;
	    std::cerr << "Transaction number = " << std::dec << (int)tnum << " (0x" << std::hex << (int)tnum << ")" << std::endl ;
	    std::cerr << "**************************************************************************************" << std::endl ;
	    
	  } else {

	    char msgOrig[80], msgReceived[80] ;
	    decodeKey(msgOrig,tnumSent[tnum]->index) ;
	    decodeKey(msgReceived,checkIndex) ;
	    std::cerr << "*************************************** ERROR ****************************************" << std::endl ;
	    std::cerr << __func__ << ":" << std::hex << "FEC slot "<<(int)getFecSlot() <<" ring "<<(int)getRingSlot()<<std::dec<< std::endl ;
	    std::cerr << "The index given in the transaction sent (" << std::dec << (int)tnum << ") is not the same than the transaction received (" << (int)tnumSent[tnum]->tnum << ")" << std::endl ;
	    std::cerr << "Original index/Received index = 0x" << std::hex << tnumSent[tnum]->index << " / 0x" << checkIndex << std::endl ;
	    std::cerr << "Original index = " << msgOrig << std::endl ;
	    std::cerr << "Received index = " << msgReceived << std::endl ;
	    std::cerr << "**************************************************************************************" << std::endl ;	  
	  }
	}
      }
    }
    // -------------------------------------------------------------------------
    // Wait for a timeout, no words has been in the FIFO receive
    // -------------------------------------------------------------------------
    else {
      timeoutReadFrame ++ ;   

      // Wait for incoming data (with a timeout)
      unsigned long watchdog = 0 ;
      fecSR0 = getFecRingSR0() ;
      while ( !(fecSR0 & FEC_SR0_DATATOFEC) && (watchdog++ < loopInTimeReadFrame_) && (fecSR0 & FEC_SR0_LINKINITIALIZED) ) {
#ifdef TIMETOWAIT
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT  + 100000*(int)getFecSlot();
	nanosleep (&req,NULL) ;
	//usleep (TIMETOWAIT) ;
#endif
	fecSR0 = getFecRingSR0() ;
      }
    }
  }
  /*
  gettimeofday(&mst,0);
  std::cerr << "After the while " << mst.tv_sec << " " << mst.tv_usec <<" Fec slot " << (int) getFecSlot() << " Ring slot " << (int) getRingSlot() << std::endl;
  */

  // *********************************************************************
  // *********************************************************************
  //
  // If one the frames has not been set, then set a time in the error
  //
  // *********************************************************************
  // *********************************************************************
  if ((countFrameTemp != tnumSent.size()) || (cptRead != 0)) {

    // Set the timeout for transaction asked
    for (accessTransactionFrameMap::iterator p=tnumSent.begin() ; p != tnumSent.end() ; p ++) {
      
      if (p->second != NULL) {

	// ------------------------------------------------------
	// Frame has not been sent
	if (!p->second->sent) {

	  if ((p->second->accessType == MODE_READ) || forceChannelAck) cptRead -- ;
	  countFrameTemp ++ ;

	  p->second->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( CODECONSISTENCYERROR,
							       "frame has not been sent over the ring for unknown reason",
							       ERRORCODE,
							       p->second->index ) ;

	  std::cerr << "The following frame has not been sent for unknown reason" << std::endl ;
	  DeviceFrame::displayAccessDeviceType(*p->second) ;
	}
	// ---------------------------------------------------
	// The direct ackownledge has been sent but no answer
	else if (p->second->dAck == 0) {

	  if ((p->second->accessType == MODE_READ) || forceChannelAck) cptRead -- ;
	  countFrameTemp ++ ;

	  tscType16 fecSR0 =getFecRingSR0() ; 
	  if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
	    p->second->e = NEWFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
							    "Fails on sending a frame, ring lost",
							    CRITICALERRORCODE,
							    buildFecRingKey(getFecSlot(), getRingSlot()),
							    "FEC status register 0", fecSR0 ) ;
	  }
	  else {
	    p->second->e = NEWFECEXCEPTIONHANDLER_INFOSUP (  DD_WRITE_OPERATION_FAILED,
							     "Timeout reached on the direct acknowledge",
							     ERRORCODE,
							     p->second->index,
							     "FEC status register 0", fecSR0 ) ;
	  }

#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES
	  std::cerr << p->second->e->what() << std::endl ;
#endif

	  // Release the transaction number used
	  releaseTransactionNumber (p->second->tnum) ;
	  // Release the channel busy
	  busy[getFecRingCcuChannelKey(p->second->index)] = 0 ;
	}
	// ---------------------------------------------------
	// Direct ack received but no read answer of force ack
	else if ( (p->second->fAck == 0) && ((p->second->accessType == MODE_READ) || forceChannelAck) ) {

	  cptRead -- ;

	  p->second->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( DD_CANNOT_READ_DATA,
							       "Unable to read a frame or the force acknowledge, timeout reached",
							       ERRORCODE,
							       p->second->index ) ;

#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES
	  std::cerr << p->second->e->what() << std::endl ;
#endif

 	  // ??????????????????????????????????????????????????????????????????
 	  std::cerr << p->second->e->what() << std::endl ;
 	  std::cerr << "CCU 0x" << std::hex << (int)getCcuKey(p->second->index) << std::endl ;
 	  std::cerr << "FEC CR0 = " << std::hex << (int)getFecRingCR0() << std::endl ;
 	  std::cerr << "FEC SR0 = " << std::hex << (int)getFecRingSR0() << std::endl ;
 	  std::cerr << "FEC SR1 = " << std::hex << (int)getFecRingSR1() << std::endl ;
 	  std::cerr << "CCU SRA = " << std::hex << (int)getCcuSRA(p->second->index) << std::endl ;
 	  std::cerr << "CCU CRE = " << std::hex << (int)getCcuCRE(p->second->index) << std::endl ;
 	  std::cerr << "CCU SRE = " << std::hex << (int)getCcuSRE(p->second->index) << std::endl ;
 	  std::cerr << "Channel enabled 0x" << std::hex << getChannelKey(p->second->index) << ": " << isChannelEnabled(p->second->index) << std::endl ;

	  // Release the transaction number used
	  releaseTransactionNumber (p->second->tnum) ;
	  // Release the channel busy
	  busy[getFecRingCcuChannelKey(p->second->index)] = 0 ;
	}
      }
    }

    // -------------------------------------------------------------------------
    // Check that all is zero so all frames has been checked and updated
    if ((cptRead != 0) || (countFrameTemp != tnumSent.size())) {

      //#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES
      std::cerr << "*************************************** ERROR ****************************************" << std::endl ;
      std::cerr << __func__ << ": the number of frames is not 0 after the analysis, please report the error to the support" << std::endl ;
      std::cerr << "\tNumber of read performed: " << std::dec << cptRead << " (should be 0)" << std::endl ;
      std::cerr << "\tNumber of operation performed: " << countFrameTemp << " / " << tnumSent.size() << std::endl ;
      std::cerr << "The status register 0 of the FEC " << std::dec << (int)getFecSlot() << ", ring " << (int)getRingSlot() << " is  " << std::hex << getFecRingSR0() << std::endl ;
      std::cerr << "**************************************************************************************" << std::endl ;
      //#endif

      // Empty the list of the exceptions raised:
      for (accessTransactionFrameMap::iterator it = tnumSent.begin() ; it != tnumSent.end() ; it ++) {

      }

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( DD_TOO_LONG_FRAME_LENGTH,
					      "The number of frames is not 0 after the analysis, please report the error to the support",
					      CRITICALERRORCODE,
					      buildFecRingKey(getFecSlot(), getRingSlot()) ) ;
    }
  }

#ifdef DEBUGMSGERRORMF
  // Display the transaction
  for (accessTransactionFrameMap::iterator it = tnumSent.begin() ; it != tnumSent.end() ; it ++) {

    std::cout << "----------------------------------------------------------------" << std::endl ;
    std::cout << "Transaction number used: " << std::dec << it->first << std::endl ;
    DeviceFrame::displayAccessDeviceType(*(it->second)) ;

    if (busyTransactionNumber[it->second->tnum]) {
      std::cerr << "Found a transaction not release for this this transaction" << std::endl ;
    }
  }
#endif
  /*
  struct timeval met;
  gettimeofday(&met,0);
  std::cerr << "End of FecRingDevice::getBlockFrames at " << met.tv_sec << " " << met.tv_usec << " Fec slot " << (int) getFecSlot() << " Ring slot " << (int) getRingSlot() << std::endl;
  */
}

/** This methods takes an array of values to be set and send it over the ring. It collects afterwards the direct acknoledge and return an array of exceptions or null or no problems arrives. Please note that the array of exceptions must be deleted by the remote user of the method. This methods make also a sort in the array depending of the CCU/channel to be accessed.
 * \warning All the accessDeviceType must be initialise with dAck = 0 and sent = false
 * \warning if this method encounters a register problem, the rest of the registers will be sent and the same exception (pointer point of view) is set in all the request to that device. So do not delete directly all the exception from the list returned
 */
void FecRingDevice::setBlockDevicesBltMode ( std::list<accessDeviceType> &vAccessDevices, bool forceAcknowledge )
  throw (FecExceptionHandler) {

  //#define DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
  //#define DEBUGMSGERRORMF

  // number of loops before the answer to a frame
  unsigned int loopInTimeWriteFrame = TIMEOUT ;

#ifdef DEBUGMSGERRORMF
  tscType16 fecSR0D = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice(" << std::dec << (int)getFecSlot() << "." << (int)getRingSlot() << ")::setBlockDevicesBltMode: begin and the SR0 is " << std::hex << fecSR0D << std::endl ;
  std::cout << "Number of frames to be sent " << std::dec << vAccessDevices.size() << std::endl ;
#endif

  // Check the status register 0 of the FEC
  checkRing() ;

  // -----------------------------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------------------------

  tscType32 fecSR0 ;

  bool finished            = false ;          // true if all operation read/write has been done or if a timeout occurs
  unsigned int wordFifo    = 0     ;          // number of allowed in 32 bits in the FIFO transmit
  unsigned int fifoRecWord = 0     ;          // number of words expected in the FIFO receive with error
  unsigned int nWordFifo   = 0     ;          // number of words expected in the FIFO receive without error
  accessTransactionFrameMap tnumSent ;        // frames already sent
  unsigned int cptRead    = 0     ;          // number of read request
  unsigned int countFrame = 0      ;          // number of frame sent
  unsigned int itemSent   = 0      ;          // number of frame sent for the current block of frames
  unsigned short tnum     = 1      ;          // current transaction number
  std::list<unsigned short> tnumUsed ;        // store all transaction number used
  bool noMoreTransaction = false   ;          // Anymore transaction to be sent ?
  Sgi::hash_map<keyType,int> busy ;          // busy FEC/Ring/CCU/channel  
  tscType32 toBeTransmited[MAXFECFIFOWORD] ;  // size of the frame
  std::list<accessDeviceType>::iterator itAccessDevice      ; // next frame to be sent
  Sgi::hash_map<keyType, FecExceptionHandler *> errorDevice ; // in order to avoid multiple send of frames to a faulty device

  // Check if the channels are PIA or I2C
  itAccessDevice = vAccessDevices.begin() ;

  // No busy channel
  for (std::list<accessDeviceType>::iterator itAccessDevice = vAccessDevices.begin() ; itAccessDevice != vAccessDevices.end() ; itAccessDevice ++) {
    busy[getFecRingCcuChannelKey(itAccessDevice->index)] = 0 ;
    errorDevice[itAccessDevice->index] = NULL ;

#ifdef DEBUGMSGERRORMF
    char msg[80] ;
    decodeKey(msg, itAccessDevice->index) ;
    std::cerr << "Device " << msg << std::endl ;
    std::cerr << "Channel busy since " << std::dec << busy[getFecRingCcuChannelKey(itAccessDevice->index)] << " (timestamp " << time(NULL) << ")" << std::endl ;
#endif

#ifdef S2NDLEVELCHECK
    if (! isi2cChannelCcu25(itAccessDevice->index) &&
	! isPiaChannelCcu25(itAccessDevice->index)) {
      std::cerr << "*************************************** ERROR ****************************************" << std::endl ;
      std::cerr << __func__ << ": This method is forseen only for PIA or I2C channels" << std::endl ;
      std::cerr << "**************************************************************************************" << std::endl ;
      RAISEFECEXCEPTIONHANDLER ( TSCFEC_INVALIDOPERATION,
				 "This method is forseen only for PIA or I2C channels",
				 ERRORCODE) ;
    }
#endif
  }

  // -------------------------------------------- starts
  // Until the end
  while (!finished) {

    //std::cout << "Building the frames" << std::endl ;

    /* **************************************************************************************************************************** */ 
    /*                                           Select the frames to be sent over each channel                                     */
    /*                                           Build the frame (see method buildPiaFrame and buildI2CFrame)                       */
    /*                                           Build a buffer with all the frames to sent (depending of the size of the FIFOs)    */
    /* **************************************************************************************************************************** */ 
    // is an item has been sent
    itemSent = 0 ;
    bool itemLoop = false ;
    fifoRecWord = wordFifo = nWordFifo = 0 ;
    tnum = 1 ;
    // The -10 is given for the last transaction if the answer or the force ack. is not arrived
    while (!noMoreTransaction && !itemLoop && (fifoRecWord < receiveFifoDepth_) && (tnum != 0)) {

      // Find the next transaction to be sent
      itAccessDevice = vAccessDevices.begin() ;
      bool occ = true ;
      noMoreTransaction = true ;
      while ( (itAccessDevice != vAccessDevices.end()) && occ) {

	if (!itAccessDevice->sent) {

	  // if the device has already an error
	  if (errorDevice.find(itAccessDevice->index) != errorDevice.end()) {

	    itAccessDevice->sent = true ;
	    itAccessDevice->e = errorDevice[itAccessDevice->index] ;
	    // Number of frame sent
	    countFrame ++ ;

#ifdef DEBUGMSGERRORMF
	    char msg[80] ;
	    decodeKey(msg, itAccessDevice->index) ;
	    std::cerr << "The device " << msg << " has already an error, no frames are sent anymore to it" << std::endl ;
#endif
	  }
	  else { // No error on this device
#ifdef DEBUGMSGERRORMF
	    char msg[80] ;
	    decodeKey(msg, itAccessDevice->index) ;
	    std::cerr << "Device " << msg << std::endl ;
	    std::cerr << "Channel busy since " << std::dec << busy[getFecRingCcuChannelKey(itAccessDevice->index)] << " (timestamp " << time(NULL) << ")" << std::endl ;
#endif

	    noMoreTransaction = false ;
	    if (!busy[getFecRingCcuChannelKey(itAccessDevice->index)]) {

#ifdef DEBUGMSGERRORMF
	      std::cout << "And channel not busy" << std::endl ;
#endif
	      
	      occ = false ;
	    }
	    else {
	      
	      // check the timestamp to see if the frame has been sent since a long time
	      if ((time(NULL) - busy[getFecRingCcuChannelKey(itAccessDevice->index)]) > 2) { // 2 sec
		
		if (itAccessDevice->dAck == 0) {  // timeout on the force acknowledge
		  
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
		  char msg[80] ;
		  decodeKey(msg, itAccessDevice->index) ;
		  std::cerr << __func__ << ": timeout reached on the direct acknowledge" << std::endl ;
		  std::cerr << "Device " << msg << std::endl ;
#endif
		  
		  itAccessDevice->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( DD_WRITE_OPERATION_FAILED,
									    "Timeout reached on the direct acknowledge",
									    ERRORCODE,
									    itAccessDevice->index ) ;

#ifdef DEBUGMSGERRORMF
		  std::cerr << itAccessDevice->e->what() << std::endl ;
#endif
		}
		else { // timeout on the read frame or the force acknowledge
		  
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
		  char msg[80] ;
		  decodeKey(msg, itAccessDevice->index) ;
		  std::cerr << __func__ << ": Cannot read the frame, timeout has been reached for transaction " << std::dec << (int)itAccessDevice->tnum << std::endl ;
		  std::cerr << "Device = " << msg << std::endl ;
#endif
		
		  itAccessDevice->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION ( DD_CANNOT_READ_DATA,
									    "Unable to read a frame or the force acknowledge, timeout reached",
									    ERRORCODE,
									    itAccessDevice->index ) ;

#ifdef DEBUGMSGERRORMF
		  std::cerr << itAccessDevice->e->what() << std::endl ;
#endif
		}
	      
		// busy channel is now release on a timeout
		busy[getFecRingCcuChannelKey(itAccessDevice->index)] = 0 ;
		occ = false ;
	      }
	      // next transaction
	      else itAccessDevice ++ ;
	    }
	  }
	}
	else itAccessDevice ++ ;
      }

      if (itAccessDevice == vAccessDevices.end()) itemLoop = true ;
      else {

	// Next transaction number, 0 if no more transaction number exists
	tnum = getNextTransactionNumber ( ) ;

#ifdef DEBUGMSGERRORMF
	// display
	char msg[80] ;
	decodeKey(msg,itAccessDevice->index) ;
	std::cout << "Index = " << msg << std::endl ;
	std::cout << "Channel busy = " << busy[getFecRingCcuChannelKey(itAccessDevice->index)] << std::endl ;
	std::cout << "Sent = " << itAccessDevice->sent << std::endl ;
	std::cout << "Transaction number used = " << tnum << std::endl ;
#endif

	// if a transaction number exists
	if (tnum != 0) {
	  
          // Transaction number
          itAccessDevice->tnum = tnum ;
	  tnumUsed.push_back(tnum) ;
	  
          // Write the i2c frame in the FIFO tra. (nothing else) and return the number of word written
	  unsigned int fifoTraRecWord = 0 ;
	  if (isi2cChannelCcu25(itAccessDevice->index)) {
	    unsigned int val ;
	    wordFifo += buildI2CFrame ( *itAccessDevice, &toBeTransmited[wordFifo], forceAcknowledge, &fifoTraRecWord, &val ) ;
	    nWordFifo += val ;
	    // Counter for read request
	    if ( (itAccessDevice->accessType == MODE_READ) || (forceAcknowledge)) cptRead ++ ;
	  }	    
	  else if (isPiaChannelCcu25(itAccessDevice->index)) {
	    wordFifo += buildPIAFrame ( *itAccessDevice, &toBeTransmited[wordFifo], &fifoTraRecWord ) ;
	    nWordFifo = wordFifo ;
	    // Counter for read request
	    if (itAccessDevice->accessType == MODE_READ) cptRead ++ ;
	  }
	      
	  fifoRecWord += fifoTraRecWord ;
	  
#ifdef DEBUGMSGERRORMF
	  char msg[80] ;
	  decodeKey(msg,itAccessDevice->index) ;
	  std::cout << "Add a transaction " << (int)tnum << "(0x" << std::hex << tnum << ") on index " << msg << std::endl ;
#endif
	  
          // This frame wait for its DACK
          tnumSent[tnum] = &(*itAccessDevice) ;
          itAccessDevice->sent = true ;  // sent

          // Channel is busy
          busy[getFecRingCcuChannelKey(itAccessDevice->index)] = time(NULL) ;
	  
	  // an item has been sent
	  itemSent ++ ;
	}
	else std::cout << "No transaction available" << std::endl ;
      }

#ifdef DEBUGMSGERRORMF
      std::cout << "while (!noMoreTransaction && !itemLoop && (fifoRecWord < " << receiveFifoDepth_ << " && (tnum != 0)) {" << std::endl ;
      std::cout << "noMoreTransaction = " << std::dec << noMoreTransaction << std::endl ;
      std::cout << "itemLoop = " << std::dec << itemLoop << std::endl ;
      std::cout << "Words = " << std::dec << fifoRecWord << "/" << receiveFifoDepth_ << std::endl ;
      std::cout << "Transaction number = " << std::dec << (int)tnum << std::endl ;
      std::cout << "Cond = " << (!noMoreTransaction && !itemLoop && (fifoRecWord < receiveFifoDepth_) && (tnum != 0)) << std::endl ;
#endif
    }

    //std::cout << "End of building the frames" << std::endl ;
    //std::cout << "Sending the frames" << std::endl ;

    /* **************************************************************************************************************************** */ 
    /*                                           Fill the FIFO transmit                                                             */
    /*                                           clear the errors and send the frames over to the ring                              */
    /*                                           wait until the FIFO transmit are sent                                              */
    /*                                           and wait for the pending IRQ                                                       */
    /* **************************************************************************************************************************** */ 

    if (itemSent) {

      // Fill the FIFO transmit
      setFifoTransmit(toBeTransmited,wordFifo); 

      //for (unsigned int  i = 0 ; i < wordFifo ; i ++) {
      //std::cout << __func__ << ": 0x" << std::hex << std::setw(8) << std::setfill('0') << toBeTransmited[i] << std::endl ;
      //}

      // Clear the error bit
      setFecRingCR1 ( DD_FEC_CLEAR_ERRORS ) ;
      // Toggle the send bit of CTRLO with RMW operation
      setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_OR ) ;
      setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_XOR ) ;

#ifdef DEBUGMSGERRORMF
      std::cout << "Send " << itemSent << " frames over the ring" << std::endl ;
#endif

      // Wait for the FIFO transmit is not running
      unsigned long watchdog = 0;
      fecSR0 = getFecRingSR0() ;
      while ( ((fecSR0 & FEC_SR0_TRARUN) || (!(fecSR0 & FEC_SR0_TRAEMPTY))) && (watchdog++ < loopInTimeWriteFrame) ) {
#ifdef TIMETOWAIT
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = TIMETOWAIT ;
	nanosleep (&req,NULL) ;
	//usleep (TIMETOWAIT) ;
#endif
	fecSR0 = getFecRingSR0() ;
      }

#ifdef RECALCULATETIMEOUT
      // --------------------------------------------------------------------------------------
      // Update the timeout
      if ( (loopInTimeWriteFrame == TIMEOUT) && (watchdog < TIMEOUT) ) {
	if (!watchdog) watchdog = 1000 ;
	loopInTimeWriteFrame = watchdog*2 ;
	
#  ifdef DEBUGMSGERROR
	std::cout << "loopInTimeWriteFrame local to MF = " << std::dec << loopInTimeWriteFrame << std::endl ;
#  endif
      }
#endif
            
      // Wait for the end of the send of the frane (FIFO transmit running)
      fecSR0 = getFecRingSR0() ;
      if (fecSR0 & FEC_SR0_TRARUN) {
	
#ifdef DEBUGMSGERROR_DISPLAYMULTIPLEFRAMES 
	std::cerr << __func__ << ": FIFO transmit running bit is always ON (SR0 = " << std::hex << fecSR0 << ")" << std::dec << std::endl ;
#endif    
	
	// Release all the transaction number used
	for (std::list<unsigned short>::iterator it = tnumUsed.begin() ; it != tnumUsed.end() ; it ++) {
	  releaseTransactionNumber(*it) ;
	}
	
#ifdef DEBUGMSGERRORMF
	for (unsigned int i = 0 ; i < wordFifo ; i ++)
	  std::cerr << "Word (" << i << ") = 0x" << std::hex << toBeTransmited[i] << std::endl ;
#endif

	RAISEFECEXCEPTIONHANDLER_INFOSUP ( DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON,
					   "FIFO transmit running bit is always on (CR0[3])",
					   CRITICALERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot()),
					   "FEC status register 0", fecSR0 ) ;
      }

      // **************************************************************************
      // Read all the frames from the FIFO receive and manage the differents errors
      // **************************************************************************
      getBlockFrames (forceAcknowledge, tnumSent, busy, cptRead) ;
      // Number of frames sent and acknowledged
      countFrame += tnumSent.size() ;
    }

    // End of the transaction, clear the table
    tnumSent.clear() ;

    /* **************************************************************************************************************************** */ 
    /*                                           This part concerns all the frames not the frames filled in the current buffer      */
    /*                                           Check that all the frames has been sent to finish the methods                      */
    /*                                           If not all the frames has been sent then wait for a timeout                        */
    /* **************************************************************************************************************************** */ 	
#ifdef DEBUGMSGERRORMF
    std::cout << "----------------------------> number of frames: " << countFrame << " / " << vAccessDevices.size() << " (number of read = " << cptRead << ")" << std::endl ;
#endif
    if (countFrame == vAccessDevices.size()) {
      // is it the end, no more frame to send and no more read request
      //std::cout << "Number of frames = " << countFrame << "/" << vAccessDevices.size() << std::endl ;
      //std::cout << "Number of read to be done = " << cptRead << std::endl ;
      if (cptRead == 0) finished = true ;
#ifdef F1STLEVELCHECK
      else {
	RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR,
				   "Some transactions has been not managed correctly (software problem)",
				   ERRORCODE ) ;
      }
#endif
    }

    /* **************************************************************************************************************************** */ 
    /*                                           Check the rings and clear errors and IRQ                                           */
    /* **************************************************************************************************************************** */ 
    fecSR0 = getFecRingSR0() ;
    // Clear the IRQ
    //if ((fecSR0 & FEC_SR0_PENDINGIRQ) || (fecSR0 & FEC_SR0_DATATOFEC))
    setFecRingCR1 (FEC_CR1_CLEARIRQ);
    // Check the status 0 of the FEC, if it is not correct then an error occurs
    if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) {
      
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_SR0NOTNOMINAL,
					 TSCFEC_SR0NOTNOMINAL_MSG + ": the next frames are not sent",
					 FATALERRORCODE,
					 buildFecRingKey(getFecSlot(),getRingSlot()), 
					 "FEC status register 0", fecSR0 ) ;
    }
  }

#ifdef DEBUGMSGERRORMF
  tscType16 fecSR0E = getFecRingSR0() ;
  std::cout << "===============================> FecRingDevice(" << std::dec << (int)getFecSlot() << "." << (int)getRingSlot() << ")::setBlockDevicesBltMode: end and the SR0 is " << std::hex << fecSR0E << std::endl ;
#endif
} 
