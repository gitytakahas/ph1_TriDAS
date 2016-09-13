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

// display all debug messages
//#define DEBUGMSGERROR
// display the timing measurement for configuration / initialisation
#define MEASURETIMING
// Wait in hardware configuration usleep(100000*instance number) to delay the FEC hardware acces
//#define WAITFECHARDWAREACCESS 100000

/**
 * - semaphore dans pia + FecDevice::writeFrame
 * - PiaResetAccess semaphore sans xdaq !
 * - faire un check de l'error severity pour interrompre tout en cas de gros probleme
 */

#include <pthread.h>
#include <stdlib.h>

// STD includes
#include <iostream>
#include <stdexcept>
#include <cctype>    // std::toupper

#include <string.h>  // strncpy
#include <time.h>    // sleep

// XDAQ includes
#include "toolbox/fsm/FailedEvent.h"
#include "toolbox/TimeInterval.h"
#include "xdaq/ApplicationRegistry.h"


// Specific includes
#include "stringConv.h"         // For template conversion to/from string

// FecSupervisor includes
#include "DeviceFactoryInterface.h"

#include "datatypes.h"
#include "tscTypes.h"

#include "keyType.h"
#include "HashTable.h"

#include "MemBufOutputSource.h"
#include "FecExceptionHandler.h"

// FEC bus headers
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
#include "FecVmeRingDevice.h"
#include "CCSTrigger.hh"        // for CCS trigger part
#endif
#if defined(BUSPCIFEC)
#include "FecPciRingDevice.h" 
#endif
#if defined(BUSUSBFEC)
#include "FecUsbRingDevice.h"
#endif

#define NONEVALUE 0xFFFFFFFF

// FEC includes
#include "xdaqTrackerFecDefs.h"
#include "FecSupervisor.h"

// This define is just there in order to set the class
#define CLASSNAME    FecSupervisor
#define CLASSNAMESTR std::string("FecSupervisor")
#define __FecSupervisorClass__
#include "FecDetectionUpload.h"
#include "DisplayCCUModules.h"
  
/** MACRO for XDAQ
 */
XDAQ_INSTANTIATOR_IMPL( FecSupervisor );

/** Default parameters
 */
#define DEFINENONE "NONE"
#define SHAREMEMORYNAME DEFINENONE
// this name is used if the work loop name is not set in the fire items
#define WORKLOOPNAME "WaitingWorkLoop" 

/** Just a macro to define NONE SSID so no SSID at all
 */
#define SSID_NONE DEFINENONE

/** Check if the plug and play is used
 */
#define NOPLUGNPLAYMESSAGE "No plug and play used"
// I2C Speed
#define I2CSPEED 100

/** This constructor is used in order to initialise the different exported parameters parameters. 
 * This parameters must be set depending of which state your are using:
 * <p>Initialise state:</i>
 * <ul>
 * <li> <i>fireItemAvailable(std::string("FecBusType"),&fecBusType_)</i>: define which kind of FEC you are using. 
 * The possible values are (defined in xdaqTrackerFecDefs.h):
 *    <ol>
 *    <li> fecBusTypeList[FECPCI] 
 *    <li> fecBusTypeList[FECVME] 
 *    <li> fecBusTypeList[FECUSB] 
 *    </ol>
 * <li> <i>fireItemAvailable(std::string("StrBusAdapter"),&strBusAdapter_)</i>: define which kind of PCI to VME link used:
 * The possible values are (defined in xdaqTrackerFecDefs.h):
 *    <ol>
 *    <li> strBusAdapterList[SBS]
 *    <li> strBusAdapterList[CAENPCI]
 *    <li> strBusAdapterList[CAENUSB]
 *    </ol>
 * <li> <i>fireItemAvailable(std::string("VmeFileName"),&vmeFileName_)</i>: define where the hal file VME configuration file is defined (automatic check is done on environement variable)
 * <li> <i>fireItemAvailable(std::string("VmeFileNamePnP"),&vmeFileNamePnP_)</i>: define where the hal file plug and play file is defined (automatic check is done on environement variable).
 * <li> for database purpose, you can define the login, password and path to the datatbase:
 *    <ol>
 *    <li> <i>fireItemAvailable(std::string("DbLogin"),&dbLogin_)</i>
 *    <li> <i>fireItemAvailable(std::string("DbPassword"),&dbPasswd_)</i>
 *    <li> <i>fireItemAvailable(std::string("DbPath"),&dbPath_)</i>
 *    <li> <i>fireItemAvailable(std::string("PartitionName"),&partitionName_)</i>: Partition name
 *    </ol>
 * </ul>
 * <p> Configure state:
 * <ul>
 * <li> <i>fireItemAvailable(std::string("FecBusType"),&fecBusType_)</i>
 * <li> <i>fireItemAvailable(std::string("VmeFileName"),&vmeFileName_)</i>
 * <li> <i>fireItemAvailable(std::string("VmeFileNamePnP"),&vmeFileNamePnP_)</i>
 * <li> <i>fireItemAvailable(std::string("StrBusAdapter"),&strBusAdapter_)</i>
 * <li> <i>fireItemAvailable(std::string("FECSlot"),&fecSlot_)</i>: FEC hardware ID (the FEC that has to be taken care by FecSupervisor)
 * <li> <i>fireItemAvailable(std::string("SSID"),&SSID_)</i>: SSID for VME FEC
 * <li> <i>fireItemAvailable(std::string("ShareMemoryName"),&shareMemoryName_)</i>: Shared memory name for the database caching system
 * <li> <i>fireItemAvailable(std::string("DcuFilterClassName"),&dcuFilterClassName_)</i>: DCU filter class name (DcuFilter by default)
 * <li> <i>fireItemAvailable(std::string("DcuFilterInstance"),&dcuFilterInstance_)</i>: instance of the DCU filter (0 by default)
 * <li> <i>fireItemAvailable(std::string("DcuWorkLoopName"),&workLoopName_)</i>: DCU / Device work loop name
 * </ul>
 * <p> Configure state:
 * <ul>
 * <li> <i>fireItemAvailable(std::string("MaxErrorNumberAllowed"),&maxErrorAllowed_)</i>: maximum number of errors before stopping the download
 * <li> <i>fireItemAvailable(std::string("XMLFileNameInput"),&xmlFileName_)</i>: XML file source for the FEC devices and PIA reset
 * <li> <i>fireItemAvailable(std::string("ReloadFirmware"),&reloadFirmware_)</i>: Crate or PLX reset depending of which you are using
 * <li> <i>fireItemAvailable(std::string("CrateReset"),&crateReset_)</i>: FEC reset
 * <li> <i>fireItemAvailable(std::string("FECReset"),&resetFec_)</i>: FEC reset
 * <li> <i>fireItemAvailable(std::string("ApplyRedundancy"),&applyRedundancy_)</i>: Apply the redundancy. At the initialisation state the FEC are checked and redundancy is applied. Then before the configure, the check is done on the ring who was reconfigured during initialisation. If the parameter is set again thanks to the parameter a complete check is done again for all rings. 
 * <li> <i>fireItemAvailable(std::string("ForceApplyRedundancy"),&forceApplyRedundancy_)</i>: force the redundancy appliement even if the ring is closed. see previous tag
 * <li> <i>fireItemAvailable(std::string("PIAReset"),&resetPia_)</i>: PIA reset
 * <li> <i>fireItemAvailable(std::string("DatabaseDownload"),&databaseDownload_)</i>: use the database for download and upload operation
 * <li> <i>fireItemAvailable(std::string("DCU_Thread_Used"),&dcuThreadUsed_)</i>: Use the work loop
 * <li> <i>fireItemAvailable(std::string("Device_Thread_Used"),&deviceThreadUsed_)</i>: Use the work loop
 * <li> <i>fireItemAvailable(std::string("ColdPllInit"),&coldPllReset_)</i>: PLL cold reset/init
 * </ul>
 */
FecSupervisor::FecSupervisor ( xdaq::ApplicationStub* stub ) : 
  TrackerCommandSender(stub), 
  JsInterface(stub), 
  fsm_(this),
  // --------------------------------------
  // display error messages
  displayDebugMessage_(false),
  displayFrameErrorMessages_(false),
  displayDcuDebugMessage_(false),
  displayDeviceDebugMessage_(false),
  displayErrorOnConsole_(false),
  // --------------------------------------
  // Initialise the objects for the accesses
  fecAccess_(NULL),
  fecAccessManager_(NULL),
  // --------------------------------------
  // Hardware parameters

  multiFrames_(true),                               // multiframes
  
  blockMode_(false),                                // block mode
  fedid_(-1),                                       // fedid that went to rsed
  crateReset_(false),                               // crate reset
  reloadFirmware_(false),                           // reload of firmware
  initTTCRx_(false),                                // Initialisation of the CCS TTCRx
  SSID_(SSID_NONE),                                 // SSID
  resetFec_(true),                                  // FEC reset
  initPia_(true),                                   // PIA reset at cold start
  resetPia_(false),                                 // PIA reset during configuration
  i2cSpeed_(100),                                   // i2c speed default value
  configureDOH_(false),                             // Configure DOH
  coldPllReset_(false),                             // Cold PLL reset
  fecHardwareId_("0"),                              // No FEC hardware id
  fecSlot_(1),                                      // no FEC slot (set by RCMS configuration)
  // --------------------------------------
  // redundancy
  forceApplyRedundancy_(true),                      // do not force the redundancy
  applyRedundancy_(true),                           // if a ring is failing then apply the redundancy except if the forceApplyRedundancy is set then try to apply the redundancy
  // --------------------------------------
  // Default firmware version
  fecFirmwareVersionExpected_(17),
  vmeFirmwareVersionExpected_(16),
  triggerFirmwareVersionExpected_(14),
  // --------------------------------------
  // Shared memory
#if defined(TRACKERXDAQ)
  shareMemoryName_(SHAREMEMORYNAME),
  dbClient_(NULL),
#endif
  // --------------------------------------
  // Database access
  fecFactory_(NULL),
  dbLogin_("nil"),
  dbPasswd_("nil"), 
  dbPath_("nil"),
  databaseAccess_(false),
  databaseDownload_(false),
  partitionName_(DEFINENONE),
  partitionId_(0),
  versionMajorId_(0),
  versionMinorId_(0),
  maskMajor_(0),
  maskMinor_(0),
  initDbVersion_(false),
  versionPiaMajor_(0),
  versionPiaMinor_(0),
  // --------------------------------------
  // General parameter
  doDownload_(true),                                // Download
  doUpload_(false),                                 // Upload (false by default but done during a download)
  doUploadOrigin_(true),                            // Upload first time
  doComparison_(true),                              // Comparison between hardware and database
  forceDownload_(false),                            // Force the full download of all devices
  dbClientChanged_(false),                          // If the database parameters (version) changed then the full download should be performed
  setUploadDone_(false),                            // upload of parameters done
  xmlFileName_(UNKNOWNFILE),                        // XML file name
  doUploadInFile_(false),                           // Upload in file
  xmlFileNameOutput_("/tmp/upload.xml"),
  xmlFileNameOutputDcu_("/tmp/DcuOutput"),
  maxErrorAllowed_(0),                              // Maximum number of errors allowed
  // --------------------------------------
  // DCU & device work loop
  dcuWorkLoopTime_(WORKLOOPTIME),                   // time between two read for the DCU readout
  deviceWorkLoopTime_(WORKLOOPTIME * 10),           // time between two read for the device readout (10 times more)
  counterDcuTime_(0),
  counterDeviceTime_(0),
  dcuThreadUsed_(false),                            // not used by default
  deviceThreadUsed_(false),                         // not used by default
  workFecSupervisorLoop_(NULL),
  suspendDcuWorkLoop_(false),
  sleepDcuWorkLoop_(true),
  suspendDeviceWorkLoop_(false),
  sleepDeviceWorkLoop_(true),
  firstTimeDcuWorkLoop_(true),                      // display
  firstTimeDeviceWorkLoop_(true),                   // display
  dcuFilterDes_(NULL),                              // DCU filter destination
  dcuFilterInstance_(0),                            // instance of the destination
  dcuFilterClassName_(DCUFILTERCLASSNAME),          // class name of the destination
  // --------------------------------------------
  // Status of the rings and trigger
  statusFECRing8_(NONEVALUE),
  statusFECRing7_(NONEVALUE),
  statusFECRing6_(NONEVALUE),
  statusFECRing5_(NONEVALUE),
  statusFECRing4_(NONEVALUE),
  statusFECRing3_(NONEVALUE),
  statusFECRing2_(NONEVALUE),
  statusFECRing1_(NONEVALUE),
  statusFECPCI_(NONEVALUE),
  statusFECUSB_(NONEVALUE),
  statusQPLLError_(NONEVALUE),
  statusQPLLLocked_(NONEVALUE),
  statusTTCRx_(NONEVALUE),
  statusSSID_(SSID_NONE),
  ringFirmwareVersion_(NONEVALUE),
  vmeFirmwareVersion_(NONEVALUE),
  triggerFirmwareVersion_(NONEVALUE),
  fecTemperature1_(NONEVALUE),
  fecTemperature2_(NONEVALUE),
  // --------------------------------------------
  // Web link
  linkPosition_(0)
{
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
				   DIAG_TRACKERFEC_SUBSYSTEMID);

  // Declare the application as a user application
  DIAG_DECLARE_USER_APP ;
  errorReportLogger_ = new ErrorReportLogger ("SiTkFecSupervisor " + getApplicationDescriptor()->getInstance(), displayErrorOnConsole_, true, LOGDEBUG, diagService_) ;

  // Configuration option for the diag => link on DIAG_CONFIGURE_CALLBACK
  xgi::bind(this, &FecSupervisor::configureDiagSystem, "configureDiagSystem");
  //                                   => link on DIAG_APLLY_CALLBACK
  xgi::bind(this, &FecSupervisor::applyConfigureDiagSystem, "applyConfigureDiagSystem");

  // For XRelay
  xoap::bind(this, &FecSupervisor::freeLclDiagSemaphore, "freeLclDiagSemaphore", XDAQ_NS_URI );
  xoap::bind(this, &FecSupervisor::freeGlbDiagSemaphore, "freeGlbDiagSemaphore", XDAQ_NS_URI );

  //For using the centralized command pannel in logreader
  //                                   => link on DIAG_REQUEST_ENTRYPOINT
  xoap::bind(this, &FecSupervisor::processOnlineDiagRequest, "processOnlineDiagRequest", XDAQ_NS_URI );
#else
  errorReportLogger_ = new ErrorReportLogger ("SiTkFecSupervisor " + getApplicationDescriptor()->getInstance(), displayErrorOnConsole_, true, LOGDEBUG) ;
#endif

  // ------------------------------------------------------------------
  // Keep the user inform about the version of FecSupervisor
  std::cout << "----------------------------------> New FecSupervisor with asynchronous fsm 3.0" << std::endl ;
  errorReportLogger_->errorReport("New FecSupervisor with asynchronous fsm 3.0", LOGUSERINFO) ;

  // -----------------------------------------------------------------------------------
  // Software tag version from CVS
  softwareTagVersion_ = FecExceptionHandler::getCVSTag() ;

  // Software status
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Constructor" ;

  // -----------------------------------------------------------------
  // Work loop for DCU and Device
  workLoopName_ = WORKLOOPNAME + toString(getApplicationDescriptor()->getInstance()/2);
  dcuReadoutWorkLoop_ = toolbox::task::bind (this, &FecSupervisor::dcuReadoutWorkLoop, "dcuReadoutWorkLoop");
  deviceReadoutWorkLoop_ = toolbox::task::bind (this, &FecSupervisor::deviceReadoutWorkLoop, "deviceReadoutWorkLoop");
  mutexThread_ = new toolbox::BSem(toolbox::BSem::FULL) ;
  mutexTaker_ = 0;
  dcuStuck_= false;
  // ------------------------------------------------------------------
  // redundancy
  for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = (applyRedundancy_ == true) ; // apply if the parameter is set

  // ------------------------------------------------------------------
  // Database access
  std::string dbLogin = "nil", dbPasswd = "nil", dbPath = "nil" ;
#ifdef DATABASE
  DbFecAccess::getDbConfiguration (dbLogin, dbPasswd, dbPath) ;
  dbLogin_ = dbLogin ; dbPasswd_ = dbPasswd ; dbPath_ = dbPath ;

  if (dbLogin_ != "nil" && dbPasswd != "nil" && dbPath_ != "nil") databaseAccess_ = true ;
#endif

  // ------------------------------------------------------------------
  // FEC on VME over a CAEN PCI
  fecBusType_ = fecBusTypeList[FECVME] ;
  strBusAdapter_ = strBusAdapterList[CAENPCI] ;

  // Configuration file for the VME
  std::string vmeFileName, vmeFileNamePnP ;
  bool plugandplayUsed = FecAccess::getVmeFileName(vmeFileName, vmeFileNamePnP ) ;
  if ( (vmeFileName.size() != 0) || (vmeFileNamePnP.size() != 0) ) {
    if (plugandplayUsed) {
      std::stringstream msgInfo ; msgInfo << "Plug and play used: plug and play file " << vmeFileNamePnP << " and FEC Id file " << vmeFileName ;
      
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }
    else {
      std::stringstream msgInfo ; msgInfo << "Geographical address used: VME file name: " << vmeFileName ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }

    vmeFileName_ = vmeFileName ; vmeFileNamePnP_ = vmeFileNamePnP ;
  }
  else {
    std::stringstream msgError ; msgError << "Did not find the VME file name configuration" ;
    errorReportLogger_->errorReport (msgError.str(), LOGERROR, 0, XDAQFEC_VMEFILEMISSING) ;
  }

  // ---------------------------------------------------------------------------------------
  // SOAP method to call the refresh of the parameters and return a soap with all informations
  xoap::bind (this, &FecSupervisor::soapStatusParameters, "Status", XDAQ_NS_URI);

  // ---------------------------------------------------------------------------------------
  // Define the final state machine
  // setup finite state machine (binding relevant callbacks)
  fsm_.initialize<FecSupervisor>(this);

  // -----------------------------------------------------------------------------------------
  // Bind SOAP callbacks for control messages: not anymore needed ?
  xoap::bind (this, &FecSupervisor::fireEvent, INITIALISE, XDAQ_NS_URI);
  xoap::bind (this, &FecSupervisor::fireEvent, DESTROY, XDAQ_NS_URI);
  xoap::bind (this, &FecSupervisor::fireEvent, CONFIGURE, XDAQ_NS_URI);
  xoap::bind (this, &FecSupervisor::fireEvent, CONFIGUREDCU, XDAQ_NS_URI);
  xoap::bind (this, &FecSupervisor::fireEvent, HALTDCU, XDAQ_NS_URI);
  xoap::bind (this, &FecSupervisor::fireEvent, HALT, XDAQ_NS_URI);

  // Bind CGI callbacks
  xgi::bind(this, &FecSupervisor::supervisorInitialise, "supervisorInitialise");
  //xgi::bind(this, &FecSupervisor::supervisorHalt, "supervisorHalt");
  //xgi::bind(this, &FecSupervisor::displayParameter, "Parameter");
  xgi::bind(this, &FecSupervisor::dispatch, "dispatch");
  xgi::bind(this, &FecSupervisor::displayStateMachine, "StateMachine");

  // --------------------------------------------------
  // Method called by the first level of diagnostic
  xoap::bind(this, &FecSupervisor::fecSupervisorRecovery, "fecSupervisorRecovery", XDAQ_NS_URI );

  // -------------------------------------------------
  // Export the different parameters
  exportParameters ( ) ;

  // -------------------------------------------------
  // Defined all bind for web pages
  webGUI ( ) ;
}

/** Destroy each access store in the different hash tables for all the device
 * that was initialised. Remove also the access to the database and FecAccess
 * that will close all the device drivers.
 */
FecSupervisor::~FecSupervisor ( ) {
  
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destruction of the application" ;

  // Delete all accesses
  deleteAccesses ( ) ;

  // Delete the mutex between the DCU work loop and the FecSupervisor
  delete mutexThread_ ;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                 Parameters from constructor                                                  */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Fire item on all the parameters needed by this application
 */
void FecSupervisor::exportParameters ( ) {

#define JSINTERFACE
#ifdef JSINTERFACE
  // ---------------------------------------------------------------------------------------
  // DCU work loop name
  declareParameter(this,std::string("DcuWorkLoopName"),&workLoopName_,"DCU work loop name","FecInitialConfiguration",false) ;

  // ---------------------------------------------------------------------------------------
  // Download from database or from db client
  declareParameter(this,std::string("DbLogin"),&dbLogin_, "Database login only if the database cache is not set (read only)","FecInitialConfiguration",false);  
  declareParameter(this,std::string("DbPassword"),&dbPasswd_, "Database password only if the database cache is not set (read only)","FecInitialConfiguration",false);    
  declareParameter(this,std::string("DbPath"),&dbPath_,"Database path only if the database cache is not set (read only)","FecInitialConfiguration",false);    
  declareParameter(this,std::string("PartitionName"),&partitionName_,"Partition used for download (if database is used) (read only)","FecInitialConfiguration",false);
#if defined(TRACKERXDAQ)
  declareParameter(this,std::string("ShareMemoryName"),&shareMemoryName_,"Shared memory name for database cache client (read only)","FecInitialConfiguration",false) ;
#endif

  // ---------------------------------------------------------------------------------------
  // Hardware access
  declareParameter(this,std::string("FecBusType"),&fecBusType_,"FEC bus (PCI, VME, USB) (read only)", "FecInitialConfiguration",false) ;
  declareParameter(this,std::string("VmeFileName"),&vmeFileName_,"VME configuration file if FEC bus is VME) (read only)", "FecInitialConfiguration",false) ;
  declareParameter(this,std::string("VmeFileNamePnP"),&vmeFileNamePnP_,"VME plug and play configuration file if FEC bus is VME) (read only)", "FecInitialConfiguration",false) ;
  declareParameter(this,std::string("StrBusAdapter"),&strBusAdapter_,"Bus adapter, CAEN, SBS if FEC bus is VME (read only)", "FecInitialConfiguration",false) ;
  declareParameter(this,std::string("FECSlot"),&fecSlot_,"FEC slot for this FecSupervisor (read only)","FecInitialConfiguration");
  declareParameter(this,std::string("SSID"),&SSID_,"FEC SSID so TRACKER (read only)", "FecInitialConfiguration",false) ;

  // ---------------------------------------------------------------------------------------
  // All possible configuration, database, file or database cache
  declareParameter(this,std::string("XMLFileNameInput"),&xmlFileName_,"FEC device configuration XML file","FecDownloadConfiguration");
  declareParameter(this,std::string("DatabaseDownload"),&databaseDownload_,"Download from database","FecDownloadConfiguration");

  // ---------------------------------------------------------------------------------------
  // Maximum number of error in hardware
  declareParameter(this,std::string("MaxErrorNumberAllowed"),&maxErrorAllowed_,"Max number of errors handled by the FEC access manager","FecDonwloadConfiguration");  

  // ---------------------------------------------------------------------------------------
  // Hardware configuration
  declareParameter(this,std::string("ReloadFirmware"),&reloadFirmware_,"Reload the FEC firmware at configure time","FecHardwareConfiguration");
  declareParameter(this,std::string("CrateReset"),&crateReset_,"Reset the VME crate at configure time","FecHardwareConfiguration");
  declareParameter(this,std::string("FECReset"),&resetFec_,"Reset the FEC at configure time","FecHardwareConfiguration");
  declareParameter(this,std::string("ApplyRedundancy"),&applyRedundancy_,"Apply redundancy to problematic ring","FecHardwareConfiguration");
  declareParameter(this,std::string("ForceApplyRedundancy"),&forceApplyRedundancy_,"Force the check of redundancy","FecHardwareConfiguration");
  declareParameter(this,std::string("PIAReset"),&resetPia_,"PIA reset at configure time","FecHardwareConfiguration");
  declareParameter(this,std::string("ColdPllInit"),&coldPllReset_,"Apply a cold PLL reset","FecHardwareConfiguration") ;
  declareParameter(this,std::string("VmeBlockMode"),&blockMode_,"Use Vme block mode for reading from the receive FIFO", "FecHardwareConfiguration") ;
  declareParameter(this,std::string("Fedid"),&fedid_,"The Fed ID for the RSED", "FecHardwareConfiguration") ;

  // ---------------------------------------------------------------------------------------
  // Thread configuration
  declareParameter(this,std::string("DCU_Thread_Used"),&dcuThreadUsed_,"Start the DCU thread","FecThreadConfiguration") ;
  declareParameter(this,std::string("Device_Thread_Used"),&deviceThreadUsed_,"Start the device thread, use for (re)configuration","FecThreadConfiguration") ;
  declareParameter(this,std::string("DCU_Thread_LoopTime"),&dcuWorkLoopTime_,"Time for the DCU work loop","FecThreadConfiguration") ;
  declareParameter(this,std::string("Device_Thread_LoopTime"),&deviceWorkLoopTime_,"Time for the device work loop (multiple of X times dcuWorkLoopTime_","FecThreadConfiguration") ;
  declareParameter(this,std::string("ForceDownload"),&forceDownload_,"Force the download of all devices even if the intelligent configuration is set (based on upload)","FecThreadConfiguration");

  // ---------------------------------------------------------------------------------------
  // DcuFilter application access
  declareParameter(this,std::string("DcuFilterClassName"),&dcuFilterClassName_,"DCU filter class name to send DCU data to DcuFilter","FecDcuFilterConfiguration") ;
  declareParameter(this,std::string("DcuFilterInstance"),&dcuFilterInstance_,"DCU filter instance to send DCU data to DcuFilter","FecDcuFilterConfiguration") ;
  
  // ---------------------------------------------------------------------------------------
  // Old parameter
  //   declareParameter(this,std::string("Upload_operation"),&doUpload_,"Upload the devices","FecDownloadConfiguration") ;
  //   declareParameter(this,std::string("Download_operation"),&doDownload_,"Do the download","FecDownloadConfiguration") ;
  //   declareParameter(this,std::string("Run_Number"),&runNumber_,"Run number","FecDownloadConfiguration") ;
  //   getApplicationInfoSpace()->addItemRetrieveListener ("Download_Operation", this);

  // ---------------------------------------------------------------------------------------
  // Status of the rings and trigger
  declareParameter(this,std::string("StatusFECPCI"),&statusFECPCI_,"FEC PCI status register 0 (0xFFFFFFFF: not read or error)","FecPCIStatus") ;
  declareParameter(this,std::string("StatusFECUSB"),&statusFECUSB_,"FEC USB status register 0 (0xFFFFFFFF: not read or error)","FecUSBStatus") ;
  declareParameter(this,std::string("StatusFECRing8"),&statusFECRing8_,"FEC ring 8 VME status register 0 (0xFFFFFFFF: not read or error)","FecVMEStatus") ; 
  declareParameter(this,std::string("StatusFECRing7"),&statusFECRing7_,"FEC ring 7 VME status register 0 (0xFFFFFFFF: not read or error)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusFECRing6"),&statusFECRing6_,"FEC ring 6 VME status register 0 (0xFFFFFFFF: not read or error)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusFECRing5"),&statusFECRing5_,"FEC ring 5 VME status register 0 (0xFFFFFFFF: not read or error)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusFECRing4"),&statusFECRing4_,"FEC ring 4 VME status register 0 (0xFFFFFFFF: not read or error)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusFECRing3"),&statusFECRing3_,"FEC ring 3 VME status register 0 (0xFFFFFFFF: not read or error)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusFECRing2"),&statusFECRing2_,"FEC ring 2 VME status register 0 (0xFFFFFFFF: not read or error)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusFECRing1"),&statusFECRing1_,"FEC ring 1 VME status register 0 (0xFFFFFFFF: not read or error)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusQPLLError"),&statusQPLLError_,"FEC VME QPLL error status register (0xFFFFFFFF: not read, 1: error, 0 not error)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusQPLLLocked"),&statusQPLLLocked_,"FEC VME QPLL locked (0xFFFFFFFF: not read, 1: locked, 0 not locked)","FecVMEStatus") ;
  declareParameter(this,std::string("StatusTTCRx"),&statusTTCRx_,"FEC VME TTCRx status (0xFFFFFFFF: not read, 1: ready, 0 not ready)","FecVMEStatus") ; 
  declareParameter(this,std::string("StatusSSID"),&statusSSID_,"FEC VME SSID (NONE: not read)","FecVMEStatus") ;
  declareParameter(this,std::string("RingFirmwareVersion"),&ringFirmwareVersion_,"FEC VME ring firmware version (0xFFFFFFFF: not read)","FecVMEStatus") ;
  declareParameter(this,std::string("VMEFirmwareVersion"),&vmeFirmwareVersion_,"FEC VME firmware version (0xFFFFFFFF: not read)","FecVMEStatus") ;
  declareParameter(this,std::string("TriggerFirmwareVersion"),&triggerFirmwareVersion_,"FEC VME trigger firmware version (0xFFFFFFFF: not read)","FecVMEStatus") ;
  declareParameter(this,std::string("FECTemperature1"),&fecTemperature1_,"FEC VME temperature sensor 1 (0xFFFFFFFF: not read)","FecVmeStatus") ;
  declareParameter(this,std::string("FECTemperature2"),&fecTemperature2_,"FEC VME temperature sensor 2 (0xFFFFFFFF: not read)","FecVMEStatus") ;

  // Export the stateName variable => cannot be exported since the fsm is in the Asynchronous State Machine
  // declareParameter(this,"stateName", &fsm_.stateName_,"Finite state machine state","FecConfiguration");

#else

  // ---------------------------------------------------------------------------------------
  // Maximum number of error in hardware
  getApplicationInfoSpace()->fireItemAvailable(std::string("MaxErrorNumberAllowed"),&maxErrorAllowed_);  

  // ---------------------------------------------------------------------------------------
  // Database
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbLogin"),&dbLogin_);  
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbPassword"),&dbPasswd_);  
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbPath"),&dbPath_);  

  // ---------------------------------------------------------------------------------------
  // Hardware access
  getApplicationInfoSpace()->fireItemAvailable(std::string("FecBusType"),&fecBusType_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("VmeFileName"),&vmeFileName_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("VmeFileNamePnP"),&vmeFileNamePnP_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("StrBusAdapter"),&strBusAdapter_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("VmeBlockMode"),&blockMode_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("Fedid"),&fedid_) ;

  // ---------------------------------------------------------------------------------------
  // All possible configuration, database, file or database cache
  getApplicationInfoSpace()->fireItemAvailable(std::string("XMLFileNameInput"),&xmlFileName_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("ReloadFirmware"),&reloadFirmware_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("CrateReset"),&crateReset_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("FECReset"),&resetFec_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("ApplyRedundancy"),&applyRedundancy_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("ForceApplyRedundancy"),&forceApplyRedundancy_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("PIAReset"),&resetPia_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("DatabaseDownload"),&databaseDownload_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("FECSlot"),&fecSlot_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("PartitionName"),&partitionName_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("SSID"),&SSID_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("DCU_Thread_Used"),&dcuThreadUsed_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("Device_Thread_Used"),&deviceThreadUsed_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("DCU_Thread_LoopTime"),&dcuWorkLoopTime_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("Device_Thread_LoopTime"),&deviceWorkLoopTime_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("ColdPllInit"),&coldPllReset_) ;
#if defined(TRACKERXDAQ)
  getApplicationInfoSpace()->fireItemAvailable(std::string("ShareMemoryName"),&shareMemoryName_) ;
#endif
  getApplicationInfoSpace()->fireItemAvailable(std::string("ForceDownload"),&forceDownload_);
  //   getApplicationInfoSpace()->fireItemAvailable(std::string("Upload_operation"),&doUpload_) ;
  //   getApplicationInfoSpace()->fireItemAvailable(std::string("Download_operation"),&doDownload_) ;
  //   getApplicationInfoSpace()->fireItemAvailable(std::string("Run_Number"),&runNumber_) ;
  //   getApplicationInfoSpace()->addItemRetrieveListener ("Download_Operation", this);

  // ---------------------------------------------------------------------------------------
  // DcuFilter application access
  getApplicationInfoSpace()->fireItemAvailable(std::string("DcuFilterClassName"),&dcuFilterClassName_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("DcuFilterInstance"),&dcuFilterInstance_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("DcuWorkLoopName"),&workLoopName_) ;
  
  // ---------------------------------------------------------------------------------------
  // Status of the rings and trigger
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECPCI"),&statusFECPCI_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECUSB"),&statusFECUSB_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECRing8"),&statusFECRing8_); // (0xFFFFFFFF: not read or error) 
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECRing7"),&statusFECRing7_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECRing6"),&statusFECRing6_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECRing5"),&statusFECRing5_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECRing4"),&statusFECRing4_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECRing3"),&statusFECRing3_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECRing2"),&statusFECRing2_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusFECRing1"),&statusFECRing1_); // (0xFFFFFFFF: not read or error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusQPLLError"),&statusQPLLError_); // (0xFFFFFFFF: not read, 1: error, 0 not error)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusQPLLLocked"),&statusQPLLLocked_); // (0xFFFFFFFF: not read, 1: locked, 0 not locked)
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusTTCRx"),&statusTTCRx_); // (0xFFFFFFFF: not read, 1: ready, 0 not ready) 
  getApplicationInfoSpace()->fireItemAvailable(std::string("StatusSSID"),&statusSSID_); // (NONE: not read)
  getApplicationInfoSpace()->fireItemAvailable(std::string("RingFirmwareVersion"),&ringFirmwareVersion_); // (0xFFFFFFFF: not read)
  getApplicationInfoSpace()->fireItemAvailable(std::string("VMEFirmwareVersion"),&vmeFirmwareVersion_); // (0xFFFFFFFF: not read)
  getApplicationInfoSpace()->fireItemAvailable(std::string("TriggerFirmwareVersion"),&triggerFirmwareVersion_); // (0xFFFFFFFF: not read)
  getApplicationInfoSpace()->fireItemAvailable(std::string("FECTemperature1"),&fecTemperature1_); // (0xFFFFFFFF: not read)
  getApplicationInfoSpace()->fireItemAvailable(std::string("FECTemperature2"),&fecTemperature2_); // (0xFFFFFFFF: not read)

  // Export the stateName variable => cannot be exported since the fsm is in the Asynchronous State Machine
  // getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);
#endif
}

/** Define all web methods for web pages
 */
void FecSupervisor::webGUI ( ) {

  // -----------------------------------------------
  // Nagivation related links
  std::string url = getApplicationDescriptor()->getURN();
  //relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/MoreParameters\" target=\"_blank\">More Parameters</a>", url.c_str()));
  //relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/MoreParameters\" target=\"_blank\">More Parameters</a>", url.c_str()));  
  //relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/HardwareCheck\" target=\"_blank\">Hardware Check</a>", url.c_str()));
  //relatedLinksNavigation_.push_back(toolbox::toString("<a href=http://x5oracle.cern.ch:8080/JSPWiki/Wiki.jsp?page=FECSoftware target=\"_blank\">Documentation</a>", url.c_str()));

  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/MoreParameters\">More Parameters</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/StateMachine\">State Machine</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/HardwareCheck\">Hardware Check</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/ConfigureDiagSystem\">Configure DiagSystem</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=https://twiki.cern.ch/twiki/bin/view/CMS/FECSoftware target=\"_blank\">Documentation</a>", url.c_str()));

  //xgi::bind(this, &FecSupervisor::displayRelatedLinks, "FecRelatedLinks");
  xgi::bind(this, &FecSupervisor::displayMoreParameters, "MoreParameters");
  xgi::bind(this, &FecSupervisor::displayHardwareCheck, "HardwareCheck");
  xgi::bind(this, &FecSupervisor::displayConfigureDiagSystem, "ConfigureDiagSystem");

  // -----------------------------------------------
  // Display more parameters
  xgi::bind(this, &FecSupervisor::moreParameters, "moreParameters");
  xgi::bind(this, &FecSupervisor::fecRingChoice, "fecRingChoice"); 
  xgi::bind(this, &FecSupervisor::displayFecRingDisplay, "FECRingDisplay"); 

  // -----------------------------------------------
  // Hardware check
  // Bind all methods for the display of the ring
  BINDCCUMODULESMETHODS ;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                 Finite state machine                                                         */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Create the differents access database, FecAccess, FecAccessManager
 * \warning this method delete first all the possible accesses
 */
//void FecSupervisor::initialiseAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {
bool FecSupervisor::initialising(toolbox::task::WorkLoop* wl) {

#ifdef MEASURETIMING
  unsigned long gStartMillis = XMLPlatformUtils::getCurrentMillis();
#endif

#ifdef WAITFECHARDWAREACCESS
  usleep((unsigned int)(WAITFECHARDWAREACCESS * getApplicationDescriptor()->getInstance())) ;
#endif

#ifdef TKDIAG
  // Apply the diagnostic fire items value to internal variables and check validity
  DIAG_EXEC_FSM_INIT_TRANS ;
#endif

  // Fix the partition name
  if ( (partitionName_ == "") || (partitionName_ == DEFINENONE) ) {
    char *basic=getenv ("ENV_CMS_TK_PARTITION") ;
    if (basic != NULL) partitionName_ = std::string(basic) ;
  }

  // Explicit message
  std::ostringstream strProcess ; strProcess << "SiTkFecSupervisor " << getApplicationDescriptor()->getInstance() << ", on slot " << fecSlot_.toString() ;
  errorReportLogger_->setStrProcess(strProcess.str()) ;

  // Delete all accesses
  deleteAccesses ( ) ;

#if defined(TRACKERXDAQ)
  // Access the shared memory
  if (shareMemoryName_ != DEFINENONE) {
    try {
      // Caching system access
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Initialisation: database caching system creation" ;
      dbClient_ = new DbClient(shareMemoryName_.toString()) ;
      databaseAccess_ = false ; // no need for database
    }
    catch (std::string &e) {
      std::stringstream msgError ; msgError << "Unable to create an access to the database client (DbClient: " << e << ") with the shared memory " << shareMemoryName_.toString() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      shareMemoryName_ = DEFINENONE ; dbClient_ = NULL ;
    }
  }
#endif

#ifdef DATABASE 
  if (databaseAccess_) {
#  ifdef TRACKERXDAQ
    if (dbClient_ == NULL)
#  endif
      {
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Initialisation: database access creation" ;
	createDatabaseAccess () ;
	getPartitionVersion () ;
      }
  }
#endif

  // FEC factory to retrieve the parameters
  if (fecFactory_ == NULL) {
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Initialisation: FEC factory creation" ;
    fecFactory_ = new FecFactory ( ) ;
    fecFactory_->setOutputFileName ( xmlFileNameOutput_ ) ;
  }

  // FEC access for the FEC hardware access
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Initialisation: FEC hardware creation" ;

  // Create FecAccess
  fecAccess_ = createFecAccess ( ) ;

  if (fecAccess_ != NULL) {

    // ---------------------------------------------------------------------------
    // Do firmware reload if asked
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Initialisation: reload the firmware" ;
    if (reloadFirmware_) { setReloadFirmware() ; reloadFirmware_ = false ; }

    // ---------------------------------------------------------------------------
    // SSID
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Initialisation: set the SSID" ;
    bool isFine = setCCSTriggerSSID ( ) ;

    // ---------------------------------------------------------------------------
    // FecHardwareId
    if ( (fecHardwareId_ != "0") && (fecHardwareId_ != "") ) { // Modification of message if VME FEC
      // Explicit message
      std::ostringstream strProcess ; strProcess << "SiTkFecSupervisor " << getApplicationDescriptor()->getInstance() << ", on slot " << fecSlot_.toString() << " (" << fecHardwareId_ << ")" ;
      errorReportLogger_->setStrProcess(strProcess.str()) ;
      errorReportLogger_->setFecHardwareId(fecHardwareId_) ;
    }

    // ---------------------------------------------------------------------------
    // FEC access manager for the devices
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Initialisation: Access manager creation" ;
    fecAccessManager_ = new FecAccessManager ( fecAccess_ ) ;
    fecAccessManager_->setMaxErrorAllowed ( maxErrorAllowed_ ) ;
    fecAccessManager_->setDisplayDebugMessage (displayDeviceDebugMessage_) ;

    // ---------------------------------------------------------------------------
    // FEC temperature
    try {
      int fecTemp0, fecTemp1 ;
      fecAccessManager_->getFecAccess()->getFecTemperature(buildFecRingKey(fecSlot_,0),fecTemp0,fecTemp1) ;
      if (fecTemp0 < 10 || fecTemp1 < 10 || fecTemp0 > 40 || fecTemp1 > 40) {
	std::stringstream msgError ; msgError << "The FEC temperature from slot " << std::dec << fecSlot_.toString() << " is too high or too low: " << fecTemp0 << " / " << fecTemp1 ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    catch (FecExceptionHandler &e) {
      
      errorReportLogger_->errorReport ("Unable to read FEC temperatures version on slot " + fecSlot_.toString(), e, LOGFATAL) ;
      
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: Unable to read FEC VME firmware version on slot "
				      << fecSlot_.toString() 
				      << std::endl << e.what()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }

    // ---------------------------------------------------------------------------
    // Make a reset of the FEC
    setFecReset ( ) ;

    // ---------------------------------------------------------------------------
    // status for XMAS
    refreshParameters() ;

    // ---------------------------------------------------------------------------
    // check the redundancy
    if (forceApplyRedundancy_) applyRedundancy_ = true ;
    for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = (applyRedundancy_ == true) ; // apply if the parameter is set
    if (applyRedundancy_) {
      setRedundancy ( ) ;
      applyRedundancy_ = false ; // set it to false, will not be applied anymore on the rings which where detected correctly
    }

    // ---------------------------------------------------------------------------
    // Making the configuration of hardware
    //hardwareConfiguration ( ) ;

    // In case of SSID problem
    if (!isFine) {
      fsm_.fireFailed("Fail",this);
      return false ;
    }
  }
  else {

    // Do not start the DCU/Device readout, nothing can be read
    dcuThreadUsed_    = false ;
    deviceThreadUsed_ = false ;

    // Fire a fail state
    fsm_.fireFailed("Fail",this);
    return false ;
  }

#ifdef MEASURETIMING
  unsigned long gEndMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream measuretiming ; 
  measuretiming << "FecSupervisor on slot " << fecSlot_.toString() << " (FEC hardware ID = " << fecHardwareId_ << ") initialising (in total) " << (gEndMillis-gStartMillis) << " ms" << "(I2CSpeed "<< fecAccessManager_->getFecAccess()->geti2cChannelSpeed () <<" kHz)"; 
  std::cout << measuretiming.str() << std::endl ;
  errorReportLogger_->errorReport( measuretiming.str(), LOGUSERINFO ) ;
#endif

  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("InitialiseDone",this);

  return false ;
}

/** Create the differents access database, FecAccess, FecAccessManager
 * \warning this method delete first all the possible accesses
 */
//void FecSupervisor::destroyAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {
bool FecSupervisor::destroying(toolbox::task::WorkLoop* wl) {

  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": RCMS destroy action" ;
  errorReportLogger_->errorReport("destroying", LOGDEBUG) ;

  // Delete all accesses
  deleteAccesses ( ) ;

  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("DestroyDone",this);

  return false ;
}

/** Configure state: do the download/upload configuration
 * Tasks:
 * <ul>
 * <li> Manage the DCU work loop
 * <li> Manage the device work loop
 * </ul>
 * \warning to do this differents tasks, the corresponding boolean must be set to true
 */
//void FecSupervisor::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {
bool FecSupervisor::configuring(toolbox::task::WorkLoop* wl) {

#ifdef MEASURETIMING
  unsigned long gStartMillis = XMLPlatformUtils::getCurrentMillis();
#endif

#ifdef WAITFECHARDWAREACCESS
  usleep((unsigned int)(WAITFECHARDWAREACCESS * getApplicationDescriptor()->getInstance())) ;
#endif

  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " BEGIN", LOGINFO) ;

  // ---------------------------------------------------------------------------
  dcuStuck_ = false;
  // Making the configuration of hardware
  hardwareConfiguration (-1) ;
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After hardwareConfiguration()", LOGINFO) ;

  // Change the parameters in the work loop
  if (dcuThreadUsed_) manageDcuWorkLoop ( ) ;
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After manageDcuWorkLoop()", LOGINFO) ;
  if (deviceThreadUsed_) manageDeviceWorkLoop ( ) ;

  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " END", LOGINFO) ;
 
#ifdef MEASURETIMING
  unsigned long gEndMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream measuretiming ; 
  measuretiming << "FecSupervisor on slot " << fecSlot_.toString() << " (FEC hardware ID = " << fecHardwareId_ << ") configuring (in total) " << (gEndMillis-gStartMillis) << " ms"<< "(I2CSpeed "<< fecAccessManager_->getFecAccess()->geti2cChannelSpeed () <<" kHz)";
  std::cout << measuretiming.str() << std::endl ;
  errorReportLogger_->errorReport( measuretiming.str(), LOGUSERINFO ) ;
#endif

  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("ConfigureDone",this);

  return false ;
}


/** Resume state
 */
bool FecSupervisor::resuming(toolbox::task::WorkLoop* wl) {
  // for the mutex take with timeout
  struct timeval tv;
  tv.tv_sec=60;
  tv.tv_usec=0;
  std::stringstream msgWarn ; 
  std::cerr << "We are in FecSupervisor::resuming !!" << std::endl;
  // force the redundancy
  for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = true;

  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__), LOGINFO) ;
    // get the mutex
  //    mutexThread_->take() ;
  /*  
    if (mutexThread_->take(&tv) == EBUSY) {
      std::cerr << "Timeout taking mutexThread_ in FecSupervisor::resuming: mutexTaker_ " << mutexTaker_ <<std::endl;
      msgWarn << "Timeout taking mutexThread_ in FecSupervisor::resuming: mutexTaker_ " << mutexTaker_ ;
      errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING); 
      setRedundancy(false);
    } else {
      mutexTaker_ = 13;
      setRedundancy(false);
      // release the mutex
      mutexTaker_ = 0;
      mutexThread_->give() ;
    }
  */
  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("ResumeDone",this);

  return false ;
}

/** FixSoftError command
 */

bool FecSupervisor::fixingSoftError(toolbox::task::WorkLoop* wl) {
  std::cerr << "We are in FecSupervisor::fixingSoftError !!" << std::endl;
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__), LOGUSERINFO) ;
  std::stringstream msgWarn; 
  msgWarn << "Got fedid "<< fedid_;
  errorReportLogger_->errorReport (msgWarn.str(), LOGUSERINFO);

  // force the redundancy
  for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = true;
  // for the mutex take with timeout
  struct timeval tv;
  tv.tv_sec=60;
  tv.tv_usec=0;
  bool fed101 = false;
  bool fed258 = false;
  for (int i=0; i<8; i++) fed101 |= fcon_[i][101]; //TI slot 21
  for (int i=0; i<8; i++) fed258 |= fcon_[i][258]; //TM slot 16
  int slot;
  std::stringstream(fecSlot_.toString()) >> slot;

  if ((fedid_ == 101 && fed101) || (fedid_ ==-1 && fed258))
    {
    if (mutexThread_->take(&tv) == EBUSY) {
      std::cerr << "Timeout taking mutexThread_ in FecSupervisor::fixingSoftError: mutexTaker_ " << mutexTaker_ <<std::endl;
      std::stringstream msgWarn;
      msgWarn << "Timeout taking mutexThread_ in FecSupervisor::fixingSoftError: mutexTaker_ " << mutexTaker_ ;
      errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING);
      ringdiag (3);
      setRedundancy(false);
    } else {
      mutexTaker_ = 15;
      ringdiag (3);
      setRedundancy(false);
      // release the mutex
      mutexTaker_ = 0;
      mutexThread_->give() ;
    }
  }
  else if (fedid_ != -1) {
      for (int i=0; i<8; i++) if(fcon_[i][fedid_]){
	std::stringstream fixMsg;
	fixMsg << "Fixing fedid "<< fedid_ << " configuring ring " << i+1;
	errorReportLogger_->errorReport (fixMsg.str(), LOGUSERINFO);
	ringdiag (i+1);
	hardwareConfiguration(i+1);
      }
  }
  else {
    // not caused by a tracker RSED 
  }

  errorReportLogger_->errorReport("End of FecSupervisor::fixingSoftError", LOGUSERINFO) ;
  // Fire the FSM to let RCMS know that the application is in correct state

  fsm_.fireEvent("FixSoftErrorDone",this);

  return false ;

}

/** Pause state
 */
bool FecSupervisor::pausing(toolbox::task::WorkLoop* wl) {
  std::cerr << "We are in FecSupervisor::pausing !!" << std::endl;
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__), LOGINFO) ;

  // force the redundancy
  for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = true;

  
  // for the mutex take with timeout
  struct timeval tv;
  tv.tv_sec=60;
  tv.tv_usec=0;
  std::stringstream msgWarn ; 
  // only the TIB fec on slot 21 
  int slot;
  std::stringstream(fecSlot_.toString()) >> slot;

  if (slot != 21) {
  // Fire the FSM to let RCMS know that the application is in correct state
    fsm_.fireEvent("PauseDone",this);
    return false;
  }

  if (isRedundancyNeeded()) {

    // force the redundancy
    for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = true;

    // get the mutex
    //    mutexThread_->take() ;
    if (dcuStuck_) {
      setRedundancy(false);
    }
    else if (mutexThread_->take(&tv) == EBUSY) {
      std::cerr << "Timeout taking mutexThread_ in FecSupervisor::pausing: mutexTaker_ " << mutexTaker_ <<std::endl;
      msgWarn << "Timeout taking mutexThread_ in FecSupervisor::pausing: mutexTaker_ " << mutexTaker_ ;
      errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING);
      // not really stuck
      //dcuStuck_ = true; 
      setRedundancy(false);
    } else {
      mutexTaker_ = 14;
      setRedundancy(false);
      // release the mutex
      mutexTaker_ = 0;
      mutexThread_->give() ;
    }
  }
  else 
    errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " setRedundancy not needed for this FecSupervisor", LOGUSERINFO) ;

  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("PauseDone",this);

  return false ;
}

/** Enable state: nothing
 */

bool FecSupervisor::enabling(toolbox::task::WorkLoop* wl) {
  // for the mutex take with timeout
  struct timeval tv;
  tv.tv_sec=60;
  tv.tv_usec=0;
  std::stringstream msgWarn ; 
  // force the redundancy

  for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = true;
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__), LOGDEBUG) ;
  if (hasRedundancy_ && (unsigned long)fecSlot_ == 16) {
    // get the mutex
  //    mutexThread_->take() ;
    if (mutexThread_->take(&tv) == EBUSY) {
      std::cerr << "Timeout taking mutexThread_ in FecSupervisor::enabling: mutexTaker_ " << mutexTaker_ <<std::endl;
      msgWarn << "Timeout taking mutexThread_ in FecSupervisor::enabling: mutexTaker_ " << mutexTaker_ ;
      errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING); 
      setRedundancy(false);
    } else {
      mutexTaker_ = 12;
      setRedundancy(false);
      // release the mutex
      mutexTaker_ = 0;
      mutexThread_->give() ;
    }
  }

  /* moved back to configuring
  // Change the parameters in the work loop
  
  if (dcuThreadUsed_) manageDcuWorkLoop ( ) ;
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After manageDcuWorkLoop()", LOGINFO) ;
  if (deviceThreadUsed_) manageDeviceWorkLoop ( ) ;
  */
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " END", LOGUSERINFO) ;
  
  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("EnableDone",this);

  return false ;
}

/** Enable state: nothing
 */
bool FecSupervisor::disabling(toolbox::task::WorkLoop* wl) {

  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__), LOGDEBUG) ;
  /* do nothing 
  // for the mutex take with timeout
  struct timeval tv;
  tv.tv_sec=60;
  tv.tv_usec=0;
  std::stringstream msgWarn ; 
  // sometime it gets stuck while taking the mutex
  // since disabling is now called at stop, this is more annoying than before
  // so let's put a timeout in the mutex take and stop the dcu workloop anyway 
  // problem should move to the next configure

`  // get the mutex
  //    mutexThread_->take() ;
    if (mutexThread_->take(&tv) == EBUSY) {
      std::cerr << "Timeout taking mutexThread_ in FecSupervisor::disabling: mutexTaker_ " << mutexTaker_ <<std::endl;
      msgWarn << "Timeout taking mutexThread_ in FecSupervisor::disabling: mutexTaker_ " << mutexTaker_ ;
      errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING); 
      // stop the dcu workloop anyway
      stopWorkLoop() ;      
    } else {
      mutexTaker_ = 11;
      // stop the dcu workloop
      stopWorkLoop() ;      
      // release the mutex
      mutexTaker_ = 0;
      mutexThread_->give() ;
    }
  */
  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("DisableDone",this);

  return false ;
}

/** This method will detect the DCU from a ring and enable the DCU work loop (then the values are sent to PVSS)
 */
//void FecSupervisor::configureDcuAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {
bool FecSupervisor::dcuconfiguring(toolbox::task::WorkLoop* wl) {

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    // Error reporting: sent to the 1st level of error diag
    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::dcuconfiguring)", LOGFATAL, 0, XDAQFEC_HARDWAREACCESSERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    // Fire the FSM to let RCMS know that the application is in correct state
    fsm_.fireFailed("Fail",this);
    //fsm_.fireFailed("No VME FEC board detected on this setup on the slot " + fecSlot_.toString(),this) ;

    return false ;
  }

  // Change the parameters in the DCU work loop
  if (dcuThreadUsed_) {

    // take the semaphore to avoid problem
    mutexThread_->take() ;
    mutexTaker_ = 1;
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport( "semaphore taken", LOGDEBUG) ;
#endif

    // remove the DCU to force the detection of the DCU
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Configuration DCU: remove the DCUs from the access manager" ;
    fecAccessManager_->removeDevices(DCU) ;
    // release the semaphore
    mutexTaker_ = 0;
    mutexThread_->give() ;
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport( "semaphore released", LOGDEBUG) ;
#endif

    // Re-enable the DCU work loop with the different parameters
    manageDcuWorkLoop ( ) ;
  }

  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("ConfigureDcuDone",this);

  return false ;
}

/** Halt action
 */
//void FecSupervisor::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {
bool FecSupervisor::halting(toolbox::task::WorkLoop* wl) {
  
  //errorReportLogger_->errorReport("halting", LOGDEBUG) ;

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    // Error reporting: sent to the 1st level of error diag
    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::halting)", LOGFATAL, 0, XDAQFEC_HARDWAREACCESSERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    // Fire the FSM to let RCMS know that the application is in correct state
    fsm_.fireFailed("Fail",this);
    //fsm_.fireFailed("No VME FEC board detected on this setup on the slot " + fecSlot_.toString(),this) ;    

    return false ;
  }

  // In case of halt (state) before the end of a download/upload operation
  fecAccessManager_->setHalt(false) ;

#ifdef DEBUGMSGERROR
  // Display the value of the control registers of the PLL
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Halt: display all PLL registers" ;
  pllAccess::displayPllRegisters (*fecAccessManager_->getFecAccess(),fecAccessManager_->getPllAccesses()) ;
#endif

  // Fire the FSM to let RCMS know that the application is in correct state
  fsm_.fireEvent("HaltDone",this);

  return false ;
}

/** Fail action
 */
void FecSupervisor::failAction(toolbox::Event::Reference event) throw (toolbox::fsm::exception::Exception) {

  // Determine reason for failure if there is one
  if(typeid(*event) == typeid(toolbox::fsm::FailedEvent)) {

    toolbox::fsm::FailedEvent &failedEvent =
      dynamic_cast<toolbox::fsm::FailedEvent&>(*event);
    xcept::Exception exception = failedEvent.getException();
 
    std::stringstream oss;
 
    oss << "Failure occurred when performing transition from: ";
    oss << failedEvent.getFromState();
    oss <<  " to: ";
    oss << failedEvent.getToState();
    oss << " exception history: ";
    oss << xcept::stdformat_exception_history(exception);
    
    //reason = oss.str();

    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;	
      textInformation_.str(oss.str()) ;
    }

    errorReportLogger_->errorReport( oss.str(), LOGFATAL) ;

    reasonOfFailure_ = oss.str() ;
  }
  else if(typeid(*event) == typeid(toolbox::Event)) {
    toolbox::Event &forceFailedEvent = dynamic_cast<toolbox::Event&>(*event);
    reasonOfFailure_ = forceFailedEvent.type() ;
  }
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                 Basic methods                                                                */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Method called for the hardware configuration
 * Tasks:
 * <ul>
 * <li> FEC reset
 * <li> Reset the PIA
 * <li> Donwload the parameters
 * <li> Upload the parameters
 * </ul>
 */
void FecSupervisor::hardwareConfiguration (int ringslot) {

  std::stringstream msgcounter;
  msgcounter << "start of hardwareConfiguration: getSr0Counter = "<< FecVmeRingDevice::getSr0Counter()<< " getTraFifoCounter = "<<FecVmeRingDevice::getTraFifoCounter() << " getTraFifoSent = " << FecVmeRingDevice::getTraFifoSent();

  errorReportLogger_->errorReport( msgcounter.str(), LOGUSERINFO ) ;
    

#ifdef MEASURETIMING
  unsigned long gStartMillis = XMLPlatformUtils::getCurrentMillis();
#endif

  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " BEGIN", LOGINFO) ;
  if (ringslot==-1) {
  // FEC reset asked
  if (resetFec_) { // || resetPia_) {
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Configuration: FEC reset" ;
    setFecReset ( ) ;
    applyRedundancy_ = true ; // should redo the redundancy
    errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After setFecReset", LOGINFO) ;
  }

  // check the redundancy
  if (forceApplyRedundancy_) applyRedundancy_ = true ;
  for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = (applyRedundancy_ == true) ; // apply if the parameter is set
  if (applyRedundancy_) {
    for (int i = 0 ; i < 9 ; i ++) redundancyRings_[i] = true ; // apply all redundancy for all the rings
    setRedundancy ( ) ; // only for the rings which have been set as to be checked
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After setRedundancy", LOGINFO) ;
  }

  // Reset the TTCRx
  // if (initTTCRx_) {
  //  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Configuration: Init TTCRx" ;
  //  initTTCRx ( ) ;
  // }

  // Make a PLL cold reset
  if (coldPllReset_) {
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Configuration: Cold PLL init" ;
    setColdPllReset () ;
    errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After setColdPllReset", LOGINFO) ;
  }

  // Make a PIA reset
  if (resetPia_ || initPia_) {
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Configuration: PIA reset" ;
    setPiaReset ( ) ;
    initPia_ = false ;
    errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After setPiaReset", LOGINFO) ;
  }
  }
  std::stringstream msgcounter3;
  msgcounter3 << "before setDownload: getSr0Counter = "<< FecVmeRingDevice::getSr0Counter()<< " getTraFifoCounter = "<<FecVmeRingDevice::getTraFifoCounter()<< " getTraFifoSent = " << FecVmeRingDevice::getTraFifoSent();

  errorReportLogger_->errorReport( msgcounter.str(), LOGUSERINFO ) ;

  // Make a download
  if (doDownload_) {
    setDownload (ringslot) ;
    errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After setDownload", LOGINFO) ;
  }

  // Make an upload
  if (doUpload_) {
    setUpload ( ) ;
    errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After setUpload", LOGINFO) ;
  }

  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " END", LOGINFO) ;

  std::stringstream msgcounter2;
  msgcounter2 << "end of hardwareConfiguration: getSr0Counter = "<< FecVmeRingDevice::getSr0Counter()<< " getTraFifoCounter = "<<FecVmeRingDevice::getTraFifoCounter()<< " getTraFifoSent = " << FecVmeRingDevice::getTraFifoSent();
  errorReportLogger_->errorReport( msgcounter2.str(), LOGUSERINFO ) ;

#ifdef MEASURETIMING
  unsigned long gEndMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream measuretiming ; 
  measuretiming << "FecSupervisor on slot " << fecSlot_.toString() << " (FEC hardware ID = " << fecHardwareId_ << ") hardware configuration tooks (in total) " << (gEndMillis-gStartMillis) << " ms"<< "(I2CSpeed "<< fecAccessManager_->getFecAccess()->geti2cChannelSpeed () <<" kHz)";
  std::cout << measuretiming.str() << std::endl ;
  errorReportLogger_->errorReport( measuretiming.str(), LOGUSERINFO ) ;
#endif

}

/** Delete all objects used during the life time of the FecSupervisor or during the initialisation
 */
void FecSupervisor::deleteAccesses ( ) {

  // Work Loop
  if (workFecSupervisorLoop_ != NULL) {
    stopWorkLoop() ;
    if (readoutTimer_) { delete readoutTimer_ ; readoutTimer_ = NULL ; }
    delete workFecSupervisorLoop_ ; workFecSupervisorLoop_ = NULL ;
  }
  
  // Remove all the access
  if (fecAccessManager_ != NULL) {
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destroy the access manager" ;
    delete fecAccessManager_ ;
    fecAccessManager_ = NULL ;
  }

  // Disable all the access to the FEC and devices
  if (fecAccess_ != NULL) {
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destroy the FEC hardware access" ;
    delete fecAccess_ ;
    fecAccess_ = NULL ;
  }
  
  // Disconnect the database if it is set
  if (fecFactory_ != NULL) {
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Delete the FEC factory" ;
    delete fecFactory_ ;
    fecFactory_ = NULL ;
    //databaseAccess_   = false ; => set in the web page
    databaseDownload_ = false  ;
  }

#if defined(TRACKERXDAQ)
  // Disconnect the database caching system
  if (dbClient_ != NULL) {
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Delete the database caching system" ;
    delete dbClient_ ;
    dbClient_ = NULL ;
  }
#endif

  // delete the map of faulty devices
  for (Sgi::hash_map<keyType,deviceDescription *>::iterator it = hashMapFaultyDevice_.begin() ; it != hashMapFaultyDevice_.end() ; it ++) {
    delete it->second ;
  }
  hashMapFaultyDevice_.clear() ;
}

/** Create the FEC Access class depending on the version of the FecSoftware and the FEC type
 * If the VME FEC is used, the VME file name must be used and defined (vmeFileName_)
 * \warning this method is the same method than the CrateController::createFecAccess
 */
FecAccess *FecSupervisor::createFecAccess ( ) {

  // hardware access
  FecAccess *fecAccess = NULL ;

  // Debug message
  if (fecBusType_ == fecBusTypeList[FECVME])
    errorReportLogger_->errorReport ( "Create a VME FEC on slot " + fecSlot_.toString(), LOGDEBUG) ;
  else if (fecBusType_ == fecBusTypeList[FECPCI])
    errorReportLogger_->errorReport ( "Create a PCI FEC on slot " + fecSlot_.toString(), LOGDEBUG) ;
  else if (fecBusType_ == fecBusTypeList[FECUSB])
    errorReportLogger_->errorReport ( "Create an USB FEC on slot " + fecSlot_.toString(), LOGDEBUG) ;
  else
    errorReportLogger_->errorReport ( "Please specify a FEC bus type on slot " + fecSlot_.toString(), LOGFATAL, 0, XDAQFEC_NOFECBUSTYPE) ;

  // FEC type
  enumFecBusType fecBusType = FECVME ;
  if (fecBusType_ == fecBusTypeList[FECPCI]) fecBusType = FECPCI ;
  else if (fecBusType_ == fecBusTypeList[FECVME]) fecBusType = FECVME ;
  else if (fecBusType_ == fecBusTypeList[FECUSB]) fecBusType = FECUSB ;
  else {

    errorReportLogger_->errorReport ("No bus (PCI, VME or USB) is specified cannot continue on slot " + fecSlot_.toString(), 
				     LOGFATAL, 0, XDAQFEC_NOFECBUSTYPE) ;
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
      fecAccess = new FecAccess (true, false, true, false, I2CSPEED) ;
      ringMin_ = 0 ; ringMax_ = 0 ;
      fecHardwareId_ = "PCI" ;
#endif
      break ;
    case FECVME: {
#if defined(BUSVMECAENPCI) || defined(BUSVMECAENUSB) || defined (BUSVMESBS)
      std::string strBusAdapterName ;
      if      (strBusAdapter_ == strBusAdapterList[SBS])     strBusAdapterName = FecVmeRingDevice::STR_BUSADAPTER_NAME[SBS]     ;
      else if (strBusAdapter_ == strBusAdapterList[CAENPCI]) strBusAdapterName = FecVmeRingDevice::STR_BUSADAPTER_NAME[CAENPCI] ;
      else if (strBusAdapter_ == strBusAdapterList[CAENUSB]) strBusAdapterName = FecVmeRingDevice::STR_BUSADAPTER_NAME[CAENUSB] ;

      // Only the access to the corresponding FEC slot is created
      unsigned int fecAddresses[MAX_NUMBER_OF_SLOTS] = {0} ;
      fecAddresses[fecSlot_] = FecVmeRingDevice::VMEFECBASEADDRESSES[fecSlot_] ;

      // bool forceAck, bool init, bool scan, scan crate
      if (vmeFileNamePnP_ != "") {
	fecAccess = new FecAccess (0, vmeFileName_, vmeFileNamePnP_, true, false, false, false, I2CSPEED, strBusAdapterName, blockMode_) ;
      }
      else {
	fecAccess = new FecAccess (0, vmeFileName_, fecAddresses, true, false, false, false, I2CSPEED, strBusAdapterName, blockMode_) ;
      }

      // After a crate reset, the plug and play must re-charged (if it is used)
      // crate reset if asked
      if (crateReset_) { fecAccess->crateReset() ; crateReset_ = false ; }

      ringMin_ = FecVmeRingDevice::getMinVmeFecRingValue() ; 
      ringMax_ = FecVmeRingDevice::getMaxVmeFecRingValue() ;

      // Retrieve the FEC hardware ID
      if (ringMin_ == 0) {
	for (unsigned int ring = FecVmeRingDevice::getMinVmeFecRingValue() ; ring <= FecVmeRingDevice::getMaxVmeFecRingValue() ; ring ++) {
	  try {
	    fecAccess->setFecRingDevice(buildFecRingKey(fecSlot_,ring)) ;

	    fecHardwareId_ = fecAccess->getFecHardwareId (buildFecRingKey(fecSlot_,ringMin_)) ;
	    errorReportLogger_->setFecHardwareId(fecHardwareId_) ;

	    std::stringstream msgInfo ; msgInfo << "Found a FEC on slot " << fecSlot_ << ", ring " << (int)ring << " with FEC hard ID " << fecHardwareId_; 
	    errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	  }
	  catch (FecExceptionHandler &e) {
	    std::stringstream msgInfo ; msgInfo << "No FEC on slot " << fecSlot_ << ", ring " << ring ; 
	    errorReportLogger_->errorReport (msgInfo.str(), e, LOGINFO) ; // at that moment, we do not know if the ring is faulty or if it is normal (thanks to S. Mersi)
	  }
	}
      }
      else {
	for (unsigned int ring =  FecVmeRingDevice::getMaxVmeFecRingValue() ; ring >= FecVmeRingDevice::getMinVmeFecRingValue() ; ring --) {
	  try {
	    fecAccess->setFecRingDevice(buildFecRingKey(fecSlot_,ring)) ;

	    fecHardwareId_ = fecAccess->getFecHardwareId (buildFecRingKey(fecSlot_,ringMin_)) ;
	    errorReportLogger_->setFecHardwareId(fecHardwareId_) ;

	    std::stringstream msgInfo ; msgInfo << "Found a FEC on slot " << fecSlot_ << ", ring " << (int)ring << " with FEC hard ID " << fecHardwareId_;
	    errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	  }
	  catch (FecExceptionHandler &e) {
	    std::stringstream msgInfo ; msgInfo << "No FEC on slot " << fecSlot_ << ", ring " << ring ; 
	    errorReportLogger_->errorReport (msgInfo.str(), e, LOGINFO) ; // mersi patch
	  }
	}
      }

      // retrieve the FEC hardware ID
      fecHardwareId_ = fecAccess->getFecHardwareId (buildFecRingKey(fecSlot_,ringMin_)) ;

      if (fecHardwareId_.size() != 14) { // Problem in the FEC hardware ID, send an error (can be a warning)
	std::stringstream msgError ; msgError << "Found a FEC on slot " << fecSlot_ << " with a bad FEC hardware ID " << fecHardwareId_ 
					      << ": size should be 14 and is " << std::dec << fecHardwareId_.size() << std::endl ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }

      // Crosscheck the firmware version and readout the FEC temperature
      // and check the status of the rings and triggers
      try {
	// Check of the firmware
	ringFirmwareVersion_ = fecAccess->getFecFirmwareVersion(buildFecRingKey(fecSlot_,ringMin_)) ;
	if ((unsigned int)(ringFirmwareVersion_) < MINFIRMWAREVERSION) {
	  std::stringstream msgWarn ; 
	  msgWarn << "The FEC on slot " << fecSlot_ 
		  << " does not support the multiframes mode, disable it (0x" 
		  << std::hex << ringFirmwareVersion_ << ")" ;
	  errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING) ;
	  multiFrames_ = false ;
	}

	// FEC firmware >= 1700
	if (ringFirmwareVersion_ <= fecFirmwareVersionExpected_) {
	  std::stringstream msgError ; msgError << "FEC firmware version " << " on slot " << fecSlot_.toString() << " is not correct: FEC firmware is " << ringFirmwareVersion_ << " and should be " << fecFirmwareVersionExpected_.toString() ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	}
      }
      catch (FecExceptionHandler &e) {
	
	errorReportLogger_->errorReport ("Unable to read FEC firmware version on slot " + fecSlot_.toString(), e, LOGFATAL) ;
	
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: Unable to read FEC firmware version on slot "
					<< fecSlot_.toString() 
					<< std::endl << e.what()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
      }

      try {
	// VME firmware >= 16
	if (fecAccess->getVmeVersion(buildFecRingKey(fecSlot_,ringMin_)) <= vmeFirmwareVersionExpected_) {
	  std::stringstream msgError ; msgError << "FEC VME firmware version " << " on slot " << fecSlot_.toString() << " is not correct: FEC firmware is " << fecAccess->getVmeVersion(buildFecRingKey(fecSlot_,ringMin_)) << " and should be " << vmeFirmwareVersionExpected_.toString() ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	}
      }
      catch (FecExceptionHandler &e) {
	
	errorReportLogger_->errorReport ("Unable to read FEC VME firmware version on slot " + fecSlot_.toString(), e, LOGFATAL) ;
	
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: Unable to read FEC VME firmware version on slot "
					<< fecSlot_.toString() 
					<< std::endl << e.what()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
      }

      try {
	// Trigger firmware >= 14
	if (fecAccess->getTriggerVersion(buildFecRingKey(fecSlot_,ringMin_)) <= triggerFirmwareVersionExpected_) {
	  std::stringstream msgError ; msgError << "FEC trigger firmware version " << " on slot " << fecSlot_.toString() << " is not correct: FEC firmware is " << fecAccess->getTriggerVersion(buildFecRingKey(fecSlot_,ringMin_)) << " and should be " << triggerFirmwareVersionExpected_.toString() ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	}
      }
      catch (FecExceptionHandler &e) {
	
	errorReportLogger_->errorReport ("Unable to read FEC trigger firmware version on slot " + fecSlot_.toString(), e, LOGFATAL) ;
	
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: Unable to read FEC Trigger firmware version on slot "
					<< fecSlot_.toString() 
					<< std::endl << e.what()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
      }
#endif
      break ;
    }
    case FECUSB:
#if defined(BUSUSBFEC)
      ringMin_ = 0 ; ringMax_ = 0 ;
      fecAccess = new FecAccess (0,true, false, true, false, I2CSPEED) ;
      fecHardwareId_ = "USB" ;
#endif
      break ;
    }

    // Get the FEC list
    fecList = fecAccess->getFecList() ;
    if ((fecList == NULL) || (fecList->empty())) {
      if (fecList != NULL) delete fecList ;
      errorReportLogger_->errorReport ("No VME FEC board detected on this setup on the slot " + fecSlot_.toString(), LOGFATAL, 0, XDAQFEC_NOFECDETECTED) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "FATAL Error: No VME FEC board detected on this setup on slot "
				      << fecSlot_.toString()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
      

      delete fecAccess ; fecAccess = NULL ; // No FEC here
    }
  }
  catch (FecExceptionHandler &e) {

    errorReportLogger_->errorReport ("Unable to create a FEC access on slot " + fecSlot_.toString(), e, LOGFATAL) ;

    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: FEC: Unable to create a FEC access on slot "
				    << fecSlot_.toString() 
				    << std::endl << e.what()
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    if (fecAccess != NULL) delete fecAccess ; fecAccess = NULL ; // No FEC here or fatal error
  }
  
  delete fecList;

  return (fecAccess) ;
}


/* ************************************************************************************************************ */
/*                                                                                                              */
/*                              Database                                                                        */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Create an access to the database. First check if the env. variables are set (DBCONFLOGIN,
 * DBCONFPASSWD, DBCONFPATH. If not, check the export params
 */
void FecSupervisor::createDatabaseAccess ( ) {
  
#ifdef DATABASE
  // if not simply disable the database until a next connect is performed
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
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;

      fecFactory_ = new FecFactory ( dbLogin_, dbPasswd_, dbPath_ ) ;
      
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
    }  
  }
#else     // DATABASE
  
  databaseAccess_ = false ;
  dbLogin_ = "nil" ;
  dbPasswd_ = "nil" ;
  dbPath_ = "nil" ;
#endif    // DATABASE

  databaseDownload_ = databaseAccess_ ;
}

/** Retrieve the partition versus the version and update the parameters coming from the database
 * ie partitionId_, versionMajorId_, versionMinorId_ and the mask version
 * This method make an access to the database
 */
void FecSupervisor::getPartitionVersion ( ) {

#ifdef DATABASE
  // Retrieve the current version
  if (databaseAccess_ && databaseDownload_) {

    try {
      // ---------------------------------------------------------------------------------------------------------------------------
      // FEC device version
      std::stringstream msgInfo ; msgInfo << "Access the database and try to retrieve version versus the partition " << partitionName_.toString() ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
      
      // Get the current version
      fecFactory_->getFecDevicePartitionVersion ( partitionName_.toString(), &versionMajorId_, &versionMinorId_, &maskMajor_, &maskMinor_, &partitionId_ ) ;
      
      // Partition / version and update the export parameters for the run control
      if (partitionId_ != 0) {
	std::string partitionName = fecFactory_->getDatabaseAccess()->getPartitionName (partitionId_) ;
	if (partitionName != partitionName_.toString()) {
	  std::stringstream msgFatal ; msgFatal << "The partition (FEC device) set by the RMCS does not corresponds to the partition from the database " << partitionName_.toString() << " / " <<  partitionName ;
	  errorReportLogger_->errorReport (msgFatal.str(), LOGFATAL) ;
	}
	else {
	  msgInfo.str("") ; msgInfo << "\tPartition " << partitionName << "(" << partitionId_ << "): FEC device Version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ; 
	}
      }
      else {
	std::stringstream msgError ; msgError << "FEC device version for the partition " << partitionName_.toString() << " has a problem (cannot retrieve the partition id)" << std::endl ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    catch (FecExceptionHandler &e) {
      
      errorReportLogger_->errorReport ("Unable to retrieve the partition/FEC version on the current database", e, LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "ERROR: Unable to retrieve the partition/FEC version on the current database" 
				      << std::endl << e.what() 
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }
  }
#endif
}

/** Set the partitionversion with the value set in versionMajorId_ and versionMinorId_ and depend of the partitionId/partitionName
 * The partitionId and the partitionName is automatically recover and the version if different from the
 * previous is set. The first step is to recover the values and check if difference exists. If yes, the database
 * is update in a new state
 */
void FecSupervisor::setPartitionVersion ( ) {
#ifdef DATABASE
  // Retrieve the current version
  if (databaseAccess_ && databaseDownload_) {

    if ((versionMajorId_ != 0) || (versionMinorId_ != 0)) {
      fecFactory_->setFecDevicePartitionVersion (partitionName_.toString(), versionMajorId_, versionMinorId_) ;
      initDbVersion_ = true ;
    }
  }
#endif
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                              Download / Upload                                                               */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Download the values in the hardware and upload it (a comparison can
 * be performed)
 * \warning Update the DCUs for the thread that has to be read
 */
bool FecSupervisor::setDownload (int ringslot) {

  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " Start of FecSupervisor::setDownload", LOGINFO) ;

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::setDownload)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return false ;
  }

  // if the file exist parse it
#if defined(TRACKERXDAQ)
  if (dbClient_ == NULL) 
#endif
    if ( (!databaseAccess_ || !databaseDownload_) && (xmlFileName_ == UNKNOWNFILE)) {

      errorReportLogger_->errorReport ("No download source has been set, set the XML file name before or set the database for the FEC on slot " + fecSlot_.toString(), LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "ERROR: Cannot do operation on devices, set the XML file name before or set the database"
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ; 
      }
      
      return false ;
    }

  // An error ?
  bool errorsOccured = false ;

  // ----------------------------------------------------------------
  // Retrieve the devices
  deviceVector vDevice ;
  if (setUploadDone_ && !forceDownload_ && !dbClientChanged_) {

    for (Sgi::hash_map<keyType,deviceDescription *>::iterator it = hashMapFaultyDevice_.begin() ; it != hashMapFaultyDevice_.end() ; it ++) {
      vDevice.push_back(it->second) ;
    }

    std::stringstream msg ; msg << "An upload was done with all devices, the configuration will take care only about the devices that loose their configuration (" << vDevice.size() << " devices" << std::endl ;
    errorReportLogger_->errorReport(msg.str(),LOGWARNING); 
  }
  else {

    // We can consider now the version will not changed (until the next message)
    dbClientChanged_ = false ;

    // retrieve devices from DB client or from database
    try {
#if defined(TRACKERXDAQ)
      if (dbClient_ != NULL) {
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Download of the values: database caching system access to retrieve all devices" ;

	try {
	  dbClient_->parse() ;
          
	  if (ringslot==-1) {
	    ConnectionVector *vcon =dbClient_->getConnections();
	    if (vcon == 0)
	      std::cerr << "WARNING: vcon is null !!!" << std::endl;
	    else {
	      std::cerr << "Got vcon size="<<vcon->size()<<std::endl; 
	      for (int i=0; i<8; i++) for (int j=0; j<500; j++) fcon_[i][j]=false;
	      for (ConnectionVector::iterator it = vcon->begin() ; it != vcon->end() ; it ++) {
		if ((*it)->getFecSlot()==fecSlot_) {
		  // (*it)->display();
		  int rslot=(*it)->getRingSlot();
		  //int fid=(*it)->getFedId();
		  //std::cerr << "ring slot " << rslot << " fedid " << fid << std::endl;
		  if(rslot==0)
		    std::cerr << "WARNING: ring slot 0 from connection !!! ignored"<<std::endl;
		  else 
		    fcon_[(*it)->getRingSlot()-1][(*it)->getFedId()]=true;
		}
	      }
	    }
	    dbClient_->getDevices(fecHardwareId_, &vDevice) ;
	  } else 
	    dbClient_->getDevices(fecHardwareId_, ringslot, &vDevice) ;

	  //errorReportLogger_->errorReport(toString(vDevice.size()) + " FEC devices to be downloaded", LOGDEBUG) ;
	}
	catch (std::string &e) {
          
	  if (ringslot==-1) 
	    errorReportLogger_->errorReport ("No devices found from the DbClient for FEC on slot " + fecSlot_.toString(), LOGERROR) ;
	  else {
	    std::stringstream msgWarn ; 
	    msgWarn << "No devices found from the DbClient for FEC on slot " << fecSlot_ << " ring " << ringslot;
	    errorReportLogger_->errorReport (msgWarn.str(), LOGERROR) ;
	  }
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "ERROR: No devices found from the DbClient for FEC on slot " + fecSlot_.toString() + ", set the XML file name before or set the database" 
					<< std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ; 
	  }
	}
      } else 
#endif
	{
	  // Database or file: if the database is set, 0.0 means that you retrieve the current version from database, if the database is not set, the vector are retrieven from the file set with FecFactory::addFileName or FecFactory::setInputFileName
#ifdef DATABASE
	  if (databaseDownload_ && databaseAccess_) {
	    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Download of the values: database access to retrieve all devices" ;
	    fecFactory_->getFecDeviceDescriptions (partitionName_.toString(), fecHardwareId_, vDevice) ; //, versionMajorId_,versionMinorId_) ;
	  }
	  else 
#endif
	    {
	      // Parse the file for the devices and PIA reset in case of modifications   
	      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Download of the values: read data from file " + xmlFileName_.toString() ;
	      fecFactory_->setInputFileName(xmlFileName_.toString(), fecHardwareId_) ;
	      fecFactory_->getFecDeviceDescriptions ( vDevice ) ;
	    }
	}
    }
    // Error during download from file or database
    catch (FecExceptionHandler &e) {

      std::stringstream msgError ; 
      if (databaseAccess_)
	msgError << "Unable to retrieve the devices from the database" ;
      else
	msgError << "Unable to retrieve the devices from the file " << xmlFileName_.toString() ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
    }
    // Error during download from the DB client
    catch (std::string e) {
      
      std::stringstream msgError ; msgError << "Error in retrieving the devices from the database client (DbClient: " << e << ")" ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }
    
  // ----------------------------------------------------------------
  // Display the number of devices
#ifdef DEBUGMSGERROR
  // Display the number of devices for each type of FEC
  unsigned int dcuCcuNumber = 0, dcuFehNumber = 0, apvNumber = 0, pllNumber = 0, muxNumber = 0, dohNumber = 0, aohNumber = 0 ;
  for (deviceVector::iterator itDevice = vDevice.begin() ; itDevice != vDevice.end() ; itDevice ++) {
    switch ((*itDevice)->getDeviceType()) {
    case DCU: {
      dcuDescription *dcuD = dynamic_cast<dcuDescription *>(*itDevice) ;
      if (dcuD->getDcuType() == DCUFEH) dcuFehNumber ++ ;
      else if (dcuD->getDcuType() == DCUCCU) dcuCcuNumber ++ ;	  
      break ;
    }
    case APV25:
      apvNumber ++ ;
      break ;
    case APVMUX:
      muxNumber ++ ;
      break ;
    case LASERDRIVER: {
      laserdriverDescription *ldD = dynamic_cast<laserdriverDescription *>(*itDevice) ;
      if (getAddressKey(ldD->getKey()) == 0x60) aohNumber ++ ;
      else if (getAddressKey(ldD->getKey()) == 0x70) {
	std::cerr << "Device type not correctly set for DOH" << std::endl ;
	dohNumber ++ ;
      }
      break ;
    }
    case DOH:
      dohNumber ++ ;
      break ;
    case PLL: 
      pllNumber ++ ;
      break ;
    default:
      std::cerr << "Unknown device" << std::endl ;
      break ;
    }
  }
  std::cout << "Found " << std::dec << apvNumber << " APVs" << std::endl ;
  std::cout << "Found " << std::dec << muxNumber << " APV MUX" << std::endl ;
  std::cout << "Found " << std::dec << pllNumber << " PLLs" << std::endl ;
  std::cout << "Found " << std::dec << aohNumber << " AOHs" << std::endl ;
  std::cout << "Found " << std::dec << dohNumber << " DOHs" << std::endl ;
  std::cout << "Found " << std::dec << dcuCcuNumber << " DCUs on CCU" << std::endl ;
  std::cout << "Found " << std::dec << dcuFehNumber << " DCUs on FEH" << std::endl ;
#endif

  // ----------------------------------------------------------------
  // Download the devices
  if (vDevice.size() > 0) {

    // Suspend the DCU work loop during the download in order to avoid
    // Multiple access -> the synchronisation is done in the DcuThread with mutex
    mutexThread_->take() ;
    mutexTaker_ = 2;
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport( "semaphore taken", LOGDEBUG) ;
#endif

    try {
      // Download in the hardware
      // FecFactory::display(vDevice) ;
      unsigned int error = 0 ;
      std::list<FecExceptionHandler *> errorList ;

      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Download of the values: download all devices to the hardware" ;

      unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();

      if (multiFrames_) {
	errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " Start of fecAccessManager_->downloadValuesMultipleFrames", LOGINFO) ;	
	error = fecAccessManager_->downloadValuesMultipleFrames (&vDevice, errorList, coldPllReset_, configureDOH_) ;
	errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " End of fecAccessManager_->downloadValuesMultipleFrames", LOGINFO) ;	
      }
      else {
	error = fecAccessManager_->downloadValues (&vDevice, errorList, coldPllReset_, configureDOH_) ;
      }

      unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
      std::stringstream msgLocalInfo ;
      msgLocalInfo << "FecSupervisor on slot " << fecSlot_.toString() << " (FEC hardware ID = " << fecHardwareId_ << ") download of " << vDevice.size() << " FEC devices tooks " 
		   << (endMillis-startMillis) << " ms with " << errorList.size() << " errors during the download" ;
      errorReportLogger_->errorReport (msgLocalInfo.str(),LOGUSERINFO) ;

      // Error check
      if (error) {
	
	// Display and send the error to diagnostic system
	unsigned int errorDisplay = manageErrorList ( *errorReportLogger_, "Error during download", errorList ) ;

	// Message of too many errors
	if ( (fecAccessManager_->getMaxErrorAllowed() != 0) && (error >= fecAccessManager_->getMaxErrorAllowed()) ) {
	  
	  std::stringstream msgError ; msgError << "Too many errors (" << std::dec << fecAccessManager_->getMaxErrorAllowed() << ") occur during the download => stopping" ;

#  ifdef TRACKERXDAQ
	  if (dbClient_ != NULL) 
	    msgError << " for the database cache: " << fecHardwareId_ << " on slot " << std::dec << fecSlot_ ;
	  else
#  endif
#  ifdef DATABASE
	    if (databaseDownload_ && databaseAccess_)
	      msgError << " for the database for partition " << partitionName_.toString() << " (" << partitionId_ << "): FEC " 
		       << fecHardwareId_ << " on slot " << std::dec << fecSlot_ 
		       << " with version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	    else 
#  else
	      msgError << " for FEC " << fecHardwareId_ << " on slot " << std::dec << fecSlot_ << " for the file " << xmlFileName_.toString() ;
#  endif
	  
	  errorReportLogger_->errorReport (msgError.str(),LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str() << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error on console
	  errorReportLogger_->errorReport(msgError.str(), LOGERROR) ;
	  std::cout << msgError.str() << std::endl ;
	}
	else {
	 
	  std::stringstream msgError ; msgError << std::dec << errorDisplay << " errors during the parsing or downloading, check previous errors to know" ;
	  
#  ifdef TRACKERXDAQ
	  if (dbClient_ != NULL) 
	    msgError << " for the database cache" ;
	  else
#endif
#  ifdef DATABASE
	    if (databaseDownload_ && databaseAccess_) 
	      msgError << " for the database for partition " << partitionName_.toString() << " (" << partitionId_ 
		       << "): FEC device version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	    else 
#  else
	      msgError << " for the file " << xmlFileName_.toString() << std::endl ;
#  endif

	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str() << std::endl << textInformation_.str() << std::endl << std::endl ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error on console
	  errorReportLogger_->errorReport(msgError.str(), LOGERROR) ;
	  std::cout << msgError.str() << std::endl ;
	}
      }
      // No error
      else {

	if (vDevice.size() > 0) {

#if defined(TRACKERXDAQ)
	  if (dbClient_ != NULL) {

	    std::stringstream msgInfo ; msgInfo << "Parsing and downloading done successfully for the database cache" ;
	    errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
	    std::cout << msgInfo.str() << std::endl ;
	    if (displayFrameErrorMessages_) { 
	      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	      std::stringstream temp ; temp << msgInfo.str() << std::endl << std::endl << textInformation_.str() ;
	      textInformation_.str(temp.str()) ;
	    }
	  } else
#endif
#ifdef DATABASE
	    if (databaseDownload_ && databaseAccess_) {

	      // retrieve the current state for the partition manage by this instance of the FecSupervisor
	      getPartitionVersion() ;
	      
	      std::stringstream msgInfo ; msgInfo << "Parsing and downloading done successfully for the database: Partition " << partitionName_.toString() 
						  << "(" << partitionId_ << "): version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	      errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
	      std::cout << msgInfo.str() << std::endl ;
	      if (displayFrameErrorMessages_) { 
		if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
		std::stringstream temp ; temp << msgInfo.str() << std::endl << std::endl << textInformation_.str() ;
		textInformation_.str(temp.str()) ;
	      }
	    }
	    else
#endif
	      {
		std::stringstream msgInfo ; msgInfo << "Parsing and downloading done successfully for FEC for the file " << xmlFileName_.toString() ;
		errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
		std::cout << msgInfo.str() << std::endl ;
		if (displayFrameErrorMessages_) { 
		  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
		  std::stringstream temp ; temp << msgInfo.str() ; //" for:" << std::endl ;
		  //temp << " for:" << std::endl << "   " << std::dec << fecAccessManager_->
		  temp << std::endl << std::endl << textInformation_.str() ;
		  textInformation_.str(temp.str()) ;
		}
	      }
	}
	else {
	  
	  std::stringstream msgError ; 

#if defined(TRACKERXDAQ)
	  if (dbClient_ != NULL) 
	    msgError << "No device found to be downloaded in the DbClient" ;
	  else
#endif
	    if (databaseDownload_ && databaseAccess_)
	      msgError << "No device found to be downloaded for the partition  " << partitionName_.toString() << " in the database" ;
	    else
	      msgError << "No device found to be downloaded in the file " << xmlFileName_.toString() ;

	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  std::cout << msgError.str() << std::endl ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "FATAL Error: " << msgError.str() << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }
	}
      }
      // The vector cannot be deleted there, it is done in the delete of XMLFecDevice
    }
    catch (FecExceptionHandler &e) {

      if (e.getErrorCode() >= (MAXFECSUPERVISORERRORCODE+1)) {

	errorReportLogger_->errorReport ("An error occurs during the download from the database", e, LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: An error occurs during the download from the database"
					<< std::endl << e.what() 
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
	
	//databaseDownload_ = false ;
      }
      else {

	errorReportLogger_->errorReport ("Download error", e, LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: download error"
					<< std::endl << e.what()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
      }
    }

    // Suspend the DCU work loop during the download in order to avoid
    // Multiple access -> the synchronisation is done in the DcuThread with mutex
    mutexTaker_ = 0;
    mutexThread_->give() ;

#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport( "semaphore released", LOGDEBUG) ;
#endif
  }
  else {  // vDevice is empty

    // If we use intelligent configuration, the number of devices to be downloaded can be zero
    if (!setUploadDone_ || forceDownload_) {

      std::stringstream msgError ;

#if defined(TRACKERXDAQ)
      if (dbClient_ != NULL) { 
	msgError << "No device found to be downloaded for the database client with the shared memory " << shareMemoryName_.toString() ;
      }
      else 
#endif
	{
	  msgError << "No device found to be downloaded for the database: Partition " << partitionName_.toString() << "(" << partitionId_ 
		   << "): FEC device version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	}

      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      std::cout << msgError.str() << std::endl ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "FATAL Error: " << msgError.str() << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }
  }

  // delete the device retrieved => these vector should not be anymore deleted, 
  // the DbClient or the Factory is taking care about the delete
  // FecFactory::deleteVector ( vDevice ) ;
  // but it should be deleted when the intelligent configuration is used
  if (setUploadDone_ && !forceDownload_) {
    for (Sgi::hash_map<keyType,deviceDescription *>::iterator it = hashMapFaultyDevice_.begin() ; it != hashMapFaultyDevice_.end() ; it ++) {
      delete it->second ;
    }
    hashMapFaultyDevice_.clear() ;
  }

  // No upload done for the time (we just configure it)
  setUploadDone_ = false ;

  return (errorsOccured) ;
}


/** This method read the values from all the devices from access manager and
 * upload it in database or file corresponding to the last download.
 */
bool FecSupervisor::setUpload ( ) {

  // First time is fatal after it is INFO
  static enum logLevelError levelError = LOGERROR ;

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::setUpload)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return false ;
  }

  errorReportLogger_->errorReport( "--- FecSupervisor: Begin of device readout", LOGDEBUG) ;

  unsigned int errorsOccured = 0 ;
  // Upload only if devices has already been uploaded
  if (fecAccessManager_->getPllAccesses().size() || 
      fecAccessManager_->getPllAccesses().size() ||
      fecAccessManager_->getApvAccesses().size() ||
      fecAccessManager_->getAOHAccesses().size() ||
      fecAccessManager_->getDOHAccesses().size() ||
      fecAccessManager_->getApvMuxAccesses().size() ) {

    // Suspend the DCU work loop during the download in order to avoid
    // Multiple access -> Synchronisation is done in the 
    mutexThread_->take() ;
    mutexTaker_ = 3;
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport( "semaphore taken", LOGDEBUG) ;
#endif
    errorReportLogger_->errorReport ("Download the values from the hardware", LOGINFO) ;
    
    // Retrieve all the devices from the parsing class, do not upload the DCU
    deviceVector *vDevice = NULL ;
    std::list<FecExceptionHandler *> errorList ;
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Upload of the values: upload of the values from the hardware and make a comparison" ;
    if (!multiFrames_) vDevice = fecAccessManager_->uploadValues ( errorList, doComparison_, false, configureDOH_ ) ;
    else vDevice = fecAccessManager_->uploadValuesMultipleFrames ( errorList, doComparison_, false, configureDOH_ ) ;

    // Report all the errors
    errorsOccured = manageErrorList ( *errorReportLogger_, "Error during upload", errorList ) ;

    // An upload was done if a comparison is done
    if (doComparison_) setUploadDone_ = true ; 
    
    // Check and update the database or a file ...
    if ( (vDevice == NULL) || (vDevice->empty() ) ) {
      if (doComparison_) {
	if (!sleepDeviceWorkLoop_ && firstTimeDeviceWorkLoop_ && (workFecSupervisorLoop_ != NULL) && workFecSupervisorLoop_->isActive() && deviceThreadUsed_) {
	  firstTimeDeviceWorkLoop_ = false ;
	  errorReportLogger_->errorReport ("Device work loop start and no difference between the download and upload was detected", LOGUSERINFO) ;
	}

	errorReportLogger_->errorReport ("No difference between the download and upload was detected", LOGINFO) ;
      }
    }
    else {
      if (doComparison_) {
	std::stringstream msgError ; msgError << "Found " << vDevice->size() << " differences between the the download and upload" ;

	if (!sleepDeviceWorkLoop_ && firstTimeDeviceWorkLoop_ && (workFecSupervisorLoop_ != NULL) && workFecSupervisorLoop_->isActive() && deviceThreadUsed_) {
	  firstTimeDeviceWorkLoop_ = false ;
	  errorReportLogger_->errorReport ("Device work loop start and " + msgError.str(), LOGUSERINFO) ;
	}

	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;

	// some devices have been dectected in error
	for (deviceVector::iterator it = vDevice->begin() ; it != vDevice->end() ; it ++) {
	  Sgi::hash_map<keyType,deviceDescription *>::iterator desmap = hashMapFaultyDevice_.find((*it)->getKey());
	  if (desmap == hashMapFaultyDevice_.end()) { // if it is not present then add it to the faulty devices
	    hashMapFaultyDevice_[(*it)->getKey()] = *it ;
	  }
	  else { // delete the corresponding description
	    delete *it ;
	  }
	}
      }
    }
    
    // Delete the elements in the vector of devices
    // FecFactory::deleteVectorI (*vDevice) ; => do not delete it since the devices can be stored in hashMapFaultyDevice_ 
    if (vDevice != NULL) delete vDevice ;

    // Suspend the DCU work loop during the download in order to avoid
    // Multiple access
    mutexTaker_ = 0;
    mutexThread_->give() ;
    
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport( "semaphore released", LOGDEBUG) ;
#endif
  }
  // No device has been uploaded
  else {
    errorReportLogger_->errorReport ("No device has been uploaded to the hardware, cannot perform an upload", levelError) ;
    levelError = LOGINFO ;
  }

  errorReportLogger_->errorReport ("--- FecSupervisor: End of device readout", LOGDEBUG) ;

  return (errorsOccured) ;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*               Device & DCU Work Loop                                                                         */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Submit the methods to the work loop depending of the parameters set by RCMS
 */
void FecSupervisor::submitWorkLoopReadout ( ) {

  static unsigned int counterDcuTime = counterDcuTime_ ;
  static unsigned int counterDeviceTime = counterDeviceTime_ ;

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::submitWorkLoopReadout)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
 				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // Inject the DCU readout
  if (workFecSupervisorLoop_ != NULL) {

    // ---------------------------------------------------
    // If the dcu work loop is not suspended
    if (dcuThreadUsed_) counterDcuTime ++ ;
    if (dcuThreadUsed_ && !suspendDcuWorkLoop_ && sleepDcuWorkLoop_ && (counterDcuTime >= counterDcuTime_)) {
      try {
	if (!workFecSupervisorLoop_->isActive()) workFecSupervisorLoop_->activate();
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Work loop submission: submit the DCU work loop" ;
	workFecSupervisorLoop_->submit(dcuReadoutWorkLoop_);
      }
      catch (toolbox::exception::QueueFull& qf) {
	errorReportLogger_->errorReport ("work loop queue is full (toolbox::exception::QueueFull): " + std::string(qf.what()), LOGWARNING) ;
      }
      catch (toolbox::task::exception::Exception &e) {

	std::string str = e.what() ;
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int)) std::tolower);
	if (str.find("queue full", 0) == std::string::npos)
	  if (!workFecSupervisorLoop_->isActive())
	    errorReportLogger_->errorReport ("Cannot start the DCU readout, problem in the work loop submission (toolbox::task::exception::Exception): " + std::string(e.what()), LOGFATAL) ;
	  else 
	    errorReportLogger_->errorReport ("Cannot submit the DCU readout, problem in the work loop submission (toolbox::task::exception::Exception): " + std::string(e.what()), LOGERROR) ;
	else
	  errorReportLogger_->errorReport ("Work loop queue is full (toolbox::exception::QueueFull): " + std::string(e.what()), LOGWARNING) ;
      }

      if (counterDcuTime >= counterDcuTime_) counterDcuTime = 1 ;
    }

    // ---------------------------------------------------
    // If the device work loop is not suspended
    if (deviceThreadUsed_) counterDeviceTime ++ ;
    if (deviceThreadUsed_ && !suspendDeviceWorkLoop_ && sleepDeviceWorkLoop_ && (counterDeviceTime > counterDeviceTime_)) {
      try {
	if (!workFecSupervisorLoop_->isActive()) workFecSupervisorLoop_->activate();
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Work loop submission: submit the device work loop" ;
	workFecSupervisorLoop_->submit(deviceReadoutWorkLoop_);
      }
      catch (toolbox::exception::QueueFull& qf) {
	errorReportLogger_->errorReport ("work loop queue is full (toolbox::exception::QueueFull): " + std::string(qf.what()), LOGWARNING) ;
      }
      catch (toolbox::task::exception::Exception &e) {

	std::string str = e.what() ;
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int)) std::tolower);
	if (str.find("queue full", 0) == std::string::npos)
	  if (!workFecSupervisorLoop_->isActive())
	    errorReportLogger_->errorReport ("Cannot start the device readout, problem in the work loop submission (toolbox::task::exception::Exception): " + std::string(e.what()), LOGFATAL) ;
	  else 
	    errorReportLogger_->errorReport ("Cannot submit the device readout, problem in the work loop submission (toolbox::task::exception::Exception): " + std::string(e.what()), LOGERROR) ;
	else
	  errorReportLogger_->errorReport ("Work loop queue is full (toolbox::exception::QueueFull): " + std::string(e.what()), LOGWARNING) ;
      }

      if (counterDeviceTime >= counterDeviceTime_) counterDeviceTime = 1 ;
    }
  }
  else {
    errorReportLogger_->errorReport ("Cannot start the work loop readout, work loop not known, incoherence in the software", LOGFATAL) ;
  }
}

/** Timer method called, this method inject the method dcuReadWorkLoop in the queue to be executed
 */
void FecSupervisor::timeExpired (toolbox::task::TimerEvent& e) {

  submitWorkLoopReadout () ;
}

/** Create the work loop and the timer needed by the readout
 */
void FecSupervisor::manageWorkLoop ( ) {

  // Define the counter for each of the operation (DCU or device)
  unsigned int workLoopTime = dcuWorkLoopTime_ ;
  counterDeviceTime_ = 0 ;
  counterDcuTime_ = 0 ;

  if ( (dcuThreadUsed_) && (deviceThreadUsed_) ) {
    workLoopTime = deviceWorkLoopTime_ > dcuWorkLoopTime_ ? dcuWorkLoopTime_ : deviceWorkLoopTime_ ;
    counterDeviceTime_ = lrint( ((double)deviceWorkLoopTime_ / (double)workLoopTime) ) ;
    counterDcuTime_ = lrint( ((double)dcuWorkLoopTime_ / (double)workLoopTime) ) ;
  }
  else if (dcuThreadUsed_) {
    workLoopTime = dcuWorkLoopTime_ ;
    counterDeviceTime_ = 0 ;
    counterDcuTime_ = 1 ;
  }
  else if (deviceThreadUsed_) {
    workLoopTime = deviceWorkLoopTime_ ;
    counterDcuTime_ = 0 ;
    counterDeviceTime_ = 1 ;
  }
 
  if (dcuThreadUsed_) dcuWorkLoopTime_ = counterDcuTime_ * workLoopTime ;
  if (deviceThreadUsed_) deviceWorkLoopTime_ = counterDeviceTime_ * workLoopTime ;

  // period of workLoopTime secs 
  static toolbox::TimeInterval interval(workLoopTime,0); 

  // Make the work loop if needed
  if (workFecSupervisorLoop_ == NULL) {

    // Set the work loop name
    if (workLoopName_ == DEFINENONE) {
      errorReportLogger_->errorReport ("Set the work loop name to " + std::string(WORKLOOPNAME), LOGWARNING) ;
      workLoopName_ = WORKLOOPNAME ;
    }

    // Create it
    try {
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Work loop management: work loop creation" ;
      // Retrieve the work loop
      workFecSupervisorLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop(workLoopName_, "waiting");
      if (!workFecSupervisorLoop_->isActive()) workFecSupervisorLoop_->activate();
      std::stringstream timerName;
      timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
      timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
      readoutTimer_ = toolbox::task::getTimerFactory()->createTimer(timerName.str());
      toolbox::TimeVal start = toolbox::TimeVal::gettimeofday() ; // + getApplicationDescriptor()->getInstance() * 30 ;
      readoutTimer_->scheduleAtFixedRate( start, this, interval,  0, "" );

      // First submission cause the timer does not start after the first initilialisation
      submitWorkLoopReadout ( ) ;

      if (dcuThreadUsed_)
	errorReportLogger_->errorReport ("Start the timer for the DCU readout with the name " + timerName.str() + " and time " + toString(counterDcuTime_ * workLoopTime) + " on the work loop" + workLoopName_.toString(), LOGINFO) ;
      if (deviceThreadUsed_) 
	errorReportLogger_->errorReport ("Start the timer for the device readout with the name " + timerName.str() + " and time " + toString(counterDeviceTime_ * workLoopTime) + " on the work loop" + workLoopName_.toString(), LOGINFO) ;

      //std::stringstream msgInfo ; msgInfo << "DCU & device readout started for the FEC" ;
      //errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
    }
    catch (toolbox::task::exception::InvalidListener &e) {
      errorReportLogger_->errorReport ("Cannot start the DCU & device readout for timer reason (toolbox::task::exception::InvalidListener): " + std::string(e.what()), LOGFATAL) ;
      try {
	if (workFecSupervisorLoop_ != NULL) {
	  // Delete the workloop for that supervisor
	  workFecSupervisorLoop_->cancel();
	  workFecSupervisorLoop_ = NULL ;
	}
      }
      catch (xdaq::exception::Exception& e) {
	errorReportLogger_->errorReport ("Cannot start the DCU & device readout, problem in the work loop", e, LOGFATAL) ;
      }  
    }
    catch (toolbox::task::exception::InvalidSubmission &e) {
      errorReportLogger_->errorReport ("Cannot start the DCU & device readout for timer reason (toolbox::task::exception::InvalidSubmission): " + std::string(e.what()), LOGFATAL) ;
      try {
	if (workFecSupervisorLoop_ != NULL) {
	  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Work loop management: stop the work loop" ;
	  // Delete the workloop for that supervisor
	  workFecSupervisorLoop_->cancel();
	  workFecSupervisorLoop_ = NULL ;
	}
      }
      catch (xdaq::exception::Exception& e) {
	errorReportLogger_->errorReport ("Cannot start the DCU & device readout, problem in the work loop", e, LOGFATAL) ;
      }  
    }
    catch (toolbox::task::exception::NotActive &e) {
      errorReportLogger_->errorReport ("Cannot start the DCU & device readout for timer reason (toolbox::task::exception::NotActive): " + std::string(e.what()), LOGFATAL) ;
      try {
	if (workFecSupervisorLoop_ != NULL) {
	  // Delete the workloop for that supervisor
	  workFecSupervisorLoop_->cancel();
	  workFecSupervisorLoop_ = NULL ;
	}
      }
      catch (xdaq::exception::Exception& e) {
	errorReportLogger_->errorReport ("Cannot start the DCU & device readout, problem in the work loop", e, LOGFATAL) ;
      }  
    }
    catch (toolbox::task::exception::Exception& e) {
      errorReportLogger_->errorReport ("Cannot start the DCU & device readout, problem in the work loop" + std::string(e.what()), LOGFATAL) ;
    }  
    catch (xdaq::exception::Exception& e) {
      errorReportLogger_->errorReport ("Cannot start the DCU & device readout, problem in the work loop", e, LOGFATAL) ;
    }  
  }
  else {
    if (readoutTimer_ != NULL) {
    // Change the delay of the timer or restart the dcu readout
      interval.sec(workLoopTime) ;
      try {
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Work loop management: change the timing between two readout" ;
	if (readoutTimer_->isActive()) readoutTimer_->stop() ;
	if (!readoutTimer_->isActive()) readoutTimer_->start() ;
	toolbox::TimeVal start = toolbox::TimeVal::gettimeofday() ; // + getApplicationDescriptor()->getInstance() * 30 ;
	readoutTimer_->scheduleAtFixedRate( start, this, interval,  0, "" );		
      }
      catch (toolbox::task::exception::InvalidListener &e) {
	errorReportLogger_->errorReport ("Cannot start the DCU & device readout for timer reason (toolbox::task::exception::InvalidListener): " + std::string(e.what()), LOGFATAL) ;
      }
      catch (toolbox::task::exception::InvalidSubmission &e) {
	errorReportLogger_->errorReport ("Cannot start the DCU & device readout for timer reason (toolbox::task::exception::InvalidSubmission): " + std::string(e.what()), LOGFATAL) ;
      }
      catch (toolbox::task::exception::NotActive &e) {
	errorReportLogger_->errorReport ("Cannot start the DCU & device readout for timer reason (toolbox::task::exception::NotActive): " + std::string(e.what()), LOGFATAL) ;
      }
    }
  }
}

/** Stop the DCU work loop
 */
void FecSupervisor::stopWorkLoop ( ) {
  errorReportLogger_->errorReport ("stopWorkLoop called", LOGUSERINFO); 
  // Stop the timer
  if (readoutTimer_ != NULL) {

    // Delete the timer, stop it
    try {
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Work loop management: stop the timer for the work loop" ;
      readoutTimer_->stop() ; 
    }
    catch (toolbox::task::exception::NotActive &e) {
      errorReportLogger_->errorReport ("Cannot stop the timer for the DCU & device readout, incoherence in the software (toolbox::task::exception::NotActive): " + std::string(e.what()), LOGERROR) ;
    }
    catch (toolbox::task::exception::NoJobs &e) {
      errorReportLogger_->errorReport ("Cannot stop the timer for the DCU & device readout, incoherence in the software (toolbox::task::exception::NoJobs): " + std::string(e.what()), LOGERROR) ;
    }
    catch (toolbox::task::exception::JobNotFound &e) {
      errorReportLogger_->errorReport ("Cannot stop the timer for the DCU & device readout, incoherence in the software (toolbox::task::exception::JobNotFound): " + std::string(e.what()), LOGERROR) ;
    }
  }

  // Stop the work loop
  if (workFecSupervisorLoop_ != NULL) {

    try {
      // Delete the workloop for that supervisor
      workFecSupervisorLoop_->cancel();
    }
    catch (toolbox::task::exception::Exception &e) {
      errorReportLogger_->errorReport ("Cannot stop the DCU & device readout work loop, incoherence in the software (toolbox::task::exception::Exception): " + std::string(e.what()), LOGERROR) ;
    }

    // Send a USERINFO
    std::stringstream msgInfo ; msgInfo << "DCU & device readout stopped" ;
    errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
  }
}

/** This method find all the DCU from the database or by detection and declare it in the FecAccessManager
 */
void FecSupervisor::detectDCU ( ) {

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::detectDCU)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // take the semaphore to avoid problem
  mutexThread_->take() ;
    mutexTaker_ = 4;
#ifdef DEBUGMSGERROR
  errorReportLogger_->errorReport( "semaphore taken", LOGDEBUG) ;
#endif

  deviceVector vDevice ;
  bool autoDetection = true ;
  
  // ------------------------------------------------------------------------
  //
  try {
    // Try to find all the DCUs with the file or database
    if (! autoDetection) {

      // ??????????????????????????????????????????????????????
      // DB client to be added
      //xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: access the database cache system to retrieve all the DCUs" ;


      // ??????????????????????????????????????????????????????
      // The DCU are now integrated in the devices vector
      // Retrieve all the devices from the input (database or file), only the DCU that are not disabled both for DCU readout and in the DAQ
      // xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: accessing the database to retrieve all the DCUs" ;
      //fecFactory_->getDcuDescriptions (partitionName_.toString(), fecHardwareId_, vDevice, 0, 0, false, false) ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    // The message are not displayed because of ambigious
    // problem with the user
    // errorReportLogger_->errorReport ("Error during the retrieve of the DCUs", e, LOGERROR) ;
    
    // A problem occurs during the parsing of the file
    // Make an automatic detection
    autoDetection = true ;
  }

  // ------------------------------------------------------------------------
  //
  if (!autoDetection) {

    // Download the values only for dcus
    if ( vDevice.empty() ) {
      autoDetection = true ;
    }
    else {

      // Vector of DCU
      deviceVector nDevice ;

      // For each device => access it
      for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {

        deviceDescription *deviced = (*device)->clone() ;	
        if (deviced->getDeviceType() == DCU) {
          nDevice.push_back (deviced) ;
        }

	//char msg[80] ; decodeKey(msg,deviced->getKey()) ;
	//errorReportLogger_->errorReport( "DCU on " + ((dcuDescription *)deviced)->getDcuType() + ": " + msg, LOGDEBUG) ;
      }

      // Download it
      deviceVector uDevice ;
      unsigned int error = 0 ;
      std::list<FecExceptionHandler *> errorList ;
      if (multiFrames_) {
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: connecting all the DCUs coming from database or database cache system" ;
	fecAccessManager_->downloadValuesMultipleFrames (&nDevice, errorList) ;
	// Check if errors has been reported
	manageErrorList ( *errorReportLogger_, "Error during the connection of the DCUs",  errorList) ;
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: upload the DCU from the hardware" ;
	error = fecAccessManager_->uploadValuesMultipleFrames ( uDevice, errorList ) ;
	// Check if errors has been reported
	manageErrorList ( *errorReportLogger_, "Error during the upload of the DCUs",  errorList) ;
      }
      else {
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: connecting all the DCUs coming from database or database cache system" ;
	fecAccessManager_->downloadValues (&nDevice, errorList) ;
	// Check if errors has been reported
	manageErrorList ( *errorReportLogger_, "Error during the connection of the DCUs",  errorList) ;
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: upload the DCU from the hardware" ;
	error = fecAccessManager_->uploadValues ( uDevice, errorList ) ;
	// Check if errors has been reported
	manageErrorList ( *errorReportLogger_, "Error during the upload of the DCUs",  errorList) ;
      }

      if (uDevice.size() > 0) {
          
        // Ok found dcu
	std::stringstream msgInfo ; msgInfo << "Found " << uDevice.size() << " DCU and starts the thread" ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;

	// Add only the DCU with no errors
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: remove all DCU in the access manager" ;
	fecAccessManager_->removeDevices (DCU) ;
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: connecting all the DCUs coming from the hardware" ;
	if (multiFrames_) {
	  fecAccessManager_->downloadValuesMultipleFrames (&uDevice, errorList) ;
	  manageErrorList ( *errorReportLogger_, "Error during the connection of the DCUs",  errorList) ;
	}
	else {
	  fecAccessManager_->downloadValues (&uDevice, errorList) ;
	  manageErrorList ( *errorReportLogger_, "Error during the connection of the DCUs",  errorList) ;
	}

        // Delete the upload values
	FecFactory::deleteVectorI (uDevice) ;
	uDevice.clear() ;
      }

      FecFactory::deleteVectorI (nDevice) ;
      nDevice.clear() ;
    }
  }

  // ------------------------------------------------------------------------
  //
  if (autoDetection) {

    // Display
    std::stringstream msgInfo ; msgInfo << "Start autodetection of DCUs" ;
    errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;

    // Clear the list in the access manager
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: remove all DCU in the access manager" ;
    fecAccessManager_->removeDevices (DCU) ;

    std::list<keyType> *fecList = NULL ;
    try {
      // Ask to FecAccess to scan for FECs and add it to the FecAccess table
      //fecList = fecAccess_->scanForFECs ( 0, 4 ) ;
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: retrieve the FEC list" ;
      fecList = fecAccess_->getFecList ( ) ;
    }
    catch (FecExceptionHandler &e) {

      errorReportLogger_->errorReport ("Error during the scan of the FECs", e, LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "ERROR: Error during the scan of the FECs"
				      << std::endl << e.what()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }

    // Just a display
    if (fecList != NULL) {

      for (std::list<keyType>::iterator p=fecList->begin();p!=fecList->end();p++) {
      
	keyType index = *p ;
	
	std::stringstream msgDebug ; msgDebug << "FEC " << std::dec << (int)getFecKey(index) << "." << (int)getRingKey(index) << " (SR0 = 0x" << std::hex << fecAccess_->getFecRingSR0(index) << ")" ;
	
	errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      }
    }
    else {
      
      errorReportLogger_->errorReport ("No VME FEC board detected on this setup", LOGFATAL) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "FATAL: No VME FEC board detected on this setup"
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }

    // Delete the list
    if (fecList != NULL) delete fecList ; 

    // Detect all the DCUs 
    keyType deviceValues[2] = {DCUADDRESS, NORMALMODE} ; // DCU

    // Retrieve all the DCUs connected to all the FEC rinfs
    std::list<keyType> *dcuList = NULL ;
    try {
      dcuList = fecAccess_->scanRingForI2CDevice ( deviceValues, 1, false, displayDebugMessage_ ) ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("Problem during the detection of some DCUs", e, LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "FATAL Error: Unable to detect any DCU on the FEC" 
				      << std::endl << e.what()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }

    if (dcuList != NULL) {

      std::stringstream msgInfo1 ; msgInfo1 << "Found " << std::dec << dcuList->size() << " DCUs for FEC" ;
      errorReportLogger_->errorReport (msgInfo1.str(), LOGINFO) ;

      // Vector of DCU
      deviceVector nDevice ;

      for (std::list<keyType>::iterator p=dcuList->begin();p!=dcuList->end();p++) {

	char msg[80] ;
	decodeKey(msg,*p) ;
	std::stringstream msgDebug ; msgDebug << "DCU " << msg << " found" ;
	errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;

	// Build a vector of dcuDescription
	dcuDescription *dcu = new dcuDescription ( *p, 0,0,0,0,0,0,0,0,0,0) ;
	if (fecBusType_ == fecBusTypeList[FECVME]) dcu->setFecHardwareId(fecHardwareId_,0) ; // => crate id not known by FecSupervisor
	
        // Add it to a vector of device description
        nDevice.push_back ( dcu ) ;
      }

      // Download it
      unsigned int error = 0 ;
      std::list<FecExceptionHandler *> errorList ;
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Detection of the DCU: connecting all the DCUs coming from the automatic detection" ;
      if (multiFrames_) error = fecAccessManager_->downloadValuesMultipleFrames (&nDevice, errorList) ;
      else error = fecAccessManager_->downloadValues (&nDevice, errorList) ;

      // Check if errors has been reported
      manageErrorList ( *errorReportLogger_, "Error during the connection of the DCUs",  errorList) ;

      std::stringstream msgInfo ; msgInfo << "Found " << nDevice.size() << " DCU and starts the thread" ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;

      FecFactory::deleteVectorI (nDevice) ;
      nDevice.clear() ;
    }
    else {

      std::stringstream msgError ; msgError << "Unable to detect DCUs from the hardware" ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: " << msgError.str() 
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }

    // Delete the dcu list
    delete dcuList ;
  }

  // delete the device retrieved => these vector should not be anymore deleted, 
  // the DbClient or the Factory is taking care about the delete
  // FecFactory::deleteVector ( vDevice ) ;

  // take the semaphore to avoid problem
  mutexTaker_ = 0;
  mutexThread_->give() ;

#ifdef DEBUGMSGERROR
  errorReportLogger_->errorReport( "semaphore released", LOGDEBUG) ;
#endif
}

/** This method readout the DCU, this method is executed once and queue by the timer
 */
bool FecSupervisor::dcuReadoutWorkLoop (toolbox::task::WorkLoop* wl) {

  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " Start of FecSupervisor::dcuReadoutWorkLoop", LOGINFO) ;
  std::cerr << "FecSupervisor::dcuReadoutWorkLoop wl = " << std::hex << (void *) wl << std::dec << std::endl;
 
  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::dcuReadoutWorkLoop)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return false ;
  }

  unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream msgInfo1 ; msgInfo1 << "FecSupervisor " << getApplicationDescriptor()->getInstance() << ": Begin of DCU readout: " << time(NULL) ;
  errorReportLogger_->errorReport( msgInfo1.str(), LOGDEBUG) ;

  sleepDcuWorkLoop_ = false ;

  errorReportLogger_->errorReport( "WORKLOOP: Next DCU readout", LOGDEBUG ) ;

  // take the mutex
  mutexThread_->take() ;
  mutexTaker_ = 5;

#ifdef DEBUGMSGERROR
  errorReportLogger_->errorReport( "DCU WorkLoop: semaphore taken", LOGDEBUG) ;
#endif

  errorReportLogger_->errorReport( "WORKLOOP: start the DCU readout", LOGUSERINFO ) ;
  
  // readout the DCU
  deviceVector dcuDevices ;
  std::list<FecExceptionHandler *> errorList ;
  xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": DCU work loop: upload DCU values" ;
  if (multiFrames_) {
    try {
    fecAccessManager_->uploadValuesMultipleFrames ( dcuDevices, errorList ) ;
    }
    catch (FecExceptionHandler e) {	 
      errorList.push_back(new FecExceptionHandler(e)) ;	 
      std::cerr << "Caught fec exception from fecAccessManager_->uploadValuesMultipleFrames: " << e.what() << std::endl;
    }	 
    catch (std::exception e) {
      std::cerr << "Caught std exception from fecAccessManager_->uploadValuesMultipleFrames: " << e.what() << std::endl;      
    }
    catch (...){
      std::cerr << "Caught unknown exception from fecAccessManager_->uploadValuesMultipleFrames: " << std::endl;
    }

    errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " After fecAccessManager_->uploadValuesMultipleFrames", LOGINFO) ;
  }
  else {
    for (dcuAccessedType::iterator it = fecAccessManager_->getDcuAccesses().begin() ; it != fecAccessManager_->getDcuAccesses().end() ; it ++) {
      
      // Read all the DCUs and build XML test needed by the database
      try {
	dcuDevices.push_back(it->second->getValues()) ;
      }
      catch (FecExceptionHandler &e) {
	errorList.push_back(new FecExceptionHandler(e)) ;
      }
    }
  }

  // Display errors
  unsigned int errors = FecSupervisor::manageErrorList ( *errorReportLogger_, "DCU WorkLoop: Error during the upload of the DCUs", errorList) ;
  std::stringstream msgInfo ; msgInfo << "DCU WorkLoop: read " << std::dec << dcuDevices.size() << " DCUs with " << errors << " errors in the readout" ;
  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
  
  // Readout the FEC temperature and if the value is greater than 50 then send an error
  int tempSensor1 = 0, tempSensor2 = 0 ;
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " Before FEC temperature readout", LOGINFO) ;
  if (fecBusType_ == fecBusTypeList[FECVME]) {
    try {
      xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": DCU work loop: read the FEC temperature" ;
      fecAccessManager_->getFecAccess()->getFecTemperature(buildFecRingKey(fecSlot_,0),tempSensor1,tempSensor2) ;
    }
    catch (FecExceptionHandler &e) {
      std::stringstream msgInfo ; msgInfo << "Unable to readout the temperature" ;
      errorReportLogger_->errorReport (msgInfo.str(), e, LOGERROR) ;
      tempSensor1 = tempSensor2 = 0 ;
    }
  }
  errorReportLogger_->errorReport( "WORKLOOP: end of readout, found " + toString(dcuDevices.size()) + " DCUs", LOGUSERINFO) ;    
  // release the mutex
  mutexTaker_ = 0;
  mutexThread_->give() ;

#ifdef DEBUGMSGERROR
  errorReportLogger_->errorReport( "semaphore released", LOGDEBUG) ;
#endif

  
    // Send to the DCU filter if existing
  if (dcuFilterDes_ != NULL) {
    if (dcuDevices.size()) {
      
      //#define CHECKDCUHARDID
#ifdef CHECKDCUHARDID
      for (deviceVector::iterator it = dcuDevices.begin() ; it != dcuDevices.end() ; it ++) {
	dcuDescription *dcuD1 = (dcuDescription *)(*it) ;
	if (dcuD1->getDcuHardId() == 0) {
	  char msg[80] ;
	  decodeKey(msg,dcuD1->getKey()) ;
	  std::stringstream msgError ; msgError << "Readout an invalid DCU hardware id (0) on " << msg ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	}
      }
#endif
      
      xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": DCU work loop: send the DCU values to the DCU filter" ;
      sendI2ODcuValues (dcuFilterDes_, dcuDevices) ;
    }
    if (tempSensor1 != 0 && tempSensor2 != 0) {
      xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": DCU work loop: send FEC temperatures to the DCU filter" ;
      sendI2OFecTemperature (dcuFilterDes_,buildFecRingKey(fecSlot_,0),tempSensor1,tempSensor2) ;
    }
  }

  // Send a message to the error report logger to signal the DCU readout started and the number of DCUs
  if (firstTimeDcuWorkLoop_) {
    firstTimeDcuWorkLoop_ = false ;
    errorReportLogger_->errorReport ("DCU readout started and " + toString(dcuDevices.size()) + " DCUs sent to the DCU filter", LOGUSERINFO) ;
  }
    
  if (displayDcuDebugMessage_) FecFactory::display (dcuDevices) ;

  // Delete the DCUs readout
  FecFactory::deleteVectorI(dcuDevices) ;

  sleepDcuWorkLoop_ = true ;

  unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream msgInfo2 ; msgInfo2 << "FecSupervisor " << getApplicationDescriptor()->getInstance() << ": End of DCU readout in " << (endMillis-startMillis) << " ms" ;
  errorReportLogger_->errorReport( msgInfo2.str(), LOGDEBUG) ;

  return false ;
}

/** This method readout all the devices, this method is executed once and queue by the timer
 */
bool FecSupervisor::deviceReadoutWorkLoop (toolbox::task::WorkLoop* wl) {

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::deviceReadoutWorkLoop)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return false ;
  }

  unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream msgInfo1 ; msgInfo1 << "FecSupervisor " << getApplicationDescriptor()->getInstance() << ": Begin of DCU readout: " << time(NULL) ;
  errorReportLogger_->errorReport( msgInfo1.str(), LOGDEBUG) ;

  sleepDeviceWorkLoop_ = false ;
  setUpload ( ) ;
  sleepDeviceWorkLoop_ = true ;

  unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream msgInfo2 ; msgInfo2 << "FecSupervisor " << getApplicationDescriptor()->getInstance() << ": End of device readout in " << (endMillis-startMillis) << " ms" ;
  errorReportLogger_->errorReport( msgInfo2.str(), LOGDEBUG) ;

  return false ;
}

/** Manage any change in the parameters of the DcuThread
 * So create if needed or kill it
 * change the time between two reads
 */
void FecSupervisor::manageDcuWorkLoop ( ) {

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::manageDcuWorkLoop)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // DCU work loop 
  if (dcuThreadUsed_) {
    
    // Detect the DCU for the readout of the DCU
    if (fecAccessManager_->getDcuAccesses().size() == 0) detectDCU() ;
    
    // Detect the DCU filter
    if (dcuFilterDes_ == NULL) {
      try {
	dcuFilterDes_ = getApplicationContext()->getDefaultZone()->getApplicationDescriptor(dcuFilterClassName_,dcuFilterInstance_) ;
	errorReportLogger_->errorReport ("Found the DcuFilter", LOGINFO) ;
      }
      catch (xdaq::exception::Exception& e) {
	
	errorReportLogger_->errorReport ("Cannot detect the DcuFilter", e, LOGWARNING) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "ERROR: cannot detect the DcuFilter" 
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
      }
    }

    // Manage the work loop (create or adjust the time) ;
    manageWorkLoop ( ) ;
  }
}

/** Manage any change in the parameters of the DcuThreads
 * So create if needed or kill it
 * change the time between two reads
 */
void FecSupervisor::manageDeviceWorkLoop ( ) {

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::manageDeviceWorkLoop)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // Device work loop 
  if (deviceThreadUsed_) {
    
    // Manage the work loop (create or adjust the time) ;
    manageWorkLoop ( ) ;
  }
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*           Method for all the possible reset (PLX, FEC, TTCRx, PIA)                                           */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Reset the PLX for all FECs
 * \warning this method catch the exception
 */
void FecSupervisor::setReloadFirmware () {

  // Check the the accesses was created
  if (fecAccess_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::setReloadFirmware)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // reload all the firmware on the card
  try {
    // No more device exists so the DCU is suspended and re-started
    mutexThread_->take() ;
    mutexTaker_ = 6;
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport ("semaphore taken", LOGDEBUG) ;
#endif

    // Reinitialise all hash_map
    if (fecAccessManager_ != NULL) fecAccessManager_->removeDevices () ;
    fecAccess_->fecHardReset( ) ; 

    // Message
    if (fecAccess_->getFecBusType() == FECVME) 
      errorReportLogger_->errorReport ("Firmware reloaded on FEC " + fecSlot_.toString() + "/" + fecHardwareId_, LOGUSERINFO) ;
    else 
      errorReportLogger_->errorReport ("Hardware reset done on FEC " + fecSlot_.toString(), LOGUSERINFO) ;

    // Re-start the DCU work loop
    mutexTaker_ = 0;
    mutexThread_->give() ;

#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport ("semaphore released", LOGDEBUG) ;
#endif
  }
  catch (FecExceptionHandler &e) {

    errorReportLogger_->errorReport ("Error during the reload of the firmware", e, LOGERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "ERROR: Error during the hardware reset" 
				    << std::endl << e.what() 
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }
}

/** Reset all FECs
 * \warning this method catch the exception
 */
void FecSupervisor::setFecReset () {
  // for the mutex take with timeout
  struct timeval tv;
  tv.tv_sec=60;
  tv.tv_usec=0;

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::setFecReset)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // No more device exists so the DCU is suspended and re-started

    mutexThread_->take() ;
    /*
    if (mutexThread_->take(&tv) == EBUSY) {
      std::cerr << "Timeout taking mutexThread_. Aborting !" << std::endl;
      abort ();
    } 
    */
    mutexTaker_ = 7;
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport ("semaphore taken", LOGDEBUG) ;
#endif

  // Reset the FEC
  try {
    // Make the reset: only reset the FEC not the crate is allowed
    errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " Before  fecAccessManager_->removeDevices ()", LOGINFO) ;
    fecAccessManager_->removeDevices () ;
    std::stringstream msgInfo ; msgInfo << "Reset the FEC" ;
    errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ; 

    std::list<keyType> *fecList = fecAccess_->getFecList() ;
    if ((fecList != NULL) && (!fecList->empty())) {
      for (std::list<keyType>::iterator fecRing = fecList->begin() ; fecRing != fecList->end() ; fecRing ++) {
	keyType index = *fecRing ;
	try {
	  if (fecBusType_ == fecBusTypeList[FECPCI]) fecAccess_->fecHardReset(index) ;
	  fecAccess_->fecRingResetFSM(index) ;
	  fecAccess_->fecRingReset(index) ;
	  fecAccess_->fecRingResetB(index) ;
	}
	catch (FecExceptionHandler &e) {
	  
	  errorReportLogger_->errorReport ("Error during the reset of FEC", e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "ERROR: Error during the reset of the FEC"
					  << std::endl << e.what()
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }
	}  
      }
    }
    else {
      if (fecList != NULL) delete fecList ;
      errorReportLogger_->errorReport ("Reset all FECs", LOGINFO) ;    
      fecAccess_->fecRingReset() ;
      if (fecBusType_ == fecBusTypeList[FECPCI]) fecAccess_->fecHardReset() ;
    }
  }
  catch (FecExceptionHandler &e) {

    errorReportLogger_->errorReport ("Error during the reset of FEC", e, LOGERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "ERROR: Error during the reset of the FEC"
				    << std::endl << e.what()
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }


  // Re-start the DCU work loop
    mutexTaker_ = 0;
    mutexThread_->give() ;

#ifdef DEBUGMSGERROR
  errorReportLogger_->errorReport ("semaphore released", LOGDEBUG) ;
#endif
}

/** Reset FEC CCS TTCRx
 * \warning this method catch the exception
 */
void FecSupervisor::initTTCRx () {

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::initTTCRx)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  try {
    errorReportLogger_->errorReport ("Initialise all the FEC TTCRxs", LOGINFO) ;

    // Initiliase the TTCRx
    if (fecBusType_ == fecBusTypeList[FECVME]) 
      fecAccess_->getCCSTrigger(buildFecRingKey(fecSlot_,ringMin_))->setResetTTCRx() ;
    else fecAccess_->initTTCRx() ;
  }
  catch (FecExceptionHandler &e) {

    errorReportLogger_->errorReport ("Error during the initialisation of the TTCRx", e, LOGERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "ERROR: Error during the initialisation of the TTCRx"
				    << std::endl << e.what() 
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }
}

/** Set the SSID for the CCS Trigger of the FEC boards
 */
bool FecSupervisor::setCCSTriggerSSID ( ) {

  if (SSID_ == SSID_NONE) return false ;

  bool fineOk = true ;

  // For each FEC board set the SSID
  std::list<keyType> *fecSlotList = NULL ;
  if (fecHardwareId_ != "0") {

    // Retrieve the corresponding FEC
    keyType index = buildFecRingKey(fecSlot_,ringMin_) ; //fecAccess_->getFecIndex ( fecHardwareId_ ) ;
    fecSlotList = new std::list<keyType> ;
    fecSlotList->push_back (index) ;

    std::stringstream msgInfo ; msgInfo << "Set the CCS Trigger SSID " << SSID_.toString() ;
    errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
  }
  else {
    // For each FEC board, retrieve the CCSTrigger and set the ssidSystem
    fecSlotList = fecAccess_->getFecList( ) ;

    if (fecSlotList != NULL) {
      std::stringstream msgInfo ; msgInfo << "Set the CCS Trigger SSID " << SSID_.toString() << " for the " << fecSlotList->size() << " FECs detected on the crate" ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }
  }
  
  int alreadySet[100] = {0} ;
  if ((fecSlotList != NULL) && (!fecSlotList->empty())) {
    for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
      keyType index = *p ;
      
      // Already set or not ?
      if (!alreadySet[getFecKey(index)]) {
	alreadySet[getFecKey(index)] = 1 ;
	try {
	  fecAccess_->setCCSTrigger ( index, SSID_.toString() ) ;	
	  statusSSID_ = fecAccess_->getSSID(index) ;
          if (initTTCRx_) fecAccess_->getCCSTrigger(index)->setResetTTCRx() ;   
	  std::stringstream msgInfo ; msgInfo << "SSID " << statusSSID_.toString() << " set for the FEC " << (int)getFecKey(index) ;
	  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	}
	catch (FecExceptionHandler &e) {
	  std::stringstream msgError ; msgError << "Unable to set the SSID " << SSID_.toString() << " for the FEC " << (int)getFecKey(index) ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	}
      }
    }
    
    if (fecSlotList != NULL) delete fecSlotList ;
  }

  // Trigger status
  try {
    statusQPLLError_ = statusQPLLLocked_ = statusTTCRx_ = NONEVALUE ;
    unsigned int triggerStatus0Value = fecAccess_->getCCSTriggerStatus0(buildFecRingKey(fecSlot_,ringMin_)) ;
    
    std::stringstream msgUserInfo ;
    if (triggerStatus0Value & CCS_SR0_QPLL_ERROR) { msgUserInfo << "QLL Error" ; statusQPLLError_ = 1 ; }
    else { msgUserInfo << "QPLL ok" ; statusQPLLError_ = 0 ; }
    if (triggerStatus0Value & CCS_SR0_QPLL_LOCKED) { msgUserInfo << " / QPLL locked" ; statusQPLLLocked_ = 1 ; }
    else { msgUserInfo << " / QPLL not locked" ; statusQPLLLocked_ = 0 ; }
    if (triggerStatus0Value & CCS_SR0_TTCRX_READY) { msgUserInfo << " / TTCRx ready" ; statusTTCRx_ = 1 ; }
    else { msgUserInfo << " / TTCRx not ready" ; statusTTCRx_ = 0 ; }
    msgUserInfo << " on FEC slot " << std::dec << fecSlot_.toString() ;

    if ( ((unsigned int)statusQPLLError_ == 1) || 
	 ((unsigned int)statusQPLLLocked_ == 0) || 
	 ((unsigned int)statusTTCRx_ == 0) ) { // Error

      unsigned int errorCode = 0 ;
      std::stringstream msgError ;
      if (statusQPLLError_) { errorCode = XDAQFEC_QPLLERROR ; msgError << XDAQFEC_QPLLERROR_MSG ; }
      if (!statusQPLLLocked_) { errorCode = XDAQFEC_QPLLNOTLOCKED ; msgError << XDAQFEC_QPLLNOTLOCKED_MSG ; }
      if (!statusTTCRx_) { errorCode = XDAQFEC_TTCRXNOTREADY ; msgError << XDAQFEC_TTCRXNOTREADY_MSG ; }

      msgError << " on FEC " << fecSlot_.toString() << ", FecHardwareId " << fecHardwareId_ ;
      FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER (errorCode, msgError.str(), FATALERRORCODE) ;
      errorReportLogger_->errorReport (msgUserInfo.str(), *e, LOGFATAL) ;
      delete e;

      fineOk = false ;
    }
    else errorReportLogger_->errorReport (msgUserInfo.str(), LOGUSERINFO) ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport ("Unable to read the FEC CCS trigger status on slot " + fecSlot_.toString(), e, LOGFATAL) ;
    
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "Error: Unable to read FEC VME firmware version on slot "
				    << fecSlot_.toString() 
				    << std::endl << e.what()
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    fineOk = false ;
  }

  return fineOk ;
}

/** Reset modules via PIA channels (send command to PIA channel)
 * \warning this method catch the exception
 */
void FecSupervisor::setPiaReset () {

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::setPiaReset)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // if the file exist parse it
#if defined(TRACKERXDAQ)
  if (dbClient_ == NULL) 
#endif
    if ( (!databaseAccess_ || !databaseDownload_) && (xmlFileName_ == UNKNOWNFILE)) {

      errorReportLogger_->errorReport ("Cannot do operation on devices, set the XML file name before or set the database", LOGWARNING) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "WARNING: Cannot do operation on devices, set the XML file name before or set the database"
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
		    
      return ;
    }

  // Vector of PIA reset
  piaResetVector vPiaReset ;
  try {

    if (fecHardwareId_ != "0") {
      std::stringstream msgInfo ; msgInfo << "Reset modules via PIA channels for FEC" ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }
    else 
      errorReportLogger_->errorReport ("Reset modules via PIA channels", LOGINFO) ;

#if defined(TRACKERXDAQ)
    // Database cache system
    if (dbClient_ != NULL) {
      try {
	dbClient_->parse() ;
	dbClient_->getPiaReset(fecHardwareId_, &vPiaReset) ;
	//errorReportLogger_->errorReport( toString(vPiaReset.size()) + " PIA reset to be downloaded", LOGDEBUG) ;
      }
      catch (std::string &e) {
	
	errorReportLogger_->errorReport ("No PIA reset found from the DbClient for FEC on slot " + fecSlot_.toString(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "ERROR: No PIA reset found from the DbClient for FEC on slot " + fecSlot_.toString() + ", set the XML file name before or set the database" 
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ; 
	}
      }
    } else 
#endif
      {
	// Try to retrieve values from database or file    
#ifdef DATABASE
	if (databaseAccess_ && databaseDownload_) {
	  
	  std::stringstream msgInfo ; msgInfo << "Retrieve the PIA reset from the database for FEC" ;
	  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	  
	  // Retrieve all the devices from the parsing class for the current version
	  fecFactory_->getPiaResetDescriptions (partitionName_.toString(), fecHardwareId_, vPiaReset, 0,0) ;
	}
	else
#endif
	  {
	    std::stringstream msgInfo ; msgInfo << "Parse the XML file " << xmlFileName_.toString() << " for PIA reset" ;
	    errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	    
	    // Parse the file for the devices and PIA reset in case of modifications      
	    fecFactory_->setInputFileName(xmlFileName_.toString(), fecHardwareId_) ;
	    
	    // Retrieve all the devices from the parsing class
	    fecFactory_->getPiaResetDescriptions ( vPiaReset ) ;
	  }
      }
  }
  // Error during download from file or database
  catch (FecExceptionHandler &e) {

    std::stringstream msgError ; 
    if (databaseAccess_)
      msgError << "Unable to retrieve the devices from the database" ;
    else
      msgError << "Unable to retrieve the devices from the file " << xmlFileName_.toString() ;
    errorReportLogger_->errorReport (msgError.str(),e,LOGERROR) ;
  }
  // Error during download from the DB client
  catch (std::string e) {

    std::stringstream msgError ; msgError << "Error in retrieving the devices from the database client (DbClient: " << e << ")" ;
    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
  }

  // PIA reset settings
  if ( ! vPiaReset.empty() ) {
    
    // Suspend the DCU work loop during the download in order to avoid
    // Multiple access -> the synchronisation is done in the DcuThread with mutex
    mutexThread_->take() ;
    mutexTaker_ = 8;
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport ("semaphore taken", LOGDEBUG) ;
#endif

    std::list<FecExceptionHandler *> errorList ;
    
    try {
      //FecFactory::display(vPiaReset) ;
      if (multiFrames_) fecAccessManager_->resetPiaModulesMultipleFrames ( &vPiaReset, errorList ) ;
      else fecAccessManager_->resetPiaModules ( &vPiaReset, errorList ) ;
    }
    catch (FecExceptionHandler &e) {
      
      errorReportLogger_->errorReport ("Error when a reset is performed through PIA/PIO channels", e, LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "ERROR: Error when a reset is performed through PIA/PIO channels"
				      << std::endl << e.what()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }

    // Suspend the DCU work loop during the download in order to avoid
    // Multiple access -> the synchronisation is done in the DcuThread with mutex
    mutexTaker_ = 0;
    mutexThread_->give() ;

#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport ("semaphore released", LOGDEBUG) ;
#endif

    // Report the errors
    unsigned int error = manageErrorList ( *errorReportLogger_, "Error during PIA reset", errorList ) ;

    if (error) {

      // Message of too many errors
      if (error >= fecAccessManager_->getMaxErrorAllowed()) {
	  
	std::stringstream msgError ; msgError << "Too many PIA errors (" << std::dec << fecAccessManager_->getMaxErrorAllowed() << ") occur during the download => stopping" ;
	 
#  ifdef TRACKERXDAQ
	if (dbClient_ != NULL) 
	  msgError << " for the database cache: " << fecHardwareId_ << " on slot " << std::dec << fecSlot_ ;
	else
#  endif
#  ifdef DATABASE
	  if (databaseDownload_ && databaseAccess_)
	    msgError << " for the database for partition " << partitionName_.toString() << " (" << partitionId_ << "): FEC " << fecHardwareId_ << " on slot " << std::dec << fecSlot_ 
		     << " with version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	  else 
#  else
	    msgError << " for FEC " << fecHardwareId_ << " on slot " << std::dec << fecSlot_ << " for the file " << xmlFileName_.toString() ;
#  endif
	 
	errorReportLogger_->errorReport (msgError.str(),LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str() << std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
      }
      else {
	 
	std::stringstream msgError ; msgError << std::dec << error << " PIA errors during the parsing or downloading, check previous errors to know" ;
	 
#  ifdef TRACKERXDAQ
	if (dbClient_ != NULL) 
	  msgError << " for the database cache: " << fecHardwareId_ << " on slot " << std::dec << fecSlot_ ;
	else
#endif
#  ifdef DATABASE
	  if (databaseDownload_ && databaseAccess_) 
	    msgError << " for FEC " << fecHardwareId_ << " on slot " << std::dec << fecSlot_ << " for the database for partition " << partitionName_.toString() 
		     << " (" << partitionId_ << "): FEC device version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	  else 
#  else
	    msgError << " for FEC " << fecHardwareId_ << " on slot " << std::dec << fecSlot_ << " for the file " << xmlFileName_.toString() << std::endl ;
#  endif
	 
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str() << std::endl << textInformation_.str() << std::endl << std::endl ;
	  textInformation_.str(temp.str()) ;
	}
	 
	// Error on console
	errorReportLogger_->errorReport(msgError.str(), LOGERROR) ;
	std::cout << msgError.str() << std::endl ;
      }
    }
  }
  else { // No PIA Reset

#if defined(TRACKERXDAQ)
    // Database caching system
    if (dbClient_ != NULL) {
      std::stringstream msgWarn ; msgWarn << "No PIA/PIO reset operation found in the database cache system" ;

      errorReportLogger_->errorReport (msgWarn.str(), LOGFATAL) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Warning: " << msgWarn.str()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    } else 
#endif
      {
#ifdef DATABASE
	if (databaseDownload_ && databaseAccess_) {
	  std::stringstream msgWarn ; msgWarn << "No PIA/PIO reset operation found for the database: Partition " << partitionName_.toString() << "(" << partitionId_ << ")" ; 
	  //: Pia Reset Version " << versionPiaMajor_ << "." << versionPiaMinor_ << std::endl ;
	  
	  // toString(versionPiaMajor_) + "." << toString(versionMinorId_)
	  errorReportLogger_->errorReport (msgWarn.str(), LOGFATAL) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Warning: " << msgWarn.str()
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }
	}
	else
#endif
	  {
	    std::stringstream msgWarn ; msgWarn << "No PIA/PIO reset operation found for the file " << xmlFileName_.toString() ;
	    errorReportLogger_->errorReport (msgWarn.str(), LOGFATAL) ;
	    if (displayFrameErrorMessages_) { 
	      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	      std::stringstream temp ; temp << "Warning: " << msgWarn.str()
					    << std::endl << textInformation_.str() ;
	      textInformation_.str(temp.str()) ;
	    }
	  }
      }
  }

  // delete the vector of PIA reset => these vector should not be anymore deleted, 
  // the DbClient or the Factory is taking care about the delete
  // FecFactory::deleteVector (vPiaReset) ;  
}


/** Reset the PLL through a specific method for cold conditions
 * \warning this method catch the exception
 */
void FecSupervisor::setColdPllReset () {

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::setColdPllReset)", LOGFATAL) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  try {
    
    errorReportLogger_->errorReport ("Reset PLL for cold conditions", LOGINFO) ;

    // Suspend the DCU work loop during the download in order to avoid
    // Multiple access -> the synchronisation is done in the DcuThread with mutex
    mutexThread_->take() ;
    mutexTaker_ = 9;
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport ("semaphore taken", LOGDEBUG) ;
#endif

    // Hardware reset
    std::list<FecExceptionHandler *> errorList ;
    unsigned int error = fecAccessManager_->setColdPllReset ( errorList, multiFrames_ ) ;

    // Suspend the DCU work loop during the download in order to avoid
    // Multiple access -> the synchronisation is done in the DcuThread with mutex
    mutexTaker_ = 0;
    mutexThread_->give() ;
    
#ifdef DEBUGMSGERROR
    errorReportLogger_->errorReport ("semaphore released", LOGDEBUG) ;
#endif

    // Display and send the error to diagnostic system
    manageErrorList ( *errorReportLogger_, "Error during download", errorList ) ;
     
    if (error) RAISEFECEXCEPTIONHANDLER ( XDAQFEC_PLLOPERATIONAL, "One or several errors occurs during the cold PLL reset", ERRORCODE ) ;
  }
  catch (FecExceptionHandler &e) {

    errorReportLogger_->errorReport ("Error during a cold PLL reset", e, LOGERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "ERROR: Error during a cold PLL reset"
				    << std::endl << e.what() 
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   I2O message                                                                */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Receive an I2O message and dispatch the command depend of the message
 */
toolbox::mem::Reference* FecSupervisor::senderTrackerCommandHandler(PI2O_TRACKER_COMMAND_MESSAGE_FRAME com) {

  mutexThread_->take() ;
  mutexTaker_ = 10;
#ifdef DEBUGMSGERROR
  errorReportLogger_->errorReport ("semaphore taken", LOGDEBUG) ;
#endif

  // Answer
  toolbox::mem::Reference* ackRef = NULL;
  PI2O_TRACKER_COMMAND_MESSAGE_FRAME reply ;

  if ((com->System != SYST_TRACKER) || (com->SubSystem != SUB_SYST_FEC ) || (fecAccessManager_ == NULL)) {

    // Check the the accesses was created
    if (fecAccessManager_ == NULL) {

      errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::senderTrackerCommandHandler)", LOGFATAL) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }
    else {
      std::stringstream msgError ; msgError << "FecSupervisor: Receive a message from a unknow system (System = " << com->System << " sub-system = " << com->SubSystem << " and command = " << com->Command ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: " << msgError.str() << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }

    ackRef = allocateTrackerCommand(1) ;
    reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
    reply->Error = -1 ;
  }
  else {

    // command
    U32 command = com->Command ;

    // Upload or download
    bool download = false ;
    char cmd[100] ; // in case of error

    switch (command) {

#ifndef DBCLIENT_DATABASECHANGED
#warning "Please define the message DBCLIENT_DATABASECHANGED for i2o message to signal that db cache has changed"
#define DBCLIENT_DATABASECHANGED 0xFFFFFFFF
#endif

    // -------------------------------------------------------------------------------------------------------------
    // Reconfigure the FEC without touching any other option
    case DBCLIENT_DATABASECHANGED: {
      dbClientChanged_ = true ;

      ackRef = allocateTrackerCommand(1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = 0 ;
      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Reconfigure the FEC without touching any other option
    case FEC_RECONFIGURING: {
      // Backup the previous values
      bool forceApplyRedundancy = forceApplyRedundancy_ ;
      bool applyRedundancy      = applyRedundancy_      ;
      bool coldPllReset         = coldPllReset_         ;
      bool resetPia             = resetPia_             ;
      bool initPia              = initPia_              ;
      bool doDownload           = doDownload_           ;
      bool doUpload             = doUpload_             ;

      // Default option for this command
      forceApplyRedundancy_ = false ; // do not force redundancy
      applyRedundancy_      = false ; // apply redundancy only on rings with problem identified and that can be corrected
      coldPllReset_         = false ; // cold PLL reset
      resetPia_             = false ; // reset PIA
      initPia_              = false ; // reset PIA
      doDownload_           = true  ; // download hardware configuration !!!!!
      doUpload_             = false ; // upload hardware configuration

      // Hardware configuration
      hardwareConfiguration (-1) ;

      // put the previous values
      forceApplyRedundancy_ = forceApplyRedundancy ;
      applyRedundancy_      = applyRedundancy      ;
      coldPllReset_         = coldPllReset         ;
      resetPia_             = resetPia             ;
      initPia_              = initPia              ;
      doDownload_           = doDownload           ;
      doUpload_             = doUpload             ;

      // Message to originator
      ackRef = allocateTrackerCommand(1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = 0 ;
      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Get the FEC hardware ID
    case FEC_GETHARDID: { // Return the FEC hardware ID

      // if it is a FEC PCI or a FEC USB then the FEC hardware ID returned is the instance of the FecSupervisor
      #define SIZESTRFECHARDWAREID 14

      int sizeFec = SIZESTRFECHARDWAREID ; // to avoid problem if the ID is not 8 digits in the i2o message buffer
      // int sizeFec = fecHardwareId_.size() ;

      ackRef = allocateTrackerCommand((int)(sizeFec/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      char fecHardIdStr[SIZESTRFECHARDWAREID+1] = {0} ;
      strncpy (fecHardIdStr,fecHardwareId_.c_str(),sizeFec) ;

      //errorReportLogger_->errorReport("I receive FEC_GETHARDID message and I return " + fecHardIdStr, LOGDEBUG) ;

      memset(reply->data,0,sizeFec) ;
      memcpy (reply->data, &fecHardIdStr, (sizeFec+1)*sizeof(char)) ; // No change, no upload      
      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // FEC commands
    case FEC_HARD_RESET: { // Reset the PLX
      // data[0] => FEC number, data[1] => ring number
      // if data[0] < 0 => all FEC plx are reseted
      // Only the error value is returned
      if (com->data[0] >= 0) {
        fecAccess_->fecHardReset( buildCompleteKey(com->data[0], com->data[1], 0, 0, 0) ) ;
      }
      else {
	fecAccess_->fecHardReset( ) ;
      }

      ackRef = allocateTrackerCommand(1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = 0 ;
      break ;
    }
 
    // -------------------------------------------------------------------------------------------------------------
    // Reset the FEC
    case FEC_SOFT_RESET: { 
      // data[0] => FEC number, data[1] => ring number
      // if data[0] < 0 => all FEC plx are reseted
      // Only the error value is returned
      if (com->data[0] >= 0)
	fecAccess_->fecRingReset( buildCompleteKey(com->data[0], com->data[1], 0, 0, 0) ) ; 
      else
	fecAccess_->fecRingReset( ) ;

      ackRef = allocateTrackerCommand(1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = 0 ;
      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // PIA commands
    case PIA_RESET: { // Reset through the PIA      
      // A specific PIA reset is given
      // data is a PIA reset description
      piaResetDescription piaResetd (*((piaResetDescription *)com->data)) ;
      
      ackRef = allocateTrackerCommand((int)(sizeof(piaResetDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      memcpy (reply->data, &piaResetd, sizeof(piaResetDescription)) ; // No change, no upload
      
      // Retrieve the access
      PiaResetAccess *piaReset = fecAccessManager_->getPiaAccess (piaResetd.getKey()) ;
      if (piaReset == NULL) {
       
	// Log the error
	decodeKey(cmd,piaResetd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The PIA reset " << cmd << " does not exist (index 0x" << std::hex << piaResetd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	memcpy (reply->data, &piaResetd, sizeof(piaResetDescription)) ;
	reply->Error = -1 ;
      }
      else {
	try {
	  piaReset->setValues (piaResetd) ;
	}
	catch (FecExceptionHandler &e) {
	  
	  // Log the error
	  decodeKey(cmd,piaResetd.getKey()) ;
	  std::stringstream msgError ; msgError << "PIA i2o message on " << cmd << " unable to reset through a PIA channel" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------      
    // APV commands
    case APV_SETVALUES: // Set the values and return the upload values
      download = true ;
    case APV_GETVALUES: { // Upload the values and return it

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      apvAccess *apv = (apvAccess *)fecAccessManager_->getAccess (APV25, apvd.getKey()) ;
      if (apv == NULL) {

	// Log the error
	decodeKey(cmd,apvd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The APV " << cmd << " does not exist (index 0x" << std::hex <<  apvd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
      }
      else {
	try {
	  if (download) apv->setValues (apvd) ; // download
	  apvDescription *apvU = apv->getValues ( ) ; // upload

	  // Copy the answer
	  memcpy (reply->data, apvU, sizeof(apvDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  delete apvU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,apvd.getKey()) ;
	  std::stringstream msgError ; msgError << "APV i2o message on " << cmd << ": unable to set an APV" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------      
    // APV commands for all APV in the same time
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_SETALLVALUES: {

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      bool 
	apvModeF = true, latencyF = true, muxGainF = true,
	ipreF = true,    ipcascF = true,  ipsfF = true,
	ishaF = true,    issfF = true,    ipspF = true,
	imuxinF = true,  icalF = true,    ispareF = false,
	vfpF = true,     vfsF = true,     vpspF = true,
	cdrvF = true,    cselF = true,    apvErrorF = false ;

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( apvd, errorList,
									     apvModeF, latencyF, muxGainF,
									     ipreF,    ipcascF,  ipsfF,
									     ishaF,    issfF,    ipspF,
									     imuxinF,  icalF,    ispareF,
									     vfpF,     vfsF,     vpspF,
									     cdrvF,    cselF,    apvErrorF ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of APVs (i2o command = APV_SETALLVALUES)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &apvd, sizeof(apvDescription)) ;

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------      
    // Set the latency, CDRV, CSEL for all APVs at the same time
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_ALLAPVLATENCY: {

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      bool 
	apvModeF = false, latencyF = true,  muxGainF = false,
	ipreF = false,    ipcascF = false,  ipsfF = false,
	ishaF = false,    issfF = false,    ipspF = false,
	imuxinF = false,  icalF = false,    ispareF = false,
	vfpF = false,     vfsF = false,     vpspF = false,
	cdrvF = false,    cselF = false,    apvErrorF = false ;

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( apvd, errorList,
									     apvModeF, latencyF, muxGainF,
									     ipreF,    ipcascF,  ipsfF,
									     ishaF,    issfF,    ipspF,
									     imuxinF,  icalF,    ispareF,
									     vfpF,     vfsF,     vpspF,
									     cdrvF,    cselF,    apvErrorF ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of APVs (i2o command = APV_ALLAPVLATENCY)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &apvd, sizeof(apvDescription)) ;

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Set the latency, CDRV, CSEL
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_CALIB: { // Upload the values and return it

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      apvAccess *apv = (apvAccess *)fecAccessManager_->getAccess (APV25, apvd.getKey()) ;
      if (apv == NULL) {

	// Log the error
	decodeKey(cmd,apvd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The APV " << cmd << " does not exist (index 0x" << std::hex <<  apvd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	//memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
      }
      else {
	try {
	  //apv->setValues (apvd) ; // download
	  apv->setLatency (apvd.getLatency()) ;
	  apv->setCdrv    (apvd.getCdrv()) ;
	  apv->setCsel    (apvd.getCsel()) ;
	  //apvDescription *apvU = apv->getValues ( ) ; // upload

	  // Copy the answer
	  //memcpy (reply->data, apvU, sizeof(apvDescription)) ;
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  //delete apvU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,apvd.getKey()) ;
	  std::stringstream msgError ; msgError << "APV i2o message on " << cmd << ": unable to set an APV" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------      
    // Set the latency, CDRV, CSEL for all APVs at the same time
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_ALLAPVCALIB: {

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      bool 
	apvModeF = false, latencyF = true, muxGainF = false,
	ipreF = false,    ipcascF = false,  ipsfF = false,
	ishaF = false,    issfF = false,    ipspF = false,
	imuxinF = false,  icalF = false,    ispareF = false,
	vfpF = false,     vfsF = false,     vpspF = false,
	cdrvF = true,    cselF = true,    apvErrorF = false ;

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( apvd, errorList,
									     apvModeF, latencyF, muxGainF,
									     ipreF,    ipcascF,  ipsfF,
									     ishaF,    issfF,    ipspF,
									     imuxinF,  icalF,    ispareF,
									     vfpF,     vfsF,     vpspF,
									     cdrvF,    cselF,    apvErrorF ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of APVs (i2o command = APV_ALLAPVCALIB)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &apvd, sizeof(apvDescription)) ;

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Set the VPSP
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_VPSP: {

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      apvAccess *apv = (apvAccess *)fecAccessManager_->getAccess (APV25, apvd.getKey()) ;
      if (apv == NULL) {

	// Log the error
	decodeKey(cmd,apvd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The APV " << cmd << " does not exist (index 0x" << std::hex <<  apvd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	//memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
      }
      else {
	try {
	  //apv->setValues (apvd) ; // download
	  apv->setVpsp (apvd.getVpsp()) ;
	  //apvDescription *apvU = apv->getValues ( ) ; // upload

	  // Copy the answer
	  //memcpy (reply->data, apvU, sizeof(apvDescription)) ;
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  //delete apvU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,apvd.getKey()) ;
	  std::stringstream msgError ; msgError << "APV i2o message on " << cmd << ": unable to set an APV" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Set the VPSP
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_ALLVPSP: {

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      bool 
	apvModeF = false, latencyF = false, muxGainF = false,
	ipreF = false,    ipcascF = false,  ipsfF = false,
	ishaF = false,    issfF = false,    ipspF = false,
	imuxinF = false,  icalF = false,    ispareF = false,
	vfpF = false,     vfsF = false,     vpspF = true,
	cdrvF = false,    cselF = false,    apvErrorF = false ;

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( apvd, errorList,
									     apvModeF, latencyF, muxGainF,
									     ipreF,    ipcascF,  ipsfF,
									     ishaF,    issfF,    ipspF,
									     imuxinF,  icalF,    ispareF,
									     vfpF,     vfsF,     vpspF,
									     cdrvF,    cselF,    apvErrorF ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of APVs (i2o command = APV_ALLVPSP)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &apvd, sizeof(apvDescription)) ;

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------      
    // Set the latency, CDRV, CSEL for all APVs at the same time
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_ALLAPVMODE: {

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      bool 
	apvModeF = true, latencyF =false, muxGainF = false,
	ipreF = false,    ipcascF = false,  ipsfF = false,
	ishaF = false,    issfF = false,    ipspF = false,
	imuxinF = false,  icalF = false,    ispareF = false,
	vfpF = false,     vfsF = false,     vpspF = false,
	cdrvF = false,    cselF = false,    apvErrorF = false ;

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( apvd, errorList,
									     apvModeF, latencyF, muxGainF,
									     ipreF,    ipcascF,  ipsfF,
									     ishaF,    issfF,    ipspF,
									     imuxinF,  icalF,    ispareF,
									     vfpF,     vfsF,     vpspF,
									     cdrvF,    cselF,    apvErrorF ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of APVs (i2o command = APV_ALLAPVMODE)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &apvd, sizeof(apvDescription)) ;

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Set the mode, isha, vfs, ical
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_PRECALIB: { // Upload the values and return it

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      apvAccess *apv = (apvAccess *)fecAccessManager_->getAccess (APV25, apvd.getKey()) ;
      if (apv == NULL) {

	// Log the error
	decodeKey(cmd,apvd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The APV " << cmd << " does not exist (index 0x" << std::hex <<  apvd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	//memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
      }
      else {
	try {
	  //apv->setValues (apvd) ; // download
	  apv->setApvMode (apvd.getApvMode()) ;
	  apv->setIsha (apvd.getIsha()) ;
	  apv->setVfs (apvd.getVfs()) ;
	  apv->setIcal (apvd.getIcal()) ;
	  //apvDescription *apvU = apv->getValues ( ) ; // upload

	  // Copy the answer
	  //memcpy (reply->data, apvU, sizeof(apvDescription)) ;
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  //delete apvU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,apvd.getKey()) ;
	  std::stringstream msgError ; msgError << "APV i2o message on " << cmd << ": unable to set an APV" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }




    // -------------------------------------------------------------------------------------------------------------
    // Set the mode,Latency
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_PREDELAY: { // Upload the values and return it

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      apvAccess *apv = (apvAccess *)fecAccessManager_->getAccess (APV25, apvd.getKey()) ;
      if (apv == NULL) {

	// Log the error
	decodeKey(cmd,apvd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The APV " << cmd << " does not exist (index 0x" << std::hex <<  apvd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	//memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
      }
      else {
	try {
	  //apv->setValues (apvd) ; // download
	  apv->setApvMode (apvd.getApvMode()) ;
	  apv->setLatency (apvd.getLatency()) ;
	  //apvDescription *apvU = apv->getValues ( ) ; // upload

	  // Copy the answer
	  //memcpy (reply->data, apvU, sizeof(apvDescription)) ;
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  //delete apvU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,apvd.getKey()) ;
	  std::stringstream msgError ; msgError << "APV i2o message on " << cmd << ": unable to set an APV" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &apvd, sizeof(apvDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------      
    // Set the latency, CDRV, CSEL for all APVs at the same time
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case APV_ALLPRECALIB: {

      apvDescription apvd (*((apvDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(apvDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      bool 
	apvModeF = true, latencyF = false, muxGainF = false,
	ipreF = false,    ipcascF = false,  ipsfF = false,
	ishaF = true,    issfF = false,    ipspF = false,
	imuxinF = false,  icalF = true,    ispareF = false,
	vfpF = false,     vfsF = true,     vpspF = false,
	cdrvF = false,    cselF = false,    apvErrorF = false ;

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( apvd, errorList,
									     apvModeF, latencyF, muxGainF,
									     ipreF,    ipcascF,  ipsfF,
									     ishaF,    issfF,    ipspF,
									     imuxinF,  icalF,    ispareF,
									     vfpF,     vfsF,     vpspF,
									     cdrvF,    cselF,    apvErrorF ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of APVs (i2o command = APV_ALLPRECALIB)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &apvd, sizeof(apvDescription)) ;

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Laserdriver commands
    case LASERDRIVER_SETVALUES: // Set the values and return the upload values
      download = true ;
    case LASERDRIVER_GETVALUES: { // Upload the values and return it

      laserdriverDescription laserd (*((laserdriverDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(laserdriverDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      laserdriverAccess *laser = (laserdriverAccess *)fecAccessManager_->getAccess (LASERDRIVER, laserd.getKey()) ;
      if (laser == NULL) {

	// Log the error
	decodeKey(cmd,laserd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The laserdriver " << cmd << " does not exist (index 0x" << std::hex << laserd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	memcpy (reply->data, &laserd, sizeof(laserdriverDescription)) ;
      }
      else {
	try {

	  if (download) laser->setValues (laserd) ; // download

	  laserdriverDescription *laserU = laser->getValues ( ) ; // upload

	  // Copy the answer
	  memcpy (reply->data, laserU, sizeof(laserdriverDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  delete laserU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,laserd.getKey()) ;
	  std::stringstream msgError ; msgError << "Laserdriver i2o message on " << cmd << ": unable to set a laserdriver" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &laserd, sizeof(laserdriverDescription)) ;
	  reply->Error = e.getErrorCode() ;

	  //i2cRecover(e) ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Laserdriver commands for all the laserdriver
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case LASERDRIVER_SETALLVALUES: {

      laserdriverDescription laserd (*((laserdriverDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(laserdriverDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( laserd, errorList ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of laserdrivers (i2o command = LASERDRIVER_SETALLVALUES)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &laserd, sizeof(laserdriverDescription)) ;

      break ;      
    }

    // -------------------------------------------------------------------------------------------------------------
    // DOH commands
    case DOH_SETVALUES: // Set the values and return the upload values
      download = true ;
    case DOH_GETVALUES: { // Upload the values and return it

      laserdriverDescription laserd (*((laserdriverDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(laserdriverDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      DohAccess *doh = (DohAccess *)fecAccessManager_->getAccess (DOH, laserd.getKey()) ;
      if (doh == NULL) {

	// Log the error
	decodeKey(cmd,laserd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The DOH " << cmd << " does not exist (index 0x" << std::hex << laserd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	memcpy (reply->data, &laserd, sizeof(laserdriverDescription)) ;
      }
      else {
	try {
	  if (download) doh->setValues (laserd) ; // download
	  laserdriverDescription *laserU = doh->getValues ( ) ; // upload

	  // Copy the answer
	  memcpy (reply->data, laserU, sizeof(laserdriverDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  delete laserU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,laserd.getKey()) ;
	  std::stringstream msgError ; msgError << "Laserdriver i2o message on " << cmd << ": unable to set a laserdriver" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in reply
	  memcpy (reply->data, &laserd, sizeof(laserdriverDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // MUX commands
    case MUX_SETVALUES: // Set the values and return the upload values
      download = true ;
    case MUX_GETVALUES: { // Upload the values and return it

      muxDescription muxd (*((muxDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(muxDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      muxAccess *mux = (muxAccess *)fecAccessManager_->getAccess (APVMUX, muxd.getKey()) ;
      if (mux == NULL) {

	// Log the error
	decodeKey(cmd,muxd.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The APV MUX " << cmd << " does not exist (index 0x" << std::hex << muxd.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	memcpy (reply->data, &muxd, sizeof(muxDescription)) ;
      }
      else {
	try {
	  if (download) mux->setValues (muxd) ; // download
	  muxDescription *muxU = mux->getValues ( ) ; // upload

	  // Copy the answer
	  memcpy (reply->data, muxU, sizeof(muxDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  delete muxU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,muxd.getKey()) ;
	  std::stringstream msgError ; msgError << "MUX i2o message on " << cmd << ": unable to set an APV Mux" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &muxd, sizeof(muxDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Mux commands for all the mux
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case MUX_SETALLVALUES: {

      muxDescription muxd (*((muxDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(muxDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( muxd, errorList ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of MUXs (i2o command = MUX_SETALLVALUES)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &muxd, sizeof(muxDescription)) ;

      break ;      
    }

    // -------------------------------------------------------------------------------------------------------------
    // PLL commands
    case PLL_SETVALUES: // Set the values and return the upload values
      download = true ;
    case PLL_GETVALUES: { // Upload the values and return it

      pllDescription plld (*((pllDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(pllDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      pllAccess *pll = (pllAccess *)fecAccessManager_->getAccess (PLL, plld.getKey()) ;
      if (pll == NULL) {

	// Log the error
	decodeKey(cmd,plld.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The PLL " << cmd << " does not exist (index 0x" << std::hex << plld.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	memcpy (reply->data, &plld, sizeof(pllDescription)) ;
      }
      else {
	try {
	  if (download) {
#ifdef DEBUGMSGERROR
	    std::cout << "Display the PLL during the i2o (mono-message) with delay fine " 
		      << (int)plld.getDelayFine() << " and delay coarse " 
		      << (int)plld.getDelayCoarse() << std::endl ;
	    pll->displayPllRegisters() ; // display the registers of the PLL
#endif

	    pll->setValues (plld) ; // download

#ifdef DEBUGMSGERROR
	    pll->displayPllRegisters() ; // display the registers of the PLL
#endif
	  }
	  pllDescription *pllU = pll->getValues ( ) ; // upload

	  // Copy the answer
	  memcpy (reply->data, pllU, sizeof(pllDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  delete pllU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,plld.getKey()) ;
	  std::stringstream msgError ; msgError << "PLL i2o message on " << cmd << ": unable to set a PLL" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &plld, sizeof(pllDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Pll commands for all the pll
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case PLL_SETALLVALUES: {

      pllDescription plld (*((pllDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(pllDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( plld, errorList ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of PLLs (i2o command = PLL_SETALLVALUES)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      memcpy (reply->data, &plld, sizeof(pllDescription)) ;      

      break ;
    }


    // -------------------------------------------------------------------------------------------------------------
    // Pll commands for all the pll
    // The reply contains the orginal value and the error
    // The device is not uploaded
    case PLL_SETALLADDDELAY: {

      S32 value = (S32)(*com->data) ;
      tscType8 delay = (tscType8)value ;

      ackRef = allocateTrackerCommand((int)(sizeof(pllDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->downloadValuesMultipleFrames ( delay, errorList ) ;

      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during download of PLLs (i2o command = PLL_SETALLADDDELAY)", errorList ) ;

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;

      // Copy the answer
      value = delay ;
      memcpy (reply->data, &value, sizeof(S32)) ;

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Apply a cold PLL reset on a specific PLL
    // The reply contains the orginal description and the error
    // The device is not uploaded
    case PLL_COLDRESET: { // Cold reset for the PLL

      pllDescription plld (*((pllDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(pllDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      pllAccess *pll = (pllAccess *)fecAccessManager_->getAccess (PLL, plld.getKey()) ;
      if (pll == NULL) {

	// Log the error
	decodeKey(cmd,plld.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The PLL " << cmd << " does not exist (index 0x" << std::hex << plld.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	memcpy (reply->data, &plld, sizeof(pllDescription)) ;
      }
      else {
	try {
	  pll->setPllDac ( plld.getPllDac() ) ; // set a value for the PLL DAQ
	  pll->pllInit ( ) ; // cold reset
	  pllDescription *pllU = pll->getValues ( ) ; // upload

	  // Copy the answer
	  memcpy (reply->data, pllU, sizeof(pllDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  delete pllU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,plld.getKey()) ;
	  std::stringstream msgError ; msgError << "PLL i2o message on " << cmd << ": unable to set a PLL" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &plld, sizeof(pllDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Apply a cold PLL reset on all PLLs
    // The reply contains only the error
    // The device is not uploaded
    case PLL_ALLCOLDRESET: { // Cold reset for the PLL

      // Set the values to all PLL access with the PLL DAQ set in the description
      pllDescription plld (*((pllDescription *)com->data)) ;
      for (pllAccessedType::iterator p=fecAccessManager_->getPllAccesses().begin();p!=fecAccessManager_->getPllAccesses().end();p++) {
	((pllAccess *)(p->second))->setPllDac ( plld.getPllDac() ) ;
      }

      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      std::list<FecExceptionHandler *> errorList ;
      unsigned int error = fecAccessManager_->setColdPllReset ( errorList, true ) ;
      
      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during the apply of PLL reset (i2o command = PLL_ALLCOLDRESET)", errorList ) ;
      
      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;
      
      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // DCU commands
    case DCU_GETVALUES: { // return the upload values

      dcuDescription dcud (*((dcuDescription *)com->data)) ;
      ackRef = allocateTrackerCommand((int)(sizeof(dcuDescription)/sizeof(int)) + 1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();

      // Retrieve the access
      dcuAccess *dcu = (dcuAccess *)fecAccessManager_->getAccess (DCU, dcud.getKey()) ;
      if (dcu == NULL) {

	// Log the error
	decodeKey(cmd,dcud.getKey()) ;
	std::stringstream msgError ; msgError << "(i2o message " << std::dec << com->Command << " from system " << com->System << "." << com->SubSystem << "): The DCU " << cmd << " does not exist (index 0x" << std::hex << dcud.getKey() << ")" ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Error: " << msgError.str()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}

	// Error in the reply
	reply->Error = -1 ;
	memcpy (reply->data, &dcud, sizeof(dcuDescription)) ;
      }
      else {
	try {
	  dcuDescription *dcuU = dcu->getValues ( ) ; // upload

	  // Copy the answer
	  memcpy (reply->data, dcuU, sizeof(dcuDescription)) ;
	  reply->Error = DD_RETURN_OK ;

	  // delete the uploaded values
	  delete dcuU ;
	}
	catch (FecExceptionHandler &e) {

	  // Log the error
	  decodeKey(cmd,dcud.getKey()) ;
	  std::stringstream msgError ; msgError << "DCU i2o message on " << cmd << ": unable to get a DCU" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	  if (displayFrameErrorMessages_) { 
	    if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	    std::stringstream temp ; temp << "Error: " << msgError.str()
					  << std::endl << e.what() 
					  << std::endl << textInformation_.str() ;
	    textInformation_.str(temp.str()) ;
	  }

	  // Error in the reply
	  memcpy (reply->data, &dcud, sizeof(dcuDescription)) ;
	  reply->Error = e.getErrorCode() ;
	}
      }

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // DCU commands
    case DCU_GETALLVALUES: { // return the upload values

      // Vector of DCUs
      deviceVector dcuValues ;
      unsigned int error = DD_RETURN_OK ;

      std::list<FecExceptionHandler *> errorList ;
      error = fecAccessManager_->uploadValuesMultipleFrames ( dcuValues, errorList ) ;
      // Display and send the error to diagnostic system
      manageErrorList ( *errorReportLogger_, "Error during upload of all the DCUs (i2o command = DCU_GETALLVALUES)", errorList ) ;

      int countMaxDcu = dcuValues.size(), counterDcu = 0 ;
      // Maximum size of the buffer is 256kb
      if ((countMaxDcu*sizeof(dcuDescription)) > MAXI2OMESSAGESIZE) countMaxDcu = (int)(MAXI2OMESSAGESIZE / sizeof(dcuDescription)) ;

      ackRef = allocateTrackerCommand((int)((countMaxDcu*sizeof(dcuDescription))/sizeof(int)) + 2) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->data[0] = countMaxDcu ;
      dcuDescription *dcu = (dcuDescription *)(&reply->data[1]) ;

      for (deviceVector::iterator itDcuValues = dcuValues.begin() ; (itDcuValues != dcuValues.end()) && (counterDcu < countMaxDcu) ; itDcuValues ++, counterDcu++) {
	memcpy(&dcu[counterDcu], *itDcuValues, sizeof(dcuDescription)) ;
      }

      if (error)
	reply->Error = -error ;
      else
	reply->Error = DD_RETURN_OK ;	

      break ;
    }

    // -------------------------------------------------------------------------------------------------------------
    // Detect all the DCUs and sent it back
    case DCU_DETECTDCUS: { // return the upload values

      std::list<keyType> *fecList = NULL ;
      try {
	fecAccessManager_->removeDevices () ;
	fecAccess_->fecRingResetFSM() ;
	fecAccess_->fecRingReset() ;
	fecList = fecAccess_->getFecList() ;
	if (fecList != NULL) {
	  for (std::list<keyType>::iterator p=fecList->begin();p!=fecList->end();p++) {
	    try {
	      keyType index = *p ;
	      errorReportLogger_->errorReport("Ring " + toString(getRingKey(index)) + ": 0x" + toHexString(fecAccess_->getFecRingSR0(index)), LOGDEBUG) ;
	    }
	    catch (FecExceptionHandler &e) {
	      
	    }
	  }
	}
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger_->errorReport ("Problem during the reset of the rings", e, LOGWARNING) ;
      }
      if (fecList != NULL) delete fecList ;

      // Make a PIA reset
      piaResetVector piaV ; std::list<std::string> listError ;
      FecDetectionUpload::getCratePiaReset (piaV,listError,*fecAccess_) ;
      std::list<FecExceptionHandler *> errorList ;
      if (piaV.size()) {
	if (multiFrames_) fecAccessManager_->resetPiaModulesMultipleFrames(&piaV,errorList) ;
	else fecAccessManager_->resetPiaModules(&piaV,errorList) ;
	manageErrorList(*errorReportLogger_, "Error during on automatic detection in PIA reset", errorList ) ;
	FecFactory::deleteVectorI(piaV) ;
      }
      else {
	std::stringstream msgInfo ; msgInfo << "No PIA reset found on the FEC" ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGDEBUG) ;
      }

      // Scan for DCU
      std::list<keyType> *dcuList = NULL ;
      keyType deviceValues[2] = {DCUADDRESS, NORMALMODE} ; // DCU
      try {
	dcuList = fecAccess_->scanRingForI2CDevice ( deviceValues, 1, false, displayDebugMessage_ ) ;
      }
      catch (FecExceptionHandler &e) {
	std::stringstream msgError ; msgError << "Problem during the detection of DCU" ;
	errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
      }

      // Readout the DCU hard ID
      deviceVector nDevice, uDevice ;
      if ( (dcuList != NULL) && (dcuList->size()) ) {
	for (std::list<keyType>::iterator p=dcuList->begin();p!=dcuList->end();p++) {
	  nDevice.push_back(new dcuDescription (*p, 0,0,0,0,0,0,0,0,0,0)) ;
	}
	delete dcuList ;

	if (nDevice.size()) {
	  // Declare the DCUs
	  unsigned int error = 0 ;
	  std::list<FecExceptionHandler *> errorList ;

	  if (multiFrames_) error = fecAccessManager_->downloadValuesMultipleFrames (&nDevice, errorList) ;
	  else error = fecAccessManager_->downloadValues (&nDevice, errorList) ;
	  
	  // Upload only the DCU hard
	  if (multiFrames_) fecAccessManager_->uploadValuesMultipleFrames ( uDevice, errorList, true ) ;
	  else fecAccessManager_->uploadValues ( uDevice, errorList, true ) ;

	  // Check if errors has been reported
	  manageErrorList ( *errorReportLogger_, "Error during the connection of the DCUs",  errorList) ;
	  error += errorList.size() ;
	}
	else {
	  errorReportLogger_->errorReport ("No DCU found on the i2o message DCU_DETECTDCUS", LOGWARNING) ;
	}
      }

      // Send it back
      int countMaxDcu = uDevice.size(), counterDcu = 0 ;
      // Maximum size of the buffer is 256kb
      if ((countMaxDcu*sizeof(dcuDescription)) > MAXI2OMESSAGESIZE) countMaxDcu = (int)(MAXI2OMESSAGESIZE / sizeof(dcuDescription)) ;
      ackRef = allocateTrackerCommand((int)(uDevice.size()*sizeof(dcuDescription)/sizeof(int)) + 2) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->data[0] = countMaxDcu ;

      //std::cout << "FecSupervisor instance " << getApplicationDescriptor()->getInstance() << ": Sending " << reply->data[0] << " DCUs to DcuFilter (" << (((int)(uDevice.size()*sizeof(dcuDescription)/sizeof(int)) + 2)*sizeof(int))  << " bytes, 1 DCU description is " << sizeof(dcuDescription) << " bytes)" << std::endl ;

      dcuDescription *dcu = (dcuDescription *)(&reply->data[1]) ;
      for (deviceVector::iterator itDcuValues = uDevice.begin() ; (itDcuValues != uDevice.end()) && (counterDcu < countMaxDcu) ; itDcuValues ++, counterDcu++) {

	dcuDescription *dcuI = dynamic_cast<dcuDescription *>(*itDcuValues) ;
	memcpy(&dcu[counterDcu], dcuI, sizeof(dcuDescription)) ;
      }

      // Delete the vector of DCUs
      if (nDevice.size() > 0) {
	FecFactory::deleteVectorI (nDevice) ;
	nDevice.clear() ;
      }
      if (uDevice.size() > 0) {
	FecFactory::deleteVectorI (uDevice) ;
	uDevice.clear() ;
      }

      reply->Error = DD_RETURN_OK ;	
      break ;
    }

    // ------------------------------------------------------------------------------------
    // ---------------------------------- for DCU and device readout suspend and resume

    case I2O_SUSPENDDEVICEREADOUT: { // Suspend the device readout before commissioning
      if (deviceThreadUsed_) {
	if (com->data[0] >= 0) deviceWorkLoopTime_ = com->data[0] ;
	errorReportLogger_->errorReport ("Suspend the device readout due to commissioning procedure", LOGINFO) ;
	suspendDeviceWorkLoop_ = true ;
      }
      ackRef = allocateTrackerCommand(sizeof(unsigned int)/sizeof(int)+1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = DD_RETURN_OK ;
      unsigned int val = deviceWorkLoopTime_ ;
      memcpy (reply->data, &val, sizeof(unsigned int)) ;
      break ;
    }
    case I2O_RESUMEDEVICEREADOUT: { // Resume the device readout after commissioning
      if (deviceThreadUsed_) {
	if (com->data[0] >= 0) deviceWorkLoopTime_ = com->data[0] ;
	errorReportLogger_->errorReport ("Resume the device readout due to commissioning procedure", LOGINFO) ;
	suspendDeviceWorkLoop_ = false ;
	manageDeviceWorkLoop() ;
      }
      ackRef = allocateTrackerCommand(sizeof(unsigned int)/sizeof(int)+1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = DD_RETURN_OK ;
      unsigned int val = deviceWorkLoopTime_ ;
      memcpy (reply->data, &val, sizeof(unsigned int)) ;
      break ;
    }
    case I2O_SUSPENDDCUREADOUT: { // Suspend the DCU readout
      if (dcuThreadUsed_) {
	if (com->data[0] >= 0) dcuWorkLoopTime_ = com->data[0] ;
	errorReportLogger_->errorReport ("Suspend the DCU readout due to commissioning procedure", LOGINFO) ;
	suspendDcuWorkLoop_ = true ;
      }
      ackRef = allocateTrackerCommand(sizeof(unsigned int)/sizeof(int)+1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = DD_RETURN_OK ;
      unsigned int val = dcuWorkLoopTime_ ;
      memcpy (reply->data, &val, sizeof(unsigned int)) ;
      break ;
    }
    case I2O_RESUMEDCUREADOUT: { // Resume the DCU readout
      if (dcuThreadUsed_) {
	if (com->data[0] >= 0) dcuWorkLoopTime_ = com->data[0] ;
	errorReportLogger_->errorReport ("Resume the DCU readout due to commissioning procedure", LOGINFO) ;
	suspendDcuWorkLoop_ = false ;
	manageDcuWorkLoop() ;
      }
      ackRef = allocateTrackerCommand(sizeof(unsigned int)/sizeof(int)+1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = DD_RETURN_OK ;
      unsigned int val = dcuWorkLoopTime_ ;
      memcpy (reply->data, &val, sizeof(unsigned int)) ;
      break ;
    }

    default: {

      // Error in the reply
      ackRef = allocateTrackerCommand(1) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = -1 ;

      // Message not known
      std::stringstream msgError ; msgError << "Receive an unknow command " << com->Command << " from system " << com->System << "and sub-system " << com->SubSystem ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;

      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: " << msgError.str()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }
    }
  }
  
  // Suspend the DCU work loop during the download in order to avoid Multiple access
    mutexTaker_ = 0;
    mutexThread_->give() ;

#ifdef DEBUGMSGERROR
  errorReportLogger_->errorReport ("semaphore released", LOGDEBUG) ;
#endif

  // Return the answer
  return ackRef;
}

/** \param FEC index
 * \param FEC temperature sensor 1
 * \param FEC temperature sensor 2
 */
void FecSupervisor::sendI2OFecTemperature ( xdaq::ApplicationDescriptor *dcuFilterDes, keyType indexFecTemp, int tempSensor1, int tempSensor2 ) {

  // Create a pointer to send the values
  int ptri[3] = {getFecKey(indexFecTemp), tempSensor1, tempSensor2} ;
  try {
    this->sendTrackerCommand(dcuFilterDes, 
			     SYST_TRACKER, SUB_SYST_FEC, 
			     VERS_FEC, FEC_TEMP_SEND, (S32 *)ptri, 3) ;
    
    // Check the anwser
    PI2O_TRACKER_COMMAND_MESSAGE_FRAME reply = this->getReply() ;
    int sw = reply->data[0] ;
    if (sw == DCUFILTER_OK) {

      // Logger for error or information
      std::stringstream msgDebug ; msgDebug << "Work loop: FEC temperatures sent to the " << dcuFilterDes->getClassName() << " instance " << std::dec << (int)dcuFilterDes->getInstance() ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
    }
    else {

      // Logger for error or information
      std::stringstream msgError ; msgError << "Work loop: error during the sending of the FEC temperatures to the " << dcuFilterDes->getClassName() << " instance " << std::dec << (int)dcuFilterDes->getInstance() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }
  catch (xdaq::exception::Exception& e) {
    
    // Logger for error or information
    errorReportLogger_->errorReport ("Unable to find a DCU filter during the FEC temperature send values operation", e, LOGERROR) ; 
  }
}

/** \param vDevice - vector of dcu descriptions
 * Even in case of error in the sending of message, the values continue to 
 * be sent until the method setDcuFilterEnable ( bool ) is called
 */
void FecSupervisor::sendI2ODcuValues( xdaq::ApplicationDescriptor *dcuFilterDes, deviceVector vDevice ) {

  if(! vDevice.empty() ) {

    // Convert it in an array
    int *ptri = new int[(vDevice.size()*sizeof(dcuDescription))/sizeof(int)+2] ;
    ptri[0] = vDevice.size() ;
    dcuDescription *dcu = (dcuDescription *)(&ptri[1]) ;
    int i = 0 ;
    for (deviceVector::iterator p=vDevice.begin() ; p != vDevice.end() ; p ++, i ++)
      memcpy (&dcu[i], *p, sizeof(dcuDescription)) ;

    //errorReportLogger_->errorReport ("Sending " + toString((vDevice.size()*sizeof(dcuDescription))) + " data through the i2o messages for " + toString(vDevice.size()) + " DCUs", LOGDEBUG) ;

    try {
      this->sendTrackerCommand(dcuFilterDes, 
			       SYST_TRACKER, SUB_SYST_FEC, 
			       VERS_FEC, DCU_VALUES_SEND, (S32 *)ptri,
			       (vDevice.size()*sizeof(dcuDescription))/sizeof(int) + 2) ;

      // Check the anwser
      PI2O_TRACKER_COMMAND_MESSAGE_FRAME reply = this->getReply() ;
      int sw = reply->data[0] ;
      if (sw == DCUFILTER_OK) {

	// Logger for error or information
	std::stringstream msgDebug ; msgDebug << "Work loop: DCU values sent to the " << dcuFilterDes->getClassName() << " instance " << std::dec << (int)dcuFilterDes->getInstance() ;
	errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      }
      else {

	// Logger for error or information
	std::stringstream msgError ; msgError << "Work loop: error during the sending of the DCU values to the " << dcuFilterDes->getClassName() << " instance " << std::dec << (int)dcuFilterDes->getInstance() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    catch (xdaq::exception::Exception& e) {

      // Logger for error or information
      errorReportLogger_->errorReport ("Unable to find a DCU filter during the DCU send values operation", e, LOGERROR) ; 
    }

    delete ptri ;
  }
}
 
/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Action performed                                                           */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Handle a soap message and return all parameters based on the following parameters:
 * <lu>
 * <li>StatusFECRing8: SR0 of the FEC ring 8
 * <li>StatusFECRing7: SR0 of the FEC ring 7
 * <li>StatusFECRing6: SR0 of the FEC ring 6
 * <li>StatusFECRing5: SR0 of the FEC ring 5
 * <li>StatusFECRing4: SR0 of the FEC ring 4
 * <li>StatusFECRing3: SR0 of the FEC ring 3
 * <li>StatusFECRing2: SR0 of the FEC ring 2
 * <li>StatusFECRing1: SR0 of the FEC ring 1
 * <li>StatusFECPCI: SR0 of the FEC PCI
 * <li>StatusFECUSB: SR0 of the FEC USB
 * <li>StatusQPLLError: QPLL error, QPLL not in error, error during read
 * <li>StatusQPLLLocked: QPLL locked, QPLL not locked, error during read
 * <li>StatusTTCRx: ready, not ready, error during read
 * <li>StatusSSID: SSID, error during read
 * <li>RingFirmwareVersion: firmware version, error during read
 * <li>VMEFirmwareVersion: firmware version, error during read
 * <li>TriggerFirmwareVersion: firmware version, error during read
 * <li>FecTemperature1: FEC temperature, error during read
 * <li>FecTemperature2: FEC temperature, error during read
 * <li>FecHardwareId: return the FEC hardware ID (if it is set)
 * </lu>
 */
xoap::MessageReference FecSupervisor::soapStatusParameters (xoap::MessageReference msg) throw (xoap::exception::Exception) {

  std::string errorDuringRead = "Error during read" ;

  xoap::SOAPPart part = msg->getSOAPPart();
  xoap::SOAPEnvelope env = part.getEnvelope();
  xoap::SOAPBody body = env.getBody();
  DOMNode* node = body.getDOMNode();
  DOMNodeList* bodyList = node->getChildNodes();
  
  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();

  // Check the the accesses was created
  if (fecAccess_ == NULL) {
    // Error reporting: sent to the 1st level of error diag
    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::statusParameters)", LOGFATAL, 0, XDAQFEC_HARDWAREACCESSERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    xoap::SOAPName responseName = envelope.createName("No hardware access created, cannot make any request on it", "xdaq", XDAQ_NS_URI);
    envelope.getBody().addBodyElement ( responseName );
  }
  else {
    refreshParameters() ;
    
    for (unsigned int i = 0; i < bodyList->getLength(); i++)   {
      DOMNode* command = bodyList->item(i);
      
      if (command->getNodeType() == DOMNode::ELEMENT_NODE) {
	std::string commandName = xoap::XMLCh2String (command->getLocalName());
	std::stringstream answerR ;

	if (commandName == "StatusFECRing8") {
	  if ((unsigned int)statusFECRing8_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECRing8_.toString() ;
	}
	else if (commandName == "StatusFECRing7") {
	  if ((unsigned int)statusFECRing7_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECRing7_.toString() ;
	}
	else if (commandName == "StatusFECRing6") {
	  if ((unsigned int)statusFECRing6_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECRing6_.toString() ;
	}
	else if (commandName == "StatusFECRing5") {
	  if ((unsigned int)statusFECRing5_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECRing5_.toString() ;
	}
	else if (commandName == "StatusFECRing4") {
	  if ((unsigned int)statusFECRing4_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECRing4_.toString() ;
	}
	else if (commandName == "StatusFECRing3") {
	  if ((unsigned int)statusFECRing3_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECRing3_.toString() ;
	}
	else if (commandName == "StatusFECRing2") {
	  if ((unsigned int)statusFECRing2_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECRing2_.toString() ;
	}
	else if (commandName == "StatusFECRing1") {
	  if ((unsigned int)statusFECRing1_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECRing1_.toString() ;
	}
	else if (commandName == "StatusFECPCI") {
	  if ((unsigned int)statusFECPCI_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECPCI_.toString() ;
	}
	else if (commandName == "StatusFECUSB") {
	  if ((unsigned int)statusFECUSB_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << statusFECUSB_.toString() ;
	}
	else if (commandName == "StatusQPLLError") {
	  switch ((unsigned int)statusQPLLError_){
	  case NONEVALUE: answerR << errorDuringRead ; break ;
	  case 1: answerR << "QPLL error" ; break ;
	  case 2: answerR << "QPLL not in error" ; break ;
	  }
	}
	else if (commandName == "StatusQPLLLocked") {
	  switch ((unsigned int)statusQPLLLocked_){
	  case NONEVALUE: answerR << errorDuringRead ; break ;
	  case 1: answerR << "QPLL locked" ; break ;
	  case 2: answerR << "QPLL not locked" ; break ;
	  }
	}
	else if (commandName == "StatusTTCRx") {
	  switch ((unsigned int)statusTTCRx_){
	  case NONEVALUE: answerR << errorDuringRead ; break ;
	  case 1: answerR << "ready" ; break ;
	  case 2: answerR << "not ready" ; break ;
	  }
	}
	else if (commandName == "StatusSSID") {
	  if (statusSSID_.toString() == "NONE") answerR << errorDuringRead ;
	  else answerR << statusSSID_.toString() ;
	}
	else if (commandName == "RingFirmwareVersion") {
	  if ((unsigned int)ringFirmwareVersion_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << ringFirmwareVersion_.toString() ;
	}
	else if (commandName == "VMEFirmwareVersion") {
	  if ((unsigned int)vmeFirmwareVersion_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << vmeFirmwareVersion_.toString() ;
	}
	else if (commandName == "TriggerFirmwareVersion") {
	  if ((unsigned int)triggerFirmwareVersion_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << triggerFirmwareVersion_.toString() ;
	}
	else if (commandName == "FecTemperature1") {
	  if ((unsigned int)fecTemperature1_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << fecTemperature1_.toString() ;
	}
	else if (commandName == "FecTemperature2") {
	  if ((unsigned int)fecTemperature2_ == NONEVALUE) answerR << errorDuringRead ;
	  else answerR << fecTemperature2_.toString() ;
	}
	else if (commandName == "FecHardwareId") {
	  if (fecHardwareId_ == "") answerR << errorDuringRead ;
	  else answerR << fecHardwareId_ ;
	}

	xoap::SOAPName responseName = envelope.createName( commandName + "Response" + answerR.str(), "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement ( responseName );	
      }
    }
  }

  return reply;
}

/** refresh all exported parameters for the FEC
 */
void FecSupervisor::refreshParameters ( ) {

  statusFECRing8_ = statusFECRing7_ = statusFECRing6_ = statusFECRing5_ = NONEVALUE ;
  statusFECRing4_ = statusFECRing3_ = statusFECRing2_ = statusFECRing1_ = NONEVALUE ;
  statusFECPCI_ = statusFECUSB_ = NONEVALUE ;
  statusQPLLError_ = statusQPLLLocked_ = statusTTCRx_ = NONEVALUE ;
  statusSSID_ = SSID_NONE;
  ringFirmwareVersion_ = vmeFirmwareVersion_ = triggerFirmwareVersion_ = NONEVALUE ;
  fecTemperature1_ = fecTemperature2_ = NONEVALUE ;

  // Check the the accesses was created
  if (fecAccess_ == NULL) {

    // Error reporting: sent to the 1st level of error diag
    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::refreshParameters)", LOGFATAL, 0, XDAQFEC_HARDWAREACCESSERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // ----------------------------------------------------------
  // Status of each of the FEC supervisor
  // FEC hardware ID, fec firmware, trigger firmware, VME firmware
  // QPLL Error ?
  // QPLL Lock ?
  // SSID ?
  // FEC status for each ring ?

  // Number of DCU readout (mean value per ring)
  // Number of devices manage by the FEC supervisor
  // Number of errors in last download
  // Number of errors during the last upload of the FEC firmware

  // -------------------------------------------------------------------------------------
  // FEC hardware ID and firmware versions
  std::string errorMsgSuite = "no error" ;
  try {
    if (fecAccess_->getFecBusType() == FECVME) {
      errorMsgSuite = "Unable to read the FEC hardware ID for the FEC on slot " ;
      fecHardwareId_ = fecAccess_->getFecHardwareId (buildFecRingKey(fecSlot_,ringMin_)) ;
      errorMsgSuite = "Unable to read the ring firmware version for the FEC on slot " ;
      ringFirmwareVersion_ = fecAccess_->getFecFirmwareVersion(buildFecRingKey(fecSlot_,ringMin_)) ;
      errorMsgSuite = "Unable to read the VME firmware version for the FEC on slot " ;
      vmeFirmwareVersion_ = fecAccess_->getVmeVersion(buildFecRingKey(fecSlot_,ringMin_)) ;
      errorMsgSuite = "Unable to read the trigger firmware version for the FEC on slot " ;
      triggerFirmwareVersion_ = fecAccess_->getTriggerVersion(buildFecRingKey(fecSlot_,ringMin_)) ;
      errorMsgSuite = "Unable to read the FEC temperatures version for the FEC on slot " ;
      int fecTemp0, fecTemp1 ;
      fecAccessManager_->getFecAccess()->getFecTemperature(buildFecRingKey(fecSlot_,0),fecTemp0,fecTemp1) ;
      fecTemperature1_ = fecTemp0 ;
      fecTemperature2_ = fecTemp1 ;
    }
  }					
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport (errorMsgSuite + fecSlot_.toString(), e, LOGFATAL) ;
    
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << errorMsgSuite 
				    << fecSlot_.toString() 
				    << std::endl << e.what()
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }
    
  // ------------------------------------------------------------------------------------
  // read all status registers from FEC
  switch ( fecAccess_->getFecBusType() ) {
  case FECPCI: {
#if defined(BUSPCIFEC)
    statusFECPCI_ = fecAccess_->getFecRingSR0(buildFecRingKey(fecSlot_,ringMin_)) ;
#endif
    break ;
  }
  case FECVME: {
    for (unsigned int ring = FecVmeRingDevice::getMinVmeFecRingValue() ; ring <= FecVmeRingDevice::getMaxVmeFecRingValue() ; ring ++) {
      try {
	unsigned long value = fecAccess_->getFecRingSR0(buildFecRingKey(fecSlot_,ring)) ;
	switch (ring) {
	case 1: statusFECRing1_ = value ; break ;
	case 2: statusFECRing2_ = value ; break ;
	case 3: statusFECRing3_ = value ; break ;
	case 4: statusFECRing4_ = value ; break ;
	case 5: statusFECRing5_ = value ; break ;
	case 6: statusFECRing6_ = value ; break ;
	case 7: statusFECRing7_ = value ; break ;
	case 8: statusFECRing8_ = value ; break ;
	} 
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger_->errorReport ("Unable to read the status register 0 on FEC " + fecSlot_.toString() + " ring " + toString(ring), e, LOGINFO) ; // mersi patch
	
	if (displayFrameErrorMessages_) { 
	  if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	  std::stringstream temp ; temp << "Unable to read the status register 0 on FEC "
					<< fecSlot_.toString() 
					<< " ring " << ring
					<< std::endl << e.what()
					<< std::endl << textInformation_.str() ;
	  textInformation_.str(temp.str()) ;
	}
      }
    }
    break ;
  }
  case FECUSB: {
#if defined(BUSUSBFEC)
    statusFECUSB_ = fecAccess_->getFecRingSR0(buildFecRingKey(fecSlot_,ringMin_)) ;
#endif
    break ;
  }
  }

  // ----------------------------------------------------------
  // trigger status
  if (fecAccess_->getFecBusType() == FECVME) {
    try {
      statusQPLLError_ = statusQPLLLocked_ = statusTTCRx_ = NONEVALUE ;
      unsigned int triggerStatus0Value = fecAccess_->getCCSTriggerStatus0(buildFecRingKey(fecSlot_,ringMin_)) ;
      
      std::stringstream msgUserInfo ;
      if (triggerStatus0Value & CCS_SR0_QPLL_ERROR) { msgUserInfo << "QLL Error" ; statusQPLLError_ = 1 ; }
      else { msgUserInfo << "QPLL ok" ; statusQPLLError_ = 0 ; }
      if (triggerStatus0Value & CCS_SR0_QPLL_LOCKED) { msgUserInfo << " / QPLL locked" ; statusQPLLLocked_ = 1 ; }
      else { msgUserInfo << " / QPLL not locked" ; statusQPLLLocked_ = 0 ; }
      if (triggerStatus0Value & CCS_SR0_TTCRX_READY) { msgUserInfo << " / TTCRx ready" ; statusTTCRx_ = 1 ; }
      else { msgUserInfo << " / TTCRx not ready" ; statusTTCRx_ = 0 ; }
      msgUserInfo << " on FEC slot " << std::dec << fecSlot_.toString() ;
      errorReportLogger_->errorReport (msgUserInfo.str(), LOGUSERINFO) ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("Unable to read the FEC CCS trigger status on slot " + fecSlot_.toString(), e, LOGFATAL) ;
      
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: Unable to read FEC VME firmware version on slot "
				      << fecSlot_.toString() 
				      << std::endl << e.what()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }
  }

  // ----------------------------------------------------------
  // Get the SSID
  if (fecAccess_->getFecBusType() == FECVME) { 
    try {
      statusSSID_ = fecAccess_->getSSID(buildFecRingKey(fecSlot_,ringMin_)) ;
      std::stringstream msgInfo ; msgInfo << "SSID " << statusSSID_.toString() << " set for the FEC " << fecSlot_.toString() ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }
    catch (FecExceptionHandler &e) {
      std::stringstream msgError ; msgError << "Unable to get the SSID for FEC " << fecSlot_.toString() ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
      
      if (displayFrameErrorMessages_) { 
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: Unable to read SSID on slot "
				      << fecSlot_.toString() 
				      << std::endl << e.what()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;
      }
    }
  }
}


/** when an action performed method is set
 */
void FecSupervisor::actionPerformed(xdata::Event& event) {

  refreshParameters ( ) ;
}

//
// SOAP Callback for FSM
//
xoap::MessageReference FecSupervisor::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception) {

  //errorReportLogger_->errorReport("fireEvent", LOGDEBUG) ;

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
	fsm_.fireEvent(commandName,this) ;
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

void FecSupervisor::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  
  //errorReportLogger_->errorReport("Default page", LOGDEBUG) ;

  if (fsm_.getCurrentState() == 'I' ) {
    this->displayInitialisePage(in,out);
  }
  else if ( fsm_.getCurrentState() == 'F' ) {
    displayFailurePage ( in, out ) ;
  }
  else {

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
      displayHardwareCheck(in,out) ;
      break ;
#ifdef TKDIAG
    case 4:
      displayConfigureDiagSystem(in,out) ;
      break ;
#endif
    default:
      displayRelatedLinks (in,out,true) ;
      break ;
    }
  }
}

// ------------------------------------------------------------------------------------------------------------ //
// Initialise Web Page intialisation
// ------------------------------------------------------------------------------------------------------------ //

/** Initialise page
 */
void FecSupervisor::displayInitialisePage(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  errorReportLogger_->errorReport("displayInitialisePage", LOGDEBUG) ;

#ifdef DATABASE
  if ( (dbLogin_.toString() != "nil") && (dbPasswd_.toString() != "nil") && (dbPath_.toString() != "nil") )
    errorReportLogger_->errorReport("Database = " + dbLogin_.toString() + "/" + dbPasswd_.toString() + "@" + dbPath_.toString(), LOGDEBUG) ;
  else
    errorReportLogger_->errorReport("No database for the time being", LOGDEBUG) ;
#else
  errorReportLogger_->errorReport("Database not compiled", LOGDEBUG) ;
#endif
  
  std::stringstream msgInfo1 ; msgInfo1 << "FEC bus type: " << fecBusType_.toString() 
					 << " (" << fecBusTypeList[FECVME] << ", " 
					 << fecBusTypeList[FECPCI] << ", " 
					 << fecBusTypeList[FECUSB] << ") " ;
  if (fecBusType_.toString() == fecBusTypeList[FECVME]) {
    msgInfo1 << " on " << strBusAdapter_.toString() 
	     << " (" << strBusAdapterList[CAENPCI] << ", " 
	     << strBusAdapterList[CAENUSB] << ", "
	     << strBusAdapterList[SBS] << "): " ;

    if (vmeFileNamePnP_ == "") 
      msgInfo1 << "Geographical address used: VME file name " << vmeFileName_.toString() << std::endl ;
    else
      msgInfo1 << "Plug and play used: plug and play file " << vmeFileNamePnP_.toString() << " and FEC Id file " << vmeFileName_.toString() ;
  }
  errorReportLogger_->errorReport(msgInfo1.str(), LOGDEBUG) ;

  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("FecSupervisor Initialisation") << std::endl;
  xgi::Utils::getPageHeader(*out, "FecSupervisor Initialisation");
	
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();

  url += "/supervisorInitialise";	

  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  *out << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", INITIALISE);

  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  // FEC type
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("FEC type") << std::endl;
  *out << cgicc::p() << std::endl ;

  if (fecBusType_.toString() == fecBusTypeList[FECPCI]) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECPCI])
      .set("selected")
      .set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECPCI])
      .set("selected")<< std::endl;
  }
  *out << cgicc::label("Optical PCI FEC") << std::endl;

  if (fecBusType_.toString() == fecBusTypeList[FECUSB]) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECUSB])
      .set("selected") 
      .set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECUSB])
      .set("selected") << std::endl;
  }
  *out << cgicc::label(" USB FEC") << std::endl ;

  // VME FEC over SBS bus adapter
  if (fecBusType_.toString() == fecBusTypeList[FECVME] &&
      strBusAdapter_.toString() == strBusAdapterList[SBS]) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECVME]+"_"+strBusAdapterList[SBS])
      .set("selected").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECVME]+"_"+strBusAdapterList[SBS])
      .set("selected") << std::endl;
  }
  *out << cgicc::label(" VME FEC over SBS ") << std::endl;

  // VME FEC over CAEN PCI bus adapter
  if (fecBusType_.toString() == fecBusTypeList[FECVME] &&
      strBusAdapter_.toString() == strBusAdapterList[CAENPCI]) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECVME]+"_"+strBusAdapterList[CAENPCI])
      .set("selected").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECVME]+"_"+strBusAdapterList[CAENPCI])
      .set("selected") << std::endl;
  }
  *out << cgicc::label(" VME FEC over CAEN PCI") << std::endl;

  // VME FEC over CAEN USB bus adapter
  if (fecBusType_.toString() == fecBusTypeList[FECVME] &&
      strBusAdapter_.toString() == strBusAdapterList[CAENUSB]) {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECVME]+"_"+strBusAdapterList[CAENUSB])
      .set("selected").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","fecBusType")
      .set("size","30")
      .set("value",fecBusTypeList[FECVME]+"_"+strBusAdapterList[CAENUSB])
      .set("selected") << std::endl;
  }
  *out << cgicc::label(" VME FEC over USB ") << std::endl;

  *out << cgicc::p() << cgicc::label(" VME FEC Configuration: ") << std::endl;
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Plug and play used") << std::endl;
  *out << "</td>";
  *out << "<td>";
  if (vmeFileNamePnP_ != "") 
    *out << cgicc::input().set("type", "checkbox")
      .set("name","plugnplayUsed").set("checked","true") << std::endl;
  else
    *out << cgicc::input().set("type", "checkbox")
      .set("name","plugnplayUsed") << std::endl;
  *out << "</td>";
  *out << "</tr>";
  
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::p() << cgicc::label("   File name configuration ");
  *out << "</td>";
  *out << "<td>";
  *out << cgicc::input().set("type","text")
    .set("name","vmeFileName")
    .set("size","80")
    .set("value",vmeFileName_) << std::endl;
  *out << "</td>";
  *out << "</tr>";

  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::p() << cgicc::label("   Plug and play configuration ");
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (vmeFileNamePnP_ != "") 
    *out << cgicc::input().set("type","text")
      .set("name","vmeFileNamePnP")
      .set("size","80")
      .set("value",vmeFileNamePnP_) << std::endl;
  else
    *out << cgicc::input().set("type","text")
      .set("name","vmeFileNamePnP")
      .set("size","80")
      .set("value",NOPLUGNPLAYMESSAGE) << std::endl;   
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;

  // End of the field
  *out << cgicc::fieldset() << std::endl;

  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  // Configuration parameter
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Configuration Parameters") << std::endl;
  *out << cgicc::p() << std::endl ;

  // Size of all the next fields
  std::string sizeField = "15" ;

  if (fecBusType_ == fecBusTypeList[FECVME]) {

    // FEC VME Information
    *out << "VME FEC Information:" << std::endl ;
    
    *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
    // FEC slot
    *out << "<tr>" << std::endl;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    // *out << cgicc::label("FEC hardware ID ") << std::endl;
    *out << cgicc::label("FEC slot") << std::endl;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << "&nbsp;<select name=\"fecSlot\">";
    for (unsigned int i = 0 ; i < MAX_NUMBER_OF_SLOTS ; i ++) { 
      if ((xdata::UnsignedLongT)fecSlot_ == i) 
	*out << "<option selected>" << toString(i) << "</option>";
      else
	*out << "<option>" << toString(i) << "</option>";
    }
    // *out << cgicc::input().set("type","text")
    // .set("name","fecHardwareId")
    // .set("size",sizeField)
    // .set("value",fecHardwareId_) << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;

    // SSID
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Please select the SSID") << std::endl;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << "&nbsp;<select name=\"SSID\">";
    bool selected = false ;
    for (unsigned int i = 0 ; i < 8 ; i ++) { 
      if (SSID_ == CCSTrigger::SubSystems[i]) {
	*out << "<option selected>" << CCSTrigger::SubSystems[i] << "</option>";
	selected = true ;
      }
      else
	*out << "<option>" << CCSTrigger::SubSystems[i] << "</option>";
    }
    if (!selected) *out << "<option selected>" << SSID_NONE << "</option>";
    else *out << "<option>" << SSID_NONE << "</option>";
    *out << "</select>" << std::endl;
    *out << "</tr>" << std::endl;

    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Crate Reset") << std::endl;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    //     if (crateReset_) {
    //       *out << cgicc::p() << cgicc::input().set("type", "checkbox")
    // 	.set("name","crateReset").set("checked","true") << std::endl;
    //     }
    //     else {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","crateReset") << std::endl;
    //    }
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;

    // Reload of the firmware
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Reload Firmware") << std::endl;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    if (reloadFirmware_) {
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
	.set("name","reloadFirmware").set("checked","true") << std::endl;
    }
    else {
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
	.set("name","reloadFirmware") << std::endl;
    }
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
   
    // Multi-frames
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Multi-frames algorithm") << std::endl;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    if (multiFrames_) {
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
 	.set("name","multiFrames").set("checked","true") << std::endl;
    }
    else {
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
	.set("name","multiFrames") << std::endl;
    }
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;

    // Block transfer
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("VME Block Transfer") << std::endl;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    if (blockMode_) {
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
 	.set("name","blockMode").set("checked","true") << std::endl;
    }
    else {
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
	.set("name","blockMode") << std::endl;
    }
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;

    // Apply redundancy
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Apply Redundancy") << std::endl;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    if (applyRedundancy_)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
	.set("name","applyRedundancy").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
	.set("name","applyRedundancy") << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;

    // Force the Apply redundancy
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Force the redundancy reconfiguration") << std::endl;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    if (forceApplyRedundancy_)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
	.set("name","forceApplyRedundancy").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox")
	.set("name","forceApplyRedundancy") << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;

    *out << cgicc::table() << std::endl;
  }

  // -------------------------------------------------------------
  // Work loop
  *out << cgicc::p() << "Work loop for DCU and device readout" << std::endl ;

  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  // DCU & device work Loop
  if (dcuThreadUsed_) {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","dcuThreadUsed").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","dcuThreadUsed") << std::endl;
  }
  *out << cgicc::label("DCU readout") << std::endl ;
  *out << "<td>" << "</tr>" << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (deviceThreadUsed_) {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","deviceThreadUsed").set("checked","true") << std::endl;
  }
  else {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","deviceThreadUsed") << std::endl;
  }
  *out << cgicc::label("Device readout") << std::endl ;
  *out << "<td>" << "</tr>" << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Work loop to be used")<< std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","workLoopName")
    .set("size",sizeField)
    .set("value",workLoopName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() ;

  // End of the field
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << std::endl ;

  // ----------------------------------------------------------
  // Add the parameters
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Download Parameters") << std::endl;
  *out << cgicc::p() << std::endl ;

#ifdef DATABASE
  // Database parameters
  // First check dyanmically if the values are set
  std::string dblogin = "nil", dbpass  = "nil", dbpath = "nil" ;
  DbFecAccess::getDbConfiguration (dblogin, dbpass, dbpath) ;

  // Database login ...
  if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
      (dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) databaseAccess_ = false ;
#else
  databaseAccess_ = false ;
#endif

  bool downloadedDefined = false ;
#if defined(TRACKERXDAQ)
  // Database access
  downloadedDefined = (shareMemoryName_ != DEFINENONE) ;
  if (shareMemoryName_ != DEFINENONE)
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","download")
      .set("size","30")
      .set("value","sharedmemory")
      .set("selected").set("checked","true") << std::endl;
  else 
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","download")
      .set("size","30")
      .set("value","sharedmemory")
      .set("selected") << std::endl;
  *out << cgicc::label("Database Caching System")  << std::endl;
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Shared memory name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","shareMemoryName")
    .set("size",sizeField)
    .set("value",shareMemoryName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() ;
#endif

#ifdef DATABASE
  if (databaseAccess_ && !downloadedDefined)
    *out << cgicc::p() << cgicc::input().set("type", "radio")
      .set("name","download")
      .set("size","30")
      .set("value","database")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "radio")
      .set("name","download")
      .set("size","30")
      .set("value","database")
      .set("selected") << std::endl;
  *out << cgicc::label("Database Used")  << std::endl;

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
  //   *out << "<tr>" << std::endl;
  //   *out << "<td>"; *out << "</td>" << std::endl;
  //   *out << "<td>"; 
  //   *out << cgicc::label("FEC hardware ID ") << std::endl;
  //   *out << "</td>" << std::endl;
  //   *out << "<td>"; 
  //   *out << cgicc::input().set("type","text")
  //     .set("name","fecHardwareId")
  //     .set("size",sizeField)
  //     .set("value",fecHardwareId_) << std::endl;
  //   *out << "</td>" << std::endl;
  //   *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Partition Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 

  // Check if the env variable has been set to display the correct name 
  // please note if this value is set then nothing can be done to change it
  // Fix the partition name
  if ( (partitionName_ == "") || (partitionName_ == DEFINENONE) ) {
    char *basic=getenv ("ENV_CMS_TK_PARTITION") ;
    if (basic != NULL) partitionName_ = std::string(basic) ;
  }

  if ( (partitionName_ == "") || (partitionName_ == DEFINENONE) ) {
    *out << cgicc::input().set("type","text")
      .set("name","partitionName")
      .set("size",sizeField)
      .set("readonly","readonly")
      .set("value",partitionName_.toString()) << std::endl;
  }
  else {
    *out << cgicc::input().set("type","text")
      .set("name","partitionName")
      .set("size",sizeField)
      .set("value",partitionName_.toString()) << std::endl;
  }

  
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;
#endif

  // File download
  if (!downloadedDefined && !databaseAccess_) 
    *out << cgicc::p() << cgicc::input().set("type", "radio")
      .set("name","download")
      .set("size","30")
      .set("value","file")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "radio")
      .set("name","download")
      .set("size","30")
      .set("value","file")
      .set("selected") << std::endl;
  *out << cgicc::label("File download")  << std::endl;

  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("File name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","xmlFileName")
    .set("size","80")
    .set("value",xmlFileName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;

  // End of the field
  *out << cgicc::fieldset() << std::endl;

  *out << cgicc::p() << std::endl ;

  *out << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", INITIALISE);

  // End of the form
  *out << cgicc::form() << std::endl;

  //#ifdef TKDIAG
  // Configuration of the diagnostic system
  //DIAG_SET_CONFIG_BUTTON_CALLBACK();
  //#endif
	
  //xgi::Utils::getPageFooter(*out);
  *out << "</HTML>" << std::endl ;
}


/** Initialise action
 */
void FecSupervisor::supervisorInitialise(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  errorReportLogger_->errorReport("supervisorInitialise", LOGDEBUG) ;

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // FEC type
    fecBusType_ = cgi["fecBusType"]->getValue() ;

    if (fecBusType_.toString().substr(0,fecBusTypeList[FECVME].size()) == fecBusTypeList[FECVME]) {

      strBusAdapter_ = fecBusType_.toString().substr(fecBusTypeList[FECVME].size()+1,fecBusType_.toString().size()) ;
      fecBusType_ = fecBusTypeList[FECVME] ;

      std::stringstream msgDebug ; msgDebug << "FEC type: " << fecBusType_.toString() << " over " << strBusAdapter_.toString() ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
    }
    else {

      std::stringstream msgDebug ; msgDebug << "FEC type: " << fecBusType_.toString() << " over " << strBusAdapter_.toString() ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
    }

    // VME File name
    vmeFileName_ = cgi["vmeFileName"]->getValue();
    vmeFileNamePnP_ = cgi["vmeFileNamePnP"]->getValue();

    std::stringstream msgDebug ; 
    if (vmeFileNamePnP_ == NOPLUGNPLAYMESSAGE) {
      msgDebug << "Geographical address used: VME file name: " << vmeFileName_.toString() ;
      vmeFileNamePnP_ = "" ;
    }
    else {
      msgDebug << "Plug and play used: plug and play file " << vmeFileNamePnP_.toString() << " and FEC Id file " << vmeFileName_.toString() ;
    }
    errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;

#if defined(TRACKERXDAQ)
    // -------------------------------------------------
    // Download through the database client
    if (cgi["download"]->getValue() == "sharedmemory") {
      shareMemoryName_ = cgi["shareMemoryName"]->getValue() ;
      std::stringstream msgDebug ; msgDebug << "Using the database caching system with the shared memory " << shareMemoryName_ .toString() ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
    }
    else shareMemoryName_ = DEFINENONE ;
#endif
#ifdef DATABASE
    // -------------------------------------------------
    // Download by database
    // Keep the database access if it is set in case of shared memory problem
    // databaseAccess_ = (cgi["download"]->getValue() == "database") ;

    // Database access
    dbLogin_ = cgi["dbLogin"]->getValue();
    dbPasswd_ = cgi["dbPasswd"]->getValue();
    dbPath_ = cgi["dbPath"]->getValue();

    if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
	(dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) databaseAccess_ = false ;

#  ifdef TRACKERXDAQ
    if (shareMemoryName_ == DEFINENONE)
#  endif
      if (databaseAccess_) {
	std::stringstream msgDebug ; msgDebug << "Database access declared: " << dbLogin_.toString() << "/" << dbPasswd_.toString() << "@" << dbPath_.toString() ;
	errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      }
    
    // Partition name
    partitionName_ = cgi["partitionName"]->getValue() ;
#endif
    // File download
    //if (cgi["download"]->getValue() == "file")
    xmlFileName_ = cgi["xmlFileName"]->getValue() ;

    // FEC slot
    std::string fecSlotStr = cgi["fecSlot"]->getValue() ;
    fecSlot_ = fromString<unsigned long>(fecSlotStr) ;

    // SSID
    SSID_ = cgi["SSID"]->getValue() ;

    // DCU & readout
    dcuThreadUsed_ = cgi.queryCheckbox("dcuThreadUsed");
    deviceThreadUsed_ = cgi.queryCheckbox("deviceThreadUsed");

    if (dcuThreadUsed_ || deviceThreadUsed_) {
      workLoopName_ = cgi["workLoopName"]->getValue() ;
//       if (workLoopName_ != DEFINENONE)
// 	errorReportLogger_->errorReport ("Start the DCU & device work loop", LOGDEBUG) ;
//       else 
// 	errorReportLogger_->errorReport ("Do not use a DCu & device work loop", LOGDEBUG) ;
    }

    // Crate Reset
    //crateReset_ = cgi.queryCheckbox("crateReset");
    //if (crateReset_) 
    if (cgi.queryCheckbox("crateReset")) {
      errorReportLogger_->errorReport ("A crate reset will be issued", LOGWARNING) ;
      crateReset_ = true ;
    }

    if (cgi.queryCheckbox("reloadFirmware")) {
      errorReportLogger_->errorReport ("The firmware will reloaded", LOGWARNING) ;
      reloadFirmware_ = true ;
    }

    // Mutiframes algorithm
    if (cgi.queryCheckbox("multiFrames")) {
      errorReportLogger_->errorReport ("Use the multiframes algorithm", LOGINFO) ;
      multiFrames_ = true ;
    }

    // VME block transfer
    if (blockMode_ != cgi.queryCheckbox("blockMode")) {
      errorReportLogger_->errorReport ("Use the VME block transfer", LOGINFO) ;
      blockMode_ = cgi.queryCheckbox("blockMode") ;
      if (fecAccess_ != NULL) fecAccess_->setFifoAccessMode (blockMode_) ;
    }

    // Apply redundancy
    if (applyRedundancy_ != cgi.queryCheckbox("applyRedundancy")) {
      errorReportLogger_->errorReport ("Apply the redundancy", LOGINFO) ;
      applyRedundancy_ = cgi.queryCheckbox("applyRedundancy") ;
    }

    // Force the redundancy reconfiguration
    if (forceApplyRedundancy_ != cgi.queryCheckbox("forceApplyRedundancy")) {
      errorReportLogger_->errorReport ("Force the redundancy reconfiguration", LOGINFO) ;
      forceApplyRedundancy_ = cgi.queryCheckbox("forceApplyRedundancy") ;
    }
  }
  catch(const std::exception& e) {
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }
		
  try {
    fsm_.fireEvent(INITIALISE,this) ;
  }
  catch (toolbox::fsm::exception::Exception & e) {
    XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }
	
  this->Default(in,out);
}

// ------------------------------------------------------------------------------------------------------------ //
// State machine
// ------------------------------------------------------------------------------------------------------------ //

/** Display all the possible states
 */
void FecSupervisor::displayStateMachine(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  // Display the page
  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("FecSupervisor") << std::endl;
  xgi::Utils::getPageHeader(*out, "FecSupervisor State Machine");

  // Display the links
  displayRelatedLinks (in,out) ;
  linkPosition_ = 2 ;  

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/dispatch";

  // display FSM
  std::set<std::string> possibleInputs = fsm_.getInputs(fsm_.getCurrentState());
  std::set<std::string> allInputs = fsm_.getInputs();

  // Error: Check the the accesses was created
  if (fecAccessManager_ == NULL) {
    *out << FEC_RED << "Error during the creation of the FEC hardware access" << "</span>" ; 
  }
    
  //*out << cgicc::h3("FecSupervisor: Finite State Machine") << std::endl;
  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << cgicc::tr() << std::endl;
  *out << "<th>" << fsm_.getStateName(fsm_.getCurrentState()) << "</th>" << std::endl;
  *out << cgicc::tr() << std::endl;
  *out << cgicc::tr() << std::endl;
  std::set<std::string>::iterator i;
  bool allDisabled = true ;
  for ( i = allInputs.begin(); i != allInputs.end(); i++) {

    if ((*i) != INITIALISE && (*i) != DESTROY && (*i) != FAIL && ((*i).find("Done") == std::string::npos) ) {

      *out << cgicc::td(); 
      *out << cgicc::form().set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;
      
      if ( possibleInputs.find(*i) != possibleInputs.end() ) {
	*out << cgicc::input().set("type", "submit").set("name", "StateInput").set("value", (*i) );
	allDisabled = false ;
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

  if (allDisabled) *out << cgicc::h2("A thread for the state machine was started, please click on the link until you get the FSM") << std::endl;

  // ------------------------------------------------------------
  if (displayFrameErrorMessages_) { 

    *out << cgicc::h3("Information, error or warning") << std::endl;
    *out << "<textarea width=\"100%\" height=\"300px\" style=\"width:100%;height:300px\" name=\"code\" wrap=\"logical\" rows=\"12\" cols=\"42\">" << std::endl;
    size_t hsize=textInformation_.str().size();
    *out << "[INFO: Size of HTML-output stream: " <<hsize
	 <<", time: " <<getCurrentTime()<<"]" << std::endl<< std::endl;
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
    *out << "</textarea>" << std::endl;

    //   // Check if an error occurrs and display the related message
    //   if (errorOccurs_) { // && displayFrameErrorMessages_) {
    //     *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    //     *out << cgicc::h2(reasonOfFailure_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    //     *out << cgicc::p() << std::endl ;
    //     errorOccurs_ = false ;
    //   }
  }

  *out << "</HTML>" << std::endl ; 

  // --------------------------------------------
  // Thread started
  //if (allDisabled) {
  //sleep((unsigned int)2) ;
  //this->displayStateMachine(in,out) ;
  //}
}


/** Depend of which state was initiated
 * Possible state are: Initialise, Configure, Halt
 */
void FecSupervisor::dispatch(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{   
  try {

    // ------------------------------------------------------------
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // ------------------------------------------------------------
    // Fire the corresponding state
    cgicc::const_form_iterator stateInputElement = cgi.getElement("StateInput");
    std::string stateInput = (*stateInputElement).getValue();

    try {
      if (stateInput != INITIALISE) {
	fsm_.fireEvent(stateInput,this) ;
      }
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

// ------------------------------------------------------------------------------------------------------------ //
// Failure
// ------------------------------------------------------------------------------------------------------------ //

/** Failure page
 */
void FecSupervisor::displayFailurePage(xgi::Input * in, xgi::Output * out) {
  
  //errorReportLogger_->errorReport("displayFailurePage", LOGDEBUG) ;

  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("FecSupervisor Failure") << std::endl;
  xgi::Utils::getPageHeader(*out, "FecSupervisor Failure Page");

  // Display the links
  displayRelatedLinks (in,out) ;

  if (displayFrameErrorMessages_) { 
    *out << "<textarea width=\"100%\" height=\"300px\" style=\"width:100%;height:300px\" name=\"code\" wrap=\"logical\" rows=\"12\" cols=\"42\">" << std::endl;
    size_t hsize=textInformation_.str().size();
    *out << "[INFO: Size of HTML-output stream: " <<hsize
	 <<", time: " <<getCurrentTime()<<"]" << std::endl<< std::endl;
    *out << textInformation_.str();
    const size_t max = 30000;
    if (hsize > max){
      textInformation_.str("");
      *out << std::endl <<"[Size of HTML-output stream = " << textInformation_.str().size()
	   << " is bigger than max = " << max << " --> clearing stream.]" << std::endl;
    }
    *out << "</textarea>" << std::endl;
  }

  if (reasonOfFailure_ != "") {
    *out << cgicc::h2(reasonOfFailure_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
  }
  else {
    *out << cgicc::h2("Unknow reason").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
  }
  
  *out << "</HTML>" << std::endl ;
}

// ------------------------------------------------------------------------------------------------------------ //
// Related links
// ------------------------------------------------------------------------------------------------------------ //

/** Related links with more all parameters that can be changed
 */
void FecSupervisor::displayMoreParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayMoreParameters", LOGDEBUG) ;
  
  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("FecSupervisor display more more parameters") << std::endl;  

  xgi::Utils::getPageHeader(*out, "FecSupervisor: More Paramters to be configured");

  // Display the links
  displayRelatedLinks (in,out) ;
  linkPosition_ = 1 ;

  *out << cgicc::p() << "About the parameters: " << std::endl ;
  *out << cgicc::ul() ;
  *out << cgicc::li() << "PLL cold reset will be performed on the next configure" << cgicc::li() << std::endl ;
  if (fecBusType_ == fecBusTypeList[FECVME])
    *out << cgicc::li() << "SSID/CCS TTCRx reset can be changed at any time, only the corresponding FEC will be affected" << cgicc::li() << std::endl ;
  *out << cgicc::li() << "Any change on the i2c speed needs a FEC reset" << cgicc::li() << std::endl ;
  *out << cgicc::li() << "The change on the DCU work loop or the work loop will be done automatically after the submission" << cgicc::li() << std::endl ;
  *out << cgicc::li() << "The change of any display takes place automatically" << cgicc::li() << std::endl ;
  *out << cgicc::ul() ;
  *out << cgicc::hr() << cgicc::p() << std::endl ;

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/moreParameters";	

  // Form
  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  // Apply
  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply");
  *out << cgicc::p() << std::endl ;

  // ----------------------------------------------------------------------------
  std::ostringstream fecInfo ; 
  if (fecAccessManager_ == NULL) {
    fecInfo << FEC_RED << "Error during the creation of the FEC hardware access" << "</span>" ; 
  }
  else {
    fecInfo << "FEC hardware ID: " << FEC_GREEN << fecHardwareId_ << "</span>" << " in slot " << FEC_GREEN << fecSlot_.toString() << "</span>" ;
  }

  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Download Parameters") << std::endl;
  *out << cgicc::p() << std::endl ;
  *out << cgicc::p() << cgicc::label(fecInfo.str()) << std::endl ;

  bool alreadySet = false ;

#if defined(TRACKERXDAQ)
  if (shareMemoryName_ != DEFINENONE) {

    std::ostringstream sharedInfo ; sharedInfo << "Use the caching system with the shared memory " << FEC_GREEN << shareMemoryName_.toString() << "</span>" ;

    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","databaseDownload")
      .set("size","30")
      .set("value","shareMemoryName")
      .set("selected").set("checked","true") << std::endl;
    *out << cgicc::label(sharedInfo.str()) << std::endl ;

    alreadySet = true ;
  }
  else
#endif
#ifdef DATABASE
    if (databaseAccess_) {

      if (databaseDownload_) {

	// Retrieve the partition version
	getPartitionVersion() ;

	if( (versionMajorId_ == 0) && (versionMinorId_ == 0)) {

	  std::ostringstream versionInfo ; versionInfo << "Database download with the current version (" << FEC_RED << "no version found" << "</span>) and partition " ;
	  if (partitionName_ == DEFINENONE)
	    versionInfo << FEC_RED << partitionName_.toString() << "</span>" ;
	  else 
	    versionInfo << FEC_GREEN << partitionName_.toString() << "</span>" ;
	  // Database with current version
	  *out << cgicc::p() << cgicc::input().set("type","radio")
	    .set("name","databaseDownload")
	    .set("size","30")
	    .set("value","databaseDownloadCurrentVersion")
	    .set("selected").set("checked","true") << std::endl;
	  *out << cgicc::label(versionInfo.str()) << std::endl;

	  //	// Database with specific version
	  // 	*out << cgicc::p() << cgicc::input().set("type","radio")
	  // 	  .set("name","databaseDownload")
	  // 	  .set("size","30")
	  // 	  .set("value","databaseDownloadSpecificVersion")
	  // 	  .set("selected") << std::endl;
	  // 	*out << cgicc::label(" Database download with the version: ") << std::endl;
	  // 	*out << cgicc::input().set("type","text")
	  // 	  .set("name","versionId")
	  // 	  .set("size","10")
	  // 	  .set("value",toString(versionMajorId_) + "." + toString(versionMinorId_)) << std::endl;
	}
	else {
	  std::ostringstream versionInfo ; 
	  versionInfo << "Database download with the current version (version " 
		      << FEC_GREEN << toString(versionMajorId_) << "." << toString(versionMinorId_) 
		      << " (mask = " << toString(maskMajor_) << "." << toString(maskMinor_) << ")" 
		      << "</span>) and partition " << FEC_GREEN << partitionName_.toString() << "</span>" ;
	  // Database with current version
	  *out << cgicc::p() << cgicc::input().set("type","radio")
	    .set("name","databaseDownload")
	    .set("size","30")
	    .set("value","databaseDownloadCurrentVersion")
	    .set("selected").set("checked","true") << std::endl;
	  *out << cgicc::label(versionInfo.str()) << std::endl;
	
	  // 	// Database with specific version
	  // 	*out << cgicc::p() << cgicc::input().set("type","radio")
	  // 	  .set("name","databaseDownload")
	  // 	  .set("size","30")
	  // 	  .set("value","databaseDownloadSpecificVersion")
	  // 	  .set("selected").set("checked","true") << std::endl;
	  // 	*out << cgicc::label(" Database download with the version: ") << std::endl;
	  // 	*out << cgicc::input().set("type","text")
	  // 	  .set("name","versionId")
	  // 	  .set("size","10")
	  // 	  .set("value",toString(versionMajorId_) + "." + toString(versionMinorId_)).set("checked","true") << std::endl;
	}

	alreadySet = true ;
      }
      else {
      
	alreadySet = false ;
      
	std::ostringstream versionInfo ; versionInfo << "Database download with the current version" ;
	if( (versionMajorId_ == 0) && (versionMinorId_ == 0)) {
	  versionInfo << FEC_RED << " (no version found" << "</span>) and partition " ;
	  if ( (partitionName_ == DEFINENONE) || (partitionId_ == 0) )
	    versionInfo << FEC_RED << partitionName_.toString() << "</span>" ;
	  else 
	    versionInfo << FEC_GREEN << partitionName_.toString() << "</span>" ;
	}
	else {
	  versionInfo << " (version " << FEC_GREEN << toString(versionMajorId_) << "." << toString(versionMinorId_) 
		      << " (mask = " << toString(maskMajor_) << "." << toString(maskMinor_) << ")" 
		      << "</span>) and partition " << FEC_GREEN << partitionName_.toString() << "</span>" ;
	}
      
	// Database with current version
	*out << cgicc::p() << cgicc::input().set("type","radio")
	  .set("name","databaseDownload")
	  .set("size","30")
	  .set("value","databaseDownloadCurrentVersion")
	  .set("selected") << std::endl;
	*out << cgicc::label(versionInfo.str()) << std::endl;
      
	//       // Database with specific version
	//       *out << cgicc::p() << cgicc::input().set("type","radio")
	// 	.set("name","databaseDownload")
	// 	.set("size","30")
	// 	.set("value","databaseDownloadSpecificVersion")
	// 	.set("selected") << std::endl;
	//       *out << cgicc::label(" Database download with the version: ") << std::endl;
	//       *out << cgicc::input().set("type","text")
	// 	.set("name","versionId")
	// 	.set("size","10")
	// 	.set("value",toString(versionMajorId_) + "." + toString(versionMinorId_)).set("checked","true") << std::endl;      
      }
    }
#endif

  if (!alreadySet) {
    // XML file set
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","databaseDownload")
      .set("size","30")
      .set("value","xmlFileName")
      .set("selected").set("checked","true") << std::endl;
  }
  else {
    // XML file set
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","databaseDownload")
      .set("size","30")
      .set("value","xmlFileName")
      .set("selected") << std::endl;
  }
  
  // XML file name
  *out << cgicc::label(" XML file used: ") << std::endl;
  *out << cgicc::label("   File name ");
  *out << cgicc::input().set("type","text")
    .set("name","xmlFileName")
    .set("size","60")
    .set("value",xmlFileName_.toString()) << cgicc::p() << std::endl;
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << std::endl ;
  
  // Add the parameters
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Configuration Parameters") << std::endl;
  *out << cgicc::p() << std::endl ;
  
  // Reset parameters
  std::string hardReset ;
  if (fecBusType_ == fecBusTypeList[FECVME]) hardReset = "Crate reset" ;
  else if (fecBusType_ == fecBusTypeList[FECPCI]) hardReset = "PLX reset" ;
  else if (fecBusType_ == fecBusTypeList[FECUSB]) hardReset = "Not yet implemented" ;
  
  // FEC reset
  if (resetFec_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fecReset").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","fecReset") << std::endl;
  *out << cgicc::label("FEC reset") << std::endl;

  // Apply redundancy
  if (applyRedundancy_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","applyRedundancy").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","applyRedundancy") << std::endl;
  *out << cgicc::label("Apply redundancy on all rings") << std::endl;

  // Force the Apply redundancy
  if (forceApplyRedundancy_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","forceApplyRedundancy").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","forceApplyRedundancy") << std::endl;
  *out << cgicc::label("Force the redundancy reconfiguration") << std::endl;
  
  // PIA reset
  if (resetPia_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","piaReset").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","piaReset") << std::endl;
  *out << cgicc::label("PIA reset") << std::endl;

  // DOH configuration
  if (configureDOH_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","configureDOH").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","configureDOH") << std::endl;
  *out << cgicc::label("Configure the DOHs") << std::endl;  
  
  // Upload operation
  if (doUpload_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doUpload").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doUpload") << std::endl;
  *out << cgicc::label("Upload & Compare values") << std::endl;  
  
  // End of the field
  *out << cgicc::p() << std::endl;
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << std::endl ;
  
  // ----------------------------------------------------------------------------
  
  *out << cgicc::fieldset() << std::endl;
  if (fecBusType_ == fecBusTypeList[FECVME])
    *out << cgicc::p() << cgicc::legend("PLL, TTCRx and SSID") << std::endl;
  else
    *out << cgicc::p() << cgicc::legend("PLL and TTCRx") << std::endl;
  
  // Init TTCRx
  if (initTTCRx_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","initTTCRx").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","initTTCRx") << std::endl;
  *out << cgicc::label("Initialise TTCRx") << std::endl;
  
  // PLL cold reset
  if (coldPllReset_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","coldPllReset").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","coldPllReset") << std::endl;
  *out << cgicc::label("PLL cold reset (version " + pllAccess::getPllColdInitVersion() + ")") << std::endl;

  // SSID
  if (fecBusType_ == fecBusTypeList[FECVME]) {

    *out << cgicc::p() << cgicc::label("Please select the SSID") << std::endl;
    *out << "&nbsp;<select name=\"SSID\">";
    bool selected = false ;
    for (unsigned int i = 0 ; i < 8 ; i ++) { 
      if (SSID_ == CCSTrigger::SubSystems[i]) {
	*out << "<option selected>" << CCSTrigger::SubSystems[i] << "</option>";
	selected = true ;
      }
      else
	*out << "<option>" << CCSTrigger::SubSystems[i] << "</option>";
    }
    if (!selected) *out << "<option selected>" << SSID_NONE << "</option>";
    else *out << "<option>" << SSID_NONE << "</option>";
  }
  *out << "</select>" << std::endl;
  *out << cgicc::fieldset() << std::endl;

  *out << cgicc::p() << std::endl ;

  // I2C Speed
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << cgicc::legend("I2C Speed") << std::endl;
  if (i2cSpeed_ == 100)
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","i2cSpeed")
      .set("value","100")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","i2cSpeed")
      .set("value","100")
      .set("selected") << std::endl;
  *out << cgicc::label(" 100 Khz ") << std::endl;
  if (i2cSpeed_ == 200)
    *out << cgicc::input().set("type","radio")
      .set("name","i2cSpeed")
      .set("value","200")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::input().set("type","radio")
      .set("name","i2cSpeed")
      .set("value","200")
      .set("selected") << std::endl;
  *out << cgicc::label(" 200 Khz ") << std::endl;
  if (i2cSpeed_ == 400)
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","i2cSpeed")
      .set("value","400")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","i2cSpeed")
      .set("value","400")
      .set("selected") << std::endl;
  *out << cgicc::label(" 400 Khz ") << std::endl;
  if (i2cSpeed_ == 1000)
    *out << cgicc::input().set("type","radio")
      .set("name","i2cSpeed")
      .set("value","1000")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::input().set("type","radio")
      .set("name","i2cSpeed")
      .set("value","1000")
      .set("selected") << std::endl;
  *out << cgicc::label(" 1 Mhz ") << std::endl;
  // End of the field
  *out << cgicc::fieldset() << std::endl;

  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------
  // Device & DCU work loop
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << cgicc::legend("DCU and Device Work Loop") << std::endl;

  if ( (dcuThreadUsed_ || deviceThreadUsed_) && (workFecSupervisorLoop_ == NULL) ) 
    *out << cgicc::p() << cgicc::label("Please Configure/ConfigureDcu the FecSupervisor or click Apply to start it") << std::endl;

  *out << cgicc::p() << cgicc::label("Time between two DCU reads: ") << std::endl;
  *out << cgicc::input().set("type","text")
    .set("name","dcuWorkLoopTime")
    .set("size","10")
    .set("value",dcuWorkLoopTime_.toString()) << std::endl;

  // DCU work loop
  if (dcuThreadUsed_)
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","dcuThreadUsed")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","dcuThreadUsed")
      .set("selected") << std::endl;
  *out << cgicc::label("DCU Work Loop Used") << std::endl;

  // DCU work loop
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  if (!dcuThreadUsed_ || workFecSupervisorLoop_ == NULL) {
    *out << cgicc::tr() << std::endl ;
    *out << cgicc::td() << cgicc::td() << std::endl ;
    *out << cgicc::td() << std::endl ;
    *out << cgicc::p() << FEC_RED << cgicc::label("No DCU readout enabled") << "</span>" << std::endl;
    *out << cgicc::td() << cgicc::tr() << std::endl ;
  }
  else { // thread or work loop to readout DCU are exiting

    // Work loop
    if (workFecSupervisorLoop_ != NULL) {

      *out << cgicc::tr() << std::endl ;
      *out << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << std::endl ;
      *out << cgicc::p() << cgicc::label("Work Loop Status: ") << std::endl;
      if (workFecSupervisorLoop_->isActive()) {
	*out << cgicc::input().set("type","radio")
	  .set("name","RunningWL")
	  .set("value","true")
	  .set("selected").set("checked","true") << std::endl;
      }
      else {
	*out << cgicc::input().set("type","radio")
	  .set("name","RunningWL")
	  .set("value","false")
	  .set("selected") << std::endl;
      }
      *out << cgicc::label("Running ") << std::endl;
      
      if (suspendDcuWorkLoop_) { // || (sleepDcuWorkLoop_ && suspendDcuWorkLoop_)) {
	
	*out << cgicc::input().set("type","radio")
	  .set("name","SuspendedWL")
	  .set("value","true")
	  .set("selected").set("checked","true") << std::endl;
      }
      else {
	*out << cgicc::input().set("type","radio")
	  .set("name","SuspendedWL")
	  .set("value","false")
	  .set("selected") << std::endl;
      }
      *out << cgicc::label("Suspended ") << std::endl;
      
      if (sleepDcuWorkLoop_) {
	
	*out << cgicc::input().set("type","radio")
	  .set("name","SleepingWL")
	  .set("value","true")
	  .set("selected").set("checked","true") << std::endl;
      }
      else {
	*out << cgicc::input().set("type","radio")
	  .set("name","SleepingWL")
	  .set("value","false")
	  .set("selected") << std::endl;
      }
      *out << cgicc::label("Sleeping ") << std::endl;
      
      *out << cgicc::td() << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << std::endl ;

      *out << cgicc::p() << cgicc::label("DCU Work Loop Control: ") << std::endl;
      
      if (suspendDcuWorkLoop_)
	*out << cgicc::input().set("type","checkbox")
	  .set("name","DcuWorkLoopSuspend")
	  .set("selected").set("checked","true") << std::endl;
      else
	*out << cgicc::input().set("type","checkbox")
	  .set("name","DcuWorkLoopSuspend")
	  .set("selected") << std::endl;
      *out << cgicc::label("Suspend it") << std::endl;

      *out << cgicc::td() << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << std::endl ;

      // Display DCU values
      if (displayDcuDebugMessage_)
	*out << cgicc::p() << cgicc::input().set("type","checkbox")
	  .set("name","displayDcuDebugMessage")
	  .set("selected").set("checked","true") << std::endl;
      else
	*out << cgicc::p() << cgicc::input().set("type","checkbox")
	  .set("name","displayDcuDebugMessage")
	  .set("selected") << std::endl;
      *out << cgicc::label("Display DCU values on console") << std::endl;
    }

    *out << cgicc::td() << cgicc::tr() << std::endl ;
  }
  *out << cgicc::table() << std::endl ;

  // --------------------------------------------------------
  // Device work loop

  *out << cgicc::p() << cgicc::label("Time between two device reads: ") << std::endl;
  *out << cgicc::input().set("type","text")
    .set("name","deviceWorkLoopTime")
    .set("size","10")
    .set("value",deviceWorkLoopTime_.toString()) << std::endl;

  if (deviceThreadUsed_)
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","deviceThreadUsed")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","deviceThreadUsed")
      .set("selected") << std::endl;
  *out << cgicc::label("Device Work Loop Used") << std::endl;

  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  // Device work loop
  if (!deviceThreadUsed_ || workFecSupervisorLoop_ == NULL) {
    *out << cgicc::tr() << std::endl ;
    *out << cgicc::td() << cgicc::td() << std::endl ;
    *out << cgicc::td() << std::endl ;
    *out << cgicc::p() << FEC_RED << cgicc::label("No device readout enabled") << "</span>" << std::endl;
    *out << cgicc::td() << cgicc::tr() << std::endl ;
  }
  else { // thread or work loop to readout DCU are exiting

    // Work loop
    if (workFecSupervisorLoop_ != NULL) {

      *out << cgicc::tr() << std::endl ;
      *out << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << std::endl ;

      *out << cgicc::p() << cgicc::label("Work Loop Status: ") << std::endl;
      if (workFecSupervisorLoop_->isActive()) {
	*out << cgicc::input().set("type","radio")
	  .set("name","DeviceRunningWL")
	  .set("value","true")
	  .set("selected").set("checked","true") << std::endl;
      }
      else {
	*out << cgicc::input().set("type","radio")
	  .set("name","DeviceRunningWL")
	  .set("value","false")
	  .set("selected") << std::endl;
      }
      *out << cgicc::label("Running ") << std::endl;
      
      if (suspendDeviceWorkLoop_) { 
	
	*out << cgicc::input().set("type","radio")
	  .set("name","DeviceSuspendedWL")
	  .set("value","true")
	  .set("selected").set("checked","true") << std::endl;
      }
      else {
	*out << cgicc::input().set("type","radio")
	  .set("name","DeviceSuspendedWL")
	  .set("value","false")
	  .set("selected") << std::endl;
      }
      *out << cgicc::label("Suspended ") << std::endl;
      
      if (sleepDeviceWorkLoop_) {
	
	*out << cgicc::input().set("type","radio")
	  .set("name","DeviceSleepingWL")
	  .set("value","true")
	  .set("selected").set("checked","true") << std::endl;
      }
      else {
	*out << cgicc::input().set("type","radio")
	  .set("name","DeviceSleepingWL")
	  .set("value","false")
	  .set("selected") << std::endl;
      }
      *out << cgicc::label("Sleeping ") << std::endl;

      *out << cgicc::td() << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << std::endl ;
      
      *out << cgicc::p() << cgicc::label("Device Work Loop Control: ") << std::endl;
      
      if (suspendDeviceWorkLoop_)
	*out << cgicc::input().set("type","checkbox")
	  .set("name","DeviceWorkLoopSuspend")
	  .set("selected").set("checked","true") << std::endl;
      else
	*out << cgicc::input().set("type","checkbox")
	  .set("name","DeviceWorkLoopSuspend")
	  .set("selected") << std::endl;
      *out << cgicc::label("Suspend it") << std::endl;

      *out << cgicc::td() << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << std::endl ;

      // Display DCU values
      if (displayDeviceDebugMessage_)
	*out << cgicc::p() << cgicc::input().set("type","checkbox")
	  .set("name","displayDeviceDebugMessage")
	  .set("selected").set("checked","true") << std::endl;
      else
	*out << cgicc::p() << cgicc::input().set("type","checkbox")
	  .set("name","displayDeviceDebugMessage")
	  .set("selected") << std::endl;
      *out << cgicc::label("Display device values on console") << std::endl;

      *out << cgicc::td() << cgicc::tr() << std::endl ;
    }
  }
  *out << cgicc::table() << std::endl ;
  *out << cgicc::fieldset() << std::endl;

  *out << cgicc::p() << std::endl ;

  // Display errors in frame
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << cgicc::legend("Display Messages") << std::endl;
  if (displayFrameErrorMessages_) 
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayFrameErrorMessages")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayFrameErrorMessages")
      .set("selected") << std::endl;
  *out << cgicc::label("Display error messages on web pages") << std::endl;

  // Display messages on console
  if (displayDebugMessage_) 
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayDebugMessage")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayDebugMessage")
      .set("selected") << std::endl;
  *out << cgicc::label("Display messages on console") << std::endl;

  // Display error on console
  if (displayDebugMessage_) displayErrorOnConsole_ = true ;
  if (displayErrorOnConsole_)
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayErrorOnConsole")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayErrorOnConsole")
      .set("selected") << std::endl;
  *out << cgicc::label("Display error messages on console") << std::endl;

  // Display device values
  if ((fecAccessManager_ != NULL) && (fecAccessManager_->getDisplayDebugMessage()))
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayFecAccessManagerDebugMessage")
      .set("selected").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","checkbox")
      .set("name","displayFecAccessManagerDebugMessage")
      .set("selected") << std::endl;
  *out << cgicc::label("Display device values on console") << std::endl;

  // Max number of errors allowed
  *out << cgicc::p() << cgicc::label("Maximum number of errors allowed: ") << std::endl;
  *out << cgicc::input().set("type","text")
    .set("name","maxErrorAllowed")
    .set("size","10")
    .set("value",maxErrorAllowed_.toString()) << std::endl;

  // Multiple frame algorithm
  if (multiFrames_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","multiFrames").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","multiFrames") << std::endl;
  *out << cgicc::label("Multiple frame algorithm used") << std::endl;

  // VME Block transfer
  if (blockMode_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","blockMode").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","blockMode") << std::endl;
  *out << cgicc::label("VME Block Transfer used") << std::endl;

  // Status of the application
  *out << cgicc::p() << FEC_GREEN << "Last action performed: " << xdaqApplicationStatus_.toString() << "</span>" << std::endl ;

  *out << cgicc::fieldset() << std::endl;

  // Apply
  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply");
  
  // End of the form
  *out << cgicc::form() << std::endl;

  *out << "</HTML>" << std::endl ; 
}

/** Apply the parameters
 */
void FecSupervisor::moreParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("moreParameters", LOGDEBUG) ;

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // ------------------------------------------------------------
    // Check the different parameters of the checking box
    resetFec_   = cgi.queryCheckbox("fecReset");
    applyRedundancy_ = cgi.queryCheckbox("applyRedundancy");
    forceApplyRedundancy_ = cgi.queryCheckbox("applyRedundancy");
    resetPia_   = cgi.queryCheckbox("piaReset");
    doUpload_   = cgi.queryCheckbox("doUpload");
    configureDOH_ = cgi.queryCheckbox("configureDOH");

    // a message for redundancy
    if (forceApplyRedundancy_) applyRedundancy_ = true ;
    if (forceApplyRedundancy_ && applyRedundancy_) {
	errorReportLogger_->errorReport ("For the redundancy settings on the rings even if the ring is working", LOGUSERINFO) ;
    }

    // ------------------------------------------------------------
    // Retrieve all the parameters for the configuration
    databaseDownload_ = false ;
#ifdef DATABASE
    if (databaseAccess_) {

      // Check if the user want to use the database or the files
      std::string databaseDownload = cgi["databaseDownload"]->getValue() ;
      if (databaseDownload == "databaseDownloadCurrentVersion") {
	databaseDownload_ = true ;
	versionMajorId_ = 0 ;
	versionMinorId_ = 0 ;
	maskMajor_ = 0 ;
	maskMinor_ = 0 ;

	std::stringstream msgInfo ; msgInfo << "Use the database with current version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
      }
      else if (databaseDownload == "databaseDownloadSpecificVersion") {
	databaseDownload_ = true ;
        std::string versionStr = cgi["versionId"]->getValue() ;
	std::string::size_type ipass = versionStr.find(".");
	if (ipass != std::string::npos) {
	  versionMajorId_ = fromString<unsigned long>(versionStr.substr(0,ipass)) ;
	  versionMinorId_ = fromString<unsigned long>(versionStr.substr(ipass+1,versionStr.size())) ;
	  maskMajor_ = 0 ;
	  maskMinor_ = 0 ;
	}
	//setPartitionVersion() ;

	std::stringstream msgInfo ; msgInfo << "Use the database with version " << versionMajorId_ << "." << versionMinorId_ << " (mask = " << maskMajor_ << "." << maskMinor_ << ")" ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
      }
    }
#endif

    // Retrieve the file name if the database is not used
    if (!databaseDownload_) {

      xmlFileName_ = cgi["xmlFileName"]->getValue();

      std::stringstream msgInfo ; msgInfo << "Use an XML file: " << xmlFileName_.toString() ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
    }

    // ----------------------------------------------------
    // Work Loop used
    dcuThreadUsed_ = cgi.queryCheckbox("dcuThreadUsed");
    deviceThreadUsed_ = cgi.queryCheckbox("deviceThreadUsed");

    if (dcuThreadUsed_) {

      dcuWorkLoopTime_ = fromString<unsigned long int>(cgi["dcuWorkLoopTime"]->getValue()) ;
      manageDcuWorkLoop () ;
      if (workFecSupervisorLoop_ != NULL) { 
	suspendDcuWorkLoop_ = cgi.queryCheckbox("DcuWorkLoopSuspend") ;
	displayDcuDebugMessage_ = cgi.queryCheckbox("displayDcuDebugMessage") ;
      }
    }
    if (deviceThreadUsed_) {

      deviceWorkLoopTime_ = fromString<unsigned long int>(cgi["deviceWorkLoopTime"]->getValue()) ;
      manageDeviceWorkLoop () ;
      if (workFecSupervisorLoop_ != NULL) { 
	suspendDeviceWorkLoop_ = cgi.queryCheckbox("DeviceWorkLoopSuspend") ;
	displayDeviceDebugMessage_ = cgi.queryCheckbox("displayDeviceDebugMessage") ;
      }
    }

    // if the work loop is not used anymore then stop it
    if (!dcuThreadUsed_ && !deviceThreadUsed_) stopWorkLoop() ;

    // -----------------------------------------------------------------
    // Other parameters
    initTTCRx_     = cgi.queryCheckbox("initTTCRx");
    coldPllReset_  = cgi.queryCheckbox("coldPllReset");
    i2cSpeed_      = fromString<unsigned short>(cgi["i2cSpeed"]->getValue()) ;
    displayFrameErrorMessages_ = cgi.queryCheckbox("displayFrameErrorMessages");
    displayDebugMessage_ = cgi.queryCheckbox("displayDebugMessage");

    //if (displayDebugMessage_) displayErrorOnConsole_ = true ;
    displayErrorOnConsole_ = cgi.queryCheckbox("displayErrorOnConsole");
    //bool displayDebugMessageDcu = cgi.queryCheckbox("displayDebugMessageDcu");
    //bool displayDebugMessageAccessManager = cgi.queryCheckbox("displayDebugMessageAccessManager");

    // Display
    errorReportLogger_->setlogLevel (displayDebugMessage_ ? LOGDEBUG : LOGWARNING) ;
    errorReportLogger_->setDisplayOutput (displayErrorOnConsole_) ;

    // Multiframes
    maxErrorAllowed_ = fromString<unsigned long int>(cgi["maxErrorAllowed"]->getValue()) ;
    multiFrames_     = cgi.queryCheckbox("multiFrames");

    // VME block transfer
    if (blockMode_ != cgi.queryCheckbox("blockMode")) {
      blockMode_ = cgi.queryCheckbox("blockMode") ;
      if (fecAccess_ != NULL) fecAccess_->setFifoAccessMode (blockMode_) ;
    }

    // Number of errors
    if (fecAccessManager_ != NULL) {
      fecAccessManager_->setMaxErrorAllowed ( maxErrorAllowed_ ) ;
      fecAccessManager_->setDisplayDebugMessage(cgi.queryCheckbox("displayFecAccessManagerDebugMessage")) ;
    }

    // Reset CCS TTCRx
    if (initTTCRx_) initTTCRx() ;

    // Check the SSID for VME FEC and set it if needed
    if (fecBusType_ == fecBusTypeList[FECVME]) {
      
      if (SSID_ != cgi["SSID"]->getValue()) {
	SSID_ = cgi["SSID"]->getValue() ;
	setCCSTriggerSSID ( ) ;
      }
    }
  }
  catch(const std::exception& e) {
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  // Redisplay the page
  this->displayMoreParameters (in,out) ;
}
 
//  xgi::bind(this, &FecSupervisor::displayMoreParameters, "MoreParameters");
//  xgi::bind(this, &FecSupervisor::displayFecHardwareCheck, "HardwareCheck");
// Related links with partition / version creation
void FecSupervisor::displayHardwareCheck (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  if (fecAccessManager_ != NULL) {

    //*out << cgicc::HTTPHTMLHeader();
    *out << "<HTML>" << std::endl ;
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("FecSupervisor Hardware Check") << std::endl;
    xgi::Utils::getPageHeader(*out, "FecSupervisor Hardware Check");

    // ------------------------------------------------------------------------
    // Display the links
    displayRelatedLinks (in,out) ;
    linkPosition_ = 3 ;

    // ------------------------------------------------------------------------
    // Display the FEC and ring information
    displayFecRingField(in,out,fecSlot_) ;

    // ------------------------------------------------------------------------
    // End of form
    *out << "</HTML>" << std::endl ; 
  }
  else {
    //*out << cgicc::HTTPHTMLHeader();
    *out << "<HTML>" << std::endl ;
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("FecSupervisor Hardware Check") << std::endl;
    xgi::Utils::getPageHeader(*out, "FecSupervisor Hardware Check");

    // ------------------------------------------------------------------------
    // Display the links
    displayRelatedLinks (in,out) ;
    linkPosition_ = 3 ;

    // ------------------------------------------------------------------------
    // Display the FEC and ring information
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

    // ------------------------------------------------------------------------
    // End of form
    *out << "</HTML>" << std::endl ; 
  }
}

/** Display all the FEC Ring with status and resets
 */
void FecSupervisor::displayFecRingDisplay (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayFecRingDisplay", LOGDEBUG) ;
  
  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("FecSupervisor FEC ring display") << std::endl;

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::displayFecRingDisplay)", LOGFATAL, 0, XDAQFEC_HARDWAREACCESSERROR) ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

    *out << "</HTML>" << std::endl ;     

    return ;
  }

  *out << "<center>" << cgicc::h3("FEC / Ring").set("style", "font-family: arial") << "</center>" << std::endl;  
  *out << "<center>" << "Firmware version 0x" << std::hex << fecAccess_->getFecFirmwareVersion(buildFecRingKey(fecSlot_,ringMin_)) << "</center>" << std::endl ; 
  //*out << "<center>" << "VME Firmware version " << "Not implemented" << "</center>" << std::endl ;
  *out << cgicc::br();
  *out << "<center>" << "Please select the FEC ring:" << "</center>" << std::endl;   
  *out << cgicc::br();

  // ----------------------------------------------------------------------
  // Form
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/fecRingChoice";
  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  // retrieve the list of the FECs
  std::list<keyType> *fecList = NULL ;
  try {
    fecList = fecAccess_->getFecList() ;
    if ((fecList == NULL) || (fecList->empty())) {
      if (fecList != NULL) delete fecList ;
      *out << cgicc::h2("No FEC/ring detected").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    }
    else {

      // Find a ring to select it if it is not
      bool findFec = false ;
      for (std::list<keyType>::iterator p=fecList->begin();p!=fecList->end();p++) 
	if (*p == indexFecRingChosen_) findFec = true ;
      if (!findFec)
	indexFecRingChosen_ = *(fecList->begin()) ;
      
      *out << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0").set("align","center") << std::endl ;
      *out << cgicc::tr().set("align","center") << std::endl;
      *out << "<th align=center>" << std::endl;
      *out << cgicc::label(" ") << std::endl;
      *out << "<th align=center>" << std::endl;
      *out << cgicc::label("FEC/Ring") << std::endl;
      *out << "<th align=center>" << std::endl;
      *out << cgicc::label("Status") << std::endl;
      *out << cgicc::tr() << std::endl ;
      
      // Retrieve the FEC list
      for (std::list<keyType>::iterator p=fecList->begin();p!=fecList->end();p++) {
	
	keyType index = *p ;
	std::string strFecRing = toString(getFecKey(index)) + "." + toString(getRingKey(index)) ;
	
	std::stringstream msgInfo ; msgInfo << "FEC " << std::dec << (int)getFecKey(index) << "." << (int)getRingKey(index) << " found" ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	
	*out << cgicc::tr().set("align","center") << std::endl;
	if (index == indexFecRingChosen_)
	  *out << cgicc::td() << cgicc::input().set("type","radio")
	    .set("name","FecRingChoice")
	    .set("size","30")
	    .set("value",strFecRing)
	    .set("selected").set("checked","true") << cgicc::td() << std::endl;
	else
	  *out << cgicc::td() << cgicc::input().set("type","radio")
	    .set("name","FecRingChoice")
	    .set("size","30")
	    .set("value",strFecRing)
	    .set("selected") << cgicc::td() << std::endl;
	*out << cgicc::td() << cgicc::label(strFecRing) <<  cgicc::td() <<std::endl;
	if (isFecSR0Correct(fecAccess_->getFecRingSR0(index)))
	  *out << cgicc::td() << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/greenLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">" << cgicc::td() ;
	else
	  if (fecAccess_->getFecRingSR0(index) & FEC_SR0_LINKINITIALIZED)
	    *out << cgicc::td() << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/orangeLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">" << cgicc::td() ;
	  else
	    *out << cgicc::td() << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/redLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">" << cgicc::td() ;
	*out << cgicc::tr() << std::endl;
      }
      
      // End of the table
      *out << cgicc::table() << std::endl;
      
      // Apply
      *out << "<p align=CENTER>" << std::endl ;
      *out << cgicc::input().set("type", "submit")
	.set("name", "submit")
	.set("value", "Select");
      *out << "</p>" << std::endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot retrieve the list of the FEC and their status").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;

    errorReportLogger_->errorReport ("Cannot retrieve the list of the FEC and their status", e, LOGERROR) ;
  }
   
  delete fecList ;

  // ------------------------------------------------------------------------
  // End of form
  *out << cgicc::form() << std::endl;
  *out << "</HTML>" << std::endl ; 
}

/** Just select the corresponding FEC ring depend of the choice of the user
 */
void FecSupervisor::fecRingChoice (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {  
  
  std::list<keyType> *fecList = NULL ;
  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);
    
    // Check which ring is chosen by the user
    // retrieve the list of the FECs
    fecList = fecAccess_->getFecList() ;
    if ((fecList == NULL) || (fecList->empty())) {
      if (fecList != NULL) delete fecList ;
      *out << cgicc::h2("No FEC/ring detected").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    }
    else {
      bool trouve = false ;
      for (std::list<keyType>::iterator p=fecList->begin();((p!=fecList->end()) && !trouve);p++) {
	
	keyType index = *p ;
	std::string strFecRing = toString(getFecKey(index)) + "." + toString(getRingKey(index)) ;

	if (strFecRing == cgi["FecRingChoice"]->getValue()) {
	  indexFecRingChosen_ = index ;
	  trouve = true ;
	}
      }
    }
  }
  catch (FecExceptionHandler &e) {

    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot retrieve the list of the FEC and their status").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;

    errorReportLogger_->errorReport ("Cannot retrieve the list of the FEC and their status", e, LOGERROR) ;
  }
  catch(const std::exception& e) {

    XCEPT_RAISE(xgi::exception::Exception, e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }
   
  delete fecList ;

  // Re-display the page
  //this->displayFecRingDisplay(in,out) ;
  this->Default(in,out) ;
}

/** This method will receive the command issue by the recovery lvl1 system
 * \param msg - message
 * \return retrun an acknowledge
 */
xoap::MessageReference FecSupervisor::fecSupervisorRecovery (xoap::MessageReference msg) throw (xoap::exception::Exception) {

  // reply to caller
  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName responseName = envelope.createName( "receivedLogResponse", "xdaq", XDAQ_NS_URI);
  envelope.getBody().addBodyElement ( responseName );

  // --------------------------------------------------------------------------------------------------------
  // Analyse and parse the message 

  // --------------------------------------------------------------------------------------------------------
  // Action
  errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::fecSupervisorRecovery)", LOGFATAL, 1, 1024) ;

  // --------------------------------------------------------------------------------------------------------
  // End of recovery 
  errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied (FecSupervisor::fecSupervisorRecovery)", LOGFATAL, -1, 1024) ;

  return reply;
}
/** Check if there are rings needing redundancy
 */
bool FecSupervisor::isRedundancyNeeded () {
  // Check the the accesses was created
  if (fecAccess_ == NULL) {

    // Error reporting: sent to the 1st level of error diag
    errorReportLogger_->errorReport (std::string("Hardware access was not created, no command can be applied ") + __PRETTY_FUNCTION__, LOGFATAL, 0, XDAQFEC_HARDWAREACCESSERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return false;
  }

  // get the list
  std::list<keyType> *fecList = fecAccess_->getFecList() ;
  if ((fecList == NULL) || (fecList->empty())) {
    if (fecList != NULL) delete fecList ;
    errorReportLogger_->errorReport ("No VME FEC board detected on this setup on the slot " + fecSlot_.toString(), LOGFATAL, 0, XDAQFEC_NOFECDETECTED) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: No VME FEC board detected on this setup on slot "
				    << fecSlot_.toString()
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return false;
  }

  bool firstDbClient = true ;

  // -------------------------------------------------------------------------
  // for each ring
  for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {

    if (redundancyRings_[getRingKey(*it)]) { // if redundancy has not already been set

      // redundancyRings_[getRingKey(*it)] = false ; 
      try {

	// -------------------------------------------------------------------------
	// Check that one configuration is available
	// retrieve the ring information or from the DbClient or from the database
	TkRingDescription *tkRing = NULL ;

#if defined(TRACKERXDAQ)
	// Access the shared memory
	if (shareMemoryName_ != DEFINENONE) {

	  // The first time when it is needed => make the parsing of data
	  if (firstDbClient) {
	    // parse the database to get the redundancy configuration
	    if (dbClient_ != NULL) dbClient_->parse() ;
	    firstDbClient = false ;
	  }
	  
	  try {
	    // download the redundancy
	    tkRing = dbClient_->getTkRingDescription ( fecHardwareId_, getRingKey(*it) ) ;
	    if (tkRing == NULL) {
	      std::stringstream msgError ; msgError << "Unable to retrieve the ring from the database client, no reconfiguration available for the ring " << std::dec << getRingKey(*it) ;
		//Modified by L.GROSS - 28/07/2009 16:20 - log level downgraded from WARNING to INFO
//	      errorReportLogger_->errorReport (msgError.str(), LOGWARNING) ;
	      errorReportLogger_->errorReport (msgError.str(), LOGINFO) ;
	    }
	  }
	  catch (std::string &e) {
	    std::stringstream msgError ; msgError << "Unable to retrieve the ring from the database client (DbClient: " << e << ") with the the shared memory " << shareMemoryName_.toString() << " for FEC " << fecHardwareId_ << " ring " << std::dec << getRingKey(*it) ;
	    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  } 
	}
	else
#endif
#ifdef DATABASE
	  // Access the database
	  if (databaseDownload_ && databaseAccess_ && (fecFactory_ != NULL)) {
	    
	    try {
	      tkRing = fecFactory_->getDbRingDescription(fecHardwareId_, getRingKey(*it)) ;
	      if (tkRing == NULL) {
		std::stringstream msgError ; msgError << "Unable to retrieve the ring from the database, no reconfiguration available for FEC " << fecHardwareId_ << " ring " << std::dec << getRingKey(*it) ;
		errorReportLogger_->errorReport (msgError.str(), LOGWARNING) ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::stringstream msgError ; msgError << "Unable to retrieve the ring from database (FecExceptionHandler) for FEC " << fecHardwareId_ << " ring " << std::dec << getRingKey(*it) ;
	      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	    }
	    catch (oracle::occi::SQLException &e) {
	      std::stringstream msgError ; msgError << "Unable to retrieve the ring from database (oracle::occi::SQLException) for FEC " << fecHardwareId_ << " ring " << std::dec << getRingKey(*it) ;
	      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	    }
	  }
	  else
#endif
	    {
	      std::stringstream msgError ; msgError << "No database or DbClient specified, cannot apply redundancy check" ;
	      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	    }
	
	// -----------------------------------------------------------
	// Found one configuration apply it
	if (tkRing != NULL) {

	  // -------------------------------------------------------------------------
	  // apply the redundancy 
	  if (!tkRing->isReconfigurable()) {
	    std::stringstream msgInfo ; 
	    msgInfo << "The configuration given for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) << " is not correct, please check it: " ;
	    tkRing->display(msgInfo) ;
	    std::string detailedError = tkRing->getReconfigurationProblem();
	    msgInfo << ": " << detailedError ;
	    errorReportLogger_->errorReport (msgInfo.str(), LOGERROR) ;
	  }
	  else {
            try {
	      tscType16 fecSR0 = fecAccess_->getFecRingSR0(*it) ;
	      if (fecSR0&FEC_SR0_LINKINITIALIZED) {
		std::stringstream msgInfo ; 
		msgInfo << "SR0 is correct (0x" << std::hex <<fecSR0 << std::dec<<"), not applying redundancy on FEC " << getFecKey(*it) << "." << getRingKey(*it) << ": " ; tkRing->display(msgInfo) ;
		errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
		continue;
	      }
	      else {
		delete fecList ;
		return true;
	      }
	    } catch (FecExceptionHandler &e) {
	      redundancyRings_[getRingKey(*it)] = true ; 
	      std::stringstream msgError ; msgError << "Error checking SR0 in isRedundancyNeeded for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	    }
	    catch (std::exception e) {
	      redundancyRings_[getRingKey(*it)] = true ; 
	      std::cerr << "Caught std exception is isRedundancyNeeded" << e.what() << std::endl;      
	      std::stringstream msgError ; msgError << "Error reading SR0 in isRedundancyNeeded for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) << ": "<<e.what();
	      errorReportLogger_->errorReport (msgError.str(),  LOGERROR) ;
	    }
	    catch (...){
	      redundancyRings_[getRingKey(*it)] = true ; 
	      std::cerr << "Caught unknown exception in isRedundancyNeeded " << std::endl;
	      std::stringstream msgError ; msgError << "Error during the recovery of the ring for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	    }
	  }
	}
	else {
	  std::stringstream msgInfo ; msgInfo << "No available reconfiguration for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	  errorReportLogger_->errorReport (msgInfo.str(), LOGDEBUG) ;
	}
      }
      catch (FecExceptionHandler &e) {
	std::stringstream msgError ; msgError << "Unable to retrieve FEC status on FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
      }
    }
  }
  delete fecList ;
  return false;
}

/** The method scan the rings for each ring that has a not correct state, try to recover it
 */
void FecSupervisor::setRedundancy (bool reset) {
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " START", LOGUSERINFO) ;

  hasRedundancy_ = false;

  // Check the the accesses was created
  if (fecAccess_ == NULL) {

    // Error reporting: sent to the 1st level of error diag
    errorReportLogger_->errorReport (std::string("Hardware access was not created, no command can be applied ") + __PRETTY_FUNCTION__, LOGFATAL, 0, XDAQFEC_HARDWAREACCESSERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: Hardware access was not created, no command can be applied"
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  // get the list
  std::list<keyType> *fecList = fecAccess_->getFecList() ;
  if ((fecList == NULL) || (fecList->empty())) {
    if (fecList != NULL) delete fecList ;
    errorReportLogger_->errorReport ("No VME FEC board detected on this setup on the slot " + fecSlot_.toString(), LOGFATAL, 0, XDAQFEC_NOFECDETECTED) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "FATAL Error: No VME FEC board detected on this setup on slot "
				    << fecSlot_.toString()
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }

    return ;
  }

  bool firstDbClient = true ;

  // -------------------------------------------------------------------------
  // for each ring
  for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {

    if (redundancyRings_[getRingKey(*it)]) { // if redundancy has not already been set

      redundancyRings_[getRingKey(*it)] = false ; 
      try {

	// -------------------------------------------------------------------------
	// Check that one configuration is available
	// retrieve the ring information or from the DbClient or from the database
	TkRingDescription *tkRing = NULL ;

#if defined(TRACKERXDAQ)
	// Access the shared memory
	if (shareMemoryName_ != DEFINENONE) {

	  // The first time when it is needed => make the parsing of data
	  if (firstDbClient) {
	    // parse the database to get the redundancy configuration
	    if (dbClient_ != NULL) dbClient_->parse() ;
	    firstDbClient = false ;
	  }
	  
	  try {
	    // download the redundancy
	    tkRing = dbClient_->getTkRingDescription ( fecHardwareId_, getRingKey(*it) ) ;
	    if (tkRing == NULL) {
	      std::stringstream msgError ; msgError << "Unable to retrieve the ring from the database client, no reconfiguration available for the ring " << std::dec << getRingKey(*it) ;
		//Modified by L.GROSS - 28/07/2009 16:20 - log level downgraded from WARNING to INFO
//	      errorReportLogger_->errorReport (msgError.str(), LOGWARNING) ;
	      errorReportLogger_->errorReport (msgError.str(), LOGINFO) ;
	    }
	  }
	  catch (std::string &e) {
	    std::stringstream msgError ; msgError << "Unable to retrieve the ring from the database client (DbClient: " << e << ") with the the shared memory " << shareMemoryName_.toString() << " for FEC " << fecHardwareId_ << " ring " << std::dec << getRingKey(*it) ;
	    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  } 
	}
	else
#endif
#ifdef DATABASE
	  // Access the database
	  if (databaseDownload_ && databaseAccess_ && (fecFactory_ != NULL)) {
	    
	    try {
	      tkRing = fecFactory_->getDbRingDescription(fecHardwareId_, getRingKey(*it)) ;
	      if (tkRing == NULL) {
		std::stringstream msgError ; msgError << "Unable to retrieve the ring from the database, no reconfiguration available for FEC " << fecHardwareId_ << " ring " << std::dec << getRingKey(*it) ;
		errorReportLogger_->errorReport (msgError.str(), LOGWARNING) ;
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      std::stringstream msgError ; msgError << "Unable to retrieve the ring from database (FecExceptionHandler) for FEC " << fecHardwareId_ << " ring " << std::dec << getRingKey(*it) ;
	      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	    }
	    catch (oracle::occi::SQLException &e) {
	      std::stringstream msgError ; msgError << "Unable to retrieve the ring from database (oracle::occi::SQLException) for FEC " << fecHardwareId_ << " ring " << std::dec << getRingKey(*it) ;
	      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	    }
	  }
	  else
#endif
	    {
	      std::stringstream msgError ; msgError << "No database or DbClient specified, cannot apply redundancy check" ;
	      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	    }
	
	// -----------------------------------------------------------
	// Found one configuration apply it
	if (tkRing != NULL) {

	  // -------------------------------------------------------------------------
	  // apply the redundancy 
	  if (!tkRing->isReconfigurable()) {
	    std::stringstream msgInfo ; 
	    msgInfo << "The configuration given for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) << " is not correct, please check it: " ;
	    tkRing->display(msgInfo) ;
	    std::string detailedError = tkRing->getReconfigurationProblem();
	    msgInfo << ": " << detailedError ;
	    errorReportLogger_->errorReport (msgInfo.str(), LOGERROR) ;
	  }
	  else {
	    try {
	      std::stringstream msgInfo ;
	      tscType16 fecSR0;

	      /*
	      tscType16 fecSR0 = fecAccess_->getFecRingSR0(*it) ;
	      if (fecSR0&FEC_SR0_LINKINITIALIZED) {
		msgInfo << "SR0 is correct (0x" << std::hex <<fecSR0 << std::dec<<"), not applying redundancy on FEC " << getFecKey(*it) << "." << getRingKey(*it) << ": " ; tkRing->display(msgInfo) ;
		errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
		continue;
	      }
	      */
              hasRedundancy_ = true;
	      msgInfo << "Apply redundancy on FEC " << getFecKey(*it) << "." << getRingKey(*it) << ": " ; tkRing->display(msgInfo) ;
	      errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
	      
	      bool reconfigurationWorked=false;
	      int nReconfigurationTries=12;
	      for (int counter=0; (counter<nReconfigurationTries)&&(!reconfigurationWorked); counter++) {
		fecSR0 = fecAccess_->fecRingReconfigure (*it, *tkRing);
		usleep(1000); // wait 1ms
		
		// -------------------------------------
		// is the reconfiguration working fine ?
		reconfigurationWorked=isFecSR0Correct(fecSR0);
		if (!reconfigurationWorked) {
		  std::stringstream msgError;
		  msgError << "The configuration given for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it)
			   << " did not work at try number " << counter+1 << std::endl;
		  errorReportLogger_->errorReport (msgError.str(), LOGWARNING);
		}
		
	      }
	      
	      if (!isFecSR0Correct(fecSR0)) {
		std::stringstream msgError ; 
		msgError << "The configuration given for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) 
			 << " returns the status 0x" << std::hex << fecSR0 << std::dec ;
		errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	      }
	      else {
		// -------------------------------------
		// Everything is ok until now: redundancy is working.
		std::stringstream msgInfo ; 
		msgInfo << "The configuration given for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) 
			<< " returns the status 0x" << std::hex << fecSR0 << std::dec << " and was applied correctly" ;
		errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
		
		redundancyRings_[getRingKey(*it)] = true ; // redundancy was checked correctly

#define FECSUPERVISOR_DISPLAYCCUSRA		
#ifdef FECSUPERVISOR_DISPLAYCCUSRA
		// -------------------------------------
		// Clear the CCU SRA and reset the i2c channel
		if (reset) {
		  try {
		    std::list<keyType> *listCCU = fecAccess_->getCcuList(*it) ;
		    if (listCCU->size() > 0) {
		      for (std::list<keyType>::iterator ccuit = listCCU->begin() ; ccuit != listCCU->end() ; ccuit ++) {
			try {
			  fecAccess_->setCcuCRA(*ccuit, CCU_CRA_CLRE || CCU_CRA_RES ) ;
			  
			  std::stringstream msgInfo ;
			  msgInfo << "After reconfiguration: FEC " << getFecKey(*ccuit) << " ring " << getRingKey(*ccuit) << " CCU 0x" << std::hex << getCcuKey(*ccuit) << ": SRA = 0x" << (int)fecAccess_->getCcuSRA(*ccuit) << std::dec << std::endl ;
			  errorReportLogger_->errorReport (msgInfo.str(), LOGDEBUG) ;
			}
			catch (FecExceptionHandler &e) {
			  std::stringstream msgError ; msgError << "Unable to clear the CCU CRA " << fecSlot_.toString() << " ring " << getRingKey(*ccuit) << " CCU " << getCcuKey(*ccuit) ;
			  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
			}
		      }
		    }
		  }
		  catch (FecExceptionHandler &e) {
		    std::stringstream msgError ; msgError << "Unable to get the list of CCUs or cannot read the CCU SRA for one of the CCUsfor FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
		    errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
		  }
		}
#endif
	      }
	    }
	    catch (FecExceptionHandler &e) {
	      redundancyRings_[getRingKey(*it)] = true ; 
	      std::stringstream msgError ; msgError << "Error during the recovery of the ring for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	    }
	    catch (std::exception e) {
	      redundancyRings_[getRingKey(*it)] = true ; 
	      std::cerr << "Caught std exception is setRedundancy" << e.what() << std::endl;      
	      std::stringstream msgError ; msgError << "Error during the recovery of the ring for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) << ": "<<e.what();
	      errorReportLogger_->errorReport (msgError.str(),  LOGERROR) ;
	    }
	    catch (...){
	      redundancyRings_[getRingKey(*it)] = true ; 
	      std::cerr << "Caught unknown exception in setRedundancy " << std::endl;
	      std::stringstream msgError ; msgError << "Error during the recovery of the ring for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	    }
	  }
	}
	else {
	  std::stringstream msgInfo ; msgInfo << "No available reconfiguration for FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	  errorReportLogger_->errorReport (msgInfo.str(), LOGDEBUG) ;
	}
      }
      catch (FecExceptionHandler &e) {
	std::stringstream msgError ; msgError << "Unable to retrieve FEC status on FEC " << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
      }
    }
  }

  delete fecList ;
  errorReportLogger_->errorReport(std::string(__PRETTY_FUNCTION__) + " END", LOGUSERINFO) ;
}

/** This method analyse the error and try to display the source of the error
 * e - FecExceptionHandler
 */
void FecSupervisor::i2cRecover ( FecExceptionHandler e ) {

  try {
    if (e.getPositionGiven()) {
      
      keyType index = e.getHardPosition() ;
      tscType16 fecSR0 = fecAccess_->getFecRingSR0(index) ;
      if (isFecSR0Correct(fecSR0)) {
	if (fecSR0 & fecSR0 & FEC_SR0_LINKINITIALIZED) {
	  if (fecAccess_->getFecRingDevice(index)->isChannelEnabled(index)) {
	    std::stringstream msgError ; msgError << "-----------------> Source of the problem: not known" << std::endl ;
	    errorReportLogger_->errorReport (msgError.str(),LOGERROR) ;
	  }
	  else {
	    std::stringstream msgError ; msgError << "-----------------> Source of the problem: Channel not enabled (0x" << std::hex << (int)fecAccess_->getCcuCRE(index) << ")";
	    errorReportLogger_->errorReport (msgError.str(),LOGERROR) ;
	  }
	}
	else {
	  std::stringstream msgError ; msgError << "-----------------> Source of the problem: Ring not close" ;
	  errorReportLogger_->errorReport (msgError.str(),LOGERROR) ;
	}
      }
    }
  }
  catch (FecExceptionHandler &e) {
    // Log the error
    std::stringstream msgError ; msgError << "-----------------> I2C Recover procedure: unable to apply a command" ;
    errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
    if (displayFrameErrorMessages_) { 
      if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
      std::stringstream temp ; temp << "Error: " << msgError.str()
				    << std::endl << e.what() 
				    << std::endl << textInformation_.str() ;
      textInformation_.str(temp.str()) ;
    }
  }
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
void FecSupervisor::bypass ( FecAccess *fecAccess, unsigned int ring, unsigned short ccu1, unsigned short ccu3, unsigned short dummy ) {

  unsigned long tms = 10 ;

  keyType indexFecRing = buildFecRingKey(fecSlot_,ring) ;
  keyType ccuOne       = buildCompleteKey(fecSlot_,ring,ccu1,0,0) ;
  keyType ccuThree     = buildCompleteKey(fecSlot_,ring,ccu3,0,0) ;
  keyType ccuDummy     = buildCompleteKey(fecSlot_,ring,dummy,0,0) ;

  try {
    errorReportLogger_->errorReport("Starting the redundancy of the ring by bypassing one of the CCU (SR0 = 0x" + 
				   toHexString((int)fecAccess->getFecRingSR0(indexFecRing)), LOGINFO) ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport("Cannot read the SR0 but continue anyway with the redundancy", e, LOGERROR) ;
  }

  try {
    unsigned int timeout = 10 ;
    do {
      // Make a reset
      fecAccess->fecHardReset (indexFecRing) ;
      fecAccess->fecRingReset (indexFecRing) ;
      fecAccess->fecRingResetB ( indexFecRing ) ;
      if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) {
	
	errorReportLogger_->errorReport("PARASITIC: After reset => SR0 = 0x" +
				       toHexString((int)fecAccess->getFecRingSR0(indexFecRing)), LOGWARNING) ;
	emptyFIFOs ( fecAccess, indexFecRing, false ) ;
      }
      
      // Log message
      errorReportLogger_->errorReport("FEC reset Performed (SR0 = 0x" +
				     toHexString((int)fecAccess->getFecRingSR0(indexFecRing)), LOGINFO) ;
      timeout -- ;
	
      if (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing))) usleep (tms) ;
    }
    while (! isFecSR0Correct(fecAccess->getFecRingSR0(indexFecRing)) && timeout > 0) ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport("Problem during the reset of the ring", e, LOGERROR) ;
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
    errorReportLogger_->errorReport("Problem during the redundancy", e, LOGERROR) ;
  }

  try { // Empty the FIFOs
    emptyFIFOs ( fecAccess, indexFecRing, false ) ;    
    errorReportLogger_->errorReport("End of the redundancy (SR0 = 0x" +
				   toHexString((int)fecAccess->getFecRingSR0(indexFecRing)), LOGINFO) ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport("Error during the read of the SR0", e, LOGERROR) ;
  }
  
  try {
    // get the CCU list and display it
    std::list<keyType> *ccuList = fecAccess->getCcuList ( indexFecRing ) ;
    std::stringstream msgCcu ;
    msgCcu << "Found the CCU " << std::hex ;
    for (std::list<keyType>::iterator it = ccuList->begin() ; it != ccuList->end() ; it++) {
      msgCcu << getCcuKey(*it) << "\t" ;
    }
    delete ccuList ;
    errorReportLogger_->errorReport(msgCcu.str(), LOGINFO) ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport("Problem during the retrieve of the CCU", e, LOGERROR) ;
  }
}

/** This method try to recover the ring until the status register 0 is ok
 */
void FecSupervisor::emptyFIFOs ( FecAccess *fecAccess, keyType index, bool display ) {

  //errorReportLogger_->errorReport("Start emptyFIFOs: SR0 = 0x" + toHexString((int)fecAccess->getFecRingSR0(indexFecRing)), LOGINFO) ;
  
  try {
    fecAccess->getFecRingDevice(index)->emptyFifoReceive() ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport("Problem during the emptyed of the FIFO receive", e, LOGERROR) ;
  }
  try {
    fecAccess->getFecRingDevice(index)->emptyFifoReturn() ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport("Problem during the emptyed of the FIFO return", e, LOGERROR) ;
  }
  try {
    fecAccess->getFecRingDevice(index)->emptyFifoTransmit() ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport("Problem during the emptyed of the FIFO transmit", e, LOGERROR) ;
  }
}

void FecSupervisor::ringdiag (int ringslot) 
{
  // List of FEC, ring found

  if (fecAccess_ == NULL) {
    std::stringstream temp ; 
    temp << "Null fecAccess in rigdiag slot " << ringslot;
    errorReportLogger_->errorReport (temp.str(), LOGUSERINFO) ;
    return;
  }
  std::list<keyType> *fecList = fecAccess_->getFecList() ;
  if ((fecList == NULL) || (fecList->empty())) {
    std::stringstream temp ; 
    temp << "Null or empty fecListin rigdiag slot " << ringslot;
    errorReportLogger_->errorReport (temp.str(), LOGUSERINFO) ;
    return;
  }  

  // find the ring
  for (std::list<keyType>::iterator it = fecList->begin() ; it != fecList->end() ; it ++) {
    keyType index = *it;
    if (getRingKey(index) == ringslot) {
      unsigned long sr0 = fecAccess_->getFecRingSR0(index);
      std::stringstream temp ; 
      temp << "ringdiag: slot "<<ringslot<< " SR0 " << std::hex << sr0;
      errorReportLogger_->errorReport (temp.str(), LOGUSERINFO) ;
      if (sr0&FEC_SR0_LINKINITIALIZED) {
	//  we can access the ring
	try {
	  std::list<keyType> *listCCU = fecAccess_->getCcuList(*it) ;
          if (listCCU == 0) {
	   std::stringstream msgError;
	   msgError << "Got a null listCCU for ring " << ringslot;
           errorReportLogger_->errorReport (msgError.str(),LOGUSERINFO) ;
	   break;
	  }
	  if (listCCU->size() > 0) {
	    for (std::list<keyType>::iterator ccuit = listCCU->begin() ; ccuit != listCCU->end() ; ccuit ++) {
	      try {
		std::stringstream msgError;
		msgError << "CCU " << getCcuKey(*ccuit) << ": SRA = 0x"<<std::hex << (int)fecAccess_->getCcuSRA(*ccuit) << " SRB = 0x"<<(int)fecAccess_->getCcuSRB(*ccuit) << " SRC = 0x"<<(int)fecAccess_->getCcuSRC(*ccuit) << " SRD = 0x"<<(int)fecAccess_->getCcuSRD(*ccuit) << " SRE = 0x"<<(int)fecAccess_->getCcuSRE(*ccuit) << " SRF = 0x"<<(int)fecAccess_->getCcuSRF(*ccuit); 
		errorReportLogger_->errorReport (msgError.str(),LOGUSERINFO) ;
		std::stringstream msgError2;
		msgError2 << "CCU " << getCcuKey(*ccuit) << ": CRA = 0x"<<std::hex << (int)fecAccess_->getCcuCRA(*ccuit) << " CRB = 0x"<<(int)fecAccess_->getCcuCRB(*ccuit) << " CRC = 0x"<<(int)fecAccess_->getCcuCRC(*ccuit) << " CRD = 0x"<<(int)fecAccess_->getCcuCRD(*ccuit) << " CRE = 0x"<<(int)fecAccess_->getCcuCRE(*ccuit); 
		errorReportLogger_->errorReport (msgError2.str(),LOGUSERINFO) ;
	      }
	      catch (FecExceptionHandler &e) {
		std::stringstream msgError ; 
		msgError << "Unable to read CCU registers" << fecSlot_.toString() << " ring " << getRingKey(*ccuit) << " CCU " << getCcuKey(*ccuit) ;
		errorReportLogger_->errorReport (msgError.str(), e, LOGUSERINFO) ;
	      }

	    }
	  }
	}
	catch (FecExceptionHandler &e) {
	  std::stringstream msgError ; msgError << "Unable to get the list of CCUs" << fecSlot_.toString() << " ring " << getRingKey(*it) ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	}
	break;
      }
    }
  }
}
