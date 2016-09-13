
/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef GOHACCESS_H
#define GOHACCESS_H

#include "FecAccess.h"
#include "deviceAccess.h"
#include "gohDescription.h"

#define GOH_ADDRESS_OFFSET 0x00
#define GOH_DATA_OFFSET    0x01


/**
 * \class gohAccess
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
class gohAccess : public deviceAccess {


 private: 

  static const char *names[] ;
  static const bool ro_[] ;
  static const tscType8 rmask_[] ;  

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  gohAccess (FecAccess *fec, 
             tscType16 fecSlot,
             tscType16 ringSlot,
             tscType16 ccuAddress,
             tscType16 i2cChannel,
             tscType16 i2cAddress ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  gohAccess (FecAccess *fec, 
             keyType key ) ;

  /** Remove the access 
   */
  ~gohAccess () ;

  /** \brief set the description for future comparison
   */
  void setDescription ( class gohDescription& gohValues );

  /** \brief Set all values from an GOH description
   */
  void setValues ( class gohDescription& gohValues );

  /** \brief Get all values from an GOH hybrid

   */
  gohDescription *getValues ( ) ;



  /** \brief Set the value in the specified register
   */
  void setValue(int reg,  tscType8 val );



  /** \brief Set the value in the specified register
   */
  void setControl0 ( tscType8 cr );

  /** \brief Set the value in the specified register
   */
  void setControl1 ( tscType8 cr  );

 

  /** \brief Set the value in the specified register
   */
  void setControl2 ( tscType8 cr ) ; 

  /** \brief Set the value in the specified register
   */
  void setControl3 ( tscType8 cr ) ;


   
 /** \brief Get the value in the specified register
   */
  tscType8 getValue (int reg) ;
/** \brief Get the value in the specified register
   */
  tscType8 getMaskedValue (int reg) ;
  
  /** \brief Get the value in the specified register
   */
  tscType8 getControl0 ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl1  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl2 ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getControl3 ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getStatus0    ( ) ;
  /** \brief Get the value in the specified register
   */
  tscType8 getStatus1    ( ) ;


  
} ;

#endif
