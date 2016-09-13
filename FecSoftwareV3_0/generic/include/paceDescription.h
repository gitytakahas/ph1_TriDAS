/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef PACEDESCRIPTION_H
#define PACEDESCRIPTION_H

#include <iostream>
#include <iomanip>

#include "tscTypes.h"
#include "deviceDescription.h"
#include "paceDefinition.h"

#define PACE_DESC_CR       0 
#define PACE_DESC_LATENCY   1 
#define PACE_DESC_CHIPID0   2 
#define PACE_DESC_CHIPID1   3 
#define PACE_DESC_VMEMREF  4 
#define PACE_DESC_VSHIFT  5 
#define PACE_DESC_VOUTBUF  6
#define PACE_DESC_IREADAMP  7
#define PACE_DESC_ISHIFT 8 
#define PACE_DESC_IMUXBUF 9
#define PACE_DESC_IOUTBUF 10
#define PACE_DESC_ISPARE 11
#define PACE_DESC_UPSETREG 12 

#define PACE_DESC_NUM 13 
#define PACE_DESC_PAR_NUM 12 



/**
 * \class paceDescription
 * This class give a description of all the PACE3B registers:
 * <ul>
 * <li>Control;
 * <li>Latency;
 * <li>ChipID;
 * <li>VMemRef;
 * <li>VShifter;
 * <li>VOutBuf;
 * <li>IReadAmp;
 * <li>IShifter;
 * <li>IMuxBuf;
 * <li>IOutBuf;
 * <li>ISpare;
 * <li>Single Event Upset  
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can be set also
 * \see paceDefinition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "paceDefinition.h"
 * \author Wojciech BIALAS
 * \date March 2004
 * \brief This class give a description of all the PACE3B registers
 */
class paceDescription: public deviceDescription {

 private:
  

  tscType8 values_[PACE_DESC_NUM] ; // Pace3B register replicas

  static const tscType8 comp_mask_[] ; // comparision mask

  static const  char *names[] ; // register names 

  static const  char *parnames[] ; // parameter names

  static const  int  regoffset[] ; // parameter offset table 

  static const  ParameterDescription::enumTscType   parTscType[] ; // parametar type table 

 public:
  /** \brief Constructor in order to zero all the parameters
   * Build a description of an PACE3B with all the values specified:
   * \param accessKey - key access of a device


   
   */
  paceDescription (keyType accessKey = 0x00000000 ):
    deviceDescription ( PACE, accessKey ) {

     
     int i; 
     for(i=0;i<PACE_DESC_NUM;i++) values_[i] = 0x00 ; 


  }

  /** \brief Constructor in order to set all the parameters
   * Build a description of an PACE3B with all the values specified:
   * \param accessKey - key access of a device
   * \param cr - Pace3B register
   * \param latency - Pace3B register
   * \param chipID - Pace3B register
   * \param vmemref - Pace3B register
   * \param vshift - Pace3B register
   * \param voutbuf - Pace3B register
   * \param ireadamp - Pace3B register
   * \param ishift - Pace3B register
   * \param imuxbuf - Pace3B register
   * \param ioutbuf - Pace3B register
   * \param ispare - Pace3B register
   * \param upsetreg - Pace3B register

   
   */
  paceDescription (keyType accessKey,
		    tscType8 cr,
                  tscType8 latency,
                  tscType16 chipID,
          
                  tscType8 vmemref,
                  tscType8 vshift,
                  tscType8 voutbuf,
                  tscType8 ireadamp,
                  tscType8 ishift,
                  tscType8 imuxbuf,
                  tscType8 ioutbuf,
                  tscType8 ispare,
                  tscType8 upsetreg
                
                  ):
    deviceDescription ( PACE, accessKey ) {

    setDescriptionValues (cr,
                   latency,
                   chipID,
                   
                   vmemref,
                   vshift,
                   voutbuf,
                   ireadamp,
                   ishift,
                   imuxbuf,
                   ioutbuf,
                   ispare,
                   upsetreg
                 
			  );
  }

 /** \brief Constructor in order to set all the parameters
   * Build a description of an PACE3B with all the values specified:

   * \param cr - Pace3B register
   * \param latency - Pace3B register
   * \param chipID - Pace3B register
   * \param vmemref - Pace3B register
   * \param vshift - Pace3B register
   * \param voutbuf - Pace3B register
   * \param ireadamp - Pace3B register
   * \param ishift - Pace3B register
   * \param imuxbuf - Pace3B register
   * \param ioutbuf - Pace3B register
   * \param ispare - Pace3B register
   * \param upsetreg - Pace3B register

   
   */
  paceDescription (
		   tscType16 fecSlot,
                  tscType16 ringSlot,
                  tscType16 ccuAddress,
                  tscType16 i2cChannel,
                  tscType16 i2cAddress, 
		    tscType8 cr,
                  tscType8 latency,
                  tscType16 chipID,
                  tscType8 vmemref,
                  tscType8 vshift,
                  tscType8 voutbuf,
                  tscType8 ireadamp,
                  tscType8 ishift,
                  tscType8 imuxbuf,
                  tscType8 ioutbuf,
                  tscType8 ispare,
                  tscType8 upsetreg
                
                  ):
    deviceDescription ( PACE, fecSlot, ringSlot, ccuAddress, i2cChannel,
			i2cAddress) {

    setDescriptionValues (cr,
                   latency,
                   chipID,
                   vmemref,
                   vshift,
                   voutbuf,
                   ireadamp,
                   ishift,
                   imuxbuf,
                   ioutbuf,
                   ispare,
                   upsetreg
                 
			  );
  }



  /** \brief Constructor in order to set all the parameters
   * Build a description of an PACE3B from other PACE3B description:
   * \param pace - paceDescription class 
 
   
   */
  paceDescription (paceDescription& pace) :
    deviceDescription ( PACE, pace.getKey() ) {

    int i ; 
    for(i=0; i < PACE_DESC_NUM ; i++ ) { 
      values_[i] = pace.getValue(i) ; 
    } ; 
  }



  
/** \brief Constructor in order to set all the parameters
   * Build a description of an PACE3B from parameter names:
   * \param parameterNames - parameter hash table  
 
   
   */
  paceDescription (parameterDescriptionNameType parameterNames);






/** \brief Constructor in order to set all the parameters
   * Build a description of an PACE3B with all the values specified:
   * \param accessKey - key access of a device
   * \param cr0 - Pace3B register
   * \param cr1 - Pace3B register
   * \param chipID - Pace3B register
   * \param calchan0 - Pace3B register
   * \param calchan1 - Pace3B register
   * \param calchan2 - Pace3B register
   * \param calchan3 - Pace3B register
   * \param vcal - Pace3B register
   * \param vopreamp - Pace3B register
   * \param voshaper - Pace3B register
   * \param vspare - Pace3B register
   * \param ipreamp - Pace3B register
   * \param ishaper - Pace3B register
   * \param isf - Pace3B register
   * \param ispare - Pace3B register
   
   */
void    setDescriptionValues (
                  tscType8 cr,
                  tscType8 latency,
                  tscType16 chipID,
               
                  tscType8 vmemref,
                  tscType8 vshift,
                  tscType8 voutbuf,
                  tscType8 ireadamp,
                  tscType8 ishift,
                  tscType8 imuxbuf,
                  tscType8 ioutbuf,
                  tscType8 ispare,
                  tscType8 upsetreg                 
		   )
   {
    setCR (cr) ;
    setLatency (latency) ;
    setChipID (chipID);
  
    setVmemref (vmemref);
    setVshift (vshift);
    setVoutbuf (voutbuf);
    setIreadamp (ireadamp);
    setIshift (ishift);
    setImuxbuf  (imuxbuf);
    setIoutbuf  (ioutbuf);
    setIspare   (ispare);
    setUpsetReg (upsetreg);
   
  }


  /** \brief Return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getCR    ( ) { return (values_[PACE_DESC_CR]); }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B registe
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getLatency ( ) { return (values_[PACE_DESC_LATENCY]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType16 getChipID ( ) {
     tscType16 val = ((tscType16)~values_[PACE_DESC_CHIPID0]) << 8 ; 
     val |= 0x00ff & (~values_[PACE_DESC_CHIPID1]) ;
     return (val) ; 
  }

 
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getVmemref  ( ) { return (values_[PACE_DESC_VMEMREF]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getVshift    ( ) { return (values_[PACE_DESC_VSHIFT]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getVoutbuf    ( ) { return (values_[PACE_DESC_VOUTBUF]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getIreadamp    ( ) { return (values_[PACE_DESC_IREADAMP]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getIshift   ( ) { return (values_[PACE_DESC_ISHIFT]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getImuxbuf  ( ) { return (values_[PACE_DESC_IMUXBUF]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getIoutbuf    ( ) { return (values_[PACE_DESC_IOUTBUF]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getIspare  ( ) { return (values_[PACE_DESC_ISPARE]) ; }
  /** \brief return the value of the PACE3B register corresponding to an PACE3B register
   * Return the value of the PACE3B register corresponding to an PACE3B register
   * \return the value
   */
inline  tscType8 getUpsetReg     ( ) { return (values_[PACE_DESC_UPSETREG]) ; }


inline void setValue(int reg, tscType8 val) { 
   // reminder there should be protection on reg
   values_[reg] = val ; 
};


 
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setCR    ( tscType8 cr    ) { values_[PACE_DESC_CR] = cr ; }
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setLatency ( tscType8 latency ) { values_[PACE_DESC_LATENCY] = latency ; }

  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setChipID    ( tscType16 chipid  ) { 
     values_[PACE_DESC_CHIPID0] = 0x00ff & ((~chipid)>>8) ;
     values_[PACE_DESC_CHIPID1] = 0x00ff & (~chipid) ;
  }




  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setVmemref ( tscType8 vmem ) { values_[PACE_DESC_VMEMREF] = vmem ; }
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setVshift ( tscType8 vshift ) { values_[PACE_DESC_VSHIFT] = vshift ; }
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setVoutbuf ( tscType8 voutbuf ) { values_[PACE_DESC_VOUTBUF] = voutbuf ; }
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setIreadamp ( tscType8 ireadamp ) { values_[PACE_DESC_IREADAMP] = ireadamp ; }
  
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setIshift  ( tscType8 ishift    ) {values_[PACE_DESC_ISHIFT] = ishift ; }
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setImuxbuf ( tscType8 imuxbuf ) {values_[PACE_DESC_IMUXBUF]  = imuxbuf ; }
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setIoutbuf ( tscType8 ioutbuf ) { values_[PACE_DESC_IOUTBUF] = ioutbuf ; }
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setIspare ( tscType8 vspare  ) { values_[PACE_DESC_ISPARE] = vspare ; }
  /** \brief set the value for register
   * Set the value for an PACE3B description
   * \param Status - value to be set
   */
inline  void setUpsetReg ( tscType8 upsetreg ) { values_[PACE_DESC_UPSETREG] = upsetreg ; }
 


 /** \brief get the value for register
   * get the value for an PACE3B description
   * \param reg - register number to query
   */

inline  tscType8 getValue(int reg) { 
    // reminder : need a protection on reg val 
    return (values_[reg]) ; 
  } 

 /** \brief get the value for register
   * get the masked value for an PACE3B description
   * \param reg - register number to query
   */

inline  tscType8 getMaskedValue(int reg) { 
    return (values_[reg] & comp_mask_[reg]) ; 
  } 



  /** \brief In order to compare two PACE3B descriptions
   * \param pace - description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( paceDescription &pace ) {

#ifdef DEBUGMSGERROR
    displayDifferences ( pace ) ;
#endif

   
      return (!((*this)==pace) )  ;
  }
  
  /** \brief In order to compare two PACE3B descriptions
   * \param pace - description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( paceDescription &pace ) {

#ifdef DEBUGMSGERROR
   displayDifferences ( pace ) ;
#endif
   bool flag = true ; 
   int i ; 

   for(i=0;i<PACE_DESC_NUM;i++) {
     if (getMaskedValue(i)!=pace.getMaskedValue(i) ) flag = false ; 
     
   }
      return flag ;
  }

  /** Clone a device description
   * \return the device description cloned
   * \warning to not forget to delete the result once you have finished with it
   */
  paceDescription *clone ( ) {

    paceDescription *pace = new paceDescription (*this) ;
    //pace->setFecHardwareId(fecHardwareId_) ;
    //pace->setEnabled(enabled_) ;
    return (pace) ;
  }

  /** \brief Display the PACE values where the comparison is different
   * \param pace - uploaded values
   */
  void displayDifferences ( paceDescription &pace ) {

    char msg[80] ;
    decodeKey (msg, accessKey_) ;
    std::cout << "PACE " <<  msg << std::endl ;
    int i ; 



   for(i=0;i<PACE_DESC_NUM;i++) {
     if (getMaskedValue(i)!=pace.getMaskedValue(i) ) { 
       std::cout << "Reg."  << names[i] << " differ : " ; 
       std::cout << (int) getValue(i)  << "/" ; 
       std::cout << (int) pace.getValue(i) << " " ; 
       std::cout << "(0x" << std::hex << std::setw(2) << std::setfill('0') << (int) getValue(i) << "/" ;
       std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int) pace.getValue(i) << ")" ;
       std::cout << std::setfill(' ') << std::dec  ; 
     }      
   }

   std::cout << std::endl  << std::endl ; 
  }

  /** \brief Display the PACE values
   */
  void display ( ) {

    std::cout << "Pace: 0x" << std::setw(2) << std::setfill('0') << std::hex << getFecSlot() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getRingSlot() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getCcuAddress() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getChannel() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getAddress() ;
    std::cout << std::dec << std::endl ; 

    //std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;

    int i; 

   for(i=0;i<PACE_DESC_NUM;i++) {
     
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

// W.B. important : access class definition has to be after descritpion : 
//
//#include "paceAccess.h"

#endif
