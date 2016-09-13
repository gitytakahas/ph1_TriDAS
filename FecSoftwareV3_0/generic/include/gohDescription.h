/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef GOHDESCRIPTION_H
#define GOHDESCRIPTION_H

#include <iostream>
#include <iomanip>

#include "tscTypes.h"
#include "deviceDescription.h"
#include "gohDefinition.h"

#define GOH_DESC_CONTROL_0       0
#define GOH_DESC_CONTROL_1       1
#define GOH_DESC_CONTROL_2       2
#define GOH_DESC_CONTROL_3       3
#define GOH_DESC_STATUS_0        4
#define GOH_DESC_STATUS_1        5
 

#define GOH_DESC_NUM 6
#define GOH_DESC_PAR_NUM 6



/**
 * \class gohDescription
 * This class give a description of all the GOH registers:
 * <ul>
 * <li>Control0;
 * <li>Control1;
 * <li>Control2;
 * <li>Control3;
 * <li>Status0;
 * <li>Status1; 
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can be set also
 * \see gohDefinition.h 
 *      and the offset of the different registers
 * \include "gohDefinition.h"
 * \author Wojciech BIALAS
 * \date March 2004
 * \brief This class give a description of all the GOH3B registers
 */
class gohDescription: public deviceDescription {

 private:
  

  tscType8 values_[GOH_DESC_NUM] ; // Goh3B register replicas

  static const tscType8 comp_mask_[] ; // comparision mask

  static const  char *names[] ; // register names 

  static const  char *parnames[] ; // parameter names

  static const  int  regoffset[] ; // parameter offset table 

  static const  ParameterDescription::enumTscType   parTscType[] ; // parametar type table 

 public:
  /** \brief Constructor in order to zero all the parameters
   * Build a description of an GOH3B with all the values specified:
   * \param accessKey - key access of a device


   
   */
  gohDescription (keyType accessKey = 0x00000000 ):
    deviceDescription ( GOH, accessKey ) {

     
     int i; 
     for(i=0;i<GOH_DESC_NUM;i++) values_[i] = 0x00 ; 


  }

  /** \brief Constructor in order to set all the parameters
   * Build a description of an GOH3B with all the values specified:
   * \param accessKey - key access of a device
   */
  gohDescription (keyType accessKey,
		  tscType8 control0,
                  tscType8 control1,
                  tscType8 control2,
          
                  tscType8 control3,
                  tscType8 status0,
                  tscType8 status1

                  ):
    deviceDescription ( GOH, accessKey ) {

    setDescriptionValues (control0,
                   control1,
                   control2,
                   
                   control3,
                   status0,
                   status1                 
			  );
  }

 /** \brief Constructor in order to set all the parameters
   * Build a description of an GOH3B with all the values specified:   
   */
  gohDescription (
		   tscType16 fecSlot,
                  tscType16 ringSlot,
                  tscType16 ccuAddress,
                  tscType16 i2cChannel,
                  tscType16 i2cAddress, 
		  tscType8 control0,
                  tscType8 control1,
                  tscType8 control2,
          
                  tscType8 control3,
                  tscType8 status0,
                  tscType8 status1
                
                  ):
    deviceDescription ( GOH, fecSlot, ringSlot, ccuAddress, i2cChannel,
			i2cAddress) {
    setDescriptionValues (control0,
                   control1,
                   control2,
                   
                   control3,
                   status0,
                   status1                 
			  );
  }



  /** \brief Constructor in order to set all the parameters
   * Build a description of an GOH3B from other GOH3B description:
   * \param goh - gohDescription class 
 
   
   */
  gohDescription (gohDescription& goh) :
    deviceDescription ( GOH, goh.getKey() ) {

    int i ; 
    for(i=0; i < GOH_DESC_NUM ; i++ ) { 
      values_[i] = goh.getValue(i) ; 
    } ; 
  }



  
/** \brief Constructor in order to set all the parameters
   * Build a description of an GOH3B from parameter names:
   * \param parameterNames - parameter hash table  
 
   
   */
  gohDescription (parameterDescriptionNameType parameterNames);


/** \brief Constructor in order to set all the parameters
   * Build a description of an GOH3B with all the values specified:
   * \param accessKey - key access of a device
   */
void    setDescriptionValues (
                  tscType8 control0,
                  tscType8 control1,
                  tscType8 control2,
                  tscType8 control3,
                  tscType8 status0,
                  tscType8 status1
		   )
   {
   setControl0 (control0) ;
   setControl1 (control1) ;
   setControl2 (control2) ;
   setControl3 (control3) ;
   setStatus0 (status0) ;
   setStatus1 (status1) ;   
  }


  /** \brief Return the value of the GOH register corresponding to an GOH register
   * Return the value of the GOH register corresponding to an GOH register
   * \return the value
   */
inline  tscType8 getControl0    ( ) { return (values_[GOH_DESC_CONTROL_0]); }

  /** \brief Return the value of the GOH register corresponding to an GOH register
   * Return the value of the GOH register corresponding to an GOH register
   * \return the value
   */
inline  tscType8 getControl1   ( ) { return (values_[GOH_DESC_CONTROL_1]); }

  /** \brief Return the value of the GOH register corresponding to an GOH register
   * Return the value of the GOH register corresponding to an GOH register
   * \return the value
   */
inline  tscType8 getControl2    ( ) { return (values_[GOH_DESC_CONTROL_2]); }

  /** \brief Return the value of the GOH register corresponding to an GOH register
   * Return the value of the GOH register corresponding to an GOH register
   * \return the value
   */
inline  tscType8 getControl3    ( ) { return (values_[GOH_DESC_CONTROL_3]); }

  /** \brief Return the value of the GOH register corresponding to an GOH register
   * Return the value of the GOH register corresponding to an GOH register
   * \return the value
   */
inline  tscType8 getStatus0    ( ) { return (values_[GOH_DESC_STATUS_0]); }

  /** \brief Return the value of the GOH register corresponding to an GOH register
   * Return the value of the GOH register corresponding to an GOH register
   * \return the value
   */
inline  tscType8 getStatus1    ( ) { return (values_[GOH_DESC_STATUS_1]); }


inline void setValue(int reg, tscType8 val) { 
   // reminder there should be protection on reg
   values_[reg] = val ; 
};

 
  /** \brief set the value for register
   * Set the value for an GOH description
   * \param Status - value to be set
   */
inline  void setControl0    ( tscType8 cr    ) { values_[GOH_DESC_CONTROL_0] = cr ; }
  /** \brief set the value for register
   * Set the value for an GOH description
   * \param Status - value to be set
   */
inline  void setControl1    ( tscType8 cr    ) { values_[GOH_DESC_CONTROL_1] = cr ; }
  /** \brief set the value for register
   * Set the value for an GOH description
   * \param Status - value to be set
   */
inline  void setControl2    ( tscType8 cr    ) { values_[GOH_DESC_CONTROL_2] = cr ; }
  /** \brief set the value for register
   * Set the value for an GOH description
   * \param Status - value to be set
   */
inline  void setControl3    ( tscType8 cr    ) { values_[GOH_DESC_CONTROL_3] = cr ; }
  /** \brief set the value for register
   * Set the value for an GOH description
   * \param Status - value to be set
   */
inline  void setStatus0    ( tscType8 cr    ) { values_[GOH_DESC_STATUS_0] = cr ; }
  /** \brief set the value for register
   * Set the value for an GOH description
   * \param Status - value to be set
   */
inline  void setStatus1    ( tscType8 cr    ) { values_[GOH_DESC_STATUS_1] = cr ; }

 /** \brief get the value for register
   * get the value for an GOH description
   * \param reg - register number to query
   */

inline  tscType8 getValue(int reg) { 
    // reminder : need a protection on reg val 
    return (values_[reg]) ; 
  } 

 /** \brief get the value for register
   * get the masked value for an GOH description
   * \param reg - register number to query
   */

inline  tscType8 getMaskedValue(int reg) { 
    return (values_[reg] & comp_mask_[reg]) ; 
  } 



  /** \brief In order to compare two GOH descriptions
   * \param goh - description to be compared
   * \return bool - true if one value are different, false if not
   */
  bool operator!= ( gohDescription &goh ) {

#ifdef DEBUGMSGERROR
    displayDifferences ( goh ) ;
#endif

   
      return (!((*this)==goh) )  ;
  }
  
  /** \brief In order to compare two GOH descriptions
   * \param goh - description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( gohDescription &goh ) {

#ifdef DEBUGMSGERROR
   displayDifferences ( goh ) ;
#endif
   bool flag = true ; 
   int i ; 

   for(i=0;i<GOH_DESC_NUM;i++) {
     if (getMaskedValue(i)!=goh.getMaskedValue(i) ) flag = false ; 
     
   }
      return flag ;
  }

  /** Clone a device description
   * \return the device description cloned
   * \warning to not forget to delete the result once you have finished with it
   */
  gohDescription *clone ( ) {

    gohDescription *goh = new gohDescription (*this) ;
    //goh->setFecHardwareId(fecHardwareId_) ;
    //goh->setEnabled(enabled_) ;
    return (goh) ;
  }

  /** \brief Display the GOH values where the comparison is different
   * \param goh - uploaded values
   */
  void displayDifferences ( gohDescription &goh ) {

    char msg[80] ;
    decodeKey (msg, accessKey_) ;
    std::cout << "GOH " <<  msg << std::endl ;
    int i ; 



   for(i=0;i<GOH_DESC_NUM;i++) {
     if (getMaskedValue(i)!=goh.getMaskedValue(i) ) { 
       std::cout << "Reg."  << names[i] << " differ : " ; 
       std::cout << (int) getValue(i)  << "/" ; 
       std::cout << (int) goh.getValue(i) << " " ; 
       std::cout << "(0x" << std::hex << std::setw(2) << std::setfill('0') << (int) getValue(i) << "/" ;
       std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int) goh.getValue(i) << ")" ;
       std::cout << std::setfill(' ') << std::dec  ; 
     }      
   }

   std::cout << std::endl  << std::endl ; 
  }

  /** \brief Display the GOH values
   */
  void display ( ) {

    std::cout << "Goh: 0x" << std::setw(2) << std::setfill('0') << std::hex << getFecSlot() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getRingSlot() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getCcuAddress() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getChannel() ;
    std::cout << ":0x" << std::setw(2) << std::setfill('0') << std::hex << getAddress() ;
    std::cout << std::dec << std::endl ; 

    //std::cout << "\tenabled: " << std::dec << isEnabled() << std::endl;

    int i; 

   for(i=0;i<GOH_DESC_NUM;i++) {
     
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
   * Set the value for an Delta description. 
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
//#include "gohAccess.h"

#endif
