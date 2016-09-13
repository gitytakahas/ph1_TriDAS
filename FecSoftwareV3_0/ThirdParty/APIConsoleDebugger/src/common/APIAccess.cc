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

#include <stdio.h>    // fopen snprintf
#include <string.h>   // strcmp
#include <unistd.h>   // usleep
#include <sys/time.h> // time

// For socket
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
#include "FecVmeRingDevice.h"
#endif
#if defined(BUSPCIFEC)
#include "FecPciRingDevice.h"
#endif
#if defined(BUSUSBFEC)
#include "FecUsbRingDevice.h"
#endif

#include "HashTable.h"
#include "APIAccess.h"
#include "FecRingRegisters.h"

#define MAXERRORCOUNT 1
#define MAXCAR (DD_USER_MAX_MSG_LENGTH * 2 + 2)

// Default firmware version
#define MFECFIRMWARE 0x17
#define VMEFIRMWARE 0x16
#define TRIGGERFIRMWARE 0x14

// To display getchar or not
static bool noGetchar_ = false ;
void setNoGetchar( bool val ) {
  noGetchar_ = val ;
}

bool getNoGetchar ( ) {
  return noGetchar_ ;
}

char fileSuperName_[100] ;

void setSuperFileName ( char *fileName ) {

  strncpy (fileSuperName_,fileName,100) ;
}

// Global variable for displaying error
static bool errorCounterFlag_ = false ;
void setErrorCounterFlag ( bool val ) {

  errorCounterFlag_ = true ;
}

bool getErrorCounterFlag ( ) {

  return (errorCounterFlag_) ;
}

// Output file
static FILE *stdchan_ = stderr ;

void setStdchan ( FILE *chan ) {
  stdchan_ = chan ;
}

FILE *getStdchan ( ) {

  return (stdchan_) ;
}


inline std::string IntToString(int t, int base = 16)
{
   std::ostringstream oss;
   if(base == 10) oss << t;
   else oss << std::hex << t;
   return oss.str();
}

// range for FEC and ring default = PCI
static unsigned int minFecSlot_ = 0 ;
static unsigned int maxFecSlot_ = 4 ;
static unsigned int minFecRing_ = 0 ;
static unsigned int maxFecRing_ = 0 ;

static enumFecBusType fecBusType_ ;
void setFecType ( enumFecBusType fecBusType ) {

  fecBusType_ = fecBusType ;
  switch (fecBusType) {
  case FECVME:
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
    minFecSlot_ = FecVmeRingDevice::MINVMEFECSLOT ;
    maxFecSlot_ = FecVmeRingDevice::MAXVMEFECSLOT ;
    minFecRing_ = FecVmeRingDevice::getMinVmeFecRingValue() ;
    maxFecRing_ = FecVmeRingDevice::getMaxVmeFecRingValue() ;
#endif
    break ;
  case FECPCI:
#if defined(BUSPCIFEC)
    minFecSlot_ = FecPciRingDevice::MINPCIFECSLOT ;
    maxFecSlot_ = FecPciRingDevice::MAXPCIFECSLOT ;
    minFecRing_ = FecPciRingDevice::MINPCIFECRING ;
    maxFecRing_ = FecPciRingDevice::MAXPCIFECRING ;
#endif
    break ;
  case FECUSB:
#if defined(BUSUSBFEC)
    minFecSlot_ = FecUsbRingDevice::minUsbFecSlot ;
    maxFecSlot_ = FecUsbRingDevice::maxUsbFecSlot ;
    minFecRing_ = FecUsbRingDevice::minUsbFecRing ;
    maxFecRing_ = FecUsbRingDevice::maxUsbFecRing ;
#endif
    break;
  }
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       SSID                                              */
/*                                                                         */
/* ----------------------------------------------------------------------- */
void setSSID ( FecAccess *fecAccess, std::string ssidSystem, bool display, bool resetCCSTTCRx ) {

  int alreadySet[100] = {0} ;

  // For each FEC board, retreive the CCSTrigger and set the ssidSystem
  std::list<keyType> *fecSlotList = fecAccess->getFecList( ) ;
    
  if ((fecSlotList != NULL) && (!fecSlotList->empty())) {
    for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
      keyType index = *p ;

      if (!alreadySet[getFecKey(index)]) {
	//std::cout << "Set the SSID for the FEC " << (int)getFecKey(index) << std::endl;
	alreadySet[getFecKey(index)] = 1 ;

	try {
	  CCSTrigger *ccsTrigger = fecAccess->setCCSTrigger ( index, ssidSystem ) ;

	  if (display) std::cout << "SSID " << ssidSystem << " set for the FEC " << (int)getFecKey(index) << std::endl;

          // toggle the bit 7 for TTCRx reset 
          if (resetCCSTTCRx) {
	    ccsTrigger->setResetTTCRx() ; 
	  }

	  if (ccsTrigger->getSSID() != ssidSystem) {
	    if (display) std::cout << "SSID hardware is different than the one set in the hardware: " << ssidSystem << " / " << ccsTrigger->getSSID() << std::endl ;
	  }
	  else 
	    if (display) std::cout << "SSID read from hardware = " << ccsTrigger->getSSID() << std::endl ; 
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "************************ ERROR *************************************" << std::endl ;
	  std::cerr << "Unable to set the SSID " << ssidSystem << " for the FEC " << getFecKey(index) << std::endl ;
	  std::cerr << e.what() << std::endl ;
	  std::cerr << "********************************************************************" << std::endl ;
	}
      }
    }

    delete fecSlotList ;
  }
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       Read file function                                */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** Retreive a frame from a string and return the first char.
 * The command must be 'c Frame'. 
 */
char string2Frame ( char *strOrig, tscType16 *frame, int *nbVal ) {
  
  // command
  char cmd = strOrig[0] ;
  int nb = 0 ;

  // Translate the rest of the command in a frame
  // strtok change the string => copy it
  char strFr[MAXCAR] ;
  snprintf (strFr, MAXCAR, "%s", &strOrig[2]) ;
  char *strFrTemp = strFr ;
  
  // tokenize it
  char *str = strtok (strFrTemp, " ") ;

  // Retreive the frame
  while ( (str != NULL) && (nb < DD_USER_MAX_MSG_LENGTH) ) {

    unsigned int val ;
    if (sscanf (str, "%x", &val)) {
      frame[nb ++] = (tscType16) val ;

      // Next token
      str = strtok (NULL, " ") ;
    }
    else {
      std::cout << "Incoherent value in string " << str << std::endl ;
      str = NULL ;
      nb = -1 ;
    }
  }

  if (nb == DD_USER_MAX_MSG_LENGTH) {
    std::cout << "The frame given is too big cannot send it over the ring" << std::endl ;
    nb = -1 ;
  }

  *nbVal = nb ;
  return (cmd) ;
}

/** Open a server socket and wait for any command
 * <p>request     answer            comment
 * <p>s           SR0 SR1 CR0 CR1   status
 * <p>r frame     Flag Direct Ack   send a frame over the ring
 * <p>w frame     Flag Direct Ack   send a frame over the ring
 * <p>z           Flag              reset
 * Flag=0 for nok, 1 for ok
 * \param port - port for the socket
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 */
int socketCommand ( FecAccess *fecAccess, 
		    int port,
		    tscType8 fecAddress,
		    tscType8 ringAddress ) {

#define NOK 1
#define OK  0

  int socketListen=socket(PF_INET, SOCK_DGRAM,0);
  if (socketListen < 0) {

    perror ("socket: cannot open the socket") ;
    return -1 ;
  }
  struct sockaddr_in sockAddr ;
  sockAddr.sin_family=AF_INET;
  sockAddr.sin_port = htons(1300);
  sockAddr.sin_addr.s_addr =htonl(INADDR_ANY);
  if (bind(socketListen,(struct sockaddr *) &sockAddr,sizeof(sockAddr)) < 0) {

    perror ("bind: cannot bind the socket") ;
    return -1 ;
  }
  int soSize=sizeof(sockAddr);

  FecRingDevice *fecDevice = NULL ;
  try {

    fecDevice = fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
    fecDevice->fecHardReset() ;
    // Disable PLX interrupts
    fecDevice->setIRQ (false) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << "Did not find the FEC" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    return -1 ;
  }

  int msgSize ;
  char cans[MAXCAR], cmd[MAXCAR];
  do {
    
    memset(cmd,0,MAXCAR);
    msgSize=recvfrom(socketListen,cmd,MAXCAR,0,(struct sockaddr *)&sockAddr,(socklen_t *)&soSize);
    
    // Is it a command
    if (strncasecmp (cmd, "END", 3) != 0) {
      
      // Retreive the command
      std::cout << cmd << std::endl ;
      snprintf (cans, MAXCAR, " %d ", NOK) ;
      
      // The frame to be sent
      tscType16 frame[DD_USER_MAX_MSG_LENGTH], value ;
      int nb = 0, val ;
      DD_TYPE_ERROR lcl_err = 0 ;

      // For time measurements
      struct timeval time1, time2;
      struct timezone zone1, zone2;
      double timesec ;

      // Command ?
      char choix = string2Frame ( cmd, frame, &nb ) ;
      
      if (nb >= 0) {
	  
        // Declaration
        tscType16 nbVal = (tscType16)nb ;
        tscType16 realSize ;
        
        // Choice
        switch (choix) {
        case 'a': // Select the input B for the FEC
          std::cout << "Select the input B for the FEC" << std::endl ;
          std::cout << "The answer can be:" << std::endl ;
          std::cout << "\tSelect input B, CR0 = Value CR1= Value" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            //fecDevice->setFecRegister ( DD_FEC_CTRL0_OFFSET, DD_FEC_SEL_SER_IN, CMD_OR ) ;
	    fecDevice->setFecRingCR0 (DD_FEC_SEL_SER_IN, CMD_OR) ;
            snprintf ( cans, MAXCAR, "Select input B, CR0 = %x CR1= %x", 
                       fecDevice->getFecRingCR0(), fecDevice->getFecRingCR1()) ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 'b': // Select the output B for the FEC
	  std::cout << "Select the output B for the FEC" << std::endl ;
          try {
            //fecDevice->setFecRegister ( DD_FEC_CTRL0_OFFSET, DD_FEC_SEL_SER_OUT, CMD_OR ) ;
	    fecDevice->setFecRingCR0 (DD_FEC_SEL_SER_OUT, CMD_OR) ;
            snprintf ( cans, MAXCAR, "Select output B, CR0 = %x CR1 = %x", 
                       fecDevice->getFecRingCR0(), fecDevice->getFecRingCR1()) ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 'c': // Output to A for the FEC
          std::cout << "Output to A for the FEC" << std::endl ;
          std::cout << "The answer can be:" << std::endl ;
          std::cout << "\tSelect input B, CR0 = Value CR1= Value" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            //fecDevice->setFecRegister ( DD_FEC_CTRL0_OFFSET, DD_FEC_SEL_SER_OUT, CMD_XOR ) ;
	    fecDevice->setFecRingCR0 ( DD_FEC_SEL_SER_OUT, CMD_XOR ) ;
            snprintf ( cans, MAXCAR, "Select input A, CR0 = %x CR1 = %x", 
                       fecDevice->getFecRingCR0(), fecDevice->getFecRingCR1()) ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 'd': // Input to A for the FEC
          std::cout << "Input A for the FEC" << std::endl ;
          std::cout << "The answer can be:" << std::endl ;
          std::cout << "\tSelect input B, CR0 = Value CR1= Value" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            //fecDevice->setFecRegister ( DD_FEC_CTRL0_OFFSET, DD_FEC_SEL_SER_IN, CMD_XOR ) ;
	    fecDevice->setFecRingCR0 ( DD_FEC_SEL_SER_IN, CMD_XOR ) ;
            snprintf ( cans, MAXCAR, "Select input A, CR0 = %x CR1 = %x", 
                       fecDevice->getFecRingCR0(), fecDevice->getFecRingCR1()) ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 'f': // Read FIFO receive
	  std::cout << "Read FIFO receive" << std::endl ;
	  std::cout << "The answer can be:" << std::endl ;
          std::cout << "\t<FIFO value>" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            value = fecDevice->getFifoReceive ( ) ;
            snprintf ( cans, MAXCAR, "%x ", value) ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 'l': // Release FEC
          std::cout << "Release FEC" << std::endl ;
	  std::cout << "The answer can be:" << std::endl ;
          std::cout << "\tFEC Release" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            fecDevice->fecRingRelease ( ) ;
            snprintf ( cans, MAXCAR, "FEC release" ) ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 'C': // ????????????????????????????
	  std::cout << "This method is not implemented" << std::endl ;
          std::cout << "No comments are given" << std::endl ;
          break ;
        case 'h': // Read 100 words in the FIFO receive
          std::cout << "Read 100 words in the FIFO receive" << std::endl ;
	  std::cout << "The answer can be:" << std::endl ;
          std::cout << "\t 0 time: ok and time is the timing to write 100 words" << std::endl ;
          std::cout << "\t 1 time: not ok and time is the timing with the errors to write the 100 words" << std::endl ;
          val = OK ;
          gettimeofday(&time1, &zone1);
          for (tscType16 i = 0 ; i < 100 ; i ++) {
            try {
              value = fecDevice->getFifoReceive ( ) ;
            }
            catch (FecExceptionHandler &e) {
              std::cout << "------------ Exception ----------" << std::endl ;
              std::cout << e.what()  << std::endl ;
              std::cout << "---------------------------------" << std::endl ;
              std::cout << "Continue ..." << std::endl ;
              val = NOK ;
            }
          }
          gettimeofday(&time2, &zone2);
          timesec = (time2.tv_sec + time2.tv_usec * 0.000001) - (time1.tv_sec + time1.tv_usec * 0.000001) ;

          // Answer
          snprintf (cans, MAXCAR, " %d %f ", val, timesec) ;
          break ;

        case 't': // Write transmit FIFO per 8 bits paquets
	  std::cout << "This method is not implemented" << std::endl ;
          std::cout << "No comments are given" << std::endl ;
          break ;
        case 'u': // Impulsion send
	  std::cout << "Impulsion send" << std::endl ;
	  std::cout << "The answer can be:" << std::endl ;
          std::cout << "\t<SR0 value> <SR1 value> <CR0 value> <CR1 value>" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            value = fecDevice->getFecRingCR0() ;
            fecDevice->setFecRingCR0( value | DD_FEC_ENABLE_FEC ) ;
            fecDevice->setFecRingCR0( value | DD_FEC_ENABLE_FEC | DD_FEC_SEND ) ;
            fecDevice->setFecRingCR0( value | DD_FEC_ENABLE_FEC ) ;
            snprintf (cans, MAXCAR, " %x %x %x %x ",
                      (tscType16)fecDevice->getFecRingSR0(),
                      fecDevice->getFecRingSR1(),
                      fecDevice->getFecRingCR0(),
                      fecDevice->getFecRingCR1() ) ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 'x': // Clear error bits
          std::cout << "Clear error bits" << std::endl ;
          std::cout << "The answer can be:" << std::endl ;
          std::cout << "\tClear error bit OK" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            fecDevice->setFecRingCR1 ( DD_FEC_CLEAR_IRQ | DD_FEC_CLEAR_ERRORS ) ;
            snprintf ( cans, MAXCAR, "Clear error bit OK") ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 'y': // Reset TTCRx
          std::cout << "Reset TTCRx" << std::endl ;
          std::cout << "The answer can be:" << std::endl ;
          std::cout << "\tReset TTCRx OK" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            fecDevice->setFecRingCR0 (DD_FEC_ENABLE_FEC | DD_FEC_RESET_TTCRX ) ;
            fecDevice->setFecRingCR0 (DD_FEC_ENABLE_FEC) ;
            snprintf ( cans, MAXCAR, "Reset TTCRx OK") ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break ;
        case 's': // retreive the FEC status SR0, SR1, CR0, CR1
          std::cout << "retreive the FEC status SR0, SR1, CR0, CR1" << std::endl ;
          std::cout << "The answer can be:" << std::endl ;
          std::cout << "\t<SR0 value> <SR1 value> <CR0 value> <CR1 value>" << std::endl ;
	  std::cout << "\t " << NOK << " <Error Code>" << std::endl ;
          try {
            snprintf (cans, MAXCAR, "%d %x %x %x %x ",
                      OK,
                      (tscType16)fecDevice->getFecRingSR0(),
                      fecDevice->getFecRingSR1(),
                      fecDevice->getFecRingCR0(),
                      fecDevice->getFecRingCR1() ) ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break;
        case 'r': { // Send a read frame and read back the answer
          std::cout << "Send a read frame and read back the answer" << std::endl ;
	  std::cout << "The answer can be:" << std::endl ;
          std::cout << "\tAnswer frame" << std::endl ;
          std::cout << "\t " << NOK << " <Error Code>: error or if the CCU broadcast mode is used" << std::endl ;
          
          // Set the transaction number to 0xFE
          // convention in FecSoftware to read back a frame for the device driver
          realSize = frame[2] ;
          if (realSize & FEC_LENGTH_2BYTES) { // Size in two bytes length
            realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;
            frame[5] = 0xFE ;
          }
          else frame[4] = 0xFE ;
	  
          realSize += 3 ;
          
          std::cout << "writeFrame ( { " << std::endl ;
          for (int i = 0 ; i < realSize ; i ++ )
            std::cout << "0x" << std::hex << (int)frame[i] << std::endl ;
          std::cout << "} )" << std::endl ;

          if (frame[0] != BROADCAST_ADDRESS) {

            // Enable interrupts
            fecDevice->setIRQ (true) ;
          
            // send it
	    tscType8 frame8[DD_USER_MAX_MSG_LENGTH] ;
	    for (int i = 0 ; i < realSize ; i ++) frame8[i] = (tscType8)frame[i] ;
	    fecDevice->writeFrame (frame8) ;
          
            if (lcl_err == DD_RETURN_OK)
              snprintf (cans, MAXCAR, " %d ", OK) ;
            else {
              // glue_fecdriver_get_error_message (lcl_err, msg) ;
	      std::string msg = FecExceptionHandler::dd_decode_error ( lcl_err ) ;
              std::cout << "Error during the writeFrame: " << lcl_err << std::endl << msg << std::endl ;
              snprintf (cans, MAXCAR, " %d ", NOK) ;
            }
          
            std::cout << "Trame direct ( { " << std::endl ;
            for (int i = 0 ; i < (nbVal+3) ; i ++ )
              std::cout << "0x" << std::hex << (int)frame[i] << std::endl ;
            std::cout << "} )" << std::endl ;
          
            for (int i = 0 ; i < (nbVal+3) ; i ++ ) {
              snprintf (&cans[strlen(cans)], MAXCAR, "%x ", frame[i]) ;
            }
          
            if (lcl_err == DD_RETURN_OK) {
	    
	      try {
		tscType8 frame8[DD_USER_MAX_MSG_LENGTH] ;
		// Read back
		if (frame[2] & FEC_LENGTH_2BYTES) // Size in two bytes length
		  fecDevice->readFrame ( (tscType8)frame[5], frame8 ) ;
		else
		  fecDevice->readFrame ( (tscType8)frame[4], frame8 ) ;

		realSize = frame[2] ;
		if (realSize & FEC_LENGTH_2BYTES) // Size in two bytes length
		  realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;
		for (int i = 0 ; i < realSize ; i ++) frame[i] = (tscType8)frame8[i] ;

		lcl_err = DD_RETURN_OK ;
	      }
	      catch (FecExceptionHandler &e) {

		lcl_err = e.getErrorCode() ;
	      }
            
              realSize = frame[2] ;
              if (realSize & FEC_LENGTH_2BYTES) // Size in two bytes length
                realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;
	    
              std::cout << "readFrame ( { " << std::endl ;
              for (int i = 0 ; i < (realSize+3) ; i ++ )
                std::cout << "0x" << std::hex << (int)frame[i] << std::endl ;
              std::cout << "} )" << std::endl ;
            
              if (lcl_err == DD_RETURN_OK)	      
                snprintf (cans, MAXCAR, " %d ", OK) ;
              else {
                //glue_fecdriver_get_error_message (lcl_err, msg) ;
		std::string msg = FecExceptionHandler::dd_decode_error ( lcl_err ) ;
                std::cout << "Error during the writeFrame:" << lcl_err << std::endl <<  msg << std::endl ;
                snprintf (cans, MAXCAR, " %d ", NOK) ;
              }

              for (int i = 0 ; i < (realSize+3) ; i ++ ) {
                snprintf (&cans[strlen(cans)], MAXCAR, "%x ", frame[i]) ;
              }
            }
          }
          else {

            std::cerr << "This message cannot be used with the r command" << std::endl ;
            std::cerr << "Use w for send the command and f to read back each word of the receive FIFO" << std::endl ;
            snprintf (cans, MAXCAR, " %d ", NOK) ;
          }
          break;
	}
        case 'w': { // Send a frame and read back the direct ack.

          std::cout << "Send a frame and read back the direct ack" << std::endl ;
          std::cout << "The answer can be:" << std::endl ;
          std::cout << "\tdirect acknowledge frame" << std::endl ;
          std::cout << "\t " << NOK << " <Error Code>: error or if the CCU broadcast mode is used" << std::endl ;
          // Set the transaction number to 0xFE
          // convention in FecSoftware to read back a frame for the device driver
          realSize = frame[2] ;
          if (realSize & FEC_LENGTH_2BYTES) { // Size in two bytes length
            realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;
            frame[5] = 0xFE ;
          }
          else frame[4] = 0xFE ;
          
          realSize += 3 ;
          
          std::cout << "writeFrame ( { " << std::endl ;
          for (int i = 0 ; i < realSize ; i ++ )
            std::cout << "0x" << std::hex << (int)frame[i] << std::endl ;
          std::cout << "} )" << std::endl ;

          if (frame[0] != BROADCAST_ADDRESS) {

            // Enable interrupts
            fecDevice->setIRQ (true) ;
          
            // send it
	    tscType8 frame8[DD_USER_MAX_MSG_LENGTH] ;
	    for (int i = 0 ; i < realSize ; i ++) frame8[i] = (tscType8)frame[i] ;
            fecDevice->writeFrame (frame8) ;

            if (lcl_err == DD_RETURN_OK)
              snprintf (cans, MAXCAR, " %d ", OK) ;
            else {
              //glue_fecdriver_get_error_message (lcl_err, msg) ;
	      std::string msg = FecExceptionHandler::dd_decode_error ( lcl_err ) ;
              std::cout << "Error during the writeFrame:" << lcl_err << std::endl << msg << std::endl ;
              snprintf (cans, MAXCAR, " %d ", NOK) ;
            }

            // Disable interrupts
            fecDevice->setIRQ (false) ;
          }
          else {

            // IRQ are disabled

            // send it
	    tscType8 frame8[DD_USER_MAX_MSG_LENGTH] ;
	    for (int i = 0 ; i < realSize ; i ++) frame8[i] = (tscType8)frame[i] ;
            fecDevice->writeFrame (frame8) ;

            // Answer like the direct ack must be read
            // Word by word in FIFO receive
            for (tscType16 i = 0 ; i < (nbVal+3) ; i ++) {
              frame[i] = fecDevice->getFifoReceive() ;
            }
          }
           
          // Put the answer in the msg
          std::cout << "Trame direct ( { " << std::endl ;
          for (int i = 0 ; i < (nbVal+3) ; i ++ )
            std::cout << "0x" << std::hex << (int)frame[i] << std::endl ;
          std::cout << "} )" << std::endl ;
            
          for (int i = 0 ; i < (realSize+3) ; i ++ ) {
            snprintf (&cans[strlen(cans)], MAXCAR, "%x ", frame[i]) ;
          }
          break;
	}
        case 'z': // Reset the PLX and FEC
          std::cout << "Reset FEC" << std::endl ;
          try {
            fecDevice->fecRingReset() ;
          }
          catch (FecExceptionHandler &e) {
            std::cout << "------------ Exception ----------" << std::endl ;
            std::cout << e.what()  << std::endl ;
            std::cout << "---------------------------------" << std::endl ;
            snprintf (cans, MAXCAR, " %d %ld ", NOK, e.getErrorCode()) ;
          }
          break;
        default:
          std::cout << "Unknown command: " << cmd << std::endl ;
          snprintf (cans, MAXCAR, " %d ", NOK) ;
          break ;
	}
      }
      else {
        std::cout << "Command ignored: " << cmd << std::endl ;
        snprintf (cans, MAXCAR, " %d ", NOK) ;
      }
    }
    else {
      snprintf (cans, MAXCAR, " %s", cmd) ;
    }
    
    // Send back the end
    sendto(socketListen,cans,strlen(cans),0,(struct sockaddr *)&sockAddr,soSize);
  }
  while ( msgSize > 0 && strncasecmp(cmd, "END", 3) != 0 );

  try {
    // Enable interrupts
    fecDevice->setIRQ (true) ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }

  return 0 ;
}

/** Open a file and send all the commands to the ring
 * <p>Command: -fileCommand
 * <p>Possible tags in the file are:
 * <ul>
 * <li> noDisplay: Do not display any message
 * <li> noWait: Do not wait after each frame
 * <li> noClearErrors: by default, when an error occurs, a clear of the FEC control register are done (CR1 = 0x3 and CR0 = 0x1). If this command is set the clear is not performed
 * <li> Frame: <Dest Src Len channel transaction command data ...> 
 *      to send a frame over the ring. The direct ack is read back and display.
 *      Note that the transaction is fixed by the device driver so the 
 *      value given is not important (for value 0xFE, see next command).
 * <li> Frame: <Dest Src Len channel 0xFE command data ...> 
 *      to send a frame over the ring and read back the direct ack and the 
 *      answer of the frame
 * <li> w FecRegister Value: write the given FEC register to the value given
 *      (FEC register are CR0, CR1, SR0, SR1).
 * <li> r FecRegister: read the given FEC register and display it
 *      (FEC register are CR0, CR1, SR0, SR1).
 * </ul>
 * \param filename - file name
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \warning the loop is not possible
 */
int readFile ( FecAccess *fecAccess,
	       char *filename,
               tscType8 fecAddress,
               tscType8 ringAddress) {

  FILE *fichier ;
  char strFrame[MAXCAR] ;
  FecRingDevice *fecDevice ;
  int error = 0 ;

  fichier = fopen ( filename, "r" ) ;
  if (fichier == NULL) {
    
    std::cout << filename << " cannot be opened" << std::endl ;
    return (1) ;
  }

  try {
    fecDevice = fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
    //fecDevice->fecRingResetFSM() ;
    fecDevice->fecRingReset() ;
    std::cout << "Status register of the FEC = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    return (1) ;
  }

  bool wait = true ;
  bool display = true ;
  bool displayComment = false ;
  bool fecRegisterAccess = false ;
  bool clearErrors = true ;
  char comment[MAXCAR] ;
  unsigned int tra = 1 ;

  while (fgets (strFrame, MAXCAR, fichier) != NULL) {

    // strtok change the string => copy it
    char strFr[MAXCAR] ;
    snprintf (strFr, MAXCAR, "%s", strFrame) ;
    char *strFrTemp = strFr ;

    // The frame to be sent
    tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ;

    // Number of values
    tscType16 nbVal = 0 ;
    char *str ;
    unsigned int valNew ;
    bool flag = true ;

    do {

      if (flag) {
	// tokenize it
	str = strtok (strFrTemp, " ") ;
	flag = false ;
      }
      else {
	str = strtok (NULL, " ") ;
      }

      if (str != NULL) {

	switch (str[0]) {
	case '#': // A comment => print it
	  displayComment = true; 
	  snprintf (comment, MAXCAR, "%s", strFrame) ;
	  str = NULL ;
	  frame[0] = 0xFF ;
	  break ;
	case 'R': // read a FEC status/control register
	case 'r': 
	  fecRegisterAccess = true ;

	  // Check which register
	  str = strtok (strFrTemp, " ") ;

	  if (strncasecmp (str, "CR0", 3) == 0) {
	    frame[nbVal ++] = 0 ;
	  }
	  else if (strncasecmp (str, "CR1", 3) == 0) {
	    frame[nbVal ++] = 1 ;
	  }
	  else if (strncasecmp (str, "SR0", 3) == 0) {
	    frame[nbVal ++] = 2 ;
	  }
	  else if (strncasecmp (str, "SR1", 3) == 0) {
	    frame[nbVal ++] = 3 ;
	  }
	  else {
	    std::cout << "Unknown FEC register" << std::endl ;
	    fecRegisterAccess = false ;
	    frame[0] = 0xFF ;
	  }
	  str = NULL ;
	  break ;
	case 'W': // write a FEC control register
	case 'w': 
	  fecRegisterAccess = true ;

	  // Check which register
	  str = strtok (strFrTemp, " ") ;

	  if (strncasecmp (str, "CR0", 3) == 0) {
	    frame[nbVal] = 0 ;
	    nbVal++;
	  }
	  else if (strncasecmp (str, "CR1", 3) == 0) {
	    frame[nbVal] = 1 ;
	    nbVal ++ ;
	  }
	  else {
	    std::cout << "Unknown FEC register" << std::endl ;
	    fecRegisterAccess = false ;
	    frame[0] = 0xFF ;
	    str = NULL ;
	  }

	  str = strtok (strFrTemp, " ") ;
	  if (sscanf (str, "%x", &valNew) != 1) {
	    std::cout << "Bad value for the setting" << std::endl ;
	    fecRegisterAccess = false ;
	    frame[0] = 0xFF ;
	    str = NULL ;
	  }
	  else {
	    frame[nbVal] = valNew ;
	    nbVal ++ ;
	  }
	  break ;
	case 'N': // Do not wait after each frame
	case 'n': 
	  if (strncasecmp (str, "nowait", 6) == 0) {
	    std::cout << "No wait" << std::endl ;
	    wait = false ;
	    str = NULL ;
	    frame[0] = 0xFF ;
	  }
	  else if (strncasecmp (str, "nodisplay", 6) == 0) {
	    std::cout << "No display" << std::endl ;
	    display = false ;
	    str = NULL ;
	    frame[0] = 0xFF ;
	  }
	  else if (strncasecmp (str, "noclearerrors", 13) == 0) {
	    std::cout << "After an error, the clear of errors in FEC register is not performed" << std::endl ;
	    clearErrors = false ;
	    str = NULL ;
	    frame[0] = 0xFF ;
	  }
	  break ;
	}
      }

      if (str != NULL) {
        // Convert it
        unsigned int val ;
        if (sscanf (str, "%x", &val)) frame[nbVal ++] = val ;
        else {
          
          // Error
          std::cout << "The frame " << strFrame << " is not correct (val = " << str << ") => frame dropped" << std::endl ;
          str = NULL ;
          frame[0] = 0xFF ;
        }
      }

    } while (str != NULL) ;

    // Find a correct frame
    if (frame[0] != 0xFF) {

      try {

        // Check if it's a FEC register
        if (fecRegisterAccess) {
	
          // Display the comments
          if (display && displayComment) {
            std::cout << comment ;
            displayComment = false ;
          }

          // Read or write the register
          switch (strFrame[0]) {
          case 'r': // Read the corresponding register
          case 'R': 
            switch (frame[0]) {
            case 0: // CR0
              std::cout << "FEC CR0 = 0x" << std::hex << fecDevice->getFecRingCR0() << std::endl ;
              break ;
            case 1: // CR1
              std::cout << "FEC CR1 = 0x" << std::hex << fecDevice->getFecRingCR1() << std::endl ;
	      break ;
            case 2: // SR0
              std::cout << "FEC SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
              break ;
            case 3: // SR1
              std::cout << "FEC SR1 = 0x" << std::hex << fecDevice->getFecRingSR1() << std::endl ;
              break ;
            }
            break ;
          case 'w': // Write the corresponding register
          case 'W':
            switch (frame[0]) {
            case 0: // CR0
	      std::cout << "write 0x" << std::hex << (int)frame[1] << " in CR0" << std::endl ;
              fecDevice->setFecRingCR0 ( frame[1] ) ;
              break ;
            case 1: // CR1
	      std::cout << "write 0x" << std::hex << (int)frame[1] << " in CR1" << std::endl ;
              fecDevice->setFecRingCR1 ( frame[1] ) ;
              break ;
            }
            break;
          }
          fecRegisterAccess = false ;
        }
        else {

          unsigned int realSize = frame[2] ;
          if (realSize & FEC_LENGTH_2BYTES) // Size in two bytes length
            realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;

          realSize += 3 ;

          // Check the length
          if (nbVal == realSize) {
            
            // read back value
            bool readback = (frame[4] == 0xFE) ;
	    frame[4] = tra ; tra ++ ;
	    if (tra == 127) tra = 1 ;
            
            // Display the comments
            if (display && displayComment) {              
              std::cout << comment ;
              displayComment = false ;
            }
            if (display) {              
              std::cout << "writeFrame ( { " ;
              for (unsigned int i = 0 ; i < realSize ; i ++ )
                std::cout << "0x" << std::hex << (int)frame[i] << " " ;
              std::cout << " } )" << std::endl ;
            }

	    tscType32 frameToBeSent[DD_USER_MAX_MSG_LENGTH] ;
	    unsigned int realSize32 = (realSize+3)/4, cpt = 0 ;
	    if ( ((realSize+3) % 4) != 0 ) realSize32 += 1 ;
	    for (unsigned int i = 0 ; i < realSize32 ; i ++) {
	      frameToBeSent[i] = (frame[cpt] << 24) | (frame[cpt+1] << 16) | (frame[cpt+2] << 8) | frame[cpt+3] ;
	      // Next word
	      cpt += 4 ;
	    }

	    // FIFO transmit
	    //fecDevice->setFecRingCR1 ( DD_FEC_CLEAR_ERRORS | DD_FEC_CLEAR_IRQ ) ;
	    std::cout << "Fill the FIFO receive" << std::endl ;
	    fecDevice->setFifoTransmit(frameToBeSent,realSize32) ;
	    std::cout << "Toggle the bit send" << std::endl ;
	    std::cout << "Frame sent SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
	    fecDevice->setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_OR ) ;
	    fecDevice->setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_XOR ) ;
	    // Wait for the send of the frame
	    unsigned int fecSR0 = fecDevice->getFecRingSR0() ;
	    std::cout << "Frame sent SR0 = 0x" << std::hex << fecSR0 << std::endl ;
	    while ( ((fecSR0 & FEC_SR0_TRARUN) || (!(fecSR0 & FEC_SR0_TRAEMPTY))) ) {
	      fecSR0 = fecDevice->getFecRingSR0() ;
	    }
	    fecSR0 = fecDevice->getFecRingSR0() ;
	    std::cout << "Frame sent SR0 = 0x" << std::hex << fecSR0 << std::endl ;
	    // Wait the direct ack
 	    while ( (!(fecSR0 & FEC_SR0_PENDINGIRQ)) ) {
 	      fecSR0 = fecDevice->getFecRingSR0() ;
 	    }
	    std::cout << "Direct ack (at least) received SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
	    // Read the direct
	    // Size of dir ACK is same as frame itself plus status byte , like this:
	    tscType32 frameReceived[DD_USER_MAX_MSG_LENGTH] ;
	    realSize32 = (realSize+3+1)/4 ;
	    if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
	    fecDevice->getFifoReceive(frameReceived,realSize32);

	    realSize = (frameReceived[0] >> 8) & 0xFF ;
	    tscType8 word2 = frameReceived[0]  & 0xFF ;
	    if (realSize & FEC_LENGTH_2BYTES) 
	      realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
	    realSize32 = (realSize+3+1)/4 ;
	    if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;

	    std::cout << realSize32 << std::endl ;
	    std::cout << realSize << std::endl ;
	    getchar() ;

	    cpt = 0 ;
	    memset(frame,0,DD_USER_MAX_MSG_LENGTH*4) ;
	    for (tscType32 i = 0 ; i < realSize32 ; i ++) {
	      frame[cpt++] = (frameReceived[i] >> 24) & 0xFF ;
	      frame[cpt++] = (frameReceived[i] >> 16) & 0xFF ;
	      frame[cpt++] = (frameReceived[i] >>  8) & 0xFF ;
	      frame[cpt++] = (frameReceived[i])       & 0xFF ;
	    }

	    if (display) {
	      std::cout << "Direct ack ( { " ;
	      for (unsigned int i = 0 ; i < (realSize+3+1) ; i ++ )
		std::cout << "0x" << std::hex << (int)frame[i] << " " ;
	      std::cout << "} )" << std::dec << std::endl ;
	    }

	    if (readback) {
	      memset(frameReceived,0,DD_USER_MAX_MSG_LENGTH) ;
	      std::cout << "Wait for the force ack or answer SR0 = " << std::hex << fecDevice->getFecRingSR0() << std::endl ;
	      fecSR0 = fecDevice->getFecRingSR0() ;
	      while ( !(fecSR0 & FEC_SR0_DATATOFEC) ) {
		fecSR0 = fecDevice->getFecRingSR0() ; ;
	      }
	      std::cout << "Frame received SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
	      // Read back the frame
	      std::cout << "Read back the frame" << std::endl ;
	      frameReceived[0] = fecDevice->getFifoReceive() ;
	      // Check the size of the frame
	      realSize = (frameReceived[0] >> 8) & 0xFF ;
	      tscType8 word2 = frameReceived[0]  & 0xFF ;
	      if (realSize & FEC_LENGTH_2BYTES) {
		realSize = ((realSize & 0x7F) << 8) + word2 + 1 ;
		// +1 for the two byte words for length
	      }
	      // +3 => Dst, Src, Length  ; +1 => status after the frame
	      realSize32 = (realSize+3+1)/4 ;
	      if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
	      fecDevice->getFifoReceive(frameReceived+1,realSize32-1);
	      cpt = 0 ;
	      memset(frame,0,DD_USER_MAX_MSG_LENGTH*4) ;
	      for (tscType32 i = 0 ; i < realSize32 ; i ++) {
		frame[cpt++] = (frameReceived[i] >> 24) & 0xFF ;
		frame[cpt++] = (frameReceived[i] >> 16) & 0xFF ;
		frame[cpt++] = (frameReceived[i] >>  8) & 0xFF ;
		frame[cpt++] = (frameReceived[i])       & 0xFF ;
	      }
	      if (display) {
		std::cout << "Direct ack ( { " ;
		for (unsigned int i = 0 ; i < (realSize+3+1) ; i ++ )
		  std::cout << "0x" << std::hex << (int)frame[i] << " " ;
		std::cout << "} )" << std::dec << std::endl ;
	      }
	    }
	  }
	}
      }
      catch (FecExceptionHandler &e) {
        
        std::cout << "------------ Exception ----------" << std::endl ;
        std::cout << e.what()  << std::endl ;
        std::cout << "---------------------------------" << std::endl ;
        
        error += 1 ;
        if (clearErrors) {
          std::cout << "Clear errors" << std::endl ;
          //fecDevice->setFecRingCR0(0x1) ;
          fecDevice->setFecRingCR1(0x3) ;
        }
      }
      
      if (! noGetchar_) {
	if (wait) {
	  // Wait for a key
	  std::cout << "Press enter to continue" << std::endl ;
	  getchar() ;
	}
      }
    }
  }

  return (error) ;
}

/** Open a file and send all the commands to the ring
 * <p>Command: -fileCommand
 * <p>Possible tags in the file are:
 * <ul>
 * <li> noDisplay: Do not display any message
 * <li> noWait: Do not wait after each frame
 * <li> noClearErrors: by default, when an error occurs, a clear of the FEC control register are done (CR1 = 0x3 and CR0 = 0x1). If this command is set the clear is not performed
 * <li> Frame: <Dest Src Len channel transaction command data ...> 
 *      to send a frame over the ring. The direct ack is read back and display.
 *      Note that the transaction is fixed by the device driver so the 
 *      value given is not important (for value 0xFE, see next command).
 * <li> Frame: <Dest Src Len channel 0xFE command data ...> 
 *      to send a frame over the ring and read back the direct ack and the 
 *      answer of the frame
 * <li> w FecRegister Value: write the given FEC register to the value given
 *      (FEC register are CR0, CR1, SR0, SR1).
 * <li> r FecRegister: read the given FEC register and display it
 *      (FEC register are CR0, CR1, SR0, SR1).
 * </ul>
 * \param filename - file name
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \warning the loop is not possible
 */
int readFile1 ( FecAccess *fecAccess,
	       char *filename,
               tscType8 fecAddress,
               tscType8 ringAddress) {

  FILE *fichier ;
  char strFrame[MAXCAR] ;
  FecRingDevice *fecDevice ;
  int error = 0 ;
  DD_TYPE_ERROR lcl_err ;

  fichier = fopen ( filename, "r" ) ;
  if (fichier == NULL) {
    
    std::cout << filename << " cannot be opened" << std::endl ;
    return (1) ;
  }

  try {
    fecDevice = fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
    fecDevice->fecRingResetFSM() ;
    fecDevice->fecRingReset() ;
    std::cout << "Status register of the FEC = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    return (1) ;
  }

  bool wait = true ;
  bool display = true ;
  bool displayComment = false ;
  bool fecRegisterAccess = false ;
  bool clearErrors = true ;
  char comment[MAXCAR] ;

  while (fgets (strFrame, MAXCAR, fichier) != NULL) {

    // strtok change the string => copy it
    char strFr[MAXCAR] ;
    snprintf (strFr, MAXCAR, "%s", strFrame) ;
    char *strFrTemp = strFr ;

    // The frame to be sent
    tscType16 frame[DD_USER_MAX_MSG_LENGTH] ;

    // Number of values
    tscType16 nbVal = 0 ;
    char *str ;
    unsigned int valNew ;
    bool flag = true ;

    do {

      if (flag) {
	// tokenize it
	str = strtok (strFrTemp, " ") ;
	flag = false ;
      }
      else {
	str = strtok (NULL, " ") ;
      }

      if (str != NULL) {

	switch (str[0]) {
	case '#': // A comment => print it
	  displayComment = true; 
	  snprintf (comment, MAXCAR, "%s", strFrame) ;
	  str = NULL ;
	  frame[0] = 0xFFFF ;
	  break ;
	case 'R': // read a FEC status/control register
	case 'r': 
	  fecRegisterAccess = true ;

	  // Check which register
	  str = strtok (strFrTemp, " ") ;

	  if (strncasecmp (str, "CR0", 3) == 0) {
	    frame[nbVal ++] = 0 ;
	  }
	  else if (strncasecmp (str, "CR1", 3) == 0) {
	    frame[nbVal ++] = 1 ;
	  }
	  else if (strncasecmp (str, "SR0", 3) == 0) {
	    frame[nbVal ++] = 2 ;
	  }
	  else if (strncasecmp (str, "SR1", 3) == 0) {
	    frame[nbVal ++] = 3 ;
	  }
	  else {
	    std::cout << "Unknown FEC register" << std::endl ;
	    fecRegisterAccess = false ;
	    frame[0] = 0xFFFF ;
	  }
	  str = NULL ;
	  break ;
	case 'W': // write a FEC control register
	case 'w': 
	  fecRegisterAccess = true ;

	  // Check which register
	  str = strtok (strFrTemp, " ") ;

	  if (strncasecmp (str, "CR0", 3) == 0) {
	    frame[nbVal] = 0 ;
	    nbVal++;
	  }
	  else if (strncasecmp (str, "CR1", 3) == 0) {
	    frame[nbVal] = 1 ;
	    nbVal ++ ;
	  }
	  else {
	    std::cout << "Unknown FEC register" << std::endl ;
	    fecRegisterAccess = false ;
	    frame[0] = 0xFFFF ;
	    str = NULL ;
	  }

	  str = strtok (strFrTemp, " ") ;
	  if (sscanf (str, "%x", &valNew) != 1) {
	    std::cout << "Bad value for the setting" << std::endl ;
	    fecRegisterAccess = false ;
	    frame[0] = 0xFFFF ;
	    str = NULL ;
	  }
	  else {
	    frame[nbVal] = valNew ;
	    nbVal ++ ;
	  }
	  break ;
	case 'N': // Do not wait after each frame
	case 'n': 
	  if (strncasecmp (str, "nowait", 6) == 0) {
	    std::cout << "No wait" << std::endl ;
	    wait = false ;
	    str = NULL ;
	    frame[0] = 0xFFFF ;
	  }
	  else if (strncasecmp (str, "nodisplay", 6) == 0) {
	    std::cout << "No display" << std::endl ;
	    display = false ;
	    str = NULL ;
	    frame[0] = 0xFFFF ;
	  }
	  else if (strncasecmp (str, "noclearerrors", 13) == 0) {
	    std::cout << "After an error, the clear of errors in FEC register is not performed" << std::endl ;
	    clearErrors = false ;
	    str = NULL ;
	    frame[0] = 0xFFFF ;
	  }
	  break ;
	}
      }

      if (str != NULL) {
        // Convert it
        unsigned int val ;
        if (sscanf (str, "%x", &val)) frame[nbVal ++] = val ;
        else {
          
          // Error
          std::cout << "The frame " << strFrame << " is not correct (val = " << str << ") => frame dropped" << std::endl ;
          str = NULL ;
          frame[0] = 0xFFFF ;
        }
      }

    } while (str != NULL) ;

    // Find a correct frame
    if (frame[0] != 0xFFFF) {

      try {

        // Check if it's a FEC register
        if (fecRegisterAccess) {
	
          // Display the comments
          if (display && displayComment) {
            std::cout << comment << std::endl ;
            displayComment = false ;
          }

          // Read or write the register
          switch (strFrame[0]) {
          case 'r': // Read the corresponding register
          case 'R': 
            switch (frame[0]) {
            case 0: // CR0
              std::cout << "FEC CR0 = 0x" << std::hex << fecDevice->getFecRingCR0() << std::endl ;
              break ;
            case 1: // CR1
              std::cout << "FEC CR1 = 0x" << std::hex << fecDevice->getFecRingCR1() << std::endl ;
	      break ;
            case 2: // SR0
              std::cout << "FEC SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
              break ;
            case 3: // SR1
              std::cout << "FEC SR1 = 0x" << std::hex << fecDevice->getFecRingSR1() << std::endl ;
              break ;
            }
            break ;
          case 'w': // Write the corresponding register
          case 'W':
            switch (frame[0]) {
            case 0: // CR0
	      std::cout << "write 0x" << std::hex << (int)frame[1] << " in CR0" << std::endl ;
              fecDevice->setFecRingCR0 ( frame[1] ) ;
              break ;
            case 1: // CR1
	      std::cout << "write 0x" << std::hex << (int)frame[1] << " in CR1" << std::endl ;
              fecDevice->setFecRingCR1 ( frame[1] ) ;
              break ;
            }
            break;
          }
          fecRegisterAccess = false ;
        }
        else {

          tscType16 realSize = frame[2] ;
          if (realSize & FEC_LENGTH_2BYTES) // Size in two bytes length
            realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;

          realSize += 3 ;

          // Check the length
          if (nbVal == realSize) {
            
            // read back value
            bool readback = (frame[4] == 0xFE) ;
            
            // Display the comments
            if (display && displayComment) {
              
              std::cout << comment << std::endl ;
              displayComment = false ;
            }

            if (display) {
              
              std::cout << "writeFrame ( { " << std::endl ;
              for (int i = 0 ; i < realSize ; i ++ )
                std::cout << "0x" << std::hex << (int)frame[i] ;
              std::cout << " } )" << std::endl ;
            }
	    
            // send it
	    try {
	      tscType8 frame8[DD_USER_MAX_MSG_LENGTH] ;
	      for (int i = 0 ; i < realSize ; i ++) frame8[i] = (tscType8)frame[i] ;
	      fecDevice->writeFrame (frame8) ;
	      lcl_err = DD_RETURN_OK ;
	    }
	    catch (FecExceptionHandler &e) {
	      lcl_err = e.getErrorCode() ;
	    }
            
            if (lcl_err != DD_RETURN_OK) {
              //glue_fecdriver_get_error_message (lcl_err, msg) ;
	      std::string msg = FecExceptionHandler::dd_decode_error ( lcl_err ) ;
              std::cout << "Error during the writeFrame: " << lcl_err << std::endl << msg << std::endl ;
              if (clearErrors) {
                std::cout << "Clear errors" << std::endl ;
                fecDevice->setFecRingCR0(0x1) ;
                fecDevice->setFecRingCR1(0x3) ;
              }
            }

            if (display) {
              std::cout << "Trame direct ( { " << std::endl ;
              for (int i = 0 ; i < (nbVal+3) ; i ++ )
		std::cout << "0x" << std::hex << (int)frame[i] << std::endl ;
              std::cout << " } )" << std::endl ;
            }

            // Read back
            if (readback && (lcl_err == DD_RETURN_OK)) {

	      try {
		tscType8 frame8[DD_USER_MAX_MSG_LENGTH] ;
		// Read back
		if (frame[2] & FEC_LENGTH_2BYTES) // Size in two bytes length
		  fecDevice->readFrame ( (tscType8)frame[5], frame8 ) ;
		else
		  fecDevice->readFrame ( (tscType8)frame[4], frame8 ) ;

		realSize = frame[2] ;
		if (realSize & FEC_LENGTH_2BYTES) // Size in two bytes length
		  realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;
		for (int i = 0 ; i < realSize ; i ++) frame[i] = (tscType8)frame8[i] ;

		lcl_err = DD_RETURN_OK ;
	      }
	      catch (FecExceptionHandler &e) {

		lcl_err = e.getErrorCode() ;
	      }

              realSize = frame[2] ;
              if (realSize & FEC_LENGTH_2BYTES) // Size in two bytes length
                realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;

              if (lcl_err == DD_RETURN_OK) {
                
                if (display) {
                  std::cout << "readFrame ( { " << std::endl ;
                  for (int i = 0 ; i < (realSize+3) ; i ++ )
		    std::cout << "0x" << std::hex << (int)frame[i] << std::endl ;
                  std::cout << " } )" << std::endl ;
                }
              } 
              else {
                //glue_fecdriver_get_error_message (lcl_err, msg) ;
		std::string msg = FecExceptionHandler::dd_decode_error ( lcl_err ) ;
                std::cout << "Error during the writeFrame: " << lcl_err << std::endl << msg << std::endl ;
              }
            }
          }
        }
      }

      catch (FecExceptionHandler &e) {
        
        std::cout << "------------ Exception ----------" << std::endl ;
        std::cout << e.what()  << std::endl ;
        std::cout << "---------------------------------" << std::endl ;
        
        error += 1 ;
        if (clearErrors) {
          std::cout << "Clear errors" << std::endl ;
          fecDevice->setFecRingCR0(0x1) ;
          fecDevice->setFecRingCR1(0x3) ;
        }
      }
      
      if (! noGetchar_) {
	if (wait) {
	  // Wait for a key
	  std::cout << "Press enter to continue" << std::endl ;
	  getchar() ;
	}
      }
    }
  }

  return (error) ;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       For CCU alarms                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/**
 * This method wait for CCU alarms
 * <p>command: -waitCCUAlarms
 * \param fecAccess - FEC access object
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \warning the loop is not used
 */
void waitCCUAlarms ( FecAccess *fecAccess, 
                     tscType8 fecAddress,
		     tscType8 ringAddress,
                     long loop, unsigned long tms ) {

  // Find the FECs
try {
// Scan the PCI slot for device driver loaded
    fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
}
catch (FecExceptionHandler &e) {  
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot find any FECs on the ring: " << e.what() << std::endl ; 
    std::cerr << "**************************************************" << std::endl ;
    
    return ;
  }

  std::list<keyType> *ccuList = NULL ;
  try {

    // Scan the ring for each FECs and display the CCUs
    ccuList = fecAccess->getCcuList ( buildFecRingKey (fecAddress, ringAddress) ) ;

    if (ccuList != NULL) {
      
      // Wait for any alarms
      for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
	
	for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
          
	  keyType index = *p ;

	  // After each alarm, the bits are cleared => reenable the alarms
	  // This method also enable interrupts in PIA channel. The index must contain
	  // the value of the given channel or 0 if you want all PIA channels enabled.
	  std::cout << std::hex << "FEC 0x" << getFecKey(index) 
		    << " Ring 0x " << getRingKey(index) 
		    << " CCU 0x" << getCcuKey(index)
		    << " found => enable alarms 1 to 4" << std::endl ;
	  fecAccess->setCcuAlarmsEnable ( index, true, true, true, true ) ;
          
	  //std::cout << "CCU CRB = 0x" << std::hex << fecAccess->getCcuCRB (index) << std::endl ;
	}
	
	// Wait for ever to any alarms
	tscType8 frame[DD_USER_MAX_MSG_LENGTH] = {0} ;
	fecAccess->waitForAnyCcuAlarms (buildFecRingKey(fecAddress,ringAddress), frame) ;
	
	// Display Frame
	tscType16 realSize = frame[2] ;
	if (realSize & FEC_LENGTH_2BYTES) // Size in two bytes length
	  realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;
	
	// Date and time
	time_t timestamp = time(NULL) ;
	fprintf (stdchan_, "timeStamp=%ld\n", timestamp) ;
	fprintf (stdchan_, "Warning emitted by a device, Frame = ") ;
	fprintf (stdchan_, "{ ") ;
	for (int i = 0 ; i < (realSize+3) ; i ++ )
	  fprintf (stdchan_, "0x%x ", frame[i]) ;
	fprintf (stdchan_, "} )\n") ;
	
 	if (! noGetchar_) {
	  // Display a message to clear the source
	  std::cout << "Please Clear the source before press enter" << std::endl ;
	  getchar() ;
	}
	
	// Wait
	if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
      }
    }
    else {
      std::cout << "No CCU found on FECs" << std::endl ;
    }  
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Delete = already store in the table of FEC device class
  delete ccuList ;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       i2c channels methods                              */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** 
 * <p>Command: -i2c
 * \param fecAccess - FEC Access object
 * \param fecAddress - slot of the FEC.
 * \param ringAddress - slot of the ring.
 * \param ccuAddress - address of the first CCU of the ring.
 * \param channelAddress - address of the channel.
 * \param deviceAddress - address of the device.
 * \param modeType - type of device to access.
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
void testI2CDevice (FecAccess *fecAccess,
                    tscType8 fecAddress,
                    tscType8 ringAddress,
                    tscType8 ccuAddress,
                    tscType8 channelAddress,
                    tscType8 deviceAddress,
                    enumDeviceType modeType,
                    long loop, unsigned long tms ) {

  unsigned int value ;
  keyType index = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,deviceAddress) ; 

  try {
    fecAccess->addi2cAccess (index, 
                             modeType,
                             MODE_SHARE) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return ;
  }

  std::cout << "Value of the device : 0x" << std::hex << (int)fecAccess->read(index) << std::endl ;
  std::cout << "Value to write to the device : 0x" ;
  std::cin >> value ;

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {
      std::cout << "Writing 0x" << std::hex << value << " to the device..." << std::endl ;
      fecAccess->write(index, value) ;
      std::cout << "Check... => new value of the device : 0x" << std::hex << (int)fecAccess->read(index) << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;

      // display all registers
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;
      if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
      if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }
}

/** <p> no command:
 * make a pia reset on all CCUs that can be found and make a PIA reset on all PIA channels
 * \param fecAccess - FEC Access object
 */
void allCCUsPiaReset ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress ) {

  std::cout << "Making a PIA reset for the FEC " << std::dec << (int)fecAddress << " ring " << (int)ringAddress << " on all CCUs on all PIA channels" << std::endl ;
  
  // Scan the PCI or VME slot for device driver loaded
  //std::list<keyType> *fecSlotList = fecAccess->getFecList( ) ;
    
  //if ((fecSlotList != NULL) && (!fecSlotList->empty())) {
  //for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
            
  keyType indexFEC = buildFecRingKey(fecAddress,ringAddress) ;
  //std::cout << "-------------------------------------------------" << std::endl ;
  //std::cout << "Scan the ring to find CCUs for FEC " << std::dec << (int)getFecKey(indexFEC) << " ring " << (int)getRingKey(indexFEC) << std::endl ;
      
  try {
    // Scan the ring for each FECs and display the CCUs
    std::list<keyType> *ccuList = fecAccess->getCcuList ( indexFEC ) ;
    
    if (ccuList != NULL) {
      
      for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {
	
	keyType index = *p ;
	
	// For each channel
	for (tscType8 channel = 0x30 ; channel <= 0x30 ; channel ++) {
	  
	  try {
	    keyType indexPIA = buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),channel,0) ;
		
	    char msg[80] ;
	    decodeKey(msg, indexPIA) ;
	    //std::cout << "Making a PIA reset on channel " << msg << std::endl ;
	    
	    PiaResetAccess piaResetAccess(fecAccess, indexPIA, 0xFF) ;
	    piaResetDescription piaD ( index, (unsigned long)1000, (unsigned long)100, (tscType8)0xFF ) ;
	    piaResetAccess.setValues (piaD) ;
	    //piaResetDescription *piaD1 = piaResetAccess.getValues ( ) ;
	    
	    //if (piaD == *piaD1)
	    //std::cout << "Pia reset comparison ok" << std::endl ;
	    //else
	    //std::cout << "Pia reset comparison error" << std::endl ;
	  }
	  catch (FecExceptionHandler &e) {

	    std::cout << "------------ Exception ----------" << std::endl ;
	    std::cout << e.what()  << std::endl ;
	    std::cout << "---------------------------------" << std::endl ;

	    // display all registers
	    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	    // Original frame
	    if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;	    
	    if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
	    if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;
	  }
	}
      }
    }
    else {
      std::cerr << "Did not found CCU on the FEC " << getFecKey(indexFEC) << " ring " << getRingKey(indexFEC) << std::endl ;
    }
    
    delete ccuList ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
  
  if (! noGetchar_) {
    
    std::cout << "Pia reset performed" << std::endl ;
    std::cout << "Press <enter> to continue" << std::endl ;
    getchar() ;
  }
}

/** 
 * <p>command: -tracker
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param sixApvsF - test 6 APVs ( 0x20, 0x21, 0x22, 0x23, 0x24, 0x25 )
 * \param fourApvsF - test 4 APVs ( 0x20, 0x21, 0x24, 0x25 )
 * \param apvmuxF - test the MUX (0x43)
 * \param dcuF - test the DCU (DCUADDRESS)
 * \param laserdriverF - test the laserdriver (0x60)
 * \param dohF - test the laserdriver (0x70)
 * \param pllF - test the PLL (0x44)
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testTrackerDevices ( FecAccess *fecAccess,
                         tscType8 fecAddress, 
                         tscType8 ringAddress,
                         tscType8 ccuAddress, 
                         tscType8 channelAddress,
                         bool sixApvsF,
                         bool fourApvsF,
                         bool apvmuxF,
                         bool dcuF,
                         bool laserdriverF,
                         bool dohF,
                         bool pllF,
                         long loop, unsigned long tms,
			 std::vector<std::string> &results) {

  bool resetEachError = true ;
  bool resetPerformed = true ;

  // Start to make a PIA reset
  //std::cout << "Make a PIA reset on all FEC and all CCUs" << std::endl ;
  // allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ; // mersi patch

  int error = 0 ;
  apvAccess *apv1 = NULL ;
  apvAccess *apv2 = NULL ;
  apvAccess *apv3 = NULL ;
  apvAccess *apv4 = NULL ;
  apvAccess *apv5 = NULL ;
  apvAccess *apv6 = NULL ;
  muxAccess *mux = NULL ;
  dcuAccess *dcu = NULL ;
  laserdriverAccess *laserdriver = NULL;
  DohAccess *doh = NULL ;
  pllAccess *pll = NULL ;

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    // In case of reset, performed a new connections
    if (resetPerformed) {
      try {
	
	if (sixApvsF || fourApvsF) {
	  
	  apv1 = new apvAccess ( fecAccess,
				 fecAddress,
				 ringAddress,
				 ccuAddress,
				 channelAddress,
				 0x20) ;

	  apv2 = new apvAccess ( fecAccess,
				 fecAddress,
				 ringAddress,
				 ccuAddress,
				 channelAddress,
				 0x21) ;
	  
	  apv3 = new apvAccess ( fecAccess,
				 fecAddress,
				 ringAddress,
				 ccuAddress,
				 channelAddress,
				 0x24) ;
	  
	  apv4 = new apvAccess ( fecAccess,
				 fecAddress,
				 ringAddress,
				 ccuAddress,
				 channelAddress,
				 0x25) ;
	  
	  if (sixApvsF) {
	    apv5 = new apvAccess ( fecAccess,
				   fecAddress,
				   ringAddress,
				   ccuAddress,
				   channelAddress,
				   0x22) ;
	    
	    apv6 = new apvAccess ( fecAccess,
				   fecAddress,
				   ringAddress,
				   ccuAddress,
				   channelAddress,
				   0x23) ;
	  }
	}
	
	if (apvmuxF) {
	  mux = new muxAccess ( fecAccess,
				fecAddress,
				ringAddress,
				ccuAddress,
				channelAddress,
				0x43) ;
	}
	
	if (dcuF) {
	  
	  dcu = new dcuAccess ( fecAccess,
				fecAddress,
				ringAddress,
				ccuAddress,
				channelAddress,
				DCUADDRESS) ;
	}
	
	if (laserdriverF) {
	  
	  laserdriver = new laserdriverAccess (fecAccess,
					       fecAddress,
					       ringAddress,
					       ccuAddress,
					       channelAddress,
					       0x60) ;
	}
	
	if (dohF) {
	  
	  doh = new DohAccess (fecAccess,
			       fecAddress,
			       ringAddress,
			       ccuAddress,
			       channelAddress,
			       0x70) ;
	}
	
	if (pllF) {
	  
	  pll = new pllAccess (fecAccess,
			       fecAddress,
			       ringAddress,
			       ccuAddress,
			       channelAddress,
			       0x44) ;
	}

	resetPerformed = false ;
      }
      catch (FecExceptionHandler &e) {
	
	std::cout << "------------ Exception ----------" << std::endl ;
	std::cout << "Cannot create the access to a device" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "---------------------------------" << std::endl ;
	
	if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;
	
	// display all registers
	if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	// Original frame
	if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;
	if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
	if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;
	
	return 1 ;
      }
    }

    if(!dohF && !(dcuF && !fourApvsF)) results.push_back("<Module>");

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;
    
    try {

      // JM
      if(!(!sixApvsF && fourApvsF && !apvmuxF && dcuF && !laserdriverF && !dohF && !pllF)) {

	// Init the settings value for the apv
	
	apvDescription apvd ((tscType8)47,
			     (tscType8)100,
			     (tscType8)4,
			     (tscType8)98,
			     (tscType8)52,
			     (tscType8)34,
			     (tscType8)80,
			     (tscType8)34,
			     (tscType8)55,
			     (tscType8)34,
			     (tscType8)40,
			     (tscType8)0, // Ispare
			     (tscType8)30,
			     (tscType8)70,
			     (tscType8)39,
			     (tscType8)0,
			     (tscType8)8,
			     (tscType8)0) ; // Mersi patch: configuring APVs correctly

	std::cout << "mersi values for the apv" << std::endl; 

	if ( (apv1 != NULL) || 
	     (apv2 != NULL) || 
	     (apv3 != NULL) || 
	     (apv4 != NULL) ) std::cout << "Set the APVs" << std::endl ;
	
	// Set the APVs
	if (apv1 != NULL) apv1->setValues ( apvd );
	if (apv2 != NULL) apv2->setValues ( apvd );
	if (apv3 != NULL) apv3->setValues ( apvd );
	if (apv4 != NULL) apv4->setValues ( apvd );
	if (apv5 != NULL) apv5->setValues ( apvd );
	if (apv6 != NULL) apv6->setValues ( apvd );
	
	if ( (apv1 != NULL) || 
	     (apv2 != NULL) || 
	     (apv3 != NULL) || 
	     (apv4 != NULL) ) std::cout << "Apv set !!!" << std::endl ;
	
	// Get their values back
	if (apv1 != NULL) {
	  apvDescription *monApv1 = apv1->getValues() ;
	  if (*monApv1 == apvd) { // Show debug message
	    std::cout << "Apv 1 Ok" << std::endl ;
	  }
	  else {
	    std::cout << "Apv 1 Error" << std::endl ;
	    if(!dcuF) results.push_back("APV 0x20 Error");
	  }
	  
	  monApv1->display() ;
	  delete monApv1 ;
	}
	if (apv2 != NULL) {
	  apvDescription *monApv2 = apv2->getValues() ;
	  if (*monApv2 == apvd) { // Show debug message
	    std::cout << "Apv 2 Ok" << std::endl ;
	  }
	  else {
	    std::cout << "Apv 2 Error" << std::endl ;
	    if(!dcuF) results.push_back("APV 0x21 Error");
	  }
	  
	  monApv2->display() ;
	  delete monApv2 ;
	}
	if (apv3 != NULL) {
	  apvDescription *monApv3 = apv3->getValues() ;
	  if (*monApv3 == apvd) { // Show debug message
	    std::cout << "Apv 3 Ok" << std::endl ;
	  }
	  else {
	    std::cout << "Apv 3 Error" << std::endl ;
	    if(!dcuF) results.push_back("APV 0x24 Error");
	  }
	  
	  monApv3->display() ;
	  delete monApv3 ;
	}
	if (apv4 != NULL) {
	  apvDescription *monApv4 = apv4->getValues() ;
	  if (*monApv4 == apvd) { // Show debug message
	    std::cout << "Apv 4 Ok" << std::endl ;
	  }
	  else {
	    std::cout << "Apv 4 Error" << std::endl ;
	    if(!dcuF) results.push_back("APV 0x25 Error");
	  }
	  
	  monApv4->display() ;
	  delete monApv4 ;
	}
	if (apv5 != NULL) {
	  apvDescription *monApv5 = apv5->getValues() ;
	  if (*monApv5 == apvd) { // Show debug message
	    std::cout << "Apv 5 Ok" << std::endl ;
	  }
	  else {
	    std::cout << "Apv 5 Error" << std::endl ;
	    if(!dcuF) results.push_back("APV 0x22 Error");
	  }
	  
	  monApv5->display() ;
	  delete monApv5 ;
	}
	if (apv6 != NULL) {
	  apvDescription *monApv6 = apv6->getValues() ;
	  if (*monApv6 == apvd) { // Show debug message
	    std::cout << "Apv 6 Ok" << std::endl ;
	  }
	  else {
	    std::cout << "Apv 6 Error" << std::endl ;
	    if(!dcuF) results.push_back("APV 0x23 Error");
	  }
	  
	  monApv6->display() ;
	  delete monApv6 ;
	}
      }
      
      if (mux != NULL) {
	  
        muxDescription muxd ((tscType16)0xFF) ;

        std::cout << "Set MUX" << std::endl ;
        mux->setValues (muxd) ; // Set the Mux to the muxd value
        std::cout << "MUX set !!!" << std::endl ;

        muxDescription *monmux = mux->getValues() ; // Get the Mux value
        if (*monmux == muxd) { // Show debug message
          std::cout << "Mux Ok" << std::endl ;
        }
        else {
          std::cout << "Mux Error" << std::endl ;
	  results.push_back("MUX Error");
        }
        monmux->display() ;
        delete monmux ;
      }

      if (dcu != NULL) {

	if(fourApvsF) 
	  results.push_back("<DCU>");
	else results.push_back("<DCUCCU>");
	
        dcuDescription *mondcu= dcu->getValues() ;

	results.push_back("   <DcuHardId>" + IntToString(mondcu->getDcuHardId(), 10) + "</DcuHardId>");

        results.push_back("   <Channel0 = " + IntToString(mondcu->getDcuChannel0(),10) + ">");
	results.push_back("   <Channel1 = " + IntToString(mondcu->getDcuChannel1(),10) + ">");
	results.push_back("   <Channel2 = " + IntToString(mondcu->getDcuChannel2(),10) + ">");
	results.push_back("   <Channel3 = " + IntToString(mondcu->getDcuChannel3(),10) + ">");
	results.push_back("   <Channel4 = " + IntToString(mondcu->getDcuChannel4(),10) + ">");
	results.push_back("   <Channel5 = " + IntToString(mondcu->getDcuChannel5(),10) + ">");
	results.push_back("   <Channel6 = " + IntToString(mondcu->getDcuChannel6(),10) + ">");
	results.push_back("   <Channel7 = " + IntToString(mondcu->getDcuChannel7(),10) + ">");
	mondcu->display() ;      
        delete mondcu ;

	if(fourApvsF) 
	  results.push_back("</DCU>");
	else results.push_back("</DCUCCU>");
      }

      if (laserdriver != NULL) {

        // Init the settings value for the laserdriver
        tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
        for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
          biasInit[i] = 18 ;
          gainInit[i] = 2  ;
        }
        laserdriverDescription ldd (gainInit,biasInit) ;

        std::cout << "Set the laserdriver" << std::endl ;
        laserdriver->setValues (ldd) ;
        std::cout << "Laserdriver set !!!" << std::endl ;

        // Get its value back
        laserdriverDescription *mald = laserdriver->getValues() ;

        if (*mald == ldd) { // Show debug message
          std::cout << "Laser driver Ok" << std::endl ;
        }
        else {
          std::cout << "Laser driver Error" << std::endl ;
	  results.push_back("LASERDRIVER Error");
        }

        mald->display ( ) ;
	delete mald ;
      }

      if (doh != NULL) {

        // Init the settings value for the doh
        tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
        for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
          biasInit[i] = 48 ;
          gainInit[i] = 2 ;
        }
        laserdriverDescription ldd (gainInit,biasInit) ;

        std::cout << "Set the doh" << std::endl ;
        doh->setValues (ldd, true) ;
        std::cout << "Doh set !!!" << std::endl ;

        // Get its value back
        laserdriverDescription *mald = doh->getValues() ;

        if (*mald == ldd) { // Show debug message
          std::cout << "DOH Ok" << std::endl ;
	  results.push_back("DOH Ok");
        }
        else {
          std::cout << "DOH Error" << std::endl ;
	  results.push_back("DOH Error");
        }

        mald->display ( ) ;
	delete mald ;
      }

      if (pll != NULL) {

        pllDescription plld ((tscType8)6,(tscType8)1) ;
        std::cout << "Set the PLL" << std::endl ;
        pll->setValues (plld) ;
        std::cout << "PLL set !!!" << std::endl ;

        // Get its value back     
        pllDescription *mapll = pll->getValues() ;
        if (*mapll == plld) { // Show debug message
          std::cout << "Pll Ok" << std::endl ;
        }
        else {
          std::cout << "Pll Error" << std::endl ;
	  results.push_back("PLL Error");
        }
        // Show the settings recovered from the pll
        mapll->display() ;
        delete mapll ;
      }

      results.push_back("<LOOP>");
    }

    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;

      error += 1 ;

      //if (error > MAXERRORCOUNT) loop = loopI ;
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;
      if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
      if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;

      // Make a reset
      if (resetEachError) {

	// Before making any reset, delete the pointers
	try {
	  delete apv1 ; delete apv2 ; delete apv3 ; delete apv4 ; delete apv5 ; delete apv6 ;
	  delete mux ; delete dcu ; delete laserdriver ; delete doh ; delete pll ;
	  apv1 = apv2 = apv3 = apv4 = apv5 = apv6 = NULL ;
	  mux = NULL ; dcu = NULL ; laserdriver = NULL ; doh = NULL ; pll = NULL ;
	}
	catch (FecExceptionHandler &e) {
      
	  std::cout << "------------ Exception ----------" << std::endl ;
	  std::cout << e.what()  << std::endl ;
	  std::cout << "---------------------------------" << std::endl ;
	  
	  if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    
	  
	  error += 1 ;
	  
	  // Check the registers of the FEC and CCU and display it
	  if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	  // Original frame
	  if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;
	  if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
	  if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;
	}

	// Reset the FEC
	std::cout << "Make a FEC reset" << std::endl ;
	fecAccess->fecRingReset (buildFecRingKey (fecAddress, ringAddress)) ;
	std::cout << "Make a PIA reset" << std::endl ;
	// PIA reset
	allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ;

	// reset performed
	resetPerformed = true ;
      }
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  if(!doh && !(dcuF && !fourApvsF)) results.push_back("</Module>");

  try {
    delete apv1 ;
    delete apv2 ;
    delete apv3 ;
    delete apv4 ;
    delete apv5 ;
    delete apv6 ;
    delete mux ;
    delete dcu ;
    delete laserdriver ;
    delete doh ;
    delete pll ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error += 1 ;

    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;
    if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
    if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;
  }

  // Switch off the APV
  //std::cout << "Make a PIA reset on all FEC and all CCUs" << std::endl ;
  // mersi patch
  std::cout << "mersi patch" << std::endl;
  // allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ; // mersi patch

  return (error) ;
}

/** 
 * <p>command: -trackeri2ctest
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param sixApvsF - test 6 APVs ( 0x20, 0x21, 0x22, 0x23, 0x24, 0x25 )
 * \param fourApvsF - test 4 APVs ( 0x20, 0x21, 0x24, 0x25 )
 * \param apvmuxF - test the MUX (0x43)
 * \param dcuF - test the DCU (DCUADDRESS)
 * \param laserdriverF - test the laserdriver (0x60)
 * \param dohF - test the laserdriver (0x70)
 * \param pllF - test the PLL (0x44)
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testTrackerDevicesI2CTest ( FecAccess *fecAccess,
				tscType8 fecAddress, 
				tscType8 ringAddress,
				tscType8 ccuAddress, 
				tscType8 channelAddressSt,
				tscType8 channelAddressEd,
				bool sixApvsF,
				bool fourApvsF,
				bool apvmuxF,
				bool dcuF,
				bool laserdriverF,
				bool dohF,
				bool pllF,
				long loop, unsigned long tms,
				std::vector<std::string> &results) {
  
  bool transitionErrorApv[6] = {false,false,false,false,false,false};
  unsigned int errorCounter[6] = {0} ;
  bool piaResetF = false ;

  //void srandom(unsigned int seed);
  //srandom (time(NULL)) ;

  // Start to make a PIA reset
  //std::cout << "Make a PIA reset on all FEC and all CCUs" << std::endl ;
  allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ;

  int error = 0 ;
  apvAccess *apv1[32] ;
  apvAccess *apv2[32] ;
  apvAccess *apv3[32] ;
  apvAccess *apv4[32] ;
  apvAccess *apv5[32] ;
  apvAccess *apv6[32] ;
  muxAccess *mux[32] ;
  dcuAccess *dcu[32] ;
  laserdriverAccess *laserdriver[32] ;
  DohAccess *doh[32] ;
  pllAccess *pll[32] ;

  for (int i = 0 ; i < 32 ; i ++) {
    apv1[i] = NULL ;
    apv2[i] = NULL ;
    apv3[i] = NULL ;
    apv4[i] = NULL ;
    apv5[i] = NULL ;
    apv6[i] = NULL ;
    mux[i] = NULL ;
    dcu[i] = NULL ;
    laserdriver[i] = NULL ;
    doh[i] = NULL ;
    pll[i] = NULL ;
  }

  try {
    
    for (int channelAddress = channelAddressSt ; channelAddress <= channelAddressEd ; channelAddress ++) {

#define TOBI2CCHANNEL
#ifdef TOBI2CCHANNEL
      if ( (channelAddress == 0x11) || 
	   (channelAddress == 0x13) || 
	   (channelAddress == 0x14) || 
	   (channelAddress == 0x17) || 
	   (channelAddress == 0x19) || 
	   (channelAddress == 0x1A) )
#endif
	{
	  if (sixApvsF || fourApvsF) {

	    apv1[channelAddress] = new apvAccess ( fecAccess,
						   fecAddress,
						   ringAddress,
						   ccuAddress,
						   channelAddress,
						   0x20) ;
	
	    apv2[channelAddress] = new apvAccess ( fecAccess,
						   fecAddress,
						   ringAddress,
						   ccuAddress,
						   channelAddress,
						   0x21) ;
	
	    apv3[channelAddress] = new apvAccess ( fecAccess,
						   fecAddress,
						   ringAddress,
						   ccuAddress,
						   channelAddress,
						   0x24) ;
	
	    apv4[channelAddress] = new apvAccess ( fecAccess,
						   fecAddress,
						   ringAddress,
						   ccuAddress,
						   channelAddress,
						   0x25) ;
	
	    if (sixApvsF) {
	      apv5[channelAddress] = new apvAccess ( fecAccess,
						     fecAddress,
						     ringAddress,
						     ccuAddress,
						     channelAddress,
						     0x22) ;
	  
	      apv6[channelAddress] = new apvAccess ( fecAccess,
						     fecAddress,
						     ringAddress,
						     ccuAddress,
						     channelAddress,
						     0x23) ;
	    }
	  }

	  if (apvmuxF) {
	    mux[channelAddress] = new muxAccess ( fecAccess,
						  fecAddress,
						  ringAddress,
						  ccuAddress,
						  channelAddress,
						  0x43) ;
	  }

	  if (dcuF) {

	    dcu[channelAddress] = new dcuAccess ( fecAccess,
						  fecAddress,
						  ringAddress,
						  ccuAddress,
						  channelAddress,
						  DCUADDRESS) ;
	  }
      
	  if (laserdriverF) {
	
	    laserdriver[channelAddress] = new laserdriverAccess (fecAccess,
								 fecAddress,
								 ringAddress,
								 ccuAddress,
								 channelAddress,
								 0x60) ;
	  }

	  if (dohF) {
	
	    doh[channelAddress] = new DohAccess (fecAccess,
						 fecAddress,
						 ringAddress,
						 ccuAddress,
						 channelAddress,
						 0x70) ;
	  }
      
	  if (pllF) {
	
	    pll[channelAddress] = new pllAccess (fecAccess,
						 fecAddress,
						 ringAddress,
						 ccuAddress,
						 channelAddress,
						 0x44) ;
	  }
	}
    }
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << "Cannot create the access to a device" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
    
    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;
    
    return 1 ;
  }
  
  if(!dohF && !(dcuF && !fourApvsF)) results.push_back("<Module>");

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    //unsigned int randValue = random ( ) ;
    //int selectedValue = randValue % 6 ;
    //std::cout << selectedValue << std::endl ;

    std::cout << (int)channelAddressSt << std::endl ;
    std::cout << (int)channelAddressEd << std::endl ;

    for (int channelAddress = channelAddressSt ; channelAddress <= channelAddressEd ; channelAddress ++) {
#ifdef TOBI2CCHANNEL
      if ( (channelAddress == 0x11) || 
	   (channelAddress == 0x13) || 
	   (channelAddress == 0x14) || 
	   (channelAddress == 0x17) || 
	   (channelAddress == 0x19) || 
	   (channelAddress == 0x1A) )
#endif
	{
	  if (loop != 1)
	    std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

	  try {

	    // JM
	    if(!(!sixApvsF && fourApvsF && !apvmuxF && dcuF && !laserdriverF && !dohF && !pllF)) {

	      // Init the settings value for the apv

	      apvDescription apvd ((tscType8)0x2b,
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
				   (tscType8)0,    // Ispare				   (tscType8)0x43,
				   (tscType8)0x43,
				   (tscType8)0x14,
				   (tscType8)0xFB,
				   (tscType8)0xFE,
				   (tscType8)0) ;
	  
	      if ( (apv1[channelAddress] != NULL) || 
		   (apv2[channelAddress] != NULL) || 
		   (apv3[channelAddress] != NULL) || 
		   (apv4[channelAddress] != NULL) ) std::cout << "Set the APVs" << std::endl ;

	      try {
		// Set the APVs
		if (apv1[channelAddress] != NULL) apv1[channelAddress]->setValues ( apvd );
		transitionErrorApv[0] = false ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the APV to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[0]) {
		  errorCounter[0] ++ ;
		  transitionErrorApv[0] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv2[channelAddress] != NULL) apv2[channelAddress]->setValues ( apvd );
		transitionErrorApv[1] = false ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the APV to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[1]) {
		  errorCounter[1] ++ ;
		  transitionErrorApv[1] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv3[channelAddress] != NULL) apv3[channelAddress]->setValues ( apvd );
		transitionErrorApv[2] = false ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the APV to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[2]) {
		  errorCounter[2] ++ ;
		  transitionErrorApv[2] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv4[channelAddress] != NULL) apv4[channelAddress]->setValues ( apvd );
		transitionErrorApv[3] = false ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the APV to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[3]) {
		  errorCounter[3] ++ ;
		  transitionErrorApv[3] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv5[channelAddress] != NULL) apv5[channelAddress]->setValues ( apvd );
		transitionErrorApv[4] = false ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the APV to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[4]) {
		  errorCounter[4] ++ ;
		  transitionErrorApv[4] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv6[channelAddress] != NULL) apv6[channelAddress]->setValues ( apvd );
		transitionErrorApv[5] = false ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the APV to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[5]) {
		  errorCounter[5] ++ ;
		  transitionErrorApv[5] = true ;
		}
		piaResetF = true ;
	      }
	
	      if ( (apv1[channelAddress] != NULL) || 
		   (apv2[channelAddress] != NULL) || 
		   (apv3[channelAddress] != NULL) || 
		   (apv4[channelAddress] != NULL) ) std::cout << "Apv set !!!" << std::endl ;

	      try {
		// Get their values back
		if (apv1[channelAddress] != NULL) {
		  apvDescription *monApv1 = apv1[channelAddress]->getValues() ;
		  transitionErrorApv[0] = false ;
		  if (*monApv1 == apvd) { // Show debug message
		    std::cout << "Apv 1 Ok" << std::endl ;
		  }
		  else {
		    std::cout << "Apv 1 Error" << std::endl ;
		    if(!dcuF) results.push_back("APV 0x20 Error");
		  }
	  
		  //monApv1->display() ;
		  delete monApv1 ;
		}
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the APV 0x20 *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[0]) {
		  errorCounter[0] ++ ;
		  transitionErrorApv[0] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv2[channelAddress] != NULL) {
		  apvDescription *monApv2 = apv2[channelAddress]->getValues() ;
		  transitionErrorApv[1] = false ;
		  if (*monApv2 == apvd) { // Show debug message
		    std::cout << "Apv 2 Ok" << std::endl ;
		  }
		  else {
		    std::cout << "Apv 2 Error" << std::endl ;
		    if(!dcuF) results.push_back("APV 0x21 Error");
		  }
	  
		  //monApv2->display() ;
		  delete monApv2 ;
		}
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the APV 0x21 *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[1]) {
		  errorCounter[1] ++ ;
		  transitionErrorApv[1] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv3[channelAddress] != NULL) {
		  apvDescription *monApv3 = apv3[channelAddress]->getValues() ;
		  transitionErrorApv[2] = false ;
		  if (*monApv3 == apvd) { // Show debug message
		    std::cout << "Apv 3 Ok" << std::endl ;
		  }
		  else {
		    std::cout << "Apv 3 Error" << std::endl ;
		    if(!dcuF) results.push_back("APV 0x24 Error");
		  }
	  
		  //monApv3->display() ;
		  delete monApv3 ;
		}
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the APV 0x24 *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[2]) {
		  errorCounter[2] ++ ;
		  transitionErrorApv[2] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv4[channelAddress] != NULL) {
		  apvDescription *monApv4 = apv4[channelAddress]->getValues() ;
		  transitionErrorApv[3] = false ;
		  if (*monApv4 == apvd) { // Show debug message
		    std::cout << "Apv 4 Ok" << std::endl ;
		  }
		  else {
		    std::cout << "Apv 4 Error" << std::endl ;
		    if(!dcuF) results.push_back("APV 0x25 Error");
		  }
	    
		  //monApv4->display() ;
		  delete monApv4 ;
		}
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the APV 0x25 *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[4]) {
		  errorCounter[4] ++ ;
		  transitionErrorApv[4] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv5[channelAddress] != NULL) {
		  apvDescription *monApv5 = apv5[channelAddress]->getValues() ;
		  transitionErrorApv[5] = false ;
		  if (*monApv5 == apvd) { // Show debug message
		    std::cout << "Apv 5 Ok" << std::endl ;
		  }
		  else {
		    std::cout << "Apv 5 Error" << std::endl ;
		    if(!dcuF) results.push_back("APV 0x22 Error");
		  }
	  
		  //monApv5->display() ;
		  delete monApv5 ;
		}
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the APV 0x22 *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[4]) {
		  errorCounter[4] ++ ;
		  transitionErrorApv[4] = true ;
		}
		piaResetF = true ;
	      }
	      try {
		if (apv6[channelAddress] != NULL) {
		  apvDescription *monApv6 = apv6[channelAddress]->getValues() ;
		  if (*monApv6 == apvd) { // Show debug message
		    std::cout << "Apv 6 Ok" << std::endl ;
		  }
		  else {
		    std::cout << "Apv 6 Error" << std::endl ;
		    if(!dcuF) results.push_back("APV 0x23 Error");
		  }
	    
		  //monApv6->display() ;
		  delete monApv6 ;
		}
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the APV 0x23 *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
		if (!transitionErrorApv[5]) {
		  errorCounter[5] ++ ;
		  transitionErrorApv[5] = true ;
		}
		piaResetF = true ;
	      }
	    }

	    if (piaResetF) {
	      // Start to make a PIA reset
	      //std::cout << "Make a PIA reset on all FEC and all CCUs" << std::endl ;
	      allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ;
	      piaResetF = false ;

	      for (int i = 0 ; i < 6 ; i ++) transitionErrorApv[i] = false ;
	    }

	    if (mux[channelAddress] != NULL) {
	  
	      muxDescription muxd ((tscType16)0xFF) ;

	      try {
		std::cout << "Set MUX" << std::endl ;
		mux[channelAddress]->setValues (muxd) ; // Set the Mux to the muxd value
		std::cout << "MUX set !!!" << std::endl ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the MUX to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }
	      try {
		muxDescription *monmux = mux[channelAddress]->getValues() ; // Get the Mux value
		if (*monmux == muxd) { // Show debug message
		  std::cout << "Mux Ok" << std::endl ;
		}
		else {
		  std::cout << "Mux Error" << std::endl ;
		  results.push_back("MUX Error");
		}

		//monmux->display() ;
		delete monmux ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the MUX *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "**********************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }
	    }

	    if (dcu[channelAddress] != NULL) {

	      if(fourApvsF) 
		results.push_back("<DCU>");
	      else results.push_back("<DCUCCU>");
	
	      try {
		dcuDescription *mondcu= dcu[channelAddress]->getValues() ;

		results.push_back("   <DcuHardId>" + IntToString(mondcu->getDcuHardId(), 10) + "</DcuHardId>");

		results.push_back("   <Channel0 = " + IntToString(mondcu->getDcuChannel0(),10) + ">");
		results.push_back("   <Channel1 = " + IntToString(mondcu->getDcuChannel1(),10) + ">");
		results.push_back("   <Channel2 = " + IntToString(mondcu->getDcuChannel2(),10) + ">");
		results.push_back("   <Channel3 = " + IntToString(mondcu->getDcuChannel3(),10) + ">");
		results.push_back("   <Channel4 = " + IntToString(mondcu->getDcuChannel4(),10) + ">");
		results.push_back("   <Channel5 = " + IntToString(mondcu->getDcuChannel5(),10) + ">");
		results.push_back("   <Channel6 = " + IntToString(mondcu->getDcuChannel6(),10) + ">");
		results.push_back("   <Channel7 = " + IntToString(mondcu->getDcuChannel7(),10) + ">");
		mondcu->display() ;      
		delete mondcu ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read the DCU *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "*****************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }
	      if(fourApvsF) 
		results.push_back("</DCU>");
	      else results.push_back("</DCUCCU>");
	    }

	    if (laserdriver[channelAddress] != NULL) {

	      // Init the settings value for the laserdriver
	      tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
	      for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
		biasInit[i] = 18 ;
		gainInit[i] = 2  ;
	      }
	      laserdriverDescription ldd (gainInit,biasInit) ;

	      try {
		std::cout << "Set the laserdriver" << std::endl ;
		laserdriver[channelAddress]->setValues (ldd) ;
		std::cout << "Laserdriver set !!!" << std::endl ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the LASERDRIVER to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***********************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }
	      try {
		// Get its value back
		laserdriverDescription *mald = laserdriver[channelAddress]->getValues() ;
		if (*mald == ldd) { // Show debug message
		  std::cout << "Laser driver Ok" << std::endl ;
		}
		else {
		  std::cout << "Laser driver Error" << std::endl ;
		  results.push_back("LASERDRIVER Error");
		}
		//mald->display ( ) ;
		delete mald ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the LASERDRIVER *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "******************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }
	    }

	    if (doh[channelAddress] != NULL) {

	      // Init the settings value for the doh
	      tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
	      for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
		biasInit[i] = 48 ;
		gainInit[i] = 2 ;
	      }
	      laserdriverDescription ldd (gainInit,biasInit) ;
	
	      try {
		std::cout << "Set the doh" << std::endl ;
		doh[channelAddress]->setValues (ldd, true) ;
		std::cout << "Doh set !!!" << std::endl ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the DOH to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }
	      try {
		// Get its value back
		laserdriverDescription *mald = doh[channelAddress]->getValues() ;

		if (*mald == ldd) { // Show debug message
		  std::cout << "DOH Ok" << std::endl ;
		  results.push_back("DOH Ok");
		}
		else {
		  std::cout << "DOH Error" << std::endl ;
		  results.push_back("DOH Error");
		}
	  
		//mald->display ( ) ;
		delete mald ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the DOH *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "**********************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }
	    }

	    if (pll[channelAddress] != NULL) {

	      pllDescription plld ((tscType8)6,(tscType8)1) ;

	      try {
		std::cout << "Set the PLL" << std::endl ;
		pll[channelAddress]->setValues (plld) ;
		std::cout << "PLL set !!!" << std::endl ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR in set the PLL to default values *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "***************************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }

	      try {
		// Get its value back     
		pllDescription *mapll = pll[channelAddress]->getValues() ;
		if (*mapll == plld) { // Show debug message
		  std::cout << "Pll Ok" << std::endl ;
		}
		else {
		  std::cout << "Pll Error" << std::endl ;
		  results.push_back("PLL Error");
		}
		// Show the settings recovered from the pll
		//mapll->display() ;
		delete mapll ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "****** ERROR unable to read back the PLL *****" << std::endl ;
		std::cout << e.what() << std::endl ;
		std::cout << "**********************************************" << std::endl ;
		displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;
	      }
	    }

	    results.push_back("<LOOP>");
	  }

	  catch (FecExceptionHandler &e) {
      
	    std::cout << "------------ Exception ----------" << std::endl ;
	    std::cout << e.what()  << std::endl ;
	    std::cout << "---------------------------------" << std::endl ;
	    displayStatus ( fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress, &e ) ;

	    //       if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;
	    //       error += 1 ;
	    //       if (error > MAXERRORCOUNT) loop = loopI ;
	  }

	  // Wait
	  if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
	}
    }
  }

  if(doh!=0 && !(dcuF && !fourApvsF)) results.push_back("</Module>");

  try {
    for (int channelAddress = channelAddressSt ; channelAddress <= channelAddressEd ; channelAddress ++) {
      delete apv1[channelAddress] ;
      delete apv2[channelAddress] ;
      delete apv3[channelAddress] ;
      delete apv4[channelAddress] ;
      delete apv5[channelAddress] ;
      delete apv6[channelAddress] ;
      delete mux[channelAddress] ;
      delete dcu[channelAddress] ;
      delete laserdriver[channelAddress] ;
      delete doh[channelAddress] ;
      delete pll[channelAddress] ;
    }
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error += 1 ;
  }

  // Switch off the APV
  //std::cout << "Make a PIA reset on all FEC and all CCUs" << std::endl ;
  // allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ; // patch by mersi

  // Display
  std::cout << "Number of error transition" << std::endl ;
  std::cout << "  - APV 0x20: Error = " << errorCounter[0] << std::endl ;
  std::cout << "  - APV 0x21: Error = " << errorCounter[1] << std::endl ;
  std::cout << "  - APV 0x22: Error = " << errorCounter[4] << std::endl ;
  std::cout << "  - APV 0x23: Error = " << errorCounter[5] << std::endl ;
  std::cout << "  - APV 0x24: Error = " << errorCounter[2] << std::endl ;
  std::cout << "  - APV 0x25: Error = " << errorCounter[3] << std::endl ;
    

  return (error) ;
}


/**
 * <p>command: -device dcu
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testDcu ( FecAccess *fecAccess,
              tscType8 fecAddress,
              tscType8 ringAddress,
              tscType8 ccuAddress,
              tscType8 channelAddress,
              tscType8 deviceAddress,
              long loop, unsigned long tms ) {


  int error = 0 ;
  dcuAccess *dcu = NULL;

  try {

    dcu = new dcuAccess  (fecAccess,
                          fecAddress,
                          ringAddress,
                          ccuAddress,
                          channelAddress,
                          deviceAddress) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    return 1 ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      // Hardware id and 0 for the channels
      // dcuDescription dcud (1,0,0,0,0,0,0,0,0) ;

      //dcu->setValues (dcud) ;
      std::cout << std::endl << "Getting all values from channels..." << std::endl ;
      dcuDescription *mondcu= dcu->getValues() ;

      std::cout << "Hard Id   = " << mondcu->getDcuHardId()   << std::endl ; 
      std::cout << "Channel 0 = " << mondcu->getDcuChannel0() << std::endl ;
      std::cout << "Channel 1 = " << mondcu->getDcuChannel1() << std::endl ;
      std::cout << "Channel 2 = " << mondcu->getDcuChannel2() << std::endl ;
      std::cout << "Channel 3 = " << mondcu->getDcuChannel3() << std::endl ;
      std::cout << "Channel 4 = " << mondcu->getDcuChannel4() << std::endl ;
      std::cout << "Channel 5 = " << mondcu->getDcuChannel5() << std::endl ;
      std::cout << "Channel 6 = " << mondcu->getDcuChannel6() << std::endl ;
      std::cout << "Channel 7 = " << mondcu->getDcuChannel7() << std::endl ;
      
      delete mondcu ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      
      error = 1 ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {

    if (dcu != NULL) delete dcu ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
    
    error = 1 ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;
  }

  return (error) ;
}


/**
 * <p>command: -device philips
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testPhilips ( FecAccess *fecAccess,
                  tscType8 fecAddress,
                  tscType8 ringAddress,
                  tscType8 ccuAddress,
                  tscType8 channelAddress,
                  tscType8 deviceAddress,
                  long loop, unsigned long tms ) {

  int error = 0 ;

  philipsAccess *philips = NULL ;

  try {
    philips = new philipsAccess (fecAccess,
                                 fecAddress,
                                 ringAddress,
                                 ccuAddress,
                                 channelAddress,
                                 deviceAddress) ; // Open a philips access
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      philipsDescription philipsd ((tscType8)0xFF) ;
    
      philips->setValues (philipsd) ; // Set the chip philips to philipsd value
    
      philipsDescription *monphilips = philips->getValues() ; // Get the chip philips value
    
      if (*monphilips == philipsd) { // Show debug message
        std::cout << "Philips Ok" << std::endl ;
      }
      else {
        std::cout << "Philips Error" << std::endl ;
      }
    
      // Show the value get
      std::cout << "0x" << std::hex << (int)monphilips->getRegister() << std::endl ;

      delete monphilips ;
    }  
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;      

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {

    if (philips != NULL) delete philips ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error = 1 ;
  }

  return (error) ;
}

#ifdef TOTEM
/**
 * <p>command: -device vfat
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testVFAT ( FecAccess *fecAccess,
	       tscType8 fecAddress,
	       tscType8 ringAddress,
	       tscType8 ccuAddress,
	       tscType8 channelAddress,
	       tscType8 deviceAddress,
	       long loop, unsigned long tms ) {

  int error = 0 ;

  vfatAccess *vfat = NULL ;

  try {
    vfat = new vfatAccess (fecAccess,
			   fecAddress,
			   ringAddress,
			   ccuAddress,
			   channelAddress,
			   deviceAddress) ; // Open a VFAT access
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      vfatDescription vfatd ;  // Create description
      vfatd.setDescriptionDefaultValues() ; // Set default values ( Values are defined in vfatDefinition.h )
      vfat->setVfatValues (vfatd) ; // Set the chip vfat to vfatd values
      vfatDescription *monvfat = vfat->getVfatValues() ; // Get the chip vfat values

      // Show debug message
      if (*monvfat == vfatd) { std::cout << "Vfat Ok" << std::endl ; }
      else { std::cout << "Vfat Error" << std::endl ; }
    
      // Show the register values in the vfat chip
      monvfat->display();

      delete monvfat ;
    }
 
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {

    if (vfat != NULL) delete vfat ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error = 1 ;
  }

  return (error) ;
}

/**
 * <p>command: -device cchip
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testTotemCChip ( FecAccess *fecAccess,
		     tscType8 fecAddress,
		     tscType8 ringAddress,
		     tscType8 ccuAddress,
		     tscType8 channelAddress,
		     tscType8 deviceAddress,
		     long loop, unsigned long tms ) {

  int error = 0 ;

  totemCChipAccess *cc = NULL ;

  try {
    cc = new totemCChipAccess ( fecAccess,
				fecAddress,
				ringAddress,
				ccuAddress,
				channelAddress,
				deviceAddress ) ; // Open a CCHIP access
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      // Create description and set values
      totemCChipDescription ccd ((tscType8)0x55, // Control0
				 (tscType8)0x55, // Control1
				 (tscType8)0x55, // Control2
				 (tscType8)0x55, // Control3
				 (tscType8)0x55, // Control4
				 (tscType8)0x00, // Chip ID0 (RO)
				 (tscType8)0x00, // Chip ID1 (RO)
				 (tscType8)0x00, // Counter0 (RO)
				 (tscType8)0x00, // Counter1 (RO)
				 (tscType8)0x00, // Counter2 (RO)
				 (tscType8)0x55, // Mask0
				 (tscType8)0x55, // Mask1
				 (tscType8)0x55, // Mask2
				 (tscType8)0x55, // Mask3
				 (tscType8)0x55, // Mask4
				 (tscType8)0x55, // Mask5
				 (tscType8)0x55, // Mask6
				 (tscType8)0x55, // Mask7
				 (tscType8)0x55, // Mask8
				 (tscType8)0x55, // Mask9
				 (tscType8)0x55); // Resistor
      
      cc->setValues (ccd) ; // Set the chip cc to ccd values
      totemCChipDescription *moncc = cc->getValues() ; // Get the chip cc values

      // Show debug message
      if (*moncc == ccd) { std::cout << "CC Ok" << std::endl ; }
      else { std::cout << "CC Error" << std::endl ; }
    
      // Show the register values in the cchip
      moncc->display();

      delete moncc ;
    }
 
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {

    if (cc != NULL) delete cc ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error = 1 ;
  }

  return (error) ;
}

/**
 * <p>command: -device tbb
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testTotemBB ( FecAccess *fecAccess,
		  tscType8 fecAddress,
		  tscType8 ringAddress,
		  tscType8 ccuAddress,
		  tscType8 channelAddress,
		  tscType8 deviceAddress,
		  long loop, unsigned long tms ) {

  int error = 0 ;
  totemBBAccess *tbb = NULL ;

  try {

    tbb = new totemBBAccess ( fecAccess,
			      fecAddress,
			      ringAddress,
			      ccuAddress,
			      channelAddress,
			      deviceAddress ) ; // Open a TBB access
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      // Create description and set values
      totemBBDescription tbbd ( (tscType8)0x1, // Control
				(tscType8)0x0, // Latency1
				(tscType8)0x5, // Latency2
				(tscType8)0x0, // Repeats1
				(tscType8)0x5 ) ; // Repeats2

      
      tbb->setValues (tbbd) ; // Set the tbb to tbbd values
      totemBBDescription *montbb = tbb->getValues() ; // Get the tbb values

      // Show debug message
      if (*montbb == tbbd) { std::cout << "TBB Ok" << std::endl ; }
      else { std::cout << "TBB Error" << std::endl ; }
    
      // Show the register values in the tbb
      montbb->display();

      delete montbb ;
    }
 
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {

    if (tbb != NULL) delete tbb ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error = 1 ;
  }

  return (error) ;
}
#endif

/** 
 * <p>Command: -i2creg
 * \param fecAccess - FEC Access object
 * \param fecAddress - slot of the FEC.
 * \param ringAddress - slot of the ring.
 * \param ccuAddress - address of the first CCU of the ring.
 * \param channelAddress - address of the channel.
 * \param deviceAddress - address of the device.
 * \param value - value to be written.
 * \param modeType - type of device to access.
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
void testI2CRegister ( FecAccess *fecAccess,
		       tscType8 fecAddress,
		       tscType8 ringAddress,
		       tscType8 ccuAddress,
		       tscType8 channelAddress,
		       tscType8 deviceAddress,
		       tscType8 value,
		       enumDeviceType modeType,
		       long loop, unsigned long tms ) {

  //unsigned int value ;
  keyType index = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,deviceAddress) ; 

  try {

    fecAccess->addi2cAccess (index, 
                             modeType,
                             MODE_SHARE) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1){ std::cout << "----------------------- Loop " << loopI+1 << std::endl ; }

    try {

      std::cout << "Writing 0x" << std::hex << (int)value << " to the device..." << std::endl ;
      fecAccess->write(index, value) ;
      std::cout << "Check... => new value of the device : 0x" << std::hex << (int)fecAccess->read(index) << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;

      // display all registers
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;
      if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
      if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }
}

/**
 * <p>command: -device mux 
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testMux ( FecAccess *fecAccess,
              tscType8 fecAddress,
              tscType8 ringAddress,
              tscType8 ccuAddress,
              tscType8 channelAddress,
              tscType8 deviceAddress,
              long loop, unsigned long tms ) {

  int error = 0 ;
  muxAccess *mux = NULL ;
  try {

    mux = new muxAccess ( fecAccess,
                          fecAddress,
                          ringAddress,
                          ccuAddress,
                          channelAddress,
                          deviceAddress) ; // Open a mux access
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {
      muxDescription muxd ((tscType16)0xFF) ;

      mux->setValues (muxd) ; // Set the Mux to the muxd value

      muxDescription *monmux = mux->getValues() ; // Get the Mux value

      if (*monmux == muxd) { // Show debug message
        std::cout << "Mux Ok" << std::endl ;
      }
      else {
        std::cout << "Mux Error" << std::endl ;
      }
      
      std::cout << monmux->getResistor() << std::endl ; // Show the value get

      delete monmux ;
    }


    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;      

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {

    if (mux != NULL) delete mux ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error = 1 ;
  }

  return (error) ;
}

/**
 * <p>command: -device apv
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int test4Apvs ( FecAccess *fecAccess,
                tscType8 fecAddress,
                tscType8 ringAddress,
                tscType8 ccuAddress,
                tscType8 channelAddress,
                tscType8 deviceAddress1,
                tscType8 deviceAddress2,
                tscType8 deviceAddress3,
                tscType8 deviceAddress4,
                long loop, unsigned long tms ) {

  int error = 0 ;
  apvAccess *apv1 = NULL ;
  apvAccess *apv2 = NULL ;
  apvAccess *apv3 = NULL ;
  apvAccess *apv4 = NULL ;

  try {

    // Open 4 APV access
    apv1 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress1) ;
    
    apv2 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress2) ;
    
    apv3 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress3) ;

    apv4 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress4) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
       
    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;
    
    try { 
      // Init the settings value for the apv
      apvDescription apvd ((tscType8)0x2b,
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
      
      //     apvDescription apvd1 (
      // 			  0x0b,
      // 			  0x64,
      // 			  0x2,
      // 			  115,
      // 			  60,
      // 			  50,
      // 			  90,
      // 			  50,
      // 			  80,
      // 			  50,
      // 			  0x50,
      //        0,    // Ispare
      // 			  67,
      // 			  70,
      // 			  40,
      // 			  0xFB,
      // 			  0xFE,
      // 			  0,
      // 			  ) ;

      std::cout << "Set the 4 APVs" << std::endl ;

      // Set the APVs
      apv1->setValues ( apvd ); 
      apv2->setValues ( apvd ); 
      apv3->setValues ( apvd ); 
      apv4->setValues ( apvd ); 
      
      std::cout << "Apv set !!!" << std::endl ;

      // Get their values back
      apvDescription *monApv1 = apv1->getValues() ;
      apvDescription *monApv2 = apv2->getValues() ;
      apvDescription *monApv3 = apv3->getValues() ;
      apvDescription *monApv4 = apv4->getValues() ;
      
      if (*monApv1 == apvd) { // Show debug message
        std::cout << "Apv Ok" << std::endl ;
      }
      else {
        std::cout << "Apv Error" << std::endl ;
}

      // Show the settings recovered from the APVs
      monApv1->display() ;
      monApv2->display() ;
      monApv3->display() ;
      monApv4->display() ;

      delete monApv1 ;
      delete monApv2 ;
      delete monApv3 ;
      delete monApv4 ;
    }

    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      
      error = 1 ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {

    if (apv1 != NULL) delete apv1 ;
    if (apv2 != NULL) delete apv2 ;
    if (apv3 != NULL) delete apv3 ;
    if (apv4 != NULL) delete apv4 ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error = 1 ;
  }

  return (error) ;
}

/** 
 * <p>command: -device apv6
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int test6Apvs ( FecAccess *fecAccess,
                tscType8 fecAddress, 
                tscType8 ringAddress,
                tscType8 ccuAddress, 
                tscType8 channelAddress,
                tscType8 deviceAddress1,
                tscType8 deviceAddress2,
                tscType8 deviceAddress3,
                tscType8 deviceAddress4,
                tscType8 deviceAddress5,
                tscType8 deviceAddress6,
                long loop, unsigned long tms ) {

  int error = 0 ;
  apvAccess *apv1 = NULL ;
  apvAccess *apv2 = NULL ;
  apvAccess *apv3 = NULL ;
  apvAccess *apv4 = NULL ;
  apvAccess *apv5 = NULL ;
  apvAccess *apv6 = NULL ;

  try {
    
    // Open 4 APV access
    apv1 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress1) ;

    apv2 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress2) ;

    apv3 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress3) ;
     
    apv4 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress4) ;

    apv5 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress5) ;

    apv6 = new apvAccess ( fecAccess,
                           fecAddress,
                           ringAddress,
                           ccuAddress,
                           channelAddress,
                           deviceAddress6) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      // Init the settings value for the apv
      apvDescription apvd ((tscType8)0x2b,
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
      
      //     apvDescription apvd1 (
      // 			  (tscType8)0x0b,
      // 			  (tscType8)0x64,
      // 			  (tscType8)0x2,
      // 			  (tscType8)115,
      // 			  (tscType8)60,
      // 			  (tscType8)50,
      // 			  (tscType8)90,
      // 			  (tscType8)50,
      // 			  (tscType8)80,
      // 			  (tscType8)50,
      // 			  (tscType8)0x50,
      //                          (tscType8)0,
      // 			  (tscType8)67,
      // 			  (tscType8)70,
      // 			  (tscType8)40,
      // 			  (tscType8)0xFB,
      // 			  (tscType8)0xFE,
      // 			  (tscType8)0
      // 			  ) ;

      std::cout << "Set the 4 APVs" << std::endl ;
      
      // Set the APVs
      apv1->setValues ( apvd );
      apv2->setValues ( apvd );
      apv3->setValues ( apvd );
      apv4->setValues ( apvd );
      apv5->setValues ( apvd );
      apv6->setValues ( apvd );

      std::cout << "Apv set !!!" << std::endl ;

      // Get their values back
      apvDescription *monApv1 = apv1->getValues() ;
      apvDescription *monApv2 = apv2->getValues() ;
      apvDescription *monApv3 = apv3->getValues() ;
      apvDescription *monApv4 = apv4->getValues() ;
      apvDescription *monApv5 = apv5->getValues() ;
      apvDescription *monApv6 = apv6->getValues() ;

      if (*monApv1 == apvd) { // Show debug message
        std::cout << "Apv Ok" << std::endl ;
      }
      else {
        std::cout << "Apv Error" << std::endl ;
      }

      // Show the settings recovered from the APVs
      monApv1->display() ;
      monApv2->display() ;
      monApv3->display() ;
      monApv4->display() ;
      monApv5->display() ;
      monApv6->display() ;
      
      delete monApv1 ;
      delete monApv2 ;
      delete monApv3 ;
      delete monApv4 ;
      delete monApv5 ;
      delete monApv6 ;
    }

    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;    

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {
    delete apv1 ;
    delete apv2 ;
    delete apv3 ;
    delete apv4 ;
    delete apv5 ;
    delete apv6 ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    error = 1 ;
  }

  return (error) ;
}

/** 
 * <p>command: -device pll
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testPll ( FecAccess *fecAccess,
              tscType8 fecAddress, 
              tscType8 ringAddress,
              tscType8 ccuAddress, 
              tscType8 channelAddress,
              tscType8 deviceAddress,
              long loop, unsigned long tms ) {

  int error = 0 ;
  pllAccess *pll ;

  try {

    pll = new pllAccess ( fecAccess,
                          fecAddress,
                          ringAddress,
                          ccuAddress,
                          channelAddress,
                          deviceAddress) ; // Open a pll access
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      // Init the settings value for the pll
      pllDescription plld ((tscType8)1,(tscType8)10) ;
//       pllDescription plld ((tscType8)0,(tscType8)0) ;
//       // Set the pll 
//       pll->setValues (plld) ;
//       std::cout << "Reset" << std::endl ;
//       getchar() ;
//       pll->pllReset() ;
//       std::cout << "cold init" << std::endl ;
//       getchar() ;
//       pll->pllInit() ;
//       getchar() ;

      // Get its value back     
      pllDescription *mapll = pll->getValues() ;

      if (*mapll == plld) { // Show debug message
        std::cout << "Pll Ok" << std::endl ;
      }
      else {
        std::cout << "Pll Error" << std::endl ;
      }
      
      // Show the settings recovered from the pll
      mapll->display() ;
    
      delete mapll ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;      

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  try {
    delete pll ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return 1 ;
  }

  return (error) ;
}

/** Display the binary
 */
std::string displayBinary ( unsigned int value, int nBits ) {

  std::ostringstream valString ;
  int val[100] = {0} ;

  for (int i = 0 ; i < nBits ; i ++) {
    if (value & (1 << i)) val[i] = 1 ;
  }

  for (int i = (nBits-1) ; i >= 0 ; i --) {
    valString << val[i] ;
  }

  return valString.str() ;
}

/**
 * <p> command: -pllCheckLock
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 */
int pllLockCheck ( FecAccess *fecAccess,
		   tscType8 fecAddress, 
		   tscType8 ringAddress,
		   tscType8 ccuAddress, 
		   tscType8 channelAddress,
		   tscType8 deviceAddress,
		   long loop, unsigned long tms ) {

  // Make a PIA reset to detect all the devices
  //std::cout << "Make a PIA reset on all FEC and all CCUs" << std::endl ;
  //allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ;

  // Find the FECs
  try {
    // Scan the PCI slot for device driver loaded
    fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
  }
  catch (FecExceptionHandler &e) {  
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot find the FEC " << std::dec << (int)fecAddress << " ring " << (int)ringAddress << ": " << e.what() << std::endl ; 
    std::cerr << "**************************************************" << std::endl ;
  }

  try {
    keyType indexPll = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,0x44) ;
    pllAccess pll(fecAccess,indexPll) ;
    char msg[80] ; decodeKey(msg,indexPll) ;
    std::cout << "Accessing the PLL on " << msg << std::endl ;
    for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

      if (loop != 1)
	std::cout << "----------------------- Loop " << std::dec << loopI+1 << std::endl ;
      //pll.setCNTRL1(0x80);
      //pll.pllInit() ;
      //for (int i = 0 ; i <= 0xF ; i ++) {

      pll.setDelayFine(13) ;
      pll.setDelayCoarse(3) ;

      std::cout << "PLL 1 = " << std::hex << (int)pll.getCNTRL1() << " (" << displayBinary(pll.getCNTRL1(),8) << ")" << std::endl ;
      std::cout << "PLL 2 = " << std::hex << (int)pll.getCNTRL2() << " (" << displayBinary(pll.getCNTRL2(),8) << ")"  << std::endl ;
      std::cout << "PLL 3 = " << std::hex << (int)pll.getCNTRL3() << " (" << displayBinary(pll.getCNTRL3(),8) << ")" << std::endl ;
      std::cout << "PLL 4 = " << std::hex << (int)pll.getCNTRL4() << " (" << displayBinary(pll.getCNTRL4(),8) << ")" << std::endl ;
      std::cout << "PLL 5 = " << std::hex << (int)pll.getCNTRL5() << " (" << displayBinary(pll.getCNTRL5(),8) << ")" << std::endl ;

      pll.displayPllRegisters() ;
      
      //std::cout << "Writing value in delay fine = " << std::hex << (int)pll.getClockPhaseMap(0xa) << std::endl ;
      //pll.setDelayFine(0xa) ;
      //std::cout << "Writing value in delay fine = " << std::hex << (int)pll.getTriggerDelayMap(i) << std::endl ;
      //pll.setDelayCoarse(i) ;
      // 	pll.pllInit() ;
      
// 	ctrl2 = pll.getCNTRL2() ;
// 	std::cout << "PLL 1 = " << std::hex << (int)pll.getCNTRL1() << std::endl ;
// 	std::cout << "PLL 2 = " << std::hex << (int)ctrl2 << std::endl ;
// 	std::cout << "PLL 3 = " << std::hex << (int)pll.getCNTRL3() << std::endl ;
// 	pll.setCNTRL2(ctrl2 & 0xDF) ;
// 	//1101 1111
// 	std::cout << "PLL 4 = " << std::hex << (int)pll.getCNTRL4() << std::endl ;
// 	pll.setCNTRL2(ctrl2 | 0x20) ;
// 	std::cout << "PLL 5 = " << std::hex << (int)pll.getCNTRL4() << std::endl ;
// 	pll.setCNTRL2(ctrl2) ;

	getchar() ;
	//}

      // Wait
      if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
    }
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
  }

  return 0 ;

  //keyType indexFecRing = buildFecRingKey(fecAddress,ringAddress) ;
  int sizeValues = 1 ;
  keyType
    deviceValues[1][2] = { 
      {0x44, NORMALMODE} // PLL
    } ;

  try {
    // false => use broadcast
    // true  => display
    //std::list<keyType> *deviceList = fecAccess->scanRingForI2CDevice ( indexFecRing, (keyType *)deviceValues, sizeValues, false, true ) ;
    std::list<keyType> *deviceList = fecAccess->scanRingForI2CDevice ( (keyType *)deviceValues, sizeValues, false, true ) ;

    //std::list<keyType> *deviceList = new std::list<keyType> ;
    //deviceList->push_back (buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,0x44)) ;

    if (deviceList != NULL) {
      deviceList->sort() ;

      for (std::list<keyType>::iterator itPll = deviceList->begin() ; itPll != deviceList->end() ; itPll++ ) {

	keyType indexPll = *itPll ;
	try {
	  pllAccess pll(fecAccess,indexPll) ;

	  // V4
// 	  char msg[80] ; decodeKey(msg,indexPll) ;
// 	  std::cout << "----------------------------------------------------" << std::endl ;
// 	  std::cout << "(V3) Set the PLL " << msg << std::endl ;
// 	  pll.setCNTRL1(0x80);
// 	  pll.setCNTRL2(0xC0);
// 	  pll.setCNTRL1(0x84);
// 	  pll.setCNTRL3(0xFF);
// 	  pll.setCNTRL4(0x00);
// 	  usleep(5000) ;
// 	  pll.setCNTRL2(0x80);
// 	  pll.setCNTRL1(0x00);
// 	  std::cout << "(V3) PLL " << msg << " set" << std::endl ;

	  // V6
	  pll.setCNTRL1(0x80);
	  pll.setCNTRL2(0x00);
	  pll.setCNTRL3(0xFF);
	  pll.setCNTRL4(0x00);
	  usleep(5000) ;
	  pll.setCNTRL2(0x80);

// 	  // ?????????? wb reg 2 reading added 
// 	  std::cout << "Found a PLL on position " << msg << std::endl ;
// 	  std::cout << "\t" << "PLL CTRL2 = 0x" << std::hex << (int)pll.getCNTRL2() <<std::endl;
// 	  std::cout << "\t" << "Delay Fine   = " << (int)pll.getClockPhase() << std::endl ;
// 	  std::cout << "\t" << "Delay Coarse = " << (int)pll.getTriggerDelay() << std::endl ;
// // 	  if (pll.getGoingStatus()) std::cout << "\t" << "PLL locked" << std::endl ;
// // 	  else {
// // 	    std::cout << "\t" << "PLL NOT LOCKED" << std::endl ;
// // 	    pll.pllReset() ;
// // 	    if (pll.getGoingStatus()) std::cout << "\t" << "After simple reset: PLL locked" << std::endl ;
// // 	    else {
// // 	      std::cout << "\t" << "After simple reset: PLL NOT LOCKED" << std::endl ;
// // 	      tscType8 ctrl1 = pll.pllInit() ;
// // 	      if (pll.getGoingStatus()) std::cout << "\t" << "After cold reset: PLL locked" << std::endl ;
// // 	      else std::cout << "\t" << "After cold reset: PLL NOT LOCKED (ctrl1 = 0x" << std::hex << ctrl1 << ")" << std::endl ;
// // 	    }
// // 	  }

// 	  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

// 	    if (loop != 1)
// 	      std::cout << "----------------------- Loop " << std::dec << loopI+1 << std::endl ;

// 	    //std::cout << "Check if the PLL has locked" << std::endl ;
// 	    //std::cout << "Press enter to continue" ; getchar() ;
// 	    if (pll.getGoingStatus()) std::cout << "\t" << "PLL locked (CTRL1 = 0x" << std::hex << (int)pll.getCNTRL1() << ")" << std::endl ;
// 	    else std::cout << "\t" << "PLL NOT LOCKED (CTRL1 = 0x" << std::hex << (int)pll.getCNTRL1() << ")" << std::endl ;
// 	    // ???? wb added 
// 	    std::cout << "\t" << "PLL CTRL2 = 0x" << std::hex << (int)pll.getCNTRL2() <<std::endl;
// 	    //std::cout << "Force the simple PLL reset and check if the PLL has locked" << std::endl ;
// 	    //std::cout << "Press enter to continue" ; getchar() ;
// 	    pll.pllReset() ;
// 	    // ???????? wb added reading of REG 2 , 3, 4  readings  and sleep 
// 	    getchar();
// 	    if (pll.getGoingStatus()) {
// 	      std::cout << "\t" << "PLL locked after a simple reset (CTRL1 = 0x" << std::hex << (int)pll.getCNTRL1() << ")" << std::endl ;
// 	      tscType8 pll_ctrl2 =pll.getCNTRL2();
// 	      std::cout << "\t" << "PLL CTRL2 = 0x" << std::hex << (int)pll_ctrl2 <<std::endl;
// 	      pll_ctrl2 &= ~PLL_CTRL2_RSEL; 
// 	      pll.setCNTRL2(pll_ctrl2 ); 
// 	      std::cout << "\t" << "PLL CTRL2 = 0x" << std::hex << (int)pll.getCNTRL2() <<std::endl;
// 	      std::cout << "\t" << "PLL CTRL4.3  = 0x" << std::hex << (int)pll.getCNTRL4()<<"."<< (int)pll.getCNTRL3() <<std::endl;
// 	    }
// 	    else std::cout << "\t" << "PLL NOT LOCKED after a simple reset (CTRL1 = 0x" << std::hex << (int)pll.getCNTRL1() << ")" << std::endl ;	    
// 	    //std::cout << "Force the cold PLL init and check if the PLL has locked" << std::endl ;
// 	    //std::cout << "Press enter to continue" ; getchar() ;
	  
// 	    // -----------------------------------------------
// 	    // VERSION   4 
// 	    //pll.pllInit() ;
// 	    // "Special sequence"
// 	    // 1 - write 0x80 to reg 1
// // 	     pll.setCNTRL1(PLL_CTRL1_MODE);
// // 	    // 2 - write 0xC0 to reg 2
// // 	    pll.setCNTRL2(PLL_CTRL2_I2CGOING | PLL_CTRL2_I2CHIGHGAIN);
// // 	    // 3 - write 0x84 to reg 1
// // 	    pll.setCNTRL1(PLL_CTRL1_MODE | PLL_CTRL1_FORCEPFD);
// // 	    //pll.setCNTRL1(PLL_CTRL1_MODE);
// // 	    // 0xFF
// // 	    // tscType8 dacbyte=0xFF & plldaq ;
// // 	    // 4 - write 0xFF to reg 3
// // 	    pll.setCNTRL3(0xFF);
// // 	    // 5 - write 0x00 to reg 4
// // 	    pll.setCNTRL4(0x00);
// // 	    // Wait for 5000 ms
// // 	    usleep (PLLDELAY) ;
// // 	    // 6 - write 0x80 to reg 2
// // 	    pll.setCNTRL2(PLL_CTRL2_I2CGOING);
// // 	    //??????wb keep high gain 
// // 	    // pll.setCNTRL2(PLL_CTRL2_I2CGOING | PLL_CTRL2_I2CHIGHGAIN);
// // 	    // 7 - write 0x80 to reg 1 
// // 	    pll.setCNTRL1(PLL_CTRL1_MODE);
    
// // 	    // Wait for 5000 us
// // 	    usleep (PLLDELAY) ;
// 	    std::cout << "Pll cold init low gain "<<std::endl;
// 	    // VERSION 5 
// 	    // 1 - write 0x80 to reg 1
// 	    pll.setCNTRL1(PLL_CTRL1_MODE);
// 	    // 2 - write 0x40 to reg 2  set high gain 
// 	    pll.setCNTRL2( PLL_CTRL2_I2CHIGHGAIN);
// 	    // ver 5.a no high gain 
// 	    pll.setCNTRL2( 0);
// 	    // 3 - write 0x84 to reg 1  skip for ver 5 
// 	    // pll.setCNTRL1(PLL_CTRL1_MODE | PLL_CTRL1_FORCEPFD);
// 	    //pll.setCNTRL1(PLL_CTRL1_MODE);
// 	    // 0xFF
// 	    // tscType8 dacbyte=0xFF & plldaq ;
// 	    // 4 - write 0xFF to reg 3
// 	    pll.setCNTRL3(0xFF);
// 	    // 5 - write 0x00 to reg 4
// 	    pll.setCNTRL4(0x00);
// 	    // Wait for 5000 ms
// 	    usleep (PLLDELAY) ;
// 	    // 6 - write 0x80 to reg 2
// 	    // pll.setCNTRL2(PLL_CTRL2_I2CGOING| PLL_CTRL2_I2CHIGHGAIN  );
// 	    // ver 5a  no high gain 
// 	     pll.setCNTRL2(PLL_CTRL2_I2CGOING  );
// 	    // 7 - write 0x80 to reg 1  still needed ? 
// 	    // pll.setCNTRL1(PLL_CTRL1_MODE);
    
// 	    // Wait for 5000 us
// 	    usleep (PLLDELAY) ;


// //  	    std::cout << "Before PFD" ; getchar() ;
// //  	    pll.setCNTRL1(PLL_CTRL1_MODE | PLL_CTRL1_FORCEPFD);
// //  	    std::cout << "PFD Set" ; getchar() ;
// //      	    pll.setCNTRL1(PLL_CTRL1_MODE);
// //  	    std::cout << "Reset PFD" ; getchar() ;

// 	    // Wait for the GOING bit for normal operation
// 	    //tscType8 cntrl1 = getCNTRL1 ( ) ; 
// 	    // -----------------------------------------------

// 	    if (pll.getGoingStatus()) std::cout << "\t" << "PLL locked after the cold PLL init (CTRL1 = 0x" << std::hex << (int)pll.getCNTRL1() << ")" << std::endl ;
// 	    else std::cout << "\t" << "PLL NOT LOCKED after the cold PLL init (CTRL1 = 0x" << std::hex << (int)pll.getCNTRL1() << ")" << std::endl ;
// 	    std::cout << "\t" << "PLL CTRL2 = 0x" << std::hex << (int)pll.getCNTRL2() <<std::endl;
// 	    getchar();
// // 	    for ( tscType8  fine=0;fine < 24 ;fine++){
// // 	      pll.setDelayFine(fine);
// // 	      getchar();
// // 	    }
	    
// 	  }

	}
	catch (FecExceptionHandler &e) {
	  
	  std::cout << "PLL access problem: " << e.what()  << std::endl ;
	}
      }
    }
    else {
      std::cerr << "No PLL was detected on FEC " << std::dec << fecAddress << "." << ringAddress << std::endl ;
    }
  } 
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }

  return 0 ;
}

/** 
 * <p>command: -device laserdriver
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testLaserdriver ( FecAccess *fecAccess,
                      tscType8 fecAddress, 
                      tscType8 ringAddress,
                      tscType8 ccuAddress, 
                      tscType8 channelAddress,
                      tscType8 deviceAddress,
                      long loop, unsigned long tms,
		      bool readOnly ) {

  int error = 0 ;

  laserdriverAccess *ld = NULL ;
  laserdriverDescription *mald = NULL ;

  try {

    ld = new laserdriverAccess ( fecAccess,
                                 fecAddress,
                                 ringAddress,
                                 ccuAddress,
                                 channelAddress,
                                 deviceAddress) ; // Open a laserdriber access
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {
      // Init the settings value for the laserdriver
      tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
      for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
        biasInit[i] = 18 ;
        gainInit[i] = 2 ;
      }

      //std::cout << MAXLASERDRIVERCHANNELS << std::endl ;

      // Set the laserdriver
      laserdriverDescription ldd (gainInit,biasInit) ;
     
      if (!readOnly) {
	ld->setValues (ldd) ;
	std::cout << "Write ok ..." << std::endl ;
      }

      // Get its value back
      mald = ld->getValues() ;

      if (!readOnly) {
	if (*mald == ldd) { // Show debug message
	  std::cout << "Laser driver Ok" << std::endl ;
	}
	else {
	  std::cout << "Laser driver Error" << std::endl ;
	}
      }

      // Show the settings recovered from the laserdriver
      std::cout << "Gain for all channels = 0x" << std::hex << (int)mald->getGain() << std::endl ;

      tscType8 bias[MAXLASERDRIVERCHANNELS] ;
      mald->getBias (bias) ;

      for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) 
        std::cout << "Bias chan " << i << " = 0x" << std::hex << (int)bias[i] << std::endl ;

      delete mald ;
    }

    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;      

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }


  try {
    if (ld != NULL) delete ld ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    return 1 ;
  }

  return (error) ;
}

/** 
 * <p>command: -device doh
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testDoh ( FecAccess *fecAccess,
                      tscType8 fecAddress, 
                      tscType8 ringAddress,
                      tscType8 ccuAddress, 
                      tscType8 channelAddress,
                      tscType8 deviceAddress,
                      long loop, unsigned long tms ) {

  int error = 0 ;

  DohAccess *doh = NULL ;
  laserdriverDescription *mald = NULL ;

  try {

    doh = new DohAccess ( fecAccess,
                                 fecAddress,
                                 ringAddress,
                                 ccuAddress,
                                 channelAddress,
                                 deviceAddress) ; // Open a laserdriber access
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {
      // Init the settings value for the doh
      tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
      for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
        biasInit[i] = 48 ;
        gainInit[i] = 2 ;
      }

      //std::cout << MAXLASERDRIVERCHANNELS << std::endl ;

      // Set the doh
      laserdriverDescription ldd (gainInit,biasInit) ;
     
      doh->setValues (ldd, true) ;
   
      std::cout << "Write ok ..." << std::endl ;

      // Get its value back
      mald = doh->getValues() ;

      if (*mald == ldd) { // Show debug message
        std::cout << "Laser driver Ok" << std::endl ;
      }
      else {
        std::cout << "Laser driver Error" << std::endl ;
      }

      // Show the settings recovered from the laserdriver
      std::cout << "Gain for all channels = 0x" << std::hex << (int)doh->getGain() << std::endl ;

      tscType8 bias[MAXLASERDRIVERCHANNELS] ;
      mald->getBias (bias) ;

      for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) 
        std::cout << "Bias chan " << i << " = 0x" << std::hex << (int)bias[i] << std::endl ;

      delete mald ;
    }

    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;      

      error = 1 ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }


  try {
    if (doh != NULL) delete doh ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;    

    return 1 ;
  }

  return (error) ;
}

/** The following will be performed
 * <ul>
 * <li> Write bias on the laserdriver specified
 * </ul>
 * <p>command: -lasergain
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 */
int testLaserGain (FecAccess *fecAccess,
		   tscType8 fecAddress, 
		   tscType8 ringAddress,
		   tscType8 ccuAddress, 
		   tscType8 channelAddress,
		   long loop, unsigned long tms) {

  laserdriverAccess *laserdriver = NULL;
  tscType8 gain = 1 ;

  // Recover the system if it is not correct
  // testRecover (fecAccess,fecAddress,ringAddress) ;

  // Start the test
  try {
    laserdriver = new laserdriverAccess (fecAccess,
					 fecAddress,
					 ringAddress,
					 ccuAddress,
					 channelAddress,
					 0x60) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      std::cout << "Set the Gain of the laserdriver = " << (int)gain << std::endl ;
      laserdriver->setGain(gain) ;
      //std::cout << "Get the gain of the laserdriver = 0x" << std::hex << (int)laserdriver->getGain() << std::endl ;
   
    }
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception: set laserdriver  ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------------------------" << std::endl ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }
  
  try {
    if (laserdriver != NULL) delete laserdriver ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }

  return 0 ;
}

/** The following will be performed
 * <ul>
 * <li> Set the APV mode to 0x2B
 * <li> loop
 * <ol>
 *   <li> Set the gain of the AOH to 1
 *   <li> Get the APV mode
 * </ol>
 * <li> loop
 * <ol>
 *   <li> Set the APV with default values
 *   <li> Get the APV and make comparison
 *   <li> Set the Laserdriver 1,25
 *   <li> Get the APV and make comparison
 * </ol>
 * </ul>
 * <p>command: -apvlasertest
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 */
int testApvLaser (FecAccess *fecAccess,
		  tscType8 fecAddress, 
		  tscType8 ringAddress,
		  tscType8 ccuAddress, 
		  tscType8 channelAddress,
		  long loop, unsigned long tms) {

  int error = 0 ;
  apvAccess *apv1 = NULL ;
  laserdriverAccess *laserdriver = NULL;
  tscType8 gain = 1, bias = 25 ;
  tscType8 mode = 0x2b ;

  // Recover the system if it is not correct
  testRecover (fecAccess,fecAddress,ringAddress) ;

  // PIA reset
  unsigned int  piaResetValue = 0xFF ;
  unsigned long delayActiveReset = 10 ;
  unsigned long intervalDelayReset = 10000 ;
  unsigned int  piaChannel = 0x30 ;
  testPIAResetfunctions (fecAccess,
			 fecAddress,
			 ringAddress,
			 ccuAddress,
			 piaChannel,
			 piaResetValue,
			 delayActiveReset,
			 intervalDelayReset, 1, 0) ;

  // Start the test
  try {

    apv1 = new apvAccess ( fecAccess,
			   fecAddress,
			   ringAddress,
			   ccuAddress,
			   channelAddress,
                           0x20) ;
    //apv1->setLatency(100) ;
    apv1->setApvMode(mode) ;

    laserdriver = new laserdriverAccess (fecAccess,
					 fecAddress,
					 ringAddress,
					 ccuAddress,
					 channelAddress,
					 0x60) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    return 1 ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      std::cout << "Set the Gain of the laserdriver = " << (int)gain << std::endl ;
      laserdriver->setGain(gain) ;
      //std::cout << "Get the gain of the laserdriver = 0x" << std::hex << (int)laserdriver->getGain() << std::endl ;
   
    }
    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception: set laserdriver  ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------------------------" << std::endl ;
      
      error += 1 ;
    }

    if (! noGetchar_) getchar() ;

    for (int i = 0 ; i < 1 ; i ++) {

      try {
	std::cout << "Get the mode of the APV = " << (int)apv1->getApvMode () << std::endl ;
	//std::cout << "Get the latency of the APV = " << (int)apv1->getLatency () << std::endl ; 
      }
      catch (FecExceptionHandler &e) {
      
	std::cout << "------------ Exception: get APV  ----------" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "-------------------------------------------" << std::endl ;
	
	error =+ 1 ;
      }

      if (! noGetchar_) getchar() ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }
  
  try {

    if (apv1 != NULL) delete apv1 ;
    if (laserdriver != NULL) delete laserdriver ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }


  // ---------------------------------------------------------------------------
  if (error) {
    std::cout << "Detect " << error << " during the first tests" << std::endl ;
    error = 0 ;

    getchar() ;
  }
  // ---------------------------------------------------------------------------

#define WRITEAPV
#define READAPV
#define WRITELASERDRIVER
  //#define READLASERDRIVER

  //apvAccess *apv1 = NULL ;
  apvAccess *apv2 = NULL ;
  apvAccess *apv3 = NULL ;
  apvAccess *apv4 = NULL ;
  apvAccess *apv5 = NULL ;
  apvAccess *apv6 = NULL ;
  //laserdriverAccess *laserdriver = NULL;
  bool sixApvsF = false ;
  bool errorAPV1 = false, errorAPV2 = false, errorAPV3 = false, errorAPV4 = false, errorAPV5 = false, errorAPV6 = false ;

  try {
    apv1 = new apvAccess ( fecAccess,
			   fecAddress,
			   ringAddress,
			   ccuAddress,
			   channelAddress,
			   0x20) ;

    apv2 = new apvAccess ( fecAccess,
			   fecAddress,
			   ringAddress,
			   ccuAddress,
			   channelAddress,
			   0x21) ;
  
    apv3 = new apvAccess ( fecAccess,
			   fecAddress,
			   ringAddress,
			   ccuAddress,
			   channelAddress,
			   0x24) ;
    
    apv4 = new apvAccess ( fecAccess,
			   fecAddress,
			   ringAddress,
			   ccuAddress,
			   channelAddress,
			   0x25) ;

    if (sixApvsF) {
      apv5 = new apvAccess ( fecAccess,
			     fecAddress,
			     ringAddress,
			     ccuAddress,
			     channelAddress,
			     0x22) ;
      
      apv6 = new apvAccess ( fecAccess,
			     fecAddress,
			     ringAddress,
			     ccuAddress,
			     channelAddress,
			     0x23) ;
    }

    laserdriver = new laserdriverAccess (fecAccess,
					 fecAddress,
					 ringAddress,
					 ccuAddress,
					 channelAddress,
					 0x60) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    return 1 ;
  }

  apvDescription apvd ((tscType8)15,
		       (tscType8)100,
		       (tscType8)4,
		       (tscType8)98,
		       (tscType8)52,
		       (tscType8)34,
		       (tscType8)34,
		       (tscType8)55,
		       (tscType8)34,
		       (tscType8)29,
		       (tscType8)30,
		       (tscType8)0,    // Ispare
		       (tscType8)30,
		       (tscType8)60,
		       (tscType8)36,
		       (tscType8)255,
		       (tscType8)127,
		       (tscType8)0) ;
  
  // Init the settings value for the laserdriver
  tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    
    biasInit[i] = gain ;
    gainInit[i] = bias ;
  }
  laserdriverDescription ldd (gainInit,biasInit) ;
  
  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

#ifdef WRITEAPV
    std::cout << "Set the APVs" << std::endl ;
    // Set the APVs
    try {
      if (apv1 != NULL) apv1->setValues ( apvd );
      if (errorAPV1) {
	std::cout << "APV 1 recovered" << std::endl ;
	errorAPV1 = false ;
      }
    }
    catch (FecExceptionHandler &e) {
      error += 1 ;
      errorAPV1 = true ; 
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      if (! noGetchar_) getchar() ;
    }
    try {
      if (apv2 != NULL) apv2->setValues ( apvd );
      if (errorAPV2) {
	std::cout << "APV 2 recovered" << std::endl ;
	errorAPV2 = false ;
      }
    }
    catch (FecExceptionHandler &e) {
      error += 1 ;
      errorAPV2 = true ; 
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      if (! noGetchar_) getchar() ;
    }
    try {
      if (apv3 != NULL) apv3->setValues ( apvd );
      if (errorAPV3) {
	std::cout << "APV 3 recovered" << std::endl ;
	errorAPV3 = false ;
      }
    }
    catch (FecExceptionHandler &e) {
      error += 1 ;
      errorAPV3 = true ; 
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      if (! noGetchar_) getchar() ;
    }
    try {
      if (apv4 != NULL) apv4->setValues ( apvd );
      if (errorAPV4) {
	std::cout << "APV 4 recovered" << std::endl ;
	errorAPV4 = false ;
      }
    }
    catch (FecExceptionHandler &e) {
      error += 1 ;
      errorAPV4 = true ; 
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      if (! noGetchar_) getchar() ;
    }
    try {
      if (apv5 != NULL) apv5->setValues ( apvd );
      if (errorAPV5) {
	std::cout << "APV 5 recovered" << std::endl ;
	errorAPV5 = false ;
      }
    }
    catch (FecExceptionHandler &e) {
      error += 1 ;
      errorAPV5 = true ; 
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      if (! noGetchar_) getchar() ;
    }
    try {
      if (apv6 != NULL) apv6->setValues ( apvd );
      if (errorAPV6) {
	std::cout << "APV 6 recovered" << std::endl ;
	errorAPV6 = false ;
      }
    }
    catch (FecExceptionHandler &e) {
      error += 1 ;
      errorAPV6 = true ; 
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      if (! noGetchar_) getchar() ;
    }
	
    if ( (apv1 != NULL) || 
	 (apv2 != NULL) || 
	 (apv3 != NULL) || 
	 (apv4 != NULL) ||
	 (apv5 != NULL) ||
	 (apv6 != NULL) ) std::cout << "Apv set !!!" << std::endl ;
#endif // WRITEAPV

#ifdef READAPV
    // Get their values back
    if (apv1 != NULL) {
      try {
	apvDescription *monApv1 = apv1->getValues() ;
	if (*monApv1 == apvd) { // Show debug message
	  std::cout << "Apv 1 Ok Read" << std::endl ;
	  if (errorAPV1) {
	    std::cout << "APV 1 recovered" << std::endl ;
	    errorAPV1 = false ;
	  }
	}
	else {
	  std::cout << "Apv 1 Error" << std::endl ;
	}
	//monApv1->display() ;
	delete monApv1 ;
      }
      catch (FecExceptionHandler &e) {
	error += 1 ;
	errorAPV1 = true ;
	std::cout << "------------ Exception ----------" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "---------------------------------" << std::endl ;
	if (! noGetchar_) getchar() ;
      }	  
    }
    if (apv2 != NULL) {
      try {
	apvDescription *monApv2 = apv2->getValues() ;
	if (*monApv2 == apvd) { // Show debug message
	  std::cout << "Apv 2 Ok Read" << std::endl ;
	  if (errorAPV2) {
	    std::cout << "APV 2 recovered" << std::endl ;
	    errorAPV2 = false ;
	  }
	}
	else {
	  std::cout << "Apv 2 Error" << std::endl ;
	}
	//monApv2->display() ;
	delete monApv2 ;
      }
      catch (FecExceptionHandler &e) {
	error += 1 ;
	errorAPV2 = true ;
	std::cout << "------------ Exception ----------" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "---------------------------------" << std::endl ;
	if (! noGetchar_) getchar() ;
      }
    }
    if (apv3 != NULL) {
      try {
	apvDescription *monApv3 = apv3->getValues() ;
	if (*monApv3 == apvd) { // Show debug message
	  std::cout << "Apv 3 Ok Read" << std::endl ;
	  if (errorAPV3) {
	    std::cout << "APV 3 recovered" << std::endl ;
	    errorAPV3 = false ;
	  }
	}
	else {
	  std::cout << "Apv 3 Error" << std::endl ;
	}
	//monApv3->display() ;
	delete monApv3 ;
      }
      catch (FecExceptionHandler &e) {
	error += 1 ;
	errorAPV3 = true ;
	std::cout << "------------ Exception ----------" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "---------------------------------" << std::endl ;
	if (! noGetchar_) getchar() ;
      }
    }
    if (apv4 != NULL) {
      try {
	apvDescription *monApv4 = apv4->getValues() ;
	if (*monApv4 == apvd) { // Show debug message
	  std::cout << "Apv 4 Ok Read" << std::endl ;
	  if (errorAPV4) {
	    std::cout << "APV 4 recovered" << std::endl ;
	    errorAPV4 = false ;
	  }
	}
	else {
	  std::cout << "Apv 4 Error" << std::endl ;
	}
	//monApv4->display() ;
	delete monApv4 ;
      }
      catch (FecExceptionHandler &e) {
	error += 1 ;
	errorAPV4 = true ;
	std::cout << "------------ Exception ----------" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "---------------------------------" << std::endl ;
	if (! noGetchar_) getchar() ;
      }
    }
    if (apv5 != NULL) {
      try {
	apvDescription *monApv5 = apv5->getValues() ;
	if (*monApv5 == apvd) { // Show debug message
	  std::cout << "Apv 5 Ok Read" << std::endl ;
	  if (errorAPV5) {
	    std::cout << "APV 5 recovered" << std::endl ;
	    errorAPV5 = false ;
	  }
	}
	else {
	  std::cout << "Apv 5 Error" << std::endl ;
	}
	//monApv5->display() ;
	delete monApv5 ;
      }
      catch (FecExceptionHandler &e) {
	error += 1 ;
	errorAPV5 = true ;
	std::cout << "------------ Exception ----------" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "---------------------------------" << std::endl ;
	if (! noGetchar_) getchar() ;
      }
    }
    if (apv6 != NULL) {
      try {
	apvDescription *monApv6 = apv6->getValues() ;
	if (*monApv6 == apvd) { // Show debug message
	  std::cout << "Apv 6 Ok Read" << std::endl ;
	  if (errorAPV6) {
	    std::cout << "APV 6 recovered" << std::endl ;
	    errorAPV6 = false ;
	  }
	}
	else {
	  std::cout << "Apv 6 Error" << std::endl ;
	}
	//monApv6->display() ;
	delete monApv6 ;
      }
      catch (FecExceptionHandler &e) {
	error += 1 ;
	errorAPV6 = true ;
	std::cout << "------------ Exception ----------" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "---------------------------------" << std::endl ;
	if (! noGetchar_) getchar() ;
      }
    }
#endif // READAPV

  if (laserdriver != NULL) {
	
#ifdef WRITELASERDRIVER
    try {
      std::cout << "Set the laserdriver bias " << bias  <<" gain " << gain << std::endl ;
      laserdriver->setValues (ldd) ;
      std::cout << "Laserdriver set !!!" << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      error += 1 ;
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      if (! noGetchar_) getchar() ;
    }
#endif
#ifdef READLASERDRIVER
    try {
      // Get its value back
      laserdriverDescription *mald = laserdriver->getValues() ;

      if (*mald == ldd) { // Show debug message
	std::cout << "Laser driver Ok" << std::endl ;
      }
      else {
	std::cout << "Laser driver Error" << std::endl ;
      }
    
      mald->display ( ) ;
      delete mald ;
    }
    catch (FecExceptionHandler &e) {
      error += 1 ;
      errorAPV6 = true ;
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;
      if (! noGetchar_) getchar() ;
    }
#endif
  }
  }

  // ---------------------------------------------------------------------------
  if (error) {
    std::cout << "Detect " << error << " during the first tests" << std::endl ;
    getchar() ;
  }
  // ---------------------------------------------------------------------------

  return (error) ;
}

/** 
 * <p>command: -calibrateDOH
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param channelAddress - I2C channel number
 * \param deviceAddress - I2C device address
 */
int calibrateDoh ( FecAccess *fecAccess,
		   tscType8 fecAddress, 
		   tscType8 ringAddress,
		   tscType8 ccuAddress, 
		   tscType8 channelAddress,
		   tscType8 deviceAddress ) {

  unsigned char gain = 21 ; // 3 gain: XX XX XX => 01 0101 => 1+4+16

  DohAccess *doh = NULL ;
  keyType index = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,deviceAddress) ;
  laserdriverDescription *mald = NULL ;

  try {
    
    doh = new DohAccess ( fecAccess,
			  fecAddress,
			  ringAddress,
			  ccuAddress,
			  channelAddress,
			  deviceAddress) ; // Open a laserdriver access


    std::cout << "--------------------------------------------------------" << std::endl ;
    try {
      //                                                debug message
      //                                                      gain
      //                                                         min bias value
      laserdriverDescription *dohd1 = doh->calibrateDOH(true, 1, 10) ;
      dohd1->display() ;
    }
    catch (FecExceptionHandler &e) {
      std::cout << "DOH calibration " << e.what() << std::endl ;
    }
    try {
      delete doh ;
    }
    catch (FecExceptionHandler &e) {
      std::cout << "Delete DOH " << e.what() << std::endl ;
    }
    std::cout << "--------------------------------------------------------" << std::endl ;
    std::cout << "End of the procedure encoded in DohAccess, starting custom procedure" << std::endl ;
    if (! noGetchar_) {
      std::cout << "Press <enter> to continue" << std::endl ;
      getchar() ;    
    }

    doh = new DohAccess ( fecAccess,
			  fecAddress,
			  ringAddress,
			  ccuAddress,
			  channelAddress,
			  deviceAddress) ; // Open a laserdriver access


    std::cout << "Set the gain to 0x" << std::hex << (int)gain << std::endl ;
    doh->setGain(gain) ;

    index = doh->getKey() ;
    
    mald = doh->getValues( ) ;
    std::cout << "Value read back from the DOH" << std::endl ;
    std::cout << "Channel\t1\t2\t3" << std::endl ;
    std::cout << "Bias   \t" << std::dec
	 << (int)mald->getBias0() << "\t" 
	 << (int)mald->getBias1() << "\t" 
	 << (int)mald->getBias2() << std::endl ;
    std::cout << "Gain   \t" << std::dec
	 << (int)mald->getGain0() << "\t" 
	 << (int)mald->getGain1() << "\t" 
	 << (int)mald->getGain2() << std::endl ;
    delete mald ;

    if (! noGetchar_) {
      std::cout << "Press <enter> to continue" << std::endl ;
      getchar() ;    
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
    
    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;
    
    return 1 ;
  }

  // Read CR0
  tscType16 fecSR0 = fecAccess->getFecRingSR0(doh->getKey()) ;    
  std::cout << "Status register 0 of the FEC " << fecAddress << " ring " << ringAddress << ": " << std::hex << fecSR0 << std::endl ;

  if (! isFecSR0Correct(fecSR0)) {
    
    std::cerr << "FEC status is not correct, recover the ring" << std::endl ;
    testRecover ( fecAccess, fecAddress, ringAddress ) ;
    
    fecSR0 = fecAccess->getFecRingSR0(doh->getKey()) ;
    if (! isFecSR0Correct(fecSR0)) {
      std::cerr << "Cannot recover ring => exiting" << std::endl ;
      return -1 ;
    }
  }

  tscType8 biasToBeSet[MAXLASERDRIVERCHANNELS] ;
  tscType8 gainT[MAXLASERDRIVERCHANNELS] ;

  fecSR0 = fecAccess->getFecRingSR0(doh->getKey()) ;      
  while (isFecSR0Correct(fecSR0)) {

    try {
      // Get the original values and display it
      mald = doh->getValues() ;
      for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
	gainT[i] = mald->getGain(i) ;
	biasToBeSet[i] = mald->getBias(i) ;
      }
      delete mald ;
      
      std::cout << "Channel\t1\t2\t3" << std::endl ;
      std::cout << "Bias   \t" << std::dec
	   << (int)biasToBeSet[0] << "\t"
	   << (int)biasToBeSet[1] << "\t" 
	   << (int)biasToBeSet[2] << std::endl ;
      std::cout << "Gain   \t" << std::dec 
	   << (int)gainT[0] << "\t" 
	   << (int)gainT[1] << "\t" 
	   << (int)gainT[2] << std::endl ;

      // Save the bias
      for (uint i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
	biasToBeSet[i] -- ;
      }

      // Set the bias
      doh->setBias (biasToBeSet) ;

      // Get the original values and display it
      mald = doh->getValues() ;
      for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
	gainT[i] = mald->getGain(i) ;
	biasToBeSet[i] = mald->getBias(i) ;
      }
      delete mald ;

      fecSR0 = fecAccess->getFecRingSR0(doh->getKey()) ;    
      std::cout << "Status register 0 of the FEC " << fecAddress << " ring " << ringAddress << ": " << std::hex << fecSR0 << std::endl ;
    }
    catch (FecExceptionHandler &e) {
      
      fecSR0 = fecAccess->getFecRingSR0(doh->getKey()) ;    
      std::cout << "-------------------------------------> End calibration: the ring is lost (SR0 = 0x" << std::hex << fecSR0 << ")" << std::endl ;
    }
  }

  try {
    delete doh ;
    doh = NULL ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Error during the delete of the DOH: " << e.what() << std::endl ;
  } 

  try {
    fecAccess->fecRingReset(index) ;
    unsigned int timeout = 0 ;
    while (!isFecSR0Correct(fecAccess->getFecRingSR0(index)) && timeout++ < 100) {
      std::cerr << "Bad SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(index) << std::endl ;
      sleep(1) ;
    }
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Error during the reset of the FEC: " << e.what() << std::endl ;
  }
  
  try {
    fecSR0 = fecAccess->getFecRingSR0(index) ;
    std::cout << "-------------------------------------> After reset: SR0 = 0x" << std::hex << fecSR0 << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Error during the read of the SR0: " << e.what() << std::endl ;
  }

  // After a reset the device must be reconnecte
  try {
    doh = new DohAccess ( fecAccess,
			  fecAddress,
			  ringAddress,
			  ccuAddress,
			  channelAddress,
			  deviceAddress) ; // Open a laserdriver access

//     displayCCUCRE(fecAccess->getCcuCRE(index)) ;

//     // Get the original values and display it
//     mald = doh->getValues() ;
//     std::cout << "Gain: " 
// 	      << std::dec << (int)mald->getGain(0) << " " 
// 	      << std::dec << (int)mald->getGain(1) << " " 
// 	      << std::dec << (int)mald->getGain(2) << std::endl ;
//     std::cout << "Gain: " 
// 	      << std::dec << (int)mald->getBias((unsigned char)0) << " " 
// 	      << std::dec << (int)mald->getBias((unsigned char)1) << " " 
// 	      << std::dec << (int)mald->getBias((unsigned char)2) << std::endl ;
//     delete mald ;
//     getchar() ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << "Error during the reconnection of the DOH: " ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    delete doh ;

    return -1 ;
  }

  // New Settings
  for (uint i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
    biasToBeSet[i] = biasToBeSet[i] + DOHSTEP ;
  }
  laserdriverDescription ldd(gain, biasToBeSet) ;

  std::cout << "Bias to be set" << std::endl ;
  std::cout << "Channel\t1\t2\t3" << std::endl ;
  std::cout << "Bias   \t" << std::dec
	    << (int)ldd.getBias0() << "\t"
	    << (int)ldd.getBias1() << "\t" 
	    << (int)ldd.getBias2() << std::endl ;
  std::cout << "Gain   \t" << std::dec 
	    << (int)ldd.getGain0() << "\t" 
	    << (int)ldd.getGain1() << "\t" 
	    << (int)ldd.getGain2() << std::endl ;


  if (! noGetchar_) {
    std::cout << "Press <enter> to continue" << std::endl ;
    getchar() ;
  }

  try {

    // set the bias
    //doh->setBias (biasToBeSet) ;
    doh->setValues(ldd) ;

    // Get the original values and display it
    mald = doh->getValues() ;
    for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
      gainT[i] = mald->getGain(i) ;
      biasToBeSet[i] = mald->getBias(i) ;
    }
    delete mald ;
      
    std::cout << "Channel\t1\t2\t3" << std::endl ;
    std::cout << "Bias   \t" << std::dec
	      << (int)biasToBeSet[0] << "\t"
	      << (int)biasToBeSet[1] << "\t" 
	      << (int)biasToBeSet[2] << std::endl ;
    std::cout << "Gain   \t" << std::dec 
	      << (int)gainT[0] << "\t" 
	      << (int)gainT[1] << "\t" 
	      << (int)gainT[2] << std::endl ;
  }
  catch (FecExceptionHandler &e) {

    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << "Error during the settings of the DOH: " ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }
  
  try {
    delete doh ;
  }
  catch (FecExceptionHandler &e) {

    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }  

  std::cout << "-------------------------------------> End of the procedure: SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(index) << std::endl ;

  return (isFecSR0Correct(fecAccess->getFecRingSR0(index))) ;
}

		   
/**
 * This method try to find all the device driver loaded for FECs and scan the ring
 * for each FEC to find all the CCUs.
 * <p>command: -scanccu (in place of -scanring)
 * \param fecAccess - FEC access object
 * \param noBroadcast - CCU broadcast mode used (false) or not (true) [OPTION parameter]
 * \warning the loop is not used
 */
void testScanCCU ( FecAccess *fecAccess, 
		   tscType8 fecAddress,
		   tscType8 ringAddress,
		   bool noBroadcast ) {

  std::list<keyType> *ccuList = NULL ;
  try {

    // Scan the ring for each FECs and display the CCUs
    ccuList = fecAccess->getCcuList ( buildFecRingKey(fecAddress,ringAddress), noBroadcast ) ;

    if (ccuList != NULL) {

      for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
      
	keyType index = *p ;
	
	std::cout << std::hex << "FEC 0x" << getFecKey(index) 
		  << " Ring 0x " << getRingKey(index) 
		  << " CCU 0x" << getCcuKey(index)
		  << " found" << std::endl ;
      }
    }
    else {
      std::cout << "No CCU found on FECs" << std::endl ;
    }  
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Delete = already store in the table of FEC device class
  delete ccuList ;
}

/** 
 * This method scan the ring for device Tracker APV (6), MUX, PLL, DCU, LASERDRIVER, DOH.
 * <p>command: -scantrackerdevice (in place of -scanringdevice)
 * <p>command: -calibrateI2CSpeed test the i2c speed for the devices
 * \param fecAddress - FEC address
 * \warning the test is read and try to read back the force acknowledge for the base address
 * in the rigth mode. Not all the registers are accessed.
 * \warning the loop is not used
 */
std::vector<std::string> testScanTrackerDevice ( FecAccess *fecAccess, 
						 tscType8 fecAddress,
						 tscType8 ringAddress,
						 long loop, unsigned long tms, 
						 bool onlyDCU,
						 bool testI2CSpeed ) { 

  std::vector<std::string> results;

  // Make a PIA reset to detect all the devices
  //std::cout << "Make a PIA reset on all FEC and all CCUs" << std::endl ;
  allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ;

  FecRingDevice *fecDevice = NULL ;
  
  // Find the FECs
  try {
    // Scan the PCI slot for device driver loaded
    fecDevice = fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
  }
  catch (FecExceptionHandler &e) {  
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot find the FEC " << std::dec << (int)fecAddress << " ring " << (int)ringAddress << ": " << e.what() << std::endl ; 
    std::cerr << "**************************************************" << std::endl ;
    
    return results ;
  }

  int sizeValues = 11 ;
  keyType
    deviceValues[11][2] = { 
      {DCUADDRESS, NORMALMODE}, // DCU
      {0x20, RALMODE   }, // APV
      {0x21, RALMODE   }, // APV
      {0x22, RALMODE   }, // APV
      {0x23, RALMODE   }, // APV
      {0x24, RALMODE   }, // APV
      {0x25, RALMODE   }, // APV
      {0x43, RALMODE   }, // MUX
      {0x60, NORMALMODE}, // Laserdriver
      {0x70, NORMALMODE}, // DOH
      {0x44, NORMALMODE} // PLL
    } ;

  if (onlyDCU) sizeValues = 1 ;

  //std::cout << "Scan ring for CCU and try to find devices" << std::endl ;
  //for (int i = 0 ; i < sizeValues ; i ++) {
  //  std::cout << "Device " << i+1 << " => address 0x" << std::hex << deviceValues[i][0] << std::endl ;
  //}
  apvAccessedType apvSet ;
  pllAccessedType pllSet ;
  laserdriverAccessedType laserdriverSet ;
  dohAccessedType dohSet ;
  muxAccessedType muxSet ;
  dcuAccessedType dcuSet ;

  // Test error on the scan ring
  try {

    for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

      if (loop != 1)
	std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

      
      // false => use broadcast
      // true  => display
      std::list<keyType> *deviceList = fecDevice->scanRingForI2CDevice ( (keyType *)deviceValues, sizeValues, false, true ) ;
      Sgi::hash_map<keyType, bool *> trackerModule ;

      if (deviceList != NULL) {
	deviceList->sort() ;
	bool *module ;

	for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) {
      
	  keyType index = *p ;
   
	  char msg[100] ;
	  decodeKey (msg, index) ;

	  keyType indexChannel = buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),getChannelKey(index),0) ;
	  if (trackerModule.find(indexChannel) == trackerModule.end()) {
	    trackerModule[indexChannel] = new bool[12] ;
	    module = trackerModule[indexChannel] ;
	    for (int i = 0 ; i < 12 ; i ++) module[i] = false ;
	  }
	  else module = trackerModule[indexChannel] ;

	  switch (getAddressKey(index)) {
	  case DCUADDRESS:
	    try {
	      dcuSet[index] = new dcuAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[10] = true ;
	    std::cout << "DCU " << msg << " found" << std::endl ;
	    break ;
	  case 0x20:
	    try {
	      apvSet[index] = new apvAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[0] = true ;
	    std::cout << "APV " << msg << " found" << std::endl ;
	    break ;
	  case 0x21:
	    try {
	      apvSet[index] = new apvAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[1] = true ;
	    std::cout << "APV " << msg << " found" << std::endl ;
	    break ;
	  case 0x22:
	    try {
	      apvSet[index] = new apvAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[2] = true; 
	    std::cout << "APV " << msg << " found" << std::endl ;
	    break ;
	  case 0x23:
	    try {
	      apvSet[index] = new apvAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[3] = true ;
	    std::cout << "APV " << msg << " found" << std::endl ;
	    break ;
	  case 0x24:
	    try {
	      apvSet[index] = new apvAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[4] = true; 
	    std::cout << "APV " << msg << " found" << std::endl ;
	    break ;
	  case 0x25:
	    try {
	      apvSet[index] = new apvAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[5] = true ;
	    std::cout << "APV " << msg << " found" << std::endl ;
	    break ;
	  case 0x43:
	    try {
	      muxSet[index] = new muxAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[6] = true ;
	    std::cout << "MUX " << msg << " found" << std::endl ;
	    break ;
	  case 0x44:
	    try {
	      pllSet[index] = new pllAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[7] = true ;
	    std::cout << "PLL " << msg << " found" << std::endl ;
	    break ;
	  case 0x60:
	    try {
	      laserdriverSet[index] = new laserdriverAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[8] = true ;
	    std::cout << "AOH " << msg << " found" << std::endl ;
	    break ;
	  case 0x70:
	    try {
	      dohSet[index] = new DohAccess (fecAccess, index) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Cannot create the access to a device" << std::endl ;
	      std::cout << e.what() << std::endl ;
	    }
	    module[9] = true ;
	    std::cout << "DOH " << msg << " found" << std::endl ;
	    break ;
	  }
	}
	    
	// Message
	if (! noGetchar_) {
	  std::cout << "Press <Enter> to continue" ;
	  getchar() ;
	}

	tscType16 currentCcuAddress = 0;

	for (Sgi::hash_map<keyType, bool *>::iterator p=trackerModule.begin();p!=trackerModule.end();p++) {

	  keyType indexF = p->first ;
	  module = p->second ;
	  if (module != NULL) {
	  
	    // 4 APVs
	    bool apv4F = module[0] && module[1] && module[4] && module[5] ;
	    bool pllF  = module[7] ;
	    bool muxF  = module[6] ;
	    bool aohF  = module[8] ;
	    bool dohF  = module[9] ;
	    bool dcu0F  = module[10] ;
	    bool dcu78F = module[11] ;

	    bool apv6F  = false ;
	    if (module[2] || module[3]) apv6F = true ;
	    
	    if(currentCcuAddress == 0) {
	      currentCcuAddress = getCcuKey(indexF);
	      results.push_back("<CCU = " + IntToString(currentCcuAddress) + ">");
	    }

	    if(currentCcuAddress != getCcuKey(indexF)) {
	      results.push_back("</CCU>");
	      currentCcuAddress = getCcuKey(indexF);
	      results.push_back("<CCU = " + IntToString(currentCcuAddress) + ">");
	    }
	    
	    // Is it a module
	    if (apv6F || apv4F || pllF || muxF || aohF) {
	      std::cout << "Found a module on FEC " << std::dec << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << std::hex << getCcuKey(indexF) << " channel 0x" << std::hex << getChannelKey(indexF) << std::endl ;

	      results.push_back("   <Module = " + IntToString(getChannelKey(indexF))+ ">");

	      // Check each part
	      if (!module[0]) {
		std::cerr << "\t" << "Miss an APV at address 0x20" << std::endl ;
		results.push_back("      <APV 0x20 missing>");
	      }
	      if (!module[1]) {
		std::cerr << "\t" << "Miss an APV at address 0x21" << std::endl ;
		results.push_back("      <APV 0x21 missing>");
	      }
	      if (apv6F && !module[2]) {
		std::cerr << "\t" << "Miss an APV at address 0x22" << std::endl ;
		results.push_back("      <APV 0x22 missing>");
	      }
	      if (apv6F && !module[3]) {
		std::cerr << "\t" << "Miss an APV at address 0x23" << std::endl ;
		results.push_back("      <APV 0x23 missing>");
	      }
	      if(!apv6F) {
		results.push_back("      <APV 0x22 not detected>");
		results.push_back("      <APV 0x23 not detected>");
	      }
	      if (!module[4]) {
		std::cerr << "\t" << "Miss an APV at address 0x24" << std::endl ;
		results.push_back("      <APV 0x24 missing>");
	      }
	      if (!module[5]) {
		std::cerr << "\t" << "Miss an APV at address 0x25" << std::endl ;
		results.push_back("      <APV 0x25 missing>");
	      }
	      if (!muxF) {
		std::cerr << "\t" << "Miss a  MUX at address 0x43" << std::endl ;
		results.push_back("      <MUX 0x43 missing>");
	      }
	      if (!pllF) {
		std::cerr << "\t" << "Miss a  PLL at address 0x44" << std::endl ;
		results.push_back("      <PLL 0x44 missing>");
	      }
	      if (!aohF) {
		std::cerr << "\t" << "Miss an AOH at address 0x60" << std::endl ;
		results.push_back("      <AOH 0x60 missing>");
	      }
	      if (!dcu0F) {
		std::cerr << "\t" << "Miss a  DCU at address 0x0" << std::endl ;
		results.push_back("      <DCU 0x0 missing>");
	      }

	      if (testI2CSpeed) {

		// Parameters to be downloaded and uploaded
		apvDescription apvD ((tscType8)0x2b,(tscType8)0x64,(tscType8)0x4,(tscType8)0x73,(tscType8)0x3c,(tscType8)0x32,(tscType8)0x32,(tscType8)0x32,(tscType8)0x50,(tscType8)0x32,(tscType8)0x50,(tscType8)0,(tscType8)0x43,(tscType8)0x43,(tscType8)0x14,(tscType8)0xFB,(tscType8)0xFE,(tscType8)0), *apvU ;
		muxDescription muxD ((tscType16)0xFF), *muxU ;
		pllDescription pllD ((tscType8)6,(tscType8)1), *pllU ;
		tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
		for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
		  biasInit[i] = 18 ;
		  gainInit[i] = 2  ;
		}
		laserdriverDescription aohD (gainInit,biasInit), *aohU ;
		dcuDescription *dcuU ;
	      
		tscType16 i2cSpeed = 100, i2cSpeedOpti[12] = {0} ;

		while (i2cSpeed <= 1000) {

		  tscType8 channelCRA = 0 ;
		  try {
		    // Set the i2cSpeed at the correct value
		    channelCRA = fecAccess->geti2cChannelCRA (indexF) ;
		    channelCRA &= 0xFC ;
		    switch (i2cSpeed) {
		    case 100: channelCRA |= 0x0 ; break ;
		    case 200: channelCRA |= 0x1 ; break ;
		    case 400: channelCRA |= 0x2 ; break ;
		    case 1000: channelCRA |= 0x3 ; break;
		    }
		    //std::cout << "Set the i2c channel to speed " << std::dec << i2cSpeed << std::endl ;
		    fecAccess->seti2cChannelCRA (indexF,channelCRA) ;
		  }
		  catch (FecExceptionHandler &e) {
		    std::cerr << "Cannot retreive the CRA of i2c channel: " << e.what() << std::endl ;
		  }

		  for (int i = 0 ; i < 6 ; i ++) {
		    // Check each part
		    if (module[i]) {
		      try {
			//std::cout << "APV " << (0x20 + i) << std::endl ;

			// create the access
			apvAccess *device = apvSet[indexF | setAddressKey((0x20+i))] ;

			device->setValues (apvD) ;
			apvU = device->getValues ( ) ;
			delete apvU ;

			// Ok for this speed
			i2cSpeedOpti[i] = i2cSpeed ;
		      }
		      catch (FecExceptionHandler &e) {
			if (i2cSpeed == 100) {
			  char msg[100] ;
			  decodeKey(msg, indexF | setAddressKey((0x20+i))) ;
			  std::cerr << "\t\tAPV 0x" << std::hex << (0x20+i) << ": Inconsistency error at 100 kHz => cannot download/upload it" << std::endl ;

			  results.push_back("      <i2cSpeed APV 0x" + IntToString(20+i) + " = error>");
			}
		      }
		      //std::cout << "\tAPV 0x" << std::hex << (0x20+i) << ": I2C speed " << std::dec << i2cSpeedOpti[i] << " supported" << std::endl ;
		    }
		  }

		  if (muxF) {
		    try {
		      //std::cout << "MUX " << 0x43 << std::endl ;

		      // create the access
		      muxAccess *device = muxSet[indexF | setAddressKey(0x43)] ;
		      device->setValues (muxD) ;
		      muxU = device->getValues ( ) ;
		      delete muxU ;

		      // Ok for this speed
		      i2cSpeedOpti[6] = i2cSpeed ;
		    }
		    catch (FecExceptionHandler &e) {
		      if (i2cSpeed == 100) {
			char msg[100] ;
			decodeKey(msg, indexF | setAddressKey(0x43)) ;
			std::cerr << "\t\tMUX 0x" << std::hex << 0x43 << ": Inconsistency error at 100 kHz => cannot download/upload it" << std::endl ;

			results.push_back("      <i2cSpeed MUX 0x43 = error>");
		      }
		    }
		    //std::cout << "\tMUX 0x" << std::hex << 0x43 << ": I2C speed " << std::dec << i2cSpeedOpti[6] << " supported" << std::endl ;
		  }

		  if (pllF) {
		    try {
		      //std::cout << "PLL " << 0x44 << std::endl ;

		      // create the access
		      pllAccess *device = pllSet[indexF | setAddressKey(0x44)] ;
		      device->setValues (pllD) ;
		      pllU = device->getValues ( ) ;
		      delete pllU ;

		      // Ok for this speed
		      i2cSpeedOpti[7] = i2cSpeed ;
		    }
		    catch (FecExceptionHandler &e) {
		      if (i2cSpeed == 100) {
			char msg[100] ;
			decodeKey(msg, indexF | setAddressKey(0x44)) ;
			std::cerr << "\t\tPLL 0x" << std::hex << 0x44 << ": Inconsistency error at 100 kHz => cannot download/upload it" << std::endl ;

			results.push_back("      <i2cSpeed PLL 0x44 = error>");
		      }
		    }
		    //std::cout << "\tPLL 0x" << std::hex << 0x44 << ": I2C speed " << std::dec << i2cSpeedOpti[7] << " supported" << std::endl ;
		  }

		  if (aohF) {
		    try {
		      //std::cout << "AOH " << 0x60 << std::endl ;

		      // create the access
		      laserdriverAccess *device = laserdriverSet[indexF | setAddressKey(0x60)] ;
		      device->setValues (aohD) ;
		      aohU = device->getValues ( ) ;
		      delete aohU ;

		      // Ok for this speed
		      i2cSpeedOpti[8] = i2cSpeed ;
		    }
		    catch (FecExceptionHandler &e) {
		      if (i2cSpeed == 100) {
			char msg[100] ;
			decodeKey(msg, indexF | setAddressKey(0x60)) ;
			std::cerr << "\t\tAOH 0x" << std::hex << 0x60 << ": Inconsistency error at 100 kHz => cannot download/upload it" << std::endl ;

			results.push_back("      <i2cSpeed LASERDRIVER 0x60 = error>");
		      }
		    }
		    //std::cout << "\tAOH 0x" << std::hex << 0x60 << ": I2C speed " << std::dec << i2cSpeedOpti[8] << " supported" << std::endl ;
		  }

		  if (dcu0F) {
		    try {
		      //std::cout << "DCU " << 0x0 << std::endl ;

		      // create the access
		      dcuAccess *device = dcuSet[indexF | setAddressKey(0x0)] ;
		      dcuU = device->getValues ( ) ;
		      delete dcuU ;

		      // Ok for this speed
		      i2cSpeedOpti[10] = i2cSpeed ;
		    }
		    catch (FecExceptionHandler &e) {
		      if (i2cSpeed == 100) {
			char msg[100] ;
			decodeKey(msg, indexF | setAddressKey(0x0)) ;
			std::cerr << "\t\tDCU 0x" << std::hex << 0x0 << ": Inconsistency error at 100 kHz => cannot download/upload it" << std::endl ;

			results.push_back("      <i2cSpeed DCU 0x0 = error>");
		      }
		    }
		    //std::cout << "\tDCU 0x" << std::hex << 0x0 << ": I2C speed " << std::dec << i2cSpeedOpti[10] << " supported" << std::endl ;
		  }
		  switch (i2cSpeed) {
		  case 100: i2cSpeed = 200 ; break ;
		  case 200: i2cSpeed = 400 ; break ;
		  case 400: i2cSpeed = 1000 ; break ;
		  case 1000: i2cSpeed = 1001 ; break;
		  }
		}

		// Find the minimum
		tscType16 speedMin = 1000 ;
		bool deviceError = false ;
		for (int i = 0 ; i < 12 ; i ++) {
		  if (module[i]) {
		    if (i2cSpeedOpti[i] != 0)
		      speedMin = speedMin < i2cSpeedOpti[i] ? i2cSpeedOpti[i] : speedMin ;
		    else deviceError = true ;
		  }
		}
		std::cout << "\tI2C speed supported: " << std::dec << speedMin ;
		if (deviceError) {
		  std::cout << " but some device seems to be unstable" << std::endl ;

		  results.push_back("      <i2cSpeed = " + IntToString(speedMin, 10) + "kHz but some device seems to be unstable>");
		}
		else {
		  std::cout << std::endl ;
		  results.push_back("      <i2cSpeed = " + IntToString(speedMin, 10) + "kHz>");
		}
	      }
	    }
	    else {
	      // Is it a DCU on CCU
	      if ( (dcu0F || dcu78F) && !apv6F && !apv4F && !pllF && !muxF && !aohF ) {

		std::cout << "Found a DCU on CCU on FEC " << std::dec << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << std::hex << getCcuKey(indexF) << " channel 0x" << std::hex << getChannelKey(indexF) << std::endl;

		std::string dcuAddress;
		if(dcu78F) dcuAddress = "78";
		else dcuAddress = "0";

		results.push_back("   <DCU 0x" + dcuAddress  + " on channel 0x" + IntToString(getChannelKey(indexF)) + ">");

		if (testI2CSpeed) {

		  // Parameters to be downloaded and uploaded
		  dcuDescription *dcuU ;
	      
		  tscType16 i2cSpeed = 100, i2cSpeedOpti = 0 ;

		  while (i2cSpeed <= 1000) {

		    tscType8 channelCRA = 0 ;
		    try {
		      // Set the i2cSpeed at the correct value
		      channelCRA = fecAccess->geti2cChannelCRA (indexF) ;
		      channelCRA &= 0xFC ;
		      switch (i2cSpeed) {
		      case 100: channelCRA |= 0x0 ; break ;
		      case 200: channelCRA |= 0x1 ; break ;
		      case 400: channelCRA |= 0x2 ; break ;
		      case 1000: channelCRA |= 0x3 ; break;
		      }
		      //std::cout << "Set the i2c channel to speed " << std::dec << i2cSpeed << std::endl ;
		      fecAccess->seti2cChannelCRA (indexF,channelCRA) ;
		    }
		    catch (FecExceptionHandler &e) {
		      std::cerr << "Cannot retreive the CRA of i2c channel: " << e.what() << std::endl ;
		    }
		  
		    try {
		      // create the access
		      dcuAccess *device = dcuSet[indexF | setAddressKey(0x0)] ;
		      dcuU = device->getValues ( ) ;
		      delete dcuU ;

		      // Ok for this speed
		      i2cSpeedOpti = i2cSpeed ;
		    }
		    catch (FecExceptionHandler &e) {
		      if (i2cSpeed == 100) {
			char msg[100] ;
			decodeKey(msg, indexF | setAddressKey(0x0)) ;
			std::cerr << "\t\t: Inconsistency error at 100 kHz => cannot download/upload it" << std::endl ;

			results.push_back("      <i2cSpeed = error>");
		      }
		    }
		    //std::cout << "\tDCU : I2C speed " << std::dec << i2cSpeedOpti << " supported" << std::endl ;
		  
		    switch (i2cSpeed) {
		    case 100: i2cSpeed = 200 ; break ;
		    case 200: i2cSpeed = 400 ; break ;
		    case 400: i2cSpeed = 1000 ; break ;
		    case 1000: i2cSpeed = 1001 ; break;
		    }
		  }
		  std::cout << "\tDCU : I2C speed " << std::dec << i2cSpeedOpti << " supported" << std::endl ;

		  results.push_back("      <i2cSpeed = " + IntToString(i2cSpeedOpti, 10) + "kHz>");
		}

		results.push_back("   </DCU>");
	      }
	    
	      // Is it a DOH
	      if (dohF) {
		std::cout << "Found a DOH on FEC " << std::dec << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << std::hex << getCcuKey(indexF) << " channel 0x" << std::hex << getChannelKey(indexF) << std::endl ;
 
		results.push_back("   <DOH 0x70 on channel 0x" + IntToString(getChannelKey(indexF)) + ">");

		if (testI2CSpeed) {
		
		  // Parameters to be downloaded and uploaded
		  tscType8 biasInit[MAXLASERDRIVERCHANNELS], gainInit[MAXLASERDRIVERCHANNELS] ;
		  for (int i = 0 ; i < MAXLASERDRIVERCHANNELS ; i ++) {
		    biasInit[i] = 48 ;
		    gainInit[i] = 2  ;
		  }
		  laserdriverDescription dohD (gainInit,biasInit), *dohU ;
		
		  tscType16 i2cSpeed = 100, i2cSpeedOpti = 0 ;
		
		  while (i2cSpeed <= 1000) {

		    try {
		      // create the access
		      DohAccess *device = dohSet[indexF | setAddressKey(0x70)] ;
		      device->setValues (dohD, true) ;
		      dohU = device->getValues ( ) ;
		      delete dohU ;

		      // Ok for this speed
		      i2cSpeedOpti = i2cSpeed ;
		    }
		    catch (FecExceptionHandler &e) {
		      if (i2cSpeed == 100) {
			char msg[100] ;
			decodeKey(msg, indexF | setAddressKey(0x60)) ;
			std::cerr << "\t\tDOH 0x" << std::hex << 0x70 << ": Inconsistency error at 100 kHz => cannot download/upload it" << std::endl ;

			results.push_back("      <i2cSpeed = error>");
		      }
		    }
		    switch (i2cSpeed) {
		    case 100: i2cSpeed = 200 ; break ;
		    case 200: i2cSpeed = 400 ; break ;
		    case 400: i2cSpeed = 1000 ; break ;
		    case 1000: i2cSpeed = 1001 ; break;
		    }
		  }
		  std::cout << "\tDOH 0x" << std::hex << 0x70 << ": I2C speed " << std::dec << i2cSpeedOpti << " supported" << std::endl ;
		  results.push_back("      <i2cSpeed = " + IntToString(i2cSpeedOpti, 10) + "kHz>");
		}

results.push_back("   </DOH>");
}
	    }
	  }
	}

	// delete hash_map
	for (apvAccessedType::iterator p=apvSet.begin();p!=apvSet.end();p++) {
	  try { delete p->second ; }
	  catch (FecExceptionHandler &e) {	}
	}
	apvSet.clear() ;
	for (pllAccessedType::iterator p=pllSet.begin();p!=pllSet.end();p++) {
	  try { delete p->second ; }
	  catch (FecExceptionHandler &e) {	}
	}
	pllSet.clear() ;
	for (laserdriverAccessedType::iterator p=laserdriverSet.begin();p!=laserdriverSet.end();p++) {
	  try { delete p->second ; }
	  catch (FecExceptionHandler &e) {	}
	}
	laserdriverSet.clear() ;
	for (dohAccessedType::iterator p=dohSet.begin();p!=dohSet.end();p++) {
	  try { delete p->second ; }
	  catch (FecExceptionHandler &e) {	}
	}
	dohSet.clear() ;
	for (muxAccessedType::iterator p=muxSet.begin();p!=muxSet.end();p++) {
	  try { delete p->second ; }
	  catch (FecExceptionHandler &e) { }
	}
	muxSet.clear() ;
	for (dcuAccessedType::iterator p=dcuSet.begin();p!=dcuSet.end();p++) {
	  try { delete p->second ; }
	  catch (FecExceptionHandler &e) { }
	}
	dcuSet.clear() ;
	for (Sgi::hash_map<keyType, bool *>::iterator p=trackerModule.begin();p!=trackerModule.end();p++) {
	  delete[] p->second ;
	}
	trackerModule.clear() ;
      }
      else {
	std::cout << "No device found on the CCU and channels" << std::endl ;
      }

      // Message
      if (! noGetchar_) {
	std::cout << "Press <Enter> to continue" ;
	getchar() ;
      }

      // Wait
      if ( (loop != 1) && (tms > 0) ) usleep (tms) ;

      delete deviceList ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << "Cannot scan the ring for i2c devices" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }
  results.push_back("</CCU>");

  // Switch all the APVs
  //std::cout << "Make a PIA reset on all FEC and all CCUs" << std::endl ;
  allCCUsPiaReset (fecAccess, fecAddress, ringAddress) ;

  return results ;
}

/**
 * This method scan the ring for all the I2C address (0x0 -> 0xFF)
 * <p>command: -scanringedevice
 * \param fecAddress - FEC address
 * \param ringSlot   - ring slot
 * \warning the precompilation flag must be set
 * \warning the loop is not used
 */
void testScanRingEDevice ( FecAccess *fecAccess,
			   tscType8 fecAddress, tscType8 ringAddress,
			   bool noBroadcast ) { 

  FecRingDevice *fecDevice ;
  try {

    fecDevice = fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << "Did not find the FEC" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    return ;
  }

  try {
    fecDevice->fecHardReset() ;
    // Disable PLX interrupts
    fecDevice->setIRQ (false) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << "Error during the reset of the FEC" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }

  std::list<keyType> *deviceList = fecDevice->scanRingForI2CDevice ( noBroadcast, true ) ;

  if (deviceList != NULL) {
    deviceList->sort() ;

    for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) {
      
      keyType index = *p ;
   
      char msg[100] ;
      decodeKey (msg, index) ;

      std::cout << "Device " << msg << " found" << std::endl ;
    }
  }
  else {
    std::cout << "No device found on the CCU and channels" << std::endl ;
  }

  delete deviceList ;
}

/**
 * <p>command: -status
 * \param fecAccess - FEC Access object
 * \param noBroadcast - CCU broadcast mode used (false, default) or not (true) [OPTION parameter]
 * \warning the loop is not used
 */
void testStatus ( FecAccess *fecAccess,
		  bool noBroadcast ) {

  std::list<keyType> *ccuList = NULL ;
  try {

    // Message
    std::cout << "-------------------------------------------------" << std::endl ;
    std::cout << "Scan the device driver FECs" << std::endl ;

    for (tscType8 fecAddress = minFecSlot_ ; fecAddress <= maxFecSlot_  ; fecAddress ++) {

      for (tscType8 ringAddress = minFecRing_ ; ringAddress <= maxFecRing_ ; ringAddress++) {
	
	// Find the FECs
	try {
	  // Scan the PCI slot for device driver loaded
	  fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;

	  keyType index = buildFecRingKey(fecAddress,ringAddress) ;        
	  tscType32 fecSR0 = fecAccess->getFecRingSR0 (index) ;
	  std::cout << "Found a FEC device driver on slot " << std::dec << (int)fecAddress << "." << (int)ringAddress << std::endl ;
	  std::cout << "\tFEC status register  0: 0x" << std::hex << fecSR0 << std::endl ;
	  if (fecSR0 & 0x1)
	    std::cout << "\t\tFIFO transmit running" << std::endl ;
	  if (fecSR0 & 0x2)
	    std::cout << "\t\tFIFO receive running" << std::endl ;
	  if (fecSR0 & 0x4)
	    std::cout << "\t\tFIFO receive half full" << std::endl ;
	  if (fecSR0 & 0x8)
	    std::cout << "\t\tFIFO receive full" << std::endl ;
	  if (fecSR0 & 0x10)
	    std::cout << "\t\tFIFO receive empty" << std::endl ;
	  if (fecSR0 & 0x20)
	    std::cout << "\t\tFIFO return half full" << std::endl ;
	  if (fecSR0 & 0x40)
	    std::cout << "\t\tFIFO return full" << std::endl ;
	  if (fecSR0 & 0x80)
	    std::cout << "\t\tFIFO return empty" << std::endl ;
	  if (fecSR0 & 0x100)
	    std::cout << "\t\tFIFO transmit half full" << std::endl ;
	  if (fecSR0 & 0x200)
	    std::cout << "\t\tFIFO transmit full" << std::endl ;
	  if (fecSR0 & 0x400)
	    std::cout << "\t\tFIFO transmit empty" << std::endl ;
	  if (fecSR0 & 0x800)
	    std::cout << "\t\tLink initialise" << std::endl ;
	  if (fecSR0 & 0x1000) 
	    std::cout << "\t\tPending irg" << std::endl ;
	  if (fecSR0 & 0x2000)
	    std::cout << "\t\tData to FEC" << std::endl ;
	  if (fecSR0 & 0x4000)
	    std::cout << "\t\tTTCRx ok" << std::endl ;
	
	  tscType32 fecSR1 = fecAccess->getFecRingSR1 (index) ;
	  std::cout << "\tFEC status register  1: 0x" << std::hex << fecSR1 << std::endl ;
	  if (fecSR1 & 0x1)
	    std::cout << "\t\tIllegal data" << std::endl ;
	  if (fecSR1 & 0x2)
	    std::cout << "\t\tIllegal sequence" << std::endl ;
	  if (fecSR1 & 0x4) 
	    std::cout << "\t\tCRC error" << std::endl ;
	  if (fecSR1 & 0x8) 
	    std::cout << "\t\tData copied" << std::endl ;
	  if (fecSR1 & 0x10) 
	    std::cout << "\t\tAddress seen" << std::endl ;
	  if (fecSR1 & 0x20) 
	    std::cout << "\t\tError" << std::endl ;
	
	  tscType32 fecCR0 = fecAccess->getFecRingCR0 (index) ;
	  std::cout << "\tFEC control register 0: 0x" << std::hex << fecCR0 << std::endl ;
	  if (fecCR0 & 0x1)
	    std::cout << "\t\tFEC enable" << std::endl ;
	  else
	    std::cout << "\t\tFEC disable" << std::endl ;
	  //if (fecCR0 & 0x2)
	  //  std::cout << "\t\tSend" << std::endl ;
	  //if (fecCR0 & 0x4)
	  //  std::cout << "\t\tResend" << std::endl ;
	  if (fecCR0 & 0x8)
	    std::cout << "\t\tOutput ring B" << std::endl ;
	  else
	    std::cout << "\t\tOutput ring A" << std::endl ;
	  if (fecCR0 & 0x10)
	    std::cout << "\t\tInput ring B" << std::endl ;
	  else
	    std::cout << "\t\tInput ring A" << std::endl ;
	  std::cout << "\t\tSel irq mode = " << std::dec << (fecCR0 & 0x20) << std::endl ;
	  //std::cout << "\t\tReset TTCRx = " << std::dec << (fecCR0 & 0x20) << std::endl ;
	  
	  tscType32 fecCR1 = fecAccess->getFecRingCR1 (index) ;
	  std::cout << "\tFEC control register 1: " << std::hex << fecCR1 << std::endl ;
	
	  // Start the scanning of the ring for each FEC device
	  FecRingDevice *fecDevice = fecAccess->getFecRingDevice ( index ) ;

	  fecDevice->fecScanRingBroadcast ( ) ;
	}
	catch (FecExceptionHandler &e) {

	}
      }
    }

    // Message
    if (! noGetchar_) {
      std::cout << "Press <Enter> to continue" ;
      getchar() ;
    }

    // Scan the PCI or VME slot for device driver loaded
    std::list<keyType> *fecSlotList = fecAccess->getFecList( ) ;
    
    if ((fecSlotList != NULL) && (!fecSlotList->empty())) {
      for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
        
        keyType indexFEC = *p ;        
	std::cout << "-------------------------------------------------" << std::endl ;
	std::cout << "Scan the ring to find CCUs for FEC " << std::dec << (int)getFecKey(indexFEC) << " ring " << (int)getRingKey(indexFEC) << std::endl ;
	
	try {
	  // Scan the ring for each FECs and display the CCUs
	  ccuList = fecAccess->getCcuList ( indexFEC, noBroadcast ) ;

	  if (ccuList != NULL) {
	    
	    for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {
	      
	      keyType index = *p ;
	      
	      std::cout << std::hex << "FEC 0x" << getFecKey(index) 
			<< " Ring 0x " << getRingKey(index) 
			<< " CCU 0x" << getCcuKey(index)
			<< " found" << std::endl ;
	    
	      // Get CCU registers
	      tscType32 SRA = fecAccess->getCcuSRA(index) ;
	      fecAccess->setCcuCRA(index,CCU_CRA_CLRE || CCU_CRA_RES) ;
	      tscType32 SRABis = fecAccess->getCcuSRA(index) ;
	      tscType32 SRB = fecAccess->getCcuSRB(index) ;
	      tscType32 SRC = fecAccess->getCcuSRC(index) ;
	      tscType32 SRD = fecAccess->getCcuSRD(index) ;
	      tscType32 SRE = fecAccess->getCcuSRE(index) ;
	      tscType32 SRF = fecAccess->getCcuSRF(index) ;
	      tscType32 CRA = fecAccess->getCcuCRA(index) ;
	      tscType32 CRB = fecAccess->getCcuCRB(index) ;
	      tscType32 CRC = fecAccess->getCcuCRC(index) ;
	      tscType32 CRD = fecAccess->getCcuCRD(index) ;
	      tscType32 CRE = fecAccess->getCcuCRE(index) ;
	      //tscType32 CRF = fecAccess->getCcuCRF(index) ;
	      
	      std::cout << "\tCCU status register  A: 0x" << std::hex << SRA << std::endl ; displayCCUSRA(SRA) ;
	      std::cout << "\tCCU status register  A: 0x" << std::hex << SRA << std::endl ; displayCCUSRA(SRABis) ;
	      std::cout << "\tCCU status register  B: 0x" << std::hex << SRB << std::endl ; displayCCUSRB(SRB) ;
	      std::cout << "\tCCU status register  C: 0x" << std::hex << SRC << std::endl ; displayCCUSRC(SRC) ;
	      std::cout << "\tCCU status register  D: 0x" << std::hex << SRD << std::endl ; displayCCUSRD(SRD) ;
	      std::cout << "\tCCU status register  E: 0x" << std::hex << SRE << std::endl ; displayCCUSRE(SRE) ;
	      std::cout << "\tCCU status register  F: 0x" << std::hex << SRF << std::endl ; displayCCUSRF(SRF) ;
	      std::cout << "\tCCU control register A: 0x" << std::hex << CRA << std::endl ; displayCCUCRA(CRA) ;
	      std::cout << "\tCCU control register B: 0x" << std::hex << CRB << std::endl ; displayCCUCRB(CRB) ;
	      std::cout << "\tCCU control register C: 0x" << std::hex << CRC << std::endl ; displayCCUCRC(CRC) ;
	      std::cout << "\tCCU control register D: 0x" << std::hex << CRD << std::endl ; displayCCUCRD(CRD) ;
	      std::cout << "\tCCU control register E: 0x" << std::hex << CRE << std::endl ; displayCCUCRE(CRE) ;
	      //std::cout << "\tCCU control register F: 0x" << std::hex << CRF << std::endl ; 
	      
	      if (! noGetchar_) {
		std::cout << "Press <Enter> to continue" ;
		getchar() ;
	      }
	    }
	  }
	  else {
	    std::cout << "No CCU on FEC " << std::dec << (int)getFecKey(indexFEC) << " ring " << (int)getRingKey(indexFEC) << std::endl ;
	  }
	}
	catch (FecExceptionHandler &e) {
	  std::cout << "No CCU on FEC " << std::dec << (int)getFecKey(indexFEC) << " ring " << (int)getRingKey(indexFEC) << std::endl ;
	}
      }
    }
    else {
      std::cout << "No FEC found" << std::endl ;
    }  
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Delete = already store in the table of FEC device class
  delete ccuList ;
}

/** 
 * <p>command: -statusCCU
 * \param fecAccess - FEC Access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param ccuAddress - CCU address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
int testCCU ( FecAccess *fecAccess,
	      tscType8 fecAddress, 
	      tscType8 ringAddress,
	      tscType8 ccuAddress, 
	      long loop, unsigned long tms ) {

  keyType index = buildCompleteKey(fecAddress,ringAddress,ccuAddress,0,0);

  try {

    if (fecAccess->getFecRingDevice(index) != NULL) {

      tscType16 fecSR0 = fecAccess->getFecRingSR0 (index) ; displayFECSR0 (fecSR0) ;
      tscType16 fecSR1 = fecAccess->getFecRingSR1 (index) ; displayFECSR1 (fecSR1) ;
      tscType16 fecCR0 = fecAccess->getFecRingCR0 (index) ; displayFECCR0 (fecCR0) ;
      //tscType16 CR1 = fecAccess->getFecRingCR1 (index) ; displayFECCR1 (fecCR1) ;    
      
      // Message
      if (! noGetchar_) {
        std::cout << "Press <Enter> to continue" ;
        getchar() ;
      }
    
      // Display all the status of the corresponding CCU
      std::cout << "Status for CCU 0x" << std::hex << (int)ccuAddress << std::endl ;
      tscType8 SRA = fecAccess->getCcuSRA(index) ; displayCCUSRA(SRA) ;
      tscType8 SRB = fecAccess->getCcuSRB(index) ; displayCCUSRB(SRB) ;
      tscType8 SRC = fecAccess->getCcuSRC(index) ; displayCCUSRC(SRC) ;
      tscType8 SRD = fecAccess->getCcuSRD(index) ; displayCCUSRD(SRD) ;
      tscType32 SRE = fecAccess->getCcuSRE(index) ; displayCCUSRE(SRE) ;
      tscType16 SRF = fecAccess->getCcuSRF(index) ; displayCCUSRF(SRF) ;
      tscType8 CRA = fecAccess->getCcuCRA(index) ; displayCCUSRF(CRA) ;
      tscType8 CRB = fecAccess->getCcuCRB(index) ; displayCCUCRA(CRB) ;
      tscType8 CRC = fecAccess->getCcuCRC(index) ; displayCCUCRB(CRC) ;
      tscType8 CRD = fecAccess->getCcuCRD(index) ; displayCCUCRC(CRD) ;
      tscType32 CRE = fecAccess->getCcuCRE(index) ; displayCCUCRE(CRE) ;
    }
    else {
      std::cout << "No FEC device driver found" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;

    return -1 ;
  }  

  return 0 ;
}

/**
 * \warning note that the counters must be available
 * \warning before display all status, operations must be done on CCUs.
 * Only CCUs which supported already operations are displayed.
 */
void displayStatus ( FecExceptionHandler *e, unsigned int count,
                     FecAccess *fecAccess, FILE *stdchan ) {

  time_t timestamp = time(NULL) ;
  errorType errorCode = 0 ;
  keyType index = 0xFFFFFFFF ;
  std::string dMsg = "No error";
  static unsigned int errorCounter = 1 ;

  if (e != NULL) {

    timestamp = e->getTimeStamp() ;
    errorCode = e->getErrorCode() ;
    index = 0xFFFFFFFF ;
    if (e->getPositionGiven())
      index = e->getHardPosition() ;
    dMsg = e->getErrorMessage ( ) ;
  }

  try {
    
    fprintf (stdchan_, "------------------------------------ Error %d\n", errorCounter) ;
    errorCounter ++ ;

    // Scan the PCI slot for device driver loaded
    std::list<keyType> *fecSlotList = fecAccess->getFecList( ) ;
    
    if (fecSlotList != NULL) {

      fprintf (stdchan, "timestamp=%ld\n", timestamp) ;
      if (count != 0) 
        fprintf (stdchan, "counter=%d\n", count) ;

      if (e != NULL) {
        fprintf (stdchan, "errorCode=%ld\n", errorCode) ;
        fprintf (stdchan, "sourceErrorIndex=0x%04X\n", index) ;
        fprintf (stdchan, "dMsg=\"%s\"\n", dMsg.c_str()) ;
      }
      
      for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
        
        keyType index = *p ;        
        FecRingDevice *fecDevice = fecAccess->getFecRingDevice ( index ) ;
        
	fprintf (stdchan, "FEC %d ring %d\n", getFecKey(index), getRingKey(index)) ;

	// FEC control/status registers
	fprintf ( stdchan, "FEC_SR0=0x%04X\n", (tscType16)fecDevice->getFecRingSR0()) ;
	fprintf ( stdchan, "FEC_SR1=0x%04X\n", fecDevice->getFecRingSR1()) ;
	fprintf ( stdchan, "FEC_CR0=0x%04X\n", fecDevice->getFecRingCR0()) ;
	fprintf ( stdchan, "FEC_CR1=0x%04X\n", fecDevice->getFecRingCR1()) ;

	// For each CCU on that ring
	std::list<keyType> *ccuList = fecDevice->getCcuList() ;
	if (ccuList != NULL) {
	  for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {
	
	    keyType indexCcu = *p ;
	  
	    // Get CCU registers
	    unsigned int SRA = fecAccess->getCcuSRA(indexCcu) ;
	    unsigned int SRB = fecAccess->getCcuSRB(indexCcu) ;
	    unsigned int SRC = fecAccess->getCcuSRC(indexCcu) ;
	    unsigned int SRD = fecAccess->getCcuSRD(indexCcu) ;
	    unsigned int SRE = fecAccess->getCcuSRE(indexCcu) ;
	    unsigned int SRF = fecAccess->getCcuSRF(indexCcu) ;
	    unsigned int CRA = fecAccess->getCcuCRA(indexCcu) ;
	    unsigned int CRB = fecAccess->getCcuCRB(indexCcu) ;
	    unsigned int CRC = fecAccess->getCcuCRC(indexCcu) ;
	    unsigned int CRD = fecAccess->getCcuCRD(indexCcu) ;
	    unsigned int CRE = fecAccess->getCcuCRE(indexCcu) ;
	    //unsigned int CRF = fecAccess->getCcuCRF(indexCcu) ;
	  
	    unsigned int ccuAddress = getCcuKey (indexCcu) ;
	    fprintf ( stdchan, "CCU_0x%02X_SRA=0x%04X\n", ccuAddress, SRA) ; 
	    fprintf ( stdchan, "CCU_0x%02X_SRB=0x%04X\n", ccuAddress, SRB) ; 
	    fprintf ( stdchan, "CCU_0x%02X_SRC=0x%04X\n", ccuAddress, SRC) ; 
	    fprintf ( stdchan, "CCU_0x%02X_SRD=0x%04X\n", ccuAddress, SRD) ; 
	    fprintf ( stdchan, "CCU_0x%02X_SRE=0x%04X\n", ccuAddress, SRE) ; 
	    fprintf ( stdchan, "CCU_0x%02X_SRF=0x%04X\n", ccuAddress, SRF) ; 
	    fprintf ( stdchan, "CCU_0x%02X_CRA=0x%04X\n", ccuAddress, CRA) ; 
	    fprintf ( stdchan, "CCU_0x%02X_CRB=0x%04X\n", ccuAddress, CRB) ; 
	    fprintf ( stdchan, "CCU_0x%02X_CRC=0x%04X\n", ccuAddress, CRC) ; 
	    fprintf ( stdchan, "CCU_0x%02X_CRD=0x%04X\n", ccuAddress, CRD) ; 
	    fprintf ( stdchan, "CCU_0x%02X_CRE=0x%04X\n", ccuAddress, CRE) ; 
	  }
	}
      }
    }
    else {
      fprintf ( stdchan, "Unable to read the CCUs" ) ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}

/**
 * \warning note that the counters must be available
 * \warning before display all status, operations must be done on CCUs.
 * Only CCUs which supported already operations are displayed.
 */
void displayStatus ( FecAccess *fecAccess,
		     tscType8 fecAddress, 
		     tscType8 ringAddress,
		     tscType8 ccuAddress, 
		     tscType8 channelAddress,
		     FecExceptionHandler *e) {

  FILE *stdchan = NULL ;

  //std::cout << fileSuperName_ << std::endl ;
  if (fileSuperName_ != NULL)
    stdchan = fopen (fileSuperName_, "a") ;

  if (stdchan == NULL) stdchan = stderr;

  time_t timestamp = time(NULL) ;
  errorType errorCode = 0 ;
  keyType index = 0xFFFFFFFF ;
  std::string dMsg = "No error";
  static unsigned int errorCounter = 1 ;
  keyType currentIndex = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,0) ;

  if (e != NULL) {

    timestamp = e->getTimeStamp() ;
    errorCode = e->getErrorCode() ;
    index = 0xFFFFFFFF ;
    if (e->getPositionGiven())
      index = e->getHardPosition() ;
    dMsg = e->getErrorMessage ( ) ;
  }

  try {
    
    fprintf (stdchan, "------------------------------------ Error %d\n", errorCounter) ;
    errorCounter ++ ;

    fprintf (stdchan, "timestamp=%ld\n", timestamp) ;
    
    if (e != NULL) {
      fprintf (stdchan, "errorCode=%ld\n", errorCode) ;
      fprintf (stdchan, "sourceErrorIndex=0x%04X\n", index) ;
      char msg[80] ;
      decodeKey(msg,index) ;
      fprintf (stdchan, "sourceErrorIndexSTR=%s\n", msg) ;
      fprintf (stdchan, "dMsg=\"%s\"\n", dMsg.c_str()) ;
    }

    try {
      // FEC control/status registers
      fprintf ( stdchan, "FEC_SR0=0x%04X\n", fecAccess->getFecRingSR0(currentIndex)) ;
      fprintf ( stdchan, "FEC_SR1=0x%04X\n", fecAccess->getFecRingSR1(currentIndex)) ;
      fprintf ( stdchan, "FEC_CR0=0x%04X\n", fecAccess->getFecRingCR0(currentIndex)) ;
      fprintf ( stdchan, "FEC_CR1=0x%04X\n", fecAccess->getFecRingCR1(currentIndex)) ;
    }
    catch (FecExceptionHandler &e) {
      fprintf ( stdchan, "DISPLAYSTATUS_ERROR: Unable to read one of all FEC registers\n" ) ;
    }

    try {
      // Get CCU registers
      unsigned int SRA = fecAccess->getCcuSRA(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_SRA=0x%04X\n", ccuAddress, SRA) ; 
      unsigned int SRB = fecAccess->getCcuSRB(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_SRB=0x%04X\n", ccuAddress, SRB) ; 
      unsigned int SRC = fecAccess->getCcuSRC(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_SRC=0x%04X\n", ccuAddress, SRC) ; 
      unsigned int SRD = fecAccess->getCcuSRD(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_SRD=0x%04X\n", ccuAddress, SRD) ; 
      unsigned int SRE = fecAccess->getCcuSRE(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_SRE=0x%04X\n", ccuAddress, SRE) ;
      unsigned int SRF = fecAccess->getCcuSRF(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_SRF=0x%04X\n", ccuAddress, SRF) ; 
      unsigned int CRA = fecAccess->getCcuCRA(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_CRA=0x%04X\n", ccuAddress, CRA) ; 
      unsigned int CRB = fecAccess->getCcuCRB(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_CRB=0x%04X\n", ccuAddress, CRB) ; 
      unsigned int CRC = fecAccess->getCcuCRC(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_CRC=0x%04X\n", ccuAddress, CRC) ; 
      unsigned int CRD = fecAccess->getCcuCRD(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_CRD=0x%04X\n", ccuAddress, CRD) ; 
      unsigned int CRE = fecAccess->getCcuCRE(currentIndex) ;
      fprintf ( stdchan, "CCU_0x%02X_CRE=0x%04X\n", ccuAddress, CRE) ; 
      //unsigned int CRF = fecAccess->getCcuCRF(currentIndex) ;
    }
    catch (FecExceptionHandler &e) {
      fprintf ( stdchan, "DISPLAYSTATUS_ERROR: Unable to read one of the CCU registers\n") ;
    }

    // Display the i2c channel register control and status register
    if (isi2cChannelCcu25(currentIndex)) {
      try {
	unsigned int iCRA = fecAccess->geti2cChannelCRA(currentIndex) ;
	fprintf ( stdchan, "CCU_0x%02X_I2C_0x%02X_CRA=0x%04X\n", ccuAddress, channelAddress, iCRA) ; 
	unsigned int iSRA = fecAccess->geti2cChannelSRA(currentIndex) ;
	fprintf ( stdchan, "CCU_0x%02X_I2C_0x%02X_SRA=0x%04X\n", ccuAddress, channelAddress, iSRA) ; 
	unsigned int iSRB = fecAccess->geti2cChannelSRB(currentIndex) ;
	fprintf ( stdchan, "CCU_0x%02X_I2C_0x%02X_SRB=0x%04X\n", ccuAddress, channelAddress, iSRB) ;
	unsigned int iSRC = fecAccess->geti2cChannelSRC(currentIndex) ;
	fprintf ( stdchan, "CCU_0x%02X_I2C_0x%02X_SRC=0x%04X\n", ccuAddress, channelAddress, iSRC) ;
	unsigned int iSRD = fecAccess->geti2cChannelSRD(currentIndex) ;
	fprintf ( stdchan, "CCU_0x%02X_I2C_0x%02X_SRD=0x%04X\n", ccuAddress, channelAddress, iSRD) ; 
      }
      catch (FecExceptionHandler &e) {
	fprintf ( stdchan, "DISPLAYSTATUS_ERROR: Unable to read one of the CCU I2C registers\n") ;
      }
    }

    // Clear the error registers in the CCU and in the FEC
    //fecAccess->setFecSR1(currentIndex, (tscType16)0x7) ;    
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       Memory channel                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** 
 * \param fecAccess - FEC Access object
 * \param index - index of the memory channel
 * \param maxVal - number of accesses
 * \warning the memory channel init must be done before the call
 */
void testMemWriteSingle ( FecAccess *fecAccess,
                          keyType myKey,
                          unsigned long maxVal ) {

  try {
    std::cout << "**************************************************************" << std::endl ;
    std::cout << "Single byte write in memory for " << maxVal << " values" << std::endl ;
    std::cout << "**************************************************************" << std::endl ;

    struct timeval time1, time2;
    struct timezone zone1, zone2;
    gettimeofday(&time1, &zone1);

    for (unsigned long i = 0 ; i < maxVal ; i ++) {
      
      tscType16 AH, AL ;
	
      if (i <= 255) { 
        AH = 0 ;
        AL = i ;
      }
      else {
        AL = (i & 0xFF) ;
        AH = (i & 0xFF00) >> 8 ;
      }

      fecAccess->write (myKey, AH, AL, 0) ;
    }

    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    double perf = (double) maxVal ;
    perf /= (1024*1024*timesec) ;
    std::cout << "Delay in seconds for " << maxVal << " words: " << timesec << " => " << perf << " Mbytes/sec" << std::endl ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;
    
    // Display status
    std::cout << "FEC status ..." << std::endl ;
    std::cout << "FEC CR0 = 0x" << std::hex << fecAccess->getFecRingCR0(myKey) << std::endl ;
    std::cout << "FEC CR1 = 0x" << std::hex << fecAccess->getFecRingCR1(myKey) << std::endl ;
    std::cout << "FEC SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(myKey) << std::endl ;
    std::cout << "FEC SR1 = 0x" << std::hex << fecAccess->getFecRingSR1(myKey) << std::endl ;
  }
}

/** 
 * \param fecAccess - FEC Access object
 * \param index - index of the memory channel
 * \param maxVal - number of accesses
 * \warning the memory channel init must be done before the call
 */
void testMemReadSingle ( FecAccess *fecAccess,
                         keyType myKey,
                         unsigned long maxVal) {


  try {
    std::cout << "**************************************************************" << std::endl ;
    std::cout << "Single byte read in memory for " << maxVal << " values" << std::endl ;
    std::cout << "**************************************************************" << std::endl ;
      
    struct timeval time1, time2;
    struct timezone zone1, zone2;
    gettimeofday(&time1, &zone1);

    for (unsigned long i = 0 ; i < maxVal ; i ++) {
      
      tscType16 AH, AL ;
	
      if (i <= 255) { 
	AH = 0 ;
	AL = i ;
      }
      else {
	AL = (i & 0xFF) ;
	AH = (i & 0xFF00) >> 8 ;
      }

      fecAccess->read (myKey, AH, AL) ;
    }

    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    double perf = (double) maxVal ;
    perf /= (1024*1024*timesec) ;
    printf( "Delay in seconds for %ld words: %f => %f Mbytes/sec\n", maxVal, timesec, perf) ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    // Display status
    std::cout << "FEC status ..." << std::endl ;
    std::cout << "FEC CR0 = 0x" << std::hex << fecAccess->getFecRingCR0(myKey) << std::endl ;
    std::cout << "FEC CR1 = 0x" << std::hex << fecAccess->getFecRingCR1(myKey) << std::endl ;
    std::cout << "FEC SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(myKey) << std::endl ;
    std::cout << "FEC SR1 = 0x" << std::hex << fecAccess->getFecRingSR1(myKey) << std::endl ;
  }
}

/** 
 * \param fecAccess - FEC Access object
 * \param index - index of the memory channel
 * \param maxVal - number of accesses
 * \warning the memory channel init must be done before the call
 */
void testMemWriteMultiple ( FecAccess *fecAccess,
                            keyType myKey,
                            unsigned long maxVal) {

  try {

    std::cout << "*********************************************************************" << std::endl ;
    std::cout << "Multiple bytes write in memory for " << maxVal << " values" << std::endl ;
    std::cout << "*********************************************************************" << std::endl ;

    tscType8 *frame = new tscType8[maxVal] ;
    for (unsigned long i = 1 ; i <= maxVal ; i ++) frame[i-1] = i % 256 ;

    struct timeval time1, time2;
    struct timezone zone1, zone2;
    gettimeofday(&time1, &zone1);
    fecAccess->write (myKey, 0, 0, frame, maxVal) ;
    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    double perf = (double) maxVal ;
    perf /= (1024*1024*timesec) ;

    std::cout << "*********************************************************************" << std::endl ;
    std::cout << "Multiple bytes write in memory for " << maxVal << " values" << std::endl ;
    std::cout << "Delay in seconds for " << maxVal << " words: " << timesec << " => " << perf << " Mbytes/sec" << std::endl ;
    std::cout << "*********************************************************************" << std::endl ;

    // delete array
    delete []frame ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    // Display status
    std::cout << "FEC status ..." << std::endl ;
    std::cout << "FEC CR0 = 0x" << std::hex << fecAccess->getFecRingCR0(myKey) << std::endl ;
    std::cout << "FEC CR1 = 0x" << std::hex << fecAccess->getFecRingCR1(myKey) << std::endl ;
    std::cout << "FEC SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(myKey) << std::endl ;
    std::cout << "FEC SR1 = 0x" << std::hex << fecAccess->getFecRingSR1(myKey) << std::endl ;
  }
}

/**
 * \param fecAccess - FEC Access object
 * \param index - index of the memory channel
 * \param maxVal - number of accesses
 * \warning the memory channel init must be done before the call
 */
void testMemReadMultiple ( FecAccess *fecAccess,
                           keyType myKey,
                           unsigned long maxVal) {


  try {

    std::cout << "*********************************************************************" << std::endl ;
    std::cout << "Multiple bytes read in memory for " << maxVal << " values" << std::endl ;
    std::cout << "*********************************************************************" << std::endl ;

    tscType8 *frame = new tscType8[maxVal] ;

    struct timeval time1, time2;
    struct timezone zone1, zone2;
    gettimeofday(&time1, &zone1);

    fecAccess->read (myKey, 0, 0, maxVal, frame) ;

    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    double perf = (double) maxVal ;
    perf /= (1024*1024*timesec) ;
    std::cout << "*********************************************************************" << std::endl ;
    std::cout << "Multiple bytes read in memory for " << maxVal << " values" << std::endl ;
    std::cout << "Delay in seconds for " << maxVal << " words: " << timesec << " => " << perf << " Mbytes/sec" << std::endl ;
    std::cout << "*********************************************************************" << std::endl ;

    // delete array
    delete []frame ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    // Display status
    std::cout << "FEC status ..." << std::endl ;
    std::cout << "FEC CR0 = 0x" << std::hex << fecAccess->getFecRingCR0(myKey) ;
    std::cout << "FEC CR1 = 0x" << std::hex << fecAccess->getFecRingCR1(myKey) ;
    std::cout << "FEC SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(myKey) ;
    std::cout << "FEC SR1 = 0x" << std::hex << fecAccess->getFecRingSR1(myKey) ;
  }
}

/** 
 * \param fecAccess - FEC Access object
 * \param  fecAddress - slot of the FEC.
 * \param  ringAddress - slot of the ring.
 * \param  ccuAddress - address of the first CCU of the ring.
 * \param  channelAddress - address of the channel.
 * Command: -memoryPerf
 * \warning the loop is not possible
 */
void testMemoryFunctions (FecAccess *fecAccess,
                          tscType8 fecAddress,
                          tscType8 ringAddress,
                          tscType8 ccuAddress,       
                          tscType8 channelAddress,
                          unsigned long maxVal ) {

  keyType myKey = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,0) ; // Build the key

  try {

    std::cout << "-----------------------------" << std::endl ;
    std::cout << "Write 0x" << std::hex << INITCCU25MEMORYCRA << " into the memory CRA" << std::endl ;
    std::cout << "Open WIN 1 to 0x0 - 0x" << MAXMEMORYWINDOWENABLE << std::endl ;
    std::cout << "-----------------------------" << std::endl ;

    // Add the channel
    fecAccess->addMemoryAccess (myKey, MODE_SHARE) ;

    std::cout << "**************************************************************" << std::endl ;
    std::cout << "test of FecRingDevice functions" << std::endl ; 
    std::cout << "**************************************************************" << std::endl ;

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   Performance measurements for " << maxVal << " values" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ; 
    
    testMemWriteMultiple (fecAccess, myKey, maxVal) ;
    testMemReadMultiple (fecAccess, myKey, maxVal) ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception in APIAccess::testMemoryFunctions ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "-------------------------------------------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    // Display status
    std::cout << "FEC status ..." << std::endl ;
    std::cout << "FEC CR0 = 0x" << std::hex << fecAccess->getFecRingCR0(myKey) << std::endl ;
    std::cout << "FEC CR1 = 0x" << std::hex << fecAccess->getFecRingCR1(myKey) << std::endl ;
    std::cout << "FEC SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(myKey) << std::endl ;
    std::cout << "FEC SR1 = 0x" << std::hex << fecAccess->getFecRingSR1(myKey) << std::endl ;
  }
}

/** 
 * \param fecAccess - FEC Access object
 * \param  fecAddress - slot of the FEC.
 * \param  ringAddress - slot of the ring.
 * \param  ccuAddress - address of the first CCU of the ring.
 * \param  channelAddress - address of the channel.
 * Command: -memory
 * \warning the loop is not possible
 */
void testMemoryFunctions (FecAccess *fecAccess,
                          tscType8 fecAddress,
                          tscType8 ringAddress,
                          tscType8 ccuAddress,       
                          tscType8 channelAddress) {
  
  keyType myKey = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,0) ; // Build the key

  // For memory performance
  int maxVal = 1000 ;
  tscType8 *frame = new tscType8[maxVal] ;
  tscType8 *frame1 = new tscType8[maxVal] ;

  try {

    std::cout << "-----------------------------" << std::endl ;
    std::cout << "Write 0x" << std::hex << INITCCU25MEMORYCRA << " into the memory CRA" << std::endl ;
    std::cout << "Open WIN 1 to 0x0 - 0x" << MAXMEMORYWINDOWENABLE << std::endl ;
    std::cout << "-----------------------------" << std::endl ;

    fecAccess->addMemoryAccess (myKey, MODE_SHARE) ;

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   Retreive the value registers" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;   
    std::cout << "Memory CRA    = 0x" << std::hex << fecAccess->getMemoryChannelCRA(myKey) << std::endl ;    
    std::cout << "Memory Status = 0x" << std::hex << fecAccess->getMemoryChannelStatus(myKey) << std::endl ;    
    std::cout << "Memory WIN1L  = 0x" << std::hex << fecAccess->getMemoryChannelWin1LReg(myKey) << std::endl ;    
    std::cout << "Memory WIN1H  = 0x" << std::hex << fecAccess->getMemoryChannelWin1HReg(myKey) << std::endl ;
    std::cout << "Memory WIN2L  = 0x" << std::hex << fecAccess->getMemoryChannelWin2LReg(myKey) << std::endl ;    
    std::cout << "Memory WIN2H  = 0x" << std::hex << fecAccess->getMemoryChannelWin2HReg(myKey) << std::endl ;      

    if (! noGetchar_) {
      std::cout << "Press <Enter> to continue" ;
      getchar();
    }

    for (int co=1 ; co < maxVal+1 ; co++) {
      frame[co-1]=co%256 ;
      frame1[co-1]=0 ;
    }

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   test of single byte write" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;
    std::cout << "Write " << maxVal << " words" << std::endl ;
    for (int i = 0 ; i < maxVal ; i ++) {
      
      tscType16 AH, AL ;
      
      if (i <= 255) { 
	AH = 0 ;
	AL = i ;
      }
      else {
	AL = (i & 0xFF) ;
	AH = (i & 0xFF00) >> 8 ;
      }

      fecAccess->write (myKey, AH, AL, frame1[i]) ;
    }
      
    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   test of single byte read" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;
    std::cout << "Read back " << maxVal << " words and do comparison" << std::endl ;
    for (int i = 0 ; i < maxVal ; i ++) {
      
      tscType16 AH, AL ;
      
      if (i <= 255) { 
	AH = 0 ;
	AL = i ;
      }
      else {
	AL = (i & 0xFF) ;
	AH = (i & 0xFF00) >> 8 ;
      }
	
      tscType16 val = fecAccess->read (myKey, AH, AL) ;
      if (val != frame1[i]) {
	std::cout << "Single byte read: Values are different in position " << i << "(" << (int)frame1[i] << " != " << val << ")" << std::endl ;
      }
      //else std::cout << (int)frame[i] << "/" << val << std::endl ;
    }

    if (! noGetchar_) {
      std::cout << "Press <Enter> to continue" ;
      getchar();
    }

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   test of multiple byte write" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;
    std::cout << "Write " << maxVal << " words in multiple byte write, read back and do comparison" << std::endl ;
    fecAccess->write ( myKey, 0, 0, frame, maxVal) ;
    for (int i = 0 ; i < maxVal ; i ++) {
	
      tscType16 AH, AL ;
	
      if (i <= 255) { 
	AH = 0 ;
	AL = i ;
      }
      else {
	AL = (i & 0xFF) ;
	AH = (i & 0xFF00) >> 8 ;
      }
      
      tscType16 val = fecAccess->read ( myKey, AH, AL ) ;
      if (val != frame[i]) {
        std::cout << "multiple byte write: Values are different in position " << i << "(" << (int)frame[i] << " != " << val << ")" << std::endl ;
        if ( (i != 0) && ((i % 10) == 0)) 
          if (! noGetchar_) {
            std::cout << "Press <Enter> to continue" ;
            getchar() ;
          }
      }
      //else std::cout << "%d %d\n", frame[i], val) ;
    }

    if (! noGetchar_) {
      std::cout << "Press <Enter> to continue" ;
      getchar();
    }

    for (int co=1 ; co < maxVal+1 ; co++) frame[co-1]=0xFF & ~co%256 ;
    fecAccess->write ( myKey, 0, 0, frame, maxVal) ;

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   test of multiple byte read for " << maxVal << " values" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;
    std::cout << "Read back " << maxVal << " words in multiple byte read, and do comparison" << std::endl ;
    fecAccess->read ( myKey, 0, 0, maxVal, frame1) ;
    for (int i = 0 ; i < maxVal ; i ++) {

      if (frame1[i] != frame[i]) {
        std::cout << "Values are different in position " << i << "(" << (int)frame[i] << " != " << (int)frame1[i] << ")" << std::endl ;
        if ( (i != 0) && ((i % 10) == 0)) 
          if (! noGetchar_) {
            std::cout << "Press <Enter> to continue" ;
            getchar() ;
          }
      }
    }

    for (int co=1 ; co < maxVal+1 ; co++) frame[co-1]=co%256 ;
    fecAccess->write ( myKey, 0, 0, frame, maxVal) ;

    if (! noGetchar_) {
      std::cout << "Press <Enter> to continue" ;
      getchar();
    }

    int nbValues = 0 ;
    do {

      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of multiple byte write" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;

      do {
	std::cout << "Enter the number of values to be written <= " << maxVal << " (0 = finished)" 
		  << std::endl << "\tLimit of the device driver is " <<  DD_USER_MAX_MSG_LENGTH 
		  << std::endl << "\tLimit of the one byte for the length is " << FEC_UPPERLIMIT_LENGTH << std::endl ;
	scanf ("%d", &nbValues) ;
      } 
      while ( (nbValues < 0) || (nbValues > maxVal) ) ; 
      
      fecAccess->write ( myKey, 0, 0, frame, nbValues) ;
  
      for (int i = 0 ; i < nbValues ; i ++) {
        
        std::cout << "0x" << std::hex << (int)fecAccess->read ( myKey, 0, i ) << "(" << std::dec << (int)fecAccess->read ( myKey, 0, i ) << ")" << std::endl ;
      }

      if (! noGetchar_) {
        std::cout << "Press <Enter> to continue" ;
        getchar();
      }

    }
    while (nbValues != 0) ;

    nbValues = 0 ;
    do {
      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of multiple byte read" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;

      do {
	std::cout << "Enter the number of values to be read <= " << maxVal << " (0 = finished)" 
		  << std::endl << "\tLimit of the device driver is " <<  DD_USER_MAX_MSG_LENGTH 
		  << std::endl << "\tLimit of the one byte for the length is " << FEC_UPPERLIMIT_LENGTH << std::endl ;
	scanf ("%d", &nbValues) ;
      }
      while ( (nbValues < 0) || (nbValues > maxVal) ) ; 

      fecAccess->read ( myKey, 0, 0, nbValues, frame) ;
      
      for (int i = 0 ; i < nbValues ; i ++) {
        
        std::cout << "0x" << std::hex << (int)frame[i] << "(" << std::dec << (int)frame[i] << ")" << std::endl ;
      }

      if (! noGetchar_) {
        std::cout << "Press <Enter> to continue" ;
        getchar();
      }
    }
    while (nbValues != 0) ;

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   test of read modify write" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;
    tscType16 val = fecAccess->read (myKey, 0, 0) ;
    fecAccess->write (myKey, 0, 0, CMD_OR, 0xFF) ;
    std::cout << "Applying OR 0x" << std::hex << fecAccess->getMemoryChannelMaskReg(myKey)
	      << " in read modify write on 1 bytes in memory: 0x" << fecAccess->read (myKey, 0, 0)
	      << " (before was 0x" << val << ")" << std::endl ;

    val = fecAccess->read (myKey, 0, 1) ;
    fecAccess->write (myKey, 0, 1, CMD_AND, 0x0) ;
    std::cout << "Applying AND 0x" << std::hex << fecAccess->getMemoryChannelMaskReg(myKey)
	      << " in read modify write on 1 bytes in memory: 0x" << fecAccess->read (myKey, 0, 1)
	      << " (before was 0x" << val << ")" << std::endl ;

    val = fecAccess->read (myKey, 0, 2) ;
    fecAccess->write (myKey, 0, 2, CMD_XOR, 0xFF) ;
    std::cout << "Applying XOR 0x" << std::hex << fecAccess->getMemoryChannelMaskReg(myKey)
	      << " in read modify write on 1 bytes in memory: 0x" << fecAccess->read (myKey, 0, 2)
	      << " (before was 0x" << val << ")" << std::endl ;

    val = fecAccess->read (myKey, 0, 3) ;
    fecAccess->write (myKey, 0, 0, CMD_AND) ;
    std::cout << "Applying AND with the last mask (0x" << std::hex << fecAccess->getMemoryChannelMaskReg(myKey)
	      << ") in read modify write on 1 bytes in memory: 0x" << fecAccess->read (myKey, 0, 3)
	      << " (before was 0x" << val << ")" << std::endl ;

    if (! noGetchar_) {
      std::cout << "Press <Enter> to continue" ;
      getchar();
    }

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   Retreive the value registers" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;   
    std::cout << "Memory CRA    = 0x" << std::hex << fecAccess->getMemoryChannelCRA(myKey) << std::endl ;
    std::cout << "Memory Status = 0x" << std::hex << fecAccess->getMemoryChannelStatus(myKey) << std::endl ;
    std::cout << "Memory WIN1L  = 0x" << std::hex << fecAccess->getMemoryChannelWin1LReg(myKey) << std::endl ;
    std::cout << "Memory WIN1H  = 0x" << std::hex << fecAccess->getMemoryChannelWin1HReg(myKey) << std::endl ;
    std::cout << "Memory WIN2L  = 0x" << std::hex << fecAccess->getMemoryChannelWin2LReg(myKey) << std::endl ;
    std::cout << "Memory WIN2H  = 0x" << std::hex << fecAccess->getMemoryChannelWin2HReg(myKey) << std::endl ;

    if (! noGetchar_) {
      std::cout << "Press <Enter> to continue" ;
      getchar();
    }

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   Performance measurements" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ; 
    
    unsigned long maxPerf ;
    std::cout << "Enter a number (65535 maximum): " << std::endl ;
    scanf ("%lu", &maxPerf) ;

    if (maxPerf > 0) {

      testMemWriteSingle (fecAccess, myKey, maxPerf) ; 
      testMemReadSingle (fecAccess, myKey, maxPerf) ;
      testMemWriteMultiple (fecAccess, myKey, maxPerf) ;
      testMemReadMultiple (fecAccess, myKey, maxPerf) ;
    }
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    // Display status
    std::cout << "FEC status ..." << std::endl ;
    std::cout << "FEC CR0 = 0x" << std::hex << fecAccess->getFecRingCR0(myKey) << std::endl ;
    std::cout << "FEC CR1 = 0x" << std::hex << fecAccess->getFecRingCR1(myKey) << std::endl ;
    std::cout << "FEC SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(myKey) << std::endl ;
    std::cout << "FEC SR1 = 0x" << std::hex << fecAccess->getFecRingSR1(myKey) << std::endl ;
  }

  // delete arrays
  delete []frame ;
  delete []frame1 ;
}

/** 
 * \param fecDevice - access to the address
 * \param index - index of the memory channel
 * \param maxVal - number of accesses
 * \warning this method cannot be accessed directly
 */
void testMemWriteSingle ( FecRingDevice *fecDevice,
                          keyType myKey,
                          unsigned long maxVal ) {

  try {
    std::cout << "**************************************************************" << std::endl ;
    std::cout << "Single byte write in memory for " << maxVal << " values" << std::endl ;
    std::cout << "**************************************************************" << std::endl ;
      
    fecDevice->setChannelEnable(myKey,true) ;    
    fecDevice->setMemoryChannelCRA (myKey, 0xF) ;
    fecDevice->setMemoryChannelWin1HReg (myKey, 0xFFFF) ;
    std::cout << "-----------------------------" << std::endl ;
    std::cout << "Write 0xF into the memory CRA" << std::endl ;
    std::cout << "Open WIN 1 to 0x0 - 0xFFFF" << std::endl ;
    std::cout << "-----------------------------" << std::endl ;

    struct timeval time1, time2;
    struct timezone zone1, zone2;
    gettimeofday(&time1, &zone1);

    for (unsigned long i = 0 ; i < maxVal ; i ++) {
      
      tscType16 AH, AL ;
	
      if (i <= 255) { 
        AH = 0 ;
        AL = i ;
      }
      else {
        AL = (i & 0xFF) ;
        AH = (i & 0xFF00) >> 8 ;
      }

      fecDevice->writeIntoMemory (myKey, AH, AL, 0) ;
    }

    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    double perf = (double) maxVal ;
    perf /= (1024*1024*timesec) ;
    printf( "Delay in seconds for %ld words: %f => %f Mbytes/sec\n", maxVal, timesec, perf) ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
    
    // Display status
    if (fecDevice != NULL) {
      // Display status
      std::cout << "FEC status ..." << std::endl ;
      std::cout << "FEC CR0 = 0x" << std::hex << fecDevice->getFecRingCR0() ;
      std::cout << "FEC CR1 = 0x" << std::hex << fecDevice->getFecRingCR1() ;
      std::cout << "FEC SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() ;
      std::cout << "FEC SR1 = 0x" << std::hex << fecDevice->getFecRingSR1() ;
    }
  }
}

/** 
 * \param fecDevice - access to the address
 * \param index - index of the memory channel
 * \param maxVal - number of accesses
 * \warning this method cannot be accessed directly
 */
void testMemReadSingle ( FecRingDevice *fecDevice,
                         keyType myKey,
                         unsigned long maxVal) {


  try {
    std::cout << "**************************************************************" << std::endl ;
    std::cout << "Single byte read in memory for " << maxVal << " values" << std::endl ;
    std::cout << "**************************************************************" << std::endl ;
      
    fecDevice->setChannelEnable(myKey,true) ;
    fecDevice->setMemoryChannelCRA (myKey, 0xF) ;
    fecDevice->setMemoryChannelWin1HReg (myKey, 0xFFFF) ;    
    std::cout << "-----------------------------" << std::endl ;
    std::cout << "Write 0xF into the memory CRA" << std::endl ;
    std::cout << "Open WIN 1 to 0x0 - 0xFFFF" << std::endl ;
    std::cout << "-----------------------------" << std::endl ;

    struct timeval time1, time2;
    struct timezone zone1, zone2;
    gettimeofday(&time1, &zone1);

    for (unsigned long i = 0 ; i < maxVal ; i ++) {
      
      tscType16 AH, AL ;
	
      if (i <= 255) { 
	AH = 0 ;
	AL = i ;
      }
      else {
	AL = (i & 0xFF) ;
	AH = (i & 0xFF00) >> 8 ;
      }

      fecDevice->readFromMemory (myKey, AH, AL) ;
    }

    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    double perf = (double) maxVal ;
    perf /= (1024*1024*timesec) ;
    printf( "Delay in seconds for %ld words: %f => %f Mbytes/sec\n", maxVal, timesec, perf) ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    // Display status
    if (fecDevice != NULL) {
      // Display status
      std::cout << "FEC status ..." << std::endl ;
      std::cout << "FEC CR0 = 0x" << std::hex << fecDevice->getFecRingCR0() ;
      std::cout << "FEC CR1 = 0x" << std::hex << fecDevice->getFecRingCR1() ;
      std::cout << "FEC SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() ;
      std::cout << "FEC SR1 = 0x" << std::hex << fecDevice->getFecRingSR1() ;
    }
  }
}

/** 
 * \param fecDevice - access to the address
 * \param index - index of the memory channel
 * \param maxVal - number of accesses
 * \warning this method cannot be accessed directly
 */
void testMemWriteMultiple ( FecRingDevice *fecDevice,
                            keyType myKey,
                            unsigned long maxVal) {

  try {

    fecDevice->setChannelEnable(myKey,true) ;
    fecDevice->setMemoryChannelCRA (myKey, 0xF) ;
    fecDevice->setMemoryChannelWin1HReg (myKey, 0xFFFF) ;    
    std::cout << "-----------------------------" << std::endl ;
    std::cout << "Write 0xF into the memory CRA" << std::endl ;
    std::cout << "Open WIN 1 to 0x0 - 0xFFFF" << std::endl ;
    std::cout << "-----------------------------" << std::endl ;

    tscType8 *frame = new tscType8[maxVal] ;
    for (unsigned long i = 1 ; i <= maxVal ; i ++) frame[i-1] = i % 256 ;

    struct timeval time1, time2;
    struct timezone zone1, zone2;
    gettimeofday(&time1, &zone1);
    fecDevice->writeIntoMemory (myKey, 0, 0, frame, maxVal) ; //, true) ;
    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    double perf = (double) maxVal ;
    perf /= (1024*1024*timesec) ;

    std::cout << "*********************************************************************" << std::endl ;
    //    if (burstMode)
    //      std::cout << "Multiple bytes write in memory for " << maxVal << " values in burst mode" << std::endl ;
    //    else
    std::cout << "Multiple bytes write in memory for " << maxVal << " values" << std::endl ;
    std::cout << "Delay in seconds for " << maxVal << " words: " << timesec << " => " << perf << " Mbytes/sec" << std::endl ;
    std::cout << "*********************************************************************" << std::endl ;

    // delete array
    delete []frame ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    // Display status
    if (fecDevice != NULL) {
      // Display status
      std::cout << "FEC status ..." << std::endl ;
      std::cout << "FEC CR0 = 0x" << std::hex << fecDevice->getFecRingCR0() << std::endl ;
      std::cout << "FEC CR1 = 0x" << std::hex << fecDevice->getFecRingCR1() << std::endl ;
      std::cout << "FEC SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
      std::cout << "FEC SR1 = 0x" << std::hex << fecDevice->getFecRingSR1() << std::endl ;
    }
  }
}

/**
 * \param fecDevice - access to the address
 * \param index - index of the memory channel
 * \param maxVal - number of accesses
 * \warning this method cannot be accessed directly
 */
void testMemReadMultiple ( FecRingDevice *fecDevice,
                           keyType myKey,
                           unsigned long maxVal) {


  try {

    std::cout << "*********************************************************************" << std::endl ;
    std::cout << "Multiple bytes read in memory for " << maxVal << " values" << std::endl ;
    std::cout << "*********************************************************************" << std::endl ;

    fecDevice->setChannelEnable(myKey,true) ;
    fecDevice->setMemoryChannelCRA (myKey, 0xF) ;
    fecDevice->setMemoryChannelWin1HReg (myKey, 0xFFFF) ;    
    std::cout << "-----------------------------" << std::endl ;
    std::cout << "Write 0xF into the memory CRA" << std::endl ;
    std::cout << "Open WIN 1 to 0x0 - 0xFFFF" << std::endl ;
    std::cout << "-----------------------------" << std::endl ;

    tscType8 *frame = new tscType8[maxVal] ;
    struct timeval time1, time2;
    struct timezone zone1, zone2;
    gettimeofday(&time1, &zone1);

    fecDevice->readFromMemory (myKey, 0, 0, maxVal, frame) ;

    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    double perf = (double) maxVal ;
    perf /= (1024*1024*timesec) ;
    std::cout << "*********************************************************************" << std::endl ;
    std::cout << "Multiple bytes read in memory for " << maxVal << " values" << std::endl ;
    std::cout << "Delay in seconds for " << maxVal << " words: " << timesec << " => " << perf << " Mbytes/sec" << std::endl ;
    std::cout << "*********************************************************************" << std::endl ;

    // delete array
    delete []frame ;
  }

  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    // Display status
    std::cout << "FEC status ..." << std::endl ;
    std::cout << "FEC CR0 = 0x" << std::hex << fecDevice->getFecRingCR0() << std::endl ;
    std::cout << "FEC CR1 = 0x" << std::hex << fecDevice->getFecRingCR1() << std::endl ;
    std::cout << "FEC SR0 = 0x" << std::hex << fecDevice->getFecRingSR0() << std::endl ;
    std::cout << "FEC SR1 = 0x" << std::hex << fecDevice->getFecRingSR1() << std::endl ;
    
    std::cout << "CCU status ..." << std::endl ;
    std::cout << "CCU CRA = 0x" << std::hex << fecDevice->getCcuCRA(myKey) << std::endl ;
    std::cout << "CCU SRE = 0x" << std::hex << fecDevice->getCcuSRE(myKey) << std::endl ;
  }
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       PIA channels                                      */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** 
 * <p>Command: -pia
 * \param fecAccess - FEC Access object
 * \param  fecAddress - slot of the FEC.
 * \param  ringAddress - slot of the ring.
 * \param  ccuAddress - address of the first CCU of the ring.
 * \param  channelAddress - address of the channel.
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
void testPIAfunctions (FecAccess *fecAccess,
                       tscType8 fecAddress,
                       tscType8 ringAddress,
                       tscType8 ccuAddress,
                       tscType8 channelAddress,
                       long loop, unsigned long tms ) { 

  keyType myKey = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,0) ; // Build the key

  try {

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   test of addpiaaccess" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;                    
    fecAccess->addPiaAccess(myKey,MODE_SHARE) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return ;
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      std::cout << "-----------------------------" << std::endl ;
      std::cout << "test on pia GCR" << std::endl ;
      std::cout << "-----------------------------" << std::endl ;

      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of getpiachannelgcr" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;                       
      std::cout << "GCR = 0x" << fecAccess->getPiaChannelGCR(myKey) << std::endl ;
      std::cout << "(should be : 0x0)" << std::endl ; 

      if (channelAddress != 0x12) { // PIA channel CCU 25

        std::cout << "     .       " << std::endl ;
        std::cout << "    / \\     " << std::endl ;
        std::cout << "   / | \\    " << std::endl ;
        std::cout << "  /  |  \\   test of setpiachannelgcr" << std::endl ;
        std::cout << " /   .   \\  " << std::endl ;
        std::cout << "/_________\\ " << std::endl ;                        
        fecAccess->setPiaChannelGCR(myKey,0x11) ;

        std::cout << "value of GCR after setpiachannelgcr : " << std::endl ;
      std::cout << "GCR = 0x" << fecAccess->getPiaChannelGCR(myKey) << std::endl ;
        std::cout << "(should be : 0x11)" << std::endl ; 
      }
      
      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of setpiachannelgcr(rmw)" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;                           
      fecAccess->setPiaChannelGCR(myKey,0x11,CMD_XOR) ;
      
      std::cout << "value of GCR after setpiachannelgcr(rmw) : " << std::endl ;
      std::cout << "GCR = 0x" << std::hex <<  fecAccess->getPiaChannelGCR(myKey) << std::endl ;
      std::cout << "(should be : 0x0)" << std::endl ; 

      // Set to 0x0 (to be sure) !
      fecAccess->setPiaChannelGCR(myKey,INITCCU25PIAGCR) ;

      if (! noGetchar_) {
        std::cout << "Press <Enter> to continue" ;
        getchar() ;
      }

      std::cout << "-----------------------------" << std::endl ;
      std::cout << "test on pia DDR" << std::endl ;
      std::cout << "-----------------------------" << std::endl ;

      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of getpiachannelddr" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;   
      std::cout << "DDR = 0x" << std::hex << (int)fecAccess->getPiaChannelDDR(myKey) << std::endl ;
      std::cout << "(should be : 0x0)" << std::endl ; 
      
      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of setpiachannelddr" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;
      fecAccess->setPiaChannelDDR(myKey,0x1) ;

      std::cout << "value of DDR after setpiachannelddr : " << std::endl ;
      std::cout << "DDR = 0x" << std::hex << (int)fecAccess->getPiaChannelDDR(myKey) << std::endl ;
      std::cout << "(should be : 0x1)" << std::endl ; 

      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of setpiachannelddr(rmw)" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;
      fecAccess->setPiaChannelDDR(myKey,0x1,CMD_XOR) ;
    
      std::cout << "value of DDR after setpiachannelddr(rmw) : " << std::endl ;
      std::cout << "DDR = 0x" << std::hex << (int)fecAccess->getPiaChannelDDR(myKey) << std::endl ;
      std::cout << "(should be : 0x0)" << std::endl ; 

      // Set the output 
      fecAccess->setPiaChannelDDR(myKey,0xFF) ;
      std::cout << "Set the PIA as output ; DDR = 0x" << std::hex << fecAccess->getPiaChannelDDR(myKey) << " (should be 0xFF)" << std::endl ;

      if (! noGetchar_) {
        std::cout << "Press <Enter> to continue" ;
        getchar() ;
      }

      std::cout << "-----------------------------" << std::endl ;
      std::cout << "test on pia Data Reg" << std::endl ;
      std::cout << "-----------------------------" << std::endl ;

      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of getpiachanneldatareg" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;  
      std::cout << "DATAREG = 0x" << std::hex << (int)fecAccess->getPiaChannelDataReg(myKey) << std::endl ;
  
      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of setpiachannelDataReg" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;    
      fecAccess->setPiaChannelDataReg(myKey,0xFF) ;

      std::cout << "value of DataReg after setpiachannelDataReg : " << std::endl ;
      std::cout << "DataReg = 0x" << std::hex << (int)fecAccess->getPiaChannelDataReg(myKey) << std::endl ;
      std::cout << "(should be : 0xFF)" << std::endl ; 

      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of setpiachannelDataReg" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;    
      fecAccess->setPiaChannelDataReg(myKey,0x20) ;

      std::cout << "value of DataReg after setpiachannelDataReg : " << std::endl ;
      std::cout << "DataReg = 0x" << std::hex << (int)fecAccess->getPiaChannelDataReg(myKey) << std::endl ;
      std::cout << "(should be : 0x20)" << std::endl ; 

      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of setpiachannelDataReg(rmw)" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ; 
      fecAccess->setPiaChannelDataReg(myKey,0x20,CMD_XOR) ;
    
      std::cout << "value of DataReg after setpiachannelDataReg(rmw) : " << std::endl ;
      std::cout << "DataReg = 0x" << std::hex << (int)fecAccess->getPiaChannelDataReg(myKey) << std::endl ;
      std::cout << "(should be : 0x0)" << std::endl ; 

      if (! noGetchar_) {
        std::cout << "Press <Enter> to continue" ;
        getchar() ;
      }

      std::cout << "-----------------------------" << std::endl ;
      std::cout << "test on pia Status Reg" << std::endl ;
      std::cout << "-----------------------------" << std::endl ;

      std::cout << "     .       " << std::endl ;
      std::cout << "    / \\     " << std::endl ;
      std::cout << "   / | \\    " << std::endl ;
      std::cout << "  /  |  \\   test of getpiachannelStatus" << std::endl ;
      std::cout << " /   .   \\  " << std::endl ;
      std::cout << "/_________\\ " << std::endl ;  
      std::cout << "STATUS = 0x" << std::hex << (int)fecAccess->getPiaChannelStatus(myKey) << std::endl ;
      std::cout << "(should be : 0x0)" << std::endl ; 

      if (! noGetchar_) {
        std::cout << "Press <Enter> to continue" ;
        getchar() ;
      }
    }

    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }
}

/** 
 * <p>Command: -piaReset
 * \param fecAccess - FEC Access object
 * \param  fecAddress - slot of the FEC.
 * \param  ringAddress - slot of the ring.
 * \param  ccuAddress - address of the first CCU of the ring.
 * \param  channelAddress - address of the channel.
 * \param delayActiveReset - delay where the reset is active in microseconds
 * \param intervalDelayReset - delay between two reset
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
std::string testPIAResetfunctions (FecAccess *fecAccess,
                            tscType8 fecAddress,
                            tscType8 ringAddress,
                            tscType8 ccuAddress,
                            tscType8 channelAddress,
                            tscType16 value,
                            unsigned long delayActiveReset,
                            unsigned long intervalDelayReset,
                            long loop, unsigned long tms ) { 

  std::string result;

  PiaResetAccess *piaResetAccess ;
  keyType myKey = buildCompleteKey(fecAddress,ringAddress,ccuAddress,channelAddress,0) ; // Build the key

  try {

    std::cout << "     .       " << std::endl ;
    std::cout << "    / \\     " << std::endl ;
    std::cout << "   / | \\    " << std::endl ;
    std::cout << "  /  |  \\   test of piaReset" << std::endl ;
    std::cout << " /   .   \\  " << std::endl ;
    std::cout << "/_________\\ " << std::endl ;                    
    piaResetAccess = new PiaResetAccess(fecAccess, myKey, 0xFF) ;
  }
  catch (FecExceptionHandler &e) {
      
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;

    if (errorCounterFlag_) displayStatus ( &e, 0, fecAccess, stdchan_ ) ;

    return "Pia access creation error";
  }

  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if (loop != 1)
      std::cout << "----------------------- Loop " << loopI+1 << std::endl ;

    try {

      // Create the piaResetDescription
      piaResetDescription piaD ( myKey, (unsigned long)delayActiveReset, (unsigned long)intervalDelayReset, (tscType8)value ) ;

      piaResetAccess->setValues (piaD) ;

      piaResetDescription *piaD1 = piaResetAccess->getValues ( ) ;

      if (piaD == *piaD1) {
        result = "Pia reset comparison ok";
	std::cout << result << std::endl ;
      }
      else {
        result = "Pia reset comparison error";
	std::cout << result << std::endl ;
	
      }

      delete piaD1 ;
    }

    catch (FecExceptionHandler &e) {
      
      std::cout << "------------ Exception ----------" << std::endl ;
      std::cout << e.what()  << std::endl ;
      std::cout << "---------------------------------" << std::endl ;

      if (errorCounterFlag_) displayStatus ( &e, loopI, fecAccess, stdchan_ ) ;

      result = "Access to hardware error";
    }

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  // delete the PIA reset
  try {
    delete piaResetAccess ;
  }
  catch (FecExceptionHandler &e) {    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }

  return result;
}

/**
 * This method wait for PIA interrupts
 * <p>command: -waitPiaInterrupt
 * \param fecAccess - FEC access object
 * \param fecAddress - FEC address
 * \param piaChannel - piaChannel
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \warning the loop is not used
 */
void waitPiaInterrupt ( FecAccess *fecAccess, 
                        tscType8 fecAddress,
			tscType8 ringAddress,
                        tscType16 piaChannel,
                        long loop, unsigned long tms ) {

  std::list<keyType> *ccuList = NULL ;
  try {

    if (fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) != NULL) {
      
      // Scan the ring for each FECs and display the CCUs
      ccuList = fecAccess->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
      
      if (ccuList != NULL) {

        // Wait for any alarms
        for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

          for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
          
            keyType index = *p ;

            // After each alarm, the bits are cleared => reenable the alarms
            // This method also enable interrupts in PIA channel. The index must contain
            // the value of the given channel or 0 if you want all PIA channels enabled.
            if (piaChannel == 0) {

	      std::cout << std::hex << "FEC 0x" << getFecKey(index) 
			<< " Ring 0x " << getRingKey(index) 
			<< " CCU 0x" << getCcuKey(index)
			<< " found => enable alarms for all PIA channels" << std::endl ;
            }
            else {
	      std::cout << std::hex << "FEC 0x" << getFecKey(index) 
			<< " Ring 0x " << getRingKey(index) 
			<< " CCU 0x" << getCcuKey(index)
			<< " found => enable alarm for PIA channel " << piaChannel << std::endl ;
            }
	    
            fecAccess->setPiaInterruptEnable ( index | setChannelKey(piaChannel) ) ;
          }
          
          // Wait for ever to any alarms
          tscType8 frame[DD_USER_MAX_MSG_LENGTH] = {0} ;
          fecAccess->waitForAnyCcuAlarms (buildFecRingKey(fecAddress,ringAddress), frame) ;

          // Display Frame
          tscType16 realSize = frame[2] ;
          if (realSize & FEC_LENGTH_2BYTES) // Size in two bytes length
            realSize = ((frame[2] & 0x7F) << 8) + frame[3] + 1;
          
          // Date and time
          time_t timestamp = time(NULL) ;
          fprintf (stdchan_, "timeStamp=%ld\n", timestamp) ;
          fprintf (stdchan_, "Warning emitted by a device, Frame = ") ;
          fprintf (stdchan_, "{ ") ;
          for (int i = 0 ; i < (realSize+3) ; i ++ )
            fprintf (stdchan_, "0x%x ", frame[i]) ;
          fprintf (stdchan_, "} )\n");
	  
          for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	    
            keyType index = *p ;
	    
            // Clear de l'interrupts => Write PIA-GCR<2> = 1(0x30-0x33)
            fecAccess->setPiaClearInterrupts ( index | setChannelKey(piaChannel) ) ;
          }
	  
	  if (! noGetchar_) {
	    // Display a message to clear the source
	    std::cout << "Please Clear the source before press enter" << std::endl ;
	    getchar() ;
	  }
	  
          // Wait
          if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
        }
      }
      else {
        std::cout << "No CCU found on FECs" << std::endl ;
      }  
    }
    else {
      std::cout << "No FEC device driver found" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
  
  // Delete = already store in the table of FEC device class
  delete ccuList ;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       Reconfiguration functions                         */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** This method is able to switch ring A to ring B for the FEC and 
 * each CCU
 * <p>command: -redundancy
 * \param fecAccess - FEC access object
 * \param fecAddress - FEC slot
 * \param ccuAddresses - array of CCU addresses (or FEC)
 * \param numberOfCCU - number of CCU to be reconfigured
 * \bug the command is removed and replaced by -bypassed
 */
void testRedundancyRing ( FecAccess *fecAccess, 
			  tscType8 fecAddress,
			  tscType8 ringAddress,
			  uint ccuAddresses[][3], 
			  uint numberOfCCU ) {

  keyType indexFecRing = buildFecRingKey(fecAddress,ringAddress) ;

  // Find the FECs
  try {
    // Scan the PCI slot for device driver loaded
    fecAccess->getFecRingDevice (indexFecRing) ;
  }
  catch (FecExceptionHandler &e) {  

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot find any FECs on the ring: " << e.what() << std::endl ; 
    std::cerr << "**************************************************" << std::endl ;

    return ;
  }

  // Ask to not initialise the Fec Device at the start point (FecRingDevice
  // object creation
  bool fecDeviceInit = fecAccess->getFecRingDeviceInit () ;
  if (fecDeviceInit == true) fecAccess->setFecRingDeviceInit (false) ;

  // disable the IRQ
  std::cout << "Disable the IRQ" << std::endl ;
  fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

  try {
    //   FEC 0        CCU 0x10      CCU 0x18      CCU 0x7F
    //         Input A
    //            Ouput B
    // { {0x0, 0, 1}, {0x10, 0, 0}, {0x18, 1, 0}, {0x7F, 0, 0}

    for (unsigned int i = 0 ; i < numberOfCCU ; i++) {  

      // FEC
      if (ccuAddresses[i][0] == 0x0) {

	// Retreive the CR0 value from cache
	tscType32 fecCR0 = fecAccess->getFecRingCR0(indexFecRing) ;

	// Make an or with the value => input
	if (ccuAddresses[i][1] == 0) fecCR0 &= 0xFFEF ;
	else
	  fecCR0 = (fecCR0 & 0xFFEF) | 0x0010 ;

	// Make an or with the value => output
	if (ccuAddresses[i][2] == 0) fecCR0 &= 0xFFF7 ;
	else
	  fecCR0 = (fecCR0 & 0xFFF7) | 0x0008 ;
	
	try {
	  fecAccess->setFecRingCR0 (indexFecRing, fecCR0) ;
	}
	catch (FecExceptionHandler &e) { }

      }
      else { // CCU
      
	keyType index = buildCompleteKey(fecAddress,ringAddress,ccuAddresses[i][0],0,0) ; 
	
	tscType32 CRC = 0 ;
	
	// Make an or with the value => input
	if (ccuAddresses[i][1] == 0) CRC &= 0xFE ;
	else
	  CRC = (CRC & 0xFE) | 0x01 ;

	// Make an or with the value => output
	if (ccuAddresses[i][2] == 0) CRC &= 0xFD ;
	else
	  CRC = (CRC & 0xFD) | 0x02 ;

	try {
	  fecAccess->setCcuCRC (index, CRC) ;
	}
	catch (FecExceptionHandler &e) { }
      }
    }

    try {
      // Twice ?
      fecAccess->fecRingRelease( indexFecRing ) ;
std::cout << "FEC SR0 = 0x" << std::hex << fecAccess->getFecRingSR0( indexFecRing ) << std::endl ;
    }
    catch (FecExceptionHandler &e) { }
    
  }
  catch (FecExceptionHandler &e) { 

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Initialise the Fec object or not at the start point
  fecAccess->setFecRingDeviceInit (fecDeviceInit) ;
  
  // enable the IRQ
  std::cout << "Reenable the IRQ" << std::endl ;
  fecAccess->getFecRingDevice (indexFecRing)->setIRQ(true) ;
}

/** This method read the DCU and the CCU registers during a certain amount of time
 * give in argument
 * \param fecAccess - access to the FECs
 * \param ccuList - list of the CCUs
 */
unsigned int readDcuCcuMinutes ( FecAccess *fecAccess,
                                 tscType16 fecAddress,
				 tscType16 ringAddress,
                                 std::list<keyType> *ccuList,
				 tscType8 dcuChannel,
                                 keyType ccuDummyIndex,
                                 std::vector<std::string> &results,
                                 int dcuTest) {

  keyType indexFecRing = buildFecRingKey(fecAddress,ringAddress) ;

  unsigned int transactionNumber = 0 ;

  //double secondsForTest = (360/3) ; // => 5 mins
  double secondsForTest = 1 ; // => 1 mins
  //double secondsForTest = 0 ; // => nothing is performed

  struct timeval time3, time4 ;
  struct timezone zone3, zone4 ;

  try {
    // Re-enable the IRQ
    fecAccess->getFecRingDevice (indexFecRing)->setIRQ(true) ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "WARNING: cannot enable the IRQ: " << e.what() << std::endl ;
  }
 
  // Read during some minutes the status and control register of the CCU and the DCU
  dcuAccess *dcu = NULL ;
  for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {

    keyType index = *p ;

    results.push_back("   <AccessToCcuAddress = 0x" + IntToString(getCcuKey(index)) +">");

    try {
      // Delete = already store in the table of FEC device class
      if (dcu != NULL) {
        delete dcu ;
        // 1 transation for the disable of the corresponding i2c channel
        transactionNumber +=1 ;
        dcu = NULL ;
      }
    }
    catch (FecExceptionHandler &e) {
      std::cout << "WARNING: cannot delete the DCU access: " << e.what() << std::endl ;
    }

    gettimeofday(&time3, &zone3);
    gettimeofday(&time4, &zone4);
    double timesec3 = time3.tv_sec + time3.tv_usec * 0.000001 ;
    double timesec4 = time4.tv_sec + time4.tv_usec * 0.000001 ;
    double timesecT = 0 ;

    bool displayCCU = true, displayDCU = true ;
    int error1 = 0, error2 = 0, totalTest = 0 ;

    while (timesecT < secondsForTest) {
     
      // Read status/control registers of the CCUs
      try {
        if (displayCCU) {
          std::cout << "CCU registers(0x" << std::hex << getCcuKey(index) << ") = " << (int)fecAccess->getCcuSRB (index) << " " << (int)fecAccess->getCcuCRA (index) << " " << (int)fecAccess->getCcuCRE (index) << " " << (int)fecAccess->getCcuSRA (index) << " " << (int)fecAccess->getCcuSRE (index) << std::endl ;

          transactionNumber += 5 ;

          displayCCU = false ;
        }
      }
      catch (FecExceptionHandler &e) {
        error1 ++ ;
      }     
     
      // Read the value of the DCUs and display for the first time the channel except for the dummy CCU
      if ( (getCcuKey(index) != getCcuKey(ccuDummyIndex) && (dcuTest < 3)) ||
           (getCcuKey(index) == getCcuKey(ccuDummyIndex) && (dcuTest == 1)) ) {
        try {
          if (dcu == NULL) {
            dcu = new dcuAccess (fecAccess, index | setChannelKey(dcuChannel)) ;

            // 2 transactions done:
            //  * enable the channel
            //  * initialise the i2c channel CRA
            transactionNumber += 2 ;
          }

          dcuDescription *mondcu= dcu->getValues() ;

          // 3 transactions for the DCU hard id
          // + 3 transactions per channel
          transactionNumber += 3 + 3 * 8 ;
         
          if (displayDCU) {
            displayDCU = false ;
            std::cout << "DCU(0x" << std::hex << getCcuKey(index) << "): "
                 << std::dec << mondcu->getDcuHardId()
                 << ": channel 6 = " << std::dec << mondcu->getDcuChannel6() 
                 << ": channel 7 = " << std::dec << mondcu->getDcuChannel7() 
                 << std::endl ;

          }
          delete mondcu ;
        }
        catch (FecExceptionHandler &e) {
          error2 ++ ;
        }
      }
     
      gettimeofday(&time4, &zone4);
      timesec4 = time4.tv_sec + time4.tv_usec * 0.000001 ;
      timesecT = timesec4 - timesec3 ;

      totalTest ++ ;
    }

//     gettimeofday(&time4, &zone4);
//     timesec4 = time4.tv_sec + time4.tv_usec * 0.000001 ;
//     timesecT = timesec4 - timesec3 ;
//     std::cout << "Number of loop is " << totalTest << " for " << transactionNumber << " transactions (" << timesecT << " seconds)" << std::endl ;

    // Display errors
    if (error1) {
      //std::cout << "ERROR: cannot read the control and status register of the CCU 0x" << getCcuKey(index) << ": " << e.what() << std::endl ;
      std::cout << "ERROR: cannot read the control and status register of the CCU 0x" << getCcuKey(index) << " ( " << std::dec << error1 << " / " << totalTest << ")" << std::endl ;

      results.push_back("      <AccessCcu>Nok</AccessCcu>");

    }
    else results.push_back("      <AccessCcu>Ok</AccessCcu>");

    // Display errors
    if (error2) {
      //std::cout << "ERROR: cannot read the DCU of the CCU 0x" << getCcuKey(index) << ": " << e.what() << std::endl ;
      std::cout << "ERROR: cannot read the DCU of the CCU 0x" << getCcuKey(index) <<  " ( " << std::dec << error2 << " / " << totalTest << ")" << std::endl ;

      results.push_back("      <AccessDcu>Nok</AccessDcu>");
    }
    else results.push_back("      <AccessDcu>Ok</AccessDcu>");

    results.push_back("   </AccessToCcuAddress>");
  }

  try {
    // Delete = already store in the table of FEC device class
    if (dcu != NULL) delete dcu ;

    // 1 transation for the disable of the corresponding channel
    transactionNumber += 1 ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "WARNING: cannot delete the DCU access: " << e.what() << std::endl ;
  }

  try {
    // Re-enable the IRQ
    fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "ERROR: cannot enable the IRQ: " << e.what() << std::endl ;
  }

  return (transactionNumber) ;
}


/** Check if the list of the CCU returned is correct in regards of the number of CCU and CCU addresses expected
 * \param ccuList - list of the CCU found after the bypass
 * \param numberCCU - number of CCU orginaly found
 * \param ccuAddresses - addresses of the CCU orginaly found
 * \param ccuIndex - index in the array ccuAddresses where the CCU is bypassed => if equal to -1 then the number of result and the original values must be the same
 * \return return true if ok
 */
bool checkRedundancyAddress ( std::list<keyType> *ccuList,
                              uint numberCCU,
                              keyType *ccuAddresses,
                              int ccuIndex,
                              bool sizeNotSame = true) {
  bool error = false ;

  //std::cout << numberCCU << " / " << ccuList->size() << std::endl ;
 
  // Check the size: size(ccuList) = numberCCU-1
  if (((sizeNotSame) && ((numberCCU-1) != ccuList->size())) ||
      ((!sizeNotSame) && (numberCCU != ccuList->size())) ) {

    std::cout << "ERROR: the number of CCU expected is different than the CCU found in the scan ring" ;
    if (ccuIndex != -1)
      std::cout << " for CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuIndex]) << std::endl ;
    else
      std::cout << std::endl ;
   
    error = true ;
  }

  // Check the addresses
  int pos = 0 ;
  for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end() && !error;p++) {

    if (pos == ccuIndex) pos ++ ;

    //std::cout << "pos = " << std::dec << pos << " / " << ccuIndex << std::endl ;
    //std::cout << "ccuAddresses[pos] = 0x" << std::hex << getCcuKey(ccuAddresses[pos]) << std::endl ;
    //std::cout << "index = 0x" << getCcuKey(*p) << std::endl ;

    keyType index = *p ;
    if (index != ccuAddresses[pos]) {
     
      std::cout << "ERROR: Found CCU address 0x" << std::hex << getCcuKey(index) << " but the CCU must be 0x" << getCcuKey(ccuAddresses[pos]) << std::endl ;
      error = true ;
    }

    pos ++ ;
  }

  return error ;
}

/** This method bypass one CCU from the ring depending of the configuration
 * \param ring
 * \param ccu1 - CCU 1
 * \param ccu3 - CCU 3
 * \param dummy - for the dummy CCU (0x0 => to put the dummy CCU specified in ccu3, 0x1 for any other configuration, != 0x1 and != 0x0 to bypass the lastest CCU using the dummy CCU
 * \warning ccu1 or ccu3 is 0 then it is considered as the FEC
 * TOB modification bypass
 * List of the CCUs are: 0x0 (FEC), 0x1, 0x2, 0x5, 0x9, 0x6, 0x6f, 0x34, 0x7E
 * bypass options are: (ring, CCU/FEC, CCU/FEC)
 * Please all the position here should be tested
 * bypass (0x0,0x0,0x1) will bypass use the ring B of the FEC and CCU 0x1 on the ring 0
 * bypass (0x0,0x0,0x2) will bypass the CCU 0x1 on ring 0
 * bypass (0x0,0x1,0x5) will bypass the CCU 0x2 on ring 0
 * ...
 * bypass (0x0,0x34,0x7E,0x0) will put the dummy CCU in the ring 0
 * bypass (0x0,0x6F,0x0,0x7E) will bypass the CCU 0x34 on ring 0 using the CCU 0x7E
 */
void bypass ( FecAccess *fecAccess, unsigned int fecSlot_, unsigned int ring, unsigned short ccu1, unsigned short ccu3, unsigned short dummy ) {

  unsigned long tms = 10 ;

  keyType indexFecRing = buildFecRingKey(fecSlot_,ring) ;
  keyType ccuOne       = buildCompleteKey(fecSlot_,ring,ccu1,0,0) ;
  keyType ccuThree     = buildCompleteKey(fecSlot_,ring,ccu3,0,0) ;
  keyType ccuDummy     = buildCompleteKey(fecSlot_,ring,dummy,0,0) ;

  try {
    std::cout << "-----------------------------------------" << std::endl ;
    std::cout << "Starting the redundancy of the ring by bypassing one of the CCU (SR0 = " 
	      << std::hex << (int)fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Cannot read the SR0: " << e.what() << std::endl ;
    std::cout << "Continue anyway" << e.what() << std::endl ;
  }

  try {
    unsigned int timeout = 10 ;
    do {
      // Make a reset
      fecAccess->fecHardReset (indexFecRing) ;
      fecAccess->fecRingReset (indexFecRing) ;
      fecAccess->fecRingResetB ( indexFecRing ) ;
      if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
	
	std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex << (int)fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	emptyFIFOs ( fecAccess, indexFecRing, false ) ;
      }
      
      // Log message
      std::cout << "FEC reset Performed (SR0 = 0x" << std::hex << (int)fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
      timeout -- ;
	
      if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) usleep (tms) ;
    }
    while (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing)) && timeout > 0) ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Problem during the reset of the ring: " << e.what() << std::endl ;
    std::cout << "Continue anyway" << e.what() << std::endl ;
  }

  try {
    if (dummy == 0x0) { //dummy = true, ccu1 = 0x34, ccu3 = 0x7E
      fecAccess->setFecRingCR0(indexFecRing, FEC_CR0_SELSERIN) ;
      fecAccess->setCcuCRC(ccuOne, CCU_CRC_SSP) ;  // output B
      fecAccess->fecRingRelease ( indexFecRing ) ;
    }
    else if (dummy != 0x1) {

      fecAccess->setFecRingCR0(indexFecRing, FEC_CR0_SELSERIN) ;
      fecAccess->setCcuCRC(ccuOne, CCU_SRC_OUTPUTPORT) ;  // output B
      fecAccess->setCcuCRC(ccuDummy, CCU_SRC_OUTPUTPORT) ;  // output B
    }
    else if (ccu1 == 0x0) { // change the FEC
      fecAccess->setFecRingCR0(indexFecRing, FEC_CR0_SELSEROUT) ;
      fecAccess->setCcuCRC(ccuThree, CCU_CRC_ALTIN) ;
      fecAccess->fecRingRelease ( indexFecRing ) ;
    }
    else { // CCU in the middle
      fecAccess->setCcuCRC (ccuOne, CCU_CRC_SSP) ;
      fecAccess->setCcuCRC (ccuThree, CCU_CRC_ALTIN) ;
      fecAccess->fecRingRelease ( indexFecRing ) ;
    }
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Problem during the redundancy: " << e.what() << std::endl ;
  }

  try { // Empty the FIFOs
    emptyFIFOs ( fecAccess, indexFecRing, false ) ;    
    std::cout << "End of the redundancy (SR0 = 0x" 
	      << std::hex << (int)fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;    
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Error during the read of the SR0: " << e.what() << std::endl ;
  }
  
  try {
    // get the CCU list and display it
    std::list<keyType> *ccuList = fecAccess->getCcuList ( indexFecRing ) ;
    std::cout << "Found the CCU " << std::hex ;
    for (std::list<keyType>::iterator it = ccuList->begin() ; it != ccuList->end() ; it++) {
      std::cout << getCcuKey(*it) << "\t" ;
    }
    delete ccuList ;
    std::cout << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "Problem during the reset of the ring: " << e.what() << std::endl ;
    std::cout << "Continue anyway" << e.what() << std::endl ;
  }

  std::cout << "-------------------------------" << std::endl ;
}


/** Clear the error in the FECs and in the different CCUs
 */
void clearFecCcuErrorRegisters ( FecAccess *fecAccess,
				 tscType8 fecAddress,
				 tscType8 ringAddress,
				 std::list<keyType> ccuAddresses ) {

  keyType indexFecRing = buildFecRingKey(fecAddress,ringAddress) ;

  try {
    // Clear the FEC register 1 to have a clear view of the errors coming
    fecAccess->setFecRingCR1(indexFecRing, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Error during the set of the FEC CR1: " << e.what() << std::endl;
  }

  if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
    std::cerr << "SR0 is not correct: clear only the SR1 of the FEC" << std::endl ;
  }
  else {
    //std::cerr << "Clear errors on CCU SRA and FEC SR1" << std::endl ;
    keyType previous = 0 ;
    for (std::list<keyType>::iterator it = ccuAddresses.begin() ; it != ccuAddresses.end() ; it ++) {
      
      try {
	keyType index = *it ;
	//std::cout << "Clear errors on CCU 0x" << std::hex << index << std::endl ;
	fecAccess->setCcuCRA (index, CCU_CRA_CLRE) ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "Error during the set of CCU CRA: " << e.what() << std::endl ;
	displayFecStatus(fecAccess,buildFecRingKey(fecAddress,ringAddress)) ;
	if (previous != 0) {
	  std::cout << "CCU " << getCcuKey(previous) << std::endl;  
	  displayCCUSRA (fecAccess->getCcuSRA(previous)) ;
	  displayCCUCRC (fecAccess->getCcuCRC(previous)) ;
	  displayCCUSRC (fecAccess->getCcuSRC(previous)) ;
	}
// 	std::list<keyType>::iterator it1 = it ;
// 	it1++ ;
// 	if (it1 != ccuAddresses.end()) {
// 	  std::cout << "CCU " << getCcuKey(*it) << std::endl;  
// 	  displayCCUSRA (fecAccess->getCcuSRA(*it1)) ;
// 	  displayCCUSRC (fecAccess->getCcuSRC(*it1)) ;
// 	  displayCCUSRC (fecAccess->getCcuCRC(*it1)) ;
// 	}
	std::cout << "CCU " << getCcuKey(*it) << std::endl;  
	displayCCUSRB (fecAccess->getCcuSRB(*it)) ;
	displayCCUSRA (fecAccess->getCcuSRA(*it)) ;
	displayCCUCRC (fecAccess->getCcuCRC(*it)) ;
	displayCCUSRC (fecAccess->getCcuSRC(*it)) ;
	getchar() ;

	try {
	  keyType index = *it ;
	  //std::cout << "Clear errors on CCU 0x" << std::hex << index << std::endl ;
	  fecAccess->setCcuCRA (index, CCU_CRA_CLRE) ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "Error during the set of CCU CRA: " << e.what() << std::endl ;
	}
      }
      previous = *it ;
    }
  }
}

/** Display the CCU SRA and FEC SR1
 */
void displayFecCcuRegisters ( FecAccess *fecAccess,
			      tscType8 fecAddress,
			      tscType8 ringAddress,
			      std::list<keyType> ccuAddresses ) {

  keyType indexFecRing = buildFecRingKey(fecAddress,ringAddress) ;
  
  try {
    // Display FEC SR1
    displayFECSR1 (fecAccess->getFecRingSR1(indexFecRing)) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Error during reading the FEC SR1: " << e.what() << std::endl ;
  }

  if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
    std::cerr << "SR0 is not correct: clear only the SR1 of the FEC" << std::endl ;
  }
  else {
    //std::cerr << "Clear errors on CCU SRA and FEC SR1" << std::endl ;
    for (std::list<keyType>::iterator it = ccuAddresses.begin() ; it != ccuAddresses.end() ; it ++) {

      try {
	keyType index = *it ;
	//std::cout << "Clear errors on CCU 0x" << std::hex << index << std::endl ;
	displayCCUSRA(fecAccess->getCcuSRA (index)) ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "Error during reading of CCU SRA: " << e.what() << std::endl ;
      }
    }
  }
  
}


/** This method is able to switch ring A to ring B for the FEC and
 * each CCU
 * <p>command: -autoTrackerRedundancy
 * \param fecAccess - FEC access object
 * \param fecAddress - slot of the FEC.
 * \param ringAddress - slot of the ring.
 * \param channel - DCU channel if it is tested (see mode in ProgramTest)
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \param dcuTest - 1 -> test all DCUs ; 2 -> test all DCU excepts Dummy CCU ; 3 -> no test on the DCUs
 * \warning this command will test several position since the method autoRedundancyRing
 */
std::vector<std::string> autoTrackerRedundancyRing ( FecAccess *fecAccess,
						     tscType8 fecAddress,
						     tscType8 ringAddress,
						     tscType8 dcuChannel,
						     int dcuTest,
						     long loop, unsigned long tms ) {

#define CCUDUMMYTIBTIDADDRESS 0x7F

  keyType indexFecRing = buildFecRingKey(fecAddress,ringAddress) ;

  std::vector<std::string> results;

  unsigned int transactionNumber = 0 ;
  unsigned int totalTransactionNumber = 0 ;

  // Find the FECs
  try {
    // Scan the PCI slot for device driver loaded
    fecAccess->getFecRingDevice (indexFecRing) ;
  }
  catch (FecExceptionHandler &e) { 

    std::cerr << "*********** ERROR ********************************" << std::endl ;
    std::cerr << "Cannot find any FECs on the ring: " << e.what() << std::endl ;
    std::cerr << "**************************************************" << std::endl ;

    return results;
  }
  
  // While the loop is not finished
  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
    
    transactionNumber = 0 ;
    keyType ccuDummyIndex = 0 ;
    
    std::cout << "--------------------------------> Test " << std::dec << loopI << " start at " << time(NULL) << std::endl ;
    
    // Display the time
    struct timeval time1, time2 ;
    struct timezone zone1, zone2 ;
    
    // Find all the CCUs
    std::list<keyType> *ccuList = NULL ;

    try {
      
      unsigned int timeout = 10 ;
      do {
	
        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check the CCUs on the ring
        //
        // -----------------------------------------------------------------------------------------------------------
        // Make a reset
        fecAccess->fecHardReset   (indexFecRing) ;
	fecAccess->fecRingReset   (indexFecRing) ;
	fecAccess->fecRingResetB  (indexFecRing) ;
	fecAccess->fecRingResetFSM(indexFecRing) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
	  
          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;
        }
	
        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
       
        timeout -- ;
	
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) usleep (tms) ;
      }
      while (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing)) && timeout > 0) ;
    
      // get the time
      gettimeofday(&time1, &zone1);
    
      // Scan the ring for each FECs and display the CCUs
      ccuList = fecAccess->getCcuList ( indexFecRing ) ;
      if (ccuList != NULL) 
	clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

      // 1 transaction for each scan ring
      transactionNumber += 1 ;
     
      if (ccuList != NULL) {
       
        // Store all the addresses
        keyType ccuAddresses[127] ;
        unsigned int nbCcu = 0 ;

        // Log Message
        std::cout << "After Reset: CCU found " ;

        for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {

          // 1 transaction for the detection of the CCU type
          transactionNumber += 1 ;
         
          keyType index = *p ;
          ccuAddresses[nbCcu++] = index ;

          // Log Message
          std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
        }

        // Log message
        std::cout << std::endl ;

        // Delete = already store in the table of FEC device class
        delete ccuList ;

        // disable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

        // -----------------------------------------------------------------------------------------------------------
        //
        // Check for the dummy CCU address
        //
        // -----------------------------------------------------------------------------------------------------------
        // Find the dummy CCU
        int errorCCU = 0, dummyCcu = false, dummyCcuTibTid = false ;
	fecAccess->setCR0Receive (indexFecRing, false) ;
        fecAccess->setFecRingCR0 ( ccuAddresses[nbCcu-1], FEC_CR0_SELSERIN ) ;
        fecAccess->setCcuCRC ( ccuAddresses[nbCcu-1], CCU_CRC_SSP) ;
	fecAccess->setCR0Receive (indexFecRing, true) ;
	fecAccess->fecRingRelease ( indexFecRing ) ;
        // 1 transation for the CRC
        transactionNumber += 1 ;

        // Check if a dummy CCU exists
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {

          //std::cout << "SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, ccuAddresses[nbCcu-1] ) ;
        }

	// DUMMY CCU by B -> A (TEC/TOB)
        if (isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {
         
          // Scan the ring for each FECs and display the CCUs
          ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	  if (ccuList != NULL) 
	    clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

          // 1 transation for scanring
          transactionNumber += 1 ;
	  
          if (ccuList != NULL) {
	    
            // Log Message
            std::cout << "Dummy CCU: CCU found " ;
	    
            nbCcu = 0 ;
            for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	      
              // 1 transation for detection of the CCU type
              transactionNumber += 1 ;
	      
              keyType index = *p ;
              ccuAddresses[nbCcu++] = index ;
	      
              // Log Message
              std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
            }
	    
            // Log Message
            std::cout << std::endl ;
	    
            // Log Message
            std::cout << "Dummy CCU address found 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-1])
                 << " (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]) << ")"
                 << std::endl ;
	    
            ccuDummyIndex = ccuAddresses[nbCcu-1] ;
            dummyCcu = true ;
	    
            results.push_back("<CCUDummy = 0x" + IntToString(getCcuKey(ccuDummyIndex)) + ">");
	    
            // Read during a certain amount of time the CCU and the DCU
            transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
	    
            results.push_back("</CCUDummy>");
	    
            // Delete the list of the CCUs
            delete ccuList ;
	  }
	  else {
	    
	    std::cout << "ERROR: no CCU found after the check of the dummy CCU" << std::endl ;
	    errorCCU = 1 ;
	  }
	}
	else { // DUMMY CCU by B -> B (TIB)

	  // -----------------------------------------------------------------------------------------------------------
	  //
	  // Check for the dummy CCU address IN TIB / TID solution with end-cap
	  //
	  // -----------------------------------------------------------------------------------------------------------
	  // Find the dummy CCU
	  // Try in case of TIB/TID end cap solution to find the CCU such as:
	  // LAST CCU A -> FEC INPUT A
	  // LAST CCU B -> Dummy CCU B
	  // Note that the dummy CCUs address must always be 0x7F
	  keyType indexCCUDummyTIBTID = buildCompleteKey(fecAddress,ringAddress,CCUDUMMYTIBTIDADDRESS,0,0) ;
	  fecAccess->setCR0Receive (indexFecRing, false) ;
	  fecAccess->setCcuCRC ( indexCCUDummyTIBTID, CCU_CRC_ALTIN) ;
	  fecAccess->setCR0Receive (indexFecRing, true) ;
	  fecAccess->fecRingRelease ( indexFecRing ) ;
	  //fecAccess->setFecRingCR0 ( ccuAddresses[nbCcu-1], FEC_CR0_SELSERIN ) ;

	  sleep(1) ;

	  // 1 transation for the CRC
	  transactionNumber += 1 ;

	  // Check if a dummy CCU exists
	  if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {
	    
	    //std::cout << "SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	    emptyFIFOs ( fecAccess, ccuAddresses[nbCcu-1] ) ;
	  }

	  if (isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {
         
	    // Scan the ring for each FECs and display the CCUs
	    ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	    if (ccuList != NULL) 
	      clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

	    // 1 transation for scanring
	    transactionNumber += 1 ;
     
	    if (ccuList != NULL) {

	      // Log Message
	      std::cout << "Dummy CCU TIB/TID: CCU found " ;

	      nbCcu = 0 ;
	      for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
		
		// 1 transation for detection of the CCU type
		transactionNumber += 1 ;
		
		keyType index = *p ;
		ccuAddresses[nbCcu++] = index ;
		
		// Log Message
		std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
	      }
	      
	      // Log Message
	      std::cout << std::endl ;
	      
	      // Log Message
	      std::cout << "Dummy CCU TIB/TID address found 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-1])
		   << " (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]) << ")"
		   << std::endl ;

	      dummyCcuTibTid = true ;
	      ccuDummyIndex = ccuAddresses[nbCcu-1] ;
	      dummyCcu = true ;

	      results.push_back("<CCUDummy = 0x" + IntToString(getCcuKey(ccuDummyIndex)) + ">");

	      // Read during a certain amount of time the CCU and the DCU
	      transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
		
	      results.push_back("</CCUDummy>");

	      // Delete the list of the CCUs
	      delete ccuList ;
	    }
	    else {

	      std::cout << "ERROR: no CCU found after the check of the dummy CCU in TIB/TID ring" << std::endl ;
	      errorCCU = 1 ;
	    }
	  }
	  else {
	    // Log Message
	    std::cout << "ERROR: Dummy CCU, cannot find it, SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ;
	    errorCCU = 1 ;         
	  }
	}

        // Number of CCUs without the dummy CCU
        // Check the address
        uint numberCCU = nbCcu ;
        if (dummyCcu) numberCCU -= 1 ;

        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check if the output B of the FEC goes to the input B of the first CCU
        //
        // -----------------------------------------------------------------------------------------------------------
        // Reset the Ring A and B
        fecAccess->fecHardReset ( indexFecRing ) ;
        fecAccess->fecRingReset ( indexFecRing ) ;
        fecAccess->fecRingResetB ( indexFecRing ) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;
        }

        // disable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;

        // Use the ring B for the first CCU
	fecAccess->setCR0Receive (indexFecRing, false) ;
        fecAccess->setFecRingCR0 ( ccuAddresses[0], FEC_CR0_SELSEROUT ) ;
        fecAccess->setCcuCRC ( ccuAddresses[0], CCU_CRC_ALTIN) ;
	fecAccess->setCR0Receive (indexFecRing, true) ;
        fecAccess->fecRingRelease ( indexFecRing ) ;

        // 1 transation for the CRC
        transactionNumber += 1 ; 

        results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[0])) + ">");
       
        // If the status is not correct then
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0])))
          emptyFIFOs ( fecAccess, ccuAddresses[0] ) ;

        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0]))) {

          // Log Message
	  std::cout << "ERROR when I use the FEC out B to 1st CCU input B 0x" << std::hex << getCcuKey(ccuAddresses[0]) << ", SR0 = 0x" <<  fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ;
          errorCCU = 2 ;
	  results.push_back("   <InputB>Nok</InputB>");
        }
        else {

          // Log Message
          std::cout << "Ring B for the 1st CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) << " Ok (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;
         
          // Scan the ring for each FECs and display the CCUs
          ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	  if (ccuList != NULL) 
	    clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

          // 1 transaction for each scan ring
          transactionNumber += 1 ;

          if (ccuList != NULL) {
           
            if (checkRedundancyAddress ( ccuList, numberCCU, ccuAddresses, -1, false)) {
              std::cout << "ERROR: during the bypass of the CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) 
                   << " => the number of CCUs or the CCU addresses does not corresponds to what is expected"
                   << std::endl ;
              results.push_back("   <InputB>Nok</InputB>");
            }
            else {
              results.push_back("   <InputB>Ok</InputB>");

              // Log Message
              std::cout << "Ring B for the 1st CCU: CCU found " ;
             
              for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
               
                // 1 transaction for the detection of the CCU type
                transactionNumber += 1 ;
               
                keyType index = *p ;
               
                // Log Message
                std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
              }
           
              // Log Message
              std::cout << std::endl ;
             
              // Read during a certain amount of time the CCU and the DCU
              transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;

              // Delete = already store in the table of FEC device class
              delete ccuList ;
            }
          }
          else {
            std::cout << "ERROR: no CCU found after the use of ring B for the 1st CCU 0x"  << std::hex << getCcuKey(ccuAddresses[0]) << std::endl ;
            results.push_back("   <InputB>Nok</InputB>");
            errorCCU = 2 ;
          }
        }

        //results.push_back("</CCUAddress>");    

        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check the bypass of the 1st CCU
        //
        // -----------------------------------------------------------------------------------------------------------
        // Reset the Ring A and B
        fecAccess->fecHardReset ( indexFecRing ) ;
        fecAccess->fecRingReset ( indexFecRing ) ;
        fecAccess->fecRingResetB ( indexFecRing ) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;
        }

        // disable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;

        // Bypass the 1st CCU
	fecAccess->setCR0Receive (indexFecRing, false) ;
        fecAccess->setFecRingCR0 ( ccuAddresses[0], FEC_CR0_SELSEROUT ) ;
        fecAccess->setCcuCRC ( ccuAddresses[1], CCU_CRC_ALTIN) ;
	fecAccess->setCR0Receive (indexFecRing, true) ;
        fecAccess->fecRingRelease ( indexFecRing ) ;

        // 1 transation for the CRC
        transactionNumber += 1 ; 

        //results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[0])) + ">");
       
        // If the status is not correct then
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0])))
          emptyFIFOs ( fecAccess, ccuAddresses[0] ) ;

        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0]))) {

          // Log Message
          std::cout << "ERROR when I bypass the 1st CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) << ", SR0 = 0x" <<  fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ;
          errorCCU = 3 ;
          results.push_back("   <Bypass>Nok</Bypass>");
        }
        else {

          // Log Message
          std::cout << "Bypass the 1st CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) << " Ok (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;
         
          // Scan the ring for each FECs and display the CCUs
          ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	  if (ccuList != NULL) 
	    clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

          // 1 transaction for each scan ring
          transactionNumber += 1 ;

          if (ccuList != NULL) {
           
            // Check the address
            if (checkRedundancyAddress ( ccuList, numberCCU, ccuAddresses, 0, true)) {
              std::cout << "ERROR: during the bypass of the CCU 0x" << std::hex << getCcuKey(ccuAddresses[0])
                   << " => the number of CCUs or the CCU addresses does not corresponds to what is expected"
                   << std::endl ;
              results.push_back("   <Bypass>Nok</Bypass>");
            }
            else {
              results.push_back("   <Bypass>Ok</Bypass>");
           

              // Log Message
              std::cout << "Bypass of the 1st CCU: CCU found " ;
             
              for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {

                // 1 transaction for the detection of the CCU type
                transactionNumber += 1 ;
               
                keyType index = *p ;
               
                // Log Message
                std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
              }
             
              // Log Message
              std::cout << std::endl ;

              // Read during a certain amount of time the CCU and the DCU
              transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
	      
              // Delete = already store in the table of FEC device class
              delete ccuList ;
            }
          }
          else {
            std::cout << "ERROR: no CCU found after the bypassed of the 1st CCU 0x"  << std::hex << getCcuKey(ccuAddresses[0]) << std::endl ;
            results.push_back("   <Bypass>Nok</Bypass>");
            errorCCU = 3 ;
          }
        }
        results.push_back("</CCUAddress>");

        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check the bypass of each CCU between the 2nd CCU and the before last one
        //
        // -----------------------------------------------------------------------------------------------------------
        // Reset the Ring A and B
        fecAccess->fecHardReset ( indexFecRing ) ;
        fecAccess->fecRingReset ( indexFecRing ) ;
        fecAccess->fecRingResetB ( indexFecRing ) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;      
        }

        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
       
        // disable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;
       
        // Bypass the other CCU except the last one
        for (uint ccuI = 1 ; ccuI < (numberCCU-1) ; ccuI ++) {

	  fecAccess->setCR0Receive (indexFecRing, false) ;
          fecAccess->setCcuCRC (ccuAddresses[ccuI-1], CCU_CRC_SSP) ;
          fecAccess->setCcuCRC (ccuAddresses[ccuI+1], CCU_CRC_ALTIN) ;
	  fecAccess->setCR0Receive (indexFecRing, true) ;
          fecAccess->fecRingRelease ( indexFecRing ) ;

	  sleep(1) ;

          results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[ccuI])) + ">");

          // 1 transation for the CRC
          transactionNumber += 2 ;
         
          // If the status is not correct then
          if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[ccuI])))
            emptyFIFOs ( fecAccess, ccuAddresses[ccuI] ) ;

          if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[ccuI]))) {
           
            std::cout << "ERROR when I bypass the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI])
                 << ", SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[ccuI]) << std::endl ;
            errorCCU = ccuI + 3 ;
           
            results.push_back("   <Bypass>Nok</Bypass>");
          }
          else {

            std::cout << "Bypass of the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << " Ok (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;

            // Scan the ring for each FECs and display the CCUs
            ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	    if (ccuList != NULL) 
	      clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

            // 1 transaction for each scan ring
            transactionNumber += 1 ;

            if (ccuList != NULL) {

              if (checkRedundancyAddress ( ccuList, numberCCU, ccuAddresses, ccuI, true)) {
                std::cout << "ERROR: during the bypass of the CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI])
                     << " => the number of CCUs or the CCU addresses does not corresponds to what is expected"
                     << std::endl ;
                results.push_back("   <Bypass>Nok</Bypass>");
              }
              else {
                results.push_back("   <Bypass>Ok</Bypass>");
               
                // Log Message
                std::cout << "Bypass of the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << ": CCU found " ;
               
                for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
                 
                  // 1 transaction for the detection of the CCU type
                  transactionNumber += 1 ;  
                 
                  keyType index = *p ;
                 
                  // Log Message
                  std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
                }
               
                // Log Message
                std::cout << std::endl ;
               
                // Read during a certain amount of time the CCU and the DCU
                transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
               
                // Delete = already store in the table of FEC device class
                delete ccuList ;
              }
            }
            else {
              std::cout << "ERROR: no CCU found after the bypassed of the "  << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << std::endl ;
              results.push_back("   <Bypass>Nok</Bypass>");
              errorCCU = ccuI+3 ;

            }
           
            results.push_back("</CCUAddress>");

          }

          // Reset the Ring A and B
          fecAccess->fecHardReset ( indexFecRing ) ;
          fecAccess->fecRingReset ( indexFecRing ) ;
          fecAccess->fecRingResetB ( indexFecRing ) ;
          if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

            std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
            emptyFIFOs ( fecAccess, indexFecRing, true ) ;
          }

          // disable the IRQ
          fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

          // Log message
          std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
        }

        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check the bypass of the last CCU
        //
        // -----------------------------------------------------------------------------------------------------------
        // Bypass the last CCU
        if (dummyCcu) {

	  fecAccess->setCR0Receive (indexFecRing, false) ;
          fecAccess->setFecRingCR0 ( indexFecRing, FEC_CR0_SELSERIN ) ;
          fecAccess->setCcuCRC (ccuAddresses[nbCcu-3], CCU_CRC_SSP) ;
	  if (!dummyCcuTibTid) {
	    fecAccess->setCcuCRC (ccuAddresses[nbCcu-1], CCU_CRC_ALTIN) ;
	  }
	  fecAccess->setCR0Receive (indexFecRing, true) ;
          fecAccess->fecRingRelease ( indexFecRing ) ;

          results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[nbCcu-2])) + ">");

          // 1 transation for the CRC
          transactionNumber += 2 ;

          // If the status is not correct then
          if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-2])))
            emptyFIFOs ( fecAccess, ccuAddresses[nbCcu-2] ) ;

          if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-2]))) {

            std::cout << "ERROR when I bypass the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2])
                 << " with the dummy CCU, SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[nbCcu-2]) << std::endl ;
            errorCCU = numberCCU + 3 ;

            results.push_back("   <Bypass>Nok</Bypass>");
          }
          else {

            std::cout << "Bypass of the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2]) << " Ok with the dummy CCU, (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;

            // Scan the ring for each FECs and display the CCUs
            ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	    if (ccuList != NULL) 
	      clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

            // 1 transaction for each scan ring
            transactionNumber += 1 ;
           
            if (ccuList != NULL) {

              if (checkRedundancyAddress ( ccuList, numberCCU, ccuAddresses, nbCcu-2, false)) {
                std::cout << "ERROR: during the bypass of the CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2])
                     << " => the number of CCUs or the CCU addresses does not corresponds to what is expected"
                     << std::endl ;
                results.push_back("   <Bypass>Nok</Bypass>");
              }
              else {
                results.push_back("   <Bypass>Ok</Bypass>");
             
                // Log Message
                std::cout << "Bypass of the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2]) << " CCU found " ;
               
                for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
                 
                  // 1 transaction for the detection of the CCU type
                  transactionNumber += 1 ;     
                 
                  keyType index = *p ;
                 
                  // Log Message
                  std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
                }
               
                // Log Message
                std::cout << std::endl ;
               
                // Read during a certain amount of time the CCU and the DCU
                transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
               
                // Delete = already store in the table of FEC device class
                delete ccuList ;
              }
            }
            else {
              std::cout << "ERROR: no CCU found after the bypassed of the last CCU 0x"  << std::hex << getCcuKey(ccuAddresses[nbCcu-2]) << " with dummy CCU" << std::endl ;
              errorCCU = numberCCU + 3 ;
              results.push_back("   <Bypass>Nok</Bypass>");
            }
          }
         
          results.push_back("</CCUAddress>");
        }
        else {
          results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[nbCcu-1])) + ">");
          results.push_back("   <Bypass>Nok</Bypass>");
          results.push_back("</CCUAddress>");
        }


        // Display the final message
        if (errorCCU) {
	  if (errorCCU == 1)
	    std::cout << "Redundancy: Error on dummy CCU, cannot find it, Cannot use the redundancy" << std::endl ;
	  else if (errorCCU == 2)
	    std::cout << "Redundancy: error when I use the FEC out B to 1st CCU input B 0x" << std::hex << getCcuKey(ccuAddresses[0]) << ", SR0 = 0x" <<  fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ;
	  else
	    std::cout << "Redundancy: Error when the CCUs are bypassed, Cannot use the redundancy for the CCU 0x" << std::hex << getCcuKey(ccuAddresses[errorCCU-3]) << std::endl ;
        }
        else {
           
          std::cout << "Redundancy: Ring B is ok" << std::endl ;
        }

        // Reset the Ring A and B
        fecAccess->fecHardReset ( indexFecRing ) ;
        fecAccess->fecRingReset ( indexFecRing ) ;
        fecAccess->fecRingResetB ( indexFecRing ) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
	  
          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;
        }

        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
       
        // enable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(true) ;
      }
      else {
       
        // Log Message
        std::cout << "ERROR: No CCU found on the ring, (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
      }
    }
    catch (FecExceptionHandler &e) {
     
      std::cerr << "*********** ERROR ********************************" << std::endl ;
      std::cerr << "An error occurs during hardware access" << std::endl ;
      std::cerr << e.what()  << std::endl ;
      std::cerr << "**************************************************" << std::endl ;
    }

    // Display the time
    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    std::cout << "--------------------------------> Test " << std::dec << loopI << " stop and takes " << std::dec << timesec << " s for "
         << transactionNumber << " transactions (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
    
    totalTransactionNumber += transactionNumber ;
    
    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }
  
  std::cout << "End of the method, the total number of transaction is " << std::dec << totalTransactionNumber << std::endl ;

  return results;
}

/** This method is able to switch ring A to ring B for the FEC and
 * each CCU
 * <p>command: Previous version
 * \param fecAccess - FEC access object
 * \param fecAddress - slot of the FEC.
 * \param ringAddress - slot of the ring.
 * \param channel - DCU channel if it is tested (see mode in ProgramTest)
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \param dcuTest - 1 -> test all DCUs ; 2 -> test all DCU excepts Dummy CCU ; 3 -> no test on the DCUs
 * \warning this command will test several position since the method autoRedundancyRing
 */
std::vector<std::string> autoTrackerRedundancyRingPrevious ( FecAccess *fecAccess,
						   tscType8 fecAddress,
						   tscType8 ringAddress,
						   tscType8 dcuChannel,
						   int dcuTest,
						   long loop, unsigned long tms ) {

#define CCUDUMMYTIBTIDADDRESS 0x7F

  keyType indexFecRing = buildFecRingKey(fecAddress,ringAddress) ;

  std::vector<std::string> results;

  unsigned int transactionNumber = 0 ;
  unsigned int totalTransactionNumber = 0 ;

  // Find the FECs
  try {
    // Scan the PCI slot for device driver loaded
    fecAccess->getFecRingDevice (indexFecRing) ;
  }
  catch (FecExceptionHandler &e) { 

    std::cerr << "*********** ERROR ********************************" << std::endl ;
    std::cerr << "Cannot find any FECs on the ring: " << e.what() << std::endl ;
    std::cerr << "**************************************************" << std::endl ;

    return results;
  }
  
  // While the loop is not finished
  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
    
    transactionNumber = 0 ;
    keyType ccuDummyIndex = 0 ;
    
    std::cout << "--------------------------------> Test " << std::dec << loopI << " start at " << time(NULL) << std::endl ;
    
    // Display the time
    struct timeval time1, time2 ;
    struct timezone zone1, zone2 ;
    
    // Find all the CCUs
    std::list<keyType> *ccuList = NULL ;

    try {
      
      unsigned int timeout = 10 ;
      do {
	
        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check the CCUs on the ring
        //
        // -----------------------------------------------------------------------------------------------------------
        // Make a reset
        fecAccess->fecHardReset (indexFecRing) ;
	fecAccess->fecRingReset (indexFecRing) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
	  
          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;
        }
	
        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
       
        timeout -- ;
	
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) usleep (tms) ;
      }
      while (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing)) && timeout > 0) ;
    
      // get the time
      gettimeofday(&time1, &zone1);
    
      // Scan the ring for each FECs and display the CCUs
      ccuList = fecAccess->getCcuList ( indexFecRing ) ;
      if (ccuList != NULL) 
	clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

      // 1 transaction for each scan ring
      transactionNumber += 1 ;
     
      if (ccuList != NULL) {
       
        // Store all the addresses
        keyType ccuAddresses[127] ;
        unsigned int nbCcu = 0 ;

        // Log Message
        std::cout << "After Reset: CCU found " ;

        for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {

          // 1 transaction for the detection of the CCU type
          transactionNumber += 1 ;
         
          keyType index = *p ;
          ccuAddresses[nbCcu++] = index ;

          // Log Message
          std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
        }

        // Log message
        std::cout << std::endl ;

        // Delete = already store in the table of FEC device class
        delete ccuList ;

        // disable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

        // -----------------------------------------------------------------------------------------------------------
        //
        // Check for the dummy CCU address
        //
        // -----------------------------------------------------------------------------------------------------------
        // Find the dummy CCU
        int errorCCU = 0, dummyCcu = false, dummyCcuTibTid = false ;
        fecAccess->setFecRingCR0 ( ccuAddresses[nbCcu-1], FEC_CR0_SELSERIN ) ;
        fecAccess->setCcuCRC ( ccuAddresses[nbCcu-1], CCU_CRC_SSP) ;
	fecAccess->fecRingRelease ( indexFecRing ) ;

        // 1 transation for the CRC
        transactionNumber += 1 ;

        // Check if a dummy CCU exists
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {

          //std::cout << "SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, ccuAddresses[nbCcu-1] ) ;
        }
        if (isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {
         
          // Scan the ring for each FECs and display the CCUs
          ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	  if (ccuList != NULL) 
	    clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

          // 1 transation for scanring
          transactionNumber += 1 ;
	  
          if (ccuList != NULL) {
	    
            // Log Message
            std::cout << "Dummy CCU: CCU found " ;
	    
            nbCcu = 0 ;
            for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	      
              // 1 transation for detection of the CCU type
              transactionNumber += 1 ;
	      
              keyType index = *p ;
              ccuAddresses[nbCcu++] = index ;
	      
              // Log Message
              std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
            }
	    
            // Log Message
            std::cout << std::endl ;
	    
            // Log Message
            std::cout << "Dummy CCU address found 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-1])
                 << " (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]) << ")"
                 << std::endl ;
	    
            ccuDummyIndex = ccuAddresses[nbCcu-1] ;
            dummyCcu = true ;
	    
            results.push_back("<CCUDummy = 0x" + IntToString(getCcuKey(ccuDummyIndex)) + ">");
	    
            // Read during a certain amount of time the CCU and the DCU
            transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
	    
            results.push_back("</CCUDummy>");
	    
            // Delete the list of the CCUs
            delete ccuList ;
	  }
	  else {
	    
	    std::cout << "ERROR: no CCU found after the check of the dummy CCU" << std::endl ;
	    errorCCU = 1 ;
	  }
	}
	else {

	  // -----------------------------------------------------------------------------------------------------------
	  //
	  // Make a reset and check if the output B of the FEC goes to the input B of the first CCU
	  //
	  // -----------------------------------------------------------------------------------------------------------
	  // Reset the Ring A and B
	  fecAccess->fecHardReset ( indexFecRing ) ;
	  fecAccess->fecRingReset ( indexFecRing ) ;
	  fecAccess->fecRingResetB ( indexFecRing ) ;
	  if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

	    std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	    emptyFIFOs ( fecAccess, indexFecRing, true ) ;
	  }

	  // disable the IRQ
	  fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

	  // Log message
	  std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;

	  // -----------------------------------------------------------------------------------------------------------
	  //
	  // Check for the dummy CCU address IN TIB / TID solution with end-cap
	  //
	  // -----------------------------------------------------------------------------------------------------------
	  // Find the dummy CCU
	  // Try in case of TIB/TID end cap solution to find the CCU such as:
	  // LAST CCU A -> FEC INPUT A
	  // LAST CCU B -> Dummy CCU B
	  // Note that the dummy CCUs address must always be 0x7E
	  keyType indexCCUDummyTIBTID = buildCompleteKey(fecAddress,ringAddress,CCUDUMMYTIBTIDADDRESS,0,0) ;
	  fecAccess->setCcuCRC ( ccuAddresses[nbCcu-1], CCU_CRC_SSP) ;
	  fecAccess->setCcuCRC ( indexCCUDummyTIBTID, CCU_CRC_ALTIN) ;
	  fecAccess->fecRingRelease ( indexFecRing ) ;
	  fecAccess->setFecRingCR0 ( ccuAddresses[nbCcu-1], FEC_CR0_SELSERIN ) ;

	  // 1 transation for the CRC
	  transactionNumber += 1 ;

	  // Check if a dummy CCU exists
	  if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {

	    //std::cout << "SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	    emptyFIFOs ( fecAccess, ccuAddresses[nbCcu-1] ) ;
	  }
	  if (isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {
         
	    // Scan the ring for each FECs and display the CCUs
	    ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	    if (ccuList != NULL) 
	      clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

	    // 1 transation for scanring
	    transactionNumber += 1 ;
     
	    if (ccuList != NULL) {

	      // Log Message
	      std::cout << "Dummy CCU TIB/TID: CCU found " ;

	      nbCcu = 0 ;
	      for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
		
		// 1 transation for detection of the CCU type
		transactionNumber += 1 ;
		
		keyType index = *p ;
		ccuAddresses[nbCcu++] = index ;
		
		// Log Message
		std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
	      }
	      
	      // Log Message
	      std::cout << std::endl ;
	      
	      // Log Message
	      std::cout << "Dummy CCU TIB/TID address found 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-1])
		   << " (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]) << ")"
		   << std::endl ;

	      ccuDummyIndex = ccuAddresses[nbCcu-1] ;
	      dummyCcu = true ;

	      results.push_back("<CCUDummy = 0x" + IntToString(getCcuKey(ccuDummyIndex)) + ">");

	      // Read during a certain amount of time the CCU and the DCU
	      transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
		
	      results.push_back("</CCUDummy>");

	      // Delete the list of the CCUs
	      delete ccuList ;
	    }
	    else {

	      std::cout << "ERROR: no CCU found after the check of the dummy CCU in TIB/TID ring" << std::endl ;
	      errorCCU = 1 ;
	    }
	  }
	  else {
	    // Log Message
	    std::cout << "ERROR: Dummy CCU, cannot find it, SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ;
	    errorCCU = 1 ;         
	  }
	}

        // Number of CCUs without the dummy CCU
        // Check the address
        uint numberCCU = nbCcu ;
        if (dummyCcu) numberCCU -= 1 ;

        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check if the output B of the FEC goes to the input B of the first CCU
        //
        // -----------------------------------------------------------------------------------------------------------
        // Reset the Ring A and B
        fecAccess->fecHardReset ( indexFecRing ) ;
        fecAccess->fecRingReset ( indexFecRing ) ;
        fecAccess->fecRingResetB ( indexFecRing ) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;
        }

        // disable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;

        // Use the ring B for the first CCU
        fecAccess->setFecRingCR0 ( ccuAddresses[0], FEC_CR0_SELSEROUT ) ;
        fecAccess->setCcuCRC ( ccuAddresses[0], CCU_CRC_ALTIN) ;
        fecAccess->fecRingRelease ( indexFecRing ) ;

        // 1 transation for the CRC
        transactionNumber += 1 ; 

        results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[0])) + ">");
       
        // If the status is not correct then
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0])))
          emptyFIFOs ( fecAccess, ccuAddresses[0] ) ;

        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0]))) {

          // Log Message
	  std::cout << "ERROR when I use the FEC out B to 1st CCU input B 0x" << std::hex << getCcuKey(ccuAddresses[0]) << ", SR0 = 0x" <<  fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ;
          errorCCU = 2 ;
	  results.push_back("   <InputB>Nok</InputB>");
        }
        else {

          // Log Message
          std::cout << "Ring B for the 1st CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) << " Ok (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;
         
          // Scan the ring for each FECs and display the CCUs
          ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	  if (ccuList != NULL) 
	    clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

          // 1 transaction for each scan ring
          transactionNumber += 1 ;

          if (ccuList != NULL) {
           
            if (checkRedundancyAddress ( ccuList, numberCCU, ccuAddresses, -1, false)) {
              std::cout << "ERROR: during the bypass of the CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) 
                   << " => the number of CCUs or the CCU addresses does not corresponds to what is expected"
                   << std::endl ;
              results.push_back("   <InputB>Nok</InputB>");
            }
            else {
              results.push_back("   <InputB>Ok</InputB>");

              // Log Message
              std::cout << "Ring B for the 1st CCU: CCU found " ;
             
              for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
               
                // 1 transaction for the detection of the CCU type
                transactionNumber += 1 ;
               
                keyType index = *p ;
               
                // Log Message
                std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
              }
           
              // Log Message
              std::cout << std::endl ;
             
              // Read during a certain amount of time the CCU and the DCU
              transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;

              // Delete = already store in the table of FEC device class
              delete ccuList ;
            }
          }
          else {
            std::cout << "ERROR: no CCU found after the use of ring B for the 1st CCU 0x"  << std::hex << getCcuKey(ccuAddresses[0]) << std::endl ;
            results.push_back("   <InputB>Nok</InputB>");
            errorCCU = 2 ;
          }
        }

        //results.push_back("</CCUAddress>");    

        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check the bypass of the 1st CCU
        //
        // -----------------------------------------------------------------------------------------------------------
        // Reset the Ring A and B
        fecAccess->fecHardReset ( indexFecRing ) ;
        fecAccess->fecRingReset ( indexFecRing ) ;
        fecAccess->fecRingResetB ( indexFecRing ) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;
        }

        // disable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;

        // Bypass the 1st CCU
        fecAccess->setFecRingCR0 ( ccuAddresses[0], FEC_CR0_SELSEROUT ) ;
        fecAccess->setCcuCRC ( ccuAddresses[1], CCU_CRC_ALTIN) ;
        fecAccess->fecRingRelease ( indexFecRing ) ;

        // 1 transation for the CRC
        transactionNumber += 1 ; 

        //results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[0])) + ">");
       
        // If the status is not correct then
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0])))
          emptyFIFOs ( fecAccess, ccuAddresses[0] ) ;

        if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0]))) {

          // Log Message
          std::cout << "ERROR when I bypass the 1st CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) << ", SR0 = 0x" <<  fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ;
          errorCCU = 3 ;
          results.push_back("   <Bypass>Nok</Bypass>");
        }
        else {

          // Log Message
          std::cout << "Bypass the 1st CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) << " Ok (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;
         
          // Scan the ring for each FECs and display the CCUs
          ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	  if (ccuList != NULL) 
	    clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

          // 1 transaction for each scan ring
          transactionNumber += 1 ;

          if (ccuList != NULL) {
           
            // Check the address
            if (checkRedundancyAddress ( ccuList, numberCCU, ccuAddresses, 0, true)) {
              std::cout << "ERROR: during the bypass of the CCU 0x" << std::hex << getCcuKey(ccuAddresses[0])
                   << " => the number of CCUs or the CCU addresses does not corresponds to what is expected"
                   << std::endl ;
              results.push_back("   <Bypass>Nok</Bypass>");
            }
            else {
              results.push_back("   <Bypass>Ok</Bypass>");
           

              // Log Message
              std::cout << "Bypass of the 1st CCU: CCU found " ;
             
              for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {

                // 1 transaction for the detection of the CCU type
                transactionNumber += 1 ;
               
                keyType index = *p ;
               
                // Log Message
                std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
              }
             
              // Log Message
              std::cout << std::endl ;

              // Read during a certain amount of time the CCU and the DCU
              transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
	      
              // Delete = already store in the table of FEC device class
              delete ccuList ;
            }
          }
          else {
            std::cout << "ERROR: no CCU found after the bypassed of the 1st CCU 0x"  << std::hex << getCcuKey(ccuAddresses[0]) << std::endl ;
            results.push_back("   <Bypass>Nok</Bypass>");
            errorCCU = 3 ;
          }
        }
        results.push_back("</CCUAddress>");

        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check the bypass of each CCU between the 2nd CCU and the before last one
        //
        // -----------------------------------------------------------------------------------------------------------
        // Reset the Ring A and B
        fecAccess->fecHardReset ( indexFecRing ) ;
        fecAccess->fecRingReset ( indexFecRing ) ;
        fecAccess->fecRingResetB ( indexFecRing ) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;      
        }

        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
       
        // disable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;
       
        // Bypass the other CCU except the last one
        for (uint ccuI = 1 ; ccuI < (numberCCU-1) ; ccuI ++) {

          fecAccess->setCcuCRC (ccuAddresses[ccuI-1], CCU_CRC_SSP) ;
          fecAccess->setCcuCRC (ccuAddresses[ccuI+1], CCU_CRC_ALTIN) ;
          fecAccess->fecRingRelease ( indexFecRing ) ;

          results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[ccuI])) + ">");

          // 1 transation for the CRC
          transactionNumber += 2 ;
         
          // If the status is not correct then
          if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[ccuI])))
            emptyFIFOs ( fecAccess, ccuAddresses[ccuI] ) ;

          if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[ccuI]))) {
           
            std::cout << "ERROR when I bypass the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI])
                 << ", SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[ccuI]) << std::endl ;
            errorCCU = ccuI + 3 ;
           
            results.push_back("   <Bypass>Nok</Bypass>");
          }
          else {

            std::cout << "Bypass of the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << " Ok (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;

            // Scan the ring for each FECs and display the CCUs
            ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	    if (ccuList != NULL) 
	      clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

            // 1 transaction for each scan ring
            transactionNumber += 1 ;

            if (ccuList != NULL) {

              if (checkRedundancyAddress ( ccuList, numberCCU, ccuAddresses, ccuI, true)) {
                std::cout << "ERROR: during the bypass of the CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI])
                     << " => the number of CCUs or the CCU addresses does not corresponds to what is expected"
                     << std::endl ;
                results.push_back("   <Bypass>Nok</Bypass>");
              }
              else {
                results.push_back("   <Bypass>Ok</Bypass>");
               
                // Log Message
                std::cout << "Bypass of the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << ": CCU found " ;
               
                for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
                 
                  // 1 transaction for the detection of the CCU type
                  transactionNumber += 1 ;  
                 
                  keyType index = *p ;
                 
                  // Log Message
                  std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
                }
               
                // Log Message
                std::cout << std::endl ;
               
                // Read during a certain amount of time the CCU and the DCU
                transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
               
                // Delete = already store in the table of FEC device class
                delete ccuList ;
              }
            }
            else {
              std::cout << "ERROR: no CCU found after the bypassed of the "  << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << std::endl ;
              results.push_back("   <Bypass>Nok</Bypass>");
              errorCCU = ccuI+3 ;

            }
           
            results.push_back("</CCUAddress>");

          }

          // Reset the Ring A and B
          fecAccess->fecHardReset ( indexFecRing ) ;
          fecAccess->fecRingReset ( indexFecRing ) ;
          fecAccess->fecRingResetB ( indexFecRing ) ;
          if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

            std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
            emptyFIFOs ( fecAccess, indexFecRing, true ) ;
          }

          // disable the IRQ
          fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

          // Log message
          std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
        }

        // -----------------------------------------------------------------------------------------------------------
        //
        // Make a reset and check the bypass of the last CCU
        //
        // -----------------------------------------------------------------------------------------------------------
        // Bypass the last CCU
        if (dummyCcu) {

          fecAccess->setCcuCRC (ccuAddresses[nbCcu-3], CCU_CRC_SSP) ;

	  if (!dummyCcuTibTid)
	    fecAccess->setCcuCRC (ccuAddresses[nbCcu-1], CCU_CRC_ALTIN) ;

          fecAccess->fecRingRelease ( indexFecRing ) ;
          fecAccess->setFecRingCR0 ( indexFecRing, FEC_CR0_SELSERIN ) ;

          results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[nbCcu-2])) + ">");

          // 1 transation for the CRC
          transactionNumber += 2 ;

          // If the status is not correct then
          if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-2])))
            emptyFIFOs ( fecAccess, ccuAddresses[nbCcu-2] ) ;

          if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-2]))) {

            std::cout << "ERROR when I bypass the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2])
                 << " with the dummy CCU, SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[nbCcu-2]) << std::endl ;
            errorCCU = numberCCU + 3 ;

            results.push_back("   <Bypass>Nok</Bypass>");
          }
          else {

            std::cout << "Bypass of the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2]) << " Ok with the dummy CCU, (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;

            // Scan the ring for each FECs and display the CCUs
            ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	    if (ccuList != NULL) 
	      clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

            // 1 transaction for each scan ring
            transactionNumber += 1 ;
           
            if (ccuList != NULL) {

              if (checkRedundancyAddress ( ccuList, numberCCU, ccuAddresses, nbCcu-2, false)) {
                std::cout << "ERROR: during the bypass of the CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2])
                     << " => the number of CCUs or the CCU addresses does not corresponds to what is expected"
                     << std::endl ;
                results.push_back("   <Bypass>Nok</Bypass>");
              }
              else {
                results.push_back("   <Bypass>Ok</Bypass>");
             
                // Log Message
                std::cout << "Bypass of the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2]) << " CCU found " ;
               
                for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
                 
                  // 1 transaction for the detection of the CCU type
                  transactionNumber += 1 ;     
                 
                  keyType index = *p ;
                 
                  // Log Message
                  std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
                }
               
                // Log Message
                std::cout << std::endl ;
               
                // Read during a certain amount of time the CCU and the DCU
                transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
               
                // Delete = already store in the table of FEC device class
                delete ccuList ;
              }
            }
            else {
              std::cout << "ERROR: no CCU found after the bypassed of the last CCU 0x"  << std::hex << getCcuKey(ccuAddresses[nbCcu-2]) << " with dummy CCU" << std::endl ;
              errorCCU = numberCCU + 3 ;
              results.push_back("   <Bypass>Nok</Bypass>");
            }
          }
         
          results.push_back("</CCUAddress>");
        }
        else {
          results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[nbCcu-1])) + ">");
          results.push_back("   <Bypass>Nok</Bypass>");
          results.push_back("</CCUAddress>");
        }


        // Display the final message
        if (errorCCU) {
	  if (errorCCU == 1)
	    std::cout << "Redundancy: Error on ummy CCU, cannot find it, Cannot use the redundancy" << std::endl ;
	  else if (errorCCU == 2)
	    std::cout << "Redundancy: error when I use the FEC out B to 1st CCU input B 0x" << std::hex << getCcuKey(ccuAddresses[0]) << ", SR0 = 0x" <<  fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ;
	  else
	    std::cout << "Redundancy: Error when the CCUs are bypassed, Cannot use the redundancy for the CCU 0x" << std::hex << getCcuKey(ccuAddresses[errorCCU-3]) << std::endl ;
        }
        else {
           
          std::cout << "Redundancy: Ring B is ok" << std::endl ;
        }

        // Reset the Ring A and B
        fecAccess->fecHardReset ( indexFecRing ) ;
        fecAccess->fecRingReset ( indexFecRing ) ;
        fecAccess->fecRingResetB ( indexFecRing ) ;
        if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
	  
          std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
          emptyFIFOs ( fecAccess, indexFecRing, true ) ;
        }

        // Log message
        std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
       
        // enable the IRQ
        fecAccess->getFecRingDevice (indexFecRing)->setIRQ(true) ;
      }
      else {
       
        // Log Message
        std::cout << "ERROR: No CCU found on the ring, (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
      }
    }
    catch (FecExceptionHandler &e) {
     
      std::cerr << "*********** ERROR ********************************" << std::endl ;
      std::cerr << "An error occurs during hardware access" << std::endl ;
      std::cerr << e.what()  << std::endl ;
      std::cerr << "**************************************************" << std::endl ;
    }

    // Display the time
    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    std::cout << "--------------------------------> Test " << std::dec << loopI << " stop and takes " << std::dec << timesec << " s for "
         << transactionNumber << " transactions (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
    
    totalTransactionNumber += transactionNumber ;
    
    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }
  
  std::cout << "End of the method, the total number of transaction is " << std::dec << totalTransactionNumber << std::endl ;

  return results;
}

/** This method is able to switch ring A to ring B for the FEC and 
 * each CCU
 * <p>command: -autoredundancy
 * \param fecAccess - FEC access object
 * \param fecAddress - slot of the FEC.
 * \param ringAddress - slot of the ring.
 * \param dcuChannel - channel of the DCU (not used in this method)
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \param This method, only check if the CCU bypass is correct without any CCU on ring
 */
std::vector<std::string> autoRedundancyRing ( FecAccess *fecAccess,
				    tscType8 fecAddress,
				    tscType8 ringAddress,
				    tscType8 dcuChannel,
				    int dcuTest,
				    long loop, unsigned long tms ) { 

  keyType indexFecRing = buildFecRingKey(fecAddress,ringAddress) ;

  std::vector<std::string> results;

  unsigned int transactionNumber = 0 ;
  unsigned int totalTransactionNumber = 0 ;
  bool error = false ;

  // Find the FECs
  try {
    // Scan the PCI slot for device driver loaded
    fecAccess->getFecRingDevice (indexFecRing) ;
  }
  catch (FecExceptionHandler &e) {  

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "Cannot find any FECs on the ring: " << e.what() << std::endl ; 
    std::cerr << "**************************************************" << std::endl ;

    return results;
  }

  // While the loop is not finished
  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    transactionNumber = 0 ;

    std::cout << "--------------------------------> Test " << std::dec << loopI << " start at " << time(NULL) << std::endl ;

    // Display the time
    struct timeval time1, time2 ;
    struct timezone zone1, zone2 ;

    // Find all the CCUs
    std::list<keyType> *ccuList = NULL ;

    try {
      
      unsigned int timeout = 10 ;
      do {

	// -----------------------------------------------------------------------------------------------------------
	//
	// Make a reset and check the CCUs on the ring
	//
	// -----------------------------------------------------------------------------------------------------------
	// Make a reset
	//fecAccess->fecHardReset (indexFecRing) ;
	fecAccess->fecRingReset (indexFecRing) ;
	if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
	  
	  std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	  emptyFIFOs ( fecAccess, indexFecRing, true ) ;
	}
	
	// Log message
	std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
	
	timeout -- ;
	
	if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) usleep (tms) ;
      }
      while (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing)) && timeout > 0) ;
      
      // get the time
      gettimeofday(&time1, &zone1);
      

      // Scan the ring for each FECs and display the CCUs
      ccuList = fecAccess->getCcuList ( indexFecRing ) ;
      if (ccuList != NULL) 
	clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

      // 1 transaction for each scan ring
      transactionNumber += 1 ;
      
      if (ccuList != NULL) {
	
	// Store all the addresses
	keyType ccuAddresses[127] ;
	unsigned int nbCcu = 0 ;

	// Log Message
	std::cout << "After Reset: CCU found " ;

	for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {

	  // 1 transaction for the detection of the CCU type
	  transactionNumber += 1 ;
	  
	  keyType index = *p ;
	  ccuAddresses[nbCcu++] = index ;

	  // Log Message
	  std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
	}

	// Log message
	std::cout << std::endl ;

	// Delete = already store in the table of FEC device class
	delete ccuList ;

	// disable the IRQ
	fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

	// -----------------------------------------------------------------------------------------------------------
	//
	// Make a reset and check the bypass of the 1st CCU
	//
	// -----------------------------------------------------------------------------------------------------------
	// Reset the Ring A and B
	//fecAccess->fecHardReset ( indexFecRing ) ;
	fecAccess->fecRingReset ( indexFecRing ) ;
	fecAccess->fecRingResetB ( indexFecRing ) ;
	if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

	  std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	  emptyFIFOs ( fecAccess, indexFecRing, true ) ;
	}

	// disable the IRQ
	fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

	// Log message
	std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;

	// Bypass the 1st CCU
	fecAccess->setFecRingCR0 ( ccuAddresses[0], FEC_CR0_SELSEROUT ) ;
	fecAccess->setCcuCRC ( ccuAddresses[1], CCU_CRC_ALTIN) ;
	fecAccess->fecRingRelease ( indexFecRing ) ;

	// 1 transation for the CRC
	transactionNumber += 1 ;  

	results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[0])) + ">");
	
	// If the status is not correct then
	if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0])))
	  emptyFIFOs ( fecAccess, ccuAddresses[0] ) ;

	if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[0]))) { 

	  // Log Message
	  std::cout << "ERROR when I bypass the 1st CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) << ", SR0 = 0x" <<  fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl ; 
	  error = true ;
	  results.push_back("   <Bypass>Nok</Bypass>");
	}
	else {

	  results.push_back("   <Bypass>Ok</Bypass>");

	  // Log Message
	  std::cout << "Bypass the 1st CCU 0x" << std::hex << getCcuKey(ccuAddresses[0]) << " Ok (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;
	  
	  // Scan the ring for each FECs and display the CCUs
	  ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	  if (ccuList != NULL) 
	    clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

	  // 1 transaction for each scan ring
	  transactionNumber += 1 ;

	  if (ccuList != NULL) {
	    
	    // Log Message
	    std::cout << "Bypass of the 1st CCU: CCU found " ;

	    for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	      
	      // 1 transaction for the detection of the CCU type
	      transactionNumber += 1 ;

	      keyType index = *p ;
	      
	      // Log Message
	      std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
	    }
	    
	    // Log Message
	    std::cout << std::endl ;

	    // Read during a certain amount of time the CCU and the DCU
	    // transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;

	    // Delete = already store in the table of FEC device class
	    delete ccuList ;
	  }
	  else {
	    std::cout << "ERROR: no CCU found after the bypassed of the 1st CCU 0x"  << std::hex << getCcuKey(ccuAddresses[0]) << std::endl ;
	  }
	}

	results.push_back("</CCUAddress>");

	// -----------------------------------------------------------------------------------------------------------
	//
	// Make a reset and check the bypass of each CCU between the 2nd CCU and the before last one
	//
	// -----------------------------------------------------------------------------------------------------------
	// Reset the Ring A and B
	//fecAccess->fecHardReset ( indexFecRing ) ;
	fecAccess->fecRingReset ( indexFecRing ) ;
	fecAccess->fecRingResetB ( indexFecRing ) ;
	if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

	  std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	  emptyFIFOs ( fecAccess, indexFecRing, true ) ;	
	}

	// Log message
	std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
	
	// disable the IRQ
	fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;
	
	// Bypass the other CCU except the last one
	uint numberCCU = nbCcu - 1 ;

	for (uint ccuI = 1 ; ccuI < numberCCU ; ccuI ++) {

	  fecAccess->setCcuCRC (ccuAddresses[ccuI-1], CCU_CRC_SSP) ;
	  fecAccess->setCcuCRC (ccuAddresses[ccuI+1], CCU_CRC_ALTIN) ;
	  fecAccess->fecRingRelease ( indexFecRing ) ;

	  results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[ccuI])) + ">");

	  // 1 transation for the CRC
	  transactionNumber += 2 ;
	  
	  // If the status is not correct then
	  if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[ccuI])))
	    emptyFIFOs ( fecAccess, ccuAddresses[ccuI] ) ;

	  if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[ccuI]))) { 
	    
	    std::cout << "ERROR when I bypass the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI])
		 << ", SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[ccuI]) << std::endl ;
	    error = true ;
	    
	    results.push_back("   <Bypass>Nok</Bypass>");
	  }
	  else {

	    results.push_back("   <Bypass>Ok</Bypass>");
	    
	    std::cout << "Bypass of the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << " Ok (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;

	    // Scan the ring for each FECs and display the CCUs
	    ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	    if (ccuList != NULL) 
	      clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

            // 1 transaction for each scan ring
	    transactionNumber += 1 ;

	    if (ccuList != NULL) {

	      // Log Message
	      std::cout << "Bypass of the " << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << ": CCU found " ;

	      for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {

		// 1 transaction for the detection of the CCU type
		transactionNumber += 1 ;   

		keyType index = *p ;
		
		// Log Message
		std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
	      }

	      // Log Message
	      std::cout << std::endl ;

	      // Read during a certain amount of time the CCU and the DCU
	      // transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;

	      // Delete = already store in the table of FEC device class
	      delete ccuList ;
	    }
	    else {
	      std::cout << "ERROR: no CCU found after the bypassed of the "  << std::dec << ccuI+1 << "th CCU 0x" << std::hex << getCcuKey(ccuAddresses[ccuI]) << std::endl ;
	    }
	    
	    results.push_back("</CCUAddress>");

	  }

	  // Reset the Ring A and B
	  //fecAccess->fecHardReset ( indexFecRing ) ;
	  fecAccess->fecRingReset ( indexFecRing ) ;
	  fecAccess->fecRingResetB ( indexFecRing ) ;
	  if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

	    std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	    emptyFIFOs ( fecAccess, indexFecRing, true ) ;
	  }

	  // disable the IRQ
	  fecAccess->getFecRingDevice (indexFecRing)->setIRQ(false) ;

	  // Log message
	  std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
	}

	// -----------------------------------------------------------------------------------------------------------
	//
	// Make a reset and check the bypass of each the last CCU
	//
	// -----------------------------------------------------------------------------------------------------------
	// Bypass the last CCU
	results.push_back("<CCUAddress = 0x" + IntToString(getCcuKey(ccuAddresses[nbCcu-1])) + ">");

	// Bypass the last CCU
	fecAccess->setCcuCRC (ccuAddresses[nbCcu-2], CCU_CRC_SSP) ;
	fecAccess->setFecRingCR0 ( indexFecRing, FEC_CR0_SELSERIN ) ;

	// 1 transation for the CRC
	transactionNumber += 1 ;

	// If the status is not correct then
	if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1])))
	  emptyFIFOs ( fecAccess, ccuAddresses[nbCcu-1] ) ;

	if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) { 
	  
	  std::cout << "ERROR when I bypass the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-1]) 
	       << " without the dummy CCU, SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[nbCcu-2]) << std::endl ;
	  error = true ;

	  results.push_back("   <Bypass>Nok</Bypass>");
	  
	}
	else {

	  results.push_back("   <Bypass>Ok</Bypass>");

	  std::cout << "Bypass of the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-1]) << " Ok without the dummy CCU, (SR0 = 0x" << fecAccess->getFecRingSR0(ccuAddresses[0]) << ")" << std::endl ;

	  // Scan the ring for each FECs and display the CCUs
	  ccuList = fecAccess->getCcuList ( indexFecRing ) ;
	  if (ccuList != NULL) 
	    clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

	  // 1 transaction for each scan ring
	  transactionNumber += 1 ;
	    
	  if (ccuList != NULL) {
	      
	    // Log Message
	    std::cout << "Bypass of the last CCU 0x" << std::hex << getCcuKey(ccuAddresses[nbCcu-2]) << " CCU found " ;

	    for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	      
	      // 1 transaction for the detection of the CCU type
	      transactionNumber += 1 ;

	      keyType index = *p ;
	      
	      // Log Message
	      std::cout << std::hex << "0x" << getCcuKey(index) << " " ;
	    }
	    
	    // Log Message
	    std::cout << std::endl ;

	    // Read during a certain amount of time the CCU and the DCU
	    // transactionNumber += readDcuCcuMinutes (fecAccess, fecAddress, ringAddress, ccuList, dcuChannel, ccuDummyIndex, results, dcuTest) ;
	    
	    // Delete = already store in the table of FEC device class
	    delete ccuList ;
	  }
	  else {
	    std::cout << "ERROR: no CCU found after the bypassed of the last CCU 0x"  << std::hex << getCcuKey(ccuAddresses[nbCcu-1]) << " without dummy CCU" << std::endl ;
	  }
	}
	
	results.push_back("</CCUAddress>");

	// Display the final message
	if (error) {
	  
	  std::cout << "Redundancy: Error when the CCUs are bypassed, Cannot use the redundancy" << std::endl ;
	}
	else {

	  std::cout << "Redundancy: Ring B is ok" << std::endl ;
	}

	// Reset the Ring A and B
	//fecAccess->fecHardReset ( indexFecRing ) ;
	fecAccess->fecRingReset ( indexFecRing ) ;
	fecAccess->fecRingResetB ( indexFecRing ) ;
	if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

	  std::cout << "PARASITIC: After reset => SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << std::endl ;
	  emptyFIFOs ( fecAccess, indexFecRing, true ) ;
	}

	// Log message
	std::cout << "PLX and FEC reset Performed (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
	
	// enable the IRQ
	fecAccess->getFecRingDevice (indexFecRing)->setIRQ(true) ;
      }
      else {

	// Log Message
	std::cout << "ERROR: No CCU found on the ring, (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;
      }
    }
    catch (FecExceptionHandler &e) { 
      
      std::cerr << "*********** ERROR ********************************" << std::endl ; 
      std::cerr << "An error occurs during hardware access" << std::endl ;
      std::cerr << e.what()  << std::endl ;
      std::cerr << "**************************************************" << std::endl ;
    }

    // Display the time
    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
    std::cout << "--------------------------------> Test " << std::dec << loopI << " stop and takes " << std::dec << timesec << " s for "
	 << transactionNumber << " transactions (SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(indexFecRing) << ")" << std::endl ;

    totalTransactionNumber += transactionNumber ;

    // Wait
    if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
  }

  std::cout << "End of the methods, the total number of transaction is " << std::dec << totalTransactionNumber << std::endl ;

  return results;
}


/**
 * <p>command: -bypassed <CCU address>
 * \param fecAccess - FEC access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 * \param ccuAddresses - array of CCU addresses (or FEC)
 * \warning the loop is not used
 */
void ccuBypassed ( FecAccess *fecAccess, 
                   tscType8 fecAddress,
                   tscType8 ringAddress,
                   uint ccuAddress ) {

  std::list<keyType> *ccuListRing = NULL ;
  std::list<keyType> *ccuList     = NULL ;
  try {

    FecRingDevice *fecDevice = NULL ;
    // Find the FECs
    try {
      // Scan the PCI slot for device driver loaded
      fecDevice = fecAccess->getFecRingDevice ( buildFecRingKey(fecAddress,ringAddress) ) ;
    }
    catch (FecExceptionHandler &e) {  

      std::cerr << "*********** ERROR ********************************" << std::endl ; 
      std::cerr << "Cannot find any FECs on the ring: " << e.what() << std::endl ; 
      std::cerr << "**************************************************" << std::endl ;

      return ;
    }

    keyType index = buildFecRingKey(fecAddress,ringAddress) ;
        
    std::cout << "FEC " << std::dec << getFecKey(index) << "." << getRingKey(index) << std::endl ;

    // Start the scanning of the ring for each FEC device
    // Retreive for the concerning FEC the CCU on the ring
    ccuList = fecAccess->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
    if (ccuList != NULL) 
      clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;

    if (ccuList != NULL) {

      // For each ring
      for (tscType16 ring=0 ; ring < MAXRING ; ring ++) {
	
	std::cout << " \tRing " << ring ;

	// For each CCU
	std::cout << ": CCU: " ;
	for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {
	  keyType index = *p ;
	    
	  if (getRingKey(index) == ring) std::cout << "0x" << std::hex << (int)getCcuKey(index) << std::endl ;

	  if ( (getFecKey(index) == fecAddress) && (ring == ringAddress) ) {
	    if (ccuListRing == NULL) ccuListRing = new std::list<keyType> ;
	    ccuListRing->push_back (buildCompleteKey (fecAddress, ring, getCcuKey(index), 0, 0)) ;
	  }
	}
	std::cout << std::endl ;
      }
    }
    else {
      std::cout << "\tNo CCU found on FEC " << (int)fecDevice->getFecSlot() << std::endl ;
    }

    // Message
    if (! noGetchar_) {
      std::cout << "Press <Enter> to continue" ;
      getchar() ;
    }

    if (ccuListRing != NULL) {

      // -----------------------------------------------------------------
      std::cout << "Status of the FEC " << (int)fecAddress << " and the ring " << (int)ringAddress << std::endl ;
      std::cout << "Ring(" << (int)fecAddress << "," << (int)ringAddress << std::endl ;
      keyType indexPCCU = 0 ;
      keyType indexACCU = 0 ;
      bool next = false, previous = false, ok = false ;
      for (std::list<keyType>::iterator p=ccuListRing->begin() ; p!=ccuListRing->end() ; p++) {

	if (next) {
	  indexACCU = *p ;
	  next = false ;
	}

	keyType index = *p ;
	
	std::cout << "-> 0x" << std::hex << getCcuKey(index) << std::endl ;
	
	if (getCcuKey(index) == ccuAddress) {
	  next = true ;
	  previous = true ;
	  ok = true ;
	}
	
	if (!previous) indexPCCU = *p ;
      }

      std::cout << "-> Ring(" << (int)fecAddress << "," << (int)ringAddress << std::endl ;

      tscType32 fecCR0 = fecDevice->getFecRingCR0() ;
      std::cout << " FEC " << (fecCR0 & 0x10 ? 'B' : 'A') << " " << (fecCR0 & 0x8 ? 'B' : 'A') << std::endl ;
      tscType32 ccuCRC = 0 ;
      for (std::list<keyType>::iterator p=ccuListRing->begin() ; p!=ccuListRing->end() ; p++) {
	keyType index = *p ;
	
	tscType32 CRC = fecDevice->getCcuCRC(index) ;
	std::cout << "0x" << std::hex << (int)getCcuKey(index) << " " << (CRC & 0x1 ? 'B' : 'A') << " " << (CRC & 0x2 ? 'B' : 'A') << std::endl ;
	  
	if ((getCcuKey(index) == ccuAddress) && (getRingKey(index) == ringAddress)) ccuCRC = CRC ;
      }

      // Message
      if (! noGetchar_) {
	std::cout << "Press <Enter> to continue" ;
	getchar() ;
      }
      
      // -----------------------------------------------------------------
      if (ok && ccuAddress) {
std::cout << "Start the bypassed of the CCU 0x" << std::hex << ccuAddress << std::endl ;
	
	// Not the last CCU
	if (indexACCU!= 0) {
	  
	  try {
	    // Set the output to B for the previous CCU
	    if (indexPCCU == 0) { // FEC
	      fecDevice->setFecRingCR0 (fecCR0 | 0x8) ;
	    }
	    else // CCU
	      fecDevice->setCcuCRC (indexPCCU, ccuCRC | 0x2) ;
	  }
	  catch (FecExceptionHandler &e) { // Normal error => ring is broken
	  }
	  
	  fecDevice->fecRingRelease() ;
	  
	  try {
	    // Set the input to B for the next CCU
	    if (indexACCU == 0) { // FEC 
	      fecDevice->setFecRingCR0 (fecCR0 | 0x10) ;
	    }
	    else // CCU
	      fecDevice->setCcuCRC (indexACCU, ccuCRC | 0x1) ;
	  }
	  catch (FecExceptionHandler &e) { // Normal error => ring is broken
	  }
	  
	  fecDevice->fecRingRelease() ;
	}
	else {
	  
	  // Last CCU, an intermediate CCU is between the last CCU and the FEC
	  try {
	    // Set the output to B for the previous CCU
	    if (indexPCCU == 0) { // FEC
	      fecDevice->setFecRingCR0 (fecCR0 | 0x8) ;
	    }
	    else { // CCU
	      fecDevice->setCcuCRC (indexPCCU, ccuCRC | 0x2) ;
	    }

	    fecDevice->fecRingRelease() ;

	    // FEC on input B
	    fecDevice->setFecRingCR0 (fecCR0 | 0x10) ;
	    
	    fecDevice->fecRingRelease() ;
	  }
	  catch (FecExceptionHandler &e) { // Normal error => ring is broken
	  }	    
	}

	//try {
	// Reset ring B
	//  fecDevice->setFecRingCR0 (fecDevice->getFecRingCR0() | 0x0200) ;
	//}
	//catch (FecExceptionHandler &e) { // Normal error => ring is broken
	//}	    

	// Message
	if (! noGetchar_) {
	  std::cout << "Press <Enter> to continue" ;
	  getchar() ;
	}

	//if (isFecSR0Correct(fecDevice->getFecRingSR0()))
	//  testScanCCU(fecAccess) ;
	//else std::cout << "The FEC status register 0 is not correct (0x" << std::hex << fecDevice->getFecRingSR0() << ")" << std::endl ;
      }
      else {
	if (ccuAddress != 0)
	  std::cout << "The CCU 0x" << std::hex << (int)ccuAddress << " is not on the FEC " << std::dec << (int)fecAddress << ", ring " << (int)ringAddress << std::endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // Delete = already store in the table of FEC Access class and CCU list
  delete ccuList ;
  delete ccuListRing ;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       Key function                                      */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/**
 * <p>Command: -key index
 * <p>Command: -SR0 sr0
 * \param index - path index 
 * \warning no loop can be applied
 */
int testKey ( keyType myKey, tscType32 wordSR0 ) {

  if (myKey != 0xFFFFFFFF) {
    std::cout << "key: 0x" << std::hex << myKey << std::endl ;
    std::cout << "fecAddress     => 0x" << std::hex << getFecKey(myKey) << std::endl ;
    std::cout << "ringAddress    => 0x" << std::hex << getRingKey(myKey) << std::endl ;
    std::cout << "ccuAddress     => 0x" << std::hex << getCcuKey(myKey) << std::endl ;
    std::cout << "channelAddress => 0x" << std::hex << getChannelKey(myKey) << std::endl ;
    std::cout << "deviceAddress  => 0x" << std::hex << getAddressKey(myKey) << std::endl ;

    char msg[100] ;
    decodeKey (msg, myKey) ;
    std::cout << msg << std::endl ;
  }
  else {
    std::cout << FecRingRegisters::decodeFECSR0(wordSR0) << std::endl ;
  }

  return 0 ;
}

/**
 * <p> command: -justatest
 * \param fecAccess - FEC Access object
 * \param index - path index 
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 */
void justATest ( FecAccess *fecAccess, keyType myKey ) {

#define LOOPFORMEASURE 100000

  // *********************************************************************************
  // This test measure the time for one device for the one register to apply a transaction

  // enable all the channel for the CCU
  FecRingDevice *fecFvice = NULL ;
  try {
    fecFvice = fecAccess->getFecRingDevice(myKey) ;
    fecAccess->setCcuCRE(myKey,0xFFFFFFFF) ;
    fecFvice->setInitI2cChannelCRA(myKey,true,100) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Cannot init the FEC and CCU: " << e.what() << std::endl ;
    return ;
  }

  tscType8 frameRead8 [DD_USER_MAX_MSG_LENGTH] = {0} ;
  tscType8 frameWrite8[DD_USER_MAX_MSG_LENGTH] = {0} ;
  std::ostringstream message ;
  switch (getAddressKey(myKey)) {
  case 0x20: // APV
  case 0x21: // APV
  case 0x22: // APV
  case 0x23: // APV
  case 0x24: // APV
  case 0x25: { // APV
    message << "APV 0x" << std::hex << myKey << " register " << std::dec << APV25_MOD_REG  << " for " << LOOPFORMEASURE << " times" ;
    tscType8 frameRead[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x05,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEREADRALMODE,getAddressKey(myKey),APV25_MOD_REG} ;
    tscType8 frameWrite[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x06,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEWRITERALMODE,getAddressKey(myKey),APV25_MOD_REG,1} ;

    memcpy (frameRead8, frameRead, DD_USER_MAX_MSG_LENGTH) ;
    memcpy (frameWrite8, frameWrite, DD_USER_MAX_MSG_LENGTH) ;
    break ;
  }
  case DCUADDRESS: { // DCU
    message << "DCU 0x" << std::hex << myKey << " register " << std::dec << TREG  << " for " << LOOPFORMEASURE << " times" ;
    tscType8 frameRead[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x04,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEREADNORMALMODE,getAddressKey(myKey),TREG} ;
    tscType8 frameWrite[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x05,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEWRITENORMALMODE,getAddressKey(myKey),TREG,1} ;

    memcpy (frameRead8, frameRead, DD_USER_MAX_MSG_LENGTH) ;
    memcpy (frameWrite8, frameWrite, DD_USER_MAX_MSG_LENGTH) ;
    break ;
  }
  case 0x43: { // MUX
    message << "MUX 0x" << std::hex << myKey << " register " << std::dec << MUX_RES_REG << " for "  << LOOPFORMEASURE << " times" ;
    tscType8 frameRead[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x05,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEREADRALMODE,getAddressKey(myKey),MUX_RES_REG} ;
    tscType8 frameWrite[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x06,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEWRITERALMODE,getAddressKey(myKey),MUX_RES_REG,1} ;    

    memcpy (frameRead8, frameRead, DD_USER_MAX_MSG_LENGTH) ;
    memcpy (frameWrite8, frameWrite, DD_USER_MAX_MSG_LENGTH) ;
    break ;
  }
  case 0x44: { // PLL
    message << "PLL 0x" << std::hex << myKey << " register " << std::dec << CNTRL_1 << " for "  << LOOPFORMEASURE << " times" ;
    tscType8 frameRead[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x04,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEREADNORMALMODE,getAddressKey(myKey),CNTRL_1} ;
    tscType8 frameWrite[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x05,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEWRITENORMALMODE,getAddressKey(myKey),CNTRL_1,1} ;

    memcpy (frameRead8, frameRead, DD_USER_MAX_MSG_LENGTH) ;
    memcpy (frameWrite8, frameWrite, DD_USER_MAX_MSG_LENGTH) ;
    break ;
  }
  case 0x60: { // AOH
    message << "AOH 0x" << std::hex << myKey << " register " << std::dec << 0 << " for "  << LOOPFORMEASURE << " times" ;
    tscType8 frameRead[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x04,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEREADNORMALMODE,getAddressKey(myKey),0x0} ;
    tscType8 frameWrite[DD_USER_MAX_MSG_LENGTH] = {getCcuKey(myKey),FRAMEFECNUMBER,0x05,getChannelKey(myKey),0xFE,CMD_SINGLEBYTEWRITENORMALMODE,getAddressKey(myKey),0x0,1} ;

    memcpy (frameRead8, frameRead, DD_USER_MAX_MSG_LENGTH) ;
    memcpy (frameWrite8, frameWrite, DD_USER_MAX_MSG_LENGTH) ;
    break ;
  }
  case 0x70: { // DOH
    std::cerr << "No operation is intend for the DOH" << std::endl ;
    return ;
    break ;
  }
  default: { 
    std::cerr << "Unknown device" << std::endl ;
    return ;
  }
  }

  try {
    tscType8 frameD[DD_USER_MAX_MSG_LENGTH] = {0} ;
    time_t timeD0 = time(NULL);
    for (unsigned int i = 0 ; i < LOOPFORMEASURE ; i ++) {
      fecFvice->writeFrame(frameWrite8) ;
      fecFvice->readFrame(frameWrite8[4],frameD,3) ;
    }
    time_t timeD1 = time(NULL) ;
    unsigned int diffD = timeD1 - timeD0 ;
    std::cout << "Time to write " << message.str() << " is " << diffD << " so " << (double)((double)diffD/(double)LOOPFORMEASURE) << " per operation" << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Error during the write of the frame on " << message.str() << std::endl ;
    std::cerr << e.what() << std::endl ;
  }

  try {
    tscType8 frameD[DD_USER_MAX_MSG_LENGTH] = {0} ;
    time_t timeD0 = time(NULL);
    for (unsigned int i = 0 ; i < LOOPFORMEASURE ; i ++) {
      fecFvice->writeFrame(frameRead8) ;
      fecFvice->readFrame(frameRead8[4],frameD,4) ;
    }
    time_t timeD1 = time(NULL) ;
    unsigned int diffD = timeD1 - timeD0 ;
    std::cout << "Time to read " << message.str() << " is " << diffD << " so " << (double)((double)diffD/(double)LOOPFORMEASURE) << " per operation" << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Error during the write of the frame on " << message.str() << std::endl ;
    std::cerr << e.what() << std::endl ;
  }

  return ;

  // **********************************************************************************
  // TIME TO READ ONE CONTROL REGISTER IN THE FEC
  // Pentium III (Coppermine)
  // Time to read 10000000 times the status register 0 of the FEC is 168 so 1.68e-05 per operation
  // Time to read 10000000 times the status register 0 of the FEC is 152 so 1.52e-05 per operation
  // Time to write 10000000 times the control register 0 of the FEC is 157 so 1.57e-05 per operation
  // Time to write 10000000 times the control register 0 of the FEC is 142 so 1.42e-05 per operation
  // Intel(R) Pentium(R) 4 CPU 3.00GHz
  // Time to read 10000000 times the status register 0 of the FEC is 151 so 1.51e-05 per operation
  // Time to read 10000000 times the status register 0 of the FEC is 128 so 1.28e-05 per operation
  // Time to write 10000000 times the control register 0 of the FEC is 137 so 1.37e-05 per operation
  // Time to write 10000000 times the control register 0 of the FEC is 111 so 1.11e-05 per operation
  // 2 x Intel(R) Xeon(TM) CPU 3.00GHz
  // Time to read 10000000 times the status register 0 of the FEC is 151 so 1.51e-05 per operation
  // Time to read 10000000 times the status register 0 of the FEC is 124 so 1.24e-05 per operation
  // Time to write 10000000 times the control register 0 of the FEC is 144 so 1.44e-05 per operation
  // Time to write 10000000 times the control register 0 of the FEC is 120 so 1.2e-05 per operation
//   time_t timeM0 = time(NULL);
//   for (unsigned int i = 0 ; i < LOOPFORMEASURE ; i ++) {
//     fecAccess->getFecRingSR0(myKey) ;
//   }
//   time_t timeM1 = time(NULL) ;
//   unsigned int diff = timeM1 - timeM0 ;
//   std::cout << "Time to read " << std::dec << LOOPFORMEASURE << " times the status register 0 of the FEC is " << diff << " so " << (double)((double)diff/(double)LOOPFORMEASURE) << " per operation" << std::endl ;

//   FecRingDevice *fecEvice = fecAccess->getFecRingDevice(myKey) ;
//   timeM0 = time(NULL);
//   for (unsigned int i = 0 ; i < LOOPFORMEASURE ; i ++) {
//     fecEvice->getFecRingSR0() ;
//   }
//   timeM1 = time(NULL) ;
//   diff = timeM1 - timeM0 ;
//   std::cout << "Time to read " << std::dec << LOOPFORMEASURE << " times the status register 0 of the FEC is " << diff << " so " << (double)((double)diff/(double)LOOPFORMEASURE) << " per operation" << std::endl ;

  // **********************************************************************************
  // TEST OF THE METHOD BYPASS
  std::cout << "// 0x1, 0x2, 0x5, 0x9, 0x6, 0x6f, 0x34, 0x7E" << std::endl ;
  std::cout << "Press enter to continue" ; getchar() ;
  bypass (fecAccess,getFecKey(myKey),getRingKey(myKey),0x34,0x7E,0x0) ;
  std::cout << "Press enter to continue" ; getchar() ;
  bypass (fecAccess,getFecKey(myKey),getRingKey(myKey),0x0,0x1) ;
  std::cout << "Press enter to continue" ; getchar() ;
  bypass (fecAccess,getFecKey(myKey),getRingKey(myKey),0x0,0x2) ;
  std::cout << "Press enter to continue" ; getchar() ;
  bypass (fecAccess,getFecKey(myKey),getRingKey(myKey),0x1,0x5) ;
  std::cout << "Press enter to continue" ; getchar() ;
  bypass (fecAccess,getFecKey(myKey),getRingKey(myKey),0x2,0x9) ;
  std::cout << "Press enter to continue" ; getchar() ;
  bypass (fecAccess,getFecKey(myKey),getRingKey(myKey),0x5,0x6) ;
  std::cout << "Press enter to continue" ; getchar() ;
  bypass (fecAccess,getFecKey(myKey),getRingKey(myKey),0x9,0x6F) ;
  std::cout << "Press enter to continue" ; getchar() ;
  bypass (fecAccess,getFecKey(myKey),getRingKey(myKey),0x6F,0x0,0x7E) ;
  std::cout << "Press enter to continue" ; getchar() ;

  return ;

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  // **********************************************************************************
  // TEST the send of a frame other a ring
  // Fill a buffer of frames to have at least 512 words in the FIFO receive
  if (fecAccess->getFecBusType() != FECVME) {
    std::cerr << "Not a VME FEC" << std::endl ;
    return ;
  }
  FecRingDevice *fecVmeRingDevice = (FecRingDevice *)fecAccess->getFecRingDevice ( myKey ) ;
  fecVmeRingDevice->setCcuCRE(myKey, 0xFFFF) ;
  std::cout << "CRE of CCU 0x" << std::hex << getCcuKey(myKey) << " = 0x" << fecVmeRingDevice->getCcuCRE(myKey) << std::endl ;
  tscType32 fifotransmit[256] = {0} ;
  tscType32 fiforeceive[1024] = {0} ;
  int pos = 0 ;
  int ccu = getCcuKey(myKey) ;

  int channel = 0x10 ;

  for (int tra = 1 ; (tra < 17) ; tra++) {

    if (getChannelKey(myKey) == 0) {
      // CCU test
      fifotransmit[pos] = ((ccu & 0xFF) << 24) | 0x00000300; pos ++ ;
      std::cout << "Tra FIFO[" << std::dec << std::setw(2) << std::setfill('0') << (pos-1) << "] = " << std::hex << std::setw(8) << std::setfill('0') << fifotransmit[pos-1] << std::endl ;    
      fifotransmit[pos] = ((tra & 0xFF) << 24) | 0x00140000; pos ++ ;
      std::cout << "Tra FIFO[" << std::dec << std::setw(2) << std::setfill('0') << (pos-1) << "] = " << std::hex << std::setw(8) << std::setfill('0') << fifotransmit[pos-1] << std::endl ;    
    }
    else {
      
      // I2C ------------------------
//       fifotransmit[pos] = ((ccu & 0xFF) << 24) | 0x00000600 | (getChannelKey(myKey)) ; pos ++ ;
//       std::cout << "Tra FIFO[" << std::dec << std::setw(2) << std::setfill('0') << (pos-1) << "] = " << std::hex << std::setw(8) << std::setfill('0') << fifotransmit[pos-1] << std::endl ;    
//       fifotransmit[pos] = ((tra & 0xFF) << 24) | 0x0002202F; pos ++ ;
//       std::cout << "Tra FIFO[" << std::dec << std::setw(2) << std::setfill('0') << (pos-1) << "] = " << std::hex << std::setw(8) << std::setfill('0') << fifotransmit[pos-1] << std::endl ;    
      
      fifotransmit[pos] = ((ccu & 0xFF) << 24) | 0x00000600 | (channel) ; pos ++ ;
      std::cout << "Tra FIFO[" << std::dec << std::setw(2) << std::setfill('0') << (pos-1) << "] = " << std::hex << std::setw(8) << std::setfill('0') << fifotransmit[pos-1] << std::endl ;    
      fifotransmit[pos] = ((tra & 0xFF) << 24) | 0x0002202F; pos ++ ;
      std::cout << "Tra FIFO[" << std::dec << std::setw(2) << std::setfill('0') << (pos-1) << "] = " << std::hex << std::setw(8) << std::setfill('0') << fifotransmit[pos-1] << std::endl ;   
      channel ++;
      if (channel == 0x20) channel = 0x10 ;
    }
  }

  fecVmeRingDevice->setFifoTransmit(fifotransmit,pos) ;
  // Clear the error bit
  fecVmeRingDevice->setFecRingCR1 ( DD_FEC_CLEAR_ERRORS ) ;
  // Toggle the send bit of CTRLO with RMW operation
  fecVmeRingDevice->setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_OR ) ;
  fecVmeRingDevice->setFecRingCR0 ( FEC_CR0_SEND | FEC_CR0_ENABLEFEC, CMD_XOR ) ;

  // Wait for the FIFO transmit is not running
  sleep(10) ;
  tscType32 fecSR0 = fecVmeRingDevice->getFecRingSR0() ;
  while ( ((fecSR0 & FEC_SR0_TRARUN) || (!(fecSR0 & FEC_SR0_TRAEMPTY))) ) {
    fecSR0 = fecVmeRingDevice->getFecRingSR0() ;
  }
  while (fecSR0 & FEC_SR0_RECRUN) {
    fecSR0 = fecVmeRingDevice->getFecRingSR0() ;
  }

  tscType32 wordSR0 = (fecVmeRingDevice->getFecRingSR0() & 0xFFFF0000) >> 16 ;
  std::cout << "SR0 = " << std::hex << (int)(fecVmeRingDevice->getFecRingSR0() & 0xFFFF) << std::endl;
  std::cout << "Waiting for " << std::dec << wordSR0 << " words in FIFO receive" << std::endl ;

  unsigned int position = 0 ;
  while (!(fecVmeRingDevice->getFecRingSR0() & FEC_SR0_RECEMPTY)) {
    fiforeceive[position] = fecVmeRingDevice->getFifoReceive() ;
    std::cout << "Rec FIFO[" << std::dec << std::setw(2) << std::setfill('0') << position << "] = " << std::hex << std::setw(8) << std::setfill('0') << fiforeceive[position] << std::endl ;

    position ++ ;
  }

  return ;
#endif
  
  while (wordSR0 > 0) {
    fecVmeRingDevice->getFifoReceive(fiforeceive,wordSR0) ;
    unsigned int position = 0 ; int transaction = 1 ;
    while (position < wordSR0) {
      //std::cout << "FIFO[" << std::dec << position << "] = " << std::hex << fiforeceive[position] << std::endl ;
      tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = {0} ; // frame received
      unsigned int cpt = 0 ;
      int realSize = (fiforeceive[position] >> 8) & 0xFF ;
      int realSize32 = (realSize+3+1)/4;
      if ( ((realSize+3+1) % 4) != 0 ) realSize32 += 1 ;
      //std::cout << realSize << "/" << realSize32 << std::endl ;
      for ( int i = 0 ; i < realSize32 ; i++, position ++) {
	std::cout << "Rec FIFO[" << std::dec << std::setw(2) << std::setfill('0') << position << "] = " << std::hex << std::setw(8) << std::setfill('0') << fiforeceive[position] << std::endl ;
	frame[cpt++] = (fiforeceive[position] >> 24) & 0xFF ;
	frame[cpt++] = (fiforeceive[position] >> 16) & 0xFF ;
	frame[cpt++] = (fiforeceive[position] >>  8) & 0xFF ;
	frame[cpt++] = (fiforeceive[position])       & 0xFF ;
      }
      std::cout << "Frame received ( { " ;
      for (int i = 0 ; i < ((frame[2]+3)+1) ; i ++ )
	std::cout << "0x" << std::hex << (int)frame[i] << " " ;
      std::cout << "} )" << std::dec << std::endl ;

      if (transaction != frame[4]) {
	std::cout << "Transaction = " << (int)frame[4] << " should be " << (int)transaction << std::endl;
	getchar() ;
      }
      if (frame[0] != ccu) transaction ++ ;
    }

    sleep(1) ;
    wordSR0 = (fecVmeRingDevice->getFecRingSR0() & 0xFFFF0000) >> 16 ;
    std::cout << "SR0 = " << std::hex << (int)(fecVmeRingDevice->getFecRingSR0() & 0xFFFF) << std::endl;
    std::cout << "Waiting for " << std::dec << wordSR0 << " words in FIFO receive" << std::endl ;
  }

  return ;

  // Access a register of the given DCU and write/read for ever the TREG (test) register
  FecRingDevice *fecRingDevice = fecAccess->getFecRingDevice ( myKey ) ;



  // Build the frame for the write
  tscType8 frameWrite[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(myKey), FRAMEFECNUMBER, 
    0x05, getChannelKey(myKey), 
    0xFE, CMD_SINGLEBYTEWRITENORMALMODE,
    (tscType8)(getAddressKey(myKey) | TREG), 0 } ;

  // Build the frame for the read
  tscType8 frameRead[DD_USER_MAX_MSG_LENGTH*4] = { 
    getCcuKey(myKey), FRAMEFECNUMBER, 
    0x04, getChannelKey(myKey), 
    0xFE, CMD_SINGLEBYTEREADNORMALMODE,
    (tscType8)(getAddressKey(myKey) | TREG) } ;

  // readout frame
  tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] ;

  // Error oounter
  long double errorCounter = 0 ;
  long double counter = 0 ;

  char msg[80] ;
  decodeKey(msg,myKey) ;
  std::cerr << "This program will loop on the TREG registers on " << msg << std::endl ;
  // Enable the channel on the given CCU
  std::cerr << "Enable the channel on the CCU 0x" << std::hex << (int)getCcuKey(myKey) << std::endl ;
  fecRingDevice->setChannelEnable ( myKey, true ) ;
  // Enable the force ack on the CCU
  std::cerr << "Enable the force ack. on the CCU 0x" << std::hex << (int)getCcuKey(myKey) << std::endl ;
  fecRingDevice->setInitI2cChannelCRA( myKey, true, 100) ;
  std::cerr << "Write frame is: " << FecRingRegisters::decodeFrame (frameWrite) << std::endl ;
  std::cerr << "Read frame is:  " << FecRingRegisters::decodeFrame (frameRead) << std::endl ;
  std::cerr << "Start time " << std::dec << time(NULL) << std::endl ;

  // Loop on all the possible values
  for (int i = 0 ; i <= 255 ; i ++) {

    std::cout << "----------------------------> Write in the TREG the value " << std::dec << (int)i << std::endl ;
    frameWrite[7] = (tscType8) i ;
    std::string error = "No error" ;
    try {
      // Write frame with write TREG
      error = "ERROR: Cannot read back the value of the DCU TREG" ;
      fecRingDevice->writeFrame ( frameWrite ) ;
      std::cout << "Transaction number is: " << std::dec << (int)frameWrite[4] << " (0x" << std::hex << (int)frameWrite[4] << ")" << std::endl ;

      // Read the force ack of the write TREG
      std::cout << "Read back the force ack." << std::endl ;
      error = "ERROR: Error during the read back of the force ack." ;
      fecRingDevice->readFrame ( frameWrite[4], frame, 3) ;

      // Write frame with read TREG
      std::cout << "Write the frame to read Back the value of TREG" << std::endl ;
      error = "ERROR: Error the frame with read of TREG" ;
      fecRingDevice->writeFrame ( frameRead ) ;

      // Read the answer for the value of TREG
      std::cout << "Read back the value of TREG" << std::endl ;
      error = "ERROR: Error during the read back of the value of TREG" ;
      fecRingDevice->readFrame ( frameRead[4], frame, 4) ;

      // Analyse the frame to have the value written
      if (frame[5] != i) {
	std::cerr << "ERROR: Found a bad value in the read, expected value is " << std::dec << (int)i << ", value received is " << std::dec << (int)frame[5] << std::endl ;
	std::cerr << "The read request is: " << FecRingRegisters::decodeFrame (frameRead) << std::endl ;
	std::cerr << "The answer is: " << FecRingRegisters::decodeFrame (frame) << std::endl ;
      }
      else std::cout << "Values read back is correct: " << std::dec << (int)frame[5] << std::endl ;
    }
    catch (FecExceptionHandler &e) {
	
      std::cerr << "------------- ERROR -------------" << std::endl ;
      std::cerr << error << std::endl ;
      std::cerr << e.what()  << std::endl ;
      std::cerr << "---------------------------------" << std::endl ;
	
      // display all registers
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;
      if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
      if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;
      
      // Reset the FEC
      std::cout << "Make a FEC reset" << std::endl ;
      fecAccess->fecRingReset (myKey) ;
      
      // Enable the channel on the given CCU
      std::cout << "Enable the channel on the CCU 0x" << std::hex << (int)getCcuKey(myKey) << std::endl ;
      fecRingDevice->setChannelEnable ( myKey, true ) ;
      // Enable the force ack on the CCU
      std::cout << "Enable the force ack. on the CCU 0x" << std::hex << (int)getCcuKey(myKey) << std::endl ;
      fecRingDevice->setInitI2cChannelCRA( myKey, true, 100) ;

      errorCounter ++ ;
      std::cerr << std::dec << counter << " downloads with " << errorCounter << " errors (timestamp = " << time(NULL) << ")" << std::endl ;
    }

    counter ++ ;

#define VALUEPRINT 100000
    if ((counter/VALUEPRINT) - (unsigned long)(counter/VALUEPRINT) == 0) {
      std::cerr << std::dec << counter << " downloads with " << errorCounter << " errors (timestamp = " << time(NULL) << ")" << std::endl ;
    }
  }
  
#ifdef COUCOU

#define NVALUE 500 

  std::cout << "Before reset" << std::endl ;
  displayFecStatus (fecAccess, myKey) ;
  getchar() ;

  //void srandom(unsigned int seed);
  srandom (time(NULL)) ;

  FecRingDevice *fecRingDevice = fecAccess->getFecRingDevice ( myKey ) ;
  //FecVmeRingDevice *fecVmeRingDevice = (FecVmeRingDevice *)fecRingDevice ; 

  fecAccess->fecRingReset (myKey) ;
  fecAccess->setFecRingCR0 (myKey, 0) ;

  std::cout << "After reset" << std::endl ;
  displayFecStatus (fecAccess, myKey) ;
  getchar() ;

  tscType32 *values = new tscType32[NVALUE] ;

  for (int i = 0 ; i < NVALUE ; i ++) {

    values[i] = random ( ) ;
    //std::cout << "Value[" << i << "] = " << values[i] << std::endl ;
  }

  for (int i = 0 ; i < NVALUE ; i ++) {

    fecRingDevice->setFifoTransmit ( values[i] ) ;
  }

  bool err = false ;
  for (int i = 0 ; i < NVALUE ; i ++) {

    tscType32 val = fecRingDevice->getFifoTransmit() ;
    if (val != values[i]) {

      std::cout << "Values must be " << values[i] << " and it is " << val << std::endl ;
      err = true ;
    }

    //std::cout << "Value[" << i << "] = " << val << std::endl ;
  }

  if (err) {
    std::cout << "FIFO transmit: Error in comparison" << std::endl ;
  }
  else {
    std::cout << "FIFO transmit: No problem in comparison" << std::endl ;
  }
  getchar() ;

  for (int i = 0 ; i < NVALUE ; i ++) {

    fecRingDevice->setFifoReceive ( values[i] ) ;
  }

  err = false ;
  for (int i = 0 ; i < NVALUE ; i ++) {

    tscType32 val = fecRingDevice->getFifoReceive() ;
    if (val != values[i]) {

      std::cout << "Values must be " << values[i] << " and it is " << val << std::endl ;
      err = true ;
    }

    //std::cout << "Value[" << i << "] = " << val << std::endl ;
  }

  if (err) {
    std::cout << "FIFO receive: Error in comparison" << std::endl ;
  }
  else {
    std::cout << "FIFO receive: No problem in comparison" << std::endl ;
  }
  getchar() ;


  for (int i = 0 ; i < NVALUE ; i ++) {

    fecRingDevice->setFifoReturn ( values[i] ) ;
  }

  err = false ;
  for (int i = 0 ; i < NVALUE ; i ++) {

    tscType32 val = fecRingDevice->getFifoReturn() ;
    if (val != values[i]) {

      std::cout << "Values must be " << values[i] << " and it is " << val << std::endl ;
      err = true ;
    }

    //std::cout << "Value[" << i << "] = " << val << std::endl ;
  }

  if (err) {
    std::cout << "FIFO return: Error in comparison" << std::endl ;
  }
  else {
    std::cout << "No problem in comparison" << std::endl ;
  }
  getchar() ;  

  displayFecStatus (fecAccess, myKey) ;
  getchar () ;

  // ---------------------------------------------------------
  for (int i = 0 ; i < 10 ; i ++) {

    fecRingDevice->setFifoTransmit ( values[i] ) ;
    std::cout << "Value[" << i << "] = " << values[i] << std::endl ;    
  }  
  
  // send over the ring
  fecAccess->setFecRingCR0 (myKey, 0x3) ;
  fecAccess->setFecRingCR0 (myKey, 0x1) ;

  // Read the values from FIFO receive
  for (int i = 0 ; i < 10 ; i ++) {

    tscType32 val = fecRingDevice->getFifoReceive() ;
    std::cout << "Value[" << i << "] = " << val << std::endl ;

    //if (val != values[i])
    std::cout << "Values " << values[i] << " / " << val << std::endl ;
  }

  displayFecStatus (fecAccess, myKey) ;
  getchar () ;

#endif

  /** Test of the method bool checkRedundancyAddress ( 
   * std::list<keyType> *ccuList, 
   * uint numberCCU, 
   * keyType *ccuAddresses, 
   * int ccuIndex )
  */

//   keyType index1  = buildCompleteKey(getFecKey(myKey), getRingKey(myKey), 0x1, 0, 0) ;
//   keyType index2  = buildCompleteKey(getFecKey(myKey), getRingKey(myKey), 0x2, 0, 0) ;
//   keyType index10 = buildCompleteKey(getFecKey(myKey), getRingKey(myKey), 0x10, 0, 0) ;
//   keyType index9  = buildCompleteKey(getFecKey(myKey), getRingKey(myKey), 0x9, 0, 0) ;
//   keyType index8  = buildCompleteKey(getFecKey(myKey), getRingKey(myKey), 0x8, 0, 0) ;
//   keyType index7  = buildCompleteKey(getFecKey(myKey), getRingKey(myKey), 0x7, 0, 0) ;
//   keyType index7E = buildCompleteKey(getFecKey(myKey), getRingKey(myKey), 0x7E, 0, 0) ;

//   std::list<keyType> ccuList ;
//   ccuList.push_back (index1) ;
//   ccuList.push_back (index2) ;
//   ccuList.push_back (index10) ;
//   ccuList.push_back (index9) ;
//   ccuList.push_back (index8) ;
//   //ccuList.push_back (index7) ;
//   ccuList.push_back (index7E) ;

//   // Check if the FEC output B goes to input B of the first CCU
//   uint ccuAddresses[127] = {index1, index2, index10, index9, index8, index7, index7E} ;
//   if (checkRedundancyAddress (&ccuList, 7, ccuAddresses, 5))
//     std::cout << "Check if the FEC output B goes to input B of the first CCU ===> not Ok" << std::endl ;
//   else
//     std::cout << "Check if the FEC output B goes to input B of the first CCU ===> Ok" << std::endl ;

  /*
  fecAccess->getFecList( ) ;

  keyType index = buildFecRingKey(0,0) ;

  try {
    fecAccess->fecHardReset(index);
    fecAccess->fecRingReset(index);
  }
  catch (FecExceptionHandler &e) { std::cout << e.what() << std::endl; }

  try {
    if (! isFecSR0Correct(fecAccess->getFecRingSR0(index))) emptyFIFOs ( fecAccess, index, true ) ;
    if (isFecSR0Correct(fecAccess->getFecRingSR0(index))) {
  
    }
  }
  catch (FecExceptionHandler &e) { std::cout << e.what() << std::endl; }

  emptyFIFOs(fecAccess, index, true);

  unsigned int piaResetValue = 0xFF ;
  unsigned long delayActiveReset = 10 ;
  unsigned long intervalDelayReset = 10000 ;
  tscType16 firstPiaChannel = 0x30;
  testPIAResetfunctions (fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1, firstPiaChannel, 
			 piaResetValue, delayActiveReset, intervalDelayReset, 1, 0) ;
  testPIAResetfunctions (fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x2, firstPiaChannel, 
			 piaResetValue, delayActiveReset, intervalDelayReset, 1, 0) ;
  testPIAResetfunctions (fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1B, firstPiaChannel, 
			 piaResetValue, delayActiveReset, intervalDelayReset, 1, 0) ;

  // Ring 0
  testScanTrackerDevice ( fecAccess, (tscType8)0, (tscType8)0, 1, 0, false) ;

  std::vector<std::string> results ;
  */
  // CCU 0x1
  /*testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1, (tscType8)0x16,
		       true, true, true, true, true, false, true, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1, (tscType8)0x17,
		       true, true, true, true, true, false, true, 
		       1, 0, results); 
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1, (tscType8)0x1F,
		       true, true, true, true, true, false, true, 
		       1, 0, results) ;

  // CCU 0x2
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x2, (tscType8)0x16,
		       true, true, true, true, true, false, true, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x2, (tscType8)0x17,
		       true, true, true, true, true, false, true, 
		       1, 0, results); 
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x2, (tscType8)0x1F,
		       true, true, true, true, true, false, true, 
		       1, 0, results) ;

  // CCU 0x1B
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1B, (tscType8)0x16,
		       true, true, true, true, true, false, true, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1B, (tscType8)0x17,
		       true, true, true, true, true, false, true, 
		       1, 0, results); 
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1B, (tscType8)0x1F,
		       true, true, true, true, true, false, true, 
		       1, 0, results) ;*/

  // dcuccu and doh

  /*
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x42, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;

  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x44, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;

  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x2d, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x41, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x4a, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x43, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x47, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x2, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x40, (tscType8)0x1e,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;



 
  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x2, (tscType8)0x1f,
		       false, false, false, false, false, true, false, 
		       1, 0, results) ;

  testTrackerDevices ( fecAccess, (tscType8)0, (tscType8)0, (tscType8)0x1, (tscType8)0x1f,
		       false, false, false, false, false, true, false, 
		       1, 0, results) ;
  */

  // Next ring

  /*  autoTrackerRedundancyRing ( fecAccess, (tscType8)1, (tscType8)0, 3, 1, 0) ;

  testPIAResetfunctions (fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1, firstPiaChannel, 
			 piaResetValue, delayActiveReset, intervalDelayReset, 1, 0) ;
  testPIAResetfunctions (fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x2, firstPiaChannel, 
			 piaResetValue, delayActiveReset, intervalDelayReset, 1, 0) ;
  testPIAResetfunctions (fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1B, firstPiaChannel, 
			 piaResetValue, delayActiveReset, intervalDelayReset, 1, 0) ;

  testScanTrackerDevice ( fecAccess, (tscType8)1, (tscType8)0, 1, 0, false); 

  // CCU 0x1
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1, (tscType8)0x16,
		       true, false, true, true, true, false, true, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1, (tscType8)0x17,
		       true, false, true, true, true, false, true, 
		       1, 0, results); 
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1, (tscType8)0x1F,
		       true, false, true, true, true, false, true, 
		       1, 0, results) ;

  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1, (tscType8)0x10,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;

  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1, (tscType8)0x10,
		       false, false, false, false, false, true, false, 
		       1, 0, results) ;

  // CCU 0x2
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x2, (tscType8)0x16,
		       true, false, true, true, true, false, true, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x2, (tscType8)0x17,
		       true, false, true, true, true, false, true, 
		       1, 0, results); 
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x2, (tscType8)0x1F,
		       true, false, true, true, true, false, true, 
		       1, 0, results) ;

  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x2, (tscType8)0x10,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;

  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x2, (tscType8)0x10,
		       false, false, false, false, false, true, false, 
		       1, 0, results) ;

  // CCU 0x1B
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1B, (tscType8)0x16,
		       true, false, true, true, true, false, true, 
		       1, 0, results) ;
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1B, (tscType8)0x17,
		       true, false, true, true, true, false, true, 
		       1, 0, results); 
  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1B, (tscType8)0x1F,
		       true, false, true, true, true, false, true, 
		       1, 0, results) ;

  testTrackerDevices ( fecAccess, (tscType8)1, (tscType8)0, (tscType8)0x1B, (tscType8)0x10,
		       false, false, false, true, false, false, false, 
		       1, 0, results) ;*/

}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       Reset function                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** This method make different tests on the FEC in order
 * <p> command: -fectest
 * The tests are:
 * <ul>
 * <li> Read / write random values into the different FIFO and read back for comparison: not already implemented
 * <li> Test the reset line on the FEC to see if it goes to the CCUs
 * </ul>
 * \param fecAccess - FEC access object
 * \param loop - number of times to make the test
 * \param tms - time between two accesses
 */
void fecTest ( FecAccess *fecAccess,long loop, unsigned long tms ) {

  // --------------------------------------------------------------------------------------------
  // Only VME FEC 
  if (fecAccess->getFecBusType() != FECVME) {
    std::cerr << "No test can be performed on USB/PCI FEC, only VME FEC can be used" << std::endl ;
    return ;
  }

  // --------------------------------------------------------------------------------------------
  // set the SSID to TRACKER
#define SSID "TRACKER"
  std::cout << "Set the SSID of all FECs to " << SSID << std::endl ;
  setSSID ( fecAccess, SSID, false ) ;

  // --------------------------------------------------------------------------------------------
  // Output file
#define OUTPUTTESTFILENAME "/tmp/FecTest"
  time_t value = time(NULL) ;
  struct tm *structTime = localtime(&value) ;
  std::stringstream fileName ; 
  fileName << OUTPUTTESTFILENAME << std::setw(2) << std::setfill('0') << structTime->tm_mday 
	   << std::setw(2) << std::setfill('0') << (structTime->tm_mon+1)
	   << std::setw(2) << std::setfill('0') << (structTime->tm_year-100) 
	   << "_" << std::setw(2) << std::setfill('0') << structTime->tm_hour 
	   << std::setw(2) << std::setfill('0') << structTime->tm_min << ".txt" ;
  std::ofstream outputStream(fileName.str().c_str()) ;
  if (!outputStream.is_open()) {
    std::cerr << "Cannot open file " << fileName.str() << " test stop here" << std::endl ;
    return ;
  }
  else {
    std::cout << "---------- Output result will be set to " << fileName.str() << std::endl ;
  }
 
  // Information
  if (outputStream.is_open()) 
    outputStream << "FEC" << "\t" 
		 << "Ring" << "\t" 
		 << "FECHARDID" << "\t" 
		 << "mFEC" << "\t" 
		 << "VME" << "\t" 
		 << "Trigger" << "\t"
      		 << "SSID" << "\t" 
		 << "TRIG_SR0" << "\t"
		 << "TEMP1" << "\t"
		 << "TEMP2" << "\t"
		 << "CR0" << "\t"
		 << "SR0" << "\t"
		 << "Status" << "\t" // frame sent status (transaction number of example)
		 << "time in ms" << "\t"
		 << std::endl ;

  // FEC list
  std::list<keyType> *fecSlotList = NULL ;
  try {
    fecSlotList = fecAccess->getFecList ( ) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << "Unable to retreive the FEC list: " << e.what() << std::endl ;
    return ;
  }
  if ( (fecSlotList == NULL) || (fecSlotList->size() == 0) ) {
    std::cerr << "No FEC in the crates" << std::endl ;
    return ;
  }

  // For each FEC/Ring
  for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {

    unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

    keyType fecRingIndex = *p ;
    std::cout << "Testing FEC: " << std::dec << getFecKey(fecRingIndex) << "." << getRingKey(fecRingIndex) << std::endl ;
    try {
      fecAccess->setFecRingCR0(fecRingIndex, FEC_CR0_RESETOUT | FEC_CR0_RESETRINGB | FEC_CR0_RESETFSMFEC ) ;
      fecAccess->setFecRingCR0(fecRingIndex, 0x0) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Unable to reset the ring: " << e.what() << std::endl ;
    }

    // --------------------------------------------------------------------------------------------
    // FEC slot and ring slot
    outputStream << std::dec << getFecKey(fecRingIndex) << "\t" ;
    outputStream << std::dec << getRingKey(fecRingIndex) << "\t" ;

    // --------------------------------------------------------------------------------------------
    // Read the FEC hardware ID
    try {
      outputStream << fecAccess->getFecHardwareId(fecRingIndex) ;
      outputStream << "\t" ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot read the FEC hardware ID: " << e.what() << std::endl ;
      outputStream << "ERROR:FECHARDID" << "\t" ;
    }
    
    // --------------------------------------------------------------------------------------------
    // Check the mFEC firmware version
    try {
      unsigned int mfecVersion = fecAccess->getFecFirmwareVersion ( fecRingIndex ) ;
      if (mfecVersion != MFECFIRMWARE) 
	outputStream << "ERROR:" << std::hex << mfecVersion ;
      else
	outputStream << std::hex << mfecVersion ;
      outputStream << "\t" ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot read the mFEC firmware version: " << e.what() << std::endl ;
      outputStream << "ERROR:MFECFIRMWARE" << "\t" ;
    }
    
    // --------------------------------------------------------------------------------------------
    // Check the VME firmware version
    try {
      unsigned int vmeVersion = fecAccess->getVmeVersion ( fecRingIndex ) ;
      if (vmeVersion != VMEFIRMWARE) 
	outputStream << "ERROR:" << std::hex << vmeVersion ;
      else
	outputStream << std::hex << vmeVersion ;
      outputStream << "\t" ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot read the VME firmware version: " << e.what() << std::endl ;
      outputStream << "ERROR:VMEFIRMWARE" << "\t" ;
    }
    
    // --------------------------------------------------------------------------------------------
    // Check the trigger firmware version
    try {
      unsigned int triggerVersion = fecAccess->getTriggerVersion ( fecRingIndex ) ;
      if (triggerVersion != TRIGGERFIRMWARE) 
	outputStream << "ERROR:" << std::hex << triggerVersion ;
      else
	outputStream << std::hex << triggerVersion ;
      outputStream << "\t" ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot read the trigger firmware version: " << e.what() << std::endl ;
      outputStream << "ERROR:TRIGGERFIRMWARE" << "\t" ;
    }
    
    // ---------------------------------------------------------------------------------------------
    // Check the SSID
    try {
      std::string ssid = fecAccess->getSSID ( fecRingIndex ) ;
      if (ssid != SSID) 
	outputStream << "ERROR:" << ssid ;
      else
	outputStream << ssid ;
      outputStream << "\t" ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot read the SSID: " << e.what() << std::endl ;
      outputStream << "ERROR:SSID" << "\t" ;
    }

    // ---------------------------------------------------------------------------------------------
    // Check the trigger status register
    try {
      unsigned int triggerStatus0Value = fecAccess->getCCSTriggerStatus0( fecRingIndex ) ;
      if (triggerStatus0Value & CCS_SR0_QPLL_ERROR) {
	outputStream << "ERROR:0x" << std::hex << triggerStatus0Value << "\t" ;
      }
      else if (!(triggerStatus0Value & CCS_SR0_QPLL_LOCKED)) {
	outputStream << "ERROR:0x" << std::hex << triggerStatus0Value << "\t" ;
      }
      else if (!(triggerStatus0Value & CCS_SR0_TTCRX_READY)) {
	outputStream << "ERROR:0x" << std::hex << triggerStatus0Value << "\t" ;
      }
      else outputStream << "0x" << std::hex << triggerStatus0Value << "\t" ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot read the trigger status 0: " << e.what() << std::endl ;
      outputStream << "ERROR:TRIGSR" << "\t" ;
    }
    
    // ----------------------------------------------------------------------------------------------
    // Check the temperature
#define LOWTEMPERATURE 10
#define MAXTEMPERATURE 40
    try {
      int sensor1 = 0, sensor2 = 0 ;
      fecAccess->getFecTemperature ( fecRingIndex, sensor1, sensor2 ) ;
      if ( (sensor1 < LOWTEMPERATURE) || (sensor1 > MAXTEMPERATURE) )
	outputStream << "ERROR:" << std::dec << sensor1 << "\t" ;
      else 
	outputStream << std::dec << sensor1 << "\t" ;
      if ( (sensor2 < LOWTEMPERATURE) || (sensor2 > MAXTEMPERATURE) )
	outputStream << "ERROR:" << std::dec << sensor2 << "\t" ;
      else 
	outputStream << std::dec << sensor2 << "\t" ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot read the SSID: " << e.what() << std::endl ;
      outputStream << "ERROR:TEMP1" << "\t" << "ERROR:TEMP2" << "\t" ;
    }

    // ----------------------------------------------------------------------------------------------
    // Close the loop back and display CR0 and SR0
    tscType32 fecSR0 = 0 ;
    try {
      fecAccess->setFecRingCR0 ( fecRingIndex, FEC_CR0_LOOPBACK ) ;
      tscType32 fecCR0 = fecAccess->getFecRingCR0 ( fecRingIndex ) ;
      unsigned int timeout = 0 ;
      while ( (timeout++ < 1000) && !(fecCR0 & FEC_CR0_LOOPBACK) ) {
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ;
	nanosleep (&req,NULL) ;
	fecCR0 = fecAccess->getFecRingCR0 ( fecRingIndex ) ;
      }

      fecAccess->setFecRingCR0 ( fecRingIndex, FEC_CR0_RESETOUT | FEC_CR0_RESETRINGB | FEC_CR0_RESETFSMFEC | FEC_CR0_LOOPBACK ) ;
      { struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ;	nanosleep (&req,NULL) ; }
      fecAccess->setFecRingCR0 ( fecRingIndex, (FEC_CR0_LOOPBACK | FEC_CR0_ENABLEFEC) ) ;
      fecCR0 = fecAccess->getFecRingCR0 ( fecRingIndex ) ;
      timeout = 0 ;
      while ( (timeout++ < 1000) && (fecCR0 != (FEC_CR0_LOOPBACK | FEC_CR0_ENABLEFEC)) ) {
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ;
	nanosleep (&req,NULL) ;
	fecCR0 = fecAccess->getFecRingCR0 ( fecRingIndex ) ;
      }

      if (fecCR0 == (FEC_CR0_LOOPBACK | FEC_CR0_ENABLEFEC)) 
	outputStream << std::hex << fecCR0 << "\t" ;
      else
	outputStream << "ERROR:" << std::hex << fecCR0 << "\t" ;

      fecSR0 = fecAccess->getFecRingSR0 ( fecRingIndex ) ;
      timeout = 0 ;
      while ( (timeout++ < 1000) && (!isFecSR0Correct(fecSR0)) ) {
	struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ;
	nanosleep (&req,NULL) ;
	fecSR0 = fecAccess->getFecRingSR0 ( fecRingIndex ) ;
      }

      if (isFecSR0Correct(fecSR0)) 
	outputStream << std::hex << fecSR0 << "\t" ;
      else
	outputStream << "ERROR:" << std::hex << fecSR0 << "\t" ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot close the loop back: " << e.what() << std::endl ;
      outputStream << "ERROR:LOOPBACK" << "\t" ;
    }

    // ----------------------------------------------------------------------------------------------
    // Send a frame over the loop back
    try {
      if (!isFecSR0Correct(fecSR0)) outputStream << "NA" << "\t" ;
      else {
	// Send a frame over the ring
	tscType8 frameSent[DD_USER_MAX_MSG_LENGTH*4] = {BROADCAST_ADDRESS,FRAMEFECNUMBER,0x03,0x00,0x01,CMD_CCUREADCRA} ;
	tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = {BROADCAST_ADDRESS,FRAMEFECNUMBER,0x03,0x00,0x01,CMD_CCUREADCRA} ;
	fecAccess->setFecRingCR1 (fecRingIndex,FEC_CR1_CLEARIRQ);
	fecAccess->getFecRingDevice(fecRingIndex)->writeFrame(frame,true) ;

	//tscType8 transactionNumber = frame[4] ;
	//tscType8 statusFrame = frame[6] ;
	if ( (frame[0] != frameSent[0]) || (frame[1] != frameSent[1]) ||
	     (frame[2] != frameSent[2]) || (frame[3] != frameSent[3]) || 
	     (frame[5] != frameSent[5]) || (frame[6] != 0x80) ) {
	  std::cerr << "Incoherence in the frame from the direct ack is not correct is: " << std::endl ;
	  for (int i = 0 ; i < (3+3+1) ; i ++ )
	    std::cerr << "0x" << std::hex << (int)frame[i] << " " ;
	  std::cerr << std::endl ;
	  std::cerr << "and should be (except for the transaction number and status): " << std::endl ;
	  for (int i = 0 ; i < (3+3) ; i ++ )
	    std::cerr << "0x" << std::hex << (int)frameSent[i] << " " ;
	  std::cerr << std::endl; 
	  outputStream << "ERROR:FRAME" << "\t" ;
	}
	else outputStream << "OK" << "\t" ;	

	// Clear the IRQ
	fecAccess->setFecRingCR1 (fecRingIndex, FEC_CR1_CLEARIRQ);
      }
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "Cannot send a frame over the ring: " << e.what() << std::endl ;
      outputStream << "ERROR:FRAME" << "\t" ;
    }

    // ----------------------------------------------------------------------------------------------
    // Disable receive and send block of data to the FIFOs

    // ----------------------------------------------------------------------------------------------
    // std::endl between each ring

    unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
    outputStream << std::dec << (endMillis - startMillis) << std::endl ;
  }    

  outputStream.close() ;

  // ----------------------------------------------------------------------------------------------
  // Test for the loop given the frame over the loop back
  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  int loopBackError[22][8] = {{0}} ;
  for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

    if ((loopI % 100) == 0)
      std::cout << "----------------------- Loop " << loopI << " / " << loop << std::endl ;
    
    // For each ring
    for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
      keyType fecRingIndex = *p ;

      //if (getRingKey(fecRingIndex) == 1) std::cout << "Test all rings on FEC " << std::dec << getFecKey(fecRingIndex) << std::endl ;

      // ----------------------------------------------------------------------------------------------
      // Error ? => try to recover
      if (!isFecSR0Correct(fecAccess->getFecRingSR0(fecRingIndex)) && !loopBackError[getFecKey(fecRingIndex)][getRingKey(fecRingIndex)]) {
	fecAccess->setFecRingCR0 ( fecRingIndex, FEC_CR0_LOOPBACK ) ;
	tscType32 fecCR0 = fecAccess->getFecRingCR0 ( fecRingIndex ) ;
	unsigned int timeout = 0 ;
	while ( (timeout++ < 1000) && !(fecCR0 & FEC_CR0_LOOPBACK) ) {
	  struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ;
	  nanosleep (&req,NULL) ;
	  fecCR0 = fecAccess->getFecRingCR0 ( fecRingIndex ) ;
	}
	fecAccess->setFecRingCR0 ( fecRingIndex, FEC_CR0_RESETOUT | FEC_CR0_RESETRINGB | FEC_CR0_RESETFSMFEC | FEC_CR0_LOOPBACK ) ;
	{ struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ;	nanosleep (&req,NULL) ; }
	fecAccess->setFecRingCR0 ( fecRingIndex, (FEC_CR0_LOOPBACK | FEC_CR0_ENABLEFEC) ) ;
	fecCR0 = fecAccess->getFecRingCR0 ( fecRingIndex ) ;
	timeout = 0 ;
	while ( (timeout++ < 1000) && (fecCR0 != (FEC_CR0_LOOPBACK | FEC_CR0_ENABLEFEC)) ) {
	  struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ;
	  nanosleep (&req,NULL) ;
	  fecCR0 = fecAccess->getFecRingCR0 ( fecRingIndex ) ;
	}
	tscType16 fecSR0 = fecAccess->getFecRingSR0 ( fecRingIndex ) ;
	timeout = 0 ;
	while ( (timeout++ < 1000) && (!isFecSR0Correct(fecSR0)) ) {
	  struct timespec req ; req.tv_sec = 0 ; req.tv_nsec = 10000000 ;
	  nanosleep (&req,NULL) ;
	  fecSR0 = fecAccess->getFecRingSR0 ( fecRingIndex ) ;
	}

	if (!isFecSR0Correct(fecSR0)) {
	  std::cerr << "FATAL: Lost the loop back for the FEC " 
		    << std::dec << getFecKey(fecRingIndex) << "." << getRingKey(fecRingIndex) 
		    << " and cannot recover it" << std::endl ;
	  loopBackError[getFecKey(fecRingIndex)][getRingKey(fecRingIndex)] = 1 ; // stop the test here for that ring
	}
	else
	  std::cerr << "WARNING: Lost the loop back for the FEC " 
		    << std::dec << getFecKey(fecRingIndex) << "." << getRingKey(fecRingIndex) 
		    << ": recovered" << std::endl ;
      }
      // Make the test with the frames
      else {
	// Send a frame over the ring
	tscType8 frameSent[DD_USER_MAX_MSG_LENGTH*4] = {BROADCAST_ADDRESS,FRAMEFECNUMBER,0x03,0x00,0x01,CMD_CCUREADCRA} ;
	tscType8 frame[DD_USER_MAX_MSG_LENGTH*4] = {BROADCAST_ADDRESS,FRAMEFECNUMBER,0x03,0x00,0x01,CMD_CCUREADCRA} ;
	fecAccess->setFecRingCR1 (fecRingIndex,FEC_CR1_CLEARIRQ);
	fecAccess->getFecRingDevice(fecRingIndex)->writeFrame(frame,true) ;

	//tscType8 transactionNumber = frame[4] ;
	//tscType8 statusFrame = frame[6] ;
	if ( (frame[0] != frameSent[0]) || (frame[1] != frameSent[1]) ||
	     (frame[2] != frameSent[2]) || (frame[3] != frameSent[3]) || 
	     (frame[5] != frameSent[5]) || (frame[6] != 0x80) ) {
	  std::cerr << "ERROR: Incoherence in the frame from the direct ack is not correct is: ( " ;
	  for (int i = 0 ; i < (3+3+1) ; i ++ )
	    std::cerr << "0x" << std::hex << (int)frame[i] << " " ;
	  std::cerr << ") and should be (except for the transaction number and status): ( " ;
	  for (int i = 0 ; i < (3+3) ; i ++ )
	    std::cerr << "0x" << std::hex << (int)frameSent[i] << " " ;
	  std::cerr << ")" << std::endl ;
	}

	fecAccess->setFecRingCR1 (fecRingIndex, FEC_CR1_CLEARIRQ);
      }
    }
  }
  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
  std::cout << "Test on FIFOs and loop back took " << std::dec << (endMillis-startMillis) << " ms for " << loop << " times"<< std::endl ;
 
  delete fecSlotList ;
}

/** This method make different tests on the FEC in order
 * <p> command: -fectest
 * The tests are:
 * <ul>
 * <li> Read / write random values into the different FIFO and read back for comparison: not already implemented
 * <li> Test the reset line on the FEC to see if it goes to the CCUs
 * </ul>
 * \param fecAccess - FEC access object
 * \param fecAddress - FEC slot to be reseted
 * \param ringAddress - ring to be reseted
 */
void fecTest1 ( FecAccess *fecAccess,
		tscType8 fecAddress,
		tscType8 ringAddress ) {

  try {
    // Index of the FEC/ring
    keyType indexFecRing = buildFecRingKey ( fecAddress, ringAddress ) ;

    // Reset the ring and check the SR0
    // Make a hard reset
    fecAccess->fecHardReset(indexFecRing) ;
    // Make a soft reset
    fecAccess->fecRingReset(indexFecRing) ;

    // 0010 0000 0000
    //std::cout << "Close the ring " << std::endl ;
    //fecAccess->setFecRingCR0 ( indexFecRing, 0x200 ) ;
    //fecAccess->fecRingReset(indexFecRing) ;
    //std::cout << "CR0 = " << std::hex << fecAccess->getFecRingCR0 ( indexFecRing ) << std::endl ;
    //std::cout << "SR0 = " << std::hex << fecAccess->getFecRingSR0 ( indexFecRing ) << std::endl ;
    //getchar() ;

    // Display it
    std::string strSR0 = FecRingRegisters::decodeFECSR0 ( fecAccess->getFecRingSR0 (indexFecRing) ) ;
    std::string strSR1 = FecRingRegisters::decodeFECSR1 ( fecAccess->getFecRingSR1 (indexFecRing) ) ;
    std::cout << strSR0 << std::endl ;
    std::cout << strSR1 << std::endl ;

    // Test the FIFO
    std::cout << "Do you want to test the FIFO ? (y/n)" ; char c = getchar() ;
    getchar() ;
    if (c != 'n' || c != 'N') {
      fecAccess->getFecRingDevice(indexFecRing)->setFecRingCR0(0) ; // not enable

      bool errorTra = false ;
      unsigned int fifoTra = fecAccess->getFecRingDevice(indexFecRing)->getTransmitFifoDepth() ;
      std::cout << "FIFO transmit test for " << std::dec << fifoTra << " words" << std::endl ;
      std::cout << "Press <enter> to continue" ; getchar() ;
      for (unsigned int i = 1 ; i <= fifoTra ; i ++) {
	fecAccess->getFecRingDevice(indexFecRing)->setFifoTransmit(i) ;
      }
      for (unsigned int i = 1 ; i <= fifoTra ; i ++) {
	tscType32 value = fecAccess->getFecRingDevice(indexFecRing)->getFifoTransmit() ;
	if (value != i) {
	  std::cout << std::dec << i << "\t" << value ;
	  std::cout << " <===" << std::endl ;
	  errorTra = true ;
	}
	//else std::cout << std::endl ;
      }
      bool errorRec = false ;
      unsigned int fifoRec = fecAccess->getFecRingDevice(indexFecRing)->getReceiveFifoDepth() ;
      std::cout << "FIFO Receive test for " << std::dec << fifoRec << " words" << std::endl ;
      std::cout << "Press <enter> to continue" ; getchar() ;
      for (unsigned int i = 1 ; i <= fifoRec ; i ++) {
	fecAccess->getFecRingDevice(indexFecRing)->setFifoReceive(i) ;
      }
      for (unsigned int i = 1 ; i <= fifoRec ; i ++) {
	tscType32 value = fecAccess->getFecRingDevice(indexFecRing)->getFifoReceive() ;
	if (value != i) {
	  std::cout << std::dec << i << "\t" << value ;
	  std::cout << " <===" << std::endl ;
	  errorRec = true ;
	}
	//else std::cout << std::endl ;
      }
      unsigned int fifoRet = fecAccess->getFecRingDevice(indexFecRing)->getReturnFifoDepth() ;
      std::cout << "FIFO Return test for " << std::dec << fifoRet << " words" << std::endl ;
      std::cout << "Press <enter> to continue" ; getchar() ;
      for (unsigned int i = 1 ; i <= fifoRet ; i ++) {
	fecAccess->getFecRingDevice(indexFecRing)->setFifoReceive(i) ;
      }
      bool errorRet = false ;
      for (unsigned int i = 1 ; i <= fifoRet ; i ++) {
	tscType32 value = fecAccess->getFecRingDevice(indexFecRing)->getFifoReceive() ;
	if (value != i) {
	  std::cout << std::dec << i << "\t" << value ;
	  std::cout << " <===" << std::endl ;
	  errorRet = true ;
	}
	//else std::cout << std::endl ;
      }

      // Test the block transfer
      int bltModeTra = 0 ;
      int bltModeRec = 0 ;
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
      if ( (fecAccess->getFecBusType() == FECVME) && (fecAccess->getFecRingDevice(indexFecRing)->getFecFirmwareVersion() >= 0x1500) ) {

	tscType32 *buffer  = new tscType32[fifoRec] ;
	tscType32 *buffer1 = new tscType32[fifoRec] ;

	unsigned int value = 517 ;
	for (unsigned int i = 0 ; i < fifoRec ; i ++) {
	  buffer[i] = value ;
	  buffer1[i] = 0 ;
	  value ++ ;
	  //std::cout << "buffer{" << i << "] = " << buffer[i] << std::endl ;
	}

	if (buffer != NULL && buffer1 != NULL) {
	  
	  bltModeTra = 1 ;
	  bltModeRec = 1 ;

	  FecVmeRingDevice *fecDevice = (FecVmeRingDevice*) fecAccess->getFecRingDevice(indexFecRing); 
	  fecRingDeviceFifoAccessModeEnum storedAccessMode = FECNOBLT ; 
	  if (fecDevice) { 
	    storedAccessMode = fecDevice->getFifoAccessMode() ;
          }
	  if (storedAccessMode != FECDOBLT) fecAccess->setFifoAccessMode(FECDOBLT);

	  std::cout << "Test the block transfer to the FIFO receive for " << fifoRec << " words" << std::endl ;
	  std::cout << "Press <enter> to continue" ; getchar() ;
	  fecAccess->getFecRingDevice(indexFecRing)->setFifoReceive (buffer, (int)fifoRec) ;
	  fecAccess->getFecRingDevice(indexFecRing)->getFifoReceive (buffer1, (int)fifoRec) ;

	  for (unsigned int i = 0 ; i < fifoRec ; i ++) {
	    if (buffer[i] != buffer1[i]) {
	      std::cout << std::dec << buffer[i] << "\t" << buffer1[i] ;
	      std::cout << " <=====" << std::endl ;
	      bltModeRec = -1 ;
	    }
	    //else std::cout << std::dec << buffer[i] << "\t" << buffer1[i] << std::endl ;
	    //else std::cout << std::endl ;
	  }

	  std::cout << "Test the block transfer to the FIFO transmit for " << fifoTra << " words" << std::endl ;
	  std::cout << "Press <enter> to continue" ; getchar() ;
	  fecAccess->getFecRingDevice(indexFecRing)->setFifoTransmit (buffer, (int)fifoTra) ;
	  fecAccess->getFecRingDevice(indexFecRing)->getFifoTransmit (buffer1, (int)fifoTra) ;
	  
	  for (unsigned int i = 0 ; i < fifoTra ; i ++) {
	    if (buffer[i] != buffer1[i]) {
	      std::cout << std::dec << buffer[i] << "\t" << buffer1[i] ;
	      std::cout << " <=====" << std::endl ;
	      bltModeTra = -1 ;
	    }
	    //else std::cout << std::endl ;
	  }
	  fecAccess->setFifoAccessMode(storedAccessMode);
	}
	else
	  std::cout << "Cannot allocate buffer for tests" << std::endl ;

	delete buffer ;
	delete buffer1 ;
      }
#endif

      bool error = false ;
      if (errorTra) {
	std::cerr << "Problem of access in FIFO transmit" << std::endl ;
	error = true ;
      }
      if (errorRec) {
	std::cerr << "Problem of access in FIFO receive" << std::endl ;
	error = true ;
      }
      if (errorRet) {
	std::cerr << "Problem of access in FIFO return" << std::endl ;
	error = true ;
      }
      if (bltModeTra == -1) {
	std::cerr << "Problem of block transfer access in FIFO transmit" << std::endl ;
	error = true ;
      }
      if (bltModeRec == -1) {
	std::cerr << "Problem of block transfer access in FIFO receive" << std::endl ;
	error = true ;
      }

      if (!error) {

	if (bltModeTra == 1 && bltModeRec == 1)
	  std::cout << "Test of the FIFO in mono-word access and block mode access successfully tested" << std::endl ;
	else
	  std::cout << "Test of the FIFO in mono-word access successfully tested" << std::endl ;
      }
      std::cout << "Press <enter> to continue" ; getchar() ;
    }

    // Reset the ring and check the SR0
    // Make a hard reset
    fecAccess->fecHardReset(indexFecRing) ;
    // Make a soft reset
    fecAccess->fecRingReset(indexFecRing) ;
    
    // Display it
    strSR0 = FecRingRegisters::decodeFECSR0 ( fecAccess->getFecRingSR0 (indexFecRing) ) ;
    strSR1 = FecRingRegisters::decodeFECSR1 ( fecAccess->getFecRingSR1 (indexFecRing) ) ;
    std::cout << strSR0 << std::endl ;
    std::cout << strSR1 << std::endl ;
    
    // if the SR0 is correct then try to check if the reset issued by the FEC goes to the CCUs
    if (isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {

      std::list<keyType> *ccuList = NULL ;
      try {
	
	// Scan the ring for each FECs and display the CCUs
	ccuList = fecAccess->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
	if (ccuList != NULL) 
	  clearFecCcuErrorRegisters (fecAccess, fecAddress, ringAddress, *ccuList) ;
	
	if (ccuList != NULL) {
	  
	  for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	    
	    keyType index = *p ;
	    char msg[80] ;
	    decodeKey(msg,index) ;
	    std::cout << msg << " found" << std::endl ;

	    try {
	      // set the CCU CRE to a default value
	      fecAccess->setCcuCRE ( index, 0xADFACD+1 ) ;
	      std::cout << msg << ": CRE set to 0x" << std::hex << fecAccess->getCcuCRE ( index ) << std::endl ;
	    }
	    catch (FecExceptionHandler &e) {
	      
	      std::cerr << "*********** ERROR ********************************" << std::endl ; 
	      std::cerr << "Cannot set a CRE for the CCU " << msg << std::endl ;
	      std::cerr << e.what()  << std::endl ;
	      std::cerr << "**************************************************" << std::endl ;
	    }
	  }

	  // Reset the ring and check the reset issued by the FEC goes to the CCU
	  // Make a soft reset
	  fecAccess->fecRingReset(indexFecRing) ;

	  bool resetOk = true ;
	  for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	
	    keyType index = *p ;
	    char msg[80] ;
	    decodeKey(msg,index) ;
	    
	    try {
	      usleep(1) ;
	      // get the CCU CRE => must be zero if the reset is correctly done
	      tscType32 ccuCRE = fecAccess->getCcuCRE ( index ) ;
	      std::cout << msg << ": CRE = " << std::hex << ccuCRE << std::endl ;
	      if (ccuCRE != 0) {
		std::cerr << "----- ERROR: the reset does not go the CCU " << msg << std::endl ;
		resetOk = false ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      
	      std::cerr << "*********** ERROR ********************************" << std::endl ; 
	      std::cerr << "Cannot get a CRE for the CCU " << msg << std::endl ;
	      std::cerr << e.what()  << std::endl ;
	      std::cerr << "**************************************************" << std::endl ;
	    }
	  }

	  if (!resetOk) {
	    std::cerr << "One of the CCU does not receive the reset" << std::endl ;
	  }
	  else {
	    std::cout << "The FEC reset is correctly issued to the CCUs" << std::endl ;
	  }
	}
	else {
	  std::cout << "No CCU found on FECs" << std::endl ;
	}  
      }
      catch (FecExceptionHandler &e) {
	
	std::cerr << "*********** ERROR ********************************" << std::endl ; 
	std::cerr << "An error occurs during hardware access" << std::endl ;
	std::cerr << e.what()  << std::endl ;
	std::cerr << "**************************************************" << std::endl ;
      }

      // Delete = already store in the table of FEC device class
      delete ccuList ;
    }
  }
  catch (FecExceptionHandler &e) {
      
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  if (! noGetchar_) {
    std::cout << "Press <enter> to continue" << std::endl ;
    getchar() ;
  }
}

/**
 * this method create the FEC plug and play file
 */
void createPlugAndPlayFile ( FecAccess *fecAccess, std::string pnpFileName ) {

  std::ofstream fichier ( pnpFileName.c_str() ) ;
  if (!fichier) {
    std::cerr << "Cannot open the file " << pnpFileName << ", cannot upload the file" << std::endl ;
    return ;
  }

  time_t coucou = time(NULL);
  fichier << "********************************************************" << std::endl 
	  << "*  serialNumber       typeId      baseAddress(hex)" << std::endl
	  << "*  update on " << ctime(&coucou) 
	  << "********************************************************" << std::endl ;

  // list of rings
  std::list<keyType> *fecList = fecAccess->getFecList ( ) ;

  // loop on the list of rings
  int fecAdded[MAX_NUMBER_OF_SLOTS+1] = {0} ;
  unsigned int fecNumber = 0 ;
  for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
    try {
      if (!fecAdded[getFecKey(*it)]) {
	fecAccess->getFecHardwareId(*it) ;
	fichier << fecAccess->getFecHardwareId(*it) << "       " << "FEC-CCS" << "      " << "00000000" << std::endl ;
	fecAdded[getFecKey(*it)] = 1 ;
	fecNumber ++ ;
      }
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "No FEC hardware ID on FEC " << getFecKey(*it) << std::endl ;
    }
  }
    
  fichier << "********************************************************" << std::endl ;
  fichier.close();

  std::cout << "Upload done in file " << pnpFileName << " for " << fecNumber << " FECs" << std::endl ;
}

/**
 * This method reset the crate
 * <p>command: -crateReset
 * <p>command: -testCrateReset
 * \param fecAccess - FEC access object
 */
void crateReset ( FecAccess *fecAccess, bool testCrateReset,
		  long loop, unsigned long tms ) {

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  // -----------------------------------------------------------------------------------
  // crate reset + reload the mFEC firmware
  try {
    if (fecAccess->getFecBusType() == FECVME) {
      fecAccess->crateReset() ;
    }
    else {
      std::cerr << "The crate reset is not available for other FEC than VME FEC" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  // -----------------------------------------------------------------------------------
  // reset the crate then test the crate reset to see if firmware are correctly reloaded
  if (testCrateReset) { 

    std::ofstream fichier ( "/tmp/FecTestCrateReset.txt", std::ios_base::app ) ;
    if (!fichier) {
      std::cerr << "Cannot open the file /tmp/FecTestCrateReset.txt, cannot run the test" << std::endl ;
      return ;
    }
      
    time_t coucou = time(NULL) ;
    fichier << std::endl << std::endl << "--------------------------------------------- test started on " << ctime(&coucou) ;
    fichier.flush() ;

    std::list<keyType> *listFecK = fecAccess->getFecList() ;
    std::list<keyType> listFecKey ;
    for (std::list<keyType>::iterator it = listFecK->begin() ; it != listFecK->end() ; it ++) {
      try {
	FecVmeRingDevice *fecVmeRing = (FecVmeRingDevice *)fecAccess->getFecRingDevice(*it) ;
	fecVmeRing->getVmeVersion () ;
	fecVmeRing->getFecRingSR0() ;
	listFecKey.push_back(*it) ;
      }
      catch (FecExceptionHandler &e) {
	try {
	  fichier << "Strange, the ring " << getFecKey(*it) << "." << getRingKey(*it) << " was existing before any crate reset and does exists anymore" ;
	  fichier.flush() ;
	  fecAccess->fecHardReset(*it,0x1) ;
	  fecAccess->getFecRingSR0(*it) ;
	  fichier << ": recovered" << std::endl ;
	}
	catch (FecExceptionHandler &e) { 
	  fichier << ": still in error, exiting" << std::endl ;
	  return ;
	}
      }
    }
    delete listFecK ;

    if (!listFecKey.size()) {
      coucou = time(NULL) ;
      fichier << "no FECs on that crate, stopping the test on " << ctime(&coucou) ;
      fichier.close() ;
    }
    else {
#define LOOPIDISPLAY 10000
      try {
	for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

	  //std::cout << loopI << ": " << (loopI >= 100) << " " << ((loopI % 100)==0) << std::endl ;
	  if ( (loopI >= LOOPIDISPLAY) && ((loopI % LOOPIDISPLAY) == 0) ) {
	    coucou = time(NULL) ;
	    fichier << "| " << loopI << " times | " << ctime(&coucou) ;
	    fichier.flush() ;
	  }

	  // crate reset withtout reload the firmware
	  FecVmeRingDevice::crateReset ( );
	  
	  for (std::list<keyType>::iterator it = listFecKey.begin() ; it != listFecKey.end() ; it ++) {
	    try {
	      FecVmeRingDevice *fecVmeRing = (FecVmeRingDevice *)fecAccess->getFecRingDevice(*it) ;
	      if ( (loopI >= LOOPIDISPLAY) && ((loopI % LOOPIDISPLAY) == 0) ) {
		std::cout << loopI << " times" << " on FEC " << getFecKey(*it) << " ring " << getRingKey(*it) ;
		fichier << "| " << loopI << " times | " << "FEC | " << getFecKey(*it) << " | Ring | " << getRingKey(*it) ;
	      }

	      try {
		if ( (loopI >= LOOPIDISPLAY) && ((loopI % LOOPIDISPLAY) == 0) ) {
		  std::cout << ", VME firmware version = " << fecVmeRing->getVmeVersion( ) ;
		  fichier <<  " | VME firmware version | " << fecVmeRing->getVmeVersion( ) ;
		}
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << std::endl ;
		coucou = time(NULL) ;
		fichier << "ERROR on reading the VME firmware on " << getFecKey(*it) ;
		fichier.flush() ;
	      }
	      
	      try {
		if ( (loopI >= LOOPIDISPLAY) && ((loopI % LOOPIDISPLAY) == 0) ) {
		  std::cout << ", SR0 = 0x" << std::hex << fecVmeRing->getFecRingSR0() << std::dec << std::endl ;
		  fichier <<  " | SR0 | 0x" << std::hex << fecVmeRing->getFecRingSR0() << std::dec << " | " ;
		}
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << std::endl ;
		fichier << "ERROR on reading the SR0 on ring " << getFecKey(*it) << "." << getRingKey(*it) << std::endl ;
		fichier.flush() ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      fichier << "No FEC found on " << getFecKey(*it) << std::endl ;
	      fichier.flush() ;
	    }

	    if ( (loopI >= LOOPIDISPLAY) && ((loopI % LOOPIDISPLAY) == 0) ) {
	      fichier << std::endl ;
	    }
	  }

	  // Wait
	  if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
	}
      }
      catch (FecExceptionHandler &e) {
	
	std::cerr << "*********** ERROR ********************************" << std::endl ; 
	std::cerr << "An error occurs during hardware access" << std::endl ;
	std::cerr << e.what()  << std::endl ;
	std::cerr << "**************************************************" << std::endl ;

	coucou = time(NULL) ;
	fichier << "Problem during the crate reset: " << e.what() << ", stopping the test on " << ctime(&coucou) ;
	fichier.close() ;
      }
    }

    coucou = time(NULL) ;
    fichier << "Number of loops done = " << loop << ", test stop at " << ctime(&coucou) ;
    fichier.close() ;
  }
#endif
}

/**
 * This method toggle the bit to reload the firmware from the VME FPGA embedded on the board
 * and the mFEC FPGA
 * <p>command -uFecReload
 */
void uFecReload( FecAccess *fecAccess,
		 tscType8 fecAddress ) {

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  if (fecAccess->getFecBusType() != FECVME) {
    std::cerr << "Only VME FEC can have its firmwares reloaded" << std::endl ;
    return ;
  }

  try {
    // Build the index for the given ring
    keyType indexFec = buildFecRingKey(fecAddress,0) ;

    // toggle all bits
    std::cout << "Reload the firmware and wait 2 secondes for a complete reload" << std::endl ;
    fecAccess->fecHardReset(indexFec,0x1) ;
    sleep(2) ;

    // Check the SR0 and display it
    std::list<keyType> *fecSlotList = fecAccess->getFecList( ) ;
    if ((fecSlotList != NULL) && (!fecSlotList->empty())) {
      for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
	keyType index = *p ;
	if (getFecKey(index) == getFecKey(indexFec)) {
	  FecRingDevice *fecDevice = fecAccess->getFecRingDevice (index) ;
	  std::cout << "FEC HARD ID\tFEC\t\tmFEC version\tVME Version\tSR0" << std::endl ;
	  try {
	    std::cout << ((FecVmeRingDevice *)fecDevice)->getFecHardwareId() << "\t" ;
	    std::cout << "FEC " << std::dec << getFecKey(index) << "." << getRingKey(index) << "\t\t" ;
	    tscType16 version = fecDevice->getFecFirmwareVersion() ;
	    std::cout << std::hex << std::setw(2) << std::setfill('0') << version << "\t\t" ;
	    if (version >= MFECFIRMWARE) {
	      tscType16 vmeVersion = ((FecVmeRingDevice *)fecDevice)->getVmeVersion() ;
	      std::cout << std::hex << std::setw(2) << std::setfill('0') << vmeVersion << "\t\t" ;
	    }
	  }
	  catch (FecExceptionHandler &e) {
	    std::cout << "ERROR in accessing the FEC or the ring: " << e.what() << std::endl ; ;
	  }
	}
      }
    }
    delete fecSlotList ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
#endif
}

/**
 * This method try to find all the device driver loaded for FECs and reset all PLX 
 * and FECs
 * <p>command: -reset
 * \param fecAccess - FEC access object
 * \param fecAddress - FEC slot to be reseted
 * \param ringAddress - ring to be reseted
 * \warning the loop is used only for the FEC reset it self
 */
void resetPlxFec ( FecAccess *fecAccess,
		   tscType8 fecAddress,
		   tscType8 ringAddress,
		   long loop, unsigned long tms ) {

  try {

    // Build the index for the given ring
    keyType index = buildFecRingKey(fecAddress,ringAddress) ;

    // Make a reset
    //fecAccess->fecHardReset(index) ;

#ifdef TTCRx
    fecAccess->initTTCRx(index) ;
#endif

    // Wait for any alarms
    for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

      //if (fecSlotList != NULL) {
      //for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
      //keyType index = *p ;

      // Ring A
      fecAccess->fecRingReset(index) ;
      // Ring B
      fecAccess->fecRingResetB(index) ;
      // Reset the FSM machine
      fecAccess->fecRingResetFSM(index) ;
        
      tscType16 fecSR0 = fecAccess->getFecRingSR0(index) ;
      std::cout << "Value of the Status Register 0 of the FEC " << (int)getFecKey(index) << ", ring " << (int)getRingKey(index) << ": 0x" << std::hex << (int)fecSR0 << std::endl ;
      if (fecSR0 & 0x1)
	std::cout << "\tFIFO transmit running" << std::endl ;
      if (fecSR0 & 0x2)
	std::cout << "\tFIFO receive running" << std::endl ;
      if (fecSR0 & 0x4)
	std::cout << "\tFIFO receive half full" << std::endl ;
      if (fecSR0 & 0x8)
	std::cout << "\tFIFO receive full" << std::endl ;
      if (fecSR0 & 0x10)
	std::cout << "\tFIFO receive empty" << std::endl ;
      if (fecSR0 & 0x20)
	std::cout << "\tFIFO return half full" << std::endl ;
      if (fecSR0 & 0x40)
	std::cout << "\tFIFO return full" << std::endl ;
      if (fecSR0 & 0x80)
	std::cout << "\tFIFO return empty" << std::endl ;
      if (fecSR0 & 0x100)
	std::cout << "\tFIFO transmit half full" << std::endl ;
      if (fecSR0 & 0x200)
	std::cout << "\tFIFO transmit full" << std::endl ;
      if (fecSR0 & 0x400)
	std::cout << "\tFIFO transmit empty" << std::endl ;
      if (fecSR0 & 0x800)
	std::cout << "\tLink initialise" << std::endl ;
      if (fecSR0 & 0x1000) 
	std::cout << "\tPending irg" << std::endl ;
      if (fecSR0 & 0x2000)
	std::cout << "\tData to FEC" << std::endl ;
      if (fecSR0 & 0x4000)
	std::cout << "\tTTCRx ok" << std::endl ;

      // Wait
      if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}

/**
 * This method try to find all the device driver loaded for FECs and reset all PLX 
 * and FECs
 * <p>command: -resetB
 * \param fecAccess - FEC access object
 * \warning the loop is not used
 */
void resetRingB ( FecAccess *fecAccess,
		  long loop, unsigned long tms ) {

  try {

    // Scan the PCI slot for device driver loaded
    std::list<keyType> *fecSlotList = fecAccess->getFecList() ;
    
    // Make a reset
    // fecAccess->fecHardReset() ;
    
    //#ifdef TTCRx
    //fecAccess->initTTCRx() ;
    //#endif

    // Wait for any alarms
    for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
      
      if (fecSlotList != NULL) {
	for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
	  
	  keyType index = *p ;

	  std::cout << loopI << ": Reset on ring B ..." << std::endl ;
	  fecAccess->fecRingResetB(index) ;
	}
      }

      // Wait
      if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
    }
    
    // Delete = already store in the table of FEC Access class
    delete fecSlotList ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}

/**
 * This method try to find all the device driver loaded for FECs and reset all PLX 
 * and FECs
 * <p>command: -resetA
 * \param fecAccess - FEC access object
 * \warning the loop is not used
 */
void resetRingA ( FecAccess *fecAccess,
		  long loop, unsigned long tms ) {

  try {

    // Scan the PCI slot for device driver loaded
    std::list<keyType> *fecSlotList = fecAccess->getFecList() ;
    
    // Make a reset
    // fecAccess->fecHardReset() ;
    
    //#ifdef TTCRx
    //fecAccess->initTTCRx() ;
    //#endif

    // Wait for any alarms
    for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
      
      if (fecSlotList != NULL) {
	for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
	  
	  keyType index = *p ;

	  std::cout << loopI << ": Reset on ring A ..." << std::endl ;
	  fecAccess->fecRingReset(index) ;
	}
      }

      // Wait
      if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
    }
    
    // Delete = already store in the table of FEC Access class
    delete fecSlotList ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}

/**
 * This method is to find all the device driver loaded for FECs  
 * and toggle receiver clock polarity
 * <p>command: -invrecvclk
 * \param fecAccess - FEC access object
 * 
 */
void toggleReceiverClockPolarity ( FecAccess *fecAccess,
				   long loop, unsigned long tms ) {

  try {
    
    // Scan the PCI slot for device driver loaded
    std::list<keyType> *fecSlotList = fecAccess->getFecList() ;
    
    for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
      
      if (fecSlotList != NULL) {
	for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
	  
	  keyType index = *p ;
	  
	  std::cout << loopI << ": toggle  recv. clock ..." << std::endl ;
	  fecAccess->setFecRingCR0(index,FEC_CR0_DISABLERECEIVE,CMD_OR) ;
	  //tscType16 vcr0 = fecAccess->getFecRingCR0(index) ;
	  //if (vcr0 & FEC_CR0_POLARITY) 
	  //fecAccess->setInvertClockPolarity(index,false);
	  //else 
	  fecAccess->setInvertClockPolarity(index,true);  
	  fecAccess->setFecRingCR0(index,FEC_CR0_DISABLERECEIVE,CMD_XOR) ;
	}
      }

      // Wait
      if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
    }
    
    // Delete = already store in the table of FEC Access class
    delete fecSlotList ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}

/** reset TTCRx in CCS
 * <p>command: -resetCCSTTCRx
 */
void commandResetCCSTTCRx ( FecAccess *fecAccess, 
			    tscType8 fecAddress,
			    std::string ssid,
			    long loop, 
			    unsigned long tms ) {

  try {
    setSSID ( fecAccess, ssid, false, false ) ;
    
    keyType index = buildFecRingKey(fecAddress,1) ;
    //std::list<keyType> *fecSlotList = fecAccess->getFecList() ;
    
    fecAccess->getCCSTrigger(index)->setResetTTCRx() ; 
    int loopI = 0 ;
    while ( (loopI < loop) && (fecAccess->getCCSTriggerStatus0(index) != (CCS_SR0_QPLL_LOCKED | CCS_SR0_TTCRX_READY)) ) { 
      
      usleep(tms) ;
      
      std::cout << "Try " << std::dec << loopI << std::hex << "value = 0x" << fecAccess->getCCSTriggerStatus0(index) << std::endl ;
    }

    std::cout << "End of ResetCCSTTCRx: " << std::hex << "value = 0x" << fecAccess->getCCSTriggerStatus0(index) << std::endl ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
  }
}


/** Display the FEC version
 * <p>command: -fecver
 * <p>command: -fectrigger
 */
void displayFecVersion( FecAccess *fecAccess, 
			long loop, 
			unsigned long tms,
			bool trigger,
			std::string ssid,
			bool resetCCSTTCRx ) {
  try {

    // Scan the PCI slot for device driver loaded
    std::list<keyType> *fecSlotList = fecAccess->getFecList() ;

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
    if (fecAccess->getFecBusType() == FECVME) {

      if (trigger) {
	std::cout << "Set the SSID of all FECs to " << ssid << std::endl ;
	setSSID ( fecAccess, ssid, false, resetCCSTTCRx ) ;
      }

      std::cout << "FEC HARD ID\tFEC\t\tmFEC version\tVME Version\tTrigger Version\tSR0" ;
      if (trigger)
	std::cout << "\tSSID\tCCS Status\tCCS Control" << std::endl ;
      else
	std::cout << std::endl ;
    }
    else
#endif
      std::cout << "FEC\t\tmFEC version\tSR0" << std::endl ;

    for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
      
      if (fecSlotList != NULL) {
	for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {

	  keyType index = *p ;
	  FecRingDevice *fecDevice = fecAccess->getFecRingDevice(index); 

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
	  if (fecAccess->getFecBusType() == FECVME) {
	    try {
	      std::cout << ((FecVmeRingDevice *)fecDevice)->getFecHardwareId() << "\t" ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "ERROR" << "\t\t" ;
	    }
	  }
#endif

	  if (fecDevice) {
	    if (getFecKey(index) < 10)
	      std::cout << "FEC " << std::dec << getFecKey(index) << "." << getRingKey(index) << "\t\t" ;
	    else
	      std::cout << "FEC " << std::dec << getFecKey(index) << "." << getRingKey(index) << "\t" ;
	    tscType16 version = 0 ;
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
	    try {
	      version = fecDevice->getFecFirmwareVersion() ; 
	      //std::cout << "idx: 0x" << std::hex << std::setw(8) << std::setfill('0') << index << " ver: " ; 
	      std::cout << std::hex << std::setw(2) << std::setfill('0') << version << "\t\t" ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "ERROR" << "\t\t" ;
	    }

	    if ( (version >= MFECFIRMWARE) && (fecAccess->getFecBusType() == FECVME) ) {
	      try {
		tscType16 vmeVersion = ((FecVmeRingDevice *)fecDevice)->getVmeVersion() ;
		std::cout << std::hex << std::setw(2) << std::setfill('0') << vmeVersion << "\t\t" ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "ERROR" << "\t\t" ;
	      }
	    }
	    else std::cout << "NA\t\t" ;

	    if ( (version >= MFECFIRMWARE) && (fecAccess->getFecBusType() == FECVME) ) {
	      try {
		tscType16 triggerVersion = ((FecVmeRingDevice *)fecDevice)->getTriggerVersion() ;
		std::cout << std::hex << std::setw(2) << std::setfill('0') << triggerVersion << "\t\t" ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "ERROR" << "\t\t" ;
	      }
	    }
	    else std::cout << "NA\t\t" ;
#endif

	    try {
	      tscType16 fecSR0 = fecDevice->getFecRingSR0() ;
	      std::cout << std::hex << std::setw(2) << std::setfill('0') << fecSR0 << "\t" ;
	      if (fecSR0 & FEC_SR0_LINKINITIALIZED && !trigger) {
		if (isFecSR0Correct(fecSR0)) std::cout << "<<<" ;
		else {
		  bool closedRing = true ;
		  int timeout = 0 ;
		  while ((timeout < 10) && closedRing) {
		    fecDevice->fecRingReset() ;
		    fecSR0 = fecDevice->getFecRingSR0() ;
		    if (!(fecSR0 & FEC_SR0_LINKINITIALIZED)) closedRing = false ;
		    else timeout ++ ;
		  }
		  if (closedRing) std::cout << "< " ;
		}
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "ERROR" << "\t" ;
	    }

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
	    if ((fecAccess->getFecBusType() == FECVME) && trigger) {
	      try {
		std::cout << fecAccess->getSSID(index) << "\t\t" ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "ERROR" << "\t" ;
		//std::cout << e.what() << std::endl;
	      }
	      try {
		std::cout << std::hex << fecAccess->getCCSTriggerStatus0(index) << "\t" ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "ERROR" << "\t" ;
		//std::cout << e.what() << std::endl;
	      }
	      try {
		std::cout << std::hex << fecAccess->getCCSTriggerConfig0(index) << "\t" ;
	      }
	      catch (FecExceptionHandler &e) {
		std::cout << "ERROR" << "\t" ;
		//std::cout << e.what() << std::endl;
	      }
	    }
#endif

	    std::cout << std::endl ;
	  } 
	}
      }

      // Wait
      if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
    }

    // Delete = already store in the table of FEC Access class
    delete fecSlotList ;
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
} 

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       Temperature of the FEC display                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** Display the FEC temperature for the two sensors
 * <p>command: -fecTemp
 * \param fecAccess - FEC access object
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time between two loops
 */
void fecTempTest ( FecAccess *fecAccess, unsigned int fecAddress,
		   long loop, unsigned long tms ) {

#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
  if (fecAccess->getFecBusType() == FECVME) {

    std::list<keyType> fecSlotReady ;
    if (fecAddress == 0xFFFFFFFF) {

      std::list<keyType> *fecSlotList = fecAccess->getFecList( ) ;
      if ((fecSlotList != NULL) && (!fecSlotList->empty())) {
	for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
	  keyType index = *p ;

	  if (getRingKey(index) == FecVmeRingDevice::getMinVmeFecRingValue()) 
	    fecSlotReady.push_back(index) ; //setFecKey(getFecKey(index))) ;
	}
      }
      delete fecSlotList ;
    }
    else fecSlotReady.push_back(buildFecRingKey(fecAddress,FecVmeRingDevice::getMinVmeFecRingValue())) ;

    int tempSensor[22][2] = {{0},{0}} ;
    long numberOfLoopBeforeNextLogin = 1 ;

    if (tms != 0) numberOfLoopBeforeNextLogin = (long)(3600/(tms/1000000)) ;
    std::ostringstream fileName ; //fileName << "/tmp/LogFecTempature_" << time(NULL) << ".txt" ;
    fileName << "/tmp/LogFecTempature.txt" ;
    bool logFile = true ;

    std::ofstream tempFecStream(fileName.str().c_str(), std::ios_base::out | std::ios_base::app) ;
    if (loop == 1) tempFecStream.close() ;
    else std::cout << "Send the output to the file " << fileName.str() << std::endl ;

    for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {

      try {

	// Log values ?
	if ((loopI % numberOfLoopBeforeNextLogin) == 0) {
//	  tempFecStream << loopI << "%" << numberOfLoopBeforeNextLogin << " = " << (loopI % numberOfLoopBeforeNextLogin) << std::endl ;
	  logFile = true ;
	}

	for (std::list<keyType>::iterator p=fecSlotReady.begin() ; p!=fecSlotReady.end() ; p++) {

	  keyType index = *p ;
	  int tempSensor1 = 0, tempSensor2 = 0 ;
	  fecAccess->getFecTemperature (index, tempSensor1, tempSensor2) ;
	  
	  std::cout << "FEC on slot " << std::dec << getFecKey(index) << std::endl ;
	  std::cout << "\tSensor 1 temperature = " << tempSensor1 ;
	  if (tempSensor[getFecKey(index)][0] > 0) std::cout << "/" << tempSensor[getFecKey(index)][0] << std::endl ;
	  else std::cout << std::endl ;
	  std::cout << "\tSensor 2 temperature = " << tempSensor2 ;
	  if (tempSensor[getFecKey(index)][0] > 0) std::cout << "/" << tempSensor[getFecKey(index)][1] << std::endl ;
	  else std::cout << std::endl ;
	  // If the new temperature is different from the previous one or if you read it 
	  // without a log since one hour then log the file
	  int t1 = tempSensor[getFecKey(index)][0] - tempSensor1 ; if (t1 < 0) t1 = -t1 ;
	  int t2 = tempSensor[getFecKey(index)][1] - tempSensor2 ; if (t2 < 0) t2 = -t2 ;
	  if ( (t1 >= 2) || (t2 >= 2) ) logFile = true ;
// 	  if ( (t1 >= 2) || (t2 >= 2) ) {
// 	    if (t1 >= 1) tempFecStream << "Log in file for variation " << tempSensor1 << "/" << tempSensor[getFecKey(index)][0] << std::endl ;
// 	    if (t2 >= 1) tempFecStream << "Log in file for variation " << tempSensor2 << "/" << tempSensor[getFecKey(index)][1] << std::endl ;
// 	  }

	  // Remind the values
	  tempSensor[getFecKey(index)][0] = tempSensor1 ;
	  tempSensor[getFecKey(index)][1] = tempSensor2 ;
	}
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "*********** ERROR ********************************" << std::endl ; 
	std::cerr << "An error occurs during hardware access" << std::endl ;
	std::cerr << e.what()  << std::endl ;
	std::cerr << "**************************************************" << std::endl ;

	tempSensor[getFecKey(e.getHardPosition())][0] = -1 ;
	tempSensor[getFecKey(e.getHardPosition())][1] = -1 ;

	if (tempFecStream.is_open())
	  tempFecStream << time(NULL) << ":" << std::dec << (int)getFecKey(e.getHardPosition()) << ":" << "HARDWAREACCESSPROBLEM" << std::endl ;
      }

      // Log in the output
      if (logFile && tempFecStream.is_open()) {
	for (int i = 2 ; i < 22 ; i ++) {
	  if (tempSensor[i][0] != 0) {
	    if (tempSensor[i][0] == -1)
	      tempFecStream << time(NULL) << ":" << std::dec << (int)i << ":" << "HARDWAREACCESSPROBLEM:HARDWAREACCESSPROBLEM" << std::endl ;
	    else
	      tempFecStream << time(NULL) << ":" << std::dec << (int)i << ":" << tempSensor[i][1] << ":" << tempSensor[i][0] << std::endl ;
	  }
	}
	logFile = false ;
      }

      // Wait
      if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
    }
  }
  else {
    std::cerr << "Temperature display is only available for VME FEC" << std::endl ;
  }
#endif
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       default test                                      */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/**
 * <p>command: -spuriousreset
 * This method wait until a bad SR0 but it try a FEC reset, PLX reset
 * \param fecAccess - FEC access object
 * \param fecAddress - FEC address
 * \param ringAddress - ring address
 */
int checkSpuriousReset ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress ) {

  
  FecRingDevice *fecDevice = NULL;
  unsigned int receiveFifoDepth = 0 ;
  unsigned int transmitFifoDepth = 0 ;
  unsigned int returnFifoDepth = 0 ;
  
  try {
    
    //fecAccess->setFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
    
    // Retreive the corresponding FecRingDevice
    fecDevice = fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;
    
    // Retreive the size of the FIFO depth
    receiveFifoDepth = fecAccess->getReceiveFifoDepth(buildFecRingKey(fecAddress,ringAddress)) ;
    transmitFifoDepth = fecAccess->getTransmitFifoDepth(buildFecRingKey(fecAddress,ringAddress)) ;
    returnFifoDepth = fecAccess->getReturnFifoDepth(buildFecRingKey(fecAddress,ringAddress)) ;

    // Not documented
    fecDevice->setIRQ (false) ;

    std::cout << "Using the ring " << (int)ringAddress << std::endl ;

    // Wait for ever or a bad SR0
    bool flag = true ;
    // Get the CCU list for SRA
    std::list<keyType> *ccuList = NULL ;

    try {
      while (flag) {

	if (ccuList == NULL) {
	  // Scan the ring for each FECs and display the CCUs
	  //for (unsigned int ringAddress = 0 ; ringAddress < 8 ; ringAddress ++) {
	  {
	     std::list<keyType> *ccu1 = fecAccess->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
	     if ((ccu1 != NULL) && (ccu1->size() > 0)) {
	       if (ccuList == NULL) ccuList = new std::list<keyType> ;
	       for (std::list<keyType>::iterator it = ccu1->begin() ; it != ccu1->end() ; it ++) {
		 ccuList->push_back((*it)) ;
	       }
	       std::cout << "Found " << std::dec << ccu1->size() << " CCUs on ring " << ringAddress << std::endl ;
	     }
	  }
	  if ((ccuList != NULL) && (ccuList->size() > 0)) {
	    for (std::list<keyType>::iterator it = ccuList->begin() ; it != ccuList->end() ; it ++) {
	      keyType index = *it ;
	      fecAccess->setCcuCRE(index, 0xADFACE) ;
	      std::cout << "Ring " << std::dec << getRingKey(index) << " CCU 0x" << std::hex << getCcuKey(index) << " CRE = 0x" <<  fecAccess->getCcuCRE(index) << std::endl ;
	    }
	  }
	  else {
	    std::cerr << "No CCUs on the ring => exiting" << std::endl ;
	    return -1;
	  }
	}
	else {

	  if (!noGetchar_) {
	    std::cout << "Press enter when you want to read back the CCU registers" ; getchar() ;
	  }
	  bool error = false ;
	  if ((ccuList != NULL) && (ccuList->size() > 0)) {
	    for (std::list<keyType>::iterator it = ccuList->begin() ; it != ccuList->end() ; it ++) {
	      keyType index = *it ;
	      if (fecAccess->getCcuCRE(index) != 0xADFACE) {
		std::cout << "Error on CRE must be 0xADFACE: is 0x" << std::hex << fecAccess->getCcuCRE(index) << std::endl ;
		fecAccess->setCcuCRE(index, 0xADFACE) ;
		std::cout << "--------------------------------------------------------> Error: Ring " << std::dec << getRingKey(index) << " CCU 0x" << std::hex << getCcuKey(index) << " CRE = 0x" <<  fecAccess->getCcuCRE(index) << std::endl ;
		error = true ;
	      }
	      else std::cout << std::hex << "====> CCU 0x" << getCcuKey(index) << " CRE = 0x" <<  fecAccess->getCcuCRE(index) << std::endl ;
	    }

	    if (error) { std::cout << "One the CCU has been reseted" << std::endl ; getchar() ; }
	  }
	}
      }
    }
    catch (FecExceptionHandler &e) {
      
      std::cerr << "------------ Exception ----------" << std::endl ;
      std::cerr << e.what()  << std::endl ;
      std::cerr << "---------------------------------" << std::endl ;
      return -1;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }

  // Not documented
  if (fecDevice != NULL) fecDevice->setIRQ (true) ;

  return 0 ;
}

/**
 * <p>command: -parasitic
 * This method wait until a bad SR0 but it try a FEC reset, PLX reset
 * \param fecAccess - FEC access object
 * \param fecAddress - FEC address
 * \param ringAddress - ring address
 */
int checkParasiticFrame ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress ) {

  
  FecRingDevice *fecDevice = NULL;
  unsigned int receiveFifoDepth = 0 ;
  unsigned int transmitFifoDepth = 0 ;
  unsigned int returnFifoDepth = 0 ;

  try {

    //fecAccess->setFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;

    // Retreive the corresponding FecRingDevice
    fecDevice = fecAccess->getFecRingDevice (buildFecRingKey(fecAddress,ringAddress)) ;

    // Retreive the size of the FIFO depth
    receiveFifoDepth = fecAccess->getReceiveFifoDepth(buildFecRingKey(fecAddress,ringAddress)) ;
    transmitFifoDepth = fecAccess->getTransmitFifoDepth(buildFecRingKey(fecAddress,ringAddress)) ;
    returnFifoDepth = fecAccess->getReturnFifoDepth(buildFecRingKey(fecAddress,ringAddress)) ;

    // Not documented
    fecDevice->setIRQ (false) ;

    // Wait for ever or a bad SR0
    bool flag = true ;
    // Get the CCU list for SRA
    std::list<keyType> *ccuList = NULL ;
    // Scan the ring for each FECs and display the CCUs
    ccuList = fecAccess->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
    if (ccuList->size() == 0) { 
      std::cerr << "No CCUs on the ring => exiting" << std::endl ;
      delete ccuList ; ccuList = NULL ; 
    }

    while (flag) {

      // Get the SR0
      tscType16 fecSR0 = fecDevice->getFecRingSR0() ;
      
      if (! isFecSR0Correct(fecSR0)) {
        
        std::cout << "The FEC status register 0 is not correct 0x" << std::hex << fecSR0 << std::endl ;
        
        // Check what is wrong
        if (! (fecSR0 & DD_FEC_LINK_INITIALIZED)) {
          
          std::cout << "The ring is opened" << std::endl ;
        }
        else {
          
          // Check the FIFO receive
          if (! (fecSR0 & DD_FEC_REC_EMPTY)) {
            
            std::cout << "The fifo receive is not empty" << std::endl ;
            
            try {
              unsigned int i = 0 ;
	      while ((!((fecSR0=fecDevice->getFecRingSR0()) & DD_FEC_REC_EMPTY) || 
		      (fecSR0 & DD_FEC_REC_RUN)) && (i < receiveFifoDepth) ) {
		time_t timestamp = time(NULL) ;
		std::cerr << "Time: " << ctime (&timestamp) << std::endl ;
		std::cout << "FIFO Receive Value " << std::dec << (i+1) << " = 0x" << std::hex << fecDevice->getFifoReceive() << std::endl ;

                i ++ ;
              }

	      tscType16 fecSR1 =  fecDevice->getFecRingSR1() ;
	      std::cout << "SR1 = 0x" << std::hex << fecSR1 << std::endl ;
	      if (fecSR1 & FEC_SR1_ILLDATA) 
		std::cerr << "Illegal data detected by the FEC" << std::endl ;
	      if (fecSR1 & FEC_SR1_ILLSEQ)
		std::cerr << "Illegal sequence detected by the FEC" << std::endl ;
	      if (fecSR1 & FEC_SR1_CRCERROR)
		std::cerr << "CRC error" << std::endl ;
	      if (fecSR1 & FEC_SR1_CLOCKERROR)
		std::cerr << "Clock error" << std::endl ;
	      std::cout << "Clear the errors on the FEC" << std::endl ;
	      fecDevice->setFecRingCR1 (FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;
	      
	      if (ccuList != NULL) {
		for (std::list<keyType>::iterator it = ccuList->begin() ; it != ccuList->end() ; it ++) {
		  tscType8 sra = fecDevice->getCcuSRA(*it) ;
		  std::cerr << "SRA of the CCU 0x" << std::hex << getCcuKey(*it) << ": " << (int)sra << std::endl ;
		  if (sra & CCU_SRA_CRC_ERROR)
		    std::cerr << "\tCRC error" << std::endl ;
		  if (sra & CCU_SRA_IN_ERROR)
		    std::cerr << "\tinternal node error" << std::endl ;
		  if (sra & CCU_SRA_ALSET)
		    std::cerr << "\talarm input active" << std::endl ;
		  if (sra & CCU_SRA_PAR_ERROR)
		    std::cerr << "\tparity error in CCU internal register" << std::endl ;
		  if (sra & CCU_SRA_PARCH_ERROR)
		    std::cerr << "\tparity error in any of the channels" << std::endl ;
		  if (sra & CCU_SRA_ISEQ_ERROR)
		    std::cerr << "\tillegal sequence of tokens detected" << std::endl ;
		  if (sra & CCU_SRA_ICMD_ERROR)
		    std::cerr << "\tInvalid command detected" << std::endl ;
		  if (sra & CCU_SRA_GEN_ERROR)
		    std::cerr << "\terror detected in one of the channels." << std::endl ;

		  fecDevice->setCcuCRA(*it, CCU_CRA_CLRE) ;
		}
	      }

              if (i == receiveFifoDepth) {
                std::cout << "Cannot empty the FIFO receive" << std::endl ;

                // Try it again
                fecDevice->emptyFifoReceive() ;
              }
            }
            catch (FecExceptionHandler &e) {
            }
          }
          
          // Check the FIFO transmit
          if (! (fecSR0 & DD_FEC_TRA_EMPTY)) {
            
            std::cout << "The fifo transmit is not empty" << std::endl ;
            
            try {
              unsigned int i = 0 ;
	      while ((!((fecSR0=fecDevice->getFecRingSR0()) & DD_FEC_TRA_EMPTY) || 
		      (fecSR0 & DD_FEC_TRA_RUN)) && (i < transmitFifoDepth) ) {
		std::cout << "FIFO Transmit Value " << std::dec << (i+1) << " = 0x" << std::hex << fecDevice->getFifoTransmit() << std::endl ;
                i ++ ;
              }

              if (i == transmitFifoDepth) {
                std::cout << "Cannot empty the FIFO transmit" << std::endl ;
              }
            }
            catch (FecExceptionHandler &e) {
            }
          }
          
          // Check the FIFO return
          if (! (fecSR0 & DD_FEC_RET_EMPTY)) {
            
            std::cout << "The fifo return is not empty" << std::endl ;
            
            try {
              unsigned int i = 0 ;
	      while ((!((fecSR0=fecDevice->getFecRingSR0()) & DD_FEC_REC_EMPTY) )
		     && (i < returnFifoDepth) ) {
		std::cout << "FIFO Return Value " << std::dec << (i+1) << " = 0x" << std::hex << fecDevice->getFifoReturn() << std::endl ;
                i ++ ;
              }

              if (i == returnFifoDepth) {
                std::cout << "Cannot empty the FIFO return" << std::endl ;
              }
            }
            catch (FecExceptionHandler &e) {
            }
          }
        }
      }

      // Get the SR0
      fecSR0 = fecDevice->getFecRingSR0() ;
      if (! isFecSR0Correct(fecSR0)) {
        std::cout << "The FEC status register 0 is not correct 0x" << std::hex << fecSR0 << std::endl ;
	std::cout << "Try a FEC reset" << std::endl ;
	fecDevice->fecRingReset() ;
	// Get the SR0
	fecSR0 = fecDevice->getFecRingSR0() ;
        std::cout << "FEC status register 0 = 0x" << std::hex << fecSR0 << std::endl ;
      }

      if (! isFecSR0Correct(fecSR0)) {
        std::cout << "The FEC status register 0 is not correct 0x" << std::hex << fecSR0 << std::endl ;
	std::cout << "Try a PLX reset" << std::endl ;
	fecDevice->fecHardReset() ;
	// Get the SR0
	fecSR0 = fecDevice->getFecRingSR0() ;
        std::cout << "FEC status register 0 = 0x" << std::hex << fecSR0 << std::endl ;

      }

      if (! isFecSR0Correct(fecSR0)) 
	std::cout << "Stop the programm because of bad FEC SR0" << std::endl ;

      usleep (100) ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
  }

  // Not documented
  if (fecDevice != NULL) fecDevice->setIRQ (true) ;

  return (0) ;
}

/**
 * <p>Command: no command
 * read the SR0 of the FEC
 * \param fecAddress - FEC address
 * \param loop - a loop (if negative => for ever ^C needed to end the process)
 * \param tms - time before two loop
 * \warning if the access cannot be performed, the loop is not used
 */
void readFecRegisterSR0 (FecAccess *fecAccess,
			 tscType8 fecAddress,
			 tscType8 ringAddress, 
                         long loop, unsigned long tms ) {

  std::list<keyType> *fecSlotList = NULL ;

  try {
    // Scan the PCI or VME slot for device driver loaded
    fecSlotList = fecAccess->getFecList( ) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
    
    return;
  }

  if ((fecSlotList != NULL) && (!fecSlotList->empty())) {
    for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
            
      keyType indexFEC = *p ;
      std::cout << "####################################################" << std::endl ;
      std::cout << "FEC " << std::dec << (int)getFecKey(indexFEC) << " Ring " << (int)getRingKey(indexFEC) << std::endl ;
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
      if (fecAccess->getFecBusType() == FECVME) {
	FecVmeRingDevice *fecVmeRing = (FecVmeRingDevice *)fecAccess->getFecRingDevice (indexFEC) ;
	try {
	  std::cout << "FEC hardware ID = " << fecVmeRing->getFecHardwareId() << std::endl ;
	  std::cout << "VME Firmware version = " << fecVmeRing->getVmeVersion() << std::endl ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "Unable to read the firmware version from the VME FEC" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	}
      }
      std::cout << "FEC firmware version = " << std::hex << fecAccess->getFecFirmwareVersion(indexFEC) << std::endl ;
#endif

      for (long loopI = 0 ; (loopI < loop) || (loop < 0) ; loopI ++) {
	
	if (loop != 1)
	  std::cout << "----------------------- Loop " << loopI+1 << std::endl ;
	
	try {

	  //std::cout << std::hex << std::endl ; 
	  fecAccess->getFecRingSR0(indexFEC) ;
	  tscType32 fecSR0 = fecAccess->getFecRingSR0(indexFEC) ;

	  std::cout << "Value of the Status Register 0 of the fec : 0x" << std::hex << fecSR0 << std::endl ;
	  if (fecSR0 & 0x1)
	    std::cout << "\tFIFO transmit running" << std::endl ;
	  if (fecSR0 & 0x2)
	    std::cout << "\tFIFO receive running" << std::endl ;
	  if (fecSR0 & 0x4)
	    std::cout << "\tFIFO receive half full" << std::endl ;
	  if (fecSR0 & 0x8)
	    std::cout << "\tFIFO receive full" << std::endl ;
	  if (fecSR0 & 0x10)
	    std::cout << "\tFIFO receive empty" << std::endl ;
	  if (fecSR0 & 0x20)
	    std::cout << "\tFIFO return half full" << std::endl ;
	  if (fecSR0 & 0x40)
	    std::cout << "\tFIFO return full" << std::endl ;
	  if (fecSR0 & 0x80)
	    std::cout << "\tFIFO return empty" << std::endl ;
	  if (fecSR0 & 0x100)
	    std::cout << "\tFIFO transmit half full" << std::endl ;
	  if (fecSR0 & 0x200)
	    std::cout << "\tFIFO transmit full" << std::endl ;
	  if (fecSR0 & 0x400)
	    std::cout << "\tFIFO transmit empty" << std::endl ;
	  if (fecSR0 & 0x800)
	    std::cout << "\tLink initialise" << std::endl ;
	  if (fecSR0 & 0x1000) 
	    std::cout << "\tPending irg" << std::endl ;
	  if (fecSR0 & 0x2000)
	    std::cout << "\tData to FEC" << std::endl ;
	  if (fecSR0 & 0x4000)
	    std::cout << "\tTTCRx ok" << std::endl ;
	}
	catch (FecExceptionHandler &e) {
	  
	  std::cout << "------------ Exception ----------" << std::endl ;
	  std::cout << e.what()  << std::endl ;
	  std::cout << "---------------------------------" << std::endl ;
	}

	// Wait
	if ( (loop != 1) && (tms > 0) ) usleep (tms) ;
      }
    }
  }
  else {
    std::cout << "Did not find any FEC" << std::endl ;
  }

  delete fecSlotList ;
}

/**
 * This method display all FEC status for a given ring
 * \param fecAccess - FEC access object
 * \param findex - which FEC and ring
 * \param ringAddress - ring slot
 */
void displayFecStatus ( FecAccess *fecAccess,
                        keyType index ) {

  try {

    // Display all status
    tscType16 fecSR0 = fecAccess->getFecRingSR0 (index) ;
    std::cout << "\tFEC status register 0: 0x" << std::hex << fecSR0 << std::endl ;
    if (fecSR0 & FEC_SR0_TRARUN)
      std::cout << "\t\tFIFO transmit running" << std::endl ;
    if (fecSR0 & FEC_SR0_RECRUN)
      std::cout << "\t\tFIFO receive running" << std::endl ;
    if (fecSR0 & FEC_SR0_RECFULL)
      std::cout << "\t\tFIFO receive full" << std::endl ;
    if (fecSR0 & FEC_SR0_RECEMPTY)
      std::cout << "\t\tFIFO receive empty" << std::endl ;
    if (fecSR0 & FEC_SR0_RETFULL)
      std::cout << "\t\tFIFO return full" << std::endl ;
    if (fecSR0 & FEC_SR0_RETEMPTY)
      std::cout << "\t\tFIFO return empty" << std::endl ;
    if (fecSR0 & FEC_SR0_TRAFULL)
      std::cout << "\t\tFIFO transmit full" << std::endl ;
    if (fecSR0 &  FEC_SR0_TRAEMPTY)
      std::cout << "\t\tFIFO transmit empty" << std::endl ;
    if (fecSR0 & FEC_SR0_LINKINITIALIZED)
      std::cout << "\t\tLink initialise" << std::endl ;
    if (fecSR0 & FEC_SR0_PENDINGIRQ) 
      std::cout << "\t\tPending irg" << std::endl ;
    if (fecSR0 & FEC_SR0_DATATOFEC)
      std::cout << "\t\tData to FEC" << std::endl ;
    if (fecSR0 & FEC_SR0_TTCRXREADY)
      std::cout << "\t\tTTCRx ok" << std::endl ;
    
    tscType16 fecSR1 = fecAccess->getFecRingSR1 (index) ;
    std::cout << "\tFEC status register 1: 0x" << std::hex << fecSR1 << std::endl ;
    if (fecSR1 & FEC_SR1_ILLDATA)
      std::cout << "\t\tIllegal data" << std::endl ;
    if (fecSR1 & FEC_SR1_ILLSEQ)
      std::cout << "\t\tIllegal sequence" << std::endl ;
    if (fecSR1 & FEC_SR1_CRCERROR) 
      std::cout << "\t\tCRC error" << std::endl ;
    if (fecSR1 & FEC_SR1_DATACOPIED) 
      std::cout << "\t\tData copied" << std::endl ;
    if (fecSR1 & FEC_SR1_ADDRSEEN) 
      std::cout << "\t\tAddress seen" << std::endl ;
    if (fecSR1 & FEC_SR1_ERROR) 
      std::cout << "\t\tError" << std::endl ;
    if (fecSR1 & FEC_SR1_TIMEOUT) 
      std::cout << "\t\tTimeout" << std::endl ;
    if (fecSR1 & FEC_SR1_CLOCKERROR) 
      std::cout << "\t\tClock error" << std::endl ;

    tscType16 fecCR0 = fecAccess->getFecRingCR0 (index) ;
    std::cout << "\tFEC control register 0: 0x" << std::hex << fecCR0 << std::endl ;
    if (fecCR0 & FEC_CR0_ENABLEFEC)
      std::cout << "\t\tFEC enable" << std::endl ;
    else
      std::cout << "\t\tFEC disable" << std::endl ;
    //if (fecCR0 & FEC_CR0_SEND)
    //  std::cout << "\t\tSend" << std::endl ;
    //if (fecCR0 & 0x4)
    //  std::cout << "\t\tResend" << std::endl ;
    if (fecCR0 & FEC_CR0_XTALCLOCK) 
      std::cout << "\t\tXtal clock" << std::endl ;
    if (fecCR0 & FEC_CR0_SELSEROUT)
      std::cout << "\t\tOutput ring B" << std::endl ;
    else
      std::cout << "\t\tOutput ring A" << std::endl ;
    if (fecCR0 & FEC_CR0_SELSERIN)
      std::cout << "\t\tInput ring B" << std::endl ;
    else
      std::cout << "\t\tInput ring A" << std::endl ;
    
    // The CR1 cannot be read
    //tscType16 fecCR1 = fecAccess->getFecRingCR1 (index) ;
    //std::cout << "\tFEC control register 1: 0x%04X\n", fecCR1) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}


/** This method try to recover the ring until the status register 0 is ok
 */
void emptyFIFOs ( FecAccess *fecAccess, keyType index, bool display ) {

  unsigned int receiveFifoDepth = 0 ;
  unsigned int transmitFifoDepth = 0 ;
  unsigned int returnFifoDepth = 0 ;

  //std::cout << "Start emptyFIFOs: SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(index) << std::endl ;
  try {
    
    // Start the scanning of the ring for each FEC device
    FecRingDevice *fecDevice = fecAccess->getFecRingDevice ( index ) ;

    receiveFifoDepth = fecAccess->getReceiveFifoDepth(index) ;
    transmitFifoDepth = fecAccess->getTransmitFifoDepth(index) ;
    returnFifoDepth = fecAccess->getReturnFifoDepth(index) ;

    // Is the status is correct
    if (! isFecSR0Correct(fecAccess->getFecRingSR0(index))) {

      uint fecSR0 = fecAccess->getFecRingSR0(index) ;

      //std::cout << "      emptyFIFOs: SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(index) << std::endl ;
      fecSR0 =  fecAccess->getFecRingSR0(index) ;

      // FIFO receive
      if (! (fecSR0 & FEC_SR0_RECEMPTY)) {
        
        // std::cout << "FIFO receive not empty" << std::endl ;
        tscType32 value = 0 ;
        tscType16 FecSR0 ; 
	
        unsigned int loopCnt = 0 ; 
        
        while (
               (!((FecSR0=fecAccess->getFecRingSR0(index)) & FEC_SR0_RECEMPTY) || (FecSR0 & FEC_SR0_RECRUN)) 
               && (loopCnt < receiveFifoDepth)
               && (value != 0xFFFF) ) {
	  

	  if (display) 
	    std::cout << "FIFO receive word(" << std::dec << loopCnt << ") = " 
		 <<  fecDevice->getFifoReceive()
		 << " (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(index)
		 << ")" << std::endl ;
	  else
	    fecDevice->getFifoReceive() ;
          
          loopCnt++; 
          
        } // end of while loop 
      }
      
      // Clear the errors and display the status
      // fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

      //std::cout << "      emptyFIFOs: SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(index) << std::endl ;
      fecSR0 =  fecAccess->getFecRingSR0(index) ;

      // FIFO return
      if (! (fecSR0 & FEC_SR0_RETEMPTY)) {

	// std::cout << "FIFO return not empty" << std::endl ;
	tscType32 value = 0 ;
	tscType16 FecSR0 ; 

	unsigned int loopCnt = 0 ; 

	while (
	       (!((FecSR0=fecAccess->getFecRingSR0(index)) & FEC_SR0_RETEMPTY)) 
	       && (loopCnt < returnFifoDepth)
	       && (value != 0xFFFF) ) {

	  if (display) 
	    std::cout << "FIFO return word(" << std::dec << loopCnt << ") = " 
		 <<  fecDevice->getFifoReturn()
		 << " (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(index)
		 << ")" << std::endl ;
	  else
	    fecDevice->getFifoReturn() ;
          
	  loopCnt++; 
	} // end of while loop 
      }

      // Clear the errors and display the status
      // fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

      //std::cout << "      emptyFIFOs: SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(index) << std::endl ;
      fecSR0 =  fecAccess->getFecRingSR0(index) ;

      // FIFO transmit
      if (! (fecSR0 & FEC_SR0_TRAEMPTY)) {

	// std::cout << "FIFO transmit not empty" << std::endl ;
	tscType32 value = 0 ;
	tscType16 FecSR0 ; 

	unsigned int loopCnt = 0 ; 

	while (
	       (!((FecSR0=fecAccess->getFecRingSR0(index)) & FEC_SR0_TRAEMPTY) || (FecSR0 & FEC_SR0_TRARUN)) 
	       && (loopCnt < transmitFifoDepth)
	       && (value != 0xFFFF) ) {

	  if (display) 
	    std::cout << "FIFO transmit word(" << std::dec << loopCnt << ") = " 
		 <<  fecDevice->getFifoTransmit()
		 << " (SR0 = 0x" << std::hex <<  fecAccess->getFecRingSR0(index)
		 << ")" << std::endl ;
	  else
	    fecDevice->getFifoTransmit() ;
     
	  loopCnt++; 
	} // end of while loop 
      }

      //std::cout << "      emptyFIFOs: SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(index) << std::endl ;
      fecSR0 =  fecAccess->getFecRingSR0(index) ;

      // Pending IRQ
      if (fecSR0 & FEC_SR0_PENDINGIRQ) {

	//std::cout << "setFecRingCR1 (" << std::hex << (FEC_CR1_CLEARIRQ) << ")" << std::endl ;
	fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ) ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }

  std::cout << "Stop emptyFIFOs: SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(index) << std::endl ;
}

/**
 * This method try to recover the ring until the status register 0 is ok
 * <p>command: -recover
 * \param fecAccess - FEC access object
 * \param fecAddress - FEC slot
 * \param ringAddress - ring slot
 */
void testRecover ( FecAccess *fecAccess,
		   tscType8 fecAddress,
		   tscType8 ringAddress ) {

  unsigned int receiveFifoDepth = 0 ;
  unsigned int transmitFifoDepth = 0 ;
  unsigned int returnFifoDepth = 0 ;

  try {

    // Scan the PCI slot for device driver loaded
    // std::list<keyType> *fecSlotList = fecAccess->scanForFECs (0, 4) ;
    // delete fecSlotList ;

    keyType index = buildFecRingKey(fecAddress, ringAddress) ;

    // Start the scanning of the ring for each FEC device
    FecRingDevice *fecDevice = fecAccess->getFecRingDevice ( index ) ;

    // Retreive FIFO depth
    receiveFifoDepth = fecAccess->getReceiveFifoDepth ( index ) ;
    transmitFifoDepth = fecAccess->getTransmitFifoDepth ( index ) ;
    returnFifoDepth = fecAccess->getReturnFifoDepth ( index ) ;

    // Display all status
    std::cout << "---------------------------------------------------------------" << std::endl ;
    std::cout << "FEC " << std::dec << getFecKey(index) << "." << getRingKey(index) << std::endl ;
    displayFecStatus (fecAccess, index) ;

    // Clear the errors and display the status
    fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;
    
    if (! isFecSR0Correct(fecAccess->getFecRingSR0(index))) {
      
      std::cout << "---------------------------------------------------------------" << std::endl ;
      std::cout << "FEC " << std::dec << getFecKey(index) << "." << getRingKey(index) << " => recovering " << std::endl ;
          tscType16 fecSR0 = fecAccess->getFecRingSR0 (index) ;
      std::cout << "\tFEC status register 0: 0x" << std::hex << fecSR0 << std::endl ;
      
      // Try to emptyied the FIFO
      if (! (fecSR0 & FEC_SR0_RECEMPTY)) {
        
        std::cout << "\t\tFIFO receive not empty" << std::endl ;
        tscType32 value = 0 ;
        tscType16 FecSR0 ; 

        unsigned int loopCnt = 0 ; 
        
        while (
               (!((FecSR0=fecAccess->getFecRingSR0(index)) & FEC_SR0_RECEMPTY) || (FecSR0 & FEC_SR0_RECRUN)) 
               && (loopCnt < receiveFifoDepth)
               && (value != 0xFFFF) ) {

          // if (value==0xffff) fifo is empty : go out of loop  
          tscType32 value = 
	    fecDevice->getFifoReceive() ; //getFifoReceive() ;
	  std::cout << "VALUE = " << std::hex << value << std::endl ;
          
          loopCnt++; 
          
        } // end of while loop 
  
	if (loopCnt >= receiveFifoDepth)
	  std::cout << "\t\t>>>>>>>>>>>>>>>>> Cannot empty the FIFO receive" << std::endl ;
	else
	  std::cout << "\t\tThe FIFO receive is now empty" << std::endl ;
      }
      else std::cout << "\t\tThe FIFO receive is empty" << std::endl ;

      // Clear the errors and display the status
      fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

      if (! (fecSR0 & FEC_SR0_RETEMPTY)) {

	std::cout << "\t\tFIFO return not empty" << std::endl ;
	tscType32 value = 0 ;
	tscType16 FecSR0 ; 

	unsigned int loopCnt = 0 ; 

	while (
	       (!((FecSR0=fecAccess->getFecRingSR0(index)) & FEC_SR0_RETEMPTY)) 
	       && (loopCnt < returnFifoDepth)
	       && (value != 0xFFFF) ) {

	  // if (value==0xffff) fifo is empty : go out of loop  
	  fecDevice->getFifoReturn() ; //getFifoReturn() ;
     
	  loopCnt++; 

	} // end of while loop 
  
	if (loopCnt >= returnFifoDepth)
	  std::cout << "\t\t>>>>>>>>>>>>>>>>>> Cannot empty the FIFO return" << std::endl ;
	else
	  std::cout << "\t\tThe FIFO return is now empty" << std::endl ;
      }
      else 
	std::cout << "\t\tThe FIFO return is empty" << std::endl ;

      // Clear the errors and display the status
      fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

      if (! (fecSR0 & FEC_SR0_TRAEMPTY)) {

	std::cout << "\t\tFIFO transmit not empty" << std::endl ;
	tscType32 value = 0 ;
	tscType16 FecSR0 ; 

	unsigned int loopCnt = 0 ; 

	while (
	       (!((FecSR0=fecAccess->getFecRingSR0(index)) & FEC_SR0_TRAEMPTY) || (FecSR0 & FEC_SR0_TRARUN)) 
	       && (loopCnt < transmitFifoDepth)
	       && (value != 0xFFFF) ) {

	  // if (value==0xffff) fifo is empty : go out of loop  
	  fecDevice->getFifoTransmit() ;//getFifoTransmit() ;
     
	  loopCnt++; 

	} // end of while loop 
  
	if (loopCnt >= transmitFifoDepth)
	  std::cout << "\t\t>>>>>>>>>>>>>>>> Cannot empty the FIFO transmit" << std::endl ;
	else
	  std::cout << "\t\tThe FIFO transmit is now empty" << std::endl ;
      }
      else std::cout << "\t\tThe FIFO transmit is empty" << std::endl ;

      // Clear the errors and display the status
      fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

      if (fecSR0 & FEC_SR0_LINKINITIALIZED)
	std::cout << "\t\tLink initialise" << std::endl ;
      else {
	std::cout << "\t\tLink not initialise" << std::endl ;
	std::cout << "Try to reconfigure the ring" << std::endl ;
	std::cout << "Not implemented" << std::endl ;
	// ????????????????????????????????????????
      }

      // Clear the errors and display the status
      fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

      if (fecSR0 & FEC_SR0_PENDINGIRQ) 
	std::cout << "\t\tPending irg" << std::endl ;

      if (fecSR0 & FEC_SR0_DATATOFEC)
	std::cout << "\t\tData to FEC" << std::endl ;

      if (fecSR0 & FEC_SR0_TTCRXREADY)
	std::cout << "\t\tTTCRx ok" << std::endl ;

      // Clear the errors and display the status
      fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;
    }

    if (! isFecSR0Correct(fecAccess->getFecRingSR0(index))) {
      std::cout << "\t\tTry to make a soft reset" << std::endl ;
      fecAccess->fecRingReset(index) ;
    }

    if (! isFecSR0Correct(fecAccess->getFecRingSR0(index))) {
      std::cout << "\t\tMake a hard reset" << std::endl ;
      fecAccess->fecHardReset(index) ;
    }

    // Clear the errors and display the status
    fecAccess->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;
    
    if (! isFecSR0Correct(fecAccess->getFecRingSR0(index))) {
      std::cout << "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Cannot recover the ring" << std::endl ;
      if (! noGetchar_) {
	std::cout << "Press <enter> to continue" << std::endl ;
	getchar() ;
      }
    }
  
    std::cout << "---------------------------------------------------------------" << std::endl ;
    std::cout << "FEC " << std::dec << getFecKey(index) << "." << getRingKey(index) << std::endl ;
    displayFecStatus (fecAccess, index) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}

/**
 * <p>command: no directly accessible
 * \param SR0: FEC status register 0
 */
void displayFECSR0 ( tscType16 fecSR0 ) {
  
  std::cout << "\tFEC status register  0: 0x" << std::hex << fecSR0 << std::endl ;
  if (fecSR0 & 0x1)
    std::cout << "\t\tFIFO transmit running" << std::endl ;
  if (fecSR0 & 0x2)
    std::cout << "\t\tFIFO receive running" << std::endl ;
  if (fecSR0 & 0x4)
    std::cout << "\t\tFIFO receive half full" << std::endl ;
  if (fecSR0 & 0x8)
    std::cout << "\t\tFIFO receive full" << std::endl ;
  if (fecSR0 & 0x10)
    std::cout << "\t\tFIFO receive empty" << std::endl ;
  if (fecSR0 & 0x20)
    std::cout << "\t\tFIFO return half full" << std::endl ;
  if (fecSR0 & 0x40)
    std::cout << "\t\tFIFO return full" << std::endl ;
  if (fecSR0 & 0x80)
    std::cout << "\t\tFIFO return empty" << std::endl ;
  if (fecSR0 & 0x100)
    std::cout << "\t\tFIFO transmit half full" << std::endl ;
  if (fecSR0 & 0x200)
    std::cout << "\t\tFIFO transmit full" << std::endl ;
  if (fecSR0 & 0x400)
    std::cout << "\t\tFIFO transmit empty" << std::endl ;
  if (fecSR0 & 0x800)
    std::cout << "\t\tLink initialise" << std::endl ;
  if (fecSR0 & 0x1000) 
    std::cout << "\t\tPending irg" << std::endl ;
  if (fecSR0 & 0x2000)
    std::cout << "\t\tData to FEC" << std::endl ;
  if (fecSR0 & 0x4000)
    std::cout << "\t\tTTCRx ok" << std::endl ;
}
	
/**
 * <p>command: no directly accessible
 * \param SR1: FEC status register 1
 */
void displayFECSR1 ( tscType16 fecSR1 ) {

  std::cout << "\tFEC status register  1: 0x" << std::hex << fecSR1 << std::endl ;
  if (fecSR1 & 0x1)
    std::cout << "\t\tIllegal data" << std::endl ;
  if (fecSR1 & 0x2)
    std::cout << "\t\tIllegal sequence" << std::endl ;
  if (fecSR1 & 0x4) 
    std::cout << "\t\tCRC error" << std::endl ;
  if (fecSR1 & 0x8) 
    std::cout << "\t\tData copied" << std::endl ;
  if (fecSR1 & 0x10) 
    std::cout << "\t\tAddress seen" << std::endl ;
  if (fecSR1 & 0x20) 
    std::cout << "\t\tError" << std::endl ;
}


/**
 * <p>command: no directly accessible
 * \param CR0: FEC control register 0
 */
void displayFECCR0 ( tscType16 fecCR0 ) {

  std::cout << "\tFEC control register 0:  0x" << std::hex << fecCR0 << std::endl ;
  if (fecCR0 & 0x1)
    std::cout << "\t\tFEC enable" << std::endl ;
  else
    std::cout << "\t\tFEC disable" << std::endl ;
  //if (fecCR0 & 0x2)
  //  std::cout << "\t\tSend" << std::endl ;
  //if (fecCR0 & 0x4)
  //  std::cout << "\t\tResend" << std::endl ;
  if (fecCR0 & 0x8)
    std::cout << "\t\tOutput ring B" << std::endl ;
  else
    std::cout << "\t\tOutput ring A" << std::endl ;
  if (fecCR0 & 0x10)
    std::cout << "\t\tInput ring B" << std::endl ;
  else
    std::cout << "\t\tInput ring A" << std::endl ;
  std::cout << "\t\tSel irq mode = " << (fecCR0 & 0x20) << std::endl ;
  //std::cout << "\t\tReset TTCRx = " << (fecCR0 & 0x40) << std::endl ;
}

/**
 * <p>command: no directly accessible
 * \param CR1: FEC control register 1
 */
void displayFECCR1 ( tscType16 fecCR1 ) {

  std::cout << "\tFEC control register 1:  0x" << std::hex << fecCR1 << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU status register A
 */
void displayCCUSRA ( tscType8 SRA ) {

  std::cout << "\t\tCCU status register A = 0x" << std::hex << (int)SRA << std::endl ;
  if (SRA & 0x1)
    std::cout << "\t\t(PED)     Error bit: CRC error" << std::endl ;
  if (SRA & 0x2) 
    std::cout << "\t\t(IE)      Internal error: node controller state machine error" << std::endl ;
  if (SRA & 0x4)
    std::cout << "\t\t(ALSET)   One or more ALARM inputs are currently active" << std::endl ;
  if (SRA & 0x8)
    std::cout << "\t\t(CCUPERR) Parity error in the internal CCU registers" << std::endl ;
  if (SRA & 0x10)
    std::cout << "\t\t(CHAPERR) Parity error in any of the CCU channels" << std::endl ;
  if (SRA & 0x20)
    std::cout << "\t\t(ILLSEQ)  Illegal sequence" << std::endl ;
  if (SRA & 0x40)
    std::cout << "\t\t(INVMCD)  Invalid command" << std::endl ;
  if (SRA & 0x80)
    std::cout << "\t\t(GE)      Global error" << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU status register B
 */
void displayCCUSRB ( tscType8 SRB ) {
  
  std::cout << "\t\tLast correctly received transaction number (SRB) " << (int)SRB << "(0x" << std::hex << (int)SRB << ")" << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU status register C
 */
void displayCCUSRC ( tscType8 SRC ) {
  
  std::cout << "\t\tCCU status register C = 0x" << std::hex << (int)SRC << std::endl ;
  std::cout << "\t\tInput " << (SRC & 0x1 ? 'B' : 'A') << " Ouput " << (SRC & 0x2 ? 'B' : 'A') << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU status register D
 */
void displayCCUSRD ( tscType8 SRD ) {

  std::cout << "\t\tSource field for the last ring message addressed to this CCU: 0x" << std::hex << (int)SRD << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU status register E
 */
void displayCCUSRE ( tscType32 SRE ) {

  std::cout << "\t\tCCU status register E = 0x" << std::hex << (unsigned int)SRE << std::endl ;
  ushort channelBusy[18] = {0} ;

  for (ushort i = 0 ; i < 18 ; i ++) {
    
    if (SRE & (0x1 << i)) channelBusy[i] = 1 ;
  }

  std::cout << "\t\tList of busy channels:\n\t\t- i2c: " << std::endl ;
  for (ushort i = 0 ; i < 16 ; i ++) 
    if (channelBusy[i]) std::cout << i+1 << std::endl ;
  std::cout << std::endl ;
  if (channelBusy[16] == 1) std::cout << "\t\t- Trigger channel busy" << std::endl ;
  if (channelBusy[17] == 1) std::cout << "\t\t- Memory channel busy" << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU status register F
 */
void displayCCUSRF ( tscType16 SRF ) {
  
  std::cout << "\t\tNumber of parity error since the last reset: 0x" << std::hex << SRF << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU control register A
 */
void displayCCUCRA ( tscType8 CRA ) {

  std::cout << "\t\tCCU control register A = 0x" << std::hex << CRA << std::endl ;
  if (CRA & 0x20)
    std::cout << "\t\t(EXTRES) Generate external reset" << std::endl ;
  if (CRA & 0x40)
    std::cout << "\t\t(CLRE)   Clear error" << std::endl ;
  if (CRA & 0x80)
    std::cout << "\t\t(RES)    Reset all channels" << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU control register B
 */
void displayCCUCRB ( tscType8 CRB ) {
  
  std::cout << "\t\tCCU control register B = 0x" << std::hex << (int)CRB << std::endl ;
  if (CRB & 0x1)
    std::cout << "\t\t(ENAL1) Enable ALARM1 interrupt" << std::endl ;
  if (CRB & 0x2) 
    std::cout << "\t\t(ENAL2) Enable ALARM2 interrupt" << std::endl ;
  if (CRB & 0x4)
    std::cout << "\t\t(ENAL3) Enable ALARM3 interrupt" << std::endl ;
  if (CRB & 0x8)
    std::cout << "\t\t(ENAL4) Enable ALARM4 interrupt" << std::endl ;
  
  std::cout << "\t\t(RTRY) Retry count: " << (((CRB & 0x3) == 0x3) ? 4 : (CRB & 0x3)) << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU control register C
 */
void displayCCUCRC ( tscType8 CRC ) {
  
  std::cout << "\t\tCCU control register C = 0x" << std::hex << (int)CRC << std::endl ;
  std::cout << "\t\tInput " << (CRC & 0x1 ? 'B' : 'A') << " Ouput " << (CRC & 0x2 ? 'B' : 'A') << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU control register D
 */
void displayCCUCRD ( tscType8 CRD ) {

  std::cout << "\t\tCCU control register D = 0x" << std::hex << (int)CRD << std::endl ;
  std::cout << "\t\tBroadcast class: 0x" << std::hex << (int)(CRD & 0x7F) << std::endl ;
}

/**
 * <p>command: no directly accessisble
 * \param CCU control register E
 */
void displayCCUCRE ( tscType32 CRE ) {

  std::cout << "\t\tCCU control register E = 0x" << std::hex << (unsigned int)CRE << std::endl ;
  ushort channelEnable[23] = {0}, i = 0 ;
  bool i2cDisplay = false, piaDisplay = false ;
  
  for (i = 0 ; i < 23 ; i ++) {
    channelEnable[i] = CRE & (0x1 << i) ;
    if (channelEnable[i]) {
      if (i < 16) i2cDisplay = true ;
      else
	if (i < 20) piaDisplay = true ;
    }
  }
  
  std::cout << "\t\tList of enable channels:" << std::endl ;
  
  std::cout << "\t\t- i2c: " << std::endl ;
  if (! i2cDisplay) std::cout << "None" << std::endl ;
  for (ushort i = 0 ; i < 16 ; i ++) 
    if (channelEnable[i]) std::cout << std::dec << (i+1) << " ";

  std::cout << "\n\t\t- PIA: " << std::endl ;
  if (! piaDisplay) std::cout << "None" << std::endl ;
  
  for (; i < 20 ; i ++) 
    if (channelEnable[i]) std::cout << std::dec << (i+1) << " " ;
  std::cout << std::endl ;

  if (channelEnable[20]) std::cout << "\t\t- Memory channel enable" << std::endl ;
  else std::cout << "\t\t- Memory channel not enabled" << std::endl ;
  if (channelEnable[21]) std::cout << "\t\t- Trigger channel enable" << std::endl ;
  else std::cout << "\t\t- Trigger channel not enabled" << std::endl ;
  if (channelEnable[22]) std::cout << "\t\t- JTAG channel enable" << std::endl ;
  else std::cout << "\t\t- JTAG channel not enabled" << std::endl ;
}
