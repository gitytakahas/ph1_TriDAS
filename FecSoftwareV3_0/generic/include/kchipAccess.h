/*
This file is part of CMS Preshower  Control Software project.

.



Copyright 2004, Wojciech BIALAS , CERN , Geneva , Switzerland 
*/
#ifndef KCHIPACCESS_H
#define KCHIPACCESS_H

#include "FecAccess.h"
#include "deviceAccess.h"
#include "kchipDescription.h"

/**
 * \class kchipAccess
 * This class defines an KCHIP3B and make possible the downloading or
 * uploading of the values related to a specific register.
 * It also implements all the method specific to the KCHIP device.
 * Based on apvAccess class by Frederic DROUHIN - Universite de Haute-Alsace,
 * Mulhouse-France
 * \author Wojciech BIALAS
 * \date March 2004
 * \warning All the I2C accesses are done in extended/ral mode
 * \warning An offset of 1 is added to the address when a read is performed
 * \brief This class define all the hardware accesses for the KCHIP3B througth the FecAccess class
 */
class kchipAccess : public deviceAccess {


 private: 

  static const char *names[] ;
  static const bool ro_[] ;
  static const bool reserved_[];
  static const bool debugonly_[];
  static const tscType8 rmask_[] ; 
  static const int lookup_[] ;  

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  kchipAccess (FecAccess *fec, 
             tscType16 fecSlot,
             tscType16 ringSlot,
             tscType16 ccuAddress,
             tscType16 i2cChannel,
             tscType16 i2cAddress ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  kchipAccess (FecAccess *fec, 
             keyType key ) ;

  /** Remove the access 
   */
  ~kchipAccess () ;

  /** \brief set the description for future comparison
   */
  void setDescription ( class kchipDescription& kchipValues );

  /** \brief Set all values from an KCHIP3B description
   */
  void setValues ( class kchipDescription& kchipValues );

  /** \brief Get all values from an KCHIP3B

   */
  kchipDescription *getValues ( ) ;


  void setCR( tscType8 cr );

  void setECR ( tscType8 ecr );

  void setKidLow ( tscType8 kid_l ); 

  void setKidHigh ( tscType8 kid_h ); 

  void setMaskT1Cmd ( tscType8 mask_t1cmd ); 

  void setLatency ( tscType8 latency  ); 

  void setGintBusy ( tscType8 gintbusy ); 

  void setGintIdle ( tscType8 gintidle );

  void setFifoMap ( tscType8 fifomap ); 

  void setFifoDataLow ( tscType8 fifodata_l );

  void setFifoDataHigh ( tscType8 fifodata_h );

  void setCalPulseDelay ( tscType8 calpulse_delay );

  void setCalPulseWidth ( tscType8 calpulse_width );

  void setAdcLatency ( tscType8 adclatency );

  void setPFifoOverflow ( tscType8 pfifo_overflow );

  void setValueFromDescription(int reg, tscType8 val);

  void setValue(int reg, tscType8 val);

  tscType8 getValue (int reg );

  tscType8 getMaskedValue (int reg );

  tscType8 getDescriptionValue (int reg );

  tscType8 getDescriptionMaskedValue (int reg );

  tscType8 getCR ( );

  tscType8 getECR ( );

  tscType8 getChipID0 ( ); 

  tscType8 getChipID1 ( );

  tscType8 getKidLow ( );

  tscType8 getKidHigh ( );

  tscType8 getMaskT1Cmd ( );

  tscType8 getLastT1Cmd ( ); 

  tscType8 getLatency ( );

  tscType8 getEvCnt ( );

  tscType8 getBnchCntLow ( ); 

  tscType8 getBnchCntHigh ( );

  tscType8 getGintBusy ( );

  tscType8 getGintIdle ( );

  tscType8 getFifoMap ( );

  tscType8 getFifoDataLow ( );

  tscType8 getFifoDataHigh ( );

  tscType8 getSR0 ( ) ;

  tscType8 getSR1 ( );

  tscType8 getSeuCounter ( );

  tscType8 getCalPulseDelay ( );

  tscType8 getCalPulseWidth ( );

  tscType8 getAdcLatency ( );

  tscType8 getPFifoOverflow ( );

} ;

#endif
