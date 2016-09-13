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
#ifndef DCUFILTER_H
#define DCUFILTER_H

#include <string>

  // XDAQ includes
#include "xdata/UnsignedLong.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"

  // for the work loop and timer
#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/task/WaitingWorkLoop.h" 
#include "toolbox/BSem.h"

#include "FecExceptionHandler.h"

#include "FsmWithStateName.h"

// I2O messaging
#include "TrackerCommandSender.h"
#include "TrackerCommandListener.h"

// Error reporting
#include "ErrorReportLogger.h"
#ifdef TKDIAG
#  include <diagbag/DiagBagWizard.h>
#endif

/** Database access
 */
#include "FecFactory.h"

/** Conversion factors factory
 */
#include "DeviceFactory.h"
#include "jsinterface.h"

// Interface to send the messages
#include "XPditor.h"

// Hash map for the DCU already received (raw data) => DCU Hard ID / dcuDescription
typedef Sgi::hash_map<tscType32, dcuDescription *> DcuValuesReceivedType ;
typedef Sgi::hash_map<unsigned long, TkDcuConversionFactors *> DcuConversionsHashMapType ;

/**
 * \class DcuFilter
 * \version 1.1
 * \author Frederic Drouhin
 * \date Febrary 2006
 * The DcuFilter receives from all FecSupervisor in its partition the DCU raw data values through an i2o
 * message, then convert it and send it to PVSS. In order to avoid flood of data in PVSS (interpreted software)
 * the DCU are sent at each interval (defined as a fireitem) or on change. Each interval all the values are sent
 * even if in the interval some DCUs has been sent due to a change.
 * <BR>Datapoint names sent to PVSS are (where tkkdcs01 is a fireitem), DCU on FEH datapoint names:
 * <ul>
 * <li>trkdcs01:DCU_<DCUHARDID>.DCUID:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.Timestamp:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.Status:_original.._value
 *   <ul>
 *   <li>NOCONVERSION: no conversion for that DCU (not even default conversion)
 *   <li>OK: DCU with conversion factors (can be default default conversion)
 *   <li>ERROR: the DCU was not received (hardware failure for example)
 *   </lu>
 * <li>trkdcs01:DCU_<DCUHARDID>.TSi:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.V250:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.V125:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.I:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.THyb:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.DetID:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.TDCU:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.ConstrID:_original.._value
 * <li>trkdcs01:DCU_<DCUHARDID>.Quality:_original.._value
 * </ul>
 * <BR>Datapoint names sent to PVSS are (where tkkdcs01 is a fireitem), DCU on CCU datapoint names:
 * <ul>
 * <li>tkkdcs01:DCUCCU_DCUHARDID>.DCUID:_original.._value
 * <li>tkkdcs01:DCUCCU_DCUHARDID>.Timestamp:_original.._value
 * <li>tkkdcs01:DCUCCU_DCUHARDID>.Status:_original.._value
 * <li>tkkdcs01:DCUCCU_DCUHARDID>.TSi:_original.._value: only for TOB
 * <li>tkkdcs01:DCUCCU_DCUHARDID>.V250:_original.._value: only for TOB
 * <li>tkkdcs01:DCUCCU_DCUHARDID>.V125:_original.._value: only for TOB
 * <li>tkkdcs01:DCUCCU_DCUHARDID>.TDCU:_original.._value
 * </ul>
 * This class will also make the mapping between the DCU and the PSU channel and power groups, following the file:
 * <ul>
 * <li>CONTROL CHANNELS
 * <li>TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard12/channel000,MYTK_TIB_Layer2
 * <li> ...
 * <li>POWER GROUPS
 * <li>TEST:CAEN/904_SY1527/branchController01/easyCrate1/easyBoard00,MYTK_TIB_L2_2_1_2_3
 * <li> ...
 * </ul>
 * So this file is parsed then for each control channel, switch on, ask all FEC in the context to detect and send the DCUs
 * and build a map of the DCU on CCU and the control channels, then do the same for the DCU on FEH.
 */
class DcuFilter: public TrackerCommandSender,public JsInterface {

 private:

  /** Status of the XDAQ application (last action performed)
   */
  xdata::String xdaqApplicationStatus_ ;
  xdata::String xdaqApplicationStatus2_ ;
  /** Position in the link window
   */
  unsigned int linkPosition_ ;

  /** Partition name, only usefull to upload data into the database
   */
  xdata::String pvssPoint_ ;

  /** Partition name, only usefull to upload data into the database
   */
  xdata::String partitionName_ ;

  /** Force the upload in file if the database is set
   */
  xdata::Boolean doUploadInFile_ ;

  /** Directory for the output of the raw/converted DCU data (default is /tmp)
   */
  xdata::String directoryDcuDataName_ ;

  /** Send the data to PVSS
   */
  xdata::Boolean doUploadPVSS_ ;

  /** Time between two sends of DCU in case of the values are equals
   * The DCU are sent only in case of differences except every timePVSSSend_ seconds
   */
  xdata::UnsignedLong timePVSSSend_ ;

  /** Level to be reached in order to send it to PVSS
   */
  xdata::UnsignedLong pourcentDifferenceForPVSS_ ;

  /** Limit of DPs size
   */
  xdata::UnsignedLong blockDpToBeSent_ ;

  /** Machine name for PVSS
   */
  xdata::String pvssClassName_ ;

  /** OLD Port for PVSS => deprecated
   */
  xdata::UnsignedLong pvssInstance_ ;

  /** Machine name for SOAP action string
   */
  xdata::String soapActionString_ ;

  /** Upload in the database if it is set
   */
  xdata::Boolean doUploadInDatabase_ ;

  /** Make the conversion
   */
  xdata::Boolean doConversion_ ;

  /** Which is the input for the conversion, for the database, you must have databaseAccess_ && !conversionFromFile_
   */
  xdata::Boolean conversionFromFile_ ;

  /** If the conversion input is a file, then we need the name
   */
  xdata::String xmlConversionInputFile_ ;

  /** File parsed to avoid several times the readout of the conversion factors
   */
  std::string fileConversionParsed_ ;

  /** Display the DCU list with the latest values
   */
  xdata::Boolean displayDcuList_ ;

  /** Counter for the number of values received
   */
  unsigned long counter_ ;

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
  FecDeviceFactory *fecDeviceFactory_ ;

  /** Factory for the conversion factors
   */
  DeviceFactory *tkDcuConversionFactory_ ;

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

  /** Sent the PVSS error message as info in place of error
   */
  bool ignoreErrorMessage_ ;

  /** Run number for PVSS (fire item)
   */
  xdata::UnsignedLong runNumber_ ;

  /** Run number for PVSS 
   */
  unsigned long runNumberSent_ ;

  /** Error occurs
   */
  bool errorOccurs_ ;

  /** Error message related to errorOccurs
   */
  std::string lastErrorMessage_ ;

  /** Navigation bar for the related links
   */
  std::vector<std::string> relatedLinksNavigation_;

  /** Hash map of conversion factors
   */
  DcuConversionsHashMapType dcuConversionFactorsMap_ ;

  /** List of the DCU received
   */
  DcuValuesReceivedType dcuValuesReceived_ ;

  /** List of the DCU sent to PVSS
   */
  DcuValuesReceivedType dcuValuesSentToPVSS_ ;

  /** List of the DCU already sent to database
   */
  DcuValuesReceivedType dcuValuesSentToDB_ ;

  /** Unique name for PVSS
   */
  std::string originatorStr_ ;

  /** Interface to send message to PVSS
   */
  XPditor *xPditor_ ;

  // For PVSS test

  /** Data point name for PVSS test
   */
  std::string dataPointName_ ;

  /** Data value for PVSS test
   */
  std::string dataPointValue_ ;

  /** PVSS test write requested
   */
  bool dataPVSSWrite_ ;

  /** PVSS Test read requested
   */
  bool dataPVSSRead_ ;

  /** PVSS test multiple write
   */
  bool dataPVSSMultipleWrite_ ;

  /** Number of datapoints to be set in //
   */
  unsigned int numberOfDPs_ ;

  /** Status of the write
   */
  std::string pvssWriteStatus_ ;

  /** Status of the write
   */
  std::string pvssReadStatus_ ;

  /** Automatic detection on the PSU channels connected to the DCUs
   */
  xdata::Boolean dcuPSUChannelDetection_ ;

  /** Switch off the control group once the connection is done, this parameter is also set to true if no power group are present.
   */
  xdata::Boolean switchOffControlGroup_ ;
  /** 
   stuff to control the sending of run number/type to PVSS on configure/enable
  */
  bool sendruntopvss_;
  toolbox::BSem *mutexRunPvss_ ;
  toolbox::BSem *mutexAppStatus_;

  /** Configuration file for the PSU channels with the following lines:
   * CONTROL CHANNELS 
   * CAEN/TOB/branchController15/easyCrate3/easyBoard14/channel000
   * CAEN/TOB/branchController15/easyCrate3/easyBoard15/channel000
   * ...
   * POWER GROUPS 
   * CAEN/TOB/branchController15/easyCrate0/easyBoard06
   * CAEN/TOB/branchController15/easyCrate0/easyBoard07
   * 
   * For this commisioning procedures, the program will swith on the CONTROL CHANNELS one by one and detect for each
   * control channels which are the DCU on CCU (by substracting the CCU already detected)
   * Then it continues on the POWER GROUPS. This operation is only done at the intialisation of the state.
   * The FecSupervisor should be initialised before doing this operation.
   */
  xdata::String psuConfigurationFile_ ;

  /** Upload in database (false) of file (true)
   */
  bool doUploadPSUDCUMapFile_ ;

  /** File for the upload of PSU/DCU
   */
  std::string psuDcuMapFileName_ ;

  /** Upload Work Loop
   */
  toolbox::task::WorkLoop *workDcuFilterLoop_ ;

  /** Binded method for the work loop
   */
  toolbox::task::ActionSignature* uploadWorkLoop_ ;

  /** Is the the work loop sleeping
   */
  bool sleepUploadWorkLoop_ ;

  /** Queue of vector for the FecSupervisor
   */
  std::list<deviceVector *> listQueue_ ;

  /** Semaphore to avoid multiple request on the queue
   */
  toolbox::BSem *mutexVectorQueue_ ;

  /** Number of FecSupervisors
   */
  unsigned int fecSupervisorNumbers_ ;

  /** Check the DCU data to see if a difference appears
   */
  bool checkDCUData_ ;

  /** Output file for this error
   */
  std::string checkDcuDataFile_ ;

  /** Check the DCU data to see if a difference appears
   */
  bool dumpDCUMissing_ ;

  /** Output file for this error
   */
  std::string dumpDCUMissingFile_ ;

  /** stream to store the DCU missing
   */
  std::ofstream *dumpDCUMissingStream_ ;

  /** Number of DCU sent to PVSS
   */
  unsigned int numberOfDcuSentToPVSS_ ;

  /** Software tag version set by CVS
   */
  xdata::String softwareTagVersion_ ;

  /** Is it a tkcc test?
   */
  xdata::Boolean tkccTest_ ;

  /** sub-detector assigned by the database depending on the machine name
   */
  xdata::String subDetector_ ;

  /** datapoint for run number datapoint element
   */
  std::string runNumberDpe_ ;

  /** datapoint for Commissioning procedure for PVSS
   */
  std::string runTypeMessage_ ;

  /** datapoint for DCU/PSU map result summary
   */
  std::string dcuPsuMapResult_ ;

  /** datapoint for DCU/PSU progress value control group
   */
  std::string dcuPsuMapProgressCG_ ;

  /** datapoint for DCU/PSU progress value power group
   */
  std::string dcuPsuMapProgressPG_ ;

  /** status of the control channel, connected, interlocked, ok
   */
  std::map<std::string, std::string> statusIlkControl_ ;

 public:

  // --------------------------------------------------------- XDAQ call

  XDAQ_INSTANTIATOR();

  // --------------------------------------------------------- XDAQ Definition

  /** \brief Constructor for the initialisation of the xdata/export parameters
   * \warning nothing is created in this method (nor FecFactory, Database access), see dcuFilterInitialise
   */
  DcuFilter( xdaq::ApplicationStub* stub ) ;

  /** \brief Destructor that remove all the accesses done to the hardware 
   * and destroy the connection with the database
   */
  ~DcuFilter ( ) ;

  // ------------------------------------------------------- Finite state machine
  /** \brief initiliase all the paremters for the DcuFilter
   */
  void initialiseAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  /** \brief configure state 
   */
  void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  /** \brief enable state 
   */
  void enableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  /** \brief stop state 
   */
  void stopAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  /** \brief halt state 
   */
  void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  /** \brief initiliase all the paremters for the FecSupervisor
   */
  void destroyAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  // ------------------------------------------------------- Database
  /** \brief database creation
   */
  void createDatabaseAccess ( ) ;

  /** \brief Retreive the parameters for the conversion
   */
  void getConversionFactors ( ) ;

  // ------------------------------------------------------- I2O message

  /** \brief receive all i2o messages and dispatch it
   */
  toolbox::mem::Reference* senderTrackerCommandHandler(PI2O_TRACKER_COMMAND_MESSAGE_FRAME com) ;

  // ------------------------------------------------------- Work Loop upload

  /** \brief method that upload the data in PVSS/file
   */
  bool uploadWorkLoop (toolbox::task::WorkLoop* wl) ;

  // ------------------------------------------------------- Conversion factors

  /** \brief Retreive the conversion factors and return the conversionFactors together with the value
   */
  TkDcuConversionFactors *getTkDcuConversionFactors ( dcuDescription dcuD ) ;

  // ------------------------------------------------------- Manage the DCU information received

  /** \brief upload the raw data to a file
   */
  void uploadDcuToFile ( deviceVector vDevice ) ;

  /** \brief upload the converted data to a file
   */
  void uploadDcuConvertedToFile ( deviceVector vDevice ) ;

  /** \brief upload the raw data to the database
   */
  void uploadDcuToDatabase ( deviceVector vDevice ) ;

  /** \brief upload the raw data to PVSS
   */
  void uploadDcuToPVSS ( deviceVector vDevice ) ;

  /** \brief upload the data converted to PVSS
   */
  void uploadDcuToPVSS ( ) ;

  // ------------------------------------------------------- commissioning procedure

  /** \brief detect the DCU information together with the PSU channels
   */
  void detectDcuPSUChannels ( ) ;

  /** \brief get the DCU from the FecSupervisors
   */
  void getDcuFromFecSupervisor ( std::set<xdaq::ApplicationDescriptor*> xdaqFecSupervisor, std::map<unsigned int, TkDcuPsuMap *> &dcuPsuMapResult, TkDcuPsuMap *current ) ;

  /** \brief switch on a channel
   */
  int switchChannel ( TkDcuPsuMap *channel, bool controlGroup, bool switchDpStatus, unsigned int position, unsigned int totalNumber ) ;

  /** \brief detect the DCU information together with the PSU channels (depercted)
   */
  void detectDcuPSUChannelsOldVersion ( ) ;

  /** \brief detection for a given list
   */
  void detectDcuForPSUList ( std::list<std::string> psuDPeList, std::set<xdaq::ApplicationDescriptor*> fecSupervisors, 
			     Sgi::hash_map<unsigned int, std::string> &dcuPSUMapping, 
			     std::map<std::string, deviceVector > &psuDcuMap, 
			     bool switchOff, std::ofstream &fileErrorOn, bool cg ) ;

  /** \brief old version for the detection of PSU/DCU map
   */
  void detectDcuPSUChannelsFromFileOldVersion ( ) ;

  /** \brief read the file provided by DCS and return the list of the datapoints
   */
  void getPSUFileInformation ( std::string file, std::list<std::string> &controlChannelList, std::list<std::string> &powerGroupList,
			       std::map<std::string, std::string> &psuDcuType ) ;

  // ------------------------------------------------------- actionPerformed

  /** \brief When an item is fired
   */
  void actionPerformed(xdata::Event& event) ;

  /** FSM Soap message
   */
  xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception, xcept::Exception) ;

  // ------------------------------------------------------- Web methods

  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) ;

  /** \brief Web page for the initialise
   */
  void displayInitialisePage(xgi::Input * in, xgi::Output * out) ;

  /** \brief Initialisation parameters web page
   */
  void dcuFilterInitialise(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Display the related links
   */
  void displayRelatedLinks(xgi::Input * in, xgi::Output * out, bool withHTMLTag = false) ;

  /** \brief display all DCUs received
   */
  void displayDcuFilter (xgi::Input * in, xgi::Output * out) ;

  /** \brief apply button on the parameters given by the previous method
   */
  void dcuListApply(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display all DCUs received
   */
  void displayDcuList (xgi::Input * in, xgi::Output * out) ;

  /** \brief display the value of a DCU
   */
  void displayDcu (xgi::Input * in, xgi::Output * out) ;

  /** \brief display the state machine
   */
  void displayStateMachine (xgi::Input * in, xgi::Output * out) ;

  /** \brief finite state machine action
   */
  void dispatch (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Display the Web page for a test with PVSS
   */
  void displayPVSSTest (xgi::Input * in, xgi::Output * out) ;

  /** \brief Apply the command sent by displayPVSSTest (send a command to PVSS)
   */
  void applyPVSSTest(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;
  /** \bried Send run number and type to PVSS
   */

  void  sendRunStuffToPVSS (void);

  /** \brief failure
   */
  void failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  // --------------------------------- Diagnostic system call back and recovery method

  /** \brief Method called when a recovery is done by the 1st level
   */
  xoap::MessageReference dcuFilterRecovery (xoap::MessageReference msg) 
    throw (xoap::exception::Exception)  ;

  /** \brief display the configuration of the diag system
   */
  void displayConfigureDiagSystem ( xgi::Input * in, xgi::Output * out ) ;

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

} ;

#endif

