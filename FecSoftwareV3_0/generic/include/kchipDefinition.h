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

#define DELTA_REG_NUM 24 



// Physical Registers are:


#define KCHIP_CFG_REG       0 
#define KCHIP_ECFG_REG      1 
#define KCHIP_KID_L_REG  2 
#define KCHIP_KID_H_REG  3
#define KCHIP_MASK_T1CMD_REG  4
#define KCHIP_LAST_T1CMD_REG  5
#define KCHIP_LATENCY_REG    6 
#define KCHIP_EVCNT_REG 7
#define KCHIP_BNCHCNT_L_REG 8
#define KCHIP_BNCHCNT_H_REG 9 
#define KCHIP_RESERVED_0A_REG 10 
#define KCHIP_GINT_BUSY_REG 11
#define KCHIP_GINT_IDLE_REG 12 
#define KCHIP_FIFOMAP_REG 13  
#define KCHIP_FIFODATA_H_REG 14
#define KCHIP_FIFODATA_L_REG 15
#define KCHIP_SR0_REG 16
#define KCHIP_SR1_REG 17
#define KCHIP_SEU_COUNTER_REG 18
#define KCHIP_CALPULSE_DELAY_REG 19
#define KCHIP_CALPULSE_WIDTH_REG 20
#define KCHIP_ADC_LATENCY_REG 21
#define KCHIP_PFIFO_OVERFLOW_REG 22
#define KCHIP_RESERVED_17_REG 23
#define KCHIP_RESERVED_18_REG 24
#define KCHIP_RESERVED_19_REG 25
#define KCHIP_RESERVED_1A_REG 26
#define KCHIP_RESERVED_1B_REG 27
#define KCHIP_RESERVED_1C_REG 28
#define KCHIP_RESERVED_1D_REG 29
#define KCHIP_CHIPID0_REG 30
#define KCHIP_CHIPID1_REG 31
 



#endif
