/*  
  Author  Evgueni Vlassov 29.05.2005 ITEP, Moscow
*/

#include "FecVmeRingDevice.h"
#include "CCSTrigger.hh"

#include "CCSDefinition.h"

//#define CCSTRIGGERDEBUGMSG
#ifdef DEBUGMSGERROR
#  ifndef CCSTRIGGERDEBUGMSG
#    define CCSTRIGGERDEBUGMSG
#  endif
#endif

// Initialise static variables for the Trigger FPGA firmware names

const std::string CCSTrigger::SubSystems[8] = {"RESERVED0","TRACKER","ECAL","PRESHOWER",
					       "PIXELS","RPC","TOTEM","RESERVED2"};

const std::string CCSTrigger::BlockNames[8] = {"L1","B101B","B110B","B111B","OUT1",
					       "OUT2","OUT3","OUT4"};   

const std::string CCSTrigger::Inputs[15]= {"IN1","IN2","IN3","IN4","L1","B101B","B110B","B111B",
					   "OUT1","OUT2","OUT3","OUT4","SOFT","GEN","TTC"};

const int   CCSTrigger::Enable[15]=       {    1,    1,    1,    1,   2,      2,      2,      2,
					       2,     2,     2,     2,     4,    4,    8};

const int   CCSTrigger::Select[15]=       {    0,    1,    2,    3,   0,      4,      8,    0xc,
					       0x10,  0x14,  0x18,  0x1c,     0, 0x40,    0};

/** Default value to be set with the SSID
 */
const uint32_t CCSTrigger::CCSVALUESSID[8] = {0xB8,0xB8,0x60,0x1B8,0xB8,0xB8,0xB8,0xB8 } ;

/**
 * \param trgBoard - VME64xDevice
 * \param subSystem - SSID
 * \param fecSlot - slot of the FEC in the crate, optional parameter (default 255)
 * \warning the value set in the conf 0 is the value given by CCSVALUESSID and by the SSID
 */
CCSTrigger::CCSTrigger(HAL::VME64xDevice *trgBoard, std::string subSystem, tscType8 fecSlot) throw (FecExceptionHandler) {

  using namespace std; 

  trgboard_ = trgBoard ;
  fecSlot_  = fecSlot  ;

  unsigned i = 0 ; while ((subSystem != SubSystems[i]) && (i < 8)) i ++ ;
  if (i > 7) {

#ifdef CCSTRIGGERDEBUGMSG
    std::cerr << "Error the SSID: " << subSystem << " does not exists" << std::endl ;
#endif

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_FECPARAMETERNOTMANAGED,
					    "bad subsystems to be set in the SSID for the subsystem " + subSystem,
					    FATALERRORCODE,
					    setFecSlotKey(fecSlot)) ;
  }

  // store subsystem value: 
  subSystem_ = (SubSystemsEnum) i ; // not pretty but works..

  // Calculate the SSID
  uint32_t value = (i << SSIDOFFSET) ;
  value |= CCSVALUESSID[i] ;

  if (subSystem_==Preshower) { // preshower
    unsigned trgConf0 = getCCSTriggerConfig0();
    cout <<"TRIG_CONFIG0: 0x" << hex << trgConf0 << dec << endl ; 
    unsigned clkMode = (trgConf0 & 0x00ff); 
    unsigned localCmds = (trgConf0 & 0x0100);
    unsigned ssid = (trgConf0 & 0x7000);

    if (clkMode!=0xb8 && clkMode != 0x58) { 
      // not valid local nor external clock, force local cmds 
      // and local clk 
      localCmds = 0x0100 ; 
      clkMode = 0x58 ; 
    }

    // force local cmds in case of local clk mode 
    if (clkMode == 0x58) localCmds = 0x0100 ; 
    
    // in case ext clock and RESERVED0 first force local cmds 
    if (ssid==0 || clkMode == 0xb8 ) {
       localCmds = 0x0100 ; 
       cout <<"writing TRIG_CONFIG0: 0x" << hex << 0x01b8 << dec << endl ;
       setCCSTriggerConfig0 (0x01b8) ;  
    }

    value = 0x3000 | localCmds | clkMode ; 

    cout <<"writing TRIG_CONFIG0: 0x" << hex << value << dec << endl ;
    setCCSTriggerConfig0 (value) ;

    clear(); // fill defaults
  }
  else { 

    // for others
    setCCSTriggerConfig0 (value) ;
  }

}

/** Just for nothing
 */
CCSTrigger::~CCSTrigger ( ) {

}

/** This method set the CCSTrigger config 0
 * \param value - value to be set
 */
void CCSTrigger::setCCSTriggerConfig0 ( uint32_t value ) throw (FecExceptionHandler) {

  try {
    trgboard_->write("TRIG_CONFIG0", value);

#ifdef CCSTRIGGERDEBUGMSG
    std::cout << "CONFIG0 = 0x" << std::hex << value << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
}

/** This method get the CCSTrigger config 0
 * \return value from the hardware
 */
uint32_t CCSTrigger::getCCSTriggerConfig0 ( ) throw (FecExceptionHandler) {

  uint32_t value = 0 ;

  try {
    trgboard_->read("TRIG_CONFIG0", (haltype *)&value);

#ifdef CCSTRIGGERDEBUGMSG
    std::cout << "CONFIG0 = 0x" << std::hex << value << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return value ;
}

/** This method get the CCSTrigger config 0
 * \return value from the hardware
 */
uint32_t CCSTrigger::getCCSTriggerStatus0 ( ) throw (FecExceptionHandler) {

  uint32_t value = 0 ;

  try {
    trgboard_->read("TRIG_STATUS0", (haltype *)&value);

#ifdef CCSTRIGGERDEBUGMSG
    std::cout << "CONFIG0 = 0x" << std::hex << value << std::endl ;
#endif
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return value ;
}

/** Return the corresponding SSID
 * \return SSID for the given subsystem
 */
std::string CCSTrigger::getSSID ( ) throw (FecExceptionHandler) {

  uint32_t value = getCCSTriggerConfig0 ( ) ;
  uint32_t SSID  =  (value & SSIDMASK) >> SSIDOFFSET ;

  return SubSystems[SSID] ;
}

/** Set the SSIS in the corresponding FEC
 * \param SSID - SSID string
  * \warning the value set in the conf 0 is the value given by CCSVALUESSID and by the SSID
 */
void CCSTrigger::setSSID ( std::string subSystem ) throw (FecExceptionHandler) {

  using namespace std; 
 
  int i = 0 ; while ((subSystem != SubSystems[i]) && (i < 8)) i ++ ;

  if (i > 7) 
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_FECPARAMETERNOTMANAGED,
					    "Bad subsystem for SSID " + subSystem,
					    FATALERRORCODE,
					    setFecSlotKey(fecSlot_) ) ;

  // store subsystem value:
  subSystem_ = (SubSystemsEnum) i ; // not pretty but works..

  // Calculate the SSID
  uint32_t value = (i << SSIDOFFSET) ;
  value |= CCSVALUESSID[i] ;


  if (subSystem_==Preshower) { // preshower
    unsigned trgConf0 = getCCSTriggerConfig0();
    cout <<"TRIG_CONFIG0: 0x" << hex << trgConf0 << dec << endl ;
    unsigned clkMode = (trgConf0 & 0x00ff);
    unsigned localCmds = (trgConf0 & 0x0100);
    unsigned ssid = (trgConf0 & 0x7000);
                                                                                                                                       
    if (clkMode!=0xb8 && clkMode != 0x58) {
      // not valid local nor external clock, force local cmds
      // and local clk
      localCmds = 0x0100 ;
      clkMode = 0x58 ;
    }
                                                                                                                                       
    // force local cmds in case of local clk mode
    if (clkMode == 0x58) localCmds = 0x0100 ;
                                                                                                                                       
    // in case ext clock and RESERVED0 first force local cmds
    if (ssid==0 || clkMode == 0xb8 ) {
       localCmds = 0x0100 ;
       cout <<"writing TRIG_CONFIG0: 0x" << hex << 0x01b8 << dec << endl ;
       setCCSTriggerConfig0 (0x01b8) ;
    }
                                                                                                                                       
    value = 0x3000 | localCmds | clkMode ;
                                                                                                                                       
    cout <<"writing TRIG_CONFIG0: 0x" << hex << value << dec << endl ;
    setCCSTriggerConfig0 (value) ;
                                                                                                                                       
    clear(); // fill defaults
                                                                                                                                       
                                                                                                                                       
  } else {
    // for others
    setCCSTriggerConfig0 (value) ;
  }
}


/** Set the CONFIG0
 */
void CCSTrigger::setSSID ( std::string subSystem, uint32_t originalValue ) throw (FecExceptionHandler) {
 
  int i = 0 ; while (subSystem != SubSystems[i]) i ++ ;
  if (i > 7) 
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( TSCFEC_FECPARAMETERNOTMANAGED,
					    "Bad subsystem for SSID " + subSystem,
					    FATALERRORCODE,
					    setFecSlotKey(fecSlot_) ) ;

  // store subsystem value:
  subSystem_ = (SubSystemsEnum) i ; // not pretty but works..

  // Calculate the SSID
  uint32_t value = (i << SSIDOFFSET) ;
  value |= (originalValue & 0x01ff );

  setCCSTriggerConfig0 (value) ;
}

/** Reset the TTCRx
 */
void CCSTrigger::setResetTTCRx ( ) throw (FecExceptionHandler) {
 
  // Read the value
  uint32_t value = getCCSTriggerConfig0() ;

  // Put the reset TTCRx to 0
  value &= (~TTCRXRESET) ;
  setCCSTriggerConfig0 (value) ;

  // Put the reset TTCRx to 1
  value |= TTCRXRESET ;
  setCCSTriggerConfig0 (value) ;
}

/** Reset the QPLL
 */
void CCSTrigger::setResetQPLL ( ) throw (FecExceptionHandler) {

  // Read the value
  uint32_t value = getCCSTriggerConfig0() ;

  // Put the reset TTCRx to 0
  value &= (~QPLLRESET) ;
  setCCSTriggerConfig0 (value) ;

  // Put the reset TTCRx to 1
  value |= QPLLRESET ;
  setCCSTriggerConfig0 (value) ;
}

/** Set the QPLL auto restart
 */
void CCSTrigger::setQPLLAutoRestart ( ) throw (FecExceptionHandler) {

  // Read the value
  uint32_t value = getCCSTriggerConfig0() ;

  // Put the reset TTCRx to 1
  value |= QPLLAUTORESTART ;
  setCCSTriggerConfig0 (value) ;

  // Put the reset TTCRx to 0
  value &= (~QPLLAUTORESTART) ;
  setCCSTriggerConfig0 (value) ;

  // Put the reset TTCRx to 1
  value |= QPLLAUTORESTART ;
  setCCSTriggerConfig0 (value) ;
}

/** Error on QPLL
 * \return true if error false if not
 */
bool CCSTrigger::getStatusQPLLError ( ) throw (FecExceptionHandler) {

  return (getCCSTriggerStatus0() & STATUS0_QPLLERROR) ;
}

/** QPLL locked
 * \return true if locked false if not
 */
bool CCSTrigger::getStatusQPLLLocked ( ) throw (FecExceptionHandler) {

  return (getCCSTriggerStatus0() & STATUS0_QPLLLOCKED) ;
}

/** TTCRx Ready
 * \return true if ready false if not
 */
bool CCSTrigger::getStatusTTCRxReady ( ) throw (FecExceptionHandler) {

  return (getCCSTriggerStatus0() & STATUS0_TTCRXREADY) ;
}

/**
 */
void CCSTrigger::clear() throw (FecExceptionHandler) {

  try {

    for (int i=0;i<8;i++) {
      trgboard_->write("TRIG_LOC_"+BlockNames[i],0);
    }

    trgboard_->write("TRIG_LOC_EXTIN0",0);
    trgboard_->write("TRIG_LOC_EXTIN1",0);
    trgboard_->write("TRIG_LOC_STRB",0x1f00);
    unsigned loc_bcmd = 0 ; 
    if (subSystem_ == Preshower ) loc_bcmd = 0x0051 ; 
    trgboard_->write("TRIG_LOC_BCMD",loc_bcmd);
    trgboard_->write("TRIG_LOC_FREQ",0);
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
}

/**
 */
bool CCSTrigger::SetFrequency(int frequency) throw (FecExceptionHandler) {

  try {
    uint32_t value;
    trgboard_->read("TRIG_CONFIG0",(haltype *)&value);
    if((frequency>=0) && (frequency<0x40)) value = value | 0x40 | frequency;
    else return false;
    trgboard_->write("TRIG_CONFIG0",value);
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return true; 
}

/**
 */
int CCSTrigger::GetFrequency() throw (FecExceptionHandler) {

  try {
    uint32_t value;
    trgboard_->read("TRIG_CONFIG0",(haltype *)&value);
    return value & 0x3f;
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return 0 ;
}

/**
 */
uint32_t CCSTrigger::getID() throw (FecExceptionHandler) {

  try {
    uint32_t value;
    trgboard_->read("TRIG_ID",(haltype *)&value);
    return value;
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return 0 ;
}

/**
 */
bool CCSTrigger::SetInput(std::string block, std::string input) throw (FecExceptionHandler) {

  try {
    for (int bl=0; bl<8; bl++) {
      if(block == BlockNames[bl]) {
	for (int inp=0;inp<15;inp++) {
	  if(input == Inputs[inp]) {
	    uint32_t value;
	    trgboard_->read("TRIG_LOC_"+block,(haltype *)&value);
	    value = value & 0xffff0000;
	    uint32_t selena = Enable[inp] | (Select[inp]<<8);
	    value = value | selena ; 
	    trgboard_->write("TRIG_LOC_"+block,value);
	    return true;
	  }
	}
      }
    }
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return false;
}

/**
 */
bool CCSTrigger::AddInput(std::string block, std::string input) throw (FecExceptionHandler) {

  try {
    for (int bl=0; bl<8; bl++) {
      if(block == BlockNames[bl]) {
	for (int inp=0;inp<15;inp++) {
	  if(input == Inputs[inp]) {
	    uint32_t value;
	    trgboard_->read("TRIG_LOC_"+block,(haltype *)&value);
	    int selena = Enable[inp] | (Select[inp]<<8);
	    trgboard_->write("TRIG_LOC_"+block,(selena|value));
	    return true;
	  }
	}
      }
    }
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return false;
}

/**
 */
bool CCSTrigger::SetDelay(std::string block, int Delay) throw (FecExceptionHandler) {

  try {
    for (int bl=0; bl<8; bl++) {
      if(block == BlockNames[bl]) {
	int delay = Delay;
	if((delay>0xffff) || (delay<0)) delay = 0xffff;
	uint32_t value;
	trgboard_->read("TRIG_LOC_"+block,(haltype *)&value);
	value = (value & 0x0000ffff) | (delay<<16);
	trgboard_->write("TRIG_LOC_"+block,value);
	return true;
      }
    }
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return false;
}

/**
 */
bool CCSTrigger::SetCounter(std::string input, int Counter) throw (FecExceptionHandler) {

  try {
    std::string extin;
    int counter = Counter;;
    counter &= 0xffff;
    for (int bl=0; bl<4; bl++) {
      if(input == Inputs[bl]) {
	uint32_t value;
	if(bl<2) extin = "TRIG_LOC_EXTIN0";
	else extin = "TRIG_LOC_EXTIN1";
	trgboard_->read(extin,(haltype *)&value);
	if(bl%2 == 0) {
	  value &=0xffff0000;
	  value |= counter;
	} else {
	  value &=0x0000ffff;
	  value |= (counter<<16);
	}
	trgboard_->write(extin,value);
	//cout<<"SetCounter: extin="<<extin<<"  value="<<hex<<value<<endl;
	trgboard_->read(extin,(haltype *)&value);
	//cout<<"SetCounter2: extin="<<extin<<"  value="<<hex<<value<<endl;
	
	return true;
      }
    }
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return false;
}

/**
 */
int CCSTrigger::GetCounter(std::string input) throw (FecExceptionHandler) {

  try {
    std::string extin;
    for (int bl=0; bl<4; bl++) {
      if(input == Inputs[bl]) {
	uint32_t value;
	if(bl<2) extin = "TRIG_LOC_EXTIN0";
	else extin = "TRIG_LOC_EXTIN1";
	trgboard_->read(extin,(haltype *)&value);
	//cout<<"Get1: extin="<<extin<<" value="<<hex<<value<<endl;
	if(bl%2 != 0) value = value>>16; 
	value &=0x0000ffff;
	//cout<<"Get2: value="<<hex<<value<<endl;
	return value;
      }
    }
    return -1; // input block not found
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return -1;
}

/**
 */
bool CCSTrigger::SetTTCCode(std::string block, int Code) throw (FecExceptionHandler) {

  try {
    for (int bl=1; bl<8; bl++) {
      if(block == BlockNames[bl]) {
	int code = Code;
	int shift = (bl-1)*4;
	code = code & 0xf;
	uint32_t value;
	trgboard_->read("TRIG_LOC_BCMD",(haltype *)&value);
	value = value & (~(0xf<<shift));
	value |= (code<<shift);
	trgboard_->write("TRIG_LOC_BCMD",value);
	return true;
      }
    }
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return false;
}

/**
 */
bool CCSTrigger::SendTrigger(std::string input) throw (FecExceptionHandler) {

  bool found=false;
  try {
    for (int bl=0; bl<8; bl++) {
      if(input == BlockNames[bl]) {
	uint32_t value = 1<<bl;
	trgboard_->write("TRIG_LOC_STRB",value);
	found = true;
	break;
      }
    }
    if(!found) {
      for (int bl=0; bl<4; bl++) {
	if(input == Inputs[bl]) {
	  uint32_t value = 1<<(bl+8);
	  trgboard_->write("TRIG_LOC_STRB",value);
	  found = true;
	  break;
	}
      }
    }
  }
  // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // If you write to an item you can specify in the call a parameter which
  // then makes hal immediately read back the item you wrote in order to
  // verify that it really arrived in a register. This exception is thrown
  // then the value read back is not equal to the one originally written.
  // (Perfect for automatic hardware tests)
  catch ( HAL::VerifyException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_VERIFYEXCEPTION,
					    HAL_VERIFYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Imagine you have defined an item with the mask 0x000000f0
  // this means the item is contained in bits 4..7
  // now you may write values 0 to 0xf into these 4 bits. If you try to write
  // 10 you get this exception since it would try to set bit 8 which is out
  // of the mask region
  catch ( HAL::MaskBoundaryException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_MASKBOUNDARYEXCEPTION,
					    HAL_MASKBOUNDARYEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;    
  }
  // When you work with offsets it is checked that you stay in the limits of
  // the addressTable. This gives some means of security against programming
  // bugs when you work with offsets. (See manual if you need more info but
  // there is not much more behind it...)
  catch ( HAL::AddressOutOfLimitsException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ADDRESSOUTOFLIMITSEXCEPTION,
					    HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG + (std::string)e.what(),
					    ERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }

  return found;
}

void CCSTrigger::useLocalCmds() { 
  unsigned trgConf0 = getCCSTriggerConfig0(); 
  trgConf0 |= 0x100 ; 
  setCCSTriggerConfig0(trgConf0);
}

void CCSTrigger::useExternalCmds() { 
  unsigned trgConf0 = getCCSTriggerConfig0(); 
  trgConf0 &= (~0x100) ; // pull down local mode bit
  setCCSTriggerConfig0(trgConf0);

} 

void CCSTrigger::useLocalClk() { 
  unsigned trgConf0 = getCCSTriggerConfig0(); 
  unsigned clockMode = trgConf0 & 0x00ff ; 
  unsigned localCmds = 0x0100 ; // force local commds too..
  unsigned ssid =  trgConf0 & 0x7000 ;
  clockMode = 0x58 ; 
  setCCSTriggerConfig0(ssid | localCmds | clockMode );
}

void CCSTrigger::useExternalClk() { 
  unsigned trgConf0 = getCCSTriggerConfig0(); 
  unsigned clockMode = trgConf0 & 0x00ff ; 
  unsigned localCmds = 0x0100 ; // force local commds too..
  unsigned ssid =  trgConf0 & 0x7000 ;
  clockMode = 0xb8; 
  setCCSTriggerConfig0(localCmds | clockMode );
  // do we need delay here ?
  setCCSTriggerConfig0(ssid | localCmds | clockMode );
} 
