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
#ifndef FECACCESS_H
#define FECACCESS_H

#include <fstream>

#include "hashMapDefinition.h" // For hash_map

// All types
#include "tscTypes.h"
#include "keyType.h"
#include "deviceFrame.h"

// Exceptions
#include "FecExceptionHandler.h"

// FEC hard Access
#include "hal/VME64xCrate.hh"
#include "hashMapVMEDevice.h"
#include "hashMapFecUsbSerial.h"

// Channel access
#include "ccuChannelAccess.h"

// Description access for redundancy
#include "TkRingDescription.h"

// CCS Trigger part
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
#include "CCSTrigger.hh"
#include "FecVmeRingDevice.h"
#include "FecVmeTemperature.h"
#endif

// Definition for the map that store the information
// for the CCU channels initialize and for the device
// that has been connected by the user and the pair
// corresponding to the map
// FecAccess class

// Manage all the FecRingDevice class needed (one per FEC)
typedef Sgi::hash_map<keyType, FecRingDevice *> fecMapAccessedType ;

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
// Manage all the CCSTrigger object (one per FEC)
typedef Sgi::hash_map<keyType, CCSTrigger *> ccsTriggerMapAccessedType ;

// FEC VME Temperature register by FEC slot
typedef Sgi::hash_map<keyType, FecVmeTemperature *> fecTemperatureMapAccessedType ;
#endif

// Manage all the conccurent access to the channels
typedef Sgi::hash_map<keyType, unsigned int> channelMapAccessedType ;

// Manage all the real hardware acesses
typedef Sgi::hash_map<keyType, ccuChannelAccess *> deviceMapAccessedType  ;

// Table with the FEC hardware id to find the index
typedef Sgi::hash_map<const char *, keyType, eqstr, eqstr> fecHardwareIdMapIndexType ;

/**
 * \class FecAccess
 * This class is the main class in this project. It handles all the creation of the FecRingDevice object. Each methods requiered a key to the FEC,ring/CCU/channel or i2c device you want to access.
 * It manages several access:
 * <ul>
 * <li> FEC access: enable access to the CR0, CR1, SR0, SR1,
 * plx and fec reset (see the fec documentation). This
 * functions can only call in the supervisor mode.
 * <li> PIA, i2c and memory access: this class store all the information needed
 * in order to make an access and disable multiple access to
 * a device. Only the supervisor mode can make some action forbidden
 * </ul>
 * This class is the based that
 * It handles the different bus and FEC that can be used:
 * <ul>
 * <li> VME FEC through a PCI to VME interface:
 * <ol>
 * <li> CAEN PCI to VME interface
 * <li> CAEN USB to VME interface
 * <li> SBS with optical or electrical PCI to VME interface
 * </ol>
 * <li> PCI FEC whatever the version of the FEC and firmware:
 * <ol>
 * <li> Old electrical FEC with Latice
 * <li> Optical FEC with firmware 0.3
 * <li> Optical FEC with firmware greater than 1.0
 * <li> New electrical FEC with firmware greater than 1.0
 * </ol>
 * <li> USB FEC (preshower version).
 * </ul>
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class enable all the FEC/CCU/Channel access.
 * \warning Only the I2C and PIA accesses are implemented
 * \warning All the hash tables used in this class (and in the FecRingDevice and
 * fecSupervisor classes) is based on a key (called also index) defined in the file:
 * \include keyType.h
 * \bug This class will used a shared memory in order to manage several accesses from 
 * several processes.
 */
class FecAccess {

 private:

  /** FEC bus type
   */
  enumFecBusType fecBusType_ ;

  /** All the devices are reserved and no other process
   * can access this device until it is release
   * \brief Hash table to know which device is used
   */
  deviceMapAccessedType  deviceEnable_ ;

  /** When a channel is more used, it is set to disable
   * \brief Hash table to know which channel is used
   */
  channelMapAccessedType channelEnable_ ;

  /** All the FecRingDevice are class in this map.
   * Hash table in order to remind which FEC is used
   */
  fecMapAccessedType     fecRingEnable_ ;

  /** Use the force acknowledge for the I2C channels
   */
  bool forceChannelAck_  ;

  /** Use for the block mode for the VME access
   */
  bool blockMode_ ;

  /** Initialise or not the FEC when the FecRingDevice object is created
   */
  bool initFecRingDevice_ ;

  /** I2C speed of the channels
   */
  tscType16 i2cSpeed_ ;

  /** Clock return polarity
   */
  bool invertClockPolarity_ ;

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  /** For VME plug and play (only needed if you are using a VME FEC)
   */
  HAL::VME64xCrate *vme64xCrate_ ;

  /** VME hardware access (only needed if you are using a VME FEC)
   */
  hashMapVMEDevice hardwareDevices_ ;

  /** All the CCS trigger object
   */
  ccsTriggerMapAccessedType ccsTriggerList_ ;
#endif

#if defined(BUSUSBFEC)

  /** FECUSB hardware access (only needed if you are using a USB FEC)
   */
  hashMapFecUsbSerial fecUsbSerialNumbers_ ;  

#endif 

  /** FEC VME ring device
   */
  int reloadedFirmware_[MAX_NUMBER_OF_SLOTS+1] ;

  /** All the temperature object
   */
  fecTemperatureMapAccessedType fecTemperatureList_ ;

  /** Map of the FEC hardware ID to find the corresponding index, only used for VME FEC.
   * To retreive from the index the FEC hardware ID, you need to call the method: getFecIndex and getFecHardwareId
   */
  fecHardwareIdMapIndexType fecHardwareIdMapIndex_ ;

  /** Adapter slot used by the FecAccess
   */
  unsigned adapterSlot_ ;

  /** Store the file for the plug and play
   */
  std::string configurationFile_ ;
  
  /** Store the file for the plug and play
   */
  std::string mapFile_ ;

  /** Bus adapter name
   */
  std::string strBusAdapter_ ;

  /** \brief Check the index and retreive the channel access
   */
  ccuChannelAccess *checkIndex ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Method for the VME plug and play configuration
   */
  void configurePlugAndPlay ( unsigned adapterSlot, std::string configurationFile, std::string mapFile, std::string strBusAdapter ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Method for the base address specification
   */
  void configureBaseAddresses ( unsigned adapterSlot, std::string configurationFile, const uint32_t *baseAddresses, std::string strBusAdapter ) 
  throw ( FecExceptionHandler ) ;

  /** \brief initialise the FEC access 
   */
  void setInitFecAccess ( bool forceAck, bool initFec, bool scanFECs, bool scanCCUs, tscType16 i2cSpeed, 
			  tscType8 fecSlotMin, tscType8 fecSlotMax, tscType8 ringMin, tscType8 ringMax,
			  bool invertClockPolarity ) 
    throw (FecExceptionHandler) ;

 public:

  // ---------------------------- Constructors and destructor

  /** \brief constructor for the PCI FEC Access
   */
  FecAccess ( bool forceAck = false, bool initFec = true, bool scanFECs = false, bool scanCCUs = false, tscType16 i2cSpeed = 100, bool invertClockPolarity = false )
    throw ( FecExceptionHandler ) ;

  /** \brief Constructor with VME plug and play, force acknowledge and scan at the beginning
   */
  FecAccess (hashMapVMEDevice hashMapVMEDevice, 
	     bool forceAck = false, bool initFec = true, bool scanFECs = false, bool scanCCUs = false, tscType16 i2cSpeed = 100, 
	     bool blockMode = false, tscType8 numberOfRing = 8, bool invertClockPolarity = false )
    throw ( FecExceptionHandler ) ;

  /** \brief Constructor with VME plug and play, force acknowledge and scan at the beginning
   */
  FecAccess ( unsigned adapterSlot, 
              std::string configurationFile, std::string mapFile,
              bool forceAck = false, bool initFec = true, bool scanFECs = false, bool scanCCUs = false, tscType16 i2cSpeed = 100,
	      std::string strBusAdapter = "SBS620x86LinuxBusAdapter", bool blockMode = false, tscType8 numberOfRing = 8, bool invertClockPolarity = false )
    throw ( FecExceptionHandler ) ;

  /** \brief Constructor with the specification of the base address for each slot, force acknowledge and scan at the beginning
   */
  FecAccess ( unsigned adapterSlot, std::string configurationFile, 
              const uint32_t *baseAddresses,
	      bool forceAck = false, bool initFec = true, bool scanFECs = false, bool scanCCUs = false, tscType16 i2cSpeed = 100,
	      std::string strBusAdapter = "SBS620x86LinuxBusAdapter", bool blockMode = false, tscType8 numberOfRing = 8, bool invertClockPolarity = false )
    throw ( FecExceptionHandler ) ;

   /** \brief constructor for the USB FEC Access
    */
   FecAccess ( hashMapFecUsbSerial fecUsbSerials, bool forceAck = false, bool initFec = true,
	       bool scanFECs = false, bool scanCCUs = false,
	       tscType16 i2cSpeed = 100, bool invertClockPolarity = false ) throw (FecExceptionHandler ); 

   /** \brief Remove all the accesses and close all the device driver and
    * disable all the channels
    */
  ~FecAccess ( ) ;

  /** \brief delete all VME accesses
   */
  void deleteVmeAccesses ( ) ;

  // ----------------------------------------------- Software methods

  /** \brief Minimum ring number
   */
  tscType16 getMinRingNumber ( ) ;

  /** \brief Minimum ring number
   */
  tscType16 getMaxRingNumber ( ) ;

  /** \brief Set the force acknoledge on i2c channels
   */
  void setForceAcknowledge ( bool forceAck ) ;

  /** \brief Get the force acknoledge on i2c channels
   */
  bool getForceAcknowledge ( ) ;

  /** \brief Initialise all the FecRingDevice
   */
  void setFecRingDeviceInit ( bool initFecRingDevice ) ;

  /** \brief set the i2c speed for the channels
   */
  void seti2cChannelSpeed ( tscType16 i2cSpeed ) ;

  /** \brief retreive the i2c speed
   */
  tscType16 geti2cChannelSpeed ( ) ;

  /** \brief Set the flag for the clock polarity
   */
  void setInvertClockPolarityFlag ( bool invertClockPolarity ) ;

  /** \brief Get the flag for the clock polarity
   */
  bool getInvertClockPolarityFlag ( ) ;
  
  /** \brief Find in the hash_map or create the FecRingDevice corresponding to the index.
   */
  FecRingDevice *setFecRingDevice ( keyType index ) ;

  /** \brief Retreive the value of the FecRingDevice init
   */
  bool getFecRingDeviceInit ( ) ;

  /** \brief Return the FEC bus type
   */
  enumFecBusType getFecBusType ( ) ;

  /** \brief Return the FecRingDevice corresponding to the slot
   */
  FecRingDevice *getFecRingDevice ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief restrieve all the FECs already connected
   */
  std::list<keyType> *getFecList ( ) ;

  /** \brief set the FIFO block mode for the data transfer to the FEC
   */
  void setFifoAccessMode ( bool blockMode ) ;

  /** \brief set the FIFO block mode for the data transfer to the FEC
   */
  bool getFifoAccessMode ( ) ;

  /** \brief set the timeout for the wait of FIFO transmit running
   */
  void setLoopInTimeWriteFrame ( unsigned long loopInTimeWriteFrame ) ;

  /** \brief set the timeout for the wait of the direct ack
   */
  void setLoopInTimeDirectAck ( unsigned long loopInTimeDirectAck ) ;

  /** \brief set the timeout for the wait of the force ack or answer
   */
  void setLoopInTimeReadFrame ( unsigned long loopInTimeReadFrame ) ;

  /**
   * \brief return transmit fifo size (depth) in D32 words 
   */
  unsigned int getTransmitFifoDepth ( keyType index ) throw (FecExceptionHandler) ;

  /**
   * \brief receive fifo size (depth) in D32 words 
   */
  unsigned int getReceiveFifoDepth ( keyType index ) throw (FecExceptionHandler) ;

  /** 
   * \brief return fifo size (depth) in D32 words 
   */
  unsigned int getReturnFifoDepth ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief empty the FIFO receive
   */
  void emptyFifoReceive ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief empty the FIFO return
   */
  void emptyFifoReturn ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief empty the FIFO transmit
   */
  void emptyFifoTransmit ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief clear all errors for FEC and CCUs
   */
  void clearFecCcuErrorRegisters ( keyType index, bool noBroadcast = false ) throw (FecExceptionHandler) ;

  // ----------------------------------------------- FEC methods
  /** \brief Return the corresponding CCS Trigger
   */
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  CCSTrigger *getCCSTrigger ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Set the SSID in the CCSTrigger object, create it if the object is not created
   */
  CCSTrigger *setCCSTrigger ( keyType index, std::string SSID ) 
    throw (FecExceptionHandler) ;
#endif

  /** \brief Retreive the FEC trigger control register 0
   */
  uint32_t getCCSTriggerConfig0 ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief set the trigger register 0 of the FEC/CCS
   */
  void setCCSTriggerConfig0 ( keyType index, uint32_t value ) throw (FecExceptionHandler) ;

  /** \brief Retreive the FEC trigger status register 0
   */
  uint32_t getCCSTriggerStatus0 ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief Retreive the FEC SSID
   */
  std::string getSSID ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief return the temperature of the given VME FEC
   */
  void getFecTemperature ( keyType index, int &tempSensor1, int &tempSensor2 ) 
    throw (FecExceptionHandler) ;

  /** \brief scan the PCI/VME bus for FECs
   */
  std::list<keyType> *scanFecRing (tscType8 fecSlotMin,
				   tscType8 fecSlotMax,
				   tscType8 ringMin,
				   tscType8 ringMax ) 
    throw (FecExceptionHandler) ;

  /** \brief scan the VME bus for FECs for a given FEC
   */
  keyType scanFecRing ( std::string fecHardwareId ) throw (FecExceptionHandler) ;

  /**
   * \brief return firmware version
   */
  tscType16 getFecFirmwareVersion( keyType index ) throw ( FecExceptionHandler ) ;

  /** \brief return the FEC hardware ID for a give index
   */
  std::string getFecHardwareId ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief return the FEC Trigger version
   */
  tscType32 getTriggerVersion ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief return the FEC VME version
   */
  tscType32 getVmeVersion ( keyType index ) throw (FecExceptionHandler) ;

  /** \brief return the index for a given FEC hardware ID
   */
  keyType getFecIndex ( std::string fecHardwareId ) throw (FecExceptionHandler) ;

  /** \brief set the input of the FEC
   */
  void setFecInputB (  keyType index, bool inputB ) throw (FecExceptionHandler) ;

  /** \brief set the output of the FEC
   */
  void setFecOutputB (  keyType index, bool outputB ) throw (FecExceptionHandler) ;

  /** \brief invert the clock polarity
   */
  void setInvertClockPolarity ( bool invertPolarity ) throw (FecExceptionHandler) ;

  /** \brief invert the clock polarity
   */
  void setInvertClockPolarity ( keyType index, bool invertPolarity ) throw (FecExceptionHandler) ;

  /** \brief use the internal clock
   */
  void setInternalClock (  keyType index, bool internal ) throw (FecExceptionHandler) ;

  /** \brief Set the FEC, ring control register 0
   */
  void setFecRingCR0 ( keyType index, tscType16 value, bool force = false ) 
    throw (FecExceptionHandler) ;

  /** \brief Set the FEC, ring control register 0
   */
  void setFecRingCR0 ( keyType index, tscType16 value, logicalOperationType op ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the FEC, ring control register 0
   */
  tscType16 getFecRingCR0 ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Set the FEC, ring control register 1
   */
  void setFecRingCR1 ( keyType index, tscType16 value ) 
    throw (FecExceptionHandler) ;

  /** \brief Enable or disable the irq for the given FEC
   */
  void setIRQ ( keyType index, bool enable, tscType8 vmeLevel=1 ) 
    throw (FecExceptionHandler) ;

  /** \brief Enable or disable the irq for the all the FECs
   */
  void setIRQ ( bool enable, tscType8 vmeLevel=1 ) 
    throw (FecExceptionHandler) ;

  /*----------- This method has no sense, the CR1 CANNOT BE READ ----------- 
  void setFecRingCR1 ( keyType index, 
		       tscType16 value 
		       logicalOperationType op )  ;
  throw (FecExceptionHandler) ;
  ----------------------------------------------------------------------- */

  /** \brief Get the FEC, ring control register 1
   */
  tscType16 getFecRingCR1 ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the FEC, ring status register 0
   */
  tscType16 getFecRingSR0 ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the FEC, ring status register 1
   */
  tscType16 getFecRingSR1 ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Hard reset for all FEC, ring
   */
  void fecHardReset ( ) 
    throw (FecExceptionHandler) ;

  /** \brief Hard reset for all FEC, ring
   */
  void fecHardReset ( keyType index, tscType8 value = 0 ) 
    throw (FecExceptionHandler) ;

  /** \brief only for VME crate, reset the crate
   */
  void crateReset ( ) 
    throw (FecExceptionHandler) ;

  /** \brief reset all FEC, ring FSM
   */
  void fecRingResetFSM ( ) 
    throw (FecExceptionHandler) ;

  /** \brief FEC, ring reset FSM
   */
  void fecRingResetFSM ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief reset all FEC, ring
   */
  void fecRingReset ( ) 
    throw (FecExceptionHandler) ;

  /** \brief FEC, ring reset
   */
  void fecRingReset ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Initialise the TTCRx
   */
  void initTTCRx ( ) 
    throw (FecExceptionHandler) ;

  /** \brief Initialise the TTCRx
   */
  void initTTCRx ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Disable or enable the receive for a FEC (redundancy)
   */
  void setCR0Receive ( keyType index, bool enable ) 
    throw (FecExceptionHandler) ;

  /** \brief FEC, ring reset on ring B 
   */
  void fecRingResetB ( ) 
    throw (FecExceptionHandler) ;

  /** \brief FEC, ring reset on ring B 
   */
  void fecRingResetB ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief release the FEC so re-generate a token on the ring
   */
  void fecRingRelease ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Reconfiguration of the ring
   */
  tscType16 fecRingReconfigure ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Reconfiguration of the ring for a given set of data
   */
  tscType16 fecRingReconfigure ( keyType index, TkRingDescription &tkRing ) 
    throw (FecExceptionHandler) ;

  /** \brief retreive all status and control registers for a given FEC, ring
   */
  FecRingRegisters getFecRingRegisters ( keyType index )
    throw (FecExceptionHandler) ;

  // ----------------------------------------------- CCU methods

  /** \brief check if a channel is enabled
   */
  bool isChannelEnabled ( keyType index )  throw (FecExceptionHandler) ;

  /** \brief Set the CCU register A
   */
  void setCcuCRA ( keyType index, tscType8 value ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU control register A
   */
  tscType8 getCcuCRA ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Set the CCU register B
   */
  void setCcuCRB ( keyType index, tscType8 value ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU control register B
   */
  tscType8 getCcuCRB ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Set the CCU register C
   */
  void setCcuCRC ( keyType index, tscType8 value ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU control register C
   */
  tscType8 getCcuCRC ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Set the CCU register D
   */
  void setCcuCRD ( keyType index, tscType8 value ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU control register D
   */
  tscType8 getCcuCRD ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Set the CCU register E
   */
  void setCcuCRE ( keyType index, tscType24 value ) 
    throw (FecExceptionHandler) ;

  /** \brief Set the CCU register E
   */
  void setCcuCRE ( keyType index, 
                   tscType8 valueI2C, 
                   tscType8 valuePIA,
                   tscType8 valueMemory,
                   tscType8 valueTrigger,
                   tscType8 valueJTAG) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU control register E
   */
  tscType24 getCcuCRE ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU status register A
   */
  tscType8 getCcuSRA ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU status register B
   */
  tscType8 getCcuSRB ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU status register C
   */
  tscType8 getCcuSRC ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU status register D
   */
  tscType8 getCcuSRD ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU status register E
   */
  tscType24 getCcuSRE ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU status register F
   */
  tscType16 getCcuSRF ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU status register G
   */
  tscType8 getCcuSRG ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Get the CCU status register H
   */
  tscType8 getCcuSRH ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief retreive the list of the CCU on that FEC, ring
   */
  std::list<keyType> *getCcuList ( keyType index, bool noBroadcast = false, bool scan = true) 
    throw (FecExceptionHandler) ;

  // ------------------------------- CCU Alarms

  /** \brief Enable or disable CCU alarms
   */
  void setCcuAlarmsEnable ( keyType index, bool enable1, bool enable2, bool enable3, bool enable4 ) 
    throw (FecExceptionHandler) ;

  /** \brief Wait for any CCU alarms
   */ 
  void waitForAnyCcuAlarms ( keyType index, tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ) 
    throw (FecExceptionHandler) ;

  /** \brief Enable the PIA interrupts
   */
  void setPiaInterruptEnable ( keyType index ) 
    throw (FecExceptionHandler) ;

  /** \brief Clear the interrupt on PIA channels
   */
  void setPiaClearInterrupts ( keyType index ) 
    throw (FecExceptionHandler) ;

  // ----------------------------------------------- Channel method

  /** \brief re-enable a channel
   */
  void setChannelEnable ( keyType index, bool enable ) 
    throw (FecExceptionHandler) ;

  // --------------------------------------------- PIA channels

  /** \brief Add a PIA access with the all the values (fec, ring, ccu, channel, address)
   */
  keyType addPiaAccess ( tscType8 fecSlot,
                         tscType8 ringSlot,
                         tscType8 ccuAddress,
                         tscType8 piaChannel,
                         enumAccessModeType accessMask ) 
    throw (FecExceptionHandler) ;

  /** \brief Add a PIA access with the key
   */
  keyType addPiaAccess ( keyType index,
                         enumAccessModeType accessMask ) 
    throw (FecExceptionHandler) ;

  /** \brief Remove a pia access
   */
  void removePiaAccess ( keyType index) throw (FecExceptionHandler) ;

  /** \brief Reset PIA channel
   */
  void piaChannelReset ( keyType index ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Set the PIA general control register
   */
  void setPiaChannelGCR ( keyType index, tscType8 value ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Set the PIA general control register
   */
  void setPiaChannelGCR ( keyType index, tscType8 value, logicalOperationType op ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Get the PIA general control register
   */
  tscType8 getPiaChannelGCR ( keyType index ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Get the PIA status register
   */
  tscType8 getPiaChannelStatus ( keyType index ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Set the PIA data direction register
   */
  void setPiaChannelDDR ( keyType index, tscType8 value ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Set the PIA data direction register
   */
  void setPiaChannelDDR ( keyType index, tscType8 value, logicalOperationType op ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Get the PIA data direction register
   */
  tscType8 getPiaChannelDDR ( keyType index ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Set the PIA data register
   */
  void setPiaChannelDataReg ( keyType index, tscType8 value ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Set the PIA data register
   */
  void setPiaChannelDataReg ( keyType index, tscType8 value, logicalOperationType op ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Get the PIA data register
   */
  tscType8 getPiaChannelDataReg ( keyType index ) 
    throw ( FecExceptionHandler ) ;

  // --------------------------------------------- I2C channels

  /** \brief Add an I2C access with all the values
   */
  keyType addi2cAccess ( tscType8 fecSlot,
                         tscType8 ringSlot,
                         tscType8 ccuAddress,
                         tscType8 i2cChannel,
                         tscType8 i2cAddress,
                         enumDeviceType deviceType,
                         enumAccessModeType accessMask ) 
    throw ( FecExceptionHandler ) ;

  /** \brief Add an I2C access with a key
   */
  keyType addi2cAccess ( keyType index,
                         enumDeviceType deviceType,
                         enumAccessModeType accessMask ) 
    throw (FecExceptionHandler) ;

  /** \brief remove an I2C access
   */
  void removei2cAccess ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Set the I2C control register A
   */
  void seti2cChannelCRA ( keyType index, tscType8 value)  
    throw (FecExceptionHandler) ;

  /** \brief Reset an I2C channel
   */
  void i2cChannelReset  ( keyType index )  
    throw (FecExceptionHandler) ;
  
  /** \brief Get the I2C control register A
   */
  tscType8 geti2cChannelCRA ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Get the I2C status register A
   */
  tscType8 geti2cChannelSRA ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Get the I2C status register B
   */
  tscType8 geti2cChannelSRB ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Get the I2C status register C
   */
  tscType8 geti2cChannelSRC ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Get the I2C status register D
   */
  tscType8 geti2cChannelSRD ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief scan the ring for devices
   */
  std::list<keyType> *scanRingForI2CDevice ( keyType  *deviceValues, 
                                        tscType32 sizeDevices,
					bool noBroadcast = false,
					bool display = true) 
    throw (FecExceptionHandler) ;

  /** \brief scan a ring for devices
   */
  std::list<keyType> *scanRingForI2CDevice ( keyType   index,
				        keyType  *deviceValues, 
                                        tscType32 sizeDevices,
					bool noBroadcast = false,
					bool display = true) 
    throw (FecExceptionHandler) ;


  // --------------------------------------------- Memory channels

  /** \brief Add a memory access with the all the values (fec, ring, ccu, channel, address)
   */
  keyType addMemoryAccess ( tscType8 fecSlot,
                            tscType8 ringSlot,
                            tscType8 ccuAddress,
                            tscType8 piaChannel,
                            enumAccessModeType accessMask )  
    throw (FecExceptionHandler) ;



  /** \brief Add a Memory access with the key
   */
  keyType addMemoryAccess ( keyType index,
                         enumAccessModeType accessMask ) 
    throw (FecExceptionHandler) ;

  /** \brief Remove a memory access
   */
  void removeMemoryAccess ( keyType index) 
    throw (FecExceptionHandler) ;

  /** \brief Reset Memory channel
   */
  void memoryChannelReset ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Set the memory control register A
   */
  void setMemoryChannelCRA ( keyType index, tscType8 value )  
    throw (FecExceptionHandler) ;;

  /** \brief Set the PIA control register A
   */
  void setMemoryChannelCRA ( keyType index, tscType8 value, logicalOperationType op )  
    throw (FecExceptionHandler) ;

  /** \brief Get the Memory control register A
   */
  tscType8 getMemoryChannelCRA ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Get the memory status register
   */
  tscType8 getMemoryChannelStatus ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to set the memory WIN1L register
   */
  void setMemoryChannelWin1LReg ( keyType index, tscType16 value )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to set the memory WIN1H register
   */
  void setMemoryChannelWin1HReg ( keyType index, tscType16 value )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to set the memory WIN2L register
   */
  void setMemoryChannelWin2LReg ( keyType index, tscType16 value )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to set the memory WIN2H register
   */
  void setMemoryChannelWin2HReg ( keyType index, tscType16 value )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to set the memory mask register
   */
  void setMemoryChannelMaskReg ( keyType index, tscType8 value )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to retreive memory mask register
   */
  tscType8 getMemoryChannelMaskReg ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to retreive memory WIN1L register
   */
  tscType16 getMemoryChannelWin1LReg ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to retreive memory WIN1H register
   */
  tscType16 getMemoryChannelWin1HReg ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to retreive memory WIN2L register
   */
  tscType16 getMemoryChannelWin2LReg ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to retreive memory WIN2H register
   */
  tscType16 getMemoryChannelWin2HReg ( keyType index )  
    throw (FecExceptionHandler) ;

  /** \brief Method in order to retreive memory mask register
   */
  tscType8 getMemoryChanneMaskReg ( keyType index )  
    throw (FecExceptionHandler) ;

  // ----------------------------------------------- Read / Write methods

  /** \brief Download a block of frames
   */
  unsigned int setBlockDevices ( keyType index, accessDeviceTypeList &vAccesses, std::list<FecExceptionHandler *> &errorList, bool piaChannel = false, bool debugMessageDisplay = false ) 
    throw (FecExceptionHandler) ;

  /** \brief Download a block of frames to all rings (ring per ring)
   */
  unsigned int setBlockDevices ( accessDeviceTypeListMap &hAccesses, std::list<FecExceptionHandler *> &errorList, bool piaChannel = false, bool debugMessageDisplay = false ) ;

  /** \brief Download a block of frames in parallel on all rings 
   */
  unsigned int setBlockDevicesParallel ( accessDeviceTypeListMap &hAccesses, std::list<FecExceptionHandler *> &errorList, bool piaChannel = false, bool debugMessageDisplay = false ) ;

  /** \brief Read a value from the device specified in the key
   */
  tscType8 read (keyType index)  
    throw (FecExceptionHandler) ;

  /** \brief Read a value from the device specified in the key
   */
  tscType8 readOffset (keyType index, tscType8 offset)  
    throw (FecExceptionHandler) ;

  /** \brief Read a value from the device specified in the key
   */
  tscType8 readOffset (keyType index, tscType8 offset, tscType8 decal)  
    throw (FecExceptionHandler) ;

  /** \brief Read a value from the device specified in the key
   */
  tscType8 read (keyType index, tscType8 offset)  
    throw (FecExceptionHandler) ;

  /** \brief Read a value from the memory channel
   */
  tscType8 read (keyType index, tscType8 AH, tscType8 AL )  
    throw (FecExceptionHandler) ;

  /** \brief Read a value from the memory channel
   */
  void read (keyType index, 
             tscType8 AH, tscType8 AL,
             unsigned size, tscType8 *values)  
    throw (FecExceptionHandler) ;

  /** \brief Write a value into the device specified in the key
   */
  void write (keyType index, tscType8 value)  
    throw (FecExceptionHandler) ;

  /** \brief Write a value into the device specified in the key
   */
  void writeOffset (keyType index, tscType8 offset, tscType8 value)  
    throw (FecExceptionHandler) ;

  /** \brief Write a value into the device specified in the key
   */
  void writeOffset (keyType index, tscType8 offset, tscType8 decal, tscType8 value)  
    throw (FecExceptionHandler) ;

  /** \brief Write a value into the device specified in the key
   */
  void write (keyType index, tscType8 offset, tscType8 value)  
    throw (FecExceptionHandler) ;

  /** \brief Write a value into the memory channel
   */
  void write (keyType index, 
              tscType8 AH, tscType8 AL, 
              tscType8 value)  
    throw (FecExceptionHandler) ;

  /** \brief Write a value into the memory channel
   */
  void write (keyType index, 
              tscType8 AH, tscType8 AL, 
              tscType8 *values, unsigned size)  
    throw (FecExceptionHandler) ;

  /** \brief Read modify write a value from the memory specified by the key in single byte
   */
  void write ( keyType index, 
               tscType8 AH, tscType8 AL, 
               logicalOperationType op, 
               tscType8 mask )  
    throw (FecExceptionHandler) ;

  /* ---------------------------------------------------------------------------------- */
  /* ---------------------------------------------------------------------------------- */
  /* ---------------------------------------------------------------------------------- */

  /** \brief This method sort a FEC/Ring list by the order of the front panel (from 8 to 1)
   * \param e1 - one element (key index)
   * \param e2 - another element (key index)
   */
  static bool sortByFrontPanelNumber ( keyType e1, keyType e2 ) {
    if (getFecKey(e1) > getFecKey(e2)) return false ;
    else if (getFecKey(e1) < getFecKey(e2)) return true ;
    else { // ==
      if (getRingKey(e1) < getRingKey(e2)) return false ;
      else return true ;
    }
  }
 
  /** \brief This detect the configuration for the VME or in plug and play if the file config/PlugNPlayConfigure.dat is detected or in geographical address otherwise
   */
  static bool getVmeFileName ( std::string &vmeFileName, std::string &vmeFileNamePnP ) ;

  /** \brief create a FEC access with parameter
   */  
  static FecAccess *createFecAccess ( int argc, char **argv, int *cnt, bool init = true ) throw (FecExceptionHandler) ;

};
#endif
