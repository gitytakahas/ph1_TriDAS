/*
Author:
Juha Petäjäjärvi
*/

#ifndef GPIBDEVICE_H
#define GPIBDEVICE_H

#include <iostream>
#include <sstream>
#include "ni488.h"
#include <stdio.h>

class GPIBDevice {

 private:

  int deviceKey_ ;

  void error( std::string msg ) ;

 public:

  GPIBDevice() ;

  GPIBDevice( int index, int primary, int secondary, int timeout, int mode, int smode ) ;

  ~GPIBDevice() ;

  void setup( float voltage, float current ) ;

  void release() ;

  void powerOn() ;

  void powerOff() ;

  void setVoltage( float voltage ) ;

  void setCurrent( float current ) ;

  float getVoltage() ;

  float getCurrent() ;

  std::string read( std::string cmd ) ;

  void write( std::string cmd ) ;

  int getDeviceKey() ;

} ;

#endif
