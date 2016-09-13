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

  Copyright 2004, Wojciech BIALAS, CERN, Geneva, Switzerland

*/
#ifndef FECUSBRINGDEVICE_H
#define FECUSBRINGDEVICE_H

//System includes
#include <cstdlib>

// FecSoftware includes
#include "tscTypes.h"
#include "fec_usb.h"
#include "FecExceptionHandler.h"
#include "FecRingDevice.h"



//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <termios.h>
//#include <stdio.h>

// some typical values        




/** This class gives all the methods to read / write a word in the different registers of the USB FEC ie.:
 * FIFO receive, FIFO transmit, FIFO return, control register 0 and 1, status register 0 and 1
 * This class inherits from FecRingDevice in order to have the same methods for the type of FECs: VME, PCI, USB
 * This class inherits from FecRingDevice which contains all the high level methods for CCU, channels (i2c, pia, memory) 
 */
class FecUsbRingDevice: public FecRingDevice {

 private:
  /* file descriptor form pseudo serial port initialized in/by constructor  */
  int currentPortFD_;
  int device_ ; 
  char devName_[256] ;   

 public:

  /** Number of slot in the USB FEC bus
   */
  static const unsigned int minUsbFecSlot ;
  /** Number of slot in the FEC USB bus
   */
  static const unsigned int maxUsbFecSlot ;
  /** Number of ring per USB FEC
   */
  static const unsigned int maxUsbFecRing ;  
  /** Number of ring per USB FEC
   */
  static const unsigned int minUsbFecRing ;

      /** addresses of fifos , these vary according to 
       fec_core firmware version */
      tscType32 fifoTransmitAddress ; 
      tscType32 fifoReturnAddress ;
      tscType32 fifoReceiveAddress ;      

  /******************************************************
			CONSTRUCTOR - DESTRUCTOR
  ******************************************************/

  /** \brief Build an access to the PCI FEC
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot on the corresponding FEC
   * \param init - initialise or not the FEC ring
   */
  FecUsbRingDevice (std::string serial, tscType8 fecSlot, tscType8 ringSlot, bool init = true, bool invertClockPolarity = false ) throw ( FecExceptionHandler ) ;
	
  //Destructor
  virtual ~FecUsbRingDevice() throw ( FecExceptionHandler );


  /******************************************************
			BOARD RESET & CONFIGURATION FUNCTIONS
  ******************************************************/

  // hm.. may be some functions
  




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
  tscType32 getFecRingSR0( ) throw ( FecExceptionHandler );

  /** \brief Get the status register 1  the FEC
   * \return value read
   */
  tscType16 getFecRingSR1( ) throw ( FecExceptionHandler );
	


  /** \brief Get the FEC version register the FEC
   * \return value read
   */
  tscType16 getFecFirmwareVersion( )  throw ( FecExceptionHandler );
	
	
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
  tscType8 getFifoReturn ( ) throw ( FecExceptionHandler );

  /** \brief write a word in the FIFO return
   * \param value - value to be written
   */
  void setFifoReturn ( tscType8 fiforetValue ) throw ( FecExceptionHandler );

  /** \brief return a word from the FIFO transmit
   */
  tscType32 getFifoTransmit ( ) throw ( FecExceptionHandler );

  /** \brief write a word in the FIFO transmit
   * \param value - value to be written
   */
  void setFifoTransmit ( tscType32 fifotraValue ) throw ( FecExceptionHandler );

  /** \brief Use USB block transfer to write a set of values inside the FIFO
   * \param value - values to be written (array)
   * \param count - number of values
   */
  void setFifoReceive ( tscType32 *value, int count) 
    throw (FecExceptionHandler);

  /** \brief Use USB block transfer to read values from the FIFO
   * \param value - value to be read
   * \param count - number of value to be read
   * \return pointer to the array of value ( = value )
   */
  tscType32* getFifoReceive (  tscType32 *value, int count ) 
    throw (FecExceptionHandler);

  /** \brief Use USB block transfer to word a set of values inside the FIFO
   * \param value - values to be written (array)
   * \param count - number of values
   */
  void setFifoReturn ( tscType8 *value, int count ) 
    throw (FecExceptionHandler);

  /** \brief Use USB block transfer to read values from the FIFO
   * \param value - value to be read
   * \param count - number of value to be read
   * \return pointer to the array of value ( = value )
   */
  tscType8* getFifoReturn (  tscType8 *value,int count ) 
    throw (FecExceptionHandler);

  /** \brief Use USB block transfer to word a set of values inside the FIFO
   * \param value - values to be written (array)
   * \param count - number of values
   */
  void setFifoTransmit ( tscType32 *value, int count ) 
    throw (FecExceptionHandler);

  /** \brief Use USB block transfer to read values from the FIFO
   * \param value - value to be read
   * \param count - number of value to be read
   * \return pointer to the array of value ( = value )
   */
  tscType32* getFifoTransmit (  tscType32 *value,int count ) 
    throw (FecExceptionHandler);
      
      
      

	
  /******************************************************
		HARD RESET
  ******************************************************/	

  /** \brief Issue a hard reset
   */
  void fecHardReset ( ) throw ( FecExceptionHandler ) ;
      
  /******************************************************
		IRQ enable / disable
  ******************************************************/	

  /** \brief enable or disable the IRQ, not use in the USB FEC
   */
  void setIRQ ( bool enable, tscType8 level=1 ) throw ( FecExceptionHandler ) ;

} ;

#endif
