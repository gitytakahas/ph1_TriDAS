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
#include <iostream>

#include "FecVmeRingDevice.h"

#include "i2cAccess.h"
#include "apvAccess.h"
#include "muxAccess.h"
#include "pllAccess.h"
#include "laserdriverAccess.h"
#include "DohAccess.h"
#include "dcuAccess.h"
#include "FecAccess.h"

// The keyType is needed for all the methods, it is composed by a FEC, ring, CCU, channel, i2c address
// All the parameters are not needed
// This method ask you to enter the value of the FEC/Ring/CCU/Channel/I2C address and return the key built
keyType getAccessKey ( ) ;
// This method takes the FEC/Ring/CCU/Channel and i2c address and build the corresponding key
keyType getAccessKey ( int fecSlot, int ringSlot, int ccuAddress = 0, int ccuChannel = 0, int i2cAddress = 0 ) ;
// This method parse the arguments given in file and convert it to fecSlot, ringSlot, ccuAddress, ccuChannel and i2cAddress
keyType parseArgv ( int argc, char *argv[] ) ;

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       I2C part                                          */
/*                                                                         */
/* ----------------------------------------------------------------------- */
void i2cPart ( FecAccess &fecAccess, int argc, char *argv[] ) {

  // Channel index
  keyType indexChannel = 0 ;

  // Get the channels where the device are, if no arguments are given
  if (argc == 0) {
    indexChannel = getAccessKey ( ) ;
  } else {
    // parse the parameters
    indexChannel = parseArgv ( argc, argv ) ;
  }

  // Just for display
  char msgChannel[80] ; decodeKey(msgChannel,indexChannel) ;

  // Create an access to the corresponding ring
  FecRingDevice *fecRingDevice = NULL ;
  try {
    fecRingDevice = fecAccess.getFecRingDevice ( indexChannel ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot access the ring that connects the channel " << msgChannel << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;    

    return ;
  }

  // Parameter settings

  // if you intend to access the device in RAL MODE please set it to and comment the others:
  // enumDeviceType deviceType = APV25 ; 
  // if you intend to access the device in EXTENDED MODE please set it to and comment the others:
  // enumDeviceType deviceType = FOREXTENDED ; 
  // if you intend to access the device in NORMAL MODE please set it to and comment the others:
  enumDeviceType deviceType = PHILIPS ; 
  enumAccessModeType accessMask = MODE_SHARE ; // read/write access

  // Create an i2c access
  i2cAccess i2c(indexChannel,deviceType,accessMask,fecRingDevice) ;

  // Enable the corresonding channel
  try {
    i2c.setChannelEnable (true) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Unable to enable the channel " << msgChannel << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;    

    return ;
  }

  // Initialise the channel
  bool useForceAcknowledge = true ;
  tscType16 i2cSpeed = 400 ;
  try {
    i2c.setChannelInit (useForceAcknowledge,i2cSpeed) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Unable to initialise the channel " << msgChannel << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;    

    return ;
  }

  // Access to a register in the different i2c mode
  try {
    // ----------------------------------------- Normal mode
    // Write the one register in normal mode
    i2c.write (0xFF) ;
    // Read back the value
    tscType8 valueN = i2c.read() ;

    if (valueN != 0xFF) std::cout << "Value read is different than the value written " << std::hex << valueN << "/" << 0xFF << std::endl ;
    
    // ----------------------------------------- Extended mode
    // the enumDeviceType deviceType must be set to FOREXTENDED
    tscType8 offsetE = 0 ;
    i2c.write ( offsetE, 0xFF ) ;
    tscType8 valueE = i2c.read ( offsetE ) ;

    if (valueE != 0xFF) std::cout << "Value read is different than the value written " << std::hex << valueE << "/" << 0xFF << std::endl ;

    // ----------------------------------------- Ral mode
    // the enumDeviceType deviceType must be set to APV25
    tscType8 offsetR = 0 ;
    i2c.write ( offsetR, 0xFF ) ;
    tscType8 valueR = i2c.read ( offsetR ) ;

    if (valueR != 0xFF) std::cout << "Value read is different than the value written " << std::hex << valueR << "/" << 0xFF << std::endl ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Access error to the device " << msgChannel << std::endl ;
    std::cerr << "*******************************************" << std::endl ;    
  }

  // Disable the channel
  try {
    i2c.setChannelEnable (false) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Unable to enable the channel " << msgChannel << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;    

    return ;
  }
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                       Frame part                                         */
/*                                                                          */
/* in this example the channel must be enable and the force acknowledge set */
/*                                                                          */
/* ------------------------------------------------------------------------ */
void framePart ( FecAccess &fecAccess, int argc, char *argv[] ) {
  // Channel index
  keyType indexChannel = 0 ;

  // Get the channels where the device are, if no arguments are given
  if (argc == 0) {
    indexChannel = getAccessKey ( ) ;
  } else {
    // parse the parameters
    indexChannel = parseArgv ( argc, argv ) ;
  }

  // Just for display
  char msgChannel[80] ; decodeKey(msgChannel,indexChannel) ;

  // Retreive the corresponding ring
  FecRingDevice *fecRingDevice = NULL ;
  try {
     fecRingDevice = fecAccess.getFecRingDevice ( indexChannel ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot access the ring that connects the channel " << msgChannel << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;    

    return ;
  }

  // Retreive a transaction number available
  tscType8 tnum = 0x10 ;

  // --------------------------------------------------------------------------
  // Build an i2c frame in 8 bits words => i2c access in normal mode
  // Build the frame and send it via the method write frame
  /* Dest        = index, 
   * Src         = 0x0, 
   * Length      = 4, 
   * Channel     = index
   * tnum        = transaction number
   * Command     = single byte read normal mode
   * Address     = index
   */
  tscType8 realSize = 4 ; // size of the frames
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(indexChannel), FRAMEFECNUMBER, 
    realSize, getChannelKey(indexChannel), 
    tnum, CMD_SINGLEBYTEREADNORMALMODE,
    getAddressKey(indexChannel) } ;
  tscType8 frameR[DD_USER_MAX_MSG_LENGTH*4] ; // answer

  // Display
  std::cout << "Frame to be send ( { " ;
  for (int i = 0 ; i < (realSize+3) ; i ++ )
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  std::cout << "} )" << std::dec << std::endl ;

  // ----------------------------------- Use the FecRingDevice method to send and receive
  // ----------------------------------- You can find later in this example the way to send the frame by yourself
  // Here the send is done by the FecRingDevice::writeFrame method and all the possible error is managed by that method
  try {

    // Send the frame
    fecRingDevice->writeFrame (frame) ;
    
    // display the direct ack
    std::cout << "Direct acknowledge ( { " ;
    for (int i = 0 ; i < (realSize+3+1) ; i ++ )
      std::cout << "0x" << std::hex << (int)frame[i] << " " ;
    std::cout << "} )" << std::dec << std::endl ;
  
    // Receive the answer
    // You need to specify the transaction number that has been used
    // a frame output is returned
    // optional parameter, the size you are expected in the length word of the frame, only used for i2c command
    fecRingDevice->readFrame (tnum,frameR,3) ;

    // Display the frame received
    std::cout << "Answer ( { " ;
    for (int i = 0 ; i < 5 ; i ++ )
      std::cout << "0x" << std::hex << (int)frame[i] << " " ;
    std::cout << "} )" << std::dec << std::endl ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Unsable to send or receive a frame for the device " << msgChannel << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        
  }

  // ----------------------------------- Here the manual send of a frame

  // Convert it in 32 bits word
  tscType32 c[DD_USER_MAX_MSG_LENGTH] ;
  tscType32 realSize32 = (realSize+3)/4 ;
  tscType32 cpt = 0 ;
  if ( ((realSize+3) % 4) != 0 ) realSize32 += 1 ;
  for (tscType32 i = 0 ; i < realSize32 ; i ++) {

    c[i] = (frame[cpt] << 24) | (frame[cpt+1] << 16) | (frame[cpt+2] << 8) | frame[cpt+3] ;
    std::cout << "c[" << std::dec << i << "] = 0x" << std::hex << c[i] << ", frame[" << std::dec << cpt << "] =  0x" << std::hex << (int)frame[cpt] << std::endl ;
    // Next word
    cpt += 4 ;
  }

  // Clear the error bit
  fecRingDevice->setFecRingCR1 ( DD_FEC_CLEAR_ERRORS ) ;

  // Set the frame in the FIFO transmit
  fecRingDevice->setFifoTransmit(c,realSize32); 

  // Send it over the ring
  // Toggle the send bit of CTRLO with RMW operation
  fecRingDevice->setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_OR ) ;
  fecRingDevice->setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_XOR ) ;

  // Wait for the end of the transmission
#define TIMEOUT 100000
  unsigned int timeout = 0 ;
  tscType16 fecSR0 = fecRingDevice->getFecRingSR0() ;
  while ( ((fecSR0 & FEC_SR0_TRARUN) || (!(fecSR0 & FEC_SR0_TRAEMPTY))) && (timeout++ < TIMEOUT) ) {
    fecSR0 = fecRingDevice->getFecRingSR0() ;
  }
  
  // FIFO transmit receive running
  if (fecRingDevice->getFecRingSR0() & FEC_SR0_TRARUN) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << "FIFO transmit running bit is always ON (SR0 = " << std::hex << fecRingDevice->getFecRingSR0() << ")" << std::dec << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        

    return ;
  }

  // Timeout ?
  if (timeout >= TIMEOUT) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << "Timeout reached, unable to send the frame for the device " << msgChannel << std::endl ;
    std::cerr << "FEC SR0 = " << std::hex << fecRingDevice->getFecRingSR0() << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        

    return ;
  }

  // now, wait for the direct ack ..
  timeout = 0 ;
  fecSR0 = fecRingDevice->getFecRingSR0() ;
  
  while ( !(fecSR0 & FEC_SR0_PENDINGIRQ) && (timeout++ < TIMEOUT) ) {
    fecSR0 = fecRingDevice->getFecRingSR0() ;
  }

  // FIFO receive empty
  if (fecSR0 & FEC_SR0_RECEMPTY) {

    std::cerr << "*************** ERROR *********************" << std::endl ;   
    std::cerr << "Timeout reached on the direct acknowledge" << std::endl ;
    std::cerr << "FEC SR0 = " << std::hex << fecRingDevice->getFecRingSR0() << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        

    return ;
  }

  // Timeout ?
  if (timeout >= TIMEOUT) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << "Timeout reached, unable to receive the direct acknowledge for the device " << msgChannel << std::endl ;
    std::cerr << "FEC SR0 = " << std::hex << fecRingDevice->getFecRingSR0() << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        

    return ;
  }

  // Read back the direct aknowledge
  realSize32 = (realSize+3+1)/4 ;
  if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
  fecRingDevice->getFifoReceive(c,realSize32);

  // Convert it in 8 bits word
  // The status is just after the frame
  cpt = 0 ;
  for (tscType32 i = 0 ; i < realSize32 ; i ++) {
    
    std::cout << "c[" << i << "] = " << std::hex << c[i] << std::endl ;
    
    frameR[cpt++] = (c[i] >> 24) & 0xFF ;
    frameR[cpt++] = (c[i] >> 16) & 0xFF ;
    frameR[cpt++] = (c[i] >>  8) & 0xFF ;
    frameR[cpt++] = (c[i])       & 0xFF ;
  }

  // display the direct ack
  std::cout << "Direct acknowledge ( { " ;
  for (int i = 0 ; i < (realSize+3+1) ; i ++ )
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  std::cout << "} )" << std::dec << std::endl ;

  // here you can check the size of frame between the frame sent and the frame receive
  // You chan check also if the transaction number sent is the same than the one received

  // Analyse the status of the frame, 
  if (frameR[realSize + 3] != FECACKNOERROR32) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << "Bad direct acknowledge status = " << std::hex << frameR[realSize + 3] << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        

    return ;
  }

  // Read back the force acknowledge (set to true by default)
  timeout = 0 ;
  fecSR0 = fecRingDevice->getFecRingSR0() ;
  while ( !(fecSR0 & FEC_SR0_DATATOFEC) && (timeout++ < TIMEOUT) ) {
    fecSR0 = fecRingDevice->getFecRingSR0() ;
  }

  // FIFO receive empty
  if (fecSR0 & FEC_SR0_RECEMPTY) {

    std::cerr << "*************** ERROR *********************" << std::endl ;   
    std::cerr << "Timeout reached on the force acknowledge or read answer" << std::endl ;
    std::cerr << "FEC SR0 = " << std::hex << fecRingDevice->getFecRingSR0() << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        

    return ;
  }

  // Timeout ?
  if (timeout >= TIMEOUT) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << "Timeout reached, unable to receive the direct acknowledge for the device " << msgChannel << std::endl ;
    std::cerr << "FEC SR0 = " << std::hex << fecRingDevice->getFecRingSR0() << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        

    return ;
  }

  //clean up interrupts and unset the DATATOFEC bit in SR0
  fecRingDevice->setFecRingCR1 (FEC_CR1_CLEARIRQ);

  // Read the answer
  // Retreive the size
  c[0] = fecRingDevice->getFifoReceive() ;

  // Check the size of the frame by reading the first word in the FIFO receive
  realSize = (c[0] >> 8) & 0xFF ;
  tscType8 word2 = c[0]  & 0xFF ;
  if (realSize & FEC_LENGTH_2BYTES) {
  
    realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
    // +1 for the two byte words for length
  }

  // +3 => Dst, Src, Length  ; +1 => status after the frame
  realSize32 = (realSize+3+1)/4 ;
  if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;

  // Here you can check the size returned

  // Read the rest of the frame
  // Read back the frame
  fecRingDevice->getFifoReceive(c+1,realSize32-1);

  cpt = 0 ;
  for (tscType32 i = 0 ; i < realSize32 ; i ++) {
      
    std::cout << "c[" << std::dec << i << "] = 0x" << std::hex << c[i] << std::endl ;
    
    frame[cpt++] = (c[i] >> 24) & 0xFF ;
    frame[cpt++] = (c[i] >> 16) & 0xFF ;
    frame[cpt++] = (c[i] >>  8) & 0xFF ;
    frame[cpt++] = (c[i])       & 0xFF ;
  }

  std::cout << "Frame read ( { " ;
  for (tscType8 i = 0 ; i < (realSize+3+1) ; i ++ )
    std::cout << "0x" << std::hex << (int)frame[i] << " " ;
  std::cout << "} )" << std::dec << std::endl ;

  // Here you can check if the transaction number is the same than the transaction used

  // Here analyse for i2c commands the status A of the i2c channel set at the end of the frame
  if (frame[5] & CCUFACKERROR) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << "Bad force acknowledge status = " << std::hex << frame[5] << std::endl ;
    std::cerr << "*******************************************" << std::endl ;        

    return ;
  }

  // Display the value read
  std::cout << "Value read in the i2c device " << msgChannel << ": " << std::dec << frame[4] << std::endl ;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       Device part                                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/** Access a device
 * \param fecAccess - FEC hardware access
 * \param argc - number of argument, this parameter is needed if you specify some parameters in the command line.
 * \param argv - list of arguments, this parameter is needed if you specify some parameters in the command line.
 * \example For argv and argc, you must specify ./I2CTemplace.exe -vme -fec 0 -ring 0 -ccu 0x7E -i2cchannel 0x10, so argc will contains 8 and argv[0] will start with "-fec"
 */
void devicePart ( FecAccess &fecAccess, int argc, char *argv[] ) {

  // Channel index
  keyType indexChannel = 0 ;

  // ---------------------------------------------------------------------------------------------------
  // Get the channels where the device are, if no arguments are given
  if (argc == 0) {
    indexChannel = getAccessKey ( ) ;
  } else {
    // parse the parameters
    indexChannel = parseArgv ( argc, argv ) ;
  }

  // All the access are done through description of each device or you can set each register independtly

  // ---------------------------------------------------------------------------------------------------
  // The next commands will access the APV in read/write
  keyType indexApv = indexChannel | setAddressKey ( 0x20 ) ;
  char msgApv[80] ; decodeKey (msgApv,indexApv) ; // use for display
  apvAccess *apv = NULL ;
  try {
    apv = new apvAccess ( &fecAccess, indexApv ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot create an access to the APV " << msgApv << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;

    return ;
  }

  // Create an APV description, please check the file apvDescription for the order of the parameters
  apvDescription apvD ((tscType8)0x2b,
		       (tscType8)0x64,
		       (tscType8)0x4,
		       (tscType8)0x73,
		       (tscType8)0x3c,
		       (tscType8)0x32,
		       (tscType8)0x32,
		       (tscType8)0x32,
		       (tscType8)0x50,
		       (tscType8)0x32,
		       (tscType8)0x50,
		       (tscType8)0,    // Ispare
		       (tscType8)0x43,
		       (tscType8)0x43,
		       (tscType8)0x14,
		       (tscType8)0xFB,
		       (tscType8)0xFE,
		       (tscType8)0) ;

  // Download the APV description
  try {
    apv->setValues ( apvD );
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the values to the APV " << msgApv << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload an APV description
  apvDescription *monApv = NULL ;
  try {
    monApv = apv->getValues() ;
    // display the values
    monApv->display() ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the values to the APV " << msgApv << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (monApv != NULL) {
    if (*monApv == apvD) { 
      std::cout << "Comparison for the APV " << msgApv << " ok" << std::endl ;
    }
    else {
      std::cout << "Error in comparison for the APV " << msgApv << std::endl ;
      monApv->displayDifferences ( apvD ) ;
    }

    // delete the APV description return during the upload
    delete monApv ;
  }

  // Download the apv mode
  try {
    apv->setApvMode ( apvD.getApvMode () ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the mode to the APV " << msgApv << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload the apv mode
  tscType8 mode = 0 ;
  try {
    mode = apv->getApvMode () ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the mode to the APV " << msgApv << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare the apv mode
  if (mode == apvD.getApvMode ()) {
    std::cout << "Comparison for the mode of the APV " << msgApv << " ok" << std::endl ;
  }
  else {
    std::cout << "Comparison for the mode of the APV " << msgApv << " is not ok" << std::endl ;
  }

  // Following methods exists for the APV to set register one by one
  try {
    apv->setApvMode ( apvD.getApvMode () ) ;
    apv->setLatency ( apvD.getLatency () ) ;
    apv->setMuxGain ( apvD.getMuxGain () ) ;
    apv->setIpre    ( apvD.getIpre    () ) ;
    apv->setIpcasc  ( apvD.getIpcasc  () ) ;
    apv->setIpsf    ( apvD.getIpsf    () ) ;
    apv->setIsha    ( apvD.getIsha    () ) ;
    apv->setIssf    ( apvD.getIssf    () ) ;
    apv->setIpsp    ( apvD.getIpsp    () ) ;
    apv->setImuxin  ( apvD.getImuxin  () ) ;
    apv->setIcal    ( apvD.getIcal    () ) ;
    apv->setIspare  ( apvD.getIspare  () ) ;
    apv->setVfp     ( apvD.getVfp     () ) ;
    apv->setVfs     ( apvD.getVfs     () ) ;
    apv->setVpsp    ( apvD.getVpsp    () ) ;
    apv->setCdrv    ( apvD.getCdrv    () ) ;
    apv->setCsel    ( apvD.getCsel    () ) ;
    
    apv->getApvMode ( ) ;
    apv->getLatency ( ) ;
    apv->getMuxGain ( ) ;
    apv->getIpre    ( ) ;
    apv->getIpcasc  ( ) ;
    apv->getIpsf    ( ) ;
    apv->getIsha    ( ) ;
    apv->getIssf    ( ) ;
    apv->getIpsp    ( ) ;
    apv->getImuxin  ( ) ;
    apv->getIcal    ( ) ;
    apv->getIspare  ( ) ;
    apv->getVfp     ( ) ;
    apv->getVfs     ( ) ;
    apv->getVpsp    ( ) ;
    apv->getCdrv    ( ) ;
    apv->getCsel    ( ) ;
    apv->getApvError( ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the settings of the APV " << msgApv << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // destroy the access to the APV
  try {
    delete apv ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the delete of the APV " << msgApv << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // ---------------------------------------------------------------------------------------------------
  // The next commands will access the APV MUX in read/write
  keyType indexMux = indexChannel | setAddressKey ( 0x43 ) ;
  char msgMux[80] ; decodeKey(msgMux,indexMux) ; // use for display
  muxAccess *mux = NULL ;
  try {
    mux = new muxAccess ( &fecAccess, indexMux ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot create an access to the MUX " << msgMux << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
    
    return ;
  }

  // Create a MUX description, please check the file muxDescription for the order of the parameters
  muxDescription muxD ((tscType16)0xFF) ;

  // Download the MUX description
  try {
    mux->setValues ( muxD );
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the values to the MUX " << msgMux << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload a MUX description
  muxDescription *monMux = NULL ;
  try {
    monMux = mux->getValues() ;
    // display the values
    monMux->display() ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the values to the MUX " << msgMux << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (monMux != NULL) {
    if (*monMux == muxD) { 
      std::cout << "Comparison for the MUX " << msgMux << " ok" << std::endl ;
    }
    else {
      std::cout << "Error in comparison for the MUX " << msgMux << std::endl ;
      monMux->displayDifferences ( muxD ) ;
    }

    // delete the MUX description return during the upload
    delete monMux ;
  }

  // Download the MUX resistor
  try {
    mux->setResistor ( muxD.getResistor () ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the mode to the MUX " << msgMux << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload the MUX resistor
  tscType8 resistor = 0 ;
  try {
    resistor = mux->getResistor () ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the mode to the MUX " << msgMux << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (resistor == muxD.getResistor ()) {
    std::cout << "Comparison for the resistor of the MUX " << msgMux << " ok" << std::endl ;
  }
  else {
    std::cout << "Comparison for the resistor of the MUX " << msgMux << " is not ok" << std::endl ;
  }

  // destroy the access to the MUX
  try {
    delete mux ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the delete of the MUX " << msgMux << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // ---------------------------------------------------------------------------------------------------
  // The next commands will access the PLL in read/write
  keyType indexPll = indexChannel | setAddressKey ( 0x44 ) ;
  char msgPll[80] ; decodeKey(msgPll,indexPll) ; // use for display
  pllAccess *pll = NULL ;
  try {
    pll = new pllAccess ( &fecAccess, indexPll ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot create an access to the PLL " << msgPll << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
    
    return ;
  }

  // Check if the PLL is locked
  try {
    pll->setPllCheckGoingBit() ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot reset the PLL " << msgPll << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;

    delete pll ;

    return ;
  }

  // Create a PLL description, please check the file pllDescription for the order of the parameters
  // delay fine / clock phase, delay coarse / trigger delay
  pllDescription pllD ((tscType8)6,(tscType8)1) ;

  // Download the PLL description
  try {
    pll->setValues ( pllD );
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the values to the PLL " << msgPll << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload a PLL description
  pllDescription *monPll = NULL ;
  try {
    monPll = pll->getValues() ;
    // display the values
    monPll->display() ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the values to the PLL " << msgPll << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (monPll != NULL) {
    if (*monPll == pllD) { 
      std::cout << "Comparison for the PLL " << msgPll << " ok" << std::endl ;
    }
    else {
      std::cout << "Error in comparison for the PLL " << msgPll << std::endl ;
      monPll->displayDifferences ( pllD ) ;
    }

    // delete the PLL description return during the upload
    delete monPll ;
  }

  // Download the PLL clock phase / delay fine
  try {
    pll->setClockPhase ( pllD.getClockPhase() ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the mode to the PLL " << msgPll << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload the PLL clock phase / delay fine
  tscType8 clockPhase = 0 ;
  try {
    clockPhase = pll->getClockPhase( ) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the mode to the PLL " << msgPll << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (clockPhase == pllD.getClockPhase ()) {
    std::cout << "Comparison for the clock phase of the PLL " << msgPll << " ok" << std::endl ;
  }
  else {
    std::cout << "Comparison for the clock phase of the PLL " << msgPll << " is not ok" << std::endl ;
  }

  // Following methods exists for the PLL to set register one by one
  try {
    pll->setClockPhase ( pllD.getClockPhase() ) ;
    pll->setTriggerDelay ( pllD.getTriggerDelay () ) ;

    pll->getClockPhase( ) ;
    pll->getTriggerDelay( ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the settings of the PLL " << msgPll << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // destroy the access to the PLL
  try {
    delete pll ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the delete of the PLL " << msgPll << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // ---------------------------------------------------------------------------------------------------
  // The next commands will access the AOH/laserdriver in read/write
  keyType indexLaserdriver = indexChannel | setAddressKey ( 0x60 ) ;
  char msgLaserdriver[80] ; decodeKey(msgLaserdriver,indexLaserdriver) ; // use for display
  laserdriverAccess *laserdriver = NULL ;
  try {
    laserdriver = new laserdriverAccess ( &fecAccess, indexLaserdriver ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot create an access to the LASERDRIVER " << msgLaserdriver << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
    
    return ;
  }

  // Create a LASERDRIVER description, please check the file laserdriverDescription for the order of the parameters
  // Init the settings value for the laserdriver
  tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    biasInit[i] = 18 ;
    gainInit[i] = 2  ;
  }
  laserdriverDescription laserdriverD (gainInit,biasInit) ;
	
  // Download the LASERDRIVER description
  try {
    laserdriver->setValues ( laserdriverD );
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the values to the LASERDRIVER " << msgLaserdriver << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload a LASERDRIVER description
  laserdriverDescription *monLaserdriver = NULL ;
  try {
    monLaserdriver = laserdriver->getValues() ;
    // display the values
    monLaserdriver->display() ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the values to the LASERDRIVER " << msgLaserdriver << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (monLaserdriver != NULL) {
    if (*monLaserdriver == laserdriverD) { 
      std::cout << "Comparison for the LASERDRIVER " << msgLaserdriver << " ok" << std::endl ;
    }
    else {
      std::cout << "Error in comparison for the LASERDRIVER " << msgLaserdriver << std::endl ;
      monLaserdriver->displayDifferences ( laserdriverD ) ;
    }

    // delete the LASERDRIVER description return during the upload
    delete monLaserdriver ;
  }

  // Download the LASERDRIVER gain
  try {
    laserdriver->setGain ( laserdriverD.getGain ( ) ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the mode to the LASERDRIVER " << msgLaserdriver << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload the LASERDRIVER gain
  tscType8 gain = 0 ;
  try {
    gain = laserdriver->getGain () ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the mode to the LASERDRIVER " << msgLaserdriver << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (gain == laserdriverD.getGain ()) {
    std::cout << "Comparison for the clock phase of the LASERDRIVER " << msgLaserdriver << " ok" << std::endl ;
  }
  else {
    std::cout << "Comparison for the clock phase of the LASERDRIVER " << msgLaserdriver << " is not ok" << std::endl ;
  }

  // Following methods exists for the LASERDRIVER to set register one by one
  try {
    tscType8 bias[MAXLASERDRIVERCHANNELS] ;
    laserdriverD.getBias ( bias ) ;

    laserdriver->setBias ( bias ) ;
    laserdriver->setGain ( laserdriverD.getGain ( ) ) ;

    laserdriver->getBias (bias) ;
    laserdriver->getGain ( ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the settings of the LASERDRIVER " << msgLaserdriver << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // destroy the access to the LASERDRIVER
  try {
    delete laserdriver ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the delete of the LASERDRIVER " << msgLaserdriver << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // ---------------------------------------------------------------------------------------------------
  // The next commands will access the DOH in read/write
  keyType indexDoh = indexChannel | setAddressKey ( 0x70 ) ;
  char msgDoh[80] ; decodeKey(msgDoh,indexDoh) ; // use for display
  DohAccess *doh = NULL ;
  try {
    doh = new DohAccess ( &fecAccess, indexDoh ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot create an access to the DOH " << msgDoh << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
    
    return ;
  }

  // Create a LASERDRIVER description, please check the file laserdriverDescription for the order of the parameters
  // Init the settings value for the laserdriver
  tscType8 biasInitD[MAXLASERDRIVERCHANNELS], gainInitD[MAXLASERDRIVERCHANNELS] ;
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    biasInitD[i] = 18 ;
    gainInitD[i] = 2  ;
  }
  laserdriverDescription dohD (gainInitD,biasInitD) ;
	
  // Download the LASERDRIVER description
  try {
    doh->setValues ( dohD );
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the values to the DOH " << msgDoh << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload a DOH description
  laserdriverDescription *monDoh = NULL ;
  try {
    monDoh = doh->getValues() ;
    // display the values
    monDoh->display() ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the values to the DOH " << msgDoh << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (monDoh != NULL) {
    if (*monDoh == dohD) { 
      std::cout << "Comparison for the DOH " << msgDoh << " ok" << std::endl ;
    }
    else {
      std::cout << "Error in comparison for the DOH " << msgDoh << std::endl ;
      monDoh->displayDifferences ( dohD ) ;
    }

    // delete the DOH description return during the upload
    delete monDoh ;
  }

  // Download the DOH gain
  try {
    doh->setGain ( dohD.getGain ( ) ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot download the mode to the DOH " << msgDoh << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Upload the DOH gain
  tscType8 gainD = 0 ;
  try {
    gainD = doh->getGain () ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the mode to the DOH " << msgDoh << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Compare it
  if (gainD == dohD.getGain ()) {
    std::cout << "Comparison for the clock phase of the DOH " << msgDoh << " ok" << std::endl ;
  }
  else {
    std::cout << "Comparison for the clock phase of the DOH " << msgDoh << " is not ok" << std::endl ;
  }

  // Following methods exists for the DOH to set register one by one
  try {
    tscType8 bias[MAXLASERDRIVERCHANNELS] ;
    dohD.getBias ( bias ) ;

    doh->setBias ( bias ) ;
    doh->setGain ( dohD.getGain ( ) ) ;

    doh->getBias (bias) ;
    doh->getGain ( ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the settings of the DOH " << msgDoh << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // destroy the access to the DOH
  try {
    delete doh ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the delete of the DOH " << msgDoh << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // ---------------------------------------------------------------------------------------------------
  // The next commands will access the DCU in read
  keyType indexDcu = indexChannel | setAddressKey ( 0x0 ) ;
  char msgDcu[80] ; decodeKey(msgDcu,indexDcu) ; // use for display
  dcuAccess *dcu = NULL ;
  try {
    dcu = new dcuAccess ( &fecAccess, indexDcu ) ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot create an access to the DCU " << msgDcu << std::endl ;
    std::cerr << "Exiting ........" << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
    
    return ;
  }

  // Upload a DCU description
  dcuDescription *monDcu = NULL ;
  try {
    monDcu = dcu->getValues() ;
    // display the values
    monDcu->display() ;

    // delete the description uploaded
    delete monDcu ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Cannot upload the values to the DCU " << msgDcu << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // Following methods exists for the PLL to set register one by one
  try {
    dcu->getDcuHardId () ;
    bool setTreg = true ;
    for (tscType8 channel = 0 ; channel < 8 ; channel ++) {
      dcu->getDcuChannel(channel,setTreg) ;
      setTreg = false ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the settings of the DCU " << msgDcu << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }

  // destroy the access to the DCU
  try {
    delete dcu ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*************** ERROR *********************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "Error during the delete of the DCU " << msgDcu << std::endl ;
    std::cerr << "*******************************************" << std::endl ;
  }
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       main functions                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** Main function, you must specify at the starting which kind of FEC you are using and with kind of VME bus you using
 */
int main ( int argc, char *argv[] ) {

  // Create the hardware access
  FecAccess *createFecAccess ( int argc, char **argv, int *cnt ) ;

  // Hardware access
  FecAccess *fecAccess = NULL ;

  // Check the parameter
  if (argc < 2) {

    std::cerr << argv[0] << ": error in usage, please specify bus type " << std::endl ;
    std::cerr << argv[0] << " [-pci | -vmesbs | -vmecaenpci | -vmecaenusb | -usb]" << std::endl ;
    return -1 ;
  }

  // Create the FEC Access
  int cnt ;
  try {
    fecAccess = createFecAccess ( argc, argv, &cnt ) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
    exit (-1) ;
  }

  // Main Part, please call the relevant function
  int choice = 0 ;
  do {
    std::cout << "Which method you want to use" << std::endl ;
    std::cout << "  0: Quit" << std::endl ;
    std::cout << "  1: Device method" << std::endl ;
    std::cout << "  2: i2c method" << std::endl ;
    std::cout << "  3: frame method" << std::endl ;
    std::cout << "     Choice: " ;
    std::cin >> choice ;
    switch (choice) {
    case 0: 
      std::cout << "Exiting" << std::endl ;
      break ;
    case 1: 
      devicePart ( *fecAccess, argc-cnt, &argv[cnt] ) ;
      break ;
    case 2: 
      i2cPart ( *fecAccess, argc-cnt, &argv[cnt] ) ;
      break ;
    case 3: 
      framePart ( *fecAccess, argc-cnt, &argv[cnt] ) ;
      break ;
    default:
      std::cerr << "Invalid choice" << std::endl ;
    }
  } while (choice != 0) ;

  try {
    delete fecAccess ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "------------ Exception ----------" << std::endl ;
    std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
    std::cerr << "---------------------------------" << std::endl ;
  }
  
  return 0 ;
}

/** Convert the arguments given in the command line into a key
 * possible arguments are:
 * <ul>
 * <li>-fec
 * <li>-ring
 * <li>-ccu
 * <li>-channel
 * <li>-i2caddress
 * </ul>
 */
keyType parseArgv ( int argc, char *argv[] ) {

  int fecSlot = 0, ringSlot = 0, ccuAddress = 0, ccuChannel = 0, i2cAddress = 0 ;

  for (int i = 0 ; i < argc ; i ++) {

    if (strncmp (argv[i], "-fec", 4) == 0) {

      i ++ ;
      if ((i == argc) || (!sscanf (argv[i], "%d", &fecSlot))) {
	std::cerr << "Invalid argument for the FEC slot" << std::endl ;
      }
    }
    else if (strncmp (argv[i], "-ring", 5) == 0) { 

      i ++ ;
      if ((i == argc) || (!sscanf (argv[i], "%d", &ringSlot))) {
	std::cerr << "Invalid argument for the ring slot" << std::endl ;
      }
    }
    else if (strncmp (argv[i], "-ccu", 4) == 0) {

      i ++ ;
      if ((i == argc) || (!sscanf (argv[i], "%d", &ccuAddress))) {
	std::cerr << "Invalid argument for the CCU address" << std::endl ;
      }
    }
    else if (strncmp (argv[i], "-channel", 8) == 0) {

      i ++ ;
      if ((i == argc) || (!sscanf (argv[i], "%d", &ccuChannel))) {
	std::cerr << "Invalid argument for the CCU channel" << std::endl ;
      }
    }
    else if (strncmp (argv[i], "-i2cAddress", 12) == 0) {

      i ++ ;
      if ((i == argc) || (!sscanf (argv[i], "%d", &i2cAddress))) {
	std::cerr << "Invalid argument for the CCU channel" << std::endl ;
      }
    }
  }

  keyType index = buildCompleteKey(fecSlot, ringSlot, ccuAddress, ccuChannel, i2cAddress) ;
  char msg[80] ;
  decodeKey(msg,index) ;
  std::cout << "Make an access to " << msg << std::endl ;
  
  return (index) ;
}

/** Ask for a FEC, ring, CCU, channel and i2c address and return the associate key
 */
keyType getAccessKey ( ) {

  int fecSlot, ringSlot, ccuAddress = 0, ccuChannel = 0, i2cAddress = 0 ;

  std::cout << "Please enter the FEC slot: " ;
  std::cin >> fecSlot ;
  std::cout << "Please enter the ring slot: " ;
  std::cin >> ringSlot ;
  std::cout << "Please enter the CCU address: " ;
  std::cin >> ccuAddress ;
  std::cout << "Please enter the CCU channel: " ;
  std::cin >> ccuChannel ;
  std::cout << "Please enter the I2C address: " ;
  std::cin >> i2cAddress ;

  return (buildCompleteKey(fecSlot, ringSlot, ccuAddress, ccuChannel, i2cAddress)) ;
}

/** Build for a FEC, ring, CCU, channel and i2c address and return the associate key
 */
keyType getAccessKey ( int fecSlot, int ringSlot, int ccuAddress, int ccuChannel, int i2cAddress ) {

  return (buildCompleteKey(fecSlot, ringSlot, ccuAddress, ccuChannel, i2cAddress)) ;
}


/** Create the FEC Access class depending on the version of the FecSoftware and the FEC type
 */
FecAccess *createFecAccess ( int argc, char **argv, int *cnt ) {

  FecAccess *fecAccess = NULL ;

  int cpt = 1 ;

  // FEC type
  enumFecBusType fecBusType = FECVME ;
  enumBusAdapter busAdapter = SBS    ;

  // VME configuration
#define MAXCARS 1000
  char vmeFileName[MAXCARS] = "" ;

  if (argc >= 2) {
    if (strcasecmp (argv[cpt],"-pci") == 0) { // If a new fecAddress has been set
      
      fecBusType = FECPCI ;
      cpt ++ ;
    } 
    else if ( (strcasecmp (argv[cpt],"-vmesbs") == 0) || (strcasecmp (argv[cpt],"-vme") == 0)) { // If a new fecAddress has been set

      cpt ++ ;
      fecBusType = FECVME ;
      busAdapter = SBS    ;
    }
    else if (strcasecmp (argv[cpt],"-vmecaenpci") == 0) { // If a new fecAddress has been set

      cpt ++ ;
      fecBusType = FECVME ;
      busAdapter = CAENPCI    ;
    }
    else if (strcasecmp (argv[cpt],"-vmecaenusb") == 0) { // If a new fecAddress has been set

      cpt ++ ;
      fecBusType = FECVME  ;
      busAdapter = CAENUSB ;
    }
    else if (strcasecmp (argv[cpt],"-usb") == 0) { // If a new fecAddress has been set
      
      fecBusType = FECUSB ;
      cpt ++ ;
    } 
  }

  if (fecBusType == FECVME) {
    // Check if a filename is given
    if ( (cpt < argc) && (argv[cpt][0] != '-') ) {
      strncpy (vmeFileName, argv[cpt], MAXCARS) ;
      cpt++ ;
      
      std::ifstream fichier (vmeFileName) ;
      if (fichier) {
	std::cout << "VME FEC will be used with the file " << vmeFileName << std::endl ;
      }
      else {
	std::cerr << "The file " << vmeFileName << " does not exists" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else {

      char *basic = NULL ;

      // $FECSOFTWARE_ROOT/config/FecAddressTable.dat
      if (basic == NULL) {
	basic=getenv ("FECSOFTWARE_ROOT") ;
	if (basic != NULL) {
	  strncpy (vmeFileName, basic, MAXCARS) ;
	  strncat (vmeFileName, "/config/FecAddressTable.dat", MAXCARS) ;
	  std::ifstream fichier (vmeFileName) ;
	  if (! fichier) basic = NULL ;
	}
      }

      // $HOME/FecSoftwareV3_0/config/FecAddressTable.dat
      if (basic == NULL) {
	basic = getenv ("HOME") ;
	strncpy (vmeFileName, basic, MAXCARS) ;
	strncat (vmeFileName, "/FecSoftwareV3_0/config/FecAddressTable.dat", MAXCARS) ;
	std::ifstream fichier (vmeFileName) ;
	if (! fichier) basic = NULL ;
      }

      // $HOME/FecSoftwareV2_0/config/FecAddressTable.dat
      if (basic == NULL) {
	basic = getenv ("HOME") ;
	strncpy (vmeFileName, basic, MAXCARS) ;
	strncat (vmeFileName, "/FecSoftwareV2_0/config/FecAddressTable.dat", MAXCARS) ;
	std::ifstream fichier (vmeFileName) ;
	if (! fichier) basic = NULL ;
      }

      // $FECSOFTWARE_ROOT/../FecSoftwareV2_0/config/FecAddressTable.dat
      if (basic == NULL) {
	basic=getenv ("FECSOFTWARE_ROOT") ;
	if (basic != NULL) {
	  strncpy (vmeFileName, basic, MAXCARS) ;
	  strncat (vmeFileName, "/../FecSoftwareV2_0/config/FecAddressTable.dat", MAXCARS) ;
	  std::ifstream fichier (vmeFileName) ;
	  if (! fichier) basic = NULL ;
	}
      }

      // $ENV_CMS_TK_FEC_ROOT/config/FecAddressTable.dat
      if (basic == NULL) {
	basic=getenv ("ENV_CMS_TK_FEC_ROOT") ;
	if (basic != NULL) {
	  strncpy (vmeFileName, basic, MAXCARS) ;
	  strncat (vmeFileName, "/config/FecAddressTable.dat", MAXCARS) ;
	  std::ifstream fichier (vmeFileName) ;
	  if (! fichier) basic = NULL ;
	}
      }

      if (basic) {
	std::cout << "VME FEC will be used with the file " << vmeFileName << std::endl ;
      }
      else {
	std::cerr << "No file found for VME" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
  } 
  
  // Create the FEC Access
  switch ( fecBusType ) {
  case FECPCI:
    // bool forceAck, bool initFec, bool scan
    fecAccess = new FecAccess (true, false, true, false, 100) ;
    break ;
  case FECVME:
    // ulong adapterSlot, string std::configurationFile, 
    // bool forceAck, bool init, bool scan
    fecAccess = new FecAccess (0, vmeFileName, FecVmeRingDevice::VMEFECBASEADDRESSES, true, false, true, false, 100, FecVmeRingDevice::STR_BUSADAPTER_NAME[busAdapter]) ;
    break ;
  case FECUSB:
    // FEC usb
    fecAccess = new FecAccess (0,true, false, true, false, 100) ;
    break ;
  }

  if (!fecAccess) {
     std::cout << "Creation of FecAccess failed. fecAccess pointer null." << std::endl ; 
     exit (EXIT_FAILURE) ; ; 
  }

  std::list<keyType> *fecList = fecAccess->getFecList() ;

  if ((fecList == NULL) || (fecList->empty())) {
    std::cerr << "Warning: No FEC rings board found" << std::endl ;
  }

  delete fecList;

  *cnt = cpt ;

  return (fecAccess) ;
}

