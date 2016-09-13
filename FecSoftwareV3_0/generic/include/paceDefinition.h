/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
Copyright: 2004 
based on apvDefinition class by 
Frederic DROUHIN - Universite de Haute-Alsace,
Mulhouse-France
*/
#ifndef PACEDEFINITION_H
#define PACEDEFINITION_H

// This file defined all the offset for DELTA registers

// Note that the PACEAMs are accessible in extended (RAL) mode (I2C)
// Offset of 1 for reading value

#define PACE_REG_NUM 13 

#define PACE_READ          0x01 

// Registers are:
#define PACE_CR_REG        0x00  
#define PACE_LATENCY_REG   0x01  /* +x */ /* x=1 : read i; x=0 : write */
#define PACE_CHIPID0_REG   0x02  /* +x */
#define PACE_CHIPID1_REG   0x03  /* +x */
#define PACE_VMEMREF_REG   0x04  /* +x */
#define PACE_VSHIFT_REG    0x05  /* +x */
#define PACE_VOUTBUF_REG   0x06  /* +x */
#define PACE_IREADAMP_REG  0x07  /* +x */
#define PACE_ISHIFT_REG    0x08  /* +x */
#define PACE_IMUXBUF_REG   0x09  /* +x */
#define PACE_IOUTBUF_REG   0x0a  /* +x */
#define PACE_ISPARE_REG    0x0b  /* +x */
#define PACE_UPSET_REG     0x0c  /* +x */


// Mode Register Definition 
//#define PACE_RUN      0x01




#endif
