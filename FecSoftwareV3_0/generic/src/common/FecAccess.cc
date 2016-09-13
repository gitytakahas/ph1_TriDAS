/*
  This file is part of FEC Software project.
  
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

// S'occuper des catch et de la politique de renvoi des exceptions
// Check if the corresponding devices is a FEC
// Reflechir si dans ~FecAccess, la creation du HardwareDevice n'est pas dynamique
// Checker la methode addMemoryAccess par rapport a une methode addI2CAccess

#include <iostream>

#include "deviceFrame.h"

#include "FecExceptionHandler.h"

#include "piaAccess.h"
#include "i2cAccess.h"
#include "memoryAccess.h"

#define EVGUENI_INVERTCLOCKPOLARITY

// For hardware access
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
#include "FecVmeRingDevice.h"
#endif
#if defined(BUSPCIFEC)
#include "FecPciRingDevice.h" 
#endif
#if defined(BUSUSBFEC)
#include "FecUsbRingDevice.h"
#endif

#include "FecAccess.h"

// -------------------------------------------------------------------------------------
//
//                              Constructors and destructor
//
// -------------------------------------------------------------------------------------

/** Initialise the force ack to not use it or not and ask for each
 * FecRingDevice opened to initialise it or not. 
 * \param forceAck - to enable (or disable) the force acknowledge bit for the I2C channels
 * \param initFEC - to intialise the FEC device when it is opened
 * \param scanFECs - scan all FEC,ring available on the bus
 * \param scanCCUs - scan all the CCUs
 * \param i2cSpeed - i2c speed
 * \exception FecExceptionHandler
 * not the same version than the API (see scanFecRing)
 * \warning Only for PCI FEC
 */
FecAccess::FecAccess ( bool forceAck, bool initFec, 
                       bool scanFECs, bool scanCCUs,
		       tscType16 i2cSpeed, bool invertClockPolarity )
  throw (FecExceptionHandler ) {

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  vme64xCrate_ = NULL ;
#endif

#if !defined(BUSPCIFEC)
  RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR,
			    "Support for PCI is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  // PCI bus
  fecBusType_ = FECPCI ;

  // Initialise the object
  setInitFecAccess ( forceAck, initFec, scanFECs, scanCCUs,
		     i2cSpeed, 
		     FecPciRingDevice::MINPCIFECSLOT, FecPciRingDevice::MAXPCIFECSLOT, 
		     FecPciRingDevice::MINPCIFECRING, FecPciRingDevice::MAXPCIFECRING, invertClockPolarity ) ;
#endif
}

/** This method creates the VME plug'n play and retreive all the HardwareDevice
 * needed.
 * <p>See also the method configurePlugAndPlay()
 * \param adapterSlot - adapter slot (equivalent in HAL of the crate number)
 * \param configurationFile - configuration for the FEC, Ring
 * \param mapFile - slot configuration (slot - config file)
 * \param forceAck - to enable (or disable) the force acknowledge bit for the I2C channels
 * \param initFEC - to intialise the FEC device when it is opened
 * \param scanFECs - scan all FEC,ring available on the bus
 * \param scanCCUs - scan all the CCUs
 * \param i2cSpeed - i2c speed
 * \param numberOfRing: FEC can have only few mFECs, each mFECs missing give a timeout on the 
 * FEC VME that can be very long in order to avoid this problem, numberOfRing (default 8) is specified to scan only the ring involved:
 * <ul>
 * <li> Numbering schema given in the dat file is going from 0 to 7, top to bottom, if the numberOfRing is 5 then the scan will go from ring 0, 1, 2, 3, 4
 * <li> Numbering schema given in the dat file is going from 8 to 1, top to bottom, if the numberOfRing is 5 then the scan will go from ring 4, 5, 6, 7, 8 in this order
 * </ul>
 * Please remember that the top slot should be always set and the numberOfRing is working only if you put the mFECs in contigus way.
 * \exception FecExceptionHandler
 * \warning Only for VME FEC
 */
FecAccess::FecAccess ( unsigned adapterSlot, 
		       std::string configurationFile, std::string mapFile,
                       bool forceAck, bool initFec, 
                       bool scanFECs, bool scanCCUs,
		       tscType16 i2cSpeed,
		       std::string strBusAdapter,
		       bool blockMode, tscType8 numberOfRing, bool invertClockPolarity )
  throw ( FecExceptionHandler ) {

  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "The plug and play is not supported by the FEC software since 16/03/2012",
			    FATALERRORCODE ) ;

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  vme64xCrate_ = NULL ;
#endif

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  // VME bus
  fecBusType_ = FECVME ;
  adapterSlot_ = adapterSlot ;

  // Block mode
  blockMode_ = blockMode ;

  // Plug'n play  
  configurePlugAndPlay ( adapterSlot, configurationFile, mapFile, strBusAdapter ) ;

  // Ring to be scanned
  tscType8 ringMin = FecVmeRingDevice::getMinVmeFecRingValue() ; // 0 or 1
  tscType8 ringMax = FecVmeRingDevice::getMaxVmeFecRingValue() ; // 7 or 8
  if (ringMin == 1) { // 8 to 1 top to bottom
    ringMin = (ringMax - numberOfRing + 1) ;
  }
  else if (ringMin == 0) { // 0 to 7 top to bottom
    ringMax = (numberOfRing - 1) ;
  }

  // Initialise the object
  setInitFecAccess ( forceAck, initFec, scanFECs, scanCCUs,
		     i2cSpeed, 
		     FecVmeRingDevice::MINVMEFECSLOT, FecVmeRingDevice::MAXVMEFECSLOT, 
		     ringMin, ringMax, invertClockPolarity ) ;
#endif
}

/** This method creates the VME plug'n play and retreive all the HardwareDevice
 * needed.
 * <p>See also the method configureBaseAddresses()
 * \param adapterSlot - adapter slot (equivalent in HAL of the crate number) 
 * \param mapFile - slot configuration (slot - config file)
 * \param configurationFile: configuration for the FEC, Ring
 * \param vmeBaseAddresses: this array contains for each slot the base address of the board. Note that if the base address is 0 then no board is given at this slot.
 * \param forceAck - to enable (or disable) the force acknowledge bit for the I2C channels
 * \param initFEC - to intialise the FEC device when it is opened
 * \param scanFECs - scan all FEC,ring available on the bus
 * \param scanCCUs - scan all the CCUs
 * \param i2cSpeed - i2c speed
 * \param numberOfRing: FEC can have only few mFECs, each mFECs missing give a timeout on the 
 * FEC VME that can be very long in order to avoid this problem, numberOfRing (default 8) is specified to scan only the ring involved:
 * <ul>
 * <li> Numbering schema given in the dat file is going from 0 to 7, top to bottom, if the numberOfRing is 5 then the scan will go from ring 0, 1, 2, 3, 4
 * <li> Numbering schema given in the dat file is going from 8 to 1, top to bottom, if the numberOfRing is 5 then the scan will go from ring 4, 5, 6, 7, 8 in this order
 * </ul>
 * Please remember that the top slot should be always set and the numberOfRing is working only if you put the mFECs in contigus way.
 * \warning Only for VME FEC
 */
FecAccess::FecAccess ( unsigned adapterSlot, std::string configurationFile, 
                       const uint32_t *vmeBaseAddresses,
		       bool forceAck, bool initFec, 
                       bool scanFECs, bool scanCCUs,
		       tscType16 i2cSpeed,
		       std::string strBusAdapter,
		       bool blockMode,tscType8 numberOfRing, bool invertClockPolarity )
  throw ( FecExceptionHandler ) {

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  vme64xCrate_ = NULL ;
#endif

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  // VME bus
  fecBusType_ = FECVME ;
  adapterSlot_ = adapterSlot ;

  // Block mode
  blockMode_ = blockMode ;

  // The base addresses of each slot is given in the table
  configureBaseAddresses ( adapterSlot, configurationFile, vmeBaseAddresses, strBusAdapter ) ;

  // Ring to be scanned
  tscType8 ringMin = FecVmeRingDevice::getMinVmeFecRingValue() ; // 0 or 1
  tscType8 ringMax = FecVmeRingDevice::getMaxVmeFecRingValue() ; // 7 or 8
  if (ringMin == 1) { // 8 to 1 top to bottom
    ringMin = (ringMax - numberOfRing + 1) ;
  }
  else if (ringMin == 0) { // 0 to 7 top to bottom
    ringMax = (numberOfRing - 1) ;
  }

  // Initialise the object
  setInitFecAccess ( forceAck, initFec, scanFECs, scanCCUs,
		     i2cSpeed, 
		     FecVmeRingDevice::MINVMEFECSLOT, FecVmeRingDevice::MAXVMEFECSLOT, 
		     ringMin, ringMax, invertClockPolarity ) ;
#endif
}

/** This method creates a FecAccess
 * needed.
 * <p>See also the method configurePlugAndPlay()
 * \param hashMapVMEDevice - hash_map of VMEDevice, this table copy the list into its own list but do not clone it so do not delete it before the destruction of FecAccess
 * \param forceAck - to enable (or disable) the force acknowledge bit for the I2C channels
 * \param initFEC - to intialise the FEC device when it is opened
 * \param scanFECs - scan all FEC,ring available on the bus
 * \param scanCCUs - scan all the CCUs
 * \param i2cSpeed - i2c speed
 * \param numberOfRing: FEC can have only few mFECs, each mFECs missing give a timeout on the 
 * FEC VME that can be very long in order to avoid this problem, numberOfRing (default 8) is specified to scan only the ring involved:
 * <ul>
 * <li> Numbering schema given in the dat file is going from 0 to 7, top to bottom, if the numberOfRing is 5 then the scan will go from ring 0, 1, 2, 3, 4
 * <li> Numbering schema given in the dat file is going from 8 to 1, top to bottom, if the numberOfRing is 5 then the scan will go from ring 4, 5, 6, 7, 8 in this order
 * </ul>
 * Please remember that the top slot should be always set and the numberOfRing is working only if you put the mFECs in contigus way.
 * \exception FecExceptionHandler
 * \warning Only for VME FEC
 */
FecAccess::FecAccess ( hashMapVMEDevice hashMapVMEDevice,
		       bool forceAck, bool initFec, 
                       bool scanFECs, bool scanCCUs,
		       tscType16 i2cSpeed,
		       bool blockMode, tscType8 numberOfRing, bool invertClockPolarity )
  throw ( FecExceptionHandler ) {

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  vme64xCrate_ = NULL ;
#endif

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  // VME bus
  fecBusType_ = FECVME ;

  // Block mode
  blockMode_ = blockMode ;

  // Copy the list of VMEDevice in its own table
  for (hashMapVMEDevice::iterator vme = hashMapVMEDevice.begin() ; vme != hashMapVMEDevice.end() ; vme ++) {
    hardwareDevices_[vme->first] = vme->second ;
    if (vme->second) FecVmeRingDevice::autoNumberingSchema(vme->second->getAddressTableInterface()); 
  }

  // Ring to be scanned
  tscType8 ringMin = FecVmeRingDevice::getMinVmeFecRingValue() ; // 0 or 1
  tscType8 ringMax = FecVmeRingDevice::getMaxVmeFecRingValue() ; // 7 or 8
  if (ringMin == 1) { // 8 to 1 top to bottom
    ringMin = (ringMax - numberOfRing + 1) ;
  }
  else if (ringMin == 0) { // 0 to 7 top to bottom
    ringMax = (numberOfRing - 1) ;
  }

  // Initialise the object
  setInitFecAccess ( forceAck, initFec, scanFECs, scanCCUs,
		     i2cSpeed, 
		     FecVmeRingDevice::MINVMEFECSLOT, FecVmeRingDevice::MAXVMEFECSLOT, 
		     ringMin, ringMax, invertClockPolarity ) ;
#endif
}

/** Initialise the force ack to not use it or not and ask for each
 * FecDevice opened to initialise it or not. 
 * \param fd - device descriptor. Only valid value: 0 
 * \param forceAck - to enable (or disable) the force acknowledge bit for the I2C channels
 * \param initFEC - to intialise the FEC device when it is opened
 * \param scanFECs - scan all FEC,ring available on the bus
 * \param scanCCUs - scan all the CCUs
 * \param i2cSpeed - i2c speed
 * \warning this method is not recommanded due to the fact that
 * several FecSupervisor can managed the same PCI/VME/USB bus
 * \exception FecExceptionHandler
 * not the same version than the API (see scanFecRing)
 * \warning Only for USB FEC
 */
FecAccess::FecAccess ( hashMapFecUsbSerial fecUsbSerials, bool forceAck, bool initFec, 
                       bool scanFECs, bool scanCCUs,
		       tscType16 i2cSpeed, bool invertClockPolarity )
  throw (FecExceptionHandler ) {

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  vme64xCrate_ = NULL ;
#endif

#if !defined(BUSUSBFEC)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "Support for USB VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  // USB bus
  fecBusType_ = FECUSB ;
  
  fecUsbSerialNumbers_ = fecUsbSerials ; 

  // Initialise the object
  setInitFecAccess ( forceAck, initFec, scanFECs, scanCCUs,
		     i2cSpeed, 
                     FecUsbRingDevice::minUsbFecSlot, FecUsbRingDevice::maxUsbFecSlot, 
                     FecUsbRingDevice::minUsbFecRing, FecUsbRingDevice::maxUsbFecRing, invertClockPolarity ) ;
#endif
}

/** The desctructor is used for:
 * <ul>
 * <li> Disable all the channels
 * <li> Close all the device drivers opened during all the access
 * <li> Remove all the objects from the hash table
 * </ul>
 * \warning the VME devices are cleared when exiting
 */
FecAccess::~FecAccess ( ) {

  // Delete all the elements from the map
  // Remove all the accesses on ccu channel access
  for (deviceMapAccessedType::iterator p=deviceEnable_.begin();p!= deviceEnable_.end();p++) {
    delete p->second ;
  }

  switch (fecBusType_) {
  case FECVME:
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
    deleteVmeAccesses ( ) ;
#endif
    break ;
  case FECPCI:
#if defined(BUSPCIFEC)
    // Remove all the accesses on the FEC device
    for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
      delete p->second ;
    }
#endif
    break ;
  case FECUSB: 
#if defined(BUSUSBFEC)
   // Remove all the accesses on the FEC device
    for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
      // all FECUSB destructing/release code is inside FecUsbRingDevice destructor code:  
      delete p->second ;
    }
#endif
    break ;
  }

  // No more channels are connected
  // channelEnable_.clear() ;  
}

/** Delete all the VME accesses
 */
void FecAccess::deleteVmeAccesses ( ) {

  if (fecBusType_ != FECVME) return ;

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  // Remove all the accesses on the FEC device
  for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
    delete p->second ;
  }

  // delete the plug and play VME adapter
  if (vme64xCrate_ != NULL) {
    //delete vme64xCrate_ ; 
  }
  else {
    // Delete the corresponding HardwareDevice
    for ( hashMapVMEDevice::iterator p = hardwareDevices_.begin() ; p != hardwareDevices_.end() ; p ++)
      delete p->second ;
  }
#endif

  // Close all access for example the SBS and so on => never use this method if you are threaded
  // FecVmeRingDevice::closeFecVmeAccess() ;
}

/** Initialisation of the FEC access object
 * \param forceAck - to enable (or disable) the force acknowledge bit for the I2C channels
 * \param initFEC - to intialise the FEC device when it is opened
 * \param scanFECs - scan all FEC,ring available on the bus
 * \param scanCCUs - scan all the CCUs
 * \param i2cSpeed - i2c speed
 * \param fecSlotMin - FEC slot minimum
 * \param fecSlotMax - FEC slot maximum
 * \param ringMin - FEC ring minimum
 * \param ringMax - FEC ring maximum
 * \param invertClockPolarity - invert the clock polarity
 * \warning this method is not recommanded due to the fact that
 * several FecSupervisor can managed the same PCI/VME/USB bus
 * \exception FecExceptionHandler
 * not the same version than the API (see scanFecRing)
 */
void FecAccess::setInitFecAccess ( bool forceAck, bool initFec, bool scanFECs, bool scanCCUs, tscType16 i2cSpeed, 
				   tscType8 fecSlotMin, tscType8 fecSlotMax, tscType8 ringMin, tscType8 ringMax,
				   bool invertClockPolarity )
  throw (FecExceptionHandler) {

  for (unsigned int i = 0 ; i < MAX_NUMBER_OF_SLOTS+1 ; i ++) reloadedFirmware_[i] = 0 ;

  // clear the hash_map
  deviceEnable_.clear() ;
  channelEnable_.clear() ;
  fecRingEnable_.clear() ;

  // Force acknowledge not used
  forceChannelAck_   = forceAck ;
  // Initalised all the FecRingDevice needed
  initFecRingDevice_ = initFec ;
  // i2c speed
  i2cSpeed_ = i2cSpeed  ;
  // Invert the clock polarity
  invertClockPolarity_ = invertClockPolarity ;

  if (scanFECs) {

    std::list<keyType> *fecList = scanFecRing ( fecSlotMin, fecSlotMax, ringMin, ringMax ) ;
    delete fecList ;
    
    if (scanCCUs) {
      
      for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
	std::list<keyType> *ccuList = p->second->getCcuList ( ) ;
	delete ccuList ;
      }
    }
  } 
}

// -------------------------------------------------------------------------------------
//
//                         Private methods for the VME configuration
//
// -------------------------------------------------------------------------------------

/** Configure the plug and play for the VME board
 * <p> See also FecVmeRingDevice::configurePlugAndPlay()
 * \param adapterSlot - adapter slot (equivalent in HAL of the crate number)
 * \param configurationFile - configuration for the FEC, Ring (mapFile)
 * \param mapFile - slot configuration (slot - config file)
 */
void FecAccess::configurePlugAndPlay ( unsigned adapterSlot, 
                                       std::string configurationFile, 
				       std::string mapFile, 
				       std::string strBusAdapter ) 
  throw ( FecExceptionHandler ) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  try {
    // Plug and play
    vme64xCrate_ = FecVmeRingDevice::configurePlugAndPlay ( adapterSlot, configurationFile, mapFile, strBusAdapter, hardwareDevices_ ) ;

    adapterSlot_ = adapterSlot ;
    configurationFile_ = configurationFile ;
    mapFile_ = mapFile ;
    strBusAdapter_ = strBusAdapter ;
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP( HAL_NOSUCHITEMEXCEPTION,
 				      HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
 				      ERRORCODE,
 				      0,
 				      "Adapter slot", adapterSlot) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP( HAL_BUSADAPTEREXCEPTION,
			      HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
			      ERRORCODE,
			      0,
			      "Adapter slot", adapterSlot) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_ILLEGALOPERATIONEXCEPTION,
			       HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
			       ERRORCODE,
			       0,
			       "Adapter slot", adapterSlot) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_VERIFYEXCEPTION,
			       HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
			       ERRORCODE,
			       0,
			       "Adapter slot", adapterSlot) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_MASKBOUNDARYEXCEPTION,
			       HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
			       ERRORCODE,
			       0,
			       "Adapter slot", adapterSlot) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
			       HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
			       ERRORCODE,
			       0,
			       "Adapter slot", adapterSlot) ;
    //e.what()) ;
  }
  catch (...) { 
    //std::cout << "Uknown problem in FecVmeRingDevice::setFifoTransmit(BLT transfer) " << std::endl ; 
    RAISEFECEXCEPTIONHANDLER_INFOSUP( CODECONSISTENCYERROR,
			       "Exception unknown raised",
			       ERRORCODE,
			       0,
			       "Adapter slot", adapterSlot) ;
  }
#endif
}

/** Take each slot and retreive through the table the base address of the corresponding board
 * <p> See also FecVmeRingDevice::configureHardBaseAddress()
 * \param adapterSlot - adapter slot (equivalent in HAL of the crate number)
 * \param configurationFile - configuration for the FEC, Ring 
 * \param vmeBaseAddresses - this array contains for each slot the base address of the board. Note that if the base address is 0 then no board is given at this slot.
 */
void FecAccess::configureBaseAddresses ( unsigned adapterSlot, 
					 std::string configurationFile, 
					 const uint32_t *vmeBaseAddresses, 
					 std::string strBusAdapter ) 
  throw ( FecExceptionHandler ) {

  
  
#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    ERRORCODE ) ;
#else
  // No plug and play
  vme64xCrate_ = NULL ;

  // For each slot, retreive the corresponding HardwareDevice
  for ( unsigned i = FecVmeRingDevice::MINVMEFECSLOT ; i <= FecVmeRingDevice::MAXVMEFECSLOT ; i ++ ) {

    try {

       
      // Base Address = 0
      if (vmeBaseAddresses[i] != 0) {

	try {
	  hardwareDevices_[i] = (HAL::VME64xDevice *)FecVmeRingDevice::configureHardBaseAddress (adapterSlot, configurationFile, vmeBaseAddresses[i], i, strBusAdapter ) ;


	  // for some reason the creation of VME device fails
	  if (hardwareDevices_[i] == NULL) {
	    
	    // errorCode,errorMessage,faultSeverity,hardPosition
	    RAISEFECEXCEPTIONHANDLER(HAL_BUSADAPTEREXCEPTION,
						  "The VME Device fails to be created for address " + vmeBaseAddresses[i],
						  FATALERRORCODE) ;
	  }

#ifdef DEBUGMSGERROR
	  std::cout << "The Description gives a FEC address 0x" << std::hex << vmeBaseAddresses[i] << std::dec <<" at slot " << i << std::endl ;
#endif
	}
	catch (FecExceptionHandler &e) {

	  // If the error means not support was compiled raise the exception to the main method
	  if (e.getErrorCode() == TSCFEC_FECDDNOTOPEN) throw e ;

	  hardwareDevices_[i] = NULL ;
	  std::cerr << "No FEC connected on the slot " << i << std::endl ;
	  std::cerr << e.what() << std::endl ;

#ifdef DEBUGMSGERROR
	  std::cout << "No FEC connected on the slot " << i << std::endl ;
	  std::cout << e.what() << std::endl ;
#endif
	}
      }
#ifdef DEBUGMSGERROR
      else std::cout << "slot " << i << " empty" << std::endl ;
#endif
    }
    // Bad item name => Software problem
    catch ( HAL::NoSuchItemException &e ) {

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_NOSUCHITEMEXCEPTION,
				 HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
				 ERRORCODE,
				 0,
				 "Adapter slot", adapterSlot) ;
      //e.what()) ;
    }
    // Hardware problem access: Crate, SBS, driver SBS
    catch ( HAL::BusAdapterException &e ) {

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_BUSADAPTEREXCEPTION,
				 HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
				 ERRORCODE,
				 0,
				 "Adapter slot", adapterSlot) ;
      // e.what()) ;
    }
    // Bad operation
    catch ( HAL::IllegalOperationException &e ) {

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_ILLEGALOPERATIONEXCEPTION,
				 HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
				 ERRORCODE,
				 0,
				 "Adapter slot", adapterSlot) ;
      //e.what()) ;
    }
    // If you write to an item you can specify in the call a parameter which
    // then makes hal immediately read back the item you wrote in order to
    // verify that it really arrived in a register. This exception is thrown
    // then the value read back is not equal to the one originally written.
    // (Perfect for automatic hardware tests)
    catch ( HAL::VerifyException &e ) {

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_VERIFYEXCEPTION,
				 HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
				 ERRORCODE,
				 0,
				 "Adapter slot", adapterSlot) ;
      //e.what()) ;
    }
    // Imagine you have defined an item with the mask 0x000000f0
    // this means the item is contained in bits 4..7
    // now you may write values 0 to 0xf into these 4 bits. If you try to write
    // 10 you get this exception since it would try to set bit 8 which is out
    // of the mask region
    catch ( HAL::MaskBoundaryException &e ) {

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_MASKBOUNDARYEXCEPTION,
				 HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
				 ERRORCODE,
				 0,
				 "Adapter slot", adapterSlot) ;
      //e.what()) ;    
    }
    // When you work with offsets it is checked that you stay in the limits of
    // the addressTable. This gives some means of security against programming
    // bugs when you work with offsets. (See manual if you need more info but
    // there is not much more behind it...)
    catch ( HAL::AddressOutOfLimitsException &e ) {

      RAISEFECEXCEPTIONHANDLER_INFOSUP ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
				 HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
				 ERRORCODE,
				 0,
				 "Adapter slot", adapterSlot) ;
      //e.what()) ;
    }
    catch (...) { 
      //std::cout << "Uknown problem in FecVmeRingDevice::setFifoTransmit(BLT transfer) " << std::endl ; 
      RAISEFECEXCEPTIONHANDLER_INFOSUP( CODECONSISTENCYERROR,
				 "Exception unknown raised",
				 ERRORCODE,
				 0,
				 "Adapter slot", adapterSlot) ;
    }
  }
#endif
}

// -------------------------------------------------------------------------------------
//
//                                Software methods
//
// -------------------------------------------------------------------------------------

/** Depending of the FEC type return the minimum ring number:
 * <ul>
 * <li>VME: 0 to 7 or 1 to 8
 * <li>PCI: 0 to X
 * <li>USB: 0 to X
 * </ul>
 * \return minimum ring
 * \warning this method does not check the hardware rings just the software issue as declared in file FecAddressTable.dat
 */
tscType16 FecAccess::getMinRingNumber ( ) {

  switch (fecBusType_) {
  case FECVME: 
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
    return FecVmeRingDevice::getMinRingValue() ; 
#endif
    break ;
  case FECPCI: 
#if defined(BUSPCIFEC)
    return FecPciRingDevice::MINPCIFECRING ; 
#endif
    break ;
  case FECUSB: 
#if defined(BUSUSBFEC)
    return FecUsbRingDevice::minUsbFecRing ; 
#endif
    break ;
  }

  return 0xFFFF ;
}

/** Depending of the FEC type return the maximum ring number:
 * <ul>
 * <li>VME: 0 to 7 or 1 to 8
 * <li>PCI: 0 to X
 * <li>USB: 0 to X
 * </ul>
 * \return minimum ring
 * \warning this method does not check the hardware rings just the software issue as declared in file FecAddressTable.dat
 */
tscType16 FecAccess::getMaxRingNumber ( ) {

  switch (fecBusType_) {
  case FECVME: 
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
    return FecVmeRingDevice::getMaxRingValue() ; 
#endif
    break ;
  case FECPCI: 
#if defined(BUSPCIFEC)
    return FecPciRingDevice::MAXPCIFECRING ;
#endif
    break ;
  case FECUSB: 
#if defined(BUSUSBFEC)
    return FecUsbRingDevice::maxUsbFecRing ; 
#endif
    break ;
  }

  return 0 ;
}

/** Set the force acknoledge on i2c channels
 * for all the FecRingDevice (for the next creation)
 * \param forceAck - boolean to force or not the ack
 */
void FecAccess::setForceAcknowledge ( bool forceAck ) {

  forceChannelAck_ = forceAck ;
}

/** Get the force acknoledge on i2c channels
 * \return true or false
 */
bool FecAccess::getForceAcknowledge ( ) {

  return (forceChannelAck_) ;
}

/** Initialise all the FecRingDevice (for the next creation)
 * \param fecDeviceInit - boolean to initialise or not the FecRingDevice
 */
void FecAccess::setFecRingDeviceInit ( bool init ) {

  initFecRingDevice_ = init ;
}

/** Retreive the value of the FecRingDevice init
 * \return true or false
 */
bool FecAccess::getFecRingDeviceInit ( ) {

  return (initFecRingDevice_) ;
}

/*
 * \return the bus type: VME, USB, PCI
 */
enumFecBusType FecAccess::getFecBusType ( ) {

  return (fecBusType_) ;
}

/** Method to set the i2c speed for a given index, note that only future
 * connection will support this speed
 * \param i2cSpeed - value to be set
 * \exception FecExceptionHandler: exception if the i2c speed is not correct
 */
void FecAccess::seti2cChannelSpeed ( tscType16 i2cSpeed ) {

  if ( (i2cSpeed != 100) && (i2cSpeed != 200) && (i2cSpeed != 400) && (i2cSpeed != 1000)) {

    RAISEFECEXCEPTIONHANDLER( XDAQFEC_INVALIDOPERATION,
			      "the i2c speed is not correct (i2cSpeed = " + toString(i2cSpeed) + ")",
			      ERRORCODE ) ;
  }

  i2cSpeed_ = i2cSpeed ;
}

/** Return the i2c speed set
 * \return i2cSpeed value
 */
tscType16 FecAccess::geti2cChannelSpeed ( ) {

  return (i2cSpeed_) ;
}

/** \param invertClockPolarity - true to invert the clock polarity
 */
void FecAccess::setInvertClockPolarityFlag ( bool invertClockPolarity ) {

  invertClockPolarity_ = invertClockPolarity ;
}

/** \return if the clock polarity is inverted
 */
bool FecAccess::getInvertClockPolarityFlag ( ) {

  return invertClockPolarity_ ;
}

/** \param index - index of the FEC, ring
 * \return FecRingDevice corresponding of the FEC, ring
 * \exception FecExceptionHandler
 */
FecRingDevice *FecAccess::setFecRingDevice ( keyType index ) {

  // Try to find the FEC device
  FecRingDevice *fec = NULL ;
  fecMapAccessedType::iterator fecPair = 
    fecRingEnable_.find(buildFecRingKey(getFecKey(index), getRingKey(index))) ;

  // Add the FEC to the list of FEC enable if it is not existing
  if (fecPair == fecRingEnable_.end()) {

#ifdef DEBUGMSGERROR
    std::cout << "Try to create FEC Ring " << getFecKey(index) << "." << getRingKey(index) << std::endl ;
#endif

    switch ( fecBusType_ ) {
    case FECPCI:
#if defined(BUSPCIFEC)
      fec = new FecPciRingDevice ( getFecKey(index), getRingKey(index), initFecRingDevice_, invertClockPolarity_ ) ;
#endif
      break ;
    case FECVME:
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)

#ifdef DEBUGMSGERROR
      std::cout << "----------------------------------> FEC " << getFecKey(index) << " ring " << getRingKey(index) << ": vmeDevice = 0x" << hardwareDevices_[getFecKey(index)] << std::endl ;
#endif
      
      if (hardwareDevices_.find(getFecKey(index)) != hardwareDevices_.end()) {

	// Create the VME access
	fecRingDeviceFifoAccessModeEnum blockModeE = FECNOBLT ;
	if (blockMode_) blockModeE = FECDOBLT ;

	try {

	  fec = new FecVmeRingDevice ( hardwareDevices_[getFecKey(index)], getFecKey(index), getRingKey(index), initFecRingDevice_, blockModeE, 0, invertClockPolarity_ ) ;
	  
	}
	catch (FecExceptionHandler e) {
	  throw e ;
	}

	if (!reloadedFirmware_[getFecKey(index)] && ((FecVmeRingDevice *)fec)->getmFecInitialised() == 0) { // hardware access created but problem during initialisation, trying to reload the mFEC firmware
#ifdef DEBUGMSGERROR
	  std::cout << "Warning: FEC on slot " << std::dec << getFecKey(index) << " was created but the intialisation failed, trying to reload the mFEC firmware" << std::endl ;
#endif
	  ((FecVmeRingDevice *)fec)->uFecsReload() ;
	  // this will or initialise correctly the ring or will raise a FecExceptionHandler if the hardware is not existing
	  ((FecVmeRingDevice *)fec)->setInitFecRingDevice ( initFecRingDevice_, invertClockPolarity_ ) ; 
	  reloadedFirmware_[getFecKey(index)] = 1 ;

#ifdef EVGUENI_INVERTCLOCKPOLARITY
	  // Invert the clock polarity for the rest of the mFECs (Evgueni changes)
	  if (invertClockPolarity_) {
	    for (fecMapAccessedType::iterator fecPair = fecRingEnable_.begin() ; fecPair != fecRingEnable_.end() ; fecPair ++) {
	      FecVmeRingDevice *fec = (FecVmeRingDevice *)fecPair->second ;
	      if (fec != NULL) fec->setInvertClockPolarity(invertClockPolarity_) ;
	    }
	  }
#endif
	}
	else 
	  if (((FecVmeRingDevice *)fec)->getmFecInitialised() == 1) { // everything goes fine
	    reloadedFirmware_[getFecKey(index)] = 1 ;
#ifdef DEBUGMSGERROR
	    std::cout << "Everything is fine on FEC " << getFecKey(index) << "." << getRingKey(index) << ": mfec ok = " << ((FecVmeRingDevice *)fec)->getmFecInitialised() << " mFEC firmware " << std::hex << ((FecVmeRingDevice *)fec)->getFecFirmwareVersion() << std::dec << std::endl ;
#endif
	  }
	  else if (((FecVmeRingDevice *)fec)->getmFecInitialised() == 0) {
#ifdef DEBUGMSGERROR
	    std::cout << "Did not reload the firmware on the FEC " << getFecKey(index) << "." << getRingKey(index) << ": mfec ok = " << ((FecVmeRingDevice *)fec)->getmFecInitialised() << std::endl ;
#endif
	    // force the failing in reading the SR0 from the FEC
	    ((FecVmeRingDevice *)fec)->getFecRingSR0() ;
	  }
#ifdef DEBUGMSGERROR
	  else if (((FecVmeRingDevice *)fec)->getmFecInitialised() == -1)
	    std::cout << "Not hardware access was created on the FEC " << getFecKey(index) << "." << getRingKey(index) << ": mfec ok = " << ((FecVmeRingDevice *)fec)->getmFecInitialised() << std::endl ;
#endif

	// Set the block mode to block transfer
	//if (fec->getFecFirmwareVersion () >= MINFIRMWAREVERSION) fec->setFifoAccessMode(FECDOBLT) ;

	std::string fecHardwareId = ((FecVmeRingDevice *)fec)->getFecHardwareId() ;

#ifdef DEBUGMSGERROR
	std::cout << "FEC " << fecHardwareId << " is on slot " << std::dec << getFecKey(index) << std::endl ;
#endif

	fecHardwareIdMapIndex_[fecHardwareId.c_str()] = setFecSlotKey(getFecKey(index)) ;
      }
      else {

	RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				    "FEC slot does not exists",
				    ERRORCODE,
				    index ) ;

      }
#endif
      break ;
    case FECUSB: 
#if defined(BUSUSBFEC)
      
      std::string serialNumber = fecUsbSerialNumbers_[getFecKey(index)]; 
      
      if (!serialNumber.empty()) { 
	std::cout << "Trying fec: " << getFecKey(index) 
		  << " ring: " <<  getRingKey(index) 
		  << " serial: " << serialNumber << std::endl ; 
      fec = new FecUsbRingDevice (serialNumber, getFecKey(index), getRingKey(index), 
				  initFecRingDevice_, invertClockPolarity_ ) ;
      if (fec) std::cout << "Creation FecRingDevice OK. " << std::endl ; 
      } else { 
	std::cout  << "Serial number for fec slot: " << getFecKey(index)
		   << " is empty." << std::endl ;  
      }
#endif
      break ;
    }

    // A fec ring device must be opened and add to the map  
    if (fec != NULL) {
      // Note the hardware interface is deleted by the FecRingDevice
      fecRingEnable_[buildFecRingKey(getFecKey(index), getRingKey(index))] = fec ;
    }
  }
  else {
    fec = fecPair->second ;
#ifdef DEBUGMSGERROR
    std::cout << "The FEC (" << (int)fec->getFecSlot() << "." << (int)fec->getRingSlot() << ") is existing in the FecAccess table" << std::endl ;
#endif    
  }

  return (fec) ;
}

/** Get the FecRingDevice corresponding to the fecSlot (device driver class for the slot)
 * \param index - index of the FEC, ring
 * \return the corresponding FecRingDevice if it exists
 * \exception FecExceptionHandler:
 * <ul>
 * <li>XDAQFEC_INVALIDOPERATION
 * </ul>
 */
FecRingDevice* FecAccess::getFecRingDevice ( keyType index ) 
  throw (FecExceptionHandler) {

  keyType fecRingKey = getFecRingKey(index) ;

  FecRingDevice *fec ;
  fecMapAccessedType::iterator fecPair = fecRingEnable_.find(fecRingKey) ;

  if (fecPair == fecRingEnable_.end()) {
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"FEC slot does not exists",
				ERRORCODE,
				index ) ;
  } 


  // FecRingDevice object
  fec = fecPair->second ;
  return(fec) ;
}


/** Check index and return if needed an exception
 * \param index - index of device or channel
 * \return ccuChannelAccess - channel access (i2c, pia)
 * \exception FecExceptionHandler:
 * <ul>
 * <li>XDAQFEC_INVALIDOPERATION
 * </ul>
 */
ccuChannelAccess *FecAccess::checkIndex ( keyType index ) 
  throw (FecExceptionHandler) {

#ifdef T3TDLEVELCHECK
  // Check if the index is correct 
  if ( ! isIndexCorrectCcu25(index) && ! isIndexCorrectOldCcu(index) ) {
    
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"One or several parameters are not correct in the index",
				ERRORCODE,
				index ) ;
  }

//  if (getModeKey(index) == TSC_MODE_WRITE) {
//    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
//				"Cannot read, the mode access is in write mode only",
//				ERRORCODE,
//				index ) ;
//  }
#endif

  deviceMapAccessedType::iterator device = deviceEnable_.find(index) ;
  if (device == deviceEnable_.end()) { // Device not found
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"device is not connected, please use add method in order to connect the device and/or the channel",
				ERRORCODE,
				index ) ;
  }

  return (device->second) ;
}

/** Get the list of keys for the FECs
 * \return a list of keyType (must be delete after)
 * \warning no scan are performed in this part, you must call the method
 * scanFecRing before or have called the constructor with a scan equal to true
 * \see FecAccess::scanFecRing
 * \see FecAccess::FecAccess ( ... )
 */
std::list<keyType> *FecAccess::getFecList ( ) {

  tscType8 fecMin = 0, fecMax = 0, ringMin = 0, ringMax = 0 ;
  
  switch ( fecBusType_ ) {
  case FECPCI:
#if defined(BUSPCIFEC)
    fecMin = FecPciRingDevice::MINPCIFECSLOT ;
    fecMax = FecPciRingDevice::MAXPCIFECSLOT ;
    ringMin = FecPciRingDevice::MINPCIFECRING ;
    ringMax = FecPciRingDevice::MAXPCIFECRING ;
#endif
    break ;
  case FECVME:
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
    fecMin = FecVmeRingDevice::MINVMEFECSLOT ;
    fecMax = FecVmeRingDevice::MAXVMEFECSLOT ;
    ringMin = FecVmeRingDevice::getMinVmeFecRingValue() ;
    ringMax = FecVmeRingDevice::getMaxVmeFecRingValue() ;
#endif
    break ;
  case FECUSB: 
#if defined(BUSUSBFEC)
    fecMin = FecUsbRingDevice::minUsbFecSlot ;
    ringMin = FecUsbRingDevice::minUsbFecRing ;    
    fecMax = FecUsbRingDevice::maxUsbFecSlot ;
    ringMax = FecUsbRingDevice::maxUsbFecRing ;
#endif
    break ;
  }

  // List
  std::list<keyType> *fecList = NULL ;
  std::vector<keyType> fecV ;
  
  // Retreive each FecRingDevice
  for ( int fecSlot = fecMin ; fecSlot <= fecMax ; fecSlot ++ ) {
    
    //for ( int ringSlot = ringMin ; ringSlot <= ringMax ; ringSlot ++ ) {
    for ( int ringSlot = ringMax ; ringSlot >= ringMin ; ringSlot -- ) {
      
      keyType index = buildFecRingKey(fecSlot, ringSlot) ;
      try {
        FecRingDevice *fec = getFecRingDevice ( index ) ;
        if (fec != NULL) fecV.push_back (index) ;
      }
      catch (FecExceptionHandler &e) {
      }
    }
  } 

  // Re-order the rings
  if (fecV.size()) {
    if ( (fecBusType_ == FECVME) && (getMinRingNumber() == 1) ) // sort it by FEC and ring from 8 to 1 to have the same numbering than the front panel 
      std::sort(fecV.begin(),fecV.end(),FecAccess::sortByFrontPanelNumber) ;
    else std::sort(fecV.begin(),fecV.end()) ;
    fecList = new std::list<keyType>(fecV.begin(),fecV.end()) ;
  }
  
  return (fecList) ;
}

/** Set the block for each FEC already declared and set the next one to use the block mode.
 * \param blockMode - block mode used (see FecVmeRingDevice)
 */
void FecAccess::setFifoAccessMode ( bool blockMode ) {

  // Set the block
  blockMode_ = blockMode ;

  // Change the FEC already declared
  if ( (fecBusType_ == FECVME) && (fecRingEnable_.size()) ) {

    fecRingDeviceFifoAccessModeEnum blockModeE = FECNOBLT ;
    if (blockMode_) blockModeE = FECDOBLT ;
    
    for (fecMapAccessedType::iterator fecPair = fecRingEnable_.begin() ; fecPair != fecRingEnable_.end() ; fecPair ++) {
      
      FecVmeRingDevice *fec = (FecVmeRingDevice *)fecPair->second ;
      fec->setFifoAccessMode (blockModeE) ;
    }
  }
}

/** \return block mode, do nothing if the bus type is not a VME FEC
 */
bool FecAccess::getFifoAccessMode ( ) {

  return blockMode_ ;
}

/** \brief set the timeout for the wait of FIFO transmit running
 * \warning please do not use this method you will have errors in the system !
 */
void FecAccess::setLoopInTimeWriteFrame ( unsigned long loopInTimeWriteFrame ) {
  for (fecMapAccessedType::iterator fecPair = fecRingEnable_.begin() ; fecPair != fecRingEnable_.end() ; fecPair ++) {
      
    FecRingDevice *fec = (FecRingDevice *)fecPair->second ;
    fec->setLoopInTimeWriteFrame(loopInTimeWriteFrame) ;
  }
}

/** \brief set the timeout for the wait of the direct ack
 * \warning please do not use this method you will have errors in the system !
 */
void FecAccess::setLoopInTimeDirectAck ( unsigned long loopInTimeDirectAck ) {
  for (fecMapAccessedType::iterator fecPair = fecRingEnable_.begin() ; fecPair != fecRingEnable_.end() ; fecPair ++) {
      
    FecRingDevice *fec = (FecRingDevice *)fecPair->second ;
    fec->setLoopInTimeDirectAck(loopInTimeDirectAck) ;
  }
}

/** \brief set the timeout for the wait of the force ack or answer
 * \warning please do not use this method you will have errors in the system !
 */
void FecAccess::setLoopInTimeReadFrame ( unsigned long loopInTimeReadFrame ) {
  for (fecMapAccessedType::iterator fecPair = fecRingEnable_.begin() ; fecPair != fecRingEnable_.end() ; fecPair ++) {
      
    FecRingDevice *fec = (FecVmeRingDevice *)fecPair->second ;
    fec->setLoopInTimeReadFrame(loopInTimeReadFrame) ;
  }
}

/**
 * \param index - index of the FEC
 * \return transmit fifo size (depth) in D32 words 
 */
unsigned int FecAccess::getTransmitFifoDepth ( keyType index ) throw (FecExceptionHandler) {

  FecRingDevice *fec = getFecRingDevice ( index ) ;
  return fec->getTransmitFifoDepth() ;
}

/**
 * \param index - index of the FEC
 * \return receive fifo size (depth) in D32 words 
 */
unsigned int FecAccess::getReceiveFifoDepth ( keyType index  ) throw (FecExceptionHandler) {
  
  FecRingDevice *fec = getFecRingDevice ( index ) ;
  return fec->getReceiveFifoDepth() ;
}

/** 
 * \param index - index of the FEC
 * \return fifo size (depth) in D32 words 
 */
unsigned int FecAccess::getReturnFifoDepth ( keyType index ) throw (FecExceptionHandler) {

  FecRingDevice *fec = getFecRingDevice ( index ) ;
  return fec->getReturnFifoDepth() ;
}

/**
 * \param index - index of the FEC
 */
void FecAccess::emptyFifoReceive ( keyType index  ) throw (FecExceptionHandler) {
  
  FecRingDevice *fec = getFecRingDevice ( index ) ;
  return fec->emptyFifoReceive() ;
}

/**
 * \param index - index of the FEC
 */
void FecAccess::emptyFifoTransmit ( keyType index  ) throw (FecExceptionHandler) {
  
  FecRingDevice *fec = getFecRingDevice ( index ) ;
  return fec->emptyFifoTransmit() ;
}

/**
 * \param index - index of the FEC
 */
void FecAccess::emptyFifoReturn ( keyType index  ) throw (FecExceptionHandler) {
  
  FecRingDevice *fec = getFecRingDevice ( index ) ;
  return fec->emptyFifoReturn() ;
}

// -------------------------------------------------------------------------------------
//
//                                FEC methods
//
// -------------------------------------------------------------------------------------

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
/** Return the CCS trigger depending of the FEC slot
 * \return address of the CCSTrigger
 */
CCSTrigger *FecAccess::getCCSTrigger ( keyType index ) throw (FecExceptionHandler) {

  keyType fecRingKey = setFecSlotKey(getFecKey(index)) ;
  
  CCSTrigger *ccsTrigger ;
  ccsTriggerMapAccessedType::iterator ccsPair = ccsTriggerList_.find(fecRingKey) ;

  if (ccsPair == ccsTriggerList_.end()) {
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"FEC slot does not exists",
				ERRORCODE,
				index ) ;
  } 


  // FecRingDevice object
  ccsTrigger = ccsPair->second ;
  return(ccsTrigger) ;
}
#endif

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
/** Set the CCS trigger for the given SSID
 * \param index - index of the FEC 
 * \param SSID - SSID of the system
  */
CCSTrigger *FecAccess::setCCSTrigger ( keyType index, std::string SSID ) throw (FecExceptionHandler) {

  CCSTrigger *ccsTrigger = NULL ;

  if (hardwareDevices_.find(getFecKey(index)) != hardwareDevices_.end()) {

    // Create the CCS trigger access
    keyType fecRingKey = setFecSlotKey(getFecKey(index)) ;
    
    // Try to find the CCS trigger in the map
    ccsTriggerMapAccessedType::iterator ccsPair = ccsTriggerList_.find(fecRingKey) ;

    if (ccsPair == ccsTriggerList_.end()) {

      ccsTrigger = new CCSTrigger ( hardwareDevices_[getFecKey(index)], SSID, getFecKey(index) ) ;
      ccsTriggerList_[setFecSlotKey(getFecKey(index))] = ccsTrigger ;
    }
    else {

      ccsTrigger = ccsPair->second ;
      ccsTrigger->setSSID (SSID) ;
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"FEC slot does not exists",
				ERRORCODE,
				index ) ;
  }

  return (ccsTrigger) ;
}
#endif

/** Retreive the FEC trigger configuration register 0
 * \param index of the FEC
 * \return value of the register
 */
uint32_t FecAccess::getCCSTriggerConfig0 ( keyType index ) throw (FecExceptionHandler) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  return (getCCSTrigger(index)->getCCSTriggerConfig0()) ;
#endif
}


/** Set the control register 0 of the CCS board
 * \param index of the FEC
 * \param value to be set in the register
 */
void FecAccess::setCCSTriggerConfig0 ( keyType index, uint32_t value ) throw (FecExceptionHandler) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;

#else
  getCCSTrigger(index)->setCCSTriggerConfig0(value) ;
#endif
}

/** Retreive the FEC trigger status register 0
 * \param index of the FEC
 * \return value of the register
 */
uint32_t FecAccess::getCCSTriggerStatus0 ( keyType index ) throw (FecExceptionHandler) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;

#else
  return (getCCSTrigger(index)->getCCSTriggerStatus0()) ;
#endif
}

/** Retreive the FEC trigger SSID
 * \param index of the FEC
 * \return name of the sub-detector
 */
std::string FecAccess::getSSID ( keyType index ) throw (FecExceptionHandler) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  return (getCCSTrigger(index)->getSSID()) ;
#endif
}

/** Retreive the temperature for the sensors on the VME FEC
 * \param index - index of the FEC 
 */
void FecAccess::getFecTemperature ( keyType index, int &tempSensor1, int &tempSensor2 ) 
  throw (FecExceptionHandler) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  if (hardwareDevices_.find(getFecKey(index)) != hardwareDevices_.end()) {

    FecVmeTemperature *fecTemp = NULL ;
    fecTemperatureMapAccessedType::iterator fecTempIt = fecTemperatureList_.find(index) ;
    
    if (fecTempIt == fecTemperatureList_.end()) {
      // build the temperature access
      fecTemp = new FecVmeTemperature ( hardwareDevices_[getFecKey(index)], getFecKey(index) ) ;
      fecTemperatureList_[index] = fecTemp ;
    }
    else fecTemp = fecTempIt->second ;

    tempSensor1 = fecTemp->getTemp(1) ;
    tempSensor2 = fecTemp->getTemp(2) ;
  }
  else
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"FEC slot does not exists",
				ERRORCODE,
				index ) ;
#endif
}

/** scan for FECs and rings
 * <p>See also the constructor FecRingDevice::FecRingDevice()
 * \param fecSlotMin - first FEC slot
 * \param fecSlotMax - last FEC slot
 * \param ringMin - first ring (0 top or 1 bottom)
 * \param ringMax - last ring (7 bottom or 8 top)
 * \return a list of keyType
 * \warning this method add all the FECs to the FEC map
 * \warning this method is not recommanded due to the fact that
 * several FecSupervisor can managed the same PCI/VME bus
 * \exception An exception is thrown if one of the device driver does not have the correct version (version between device driver and API software): 
 * <ul>
 * <li>TSCFEC_BADCOMPATIBILITY
 * </ul>
 */
std::list<keyType> *FecAccess::scanFecRing (tscType8 fecSlotMin,
					    tscType8 fecSlotMax,
					    tscType8 ringMin,
					    tscType8 ringMax ) 
  throw (FecExceptionHandler) {

  // For each slot
  for ( int fecSlot = fecSlotMin ; fecSlot <= fecSlotMax ; fecSlot ++) {
      
    //for ( int ringSlot = ringMin ; ringSlot <= ringMax ; ringSlot ++ ) {
    for ( int ringSlot = ringMax ; ringSlot >= ringMin ; ringSlot -- ) {

      try {
	// Find or create the corresponding FecDevice
	setFecRingDevice ( buildFecRingKey(fecSlot,ringSlot) ) ;

#ifdef DEBUGMSGERROR
	std::cout << "FecAccess::scanFecRing: Found a FEC on slot number " << fecSlot << ", ring " << ringSlot << std::endl ;
#endif
      }
      catch (FecExceptionHandler &e) {
	  
#ifdef DEBUGMSGERROR
	std::cout << e.what() << std::endl ;
#endif
	
	// Bad version for the device driver
	if (e.getErrorCode() == TSCFEC_BADCOMPATIBILITY) {
	  
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_BADCOMPATIBILITY,
						  "the version is not compatible between the driver loaded and the FEC C++ API",
						  CRITICALERRORCODE,
						  buildFecRingKey(fecSlot,ringSlot) ) ;
	}
      }
    }
  }
    
  // Fill the list
  std::list<keyType> *fecList = NULL ;
  for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
    if (p->second != NULL) {
      if (fecList == NULL) fecList = new std::list<keyType> ;
      fecList->push_back(buildFecRingKey(p->second->getFecSlot(), p->second->getRingSlot())) ;
    }
  }
    
  return (fecList) ;
}

/** This method scan the crate for a given FEC hardware ID and create the FEC rings needed
 * \param fecHardwareId - FEC hardware ID
 * \return index of the FEC
 * \warning this method is only available for VME FEC
 */
keyType FecAccess::scanFecRing ( std::string fecHardwareId ) throw (FecExceptionHandler) {
  
  // std::cout << __PRETTY_FUNCTION__ << std::endl; 
  
#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else

#ifdef DEBUGMSGERROR
  std::cout << "Scan the crate to find the FEC " << fecHardwareId << std::endl ;
#endif

  keyType index = 0 ;

  if (fecBusType_ != FECVME) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"only available for VME FEC",
				ERRORCODE,
				index) ;
  }

  // For each slot
  for ( unsigned int fecSlot = FecVmeRingDevice::MINVMEFECSLOT ; fecSlot <= FecVmeRingDevice::MAXVMEFECSLOT ; fecSlot ++) {

    if (hardwareDevices_.find(fecSlot) != hardwareDevices_.end()) {

#ifdef DEGBUMSGERROR
      std::cout << "Serial number = " << FecVmeRingDevice::getSerialNumber(fecSlot,adapterSlot_) << " for FEC " << fecSlot << std::endl ;
      std::cout << "FEC hardware ID to be found is: " << fecHardwareId << std::endl ;
#endif

      if (FecVmeRingDevice::getSerialNumber(fecSlot,adapterSlot_) == fecHardwareId) {

	index = setFecSlotKey(fecSlot) ;

	//for ( unsigned int ringSlot = FecVmeRingDevice::getMinVmeFecRingValue() ; ringSlot <= FecVmeRingDevice::getMaxVmeFecRingValue() ; ringSlot ++ ) {
	for ( unsigned int ringSlot = FecVmeRingDevice::getMaxVmeFecRingValue() ; ringSlot >= FecVmeRingDevice::getMinVmeFecRingValue() ; ringSlot -- ) {

	  try {

	    // Find or create the corresponding FecDevice
	    setFecRingDevice ( buildFecRingKey(fecSlot,ringSlot) ) ;

#ifdef DEBUGMSGERROR
	    std::cout << "FecAccess::scanFecRing: Found a FEC on slot number " << fecSlot << ", ring " << ringSlot << std::endl ;
#endif
	  }
	  catch (FecExceptionHandler &e) { 
	    
#ifdef DEBUGMSGERROR
	    std::cout << e.what() << std::endl ;
#endif
	  
	    // Bad version for the device driver
	    if (e.getErrorCode() == TSCFEC_BADCOMPATIBILITY) {

	      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_BADCOMPATIBILITY,
					  "the version is not compatible between the driver loaded and the FEC C++ API",
					  CRITICALERRORCODE,
					  buildFecRingKey(fecSlot,ringSlot) ) ;
	    }
	  }
	}
      }
    }
  }

  return (index) ;  

#endif
}

/**
 * \param index - index of the FEC
 * \warning no error is raised one of the CCU registers has errors
 */
void FecAccess::clearFecCcuErrorRegisters ( keyType index, bool noBroadcast ) throw (FecExceptionHandler) {
  
  // Clear the FEC CR1 
  setFecRingCR1(index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

  // Clear the CCU registers
  //if (isFecSR0Correct(getFecRingSR0(index))) {
  std::list<keyType> *ccuList = getCcuList ( index, noBroadcast ) ;
  if ( (ccuList != NULL) && (ccuList->size()) ) {
    for (std::list<keyType>::iterator it = ccuList->begin() ; it != ccuList->end() ; it ++) {
      try {
	keyType ccuIndex = *it ;
	setCcuCRA (ccuIndex, CCU_CRA_CLRE) ;
      }
      catch (FecExceptionHandler &e) {	}
    }
  }
  if (ccuList != NULL) delete ccuList ;
  //}
}

/**
 * \param index - index of the FEC
 * \return firmware version
 */
tscType16 FecAccess::getFecFirmwareVersion( keyType index ) throw ( FecExceptionHandler ) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			    "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  if (fecBusType_ == FECVME) {

    // To avoid problem in the numbering schema (mask and the ring and replace it by 0 or 8
    if (FecVmeRingDevice::getMinVmeFecRingValue()==1) index = buildFecRingKey(getFecKey(index),8);
    else index = buildFecRingKey(getFecKey(index),0);
    
    FecRingDevice *fec = getFecRingDevice ( index ) ;
    return fec->getFecFirmwareVersion( ) ;
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
			       "only available for VME FEC",
			       ERRORCODE,
			       index) ;
  }
  
  return 0 ;
#endif
}

/**
 * \param index - index of the FEC
 * \return return FEC hardware ID
 * \warning this method is only used for VME FEC, raise an exception if it is used for another FEC
 * \see FecAccess::getFecIndex ( std::string ) 
 */
std::string FecAccess::getFecHardwareId ( keyType index ) throw (FecExceptionHandler) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  if (fecBusType_ == FECVME) {

    // To avoid problem in the numbering schema (mask and the ring and replace it by 0 or 8
    if (FecVmeRingDevice::getMinVmeFecRingValue()==1) index = buildFecRingKey(getFecKey(index),8);
    else index = buildFecRingKey(getFecKey(index),0);

    // Retreive the corresponding FecRingDevice
    FecRingDevice *fec = getFecRingDevice ( index ) ;

    // Retreive the serial number
    return (((FecVmeRingDevice *)fec)->getFecHardwareId()) ;
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"only available for VME FEC",
				ERRORCODE,
				index) ;
  }

  return "0" ;
#endif
}

/**
 * \param index - index of the FEC
 * \return return the trigger firmware version of the FEC
 * \warning this method is only used for VME FEC, raise an exception if it is used for another FEC
 * \see FecAccess::getFecIndex ( std::string ) 
 */
tscType32 FecAccess::getTriggerVersion ( keyType index ) throw (FecExceptionHandler) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;
#else
  if (fecBusType_ == FECVME) {

    // To avoid problem in the numbering schema (mask and the ring and replace it by 0 or 8
    if (FecVmeRingDevice::getMinVmeFecRingValue()==1) index = buildFecRingKey(getFecKey(index),8);
    else index = buildFecRingKey(getFecKey(index),0);

    // Retreive the corresponding FecRingDevice
    FecRingDevice *fec = getFecRingDevice ( index ) ;

    // Retreive the serial number
    return (((FecVmeRingDevice *)fec)->getTriggerVersion()) ;
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"only available for VME FEC",
				ERRORCODE,
				index) ;
  }

  return 0 ;
#endif
}

/**
 * \param index - index of the FEC
 * \return return the VME version of the FEC
 * \warning this method is only used for VME FEC, raise an exception if it is used for another FEC
 * \see FecAccess::getFecIndex ( std::string ) 
 */
tscType32 FecAccess::getVmeVersion ( keyType index ) throw (FecExceptionHandler) {

#if !defined(BUSVMECAENPCI) && !defined(BUSVMECAENUSB) && !defined (BUSVMESBS)
  RAISEFECEXCEPTIONHANDLER (CODECONSISTENCYERROR,
			     "Support for CAEN and SBS VME is not compiled and cannot be used",
			    FATALERRORCODE ) ;

#else
  if (fecBusType_ == FECVME) {

    // To avoid problem in the numbering schema (mask and the ring and replace it by 0 or 8
    if (FecVmeRingDevice::getMinVmeFecRingValue()==1) index = buildFecRingKey(getFecKey(index),8);
    else index = buildFecRingKey(getFecKey(index),0);

    // Retreive the corresponding FecRingDevice
    FecRingDevice *fec = getFecRingDevice ( index ) ;

    // Retreive the serial number
    return (((FecVmeRingDevice *)fec)->getVmeVersion()) ;
  }
  else {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"only available for VME FEC",
				ERRORCODE,
				index) ;
  }

  return 0 ;
#endif
}

/**
 * \param fecHardwareId - FEC hardware ID
 * \return return index of the FEC
 * \see FecAccess::getFecgetFecHardwareId ( keyType ) 
 */
keyType FecAccess::getFecIndex ( std::string fecHardwareId ) throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  std::cout << "FEC " << fecHardwareId << " is on slot " << std::dec << getFecKey(fecHardwareIdMapIndex_[fecHardwareId.c_str()]) << std::endl ;
#endif

  return (fecHardwareIdMapIndex_[fecHardwareId.c_str()]) ;
}

/** 
 * \param index - key for the FEC
 * \param inputB - true if you want to use B, false if you want to use input A
 * \warning any change in the CR0 will keep the value set
 */
void FecAccess::setFecInputB ( keyType index, bool inputB ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Set the control register 0
  fec->setFecInputB ( inputB ) ;
}

/** 
 * \param index - key for the FEC
 * \param outputB - true if you want to use B, false if you want to use output A
 * \warning any change in the CR0 will keep the value set
 */
void FecAccess::setFecOutputB ( keyType index, bool outputB ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Set the control register 0
  fec->setFecOutputB ( outputB ) ;
}

/** 
 * \param index - key for the FEC
 * \param invertPolarity - inverse or not the polarity
 * \warning any change in the CR0 will keep the value set
 */
void FecAccess::setInvertClockPolarity ( bool invertPolarity ) 
  throw (FecExceptionHandler) {

  for (fecMapAccessedType::iterator fecPair = fecRingEnable_.begin() ; fecPair != fecRingEnable_.end() ; fecPair ++) {
      
    FecRingDevice *fec = fecPair->second ;
    fec->setInvertClockPolarity ( invertPolarity ) ;
  }

  invertClockPolarity_ = invertPolarity ; 
}

/** 
 * \param index - key for the FEC
 * \param invertPolarity - inverse or not the polarity
 * \warning any change in the CR0 will keep the value set
 */
void FecAccess::setInvertClockPolarity ( keyType index, bool invertPolarity ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Set the control register 0
  fec->setInvertClockPolarity ( invertPolarity ) ;
}

/** 
 * \param index - key for the FEC
 * \param internal - internal clock
 * \warning any change in the CR0 will keep the value set
 */
void FecAccess::setInternalClock ( keyType index, bool internal ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Set the control register 0
  fec->setInternalClock ( internal ) ;
}

/** Set the control register 0 of the FEC
 * <p>See also the method FecRingDevice::setFecCR0()
 * \param index - key for the FEC
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setFecRingCR0 ( keyType index, tscType16 value, bool force ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Set the control register 0
  fec->setFecRingCR0 ( value, force ) ;
}

/** Set the control register 0 of the FEC by read modify write operation
 * <p>See also the method FecRingDevice::setFecRingCR0()
 * \param index - key for the FEC
 * \param value - value to be written
 * \param op - operation to be performed (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 */
void FecAccess::setFecRingCR0 ( keyType index, 
				tscType16 value, 
				logicalOperationType op )
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Set the control register 0
  fec->setFecRingCR0 ( value, op ) ;
}

/** Get the control register 0 of the FEC for a specific FEC specified in the key
 * <p>See also the method FecRingDevice::getFecRingCR0()
 * \param index - key for the FEC
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType16 FecAccess::getFecRingCR0 ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Read the control register
  tscType16 regis = fec->getFecRingCR0 ( ) ;

  return (regis) ;
}

/** Set the control register 1 of the FEC
 * <p>See also the method FecRingDevice::setFecRingCR1()
 * \param index - key for the FEC
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setFecRingCR1 ( keyType index, tscType16 value ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Set the control register 1
  fec->setFecRingCR1 ( value ) ;
}

/** Set the control register 1 of the FEC
 * <p>See also the method FecRingDevice::setFecRingCR1()
 * \param index - key for the FEC
 * \param value - value to be written
 * \param op - operation to be performed (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 */
/*----------- This method has no sense, the CR1 CANNOT BE READ -----------
void FecAccess::setFecRingCR1 ( keyType index, 
				tscType16 value 
				logicalOperationType op ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Set the control register 1
  fec->setFecRingCR1 ( value, op ) ;
}
-------------------------------------------------------------------------- */

/** Get the control register 1 of the FEC for a specific FEC specified in the key
 * <p>See also the method FecRingDevice::getFecRingCR1()
 * \param index - key for the FEC
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType16 FecAccess::getFecRingCR1 ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Read the control register
  tscType16 regis = fec->getFecRingCR1 ( ) ;

  return (regis) ;
}

/** Get the status register 0 of the FEC for a specific FEC specified in the key
 * <p>See also the method FecRingDevice::getFecRingSR0()
 * \param index - key for the FEC
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType16 FecAccess::getFecRingSR0 ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Read the status register
  tscType16 regis = fec->getFecRingSR0 ( ) ;

  return (regis) ;
}

/** Get the status register 1 of the FEC for a specific FEC specified in the key
 * <p>See also the method FecRingDevice::getFecRingSR1()
 * \param index - key for the FEC
 * \return the value read
 * \exception FecExceptionHandler
 */
tscType16 FecAccess::getFecRingSR1 ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Read the status register
  tscType16 regis = fec->getFecRingSR1 ( ) ;

  return (regis) ;
}

/** Reset all FEC, rings declared in the map
 * <p>See also the method FecRingDevice::fecHardReset
 * \param index - FEC to be reseted
 * \exception FecExceptionHandler
 * \warning this method reload the mFEC firmware only
 */
void FecAccess::fecHardReset ( ) 
  throw (FecExceptionHandler) {

  FecExceptionHandler exOnLast ;

  if (fecRingEnable_.empty()) {
    
    // Exception
    RAISEFECEXCEPTIONHANDLER ( XDAQFEC_INVALIDOPERATION,
				"No FEC connected",
			       ERRORCODE ) ;
  }

  int fecVmeRing[FecVmeRingDevice::MAXVMEFECSLOT+1] = {0} ; // +1 to be sure
  for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {

    if (fecBusType_ == FECVME) {

      if (!fecVmeRing[(p->second)->getFecSlot()]) { // in the other case the reset have been done
	try {
#ifdef DEBUGMSGERROR
	  std::cout << "Reloading the mFEC firmware on slot " << (int)((p->second)->getFecSlot()) << std::endl ;
#endif
	  
	  ((FecVmeRingDevice *)p->second)->uFecsReload () ;
	  fecVmeRing[(p->second)->getFecSlot()] = 1 ;
	}
	catch (FecExceptionHandler &e) {
	  exOnLast = e ;
	}
      }
    }
    else {
      try {
	p->second->fecHardReset () ;
      }
      catch (FecExceptionHandler &e) {
	exOnLast = e ;
      }
    }
  }

  channelEnable_.clear() ;  
  deviceEnable_.clear() ;

  // raise the exception the last exception found
  if (exOnLast.getErrorCode() != 0) throw exOnLast ;
}

/** Reset all FEC, rings declared in the map
 * <p>See also the method FecRingDevice::fecHardReset
 * \param index - FEC to be reseted
 * \exception FecExceptionHandler
 * \warning this method reload in the plug and play the firmware and in mode not plug and play make a crate reset
 */
void FecAccess::crateReset ( ) throw (FecExceptionHandler) {

  if (fecBusType_ != FECVME) return ;

  if (fecRingEnable_.empty()) {
    
    // Exception
    RAISEFECEXCEPTIONHANDLER ( XDAQFEC_INVALIDOPERATION,
			       "No FEC connected",
			       ERRORCODE ) ;
  }

  // Not a loop
  // for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
  fecMapAccessedType::iterator p=fecRingEnable_.begin() ;

  if (p!=fecRingEnable_.end()) {
    // Reset the corresponding FEC or crate
    ((FecVmeRingDevice *)(p->second))->crateReset() ;
    // reload the firmware on all mFECs
    fecHardReset() ;
  }

  // Delete all the elements from the map
  channelEnable_.clear() ;  
  deviceEnable_.clear() ;

  // if the plug and play is set then re-create the plug and play
  if (vme64xCrate_ != NULL) {
    deleteVmeAccesses() ;
    configurePlugAndPlay(adapterSlot_, configurationFile_, mapFile_, strBusAdapter_) ;
  }
}

/** Reset the corresponding FEC
 * <p>See also the method FecRingDevice::fecHardReset
 * \param index - FEC to be reseted
 * \param value - only for VME FEC, please refer to the method FecVmeRingDevice::uFecsReload(value) (by default only the mFECs)
 * \exception FecExceptionHandler
 */
void FecAccess::fecHardReset ( keyType index, tscType8 value ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the FEC found => true means that the re-configuration will be done directly
  if ((fecBusType_ == FECVME) && (vme64xCrate_ == NULL) && (value != 0) ) 
    ((FecVmeRingDevice *)fec)->uFecsReload(value) ;
  else fec->fecHardReset ( ) ;
}


/** Reset all FEC, rings declared in the map
 * <p>See also the method FecRingDevice::fecRingReset
 * \param index - FEC to be reseted
 * \exception FecExceptionHandler
 */
void FecAccess::fecRingResetFSM ( ) 
  throw (FecExceptionHandler) {

  if (fecRingEnable_.empty()) {
    
    // Exception
    RAISEFECEXCEPTIONHANDLER ( XDAQFEC_INVALIDOPERATION,
				"No FEC connected",
			       ERRORCODE ) ;
  }

  for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
    
    // Reset the corresponding fec
    if (p->second != NULL) p->second->fecRingResetFSM () ;
  }

  // the reset of FSM does not touch the hardware part (CCU or devices)
  // Delete all the elements from the map
  //channelEnable_.clear() ;  
  //deviceEnable_.clear() ;
}

/** Reset the corresponding FEC
 * <p>See also the method FecRingDevice::fecRingReset
 * \param index - FEC to be reseted
 * \exception FecExceptionHandler
 */
void FecAccess::fecRingResetFSM ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the FEC found => true means that the re-configuration will be done directly 
  fec->fecRingResetFSM ( ) ;

  // Remove the elements from the map
  channelEnable_.clear() ;  
  deviceEnable_.clear() ;
}

/** Reset all FEC, rings declared in the map
 * <p>See also the method FecRingDevice::fecRingReset
 * \param index - FEC to be reseted
 * \exception FecExceptionHandler
 */
void FecAccess::fecRingReset ( ) 
  throw (FecExceptionHandler) {

  if (fecRingEnable_.empty()) {
    
    // Exception
    RAISEFECEXCEPTIONHANDLER ( XDAQFEC_INVALIDOPERATION,
				"No FEC connected",
			       ERRORCODE ) ;
  }

  for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
    
    // Reset the corresponding fec
    if (p->second != NULL) p->second->fecRingReset () ;
  }

  // Delete all the elements from the map
  channelEnable_.clear() ;  
  deviceEnable_.clear() ;
}

/** Reset the corresponding FEC
 * <p>See also the method FecRingDevice::fecRingReset
 * \param index - FEC to be reseted
 * \exception FecExceptionHandler
 */
void FecAccess::fecRingReset ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the FEC found => true means that the re-configuration will be done directly 
  fec->fecRingReset ( ) ;

  // Remove the elements from the map
  channelEnable_.clear() ;  
  deviceEnable_.clear() ;
}

/** Initialise the TTCRx
 * <p>See also the method FecRingDevice::setTTCRxInit
 * \exception FecExceptionHandler
 */
void FecAccess::initTTCRx ( ) 
  throw (FecExceptionHandler) {

  if (fecRingEnable_.empty()) {
    
    // Exception
    RAISEFECEXCEPTIONHANDLER ( XDAQFEC_INVALIDOPERATION,
				"No FEC connected",
			       ERRORCODE ) ;
  }

  for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
    
    // Reset the corresponding fec
    p->second->initTTCRx () ;
  }
}

/** Initialise the TTCRx
 * <p>See also the method FecRingDevice::setTTCRxInit
 * \param index - TTCRx reset
 * \exception FecExceptionHandler
 */
void FecAccess::initTTCRx ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the FEC found => true means that the re-configuration will be done directly 
  fec->initTTCRx ( ) ;
}

/** Disable or enable the receive for a FEC (redundancy)
 * \param index - index of the FEC
 * \exception FecExceptionHandler
 */
void FecAccess::setCR0Receive ( keyType index, bool enable ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the FEC found => true means that the re-configuration will be done directly 
  fec->setCR0Receive ( enable ) ;
}

/** Reset all FEC, rings declared in the map
 * <p>See also the method FecRingDevice::fecRingReset
 * \param index - FEC to be reseted
 * \exception FecExceptionHandler
 */
void FecAccess::fecRingResetB ( ) 
  throw (FecExceptionHandler) {

  if (fecRingEnable_.empty()) {
    
    // Exception
    RAISEFECEXCEPTIONHANDLER ( XDAQFEC_INVALIDOPERATION,
				"No FEC connected",
			       ERRORCODE ) ;
  }

  for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end();p++) {
    
    // Reset the corresponding fec
    if (p->second != NULL) p->second->fecRingResetB () ;
  }

  // Delete all the elements from the map
  channelEnable_.clear() ;  
  deviceEnable_.clear() ;
}

/** Reset the corresponding FEC on ring
 * <p>See also the method FecRingDevice::fecRingResetB
 * \param index - FEC, ring to be reseted
 * \exception FecExceptionHandler
 * \warning this method must be used once you lost the ring A. It send a reset to CCU
 * through ring B and all CCU will be reseted
 */
void FecAccess::fecRingResetB ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the FEC found => true means that the re-configuration will be done directly
  fec->fecRingResetB ( ) ;

  // Remove the elements from the map
  channelEnable_.clear() ;  
  deviceEnable_.clear() ;
}

/** Regenerate the token on the ring
 * <p>See also the method FecRingDevice::FecRingRelease
 * \param index - FEC, ring to be reseted
 * \exception FecExceptionHandler
 * \warning this method must be used once you lost the link initialised
 */
void FecAccess::fecRingRelease ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the FEC found => true means that the re-configuration will be done directly
  fec->fecRingRelease ( ) ;
}

/** This method reconfigure the ring due to a CCU fault.
 * <p>See also the method FecRingDevice::fecRingReconfigure()
 * \param index - ring to be reconfigure
 * \return the status of the SR0 for the given ring
 * \exception FecExceptionHandler
 */
tscType16 FecAccess::fecRingReconfigure ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reconfigure the ring
  tscType16 fecSR0 = fec->fecRingReconfigure ( ) ;

  // Clear all the table (reset hardware)
  channelEnable_.clear() ;  
  deviceEnable_.clear() ;

  return fecSR0 ;
}

/** This method reconfigure the ring due to a CCU fault.
 * <p>See also the method FecRingDevice::fecRingReconfigure()
 * \param index - ring to be reconfigure
 * \return the status of the SR0 for the given ring
 * \exception FecExceptionHandler
 */
tscType16 FecAccess::fecRingReconfigure ( keyType index, TkRingDescription &tkRing ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reconfigure the ring
  tscType16 fecSR0 = fec->fecRingReconfigure ( tkRing ) ;

  // Clear all the table (reset hardware)
  channelEnable_.clear() ;  
  deviceEnable_.clear() ;

  return fecSR0 ;
}

/**
 * \param index - key of the given CCU
 * \return std::list<tscType32> with in order:
 * <ul>
 * <li> Key(FEC,RING) | { SR0 SR1 CR0 CR1 }
 * </ul>
 * \warning a scan ring or accesses must be done before any check
 * \warning the list must be deleted when you have finished with it
 * \warning in case of error the value put is CCU address | 0xFFFF
 */
FecRingRegisters FecAccess::getFecRingRegisters ( keyType index )
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  return (fec->getFecRingRegisters(index)) ;
}

// -------------------------------------------------------------------------------------
//
//                                IRQ methods, block transfer mode
//
// -------------------------------------------------------------------------------------

/** Enable or disable the irq on the given FEC (PCI or VME)
 * \param index - index of the FEC to be enabled or disabled
 * \param enable - enable or disable the irq
 * \param vmeLevel - level to be raised [option, default 1]
 * \bug the vector calculation must be done in this method and a method must be registered to be fired. This method is not yet implemented
 * \warning no exception is raised if the FEC is an USB FEC (without IRQ)
 */
void FecAccess::setIRQ ( keyType index, bool enable, tscType8 vmeLevel ) throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;
  switch (fecBusType_) {
  case FECPCI:
    // retreive the corresponding FEC PCI ring device
    fec->setIRQ ( enable ) ;
    break ;
  case FECVME:
    // retreive the corresponding FEC VME ring device
    fec->setIRQ ( enable, vmeLevel ) ;
    break ;
  case FECUSB:
    // Not IRQ in USB FEC === fec->setIRQ ( enable ) ;
    break ;
  }
}

/** Enable or disable the irq on the FEC
 * \param enable - enable or disable the irq
 * \param vmeLevel - level to be raised [option, default 1]
 * \bug the vector calculation must be done in this method and a method must be registered to be fired. This method is not yet implemented
 * \warning no exception is raised if the FEC is an USB FEC (without IRQ)
 */
void FecAccess::setIRQ ( bool enable, tscType8 vmeLevel ) throw (FecExceptionHandler) {

  // No IRQ for the FECUSB
  if (fecBusType_ == FECUSB) return ;

  // PCI or VME FEC
  std::list<keyType> *fecList = getFecList ( ) ;

  if ( (fecList != NULL) && (fecList->size() > 0) ) {

    for (std::list<keyType>::iterator p = fecList->begin() ; p != fecList->end() ; p ++) {

      keyType index = *p ;

      // Retreive the corresponding FecRingDevice
      FecRingDevice *fec = getFecRingDevice ( index ) ;
      switch (fecBusType_) {
      case FECPCI:
	// retreive the corresponding FEC PCI ring device
	fec->setIRQ ( enable ) ;
	break ;
      case FECVME:
	// retreive the corresponding FEC VME ring device
	fec->setIRQ ( enable, vmeLevel ) ;
	break ;
      case FECUSB:
	// Not IRQ in USB FEC === fec->setIRQ ( enable ) ;
	break ;
      }
    }
  }

  delete fecList ;
}

// -------------------------------------------------------------------------------------
//
//                                CCU methods
//
// -------------------------------------------------------------------------------------

/** Method in order to retreive status register A from the CCU
 * <p>See also the method FecRingDevice::getCcuSRA()
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType8 FecAccess::getCcuSRA ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuSRA ( index )) ;  
}

/** Method in order to retreive status register B from the CCU
 * <p>See also the method FecRingDevice::getCcuSRB
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType8 FecAccess::getCcuSRB ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuSRB ( index )) ;  
}

/** Method in order to retreive status register C from the CCU
 * <p>See also the method FecRingDevice::getCcuSRC()
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType8 FecAccess::getCcuSRC ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuSRC ( index )) ;  
}

/** Method in order to retreive status register D from the CCU
 * <p>See also the method FecRingDevice::getCcuSRD
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType8 FecAccess::getCcuSRD ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuSRD ( index )) ;  
}

/** Method in order to retreive status register E from the CCU
 * <p>See also the method FecRingDevice::getCcuSRE()
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType32 FecAccess::getCcuSRE ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuSRE ( index )) ;  
}

/** Method in order to retreive status register F from the CCU
 * <p>See also the method FecRingDevice::getCcuSRF
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType16 FecAccess::getCcuSRF ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuSRF ( index )) ;  
}

/** Method in order to retreive status register G from the CCU (only for the old CCU)
 * <p>See also the method FecRingDevice::getCcuSRG()
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType8 FecAccess::getCcuSRG ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuSRG ( index )) ;  
}

/** Method in order to retreive status register H from the CCU (only for the old CCU)
 * <p>See also the method FecRingDevice::getCcuSRH
 * \param index - key of the corresponding CCU
 * \param registerValue - register to be accessed (command for the CCU)
 * \return value read
 * \exception FecExceptionHandlerception
 */
tscType8 FecAccess::getCcuSRH ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuSRH ( index )) ;  
}

/** Method in order to set CCU control register A
 * <p>See also the method FecRingDevice::setCcuCRA()
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setCcuCRA ( keyType index, tscType8 value ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  fec->setCcuCRA ( index, value ) ;
}

/** Method in order to retreive control register A from the CCU
 * <p>See also the method FecRingDevice::getCcuCRA
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::getCcuCRA ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuCRA ( index )) ;  
}

/** Method in order to set CCU control register B
 * <p>See also the method FecRingDevice::setCcuCRB()
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setCcuCRB ( keyType index, tscType8 value ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  fec->setCcuCRB ( index, value ) ;
}

/** Method in order to retreive control register B from the CCU
 * <p>See also the method FecRingDevice::getCcuCRB
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::getCcuCRB ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuCRB ( index )) ;  
}

/** Method in order to set CCU control register C
 * <p>See also the method FecRingDevice::setCcuCRC()
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setCcuCRC ( keyType index, tscType8 value ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  fec->setCcuCRC ( index, value ) ;
}

/** Method in order to retreive control register C from the CCU
 * <p>See also the method FecRingDevice::getCcuCRC()
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType8 FecAccess::getCcuCRC ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuCRC ( index )) ;  
}

/** Method in order to set CCU control register D
 * <p>See also the method FecRingDevice::setCcuCRD()
 * \param index - key of the corresponding CCU
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setCcuCRD ( keyType index, tscType8 value ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  fec->setCcuCRD ( index, value ) ;
}

/** Method in order to retreive control register D from the CCU
 * <p>See also the method FecRingDevice::getCcuCRD()
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType8 FecAccess::getCcuCRD ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuCRD ( index )) ;  
}

/** Method in order to set CCU control register E (only for CCU 25). 
 * <p>See also the method FecRingDevice::setCcuCRE()
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
void FecAccess::setCcuCRE ( keyType index, tscType24 value ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  fec->setCcuCRE ( index, value ) ;
}

/** Method in order to set CCU control register E (only for CCU 25).
 * <p>See also the method FecRingDevice::setCcuCRE()
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
 * \exception FecExceptionHandler
 */
void FecAccess::setCcuCRE ( keyType index, 
                            tscType8 valueI2C, 
                            tscType8 valuePIA,
                            tscType8 valueMemory,
                            tscType8 valueTrigger,
                            tscType8 valueJTAG) 
  throw ( FecExceptionHandler ) {

  setCcuCRE ( index, 
              (valueI2C & 0xFF) | 
              ((valuePIA & 0x7) << 16)|
              ((valueMemory & 0x1) << 20)| 
              ((valueTrigger & 0x1) << 21) |
              ((valueJTAG & 0x1) << 22) ) ;
}

/** Method in order to retreive control register E from the CCU (only for CCU 25)
 * <p>See also the method FecRingDevice::getCcuCRE()
 * \param index - key of the corresponding CCU
 * \return value read
 * \exception FecExceptionHandler
 */
tscType24 FecAccess::getCcuCRE ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuCRE ( index )) ;  
}

/** Check if a channel is enabled
 * \param index of the FEC, ring, CCU and channels requested 
 * \return true if the channel is enabled, false if not
 * \exception FecExceptionHandler
 */
bool FecAccess::isChannelEnabled ( keyType index )  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->isChannelEnabled ( index )) ;  
}

/** scan for CCUs on each FEC, ring
 * <p>See also the method FecRingDevice::getCcuList()
 * \param index - index of the corresponding FEC, ring
 * \param noBroadcast - CCU broadcast mode used (false) or not (true) [OPTION parameter]
 * \return a list of keyType (must be delete after)
 * \warning this method add all the FECs to the FEC map
 * \warning the list must be deleted when you have finished with it
 * \warning this method is not recommanded due to the fact that
 * several FecSupervisor can managed the same PCI/VME bus
 */
std::list<keyType> *FecAccess::getCcuList ( keyType index, bool noBroadcast, bool scan ) 
  throw (FecExceptionHandler) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // CCU Access
  return (fec->getCcuList ( noBroadcast, scan )) ;
}

// -------------------------------------------------------------------------------------
//
//                                CCU Alarms
//
// -------------------------------------------------------------------------------------
/** 
 * \param index - index of the CCU (FEC/RING/CCU). If channel key is equal
 * to 0 then all PIA channels will be enabled.
 * <p>See also the method FecRingDevice::setCcuAlarmsEnable()
 * \param enable1 - enable CCU alarm 1
 * \param enable2 - enable CCU alarm 2
 * \param enable3 - enable CCU alarm 3
 * \param enable4 - enable CCU alarm 4
 */
void FecAccess::setCcuAlarmsEnable ( keyType index, bool enable1, bool enable2, bool enable3, bool enable4 ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Enable alarms
  fec->setCcuAlarmsEnable (index, enable1, enable2, enable3, enable4) ;
}

/** 
 * <p>See also the method FecRingDevice::waitForAnyCcuAlarms()
 * \param index - index of the CCU (FEC/RING/CCU)
 * \param frame - frame received by the warning 
 * \warning This method is a blocked command no interruption can be done
 * \warning The size of the frame must be DD_USER_MAX_MSG_LENGTH
 */ 
void FecAccess::waitForAnyCcuAlarms ( keyType index, tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Wait for any CCU alarms
  fec->waitForAnyCcuAlarms ( frame ) ;
}

/** 
 * <p>See also the method FecRingDevice::setPiaClearInterrupts()
 * \param index - index of the CCU (FEC/RING/CCU/PIA channels). If channel key is equal
 * to 0 then all PIA channels will be enabled.
 */
void FecAccess::setPiaInterruptEnable ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Wait for any CCU alarms
  fec->setPiaClearInterrupts ( index ) ;
}

/** 
 * <p>See also the method FecRingDevice::setPiaClearInterrupts()
 * \param index - index of the CCU (FEC/RING/CCU/PIA channels). If channel key is equal
 * to 0 then all PIA channels will be enabled.
 */
void FecAccess::setPiaClearInterrupts ( keyType index ) 
  throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Wait for any CCU alarms
  fec->setPiaClearInterrupts ( index ) ;
}

// -------------------------------------------------------------------------------------
//
//                                Channels methods
//
// -------------------------------------------------------------------------------------

/** Enable a channel corresponding to the device
 * \param index - index of the channel (device)
 * \param enable - enable or disable the channel (bool)
 * \exception FecExceptionHandler
 */
void FecAccess::setChannelEnable ( keyType index, bool enable ) throw ( FecExceptionHandler ) {

  // Retreive the access
  i2cAccess *i2c = (i2cAccess *)checkIndex (index) ;

  // Enable the channel
  i2c->setChannelEnable (enable) ;
  i2c->setChannelInit () ;
}

// -------------------------------------------------------------------------------------
//
//                                PIA Channels
//
// -------------------------------------------------------------------------------------

/** Add an access to a device on an PIA channel. Make an piaAccess object
 * corresponding to the PIA channel that will be accessed and store it in the
 * device hash table.
 * \param fecSlot - fec number
 * \param ringSlot - ring number
 * \param ccuAddress - CCU address
 * \param piaChannel - PIA channel
 * \param accessMask - access type (read, write, both)
 * \return an index on the map that must use in future access
 * \exception FecExceptionHandler
 */
keyType FecAccess::addPiaAccess ( tscType8 fecSlot,
                                  tscType8 ringSlot,
                                  tscType8 ccuAddress,
                                  tscType8 piaChannel,
                                  enumAccessModeType accessMask ) throw ( FecExceptionHandler ) {


  return (addPiaAccess ( buildCompleteKey ( fecSlot, ringSlot, ccuAddress, piaChannel, PIAADDRESSNUMBER ), accessMask)) ;

}

/** Add an access to a device on an PIA channel. Make an piaAccess object
 * corresponding to the PIA channel that will be accessed and store it in the
 * device hash table.
 * \param index - index of the device (contain all the path to access the device)
 * \param accessMask - access type (read, write, both)
 * \return an index on the map that must use in future access
 * \exception FecExceptionHandler:
 * <ul>
 * <li>XDAQFEC_INVALIDOPERATION
 * </ul>
 */
keyType FecAccess::addPiaAccess ( keyType index,
                                  enumAccessModeType accessMask ) 
  throw (FecExceptionHandler) {

  // Find or create the corresponding FecDevice
  FecRingDevice *fec = setFecRingDevice ( index ) ;
  if (fec == NULL) {
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"No FEC connected",
				ERRORCODE,
				index) ;
  }

  fec->addNewAccess() ;

  // Check if the parameters are ok else an exception is returned
  if (index != 0) {

    // Check if the device can be accessed via the map deviceEnable_
    if (deviceEnable_.find(index) == deviceEnable_.end()) {

      // Create the pia access
      piaAccess *pia = new piaAccess ( index, accessMask, fec ) ;
      index = pia->getKey ( ) ;

      // Add the device key in the map
      deviceEnable_[index] = pia ;

      // Build the key for the channel
      keyType keyChannel = buildChannelKey (index) ;

      // Check if the i2c channel corresponding to FEC:RING:CCU:CHANNEL is enable
      channelMapAccessedType::iterator channel = channelEnable_.find(keyChannel) ;
      if (channel == channelEnable_.end()) {

        // Enable the i2c channel channel
        pia->setChannelEnable(true) ;

        // Set the GCR, fix also the duration of the signal
        pia->setChannelInit ( ) ;

        // Add the channel key in the channel map
        channelEnable_[keyChannel] = 1 ;

      } 
      else {
        // Add a new access
        channel->second += 1 ;
      }
    }
    else {

      // Exception
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device is already connected by another thread",
				  ERRORCODE,
				  index ) ;
    }
  }
  else {

    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"index is not correct (equals to 0)",
				ERRORCODE,
				index ) ;
  }

  return (index) ;
}

/** Remove an index from device hash table and check if the
 * channel must be disable (disable it if necessary).
 * \param index - key of corresponding device
 * \exception FecExceptionHandler:
 * <ul>
 * <li>XDAQFEC_INVALIDOPERATION
 * </ul>
 */
void FecAccess::removePiaAccess ( keyType index ) 
  throw (FecExceptionHandler) {

  // Retreive the information in the map both device and channel
  keyType keyChannel = buildChannelKey (index) ;
  channelMapAccessedType::iterator channel = channelEnable_.find(keyChannel) ;

  deviceMapAccessedType::iterator device = deviceEnable_.find(index) ;

  // The device exists ?
  if (device == deviceEnable_.end()) {

    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"device is not connected, please use add method in order to connect the device and/or the channel",
				ERRORCODE,
				index ) ;
  }

  piaAccess *pia = (piaAccess *)device->second ;

  if ( (channel != channelEnable_.end()) && (device != deviceEnable_.end()) ) {

    // Check if the channel must be disable 
    if (channel->second == 0) {
      
      // Disable the channel
      pia->setChannelEnable (false) ;

      // remove the keyChannel from the device
      channelEnable_.erase (keyChannel) ;
    }
    else {

      // Remove one access from the channel
      channel->second -= 1 ;
    }

    // remove the access number correspoding to the FecDevice
    FecRingDevice *fec = getFecRingDevice ( index ) ;
    fec->removeAccess ( ) ;

    // Destroy the corresponding device
    delete (pia) ;
    deviceEnable_.erase (index) ; // Remove the device from the map
  }
  else {
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"channel is not connected, please use add method in order to connect the device and/or the channel",
				ERRORCODE,
				index ) ;
  }
}

/** Method in order to reset a PIA channel
 * \param index - key of the corresponding channel
 * \exception FecExceptionHandler
 */
void FecAccess::piaChannelReset ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the channel
  fec->piaChannelReset ( index ) ;
}

/** Method in order to set PIA general control register 
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setPiaChannelGCR ( keyType index, tscType8 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->setPiaChannelGCR ( index, value ) ;
}

/** Method in order to set PIA general control register by read modify write operation
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \param op - operation to be done (TSC_FECSERVER_OR, TSC_FECSERVER_AND, TSC_FECSERVER_XOR, TSC_FECSERVER_EQUAL)
 * \exception FecExceptionHandler
 */
void FecAccess::setPiaChannelGCR ( keyType index, tscType8 value, logicalOperationType op ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->setPiaChannelGCR ( index, value, op ) ;
}

/** Method in order to retreive PIA general control register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */   
tscType8 FecAccess::getPiaChannelGCR ( keyType index ) throw ( FecExceptionHandler ) {
 
  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->getPiaChannelGCR ( index )) ;
}

/** Method in order to retreive PIA status register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::getPiaChannelStatus ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->getPiaChannelStatus ( index )) ;
}

/** Method in order to set PIA Data Direction Register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setPiaChannelDDR ( keyType index, tscType8 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->setPiaChannelDDR ( index, value ) ;
}

/**  Method in order to set PIA Data Direction Register by read modify write operation
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR, CMD_EQUAL)
 * \exception FecExceptionHandler
 */
void FecAccess::setPiaChannelDDR ( keyType index, tscType8 value, logicalOperationType op ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->setPiaChannelDDR ( index, value, op ) ;
}

/** Method in order to retreive PIA Data Direction Register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::getPiaChannelDDR ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->getPiaChannelDDR ( index )) ;
}

/** Method in order to set PIA data register 
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setPiaChannelDataReg ( keyType index, tscType8 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->setPiaChannelDataReg ( index, value ) ;
}

/** Method in order to set PIA Data Register by read modify write operation
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \param op - operation to be done (TSC_FECSERVER_OR, TSC_FECSERVER_AND, TSC_FECSERVER_XOR, TSC_FECSERVER_EQUAL)
 * \exception FecExceptionHandler
 */
void FecAccess::setPiaChannelDataReg ( keyType index, tscType8 value, logicalOperationType op ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->setPiaChannelDataReg ( index, value, op ) ;
}

/** Method in order to retreive PIA data register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */   
tscType8 FecAccess::getPiaChannelDataReg ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->getPiaChannelDataReg ( index )) ;
}

// -------------------------------------------------------------------------------------
//
//                                I2C channels
//
// -------------------------------------------------------------------------------------

/** Add an access to a device on an I2C channel. Make an i2cAccess object
 * corresponding to the I2C channel that will be accessed and store it in the
 * device hash table.
 * \param fecSlot - fec number
 * \param ringSlot - ring number
 * \param ccuAddress - CCU address
 * \param i2cChannel - I2C channel
 * \param i2cAddress - I2C address
 * \param deviceType - type of the device:
 * <ul>
 * <li>DCU, LASERDRIVER (4.2), PLL: single byte normal mode where a register address is <base address> + <offset>
 * <li>PHILIPS: single byte normal mode where a register address is <base address> (only one register)
 * <li>APV, APVMUX: single byte ral mode where a register address is <base address> and an offset is performed when a command is called
 * <li>LASERDRIVER (old version): single byte normal mode where a register address is <base address> shifted by the value predifined with offset.
 * <li>FOREXTENDEDMODE (like the ECAL-FENIX): single byte extended mode where a register address is the <base address bits [7:0]> with an offset <base address bits [15:8]>. This mode must be improved to find a correct solution, the keyType have only 8 bits for the device address. 
 * </ul>
 * \param accessMask - access mode
 * \return an index on the map that must use in future access
 * \exception FecExceptionHandler
 */
keyType FecAccess::addi2cAccess ( tscType8 fecSlot,
                                  tscType8 ringSlot,
                                  tscType8 ccuAddress,
                                  tscType8 i2cChannel,
                                  tscType8 i2cAddress,
                                  enumDeviceType deviceType,
                                  enumAccessModeType accessMask ) throw ( FecExceptionHandler ) {

  return (addi2cAccess (buildCompleteKey ( fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress ), deviceType, accessMask)) ;

}

/** Add an access to a device on an I2C channel. Make an i2cAccess object
 * corresponding to the I2C channel that will be accessed and store it in the
 * device hash table.
 * \param index - define the path of the device
 * \param deviceType - type of the device (pll, apv, ...)
 * \param accessMask - access mode
 * \return an index on the map that must use in future access
 * \exception FecExceptionHandler:
 * <ul>
 * <li>XDAQFEC_INVALIDOPERATION
 * </ul>
 */
keyType FecAccess::addi2cAccess ( keyType index, 
                                  enumDeviceType deviceType,
                                  enumAccessModeType accessMask ) 
  throw (FecExceptionHandler) {

#ifdef DEBUGMSGERROR
  char msg2000[80] ;
  decodeKey(msg2000,index) ;
  std::cout << "FecAccess::addi2cAccess: Add i2c access for device " << msg2000 << std::endl ;
#endif

  // Check the different values and reject if not correct
  // Find or create the corresponding FecDevice
  FecRingDevice *fec = setFecRingDevice ( index ) ;
  if (fec == NULL) {
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"FEC slot does not exists",
				ERRORCODE,
				index ) ;
  }
  fec->addNewAccess() ;

  // Check if the parameters are ok else an exception is returned
  char msg[1000] ;
  decodeKey (msg,index) ;
 
  if (index != 0) {

    // Check if the device can be accessed via the map deviceEnable_
    if (deviceEnable_.find(index) == deviceEnable_.end()) {

      // Create the i2c access
      i2cAccess *i2c = new i2cAccess ( index, deviceType, accessMask, fec) ;

      // Add the device key in the map
      deviceEnable_[index] = i2c ;

      // Build the key for the channel 
      keyType keyChannel = buildChannelKey (index) ;

      // Check if the i2c channel corresponding to fecSlot:ring:ccu:channel is enable
      channelMapAccessedType::iterator channel = channelEnable_.find(keyChannel) ;

      if (channel == channelEnable_.end()) {

        // Enable the i2c channel channel 
        i2c->setChannelEnable(true) ;

        // The CRA must be set for the channel {CCU,FEC,0x4,Channel,0,0xF0,0xC0}
        // and set the bit force ack if needed
        i2c->setChannelInit ( forceChannelAck_, i2cSpeed_ ) ;

        // Add the channel key in the channel map
        channelEnable_[keyChannel] = 1 ;

      } 
      else {
        // Add a new access
        channel->second += 1 ;
      }
    } 
    else {
      // Exception
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device is already connected by another thread",
				  ERRORCODE,
				  index ) ;
    }
  }
  else {
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"index is not correct (equals to 0)",
				ERRORCODE,
				index ) ;
  }

  return (index) ;
}

/** Remove an index from device hash table and check if the
 * channel must be disable (disable it if necessary).
 * \param index - key of corresponding device
 * \exception FecExceptionHandler
 */
void FecAccess::removei2cAccess ( keyType index ) throw ( FecExceptionHandler ) {

#ifdef DEBUGMSGERROR
  char msg2000[80] ;
  decodeKey(msg2000,index) ;
  std::cout << "Delete i2c access for device " << msg2000 << std::endl ;
#endif

  // Retreive the information in the map both device and channel
  keyType keyChannel = buildChannelKey (index) ;
  channelMapAccessedType::iterator channel = channelEnable_.find(keyChannel) ;

  deviceMapAccessedType::iterator device = deviceEnable_.find(index) ;

  // The device exists ?
  if ( (device == deviceEnable_.end())   || 
       (channel == channelEnable_.end()) || 
       (device->second == NULL) ) {
    //|| (channel->second == NULL) ) { => generate a warning

    if ( (device != deviceEnable_.end()) && (device->second != NULL) ) {
      // Delete
      i2cAccess *i2c = (i2cAccess *)device->second ;
      // Disable the channel
      i2c->setChannelEnable (false) ;
    }

    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"device is not connected, please use add method in order to connect the device and/or the channel",
				ERRORCODE,
				index ) ;
  }

  i2cAccess *i2c = (i2cAccess *)device->second ;

  // Check if the channel must be disable
  if (channel->second == 0) {

    // Disable the channel
    i2c->setChannelEnable (false) ;

    // remove the keyChannel from the device
    channelEnable_.erase (keyChannel) ;
  }
  else {

    // Remove one access from the channel
    channel->second -= 1 ;
  }

  // remove the access number correspoding to the FecDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;
  fec->removeAccess ( ) ;

  // Destroy the corresponding device
  delete device->second ;
  deviceEnable_.erase (index) ; // Remove the device from the map
}

/** Method in order to set I2C control register A
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::seti2cChannelCRA ( keyType index, tscType8 value) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->seti2cChannelCRA ( index, value ) ;
}

/** Method in order to retreive I2C control register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::geti2cChannelCRA ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->geti2cChannelCRA ( index )) ;
}

/** Method in order to retreive I2C status register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::geti2cChannelSRA ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->geti2cChannelSRA ( index )) ;
}

/** Method in order to retreive I2C status register B from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::geti2cChannelSRB ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->geti2cChannelSRB ( index )) ;
}

/** Method in order to retreive I2C status register C from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::geti2cChannelSRC ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->geti2cChannelSRC ( index )) ;
}

/** Method in order to retreive I2C status register D from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::geti2cChannelSRD ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->geti2cChannelSRD ( index )) ;
}

/** Method in order to reset an I2C channel
 * \param index - key of the corresponding channel
 * \exception FecExceptionHandler
 */
void FecAccess::i2cChannelReset ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the channel
  fec->i2cChannelReset ( index ) ;
}

/** scan the ring for each FEC and create a vector associated
 * for all the tracker devices (I2C bus)
 * \param device - array of device address with device[0] = device address
 * and device[1] = mode (NORMALMODE, EXTENDEDMODE, RALMODE)
 * \param size - number of devices
 * \param noBroadcast - use (false) or not (true) the CCU broadcast
 * \param display - display a message for each channel scanned
 * \return a list of keyType
 * \exception FecExceptionHandler
 * \warning this method is dedicated to the tracker i2c devices but can
 * be adapted for all the FEC devices
 * \warning this method is dedicated to the CCU 25 but can work for old CCUs
 */
std::list<keyType> *FecAccess::scanRingForI2CDevice ( keyType  *deviceValues, 
						      tscType32 sizeDevices,
						      bool noBroadcast,
						      bool display)
  throw (FecExceptionHandler) {

  std::list<keyType> *listDevice = NULL ;

  // For each FEC
  for (fecMapAccessedType::iterator p=fecRingEnable_.begin();p!=fecRingEnable_.end(); p++) {  

    FecRingDevice *fec = p->second ;

#ifdef DEBUGERRORMSG
    std::cout << "Check FEC " << (int)fec->getFecSlot() << " Ring " << (int)fec->getRingSlot() << std::endl ;
#endif

    try {

      std::list<keyType> *devs = fec->scanRingForI2CDevice ( deviceValues, sizeDevices, noBroadcast, display ) ;

      if (devs != NULL) {
      
	if (listDevice == NULL) listDevice = new std::list<keyType> ;
	listDevice->merge ( *devs ) ;
      }
      
      // Delete the list use for the merge
      delete devs ;
    }
    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
      std::cerr << "FecAccess::scanRingForI2CDevice " << e.what() << std::endl ;
#endif
    }
  }

  if (display && (listDevice != NULL)) std::cout << std::endl ;

  return (listDevice) ;
}

/** scan a given ring and create a vector associated
 * for all the tracker devices (I2C bus)
 * \param index of the ring
 * \param device - array of device address with device[0] = device address
 * and device[1] = mode (NORMALMODE, EXTENDEDMODE, RALMODE)
 * \param size - number of devices
 * \param noBroadcast - use (false) or not (true) the CCU broadcast
 * \param display - display a message for each channel scanned
 * \return a list of keyType
 * \exception FecExceptionHandler
 * \warning this method is dedicated to the tracker i2c devices but can
 * be adapted for all the FEC devices
 * \warning this method is dedicated to the CCU 25 but can work for old CCUs
 */
std::list<keyType> *FecAccess::scanRingForI2CDevice ( keyType index,
						      keyType  *deviceValues, 
						      tscType32 sizeDevices,
						      bool noBroadcast,
						      bool display)
  throw (FecExceptionHandler) {

  std::list<keyType> *listDevice = NULL ;

  // Retreive the ring
  FecRingDevice *fec = getFecRingDevice (index) ;
  
#ifdef DEBUGERRORMSG
  std::cout << "Scaning for devices: Check FEC " << (int)fec->getFecSlot() << " Ring " << (int)fec->getRingSlot() << std::endl ;
#endif

  try {
    
    std::list<keyType> *devs = fec->scanRingForI2CDevice ( deviceValues, sizeDevices, noBroadcast, display ) ;
    
    if (devs != NULL) {
      
      if (listDevice == NULL) listDevice = new std::list<keyType> ;
      listDevice->merge ( *devs ) ;
    }
    
    // Delete the list use for the merge
    delete devs ;
  }
  catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
    std::cerr << "FecAccess::scanRingForI2CDevice " << e.what() << std::endl ;
#endif
  }

  return (listDevice) ;
}

// -------------------------------------------------------------------------------------
//
//                                Memory Channel
//
// -------------------------------------------------------------------------------------

/** Add an access to a device on an memory channel. Make an memoryAccess object
 * corresponding to the memory channel that will be accessed and store it in the
 * device hash table.
 * \param fecSlot - fec number
 * \param ringSlot - ring number
 * \param ccuAddress - CCU address
 * \param piaChannel - PIA channel
 * \param accessMask - access type (read, write, both)
 * \return an index on the map that must use in future access
 * \exception FecExceptionHandler
 */
keyType FecAccess::addMemoryAccess ( tscType8 fecSlot,
                                     tscType8 ringSlot,
                                     tscType8 ccuAddress,
                                     tscType8 memoryChannel,
                                     enumAccessModeType accessMask ) throw ( FecExceptionHandler ) {


  return (addMemoryAccess ( buildCompleteKey ( fecSlot, ringSlot, ccuAddress, memoryChannel, MEMORYADDRESSNUMBER ), accessMask)) ;

}

/** Add an access to a device on a memory channel. Make an memoryAccess object
 * corresponding to the memory channel that will be accessed and store it in the
 * device hash table.
 * \param index - index of the device (contain all the path to access the device)
 * \param accessMask - access type (read, write, both)
 * \return an index on the map that must use in future access
 * \exception FecExceptionHandler:
 * <ul>
 * <li>XDAQFEC_INVALIDOPERATION
 * </ul>
 */
keyType FecAccess::addMemoryAccess ( keyType index,
                                     enumAccessModeType accessMask ) 
  throw (FecExceptionHandler) {

  // Check the different values and reject if not correct
  
  // Find or create the corresponding FecDevice
  FecRingDevice *fec = setFecRingDevice ( index ) ;
  if (fec == NULL) {
    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"FEC slot does not exists",
				ERRORCODE,
				index ) ;
  }
  fec->addNewAccess() ;

  // Check if the parameters are ok else an exception is returned
  if (index != 0) {

    // Check if the device can be accessed via the map deviceEnable_
    if (deviceEnable_.find(index) == deviceEnable_.end()) {

      // Create the memory access
      memoryAccess *memory = new memoryAccess ( index, accessMask, fec ) ;
      index = memory->getKey ( ) ;

      // Add the device key in the map
      deviceEnable_[index] = memory ;

      // Build the key for the channel 
      keyType keyChannel = buildChannelKey (index) ;

      // Check if the i2c channel corresponding to FEC:RING:CCU:CHANNEL is enable
      channelMapAccessedType::iterator channel = channelEnable_.find(keyChannel) ;
      if (channel == channelEnable_.end()) {

        // Enable the i2c channel channel */
        memory->setChannelEnable(true) ;

        // Set the GCR, fix also the duration of the signal
        memory->setChannelInit ( ) ;

        // Add the channel key in the channel map
        channelEnable_[keyChannel] = 1 ;

      } 
      else {
        // Add a new access
        channel->second += 1 ;
      }
    }
    else {

      // Exception
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				  "device is already connected by another thread",
				  ERRORCODE,
				  index ) ;
    }
  }

  return (index) ;
}

/** Remove an index from device hash table and check if the
 * channel must be disable (disable it if necessary).
 * \param index - key of corresponding device
 * \exception FecExceptionHandler:
 * <ul>
 * <li>XDAQFEC_INVALIDOPERATION
 * </ul>
 */
void FecAccess::removeMemoryAccess ( keyType index ) 
  throw (FecExceptionHandler) {

  /** Retreive the information in the map both device and channel */
  keyType keyChannel = buildChannelKey (index) ;
  channelMapAccessedType::iterator channel = channelEnable_.find(keyChannel) ;

  deviceMapAccessedType::iterator device = deviceEnable_.find(index) ;

  // The device exists ?
  if (device == deviceEnable_.end()) {

    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"device is not connected, please use add method in order to connect the device and/or the channel",
				ERRORCODE,
				index ) ;
  }

  memoryAccess *memory = (memoryAccess *)device->second ;

  if ( (channel != channelEnable_.end()) && (device != deviceEnable_.end()) ) {

    // Check if the channel must be disable 
    if (channel->second == 0) {
      
      // Disable the channel
      memory->setChannelEnable (false) ;

      // remove the keyChannel from the device
      channelEnable_.erase (keyChannel) ;
    }
    else {

      // Remove one access from the channel
      channel->second -= 1 ;
    }

    // remove the access number correspoding to the FecDevice
    FecRingDevice *fec = getFecRingDevice ( index ) ;
    fec->removeAccess ( ) ;

    // Destroy the corresponding device
    delete (memory) ;
    deviceEnable_.erase (index) ; // Remove the device from the map
  } 
  else {

    // Exception
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
				"channel is not connected, please use add method in order to connect the device and/or the channel",
				ERRORCODE,
				index ) ;
  }
}

/** Method in order to reset a memory channel
 * \param index - key of the corresponding channel
 * \exception FecExceptionHandler
 */
void FecAccess::memoryChannelReset ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Reset the channel
  fec->memoryChannelReset ( index ) ;
}

/** Method in order to set control register A
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \exception FecExceptionHandler
 */
void FecAccess::setMemoryChannelCRA ( keyType index, tscType8 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->setMemoryChannelCRA ( index, value ) ;
}

/** Method in order to set memory control register A by read modify write operation
 * \param index - key of the corresponding channel
 * \param value - value to be written
 * \param op - operation to be done (TSC_FECSERVER_OR, TSC_FECSERVER_AND, TSC_FECSERVER_XOR, TSC_FECSERVER_EQUAL)
 * \exception FecExceptionHandler
 */
void FecAccess::setMemoryChannelCRA ( keyType index, tscType8 value, logicalOperationType op ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  fec->setMemoryChannelCRA ( index, value, op ) ;
}

/** Method in order to retreive memory control register A from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */   
tscType8 FecAccess::getMemoryChannelCRA ( keyType index ) throw ( FecExceptionHandler ) {
 
  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->getMemoryChannelCRA ( index )) ;
}

/** Method in order to retreive memory status register from a CCU
 * \param index - key of the corresponding channel
 * \return value read
 * \exception FecExceptionHandler
 */ 
tscType8 FecAccess::getMemoryChannelStatus ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  // Hardware access
  return (fec->getMemoryChannelStatus ( index )) ;
}


/** Method in order to set the memory WIN1L register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecAccess::setMemoryChannelWin1LReg ( keyType index, tscType16 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  fec->setMemoryChannelWin1LReg (index, value) ;
}

/** Method in order to set the memory WIN1H register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecAccess::setMemoryChannelWin1HReg ( keyType index, tscType16 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  fec->setMemoryChannelWin1HReg (index, value) ;
}

/** Method in order to set the memory WIN2L register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecAccess::setMemoryChannelWin2LReg ( keyType index, tscType16 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  fec->setMemoryChannelWin2LReg (index, value) ;
}

/** Method in order to set the memory WIN2H register
 * \param index - key of the corresponding channel
 * \param value - value to be written
 */
void FecAccess::setMemoryChannelWin2HReg ( keyType index, tscType16 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  fec->setMemoryChannelWin2HReg (index, value) ;
}

/** Method in order to set the memory mask register
 * \param index - key of the corresponding channel
 * \param value - value used during a RMW operation (see below)
 * \param op - logical operation (and / or / xor)
 */
void FecAccess::setMemoryChannelMaskReg ( keyType index, tscType8 value ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  fec->setMemoryChannelMaskReg (index, value) ;
}

/** Method in order to retreive memory mask register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType8 FecAccess::getMemoryChannelMaskReg ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  return (fec->getMemoryChannelMaskReg (index)) ;
}

/** Method in order to retreive memory WIN1L register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecAccess::getMemoryChannelWin1LReg ( keyType index ) throw ( FecExceptionHandler ) {


  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  return (fec->getMemoryChannelWin1LReg (index)) ;
}

/** Method in order to retreive memory WIN1H register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecAccess::getMemoryChannelWin1HReg ( keyType index ) throw ( FecExceptionHandler ) {


  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  return (fec->getMemoryChannelWin1HReg (index)) ;
}

/** Method in order to retreive memory WIN2L register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecAccess::getMemoryChannelWin2LReg ( keyType index ) throw ( FecExceptionHandler ) {


  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  return (fec->getMemoryChannelWin2LReg (index)) ;
}

/** Method in order to retreive memory WIN2H register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType16 FecAccess::getMemoryChannelWin2HReg ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  return (fec->getMemoryChannelWin2HReg (index)) ;
}

/** Method in order to retreive memory mask register
 * \param index - key of the corresponding channel
 * \return value read
 */   
tscType8 FecAccess::getMemoryChanneMaskReg ( keyType index ) throw ( FecExceptionHandler ) {

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  return (fec->getMemoryChanneMaskReg (index)) ;
}

// -------------------------------------------------------------------------------------
//
//                                 Read / Write methods
//
// -------------------------------------------------------------------------------------

/** Write a block of devices into the hardware for i2c or PIA channels
 * \param index - index of the ring
 * \param vAccessPia - list of frames
 * \param piaChannel - by default false, used to set to false the force acknowledge
 * \param errorList - list of exceptions, please note the exceptions insert in the list must be deleted by the remote methods
 */
unsigned int FecAccess::setBlockDevices ( keyType index, accessDeviceTypeList &vAccessDevices, std::list<FecExceptionHandler *> &errorList, bool piaChannel, bool debugMessageDisplay ) throw (FecExceptionHandler) {

  unsigned int error = 0 ;

  // Retreive the corresponding FecRingDevice
  FecRingDevice *fec = getFecRingDevice ( index ) ;

  if (vAccessDevices.size()) {
    fec->setBlockDevices (vAccessDevices, (forceChannelAck_ & !piaChannel) ) ;

    // Collect the errors
    error += DeviceFrame::decodeErrorFrame ( vAccessDevices, errorList, debugMessageDisplay ) ;
  }

  return (error) ;
}

/** \brief send over the ring the multiple frames block and decode the error afterwards
 * \param hAccesses - hash_table with the block for each ring
 * \param fecAccess - FEC hardware access to send the frames
 * \param piaChannel - by default false, used to set to false the force acknowledge
 * \return the number of error
 */
unsigned int FecAccess::setBlockDevices ( accessDeviceTypeListMap &hAccesses, std::list<FecExceptionHandler *> &errorList, bool piaChannel, bool debugMessageDisplay ) {

#ifdef DEBUGMSGERROR
  std::cout << "-----------------------------------------------------------> " << __PRETTY_FUNCTION__ << std::endl ; 
  std::cout << fecRingEnable_.size() << std::endl ;
  std::cout << std::hex << fecRingEnable_.begin()->second->getFecFirmwareVersion() << std::dec << std::endl;
#endif

  if (fecRingEnable_.size() > 0) {
    if (fecRingEnable_.begin()->second->getFecFirmwareVersion() >= MINFIRMWAREVERSION) {
#ifdef DEBUGMSGERROR
      std::cerr << "FecAccess::setBlockDevices: using the parallelisation on ring with the method FecAccess::setBlockDevicesParallel" << std::endl ;
#endif

      return setBlockDevicesParallel ( hAccesses, errorList, piaChannel, debugMessageDisplay ) ;
    }
  }

  // ONLINERUNNING
  std::cerr << "Online running> this method is deprecated and may not work" << std::endl ;

  unsigned int error = 0 ;
  
  // Send it over the ring and analyse the error
  // send the frames over the ring
  // For each ring, take the list and send it
  for (accessDeviceTypeListMap::iterator vAccesses = hAccesses.begin() ; vAccesses != hAccesses.end() ; vAccesses ++) {
      
    // Download the block of frames into the hardware
#ifdef DEBUGMSGERROR
    std::cout << "Download " << vAccesses->second.size() << " frames for the ring " << getFecKey(vAccesses->first) << "." << getRingKey(vAccesses->first) << std::endl ;
#endif
      
    if (vAccesses->second.size() > 0 ) {

      try {
	// Send over the ring
	error += setBlockDevices( buildFecRingKey(getFecKey(vAccesses->first),getRingKey(vAccesses->first)), vAccesses->second, errorList, piaChannel, debugMessageDisplay ) ;
      }
      catch (FecExceptionHandler &e) {

	error = 1 ;
	FecExceptionHandler *eClone = e.clone() ;
	errorList.push_back(eClone) ;
      }
    }
  }
  
  return (error) ;
}

/** Send over the ring the multiple frames block and decode the error afterwards. 
 * This method compare to the previous one make the send of the frames in parallel on several rings.
 * \param hAccesses - hash_table with the block for each ring
 * \param fecAccess - FEC hardware access to send the frames
 * \param piaChannel - by default false, used to set to false the force acknowledge
 * \return the number of error
 */
unsigned int FecAccess::setBlockDevicesParallel ( accessDeviceTypeListMap &hAccesses, std::list<FecExceptionHandler *> &errorList, bool piaChannel, bool debugMessageDisplay ) {
  //std::cout << "FecAccess::setBlockDevicesParallel" << std::endl ; 

  unsigned int error = 0 ;

  // busy FEC/Ring/CCU/channel
  Sgi::hash_map<keyType, int> busy ;

  // in order to avoid multiple send of frames to a faulty device
  Sgi::hash_map<keyType, FecExceptionHandler *> errorDevice ; 

  // to classified if a fatal errors occurs on the ring or if the whole transaction has been sent
  // or when all the transaction has been sent
  int endTransactionRing[MAX_NUMBER_OF_SLOTS][9] = {{0}} ;

  // Finished when all the frames has been sent
  bool finished = false ;

  // While the the complete list of frames has not been sent
  do {

    // frames just sent
    accessTransactionFrameMap tnumSent[MAX_NUMBER_OF_SLOTS][9] ;
    // Number of frame that needs a read
    unsigned cptRead[MAX_NUMBER_OF_SLOTS][9] = {{0}} ; 
    // store all transaction number used
    std::list<unsigned short> tnumUsed ;

    // For each list of accesses
    for (accessDeviceTypeListMap::iterator vAccesses = hAccesses.begin() ; vAccesses != hAccesses.end() ; vAccesses ++) {

      // if elements are present and no error on the ring happends
      if ( (vAccesses->second.size() > 0) && 
	   !endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)]) {

	// Retreive the corresponding ring
	FecRingDevice *fecRingDevice = NULL ;

	try {
	  // Retreive the corresponding ring
	  fecRingDevice = getFecRingDevice(vAccesses->first) ;
	  // Check the validity of the ring
	  fecRingDevice->checkRing() ;
	}
	catch (FecExceptionHandler &e) {

	  FecExceptionHandler *eClone = e.clone() ;
	  errorList.push_back(eClone) ;

	  endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] = 1 ;
	}

	if (! endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)]) {

	  // *********************************************************************
	  // *********************************************************************
	  //
	  // Sort the frame to be sent
	  //
	  // *********************************************************************
	  // *********************************************************************

	  // Next transaction number
	  unsigned int transactionNumber = 1 ;
	  
	  // Number of word in the FIFO receive
	  unsigned fifoRecWord = 0 ; // Number of words to be received (use to evaluate the maximum size)
	  unsigned wordFifo    = 0 ; // Number of words to be transmitted
	  unsigned nWordFifo   = 0 ; // Number of words expected without error
	  bool noMoreTransactionToBeSent = true ; // if no more transaction has to be sent
	  // FIFO transmit buffer
	  tscType32 toBeTransmited[MAXFECFIFOWORD] ; 
	  
	  // Start since the beginning
	  std::list<accessDeviceType>::iterator itAccessDevice = vAccesses->second.begin() ;

	  // Sort the frame for the corresponding ring
	  // While transaction number for that ring is available       
	  while (transactionNumber &&
		 (fifoRecWord < fecRingDevice->getReceiveFifoDepth()) &&
		 (itAccessDevice != vAccesses->second.end()) &&
		 !endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)]) {

	    //DeviceFrame::displayAccessDeviceType(*itAccessDevice) ;

	    // if the channel is busy more than 2 sec, at that level we consider that we have a timeout
	    if ( busy[getFecRingCcuChannelKey(itAccessDevice->index)] && 
		 (time(NULL) > (busy[getFecRingCcuChannelKey(itAccessDevice->index)]+2)) ) {
	      // Timeout on the direct acknowledge
	      std::string errorMsg = "Timeout reached on the direct acknowledge" ;
	      DD_TYPE_ERROR lcl_err = DD_WRITE_OPERATION_FAILED ;
	      // timeout on the force acknowledge
	      if (itAccessDevice->dAck != 0) {
		errorMsg = "Timeout reached on the force acknowledge" ;
		lcl_err = DD_CANNOT_READ_DATA ;
	      }
	      
	      // Set the error
	      itAccessDevice->e = NEWFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
								       errorMsg,
								       ERRORCODE,
								       itAccessDevice->index) ;

	      busy[getFecRingCcuChannelKey(itAccessDevice->index)] = 0 ;

#ifdef DEBUGMSGERROR
	      std::cerr << itAccessDevice->e->what() << std::endl ;
#endif
	    }

	    // Check if the frame has not been sent and if the channel is not occupied
	    if (!itAccessDevice->sent && !busy[getFecRingCcuChannelKey(itAccessDevice->index)]) {
	      // Check if an error already happends on the same device
	      if (errorDevice.find(itAccessDevice->index) != errorDevice.end()) { // set the same error
		itAccessDevice->sent = true ;
		itAccessDevice->e = errorDevice[itAccessDevice->index] ;
	      }
	      else { // No error on the device => add it to the list to be sent
		
		// Transaction number
		transactionNumber = fecRingDevice->getNextTransactionNumber() ;
		itAccessDevice->tnum = transactionNumber ;
		itAccessDevice->sent = true ;

		unsigned int fifoTraRecWord = 0 ;
		
		// Write the i2c frame in the array for the FIFO tra. (nothing else) and return the number of word written
		if (isi2cChannelCcu25(itAccessDevice->index)) {
		  unsigned int val ;
		  wordFifo += fecRingDevice->buildI2CFrame ( *itAccessDevice, &toBeTransmited[wordFifo], forceChannelAck_, &fifoTraRecWord, &val ) ;
		  // Number of word expected without error
		  nWordFifo += val ;
		  // Counter for read request
		  if ( (itAccessDevice->accessType == MODE_READ) || (forceChannelAck_)) cptRead[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] ++ ;
		}	
		// Write the PIA frame in the array for the FIFO tra. (nothing else) and return the number of word written
		else if (isPiaChannelCcu25(itAccessDevice->index)) {
		  wordFifo += fecRingDevice->buildPIAFrame ( *itAccessDevice, &toBeTransmited[wordFifo], &fifoTraRecWord ) ;
		  // Number of word expected without error
		  nWordFifo = wordFifo ;
		  // Counter for read request
		  if (itAccessDevice->accessType == MODE_READ) cptRead[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] ++ ;
	      }

		// Maximum Number of words to be received (with errors)
		fifoRecWord += fifoTraRecWord ;
	      }
	      // This frame wait for its DACK
	      tnumSent[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)][transactionNumber] = &(*itAccessDevice) ;

	      // The transaction is set as send
	      itAccessDevice->sent = true ;
	      // Channel is busy
	      busy[getFecRingCcuChannelKey(itAccessDevice->index)] = time(NULL) ;
	    }
	    // ---------------------------------------------------------------
	    // No all frames has been sent at that level
	    else if (!itAccessDevice->sent) noMoreTransactionToBeSent = false ;
	    
	    // Next frame
	    itAccessDevice ++ ;
	  }

	  // **********************************************************
	  // Check if it is finished for that ring
	  // **********************************************************
	  if (noMoreTransactionToBeSent) {

	    // Perhaps a transaction has not been sent in the rest of the frames
	    // if a frame has not been sent before this position (itAccessDevice) then it has been seen 
	    // in the line else if (!itAccessDevice->sent) noMoreTransactionToBeSent = false ; just up there
	    for ( ; (itAccessDevice != vAccesses->second.end()) && (itAccessDevice->sent) ; itAccessDevice++ ) ;
	    if (itAccessDevice == vAccesses->second.end())
	      endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] = 2 ;
	  }

	  // **********************************************************
	  // Just display the frames
	  // **********************************************************

#ifdef DEBUGMSGERROR
	  std::cout << "Number of words to be sent in the FIFO transmit " << std::dec << wordFifo << std::endl ;
	  for (unsigned int i = 0 ; i < wordFifo ; i ++) {
	    
	    int size = ((toBeTransmited[i] & 0x0000FF00) >>  8) ;
	    std::cout << std::hex
		      << "0x" << ((toBeTransmited[i] & 0xFF000000) >> 24) << " "
		      << "0x" << ((toBeTransmited[i] & 0x00FF0000) >> 16) << " "
		      << "0x" << ((toBeTransmited[i] & 0x0000FF00) >>  8) << " "
		      << "0x" << ((toBeTransmited[i] & 0x000000FF) >>  0) << " " ;
	    
	    unsigned int mask = 0xFF000000 ;
	    unsigned int decal = 24 ;
	    
	    i ++ ;
	    size -= 1 ;

	    for (int j = 0 ; j < size ; j ++) {
	      
	      std::cout << "0x" << ((toBeTransmited[i] & mask) >> decal) << " " ;

	      if ((decal == 0) && ((j+1) < size)) {
		mask = 0xFF000000 ;
		decal = 24 ;
		i ++ ;
	      }
	      else {
		mask = (mask >> 8) ;
		decal -= 8 ;
	      }
	    }
	    std::cout << std::endl ;
	  }

	  std::cout << "Sending " << std::dec << tnumSent[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)].size() << " on the ring " << (int)fecRingDevice->getFecSlot() << "." << (int)fecRingDevice->getRingSlot() << std::endl ; 
#endif

	  //std::cout << "Sending " << std::dec << tnumSent[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)].size() << " frames on the ring " << (int)fecRingDevice->getFecSlot() << "." << (int)fecRingDevice->getRingSlot() << std::endl ; 	  


	  // If some transactions have to be sent
	  if (tnumSent[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)].size()) {
	    // **********************************************************
	    // Fill the FIFO transmit for the ring and toggle the bit send
	    // **********************************************************
	    // Clear the error bit

	    try {
	      fecRingDevice->setFecRingCR1 ( DD_FEC_CLEAR_ERRORS ) ;
	      fecRingDevice->setFifoTransmit(toBeTransmited, wordFifo) ;
	      fecRingDevice->setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_OR ) ;
	      fecRingDevice->setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_XOR ) ;
	    }
	    catch (FecExceptionHandler &e) {
#ifdef DEBUGMSGERROR
	      std::cout << "Error during the send of the frames: " << e.what() << std::endl ;
#endif

	      FecExceptionHandler *eClone = e.clone() ;
	      errorList.push_back(eClone) ;
	      
	      endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] = 1 ;
	    }
	  }
	}
      }
    }

    // ****************************************************
    // Read all the frame corresponding to the rings
    // ****************************************************
    // Is it finished ?
    finished = true ;
    // For each list of accesses
    for (accessDeviceTypeListMap::iterator vAccesses = hAccesses.begin() ; vAccesses != hAccesses.end() ; vAccesses ++) {

      // If elements are present
      // If the value of endTransactionRing is 2 then it is the end of the frames
      // and AFTER the sending of the frames it is finished
      if ((vAccesses->second.size() > 0) && 
	  (endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] != 1)) {
	
	// Retreive the corresponding ring
	FecRingDevice *fecRingDevice = NULL ;
	try {
	  // Retreive the corresponding ring
	  fecRingDevice = getFecRingDevice(vAccesses->first) ;

	  // **************************************************************************
	  // Read all the frames from the FIFO receive and manage the differents errors
	  // **************************************************************************
	  if (tnumSent[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)].size()) {

	    fecRingDevice->getBlockFrames (forceChannelAck_ && !piaChannel, tnumSent[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)], busy, cptRead[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] ) ;
	    // fecRingDevice->checkRing() ;
	  }
	}
	catch (FecExceptionHandler &e) {

	  FecExceptionHandler *eClone = e.clone() ;
	  errorList.push_back(eClone) ;

	  endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] = 1 ;
	}

	// ***************************************************************************
	// Is it finished ?
	// ***************************************************************************
	// If the value of endTransactionRing is 2 then it is the end of the frames
	// and AFTER the sending of the frames it is finished

	if (endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] == 2)
	  endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)] = 1 ;

	if (!endTransactionRing[getFecKey(vAccesses->first)][getRingKey(vAccesses->first)]) finished = false ;
      }
    }
  }
  while (!finished) ;

  // **********************************************************************
  // **********************************************************************
  //
  // Decode the errors
  // 
  // **********************************************************************
  // **********************************************************************
  for (accessDeviceTypeListMap::iterator vAccesses = hAccesses.begin() ; vAccesses != hAccesses.end() ; vAccesses ++) {
    if (vAccesses->second.size()) {
      // Collect the errors
      error += DeviceFrame::decodeErrorFrame ( vAccesses->second, errorList, debugMessageDisplay ) ;
    }
  }

  return (error) ;
}

/** Read a value in an I2C device (if the key specified any I2C device) or a value
 * from a PIA channel (if the key specified any PIA channel => data register)
 * \param index - key in the map
 * \return the value read
 * \warning PIA/I2C channels
 */
tscType8 FecAccess::read (keyType index) throw ( FecExceptionHandler ) {

  // Device found and read a value
  ccuChannelAccess *device = checkIndex (index) ;
  tscType8 value = device->read () ;
  
  return (value) ;
}

/** Read a value in an I2C device (normal mode access (offset))
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address
 * \param decal - shift the value. Use only for the laserdriver
 * \return the value read
 * \warning only I2C channels
 */
tscType8 FecAccess::readOffset (keyType index, tscType8 offset) throw ( FecExceptionHandler ) {
  return (readOffset (index, offset, 0)) ;
}

/** Read a value in an I2C device (normal mode access (offset/shift of the address))
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address
 * \param decal - shift the value. Use only for the laserdriver
 * \return the value read
 * \warning only I2C channels
 */
tscType8 FecAccess::readOffset (keyType index, 
				 tscType8 offset, 
				 tscType8 decal) throw ( FecExceptionHandler ) {
  // Retreive the access
  i2cAccess *i2c = (i2cAccess *)checkIndex (index) ;

  // Retreive the value
  tscType8 value = i2c->readOffset ( offset, decal ) ;
  
  return (value) ;
}

/** Read a value in an I2C device (extended/ral mode)
 * \param index - index of the device (see the key comments in keyType.h)
 * \param offset - give an offset to the address
 * \return the value read
 * \warning only I2C channels
 */
tscType8 FecAccess::read (keyType index, tscType8 offset) throw ( FecExceptionHandler ) {

  // Device found and write the value
  i2cAccess *i2c = (i2cAccess *)checkIndex (index) ;
  tscType8 value = i2c->read (offset) ;
  
  return (value) ;
}

/** Read a value from a memory channel in single byte mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \return the value read
 * \warning only memory channel
 */
tscType8 FecAccess::read (keyType index, 
			  tscType8 AH, 
			  tscType8 AL ) throw ( FecExceptionHandler ) {

  // Device found and write the value
  memoryAccess *memory = (memoryAccess *)checkIndex (index) ;
  tscType8 value = memory->read (AH, AL) ;
  
  return (value) ;
}

/** Read a value from a memory channel in multiple byte mode
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param size - number of values to be read
 * \param values - pointer to an array of values
 * \warning only memory channel
 */
void FecAccess::read (keyType index, 
                      tscType8 AH, tscType8 AL,
                      unsigned size,
                      tscType8 *values ) throw ( FecExceptionHandler ) {

  // Device found and write the value
  memoryAccess *memory = (memoryAccess *)checkIndex (index) ;
  memory->read (AH, AL, size, values) ;
}

/** Write a value in an I2C device (if the key specified any I2C device) or a value
 * from a PIA channel (if the key specified any PIA channel => data register)
 * \param index - key in the map
 * \param value - value to be written
 * \warning PIA/I2C channels
 */
void FecAccess::write (keyType index, tscType8 value) throw ( FecExceptionHandler ) {

  // Device found and read a value
  ccuChannelAccess *device = checkIndex (index) ;
  device->write (value) ;
}

/** Write a value in an I2C device in normal mode (offset of the address))
 * \param index - key in the map
 * \param offset - give an offset to the address
 * \param decal - shift the value. Use only for the laserdriver
 * \param value - value to be written
 * \warning only I2C channels
 */
void FecAccess::writeOffset (keyType index, tscType8 offset, tscType8 value) throw ( FecExceptionHandler ) {
  writeOffset (index, offset, 0, value) ;
}

/** Write a value in an I2C device in normal mode (offset/shift of the address))
 * \param index - key in the map
 * \param offset - give an offset to the address
 * \param decal - shift the value. Use only for the laserdriver
 * \param value - value to be written
 * \warning only I2C channels
 */
void FecAccess::writeOffset (keyType index, 
                             tscType8 offset, tscType8 decal, 
                             tscType8 value) throw ( FecExceptionHandler ) {

  // Device found and write the value
  i2cAccess *i2c = (i2cAccess *)checkIndex (index) ;
  i2c->writeOffset (offset, decal, value) ;
}

/** Write a value in an I2C device in extended/ral mode
 * \param index - key in the map
 * \param offset - give an offset to the address
 * \param value - value to be written
 * \warning only I2C channels
 */
void FecAccess::write (keyType index, tscType8 offset, tscType8 value) throw ( FecExceptionHandler ) {

  // Device found and write the value
  i2cAccess *i2c = (i2cAccess *)checkIndex (index) ;
  i2c->write (offset, value) ;
}

/** Write a value in a memory channel in single byte mode
 * \param index - key in the map
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param value - value to be written
 * \warning only memory channel
 */
void FecAccess::write (keyType index, 
                       tscType8 AH, tscType8 AL, 
                       tscType8 value) throw ( FecExceptionHandler ) {

  // Device found and write the value
  memoryAccess *memory = (memoryAccess *)checkIndex (index) ;
  memory->write (AH, AL, value) ;
}

/** Write a value in a memory channel in multiple byte mode
 * \param index - key in the map
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param size - number of values to be read
 * \param values - pointer to an array of values
 * \warning only memory channel
 */
void FecAccess::write (keyType index, 
                       tscType8 AH, tscType8 AL, 
                       tscType8 *values,
                       unsigned size) throw ( FecExceptionHandler ) {

  // Device found and write the value
  memoryAccess *memory = (memoryAccess *)checkIndex (index) ;
  memory->write (AH, AL, values, size) ;
}

/** Read modify write a value from the memory specified by the key in single byte
 * \param index - index of the device (see the key comments in keyType.h)
 * \param AH - Position in the high memory
 * \param AL - Position in the low memory
 * \param op - operation to be done (CMD_OR, CMD_AND, CMD_XOR)
 * \param mask - mask to be done during the read modify write
 */
void FecAccess::write ( keyType index, 
                        tscType8 AH, tscType8 AL, 
                        logicalOperationType op, 
                        tscType8 mask ) throw ( FecExceptionHandler ) {

  // Device found and write the value
  memoryAccess *memory = (memoryAccess *)checkIndex (index) ;
  memory->write (AH, AL, op, mask) ;
}

// ********************************************************************************************************
//
//                                STATIC method for FecAccess creation
//
// ********************************************************************************************************

/** 
 * \param vmeFileName - geographical item or plug and play and play FEC ID list
 * \param vmeFileNamePnP - empty if geographical address used or the plug and play file
 * \return true if plug and play is used, false in geographical address
 * \warning this method looks in $HOME/FecSoftwareV3_0/config/ or in $FECSOFTWARE_ROOT/config/
 */
bool FecAccess::getVmeFileName ( std::string &vmeFileName, std::string &vmeFileNamePnP ) {
  
  // Try to find the correct file for the VME file name for offset
  // and addresses
  vmeFileName.clear() ; vmeFileNamePnP.clear() ;
  bool plugandplayUsed = false ;
  
  // $ENV_CMS_TK_FEC_ROOT directory or ENV_TRACKER_DAQ directory
  char *basic=getenv ("ENV_TRACKER_DAQ") ;
  if (basic != NULL) {
    std::ostringstream vmeFilePnP, vmeFileList ;
    vmeFilePnP  << basic << "/config/PlugNPlayConfigure.dat" ;
    vmeFileList << basic << "/config/FecHardwareIdList.dat" ;
    std::ifstream fichierPnP  (vmeFilePnP.str().c_str()) ;
    std::ifstream fichierList (vmeFileList.str().c_str()) ;

    if (fichierPnP && fichierList) { // Use the plug and play
      vmeFileNamePnP = vmeFilePnP.str() ;
      vmeFileName    = vmeFileList.str() ;
      plugandplayUsed = true ;
    }
    else { // Test the geographical address

      std::ostringstream vmeFileOff ;
      vmeFileOff << basic << "/config/FecAddressTable.dat" ;
      std::ifstream fichier (vmeFileOff.str().c_str()) ;
      if (fichier) {
	vmeFileName    = vmeFileOff.str() ;
	vmeFileNamePnP = "" ;
	plugandplayUsed = false ;
      }
      else basic = NULL ;
    }
  }

  // No ENV_TRACKER_DAQ
  if (basic == NULL) {
    basic=getenv ("ENV_CMS_TK_FEC_ROOT") ;
    if (basic != NULL) {
      std::ostringstream vmeFilePnP, vmeFileList ;
      vmeFilePnP  << basic << "/config/PlugNPlayConfigure.dat" ;
      vmeFileList << basic << "/config/FecHardwareIdList.dat" ;
      std::ifstream fichierPnP  (vmeFilePnP.str().c_str()) ;
      std::ifstream fichierList (vmeFileList.str().c_str()) ;
	
      if (fichierPnP && fichierList) { // Use the plug and play
	vmeFileNamePnP = vmeFilePnP.str() ;
	vmeFileName    = vmeFileList.str() ;
	plugandplayUsed = true ;
      }
      else { // Test the geographical address
	  
	std::ostringstream vmeFileOff ;
	vmeFileOff << basic << "/config/FecAddressTable.dat" ;
	std::ifstream fichier (vmeFileOff.str().c_str()) ;
	if (fichier) {
	  vmeFileName    = vmeFileOff.str() ;
	  vmeFileNamePnP = "" ;
	  plugandplayUsed = false ;
	}
	else basic = NULL ;
      }
    }
  }

  // No ENV_TRACKER_DAQ and no ENV_CMS_TK_FEC_ROOT
  if (basic == NULL) {
    // $HOME/FecSoftwareV3_0/config
    basic = getenv ("HOME") ;

    if (basic != NULL) {
      std::ostringstream vmeFilePnP, vmeFileList ;
      vmeFilePnP  << basic << "/FecSoftwareV3_0/config/PlugNPlayConfigure.dat" ;
      vmeFileList << basic << "/FecSoftwareV3_0/config/FecHardwareIdList.dat" ;
      std::ifstream fichierPnP  (vmeFilePnP.str().c_str()) ;
      std::ifstream fichierList (vmeFileList.str().c_str()) ;

      if (fichierPnP && fichierList) { // Use the plug and play
	vmeFileNamePnP = vmeFilePnP.str() ;
	vmeFileName    = vmeFileList.str() ;
	plugandplayUsed = true ;
      }
      else { // Test the geographical address

	std::ostringstream vmeFileOff ;
	vmeFileOff << basic << "/FecSoftwareV3_0/config/FecAddressTable.dat" ;
	std::ifstream fichier (vmeFileOff.str().c_str()) ;
	if (fichier) {
	  vmeFileName    = vmeFileOff.str() ;
	  vmeFileNamePnP = "" ;
	  plugandplayUsed = false ;
	}
	else basic = NULL ;
      }
    }
  }

  // No ENV_TRACKER_DAQ and no ENV_CMS_TK_FEC_ROOT and no $HOME/FecSoftwareV3_0/config
  if (basic == NULL) {
    // $FECSOFTWARE_ROOT/config/
    basic=getenv ("FECSOFTWARE_ROOT") ;
    if (basic != NULL) {
      std::ostringstream vmeFilePnP, vmeFileList ;
      vmeFilePnP  << basic << "/FecSoftwareV3_0/config/PlugNPlayConfigure.dat" ;
      vmeFileList << basic << "/FecSoftwareV3_0/config/FecHardwareIdList.dat" ;
      std::ifstream fichierPnP  (vmeFilePnP.str().c_str()) ;
      std::ifstream fichierList (vmeFileList.str().c_str()) ;

      if (fichierPnP && fichierList) { // Use the plug and play
	vmeFileNamePnP = vmeFilePnP.str() ;
	vmeFileName    = vmeFileList.str() ;
	plugandplayUsed = true ;
      }
      else { // Test the geographical address

	std::ostringstream vmeFileOff ;
	vmeFileOff << basic << "/FecSoftwareV3_0/config/FecAddressTable.dat" ;
	std::ifstream fichier (vmeFileOff.str().c_str()) ;
	if (fichier) {
	  vmeFileName    = vmeFileOff.str() ;
	  vmeFileNamePnP = "" ;
	  plugandplayUsed = false ;
	}
	else basic = NULL ;
      }
    }
  }

  return (plugandplayUsed) ;
}

/** To avoid duplication of the code, here the method to create the FEC Access with or without plug and play from the console
 * \param argc - number of arguments
 * \param argv - arguments  (-pci, -vmesbs, -vmecaenpci, -vmecaenusb, -usb and files related to VME). For VME file names: following the method getVmeFileName, if the files 
PlugNPlayConfigure.dat and FecHardwareIdList.dat are existing in the directories (see getVmeFileName for details) then the plug and play is automatically used. If not the geometrical address is used (FecAddressTable.dat).
 * \param cnt - index to the next parameters after the detection
 * \param init - initialisation of the FEC that can be overwrite by the options.
 * Options of this method are:
 * <lu>
 * <li> force acknowledge used (-noFack or -fack (default is -fack))
 * <li> i2cSpeed used (-i2cSpeed [100 | 200 | 400 | 1000])
 * <li> initialise or not the FEC (-noInitFec, -initFec (default is initFec)
 * <li> initialise only the corresponding FEC if it is different from 0xFFFFFFFF (only for VME FEC) (-fec <>)
 * <li> invert the clock polarity (-invrecvclk, false by default)
 * </lu>
 * \return FecAccess created
 */
FecAccess *FecAccess::createFecAccess ( int argc, char **argv, int *cnt, bool init ) throw (FecExceptionHandler) {

  bool forceAck = true ; 
  unsigned int i2cSpeed = 100 ;
  bool initFec = init ;
  unsigned int fecSlot = 0xFFFFFFFF ;
  bool invertClockPolarity = false ;
  std::string fecHardwareId; 
  
  for (int i = 0 ; i < argc ; i ++) {
    if (strncasecmp(argv[i],"-invrecvclk",strlen("-invrecvclk")) == 0) invertClockPolarity = true ;
    else if (strncasecmp(argv[i],"-i2cSpeed",strlen("-i2cSpeed")) == 0) {
      i2cSpeed = 0 ;
      if ((i+1) < argc) {
	i ++ ;
	if (sscanf (argv[i], "%u", &i2cSpeed) == 0) {
	  std::cerr << "Please specify the I2C speed after option -i2cSpeed [100 | 200 | 400 | 1000" << std::endl ;
	  return NULL ;
	}
      }
      if ( (i2cSpeed != 100) && (i2cSpeed != 200) && (i2cSpeed != 400) && (i2cSpeed != 1000) ) i2cSpeed = 0 ;
      if (i2cSpeed == 0) {
	std::cerr << "Please specify the I2C speed after option -i2cSpeed [100 | 200 | 400 | 1000" << std::endl ;
	return NULL ;
      }
    }
    else if (strcasecmp(argv[i],"-initFec") == 0) initFec = true ;
    else if (strcasecmp(argv[i],"-noInitFec") == 0) initFec = false ;
    else if (strcasecmp(argv[i],"-noFack") == 0) forceAck = false ;
    else if (strcasecmp(argv[i],"-fack") == 0) forceAck = true ;
    else if (strcasecmp(argv[i],"-fec") == 0) {
      if ((i+1) < argc) {
	i ++ ;
	if (sscanf (argv[i], "%u", &fecSlot) == 0) {
	  std::cerr << "Please specify the FEC slot between 1 and 21" << std::endl ;
	  return NULL ;
	}
      }
      else {
	std::cerr << "Please specify the FEC slot after option -fec" << std::endl ;
	return NULL ;
      }
    } else if (strcasecmp(argv[i],"-fechardwareid") == 0) {
      if ((i+1) < argc) {
	i ++ ;
	if (argv[i][0] == '-') {
	  std::cerr << "value after -fechardwareid should not begin with '-'" << std::endl ;
	  return NULL ;
	}
	fecHardwareId = std::string(argv[i]);
      }
      else {
	std::cerr << "Please specify the FEC USB serial number after option -fechardwareid" << std::endl ;
	return NULL ;
      }      
    } 
  }

  FecAccess *fecAccess = NULL ;

  int cpt = 1 ;

  // FEC type
  enumFecBusType fecBusType = FECVME  ;
  enumBusAdapter busAdapter = CAENPCI ;

  std::string vmeFileName ;
  std::string vmeFileNamePnP ;
  bool pnpUsed = false ;

  if (argc >= 2) {
    if (strcasecmp (argv[cpt],"-pci") == 0) { // If a new fecAddress has been set
	
      fecBusType = FECPCI ;
      cpt ++ ;
    } 
    else if ( (strcasecmp (argv[cpt],"-vmesbs") == 0) || (strcasecmp (argv[cpt],"-vme") == 0)) { // If a new fecAddress has been set

      cpt ++ ;
      fecBusType = FECVME ;
      busAdapter = SBS    ;
    }
    else if (strcasecmp (argv[cpt],"-vmecaenpci") == 0) { // If a new fecAddress has been set

      cpt ++ ;
      fecBusType = FECVME ;
      busAdapter = CAENPCI    ;
    }
    else if (strcasecmp (argv[cpt],"-vmecaenusb") == 0) { // If a new fecAddress has been set
	
      cpt ++ ;
      fecBusType = FECVME  ;
      busAdapter = CAENUSB ;
    }
    else if (strcasecmp (argv[cpt],"-usb") == 0) { // If a new fecAddress has been set
      
      fecBusType = FECUSB ;
      cpt ++ ;
    } 
  }
    
  if (fecBusType == FECVME) {

    // Check if a filename is given
    if ( (cpt < argc) && (argv[cpt][0] != '-') ) {
      vmeFileName = argv[cpt] ;
      cpt++ ;
	
      std::ifstream fichier (vmeFileName.c_str()) ;
      if (fichier) {
	std::cout << "VME FEC will be used with the file " << vmeFileName << std::endl ;
      }
      else {
	std::cerr << "The file " << vmeFileName << " does not exists" << std::endl ;
	return NULL ;
      }
    }
    else {

      // Get the VME File name
      pnpUsed = FecAccess::getVmeFileName ( vmeFileName, vmeFileNamePnP ) ;
	
      if (vmeFileName.size() != 0) {
	if (pnpUsed)
	  std::cout << "Plug and play is used with file " << vmeFileNamePnP << " and file " << vmeFileName << std::endl ;
	else
	  std::cout << "VME FEC will be used with the file " << vmeFileName << std::endl ;
      }
      else {
	std::cerr << "No file found for VME" << std::endl ;
	return NULL ;
      }
    }
  } 
    
  // Create the FEC Access
  switch ( fecBusType ) {
  case FECPCI:
    // bool forceAck, bool initFec, bool scanFECs, bool scanCCus, bool i2cSpeed
    fecAccess = new FecAccess (forceAck, initFec, true, false, (tscType16)i2cSpeed, invertClockPolarity) ;
    break ;
  case FECVME: {
    uint32_t fecAddresses[MAX_NUMBER_OF_SLOTS] = {0} ;      
    if (fecSlot != 0xFFFFFFFF) {
      // Only the access to the corresponding FEC slot is created
      fecAddresses[fecSlot] = FecVmeRingDevice::VMEFECBASEADDRESSES[fecSlot] ;
    }
    else {
      for (unsigned int i = 0 ; i < MAX_NUMBER_OF_SLOTS ; i ++) 
	fecAddresses[i] = FecVmeRingDevice::VMEFECBASEADDRESSES[i] ;
    }

    // unsigned adapterSlot, std::string configurationFile, 
    // bool forceAck, bool init, bool scanFECs, bool scanCCus, bool i2cSpeed
    if (pnpUsed) {
      fecAccess = new FecAccess (0, vmeFileName, vmeFileNamePnP, forceAck, initFec, true, false, (tscType16)i2cSpeed, FecVmeRingDevice::STR_BUSADAPTER_NAME[busAdapter], false, 8, invertClockPolarity) ;
    }
    else {

      fecAccess = new FecAccess (0, vmeFileName, fecAddresses, forceAck, initFec, true, false, (tscType16)i2cSpeed, FecVmeRingDevice::STR_BUSADAPTER_NAME[busAdapter], false, 8, invertClockPolarity) ;
    }
    break ;
  }
  case FECUSB:
    // FEC usb
    hashMapFecUsbSerial theMap; 

    if (fecSlot == 0xFFFFFFFF) fecSlot = 0 ; 
      
    theMap[fecSlot] = fecHardwareId ; 
    
    fecAccess = new FecAccess (theMap, forceAck, initFec, true, false, (tscType16)i2cSpeed, invertClockPolarity) ;
    break ;
  }
    
  if (!fecAccess) {
    std::cerr << "Creation of FecAccess failed. fecAccess pointer null." << std::endl ; 
    return NULL ;
  }
    
  std::list<keyType> *fecList = fecAccess->getFecList() ;
    
  if ((fecList == NULL) || (fecList->empty())) {
    std::cerr << "Warning: No FEC rings board found" << std::endl ;
  }
  delete fecList;
    
  *cnt = cpt ;

  return (fecAccess) ;
}

