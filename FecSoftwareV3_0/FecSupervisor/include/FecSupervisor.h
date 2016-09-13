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
#ifndef FECSUPERVISOR_H
#define FECSUPERVISOR_H

#include <string>

  // XDAQ includes
#include "xdata/UnsignedLong.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"

  // for the work loop and timer
#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/task/WaitingWorkLoop.h" 
#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/BSem.h"
//#include "FsmWithStateName.h"
#include "StateMachine.h"

  // FEC includes
#include "HashTable.h"

#ifdef DATABASE
#include "DbFecAccess.h"
#endif

#include "FecExceptionHandler.h"

#include "i2cAccess.h"
#include "FecFactory.h"
#include "FecAccessManager.h"
#include "FecAccess.h"

// I2O messaging
#include "TrackerCommandSender.h"
#include "jsinterface.h"
#include "xdaqTrackerDcuDefs.h"

#ifdef TRACKERXDAQ
#  include "DbClient.h"
#endif

// Error reporting
#include "ErrorReportLogger.h"
#ifdef TKDIAG
#  include <diagbag/DiagBagWizard.h>
#endif

// XML File unknown
#define UNKNOWNFILE "Unknown"

// DCU loop time
#define WORKLOOPTIME 30

/**
 * \class FecSupervisor
 * This class define the FEC supervisor. It is able to download the values issued from
 * a database (ORACLE database) into the hardware thought the FecAccess class. It is
 * also able to upload from the hardware devices into the database. The order can comes
 * from a filter unit (for specific manipulation like calibration ...) and from the run 
 * control for download or upload. So the FEC supervisor is a listener for
 * i2o message and SOAP message.
 * SOAP messages for the DCU are also implemented via the method soapGetDcuInformation
 * \version 4.0
 * \author Frederic Drouhin
 * \date December 2002
 * \warning the Soap message are not implemented, wait for any run control
 * \warning the xdaqApplication class is done by the official CMS data acquistion system.
 * \warning All the hash table use a key defined in the file keyType.h:
 * \include keyType.h
 * \warning This class use XML. The DTD can be fine in the root directory in xml directory.
 * \brief This class make the link between a database (where device values are stored) or
 * a XML file and the hardware devices.
 */
class FecSupervisor: public TrackerCommandSender, public JsInterface, public toolbox::task::TimerListener {

 private:

  // ------------------------------------------------------------------- FSM

  /** using a rung with redundancy; determined by setRedundancy 
   */
  bool hasRedundancy_;

  /** Application final state machine
   */
  StripTrackerFSM::StateMachine fsm_;  

  /** display all messages
   */
  bool displayDebugMessage_ ;

  // ------------------------------------------------------------------- display error messages

  /** display all errors, fatal and warnings in the web pages
   */
  bool displayFrameErrorMessages_ ;

  /** Display error messages
   */
  bool displayDcuDebugMessage_ ;

  /** Display error messages
   */
  bool displayDeviceDebugMessage_ ;

  /** Display error on console
   */
  bool displayErrorOnConsole_ ;

  // ----------------------------------------------------------------- Initialise the objects for the accesses

  /** Concurrent access to the hardware via the FecAccess class
   */
  FecAccess *fecAccess_ ;

  /** All the accesses is managed in a class
   */
  FecAccessManager *fecAccessManager_ ;

  // ----------------------------------------------------------------- Hardware access

  /** Multi frames algorithm
   */
  bool multiFrames_ ;

  /** VME Block mode
   */
  xdata::Boolean  blockMode_ ;

  /** Fix ring in tib fec 21
   */
  xdata::Integer  fedid_ ;

  /** Apply a crate reset once on the system
   */
  xdata::Boolean crateReset_ ;

  /** Reset a FEC via a PLX reset (exported)
   */
  xdata::Boolean reloadFirmware_  ;

  /** Initialise the TTCRx (exported)
   */
  bool initTTCRx_ ;

  /** SSID
   */
  xdata::String SSID_ ; // see CCSTrigger

  /** Reset a FEC (expored)
   */
  xdata::Boolean resetFec_  ;

  /** Apply PIA reset at cold start on the first configuration
   */
  bool initPia_ ;

  /** Do the reset of the modules when the boolean is set during configuration
   * \warning this reset is done through PIA channels and it is only avalaible
   * on CCU 25 (exported)
   */
  xdata::Boolean resetPia_ ;

  /** I2C speed, export param
   */
  unsigned short i2cSpeed_ ;

  /** Configure the DOH
   */
  xdata::Boolean configureDOH_ ;

  /** Force a cold reset for the pll
   */
  xdata::Boolean coldPllReset_ ;

  /** FEC hardware ID
   */
  std::string fecHardwareId_ ;

  /** FEC slot 
   */
  xdata::UnsignedLong fecSlot_ ;

  /** VME file name
   */
  xdata::String vmeFileName_ ;

  /** VME file name
   */
  xdata::String vmeFileNamePnP_ ;
  
  /** type of the FEC
   */
  xdata::String fecBusType_ ;

  /** bus adapter used
   */
  xdata::String strBusAdapter_ ;

  // ----------------------------------------------------------------- Redundancy

  /** For the apply of redundancy configuration
   */
  xdata::Boolean forceApplyRedundancy_  ;

  /** Apply the redundancy
   */
  xdata::Boolean applyRedundancy_  ;

  /** Redundancy applied
   */
  bool redundancyRings_[9] ;

  // ----------------------------------------------------------------- Firmware expected

  /** FEC Firmware version 
   */
  xdata::UnsignedLong fecFirmwareVersionExpected_ ;

  /** VME Firmware version 
   */
  xdata::UnsignedLong vmeFirmwareVersionExpected_ ;

  /** Trigger Firmware version 
   */
  xdata::UnsignedLong triggerFirmwareVersionExpected_ ;

  // ----------------------------------------------------------------- Shared memory

#ifdef TRACKERXDAQ
  /** Share memory to the DbClient
   */
  xdata::String shareMemoryName_ ;

  /** Database client for caching system
   */
  DbClient *dbClient_ ;
#endif

  // ----------------------------------------------------------------- Database access

  /** Device factory to retreive or set values in database or files
   */
  FecFactory *fecFactory_ ;

  /** Login on database
   */
  xdata::String dbLogin_ ;

  /** Passwd on database (read only, will never display)
   */
  xdata::String dbPasswd_ ;

  /** path to database
   */
  xdata::String dbPath_ ;

  /** Database access, this value can be set dynamically (properties of the
   * xdaqWin application or any run control) or in static by disactivate the
   * database in the Makefile (exported)
   */
  bool databaseAccess_ ;

  /** Check if the database download is performed
   */
  xdata::Boolean databaseDownload_ ;

  /** Partition name
   */
  xdata::String partitionName_ ;

  /** Partition number, only in case of database
   */
  unsigned int partitionId_ ;

  /** Version to be used for the download
   */
  unsigned int versionMajorId_, versionMinorId_ ;

  /** Version to be used for the mask
   */
  unsigned int maskMajor_, maskMinor_ ;

  /** Is the version is initialised
   */
  bool initDbVersion_ ;

  /** version for the PIA reset (not anymore used, no version is available for it)
   */
  unsigned int versionPiaMajor_, versionPiaMinor_ ;
  bool fcon_[8][500];

  // ----------------------------------------------------------------- General parameter

  /** Download from the database or from a file
   * the values to the hardware (exported)
   */
  bool doDownload_ ;

  /** Upload in the database or in a file
   * the values from the hardware (exported)
   */
  bool doUpload_ ;

  /** What was the state of the upload at the origin. This attribut
   * is added in order to do not make the confusion between the doUpload
   * parameter and the upload for DCUs
   */
  bool doUploadOrigin_ ;

  /** Do the comparison when the upload was done: what you write is what is you read
   * \warning note that only the differences are stored in database or file (exported)
   */
  bool doComparison_ ;

  /** Force the download of all devices, do not use the intelligent configuration
   */
  xdata::Boolean forceDownload_ ;

  /** If the database parameters (version) changed then the full download should be performed
   */
  bool dbClientChanged_ ;                          

  /** true if an upload was done (for comparison of parameters and intelligent download)
   */
  bool setUploadDone_ ;

  /** XML File name for the default values for the hardware. The download can be
   * done in two ways, the database or an XML file. (exported)
   */
  xdata::String xmlFileName_ ;

  /** Force the upload in file if the database is set
   */
  bool doUploadInFile_ ;  

  /** XML output file for upload
   */
  std::string xmlFileNameOutput_ ;

  /** XML output file for DCU upload
   */
  std::string xmlFileNameOutputDcu_ ;

  /** Maximum number of errors allowed
   */
  xdata::UnsignedLong maxErrorAllowed_ ;

  // ----------------------------------------------------------------- DCU & device work loop

  /** loop time for the DCU thread (exported)
   */
  xdata::UnsignedLong dcuWorkLoopTime_ ;

  /** loop time for the DCU thread (exported)
   */
  xdata::UnsignedLong deviceWorkLoopTime_ ;

  /** Counter in the timer to know if the DCU should be readout or not
   */
  unsigned int counterDcuTime_ ;

  /** Counter in the timer to know if the DCU should be readout or not
   */
  unsigned int counterDeviceTime_ ;

  /** Do I used the DCU Thread ? (exported)
   */
  xdata::Boolean dcuThreadUsed_ ;

  /** Do I used the device Thread ? (exported)
   */
  xdata::Boolean deviceThreadUsed_ ;

  /** Waiting work loop name
   */
  xdata::String workLoopName_ ;

  /** Working loop in the system
   */
  toolbox::task::WorkLoop* workFecSupervisorLoop_ ;

  /** Is the work loop suspended ?
   */
  bool suspendDcuWorkLoop_ ;

  /** Is the work loop suspended ?
   */
  bool sleepDcuWorkLoop_ ;

  /** Is the work loop suspended ?
   */
  bool suspendDeviceWorkLoop_ ;

  /** Is the work loop suspended ?
   */
  bool sleepDeviceWorkLoop_ ;

  /** Just for display first time of DCU work loop message
   */
  bool firstTimeDcuWorkLoop_ ;

  /** Just for display first time of device work loop message
   */
  bool firstTimeDeviceWorkLoop_ ;

  // ----------------------------------------------------------------- Method Workloop

  /** method to be activated by the work loop
   */
  toolbox::task::ActionSignature* dcuReadoutWorkLoop_ ;

  /** method to be activated by the work loop
   */
  toolbox::task::ActionSignature* deviceReadoutWorkLoop_ ;

  /** timer for the DCU readout
   */
  toolbox::task::Timer * readoutTimer_;

  // ----------------------------------------------------------------- DcuFilter

  /** DcuFilter application description
   */
  xdaq::ApplicationDescriptor *dcuFilterDes_ ;

  /** Instance of the DCU Filter
   */
  xdata::UnsignedLong dcuFilterInstance_ ; // should be UnsignedInterger32 but compilation problem

  /** Class name of the DCU filter
   */
  xdata::String dcuFilterClassName_ ;

  // ------------------------------------------------------------------ Status

  /** XMAS status for the ring 8 (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECRing8_ ;

  /** XMAS status for the ring 7 (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECRing7_ ; 

  /** XMAS status for the ring 6 (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECRing6_ ;

  /** XMAS status for the ring 5 (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECRing5_ ;

  /** XMAS status for the ring 4 (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECRing4_ ;

  /** XMAS status for the ring 3 (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECRing3_ ;

  /** XMAS status for the ring 2 (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECRing2_ ;

  /** XMAS status for the ring 1 (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECRing1_ ;

  /** XMAS status for the PCI FEC (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECPCI_ ;

  /** XMAS status for the USB FEC (0xFFFFFFFF: not read or error)
   */
  xdata::UnsignedLong statusFECUSB_ ;

  /** XMAS status for QPLL error (0xFFFFFFFF: not read, 1: error, 0 not error)
   */
  xdata::UnsignedLong statusQPLLError_ ;

  /** XMAS status for QPLL lock (0xFFFFFFFF: not read, 1: locked, 0 not locked)
   */
  xdata::UnsignedLong statusQPLLLocked_ ;

  /** XMAS status for TTCRx (0xFFFFFFFF: not read, 1: ready, 0 not ready)
   */
  xdata::UnsignedLong statusTTCRx_ ;

  /** XMAS SSID read from hardware (NONE: not read)
   */
  xdata::String statusSSID_ ;

  /** XMAS ring firmware version (0xFFFFFFFF: not read)
   */
  xdata::UnsignedLong ringFirmwareVersion_ ;

  /** XMAS VME firmware version (0xFFFFFFFF: not read)
   */
  xdata::UnsignedLong vmeFirmwareVersion_ ;

  /** XMAS trigger firmware version (0xFFFFFFFF: not read)
   */
  xdata::UnsignedLong triggerFirmwareVersion_ ;

  /** XMAS temperature (0xFFFFFFFF: not read)
   */
  xdata::UnsignedLong fecTemperature1_ ;

  /** XMAS temperature (0xFFFFFFFF: not read)
   */
  xdata::UnsignedLong fecTemperature2_ ;

  // ------------------------------------------------------------------ Web link

  /** Position into the links
   */
  unsigned int linkPosition_ ;

  /** Navigation bar for the related links
   */
  std::vector<std::string> relatedLinksNavigation_;

  /** Navigation bar for the hardware check
   */
  std::vector<std::string> hardwareCheckNavigation_;

  // ----------------------------------------------------------------- Diagnostic system and error reporting

  /** Error reporting
   */
  ErrorReportLogger *errorReportLogger_ ;

#ifdef TKDIAG
  /** Error reporting through the diagnostic system
   */
  DiagBagWizard *diagService_;
#endif

  // ------------------------------------------------------------------ Other parameters

  /** Semaphore to avoid duplicate access on the same ring
   */
  toolbox::BSem *mutexThread_ ;

  int mutexTaker_;
  bool dcuStuck_;

  /** Status of the XDAQ application (last action performed)
   */
  xdata::String xdaqApplicationStatus_ ;

  /** Status of the XDAQ application (last action performed)
   different one in order to avoid double free while assigning it in concurrent code
   */
  xdata::String xdaqApplicationStatus2_ ;

  /** Error or information or warning must be put on this stringstream
   */
  std::stringstream textInformation_ ;

  /** Reason of failure 
   */
  std::string reasonOfFailure_ ;

  /** Software tag version set by CVS
   */
  xdata::String softwareTagVersion_ ;

  // ------------------------------------------------------------------ For browsing hardware

  /** ring min and ring max
   */
  unsigned int ringMin_, ringMax_ ;

  /** Index of the FEC
   */
  keyType indexFecRingChosen_ ;

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

  /** Devices read from the database, not used
   */
  deviceVector deviceValues_ ;

  /** Devices if it has to be changed, not used
   */
  Sgi::hash_map<keyType,deviceDescription *> hashMapFaultyDevice_ ;

 public:

  // --------------------------------------------------------- XDAQ call

  XDAQ_INSTANTIATOR();

  // --------------------------------------------------------- XDAQ Definition

  /** \brief Constructor for the initialisation of the xdata/export parameters
   * \warning nothing is created in this method (nor FecFactory, FecAccess, Database access, FecAccessManager), see fecSupervisorInitialise
   */
  FecSupervisor ( xdaq::ApplicationStub* stub ) ;

  /** \brief Destructor that remove all the accesses done to the hardware 
   * and destroy the connection with the database
   */
  ~FecSupervisor ( ) ;

  // ------------------------------------------------------- Parameters from constructor
  /** \brief Fire item on all the parameters needed by this application
   */
  void exportParameters ( )  ;

  /** \brief Define all web methods for web pages
   */
  void webGUI ( ) ;

  // ------------------------------------------------------- Finite state machine
  /** \brief initiliase all the paremters for the FecSupervisor
   */
  //void initialiseAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;
  bool initialising(toolbox::task::WorkLoop* wl) ;

  /** \brief initiliase all the paremters for the FecSupervisor
   */
  //void destroyAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;
  bool destroying(toolbox::task::WorkLoop* wl) ;

  /** \brief configure command 
   */
  //void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;
  bool configuring(toolbox::task::WorkLoop* wl) ;

  /** \brief dcuconfigure command 
   */
  //void configureDcuAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;
  bool dcuconfiguring(toolbox::task::WorkLoop* wl) ;

  /** \brief halt command
   */
  //void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;
  bool halting(toolbox::task::WorkLoop* wl) ;

  /** \brief fixingSoftError command 
   */
  bool fixingSoftError(toolbox::task::WorkLoop* wl) ;

  /** \brief Pause command 
   */
  bool pausing(toolbox::task::WorkLoop* wl) ;

  /** \brief Resume command
   */
  bool resuming(toolbox::task::WorkLoop* wl) ;

  /** \brief Enable command 
   */
  bool enabling(toolbox::task::WorkLoop* wl) ;

  /** \brief disable command
   */
  bool disabling(toolbox::task::WorkLoop* wl) ;

  /** \brief Fail command
   */
  void failAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  /** FSM call back
   */
  xoap::MessageReference fsmCallback(xoap::MessageReference msg) throw (xoap::exception::Exception) {
    
    return fsm_.commandCallback(msg);
  }

  // ------------------------------------------------------- Independant methods

  /** \brief make the configuration of FECs / devices
   */
  void hardwareConfiguration (int ringslot) ;

  /** \brief delete all the accesses
   */
  void deleteAccesses ( ) ;

  /** \brief retrieve the VME file name for the VME FEC
   */
  bool getVmeFileName ( std::string &vmeFileName, std::string &vmeFileNamePnP ) ;

  /** \brief Create a FecAccess, this method is both compatible with the VME and PCI FECs
   */
  FecAccess *createFecAccess ( ) ;

  // -------------------------------------------------------  Database methods

  /** \brief Create an access to the database
   */
  void createDatabaseAccess ( ) ;

  /** \brief retreive the value for partition/version
   */
  void getPartitionVersion ( ) ;

  /** \brief set the version in database for a partition
   */
  void setPartitionVersion ( ) ;

  // ------------------------------------------------------- Download / upload

  /** \brief Download the values in the database
   */
  bool setDownload (int ringslot) ;

  /** \brief Upload it in the database
   */
  bool setUpload   ( ) ;

  // ------------------------------------------------------- DCU / device work loop

  /** \brief submit the DCU and device readout to the work loop
   */
  void submitWorkLoopReadout ( ) ;

  /** \brief Timer method called, this method inject the method dcuReadWorkLoop and deviceReadoutWorkLoop in the queue to be executed
   */
  void timeExpired (toolbox::task::TimerEvent& e) ;

  /** \brief Manage the work loop (start and time between two call to the readout methods
   */
  void manageWorkLoop ( ) ;

  /** \brief Stop the DCU work loop if it is not used
   */
  void stopWorkLoop ( ) ;

  /** \brief Method to readout the DCU by the work loop
   */
  bool dcuReadoutWorkLoop (toolbox::task::WorkLoop* wl) ;

  /** \brief Manage the DCU readout with the work loop
   */
  void manageDcuWorkLoop ( ) ;

  /** \brief Manage the Device readout with the work loop
   */
  void manageDeviceWorkLoop ( ) ;

  /** Method to readout the devices by the work loop (same as the DCU work loop)
   */
  bool deviceReadoutWorkLoop (toolbox::task::WorkLoop* wl) ;

  /** \brief method to create a DCU thread
   */
  void detectDCU ( ) ;

  // -------------------------------------------------------- Method for all the possible reset (PLX, FEC, TTCRx, PIA)

  /** \brief PLX reset
   */
  void setReloadFirmware ( ) ;

  /** \brief FEC reset
   */
  void setFecReset ( ) ;

  /** \brief Reset or initialisae FECs TTCRx
   */
  void initTTCRx ( ) ;

  /** \brief Set the SSID in the CCS Trigger
   */
  bool setCCSTriggerSSID ( ) ;

  /** \brief reset modules via the PIA channels
   */
  void setPiaReset ( ) ;

  /** \brief cold reset for PLL
   */
  void setColdPllReset ( ) ;

  // ------------------------------------------------------- I2O message

  /** \brief receive all i2o messages and dispatch it
   */
  toolbox::mem::Reference* senderTrackerCommandHandler(PI2O_TRACKER_COMMAND_MESSAGE_FRAME com) ;

  /** \brief send FEC temperature to the DcuFilter
   */
  void sendI2OFecTemperature ( xdaq::ApplicationDescriptor *dcuFilterDes, keyType indexFecTemp, int tempSensor1, int tempSensor2 ) ;

  /** \brief send DCU values to the DcuFilter
   */
  void sendI2ODcuValues( xdaq::ApplicationDescriptor *dcuFilterDes, deviceVector vDevice ) ;

  // ------------------------------------------------------- actionPerformed

  /** \brief Answer to a SOAP request
   */
  xoap::MessageReference soapStatusParameters (xoap::MessageReference msg) throw (xoap::exception::Exception) ;

  /** \brief refresh all parameters to be exported to XMAS
   */
  void refreshParameters ( ) ;

  /** \brief When an item is fired
   */
  void actionPerformed(xdata::Event& event) ;

  // ------------------------------------------------------- Web methods

  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) ;

  /** \brief Web page for the initialise
   */
  void displayInitialisePage(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Initialisation parameters web page
   */
  void supervisorInitialise(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief finite state machine web page 
   */
  void displayStateMachine (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief finite state machine action
   */
  void dispatch (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief related links web page
   */
  void displayRelatedLinks (xgi::Input * in, xgi::Output * out, bool withHTMLTag = false) throw (xgi::exception::Exception) ;

  /** \brief display more parameters
   */
  void displayMoreParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;
  
  /** \brief action on the parameters following displayMoreParameters
   */
  void moreParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief hardware check
   */
  void displayHardwareCheck (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display all FEC ring
   */
  void displayFecRingDisplay (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief action corresponding to the method displayFecRingDisplay
   */
  void fecRingChoice (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

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

  /** \brief display all the error during a detection of the devices
   */
  void displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList, Sgi::hash_map<keyType, bool> *deviceError ) ;

  /** \brief display all the CCU / modules
   */
  void displayCcuModules(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display all the CCU registers
   */
  void displayCcuRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display all the modules with the devices connected to
   */
  void displayModuleDevices (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

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

  /** \brief display the erreur
   */
  void displayFecRingField (xgi::Input * in, xgi::Output * out, unsigned int fecSlot) throw (xgi::exception::Exception) ;

  /** \brief display a list in a field with some information
   */
  void displayListModulesInList ( xgi::Output * out, 
				  std::string legendFieldName, std::string nameAnchor,
				  std::string foundModule,
				  std::list<keyType> modulesToDisplay,
				  Sgi::hash_map<keyType, bool> deviceError,
				  Sgi::hash_map<keyType, bool> deviceMissing ) ;

  /** \brief display some values from a module
   */
  void displayModuleParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display some values from a module
   */
  void displayModuleParameters (xgi::Input * in, xgi::Output * out, unsigned int dcuHardId, keyType index) throw (xgi::exception::Exception) ;

  /** \brief failure page
   */
  void displayFailurePage(xgi::Input * in, xgi::Output * out) ;

  /** FSM Soap message
   */
  xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception) ;

  // --------------------------------- Diagnostic system call back

  /** \brief Method called when a recovery is done by the 1st level
   */
  xoap::MessageReference fecSupervisorRecovery (xoap::MessageReference msg) 
    throw (xoap::exception::Exception)  ;

  /** \brief Web page called for the diagnostic system
   */
  void displayConfigureDiagSystem ( xgi::Input * in, xgi::Output * out ) ;

  /** \brief display the form of the diagnostic web pages
   */
  void getDiagSystemContent(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) ;

#ifdef TKDIAG
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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  /** \brief callback XOAP for remote control pannel of LogReader application (global)
   */
  DIAG_REQUEST_ENTRYPOINT();

#endif

  /** \brief this method analyse a ring problem
   */
  void i2cRecover ( FecExceptionHandler e ) ;

  /** \brief this method makes a ring redundancy check. 
   */
  void setRedundancy (bool reset=true) ;
  /** \brief check if there are rings needing redundancy
   */
  bool isRedundancyNeeded ();
  /** This method bypass one CCU from the ring depending of the configuration
   */
  void bypass ( FecAccess *fecAccess, unsigned int ring, unsigned short ccu1, unsigned short ccu3, unsigned short dummy = 0x1 ) ;

  /** Empty the FIFOs for a given ring
   */
  void emptyFIFOs ( FecAccess *fecAccess, keyType index, bool display ) ;

  /** show informationa about a given ring 
   */
  void ringdiag (int slot);

  // ------------------------------------------------------- Error method

  /** \brief manage all the errors from a given list
   * This method take list of FecEceptionHandler and depending of which kind of error report it have, send it to the remote diagnostic system
   * Please note that this error messages are sent to the first level of diagnostic systems
   * \param message - message that is displayed before the error
   * \param errorList - list of errors
   */
  static unsigned int manageErrorList ( ErrorReportLogger &errorReportLogger, std::string message, std::list<FecExceptionHandler *> &errorList ) {

    if (errorList.size() == 0) return 0 ;

    Sgi::hash_map<keyType, bool> errorDevice ; // in order to avoid multiple display of errors
    Sgi::hash_map<unsigned int, FecExceptionHandler *> deleteException ; // to delete the exception
    unsigned long error = 0 ;

    // simple message
    // std::cout << message << ": found " << errorList.size() << " errors" << std::endl ;

    for (std::list<FecExceptionHandler *>::iterator p = errorList.begin() ; p != errorList.end() ; p ++) {

      FecExceptionHandler *e = *p ;
      unsigned long ptr = (unsigned long)e ;

      if (deleteException.find(ptr) == deleteException.end()) deleteException[ptr] = e ;

      if (e->getPositionGiven()) {

	if (!errorDevice[e->getHardPosition()]) {

	  errorDevice[e->getHardPosition()] = true ;
	  // Report the error
	  errorReportLogger.errorReport (message, *e, LOGERROR) ;
	  error ++ ;
	}
      }
      else {

	// Report the error
	errorReportLogger.errorReport (message, *e, LOGERROR) ;
	error ++ ;
      }
    }

    // Delete the exception (not redundant)
    for (Sgi::hash_map<unsigned int, FecExceptionHandler *>::iterator p = deleteException.begin() ; p != deleteException.end() ; p ++) {
      delete p->second ;
    }
    
    // No more error
    errorList.clear() ;
    
    return error ;
  }
} ;


#endif
