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

// fecRingReconfigure marche pour une CCU du milieu mais le scan juste derriere ne marche pas
// fecRingReconfigure marche de temps en temps pour la 1ere CCU
// fecRingReconfigure n'a pas pu etre tester pour la derniere CCU
// fecScanOrderOldCcu ne marche plus

// Semaphore dans writeFrame

// Ajouter un bit de status pour savoir qui a produit l'erreur avec:
// au niveau du FecExceptionHandler (en plus)
//     bit 0 => DEVICE    probleme de device   11111
//     bit 1 => CHANNEL   probleme de channel  11110
//     bit 2 => CCU       probleme de CCU      11100
//     bit 3 => RING      probleme de ring     11000
//     bit 4 => FEC       probleme de FEC      10000

// Verifier le bit de la clock polarity CR0<7> => 0x80

/** C include
 */ 
#include <stdio.h>      // For snprintf function
#include <sys/types.h>  // For open function
#include <sys/stat.h>   // For open function
#include <fcntl.h>      // For open function
#include <unistd.h>     // For close function
#include <string.h>     // For memcpy function

/** Driver
 */
#include "datatypes.h"
#include "glue.h"
#include "version.h"

/** Type definition
 */
#include "cmdDescription.h"

/** Class
 */
#include "FecDevice.h"

#define EMULATION

/** In order to display the frames send to the driver
 * Only for debugging
 */
//#define DEBUGMSGERROR_DISPLAYFRAME 
#ifdef DEBUGMSGERROR
#ifndef DEBUGMSGERROR_DISPLAYFRAME
#define DEBUGMSGERROR_DISPLAYFRAME
#endif 
#endif

/** Timeout after the reset, x 100 microseconds
 */
#define TIMEOUT 1

/**
 * Open the device driver corresponding to the slot given by argument
 * and initialise all the attributs (CCU hash table).
 * \param slot - fec slot
 * \exception FecExceptionHandler
 * \see FecDevice::setInitFecDevice
 */
FecDevice::FecDevice ( tscType16 slot ) {

  setInitFecDevice ( slot, true ) ;
}

/**
 * Open the device driver corresponding to the slot given by argument
 * and initialise all the attributs (CCU hash table).
 * \param slot - fec slot
 * \param init - initialise the FEC. If the SR0 is not correct then
 * reset the PLX and the FEC and try to emptyied the FIFO
 * \exception FecExceptionHandler
 * \see FecDevice::setInitFecDevice
 */
FecDevice::FecDevice ( tscType16 slot, bool init ) {

  setInitFecDevice ( slot, init ) ;
}

/**
 * Open the device driver corresponding to the slot given by argument
 * and initialise all the attributs (CCU hash table).
 * \param slot - fec slot
 * \param init - initialise the FEC. If the SR0 is not correct then
 * reset the PLX and the FEC and try to emptyied the FIFO
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_CANNOTOPENDEVICEDRIVER
 * <li>TSCFEC_SR0NOTNOMINAL
 * </ul>
 */
void FecDevice::setInitFecDevice ( tscType16 slot, bool init ) 
  throw (FecExceptionHandler) {

#ifndef EMULATION
  // Build the name of the device driver
  char devicename[100] ;
  snprintf (devicename, 100, DEVICEDRIVERNAME, slot) ;

  // Reconfiguration
  reconfigurationRunning_ = false ;

  // Open the device driver
  deviceDescriptor_ = open (devicename, (O_RDWR | O_NONBLOCK)) ;

  // Check error
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_CANNOTOPENDEVICEDRIVER_ERRORSEVERITY, 
                                TSCFEC_CANNOTOPENDEVICEDRIVER, 
                                setFecKey (slot),
                                TSCFEC_CANNOTOPENDEVICEDRIVER_UMSG, 
                                "FecDevice::FecDevice: " + 
                                TSCFEC_CANNOTOPENDEVICEDRIVER_DMSG ) ;
  }

  // Check the device driver version
  char ddVersion[DD_VERSION_STRING_SIZE] ;
  DD_TYPE_ERROR lcl_err = glue_fecdriver_get_driver_version(deviceDescriptor_, ddVersion) ;
  if ( (lcl_err != DD_RETURN_OK) || (strcmp (ddVersion, FEC_DRIVER_VERSION)) ) {
    
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
				TSCFEC_BADCOMPATIBILITY_ERRORSEVERITY,
				TSCFEC_BADCOMPATIBILITY, 
				setFecKey (slot),
				TSCFEC_BADCOMPATIBILITY_UMSG, 
				"FecDevice::FecDevice: " + 
				TSCFEC_BADCOMPATIBILITY_DMSG +
				": the version is not compatible between the driver loaded and the FEC C++ API") ;
  }
#endif

  // Fec Slot
  fecSlot_ = slot ;

  // Clear the error counting
  resetErrorCounters ( ) ;

  // Access
  accessNumber_ = 1 ;
  
  // Read CR0
  tscType32 SR0 = getFecSR0() ;    

  if (isFecSR0Correct(SR0)) {
    
    // Set the input/output as it is in SR0
    fecInput_  = isRingInputA(getFecCR0()) ;
    fecOutput_ = isRingOutputA(getFecCR0()) ;

    // Clock polarity
    clockPolarity_ = isInvertClockPolarity(getFecCR0()) ;
  }
  else {

    if (init) {

      // Clear errors
      setFecCR0 (DD_FEC_ENABLE_FEC) ; // Done in the FEC Reset method
      setFecCR1 (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS) ; // Clear errors    

      // Set the input/output as A
      fecInput_  = 0 ;
      fecOutput_ = 0 ;
      clockPolarity_ = false ;
      
      // Reset the PLX and the FEC
      plxReset ( ) ;
      fecReset ( ) ;

      // Re read after the reset the status
      SR0 = getFecSR0() ;      

      // Try to empty the FIFO is the status is not correct
      if (!isFifoReceiveEmpty(SR0)) emptyFifoReceive ( ) ;  
      
#ifdef F1STLEVELCHECK
      
      int timeout = 0 ;
      do {
        // Check the SR0 of the FEC
        if (! isFecSR0Correct(getFecSR0())) {
          
          // Set the control register of the FEC
          fecReset ( ) ;
#ifdef INITTTCRX
          initTTCRx ( ) ;
#endif
          // Set the input/output as A
          fecInput_  = 0 ;
          fecOutput_ = 0 ;
        
          setFecCR0 (DD_FEC_ENABLE_FEC) ; // Done in the FEC Reset method
          setFecCR1 (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS) ; // Clear errors    
        }
      
        sleep (1) ;
        
        timeout ++ ;
      }
      while ((! isFecSR0Correct(getFecSR0())) && (timeout < 10) ) ;

      if (! isFecSR0Correct(getFecSR0())) {
      
        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                    TSCFEC_SR0NOTNOMINAL_ERRORSEVERITY, 
                                    TSCFEC_SR0NOTNOMINAL, 
                                    setFecKey (slot), 
                                    "FEC status register 0", getFecSR0(),  
                                    TSCFEC_SR0NOTNOMINAL_UMSG, 
                                    "FecDevice::FecDevice: " + 
                                    TSCFEC_SR0NOTNOMINAL_DMSG ) ;
      }

#endif // End of reset at starting point

    }
    else 
      if (! isFecSR0Correct(getFecSR0()))
        fprintf (stderr, "The status register 0 of the FEC is not correct (0x%X)\n", getFecSR0()) ;    
  }

  // Scan the ring in order to find the CCUs and their status
  // For each ring
  for (int i = 0 ; i < MAXRING ; i ++) {

    // Initiliase to zero the ccu per rings
    nbCcuOnTheRing_[i] = 0 ;
    
    // For each CCU => initialise the values
    for (int j = 0 ; j < MAXCCU ; j ++) {
      
      ccuMapAccess_[i][j] = NULL ;
      ccuMapOrder_[i][j] = NULL ;
    }
    
#ifdef SCANCCUATSTARTING
#ifdef BROADCASTUSE
    fecScanOrderCcu25 (i) ;
#else
    fecScanOrderOldCcu (i) ;
#endif
#endif
    
  }
  
#ifndef EMULATION
  // Retreive FEC/PLX reset counters
  try {
    DD_TYPE_ERROR lcl_err = glue_fec_get_plx_reset_counter (deviceDescriptor_, &plxResetPerformed_) ;
    if (lcl_err != DD_RETURN_OK) {
      
      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  DEVICEDRIVERSUBSYSTEM,
                                  SOFTWAREERRORCODE,
                                  lcl_err,
                                  setFecKey(fecSlot_), 
                                  TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                  "FecDevice::FecDevice: " + 
                                  TSCFEC_FECPARAMETERNOTMANAGED_DMSG +
                                  ": code consistency error when I retreive the PLX reset counter") ;
    }
    
    lcl_err = glue_fec_get_fec_reset_counter (deviceDescriptor_, &fecResetPerformed_) ;
    if (lcl_err != DD_RETURN_OK) {
      
      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  DEVICEDRIVERSUBSYSTEM,
                                  SOFTWAREERRORCODE,
                                  lcl_err,
                                  setFecKey(fecSlot_), 
                                  TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                  "FecDevice::FecDevice: " + 
                                  TSCFEC_FECPARAMETERNOTMANAGED_DMSG +
                                  ": code consistency error when I retreive the FEC reset counter") ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    cerr << "------------------ WARNING --------------------" << endl ;
    cerr << e.what() << endl ;
    cerr << "-----------------------------------------------" << endl ;
  }
#endif
}


/** Close the device driver
 */
FecDevice::~FecDevice ( ) {

  // For each ring
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {

    // Delete all CCUs
    for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++) {

      delete p->second ; // Remove all the accesses for the CCU device
    }

    ccuMapAccess_[ring].clear() ;
  }

  // Close the device driver
  if (deviceDescriptor_ != -1) 
    close (deviceDescriptor_) ;
 }

/** 
 * \return the slot of the FEC
 */
int FecDevice::getFecSlot ( ) {

  return (fecSlot_) ;
}

/** Store the number of access for this device driver
 */
void FecDevice::addNewAccess ( ) {

  accessNumber_ ++ ;
}

/** Store the number of access for this device driver
 */
void FecDevice::removeAccess ( ) {
  
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
void FecDevice::basicCheckError ( keyType index ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(getFecKey(index)),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::basicCheckError: " + TSCFEC_SR0NOTNOMINAL_DMSG) ;
  }
#endif

#ifdef S2NDLEVELCHECK

  // Check the FEC slot
  if (fecSlot_ != getFecKey(index)) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::basicCheckError" + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": wrong FEC slot") ;
  }

#endif

  // Check the ring slot
  int ring = getRingKey (index) ; 
  if ( (ring < 0) || (ring >= MAXRING) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                buildCompleteKey(getFecKey(index),getRingKey(index),0,0,0),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::basicCheckError: "
                                + TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": wrong ring slot (must be 0)") ;
  }


  // ifdf F1STLEVELCHECK and ifndef SCANCCUATSTARTING then do the instructions
#ifdef F1STLEVELCHECK
  bool isCcu25 = connectCCU (index) ;
#else
#    ifndef SCANCCUATSTARTING
           connectCCU (index) ;
#    endif
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

	throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                              TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                              TSCFEC_FECPARAMETERNOTMANAGED, 
                              index,
                              TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                              "FecDevice::basicCheckError: " + 
                              TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                              " (CCU 25): wrong channel slot") ;
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

	throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                              TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                              TSCFEC_FECPARAMETERNOTMANAGED, 
                              index,
                              TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                              "FecDevice::basicCheckError: " + 
                              TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                              " (old CCU): wrong channel slot") ;
      }
  }

#endif

}

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
bool FecDevice::connectCCU ( keyType index ) throw (FecExceptionHandler) {

  // is a CCU 25 ?
  bool isCcu25 ;

  // Try to see if a CCU is connected 
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if ((ccu == ccuMapAccess_[getRingKey(index)].end()) || (ccu->second == NULL)) {

#ifdef SCANCCUATSTARTING

    // All the CCU are scanned in the constructor
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::connectCCU: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + ": the CCU is not present on the ring") ;

#else

    // Find the CCU type => old CCU or CCU 25
    // Try to get the value of the CRE to recognise an Old CCU and a CCU 25
    tscType32 CRE ;
    isCcu25 = getCcuType ( getRingKey(index), getCcuKey (index), &CRE ) ;
#ifdef EMULATION
    isCcu25 = true ;
#endif

    // Not connected => connect it
    ccuMapAccess_[getRingKey(index)][getCcuKey(index)] = new ccuDevice(getCcuKey(index),0,0,isCcu25) ;

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

#endif

  }
  else { // CCU Found is a CCU 25 ?
    isCcu25 = ccu->second->isCcu25() ;
  }

  return (isCcu25) ;
}

// -------------------------------------------------------------------------------------
//
//                                For the FEC registers 
//
// -------------------------------------------------------------------------------------

/** Send a frame over the ring. The frame is defined in the CCU documentation for
 * all the channels and CCU. The direct acknowledge is checked and returned in case of
 * error.
 * \param frame - frame to be sent. The size of the frame is DD_USER_MAX_MSG_LENGTH. At the
 * end of the method the frame returned is the direct acknoledge.
 * \param sizet - size of the direct acknowledge. (output). If the size length is
 * coded on two bytes in the frame, the sizet will be the correct size. For example
 * {0x81 0x0} in frame, the sizet returned will be 256 (0x0100).
 * \return Error Type of the device driver
 * \exception FecExceptionHandler:
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 * \warning The first word of the frame returned is the transaction number. The rest
 * of the frame is the direct acknowledge
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
 */
DD_TYPE_ERROR FecDevice::writeFrame ( tscType16 *frame, 
                                      tscType16 *sizet ) 
  throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, 
                                TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::writeFrame: " + 
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

  if (! isFecSR0Correct(SR0)) {
    
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
				TSCFEC_SR0NOTNOMINAL_ERRORSEVERITY, 
				TSCFEC_SR0NOTNOMINAL, 
				setFecKey (slot), 
				"FEC status register 0", getFecSR0(),  
				TSCFEC_SR0NOTNOMINAL_UMSG, 
				"FecDevice::writeFrame: " + 
				TSCFEC_SR0NOTNOMINAL_DMSG ) ;

  }
#endif

  // Check the size of the frame
  tscType16 realSize = frame[2] ;
  if (realSize & FEC_LENGTH_2BYTES) {
  
    realSize = (frame[2] & 0x7F) << 8 | frame[3] + 1 ;
    // +1 for the two byte words for length

    // Set the end of frame 
    frame[realSize+2] |= DD_FIFOTRA_EOFRAME_MASK ;
  }
  else {

    // Set the end of frame 
    frame[realSize+2] |= DD_FIFOTRA_EOFRAME_MASK ;
  }

#ifdef DEBUGMSGERROR_DISPLAYFRAME
  printf ("FecDevice::writeFrame ( { ") ;
  for (int i = 0 ; i < (realSize+3) ; i ++ )
    printf ("0x%x ", frame[i]) ;
  printf ("} )\n") ;
#endif

  // Send the frame to the device driver via the FIFO transmit
  DD_TYPE_ERROR lcl_err = 
#ifndef EMULATION
    glue_fec_write_frame( deviceDescriptor_, frame ) ;  
#else
  DD_RETURN_OK ;
#endif

  // Retreive the direct acknowledge
  realSize = frame[2] ;
  if (realSize & FEC_LENGTH_2BYTES) {
  
   realSize = (frame[2] & 0x7F) << 8 | frame[3] + 1 ;
   // +1 for the two byte words for length

   *sizet = realSize - 1 ;
  }
  else
    *sizet = realSize ;

#ifdef EMULATION
  // Change the acknowledge
  frame[realSize+3] &= 0xB000 ;
#endif 

#ifdef DEBUGMSGERROR_DISPLAYFRAME 
  if (lcl_err != DD_RETURN_OK) {

    char msg[1000] ;
    glue_fecdriver_get_error_message (lcl_err, msg) ;
    printf ("Error during the FecDevice::writeFrame:lcl_err = %d\n\t%s\n", lcl_err, msg) ;
  }
 
  printf ("FecDevice::Trame direct ( { ") ;
  for (int i = 0 ; i < (realSize+3) ; i ++ )
    printf ("0x%x ", frame[i]) ;
  printf ("} )\n") ;
#endif

  return (lcl_err) ;
}

/** Fill the FIFO transmit with a frame and send it if the order is given.
 * The frame is defined in the CCU documentation for all the channels and CCU. 
 * The direct acknowledge is checked and returned in case of error.
 * \param frame - frame to be sent. The size of the frame is DD_USER_MAX_MSG_LENGTH. At the
 * end of the method the frame returned is the direct acknoledge.
 * \param sizet - size of the direct acknowledge. (output). If the size length is
 * coded on two bytes in the frame, the sizet will be the correct size. For example
 * {0x81 0x0} in frame, the sizet returned will be 256 (0x0100).
 * \param sendIt - true means that the frame is sent over the ring. False means that the
 * frame is send to the FIFO receive but not send to the ring.
 * \return Error Type of the device driver
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 * \warning The first word of the frame returned is the transaction number. The rest
 * of the frame is the direct acknowledge
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
 * \bug the method to fill the FIFO does not more exists so the frame is send all the time
 * over the ring (sentIt = true)
 */
DD_TYPE_ERROR FecDevice::writeFrame ( tscType16 *frame, 
                                      tscType16 *sizet,
                                      bool sendIt) throw (FecExceptionHandler) {
  
#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::writeFrame (sendIt): " + 
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Check the size of the frame
  tscType16 realSize = frame[2] ;
  if (realSize & FEC_LENGTH_2BYTES) {
  
    realSize = (frame[2] & 0x7F) << 8 | frame[3] + 1 ;
    // +1 for the two byte words for length

    // Set the end of frame 
    frame[realSize+2] |= DD_FIFOTRA_EOFRAME_MASK ;
  }
  else {

    // Set the end of frame 
    frame[realSize+2] |= DD_FIFOTRA_EOFRAME_MASK ;
  }

#ifdef DEBUGMSGERROR_DISPLAYFRAME
  printf ("FecDevice::writeFrame ( { ") ;
  for (int i = 0 ; i < (realSize+3) ; i ++ )
    printf ("0x%x ", frame[i]) ;
  printf ("} )\n") ;
#endif

  // Send the frame to the device driver via the FIFO transmit
  DD_TYPE_ERROR lcl_err ;

#ifndef EMULATION
  if (sendIt)
    lcl_err = glue_fec_write_frame( deviceDescriptor_, frame ) ;
  else {
    cerr << "---------------- Warning -------------------" << endl ;
    cerr << "the function does not exists. If you need it" << endl ;
    cerr << "please email to fec-support@ires.in2p3.fr" << endl ;
    cerr << "--------------------------------------------" << endl ;
    lcl_err = glue_fec_write_frame( deviceDescriptor_, frame ) ;
  }
#endif

  // Retreive the direct acknowledge
  realSize = frame[2] ;
  if (realSize & FEC_LENGTH_2BYTES) {
  
   realSize = (frame[2] & 0x7F) << 8 | frame[3] + 1 ;
   // +1 for the two byte words for length

   *sizet = realSize - 1 ;
  }
  else
    *sizet = realSize ;

#ifdef DEBUGMSGERROR_DISPLAYFRAME 
  if (lcl_err != DD_RETURN_OK) {

    char msg[1000] ;
    glue_fecdriver_get_error_message (lcl_err, msg) ;
    printf ("Error during the FecDevice::writeFrame:lcl_err = %d\n\t%s\n", lcl_err, msg) ;
  }
 
  printf ("FecDevice::Trame direct ( { ") ;
  for (int i = 0 ; i < (realSize+3) ; i ++ )
    printf ("0x%x ", frame[i]) ;
  printf ("} )\n") ;
#endif

  return (lcl_err) ;
}

/** Read a frame in the FEC FIFO received. This frame can be a the answer to
 * a read command or the force acknowledge frame.
 * \param transaction - transaction number (value returned by the device driver)
 * \param frame - output frame
 * \return local error from the device driver
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 */
DD_TYPE_ERROR FecDevice::readFrame ( tscType16 transaction, 
                                          tscType16 *frame ) throw (FecExceptionHandler) {
#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::readFrame: " + 
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Transaction number
  frame[0] = transaction ;
 
  DD_TYPE_ERROR lcl_err = 
#ifndef EMULATION
    glue_fec_read_frame( deviceDescriptor_, frame ) ;
#else
  DD_RETURN_OK ;
#endif
  
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
  if (lcl_err == DD_RETURN_OK) {

    // Check the size of the frame
    tscType16 realSize = frame[2] ;
    if (realSize & FEC_LENGTH_2BYTES) {

      realSize = (frame[2] & 0x7F) << 8 | frame[3] + 1 ;
      // +1 for the two byte words for length
    }

    printf ("FecDevice::readFrame ( { ") ;
    for (int i = 0 ; i < (realSize+3) ; i ++ )
      printf ("0x%x ", frame[i]) ;
    printf ("} )\n") ;
  } 
  else {
    char msg[1000] ;
    glue_fecdriver_get_error_message (lcl_err, msg) ;
    printf ("Error during the FecDevice::readFrame:lcl_err = %d\n\t%s\n", lcl_err, msg) ;
  }
#endif

  return (lcl_err) ;
}


/** Method in order to retreive a control/status register from the FEC
 * \param registerValue - register to be accessed (command for the FEC)
 * \return value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
tscType16 FecDevice::getFecRegister ( tscType16 registerValue ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::getFecRegister: " + 
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Value to be read and error
  tscType16 value ;
#ifndef EMULATION 
  DD_TYPE_ERROR lcl_err ;
#else
  DD_TYPE_ERROR lcl_err = DD_RETURN_OK ;
#endif

  // Check if the register value given by param is correct
  switch (registerValue) {
  case DD_FEC_CTRL0_OFFSET: 
#ifndef EMULATION
    lcl_err =  glue_fec_get_ctrl0 ( deviceDescriptor_, &value ) ;
#else
    value = 0x1 | (fecOutput_ ? FEC_CR0_SELSEROUT : 0) | (fecInput_ ? FEC_CR0_SELSERIN : 0) ;
#endif
    break ;
  case DD_FEC_CTRL1_OFFSET: 
#ifndef EMULATION
    lcl_err =  glue_fec_get_ctrl1 ( deviceDescriptor_, &value ) ;
#else
    value = 0xFFFF ;
#endif
    break ;
  case DD_FEC_STAT0_OFFSET:
#ifndef EMULATION
    lcl_err =  glue_fec_get_status0 ( deviceDescriptor_, &value ) ;
#else
    value = 0x0C90 ;
#endif
    break ;
  case DD_FEC_STAT1_OFFSET: 
#ifndef EMULATION
    lcl_err =  glue_fec_get_status1 ( deviceDescriptor_, &value ) ;
#else
    value = 0x0 ;
#endif
    break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_), 
                                "FEC register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::getFecRegister: " + 
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command (cannot get a FEC register)") ;
  }

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                "FEC register", registerValue,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getFecRegister: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": FEC register access error") ;
  }

  return (value) ;
}

/** Method in order to set a FEC control register 
 * \param register - register to be accessed (command for the FEC)
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * <li>
 * </ul>
 */
void FecDevice::setFecRegister ( tscType16 registerValue, 
                                 tscType16 value ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::setFecRegister: " +
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

#ifndef EMULATION 
  // ERROR
  DD_TYPE_ERROR lcl_err ;
#else
  DD_TYPE_ERROR lcl_err = DD_RETURN_OK ;
#endif

  // Check if the register is correct
  switch (registerValue) {
  case DD_FEC_CTRL0_OFFSET:
#ifndef EMULATION 
    lcl_err =  glue_fec_set_ctrl0 ( deviceDescriptor_, value ) ;
#endif
    break ;
  case DD_FEC_CTRL1_OFFSET: 
#ifndef EMULATION
    lcl_err =  glue_fec_set_ctrl1 ( deviceDescriptor_, value ) ;
#endif
    break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,      
                                setFecKey(fecSlot_), 
                                "FEC register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setFecRegister: " +
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command (cannot set a FEC register)") ;
  }

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM,  
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                "FEC register", registerValue,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::setFecRegister: " +
                                TSCFEC_REGISTERACCESS_DMSG + ": FEC register access error") ;
  }
}


/** Method in order to set a control register from the FEC by read modify write operation
 * \param register - register to be accessed (command for the FEC)
 * \param mask - mask to be applied
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * <li>
 * </ul>
 */
void FecDevice::setFecRegister ( tscType16 registerValue, 
                                 tscType16 mask,  
                                 logicalOperationType op ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {
    
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::setFecRegister(RMW): " + 
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

#ifdef T3TDLEVELCHECK

  // Check if the register is correct
  switch (registerValue) {
  case DD_FEC_CTRL0_OFFSET: break ;
  case DD_FEC_CTRL1_OFFSET: break ;
    //case DD_FEC_STAT0_OFFSET: break ; // The status 0 and 1 cannot be set
    //case DD_FEC_STAT1_OFFSET: break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_), 
                                "FEC register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setFecRegister: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command (cannot set a FEC register (RMW))") ;
  }

#endif

  tscType16 value = getFecRegister ( registerValue ) ;

  switch (op) {
  case CMD_OR: value |= mask   ; break ;
  case CMD_XOR: value &= ~mask ; break ;
  case CMD_AND: value &= mask  ; break ;
  case CMD_EQUAL: break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_), 
                                "Logical operation", op,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setFecRegister(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG +
                                ": invalid logical operation") ;
  }

  // Perform the write on the register
  setFecRegister ( registerValue, value ) ;
}

/** Get the control register 0 of the FEC
 * \return the value read
 */
tscType16 FecDevice::getFecCR0 ( ) {

  return (getFecRegister(DD_FEC_CTRL0_OFFSET)) ;
}

/** Get the control register 1 of the FEC
 * \return the value read
 */
tscType16 FecDevice::getFecCR1 ( ) {

  return (getFecRegister(DD_FEC_CTRL1_OFFSET)) ;
}

/** Get the status register 0 of the FEC
 * \return the value read
 */
tscType16 FecDevice::getFecSR0 ( ) {

  return (getFecRegister(DD_FEC_STAT0_OFFSET)) ;
}

/** Get the status register 1 of the FEC
 * \return the value read
 */
tscType16 FecDevice::getFecSR1 ( ) {

  return (getFecRegister(DD_FEC_STAT1_OFFSET)) ;
}

/** Set the control register 0 of the FEC
 * \param value - value to be written
 */
void FecDevice::setFecCR0 ( tscType16 value ) {

#ifdef DEBUGMSGERROR_DISPLAYFRAME
  printf ("SetFecCR0 (0x%x)\n", value) ;
#endif

  setFecRegister(DD_FEC_CTRL0_OFFSET, value) ;
}

/** Set the control register 1 of the FEC
 * \param value - value to be written
 */
void FecDevice::setFecCR1 ( tscType16 value ) {

#ifdef DEBUGMSGERROR_DISPLAYFRAME
  printf ("SetFecCR1 (0x%x)\n", value) ;
#endif

  setFecRegister(DD_FEC_CTRL1_OFFSET, value) ;
}

/** empty the FIFO receive by reading all the values
 * \warning a timeout is implemented (4096 maximum)
 * \warning if the PLX interrupts are enable, the driver in normal case
 * will take care about all values. In some case of errors, the driver 
 * will not emptyied the FIFO due for example to a flood of interrupts 
 * and in some strange indermine cases.
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 * \thanks Wojciech BIALAS
 */
void FecDevice::emptyFifoReceive ( ) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::emptyFifoReceive: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  tscType16 value = 0 ;
#ifndef EMULATION
  DD_TYPE_ERROR lcl_err;
#else
  DD_TYPE_ERROR lcl_err = DD_RETURN_OK;
#endif
  tscType16 FecSR0 ; 

  int loopCnt = 0 ; 

  while (
         (!((FecSR0=getFecSR0()) & DD_FEC_REC_EMPTY) || (FecSR0 & DD_FEC_REC_RUN)) 
         && (loopCnt < RECEIVEFIFODEPTH)
         && (value != 0xFFFF) ) {

    // if (value==0xffff) fifo is empty : go out of loop  
#ifndef EMULATION
    lcl_err = glue_fec_get_fiforec_item (deviceDescriptor_, &value ) ;
#endif
     
    if (lcl_err != DD_RETURN_OK ) { 
      
    	throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  DEVICEDRIVERSUBSYSTEM,
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err,
                                  setFecKey(fecSlot_), 
                                  "FEC FIFO RECEIVE", DD_FEC_FIFOREC_OFFSET,
                                  TSCFEC_REGISTERACCESS_UMSG, 
                                  "FecDevice::emptyFifoReceive: " + 
                                  TSCFEC_REGISTERACCESS_DMSG + 
                                  ": FEC register access error") ;
    
    }
     
    loopCnt++; 

  } // end of while loop 
  
  if (loopCnt >= RECEIVEFIFODEPTH) { 

  	throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                "FEC FIFO RECEIVE", DD_FEC_FIFOREC_OFFSET,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::emptyFifoReceive: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": FEC recv. fifo could not be emptyied. ") ;
  
  } 
}

/** read a word in the FIFO receive
 * \return the word read
 * \warning if the SR0 signals that the FIFO receive is empty, 
 * an exception is raised
 * \warning if the PLX interrupts are enable, the driver in normal case
 * will take care about all values. In some case of errors, the driver 
 * will not emptyied the FIFO due for example to a flood of interrupts 
 * and in some strange indermine cases.
 */
tscType16 FecDevice::getFifoReceiveWord ( ) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::getFifoReceiveWord: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  tscType16 value = 0 ;
  DD_TYPE_ERROR lcl_err;
  tscType16 FecSR0 = getFecSR0 ( ) ; 

  // Check if the FIFO is empty
  if ( (FecSR0 & DD_FEC_REC_EMPTY) && (! FecSR0 & DD_FEC_REC_RUN) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_), 
                                "FEC FIFO RECEIVE", DD_FEC_FIFOREC_OFFSET,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getFifoReceiveWord: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": the FIFO receive is empty") ;
  }

#ifndef EMULATION
  // Read the word
  lcl_err = glue_fec_get_fiforec_item (deviceDescriptor_, &value ) ;
#else
  lcl_err = DD_RETURN_OK ;
  value = 0x0 ;
#endif
     
  if (lcl_err != DD_RETURN_OK ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                "FEC FIFO RECEIVE", DD_FEC_FIFOREC_OFFSET,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getFifoReceiveWord: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": FEC register access error") ;
  }
    
  return (value) ;
}

/** read a word in the FIFO return
 * \return the word read
 * \warning if the SR0 signals that the FIFO receive is empty, 
 * an exception is raised
 * \warning This FIFO is taken care by the firmware of the FEC for
 * retransmission
 */
tscType16 FecDevice::getFifoReturnWord ( ) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::getFifoReturnWord: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  tscType16 value = 0 ;
  DD_TYPE_ERROR lcl_err;
  tscType16 FecSR0 = getFecSR0 ( ) ; 

  // Check if the FIFO is empty
  if (FecSR0 & DD_FEC_RET_EMPTY) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_), 
                                "FEC FIFO RETURN", DD_FEC_FIFOREC_OFFSET,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getFifoReturnWord: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": the FIFO return is empty") ;
  }

#ifndef EMULATION
  // Read the word
  lcl_err = glue_fec_get_fiforet_item (deviceDescriptor_, &value ) ;
#else
  lcl_err = DD_RETURN_OK ;
  value = 0x0 ;
#endif
     
  if (lcl_err != DD_RETURN_OK ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                "FEC FIFO RETURN", DD_FEC_FIFOREC_OFFSET,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getFifoReturnWord: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": FEC register access error") ;
  }
    
  return (value) ;
}

/** read a word in the FIFO transmit
 * \return the word read
 * \warning if the SR0 signals that the FIFO transmit is empty, 
 * an exception is raised
 * \warning if the PLX interrupts are enable, the driver in normal case
 * will take care about all values. In some case of errors, the driver 
 * will not emptyied the FIFO due for example to a flood of interrupts 
 * and in some strange indermine cases.
 */
tscType16 FecDevice::getFifoTransmitWord ( ) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::getFifoTransmitWord: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  tscType16 value = 0 ;
  DD_TYPE_ERROR lcl_err;
  tscType16 FecSR0 = getFecSR0 ( ) ; 

  // Check if the FIFO is empty
  if ( (FecSR0 & DD_FEC_TRA_EMPTY) && (! FecSR0 & DD_FEC_TRA_RUN) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_), 
                                "FEC FIFO TRANSMIT", DD_FEC_FIFOTRA_OFFSET,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getFifoTransmitWord: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": the FIFO transmit is empty") ;
  }

#ifndef EMULATION
  // Read the word
  lcl_err = glue_fec_get_fifotra_item (deviceDescriptor_, &value ) ;
#else
  lcl_err = DD_RETURN_OK ;
  value = 0x0 ;
#endif
     
  if (lcl_err != DD_RETURN_OK ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                "FEC FIFO TRANSMIT", DD_FEC_FIFOTRA_OFFSET,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getFifoTransmitWord: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": FEC register access error") ;
  }
    
  return (value) ;
}


/** FEC Reset
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 * \warning The FEC reset FEC/CCU (but not the modules done via PIA channels for the CMS Tracker)
 */
void FecDevice::fecReset ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::fecReset: " +  
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  //DD_TYPE_ERROR lcl_err = glue_fec_soft_reset(deviceDescriptor_);
  setFecCR0 ( DD_FEC_RESET_OUT | DD_FEC_ENABLE_FEC ) ;

  // For each ring delete all CCUs, the ring is reseted
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {

    // Delete all CCUs
    for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++)
      delete p->second ; // Remove all the accesses for the CCU device

    ccuMapAccess_[ring].clear() ;
  }

  // Wait for the correct status
  int timeout = TIMEOUT ;
  while (! isFecSR0Correct(getFecSR0()) && (timeout-- >= 0)) usleep(100) ;

#ifndef EMULATION
  // Empty the buffer for the cast from 32 bits to 16 bits
  glue_fec_get_fiforec_item ( deviceDescriptor_, NULL ) ;
#endif

  /* ------------------------------------------------------------
  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_),
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::fecReset: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": FEC reset error") ;
  }
  --------------------------------------------------------------- */

  // Set the input/output as A
  fecInput_  = 0 ;
  fecOutput_ = 0 ;

  setFecCR0 (DD_FEC_ENABLE_FEC) ; // Enable FEC
  setFecCR1 (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS) ; // Clear errors

  // Reset counter internal 
  fecResetPerformed_ ++ ;
}

/** FEC Reset on ring B
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 * \warning The FEC reset FEC/CCU (but not the modules done via PIA channels for the CMS Tracker)
 */
void FecDevice::fecResetRingB ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::fecReset: " +  
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // reset ring B
  setFecCR0 ( FEC_CR0_RESETRINGB | DD_FEC_ENABLE_FEC ) ;

  // Wait for the correct status
  int timeout = TIMEOUT ;
  while (! isFecSR0Correct(getFecSR0()) && (timeout-- >= 0)) usleep(100) ;

#ifndef EMULATION
  // Empty the buffer for the cast from 32 bits to 16 bits
  glue_fec_get_fiforec_item ( deviceDescriptor_, NULL ) ;
#endif

  // For each ring delete all CCUs, the ring is reseted
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {

    // Delete all CCUs
    for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++)
      delete p->second ; // Remove all the accesses for the CCU device

    ccuMapAccess_[ring].clear() ;
  }

  // Set the input/output as A
  fecInput_  = 0 ;
  fecOutput_ = 0 ;

  setFecCR0 (DD_FEC_ENABLE_FEC) ; // Enable FEC
  setFecCR1 (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS) ; // Clear errors

  // Reset counter internal 
  fecResetPerformed_ ++ ;
}

/** Reset FEC and reconfigure the ring in order to retreive the previous state
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::fecReset ( bool reconf ) throw (FecExceptionHandler) {

  if (! reconf) fecReset ( ) ;
  else {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
    if (deviceDescriptor_ == -1) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                  TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                  setFecKey(fecSlot_),
                                  TSCFEC_FECDDNOTOPEN_UMSG, 
                                  "FecDevice::fecReset(bool): " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
    }

#endif

#ifdef INITIALIAZE_FEC
    // Set the control register of the FEC to the correct value 
#ifdef INITTTCRX
    setFecCR0 (DD_FEC_ENABLE_FEC | DD_FEC_RESET_TTCRX | DD_FEC_RESET_OUT) ; // Reset TTCRx and FEC
#else
    setFecCR0 (DD_FEC_ENABLE_FEC | DD_FEC_RESET_OUT) ; // Reset FEC
#endif
    setFecCR0 (DD_FEC_ENABLE_FEC) ; // Enable FEC
    setFecCR1 (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS)    ; // Clear errors
#endif

    // Wait for the correct status
    int timeout = TIMEOUT ;
    while (! isFecSR0Correct(getFecSR0()) && (timeout-- >= 0)) usleep(100) ;

#ifndef EMULATION
    // Empty the buffer for the cast from 32 bits to 16 bits
    glue_fec_get_fiforec_item ( deviceDescriptor_, NULL ) ;
#endif

    // Set the input/output of the FEC
    tscType16 fecCR0 = DD_FEC_ENABLE_FEC ;
    if (fecInput_  != 0) fecCR0 |= DD_FEC_SEL_SER_IN ;
    if (fecOutput_ != 0) fecCR0 |= DD_FEC_SEL_SER_OUT ;

    setFecCR0 (fecCR0) ;

    // Wait for the correct status
    timeout = TIMEOUT ;
    while (! isFecSR0Correct(getFecSR0()) && (timeout-- >= 0)) usleep(100) ;

    // Make the configuration of the CCU
    ccuChannelReConfiguration ( ) ;

    // Reset counter internal 
    fecResetPerformed_ ++ ;
  }
}

/** Create a new token in the FEC after a timeout
 * \param isFec - FEC implied ?
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 */
void FecDevice::fecRelease ( bool isFec ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::fecRelease(bool): " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Generate the token
  if (! isFec) {
    setFecCR1 ( DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS ) ; // Clear errors
    setFecCR0 ( DD_FEC_ENABLE_FEC ) ;
    setFecCR0 ( 0x3 ) ;
    setFecCR0 ( 0x0 ) ;
    setFecCR1 ( 0x4 ) ;
  } 
  else {
    setFecCR1 ( 0x4 ) ;
    usleep (100) ;
    setFecCR1 ( 0x4 ) ;
  }
}

/** Create a new token in the FEC after a timeout. If a ring is opened the FEC release make possible the send of the frame. So for example if you want to recover a badly configure ring, you must write the frame and after make a FEC release to forsee the send of the frame over the ring. When the frame is sent, you do not have any garentee that at the end of the release the frame is sent. You must wait on the FIFO transmit is not running. This operation is done in this method.
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 * \warning the FEC release is made two times due to the hardware (?)
 */
void FecDevice::fecRelease ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::fecRelease: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Generate the token
  setFecCR1 ( 0x4 ) ;

  // Wait for the FIFO transmit is not running
  long watchdog = 0;
  while ( (getFecSR0() & FEC_SR0_TRARUN) && (watchdog++ < 1000) ) {
    usleep (0) ;
  }

  // No anwer
  if (getFecSR0() & FEC_SR0_TRARUN) {
    
#ifdef DEBUGMSGERROR_DISPLAYFRAME 
    cerr << "FecDevice::fecRelease: FIFO transmit running bit is always ON (SR0 = " << hex << getFecSR0() << ")" << dec << endl ;
#endif    
    
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
				DEVICEDRIVERSUBSYSTEM,
				TSCFEC_REGISTERACCESS_ERRORSEVERITY,
				TSCFEC_REGISTERACCESS,
				getFecSR0(),
				TSCFEC_REGISTERACCESS_UMSG, 
				"FecRingDevice::fecRelease: " +
				TSCFEC_REGISTERACCESS_DMSG + 
				"FIFO transmit running bit is always ON") ;
  }
}

/** initialise the TTCRx
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::initTTCRx ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::initTTCRx: " +  
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  //DD_TYPE_ERROR lcl_err = glue_fec_init_ttcrx(deviceDescriptor_);
  setFecCR0 ( FEC_CR0_RESETTCRX ) ;
  setFecCR0 ( FEC_CR0_ENABLEFEC ) ;

//   if (lcl_err != DD_RETURN_OK) {

//     throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
//                                 DEVICEDRIVERSUBSYSTEM,
//                                 TSCFEC_REGISTERACCESS_ERRORSEVERITY,
//                                 lcl_err,
//                                 setFecKey(fecSlot_),
//                                 TSCFEC_REGISTERACCESS_UMSG, 
//                                 "FecDevice::initTTCRx: " +  
//                                 TSCFEC_REGISTERACCESS_DMSG + 
//                                 ": TTCRx initialisation error") ;
//   }
}

// -------------------------------------------------------------------------------------
//
//                                For the ring reconfiguration/detection
//
// -------------------------------------------------------------------------------------

/** This method scan the ring connected to the fec (by broadcast operation)
 * and store the different value of the CCU in the map table.
 * The order of the CCUs are store in the ccuMapOrder_.
 * \param ring - ring to be scanned
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>
 * </ul>
 * \bug The broadcast mode must replace the loop on the CCU address.
 * \warning This method is done normally when the FecDevice is created
 * \warning When you use this method all the CCU must be CCU 25
 */
void FecDevice::fecScanOrderCcu25 ( tscType16 ring ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::fecScanOrderCcu25: " +  
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Delete all CCUs before the scan to avoid an overlap
  for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++) {
    
    delete p->second ; // Remove all the accesses for the CCU device
  }
  ccuMapAccess_[ring].clear() ;

  // Disable the plx interrupts
  setEnableIRQ(false) ;

  // Frame to be sent
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = {BROADCAST_ADDRESS, 0x0000, 0x0003, 0x0000, 0x0001, CMD_CCUREADCRA} ; // frame used for broadcast

#ifndef EMULATION
  tscType16 value ;
  tscType16 temporary[RECEIVEFIFODEPTH] ;
#endif

  tscType16 sizet = 3 ;

  // All the CCUs found
  tscType16 ccuAddresses[MAXCCU]; // table where will be store the position of each ccu in the ring (index 0 <=> position 1) 

  // Empty the buffer for the cast from 32 bits to 16 bits
  glue_fec_get_fiforec_item ( deviceDescriptor_, NULL ) ;

  // Write the broadcast frame
#ifndef EMULATION
  DD_TYPE_ERROR lcl_err = 
#endif
    writeFrame ( frame, &sizet ) ;

  // This part is used to be sure that the frame is sent and some data was received
  // Wait while the FIFO transmit is running
  int timeoutWait = 10 ;
  while ( (getFecSR0() & FEC_SR0_TRARUN) && (timeoutWait -- > 0) ) {
    usleep(10) ;
  }

  // Wait while the FIFO receive is empty
  timeoutWait = 10 ;
  while ( (getFecSR0() & FEC_SR0_RECEMPTY) && (timeoutWait -- > 0) ) {
    usleep(10) ;
  }

  // Wait while the FIFO receive is running
  timeoutWait = 10 ;
  while ( (getFecSR0() & FEC_SR0_RECRUN) && (timeoutWait -- > 0) ) {
    usleep(10) ;
  }

  // Number of CCU
  int nbccu = 0 ;

#ifndef EMULATION
  // timeout
  int timeout = 0 ;

  // Start to see the CCUs
  do {

    int pos = 0 ;

    do {
      lcl_err = glue_fec_get_fiforec_item ( deviceDescriptor_, &value ) ;

      if (lcl_err == DD_RETURN_OK) {
        temporary[pos] = value ;
        pos ++ ;
      }

      timeout ++ ;
    }
    while ( (lcl_err == DD_RETURN_OK) && (!(value & DD_FIFOTRA_EOFRAME_MASK)) && (timeout < RECEIVEFIFODEPTH) ) ;

    if (temporary[0] == 0) { // Direct ack of the broadcast frame sent

      ccuAddresses[nbccu] = temporary[1] ;
      nbccu += 1 ;
    }

    timeout ++ ;
  } 
  while ( (lcl_err == DD_RETURN_OK) && (value != 0xFFFF) && (timeout < RECEIVEFIFODEPTH) ) ;

  // timeout problem
  if (timeout == RECEIVEFIFODEPTH) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM,
                                SOFTWAREERRORCODE,
                                TSCFEC_CODECONSISTENCYERROR,
                                setFecKey(fecSlot_) , 
                                TSCFEC_CODECONSISTENCYERROR_UMSG,
                                "FecDevice::fecScanOrderCcu25: " + 
                                TSCFEC_CODECONSISTENCYERROR_DMSG +
                                ": code consistency error when I try to scan the ring for CCU") ; 
  }
#else

  ccuAddresses[nbccu++] = 0x1 ;
  ccuAddresses[nbccu++] = 0x2 ;
  ccuAddresses[nbccu++] = 0x10 ;
  ccuAddresses[nbccu++] = 0x11 ;
  ccuAddresses[nbccu++] = 0x20 ;
  ccuAddresses[nbccu++] = 0x21 ;

#endif

  // Clear errors
  setFecCR1 (0x3) ;

  // Re-enable the plux IRQ
  setEnableIRQ(true) ;

  // Check that we have no the same address for the CCUs
  for (int i = 0 ; i < nbccu-1 ; i ++) 
    for (int j = i + 1 ; j < nbccu ; j++) 
      if (ccuAddresses[i] == ccuAddresses[j]) {
#ifdef DEBUGMSGERROR
        cerr << "Several CCU have the same address " << hex << ccuAddresses[i] << dec << endl ;
#endif

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                    FECSUPERVISORSUBSYSTEM,
                                    SOFTWAREERRORCODE,
                                    TSCFEC_CODECONSISTENCYERROR,
                                    buildCompleteKey(fecSlot_,ring,ccuAddresses[i],0,0), 
                                    TSCFEC_CODECONSISTENCYERROR_UMSG,
                                    "FecDevice::fecScanOrderCcu25: " + 
                                    TSCFEC_CODECONSISTENCYERROR_DMSG +
                                    ": code consistency error when I try to scan the ring for CCU, several CCUs have the same address") ; 
      }

  // Add the CCU to
  for (int i = 0 ; i < nbccu ; i ++) {

    // Find the CCU type => old CCU or CCU 25
    // Try to get the value of the CRE to recognise an Old CCU and a CCU 25
    tscType32 CRE ;
    bool isCcu25 = false ;

    try {
      isCcu25 = getCcuType ( ring, ccuAddresses[i], &CRE ) ;
    }
    catch (FecExceptionHandler &e) { }

#ifdef DEBUGMSGERROR
    if (isCcu25)
      printf ("Found a CCU 25 at address 0x%x\n", ccuAddresses[i]) ;
    else 
      printf ("Found an old CCU at address 0x%x\n", ccuAddresses[i]) ;
#endif

    // Check if the CCU was found before
    if (ccuMapAccess_[ring][ccuAddresses[i]] == NULL) {

      // Must be a CCU 25 for the broadcast mode
       ccuDevice *ccu = new ccuDevice (ccuAddresses[i],0,0,isCcu25) ;
      ccuMapAccess_[ring][ccuAddresses[i]] = ccu ;
      ccuMapOrder_[ring][i] = ccu ;
    }
  }

  nbCcuOnTheRing_[ring] = nbccu ;
}

/** This method scan the ring connected to the fec (by broadcast operation)
 * and store the different value of the CCU in the map table.
 * The order of the CCUs are store in the ccuMapOrder_.
 * \param ring - ring to be scanned
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>
 * </ul>
 * \bug The broadcast mode must replace the loop on the CCU address.
 * \warning This method is done normally when the FecDevice is created
 * \warning When you use this method all the CCU must be old CCUs
 */
void FecDevice::fecScanOrderOldCcu ( tscType16 ring ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::fecScanOrderOldCcu: " +  
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Delete all CCUs before the scan to avoid an overlap
  for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++) {
    
    delete p->second ; // Remove all the accesses for the CCU device
  }

  ccuMapAccess_[ring].clear() ;

  // Frames to be sent
  tscType16 frame2Send[DD_USER_MAX_MSG_LENGTH] ; // frame which be used as one of the parameters of the method writeFrame() 

  tscType16 frameInitCcu[DD_USER_MAX_MSG_LENGTH] = {0x0001,0x0000,0x0004,0x0000,0x0000,0x0000,INITOLDCCUCRA}; // frame which initialise the CCU

  tscType16 ccuAddresses[MAXCCU]; // table where will be store the position of each ccu in the ring (index 0 <=> position 1) 

  tscType16 sizet = 4 ;
  DD_TYPE_ERROR lcl_err;

  int nbccu = 0 ;  

  // Try to init CCU with the address i
  for( int i = 0x1 ; i < 0x80 ; i++ ) {

    // Reinitialisation of the frame (for initialisation of ccu) to write   
    for( int k = 0 ; k < 7 ; k++ ) {
      frame2Send[k] = frameInitCcu[k];
    }

    sizet = 4; 
    frame2Send[0] = i;

    // Send the frame
    lcl_err = writeFrame(frame2Send,&sizet);
    
    // Check the answer
    if ( lcl_err == DD_RETURN_OK ) { // if the address i is assigned to a ccu 

      ccuAddresses[nbccu] = i;
      nbccu++;
    }
  }

  // Store the number of CCU found in the ring
  nbCcuOnTheRing_[ring] = nbccu ;

  // ????????????????????????
  //#ifdef DEBUGMSGERROR
  if ( nbccu == 0 )
    printf("Scan Ring:no CCU found\n") ;
  else {
    printf ("Scan Ring:CCU found at address(es): ") ;
    for (int z = 0 ; z < nbccu ; z ++) 
      printf ("0x%x ", ccuAddresses[z]) ;
    printf ("\n");
  }
  //#endif

  // Check the number of CCU on the ring
  if (nbccu == 1) {  // One CCU => create the device

    // Find the CCU type => old CCU or CCU 25
    // Try to get the value of the CRE to recognise an Old CCU and a CCU 25
    tscType32 CRE ;
    bool isCcu25 = getCcuType ( ring, ccuAddresses[0], &CRE ) ;

#ifdef DEBUGMSGERROR
    if (isCcu25)
      printf ("Scan Ring: Found 1 CCU 25 0x%X\n", ccuAddresses[0]) ;
    else
      printf ("Scan Ring: Found 1 Old CCU 0x%X\n", ccuAddresses[0]) ;
#endif

    // Check if the CCU was found before
    if (ccuMapAccess_[ring][ccuAddresses[0]] == NULL) {
      ccuDevice *ccu = new ccuDevice ( ccuAddresses[0], 0, 0, isCcu25 ) ;
      ccuMapAccess_[ring][ccuAddresses[0]] = ccu ;
      ccuMapOrder_[ring][0] = ccu ;
    }
  }

  // Several CCU => find the order
  else if ( nbccu > 1 ) {

    int nbccuorder = 0 ;

#ifdef DEBUGMSGERROR
    printf("start of the search for the order of the CCUs\n") ;
#endif

    tscType16 frame_input_A [DD_USER_MAX_MSG_LENGTH] = { 0x0, 0x0000, 0x0004, 0x0000, 0x0000, 0x0002, 0x8000} ;
    tscType16 frame_input_B [DD_USER_MAX_MSG_LENGTH] = { 0x0, 0x0000, 0x0004, 0x0000, 0x0000, 0x0002, 0x8001} ;
    tscType16 frame_output_A[DD_USER_MAX_MSG_LENGTH] = { 0x0, 0x0000, 0x0004, 0x0000, 0x0000, 0x0002, 0x8000} ;
    tscType16 frame_output_B[DD_USER_MAX_MSG_LENGTH] = { 0x0, 0x0000, 0x0004, 0x0000, 0x0000, 0x0002, 0x8002} ;
    tscType16 frame_test    [DD_USER_MAX_MSG_LENGTH] = { 0x0, 0x0000, 0x0003, 0x0000, 0x0000, 0x8010     } ;
    tscType16 temp ;

    do {

      // Reset to be in the correct mode
      fecReset( ) ;
      
      if ( nbccuorder == 0 ) { // Search for the 2 first ccus

        setFecCR0 ( DD_FEC_ENABLE_FEC | DD_FEC_SEL_SER_OUT ) ; // fec in output B

        //#ifdef DEBUGMSGERROR
        printf ("set the FEC to output B\n") ;
        //#endif
  
      }
      else {

        //#ifdef DEBUGMSGERROR
        printf ("set the ccu %x to output B\n", ccuAddresses[nbccuorder-1]) ;
        //#endif

        frame_output_B[0] = ccuAddresses[nbccuorder-1] ; // CCU in output B
          
        for( int k = 0 ; k < 7 ; k++ ) {
          frame2Send[k] = frame_output_B[k] ;
        }

        sizet = 4 ;
                  
        lcl_err = writeFrame(frame2Send,&sizet) ; // Send the frame
        fecRelease ( ) ;
      }

      bool found = false ;

      for( int ccuVal0 = nbccuorder ; (ccuVal0 < nbccu) && !found ; ccuVal0++ ) {

        // ????????????????????,,
        //#ifdef DEBUGMSGERROR
        printf ("set the ccu %x to input B\n", ccuAddresses[ccuVal0]) ;
        //#endif

        frame_input_B[0] = ccuAddresses[ccuVal0] ; //CCU in input B
        for( int k = 0 ; k < 7 ; k++ ) {
          frame2Send[k] = frame_input_B[k] ;
        }
        sizet = 4 ;
        lcl_err = writeFrame(frame2Send,&sizet) ; // Send the frame
        fecRelease ( ) ;

        // Check if the ring is closed
        frame_test[0] = ccuAddresses[ccuVal0] ;
        for( int k = 0 ; k < 7 ; k++ ) {
          frame2Send[k] = frame_test[k] ;
        }
        sizet = 3 ;
        lcl_err = writeFrame(frame2Send,&sizet) ; // Send the frame
        fecRelease ( ) ;
        
        if ( (lcl_err == DD_RETURN_OK) && 
             !(frame2Send[5] & FECACKERROR ) ) { // if the ccu doesn't get the frame

          // ????????????????????
          //#ifdef DEBUGMSGERROR
          printf("--the ring is close-- => Ccu %x\n", ccuAddresses[ccuVal0]) ;
          //#endif

          // Search which CCU is not in the ring
          for ( int ccuVal1 = nbccuorder ; (ccuVal1 < nbccu) && !found; ccuVal1++ ) {
            
            if (ccuAddresses[ccuVal1] != ccuAddresses[ccuVal0]) {

              frame_test[0] = ccuAddresses[ccuVal1] ;
              for ( int k = 0 ; k < 7 ; k++ ) {
                frame2Send[k] = frame_test[k] ;
              }
              sizet = 3 ;
              lcl_err = writeFrame(frame2Send,&sizet) ; // Send the frame
              
              // if the ccu doesn't get the frame => the CCU missing is found
              if ( (lcl_err != DD_RETURN_OK) || (frame2Send[5] & FECACKERROR ) ) { 
                
                // ????????????????????,,
                //#ifdef DEBUGMSGERROR
                printf ("Ccu %x\n", ccuAddresses[ccuVal1]) ;
                printf ("set the ccu %x to input A\n", ccuAddresses[ccuVal0]) ;
                //#endif

                fecRelease ( ) ;
                frame_input_A[0] = ccuAddresses[ccuVal0] ; //CCU in input A
		
                for( int k = 0 ; k < 7 ; k++ ) {
                  frame2Send[k] = frame_input_A[k] ;
                }
                sizet = 4 ;
                lcl_err = writeFrame(frame2Send,&sizet) ; // Send the frame
                fecRelease ( ) ;

                // set ccu in the right order
                temp = ccuAddresses[nbccuorder+1] ;
                ccuAddresses[nbccuorder+1] = ccuAddresses[ccuVal0] ;
                ccuAddresses[ccuVal0] = temp ;
                  
                temp = ccuAddresses[nbccuorder] ;
                ccuAddresses[nbccuorder] = ccuAddresses[ccuVal1] ;
                ccuAddresses[ccuVal1] = temp ;
		
                if (nbccuorder == 0) { // Search for the 2 first ccus
                  setFecCR0 ( DD_FEC_ENABLE_FEC ) ; // fec in input and output A

                  // ????????????????????
                  //#ifdef DEBUGMSGERROR
                  printf ("set the FEC to output A\n") ;
                  //#endif
                }
                else {

                  // ?????????????????????????
                  //#ifdef DEBUGMSGERROR
                  printf ("Set the CCU %x to output A\n", ccuAddresses[nbccuorder-1]) ;
                  //#endif
                  
                  fecRelease ( ) ;
                  frame_output_A[0] = ccuAddresses[nbccuorder-1] ; //CCU in output A

                  for( int k = 0 ; k < 7 ; k++ ) {
                    frame2Send[k] = frame_output_A[k] ;
                  }
                  sizet = 4 ;
                  lcl_err = writeFrame(frame2Send,&sizet) ; // Send the frame
                  fecRelease ( ) ;
                }
                
                found = true ;
                nbccuorder = nbccuorder + 2 ;                          
              }
            } 
          }
        } 
        else {

          // ?????????????????????????
          //#ifdef DEBUGMSGERROR
          printf ("set the ccu %x to input A\n", ccuAddresses[ccuVal0]) ;
          //#endif
	    
          fecRelease ( ) ;
          frame_input_A[0] = ccuAddresses[ccuVal0] ; //CCU in input A
          
          for( int k = 0 ; k < 7 ; k++ ) {
            frame2Send[k] = frame_input_A[k] ;
          }
          sizet = 4 ;
          lcl_err = writeFrame(frame2Send,&sizet) ; // Send the frame
          fecRelease ( ) ;
        }
      }
    } while ( nbccuorder < (nbccu-1) ) ;
 
    // Create all the object for each ccu
    for ( int i = 0 ; i < nbccu ; i++ ) {

      // Find the CCU type => old CCU or CCU 25
      // Try to get the value of the CRE to recognise an Old CCU and a CCU 25
      tscType32 CRE ;
      bool isCcu25 = getCcuType ( ring, ccuAddresses[0], &CRE ) ;

      // ??????????????????????????????????
      //#ifdef DEBUGMSGERROR
      if (isCcu25)
        printf ("Scan Ring: Found 1 CCU 25 0x%X\n", ccuAddresses[i]) ;
      else
        printf ("Scan Ring: Found 1 Old CCU 0x%X\n", ccuAddresses[i]) ;
      //#endif

      // Check if the CCU was found before
      if (ccuMapAccess_[ring][ccuAddresses[i]] == NULL) {
        ccuDevice *ccu = new ccuDevice ( ccuAddresses[i], 0, 0, isCcu25 ) ;
        ccuMapAccess_[ring][ccuAddresses[i]] = ccu ;
        ccuMapOrder_[ring][i] = ccu ;
      }
    }

#ifdef DEBUGMSGERROR
    printf ("Order of the CCU : ") ;
    for (int z = 0 ; z < nbccu ; z ++)
      printf ("The CCU%d is at the address : 0x%x ", z+1, ccuAddresses[z]) ;
    printf ("\n") ;
#endif 
  }
}


/** This method reconfigure the ring due to a CCU fault.
 * \param index - ring to be reconfigure
 * \bug this method does not remove the fault CCU in the hash table.
 * \bug this method must be debug for the 1st and the last CCU
 * \warning this method need the order of the CCU
 */
void FecDevice::fecRingReconfigure ( keyType index ) {

  fecRingReconfigure ( getRingKey (index) ) ;
}

/** This method reconfigure the ring due to a CCU fault.
 * \param ring - ring to be reconfigure
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
void FecDevice::fecRingReconfigure ( tscType16 ring ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::fecRingReconfigure: " +  
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  tscType16 fecSR0Value = getFecSR0() ;
  tscType16 fecCR0Value = getFecCR0() ;    

  // Ring is ok ?
  if (isFecSR0Correct(fecSR0Value)) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_SR0NOTNOMINAL_ERRORSEVERITY, 
                                TSCFEC_SR0NOTNOMINAL, 
                                setFecKey (fecSlot_), 
                                "FEC status register 0", getFecSR0(),  
                                TSCFEC_SR0NOTNOMINAL_UMSG, 
                                "FecDevice::fecRingReconfigure: " +  
                                TSCFEC_SR0NOTNOMINAL_DMSG + 
                                " : ring reconfiguration cannot be done due, the FEC status register 0 is not correct") ;

  }

  // reset ring B
  fecResetRingB() ;

  // Find the type for the ring of CCUs (assume that the ring manage the same type of CCUs
  bool isCcu25 = ccuMapAccess_[ring].begin()->second->isCcu25( ) ;

  if (! isCcu25) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_) | setRingKey(ring),
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::fecRingReconfigure: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command for old CCUs, the ring cannot be reconfigured") ;
  }

  if (nbCcuOnTheRing_[ring] <= 1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_) | setRingKey(ring), 
                                "number of CCUs", nbCcuOnTheRing_[ring],
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::fecRingReconfigure: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": not enough CCUs (0 or 1 CCU is available)") ;
  }

  bool result = false ; // the reconfiguration is ok ?
  unsigned int i, j = 1 ; // start of the standard reconfiguration

  // test if the first ccu is still in the ring
  if ( ccuMapOrder_[ring][1]->getInput() == 0x0 ) { 

    // reset the FEC
    fecReset() ;

    // Set the FEC as the ouput B
    setFecCR0 ( DD_FEC_ENABLE_FEC | DD_FEC_SEL_SER_OUT ) ; 

    // CCU in input B
    setCcuCRC( setCcuKey(ccuMapOrder_[ring][1]->getAddress()), 0x0001);
    fecRelease( true ) ;
    //0x0001 for input B

    // Empty the FIFO Reveice
    emptyFifoReceive() ;

    // Enable the optical
    disableEnableOpticalLink ( ) ;

#ifdef DEBUGMSGERROR
    printf ("At the end: SR0 = 0x%x\n", getFecSR0()) ;
#endif

    // check if the ring is closed 
    // 0000 1000 0000 0000
    //    0    8    0    0
    if (getFecSR0() & FEC_LINK_INITIALISE) {

      result = true ; // stop the reconfiguration

      // Change the value of the control register
      fecOutput_ = 1 ;
      ccuMapOrder_[ring][1]->setInput (1) ;

#ifdef DEBUGMSGERROR
      printf ("Ring ok => CCU 0x%x faulty SR0 = 0x%x\n", ccuMapOrder_[ring][0]->getAddress(), getFecSR0()) ;
#endif
    }
    else { // go back to the old configuration

      setFecCR0 ( fecCR0Value ) ; // fec in output A
      fecRelease ( ) ; // regenerate the token

      setCcuCRC ( setCcuKey(ccuMapOrder_[ring][1]->getAddress()), ccuMapOrder_[ring][1]->getInput()) ; // second ccu in input A        
      fecRelease ( true ) ; // the fec mustn't wait for an answer

#ifdef DEBUGMSGERROR
      printf ("Ring Non ok => CCU 0x%x not faulty\n", ccuMapOrder_[ring][0]->getAddress()) ;
#endif
    }
  }
  else j = j + 2 ;

  if (result == false) {

    // reset the FEC
    fecReset() ;
    // reset ring B
    fecResetRingB() ;
 
    // check if a ccu which isn't before or after the fec is guilty
    for ( i = j ; i < nbCcuOnTheRing_[ring]-1 && !result; i ++ ) { 

#ifdef DEBUGMSGERROR
      printf ("Try to see if it's the CCU %x\n", ccuMapOrder_[ring][i]->getAddress()) ;
#endif

      if ( ccuMapOrder_[ring][i]->getOutput() == 0x0 ) { // the ccu cheched is still in the ring
        
        setCcuCRC ( setCcuKey(ccuMapOrder_[ring][i-1]->getAddress()), 0x2 ) ; // ccu before the ccu checked in output B
        fecRelease ( false ) ; // regenerate the token

        setCcuCRC ( setCcuKey(ccuMapOrder_[ring][i+1]->getAddress()), 0x1 ) ; // ccu after the ccu checked in input B
        fecRelease ( false ) ; // regenerate the token
        setCcuCRC ( setCcuKey(ccuMapOrder_[ring][i+1]->getAddress()), 0x1 ) ; // ccu after the ccu checked in input B
        fecRelease ( false ) ; // regenerate the token

	// Empty the FIFO Reveice
	emptyFifoReceive() ;
	// Enable the optical
	disableEnableOpticalLink ( ) ;

	//#ifdef DEBUGMSGERROR
	printf ("At the end: SR0 = 0x%x\n", getFecSR0()) ;
	//#endif
        
        // check if the ring is closed
        if (isFecSR0Correct(getFecSR0())) {
          result = true ; // stop the reconfiguration

          // Change the value of the control register
          ccuMapOrder_[ring][i-1]->setOutput (1) ;
          ccuMapOrder_[ring][i+1]->setInput  (1) ;

#ifdef DEBUGMSGERROR
	  printf ("Ring ok => CCU 0x%x faulty\n", ccuMapOrder_[ring][i]->getAddress()) ;
#endif
	}
        else { // go back to the old configuration

          setCcuCRC ( setCcuKey(ccuMapOrder_[ring][i-1]->getAddress()), ccuMapOrder_[ring][i-1]->getOutput() ) ; // ccu before the ccu checked in output A
          fecRelease ( ) ; // regenerate the token
	  
          setCcuCRC ( setCcuKey(ccuMapOrder_[ring][i+1]->getAddress()), ccuMapOrder_[ring][i+1]->getInput() ) ; // ccu after the ccu checked in input A
          fecRelease ( ) ; // regenerate the token
	  
#ifdef DEBUGMSGERROR
	  printf ("Ring Non ok => CCU 0x%x not faulty\n", ccuMapOrder_[ring][i]->getAddress()) ;
#endif
        }
      }
      else i = i + 2 ; // it is not necessary to check if the next ccu is guilty
    } 

    if (result == false) {

      // reset the FEC
      fecReset() ;

#ifdef DEBUGMSGERROR
      cout << "The last ccu is still in the ring" << endl ;
#endif

      // the last ccu is still in the ring
      if ( ccuMapOrder_[ring][nbCcuOnTheRing_[ring]-2]->getOutput() == 0x0 ) { 
        
        // check if the last ccu is guilty
        fecCR0Value = getFecCR0 ( ) ; // store the value of fec CR0
          
        setFecCR0 ( DD_FEC_SEL_SER_IN | DD_FEC_ENABLE_FEC ) ; // fec in input B
        fecRelease ( ) ; // regenerate the token

        setCcuCRC ( setCcuKey(ccuMapOrder_[ring][nbCcuOnTheRing_[ring]-2]->getAddress()), 0x1) ; // the first before the last ccu in output B 

        fecRelease ( ) ; // regenerate the token
      
        // check if the ring is closed
        if (isFecSR0Correct(getFecSR0())) {
          result = true ; // stop the reconfiguration

          // Change the value of the control register
          fecInput_ = 1 ;
          ccuMapOrder_[ring][nbCcuOnTheRing_[ring]-2]->setOutput(1) ;

#ifdef DEBUGMSGERROR
	  printf ("Ring ok => CCU 0x%x faulty\n", ccuMapOrder_[ring][i-1]->getAddress()) ;
#endif
        }
        else { // go back to the old configuration

          setFecCR0 ( fecCR0Value ) ; // fec in input A
          fecRelease ( ) ; // regenerate the token

          setCcuCRC ( setCcuKey(ccuMapOrder_[ring][nbCcuOnTheRing_[ring]-2]->getAddress()), ccuMapOrder_[ring][nbCcuOnTheRing_[ring]-2]->getOutput()) ;
          fecRelease ( ) ; // regenerate the token

#ifdef DEBUGMSGERROR
	  printf ("Ring Non ok => CCU 0x%x not faulty\n", ccuMapOrder_[ring][i-1]->getAddress()) ;
#endif
        }
      }
    }
  }

  if (result == true) cout <<"The reconfiguration of the ring has succeed" <<endl;
  else cout <<"Two successives or more ccu are out of order : the ring can't be reconfigured" <<endl;
}

/** This method reconfigure the ring due to a CCU fault.
 * \param ring - ring to be reconfigure
 * \warning this method does not need the order of the CCU
 */
void FecDevice::fecRingReconfigureNorder ( tscType16 ring ) {

  // Reset the FEC to be in clear state
  tscType16 fecCR0Value = getFecCR0() ; 
  fecReset( ) ;
  setFecCR0 (fecCR0Value) ;

  tscType16 frame_input_B [DD_USER_MAX_MSG_LENGTH] = { 0x0, 0x0000, 0x0004, 0x0000, 0x0000, 0x0002, 0x8001} ;
  tscType16 frame_output_B[DD_USER_MAX_MSG_LENGTH] = { 0x0, 0x0000, 0x0004, 0x0000, 0x0000, 0x0002, 0x8002} ;
  tscType16 frame2Send    [DD_USER_MAX_MSG_LENGTH] ;

  // For each CCU
  for (int ccuout = 1 ; ccuout <= MAXCCU ; ccuout ++) {

    // Swith the ccuout to output B
    memcpy (frame2Send, frame_output_B, DD_USER_MAX_MSG_LENGTH*sizeof(tscType16)) ;
    frame2Send[0] = ccuout ;
    tscType16 sizet = 7 ;
    writeFrame (frame2Send, &sizet) ;

#ifdef DEBUGMSGERROR
    printf ("CCU %x: Switch to output B\n", ccuout) ;
#endif

    for (int ccuin = 1 ; ccuin <= MAXCCU ; ccuin ++) {
      
      // Empty the FIFO receive
      emptyFifoReceive() ;

      // Build a frame to set the ccuin in input B => two times
      memcpy (frame2Send, frame_input_B, DD_USER_MAX_MSG_LENGTH*sizeof(tscType16)) ;
      frame2Send[0] = ccuin ;
      sizet = 7 ;
      writeFrame (frame2Send, &sizet) ;

      // Call the FEC release
      fecRelease (false) ;

      // Build a frame to set the ccuin in input B
      memcpy (frame2Send, frame_input_B, DD_USER_MAX_MSG_LENGTH*sizeof(tscType16)) ;
      frame2Send[0] = ccuin ;
      sizet = 7 ;
      writeFrame (frame2Send, &sizet) ;

      // Call the FEC release
      fecRelease (false) ;

#ifdef DEBUGMSGERROR
      printf ("CCU %x: Switch to input B\n", ccuin) ;
#endif

      // Empty the FIFO receive
      emptyFifoReceive() ;

      // Link init ?
      if (getFecSR0() & FEC_LINK_INITIALISE) {

#ifdef DEBUGMSGERROR
	printf ("Found a solution: CCU %x is before CCU %x\n", ccuout, ccuin) ;
#endif

	// Disable and enable for optical link
	disableEnableOpticalLink ( ) ;
      }
    }

    // Reset the FEC to be in clear state
    tscType16 fecCR0Value = getFecCR0() ; 
    fecReset( ) ;
    setFecCR0 (fecCR0Value) ;
  }
}

/** This method reconfigure the ring after a reset in order to retreive the previous state
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 */
void FecDevice::ccuChannelReConfiguration ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, 
                                TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::ccuChannelReConfiguration: " +  
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // For each ring
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {
    // For each CCU
    for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++) {

      // Close the device driver
      ccuDevice *ccu = p->second ;
      
      if (ccu != NULL) {
        // Initialise the CCU and set the CRD for broadcast address
        if (ccu->isCcu25())
          setCcuCRA( setFecKey(fecSlot_) | setRingKey(0) | setCcuKey(ccu->getAddress()), INITCCU25CRA ) ;
        else
          setCcuCRA( setFecKey(fecSlot_) | setRingKey(0) | setCcuKey(ccu->getAddress()), INITOLDCCUCRA ) ;

        // Set the broadcast address, not that after a reset the value is 0x80
        setCcuCRD( setFecKey(fecSlot_) | setRingKey(0) | setCcuKey(ccu->getAddress()), BROADCAST_ADDRESS ) ;

        // First Check which input/ouput it is
        if (ccu->getOutput ( ) != 0) { // Out B
          setCcuRegister( setFecKey(fecSlot_) | setRingKey(0) | setCcuKey(ccu->getAddress()), CMD_CCUWRITECRC, 0x10, CMD_OR ) ; // put B as the output
        }
        
        if (ccu->getInput ( ) != 0) { // In B
          setCcuRegister( setFecKey(fecSlot_) | setRingKey(0) | setCcuKey(ccu->getAddress()), CMD_CCUWRITECRC, 0x1, CMD_OR ) ; // put B as the input
        }
        
        // Check if the channel is enable and the force ack bit
        for (tscType16 i = 0x0 ; i < 16 ; i ++) {
          
          tscType16 channel = i + 0x10 ;
          if (! ccu->isCcu25()) channel = i + 0x1 ;
          
          tscType16 CRA = ccu->geti2cChannelCRA(i); 
          if (CRA != 0) {
            // Set the CRA to its value before the reset
            seti2cChannelCRA( setFecKey(fecSlot_) | setRingKey(0) | setCcuKey(ccu->getAddress()) | setChannelKey(channel), CRA ) ;
          }
        }

        // CCU alarms
        bool alarm1 = ccu->isCcuAlarmEnable (1) ;
        bool alarm2 = ccu->isCcuAlarmEnable (2) ;
        bool alarm3 = ccu->isCcuAlarmEnable (3) ;
        bool alarm4 = ccu->isCcuAlarmEnable (4) ;
        if (alarm1 || alarm2 || alarm3 || alarm4 ) {
          keyType index = buildCompleteKey ( fecSlot_, ring, ccu->getAddress(), 0, 0) ;
          setCcuAlarmsEnable ( index, alarm1, alarm2, alarm3, alarm4 ) ;
        }
      }
    }
  }
}

/** Retreive all the CCU scanned or connected at the time when the function is called 
 * (for all rings)
 * \return a list of keyType
 */
list<keyType> *FecDevice::getCcuList ( bool noBroadcast ) {

#ifndef SCANCCUATSTARTING

  // If the scan order was not done in the FecRingDevice constructor => do it
  if (noBroadcast) {
    // If the scan order was not done in the FecDevice constructor => do it
    // For each ring
    for (tscType16 i = 0 ; i < MAXRING ; i ++) {
      fecScanOrderOldCcu (i) ;
    }
  }
  else {
    // If the scan order was not done in the FecDevice constructor => do it
    // For each ring
    for (tscType16 i = 0 ; i < MAXRING ; i ++) {
      fecScanOrderCcu25 (i) ;
    }
  }
#endif

  list<keyType> *ccuList = NULL ;
  
  // For each ring
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {
    
    // For each CCU
    for (unsigned int i = 0 ; i < nbCcuOnTheRing_[ring] ; i ++) {
      ccuDevice *ccu = ccuMapOrder_[ring][i] ;

      if (ccu != NULL) {

        if (ccuList == NULL) ccuList = new list<keyType> ;
        ccuList->push_back (buildCompleteKey (fecSlot_, ring, ccu->getAddress(), 0, 0)) ;
      }
    }
  }

  return (ccuList) ;
}


// -------------------------------------------------------------------------------------
//
//                                For the CCU registers 
//
// -------------------------------------------------------------------------------------

/** Method in order to retreive a control/status register from the CCU 
 * (Thanks to Jean Fay)
 * \param index - key of the corresponding CCU
 * \param registerValue - register to be accessed (command for the CCU)
 * \return value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * <li>
 * </ul> 
 */   
tscType32 FecDevice::getCcuRegister ( keyType index, 
                                               tscType16 registerValue ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  switch (registerValue) {
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
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "CCU register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::getCcuRegister: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command to access a CCU register") ;
  }
#endif
  
  // read the register and write the new one with the correct value
  tscType16 sizet = 3 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                  sizet, 0x0, 0xFE, registerValue } ;
  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
 
  tscType32 CR = 0 ;

  if (reconfigurationRunning_ == false ) {
    
    // Check the answer
    if ( (lcl_err != DD_RETURN_OK) || (sizet != 3) || (frame[5] & FECACKERROR) ) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  DEVICEDRIVERSUBSYSTEM,
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err,
                                  setFecKey(getFecKey(index)) | 
                                  setRingKey(getRingKey(index)) | 
                                  setCcuKey(getCcuKey(index)), 
                                  "CCU register", registerValue,
                                  TSCFEC_REGISTERACCESS_UMSG, 
                                  "FecDevice::getCcuRegister: " +  
                                  TSCFEC_REGISTERACCESS_DMSG + 
                                  ": CCU register access error") ;
    }

    // A read operation is performed
    lcl_err = readFrame(frame[4], frame) ;
    
    // unsigned long CR ;
    // tscType16 CR ;

    // Specific call for CRE or SRE for CCU 25    
    if ( registerValue == CMD_CCUREADCRE ||
         registerValue == CMD_CCUREADSRE) {

      // The read value is 24 bits seperated in frame[5] to frame[7]

      // Check the size of the frame
      // With the data read you find the status, eof, crc, addr seen, ...
      if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 5) ) {

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                    DEVICEDRIVERSUBSYSTEM,
                                    TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                    lcl_err,
                                    setFecKey(getFecKey(index)) | 
                                    setRingKey(getRingKey(index)) | 
                                    setCcuKey(getCcuKey(index)), 
                                    "CCU register", registerValue,
                                    TSCFEC_REGISTERACCESS_UMSG, 
                                    "FecDevice::getCcuRegister: " +  
                                    TSCFEC_REGISTERACCESS_DMSG + 
                                    ": cannot read back a CCU register") ;
      }
      
      // Get the value without the end of frame and status
      frame[7] &= (~DD_FIFOTRA_EOFRAME_MASK) ;
      CR = ( frame[7] | (frame[6] << 8) | (frame[5] << 16) ) ;
    }
    // Specific call for SRF for the CCU 25
    else if ( registerValue == CMD_CCUREADSRF ) {

      // The read value is 16 bits separated in frame[5] to frame[6]
      
      // Check the size of the frame
      // With the data read you find the status, eof, crc, addr seen, ...
      if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 4) ) {
        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                    DEVICEDRIVERSUBSYSTEM,
                                    TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                    lcl_err,
                                    setFecKey(getFecKey(index)) | 
                                    setRingKey(getRingKey(index)) | 
                                    setCcuKey(getCcuKey(index)),  
                                    "CCU register", registerValue,
                                    TSCFEC_REGISTERACCESS_UMSG, 
                                    "FecDevice::getCcuRegister: " +  
                                    TSCFEC_REGISTERACCESS_DMSG + 
                                    ": cannot read back CCU register (status reg. F)") ;
      }

      // Get the value without the end of frame and status
      frame[6] &= (~DD_FIFOTRA_EOFRAME_MASK) ;
      CR = ( frame[6] | (frame[5] << 8) ) ;
    }
    else {
      // Check the size of the frame
      // With the data read you find the status, eof, crc, addr seen, ...
      if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 3) ) {

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                    DEVICEDRIVERSUBSYSTEM,
                                    TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                    lcl_err,
                                    setFecKey(getFecKey(index)) | 
                                    setRingKey(getRingKey(index)) | 
                                    setCcuKey(getCcuKey(index)), 
                                    "CCU register", registerValue,
                                    TSCFEC_REGISTERACCESS_UMSG, 
                                    "FecDevice::getCcuRegister: " +  
                                    TSCFEC_REGISTERACCESS_DMSG + 
                                    ": cannot read back a CCU register") ;
      }
        
      // Get the value without the end of frame and status
      CR = frame[5] & (~DD_FIFOTRA_EOFRAME_MASK) ;
    }
  }

  return (CR) ;  
}

/** Method in order to retreive the type of the CCU
 * \param ring - ring slot (just for the error message)
 * \param ccuAddress - address of the CCU
 * \param CRE - value of the control register E
 * \return bool - true if it's a CCU25 and false if it's an old CCU
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul> 
 */   
bool FecDevice::getCcuType ( tscType16 ring, 
                             tscType16 ccuAddress,
                             tscType32 *CRE ) 
  throw (FecExceptionHandler) {

  // read the register and write the new one with the correct value
  tscType16 sizet = 3 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { ccuAddress, FRAMEFECNUMBER, 
                                                  sizet, 0x0, 0xFE, CMD_CCUREADCRE } ;
  bool isCcu25 = true ;
 
  // Write the FRAME
  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || 
       (sizet != 3) ||
       (frame[5] & FECACKERROR) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(fecSlot_) | 
                                setRingKey(ring) | 
                                setCcuKey(ccuAddress),
                                "FEC direct acknowledge", frame[5],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getCcuType: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": CCU register access error") ;
  }
    
  // A read operation is performed
  lcl_err = readFrame(frame[4], frame) ;
  
  // The read value is 24 bits seperated in frame[5] to frame[7]

  // Check the size of the frame
  // With the data read you find the status, eof, crc, addr seen, ...
  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_) | 
                                setRingKey(ring) | 
                                setCcuKey(ccuAddress),
                                "FEC frame size", frame[2],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getCcuType: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": cannot read back a CCU register") ;
  }

  if (frame[2] != 5) {
   
    isCcu25 = false ;
  }
      
  // Get the value without the end of frame and status
  *CRE = (frame[7] || (frame[6] << 8) || (frame[5] << 16) ) & (~DD_FIFOTRA_EOFRAME_MASK) ;

  return (isCcu25) ;  
}


/** Method in order to set a CCU control register 
 * \param index - key of the corresponding CCU
 * \param registerValue - register to be accessed (command for the CCU)
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * <li>
 * </ul> 
 * \warning this method check if the CCU is existing or declared in this FecDevice. If you want to change the CRC of the CCU, you must used the method setCcuCRC except for the read modify write method<
 */
void FecDevice::setCcuRegister ( keyType index, 
                                 tscType16 registerValue, 
                                 tscType16 value ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;
 
#ifdef T3TDLEVELCHECK
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  switch (registerValue) {
  case CMD_CCUWRITECRA: break ;
  case CMD_CCUWRITECRB: break ;
  case CMD_CCUWRITECRC: break ;
  case CMD_CCUWRITECRD: break ;
  case CMD_CCUWRITECRE: // Only for old CCUs
    if (! ccu->second->isCcu25()) break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "CCU register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setCcuRegister: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command") ;
  }
#endif

  // Prepare the frame and write it to the ring
  tscType16 sizet = 4 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
					      sizet, 0x0,
					      0, registerValue, value } ;
      
  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  if (reconfigurationRunning_ == false ) {

    // Check the answer
    if ( (lcl_err != DD_RETURN_OK) || (sizet != 4) || (frame[6] & FECACKERROR) ) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  DEVICEDRIVERSUBSYSTEM,
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err, 
                                  setFecKey(getFecKey(index)) | 
                                  setRingKey(getRingKey(index)) | 
                                  setCcuKey(getCcuKey(index)), 
                                  "FEC direct acknowledge", frame[6],
                                  TSCFEC_REGISTERACCESS_UMSG, 
                                  "FecDevice::setCcuRegister(RMW): " +  
                                  TSCFEC_REGISTERACCESS_DMSG + 
                                  ": CCU register access error") ;
    }
    
    // --------------------------------------------------------------------------------------------
    // Due to the small bug in the CCU 25, the control register must read and write for each access
    
    tscType16 readcommand = 0 ;
    
    switch (registerValue) {
    case CMD_CCUWRITECRA: readcommand = CMD_CCUREADCRA ; break ;
    case CMD_CCUWRITECRB: readcommand = CMD_CCUREADCRB ; break ;
    case CMD_CCUWRITECRC: readcommand = CMD_CCUREADCRC ; break ;
    case CMD_CCUWRITECRD: readcommand = CMD_CCUREADCRD ; break ;
    case CMD_CCUWRITECRE: readcommand = CMD_CCUREADCRE ; break ;
    }
    
    // Read back the value
#ifdef DEBUGMSGERROR
    tscType32 CR = 
#endif
      getCcuRegister (index, readcommand) ;
    
#ifdef DEBUGMSGERROR
    printf ("FecDevice::getCcuRegister: Read back value is 0x%x for command %x\n", CR, readcommand) ;
#endif
    
//      if ( CR != value ) {
//       throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
//                                   DEVICEDRIVERSUBSYSTEM,
//                                   TSCFEC_REGISTERACCESS_ERRORSEVERITY,
//                                   lcl_err, 
//                                   setFecKey(getFecKey(index)) | 
//                                   setRingKey(getRingKey(index)) | 
//                                   setCcuKey(getCcuKey(index)), 
//                                   readcommand,
//                                   TSCFEC_REGISTERACCESS_UMSG, 
//                                   "FecDevice::setCcuRegister(RMW): " +  
//                                   TSCFEC_REGISTERACCESS_DMSG + 
//                                   ": CCU register access error: values are different from the set to the get") ;
//      }
    
    // -------------------------------------------------------------------------------------------- 
  }
}
  
/** Method in order to set a control register from the CCU by read modify write operation
 * \param index - key of the corresponding CCU
 * \param registerValue - register to be accessed (command for the CCU)
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul> 
 */
void FecDevice::setCcuRegister ( keyType index, 
                                 tscType16 registerValue, 
                                 tscType16 value,
                                 logicalOperationType op ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK

  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;

  switch (registerValue) {
  case CMD_CCUWRITECRA:
    if (ccu->second->isCcu25()) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  FECSUPERVISORSUBSYSTEM, 
                                  TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                  TSCFEC_INVALIDOPERATION,
                                  setFecKey(getFecKey(index)) | 
                                  setRingKey(getRingKey(index)) | 
                                  setCcuKey(getCcuKey(index)), 
                                  TSCFEC_INVALIDOPERATION_UMSG, 
                                  "FecDevice::setCcuRegister(RMW): " +  
                                  TSCFEC_INVALIDOPERATION_DMSG + 
                                  ": setting the CCU CRA by read-modify-write cannot be used") ;
    }
    break ;
  case CMD_CCUWRITECRB: break ;
  case CMD_CCUWRITECRC: break ;
  case CMD_CCUWRITECRD: break ;
  case CMD_CCUWRITECRE: // Only for old CCUs
    if (! ccu->second->isCcu25()) break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "CCU register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, "FecDevice::setCcuRegister(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG + ": invalid command") ;
  }
#endif

  // Check the command
  tscType16 readcommand = 0 ;

  switch (registerValue) {
  case CMD_CCUWRITECRA: readcommand = CMD_CCUREADCRA ; break ;
  case CMD_CCUWRITECRB: readcommand = CMD_CCUREADCRB ; break ;
  case CMD_CCUWRITECRC: readcommand = CMD_CCUREADCRC ; break ;
  case CMD_CCUWRITECRD: readcommand = CMD_CCUREADCRD ; break ;
  case CMD_CCUWRITECRE: readcommand = CMD_CCUREADCRE ; break ;
  }

  // read the register and write the new one with the correct value
  tscType32 CR = getCcuRegister (index, readcommand) ;

  // Write the value with the operation done
  switch (op) {
  case CMD_OR:  value = CR | value    ; break ;
  case CMD_XOR: value = CR & (~value) ; break ;
  case CMD_AND: value = CR & value    ; break ;
  case CMD_EQUAL: break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "Logical operation", op,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setCcuRegister(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG +
                                ": invalid logical operation (cannot set a CCU register (RMW))") ;
  }

  if (CR != value) setCcuRegister (index, registerValue, value) ;
}

/** Method in order to set CCU control register A
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 */
void FecDevice::setCcuCRA ( keyType index, tscType16 value ) {
  
  setCcuRegister( index, CMD_CCUWRITECRA, value ) ;
}

//setting CRA with ReadModifyWrite is useless as the CRA's bits are ever toggled to 0x0
//void FecDevice::setCcuCRA ( keyType index, tscType16 value, logicalOperationType op ) {
//  setCcuRegister( index, CMD_CCUWRITECRA, value, op ) ;
//}

/** Method in order to set CCU control register B
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 */
void FecDevice::setCcuCRB ( keyType index, tscType16 value ) {

  setCcuRegister( index, CMD_CCUWRITECRB, value ) ;
}

/** Method in order to set CCU control register B by read modify write operation
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 */
void FecDevice::setCcuCRB ( keyType index, tscType16 value, logicalOperationType op ) {

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
 * \warning this method does not check if the CCU is existing or not because can be not closed
 */
void FecDevice::setCcuCRC ( keyType index, tscType16 value ) throw (FecExceptionHandler) {
  
  reconfigurationRunning_ = true ;

  if (value > 0x3) 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "value to be set", value,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setCcuCRC: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid value to be set on a CCU register C") ;

  // Send directly the frame over the ring
  //setCcuRegister( index, CMD_CCUWRITECRC, value ) ;
  // Prepare the frame and write it to the ring
  tscType16 sizet = 4 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
					      sizet, 0x0,
					      0, CMD_CCUWRITECRC, value } ;
  
  //DD_TYPE_ERROR lcl_err = 
    writeFrame (frame, &sizet) ;

  // Try to see if a CCU is connected 
  ccuMapAccessedType::iterator it = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if ((it != ccuMapAccess_[getRingKey(index)].end()) && (it->second != NULL)) {

    ccuDevice *ccu = it->second ;

    if ( value & 0x1 )
      ccu->setInput ( 1 ) ;
    else
      ccu->setInput ( 0 ) ;
    
    if ( value & 0x2 )
      ccu->setOutput ( 1 ) ;
    else
      ccu->setOutput ( 0 ) ; 
  }
    
  reconfigurationRunning_ = false ;
}

/** Method in order to set CCU control register C by read modify write operation
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::setCcuCRC ( keyType index, tscType16 value, logicalOperationType op ) throw (FecExceptionHandler) {
  
  reconfigurationRunning_ = true ;

  // Check the access
  basicCheckError (index) ;

  if (value >= 0x3) 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "value to be set", value,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setCcuCRC(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid value to be set on a CCU register C") ;

  // Try to see if a CCU is connected 
  // Note that the basic check is done if all flags (compiler) is set so
  // the test must redone there
  ccuMapAccessedType::iterator it = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if ((it == ccuMapAccess_[getRingKey(index)].end()) || (it->second == NULL)) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                index,
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::setCcuCRC: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": the CCU is not present on the ring") ;

  }


  // Set the CCU register
  setCcuRegister ( index, CMD_CCUWRITECRC, value, op ) ;  

  // Retreive the value in order to store in the virtual CCU class
  value = getCcuRegister ( index, CMD_CCUREADCRC ) ;

  // CCU found and write to the CCU register ok
  ccuDevice *ccu = it->second ;

  if ( value & 0x1 )
    ccu->setInput ( 1 ) ;
  else
    ccu->setInput ( 0 ) ;

  if ( value & 0x2 )
    ccu->setOutput ( 1 ) ;
  else
    ccu->setOutput ( 0 ) ;

  reconfigurationRunning_ = false ;
}

/** Method in order to set CCU control register D
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \warning The broadcast register is set to 0x80 after a reset
 */
void FecDevice::setCcuCRD ( keyType index, tscType16 value ) {

  setCcuRegister( index, CMD_CCUWRITECRD, value ) ;
}

/** Method in order to set CCU control register D by read modify write operation
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \warning The broadcast register is set to 0x80 after a reset
 */
void FecDevice::setCcuCRD ( keyType index, tscType16 value, logicalOperationType op ) {

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
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::setCcuCRE ( keyType index, tscType32 value ) 
  // tscType16 value0,  // 16 - 23
  // tscType16 value1,  // 8 - 15
  // tscType16 value2)  // 0 - 7   
  throw (FecExceptionHandler) {

  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if ((ccu->second == NULL) || (! ccu->second->isCcu25())) { // => voir pour la suite
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)),
                                "CCU register", CMD_CCUWRITECRE,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setCcuCRE: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": cannot set to control register E for old CCUs") ;
  }

  // Check the access
  basicCheckError (index) ;
 
  // Split the value into three words of 8 bits
  tscType8 value0 = (value & 0xff) ;
  tscType8 value1 = (value & 0xff00) >> 8 ;
  tscType8 value2 = (value & 0xff0000) >> 16 ;

  // Prepare the frame and write it to the ring
  tscType16 sizet = 6 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), 
						  FRAMEFECNUMBER, 
						  sizet, 0x0,
						  0, CMD_CCUWRITECRE, 
						  value2, value1, value0 } ;
      
  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
 
  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 6) || (frame[8] & FECACKERROR) ) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "CCU register", CMD_CCUWRITECRE,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::setCcuCRE: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": CCU register access error (set)") ;
  }
  
  // Due to the small bug in the CCU 25, the control register must read and write for each access 
  // tscType32 CR = 
  getCcuCRE (index) ;
  
//    if ( CR != value ) {
//     throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
//                                 DEVICEDRIVERSUBSYSTEM,
//                                 TSCFEC_REGISTERACCESS_ERRORSEVERITY,
//                                 lcl_err, 
//                                 setFecKey(getFecKey(index)) | 
//                                 setRingKey(getRingKey(index)) | 
//                                 setCcuKey(getCcuKey(index)), 
//                                 "CCU register", CMD_CCUWRITECRE,
//                                 TSCFEC_REGISTERACCESS_UMSG, 
//                                 "FecDevice::setCcuCRE: " +  
//                                 TSCFEC_REGISTERACCESS_DMSG + 
//                                 ": CCU register access error: values are different from the set to the get") ;
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
void FecDevice::setCcuCRE ( keyType index, 
                            tscType8 valueI2C, 
                            tscType8 valuePIA,
                            tscType8 valueMemory,
                            tscType8 valueTrigger,
                            tscType8 valueJTAG) {

  setCcuCRE ( index, 
              (valueI2C & 0xFF) | 
              ((valuePIA & 0x7) << 16)|
              ((valueMemory & 0x1) << 20)| 
              ((valueTrigger & 0x1) << 21) |
              ((valueJTAG & 0x1) << 22) ) ;
}

/** Method in order to set a control register from the CCU by read modify write operation
 * \param index - key of the corresponding CCU
 * \param registerValue - register to be accessed (command for the CCU)
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
void FecDevice::setCcuCRE ( keyType index, 
                            tscType32 value,
                            logicalOperationType op ) throw (FecExceptionHandler) {

  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (! ccu->second->isCcu25()) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "CCU register", CMD_CCUWRITECRE,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setCcuCRE(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": cannot access to control register E for old CCUs") ;
  }

  // read the register and write the new one with the correct value 
  tscType32 CR = getCcuCRE ( index ) ;

  // Write the value with the operation done
  switch (op) {
  case CMD_OR:  value = CR | value    ; break ;
  case CMD_XOR: value = CR & (~value) ; break ;
  case CMD_AND: value = CR & value    ; break ;
  case CMD_EQUAL: break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(fecSlot_), 
                                "Logical operation", op,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setCcuCRE(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG 
                                + ": invalid logical operation (cannot set a FEC register (RMW))") ;

  }

  if (CR != value) setCcuCRE ( index, value ) ;
}

/** Method in order to retreive control register A from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */   
tscType16 FecDevice::getCcuCRA ( keyType index ) {
 
  return ( getCcuRegister( index, CMD_CCUREADCRA )) ;
}

/** Method in order to retreive status register B from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecDevice::getCcuCRB ( keyType index ) {

  return ( getCcuRegister( index, CMD_CCUREADCRB )) ;
}

/** Method in order to retreive control register C from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecDevice::getCcuCRC ( keyType index ) {

  return ( getCcuRegister( index, CMD_CCUREADCRC )) ;
}

/** Method in order to retreive control register D from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecDevice::getCcuCRD ( keyType index ) {

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
tscType32 FecDevice::getCcuCRE ( keyType index ) throw (FecExceptionHandler) {

  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (! ccu->second->isCcu25()) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "CCU register", CMD_CCUREADCRE,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::getCcuCRE: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": cannot get to control register E for old CCUs") ;
  }


  tscType32 CR = getCcuRegister( index, CMD_CCUREADCRE ) ;

  return ( CR ) ;
}

/** Method in order to retreive status register A from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecDevice::getCcuSRA ( keyType index ) {

  return ( getCcuRegister( index, CMD_CCUREADSRA )) ;
}

/** Method in order to retreive status register B from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecDevice::getCcuSRB ( keyType index ) {

  return ( getCcuRegister( index, CMD_CCUREADSRB )) ;
}

/** Method in order to retreive status register C from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecDevice::getCcuSRC ( keyType index ) {

  return ( getCcuRegister( index, CMD_CCUREADSRC )) ;
}

/** Method in order to retreive status register D from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecDevice::getCcuSRD ( keyType index ) {

  return ( getCcuRegister( index, CMD_CCUREADSRD )) ;
}

/** Method in order to retreive status register E from the CCU
 * \param index - key of the corresponding CCU
 * \param registerValue - register to be accessed (command for the CCU)
 * \return value read
 */
tscType16 FecDevice::getCcuSRE ( keyType index ) {

  return ( getCcuRegister( index, CMD_CCUREADSRE )) ;
}

/** Method in order to retreive status register F from the CCU
 * \param index - key of the corresponding CCU
 * \return value read
 */
tscType16 FecDevice::getCcuSRF ( keyType index ) {

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
tscType16 FecDevice::getCcuSRG ( keyType index ) throw (FecExceptionHandler) {

  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "CCU register", CMD_CCUREADSRG,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::getCcuSRG: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": cannot get to status register G for CCU 25s") ;
  }
  
  return ( getCcuRegister( index, CMD_CCUREADSRG )) ;
}

/** Method in order to retreive status register H from the CCU (only for the old CCU)
 * \param index - key of the corresponding CCU
 * \param registerValue - register to be accessed (command for the CCU)
 * \return value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * </ul>
 */
tscType16 FecDevice::getCcuSRH ( keyType index ) throw (FecExceptionHandler) {

  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "CCU register", CMD_CCUREADSRH,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::getCcuSRH: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": cannot get to status register H for CCU 25s") ;
  }

  return ( getCcuRegister( index, CMD_CCUREADSRH )) ;
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
void FecDevice::setChannelEnable ( keyType index, bool enable ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Check if the channel is enable
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
 
  tscType16 channel = getChannelKey(index) ;
  if (ccu->second->getChannelEnable (getChannelKey(index)) != enable) {

    if (ccu->second->isCcu25()) { // CCU 25

      // For the CCU 25,
      //   - Only the node controller of the CCU is set for enabling a channel

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

          throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                      TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                      TSCFEC_FECPARAMETERNOTMANAGED, 
                                      setFecKey(getFecKey(index)) |
                                      setRingKey(getRingKey(index)) |
                                      setCcuKey(getCcuKey(index)) |
                                      setChannelKey(getChannelKey(index)),
                                      TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                      "FecDevice::setChannelEnable: " + 
                                      TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                      ": wrong CCU channel number") ;
        }
      }
    }
    else {

      // For the OLD CCU, 
      //  - the node controller of the CCU is enable for this channel
      //  - enable the channel i2c in the channel

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

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                    TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                    TSCFEC_FECPARAMETERNOTMANAGED, 
                                    setFecKey(getFecKey(index)) |
                                    setRingKey(getRingKey(index)) |
                                    setCcuKey(getCcuKey(index)) |
                                    setChannelKey(getChannelKey(index)),
                                    TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                    "FecDevice::setChannelEnable: " + 
                                    TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                    ": PIA channels cannot be used on old CCUs") ;

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

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                    TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                    TSCFEC_FECPARAMETERNOTMANAGED, 
                                    setFecKey(getFecKey(index)) |
                                    setRingKey(getRingKey(index)) |
                                    setCcuKey(getCcuKey(index)) |
                                    setChannelKey(getChannelKey(index)),
                                    TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                    "FecDevice::setChannelEnable: " + 
                                    TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                    ": memory channel cannot be used on old CCUs") ;

      } else if ( isTriggerChannelOldCcu(index) ) { // Trigger channel

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                    TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                    TSCFEC_FECPARAMETERNOTMANAGED, 
                                    setFecKey(getFecKey(index)) |
                                    setRingKey(getRingKey(index)) |
                                    setCcuKey(getCcuKey(index)) |
                                    setChannelKey(getChannelKey(index)),
                                    TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                    "FecDevice::setChannelEnable: " + 
                                    TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                    ": trigger channel cannot be used on old CCUs") ;

      } else if ( isJTAGChannelOldCcu(index) ) { // JTAG channel

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                    TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                    TSCFEC_FECPARAMETERNOTMANAGED, 
                                    setFecKey(getFecKey(index)) |
                                    setRingKey(getRingKey(index)) |
                                    setCcuKey(getCcuKey(index)) |
                                    setChannelKey(getChannelKey(index)),
                                    TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                    "FecDevice::setChannelEnable: " + 
                                    TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                    ": JTAG channel cannot be used on old CCUs") ;

      } else {

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                    TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                    TSCFEC_FECPARAMETERNOTMANAGED, 
                                    setFecKey(getFecKey(index)) |
                                    setRingKey(getRingKey(index)) |
                                    setCcuKey(getCcuKey(index)) |
                                    setChannelKey(getChannelKey(index)),
                                    TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                    "FecDevice::setChannelEnable: " + 
                                    TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                    ": wrong CCU channel number") ;
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
void FecDevice::setCcuAlarmsEnable ( keyType index, 
                                     bool enable1, bool enable2, bool enable3, bool enable4 ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

    tscType16 value = 0 ;

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
 * \param frame - frame received by the warning
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 * \warning Alarms works with CCU 25 and not old CCU
 * \warning This method is a blocked command no interruption can be done
 * \warning The size of the frame must be DD_USER_MAX_MSG_LENGTH
 */
void FecDevice::waitForAnyCcuAlarms ( keyType index, tscType16 frame[DD_USER_MAX_MSG_LENGTH] ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

#ifdef T3TDLEVELCHECK

    // Check if the CRB is enable for CCU alarms
    if (! ccu->second->isCcuAlarmEnable (1) && 
        ! ccu->second->isCcuAlarmEnable (2) && 
        ! ccu->second->isCcuAlarmEnable (3) && 
        ! ccu->second->isCcuAlarmEnable (4) ) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  FECSUPERVISORSUBSYSTEM, 
                                  TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                  TSCFEC_INVALIDOPERATION,
                                  setFecKey(getFecKey(index)) | 
                                  setRingKey(getRingKey(index)) | 
                                  setCcuKey(getCcuKey(index)),
                                  TSCFEC_INVALIDOPERATION_UMSG, 
                                  "FecDevice::waitForAnyCcuAlarms: " +  
                                  TSCFEC_INVALIDOPERATION_DMSG + 
                                  ": Cannot wait for alarms, they are not enabled") ;
    }
        
#endif


    DD_TYPE_ERROR lcl_err = 
#ifndef EMULATION
      glue_fec_wait_warning ( deviceDescriptor_, frame ) ;
#else
    DD_RETURN_OK ;
#endif


    // Check the answer
    if (lcl_err != DD_RETURN_OK) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  DEVICEDRIVERSUBSYSTEM,
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err, 
                                  setFecKey(getFecKey(index)) | 
                                  setRingKey(getRingKey(index)) | 
                                  setCcuKey(getCcuKey(index)),
                                  TSCFEC_REGISTERACCESS_UMSG, 
                                  "FecDevice::waitForAnyCcuAlarms: " +  
                                  TSCFEC_REGISTERACCESS_DMSG + 
                                  ": Error during the wait for CCU Alarms") ;
    }
  }
  else {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::waitForAnyCcuAlarms: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": CCU alarms cannot be used with old CCUs") ;
  }
}

/** 
 * \param frame - frame received by the warning
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 * \warning Alarms works with CCU 25 and not old CCU
 * \warning This method is a blocked command no interruption can be done
 * \warning The size of the frame must be DD_USER_MAX_MSG_LENGTH
 */
void FecDevice::waitForAnyCcuAlarms ( tscType16 frame[DD_USER_MAX_MSG_LENGTH] ) throw (FecExceptionHandler) {


  DD_TYPE_ERROR lcl_err = 
#ifndef EMULATION
    glue_fec_wait_warning ( deviceDescriptor_, frame ) ;
#else
    DD_RETURN_OK ;
#endif

  // Check the answer
  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(fecSlot_),
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::waitForAnyCcuAlarms: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": Error during the wait for CCU Alarms") ;
  }
}

/** 
 * \return list<tscType32> with in order:
 * <ul>
 * <li> FEC slot | SR0 
 * <li> FEC slot | SR1
 * <li> FEC slot | CR0 
 * <li> FEC slot | CR1
 * <li> CCU index key
 * <li> SRA
 * <li> SRB
 * <li> SRC
 * <li> SRD
 * <li> SRE
 * <li> SRF
 * <li> CRA
 * <li> CRB
 * <li> CRC
 * <li> CRD
 * <li> CRE
 * <li> CCU index key
 * <li> SRA
 * <li> etc.
 * </ul>
 * \see keyType.h
 * \warning a scan ring or accesses must be done before any check
 * \warning the list must be deleted when you have finished with it
 * \warning in case of error the value put is CCU address | 0xFFFF
 */
list<tscType32> *FecDevice::getFecCcuRegisters ( ) {

  list<tscType32> *fecCcuRegisters = new list<tscType32> ;

  try {
#ifdef DEBUGMSGERROR
    printf ("FEC SR0= 0x%x\n", getFecSR0()) ;
#endif
    fecCcuRegisters->push_back (setFecKey(fecSlot_) | getFecSR0()) ;
  }
  catch (FecExceptionHandler &e) {

    cerr << "-------------------------- WARNING ---------------------" << endl ;
    cerr << "FecDevice::getFecCcuRegisters: Problem during read Fec SR0" << endl ;
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    fecCcuRegisters->push_back (setFecKey(fecSlot_) | 0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("FEC SR1= 0x%x\n", getFecSR1()) ;
#endif
    fecCcuRegisters->push_back (setFecKey(fecSlot_) | getFecSR1()) ;
  }
  catch (FecExceptionHandler &e) {

    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "FecDevice::getFecCcuRegisters: Problem during read Fec SR1\n") ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    fecCcuRegisters->push_back (setFecKey(fecSlot_) | 0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("FEC CR0= 0x%x\n", getFecCR0()) ;
#endif
    fecCcuRegisters->push_back (setFecKey(fecSlot_) | getFecCR0()) ;
  }
  catch (FecExceptionHandler &e) {

    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read Fec CR0\n") ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    fecCcuRegisters->push_back (setFecKey(fecSlot_) | 0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("FEC CR1= 0x%x\n", getFecCR1()) ;
#endif
    fecCcuRegisters->push_back (setFecKey(fecSlot_) | getFecCR1()) ;
  }
  catch (FecExceptionHandler &e) {
    
    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read Fec CR0\n") ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    fecCcuRegisters->push_back (setFecKey(fecSlot_) | 0xFFFF) ;
  }
  // For each ring
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) // For each CCU

    for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++) {

      ccuDevice *ccu = p->second ;

      if (ccu != NULL) {

        keyType index = buildCompleteKey(fecSlot_,ring,ccu->getAddress(),0,0) ;

        // Index is push in the getCcuRegisters method

        list<tscType32> *ccuRegisters = getCcuRegisters ( index ) ;

        for (list<keyType>::iterator p=ccuRegisters->begin();p!=ccuRegisters->end();p++) {

          tscType32 value = *p ;
          fecCcuRegisters->push_back (value) ;
        }

        delete ccuRegisters ;
      }
    }

  return (fecCcuRegisters) ;
}

/** 
 * \param index - index of the CCU (FEC/RING/CCU)
 * \return list<tscType32> with in order:
 * <ul>
 * <li> index of the CCU
 * <li> SRA
 * <li> SRB
 * <li> SRC
 * <li> SRD
 * <li> SRE
 * <li> SRF
 * <li> CRA
 * <li> CRB
 * <li> CRC
 * <li> CRD
 * <li> CRE
 * </ul>
 * \see keyType.h
 * \warning a scan ring or accesses must be done before any check
 * \warning the list must be deleted when you have finished with it
 * \warning in case of error the value put is CCU address | 0xFFFF
 */
list<tscType32> *FecDevice::getCcuRegisters ( keyType index ) {
  
  // Check the access
  basicCheckError (index); 

  list<tscType32> *ccuRegisters = new list<tscType32> ;

  // Find the CCU
  ccuMapAccessedType::iterator p = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  ccuDevice *ccu = p->second ;

  // Push the index in the list
  ccuRegisters->push_back (index) ;
        
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x SRA= 0x%x\n", ccu->getAddress(), getCcuSRA (index)) ;
#endif
    ccuRegisters->push_back (getCcuSRA (index)) ;
  }
  catch (FecExceptionHandler &e) {

    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x SRA\n", ccu->getAddress()) ;
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x SRB= 0x%x\n", ccu->getAddress(), getCcuSRB (index)) ;
#endif
    ccuRegisters->push_back (getCcuSRB (index)) ;
  }
  catch (FecExceptionHandler &e) {

    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x SRB\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back ( 0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x SRC= 0x%x\n", ccu->getAddress(), getCcuSRC (index)) ;
#endif
    ccuRegisters->push_back (getCcuSRC (index)) ;
  }
  catch (FecExceptionHandler &e) {
	  
    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x SRC\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x SRD= 0x%x\n", ccu->getAddress(), getCcuSRD (index)) ;
#endif
    ccuRegisters->push_back (getCcuSRD (index)) ;
  }
  catch (FecExceptionHandler &e) {
          
    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x SRD\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x SRE= 0x%x\n", ccu->getAddress(), getCcuSRE (index)) ;
#endif
    ccuRegisters->push_back (getCcuSRE (index)) ;
  }
  catch (FecExceptionHandler &e) {
          
    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x SRE\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back ( 0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x SRF= 0x%x\n", ccu->getAddress(), getCcuSRF (index)) ;
#endif
    ccuRegisters->push_back (getCcuSRF (index)) ;
  }
  catch (FecExceptionHandler &e) {

    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x SRF\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x CRA= 0x%x\n", ccu->getAddress(), getCcuCRA (index)) ;
#endif
    ccuRegisters->push_back (getCcuCRA (index)) ;
  }
  catch (FecExceptionHandler &e) {

    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x CRA\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x CRB= 0x%x\n", ccu->getAddress(), getCcuCRB (index)) ;
#endif
    ccuRegisters->push_back (getCcuCRB (index)) ;
  }
  catch (FecExceptionHandler &e) {
          
    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x CRB\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }
  try {  
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x CRC= 0x%x\n", ccu->getAddress(), getCcuCRC (index)) ;
#endif
    ccuRegisters->push_back (getCcuCRC (index)) ;
  }
  catch (FecExceptionHandler &e) {
         
    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x CRC\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    printf ("CCU 0x%x CRD= 0x%x\n", ccu->getAddress(), getCcuCRD (index)) ;
#endif
    ccuRegisters->push_back (getCcuCRD (index)) ;
  }
  catch (FecExceptionHandler &e) {
          
    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x CRD\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }
  try {
#ifdef DEBUGMSGERROR
    fprintf (stderr, "CCU 0x%x CRE= 0x%x\n", ccu->getAddress(), getCcuCRE (index)) ;
#endif
    ccuRegisters->push_back (getCcuCRE (index)) ;
  }
  catch (FecExceptionHandler &e) {
	  
    cerr << "-------------------------- WARNING ---------------------" << endl ;
    fprintf (stderr, "Problem during read CCU 0x%x CRE\n", ccu->getAddress()) ; 
    cerr << e.what() << endl ;
    cerr << "--------------------------------------------------------" << endl ;
    ccuRegisters->push_back (0xFFFF) ;
  }

  return ccuRegisters ;
}

// -------------------------------------------------------------------------------------
//
//                                For the channel registers 
//
// -------------------------------------------------------------------------------------

/** Method in order to retreive a channel control/status register from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */   
tscType16 FecDevice::getChannelRegister ( keyType index,
                                                   tscType16 registerValue ) 
  throw (FecExceptionHandler) {
  
  // Check the access
  basicCheckError (index); 

#ifdef T3TDLEVELCHECK
  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  switch (registerValue) {
  case CMD_CHANNELTRIGGERREADCRA: break ;
  case CMD_CHANNELTRIGGERREADCRB: break ;
  case CMD_CHANNELTRIGGERREADSRA: break ;
  case CMD_CHANNELI2CREADCRA    : break ;
  case CMD_CHANNELI2CREADSRA    : break ;
  case CMD_CHANNELI2CREADSRB    : break ;
  case CMD_CHANNELI2CREADSRC    : break ; 
  case CMD_CHANNELI2CREADSRD    : break ;
  //case CMD_CHANNELMEMREADCRA:             // Same value than next command
  case CMD_CHANNELPIAREADGCR    : break ;
  case CMD_CHANNELPIAREADDDR    : break ;
  case CMD_CHANNELPIAREADDATAREG: break ;
  //case CMD_CHANNELMEMREADSTATUSREG:       // Same value than next command
  case CMD_CHANNELPIAREADSTATUS : break ;
  case CMD_CHANNELMEMREADMASKREG:
    if (ccu->second->isCcu25()) break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "CCU channel register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::getChannelRegister: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command") ;
  }
#endif

  // read the register
  tscType16 sizet = 3 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  0xFE, registerValue } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 3) || (frame[5] & FECACKERROR) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "direct acknowledge", frame[5],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getChannelRegister: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": channel register access error") ;
  }    
  
  // The bit force acknowledge is set or a read operation is performed
  lcl_err = readFrame(frame[4], frame) ;

  // Check the size of the frame 
  if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 3)
       // || (! frame[5] & DD_FIFOTRA_EOFRAME_MASK) 
       ) {
      
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Answer status acknowledge", frame[5],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getChannelRegister: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": cannot read back channel register") ;
    
  }
  
  tscType16 CR = frame[5] & (~DD_FIFOTRA_EOFRAME_MASK) ;
  
  return (CR) ;
}

/** Method in order to set a channel control register 
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 * \warning the value can be 16 bits
 */
void FecDevice::setChannelRegister ( keyType index, 
                                     tscType16 registerValue, 
                                     tscType16 value )
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  switch (registerValue) {
  case CMD_CHANNELTRIGGERWRITECRA: break ;
  case CMD_CHANNELTRIGGERWRITECRB: break ;
  case CMD_CHANNELI2CWRITECRA: break ;
  case CMD_CHANNELI2CWRITECRA: break ;
  //case CMD_CHANNELMEMWRITECRA: // Same value than next command
  case CMD_CHANNELPIAWRITEGCR: break ; 
  case CMD_CHANNELPIAWRITEDDR: break ; 
  case CMD_CHANNELPIAWRITEDATAREG: break ;
  case CMD_CHANNELMEMWRITEMASKREG: break ;
  default: 

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "CCU channel register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setChannelRegister: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command") ;
  }
#endif

  // Prepare the frame and write it to the ring
  tscType16 sizet = 4 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  0, registerValue, value } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
  
  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 4) || (frame[6] & FECACKERROR) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "direct acknowledge", frame[6],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::setChannelRegister: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": channel register access error (set)") ;
  }
}



/** Method in order to set a control register from the channel by read modify write operation
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecDevice::setChannelRegister ( keyType index, 
                                     tscType16 registerValue, 
                                     tscType16 value,
                                     logicalOperationType op )
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  switch (registerValue) {
  case CMD_CHANNELTRIGGERWRITECRA: break ;
  case CMD_CHANNELTRIGGERWRITECRB: break ;
  case CMD_CHANNELI2CWRITECRA: break ;
  case CMD_CHANNELI2CWRITECRA: break ;
  case CMD_CHANNELPIAWRITEGCR: break ;
  case CMD_CHANNELPIAWRITEDDR: break ;
  case CMD_CHANNELPIAWRITEDATAREG: break ;
  default: 

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "CCU channel register", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setChannelRegister(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command") ;
  }
#endif

  // Check the command 
  tscType16 readcommand = 0 ;

  switch (registerValue) {
  case CMD_CHANNELTRIGGERWRITECRA: readcommand = CMD_CHANNELTRIGGERREADCRA ; break ;
  case CMD_CHANNELTRIGGERWRITECRB: readcommand = CMD_CHANNELTRIGGERREADCRB ; break ;
  case CMD_CHANNELI2CWRITECRA: readcommand = CMD_CHANNELI2CREADCRA ; break ;
  case CMD_CHANNELPIAWRITEGCR: readcommand = CMD_CHANNELPIAREADGCR ; break ;
  case CMD_CHANNELPIAWRITEDDR: readcommand = CMD_CHANNELPIAREADDDR ; break ;
  case CMD_CHANNELPIAWRITEDATAREG: readcommand = CMD_CHANNELPIAREADDATAREG ; break ;
  }

  // Read the register
  tscType16 CR = getChannelRegister ( index, readcommand ) ;

  // Write the value with the operation done
  switch (op) {
  case CMD_OR:  value = CR | value    ; break ;
  case CMD_XOR: value = CR & (~value) ; break ;
  case CMD_AND: value = CR & value    ; break ;
  case CMD_EQUAL: break ;
  default: 

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Logical operation", op,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setChannelRegister(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG 
                                + ": invalid logical operation (cannot set a channel register (RMW))") ;

  }

  if (CR != value) setChannelRegister ( index, registerValue, value ) ;
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
void FecDevice::setInitPiaChannel ( keyType index ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;

  if (ccu->second->isCcu25()) {

    setChannelRegister ( index, CMD_CHANNELPIAWRITEGCR, INITCCU25PIAGCR ) ;
    setChannelRegister ( index, CMD_CHANNELPIAWRITEDDR, INITCCU25PIADDR ) ;
    setChannelRegister ( index, CMD_CHANNELPIAWRITEDATAREG, INITCCU25PIADATAREG ) ;

    ccu->second->setPiaChannelGCR (getChannelKey(index), INITCCU25PIAGCR ) ;
  }
  else {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::setInitPiaChannel: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": PIA channels cannot be used on old CCUs") ;

    // Channel GCR
    // setPiaChannelGCR (index, INITOLDCCUPIAGCR) ; // , CMD_OR) ;
    // Make a map of the current system
    // ccu->second->setPiaChannelGCR (getChannelKey(index), INITOLDCCUPIAGCR) ; //, CMD_OR) ;
  }
}


/** Method in order to set PIA general control register 
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written
 */
void FecDevice::setPiaChannelGCR ( keyType index, tscType16 value ) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEGCR, value ) ;
}

/** Method in order to set PIA general control register by read modify write operation
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 */
void FecDevice::setPiaChannelGCR ( keyType index, tscType16 value, logicalOperationType op ) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEGCR, value, op ) ;
}

/** Method in order to retreive PIA general control register from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */   
tscType16 FecDevice::getPiaChannelGCR ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELPIAREADGCR)) ;
}

/** Method in order to retreive PIA status register from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */ 
tscType16 FecDevice::getPiaChannelStatus ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELPIAREADSTATUS)) ;
}

/** Method in order to set PIA Data Direction Register
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecDevice::setPiaChannelDDR ( keyType index, tscType16 value ) throw (FecExceptionHandler) {

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEDDR, value) ;

  /** Check the force ack of the channel */
  ccuMapAccessedType::iterator ccu =  ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu == ccuMapAccess_[getRingKey(index)].end()) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::setPiaChannelDDR: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": the CCU is not present on the ring") ;

  }
}

/**  Method in order to set PIA Data Direction Register by read modify write operation
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecDevice::setPiaChannelDDR ( keyType index, 
                                   tscType16 value, 
                                   logicalOperationType op ) 
  throw (FecExceptionHandler) {
 
  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEDDR, value, op) ;

  // Check the force ack of the channel */
  ccuMapAccessedType::iterator ccu =  ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu == ccuMapAccess_[getRingKey(index)].end()) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::setPiaChannelDDR(RMW): " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": the CCU is not present on the ring") ;

  }
}

/** Method in order to retreive PIA Data Direction Register from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */ 
tscType16 FecDevice::getPiaChannelDDR ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELPIAREADDDR)) ;
}

/** Method in order to set PIA data register 
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written
 */
void FecDevice::setPiaChannelDataReg ( keyType index, tscType16 value ) {

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEDATAREG, value) ;
}

/** Method in order to set PIA Data Register by read modify write operation
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecDevice::setPiaChannelDataReg ( keyType index, 
                                       tscType16 value, 
                                       logicalOperationType op) throw (FecExceptionHandler) {

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELPIAWRITEDATAREG, value, op) ;

#ifdef DEBUGMSGERROR
  printf ("PIA channel Data reg are 0x%x / 0x%x\n",
          value, getPiaChannelDataReg(index)) ;

  if (value != getPiaChannelDataReg(index)) {

    printf ("PIA channel Data reg different = 0x%x / 0x%x\n",
            value, getPiaChannelDataReg(index)) ;
    getchar() ;
  }
#endif
}

/** Method in order to retreive PIA data register from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */   
tscType16 FecDevice::getPiaChannelDataReg ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELPIAREADDATAREG)) ;
}

/** Method in order to reset a PIA channel
 * \param index - key of the corresponding channel
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::piaChannelReset( keyType index) throw (FecExceptionHandler) {

  printf ("piaChannelReset\n") ;

  // Check the access
  basicCheckError (index) ;

  /** Prepare the frame and write it to the ring */
  tscType16 sizet = 3 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                       sizet, getChannelKey(index), 
                                                       0, CMD_CHANNELRESETPIA } ;
  
  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
  
  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 3) || (frame[6] & FECACKERROR) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)),
                                "direct acknowledge", frame[6],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::piaChannelReset: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": channel register access error (reset)") ;
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
 * \warning The source must be CLEARED before reenable it.
 */
void FecDevice::setPiaInterruptEnable ( keyType index ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

    if (getChannelKey(index) == 0) {

      for ( tscType16 channel = 0x30 ; channel <= 0x33 ; channel ++ ) {
	
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

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::setCcuAlarms: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": CCU alarms cannot be used with old CCUs") ;
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
void FecDevice::setPiaClearInterrupts ( keyType index ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu->second->isCcu25()) {

    if (getChannelKey(index) == 0) {

      for ( tscType16 channel = 0x30 ; channel <= 0x33 ; channel ++ ) {

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

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::setCcuAlarms: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": CCU alarms cannot be used with old CCUs") ;
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
 * \param i2cSpeed - if the speed is zero then the value is not changed
 */
void FecDevice::setInitI2cChannelCRA ( keyType index, bool forceAck, tscType16 i2cSpeed ) {

  // Check the access
  basicCheckError (index);

  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;

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
  if (forceAck) channelCRA |= BITFORCEACKCHANNELCRA ;

#ifdef DEBUGERRORMSG
  cout << "I2C Channel: FEC " << dec << getFecKey(index) 
       << " Ring " << dec << getRingKey(index) 
       << " CCU 0x" << hex << getCcuKey(index) 
       << " Channel 0x" << hex << getChannelKey(index) 
       << ": speed to " << dec << i2cSpeed 
       << " (CRA = 0x" << (int)channelCRA << ")"
       << endl ;
#endif

  // Set the channel CRA and set the bit force ack if needed
  // Make a map of the current system in ccuDevice class
  seti2cChannelCRA (index, channelCRA) ;
}

/** Method in order to set I2C control register A
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * </ul>
 */
void FecDevice::seti2cChannelCRA ( keyType index, tscType16 value) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELI2CWRITECRA, value) ;

  // Check the force ack of the channel
  ccuMapAccessedType::iterator ccu =  ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (ccu == ccuMapAccess_[getRingKey(index)].end()) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)),
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::seti2cChannelCRA: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": the CCU is not present on the ring") ;
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
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */ 
tscType16 FecDevice::geti2cChannelCRA ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADCRA)) ;
}

/** Method in order to retreive I2C status register A from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */ 
tscType16 FecDevice::geti2cChannelSRA ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADSRA)) ;
}

/** Method in order to retreive I2C status register B from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */ 
tscType16 FecDevice::geti2cChannelSRB ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADSRB) ) ;
}

/** Method in order to retreive I2C status register C from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */ 
tscType16 FecDevice::geti2cChannelSRC ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELI2CREADSRC)) ;
}

/** Method in order to retreive I2C status register D from a CCU
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 */ 
tscType16 FecDevice::geti2cChannelSRD ( keyType index ) {

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
void FecDevice::i2cChannelReset ( keyType index ) throw (FecExceptionHandler) {

  // Same command than PIA
  piaChannelReset (index) ;
}

/** Method in order to reset all the I2C channel and reconfigure the channels
 * \param index - key of the corresponding channel
 */
void FecDevice::i2cChannelReset ( keyType index, bool reconf ) {

  // Check the access
  basicCheckError (index); 

  i2cChannelReset ( index ) ;

  if (reconf) {  
    
    tscType16 CRAValue ;

    ccuMapAccessedType::iterator p = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
    ccuDevice *ccu = p->second ;

    for (int i = 0 ; i < MAXI2CCHANNELS ; i ++) {
      int channel = i + 0x1 ;

      CRAValue = ccu->geti2cChannelCRA ( i ) ;
      seti2cChannelCRA ( setFecKey(fecSlot_) | setRingKey(0) | setCcuKey(ccu->getAddress()) | setChannelKey(channel), CRAValue ) ;
    }
  }
}

/** Test the CCU control register E in order to know if a channel is enable or not
 * \param index - index of the channel
 * \return true if the channel is enable, false if not
 * \warning this method is only available for CCU 25
 */
bool FecDevice::isChannelEnabled ( keyType index ) {

  // Check the access
  basicCheckError (index) ;

  // Check if the channel is enable
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
 
  if (ccu->second->isCcu25()) { // CCU 25

    // Read the CRE
    tscType32 CRE = getCcuCRE (index) ;
    tscType32 channel = getChannelKey (index) ;

    if (isi2cChannelCcu25 (index)) {
      
      // Enable the I2C channel bit 0 - 15
      if (CRE & (0x1 << (channel - 0x10))) {
        
        return true ;
      }
      
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
    //  - the node controller of the CCU is enable for this channel
    //  - enable the channel i2c in the channel
    tscType16 CRA = getCcuCRA ( index ) ;
    
    if ( isi2cChannelOldCcu(index) ) { // I2C channel
      
      if ((CRA & 0x1) && isChannelEnable(geti2cChannelCRA(index))) {
        
        return true ;
      }
    }
    else if ( isPiaChannelOldCcu(index) ) { // PIA channel

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                  TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                  TSCFEC_FECPARAMETERNOTMANAGED, 
                                  setFecKey(getFecKey(index)) |
                                  setRingKey(getRingKey(index)) |
                                  setCcuKey(getCcuKey(index)) |
                                  setChannelKey(getChannelKey(index)),
                                  TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                  "FecDevice::setChannelEnable: " + 
                                  TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                  ": Other channels than i2c channels cannot be used on old CCUs") ;
    }
  }

  return false ;
}


/** Force the acknowledge bit corresponding to the channel in the key
 * \param index - key of the channel
 * \param enable - boolean for enable or disable the force ack bit
 */
void FecDevice::setChannelForceAck ( keyType index, bool enable ) {

  // Check the access
  basicCheckError (index) ;

  // Check if the channel is forced
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;

  tscType16 channel = getChannelKey(index) ;

  if (ccu->second->getBitForceAck (getChannelKey(index)) != enable) {

    // Channel Control register A 
    if (ccu->second->isCcu25 ()) { // CCU 25

      if (enable) 
        setChannelRegister (index, CMD_CHANNELI2CWRITECRA, 0x40, CMD_OR) ;
      else
        setChannelRegister (index, CMD_CHANNELI2CWRITECRA, 0x40, CMD_XOR) ;
    }
    else {                         // Old CCUs

      // It seems that for old CCUs the I2C channel CRA cannot be read back
      tscType16 value = INITOLDCCUI2CCRA ;
      if (enable) value |= 0x40 ;
      else value &= ~0x40 ;

      seti2cChannelCRA (index, value) ;
      ccu->second->seti2cChannelCRA (channel, value) ;
    }

    // Set the channel as enable in the CCU class
    ccu->second->setBitForceAck (channel, enable) ;
  }
}

/** Write a value to the device speiciied by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::writei2cDevice ( keyType index, tscType16 value ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame 
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 5, 
  //  Channel     = index
  //  Transaction = Assisgn by the device driver
  //  Command     = single byte write normal mode
  //  Address     = index
  //  DW          = value

  // Check if the force acknowledge is set
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  tscType16 transaction = 0 ;

  tscType16 channel = getChannelKey(index) ;

  if (ccu->second->getBitForceAck(channel)) transaction = 0xFE ;

  // Constitution de la frame
  tscType16 sizet = 5 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  transaction, CMD_SINGLEBYTEWRITENORMALMODE,
						  getAddressKey(index), value } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
  
  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 5) || (frame[7] & FECACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (sizet != 5) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "direct acknowledge", frame[7],
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::writei2cDevice(single byte write normal mode): " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": I2C channel access error: single byte write normal mode") ;
  }    
  
  // Check if the force acknowledge is set and must be retreive
  if (transaction == 0xFE) {

    // The bit force acknowledge is set 
    lcl_err = readFrame(frame[4], frame) ;
    
    // Check the frame and the SRA in the i2c interface CCU 25 page 29
    if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 3) || (frame[5] & CCUFACKERROR) ) {
      // Not an error from the device driver
      if (lcl_err == DD_RETURN_OK) {
	if (frame[2] != 3) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
	else lcl_err = TSCFEC_I2CREGISTERACCESS ;
      }

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err,
                                  index,
                                  "Force acknowledge", frame[5],
                                  TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                  "FecDevice::writei2cDevice(single byte write normal mode): " + 
                                  TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                  ": I2C channel access error: single byte write normal mode, cannot read back the force acknowledge") ;
    }
  }
}

/** Write a value to the device specified by the key in extended mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::writei2cDevice ( keyType index, 
                                 tscType16 offset, 
                                 tscType16 value ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 5, 
  //  Channel     = index
  //  Transaction = Assisgn by the device driver
  //  Command     = single byte write ral mode
  //  Address     = index
  //  Offset      = offset
  //  DW          = value

  // Check if the force acknowledge is set
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  tscType16 transaction = 0 ;

  tscType16 channel = getChannelKey(index) ;

  if (ccu->second->getBitForceAck(channel)) transaction = 0xFE ;

  // Constitution de la frame
  tscType16 sizet = 6 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  transaction, CMD_SINGLEBYTEWRITERALMODE,
						  getAddressKey(index), offset, value } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 6) || (frame[8] & FECACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (sizet != 6) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "direct acknowledge", frame[8],
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::writei2cDevice(single byte write extended (RAL) mode): " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": I2C channel access error: single byte write extended (RAL) mode") ;
  }    

  // Check if the force acknowledge is set and must be retreive
  if (transaction == 0xFE) {

    // The bit force acknowledge is set
    lcl_err = readFrame(frame[4], frame) ;

    // Check the frame and the SRA in the i2c interface CCU 25 page 29
    if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 3) || (frame[5] & CCUFACKERROR) ) {
      // Not an error from the device driver
      if (lcl_err == DD_RETURN_OK) {
	if (frame[2] != 3) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
	else lcl_err = TSCFEC_I2CREGISTERACCESS ;
      }

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err,
                                  index,
                                  "Force acknowledge", frame[5],
                                  TSCFEC_REGISTERACCESS_UMSG, 
                                  "FecDevice::writei2cDevice(single byte write extended (RAL) mode): " + 
                                  TSCFEC_REGISTERACCESS_DMSG +
                                  ": I2C channel access error: single byte write extended (RAL) mode, cannot read back the force acknowledge") ;
    }
  }
}

/** Write a value to the device specified by the key in RAL mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param value - value to be written
 */
void FecDevice::writei2cRalDevice ( keyType index, 
                                    tscType16 offset, 
                                    tscType16 value ) {

  // Same as write extended mode
  writei2cDevice (index, offset, value) ;
}

/** Write a value to the device speiciied by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param decal - shift the value. Use only for the laserdriver
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::writei2cDeviceOffset ( keyType index, 
                                       tscType16 offset, 
                                       tscType16 decal, 
                                       tscType16 value ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 5, 
  //  Channel     = index
  //  Transaction = Assign by the device driver
  //  Command     = single byte write normal mode
  //  Address     = index
  //  DW          = value

  // Check if the force acknowledge is set
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  tscType16 transaction = 0 ;

  tscType16 channel = getChannelKey(index) ;

  if (ccu->second->getBitForceAck(channel)) transaction = 0xFE ;

  // Constitution de la frame
  tscType16 sizet = 5 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  transaction, CMD_SINGLEBYTEWRITENORMALMODE,
						  (getAddressKey(index) << decal) | offset, value } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
  
  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 5) || (frame[7] & FECACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (sizet != 4) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "direct acknowledge", frame[7],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::writei2cDeviceOffset(single byte write normal mode): " + 
                                TSCFEC_REGISTERACCESS_DMSG +
                                ": I2C channel access error: single byte write normal mode (offset)") ;
  }    
  
  // Check if the force acknowledge is set and must be retreive
  if (transaction == 0xFE) {

    // The bit force acknowledge is set 
    lcl_err = readFrame(frame[4], frame) ;
    
    // Check the frame and the SRA in the i2c interface CCU 25 page 29 
    if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 3) || (frame[5] & CCUFACKERROR) ) {
      // Not an error from the device driver
      if (lcl_err == DD_RETURN_OK) {
	if (frame[2] != 3) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
	else lcl_err = TSCFEC_I2CREGISTERACCESS ;
      }

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err,
                                  index,
                                  "Force acknowledge", frame[5],
                                  TSCFEC_REGISTERACCESS_UMSG, 
                                  "FecDevice::writei2cDeviceOffset(single byte write normal mode): " + 
                                  TSCFEC_REGISTERACCESS_DMSG +
                                  ": I2C channel access error: single byte write normal mode (offset), cannot read back the force acknowledge") ;
    }
  }
}

/** Write a value to the device speiciied by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param value - value to be written
 */
void FecDevice::writei2cDeviceOffset ( keyType index, tscType16 offset, tscType16 value ) {
  writei2cDeviceOffset (index, offset, 0) ;
}

/** Read a value to the device specified by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \return the value read in the register
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
tscType16 FecDevice::readi2cDevice ( keyType index ) throw (FecExceptionHandler) {
  
  // Check the access
  basicCheckError (index) ;
  
  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 4, 
  //  Channel     = index
  //  Transaction = Assisgn by the device driver
  //  Command     = single byte read normal mode
  //  Address     = index
  tscType16 sizet = 4 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						       sizet, getChannelKey(index), 
						       0xFE, CMD_SINGLEBYTEREADNORMALMODE,
						       getAddressKey(index) } ;
  
  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 4) || (frame[6] & FECACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (sizet != 4) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "direct acknowledge", frame[6],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readi2cDevice (single byte read normal mode): " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": I2C channel access error: single byte read normal mode") ;
  }

  // Read back the value asked
  lcl_err = readFrame(frame[4], frame) ;
  
  // Check the frame and the SRA in the i2c interface CCU 25 page 25
  if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 4) || (frame[6] & CCUFACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (frame[2] != 4) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "Answer status acknowledge", frame[6],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readi2cDevice (single byte read normal mode): " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": I2C channel access error: single byte read normal mode, cannot read back the value") ;
  }

  return (frame[5]) ;
}

/** Read a value to the device specified by the key in extended mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param addressMSB - give an offset to the address set in the constructor
 * \return the value read in the register
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
tscType16 FecDevice::readi2cDevice ( keyType index, 
                                              tscType16 addressMSB ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 4, 
  //  Channel     = index
  //  Transaction = Assisgn by the device driver
  //  Command     = single byte read normal mode
  //  Address     = index (base address for bits 7-0
  //  AddressMSB  = base address for bits 15-8
  tscType16 sizet = 5 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                      sizet, getChannelKey(index), 
                                                      0xFE, CMD_SINGLEBYTEREADEXTENDEDMODE,
                                                      getAddressKey(index), addressMSB } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 5) || (frame[7] & FECACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (sizet != 5) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "direct acknowledge", frame[7],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readi2cDevice(single byte read extended mode): " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": I2C channel access error: single byte read extended mode") ;
  }    

  // Read back the value asked
  lcl_err = readFrame(frame[4], frame) ;
  if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 4) || (frame[6] & CCUFACKERROR) ) {

    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (frame[2] != 4) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "Answer status acknowledge", frame[6],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readi2cDevice(single byte read extended mode): " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": I2C channel access error: single byte read extended mode, cannot read back the value") ;
  }

  return (frame[5]) ;
}

/** Read a value to the device specified by the key in RAL mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \return the value read in the register
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 * \bug It seems that an error on read back doesn't correspond to a device driver error
 */
tscType16 FecDevice::readi2cRalDevice ( keyType index, 
                                                 tscType16 offset ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 4, 
  //  Channel     = index
  //  Transaction = Assisgn by the device driver
  //  Command     = single byte read normal mode
  //  Address     = index
  //  Offset      = offset
  tscType16 sizet = 5 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                      sizet, getChannelKey(index), 
                                                      0xFE, CMD_SINGLEBYTEREADRALMODE,
                                                      getAddressKey(index), offset } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 5) || (frame[7] & FECACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (sizet != 5) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "direct acknowledge", frame[7],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readi2cRalDevice(single byte read RAL mode): " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": I2C channel access error: single byte read RAL mode") ;
  }    

  // Read back the value asked
  lcl_err = readFrame(frame[4], frame) ;
  if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 4) || (frame[6] & CCUFACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (frame[2] != 4) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "Answer status acknowledge", frame[6],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readi2cRalDevice(single byte read RAL mode):" + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": I2C channel access error: single byte read RAL mode, cannot read back the value") ;
  }

  return (frame[5]) ;
}

/** Read a value to the device specified by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \param decal - shift the value. Use only for the laserdriver
 * \return the value read in the register
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
tscType16 FecDevice::readi2cDeviceOffset ( keyType index, 
					   tscType16 offset, 
					   tscType16 decal ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;
  
  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 4, 
  //  Channel     = index
  //  Transaction = Assisgn by the device driver
  //  Command     = single byte read normal mode
  //  Address     = index
  tscType16 sizet = 4 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
					      sizet, getChannelKey(index), 
					      0xFE, CMD_SINGLEBYTEREADNORMALMODE,
					      (getAddressKey(index) << decal) | offset } ;
  
  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 4) || (frame[6] & FECACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (sizet != 4) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "direct acknowledge", frame[6],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readi2cDeviceOffset(single byte read normal mode): " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": I2C channel access error: single byte read normal mode (offset)") ;
  }

  // Read back the value asked
  lcl_err = readFrame(frame[4], frame) ;
  
  // Check the frame and the SRA in the i2c interface CCU 25 page 25 
  if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 4) || (frame[6] & CCUFACKERROR) ) {
    // Not an error from the device driver
    if (lcl_err == DD_RETURN_OK) {
      if (frame[2] != 4) lcl_err = TSCFEC_CODECONSISTENCYERROR ;
      else lcl_err = TSCFEC_I2CREGISTERACCESS ;
    }

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                index,
                                "Answer status acknowledge", frame[6],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readi2cDeviceOffset(single byte read normal mode): " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": I2C channel access error: single byte read normal mode (offset), cannot read back the value") ;
  }

  return (frame[5]) ;
}

/** Read a value to the device specified by the key in normal mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address set in the constructor
 * \return the value read in the register
 */
tscType16 FecDevice::readi2cDeviceOffset ( keyType index, tscType16 offset ) {
  return (readi2cDeviceOffset (index, offset, 0)) ;
}

/** Scan the ring for each FEC and create a list associated
 * for all the I2C devices found
 * \param device - array of device address with device[0] = device address
 * and device[1] = mode (NORMALMODE, EXTENDEDMODE, RALMODE)
 * \param size - number of devices
 * \return a list of keyType
 * \exception FecExceptionHandler
 * \warning this method is dedicated to the i2c devices
 * \warning this method is dedicated to the CCU 25 but can work for old CCUs
 */
list<keyType> *FecDevice::scanRingForI2CDevice ( keyType *deviceValues, 
                                                 tscType16 sizeDevices ) 
  throw (FecExceptionHandler) {

  list<keyType> *deviceList = NULL ;

#ifndef SCANCCUATSTARTING

  // If the scan order was not done in the FecDevice constructor => do it
  // For each ring
  for (tscType16 i = 0 ; i < MAXRING ; i ++) {

    fecScanOrderCcu25 (i) ;
  }
#endif

  // For each ring
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {

    // For each CCU
    for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++) {

      ccuDevice *ccu = p->second ;

      if (ccu != NULL) {

        tscType16 offset = ccu->isCcu25() ? 0x10 : 0x1 ;

        for (int ci = 0 ; ci < MAXI2CCHANNELS ; ci ++) {
          tscType16 channel = ci + offset ;

          // build a correct index
          keyType index = buildCompleteKey ( fecSlot_, ring, ccu->getAddress(), channel, 0) ;

          // Check the force ack
          bool forceAck = ccu->getBitForceAck(channel) ;

          // Check if the channel is already enabled
          bool channelEnable = false ;

          try {
            // Check if channel is enabled
            channelEnable = isChannelEnabled (index) ;
          }
          catch (FecExceptionHandler &e) {
            
            cerr << e.what() << endl ;
            cerr << "Continue with this error ...\n" << endl ;
          }          

          try {            
            ccu->setChannelEnable (channel, channelEnable) ;
          }
          catch (FecExceptionHandler &e) {
            
            cerr << e.what() << endl ;
            cerr << "Continue with this error ...\n" << endl ;
          }

          try {
            // Enable the channel
            if (!channelEnable) setChannelEnable (index, true) ;

            // Force the acknowledge
            if (!forceAck) setChannelForceAck (index, true) ;
          }
          catch (FecExceptionHandler &e) {
	    
            cerr << e.what() << endl ;
            cerr << "Continue with this error ...\n" << endl ;
          }

          // Try to write/read to each device given
          for (int di = 0 ; di < (sizeDevices*2) ; di += 2) {

	    // Test if a channel is set in the sizeDevices
	    // If yes, the test is only performed on this channels
	    bool test = true ;
	    if (getChannelKey(deviceValues[di]) != 0)
	      test = getChannelKey(deviceValues[di]) & channel ;
            
	    if (test) {

	      tscType8 address = getAddressKey(deviceValues[di]) ;
            
	      try {

		index = buildCompleteKey ( fecSlot_, ring, ccu->getAddress(), channel, address) ;
		char msg[500] ;
		decodeKey (msg, index) ;
	      
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

#ifdef DEBUGMSGERROR
		cout << "Device " << msg << " exists" << endl ;
#endif

		// Insert it in the list
		if (deviceList == NULL) deviceList = new list<keyType> ;
		deviceList->push_back (index) ;
	      }
	      catch (FecExceptionHandler &e) {// nothing => no device 
	      }
	    }
	  }

          try {
            // Set the force ack
            if (!forceAck) setChannelForceAck (index, false) ;

            // Enable the channel
            if (!channelEnable) setChannelEnable (index, false) ;
          }
          catch (FecExceptionHandler &e) {
	    
            cerr << e.what() << endl ;
            cerr << "Continue with this error ...\n" << endl ;
          }
        }
      }
    }
  }

  return (deviceList) ;
}

/** scan the ring for each FEC and create a vector associated
 * for all the tracker devices (I2C bus)
 * \return a list of keyType
 * \exception FecExceptionHandler
 * \warning this method is dedicated to the i2c devices
 * \warning this method is dedicated to the CCU 25 but can work for old CCUs
 */
list<keyType> *FecDevice::scanRingForI2CDevice ( bool display ) throw (FecExceptionHandler) {

  list<keyType> *deviceList = NULL ;

#ifndef SCANCCUATSTARTING

  // If the scan order was not done in the FecDevice constructor => do it
  // For each ring
  for (tscType16 i = 0 ; i < MAXRING ; i ++) {
    fecScanOrderCcu25 (i) ;
  }

#endif

  // For each ring
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {

    // For each CCU
    for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++) {

      ccuDevice *ccu = p->second ;

      if (ccu != NULL) {

        tscType16 offset = ccu->isCcu25() ? 0x10 : 0x1 ;

        // For each channel
        for (int ci = 0 ; ci < MAXI2CCHANNELS ; ci ++) {

          tscType16 channel = ci + offset ;

          // build a correct index
          keyType index = buildCompleteKey ( fecSlot_, ring, ccu->getAddress(), channel, 0) ;

          // Check the force ack
          bool forceAck = ccu->getBitForceAck(channel) ;

          // Enable the channel
          setChannelEnable (index, true) ;

          // Force the acknowledge
          if (!forceAck) setChannelForceAck (index, true) ;           

          // Try to write/read to each device given
          for (tscType16 address = 0 ; address < 0xFF ; address += 1) {

            try {

              index = buildCompleteKey ( fecSlot_, ring, ccu->getAddress(), channel, address) ;

#ifndef DEBUGMSGERROR
	    if (display) {
#endif
	      cout << "Probing on FEC " << (int)getFecSlot() << " ring " << (int)ring << " CCU 0x" << hex << ccu->getAddress() << " channel " << dec << (int)channel << " address 0x" << hex << (int)address << "\r" ;
	      fflush (stdout) ;
#ifndef DEBUGMSGERROR
	    }
#endif

              readi2cDevice ( index ) ;

#ifndef DEBUGMSGERROR
	    if (display)
#endif
	      cout << "Probing on FEC " << (int)getFecSlot() << " ring " << (int)ring << " CCU 0x" << hex << ccu->getAddress() << " channel " << dec << (int)channel << " address 0x" << hex << (int)address << " ==> Found a register" << endl ;


              // Insert it in the list
              if (deviceList == NULL) deviceList = new list<keyType> ;
              deviceList->push_back (index) ;

            }
            catch (FecExceptionHandler &e) {// nothing => no device 
            }
          }

          // Set the force ack
          if (!forceAck) setChannelForceAck (index, false) ;
        }
      }
    }
  }

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
void FecDevice::memoryChannelReset( keyType index ) {

  piaChannelReset (index) ;
}


/** Method in order to set the memory control register A
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecDevice::setMemoryChannelCRA ( keyType index, tscType16 value ) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELMEMWRITECRA, value ) ;
}

/** Method in order to set the memory control register A
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecDevice::setMemoryChannelCRA ( keyType index, tscType16 value,
                                      logicalOperationType op ) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELMEMWRITECRA, value, op ) ;
}

/** Method in order to set a memory channel WIN?? register 
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \param value - value to be written (16 bits)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::setMemoryChannelWinReg ( keyType index, 
                                         tscType16 registerValue, 
                                         tscType16 value ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // The basic check is done in the setChannelRegister

  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;

  switch (registerValue) {
  case CMD_CHANNELMEMWRITEWIN1LREG: 
  case CMD_CHANNELMEMWRITEWIN1HREG: 
  case CMD_CHANNELMEMWRITEWIN2LREG: 
  case CMD_CHANNELMEMWRITEWIN2HREG: // 16 bits register
    if (ccu->second->isCcu25()) break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory channel win. reg.", registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::setMemoryChannelWinReg: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command to access memory channel register") ;
  }

  // Prepare the frame and write it to the ring
  tscType16 sizet = 5 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  0, registerValue, value >> 8, value & 0xFF} ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 5) || (frame[7] & FECACKERROR) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory channel win. reg.",
                                registerValue,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::setMemoryChannelWinReg: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": memory channel register access error (set)") ;
  }
}

/** Method in order to set the memory WIN1L register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecDevice::setMemoryChannelWin1LReg ( keyType index, tscType16 value ) {

  // The basic check is done in setMemoryChannelWinReg
  setMemoryChannelWinReg ( index, CMD_CHANNELMEMWRITEWIN1LREG, value ) ;
}

/** Method in order to set the memory WIN1H register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecDevice::setMemoryChannelWin1HReg ( keyType index, tscType16 value ) {

  // The basic check is done in setMemoryChannelWinReg
  setMemoryChannelWinReg ( index, CMD_CHANNELMEMWRITEWIN1HREG, value ) ;
}

/** Method in order to set the memory WIN2L register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecDevice::setMemoryChannelWin2LReg ( keyType index, tscType16 value ) {

  // The basic check is done in setMemoryChannelWinReg
  setMemoryChannelWinReg ( index, CMD_CHANNELMEMWRITEWIN2LREG, value ) ;
}

/** Method in order to set the memory WIN2H register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecDevice::setMemoryChannelWin2HReg ( keyType index, tscType16 value ) {

  // The basic check is done in setMemoryChannelWinReg
  setMemoryChannelWinReg ( index, CMD_CHANNELMEMWRITEWIN2HREG, value ) ;
}

/** Method in order to set the memory mask register
 * \param index - key of the corresponding channel
 * \param value - value used during a RMW operation (see below)
 * \param op - logical operation (and / or / xor)
 */
void FecDevice::setMemoryChannelMaskReg ( keyType index, tscType16 value ) {

  // Check the access
  basicCheckError (index); 

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELMEMWRITEMASKREG, value ) ;
}

/** Method in order to retreive memory control register A
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecDevice::getMemoryChannelCRA ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELMEMREADCRA)) ;
}

/** Method in order to retreive memory mask register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecDevice::getMemoryChannelMaskReg ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELMEMREADMASKREG)) ;
}

/** Method in order to retreive a memory control register (WIN??)
 * \param index - key of the corresponding channel
 * \param registerValue - register to be accessed (command for the channel)
 * \return value read
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
tscType16 FecDevice::getMemoryChannelWinReg ( keyType index,
                                                       tscType16 registerValue ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index); 

#ifdef T3TDLEVELCHECK
  // The basic check is done in the getChannelRegister
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  if (! ccu->second->isCcu25()) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                setFecKey(getFecKey(index)) |
                                setRingKey(getRingKey(index)) |
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory channel win. reg.",
                                registerValue,
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::getMemoryChannelWinReg: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": memory channel cannot be used on old CCUs") ;
  }

  switch (registerValue) {
  case CMD_CHANNELMEMREADWIN1LREG: break ;
  case CMD_CHANNELMEMREADWIN1HREG: break ;
  case CMD_CHANNELMEMREADWIN2LREG: break ;
  case CMD_CHANNELMEMREADWIN2HREG: break ;
  default: 
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)),  
                                "Memory channel win. reg.",
                                registerValue,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::getMemoryChannelWinReg: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid command to access memory channel register") ;
  }
#endif

  // read the register
  tscType16 sizet = 3 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  0xFE, registerValue } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
 
  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 3) || (frame[5] & FECACKERROR) ) {
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory channel win. reg.",
                                registerValue,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getMemoryChannelWinReg: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": memory channel register access error (get)") ;
  } 
  
  // The bit force acknowledge is set or a read operation is performed 
  lcl_err = readFrame(frame[4], frame) ;

  // Check the size of the frame 
  if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 4)
       // || (! frame[5] & DD_FIFOTRA_EOFRAME_MASK) 
       ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory channel win. reg.", registerValue,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::getMemoryChannelWinReg: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": cannot read back memory channel register") ;
  }

  tscType16 CR = (frame[5] << 8) | (frame[6] & (~DD_FIFOTRA_EOFRAME_MASK)) ;
  
  return (CR) ;
}

/** Method in order to retreive memory WIN1L register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecDevice::getMemoryChannelWin1LReg ( keyType index ) {

  return ( getMemoryChannelWinReg ( index, CMD_CHANNELMEMREADWIN1LREG)) ;
}

/** Method in order to retreive memory WIN1H register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecDevice::getMemoryChannelWin1HReg ( keyType index ) {

  return ( getMemoryChannelWinReg ( index, CMD_CHANNELMEMREADWIN1HREG)) ;
}

/** Method in order to retreive memory WIN2L register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecDevice::getMemoryChannelWin2LReg ( keyType index ) {

  return ( getMemoryChannelWinReg ( index, CMD_CHANNELMEMREADWIN2LREG)) ;
}

/** Method in order to retreive memory WIN2H register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecDevice::getMemoryChannelWin2HReg ( keyType index ) {

  return ( getMemoryChannelWinReg ( index, CMD_CHANNELMEMREADWIN2HREG)) ;
}

/** Method in order to retreive memory mask register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecDevice::getMemoryChanneMaskReg ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELMEMREADMASKREG)) ;
}

/** Method in order to retreive memory status register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecDevice::getMemoryChannelStatus ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELMEMREADSTATUSREG)) ;
}

/** Write a value to the memory specified by the key (single byte mode)
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param value - value to be written
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::writeIntoMemory ( keyType index, 
                                  tscType16 AH, tscType16 AL, 
                                  tscType16 value ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 6, 
  //  Channel     = index
  //  Transaction = Assign by the device driver
  //  Command     = single byte write
  //  AH          = Position in the high memory
  //  AL          = Position in the low memory
  //  DW          = value
  
  // Constitution de la frame
  tscType16 sizet = 6 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  0x1, CMD_CHANNELMEMSINGLEBYTEWRITE,
						  AH, AL, value } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
  
  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 6) || (frame[8] & FECACKERROR) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)),
                                "FEC direct acknowledge",
                                frame[8],
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::writeIntoMemory: " +      
                                TSCFEC_REGISTERACCESS_DMSG +
                                + ": memory channel access error (write one byte)") ;
  }    
  
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
void FecDevice::writeIntoMemory ( keyType index, 
                                  tscType16 AH, tscType16 AL, 
                                  tscType16 *values,
                                  unsigned long size ) {



  // 127 means FEC_UPPERLIMIT_LENGTH
  // 119 means FEC_UPPERLIMIT_LENGTH - 5 - 3

  // Check the access
  basicCheckError (index) ;

  if (size >= MAXMEMORYBYTESADDRESS) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                index,
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::writeIntoMemory: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": Too many bytes") ;
  }

  tscType16 sizeMaxFrame = DD_USER_MAX_MSG_LENGTH ;

  if ( sizeMaxFrame > FEC_UPPERLIMIT_LENGTH && size > (FEC_UPPERLIMIT_LENGTH-5-3) ) { 
    // 119 data => 3 words are reserved for the Dest / Src / Length
    //          => 5 words for the reset of the frame: channel / Transaction / Command / AH / AL

    writeIntoMemory2BytesLength ( index, AH, AL, values, size ) ;

    // Finished
    return ;
  }

  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = sizeMaxFrame - 3 (maximum)
  //  Channel     = index
  //  Transaction = Assign by the device driver
  //  Command     = multiple byte write
  //  AH          = Position in the high memory
  //  AL          = Position in the low memory
  //  DW[size]    = values

  // Counter of values
  unsigned long counter = 0, downloaded = 0 ;
  tscType16 al = AL, ah = AH ;

  while (counter < size) {

    // Constitution de la frame
    tscType16 sizet = sizeMaxFrame < (size-counter+5+3) ? sizeMaxFrame : (size-counter+5+3) ;
    
    // src/dest/length
    sizet -= 3 ;

    tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
							 sizet, getChannelKey(index), 
							 0x1, CMD_CHANNELMEMMULTIPLEBYTEWRITE,
							 AH, AL } ;

    // Insert in the frame the values
    // constant 8 is Dest / Src / Length / Channel / Transaction / Command / AH / AL
    memcpy (&frame[counter-downloaded+8], &values[counter], sizeof(tscType16) * (sizet-5)) ; counter += (sizet-5) ;

    // Already downloaded
    downloaded = counter ;
    AL = (al+ah*255+downloaded) & 0xFF ;
    AH = ((al+ah*255+downloaded) & 0xFF00) >> 8 ;

    // Write it
    DD_TYPE_ERROR lcl_err ;
    lcl_err = writeFrame (frame, &sizet) ;

    // Multiple byte write access generate always an error (EOF = 0xa0??)
    // Display error
    if (lcl_err != DD_RETURN_OK) {
#ifdef DEBUGMSGERROR
      char msg[1000] ;
      glue_fecdriver_get_error_message (lcl_err, msg) ;
      printf ("lcl_err = %d\n%s", lcl_err, msg) ;
      printf ("Status = 0x%x\n", getMemoryChannelStatus ( index )) ;
#endif
    }

//      if ((frame[2+sizet] & 0xF000) != 0xa000) {
//        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
//                                    TSCFEC_REGISTERACCESS_ERRORSEVERITY,
//                                    lcl_err, 
//                                    setFecKey(getFecKey(index)) | 
//                                    setRingKey(getRingKey(index)) | 
//                                    setCcuKey(getCcuKey(index)) |
//                                    setChannelKey(getChannelKey(index)), 
//                                    "FEC direct acknowledge",
//                                    frame[2+sizet],
//                                    TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
//                                    "FecDevice::writeIntoMemory(multiple bytes): " + 
//                                    TSCFEC_FECPARAMETERNOTMANAGED_DMSG + ": memory channel access error (write multiple bytes => possible error)") ;
//      }
  }

  // Clear errors on FEC and CCU
  setFecCR1 ( DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS ) ;
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
void FecDevice::writeIntoMemory2BytesLength ( keyType index, 
                                              tscType16 AH, tscType16 AL, 
                                              tscType16 *values,
                                              unsigned long size ) {

  // 127 means FEC_UPPERLIMIT_LENGTH
  // 119 means FEC_UPPERLIMIT_LENGTH - 5 - 3

  // Check the access
  basicCheckError (index) ;

  tscType16 sizeMaxFrame = DD_USER_MAX_MSG_LENGTH ;

  if (sizeMaxFrame <= FEC_UPPERLIMIT_LENGTH || size <= (FEC_UPPERLIMIT_LENGTH-5-3)) {
    // 119 data => 3 words are reserved for the Dest / Src / Length
    //          => 5 words for the reset of the frame: channel / Transaction / Command / AH / AL

    writeIntoMemory (index, AH, AL, values, size) ;
  }

  // Build the frame and send it via the method write frame */
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
  tscType16 al = AL, ah = AH ;

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

      tscType16 sizetH = 0x80 | ((sizet & 0xFF00) >> 8) ;
      tscType16 sizetL = sizet & 0xFF ;
    
      tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                      sizetH, sizetL, getChannelKey(index), 
                                                      0x1, CMD_CHANNELMEMMULTIPLEBYTEWRITE,
                                                      AH, AL } ;

      // Insert in the frame the values 
      // constant 9 is Dest / Src / LengthH / LengthL / Channel / Transaction / Command / AH / AL
      memcpy (&frame[counter-downloaded+9], &values[counter], sizeof(tscType16) * (sizet-5)) ; counter += (sizet-5) ;

      // Already downloaded
      downloaded = counter ;
      AL = (al+ah*255+downloaded) & 0xFF ;
      AH = ((al+ah*255+downloaded) & 0xFF00) >> 8 ;

      // Write it
      DD_TYPE_ERROR lcl_err ;
      lcl_err = writeFrame (frame, &sizet) ;

      // Multiple byte write access generate always an error (EOF = 0xa0??)
      // Display error
      if (lcl_err != DD_RETURN_OK) {
#ifdef DEBUGMSGERROR        
        char msg[1000] ;
        glue_fecdriver_get_error_message (lcl_err, msg) ;
        printf ("lcl_err = %d\n%s", lcl_err, msg) ;
        printf ("Status = 0x%x\n", getMemoryChannelStatus ( index )) ;
#endif
      }

//        if ((frame[3+sizet] & 0xF000) != 0xa000) {
//         throw FecExceptionHandler ( FECSUPERVISORSYSTEM, DEVICEDRIVERSUBSYSTEM, 
//                                     TSCFEC_REGISTERACCESS_ERRORSEVERITY,
//                                     lcl_err, 
//                                     setFecKey(getFecKey(index)) | 
//                                     setRingKey(getRingKey(index)) | 
//                                     setCcuKey(getCcuKey(index)) |
//                                     setChannelKey(getChannelKey(index)), 
//                                     "FEC direct acknowledge",
//                                     frame[2+sizet],
//                                     TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
//                                     "FecDevice::writeIntoMemory2BytesLength: " + 
//                                     TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
//                                     ": memory channel access error (write multiple bytes => possible error)") ;
//        }
    }
  }

  // Clear errors on FEC and CCU
  setFecCR1 ( DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS ) ;
}

/** Read modify write a value from the memory specified by the key in single byte
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR)
 * \param mask - mask to be done during the read modify write
 */
void FecDevice::writeIntoMemory ( keyType index, 
                                  tscType16 AH, tscType16 AL, 
                                  logicalOperationType op, 
                                  tscType16 mask ) {

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
 * <ul>
 * <li>TSCFEC_INVALIDOPERATION
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::writeIntoMemory ( keyType index, 
                                  tscType16 AH, tscType16 AL, 
                                  logicalOperationType op) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  // Operation
  tscType16 command ;

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
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "operation",
                                op,
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::writeIntoMemory(RMW): " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": invalid logical operation (cannot set values into memory channel (RMW))") ;
  }

  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 5, 
  //  Channel     = index
  //  Transaction = Assign by the device driver
  //  Command     = command
  //  AH          = Position in the high memory
  //  AL          = Position in the low memory
  tscType16 sizet = 5 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
						  sizet, getChannelKey(index), 
						  0x1, command,
						  AH, AL } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 5) || (frame[7] & FECACKERROR) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory address",
                                (AH << 16) | AL,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::writeIntoMemory(RMW): " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": memory channel access error (RMW)") ;

  }
}


/** Read a value from the memory specified by the key in single byte
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \return the value read in the memory
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
tscType16 FecDevice::readFromMemory ( keyType index, 
                                      tscType16 AH, tscType16 AL ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;
  
  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 5, 
  //  Channel     = index
  //  Transaction = Assign by the device driver
  //  Command     = single byte read from memory
  //  AH          = Position in the high memory
  //  AL          = Position in the low memory
  tscType16 sizet = 5 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                       sizet, getChannelKey(index), 
                                                       0xFE, CMD_CHANNELMEMSINGLEBYTEREAD,
                                                       AH, AL } ;

  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

  // Check the answer
  if ( (lcl_err != DD_RETURN_OK) || (sizet != 5) || (frame[7] & FECACKERROR) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory address",
                                (AH << 16) | AL,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readFromMemory: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": memory channel read error in single byte mode") ;
  }

  // Read back the value asked
  lcl_err = readFrame(frame[4], frame) ;

  // Check the frame
  if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 3) || !(frame[5] & DD_FIFOTRA_EOFRAME_MASK) ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory address",
                                (AH << 16) | AL,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readFromMemory: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": memory channel read error in single byte mode, cannot read back the value") ;
  }

  return (frame[5] & ~DD_FIFOTRA_EOFRAME_MASK) ;
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
void FecDevice::readFromMemory ( keyType index, 
                                 tscType16 AH, tscType16 AL,
                                 unsigned long size,
                                 tscType16 *values ) 
  throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

  if (size >= MAXMEMORYBYTESADDRESS) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECPARAMETERNOTMANAGED_ERRORSEVERITY,
                                TSCFEC_FECPARAMETERNOTMANAGED, 
                                index,
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG, 
                                "FecDevice::readFromMemory: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG + 
                                ": Too many bytes") ;
  }

  tscType16 sizeMaxFrame = DD_USER_MAX_MSG_LENGTH ;

#ifdef BUGMEMORY2BYTES
  sizeMaxFrame -= 2 ; // Memory bus channel error, lenh-lenl has to be increased by 2
#endif

  // Counter of values
  unsigned long counter = 0 ;
  tscType16 al = AL, ah = AH ;

  // Build the frame and send it via the method write frame */
  //  Dest        = index, 
  //  Src         = 0x0, 
  //  Length      = 6 or 7
  //  Channel     = index
  //  Transaction = Assign by the device driver
  //  Command     = multiple byte read from memory
  //  AH          = Position in the high memory
  //  AL          = Position in the low memory
  //  LENGTH      = length (that can be put in two separated words)

  while (counter < size) { 

    // constant 5 or 6 => read back value
    //                 => 3, 4 for Src / Dest / Length
    //                 => 2 for command (in read)

    tscType16 val ;
    if (sizeMaxFrame > FEC_UPPERLIMIT_LENGTH) val = 6 ;  // 2 words for length
    else val = 5 ;

    // Size max is
    tscType16 length = (size-counter) > (unsigned long)(sizeMaxFrame-val) ? (sizeMaxFrame-val) : (size-counter) ;

#ifdef BUGMEMORY2BYTES
    length += 2 ; // Memory bus channel error, lenh-lenl has to be increased by 2
#endif

    // Determine the size and the length words
    tscType16 sizet ;
    tscType16 LENH, LENL ;
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
    tscType16 sz = sizet ;

    tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                sizet, getChannelKey(index), 
                                                0xFE, CMD_CHANNELMEMMULTIPLEBYTEREAD,
                                                AH, AL, LENH, LENL } ;
    
    // Write it
    DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;

    // Check the answer
    if ( (lcl_err != DD_RETURN_OK) || (sizet != sz) || (frame[7] & FECACKERROR) ) {
      
#ifdef DEBUGMSGERROR
      char msg[1000] ;
      glue_fecdriver_get_error_message (lcl_err, msg) ;
      printf ("Error during the FecDevice::writeFrame:lcl_err = %d\n\t%s\n", lcl_err, msg) ;
#endif

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
				  DEVICEDRIVERSUBSYSTEM,
				  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
				  lcl_err, 
				  setFecKey(getFecKey(index)) | 
				  setRingKey(getRingKey(index)) | 
				  setCcuKey(getCcuKey(index)) |
          setChannelKey(getChannelKey(index)), 
				  "Memory address",
				  (AH << 16) | AL,
				  TSCFEC_REGISTERACCESS_UMSG, 
				  "FecDevice::readFromMemory(multiple bytes): " +  
				  TSCFEC_REGISTERACCESS_DMSG + ": memory channel read error in multiple byte mode") ;
    }

    // Read back the values asked
    lcl_err = readFrame(frame[4], frame) ;

    // Error ?
    if (lcl_err == DD_RETURN_OK) {

      // Check the size of the frame
      tscType16 deb = 5 ;
      tscType16 realSize = frame[2] ;
      if (realSize & FEC_LENGTH_2BYTES) {
        
        realSize = (frame[2] & 0x7F) << 8 | frame[3] ;
        deb += 1 ;
        // +1 for the two bytes length
      }

      // Copy the values the output buffer
      // realSize is Channel / Transaction / DATA
      memcpy (&values[counter], &frame[deb], (realSize-2)*sizeof(tscType16)) ;
      counter += (realSize - 2) ;

      // The end value have the EOF frame bit (0x8000)
      values[counter-1] &= ~DD_FIFOTRA_EOFRAME_MASK ;

      // New position in memory
      AL = (al+ah*255+counter) & 0xFF ;
      AH = ((al+ah*255+counter) & 0xFF00) >> 8 ;
    } 
    else {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err, 
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)) |
                                setChannelKey(getChannelKey(index)), 
                                "Memory address",
                                (AH << 16) | AL,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::readFromMemory(multiple bytes): " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": memory channel read error in multiple byte mode, cannot read back the values") ;
    }  
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
void FecDevice::setTriggerChannelCRA ( keyType index, tscType16 value ) {

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
void FecDevice::setTriggerChannelCRB ( keyType index, tscType16 value ) {

  // Check the access
  basicCheckError (index);

  // The basic check is done in the setChannelRegister
  setChannelRegister ( index, CMD_CHANNELTRIGGERWRITECRB, value ) ;
}

/** Method in order to retreive trigger control register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */
tscType16 FecDevice::getTriggerChannelCRA ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELTRIGGERREADCRA)) ;
}

/** Method in order to retreive trigger control register B from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */
tscType16 FecDevice::getTriggerChannelCRB ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELTRIGGERREADCRB)) ;
}

/** Method in order to retreive trigger status register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 */
tscType16 FecDevice::getTriggerChannelSRA ( keyType index ) {

  return ( getChannelRegister ( index, CMD_CHANNELTRIGGERREADSRA)) ;
}

DD_TYPE_FEC_DATA32 FecDevice::getTriggerChannelCounter ( keyType index, 
                                                         int cnt ) throw (FecExceptionHandler) {

  // Check the access
  basicCheckError (index) ;

#ifdef T3TDLEVELCHECK
  ccuMapAccessedType::iterator ccu = ccuMapAccess_[getRingKey(index)].find(getCcuKey(index)) ;
  
  if (! ccu->second->isCcu25()) {  
    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                FECSUPERIVSORSUBSYSTEM, 
                                TSCFEC_INVALIDOPERATION_ERRORSEVERITY,
                                TSCFEC_INVALIDOPERATION,
                                setFecKey(getFecKey(index)) | 
                                setRingKey(getRingKey(index)) | 
                                setCcuKey(getCcuKey(index)), 
                                "trigger channel counter register", (CMD_CHANNELTRIGGERREADCNT0 + cnt),
                                TSCFEC_INVALIDOPERATION_UMSG, 
                                "FecDevice::getTriggerChannelCounter: " +  
                                TSCFEC_INVALIDOPERATION_DMSG + 
                                ": old CCU not supported") ;
  }
#endif
  
  // read the register and write the new one with the correct value
  tscType16 sizet = 3 ;
  tscType16 frame[DD_USER_MAX_MSG_LENGTH] = { getCcuKey(index), FRAMEFECNUMBER, 
                                                  sizet, TRIGGERCHANNELNUMBER, 0xFE, (CMD_CHANNELTRIGGERREADCNT0 + cnt) } ;
  DD_TYPE_ERROR lcl_err = writeFrame (frame, &sizet) ;
 
  DD_TYPE_FEC_DATA32 CR = 0 ;

  
    
    // Check the answer
    if ( (lcl_err != DD_RETURN_OK) || (sizet != 3) || (frame[5] & FECACKERROR) ) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  DEVICEDRIVERSUBSYSTEM,
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err,
                                  setFecKey(getFecKey(index)) | 
                                  setRingKey(getRingKey(index)) | 
                                  setCcuKey(getCcuKey(index)), 
                                  "trigger channel CCU register", (CMD_CHANNELTRIGGERREADCNT0 + cnt),
                                  TSCFEC_REGISTERACCESS_UMSG, 
                                  "FecDevice::getTriggerChannelCounter: " +  
                                  TSCFEC_REGISTERACCESS_DMSG + 
                                  ": trigger channel CCU register access error") ;
    }

    // A read operation is performed
    lcl_err = readFrame(frame[4], frame) ;
    




      // The read value is 32 bits seperated in frame[5] to frame[8]

      // Check the size of the frame
      // With the data read you find the status, eof, crc, addr seen, ...
      if ( (lcl_err != DD_RETURN_OK) || (frame[2] != 6) ) {

        throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                    DEVICEDRIVERSUBSYSTEM,
                                    TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                    lcl_err,
                                    setFecKey(getFecKey(index)) | 
                                    setRingKey(getRingKey(index)) | 
                                    setCcuKey(getCcuKey(index)), 
                                    "trigger channel CCU register", (CMD_CHANNELTRIGGERREADCNT0 + cnt),
                                    TSCFEC_REGISTERACCESS_UMSG, 
                                    "FecDevice::getTriggerChannelCounter: " +  
                                    TSCFEC_REGISTERACCESS_DMSG + 
                                    ": cannot read back a trigger counter register") ;
      }
      
      // Get the value without the end of frame and status
      frame[7] &= (~DD_FIFOTRA_EOFRAME_MASK) ;
      CR = ( frame[8] | (frame[7] << 8) | (frame[6] << 16) | (frame[5] << 24) ) ;
    

  return (CR) ;  
}


// -------------------------------------------------------------------------------------
//
//                                For the FEC/PLX/TTCRx/Optical FEC functionnalites
//
// -------------------------------------------------------------------------------------

/** PLX Reset
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 * \warning the sub-layer wait for the PLX to complete the reset sequence
 */
void FecDevice::plxReset ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::plxReset: " + 
                                TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

#ifndef EMULATION
  DD_TYPE_ERROR lcl_err = glue_plx_hard_reset(deviceDescriptor_);
#else
  DD_TYPE_ERROR lcl_err = DD_RETURN_OK ;
#endif

  // Wait for the correct status
  int timeout = TIMEOUT ;
  while (! isFecSR0Correct(getFecSR0()) && (timeout-- >= 0)) usleep(100) ;

  // Empty the buffer for the cast from 32 bits to 16 bits
  glue_fec_get_fiforec_item ( deviceDescriptor_, NULL ) ;

  // For each ring delete all CCUs, the ring is reseted
  for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {

    // Delete all CCUs
    for (ccuMapAccessedType::iterator p=ccuMapAccess_[ring].begin();p!=ccuMapAccess_[ring].end();p++)
      delete p->second ; // Remove all the accesses for the CCU device

    ccuMapAccess_[ring].clear() ;
  }

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_),
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::plxReset: " +  
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": PLX reset error") ;
  }

  // Set the input/output as A
  fecInput_  = 0 ;
  fecOutput_ = 0 ;

  setFecCR0 (DD_FEC_ENABLE_FEC) ; // Enable FEC
  setFecCR1 (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS) ; // Clear errors

  // Reset counter internal 
  plxResetPerformed_ ++ ;
}

/** Reset PLX and reconfigure the ring in order to retreive the previous state
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::plxReset ( bool reconf ) throw (FecExceptionHandler) {

  if (! reconf) plxReset( ) ;
  else {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
    if (deviceDescriptor_ == -1) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                  TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, 
                                  TSCFEC_FECDDNOTOPEN,
                                  setFecKey(fecSlot_),
                                  TSCFEC_FECDDNOTOPEN_UMSG, 
                                  "FecDevice::plxReset(bool): " +  
                                  TSCFEC_FECDDNOTOPEN_DMSG) ;
    }

#endif
    
#ifndef EMULATION
    DD_TYPE_ERROR lcl_err = glue_plx_hard_reset(deviceDescriptor_);
#else
    DD_TYPE_ERROR lcl_err = DD_RETURN_OK ;
#endif

    // Wait for the correct status
    int timeout = TIMEOUT ;
    while (! isFecSR0Correct(getFecSR0()) && (timeout-- >= 0)) usleep(100) ;    

    // Empty the buffer for the cast from 32 bits to 16 bits
    glue_fec_get_fiforec_item ( deviceDescriptor_, NULL ) ;

    if (lcl_err != DD_RETURN_OK) {

      throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                  DEVICEDRIVERSUBSYSTEM,
                                  TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                  lcl_err,
                                  setFecKey(fecSlot_),
                                  TSCFEC_REGISTERACCESS_UMSG, 
                                  "FecDevice::plxReset(bool): " +  
                                  TSCFEC_REGISTERACCESS_DMSG + 
                                  ": PLX reset error") ;
    }

    // Set the control register of the FEC to the correct value 
#ifdef INITTTCRX
    setFecCR0 (DD_FEC_ENABLE_FEC | DD_FEC_RESET_TTCRX | DD_FEC_RESET_OUT) ; // Reset TTCRx and FEC
#else
    setFecCR0 (DD_FEC_ENABLE_FEC | DD_FEC_RESET_OUT) ; // Reset FEC 
#endif

    setFecCR1 (DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS)    ; // Clear errors 

    // Set the input/output of the FEC
    tscType16 fecCR0 = DD_FEC_ENABLE_FEC ;
    if (fecInput_  != 0) fecCR0 |= DD_FEC_SEL_SER_IN ;
    if (fecOutput_ != 0) fecCR0 |= DD_FEC_SEL_SER_OUT ;

    setFecCR0 (fecCR0) ;

    // Make the configuration of the CCU
    ccuChannelReConfiguration ( ) ;

    // Reset counter internal 
    plxResetPerformed_ ++ ;
  }
}

/** Disable or enable the receive in FEC optical link
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * </ul>
 */
void FecDevice::disableEnableOpticalLink ( bool enable) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::disableEnableOpticalLink (bool):" +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Enable the receive of clock for the optical link of the FEC
  tscType16 fecCR0Value = getFecCR0() ;

  if (enable) 
    setFecCR0 ( fecCR0Value ) ;
  else
    setFecCR0 ( fecCR0Value | CR0_RECEIVEOPTICAL ) ;

}

/** Disable and enable the receive in FEC optical link
 * \param enable - enable/disable the FEC receive for optical link
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_REGISTERACCESS
 * </ul>
 */
void FecDevice::disableEnableOpticalLink ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::disableEnableOpticalLink: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  // Enable the receive of clock for the optical link of the FEC
  tscType16 fecCR0Value = getFecCR0() ;

  setFecCR0 ( fecCR0Value | CR0_RECEIVEOPTICAL ) ;
  setFecCR0 ( fecCR0Value ) ;

}

/** Not documented and must not be used
 */
void FecDevice::setEnableIRQ ( bool enable ) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::disableEnableOpticalLink: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

#ifndef EMULATION  
  DD_TYPE_ERROR lcl_err ;
#else
  DD_TYPE_ERROR lcl_err = DD_RETURN_OK ;
#endif

#ifndef EMULATION
  if (enable) // Disable the plx interrupts
    lcl_err = glue_plx_enable_irqs(deviceDescriptor_) ;
  else
    lcl_err = glue_plx_disable_irqs(deviceDescriptor_) ;
#endif

  if (lcl_err != DD_RETURN_OK ) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                TSCFEC_REGISTERACCESS_ERRORSEVERITY,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                "Enable/disable PLX interrupts", 0,
                                TSCFEC_REGISTERACCESS_UMSG, 
                                "FecDevice::setEnableIRQ: " + 
                                TSCFEC_REGISTERACCESS_DMSG + 
                                ": cannot access to the PLX interrupt register") ;
  }
}

/** Set the control register 0 for clock polarity (SEL_RECEIVE_CLK_POL)
 */
void FecDevice::setInvertClockPolarity ( ) {

  setFecRegister ( DD_FEC_CTRL0_OFFSET, 0x80, CMD_XOR) ;
  clockPolarity_ = !clockPolarity_ ;
}

// -------------------------------------------------------------------------------------
//
//                          For the device driver itself 
//
// -------------------------------------------------------------------------------------

/**
 * \param enable - enable / disable
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_FECPARAMETERNOTMANAGED link with the device driver error (code consistency error)
 * </ul>
 */
void FecDevice::setErrorCounting ( bool enable ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::setErrorCounting: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  tscType32 paramCmd[DD_NB_OF_ERRORS_TO_SPY] ;

  if (enable)
    paramCmd[0] = DD_TURN_REM_ON; 
  else
    paramCmd[0] = DD_TURN_REM_OFF ;

#ifndef EMULATION
  DD_TYPE_ERROR lcl_err = glue_fec_ring_error_manager(deviceDescriptor_, paramCmd) ;
#else
  DD_TYPE_ERROR lcl_err = DD_RETURN_OK ;
#endif

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                SOFTWAREERRORCODE,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                TSCFEC_UNABLETOPERFORMOPERATION_UMSG,
                                "FecDevice::setErrorCounting (bool): " + 
                                TSCFEC_UNABLETOPERFORMOPERATION_DMSG +
                                ": code consistency error") ;
  }
}

/** Reset all the error counters
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_FECPARAMETERNOTMANAGED link with the device driver error (code consistency error)
 * </ul>
 */
void FecDevice::resetErrorCounters ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::resetErrorCounters: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

#ifndef EMULATION
  tscType32 paramCmd[DD_NB_OF_ERRORS_TO_SPY] = {DD_RESET_REM_VALUE} ;
  DD_TYPE_ERROR lcl_err = glue_fec_ring_error_manager(deviceDescriptor_, paramCmd) ;
#else 
  DD_TYPE_ERROR lcl_err = DD_RETURN_OK ;
#endif

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                SOFTWAREERRORCODE,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::resetErrorCounters: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG +
                                ": code consistency error") ;
  }

  // Reset counter internal 
  plxResetPerformed_ = 0 ;
  fecResetPerformed_ = 0 ;
}


/**
 * \return FEC device driver status, where the LSB is the driver status and the 5 MSB is the
 * FEC slot.
 * \see keyType.h for the definition of the FEC slot bits
 * \see DeviceDriver/includes/defines.h for the status bits
 * \warning if the error counting is not started (\see  FecDevice::setErrorCounting)
 * the number of errors are not set and the status doesn't signal it. But if you start
 * the error counting this number stays until a reset counter (even if you disable
 * the error counting)
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>TSCFEC_FECPARAMETERNOTMANAGED link with the device driver error (code consistency error)
 * </ul>
 */
tscType32 FecDevice::getDeviceDriverStatus ( ) throw (FecExceptionHandler) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::getDeviceDriverStatus: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  tscType16 status = 0 ;

  DD_TYPE_ERROR lcl_err = 
#ifndef EMULATION
    glue_fec_get_driver_general_status (deviceDescriptor_, &status) ;
#else
    DD_RETURN_OK ;
#endif

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                SOFTWAREERRORCODE,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::getDeviceDriverStatus: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG +
                                ": code consistency error") ;
  }

  tscType32 fecDDStatus = setFecKey (fecSlot_) | status ;

  return fecDDStatus ;
}

/**
 * \param paraCmd - array of 6 (DD_NB_OF_ERRORS_TO_SPY+3) tscType32:
 * <ul>
 * <li> Status of the device driver (\see FecDevice::getDeviceDriverStatus) 
 *      coded with the slot of the device driver
 * <li> Bad frame - too long frames
 * <li> Bad frame - too short frames
 * <li> Bad frame - invalid transaction number
 * <li> Warning emitted by a device driver
 * <li> Number of PLX reset since the last reset counter
 * <li> Number of FEC reset since the last reset counter
 * </ul>
 * \exception FecExceptionHandler
 * <ul>
 * <li>TSCFEC_FECPARAMETERNOTMANAGED link with the device driver error (code consistency error)
 * </ul>
 */
void FecDevice::getErrorCounters ( tscType32 *errorCounter ) throw (FecExceptionHandler) {

  // The F1STLEVELCHECK is done in the FecDevice::getDeviceDriverStatus 
  errorCounter[0] = getDeviceDriverStatus ( ) ;

  // Retreive the number of errors in the driver
  tscType32 paramCmd[DD_NB_OF_ERRORS_TO_SPY] = {DD_GET_REM_VALUE} ;
  DD_TYPE_ERROR lcl_err = 
#ifndef EMULATION
    glue_fec_ring_error_manager(deviceDescriptor_, paramCmd) ;
#else
    DD_RETURN_OK ;
#endif

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                SOFTWAREERRORCODE,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::getErrorCounters (1): " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG +
                                ": code consistency error") ;
  }

  errorCounter[1] = paramCmd[DD_LONG_FRAME_ERROR_OFFSET] ;
  errorCounter[2] = paramCmd[DD_SHORT_FRAME_ERROR_OFFSET] ;
  errorCounter[3] = paramCmd[DD_WRONG_TRANS_NUMBER_ERROR_OFFSET] ;
  errorCounter[4] = paramCmd[DD_WARNING_FROM_DEVICE_ERROR_OFFSET] ;

  tscType32 ddCounter ;
#ifndef EMULATION
  lcl_err = glue_fec_get_plx_reset_counter (deviceDescriptor_, &ddCounter) ;
#endif

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                SOFTWAREERRORCODE,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::getErrorCounters (2): " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG +
                                ": code consistency error") ;
  }
  errorCounter[5] = ddCounter ;

#ifndef EMULATION
  lcl_err = glue_fec_get_fec_reset_counter (deviceDescriptor_, &ddCounter) ;
#endif

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                SOFTWAREERRORCODE,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::getErrorCounters (3): " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG +
                                ": code consistency error") ;
  }
  errorCounter[6] = ddCounter ;

#ifdef DEBUGMSGERROR

  printf ("Status = 0x%x\n", errorCounter[0]) ;

  if (isErrorCountingEnabled(errorCounter[0]))
    cout << "Error counting on" << endl ;
  else
    cout << "Error counting off" << endl ;
  if (isIrqOn( errorCounter[0]))
    cout << "IRQ is enabled in the device driver" << endl ;
  else
    cout << "IRQ is disabled in the device driver" << endl ;

  // Retreive the number of errors in the driver
  if (existsFaultOnLongFrames(errorCounter[0]))
    cout << errorCounter[1] << " errors on too long frame (bad frames)" << endl ;
  if (existsFaultOnShortFrames(errorCounter[0]))
    cout << errorCounter[2] << " errors on too short frame (bad frames)" << endl ;
  if (existsFaultOnTransNumber(errorCounter[0]))
    cout << errorCounter[3] << " errors on bad transaction number (bad frames)" << endl ;
  if (existsWarningEmittedByDevice(errorCounter[0])) {
    cout << errorCounter[4] << " warning was emitted by some I2C devices" << endl ;

    try {
      tscType16 frame[DD_USER_MAX_MSG_LENGTH] ;
      readFrame ( 0x0, frame ) ;  // The frame are displayed directly in the method
    }
    catch (FecExceptionHandler &e) {
      cerr << "------------------- WARNING -----------------" << endl ;
      cerr << "Cannot read the warning message" << endl ;
      cerr << e.what() << endl ;
      cerr << "---------------------------------------------" << endl ;
    }

  }

  cout << errorCounter[5] << " PLX reset was performed since the last reset on counter" << endl ;
  cout << errorCounter[6] << " FEC reset was performed since the last reset on counter" << endl ;

#endif

}

/** This method give the number of user already connected to the driver include
 * this process
 * \return user counter
 * \exception
 * <ul>
 * <li>TSCFEC_FECDDNOTOPEN
 * <li>SOFTWAREERRORCODE
 * </ul>
 */
tscType32 FecDevice::getUserCounter ( ) {

#ifdef F1STLEVELCHECK

  // Check if the driver is opened
  if (deviceDescriptor_ == -1) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, FECSUPERVISORSUBSYSTEM, 
                                TSCFEC_FECDDNOTOPEN_ERRORSEVERITY, TSCFEC_FECDDNOTOPEN,
                                setFecKey(fecSlot_),
                                TSCFEC_FECDDNOTOPEN_UMSG, 
                                "FecDevice::getDeviceDriverStatus: " +  TSCFEC_FECDDNOTOPEN_DMSG) ;
  }

#endif

  int userCount = 0 ;
  DD_TYPE_ERROR lcl_err = 
#ifndef EMULATION
    glue_fecdriver_get_users_counter(deviceDescriptor_, &userCount);
#else
    DD_RETURN_OK ;
#endif

  if (lcl_err != DD_RETURN_OK) {

    throw FecExceptionHandler ( FECSUPERVISORSYSTEM, 
                                DEVICEDRIVERSUBSYSTEM,
                                SOFTWAREERRORCODE,
                                lcl_err,
                                setFecKey(fecSlot_), 
                                TSCFEC_FECPARAMETERNOTMANAGED_UMSG,
                                "FecDevice::getUserCounter: " + 
                                TSCFEC_FECPARAMETERNOTMANAGED_DMSG +
                                ": code consistency error") ;
  }

  return ((tscType32)userCount) ;
}

