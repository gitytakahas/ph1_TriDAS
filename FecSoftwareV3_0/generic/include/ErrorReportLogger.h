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

#ifndef ERRORREPORTLOGGER_H
#define ERRORREPORTLOGGER_H

//#if defined(XDAQSHAREDOBJECT) 
//#  include "xdaq/ApplicationContext.h"
//#endif

#ifdef DATABASE
#  include <occi.h>
#endif

#ifdef TKDIAG
#  include <diagbag/DiagBagWizard.h>
#endif

/** Level of error
 */
#define ERROLEVELNUMBER 7
enum logLevelError { LOGFATAL, LOGERROR, LOGUSERINFO, LOGWARNING, LOGINFO, LOGDEBUG, LOGTRACE } ;
enum reConfEnum { RECONFCMD, RECONFSTOP, LOGDB } ;

/**
 * \class ErrorReportLogger
 * Different level are possible:
 * <ul>
 * <li> 0 = Fatal  : if the output is displayed then the error output is used
 * <li> 1 = Error  : if the output is displayed then the error output is used
 * <li> 2 = Warning: if the output is displayed then the error output is used
 * <li> 3 = Info:    if the output is displayed then the standard output is used
 * <li> 4 = Debug:   if the output is displayed then the standard output is used
 * <li> 5 = Trace:   same level than debug
 * </ul>
 * 
 */
class ErrorReportLogger {

 private:

  /** Source of the error, if XDAQ is used then the logger is create with that string
   */
  std::string strProcess_ ;

  /** FEC hardware ID if specified
   */
  std::string fecHardwareId_ ;

  /** Crate slot if specified
   */
  unsigned long crateId_ ;

  /** Use the output
   */
  bool displayOutput_ ;

  /** Use a logger (log4cplus for example if we are in XDAQ process)
   */
  bool useLogger_ ;

  /** Level of the display, note that for log4cplus all the messages are sent
   */
  enum logLevelError logLevel_ ;

#ifdef TKDIAG
  /** Error reporting through the diagnostic system
   */
  DiagBagWizard *diagService_;
#endif

#ifdef TKDIAG
  /** List of the possible level of errors
   */
  std::string diagBagErrorLevel[ERROLEVELNUMBER] ; // = {DIAGFATAL,DIAGERROR,DIAGUSERINFO,DIAGWARN,DIAGINFO,DIAGDEBUG,DIAGTRACE} ;

  /** List of the possible reconfiguration possibilities
   */
  std::string diagBagReconfName[3] ; // = {"RECONFCMD","RECONFSTOP","LOGDB"} ;
#endif

  /** send or not to the diagnostic system
   */
  bool sendToErrorDispatcher_ ;

 public:
  
  /** \brief Error reporting constructor
   * \param strProcess - source of the error
   * \param displayOutput - display on the console (from the level specified 
   * \param useLogger - use the logger if it is set
   * \param diagService - diagnostic system error reporting
   */
#ifdef TKDIAG
  ErrorReportLogger ( std::string strProcess = "", bool displayOutput = false, bool useLogger = true, logLevelError logLevel = LOGWARNING, 
		      DiagBagWizard *diagService = NULL ):
#else
  ErrorReportLogger ( std::string strProcess = "", bool displayOutput = false, bool useLogger = true, logLevelError logLevel = LOGWARNING ):
#endif
    strProcess_(strProcess), displayOutput_(displayOutput), useLogger_(useLogger), logLevel_(logLevel) {

#ifdef DEBUGMSGERROR
    std::cout << "Log level to display = " << logLevel << std::endl ;
#endif

#ifdef TKDIAG
#  ifdef DEBUGMSGERROR
    if (diagService != NULL) std::cout << "Use the diagnostic system" << std::endl ;
#  endif
    diagService_ = diagService ;
    if (diagService != NULL) sendToErrorDispatcher_ = true ;

    // Disable internal display
    if (diagService != NULL) { 
      diagService_->setConsoleDumpEnabled(false) ; 
      //diagService_->setConsoleDumpLevel(DIAGTRACE) ; 
    }

    diagBagErrorLevel[LOGFATAL]    = DIAGFATAL    ;
    diagBagErrorLevel[LOGERROR]    = DIAGERROR    ;
    diagBagErrorLevel[LOGUSERINFO] = DIAGUSERINFO ;
    diagBagErrorLevel[LOGWARNING]  = DIAGWARN     ;
    diagBagErrorLevel[LOGINFO]     = DIAGINFO     ;
    diagBagErrorLevel[LOGDEBUG]    = DIAGDEBUG    ;
    diagBagErrorLevel[LOGTRACE]    = DIAGTRACE    ;

    diagBagReconfName[RECONFCMD]   = "RECONFCMD"  ;
    diagBagReconfName[RECONFSTOP]  = "RECONFSTOP" ;
    diagBagReconfName[LOGDB]       = "LOGDB"      ;
    
#endif

    fecHardwareId_ = "" ;
    crateId_ = 0xFFFFFFFF ;
  }

#ifdef TKDIAG
  /** Set the diagnostic service
   */
  void setDiagServer ( DiagBagWizard *diagService ) {

    diagService_ = diagService ;
  }
#endif

  /** \brief Set the source process string
   * \param strProcess - source of the error
   */
  void setStrProcess ( std::string strProcess ) {

    strProcess_ = strProcess ;
  }

  /** \brief Get the source process string
   * \return strProcess - source of the error
   */
  std::string getStrProcess ( ) {

    return strProcess_ ;
  }

  /** \brief Set the use or not of the output
   * \param displayOutput - true to display
   */
  void setDisplayOutput ( bool displayOutput ) {
    displayOutput_ = displayOutput ;
  }

  /** \brief Get the use or not of the output
   */
  bool getDisplayOutput ( ) {
    return displayOutput_ ;
  }

  /** \brief set the logger used
   */
  void setUseLogger ( bool useLogger ) {
    useLogger_ = useLogger ;
  }

  /** \brief return the used of the logger
   */
  bool getUseLogger ( ) {
    return useLogger_ ;
  }

  /** \brief set the log level (for display only)
   * \param logLevel - level of the log
   */
  void setlogLevel ( logLevelError logLevel ){
    logLevel_ = logLevel ;
  }

  /** \brief return the level of the log (for display only)
   */
  logLevelError getLogLevel ( ) {

    return logLevel_ ;
  }

  /** \brief send or not to the error dispatcher
   */
  void setSendToErrorDispatcher ( bool sendToErrorDispatcher ) {
    sendToErrorDispatcher_ = sendToErrorDispatcher ;
  }

  /** \brief check if the errors are sent to the error dispatcher
   */
  bool getSendToErrorDispatcher ( ) {
    return sendToErrorDispatcher_ ;
  }

  /** \brief set the FEC hardwware ID 
   * \param fecHardwareId - FEC hardware ID
   */
  void setFecHardwareId ( std::string fecHardwareId ) {

    fecHardwareId_ = fecHardwareId ;
  }

  /** \brief get the FEC hardware ID
   * \return FEC hardware ID
   */
  std::string getFecHardwareId ( ) {

    return fecHardwareId_ ;
  }

  /** \brief set the crate ID
   * \param crateId - crate ID
   */
  void setCrateId ( unsigned int crateId ) {

    crateId_ = crateId ;
  }

  /** \brief get the crate ID
   * \return crateId
   */
  unsigned int getCrateId ( ) {

    return crateId_ ;
  }

  /** \brief Report an error to the different logger set
   * \param errorMessage - error message
   * \param level - level of the error
   * \param firstlevel - if the diagnostic system is used then the error is sent to the first level error manager, possible values are:
   * <ul><li>0: not use the diag 1st level
   * <li>1: to use the diag 1st level (cmd = RECONFCMD)
   * <li>-1: to stop the diag 1st level (cmd = RECONFSTOP)
   * </ul>
   * \param errorCode - error code
   * \param faultState - fault state (STEADY, ACTION)
   */
  void errorReport ( std::string errorMessage, logLevelError level, // = LOGWARNING, 
		     int firstLevel = 0, unsigned int errorCode = 0, std::string faultState = "ACTION" ) {

    std::ostringstream msgDisplay ; msgDisplay << strProcess_ << ": " << errorMessage ;

#ifdef XDAQAPPLICATION
    if (useLogger_) {
#ifdef TKDIAG
      if ( (diagService_ != NULL) && sendToErrorDispatcher_ ) {

	if ( (firstLevel == 1) || (firstLevel == -1) ) { 	  // Send to the first level
	  if (firstLevel == 1)
	    diagService_->reportError(msgDisplay.str(), diagBagErrorLevel[level], diagBagReconfName[RECONFCMD], errorCode, faultState) ;
	  else
	    diagService_->reportError(msgDisplay.str(), diagBagErrorLevel[level], diagBagReconfName[RECONFSTOP], errorCode, faultState) ;
	}
	else           // Send to the error dispatcher only
	  diagService_->reportError(msgDisplay.str(), diagBagErrorLevel[level], diagBagReconfName[LOGDB], errorCode, faultState) ;
      }
      else 
#endif
	{
	  Logger logger = Logger::getInstance(strProcess_);
	  switch (level) {
	  case LOGFATAL:
	    LOG4CPLUS_FATAL ( logger, msgDisplay.str().c_str() ) ;
	    break ;
	  case LOGERROR:
	    LOG4CPLUS_ERROR ( logger, msgDisplay.str().c_str() ) ;
	    break ;
	  case LOGUSERINFO:
	    LOG4CPLUS_INFO ( logger, msgDisplay.str().c_str() ) ;
	    break ;
	  case LOGWARNING:
	    LOG4CPLUS_WARN ( logger, msgDisplay.str().c_str() ) ;
	    break ;
	  case LOGINFO:
	    LOG4CPLUS_INFO ( logger, msgDisplay.str().c_str() ) ;
	    break ;
	  case LOGDEBUG:
	    LOG4CPLUS_DEBUG ( logger, msgDisplay.str().c_str() ) ;
	    break ;
	  case LOGTRACE:
	    LOG4CPLUS_TRACE ( logger, msgDisplay.str().c_str() ) ;
	    break ;
	  }
	}
    }
#endif // XDAQAPPLICATION

    //std::cout << "level = " << level << std::endl ;
    //std::cout << "loglevel = " << logLevel_ << std::endl ;

    if (displayOutput_) {
      switch (level) {
      case LOGFATAL:
	if (logLevel_ >= LOGFATAL) {
	  std::cerr << "****************************** FATAL ERROR *********************" << std::endl ;
	  std::cerr << msgDisplay.str() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGERROR:
	if (logLevel_ >= LOGERROR) {
	  std::cerr << "******************************** ERROR *************************" << std::endl ;
	  std::cerr << msgDisplay.str() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGUSERINFO:
	if (logLevel_ >= LOGUSERINFO) {
	  std::cout << "USER INFORMATION: " << msgDisplay.str() << std::endl ;
	}
	break ;
      case LOGWARNING:
	if (logLevel_ >= LOGWARNING) {
	  std::cerr << "******************************** WARNING ***********************" << std::endl ;
	  std::cerr << msgDisplay.str() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGINFO:
	if (logLevel_ >= LOGINFO) {
	  std::cout << "INFORMATION: " << msgDisplay.str() << std::endl ;
	}
	break ;
      case LOGDEBUG:
	if (logLevel_ >= LOGDEBUG) {
	  std::cout << "DEBUG Information: " << msgDisplay.str() << std::endl ;
	}
        break ;
      case LOGTRACE:
        if (logLevel_ >= LOGTRACE) {
	  std::cout << "TRACE Information: " << msgDisplay.str() << std::endl ;
	}
        break ;
      }
    }
  }

#ifdef XDAQAPPLICATION
  /**
   * \param errorMessage - error message
   * \param e - XDAQ exception
   * \param level - level of the error
   * \param firstlevel - if the diagnostic system is used then the error is sent to the first level error manager, possible values are:
   * <ul><li>0: not use the diag 1st level
   * <li>1: to use the diag 1st level (cmd = RECONFCMD)
   * <li>-1: to stop the diag 1st level (cmd = RECONFSTOP)
   * </ul>
   * \param errorCode - error code
   * \param faultState - fault state (STEADY, ACTION)
   */
  void errorReport ( std::string errorMessage, xdaq::exception::Exception e, logLevelError level, // = LOGINFO, 
		     int firstLevel = 0, unsigned int errorCode = 0, std::string faultState = "ACTION" ) {

    std::ostringstream msgDisplay ; msgDisplay << strProcess_ << ": " << errorMessage ;

#ifdef XDAQAPPLICATION
    if (useLogger_) {
#ifdef TKDIAG
      if ( (diagService_ != NULL) && sendToErrorDispatcher_ ) {

	if ( (firstLevel == 1) || (firstLevel == -1) ) { 	  // Send to the first level
	  if (firstLevel == 1)
	    diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[RECONFCMD], errorCode, faultState) ;
	  else
	    diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[RECONFSTOP], errorCode, faultState) ;
	}
	else           // Send to the error dispatcher only
	  diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[LOGDB], errorCode, faultState) ;
      }
      else 
#endif
	{
	  Logger logger = Logger::getInstance(strProcess_);
	  switch (level) {
	  case LOGFATAL:
	    LOG4CPLUS_FATAL ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGERROR:
	    LOG4CPLUS_ERROR ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGUSERINFO:
	    LOG4CPLUS_INFO ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGWARNING:
	    LOG4CPLUS_WARN ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGINFO:
	    LOG4CPLUS_INFO ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGDEBUG:
	    LOG4CPLUS_DEBUG ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGTRACE:
	    LOG4CPLUS_TRACE ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  }
	}
    }
#endif // XDAQAPPLICATION

    if (displayOutput_) {
      switch (level) {
      case LOGFATAL:
	if (logLevel_ >= LOGFATAL) {
	  std::cerr << "****************************** FATAL ERROR *********************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGERROR:
	if (logLevel_ >= LOGERROR) {
	  std::cerr << "******************************** ERROR *************************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGUSERINFO:
	if (logLevel_ >= LOGUSERINFO) {
	  std::cout << "INFORMATION: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGWARNING:
	if (logLevel_ >= LOGWARNING) {
	  std::cerr << "******************************** WARNING ***********************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGINFO:
	if (logLevel_ >= LOGINFO) {
	  std::cout << "INFORMATION: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGDEBUG:
	if (logLevel_ >= LOGDEBUG) {
	  std::cout << "DEBUG Information: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGTRACE:
	if (logLevel_ >= LOGTRACE) {
	  std::cout << "TRACE Information: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      }
    }
  }
#endif

#ifdef DATABASE
  /**
   * \param errorMessage - error message
   * \param e - oracle occi exception
   * \param level - level of the error
   * \param firstlevel - if the diagnostic system is used then the error is sent to the first level error manager, possible values are:
   * <ul><li>0: not use the diag 1st level
   * <li>1: to use the diag 1st level (cmd = RECONFCMD)
   * <li>-1: to stop the diag 1st level (cmd = RECONFSTOP)
   * </ul>
   * \param errorCode - error code
   * \param faultState - fault state (STEADY, ACTION)
   */
  void errorReport ( std::string errorMessage, oracle::occi::SQLException e, logLevelError level, // = LOGINFO, 
		     int firstLevel = 0, std::string faultState = "ACTION" ) {

    std::ostringstream msgDisplay ; msgDisplay << strProcess_ << ": " << errorMessage ;

#ifdef XDAQAPPLICATION
    if (useLogger_) {
#ifdef TKDIAG
      if ( (diagService_ != NULL) && sendToErrorDispatcher_ ) {

	unsigned int errorCode = e.getErrorCode() ;

	if ( (firstLevel == 1) || (firstLevel == -1) ) { 	  // Send to the first level
	  if (firstLevel == 1)
	    diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[RECONFCMD], errorCode, faultState) ;
	  else
	    diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[RECONFSTOP], errorCode, faultState) ;
	}
	else           // Send to the error dispatcher only
	  diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[LOGDB], errorCode, faultState) ;
      }
      else 
#endif
	{
	  Logger logger = Logger::getInstance(strProcess_);
	  switch (level) {
	  case LOGFATAL:
	    LOG4CPLUS_FATAL ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGERROR:
	    LOG4CPLUS_ERROR ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGUSERINFO:
	    LOG4CPLUS_INFO ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGWARNING:
	    LOG4CPLUS_WARN ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGINFO:
	    LOG4CPLUS_INFO ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGDEBUG:
	    LOG4CPLUS_DEBUG ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  case LOGTRACE:
	    LOG4CPLUS_TRACE ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what()) ) ;
	    break ;
	  }
	}
    }
#endif // XDAQAPPLICATION

    if (displayOutput_) {
      switch (level) {
      case LOGFATAL:
	if (logLevel_ >= LOGFATAL) {
	  std::cerr << "****************************** FATAL ERROR *********************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGERROR:
	if (logLevel_ >= LOGERROR) {
	  std::cerr << "******************************** ERROR *************************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGUSERINFO:
	if (logLevel_ >= LOGUSERINFO) {
	  std::cout << "INFORMATION: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGWARNING:
	if (logLevel_ >= LOGWARNING) {
	  std::cerr << "******************************** WARNING ***********************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGINFO:
	if (logLevel_ >= LOGINFO) {
	  std::cout << "INFORMATION: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGDEBUG:
	if (logLevel_ >= LOGDEBUG) {
	  std::cout << "DEBUG Information: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGTRACE:
	if (logLevel_ >= LOGTRACE) {
	  std::cout << "TRACE Information: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      }
    }
  }
#endif

  /**
   * \param errorMessage - error message
   * \param e - FEC exception
   * \param level - level of the error
   * \param firstlevel - if the diagnostic system is used then the error is sent to the first level error manager, possible values are:
   * <ul><li>0: not use the diag 1st level
   * <li>1: to use the diag 1st level (cmd = RECONFCMD)
   * <li>-1: to stop the diag 1st level (cmd = RECONFSTOP)
   * </ul>
   * \param errorCode - error code
   * \param faultState - fault state (STEADY, ACTION)
   */
  void errorReport ( std::string errorMessage, FecExceptionHandler e, logLevelError level, // = LOGINFO, 
		     int firstLevel = 0, std::string faultState = "ACTION" ) {

    std::ostringstream msgDisplay ; msgDisplay << strProcess_ << ": " << errorMessage ;

#ifdef XDAQAPPLICATION
    if (useLogger_) {
#ifdef TKDIAG
      if ( (diagService_ != NULL) && sendToErrorDispatcher_ ) {

	unsigned int errorCode = e.getErrorCode() ;

	if ( (firstLevel == 1) || (firstLevel == -1) ) { 	  // Send to the first level
	  if (firstLevel == 1)
	    diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[RECONFCMD], errorCode, faultState, e.getXMLBuffer(fecHardwareId_, crateId_)) ;
	  else
	    diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[RECONFSTOP], errorCode, faultState, e.getXMLBuffer(fecHardwareId_, crateId_)) ;
	}
	else           // Send to the error dispatcher only
	  diagService_->reportError(msgDisplay.str() + ": " + e.what(), diagBagErrorLevel[level], diagBagReconfName[LOGDB], errorCode, faultState, e.getXMLBuffer(fecHardwareId_, crateId_)) ;
      }
      else 
#endif
	{
	  Logger logger = Logger::getInstance(strProcess_);
	  switch (level) {
	  case LOGFATAL:
	    LOG4CPLUS_FATAL ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what().c_str()) ) ;
	    break ;
	  case LOGERROR:
	    LOG4CPLUS_ERROR ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what().c_str()) ) ;
	    break ;
	  case LOGUSERINFO:
	    LOG4CPLUS_INFO ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what().c_str()) ) ;
	    break ;
	  case LOGWARNING:
	    LOG4CPLUS_WARN ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what().c_str()) ) ;
	    break ;
	  case LOGINFO:
	    LOG4CPLUS_INFO ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what().c_str()) ) ;
	    break ;
	  case LOGDEBUG:
	    LOG4CPLUS_DEBUG ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what().c_str()) ) ;
	    break ;
	  case LOGTRACE:
	    LOG4CPLUS_TRACE ( logger, toolbox::toString("%s: %s", msgDisplay.str().c_str(), e.what().c_str()) ) ;
	    break ;
	  }
	}
    }
#endif // XDAQAPPLICATION

    if (displayOutput_) {
      switch (level) {
      case LOGFATAL:
	if (logLevel_ >= LOGFATAL) {
	  std::cerr << "****************************** FATAL ERROR *********************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGERROR:
	if (logLevel_ >= LOGERROR) {
	  std::cerr << "******************************** ERROR *************************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGUSERINFO:
	if (logLevel_ >= LOGUSERINFO) {
	  std::cout << "INFORMATION: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGWARNING:
	if (logLevel_ >= LOGWARNING) {
	  std::cerr << "******************************** WARNING ***********************" << std::endl ;
	  std::cerr << msgDisplay.str() << ": " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	break ;
      case LOGINFO:
	if (logLevel_ >= LOGINFO) {
	  std::cout << "INFORMATION: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGDEBUG:
	if (logLevel_ >= LOGDEBUG) {
	  std::cout << "DEBUG Information: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      case LOGTRACE:
	if (logLevel_ >= LOGTRACE) {
	  std::cout << "TRACE Information: " << msgDisplay.str() << ": " << e.what() << std::endl ;
	}
	break ;
      }
    }
  } 
};
#endif
