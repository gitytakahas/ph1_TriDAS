/*
This file is part of CMS Preshower Control Software project.

Author: Wojciech BIALAS , CERN, Geneva, Switzerland

based on apvAccess class by
Frederic DROUHIN - Universite de Haute-Alsace,
Mulhouse-France
*/
#include <iostream>

#include "tscTypes.h"

#include "paceDefinition.h"
#include "paceAccess.h"

/** Constructor to store the access to the PACE3B (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
paceAccess::paceAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress )
  : deviceAccess (PACE,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress )
  
{

}

/** Constructor to store the access to the PACE3B (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
paceAccess::paceAccess (FecAccess *fec, 
                      keyType key) 
  : deviceAccess (PACE,
                fec,
                key) {

}



/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
paceAccess::~paceAccess ( ) {

  // ~deviceAccess ( ) ;
}

/** 
 * \param paceValues - PACE description
 */
void paceAccess::setDescription ( class paceDescription& paceValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = paceValues.clone() ;
}

/** Take a description value of an PACE3B and download the values in a specific PACE3B
 * \param paceValues - all the values for an PACE3B
 * \exception FecExceptionHandler
 */
void paceAccess::setValues ( class paceDescription& paceValues ) {
  
  int i ; 
  for(i=0;i<PACE_REG_NUM;i++) { 
    if (!ro_[i]) setValue(i,paceValues.getValue(i)) ; 
  }   

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = paceValues.clone() ;
}

/** Build a description of an PACE3B and return it
 * \return the PACE3B description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
paceDescription *paceAccess::getValues ( ) {

   paceDescription *paceValues = new paceDescription(getKey()); 

   if (paceValues) { 
      int i; 
      for(i=0;i<PACE_DESC_NUM;i++) { 
	 try { 
	 paceValues->setValue(i,getValue(i));
	 } catch (FecExceptionHandler& e) { 
	    delete paceValues; 
	    throw; // i.e. re-throw
	 } 
      }
      paceValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;
   }

  return (paceValues) ;
}

/** Set the PACE3B CR register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setCR( tscType8 cr0 ) {

   accessToFec_->write (accessKey_, (PACE_CR_REG<<1), cr0) ;
}

/** Set the PACE3B latency register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setLatency ( tscType8 latency ) {

   accessToFec_->write (accessKey_, (PACE_LATENCY_REG<<1), latency) ;
}

/** Set the PACE3B Vmemref register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setVmemref ( tscType8 vmemref ) {

   accessToFec_->write (accessKey_, (PACE_VMEMREF_REG<<1), vmemref) ;
}


/** Set the PACE3B Vshift register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setVshift ( tscType8 vshift ) {

   accessToFec_->write (accessKey_, (PACE_VSHIFT_REG<<1), vshift) ;
}

/** Set the PACE3B Voutbuf register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setVoutbuf ( tscType8 voutbuf ) {

   accessToFec_->write (accessKey_, (PACE_VOUTBUF_REG<<1), voutbuf) ;
}

/** Set the PACE3B Ireadamp register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setIreadamp ( tscType8 iread ) {

   accessToFec_->write (accessKey_, (PACE_IREADAMP_REG<<1), iread) ;
}


/** Set the PACE3B Ishift register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setIshift ( tscType8 ishift  ) {

   accessToFec_->write (accessKey_, PACE_ISHIFT_REG<<1, ishift) ;
}

/** Set the PACE3B Imuxbuf
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setImuxbuf ( tscType8 imux ) {

   accessToFec_->write (accessKey_, PACE_IMUXBUF_REG<<1, imux) ;
}

/** Set the PACE3B Ioutbuf
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setIoutbuf ( tscType8 ibuf ) {

   accessToFec_->write (accessKey_, PACE_IOUTBUF_REG<<1, ibuf) ;
}

/** Set the PACE3B Ispare (not used..)
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void paceAccess::setIspare ( tscType8 ispare ) {

   accessToFec_->write (accessKey_, PACE_ISPARE_REG<<1, ispare) ;
}




void paceAccess::setValue(int reg, tscType8 val) { 
  if (!ro_[reg]) { // if not read-only
    accessToFec_->write (accessKey_, (reg << 1), val) ;
  } 
} 

/** Get PACE3B Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getValue (int reg )  {

  return (accessToFec_->read (accessKey_, (reg<<1) | PACE_READ)) ;
}
/** Get PACE3B Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getMaskedValue (int reg )  {

  return (rmask_[reg] & (accessToFec_->read (accessKey_, (reg<<1) | PACE_READ))) ;
}





/** Get PACE3B control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getCR ( )  {

   return (accessToFec_->read (accessKey_, (PACE_CR_REG<<1) | PACE_READ)) ;
}

/** Get PACE3B latency
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getLatency ( ) {

   return (accessToFec_->read (accessKey_, (PACE_LATENCY_REG<<1) | PACE_READ)) ;
}

/** Get PACE3B chip ID 0 
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getChipID0 ( )  {

   return (accessToFec_->read (accessKey_,(PACE_CHIPID0_REG<<1) | PACE_READ)) ;
}

/** Get PACE3B chip ID 1 
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getChipID1 ( )  {

   return (accessToFec_->read (accessKey_,(PACE_CHIPID1_REG<<1)|PACE_READ)) ;
}

/** Get PACE3B Vmemref
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getVmemref ( ) {

   return (accessToFec_->read (accessKey_,(PACE_VMEMREF_REG<<1) | PACE_READ)) ;
}
/** Get PACE3B Vshift
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getVshift ( ) {

   return (accessToFec_->read (accessKey_,(PACE_VSHIFT_REG<<1)|PACE_READ)) ;
}
/** Get PACE3B Voutbuf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getVoutbuf ( ) {

   return (accessToFec_->read (accessKey_,(PACE_VOUTBUF_REG<<1)|PACE_READ)) ;
}
/** Get PACE3B Ireadamp
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getIreadamp ( ) {

   return (accessToFec_->read (accessKey_,(PACE_IREADAMP_REG<<1)|PACE_READ)) ;
}

/** Get PACE3B Ishift
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getIshift ( )  {

   return (accessToFec_->read (accessKey_,(PACE_ISHIFT_REG<<1)|PACE_READ)) ;
}

/** Get PACE3B Imuxbuf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getImuxbuf ( ) {

   return (accessToFec_->read (accessKey_,(PACE_IMUXBUF_REG<<1)|PACE_READ)) ;
}

/** Get PACE3B Ioutbuf
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getIoutbuf ( )  {

   return (accessToFec_->read (accessKey_,(PACE_IOUTBUF_REG<<1)|PACE_READ)) ;
}

/** Get PACE3B Ispare (not used...) 
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getIspare ( ) {

   return (accessToFec_->read (accessKey_,(PACE_ISPARE_REG<<1)|PACE_READ)) ;
}

/** Get PACE3B upset register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 paceAccess::getUpsetReg ( ) {

   return (accessToFec_->read (accessKey_,(PACE_UPSET_REG<<1)|PACE_READ)) ;
}



const char* paceAccess::names[] = { "CR" , "Latency" , "ChipID0","ChipID1","Vmemref",
			    "Vshifter","Voutbuf","Ireadamp","Ishifter","Imuxbuf",
			    "Ioutbuf","Ispare","Upset"} ; 
const tscType8 paceAccess::rmask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff } ; // i.e. all bits are checked 

const bool paceAccess::ro_[] = { 
  false,false, true, true,false,false,false,false,
  false,false,false,false, true };

 

