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
  
  Copyright 2003 - 2004, Frederic DROUHIN, Laurent Gross - Universite de Haute-Alsace, Mulhouse-France, Institut de Recherche Subatomique de Strasbourg
*/
#ifndef FECVMERINGDEVICE_H
#define FECVMERINGDEVICE_H

// System includes
#include <pthread.h>

// FecSoftware includes
#include "tscTypes.h"
#include "FecExceptionHandler.h"
#include "FecRingDevice.h"

//HAL includes
#if defined(BUSVMESBS)
#include "hal/SBS620x86LinuxBusAdapter.hh"
#endif
#if defined(BUSVMECAENUSB) || defined(BUSVMECAENPCI)
#include "hal/CAENLinuxBusAdapter.hh"
#endif

#include "hal/VME64xDummyBusAdapter.hh"
#include "hal/VME64xDevice.hh"
#include "hal/VME64xCrate.hh"
#include "hal/ASCIIFileModuleMapper.hh"
#include "hal/ASCIIFileAddressTableContainer.hh"
#include "hal/AddressTableContainerInterface.hh"
#include "hal/VMEAddressTableASCIIReader.hh"

#include "hashMapVMEDevice.h"

// Block mode used
enum fecRingDeviceFifoAccessModeEnum { FECNOBLT, FECDOBLT, FECDOMWORD };

#define VME_MAX_FRAME_LENGTH	40

/** To access directly the correct item, see FECHALITEM and FECHALCHARITEM
 */
enum fecHalItemEnum {FECHALCONTROL0, FECHALCONTROL1, FECHALSTATUS0, FECHALSTATUS1, FECHALVERSION_SRC, 
		     FECHALTRA_FIFO_R, FECHALRET_FIFO_R, FECHALREC_FIFO_R, FECHALTRA_FIFO_W, 
		     FECHALRET_FIFO_W, FECHALREC_FIFO_W,FECHALTRA_FIFO_R_BL, FECHALRET_FIFO_R_BL, 
		     FECHALREC_FIFO_R_BL,FECHALTRA_FIFO_W_BL, FECHALRET_FIFO_W_BL, FECHALREC_FIFO_W_BL};
 

#define NUMBERASCIIDIFF 48

/** This class gives all the methods to read / write a word in the different registers of the VME FEC ie.:
 * FIFO receive, FIFO transmit, FIFO return, control register 0 and 1, status register 0 and 1
 * This class inherits from FecRingDevice in order to have the same methods for the type of FECs: VME, PCI, USB
 * This class inherits from FecRingDevice which contains all the high level methods for CCU, channels (i2c, pia, memory) 
 * The first mezzanine must be always plugged at the first place from the top of the FEC. This is mandatory for the load of the firmware
 */
class FecVmeRingDevice: public FecRingDevice {

 private:
  /** For HAL access
   */
  static Sgi::hash_map<uint32_t, HAL::VMEBusAdapterInterface *> busAdapter_ ; 
  

  /** For HAL access
   */
  static HAL::ASCIIFileModuleMapper *moduleMapper_ ; 

  /** For HAL access
    */
  static HAL::ASCIIFileAddressTableContainer * addressTableContainer_ ; 

  /** For HAL access
   */
  static HAL::VMEAddressTableASCIIReader * addressTableReader_ ; 

  /** For HAL access
   */
  static HAL::VMEAddressTable * addressTable_ ; 
  
  /** VME board on which we will work, initialized in/by constructor  
   */
  HAL::VME64xDevice *currentVmeBoard_;

  /** FEC hardware ID
   */
  std::string fecHardwareId_ ;

  /** Hal item for the given ring
   */
  char fecHalItem_[17][32] ;

  /** Number of ring per VME FEC: min value (0 or 1)
   */
  static uint32_t MINVMEFECRING ;

  /** Number of ring per VME FEC: max value (7 or 8)
   */
  static uint32_t MAXVMEFECRING ;

  /** Is the plug and play was used
   */
  static int plugnplayUsed_ ;

 public:

  /** name of the bus adapter
   */
  static const std::string STR_BUSADAPTER_NAME[3] ;

  /** Base addresses of VME FEC
   */
  static const uint32_t VMEFECBASEADDRESSES[MAX_NUMBER_OF_SLOTS] ;

  /** item string for the ring 0, the value _0 must be change with the ring number
   */
  static const char FECHALITEM[][32] ;

  /** Which charater must be changed in the string
   */
  static const int FECHALCHARITEM[] ;

  /** Number of slot in the VME bus: min value
   */
  static const uint32_t MINVMEFECSLOT = 1 ;

  /** Number of slot in the VME bus: max value
   */
  static const uint32_t MAXVMEFECSLOT = 21 ;
  
  /** yes or no to use block mode to transfer fifo data
   */ 
  fecRingDeviceFifoAccessModeEnum blockMode_ ; 

  /** Is the mFEC was correctly initialised ?
   */
  int mFecInitialised_ ;

  /******************************************************
			CONSTRUCTOR - DESTRUCTOR
  ******************************************************/

  /** \brief create a VME FEC based on the VME device provided by HAL
   */
  FecVmeRingDevice ( HAL::VME64xDevice *vmedevice, tscType8 fecSlot, tscType8 ringSlot, bool init = true, fecRingDeviceFifoAccessModeEnum blockMode = FECNOBLT, uint32_t adapterSlot = 0, bool invertClockPolarity = false ) throw ( FecExceptionHandler ) ;

  /** Destructor
   */
  virtual ~FecVmeRingDevice( ) throw ( FecExceptionHandler );

  /** \return the FEC hardware id
   */
  std::string getFecHardwareId ( ) ;

  /** \brief set block transfer mode 
   * \param blockMode - block mode (FECNOBLT, FECDOBLT)
   */
  inline void setFifoAccessMode(fecRingDeviceFifoAccessModeEnum blockMode) { blockMode_ = blockMode; }; 

  /** \brief get block transfer mode 
   * \return block mode (FECNOBLT, FECDOBLT)
   */
  inline fecRingDeviceFifoAccessModeEnum getFifoAccessMode() { return blockMode_ ; }; 

  /* \brief set the block mode for the VME FEC
   * param blocMode - true to use the block, false to not use it
   */
  inline void setFifoAccessMode(bool blockMode) { 
    if (blockMode) setFifoAccessMode ( FECDOBLT ) ;
    else setFifoAccessMode ( FECNOBLT) ;
  }

  /** \brief return the status of initialisation
   * \return status of initialisation:
   * <ul>
   * <li> -1: hardware creation not done
   * <li> 0: hardware creation done but mFEC not initialised
   * <li> 1: hardware creation done and mFEC initialised
   * </ul>
   */
  inline int getmFecInitialised ( ) { return mFecInitialised_ ; } 
  
  /******************************************************
			BOARD RESET & CONFIGURATION FUNCTIONS
  ******************************************************/
  static unsigned long getSr0Counter();
  static unsigned long getTraFifoCounter();
  static unsigned long getTraFifoSent();
  /** \brief create a bus adapter depending of which was chosen
   */
  static HAL::VMEBusAdapterInterface *createBusAdapter ( uint32_t adapterSlot, std::string strBusAdapter ) throw ( HAL::BusAdapterException, FecExceptionHandler ) ;  

  //PnP initialisation function
  static HAL::VME64xCrate * configurePlugAndPlay( uint32_t adapteurSlot, std::string mapfile, std::string maptable, std::string strBusAdapter, hashMapVMEDevice &hashMapVMEDevice )
    throw (HAL::BusAdapterException,
	   HAL::NoSuchFileException,
	   HAL::IllegalValueException,
	   HAL::XMLProcessingException,
	   HAL::HardwareProblemException,
	   HAL::UnsupportedException);
  
  //Hardcoded base address initialisation function
  static HAL::VME64xDevice * configureHardBaseAddress( uint32_t adapteurSlot, std::string addresstablefile, uint32_t baseaddress, uint32_t fecSlot, std::string strBusAdapter )
    throw (HAL::BusAdapterException,
	   HAL::NoSuchFileException,
	   HAL::IllegalValueException,
	   HAL::UnsupportedException, 
	   FecExceptionHandler);

  //Check if we really have a VME board on the allocated base address
  static tscType16 detectVmeBoard(HAL::VME64xDevice *vmedevice, tscType8 fecSlot) throw ( FecExceptionHandler );

  /** \brief detect which numbering schema for the FEC rings
   */
  static void autoNumberingSchema ( HAL::VMEAddressTable addressTable ) ;

  /** \brief detect which numbering schema for the FEC rings
   */
  static void autoNumberingSchema ( std::string addresstablefile ) ;

  /** \brief detect which numbering schema for the FEC rings
   */
  static void autoNumberingSchema ( const HAL::AddressTableInterface& addressTableInterface ) ;

  /* \brief Clean up allocated memory on request
   */
  static void closeFecVmeAccess ( ) throw (HAL::BusAdapterException) ;
  
  /* \brief Resets the whole VME crate 
   */
  static void crateReset ( uint32_t adapterSlot = 0 ) throw ( FecExceptionHandler ) ;
  
  /******************************************************
		CONTROL & STATUS RTEGISTERS ACCESS
  ******************************************************/
  
  /** \brief Set the control register 0  the FEC
   */ 
  void setFecRingCR0 ( tscType16 ctrl0Value, bool force = false ) throw ( FecExceptionHandler );
  
  /** \brief Get the control register 0  the FEC
   * \return value read
   */
  tscType16 getFecRingCR0( ) throw ( FecExceptionHandler );
  
  /** \brief Set the control register 1  the FEC
   * \param ctrl1Value - value to be set
   */ 
  void setFecRingCR1( tscType16 ctrl1Value ) throw ( FecExceptionHandler );
  
  /** \brief Get the control register 1  the FEC
   * \return value read
   */
  tscType16 getFecRingCR1( ) throw ( FecExceptionHandler );
  
  /** \brief Get the status register 0  the FEC
   * \return value read
   */
  tscType32 getFecRingSR0(unsigned long sleeptime=0) throw ( FecExceptionHandler );
  
    /** \brief Get the status register 1  the FEC
   * \return value read
   */
  tscType16 getFecRingSR1( ) throw ( FecExceptionHandler );

    /** \brief return the firmware version of the FEC
   */
  tscType16 getFecFirmwareVersion( ) throw ( FecExceptionHandler ) ;

  /******************************************************
		FIFO ACCESS - NATIVE 32 BITS FORMAT
		NATIVE FORMAT ACCESS ALLOWS R/W OPERATIONS
  ******************************************************/
  
  /** \brief return a word from the FIFO receive
   */
  tscType32 getFifoReceive( ) throw ( FecExceptionHandler );
  
  /** \brief write a word in the FIFO receive
   * \param value - value to be written
   */
  void setFifoReceive( tscType32 fiforecValue ) throw ( FecExceptionHandler );

  /** \brief return a word from the FIFO return
   */
  tscType8 getFifoReturn( ) throw ( FecExceptionHandler );
  
  /** \brief write a word in the FIFO return
   * \param value - value to be written
   */
  void setFifoReturn( tscType8 fiforetValue ) throw ( FecExceptionHandler );
  
  /** \brief return a word from the FIFO transmit
   */
  tscType32 getFifoTransmit( ) throw ( FecExceptionHandler );
  
  /** \brief write a word in the FIFO transmit
   * \param value - value to be written
   */
  void setFifoTransmit( tscType32 fifotraValue ) throw ( FecExceptionHandler );  
  
  /******************************************************
		HARD RESET
  ******************************************************/	

  /** \brief issue a crate reset
   */
  void fecHardReset ( ) throw ( FecExceptionHandler ) ;

  /******************************************************
   * The next methods is able to be overloaded in the sub-classes in order to implement
   * the block mode transfer.
   * \author W. Bialas
   ******************************************************/	

   /** \brief get block of words from the FIFO transmit
    */
   tscType32* getFifoTransmit (  tscType32 *value, int count ) throw (FecExceptionHandler) ;

   /** \brief get block of words from the FIFO receive
    */
   tscType32* getFifoReceive (  tscType32 *value, int count ) throw (FecExceptionHandler) ;

   /** \brief set a word into the FIFO Transmit
    */
   void setFifoTransmit ( tscType32 *value, int count ) throw (FecExceptionHandler) ;

   /** \brief set a word into the FIFO Transmit
    */
   void setFifoReceive ( tscType32 *value, int count ) throw (FecExceptionHandler) ;

  /******************************************************
		IRQ enable / disable
  ******************************************************/

  /** \brief Enable or disable the VME IRQ with the level
   */
  void setIRQ ( bool enable, tscType8 level=1 ) throw ( FecExceptionHandler ) ;
  
  /******************************************************
           VME64x CSR/CR space operations
  ******************************************************/
  
  /**  \brief return serial number 
   */
  std::string getSerialNumber();

  /** \brief return the trigger FPGA firmware version
   */
  tscType32 getTriggerVersion() throw ( FecExceptionHandler ) ;

  /**  \brief return VMEslave version number
   */
  tscType32 getVmeVersion() throw ( FecExceptionHandler );
  /**  \brief return VMEslave version number
   */
  /**  \brief Reload all the uFECs on the board. Sleep about 1 sec after that.
   */
  void uFecsReload(tscType8 value = 0x1) throw ( FecExceptionHandler );


  /******************************************************
                 STATIC methods
  ***************************************************/

  /** \brief return the hardware id related to the slot
   */
  static std::string getSerialNumber ( uint32_t slot, uint32_t adapterSlot = 0 ) throw ( FecExceptionHandler ) ;
  
  /** \brief return the min value of the ring 
   */
  static uint32_t getMinVmeFecRingValue ( ) {

    return (MINVMEFECRING) ;
  }

  /** \brief return the min value of the ring 
   */
  static uint32_t getMaxVmeFecRingValue ( ) {

    return (MAXVMEFECRING) ;
  }

  /** \brief is the plug'm play used
   */
  static bool isPlugnPlayUsed ( ) {

    return (plugnplayUsed_) ;
  }

  /** Return the min ring value
   */
  static uint32_t getMinRingValue ( ) {
    return FecVmeRingDevice::MINVMEFECRING ;
  }
  
  /** Return the max ring value
   */
  static uint32_t getMaxRingValue ( ) {
    return FecVmeRingDevice::MAXVMEFECRING ;
  }
} ;

#endif
