
/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef PACEACCESS_H
#define PACEACCESS_H

#include "FecAccess.h"
#include "deviceAccess.h"
#include "paceDescription.h"

/**
 * \class paceAccess
 * This class defines an DELTA3B and make possible the downloading or
 * uploading of the values related to a specific register.
 * It also implements all the method specific to the DELTA device.
 * Based on apvAccess class by Frederic DROUHIN - Universite de Haute-Alsace,
 * Mulhouse-France
 * \author Wojciech BIALAS
 * \date March 2004
 * \warning All the I2C accesses are done in extended/ral mode
 * \warning An offset of 1 is added to the address when a read is performed
 * \brief This class define all the hardware accesses for the DELTA3B througth the FecAccess class
 */
class paceAccess : public deviceAccess {


 private: 

  static const char *names[] ;
  static const bool ro_[] ;
  static const tscType8 rmask_[] ;  

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  paceAccess (FecAccess *fec, 
             tscType16 fecSlot,
             tscType16 ringSlot,
             tscType16 ccuAddress,
             tscType16 i2cChannel,
             tscType16 i2cAddress ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  paceAccess (FecAccess *fec, 
             keyType key ) ;

  /** Remove the access 
   */
  ~paceAccess () ;

  /** \brief set the description for future comparison
   */
  void setDescription ( class paceDescription& deltaValues );

  /** \brief Set all values from an DELTA3B description
   */
  void setValues ( class paceDescription& deltaValues );

  /** \brief Get all values from an DELTA3B

   */
  paceDescription *getValues ( ) ;



  /** \brief Set the value in the specified register
   */
  void setValue(int reg,  tscType8 val );



  /** \brief Set the value in the specified register
   */
  void setCR ( tscType8 cr0 );

  /** \brief Set the value in the specified register
   */
  void setLatency ( tscType8 lat  );

 

  /** \brief Set the value in the specified register
   */
  void setVmemref ( tscType8 vmemref ) ; 

  /** \brief Set the value in the specified register
   */
  void setVshift ( tscType8 vshift ) ;

  /** \brief Set the value in the specified register
   */
  void setVoutbuf ( tscType8 voutbuf ) ;

  /** \brief Set the value in the specified register
   */
  void setIreadamp ( tscType8 ireadamp ) ;

  /** \brief Set the value in the specified register
   */
  void setIshift ( tscType8 ishift ) ;

  /** \brief Set the value in the specified register
   */
  void setImuxbuf ( tscType8 imuxbuf ) ;

  /** \brief Set the value in the specified register
   */
  void setIoutbuf ( tscType8 ioutbuf ) ;

  /** \brief Set the value in the specified register
   */
  void setIspare ( tscType8 ispare ) ;





   
 /** \brief Get the value in the specified register
   */
  tscType8 getValue (int reg) ;
/** \brief Get the value in the specified register
   */
  tscType8 getMaskedValue (int reg) ;
  
  /** \brief Get the value in the specified register
   */
  tscType8 getCR ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getLatency  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getChipID0  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getChipID1  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVmemref     ( ) ;
  /** \brief Get the value in the specified register
   */
  tscType8 getVshift     ( ) ;
  /** \brief Get the value in the specified register
   */
  tscType8 getVoutbuf     ( ) ;
  /** \brief Get the value in the specified register
   */
  tscType8 getIreadamp     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIshift   ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getImuxbuf     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIoutbuf     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIspare    ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getUpsetReg     ( ) ;

 

  
} ;

#endif
