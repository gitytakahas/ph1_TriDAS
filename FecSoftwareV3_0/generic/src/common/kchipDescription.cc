/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include <stdio.h>
#include "kchipDescription.h"

//  kchipDescription::kchipDescription (kchipAccess& kchip) :
//     deviceDescription ( KCHIP, kchip.getKey() ) {

//     int i ; 
//     for(i=0; i < KCHIP_DESC_NUM ; i++ ) { 
//       values_[i] = kchip.getDescriptionValue(i) ; 
//     } ; 
//   }


kchipDescription::kchipDescription (parameterDescriptionNameType parameterNames) 
: deviceDescription (KCHIP, parameterNames) { 

    int i ; 
    for(i=0; i < KCHIP_DESC_PAR_NUM ; i++ ) { 
      ParameterDescription *pd = parameterNames[parnames[i]] ;
      if (pd) { 
	 std::cout << "parameter name is: " << pd->getName() << std::endl ; 
	  setValueVarSize(regoffset[i],pd->getValueConverted(),pd->getType()) ; 
      } 
      
    } ;  

} 



parameterDescriptionNameType* kchipDescription::getParameterNames ( ) {
 
    parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;
    if (parameterNames) { 
      for (unsigned int i = 0 ; i < KCHIP_DESC_PAR_NUM ; i ++) {
	(*parameterNames)[parnames[i]] = new ParameterDescription (parnames[i], parTscType[i]) ;
      }
    }  
    return parameterNames ;
  }

const char* kchipDescription::names[] = { 
   "Config" , "EConfig" , "KID_L","KID_H",
   "Mask_T1cmd","Last_T1cmd","latency","Evcnt",
   "Bnchcnt_L","Bnchcnt_H","GintBUSY","GintIDLE",
   "Status0",
   "Status1","SEUcnt","CalPulseDly","CalPulseW",
   "AdcLatency","PFifoOvfl","ChipID0","ChipID1" } ; 

const char* kchipDescription::parnames[] = { 
  "Config" , // 0 
  "EConfig" , // 1
  "KID", // 2 
  "Mask_T1cmd", // 4
  "Last_T1cmd", // 5 
  "Latency", // 6 
  "EvCnt", // 7 
  "BnchCnt", // 8
  "GintBUSY", // 10
  "GintIDLE", //11
  "Status0", // 12
  "Status1", // 13
  "SeuCnt", // 14
  "CalPulseDelay",// 15
  "CalPulseWidth", // 16
  "AdcLatency", // 17 
  "PFifoOvfl", // 18
  "ChipID" // 19 
} ; 

const int  kchipDescription::regoffset[] = {0,1,2,4,5,6,7,8,10,11,12,13,14,15,16,17,18,19} ;

const ParameterDescription::enumTscType   kchipDescription::parTscType[] = {
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER16,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER16,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER16  
} ; 


const tscType8 kchipDescription::comp_mask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff } ; // i.e. all bits are checked 

