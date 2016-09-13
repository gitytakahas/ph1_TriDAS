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

//#define DEBUGMSGERROR

// C++ includes
#include <iostream>
#include <cstdlib>
#include "stdlib.h"
#include "stdio.h"

// XDAQ includes
#include <pthread.h>
#include "toolbox/fsm/FailedEvent.h"

// STD includes
#include <iostream>
#include <stdexcept>

#include "tscTypes.h"
#include "keyType.h"
#include "HashTable.h"


// XDAQ includes
#include "CrateController.h"

// FEC includes
#include "FecExceptionHandler.h"
#include "stringConv.h"
#if defined(BUSVMECAENUSB) || defined(BUSVMECAENPCI) || defined(BUSVMESBS)
#include "FecVmeRingDevice.h"
#include "CCSTrigger.hh"        // for CCS trigger part
#endif
#if defined(BUSPCIFEC)
#include "FecPciRingDevice.h"
#endif
#if defined(BUSUSBFEC)
#include "FecUsbRingDevice.h"
#endif
#include "tscTypes.h"
#include "TkRingDescription.h"

// FED includes
#include "Fed9UUtils.hh"
#include "Fed9UDeviceFactoryLib.hh"
#include "Fed9ULib.hh"

// This define is just there in order to set the class
#define CLASSNAME    CrateController
#define CLASSNAMESTR std::string("CrateController")
#define __CrateControllerClass__
#include "FecDetectionUpload.h"
#include "DisplayCCUModules.h"

#include "Fed9UGlobalFunctions.hh"

#ifndef UNKNOWN
#  define UNKNOWN "UNKNOWN"
#endif

#include <sys/time.h>
#include <iostream>

bool CrateController::timeDiffMs(timeval &first, timeval &second, int msecdelay) {
  struct timeval lapsed;
  int secdelay = int(msecdelay/1000.);
  msecdelay = msecdelay - (1000 * secdelay);
  if (first.tv_usec > second.tv_usec) {
    second.tv_usec += 1000000;
    second.tv_sec--;
  }
  lapsed.tv_usec = second.tv_usec - first.tv_usec;
  lapsed.tv_sec  = second.tv_sec  - first.tv_sec;
  return ((lapsed.tv_sec>=secdelay)&&(lapsed.tv_usec>=(msecdelay*1000)));
}


/** MACRO for XDAQ
 */
XDAQ_INSTANTIATOR_IMPL( CrateController );

// ----------------------------------------------------------
// Crate reset to be integrated in the FED part
// VME slot start, VME slot stop to be added in the FED part
// ----------------------------------------------------------


/** This constructor is used in order to initialise the different exported parameters parameters. 
 * State Initialise
 * Exported parameters:
 * <ul>
 * <li><i>fireItemAvailable(std::string("DbLogin"),&dbLogin_)</i>: Database login
 * <li><i>fireItemAvailable(std::string("DbPassword"),&dbPasswd_)</i>: Database password
 * <li><i>fireItemAvailable(std::string("DbPath"),&dbPath_)</i>: Database path
 * <li><i>fireItemAvailable(std::string("PartitionName"),&partitionName_)</i>: Partition name should be different than any other DB partition name
 * <li><i>fireItemAvailable(std::string("UploadFile"),&doUploadInFile_)</i>: upload in a file
 * <li><i>fireItemAvailable(std::string("UploadDatabase"),&doUploadInDatabase_)</i>: upload in a database
 * <li><i>fireItemAvailable(std::string("VMESlotBegin"),&vmeSlotBegin_)</i>:scan a part of the crate (begin of the enumeration)
 * <li><i>fireItemAvailable(std::string("VMESlotEnd"),&vmeSlotEnd_)</i>: scan a part of the crate (end of the enumeration)
 * <li><i>fireItemAvailable(std::string("StrBusAdapter"),&strBusAdapter_)</i>: which bus adapter
 * <li><i>fireItemAvailable(std::string("ScanFEDs"),&doScanFEDs_)</i>: you cannot scan a FEC then a FED or the reverse
 * <li><i>fireItemAvailable(std::string("FEDVmeFileName"),&fedVmeFileName_)</i>:
 * <li><i>fireItemAvailable(std::string("FEDStartIdOffset"),&fedStartIdOffset_)</i>: ID of the FED in database
 * <li><i>fireItemAvailable(std::string("FEDTemplateFileName"),&fedTemplateFileName_)</i>: Template file for the FED
 * <li><i>fireItemAvailable(std::string("ScanFECs"),&doScanFECs_)</i>: scan for FECs
 * <li><i>fireItemAvailable(std::string("DOHCalibration"),&doCalibrationDOH_)</i>: scan for FECs
 * <li><i>fireItemAvailable(std::string("ScanRedundancy"),&doScanRedundancy_)</i>: scan for redundancy
 * <li><i>fireItemAvailable(std::string("CheckDcuConversion"),&doCheckTkDcuConversion_)</i>: check that the conversion factors are existing for the DCUs
 * <li><i>fireItemAvailable(std::string("RemoveBadModules"),&removeBadModules_)</i>: remove the devices where the module has no PLL or AOH or MUX or no APVs
 * <li><i>fireItemAvailable(std::string("FECVmeFileName"),&fecVmeFileName_)</i>: FEC address table or FEC hardware list
 * <li><i>fireItemAvailable(std::string("FECVmeFileNamePnP"),&fecVmeFileNamePnP_)</i>: Plug and play for FEC (if it is used)
 * <li><i>fireItemAvailable(std::string("FECTemplateFileName"),&fecTemplateFileName_)</i>: template of the devices
 * <lu>
 */
CrateController::CrateController ( xdaq::ApplicationStub* stub ): TrackerCommandListener (stub) {

  // -----------------------------------------------
  displayDebugMessage_       = false ;
  displayFrameErrorMessages_ = true  ;
  //displayFrameErrorMessages_ = false ;

  // -----------------------------------------------
  // Error report and diagnostic system
#ifdef TKDIAG
  // Diag bag

  diagService_ = new DiagBagWizard((getApplicationDescriptor()->getClassName() + "DiagLvlOne") ,
				   this->getApplicationLogger(),
				   getApplicationDescriptor()->getClassName(),
				   getApplicationDescriptor()->getInstance(),
				   getApplicationDescriptor()->getLocalId(),
				   (xdaq::WebApplication *)this,
				   DIAG_TRACKER_SYSTEMID,
				   DIAG_TRACKERCRATECONTROLLER_SUBSYSTEMID);

  // Declare the application as a user application
  DIAG_DECLARE_USER_APP ; 
  errorReportLogger_ = new ErrorReportLogger ("CrateController " + toString(getApplicationDescriptor()->getInstance()), displayDebugMessage_, true, 
					      LOGDEBUG, diagService_) ;
#else
  errorReportLogger_ = new ErrorReportLogger ("CrateController " + toString(getApplicationDescriptor()->getInstance()), displayDebugMessage_, true, 
					      LOGDEBUG) ;
#endif

  // Software tag version from CVS
  softwareTagVersion_ = FecExceptionHandler::getCVSTag() ;

  // Initialise the objects for the accesses
  deviceFactory_ = NULL ;

  // ------------------------------------------------------------------
  // export paramater to run control, direct binding
  // run control can read and write all exported variables
  // directly (no need fro get and put from the user).

  // Database path
  dbLogin_  = "nil" ; getApplicationInfoSpace()->fireItemAvailable(std::string("DbLogin"),&dbLogin_);
  dbPasswd_ = "nil" ; getApplicationInfoSpace()->fireItemAvailable(std::string("DbPassword"),&dbPasswd_);  
  dbPath_   = "nil" ; getApplicationInfoSpace()->fireItemAvailable(std::string("DbPath"),&dbPath_);  
  partitionName_ = "" ; getApplicationInfoSpace()->fireItemAvailable(std::string("PartitionName"),&partitionName_);
  subDetector_ = "" ; getApplicationInfoSpace()->fireItemAvailable(std::string("SubDetector"),&subDetector_);
  roomTemperature_ = "Warm" ; getApplicationInfoSpace()->fireItemAvailable(std::string("RoomTemperature"),&roomTemperature_);
  databaseAccess_ = false ;

#ifdef DATABASE
  std::string dbLogin = "nil", dbPasswd = "nil", dbPath = "nil" ;
  DbFecAccess::getDbConfiguration (dbLogin, dbPasswd, dbPath) ;
  dbLogin_ = dbLogin ; dbPasswd_ = dbPasswd ; dbPath_ = dbPath ;

  if (dbLogin_ != "nil" && dbPasswd != "nil" && dbPath_ != "nil") databaseAccess_ = true ;

  doUploadInDatabase_ = databaseAccess_ ;
#endif

  // Upload
  doUploadInFile_ = false ; getApplicationInfoSpace()->fireItemAvailable(std::string("UploadFile"),&doUploadInFile_);
  doUploadInFile_ = false ; getApplicationInfoSpace()->fireItemAvailable(std::string("UploadDatabase"),&doUploadInDatabase_);
  vmeSlotBegin_ = 2 ; // 1 is reserved to the VME interface
  vmeSlotEnd_ = (MAX_NUMBER_OF_SLOTS-1) ; // 21
  getApplicationInfoSpace()->fireItemAvailable(std::string("VMESlotBegin"),&vmeSlotBegin_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("VMESlotEnd"),&vmeSlotEnd_) ;
  strBusAdapter_ = strBusAdapterList[CAENPCI] ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("StrBusAdapter"),&strBusAdapter_) ;

  // FED
  doScanFEDs_ = true ; getApplicationInfoSpace()->fireItemAvailable(std::string("ScanFEDs"),&doScanFEDs_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("FEDVmeFileName"),&fedVmeFileName_) ;
  fedStartIdOffset_ = 50;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FEDStartIdOffset"),&fedStartIdOffset_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FEDTemplateFileName"),&fedTemplateFileName_) ;
  xmlFEDFileName_ = "/tmp/fed" ;

  // FEC
  xmlFECFileName_ = "/tmp/fec.xml" ;
  doScanFECs_ = true ; getApplicationInfoSpace()->fireItemAvailable(std::string("ScanFECs"),&doScanFECs_);
  fecBusType_ = fecBusTypeList[FECVME] ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FecBusType"),&fecBusType_) ;
  doCheckTkDcuConversion_ = true ; getApplicationInfoSpace()->fireItemAvailable(std::string("CheckDcuConversion"),&doCheckTkDcuConversion_);
  removeBadModules_ = true ; getApplicationInfoSpace()->fireItemAvailable(std::string("RemoveBadModules"),&removeBadModules_);
  doCalibrationDOH_ = false ; getApplicationInfoSpace()->fireItemAvailable(std::string("DOHCalibration"),&doCalibrationDOH_); dohGainValue_ = 1 ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FECVmeFileName"),&fecVmeFileName_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FECVmeFileNamePnP"),&fecVmeFileNamePnP_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FECTemplateFileName"),&fecTemplateFileName_) ;

  // ---------------------------------- State machine
  // Define the final state machine
  fsm_.addState ('H', "Halted");
  fsm_.addState ('C', "Configured");
  fsm_.addStateTransition ('H','C', CONFIGURE, this, &CrateController::configureAction);
  fsm_.addStateTransition ('C','H', HALT, this, &CrateController::haltAction);
  fsm_.setFailedStateTransitionAction( this, &CrateController::failedTransition );

  //fsm_.setInitialState('I');
  fsm_.setInitialState('H');
  fsm_.reset();

  //Export the stateName variable
  getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);

  // Bind SOAP callbacks for control messages
  xoap::bind (this, &CrateController::fireEvent, CONFIGURE, XDAQ_NS_URI);
  xoap::bind (this, &CrateController::fireEvent, HALT, XDAQ_NS_URI);

  xgi::bind(this, &CrateController::dispatch, "dispatch");

  // -----------------------------------------------
  // Bind for the diagnostic system
#ifdef TKDIAG
  // Configuration option for the diag => link on DIAG_CONFIGURE_CALLBACK
  xgi::bind(this, &CrateController::configureDiagSystem, "configureDiagSystem");
  //                                   => link on DIAG_APLLY_CALLBACK
  xgi::bind(this, &CrateController::applyConfigureDiagSystem, "applyConfigureDiagSystem");

  // For XRelay
  xoap::bind(this, &CrateController::freeLclDiagSemaphore, "freeLclDiagSemaphore", XDAQ_NS_URI );
  xoap::bind(this, &CrateController::freeGlbDiagSemaphore, "freeGlbDiagSemaphore", XDAQ_NS_URI );

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
  //For using the centralized command pannel in logreader
  //                                   => link on DIAG_REQUEST_ENTRYPOINT
  xoap::bind(this, &CrateController::processOnlineDiagRequest, "processOnlineDiagRequest", XDAQ_NS_URI );

#endif

  // -----------------------------------------------
  // Method called by the first level of diagnostic
  xoap::bind(this, &CrateController::crateControllerRecovery, "crateControllerRecovery", XDAQ_NS_URI );

  // -----------------------------------------------
  // Nagivation related links
  std::string url = getApplicationDescriptor()->getURN();
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/MoreParameters\">FEC/FED Parameters</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/StateMachine\">State Machine</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/FecHardwareCheck\">FEC Hardware Check</a>", url.c_str()));
#ifdef TKDIAG
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/ConfigureDiagSystem\">Configure DiagSystem</a>", url.c_str()));
#endif
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=https://twiki.cern.ch/twiki/bin/view/CMS/FECSoftware target=\"_blank\">Documentation</a>", url.c_str()));
  
  // Position in the link, zero means no relation was done
  linkPosition_ = 0 ; 

  xgi::bind(this, &CrateController::displayMoreParameters, "MoreParameters" );
  xgi::bind(this, &CrateController::displayStateMachine, "StateMachine");
  xgi::bind(this, &CrateController::displayFecHardwareCheck, "FecHardwareCheck");
#ifdef TKDIAG
  xgi::bind(this, &CrateController::displayConfigureDiagSystem, "ConfigureDiagSystem");
#endif
  xgi::bind(this, &CrateController::displayHardwareCheckNavigation, "FecHardwareCheckNavigation");
  xgi::bind(this, &CrateController::crateControllerParameterSettings, "crateControllerParameterSettings");

  // Declare all the bind of the methods to display the FEC, RING, CCU registers + I2C modules + ...
  BINDCCUMODULESMETHODS ;

  // ----------------------------------------------
  // Basic configuration for the FEC
  // Configuration file for the VME
  std::string vmeFileName, vmeFileNamePnP, templateFileName ;
  bool plugandplayUsed = FecAccess::getVmeFileName(vmeFileName, vmeFileNamePnP ) ;
  getFecVmeFileName (templateFileName, "FecTemplateDefault.xml") ;
  if (templateFileName.size() > 0) fecTemplateFileName_ = templateFileName ;
  else fecTemplateFileName_ = "" ;
  multiFrames_ = true  ;
  blockMode_   = false ;
  fecAccessManager_ = NULL ;
  fecAccess_ = NULL ;
  if ( (vmeFileName.size() != 0) || (vmeFileNamePnP.size() != 0) ) {
    if (plugandplayUsed) {
      std::stringstream msgInfo ; msgInfo << "Plug and play used: plug and play file " << vmeFileNamePnP << " and FEC Id file " << vmeFileName ;
      
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }
    else {
      std::stringstream msgInfo ; msgInfo << "FEC: Geographical address used: VME file name: " << vmeFileName ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }

    fecVmeFileName_ = vmeFileName ; fecVmeFileNamePnP_ = vmeFileNamePnP ;
  }
  else {
    std::stringstream msgError ; msgError << "Did not find the VME file name configuration" ;
    // Error reporting: Sent to the 1st level
    errorReportLogger_->errorReport (msgError.str(), LOGERROR, 0, XDAQFEC_VMEFILEMISSING) ;
  }

  // ----------------------------------------------
  // Basic configuration for the FED
  // Configuration file for the VME
  std::string fedVmeFileName, fedtemplateFileName ;
  fedTrimDac_ = true;
  fedClockTrigger_ = std::string("TTC");
  fedReadTemp_ = true;
  fedFirstUpload_ = false;  
  
  getApplicationInfoSpace()->fireItemAvailable(std::string("FedClockTrigger"),&fedClockTrigger_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FedTrimDac"),&fedTrimDac_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FedReadTemp"),&fedReadTemp_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("FedFirstUpload"),&fedFirstUpload_) ;
  getFedVmeFileName(vmeFileName, fedtemplateFileName) ;
  fedVmeFileName_ = vmeFileName ;
  fedTemplateFileName_ = fedtemplateFileName ;
  
  // -----------------------------------------------
  crateReset_ = true ; // Reset the crate
  fecReloadFirmware_ = false ; // reload the firmware on the FEC
  crateId_ = 0 ;
  numberOfCrates_ = 1 ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("CrateId"),&crateId_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("NumberOfCrates"),&numberOfCrates_) ;

  // ----------------------------------------------
  // redundancy configuration
  forceApplyRedundancy_ = true ; getApplicationInfoSpace()->fireItemAvailable(std::string("ForceApplyRedundancy"),&forceApplyRedundancy_);
  doScanRedundancy_ = true ; getApplicationInfoSpace()->fireItemAvailable(std::string("ScanRedundancy"),&doScanRedundancy_);

  // -----------------------------------------------
  // Error in parameter settings
  errorInParameters_ = false ;
  msgErrorInParameterSettings_.str("") ;

  // -----------------------------------------------
  // ID assigned by DB
  isVsHostnameAssigned_ = false ;
}

/* Destructor
 */
CrateController::~CrateController ( ) {

  // Disconnect the database if it is set
  delete deviceFactory_ ;
  deviceFactory_ = NULL ;
  //databaseAccess_   = false ; => set in the web page

  // --------------------------------------------------------------
  // Delete the hardware access for the FEC
  try {
    delete fecAccessManager_ ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport ("FATAL Error: Unable to delete a FEC access manager", e, LOGFATAL) ;
    if (displayFrameErrorMessages_) {
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Unable to delete a FEC access manager: " 
				    << std::endl << e.what() 
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }
  try {
    delete fecAccess_ ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport ("FATAL Error: Unable to delete a FEC access", e, LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Unable to delete a FEC access" 
				    << std::endl << e.what()
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                 Finite state machine                                                         */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Configure state: do the upload of FECs, FEDs
 * \warning to do this differents tasks, the corresponding boolean must be set to true
 */
void CrateController::configureAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {

#ifdef TKDIAG
  // Apply the diagnostic fire items value to internal variables and check validity
  DIAG_EXEC_FSM_INIT_TRANS ;
#endif

  // Check if the More Parameters page have been set or not
  if (deviceFactory_ == NULL) {

    if (databaseAccess_ && doUploadInDatabase_) {
      createDatabaseAccess() ;
      if ((deviceFactory_ != NULL) && deviceFactory_->getDbUsed()) identifyIdVsHostname ( ) ;
    }
    
    if (deviceFactory_ == NULL) deviceFactory_ = new DeviceFactory(); 
  }

  // Check the partition name
  if ( (partitionName_ == "") && (doUploadInDatabase_) ) {
    errorReportLogger_->errorReport ("Cannot upload data in database, the partition name is empty", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Cannot upload data in database, the partition name is empty, disabling the upload in database"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    doUploadInDatabase_ = false ;    
  }

  // check for template file for the FEC
  if ( (subDetector_ != UNKNOWN) && 
       ( (fecTemplateFileName_.toString() == "") || 
	 (fecTemplateFileName_.toString().find("FecTemplateDefault.xml") != std::string::npos) ) ) {
    
    if (subDetector_ == "TIB/TID") subDetector_ = "TIB" ;

    // Find the template corresponding to the temperature and the sub-detector
    std::string fecTemplateFileName ;
    getFecVmeFileName ( fecTemplateFileName, "FecTemplate" + subDetector_.toString() + roomTemperature_.toString() + ".xml") ;
    fecTemplateFileName_ = fecTemplateFileName ;
  }

  try {
    // FEC 
    if (doScanFECs_) {
      if (((unsigned int)crateId_) == 0) {
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "ERROR: Cannot upload data in database with crateId_ equals to zero, change it to 1"
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
	errorReportLogger_->errorReport ("Cannot upload data in database with crateId_ equals to zero, change it to 1", LOGERROR) ;
	crateId_ = 1 ;
      }
      uploadDetectFECs () ;
    }
    
    // FED
    if (doScanFEDs_) {
      uploadDetectFEDs () ;
    }  

    // DOH
    if (doCalibrationDOH_) {
      uploadDOHCalibrated () ;
    }
  }
  catch (oracle::occi::SQLException &e) {

    errorReportLogger_->errorReport ("OCCI Exception catched in one of the two scan, software should be updated to catch this error", e, LOGERROR) ;
  }
  catch (FecExceptionHandler &e) {

    errorReportLogger_->errorReport ("FecExceptionHandler catched in one of the two scan, software should be updated to catch this error", e, LOGERROR) ;
  }
  catch (...) {

    errorReportLogger_->errorReport ("One exception catched in one of the two scan, software should be updated to catch this error", LOGERROR) ;
  }
}

void CrateController::haltAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                              Database                                                                        */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Create an access to the database. First check if the env. variables are set (DBCONFLOGIN,
 * DBCONFPASSWD, DBCONFPATH. If not, check the export params
 */
void CrateController::createDatabaseAccess ( ) {

#ifdef DATABASE
  if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
      (dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) {
    
    // First check dyanmically if the values are set
    // if not check the hardcoded values coming from the FecHeader.linux
    std::string dblogin = "nil", dbpass  = "nil", dbpath = "nil" ;
    DbFecAccess::getDbConfiguration (dblogin, dbpass, dbpath) ;

    if (dblogin == "nil") {

      databaseAccess_ = false ;
      dbLogin_ = "nil" ;
      dbPasswd_ = "nil" ;
      dbPath_ = "nil" ;

      // Check the precompilation variable
#  if defined(DATABASEUSER) && defined(DATABASEPASSWD) && defined(DATABASEPATH)
      std::string dblogin = DATABASEUSER ;
      std::string dbpass  = DATABASEPASSWD ;
      std::string dbpath  = DATABASEPATH ;
      
      if ( (dblogin == "nil") || (dbpass == "nil") ||
	   (dbpath == "nil") ||
	   (dblogin.size() == 0) || (dbpass.size() == 0) ||
	   (dbpath.size() == 0) ) {
	
	databaseAccess_ = false ;
	dbLogin_ = "nil" ;
	dbPasswd_ = "nil" ;
	dbPath_ = "nil" ;
      }
      else {

	databaseAccess_ = true ;
	dbLogin_ = dblogin ;
	dbPasswd_ = dbpass ;
	dbPath_ = dbpath ; 
      }
#  endif
    }
    else {

      databaseAccess_ = true ;
      dbLogin_ = dblogin ;
      dbPasswd_ = dbpass ;
      dbPath_ = dbpath ; 
    }
  }
  else databaseAccess_ = true ;

  if (databaseAccess_) {

    // Try to connecting the database 
    try {
      
      std::stringstream msgInfo ; msgInfo << "Create an access to the database: " << dbLogin_.toString() << "@" <<  dbPath_.toString() ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;

      // Delete the previous connections
      if (deviceFactory_ != NULL) { delete deviceFactory_ ; deviceFactory_ = NULL ; }

      deviceFactory_ = new DeviceFactory ( dbLogin_, dbPasswd_, dbPath_ ) ;
      
      databaseAccess_ = true ;
    }
    catch (oracle::occi::SQLException &e) {
      
      std::stringstream msgError ; msgError << "Unable to connect the database: " << dbLogin_.toString() << "/" <<  dbPasswd_.toString() << "@" << dbPath_.toString() ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "FATAL Error: " << msgError.str() << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
      
      databaseAccess_ = false ;
      dbLogin_ = "nil" ;
      dbPasswd_ = "nil" ;
      dbPath_ = "nil" ;

      deviceFactory_ = NULL ;
    }  
  }
#else     // DATABASE
  
  databaseAccess_ = false ;
  dbLogin_ = "nil" ;
  dbPasswd_ = "nil" ;
  dbPath_ = "nil" ;
#endif    // DATABASE

}

/** This method try to identify the subdetector, crateid, and afterwards fed id from database and the hostname of this machine
 */
void CrateController::identifyIdVsHostname ( ) {

  // -------------------------------------------------------------------------------------------------------------------------
  // Identify the crateId and the FED ID
  isVsHostnameAssigned_ = false ;
  if (deviceFactory_->getDbUsed()) {
    char hostname[100] ;
    if (!gethostname(hostname,100)) {
      try {
	hostname_ = hostname ;
	std::string subDetector ;
	unsigned int crateNumber ;
	deviceFactory_->getAllTkIdFromHostname(hostname) ;
	deviceFactory_->getSubDetectorCrateNumberFromHostname(hostname,subDetector,crateNumber) ;
	subDetector_ = subDetector ;
	crateId_ = crateNumber;
	isVsHostnameAssigned_ = true ;
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger_->errorReport ("Cannot retreive information concerning FED ID / sub-detector / crate ID", e, LOGERROR) ;
	if (displayFrameErrorMessages_) {
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Cannot retreive information concerning FED ID / sub-detector / crate ID"
					<< std::endl << e.what() 
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
	
	errorInParameters_ = true ;
	msgErrorInParameterSettings_ << "<span style=\"color: rgb(255, 0, 0);\">" 
				     << "<ul><li>" 
				     << "ERROR: cannot retreive the FED ID / sub-detector / crate ID from database for the hostname " << hostname_ 
				     << "</li></ul></span><p>" ;
      }
      catch (oracle::occi::SQLException &e) {
	errorReportLogger_->errorReport ("Cannot retreive information concerning FED ID / sub-detector / crate ID", e, LOGERROR) ;
	if (displayFrameErrorMessages_) {
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Cannot retreive information concerning FED ID / sub-detector / crate ID"
					<< std::endl << e.what() 
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	errorInParameters_ = true ;
	msgErrorInParameterSettings_ << "<span style=\"color: rgb(255, 0, 0);\">" 
				     << "<ul><li>" 
				     << "ERROR: cannot retreive the FED ID / sub-detector / crate ID from database for the hostname " << hostname_ 
				     << "</li></ul></span><p>" ;
      }
    }
    else {
      errorReportLogger_->errorReport ("Error during retreive of network hostname, cannot retreive information concerning FED ID / sub-detector / crate ID (gethostname error)", LOGERROR) ;
      if (displayFrameErrorMessages_) {
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error during retreive of network hostname, cannot retreive information concerning FED ID / sub-detector / crate ID (gethostname error)"
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }
  }
}
    
/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   I2O message                                                                */
/*                                                                                                              */
/* ************************************************************************************************************ */
/** \brief gets the I2O messages
 */
toolbox::mem::Reference* CrateController::userTrackerCommandHandler(PI2O_TRACKER_COMMAND_MESSAGE_FRAME com) {

  toolbox::mem::Reference* ackRef = NULL ;
  PI2O_TRACKER_COMMAND_MESSAGE_FRAME reply ;

  errorReportLogger_->errorReport ("CrateController: receive an i2o message", LOGDEBUG) ;

  if ((com->System == SYST_TRACKER) && 
      (com->SubSystem == SUB_SYST_FEC ) ) {

    switch (com->Command) {
    default: {
      ackRef = allocateTrackerCommand(1);
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->data[0] = -1;
      reply->PvtMessageFrame.StdMessageFrame.TargetAddress = com->PvtMessageFrame.StdMessageFrame.InitiatorAddress;

      // Error
      std::stringstream msgError ; msgError << "Receive an unknow command " << com->Command << " from system " << com->System << "and sub-system " << com->SubSystem ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: " << msgError.str() << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
      break ;
    }
    }
  }
  else {
      
    ackRef = allocateTrackerCommand(1);
    reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
    reply->data[0] = -1;
    reply->PvtMessageFrame.StdMessageFrame.TargetAddress = com->PvtMessageFrame.StdMessageFrame.InitiatorAddress;

    // Error
    std::stringstream msgError ; msgError << "Receive a message from a unknow system (System = " << com->System << " sub-system = " << com->SubSystem << " and command = " << com->Command ;
    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "Error " << msgError.str() << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }

  return ackRef;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Action performed                                                           */
/*                                                                                                              */
/* ************************************************************************************************************ */
void CrateController::actionPerformed(xdata::Event& event) {

  errorReportLogger_->errorReport ("An action performed as been done but nothing is implemented !", LOGERROR) ;
}

//
// SOAP Callback for FSM
//
xoap::MessageReference CrateController::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception) {

  //errorReportLogger_->errorReport ("fireEvent", LOGDEBUG) ;

  xoap::SOAPPart part = msg->getSOAPPart();
  xoap::SOAPEnvelope env = part.getEnvelope();
  xoap::SOAPBody body = env.getBody();
  DOMNode* node = body.getDOMNode();
  DOMNodeList* bodyList = node->getChildNodes();
  for (unsigned int i = 0; i < bodyList->getLength(); i++)  {

    DOMNode* command = bodyList->item(i);

    if (command->getNodeType() == DOMNode::ELEMENT_NODE) {

      std::string commandName = xoap::XMLCh2String (command->getLocalName());

      try {
	toolbox::Event::Reference e(new toolbox::Event(commandName, this));
	fsm_.fireEvent(e);
	// Synchronize Web state machine
	//	      wsm_.setInitialState(fsm_.getCurrentState());
      }
      catch (toolbox::fsm::exception::Exception & e) {
	XCEPT_RETHROW(xcept::Exception, "invalid command", e);
	errorReportLogger_->errorReport (e.what(), LOGERROR) ;
      }

      xoap::MessageReference reply = xoap::createMessage();
      xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
      xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
      envelope.getBody().addBodyElement ( responseName );
      return reply;
    }
  }

  XCEPT_RAISE(xcept::Exception,"command not found");	
  errorReportLogger_->errorReport ("Command not found", LOGERROR) ;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Web interfaces                                                             */
/*                                                                                                              */
/* ************************************************************************************************************ */

// ------------------------------------------------------------------------------------------------------------ //
// Default page
// ------------------------------------------------------------------------------------------------------------ //

/** Default page for the web display
 */
void CrateController::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {


  switch (linkPosition_) {
  case 0:
    displayRelatedLinks (in,out,true) ;
    break ;
  case 1:
    displayMoreParameters(in,out) ;
    break ;
  case 2:
    displayStateMachine(in,out) ;
    break ;
  case 3:
    displayFecHardwareCheck(in,out) ;
    break ;
#ifdef TKDIAG
  case 4:
    displayConfigureDiagSystem(in,out) ;
    break ;
#endif
//   case 5:
//     displayConfigureDatabase(in,out) ;
//     break ;
  default:
    displayRelatedLinks (in,out,true) ;
    break ;
  }
}

// ------------------------------------------------------------------------------------------------------------ //
// State machine
// ------------------------------------------------------------------------------------------------------------ //

/** Display all the possible states
 */
void CrateController::displayStateMachine(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("displayStateMachine", LOGDEBUG) ;

  linkPosition_ = 1 ;

  // Display the page
  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Crate Controller") << std::endl;
  xgi::Utils::getPageHeader(*out, "Crate Controller: State Machine");

  // Status bar
  displayRelatedLinks (in,out) ;

  // Error in the parameter settings
  if (errorInParameters_) {
    *out << msgErrorInParameterSettings_.str() << std::endl ;
  }
  
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/dispatch";

  // display FSM
  std::set<std::string> possibleInputs = fsm_.getInputs(fsm_.getCurrentState());
  std::set<std::string> allInputs = fsm_.getInputs();
    
  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << cgicc::tr() << std::endl;
  *out << "<th>" << fsm_.getStateName(fsm_.getCurrentState()) << "</th>" << std::endl;
  *out << cgicc::tr() << std::endl;
  *out << cgicc::tr() << std::endl;
  std::set<std::string>::iterator i;
  for ( i = allInputs.begin(); i != allInputs.end(); i++) {

    if ((*i) != INITIALISE) {
      *out << cgicc::td(); 
      *out << cgicc::form().set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;
      
      if ( possibleInputs.find(*i) != possibleInputs.end() ) {
	*out << cgicc::input().set("type", "submit").set("name", "StateInput").set("value", (*i) );
      }
      else {
	*out << cgicc::input() .set("type", "submit").set("name", "StateInput").set("value", (*i) ).set("disabled", "true");
      }
      
      *out << cgicc::form();
      *out << cgicc::td() << std::endl;
    }
  }
  *out << cgicc::tr() << std::endl;
  *out << cgicc::table() << std::endl;

  // ------------------------------------------------------------
  if (displayFrameErrorMessages_) { 
    *out << cgicc::h3("Information, error or warning")<<endl;
    *out << "<textarea width=\"100%\" height=\"300px\" style=\"width:100%;height:300px\" name=\"code\" wrap=\"logical\" rows=\"12\" cols=\"42\">"<<endl;
    size_t hsize=textInformation_.str().size();
    *out << "[INFO: Size of HTML-output stream: "<<hsize
	 <<", time: "<<getCurrentTime()<<"]"<<endl<<endl;
    if (hsize != 0) {
      if ( (textInformation_.str().find ("----------------------------------- ",0) == std::string::npos) ||
	   (textInformation_.str().find ("----------------------------------- ",0) != 0) ) {
	std::stringstream temp ; temp << "----------------------------------- " << getCurrentTime() << std::endl
				      << textInformation_.str()
				      << std::endl << std::endl ;
	textInformation_.str(temp.str()) ;
      }
    }
    *out << textInformation_.str() << std::endl << std::endl ;
    if (hsize > MAXWEBDISPLAY){
      //html->clear();
      textInformation_.str("");
      *out << std::endl <<"[Size of HTML-output stream = " << textInformation_.str().size()
	   << " is bigger than max = " << MAXWEBDISPLAY << " --> clearing stream.]" << std::endl;
    }
    *out << "</textarea>"<<endl;

//   // Check if an error occurrs and display the related message
//   if (errorOccurs_) { // && displayFrameErrorMessages_) {
//     *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
//     *out << cgicc::h2(lastErrorMessage_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
//     *out << cgicc::p() << std::endl ;
//     errorOccurs_ = false ;
//   }
  }

  if ( (fecAccess_ != NULL) && 
       ( (moduleCorrect_.size() > 0) || moduleIncorrect_.size() || moduleCannotBeUsed_.size() || dcuDohOnCcu_.size() ) ) {

  }
  *out << cgicc::html() << std::endl ;
}


/** Depend of which state was initiated
 * Possible state are: Configure, Halt
 */
void CrateController::dispatch(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {   
  try {

    // ------------------------------------------------------------
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // ------------------------------------------------------------
    // Fire the corresponding state
    cgicc::const_form_iterator stateInputElement = cgi.getElement("StateInput");
    std::string stateInput = (*stateInputElement).getValue();

    try {
      toolbox::Event::Reference e(new toolbox::Event(stateInput,this));
      fsm_.fireEvent(e);
    }
    catch (toolbox::fsm::exception::Exception & e) {
      XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
      errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    }
  }
  catch(const std::exception& e) {
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  this->displayStateMachine(in,out);
}

/** Display all the possible states
 */
void CrateController::displayFecHardwareCheck(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("ddisplayFecHardwareCheck", LOGDEBUG) ;

  std::string url = getApplicationDescriptor()->getURN();
  linkPosition_ = 3 ;

  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Crate Controller: FEC Hardware Check") << std::endl;
  xgi::Utils::getPageHeader(*out, "Crate Controller: FEC Hardware Check");

  // Status bar
  displayRelatedLinks (in,out) ;

  // Check hardware access
  if (fecAccess_ == NULL || fecAccessManager_ == NULL) {

    *out << cgicc::h2("No FEC hardware access created, configure the state machine first").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    
    //xgi::Utils::getPageFooter(*out);
    *out << cgicc::html() << std::endl;

    return ;
  }

  // Retreive the FEC list
  std::list<keyType> *fecList = fecAccess_ ->getFecList() ;
  if ((fecList == NULL) || (fecList->size() == 0)) {

    *out << cgicc::h2("No FEC detected on the system, configure the state machine first").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    
    //xgi::Utils::getPageFooter(*out);
    *out << cgicc::html() << std::endl;

    return ;
  }

  // FEC slot with the ring max
  int fecSlotPerRing[MAX_NUMBER_OF_SLOTS] ;
  for (int i = 0 ; i < MAX_NUMBER_OF_SLOTS ; i ++) fecSlotPerRing[i] = -1 ;

  for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
    keyType index = *it ;
    if (fecSlotPerRing[getFecKey(index)] < getRingKey(index)) {
      fecSlotPerRing[getFecKey(index)] = getRingKey(index) ;

      //errorReportLogger_->errorReport ("FEC " + toString(getFecKey(index)) + " ring max " + toString(getRingKey(index)), LOGDEBUG) ;
    }
  }
  delete fecList ; fecList = NULL ;

  // Display the anchor to be used
  *out << "<p style=\"font-family:arial;font-size:10pt;color:#0000AA\">" ;
  //*out << "<a href=\"#ModuleResult\">[Result of Scan]</a>" ;
  *out << "<a href=\"#ModuleWithoutError\">[Module without error]</a> " ;
  *out << "<a href=\"#ModuleWithErrors\">[Module with errors]</a> " ;
  *out << "<a href=\"#ModuleWithFatalErrors\">[Module with fatal errors]</a> " ;
  *out << "<a href=\"#DcuOnCcu\">[DCU on CCUs]</a> " ;
  *out << "<a href=\"#DohOnCcu\">[DOH]</a> " ;
  *out << "</p>" << std::endl ;
  *out << "<p style=\"font-family:arial;font-size:10pt;color:#0000AA\">" ;
  unsigned int displayTag = 1 ;
  for (int i = 1 ; i < MAX_NUMBER_OF_SLOTS ; i ++) {
    if (fecSlotPerRing[i] != -1) {
      *out << "<a href=\"#FEC" << std::dec << i << "\">[FEC Slot " << i << "]</a> " ;
      if ((displayTag % 10) == 0) {
	*out << "</p>" << std::endl ;
	*out << "<p style=\"font-family:arial;font-size:10pt;color:#0000AA\">" ;
      }
      displayTag ++ ;
    }
  }
  *out << "</p>" << std::endl ;

  bool ulBool = true ;
  bool liBool = true ;
  // -------------------------------------------------------------------------
  // Correct module
  // Display the information about the fec slot
  std::list<keyType> errorDevice, noErrorDevice ;
  for (std::list<keyType>::iterator it = moduleCorrect_.begin() ; it != moduleCorrect_.end() ; it ++) {

    keyType index = getFecRingCcuChannelKey((*it)) ;
    if ( (!deviceError_[index|setAddressKey(0x20)]) &&
	 (!deviceError_[index|setAddressKey(0x21)]) &&
	 (!deviceError_[index|setAddressKey(0x22)]) &&
	 (!deviceError_[index|setAddressKey(0x23)]) &&
	 (!deviceError_[index|setAddressKey(0x24)]) &&
	 (!deviceError_[index|setAddressKey(0x25)]) &&
	 (!deviceError_[index|setAddressKey(0x43)]) &&
	 (!deviceError_[index|setAddressKey(0x44)]) &&
	 (!deviceError_[index|setAddressKey(0x0)]) &&
	 (!deviceError_[index|setAddressKey(0x60)]) ) noErrorDevice.push_back((*it)) ;
    else errorDevice.push_back((*it)) ;
  }
  displayListModulesInList (out, "Module(s) without error", "ModuleWithoutError", 
			    " module(s) without error",
			    noErrorDevice, deviceError_, deviceMissing_) ;

  // -------------------------------------------------------------------------
  // Incorrect module
  std::list<keyType> errorModuleTotal ;
  errorModuleTotal = moduleIncorrect_ ; errorModuleTotal.merge (errorDevice) ;
  displayListModulesInList (out, "Module with APV or DCU missing or errors on APV or DCU", "ModuleWithErrors", 
			    " module(s) with errors on APV or DCU missing",
			    errorModuleTotal, deviceError_, deviceMissing_) ;
  //displayListModulesInList (out, "Module with errors on APV or DCU", "ModuleWithErrors", 
  //" module(s) with errors on APV or DCU missing",
  //errorDevice, deviceError_, deviceMissing_) ;
  
  // -------------------------------------------------------------------------
  // Very bad modules
  displayListModulesInList ( out, "Module with fatal error: PLL, MUX or AOH missing", "ModuleWithFatalErrors", 
			     " module(s) with fatal error: PLL, MUX or AOH missing",
			     moduleCannotBeUsed_, deviceError_, deviceMissing_) ;

  // --------------------------------------------------------------------------
  // DCU on CCU
  std::list<keyType> dcuOnCcu ;
  std::list<keyType> dohOnCcu ;
  for (std::list<keyType> ::iterator it = dcuDohOnCcu_.begin() ; it != dcuDohOnCcu_.end() ; it ++) {
    if (getAddressKey((*it)) == DCUADDRESS) dcuOnCcu.push_back((*it)) ;
    else dohOnCcu.push_back((*it)) ;
  }

  ulBool = liBool = true ;
  *out << cgicc::fieldset() << std::endl;
  *out << "<a name=\"DcuOnCcu\"></a>" ;
  *out << cgicc::p() << cgicc::legend("DCU on CCU") << std::endl ;
  *out << "Found " << std::dec << dcuOnCcu.size() << " DCU on CCU" << std::endl ;
  for (std::list<keyType> ::iterator it = dcuOnCcu.begin() ; it != dcuOnCcu.end() ; it ++) {
    
    if (ulBool) {
      ulBool = false ;
      *out << "<ul>" << std::endl ;
    }
    char msg[80] ; decodeKey(msg,(*it)) ;
    *out << "<li>Found a DCU on CCU on " << msg << "</li>" << std::endl ;
    if (deviceError_[(*it)|setAddressKey(0x0)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>Access problem on DCU</li>" << std::endl ;
    }
    if (!liBool) {
      *out << "</ul>" << std::endl ;
      liBool = true ;
    }
  }
  if (!ulBool) *out << "</ul>" << std::endl ;
  *out << cgicc::fieldset() << std::endl;

  // --------------------------------------------------------------------------
  // DOH
  ulBool = liBool = true ;
  *out << cgicc::fieldset() << std::endl;
  *out << "<a name=\"DohOnCcu\"></a>" ;
  *out << cgicc::p() << cgicc::legend("DOH") << std::endl ;
  *out << "Found " << std::dec << dohOnCcu.size() << " DOH" << std::endl ;
  for (std::list<keyType> ::iterator it = dohOnCcu.begin() ; it != dohOnCcu.end() ; it ++) {
    
    if (ulBool) {
      ulBool = false ;
      *out << "<ul>" << std::endl ;
    }
    char msg[80] ; decodeKey(msg,(*it)) ;
    *out << "<li>Found a DOH on " << msg << "</li>" << std::endl ;
    if (deviceError_[(*it)|setAddressKey(0x70)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>Access problem on DOH</li>" << std::endl ;
    }
    if (!liBool) {
      *out << "</ul>" << std::endl ;
      liBool = true ;
    }
  }
  if (!ulBool) *out << "</ul>" << std::endl ;
  *out << cgicc::fieldset() << std::endl;

  // --------------------------------------------------------------------------
  // For each FEC display a field with all the information needed
  unsigned int fecSlotOld = 0 ;
  for (unsigned int fecSlot = 1 ; fecSlot < MAX_NUMBER_OF_SLOTS ; fecSlot ++) {
    
    if (fecSlotPerRing[fecSlot] != -1) {

      //errorReportLogger_->errorReport ("FEC " + toString(fecSlot) + " ring max " + toString(fecSlotPerRing[fecSlot]), LOGDEBUG) ;

      if (fecSlot != fecSlotOld) {
	fecSlotOld = fecSlot ;
	displayFecRingField (in,out,fecSlot) ;
      }
    }
  }
  
  //xgi::Utils::getPageFooter(*out);
  *out << cgicc::html() << std::endl;
}

// ------------------------------------------------------------------------------------------------------------ //
// Related links
// ------------------------------------------------------------------------------------------------------------ //

/** Parameter page
 */
void CrateController::displayMoreParameters(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("displayMoreParameters", LOGDEBUG) ;
  linkPosition_ = 1 ;

  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Crate Controller Parameters") << std::endl;
  xgi::Utils::getPageHeader(*out, "Crate Controller Parameters");

  // Status bar
  displayRelatedLinks (in,out) ;

  // Is error during the lastest settings of parameters
  if (errorInParameters_) {
    *out << msgErrorInParameterSettings_.str() << std::endl ;
  }

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/crateControllerParameterSettings";	

  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  if (fsm_.getCurrentState() == 'I' ) {
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", INITIALISE);
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Apply");
  }
  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  // Size of all the next fields
  std::string sizeField = "30" ;

  // --------------------------------------------------------------------
  // General parameters
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("General") << std::endl;
  //*out << cgicc::p() << std::endl ;

  // Display debug messages
  if (displayDebugMessage_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","displayDebugMessage").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","displayDebugMessage") << std::endl;
  *out << cgicc::label("Display debug messages (filter by the diagnostic system)")  << std::endl;

  // Display errors in frame
  if (displayFrameErrorMessages_) 
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayFrameErrorMessages")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayFrameErrorMessages")
      .set("selected") << std::endl;
  *out << cgicc::label("Display error messages on web pages") << std::endl;

  // Scan for FEDs
  if (doScanFEDs_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doScanFEDs").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doScanFEDs") << std::endl;
  *out << cgicc::label("Scan the crate for FEDs")  << std::endl;

  // Scan for FECs
  if (doScanFECs_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doScanFECs").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doScanFECs") << std::endl;
  *out << cgicc::label("Scan the crate for FECs")  << std::endl;  

   // Calibrate the DOH
   if (doCalibrationDOH_)
     *out << cgicc::p() << cgicc::input().set("type", "checkbox")
       .set("name","doCalibrationDOH").set("checked","true") << std::endl;
   else
     *out << cgicc::p() << cgicc::input().set("type", "checkbox")
       .set("name","doCalibrationDOH") << std::endl;
   *out << cgicc::label("Perform the calibration of the DOH with gain")  << std::endl;
   std::stringstream dohSelect ; 
   dohSelect <<  "<select name=\"dohGainValue\">" 
	     << "<option>" << 0 << "</option>" 
	     << "<option selected>" << 1 << "</option>" 
	     << "<option>" << 2 << "</option>" << "</select>" ;
   *out << dohSelect.str() << std::endl ;

  // range of slots
  std::stringstream vmeSlotBeginSelect, vmeSlotEndSelect; 
  vmeSlotBeginSelect << "<select name=\"vmeSlotBegin\">" ;
  vmeSlotEndSelect << "<select name=\"vmeSlotEnd\">" ;
  for (unsigned long slot = 2 ; slot < MAX_NUMBER_OF_SLOTS ; slot ++) {
    if (slot == (xdata::UnsignedLongT)vmeSlotBegin_) {
      vmeSlotBeginSelect << "<option selected>" << slot << "</option>";
    }
    else {
      vmeSlotBeginSelect << "<option>" << slot << "</option>";
    }
    if (slot == (xdata::UnsignedLongT)vmeSlotEnd_) {
      vmeSlotEndSelect << "<option selected>" << slot << "</option>";
    }
    else {
      vmeSlotEndSelect << "<option>" << slot << "</option>";
    }
  }
  vmeSlotBeginSelect << "</select>" << std::endl ;
  vmeSlotEndSelect << "</select>" << std::endl ;
  
  // Crate ID
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << "<td>"; 
  *out << cgicc::label("Crate ID")  << std::endl;  
  *out << "</td>"; *out << "<td>" << std::endl;
  *out << cgicc::input().set("type","text")
    .set("name","crateId")
    .set("size","10")
    .set("value",crateId_.toString()) << std::endl;
  *out << "</td>"; *out << "</tr>" << std::endl;
  *out << "<tr>" << "<td>"; 
  *out << cgicc::label("Number of crates")  << std::endl; 
  *out << "</td>"; *out << "<td>" << std::endl;
  *out << cgicc::input().set("type","text")
    .set("name","numberOfCrates")
    .set("size","10")
    .set("value",numberOfCrates_.toString()) << std::endl;
  *out << "</td>"; *out << "</tr>" << std::endl;
  *out << "<tr>" << "<td>"; 
  *out << cgicc::label("Scan slot range: ")  << std::endl; 
  *out << "</td>" << "<td>" << std::endl;
  *out << vmeSlotBeginSelect.str() << std::endl ;
  //*out << "</td>" << "<td>" << std::endl;
  *out << vmeSlotEndSelect.str() << std::endl ;
  *out << "</td>"; *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl ;

  // Database parameters
#ifdef DATABASE
  // First check dyanmically if the values are set
  std::string dblogin = "nil", dbpass  = "nil", dbpath = "nil" ;
  DbFecAccess::getDbConfiguration (dblogin, dbpass, dbpath) ;

  // Database login ...
  if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
      (dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) databaseAccess_ = false ;
    
  // Database access
  if (databaseAccess_ && doUploadInDatabase_) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","uploadDestination")
      .set("size","30")
      .set("value","database")
      .set("selected")
      .set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","uploadDestination")
      .set("size","30")
      .set("value","database")
      .set("selected") << std::endl ;
  }
  *out << cgicc::label("Database Upload")  << std::endl;  

  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Login ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","dbLogin")
    .set("size",sizeField)
    .set("value",dbLogin_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Password ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","password")
    .set("name","dbPasswd")
    .set("size",sizeField)
    .set("value",dbPasswd_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Path ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","dbPath")
    .set("size",sizeField)
    .set("value",dbPath_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Partition Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","partitionName")
    .set("size",sizeField)
    .set("value",partitionName_.toString()) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;
#endif

  // File upload
  if (doUploadInFile_) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","uploadDestination")
      .set("size","30")
      .set("value","file")
      .set("selected")
      .set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","uploadDestination")
      .set("size","30")
      .set("value","file")
      .set("selected") << std::endl ;
  }
  *out << cgicc::label("File Upload")  << std::endl;  
    
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("FED XML File ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","xmlFEDFileName")
    .set("size",sizeField)
    .set("value",xmlFEDFileName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("FEC XML File ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","xmlFECFileName")
    .set("size",sizeField)
    .set("value",xmlFECFileName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;

  if ((!databaseAccess_ || !doUploadInDatabase_) && !doUploadInFile_) {
  *out << cgicc::p() << cgicc::input().set("type","radio")
    .set("name","uploadDestination")
    .set("size","30")
    .set("value","None")
    .set("selected")
    .set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","uploadDestination")
      .set("size","30")
      .set("value","None")
      .set("selected") << std::endl ;
  }
  *out << cgicc::label("No Upload") << std::endl;

  *out << cgicc::p() << std::endl;
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  // Hardware bus support
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Hardware Bus Support") << std::endl;
  //*out << cgicc::p() << std::endl ;

  // Crate reset
  if (crateReset_) {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","crateReset").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","crateReset") << std::endl;
  }
  *out << cgicc::label("Crate Reset") << std::endl;

  // Reload FEC firmware
  if (fecReloadFirmware_) {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fecReloadFirmware").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fecReloadFirmware") << std::endl;
  }
  *out << cgicc::label("FEC Reload Firmware") << std::endl;

  // VME FEC over SBS bus adapter
  if ( (fecBusType_.toString() == fecBusTypeList[FECVME]) && (strBusAdapter_.toString() == strBusAdapterList[SBS]) ) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",strBusAdapterList[SBS])
      .set("selected").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",strBusAdapterList[SBS])
      .set("selected") << std::endl;
  }
  *out << cgicc::label(" VME over SBS ") << std::endl;

  // VME FEC over CAEN PCI bus adapter
  if ( (fecBusType_.toString() == fecBusTypeList[FECVME]) && (strBusAdapter_.toString() == strBusAdapterList[CAENPCI]) ) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",strBusAdapterList[CAENPCI])
      .set("selected").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",strBusAdapterList[CAENPCI])
      .set("selected") << std::endl;
  }
  *out << cgicc::label(" VME over CAEN PCI") << std::endl;

  // VME FEC over CAEN USB bus adapter
  if ( (fecBusType_.toString() == fecBusTypeList[FECVME]) && (strBusAdapter_.toString() == strBusAdapterList[CAENUSB]) ) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",strBusAdapterList[CAENUSB])
      .set("selected").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",strBusAdapterList[CAENUSB])
      .set("selected") << std::endl;
  }
  *out << cgicc::label(" VME over CAEN USB") << std::endl;

  // PCI or USB FEC
  if (fecBusType_.toString() == fecBusTypeList[FECPCI]) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",fecBusTypeList[FECPCI])
      .set("selected")
      .set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",fecBusTypeList[FECPCI])
      .set("selected")<< std::endl;
  }
  *out << cgicc::label("Optical PCI FEC") << std::endl;

  if (fecBusType_.toString() == fecBusTypeList[FECUSB]) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",fecBusTypeList[FECUSB])
      .set("selected") 
      .set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","strBusAdapter")
      .set("size","30")
      .set("value",fecBusTypeList[FECUSB])
      .set("selected") << std::endl;
  }
  *out << cgicc::label("USB FEC") << std::endl ;

  // End of the field
  *out << cgicc::p() << std::endl ;
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("FED Parameters") << std::endl;
  *out << cgicc::p() << std::endl ;

  // FED
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("FED VME File Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","fedVmeFileName")
    .set("size","80")
    .set("value",fedVmeFileName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Template FED File Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","fedTemplateFileName")
    .set("size","80")
    .set("value",fedTemplateFileName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Clock Trigger ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << "&nbsp;<select name=\"fedClockTrigger\">";
  if (fedClockTrigger_ == "TTC")
    *out << "<option selected>" << "TTC" << "</option>";
  else
    *out << "<option>" << "TTC" << "</option>";
  if (fedClockTrigger_ == "Backplane")
    *out << "<option selected>" << "Backplane" << "</option>";
  else
    *out << "<option>" << "Backplane" << "</option>";
  if (fedClockTrigger_ == "Internal")
    *out << "<option selected>" << "Internal" << "</option>";
  else
    *out << "<option>" << "Internal" << "</option>";
  *out << "</select>"<< std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (fedTrimDac_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fedTrimDac").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fedTrimDac") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Trim DAC ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (fedReadTemp_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fedReadTemp").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fedReadTemp") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Read Temperature") << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>";
  if (fedFirstUpload_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fedFirstUpload").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fedFirstUpload") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>";
  *out << cgicc::label("!!Important!! Check this if it is the first upload to create a new partition!!! ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;

  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::p() << cgicc::input().set("type", "text")
    .set("name","fedStartIdOffset").set("size","5") 
    .set("value",fedStartIdOffset_.toString())
	 << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Fed Start ID Offset (normally 50)") << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;

  *out << cgicc::table() << std::endl ;
  *out << cgicc::p() << std::endl ;
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << std::endl ;
  
  // ------------------------------------------------------------------------------
  // FEC
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("FEC Parameters") << std::endl;
  *out << cgicc::p() << std::endl ;

  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("FEC VME File Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","fecVmeFileName")
    .set("size","80")
    .set("value",fecVmeFileName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Plug'n Play FEC VME File Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","fecVmeFileNamePnP")
    .set("size","80")
    .set("value",fecVmeFileNamePnP_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Template FEC File Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","fecTemplateFileName")
    .set("size","80")
    .set("value",fecTemplateFileName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;

  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Please select the detector for the scan")  << std::endl;
  *out << "</td>"; 
  *out << "<td>"; 
  *out << "<select name=\"SubDetector\">";
  if (subDetector_ == "") subDetector_ = UNKNOWN ;
  if (subDetector_ == UNKNOWN) *out << "<option selected>" << UNKNOWN << "</option>";
  else *out << "<option>" << UNKNOWN << "</option>";
  if ( (subDetector_ == "TIB") || (subDetector_ == "TID") ) *out << "<option selected>" << "TIB/TID" << "</option>";
  else *out << "<option>" << "TIB/TID" << "</option>";
  if (subDetector_ == "TOB") *out << "<option selected>" << "TOB" << "</option>";
  else *out << "<option>" << "TOB" << "</option>";
  if (subDetector_ == "TEC+") *out << "<option selected>" << "TEC+" << "</option>";
  else *out << "<option>" << "TEC+" << "</option>";
  if (subDetector_ == "TEC-") *out << "<option selected>" << "TEC-" << "</option>";
  else *out << "<option>" << "TEC-" << "</option>";
  *out << "</select>"<< std::endl;
  *out << " and room temperature " << "<select name=\"roomTemperature\">" ;
  if (roomTemperature_ == "Warm") *out << "<option selected>" << "Warm" << "</option>" ;
  else *out << "<option>" << "Warm" << "</option>" ;
  if (roomTemperature_ == "Cold") *out << "<option selected>" << "Cold" << "</option>" ;
  else *out << "<option>" << "Cold" << "</option>" ;
  *out << " and click Apply to select the FEC template file and DCU channel" << "</td>" << std::endl ;
  *out << "</tr>" << std::endl ;

   *out << "<tr>" << std::endl ;
   *out << "<td>" << "</td>" << std::endl ;
   *out << "<td>"; 
   if (doScanRedundancy_)
     *out << cgicc::p() << cgicc::input().set("type", "checkbox")
       .set("name","doScanRedundancy").set("checked","true") << std::endl;
   else
     *out << cgicc::p() << cgicc::input().set("type", "checkbox")
       .set("name","doScanRedundancy") << std::endl;
   *out << "</td>" << std::endl;
   *out << "<td>"; 
   *out << cgicc::label("Scan the CCU redundancy for all the CCUs on each ring (only on working rings)")  << std::endl;
   *out << "</td>" << std::endl;
   *out << "</tr>" << std::endl;

  *out << "<tr>" << std::endl ;
  *out << "<td>" << "</td>" << std::endl ;
  *out << "<td>"; 
  if (forceApplyRedundancy_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","forceApplyRedundancy").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","forceApplyRedundancy") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Force the redundancy on the rings before detection (the reconfiguration is automatically done if the ring is not correct)")  << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;

  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (doCheckTkDcuConversion_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doCheckTkDcuConversion").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doCheckTkDcuConversion") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Check if the DCU conversion factors exits")  << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (multiFrames_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","multiFrames").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","multiFrames") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Multi-frames algorithm used")  << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (blockMode_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","blockMode").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","blockMode") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("VME Block transfer used")  << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (removeBadModules_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","removeBadModules").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","removeBadModules") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Remove modules with no AOH, PLL, MUX")  << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl ;
  *out << cgicc::p() << std::endl ;
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << std::endl ;
  // End of Configuration
  // --------------------------------------------------------------------

  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply");

  // End of the form
  *out << cgicc::form() << std::endl;

  //xgi::Utils::getPageFooter(*out);
  *out << cgicc::html() << std::endl;
}

/** Initialise action or apply on the parameters
 */
void CrateController::crateControllerParameterSettings ( xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("crateControllerParameterSettings", LOGDEBUG) ;

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Display the debug messages
    displayDebugMessage_ = cgi.queryCheckbox("displayDebugMessage") ;
    displayFrameErrorMessages_ = cgi.queryCheckbox("displayFrameErrorMessages");

    // Scan
    doScanFEDs_ = cgi.queryCheckbox("doScanFEDs") ;
    doScanFECs_ = cgi.queryCheckbox("doScanFECs") ;
    doCalibrationDOH_ = cgi.queryCheckbox("doCalibrationDOH") ;
    dohGainValue_ = fromString<unsigned int>(cgi["dohGainValue"]->getValue()) ;

    // Crate ID
    crateId_ = fromString<unsigned long>(cgi["crateId"]->getValue());
    numberOfCrates_ = fromString<unsigned long>(cgi["numberOfCrates"]->getValue());

    // Scan slot range
    vmeSlotBegin_ = fromString<unsigned long>(cgi["vmeSlotBegin"]->getValue());
    vmeSlotEnd_ = fromString<unsigned long>(cgi["vmeSlotEnd"]->getValue());

    // Crate Reset
    crateReset_ = cgi.queryCheckbox("crateReset") ;
    if (crateReset_) errorReportLogger_->errorReport ("A crate reset will be issued", LOGWARNING) ;

    // FEC firmware reload
    fecReloadFirmware_ = cgi.queryCheckbox("fecReloadFirmware") ;
    if (fecReloadFirmware_) errorReportLogger_->errorReport ("A reload of the FEC firmware will be done", LOGWARNING) ;

    // VME File
    fedVmeFileName_ = cgi["fedVmeFileName"]->getValue();
    fedTemplateFileName_ = cgi["fedTemplateFileName"]->getValue();
    fecVmeFileName_ = cgi["fecVmeFileName"]->getValue();
    fecVmeFileNamePnP_ = cgi["fecVmeFileNamePnP"]->getValue() ;
    std::string templateFileAgain = cgi["fecTemplateFileName"]->getValue();

    // Sub detector
    subDetector_ = cgi["SubDetector"]->getValue();
    if ( (subDetector_ != UNKNOWN) && 
	 ( (fecTemplateFileName_.toString() == "") || 
	   (fecTemplateFileName_.toString().find("FecTemplateDefault.xml") != std::string::npos) ) ) {

      if (subDetector_ == "TIB/TID") subDetector_ = "TIB" ;

      // Find the template corresponding to the temperature and the sub-detector
      roomTemperature_ = cgi["roomTemperature"]->getValue() ;
      std::string fecTemplateFileName ;
      getFecVmeFileName ( fecTemplateFileName, "FecTemplate" + subDetector_.toString() + roomTemperature_.toString() + ".xml") ;
      fecTemplateFileName_ = fecTemplateFileName ;
    }
    else {
      fecTemplateFileName_ = templateFileAgain ;
    }

    // FED parameters
    fedClockTrigger_ =  cgi["fedClockTrigger"]->getValue();
    fedTrimDac_ = cgi.queryCheckbox("fedTrimDac") ;
    fedReadTemp_ = cgi.queryCheckbox("fedReadTemp") ;
    fedFirstUpload_ = cgi.queryCheckbox("fedFirstUpload") ;
    fedStartIdOffset_ = fromString<unsigned long>(cgi["fedStartIdOffset"]->getValue());
    // FEC parameters
    multiFrames_ = cgi.queryCheckbox("multiFrames") ;
    if (blockMode_ != cgi.queryCheckbox("blockMode")) {
      blockMode_   = cgi.queryCheckbox("blockMode") ;
      if (fecAccess_ != NULL) fecAccess_->setFifoAccessMode (blockMode_) ;
    }
    doScanRedundancy_ = cgi.queryCheckbox("doScanRedundancy") ;
    forceApplyRedundancy_ = cgi.queryCheckbox("forceApplyRedundancy") ;
    doCheckTkDcuConversion_ = cgi.queryCheckbox("doCheckTkDcuConversion") ;
    removeBadModules_ = cgi.queryCheckbox("removeBadModules") ;

    // Configuration
    if (std::string(cgi["uploadDestination"]->getValue()) == "file") {
      doUploadInFile_     = true  ;
      doUploadInDatabase_ = false ;
    }
    else if (std::string(cgi["uploadDestination"]->getValue()) == "None") {
      doUploadInFile_     = false ;
      doUploadInDatabase_ = false ;
    }
    else if (std::string(cgi["uploadDestination"]->getValue()) == "database") {
      doUploadInFile_     = false ;
      doUploadInDatabase_ = true  ;
      databaseAccess_     = true  ;
    }

    partitionName_ = cgi["partitionName"]->getValue() ;  // for upload in database
    xmlFEDFileName_ = cgi["xmlFEDFileName"]->getValue() ; // for upload in file
    xmlFECFileName_ = cgi["xmlFECFileName"]->getValue() ; // for upload in file

    //std::cout << "doUploadInDatabase = " << doUploadInDatabase_ << std::endl ;
    //std::cout << "doUploadInFile = " << doUploadInFile_ << std::endl ;
    //std::cout << "databaseAccess = " << databaseAccess_ << std::endl; 

#ifdef DATABASE
    // Database access    
    std::string dbLogin = cgi["dbLogin"]->getValue();
    std::string dbPasswd = cgi["dbPasswd"]->getValue();
    std::string dbPath = cgi["dbPath"]->getValue();

    if (databaseAccess_) {

      bool databaseChanged = false;
      
      if ( (dbLogin != dbLogin_.toString()) ||
	   (dbPasswd != dbPasswd_.toString()) ||
	   (dbPath != dbPath_.toString()) ) databaseChanged = true;
      
      dbLogin_ = dbLogin ;
      dbPasswd_ = dbPasswd ;
      dbPath_ = dbPath ;
      
      if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
	  (dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) {
	
	databaseAccess_ = false ;
	
	// Error reporting
	errorReportLogger_->errorReport ("Cannot create a database access, disable the database access", LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "FATAL Error: Cannot create a database access, disable the database accesss" << std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
      }
      else {
	
// 	std::cout << "Before Creating database access" << std::endl ;
// 	std::cout << "Device factory = " << deviceFactory_ << std::endl ;
// 	if (deviceFactory_ != NULL) std::cout << "Connected: " << deviceFactory_->getDbConnected() << std::endl ;
// 	std::cout << "Upload in DB " << doUploadInDatabase_ << std::endl ;
// 	std::cout << "Db access " << databaseAccess_ << std::endl ;
// 	if (deviceFactory_ != NULL) std::cout << "Db used" << deviceFactory_->getDbUsed() << std::endl ;

	if ( (databaseChanged) ||
	     (deviceFactory_ == NULL) || 
	     (!deviceFactory_->getDbConnected()) ) {

	  createDatabaseAccess() ; // the delete of the deviceFactory_ is done inside this method
	}
      }
    }

    // Select the good upload
    if (deviceFactory_ != NULL) {
      if ( doUploadInDatabase_ && deviceFactory_->getDbConnected() ) deviceFactory_->setUsingDb(true) ;
      else if (doUploadInFile_) deviceFactory_->setUsingFile(true) ;
    }

    //doUploadInDatabase_ = databaseAccess_ ;
    if (!databaseAccess_) 
#endif
      {
	//if (doUploadInFile_) {
	errorReportLogger_->errorReport ("The upload will be done in files", LOGUSERINFO) ;

	if (deviceFactory_ != NULL) { delete deviceFactory_ ; deviceFactory_ = NULL ; }
	deviceFactory_ = new DeviceFactory(); 
      }

    if (!doUploadInDatabase_ && !doUploadInFile_)
      errorReportLogger_->errorReport ("No upload performed", LOGUSERINFO) ;

    strBusAdapter_ = cgi["strBusAdapter"]->getValue() ;
    if ( (strBusAdapter_ == strBusAdapterList[SBS]) || (strBusAdapter_ == strBusAdapterList[CAENPCI]) || (strBusAdapter_ == strBusAdapterList[CAENUSB]) ) {
      fecBusType_ = fecBusTypeList[FECVME] ;
    }
    else {
      if (strBusAdapter_ == fecBusTypeList[FECPCI]) {
	fecBusType_ = fecBusTypeList[FECPCI] ;
      }
      else if (strBusAdapter_ == fecBusTypeList[FECUSB]) {
	fecBusType_ = fecBusTypeList[FECUSB] ;
      }
    }

#ifdef DATABASE
    //if ( (dbLogin_.toString() != "nil") && (dbPasswd_.toString() != "nil") && (dbPath_.toString() != "nil") ) {
    if (doUploadInDatabase_) {
      errorReportLogger_->errorReport("Database = " + dbLogin_.toString() + "/" + dbPasswd_.toString() + "@" + dbPath_.toString() + ": partition " + partitionName_.toString() , LOGDEBUG) ;
    }
    else
      errorReportLogger_->errorReport("No database", LOGDEBUG) ;
#else
    errorReportLogger_->errorReport("Database not compiled", LOGDEBUG) ;
#endif
      
    std::stringstream msgInfo1 ; msgInfo1 << "FEC bus type: " << fecBusType_.toString() 
					   << " (" << fecBusTypeList[FECVME] << ", " 
					   << fecBusTypeList[FECPCI] << ", " 
					   << fecBusTypeList[FECUSB] << ") " ;
    if (fecBusType_.toString() == fecBusTypeList[FECVME]) {
      msgInfo1 << " over " << strBusAdapter_.toString() 
	       << " (" << strBusAdapterList[CAENPCI] << ", " 
	       << strBusAdapterList[CAENUSB] << ", "
	       << strBusAdapterList[SBS] << "): " 
	       << "multiframes " << (multiFrames_ ? "ON" : "OFF") << " and " 
	       << (blockMode_ ? "VME Block transfer ON for the FEC (you need to have the latest firmware on the CAEN interface)" : "VME Block transfer OFF for the FEC (you need to have the latest firmware on the CAEN interface to use it)") ;
      
      if (fecVmeFileNamePnP_ == "") 
	msgInfo1 << "Geographical address used: VME file name " << fecVmeFileName_.toString() << std::endl ;
      else
	msgInfo1 << "Plug and play used: plug and play file " << fecVmeFileNamePnP_.toString() << " and FEC Id file " << fecVmeFileName_.toString() ;
    }
    errorReportLogger_->errorReport(msgInfo1.str(), LOGDEBUG) ;
    msgInfo1.str("") ; msgInfo1 << "VME FED over " << strBusAdapter_.toString() << ": " << fedVmeFileName_.toString() << " for HAL VME access" ;
    
    errorReportLogger_->errorReport("FEC Template: " + fecTemplateFileName_.toString() + " and FED template " + fedTemplateFileName_.toString(), LOGDEBUG) ;
    
    if (doUploadInDatabase_ && databaseAccess_) 
      errorReportLogger_->errorReport("Upload in database", LOGDEBUG) ;
    else if (doUploadInFile_)
      errorReportLogger_->errorReport("Upload in file", LOGDEBUG) ;
    else
      errorReportLogger_->errorReport("No upload perform", LOGDEBUG) ;
  }
  catch(const std::exception& e) {
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  // -------------------------------------------------------------------------------------------------------------------------
  // Identify the crateId and the FED ID
  identifyIdVsHostname ( ) ;

  // -------------------------------------------------------------------------------------------------------------------------
  // Check the settings
  // check the errors
  errorInParameters_ = false ;
  bool errorInFecDb  = false ;
  bool errorInFedDb  = false ;
  bool errorInFedVmeFileName = false ;
  bool errorInFedTemplate    = false ;
  bool errorInFecVmeFileName = false ;
  bool errorInFecTemplate    = false ;
  bool errorFecCrateId       = false ;
  msgErrorInParameterSettings_.str("") ;
  // No scan possible for FEC and FED in the same application
  if (doScanFEDs_ && doScanFECs_ ) errorInParameters_ = true ;
  // check the fedVmeFileName, fedTemplateFileName, fecVmeFileName, fecTemplateFileName
  if (doScanFEDs_) {
    std::ifstream fileInStream1(fedVmeFileName_.toString().c_str());
    if (!fileInStream1.is_open()) errorInFedVmeFileName = true ;
    else fileInStream1.close() ;
    std::ifstream fileInStream2(fedTemplateFileName_.toString().c_str());
    if (!fileInStream2.is_open()) errorInFedTemplate = true ;
    else fileInStream2.close() ;

    if (errorInFedTemplate || errorInFedVmeFileName) errorInParameters_ = true ;
  }
  if (doScanFECs_) {
    std::ifstream fileInStream1(fecVmeFileName_.toString().c_str());
    if (!fileInStream1.is_open()) errorInFecVmeFileName = true ;
    else fileInStream1.close() ;
    std::ifstream fileInStream2(fecTemplateFileName_.toString().c_str()) ;
    if (!fileInStream2.is_open()) errorInFecTemplate = true ;
    else fileInStream2.close() ;

    if (errorInFecTemplate || errorInFecVmeFileName) errorInParameters_ = true ;
    unsigned int crateId = crateId_ ;
    if (crateId == 0) { 
      errorFecCrateId = true ; 
      errorInParameters_ = true ;
      crateId_ = 1 ; 
    }
  }
  // Database parameters correct ?
  if (doUploadInDatabase_ && (!databaseAccess_ || deviceFactory_ == NULL || !deviceFactory_->getDbUsed()) ) errorInParameters_ = true ;
  else if ( (partitionName_ == "") && doUploadInDatabase_ && databaseAccess_ && (deviceFactory_ != NULL) && deviceFactory_->getDbUsed() ) errorInParameters_ = true ;
  else if ( (partitionName_ != "") && doUploadInDatabase_ && databaseAccess_ && ( (deviceFactory_ != NULL) || (!deviceFactory_->getDbUsed())) ) {

    // retreive all the partitions to Xcheck with download / upload
    std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNames() ;

    // Check the partition in the DB
    if (doScanFECs_) {
      // the partition should not exists
      for (std::list<std::string>::iterator itPart = partitionNames.begin() ; (itPart != partitionNames.end()) && !errorInFecDb ; itPart ++) {
	if (partitionName_.toString() == (*itPart)) { errorInFecDb = true ; errorReportLogger_->errorReport ("partition is existing", LOGDEBUG) ; }
      }
      if (errorInFecDb) errorInParameters_ = true ;
    }
    if (doScanFEDs_) {
      // the partition should exists
      if (partitionNames.size() == 0) errorInFedDb = true ; 
      else {
	errorInFedDb = true ;
	for (std::list<std::string>::iterator itPart = partitionNames.begin() ; (itPart != partitionNames.end()) && errorInFedDb ; itPart ++) {
	  if (partitionName_.toString() == (*itPart)) { errorInFedDb = false ; errorReportLogger_->errorReport ("partition is existing", LOGDEBUG) ; }
	}
      }
      if (errorInFedDb) errorInParameters_ = true ; 
    }
  }
  // Sub detector
  if ( (subDetector_ == UNKNOWN) && doScanFECs_ ) errorInParameters_ = true ; 

  // Error message if error has been detected
  if (errorInParameters_) {
    msgErrorInParameterSettings_ << "<span style=\"color: rgb(255, 0, 0);\">" << cgicc::fieldset() << std::endl;
    msgErrorInParameterSettings_ << cgicc::p() << cgicc::legend("Error in Parameter Settings") << std::endl ;
    msgErrorInParameterSettings_ << "<ul>" << std::endl ;

    // check scan FEC and FEDs
    if (doScanFEDs_ && doScanFECs_ ) {
      msgErrorInParameterSettings_ << "<li>" << "You cannot scan the FEC and the FED in the same application cause of HAL libraries and it will crash the CrateController" << "</li>" << std::endl ;
    }

    // check files
    if (doScanFEDs_) {
      if (errorInFedVmeFileName) {
	msgErrorInParameterSettings_ << "<li>" << "FED VME file name is not set or could not be opened, please correct the path and the file" << "</li>" << std::endl ;
      }
      if (errorInFedTemplate) {
	msgErrorInParameterSettings_ << "<li>" << "FED template is not set or could not be opened, please correct the path and the file" << "</li>" << std::endl ;
      }
    }
    if (doScanFECs_) {
      if (errorInFecVmeFileName) {
	msgErrorInParameterSettings_ << "<li>" << "FEC VME file name is not set or could not be opened, please correct the path and the file" << "</li>" << std::endl ;
      }
      if (errorInFecTemplate) {
	msgErrorInParameterSettings_ << "<li>" << "FEC template is not set or could not be opened, please correct the path and the file" << "</li>" << std::endl ;
      }
    }

    if ( doUploadInDatabase_ && (!databaseAccess_ || deviceFactory_ == NULL || !deviceFactory_->getDbUsed()) ) {
      msgErrorInParameterSettings_ << "<li>" << "You cannot upload in the database, database settings are not set and access to the database has not been created" << "</li>" << std::endl ;
    }
    else if ( (partitionName_ == "") && doUploadInDatabase_ && databaseAccess_ && (deviceFactory_ != NULL) && deviceFactory_->getDbUsed() ) {
      msgErrorInParameterSettings_ << "<li>" << "You cannot upload in the database, the partition name is empty" << "</li>" << std::endl ;
    }
    else if ( (partitionName_ != "") && doUploadInDatabase_ && databaseAccess_ && (deviceFactory_ != NULL) && deviceFactory_->getDbUsed() ) {

      // the partition should not exists for FEC
      if (doScanFECs_ && errorInFecDb) {
	msgErrorInParameterSettings_ << "<li>" << "You cannot upload in the database, the partition name is already existing in the database" << "</li>" << std::endl ;
      }
      // the partition should exists for FED
      if (doScanFEDs_ && errorInFedDb) {
	msgErrorInParameterSettings_ << "<li>" << "You cannot upload in the database, the partition name is not existing in the database so no FEC devices have been uploaded" << "</li>" << std::endl ;
      }
    }

    // Warning
    if ( (subDetector_ == UNKNOWN) && doScanFECs_ ) {
      msgErrorInParameterSettings_ << "<li>" << "Warning: you did not select your sub-detector, default FEC template file will be used" << "</li>" << std::endl ;
    }

    // Error and correction
    if ( errorFecCrateId && doScanFECs_ ) {
      msgErrorInParameterSettings_ << "<li>" << "Error: the crateId cannot be zero for the FEC, should be >= 1 to be compliant with CMSSW (value changed to 1)" << "</li>" << std::endl ;
    }

    msgErrorInParameterSettings_ << "</ul>" << std::endl ;
    msgErrorInParameterSettings_ << cgicc::fieldset() << "</span>" << "<p>" << std::endl;
  }


  // Redisplay the web page
  this->Default(in,out);
  //this->displayInitialisePage(in,out) ;
}

// ------------------------------------------------------------------------------------------------------------ //
// Failure
// ------------------------------------------------------------------------------------------------------------ //

/** Failure page
 */
void CrateController::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  //errorReportLogger_->errorReport ("failedTransition", LOGDEBUG) ;

  toolbox::fsm::FailedEvent & fe = dynamic_cast< toolbox::fsm::FailedEvent&>(*e);
  std::stringstream msgError ; msgError << "Failure occurred when performing transition from: "  
					 << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what() ;
  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
			
  if (displayFrameErrorMessages_) { 	
    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
    std::stringstream temp ; temp << "Error: Failure occurred when performing transition from: " << std::endl
				  << xcept::htmlformat_exception_history(fe.getException()) << std::endl << textInformation_.str() ;
    textInformation_.str(temp.str()) ;
  }
}

/** This method will receive the command issue by the recovery lvl1 system
 * \param msg - message
 * \return retrun an acknowledge
 */
xoap::MessageReference CrateController::crateControllerRecovery (xoap::MessageReference msg) throw (xoap::exception::Exception) {

  // reply to caller
  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName responseName = envelope.createName( "receivedLogResponse", "xdaq", XDAQ_NS_URI);
  envelope.getBody().addBodyElement ( responseName );

  // --------------------------------------------------------------------------------------------------------
  // Analyse and parse the message 

  // --------------------------------------------------------------------------------------------------------
  // Action
  errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, 1024) ;

  // --------------------------------------------------------------------------------------------------------
  // End of recovery 
  errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, -1, 1024) ;

  return reply;
}


/* ************************************************************************************************************ */
/*                                                                                                              */
/*                              FEC Methods                                                                     */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Create the default parameters for the FED and template file name
 * This method is considered as "static"
 */
void CrateController::getFecVmeFileName ( std::string &templateFileName, std::string fileName ) {

  // Try to find the correct file for the VME file name for offset
  // and addresses
  templateFileName.clear() ;

  // $ENV_CMS_TK_FEC_ROOT directory or ENV_TRACKER_DAQ directory
  char *basic = getenv ("ENV_TRACKER_DAQ") ;
  if (basic == NULL) basic=getenv ("ENV_CMS_TK_FEC_ROOT") ;
  if (basic == NULL) basic=getenv ("FECSOFTWARE_ROOT") ;

  if (basic != NULL) templateFileName = std::string(basic) + std::string("/config/" + fileName) ;
  else templateFileName = std::string("/config/") + fileName ;
}

/** Create the FEC Access class depending on the version of the FecSoftware and the FEC type
 * If the VME FEC is used, the VME file name must be used and defined (vmeFileName_)
 * \warning this method is the same method than the FecSupervisor::createFecAccess
 */
FecAccess *CrateController::createFecAccess ( ) {

  FecAccess *fecAccess = NULL ;

  // Debug message
  if (fecBusType_ == fecBusTypeList[FECVME])
    errorReportLogger_->errorReport ( "FEC: Create a VME FEC", LOGDEBUG) ;
  else if (fecBusType_ == fecBusTypeList[FECPCI])
    errorReportLogger_->errorReport ( "FEC: Create a PCI FEC", LOGDEBUG) ;
  else if (fecBusType_ == fecBusTypeList[FECUSB])
    errorReportLogger_->errorReport ( "FEC: Create an USB FEC", LOGDEBUG) ;
  else
    errorReportLogger_->errorReport ( "FEC: Please specify a FEC bus type", LOGFATAL, 0, XDAQFEC_NOFECBUSTYPE) ;

  // FEC type
  enumFecBusType fecBusType = FECVME ;
  if (fecBusType_ == fecBusTypeList[FECPCI]) fecBusType = FECPCI ;
  else if (fecBusType_ == fecBusTypeList[FECVME]) fecBusType = FECVME ;
  else if (fecBusType_ == fecBusTypeList[FECUSB]) fecBusType = FECUSB ;
  else {

    errorReportLogger_->errorReport ("FEC: No bus (PCI, VME or USB) is specified cannot continue", LOGFATAL, 0, XDAQFEC_NOFECBUSTYPE) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: No bus (PCI, VME or USB) is specified cannot continue" 
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }

  // List of FEC, ring found
  std::list<keyType> *fecList = NULL ;

  try {
    // Create the FEC Access
    switch ( fecBusType ) {
    case FECPCI:
#if defined(BUSPCIFEC)
      // bool forceAck, bool initFec, bool scan
      fecAccess = new FecAccess (true, false, true, false, 100) ;
      ringMin_ = 0 ; ringMax_ = 0 ;
#endif
      break ;
    case FECVME: {
#if defined(BUSVMECAENUSB) || defined(BUSVMECAENPCI) || defined(BUSVMESBS)
      std::string strBusAdapterName ;
      if      (strBusAdapter_ == strBusAdapterList[SBS])     strBusAdapterName = FecVmeRingDevice::STR_BUSADAPTER_NAME[SBS]     ;
      else if (strBusAdapter_ == strBusAdapterList[CAENPCI]) strBusAdapterName = FecVmeRingDevice::STR_BUSADAPTER_NAME[CAENPCI] ;
      else if (strBusAdapter_ == strBusAdapterList[CAENUSB]) strBusAdapterName = FecVmeRingDevice::STR_BUSADAPTER_NAME[CAENUSB] ;

      // Only the access to the corresponding FEC slot is created
      unsigned int fecAddresses[MAX_NUMBER_OF_SLOTS] = {0} ;
      for (unsigned int fecSlot = (unsigned int)vmeSlotBegin_ ; fecSlot <= (unsigned int)vmeSlotEnd_ ; fecSlot ++) {
	fecAddresses[fecSlot] = FecVmeRingDevice::VMEFECBASEADDRESSES[fecSlot] ;
      }

      // bool forceAck, bool init, bool scan, scan crate
      if (fecVmeFileNamePnP_ != "") {
	fecAccess = new FecAccess (0, fecVmeFileName_, fecVmeFileNamePnP_, true, false, true, false, 100, strBusAdapterName, blockMode_) ;
      }
      else {
	fecAccess = new FecAccess (0, fecVmeFileName_, fecAddresses, true, false, true, false, 100, strBusAdapterName, blockMode_) ;
      }

      // After a crate reset, the plug and play must re-charged (if it is used)
      // it is automatically done by FecAccess class
      if (crateReset_) fecAccess->crateReset() ;

      ringMin_ = FecVmeRingDevice::getMinVmeFecRingValue() ;
      ringMax_ = FecVmeRingDevice::getMaxVmeFecRingValue() ;
      }
#endif
      break ;
    case FECUSB:
#if defined(BUSUSBFEC)
      ringMin_ = 0 ; ringMax_ = 0 ;
      fecAccess = new FecAccess (0,true, false, true, false, 100) ;
#endif
      break ;
    }

    // Get the FEC list
    fecList = fecAccess->getFecList() ;
    if ((fecList == NULL) || (fecList->empty())) {
      errorReportLogger_->errorReport ("FEC: No VME FEC board detected on this setup", LOGFATAL,0,XDAQFEC_NOFECDETECTED) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "FATAL Error: FEC: No VME FEC board detected on this setup" << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }
  }
  catch (FecExceptionHandler &e) {

    errorReportLogger_->errorReport ("FEC: Unable to create a FEC access", e, LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: FEC: Unable to create a FEC access" 
				    << std::endl << e.what()
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    fecAccess = NULL ;
  }
  
  delete fecList;

  return (fecAccess) ;
}

/** Upload FECs in the output: file or database
 */
void CrateController::uploadDetectFECs ( ) {

  // ---------------------------------------------------------------
  // Create the hardware access
  if (fecAccess_ == NULL) {
    fecAccess_ = createFecAccess ( ) ;
    if (fecAccess_ != NULL) {
      // Create the FEC access manager for the download and for the upload
      if (fecAccessManager_ != NULL) delete fecAccessManager_ ;
      fecAccessManager_ = NULL ;
      try {
	// Access manager
	fecAccessManager_ = new FecAccessManager ( fecAccess_, displayDebugMessage_ ) ;
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger_->errorReport ("FEC: Unable to create a FEC access manager", e, LOGFATAL) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "FATAL Error: FEC: Unable to create a FEC access manager"
					<< std::endl << e.what()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;      
	}
	return ;
      }
    }
    else return ; // fatal error !
  }
  else fecAccessManager_->setDisplayDebugMessage(displayDebugMessage_) ;

#define MODULECORRECTFILE "ModuleCorrectScanned.txt"
#define MODULEINCORRECTFILE "ModuleIncorrectScanned.txt"
#define INSTANCEFECSLOTFECHARDWAREIDFILE "InstanceFecHardwareIdFecSlot.txt"

  // Call the method
  unsigned int devMajor = 0, devMinor = 0 ;
  bool createPartition = true ; // create a new partition
  std::list<std::string> listErrors ; // to be displayed in the web main page

  keyType indexBegin = NOFECRING, indexEnd = NOFECRING ;
  if ( ((xdata::UnsignedLongT)vmeSlotBegin_ != 2) || ((xdata::UnsignedLongT)vmeSlotEnd_ != (MAX_NUMBER_OF_SLOTS-1)) ) {

    indexBegin = buildFecRingKey(vmeSlotBegin_,fecAccess_->getMinRingNumber()) ;
    indexEnd = buildFecRingKey(vmeSlotEnd_,fecAccess_->getMaxRingNumber()) ;
  }

  bool uploadOk = 
    FecDetectionUpload::uploadDetectFECs ( *fecAccess_, *fecAccessManager_, *errorReportLogger_, (FecFactory *)deviceFactory_,
					   MODULECORRECTFILE, MODULEINCORRECTFILE, INSTANCEFECSLOTFECHARDWAREIDFILE,
					   fecTemplateFileName_.toString(), xmlFECFileName_.toString(), partitionName_.toString(), partitionName_.toString(),
					   removeBadModules_, 
					   displayDebugMessage_, multiFrames_, doUploadInDatabase_ && databaseAccess_, doUploadInFile_, createPartition, 
					   moduleCorrect_, moduleIncorrect_, moduleCannotBeUsed_, dcuDohOnCcu_, deviceError_, deviceMissing_, listErrors,
					   subDetector_.toString(), 
					   indexBegin, indexEnd, 
					   &devMajor, &devMinor, crateId_, fecReloadFirmware_,
					   forceApplyRedundancy_ ) ;

  // Display the list of the errors
  if (listErrors.size() > 0) {
    std::stringstream msgWebError ;
    for (std::list<std::string>::iterator it = listErrors.begin() ; it != listErrors.end() ; it ++) {
      msgWebError << (*it) << std::endl ;
    }

    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "Error " << msgWebError.str() << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }

  // Display an error or not depending of what has been asked
  if (displayFrameErrorMessages_) {

    if (doUploadInDatabase_ && databaseAccess_) {
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; 
      if (uploadOk)
	temp << "SUCCESS: Upload in database has been performed correctly for partition " << partitionName_.toString() << std::endl 
	     << "      Devices uploaded in version " << devMajor << "." << devMinor << std::endl ;
//	     << "      PIA uploaded in version " << piaMajor << "." << piaMinor << std::endl ;
      else temp << "ERROR: Error during the upload in database" ;
      temp << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
    else if (doUploadInFile_) {
      std::stringstream temp ; 
      if (uploadOk) temp << "SUCCESS: Upload in file " << xmlFECFileName_.toString() << "  has been performed correctly" ;
      else temp << "ERROR: Error during the ulpoad in file " << xmlFECFileName_.toString() ;
      temp << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
    else {
      std::stringstream temp ; temp << "No upload has been requiered" << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }

  // MERSI
  // This part should check the CCU on ring A + dummy CCU and upload it to the database
  // errorReportLogger_->errorReport ("Starting the check of rings for dummy CCU", LOGINFO) ;
  // uploadCCUDB ( );
  errorReportLogger_->errorReport ("The dummy CCU are not anymore checked, a bug is raised and should be debug", LOGUSERINFO) ;
  // uploadCCUDB ( );

  // Redundancy part
  if (doScanRedundancy_) {
    // MERSI
    // Test all the B channels => ring is working here
    // add method to test the B channel command in std::vector<CCUDescription> FecAccess::scanSiTrackerBChannels ( keyType fecRingIndex, std::vector<CCUDescription> ) ;
    // FEC 8, ring 0 => buildFecRingKey(8,0)
    // Call the method and define a display/errors

    // MERSI
    // if the ring is not working
    // Download the CCUs from the database
    // Given a vector of CCUDescription => make an automatic procedure to reconfigure the ring
    // std::vector<CCUDescription> FecAccess::redundancySiTracker ( keyType, std::vector<CCUDescription> ordered ) ;
    // then upload it to the database

    // Not yet implemented
    errorReportLogger_->errorReport ("Warning: The redundancy check is done only on the B input and B output of the FEC and the Ring", LOGINFO); 
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "Warning: The redundancy check is done only on the B input and B output of the FEC and the Ring"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }

  // Check the DCU conversion factors
  if (doCheckTkDcuConversion_) {
    errorReportLogger_->errorReport ("Warning: The check of the conversion factors is not yet implemented", LOGINFO); 
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "Warning: The check of the conversion factors is not yet implemented"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }

  //delete fecAccess_ ; fecAccess_ = NULL ;
}


/** Upload DOH calibration values in a minor version or in a file
 */
void CrateController::uploadDOHCalibrated ( ) {

  // ---------------------------------------------------------------
  // Create the hardware access
  if (fecAccess_ == NULL) {
    fecAccess_ = createFecAccess ( ) ;
    if (fecAccess_ != NULL) {
      // Create the FEC access manager for the download and for the upload
      if (fecAccessManager_ != NULL) delete fecAccessManager_ ;
      fecAccessManager_ = NULL ;
      try {
	// Access manager
	fecAccessManager_ = new FecAccessManager ( fecAccess_, displayDebugMessage_ ) ;
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger_->errorReport ("FEC: Unable to create a FEC access manager", e, LOGFATAL) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "FATAL Error: FEC: Unable to create a FEC access manager"
					<< std::endl << e.what()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;      
	}
	return ;
      }
    }
    else return ; // fatal error !
  }
  
  fecAccessManager_->setDisplayDebugMessage(displayDebugMessage_) ;

  keyType indexBegin = NOFECRING, indexEnd = NOFECRING ;
  if ( ((xdata::UnsignedLongT)vmeSlotBegin_ != 2) || ((xdata::UnsignedLongT)vmeSlotEnd_ != (MAX_NUMBER_OF_SLOTS-1)) ) {
    indexBegin = buildFecRingKey(vmeSlotBegin_,0) ;
    indexEnd = buildFecRingKey(vmeSlotEnd_,0) ;
  }

  unsigned int devMajor, devMinor ;
  bool uploadOk = 
    FecDetectionUpload::uploadDOHCalibrated ( *fecAccess_, *fecAccessManager_, *errorReportLogger_, *((FecFactory *)deviceFactory_),
					      xmlFECFileName_.toString(), xmlFECFileName_.toString(), partitionName_.toString(),
					      displayDebugMessage_, multiFrames_, doUploadInDatabase_ && databaseAccess_, doUploadInFile_, 
					      dohGainValue_,
					      indexBegin, indexEnd, 
					      &devMajor, &devMinor, 
					      forceApplyRedundancy_ ) ;

  // Display an error or not depending of what has been asked
  if (displayFrameErrorMessages_) {

    if (doUploadInDatabase_ && databaseAccess_) {
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; 
      if (uploadOk)
	temp << "SUCCESS: Upload of DOH calibration in database has been performed correctly for partition " << partitionName_.toString() << std::endl 
	     << "      Devices uploaded in version " << devMajor << "." << devMinor << std::endl ;
      else temp << "ERROR: Error during the upload of the DOH devices in database" ;
      temp << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
    else if (doUploadInFile_) {
      std::stringstream temp ; 
      if (uploadOk) temp << "SUCCESS: Upload of the DOH devices in file " << xmlFECFileName_.toString() << "  has been performed correctly" ;
      else temp << "ERROR: Error during the ulpoad  of the DOH devices in file " << xmlFECFileName_.toString() ;
      temp << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
    else {
      std::stringstream temp ; temp << "No upload has been requiered" << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }

  //delete fecAccess_ ; fecAccess_ = NULL ;
}


/* ************************************************************************************************************ */
/*                                                                                                              */
/*                              FED Methods                                                                     */
/*                                                                                                              */
/* ************************************************************************************************************ */
/** Upload the FEDs to the output (file or database)
 */
void CrateController::uploadDetectFEDs ( ) {

  // Check if the FEC access is opened then close it
  // The CAEN/HAL interface does not support multiple creation of bus adapter
  if (fecAccess_ != NULL) delete fecAccess_ ;
  if (fecAccessManager_ != NULL) delete fecAccessManager_ ;

  // strBusAdapter_.toString()
  //   - strBusAdapterList[SBS]
  //   - strBusAdapterList[CAENPCI]
  //   - strBusAdapterList[CAENUSB]
  // VME File name:
  //   - fedVmeFileName_.toString()
  // Template description (file name)
  //   - fedTemplateFileName_.toString()

  // DeviceFactory is created and the name is deviceFactory_
  // and can be used

  // All the hardware part should created here

  // Crate reset must be performed if crateReset_ is set to true
  // now we have to create a description for each fed, but to do this we need a template description.
  // the template description comes from either the file TemplateDescription.xml
  // or the database description under fedhardwareid = 0
  // if there is no template description then an error is thrown.
  
  //Create a Fed9UDeviceFactory
  //  std::cout << "Do you want to read temperatures also?(y/n)" << std::endl;
  //char innit;
  //cin >> innit;
  using namespace Fed9U;
  //Fed9UDeviceFactory theDeviceFactory;
  std::cout << "Using DeviceFactory.." << std::endl;
  deviceFactory_->setDebugOutput(false);
  deviceFactory_->setUsingStrips(true);
  std::cout << "Using DeviceFactory.." << std::endl;

  Fed9UDescription templateDescription;
  try {
    
    std::cout << "load template description from file" << std::endl;
    
    deviceFactory_->addFedFileName(fedTemplateFileName_.toString());//"TemplateDescription.xml");
    deviceFactory_->setUsingFile(true);
    templateDescription = deviceFactory_->getFed9UDescription(0,false,std::string("null"),1,0);

    if (strBusAdapter_.toString() == strBusAdapterList[SBS]) {
      templateDescription.setBusAdaptorType ( FED9U_HAL_BUS_ADAPTOR_SBS ) ;  
    }
    else if (strBusAdapter_.toString() == strBusAdapterList[CAENUSB]) {
      templateDescription.setBusAdaptorType ( FED9U_HAL_BUS_ADAPTOR_CAEN_USB ) ;
    }

    if (deviceFactory_->getDbConnected() && doUploadInDatabase_)
      deviceFactory_->setUsingDb(true);
  }catch (ICUtils::ICException & exc){
    std::cout << "exception caught when adding template description to device factory... \n" << exc.what() << std::endl; 
    return;
  } catch (...) {
    std::cout << "exception caught when adding template description to device factory... \n" << std::endl; 
    return;
  }
  u16 crateNumber;
  //u16 numberOfCrates = templateDescription.getCrateNumber();
  // char trimDac;
  // std::cout << "\n\nDo you want to run a trim DAC calibration?(y/n):" << std::endl;
  // std::cin >> trimDac;
  
  //std::cout << "Do you want to " << (theDeviceFactory.getDbConnected()? "upload to database or":"") << "save to file or not at all?(" <<  (theDeviceFactory.getDbConnected()? "d,":"") << "f or n)" << std::endl;
  //char ch;
  //cin >> ch;
  /*std::string partitionName;
  bool dontSave = false;
  if (tolower(ch) == 'd'){
    if ( theDeviceFactory.getDbConnected() ) {
      theDeviceFactory.setUsingDb();
      std::cout << "enter the partition name:" << std::endl;
      std::cin >> partitionName;
    }
  } else if (tolower(ch) == 'f'){
    theDeviceFactory.setUsingFile();
  } else {
    dontSave = true;
    }*/
  
  

  //std::cout << "\n\n\n!!!Please input the crate number:" << std::endl;
  //std::cin >> dec >> crateNumber;
  
  //char clockTrigger;
  //std::cout << "\nChoose either TTC clock and trigger(t), Backplane(b) or internal(i):" << std::endl;
  //cin >> clockTrigger;
  
  // check if we are scanning more than one crate by checking the number of crates
  // note that this is number of crates per CAEN bus controller. this is the number of crates controlled by
  // one Crate Controller!!! crateId_ is the starting crate number for the first crate on the chain for that crate controller
  // note that nominally there is one crate per controller. If at a later date there is any change because for example vme controllers die and 
  // we need to daisychain, then the change should be reflected in the db, in the sense that the first crate on the daisy chain will take it's 
  // id from the that in the database for that controller pc name, all subsequent crates will get an id incremented by 1 per crate on the chain. 
  std::vector <Fed9UDescription*> allFedsVector;	       	
  u32 runningTrimDacCal = 0;

  try {
    identifyIdVsHostname();
  }catch (ICUtils::ICException & exc){
    std::cout << "exception caught when downloading the crate Ids... \n" << exc.what() << std::endl; 
    return;
  } catch (...) {
    std::cout << "exception caught when downloading the crate Ids... \n" << std::endl; 
    return;
  }

  for ( unsigned int j = 0 ;  j < numberOfCrates_ ; j ++) {
    // we know we are looping over n crates

    templateDescription.setFedHardwareId(0); // hardware ID must be set to 0 otherwise you will not detect the FEDS!
    templateDescription.setHalAddressTable(fedVmeFileName_.toString());
    crateNumber = crateId_+j;	  
    
    std::cout << " crate number = " << crateNumber << std::endl;
    templateDescription.setVmeControllerDaisyChainId(j);
    
    // let's detect the FEDs in the crate and create the descriptions
    
    Fed9UCrateStatusProbe crateStatus;
    
    try {
      std::cout << "about to probe crate!" << std::endl;
      crateStatus.probeCrate(&templateDescription,vmeSlotBegin_,vmeSlotEnd_,true,true,(fedReadTemp_),true);
      std::cout << "probed crate!" << std::endl;
    } catch (ICUtils::ICException & exc){
      std::cout << " Failed to scan crate for some reason!!!" << exc.what() << std::endl;
      break;
    } catch (...) {
      std::cout << " Failed to scan crate for some reason!!!" << std::endl;
    }
    
    // if we are not saving the files or to db then we don't need to go any further
    if (!doUploadInFile_ && !doUploadInDatabase_)
      continue;
    Fed9U::Fed9UEntry fe;
    std::stringstream descriptionName;
    descriptionName.str("");
    
    u32 totalThreads( crateStatus.getNumberOfEntries() );
    std::cout << "total number of entries is! " << totalThreads <<  endl;
    bool allThreadsInitd(false);
    vector<std::ostringstream*> messages;
    //Used to keep track of which number entry we are indexing for passing the message streams to the thread.
    u32 entriesCount(0);
    
    u16 versionMajor, versionMinor;
    // loop over all Fed9UEntries in crateStatus
    std::stringstream fileName;
    fileName.str("");
    //    int inst(0);
    bool firstFed = true;
    std::cout << "about to loop over entries" << std::endl;
    u32 randNumber;
    while(crateStatus.getNextFed9UEntry(fe)) {
      try {
	std::cout << "looping over entries" << std::endl;


	// Example for FED ID
	if (isVsHostnameAssigned_) {
	  unsigned int fedId = 0 ;
	  unsigned int slot = fe.getSlotNumber();
	  try {
	    TkIdVsHostnameDescription *idVsHostname = deviceFactory_->getAllTkIdFromHostnameSlot(hostname_,slot) ;
	    if (idVsHostname != NULL) {
	      fedId = idVsHostname->getFedId() ;
	      if (fedId <50 || fedId > 489)
		throw;
	      // set the fedid in the description for this mapping.
	      templateDescription.setFedId(fedId); 
	    } else {
	      std::cerr << "IMPORTANT!!! No fed id for that hostname and slot => the FedId has been assigned based upon the crateid and slot number, this will not work for final running...!!!" << std::endl ;
	      // LM correction	templateDescription.setFedId(fedStartIdOffset_ + crateNumber*16 + fe.getSlotNumber()-2); 

	      templateDescription.setFedId(fedStartIdOffset_ + crateNumber*20 + fe.getSlotNumber()-1); 
	      // note we have 20 slots available for feds but we can only have up to 16 FEDs per crate. 
	      // Slot numbers go up to 21... so we take off 2 so that 0 is slot 2 for first crate and 56 is slot 2 for second crate and so on.	      
	    }
	  }
	  catch (...) {
	     templateDescription.setFedId(fedStartIdOffset_ + crateNumber*20 + fe.getSlotNumber()-1);
              
	     errorReportLogger_->errorReport ("FED: Unable to find the FED soft ID for hostname " + hostname_ + " and slot " + toString(slot), LOGERROR) ;
	    if (displayFrameErrorMessages_) { 
	      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	      std::stringstream temp ; temp << "Error: FEC: Unable to find the FED soft ID for hostname " << hostname_ << " and slot " << slot
					    << std::endl << textInformation_.str() ;
	      textInformation_.str(temp.str()) ;      
	    }
	  }
	}


	//deviceFactory_->setInstance(inst++);
	templateDescription.setSlotNumber(fe.getSlotNumber());
	templateDescription.setCrateNumber(crateNumber);
	templateDescription.setVmeControllerDaisyChainId(j);
	templateDescription.setFedHardwareId(fe.getFed9UABC()->getFedHardwareId()); // don't forget to set the hardware id back again to 999
	templateDescription.setFedVersion(fe.getFed9UABC()->getFedVersion());
	templateDescription.setEpromVersion(fe.getFed9UABC()->getEpromVersion());
	templateDescription.setFeFirmwareVersion(fe.getFed9UABC()->getFeFirmwareVersion());
	templateDescription.setBeFirmwareVersion(fe.getFed9UABC()->getBeFirmwareVersion());
	templateDescription.setVmeFirmwareVersion(fe.getFed9UABC()->getVmeFirmwareVersion());
	templateDescription.setDelayFirmwareVersion(fe.getFed9UABC()->getDelayFirmwareVersion());
	
	if (fedClockTrigger_ == "TTC") {
	  templateDescription.setClock(FED9U_CLOCK_TTC);
	  templateDescription.setTriggerSource(FED9U_TRIG_TTC);
	  std::cout << "set the TTC clock and Trigger" << std::endl;
	} else if(fedClockTrigger_ == "Backplane") {
	  templateDescription.setClock(FED9U_CLOCK_BACKPLANE);
	  templateDescription.setTriggerSource(FED9U_TRIG_BACKPLANE);
	  std::cout << "set the backplane clock and Trigger" << std::endl;
	} else {
	  templateDescription.setClock(FED9U_CLOCK_INTERNAL);
	  templateDescription.setTriggerSource(FED9U_TRIG_SOFTWARE);
	  std::cout << "set the internal clock and Trigger" << std::endl;
	}

	//<JRF date="30/11/2006"> adding some code to set the random number seed for the fake events, so we can have a different seed on each channel
	Fed9U::Fed9UAddress channelPair;
	for (u32 loopy=0; loopy < Fed9U::CHANNELS_PER_FED/2; loopy++) {
	  channelPair.setFedChannelPair(loopy);
	  randNumber=rand()&0x3FF;
	  templateDescription.setFakeEventRandomSeed(channelPair, randNumber);
	  templateDescription.setFakeEventRandomMask(channelPair, 25); // note this number can be changed to adjust the mask of the random number in the fake event
	}
	//</JRF>
	
	// now we store the fed in the full vector of feds
	Fed9UDescription* tempptr = new Fed9UDescription();
	*tempptr = templateDescription;
	allFedsVector.push_back(tempptr);
	  std::cout << "loaded fed into vector!!!" << std::endl;	
	//Create the message stream.
	messages.push_back( new std::ostringstream() );
	
	std::cout << "about to do Unthreded trim dac calibration!" << std::endl;
	if (fedTrimDac_) {
	  bool usingThreads=false;
	  if (usingThreads) {
	    pthread_attr_t attr;
	    pthread_t tid;
	    pthread_attr_init(&attr);
	    std::cout << "messages.size() " << messages.size() << " entriesCount " << entriesCount << std::endl;
	    void * arg[] = { tempptr, &allThreadsInitd, messages[entriesCount], &runningTrimDacCal };
	    ++entriesCount;
	    std::cout << "!!MainThread!! " << "about to launch thread with description pointer = " << hex << tempptr << dec << "  for description id " << tempptr->getFedId() <<  endl;
	    
	    const u32 currentNumberThreads( runningTrimDacCal );
	    
	    //This needs to ensure that the other threads have performed initialisation of their arguments before launching the next thread.
	    pthread_create(&tid,&attr,&Fed9U::trimDacCal,arg);
	    
	    //Wait till that thread tells you it has spawned.
	    //Each thread is prevented from completing till all have spawned hence number can only increment.
	    int ntry=0;
	    while (currentNumberThreads == runningTrimDacCal) {
	      if (totalThreads <= runningTrimDacCal) allThreadsInitd = true;
	      ntry++;
	      if (ntry>1000)
		{
		  std::cout <<"First loop " <<endl;
		  break;
		}
	      fed9Uwait(10);
	    }
	  } else {
	    // let's calibrate the TrimDac, start by creating a fed object.
	    Fed9UDevice Fed(templateDescription);
	    Fed.init();
	    Fed.setBlockModeReadout(true); 
	    try {
	      Fed.trimDACInternalCalibration();
	    }  catch (ICUtils::ICException & exc ) {
	      std::cout << exc.what() << std::endl;
	    }  //templateDescription=Fed.getFed9UVMEDeviceDescription();
	    
	    *tempptr=Fed.getFed9UVMEDeviceDescription();
	    
	    }
	}
      } catch (exception & exc) {
	//	errorReportLogger_->errorReport (string(exc.what()), exc, LOGWARNING) ;
	if (displayFrameErrorMessages_) {
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "FATAL Error: Unable to delete a FEC access manager: " 
					<< std::endl << exc.what() 
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;	  
	}
	std::cout << "Exception caught when constructing vector of feds from scan! Could be trim dac problem.: " <<exc.what() << std::endl;
      } catch (...) {
	std::cout << "Unknown exception caught when constructing vector of feds from scan!" << std::endl;
      }
    }
    
    //now we must wait for the trim dac calibration on all feds to finish
    while (runningTrimDacCal!=0) 
      {
	std::cout << "Second loop " <<endl;
	fed9Uwait(100);

      }
    
    //Printout all the messages and delete them...
    for (vector<std::ostringstream*>::iterator messagesIt( messages.begin() ); messagesIt != messages.end(); ++messagesIt) {
      if (*messagesIt != NULL) {
	std::cout << (*(*messagesIt)).str() << std::endl;
	delete *messagesIt;
	*messagesIt = NULL;
      }
    }

    
    for (std::vector<Fed9UDescription*>::iterator i = allFedsVector.begin(); i != allFedsVector.end(); i++) {
      
      try {
	
	fileName << xmlFEDFileName_.toString() << "DescCrate" << static_cast<u16>(crateNumber) << "Slot" << static_cast<u16>((*i)->getSlotNumber());
	fileName.flush();
	templateDescription.setName(fileName.str());
	fileName << ".xml";
	fileName.flush();
	if (!deviceFactory_->getDbUsed()) {
	  std::cout << std::dec << "Saving Description for FED in slot " 
	       <<  static_cast<u16>((*i)->getSlotNumber()) 
	       <<  "File Saved to location :  " 
	       << fileName.str() 
	       << std::endl;
	} 
	else {
	  std::cout << std::dec << "Uploading Description for FED in slot " 
	       <<  static_cast<u16>((*i)->getSlotNumber()) 
	       <<  " To Partition :  " 
	       << partitionName_.toString()
	       << std::endl;
	}
	deviceFactory_->setOutputFileName(fileName.str());
	fileName.str("");
	fileName.clear();
	//deviceFactory_->setInstance(templateDescription.getFedId());
	if (deviceFactory_->getDbUsed() ) {
	  deviceFactory_->setFed9UDescription(*(*i),partitionName_.toString(), &versionMajor, &versionMinor, (firstFed && fedFirstUpload_)?1:2); //this will make sure that only the first fed of the first crate will create the new major version, all subsequent feds from all other crates will be appended to it.
	  std::cout << std::dec << "Uploaded to version = " << versionMajor << "." << versionMinor << "first Fed?" << (firstFed?"true":"false")<<  endl;
	}
	else
	  deviceFactory_->setFed9UDescription(*(*i), &versionMajor, &versionMinor, firstFed?1:2);
	
	firstFed=false;
      } catch (ICUtils::ICException & exc) {
	std::cout << exc.what() << std::endl;
      } catch (...) {
	std::cout << "Unknown exception caught when saving descriptions!!!" << std::endl;
      }	
    }
  }
}

/** Create the default parameters for the FED and template file name
 * This method is considered as "tatic"
 */
void CrateController::getFedVmeFileName ( std::string &vmeFileName, std::string &templateFileName ) {

  // Try to find the correct file for the VME file name for offset
  // and addresses
  vmeFileName.clear() ; templateFileName.clear() ;

  // $ENV_CMS_TK_FEC_ROOT/config/ directory
  char *basic=getenv ("ENV_TRACKER_DAQ") ;
  if (basic == NULL) {
    basic=getenv ("ENV_CMS_TK_FED9U_ROOT") ;
    vmeFileName = std::string(basic) + std::string( "/Fed9UVmeBase/Fed9UAddressTable.dat");
    templateFileName = std::string(basic) + std::string( "/Fed9UDescriptionFiles/TemplateDescription.xml");
  }
  else {
    vmeFileName = std::string(basic) + std::string( "/config/Fed9UAddressTable.dat");
    templateFileName = std::string(basic) + std::string( "/config/TemplateDescription.xml");
  }
  if (basic != NULL) {
    errorReportLogger_->errorReport("Basic FED directory " + std::string(basic), LOGDEBUG) ;
  }
  else {
    errorReportLogger_->errorReport("Not directory specified for the FED template files", LOGFATAL) ;
    vmeFileName = "/config/Fed9UAddressTable.dat" ;
    templateFileName = "/config/TemplateDescription.xml" ;
  }
}



/** This method tries to make the following redundance path:
 *  FEC-A-B firstCCU-B-A. If this is properly working, then the
 *  B input of the ring is sane
 *  \param FecAccess* a pointer to the proper FecAccess object
 *  \param keyType indexFecRing complete keyType for a given Fec/ring
 *  \param std::list < keyType > *ccuList a pointer to the already-built list of ccus
 *  \return a boolean value: is the B line sane? returns false also in case the procedure failed
 */
bool CrateController::checkRingBInput(FecAccess * fecAccess, keyType indexFecRing, std::list < keyType > *ccuList) {

  bool isBinputSane = false;
  struct timeval timeStart, timeMeasure;
  struct timezone tzp;
  bool waitmore;
 
  if (ccuList == NULL) {
    errorReportLogger_->errorReport ("FATAL: in CrateController::checkRingBInput() the ccuList is NULL", LOGFATAL);
    return false;
  }
  
  if (fecAccess == NULL) {
    errorReportLogger_->errorReport ("FATAL: in CrateController::checkRingBInput() the fecAccess is NULL", LOGFATAL);
    return false;
  }

  // Store all the addresses in a simple array
  keyType ccuAddresses[127];
  unsigned int nbCcu = 0;
  
  for (std::list < keyType >::iterator p = ccuList->begin(); p != ccuList->end(); p++) {
    keyType index = *p;
    ccuAddresses[nbCcu++] = index;
  }  

  
  try {

    // Disable the IRQ: not needed since it is not used in FecSDoftwareV3_0 (PCI,USB and VME)
    // fecAccess->getFecRingDevice(indexFecRing)->setIRQ(false);

    // First issue a ring reset
    fecAccess->fecRingReset(indexFecRing);
    fecAccess->fecRingResetB(indexFecRing);
    fecAccess->fecRingResetFSM(indexFecRing);

    // Then wait for the ring to be closed again (100 ms at most...)
    gettimeofday (&timeStart, &tzp);
    waitmore=true;
    do {
      usleep(100);
      gettimeofday (&timeMeasure, &tzp);
      waitmore = (! timeDiffMs( timeStart, timeMeasure, 100 ));
      if (((fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))&FEC_SR0_LINKINITIALIZED)==FEC_SR0_LINKINITIALIZED) {
        waitmore=false;
      }
    } while (waitmore);

    // Check if the ring was actually properly closed
    if (!isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {
      fecAccess->emptyFifoReceive(indexFecRing);
      fecAccess->emptyFifoTransmit(indexFecRing);
      fecAccess->emptyFifoReturn(indexFecRing);
    }
    if (!isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {

      if (displayFrameErrorMessages_) {
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: could not initialize properly the ring for the FEC output B on " << getFecKey(indexFecRing) 
				      << " ring " << getRingKey(indexFecRing)
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }

      errorReportLogger_->errorReport ("ERROR: could not initialize properly the ring for the FEC output B on FEC " + toString(getFecKey(indexFecRing)) + " ring " + toString(getRingKey(indexFecRing)), LOGERROR);
      return false;
    }

    // Everything's ok, so let's start with the ring reconfiguration
    {
      stringstream msgInfo ;
      msgInfo << "FEC " << getFecKey(indexFecRing) << " ring " << getRingKey(indexFecRing) << " Everything's ok, so let's start with the ring reconfiguration" ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }

    // Reconfigure the ring to use the B input:
    fecAccess->setCR0Receive(indexFecRing, false);
    fecAccess->setFecRingCR0(ccuAddresses[nbCcu - 1], FEC_CR0_SELSEROUT);
    fecAccess->setCcuCRC(ccuAddresses[0], CCU_CRC_ALTIN) ;
    fecAccess->setCR0Receive(indexFecRing, true);
    fecAccess->fecRingRelease(indexFecRing);

    // Now let's wait the ring to be closed again:
    // first wait for the ring to be closed again (100 ms at most...) ...
    gettimeofday (&timeStart, &tzp);
    waitmore=true;
    do {
      usleep(100);
      gettimeofday (&timeMeasure, &tzp);
      waitmore = (! timeDiffMs( timeStart, timeMeasure, 100 ));
      if (((fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))&FEC_SR0_LINKINITIALIZED)==FEC_SR0_LINKINITIALIZED) {
        waitmore=false;
      }
    } while (waitmore);
    // ... and then check if the ring was actually properly closed
    if (!isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {
      fecAccess->emptyFifoReceive(indexFecRing);
      fecAccess->emptyFifoTransmit(indexFecRing);
      fecAccess->emptyFifoReturn(indexFecRing);
    }
    if (!isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {

      if (displayFrameErrorMessages_) {
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: ring is not closed after the reconfiguration: ring B input is not working properly the FEC output B on " 
				      << getFecKey(indexFecRing) << " ring " << getRingKey(indexFecRing)
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }

      errorReportLogger_->errorReport ("ERROR: ring is not closed after the reconfiguration: ring B input is not working properly on FEC " + toString(getFecKey(indexFecRing)) + " ring " + toString(getRingKey(indexFecRing)), LOGERROR);
      return false;
    }

    // Now we saw the ring is closed. Let's ask to the first CCU if the input was actually switched.
    // If so (and we get the proper rely) the reconfiguration was performed corretcly and thus the
    // B input of the ring is OK

    {
      stringstream msgInfo ;
      msgInfo << "FEC " << getFecKey(indexFecRing) << " ring " << getRingKey(indexFecRing) << " After ring reconfiguration ring is slosed" ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }

    tscType8 firstCcuCRC;
    firstCcuCRC = fecAccess->getCcuCRC(ccuAddresses[0]);
    if( CCU_CRC_ALTIN  & firstCcuCRC ) {

      // Yuppie! Everithing's ok... let's party
      if (displayFrameErrorMessages_) {
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "WONDERFUL: ring is closed after the reconfiguration: ring B input is working properly for the FEC output B on " 
				      << getFecKey(indexFecRing) << " ring " << getRingKey(indexFecRing)
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }

      errorReportLogger_->errorReport ("WONDERFUL: ring is closed after the reconfiguration: ring B input is working properly " + toString(getFecKey(indexFecRing)) + " ring " + toString(getRingKey(indexFecRing)), LOGUSERINFO);
      isBinputSane = true;
    } else {
      // Damn: the ring is closed just because the FEC did not switch to B output
      // Note that in this case the first CCU was asked to switch to the B input, but the request arrived
      // into the A input and was ignored (as CCU specs state). The test failed, but the B input line could be still ok.

      if (displayFrameErrorMessages_) {
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "ERROR: ring is closed but the first CCU is still in A input. This test could not determine if the B input line is ok on FEC "
				      << getFecKey(indexFecRing) << " ring " << getRingKey(indexFecRing)
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }

      errorReportLogger_->errorReport ("ERROR: ring is closed but the first CCU is still in A input. This test could not determine if the B input line is ok on FEC " + toString(getFecKey(indexFecRing)) + " ring " + toString(getRingKey(indexFecRing)), LOGERROR);
      return false;
    }

  } catch (FecExceptionHandler &e) {

    if (displayFrameErrorMessages_) {
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "Error: exception thrown during " << __PRETTY_FUNCTION__ << " on FEC "
				    << getFecKey(indexFecRing) << " ring " << getRingKey(indexFecRing) 
				    << ": " << e.what() 
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }


    errorReportLogger_->errorReport ("Error: exception thrown during CrateController::checkRingBInput() on FEC " + toString(getFecKey(indexFecRing)) + " ring " + toString(getRingKey(indexFecRing)), e, LOGERROR) ;
    if (displayDebugMessage_) {
      std::cerr << "ERROR: exception thrown during CrateController::checkRingBInput()" << std::endl;
    }
  }
  
  return isBinputSane;
}

/** This method tries to find the dummy CCU.
 *  Warning: it deletes the ccu list you pass it.
 *  \param FecAccess* a pointer to the proper FecAccess object
 *  \param keyType indexFecRing complete keyType for a given Fec/ring
 *  \param std::list < keyType > *ccuList a pointer to the already-built list of ccus
 *  \param bool &tibinverted returns whether the ring topology is the "special" tib inverted one.
 */
keyType CrateController::findDummyCcuKey(FecAccess * fecAccess, keyType indexFecRing, std::list < keyType > *ccuList, bool &tibinverted)
{
                                                                                                                                  
  // Mersi: this looks nasty. But we need to know the TIB/TID dummy ccu address
  // in order to detect the dummy ccu under certain conditions, which means that
  // we have to store its address here. On the other side in the future we will
  // reteive this parameter formm the database, so we won't need this definition elsewhere

  struct timeval timeStart, timeMeasure;
  struct timezone tzp;
  bool waitmore;

  tibinverted = false;
 
  if (ccuList == NULL) return 0;
 
  keyType foundDummyCcuKey = 0;

  try {

    /******************************/
    /*                            */
    /* Store all the addresses    */
    /*                            */
    /******************************/
    keyType ccuAddresses[127];
    unsigned int nbCcu = 0;
 
    for (std::list < keyType >::iterator p = ccuList->begin(); p != ccuList->end(); p++) {
      keyType index = *p;
      ccuAddresses[nbCcu++] = index;
    }



    /******************************/
    /*                            */
    /* Start with a clean ring    */
    /*                            */
    /******************************/
    
    // First issue a ring reset
    fecAccess->fecRingReset();
    fecAccess->fecRingResetB();
    fecAccess->fecRingResetFSM();
    
    // Then wait for the ring to be closed again (100 ms at most...)
    gettimeofday (&timeStart, &tzp);
    waitmore=true;
    do {
      usleep(100);
      gettimeofday (&timeMeasure, &tzp);
      waitmore = (! timeDiffMs( timeStart, timeMeasure, 100 ));
      if (((fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))&FEC_SR0_LINKINITIALIZED)==FEC_SR0_LINKINITIALIZED) {
        waitmore=false;
      }
    } while (waitmore);
    
    // Check if the ring was actually properly closed
    if (!isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {
      fecAccess->emptyFifoReceive(indexFecRing);
      fecAccess->emptyFifoTransmit(indexFecRing);
      fecAccess->emptyFifoReturn(indexFecRing);
    }
    if (!isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {
      
      if (displayFrameErrorMessages_) {
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "ERROR: could not initialize properly the ring before searching for the dummy CCU for the FEC " << getFecKey(indexFecRing) 
				      << " ring " << getRingKey(indexFecRing)
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }

      errorReportLogger_->errorReport ("ERROR: could not initialize properly the ring before searching for the dummy CCU for the FEC " + toString(getFecKey(indexFecRing)) + " ring " + toString(getRingKey(indexFecRing)), LOGERROR);
      return false;
    }


    /***********************************/
    /*                                 */
    /* Check for the dummy CCU address */
    /*                                 */
    /***********************************/

    // Find the dummy CCU
    int errorCCU = 0, dummyCcu = false;
    fecAccess->setCR0Receive(indexFecRing, false);
    fecAccess->setFecRingCR0(ccuAddresses[nbCcu - 1], FEC_CR0_SELSERIN);
    fecAccess->setCcuCRC(ccuAddresses[nbCcu - 1], CCU_CRC_SSP);
    fecAccess->setCR0Receive(indexFecRing, true);
    fecAccess->fecRingRelease(indexFecRing);

    gettimeofday (&timeStart, &tzp);
    waitmore=true;
    do {
      usleep(100);
      gettimeofday (&timeMeasure, &tzp);
      waitmore = (! timeDiffMs( timeStart, timeMeasure, 100 ));
      if (((fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))&FEC_SR0_LINKINITIALIZED)==FEC_SR0_LINKINITIALIZED) {
        waitmore=false;
      }
    } while (waitmore);

    // Check if a dummy CCU exists
    if (!isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {
      fecAccess->emptyFifoReceive(indexFecRing);
      fecAccess->emptyFifoTransmit(indexFecRing);
      fecAccess->emptyFifoReturn(indexFecRing);
    }
    // DUMMY CCU by B -> A (TEC/TOB)
    if (isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {
	
      delete ccuList;
      // Scan the ring for each FECs and display the CCUs
      ccuList = fecAccess->getCcuList(indexFecRing);
 
      if (ccuList != NULL) {
	fecAccess->clearFecCcuErrorRegisters(indexFecRing);
 
	// Log Message
	if ((ccuList->size()) == (nbCcu+1)) {
	  std::list<keyType>::iterator it = ccuList->end();
	  it--;
	  foundDummyCcuKey = (*it);
	  dummyCcu = true;
	}
 
	delete ccuList;
      } else {
	errorReportLogger_->errorReport ("ERROR: no CCU found after the check of the dummy CCU for FEC "
					 + toString(getFecKey(indexFecRing)) + " Ring " + toString(getRingKey(indexFecRing)) , LOGERROR);
	errorCCU = 1;
      }
    } else {  // DUMMY CCU by B -> B (TIB inverted ring)

      // Mersi: Ouch, I have not a correct FecRingSR0... maybe it's a TIB/TID inverted dummy connection?
      // Well: let's see!
 
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

      keyType indexCCUDummyTIBTID = buildCompleteKey(getFecKey(indexFecRing), getRingKey(indexFecRing), CCUDUMMYTIBTIDADDRESS, 0, 0);
      fecAccess->setCR0Receive(indexFecRing, false);
      fecAccess->setCcuCRC(indexCCUDummyTIBTID, CCU_CRC_ALTIN);
      fecAccess->setCR0Receive(indexFecRing, true);
      fecAccess->fecRingRelease(indexFecRing);
      //fecAccess->setFecRingCR0 ( ccuAddresses[nbCcu-1], FEC_CR0_SELSERIN ) ;


      gettimeofday (&timeStart, &tzp);
      waitmore=true;
      do {
	usleep(100);
	gettimeofday (&timeMeasure, &tzp);
	waitmore = (! timeDiffMs( timeStart, timeMeasure, 100 ));
	if (((fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))&FEC_SR0_LINKINITIALIZED)==FEC_SR0_LINKINITIALIZED) {
	  waitmore=false;
	}
      } while (waitmore);

      // Check if a dummy CCU exists
      if (!isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {
	fecAccess->emptyFifoReceive(indexFecRing);
	fecAccess->emptyFifoTransmit(indexFecRing);
	fecAccess->emptyFifoReturn(indexFecRing);
      }
 
      if (isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu - 1]))) {

	delete ccuList;
 
	// Scan the ring for each FECs and display the CCUs
	ccuList = fecAccess->getCcuList(indexFecRing);
 
	if (ccuList != NULL) {
	  fecAccess->clearFecCcuErrorRegisters(indexFecRing);
 
	  // Log Message
	  if ((ccuList->size()) == (nbCcu+1)) {
	    std::list<keyType>::iterator it = ccuList->end();
	    it--;
	    foundDummyCcuKey = (*it);
	    tibinverted = true;
	    dummyCcu = true;
	  }
 
	  delete ccuList;
 
	} else {
	  // A proper error message is handled in the calling function
	  std::cerr << "ERROR: no CCU found after the check of the dummy CCU in TIB/TID ring" << std::endl;
	  errorCCU = 1;
	}
      } else {
	// A proper error message is handled in the calling function
	std::cerr << "ERROR: Dummy CCU, cannot find it, SR0 = 0x" << std::hex << fecAccess->getFecRingSR0(ccuAddresses[0]) << std::endl;
	errorCCU = 1;
      }
    }
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport ("ERROR: exception thrown during CrateController::findDummyCcuKey()", e, LOGERROR) ;
    if (displayDebugMessage_) {
      std::cerr << "ERROR: exception thrown during CrateController::findDummyCcuKey()" << std::endl;
    }
  }
 
 
  return foundDummyCcuKey;

}


// TODO: maybe change the name of this method?

/** Scans all the available rings and reads the order of CCUs 
 *  Then it oploads them to the database 
 */
//uploadCCUDB ( fecAccess_, partitionName_.toString(), displayDebugMessage_, deviceFactory_ ) ;
void CrateController::uploadCCUDB () {
  // Scan the PCI or VME slot for device driver loaded
  std::list<keyType> *fecSlotList = fecAccess_->getFecList( ) ;
  std::list<keyType> *ccuList = NULL ;
  ccuVector* vCcuVector = NULL;
  TkRingDescription* thisRing = NULL;
  CCUDescription* thisCcu = NULL;
  int ccuOrder;
  char debugFile[100];		// for debug
  std::string myFecHwId;

  {
    stringstream msgInfo ;
    msgInfo << "Will start now the check of CCUs for " << fecSlotList->size() << " FEC rings";
    errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
  }

  if ((fecSlotList != NULL) && (!fecSlotList->empty())) {
    for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {      
      keyType indexFEC = *p ;

      {
	stringstream msgInfo ;
	msgInfo << "Will start working on FEC " << getFecKey(indexFEC) << " ring " << getRingKey(indexFEC) ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
      }

      try {
	// Issue a ring reset here to be sure that noone applied the redundancy
	// before we start!
	fecAccess_->fecRingReset(indexFEC);
	fecAccess_->fecRingResetB(indexFEC);
	fecAccess_->fecRingResetFSM(indexFEC);

	// Scan the ring for each FECs and display the CCUs
	ccuList = fecAccess_->getCcuList( indexFEC ) ;
	myFecHwId = fecAccess_->getFecHardwareId( indexFEC );
	thisRing   = new TkRingDescription( crateId_ , indexFEC, true, true, true );
	thisRing->setFecHardwareId(myFecHwId,(tscType16)crateId_);
	vCcuVector = new ccuVector();

	ccuOrder=0;
	for (std::list<keyType>::iterator pCcu=ccuList->begin(); pCcu!=ccuList->end() ; pCcu++) {
	  thisCcu = new CCUDescription( crateId_ , (*pCcu), ++ccuOrder, true, true );
	  thisCcu->setFecHardwareId(myFecHwId,(tscType16)crateId_);
	  vCcuVector->push_back(thisCcu->clone());
	  delete thisCcu;
	}

	{
	  stringstream msgInfo ;
	  msgInfo << "Number of CCUs on FEC " << getFecKey(indexFEC) << " ring " << getRingKey(indexFEC) << " is " << vCcuVector->size() ;
	  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	}

	// First let's see if the B channel input into the ring is working fine...
	bool isBinputOk;
	isBinputOk = checkRingBInput(fecAccess_ , indexFEC, ccuList);

	// Find the dummy....
	bool istibinverted;
	keyType dummyKey = findDummyCcuKey(fecAccess_, indexFEC, ccuList, istibinverted);
	if (dummyKey) {
          if (!istibinverted) {
	    // NOT inverted - it's TEC/TOB-like topology: applies to TIB rings with an odd number of CCUs
	    thisCcu = new CCUDescription( crateId_ , dummyKey, DUMMYCCUARRANGEMENT, true, false );
          } else {
	    // IS  inverted - it's inverted topology: applies to TIB rings with an even number of CCUs
	    thisCcu = new CCUDescription( crateId_ , dummyKey, (DUMMYCCUARRANGEMENT | TIBINVERTEDDUMMY) , true, false );
          }
	  thisCcu->setFecHardwareId(myFecHwId,(tscType16)crateId_);
	  vCcuVector->push_back(thisCcu->clone());
	  delete thisCcu;

	  // Comment in the CrateController display
	  if (displayFrameErrorMessages_) {
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp;
	    temp << "Info: dummy CCU with ";
	    if (!istibinverted) {
	      temp << "straight" ;
	    } else {
	      temp << "inverted";
	    }
	    temp <<  " topology, with address 0x" << std::hex << getCcuKey(dummyKey) << std::dec << " was found for FEC "
		 << getFecKey(indexFEC) << " ring " << getRingKey(indexFEC) << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }
	} else {
	  errorReportLogger_->errorReport ("ERROR: dummy CCU not found for FEC " + toString(getFecKey(indexFEC)) + " Ring " + toString(getRingKey(indexFEC)), LOGERROR) ;
	  if (displayFrameErrorMessages_) {
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp;
	    temp << "ERROR: dummy CCU not found for FEC "
		 << getFecKey(indexFEC) << " ring " << getRingKey(indexFEC) << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }
	}

	if ((isBinputOk)&&(dummyKey)) {
	  errorReportLogger_->errorReport ("SUCCESS: Redundancy B line input/output is working properly for FEC " + toString(getFecKey(indexFEC)) + " Ring " + toString(getRingKey(indexFEC)), LOGUSERINFO) ;
	  if (displayFrameErrorMessages_) {
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp;
	    temp << "SUCCESS: Redundancy B line input/output is working properly for FEC "
		 << getFecKey(indexFEC) << " ring " << getRingKey(indexFEC) << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }
	}

	
	
	thisRing->setCcuVector(*vCcuVector);
	
	if (doUploadInFile_) {
	  // TODO: read the user-chosen directory... and use it in place of /tmp
	  // take it from xmlFECFileName_ 
	  int myCrate = crateId_;
	  sprintf(debugFile, "/tmp/ccu-Crate%03d-Fec%02d-Ring%d.xml", myCrate, getFecKey(indexFEC), getRingKey(indexFEC) );
	  deviceFactory_->setFileRingDescription(debugFile, *thisRing) ;
	  errorReportLogger_->errorReport ("Dummy CCU is now uploaded in the debug file " + toString(debugFile) + " for FEC " + toString(getFecKey(indexFEC)) + " ring " + toString(getRingKey(indexFEC)), LOGINFO) ;
	}
	if (doUploadInDatabase_) {
	  deviceFactory_->setDbRingDescription(partitionName_.toString(), *thisRing) ;
	  errorReportLogger_->errorReport ("Dummy CCU is now uploaded in the database for FEC " + toString(getFecKey(indexFEC)) + " ring " + toString(getRingKey(indexFEC)), LOGINFO) ;
	}

	// Let's delete useful data after having uploaded them:
	// TODO: uncomment the following line as soon as you feel like doing it!
	// for (ccuVector::iterator it=vCcuVector->begin(); it!=vCcuVector->end(); it++) delete (*it);
	vCcuVector->clear();
	delete vCcuVector;
	
      }
      
      catch (FecExceptionHandler &e) {

	if (displayFrameErrorMessages_) {
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: exception thrown during " << __PRETTY_FUNCTION__ << " on FEC "
					<< getFecKey(indexFEC) << " ring " << getRingKey(indexFEC) 
					<< ": " << e.what() 
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}


	errorReportLogger_->errorReport ("ERROR: exception thrown during CrateController::uploadCCUDB() on FEC " + toString(getFecKey(indexFEC)) + " ring " + toString(getRingKey(indexFEC)), e, LOGERROR) ;
	if (displayDebugMessage_) {
	  std::cerr << "ERROR: exception thrown during CrateController::uploadCCUDB()" << std::endl; 
	}
      }
    }
  }
}

