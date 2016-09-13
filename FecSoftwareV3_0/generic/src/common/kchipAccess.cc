/*
This file is part of CMS Preshower Control Software project.

Author: Wojciech BIALAS , CERN, Geneva, Switzerland

based on apvAccess class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include <iostream>

#include "tscTypes.h"

#include "kchipDefinition.h"
#include "kchipAccess.h"

/** Constructor to store the access to the KCHIP3B (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
kchipAccess::kchipAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress )
  : deviceAccess (KCHIP,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress )
  
{

}

/** Constructor to store the access to the KCHIP3B (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
kchipAccess::kchipAccess (FecAccess *fec, 
                      keyType key) 
  : deviceAccess (KCHIP,
                fec,
                key) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
kchipAccess::~kchipAccess ( ) {

  // ~deviceAccess ( ) ;
}

/**
 * \param kchipValues - KCHIP description
 */
void kchipAccess::setDescription ( class kchipDescription& kchipValues ) {   

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = kchipValues.clone() ;
}

/** Take a description value of an KCHIP3B and download the values in a specific KCHIP3B
 * \param kchipValues - all the values for an KCHIP3B
 * \exception FecExceptionHandler
 */
void kchipAccess::setValues ( class kchipDescription& kchipValues ) {
  
  int i ; 
  for(i=0;i<KCHIP_DESC_NUM;i++) { // 
    setValueFromDescription(i,kchipValues.getValue(i)) ; 
  }   

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = kchipValues.clone() ;
}

/** Build a description of an KCHIP3B and return it
 * \return the KCHIP3B description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
kchipDescription *kchipAccess::getValues ( ) {

   kchipDescription *kchipValues = new kchipDescription(getKey());
   if (kchipValues) { 
      int i; 
      for(i=0;i<KCHIP_DESC_NUM;i++) { 
	 try { 
	 kchipValues->setValue(i,getDescriptionValue(i));
	 } catch (FecExceptionHandler& e) { 
	    delete kchipValues; 
	    throw; // i.e. re-throw
	 } 
      }

      kchipValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

   }
  return (kchipValues) ;
}

/** Set the KCHIP3B CR0 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setCR( tscType8 cr ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_CFG_REG, cr) ;
}

/** Set the KCHIP3B CR1 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setECR ( tscType8 ecr ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_ECFG_REG, ecr) ;
}

/** Set the KCHIP3B CalChan0 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setKidLow ( tscType8 kid_l ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_KID_L_REG, kid_l) ;
}


/** Set the KCHIP3B CalChan1 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setKidHigh ( tscType8 kid_h ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_KID_H_REG, kid_h) ;
}

/** Set the KCHIP3B CalChan2 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setMaskT1Cmd ( tscType8 mask_t1cmd ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_MASK_T1CMD_REG, mask_t1cmd) ;
}



/** Set the KCHIP3B VCal register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setLatency ( tscType8 latency  ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_LATENCY_REG, latency) ;
}


void kchipAccess::setGintBusy ( tscType8 gintbusy ) {
  accessToFec_->writeOffset (accessKey_, KCHIP_GINT_BUSY_REG, gintbusy) ;
}

/** Set the APV Isha
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setGintIdle ( tscType8 gintidle ) {

  accessToFec_->write (accessKey_, KCHIP_GINT_IDLE_REG, gintidle) ;
}

/** Set the APV Issf
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setFifoMap ( tscType8 fifomap ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_FIFOMAP_REG, fifomap) ;
}

/** Set the APV Ipsp
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setFifoDataLow ( tscType8 fifodata_l ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_FIFODATA_L_REG, fifodata_l) ;
}
/** Set the APV Ipsp
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setFifoDataHigh ( tscType8 fifodata_h ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_FIFODATA_H_REG, fifodata_h) ;
}

/** Set the APV Imuxin
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setCalPulseDelay ( tscType8 calpulse_delay ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_CALPULSE_DELAY_REG, calpulse_delay) ;
}
/** Set the APV Imuxin
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setCalPulseWidth ( tscType8 calpulse_width ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_CALPULSE_DELAY_REG, calpulse_width) ;
}
/** Set the APV Imuxin
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setAdcLatency ( tscType8 adclatency ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_ADC_LATENCY_REG, adclatency) ;
}
/** Set the APV Imuxin
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setPFifoOverflow ( tscType8 pfifo_overflow ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_PFIFO_OVERFLOW_REG, pfifo_overflow) ;
}


void kchipAccess::setValueFromDescription(int reg, tscType8 val) { 
   int physicalReg = lookup_[reg] ; // lookup table will never point to reserved registers...
  //cout << "Writing desc reg. " << reg << " physical reg. " << physicalReg ; 
  //cout << " value: " << (int) val << endl ;  
  if (!ro_[physicalReg] & !debugonly_[physicalReg]) { // if not read-only, nor debug only 
    accessToFec_->writeOffset (accessKey_,physicalReg, val) ;
    //cout << "Done." << endl ; 
  } else { 
     //cout << "sorry it is RO." << endl ;  
  }
} 

void kchipAccess::setValue(int reg, tscType8 val) { 
  if (!ro_[reg] & !debugonly_[reg] & !reserved_[reg]) { // if not read-only
    accessToFec_->writeOffset (accessKey_,reg, val) ;
  } 
} 

/** Get APV Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getValue (int reg )  {
   if (!debugonly_[reg] & !reserved_[reg]) 
      return (accessToFec_->readOffset (accessKey_, reg)) ;
   else 
      return 0 ;  
}
/** Get APV Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getMaskedValue (int reg )  {
   if (!debugonly_[reg] & !reserved_[reg])
      return (rmask_[reg] & (accessToFec_->readOffset (accessKey_,reg))) ;
   else 
      return 0 ; 
}
tscType8 kchipAccess::getDescriptionValue (int reg )  {
  int physicalReg = lookup_[reg] ;
  if (!debugonly_[physicalReg]) 
         return (accessToFec_->readOffset (accessKey_,physicalReg)) ;
  else 
     return 0 ; 
}
/** Get APV Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getDescriptionMaskedValue (int reg )  {
  int physicalReg = lookup_[reg];
  return (rmask_[physicalReg] & (accessToFec_->readOffset(accessKey_,physicalReg))) ;
}





/** Get APV Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getCR ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CFG_REG)) ;
}

/** Get APV Mode
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getECR ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_ECFG_REG)) ;
}

/** Get APV Latency
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getChipID0 ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CHIPID0_REG)) ;
}

/** Get APV Mux Gain
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getChipID1 ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CHIPID1_REG)) ;
}

/** Get KCHIP3B calchan0
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getKidLow ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_KID_L_REG)) ;
}
/** Get KCHIP3B calchan1
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getKidHigh ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_KID_H_REG )) ;
}
/** Get KCHIP3B calchan2
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getMaskT1Cmd ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_MASK_T1CMD_REG)) ;
}
/** Get KCHIP3B calchan3
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getLastT1Cmd ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_LAST_T1CMD_REG)) ;
}

/** Get APV Ipcasc
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getLatency ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_LATENCY_REG)) ;
}

/** Get APV Ipsf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getEvCnt ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_EVCNT_REG)) ;
}

/** Get APV Isha
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getBnchCntLow ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_BNCHCNT_L_REG)) ;
}

/** Get APV Issf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getBnchCntHigh ( ) {

  return (accessToFec_->readOffset (accessKey_,KCHIP_BNCHCNT_H_REG)) ;
}

/** Get APV Ipsp
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getGintBusy ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_GINT_BUSY_REG)) ;
}

/** Get APV Imuxin
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getGintIdle ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_GINT_IDLE_REG)) ;
}

/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getFifoMap ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_FIFOMAP_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getFifoDataLow ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_FIFODATA_L_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getFifoDataHigh ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_FIFODATA_H_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getSR0 ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_SR0_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getSR1 ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_SR1_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getSeuCounter ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_SEU_COUNTER_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getCalPulseDelay ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CALPULSE_DELAY_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getCalPulseWidth ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CALPULSE_WIDTH_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getAdcLatency ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_ADC_LATENCY_REG)) ;
}
/** Get APV Ical
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getPFifoOverflow ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_PFIFO_OVERFLOW_REG)) ;
}


const char* kchipAccess::names[] = { 
  "Config" , "Extended Config" , "KID low","KID high",
  "Mask T1","Last T1","Latency","Event Counter","Bunch Counter low",
  "Bunch Counter high", "Reserved (0xA)","GOL interface Busy",
  "GOL interface IDLE","Fifo Map","Fifo Data low","Fifo Data high",
  "Status 0","Status 1","SEU Counter","Cal. Pulse Delay","Cal. Pulse Width",
  "ADC latency","Pace Fifo Overflow","Reserved (0x17)","Reserved (0x18)",
  "Reserved (0x19)","Reserved (0x1a)","Reserved (0x1b)","Reserved (0x1c)",
  "Reserved (0x1d)","ChipID 0","ChipID 1" } ; 
			    
const tscType8 kchipAccess::rmask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff 

} ; // i.e. all bits are checked 

const bool kchipAccess::ro_[] = { 
  false,false,false,false,false, true,false, true,
   true, true,false,false,false,false,false,false, 
   true, true, true,false,false,false,false,false,
  false,false,false,false,false,false, true, true 

};

const bool kchipAccess::reserved_[] = { 
   false,false,false,false,false,false,false,false,
   false,false, true,false,false,false,false,false,
   false,false,false,false,false,false,false, true,
    true, true, true, true, true, true,false,false
};

const bool kchipAccess::debugonly_[] = { 
  false,false,false,false,false,false,false,false,
  false,false,false,false,false, true, true, true,
  false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false
};



const int kchipAccess::lookup_[] = { 
  KCHIP_CFG_REG,
  KCHIP_ECFG_REG, 
  KCHIP_KID_L_REG, 
  KCHIP_KID_H_REG,
  KCHIP_MASK_T1CMD_REG,
  KCHIP_LAST_T1CMD_REG,
  KCHIP_LATENCY_REG, 
  KCHIP_EVCNT_REG,
  KCHIP_BNCHCNT_L_REG,
  KCHIP_BNCHCNT_H_REG, 
  KCHIP_GINT_BUSY_REG,
  KCHIP_GINT_IDLE_REG, 
//   KCHIP_FIFOMAP_REG, 
//   KCHIP_FIFODATA_H_REG,
//   KCHIP_FIFODATA_L_REG,
  KCHIP_SR0_REG,
  KCHIP_SR1_REG,
  KCHIP_SEU_COUNTER_REG,
  KCHIP_CALPULSE_DELAY_REG,
  KCHIP_CALPULSE_WIDTH_REG,
  KCHIP_ADC_LATENCY_REG,
  KCHIP_PFIFO_OVERFLOW_REG,
  KCHIP_CHIPID0_REG,
  KCHIP_CHIPID1_REG
} ; 

