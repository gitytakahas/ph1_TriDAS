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
#ifndef CRATECONTROLLER_H
#define CRATECONTROLLER_H

#include <string>

  // XDAQ includes
#include "xdata/UnsignedLong.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"

#include "FecExceptionHandler.h"

#include "FsmWithStateName.h"

// I2O messaging
#include "TrackerCommandListener.h"

// Error reporting
#include "ErrorReportLogger.h"
#ifdef TKDIAG
#  include <diagbag/DiagBagWizard.h>
#endif

/** Database access
 */
#include "DeviceFactory.h"

/** FEC access
 */
#include "FecAccess.h"
#include "FecAccessManager.h"

#include <sys/time.h>

/**
 * \class CrateController
 * \version 1.0
 * \author Frederic Drouhin
 * \date Febrary 2006
 */
class CrateController: public TrackerCommandListener {

 private:

  /** Difference of time
   */
  bool timeDiffMs(timeval &first, timeval &second, int msecdelay);

  /** Crate reset
   */
  bool crateReset_ ;

  /** Reload firmware on FECs
   */
  xdata::Boolean fecReloadFirmware_ ;

  /** Crate Id
   */
  xdata::UnsignedLong crateId_ ;

  /** Number of crates on this crate controller (daisychained)
   */
  xdata::UnsignedLong numberOfCrates_ ;

  /** Scan rang min
   */
  xdata::UnsignedLong vmeSlotBegin_ ;

  /** Scan rang min
   */
  xdata::UnsignedLong vmeSlotEnd_ ;

  /** Partition name, only usefull to upload data into the database
   */
  xdata::String partitionName_ ;

  /** Partition name, only usefull to upload data into the database
   */
  xdata::String subDetector_ ;

  /** Room temperature for the APV/MUX parameters
   */
  xdata::String roomTemperature_ ;

  /** Force the upload in file if the database is set
   */
  xdata::Boolean doUploadInFile_ ;

  /** Force the upload in the database (an access must be given)
   */
  xdata::Boolean doUploadInDatabase_ ;

  /** Scan for FECs
   */
  xdata::Boolean doScanFECs_ ;

  /** Scan for FECs
   */
  xdata::Boolean doScanFEDs_ ;

  /** type of the FEC
   */
  xdata::String fecBusType_ ;
  
  /** bus adapter used
   */
  xdata::String strBusAdapter_ ;

  /** Database access, this value can be set dynamically (properties of the
   * xdaqWin application or any run control) or in static by disactivate the
   * database in the Makefile (exported)
   */
  bool databaseAccess_ ;

  /** Login on database
   */
  xdata::String dbLogin_ ;

  /** Passwd on database (read only, will never display)
   */
  xdata::String dbPasswd_ ;

  /** path to database
   */
  xdata::String dbPath_ ;

  /** Device factory to retreive or set values in database or files
   */
  DeviceFactory *deviceFactory_ ;

  /** Application final state machine: toolbox::fsm::FiniteStateMachine fsm_;
   */
  FsmWithStateName fsm_;

  /** Error reporting
   */
  ErrorReportLogger *errorReportLogger_ ;

#ifdef TKDIAG
  /** Error reporting through the diagnostic system
   */
  DiagBagWizard *diagService_;
#endif

  /** display all messages
   */
  bool displayDebugMessage_ ;

  /** Navigation bar for the related links
   */
  std::vector<std::string> relatedLinksNavigation_;

  /** Position into the links
   */
  unsigned int linkPosition_ ;

  /** Navigation bar for the hardware check
   */
  std::vector<std::string> hardwareCheckNavigation_;

  /** Error or information or warning must be put on this stringstream
   */
  std::stringstream textInformation_ ;

  /** Display the error messages in the web page
   */
  bool displayFrameErrorMessages_ ;

  // -----------------------------------------------
  // Error during the apply of the parameters
  //  - Partition name is empty or is already used in the DB
  //  - Scan for FECs and scan for FEDs is set
  //  - FED template not existing
  //  - FEC template not existing
  bool errorInParameters_ ;
  std::ostringstream msgErrorInParameterSettings_ ;

  // -----------------------------------------------
  // FEC part

  /** FEC VME file name
   */
  xdata::String fecVmeFileName_ ;

  /** FEC VME file name for plug and play
   */
  xdata::String fecVmeFileNamePnP_ ;

  /** FEC template file name
   */
  xdata::String fecTemplateFileName_ ;

  /** File output for FED
   */
  xdata::String xmlFEDFileName_ ;

  /** ring min and ring max
   */
  unsigned int ringMin_, ringMax_ ;

  /** ring for the display of the information
   */
  keyType indexFecRingChosen_ ;

  /** Multi frames method used
   */
  bool multiFrames_ ;

  /** VME Block transfer used
   */
  bool blockMode_ ;

  /** Remove the modules without the AOH, PLL, MUX
   */
  xdata::Boolean removeBadModules_ ;

  /** Do the DOH calibration and upload it in a major version
   */
  xdata::Boolean doCalibrationDOH_ ;

  /** Gain value to be used for the DOH calibration
   */
  unsigned char dohGainValue_ ;

  /** Scan for CCUs and dummy CCU
   */
  xdata::Boolean doScanRedundancy_ ;

  /** Force the reconfiguration of the ring
   */
  xdata::Boolean forceApplyRedundancy_ ;

  /** Scan for conversions factors
   */
  xdata::Boolean doCheckTkDcuConversion_ ;

  /** Access to the hardware
   */
  FecAccess *fecAccess_ ;

  /** Management of the devices
   */
  FecAccessManager *fecAccessManager_ ;

  /** List of the device in error
   */
  Sgi::hash_map<keyType, bool> deviceError_ ;

  /** List of the device missing
   */
  Sgi::hash_map<keyType, bool> deviceMissing_ ;

  /** List of the modules with and without errors
   */
  std::list<keyType> moduleCorrect_ ;
  std::list<keyType> moduleIncorrect_ ;
  std::list<keyType> moduleCannotBeUsed_ ;
  std::list<keyType> dcuDohOnCcu_ ;

  /** Map of the modules and devices
   */
  Sgi::hash_map<keyType, bool *> trackerModule_ ;

  // -----------------------------------------------
  // FED part

  xdata::Boolean fedTrimDac_;
  xdata::String fedClockTrigger_;
  xdata::Boolean fedReadTemp_;
  xdata::Boolean fedFirstUpload_;
  xdata::UnsignedLong fedStartIdOffset_;

  /** FED VME file name
   */
  xdata::String fedVmeFileName_ ;

  /** FED template file name
   */
  xdata::String fedTemplateFileName_ ;
  
  /** File output for FEC
   */
  xdata::String xmlFECFileName_ ;

  /** Software tag version set by CVS
   */
  xdata::String softwareTagVersion_ ;

  /** hostname
   */
  std::string hostname_ ;

  /** ID assigned
   */
  bool isVsHostnameAssigned_ ;

 public:

  // --------------------------------------------------------- XDAQ call

  XDAQ_INSTANTIATOR();

  // --------------------------------------------------------- XDAQ Definition

  /** \brief Constructor for the initialisation of the xdata/export parameters
   * \warning nothing is created in this method (nor DeviceFactory, Database access), see crateControllerInitialise
   */
  CrateController( xdaq::ApplicationStub* stub ) ;

  /** \brief Destructor that remove all the accesses done to the hardware 
   * and destroy the connection with the database
   */
  ~CrateController ( ) ;

  // ------------------------------------------------------- Finite state machine

  /** \brief configure state 
   */
  void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  /** \brief halt state 
   */
  void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  // ------------------------------------------------------- Database
  /** \brief database creation
   */
  void createDatabaseAccess ( ) ;

  /** \brief check if the given hostname is registered in the DB
   */
  void identifyIdVsHostname ( ) ;

  // ------------------------------------------------------- I2O message

  /** \brief receive all i2o messages and dispatch it
   */
  toolbox::mem::Reference* userTrackerCommandHandler(PI2O_TRACKER_COMMAND_MESSAGE_FRAME com) ;

  // ------------------------------------------------------- actionPerformed

  /** \brief When an item is fired
   */
  void actionPerformed(xdata::Event& event) ;

  /** FSM Soap message
   */
  xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception) ;

  // ------------------------------------------------------- Web methods

  /** \brief Default web page with frames
   */
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) ;

  /** \brief finite state machine web page 
   */
  void displayStateMachine (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief finite state machine action
   */
  void dispatch (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief finite state machine web page 
   */
  void displayFecHardwareCheck (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Web page for the parameters for the upload
   */
  void displayMoreParameters(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Apply the parameters described just before
   */
  void crateControllerParameterSettings (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Display the related links
   */
  void displayRelatedLinks(xgi::Input * in, xgi::Output * out, bool withHTMLTag = false) throw (xgi::exception::Exception) ;

  /** \brief failure
   */
  void failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  // --------------------------------- Diagnostic system call back

  /** \brief Method called when a recovery is done by the 1st level
   */
  xoap::MessageReference crateControllerRecovery (xoap::MessageReference msg) 
    throw (xoap::exception::Exception)  ;

  /** \brief Web page called for the diagnostic system
   */
  void displayConfigureDiagSystem ( xgi::Input * in, xgi::Output * out ) ;

#ifdef TKDIAG
  /** \brief display the form of the diagnostic web pages
   */
  void getDiagSystemContent(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) ;

  /** \brief Display all the fiels from the diagnostic system
   */
  void DIAG_CONFIGURE_CALLBACK();

  /** \brief Callback that process the DIAG_CONFIGURE_CALLBACK
   */
  void DIAG_APPLY_CALLBACK();

  /** \brief callback XOAP for the ack of the error message processing (local)
   */
  DIAG_FREELCLSEM_CALLBACK();

  /** \brief callback XOAP for the ack of the error message processing (global)
   */
  DIAG_FREEGLBSEM_CALLBACK();

  /** \brief callback XOAP for remote control pannel of LogReader application (global)
   */
  DIAG_REQUEST_ENTRYPOINT();

#endif

  // ----------------------------------- FEC methods

  /** \brief Retreive the VME configuration files for the FED
   */
  void getFecVmeFileName ( std::string &templateFileName, std::string fileName ) ;

  /** \brief Create the access for a FEC
   */
  FecAccess *createFecAccess ( ) ;

  /** \brief Upload the FECs to the output (file or database)
   */
  void uploadDetectFECs ( ) ;

  /** \brief Upload the DOH calibrated
   */
  void uploadDOHCalibrated ( ) ;

  // ----------------------------------- FED methods

  /** \brief Retreive the VME configuration files for the FED
   */
  void getFedVmeFileName ( std::string &vmeFileName, std::string &templateFileName ) ;

  /** \brief Upload the FEDs to the output (file or database)
   */
  void uploadDetectFEDs ( ) ;

  // ---------------------------------- Generic method for the FEC

  /** Display the list of modules in a list
   */
  void displayListModulesInList ( xgi::Output * out, 
				  std::string legendFieldName, std::string nameAnchor,
				  std::string foundModule,
				  std::list<keyType> modulesToDisplay,
				  Sgi::hash_map<keyType, bool> deviceError,
				  Sgi::hash_map<keyType, bool> deviceMissing ) ;

  /** \brief display the navigation bar for the hardware check
   */
  void displayHardwareCheckNavigation (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display all the FEC registers
   */
  void displayFecRingRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief action corresponding to displayFecRingRegisters method
   */
  void setFecControlRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the redundancy
   */
  void displayRedundancy(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the CCS Trigger of the FEC
   */
  void displayCCSTrigger(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief handle the command for CCS registers
   */
  void setCCSControlRegisters (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception) ;

  /** \brief display all the CCU / modules
   */
  void displayCcuModules(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the corresponding CCU
   */
  void displayCCU (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief action to be done when the form provide by the displayCCU is set
   */
  void setCcuControlRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the corresponding module
   */
  void displayModule (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the corresponding DCU on CCU
   */
  void displayDCUCCU (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the corresponding DCU on CCU
   */
  void displayDOH (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;
  
  /** \brief display a FEC with all rings and some information
   */
  void displayFecRingField (xgi::Input * in, xgi::Output * out, unsigned int fecSlot) throw (xgi::exception::Exception) ;

#define CCUDUMMYTIBTIDADDRESS 0x7F
#define DUMMYCCUARRANGEMENT   0x7F 
#define TIBINVERTEDDUMMY      0x80 

  bool checkRingBInput(FecAccess * fecAccess, keyType indexFecRing, std::list < keyType > *ccuList) ;

  keyType findDummyCcuKey(FecAccess * fecAccess, keyType indexFecRing, std::list < keyType > *ccuList, bool &tibtidinverted) ;

  /** \brief scans the rings for CCUs, reading their order and uploads it to the database
   */
  void uploadCCUDB () ;
  //void uploadCCUDB ( FecAccess *fecAccess, std::string partitionNameStr, bool displayDebugMessage, DeviceFactory *deviceFactory ) ;

} ;

#endif

