/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef DELTADESCRIPTION_H
#define DELTADESCRIPTION_H

#include <iostream>
#include <iomanip>
#include <string>

#include "tscTypes.h"
#include "deviceDescription.h"

#define DELTA_DESC_CR0       0 
#define DELTA_DESC_CR1       1 
#define DELTA_DESC_CHIPID0   2 
#define DELTA_DESC_CHIPID1   3 
#define DELTA_DESC_CALCHAN0  4 
#define DELTA_DESC_CALCHAN1  5 
#define DELTA_DESC_CALCHAN2  6
#define DELTA_DESC_CALCHAN3  7
#define DELTA_DESC_VCAL 8 
#define DELTA_DESC_VOPREAMP 9
#define DELTA_DESC_VOSHAPER 10
#define DELTA_DESC_VSPARE 11
#define DELTA_DESC_IPREAMP 12 
#define DELTA_DESC_ISHAPER 13
#define DELTA_DESC_ISF 14 
#define DELTA_DESC_ISPARE 15 

#define DELTA_DESC_NUM 16 
#define DELTA_DESC_PAR_NUM 12




/* class descRegister {  */
/*  private:  */
/*   tscType8 value_ ;  */
/*   tscType8 comp_mask_ ;  */

/*   string name_ ;  */

/*  public:  */

/*   descRegister() : value_(0x00),comp_mask_(0x00),name_("empty") {}; */

/*   descRegister(tscType8 val, tscType8 mask, string name ) :  */
/*     value_(val), comp_mask_(mask), name_(name) {}  */

/*   ~descRegister() {} */

/*   tscType8 getValue() { return value_ ; }   */
/*   tscType8 getCompMask() { return comp_mask_ ; }  */

/*   string&  getName () { return name_ ; }  */

/* } ;  */


struct chipRegDesc { 
  tscType8 val_ ; 
  tscType8 comp_mask_ ; 
} ; 


/**
 * \class deltaDescription
 * This class give a description of all the DELTA3B registers:
 * <ul>
 * <li>CR0;
 * <li>CR1;
 * <li>ChipID;
 * <li>CalChan0;
 * <li>CalChan1;
 * <li>CalChan2;
 * <li>CalChan3;
 * <li>VCal;
 * <li>VoPreamp;
 * <li>VoShaper;
 * <li>VSpare;
 * <li>IPreamp;
 * <li>IShaper;
 * <li>ISF;
 * <li>ISpare;   
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can be set also
 * \see deltaDefinition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "deltaDefinition.h"
 * \author Wojciech BIALAS
 * \date March 2004
 * \brief This class give a description of all the DELTA3B registers
 */
class deltaDescription: public deviceDescription {

 private:
  

  tscType8 values_[DELTA_DESC_NUM] ; // Delta3B register replicas

  static const tscType8 comp_mask_[DELTA_DESC_NUM] ; // comparision mask

  static const char *names[] ; // register names 
  static const char *parnames[] ; // parameter names

  static const int  regoffset[] ; // parameter offset table 

  static const ParameterDescription::enumTscType   parTscType[] ; 

 public:

  /** \brief Constructor in order to zero all the parameters
   * Build a description of an DELTA3B with all the values zero
   * \param accessKey - key access of a device
   
   */
  deltaDescription (keyType accessKey):
    deviceDescription ( DELTA, accessKey ) {

     int i; 
     for(i=0;i<DELTA_DESC_NUM;i++) values_[i] = 0x00 ; 
  }




  /** \brief Constructor in order to set all the parameters
   * Build a description of an DELTA3B with all the values specified:
   * \param accessKey - key access of a device
   * \param cr0 - Delta3B register
   * \param cr1 - Delta3B register
   * \param chipID0 - Delta3B register
   * \param chipID1 - Delta3B register
   * \param calchan0 - Delta3B register
   * \param calchan1 - Delta3B register
   * \param calchan2 - Delta3B register
   * \param calchan3 - Delta3B register
   * \param vcal - Delta3B register
   * \param vopreamp - Delta3B register
   * \param voshaper - Delta3B register
   * \param vspare - Delta3B register
   * \param ipreamp - Delta3B register
   * \param ishaper - Delta3B register
   * \param isf - Delta3B register
   * \param ispare - Delta3B register
   
   */
  deltaDescription (keyType accessKey,
		    tscType8 cr0,
                  tscType8 cr1,
                  tscType16 chipID,
                  tscType32 calchan,
                  tscType8 vcal,
                  tscType8 vopreamp,
                  tscType8 voshaper,
                  tscType8 vspare,
                  tscType8 ipreamp,
                  tscType8 ishaper,
                  tscType8 isf,
                  tscType8 ispare
                  ):
    deviceDescription ( DELTA, accessKey ) {

    setDescriptionValues (cr0,
                   cr1,
                   chipID,
                   calchan,
                   vcal,
                   vopreamp,
                   voshaper,
                   vspare,
                   ipreamp,
                   ishaper,
                   isf,
                   ispare
			  );
  }

  /** \brief Constructor in order to set all the parameters
   * Build a description of an DELTA3B with all the values specified:
   * \param accessKey - key access of a device
   * \param cr0 - Delta3B register
   * \param cr1 - Delta3B register
   * \param chipID0 - Delta3B hardware ID
   * \param calchan - Delta3B injection mask
   * \param vcal - Delta3B register
   * \param vopreamp - Delta3B register
   * \param voshaper - Delta3B register
   * \param vspare - Delta3B register
   * \param ipreamp - Delta3B register
   * \param ishaper - Delta3B register
   * \param isf - Delta3B register
   * \param ispare - Delta3B register
   
   */
  deltaDescription (tscType16 fecSlot,
                  tscType16 ringSlot,
                  tscType16 ccuAddress,
                  tscType16 i2cChannel,
                  tscType16 i2cAddress,
                  tscType8 cr0,
                  tscType8 cr1,
                  tscType16 chipID,
                  tscType32 calchan,
                  tscType8 vcal,
                  tscType8 vopreamp,
                  tscType8 voshaper,
                  tscType8 vspare,
                  tscType8 ipreamp,
                  tscType8 ishaper,
                  tscType8 isf,
                  tscType8 ispare):

    deviceDescription ( DELTA, fecSlot, ringSlot, ccuAddress, i2cChannel, i2cAddress) {

    setDescriptionValues (cr0,
                   cr1,
                   chipID,
                   calchan,
                   vcal,
                   vopreamp,
                   voshaper,
                   vspare,
                   ipreamp,
                   ishaper,
                   isf,
                   ispare);
  }

 /** \brief Constructor in order to set all the parameters
   * Build a description of an DELTA3B with all the values specified:
   * \param accessKey - key access of a device
   * \param cr0 - Delta3B register
   * \param cr1 - Delta3B register

   * \param chipID - Delta3B hardware chip ID 
   * \param calchan - Delta3B pulse injection mask 

   * \param vcal - Delta3B register
   * \param vopreamp - Delta3B register
   * \param voshaper - Delta3B register
   * \param vspare - Delta3B register
   * \param ipreamp - Delta3B register
   * \param ishaper - Delta3B register
   * \param isf - Delta3B register
   * \param ispare - Delta3B register
   
   */
  deltaDescription (tscType8 cr0,
                  tscType8 cr1,
                  tscType16 chipID,
                  
                  tscType32 calchan,

                  tscType8 vcal,
                  tscType8 vopreamp,
                  tscType8 voshaper,
                  tscType8 vspare,
                  tscType8 ipreamp,
                  tscType8 ishaper,
                  tscType8 isf,
                  tscType8 ispare
                  ):

    deviceDescription ( DELTA )  {

    setDescriptionValues (cr0,
                   cr1,
		   chipID,
                   calchan,
                   vcal,
                   vopreamp,
                   voshaper,
                   vspare,
                   ipreamp,
                   ishaper,
                   isf,
                   ispare
        );
    }

    /** \brief Constructor in order to set all the parameters
     * Build a description of an DELTA3B from other DELTA3B description:
     * \param delta - deltaDescription class 
     */
    deltaDescription (deltaDescription& delta) :
    deviceDescription ( DELTA, delta.getKey() ) {

      int i ; 
      for(i=0; i < DELTA_DESC_NUM ; i++ ) { 
	values_[i] = delta.getValue(i) ; 
      } ; 
    
    }

    /** \brief Constructor in order to get all the parameters
     * Build a description of an DELTA3B from parameterDescriptioNameType:
     * \param parameterNemes - parameter hash table     
     */
    deltaDescription (parameterDescriptionNameType parameterNames);

   
/** \brief function in order to set all the parameters of description
   * Build a description of an DELTA3B with all the values specified:
   * \param cr0 - Delta3B register
   * \param cr1 - Delta3B register
   * \param chipID - Delta3B hardware address 
   * \param calchan - Delta3B pulse injection mask 
   * \param vcal - Delta3B register
   * \param vopreamp - Delta3B register
   * \param voshaper - Delta3B register
   * \param vspare - Delta3B register
   * \param ipreamp - Delta3B register
   * \param ishaper - Delta3B register
   * \param isf - Delta3B register
   * \param ispare - Delta3B register
   
   */
  
  void setDescriptionValues ( 
		  tscType8 cr0,
                  tscType8 cr1,
                  tscType16 chipID,
                  
                  tscType32 calchan,
                  
                  tscType8 vcal,
                  tscType8 vopreamp,
                  tscType8 voshaper,
                  tscType8 vspare,
                  tscType8 ipreamp,
                  tscType8 ishaper,
                  tscType8 isf,
                  tscType8 ispare
		) {


    setCR0 (cr0) ;
    setCR1 (cr1) ;
    setChipID (chipID);
//    setChipID1 (chipID1); 
    setCalChan (calchan);
//     setCalChan1 (calchan1);
//     setCalChan2 (calchan2);
//     setCalChan3 (calchan3);
    setVcal (vcal)    ;
    setVoPreamp  (vopreamp)  ;
    setVoShaper  (voshaper)    ;
    setVspare    (vspare)    ;
    setIpreamp    (ipreamp)    ;
    setIshaper    (ishaper)    ;
    setISF  (isf)  ;
    setIspare    (ispare)    ;
   
  }

  /** \brief Return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline tscType8 getCR0    ( ) { return (values_[DELTA_DESC_CR0]); }
  /** \brief return the value of the APV register corresponding to an APV registe
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline tscType8 getCR1 ( ) { return (values_[DELTA_DESC_CR1]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline tscType16 getChipID ( ) {
     tscType16 val = ((tscType16)(~values_[DELTA_DESC_CHIPID0])<<8) & 
	(~values_[DELTA_DESC_CHIPID1]); 	
     return  val ; 
  }
//   /** \brief return the value of the APV register corresponding to an APV register
//    * Return the value of the APV register corresponding to an APV register
//    * \return the value
//    */
//   tscType8 getChipID1 ( ) { return (values_[DELTA_DESC_CHIPID1]) ; }
 
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline tscType32 getCalChan  ( ) {
     
     tscType32 val = values_[DELTA_DESC_CALCHAN0] ;   
     val |= (values_[DELTA_DESC_CALCHAN1] << 8)  ;
     val |= (values_[DELTA_DESC_CALCHAN2] << 16)  ;
     val |= (values_[DELTA_DESC_CALCHAN3] << 24)  ;
     
     return (val) ; 
  } 
//   /** \brief return the value of the APV register corresponding to an APV register
//    * Return the value of the APV register corresponding to an APV register
//    * \return the value
//    */
//   tscType8 getCalChan1    ( ) { return (values_[DELTA_DESC_CALCHAN1]) ; }
//   /** \brief return the value of the APV register corresponding to an APV register
//    * Return the value of the APV register corresponding to an APV register
//    * \return the value
//    */
//   tscType8 getCalChan2    ( ) { return (values_[DELTA_DESC_CALCHAN2]) ; }
//   /** \brief return the value of the APV register corresponding to an APV register
//    * Return the value of the APV register corresponding to an APV register
//    * \return the value
//    */
//   tscType8 getCalChan3    ( ) { return (values_[DELTA_DESC_CALCHAN3]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline  tscType8 getVcal    ( ) { return (values_[DELTA_DESC_VCAL]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline  tscType8 getVoPreamp  ( ) { return (values_[DELTA_DESC_VOPREAMP]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline  tscType8 getVoShaper    ( ) { return (values_[DELTA_DESC_VOSHAPER]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline  tscType8 getVspare  ( ) { return (values_[DELTA_DESC_VSPARE]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline  tscType8 getIpreamp     ( ) { return (values_[DELTA_DESC_IPREAMP]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline  tscType8 getIshaper     ( ) { return (values_[DELTA_DESC_ISHAPER]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline  tscType8 getISF    ( ) { return (values_[DELTA_DESC_ISF]) ; }
  /** \brief return the value of the APV register corresponding to an APV register
   * Return the value of the APV register corresponding to an APV register
   * \return the value
   */
inline  tscType8 getIspare    ( ) { return (values_[DELTA_DESC_ISPARE]) ; }  


  /** \brief set the value for register
   * Set the value for an  description register reg
   * \param val - value to be set
   */
inline void setValue(int reg, tscType8 val) { 
   // reminder there should be protection on reg
   values_[reg] = val ; 
};

  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setCR0    ( tscType8 cr0    ) { values_[DELTA_DESC_CR0] = cr0 ; }

  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setCR1 ( tscType8 cr1 ) { values_[DELTA_DESC_CR1] = cr1 ; }

  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setChipID    ( tscType16 chipid   ) { 
     values_[DELTA_DESC_CHIPID0] = (~chipid) << 8 ;
     values_[DELTA_DESC_CHIPID1] = (0x00ff & (~chipid)) ;
  }
//   /** \brief set the value for register
//    * Set the value for an APV description
//    * \param Status - value to be set
//    */
//   void setChipID1    ( tscType8 chipid1    ) { values_[DELTA_DESC_CHIPID1] = chipid1 ; }
  





  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setCalChan ( tscType32 calchan ) { 
    
     values_[DELTA_DESC_CALCHAN0] = (0x000000ff & calchan ) ; 
     values_[DELTA_DESC_CALCHAN1] = (0x000000ff & (calchan>>8) ) ; 
     values_[DELTA_DESC_CALCHAN2] = (0x000000ff & (calchan>>16) ) ;
     values_[DELTA_DESC_CALCHAN3] = (0x000000ff & (calchan>>24) ) ;
    
  }
//   /** \brief set the value for register
//    * Set the value for an APV description
//    * \param Status - value to be set
//    */
//   void setCalChan1 ( tscType8 calchan1 ) { values_[DELTA_DESC_CALCHAN1] = calchan1 ; }
//   /** \brief set the value for register
//    * Set the value for an APV description
//    * \param Status - value to be set
//    */
//   void setCalChan2 ( tscType8 calchan2 ) { values_[DELTA_DESC_CALCHAN2] = calchan2 ; }
//   /** \brief set the value for register
//    * Set the value for an APV description
//    * \param Status - value to be set
//    */
//   void setCalChan3 ( tscType8 calchan3 ) { values_[DELTA_DESC_CALCHAN3] = calchan3 ; }
  
  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setVcal    ( tscType8 vcal    ) {values_[DELTA_DESC_VCAL] = vcal ; }
  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setVoPreamp    ( tscType8 vopreamp    ) {values_[DELTA_DESC_VOPREAMP]  = vopreamp ; }
  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setVoShaper    ( tscType8 voshaper    ) { values_[DELTA_DESC_VOSHAPER] = voshaper ; }
  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setVspare  ( tscType8 vspare  ) { values_[DELTA_DESC_VSPARE] = vspare ; }
  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setIpreamp    ( tscType8 ipreamp    ) { values_[DELTA_DESC_IPREAMP] = ipreamp ; }
  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setIshaper    ( tscType8 ishaper    ) {values_[DELTA_DESC_ISHAPER]  = ishaper ; }
  /** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */
inline  void setISF    ( tscType8 isf ) { values_[DELTA_DESC_ISF] = isf ; }  

/** \brief set the value for register
   * Set the value for an APV description
   * \param Status - value to be set
   */

inline  void setIspare    ( tscType8 ispare) { values_[DELTA_DESC_ISPARE] = ispare ; }


inline  tscType8 getValue(int reg) { 
    // reminder : need a protection on reg val 
    return (values_[reg]) ; 
  } 


inline  tscType8 getMaskedValue(int reg) { 
    return (values_[reg] & comp_mask_[reg]) ; 
  } 



  /** \brief In order to compare two Delta3B descriptions
   * \param delta - description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( deltaDescription &delta ) {

#ifdef DEBUGMSGERROR
    displayDifferences ( delta ) ;
#endif

   
      return (!((*this)==delta) )  ; ;
  }
  
  /** \brief In order to compare two APV descriptions
   * \param apv - description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( deltaDescription &delta ) {

#ifdef DEBUGMSGERROR
   displayDifferences ( delta ) ;
#endif
   bool flag = true ; 
   int i ; 

   for(i=0;i<DELTA_DESC_NUM;i++) {
     if (getMaskedValue(i)!=delta.getMaskedValue(i) ) flag = false ; 
     
   }
      return flag ;
  }

  /** Clone a device description
   * \return the device description cloned
   * \warning to not forget to delete the result once you have finished with it
   */
  deltaDescription *clone ( ) {

    deltaDescription *delta = new deltaDescription (*this) ;
    //delta->setFecHardwareId(fecHardwareId_) ;
    //delta->setEnabled(enabled_) ;
    return (delta) ;
  }

  /** \brief Display the DELTA values where the comparison is different
   * \param delta - uploaded values
   */
  void displayDifferences ( deltaDescription &delta ) {

    char msg[80] ;
    decodeKey (msg, accessKey_) ;
    std::cout << "DELTA " <<  msg << std::endl ;
    int i ; 

   for(i=0;i<DELTA_DESC_NUM;i++) {
     if (getMaskedValue(i)!=delta.getMaskedValue(i) ) { 
       std::cout << "Reg."  << names[i] << " differ : " ; 
       std::cout << (int) getValue(i)  << "/" ; 
       std::cout << (int) delta.getValue(i) << " " ; 
       std::cout << "(0x" << std::hex << std::setw(2) << std::setfill('0') << (int) getValue(i) << "/" ;
       std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int) delta.getValue(i) << ")" ;
       std::cout << std::setfill(' ') << std::dec  ; 
     }      
   }

   std::cout << std::endl  << std::endl ; 


  }

  /** \brief Display the DELTA values
   */
  void display ( ) {

    std::cout << "Delta: 0x" << std::setw(2) << std::setfill('0') << std::hex << getFecSlot() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getRingSlot() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getCcuAddress() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getChannel() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getAddress() ;
    std::cout << std::dec << std::endl ; 

    //std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;

    int i ; 

   for(i=0;i<DELTA_DESC_NUM;i++) {
     std::cout << "Reg."  << names[i] << " : \t"  ; 
     std::cout << std::setw(3) << std::setfill(' ') << ((int) getValue(i)) ; 
     std::cout << "\t (0x" << std::hex << std::setw(2) << std::setfill('0') ;      
     std::cout << ((int) getValue(i))  << ")" << std::dec << std::endl ; 
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
      tscType8 *ptmp1 = (tscType8*) val ;
      
      switch (type) { 
      case ParameterDescription::INTEGER8 : 
	values_[offset] = (tscType8) (*ptmp1) ; 
	break ; 
      case ParameterDescription::INTEGER16 : 
	values_[offset] = (0x000000ff & (*ptmp2) ) ; 
	values_[offset+1] = (0x000000ff & ((*ptmp2)>>8) ) ;
	break ; 
     
      case ParameterDescription::INTEGER32 : 
	values_[offset] = (0x000000ff & (*ptmp4) ) ; 
	values_[offset+1] = (0x000000ff & ((*ptmp4)>>8) ) ;
	values_[offset+2] = (0x000000ff & ((*ptmp4)>>16) ) ;
	values_[offset+3] = (0x000000ff & ((*ptmp4)>>24) ) ;
	break ;  

      } 
   
  }


};


#endif
