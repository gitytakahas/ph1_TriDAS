/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/


#include <stdio.h>
#include "gohDescription.h"


gohDescription::gohDescription (parameterDescriptionNameType parameterNames) 
: deviceDescription (GOH, parameterNames) { 

    int i ; 
    for(i=0; i < GOH_DESC_PAR_NUM ; i++ ) { 
      ParameterDescription *pd = parameterNames[parnames[i]] ;
      if (pd) { 

	  setValueVarSize(regoffset[i],pd->getValueConverted(),pd->getType()) ; 

      } 
      
    } ;  

} 



parameterDescriptionNameType* gohDescription::getParameterNames ( ) {

    parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;
    if (parameterNames) { 
      for (unsigned int i = 0 ; i < GOH_DESC_PAR_NUM ; i ++) {
	(*parameterNames)[parnames[i]] = new ParameterDescription (parnames[i], parTscType[i]) ;
      }
    }  
    return parameterNames ;
  }

const char* gohDescription::names[] = { "Control0" , "Control1" , "Control2",
					"Control3","Status0","Status1"} ;
const char* gohDescription::parnames[] = {"Control0" , "Control1" , "Control2",
					"Control3","Status0","Status1"} ; 
const int  gohDescription::regoffset[] = {0,1,2,3,4,5} ;

const ParameterDescription::enumTscType   gohDescription::parTscType[] = {
  ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,
  ParameterDescription::INTEGER8,ParameterDescription::INTEGER8 } ; 

const tscType8 gohDescription::comp_mask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff} ; // i.e. all bits are checked 

