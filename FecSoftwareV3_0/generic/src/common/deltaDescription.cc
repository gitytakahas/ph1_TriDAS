/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
based on apvDescription class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include <stdio.h>
#include "deltaDescription.h"

deltaDescription::deltaDescription (parameterDescriptionNameType parameterNames) 
: deviceDescription (DELTA, parameterNames) { 

    int i ; 
    for(i=0; i < DELTA_DESC_PAR_NUM ; i++ ) { 
      ParameterDescription *pd = parameterNames[parnames[i]] ;
      if (pd) { 
	// int n = pd->getTypeSize() ;
	// have to finish...
	// cout << "Delta construction: parameter name is : " << pd->getName() << endl ;  
	setValueVarSize(regoffset[i],pd->getValueConverted(),pd->getType()) ;
      } 
      
    } ;  

} 



parameterDescriptionNameType* deltaDescription::getParameterNames ( ) {
 
    parameterDescriptionNameType *parameterNames = deviceDescription::getParameterNames() ;
    if (parameterNames) { 
      for (unsigned int i = 0 ; i < DELTA_DESC_PAR_NUM ; i ++) {
	(*parameterNames)[parnames[i]] = new ParameterDescription (parnames[i], parTscType[i]) ;
      }
    }  
    return parameterNames ;
  }

const char* deltaDescription::names[] = { "CR0" , "CR1" , "ChipID0","ChipID1","CalChan0",
			    "CalChan1","CalChan2","CalChan3","Vcal","VoPreamp",
			    "VoShaper","Vspare","Ipreamp","Ishaper","ISF",
			    "Ispare" } ;

const char* deltaDescription::parnames[] = { "CR0" , "CR1" , "ChipID","CalChan",
			    "Vcal","VoPreamp",
			    "VoShaper","Vspare","Ipreamp","Ishaper","ISF",
			    "Ispare" } ;

const int  deltaDescription::regoffset[] = {0,1,2,4,8,9,10,11,12,13,14,15} ;

const ParameterDescription::enumTscType   deltaDescription::parTscType[] = {ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER16,ParameterDescription::INTEGER32,
			      ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,
			      ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8,ParameterDescription::INTEGER8 } ; 


 
const tscType8 deltaDescription::comp_mask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff } ; // i.e. all bits are checked 


