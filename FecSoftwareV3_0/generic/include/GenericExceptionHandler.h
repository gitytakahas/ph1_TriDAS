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
#ifndef GENERICEXCEPTIONHANDLER_H
#define GENERICEXCEPTIONHANDLER_H

#include <iostream>
#include <exception>
#include <string>

#include <cstdlib>

  //#include <string.h> // sprintf
  //#include <time.h>   // time

#include "keyType.h"
#include "tscTypes.h"
#include "errorCodeDefinition.h"

/**
 * \class GenericExceptionHandler
 * This class manage all the exceptions. 
 * For each exception, you have several tags:
 * <ul>
 * <li>System Number
 * <li>Sub system Number
 * <li>A timestamp is added automatically
 * <li>A user and a developper message must be added also
 * </ul>
 * \author Frederic Drouhin
 * \date May 2003
 * \brief Generic exception class manager
 * \warning this class replace all the previous exceptions: ccuChannelAccessError and fecHardAccessError
 */
class GenericExceptionHandler: public std::exception {

 protected:
  errorType errorCode_ ;
  std::string userErrorMessage_ ;
  std::string developperErrorMessage_ ;
  errorType faultSeverity_ ;
  unsigned int subSystemNumber_ ;
  unsigned int systemNumber_ ;
  time_t timestamp_ ;

 public:

  /**
   * \brief Default constructor for inheritance
   */
  GenericExceptionHandler ( ) throw ( ) {

    timestamp_ = time (NULL) ;

    errorCode_ = 0 ;
    userErrorMessage_ = "No Error, problem from software" ;
    developperErrorMessage_ = "GenericExceptionHandler called by default" ;
    faultSeverity_ = 0 ;
    subSystemNumber_ = 0 ;
  }

  /**
   * \brief New Exception defined
   * \param systemNumber - value associate to a system (FEC/FED/...)
   * \param subSystemNumber - value associate to a sub-system of the previous system
   * (database/device driver/... for the FEC)
   * \param faultSeverity - severity associate to the error
   * \param errorCode - error code (the values are part in sub-system part)
   * \param userMessage - message dedicated to the user
   * \param developperMessage - message dedicated to the developper
   * \see errorCodeDefinition.h for all the values
   */
  GenericExceptionHandler ( unsigned int systemNumber, 
                            unsigned int subSystemNumber, 
                            errorType faultSeverity, 
                            errorType errorCode, 
                            std::string userMessage, 
                            std::string developperMessage = "" ) throw ( ) {

    timestamp_ = time (NULL) ;

    systemNumber_ = systemNumber ;
    subSystemNumber_ = subSystemNumber ;
    
    errorCode_ = errorCode ;
    faultSeverity_ = faultSeverity ;

    developperErrorMessage_ = developperMessage ;
    userErrorMessage_ = userMessage ;

    if ( (userErrorMessage_ == "") || (userErrorMessage_.size() == 0) ) userErrorMessage_ = developperErrorMessage_ ;
    if ( (developperErrorMessage_ == "") || (developperErrorMessage_.size() == 0) ) developperErrorMessage_ = userErrorMessage_ ;
  }
  
  /** Nothing
   */
  virtual ~GenericExceptionHandler ( ) throw ( ) { }

  /**
   * Display the last error
   */
  virtual void displayError ( ) throw ( ) {    

    std::cout << "------------------ Exception error ---------------" << std::endl ;
    std::cout << what ( ) << std::endl ;
    std::cout << "--------------------------------------------------" << std::endl ;
  }

  /**
   * \brief Returned a message containing all the error
   * \return string - message
   */
  virtual std::string what ( ) throw ( ) {

    char msg[MAXCHARMSG] ;

    std::sprintf (msg, "%s%s%s%ld",
             ctime (&timestamp_),
             developperErrorMessage_.c_str(),
             "\nGeneric functions error code: ",
             (unsigned long)errorCode_) ;

    return msg ;
  }

  /**
   * \brief Returned the error code
   * \return error code value
   */
  virtual errorType getErrorCode ( ) throw ( ) {

    return (errorCode_) ;
  }

  /** 
   * \brief Return the sub system number
   * The values can be:
   * \return tscType8 - system number
   */
  virtual unsigned int getSystemNumber ( ) throw ( ) {
    
    return (systemNumber_) ;
  }

  /** 
   * \brief Return the sub system number
   * \return tscType8 - subsystem number
   * \see getErrorCode method
   */
  virtual unsigned int getSubSystemNumber (  ) throw ( ) {

    return (subSystemNumber_) ;
  }

  /**
   * \brief Return the error severity 
   * \return the severity code error
   */
  virtual errorType getSeverityError ( ) throw ( ) {

    return (faultSeverity_) ;
  }

  /**
   * \brief Return the timestamp
   * \return the timestamp
   */
  virtual time_t getTimeStamp ( ) throw ( ) {

    return (timestamp_) ;
  }

  /**
   * \brief return the user message
   * \return user message
   */
  virtual std::string getErrorUserMessage ( ) throw ( ) {

    return (userErrorMessage_) ;
  }

  /**
   * \brief return the developper message
   * \return developper message
   */
  virtual std::string getErrorDevelopperMessage ( ) throw ( ){

    return (developperErrorMessage_) ;
  }

  /** 
   * \brief return the message from the exception
   * \return message
   */
  virtual std::string getMessage ( ) throw ( ) {

    return (userErrorMessage_) ;
  }
  
};

#endif
