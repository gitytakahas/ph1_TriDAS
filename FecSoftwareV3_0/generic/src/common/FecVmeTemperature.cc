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
  Copyright Feb 2005, Evgueni Vlassov ITEP, Moscow
*/

#include <iostream>

#include <unistd.h> // for usleep

#include "tscTypes.h"
#include "keyType.h"

#include "FecVmeTemperature.h"


/******************************************************
		CONSTRUCTOR - DESTRUCTOR
******************************************************/

/** Build a FecVmeTemperature and initalise the access
 */
FecVmeTemperature::FecVmeTemperature (HAL::VME64xDevice *vmedevice, unsigned int fecSlot ) throw ( FecExceptionHandler ) {
  vmeBoard_ = vmedevice;
  fecSlot_ = fecSlot ;
  try {
    reset((char *)item1);
    reset((char *)item2);
    outp2(1,0xCC); // skip ROM
    waitforInterrupt();
    outp2(1,0x4E); // write memory
    waitforInterrupt();
    outp2(1,0xff); // write Th
    waitforInterrupt();
    outp2(1,0x00); // write Tl
    waitforInterrupt();
    outp2(1,0x00); // write Conf Reg (resolution = 0.5 degree C, 94ms conversion time)
    waitforInterrupt();
  } 
    // Bad item name => Software problem
  catch ( HAL::NoSuchItemException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_NOSUCHITEMEXCEPTION,
					    HAL_NOSUCHITEMEXCEPTION_MSG + (std::string)e.what(),
					    FATALERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
  // Hardware problem access: Crate, SBS, driver SBS
  catch ( HAL::BusAdapterException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_BUSADAPTEREXCEPTION,
					    HAL_BUSADAPTEREXCEPTION_MSG + (std::string)e.what(),
					    FATALERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    // e.what()) ;
  }
  // Bad operation
  catch ( HAL::IllegalOperationException &e ) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( HAL_ILLEGALOPERATIONEXCEPTION,
					    HAL_ILLEGALOPERATIONEXCEPTION_MSG + (std::string)e.what(),
					    FATALERRORCODE,
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
					    FATALERRORCODE,
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
					    FATALERRORCODE,
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
					    FATALERRORCODE,
					    setFecSlotKey(getFecSlot()) ) ;
    //e.what()) ;
  }
}

/*
  Destructor ::
  Standard implementation
*/
FecVmeTemperature::~FecVmeTemperature ( ) throw ( FecExceptionHandler ) {
}



int FecVmeTemperature::getTemp(int tempSlot) throw ( FecExceptionHandler ) {

  if(tempSlot<1 || tempSlot >2)
    RAISEFECEXCEPTIONHANDLER_INFOSUP (CODECONSISTENCYERROR,
				      "There are 2 temperature sensonrs only... (1 & 2)",
				      FATALERRORCODE,
				      setFecSlotKey(getFecSlot()),
				      "Sensor requested", tempSlot) ;
  
  try { 
    outp2(0,0x01); //reset both channels
    waitforInterrupt();
    //std::cout << "before Convert" << std::endl;
    convertT();
    usleep(95000);
    //std::cout << "Before readTemps"<< std::endl;
    readTemps();    
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

  return(TEMPS[tempSlot-1]);
}

//individual Serial #s and readings
//Original source is from http://pdfserv.maxim-ic.com/en/an/AN520.pdf

int FecVmeTemperature::reset(char *Item)
{
  outp(Item,0,0x01); // send reset
  usleep(1000);
  
  //std::cout<< "inp from item=" << Item << std:Lendl;
  int locm = inp(Item,2);
  if(locm & 0x42) {
    std::cout << Item <<"  not found, irq reg=" << std::hex << locm << std::endl;
    return(1); 
  }
  //no presence found
  else {
    //std::cerr << Item <<" Device found! irq reg= " << std::hex << locm << std::endl;
    return(0); //presence found
  }
}
void FecVmeTemperature::convertT()
{
  outp2(0,0x01); // send reset
  waitforInterrupt();
  outp2(1,0xCC); // skip ROM
  waitforInterrupt();
  outp2(1,0x44); // convert Temp.
}
void FecVmeTemperature::readTemps()
{
  int LSB1,MSB1,LSB2,MSB2;
     outp2(0,0x01); // send reset
     waitforInterrupt();
     outp2(1,0xCC); // skip ROM
     waitforInterrupt();
     outp2(1,0xBE); // read memory
     waitforInterrupt();   
     outp2(1,0xFF); // read LSB
     waitforInterrupt();
     LSB1 = inp((char *)item1,1);
     LSB2 = inp((char *)item2,1);
     //std::cout << "LSB1=" << std::hex << LSB1 << "  LSB2=" << LSB2 << std::endl;
     outp2(1,0xFF); // read MSB
     waitforInterrupt();
     MSB1 = inp((char *)item1,1);
     MSB2 = inp((char *)item2,1);
     //std::cout << "MSB1=" << std::hex << MSB1 << "  MSB2=" << MSB2 << std::endl;
     TEMPS[0] = ((MSB1<<8) + LSB1 + 0x08)/16;
     TEMPS[1] = ((MSB2<<8) + LSB2 + 0x08)/16;
}

int FecVmeTemperature::inp(char * Item, int Offset){

  uint32_t Value;
  try {
    vmeBoard_->read(Item, (haltype *)&Value,Offset*4);
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

  return (Value);
}

void FecVmeTemperature::outp(char * Item, int Offset , tscType32 Value){
  try { 
    vmeBoard_->write(Item, Value, HAL::HAL_NO_VERIFY, Offset*4);
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

void FecVmeTemperature::outp2(int Offset , tscType32 Value){
  try { 
    vmeBoard_->write(item1, Value, HAL::HAL_NO_VERIFY, Offset*4);
    vmeBoard_->write(item2, Value, HAL::HAL_NO_VERIFY, Offset*4);
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

void FecVmeTemperature::waitforInterrupt(){
  usleep(1000);
  //int locm = inp(item,2);
  //std::cout << "Inter=" << std::hex <<locm;
  //if(locm & 0x10) std::cout<<"  RBF is ON"<< std::endl;
  //else cout<<endl;
  //locm = inp(item,2);
  //std::cout << "Inter#" << std::hex << locm << std::endl;
};

