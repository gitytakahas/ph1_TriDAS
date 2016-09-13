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

#ifndef FECINTERFACERINGDEVICE_H
#define FECINTERFACERINGDEVICE_H

#include "tscTypes.h"

class FecInterfaceRingDevice {

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

 protected:

  /** used by frame handling functions, to auto-set the transaction number  
   */	
  tscType8 transactionNumber_;

 public:

  /** \brief Build an interface to the correct FEC (PCI, VME, USB)
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot on the corresponding FEC
   * \param fecType - type of the FEC (PCI, VME, USB)
   */
  FecInterfaceRingDevice ( tscType8 fecSlot, tscType8 ringSlot, enumFecBusType fecType ) {

    fecSlot_ = fecSlot ;
    ringSlot_ = ringSlot ;
    fecType_ = fecType ;
  }

  /** \brief To retreive the FEC slot
   * \return FEC slot
   */
  tscType8 getFecSlot ( ) {

    return (fecSlot_) ;
  }

  /** \brief To retreive the RING slot
   * \return RING slot
   */
  tscType8 getRingSlot ( ) {

    return (ringSlot_) ;
  }

  /** \brief To retreive the FEC type
   * \return FEC type (PCI, VME, USB)
   */
  tscType8 getFecType ( ) {

    return (fecType_) ;
  }

  /** \brief return the current transaction number
   */
  tscType8 getTransactionNumber ( ) {

    return (transactionNumber_) ;
  }

  /** \brief return the current transaction number
   * and increment it 
   */
  tscType8 getNextTransactionNumber ( ) {

    transactionNumber_ ++ ;

    if (transactionNumber_ == 0x80) transactionNumber_ = 1 ;

    return (transactionNumber_) ;
  }

  /** \brief return the current transaction number
   */
  void setTransactionNumber ( tscType8 transactionNumber ) {

    transactionNumber_ = transactionNumber ;
  }
  
  /******************************************************
	Virtual methods for the FEC
  ******************************************************/

  /** \brief Set the control register 0  the FEC
   * \param ctrl0Value - value to be set
   */ 
  virtual void setFecRingCR0( tscType16 ctrl0Value ) = 0 ;

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
   virtual tscType16 getFecRingSR0( ) = 0 ;

  /** \brief Get the status register 1  the FEC
   * \return value read
   */
   virtual tscType16 getFecRingSR1( ) = 0 ;

   /** \brief Get the FEC version register 
   * \return value read
   */
   virtual tscType16 getFecFirmwareVersion( ) throw ( FecExceptionHandler ) ;
	
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
  virtual void setIRQ ( bool enable, tscType8 level ) = 0 ;

   /******************************************************
    * The next methods is able to be overloaded in the sub-classes in order to implement
    * the block mode transfer.
    * \author W. Bialas
    ******************************************************/	

  /** \brief write a block of words to the FIFO receive
   */
  virtual void setFifoReceive ( tscType32 *value, int count) throw (FecExceptionHandler) {
    int i;
    for(i=0;i<count;i++) setFifoReceive(value[i]) ;
  };

  /** \brief get block of words from the FIFO receive
   */
  virtual tscType32* getFifoReceive (  tscType32 *value, int count ) throw (FecExceptionHandler) {
    int i;
    for(i=0;i<count;i++) value[i] = getFifoReceive() ;
    return value ;
  };
  
  /** \brief set a word into the FIFO return
   */
  virtual void setFifoReturn ( tscType8 *value, int count ) throw (FecExceptionHandler) {
     int i;
     for(i=0;i<count;i++) setFifoReturn(value[i]) ;
  } ;
  
  /** \brief get a word from the FIFO return
   */
  virtual tscType8* getFifoReturn (  tscType8 *value,int count ) throw (FecExceptionHandler) {
    int i;
    for(i=0;i<count;i++) value[i]  = getFifoReturn() ;
    return value ;
  };
  
  /** \brief set a word into the FIFO Transmit
   */
  virtual void setFifoTransmit ( tscType32 *value, int count ) throw (FecExceptionHandler) {
    int i;
    for(i=0;i<count;i++) setFifoTransmit(value[i]) ;
  };
  
  /** \brief get a word from the FIFO transmit
   */
  virtual tscType32* getFifoTransmit (  tscType32 *value,int count ) throw (FecExceptionHandler) {
    int i;
    for(i=0;i<count;i++) value[i]  = getFifoTransmit() ;
    return value ;
  };
} ;

#endif
