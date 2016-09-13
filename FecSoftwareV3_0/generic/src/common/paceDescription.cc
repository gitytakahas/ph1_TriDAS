/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/


#include <stdio.h>
#include "paceDescription.h"



paceDescription::paceDescription (parameterDescriptionNameType parameterNames) 
: deviceDescription (PACE, parameterNames) { 

    int i ; 
    for(i=0; i < PACE_DESC_PAR_NUM ; i++ ) { 
      ParameterDescription *pd = parameterNames[parnames[i]] ;
      if (pd) { 

	  setValueVarSize(regoffset[i],pd->getValueConverted(),pd->getType()) ; 

      } 
      
    } ;  

} 



parameterDescriptionNameType* paceDescription::getParameterNames ( ) {
 
    parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;
    if (parameterNames) { 
      for (unsigned int i = 0 ; i < PACE_DESC_PAR_NUM ; i ++) {
	(*parameterNames)[parnames[i]] = new ParameterDescription (parnames[i], parTscType[i]) ;
      }
    }  
    return parameterNames ;
  }

const char* paceDescription::names[] = { "CR" , "Latency" , "ChipID0","ChipID1","Vmemref",
			    "Vshift","Voutbuf","Ireadamp","Ishift","Imuxbuf",
			    "Ioutbuf","Ispare","Upset"} ;
const char* paceDescription::parnames[] = { "CR" , "Latency" ,"ChipID","Vmemref",
			    "Vshifter","Voutbuf","Ireadamp","Ishifter","Imuxbuf",
			    "Ioutbuf","Ispare","Upset"} ; 
const int  paceDescription::regoffset[] = {0,1,2,4,5,6,7,8,9,10,11,12} ;

const ParameterDescription::enumTscType   paceDescription::parTscType[] = {
  ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER16,ParameterDescription::INTEGER16,
  ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8 } ; 

const tscType8 paceDescription::comp_mask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff } ; // i.e. all bits are checked 

