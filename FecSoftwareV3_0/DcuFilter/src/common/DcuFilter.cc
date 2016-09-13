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

// mkdir
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h> // time / ctime functions 

// XDAQ includes
#include "toolbox/fsm/FailedEvent.h"

// STD includes
#include <iostream>
#include <stdexcept>
#include <fstream>

// FecSupervisor includes
#include "DeviceFactoryInterface.h"

#include "datatypes.h"
#include "tscTypes.h"

#include "keyType.h"
//#include "HashTable.h"

// XDAQ includes
#include "xdaqTrackerDcuDefs.h" 
#include "DcuFilter.h"

// FEC includes
#include "FecExceptionHandler.h"
#include "stringConv.h"
  
#define PVSSSWITCH
#define DEBUGPVSSSWITCH

// Run number
#define RUNNUMBERDPE ":tkRunNumber.:_original.._value"
#define RUNNUMBERDPEPARTITION ":tkRunNumber_PARTITION.:_original.._value"

#define TKCC
#ifdef TKCC
// at the end of dcu scan (1 = ok -1 = not ok)
#  define DCUPSUMAPRESULT ":tkCC_dcuScanDone.:_original.._value" 
#  define DCUPSUMAPRESULTPARTITION ":tkCC_dcuScanDone_PARTITION.:_original.._value" 
// --> number of scanned control channels
#  define DCUPSUMAPPROGRESSCG ":tkCC_progressControl.:_original.._value"
#  define DCUPSUMAPPROGRESSCGPARTITON ":tkCC_progressControl_PARTITION.:_original.._value"
// --> number of scanned psus 
#  define DCUPSUMAPPROGRESSPG ":tkCC_progressPsu.:_original.._value"
#  define DCUPSUMAPPROGRESSPGPARTITION ":tkCC_progressPsu_PARTITION.:_original.._value"
// --> message of run type to be set in PVSS
#  define RUNTYPEMESSAGE ":tkCC_messages.:_original.._value"
#  define RUNTYPEMESSAGEPARTITION ":tkCC_messages_PARTITION.:_original.._value"
#else
#  define RUNNUMBERDPE ""
#  define RUNNUMBERDPEPARTITION ""
#  define DCUPSUMAPRRESULT ""
#  define DCUPSUMAPRRESULTPARTITION ""
#  define DCUPSUMAPPROGRESS ""
#  define DCUPSUMAPPROGRESSPARTITION ""
#  define RUNTYPEMESSAGE ""
#  define RUNTYPEMESSAGEPARTITION ""
#endif

// Definition of the PVSS Name
// trkdcs01
#define DISTRIBUTIONDPNAMECCU ":DCU_"
#define DISTRIBUTIONDPNAMEFEH ":DCU_"
#define DCUIDDPNAME           ".DCUID:_original.._value"
#define TIMESTAMPDPNAME       ".Timestamp:_original.._value"
#define STATUSDPNAME          ".Status:_original.._value"
#define TSIDPNAME             ".TSi:_original.._value"
#define V250DPNAME            ".V250:_original.._value"
#define V125DPNAME            ".V125:_original.._value"
#define IDPNAME               ".I:_original.._value"
#define THYBDPNAME            ".THyb:_original.._value"
#define DETIDDPNAME           ".DetID:_original.._value"
#define TDCUDPNAME            ".TDCU:_original.._value"
#define CONSTRIDDPNAME        ".ConstrID:_original.._value"
#define QUALITYDPNAME         ".Quality:_original.._value"
#define TDPNAME               ".T:_original.._value"

// Check if connected and no interlock
#define STATUSCONNECTED       ".actual.status:_original.._value"
#define INTERLOCK             ".actual.remIlk:_original.._value"

// By tagging 1 in this database then the computation of the DCU is performed
// The partition is extracted from the conversion factors, if for any reason
// the partition cannot be determined then the compute of the whole Tracker
// is asked
#define COMPUTETIB            ":tkDcu_updateTIB.updated:_original.._value"
#define COMPUTETID            ":tkDcu_updateTIB.updated:_original.._value"
#define COMPUTETOB            ":tkDcu_updateTOB.updated:_original.._value"
#define COMPUTETECPLUS        ":tkDcu_updateTECplus.updated:_original.._value"
#define COMPUTETECMINUS       ":tkDcu_updateTECminus.updated:_original.._value"
#define COMPUTETRACKER        ":tkDcu_updateTracker.updated:_original.._value"

// Datapoint name for the DCU 
#define DPESETTINGSON  ".settings.onOff:_original.._value"
#define DPESTATUS      ".actual.isOn:_original.._value"
#define VALUEONGET     "TRUE"
#define VALUEOFFGET    "FALSE"
#define VALUEONSET     "TRUE"
#define VALUEOFFSET    "FALSE"
#define COMMA          ","
// Add this information for power group (channel is always 000 for them)
#define PGCHANNEL      "/channel000"
// Enable channel "remote" access on control group (value = TRUE)
#define CGGLOBALON       ".settings.globalOnEn:_original.._value"

/** MACRO for XDAQ
 */
XDAQ_INSTANTIATOR_IMPL( DcuFilter );

// XML File unknown
#define UNKNOWNFILE "Unknown"

// For PSU / DCU Map
#define MAXTIMEOUTPSUSWITCHON 50
#define FILEOUTPUTPUSONERROR  "/tmp/PSUErrorOn.txt"
#define CONTROLCHANNELSSTART "CONTROL CHANNELS"
#define POWERGROUPSSTART     "POWER GROUPS"

#define DCUFILTERPARTITIONNONE "NONE"

/** This constructor is used in order to initialise the different exported parameters parameters. 
 * State Initialise
 * Exported parameters:
 * <ul>
 * <li><i>fireItemAvailable(std::string("UploadFile"),&doUploadInFile_)</i>: upload in a file
 * <li><i>fireItemAvailable(std::string("UploadDatabase"),&doUploadInDatabase_)</i>: upload the DCU readout into the configuration database
 * <li><i>fireItemAvailable(std::string("UploadPVSS"),&doUploadPVSS_)</i>: upload the DCU readout (converted values) to PVSS
 * <li><i>fireItemAvailable(std::string("Conversion"),&doConversion_)</i>: make the conversion between ADC count and real values
 * <li><i>fireItemAvailable(std::string("ConversionFromFile"),&conversionFromFile_)</i>: Conversion values coming from a file or the database
 * <li><i>fireItemAvailable(std::string("ConversionXMLFile"),&xmlConversionInputFile_)</i>: if the conversion is done from a file, what is the name
 * <li><i>fireItemAvailable(std::string("DbLogin"),&dbLogin_)</i>: Login of the database
 * <li><i>fireItemAvailable(std::string("DbPassword"),&dbPasswd_)</i>: password of the database
 * <li><i>fireItemAvailable(std::string("DbPath"),&dbPath_)</i>: path of the database
 * <li><i>fireItemAvailable(std::string("PartitionName"),&partitionName_)</i>: Partition name
 * <li><i>fireItemAvailable(std::string("DisplayDcuList"),&displayDcuList_)</i>: Display DCU list into the mainpage
 * <li><i>fireItemAvailable(std::string("TimePVSSSend"),&timePVSSSend_)</i>: Time between 2 DCUs send to PVSS in case of the channels did not change
 * <li><i>fireItemAvailable(std::string("PourcentDifferenceForPVSS"),&pourcentDifferenceForPVSS_)</i>: to set a level of difference in the DCU raw data to be sent to PVSS. If the level is reach in the DCU raw data then the values are sent to PVSS.
 * <li><i>fireItemAvailable(std::string("BlockDpToBeSent"),&blockDpToBeSent_)</i>: the performance of the send of DPs set in block but the block should be limitated. So this value give the maximum size.
 * <li><i>fireItemAvailable(std::string("DirectoryDcuDataName"),&directoryDcuDataName_)</i>: directory where the file with raw should be put
* <li><i>->fireItemAvailable(std::string("TKCCTest"),&tkccTest_);</i>: is it a tkcc test?
 * <lu>
 * Here the command to test the access to a datapoints:
 * <ul><li>${XDAQ_ROOT}/daq/psx/scripts/dpGetNames.sh "dist_1:*"  daqpic01:1972
 * <lu>
 */
DcuFilter::DcuFilter ( xdaq::ApplicationStub* stub ): TrackerCommandSender(stub), JsInterface(stub) {

  // -----------------------------------------------
  displayDebugMessage_ = false ; // All messages
  ignoreErrorMessage_ = false ; // should be false by default, do not display as error the messages

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
				   DIAG_TRACKERDCUFILTER_SUBSYSTEMID);

  // Declare the application as a user application
  DIAG_DECLARE_USER_APP ;
  errorReportLogger_ = new ErrorReportLogger ("DCU Filter " + toString(getApplicationDescriptor()->getInstance()), displayDebugMessage_, true, 
					      LOGDEBUG, diagService_) ;
#else
  errorReportLogger_ = new ErrorReportLogger ("DCU Filter " + toString(getApplicationDescriptor()->getInstance()), displayDebugMessage_, true, 
					      LOGDEBUG) ;
#endif

  // Software tag version from CVS
  softwareTagVersion_ = FecExceptionHandler::getCVSTag() ;

  // Software status

  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Constructor" ;

  errorOccurs_ = false ;

  // Initialise the objects for the accesses
  fecDeviceFactory_ = NULL ;

  // DCU/PSU mapping
  doUploadPSUDCUMapFile_  = true ; // Upload the DCU/PSU map into a file
  psuDcuMapFileName_ = "/tmp/PSUDCUMap.txt" ; // output file

  // ------------------------------------------------------------------
  // export paramater to run control, direct binding
  // run control can read and write all exported variables
  // directly (no need fro get and put from the user).

  // DCU / PSU channels detection
  dcuPSUChannelDetection_ = false ;
  // display DCU list
  displayDcuList_ = false ; //displayDebugMessage_ ;

  // Upload
  directoryDcuDataName_ = "/tmp" ; 
  doUploadInFile_     = false ; 
  doUploadInDatabase_ = false ; 
  doUploadPVSS_       = false ; 
  timePVSSSend_       = 100   ; 
  blockDpToBeSent_    = 40    ; 
  pvssClassName_      = PVSS_APPLICATION_DESCRIPTOR ; 
  soapActionString_   = SOAPACTIONSTRING ; 
  pvssInstance_       = PVSS_APPLICATION_INSTANCE ; 
  pourcentDifferenceForPVSS_ = 10 ; 
  pvssPoint_ = "SliceTestDCUS" ; 
  runNumberSent_ = 0 ; runNumber_ = 0 ; 
#ifdef TKCC
  tkccTest_ = true ; 
#else
  tkccTest_ = false ; 
#endif

  subDetector_ = "" ; 
  switchOffControlGroup_ = false ; 

  // Conversion
  doConversion_           = false       ; 
  conversionFromFile_     = false       ; 
  xmlConversionInputFile_ = UNKNOWNFILE ; 

  // $ENV_CMS_TK_FEC_ROOT/FecSupervisor/xml
  char *basic = getenv ("ENV_TRACKER_DAQ") ;
  if (basic == NULL) basic=getenv ("ENV_CMS_TK_FEC_ROOT") ;

  if (basic != NULL) {
    std::ostringstream conversionFile ;
    conversionFile << basic << "/config/DcuConversionFactors.xml" ;

    std::ifstream fichier (conversionFile.str().c_str()) ;

    if (fichier) xmlConversionInputFile_ = conversionFile.str() ;
    else 
      errorReportLogger_->errorReport ("Conversion file " + conversionFile.str() + " not existing", LOGWARNING) ;
  }
  fileConversionParsed_ = UNKNOWNFILE ;

  // Database path
  dbLogin_  = "nil" ; 
  dbPasswd_ = "nil" ; 
  dbPath_   = "nil" ; 
  partitionName_ = DCUFILTERPARTITIONNONE ; 

  //basic=getenv ("ENV_CMS_TK_PARTITION") ;
  //if (basic != NULL) partitionName_ = std::string(basic) ;
  //else partitionName_ = DCUFILTERPARTITIONNONE ;

  databaseAccess_ = false ;
  sendruntopvss_ = false;

#ifdef DATABASE
  std::string dbLogin = "nil", dbPasswd = "nil", dbPath = "nil" ;
  DbFecAccess::getDbConfiguration (dbLogin, dbPasswd, dbPath) ;
  dbLogin_ = dbLogin ; dbPasswd_ = dbPasswd ; dbPath_ = dbPath ;

  if (dbLogin_ != "nil" && dbPasswd != "nil" && dbPath_ != "nil") databaseAccess_ = true ;
#endif

  // If the database is not set then set the file as the input for the conversion factor
  if (!databaseAccess_) conversionFromFile_ = true ;

  // ---------------------------------- State machine
  // Initiliase state
  fsm_.addState ('I', "Initial") ;
  fsm_.addState ('H', "Halted");
  fsm_.addState ('C', "Configured");
  fsm_.addState ('E', "Enabled");
  fsm_.addStateTransition ('I','H', INITIALISE, this, &DcuFilter::initialiseAction);
  fsm_.addStateTransition ('H','I', DESTROY, this, &DcuFilter::initialiseAction);
  fsm_.addStateTransition ('H','C', CONFIGURE, this, &DcuFilter::configureAction);
  fsm_.addStateTransition ('C','H', HALT, this, &DcuFilter::haltAction);
  fsm_.addStateTransition ('C','E', ENABLE, this, &DcuFilter::enableAction);
  fsm_.addStateTransition ('E','H', HALT, this, &DcuFilter::stopAction);
  fsm_.addStateTransition ('E','C', STOP, this, &DcuFilter::stopAction);

  fsm_.setFailedStateTransitionAction( this, &DcuFilter::failedTransition );

  fsm_.setInitialState('I');
  fsm_.reset();

  // Bind SOAP callbacks for control messages
  xoap::bind (this, &DcuFilter::fireEvent, INITIALISE, XDAQ_NS_URI);
  xoap::bind (this, &DcuFilter::fireEvent, CONFIGURE, XDAQ_NS_URI);
  xoap::bind (this, &DcuFilter::fireEvent, ENABLE, XDAQ_NS_URI);
  xoap::bind (this, &DcuFilter::fireEvent, HALT, XDAQ_NS_URI);
  xoap::bind (this, &DcuFilter::fireEvent, STOP, XDAQ_NS_URI);
  xoap::bind (this, &DcuFilter::fireEvent, DESTROY, XDAQ_NS_URI);

  // Bind CGI callbacks
  xgi::bind(this, &DcuFilter::dcuFilterInitialise, "dcuFilterInitialise");

  // -----------------------------------------------
  // Bind for the diagnostic system
#ifdef TKDIAG
  // Configuration option for the diag => link on DIAG_CONFIGURE_CALLBACK
  xgi::bind(this, &DcuFilter::configureDiagSystem, "configureDiagSystem");
  //                                   => link on DIAG_APLLY_CALLBACK
  xgi::bind(this, &DcuFilter::applyConfigureDiagSystem, "applyConfigureDiagSystem");

  // For XRelay
  xoap::bind(this, &DcuFilter::freeLclDiagSemaphore, "freeLclDiagSemaphore", XDAQ_NS_URI );
  xoap::bind(this, &DcuFilter::freeGlbDiagSemaphore, "freeGlbDiagSemaphore", XDAQ_NS_URI );

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //For using the centralized command pannel in logreader
  //                                   => link on DIAG_REQUEST_ENTRYPOINT
  xoap::bind(this, &DcuFilter::processOnlineDiagRequest, "processOnlineDiagRequest", XDAQ_NS_URI );

#endif

  // -----------------------------------------------
  // Method called by the first level of diagnostic
  xoap::bind(this, &DcuFilter::dcuFilterRecovery, "dcuFilterRecovery", XDAQ_NS_URI );

  // -----------------------------------------------
  // Nagivation related links
  std::string url = getApplicationDescriptor()->getURN();
  //relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/MoreParameters\" target=\"_blank\">More Parameters</a>", url.c_str()));
  //relatedLinksNavigation_.push_back(toolbox::toString("<a href=http://x5oracle.cern.ch:8080/JSPWiki/Wiki.jsp?page=FECSoftware target=\"_blank\">Documentation</a>", url.c_str()));
  //relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/PVSSTest\" target=\"_blank\">PVSS Test</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/MoreParameters\">More Parameters</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/displayDcuFilter\">Main Page</a>", url.c_str()));
  //relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/displayDcuList\">DCU List</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/displayStateMachine\">State Machine</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/PVSSTest\">PVSSTest</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/ConfigureDiagSystem\">Configure DiagSystem</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=https://twiki.cern.ch/twiki/bin/view/CMS/DCUConversionFactors target=\"_blank\">Documentation</a>", url.c_str())) ;

  // Position in the link, zero means no relation was done
  linkPosition_ = 0 ;

  xgi::bind(this, &DcuFilter::displayInitialisePage,  "MoreParameters" );
  //xgi::bind(this, &DcuFilter::displayRelatedLinks, "FecRelatedLinks");
  xgi::bind(this, &DcuFilter::displayPVSSTest, "PVSSTest");
  xgi::bind(this, &DcuFilter::applyPVSSTest, "applyPVSSTest");
  xgi::bind(this, &DcuFilter::displayConfigureDiagSystem, "ConfigureDiagSystem");
  // -----------------------------------------------
  // DCU List
  xgi::bind(this, &DcuFilter::displayDcuFilter, "displayDcuFilter");
  //xgi::bind(this, &DcuFilter::displayDcuList, "displayDcuList");
  xgi::bind(this, &DcuFilter::dcuListApply, "dcuListApply");
  xgi::bind(this, &DcuFilter::displayDcu, "displayDcu");

  xgi::bind(this, &DcuFilter::dispatch, "dispatch");
  xgi::bind(this, &DcuFilter::displayStateMachine, "displayStateMachine");

  // Counter for the number of values received
  counter_ = 0 ;

  // -----------------------------------------------
  // PVSS communication
  xPditor_ = NULL ;

  // -----------------------------------------------
  // PVSS Test
  dataPVSSWrite_ = true ;
  dataPVSSRead_  = true ;
  dataPointName_  = "" ;
  dataPointValue_ = "" ;
  pvssWriteStatus_ = "No Operation performed" ;
  pvssReadStatus_ = "No Operation performed" ;

  // -----------------------------------------------
  // Upload Thread
  try {

    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Constructor: construction of the work loop for the upload" ;


    sleepUploadWorkLoop_ = true ;
    uploadWorkLoop_ = toolbox::task::bind (this, &DcuFilter::uploadWorkLoop, "uploadWorkLoop") ;
    workDcuFilterLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop("DcuFilterWorkLoop", "waiting");
    if (!workDcuFilterLoop_->isActive()) workDcuFilterLoop_->activate() ;
    mutexVectorQueue_ = new toolbox::BSem(toolbox::BSem::FULL) ;
    mutexRunPvss_ = new toolbox::BSem(toolbox::BSem::FULL) ;
    mutexAppStatus_ = new toolbox::BSem(toolbox::BSem::FULL) ;

    // set this to 20, since we do not get DCUFILTER_REGISTER
    fecSupervisorNumbers_ = 20 ;
    // back to 0
    //fecSupervisorNumbers_ = 0 ;
  }
  catch (xdaq::exception::Exception& e) {
    errorReportLogger_->errorReport ("Cannot start the upload work loop", e, LOGFATAL) ;
    workDcuFilterLoop_ = NULL ;
  }

  // To check the differences from one DCU readout to another one
  checkDCUData_ = false ;
  checkDcuDataFile_ = "/tmp/DCUValuesChanged.txt" ;
  dumpDCUMissing_ = false ;
  dumpDCUMissingFile_ = "/tmp/DCUConversionErrors.txt" ;
  dumpDCUMissingStream_ = NULL ;

  numberOfDcuSentToPVSS_ = 0 ;

  // Datapoint name
  runNumberDpe_ = pvssPoint_.toString() + RUNNUMBERDPE ;
  dcuPsuMapResult_ = pvssPoint_.toString() + DCUPSUMAPRESULT ;
  dcuPsuMapProgressCG_ = pvssPoint_.toString() + DCUPSUMAPPROGRESSCG ;
  dcuPsuMapProgressPG_ = pvssPoint_.toString() + DCUPSUMAPPROGRESSPG ;
  runTypeMessage_ = pvssPoint_.toString() + RUNTYPEMESSAGE ;

  // Export parameters
  //#define JSINTERFACE
#ifdef JSINTERFACE
  getApplicationInfoSpace()->fireItemAvailable(std::string("DCUPSUChannelDetection"),&dcuPSUChannelDetection_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("PSUConfigurationFile"),&psuConfigurationFile_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("DisplayDcuList"),&displayDcuList_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("DirectoryDcuDataName"),&directoryDcuDataName_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("UploadFile"),&doUploadInFile_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("UploadDatabase"),&doUploadInDatabase_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("UploadPVSS"),&doUploadPVSS_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("TimePVSSSend"),&timePVSSSend_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("BlockDpToBeSent"),&blockDpToBeSent_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("PVSSClassName"), &pvssClassName_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("SOAPActionString"), &soapActionString_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("PVSSInstance"), &pvssInstance_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("PourcentDifferenceForPVSS"),&pourcentDifferenceForPVSS_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("PVSSPoint"),&pvssPoint_) ;   // Where the DCS datapoints is set
  getApplicationInfoSpace()->fireItemAvailable(std::string("RunNumber"),&runNumber_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("TKCCTest"),&tkccTest_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("SubDetector"),&subDetector_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("SwitchOffControlGroup"),&switchOffControlGroup_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("Conversion"),&doConversion_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("ConversionFromFile"),&conversionFromFile_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("ConversionXMLFile"),&xmlConversionInputFile_) ;
  //Export the stateName variable
  getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbLogin"),&dbLogin_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbPassword"),&dbPasswd_);  
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbPath"),&dbPath_);  
  getApplicationInfoSpace()->fireItemAvailable(std::string("PartitionName"),&partitionName_);
#else
  getApplicationInfoSpace()->fireItemAvailable(std::string("DCUPSUChannelDetection"),&dcuPSUChannelDetection_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("PSUConfigurationFile"),&psuConfigurationFile_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("DisplayDcuList"),&displayDcuList_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("DirectoryDcuDataName"),&directoryDcuDataName_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("UploadFile"),&doUploadInFile_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("UploadDatabase"),&doUploadInDatabase_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("UploadPVSS"),&doUploadPVSS_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("TimePVSSSend"),&timePVSSSend_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("BlockDpToBeSent"),&blockDpToBeSent_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("PVSSClassName"), &pvssClassName_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("SOAPActionString"), &soapActionString_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("PVSSInstance"), &pvssInstance_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("PourcentDifferenceForPVSS"),&pourcentDifferenceForPVSS_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("PVSSPoint"),&pvssPoint_) ;   // Where the DCS datapoints is set
  getApplicationInfoSpace()->fireItemAvailable(std::string("RunNumber"),&runNumber_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("TKCCTest"),&tkccTest_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("SubDetector"),&subDetector_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("SwitchOffControlGroup"),&switchOffControlGroup_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("Conversion"),&doConversion_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("ConversionFromFile"),&conversionFromFile_) ;
  getApplicationInfoSpace()->fireItemAvailable(std::string("ConversionXMLFile"),&xmlConversionInputFile_) ;
  //Export the stateName variable
  getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbLogin"),&dbLogin_);
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbPassword"),&dbPasswd_);  
  getApplicationInfoSpace()->fireItemAvailable(std::string("DbPath"),&dbPath_);  
  getApplicationInfoSpace()->fireItemAvailable(std::string("PartitionName"),&partitionName_);
#endif


}

/* Destructor
 */
DcuFilter::~DcuFilter ( ) {

  // Stop the work loop
  if (workDcuFilterLoop_ != NULL) {
    try {

      mutexAppStatus_->take();
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destructor: stop and destroy the work loop for the upload" ;
      mutexAppStatus_->give();

      // Delete the workloop for that supervisor
      workDcuFilterLoop_->cancel();
    }
    catch (toolbox::task::exception::Exception &e) {
      errorReportLogger_->errorReport ("Cannot stop the work loop for the upload, incoherence in the software (toolbox::task::exception::Exception): " + std::string(e.what()), LOGERROR) ;
    }
    delete workDcuFilterLoop_ ; workDcuFilterLoop_ = NULL ;
  }

  // Delete the queue
  for (std::list<deviceVector *>::iterator it = listQueue_.begin() ; it != listQueue_.end() ; it ++) {
    deviceVector *vDevice = dynamic_cast<deviceVector *>(*it) ;
    FecFactory::deleteVectorI (*vDevice) ;
    vDevice->clear() ;
    delete vDevice ;
  }

  // Delete the mutex
  delete mutexVectorQueue_ ;

  // Delete the map of the conversion factors
  for (DcuConversionsHashMapType::iterator p = dcuConversionFactorsMap_.begin() ; p != dcuConversionFactorsMap_.end() ; p ++) {
    delete p->second ;
  }

  // Delete the array of DCU conversion factors
  for (DcuValuesReceivedType::iterator p = dcuValuesReceived_.begin(); p != dcuValuesReceived_.end(); p++) {
    delete p->second ;
  }
  for (DcuValuesReceivedType::iterator it = dcuValuesSentToPVSS_.begin() ; it != dcuValuesSentToPVSS_.end() ; it ++) {
    delete it->second ;
  }
  for (DcuValuesReceivedType::iterator it = dcuValuesSentToDB_.begin() ; it != dcuValuesSentToDB_.end() ; it ++) {
    delete it->second ;
  }

  // Disconnect the database if it is set
  if (fecDeviceFactory_ != NULL) {
    mutexAppStatus_->take();
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destructor: destroy the FEC factory" ;
    mutexAppStatus_->give();

    delete fecDeviceFactory_ ;
    fecDeviceFactory_ = NULL ;
    //databaseAccess_   = false ; => set in the web page
  }
  
  if (tkDcuConversionFactory_ != NULL) {
    mutexAppStatus_->take();
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destructor: destroy the DCU factory" ;
    mutexAppStatus_->give();

    delete tkDcuConversionFactory_ ;
    tkDcuConversionFactory_ = NULL ;
  }

  // delete the access to PVSS
  mutexAppStatus_->take();
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destructor: destroy the PVSS connection" ;
  mutexAppStatus_->give();
  delete xPditor_ ;
  xPditor_ = NULL ;

  // Close the files
  if (dumpDCUMissingStream_ != NULL) {
    dumpDCUMissingStream_->close() ;
    delete dumpDCUMissingStream_ ; dumpDCUMissingStream_ = NULL ;
  } 
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                 Finite state machine                                                         */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Create the differents access database
 * \warning this method delete first all the possible accesses
 */
void DcuFilter::initialiseAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  // Datapoint name by default
  runNumberDpe_ = pvssPoint_.toString() + RUNNUMBERDPE ;
  dcuPsuMapResult_ = pvssPoint_.toString() + DCUPSUMAPRESULT ;
  dcuPsuMapProgressCG_ = pvssPoint_.toString() + DCUPSUMAPPROGRESSCG ;
  dcuPsuMapProgressPG_ = pvssPoint_.toString() + DCUPSUMAPPROGRESSPG ;
  runTypeMessage_ = pvssPoint_.toString() + RUNTYPEMESSAGE ;

  // when you upload the DCU eaw data in file, append to the directoy given the partition name
  std::string directoryDcuDataName = directoryDcuDataName_.toString() ;
  std::string::size_type ipos = directoryDcuDataName.find(partitionName_.toString());
  if (ipos == std::string::npos) {
    std::stringstream directory ; directory << directoryDcuDataName_.toString() << "/" << partitionName_.toString() ;
    directoryDcuDataName_ = directory.str() ;
  }

#ifdef TKDIAG
  // Apply the diagnostic fire items value to internal variables and check validity
  DIAG_EXEC_FSM_INIT_TRANS ;
#endif

  // Fix the partition name
  if ( (partitionName_ == "") || (partitionName_ == DCUFILTERPARTITIONNONE) ) {
    char *basic=getenv ("ENV_CMS_TK_PARTITION") ;
    if (basic != NULL) partitionName_ = std::string(basic) ;
  }

  // Originator for the PSI interface
  std::ostringstream originator ; originator << "DcuFilter" << partitionName_.toString() << getApplicationDescriptor()->getInstance() ;
  originatorStr_ = originator.str() ;
  if ((partitionName_ != "") && databaseAccess_)
    errorReportLogger_->setStrProcess("DCU Filter " + toString(getApplicationDescriptor()->getInstance()) + ", partition " + partitionName_.toString()) ;

  std::stringstream msgInfo ; msgInfo << "PVSS originator is " << originatorStr_ ;
  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;

  // Create the access to PVSS (if it is already existing, it was created by hand during the initialisation web page)
  if (doUploadPVSS_ && (xPditor_ == NULL)) {
    try {
      if (pvssClassName_.toString() != "") {
	std::stringstream msgInfo ; msgInfo << "Access to PVSS performed on class " << pvssClassName_.toString() 
					    << " instance " << pvssInstance_.toString() << " (" << soapActionString_.toString() << ")" ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	mutexAppStatus_->take();
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Initialise: creation of the connection to PVSS" ;
	mutexAppStatus_->give();
	xPditor_ = new XPditor ( this, originatorStr_, pvssClassName_.toString(), pvssInstance_, soapActionString_.toString()) ;
      }
      else {
	errorReportLogger_->errorReport ("PVSS URL empty: cannot open a connection to PSX", LOGERROR) ;
      }
    }
    catch (xdaq::exception::Exception& e) {

      doUploadPVSS_ = false ;
      std::stringstream msgError ; msgError << "Unable to access PVSS on class " << pvssClassName_.toString() << " instance " << pvssInstance_.toString() 
					    << " (" << soapActionString_.toString() << ") "
					    << "=> (error = " << e.what() << ")" ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
    }
  }

  // Read the conversion factors
  if (doConversion_) { getConversionFactors() ; }

  // Dump the errors in file
  if (dumpDCUMissing_ && (dumpDCUMissingStream_ == NULL) ) 
    dumpDCUMissingStream_ = new std::ofstream(dumpDCUMissingFile_.c_str()) ;

  // Dump the DCU values in database or file
  if (doUploadInDatabase_ || doUploadInFile_) {
    if (fecDeviceFactory_ == NULL) {
#ifdef DATABASE
      if (doUploadInDatabase_ && databaseAccess_) createDatabaseAccess () ;
      else 
#endif
	{
	  doUploadInDatabase_ = false ;
	  if (doUploadInFile_) fecDeviceFactory_ = new FecDeviceFactory ( ) ;
	}
    }
  }

  // If database is present then try to find the sub-detector
  if ( (tkDcuConversionFactory_ != NULL) && (tkDcuConversionFactory_->getDbUsed()) ) {

    subDetector_ = "" ; 
      
    // retreive the sub-detector
    char hostname[100] ;
    if (!gethostname(hostname,100)) {
      try {
	std::string subDetector ;
	unsigned int crateNumber ;
	tkDcuConversionFactory_->getAllTkIdFromHostname(hostname) ;
	tkDcuConversionFactory_->getSubDetectorCrateNumberFromHostname(hostname,subDetector,crateNumber) ;

	if (subDetector == "TEC+") subDetector_ = "TECplus" ;
	else if (subDetector == "TEC-") subDetector_ = "TECminus" ;
	else if (subDetector == "TIB/TID") subDetector_ = "TIB" ;
	else if (subDetector == "TID") subDetector_ = "TIB" ;
	else if (subDetector == "TOB") subDetector_ = "TOB" ;
	else subDetector_ = subDetector ;
	
	// change all DPs names
	std::string dpe = RUNNUMBERDPEPARTITION ;
	std::string::size_type ideb = dpe.find("PARTITION") ;
	if (ideb != std::string::npos) {
	  unsigned int iend = ideb + strlen("PARTITION") ;
	  runNumberDpe_ = pvssPoint_.toString() + dpe.substr(0,ideb) + subDetector_.toString() + dpe.substr(iend,dpe.length());
	}
	
	dpe = DCUPSUMAPRESULTPARTITION ;
	ideb = dpe.find("PARTITION") ;
	if (ideb != std::string::npos) {
	  unsigned int iend = ideb + strlen("PARTITION") ;
	  dcuPsuMapResult_ = pvssPoint_.toString() + dpe.substr(0,ideb) + subDetector_.toString() + dpe.substr(iend,dpe.length());
	}

	dpe = DCUPSUMAPPROGRESSCGPARTITON ;
	ideb = dpe.find("PARTITION") ;
	if (ideb != std::string::npos) {
	  unsigned int iend = ideb + strlen("PARTITION") ;
	  dcuPsuMapProgressCG_ = pvssPoint_.toString() + dpe.substr(0,ideb) + subDetector_.toString() + dpe.substr(iend,dpe.length());
	}
	
	dpe = DCUPSUMAPPROGRESSPGPARTITION ;
	ideb = dpe.find("PARTITION") ;
	if (ideb != std::string::npos) {
	  unsigned int iend = ideb + strlen("PARTITION") ;
	  dcuPsuMapProgressPG_ = pvssPoint_.toString() + dpe.substr(0,ideb) + subDetector_.toString() + dpe.substr(iend,dpe.length());
	}

	dpe = RUNTYPEMESSAGEPARTITION ;
	ideb = dpe.find("PARTITION") ;
	if (ideb != std::string::npos) {
	  unsigned int iend = ideb + strlen("PARTITION") ;
	  runTypeMessage_ = pvssPoint_.toString() + dpe.substr(0,ideb) + subDetector_.toString() + dpe.substr(iend,dpe.length());
	}
      }
      catch (FecExceptionHandler e) {
	errorReportLogger_->errorReport ("Cannot retreive information concerning FED ID / sub-detector / crate ID", e, LOGERROR) ;
	std::stringstream temp ; temp << "Cannot retreive information concerning FED ID / sub-detector / crate ID"
				      << std::endl << e.what() 
				      << std::endl ;
	lastErrorMessage_ = temp.str() ;
	errorOccurs_ = true ;
      }
      catch (oracle::occi::SQLException e) {
	errorReportLogger_->errorReport ("Cannot retreive information concerning FED ID / sub-detector / crate ID", e, LOGERROR) ;
	std::stringstream temp ; temp << "Cannot retreive information concerning FED ID / sub-detector / crate ID"
				      << std::endl << e.what() 
				      << std::endl ;
	lastErrorMessage_ = temp.str() ;
	errorOccurs_ = true ;
      }
    }
    else {
      errorReportLogger_->errorReport ("Error during retreive of network hostname, cannot retreive information concerning FED ID / sub-detector / crate ID (gethostname error)", LOGERROR) ;
      std::stringstream temp ; temp << "Error during retreive of network hostname, cannot retreive information concerning FED ID / sub-detector / crate ID (gethostname error)"
				    << std::endl ;
      lastErrorMessage_ = temp.str() ;
      errorOccurs_ = true ;
    }
  }
}

/** Configure action
 */
void DcuFilter::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  errorReportLogger_->errorReport ("configureAction", LOGDEBUG) ;

  mutexRunPvss_->take();
  sendruntopvss_ = true;
  mutexRunPvss_->give();

  // Make the DCU/PSU map if asked
  if (dcuPSUChannelDetection_) detectDcuPSUChannels ( ) ;
}

/** Configure action
 */
void DcuFilter::enableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  errorReportLogger_->errorReport ("start enableAction", LOGDEBUG) ;

  mutexRunPvss_->take();
  sendruntopvss_ = true;
  mutexRunPvss_->give();

  errorReportLogger_->errorReport ("end enableAction", LOGDEBUG) ;

}

/** Stop action (end of run)
 */
void DcuFilter::stopAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  errorReportLogger_->errorReport ("stopAction", LOGDEBUG) ;
}

/** Halt action
 */
void DcuFilter::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  errorReportLogger_->errorReport ("haltAction", LOGDEBUG) ;
}

/** Destroy all hardware accesses
 */
void DcuFilter::destroyAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  if (xPditor_ != NULL) { 
    mutexAppStatus_->take();
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destroy: delete the connection to PVSS" ;
    mutexAppStatus_->give();
    delete xPditor_ ; xPditor_ = NULL ; 
  }
  if (fecDeviceFactory_ != NULL) { 
    mutexAppStatus_->take();
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destroy: delete the FEC factory" ;
    mutexAppStatus_->give();
    delete fecDeviceFactory_ ; fecDeviceFactory_ = NULL ; 
  }
  if (tkDcuConversionFactory_ != NULL) { 
    mutexAppStatus_->take();
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destroy: delete the DCU factory" ;
    mutexAppStatus_->give();
    delete tkDcuConversionFactory_ ; tkDcuConversionFactory_ = NULL ; 
  }
  mutexAppStatus_->take();
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Destroy: destroy DCU values received" ;
  mutexAppStatus_->give();
  for (DcuValuesReceivedType::iterator it = dcuValuesReceived_.begin() ; it != dcuValuesReceived_.end() ; it ++) {
    delete it->second ;
  }
  dcuValuesReceived_.clear() ;
  for (DcuValuesReceivedType::iterator it = dcuValuesSentToPVSS_.begin() ; it != dcuValuesSentToPVSS_.end() ; it ++) {
    delete it->second ;
  }
  dcuValuesSentToPVSS_.clear() ;
  for (DcuValuesReceivedType::iterator it = dcuValuesSentToDB_.begin() ; it != dcuValuesSentToDB_.end() ; it ++) {
    delete it->second ;
  }
  dcuValuesSentToDB_.clear() ;
}


/* ************************************************************************************************************ */
/*                                                                                                              */
/*                              Database                                                                        */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Create an access to the database. First check if the env. variables are set (DBCONFLOGIN,
 * DBCONFPASSWD, DBCONFPATH. If not, check the export params
 */
void DcuFilter::createDatabaseAccess ( ) {
  
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
      mutexAppStatus_->take();
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Connection access to the database for the FEC factory" ;
      mutexAppStatus_->give();
      fecDeviceFactory_ = new FecDeviceFactory ( dbLogin_, dbPasswd_, dbPath_ ) ;
      
      databaseAccess_ = true ;
    }
    catch (oracle::occi::SQLException &e) {
      
      std::stringstream msgError ; msgError << "Unable to connect the database: " << dbLogin_.toString() << "/" <<  dbPasswd_.toString() << "@" << dbPath_.toString() ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
      lastErrorMessage_ = msgError.str() ;
      errorOccurs_ = true ;
      
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

  conversionFromFile_ = !databaseAccess_ ;
  doUploadInDatabase_ = doUploadInDatabase_ && databaseAccess_ ;
}

/** Retreive the parameters from the database or the file for the conversion
 */
void DcuFilter::getConversionFactors ( ) {

  // No conversion for the time being
  doConversion_ = false ;

#ifdef DATABASE
  // Conversion factor from database
  if (!conversionFromFile_ && databaseAccess_) {
    
    // Retreive the conversion factors from the database
    try {
      mutexAppStatus_->take();
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Conversion factors: Connection access to the database for the DCU factory" ;
      mutexAppStatus_->give();
      tkDcuConversionFactory_ = new DeviceFactory (dbLogin_, dbPasswd_, dbPath_) ;

      //      tkDcuConversionFactory_->addConversionPartition(partitionName_) ;
      // try passing ALL - ag 14-oct-2011

      tkDcuConversionFactory_->addConversionPartition("ALL") ;
      // replace with loop on partitions -- ag 11-oct-2011 
      // doesn't work: null domdocument on the second partition ...
      /*   
      char *basic=getenv ("ENV_CMS_TK_PARTITION") ;
      std::stringstream msgWarn ;
      errorReportLogger_->errorReport ("Before loop on partitions", LOGWARNING) ;      
      std::stringstream msgWarn2;
      msgWarn2 << "partition list " << basic;
      errorReportLogger_->errorReport (msgWarn2.str(), LOGWARNING) ;
      
      msgWarn << "Getting conversion factors for ";
      if (basic==0) {
	msgWarn << partitionName_.toString();        
	tkDcuConversionFactory_->addConversionPartition(partitionName_) ;
      } else {
	char *p0,*p1;
	p0=basic;
	while (p1=strchr(p0,':')){
	  *p1='\0';
	  msgWarn << p0 << " ";
	  errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING) ;
	  tkDcuConversionFactory_->addConversionPartition(p0) ;
	  p0=p1+1;
	  *p1=':';
	} 
	msgWarn << p0;
	tkDcuConversionFactory_->addConversionPartition(p0) ;
      }
      errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING) ;
      */   
      doConversion_ = true ;
      dcuConversionFactorsMap_ = tkDcuConversionFactory_->getConversionFactors() ;
      std::stringstream msgInfo ; msgInfo << "Found " << dcuConversionFactorsMap_.size() << " DCU conversion factors" ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
      
      // Get the default conversion factors for the FEH
      try {
	// For FEH
	dcuConversionFactorsMap_[0] = tkDcuConversionFactory_->getTkDcuConversionFactors ( 0 ) ;
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger_->errorReport ("Unable to retreive the default conversion factors for DCU on FEH", e, LOGERROR) ;
	lastErrorMessage_ = "Unable to retreive the default conversion factors for DCU on FEH" ;
	errorOccurs_ = true ;
      }
      // Get the default conversion factors for the DCU on CCU
      try {
	// For DCU on CCU
	dcuConversionFactorsMap_[1] = tkDcuConversionFactory_->getTkDcuConversionFactors ( 1 ) ;
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger_->errorReport ("Unable to retreive the default conversion factors for DCU on CCU", e, LOGERROR) ;
	lastErrorMessage_ = "Unable to retreive the default conversion factors for DCU on CCU" ;
	errorOccurs_ = true ;
      }
    }
    catch (FecExceptionHandler &e) {
      
      errorReportLogger_->errorReport ("Unable to retreive the conversion factors from the current database", e, LOGERROR) ;
      lastErrorMessage_ = "ERROR: Unable to retreive the conversion factors from the current database" ;
      errorOccurs_ = true ;
      
      conversionFromFile_ = true ;
    }
    catch (oracle::occi::SQLException &e) {
      std::stringstream msgError ; msgError << "Unable to connect the database: " << dbLogin_.toString() << "/" <<  dbPasswd_.toString() << "@" << dbPath_.toString() ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
      lastErrorMessage_ = msgError.str() ;
      errorOccurs_ = true ;

      databaseAccess_ = false ;
      dbLogin_ = "nil" ;
      dbPasswd_ = "nil" ;
      dbPath_ = "nil" ;
    }
  }
#else
  databaseAccess_ = false ;
#endif

  // Conversion factory from file
  if (conversionFromFile_ && xmlConversionInputFile_ != UNKNOWNFILE) {

    try {
      mutexAppStatus_->take();
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Conversion factors: retreive conversion factors from file " + xmlConversionInputFile_.toString() ;
      mutexAppStatus_->give();
      tkDcuConversionFactory_ = new DeviceFactory ( ) ;
      tkDcuConversionFactory_->setDcuConversionInputFileName (xmlConversionInputFile_.toString()) ;
      doConversion_ = true ;
      fileConversionParsed_ = xmlConversionInputFile_.toString() ;
    }
    catch (FecExceptionHandler &e) {
      
      std::stringstream msgError ; msgError << "ERROR: Unable to retreive the conversion factors from file " << xmlConversionInputFile_.toString() ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
      lastErrorMessage_ = msgError.str() ;
      errorOccurs_ = true ;

      conversionFromFile_ = false ;
    }
  }

  // Message
  if (!doConversion_) {
    errorReportLogger_->errorReport ("The conversion between ADC count and real values will not be applied", LOGERROR) ;
    lastErrorMessage_ = "The conversion between ADC count and real values will not be applied" ;
    errorOccurs_ = true ;
  }
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   I2O message                                                                */
/*                                                                                                              */
/* ************************************************************************************************************ */
/** \brief gets the I2O messages
 */
toolbox::mem::Reference* DcuFilter::senderTrackerCommandHandler(PI2O_TRACKER_COMMAND_MESSAGE_FRAME com) {

  toolbox::mem::Reference* ackRef = NULL ;
  PI2O_TRACKER_COMMAND_MESSAGE_FRAME reply ;

  errorReportLogger_->errorReport ("receive an i2o message", LOGDEBUG) ;

  if ((com->System == SYST_TRACKER) && 
      (com->SubSystem == SUB_SYST_FEC ) ) {

    switch (com->Command) {

    case DCUFILTER_REGISTER: {
      mutexAppStatus_->take();
      xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": I2O message: registration of a FEC supervisor" ;
      mutexAppStatus_->give();
      // Answer 
      ackRef = allocateTrackerCommand(1);
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->data[0] = DCUFILTER_OK ; // OK
      //reply->PvtMessageFrame.StdMessageFrame.TargetAddress = com->PvtMessageFrame.StdMessageFrame.InitiatorAddress;
      errorReportLogger_->errorReport ("DCUFILTER_REGISTER message", LOGDEBUG) ;
      fecSupervisorNumbers_ ++ ;
      break ;
    }
      
    case DCU_VALUES_SEND: {
      mutexAppStatus_->take();
      xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": I2O message: receiving DCU data" ;
      mutexAppStatus_->give();
      // Retreive the size of the array and the DCU array
      int *ptri = com->data ;
      int numberDcu = ptri[0] ;
      dcuDescription *dcu = (dcuDescription *)&ptri[1] ;

      // Check that the values are not currently sent to any of the output
      // if the work loop with the previous data is not finished then throw away these data
      if ( (workDcuFilterLoop_ == NULL) || (listQueue_.size() > (2*fecSupervisorNumbers_)) ) {
	if (listQueue_.size() > (2*fecSupervisorNumbers_)){
	  std::stringstream msgError;
          msgError << "Too many data received, the data will not be taken in account (" <<  listQueue_.size() << " > " << 2*fecSupervisorNumbers_ << ")";
	  errorReportLogger_->errorReport (msgError.str(), LOGWARNING) ;
	}
	else
	  errorReportLogger_->errorReport ("The upload work loop cannot be activated, see previous error message", LOGFATAL) ;
      }
      else {

	  std::stringstream msgError;
          msgError << "Data accepted, queue size " <<  listQueue_.size();
	  errorReportLogger_->errorReport (msgError.str(), LOGUSERINFO) ;

	// Build the vector
	if (displayDebugMessage_)
	  std::cout << "DCU Type\tDCU Hard ID \tFEC HARDWARE ID" << std::endl ;

	deviceVector *vDevice = new deviceVector ;
	for (int i = 0 ; i < numberDcu ; i ++) {
	  dcuDescription *dcuI = new dcuDescription (dcu[i]) ;
	  vDevice->push_back(dcuI) ; 
	  if (displayDebugMessage_) {
	    if (dcuI->getDcuType() == DCUFEH) 
	      std::cout << "DCU ON FEH" << "\t" << std::dec << dcuI->getDcuHardId() << "\t" << dcuI->getFecHardwareId() << std::endl ;
	    else
	      std::cout << "DCU ON CCU" << "\t" << std::dec << dcuI->getDcuHardId() << "\t" << dcuI->getFecHardwareId() << std::endl ;
	  }
	}

	// Build the queue
	mutexVectorQueue_->take() ;
	listQueue_.push_back(vDevice) ;
	mutexVectorQueue_->give() ;

	// Queue the thread for the upload
	try {
	  mutexAppStatus_->take();
	  xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": I2O message: submit the work loop for the upload of the DCU data" ;
	  mutexAppStatus_->give();
	  workDcuFilterLoop_->submit(uploadWorkLoop_);
	}
	catch (toolbox::exception::QueueFull& qf) {
	  errorReportLogger_->errorReport ("work loop queue is full (toolbox::exception::QueueFull): " + std::string(qf.what()), LOGWARNING) ;
	}
	catch (toolbox::task::exception::Exception &e) {
	  errorReportLogger_->errorReport ("Cannot start the DCU readout, problem in the work loop submission (toolbox::task::exception::Exception): " + std::string(e.what()), LOGFATAL) ;
	}

	// Message
	std::stringstream msgInfo ; msgInfo << "number of DCU received " << vDevice->size() ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	counter_ += vDevice->size() ;
      }

      // Answer to the FecSupervisor
      ackRef = allocateTrackerCommand(1);
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      
      reply->data[0] = DCUFILTER_OK ;
      //reply->PvtMessageFrame.StdMessageFrame.TargetAddress = com->PvtMessageFrame.StdMessageFrame.InitiatorAddress;
      break ;
    }

    case DCU_GETVALUES: {
      mutexAppStatus_->take();
      xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": I2O message: request for DCU data" ;
      mutexAppStatus_->give();
      int countMaxDcu = dcuValuesReceived_.size(), counterDcu = 0 ;
      // Maximum size of the buffer is 256kb
      if ((countMaxDcu*sizeof(dcuDescription)) > MAXI2OMESSAGESIZE) countMaxDcu = (int)(MAXI2OMESSAGESIZE / sizeof(dcuDescription)) ;
      ackRef = allocateTrackerCommand((int)((countMaxDcu*sizeof(dcuDescription))/sizeof(int)) + 2) ;
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->Error = DD_RETURN_OK ;
      reply->data[0] = countMaxDcu ;
      dcuDescription *dcu = (dcuDescription *)(&reply->data[1]) ;
      for (DcuValuesReceivedType::iterator itDcuValues = dcuValuesReceived_.begin() ; (itDcuValues != dcuValuesReceived_.end()) && (counterDcu < countMaxDcu) ; itDcuValues ++, counterDcu++) {
	memcpy(&dcu[counterDcu], itDcuValues->second, sizeof(dcuDescription)) ;
      }
      break;
    }

    case FEC_TEMP_SEND: {
      mutexAppStatus_->take();
      xdaqApplicationStatus2_ = errorReportLogger_->getStrProcess ( ) + ": I2O message: FEC temperature reception" ;
      mutexAppStatus_->give();
      // Retreive the size of the array and the DCU array
      int *ptri = com->data ;
      int fecSlot = ptri[0] ;
      int fecTemp0 = ptri[1] ;
      int fecTemp1 = ptri[2] ;

      if (fecTemp0 < 10 || fecTemp1 < 10 || fecTemp0 > 40 || fecTemp1 > 40) {
	std::stringstream msgError ; msgError << "The FEC temperature from slot " << std::dec << fecSlot << " is too high or too low: " << fecTemp0 << " / " << fecTemp1 ;
	errorReportLogger_->errorReport (msgError.str(), LOGFATAL) ;
      }
      else {
	std::stringstream msgInfo ; msgInfo << "The FEC temperature from slot " << std::dec << fecSlot << " are: " << fecTemp0 << " / " << fecTemp1 ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
      }

      ackRef = allocateTrackerCommand(1);
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->data[0] = DCUFILTER_OK ;
      //reply->PvtMessageFrame.StdMessageFrame.TargetAddress = com->PvtMessageFrame.StdMessageFrame.InitiatorAddress;
      break ;
    }

    default: {
      ackRef = allocateTrackerCommand(1);
      reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
      reply->data[0] = -1;
      //reply->PvtMessageFrame.StdMessageFrame.TargetAddress = com->PvtMessageFrame.StdMessageFrame.InitiatorAddress;

      // Error
      std::stringstream msgError ; msgError << "Receive an unknow command " << com->Command << " from system " << com->System << "and sub-system " << com->SubSystem ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      lastErrorMessage_ = msgError.str() ;
      errorOccurs_ = true ;
      break ;
    }
    }
  }
  else {
      
    ackRef = allocateTrackerCommand(1);
    reply = (PI2O_TRACKER_COMMAND_MESSAGE_FRAME)ackRef->getDataLocation();
    reply->data[0] = -1;
    //reply->PvtMessageFrame.StdMessageFrame.TargetAddress = com->PvtMessageFrame.StdMessageFrame.InitiatorAddress;

    // Error
    std::stringstream msgError ; msgError << "DCU Filter: Receive a message from a unknow system (System = " << com->System << " sub-system = " << com->SubSystem << " and command = " << com->Command ;
    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    lastErrorMessage_ = msgError.str() ;
    errorOccurs_ = true ;
  }

  return ackRef;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Upload the data to PVSS / files / database                                 */
/*                                                                                                              */
/* ************************************************************************************************************ */
/** this method upload the data received to PVSS/ files/ database
 */
bool DcuFilter::uploadWorkLoop (toolbox::task::WorkLoop* wl) {

  errorReportLogger_->errorReport ("*** DcuFilter: start the upload", LOGDEBUG) ;

  sleepUploadWorkLoop_ = true ;

  // Retreive the vector of values
  mutexVectorQueue_->take() ;
  if (listQueue_.begin() == listQueue_.end()) {
    mutexVectorQueue_->give() ;
    errorReportLogger_->errorReport ("No data in the queue to be sent to PVSS", LOGERROR) ;
  }
  else {
    deviceVector *vDevice = dynamic_cast<deviceVector *>(*(listQueue_.begin())) ;
    listQueue_.pop_front() ;
    mutexVectorQueue_->give() ;
    
    // Upload in file 
    // if (doConversion_ && doUploadInFile_) {
    // xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Upload work loop: store a file with the converted data"  ;
    // uploadDcuConvertedToFile ( *vDevice ) ;
    // }
    // else 
    
    // Upload in file (only if no conversion has been applied)
    if (doUploadInFile_) {
      mutexAppStatus_->take();
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Upload work loop: store a file with the DCU raw data"  ;
      mutexAppStatus_->give();
      uploadDcuToFile ( *vDevice ) ;
    }

    // Upload to PVSS (only if no conversion has been applied)
    if (doUploadPVSS_) {
      mutexRunPvss_->take();

      if (sendruntopvss_) {
	sendruntopvss_ = false;
	mutexRunPvss_->give();
	sendRunStuffToPVSS();
      }

      else mutexRunPvss_->give();

      errorReportLogger_->errorReport ("Before  uploadDcuToPVSS ()", LOGDEBUG) ;
      uploadDcuToPVSS ( *vDevice ) ;
      errorReportLogger_->errorReport ("After  uploadDcuToPVSS ()", LOGDEBUG) ;
    }
    
    // Upload in database
    if (doUploadInDatabase_ && databaseAccess_) {
      mutexAppStatus_->take();
      xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Upload work loop: upload the DCU raw data to the configuration database"  ;
      mutexAppStatus_->give();
      errorReportLogger_->errorReport ("Before uploadDcuToDatabase()", LOGDEBUG) ;
      uploadDcuToDatabase ( *vDevice ) ;
      errorReportLogger_->errorReport ("After uploadDcuToDatabase()", LOGDEBUG) ;
    }

    // To store the errors in file /tmp/DCUValuesChanged.txt
    FILE *file = NULL ;
    bool firstOpen = false ;

    // Add the DCU as received
    for (deviceVector::iterator dcuI = vDevice->begin() ; dcuI != vDevice->end() ; dcuI++) {
      dcuDescription *dcuD = (dcuDescription *)(*dcuI) ;

      if (checkDCUData_ && (dcuValuesReceived_.find(dcuD->getDcuHardId()) != dcuValuesReceived_.end())) {

	TkDcuConversionFactors *conversionFactors = getTkDcuConversionFactors (*dcuD) ;
	if (conversionFactors != NULL) {
	  std::string dcuChannelChanged ;
	  bool levelReached = conversionFactors->isPourcentageDifferent(dcuValuesReceived_[dcuD->getDcuHardId()], pourcentDifferenceForPVSS_, dcuChannelChanged) ;
	  if (levelReached) {

	    char msg[80] ; decodeKey(msg,dcuD->getKey()) ;
	    std::stringstream msgWarn ; msgWarn << "The DCU on " << msg << " (DCU hard id 0x" << std::hex << dcuD->getDcuHardId() << ") has a difference on one channel more than " << std::dec << pourcentDifferenceForPVSS_ << "%" << ": " << dcuChannelChanged ;
	    
	    if (!doUploadPVSS_)  // Report the error, it was not done in the PVSS upload
	      errorReportLogger_->errorReport (msgWarn.str(), LOGINFO) ;
	    
	    // Store the value in file
	    if (!firstOpen) { file = fopen(checkDcuDataFile_.c_str(), "a+") ; firstOpen = true ; }
	    if (file != NULL) {
	      fprintf (file, "%s\n", msgWarn.str().c_str()) ;
	    }
	  }
	}
      }
    
      // Check of the DCU hard ID
      if (dcuD->getDcuHardId() == 0) {
	char msg[80] ;
	decodeKey(msg,dcuD->getKey()) ;
	std::stringstream msgError ; msgError << "Receive an invalid DCU hardware id (0) on " << msg ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
      else {
	// Already received
	if (dcuValuesReceived_.find(dcuD->getDcuHardId()) != dcuValuesReceived_.end()) { 
	  
	  // Check if it is the same DCU but for DCU != 0
	  // Same DCU ?
	  if (dcuD->getKey() != dcuValuesReceived_[dcuD->getDcuHardId()]->getKey()) {
	    char msg[80] ; char msg1[80] ;
	    decodeKey(msg,dcuD->getKey()) ; decodeKey(msg1,dcuValuesReceived_[dcuD->getDcuHardId()]->getKey()) ;
	    std::stringstream msgError ; msgError << "Receive two times the same DCU hard ID (" << std::dec << dcuD->getDcuHardId() << ") but for two different FEC indexes (" 
						   << std::hex << dcuD->getKey() << " / " << dcuValuesReceived_[dcuD->getDcuHardId()]->getKey() << "): " 
						   << msg << " / " << msg1 ;
	    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  }
	  
	  //std::cout << "Delete for DCU Hard ID" << dcuD->getDcuHardId() << std::endl ;
	  delete dcuValuesReceived_[dcuD->getDcuHardId()] ;
	}
	dcuValuesReceived_[dcuD->getDcuHardId()] = dcuD ;
      }
    }

    // Close the error file
    if (file != NULL) fclose(file) ;
  
    // delete the vDevice => do NOT delete it it is used to keep the tracker of the values
    // FecFactory::deleteVector (*vDevice) ;
    delete vDevice ;
  }

  sleepUploadWorkLoop_ = false ;

  errorReportLogger_->errorReport ("*** DcuFilter: end of the upload", LOGDEBUG) ;

  return sleepUploadWorkLoop_ ;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   TK DCU Conversion                                                          */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Retreive the conversion factors and return the conversionFactors together with the value
 * \param dcuD - dcuDescription
 * \return conversion factors for that DCU or default conversion factors
 */
TkDcuConversionFactors *DcuFilter::getTkDcuConversionFactors ( dcuDescription dcuD ) {

  // First error on this DCU (no conversion factors) will be raised with error then with INFO
  static std::map<unsigned int, bool > levelErrorMap ;
  static std::map<unsigned int, bool > dcuMissingMap ;
  static enum logLevelError levelErrorDefaultFeh = LOGERROR ;
  static enum logLevelError levelErrorDefaultCcu = LOGERROR; 

  if (dcuConversionFactorsMap_.find(dcuD.getDcuHardId()) != dcuConversionFactorsMap_.end()) {
    dcuConversionFactorsMap_[dcuD.getDcuHardId()]->setDcuDescription (&dcuD) ;
    return dcuConversionFactorsMap_[dcuD.getDcuHardId()] ;
  }
  else {

    // Store this DCU for no conversion factors
    if (!dcuMissingMap[dcuD.getDcuHardId()] && (dumpDCUMissingStream_ != NULL) && (dumpDCUMissingStream_->is_open()) ) {
      *dumpDCUMissingStream_ << std::dec << dcuD.getDcuHardId() << " has no conversion factors" << std::endl ;
      dcuMissingMap[dcuD.getDcuHardId()] = true ;
    }

    // Raise en error to the error collecting system
    if (!levelErrorMap[dcuD.getDcuHardId()]) {

      if (!ignoreErrorMessage_) {
	std::stringstream msgError ; msgError << "No conversion factors for the DCU " << std::dec <<  dcuD.getDcuHardId() << std::endl ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	levelErrorMap[dcuD.getDcuHardId()] = true ;
      }
    
      lastErrorMessage_ = "No conversion factors for at least one of the DCU (see list below)" ;
      errorOccurs_ = true ;
    }
    else {
      std::stringstream msgError ; msgError << "No conversion factors for the DCU " << std::dec <<  dcuD.getDcuHardId() << std::endl ;
      errorReportLogger_->errorReport (msgError.str(), LOGDEBUG) ;
    }

    // If no conversion factors has been found, try to find the default conversion factors
    if (dcuD.getDcuType() == DCUFEH) {
      if (dcuConversionFactorsMap_.find(0) != dcuConversionFactorsMap_.end()) {
	dcuConversionFactorsMap_[0]->setDcuDescription (&dcuD) ;
	return dcuConversionFactorsMap_[0] ;
      }
      else return NULL ;
    }
    else if (dcuConversionFactorsMap_.find(1) != dcuConversionFactorsMap_.end()) {
      dcuConversionFactorsMap_[1]->setDcuDescription (&dcuD) ;
      return dcuConversionFactorsMap_[1] ;
    }

    return NULL ;
  }

  TkDcuConversionFactors *conversionFactors = NULL ;
  bool defaultConversionFlag = false ;
  
  // Try to find the conversion factors
  try {
    mutexAppStatus_->take();
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Retreive the DCU conversion factors for DCU " + toString(dcuD.getDcuHardId()) ;
    mutexAppStatus_->give();
    conversionFactors = tkDcuConversionFactory_->getTkDcuConversionFactors ( dcuD.getDcuHardId() ) ;
    
  }
  catch (FecExceptionHandler &e) {
    
    if (!ignoreErrorMessage_) {
      
      std::stringstream msgError ; msgError << "No conversion factors for the DCU " << std::dec <<  dcuD.getDcuHardId() << std::endl ;
      if (levelErrorMap[dcuD.getDcuHardId()])
	errorReportLogger_->errorReport (msgError.str(), e, LOGINFO) ;
      else {
	errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	levelErrorMap[dcuD.getDcuHardId()] = true ;
      }
    }
    
    lastErrorMessage_ = "No conversion factors for at least one of the DCU (see list below)" ;
    errorOccurs_ = true ;
    defaultConversionFlag = true ;
  }
  
  // If no conversion factors has been found, try to find the default conversion factors
  if (defaultConversionFlag) {
    if (dcuD.getDcuType() == DCUFEH) {
      // Try the conversion with default parameters
      try {
	mutexAppStatus_->take();
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Retreive the DCU default conversion factors for DCU " + toString(dcuD.getDcuHardId()) + " on FEH";
	mutexAppStatus_->give();
	conversionFactors = tkDcuConversionFactory_->getTkDcuConversionFactors ( 0 ) ;
      }
      catch (FecExceptionHandler &e) {
	
	std::stringstream msgError ; msgError << "No default conversion factors, unable to convert values for the DCU " << dcuD.getDcuHardId() << ": " << e.what() << std::endl ;
	errorReportLogger_->errorReport (msgError.str(), e, levelErrorDefaultFeh) ;
	lastErrorMessage_ = "No default conversion factors for the DCU on FEH" ;
	errorOccurs_ = true ;
	levelErrorDefaultFeh = LOGINFO ;
      }
    }
    else {
      // Try the conversion with default parameters
      try {
	mutexAppStatus_->take();
	xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Retreive the DCU default conversion factors for DCU " + toString(dcuD.getDcuHardId()) + " on CCU";
	mutexAppStatus_->give();
	conversionFactors = tkDcuConversionFactory_->getTkDcuConversionFactors ( 1 ) ;
      }
      catch (FecExceptionHandler &e) {
	
	std::stringstream msgError ; msgError << "No default conversion factors, unable to convert values for the DCU " << dcuD.getDcuHardId() << ": " << e.what() << std::endl ;
	errorReportLogger_->errorReport (msgError.str(), e, levelErrorDefaultCcu) ;
	lastErrorMessage_ = "No default conversion factors for the DCU on CCU" ;
	errorOccurs_ = true ;
	levelErrorDefaultCcu = LOGINFO ;
      }
    }
  }
  
  // Convert it if possible
  if (conversionFactors != NULL) {
    
    // Set the values to be converted
    mutexAppStatus_->take();
    xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Apply the conversion on DCU " + toString(dcuD.getDcuHardId()) ;
    mutexAppStatus_->give();
    conversionFactors->setDcuDescription (&dcuD) ;
    
    // Display the values if needed
    if (displayDebugMessage_) {
      try {
	  conversionFactors->display(true) ;
      }
      catch (std::string &e) {
	std::stringstream msgError ; msgError << "Error the display for the DCU " << dcuD.getDcuHardId() << ": " << e << std::endl ;
	errorReportLogger_->errorReport (msgError.str(), LOGINFO) ;
	  lastErrorMessage_ = msgError.str() ;
	  errorOccurs_ = true ;	
      }
    }
  }
  
  dcuConversionFactorsMap_[dcuD.getDcuHardId()] = conversionFactors ;

  return conversionFactors ;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Manage the DCU information received                                        */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** This method upload the DCU converted data to a file (raw data)
 */
void DcuFilter::uploadDcuConvertedToFile ( deviceVector vDevice ) {

  static unsigned int timeStamp = 0 ;
  if (timeStamp == 0) {
    timeStamp = time(NULL) ;
  }
  std::ostringstream fileName ; fileName << directoryDcuDataName_.toString() << "/DCUConvertedValues" << timeStamp << ".txt" ;
  std::stringstream msgInfo ; msgInfo << "Upload " << std::dec << vDevice.size() << " DCU converted values in file " << fileName.str() ;
  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;

  // Open the file
  std::ofstream file(fileName.str().c_str(), std::ios_base::app );

  // Dump the converted values
  for (deviceVector::iterator iDevice = vDevice.begin() ; iDevice != vDevice.end() ; iDevice ++) {
    dcuDescription *dcuDevice = (dcuDescription *)(*iDevice) ;

    try {
      // Retreive the conversion factors
      TkDcuConversionFactors *conversionFactors = getTkDcuConversionFactors (*dcuDevice) ;
      if (conversionFactors != NULL) {
	std::string str = conversionFactors->getDisplay(true) ;
	file << str ;
      }
      else {

	std::stringstream msgError ; msgError << "No conversion factors for the DCU " << dcuDevice->getDcuHardId() << std::endl ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    catch (std::string &e) {
	  
      std::stringstream msgError ; msgError << "Error in conversion for the DCU " << dcuDevice->getDcuHardId() << ": " << e << std::endl ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
    catch (FecExceptionHandler &e) {
      
      std::stringstream msgError ; msgError << "No conversion factors for the DCU " << dcuDevice->getDcuHardId() << std::endl ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
    }
  }

  // close the file
  file.close() ;
}

/** This method upload the DCU raw data to a file
 * \param vDevice - vector of DCU
 */
void DcuFilter::uploadDcuToFile ( deviceVector vDevice ) {

  // creating the directory
  std::cout << "Creating the directory " << directoryDcuDataName_.toString() << std::endl ;
  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH ;
  int reMkdir = mkdir(directoryDcuDataName_.toString().c_str(),mode) ;
  if (!reMkdir) {
    std::stringstream msgError ; msgError << "Creation of directory " << directoryDcuDataName_.toString() << " failed" ;
    errorReportLogger_->errorReport (msgError.str(), LOGUSERINFO) ;
  }

  // Readout
  std::ostringstream fileName ; fileName << directoryDcuDataName_.toString() << "/DCURaw" << time(NULL) << ".xml" ;
  std::stringstream msgInfo ; msgInfo << "Upload " << std::dec << vDevice.size() << " DCU raw data in file " << fileName.str() ;
  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;

  if (fecDeviceFactory_ != NULL) {
    try {
      fecDeviceFactory_->setOutputFileName (fileName.str()) ; 
      fecDeviceFactory_->setDcuValuesDescriptions (partitionName_.toString(), vDevice, true) ;
    }
    catch (FecExceptionHandler &e) {
      std::stringstream msgError ; msgError << "Unable to upload the file " << fileName.str() << " with the DCU raw data " << " (" << e.what() << ")" ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }
  else {
    std::stringstream msgError ; msgError << "Incoherence in the software, trying to upload DCU values in file and factory access not created" ;
    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
  }
}

/** This method upload the DCU data i  nto the database
 * \param vDevice - vector of DCU
 */
void DcuFilter::uploadDcuToDatabase ( deviceVector vDevice ) {

  static std::map<std::string, unsigned int> timestampLastPVSSSent ;

  if (vDevice.size()) {

    // Vector to be uploaded
    deviceVector toBeUploaded ;
    deviceVector toBeUploaded2 ;

    // Is the time maximum between two uploaded is reached
    dcuDescription *dcuDevice = (dcuDescription *)(*vDevice.begin()) ;

    if (timestampLastPVSSSent.find(dcuDevice->getFecHardwareId()) == timestampLastPVSSSent.end()) 
      timestampLastPVSSSent[dcuDevice->getFecHardwareId()] = 0 ;

    std::cerr << "DEBUGFRED for " << dcuDevice->getFecSlot() << "> Previous timestamp was taken " << timestampLastPVSSSent[dcuDevice->getFecHardwareId()] << " and DCU data just received was taken at " << std::dec << dcuDevice->getTimeStamp() << " (diff = " << (dcuDevice->getTimeStamp() - timestampLastPVSSSent[dcuDevice->getFecHardwareId()]) << " / " << (xdata::UnsignedLongT)timePVSSSend_ << ")" << std::endl ;
    
    if ((dcuDevice->getTimeStamp() - timestampLastPVSSSent[dcuDevice->getFecHardwareId()]) > (xdata::UnsignedLongT)timePVSSSend_) {
      toBeUploaded2 = vDevice ;
      timestampLastPVSSSent[dcuDevice->getFecHardwareId()] = dcuDevice->getTimeStamp() ;
      
      std::cerr << "DEBUGFRED for " << dcuDevice->getFecSlot() << "> " << "sending all DCUs" << std::endl ;
    }
    // the time between two uploaded is not reached, check the differences
    else {
      for (deviceVector::iterator itDevice = vDevice.begin() ; itDevice != vDevice.end() ; itDevice ++) {

	dcuDescription *dcu = (dcuDescription *)(*itDevice) ;

	if (dcuValuesSentToDB_.find(dcu->getDcuHardId()) == dcuValuesSentToDB_.end()) {
	  // First upload
	  dcuValuesSentToDB_[dcu->getDcuHardId()] = dcu->clone() ;
	  toBeUploaded2.push_back(*itDevice) ;
	}
	else {
	  // check each channel
	  bool changed = false ;
	  for (int i = 0 ; (i <= 7) && (!changed) ; i++) {
	    double value = dcu->getDcuChannel(i) - dcuValuesSentToDB_[dcu->getDcuHardId()]->getDcuChannel(i) ; 
	    double min = (dcu->getDcuChannel(i) > dcuValuesSentToDB_[dcu->getDcuHardId()]->getDcuChannel(i) ? dcuValuesSentToDB_[dcu->getDcuHardId()]->getDcuChannel(i) : dcu->getDcuChannel(i)) ;
            min += .00001; // in case min is 0
	    value = 100.*fabs(value / min) ;
	    if (value >= (double)pourcentDifferenceForPVSS_) changed = true ;
	  }
	  if (changed) {
	    delete dcuValuesSentToDB_[dcu->getDcuHardId()] ;
	    dcuValuesSentToDB_[dcu->getDcuHardId()] = dcu->clone() ;
	    toBeUploaded2.push_back(*itDevice) ;
	  }
	  else 
	    std::cerr << "Deadbanding applied to Dcu " << dcu->getDcuHardId() <<std::endl;
	
	}
      }
    }

    std::stringstream msgInfo ; msgInfo << "Upload " << std::dec << toBeUploaded2.size() << " DCU raw data into the database (before cleaning up)" ;
    errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;

    // remove all dcuhardid = 0 from the list

    for (deviceVector::iterator itDevice = toBeUploaded2.begin() ; itDevice != toBeUploaded2.end() ; itDevice ++) {
		  dcuDescription *dcu = (dcuDescription *)(*itDevice) ;
		  if (dcu->getDcuHardId()==0) {
		    std::cerr << "Found null DcuHardId for Crate "<< (*itDevice)->getCrateSlot()<<" Fec "<< (*itDevice)->getFecSlot()<< " ring " << (*itDevice)->getRingSlot() << " ccuaddr " << (*itDevice)->getCcuAddress()<< " I2C ch " << (*itDevice)->getChannel() << " I2C address " <<  (*itDevice)->getAddress() <<"; dropping it" << std::endl;
		  }
		  else toBeUploaded.push_back(*itDevice);
		}
    

    std::stringstream msgInfo2 ; msgInfo2 << "Upload " << std::dec << toBeUploaded.size() << " DCU raw data into the database" ;
    errorReportLogger_->errorReport (msgInfo2.str(), LOGUSERINFO) ;
  
    //std::cout << "DEBUGFRED for " << dcuDevice->getFecSlot() << "> " << msgInfo.str() << std::endl ;

    if (fecDeviceFactory_ != NULL) {
	// loop an all partitions, in case this DcuFilter is getting data for multiple partitions
	char *basic=getenv ("ENV_CMS_TK_PARTITION") ;
	if (basic==0) {
	  try {
	    fecDeviceFactory_->setDcuValuesDescriptions (partitionName_.toString(), toBeUploaded) ;
	  } catch (FecExceptionHandler &e) {
	    std::stringstream msgError ; msgError << "Unable to upload into the database" << " (" << e.what() << ")" ;
	    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
            
	  }
	} else {

	  std::stringstream msgWarn ; msgWarn << "ENV_CMS_TK_PARTITION is "<<basic;
	  errorReportLogger_->errorReport (msgWarn.str(), LOGINFO) ;
	  char *p0,*p1;
	    p0=basic;
	    while (p1=strchr(p0,':')){
	      *p1='\0';
	      try { 
		std::stringstream msgWarn ; msgWarn << "Uploading to db using partition "<<p0;
		errorReportLogger_->errorReport (msgWarn.str(), LOGINFO) ;
	  
		fecDeviceFactory_->setDcuValuesDescriptions (p0, toBeUploaded);
	      }	catch (FecExceptionHandler &e) {
		std::stringstream msgError ; msgError << "Unable to upload into the database" << " (" << e.what() << ")" ;
		errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
		// dump (dcuhardid,dcutimestamp)
		for (deviceVector::iterator itDevice = toBeUploaded.begin() ; itDevice != toBeUploaded.end() ; itDevice ++) {
		  dcuDescription *dcu = (dcuDescription *)(*itDevice) ;
		  std::cerr << "DcuHardId "<< dcu->getDcuHardId() << " DcuTimeStamp " << dcu->getTimeStamp() << std::endl;
		}

	      }
	      catch (...){
		std::stringstream msgError ; msgError << "Unable to upload into the database (not a FecExceptionHandler exception)";
		errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	      }
	      p0=p1+1;
	      *p1=':';
	    }
	    // last partition in env
	    try {
	      std::stringstream msgWarn ; msgWarn << "Uploading to db using partition "<<p0;
	      errorReportLogger_->errorReport (msgWarn.str(), LOGINFO) ;
	      fecDeviceFactory_->setDcuValuesDescriptions (p0, toBeUploaded) ; 
	    }	catch (FecExceptionHandler &e) {
	      std::stringstream msgError ; msgError << "Unable to upload into the database" << " (" << e.what() << ")" ;
	      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	    }
	}
    }
    else {
      std::stringstream msgError ; msgError << "Incoherence in the software, trying to upload DCU values in database and factory access not created" ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }
}

/** This method upload the DCU raw data to PVSS
 * \param vDevice - vector of DCU
 */
void DcuFilter::uploadDcuToPVSS ( deviceVector vDevice ) {

  // Start the sending of the DCU
  unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream msgInfo1 ; msgInfo1 << "DcuFilter " << getApplicationDescriptor()->getInstance() << ": Begin of upload to PVSS" ;
  errorReportLogger_->errorReport( msgInfo1.str(), LOGDEBUG) ;

  // ???????????????????????????
  std::cout << msgInfo1.str() << std::endl ;

  //std::map<std::string,  bool > levelDpeMap ;
  unsigned int dpeSent = 0 ;
  std::string partitionToggle = "" ;
    
  static enum logLevelError levelError = LOGERROR ;
  static std::map<std::string, unsigned int> timestampLastPVSSSent ;

  // Is PVSS is correctly parametrised
  if (xPditor_ == NULL) {

    errorReportLogger_->errorReport ("The configuration for PVSS is not set", levelError) ;
    levelError = LOGINFO ;

    lastErrorMessage_ = "Error: The configuration for PVSS is not set" ;
    errorOccurs_ = true ;

    return ;
  }

  // Number of devices to be uploaded
  if (vDevice.size() == 0) {

    errorReportLogger_->errorReport ("No data to be uploaded to PVSS", LOGERROR) ;
    lastErrorMessage_ = "No data to be uploaded to PVSS" ;
    errorOccurs_ = true ;

    return ;
  }

  // URL
  std::stringstream msgInfo ; msgInfo << "Upload " << std::dec << vDevice.size() << " DCU raw data to PVSS on " << " PVSS " << pvssClassName_.toString() << " instance " << pvssInstance_.toString() << " (" << soapActionString_.toString() << ") ";
  errorReportLogger_->errorReport (msgInfo.str(), LOGDEBUG) ;

  //-------------------------------------------------------------------
  // Send the data to PVSS
  std::string computeDpName = "" ; // if needed then ask to PVSS to compute the values for a given subdetector
  numberOfDcuSentToPVSS_ = 0 ;

  // ------------------------------------------------------------------
  // Prepare the message
  std::vector<std::string> vDataPointsName ;
  std::vector<std::string> vDataPointsValues ;

  // When the last sent has been done
  bool timeReached = false ;
  dcuDescription *dcuDevice = (dcuDescription *)(*vDevice.begin()) ;

  if (timestampLastPVSSSent.find(dcuDevice->getFecHardwareId()) == timestampLastPVSSSent.end()) 
    timestampLastPVSSSent[dcuDevice->getFecHardwareId()] = 0 ;

  if ((dcuDevice->getTimeStamp() - timestampLastPVSSSent[dcuDevice->getFecHardwareId()]) > (xdata::UnsignedLongT)timePVSSSend_) {

    errorReportLogger_->errorReport ("New upload to PVSS, last time was " + toString(timestampLastPVSSSent[dcuDevice->getFecHardwareId()]) + " / " + toString(dcuDevice->getTimeStamp()), LOGDEBUG) ;
    timeReached = true ;
    timestampLastPVSSSent[dcuDevice->getFecHardwareId()] = time(NULL) ; // New sent is requiered
  }

  // Put the DP elements and values
  for (deviceVector::iterator iDevice = vDevice.begin() ; (iDevice != vDevice.end()) && doUploadPVSS_ ; iDevice ++) {

    dcuDescription *dcuDevice = (dcuDescription *)(*iDevice) ;

    if (dcuValuesSentToPVSS_.find(dcuDevice->getDcuHardId()) == dcuValuesSentToPVSS_.end())
      errorReportLogger_->errorReport ("First upload to be done for DCU " + toHexString(dcuDevice->getDcuHardId()), LOGDEBUG) ;

    // Convert it and fill the DP
    if (!doConversion_) { // Normally nothing should be sent
      
      if ( (dcuValuesSentToPVSS_.find(dcuDevice->getDcuHardId()) == dcuValuesSentToPVSS_.end()) || timeReached ) {
	
	vDataPointsName.push_back(pvssPoint_.toString() + DISTRIBUTIONDPNAMEFEH + toString(dcuDevice->getDcuHardId()) + DCUIDDPNAME) ;      
	vDataPointsValues.push_back(toString(dcuDevice->getDcuHardId())) ;
	vDataPointsName.push_back(pvssPoint_.toString() + DISTRIBUTIONDPNAMEFEH + toString(dcuDevice->getDcuHardId()) + TIMESTAMPDPNAME) ;   
	vDataPointsValues.push_back(toString(dcuDevice->getTimeStamp())) ;
	vDataPointsName.push_back(pvssPoint_.toString() + DISTRIBUTIONDPNAMEFEH + toString(dcuDevice->getDcuHardId()) + STATUSDPNAME) ;
	vDataPointsValues.push_back("NOCONVERSION") ;

	numberOfDcuSentToPVSS_ ++ ;
      }
    }
    else {

      // ---------------------------------------------------
      // Conversion to be applied and sent to PVSS
      // Try the conversion 
      TkDcuConversionFactors *conversionFactors = getTkDcuConversionFactors (*dcuDevice) ;

      // ---------------------------------------------------
      // Is conversion factors has been found ?
      if (conversionFactors != NULL) {

	std::string subDetector = conversionFactors->getSubDetector() ;

	if (conversionFactors->getDcuHardId() != 0) {

	  if ( (subDetector == "TEC") || (subDetector == "TEC+") || (subDetector == "TECP") || 
	       (subDetector == "TEC-") || (subDetector == "TECM") ||
	       (subDetector == "TIB") || (subDetector == "TID") || (subDetector == "TOB") ) {

	    if ( (subDetector == "TEC+") || (subDetector == "TECP") || 
		 (subDetector == "TEC-") || (subDetector == "TECM") ) subDetector == "TEC" ;

	    // TIB and TID are on the same crate
	    if (subDetector == "TID") subDetector = "TIB" ;

	    if (computeDpName.size()) {
	      
	      if (subDetector != computeDpName) {

		computeDpName = "TRACKER" ;
		
		std::stringstream msgError ; msgError << "Problem on DCU " << conversionFactors->getDcuHardId() << " is on subdetector " << subDetector << " and previous DCUs were on " << computeDpName ;
		errorReportLogger_->errorReport (msgError.str(), LOGINFO) ;
	      }
	    }
	    else {
	      computeDpName = subDetector ;
	    }
	  }
	  else {
	    std::stringstream msgError ; msgError << "Problem on DCU " << conversionFactors->getDcuHardId() << " is on subdetector " << subDetector << " and previous DCUs were on " << computeDpName ;
	  errorReportLogger_->errorReport (msgError.str(), LOGINFO) ;
	  }
	}


	if ( (subDetector != "TIB" && subDetector != "TID") || (conversionFactors->getDcuType() == DCUFEH) ) {

	  std::string dcuChannelChanged ;

	  if ( (dcuValuesSentToPVSS_.find(dcuDevice->getDcuHardId()) == dcuValuesSentToPVSS_.end()) ||
	       timeReached ||
	       conversionFactors->isPourcentageDifferent(dcuValuesSentToPVSS_[dcuDevice->getDcuHardId()], pourcentDifferenceForPVSS_, dcuChannelChanged) ) {

	    if ( (dcuValuesSentToPVSS_.find(dcuDevice->getDcuHardId()) != dcuValuesSentToPVSS_.end()) && 
		 (conversionFactors->isPourcentageDifferent(dcuValuesSentToPVSS_[dcuDevice->getDcuHardId()], pourcentDifferenceForPVSS_, dcuChannelChanged)) ) {
	      
	      char msg[80] ; decodeKey(msg,dcuDevice->getKey()) ;
	      std::stringstream msgWarn ; msgWarn << "The DCU on " << msg << " (DCU hard id 0x" << std::hex << dcuDevice->getDcuHardId() << ") has a difference on one channel more than " << std::dec << pourcentDifferenceForPVSS_ << "%" << ": " << dcuChannelChanged ;
	      errorReportLogger_->errorReport (msgWarn.str(), LOGINFO) ;
	    }
	    
	    try {
	      
	      // If DCU hard id is 0 then an errors occurs during the readout, reject the value
	      if (dcuDevice->getDcuHardId() == 0) {
		if (dcuDevice->getKey() != 0) {
		  char msg[80] ; decodeKey(msg,dcuDevice->getKey()) ;
		  std::stringstream msgError ; msgError << "The DCU on " << msg << " has an incoherent value in the DCU HARD ID (problem of readout?)" ;
		  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
		}
		else {
		  errorReportLogger_->errorReport ("Receive a DCU with 0 as DCU hard id and 0 as position", LOGERROR) ;
		}
	      }
	      else {
		mutexAppStatus_->take();
		xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": PVSS upload: creation of the datapoints for the DCU converted values for DCU " + toString(dcuDevice->getDcuHardId()) ;
		mutexAppStatus_->give();
		
		// Copy the data for this sent
		if (dcuValuesSentToPVSS_.find(dcuDevice->getDcuHardId()) != dcuValuesSentToPVSS_.end()) delete dcuValuesSentToPVSS_[dcuDevice->getDcuHardId()] ;
		dcuValuesSentToPVSS_[dcuDevice->getDcuHardId()] = dcuDevice->clone() ;
		
		// Build the list of DPe and values to be issued to PVSS
		if (conversionFactors->getDcuType() == DCUFEH) {
		  
		  std::string distributionDpName = pvssPoint_.toString() + DISTRIBUTIONDPNAMEFEH ;
		  
		  // Add the DCU hard ID
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + DCUIDDPNAME) ;
		  vDataPointsValues.push_back(toString(dcuDevice->getDcuHardId())) ;
		  numberOfDcuSentToPVSS_ ++ ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + DCUIDDPNAME + 
						   ", value = " + toString(dcuDevice->getDcuHardId()), LOGDEBUG) ;
		  
		  // Add the timestamp
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TIMESTAMPDPNAME) ;
		  vDataPointsValues.push_back(toString(dcuDevice->getTimeStamp())) ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TIMESTAMPDPNAME + 
						   ", value = " + toString(dcuDevice->getTimeStamp()), LOGDEBUG) ;
		  
		  // Add a status
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + STATUSDPNAME) ;
		  vDataPointsValues.push_back("OK") ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + STATUSDPNAME + 
						   ", value = " + "OK", LOGDEBUG) ;
		  
		  // Data quality
		  bool quality = true , qal ;
		  
		  // Temperature on the silicon sensor
                  // do not send to PVSS raw zeroes 
                  if (conversionFactors->getSiliconSensorTemperature(qal) != -9999.) {
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TSIDPNAME) ;
		  vDataPointsValues.push_back(toString(conversionFactors->getSiliconSensorTemperature(qal))) ;
		  quality = quality && qal ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TSIDPNAME + 
						   ", value = " + toString(conversionFactors->getSiliconSensorTemperature(qal)), LOGDEBUG) ;
		  } else {
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TSIDPNAME + ": detected a raw 0. Value not sent to PVSS", LOGWARNING);    
		  }
		  
		  // V250
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + V250DPNAME) ;
		  vDataPointsValues.push_back(toString(conversionFactors->getV250(qal))) ;
		  quality = quality && qal ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + V250DPNAME + 
						   ", value = " + toString(conversionFactors->getV250(qal)), LOGDEBUG) ;
		  
		  // V125
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + V125DPNAME) ;
		  vDataPointsValues.push_back(toString(conversionFactors->getV125(qal))) ;
		  quality = quality && qal ;
		
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + V125DPNAME + 
						   ", value = " + toString(conversionFactors->getV125(qal)), LOGDEBUG) ;
		  
		  // Leakage current 
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + IDPNAME) ;
		  vDataPointsValues.push_back(toString(conversionFactors->getILeak(qal))) ;
		  quality = quality && qal ;
		
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + IDPNAME + 
						   ", value = " + toString(conversionFactors->getILeak(qal)), LOGDEBUG) ;
		
		  // Temperature of hybrid 
		  if (conversionFactors->getHybridTemperature(qal) != -9999.) {
		    vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + THYBDPNAME) ;
		    vDataPointsValues.push_back(toString(conversionFactors->getHybridTemperature(qal))) ;
		    quality = quality && qal ;
		    
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + THYBDPNAME + 
						   ", value = " + toString(conversionFactors->getHybridTemperature(qal)), LOGDEBUG) ;
		  } else {
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + THYBDPNAME + + ": detected a raw 0. Value not sent to PVSS", LOGWARNING);  
		  }
		  // DET ID
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + DETIDDPNAME) ;
		  vDataPointsValues.push_back(toString(conversionFactors->getDetId())) ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + DETIDDPNAME + 
						   ", value = " + toString(conversionFactors->getDetId()), LOGDEBUG) ;
		  
		  // Temperature of the DCU
		  if (conversionFactors->getDcuTemperature(qal) != -9999.) {
		    vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDCUDPNAME) ;
		    vDataPointsValues.push_back(toString(conversionFactors->getDcuTemperature(qal))) ;
		    quality = quality && qal ;
		  
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDCUDPNAME + 
						     ", value = " + toString(conversionFactors->getDcuTemperature(qal)), LOGDEBUG) ;
		  } else {
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDCUDPNAME +  ": detected a raw 0. Value not sent to PVSS", LOGWARNING);    
		  }
		  // Construction ID
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + CONSTRIDDPNAME) ;
		  vDataPointsValues.push_back(toString(conversionFactors->getContainerId())) ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + CONSTRIDDPNAME + 
						   ", value = " + toString(conversionFactors->getContainerId()), LOGDEBUG) ;
		  
		  // Quality flag
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + QUALITYDPNAME) ;
		  if (quality)
		    vDataPointsValues.push_back("1") ;
		  else
		    vDataPointsValues.push_back("0") ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + QUALITYDPNAME + ", value = " + "DUNNO", LOGDEBUG) ;
		}
		else {
		  
		  std::string distributionDpName = pvssPoint_.toString() + DISTRIBUTIONDPNAMECCU ;
	      
		  // Add the DCU hard ID
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + DCUIDDPNAME) ;
		  vDataPointsValues.push_back(toString(dcuDevice->getDcuHardId())) ;
		  numberOfDcuSentToPVSS_ ++ ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + DCUIDDPNAME + 
						   ", value = " + toString(dcuDevice->getDcuHardId()), LOGDEBUG) ;
		  
		  // Add the timestamp
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TIMESTAMPDPNAME) ;
		  vDataPointsValues.push_back(toString(dcuDevice->getTimeStamp())) ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TIMESTAMPDPNAME + 
						   ", value = " + toString(dcuDevice->getTimeStamp()), LOGDEBUG) ;
		  
		  // Add a status
		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + STATUSDPNAME) ;
		  vDataPointsValues.push_back("OK") ;
		  
		  errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + STATUSDPNAME + 
						 ", value = " + "OK", LOGDEBUG) ;
		  
		  // Data quality
		  bool quality = true , qal ;
		
		  if (subDetector == "TOB") {
		  
		    // 		  // Temperature on the silicon sensor
		    // 		  vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TSIDPNAME) ;
		    // 		  vDataPointsValues.push_back(toString(conversionFactors->getSiliconSensorTemperature(qal))) ;
		    // 		  quality = quality && qal ;
		    
		    // 		  if (displayDebugMessage_) {
		    // 		    std::cout << "NAME = " << (distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TSIDPNAME) << std::endl ;
		    // 		    std::cout << "Value = " << toString(conversionFactors->getSiliconSensorTemperature(qal)) << std::endl ;
		    // 		  }	      
		    
		    // Temperature on the silicon sensor
		  // Temperature on the silicon sensor
                  // do not send to PVSS raw zeroes 
                  if (conversionFactors->getSiliconSensorTemperature(qal) != -9999.) {
		    vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDPNAME) ;
		    vDataPointsValues.push_back(toString(conversionFactors->getSiliconSensorTemperature(qal))) ;
		    quality = quality && qal ;
		    
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDPNAME + 
						     ", value = " + toString(conversionFactors->getSiliconSensorTemperature(qal)), LOGDEBUG) ;
		  } else {
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDPNAME + ": detected a raw 0. Value not sent to PVSS", LOGWARNING);    
		  }
		    
		    // V250
		    vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + V250DPNAME) ;
		    vDataPointsValues.push_back(toString(conversionFactors->getV250(qal))) ;
		    quality = quality && qal ;
		    
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) +  + 
						     ", value = " + toString(conversionFactors->getV250(qal)), LOGDEBUG) ;
		    
		    // V125
		    vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + V125DPNAME) ;
		    vDataPointsValues.push_back(toString(conversionFactors->getV125(qal))) ;
		    quality = quality && qal ;
		    
		    errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + V125DPNAME + 
						     ", value = " + toString(conversionFactors->getV125(qal)), LOGDEBUG) ;
		    
		    // Temperature of the DCU
		    if (conversionFactors->getDcuTemperature(qal)!= -9999.) {
		      vDataPointsName.push_back(distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDCUDPNAME) ;
		      vDataPointsValues.push_back(toString(conversionFactors->getDcuTemperature(qal))) ;
		      quality = quality && qal ;
		    
		      errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDCUDPNAME + 
						       ", value = " + toString(conversionFactors->getDcuTemperature(qal)), LOGDEBUG) ;
		    } else {
		      errorReportLogger_->errorReport ("NAME = " + distributionDpName + toHEXString(dcuDevice->getDcuHardId()) + TDCUDPNAME + ": detected a raw 0. Value not sent to PVSS", LOGWARNING);
		    }
		  }
		}
	      }
	    }
	    catch (std::string &e) {
	      
	      std::stringstream msgError ; msgError << "Error in conversion for the DCU " << dcuDevice->getDcuHardId() << ": " << e << std::endl ;
	      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	      lastErrorMessage_ = msgError.str() ;
	      errorOccurs_ = true ;
	    }
	    catch (FecExceptionHandler &e) {
	      
	      std::stringstream msgError ; msgError << "No conversion factors for the DCU " << dcuDevice->getDcuHardId() << std::endl ;
	      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	      lastErrorMessage_ = msgError.str() ;
	      errorOccurs_ = true ;
	    }
	  }
	}
	else {
	  static bool sentToErrorDispatcher = true ;
	  if (sentToErrorDispatcher) {
	    std::stringstream msgError ; msgInfo << "The DCU on CCU for TIB are not sent to PVSS" ; 
	    errorReportLogger_->errorReport (msgError.str(), LOGUSERINFO) ;
	    sentToErrorDispatcher = false ;
	  }
	}
      }
      else { // NO conversion factors => conversionFactors == NULL
	std::stringstream msgError ; msgError << "Cannot send the DCU to PVSS: no conversion factors for the DCU " << dcuDevice->getDcuHardId() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	lastErrorMessage_ = msgError.str() ;
	errorOccurs_ = true ;
      }
    }    
  }

  // -----------------------------------------------------------------------------------------------------------------
  // Send the data to PVSS
  if ((doUploadPVSS_) && (vDataPointsName.size() > 0)) {

    std::cout << "----------- Begin of process of sending in PVSS for " << vDataPointsName.size() << " datapoint elements" << std::endl ;
    unsigned long startMillisPVSS = XMLPlatformUtils::getCurrentMillis();

    std::vector<std::string> vName ;
    std::vector<std::string> vValue ;
    for ( std::vector<std::string>::iterator itName = vDataPointsName.begin(), itValue = vDataPointsValues.begin() ;
	  itName != vDataPointsName.end() ; itName ++, itValue ++ ) {

      vName.push_back(*itName) ;
      vValue.push_back(*itValue) ;
      if ((vName.size() >= blockDpToBeSent_) || ((itName+1) == vDataPointsName.end())) {
	
	try {
	  mutexAppStatus_->take();
	  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": PVSS upload: sending the datapoints tp PVSS" ;
	  mutexAppStatus_->give();
	  xPditor_->dpSet ( vName, vValue ) ;
	  std::stringstream msgInfo ; msgInfo << "SOAP request successfully sent: datapoints are set." ;
	  errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	  dpeSent += vName.size() ;
	}
	catch (xcept::Exception &e) {
	  static std::map<unsigned int, bool > dcuMissingMap ;

	  std::stringstream msgError ; msgError << "Error: Unable to send the DCU data to PVSS: " << e.what() ;
	  if (ignoreErrorMessage_) errorReportLogger_->errorReport (msgError.str(), LOGINFO) ;
	  else errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;

	  // Store the error
	  if ( (dumpDCUMissingStream_ != NULL) && (dumpDCUMissingStream_->is_open()) ) {
	    //29 Jun 2007 09:58:1183107525 [123136944] ERROR ch.cern.cmstracker015.p:40666.DcuFilter.instance(0) <> - DCU Filter 0, partition SliceTest_25062007_VeryCold: Error: Unable to send the DCU data to PVSS: Caught exception: psx::sapi::exception::Exception 'cannot map data point name: SliceTestDCU:DCU_BF5124.Status:_original.._value to data point id' raised at submit(/opt/TriDAS/daq/psx/sapi/src/common/ApplicationService.cc:336)
	    std::string error(e.what());
	    std::string::size_type iname = error.find(DISTRIBUTIONDPNAMEFEH) ;
	    if (iname != std::string::npos) {
	      iname += std::string(DISTRIBUTIONDPNAMEFEH).size() ;
	      char c[80] ; int i = 0 ;
	      while ( (iname < error.length()) && (error.c_str()[iname] != '.') ) {
		c[i] = error.c_str()[iname] ; iname ++ ; i ++ ;
	      }
	      c[i] = '\0' ;
	      unsigned int dcuHardId = fromString<unsigned int>(c) ;
	      if (!dcuMissingMap[dcuHardId] && (strlen(c) != 0) ) {
		*dumpDCUMissingStream_ << c << " : the corresponding datapoint element does not exists in PVSS" << std::endl ;
		dcuMissingMap[dcuHardId] = true ;
	      }
	    }
	  }

	}

	vName.clear() ; vValue.clear() ;
      }
    }

    unsigned long endMillisPVSS = XMLPlatformUtils::getCurrentMillis();
    std::cout << "----------- End of process of sending in PVSS for " << dpeSent << " datapoint elements in " << (endMillisPVSS-startMillisPVSS) << " ms" << std::endl ;
  }

  // ----------------------------------------------------------
  // Delete the connection to PVSS if requested in the meanwhile
  if ( (!doUploadPVSS_) && (xPditor_ != NULL) ) { 
    errorReportLogger_->errorReport ("PVSS upload has been interrupted by user request", LOGUSERINFO) ;
    //delete xPditor_ ; xPditor_ = NULL ; 
  }
  else if (dpeSent > 0) {

    std::stringstream msgInfo ;

    if ((computeDpName=="")||(computeDpName=="UNK")) computeDpName = "TRACKER" ;

    if (computeDpName == "TRACKER") {
      try {
	std::string statusDpCompute = xPditor_->dpGetSync ( pvssPoint_.toString() + COMPUTETRACKER ) ;
	if (statusDpCompute != "2") {
	  xPditor_->dpSet ( pvssPoint_.toString() + COMPUTETRACKER, "1") ;
	  msgInfo << "Compute the values for the whole Tracker" ;
	}
      }
      catch (xcept::Exception &e) {
	std::stringstream msgError ; msgError << "DCU to PVSS: Cannot get compute state value for all subdetector: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    else if (computeDpName == "TOB") {
      try {
	std::string statusDpCompute = xPditor_->dpGetSync ( pvssPoint_.toString() + COMPUTETOB ) ;
	if (statusDpCompute != "2") {
	  xPditor_->dpSet ( pvssPoint_.toString() + COMPUTETOB, "1") ;
	  msgInfo << "Compute the values for the TOB" ;
	}
      }
      catch (xcept::Exception &e) {
	std::stringstream msgError ; msgError << "DCU to PVSS: Cannot get compute state value for subdetector TOB: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    else if (computeDpName == "TIB") {
      try {
	std::string statusDpCompute = xPditor_->dpGetSync ( pvssPoint_.toString() + COMPUTETIB ) ;
	if (statusDpCompute != "2") {
	  xPditor_->dpSet ( pvssPoint_.toString() + COMPUTETIB, "1") ;
	  msgInfo << "Compute the values for the TIB" ;
	}
      }
      catch (xcept::Exception &e) {
	std::stringstream msgError ; msgError << "DCU to PVSS: Cannot get compute state value for subdetector TIB: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    else if (computeDpName == "TID") {
      try {
	std::string statusDpCompute = xPditor_->dpGetSync ( pvssPoint_.toString() + COMPUTETID ) ;
	if (statusDpCompute != "2") {
	  xPditor_->dpSet ( pvssPoint_.toString() + COMPUTETIB, "1") ;
	  msgInfo << "Compute the values for the TID" ;
	}
      }
      catch (xcept::Exception &e) {
	std::stringstream msgError ; msgError << "DCU to PVSS: Cannot get compute state value for subdetector TID: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    else if (computeDpName == "TEC+") {
      try {
	std::string statusDpCompute = xPditor_->dpGetSync ( pvssPoint_.toString() + COMPUTETECPLUS ) ;
	if (statusDpCompute != "2") {
	  xPditor_->dpSet ( pvssPoint_.toString() + COMPUTETECPLUS, "1") ;
	  msgInfo << "Compute the values for the TEC+" ;
	}
      }
      catch (xcept::Exception &e) {
	std::stringstream msgError ; msgError << "DCU to PVSS: Cannot get compute state value for subdetector TEC+: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    else if (computeDpName == "TEC-") {
      try {
	std::string statusDpCompute = xPditor_->dpGetSync ( pvssPoint_.toString() + COMPUTETECMINUS ) ;
	if (statusDpCompute != "2") {
	  xPditor_->dpSet ( pvssPoint_.toString() + COMPUTETECMINUS, "1") ;
	  msgInfo << "Compute the values for the TEC-" ;
	}
      }
      catch (xcept::Exception &e) {
	std::stringstream msgError ; msgError << "DCU to PVSS: Cannot get compute state value for subdetector TEC-: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
    else if (computeDpName == "TEC") {
      try {
	std::string statusDpCompute = xPditor_->dpGetSync ( pvssPoint_.toString() + COMPUTETECPLUS ) ;
	if (statusDpCompute != "2") {
	  xPditor_->dpSet ( pvssPoint_.toString() + COMPUTETECPLUS, "1") ;
	  msgInfo << "Compute the values for the TEC+" ;
	}
	statusDpCompute = xPditor_->dpGetSync ( pvssPoint_.toString() + COMPUTETECMINUS ) ;
	if (statusDpCompute != "2") {
	  xPditor_->dpSet ( pvssPoint_.toString() + COMPUTETECMINUS, "1") ;
	  msgInfo << "Compute the values for the TEC-" ;
	}
      }
      catch (xcept::Exception &e) {
	std::stringstream msgError ; msgError << "DCU to PVSS: Cannot get compute state value for subdetector TEC: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }

    if (msgInfo.str().size()) {
      errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
      partitionToggle = msgInfo.str() ;
    }
  }

  unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
  std::stringstream msgInfo2 ; msgInfo2 << "DcuFilter " << getApplicationDescriptor()->getInstance() << ": End of upload to PVSS in " << (endMillis-startMillis) << " ms" ;
  errorReportLogger_->errorReport( msgInfo2.str(), LOGDEBUG) ;

  // ???????????????????????????
  std::cout << msgInfo2.str() << std::endl ;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Commissioning procedure for DCU / PSU                                      */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Database and text file are supported. The idea is to download the PSU names and make
 * the mapping between DCU and PSU.
 *
 * Configuration file for the PSU channels with the following lines:
 * CONTROL CHANNELS
 * 
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard12/channel000,MYTK_TIB_Layer2
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard12/channel003,MYTK_TIB_Layer3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard08/channel000,MYTK_TOB_DOHM
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard09/channel000,MYTK_TEC_DOHM
 * 
 * POWER GROUPS
 * 
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard00,MYTK_TIB_L2_2_1_2_3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard01,MYTK_TIB_L2_2_1_2_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard02,MYTK_TIB_L2_2_2_2_3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard03,MYTK_TIB_L2_2_2_2_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard04,MYTK_TIB_L2_2_2_2_5
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard06,MYTK_TIB_L3_Int_1_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard07,MYTK_TIB_L3_Int_5_8
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard08,MYTK_TIB_L3_Ext_1_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard09,MYTK_TIB_L3_Ext_5_7
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard00,MYTK_TOB_1_1_2_1_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard01,MYTK_TOB_1_1_2_1_5
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard02,MYTK_TOB_1_5_1_2_3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard03,MYTK_TOB_1_5_1_2_4
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard04,MYTK_TEC_F2
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard05,MYTK_TEC_F3
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard06,MYTK_TEC_B2
 * TEST:CAEN/904_SY1527/branchController01/easyCrate2/easyBoard07,MYTK_TEC_B3
 * 
 * For this commisioning procedures, the program will swith on the CONTROL CHANNELS one by one and detect for each
 * control channels which are the DCU on CCU (by substracting the CCU already detected)
 * Then it continues on the POWER GROUPS. This operation is only done at the intialisation of the state.
 * The FecSupervisor should be initialised before doing this operation.
 * Please note that for the power groups, "/channel000" should be added
 */
void DcuFilter::detectDcuPSUChannels ( ) {
  mutexAppStatus_->take();
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Mapping of the DCU and PSU" ;
  mutexAppStatus_->give();

  // ------------------------------------------------------------------
  // Is PVSS is correctly parametrised
  if (xPditor_ == NULL) {

    errorReportLogger_->errorReport ("The configuration for PVSS is not set", LOGERROR) ;

    lastErrorMessage_ = "Error: The configuration for PVSS is not set" ;
    errorOccurs_ = true ;

    return ;
  }

  // ------------------------------------------------------------------
  if (tkDcuConversionFactory_ == NULL) {
    
    errorReportLogger_->errorReport ("No TkDcuConversionFactory available cannot recover the PSU channels", LOGERROR) ;

    lastErrorMessage_ = "Error: No TkDcuConversionFactory available cannot retreive the PSU channels" ;
    errorOccurs_ = true ;

    return ;
  }

  // ------------------------------------------------------------------
  // Check if the file name is not empty and if it is existing
  if ( (psuConfigurationFile_ != "") && (psuConfigurationFile_ != UNKNOWNFILE) ) {
    if (psuConfigurationFile_.toString() == "database") {
      if (!tkDcuConversionFactory_->getDbUsed()) {
	errorReportLogger_->errorReport ("PSU/DCU map: trying to upload in database but the database is not set", LOGFATAL) ;
	return ;
      }
      else {
	errorReportLogger_->errorReport ("Retreiving PSU names from database for partition " + partitionName_.toString(), LOGINFO) ;
	try {
	  tkDcuConversionFactory_->getPsuNamePartition(partitionName_.toString()) ;
	}
	catch (FecExceptionHandler &e) {
	  errorReportLogger_->errorReport ("Unable to retreive the PSU names from database for partition " + partitionName_.toString(), e, LOGFATAL) ;
	  lastErrorMessage_ = "Unable to retreive the PSU names from database for partition " + partitionName_.toString() ;
	  errorOccurs_ = true ;
	}
	catch (oracle::occi::SQLException &e) {
	  errorReportLogger_->errorReport ("Unable to retreive the PSU names from database for partition " + partitionName_.toString(), e, LOGFATAL) ;
	  lastErrorMessage_ = "Unable to retreive the PSU names from database for partition " + partitionName_.toString() ;
	  errorOccurs_ = true ;
	  return ;
	}
      }
    }
    else { // from text file
      try {
	tkDcuConversionFactory_->setUsingFile() ;
	tkDcuConversionFactory_->setTkDcuPsuMapFileName(psuConfigurationFile_.toString()) ;
      }
      catch (FecExceptionHandler &e) {
	errorReportLogger_->errorReport ("Unable to retreive the PSU names from database for file " + psuConfigurationFile_.toString(), e, LOGFATAL) ;
	lastErrorMessage_ = "Unable to retreive the PSU names from database for file " + psuConfigurationFile_.toString() ;
	errorOccurs_ = true ;
	return ;
      }
    }

    // is some control and power groups in the maps
    if (tkDcuConversionFactory_->getControlGroupDcuPsuMaps().empty()) {
      errorReportLogger_->errorReport ("No control groups found for " + psuConfigurationFile_.toString(), LOGFATAL) ;
      lastErrorMessage_ = "No control groups found for " + psuConfigurationFile_.toString() ;
      errorOccurs_ = true ;
      return ;
    }
    if (tkDcuConversionFactory_->getPowerGroupDcuPsuMaps().empty()) {
      errorReportLogger_->errorReport ("No power groups found for " + psuConfigurationFile_.toString(), LOGERROR) ;
      lastErrorMessage_ = "No control groups found for " + psuConfigurationFile_.toString() ;
      errorOccurs_ = true ;
    }
  }

  // Message for the user
  if (psuConfigurationFile_.toString() == "database") 
    errorReportLogger_->errorReport ("Starting the DCU/PSU mapping from database", LOGINFO) ;
  else
    errorReportLogger_->errorReport ("Starting the DCU/PSU mapping from file " + psuDcuMapFileName_, LOGINFO) ;

  // -------------------------------------------------------------
  // Get all FecSupervisor in the context
  std::set<xdaq::ApplicationDescriptor*> fecSupervisors;
  try {
    fecSupervisors = getApplicationContext()->getDefaultZone()->getApplicationDescriptors("FecSupervisor") ;
    if (fecSupervisors.size() == 0) {
      errorReportLogger_->errorReport ("No FecSupervisor detected", LOGFATAL) ;
      return ;
    }
  }
  catch (xdaq::exception::Exception& e) {
    errorReportLogger_->errorReport ("Cannot detect the FecSupervisor descriptions for I2O messages", e, LOGFATAL) ;
    return ;
  }  

  std::stringstream msgDebug ; msgDebug << "Found " << std::dec << fecSupervisors.size() << " in the partition" << std::endl ;
  errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;

  // -----------------------------------------------------------------
  // Operation to be performed:
  // So I will parse the file and store in a list of control channels (ie
  // the line starting by CAEN between "CONTROL CHANNELS" and "POWER
  // GROUPS") and the in another list all the lines starting by CAEN after
  // the "POWER GROUPS".
  // Then my process will write for each control channels found in the list
  // the DP:
  //  For all Control channels
  //    <channel>.settings.onOff with TRUE
  //    Loop 3 times: wait 1 second, check the status
  //    I will scan the DCUs (difference)
  //    Store the information
  //  For all POWER Groups
  //    <channel>.settings.onOff with TRUE
  //    Loop 3 times: wait 1 second, check the status
  //    I will scan the DCUs (difference)
  //    Store the information
  //  Switch everything off

  // clear the previous control status
  statusIlkControl_.clear() ;

  // set the progress to 0 and 0
  try {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
    std::cout << "Set the counter on " << dcuPsuMapProgressCG_ << " " << 0 << std::endl ;
    std::cout << "Set the counter on " << dcuPsuMapProgressPG_ << " " << 0 << std::endl ;
#endif
#ifdef PVSSSWITCH
    if (dcuPsuMapProgressCG_ != "") xPditor_->dpSet ( dcuPsuMapProgressCG_, "0" ) ;
    if (dcuPsuMapProgressPG_ != "") xPditor_->dpSet ( dcuPsuMapProgressPG_, "0" ) ;
#endif 
  }
  catch (xcept::Exception &e) {
    std::stringstream msgError ; msgError << "Cannot set the progress of DCU/PSU map in " << dcuPsuMapProgressCG_ << " or " << dcuPsuMapProgressPG_ << ": " << e.what() ;
    errorReportLogger_->errorReport (msgError.str(), LOGWARNING) ;
  }

  // Control and power group
  tkDcuPsuMapVector vDcuPsuCG = tkDcuConversionFactory_->getControlGroupDcuPsuMaps() ;
  tkDcuPsuMapVector vDcuPsuPG = tkDcuConversionFactory_->getPowerGroupDcuPsuMaps() ;

  // If no power group then no need to keep the control group on after the detection of the DCU
  if (vDcuPsuPG.size() == 0) {
    errorReportLogger_->errorReport ("PSU Mapping: since no power group is present, the control group is switch off once the detection of DCUs is done", LOGUSERINFO) ;
    switchOffControlGroup_ = true ;
  }
  else if (switchOffControlGroup_) {
    errorReportLogger_->errorReport ("PSU Mapping: the control groups are switch off once the detection is done so the power group DCU detection will failed and you will get error", LOGERROR) ;
  }

  // ---------------------------------------------------------------------
  // Take the hand on PVSS for the switch
  unsigned int positionHandPVSS = 1 ;
  for (tkDcuPsuMapVector::iterator it = vDcuPsuCG.begin() ; it != vDcuPsuCG.end() ; it ++) {
    std::string globalOn = (*it)->getDatapointName() + CGGLOBALON ;
    try {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
      std::cout << "Switch " << globalOn << " " << VALUEONSET << ", position " << positionHandPVSS << " over " << vDcuPsuCG.size() << std::endl ;
#endif
#ifdef PVSSSWITCH
      xPditor_->dpSet ( globalOn, VALUEONSET ) ; 
#endif 
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "PSU Mapping: Cannot get the hand on PVSS (" << globalOn  << "): " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
    positionHandPVSS ++ ;
  }
  sleep((unsigned int)2) ;
  // read back the value
  positionHandPVSS = 1 ;
  for (tkDcuPsuMapVector::iterator it = vDcuPsuCG.begin() ; it != vDcuPsuCG.end() ; it ++) {

    unsigned int timeout = 0 ;
    std::string dataPointValue = VALUEONSET ;
    std::string globalOn = (*it)->getDatapointName() + CGGLOBALON ;
    try {
      do {
#ifdef PVSSSWITCH
	dataPointValue = xPditor_->dpGetSync ( globalOn ) ;
#endif 
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	std::cout << "Get(" << timeout << ") " << globalOn << " " << dataPointValue << ", position " << positionHandPVSS << " over " << vDcuPsuCG.size() << std::endl ;
#endif
	if (dataPointValue != VALUEONSET) {
	  sleep((unsigned int)2) ;
	  timeout ++ ;
	  errorReportLogger_->errorReport ("TIMEOUT => " + toString(timeout) + ": " + globalOn + ": " + dataPointValue, LOGDEBUG) ;
	}
      }
      while ((dataPointValue != VALUEONSET) && (timeout <= MAXTIMEOUTPSUSWITCHON)) ;
      if (dataPointValue != VALUEONSET) {
	std::stringstream msgError ; msgError << "PSU Mapping: Unable to take the hand on PVSS for " << globalOn ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR); 
      }
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "PSU Mapping: Cannot get the hand on PVSS (" <<globalOn  << "): " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
    positionHandPVSS ++ ;
  }

  // ---------------------------------------------------------------------
  // Switch off the PSU control list
  errorReportLogger_->errorReport ("Control and power groups should be off or the test will be not usefull", LOGUSERINFO) ;
//   unsigned int positionOff = 1 ;
//   for (tkDcuPsuMapVector::iterator it = vDcuPsuCG.begin() ; it != vDcuPsuCG.end() ; it ++) {
//     TkDcuPsuMap *current = *it ;
//     std::string switchOff = current->getDatapointName() + DPESETTINGSON ;
//     // Switch off by security
//     try {
// #if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
//       std::cout << "Switch " << switchOff << " " << VALUEOFFSET << ": " << positionOff << " over " << vDcuPsuCG.size() << std::endl ;
// #endif
// #ifdef PVSSSWITCH
//       xPditor_->dpSet ( switchOff, VALUEOFFSET ) ; 
// #endif 
//     }
//     catch (xcept::Exception &e) {
//       std::stringstream msgError ; msgError << "PSU Mapping: Cannot switch off the power supply channel (" << switchOff << "): " << e.what() ;
//       errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
//     }
//   }

  // ---------------------------------------------------------------------
  // Switch the control group on one by one and get the DCUs
  std::map<unsigned int, TkDcuPsuMap *> dcuPsuMapResult ; 
  tkDcuPsuMapVector dcuPsuResultVector ;

  unsigned int position = 1 ;
  unsigned int totalNumber = vDcuPsuCG.size()+vDcuPsuPG.size() ;
  for (tkDcuPsuMapVector::iterator it = vDcuPsuCG.begin() ; it != vDcuPsuCG.end() ; it ++, position++) {
    // Write the switch on
    TkDcuPsuMap *current = *it ;
    errorReportLogger_->errorReport ("Switch on the control group " + current->getDatapointName() , LOGDEBUG) ;
    int errorQuestion = switchChannel (current,true,true,position,totalNumber) ;
    if (errorQuestion) { // not switched on
      std::stringstream msgError ; 
      if (errorQuestion == 1) msgError << "PSU Mapping: Cannot switch on the power supply channel " << current->getPsuName() ;
      else msgError << "PSU Mapping: channel is not connected " << current->getPsuName() ;
      //errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
    else { // everything goes fine and get the DCUs
      getDcuFromFecSupervisor( fecSupervisors, dcuPsuMapResult, current ) ;
    }

    // -- Save the intermediate result in file
    try {
      TkDcuPsuMapFactory tkDcuMapFactory;
      tkDcuMapFactory.setOutputFileName("/tmp/DcuPsuMapIntermediateResult.xml") ;
      tkDcuPsuMapVector intermediateResult ; intermediateResult.clear() ;
      for (std::map<unsigned int, TkDcuPsuMap *>::iterator it = dcuPsuMapResult.begin() ; it != dcuPsuMapResult.end() ; it ++) {
	intermediateResult.push_back(it->second) ;
      }
      tkDcuMapFactory.setTkDcuPsuMap(intermediateResult) ;
      errorReportLogger_->errorReport ("Upload the intermediate result in file /tmp/DcuPsuMapIntermediateResult.xml", LOGINFO) ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("Unable to upload the intermediate result in file /tmp/DcuPsuMapIntermediateResult.xml", e, LOGWARNING) ;
    }
    // -- end of saving intermediate result

    // set the progress
    try {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
      std::cout << "Set the counter on " << dcuPsuMapProgressCG_ << " " << position << " over " <<  vDcuPsuCG.size() << std::endl ;
#endif
#ifdef PVSSSWITCH
      if (dcuPsuMapProgressCG_ != "")
	xPditor_->dpSet ( dcuPsuMapProgressCG_, toString(position) ) ;
#endif 
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "Cannot set the progress of DCU/PSU map in " << dcuPsuMapProgressCG_ << ": " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGWARNING) ;
    }

    // Switch off control group
    if (switchOffControlGroup_) switchChannel (current,true,false,position,totalNumber) ;
  }

  // ---------------------------------------------------------------------
  // Switch the power group one by one and get the DCUs
  unsigned int pgProgress = position ;
  for (tkDcuPsuMapVector::iterator it = vDcuPsuPG.begin() ; it != vDcuPsuPG.end() ; it ++, position++) {
    // Write the switch on
    TkDcuPsuMap *current = *it ;
    errorReportLogger_->errorReport ("Switch on the power group " + current->getDatapointName() , LOGDEBUG) ;
    int errorQuestion = switchChannel (current,false,true,position,totalNumber) ;
    if (errorQuestion) { // not switched on
      std::stringstream msgError ; 
      if (errorQuestion == 1) msgError << "PSU Mapping: Cannot switch on the power supply channel " << current->getPsuName() ;
      else msgError << "PSU Mapping: channel is not connected " << current->getPsuName() ;
      //errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
    else { // everything goes fine and get the DCUs
      getDcuFromFecSupervisor( fecSupervisors, dcuPsuMapResult, current ) ;
    }
    switchChannel (current,false,false,position,totalNumber) ;

    // -- Save the intermediate result in file
    try {
      TkDcuPsuMapFactory tkDcuMapFactory;
      tkDcuMapFactory.setOutputFileName("/tmp/DcuPsuMapIntermediateResult.xml") ;
      tkDcuPsuMapVector intermediateResult ; intermediateResult.clear() ;
      for (std::map<unsigned int, TkDcuPsuMap *>::iterator it = dcuPsuMapResult.begin() ; it != dcuPsuMapResult.end() ; it ++) {
	intermediateResult.push_back(it->second) ;
      }
      tkDcuMapFactory.setTkDcuPsuMap(intermediateResult) ;
      errorReportLogger_->errorReport ("Upload the intermediate result in file /tmp/DcuPsuMapIntermediateResult.xml", LOGINFO) ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("Unable to upload the intermediate result in file /tmp/DcuPsuMapIntermediateResult.xml", e, LOGWARNING) ;
    }
    // -- end of saving intermediate result

    // set the progress
    try {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
      std::cout << "Set the counter on " << dcuPsuMapProgressPG_ << " " << (position-pgProgress+1) << " over " <<  vDcuPsuPG.size() << std::endl ;
#endif
#ifdef PVSSSWITCH
      if (dcuPsuMapProgressPG_ != "") 
	xPditor_->dpSet ( dcuPsuMapProgressPG_, toString(position-pgProgress+1) ) ;
#endif 
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "Cannot set the progress of DCU/PSU map in " << dcuPsuMapProgressPG_ << ": " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGWARNING) ;
    }
  }
  
  // -----------------------------------------------------
  // Switch off the PSU control list
  if (!switchOffControlGroup_) {
    unsigned int positionOff = 1 ;
    for (tkDcuPsuMapVector::iterator it = vDcuPsuCG.begin() ; it != vDcuPsuCG.end() ; it ++) {
      TkDcuPsuMap *current = *it ;
      std::string switchOff = current->getDatapointName() + DPESETTINGSON ;
      // Switch off by security
      try {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	std::cout << "Switch " << switchOff << " " << VALUEOFFSET << ", position " << positionOff << " over " <<  vDcuPsuCG.size() << std::endl ;
#endif
#ifdef PVSSSWITCH
	xPditor_->dpSet ( switchOff, VALUEOFFSET ) ; 
#endif 
      }
      catch (xcept::Exception &e) {
	std::stringstream msgError ; msgError << "PSU Mapping: Cannot switch off the power supply channel (" << switchOff << "): " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }
  }

  // -----------------------------------------------------
  // Store it in file or database, XML, PVSS, Error
  bool valuesHaveBeenUploaded = true ;
  try {

    for (std::map<unsigned int, TkDcuPsuMap *>::iterator it = dcuPsuMapResult.begin() ; it != dcuPsuMapResult.end() ; it ++) {
      dcuPsuResultVector.push_back(it->second) ;
    }

    // no data
    if (dcuPsuResultVector.size() == 0) valuesHaveBeenUploaded = false ;

    // upload PVSS map in file
    try {
      tkDcuConversionFactory_->setOutputPVSSFile ( dcuPsuResultVector, psuDcuMapFileName_, psuDcuMapFileName_ + ".err" ) ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("PSU/DCU map: unable to upload the results for PVSS file", e, LOGFATAL) ;
    }

    // upload the result in database or in file
    if (psuConfigurationFile_.toString() == "database") { // upload it in database

#ifdef DEBUGMSGERROR
      for (tkDcuPsuMapVector::iterator it = dcuPsuResultVector.begin() ; it != dcuPsuResultVector.end() ; it ++) {
	for (tkDcuPsuMapVector::iterator it1 = dcuPsuResultVector.begin() ; it1 != dcuPsuResultVector.end() ; it1 ++) {
	  if ( (it != it1) && ((*it)->getDcuHardId() == (*it1)->getDcuHardId()) ) {
	    std::cerr << "----------------------> " << (*it)->getDcuHardId() << " duplicated" << std::endl ;
	  }
	}
      }
#endif

      // upload result in database
      unsigned int versionMajor, versionMinor ;
      tkDcuConversionFactory_->setTkDcuPsuMap ( dcuPsuResultVector, partitionName_, &versionMajor, &versionMinor ) ;
      // message
      std::stringstream msgSuccess ; 
      msgSuccess << "PSU/DCU map: Upload of results in database done in version " << versionMajor << "." << versionMinor 
		 << ", PVSS output is in " << psuDcuMapFileName_ << " and errors in " << psuDcuMapFileName_ << ".err" ;
      errorReportLogger_->errorReport (msgSuccess.str(), LOGUSERINFO) ;
    }
    else { // upload in file
      tkDcuConversionFactory_->setUsingFile() ;
      tkDcuConversionFactory_->setOutputFileName(psuDcuMapFileName_ + ".xml" ) ;
      tkDcuConversionFactory_->setTkDcuPsuMap ( dcuPsuResultVector ) ;
      // message
      std::stringstream msgSuccess ; 
      msgSuccess << "PSU/DCU map: Upload of results in file " << psuDcuMapFileName_ << ".xml"
		 << ", PVSS output is in " << psuDcuMapFileName_ << " and errors in " << psuDcuMapFileName_ << ".err" ;
      errorReportLogger_->errorReport (msgSuccess.str(), LOGUSERINFO) ;
    }
  }
  catch (FecExceptionHandler &e) {
    valuesHaveBeenUploaded = false ;
    errorReportLogger_->errorReport ("PSU/DCU map: unable to upload the results", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    valuesHaveBeenUploaded = false ;
    errorReportLogger_->errorReport ("PSU/DCU map: unable to upload the results", e, LOGFATAL) ;
  }

  // ------------------------------------------------------------------------------------------------------------------------
  // check the result with the TKCC DB in order to get the correct acknowledge (at the end of dcu scan (1 = ok -1 = not ok))
  if (tkccTest_ && valuesHaveBeenUploaded) {

    try {
      std::vector<std::pair<std::string, bool> > coolingLoopResult ;
      bool result = tkDcuConversionFactory_->checkTKCCCoolingLoop ( partitionName_, coolingLoopResult ) ;

      // send the result to the error diagnostic system
      for (std::vector<std::pair<std::string, bool> >::iterator it = coolingLoopResult.begin() ; (it != coolingLoopResult.end()) ; it ++) {

	std::pair<std::string, bool> maPair = *it ;

	if (maPair.second) {
	  std::stringstream msg ; msg << "Cooling loop " << maPair.first << ": DCU/PSU map validated and crosscheck" ;
	  errorReportLogger_->errorReport (msg.str(), LOGUSERINFO) ;
	}
	else {
	  std::stringstream msg ; msg << "Cooling loop " << maPair.first << ": DCU/PSU map error in the crosscheck" ;
	  errorReportLogger_->errorReport (msg.str(), LOGERROR) ;
	}
      }

      std::string value = "1" ;
      if (!result) value = "-1" ;

#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
      std::cout << "Set the TKCC result on " << dcuPsuMapResult_ << " " << value << std::endl ;
#endif	
#ifdef PVSSSWITCH
      if (dcuPsuMapResult_ != "")
	xPditor_->dpSet ( dcuPsuMapResult_, value ) ;
#endif 
    }
    catch (oracle::occi::SQLException &e) {
      std::stringstream msgError ; msgError << "Cannot crosscheck the information of DCU/PSU map with TKCC DB for partition " << partitionName_.toString() << ": " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
    catch (FecExceptionHandler &e) {
      std::stringstream msgError ; msgError << "Cannot crosscheck the information of DCU/PSU map with TKCC DB for partition " << partitionName_.toString() << ": " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;      
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "Cannot set the progress of DCU/PSU map in " << dcuPsuMapResult_ << ": " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }
  
  // delete the map created
  TkDcuPsuMapFactory::deleteVectorI(dcuPsuResultVector) ;
}

/** Readout the DCU from FecSupervisor
 * \param xdaqFecSupervisor - list of FecSupervisors
 * \param dcuPsuMapResult - vector of DCU PSU map
 * \param psuName - for message
 */
void DcuFilter::getDcuFromFecSupervisor ( std::set<xdaq::ApplicationDescriptor*> fecSupervisors, std::map<unsigned int, TkDcuPsuMap *> &dcuPsuMapResult, TkDcuPsuMap *current ) {

  for (std::set<xdaq::ApplicationDescriptor*>::iterator itDes = fecSupervisors.begin() ; itDes != fecSupervisors.end() ; itDes ++) {
    // Readout the DCU
    try {

      // Access each FEC and ask the DCU descriptions
      int data = DCU_DETECTDCUS ;
      this->sendTrackerCommand(*itDes, SYST_TRACKER, SUB_SYST_FEC, VERS_FEC, 
			       DCU_DETECTDCUS, &data, 1) ;
      
      // Check the anwser
      PI2O_TRACKER_COMMAND_MESSAGE_FRAME reply = this->getReply() ;
      
      // Checking the answer
      int *ptri = reply->data ;
      int numberDcu = ptri[0] ;
      
      std::stringstream msgInfo ; msgInfo << "receive " << numberDcu << " DCU from FecSupervisor " 
					   << (*itDes)->getInstance() << " for the PSU channel " << current->getPsuName() ;
      errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
      
      dcuDescription *dcu = (dcuDescription *)&ptri[1] ;
      for (int i = 0 ; i < numberDcu ; i ++) {
	dcuDescription *dcuI = new dcuDescription (dcu[i]) ;
	if (dcuPsuMapResult.find(dcuI->getDcuHardId()) == dcuPsuMapResult.end()) {
	  TkDcuPsuMap *mapDcu = new TkDcuPsuMap ( dcuI->getDcuHardId(), current->getPsuName(), current->getPsuType(), dcuI->getKey(), dcuI->getDcuType() ) ;
	  dcuPsuMapResult[dcuI->getDcuHardId()] = mapDcu ;
	}
      }
    }
    catch (xdaq::exception::Exception& e) {
      std::stringstream msgError ; msgError << "Unable to contact FecSupervisor instance " << std::dec << (*itDes)->getInstance();
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }
}

/** Switch on a control channel
 * \param channel - TK DCU/PSU map
 * \param controlGroup - is a control group
 * \param position - position in the list
 * \param totalNumber - total number of PSU
 * \return unsigned int  - 0 if the channel is on, 1 is the channel cannot be set, -1 if the channel is not connected
 */
int DcuFilter::switchChannel ( TkDcuPsuMap *current, bool controlGroup, bool switchDpStatus, unsigned int position, unsigned int totalNumber ) {

  unsigned int errorQuestion = 0 ;

  std::string valueToBeSet = VALUEONSET ;
  std::string valueToBeGet = VALUEONGET ;
  if (!switchDpStatus) {
    valueToBeSet = VALUEOFFSET ;
    valueToBeGet = VALUEOFFGET ;
  }

  int positionValue = 0 ;

  if (valueToBeSet == VALUEONSET) errorReportLogger_->errorReport ("Switch on the PSU " + current->getDatapointName() , LOGDEBUG) ;
  else errorReportLogger_->errorReport ("Switch off the PSU " + current->getDatapointName() , LOGDEBUG) ;

  std::string switchOn = current->getDatapointName() + DPESETTINGSON ;
  std::string switchOnStatus = current->getDatapointName () + DPESTATUS ;
  std::string statusConnected = current->getDatapointName() + STATUSCONNECTED ;
  std::string statusIlk = current->getDatapointName() + INTERLOCK ;

  try {
    // Check if the control channel is connected
    if (controlGroup) {
      if ( (statusIlkControl_[statusConnected] == "NOT CONNECTED") || (statusIlkControl_[statusIlk] == "INTERLOCKED") ) {
	if (statusIlkControl_[statusConnected] == "NOT CONNECTED") {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	  std::cout <<  current->getDatapointName() << " already checked and is in error due to mis-connection" << std::endl ;
#endif
	  std::stringstream msgError ; msgError << "PSU Mapping: channel " << current->getPVSSName() << " already checked and is not connected" ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  return -1 ; // not connected skipp it
	}
	else {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	  std::cout <<  current->getDatapointName() << " already checked and is in error due to interlock" << std::endl ;
#endif
	  std::stringstream msgError ; msgError << "PSU Mapping: channel " << current->getPVSSName() << " already checked and is interlocked" ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  return -1 ; // interlocked skipp it
	}
      }
      else if ( (statusIlkControl_[statusConnected] == "OK") && (statusIlkControl_[statusIlk] == "OK") ) {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	std::cout <<  current->getDatapointName() << " already checked and is fine" << std::endl ;
#endif
      }
      else {
#ifdef PVSSSWITCH
	unsigned int dataPointValue = fromString<unsigned int>(xPditor_->dpGetSync ( statusConnected )) ;
	positionValue = 1 ;
#endif 
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	if (dataPointValue > 8) 
	  std::cout << "GetStatusConnected: " << statusConnected << " " << dataPointValue << " not connected" << std::endl ;
	else 
	  std::cout << "GetStatusConnected: " << statusConnected << " " << dataPointValue << " connected" << std::endl ;
#endif
	if (dataPointValue > 8) {
	  std::stringstream msgError ; msgError << "PSU Mapping: channel " << current->getPVSSName() << " is not connected, value = " << dataPointValue ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  statusIlkControl_[statusConnected] = "NOT CONNECTED" ;
	  return -1 ; // not connected skipp it
	}
	else statusIlkControl_[statusConnected] = "OK" ;

#ifdef PVSSSWITCH
	std::string interlocked = xPditor_->dpGetSync ( statusIlk ) ;
	positionValue = 2 ;
#endif 
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	std::cout << "GetInterlocked: " << statusIlk << " " << interlocked << std::endl ;
#endif
	if (interlocked == "TRUE") {
	  std::stringstream msgError ; msgError << "PSU Mapping: channel " << current->getPVSSName() << " is interlocked, value = " << interlocked ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  statusIlkControl_[statusIlk] = "INTERLOCKED" ;
	  return -1 ; // interlocked skipp it
	}
	else statusIlkControl_[statusIlk] = "OK" ;
      }
    }

#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
    std::cout << "Switch " << switchOn << " " << valueToBeSet << std::endl ;
#endif
#ifdef PVSSSWITCH
    xPditor_->dpSet ( switchOn, valueToBeSet ) ; 
    positionValue = 3 ;
#endif 

    std::string dataPointValue = "" ; 
    std::stringstream msgUserInfo ; msgUserInfo << "PSU Mapping: switch on the " << std::dec << position << " DPe " << switchOn 
						 << " (" << current->getPVSSName() << ") other " 
						 << totalNumber << " elements" ;
    errorReportLogger_->errorReport (msgUserInfo.str(), LOGINFO) ; 

    int timeout = 0 ;
    do {
      // Wait on the change state
#ifdef PVSSSWITCH
      dataPointValue = xPditor_->dpGetSync ( switchOnStatus ) ;
      positionValue = 4 ;
#endif 
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
      std::cout << "Get(" << timeout << ") " << switchOnStatus << ": " << dataPointValue << std::endl ;
#endif

      if (dataPointValue != valueToBeGet) {
	sleep((unsigned int)2) ;
	timeout ++ ;
	errorReportLogger_->errorReport ("TIMEOUT => " + toString(timeout) + ": " + switchOnStatus + ": " + dataPointValue, LOGDEBUG) ;
	
	// Switch on again
	if (timeout == (int)(MAXTIMEOUTPSUSWITCHON/2)) {
	  std::stringstream msgWarn ; msgWarn << "PSU Mapping: Warning switch on again the channel " << switchOn << " after " << (int)(MAXTIMEOUTPSUSWITCHON/2) << " attemps" ;
	  errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING) ; 
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	  std::cout << "Switch " << switchOn << " " << VALUEONSET << std::endl ;
#endif
#ifdef PVSSSWITCH
	  xPditor_->dpSet ( switchOn, valueToBeSet ) ; 
	  positionValue = 5 ;
#endif 
	}
      }
    }
    while ((dataPointValue != valueToBeGet) && (timeout <= MAXTIMEOUTPSUSWITCHON)) ;
    
    // Is it on ?
    if (dataPointValue != valueToBeGet) {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to switch on the control channel " << switchOnStatus ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR); 
      errorQuestion = 1 ;
    }
  }
  catch (xcept::Exception &e) {
    switch (positionValue) {
    case 0: {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to get the status of the connection " << statusConnected << " from PVSS: " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      break ;
    }
    case 1: {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to get the status of the interlock " << statusIlk << " in PVSS: " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      break ;
    }
    case 2: {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to write datapoint " << switchOn << " in PVSS: " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      break ;
    }
    case 3: {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to read datapoint " << switchOnStatus << " in PVSS: " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      break ;
    }
    case 4: {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to write datapoint " << switchOn << " in PVSS: " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      break ;
    }
    case 5: {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to write datapoint " << switchOn << " in PVSS (2nd attempt): " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      break ;
    }
    default: {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to read/write datapoint " << current->getDatapointName() << " in PVSS: " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      break ;
    }
    }
    errorQuestion = 1 ;
  }

  return errorQuestion ;
}

/** Old version of the previous method
 * \deprecated file and database are supported by the PSU/DCU map factory
 */
void DcuFilter::detectDcuPSUChannelsFromFileOldVersion ( ) {
  mutexAppStatus_->take();
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Mapping of the DCU and PSU" ;
  mutexAppStatus_->give();

  // ------------------------------------------------------------------
  // Is PVSS is correctly parametrised
  if (xPditor_ == NULL) {

    errorReportLogger_->errorReport ("The configuration for PVSS is not set", LOGERROR) ;

    lastErrorMessage_ = "Error: The configuration for PVSS is not set" ;
    errorOccurs_ = true ;

    return ;
  }

  // ------------------------------------------------------------------
  if (tkDcuConversionFactory_ == NULL) {
    
    errorReportLogger_->errorReport ("No TkDcuConversionFactory available cannot recover the PSU channels", LOGERROR) ;

    lastErrorMessage_ = "Error: No TkDcuConversionFactory available cannot recover the PSU channels" ;
    errorOccurs_ = true ;

    return ;
  }

  // ------------------------------------------------------------------
  // Check if the file name is not empty and if it is existing
  bool useFile = false ;
  if ( (psuConfigurationFile_ != "") && (psuConfigurationFile_ != UNKNOWNFILE) ) {
    std::ifstream fichierPSU (psuConfigurationFile_.toString().c_str()) ;
    if (fichierPSU) useFile = true ;
  }

  // ------------------------------------------------------------------
  // Check if file or database
  if (!useFile && tkDcuConversionFactory_->getDbUsed()) {

    errorReportLogger_->errorReport ("No input specified for the DCS power supply channels", LOGFATAL) ;

    lastErrorMessage_ = "No input specified for the DCS power supply channels" ;
    errorOccurs_ = true ;

    return ;
  }
  else if (useFile) {

    std::stringstream msgInfo ; msgInfo << "Make the mapping of PSU channel with DCU thanks to the file " << psuConfigurationFile_.toString() ;
    errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
  }
  else {

    errorReportLogger_->errorReport ("No input specified for the DCS power supply channels", LOGFATAL) ;

    lastErrorMessage_ = "No input specified for the DCS power supply channels" ;
    errorOccurs_ = true ;

    return ;
  }

  // Get all FecSupervisor in the context
  std::set<xdaq::ApplicationDescriptor*> fecSupervisors;
  try {
    fecSupervisors = getApplicationContext()->getDefaultZone()->getApplicationDescriptors("FecSupervisor") ;
    if (fecSupervisors.size() == 0) {
      errorReportLogger_->errorReport ("No FecSupervisor detected", LOGFATAL) ;
      return ;
    }
  }
  catch (xdaq::exception::Exception& e) {
    errorReportLogger_->errorReport ("Cannot detect the FecSupervisor descriptions for I2O messages", e, LOGFATAL) ;
    return ;
  }  

  std::stringstream msgDebug ; msgDebug << "Found " << std::dec << fecSupervisors.size() << " in the partition" << std::endl ;
  errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;

  // -----------------------------------------------------------------
  // Operation to be performed:
  // So I will parse the file and store in a list of control channels (ie
  // the line starting by CAEN between "CONTROL CHANNELS" and "POWER
  // GROUPS") and the in another list all the lines starting by CAEN after
  // the "POWER GROUPS".
  // Then my process will write for each control channels found in the list
  // the DP:
  //  For all Control channels
  //    <channel>.settings.onOff with TRUE
  //    Loop 3 times: wait 1 second, check the status
  //    I will scan the DCUs (difference)
  //    Store the information
  //  For all POWER Groups
  //    <channel>.settings.onOff with TRUE
  //    Loop 3 times: wait 1 second, check the status
  //    I will scan the DCUs (difference)
  //    Store the information
  //  Switch everything off

  // Start the switch on of the power control one by one to see what is the DCU mapping
  // the map psuDcuMap that gives the map between the PSU and the list of DCU hard id is filled
  Sgi::hash_map<unsigned int, std::string> dcuPSUMapping ;
  std::map<std::string, std::string> psuDcuType ;
  std::map<std::string, deviceVector> psuDcuMap ;

  // List of the DP to be set
  std::list<std::string> controlChannelList, powerGroupList ;
  getPSUFileInformation (psuConfigurationFile_.toString(), controlChannelList, powerGroupList, psuDcuType) ;

  if (controlChannelList.empty() || powerGroupList.empty()) {
    std::stringstream msgError ; msgError << "PSU mapping: Invalid number of mapping element in the file " << psuConfigurationFile_.toString() 
					   << " (" << std::dec << controlChannelList.size() << " control channels and " << powerGroupList.size() << " power groups)" ;
    errorReportLogger_->errorReport (msgError.str(), LOGFATAL) ;
    return ;
  }

  // Open the error file
  std::string::size_type loc = psuDcuMapFileName_.find( ".", 0 );
  if (loc == std::string::npos) loc = psuDcuMapFileName_.size() ;
  std::string outputFileError = psuDcuMapFileName_.substr(0,loc) + "Errors" + ".txt" ;
  std::ofstream fileErrorOn ( outputFileError.c_str() ) ;

  // Detection
  fileErrorOn << "-------------- PSU Problem" << std::endl << std::endl ;
  // Detection for the control group
  fileErrorOn << CONTROLCHANNELSSTART << std::endl << std::endl ;
  detectDcuForPSUList (controlChannelList, fecSupervisors, dcuPSUMapping, psuDcuMap, false, fileErrorOn, true) ;
  // Detector for the power group
  fileErrorOn << std::endl << POWERGROUPSSTART << std::endl << std::endl ;
  detectDcuForPSUList (powerGroupList, fecSupervisors, dcuPSUMapping, psuDcuMap, true, fileErrorOn, false) ;
  fileErrorOn << std::endl << "-------------- DCU problem" << std::endl << std::endl ;

  // Switch off the PSU control list
  for (std::list<std::string>::iterator it = controlChannelList.begin() ; it != controlChannelList.end() ; it ++) {
    std::string::size_type loc = (*it).find( COMMA, 0 );
    errorReportLogger_->errorReport ("Switch off the PSU " + (*it).substr(0,loc), LOGDEBUG) ;
    std::string switchOff = (*it).substr(0,loc) + DPESETTINGSON ;
    // Switch off by security
    try {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
      std::cout << "Switch " << switchOff << " " << VALUEOFFSET << std::endl ;
#endif
#ifdef PVSSSWITCH
      xPditor_->dpSet ( switchOff, VALUEOFFSET ) ;
#endif 
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "PSU Mapping: Cannot switch off the power supply channel: " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }

  // Upload it
  mutexAppStatus_->take();
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Upload of the mapping of the DCU and PSU" ;
  mutexAppStatus_->give();
  // File or database
  std::ofstream outputFile (psuDcuMapFileName_.c_str()) ;
  // Key File
  std::string outputFileNameKey = psuDcuMapFileName_.substr(0,loc) + "Key" + ".txt" ;
  std::ofstream outputFileKey (outputFileNameKey.c_str()) ;
  if (doUploadPSUDCUMapFile_) {
    if (!outputFile.is_open()) {
      std::stringstream msgError ; msgError << "DCU Mapping: cannot open the output file " << psuDcuMapFileName_ ;
      errorReportLogger_->errorReport (msgError.str(), LOGFATAL) ;
    }
  }
  else {
    errorReportLogger_->errorReport ("DCU Mapping database solution is not yet implemented", LOGFATAL) ;
  }

  for (std::map<std::string, deviceVector >::iterator itPsuName = psuDcuMap.begin() ; itPsuName != psuDcuMap.end() ; itPsuName ++) {

    if (displayDebugMessage_) std::cout << itPsuName->first << COMMA << psuDcuType[itPsuName->first] << COMMA ; 
    if (doUploadPSUDCUMapFile_ && outputFile.is_open()) outputFile << itPsuName->first << COMMA << psuDcuType[itPsuName->first] << COMMA ; 
    //if (doUploadPSUDCUMapFile_ && outputFileKey.is_open()) outputFileKey << itPsuName->first << COMMA << psuDcuType[itPsuName->first] << COMMA ; 
    for (deviceVector::iterator itElt = itPsuName->second.begin() ; itElt != itPsuName->second.end() ; itElt ++) {
      dcuDescription *dcu = dynamic_cast<dcuDescription *>(*itElt) ;
      if (dcu->getDcuHardId() != 0) {

	// ------------------------------------
	// Crosscheck with the conversion factors for DCU TYPE
 	TkDcuConversionFactors *tkConv = getTkDcuConversionFactors ( *dcu ) ;
 	if (tkConv->getDcuHardId() != 0) {
 	  if (psuDcuType[itPsuName->first] != dcu->getDcuType()) {
 	    std::stringstream msgError ; msgError << "PSU mapping: Invalid DCU type (" << psuDcuType[itPsuName->first] 
 						   << "/" << dcu->getDcuType() << ") regarding the PSU name " << itPsuName->first 
						   << " for DCU " << toHEXString(tkConv->getDcuHardId()) ;
 	    errorReportLogger_->errorReport (msgError.str(), LOGFATAL) ;	    
	    fileErrorOn << msgError.str() << std::endl ;
 	  }
 	}
	// ------------------------------------

	if (displayDebugMessage_) std::cout << toHEXString(dcu->getDcuHardId()) << COMMA ;
	if (doUploadPSUDCUMapFile_ && outputFile.is_open()) outputFile << toHEXString(dcu->getDcuHardId()) << COMMA ;
	if (doUploadPSUDCUMapFile_ && outputFileKey.is_open()) outputFileKey << dcu->getDcuHardId() << "\t" << toHEXString(dcu->getKey()) << "\t" << itPsuName->first << std::endl ;
      }
    }
    if (displayDebugMessage_) std::cout << std::endl ;
    if (doUploadPSUDCUMapFile_ && outputFile.is_open()) outputFile << std::endl ;
    //if (doUploadPSUDCUMapFile_ && outputFileKey.is_open()) outputFileKey << std::endl ;
    FecFactory::deleteVectorI (itPsuName->second) ;
  }
  if (outputFile.is_open()) outputFile.close() ;
  if (outputFileKey.is_open()) outputFileKey.close() ;
  if (fileErrorOn.is_open()) fileErrorOn.close() ;

  std::stringstream msgInfo ; msgInfo << "DCU PSU mapping has been done, please check the output" ;
  errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
}

/** Retreive the DCU for the PSU DP list. This method send an i2o message to the corresponding list of application and get back the DCU Hard id and build the map
 * \param psuDPeList - datapoint PSU list
 * \param fecSupervisors - FecSupervisor to be contacted in the partition
 * \param dcuPSUMapping - hash_map between the DCU and the PSU DPE 
 * \param psuDcuMap - Map between the PSU channel name and the list of DCU hard ID. 
 * \param switchOff - switch the PSU after the scan DCU
 * \param cg - control group if true or power group if false
 * \deprecated file and database are supported by the PSU/DCU map factory
 */
void DcuFilter::detectDcuForPSUList ( std::list<std::string> psuDPeList, std::set<xdaq::ApplicationDescriptor*> fecSupervisors, 
				      Sgi::hash_map<unsigned int, std::string> &dcuPSUMapping, 
				      std::map<std::string, deviceVector > &psuDcuMap, 
				      bool switchOff, std::ofstream &fileErrorOn, bool cg ) {
  mutexAppStatus_->take();
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Mapping of the DCU and PSU" ;
  mutexAppStatus_->give();
  unsigned int positionList = 1 ;
  for (std::list<std::string>::iterator it = psuDPeList.begin() ; it != psuDPeList.end() ; it ++, positionList++) {
    // Write the switch on
    std::string::size_type loc = (*it).find( COMMA, 0 );
    if (loc == std::string::npos) {
      loc = (*it).size() ;
      std::stringstream msgError ; msgError << "PSU Mapping: channel " << (*it) << " has no comma" ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
    errorReportLogger_->errorReport ("Switch on the PSU " + (*it).substr(0,loc), LOGDEBUG) ;

    std::string switchOn = (*it).substr(0,loc) ;
    std::string::size_type locChannel = switchOn.find("channel", 0) ;
    if (locChannel == std::string::npos) switchOn += PGCHANNEL ;
    switchOn += DPESETTINGSON ;

    std::string switchOnStatus = (*it).substr(0,loc) ;
    if (locChannel == std::string::npos) switchOnStatus += PGCHANNEL ;
    switchOnStatus += DPESTATUS ;
    std::string psuNameComma = (*it).substr(loc+1,(*it).size()) ;

    // Just to have an entry at that PSU channel
    dcuDescription *dcu0 = new dcuDescription((keyType)0,0,0,0,0,0,0,0,0,0,0) ;
    psuDcuMap[(*it).substr(loc+1,(*it).size())].push_back(dcu0) ;

    try {
      // Send to PVSS
      if (cg) { // enable control group channel
	std::string globalOn = (*it).substr(0,loc) + CGGLOBALON ;
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	std::cout << "Switch " << globalOn << " " << VALUEONSET << std::endl ;
#endif
#ifdef PVSSSWITCH
	xPditor_->dpSet ( globalOn, VALUEONSET ) ; 
	sleep((unsigned int)2) ;
#endif 
      }
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
      std::cout << "Switch " << switchOn << " " << VALUEONSET << std::endl ;
#endif
#ifdef PVSSSWITCH
      xPditor_->dpSet ( switchOn, VALUEONSET ) ; 
#endif 

      std::string dataPointValue = VALUEONGET ; 
      std::stringstream msgUserInfo ; msgUserInfo << "PSU Mapping: switch on the " << std::dec << positionList << " DPe " << switchOn 
						   << " (" << psuNameComma << ") other " 
						   << psuDPeList.size() << " elements" ;
      errorReportLogger_->errorReport (msgUserInfo.str(), LOGUSERINFO) ; 

      int timeout = 0 ;
      do {
	// Wait on the change state
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
      std::cout << "Get " << switchOnStatus << std::endl ;
#endif
#ifdef PVSSSWITCH
	dataPointValue = xPditor_->dpGetSync ( switchOnStatus ) ;
#endif 

	if (dataPointValue != VALUEONGET) {
	  sleep((unsigned int)2) ;
	  timeout ++ ;
	  errorReportLogger_->errorReport ("TIMEOUT => " + toString(timeout) + ": " + switchOnStatus + ": " + dataPointValue, LOGDEBUG) ;

	  // Switch on again
	  if (timeout == (int)(MAXTIMEOUTPSUSWITCHON/2)) {
	    std::stringstream msgWarn ; msgWarn << "PSU Mapping: Warning switch on again the channel " << switchOn << " after " << (int)(MAXTIMEOUTPSUSWITCHON/2) << " attemps" ;
	    errorReportLogger_->errorReport (msgWarn.str(), LOGWARNING) ; 
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	    std::cout << "Switch " << switchOn << " " << VALUEONSET << std::endl ;
#endif
#ifdef PVSSSWITCH
	    xPditor_->dpSet ( switchOn, VALUEONSET ) ; 
#endif 
	  }
	}
      }
      while ((dataPointValue != VALUEONGET) && (timeout <= MAXTIMEOUTPSUSWITCHON)) ;
      
      // Is it on ?
      if (dataPointValue != VALUEONGET) {
	std::stringstream msgError ; msgError << "PSU Mapping: Unable to switch on the control channel " << switchOnStatus ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR); 

	// Output for error in file
	fileErrorOn << (*it) << std::endl ;

	// Switch off by security
	try {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	  std::cout << "Switch " << switchOn << " " << VALUEOFFSET << std::endl ;
#endif
#ifdef PVSSSWITCH
	  xPditor_->dpSet ( switchOn, VALUEOFFSET ) ; 
#endif 
	}
	catch (xcept::Exception &e) {
	  std::stringstream msgError ; msgError << "PSU Mapping: Cannot switch off the power supply channel: " << e.what() ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	}
      }
      else {
	for (std::set<xdaq::ApplicationDescriptor*>::iterator itDes = fecSupervisors.begin() ; itDes != fecSupervisors.end() ; itDes ++) {
	  // Readout the DCU
	  try {
	    // Access each FEC and ask the DCU descriptions
	    int data = DCU_DETECTDCUS ;
	    this->sendTrackerCommand(*itDes, SYST_TRACKER, SUB_SYST_FEC, VERS_FEC, 
				     DCU_DETECTDCUS, &data, 1) ;

	    // Check the anwser
	    PI2O_TRACKER_COMMAND_MESSAGE_FRAME reply = this->getReply() ;

	    // Checking the answer
	    int *ptri = reply->data ;
	    int numberDcu = ptri[0] ;

	    std::stringstream msgInfo ; msgInfo << "receive " << numberDcu << " DCU from FecSupervisor " 
						 << (*itDes)->getInstance() << " for the PSU channel " << psuNameComma ;
	    errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;

	    dcuDescription *dcu = (dcuDescription *)&ptri[1] ;
	    deviceVector uDevice ;
	    for (int i = 0 ; i < numberDcu ; i ++) {
	      dcuDescription *dcuI = new dcuDescription (dcu[i]) ;
	      uDevice.push_back(dcuI) ;

	      if (dcuPSUMapping[dcuI->getDcuHardId()].size() == 0) {
		dcuPSUMapping[dcuI->getDcuHardId()] = (*it) ;
		psuDcuMap[(*it).substr(loc+1,(*it).size())].push_back(dcuI) ;
	      }
	    }
	  }
	  catch (xdaq::exception::Exception& e) {
	    std::stringstream msgError ; msgError << "Unable to contact FecSupervisor instance " << std::dec << (*itDes)->getInstance();
	    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  }
	}

	// Switch off the power supply if it is asked
	if (switchOff) {
	  try {
#if !defined(PVSSSWITCH) || defined(DEBUGPVSSSWITCH)
	    std::cout << "Switch " << switchOn << " " << VALUEOFFSET << std::endl ;
#endif
#ifdef PVSSSWITCH
	    xPditor_->dpSet ( switchOn, VALUEOFFSET ) ; 
#endif
	  }
	  catch (xcept::Exception &e) {
	    std::stringstream msgError ; msgError << "PSU Mapping: Cannot switch off the power supply channel: " << e.what() ;
	    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	  }
	}
      }
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "PSU Mapping: Unable to read/write a datapoint in PVSS: " << e.what() ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }

  std::stringstream msgInfo ; msgInfo << "Mapping between the DCU and PSU done";
  errorReportLogger_->errorReport (msgInfo.str(), LOGUSERINFO) ;
}

/** Parse the file to get the information about control channels and power groups
 * \param file - file name
 * \param controlList - string list with the control channels
 * \param powergroupsList - string list with the power groups
 * \param psuDcuType - set the DCU type (CCU or FEH) for each control list
 * \deprecated file and database are supported by the PSU/DCU map factory
 */
void DcuFilter::getPSUFileInformation ( std::string file, std::list<std::string> &controlChannelList, std::list<std::string> &powerGroupList,
					std::map<std::string, std::string> &psuDcuType ) {
  mutexAppStatus_->take();
  xdaqApplicationStatus_ = errorReportLogger_->getStrProcess ( ) + ": Mapping of the DCU and PSU: retreive all PSU datapoints from file " + file ;
  mutexAppStatus_->give();
  // Open the file
  std::ifstream fichierPSU (file.c_str()) ;  

  // File existing ?
  if (!fichierPSU) return ;

  char coucou[1000] ;

  // Parse it, find the tag "CONTROL CHANNELS" and continue the parsing until the tag "POWER GROUPS"

  // Remove the empty line at the beginning of the files
  do {
    fichierPSU.getline(coucou,1000) ;
  }
  while ( (std::string(coucou) != CONTROLCHANNELSSTART) && (!fichierPSU.eof()) ) ;
  if (fichierPSU.eof()) return ;

  // Get all the control channels
  do {
    fichierPSU.getline(coucou,1000) ;
    if ( (strlen(coucou) != 0) && (std::string(coucou) != POWERGROUPSSTART) ) {
      std::string pgAll(coucou) ;
      std::string::size_type loc = pgAll.find( COMMA, 0 );
      if (loc == std::string::npos) {
	std::stringstream msgError ; msgError << "PSU mapping: Invalid chain (no comma found) " << pgAll ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
      else {
	if (pgAll.size()) {
	  controlChannelList.push_back(pgAll) ;
	  psuDcuType[pgAll.substr(loc+1,pgAll.size())] = DCUCCU ;
	}
	else {
	  std::stringstream msgError ; msgError << "PSU mapping: Invalid chain (size of the chain is 0) " << pgAll ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	}
      }
    }
  }
  while ( (std::string(coucou) != POWERGROUPSSTART) && (!fichierPSU.eof()) ) ;
  if (fichierPSU.eof()) return ;

  // Get all the power groups
  do {
    fichierPSU.getline(coucou,1000) ;
    if (strlen(coucou) != 0) {
      std::string pgAll(coucou) ;
      std::string::size_type loc = pgAll.find( COMMA, 0 );
      if (loc == std::string::npos) {
	std::stringstream msgError ; msgError << "PSU mapping: Invalid chain (no comma found) " << pgAll ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
      else {
	if (pgAll.size()) {
	  powerGroupList.push_back(pgAll) ;
	  psuDcuType[pgAll.substr(loc+1,pgAll.size())] = DCUFEH ;
	}
	else {
	  std::stringstream msgError ; msgError << "PSU mapping: Invalid chain (size of the chain is 0) " << pgAll ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	}
      }
    }
  }
  while (!fichierPSU.eof()) ;
  // Continue the parsing until the end of the file

  // Close the file
  fichierPSU.close() ;

  // Display the control groups
  if (!controlChannelList.size()) errorReportLogger_->errorReport ("No Control channel found", LOGERROR) ;
  if (!powerGroupList.size()) errorReportLogger_->errorReport ("No power group found", LOGWARNING) ;
  if (displayDebugMessage_) {
    if (controlChannelList.size()) {
      
      std::cout << "Found " << controlChannelList.size() << " control channels are: " << std::endl ;
      for (std::list<std::string>::iterator it = controlChannelList.begin() ; it != controlChannelList.end() ; it ++) {
	std::cout << "\t" << *it << std::endl ;
      }
    }
    
    if (powerGroupList.size()) {
      std::cout << "Found " << powerGroupList.size() << " power groups are: " << std::endl ;
      for (std::list<std::string>::iterator it = powerGroupList.begin() ; it != powerGroupList.end() ; it ++) {
	std::cout << "\t" << *it << std::endl ;
      }
    }
  }
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Action performed                                                           */
/*                                                                                                              */
/* ************************************************************************************************************ */
void DcuFilter::actionPerformed(xdata::Event& event) {

  errorReportLogger_->errorReport ("An action performed as been done but nothing is implemented !", LOGERROR) ;
}

/**
 * SOAP Callback for FSM
 */
xoap::MessageReference DcuFilter::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception, xcept::Exception) {

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

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement ( responseName );
	return reply;
      }
      catch (toolbox::fsm::exception::Exception & e) {
	std::stringstream msgError ;
	msgError << __PRETTY_FUNCTION__ << " line " << __LINE__ << ", invalid command: " << e.what() << " for command " <<  commandName ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	XCEPT_RETHROW(xcept::Exception, "invalid command", e);
      }
    }
  }
  
  std::stringstream msgError ;
  msgError << __PRETTY_FUNCTION__ << " line " << __LINE__ << ", command not found for (cannot display message)" ; //<< msg ;
  errorReportLogger_->errorReport ("Command not found", LOGERROR) ;
  XCEPT_RAISE(xcept::Exception,"command not found");	
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Web interfaces                                                             */
/*                                                                                                              */
/* ************************************************************************************************************ */
// ------------------------------------------------------------------------------------------------------------ //
// Default page
// ------------------------------------------------------------------------------------------------------------ //

void DcuFilter::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport (Default page (state = " + fsm_.getCurrentState() + ")" , LOGDEBUG) ;
  
  if (fsm_.getCurrentState() == 'I' ) {
    this->displayInitialisePage(in,out);
  }
  else {
    switch (linkPosition_) {
    case 0:
      displayRelatedLinks (in,out,true) ;
      break ;
    case 1:
      displayInitialisePage(in,out) ;
      break ;
    case 2:
     	displayDcuFilter(in,out) ;
      break ;
    case 3:
      displayDcuList(in,out) ;
      break ;
#ifdef TKDIAG
    case 4:
      displayConfigureDiagSystem(in,out) ;
      break ;
#endif
    case 5:
      displayPVSSTest(in,out) ;
      break ;
    case 6:
      displayStateMachine(in,out) ;
      break ;
    default:
      displayRelatedLinks (in,out,true) ;
      break ;
    }
//     std::string fecRelatedLinksURL = "/";
//     fecRelatedLinksURL += getApplicationDescriptor()->getURN();
//     fecRelatedLinksURL += "/FecRelatedLinks";
//     std::string displayDcuFilterURL = "/";
//     displayDcuFilterURL += getApplicationDescriptor()->getURN();
//     displayDcuFilterURL += "/displayDcuFilter";

//     //*out << cgicc::HTTPHTMLHeader();
//     *out << "<HTML>" << std::endl ;
//     *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
//     *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
//     *out << cgicc::title("DCU Filter") << std::endl;
//     *out << " <frameset cols=\"20%,80%\" >" << std::endl;
//     *out << cgicc::frame().set("src",fecRelatedLinksURL) << std::endl; 
//     *out << cgicc::frame().set("src",displayDcuFilterURL) << std::endl ;
//     //*out << " <frameset rows=\"30%,70%\" >" << std::endl;
//     //*out << cgicc::frame().set("src",statemachineURL)<< std::endl;
//     //*out << cgicc::frame().set("src",parameterURL)<< std::endl;
//     *out << " </frameset> "<< std::endl;
//     *out << " </frameset> "<< std::endl;
//     *out << "</HTML>" << std::endl ;
  }
}

// ------------------------------------------------------------------------------------------------------------ //
// Initialise state
// ------------------------------------------------------------------------------------------------------------ //

/** Initialise page or parameter page
 */
void DcuFilter::displayInitialisePage(xgi::Input * in, xgi::Output * out) {

  //errorReportLogger_->errorReport ("displayInitialisePage", LOGDEBUG) ;

  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  if (fsm_.getCurrentState() == 'I' ) {
    *out << cgicc::title("DcuFilter Initialisation") << std::endl;
    xgi::Utils::getPageHeader(*out, "DcuFilter Initialisation");
  }
  else {
    *out << cgicc::title("DcuFilter Parameters") << std::endl;
    xgi::Utils::getPageHeader(*out, "DcuFilter Parameters");
    // Links
    displayRelatedLinks (in,out) ;
    linkPosition_ = 1 ;
  }

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();

  url += "/dcuFilterInitialise";	

// #ifdef TKDIAG
//   if (fsm_.getCurrentState() != 'I' ) {
//     // Configuration of the diagnostic system
//     DIAG_SET_CONFIG_BUTTON_CALLBACK();
//   }
// #endif

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
  // Conversion Factors
  // Size of all the next fields
  std::string sizeField = "30" ;

  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Conversion Factors") << std::endl;
  //*out << cgicc::br() << std::endl ;

  // If the conversion is asked
  if (doConversion_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doConversion").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doConversion") << std::endl;
  *out << cgicc::label("Conversion between ADC count and real values")  << std::endl;

  // Database or file access, force if no database is specified
  if (doConversion_ && !databaseAccess_ && !conversionFromFile_) conversionFromFile_ = true ;
  if (conversionFromFile_)
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","conversionfactorsource")
      .set("size","30")
      .set("value","file")
      .set("selected")
      .set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","conversionfactorsource")
      .set("size","30")
      .set("value","file")
      .set("selected") << std::endl ;
  *out << cgicc::label("File Conversion Factors")  << std::endl;
    
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("File Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","xmlConversionInputFile")
    .set("size","80")
    .set("value",xmlConversionInputFile_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;
    
  // Database parameters
#ifdef DATABASE
  // First check dyanmically if the values are set
  std::string dblogin = "nil", dbpass  = "nil", dbpath = "nil" ;
  DbFecAccess::getDbConfiguration (dblogin, dbpass, dbpath) ;

  // Database login ...
  if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
      (dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) databaseAccess_ = false ;
    
  // Database access
  if (doConversion_ && databaseAccess_ && !conversionFromFile_)
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","conversionfactorsource")
      .set("size","30")
      .set("value","database")
      .set("selected")
      .set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","conversionfactorsource")
      .set("size","30")
      .set("value","database")
      .set("selected") << std::endl ;
  *out << cgicc::label("Database Conversion Factors")  << std::endl;
    
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

  // Check if the env variable has been set to display the correct name 
  // please note if this value is set then nothing can be done to change it
  // Fix the partition name
  if ( (partitionName_ == "") || (partitionName_ == DCUFILTERPARTITIONNONE) ) {
    char *basic=getenv ("ENV_CMS_TK_PARTITION") ;
    if (basic != NULL) partitionName_ = std::string(basic) ;
  }

  if ( (partitionName_ == "") || (partitionName_ == DCUFILTERPARTITIONNONE) ) {
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

  // End of the field
  *out << cgicc::fieldset() << std::endl;
  // End of Conversion Factors
  // --------------------------------------------------------------------

  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  // PVSS Configuration
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("PVSS Configuration") << std::endl;
  //*out << cgicc::br() << std::endl ;

  // Upload to PVSS
  if (doUploadPVSS_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doUploadPVSS").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doUploadPVSS") << std::endl;
  *out << cgicc::label("Upload to PVSS")  << std::endl;

  // Reach the class name and instance
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("PVSS Class Name ") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","pvssClassName")
    .set("size",sizeField)
    .set("value",pvssClassName_.toString()) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("PVSS Instance ") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","pvssInstance")
    .set("size",sizeField)
    .set("value",pvssInstance_.toString()) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("SOAP action string ") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","soapActionString")
    .set("size",sizeField)
    .set("value",soapActionString_.toString()) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::p() << cgicc::label("DCU PVSS Point Name ") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
     .set("name","pvssPoint")
    .set("size",sizeField)
    .set("value",pvssPoint_.toString()) << std::endl; 
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::p() << cgicc::label("Time before next send ") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
     .set("name","timePVSSSend")
    .set("size",sizeField)
    .set("value",timePVSSSend_.toString()) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::p() << cgicc::label("% to reach before sending to PVSS ") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
     .set("name","pourcentDifferenceForPVSS")
    .set("size",sizeField)
    .set("value",pourcentDifferenceForPVSS_.toString()) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;

  // End of the field
  *out << cgicc::fieldset() << std::endl;
  // End of Configuration
  // --------------------------------------------------------------------

  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  // General Configuration
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("PSU / DCU Mapping") << std::endl;
  //*out << cgicc::br() << std::endl ;

  // Warning if in Initialise state
  //if (fsm_.getCurrentState() == 'I' ) 
  *out << "<span style=\"color: rgb(204, 0, 0);\">The automatic procedure will be done only if you click on the Configure or in the [StateMachine] tab or in the RCMS panel</span>" << std::endl ;

  // Make the mapping between the DCU and PSU
  if (dcuPSUChannelDetection_)
     *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","dcuPSUChannelDetection").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","dcuPSUChannelDetection") << std::endl;

  *out << cgicc::label("Mapping between the PSU and the DCU (requiered a file describing the PSU datapoints)")  << std::endl;
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>" << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Switch off the control group once the DCU dectection is done ") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  if (switchOffControlGroup_) 
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","switchOffControlGroup").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","switchOffControlGroup") << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Database or file name for the PSU datapoints ") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
     .set("name","psuConfigurationFile")
    .set("size","80")
    .set("value",psuConfigurationFile_.toString()) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << cgicc::table() << std::endl;

  if (doUploadPSUDCUMapFile_) 
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","doUploadPSUDCUMapFile")
      .set("size","30")
      .set("value","file")
      .set("selected")
      .set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","doUploadPSUDCUMapFile")
      .set("size","30")
      .set("value","file")
      .set("selected") << std::endl ;
  *out << cgicc::label("Upload the PSU/DCU Map in a file")  << std::endl;
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("File Name ") << std::endl;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
    .set("name","psuDcuMapFileName")
    .set("size","80")
    .set("value",psuDcuMapFileName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;
#ifdef DATABASE
  if (databaseAccess_ && !doUploadPSUDCUMapFile_)
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","doUploadPSUDCUMapFile")
      .set("size","30")
      .set("value","database")
      .set("selected")
      .set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type","radio")
      .set("name","doUploadPSUDCUMapFile")
      .set("size","30")
      .set("value","database")
      .set("selected") << std::endl ;
  *out << cgicc::label("Upload the PSU/DCU Map in database")  << std::endl;
#endif

  // End of the field
  *out << cgicc::fieldset() << std::endl;
  // --------------------------------------------------------------------

  *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  // General Configuration
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("General Configuration") << std::endl;
  //*out << cgicc::br() << std::endl ;

  // Upload to the configuration database
  if (doUploadInDatabase_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doUploadInDatabase").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doUploadInDatabase") << std::endl;
  *out << cgicc::label("Upload to Configuration Database")  << std::endl;

  // Upload into a file
  std::string directoryDcuDataName = directoryDcuDataName_.toString() ;
  std::string::size_type ipos = directoryDcuDataName.find(partitionName_.toString());
  if (ipos == std::string::npos) {
    std::stringstream directory ; directory << directoryDcuDataName_.toString() << "/" << partitionName_.toString() ;
    directoryDcuDataName_ = directory.str() ;
  }

  if (doUploadInFile_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doUploadInFile").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","doUploadInFile") << std::endl;
  *out << cgicc::label("Upload in a file (" + directoryDcuDataName_.toString() + "/DCURaw{TimeStamp}.xml)")  << std::endl;

  // directory output
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Directory Path for the output files") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
     .set("name","directoryDcuDataName")
    .set("size","80")
    .set("value",directoryDcuDataName_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;

  // Upload the errors in DCU conversion factors or in PVSS DPe in an file
  if (dumpDCUMissing_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","dumpDCUMissing").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","dumpDCUMissing") << std::endl;
  *out << cgicc::label("Dump the DCU without conversion factors or in error for PVSS")  << std::endl;
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("File where the errors will be stored") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
     .set("name","dumpDCUMissingFile")
    .set("size","80")
    .set("value",dumpDCUMissingFile_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;

  // Upload the differences in a file
  if (checkDCUData_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","checkDCUData").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","checkDCUData") << std::endl;
  *out << cgicc::label("Check the DCU data between two readout")  << std::endl;
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  *out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("File where the differences will be stored") << std::endl ;
  *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::input().set("type","text")
     .set("name","checkDcuDataFile")
    .set("size","80")
    .set("value",checkDcuDataFile_) << std::endl;
  *out << "</td>" << std::endl;
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;

  // Display debug messages
  if (displayDebugMessage_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","displayDebugMessage").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","displayDebugMessage") << std::endl;
  *out << cgicc::label("Display debug messages on console")  << std::endl;

  // Display debug messages
  if (ignoreErrorMessage_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","ignoreErrorMessage").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","ignoreErrorMessage") << std::endl;
  *out << cgicc::label("Do not raise error on any problem (Conversion factors missing or PVSS sending)")  << std::endl;

  // Status of the application
#ifndef FEC_GREEN
#define FEC_GREEN "<span style=\"color: rgb(51, 204, 0);\">"
#endif
  mutexAppStatus_->take();
  *out << cgicc::p() << FEC_GREEN << "Last action performed: " << xdaqApplicationStatus_.toString() << "</span>" << std::endl ;
  mutexAppStatus_->give();
  // End of the field
  *out << cgicc::fieldset() << std::endl;
  // End of Configuration
  // --------------------------------------------------------------------

  *out << cgicc::p() << std::endl ;

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

  // End of the form
  *out << cgicc::form() << std::endl;
	
  //xgi::Utils::getPageFooter(*out);
  *out << "</HTML>" << std::endl ;
}

/** Initialise action or apply on the parameters
 */
void DcuFilter::dcuFilterInitialise(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("dcuFilterInitialise", LOGDEBUG) ;

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Display the debug messages
    displayDebugMessage_ = cgi.queryCheckbox("displayDebugMessage") ;
    ignoreErrorMessage_ = cgi.queryCheckbox("ignoreErrorMessage") ;
    errorReportLogger_->setlogLevel (displayDebugMessage_ ? LOGDEBUG : LOGWARNING) ;
    errorReportLogger_->setDisplayOutput(displayDebugMessage_) ;

    // -------------------------------------------------------------------
    // A quick display
    if (displayDebugMessage_) {
      std::stringstream msgDebug ; 
      if (cgi.queryCheckbox("doConversion")) {
	msgDebug << "Conversion applied, source = " << cgi["conversionfactorsource"]->getValue() << ": " ;

	if (cgi["conversionfactorsource"]->getValue() == "file") {
	  msgDebug << "\tConversion file: " << cgi["xmlConversionInputFile"]->getValue() ;
	}
	else
	  msgDebug << "\tDatabase account: " << dbLogin_.toString() << "/" << dbPasswd_.toString() << "@" << dbPath_.toString() ;
      }
      else {
	msgDebug << "Conversion not applied" ;
      }
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
    }

    // -------------------------------------------------------------------
    // Conversion Factors
    doConversion_ = cgi.queryCheckbox("doConversion") ;
    if (doConversion_) {
      if (cgi["conversionfactorsource"]->getValue() == "file") {
	xmlConversionInputFile_ = cgi["xmlConversionInputFile"]->getValue() ;
	conversionFromFile_ = true ;
      }
      else conversionFromFile_ = false ;

      // Retreive the conversion factors from file or database
      if (doConversion_ && conversionFromFile_ && (fileConversionParsed_ != xmlConversionInputFile_.toString()) && (fsm_.getCurrentState() != 'I' )) { getConversionFactors() ; }
    }

#ifdef DATABASE
    // Database access
    dbLogin_ = cgi["dbLogin"]->getValue();
    dbPasswd_ = cgi["dbPasswd"]->getValue();
    dbPath_ = cgi["dbPath"]->getValue();

    if (! conversionFromFile_ && databaseAccess_) {
      if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
	  (dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) {

	databaseAccess_ = false ;
	doConversion_ = false ;
	
	// Error reporting
	errorReportLogger_->errorReport ("Cannot create a database access, disable the conversion", LOGERROR) ;
	lastErrorMessage_ = "Cannot create a database access, disable the conversion" ;
	errorOccurs_ = true ;
      }
      else 
	if ( doConversion_ && ( (partitionName_.toString() != cgi["partitionName"]->getValue()) || (dcuConversionFactorsMap_.size() == 0) ) ) {
	  if (fsm_.getCurrentState() != 'I' ) { getConversionFactors() ;}
	  errorReportLogger_->errorReport ("The conversion factors will be retreiven from the database", LOGUSERINFO) ;
	}
    }
#endif

    if (conversionFromFile_) { 
      if (xmlConversionInputFile_ == UNKNOWNFILE) {

	// Error reporting
	errorReportLogger_->errorReport ("No conversion file has been specified, disable the conversion", LOGERROR) ;

	doConversion_ = false ;
      }
      else if (fsm_.getCurrentState() != 'I' ) { getConversionFactors() ;}
    }
  
    // -------------------------------------------------------------------
    // Configuration
    doUploadPVSS_  = cgi.queryCheckbox("doUploadPVSS") ;
    std::string pvssClassName = cgi["pvssClassName"]->getValue() ;
    unsigned int pvssInstance = fromString<unsigned int>(cgi["pvssInstance"]->getValue()) ;
    std::string soapActionString = cgi["soapActionString"]->getValue() ;
    pvssPoint_     = cgi["pvssPoint"]->getValue() ;
    // Datapoint name
    runNumberDpe_ = pvssPoint_.toString() + RUNNUMBERDPE ;
    dcuPsuMapResult_ = pvssPoint_.toString() + DCUPSUMAPRESULT ;
    dcuPsuMapProgressCG_ = pvssPoint_.toString() + DCUPSUMAPPROGRESSCG ;
    dcuPsuMapProgressPG_ = pvssPoint_.toString() + DCUPSUMAPPROGRESSPG ;
    runTypeMessage_ = pvssPoint_.toString() + RUNTYPEMESSAGE ;
    timePVSSSend_  = fromString<unsigned int>(cgi["timePVSSSend"]->getValue()) ;
    // % of difference
    pourcentDifferenceForPVSS_ = fromString<unsigned int>(cgi["pourcentDifferenceForPVSS"]->getValue()) ;

    // Create the access to PVSS
    //if ( (!doUploadPVSS_) && (xPditor_ != NULL) ) { delete xPditor_ ; xPditor_ = NULL ; } => cannot be deleted here to avoid problem with the work loop
    if ( doUploadPVSS_ ) {
      std::ostringstream originator ; originator << "DcuFilter" << partitionName_.toString() << getApplicationDescriptor()->getInstance() ;
      if ( (originatorStr_ != originator.str()) || (xPditor_ == NULL) || 
	   (pvssClassName_.toString() != pvssClassName) || 
	   ((unsigned int)pvssInstance_ != pvssInstance) ||
	   (soapActionString_.toString() != soapActionString) ) { 

	errorReportLogger_->errorReport ("Creation of the PSX interface with the parameters: " + originatorStr_, LOGDEBUG) ;
	if (xPditor_ != NULL) { delete xPditor_ ; xPditor_ = NULL ; } 

	originatorStr_ = originator.str() ;
	pvssClassName_ = pvssClassName ;
	pvssInstance_ = pvssInstance ;
	soapActionString_ = soapActionString ;

	try {
	  if (pvssClassName_.toString() != "") {
	    std::stringstream msgInfo ; msgInfo << "Access to PVSS performed on URL " << pvssClassName_.toString() 
						<< " instance " << pvssInstance_.toString() << " (" << soapActionString_.toString() << ")" ;
	    errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	    xPditor_ = new XPditor ( this, originatorStr_, pvssClassName_.toString(), pvssInstance_, soapActionString_.toString()) ;
	  }
	  else {
	    errorReportLogger_->errorReport ("PVSS URL empty: cannot open a connection to PSX", LOGERROR) ;
	  }
	}
	catch (xdaq::exception::Exception& e) {
	  
	  doUploadPVSS_ = false ;
	  std::stringstream msgError ; msgError << "Unable to access PVSS on " << pvssClassName_.toString() << " instance " << pvssInstance_.toString()
						<< " (" << soapActionString_.toString() << ") "
						<< "=> (error = " << e.what() << ")" ;
	  errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
	}
      }
      else 
	errorReportLogger_->errorReport ("PSX interface already created with the same parameters: " + originatorStr_ + " on " + pvssClassName_.toString() + " instance " + pvssInstance_.toString() + ", " + soapActionString_.toString(), LOGDEBUG) ;
    }

    // -------------------------------------------------------------------
    // Configuration
    doUploadInDatabase_   = cgi.queryCheckbox("doUploadInDatabase") ;
    doUploadInFile_       = cgi.queryCheckbox("doUploadInFile") ;
    directoryDcuDataName_ = cgi["directoryDcuDataName"]->getValue() ;
    checkDCUData_         = cgi.queryCheckbox("checkDCUData") ;
    checkDcuDataFile_     = cgi["checkDcuDataFile"]->getValue() ;

    // DCU missing storage
    if (cgi.queryCheckbox("dumpDCUMissing")) {
      if (!dumpDCUMissing_) { // if not opened then open it 
	dumpDCUMissingFile_   = cgi["dumpDCUMissingFile"]->getValue() ;
	dumpDCUMissingStream_ = new std::ofstream(dumpDCUMissingFile_.c_str()) ;
	dumpDCUMissing_ = cgi.queryCheckbox("dumpDCUMissing") ;
      }
    }
    else if (dumpDCUMissing_) { // close it
      dumpDCUMissing_ = cgi.queryCheckbox("dumpDCUMissing") ;
      if (dumpDCUMissingStream_ != NULL) {
	dumpDCUMissingStream_->close() ;
	delete dumpDCUMissingStream_ ; dumpDCUMissingStream_ = NULL ;
      }
    }

    // --------------------------------------------------------------------
    // Mapping between DCU and PSU
    dcuPSUChannelDetection_ =  cgi.queryCheckbox("dcuPSUChannelDetection") ;
    switchOffControlGroup_ =  cgi.queryCheckbox("switchOffControlGroup") ;
    psuConfigurationFile_   = cgi["psuConfigurationFile"]->getValue() ;
    if (cgi["doUploadPSUDCUMapFile"]->getValue() == "file") doUploadPSUDCUMapFile_ = true ;
    else doUploadPSUDCUMapFile_ = false ;
    psuDcuMapFileName_ = cgi["psuDcuMapFileName"]->getValue() ;
    
    // Scan the DCU for the power supply channels
    // if ((dcuPSUChannelDetection_) && (fsm_.getCurrentState() != 'I')) detectDcuPSUChannels ( ) ;

    // Check if the database or file access has been already created
    if (doUploadInDatabase_ || doUploadInFile_) {

      if (fecDeviceFactory_ != NULL) {
	if ( ((fecDeviceFactory_->getDbUsed() && !doUploadInFile_)) ||
	     ((!fecDeviceFactory_->getDbUsed() && doUploadInDatabase_)) ) {
	  delete fecDeviceFactory_ ;
	  fecDeviceFactory_ = NULL ;
	}
      }

      if (fecDeviceFactory_ == NULL) {
#ifdef DATABASE
	if (doUploadInDatabase_ && databaseAccess_) {
	  // Database access
	  if (databaseAccess_) createDatabaseAccess () ;
	}
	else 
#endif
	  {
	    doUploadInDatabase_ = false ;
	    if (doUploadInFile_) {
	      fecDeviceFactory_ = new FecDeviceFactory ( ) ;
	    }
	  }
      }
    }
    else {
      // Destroy the database or file access
      if (fecDeviceFactory_ != NULL) { delete fecDeviceFactory_ ; fecDeviceFactory_ = NULL ; }
    }
  }
  catch(const std::exception& e) {
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  if (fsm_.getCurrentState() == 'I' ) {		

    try {
      toolbox::Event::Reference e(new toolbox::Event(INITIALISE,this));
      fsm_.fireEvent(e);
    }
    catch (toolbox::fsm::exception::Exception & e) {
      XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
      errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    }
  }
   
  this->Default(in,out);
}

/** Display all the parameters for the DCU Filter
 */
void DcuFilter::displayRelatedLinks(xgi::Input * in, xgi::Output * out, bool withHTMLTag) {

  //errorReportLogger_->errorReport ("displayRelatedLinks", LOGDEBUG) ;

  if (withHTMLTag) {
    // ----------------------------------------------------------------------
    // Here start the WEB pages
    *out << "<HTML>" << std::endl ;
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DCU Filter Parameters") << std::endl;
    xgi::Utils::getPageHeader(*out, "DCU Filter Parameters");
  }
  
  *out << "<p style=\"font-family:arial;font-size:10pt;color:#0000AA\">" ;
  for (unsigned long i = 0; i < relatedLinksNavigation_.size(); i++) {
    *out << "[" << relatedLinksNavigation_[i] << "] " ;
  }
  *out << "</p>" << std::endl ;
  
  if (withHTMLTag) {
    //xgi::Utils::getPageFooter(*out);
    *out << "</HTML>" << std::endl ;
  }

//   //*out << cgicc::HTTPHTMLHeader();
//   *out << "<HTML>" << std::endl ;
//   *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
//   *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
//   *out << cgicc::title("DCU Filter related links") << std::endl;
  
//   //xgi::Utils::getPageHeader(*out, "FecSupervisor related links");
//   *out << cgicc::h3("DcuFilter: Related Links").set("style", "font-family: arial") << std::endl;
  
//   *out << "<ul>" << std::endl ;
//   for (unsigned long i = 0; i < relatedLinksNavigation_.size(); i++) {
//     *out << "<li>" << relatedLinksNavigation_[i] ;
//   }
//   *out << "</ul>" << std::endl ;
//   *out << "</HTML>" << std::endl ; 
}

/** Initialise page
 */
void DcuFilter::displayDcuFilter (xgi::Input * in, xgi::Output * out) {

  //errorReportLogger_->errorReport ("displayDcuFilter". LOGDEBUG) ;

  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("DcuFilter Main Page") << std::endl;

  // Create HTML header for auto-refresh
  //*out << "<HEAD>" << std::endl;
  //if ( (autoRefreshEnabled_ == true) && (internalState_ == 3) ) {
  //*out << "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"";
  //*out << textRefreshRate_;
  //*out << "\">" << std::endl << "</HEAD>" << std::endl;
  //}
  //*out << "</HEAD>" << std::endl;

  xgi::Utils::getPageHeader(*out, "DcuFilter Main Page");

  // Links
  displayRelatedLinks (in,out) ;
  linkPosition_ = 2 ;

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();

  url += "/dcuListApply";	

  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  // ----------------------------------------------------------------------
  // Apply button
  *out << cgicc::p() << cgicc::h2("Click on Apply to refresh the values or to change the parameters").set("style","font-size: 15pt;  font-family: arial; color: rgb(0, 0, 0); font-weight: bold;")
       << cgicc::input().set("type", "submit").set("name", "submit").set("value", "Apply");
  

  // ----------------------------------------------------------------------
  // Status
  // Check if an error occurrs and display the related message
  *out << cgicc::p() << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Status") << std::endl;
  
  if (errorOccurs_) {
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2(lastErrorMessage_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    //*out << cgicc::td() << std::endl;
    errorOccurs_ = false ;
  }
  
  // Conversion ?
  if (doConversion_) *out << cgicc::p() << cgicc::label("Conversion applied") << std::endl ;
  else *out << cgicc::p() << cgicc::label("No conversion applied") << std::endl ;
  
  // Counter status
  std::stringstream msgInfo ; msgInfo << "Number of DCU values received: " << counter_ ;
  *out << cgicc::p() << cgicc::label(msgInfo.str().c_str()) << std::endl;

  if (doUploadPVSS_) {
    time_t now ;
    time(&now) ;
    std::stringstream msgInfo2 ; msgInfo2 << "Number of DCU sent to PVSS: " << numberOfDcuSentToPVSS_ << " at time " << ctime(&now) ;
    errorReportLogger_->errorReport (msgInfo2.str(), LOGDEBUG) ;
    *out << cgicc::p() << cgicc::label(msgInfo2.str().c_str()) << std::endl;
  }
  
  // End of the field
  *out << cgicc::fieldset() << std::endl;
  // --------------------------------------------------------------------

  *out << cgicc::p() << std::endl ;  

  // --------------------------------------------------------------------
  // Configuration
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Parameters") << std::endl;
  //*out << cgicc::br() << std::endl ;

  // Upload to PVSS
  if (displayDcuList_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","displayDcuList").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","displayDcuList") << std::endl;
  *out << cgicc::label("Display the DCU list")  << std::endl;

  // Reset the counter
  *out << cgicc::p() << cgicc::input().set("type", "checkbox")
    .set("name","resetCounter") << std::endl;
  *out << cgicc::label("Reset the counter of DCU received")  << std::endl;

  // End of the field
  *out << cgicc::fieldset() << std::endl;
  // --------------------------------------------------------------------

  *out << cgicc::p() << std::endl ; 

  // ---------------------------------------------------------------------
  // Display the DCU list if asked
  if (displayDcuList_) displayDcuList(in,out) ;

  *out << cgicc::p() << std::endl ;

  // Apply button
  *out << cgicc::p() << cgicc::h2("Click on Apply to refresh the values or to change the parameters").set("style","font-size: 15pt;  font-family: arial; color: rgb(0, 0, 0); font-weight: bold;") 
       << cgicc::input().set("type", "submit").set("name", "submit").set("value", "Apply");

  // End of the form
  *out << cgicc::form() << std::endl;

  *out << "</HTML>" << std::endl ; 
}

/** Apply of the main parameters
 */
void DcuFilter::dcuListApply(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("dcuFilterInitialise", LOGDEBUG) ;

  //#define TEST
#ifdef TEST
  static unsigned int dcuHardId = 1 ;
  keyType accessKey = buildCompleteKey(0,0,0,dcuHardId,0) ;
  dcuDescription *dcuD = new dcuDescription (accessKey,
					     time(NULL),
					     dcuHardId,
					     dcuHardId,
					     dcuHardId,
					     dcuHardId,
					     dcuHardId,
					     dcuHardId,
					     dcuHardId,
					     dcuHardId,
					     dcuHardId) ;
  dcuD->setFecHardwareId ("FEC " + toString(dcuHardId)) ;
  
  if (dcuValuesReceived_.find(dcuD->getDcuHardId()) != dcuValuesReceived_.end()) { 
    //errorReportLogger_->errorReport ("Delete for DCU Hard ID " + toString(dcuD->getDcuHardId()), LOGDEBUG) ;
    delete dcuValuesReceived_[dcuD->getDcuHardId()] ;
  }
  dcuValuesReceived_[dcuD->getDcuHardId()] = dcuD ;
 
  counter_ ++ ;
  dcuHardId ++ ;
#endif

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Display the DCU List
    displayDcuList_ = cgi.queryCheckbox("displayDcuList") ;

    // Reset the counter
    if (cgi.queryCheckbox("resetCounter")) counter_ = 0 ;
    
  }
  catch(const std::exception& e) {
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  //this->displayDcuList(in,out);
  this->Default(in,out) ;
}

/** Display the DCU list of the FECs
 */
void DcuFilter::displayDcuList (xgi::Input * in, xgi::Output * out) {

   //errorReportLogger_->errorReport ("displayDcuList",LOGDEBUG) ;

  if (linkPosition_ == 3) {
    *out << "<HTML>" << std::endl ;
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DcuFilter Main Page") << std::endl;
    xgi::Utils::getPageHeader(*out, "DcuFilter Main Page");
    
    // Links
    displayRelatedLinks (in,out) ;
    linkPosition_ = 3 ;
    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    
    url += "/dcuListApply";	

    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;

    // ----------------------------------------------------------------------
    // Apply button
    *out << cgicc::p() << cgicc::h2("Click on Apply to refresh the values or to change the parameters").set("style","font-size: 15pt;  font-family: arial; color: rgb(0, 0, 0); font-weight: bold;")
	 << cgicc::input().set("type", "submit").set("name", "submit").set("value", "Apply");
  }


  *out << "<CENTER>" << cgicc::h2("DCU list").set("style","font-size: 15pt;  font-family: arial; color: rgb(0, 0, 0); font-weight: bold;") << "</CENTER>" << std::endl;
    
  if (dcuValuesReceived_.size() == 0) *out << "<CENTER>" << "No DCU has been received" << "</CENTER>" << std::endl;
  else {
    
    *out << "<CENTER>" << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0") << std::endl ;
    *out << "<tr>" << "<td>" << std::endl ;
    *out << "DCU Hardware ID" ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << "FEC Hardware ID" ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << "FEC Position" ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << "Conversion Factors" ;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    
    std::string url = getApplicationDescriptor()->getURN();      

    //errorReportLogger_->errorReport ("Found " + toString(dcuValuesReceived_.size()) + " in the display of the DCU list", LOGDEBUG) ;

    for (DcuValuesReceivedType::iterator p=dcuValuesReceived_.begin() ; p!=dcuValuesReceived_.end() ; p++) {

      dcuDescription *dcu = p->second ;
      
      if (dcu != NULL) {

	std::ostringstream dcuPosition ; char msg[80] ; decodeKey(msg,dcu->getKey()) ; dcuPosition << "DCU on " << msg ;

	// Check the DCU hard id
	if (dcu->getDcuHardId() == 0) {
	  char msg[80] ;
	  decodeKey(msg,dcu->getKey()) ;
	  std::stringstream msgError ; msgError << "Receive an invalid DCU hardware id (0) on " << msg ;
	  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	}
	
	// Conversion factors
	TkDcuConversionFactors *conversionFactors = NULL ;
	if (doConversion_) conversionFactors = getTkDcuConversionFactors ( *dcu ) ;

	// Display it
	*out << "<tr>" << "<td>" << std::endl ;
	*out << "<a href=\"/" << url << "/displayDcu?param1=" << toString(dcu->getDcuHardId()) << "\" target=\"_blank\">" << toString(dcu->getDcuHardId()) << "</a>" << std::endl ;
	*out << "</td>" << "<td>" ;
	*out << "<a href=\"/" << url << "/displayDcu?param1=" << toString(dcu->getDcuHardId()) << "\" target=\"_blank\">" << dcu->getFecHardwareId() << "</a>" << std::endl ;
	*out << "</td>" << "<td>" ;
	*out << "<a href=\"/" << url << "/displayDcu?param1=" << toString(dcu->getDcuHardId()) << "\" target=\"_blank\">" << dcuPosition.str() << "</a>" << std::endl ;
	*out << "</td>" << "<td>" ;

	if ( (conversionFactors != NULL) && (conversionFactors->getDcuHardId() != 0) ) // Conversion factors found
	  *out << "<a href=\"/" << url << "/displayDcu?param1=" << toString(dcu->getDcuHardId()) << "\" target=\"_blank\">" << "Yes" << "</a>" << std::endl ;
	else
	  if (conversionFactors != NULL)
	    *out << "<a href=\"/" << url << "/displayDcu?param1=" << toString(dcu->getDcuHardId()) << "\" target=\"_blank\">" << "<span style=\"color: rgb(255, 102, 0);\">No (use default)</span>" << "</a>" << std::endl ;
	  else
	    *out << "<a href=\"/" << url << "/displayDcu?param1=" << toString(dcu->getDcuHardId()) << "\" target=\"_blank\">" << "<span style=\"color: rgb(255, 0, 0);\">No</span>" << "</a>" << std::endl ;

	*out <<"</td>" << "</tr>" << std::endl ;
      }
    }
    *out << cgicc::table() << "</CENTER>" << std::endl;
  }

  if (linkPosition_ == 3) {
    *out << cgicc::p() << std::endl ;
    
    // Apply button
    *out << cgicc::p() << cgicc::h2("Click on Apply to refresh the values or to change the parameters").set("style","font-size: 15pt;  font-family: arial; color: rgb(0, 0, 0); font-weight: bold;") 
	 << cgicc::input().set("type", "submit").set("name", "submit").set("value", "Apply");
    
    // End of the form
    *out << cgicc::form() << std::endl;
    
    *out << "</HTML>" << std::endl ; 
  }
}

/** Display the values for the DCU
 */
void DcuFilter::displayDcu (xgi::Input * in, xgi::Output * out) {

  //errorReportLogger_->errorReport ("displayDcu",LOGDEBUG) ;

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);
    
    std::string dcuHardIdStr = cgi["param1"]->getValue() ;
    std::string title = "DCU " + dcuHardIdStr ;
    tscType32 dcuHardId = fromString<tscType32>(dcuHardIdStr) ;
    //errorReportLogger_->errorReport ("DCU Hard ID = " + toString(dcuHardId), LOGDEBUG) ;

    *out << "<HTML>" << std::endl ;
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

    //std::string url = "/";
    //url += getApplicationDescriptor()->getURN();
    //url = url + "/displayDcu?param1=" + dcuHardIdStr;
    //*out << cgicc::form().set("method","post").set("action", url).set("enctype","multipart/form-data") << std::endl;

    // ----------------------------------------------------------------------
    // Update button
    //*out << cgicc::p() << cgicc::h2("Click on Update to refresh the values or to change the parameters").set("style","font-size: 15pt;  font-family: arial; color: rgb(0, 0, 0); font-weight: bold;") << cgicc::input().set("type", "submit").set("name", "submit").set("value", "Update");

    // ----------------------------------------------------------------------
    // Display the values
    dcuDescription *dcu = dcuValuesReceived_[dcuHardId] ;

    if (dcu != NULL) {

      // Conversions factors
      TkDcuConversionFactors *conversionFactors = NULL ;
      if (doConversion_) conversionFactors = getTkDcuConversionFactors(*dcu) ;

      //#define BASEFORMULA
#ifdef BASEFORMULA
      double valDcuTob0 = dcuDescription::calcTempResTob(dcu->getDcuChannel0(), 19.23) ;
      double valDcu0 = dcuDescription::calcTempRes(dcu->getDcuChannel0(), 19.23) ;
      double valDcuTob4 = dcuDescription::calcTempResTob(dcu->getDcuChannel4(), 9.63) ;
      double valDcu4 = dcuDescription::calcTempRes(dcu->getDcuChannel4(), 9.63) ;
      double valDcu7 = dcuDescription::calcTempInt(dcu->getDcuChannel7()) ;
#endif

      if (conversionFactors != NULL) { // Conversion factors found
	
	bool quality ;
	  
	if (conversionFactors->getDcuHardId() != 0) {
	  if (conversionFactors->getDcuType() == DCUCCU)
	    title = conversionFactors->getSubDetector() + " " + title + std::string(" on CCU") ;
	  else
	    title = conversionFactors->getSubDetector() + " " + title + std::string(" on a front-end hybrid") ;
	}
	else {
	  title = title + " (default conversion factors for DCU on Front-End Hybrid)" ;
	}

	*out << cgicc::title(title) << std::endl;  
	xgi::Utils::getPageHeader(*out, title);

	*out << cgicc::h2("ADC count together with the conversion").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

	*out << "<CENTER>" << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << "<tr>" << std::endl ;
	*out << "<td>" << "Channel" << "</td>" << std::endl;
	*out << "<td>" << "Meaning" << "</td>" << std::endl;
	*out << "<td>" << "ADC Count" << "</td>" << std::endl;
	*out << "<td>" << "Conversion" << "</td>" << std::endl;
	*out << "<td>" << "Quality Flag" << "</td>" << std::endl;
#ifdef BASEFORMULA
	*out << "<td>" << "Base Formula Values" << "</td>" << std::endl;
#endif
	*out << "</tr>" << std::endl ;

	// Channel 0
	if ( (conversionFactors->getDcuType() == DCUFEH) || 
	     (conversionFactors->getSubDetector() == "TOB") ) {
	  *out << "<tr>" 
	       << "<td>" << "channel 0" << "</td>"
	       << "<td>" << "Silicon Sensor Temperature" << "</td>"
	       << "<td>" << toString(dcu->getDcuChannel0()) << "</td>" 
	       << "<td>" << toString(conversionFactors->getSiliconSensorTemperature ( quality )) << " C" << "</td>" 
	       << "<td>" << (quality ? "Good" : "Bad") << "</td>" 
#ifdef BASEFORMULA
	       << "<td>" << "TOB: " << toString(valDcuTob0) << ", Other: " << toString(valDcu0) << "</td>" 
#endif
	       << "</tr>" << std::endl ;
	}
	else {
	    *out << "<tr>" 
		 << "<td>" << "channel 0" << "</td>" 
		 << "<td>" << "Not Used" << "</td>" 
		 << "<td>" << toString(dcu->getDcuChannel0()) << "</td>" 
		 << "<td>" << "None" 
		 << "<td>" << "None" << "</td>" 
		 << "</tr>" << std::endl ;
	}

	// Channel 1
	if (conversionFactors->getDcuType() == DCUFEH) {
	  *out << "<tr>" 
	       << "<td>" << "channel 1" << "</td>" 
	       << "<td>" << "V250" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel1())  << "</td>" 
	       << "<td>" << toString(conversionFactors->getV250 ( quality )) << " mV" << "</td>" 
	       << "<td>" << (quality ? "Good" : "Bad")  << "</td>" 
	       << "</tr>" << std::endl ;
	}
	else {
	  *out << "<tr>" 
	       << "<td>" << "channel 1" << "</td>" 
	       << "<td>" << "Not Used" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel1())  << "</td>" 
	       << "<td>" << "None"  << "</td>" 
	       << "<td>" << "None" << "</td>" 
	       << "</tr>" << std::endl ;
	}

	// Channel 2
	if (conversionFactors->getDcuType() == DCUFEH) {
	  *out << "<tr>" 
	       << "<td>" << "channel 2" << "</td>" 
	       << "<td>" << "V125" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel2())  << "</td>" 
	       << "<td>" << toString(conversionFactors->getV125 ( quality )) << " mV" << "</td>" 
	       << "<td>" << (quality ? "Good" : "Bad")  << "</td>" 
	       << "</tr>" << std::endl ;
	} 
	else if ( (conversionFactors->getDcuType() == DCUCCU) && (conversionFactors->getSubDetector() == "TOB") ) {
	  *out << "<tr>" << "<td>" << "channel 2"
	       << "<td>" << "V250"
	       << "<td>" << toString(dcu->getDcuChannel2()) 
	       << "<td>" << toString(conversionFactors->getV250 ( quality )) << " mV"
	       << "<td>" << (quality ? "Good" : "Bad") 
	       << "</tr>" << std::endl ;
	}
	else {
	  *out << "<tr>" 
	       << "<td>" << "channel 2" << "</td>" 
	       << "<td>" << "Not Used" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel2())  << "</td>" 
	       << "<td>" << "None"  << "</td>" 
	       << "<td>" << "None" << "</td>" 
	       << "</tr>" << std::endl ;
	}

	// Channel 3
	if (conversionFactors->getDcuType() == DCUFEH) {
	  *out << "<tr>" 
	       << "<td>" << "channel 3" << "</td>" 
	       << "<td>" << "I Leak" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel3())  << "</td>" 
	       << "<td>" << toString(conversionFactors->getILeak ( quality )) << " uA" << "</td>" 
	       << "<td>" << (quality ? "Good" : "Bad")  << "</td>" 
	       << "</tr>" << std::endl ;
	}
	else if ( (conversionFactors->getDcuType() == DCUCCU) && (conversionFactors->getSubDetector() == "TOB") ) {
	  *out << "<tr>" << "<td>" << "channel 3" << "</td>" 
	       << "<td>" << "V125" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel3())  << "</td>" 
	       << "<td>" << toString(conversionFactors->getV125 ( quality )) << " mV" << "</td>" 
	       << "<td>" << (quality ? "Good" : "Bad")  << "</td>" 
	       << "</tr>" << std::endl ;
	}
	else {
	  *out << "<tr>" 
	       << "<td>" << "channel 3" << "</td>" 
	       << "<td>" << "Not Used" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel3()) << "</td>" 
	       << "<td>" << "None" << "</td>" 
	       << "<td>" << "None" << "</td>" << "</tr>" << std::endl ;
	}

	// Channel 4
	if (conversionFactors->getDcuType() == DCUFEH) {
	  *out << "<tr>" 
	       << "<td>" << "channel 4" << "</td>" 
	       << "<td>" << "Hybrid Temperature" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel4()) << "</td>" 
	       << "<td>" << toString(conversionFactors->getHybridTemperature ( quality )) << " C" << "</td>" 
	       << "<td>" << (quality ? "Good" : "Bad") << "</td>" 
#ifdef BASEFORMULA
	       << "<td>" << "TOB: " << toString(valDcuTob4) << ", Other: " << toString(valDcu4) << "</td>" 
#endif
	       << "</tr>" << std::endl ;
	}
	else {
	  *out << "<tr>" 
	       << "<td>" << "channel 4" << "</td>" 
	       << "<td>" << "Not Used" << "</td>" 
	       << "<td>" << toString(dcu->getDcuChannel4())  << "</td>" 
	       << "<td>" << "None" << "</td>" 
	       << "<td>" << "None" << "</td>" 
	       << "</tr>" << std::endl ;
	}

	// Channel 5
	*out << "<tr>" 
	     << "<td>" << "channel 5" << "</td>" 
	     << "<td>" << "Not Used" << "</td>" 
	     << "<td>" << toString(dcu->getDcuChannel5()) << "</td>" 
	     << "<td>" << "None" << "</td>" 
	     << "<td>" << "None" << "</td>" 
	     << "</tr>" << std::endl ;
       
	// Channel 6
	*out << "<tr>" 
	     << "<td>" << "channel 6" << "</td>" 
	     << "<td>" << "Not Used" << "</td>" 
	     << "<td>" << toString(dcu->getDcuChannel6())  << "</td>" 
	     << "<td>" << "None"  << "</td>" 
	     << "<td>" << "None" << "</td>" 
	     << "</tr>" << std::endl ;

	// Channel 7
	*out << "<tr>" 
	     << "<td>" << "channel 7" << "</td>" 
	     << "<td>" << "DCU Temperature" << "</td>" 
	     << "<td>" << toString(dcu->getDcuChannel7())  << "</td>" 
	     << "<td>" << toString(conversionFactors->getDcuTemperature ( quality )) << " C" << "</td>" 
	     << "<td>" << (quality ? "Good" : "Bad")  << "</td>" 
#ifdef BASEFORMULA
	     << "<td>" << toString(valDcu7) << "</td>" 
#endif
	     << "</tr>" << std::endl ;
	*out << cgicc::table() << "</CENTER>" << std::endl;

	*out << cgicc::h2("Conversion factors").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

	*out << "<CENTER>" << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << "<tr>" << "<td>" << std::endl ;
	*out << "Parameter Name" ;
	*out << "</td>" << std::endl ;
	*out << "<td>"; 
	*out << "Value" ;
	*out << "</td>" << std::endl ;
	*out << "</td>" << std::endl ;
	*out << "</tr>" << std::endl ;
	*out << "<tr>" << "<td>" << "DCU Hard ID" << "</td>" << "<td>" << toString(conversionFactors->getDcuHardId()) << "</td>" << "</tr>" ;
	*out << "<tr>" << "<td>" << "Sub detector" << "</td>" << "<td>" << conversionFactors->getSubDetector() << "</td>" << "</tr>" ;
	*out << "<tr>" << "<td>" << "DCU Type" << "</td>" << "<td>" << conversionFactors->getDcuType() << "</td>" << "</tr>" ;
	*out << "<tr>" << "<td>" << "Structure ID" << "</td>" << "<td>" << conversionFactors->getStructureId() << "</td>" << "</tr>" ;
	*out << "<tr>" << "<td>" << "Container ID" << "</td>" << "<td>" << conversionFactors->getContainerId() << "</td>" << "</tr>" ;
	*out << "<tr>" << "<td>" << "DET ID" << "</td>" << "<td>" << toString(conversionFactors->getDetId()) << "</td>" << "</tr>" ;

	// ADC 0 both for FEH and (CCU for TEC and TOB)
	if ( (conversionFactors->getDcuType() == DCUFEH) ||
	     (conversionFactors->getSubDetector() == "TOB") ||
	     (conversionFactors->getSubDetector() == "TEC") ) {

	  *out << "<tr>" << "<td>" << "ADC Gain 0" << "</td>" << "<td>" << toString(conversionFactors->getAdcGain0()) << "</td>" << "</tr>" ;
	  *out << "<tr>" << "<td>" << "ADC Offset 0" << "</td>" << "<td>" << toString(conversionFactors->getAdcOffset0()) << "</td>" << "</tr>" ;
	  if (conversionFactors->getAdcCal0())
	    *out << "<tr>" << "<td>" << "ADC Cal 0" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	  else
	    *out << "<tr>" << "<td>" << "ADC Cal 0" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;
	}

	// INL only for FEH
	if (conversionFactors->getDcuType() == DCUFEH) {
	  if (conversionFactors->getAdcInl0OW())
	    *out << "<tr>" << "<td>" << "ADC INL OW 0" << "</td>" << "<td>" << toString(conversionFactors->getAdcInl0OW()) << "</td>" << "</tr>" ;
	  if (conversionFactors->getAdcInl0())
	    *out << "<tr>" << "<td>" << "ADC INL 0" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	  else
	    *out << "<tr>" << "<td>" << "ADC INL 0" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;
	}

	// I20 both for FEH and (CCU for TOB and TEC)
	if ( (conversionFactors->getDcuType() == DCUFEH) ||
	     (conversionFactors->getSubDetector() == "TOB") ||
	     (conversionFactors->getSubDetector() == "TEC") ) {
	  *out << "<tr>" << "<td>" << "I20" << "</td>" << "<td>" << toString(conversionFactors->getI20()) << "</td>" << "</tr>" ;
	  if ( (conversionFactors->getDcuType() == DCUCCU) && (conversionFactors->getSubDetector() == "TOB") ) { // in other case display by I10
	    if (conversionFactors->getICal())
	      *out << "<tr>" << "<td>" << "I CAL" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	    else
	      *out << "<tr>" << "<td>" << "I CAL" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;
	  }
	}

	// I10 both for FEH and (CCU on TEC)
	if ( (conversionFactors->getDcuType() == DCUFEH) ||
	     (conversionFactors->getSubDetector() == "TEC") ) {
	  *out << "<tr>" << "<td>" << "I10" << "</td>" << "<td>" << toString(conversionFactors->getI10()) << "</td>" << "</tr>" ;
	  if (conversionFactors->getICal())
	    *out << "<tr>" << "<td>" << "I CAL" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	  else
	    *out << "<tr>" << "<td>" << "I CAL" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;
	}

	// KDIV only for FEH
	if (conversionFactors->getDcuType() == DCUFEH) {
	  *out << "<tr>" << "<td>" << "KDiv" << "</td>" << "<td>" << toString(conversionFactors->getKDiv()) << "</td>" << "</tr>" ;
	  if (conversionFactors->getKDivCal())
	    *out << "<tr>" << "<td>" << "KDIV CAL" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	  else
	    *out << "<tr>" << "<td>" << "KDIV CAL" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;
	}

	// TS both for FEH and CCU
	*out << "<tr>" << "<td>" << "TS Gain" << "</td>" << "<td>" << toString(conversionFactors->getTsGain()) << "</td>" << "</tr>" ;
	*out << "<tr>" << "<td>" << "TS Offset" << "</td>" << "<td>" << toString(conversionFactors->getTsOffset()) << "</td>" << "</tr>" ;
	if (conversionFactors->getTsCal())
	  *out << "<tr>" << "<td>" << "TS Cal" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	else
	  *out << "<tr>" << "<td>" << "TS Cal" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;

	// R68 only for CCU on TOB
	if ( (conversionFactors->getDcuType() == DCUCCU) && (conversionFactors->getSubDetector() == "TOB") ) {
	  *out << "<tr>" << "<td>" << "R68" << "</td>" << "<td>" << toString(conversionFactors->getR68()) << "</td>" << "</tr>" ;
	  if (conversionFactors->getR68Cal())
	    *out << "<tr>" << "<td>" << "R68 CAL" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	  else
	    *out << "<tr>" << "<td>" << "R68 CAL" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;
	}

	// ADC 2 only for CCU on TOB
	if ( (conversionFactors->getDcuType() == DCUCCU) && (conversionFactors->getSubDetector() == "TOB") ) {
	  *out << "<tr>" << "<td>" << "ADC Gain 2" << "</td>" << "<td>" << toString(conversionFactors->getAdcGain2()) << "</td>" << "</tr>" ;
	  *out << "<tr>" << "<td>" << "ADC Offset 2" << "</td>" << "<td>" << toString(conversionFactors->getAdcOffset2()) << "</td>" << "</tr>" ;
	  if (conversionFactors->getAdcCal2())
	    *out << "<tr>" << "<td>" << "ADC Cal 2" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	  else
	    *out << "<tr>" << "<td>" << "ADC Cal 2" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;
	}

	// ADC 3 only for CCU on TOB
	if ( (conversionFactors->getDcuType() == DCUCCU) && (conversionFactors->getSubDetector() == "TOB") ) {
	  *out << "<tr>" << "<td>" << "ADC Gain 3" << "</td>" << "<td>" << toString(conversionFactors->getAdcGain3()) << "</td>" << "</tr>" ;
	  *out << "<tr>" << "<td>" << "ADC Offset 3" << "</td>" << "<td>" << toString(conversionFactors->getAdcOffset3()) << "</td>" << "</tr>" ;
	  if (conversionFactors->getAdcCal3())
	    *out << "<tr>" << "<td>" << "ADC Cal 3" << "</td>" << "<td>" << "True" << "</td>" << "</tr>" ;
	  else
	    *out << "<tr>" << "<td>" << "ADC Cal 3" << "</td>" << "<td>" << "False" << "</td>" << "</tr>" ;
	}
	*out << cgicc::table() << "</CENTER>" << std::endl;
      }
      else { // No Conversion factors 

	*out << cgicc::title(title) << std::endl;  
	xgi::Utils::getPageHeader(*out, title);

	*out << cgicc::h2("No conversion factors found for this DCU").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

	*out << "<CENTER>" << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << "<tr>" << std::endl ;
	*out << "<td>" << "Channel" << "</td>" << std::endl;
	*out << "<td>" << "ADC Count" << "</td>" << std::endl;
#ifdef BASEFORMULA
	*out << "<td>" << "Base Formula Values" << "</td>" << std::endl;
#endif
	*out << "</tr>" << std::endl;
	*out << "<tr>" << "<td>" << "channel 0" << "</td>" << std::endl ;
	*out << "<td>" << toString(dcu->getDcuChannel0()) << "</td>" << std::endl ;
#ifdef BASEFORMULA
	*out << "<td>" << "TOB: " << toString(valDcuTob0) << ", Other: " << toString(valDcu0) << "</td>" << std::endl ;
#endif
	*out << "</tr>" << std::endl;
	*out << "<tr>" << std::endl ;
	*out << "<tr>" << "<td>" << "channel 1" << "</td>" << std::endl ;
	*out << "<td>" << toString(dcu->getDcuChannel1()) << "</td>" << std::endl ;
	*out << "</tr>" << std::endl;
	*out << "<tr>" << std::endl ;
	*out << "<td>" << "channel 2" << "</td>" << std::endl ;
	*out << "<td>" << toString(dcu->getDcuChannel2())  << "</td>" << std::endl ;
	*out << "</tr>" << std::endl;
	*out << "<tr>" << std::endl ;
	*out << "<td>" << "channel 3" << "</td>" << std::endl ;
	*out << "<td>" << toString(dcu->getDcuChannel3())  << "</td>" << std::endl ;
	*out << "</tr>" << std::endl;
	*out << "<tr>" << std::endl ;
	*out << "<td>" << "channel 4" << "</td>" << std::endl ;
	*out << "<td>" << toString(dcu->getDcuChannel4())  << "</td>" << std::endl ;
#ifdef BASEFORMULA
	*out << "<td>" << "TOB: " << toString(valDcuTob4) << ", Other: " << toString(valDcu4) << "</td>" << std::endl ;
#endif
	*out << "</tr>" << std::endl;
	*out << "<tr>" << std::endl ;
	*out << "<td>" << "channel 5" << "</td>" << std::endl ;
	*out << "<td>" << toString(dcu->getDcuChannel5())  << "</td>" << std::endl ;
	*out << "</tr>" << std::endl;
	*out << "<tr>" << std::endl ;
	*out << "<td>" << "channel 6" << "</td>" << std::endl ;
	*out << "</td>" << "<td>" << toString(dcu->getDcuChannel6()) << "</td>" << std::endl ;
	*out << "</tr>" << std::endl;
	*out << "<tr>" << std::endl ;
	*out << "<td>" << "channel 7" << "</td>" << std::endl ;
	*out << "<td>" << toString(dcu->getDcuChannel7()) << "</td>" << std::endl ;
#ifdef BASEFORMULA
	*out << "<td>" << toString(valDcu7) << "</td>" << std::endl ;
#endif
	*out << "</tr>" << std::endl;
	*out << cgicc::table() << "</CENTER>" << std::endl;
      }

#ifdef GRAPHICS
      out->getHTTPResponseHeader().addHeader("Content-Type","image/png");
      out->getHTTPResponseHeader().addHeader("Expires", "0");
      out->getHTTPResponseHeader().addHeader("Cache-Control","no-store, no-cache, must-revalidate, max-age=0");
      out->getHTTPResponseHeader().addHeader("Cache-Control","post-check=0, pre-check=0");
      out->getHTTPResponseHeader().addHeader("Pragma", "no-cache");
      TCanvas* c = new TCanvas("pulse","Pulses",200,10,1100,900);

      c->cd();
      int nplot = 100 ; //g->GetNChips();
      int nx =2;
      int ny = (nplot +nplot%2)/2;
      float xsiz =0.5;
      float ysiz= 1./ny;
      TPad* pad[nx*ny];
      for ( int ipad=0;ipad<nx*ny;ipad++) {
	char pname[256];
	sprintf(pname,"pad%.2d",ipad);
	float xmin = (ipad%2)*0.5;
	float xmax = xmin+0.5;
	float ymin= ((ipad/2)%ny)*ysiz;
	float ymax = ymin+ysiz;
	pad[ipad] = new TPad(pname,pname,xmin,ymin,xmax,ymax,0);
	pad[ipad]->Draw("HIST");
      }

      for (int j=0;j<100;j++) {
	char hname[256];
	sprintf(hname,"|BeamTest|Det%.4d|Timing|Tick%.1d",ichan,j);
	std::string hs1(hname);
	TH1* h1 = monitor_->getHistoByName(hs1);
	pad[j]->cd(); if (h1!=NULL) h1->Draw("HIST");
      }
      c->SaveAs("/tmp/toto.png");
      //c->SaveAs("/tmp/toto.eps");
      // system("convert /tmp/toto.eps /tmp/toto.png");
      //FILE* fd=fopen("/tmp/toto.png","r");
      int fd = open("/tmp/toto.png",O_RDONLY);

      char buffer[65535];
      memset(buffer,0,65535);
      int len=0;
      while ((len=read(fd,buffer,65535))>0) {
	out->write(buffer,len);
	//      *out << buffer << endl;
	cout << len << std::endl;

	memset(buffer,0,65535);
      }
      //   infile.close();
      close(fd);

      for (int ipad=0;ipad<nx*ny;ipad++) delete pad[ipad];
      delete c;

#endif
    }

    // ----------------------------------------------------------------------
    // Update button
    //*out << cgicc::p() << cgicc::h2("Click on Update to refresh the values or to change the parameters").set("style","font-size: 15pt;  font-family: arial; color: rgb(0, 0, 0); font-weight: bold;")
    //<< cgicc::input().set("type", "submit").set("name", "submit").set("value", "Update");

    // End of the form
    // *out << cgicc::form() << std::endl;
  }
  catch(const std::exception& e) {

    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  *out << "</HTML>" << std::endl ; 
}

/** Display the state machine
 */
void DcuFilter::displayStateMachine (xgi::Input * in, xgi::Output * out) {

   //errorReportLogger_->errorReport ("displayDcuList",LOGDEBUG) ;

  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("DcuFilter State Machine") << std::endl;
  xgi::Utils::getPageHeader(*out, "DcuFilter State Machine");
    
  // Links
  displayRelatedLinks (in,out) ;
  linkPosition_ = 6 ;

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/dispatch";
  
  // display FSM
  std::set<std::string> possibleInputs = fsm_.getInputs(fsm_.getCurrentState());
  std::set<std::string> allInputs = fsm_.getInputs();

  //*out << cgicc::h3("FecSupervisor: Finite State Machine") << std::endl;
  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << cgicc::tr() << std::endl;
  *out << "<th>" << fsm_.getStateName(fsm_.getCurrentState()) << "</th>" << std::endl;
  *out << cgicc::tr() << std::endl;
  *out << cgicc::tr() << std::endl;
  std::set<std::string>::iterator i;
  for ( i = allInputs.begin(); i != allInputs.end(); i++) {
    
    if ((*i) != INITIALISE && (*i) != DESTROY) {
      
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

  *out << "</HTML>" << std::endl ; 
}

/** Depend of which state was initiated
 * Possible state are: Initialise, Configure, Halt
 */
void DcuFilter::dispatch(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
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
	toolbox::Event::Reference e(new toolbox::Event(stateInput,this));
	fsm_.fireEvent(e);
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

/** Display the Web page for a test with PVSS
 */
void DcuFilter::displayPVSSTest(xgi::Input * in, xgi::Output * out) {

  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("PVSS Test Data Point") << std::endl;
  xgi::Utils::getPageHeader(*out, "PVSS Test Data Point");

  // Links
  displayRelatedLinks (in,out) ;
  linkPosition_ = 5 ;

  try {

    if (xPditor_ == NULL) {

      // Default value for test
      errorReportLogger_->errorReport (pvssClassName_.toString() + " instance " + pvssInstance_.toString() + ", " + soapActionString_.toString(), LOGDEBUG) ;

      dataPointName_  = pvssPoint_.toString() + DISTRIBUTIONDPNAMEFEH + std::string("1") + STATUSDPNAME ;
      dataPointValue_ = "FIRST TEST" ;
      numberOfDPs_ = 100 ;	

      if (pvssClassName_.toString() == "") {
	xPditor_ = new XPditor ( this, originatorStr_, pvssClassName_.toString(), pvssInstance_, soapActionString_.toString()) ;
      }
    }

    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    
    url += "/applyPVSSTest";	
    
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;
    
    // --------------------------------------------------------------------
    // PVSS Configuration
    // Size of all the next fields
    std::string sizeField = "60" ;
    
    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::legend("PVSS Configuration") << std::endl;
    //*out << cgicc::br() << std::endl ;
    
    // Reach the machine, class name and instance
    *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("PVSS Class Name ") << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::input().set("type","text")
      .set("name","pvssClassName")
      .set("size",sizeField)
      .set("readonly","readonly")
      .set("value",pvssClassName_.toString()) << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("PVSS Instance ") << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::input().set("type","text")
      .set("name","pvssInstance")
      .set("size",sizeField)
      .set("readonly","readonly")
      .set("value",pvssInstance_.toString()) << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl ;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("SOAP Action string ") << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::input().set("type","text")
      .set("name","soapActionString")
      .set("size",sizeField)
      .set("readonly","readonly")
      .set("value",soapActionString_.toString()) << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::p() << cgicc::label("Example: " + pvssPoint_.toString() + ":testFred_DCU" + TSIDPNAME) << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::p() << cgicc::label("Data Point Name ") << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::input().set("type","text")
      .set("name","dataPointName")
      .set("size",sizeField)
      .set("value",dataPointName_) << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::p() << cgicc::label("Data Point Value ") << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::input().set("type","text")
      .set("name","dataPointValue")
      .set("size",sizeField)
      .set("value",dataPointValue_) << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Write Data Point") << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    if (dataPVSSWrite_)
      *out << cgicc::input().set("type", "checkbox")
	.set("name","writeDataPoint").set("checked","true") << std::endl;
    else
      *out << cgicc::input().set("type", "checkbox")
	.set("name","writeDataPoint") << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Read Data Point") << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>"; 
    if (dataPVSSRead_)
      *out<< cgicc::input().set("type", "checkbox")
	.set("name","readDataPoint").set("checked","true") << std::endl;
    else
      *out<< cgicc::input().set("type", "checkbox")
	.set("name","readDataPoint") << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Multiple data points write") << std::endl ;
    *out << "</td>" << std::endl;
    *out << "<td>";
    if (dataPVSSMultipleWrite_) 
      *out<< cgicc::input().set("type", "checkbox")
	.set("name","multipleDataPoints").set("checked","true") << std::endl;
    else
      *out<< cgicc::input().set("type", "checkbox")
	.set("name","multipleDataPoints") << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << "<tr>" << std::endl;
    *out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Number of datapoints to be set") << std::endl ;
    *out << "</td>" << std::endl; *out << "<td>"; 
    *out << cgicc::input().set("type","text")
      .set("name","numberOfDPs")
      .set("size",sizeField)
      .set("value",toString(numberOfDPs_)) << std::endl;
    *out << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
    *out << cgicc::table() << std::endl;

    // End of the field
    *out << cgicc::fieldset() << std::endl;
    // End of Configuration
    // --------------------------------------------------------------------

    *out << cgicc::p() << std::endl ;    

    // --------------------------------------------------------------------
    // Write / Read data
    if (dataPVSSRead_ || dataPVSSWrite_ || dataPVSSMultipleWrite_) {
      *out << cgicc::fieldset() << std::endl;
      *out << cgicc::legend("Write/Read from PVSS Data Point(s)") << std::endl;
      //*out << cgicc::br() << std::endl ;

      *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
      if (dataPVSSWrite_) {
	*out << "<tr>" << std::endl ;
	*out << "<td>"; 
	*out << cgicc::label("Status of write") << std::endl ;
	*out << "</td>" << std::endl;
	*out << "<td>"; 
	*out << cgicc::label(pvssWriteStatus_) << std::endl ;
	*out << "</td>" << std::endl;
	*out << "</tr>" << std::endl ;
      }
      if (dataPVSSMultipleWrite_) {
	*out << "<tr>" << std::endl ;
	*out << "<td>"; 
	*out << cgicc::label("Status of multiple write") << std::endl ;
	*out << "</td>" << std::endl;
	*out << "<td>"; 
	*out << cgicc::label(pvssWriteStatus_) << std::endl ;
	*out << "</td>" << std::endl;
	*out << "</tr>" << std::endl ;
      }
      if (dataPVSSRead_) {
	*out << "<tr>" << std::endl ;
	*out << "<td>"; 
	*out << cgicc::label("Status of read") << std::endl ;
	*out << "</td>" << std::endl;
	*out << "<td>"; 
	*out << cgicc::label(pvssReadStatus_) << std::endl ;
	*out << "</td>" << std::endl;
	*out << "</tr>" << std::endl ;
	if (pvssReadStatus_ == "OK") {
	  *out << "<tr>" << std::endl ;
	  *out << "<td>"; 
	  *out << cgicc::label("Read value") << std::endl ;
	  *out << "</td>" << std::endl;
	  *out << "<td>"; 
	  *out << cgicc::label(dataPointValue_) << std::endl ;
	  *out << "</td>" << std::endl;
	  *out << "</tr>" << std::endl ;
	}
      }
      *out << cgicc::table() << std::endl;
      // End of the field
      *out << cgicc::fieldset() << std::endl;
      // End of Configuration
    }
    // --------------------------------------------------------------------

    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Apply");

    // End of the form
    *out << cgicc::form() << std::endl;
  }
  catch (xdaq::exception::Exception& e) {

    std::stringstream msgError ; msgError << "Unable to access PVSS on class " << pvssClassName_.toString() << " instance " << pvssInstance_.toString()
					  << " (" << soapActionString_.toString() << ") "
					  << std::endl << "(error = " << e.what() << ")" ;
    errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;
    lastErrorMessage_ = msgError.str() ;
    errorOccurs_ = true ;
    *out << cgicc::h2(lastErrorMessage_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::td() << std::endl;
  }

  *out << "</HTML>" << std::endl ; 
}

/** Apply the command sent by displayPVSSTest (send a command to PVSS)
 */
void DcuFilter::applyPVSSTest (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("applyPVSSTest", LOGDEBUG) ;

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    //pvssClassName_ = cgi["pvssClassName"]->getValue() ;
    //pvssInstance_ = cgi["pvssInstance"]->getValue() ;
    //soapActionString_ = cgi["soapActionString"]->getValue() ;

    dataPointName_  = cgi["dataPointName"]->getValue() ;
    dataPointValue_ = cgi["dataPointValue"]->getValue() ;

    dataPVSSRead_  = cgi.queryCheckbox("readDataPoint") ;
    dataPVSSWrite_ = cgi.queryCheckbox("writeDataPoint") ;
    dataPVSSMultipleWrite_ = cgi.queryCheckbox("multipleDataPoints") ;

    std::stringstream urlStr ; urlStr << "The command will be applied on " << pvssClassName_.toString() << " instance " << pvssInstance_.toString() 
				      << ", " << soapActionString_.toString() ;
    errorReportLogger_->errorReport (urlStr.str(), LOGDEBUG) ;

    // Value to be written
    if (dataPVSSWrite_) {
      errorReportLogger_->errorReport ("Set the point point " + dataPointName_ + "  in PVSS with value " + dataPointValue_, LOGDEBUG) ;
      try {
	xPditor_->dpSet ( dataPointName_, dataPointValue_ ) ;
	pvssWriteStatus_ = "Data set Ok" ;
	std::stringstream msgInfo ; msgInfo << "SOAP request successfully sent: datapoint is set." ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGDEBUG) ;
      }
      catch (xcept::Exception &e) {
	pvssWriteStatus_ = std::string("Data set") + e.what() ;
	std::stringstream msgError ; msgError << "Error: Unable to send the DCU data to PVSS: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }

    // Value to be read
    if (dataPVSSRead_) {

      errorReportLogger_->errorReport ("Read the datapoint " + dataPointName_, LOGDEBUG) ;

      try {
	//dataPointValue_ = xPditor_->dpGetSync ( dataPointName_ ) ;
	dataPointValue_ = xPditor_->dpGetSync ( dataPointName_ ) ;
	pvssReadStatus_ = "Read dp Ok" ;
	std::stringstream msgInfo ; msgInfo << "SOAP request successfully sent: datapoint is read." ;
	errorReportLogger_->errorReport (msgInfo.str(), LOGDEBUG) ;
      }
      catch (xcept::Exception &e) {
	pvssReadStatus_ = std::string("Read dp: ") + e.what() ;
	std::stringstream msgError ; msgError << "Error: Unable to read a datapoint in PVSS: " << e.what() ;
	errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
      }
    }

    if (dataPVSSMultipleWrite_) {

      // System1:DCU1.actual.VMon1:_original.._value
      char *dps[]= { TSIDPNAME,          // 0
		     V250DPNAME,         // 1
		     V125DPNAME,         // 2
		     IDPNAME,            // 3
		     THYBDPNAME,         // 4
		     TDCUDPNAME,         // 5
		     STATUSDPNAME,       // 6
		     QUALITYDPNAME,      // 7
		     TIMESTAMPDPNAME,    // 8
		     DCUIDDPNAME,        // 9
		     CONSTRIDDPNAME,     // 10
		     DETIDDPNAME } ;     // 11

      //#define BLOCKDPS 50
      numberOfDPs_ = fromString<unsigned int>(cgi["numberOfDPs"]->getValue()) ;

      for (unsigned int BLOCKDPS = 10 ; BLOCKDPS <= numberOfDPs_ ; BLOCKDPS += 10) {
      if (numberOfDPs_ > 0) {

	unsigned int startDCU = 1, endDCU, numberOfDPs = numberOfDPs_ ;
	long timeTotal = 0 ;
	while (numberOfDPs > 0) {

	  endDCU = numberOfDPs >= BLOCKDPS ? BLOCKDPS : numberOfDPs ;
	  numberOfDPs -= endDCU ;

	  std::vector<std::string> dataPointNames, dataPointValues ;

	  for (unsigned int dcuIndex = startDCU ; dcuIndex < (endDCU+startDCU) ; dcuIndex ++) {
	  
	    std::ostringstream dpName1 ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[0] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(1.2)) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[1] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(2.5)) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[2] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(1.25)) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[3] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(1.0)) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[4] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(25.0)) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[5] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(1.0)) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[6] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back("OK") ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[7] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back("Good") ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[8] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(time(NULL))) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[9] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(dcuIndex)) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[10] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(dcuIndex)) ;
	    dpName1.str("") ; dpName1 << DISTRIBUTIONDPNAMEFEH << toHEXString(dcuIndex) << dps[11] ;
	    dataPointNames.push_back(dpName1.str()) ;
	    dataPointValues.push_back(toString(dcuIndex)) ;
	  }

	  // Display
//  	  if (dataPointNames.size() != dataPointValues.size())
//  	    errorReportLogger_->errorReport ("Incoherent number of parameters " + toString(dataPointNames.size()) + " / " +  toString(dataPointValues.size()), LOGERROR) ;
//  	  else rrorReportLogger_->errorReport ("Upload " + toString(dataPointValues.size()) + " datapoints values for " + toString(numberOfDPs_) + " datapoints", LOGDEBUG) ;
//  	  for (std::vector<std::string>::iterator it = dataPointNames.begin(), itVal = dataPointValues.begin() ; 
//  	       (it != dataPointNames.end()) && (itVal != dataPointValues.end()) ; it ++, itVal++) {
	    
//  	    errorReportLogger_->errorReport ("DP name = " + *it + " with value " + *itVal, LOGDEBUG) ;
//  	  }
// 	  errorReportLogger_->errorReport ("Send " + toString(endDCU) + " DCUs from " + toString(startDCU) + " to " + toString(endDCU+startDCU-1), LOGDEBUG) ;
	
	  //       unsigned int dcuHardIds[] = {12319047,
	  // 				   12186867,
	  // 				   14547215,
	  // 				   11925242,
	  // 				   16118411,
	  // 				   14024331,
	  // 				   8322319,
	  // 				   5894795,
	  // 				   13761295,
	  // 				   12515723,
	  // 				   16380943,
	  // 				   16186611} ;
	  //       for (unsigned int dcuIndex = 0 ; dcuIndex < 1 ; dcuIndex ++) {
	  
	  // 	for (int i = 0 ; i < MAXDCUCHANNELS ; i ++) {
	  
	  // 	  //errorReportLogger_->errorReport (DISTRIBUTIONDPNAMEFEH + toString(dcuHardIds[dcuIndex]) + ".Channel" + toString(i) + ":original..value", LOGDEBUG) ;
	  // 	  dataPointNames.push_back(DISTRIBUTIONDPNAMEFEH + toString(dcuHardIds[dcuIndex]) + ".Channel" + toString(i) + ":original..value") ;
	  // 	  dataPointValues.push_back(toString(dcuHardIds[dcuIndex])) ;
	  // 	}
	  // 	// Add the timestamp
	  // 	dataPointNames.push_back(DISTRIBUTIONDPNAMEFEH + toString(dcuHardIds[dcuIndex]) + TIMESTAMPDPNAME) ;
	  // 	dataPointValues.push_back(toString(time(NULL))) ;
	  // 	// Add the status
	  // 	dataPointNames.push_back(DISTRIBUTIONDPNAMEFEH + toString(dcuHardIds[dcuIndex]) + STATUSDPNAME) ;
	  // 	dataPointValues.push_back("OK") ;
	  //       }


 	  try {
 	    long startTime = time(NULL) ;
 	    xPditor_->dpSet ( dataPointNames, dataPointValues ) ;
 	    long stopTime = time(NULL) ;
	    timeTotal += (stopTime - startTime) ;
 	    //std::stringstream msgInfo ; msgInfo << "SOAP request successfully sent: datapoints are set." ;
 	    //errorReportLogger_->errorReport (msgInfo.str(), LOGDEBUG) ;
 	  }
 	  catch (xcept::Exception &e) {
 	    pvssWriteStatus_ = std::string("Datapoints set") + e.what() ;
 	    std::stringstream msgError ; msgError << "Error: Unable to send the DCU data to PVSS (multiple datapoints): " << e.what() ;
 	    errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
	    numberOfDPs_ = 0 ;
	    timeTotal = -1 ;
 	  }

	  startDCU = startDCU + endDCU ;
	}

	if (timeTotal >= 0) {
	  pvssWriteStatus_ = toString(numberOfDPs_) + " datapoints set Ok in " + toString(timeTotal) + " seconds for " + toString(BLOCKDPS) + " DPs same time";
	  errorReportLogger_->errorReport (pvssWriteStatus_, LOGDEBUG) ;
	}
      }
      }
    }
  }
  catch(const std::exception& e) {
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  this->displayPVSSTest(in,out) ;
}

// ------------------------------------------------------------------------------------------------------------ //
// Failure
// ------------------------------------------------------------------------------------------------------------ //

/** Failure page
 */
void DcuFilter::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  //errorReportLogger_->errorReport ("failedTransition", LOGDEBUG) ;

  toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
  std::stringstream msgError ; msgError << "Failure occurred when performing transition from: "  
					 << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what() ;
  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
				
  lastErrorMessage_ =  xcept::htmlformat_exception_history(fe.getException());		
}

/** This method will receive the command issue by the recovery lvl1 system
 * \param msg - message
 * \return retrun an acknowledge
 */
xoap::MessageReference DcuFilter::dcuFilterRecovery (xoap::MessageReference msg) throw (xoap::exception::Exception) {

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

/** Display the diagnostic page
 */
void DcuFilter::displayConfigureDiagSystem ( xgi::Input * in, xgi::Output * out ) {

  //errorReportLogger_->errorReport ("displayFecHardwareCheck", LOGDEBUG) ;

  std::string url = getApplicationDescriptor()->getURN();
  linkPosition_ = 4 ;

  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("DCU Filter: Configuration of the Diagnostic System") << std::endl;
  xgi::Utils::getPageHeader(*out, "DCU Filter: Configuration of the Diagnostic System");

  // Status bar
  displayRelatedLinks (in,out) ;

  // ----------------------------------------------------------------------
  // Integrated web pages
  getDiagSystemContent(in,out) ;

  // ----------------------------------------------------------------------
  //xgi::Utils::getPageFooter(*out);
  *out << "</HTML>" << std::endl ;
}

/** Diag system display
 */
void DcuFilter::getDiagSystemContent(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  /* Begin HTML Form() related to Diag */
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/applyConfigureDiagSystem";	
  
  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;
  
  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply Parameters");
  *out << cgicc::p() << std::endl ;

#ifdef TKDIAG  
  configureDiagSystem(in, out);
#else
  *out << cgicc::h2("Diagnostic not used in the compilation system, please contact the support to handle it").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
#endif
  
  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply Parameters");
  *out << cgicc::p() << std::endl ;
  
  *out << cgicc::form() << std::endl;
  /* Ends HTML Form() related to Diag */
  
}

void  DcuFilter::sendRunStuffToPVSS () {
  errorReportLogger_->errorReport ("Start of DcuFilter::sendRunStuffToPVSS ()", LOGDEBUG) ;
  // Check the runnumber and send it if it has been changed
  if ((xPditor_ != NULL) && (((unsigned long)runNumber_) != runNumberSent_)) {
  errorReportLogger_->errorReport ("Detected new run number", LOGUSERINFO) ;
    try {
      if (runNumberDpe_ != "") 
#ifdef PVSSSWITCH
	errorReportLogger_->errorReport ("Sending run number to PVSS", LOGUSERINFO) ;
	xPditor_->dpSet ( runNumberDpe_, runNumber_.toString() ) ;
#endif
      runNumberSent_ = (unsigned long)runNumber_ ;
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "Error: Unable to send the run number to PVSS: " << e.what() << " (dp = " << runNumberDpe_ << ")" ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }

  // Send the run type to PVSS
  if ((xPditor_ != NULL) && (tkccTest_ == true) ) {
    try {
	errorReportLogger_->errorReport ("Sending run type to PVSS", LOGUSERINFO) ;
      xPditor_->dpSet ( runTypeMessage_, "DAQ is running in commissioning procedure" ) ;
    }
    catch (xcept::Exception &e) {
      std::stringstream msgError ; msgError << "Error: Unable to send the run number to PVSS: " << e.what() << " (dp = " << runNumberDpe_ << ")" ;
      errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
    }
  }
  errorReportLogger_->errorReport ("End of DcuFilter::sendRunStuffToPVSS ()", LOGDEBUG) ;
}

