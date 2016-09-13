/*
This file is part of CMS Preshower  Control Software project.

.



Copyright 2004, Wojciech BIALAS , CERN , Geneva , Switzerland 
*/
#ifndef DELTAACCESS_H
#define DELTAACCESS_H

#include "deviceType.h"
#include "deviceFrame.h"

#include "FecAccess.h"
#include "deviceAccess.h"
#include "deltaDescription.h"


/**
 * \class deltaAccess
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
class deltaAccess : public deviceAccess {


 private: 

  static const char *names[] ;
  static const bool ro_[] ;
  static const tscType8 rmask_[] ;  

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  deltaAccess (FecAccess *fec, 
             tscType16 fecSlot,
             tscType16 ringSlot,
             tscType16 ccuAddress,
             tscType16 i2cChannel,
             tscType16 i2cAddress ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  deltaAccess (FecAccess *fec, 
             keyType key ) ;

  /** Remove the access 
   */
  ~deltaAccess () ;

  /** \brief set the description for future comparison
   */
  void setDescription ( class deltaDescription& deltaValues );

  /** \brief Set all values from an DELTA3B description
   */
  void setValues ( class deltaDescription& deltaValues );

  /** \brief Get all values from an DELTA3B

   */
  deltaDescription *getValues ( ) ;



  /** \brief Set the value in the specified register
   */
  void setValue(int reg,  tscType8 val );



  /** \brief Set the value in the specified register
   */
  void setCR0( tscType8 cr0 );

  /** \brief Set the value in the specified register
   */
  void setCR1 ( tscType8 cr1  );

  /** \brief Set the value in the specified register
   */
  void setCalChan0 ( tscType8 CalChan0  );
  
  
  /** \brief Set the value in the specified register
   */
  void setCalChan1 ( tscType8 CalChan1  );

  /** \brief Set the value in the specified register
   */
  void setCalChan2 ( tscType8 CalChan2  );

  /** \brief Set the value in the specified register
   */
  void setCalChan3 ( tscType8 CalChan3  );


  /** \brief Set the value in the specified register
   */
  void setVcal ( tscType8 VCal ) ; 

  /** \brief Set the value in the specified register
   */
  void setVoPreamp ( tscType8 VoPreamp ) ;

  /** \brief Set the value in the specified register
   */
  void setVoShaper ( tscType8 VoShaper ) ;

  /** \brief Set the value in the specified register
   */
  void setVspare ( tscType8 VSpare ) ;

  /** \brief Set the value in the specified register
   */
  void setIpreamp ( tscType8 IPreamp ) ;

  /** \brief Set the value in the specified register
   */
  void setIshaper ( tscType8 IShaper ) ;

  /** \brief Set the value in the specified register
   */
  void setISF ( tscType8 ISF ) ;

  /** \brief Set the value in the specified register
   */
  void setIspare ( tscType8 ISpare ) ;
   
 /** \brief Get the value in the specified register
   */
  tscType8 getValue (int reg) ;
/** \brief Get the value in the specified register
   */
  tscType8 getMaskedValue (int reg) ;
  
  /** \brief Get the value in the specified register
   */
  tscType8 getCR0 ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getCR1  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getChipID0  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getChipID1  ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getCalChan0     ( ) ;
  /** \brief Get the value in the specified register
   */
  tscType8 getCalChan1     ( ) ;
  /** \brief Get the value in the specified register
   */
  tscType8 getCalChan2     ( ) ;
  /** \brief Get the value in the specified register
   */
  tscType8 getCalChan3     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVcal   ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVoPreamp     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVoShaper     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getVspare     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIpreamp     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIshaper   ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getISF     ( ) ;

  /** \brief Get the value in the specified register
   */
  tscType8 getIspare     ( ) ;

  /** \brief Retreive a block of frames to download the device
   */
  void getBlockWriteValues ( class deltaDescription& deltaValues, accessDeviceTypeList &vAccess ) ;

  /** \brief static method to retreive the data from a set of delta
   */
  static unsigned int getDeltaValuesMultipleFrames ( FecAccess &fecAccess, Sgi::hash_map<keyType, deltaAccess *> &deltaSet, deviceVector &deltaVector,
								  std::list<FecExceptionHandler *> &errorList ) 
    throw (FecExceptionHandler) ;
  
} ;

#endif
