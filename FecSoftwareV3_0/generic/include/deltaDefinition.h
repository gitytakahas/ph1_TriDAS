/*
This file is part of Fec Software project.

author: Wojciech BIALAS, CERN, Geneva, Switzerland
Copyright: 2004 
based on apvDefinition class by 
Frederic DROUHIN - Universite de Haute-Alsace,
Mulhouse-France
*/
#ifndef DELTADEFINITION_H
#define DELTADEFINITION_H

// This file defined all the offset for DELTA registers

// Note that the DELTAs are accessible in extended (RAL) mode (I2C)
// Offset of 1 for reading value

#define DELTA_REG_NUM 16 

#define DELTA_READ  0x01
// Offset of 0 for writing value
#define DELTA_WRITE 0x00

// Registers are:
#define DELTA_CR0_REG       0x00  
#define DELTA_CR1_REG       0x02  /* +x */ /* x=1 : read i; x=0 : write */
#define DELTA_CHIPID0_REG   0x04  /* +x */

#define DELTA_CHIPID1_REG   0x06  /* +x */
#define DELTA_CALCHAN0_REG  0x08  /* +x */
#define DELTA_CALCHAN1_REG  0x0a  /* +x */
#define DELTA_CALCHAN2_REG  0x0c  /* +x */
#define DELTA_CALCHAN3_REG  0x0e  /* +x */
#define DELTA_VCAL_REG      0x10  /* +x */
#define DELTA_VOPREAMP_REG  0x12  /* +x */
#define DELTA_VOSHAPER_REG  0x14  /* +x */
#define DELTA_VSPARE_REG    0x16  /* +x */
#define DELTA_IPREAMP_REG   0x18  /* +x */
#define DELTA_ISHAPER_REG   0x1a  /* +x */
#define DELTA_ISF_REG       0x1c  /* +x */
#define DELTA_ISPARE_REG    0x1e  /* +x */



// Mode Register Definition 
#define DELTA_RUN      0x01
#define DELTA_GAIN     0x02
#define DELTA_CALIB    0x0c
#define DELTA_READOUT  0x10

// Mask of the values 
// - 0 means that the value read cannot be compare with the value written
// - 0xFFFF means that the value must be equal with the value written 
#define DELTA_CR0_CMP       0xff  
#define DELTA_CR1_CMP       0xff  /* +x */ /* x=1 : read i; x=0 : write */
#define DELTA_CHIPID0_CMP   0x00  /* +x */

#define DELTA_CHIPID1_CMP   0x00  /* +x */
#define DELTA_CALCHAN0_CMP  0xff  /* +x */
#define DELTA_CALCHAN1_CMP  0xff  /* +x */
#define DELTA_CALCHAN2_CMP  0xff  /* +x */
#define DELTA_CALCHAN3_CMP  0xff  /* +x */
#define DELTA_VCAL_CMP      0xff  /* +x */
#define DELTA_VOPREAMP_CMP  0xff  /* +x */
#define DELTA_VOSHAPER_CMP  0xff  /* +x */
#define DELTA_VSPARE_CMP    0xff  /* +x */
#define DELTA_IPREAMP_CMP   0xff  /* +x */
#define DELTA_ISHAPER_CMP   0xff  /* +x */
#define DELTA_ISF_CMP       0xff  /* +x */
#define DELTA_ISPARE_CMP    0xff  /* +x */

#endif
