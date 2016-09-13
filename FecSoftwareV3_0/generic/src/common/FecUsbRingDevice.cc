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
*/

#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::hex;

#include <unistd.h> // for usleep

#include "tscTypes.h"
#include "keyType.h"
#include "cmdDescription.h"
#include "dderrors.h"


#include "fec_usb.h" 
#include "FecUsbRingDevice.h"


/* static constants : */ 

/** Number of slot in the USB FEC bus
 */
const unsigned int FecUsbRingDevice::minUsbFecSlot = 0 ;
/** Number of slot in the FEC USB bus
 */
const unsigned int FecUsbRingDevice::maxUsbFecSlot = 21 ;
/** Number of ring per USB FEC
 */
const unsigned int FecUsbRingDevice::maxUsbFecRing = 0 ;  
/** Number of ring per USB FEC
 */
const unsigned int FecUsbRingDevice::minUsbFecRing = 0 ;

 

/******************************************************
		CONSTRUCTOR - DESTRUCTOR
******************************************************/

/* 
 * \param fd - device descriptor ????
 * \param fecSlot - slot of the FEC
 * \param ring - ring slot
 * \param init - initialise the FEC (with reset at the starting)
 */
FecUsbRingDevice::FecUsbRingDevice (std::string serial, tscType8 fecSlot, tscType8 ringSlot, bool init, bool invertClockPolarity  ) throw ( FecExceptionHandler ) :
  FecRingDevice ( fecSlot, ringSlot, FECUSB ) {

  device_ = fec_usb_open((char*)serial.c_str()) ; /* do not check for serial number */ 

  if (device_ < 0) {
    
    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_FECDDNOTOPEN,
				      TSCFEC_FECDDNOTOPEN_MSG,
				      FATALERRORCODE,
				      buildFecRingKey (fecSlot, ringSlot),
				      "Error code returned by the ioctl", -device_) ;
  }
	
  //Check here if board is a FEC or something else --> Exception to raise in this last case

  // Initialise or not the FEC
  FecRingDevice::setInitFecRingDevice ( init, invertClockPolarity ) ;

  // set timeout loops to minimum.. usb fec is ssslllooowwww... 
  loopInTimeWriteFrame_  = 100; 
  loopInTimeDirectAck_ = 100; 
  loopInTimeReadFrame_ = 100 ; 
}


/** Close the USB device
 */
FecUsbRingDevice::~FecUsbRingDevice ( ) throw ( FecExceptionHandler ) {

//   if (device_ != -1) { 

//     FECUSB_TYPE_ERROR status = fec_usb_close(device_) ;  


//     if (status != FECUSB_RETURN_OK) {
//          RAISEFECEXCEPTIONHANDLER_HARDPOSITION (TSCFEC_CANNOTOPENDEVICEDRIVER,
//					 "Unable to close the USB device driver",
//					 FATALERRORCODE,
//					 buildFecRingKey (getRingSlot(),getRingSlot()) ) ;
//   }
}

/******************************************************
		BOARD RESET & CONFIGURATION FUNCTIONS
******************************************************/





/******************************************************
	CONTROL & STATUS RTEGISTERS ACCESS
******************************************************/

/** Write the value given as parameter in FEC control 0 register 
 * \param ctrl0Value - value of the CR0
 * \param force - if force is set then the value is applied blindly. if force is not set then the invert clock polarity is managed following the parameter in the class.
 */
void FecUsbRingDevice::setFecRingCR0 ( tscType16 ctrl0Value, bool force ) throw ( FecExceptionHandler ) {

  if (!force) {
    if (invertClockPolarity_) ctrl0Value |= FEC_CR0_POLARITY ;
    else ctrl0Value &= (~FEC_CR0_POLARITY) ;
  } 

#ifdef FECUSBRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << ctrl0Value << std::endl ;
#endif

  FECUSB_TYPE_ERROR error = fec_usb_set_ctrl0(device_, ctrl0Value);
  if (error != FECUSB_RETURN_OK)
    {
      RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
					TSCFEC_USBACCESSPROBLEM_MSG + ": unable to access the FEC control register 0",
					ERRORCODE,
					buildFecRingKey(getFecSlot(), getRingSlot()),
					"USB error", error) ;
    }

}



/** Reads a value from FEC ctrl0 
 *\return the value read
 */
tscType16 FecUsbRingDevice::getFecRingCR0( ) throw ( FecExceptionHandler ) {
  tscType16 ctrl0Value = 0 ;
  
  FECUSB_TYPE_ERROR error = fec_usb_get_ctrl0(device_, &ctrl0Value);

#ifdef FECUSBRINGDEVICE_DEBUG
  std::cout << "DEBUG : reading value 0x" << hex << ctrl0Value << " from CRO" << std::endl ;
#endif

  if (error != FECUSB_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to access the FEC control register 0",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return (tscType16)ctrl0Value;
}



/** Write the value given as parameter in FEC control 1 register 
 * \param value to be written
 */
void FecUsbRingDevice::setFecRingCR1( tscType16 ctrl1Value ) throw ( FecExceptionHandler ) {
  
#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << ctrl1Value << " in CR1" << std::endl ;
#endif

  FECUSB_TYPE_ERROR error = fec_usb_set_ctrl1(device_, ctrl1Value);
  if (error != FECUSB_RETURN_OK) {
    
    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to access the FEC control register 1",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }

}




tscType16 FecUsbRingDevice::getFecRingCR1( ) throw ( FecExceptionHandler ) {

  tscType16 ctrl1Value = 0;
  
  FECUSB_TYPE_ERROR error = fec_usb_get_ctrl1(device_, &ctrl1Value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : reading value 0x" << hex << ctrl1Value << " from CR1" << std::endl ;
#endif

  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to access the FEC control register 1",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return (tscType16)ctrl1Value;
}

/** Reads a value from FEC SR0
 * \return the value of the FEC SR0
 */
tscType32 FecUsbRingDevice::getFecRingSR0( ) throw ( FecExceptionHandler ) {

  tscType32 sr0Value = 0 ; 
  FECUSB_TYPE_ERROR error = fec_usb_get_sr0(device_, &sr0Value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : reading value 0x" << hex << sr0Value << " from SRO" << std::endl ;
#endif

  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to access the FEC status register 0",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return (tscType32)sr0Value;
}




tscType16 FecUsbRingDevice::getFecRingSR1( ) throw ( FecExceptionHandler ) {

  tscType16 sr1Value = 0 ;
  
  FECUSB_TYPE_ERROR error = fec_usb_get_sr1(device_, &sr1Value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : reading value 0x" << hex << sr1Value << " from SR1" << std::endl ;
#endif

  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to access the FEC status register 1",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return (tscType16)sr1Value;
}

/** \brief Get the FEC version register the FEC
 * \return value read
 */
tscType16 FecUsbRingDevice::getFecFirmwareVersion( )  throw ( FecExceptionHandler ) { 
  tscType32 versionValue = 0 ;
  
  FECUSB_TYPE_ERROR error = fec_usb_get_version(device_, &versionValue);

#ifdef FECUSBRINGDEVICE_DEBUG
  std::cout << "DEBUG : Firmware version: 0x" << hex << fecVersion << " from FEC" << std::endl ;
#endif

  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to read FEC firmware",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }

  return (tscType16)((versionValue & 0x0000ff00));



} 


/******************************************************
	FIFO ACCESS - NATIVE 32 BITS FORMAT
	NATIVE FORMAT ACCESS ALLOWS R/W OPERATIONS
******************************************************/

/** Return a word from the FIFO receive
 * \param return a word from the FIFO receive
 */
tscType32 FecUsbRingDevice::getFifoReceive( ) throw ( FecExceptionHandler ) {

  tscType32 fiforec_value = 0 ;
  
  FECUSB_TYPE_ERROR error = fec_usb_get_fiforec(device_,  &fiforec_value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "Value 0x" << hex << fiforec_value << " read from fifo receive" << std::endl ;
#endif

  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to read a word in the FIFO receive",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return (tscType32)fiforec_value;
}


/** set a word in the FIFO receive
 * \param fiforecValue - word to be set
 */
void FecUsbRingDevice::setFifoReceive( tscType32 fiforecValue ) throw ( FecExceptionHandler ) {

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << fiforecValue << " to fifo receive" << std::endl ;
#endif

  FECUSB_TYPE_ERROR error = fec_usb_set_fiforec(device_, fiforecValue);
  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to write a word in the FIFO receive",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }  
}

/** Return a word from the FIFO return
 * \param return a word from the FIFO return
 */
tscType8 FecUsbRingDevice::getFifoReturn( ) throw ( FecExceptionHandler ) {
  
  tscType32 fiforet_value = 0 ;
  
  FECUSB_TYPE_ERROR error = fec_usb_get_fiforet(device_, &fiforet_value);
 
#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "Value 0x" << hex << fiforet_value << " read from fifo return" << std::endl ;
#endif

  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to read a word in the FIFO return",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return (tscType8)fiforet_value;
}


/** set a word in the FIFO return
 * \param fiforet_value - word to be set
 */
void FecUsbRingDevice::setFifoReturn( tscType8 fiforetValue ) throw ( FecExceptionHandler ) {
  
#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << fiforet_value << " to fifo return" << std::endl ;
#endif

  FECUSB_TYPE_ERROR error = fec_usb_set_fiforet(device_, fiforetValue);
  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to write a word in the FIFO return",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }

}

/** Return a word from the FIFO transmit
 * \param return a word from the FIFO transmit
 */
tscType32 FecUsbRingDevice::getFifoTransmit( ) throw ( FecExceptionHandler ) {

  tscType32 fifotra_value = 0 ; 
  
  FECUSB_TYPE_ERROR error = fec_usb_get_fifotra(device_,  &fifotra_value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "Value 0x" << hex << fifotra_value << " read from fifo transmit" << std::endl ;
#endif

  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to read a word in the FIFO transmit",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return (tscType32)fifotra_value;
}


void FecUsbRingDevice::setFifoTransmit( tscType32 fifotraValue ) throw ( FecExceptionHandler ) {

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << fiforet_value << " to fifo transmit" << std::endl ;
#endif

  FECUSB_TYPE_ERROR error = fec_usb_set_fifotra(device_, fifotraValue);
  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to write a word in the FIFO transmit",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }


}

/******************************************************
		HARD RESET
******************************************************/	

/** Hard reset of the FEC board or the ring board
 * \warning For the moment the FEC ring device is not implemented and cannot be implemented (status from 23/04/04).
 * \warning A FEC board reset will be implemented. The problem with this reset is that if you reset the board all the rings
 * and not this specific ring will be reseted. 
 * \warning Due to the fact that FEC board reset is not yet implemented, we use for the moment the crate reset, static method
 * from this class
 * \warning In the future if the hard FEC ring reset is not implemented then this method will be moved to the FecAccess
 * the same for the crate and fecHardReset will dispaeared.
 * \exception FecExceptionHandler
 * 
 */
void FecUsbRingDevice::fecHardReset ( ) throw ( FecExceptionHandler ) {

  std::cout << "FecUsbRingDevice::fecHardReset(), rising up bit 13 in FEC CR0..." << std::endl ; 
  std::cout << "FecUsbRingDevice::fecHardReset(), getting original value..." << std::endl ; 
  tscType16 vcr0 = getFecRingCR0(); 
  vcr0 |= FEC_CR0_RESETFSMFEC ; // rise up reset FEC bit (13)
  std::cout << "FecUsbRingDevice::fecHardReset(), writing modified..." << std::endl ; 
  setFecRingCR0(vcr0);
  std::cout << " done. " << std::endl ; 

  // Re-initialise all tables used in FecRingDevice
  FecRingDevice::initAfterHardReset() ;
}


/******************************************************
		IRQ enable / disable
******************************************************/	

/** IRQ address, enable and disable the IRQ, nothing for the USB FEC 
 */
void FecUsbRingDevice::setIRQ ( bool enable, tscType8 level ) throw ( FecExceptionHandler ) {

  /* empty function: do nothing , USB is not interrupt driven, no risc of interrupt overflood */
  
} 

/** Write a block of words to the FIFO receive
 */
void FecUsbRingDevice::setFifoReceive ( tscType32 *value, int count) 
  throw (FecExceptionHandler) { 

  //int i; 
  //for(i=0;i<count;i++) setFifoReceive(value[i]) ; 
       

  FECUSB_TYPE_ERROR error = fec_usb_set_block_fiforec(device_,value,count);
  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to write word(s) in the FIFO receive",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }


} 

/** Get block of words from the FIFO receive
 */
tscType32* FecUsbRingDevice::getFifoReceive ( tscType32 *value, int count ) 
  throw (FecExceptionHandler) { 
  //int i; 
  //for(i=0;i<count;i++) value[i]  = getFifoReceive() ;
  //return value ; 

  
  FECUSB_TYPE_ERROR error = fec_usb_get_block_fiforec(device_,value,count);
  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to read word(s) in the FIFO receive",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return value;
} 

/** set a word into the FIFO return
 */
void FecUsbRingDevice::setFifoReturn ( tscType8 *value, int count ) 
  throw (FecExceptionHandler) { 

  fecusbType32 valueTemp[count] ;
  for(int i=0;i<count;i++) valueTemp[i] = (fecusbType32)value[i] ;
       
  FECUSB_TYPE_ERROR error = fec_usb_set_block_fiforec(device_,valueTemp,count);
  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to write word(s) in the FIFO return",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }


}

/** get a word from the FIFO return
 */
tscType8* FecUsbRingDevice::getFifoReturn ( tscType8 *value, int count ) 
  throw (FecExceptionHandler) { 
  //int i; 
  //for(i=0;i<count;i++) value[i]  = getFifoReturn() ;
  //return value ; 
  //tscType32 fiforet_value = 0 ;
  
  fecusbType32 valueTemp[count] ;
  FECUSB_TYPE_ERROR error = fec_usb_get_block_fiforet(device_,valueTemp,count);
  for(int i=0;i<count;i++) value[i] = (tscType8)valueTemp[i] ;
  if (error != FECUSB_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
				      TSCFEC_USBACCESSPROBLEM_MSG + ": unable to read word(s) in the FIFO return",
				      ERRORCODE,
				      buildFecRingKey(getFecSlot(), getRingSlot()),
				      "USB error", error) ;
  }
  return value;

}

/** set a word into the FIFO Transmit
 */
void FecUsbRingDevice::setFifoTransmit ( tscType32 *value, int count ) 
  throw (FecExceptionHandler) {
  //int i; 
  //for(i=0;i<count;i++) setFifoTransmit(value[i]) ;
  FECUSB_TYPE_ERROR error = fec_usb_set_block_fifotra(device_,value,count);
  if (error != FECUSB_RETURN_OK)
    {
      RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
					TSCFEC_USBACCESSPROBLEM_MSG + ": unable to write word(s) in the FIFO transmit",
					ERRORCODE,
					buildFecRingKey(getFecSlot(), getRingSlot()),
					"USB error", error) ;
    }
}

/** get a word from the FIFO transmit
 */
tscType32* FecUsbRingDevice::getFifoTransmit ( tscType32 *value, int count ) 
  throw (FecExceptionHandler) {
  //int i; 
  //for(i=0;i<count;i++) value[i]  = getFifoTransmit() ;
  //return value ; 
      
  
  FECUSB_TYPE_ERROR error = fec_usb_get_block_fifotra(device_,value,count);
  if (error != FECUSB_RETURN_OK)
    {
      RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_USBACCESSPROBLEM,
					TSCFEC_USBACCESSPROBLEM_MSG + ": unable to read word(s) in the FIFO transmit",
					ERRORCODE,
					buildFecRingKey(getFecSlot(), getRingSlot()),
					"USB error", error) ;
    }
  return value;

}
