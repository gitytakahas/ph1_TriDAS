/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Fec Software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fec Software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace,
Mulhouse-France
*/
#ifndef APVDEFINITION_H
#define APVDEFINITION_H

// This file defined all the offset for APV registers

// Note that the APVs are accessible in extended (RAL) mode (I2C)
// Offset of 1 for reading value
#define APV25_READ  0x01
// Offset of 0 for writing value
#define APV25_WRITE 0x00

// Registers are:
#define APV25_ERR_REG 0x00  
#define APV25_MOD_REG 0x02  /* +x */ /* x=1 : read i; x=0 : write */
#define APV25_LAT_REG 0x04  /* +x */

#define APV25_MUXGAIN 0x06  /* +x */
#define APV25_IPRE    0x20  /* +x */
#define APV25_IPCASC  0x22  /* +x */
#define APV25_IPSF    0x24  /* +x */
#define APV25_ISHA    0x26  /* +x */
#define APV25_ISSF    0x28  /* +x */
#define APV25_IPSP    0x2a  /* +x */
#define APV25_IMUXIN  0x2c  /* +x */
#define APV25_ICAL    0x30  /* +x */
#define APV25_VFP     0x32  /* +x */
#define APV25_VFS     0x34  /* +x */
#define APV25_VPSP    0x36  /* +x */
#define APV25_CDRV    0x38  /* +x */
#define APV25_CSEL    0x3a  /* +x */

// Error Register Definition 
#define APV25_LAT_ERR 0x01
#define APV25_FIFO_ERR 0x02

// Mode Register Definition 
#define APV25_BIAS     0x01
#define APV25_TRIG     0x02
#define APV25_CALIB    0x04
#define APV25_READOUT  0x08
#define APV25_FREQ     0x10
#define APV25_POLARITY 0x20

// Mask of the values 
// - 0 means that the value read cannot be compare with the value written
// - 0xFFFF means that the value must be equal with the value written 
#define APV25_ERR_CMP 0x0000
#define APV25_MOD_CMP 0xFFFF
#define APV25_LAT_CMP 0x04

#define APV25_MUXGAIN_CMP 0x06
#define APV25_IPRE_CMP    0x20
#define APV25_IPCASC_CMP  0x22
#define APV25_IPSF_CMP    0x24
#define APV25_ISHA_CMP    0x26
#define APV25_ISSF_CMP    0x28
#define APV25_IPSP_CMP    0x2a
#define APV25_IMUXIN_CMP  0x2c
#define APV25_ICAL_CMP    0x30
#define APV25_VFP_CMP     0x32
#define APV25_VFS_CMP     0x34
#define APV25_VPSP_CMP    0x36
#define APV25_CDRV_CMP    0x38
#define APV25_CSEL_CMP    0x3a

#endif
