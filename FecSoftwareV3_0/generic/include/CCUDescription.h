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
#ifndef CCUDESCRIPTION_H
#define CCUDESCRIPTION_H
#include <vector>
#include <list>

#include "keyType.h"

#include "cmdDescription.h"
#include "ccuDefinition.h"

#include "FecExceptionHandler.h"

#include "deviceDescription.h"

/**
 * \class CCUDescription
 * This class reflect the software point of view of the configuration of a CCU. No hardware access are done in this class.
 * \author Frederic Drouhin
 * \date April 2002
 * \brief This class give the current value for a CCU connected to a ring
 * \warning This class do not access the hardware. Is created in order to keep
 * in memory the current state of a CCU
 * The attributs fecHardwareId_ are used only to retreive the information from the database
 */
class CCUDescription {

 public:

  /** typedef for storing usage type of the ccu
   * ccuNotUsed: CCU not in the ring nor in ring A nor in ring B, the CCU is power on so it can be addressed ???????????????????????
   * ccuUsed: CCU present in the ring, so it can be addressed whatever the configuration ring is
   * ccuSkipped: CCU not present on the ring, bypassed by input/output configuration, so the CCU can be or not power on. So the CCU cannot be addressed even for reconfiguration.
   */
  typedef enum {ccuNotUsed,ccuUsed,ccuSkipped} ccuUsageType; 

  /** List of name of parameter
   */
  enum{ARRANGEMENT};

  /** Enumeration name for the XML parsing
   */
  static const char *CCUPARAMETERNAMES[ARRANGEMENT+1] ; // = {"arrangement"} ;

  /** Number of parameters to be parsed
   */
  static const unsigned int CCUNUMBEROFPARAMETERS ; // = 7;

  /** Set all the channel to not initialise
   * \param keytype - the key
   * \param arrangement - order in the ring
   * \param ccu25 - true if the CCU is a CCU 25
   * \brief Constructor that create a CCU 
   */
  CCUDescription ( tscType16 crateId, keyType key, int ccuOrder , bool ccu25, bool enabled) ;

  /** Set all the channel to not initialise
   * \param address - CCU address
   * \param ccu25 - true if the CCU is a CCU 25
   * \brief Constructor that create a CCU 
   */
  CCUDescription ( keyType key, bool ccu25 = true ) ;
  /** \brief Create a CCU description from XML buffer
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   * \warning an exception is raised if one the parameter is not correct
   */
  CCUDescription ( parameterDescriptionNameType parameterNames, bool isACcu25 = true ) ;

  /** \brief Clone a CCU description
   * \return the CCUDescription cloned
   */
  CCUDescription *clone ( ) ;

  /** \brief Return the order of the CCU in the ring
   * \return CCU order
   */
  unsigned int getOrder ( ) ;

  /** \brief Return the current enabled parameter 
   * \return enabled
   */
  bool getEnabled ( ) ;

  /** Return the address of the ccu
   * \return CCU address
   */
  keyType getKey ( ) ;

  /** \brief return the crate ID
   * \return the crate ID
   */
  tscType16 getCrateId ( ) ;

  /** \brief set the crate ID
   * \param crateId - slot of the crate to be set
   */
  void setCrateId ( tscType16 crateId ) ;

  /** \brief Set the address of the ccu
   * \param address - start from 0x1 to 0x7f
   */
  void setKey ( keyType key ) ;

  /** \brief Return the bit force ack for a channel
   * \param channel - CCU channel number
   * \return boolean to know if the force ack is enable or not
   * \warning this function work for old CCU and CCU 25
   */
  bool getBitForceAck ( tscType8 channel ) ;

  /** \brief return if the CCU is a CCU25 or no
   */
  bool isCcu25 ( ) ;

  /** \brief in case of reset, put all values in the correct state
   */
  void reset ( ) ;

  /** \brief Enable or disable the Force acknowledge for a channel
   * \param channel - CCU channel number
   * \param enable - enable/disable
   */
  void setBitForceAck ( tscType8 channel, bool enable ) ;

  /** \brief Set the value of Enabled 
   * \param enabled (boolean)
   */
  void setEnabled ( bool enabled ) ;

  /** \brief Store if a CCU alarms is enabled
   * \param enable1 - CCU alarm 1
   * \param enable2 - CCU alarm 2
   * \param enable3 - CCU alarm 3
   * \param enable4 - CCU alarm 4
   */
  void setCcuAlarmsEnable ( bool enable1, bool enable2, bool enable3, bool enable4 ) ;

  /** \brief Check if a CCU alarm is enable
   * \param ccuAlarm - CCU alarm number
   * \return boolean - true if the CCU alarm is enable
   * \exception FecExceptionHandler
   * <ul>
   * <li>TSCFEC_FECPARAMETERNOTMANAGED: wrong CCU alarm
   * </ul>
   */
  bool isCcuAlarmEnable ( tscType16 ccuAlarm ) throw (FecExceptionHandler) ;

  /** \brief Store that a channel is enable or disable
   * \param channel - CCU channel number
   * \param enable - enable or disable
   */
  void setChannelEnable ( tscType8 channel, bool enable ) ;

  /** \brief return true if a channel is enable, false if not
   * \param channel - CCU channel number
   * \return - return true if a channel is enable, false if not
   */
  bool getChannelEnable ( tscType8 channel ) ;

  /** \brief Set the control register A of an I2C channel
   * \param channel - CCU channel number 
   * \param value - value set in the CRA
   */
  void seti2cChannelCRA ( tscType8 channel, tscType8 value ) ;
 
  /** Make a logical operation on the CRA store in the array
   * \brief Set the control register A of an I2C channel
   * \param channel - CCU channel number 
   * \param value - value set in the CRA
   * \param op - operation for the read modify write
   */
  void seti2cChannelCRA ( tscType8 channel, tscType8 value, logicalOperationType op ) ;

  /** \brief Return the value of the control register A of an I2C channel
   * \param channel - CCU channel number 
   * \return Value of the control register A
   */
  tscType8 geti2cChannelCRA ( tscType8 channel ) ;

  /** \brief Set the value of the General control register of a PIA channel
   * \param channel - PIA channel. The number of PIA channels depends of the CCU type
   * \param value - value set in the GCR
   */
  void setPiaChannelGCR ( tscType8 channel, tscType8 value ) ;
  
  /** Make a logical operation on the GCR store in the array
   * \brief Set the value of the General control register of a PIA channel
   * \param channel - PIA channel. The number of PIA channels depends of the CCU type
   * \param value - value set in the GCR
   * \param op - operation to be applied for the read modify write
   */
  void setPiaChannelGCR ( tscType8 channel, tscType8 value, logicalOperationType op ) ;

  /** \brief Return the value of the General control register of the PIA channel
   * \param channel - PIA channel. The number of PIA channels depends of the CCU type
   * \return Value of the general control register
   */
  tscType8 getPiaChannelGCR ( tscType8 channel ) ;

  /** \brief Set usage state of this ccu
   * \author W. Bialas
   */
  inline void setUsage(ccuUsageType usage) ;
  
  /** \brief Get usage state of this ccu
   * \author W. Bialas
   */
  inline ccuUsageType getUsage() ;

  // ***********************************************************************************************
  // FEC hardware ID
  // ***********************************************************************************************

  /** \brief Set the FEC hardware identification number
   */
  void setFecHardwareId ( std::string fecHardwareId, tscType16 crateId ) ;

  /** \brief return the FEC hardware identification number
   */
  std::string getFecHardwareId ( ) ;

  // ***********************************************************************************************
  // Display
  // ***********************************************************************************************

  /** Display the placement of the CCU in the ring
   */
  void displayRedundancy ( ) ;

  // ***********************************************************************************************
  // Static method
  // ***********************************************************************************************

  /** Use to sort a vector of CCUs 
   * \param r1 - CCUDescription
   * \param r2 - CCUDescription
   */
  static bool sortByOrder ( CCUDescription *r1, CCUDescription *r2 ) ;


  /** Display the redundancy of a vector of CCU descriptions in the correct order
   * \param vCcuVector - vector of CCU descriptions
   */
  static void displayRedundancy ( std::vector<CCUDescription *> vCcuVector ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

  /** \brief Delete a list of parameter name but only its content
   * \warning the parameter parameterNames is deleted by this method
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) ;

#ifdef _OLD_CCU_INPUT_OUTPUT_

  /** \brief Set the parameter usage in the CCUDescription
   * \param list of CCUDescription
   * \warning this method include the dummy CCU on ring B so last CCU output A -> FEC input A
   *          and last CCU output B -> Dummy CCU input A -> Dummy CCU output A -> FEC input B
   *          and Before last CCU output B -> Dummy CCU input B -> Dummy CCU output A -> FEC input B
   *          and Dummy CCU output B not connected
   */

  static void setCcuTrackerUsage ( std::list<CCUDescription *> &listCcu ) throw (FecExceptionHandler) ;

  /** \brief Set the parameter usage in the CCUDescription
   * \param list of CCUDescription
   * \warning this method include the dummy CCU on ring A
   */

  static void setCcuUsage ( std::list<CCUDescription *> &listCcu ) throw (FecExceptionHandler) ;

#endif

  void setInputA()  {inputANeeded_  = true  ; };
  void setInputB()  {inputANeeded_  = false ; };
  void setOutputA() {outputANeeded_ = true  ; };
  void setOutputB() {outputANeeded_ = false ; };
  bool getInputA()  {return   inputANeeded_   ; };
  bool getInputB()  {return (!inputANeeded_ ) ; };
  bool getOutputA() {return   outputANeeded_  ; };
  bool getOutputB() {return (!outputANeeded_) ; };

 private: 

  /** VME FEC has identification number inside the hardware.
   */
  char fecHardwareId_[100] ;

  /** Crate slot
   */
  tscType16 crateId_ ;

  /** Address of the CCU
   */
  keyType key_ ;

  /** Position in the ring
   */
  unsigned int ccuOrder_ ;

  /** This is to flag when a CCU is disabled
   *  because of a failure or any other reason
   */
  bool enabled_;

  /** Control register A of an I2C channel
   * Possible value:
   * <ul>
   * <li>0 not initialise
   * <li>0xFFFF initialise but the CRA is not set
   * <li>!= initialise and the CRA is set
   * </ul>
   */
  tscType8 i2cCRAValue_[MAXI2CCHANNELS] ;

  /** General Control Register of a PIA channel
   * Possible value:
   * <ul>
   * <li>0 not initialise
   * <li>0xFFFF initialise but the GCR is not set
   * <li>!= initialise and the GCR is set
   * </ul>
   */
  tscType8 piaGCRValue_[MAXCCUPIACHANNELS] ;

  /** Force acknoledge bit for each I2C channel
   */
  bool i2cBitForceAck_[MAXI2CCHANNELS] ;

  /** I2C channel enable or not
   */
  bool channelI2CEnable_[MAXI2CCHANNELS] ;

  /** PIA channel enable or not
   */
  bool channelPiaEnable_[MAXCCUPIACHANNELS] ;

  /** Memory channel enable or not
   */
  bool channelMemoryEnable_ ;

  /** JTAG channel enable or not
   */
  bool channelJTAGEnable_   ;

  /** Trigger channel enable or not
   */
  bool channelTriggerEnable_; 

  /** Is a CCU 25 ?
   */
  bool isACcu25_ ;

  /** Is CCU alarms enabled ?
   */
  bool ccuAlarmsEnable_[4] ;

  /** Is this Ccu used ? (usage state)
   */
  ccuUsageType usage_ ;

  /** Input/Output to be used for the redundancy
   *  Temporary-stored parameter
   */
  bool inputANeeded_;
  bool outputANeeded_;
};

#endif
