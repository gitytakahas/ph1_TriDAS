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
#ifndef FECRINGDEVICE_H
#define FECRINGDEVICE_H

/** For hash_map
 */
#include "hashMapDefinition.h"

/** C++ STL
 */
#include <list>

/** Type definition
 */
#include "tscTypes.h"
#include "keyType.h"
#include "deviceFrame.h"

/** Exceptions
 */
#include "FecExceptionHandler.h"

/** Class
 */
#include "CCUDescription.h"
#include "FecRingRegisters.h"
#include "TkRingDescription.h"

/** Type of the ring of the CCU in order to store definition
 * of the CCU like the input/output channel
 */
typedef Sgi::hash_map<keyType, CCUDescription *> ccuMapAccessedType ;

// firmware version needed to use the method in order to parallelize on the rings
#define MINFIRMWAREVERSION 0x16

/**
 * \class FecRingDevice
 * This class gives all the FEC, ring hardware access needed. 
 * <BR>It's the only class that is able to write something to the hardware. 
 * <BR>It is able to read/write to all the channels but also all the CCU/FEC/Channel
 * registers. All the accesses are done via the FEC device driver throught the glue
 * library.
 * \warning The CCU type is automatically detected but you the scan ring can only be used
 * with the CCU 25
 * \warning The hash table used in this class (and in the fecAccess and
 * fecSupervisor classes) is based on a key (called also index) defined in the file:
 * \include keyType.h
 * \author Frederic Drouhin, Laurent Gross
 * \date April 2002
 * \brief Class for the FEC hardware accesses.
 */
class FecRingDevice {

 private:

  /** FEC slot
   */
  tscType8 fecSlot_ ;

  /** ring on which we will work, initialized in/by constructor  
   */
  tscType8 ringSlot_;
	
  /** FEC type
   */
  enumFecBusType fecType_ ;

  /** Number of access via this FEC. An access is defined by any
   * channel access from the fecAccess class
   */
  int accessNumber_    ;

  /** CCU connected to each ring for this FEC
   */
  ccuMapAccessedType ccuMapAccess_;

  /** Order of the previous CCU for each ring for this FEC
   */
  CCUDescription *ccuMapOrder_[MAXCCU] ;

  /** Number of CCU for this ring
   */
  unsigned int nbCcuOnTheRing_ ;

  /** Reconfiguration running ?
   */
  bool reconfigurationRunning_ ;

#ifdef DEBUGGETREGISTERS
  /** Read the registers in case of error in the write and read frame methods
   * by default that program readout all registers in case of error [by default true], in some case this features must be disactivated in the cae, 
   * for example of scanring, scantrackerdevice, etc. This readout can be remove from the compilation by editing the FecSoftwareV3_0/FecHeader.linux 
   * and put no for the tag DO_DEBUGGETREGISTERS
   */
  bool readRegistersError_ ;
#endif

  /** Current Transaction Number (already used by a frame)
   */
  tscType8 transactionNumber_ ;

  /** frame send over the ring, request frame
   */
  tscType8 frameRequest_[DD_USER_MAX_MSG_LENGTH*4] ;

  /** direct acknowledge
   */
  tscType8 dAck_[DD_USER_MAX_MSG_LENGTH*4] ;

  /** force acknowledge or read answer
   */
  tscType8 fAck_[DD_USER_MAX_MSG_LENGTH*4] ;

  /** History of the frame that has been sent
   */
#define MAXHISTORYFRAME 10
  tscType8 frameHistory_[MAXHISTORYFRAME][DD_USER_MAX_MSG_LENGTH*4] ;
  unsigned int frameHistoryCounter_ ;
  bool turnOne_ ;

  /** transmit fifo size (depth) in D32 words 
   */
  unsigned int transmitFifoDepth_ ; 

  /** receive fifo size (depth) in D32 words 
   */
  unsigned int receiveFifoDepth_ ;

  /** return fifo size (depth) in D32 words 
   */
  unsigned int returnFifoDepth_ ;

  /** fec device firmware version 
   */
  unsigned int firmwareVersion_ ;


  /** \brief Method for basic check
   */
  void basicCheckError ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief connect and initialise a CCU
   */
  bool connectCCU ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief reconfiguration if the ring after a reset 
   */
  void ccuChannelReConfiguration ( ) throw (FecExceptionHandler) ;

  /** \brief write bytes into the memory but used two bytes for the length
   * \see writeIntoMemory
   */
  void writeIntoMemory2BytesLength ( keyType index, 
                                     tscType8 AH, tscType8 AL, 
                                     tscType8 *values,
                                     unsigned long size ) throw (FecExceptionHandler) ;

  /** To know if a transaction number is used or not
   */
  bool busyTransactionNumber[MAXTRANSACTIONNUMBER+1] ;

  /** Time of the allocation of the transaction number
   */
  time_t timeTransactionNumber[MAXTRANSACTIONNUMBER+1] ;

 protected:

  /** Clock return polarity
   */
  bool invertClockPolarity_ ;

  /** number of loop to wait before making a check of the FIFO transmit running
   * the final timeout will be multiplied by 2 => it represents the time to send the frame when the toggle on the bit send is done until the FIFO transmit is empty
   */
  unsigned long loopInTimeWriteFrame_ ;

  /** number of loop to wait before making the read of the direct acknowledge
   * the final timeout will be multiplied by 2 => it represents the time to wait the direct acknowledge so the time once the frame is sent until the time when the pending IRQ is arrived
   */
  unsigned long loopInTimeDirectAck_ ;

  /** number of loop to wait before making a read frame or read force acknowledge
   * the final timeout will be multiplied by 10 => it represents the time between the direct acknowledge is received until the the time when the data to FEC is arrived
   */
  unsigned long loopInTimeReadFrame_ ;

 public:

  // ---------------------------- Constructors and destructor

  /** \brief Build an interface to the correct FEC (PCI, VME, USB)
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot on the corresponding FEC
   * \param fecType - type of the FEC (PCI, VME, USB)
   */
  FecRingDevice ( tscType8 fecSlot, tscType8 ringSlot, enumFecBusType fecType )  ;

  /** \brief close the device driver open in the constructor
   */
  virtual ~FecRingDevice ( ) ;

  /** \brief Check the FEC and initialise it
   */
  void setInitFecRingDevice ( bool init, bool invertClockPolarity = false ) throw (FecExceptionHandler) ;
  
  // ----------------------------------------------- Software methods

  /** \brief set the invert clock polarity flag (no access to the hardware)
   */
  inline void setInvertClockPolarityFlag ( bool invertClockPolarity ) { invertClockPolarity_ = invertClockPolarity ; }

  /** \brief return the clock polarity flag (no access to the hardware)
   */
  inline bool getClockPolarityFlag ( ) { return invertClockPolarity_ ; }

  /** \brief To retreive the FEC slot
   */
  tscType8 getFecSlot ( ) ;

  /** \brief To retreive the RING slot
   */
  tscType8 getRingSlot ( ) ;

  /** \brief To retreive the FEC type
   */
  tscType8 getFecType ( ) ;

  /** \brief counter to the number of this device driver access
   */
  void addNewAccess ( ) ;

  /** \brief counter to the number of this device driver access
   */
  void removeAccess ( ) ;

  /** \brief enable or disable the read registers in case of error
   */
  void setReadRegistersError ( bool readRegisters ) ;

  /** \brief return the value of the read register parameter
   */
  bool getReadRegistersError ( ) ;

  /** \brief return transmit fifo size (depth) in D32 words 
   */
  unsigned int getTransmitFifoDepth () ;

  /** \brief return receive fifo size (depth) in D32 words 
   */
  unsigned int getReceiveFifoDepth () ;

  /** \brief return fifo size (depth) in D32 words 
   */
  unsigned int getReturnFifoDepth () ;

  /** Set the reconfiguration flag
   * /param flag - configuration flag
   */
  inline void setReconfigurationRunning(bool flag) { reconfigurationRunning_ = flag; }; 
  
  /** Check if the configuration is running
   * \return reconfiguration running
   */
  inline bool getReconfigurationRunning() { return reconfigurationRunning_ ; }; 

  /** \brief return the current transaction number
   */
  tscType8 getTransactionNumber ( ) ;

  /** \brief return the current transaction number and increment it 
   * \warning, this method must take care about the use of the transaction and not only increment it
   */
  tscType8 getNextTransactionNumber ( ) ;

  /** \brief release a transaction number after its use
   */
  void releaseTransactionNumber ( tscType8 transactionNumber ) ;

  /** \brief set the timeout for the wait of FIFO transmit running
   * \warning please do not use this method you will have errors in the system !
   */
  inline void setLoopInTimeWriteFrame ( unsigned long loopInTimeWriteFrame ) {
    std::cerr << "WARNING: setting of the timeout can affect your hardware" << std::endl ;
    loopInTimeWriteFrame_ = loopInTimeWriteFrame ;
  }

  /** \brief return the value of the timeout for the wait of FIFO transmit running
   */
  inline unsigned long getLoopInTimeWriteFrame ( ) {
    return loopInTimeWriteFrame_ ;
  }

  /** \brief set the timeout for the wait of the direct ack
   * \warning please do not use this method you will have errors in the system !
   */
  inline void setLoopInTimeDirectAck ( unsigned long loopInTimeDirectAck ) {
    std::cerr << "WARNING: setting of the timeout can affect your hardware" << std::endl ;
    loopInTimeDirectAck_ = loopInTimeDirectAck ;
  }

  /** \brief return the value of the timeout for the wait of direct ack
   */
  inline unsigned long getLoopInTimeDirectAck ( ) {
    return loopInTimeDirectAck_ ;
  }

  /** \brief set the timeout for the wait of the force ack or answer
   * \warning please do not use this method you will have errors in the system !
   */
  inline void setLoopInTimeReadFrame ( unsigned long loopInTimeReadFrame ) {
    std::cerr << "WARNING: setting of the timeout can affect your hardware" << std::endl ;
    loopInTimeReadFrame_ = loopInTimeReadFrame ;
  }

  /** \brief return the value of the timeout for the wait of FIFO transmit running
   */
  inline unsigned long getLoopInTimeReadFrame ( ) {
    return loopInTimeReadFrame_ ;
  }

  // ----------------------------------------------- Write / Read frame

  /** \brief Write a frame
   */
  virtual void writeFrame ( tscType8 *frame, bool ignoreDirectAck = false ) throw (FecExceptionHandler) ;

  /** \brief Write a frame with nanosleep delay (use for the broadcast operation on the ring)
   */
  virtual void writeFrameDelayed ( tscType8 *frame, bool ignoreDirectAck ) throw (FecExceptionHandler) ;

  /** \brief read a frame
   */
  virtual void readFrame ( tscType8 transaction, tscType8 *frame, tscType8 expectedSize = 0 )
    throw (FecExceptionHandler) ;

  // ----------------------------------------------- FEC methods

  /** \brief set the input of the FEC
   */
  void setFecInputB ( bool inputB ) throw (FecExceptionHandler) ;

  /** \brief set the output of the FEC
   */
  void setFecOutputB ( bool outputB ) throw (FecExceptionHandler) ;

  /** \brief invert the clock polarity
   */
  void setInvertClockPolarity ( bool invertPolarity ) throw (FecExceptionHandler) ;

  /** \brief use the internal clock
   */
  void setInternalClock ( bool internal ) throw (FecExceptionHandler) ;

  /** \brief Set FEC control register 0 by read modify operation
   */
  void setFecRingCR0 ( tscType16 mask, logicalOperationType op ) throw (FecExceptionHandler) ;

  /** \brief Reset the FEC FSM
   */
  void fecRingResetFSM ( ) throw (FecExceptionHandler) ;

  /** \brief Reset the FEC
   */
  void fecRingReset ( ) throw (FecExceptionHandler) ;

  /** \brief Reset the FEC
   */
  void fecRingResetB ( ) throw (FecExceptionHandler) ;

  /** \brief this method must be issued after a hard reset
   */
  void initAfterHardReset ( ) throw (FecExceptionHandler) ;

  /** \brief Check and try to recover the ring (without reconfiguration)
   */
  void checkRing ( ) throw (FecExceptionHandler) ;

  /** \brief Regenerate a token on the CCU ring
   */
  void fecRingRelease ( ) throw (FecExceptionHandler) ;

  /** \brief empty the FIFO receive
   */
  virtual void emptyFifoReceive ( ) throw (FecExceptionHandler) ;

  /** \brief empty the FIFO return
   */
  virtual void emptyFifoReturn ( ) throw (FecExceptionHandler) ;

  /** \brief empty the FIFO transmit
   */
  virtual void emptyFifoTransmit ( ) throw (FecExceptionHandler) ;

  /** \brief check the FIFO receive, mainly used in the write frame in case of bad status
   */
  void checkFifoReceive ( int display = 0 ) ;

  /** \brief Invert the clock polarity
   */
  void setInvertClockPolarity ( ) throw (FecExceptionHandler) ;

  // ------------------------ For the CCU ring scanned

  /** \brief This method scan the rings connected to the FEC and check if the order of the CCU
   */
  void fecScanRingBroadcast ( ) throw (FecExceptionHandler) ;

  /** \brief This method scan the rings connected to the FEC and check if the order of the CCU
   */
  void fecScanRingNoBroadcast ( ) throw (FecExceptionHandler) ;

  // ------------------------ For the ring reconfiguration

  /** \brief this method reconfigure the ring of the CCU because of a problem on it
   */
  tscType16 fecRingReconfigure ( ) throw (FecExceptionHandler) ;

  /** \brief this method reconfigure the ring of the CCU because of a problem on it
   */
  tscType16 fecRingReconfigure ( TkRingDescription &tkRing ) throw (FecExceptionHandler) ;

  // --------------------------- For the CCU registers

  /** \brief Get the CCU type (CCU 25 or old CCU)
   */
  bool getCcuType ( keyType index,
                    tscType32 *CRE ) throw (FecExceptionHandler) ;

  /** \brief Get the CCU type (CCU 25 or old CCU)
   */
  bool getCcuType ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Set a CCU register
   */
  void setCcuRegister ( keyType index, 
                        tscType8 ccuRegister, 
                        tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get a CCU register
   */
  tscType32 getCcuRegister ( keyType index, 
                             tscType8 ccuRegister ) throw (FecExceptionHandler) ;

  /** \brief Set a CCU register (read-modify-write operation)
   */
  void setCcuRegister ( keyType index, 
                        tscType8 ccuRegister, 
                        tscType8 value,
                        logicalOperationType op ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register A
   */
  void setCcuCRA ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  // setting CRA with ReadModifyWrite is useless as the CRA's bits are ever toggled to 0x0
  // void setCcuCRA ( keyType index, tscType8 value, logicalOperationType op ) ;

  /** \brief Get CCU control register A
   */
  tscType8 getCcuCRA ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register B
   */
  void setCcuCRB ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register B (read-modify-write operation)
   */
  void setCcuCRB ( keyType index, tscType8 value, logicalOperationType op ) throw (FecExceptionHandler) ;

  /** \brief Get CCU control register B
   */
  tscType8  getCcuCRB ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register C
   */
  void setCcuCRC ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register C (read-modify-write operation)
   */
  void setCcuCRC ( keyType index, tscType8 value, logicalOperationType op ) throw (FecExceptionHandler) ;

  /** \brief Get CCU control register C
   */
  tscType8  getCcuCRC ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register D
   */
  void setCcuCRD ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register D (read-modify-write operation)
   */
  void setCcuCRD ( keyType index, tscType8 value, logicalOperationType op ) throw (FecExceptionHandler) ;

  /** \brief Get CCU control register D
   */
  tscType8  getCcuCRD ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register E
   */
  void setCcuCRE ( keyType index, tscType24 value ) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register E
   */
  void setCcuCRE ( keyType index, 
                   tscType8 valueI2C, 
                   tscType8 valuePIA,
                   tscType8 valueMemory,
                   tscType8 valueTrigger,
                   tscType8 valueJTAG) throw (FecExceptionHandler) ;

  /** \brief Set CCU control register E (read-modify-write operation)
   */
  void setCcuCRE ( keyType index, tscType24 value, logicalOperationType op ) throw (FecExceptionHandler) ;

  /** \brief Get CCU control register E
   */
  tscType24 getCcuCRE ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get CCU status register A
   */
  tscType8  getCcuSRA ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get CCU status register B
   */
  tscType8  getCcuSRB ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get CCU status register C
   */
  tscType8  getCcuSRC ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get CCU status register D
   */
  tscType8  getCcuSRD ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get CCU status register E
   */
  tscType24 getCcuSRE ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get CCU status register F
   */
  tscType16 getCcuSRF ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get CCU status register G
   */
  tscType8 getCcuSRG ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get CCU status register H
   */
  tscType8 getCcuSRH ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief this method return a list of CCUs scanned
   */
  std::list<keyType> *getCcuList ( bool noBroadcast = false, bool scan = true ) throw (FecExceptionHandler) ;

  /** \brief read all the registers (status and control) from FEC and CCUs
   */
  FecRingRegisters getFecRingRegisters ( keyType index ) ;

  // ------------------------------- CCU Alarms

  /** \brief Enable or disable CCU alarms and PIA channel interrupts
   */
  void setCcuAlarmsEnable ( keyType index, bool enable1, bool enable2, bool enable3, bool enable4 ) throw (FecExceptionHandler) ;

  /** \brief Wait for any CCU alarms
   */ 
  void waitForAnyCcuAlarms ( keyType index, tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ) throw (FecExceptionHandler) ;

  /** \brief Wait for any CCU alarms
   */ 
  void waitForAnyCcuAlarms ( tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ) throw (FecExceptionHandler) ;

  /** \brief Enable the PIA interrupts
   */
  void setPiaInterruptEnable ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Clear the interrupt on PIA channels
   */
  void setPiaClearInterrupts ( keyType index ) throw (FecExceptionHandler) ;

  // -------------------------------- Channel methods

  /** \brief Enable the channel corresponding to the key
   */
  void setChannelEnable   ( keyType index, bool enable ) throw (FecExceptionHandler) ;

  /** \brief Get a channel register
   */
  tscType8 getChannelRegister ( keyType index, 
                                          tscType8 channelRegister ) throw (FecExceptionHandler) ;
  
  /** \brief Set a channel register
   */
  void setChannelRegister ( keyType index, 
                            tscType8 channelRegister, 
                            tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Set a channel register (read-modify-write operation)
   */
  void setChannelRegister ( keyType index, 
                            tscType8 channelRegister, 
                            tscType8 value,
                            logicalOperationType op ) throw (FecExceptionHandler) ;

  // -------------------------- For the PIA channel registers

  /** \brief Initialisethe PIA channel general control register
   */
  void setInitPiaChannel ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Set the PIA channel general control register
   */
  void setPiaChannelGCR ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Set the PIA channel general control register (read-modify-write operation)
   */
  void setPiaChannelGCR ( keyType index, tscType8 value, logicalOperationType op ) throw (FecExceptionHandler) ;

  /** \brief Get the PIA channel general control register
   */
  tscType8 getPiaChannelGCR ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get the PIA channel status register
   */
  tscType8 getPiaChannelStatus ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Set the PIA channel data direction register
   */
  void setPiaChannelDDR ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Set the PIA channel data direction register (read-modify-write operation)
   */
  void setPiaChannelDDR ( keyType index, tscType8 value, logicalOperationType op ) throw (FecExceptionHandler) ;

  /* \brief Get the PIA channel data direction register 
   */
  tscType8 getPiaChannelDDR ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Set the PIA channel data register
   */
  void setPiaChannelDataReg ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Set the PIA channel data register (read-modify-write operation)
   */
  void setPiaChannelDataReg ( keyType index, 
                              tscType8 value, 
                              logicalOperationType op ) throw (FecExceptionHandler) ;

  /** \brief Get the PIA channel data register
   */
  tscType8 getPiaChannelDataReg ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Reset a PIA channel
   */
  void piaChannelReset ( keyType index ) throw (FecExceptionHandler) ;

  // -------------------------- For the i2c channel registers

  /** \brief Initialise the channel control register A and the force ack bit
   */
  void setInitI2cChannelCRA ( keyType index, bool forceAck=false, tscType16 i2cSpeed=0 ) throw (FecExceptionHandler) ;

  /** \brief Set the i2c channel control register A
   */
  void seti2cChannelCRA ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Get the i2c channel control register A
   */
  tscType8 geti2cChannelCRA ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get the i2c channel status register A
   */
  tscType8 geti2cChannelSRA ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get the i2c channel status register B
   */
  tscType8 geti2cChannelSRB ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get the i2c channel status register C
   */
  tscType8 geti2cChannelSRC ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Get the i2c channel status register D
   */
  tscType8 geti2cChannelSRD ( keyType index ) throw (FecExceptionHandler) ;
  
  /** \brief Reset an I2C channel
   */
  void i2cChannelReset  ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Reset an I2C channel and reconfigure the previous state
   */
  void i2cChannelReset  ( keyType index, bool reconf ) throw (FecExceptionHandler) ;

  /** \brief test if an i2c channel is enable or not 
   */
  bool isChannelEnabled ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief test if an i2c channel is busy or not 
   */
  bool isChannelBusy ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief scan the ring for i2c devices
   */
  std::list<keyType> *scanRingForI2CDevice ( keyType *deviceValues, 
                                        tscType32 sizeDevices,
					bool noBroadcast = false,
					bool display = false ) 
    throw (FecExceptionHandler) ;

  /** \brief scan the ring for i2c devices
   */
  std::list<keyType> *scanRingForI2CDevice ( bool noBroadcast = false, bool display = false ) 
    throw (FecExceptionHandler) ;

  // --------------------- I2C channels

  /** \brief Force the acknowledge bit for this i2c channel
   */
  void setChannelForceAck ( keyType index, bool fack ) throw (FecExceptionHandler) ;

  /** \brief Write a value to a device connected onto a I2C CCU channel
   */
  void writei2cDevice ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Write a value to a device connected onto a I2C CCU channel
   */
  void writei2cDevice ( keyType index, tscType8 offset, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Write a value to a device connected onto a I2C CCU channel
   */
  void writei2cRalDevice ( keyType index, tscType8 offset, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Write a value to a device connected onto a I2C CCU channel
   */
  void writei2cDeviceOffset ( keyType index, 
                              tscType8 offset, 
                              tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Write a value to a device connected onto a I2C CCU channel
   */
  void writei2cDeviceOffset ( keyType index, 
                              tscType8 offset, 
                              tscType8 decal, 
                              tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief Read a value from a device connected onto a I2C CCU channel
   */
  tscType8 readi2cDevice ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Read a value from a device connected onto a I2C CCU channel
   */
  tscType8 readi2cDevice ( keyType index, tscType8 addressMSB ) throw (FecExceptionHandler) ;

  /** \brief Read a value from a device connected onto a I2C CCU channel
   */
  tscType8 readi2cRalDevice ( keyType index, tscType8 offset ) throw (FecExceptionHandler) ;

  /** \brief Read a value from a device connected onto a I2C CCU channel
   */
  tscType8 readi2cDeviceOffset ( keyType index, tscType8 offset ) throw (FecExceptionHandler) ;

  /** \brief Read a value from a device connected onto a I2C CCU channel
   */
  tscType8 readi2cDeviceOffset ( keyType index, 
                                           tscType8 offset, 
                                           tscType8 decal ) throw (FecExceptionHandler) ;

  // --------------------------- For the memory channel

  /** \brief reset the memory channel
   */
  void memoryChannelReset( keyType index ) throw (FecExceptionHandler) ;

  /** \brief set the memory control register A
   */
  void setMemoryChannelCRA ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief set the memory control register A (read-modify-write operation)
   */
  void setMemoryChannelCRA ( keyType index, tscType8 value, logicalOperationType op  ) throw (FecExceptionHandler) ;

  /** \brief set a memory channel WIN?? register
   */
  void setMemoryChannelWinReg ( keyType index, 
                                tscType8 memoryRegister, 
                                tscType16 value ) throw (FecExceptionHandler) ;

  /** \brief set the memory WIN1L register
   */
  void setMemoryChannelWin1LReg ( keyType index, tscType16 value ) throw (FecExceptionHandler) ;

  /** \brief set the memory WIN1H register
   */
  void setMemoryChannelWin1HReg ( keyType index, tscType16 value ) throw (FecExceptionHandler) ;

  /** \brief set the memory WIN2L register
   */
  void setMemoryChannelWin2LReg ( keyType index, tscType16 value ) throw (FecExceptionHandler) ;

  /** \brief set the memory WIN2H register
   */
  void setMemoryChannelWin2HReg ( keyType index, tscType16 value ) throw (FecExceptionHandler) ;
  
  /** \brief set the memory mask register
   */
  void setMemoryChannelMaskReg ( keyType index, tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief get the memory channel control register A
   */
  tscType8 getMemoryChannelCRA ( keyType index ) throw (FecExceptionHandler) ;
  
  /** \brief get the memory channel mask register
   */
  tscType8 getMemoryChannelMaskReg ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief get a memory channel WIN?? register
   */
  tscType16 getMemoryChannelWinReg ( keyType index,
                                     tscType8 memoryRegister ) throw (FecExceptionHandler) ;

  /** \brief get the memory channel WIN1L register
   */
  tscType16 getMemoryChannelWin1LReg ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief get the memory channel WIN1H register
   */
  tscType16 getMemoryChannelWin1HReg ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief get the memory channel WIN2L register
   */
  tscType16 getMemoryChannelWin2LReg ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief get the memory channel WIN1L register
   */
  tscType16 getMemoryChannelWin2HReg ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief get the memory channel mask register
   */
  tscType8 getMemoryChanneMaskReg ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief get the memory channel status register
   */
  tscType8 getMemoryChannelStatus ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief write a byte into the memory
   */
  void writeIntoMemory ( keyType index, 
                         tscType8 AH, tscType8 AL, 
                         tscType8 value ) throw (FecExceptionHandler) ;

  /** \brief write bytes into the memory
   */
  void writeIntoMemory ( keyType index, 
                         tscType8 AH, tscType8 AL, 
                         tscType8 *values,
                         unsigned long size ) throw (FecExceptionHandler) ;

  /** \brief read modify write on one byte into the memory
   */
  void writeIntoMemory ( keyType index, 
                         tscType8 AH, tscType8 AL,
                         logicalOperationType op, 
                         tscType8 mask ) throw (FecExceptionHandler) ;

  /** \brief read modify write on one byte into the memory
   */
  void writeIntoMemory ( keyType index, 
                         tscType8 AH, tscType8 AL,
                         logicalOperationType op ) throw (FecExceptionHandler) ; 

  /** \brief read a byte into the memory
   */
  tscType8 readFromMemory ( keyType index, 
                            tscType8 AH, tscType8 AL ) throw (FecExceptionHandler) ;

  /** \brief read a byte into the memory
   */
  void readFromMemory ( keyType index, 
                        tscType8 AH, tscType8 AL,
                        unsigned long size,
                        tscType8 *values ) throw (FecExceptionHandler) ;

  // -------------------------- For the trigger channel registers
  
  /** \brief Set the trigger channel CRA
   */
  void setTriggerChannelCRA ( keyType index, tscType8 value );

  /** \brief Set the trigger channel CRB
   */
  void setTriggerChannelCRB ( keyType index, tscType8 value );

  /** \brief Get the trigger channel CRA
   */
  tscType8 getTriggerChannelCRA ( keyType index );

  /** \brief Get the trigger channel CRB
   */
  tscType8 getTriggerChannelCRB ( keyType index );

  /** \brief Set the trigger channel SRA
   */
  tscType8 getTriggerChannelSRA ( keyType index );


  /** \brief Retreive the trigger counter
   */
  tscType32 getTriggerChannelCounter ( keyType index, 
                                       int cnt ) throw (FecExceptionHandler);

  // --------------------------- For the FEC functionnalites

  /** \brief enable or disable receive for FEC optical link
   */
  void setCR0Receive ( bool enable ) throw (FecExceptionHandler) ;

  /** \brief initialise the TTCRx
   */
  void initTTCRx ( ) throw (FecExceptionHandler) ;

  // -------------------------- Virtual methods, must be implemented by the child class (ie. FecVmeRingDevice, FecPciRingDevice, FecUsbRingDevice)
  
  /******************************************************
	Virtual methods for the FEC
  ******************************************************/

  /** \brief Set the control register 0  the FEC
   * \param ctrl0Value - value to be set
   */ 
  virtual void setFecRingCR0 ( tscType16 ctrl0Value, bool force = false ) = 0 ;

  /** \brief Get the control register 0  the FEC
   * \return value read
   */
  virtual tscType16 getFecRingCR0( ) = 0 ;

  /** \brief Set the control register 1  the FEC
   * \param ctrl1Value - value to be set
   */ 
  virtual void setFecRingCR1( tscType16 ctrl1Value ) = 0 ;

  /** \brief Get the control register 1  the FEC
   * \return value read
   */
   virtual tscType16 getFecRingCR1( ) = 0 ;

  /** \brief Get the status register 0  the FEC
   * \return value read
   */
   virtual tscType32 getFecRingSR0(unsigned long sleeptime=0) = 0 ;

  /** \brief Get the status register 1  the FEC
   * \return value read
   */
   virtual tscType16 getFecRingSR1( ) = 0 ;

   /** \brief Get the FEC version register 
   * \return value read
   */
   virtual tscType16 getFecFirmwareVersion( ) throw ( FecExceptionHandler ) = 0 ;
	
  /******************************************************
	FIFO ACCESS - NATIVE 32 BITS FORMAT
	NATIVE FORMAT ACCESS ALLOWS R/W OPERATIONS
   ******************************************************/

  /** \brief return a word from the FIFO receive
   */
  virtual tscType32 getFifoReceive ( ) = 0 ;
	
  /** \brief write a word in the FIFO receive
   * \param value - value to be written
   */
  virtual void setFifoReceive( tscType32 value ) = 0 ;
  
  /** \brief return a word from the FIFO return
   */
  virtual tscType8 getFifoReturn( )  = 0 ;

  /** \brief write a word in the FIFO return
   * \param value - value to be written
   */
  virtual void setFifoReturn( tscType8 value ) = 0 ;
  
  /** \brief return a word from the FIFO transmit
   */
  virtual tscType32 getFifoTransmit( ) = 0 ;

  /** \brief write a word in the FIFO transmit
   * \param value - value to be written
   */
  virtual void setFifoTransmit( tscType32 value ) = 0 ;


   /******************************************************
	HARD RESET
    ******************************************************/	

  /** \brief Hard reset (PCI => PLX reset) (VME => Crate reset) (USB => FEC CORE RESET)
   */
  virtual void fecHardReset ( ) = 0 ;

  /** \brief Enable or disable the IRQ
   */
  virtual void setIRQ ( bool enable, tscType8 level = 1 ) = 0 ;

  /** \brief write a block of words to the FIFO receive
   */
  virtual void setFifoReceive ( tscType32 *value, int count) throw (FecExceptionHandler) ;

  /** \brief get block of words from the FIFO receive
   */
  virtual tscType32* getFifoReceive (  tscType32 *value, int count ) throw (FecExceptionHandler) ;
  
  /** \brief set a word into the FIFO return
   */
  virtual void setFifoReturn ( tscType8 *value, int count ) throw (FecExceptionHandler) ;
  
  /** \brief get a word from the FIFO return
   */
  virtual tscType8* getFifoReturn ( tscType8 *value,int count ) throw (FecExceptionHandler) ;
 
  /** \brief set a word into the FIFO Transmit
   */
  virtual void setFifoTransmit ( tscType32 *value, int count ) throw (FecExceptionHandler) ;
  
  /** \brief get a word from the FIFO transmit
   */
  virtual tscType32* getFifoTransmit (  tscType32 *value,int count ) throw (FecExceptionHandler) ;

  /*****************************************
  Methods for block of frames
  *****************************************/

  /** \brief This method put a frame inside the FIFO transmit
   */
  unsigned int buildI2CFrame ( accessDeviceType b, tscType32 *toBeTransmited, bool forceAcknowledge, unsigned int *fifoRecWord, unsigned int *nWordFifo ) ;

  /** \brief This method put a frame inside the FIFO transmit
   */
  unsigned int buildPIAFrame ( accessDeviceType b, tscType32 *toBeTransmited, unsigned int *fifoRecWord ) ;

  /** This methods takes an array of values to be set and send it over the ring.
   */
  void setBlockDevices ( std::list<accessDeviceType> &vAccessDevices, bool forceAcknowledge ) throw (FecExceptionHandler) ;

  /** This methods retreive the frames sent by another methods and manage it with errors
   */
  void getBlockFrames ( bool forceChannelAck, accessTransactionFrameMap &tnumSent,
			Sgi::hash_map<keyType, int> &busy, unsigned int &cptRead )
    throw (FecExceptionHandler) ;

  /** This methods takes an array of values to be set and send it over the ring.
   */
  void setBlockDevicesBltMode ( std::list<accessDeviceType> &vAccessDevices, bool forceAcknowledge ) throw (FecExceptionHandler) ;

  /** This methods takes an array of values to be set and send it over the ring.
   */
  //void setBlockDevicesBltMode1 ( std::list<accessDeviceType> &vAccessDevices, bool forceAcknowledge ) throw (FecExceptionHandler) ;

};

#endif
