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
#ifndef FECEXCEPTIONHANDLER_H
#define FECEXCEPTIONHANDLER_H
#include <string.h>
#include <iostream>
#include <exception>
#include <string>
#include <sstream>

#include <cstdio>  // snprintf
#include <cstdlib>
//#include <string.h> // strcpy
//#include <time.h>   // time

#include "keyType.h"
#include "tscTypes.h"
#include "errorCodeDefinition.h"

#include "datatypes.h"
#include "dderrors.h"

#include "cmdDescription.h"

#include "FecRingRegisters.h"

/**
 * \class FecExceptionHandler
 * This class manage all the exceptions (from hardware or software). 
 * For each exception, you have several tags:
 * <ul>
 * <li>Error code: Depend of the error code range, you can know
 * which subsystem (device driver, FEC classes, database, XML)
 * generate this error:
 *    <ul>
 *    <li> [0, 199] device driver error code (MAXDDERRORCODE)
 *    <li> [200, 999] FEC class hierarchy (MAXFECSUPERVISORERRORCODE)
 *    <li> [1000, 1199] Database error (MAXDATABASEERRORCODE)
 *    <li> [1200, 1399] XML error (MAXXMLERRORCODE)
 *    </ul> 
 * <li>System Number: FEC Supervisor: one number for all the FEC supervisor
 * <li>Sub system Number: 
 *    <ul>
 *    <li>database exception (from ORACLE)
 *    <li>XML parsing exception (from SAX)
 *    <li>FEC device driver error (error code returned by open, read, write, ioctl, close redefined in the glue library)
 *    <li>FEC supervisor exception
 *    </ul>
 * <li>A timestamp is added automatically
 * <li>An information and a position in the hardware can be given
 * <li>A user and a developper message must be added also
 * </ul>
 * \author Frederic Drouhin
 * \date April 2003
 * \brief FEC exception class manager
 * \warning this class replace all the previous exceptions: ccuChannelAccessError and fecHardAccessError
 */
class FecExceptionHandler {
  
 private:
  
  /** Error code
   */
  errorType errorCode_ ;

  /** Error message
   */
  std::string errorMessage_ ;

  /** Fault severity
   */
  errorType faultSeverity_ ;

  /** Time stamp
   */
  time_t timestamp_ ;

  /** Method which raise the exception
   */
  std::string method_ ;

  /** source file name
   */
  std::string sourceFileName_ ;

  /** line in the code
   */
  unsigned int line_ ;

  /** Error position index
   */
  keyType hardPosition_ ;

  /** Is the position was given ?
   */
  bool positionGiven_ ;

  /** Information about which register was accessed ..
   */
  unsigned int informationSup_ ;

  /** Information name
   */
  std::string informationName_ ;

  /** Is the information was given ?
   */
  bool informationGiven_ ;

  /** Original frame source of the error
   */
  tscType8 frame_[MAXFECFIFOWORD*4] ;  // initial frame
  tscType8 dAck_[MAXFECFIFOWORD*4]  ;  // direct acknowledge
  tscType8 fAck_[MAXFECFIFOWORD*4]  ;  // force acknowledge or read answer

  /** If the regiters has been read 
   */
  bool registersRead_ ;

  /** All the registers from FEC and CCU
   */
  FecRingRegisters *fecRingRegisters_       ;  // FEC and CCU Registers

  /** FEC registers in the order, SR0, SR1, CR0
   */
  tscType16 fecRegisters_[FECCR0+1] ;

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

  /** Software tag version set by CVS
   */
  std::string softwareTagVersion_ ;

  /** Decode a key and return the position in string
   */
  std::string decodeKeyPosition ( keyType index ) {

    tscType16 fecSlot = getFecKey(index);
    tscType16 ringSlot = getRingKey(index) ;
    tscType16 ccuAddress = getCcuKey(index) ;
    tscType16 channel = getChannelKey(index) ;
    tscType16 deviceAddress = getAddressKey(index) ;

    std::stringstream strMsg ;
    strMsg << "FEC 0x" << std::hex << fecSlot << " ring 0x" << ringSlot ;
    if (ccuAddress != 0) {
      strMsg << " CCU 0x" << ccuAddress ;

      if (channel != 0) {
	strMsg << " channel 0x" << channel ;

	if (isi2cChannelCcu25(index) || isi2cChannelOldCcu(index)) {
	  strMsg << " I2C address 0x" << deviceAddress << std::dec ; //<< std::endl ;
	}
      }
    }

    return strMsg.str() ;
  }

 public:

  /**
   * \brief Default constructor for inheritance
   */
  FecExceptionHandler ( ) throw ( ):
    errorCode_(0),
    errorMessage_("Not an error"),
    faultSeverity_(INFORMATIONCODE),
    method_(__PRETTY_FUNCTION__),
    sourceFileName_(__FILE__),
    line_(__LINE__),
    hardPosition_(0),
    positionGiven_(false),
    informationSup_(0),
    informationName_("Not filled"),
    informationGiven_(false),
    registersRead_(false),
    fecRingRegisters_(NULL) {
     
    // Current timestamp
    timestamp_ = time(NULL) ;

    // Put in all the frames
    memset (frame_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    memset (dAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    memset (fAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;

    // Reset the register values
    memset(fecRegisters_, 0, (FECCR0+1) * sizeof(tscType16)) ;
    memset(ccuRegisters_, 0, (CCUCRE+1) * sizeof(tscType32)) ;
    memset(i2cRegisters_, 0, (I2CCRA+1) * sizeof(tscType8)) ;
    memset(memoryRegisters_, 0, (MEMORYCRA+1) * sizeof(tscType8)) ;
    memset(piaRegisters_, 0, (PIAGCR+1) * sizeof(tscType8)) ;

    // Set the software version
    softwareTagVersion_ = getCVSTag() ;
  }

  /**
   * \brief Exception defined for no hardware error
   */
  FecExceptionHandler ( errorType errorCode,
			std::string errorMessage,
			errorType faultSeverity,
			std::string method,
			std::string sourceFileName,
			unsigned int line ) throw ( ) :
    errorCode_(errorCode),
    errorMessage_(errorMessage),
    faultSeverity_(faultSeverity),
    method_(method),
    sourceFileName_(sourceFileName),
    line_(line),
    hardPosition_(0),
    positionGiven_(false),
    informationSup_(0),
    informationName_("Not filled"),
    informationGiven_(false),
    registersRead_(false),
    fecRingRegisters_(NULL) {

    // Current timestamp
    timestamp_ = time(NULL) ;

    // Put in all the frames
    memset (frame_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    memset (dAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    memset (fAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;

    // Reset the register values
    memset(fecRegisters_, 0, (FECCR0+1) * sizeof(tscType16)) ;
    memset(ccuRegisters_, 0, (CCUCRE+1) * sizeof(tscType32)) ;
    memset(i2cRegisters_, 0, (I2CCRA+1) * sizeof(tscType8)) ;
    memset(memoryRegisters_, 0, (MEMORYCRA+1) * sizeof(tscType8)) ;
    memset(piaRegisters_, 0, (PIAGCR+1) * sizeof(tscType8)) ;

    // Set the software version
    softwareTagVersion_ = getCVSTag() ;
  }
  
  /**
   * \brief FEC device driver or FecSupervisor hardware exception
   */
  FecExceptionHandler ( errorType errorCode,
			std::string errorMessage,
			errorType faultSeverity,
			std::string method,
			std::string sourceFileName,
			unsigned int line,
			keyType hardPosition,
			std::string informationName = "Not filled",
			unsigned int informationSup = 0,
			FecRingRegisters *fecRingRegisters = NULL,
			tscType8 *frame = NULL,
			tscType8 *dAck = NULL,
			tscType8 *fAck = NULL ) throw ( ):
    
    errorCode_(errorCode),
    errorMessage_(errorMessage),
    faultSeverity_(faultSeverity),
    method_(method),
    sourceFileName_(sourceFileName),
    line_(line),
    hardPosition_(hardPosition),
    positionGiven_(true),
    informationSup_(informationSup),
    informationName_(informationName),
    registersRead_(false),
    fecRingRegisters_(NULL) {


    // Current timestamp
    timestamp_ = time(NULL) ;

    // information sup ?
    if (informationName == "Not filled") informationGiven_ = false ;
    else informationGiven_ = true ;

    // Copy the frame if needed
    memset (frame_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    memset (dAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    memset (fAck_, 0, DD_USER_MAX_MSG_LENGTH*4) ;
    if (frame != NULL) setFrame (frame_, frame) ;
    if (dAck != NULL) setFrame (dAck_, dAck) ;
    if (fAck != NULL) setFrame (fAck_, fAck) ;

    // Reset the register values
    memset(fecRegisters_, 0, (FECCR0+1) * sizeof(tscType16)) ;
    memset(ccuRegisters_, 0, (CCUCRE+1) * sizeof(tscType32)) ;
    memset(i2cRegisters_, 0, (I2CCRA+1) * sizeof(tscType8)) ;
    memset(memoryRegisters_, 0, (MEMORYCRA+1) * sizeof(tscType8)) ;
    memset(piaRegisters_, 0, (PIAGCR+1) * sizeof(tscType8)) ;

    // Is registers have been read => clone the FecRingRegisters
    if ((fecRingRegisters != NULL) && (fecRingRegisters->getIndex() != 0)) {
      if (fecRingRegisters->clone(fecRegisters_, ccuRegisters_, i2cRegisters_, memoryRegisters_, piaRegisters_) != hardPosition_) {
	std::cerr << "FecExceptionHandler::FecExceptionHandler: mismatch between the error and the registers that was read" << std::endl ;
      }
      registersRead_ = true ;
    }

    // Set the software version
    softwareTagVersion_ = getCVSTag() ;
  }

  /** Delete the register given
   */
  virtual ~FecExceptionHandler ( ) throw ( ) { 

    if (fecRingRegisters_ != NULL) {
      delete fecRingRegisters_ ;
      fecRingRegisters_ = NULL ;
    }
  }

  /** Error code
   */
  errorType getErrorCode ( ) { return errorCode_ ; }

  /** Error message
   */
  std::string getErrorMessage ( ) { return errorMessage_ ; }

  /** Fault severity
   */
  errorType getFaultSeverity ( ) { return faultSeverity_ ; }

  /** Time stamp
   */
  time_t getTimeStamp ( ) { return timestamp_ ; }

  /** Method which raise the exception
   */
  std::string getMethod ( ) { return method_ ; }

  /** File name of source
   */
  std::string getSourceFileName ( ) { return sourceFileName_ ; }

  /** line in the code
   */
  unsigned int getLine ( ) { return line_ ; }

  /** Error position index
   */
  keyType getHardPosition ( ) { return hardPosition_ ; }

  /** Is the position was given ?
   */
  bool getPositionGiven ( ) { return  positionGiven_ ; }

  /** Information about which register was accessed ..
   */
  unsigned int getInformationSup ( ) { return informationSup_ ; }

  /** Information name
   */
  std::string getInformationName ( ) { return informationName_ ; }

  /** Is the information was given ?
   */
  bool getInformationGiven ( ) { return informationGiven_ ; }

  /** Get the frame in case of i2c error
   */
  tscType8 *getRequestFrame ( ) {

    return (frame_) ;
  }

  /** Original frame source of the error
   */
  tscType8 *getFrame ( ) { return frame_ ; }

  /** Original frame source of the error
   */
  tscType8 *getDAck ( ) { return dAck_ ; }

  /** Original frame source of the error
   */
  tscType8 *getFAck ( ) { return fAck_ ; }

  /** If the regiters has been read 
   */
  bool getRegistersRead ( ) { return registersRead_ ; }

  /** FEC registers in the order, SR0, SR1, CR0
   */
  tscType16 *getFecRegisters ( ) { return fecRegisters_ ; }

  /** Return the list of the registers for FEC and CCU
   */
  FecRingRegisters *getFecRingRegisters ( ) {

    if (fecRingRegisters_ != NULL) {
      delete fecRingRegisters_ ;
      fecRingRegisters_ = NULL ;
    }

    if (registersRead_)
      fecRingRegisters_ = new FecRingRegisters (hardPosition_, fecRegisters_, ccuRegisters_, i2cRegisters_, memoryRegisters_, piaRegisters_) ;

    return fecRingRegisters_ ;
  }

  /* CCU registers SRA, SRB, SRC, SRD, SRE, SRF, CRA, CRB, CRC, CRD, CRE
   */
  tscType32 *getCcuRegisters ( ) { return ccuRegisters_ ; }

  /** i2c registers
   */
  tscType8 *getI2cRegisters ( ) { return i2cRegisters_ ; }

  /** memory registers
   */
  tscType8 *getMemoryRegisters ( ) { return memoryRegisters_ ; }

  /** PIA registers
   */
  tscType8 *getPiaRegisters ( ) { return piaRegisters_ ; }

  /** Software tag version set by CVS
   */
  std::string getSoftwareTagVersion ( ) { return softwareTagVersion_ ; }

  /**
   * Display the last error
   */
  virtual void writeTo ( std::ostringstream flux ) throw ( ) {    

    flux << "------------------ FecExceptionHandler error ---------------" << std::endl ;
    flux << what ( ) << std::endl ;
    flux << "--------------------------------------------------" << std::endl ;
  }
  
  /**
   * \brief Returned a message containing all the error
   * \return string - message where the information is specified with the following:
   * <lu>
   * <li>What
   * <li>Where
   * <li>When
   * <li>Remarks
   * <li>More information
   * </lu>
   */
  virtual std::string what ( ) throw ( ) {

    std::stringstream strMsg ;
    strMsg << "FecExceptionHandler: " << std::endl 
	   << "\t" << "What: " << errorMessage_ << std::endl 
	   << "\t" << "File: " << sourceFileName_ << " line: " << line_ << std::endl
	   << "\t" << "Method: " << method_ << std::endl
	   << "\t" << "When: " << ctime (&timestamp_) // << std::endl 
	   << "\t" << "Error code: " << errorCode_ << std::endl ;

    // -------------------------------------------------
    // not an error
    if ( (errorCode_ == 0) || (errorCode_ > SOFTWAREERRORCODE) ) {

      strMsg << "\t" << "Wrong error message, please report it to the developper list" ;

      return strMsg.str() ;
    }

    // -------------------------------------------------
    // low level layer error
    if (errorCode_ <= MAXDDERRORCODE) {

      //glue_fecdriver_get_error_message 
      std::string errorCodeMessage = FecExceptionHandler::dd_decode_error (errorCode_) ;
      
      // Hardware position + information
      std::string positionMessage = FecExceptionHandler::decodeKeyPosition(hardPosition_) ;
      
      // error message
      strMsg << "\t" << "Hardware access error from the FEC low level layers: " << std::endl
	     << "\t\t" << errorCodeMessage << std::endl
	     << "\t\t" << positionMessage ; // << std::endl ;

      // Information
      if (informationGiven_)
	strMsg << std::endl << "\t\t" << informationName_ << " = 0x" << std::hex << informationSup_ << std::dec ;

      return strMsg.str() ;
    }

    // ------------------------------------------------------------
    // High level hardware access
    if (errorCode_ <= MAXFECSUPERVISORERRORCODE) {

      // error message
      strMsg << "\t" << "Error from the FEC high level layers" ; //<< std::endl ;

      if (positionGiven_) {
	std::string positionMessage = FecExceptionHandler::decodeKeyPosition(hardPosition_) ;
	strMsg << std::endl << "\t\t" << positionMessage ;
      }
      
      if (informationGiven_)
	strMsg << std::endl << "\t\t" << informationName_ << " = 0x" << std::hex << informationSup_  ;
      
      return strMsg.str() ;
    }

    // -------------------------------------------------------
    // Database access
    if (errorCode_ <= MAXDATABASEERRORCODE) {

      // error message
      strMsg << "\t" << "Error from the database layer accesses" ; // << std::endl ;

      return strMsg.str() ;
    }
    
    // -------------------------------------------------------
    // XML parser
    if (errorCode_ <= MAXXMLERRORCODE) {
      
      // error message
      strMsg << "\t" << "Error from the database layer accesses" ; // << std::endl ;

      return strMsg.str() ;
    }

    // -------------------------------------------------------
    // XML parser
    if (errorCode_ <= SOFTWAREERRORCODE) {
      
      // error message
      strMsg << "\t" << "Error from XDAQ layers" ; // << std::endl ;

      return strMsg.str() ;
    }

    // -------------------------------------------------------
    // error message unknown
    //std::cerr << "Online running> unknown error code: " << errorCode_ << std::endl ;
    strMsg << "\t" << "Unknown error code: Error code not in correct range" ; // << std::endl ;

    return strMsg.str() ;
  }


  /**
   * \brief Returned an xml buffer containing all the information for the error
   * \return string - message
   */
  virtual std::string getXMLBuffer ( std::string fecHardwareId = "", unsigned int crateSlot = 0xFFFFFFFF ) throw ( ) {

    std::ostringstream xmlBuffer ;

    // ------------------------------------------
    // FEC hardware ID
    if (fecHardwareId != "") xmlBuffer << "<FECHARDID>" << fecHardwareId << "</FECHARDID>" ;

    // ------------------------------------------
    // Wrong error code
    if ( (errorCode_ == 0) || (errorCode_ > MAXXMLERRORCODE) ) return xmlBuffer.str() ;

    // ------------------------------------------
    // Buffer for FEC trigger problem
    if ( (errorCode_ == XDAQFEC_QPLLERROR) || (errorCode_ == XDAQFEC_QPLLERROR) || (errorCode_ == XDAQFEC_QPLLERROR) ) {
      xmlBuffer << "<TRIGGER>" << "TRUE" << "</TRIGGER>" ;
      xmlBuffer << "<SOFTWARETAGVERSION>" << softwareTagVersion_ << "</SOFTWARETAGVERSION>" ;

      return xmlBuffer.str() ;
    }

    // ------------------------------------------
    // FEC low level layer errors
    if (errorCode_ <= MAXDDERRORCODE) {

      // Hardware position
      if (positionGiven_) {

	if (crateSlot != 0xFFFFFFFF) xmlBuffer << "<CRATENUMBER>" << crateSlot << "</CRATENUMER>" ;
	xmlBuffer << "<FEC>" << getFecKey(hardPosition_) << "</FEC>" 
		  << "<RING>" << getRingKey(hardPosition_) << "</RING>" ;
	if (getCcuKey(hardPosition_) != 0) {
	  xmlBuffer << "<CCU>" << getCcuKey(hardPosition_) << "</CCU>" ;
	  if (getChannelKey(hardPosition_) != 0) {
	    xmlBuffer << "<I2CCHANNEL>" << getChannelKey(hardPosition_) << "</I2CCHANNEL>" ;
	    if (isi2cChannelCcu25(hardPosition_) || isi2cChannelOldCcu(hardPosition_)) 
	      xmlBuffer << "<I2CADDRESS>" << getAddressKey(hardPosition_) << "</I2CADDRESS>" ;
	  }
	}
      }

      // Information
      if (informationGiven_)
	xmlBuffer << "<INFORMATION=\"" << informationName_ << "\">" << informationSup_ << "</INFORMATION>" ;

      xmlBuffer << "<SOFTWARETAGVERSION>" << softwareTagVersion_ << "</SOFTWARETAGVERSION>" ;

      return xmlBuffer.str() ;
    }

    // ------------------------------------------
    // FEC high level layer errors
    if (errorCode_ <= MAXFECSUPERVISORERRORCODE) {
      
      // Hardware position
      if (positionGiven_) {

	if (crateSlot != 0xFFFFFFFF) xmlBuffer << "<CRATENUMBER>" << crateSlot << "</CRATENUMBER>" ;
	xmlBuffer << "<FEC>" << getFecKey(hardPosition_) << "</FEC>" 
		  << "<RING>" << getRingKey(hardPosition_) << "</RING>" ;
	if (getCcuKey(hardPosition_) != 0) {
	  xmlBuffer << "<CCU>" << getCcuKey(hardPosition_) << "</CCU>" ;
	  if (getChannelKey(hardPosition_) != 0) {
	    xmlBuffer << "<I2CCHANNEL>" << getChannelKey(hardPosition_) << "</I2CCHANNEL>" ;
	    if (isi2cChannelCcu25(hardPosition_) || isi2cChannelOldCcu(hardPosition_)) 
	      xmlBuffer << "I2CADDRESS>" << getAddressKey(hardPosition_) << "</I2CADDRESS>" ;
	  }
	}
      }

      if (informationGiven_)
	xmlBuffer << "<INFORMATION=" << informationName_ << ">" << informationSup_ << "</INFORMATION>" ;

      xmlBuffer << "<SOFTWARETAGVERSION>" << softwareTagVersion_ << "</SOFTWARETAGVERSION>" ;

      return xmlBuffer.str() ;
    }

    // -------------------------------------------
    // Database layer
    if (errorCode_ <= MAXDATABASEERRORCODE) {

      xmlBuffer << "<SOFTWARETAGVERSION>" << softwareTagVersion_ << "</SOFTWARETAGVERSION>" ;
      return xmlBuffer.str() ;
    }
    
    // ------------------------------------------
    // XML layer
    if (errorCode_ <= MAXXMLERRORCODE) {
      
      xmlBuffer << "<SOFTWARETAGVERSION>" << softwareTagVersion_ << "</SOFTWARETAGVERSION>" ;
      return xmlBuffer.str() ;
    }

    // -------------------------------------------------------
    // XML parser
    if (errorCode_ <= SOFTWAREERRORCODE) {
      
      // error message
      xmlBuffer << "<SOFTWARETAGVERSION>" << softwareTagVersion_ << "</SOFTWARETAGVERSION>" ;
      return xmlBuffer.str() ;
    }

    // ------------------------------------------
    // Unknown error
    std::cerr << "Online running> unknown error code: " << errorCode_ << std::endl ;
    return "" ;
  }

  /** Clone the exception
   */
  FecExceptionHandler *clone( ) {

    FecExceptionHandler *e = new FecExceptionHandler (*this) ;
    if (registersRead_ && (fecRingRegisters_ != NULL)) {
      e->fecRingRegisters_ = fecRingRegisters_->clone() ;
      e->registersRead_ = true ;
    }
    else {
      e->registersRead_ = false ;
      e->fecRingRegisters_ = NULL ;
    }

    return e ;
  }

  // --------------------------------------------------------------------------------------------------------------
  // Static method for error decoding
  // --------------------------------------------------------------------------------------------------------------

  /** \brief Format a XML buffer based on a key/partition/crate number
   */
  static std::string toFecXML ( std::string fecHardwareId, unsigned int crateId, keyType key ) {

    std::stringstream xmlBuffer ;

    xmlBuffer << "<CRATENUMBER>" << crateId << "</CRATENUMBER>" ;
    xmlBuffer << toFecXML(fecHardwareId, key) ;

    return xmlBuffer.str() ;
  }

  /** \brief Format a XML buffer based on a key/partition/crate number
   */
  static std::string toFecXML ( std::string fecHardwareId, keyType key ) {

    std::stringstream xmlBuffer ;

    xmlBuffer << "<FECHARDID>" << fecHardwareId << "<FECHARDID>" ;
    xmlBuffer << toFecXML(key) ;

    return xmlBuffer.str() ;
  }

  /** \brief Format a XML buffer based on a key/partition/crate number
   */
  static std::string toFecXML ( keyType key ) {

    std::stringstream xmlBuffer ;

    xmlBuffer << "<FEC>" << getFecKey(key) << "</FEC>" 
	      << "<RING>" << getRingKey(key) << "</RING>" ;
    if (getCcuKey(key) != 0) {
      xmlBuffer << "<CCU>" << getCcuKey(key) << "</CCU>" ;
      if (getChannelKey(key) != 0) {
	xmlBuffer << "<I2CCHANNEL>" << getChannelKey(key) << "</I2CCHANNEL>" ;
	if (isi2cChannelCcu25(key) || isi2cChannelOldCcu(key)) 
	  xmlBuffer << "I2CADDRESS>" << getAddressKey(key) << "</I2CADDRESS>" ;
      }
    }

    return xmlBuffer.str() ;
  }


  /** Return the CVS tag if existing or HEAD if empty
   */
  static std::string getCVSTag ( ) {

#define CVSTAGNAME "$Name:  $"
    std::string tagFromCVS = CVSTAGNAME ;
    std::string tag = "HEAD" ;

    if (tagFromCVS != CVSTAGNAME) {
      std::string::size_type ipass = tagFromCVS.find(":");
      std::string::size_type ipath = tagFromCVS.size() - 2 ;
      if (ipass != std::string::npos) {
	tag = tagFromCVS.substr(ipass+1,ipath) ;
	if (tag == "" || tag == " ") tag = "HEAD" ;
      }
      else tag = "HEAD" ;
    }
    
    return tag ;
  }

  /** Set the frame in case of i2c error
   */
  static void setFrame ( tscType8 *dst, tscType8 *src, bool dAck = false ) {

    unsigned int realSize = src[2] & FEC_LENGTH_2BYTES ? ((src[2] & 0x7F) * 256 + src[3] + 1 +3) : src[2]+3 ;
    //                                                                                    ^ size is two bytes
    //                                                                                       ^ dst src length
    //                                                                                         ^ if dAck is set

    // if the direct ack is set then copy one word
    if (dAck) realSize ++ ;
    if (realSize > DD_USER_MAX_MSG_LENGTH*4 ) realSize = DD_USER_MAX_MSG_LENGTH*4 ;

    // Copy
    memcpy (dst, src, realSize*sizeof(tscType8)) ;
  }

  /** \brief Return a string with the frame
   * \return string with frame
   */
  static std::string decodeFrame ( tscType8 *frame ) {

    return (FecRingRegisters::decodeFrame(frame)) ;
  }

  /*!
   * See the public file dd_perror.h for this API description.
   * This method is extracted from the PCI FEC device driver, do not modify it
   */
  static std::string dd_decode_error ( DD_TYPE_ERROR param_code ) {

    std::stringstream errorMessage ;

    switch (param_code) {
    case DD_NO_ERROR:
      errorMessage << "The function/command has been successfully executed";
      break;
    case DD_SIG_ON_IOCTL_SEM:
      errorMessage << "I was not able to raise the interlock management semaphore, when I entered the main ioctl() function. IOCTL() request have been rejected";
      break;
    case DD_BAD_IOC_DESTINATION:
      errorMessage << "IOCTL() destination (FEC | PLX | PCI) undefined. Looks like there is a problem with the magic numbers of the ioctl() calls defined in files pci_ioctl.h, plx_ioctl.h, fec_ioctl";
      break;
    case DD_NO_PCI_SOFTWARE_SUPPORT:
      errorMessage << "Your OS does not provides software support for PCI. The kernel of your computer must be compiled with PCI support option activated";
      break;
    case DD_NO_PCI_HARDWARE_SUPPORT:
      errorMessage << "Your computer does not appears to have, physically, a PCI bus";
      break;
    case DD_PCI_DEVICE_NOT_FOUND:
      errorMessage << "The PCI device identified by DEVICE_ID & VENDR_ID (values defined in file defines.h) has not been detected on the PCI bus of your computer";
      break;
    case DD_PCI_ERROR_READ_COMMAND_REG:
      errorMessage << "Error while reading the PCI COMMAND Register from the PCI board";
      break;
    case DD_PCI_KS_CANNOT_WRITE_COMMAND_REG_TO_US:
      errorMessage << "Kernel space cannot write the previously read value COMMAND register of the PCI board to user space";
      break;
    case DD_PCI_KS_CANNOT_WRITE_IRQ_NUMBER_TO_US:
      errorMessage << "Kernel space cannot write the previously read value IRQ number of the PCI board to user space";
      break;
    case DD_PCI_KS_CANNOT_WRITE_BA_ARRAY_TO_US:
      errorMessage << "Kernel space cannot write the previously read value base addresses array to user space";
      break;
    case DD_PCI_KS_CANNOT_READ_CTRL_REGISTER_FROM_US:
      errorMessage << "Kernel space cannot read the value of CONTROL register for the PCI board from user space";
      break;
    case DD_PCI_ERROR_WRITE_CTRL_REGISTER:
      errorMessage << "Error while writing in the CONTROL register of the PCI board";
      break;
    case DD_PCI_COMMAND_NOT_ASSOCIATED:
      errorMessage << "The ioctl() command has been recognized as a PCI command, but no action is associated to it";
      break;
    case DD_PLX_ERROR_REMAPPING_PLX_BA:
      errorMessage << "Remapping of PLX Base Addresse towards locale failed ; NULL pointer returned";
      break;
    case DD_PLX_COMMAND_NOT_ASSOCIATED:
      errorMessage << "The ioctl() command has been recognized as a PLX command, but no action is associated to it";
      break;
    case DD_FEC_COMMAND_NOT_ASSOCIATED:
      errorMessage << "The ioctl() command has been recognized as a FEC command, but no action is associated to it";
      break;
    case DD_ERROR_REGISTER_MODULE:
      errorMessage << "The device driver can not register himself to the kernel ; may be a /dev/devicename problem";
      break;
    case DD_RECEIVED_MSG_TOO_LONG:
      errorMessage << "A frame of size greater than DD_MAX_MSG_LEN items was read from fifo receive, and no EOFrame bitmask found. Aborting FIFO Receive readout";
      break;
    case DD_RECEIVED_WRONG_TRANS_NUMBER:
      errorMessage << "- motif : Transaction number of incoming frame is wrong (not in allowed limits [DD_MIN_TRANS_NUMBER,DD_MAX_TRANS_NUMBER]). This can mean that a corrupted frame have bypassed the software sanity checks, and is processed as if it was a good frame";
      break;
    case DD_RING_NOISY:
      errorMessage << "Some noise occured on the ring. Frame trashed";
      break;
    case DD_COPIED_NOT_READY:
      errorMessage << "The COPIED field of a given transaction (identified by its transaction/frame number) is not set to Copied OK. Either the transaction is still running or have failed";
      break;
    case DD_TIMEOUT_FEC_IN_USE:
      errorMessage << "A write method have not been successful ; the fec_in_use flag was always set high, so I haven't been able to write the frame in the FEC FIFO transmit";
      break;
    case DD_DATA_CORRUPT_ON_WRITE:
      errorMessage << "A write operation get a corrupted frame back (direct acknowledge)";
      break;
    case DD_NO_MORE_FREE_TRANSACTION_NUMBER:
      errorMessage << "255 transactions are already in the wait queue of the write method ; impossible to add more because no more transaction number is available";
      break;
    case DD_TOO_MUCH_STACKED_READ_REQUESTS:
      errorMessage << "Too much (more than DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS, value defined in file defines.h) transactions are already waiting for read access ; impossible to add more";
      break;
    case DD_TIMEOUT_TRANS_NUMBER_MNGR_IN_USE:
      errorMessage << "The transaction number manager can not be accessed ; flag is always busy. Aborting ioctl() call";
      break;
    case DD_FLAG_READ_COUNTER_UNCONSISTENT:
      errorMessage << "The variable used as flag/counter for stacking read requests is not consistent (<0 or >DD_MAX_ALLOWED_NUMBER_OF_SIMULTANEOUS_WAITING_TRANS). Software sanity problem, better unload/reload the driver";
      break;
    case DD_TOO_LONG_FRAME_LENGTH:
      errorMessage << "The frame length extracted from data read in user space is inconsistent (greater than DD_MAX_MSG_LENGTH). Possibly, a wrong frame have been sent to me by a user. Aborting ioctl() call";
      break;
    case DD_BAD_TRANSACTION_NUMBER_SENT_TO_READ:
      errorMessage << "The transaction number sent to the read method don't fit the limits [DD_MIN_TRANS_NUMBER,DD_MAX_TRANS_NUMBER]. Certainly an error from the user";
      break;
    case DD_CANNOT_INSTALL_IRQ_MANAGER:
      errorMessage << "I can not install the interrupt manager routine";
      break;
    case DD_CANNOT_EXIT_FROM_IRQ_READ_DATA:
      errorMessage << "Too many messages are occuring too fast on the ring, I am trapped in the interrupt manager. Interrupts are disabled for the next 7 seconds, and will automatically be re-enabled after this delay";
      break;
    case DD_ERROR_MODULE_IS_RESETTING:
      errorMessage << "I can't perform any operations right now ; a system reset is on the run and has not ended yet";
      break;
    case DD_RESET_BLOCKED_BY_WRITE:
      errorMessage << "Reset function can not succeed, because one or more remaining write tasks are inactives but not ended. Better unload/reload the driver";
      break;
    case DD_RESET_BLOCKED_BY_READ:
      errorMessage << "Reset function can not succeed, because one or more remaining read tasks are inactives but not ended. Better unload/reload the driver";
      break;
    case DD_INIT_TTCRX_WATCHDOG:
      errorMessage << "Watchdog was bypassed when waiting confirmation of ttcrx initialisation. Maybe your ttcrx has nevertheless been initialized, sometimes it takes a loooong time to confirm initialisation so this is more a warning rather than an error";
      break;
    case DD_TOO_MANY_PCI_BA:
      errorMessage << "TOO MANY base address registers (>6) defined for this PCI device. Check value of DD_NBR_OF_PCI_BASE_ADDRESSES in file defines.h";
      break;
    case DD_TIMEOUT_READ_REQUEST_IN_USE:
      errorMessage << "The read method can not increment the read_tasks_waiting counter ; access flag is always busy. Aborting";
      break;
    case DD_KS_CANNOT_READ_FROM_US_FOR_PLX_WRITE:
      errorMessage << "Kernel space can not read from the user space the values to write to the PLX. Software problem";
      break;
    case DD_KS_CANNOT_READ_FROM_US_FOR_PLX_READ:
      errorMessage << "Kernel space can not read from the user space the parameters needed to perform a read operation on the PLX. Software problem";
      break;
    case DD_KS_CANNOT_WRITE_TO_US_FOR_PLX_READ:
      errorMessage << "Kernel space can not write to the user space the values read from the PLX. Software problem";
      break;
    case DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_WRITE:
      errorMessage << "Kernel space can not read from the user space the values to write to the FEC. Software problem";
      break;
    case DD_CANNOT_READ_VALUE_FROM_US_FOR_FEC_READ:
      errorMessage << "Kernel space can not read from the user space the parameters needed to perform a read operation on the FEC. Software problem";
      break;
    case DD_CANNOT_WRITE_VALUE_TO_US_FOR_FEC_READ:
      errorMessage << "Kernel space can not write to the user space the values read from the FEC. Software problem";
      break;
    case DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_GETBA:
      errorMessage << "Kernel space can not write to the user space the value of the requested PLX Base Address. Software problem";
      break;
    case DD_FEC_CANNOT_READ_VALUE_FROM_US_FOR_RET_STATUS:
      errorMessage << "Kernel space can not read from the user space the values nedded to check the status of the transaction. Software problem";
      break;
    case DD_FEC_CANNOT_WRITE_VALUE_TO_US_FOR_RET_STATUS:
      errorMessage << "Kernel space can not write to the user space the value of the status of the transaction. Software problem";
      break;
    case DD_ERROR_IRQS_ARE_INVALIDATED:
      errorMessage << "IRQ's are currently disabled. This method can not be used when IRQ's are disabled";
      break;
    case DD_KS_CANNOT_READ_FRAME_FROM_US_FOR_WRITE_FRAME:
      errorMessage << "Kernel space can not read from the user space the frame to write on the ring. Software problem";
      break;
    case DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS1:
      errorMessage << "Write operation successful, but Kernel space can not write to the user space the value of the returned direct acknowledge frame";
      break;
    case DD_KS_CANNOT_WRITE_FRAME_BACK_TO_US_POS2:
      errorMessage << "Write operation un-successful (direct acknowledge frame corrupted), but Kernel space can not write to the user space the value of the returned direct acknowledge frame";
      break;
    case DD_KS_CANNOT_READ_FROM_US_FOR_READ_FRAME:
      errorMessage << "Kernel space can not read from the user space the informations nedded to execute the read_frame() method. Software problem";
      break;
    case DD_KS_CANNOT_WRITE_TO_US_FOR_READ_FRAME:
      errorMessage << "Kernel space can not write to the user space the requested frame. Software problem";
      break;
    case DD_KS_CANNOT_WRITE_TO_US_IN_HUB:
      errorMessage << "Kernel space is not allowed to write in the memory area you gave as argument to your ioctl() request. I need to be able to write there";
      break;
    case DD_KS_CANNOT_READ_FROM_US_IN_HUB:
      errorMessage << "Kernel space is not allowed to read data from the memory area you gave as argument to your ioctl() request. I need to be able to read data from there";
      break;
    case DD_MAIN_ERROR_MODULE_IS_RESETTING:
      errorMessage << "I can't perform any operations right now ; a system reset is on the run and has not ended yet";
      break;
    case DD_READ_FRAME_CORRUPTED:
      errorMessage << "A READ request have not been fully successful ; a frame has been received but is corrupted";
      break;
    case DD_INVALID_WRITE_AUTOTUNE_CMD:
      errorMessage << "An invalid command code have been sent to the auto-tune-write-method function of the glue layer";
      break;
    case DD_INVALID_READ_AUTOTUNE_CMD:
      errorMessage << "An invalid command code have been sent to the auto-tune-read-method function of the glue layer";
      break;
    case DD_FEC_CANNOT_READ_VALUE_FROM_US:
      errorMessage << "Kernel space can not read values from the user space. Software problem";
      break;
    case DD_FEC_CANNOT_WRITE_VALUE_TO_US:
      errorMessage << "Kernel space can not write values to the user space. Software problem";
      break;
    case DD_WRITE_OPERATION_FAILED:
      errorMessage << "A write operation failed ; no data at all was returned, not even a direct acknowledge returned frame";
      break;
    case DD_EXIT_WRITE_METHOD_ON_UNSTABLE_CONDITION:
      errorMessage << "System error. Analysis of received ack frame issued an unknown status. Please send a dump of frames and informations to fec-suport@ires.in2p3.fr";
      break;
    case DD_CANNOT_READ_DATA:
      errorMessage << "A READ request have not been successful at all ; exit from request on timeout";
      break;
    case DD_EXIT_READ_METHOD_ON_UNSTABLE_CONDITION:
      errorMessage << "System error. Analysis of received data frame issued an unknown status. Please send a dump of frames and informations to fec-suport@ires.in2p3.fr";
      break;
    case DD_DATA_OVERFLOW:
      errorMessage << "An incoming frame (direct ACK or data requested) have a size which overruns driver internals. Check file includes/defines.h for more specifications";
      break;
    case DD_INVALID_REM_CMD:
      errorMessage << "An invalid/unrecognized command has been sent to the ring error management function";
      break;
    case DD_FIFOTRA_RUNNING_FLAG_IS_ALWAYS_ON:
      errorMessage << "The FIFO TRANSMIT RUNNING bit of FEC register STATUS0 is always on. You will certainly have to reset your FEC";
      break;
    case DD_CANNOT_OPEN_FEC_DRIVER:
      errorMessage << "The FEC driver cannot be opened. Either the driver is not loaded, or your /dev/fecpmcxx file is has improper permissions";
      break;
    case DD_CANNOT_CLOSE_FEC_DRIVER:
      errorMessage << "The FEC driver cannot be close. Either the driver is not loaded, or the driver USER_COUNT is not equal to zero (i.e., someone is still using the driver)";
      break;
    case SYSTEM_DRIVER_NOT_LOADED:
      errorMessage << "You are making ioctl() attempts, but no driver is loaded/matches the file descriptor you use";
      break;
    case DD_NO_FEC_BOARD_DETECTED:
      errorMessage << "The current FEC driver has not found any physical PCI FEC board on this system - Driver access is disabled";
      break;
    default:
      errorMessage << " The error code %d is not defined in the function dd_perror" ;
      break;
    }

    // -----------------------------------
    // Add the error code
    errorMessage << " (error code = " << param_code << ")." ;

    // -----------------------------------
    return errorMessage.str() ;
  }
};

/** Macro that add to the exception the __PRETTY_FUNCTION__ and__LINE__
    FecExceptionHandler ( errorType errorCode,
 	 		  std::string errorMessage,
			  errorType faultSeverity,
			  std::string method,
			  unsigned int line ) ;
 */
#define RAISEFECEXCEPTIONHANDLER(errorCode,errorMessage,faultSeverity) \
(throw FecExceptionHandler(errorCode,errorMessage,faultSeverity,__PRETTY_FUNCTION__,__FILE__,__LINE__))

/** Macro that add to the exception the __PRETTY_FUNCTION__ and__LINE__
    FecExceptionHandler ( errorType errorCode,
			  std::string errorMessage,
			  errorType faultSeverity,
			  std::string method,
			  unsigned int line,
			  keyType hardPosition )
 */
#define RAISEFECEXCEPTIONHANDLER_HARDPOSITION(errorCode,errorMessage,faultSeverity,hardPosition) \
(throw FecExceptionHandler(errorCode,errorMessage,faultSeverity,__PRETTY_FUNCTION__,__FILE__,__LINE__,hardPosition))

/** Macro that add to the exception the __PRETTY_FUNCTION__ and__LINE__
    FecExceptionHandler ( errorType errorCode,
			  std::string errorMessage,
			  errorType faultSeverity,
			  std::string method,
			  unsigned int line,
			  keyType hardPosition,
			  std::string informationName,
   			  unsigned int informationSup )
 */
#define RAISEFECEXCEPTIONHANDLER_INFOSUP(errorCode,errorMessage,faultSeverity,hardPosition,informationName,informationSup) \
(throw FecExceptionHandler(errorCode,errorMessage,faultSeverity,__PRETTY_FUNCTION__,__FILE__,__LINE__,hardPosition,informationName,informationSup))

/** Macro that add to the exception the __PRETTY_FUNCTION__ and__LINE__
    FecExceptionHandler ( errorType errorCode,
			  std::string errorMessage,
			  errorType faultSeverity,
			  std::string method,
			  unsigned int line,
			  keyType hardPosition,
			  std::string informationName,
   			  unsigned int informationSup,
			  FecRingRegisters *fecRingRegisters,
			  tscType8 *frame,
			  tscType8 *dAck,
			  tscType8 *fAck )
 */
#define RAISEFECEXCEPTIONHANDLER_FECRING(errorCode,errorMessage,faultSeverity,hardPosition,informationName,informationSup,fecRingRegisters,frame,dAck,fAck) \
(throw FecExceptionHandler(errorCode,errorMessage,faultSeverity,__PRETTY_FUNCTION__,__FILE__,__LINE__,hardPosition,informationName,informationSup,fecRingRegisters,frame,dAck,fAck))

/** For new operation
 */
#define NEWFECEXCEPTIONHANDLER(errorCode,errorMessage,faultSeverity) \
(new FecExceptionHandler(errorCode,errorMessage,faultSeverity,__PRETTY_FUNCTION__,__FILE__,__LINE__))

/** For new operation
 */
#define NEWFECEXCEPTIONHANDLER_HARDPOSITION(errorCode,errorMessage,faultSeverity,hardPosition) \
(new FecExceptionHandler(errorCode,errorMessage,faultSeverity,__PRETTY_FUNCTION__,__FILE__,__LINE__,hardPosition))

/** For new operation
 */
#define NEWFECEXCEPTIONHANDLER_INFOSUP(errorCode,errorMessage,faultSeverity,hardPosition,informationName,informationSup) \
(new FecExceptionHandler(errorCode,errorMessage,faultSeverity,__PRETTY_FUNCTION__,__FILE__,__LINE__,hardPosition,informationName,informationSup))

#endif
