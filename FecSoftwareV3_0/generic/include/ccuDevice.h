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
#ifndef CCUDEVICE_H
#define CCUDEVICE_H

#include "keyType.h"

#include "cmdDescription.h"

#include "errorDescription.h"
#include "FecExceptionHandler.h"


/** typedef for storing usage type of the ccu
 * ccuNotUsed: CCU not in the ring nor in ring A nor in ring B, the CCU is power on so it can be addressed ???????????????????????
 * ccuUsed: CCU present in the ring, so it can be addressed whatever the configuration ring is
 * ccuSkipped: CCU not present on the ring, bypassed by input/output configuration, so the CCU can be or not power on. So the CCU cannot be addressed even for reconfiguration.
 */
typedef enum {ccuNotUsed,ccuUsed,ccuSkipped} ccuUsageType; 

/**
 * \class ccuDevice
 * This class reflect the software point of view of the configuration of a CCU. No hardware access are done in this class.
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class give the current value for a CCU connected to a ring
 * \warning This class do not access the hardware. Is created in order to keep
 * in memory the current state of a CCU
 * \deprecated This class has been replaced by CCUDescription in order for database access purpose
 */
class ccuDevice {

 private: 

  /** Address of the CCU
   */
  tscType16 ccuAddress_ ;

  /** Output channel
   */
  tscType16 ccuOutput_ ;
  
  /** Input channel
   */
  tscType16 ccuInput_ ;

  /** Control register A of an I2C channel
   * Possible value:
   * <ul>
   * <li>0 not initialise
   * <li>0xFFFF initialise but the CRA is not set
   * <li>!= initialise and the CRA is set
   * </ul>
   */
  tscType16 i2cCRAValue[MAXI2CCHANNELS] ;

  /** General Control Register of a PIA channel
   * Possible value:
   * <ul>
   * <li>0 not initialise
   * <li>0xFFFF initialise but the GCR is not set
   * <li>!= initialise and the GCR is set
   * </ul>
   */
  tscType16 piaGCRValue[MAXCCUPIACHANNELS] ;

  /** Force acknoledge bit for each I2C channel
   */
  bool i2cBitForceAck[MAXI2CCHANNELS] ;

  /** I2C channel enable or not
   */
  bool channelI2CEnable[MAXI2CCHANNELS] ;

  /** PIA channel enable or not
   */
  bool channelPiaEnable[MAXCCUPIACHANNELS] ;

  /** Memory channel enable or not
   */
  bool channelMemoryEnable ;

  /** JTAG channel enable or not
   */
  bool channelJTAGEnable   ;

  /** Trigger channel enable or not
   */
  bool channelTriggerEnable; 

  /** Is a CCU 25 ?
   */
  bool isACcu25 ;

  /** Is CCU alarms enabled ?
   */
  bool ccuAlarmsEnable[4] ;

  /** Is this Ccu used ? (usage state)
   * Skipped ,ean
   */
  ccuUsageType usage_ ;

 public:

  /** Set all the channel to not initialise and the input/output
   * value to channel A
   * \param address - CCU address
   * \param output - valid ouput of the CCU (reconfiguration)
   * \param input - valid input of the CCU (reconfiguration)
   * \param ccu25 - true if the CCU is a CCU 25
   * \brief Constructor that create a CCU 
   */
  ccuDevice ( tscType16 address,
              tscType16 output,
              tscType16 input, 
              bool ccu25 ) {

    isACcu25 = ccu25 ;

    ccuAddress_  = address ;
    ccuOutput_   = output  ;
    ccuInput_    = input   ;

    for (tscType16 channel = 0 ; channel < MAXI2CCHANNELS ; channel ++) {

      i2cBitForceAck[channel] = false ;
      i2cCRAValue[channel] = 0; 
      channelI2CEnable[channel] = false ;
    }


    if (isACcu25) {
      for (tscType16 channel = 0 ; channel < NBCCU25PIACHANNELS ; channel ++)
        channelPiaEnable[channel] = false ;
    } 
    else {
      for (tscType16 channel = 0 ; channel < NBOLDCCUPIACHANNELS ; channel ++)
        channelPiaEnable[channel] = false ;
    }

    channelMemoryEnable  = false ;
    channelJTAGEnable    = false ;
    channelTriggerEnable = false ;

    for (tscType16 i = 0 ; i < 4 ; i ++) ccuAlarmsEnable[i] = false; 

    //WB:
    usage_ = ccuUsed ; 

  }

  /** \brief Return the current ouput 0->A 1->B
   * \return current output
   */
  int getOutput ( ) {
    return (ccuOutput_) ;
  }

  /** \brief Return the current input 0->A 1->B
   * \return current input
   */
  int getInput ( ) {
    return (ccuInput_) ;
  }

  /** Return the address of the ccu
   * \return CCU address
   */
  tscType16 getAddress ( ) {

    return (ccuAddress_) ;
  }

  /** \brief Set the address of the ccu
   * \param address - start from 0x1 to 0x7f
   */
  void setAddress ( tscType16 address ) {

     ccuAddress_ = address ;
  }

  /** \brief Return the bit force ack for a channel
   * \param channel - CCU channel number
   * \return boolean to know if the force ack is enable or not
   * \warning this function work for old CCU and CCU 25
   */
  bool getBitForceAck ( tscType16 channel ) {

    if (isACcu25)
      return (i2cBitForceAck[channel-0x10]) ;
    else
      return (i2cBitForceAck[channel-0x1]) ;
  }

  /** \brief return if the CCU is a CCU25 or no
   */
  bool isCcu25 ( ) {

    return (isACcu25) ;
  }

  /** \brief in case of reset, put all values in the correct state
   */
  void reset ( ) {

    ccuOutput_ = ccuInput_ = 0 ;
    for (tscType16 i = 0 ; i < MAXI2CCHANNELS ; i ++) {

      i2cCRAValue[i] = 0 ;
      i2cBitForceAck[i] = false ;
      channelI2CEnable[i] = false ;

    }
    for (tscType16 i = 0 ; i < MAXCCUPIACHANNELS ; i ++) {

      piaGCRValue[i] = 0 ;
      channelPiaEnable[i] = false ;
    }
    channelJTAGEnable = channelTriggerEnable = false ;
    for (tscType16 i = 0 ; i < 4 ; i ++) ccuAlarmsEnable[i] = false ;
  }     

  /** \brief Enable or disable the Force acknowledge for a channel
   * \param channel - CCU channel number
   * \param enable - enable/disable
   */
  void setBitForceAck ( tscType16 channel, bool enable ) {

    if (isACcu25)
      i2cBitForceAck[channel-0x10] = enable ;
    else
      i2cBitForceAck[channel-0x1] = enable ;
  }

  /** \brief Set the value of the input
   * \param input - input value (1 or 0)
   * \exception FecExceptionHandler
   * <ul>
   * <li>TSCFEC_FECPARAMETERNOTMANAGED: wrong CCU input (ring A/B) value
   * </ul>
   */
  void setInput ( tscType16 input ) throw (FecExceptionHandler) {

#ifdef T3TDLEVELCHECK
    if ( (input == 0) || (input == 1) )
      ccuInput_    = input   ;
    else {
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FECPARAMETERNOTMANAGED, 
					 "Wrong CCU input (ring A/B) value", 
					 ERRORCODE,
					 setCcuKey(ccuAddress_),
					 "value", input) ;
    }
#endif

    ccuInput_ = input   ;
  }
  
  /** \brief Set the value of the Output
   * \param output - output value (1 or 0)
   * \exception FecExceptionHandler
   * <ul>
   * <li>TSCFEC_FECPARAMETERNOTMANAGED: wrong CCU output (ring A/B) value
   * </ul>
   */
  void setOutput ( tscType16 output ) throw (FecExceptionHandler) {
    if ( (output == 0) || (output == 1) )
      ccuOutput_    = output   ;
    else {
      RAISEFECEXCEPTIONHANDLER_INFOSUP ( TSCFEC_FECPARAMETERNOTMANAGED, 
					 "Wrong CCU output (ring A/B) value", 
					 ERRORCODE,
					 setCcuKey(ccuAddress_),
					 "value", output) ;
    }
  }

  /** \brief Store if a CCU alarms is enabled
   * \param enable1 - CCU alarm 1
   * \param enable2 - CCU alarm 2
   * \param enable3 - CCU alarm 3
   * \param enable4 - CCU alarm 4
   */
  void setCcuAlarmsEnable ( bool enable1, bool enable2, bool enable3, bool enable4 ) {

    ccuAlarmsEnable[0] = enable1 ;
    ccuAlarmsEnable[1] = enable2 ;
    ccuAlarmsEnable[2] = enable3 ;
    ccuAlarmsEnable[3] = enable4 ;
  }

  /** \brief Check if a CCU alarm is enable
   * \param ccuAlarm - CCU alarm number
   * \return boolean - true if the CCU alarm is enable
   * \exception FecExceptionHandler
   * <ul>
   * <li>TSCFEC_FECPARAMETERNOTMANAGED: wrong CCU alarm
   * </ul>
   */
  bool isCcuAlarmEnable ( tscType16 ccuAlarm ) throw (FecExceptionHandler) {

    if (ccuAlarm > 3) {
      RAISEFECEXCEPTIONHANDLER_INFOSUP (TSCFEC_FECPARAMETERNOTMANAGED,
					TSCFEC_FECPARAMETERNOTMANAGED_UMSG + ": wrong CCU alarm", 
					FATALERRORCODE,
					setCcuKey(ccuAddress_),
					"CCU alarm", ccuAlarm) ;
    }

    return (ccuAlarmsEnable[ccuAlarm]) ;
  }

  /** \brief Store that a channel is enable or disable
   * \param channel - CCU channel number
   * \param enable - enable or disable
   */
  void setChannelEnable ( tscType16 channel, bool enable ) {

    if (isACcu25) {
      if ( (channel >= 0x10) && (channel <= 0x1F) )
        channelI2CEnable[channel-0x10] = enable ;
      else
        if ( (channel >= 0x30) && (channel <= 0x33) )
          channelPiaEnable[channel-0x30] = enable ;
        else
          if (channel == 0x40)
            channelMemoryEnable = enable ;
          else
            if (channel == 0x50)
              channelTriggerEnable = enable ;
            else
              if (channel == 0x60)
                channelJTAGEnable = enable ;
    }
    else {
      if ( (channel >= 0x01) && (channel <= 0x10) )
        channelI2CEnable[channel-1] = enable ;
      else
        if (channel == 0x12)
          channelPiaEnable[0] = enable ;
        else
          if (channel == 0x11)
            channelMemoryEnable = enable ;
          else
            if (channel == 0x14)
              channelTriggerEnable = enable ;
    }
  }

  /** \brief return true if a channel is enable, false if not
   * \param channel - CCU channel number
   * \return - return true if a channel is enable, false if not
   */
  bool getChannelEnable ( tscType16 channel ) {

    bool value = false ;
    
    if (isACcu25) {
      if ( (channel >= 0x10) && (channel <= 0x1F) )
        value = channelI2CEnable[channel-0x10] ;
      else
        if ( (channel >= 0x30) && (channel <= 0x33) )
          value = channelPiaEnable[channel-0x30] ;
        else
          if (channel == 0x40)
            value = channelMemoryEnable ;
          else
            if (channel == 0x50)
              value = channelTriggerEnable ;
            else
              if (channel == 0x60)
                value = channelJTAGEnable ;
    }
    else {

      if ( (channel >= 0x01) && (channel <= 0x10) )
        value = channelI2CEnable[channel-1] ;
      else
        if (channel == 0x12)
          value = channelPiaEnable[0] ;
        else
          if (channel == 0x11)
            value = channelMemoryEnable ;
          else
            if (channel == 0x14)
              value = channelTriggerEnable ;      
    }

    return (value) ;
  }

  /** \brief Set the control register A of an I2C channel
   * \param channel - CCU channel number 
   * \param value - value set in the CRA
   */
  void seti2cChannelCRA ( tscType16 channel, tscType16 value ) {

    if (isACcu25)
      i2cCRAValue[channel-0x10] = value ;
    else
      i2cCRAValue[channel-0x1] = value ;

    if (isChannelForceAck(value)) setBitForceAck (channel, true) ;
  }
 
  /** Make a logical operation on the CRA store in the array
   * \brief Set the control register A of an I2C channel
   * \param channel - CCU channel number 
   * \param value - value set in the CRA
   * \param op - operation for the read modify write
   */
  void seti2cChannelCRA ( tscType16 channel, tscType16 value, logicalOperationType op ) {

    tscType16 chan = channel - 0x10 ;

    if (! isACcu25)
      chan = channel - 0x1 ;

    switch (op) {
    case CMD_OR:  i2cCRAValue[chan] = i2cCRAValue[chan] | value    ; break ;
    case CMD_XOR: i2cCRAValue[chan] = i2cCRAValue[chan] & (~value) ; break ;
    case CMD_AND: i2cCRAValue[chan] = i2cCRAValue[chan] & value    ; break ;
    case CMD_EQUAL: break ;
    }

    if (isChannelForceAck(value)) setBitForceAck (channel, true) ;
  }

  /** \brief Return the value of the control register A of an I2C channel
   * \param channel - CCU channel number 
   * \return Value of the control register A
   */
  tscType16 geti2cChannelCRA ( tscType16 channel ) {


    if (isACcu25) channel -= 0x10 ;
    else channel -= 0x1 ;

    return (i2cCRAValue[channel-0x10]) ;
  }

  /** \brief Set the value of the General control register of a PIA channel
   * \param channel - PIA channel. The number of PIA channels depends of the CCU type
   * \param value - value set in the GCR
   */
  void setPiaChannelGCR ( tscType16 channel, tscType16 value ) {

    if (isACcu25) channel -= 0x30 ;
    else channel -= 0x12 ;

    piaGCRValue[channel] = value ;
  }
  
  /** Make a logical operation on the GCR store in the array
   * \brief Set the value of the General control register of a PIA channel
   * \param channel - PIA channel. The number of PIA channels depends of the CCU type
   * \param value - value set in the GCR
   * \param op - operation to be applied for the read modify write
   */
  void setPiaChannelGCR ( tscType16 channel, tscType16 value, logicalOperationType op ) {

    if (isACcu25) channel -= 0x30 ;
    else channel -= 0x12 ;

    switch (op) {
    case CMD_OR:  piaGCRValue[channel] = piaGCRValue[channel] | value    ; break ;
    case CMD_XOR: piaGCRValue[channel] = piaGCRValue[channel] & (~value) ; break ;
    case CMD_AND: piaGCRValue[channel] = piaGCRValue[channel] & value    ; break ;
    case CMD_EQUAL: break ;
    }
  }

  /** \brief Return the value of the General control register of the PIA channel
   * \param channel - PIA channel. The number of PIA channels depends of the CCU type
   * \return Value of the general control register
   */
  tscType16 getPiaChannelGCR ( tscType16 channel ) {

    if (isACcu25) channel -= 0x30 ;
    else channel -= 0x12 ;

    return (piaGCRValue[channel-0x30]) ;
  }

  /** \brief Set usage state of this ccu
   * \author W. Bialas
   */
  inline void setUsage(ccuUsageType usage) {
    usage_ = usage ; 
  } 
  
  /** \brief Get usage state of this ccu
   * \author W. Bialas
   */
  inline ccuUsageType getUsage() {
    return usage_ ; 
  } 
};
#endif
