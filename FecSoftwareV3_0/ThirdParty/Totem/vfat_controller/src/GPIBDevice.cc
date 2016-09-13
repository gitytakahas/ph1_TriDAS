/*
Author:
Juha Petäjäjärvi
*/

#include "GPIBDevice.h"

GPIBDevice::GPIBDevice(){}

// Constructor - open device
GPIBDevice::GPIBDevice ( int index, int primary, int secondary, int timeout, int mode, int smode ){

  // Create a unit descriptor handle
  deviceKey_ = ibdev( index,     // Board Index
		      primary,   // Device primary address
		      secondary, // Device secondary address
		      timeout,   // Timeout setting
		      mode,      // Assert EOI line at end of write
		      smode ) ;  // EOS termination mode

  // Check GPIB Error
  if (ibsta & ERR) { error("ibdev Error") ; }

  // Clear device
  ibclr(deviceKey_) ;

  // Check GPIB Error
  if (ibsta & ERR) { error("ibclr Error") ; }
}

// Deconstructor - release device
GPIBDevice::~GPIBDevice(){

  release() ;
}

// Set up power supply
void GPIBDevice::setup( float voltage, float current){

  setVoltage(voltage) ;

  setCurrent(current) ;

  //powerOn() ; // PS -> ON
  //sleep(5) ;
	
  // Measure the voltage and the current

  //float u = getVoltage() ;
  //float i = getCurrent() ;

  //std::cout << "Voltage read back: " << u << " V" << std::endl ;
  //std::cout << "Current read back: " << i << " A" << std::endl ;
}

// Release power supply
void GPIBDevice::release(){

  ibonl(deviceKey_, 0) ;
  if (ibsta & ERR) { error("ibonl Error") ; }
}

// Power on the power supply
void GPIBDevice::powerOn(){

  std::string s = "op 1" ;
  ibwrt(deviceKey_, (char*)s.c_str() , s.size()) ;

  if (ibsta & ERR) { error("ibwrt Error") ; }
}

// Power off the power supply
void GPIBDevice::powerOff(){

  std::string s = "op 0" ;
  ibwrt(deviceKey_, (char*)s.c_str() , s.size()) ;

  if (ibsta & ERR) { error("ibwrt Error") ; }
}

// Set voltage
void GPIBDevice::setVoltage( float voltage ){
  
  std::stringstream ss ;
  ss << "V " << voltage ;

  std::string cmd = ss.str() ;

  ibwrt(deviceKey_, (char*)cmd.c_str() , cmd.size()) ;
  if (ibsta & ERR) { error("ibwrt Error") ; }
}

// Set current
void GPIBDevice::setCurrent( float current ){

  std::stringstream ss ;
  ss << "I " << current ;

  std::string cmd = ss.str() ;

  ibwrt(deviceKey_, (char*)cmd.c_str() , cmd.size()) ;
  if (ibsta & ERR) { error("ibwrt Error") ; }
}

// Read voltage
float GPIBDevice::getVoltage(){

  char* reading;
  std::string cmd = "VO?" ;

  ibwrt(deviceKey_, (char*)cmd.c_str() , cmd.size()) ;
  if (ibsta & ERR) { error("ibwrt Error") ; }

  // Measure voltage
  ibrd(deviceKey_, reading ,100) ;     
  if (ibsta & ERR) { error("ibrd Error") ; }

  return(atof(reading));
}

// Read current
float GPIBDevice::getCurrent(){

  char* reading ;
  std::string cmd = "IO?" ;

  ibwrt(deviceKey_, (char*)cmd.c_str() , cmd.size()) ;
  if (ibsta & ERR) { error("ibwrt Error") ; }

  // Measure current
  ibrd(deviceKey_, reading ,100) ;     
  if (ibsta & ERR) { error("ibrd Error") ; }

  return(atof(reading));
}

// General GPIB read
std::string GPIBDevice::read( std::string cmd ){

  char reading[100] ;

  // Write command
  ibwrt(deviceKey_, (char*)cmd.c_str(), cmd.size());     
  if (ibsta & ERR) { error("ibwrt Error") ; }

  // Read answer
  ibrd(deviceKey_, reading ,100) ;     
  if (ibsta & ERR) { error("ibrd Error") ; }

  //printf("chain value : %s \n",reading);

  std::string str = reading ;

  return (str) ;
}

// General GPIB write
void GPIBDevice::write( std::string cmd ){

   ibwrt(deviceKey_, (char*)cmd.c_str(), cmd.size()) ;
   if (ibsta & ERR) { error("ibwrt Error") ; }
}

// Show GPIB error message
void GPIBDevice::error( std::string msg ) {

  std::cout << msg << std::endl ;
  std::cout << "ibsta = " << std::hex << ibsta << " <" ;

  if (ibsta & ERR ){ std::cout << " ERR" ; }
  if (ibsta & TIMO){ std::cout << " TIMO" ; }
  if (ibsta & END ){ std::cout << " END" ; }
  if (ibsta & SRQI){ std::cout << " SRQI" ; }
  if (ibsta & RQS ){ std::cout << " RQS" ; }
  if (ibsta & CMPL){ std::cout << " CMPL" ; }
  if (ibsta & LOK ){ std::cout << " LOK" ; }
  if (ibsta & REM ){ std::cout << " REM" ; }
  if (ibsta & CIC ){ std::cout << " CIC" ; }
  if (ibsta & ATN ){ std::cout << " ATN" ; }
  if (ibsta & TACS){ std::cout << " TACS" ; }
  if (ibsta & LACS){ std::cout << " LACS" ; }
  if (ibsta & DTAS){ std::cout << " DTAS" ; }
  if (ibsta & DCAS){ std::cout << " DCAS" ; }
  std::cout << " >" << std::dec << std::endl ;

  std::cout << "iberr = " << iberr ;
  if (iberr == EDVR){ std::cout << " EDVR <Driver Error>" << std::endl ; }
  if (iberr == ECIC){ std::cout << " ECIC <Not Controller-In-Charge>" << std::endl ; }
  if (iberr == ENOL){ std::cout << " ENOL <No Listener>" << std::endl ; }
  if (iberr == EADR){ std::cout << " EADR <Address error>" << std::endl ; }
  if (iberr == EARG){ std::cout << " EARG <Invalid argument>" << std::endl ; }
  if (iberr == ESAC){ std::cout << " ESAC <Not System Controller>" << std::endl ; }
  if (iberr == EABO){ std::cout << " EABO <Operation aborted>" << std::endl ; }
  if (iberr == ENEB){ std::cout << " ENEB <No GPIB board>" << std::endl ; }
  if (iberr == EDMA){ std::cout << " EDMA <DMA Error>" << std::endl ; }
  if (iberr == EOIP){ std::cout << " EOIP <Async I/O in progress>" << std::endl ; }
  if (iberr == ECAP){ std::cout << " ECAP <No capability>" << std::endl ; }
  if (iberr == EFSO){ std::cout << " EFSO <File system error>" << std::endl ; }
  if (iberr == EBUS){ std::cout << " EBUS <Command error>" << std::endl ; }
  if (iberr == ESRQ){ std::cout << " ESRQ <SRQ stuck on>" << std::endl ; }
  if (iberr == ETAB){ std::cout << " ETAB <Table Overflow>" << std::endl ; }
  if (iberr == ELCK){ std::cout << " ELCK <Interface is locked>" << std::endl ; }
  if (iberr == EARM){ std::cout << " EARM <ibnotify callback failed to rearm>" << std::endl ; }
  if (iberr == EHDL){ std::cout << " EHDL <Input handle is invalid>" << std::endl ; }
  if (iberr == EWIP){ std::cout << " EWIP <Wait in progress on specified input handle>" << std::endl ; }
  if (iberr == ERST){ std::cout << " ERST <The event notification was cancelled due to a reset of the interface>" << std::endl ; }
  if (iberr == EPWR){ std::cout << " EPWR <The interface lost power>" << std::endl ; }
  std::cout << "ibcntl = " << ibcntl << std::endl ;

  // Call ibonl to take the device and interface offline
  ibonl(deviceKey_, 0);
}

int GPIBDevice::getDeviceKey(){

  return deviceKey_ ;
}
