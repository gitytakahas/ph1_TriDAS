#ifndef CCSTrigger_hh
#define CCSTrigger_hh
 
/** \mainpage CCSTrigger for Local Trigger Supervisor
 *
 * The CCSTrigger is a class for configuration/execution of
 * CCS-FEC trigger FPGA
 *
 */
 
/** \file
 * This file contains the headers for CCSTrigger class
 */

#include <string>

#include "hal/VME64xDevice.hh"

#include "tscTypes.h"             // for SSID enumeration
#include "FecExceptionHandler.h"

/**
 */
class CCSTrigger {
  
private:
  
  enum SubSystemsEnum { Reserved0, Tracker, Ecal, Preshower, Pixels, Rpc, Totem, Reserved2 }; 
  
public: 
  /** \brief Default constructor
   */
  CCSTrigger(HAL::VME64xDevice * trgBoard, std::string subSystem, tscType8 fecSlot = 0xFF )  throw ( FecExceptionHandler ) ;

  /** \brief desctructor
   */
  ~CCSTrigger ( ) ;

  /** \brief return the FEC slot
   */
  inline tscType8 getFecSlot ( ) { return fecSlot_ ; } 

  /** \brief set the config 0
   */
  void setCCSTriggerConfig0 ( uint32_t value ) throw ( FecExceptionHandler ) ;

  /** \brief get the config 0
   */
  uint32_t getCCSTriggerConfig0 ( ) throw ( FecExceptionHandler ) ;

  /** \brief get the config 0
   */
  uint32_t getCCSTriggerStatus0 ( ) throw ( FecExceptionHandler ) ;

  /** \brief Set the SSID into the CCSTrigger object
   */
  void setSSID ( std::string subSystem, uint32_t originalValue ) throw ( FecExceptionHandler ) ;

  /** \brief Set the SSID into the CCSTrigger object
   */
  void setSSID ( std::string subSystem ) throw ( FecExceptionHandler ) ;

  /** \brief retreive the SSID from the FEC board
   */
  std::string getSSID ( ) throw ( FecExceptionHandler ) ;

  /** \brief reset the TTCRx
   */
  void setResetTTCRx ( ) throw ( FecExceptionHandler ) ;

  /** \brief reset the QPLL
   */
  void setResetQPLL ( ) throw ( FecExceptionHandler ) ;

  /** \brief set the QPLL auto restart
   */
  void setQPLLAutoRestart ( ) throw ( FecExceptionHandler ) ;

  /** QPLL locked
   */
  bool getStatusQPLLLocked ( ) throw (FecExceptionHandler) ;

  /** TTCRx Ready
   */
  bool getStatusTTCRxReady ( ) throw (FecExceptionHandler)  ;

  /** Error on QPLL
   */
  bool getStatusQPLLError ( ) throw ( FecExceptionHandler ) ;

  void clear() throw ( FecExceptionHandler ) ;

  bool SetFrequency(int frequency)  throw ( FecExceptionHandler ) ;
  int GetFrequency()  throw ( FecExceptionHandler ) ;

  uint32_t getID()  throw ( FecExceptionHandler ) ;

  bool SetInput(std::string block, std::string input) throw ( FecExceptionHandler ) ;
  bool AddInput(std::string block, std::string input) throw ( FecExceptionHandler ) ;

  bool SetDelay(std::string block, int delay) throw ( FecExceptionHandler ) ;

  bool SetCounter(std::string input, int counter) throw ( FecExceptionHandler ) ;
  int GetCounter(std::string input) throw ( FecExceptionHandler ) ;

  bool SetTTCCode(std::string block, int code) throw ( FecExceptionHandler ) ;

  bool SendTrigger(std::string Block) throw ( FecExceptionHandler ) ;


  // wb added: 
  void useLocalCmds();

  void useExternalCmds();

  void useLocalClk();

  void useExternalClk();

  static const std::string BlockNames[8];
  static const std::string Inputs[15];
  static const int Enable[15];
  static const int Select[15]; 

  /** SSID definition
   */
  static const std::string SubSystems[8];

  /** Default value to be set with the SSID
   */
  static const uint32_t CCSVALUESSID[8];

private:
  /** VME access
   */
  HAL::VME64xDevice *trgboard_;

  /** FEC slot
   */
  tscType8 fecSlot_ ;
  
  /** Subsystem for the SSID
   */
  SubSystemsEnum subSystem_ ; 
};
 
#endif // CCSTrigger_hh_included
