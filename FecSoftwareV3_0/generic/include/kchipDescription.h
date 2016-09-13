/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef KCHIPDESCRIPTION_H
#define KCHIPDESCRIPTION_H

#include <iostream>
#include <iomanip>

#include "tscTypes.h"
#include "deviceDescription.h"

#define KCHIP_DESC_CFG       0 
#define KCHIP_DESC_ECFG      1 
#define KCHIP_DESC_KID_L  2 
#define KCHIP_DESC_KID_H  3
#define KCHIP_DESC_MASK_T1CMD  4
#define KCHIP_DESC_LAST_T1CMD  5
#define KCHIP_DESC_LATENCY    6 
#define KCHIP_DESC_EVCNT 7
#define KCHIP_DESC_BNCHCNT_L 8
#define KCHIP_DESC_BNCHCNT_H 9  
#define KCHIP_DESC_GINT_BUSY 10
#define KCHIP_DESC_GINT_IDLE 11 
// #define KCHIP_DESC_FIFOMAP 12  
// #define KCHIP_DESC_FIFODATA_H 13
// #define KCHIP_DESC_FIFODATA_L 14
#define KCHIP_DESC_SR0 12
#define KCHIP_DESC_SR1 13
#define KCHIP_DESC_SEU_COUNTER 14
#define KCHIP_DESC_CALPULSE_DELAY 15
#define KCHIP_DESC_CALPULSE_WIDTH 16
#define KCHIP_DESC_ADC_LATENCY 17
#define KCHIP_DESC_PFIFO_OVERFLOW 18
#define KCHIP_DESC_CHIPID0 19
#define KCHIP_DESC_CHIPID1 20
 
#define KCHIP_DESC_NUM 21
#define KCHIP_DESC_PAR_NUM 18



/**
 * \class kchipDescription
 * This class give a description of all the KCHIP3B registers:
 * <ul>
 * <li>CFG;
 * <li>ECFG;
 * <li>KID_H;
 * <li>KID_L;
 * <li>MASK_T1CMD;
 * <li>LAST_T1CMD;
 * <li>LATENCY;
 * <li>EVCNT;
 * <li>BNCHCNT_H;
 * <li>BNCHCNT_L;
 * <li>GINT_BUSY;
 * <li>GINT_IDLE ;
 * <li>FIFOMAP;  
 * <li>FIFODATA_H;
 * <li>FIFODATA_L;
 * <li>SR0;
 * <li>SR1;
 * <li>SEU_COUNTER;
 * <li>CALPULSE_DELAY;
 * <li>CALPULSE_WIDTH;
 * <li>ADC_LATENCY;
 * <li>PFIFO_OVFL;
 * <li>SID_H;
 * <li>SID_L;
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or reset.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can be set also
 * \see kchipDefinition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "kchipDefinition.h"
 * \author Wojciech BIALAS
 * \date March 2004
 * \brief This class give a description of all the KCHIP3B registers
 */
class kchipDescription: public deviceDescription {

 private:
  
  
  tscType8 values_[KCHIP_DESC_NUM] ; // Kchip3B register replicas

  static const tscType8 comp_mask_[KCHIP_DESC_NUM] ; // comparision mask

  static const  char *names[] ; // register names 

  static const  char *parnames[] ; // parameter names

  static const int  regoffset[] ; // parameter offset table 

  static const ParameterDescription::enumTscType   parTscType[] ; 

 public:

  /** \brief Constructor in order to zero all the parameters
   * Build a description of an KCHIP3B with all the values zero:
   * \param accessKey - key access of a device
   */
  kchipDescription (keyType accessKey = 0x00000000):
    deviceDescription ( KCHIP, accessKey ) {

     int i; 
     for(i=0;i<KCHIP_DESC_NUM;i++) values_[i] = 0x00 ; 

  }



  /** \brief Constructor in order to set all the parameters
   * Build a description of an KCHIP3B with all the values specified:
   * \param accessKey - key access of a device
   * \param cr - Kchip3B register
   * \param ecr - Kchip3B register
   * \param (...) 
   * \param chipID0 - Kchip3B register
   * \param chipID1 - Kchip3B register
   
   */
  kchipDescription (keyType accessKey,
		  tscType8 cfg,
                  tscType8 ecfg,
		  tscType16 kid,
                  tscType8 mask_t1cmd,
                  tscType8 last_t1cmd,
                  tscType8 latency,
                  tscType8 evcnt,
                  tscType16 bnchcnt,
                  
                  tscType8 gint_busy,
                  tscType8 gint_idle,
		  
		    tscType8 sr0,
		    tscType8 sr1,
		    tscType8 seu_counter,
		    tscType8 calpulse_delay,
		    tscType8 calpulse_width,
		    tscType8 adc_latency, 
		    tscType8 pfifo_overflow, 
		    tscType16 chipID  
                  ):
    deviceDescription ( KCHIP, accessKey ) {

    setDescriptionValues (
  		   cfg,
                   ecfg,
                   kid, 
                   mask_t1cmd,
                   last_t1cmd,
                   latency,
                   evcnt,
                   bnchcnt,
                   gint_busy,
                   gint_idle,
		   sr0,
		   sr1,
		   seu_counter,
		   calpulse_delay,
		   calpulse_width,
		   adc_latency, 
		   pfifo_overflow, 
		   chipID   );
  }
  /** \brief Constructor in order to set all the parameters
   * Build a description of an KCHIP3B with all the values specified:
   * \param accessKey - key access of a device
   * \param cr - Kchip3B register
   * \param ecr - Kchip3B register
   * \param (...) 
   * \param chipID0 - Kchip3B register
   * \param chipID1 - Kchip3B register
   
   */

 
  kchipDescription (tscType16 fecSlot,
                  tscType16 ringSlot,
                  tscType16 ccuAddress,
                  tscType16 i2cChannel,
		  tscType16 i2cAddress,
		  tscType8 cfg,
                  tscType8 ecfg,
                  tscType16 kid,
                  
                  tscType8 mask_t1cmd,
                  tscType8 last_t1cmd,
                  tscType8 latency,
                  tscType8 evcnt,
                  tscType16 bnchcnt,
                  tscType8 gint_busy,
                  tscType8 gint_idle,
		
		  tscType8 sr0,
		  tscType8 sr1,
		  tscType8 seu_counter,
		    tscType8 calpulse_delay,
		    tscType8 calpulse_width,
		    tscType8 adc_latency, 
		    tscType8 pfifo_overflow, 
		    tscType16 chipID 
                 ):

    deviceDescription ( KCHIP, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

    setDescriptionValues (
		   cfg,
                   ecfg,
                   kid,
                   mask_t1cmd,
                   last_t1cmd,
                   latency,
                   evcnt,
                   bnchcnt,
                   
                   gint_busy,
                   gint_idle,
		 
		   sr0,
		   sr1,
		   seu_counter,
		   calpulse_delay,
		   calpulse_width,
		   adc_latency, 
		   pfifo_overflow, 
		   chipID
		   );
  }
 /** \brief Constructor in order to set all the parameters
   * Build a description of an KCHIP3B with all the values specified:
   * \param accessKey - key access of a device
   * \param cr - Kchip3B register
   * \param ecr - Kchip3B register
   * \param (...) 
   * \param chipID - Kchip3B hardware ID 
   * \param chipID1 - Kchip3B register
   
   */

  kchipDescription (
		  tscType8 cfg,
                  tscType8 ecfg,
                  tscType16 kid,
                  
                  tscType8 mask_t1cmd,
                  tscType8 last_t1cmd,
                  tscType8 latency,
                  tscType8 evcnt,
                  tscType16 bnchcnt,
                  
                  tscType8 gint_busy,
                  tscType8 gint_idle,
		  
		    tscType8 sr0,
		    tscType8 sr1,
		    tscType8 seu_counter,
		    tscType8 calpulse_delay,
		    tscType8 calpulse_width,
		    tscType8 adc_latency, 
		    tscType8 pfifo_overflow, 
		    tscType16 chipID 
                  ):

    deviceDescription ( KCHIP )  {

    setDescriptionValues (
		   cfg,
                   ecfg,
                   kid,
                   
                   mask_t1cmd,
                   last_t1cmd,
                   latency,
                   evcnt,
                   bnchcnt,
                   
                   gint_busy,
                   gint_idle,
		   
		   sr0,
		   sr1,
		   seu_counter,
		   calpulse_delay,
		   calpulse_width,
		   adc_latency, 
		   pfifo_overflow, 
		   chipID 
		   
		   );
  }



  /** \brief Constructor in order to set all the parameters
   * Build a description of an KCHIP3B from other KCHIP3B description:
   * \param kchip - kchipDescription class 
 
   
   */
  kchipDescription (kchipDescription& kchip) :
    deviceDescription ( KCHIP, kchip.getKey() ) {

    int i ; 
    for(i=0; i < KCHIP_DESC_NUM ; i++ ) { 
      values_[i] = kchip.getValue(i) ; 
    } ; 
  }



    /** \brief Constructor in order to get all the parameters
     * Build a description of an KCHIP from parameterDescriptioNameType:
     * \param parameterNames - parameter hash table     
     */
    kchipDescription (parameterDescriptionNameType parameterNames);




  void setDescriptionValues ( 
			     tscType8 cfg,
			     tscType8 ecfg,
			     tscType16 kid,
			    
			     tscType8 mask_t1cmd,
			     tscType8 last_t1cmd,
			     tscType8 latency,
			     tscType8 evcnt,
			     tscType16 bnchcnt,
			     
			     tscType8 gint_busy,
			     tscType8 gint_idle,
			     
			     tscType8 sr0,
			     tscType8 sr1,
			     tscType8 seu_counter,
			     tscType8 calpulse_delay,
			     tscType8 calpulse_width,
			     tscType8 adc_latency, 
			     tscType8 pfifo_overflow, 
			     tscType16 chipID
		) {


    setCR (cfg) ;
    setECR (ecfg) ;
    setKid (kid);
    
    setMaskT1Cmd (mask_t1cmd);
    setLastT1Cmd (last_t1cmd);
    setLatency (latency);
    setEvCnt (evcnt)  ;
    setBnchCnt  (bnchcnt)    ;
    
    setGintBusy (gint_busy)    ;
    setGintIdle (gint_idle)    ;
   
    setSR0 (sr0) ; 
    setSR1 (sr1) ; 
    setSeuCounter (seu_counter);
    setCalPulseDelay (calpulse_delay);
    setCalPulseWidth (calpulse_width);
    setAdcLantency (adc_latency);
    setPFifoOverflow (pfifo_overflow);
    setChipID (chipID);
   
   
  }

#define LOWBYTE(A)  ( 0x00ff & ((A)) )
#define HIGHBYTE(A) ( 0x00ff & (((A))>>8))


inline void setValue(int reg, tscType8 val) { 
   // reminder there should be protection on reg
   values_[reg] = val ; 
};

inline void setCR (tscType8 cr) { values_[KCHIP_DESC_CFG] = cr; } 
inline void setECR(tscType8 ecr) { values_[KCHIP_DESC_ECFG] = ecr; } 
inline void setKid (tscType16 kid) { 
   values_[KCHIP_DESC_KID_L] = LOWBYTE(kid) ; 
   values_[KCHIP_DESC_KID_H] = HIGHBYTE(kid) ;
}
//void setKid_H (tscType8 kid_h) { values_[KCHIP_DESC_KID_H] = kid_h; } 
inline void setMaskT1Cmd (tscType8 mask_t1cmd) { values_[KCHIP_DESC_MASK_T1CMD] = mask_t1cmd; } 
inline void setLastT1Cmd (tscType8 last_t1cmd){ values_[KCHIP_DESC_LAST_T1CMD] = last_t1cmd; } 
inline void setLatency (tscType8 latency) { values_[KCHIP_DESC_LATENCY] = latency; } 
inline void setEvCnt (tscType8 evcnt) { values_[KCHIP_DESC_EVCNT] = evcnt; } 
inline void setBnchCnt  (tscType16 bnchcnt) { 
   values_[KCHIP_DESC_BNCHCNT_L] = LOWBYTE(bnchcnt);
   values_[KCHIP_DESC_BNCHCNT_H] = HIGHBYTE(bnchcnt);
} 
// void setBnchCnt_H  (tscType8 bnchcnt_h) { values_[KCHIP_DESC_BNCHCNT_H] = bnchcnt_h; } 
inline void setGintBusy (tscType8 gintbusy) { values_[KCHIP_DESC_GINT_BUSY] = gintbusy; } 
inline void setGintIdle (tscType8 gintidle) { values_[KCHIP_DESC_GINT_IDLE] = gintidle; } 

// for harware debugging ONLY :  
// void setFiFoMap(tscType8 fifomap) { values_[KCHIP_DESC_FIFOMAP] = fifomap; } 
// void setFifoData_L (tscType8 fifodata_l ) { values_[KCHIP_DESC_FIFODATA_L] = fifodata_l; } 
// void setFifoData_H (tscType8 fifodata_h) { values_[KCHIP_DESC_FIFODATA_H] = fifodata_h; }
      

// other : 
inline void setSR0 (tscType8 sr0) { values_[KCHIP_DESC_SR0] = sr0 ; } 
inline void setSR1 (tscType8 sr1) { values_[KCHIP_DESC_SR1] = sr1 ; } 
inline void setSeuCounter (tscType8 seu_counter) { values_[KCHIP_DESC_SEU_COUNTER] = seu_counter; } 
inline void setCalPulseDelay (tscType8 calpulse_delay) { values_[KCHIP_DESC_CALPULSE_DELAY] = calpulse_delay; } 
inline void setCalPulseWidth (tscType8 calpulse_width) { values_[KCHIP_DESC_CALPULSE_WIDTH] = calpulse_width; } 
inline void setAdcLantency (tscType8 adc_latency) { values_[KCHIP_DESC_ADC_LATENCY] = adc_latency; } 
inline void setPFifoOverflow (tscType8 pfifo_overflow) { values_[KCHIP_DESC_PFIFO_OVERFLOW] = pfifo_overflow; } 
inline void setChipID (tscType16 chipID) { 
   values_[KCHIP_DESC_CHIPID0] = HIGHBYTE(~chipID);
   values_[KCHIP_DESC_CHIPID1] = LOWBYTE(~chipID);
} 
// void setChipID1 (tscType8 chipID1) { values_[KCHIP_DESC_CHIPID1] = chipID1; } 







inline   tscType8 getValue(int reg) { 
    // reminder : need a protection on reg val 
    return (values_[reg]) ; 
  } 


inline   tscType8 getMaskedValue(int reg) { 
    // reminder : need a protection on reg val 
    return (values_[reg] & comp_mask_[reg]) ; 
  } 


inline tscType8 getCR() { 
    return (values_[KCHIP_DESC_CFG]) ; 
  } 
inline tscType8 getECR(){ 
    return (values_[KCHIP_DESC_ECFG]) ; 
  } 




inline tscType16 getKid () { 
   tscType16 val = values_[KCHIP_DESC_KID_L] ;
   val |= ( ((tscType16)values_[KCHIP_DESC_KID_H]) << 8 ) ;
    return (val) ; 
  }
inline tscType8 getMaskT1Cmd () { 
    return (values_[KCHIP_DESC_MASK_T1CMD]) ; 
  }
inline tscType8 getLastT1Cmd () { 
    return (values_[KCHIP_DESC_LAST_T1CMD]) ; 
  }
inline tscType8 getLatency () { 
    return (values_[KCHIP_DESC_LATENCY]) ; 
  }
inline tscType8 getEvCnt () { 
    return (values_[KCHIP_DESC_EVCNT]) ; 
  }
inline tscType16 getBnchCnt  () { 
    tscType16 val = values_[KCHIP_DESC_BNCHCNT_L] ;
    val |= ( ((tscType16)values_[KCHIP_DESC_BNCHCNT_H]) << 8 ) ; 
    return (val) ; 
  }
inline tscType8 getGintBusy () { 
    return (values_[KCHIP_DESC_GINT_BUSY]) ; 
  }
inline tscType8 getGintIdle () { 
    return (values_[KCHIP_DESC_GINT_IDLE]) ; 
  }

// for hardware debugging ONLY !
// tscType8 getFifoMap() { 
//     return (values_[KCHIP_DESC_FIFOMAP]) ; 
//   }
// tscType8 getFifoData_L () { 
//     return (values_[KCHIP_DESC_FIFODATA_L]) ; 
//   }
// tscType8 getFifoData_H ()  { 
//     return (values_[KCHIP_DESC_FIFODATA_H]) ; 
//   }

// other : 
inline tscType8 getSR0 () { 
    return (values_[KCHIP_DESC_SR0]) ; 
  } 
inline tscType8 getSR1 () { 
    return (values_[KCHIP_DESC_SR1]) ; 
  } 
inline tscType8 getSeuCounter () { 
    return (values_[KCHIP_DESC_SEU_COUNTER]) ; 
  } 
inline tscType8 getCalPulseDelay () { 
    return (values_[KCHIP_DESC_CALPULSE_DELAY]) ; 
  } 
inline tscType8 getCalPulseWidth () { 
    return (values_[KCHIP_DESC_CALPULSE_WIDTH]) ; 
  }
inline tscType8 getAdcLatency () { 
    return (values_[KCHIP_DESC_ADC_LATENCY]) ; 
  }
inline tscType8 getPFifoOverflow () { 
    return (values_[KCHIP_DESC_PFIFO_OVERFLOW]) ; 
  }
inline tscType16 getChipID () { 
   tscType16 val = 0x00ff & (~values_[KCHIP_DESC_CHIPID1]) ;
   val |= 0xff00 &  ( ((tscType16)~values_[KCHIP_DESC_CHIPID0]) << 8 ) ;
    return (val) ;
  }

  /** \brief In order to compare two APV descriptions
   * \param apv - description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( kchipDescription &kchip ) {

#ifdef DEBUGMSGERROR
    displayDifferences ( kchip ) ;
#endif

   
      return false ;
  }
  
  /** \brief In order to compare two APV descriptions
   * \param apv - description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( kchipDescription &kchip ) {

#ifdef DEBUGMSGERROR
   displayDifferences ( kchip ) ;
#endif
   bool flag = true ; 
   int i ; 

   for(i=0;i<KCHIP_DESC_NUM;i++) {
     if (getMaskedValue(i)!=kchip.getMaskedValue(i) ) flag = false ; 
     
   }
      return flag ;
  }

  /** Clone a device description
   * \return the device description cloned
   * \warning to not forget to delete the result once you have finished with it
   */
  kchipDescription *clone ( ) {

    kchipDescription *kchip = new kchipDescription (*this) ;
    //kchip->setFecHardwareId(fecHardwareId_) ;
    //kchip->setEnabled(enabled_) ;
    return (kchip) ;
  }

  /** \brief Display the KCHIP values where the comparison is different
   * \param kchip - uploaded values
   */
  void displayDifferences ( kchipDescription &kchip ) {

    char msg[80] ;
    decodeKey (msg, accessKey_) ;
    std::cout << "KCHIP " <<  msg << std::endl ;
    int i ; 

   for(i=0;i<KCHIP_DESC_NUM;i++) {
     if (getMaskedValue(i)!=kchip.getMaskedValue(i) ) { 
       std::cout << "Reg."  << names[i] << " differ : " ; 
       std::cout << (int) getValue(i)  << "/" ; 
       std::cout << (int) kchip.getValue(i) << " " ; 
       std::cout << "(0x" << std::hex << std::setw(2) << std::setfill('0') << (int) getValue(i) << "/" ;
       std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int) kchip.getValue(i) << ")" ;
       std::cout << std::setfill(' ') << std::dec  ; 
     }      
   }

   std::cout << std::endl  << std::endl ; 

  }


  /** \brief Display the KCHIP values
   */
  void display ( ) {

    std::cout << "Kchip: 0x" << std::setw(2) << std::setfill('0') << std::hex << getFecSlot() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getRingSlot() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getCcuAddress() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getChannel() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getAddress() ;
    std::cout << std::dec << std::endl ; 

    //std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;

    int i ; 


   for(i=0;i<KCHIP_DESC_NUM;i++) {
     
     std::cout << "Reg."  << names[i] << " : \t"  ; 
     int val = ((int) getValue(i)) ; std::cout << std::setw(3) << std::setfill(' ') << val ; 
     std::cout << "\t (0x" << std::hex << std::setw(2) << std::setfill('0') ; 
     val = ((int) getValue(i))  ; 
     std::cout << val << ")" << std::dec << std::endl ; 

   }
   std::cout << std::setfill(' ') << std::dec << std::endl ;


    
  } // end display 


  static parameterDescriptionNameType *getParameterNames ( ) ; 

  private : 

   /** \brief set the value for register(s) 
   * Set the value for an Delta3B description. 
   + This is helper private function 
   * \param val - value to be set
   */
  void setValueVarSize (int offset, void *val, int type ) {

      tscType32 *ptmp4 = (tscType32*) val ;
      tscType16 *ptmp2 = (tscType16*) val ;
      tscType8  *ptmp1 = (tscType8*) val ;
      
      switch (type) { 
      case ParameterDescription::INTEGER8 : 
	 std::cout << "value is : " << (unsigned int)(*ptmp1) << std::endl ;
	values_[offset] = (tscType8) (*ptmp1) ; 
	break ; 
      case ParameterDescription::INTEGER16 : 
	 std::cout << "value is : " << (unsigned int)(*ptmp2) << std::endl ;
	values_[offset] = (0x000000ff & (*ptmp2) ) ; 
	values_[offset+1] = (0x000000ff & ((*ptmp2)>>8) ) ;
	break ; 
     
      case ParameterDescription::INTEGER32 : 
	 std::cout << "value is : " << (unsigned int)(*ptmp4) << std::endl ;
	values_[offset] = (0x000000ff & (*ptmp4) ) ; 
	values_[offset+1] = (0x000000ff & ((*ptmp4)>>8) ) ;
	values_[offset+2] = (0x000000ff & ((*ptmp4)>>16) ) ;
	values_[offset+3] = (0x000000ff & ((*ptmp4)>>24) ) ;
	break ;  

      } 
  }





};

// // W.B. IMPORTANT 
// // access class definition has to be after description class definition
//#include "kchipAccess.h"

#endif
