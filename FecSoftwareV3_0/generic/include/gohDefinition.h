/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
Copyright: 2004 
based on apvDefinition class by 
Frederic DROUHIN - Universite de Haute-Alsace,
Mulhouse-France
*/
#ifndef GOHDEFINITION_H
#define GOHDEFINITION_H

// This file defined all the offset for GOH registers

// Note that the PACEAMs are accessible in normal mode (I2C)

#define GOH_REG_NUM 6 

#define GOH_READ          0x01 

// Registers are:
#define GOH_CONTROL_0        0x00  
#define GOH_CONTROL_1        0x01
#define GOH_CONTROL_2        0x02
#define GOH_CONTROL_3        0x03
#define GOH_STATUS_0         0x04  
#define GOH_STATUS_1         0x05

#endif
