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

  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef FECRINGREGISTERS_H
#define FECRINGREGISTERS_H

#include <iostream>
#include <string>


#include "keyType.h"
#include "tscTypes.h"
#include "cmdDescription.h"

/**
 * \class FecRingRegisters
 * Store all the information for a given ring, this class does not read the hardware, it is up to the remote method to do it
 */
class FecRingRegisters {  
  
 private:

  /** FEC registers in the order, SR0, SR1, CR0
   */
  tscType16 fecRingRegisters_[FECCR0+1] ;

  /* CCU registers SRA, SRB, SRC, SRD, SRE, SRF, CRA, CRB, CRC, CRD, CRE
   */
  tscType32 ccuRegisters_[CCUCRE+1] ;

  /** i2c registers
   */
  tscType8 i2cRegisters_[I2CCRA+1] ;

  /** memory registers
   */
  tscType8 memoryRegisters_[MEMORYCRA+1] ;

  /** PIA registers
   */
  tscType8 piaRegisters_[PIAGCR+1] ;

  /** index
   */
  keyType index_ ;
  
 public:

  /** This constructor is just here to set default value and to clone another FecRingRegisters
   */
  FecRingRegisters ( ) {

    index_ = 0 ;

    for (int i = 0 ; i <= FECCR0 ; i ++)    fecRingRegisters_[i] = 0xFFFF ;
    for (int i = 0 ; i <= CCUCRE ; i ++)    ccuRegisters_    [i] = 0xFFFFFFFF ;
    for (int i = 0 ; i <= I2CCRA ; i ++)    i2cRegisters_    [i] = 0xFF ;
    for (int i = 0 ; i <= MEMORYCRA ; i ++) memoryRegisters_ [i] = 0xFF ;
    for (int i = 0 ; i <= PIAGCR ; i ++)    piaRegisters_    [i] = 0xFF ;
  }
  
  /** This constructor just initialise the different variables for the registers
   */
  FecRingRegisters ( keyType index ) {
    
    index_ = index ;

#ifdef DEBUGMSGERROR
    char msg[80] ;
    decodeKey(msg,index) ;
    std::cout << "FecRingRegisters::FecRingRegisters ( keyType ): key = " << msg << std::endl ;
#endif

    for (int i = 0 ; i <= FECCR0 ; i ++)    fecRingRegisters_[i] = 0xFFFF ;
    for (int i = 0 ; i <= CCUCRE ; i ++)    ccuRegisters_    [i] = 0xFFFFFFFF ;
    for (int i = 0 ; i <= I2CCRA ; i ++)    i2cRegisters_    [i] = 0xFF ;
    for (int i = 0 ; i <= MEMORYCRA ; i ++) memoryRegisters_ [i] = 0xFF ;
    for (int i = 0 ; i <= PIAGCR ; i ++)    piaRegisters_    [i] = 0xFF ;
  }

  /** Create a FecRingRegisters from the values stored in the different array
   */
  FecRingRegisters ( keyType index, tscType16 *fecRingRegisters, tscType32 *ccuRegisters, tscType8 *i2cRegisters, tscType8 *memoryRegisters, tscType8 *piaRegisters ) {

    index_ = index ;
    memcpy (fecRingRegisters_, fecRingRegisters, (FECCR0+1)*sizeof(tscType16)) ;
    memcpy (ccuRegisters_, ccuRegisters, (CCUCRE+1)*sizeof(tscType32)) ;
    memcpy (i2cRegisters_, i2cRegisters, (CCUCRE+1)*sizeof(tscType8)) ;
    memcpy (memoryRegisters_, memoryRegisters, (MEMORYCRA+1)*sizeof(tscType8)) ;
    memcpy (piaRegisters_, piaRegisters, (PIAGCR+1)*sizeof(tscType8)) ;
  }

  /** Return the index specified
   */
  keyType getIndex ( ) {

    return (index_) ;
  }
  
  /** set the FEC SR0 set during an error if value == 0xFFFF then the value was not read
   */
  void setFecSR0 ( tscType16 fecSR0 ) {

    fecRingRegisters_[FECSR0] = fecSR0 ;

  }

  /** get the FEC SR0 set during an error if value == 0xFFFF then the value was not read
   */
  tscType16 getFecSR0 ( ) {

    return fecRingRegisters_[FECSR0] ;
  }

  /** set the FEC SR1 set during an error if value == 0xFFFF then the value was not read
   */
  void setFecSR1 ( tscType16 fecSR1 ) {
    
    fecRingRegisters_[FECSR1] = fecSR1 ;
  }

  /** get the FEC SR1 set during an error if value == 0xFFFF then the value was not read
   */
  tscType16 getFecSR1 ( ) {

    return fecRingRegisters_[FECSR1] ;
  }

  /** set the FEC CR0 set during an error if value == 0xFFFF then the value was not read
   */
  void setFecCR0 ( tscType16 fecCR0 ) {
    
    fecRingRegisters_[FECCR0] = fecCR0 ;
  }
  
  /** get the FEC CR0 set during an error if value == 0xFFFF then the value was not read
   */
  tscType16 getFecCR0 ( ) {

    return fecRingRegisters_[FECCR0] ;
  }

  /** set the CCU SRA set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuSRA ( tscType8 ccuSRA ) {

    ccuRegisters_[CCUSRA] = ccuSRA ;
  }

  /** get the CCU SRA set during an error if value == 0xFFFF then the value was not read
   */
  tscType8 getCcuSRA ( ) {

    return ccuRegisters_[CCUSRA] ;
  }

  /** set the CCU SRA set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuSRB ( tscType8 ccuSRB ) {

    ccuRegisters_[CCUSRB] = ccuSRB ;
  }

  /** get the CCU SRB set during an error if value == 0xFFFF then the value was not read
   */
  tscType8 getCcuSRB ( ) {

    return ccuRegisters_[4] ;
  }

  /** set the CCU SRC set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuSRC ( tscType8 ccuSRC ) {

    ccuRegisters_[CCUSRC] = ccuSRC ;
  }

  /** get the CCU SRC set during an error if value == 0xFFFF then the value was not read
   */
  tscType8 getCcuSRC ( ) {

    return ccuRegisters_[CCUSRC] ;
  }


  /** set the CCU SRD set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuSRD ( tscType8 ccuSRD ) {

    ccuRegisters_[CCUSRD] = ccuSRD ;
  }

  /** get the CCU SRD set during an error if value == 0xFFFF then the value was not read
   */
  tscType8 getCcuSRD ( ) {

    return ccuRegisters_[CCUSRD] ;
  }

  /** set the CCU SRE set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuSRE ( tscType32 ccuSRE ) {

    ccuRegisters_[CCUSRE] = ccuSRE ;
  }

  /** get the CCU SRE set during an error if value == 0xFFFF then the value was not read
   */
  tscType32 getCcuSRE ( ) {

    return ccuRegisters_[CCUSRE] ;
  }
  
  /** set the CCU SRF set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuSRF ( tscType16 ccuSRF ) {

    ccuRegisters_[CCUSRF] = ccuSRF ;
  }

  /** get the CCU SRF set during an error if value == 0xFFFF then the value was not read
   */
  tscType16 getCcuSRF ( ) {

    return ccuRegisters_[CCUSRF] ;
  }

  /** set the CCU SRA set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuCRA ( tscType8 ccuCRA ) {

    ccuRegisters_[CCUCRA] = ccuCRA ; 
  }

  /** get the CCU CRA set during an error if value == 0xFFFF then the value was not read
   */
  tscType8 getCcuCRA ( ) {

    return ccuRegisters_[CCUCRA] ;
  }

  /** set the CCU CRB set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuCRB ( tscType8 ccuCRB ) {

    ccuRegisters_[CCUCRB] = ccuCRB ;
  }

  /** get the CCU CRB set during an error if value == 0xFFFF then the value was not read
   */
  tscType8 getCcuCRB ( ) {

    return ccuRegisters_[CCUCRB] ;
  }

  /** set the CCU CRC set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuCRC ( tscType8 ccuCRC ) {

    ccuRegisters_[CCUCRC] = ccuCRC ;
  }

  /** get the CCU CRC set during an error if value == 0xFFFF then the value was not read
   */
  tscType8 getCcuCRC ( ) {

    return ccuRegisters_[CCUCRC] ;
  }

  /** set the CCU CRD set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuCRD ( tscType8 ccuCRD ) {

    ccuRegisters_[CCUCRD] = ccuCRD ;
  }

  /** get the CCU SRA set during an error if value == 0xFFFF then the value was not read
   */
  tscType8 getCcuCRD ( ) {

    return ccuRegisters_[CCUCRD] ;
  }

  /** set the CCU SRA set during an error if value == 0xFFFF then the value was not read
   */
  void setCcuCRE ( tscType32 ccuCRE ) {

    ccuRegisters_[CCUCRE] = ccuCRE ;
  }

  /** get the CCU CRE set during an error if value == 0xFFFF then the value was not read
   */
  tscType32 getCcuCRE ( ) {

    return ccuRegisters_[CCUCRE] ;
  }

  /** set the i2c channel SRA in the corresponding array
   */
  void seti2cChannelSRA ( tscType8 i2cSRA ) {

    i2cRegisters_[I2CSRA] = i2cSRA ;
  }

  /** get the i2c channel SRA in the correspnding channel
   */
  tscType8 geti2cChannelSRA ( ) {

    return i2cRegisters_[I2CSRA] ;
  }

  /** set the i2c channel SRB in the corresponding array
   */
  void seti2cChannelSRB ( tscType8 i2cSRB ) {

    i2cRegisters_[I2CSRB] = i2cSRB ;
  }

  /** get the i2c channel SRB in the correspnding channel
   */
  tscType8 geti2cChannelSRB ( ) {

    return i2cRegisters_[I2CSRB] ;
  }

  /** set the i2c channel SRC in the corresponding array
   */
  void seti2cChannelSRC ( tscType8 i2cSRC ) {

    i2cRegisters_[I2CSRC] = i2cSRC ;
  }

  /** get the i2c channel SRC in the correspnding channel
   */
  tscType8 geti2cChannelSRC ( ) {

    return i2cRegisters_[I2CSRC] ;
  }

  /** set the i2c channel SRD in the corresponding array
   */
  void seti2cChannelSRD ( tscType8 i2cSRD ) {

    i2cRegisters_[I2CSRD] = i2cSRD ;
  }

  /** get the i2c channel SRD in the correspnding channel
   */
  tscType8 geti2cChannelSRD ( ) {

    return i2cRegisters_[I2CSRD] ;
  }

  /** set the i2c channel CRA in the corresponding array
   */
  void seti2cChannelCRA ( tscType8 i2cCRA ) {

    i2cRegisters_[I2CCRA] = i2cCRA ;
  }

  /** get the i2c channel CRA in the correspnding channel
   */
  tscType8 geti2cChannelCRA ( ) {

    return i2cRegisters_[I2CCRA] ;
  }

  /** set the PIA channel GCR in the corresponding array
   */
  void setPiaChannelGCR ( tscType8 piaGCR ) {

    piaRegisters_[PIAGCR] = piaGCR ;
  }

  /** get the PIA channel GCR in the correspnding channel
   */
  tscType8 getPiaChannelGCR ( ) {

    return piaRegisters_[PIAGCR] ;
  }

  /** set the PIA channel SR in the corresponding array
   */
  void setPiaChannelSR ( tscType8 piaSR ) {

    piaRegisters_[PIASR] = piaSR ;
  }

  /** get the PIA channel SR in the correspnding channel
   */
  tscType8 getPiaChannelSR ( ) {

    return piaRegisters_[PIASR] ;
  }

  /** clone the current registers in the parameter
   * \param fecRingRegisters - fecRingRegisters to be copied
   */
  void clone ( FecRingRegisters &fecRingRegisters ) {

    fecRingRegisters.index_ = index_ ;
    memcpy (fecRingRegisters.fecRingRegisters_, fecRingRegisters_, (FECCR0+1)*sizeof(tscType16)) ;
    memcpy (fecRingRegisters.ccuRegisters_, ccuRegisters_, (CCUCRE+1)*sizeof(tscType32)) ;
    memcpy (fecRingRegisters.i2cRegisters_, i2cRegisters_, (CCUCRE+1)*sizeof(tscType8)) ;
    memcpy (fecRingRegisters.memoryRegisters_, memoryRegisters_, (MEMORYCRA+1)*sizeof(tscType8)) ;
    memcpy (fecRingRegisters.piaRegisters_, piaRegisters_, (PIAGCR+1)*sizeof(tscType8)) ;
  }

  /** clone the current registers 
   */
  FecRingRegisters *clone ( ) {
    
    FecRingRegisters *fecRingRegisters = new FecRingRegisters ( index_ ) ;
    clone(*fecRingRegisters) ;

    return fecRingRegisters ;
  }
  

  /** Clone the values of the different arrays inside the the class
   * to the one passed by arguments
   * \param fecRingRegisters - FEC registers (output)
   * \param ccuRegisters - CCU registers (output)
   * \param i2cRegisters - I2C regsiters (output)
   * \param memoryRegisters - Memory resgisters (output)
   * \param piaRegisters - PIA resgisters (output)
   * \return hardware index
   */
  keyType clone ( tscType16 *fecRingRegisters, tscType32 *ccuRegisters, tscType8 *i2cRegisters, tscType8 *memoryRegisters, tscType8 *piaRegisters ) {

    memcpy (fecRingRegisters, fecRingRegisters_, (FECCR0+1)*sizeof(tscType16)) ;
    memcpy (ccuRegisters, ccuRegisters_, (CCUCRE+1)*sizeof(tscType32)) ;
    memcpy (i2cRegisters, i2cRegisters_, (CCUCRE+1)*sizeof(tscType8)) ;
    memcpy (memoryRegisters, memoryRegisters_, (MEMORYCRA+1)*sizeof(tscType8)) ;
    memcpy (piaRegisters, piaRegisters_, (PIAGCR+1)*sizeof(tscType8)) ;

    return (index_) ;
  }

  // --------------------------------------------------------------------------------------------------------------
  // Static method for error decoding
  // --------------------------------------------------------------------------------------------------------------

  /** \brief Return a string with the frame
   * \return string with frame
   */
  static std::string decodeFrame ( tscType8 *frame ) {

    int realSize = frame[2] & FEC_LENGTH_2BYTES ? ((frame[2] & 0x7F) *256 + frame[3] + 1) : frame[2] ;

    if (realSize != 0) {

      std::string fecerrstr="frame { ";  
      char hexwrd[8];

      int i = 0 ;
      for ( ; i < (realSize+3) ; i ++ ) {
	std::sprintf(hexwrd, "0x%X ", frame[i]);
	fecerrstr += hexwrd ;
      }
      if (frame[i] != 0) {
	sprintf(hexwrd, "0x%X", frame[i]);
	fecerrstr += std::string("(Ack ") + hexwrd + std::string(") ") ;
      }
      fecerrstr += "}" ;
      
      return (fecerrstr) ;
    }

    return ("Frame empty") ;
  }

  /** \brief Return a string with the meaning of the status register 0
   * \return string with the meaning of the SR0
   */
  static std::string decodeFECSR0( tscType16 fecSR0 ) {

    std::string  fecerrstr="";
    
    char hexwrd[8];
    fecerrstr = "FecSR0 ";
    sprintf(hexwrd, "0x%X", fecSR0);
    fecerrstr += hexwrd ;
    fecerrstr += "\n" ;

    if (fecSR0 & FEC_SR0_TRARUN)
      fecerrstr+= "  FIFO transmit running\n" ;
    if (fecSR0 & FEC_SR0_RECRUN)
      fecerrstr+= "  FIFO receive running\n" ;
    if (fecSR0 & FEC_SR0_RECFULL)
      fecerrstr+= "  FIFO receive full\n" ;
    if (fecSR0 & FEC_SR0_RECEMPTY)
      fecerrstr+= "  FIFO receive empty\n" ;
    else 
      fecerrstr+= "  FIFO receive NOT empty\n" ;
    if (fecSR0 & FEC_SR0_RETFULL)
      fecerrstr+= "  FIFO return full\n" ;
    if (fecSR0 & FEC_SR0_RETEMPTY)
      fecerrstr+= "  FIFO return empty\n" ;
    else
      fecerrstr+= "  FIFO return NOT empty\n" ;
    if (fecSR0 & FEC_SR0_TRAFULL)
      fecerrstr+= "  FIFO transmit full\n" ;
    if (fecSR0 & FEC_SR0_TRAEMPTY)
      fecerrstr+= "  FIFO transmit empty\n" ;
    else
      fecerrstr+= "  FIFO transmit NOT empty\n" ;
    if (fecSR0 & FEC_SR0_LINKINITIALIZED)
      fecerrstr+= "  Link initialise\n" ;
    else
      fecerrstr+= "  Link NOT initialise\n" ;
    if (fecSR0 & FEC_SR0_PENDINGIRQ) 
      fecerrstr+= "  Pending irg\n" ;
    if (fecSR0 & FEC_SR0_DATATOFEC)
      fecerrstr+= "  Data to FEC\n" ;
    if (fecSR0 & FEC_SR0_TTCRXREADY)
      fecerrstr+= "  TTCRx ok" ;
    else
      fecerrstr+= "  TTCRx NOT ok" ;
    
    return fecerrstr;
  }
  
  
  /** \brief Return a string with the meaning of the status register 1
   * \return string with the meaning of the SR1
   */
  static std::string decodeFECSR1( tscType16 fecSR1 ) {
    
    std::string  fecerrstr="";
    char hexwrd[8];
    fecerrstr="FecSR1 ";
    sprintf(hexwrd,"0x%X",fecSR1);
    fecerrstr+=hexwrd;

    if (FEC_SR1_ILLDATA & fecSR1) { fecerrstr+= "\n  Illegal data received";}
    if (FEC_SR1_ILLSEQ & fecSR1) { fecerrstr+= "\n  Illegal sequence received" ;}
    if (FEC_SR1_CRCERROR & fecSR1){ fecerrstr+= "\n  CRC error"; }
    if (FEC_SR1_DATACOPIED & fecSR1) { fecerrstr+= "\n  Data copied" ;}
    else {fecerrstr+= "\n  Data NOT copied" ;}
    if (FEC_SR1_ADDRSEEN & fecSR1) { fecerrstr+= "\n  Address seen" ;}
    else { fecerrstr+= "\n  Address NOT seen" ;}
    if (FEC_SR1_ERROR & fecSR1) { fecerrstr+= "\n  CCU Error" ;}
    if (FEC_SR1_TIMEOUT & fecSR1) { fecerrstr+= "\n  TimeOut" ;}
    if (FEC_SR1_CLOCKERROR  & fecSR1) { fecerrstr+= "\n  clock sync err";}

    return fecerrstr;
  }

  /** \brief Return a string with the meaning of the status register 0
   * \return string with the meaning of the SR0
   */
  static std::string decodeFECCR0( tscType16 fecCR0 ) {

    std::string  fecerrstr="";
    
    char hexwrd[8];
    fecerrstr = "FecCR0 ";
    sprintf(hexwrd, "0x%X", fecCR0);
    fecerrstr += hexwrd;
    fecerrstr += "" ;

    if (fecCR0 & FEC_CR0_ENABLEFEC)
      fecerrstr+= "\n  FEC enable" ;
    else
      fecerrstr+= "\n  FEC not enable" ;
    if (fecCR0 & FEC_CR0_SEND)
      fecerrstr+= "\n  Send frame: error, this bit must be set to 0 when the frame has been sent" ;
    if (fecCR0 & FEC_CR0_XTALCLOCK)
      fecerrstr+= "\n  Internal clock used" ;
    if (fecCR0 & FEC_CR0_SELSEROUT)
      fecerrstr+= "\n  FEC output B used" ;
    else 
      fecerrstr+= "\n  FEC output A used" ;
    if (fecCR0 & FEC_CR0_SELSERIN)
      fecerrstr+= "\n  FEC input B used" ;
    else
      fecerrstr+= "\n  FEC input A used" ;
    if (fecCR0 & FEC_CR0_RESETTCRX)
      fecerrstr+= "\n  FEC reset TTCRx: error this bit must be set to reset to TTCRx" ;
    if (fecCR0 & FEC_CR0_POLARITY)
      fecerrstr+= "\n  FEC clock inverted" ;
    if (fecCR0 & FEC_CR0_DISABLERECEIVE)
      fecerrstr+= "\n  FIFO receive disable" ;
    if (fecCR0 & FEC_CR0_RESETFSMFEC)
      fecerrstr+= "\n  ????????: error this bit must be always 0" ;
    if (fecCR0 & FEC_CR0_RESETRINGB)
      fecerrstr+= "\n  Reset ring B: error this bit must be always 0" ;
    if (fecCR0 & FEC_CR0_RESETOUT)
      fecerrstr+= "\n  Reset ring: error this bit must be always 0" ;
    
    return fecerrstr;
  }


  /**\brief Return a string with the meaning of the status register A of the CCU
   * \return string with the meaning of the CCU SRA
   */
  static std::string decodeCCUSRA( tscType8 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUSRA: node controller status ";
    sprintf(hexwrd,"0x%X",ccureg);
    ccuerrstr += hexwrd;

    if( CCU_SRA_CRC_ERROR   & ccureg ) {ccuerrstr+= "\n  CRC error in packet"; }
    if( CCU_SRA_IN_ERROR    & ccureg ) {ccuerrstr+= "\n  internal node error";} 
    if( CCU_SRA_ALSET       & ccureg ) {ccuerrstr+= "\n  alarm input active"; }
    if( CCU_SRA_PAR_ERROR   & ccureg ) {ccuerrstr+= "\n  parity error in CCU internal register"; }
    if( CCU_SRA_PARCH_ERROR & ccureg ) {ccuerrstr+= "\n  parity error in any of the channels"; }
    if( CCU_SRA_ISEQ_ERROR  & ccureg ) {ccuerrstr+= "\n  illegal sequence of tokens detectd"; }
    if( CCU_SRA_ICMD_ERROR  & ccureg ) {ccuerrstr+= "\n  Invlaid command detected"; }
    if( CCU_SRA_GEN_ERROR   & ccureg ) {ccuerrstr+= "\n  error detected in one of the channels"; }

    return ccuerrstr;
  }

  /**\brief Return a string with the meaning of the status register B of the CCU
   * \return string with the meaning of the CCU SRB
   */
  static std::string decodeCCUSRB( tscType8 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUSRB: Last correctly received transaction number (SRB) = ";
    sprintf(hexwrd,"0x%X",ccureg);
    ccuerrstr += hexwrd;
    
    return ccuerrstr;
  }

  /**\brief Return a string with the meaning of the status register C of the CCU
   * \return string with the meaning of the CCU SRC
   */
  static std::string decodeCCUSRC( tscType8 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUSRC: redundancy control:  ";
    sprintf(hexwrd,"0x%X",ccureg);
    ccuerrstr += hexwrd;
    ccuerrstr += "\n" ;

    if( CCU_SRC_INPUTPORT & ccureg ) {ccuerrstr+= "  Input port B\n"; }
    else {ccuerrstr+= "  Input port A\n"; }
    if( CCU_SRC_OUTPUTPORT & ccureg ) {ccuerrstr+= "  Output port B"; }
    else {ccuerrstr+= "  Output port A"; }

    return ccuerrstr;
  }

  /**\brief Return a string with the meaning of the status register D of the CCU
   * \return string with the meaning of the CCU SRD
   */
  static std::string decodeCCUSRD( tscType8 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUSRD: Source field for the last ring message addressed to this CCU: ";
    sprintf(hexwrd,"0x%X",ccureg);
    ccuerrstr += hexwrd;
    
    return ccuerrstr;
  }
  
  /**\brief Return a string with the meaning of the status register E of the CCU
   * \return string with the meaning of the CCU SRE
   */
  static std::string decodeCCUSRE( tscType32 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUSRE: busy channels: " ;
    sprintf(hexwrd,"0x%X",(unsigned int)ccureg);
    ccuerrstr += hexwrd;
    ccuerrstr += "\n" ;

    ushort channelBusy[18] = {0} ;
    for (ushort i = 0 ; i < 18 ; i ++)
      if (ccureg & (0x1 << i)) channelBusy[i] = 1 ;

    ccuerrstr += "  i2c busy channel: " ;
    for (ushort i = 0 ; i < 16 ; i ++) 
      if (channelBusy[i]) {
	sprintf(hexwrd,"%d",(unsigned int)i+1);
	ccuerrstr += hexwrd ;
      }

    if (channelBusy[16] == 1) ccuerrstr += "  trigger channel busy" ;
    if (channelBusy[17] == 1) ccuerrstr += "\n  memory channel busy" ;
    
    return ccuerrstr;
  }
  
  /**\brief Return a string with the meaning of the status register F of the CCU
   * \return string with the meaning of the CCU SRF
   */
  static std::string decodeCCUSRF( tscType16 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUSRF: parity error counter " ;
    sprintf(hexwrd,"%d",ccureg);
    ccuerrstr += hexwrd;
    
    return ccuerrstr;
  }
  
  /**\brief Return a string with the meaning of the control register A of the CCU
   * \return string with the meaning of the CCU CRA
   */
  static std::string decodeCCUCRA( tscType8 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUCRA: node controller configuration: ";
    sprintf(hexwrd,"0x%X",ccureg);
    ccuerrstr += hexwrd;
    
    if( CCU_CRA_EXTRES & ccureg ) {ccuerrstr+= "\n  Generates external reset"; }
    if( CCU_CRA_CLRE   & ccureg ) {ccuerrstr+= "\n  Clear error";} 
    if( CCU_CRA_RES    & ccureg ) {ccuerrstr+= "\n  All channels reset"; }
    
    return ccuerrstr;
  }
  
  /**\brief Return a string with the meaning of the control register V of the CCU
   * \return string with the meaning of the CCU CRV
   */
  static std::string decodeCCUCRB( tscType8 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUCRB: special alarms: ";
    sprintf(hexwrd,"0x%X",ccureg);
    ccuerrstr += hexwrd;
    ccuerrstr += "\n" ;
    
    if( CCU_CRB_ENAL1 & ccureg ) {ccuerrstr+= "  Enable alarm 1 interrupt\n"; }
    if( CCU_CRB_ENAL2 & ccureg ) {ccuerrstr+= "  Enable alarm 2 interrupt\n"; }
    if( CCU_CRB_ENAL3 & ccureg ) {ccuerrstr+= "  Enable alarm 3 interrupt\n"; }
    if( CCU_CRB_ENAL4 & ccureg ) {ccuerrstr+= "  Enable alarm 4 interrupt\n"; }
    switch ( ccureg & CCU_CRB_RTRY ) {
    case 0: ccuerrstr+= "  0 Retry count for alarm interrupts" ; break;
    case 1: ccuerrstr+= "  1 Retry count for alarm interrupts" ; break;
    case 2: ccuerrstr+= "  2 Retry count for alarm interrupts" ; break;
    case 4: ccuerrstr+= "  4 Retry count for alarm interrupts" ; break;
    default: ccuerrstr+= "  Invalid retry count for alarm interrupts" ; break;
    }
    
    return ccuerrstr;
  }
  
  /**\brief Return a string with the meaning of the status register C of the CCU
   * \return string with the meaning of the CCU SRC
   */
  static std::string decodeCCUCRC( tscType8 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUSRC: redundancy control:  ";
    sprintf(hexwrd,"0x%X",ccureg);
    ccuerrstr += hexwrd;
    ccuerrstr += "\n" ;

    if( CCU_CRC_ALTIN & ccureg ) {ccuerrstr+= "  Input port B\n"; }
    else {ccuerrstr+= "  Input port A\n"; }
    if( CCU_CRC_SSP   & ccureg ) {ccuerrstr+= "  Output port B"; }
    else {ccuerrstr+= "  Output port A"; }

    return ccuerrstr;
  }

  /**\brief Return a string with the meaning of the control register D of the CCU
   * \return string with the meaning of the CCU CRD
   */
  static std::string decodeCCUCRD( tscType16 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUCRD: broadcast class " ;
    sprintf(hexwrd,"0x%X",(ccureg & CCU_CRD_BCLS));
    ccuerrstr += hexwrd;
    
    return ccuerrstr;
  }

  /**\brief Return a string with the meaning of the control register E of the CCU
   * \return string with the meaning of the CCU CRE
   */
  static std::string decodeCCUCRE( tscType32 ccureg ) {
    
    std::string ccuerrstr="";
    
    char hexwrd[8];
    ccuerrstr="CCUCRE: channel enabled: " ;
    sprintf(hexwrd,"0x%X",(unsigned int)ccureg);
    ccuerrstr += hexwrd;
    ccuerrstr += "\n" ;

    ushort channelEnable[23] = {0} ;
    bool i2cDisplay = false, piaDisplay = false ;
    for (ushort i = 0 ; i < 23 ; i ++) {
      channelEnable[i] = ccureg & (0x1 << i) ;
      if (channelEnable[i]) {
	if (i < 16) i2cDisplay = true ;
	else
	  if (i < 20) piaDisplay = true ;
      }
    }

    ccuerrstr += "  i2c enabled: " ;
    if (! i2cDisplay) ccuerrstr += "None" ;
    else {
      for (ushort i = 0 ; i < 16 ; i ++) {
	if (channelEnable[i]) {
	  sprintf(hexwrd,"%d ",(i+1));
	  ccuerrstr += hexwrd ;
	}
      }
    }
    ccuerrstr += "\n" ;
      
    ccuerrstr += "  PIA enabled: " ;
    if (! piaDisplay) ccuerrstr += "None" ;
    else {
      for (ushort i = 16; i < 20 ; i ++) 
	if (channelEnable[i]) {
	  sprintf(hexwrd,"%d",(i+1));
	  ccuerrstr += hexwrd ;
	}
    }
    ccuerrstr += "\n" ;

    if (channelEnable[20]) ccuerrstr += "  Memory channel enable\n" ;
    else ccuerrstr += "  Memory channel not enabled\n" ;
    if (channelEnable[21]) ccuerrstr += "  Trigger channel enable\n" ;
    else ccuerrstr += "  Trigger channel not enabled\n" ;
    if (channelEnable[22]) ccuerrstr += "  JTAG channel enable" ;
    else ccuerrstr += "  JTAG channel not enabled" ;
    
    return ccuerrstr;
  }

  /** \brief Return a string with the meaning of the status register A of an i2c channel
   * \return string with the meaning of the i2c channel SRA
   */
  static std::string decodeI2CSRA( tscType8 i2cSRA ) {
    
    std::string  errstr="";
    char hexwrd[8];
    errstr="I2C channel SRA: " ;
    sprintf(hexwrd,"0x%X",i2cSRA);
    errstr+=hexwrd;

    if (I2C_SRA_SUCC & i2cSRA) { errstr+= "\n  Last transaction successfully executued" ;}
    if (I2C_SRA_I2CLOW & i2cSRA) { errstr+= "\n  I2C SDA line is pulled low" ;}
    if (I2C_SRA_INVCOM & i2cSRA){ errstr+= "\n  Invalid command sent to the i2c channel"; }
    if (I2C_SRA_NOACK & i2cSRA) { errstr+= "\n  I2C transaction not acknowledged by the I2C slave" ;}
    if (I2C_SRA_GE & i2cSRA) { errstr+= "\n  An errors occurs on the i2c channel" ;}

    return errstr;
  }

  /** \brief Return a string with the meaning of the control register A of an i2c channel
   * \return string with the meaning of the i2c channel CRA
   */
  static std::string decodeI2CCRA( tscType8 i2cCRA ) {
    
    std::string  errstr="";
    char hexwrd[8];
    errstr="I2C channel CRA: " ;
    sprintf(hexwrd,"0x%X",i2cCRA);
    errstr+=hexwrd;

    int speed = i2cCRA & I2C_CRA_SPEED ;
    switch (speed) {
    case 0: errstr+= "\n  Speed = 100 Khz" ; break ;
    case 1: errstr+= "\n  Speed = 200 Khz" ; break ;
    case 2: errstr+= "\n  Speed = 400 Khz" ; break ;
    case 4: errstr+= "\n  Speed = 1 Mhz" ; break ;
    }
    if (i2cCRA & I2C_CRA_EBRDCST) { errstr+= "\n  Enable broadcast operation" ;}
    if (i2cCRA & I2C_CRA_FACKW) { errstr+= "\n  Force acknowledge for write or RMW operation" ;}

    return errstr;
  }

  /** Display all the registers from FecRingRegisters methods
   * \param fecRingRegister object
   */
  static void displayAllRegisters ( FecRingRegisters &fecRingRegisters ) {

    std::cerr << "Display all registres for FEC " << std::dec 
	      << (int)getFecKey(fecRingRegisters.getIndex()) << " ring " 
	      << (int)getRingKey(fecRingRegisters.getIndex())
	      << std::hex << " CCU 0x" << (int)getCcuKey(fecRingRegisters.getIndex()) 
	      << " channel 0x" << (int)getChannelKey(fecRingRegisters.getIndex()) 
	      << " address 0x" << (int)getAddressKey(fecRingRegisters.getIndex()) << std::endl ;

    // FEC SR0
    tscType32 value = fecRingRegisters.getFecSR0() ;
    if (value != 0xFFFF) 
      std::cerr << FecRingRegisters::decodeFECSR0 ( value ) << std::endl ;
    else
      std::cerr << "SR0: Unable to read it" << std::endl ;

    // FEC SR1
    value = fecRingRegisters.getFecSR1() ;
    if (value != 0xFFFF) 
      std::cerr << FecRingRegisters::decodeFECSR1 ( value ) << std::endl ;
    else
      std::cerr << "SR1: Unable to read it" << std::endl ;

    // FEC CR0
    value = fecRingRegisters.getFecCR0() ;
    if (value != 0xFFFF) 
      std::cerr << FecRingRegisters::decodeFECCR0 ( value ) << std::endl ;
    else
      std::cerr << "CR0: Unable to read it" << std::endl ;

    // CCU SRA
    value = fecRingRegisters.getCcuSRA() ;
    if (value != 0xFF) 
      std::cerr << FecRingRegisters::decodeCCUSRA ( value ) << std::endl ;
    else
      std::cerr << "SRA: Unable to read it" << std::endl ;

    // CCU SRB
    value = fecRingRegisters.getCcuSRB() ;
    if (value != 0xFF) 
      std::cerr << FecRingRegisters::decodeCCUSRB ( value ) << std::endl ;
    else
      std::cerr << "SRB: Unable to read it" << std::endl ;

    // CCU SRC
    value = fecRingRegisters.getCcuSRC() ;
    if (value != 0xFF) 
      std::cerr << FecRingRegisters::decodeCCUSRC ( value ) << std::endl ;
    else
      std::cerr << "SRC: Unable to read it" << std::endl ;

    // CCU SRD
    value = fecRingRegisters.getCcuSRD() ;
    if (value != 0xFF) 
      std::cerr << FecRingRegisters::decodeCCUSRD ( value ) << std::endl ;
    else
      std::cerr << "SRD: Unable to read it" << std::endl ;

    // CCU SRE
    value = fecRingRegisters.getCcuSRE() ;
    if (value != 0xFFFFFFFF) 
      std::cerr << FecRingRegisters::decodeCCUSRE ( value ) << std::endl ;
    else
      std::cerr << "SRE: Unable to read it" << std::endl ;

    // CCU SRF
    value = fecRingRegisters.getCcuSRF() ;
    if (value != 0xFFFF) 
      std::cerr << FecRingRegisters::decodeCCUSRF ( value ) << std::endl ;
    else
      std::cerr << "SRF: Unable to read it" << std::endl ;

    // CCU CRA
    value = fecRingRegisters.getCcuCRA() ;
    if (value != 0xFF) 
      std::cerr << FecRingRegisters::decodeCCUCRA ( value ) << std::endl ;
    else
      std::cerr << "CRA: Unable to read it" << std::endl ;

    // CCU CRB
    value = fecRingRegisters.getCcuCRB() ;
    if (value != 0xFF) 
      std::cerr << FecRingRegisters::decodeCCUCRB ( value ) << std::endl ;
    else
      std::cerr << "CRB: Unable to read it" << std::endl ;

    // CCU CRC
    value = fecRingRegisters.getCcuCRC() ;
    if (value != 0xFF) 
      std::cerr << FecRingRegisters::decodeCCUCRC ( value ) << std::endl ;
    else
      std::cerr << "CRC: Unable to read it" << std::endl ;

    // CCU CRD
    value = fecRingRegisters.getCcuCRD() ;
    if (value != 0xFF) 
      std::cerr << FecRingRegisters::decodeCCUCRD ( value ) << std::endl ;
    else
      std::cerr << "CRD: Unable to read it" << std::endl ;

    // CCU CRE
    value = fecRingRegisters.getCcuCRE() ;
    if (value != 0xFFFFFFFF) 
      std::cerr << FecRingRegisters::decodeCCUCRE ( value ) << std::endl ;
    else
      std::cerr << "CRE: Unable to read it" << std::endl ;

    // If the channel is an i2c channel
    if (isi2cChannelCcu25(fecRingRegisters.getIndex())) {

      // i2c SRA
      value = fecRingRegisters.geti2cChannelSRA() ;
      if (value != 0xFF)
	std::cerr << FecRingRegisters::decodeI2CSRA ( value ) << std::endl ;
      else
	std::cerr << "I2C(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "): unable to read the SRA" << std::dec << std::endl ;

      // i2c SRB
      value = fecRingRegisters.geti2cChannelSRB() ;
      if (value != 0xFF)
	std::cerr << "I2C(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "), last correctly transaction number executed (SRB) = 0x" 
		  << value << std::dec << std::endl ;
      else
	std::cerr << "I2C(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "): unable to read the SRB" << std::dec << std::endl ;

      // i2c SRC
      value = fecRingRegisters.geti2cChannelSRC() ;
      if (value != 0xFF)
	std::cerr << "I2C(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "), last incorrectly transaction number executed (SRC) = 0x" << value 
		  << std::dec << std::endl ;
      else
	std::cerr << "I2C(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "): unable to read the SRC" << std::dec << std::endl ;

      // i2c SRD
      value = fecRingRegisters.geti2cChannelSRD() ;
      if (value != 0xFF)
	std::cerr << "I2C(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "), last command sent to this i2c channel (SRD) = 0x" << value 
		  << std::dec << std::endl ;
      else
	std::cerr << "I2C(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "): unable to read the SRD" << std::dec << std::endl ;

      // i2c CRA
      value = fecRingRegisters.geti2cChannelCRA() ;
      if (value != 0xFF)
	std::cerr << FecRingRegisters::decodeI2CCRA ( value ) << std::endl ;
      else
	std::cerr << "I2C(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "): unable to read the CRA" << std::dec << std::endl ;
    }
    else if (isPiaChannelCcu25(fecRingRegisters.getIndex())) {

      // PIA status
      value = fecRingRegisters.getPiaChannelSR() ;
      if (value != 0xFF)
	std::cerr << "PIA(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << ") SR = 0x" << value << std::dec << std::endl ;
      else
	std::cerr << "PIA(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "): unable to read the SR" << std::dec << std::endl ;

      // PIA GCR
      value = fecRingRegisters.getPiaChannelGCR() ;
      if (value != 0xFF)
	std::cerr << "PIA(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << ") GCR = 0x" << value << std::dec << std::endl ;
      else
	std::cerr << "PIA(0x" << std::hex << (int)getChannelKey(fecRingRegisters.getIndex()) 
		  << "): unable to read the GCR" << std::dec << std::endl ;
    }
  }

};

#endif
