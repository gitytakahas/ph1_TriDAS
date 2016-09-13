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

#include <fcntl.h>      // For open function
#include <unistd.h>     // For close function

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

#include "FecPciRingDevice.h"

//Uncomment to toggle debug messages on
//#define FECPCIRINGDEVICE_DEBUG


/******************************************************
		CONSTRUCTOR - DESTRUCTOR
******************************************************/

/* Open the PCI device driver and intialise the FEC if asked
 * \param fecSlot - slot of the FEC
 * \param ring - ring slot
 * \param init - initialise the FEC (with reset at the starting)
 */
FecPciRingDevice::FecPciRingDevice (tscType8 fecSlot, tscType8 ringSlot, bool init, bool invertClockPolarity ) throw ( FecExceptionHandler ) :
  FecRingDevice ( fecSlot, ringSlot, FECPCI ) {

  // Build the name of the device driver
  char devicename[100] ;
  snprintf (devicename, 100, DEVICEDRIVERNAME, fecSlot) ;
	
  deviceDescriptor_ = open(devicename, PCIDRIVERMODE);

  if (deviceDescriptor_ < 0) {
      RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_FECDDNOTOPEN,
					TSCFEC_FECDDNOTOPEN_MSG,
					FATALERRORCODE,
					buildFecRingKey (fecSlot, ringSlot),
					"Error code returned by the ioctl", -deviceDescriptor_) ;
  }
  
  // Now, check version compatibility
  char ddVersion[DD_VERSION_STRING_SIZE] ;
  DD_TYPE_ERROR lcl_err = glue_fecdriver_get_driver_version(deviceDescriptor_, ddVersion) ;
  if ( (lcl_err != DD_RETURN_OK) || (strcmp (ddVersion, FEC_DRIVER_VERSION)) ) {
    if (lcl_err == DD_RETURN_OK)
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_BADCOMPATIBILITY,
					      TSCFEC_BADCOMPATIBILITY_MSG,
					      FATALERRORCODE,
					      buildFecRingKey(fecSlot,ringSlot) ) ;
                                             //"Driver version", ddVersion) ; <= ddVersion is a string
    else
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					     "Unale to access the driver version",
					     FATALERRORCODE,
					     buildFecRingKey (fecSlot, ringSlot)) ;
  }
  
#ifdef IRQMANAGER
  // Disable the IRQ
  glue_plx_disable_irqs (deviceDescriptor_) ;
#endif

  // Initialise or not the FEC
  FecRingDevice::setInitFecRingDevice ( init, invertClockPolarity ) ;

  // set timeout loops to maximum, PCI FEC 
  loopInTimeWriteFrame_  = 100000; 
  loopInTimeDirectAck_ = 100000; 
  loopInTimeReadFrame_ = 1000000 ; 
}

/** Close the PCI device driver
 */
FecPciRingDevice::~FecPciRingDevice ( ) throw ( FecExceptionHandler )
{
#ifdef IRQMANAGER
  // Enable the IRQ
  glue_plx_enable_irqs (deviceDescriptor_) ;
#endif

  // Close the device driver
  if (deviceDescriptor_ != -1) 
    close (deviceDescriptor_) ;
}


/******************************************************
	CONTROL & STATUS RTEGISTERS ACCESS
******************************************************/

/** Write the value given as parameter in FEC control 0 register 
 * \param ctrl0Value - value of the CR0
 * \param force - if force is set then the value is applied blindly. if force is not set then the invert clock polarity is managed following the parameter in the class.
 */
void FecPciRingDevice::setFecRingCR0 ( tscType16 ctrl0Value, bool force ) throw ( FecExceptionHandler ) {

  if (!force) {
    if (invertClockPolarity_) ctrl0Value |= FEC_CR0_POLARITY ;
    else ctrl0Value &= (~FEC_CR0_POLARITY) ;
  }

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << ctrl0Value << std::endl ;
#endif

  DD_TYPE_ERROR lcl_err = glue_fec_set_ctrl0(deviceDescriptor_, ctrl0Value);
  if (lcl_err != DD_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FEC control register 0",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
}



/** Reads a value from FEC ctrl0 
 *\return the value read
 */
tscType16 FecPciRingDevice::getFecRingCR0( ) throw ( FecExceptionHandler ) {

  tscType32 ctrl0Value;

  DD_TYPE_ERROR lcl_err = glue_fec_get_ctrl0(deviceDescriptor_, (DD_FEC_REGISTER_DATA*)&ctrl0Value);
#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : reading value 0x" << hex << ctrl0Value << " from CRO" << std::endl ;
#endif

  if (lcl_err != DD_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FEC control register 0",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }

  return (tscType16)ctrl0Value;
}

/** Write the value given as parameter in FEC control 1 register 
 * \param value to be written
 */
void FecPciRingDevice::setFecRingCR1( tscType16 ctrl1Value ) throw ( FecExceptionHandler ) {

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << ctrl1Value << " in CR1" << std::endl ;
#endif

	
  DD_TYPE_ERROR lcl_err = glue_fec_set_ctrl1(deviceDescriptor_, ctrl1Value);

  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FEC control register 1",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
}



/** Reads a value from FEC ctrl1 
 * \return value of the FEC CR1 for that ring
 */
tscType16 FecPciRingDevice::getFecRingCR1( ) throw ( FecExceptionHandler ) {

  tscType32 ctrl1Value;

  DD_TYPE_ERROR lcl_err = glue_fec_get_ctrl1(deviceDescriptor_, (DD_FEC_REGISTER_DATA*)&ctrl1Value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : reading value 0x" << hex << ctrl1Value << " from CR1" << std::endl ;
#endif

  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FEC control register 1",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
  
  return (tscType16)ctrl1Value;
}


/** Reads a value from FEC SR0
 * \return the value of the FEC SR0
 */
tscType32 FecPciRingDevice::getFecRingSR0( ) throw ( FecExceptionHandler ) {

  tscType32 sr0Value;
  
  DD_TYPE_ERROR lcl_err = glue_fec_get_status0(deviceDescriptor_, (DD_FEC_REGISTER_DATA*)&sr0Value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : reading value 0x" << hex << sr0Value << " from SRO (lcl_err = " << dec << lcl_err << ")" << std::endl ;
#endif

  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FEC status register 0",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }

  return (tscType32)sr0Value;
}


/** Reads a value from FEC SR1
 * \return the value of the FEC SR1
 */
tscType16 FecPciRingDevice::getFecRingSR1( ) throw ( FecExceptionHandler ) {

  tscType32 sr1Value;

  DD_TYPE_ERROR lcl_err = glue_fec_get_status1(deviceDescriptor_, (DD_FEC_REGISTER_DATA*)&sr1Value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : reading value 0x" << hex << sr1Value << " from SR1" << std::endl ;
#endif

  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FEC status register 1",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
  return (tscType16)sr1Value;
}

/** Return the version of the firmware
 * \return Firmware version
 */
tscType16 FecPciRingDevice::getFecFirmwareVersion( ) throw ( FecExceptionHandler ) {

  tscType16 fecVersion ;

  DD_TYPE_ERROR lcl_err = glue_fec_get_firmware_version(deviceDescriptor_, (DD_FEC_REGISTER_DATA*)&fecVersion);

#ifdef FECPCIRINGDEVICE_DEBUG
    std::cout << "DEBUG : Firmware version: 0x" << hex << fecVersion << " from FEC" << std::endl ;
#endif

  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to read the FEC firmware version",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }

  return (fecVersion) ;
}

/******************************************************
	FIFO ACCESS - NATIVE 32 BITS FORMAT
	NATIVE FORMAT ACCESS ALLOWS R/W OPERATIONS
******************************************************/

/** Return a word from the FIFO receive
 * \param return a word from the FIFO receive
 */
tscType32 FecPciRingDevice::getFifoReceive( ) throw ( FecExceptionHandler ) {

  DD_FEC_FIFO_DATA_32 fiforec_value;


  DD_TYPE_ERROR lcl_err = glue_fec_get_native_fiforec_item_32(deviceDescriptor_, &fiforec_value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "Value 0x" << hex << fiforec_value << " read from fifo receive" << std::endl ;
#endif

  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FIFO receive",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
  return (tscType32)fiforec_value;
}




/** set a word in the FIFO receive
 * \param fiforecValue - word to be set
 */
void FecPciRingDevice::setFifoReceive( tscType32 fiforec_value )throw ( FecExceptionHandler ) {

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << fiforecValue << " to fifo receive" << std::endl ;
#endif

  DD_TYPE_ERROR lcl_err = glue_fec_set_native_fiforec_item_32(deviceDescriptor_, fiforec_value);
  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FIFO receive",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
}

/** Return a word from the FIFO return
 * \param return a word from the FIFO return
 */
tscType8 FecPciRingDevice::getFifoReturn( ) throw ( FecExceptionHandler ) {

  DD_FEC_FIFO_DATA_32 fiforet_value;

  DD_TYPE_ERROR lcl_err = glue_fec_get_native_fiforet_item_32(deviceDescriptor_, &fiforet_value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "Value 0x" << hex << fiforet_value << " read from fifo return" << std::endl ;
#endif

  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FIFO return",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
  return (tscType8)fiforet_value;
}




/** set a word in the FIFO return
 * \param fiforet_value - word to be set
 */
void FecPciRingDevice::setFifoReturn( tscType8 fiforet_value )throw ( FecExceptionHandler ) {

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << fiforet_value << " to fifo return" << std::endl ;
#endif

  DD_TYPE_ERROR lcl_err = glue_fec_set_native_fiforet_item_32(deviceDescriptor_, fiforet_value);
  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FIFO return",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
}



/** Return a word from the FIFO transmit
 * \param return a word from the FIFO transmit
 */
tscType32 FecPciRingDevice::getFifoTransmit( ) throw ( FecExceptionHandler ) {

  DD_FEC_FIFO_DATA_32 fifotra_value;

  DD_TYPE_ERROR lcl_err = glue_fec_get_native_fifotra_item_32(deviceDescriptor_, &fifotra_value);

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "Value 0x" << hex << fifotra_value << " read from fifo transmit" << std::endl ;
#endif

  if (lcl_err != DD_RETURN_OK) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FIFO transmit",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
  return (tscType32)fifotra_value;
}



/** set a word in the FIFO transmit
 * \param fifotra_value - word to be set
 */
void FecPciRingDevice::setFifoTransmit( tscType32 fifotra_value )throw ( FecExceptionHandler ) {

#ifdef FECPCIRINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << fiforet_value << " to fifo transmit" << std::endl ;
#endif

  DD_TYPE_ERROR lcl_err = glue_fec_set_native_fifotra_item_32(deviceDescriptor_, fifotra_value);

  if (lcl_err != DD_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to access the FIFO transmit",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
}






/******************************************************
		HARD RESET
******************************************************/	

/** Hard reset of the FEC board or the ring board => PLX reset is issued
 */
void FecPciRingDevice::fecHardReset ( ) throw ( FecExceptionHandler ) {

#ifdef F1STLEVELCHECK
  
  // Check if the driver is opened
  if (deviceDescriptor_ <= 0) {
    
    RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_FECDDNOTOPEN,
				      TSCFEC_FECDDNOTOPEN_MSG,
				      FATALERRORCODE,
				      buildFecRingKey (fecSlot, ringSlot),
				      "Error code returned by the ioctl", -deviceDescriptor_) ;
  }
  
#endif

  //usleep(10000) ;
  DD_TYPE_ERROR lcl_err = glue_plx_hard_reset(deviceDescriptor_);
  usleep(10000) ;

  // Check error
  if (lcl_err != DD_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to perform a PLX reset",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }

#ifdef IRQMANAGER
  // Disable the IRQ
  lcl_err = glue_plx_disable_irqs (deviceDescriptor_) ;
#endif

  // Re-initialise all tables used in FecRingDevice
  FecRingDevice::initAfterHardReset() ;
}

/******************************************************
		IRQ enable / disable
******************************************************/	

/** Enable or disable the IRQ
 * \param enable - enable or disable the IRQ on the PLX
 * \param level - not used, only for code consistent
 */
void FecPciRingDevice::setIRQ ( bool enable, tscType8 level ) throw (FecExceptionHandler) {

#ifdef IRQMANAGER
  DD_TYPE_ERROR lcl_err ;

  if (enable) {
    // Enable the IRQ
    lcl_err = glue_plx_enable_irqs (deviceDescriptor_) ;
  }
  else {
    // Disable the IRQ
    lcl_err = glue_plx_disable_irqs (deviceDescriptor_) ;
  }

  if (lcl_err != DD_RETURN_OK) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (lcl_err,
					   "Unable to perform a set the IRQ level",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
#endif
}
