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
#define MEASURETIMING

// C++ includes
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include "stdlib.h"
#include "stdio.h"

// XDAQ includes
#include <pthread.h>
#include "toolbox/fsm/FailedEvent.h"
#include "xgi/Utils.h"
#include "xgi/WSM.h"
#include "xgi/Method.h"
#include "xoap/Method.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/domutils.h"

// STD includes
#include <iostream>
#include <stdexcept>

#include "tscTypes.h"
#include "keyType.h"

// FEC includes
#include "FecExceptionHandler.h"
#include "stringConv.h"
#include "tscTypes.h"

// FED includes
#include "Fed9UUtils.hh"
#include "Fed9UDeviceFactoryLib.hh"

/** MACRO for XDAQ
 */
#include "TkConfigurationDb.h"
XDAQ_INSTANTIATOR_IMPL( TkConfigurationDb );

/** This constructor is used in order to initialise the different exported parameters parameters. 
 * State Initialise
 * Exported parameters:
 * <ul>
 * <li><i>fireItemAvailable(std::string("DbLogin"),&dbLogin_)</i>: Database login
 * <li><i>fireItemAvailable(std::string("DbPassword"),&dbPasswd_)</i>: Database password
 * <li><i>fireItemAvailable(std::string("DbPath"),&dbPath_)</i>: Database path
 */

TkConfigurationDb::TkConfigurationDb ( xdaq::ApplicationStub* stub ): xdaq::WebApplication (stub) {

  // -----------------------------------------------
  displayDebugMessage_ = false ;
  displayRunInformationMessage_.str("") ;
  dbVersion_ = 0 ;

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
				   DIAG_TRACKERTKCONFDB_SUBSYSTEMID);

  // Declare the application as a user application
  DIAG_DECLARE_USER_APP ; 
  errorReportLogger_ = new ErrorReportLogger ("TkConfigurationDb " + getApplicationDescriptor()->getInstance(), displayDebugMessage_, true, LOGDEBUG, diagService_) ;
#else
  errorReportLogger_ = new ErrorReportLogger ("TkConfigurationDb" + getApplicationDescriptor()->getInstance(), displayDebugMessage_, true, LOGDEBUG) ;
#endif

  // Software tag version from CVS
  softwareTagVersion_ = FecExceptionHandler::getCVSTag() ;

  // Error report
  if (displayDebugMessage_) errorReportLogger_->setlogLevel (LOGDEBUG) ;

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
  databaseAccess_ = false ;
  cmsOMDSlbAccess_ = false ; // not OMDS

  std::string dbLogin = "nil", dbPasswd = "nil", dbPath = "nil" ;
  DbFecAccess::getDbConfiguration (dbLogin, dbPasswd, dbPath) ;
  dbLogin_ = dbLogin ; dbPasswd_ = dbPasswd ; dbPath_ = dbPath ;

  if (dbLogin_ != "nil" && dbPasswd != "nil" && dbPath_ != "nil") databaseAccess_ = true ;

  // ---------------------------------- State machine
  // Define the final state machine
  //fsm_.addState ('H', "Halted");
  //fsm_.setFailedStateTransitionAction( this, &TkConfigurationDb::failedTransition );
  //fsm_.setInitialState('I');
  //fsm_.setInitialState('H');
  //fsm_.reset();
  //Export the stateName variable
  //getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);
  // Bind SOAP callbacks for control messages
  // xoap::bind (this, &TkConfigurationDb::fireEvent, HALT, XDAQ_NS_URI);

  // -----------------------------------------------
  // Bind for the diagnostic system
#ifdef TKDIAG
  // Configuration option for the diag => link on DIAG_CONFIGURE_CALLBACK
  xgi::bind(this, &TkConfigurationDb::configureDiagSystem, "configureDiagSystem");
  //                                   => link on DIAG_APLLY_CALLBACK
  xgi::bind(this, &TkConfigurationDb::applyConfigureDiagSystem, "applyConfigureDiagSystem");

  // For XRelay
  xoap::bind(this, &TkConfigurationDb::freeLclDiagSemaphore, "freeLclDiagSemaphore", XDAQ_NS_URI );
  xoap::bind(this, &TkConfigurationDb::freeGlbDiagSemaphore, "freeGlbDiagSemaphore", XDAQ_NS_URI );

  //For using the centralized command pannel in logreader
  //                                   => link on DIAG_REQUEST_ENTRYPOINT
  xoap::bind(this, &TkConfigurationDb::processOnlineDiagRequest, "processOnlineDiagRequest", XDAQ_NS_URI );

  // Timer for the diagnostic system
# ifdef AUTO_CONFIGURE_PROCESSES
  std::stringstream timerName;
  timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
  timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
  toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
  toolbox::TimeInterval interval(AUTO_UP_CONFIGURE_DELAY,0);
  toolbox::TimeVal start;
  start = toolbox::TimeVal::gettimeofday() + interval;
  timer->schedule( this, start,  0, "" );
# endif

#endif

  // -----------------------------------------------
  // Nagivation related links
  std::string url = getApplicationDescriptor()->getURN();
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/MoreParameters\">Database Parameters</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/ConfigureDatabase\">Configure Database</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=\"/%s/ConfigureDiagSystem\">Configure DiagSystem</a>", url.c_str()));
  relatedLinksNavigation_.push_back(toolbox::toString("<a href=https://twiki.cern.ch/twiki/bin/view/CMS/ConfigurationDatabase target=\"_blank\">Documentation</a>", url.c_str()));
  
  // Position in the link, zero means no relation was done
  linkPosition_ = 0 ; linkDbPosition_ = 0 ;

  xgi::bind(this, &TkConfigurationDb::displayMoreParameters, "MoreParameters" );
  xgi::bind(this, &TkConfigurationDb::displayConfigureDatabase, "ConfigureDatabase");
  xgi::bind(this, &TkConfigurationDb::displayConfigureDiagSystem, "ConfigureDiagSystem");
  xgi::bind(this, &TkConfigurationDb::tkConfigurationDbParameterSettings, "tkConfigurationDbParameterSettings");

  // Database management
  databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/StateManagement\">Partition/Version</a>", url.c_str())) ;
  databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/PartitionFecParameters\">FEC Partition Parameters</a>", url.c_str())) ;
  databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/ModuleManagement\">Modules & Parameters</a>", url.c_str())) ;
  databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/displayFEDManagement\">FED Parameters</a>", url.c_str())) ;
  databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/displayFECFEDConnections\">FEC/FED Connections</a>", url.c_str())) ;
  databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/displayDcuPsuMap\">PSU/DCU Map</a>", url.c_str())) ;
  databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/displayCCUConfiguration\">CCU Configuration</a>", url.c_str())) ;
  databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/displayRunInformation\">Run Information</a>", url.c_str())) ;
  // NOT IMPLEMENTED ?????????????????????????????
  //databaseRelatedLinks_.push_back(toolbox::toString("<a href=\"/%s/displayEnableDisableStructure\">Enable/Disable structure</a>", url.c_str())) ;


  xgi::bind(this, &TkConfigurationDb::displayStateManagement, "StateManagement");
  xgi::bind(this, &TkConfigurationDb::applyStateManagement, "ApplyStateManagement");

  xgi::bind(this, &TkConfigurationDb::displayPartitionFecParameters, "PartitionFecParameters") ;
  xgi::bind(this, &TkConfigurationDb::applyPartitionFecParameters, "ApplyPartitionFecParameters");

  xgi::bind(this, &TkConfigurationDb::displayModuleManagement, "ModuleManagement") ;
  xgi::bind(this, &TkConfigurationDb::applyModuleManagement, "ApplyModuleManagement");

  xgi::bind(this, &TkConfigurationDb::displayModuleParameters, "displayModuleParameters") ;
  xgi::bind(this, &TkConfigurationDb::applyModuleParameters, "ApplyModuleParameters") ;

  xgi::bind(this, &TkConfigurationDb::displayFEDManagement, "displayFEDManagement") ;
  xgi::bind(this, &TkConfigurationDb::applyFEDManagement, "ApplyFEDManagement");

  xgi::bind(this, &TkConfigurationDb::displayFEDParameters, "displayFEDParameters") ;
  xgi::bind(this, &TkConfigurationDb::applyFEDParameters, "ApplyFEDParameters") ;

  xgi::bind(this, &TkConfigurationDb::displayFEDModeParameters, "displayFEDModeParameters") ;
  xgi::bind(this, &TkConfigurationDb::applyFEDModeParameters, "ApplyFEDModeParameters") ;

  xgi::bind(this, &TkConfigurationDb::displayFECFEDConnections, "displayFECFEDConnections") ;
  xgi::bind(this, &TkConfigurationDb::applyFECFEDConnections, "ApplyFECFEDConnections") ;

  xgi::bind(this, &TkConfigurationDb::displayDCUPSUMap, "displayDcuPsuMap") ;
  xgi::bind(this, &TkConfigurationDb::applyDCUPSUMap, "ApplyDcuPsuMap") ;

  xgi::bind(this, &TkConfigurationDb::displayCCUConfiguration, "displayCCUConfiguration") ;
  xgi::bind(this, &TkConfigurationDb::applyCCUConfiguration, "ApplyCCUConfiguration") ;

  xgi::bind(this, &TkConfigurationDb::displayRunInformation, "displayRunInformation") ;
  xgi::bind(this, &TkConfigurationDb::applyRunInformation, "ApplyRunInformation") ;

  xgi::bind(this, &TkConfigurationDb::displayEnableDisableStructure, "displayEnableDisableStructure") ;
  xgi::bind(this, &TkConfigurationDb::applyEnableDisableStructure, "applyEnableDisableStructure") ;

  // ----------------------------------------------- Database Management
  partitionDbName_ = "" ;
  currentStateName_ = "" ;
  apvDbChanged_ = false ;
  pllDbChanged_ = false ;
  aohDbChanged_ = false ;

  // to dump the database
  xmlFECFileName_ = "" ;

  // ----------------------------------------------- See only current state
  viewAllPartitionNames_ = false ;
}

/* Destructor
 */
TkConfigurationDb::~TkConfigurationDb ( ) {

  // Disconnect the database if it is set
  delete deviceFactory_ ;
  deviceFactory_ = NULL ;
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                 Finite state machine                                                         */
/*                                                                                                              */
/* ************************************************************************************************************ */

#ifdef AUTO_CONFIGURE_PROCESSES
/** Timer method called
*/
void TkConfigurationDb::timeExpired (toolbox::task::TimerEvent& e) 
{
  DIAG_EXEC_FSM_INIT_TRANS ;
  errorReportLogger_->errorReport ("TkConfigurationDb diagnostic system has been auto-initialized.", LOGDEBUG) ;
}
#endif


/* ************************************************************************************************************ */
/*                                                                                                              */
/*                              Database                                                                        */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Create an access to the database. First check if the env. variables are set (DBCONFLOGIN,
 * DBCONFPASSWD, DBCONFPATH. If not, check the export params
 */
void TkConfigurationDb::createDatabaseAccess ( ) {

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
      if ((dbPath_.toString().find("cms_omds_lb") != std::string::npos) || (dbPath_.toString().find("cmsonr") != std::string::npos)) cmsOMDSlbAccess_ = true ;
      else cmsOMDSlbAccess_ = false ;
    }
    catch (oracle::occi::SQLException &e) {
      
      std::stringstream msgError ; msgError << "Unable to connect the database: " << dbLogin_.toString() << "/" <<  dbPasswd_.toString() << "@" << dbPath_.toString() ;
      errorReportLogger_->errorReport (msgError.str(), e, LOGERROR) ;

      databaseAccess_ = false ;
      dbLogin_ = "nil" ;
      dbPasswd_ = "nil" ;
      dbPath_ = "nil" ;
    }  
  }
}

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Action performed                                                           */
/*                                                                                                              */
/* ************************************************************************************************************ */
void TkConfigurationDb::actionPerformed(xdata::Event& event) {

  errorReportLogger_->errorReport ("An action performed as been done but nothing is implemented !", LOGERROR) ;
}


// /** SOAP Callback for FSM: no FSM available for this application
//  */
// xoap::MessageReference TkConfigurationDb::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
// {

//   errorReportLogger_->errorReport ("fireEvent", LOGDEBUG) ;

//   xoap::SOAPPart part = msg->getSOAPPart();
//   xoap::SOAPEnvelope env = part.getEnvelope();
//   xoap::SOAPBody body = env.getBody();
//   DOMNode* node = body.getDOMNode();
//   DOMNodeList* bodyList = node->getChildNodes();
//   for (unsigned int i = 0; i < bodyList->getLength(); i++)  {

//     DOMNode* command = bodyList->item(i);

//     if (command->getNodeType() == DOMNode::ELEMENT_NODE) {

//       std::string commandName = xoap::XMLCh2String (command->getLocalName());
      
//       try {
// 	toolbox::Event::Reference e(new toolbox::Event(commandName, this));
// 	fsm_.fireEvent(e);
// 	// Synchronize Web state machine
// 	//	      wsm_.setInitialState(fsm_.getCurrentState());
//       }
//       catch (toolbox::fsm::exception::Exception & e) {
// 	XCEPT_RETHROW(xcept::Exception, "invalid command", e);
// 	errorReportLogger_->errorReport (e.what(), LOGERROR) ;
//       }

//       xoap::MessageReference reply = xoap::createMessage();
//       xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
//       xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
//       envelope.getBody().addBodyElement ( responseName );
//       return reply;
//     }
//   }

//   XCEPT_RAISE(xcept::Exception,"command not found");	
//   errorReportLogger_->errorReport ("Command not found", LOGERROR) ;
// }

/* ************************************************************************************************************ */
/*                                                                                                              */
/*                                   Web interfaces                                                             */
/*                                                                                                              */
/* ************************************************************************************************************ */

/** Download the devices from the DB
 */
void TkConfigurationDb::downloadAllDevicesFromDb ( ) {

  // Delete the devices it is set
  dcuDevices_.clear() ;

  deviceFactory_->getFecDeviceDescriptions (partitionDbName_, deviceValues_,0,0,0,0,true,false) ;
  // sort the devices to get the DCUs
  for (deviceVector::iterator it = deviceValues_.begin() ; it != deviceValues_.end() ; it ++) 
    if ((*it)->getDeviceType() == DCU) dcuDevices_.push_back(*it) ;
}

/** Check if a module is disable or not
 */
bool TkConfigurationDb::isModuleDisabled ( deviceDescription *deviceD ) {

  bool status = deviceD->getEnabled() ;

  for (deviceVector::iterator it = deviceValues_.begin() ; it != deviceValues_.end() ; it ++) {
    
    if (getFecRingCcuChannelKey((*it)->getKey()) == getFecRingCcuChannelKey(deviceD->getKey())) {
      if ((*it)->getEnabled() != status) { 
	char msg[100];
	decodeKey(msg,(*it)->getKey()) ;
	std::cerr << "PF_FD> The device at position " << msg << " is not coherent with the DCU " << status << "/" << (*it)->getEnabled() << std::endl ;
      }
    }
  }

  bool disable = !(deviceD->getEnabled());

  return (disable) ; 
}


/** Check if a module is disable or not
 */
void TkConfigurationDb::setModuleEnabled ( deviceDescription *deviceD, bool disable ) {

  char msg[80] ; decodeKey(msg,deviceD->getKey()) ;
  errorReportLogger_->errorReport ("The module on is " + std::string((disable ? "disable" : "enable")), LOGDEBUG) ;

  for (deviceVector::iterator it = deviceValues_.begin() ; it != deviceValues_.end() ; it ++) {
    if (getFecRingCcuChannelKey((*it)->getKey()) == getFecRingCcuChannelKey(deviceD->getKey())) {
      (*it)->setEnabled(disable) ;
    }
  }
}

// ------------------------------------------------------------------------------------------------------------ //
// Default page
// ------------------------------------------------------------------------------------------------------------ //

/** Default page for the web display
 */
void TkConfigurationDb::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {


  switch (linkPosition_) {
  case 0:
    displayRelatedLinks (in,out,true) ;
    break ;
  case 1:
    displayMoreParameters(in,out) ;
    break ;
  case 4:
    displayConfigureDiagSystem(in,out) ;
    break ;
//   case 5:
//     displayConfigureDatabase(in,out) ;
//     break ;
  default:
    displayRelatedLinks (in,out,true) ;
    break ;
  }
}

// ------------------------------------------------------------------------------------------------------------ //
// Database
// ------------------------------------------------------------------------------------------------------------ //

/** Display all the possible states
 */
void TkConfigurationDb::displayConfigureDatabase(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkPosition_ = 5 ;

  if ((deviceFactory_ != NULL) && (deviceFactory_->getDbConnected())) {

    switch (linkDbPosition_) {
    case 1: 
      displayStateManagement(in,out) ;
      break ;
    case 2:
      displayPartitionFecParameters(in,out) ;
      break ;
    case 3:
      displayModuleManagement(in,out) ;
      break ;
    case 4:
      displayFEDManagement(in,out) ;
      break ;
    case 7: 
      displayCCUConfiguration(in,out) ;
      break ;
    case 5: 
      displayRunInformation(in,out) ;
      break ;
    case 6: 
      displayEnableDisableStructure(in,out) ;
      break ;
    default:
      displayDatabaseRelatedLinks(in,out,true) ;
    }
  }
  else {

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Strip Tracker Configuration DB Interface") << std::endl;
    xgi::Utils::getPageHeader(*out, "Configure Database");
    displayDatabaseRelatedLinks(in,out) ;
    *out << cgicc::h2("Please specify a database to use these links").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;      
    *out << cgicc::html() << std::endl;
  }
}

/** Display the database related links
 */
void TkConfigurationDb::displayDatabaseRelatedLinks(xgi::Input * in, xgi::Output * out, bool withHTMLTag) throw (xgi::exception::Exception) {

  linkDbPosition_ = 0 ;

  if (withHTMLTag) {
    // Display the page
    // ----------------------------------------------------------------------
    // Here start the WEB pages
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Strip Tracker Configuration Datatbase Interface") << std::endl;
    if (partitionDbName_ != "")
      xgi::Utils::getPageHeader(*out, "Database Related Links for partition " + partitionDbName_);
    else
      xgi::Utils::getPageHeader(*out, "Database Related Links") ;
    
  }

  // Status bar
  displayRelatedLinks (in,out) ;
  
  // Possibility
  *out << "<p style=\"font-family:arial;font-size:10pt;color:#0000AA\">" ;
  for (unsigned long i = 0; i < databaseRelatedLinks_.size(); i++) {
    *out << "[" << databaseRelatedLinks_[i] << "] " ;
  }
  *out << "</p>" << std::endl ;

  if (withHTMLTag) // End of HTML
    *out << cgicc::html() << std::endl ; 
}

/** Display all the states (partition / version) and allow the user to change it
 */
void TkConfigurationDb::displayStateManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 1 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Partition/Version Management");

  displayDatabaseRelatedLinks(in,out) ;

  if ((deviceFactory_ != NULL) && (deviceFactory_->getDbConnected())) {

    // Just for the display
    bool tableStarted = false ;

    // Form and action
    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    url += "/ApplyStateManagement";
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;

    try {
      // Retreive the state ID for the current state
      currentStateHistoryId_ = deviceFactory_->getCurrentStateHistoryId() ;

#ifdef MEASURETIMING
      unsigned long startMillis, endMillis ;
      startMillis = XMLPlatformUtils::getCurrentMillis();
#endif

      // Retreive all the states
      dbStates_ = deviceFactory_->getCurrentStates () ;

#ifdef MEASURETIMING
      endMillis = XMLPlatformUtils::getCurrentMillis();
      std::cout << "Retreiving states tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

      //if (dbStates_.size()) {
      if (true) {

	// display all partitions since no current state is given
	if (dbStates_.size() == 0) viewAllPartitionNames_ = true ;

	// Retreive the states
	TkState *firstState = NULL ;
	if (dbStates_.size()) firstState = (TkState *)(*dbStates_.begin()) ;
	
	// Take the list of the partition to display it if it is not in a state
	std::list<std::string> partitionNames ;
	if (viewAllPartitionNames_) 
	  partitionNames = deviceFactory_->getAllPartitionNames() ;

	for (std::list<std::string>::iterator it = partitionNames.begin() ; it != partitionNames.end() ; it ++) {
	  bool existing = false ;

	  for (tkStateVector::iterator itState = dbStates_.begin() ; itState != dbStates_.end() ; itState ++) {
	    if ((*itState)->getPartitionName() == (*it)) existing = true ;
	  }

	  // The state does not include this method then add it
	  if (!existing) {
	    std::string stateName = "" ;
	    if (firstState != NULL) stateName = firstState->getStateName() ;
	    TkState *newState = new TkState (stateName,(*it),0,0,0,0,0,0,0,0,0,0,0,0,0) ; // BEL S. TkState *newState = new TkState (stateName,(*it),0,0,0,0,0,0,0,0,0,0)
	    dbStates_.push_back(newState) ;
	  }
	}

	// Display it
	std::list<std::string> stateNameList = deviceFactory_->getAllStateHistoryNames();
	currentStateName_ = "" ;
	if (firstState != NULL) currentStateName_ = firstState->getStateName() ;

	//std::cout << "---------------------------------> " << currentStateName_ << std::endl ;

	std::stringstream stateSelectName ; stateSelectName << "<select name=\"StateName\">" ;
	for (std::list<std::string>::iterator itStateName = stateNameList.begin() ; itStateName != stateNameList.end() ; itStateName ++) {
	  if (currentStateName_ == *itStateName)
	    stateSelectName << "<option selected>" << *itStateName << "</option>";
	  else
	    stateSelectName << "<option>" << *itStateName << "</option>";
	}
	if ( (stateNameList.size() == 0) || (currentStateName_ == "NONE") ) stateSelectName << "<option selected>" << "NONE" << "</option>";
	stateSelectName << "</select>" << std::endl ;

	if (currentStateName_ == "") 
	  *out << cgicc::h2("Current State Name: NONE") ;
	else
	  *out << cgicc::h2("Current State Name: " + currentStateName_) ;
	*out << cgicc::h2("You can choose a different state name: " + stateSelectName.str()) ;

// 	std::stringstream runNumberList ; runNumberList << "<select name=\"runNumber\">" ;
// 	tkRunVector allRuns  = deviceFactory_->getAllRuns() ;
// 	for (tkRunVector::iterator it = allRuns.begin() ; it != allRuns.end() ; it ++) {
// 	  if (it == allRuns.begin()) {
// 	    runNumberList << "<option selected>" << (*it)->getRunNumber() << "</option>";
// 	  }
// 	  else {
// 	    runNumberList << "<option>" << (*it)->getRunNumber() << "</option>";
// 	  }
// 	}
// 	runNumberList << "</select>" << std::endl ;
// 	*out << cgicc::h2("Or a run number: " + runNumberList.str()) ;

	// -------------------------- Display
	tableStarted = true ;
	*out << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << cgicc::tr() << std::endl ;
	*out << cgicc::th() << "Partition Name" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "FEC Version" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "FED Version" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "Connection Version" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "DETID/DCU Version" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "DCU/PSU Map Version" << cgicc::th() << std::endl ;
	//	*out << cgicc::th() << "Analysis Version" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "Mask Version" << cgicc::th() << std::endl ;
	*out << cgicc::tr() << std::endl ;

	for (tkStateVector::iterator itState = dbStates_.begin() ; itState != dbStates_.end() ; itState ++) {

	  TkState *tkState = (*itState) ;

	  // Web Display in a table
	  *out << cgicc::tr() << std::endl; 

	  // Partition name
	  *out << cgicc::td() << tkState->getPartitionName() << cgicc::td() << std::endl ;

	  if (tkState->getPartitionName() == "") {
	    errorReportLogger_->errorReport ("Found a partition name empty, this partition cannot be used", LOGERROR) ;
	  }
	  else {

	    try {

#ifdef MEASURETIMING
	      startMillis = XMLPlatformUtils::getCurrentMillis();
#endif

	      // FEC versions
	      tkVersionVector fecVersions = deviceFactory_->getAllFecVersions(tkState->getPartitionName()) ;

#ifdef MEASURETIMING
	      endMillis = XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Retreiving FEC version for partition " << tkState->getPartitionName() << " tooks " << (endMillis-startMillis) << " ms" << std::endl; 
#endif
	      if (fecVersions.size() == 0) {
		TkVersion *tkVersion = new TkVersion ("","",tkState->getFecVersionMajorId(),tkState->getFecVersionMinorId(),0,"Created") ;
		fecVersions.push_back(tkVersion) ;
	      }

	      // Adding the disable state by putting version 0, 0
	      TkVersion *tkFecVersion = new TkVersion ("Disable","Disable",0,0,0,"Disable") ;
	      fecVersions.push_back(tkFecVersion) ;
	    
	      *out << cgicc::td() << std::endl ;
	      std::stringstream fecSelect ; fecSelect << "<select name=\"FecVersion" << tkState->getPartitionName() << "\">" ;
	      *out << fecSelect.str() ;
	      bool disableDone = false ;
	      for (tkVersionVector::iterator itVersion = fecVersions.begin() ; itVersion != fecVersions.end() ; itVersion ++) {
		TkVersion *tkVersion = (*itVersion) ;
		
		if ( (tkVersion->getVersionMajorId() == 0) && (tkVersion->getVersionMinorId() == 0) ) {
		  
		  if (!disableDone) {
		    if ( (tkState->getFecVersionMajorId() == 0) && (tkState->getFecVersionMinorId() == 0) )
		      *out << "<option selected>" << "Disable" << "</option>";
		    else
		      *out << "<option>" << "Disable" << "</option>";
		    
		    disableDone = true ;
		  }
		}
		else if ( (tkVersion->getVersionMajorId() == tkState->getFecVersionMajorId()) && 
			  (tkVersion->getVersionMinorId() == tkState->getFecVersionMinorId()) )
		  *out << "<option selected>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
		else
		  *out << "<option>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
	      }
	      *out << "</select>" << std::endl ;
	      *out << cgicc::td() << std::endl ;

#ifdef MEASURETIMING
	      startMillis = XMLPlatformUtils::getCurrentMillis();	    
#endif

	      // FED versions
	      tkVersionVector fedVersions = deviceFactory_->getAllFedVersions(tkState->getPartitionName()) ;

#ifdef MEASURETIMING
	      endMillis = XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Retreiving FED version for partition " << tkState->getPartitionName() << " tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

	      if (fedVersions.size() == 0) {
		TkVersion *tkVersion = new TkVersion ("","",tkState->getFedVersionMajorId(),tkState->getFedVersionMinorId(),0,"Created") ;
		fedVersions.push_back(tkVersion) ;
	      }

	      // Adding the disable state by putting version 0, 0
	      TkVersion *tkFedVersion = new TkVersion ("Disable","Disable",0,0,0,"Disable") ;
	      fedVersions.push_back(tkFedVersion) ;
	      
	      *out << cgicc::td() << std::endl ;
	      std::stringstream fedSelect ; fedSelect << "<select name=\"FedVersion" << tkState->getPartitionName() << "\">" ;
	      *out << fedSelect.str() ;
	      disableDone = false ;
	      for (tkVersionVector::iterator itVersion = fedVersions.begin() ; itVersion != fedVersions.end() ; itVersion ++) {
		TkVersion *tkVersion = (*itVersion) ;
		if ( (tkVersion->getVersionMajorId() == 0) && (tkVersion->getVersionMinorId() == 0) ) {
		  
		  if (!disableDone) {
		    if ( (tkState->getFedVersionMajorId() == 0) && (tkState->getFedVersionMinorId() == 0) )
		      *out << "<option selected>" << "Disable" << "</option>";
		    else
		      *out << "<option>" << "Disable" << "</option>";
		    disableDone = true ;
		  }
		}
		else if ( (tkVersion->getVersionMajorId() == tkState->getFedVersionMajorId()) && 
			  (tkVersion->getVersionMinorId() == tkState->getFedVersionMinorId()) )
		  *out << "<option selected>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
		else
		  *out << "<option>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
	      }
	      *out << "</select>" << std::endl ;
	      *out << cgicc::td() << std::endl ;

#ifdef MEASURETIMING
	      startMillis = XMLPlatformUtils::getCurrentMillis();	    
#endif

	      // Connection versions
	      tkVersionVector connectionVersions = deviceFactory_->getAllConnectionVersions(tkState->getPartitionName()) ;

#ifdef MEASURETIMING
	      endMillis = XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Retreiving Connections version for partition " << tkState->getPartitionName() << " tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

	      errorReportLogger_->errorReport ("Found " + toString(connectionVersions.size()) + " for partition " + tkState->getPartitionName() +
					       " version " + toString(tkState->getConnectionVersionMajorId()) + "." + toString(tkState->getConnectionVersionMinorId()),
					       LOGDEBUG) ;
	      
	      if (connectionVersions.size() == 0) {
		TkVersion *tkVersion = new TkVersion ("","",tkState->getConnectionVersionMajorId(),tkState->getConnectionVersionMinorId(),0,"Created") ;
		connectionVersions.push_back(tkVersion) ;
	      }
	    
	      // Adding the disable state by putting version 0, 0
	      TkVersion *tkConnectionVersion = new TkVersion ("Disable","Disable",0,0,0,"Disable") ;
	      connectionVersions.push_back(tkConnectionVersion) ;
	    
	      *out << cgicc::td() << std::endl ;
	      std::stringstream connectionSelect ; connectionSelect << "<select name=\"ConnectionVersion" << tkState->getPartitionName() << "\">" ;
	      *out << connectionSelect.str() ;
	      disableDone = false ;
	      for (tkVersionVector::iterator itVersion = connectionVersions.begin() ; itVersion != connectionVersions.end() ; itVersion ++) {
		TkVersion *tkVersion = (*itVersion) ;
		if ( (tkVersion->getVersionMajorId() == 0) && (tkVersion->getVersionMinorId() == 0) ) {
		  
		  if (!disableDone) {
		    if ( (tkState->getConnectionVersionMajorId() == 0) && (tkState->getConnectionVersionMinorId() == 0) )
		      *out << "<option selected>" << "Disable" << "</option>";
		    else
		      *out << "<option>" << "Disable" << "</option>";
		    
		    disableDone = true ;
		  }
		}
		else if ( (tkVersion->getVersionMajorId() == tkState->getConnectionVersionMajorId()) && 
			  (tkVersion->getVersionMinorId() == tkState->getConnectionVersionMinorId()) )
		  *out << "<option selected>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
		else
		  *out << "<option>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
	      }
	      *out << "</select>" << std::endl ;
	      *out << cgicc::td() << std::endl ;

#ifdef MEASURETIMING
	      startMillis = XMLPlatformUtils::getCurrentMillis();	    
#endif

	      // DcuInfo versions => retreive all DET ID versions
	      tkVersionVector dcuInfoVersions = deviceFactory_->getAllDcuInfoVersions(tkState->getPartitionName()) ;

#ifdef MEASURETIMING
	      endMillis = XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Retreiving DCU info version for partition " << tkState->getPartitionName() << " tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

	      errorReportLogger_->errorReport ("Found " + toString(dcuInfoVersions.size()) + " for partition " + tkState->getPartitionName() +
					       " version " + toString(tkState->getDcuInfoVersionMajorId()) + "." + toString(tkState->getDcuInfoVersionMinorId()),
					       LOGDEBUG) ;
	      
	      if (dcuInfoVersions.size() == 0) {
		TkVersion *tkVersion = new TkVersion ("","",tkState->getDcuInfoVersionMajorId(),tkState->getDcuInfoVersionMinorId(),0,"Created") ;
		dcuInfoVersions.push_back(tkVersion) ;
	      }
	    
	      // Adding the disable state by putting version 0, 0
	      TkVersion *tkDcuInfoVersion = new TkVersion ("Disable","Disable",0,0,0,"Disable") ;
	      dcuInfoVersions.push_back(tkDcuInfoVersion) ;
	    
	      *out << cgicc::td() << std::endl ;
	      std::stringstream dcuInfoSelect ; dcuInfoSelect << "<select name=\"DcuInfoVersion" << tkState->getPartitionName() << "\">" ;
	      *out << dcuInfoSelect.str() ;
	      disableDone = false ;
	      for (tkVersionVector::iterator itVersion = dcuInfoVersions.begin() ; itVersion != dcuInfoVersions.end() ; itVersion ++) {
		TkVersion *tkVersion = (*itVersion) ;
		if ( (tkVersion->getVersionMajorId() == 0) && (tkVersion->getVersionMinorId() == 0) ) {
		  
		  if (!disableDone) {
		    if ( (tkState->getDcuInfoVersionMajorId() == 0) && (tkState->getDcuInfoVersionMinorId() == 0) )
		      *out << "<option selected>" << "Disable" << "</option>";
		    else
		      *out << "<option>" << "Disable" << "</option>";
		    
		    disableDone = true ;
		  }
		}
		else if ( (tkVersion->getVersionMajorId() == tkState->getDcuInfoVersionMajorId()) && 
			  (tkVersion->getVersionMinorId() == tkState->getDcuInfoVersionMinorId()) )
		  *out << "<option selected>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
		else
		  *out << "<option>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
	      }
	      *out << "</select>" << std::endl ;
	      *out << cgicc::td() << std::endl ;

#ifdef MEASURETIMING
	      startMillis = XMLPlatformUtils::getCurrentMillis();	    
#endif
	      // DcuPsuMap versions
	      tkVersionVector dcuPsuMapVersions = deviceFactory_->getAllDcuPsuMapVersions(tkState->getPartitionName()) ;

#ifdef MEASURETIMING
	      endMillis = XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Retreiving DCU PSU Map version for partition " << tkState->getPartitionName() << " tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

	      errorReportLogger_->errorReport ("Found " + toString(dcuPsuMapVersions.size()) + " for partition " + tkState->getPartitionName() +
					       " version " + toString(tkState->getDcuPsuMapVersionMajorId()) + "." + toString(tkState->getDcuPsuMapVersionMinorId()),
					       LOGDEBUG) ;
	      
	      if (dcuPsuMapVersions.size() == 0) {
		TkVersion *tkVersion = new TkVersion ("","",tkState->getDcuPsuMapVersionMajorId(),tkState->getDcuPsuMapVersionMinorId(),0,"Created") ;
		dcuPsuMapVersions.push_back(tkVersion) ;
	      }
	    
	      // Adding the disable state by putting version 0, 0
	      TkVersion *tkDcuPsuMapVersion = new TkVersion ("Disable","Disable",0,0,0,"Disable") ;
	      dcuPsuMapVersions.push_back(tkDcuPsuMapVersion) ;
	    
	      *out << cgicc::td() << std::endl ;
	      std::stringstream dcuPsuMapSelect ; dcuPsuMapSelect << "<select name=\"DcuPsuMapVersion" << tkState->getPartitionName() << "\">" ;
	      *out << dcuPsuMapSelect.str() ;
	      disableDone = false ;
	      for (tkVersionVector::iterator itVersion = dcuPsuMapVersions.begin() ; itVersion != dcuPsuMapVersions.end() ; itVersion ++) {
		TkVersion *tkVersion = (*itVersion) ;
		if ( (tkVersion->getVersionMajorId() == 0) && (tkVersion->getVersionMinorId() == 0) ) {
		  
		  if (!disableDone) {
		    if ( (tkState->getDcuPsuMapVersionMajorId() == 0) && (tkState->getDcuPsuMapVersionMinorId() == 0) )
		      *out << "<option selected>" << "Disable" << "</option>";
		    else
		      *out << "<option>" << "Disable" << "</option>";
		    
		    disableDone = true ;
		  }
		}
		else if ( (tkVersion->getVersionMajorId() == tkState->getDcuPsuMapVersionMajorId()) && 
			  (tkVersion->getVersionMinorId() == tkState->getDcuPsuMapVersionMinorId()) )
		  *out << "<option selected>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
		else
		  *out << "<option>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
	      }
	      *out << "</select>" << std::endl ;
	      *out << cgicc::td() << std::endl ;

	      // Begin of mask version
#ifdef MEASURETIMING
	      startMillis = XMLPlatformUtils::getCurrentMillis();	    
#endif

	      // Mask versions
	      tkVersionVector maskVersions = deviceFactory_->getAllMaskVersions(tkState->getPartitionName()) ;

#ifdef MEASURETIMING
	      endMillis = XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Retreiving mask versions for partition " << tkState->getPartitionName() << " tooks " << (endMillis-startMillis) << " ms" << std::endl ;
#endif

	      if (maskVersions.size() == 0) {
		TkVersion *tkVersion = new TkVersion ("","",tkState->getMaskVersionMajorId(),tkState->getMaskVersionMinorId(),0,"Created") ;
		maskVersions.push_back(tkVersion) ;
	      }

	      // Adding the disable state by putting version 0, 0
	      TkVersion *tkMaskVersion = new TkVersion ("Disable","Disable",0,0,0,"Disable") ;
	      maskVersions.push_back(tkMaskVersion) ;
	      
	      *out << cgicc::td() << std::endl ;
	      std::stringstream maskSelect ; maskSelect << "<select name=\"MaskVersion" << tkState->getPartitionName() << "\">" ;
	      *out << maskSelect.str() ;
	      disableDone = false ;
	      for (tkVersionVector::iterator itVersion = maskVersions.begin() ; itVersion != maskVersions.end() ; itVersion ++) {
		TkVersion *tkVersion = (*itVersion) ;
		if ( (tkVersion->getVersionMajorId() == 0) && (tkVersion->getVersionMinorId() == 0) ) {
		  
		  if (!disableDone) {
		    if ( (tkState->getMaskVersionMajorId() == 0) && (tkState->getMaskVersionMinorId() == 0) )
		      *out << "<option selected>" << "Disable" << "</option>";
		    else
		      *out << "<option>" << "Disable" << "</option>";
		    disableDone = true ;
		  }
		}
		else if ( (tkVersion->getVersionMajorId() == tkState->getMaskVersionMajorId()) && 
			  (tkVersion->getVersionMinorId() == tkState->getMaskVersionMinorId()) )
		  *out << "<option selected>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
		else
		  *out << "<option>" << tkVersion->getVersionMajorId() << "." << tkVersion->getVersionMinorId() << "</option>";
	      }
	      *out << "</select>" << std::endl ;
	      *out << cgicc::td() << std::endl ;
	      // ---------------------- End of mask version
	    
	      *out << cgicc::tr() << std::endl ;
	    }
	    catch (FecExceptionHandler &e) {
	      //*out << cgicc::h2("Error of version for the partition " + tkState->getPartitionName() + ": " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	      errorReportLogger_->errorReport ("Error of version for the partition " + tkState->getPartitionName(), e, LOGFATAL) ;
	    }
	    catch (oracle::occi::SQLException &e) {
	      //*out << cgicc::h2("Error of version for the partition " + tkState->getPartitionName() + ": " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	      errorReportLogger_->errorReport ("Error of version for the partition " + tkState->getPartitionName(), e, LOGFATAL) ;
	    }
	  }
	}
	*out << cgicc::table() << std::endl ;

	// New state name
	*out << cgicc::br() << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please specify a new state name: " << "</big></big></span>"
	     << cgicc::input().set("type","text").set("name","NewStateName")
	  .set("value","") << std::endl;

	// View all partitions
	if (viewAllPartitionNames_) {
	  *out << cgicc::br() << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	       << cgicc::input().set("type", "checkbox").set("name","viewAllPartitionNames").set("checked","true")
	       << "View all Partitions" << "</big></big></span>" ;
	}
	else {
	  *out << cgicc::br() << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	       << cgicc::input().set("type", "checkbox").set("name","viewAllPartitionNames")
	       << "View all Partitions" << "</big></big></span>" ;
	}
	
	// Apply
	*out << cgicc::p() << cgicc::input().set("type", "submit")
	  .set("name", "submit")
	  .set("value", "Apply");

	// Few remarks
	*out << cgicc::h2("Few remarks") << std::endl ;
	*out << "<lu>" << std::endl ;
	*out << "<li>" << "If you click APPLY then a new state will be created with the versions you specify and the state will be became the current state" << "</li>" << std::endl ;
	*out << "<li>" << "If you create a new state, please specify a state name" << "</li>" << std::endl ;
	*out << "<li>" << "If you want to disable a partition, you need to disable all versions of the corresponding partition" << "</li>" << std::endl ;
	*out << "<li>" << "By definition you can have only one partition per state (for the MTCC at least)" << std::endl ;
	*out << "<li>" << "If you select another state than the current state, then the new state is set the state to be downloaded and will be displayed" << std::endl ;
	*out << "<li>" << "If you click apply and if you specify a new state name then a new state is created in the database" << std::endl ;
	if (!viewAllPartitionNames_)
	  *out << "<li>" << "Only current is showed, enable \"View all Partitions\" to display all partitions" << std::endl ;
	else
	  *out << "<li>" << "All partitions are displayed, disable \"View All Partitions\" to have only the current state" << std::endl ;
	*out << "</lu>" << std::endl ;
      }
      else {
	*out << cgicc::h2("No Current State Defined in the Database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
    }
    catch (FecExceptionHandler &e) {
      if (tableStarted) *out << cgicc::table() << std::endl ;
      *out << cgicc::h2("Problem in accessing the database: " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Cannot retreive the current state", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      if (tableStarted) *out << cgicc::table() << std::endl ;
      *out << cgicc::h2("Problem in accessing the database (Oracle exception): " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Cannot retreive the current state", e, LOGFATAL) ;
    }

    // end of the form
    *out << cgicc::form() << std::endl ;
  }
  else {
    *out << cgicc::h2("Please specify a database to use these links").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;      
  }

  *out << cgicc::html() << std::endl ; 
}

/** Apply the state change done in the method displayStateManagement
 */
void TkConfigurationDb::applyStateManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;

  xgi::Utils::getPageHeader(*out, "Partition/Version Management");
  //<HEAD>.....<META HTTP-EQUIV="Refresh" CONTENT="10">.....</HEAD>

  displayDatabaseRelatedLinks(in,out) ;

  // Create a new Cgicc object containing all the CGI data
  cgicc::Cgicc cgi(in);

  // Change the view all partitions or only the partition in the current states
  bool viewAllPartitionNamesChanged = false ;
  if ( (!viewAllPartitionNames_ && cgi.queryCheckbox("viewAllPartitionNames")) ||
       (viewAllPartitionNames_ && !cgi.queryCheckbox("viewAllPartitionNames")) ) {
    viewAllPartitionNamesChanged = true ;
    viewAllPartitionNames_ = cgi.queryCheckbox("viewAllPartitionNames") ;
    if (viewAllPartitionNames_) {
      *out << cgicc::h2("All partitions will be displayed").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
      //this->displayStateManagement(in,out) ;
    }
    else {
      *out << cgicc::h2("Only partitions in the current state will be displayed").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
      //this->displayStateManagement(in,out) ;
    }
  }

//   if (dbStates_.size() == 0) {
//     if (!viewAllPartitionNames_) {
//       *out << cgicc::h2("Cannot retreive the current state").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
//       errorReportLogger_->errorReport ("Cannot retreive the current state (you should pass by Partition/Version first and please also check and clear the cache of your browser)", LOGFATAL) ;
//     }
//     else viewAllPartitionNames_ = true ;
//   }
//   else {
  {
    try {
      // Retreive the states
      TkState *firstState = (TkState *)(*dbStates_.begin()) ;

      std::string currentDbState = "NOTASSOCIATED" ;
      if (firstState != NULL) currentDbState = firstState->getStateName() ; 

      // Check the state name
      std::list<std::string> stateNameList = deviceFactory_->getAllStateHistoryNames();
      unsigned int numberOfState = stateNameList.size() ;
      std::string selectedStateName = cgi["StateName"]->getValue() ;

      // Change the state between all states
      //if ( (numberOfState != 0) && (selectedStateName != "") && (currentStateName_ != selectedStateName) && (currentDbState != "") && (cgi["NewStateName"]->getValue() == "") ) { 
      if ( (numberOfState != 0) && (selectedStateName != "") && (currentStateName_ != selectedStateName) && (cgi["NewStateName"]->getValue() == "") ) { 

	deviceFactory_->setCurrentState(selectedStateName) ;
	
	// Re-display the web pages
	*out << cgicc::h2("Current state changed to " + selectedStateName).set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
      }
      // --------------------------------------------------------------------------------
      else {      // Create new states


	// check that the current state was not changed between the submit and here since somebody may open the same application on another machine
	if (currentStateHistoryId_ != deviceFactory_->getCurrentStateHistoryId()) {

	  *out << cgicc::h2("The current state was changed by somebody else, you cannot change again this state, please reload the current state and check").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	}
	else {

	  // State name truncated to 100 char
	  char newStateNameChar[100] = {0} ; strncpy(newStateNameChar,cgi["NewStateName"]->getValue().c_str(),100) ;
	  std::string newStateName = std::string(newStateNameChar) ;

	  if (newStateName.size() != 0) {

	    bool change = true ; // if a version has changed for a partition then update the state
	    tkStateVector newCurrentState ;
	    bool removeDisablePartition = true ; // remove the disable partition from the state
	
	    for (tkStateVector::iterator itState = dbStates_.begin() ; itState != dbStates_.end() ; itState ++) {
	      
	      TkState *tkState = (*itState) ;

	      // Check the different version and create a new state to be download in the database
	      std::string fecVersion = cgi["FecVersion" + tkState->getPartitionName()]->getValue() ;
	      std::string fedVersion = cgi["FedVersion" + tkState->getPartitionName()]->getValue() ;
	      std::string connectionVersion = cgi["ConnectionVersion" + tkState->getPartitionName()]->getValue() ;
	      std::string dcuInfoVersion = cgi["DcuInfoVersion" + tkState->getPartitionName()]->getValue() ;
	      std::string dcuPsuMapVersion = cgi["DcuPsuMapVersion" + tkState->getPartitionName()]->getValue() ;
	      std::string maskVersion = cgi["MaskVersion" + tkState->getPartitionName()]->getValue() ;
	      
	      // create the version major, minor
	      TkState *tkNewState = NULL ;
	      unsigned int fecMajor = 0, fecMinor = 0, fedMajor = 0, fedMinor = 0 ;
	      unsigned int connectionMajor = 0, connectionMinor = 0, dcuInfoMajor = 0, dcuInfoMinor = 0 ;
	      unsigned int dcuPsuMapMajor = 0, dcuPsuMapMinor = 0 ;
	      unsigned int analysisVersionMapPointerId = 0; // BEL S.
	      unsigned int maskMajor = 0, maskMinor = 0 ;
	      if ( (fecVersion == "Disable") && (fedVersion == "Disable") &&
		   (connectionVersion == "Disable") && (dcuInfoVersion == "Disable") &&
		   (dcuPsuMapVersion == "Disable") ) {
		
		// Is the version was already disabled
		if ( ((tkState->getFecVersionMajorId() == 0)       && (tkState->getFecVersionMinorId() == 0))       ||
		     ((tkState->getFedVersionMajorId() == 0)       && (tkState->getFedVersionMinorId() == 0))       ||
		     ((tkState->getConnectionVersionMajorId() == 0)&& (tkState->getConnectionVersionMinorId() == 0))||
		     ((tkState->getDcuInfoVersionMajorId() == 0)   && (tkState->getDcuInfoVersionMinorId() == 0))   ||
		     ((tkState->getDcuPsuMapVersionMajorId() == 0) && (tkState->getDcuPsuMapVersionMinorId() == 0)) ||
		     ((tkState->getMaskVersionMajorId() == 0) && (tkState->getMaskVersionMinorId() == 0)) ) {
		  
		  change = true ;

		  //std::cout << fecMajor << "." << fecMinor << "/" << tkState->getFecVersionMajorId() << "." << tkState->getFecVersionMinorId() << std::endl ;
		  //std::cout << fedMajor << "." << fedMinor << "/" << tkState->getFedVersionMajorId() << "." << tkState->getFedVersionMinorId() << std::endl ;
		  //std::cout << connectionMajor << "." << connectionMinor << "/" << tkState->getConnectionVersionMajorId() << "." << tkState->getConnectionVersionMinorId() << std::endl ;
		  //std::cout << dcuInfoMajor << "." << dcuInfoMinor << "/" << tkState->getDcuInfoVersionMajorId() << "." << tkState->getDcuInfoVersionMinorId() << std::endl ;
		  //std::cout << dcuPsuMajor << "." << dcuPsuMinor << "/" << tkState->getDcuPsuMapVersionMajorId() << "." << tkState->getDcuPsuMapVersionMinorId() << std::endl ;
		  //std::cout << maskMajor << "." << maskMinor << "/" << tkState->getMaskVersionMajorId() << "." << tkState->getMaskVersionMinorId() << std::endl ;
		  
		  if (!removeDisablePartition) {	  
		    // Now set the new state with versions = 0
		    tkNewState = new TkState (newStateName, tkState->getPartitionName(), 
					      fecMajor, fecMinor, 
					      fedMajor, fedMinor, 
					      connectionMajor, connectionMinor,
					      dcuInfoMajor, dcuInfoMinor,
					      dcuPsuMapMajor, dcuPsuMapMinor, 
					      analysisVersionMapPointerId,
					      maskMajor, maskMinor) ;
		  }
		}
	      }
	      else {
	    
		// Disable ?
		if (fecVersion != "Disable") {
		  std::string::size_type ipass = fecVersion.find(".");
		  if (ipass != std::string::npos) {
		    fecMajor = fromString<int>(fecVersion.substr(0,ipass)) ;
		    fecMinor = fromString<int>(fecVersion.substr(ipass+1,fecVersion.size())) ;
		  }
		}
		if (fedVersion != "Disable") {
		  std::string::size_type ipass = fedVersion.find(".");
		  if (ipass != std::string::npos) {
		    fedMajor = fromString<int>(fedVersion.substr(0,ipass)) ;
		    fedMinor = fromString<int>(fedVersion.substr(ipass+1,fedVersion.size())) ;
		  }
		}
		if (connectionVersion != "Disable") {
		  std::string::size_type ipass = connectionVersion.find(".");
		  if (ipass != std::string::npos) {
		    connectionMajor = fromString<int>(connectionVersion.substr(0,ipass)) ;
		    connectionMinor = fromString<int>(connectionVersion.substr(ipass+1,connectionVersion.size())) ;
		  }
		}
		if (dcuInfoVersion != "Disable") {
		  std::string::size_type ipass = dcuInfoVersion.find(".");
		  if (ipass != std::string::npos) {
		    dcuInfoMajor = fromString<int>(dcuInfoVersion.substr(0,ipass)) ;
		    dcuInfoMinor = fromString<int>(dcuInfoVersion.substr(ipass+1,dcuInfoVersion.size())) ;
		  }
		}
		if (dcuPsuMapVersion != "Disable") {
		  std::string::size_type ipass = dcuPsuMapVersion.find(".");
		  if (ipass != std::string::npos) {
		    dcuPsuMapMajor = fromString<int>(dcuPsuMapVersion.substr(0,ipass)) ;
		    dcuPsuMapMinor = fromString<int>(dcuPsuMapVersion.substr(ipass+1,dcuPsuMapVersion.size())) ;
		  }
		}
		if (maskVersion != "Disable") {
		  std::string::size_type ipass = maskVersion.find(".");
		  if (ipass != std::string::npos) {
		    maskMajor = fromString<int>(maskVersion.substr(0,ipass)) ;
		    maskMinor = fromString<int>(maskVersion.substr(ipass+1,maskVersion.size())) ;
		  }
		}

		// Check if the version was the same (only if no change has been done)
		if (!change) {
		  if ( (fecMajor != tkState->getFecVersionMajorId())      || (fecMinor != tkState->getFecVersionMinorId()) ||
		       (fedMajor != tkState->getFedVersionMajorId())      || (fedMinor != tkState->getFedVersionMinorId()) ||
		       (connectionMajor != tkState->getConnectionVersionMajorId()) || (connectionMinor != tkState->getConnectionVersionMinorId())  ||
		       (dcuInfoMajor != tkState->getDcuInfoVersionMajorId())       || (dcuInfoMinor != tkState->getDcuInfoVersionMinorId()) ||
		       (dcuPsuMapMajor != tkState->getDcuPsuMapVersionMajorId())   || (dcuPsuMapMinor != tkState->getDcuPsuMapVersionMinorId()) ||
		       (maskMajor != tkState->getMaskVersionMajorId())             || (maskMinor != tkState->getMaskVersionMinorId()) ) {
		    change = true ;

		    //std::cout << fecMajor << "." << fecMinor << "/" << tkState->getFecVersionMajorId() << "." << tkState->getFecVersionMinorId() << std::endl ;
		    //std::cout << fedMajor << "." << fedMinor << "/" << tkState->getFedVersionMajorId() << "." << tkState->getFedVersionMinorId() << std::endl ;
		    //std::cout << connectionMajor << "." << connectionMinor << "/" << tkState->getConnectionVersionMajorId() << "." << tkState->getConnectionVersionMinorId() << std::endl ;
		    //std::cout << dcuInfoMajor << "." << dcuInfoMinor << "/" << tkState->getDcuInfoVersionMajorId() << "." << tkState->getDcuInfoVersionMinorId() << std::endl ;
		    //std::cout << dcuPsuMajor << "." << dcuPsuMinor << "/" << tkState->getDcuPsuMapVersionMajorId() << "." << tkState->getDcuPsuMapVersionMinorId() << std::endl ;
		    //std::cout << maskMajor << "." << maskMinor << "/" << tkState->getMaskVersionMajorId() << "." << tkState->getMaskVersionMinorId() << std::endl ;
		  }
		}
		
		// Now set the new state with versions
		tkNewState = new TkState (newStateName, tkState->getPartitionName(), 
					  fecMajor, fecMinor, 
					  fedMajor, fedMinor, 
					  connectionMajor, connectionMinor,
					  dcuInfoMajor, dcuInfoMinor,
					  dcuPsuMapMajor, dcuPsuMapMinor, 
					  analysisVersionMapPointerId,
					  maskMajor, maskMinor ) ;
	      }
	      
	      // Add the new state
	      if (tkNewState != NULL) newCurrentState.push_back(tkNewState) ;
	    }
	    
	    // Set the state to the database
	    if ( change && (newCurrentState.size() > 0) ) {
	    
	      try {

		// Display it
		// 	  for (tkStateVector::iterator itState = newCurrentState.begin() ; itState != newCurrentState.end() ; itState ++) {
		// 	    TkState *tkState = (*itState) ;
		// 	    tkState->display() ;
		// 	  }
		
		//unsigned int stateNumber = 
		deviceFactory_->setCurrentState ( newCurrentState ) ;
		*out << cgicc::h2("The new state was added in the database and set as the next to be downloaded").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
		
		if (newCurrentState.size() != 1) {
		  *out << cgicc::h2("You cannot have several partition/versions in one state, please correct by disabling the partition not used").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
		}
	      }
	      catch (FecExceptionHandler &e) {
		*out << cgicc::h2("Problem in accessing the database: " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
		
		errorReportLogger_->errorReport ("Cannot upload the current state", e, LOGFATAL) ;
	      }
	      catch (oracle::occi::SQLException &e) {
		*out << cgicc::h2("Error during the upload of the state (ORACLE exception): " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
		
		errorReportLogger_->errorReport ("Cannot upload the current state", e, LOGFATAL) ;
	      }
	    }
	    else {
	      *out << cgicc::h2("No change on the state, command not applied").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	    }

	    // Delete all the states created
	    if (newCurrentState.size() > 0) TkDcuInfoFactory::deleteStateVectorI (newCurrentState) ;
	  }
	  else {
	    if (!viewAllPartitionNamesChanged)
	      *out << cgicc::h2("You need to specify a new state name").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	  }
	}
      }

      // Display the states
      //this->displayStateManagement(in,out) ;
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Error in state management " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Cannot upload the current state", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Error in state management " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Cannot upload the current state", e, LOGFATAL) ;
    }
    catch(const std::exception& e) {
      *out << cgicc::h2("Error CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport (e.what(), LOGERROR) ;
      XCEPT_RAISE(xgi::exception::Exception,  e.what());
    }
  }

  *out << cgicc::html() << std::endl ; 
}

/** Display all the states (partition / version) and allow the user to change it
 */
void TkConfigurationDb::displayPartitionFecParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 2 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Partition FEC Parameters");

  displayDatabaseRelatedLinks(in,out) ;

  if ((deviceFactory_ != NULL) && (deviceFactory_->getDbConnected())) {

    // Form and action
    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    url += "/ApplyPartitionFecParameters";
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;

    try {
      // Retreive the partition names
      std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNames() ;
      if (partitionNames.size()) {

 	// Build a list of partition name
 	std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
 	for (std::list<std::string>::iterator itPartition = partitionNames.begin() ; itPartition != partitionNames.end() ; itPartition ++) {
 	  if (partitionDbName_ == (*itPartition)) 
 	    partitionButton << "<option selected>" << (*itPartition) << "</option>";
 	  else
 	    partitionButton << "<option>" << (*itPartition) << "</option>";
 	}
 	partitionButton << "</select>" << std::endl ;

	// Display the partition name / FEC version / Registers / Operation
	// Build a list of device registers to be changed
	std::stringstream apvRegisterName ; 
	apvRegisterName << "<select name=\"ApvRegisterName\">" ;
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::APVMODE] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::APVLATENCY] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::MUXGAIN] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::IPRE] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::IPCASC] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::IPSF] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::ISHA] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::ISSF] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::IPSP] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::IMUXIN] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::ICAL] << "</option>";
	//apvRegisterName <<  "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::ISPARE] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::VFP] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::VFS] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::VPSP] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::CDRV] << "</option>";
	apvRegisterName << "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::CSEL] << "</option>";
	//apvRegisterName <<  "<option>" << apvDescription::APVPARAMETERNAMES[apvDescription::APVERROR] << "</option>";
	apvRegisterName << "</select>" << std::endl ;

	// Build a list of device registers to be changed
	std::stringstream pllRegisterName ; 
	pllRegisterName << "<select name=\"PllRegisterName\">" ;
	pllRegisterName <<  "<option>" << pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYFINE] << "</option>";
	pllRegisterName <<  "<option>" << pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYCOARSE] << "</option>";
	pllRegisterName << "</select>" << std::endl ;

	// Build a list of device registers to be changed
	std::stringstream aohRegisterName ; 
	aohRegisterName << "<select name=\"AohRegisterName\">" ;
	aohRegisterName << "<option>" << "All Bias" << "</option>";
	aohRegisterName << "<option>" << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS0] << "</option>";
	aohRegisterName << "<option>" << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS1] << "</option>";
	aohRegisterName << "<option>" << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS2] << "</option>";
	aohRegisterName << "<option>" << "All Gains" << "</option>";
	aohRegisterName << "<option>" << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN0] << "</option>";
	aohRegisterName << "<option>" << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN1] << "</option>";
	aohRegisterName << "<option>" << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN2] << "</option>";
	aohRegisterName << "</select>" << std::endl ;

	// Display Web Page
	*out << cgicc::h2("Change of Registers in a Partition") ;

	// Partition name to be selected
	*out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please select the partition: " << "</big></big></span>"
	     << partitionButton.str() << std::endl ;

	*out << cgicc::p() << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << cgicc::tr() << std::endl ;
	*out << cgicc::th() << "Set It" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "Device Register" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "Operation" << cgicc::th() << std::endl ;
	*out << cgicc::th() << "Value" << cgicc::th() << std::endl ;
	*out << cgicc::tr() << std::endl ;

	// Web Display in a table
	*out << cgicc::tr() << std::endl; 
	*out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","ApvSetIt") << cgicc::td() << std::endl ;
	*out << cgicc::td() << apvRegisterName.str() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<select name=\"ApvOperationName\">" 
	     << "<option selected>" << "SET" << "</option>" 
	  //	       << "<option>" << "ADD" << "</option>" 
	  //	       << "<option>" << "REMOVE" << "</option>" 
	     << "</select>" << cgicc::td() << std::endl ;
	*out << cgicc::td() << cgicc::input().set("type","text").set("name","ApvValue").set("size","3") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl; 
	*out << cgicc::tr() << std::endl; 
	*out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","PllSetIt") << cgicc::td() << std::endl ;
	*out << cgicc::td() << pllRegisterName.str() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<select name=\"PllOperationName\">" 
	     << "<option selected>" << "SET" << "</option>" 
	     << "<option>" << "ADD" << "</option>" 
	     << "<option>" << "REMOVE" << "</option>" 
	     << "</select>" << cgicc::td() << std::endl ;
	*out << cgicc::td() << cgicc::input().set("type","text").set("name","PllValue").set("size","3") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl; 
	*out << cgicc::tr() << std::endl; 
	*out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","AohSetIt") << cgicc::td() << std::endl ;
	*out << cgicc::td() << aohRegisterName.str() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<select name=\"AohOperationName\">" 
	     << "<option selected>" << "SET" << "</option>" 
	// 	       << "<option>" << "ADD" << "</option>" 
	// 	       << "<option>" << "REMOVE" << "</option>" 
	     << "</select>" << cgicc::td() << std::endl ;
	*out << cgicc::td() << cgicc::input().set("type","text").set("name","AohValue").set("size","3") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::table() << std::endl ;

	// Set it in the database
	*out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please check one these boxes once you have performed all modifications for a Partition: " 
	     << "</big></big></span>" << std::endl ;
	*out << cgicc::p() << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<big>" << "No action (keep the parameter as it is for next Apply): " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","NoAction").set("checked","true") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl ;
	*out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<big>" << "Create a major version: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CreateMajorVersion") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl ;
	*out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<big>" << "Create a minor version: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CreateMinorVersion") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<big>" << "Copy a version (minor -> major): " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CopyVersion") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<big>" << "Dump into a file: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","dumpIntoFile") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<big>" << "Reset the modifications already performed: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","ResetModifications") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	*out << cgicc::td() << "<big>" << "Geometry APV Parameter Changes (enter a file name): " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","GeometryModification") << cgicc::td() << std::endl ;
	*out << cgicc::td() << cgicc::input().set("type","text").set("name","GeometryFileName").set("size","40") << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::table() << std::endl ;

	// Apply
	*out << cgicc::p() << cgicc::input().set("type", "submit")
	  .set("name", "submit")
	  .set("value", "Apply");

	// Few remarks
	*out << cgicc::h2("Few remarks") << std::endl ;
	*out << "<lu>" << std::endl ;
	*out << "<li>" << "Perform the modifications Partition per Partition do not mix it or you will loose modifications" << "</li>" << std::endl ;
	*out << "<li>" << "You can change all the parameters and APLLY several times before upload to the database" << "</li>" << std::endl ;
	*out << "<li>" << "When you upload in the database, a new minor version is created" << "</li>" << std::endl ;
	*out << "<li>" << "If you upload into a minor version, only the APV or PLL or AOH will be submit" << "</li>" << std::endl ;
	*out << "<li>" << "If you upload into a major version, all the devices are uploaded to the database" << "</li>" << std::endl ;
	*out << "</lu>" << std::endl ;
      }
      else {
	*out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Cannot retreive the current state " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Cannot retreive the current state", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Cannot retreive the current state (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Cannot retreive the current state", e, LOGFATAL) ;
    }

    // end of the form
    *out << cgicc::form() << std::endl ;

  }
  else {
    *out << cgicc::h2("Please specify a database to use these links").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;      
  }

  *out << cgicc::html() << std::endl ; 
}

/** Apply the state change done in the method displayPartitionFecParameters
 */
void TkConfigurationDb::applyPartitionFecParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Partition FEC Parameters");

  displayDatabaseRelatedLinks(in,out) ;

  if (dbStates_.size() == 0) {
    try {
      // Retreive all the states
      dbStates_ = deviceFactory_->getCurrentStates () ;
      if (!dbStates_.size()) {
	*out << cgicc::h2("Cannot retreive the current state").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	errorReportLogger_->errorReport ("Cannot retreive the current state", LOGFATAL) ;
      }
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Cannot retreive the current state " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Cannot retreive the current state", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Cannot retreive the current state (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Cannot retreive the current state", e, LOGFATAL) ;
    }
  }

  if (dbStates_.size()) {
    try {
      // Create a new Cgicc object containing all the CGI data
      cgicc::Cgicc cgi(in);

      bool createMajorVersion = false ;
      if (cgi["actionToBeApply"]->getValue() == "CreateMajorVersion") createMajorVersion = true ;
      bool createMinorVersion = false ;
      if (cgi["actionToBeApply"]->getValue() == "CreateMinorVersion") createMinorVersion = true ;
      bool copyVersion        = false ;
      if (cgi["actionToBeApply"]->getValue() == "CopyVersion") copyVersion = true ;
      bool dumpIntoFile = false ;
      if (cgi["actionToBeApply"]->getValue() == "dumpIntoFile") dumpIntoFile = true ;
      bool resetModifications = false ;
      if (cgi["actionToBeApply"]->getValue() == "ResetModifications") resetModifications = true ;
      bool geometryChange = false ;
      if (cgi["actionToBeApply"]->getValue() == "GeometryModification") geometryChange = true ;

      // Reseting the data
      if (resetModifications) {
	//partitionDbName_ = "" ;
	*out << cgicc::h2("Reset the data for partition " + partitionDbName_) << std::endl ;
	downloadAllDevicesFromDb () ;

	// Only for minor version
	apvDbChanged_ = false ;
	pllDbChanged_ = false ;
	aohDbChanged_ = false ;
      }
      // Make the changes asked
      else {

	// Change of partition ?
	if (partitionDbName_ != std::string(cgi["PartitionName"]->getValue())) {
	  partitionDbName_ = cgi["PartitionName"]->getValue() ;
	  downloadAllDevicesFromDb () ;
	}

	// Any devices in this partition ?
	if (deviceValues_.size() > 0) {
	  
	  // Find the values to be set
	  int apvValue = 0 ; std::string apvRegister = "" ; bool apvChange = false ;
	  if (cgi.queryCheckbox("ApvSetIt")) {
	    apvRegister = cgi["ApvRegisterName"]->getValue() ;
	    std::string valueStr = cgi["ApvValue"]->getValue() ;
	    if (valueStr != "") {
	      apvValue = fromString<int>(valueStr) ;
	      apvChange = true ;
	    }
	    else 
	      *out << cgicc::h2("Invalid value for the APV registers").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	  }
	
	  // Find the values to be set
	  int pllValue = 0 ; std::string pllRegister = ""; bool pllChange = false ;
	  if (cgi.queryCheckbox("PllSetIt")) {
	    pllRegister = cgi["PllRegisterName"]->getValue() ;
	    std::string valueStr = cgi["PllValue"]->getValue() ;
	    if (valueStr != "") {
	      pllValue = fromString<int>(valueStr) ;
	      pllChange = true ;
	    }
	    else 
	      *out << cgicc::h2("Invalid value for the PLL registers").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	  }
	  
	  // Find the values to be set
	  int aohValue = 0 ; std::string aohRegister = "" ; bool aohChange = false ;
	  if (cgi.queryCheckbox("AohSetIt")) {
	    aohRegister = cgi["aohRegisterName"]->getValue() ;
	    std::string valueStr = cgi["AohValue"]->getValue() ;
	    if (valueStr != "") {
	      aohValue = fromString<int>(valueStr) ;
	      aohChange = true ;
	    }
	    else 
	      *out << cgicc::h2("Invalid value for the AOH registers").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	  }
	  
	  if (pllChange || apvChange || aohChange) {
	    // Change the devices
	    std::list<std::string> errorList ;
	    for (deviceVector::iterator itDevice = deviceValues_.begin() ; itDevice != deviceValues_.end() ; itDevice ++) {
	      
	      deviceDescription *deviceD = *itDevice ;
	      switch (deviceD->getDeviceType()) {
	      case APV25: 
		if (apvChange) {
		  apvDescription *apvD = dynamic_cast<apvDescription *>(deviceD) ;
		  // which parameter was changed ?
		  if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::APVMODE]) {
		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getApvMode()) {
			apvD->setApvMode((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV mode is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getApvMode() + apvValue ;
		      apvD->setApvMode(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getApvMode() - apvValue ;
		      apvD->setApvMode(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::APVLATENCY]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getLatency()) {
			apvD->setLatency((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV latency is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getLatency() + apvValue ;
		      apvD->setLatency(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getLatency() - apvValue ;
		      apvD->setLatency(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::MUXGAIN]) {
		  
		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getMuxGain()) {
			apvD->setMuxGain((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV Mux gain is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getMuxGain() + apvValue ;
		      apvD->setMuxGain(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getMuxGain() - apvValue ;
		      apvD->setMuxGain(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::IPRE]) {
		  
		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getIpre()) {
			apvD->setIpre((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV IPRE is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getIpre() + apvValue ;
		      apvD->setIpre(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getIpre() - apvValue ;
		      apvD->setIpre(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::IPCASC]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getIpcasc()) {
			apvDbChanged_ = true ;
			apvD->setIpcasc((tscType8)apvValue) ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV IPCASC is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getIpcasc() + apvValue ;
		      apvD->setIpcasc(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getIpcasc() - apvValue ;
		      apvD->setIpcasc(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::IPSF]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getIpsf()) {
			apvD->setIpsf((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back(" is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getIpsf() + apvValue ;
		      apvD->setIpsf(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getIpsf() - apvValue ;
		      apvD->setIpsf(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::ISHA]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getIsha()) {
			apvD->setIsha((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV ISHA is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getIsha() + apvValue ;
		      apvD->setIsha(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getIsha() - apvValue ;
		      apvD->setIsha(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::ISSF]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getIssf()) {
			apvD->setIssf((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV ISSF is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getIssf() + apvValue ;
		      apvD->setIssf(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getIssf() - apvValue ;
		      apvD->setIssf(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::IPSP]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getIpsp()) {
			apvD->setIpsp((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV IPSP is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getIpsp() + apvValue ;
		      apvD->setIpsp(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getIpsp() - apvValue ;
		      apvD->setIpsp(value) ;
		      apvDbChanged_ = true ;
		    }
		  }	      
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::IMUXIN]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getImuxin()) {
			apvD->setImuxin((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV IMUXIN is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getImuxin() + apvValue ;
		      apvD->setImuxin(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getImuxin() - apvValue ;
		      apvD->setImuxin(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::ICAL]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getIcal()) {
			apvD->setIcal((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV ICAL is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getIcal() + apvValue ;
		      apvD->setIcal(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getIcal() - apvValue ;
		      apvD->setIcal(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::ISPARE]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getIspare()) {
			apvD->setIspare((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV ISPARE is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getIspare() + apvValue ;
		      apvD->setIspare(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getIspare() - apvValue ;
		      apvD->setIspare(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::VFP]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getVfp()) {
			apvD->setVfp((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV VFP is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getVfp() + apvValue ;
		      apvD->setVfp(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getVfp() - apvValue ;
		      apvD->setVfp(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::VFS]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getVfs()) {
			apvD->setVfs((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV VFS is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getVfs() + apvValue ;
		      apvD->setVfs(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getVfs() - apvValue ;
		      apvD->setVfs(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::VPSP]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getVpsp()) {
			apvD->setVpsp((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV VPSP is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getVpsp() + apvValue ;
		      apvD->setVpsp(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getVpsp() - apvValue ;
		      apvD->setVpsp(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::CDRV]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getCdrv()) {
			apvD->setCdrv((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV CDRV is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getCdrv() + apvValue ;
		      apvD->setCdrv(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getCdrv() - apvValue ;
		      apvD->setCdrv(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::CSEL]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getCsel()) {
			apvD->setCsel((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV CSEL is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getCsel() + apvValue ;
		      apvD->setCsel(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getCsel() - apvValue ;
		      apvD->setCsel(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		  else if (apvRegister == apvDescription::APVPARAMETERNAMES[apvDescription::APVERROR]) {

		    if (cgi["ApvOperationName"]->getValue() == "SET") {
		      if ((tscType8)apvValue != apvD->getApvError()) {
			apvD->setApvError((tscType8)apvValue) ;
			apvDbChanged_ = true ;
		      }
		      //else if (!apvDbChanged_) errorList.push_back("APV APVERROR is the same in DB, no change") ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = apvD->getApvError() + apvValue ;
		      apvD->setApvError(value) ;
		      apvDbChanged_ = true ;
		    }
		    else if (cgi["ApvOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = apvD->getApvError() - apvValue ;
		      apvD->setApvError(value) ;
		      apvDbChanged_ = true ;
		    }
		  }
		}
		break ;
	      case APVMUX: break ;
	      case PLL: 
		if (pllChange) {

		  pllDescription *pllD = dynamic_cast<pllDescription *>(deviceD) ;
		  if (pllRegister == pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYFINE]) {

		    if (cgi["PllOperationName"]->getValue() == "SET") {
		      if (pllD->getDelayFine() != (tscType8)pllValue) {
			pllD->setDelayFine((tscType8)pllValue) ;
			pllDbChanged_ = true ;
		      }
		      //else if (!pllDbChanged_) errorList.push_back("PLL Delay fine is the same in DB, no change") ;
		    }
		    else if (cgi["PllOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = pllD->getDelayFine() + pllValue ;
		      pllD->setDelayFine(value) ;
		      pllDbChanged_ = true ;
		    }
		    else if (cgi["PllOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = pllD->getDelayFine() - pllValue ;
		      pllD->setDelayFine(value) ;
		      pllDbChanged_ = true ;
		    }
		  }
		  else if (pllRegister == pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYCOARSE]) {

		    if (cgi["PllOperationName"]->getValue() == "SET") {
		      if (pllD->getDelayCoarse() != (tscType8)pllValue) {
			pllD->setDelayCoarse((tscType8)pllValue) ;
			pllDbChanged_ = true ;
		      }
		      //else if (!pllDbChanged_) errorList.push_back("PLL Delay coarse is the same in DB, no change") ;
		    }
		    else if (cgi["PllOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = pllD->getDelayCoarse() + pllValue ;
		      pllD->setDelayCoarse(value) ;
		      pllDbChanged_ = true ;
		    }
		    else if (cgi["PllOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = pllD->getDelayCoarse() - pllValue ;
		      pllD->setDelayCoarse(value) ;
		      pllDbChanged_ = true ;
		    }
		  }
		}
		break ;
	      case LASERDRIVER: 

		if (aohChange && (getAddressKey(deviceD->getKey()) == 0x60)) {
		  laserdriverDescription *aohD = dynamic_cast<laserdriverDescription *>(deviceD) ;
		  if ( (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS0]) ||
		       (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS1]) ||
		       (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS2]) ||
		       (aohRegister == "All Bias") ) {

		    tscType8 bias[3] ; aohD->getBias(bias) ;
		    if ( (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS0]) ||
			 (aohRegister == "All Bias") ) {
		    
		      if (cgi["AohOperationName"]->getValue() == "SET") {
			if (bias[0] != (tscType8)aohValue) {
			  bias[0] = (tscType8)aohValue ;
			  aohDbChanged_ = true ;
			}
		      }
		      else if (cgi["AohOperationName"]->getValue() == "ADD") {
		      
			tscType8 value = bias[0] + aohValue ;
			bias[0] = value ;
			aohDbChanged_ = true ;
		      }
		      else if (cgi["AohOperationName"]->getValue() == "REMOVE") {
		      
			tscType8 value = bias[0] - aohValue ;
			bias[0] = value ;
			aohDbChanged_ = true ;
		      }
		    }
		    if ( (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS1]) ||
			 (aohRegister == "All Bias") ) {

		      if (cgi["AohOperationName"]->getValue() == "SET") {
			if (bias[1] != (tscType8)aohValue) {
			  bias[1] = (tscType8)aohValue ;
			  aohDbChanged_ = true ;
			}
		      }
		      else if (cgi["AohOperationName"]->getValue() == "ADD") {
		    
			tscType8 value = bias[1] + aohValue ;
			bias[1] = value ;
			aohDbChanged_ = true ;
		      }
		      else if (cgi["AohOperationName"]->getValue() == "REMOVE") {
		      
			tscType8 value = bias[1] - aohValue ;
			bias[1] = value ;
			aohDbChanged_ = true ;
		      }
		    }
		    if ( (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS2]) ||
			 (aohRegister == "All Bias") ) {
		      if (cgi["AohOperationName"]->getValue() == "SET") {
			if (bias[2] != (tscType8)aohValue) {
			  bias[2] = (tscType8)aohValue ;
			  aohDbChanged_ = true ;
			}
		      }
		      else if (cgi["AohOperationName"]->getValue() == "ADD") {
		    
			tscType8 value = bias[2] + aohValue ;
			bias[2] = value ;
			aohDbChanged_ = true ;
		      }
		      else if (cgi["AohOperationName"]->getValue() == "REMOVE") {
		      
			tscType8 value = bias[2] - aohValue ;
			bias[2] = value ;
			aohDbChanged_ = true ;
		      }
		    }
		    
		    if (aohDbChanged_) aohD->setBias(bias) ;
		    //else errorList.push_back("AOH Bias are the same in DB, no change") ;
		  }
		  if (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN0]) {
		    if (cgi["AohOperationName"]->getValue() == "SET") {
		      if ((tscType8)aohValue != aohD->getGain(0)) {
			aohD->setGain(0,(tscType8)aohValue) ;
			aohDbChanged_ = true ;
		      }
		      else if (!aohDbChanged_)errorList.push_back("AOH Gain 0 is the same in DB, no change") ;
		    }
		    else if (cgi["AohOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = aohD->getGain(0) + aohValue ;
		      aohD->setGain(0,value) ;
		      aohDbChanged_ = true ; 
		    }
		    else if (cgi["AohOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = aohD->getGain(0) - aohValue ;
		      aohD->setGain(0,value) ;
		      aohDbChanged_ = true ;
		    }
		  }
		  if (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN1]) {

		    if (cgi["AohOperationName"]->getValue() == "SET") {
		      if ((tscType8)aohValue != aohD->getGain(1)) {
			aohD->setGain(1,(tscType8)aohValue) ;
			aohDbChanged_ = true ;
		      }
		      //else if (!aohDbChanged_) errorList.push_back("AOH Gain 1 is the same in DB, no change") ;
		    }
		    else if (cgi["AohOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = aohD->getGain(1) + aohValue ;
		      aohD->setGain(1,value) ;
		      aohDbChanged_ = true ;
		    }
		    else if (cgi["AohOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = aohD->getGain(1) - aohValue ;
		      aohD->setGain(1,value) ;
		      aohDbChanged_ = true ;
		    }
		  }
		  if (aohRegister == laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN2]) {

		    if (cgi["AohOperationName"]->getValue() == "SET") {
		      if ((tscType8)aohValue != aohD->getGain(2)) {
			aohD->setGain(2,(tscType8)aohValue) ;
			aohDbChanged_ = true ;
		      }
		      //else if (!aohDbChanged_) errorList.push_back("AOH Gain 1 is the same in DB, no change") ;
		    }
		    else if (cgi["AohOperationName"]->getValue() == "ADD") {
		    
		      tscType8 value = aohD->getGain(2) + aohValue ;
		      aohD->setGain(2,value) ;
		      aohDbChanged_ = true ;
		    }
		    else if (cgi["AohOperationName"]->getValue() == "REMOVE") {
		    
		      tscType8 value = aohD->getGain(2) - aohValue ;
		      aohD->setGain(2,value) ;
		      aohDbChanged_ = true ;
		    }
		  }
		  if (aohRegister == "All Gains") {

		    if ( ((tscType8)aohValue != aohD->getGain(0)) ||
			 ((tscType8)aohValue != aohD->getGain(1)) ||
			 ((tscType8)aohValue != aohD->getGain(2)) ) {

		      if (cgi["AohOperationName"]->getValue() == "SET") {

			aohD->setGain(0,(tscType8)aohValue) ;
			aohD->setGain(1,(tscType8)aohValue) ;
			aohD->setGain(2,(tscType8)aohValue) ;
			aohDbChanged_ = true ;
		      }
		      else if (cgi["AohOperationName"]->getValue() == "ADD") {
		    
			aohD->setGain(0,(tscType8)(aohD->getGain(0) + aohValue)) ;
			aohD->setGain(1,(tscType8)(aohD->getGain(1) + aohValue)) ;
			aohD->setGain(2,(tscType8)(aohD->getGain(2) + aohValue)) ;
			aohDbChanged_ = true ;
		      }
		      else if (cgi["AohOperationName"]->getValue() == "REMOVE") {
		      
			aohD->setGain(0,(tscType8)(aohD->getGain(0) - aohValue)) ;
			aohD->setGain(1,(tscType8)(aohD->getGain(1) - aohValue)) ;
			aohD->setGain(2,(tscType8)(aohD->getGain(2) - aohValue)) ;
			aohDbChanged_ = true ;
		      }
		    }
		    //else if (!aohDbChanged_) errorList.push_back("AOH Gains are the same in DB, no change") ;
		  }
		}
		break ;
	      case DCU: break ;
	      case DOH: break ;
	      default: // it exists other devices but not used here
		errorList.push_back("Invalid device found in the database") ;
	      }
	    }
	  
	    // Display error list
	    if (errorList.size() != 0) {
	      *out << cgicc::h2("Some Error(s) Occur").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	      *out << "<lu>" << std::endl ;
	      for (std::list<std::string>::iterator it = errorList.begin() ; it != errorList.end() ; it ++) {
		*out << "<li>" << (*it) << "</li>" << std::endl ;
	      }
	      *out << "</lu>" << std::endl ;
	    }

	    if (apvDbChanged_ || pllDbChanged_ || aohDbChanged_) {
	      
	      *out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" << "Parameters has been updated for " 
		   << partitionDbName_ << "</big></big></span>" << std::endl ;
	      *out << "<br><big><span style=\"font-weight: bold;\">" << "Create a version to apply these changes" << "</big></span>" << std::endl ; 
	    }
	  }

	  // -----------------------------------------------------------------------------------------------
	  // Create the new versions
	  if ( (apvDbChanged_ || pllDbChanged_ || aohDbChanged_) &&
	       createMajorVersion || createMinorVersion || copyVersion || dumpIntoFile || geometryChange) {

	    unsigned int major = 0, minor = 0 ;
	    if (createMinorVersion) {
	      
	      deviceVector apvDevices, pllDevices, aohDevices ;
	      if (apvDbChanged_) apvDevices = FecFactory::getDeviceFromDeviceVector(deviceValues_, APV25) ;
	      if (pllDbChanged_) pllDevices = FecFactory::getDeviceFromDeviceVector(deviceValues_, PLL) ;
	      if (aohDbChanged_) aohDevices = FecFactory::getDeviceFromDeviceVector(deviceValues_, LASERDRIVER) ;
	      
	      // Merge the devices
	      for (deviceVector::iterator it = pllDevices.begin() ; it != pllDevices.end() ; it ++) {
		apvDevices.push_back((*it)) ;
	      }
	      for (deviceVector::iterator it = aohDevices.begin() ; it != aohDevices.end() ; it ++) {
		apvDevices.push_back((*it)) ;
	      }
	      
	      if (apvDevices.size() > 0) {
		// Upload the major version
		//deviceFactory_->setFecDeviceDescriptions (apvDevices, partitionDbName_, &major, &minor, false) ;
		deviceFactory_->setFecDeviceDescriptions (deviceValues_, partitionDbName_, &major, &minor, false) ;
	      }
	      else {
		*out << cgicc::h2("No change asked for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	      }
	    }
	    else if (createMajorVersion || copyVersion) {
	      
	      // Upload the major version
	      deviceFactory_->setFecDeviceDescriptions (deviceValues_, partitionDbName_, &major, &minor, true) ;
	    }
	    else if (dumpIntoFile) {
	      
	      if (xmlFECFileName_ == "") xmlFECFileName_ = "/tmp/" + partitionDbName_ + ".xml" ;
	      if (xmlFECFileName_ != "") {
		
		piaResetVector piaVector ;
		deviceFactory_->getPiaResetDescriptions(partitionDbName_, piaVector) ;
		deviceVector allDevices ;
		
		// Merge the two DCU and devices into the same vector
		for (deviceVector::iterator it = dcuDevices_.begin () ; it != dcuDevices_.end () ; it ++) {
		  allDevices.push_back((*it)) ;
		}
		for (deviceVector::iterator it = deviceValues_.begin () ; it != deviceValues_.end () ; it ++) {
		  allDevices.push_back((*it)) ;
		}
		
		// Upload the data to a file
		FecFactory fecFactory ;
		fecFactory.setOutputFileName(xmlFECFileName_) ;
		if (piaVector.size() > 0) 
		  fecFactory.setFecDevicePiaDescriptions (allDevices, piaVector) ;
		else
		  fecFactory.setFecDeviceDescriptions (allDevices) ;
		
		std::stringstream msgInfo ; msgInfo << "Upload done in file " << xmlFECFileName_ ;
		*out << cgicc::h2(msgInfo.str()) ;
	      }
	      else {
		*out << cgicc::h2("Please specify a file name for the upload to file in the <a href=\"/%s/MoreParameters\">[FEC/FED Parameters]</a>").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	      }
	    }
	    // ----------------------------------- Change the APV Parameters following an input file
	    else if (geometryChange) {
	      std::string fileName = cgi["GeometryFileName"]->getValue() ;
	      geometryChangeMethod ( in, out, fileName, &major, &minor ) ;
	    }
	  
	    if (!dumpIntoFile && major) {
	      // Display the result
	      std::stringstream msgInfo ; msgInfo << "Version " << std::dec << major << "." << minor << " created for partition " << partitionDbName_ ;
	      *out << cgicc::h2(msgInfo.str()) ;
	    }
	  }
	  else 
	    *out << cgicc::h2("No change asked for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	}
	else 
	  *out << cgicc::h2("No devices found for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
      }
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Problem in accessing device values in database: " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Problem in accessing device values in database", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Problem in accessing device values in database (Oracle exception): " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Problem in accessing device values in database", e, LOGFATAL) ;
    }
    catch(const std::exception& e) {
      *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport (e.what(), LOGERROR) ;
      XCEPT_RAISE(xgi::exception::Exception,  e.what());
    }
  }
  else
    *out << cgicc::h2("No state found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

  *out << cgicc::html() << std::endl ; 
}

/** Apply the change of the geometry on the devices
 */
void TkConfigurationDb::geometryChangeMethod ( xgi::Input * in, xgi::Output * out, std::string fileName, unsigned int *major, unsigned int *minor ) 
  throw (xgi::exception::Exception) {

  *major = 0 ; // in case of error major should be 0

  // -------------------------------------------------------------------------------
  // Open the file to verify its validity
  // if file is not opened then just display an error message
  // check also that the file name is not empty

  ifstream inputFile(fileName.c_str());
  bool fileOpened = inputFile.is_open() && inputFile.good();
  if (!fileOpened) {
    *out << cgicc::h2("The file for the geometry change cannot be accessed: " + fileName).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    return ;
  }
  else {
    *out << cgicc::h2("Reading data from " + fileName).set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
  }

  // -------------------------------------------------------------------------------
  // Sort the devices in order to have it by map
  Sgi::hash_map<keyType, deviceDescription *> mapDcuDevices ;
  for (deviceVector::iterator itDevice = deviceValues_.begin() ; itDevice != deviceValues_.end() ; itDevice ++) {
    mapDcuDevices[(*itDevice)->getKey()] = *itDevice ;
  }
  for (deviceVector::iterator itDevice = dcuDevices_.begin () ; itDevice != dcuDevices_.end () ; itDevice ++) {
    mapDcuDevices[(*itDevice)->getKey()] = *itDevice ;
  }

  // -------------------------------------------------------------------------------
  // For each retreived from the file
  bool firstError = true ;
  unsigned int deviceChanged = 0 ; // only upload if one devices has been changed
  while(!inputFile.eof()) {
    unsigned int fecSlot = 0, ringSlot = 0, ccuAddress = 0, i2cChannel = 0, i2cAddress = 0 ;
    unsigned int isha = 0, vfs = 0 ;
    inputFile >> fecSlot >> ringSlot >> ccuAddress >> i2cChannel >> i2cAddress >> isha >> vfs;
   
    // Create the FEC key
    keyType index = buildCompleteKey(fecSlot,ringSlot,ccuAddress,i2cChannel,i2cAddress) ;

    // Check the validity of the isha/vfs values (formal IO check)
    if(isha>256 || vfs>256 || isha==0 || vfs==0) {
      if (firstError) {
	*out << cgicc::h2("Found one or several errors").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	firstError = false ;
	*out << "<ul>" << std::endl ;
      }
      char msg[80] ;
      decodeKey (msg, index) ;
      std::stringstream msgError ; 
      if (isha == 0 || vfs==0) {
	msgError << "Missing data or trying to load wrong values for the device on " << msg ;
	msgError << ": (ISHA,VFS) = (" << isha << "," << vfs << ").";
      }
      else {
	msgError << "Trying to load wrong values for the device on " << msg ;
	msgError << ": (ISHA,VFS) = (" << isha << "," << vfs << ").";
      }
      *out << FEC_RED << "<li>" << msgError.str() << "</li>" << "</span>" << std::endl;
    }
    else {
      // ISHA and VFS are correct, then check the validity of the index
      if (mapDcuDevices.find(index) == mapDcuDevices.end()) { // Error, the key is not existing in the database, signal the error and continue
	if (firstError) {
	  *out << cgicc::h2("Found one or several errors").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	  firstError = false ;
	  *out << "<ul>" << std::endl ;
	}
	char msg[80] ;
	decodeKey (msg, index) ;
	std::stringstream msgError ; msgError << "The device on " << msg << " is not existing in the database for partition " << partitionDbName_ ;
	*out << FEC_RED << "<li>" << msgError.str() << "</li>" << "</span>" << std::endl;
      }
      else { // The device is existing change the value(s)
	
	if (mapDcuDevices[index]->getDeviceType() == APV25) {
	  apvDescription *apvD = dynamic_cast<apvDescription *>(mapDcuDevices[index]) ;
	  apvD->setIsha(isha) ; // value ISHA
	  apvD->setVfs(vfs) ;   // value VFS
	  deviceChanged ++ ;
	}
	else {
	  // Error one of the device is not considered as an APV
	  if (firstError) {
	    *out << cgicc::h2("Found one or several errors").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    firstError = false ;
	    *out << "<ul>" << std::endl ;
	  }
	  char msg[80] ;
	  decodeKey (msg, index) ;
	  std::stringstream msgError ; msgError << "The device on " << msg << " is not an APV25 in the database for the partition " << partitionDbName_ ;
	  if ( (mapDcuDevices[index]->getAddress() <= 0x20) && (mapDcuDevices[index]->getAddress() <= 0x25) ) 
	    msgError << ": the i2c address is an APV: inconsitency error in the database" ;
	  *out << FEC_RED << "<li>" << msgError << "</li>" << "</span>" << std::endl;
	}
      }
    }
  }

  if (!firstError) *out << "</ul>" << std::endl ;
  inputFile.close();

  // -------------------------------------------------------------------------------
  // Upload in the database if devices has been changed
  if (!deviceChanged) {
    *out << cgicc::h2("No device has been changed, no upload done to the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    return ;
  }
  else {
    try {
      // Upload to the database
      deviceFactory_->setFecDeviceDescriptions (deviceValues_, partitionDbName_, major, minor, false) ;
      *out << cgicc::h2("Found " + toString(deviceChanged) + " valid inputs from " + fileName).set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Problem in accessing device values in database: " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Problem in accessing device values in database", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Problem in accessing device values in database (Oracle exception): " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Problem in accessing device values in database", e, LOGFATAL) ;
    }
    catch(const std::exception& e) {
      *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport (e.what(), LOGERROR) ;
      XCEPT_RAISE(xgi::exception::Exception,  e.what());
    }
  }
}

/** Display a web page to disable and enable a module
 */
void TkConfigurationDb::displayModuleManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 3 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Module Management");

  displayDatabaseRelatedLinks(in,out) ;

  if ((deviceFactory_ != NULL) && (deviceFactory_->getDbConnected())) {

    // Form and action
    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    url += "/ApplyModuleManagement";
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;

    try {

      if (partitionDbName_ != "") {

	if (dcuDevices_.size()) {

	  // ----------------- DCU Info
	  bool dcuInformationRetreived = false ;
	  try {
	    deviceFactory_->addDetIdPartition(partitionDbName_) ;
	    dcuInformationRetreived = true ;
	  }
	  catch (FecExceptionHandler &e) {
	    *out << cgicc::h2("Unable to retreive the DCU information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the DCU information", e, LOGFATAL) ;
	  }
	  catch (oracle::occi::SQLException &e) {
	    *out << cgicc::h2("Unable to retreive the DCU information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the DCU information", e, LOGFATAL) ;
	  }

	  // ----------------- Connection Factory
	  bool connectionInformationRetreived = false ;
	  unsigned int errorConnection = 0 ;
	  ConnectionVector connections ;
	  try {
	    deviceFactory_->getConnectionDescriptions (partitionDbName_,connections,0,0,true) ;

	    //std::cout << "Found => " << "--------------------------------" << connections.size() << std::endl ;

	    connectionInformationRetreived = true ;
	  }
	  catch (FecExceptionHandler &e) {
	    *out << cgicc::h2("Unable to retreive the connection information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the information", e, LOGFATAL) ;
	  }
	  catch (oracle::occi::SQLException &e) {
	    *out << cgicc::h2("Unable to retreive the connection information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the connection information", e, LOGFATAL) ;
	  }

	  // ----------------- Display 
	  std::list<keyType> dcuIndexes ;
	  Sgi::hash_map<keyType, unsigned int> dcuIndexesHardId ;
	  Sgi::hash_map<keyType, bool> dcuDisabled ;
	  unsigned int dcuFehCounter = 0 ;
	  for (deviceVector::iterator it = dcuDevices_.begin() ; it != dcuDevices_.end() ; it ++) {
	    dcuDescription *dcuD =  dynamic_cast<dcuDescription *>(*it) ;
	    //std::cout << dcuD->getDcuType() << "/" << DCUFEH << std::endl ;
	    if (dcuD->getDcuType() == DCUFEH) {
	      dcuFehCounter ++ ;
	      dcuIndexes.push_back(dcuD->getKey()) ;
	      dcuIndexesHardId[dcuD->getKey()] = dcuD->getDcuHardId() ;
	      dcuDisabled[dcuD->getKey()] = isModuleDisabled(dcuD) ;
	    }
	  }
	  dcuIndexes.sort() ;

	  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << std::endl ;
	  *out << "<th>" << "DCU Hard Id" << "</th>" ;
	  *out << "<th>" << "FEC Position" << "</th>" ;
	  *out << "<th>" << "Disabled" << "</th>" ;
	  if (dcuInformationRetreived) {
	    *out << "<th>" << "DET ID" << "</th>" ;
	    *out << "<th>" << "# APV" << "</th>" ;
	  }
	  if (connectionInformationRetreived) {
	    *out << "<th>" << "FED ID" << "</th>" ;
	    *out << "<th>" << "FE Unit/Ch" << "</th>" ;
	    *out << "<th>" << "FED ID" << "</th>" ;
	    *out << "<th>" << "FE Unit/Ch" << "</th>" ;
	    *out << "<th>" << "FED ID" << "</th>" ;
	    *out << "<th>" << "FE Unit/Ch" << "</th>" ;
	  }
	  *out << cgicc::tr() << std::endl ;

	  std::string url = getApplicationDescriptor()->getURN();
	  *out << cgicc::h2("Found " + toString(dcuFehCounter) + " DCUs for partition " + partitionDbName_) << std::endl ;
	  for (std::list<keyType>::iterator it = dcuIndexes.begin() ; it != dcuIndexes.end() ; it ++) {

	    keyType index = (*it) ;
	    std::stringstream dcuPosition ;
	    dcuPosition << "Module on FEC " << std::dec << getFecKey(index) << " Ring " << getRingKey(index) 
			<< " CCU 0x" << std::hex << getCcuKey(index) << " channel 0x" << getChannelKey(index) ;

	    *out << cgicc::tr() ;
	    *out << cgicc::td() << "<a href=\"/" << url 
		 << "/displayModuleParameters?param1=" << toString(dcuIndexesHardId[index]) << "&param2=" << toHexString(index)
		 << "\" target=\"_blank\">" << dcuIndexesHardId[index] << "</a>" << cgicc::td() ;
	    *out << cgicc::td() << "<a href=\"/" << url 
		 << "/displayModuleParameters?param1=" << toString(dcuIndexesHardId[index]) << "&param2=" << toHexString(index)
		 << "\" target=\"_blank\">" << dcuPosition.str() << cgicc::td() ;
	    if (dcuDisabled[index]) 
	      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","DCU" + toString(dcuIndexesHardId[index])).set("checked","true") << cgicc::td() ;
	    else
	      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","DCU" + toString(dcuIndexesHardId[index])) << cgicc::td() ;

	    // DCU INFORMATION
	    int apvNumber = -1 ;
	    if (dcuInformationRetreived) {
	      try {
		TkDcuInfo *tkDcuInfo = deviceFactory_->getTkDcuInfo (dcuIndexesHardId[index]) ;
		*out << cgicc::td() << "<a href=\"/" << url 
		     << "/displayModuleParameters?param1=" << toString(dcuIndexesHardId[index]) << "&param2=" << toHexString(index)
		     << "\" target=\"_blank\">" << tkDcuInfo->getDetId() << cgicc::td() ;
		*out << cgicc::td() << tkDcuInfo->getApvNumber() << cgicc::td() ;

		apvNumber = tkDcuInfo->getApvNumber() ;
	      }
	      catch (FecExceptionHandler &e) {
		errorReportLogger_->errorReport ("Unable to retreive the DCU information for DCU " + toString(dcuIndexesHardId[index]), e, LOGFATAL) ;
		*out << cgicc::td() << "No" << cgicc::td() ;
		*out << cgicc::td() << "No" << cgicc::td() ;
	      }
	      catch (oracle::occi::SQLException &e) {
		errorReportLogger_->errorReport ("Unable to retreive the DCU information for DCU " + toString(dcuIndexesHardId[index]), e, LOGFATAL) ;
		*out << cgicc::td() << "ERROR" << cgicc::td() ;
		*out << cgicc::td() << "ERROR" << cgicc::td() ;
	      }
	    }

	    // CONNECTION PARAMETERS
	    if (connectionInformationRetreived) {
	      try {
		ConnectionVector vConnections ; ((ConnectionFactory *)deviceFactory_)->getFedChannelListByDcu(dcuIndexesHardId[index], vConnections) ;
	        keyType wrongConnection = 0 ;
		int ribbon = 0;
		int ribbonChannel = 0;
		for (ConnectionVector::iterator itConnection = vConnections.begin() ; itConnection != vConnections.end() ; itConnection ++) {
		  ConnectionDescription *fedChannelConnectionDescription = *itConnection ;
		  keyType indexConnection = buildCompleteKey(fedChannelConnectionDescription->getFecSlot(),fedChannelConnectionDescription->getRingSlot(),fedChannelConnectionDescription->getCcuAddress(),fedChannelConnectionDescription->getI2cChannel(),0) ;
		  if (index != indexConnection) {
		    *out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << fedChannelConnectionDescription->getFedId() << cgicc::td() ;
		    *out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << fedChannelConnectionDescription->getFedChannel() << cgicc::td() ;
		    wrongConnection = indexConnection ;
		  }
		  else {
		    ribbonChannel = 12 - (fedChannelConnectionDescription->getFedChannel() % 12);
		    ribbon = 8 - int(fedChannelConnectionDescription->getFedChannel() / 12.) ;
		    *out << cgicc::td() << fedChannelConnectionDescription->getFedId() << cgicc::td() ;
		    *out << cgicc::td() << ribbon << "/" << ribbonChannel << cgicc::td() ;
		  }
		}

		if (vConnections.size() < 3) {

		  bool colorFlag = false ;
		  if (apvNumber != -1) {
		    if ((unsigned int)apvNumber != (vConnections.size()*2)) colorFlag = true ;
		  }
		  else if (vConnections.size() < 2) colorFlag = true ;

		  for (int i = vConnections.size() ; i < 3 ; i ++) {

		    if (apvNumber != -1) {
		      if ( ((apvNumber == 4) && (i < 2)) || (apvNumber == 6) ) {
			*out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "No" << cgicc::td()
			     << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "No" << cgicc::td() ;
			errorConnection ++ ;
		      }
		      else {
			*out << cgicc::td() << "-" << cgicc::td() 
			     << cgicc::td() << "-" << cgicc::td() ;
		      }
		    }
		    else {
		      if (colorFlag) {
			*out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "No" << cgicc::td() 
			     << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "No" << cgicc::td() ;
			errorConnection ++ ;
		      }
		      else 
			*out << cgicc::td() << "No" << cgicc::td() << cgicc::td() << "No" << cgicc::td() ;
		    }
		  }
		}

		if (wrongConnection != 0) 
		  *out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "Bad connection: " << "FEC " << std::dec << getFecKey(wrongConnection) 
		       << " Ring " << getRingKey(wrongConnection) << " CCU 0x" << std::hex << getCcuKey(wrongConnection) 
		       << " channel 0x" << getChannelKey(wrongConnection) << cgicc::td() << std::dec ;
	      }
	      catch (FecExceptionHandler &e) {
		errorReportLogger_->errorReport ("Unable to retreive the connection information for DCU " + toString(dcuIndexesHardId[index]), e, LOGFATAL) ;
		*out << cgicc::td() << "No" << cgicc::td() << cgicc::td() << "No" << cgicc::td() ;
		*out << cgicc::td() << "No" << cgicc::td() << cgicc::td() << "No" << cgicc::td() ;
		*out << cgicc::td() << "No" << cgicc::td() << cgicc::td() << "No" << cgicc::td() ;
	      }
	      catch (oracle::occi::SQLException &e) {
		errorReportLogger_->errorReport ("Unable to retreive the connection information for DCU " + toString(dcuIndexesHardId[index]), e, LOGFATAL) ;
		*out << cgicc::td() << "ERROR" << cgicc::td() << cgicc::td() << "ERROR" << cgicc::td() ;
		*out << cgicc::td() << "ERROR" << cgicc::td() << cgicc::td() << "ERROR" << cgicc::td() ;
		*out << cgicc::td() << "ERROR" << cgicc::td() << cgicc::td() << "ERROR" << cgicc::td() ;
	      }
	    }

	    *out << cgicc::tr() << std::endl ;

	  }

	  *out << cgicc::table() << std::endl ;
	  if (connectionInformationRetreived && errorConnection > 0)
	    *out << cgicc::h2("Found " + toString(errorConnection) + " connection errors for partition " + partitionDbName_).set("style","color: rgb(255, 0, 0)") << std::endl ;

	  // Set it in the database if needed
	  *out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	       << "Please check one these boxes once you have performed all modifications for a Partition: " 
	       << "</big></big></span>" << std::endl ;
	  *out << cgicc::p() << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Create a major version: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CreateMajorVersion") << cgicc::td() << std::endl ;

	  *out << cgicc::tr() << std::endl ;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Create a minor version: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CreateMinorVersion") << cgicc::td() << std::endl ;

	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Disable/Re-Enable the modules selected: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","DisableModules") << cgicc::td() << std::endl ;

	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Reset the modifications already performed: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","ResetModifications").set("checked","true") << cgicc::td() << std::endl ;
	  *out << cgicc::tr() << std::endl;

	  *out << cgicc::tr() << std::endl;
	  //	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  //      *out << cgicc::td() << "<big>" << "Propagate the fiber delay in the FED: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","ApplyFiberDelay") << cgicc::td() << std::endl ;
	  //	  *out << cgicc::tr() << std::endl;

	  *out << cgicc::table() << std::endl ;
	}
	else {
	  *out << cgicc::h2(std::string("No DCU found for the partition ") + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	}
      }
      else {
	
	// Retreive the partition names
	std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNames() ;
	if (partitionNames.size()) {
	  
	  // Build a list of partition name
	  std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
	  for (std::list<std::string>::iterator itPartition = partitionNames.begin() ; itPartition != partitionNames.end() ; itPartition ++) {
	    if (partitionDbName_ == (*itPartition)) 
	      partitionButton << "<option selected>" << (*itPartition) << "</option>";
	    else
	      partitionButton << "<option>" << (*itPartition) << "</option>";
	  }
	  partitionButton << "</select>" << std::endl ;
	  
	  *out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	       << "Please select the partition then click Apply: " << "</big></big></span>"
	       << partitionButton.str() << std::endl ;
	}
	else *out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Unable to retreive the DCU from the database " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the DCU from the database", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Unable to retreive the DCU from the database (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the DCU from the database", e, LOGFATAL) ;
    }

    // Apply
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Apply");

    // End of the form
    *out << cgicc::form() << std::endl;
    
    // Few remarks
    *out << cgicc::h2("Few remarks") << std::endl ;
    *out << "<lu>" << std::endl ;
    if (partitionDbName_ != "") {
      *out << "<li>" << "By clicking on the module link (DCU hard id or position), you can display and change the FEC parameters" << "</li>" << std::endl ;
      *out << "<li>" << "Once you have performed all the changed needed on the parameters, you can Apply, do not click on it before all the operations has been done on all the modules you may want to do" << "</li>" << std::endl ;
      *out << "<li>" << "If the version were not use for a run, then the disable/enable is set on the current version" << "</li>" << std::endl ;
    }
    else {
      *out << "<li>" << "The list of the modules will be displayed after you click Apply" << "</li>" << std::endl ;
    }
    *out << "<li>" << "If you click again on the link " 
	 << "<a href=\"/%s/MoreParameters\">[More Parameters]</a>"
	 << ", you can choose a different partition" << "</li>" << std::endl ;
    *out << "<lu>" << std::endl ;
  }
  else {
    *out << cgicc::h2("Please specify a database to use these links").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;      
  }
  
  *out << cgicc::html() << std::endl ; 
}

#ifdef OLDVERSION
/** Display a web page to disable and enable a module
 */
void TkConfigurationDb::displayModuleManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 3 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Module Management");

  displayDatabaseRelatedLinks(in,out) ;

  if ((deviceFactory_ != NULL) && (deviceFactory_->getDbConnected())) {

    // Form and action
    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    url += "/ApplyModuleManagement";
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;

    try {

      if (partitionDbName_ != "") {

	if (dcuDevices_.size()) {

	  // ----------------- DCU Info
	  bool dcuInformationRetreived = false ;
	  try {
	    deviceFactory_->addDetIdPartition(partitionDbName_) ;
	    dcuInformationRetreived = true ;
	  }
	  catch (FecExceptionHandler &e) {
	    *out << cgicc::h2("Unable to retreive the DCU information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the DCU information", e, LOGFATAL) ;
	  }
	  catch (oracle::occi::SQLException &e) {
	    *out << cgicc::h2("Unable to retreive the DCU information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the DCU information", e, LOGFATAL) ;
	  }

	  // ----------------- Connection Factory
	  bool connectionInformationRetreived = false ;
	  unsigned int errorConnection = 0 ;
	  try {
	    deviceFactory_->createInputDBAccess();
	    deviceFactory_->setInputDBVersion(partitionDbName_);
	    deviceFactory_->getTrackerParser()->buildModuleList();
	    connectionInformationRetreived = true ;
	  }
	  catch (FecExceptionHandler &e) {
	    *out << cgicc::h2("Unable to retreive the connection information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the information", e, LOGFATAL) ;
	  }
	  catch (oracle::occi::SQLException &e) {
	    *out << cgicc::h2("Unable to retreive the connection information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the connection information", e, LOGFATAL) ;
	  }

	  // ----------------- Display 
	  std::list<keyType> dcuIndexes ;
	  Sgi::hash_map<keyType, unsigned int> dcuIndexesHardId ;
	  Sgi::hash_map<keyType, bool> dcuDisabled ;
	  unsigned int dcuFehCounter = 0 ;
	  for (deviceVector::iterator it = dcuDevices_.begin() ; it != dcuDevices_.end() ; it ++) {
	    dcuDescription *dcuD =  dynamic_cast<dcuDescription *>(*it) ;
	    //std::cout << dcuD->getDcuType() << "/" << DCUFEH << std::endl ;
	    if (dcuD->getDcuType() == DCUFEH) {
	      dcuFehCounter ++ ;
	      dcuIndexes.push_back(dcuD->getKey()) ;
	      dcuIndexesHardId[dcuD->getKey()] = dcuD->getDcuHardId() ;
	      dcuDisabled[dcuD->getKey()] = isModuleDisabled(dcuD) ;
	    }
	  }
	  dcuIndexes.sort() ;

	  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << std::endl ;
	  *out << "<th>" << "DCU Hard Id" << "</th>" ;
	  *out << "<th>" << "FEC Position" << "</th>" ;
	  *out << "<th>" << "Disabled" << "</th>" ;
	  if (dcuInformationRetreived) {
	    *out << "<th>" << "DET ID" << "</th>" ;
	    *out << "<th>" << "# APV" << "</th>" ;
	  }
	  if (connectionInformationRetreived) {
	    *out << "<th>" << "FED ID" << "</th>" ;
	    *out << "<th>" << "FED Channel" << "</th>" ;
	    *out << "<th>" << "FED ID" << "</th>" ;
	    *out << "<th>" << "FED Channel" << "</th>" ;
	    *out << "<th>" << "FED ID" << "</th>" ;
	    *out << "<th>" << "FED Channel" << "</th>" ;
	  }
	  *out << cgicc::tr() << std::endl ;

	  std::string url = getApplicationDescriptor()->getURN();
	  *out << cgicc::h2("Found " + toString(dcuFehCounter) + " DCUs for partition " + partitionDbName_) << std::endl ;
	  for (std::list<keyType>::iterator it = dcuIndexes.begin() ; it != dcuIndexes.end() ; it ++) {

	    keyType index = (*it) ;
	    std::stringstream dcuPosition ;
	    dcuPosition << "Module on FEC " << std::dec << getFecKey(index) << " Ring " << getRingKey(index) 
			<< " CCU 0x" << std::hex << getCcuKey(index) << " channel 0x" << getChannelKey(index) ;

	    *out << cgicc::tr() ;
	    *out << cgicc::td() << "<a href=\"/" << url 
		 << "/displayModuleParameters?param1=" << toString(dcuIndexesHardId[index]) << "&param2=" << toHexString(index)
		 << "\" target=\"_blank\">" << dcuIndexesHardId[index] << "</a>" << cgicc::td() ;
	    *out << cgicc::td() << "<a href=\"/" << url 
		 << "/displayModuleParameters?param1=" << toString(dcuIndexesHardId[index]) << "&param2=" << toHexString(index)
		 << "\" target=\"_blank\">" << dcuPosition.str() << cgicc::td() ;
	    if (dcuDisabled[index]) 
	      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","DCU" + toString(dcuIndexesHardId[index])).set("checked","true") << cgicc::td() ;
	    else
	      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","DCU" + toString(dcuIndexesHardId[index])) << cgicc::td() ;

	    // DCU INFORMATION
	    int apvNumber = -1 ;
	    if (dcuInformationRetreived) {
	      try {
		TkDcuInfo *tkDcuInfo = deviceFactory_->getTkDcuInfo (dcuIndexesHardId[index]) ;
		*out << cgicc::td() << "<a href=\"/" << url 
		     << "/displayModuleParameters?param1=" << toString(dcuIndexesHardId[index]) << "&param2=" << toHexString(index)
		     << "\" target=\"_blank\">" << tkDcuInfo->getDetId() << cgicc::td() ;
		*out << cgicc::td() << tkDcuInfo->getApvNumber() << cgicc::td() ;

		apvNumber = tkDcuInfo->getApvNumber() ;
	      }
	      catch (FecExceptionHandler &e) {
		errorReportLogger_->errorReport ("Unable to retreive the DCU information for DCU " + toString(dcuIndexesHardId[index]), e, LOGFATAL) ;
		*out << cgicc::td() << "No" << cgicc::td() ;
		*out << cgicc::td() << "No" << cgicc::td() ;
	      }
	      catch (oracle::occi::SQLException &e) {
		errorReportLogger_->errorReport ("Unable to retreive the DCU information for DCU " + toString(dcuIndexesHardId[index]), e, LOGFATAL) ;
		*out << cgicc::td() << "ERROR" << cgicc::td() ;
		*out << cgicc::td() << "ERROR" << cgicc::td() ;
	      }
	    }

	    // CONNECTION PARAMETERS
	    if (connectionInformationRetreived) {
	      try {
		std::vector<FedChannelConnectionDescription *> vConnections = deviceFactory_->getFedChannelListByDcu(dcuIndexesHardId[index]) ;
	        keyType wrongConnection = 0 ;
		for (std::vector<FedChannelConnectionDescription *>::iterator itConnection = vConnections.begin() ; itConnection != vConnections.end() ; itConnection ++) {
		  FedChannelConnectionDescription *fedChannelConnectionDescription = *itConnection ;
		  keyType indexConnection = buildCompleteKey(fedChannelConnectionDescription->getSlot(),fedChannelConnectionDescription->getRing(),fedChannelConnectionDescription->getCcu(),fedChannelConnectionDescription->getI2c(),0) ;
		  if (index != indexConnection) {
		    *out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << fedChannelConnectionDescription->getFedId() << cgicc::td() ;
		    *out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << fedChannelConnectionDescription->getFedChannel() << cgicc::td() ;
		    wrongConnection = indexConnection ;
		  }
		  else {
		    *out << cgicc::td() << fedChannelConnectionDescription->getFedId() << cgicc::td() ;
		    *out << cgicc::td() << fedChannelConnectionDescription->getFedChannel() << cgicc::td() ;
		  }
		}

		if (vConnections.size() < 3) {

		  bool colorFlag = false ;
		  if (apvNumber != -1) {
		    if ((unsigned int)apvNumber != (vConnections.size()*2)) colorFlag = true ;
		  }
		  else if (vConnections.size() < 2) colorFlag = true ;

		  for (int i = vConnections.size() ; i < 3 ; i ++) {

		    if (apvNumber != -1) {
		      if ( ((apvNumber == 4) && (i < 2)) || (apvNumber == 6) ) {
			*out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "No" << cgicc::td()
			     << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "No" << cgicc::td() ;
			errorConnection ++ ;
		      }
		      else {
			*out << cgicc::td() << "-" << cgicc::td() 
			     << cgicc::td() << "-" << cgicc::td() ;
		      }
		    }
		    else {
		      if (colorFlag) {
			*out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "No" << cgicc::td() 
			     << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "No" << cgicc::td() ;
			errorConnection ++ ;
		      }
		      else 
			*out << cgicc::td() << "No" << cgicc::td() << cgicc::td() << "No" << cgicc::td() ;
		    }
		  }
		}

		if (wrongConnection != 0) 
		  *out << cgicc::td().set("style","background-color: rgb(255, 0, 0)") << "Bad connection: " << "FEC " << std::dec << getFecKey(wrongConnection) 
		       << " Ring " << getRingKey(wrongConnection) << " CCU 0x" << std::hex << getCcuKey(wrongConnection) 
		       << " channel 0x" << getChannelKey(wrongConnection) << cgicc::td() << std::dec ;
	      }
	      catch (FecExceptionHandler &e) {
		errorReportLogger_->errorReport ("Unable to retreive the connection information for DCU " + toString(dcuIndexesHardId[index]), e, LOGFATAL) ;
		*out << cgicc::td() << "No" << cgicc::td() << cgicc::td() << "No" << cgicc::td() ;
		*out << cgicc::td() << "No" << cgicc::td() << cgicc::td() << "No" << cgicc::td() ;
		*out << cgicc::td() << "No" << cgicc::td() << cgicc::td() << "No" << cgicc::td() ;
	      }
	      catch (oracle::occi::SQLException &e) {
		errorReportLogger_->errorReport ("Unable to retreive the connection information for DCU " + toString(dcuIndexesHardId[index]), e, LOGFATAL) ;
		*out << cgicc::td() << "ERROR" << cgicc::td() << cgicc::td() << "ERROR" << cgicc::td() ;
		*out << cgicc::td() << "ERROR" << cgicc::td() << cgicc::td() << "ERROR" << cgicc::td() ;
		*out << cgicc::td() << "ERROR" << cgicc::td() << cgicc::td() << "ERROR" << cgicc::td() ;
	      }
	    }

	    *out << cgicc::tr() << std::endl ;

	  }

	  *out << cgicc::table() << std::endl ;
	  if (connectionInformationRetreived && errorConnection > 0)
	    *out << cgicc::h2("Found " + toString(errorConnection) + " connection errors for partition " + partitionDbName_).set("style","color: rgb(255, 0, 0)") << std::endl ;

	  // Set it in the database if needed
	  *out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	       << "Please check one these boxes once you have performed all modifications for a Partition: " 
	       << "</big></big></span>" << std::endl ;
	  *out << cgicc::p() << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Create a major version: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CreateMajorVersion") << cgicc::td() << std::endl ;

	  *out << cgicc::tr() << std::endl ;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Create a minor version: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CreateMinorVersion") << cgicc::td() << std::endl ;

	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Disable/Re-Enable the modules selected: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","DisableModules") << cgicc::td() << std::endl ;

	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Reset the modifications already performed: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","ResetModifications").set("checked","true") << cgicc::td() << std::endl ;
	  *out << cgicc::tr() << std::endl;

	  *out << cgicc::table() << std::endl ;
	}
	else {
	  *out << cgicc::h2(std::string("No DCU found for the partition ") + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	}
      }
      else {
	
	// Retreive the partition names
	std::list<std::string> *partitionNames = deviceFactory_->getAllPartitionNames() ;
	if ( (partitionNames != NULL) && (partitionNames->size()) ) {
	  
	  // Build a list of partition name
	  std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
	  for (std::list<std::string>::iterator itPartition = partitionNames->begin() ; itPartition != partitionNames->end() ; itPartition ++) {
	    if (partitionDbName_ == (*itPartition)) 
	      partitionButton << "<option selected>" << (*itPartition) << "</option>";
	    else
	      partitionButton << "<option>" << (*itPartition) << "</option>";
	  }
	  partitionButton << "</select>" << std::endl ;
	  
	delete partitionNames ;

	*out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please select the partition then click Apply: " << "</big></big></span>"
	     << partitionButton.str() << std::endl ;
	}
	else *out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Unable to retreive the DCU from the database " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the DCU from the database", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Unable to retreive the DCU from the database (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the DCU from the database", e, LOGFATAL) ;
    }

    // Apply
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Apply");

    // End of the form
    *out << cgicc::form() << std::endl;
    
    // Few remarks
    *out << cgicc::h2("Few remarks") << std::endl ;
    *out << "<lu>" << std::endl ;
    if (partitionDbName_ != "") {
      *out << "<li>" << "By clicking on the module link (DCU hard id or position), you can display and change the FEC parameters" << "</li>" << std::endl ;
      *out << "<li>" << "Once you have performed all the changed needed on the parameters, you can Apply, do not click on it before all the operations has been done on all the modules you may want to do" << "</li>" << std::endl ;
      *out << "<li>" << "If the version were not use for a run, then the disable/enable is set on the current version" << "</li>" << std::endl ;
    }
    else {
      *out << "<li>" << "The list of the modules will be displayed after you click Apply" << "</li>" << std::endl ;
    }
    *out << "<li>" << "If you click again on the link " 
	 << "<a href=\"/%s/MoreParameters\">[Database Parameters]</a>"
	 << ", you can choose a different partition" << "</li>" << std::endl ;
    *out << "<lu>" << std::endl ;
  }
  else {
    *out << cgicc::h2("Please specify a database to use these links").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;      
  }
  
  *out << cgicc::html() << std::endl ; 
}
#endif

/** Apply the disable or enable or one or several modules
 */
void TkConfigurationDb::applyModuleManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

//   *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
//   *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
//   *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
//   xgi::Utils::getPageHeader(*out, "Module Management");
//   displayDatabaseRelatedLinks(in,out) ;

  // Command to be applied

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Check if it is a partition name choose or it was already chosen
    if ( (partitionDbName_ == "") || (deviceValues_.size() == 0) || (dcuDevices_.size() == 0) ) {
      partitionDbName_ = cgi["PartitionName"]->getValue() ;
      downloadAllDevicesFromDb() ;
      displayModuleManagement(in,out) ;
    }
    else {
      *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
      *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
      *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
      xgi::Utils::getPageHeader(*out, "Module Management");
      
      displayDatabaseRelatedLinks(in,out) ;

      // Depending of the action: "actionToBeApply", "NoAction", "CreateMajorVersion", "CreateMinorVersion", "ResetModifications", "DisableModules"
      bool createMajorVersion = false ;
      if (cgi["actionToBeApply"]->getValue() == "CreateMajorVersion") createMajorVersion = true ;
      bool createMinorVersion = false ;
      if (cgi["actionToBeApply"]->getValue() == "CreateMinorVersion") createMinorVersion = true ;
      bool resetModifications = false ;
      if (cgi["actionToBeApply"]->getValue() == "ResetModifications") resetModifications = true ;
      bool disableModules = false ;
      if (cgi["actionToBeApply"]->getValue() == "DisableModules") disableModules = true ;
      //bool applyFiberDelay = false ;
      //if (cgi["actionToBeApply"]->getValue() == "ApplyFiberDelay") applyFiberDelay = true ;

      // -------------------------------------------------------------------------------------------- 
      // Create major or minor version
      if (createMajorVersion) {
	unsigned int major = 0, minor = 0 ;
	// Upload the major version
	deviceFactory_->setFecDeviceDescriptions (deviceValues_, partitionDbName_, &major, &minor, true) ;
	// Display the result
	std::stringstream msgInfo ; msgInfo << "Version " << std::dec << major << "." << minor << " created for partition " << partitionDbName_ ;
	*out << cgicc::h2(msgInfo.str()) ;
      }
      // -----------------------------------------------------------------------------------------------
      else if (createMinorVersion) {

	// Upload only the PLL, APV and AOH
	deviceVector apvDevices, pllDevices, aohDevices ;
	apvDevices = FecFactory::getDeviceFromDeviceVector(deviceValues_, APV25) ;
	pllDevices = FecFactory::getDeviceFromDeviceVector(deviceValues_, PLL) ;
	aohDevices = FecFactory::getDeviceFromDeviceVector(deviceValues_, LASERDRIVER) ;
	
	// Merge the devices
	for (deviceVector::iterator it = pllDevices.begin() ; it != pllDevices.end() ; it ++) {
	  apvDevices.push_back((*it)) ;
	}
	for (deviceVector::iterator it = aohDevices.begin() ; it != aohDevices.end() ; it ++) {
	  apvDevices.push_back((*it)) ;
	}
	
	if (apvDevices.size() > 0) {
	  unsigned int major = 0, minor = 0 ;
	  // Upload the major version
	  //deviceFactory_->setFecDeviceDescriptions (apvDevices, partitionDbName_, &major, &minor, false) ;
	  deviceFactory_->setFecDeviceDescriptions (deviceValues_, partitionDbName_, &major, &minor, false) ;
	  // Display the result
	  std::stringstream msgInfo ; msgInfo << "Version " << std::dec << major << "." << minor << " created for partition " << partitionDbName_ ;
	  *out << cgicc::h2(msgInfo.str()) ;
	}
	else {
	  *out << cgicc::h2("No change asked for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	}
      }
      // -----------------------------------------------------------------------------------------------
      else if (resetModifications) {
	
	downloadAllDevicesFromDb() ;
	*out << cgicc::h2("Reset performed") << std::endl ;
      }
      // -----------------------------------------------------------------------------------------------
      else if (disableModules) {

	// Check which DCU has been disabled in the interface
	// "DCU" + toString(dcuIndexesHardId[index]))
	std::vector<unsigned int> vModuleDisabled ;
	std::vector<unsigned int> vModuleEnabled  ;
	for (deviceVector::iterator dcuI = dcuDevices_.begin() ; dcuI != dcuDevices_.end() ; dcuI ++) {
	  dcuDescription *dcuD = dynamic_cast<dcuDescription *>(*dcuI) ;
	  if (dcuD->getDcuType() == DCUFEH) {
	    std::string name = "DCU" +  toString(dcuD->getDcuHardId()) ;
	    bool moduleDisabled = isModuleDisabled(dcuD) ;
	    if ( (cgi.queryCheckbox(name) && !moduleDisabled) ||
		 (!cgi.queryCheckbox(name) && moduleDisabled) ) {
	      if (cgi.queryCheckbox(name)) {
		vModuleDisabled.push_back(dcuD->getDcuHardId()) ;
		setModuleEnabled(dcuD,false) ;
	      }
	      else {
		vModuleEnabled.push_back(dcuD->getDcuHardId()) ;
		setModuleEnabled(dcuD,true) ;
	      }
	    }
	  }
	}

	// Disable or enable the modules
	if (vModuleDisabled.size() || vModuleEnabled.size()) {

 	  // ------------------------------------------------------------------------------------
 	  // Disable and enable the corresponding modules
	  // please note that this method create version for FED/FEC/Connections
	  // and reuse to enable some modules
 	  if (vModuleDisabled.size()) { 
 	    deviceFactory_->setEnableModules(partitionDbName_,vModuleDisabled, false) ;
 	  }
 	  if (vModuleEnabled.size()) {
 	    deviceFactory_->enableDevice(partitionDbName_,vModuleEnabled) ;
 	  }

// 	  // ------------ Check to state to see a version should be uploaded
// 	  // Version
// 	  unsigned int versionMajor = 0, versionMinor = 0 ;
// 	  unsigned int major = 0, minor = 0 ;
// 	  int createVersion = 1 ;
// 	  // A VERSION SHOULD BE ALWAYS uploaded even if it was never used
// 	  // FED: Upload a major version and disable the devices => a failure can occurs of the FED version is already used (if needed)
// 	  unsigned long fedDownloadTime = 0, fedUploadTime = 0, fecUploadTime = 0, startMillis, endMillis ;
// 	  if (createVersion) {
// 	    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	    std::vector<Fed9U::Fed9UDescription*> *fedVector = deviceFactory_->getFed9UDescriptions(partitionDbName_) ;
// 	    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	    fedDownloadTime = (endMillis - startMillis) ;
// 	    if (fedVector->size()) {
// 	      deviceFactory_->setUsingStrips(true);
// 	      startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	      deviceFactory_->setFed9UDescriptions(*fedVector,partitionDbName_,(unsigned short *)(&versionMajor),(unsigned short *)(&versionMinor),0); // minor version
// 	      endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	      fedUploadTime = (endMillis - startMillis) ;
// 	    }
// 	    // FEC: upload a minor version
// 	    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	    deviceFactory_->setFecDeviceDescriptions (deviceValues_, partitionDbName_, &major, &minor, false) ;
// 	    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	    fecUploadTime = (endMillis - startMillis) ;
// 	  }
// 	  // ------------------------------------------------------------------------------------
// 	  // Disable and enable the corresponding modules
// 	  if (vModuleDisabled.size()) {
// 	    deviceFactory_->disableDevice(partitionDbName_,vModuleDisabled) ;
// 	  }
// 	  if (vModuleEnabled.size()) {
// 	    deviceFactory_->enableDevice(partitionDbName_,vModuleEnabled) ;
// 	  }
// 	  // -------------------------------------------------------------------------------------
// 	  // Display
// 	  if (createVersion) {
// 	    std::stringstream msgInfo1 ; msgInfo1 << "Version created for partition " << partitionDbName_ ;
// 	    *out << cgicc::h2(msgInfo1.str()) ; 
// 	    *out << "<ul>" << std::endl ;
// 	    std::stringstream msgInfo2 ; msgInfo2 << "<li>" << "FEC version " << std::dec << major << "." << minor << " created in " << fecUploadTime << " ms" << std::endl ;
// 	    *out << cgicc::h3(msgInfo2.str()) ; 
// 	    std::stringstream msgInfo3 ; msgInfo3 << "<li>" << "FED Version " << versionMajor << "." << versionMinor << " created in " << fedUploadTime << " ms" << std::endl ;
// 	    *out << cgicc::h3(msgInfo3.str()) ; 
// 	    *out << "</ul>" << std::endl ;
// 	  }
// 	  else {
// 	    std::stringstream msgInfo1 ; msgInfo1 << "Version used for partition " << partitionDbName_ ;
// 	    *out << cgicc::h2(msgInfo1.str()) ; 
// 	    *out << "<ul>" << std::endl ;
// 	    std::stringstream msgInfo2 ; msgInfo2 << "<li>" << "FEC version " << std::dec << major << "." << minor << std::endl ;
// 	    *out << cgicc::h3(msgInfo2.str()) ; 
// 	    std::stringstream msgInfo3 ; msgInfo3 << "<li>" << "FED Version " << versionMajor << "." << versionMinor << std::endl ;
// 	    *out << cgicc::h3(msgInfo3.str()) ; 
// 	    *out << "</ul>" << std::endl ;
// 	  }

	  // Download all devices again
	  downloadAllDevicesFromDb() ;
	    
	  *out << cgicc::h2("DCU List of modules disabled/re-enalbed (versions for FEC/FED/Connections were created)") ;
	  std::stringstream msgInfo4 ; msgInfo4 << "<li>" << vModuleDisabled.size() << " DCUs list of modules to be disabled" ;
	  std::stringstream msgInfo5 ; msgInfo5 << "<li>" << vModuleEnabled.size() << " DCUs list of modules to be enabled" ;
	  *out << "<ul>" << std::endl ;
	  *out << cgicc::h3(msgInfo4.str()) ; 
	  *out << cgicc::h3(msgInfo5.str()) ; 
	  *out << "</ul>" << std::endl ;
	  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << std::endl ;
	  *out << "<th>" << "DCU Hard ID" << "</th>"; 
	  *out << "<th>" << "Status" << "</th>"; 
	  *out << cgicc::tr() << std::endl ;
	  for (std::vector<unsigned int>::iterator it = vModuleDisabled.begin() ; it != vModuleDisabled.end() ; it ++) {
	    *out << cgicc::tr() << cgicc::td() << std::endl ;
	    *out << (unsigned int)(*it) << std::endl ;
	    *out << cgicc::td() << cgicc::td() << std::endl ;
	    *out << "DISABLED" << std::endl ;
	    *out << cgicc::td() << cgicc::tr() << std::endl ;
	  }
	  for (std::vector<unsigned int>::iterator it = vModuleEnabled.begin() ; it != vModuleEnabled.end() ; it ++) {
	    *out << cgicc::tr() << cgicc::td() << std::endl ;
	    *out << (unsigned int)(*it) << std::endl ;
	    *out << cgicc::td() << cgicc::td() << std::endl ;
	    *out << "ENABLED" << std::endl ;
	    *out << cgicc::td() << cgicc::tr() << std::endl ;
	  }
	  *out << cgicc::table() << std::endl ;
	}
	else 
	  *out << cgicc::h2("No module has been specified to be enabled or disabled").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl ;
      }
//       // -----------------------------------------------------------------------------------
//       // Apply the fiber delay from TkDcuInfo table to the FED latency
//       else if (applyFiberDelay) {
// 	unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	deviceFactory_->updateChannelDelays(partitionDbName_) ;
// 	unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
// 	unsigned long updateChannelTime = (startMillis - endMillis) ;
// 	*out << cgicc::h2("Update of the fiber delay in FED delay was done correctly in " + toString(updateChannelTime) + " for the partition " + partitionDbName_) << std::endl ;
//       }

	//*out << cgicc::h2("This functionnality is not yet implemented").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl ;
      else {
	*out << cgicc::h2("No change has been asked").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl ;
      }
      
      *out << cgicc::html() << std::endl ;       
    }
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2(e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Problem in accessing the modules in database", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2(e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Problem in accessing the modules in database", e, LOGFATAL) ;
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
  //*out << cgicc::html() << std::endl;
}

// ------------------------------------------------------------------------------------------------------------------- //
// FEC parameter modification for one module
// ------------------------------------------------------------------------------------------------------------------- //

/** Display some FEC parameters for one given module
 * Web param1 is the DCU hard id in dec
 * Web param2 is the DCU index in hex
 */
void TkConfigurationDb::displayModuleParameters (xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {

  linkDbPosition_ = 3 ;

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);
    unsigned int dcuHardId = fromString<unsigned int>(cgi["param1"]->getValue()) ;
    keyType index = fromHexString<unsigned int>(cgi["param2"]->getValue()) ;

    displayModuleParameters(in,out,dcuHardId,index) ;
  }
  catch (const std::exception& e) {
    *out << cgicc::h2("Error CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

/** Display the FEC parameters for a given module pass by argument
 * \param dcuHardId - DCU hardware ID
 * \param index - index of the module
 */
void TkConfigurationDb::displayModuleParameters (xgi::Input * in, xgi::Output * out, unsigned int dcuHardId, keyType index ) throw (xgi::exception::Exception) {

  linkDbPosition_ = 3 ;

  // ----------------- DCU Info
  unsigned int apvNumber = 0 ;
  try {
    deviceFactory_->addDetIdPartition(partitionDbName_) ;
   TkDcuInfo *tkDcuInfo = deviceFactory_->getTkDcuInfo (dcuHardId) ;
    apvNumber = tkDcuInfo->getApvNumber() ;
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2("Unable to retreive the DCU information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the DCU information", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2("Unable to retreive the DCU information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the DCU information", e, LOGFATAL) ;
  }
  
  std::stringstream msgInfo ;
  msgInfo << "Module (" << dcuHardId 
	  << ") on FEC " << std::dec << getFecKey(index) 
	  << " ring " << getRingKey(index) 
	  << " CCU 0x" << std::hex << getCcuKey(index)
	  << " Channel 0x" << getChannelKey(index)
	  << std::endl ;

  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title(msgInfo.str()) << std::endl;
  xgi::Utils::getPageHeader(*out, msgInfo.str());

  // Empty the list devices to be changed
  if (devicesToBeChanged_.size() > 0) devicesToBeChanged_.clear() ;

  // Found the APV, PLL and AOH on this module
  apvDescription *apvD20       = NULL ;
  apvDescription *apvD21       = NULL ;
  apvDescription *apvD22       = NULL ;
  apvDescription *apvD23       = NULL ;
  apvDescription *apvD24       = NULL ;
  apvDescription *apvD25       = NULL ;
  apvDescription *apvD         = NULL ; // one of the APV existing
  pllDescription *pllD         = NULL ;
  laserdriverDescription *aohD = NULL ;
  dcuDescription *dcuD         = NULL ;
  bool found = false ;
  for (deviceVector::iterator itDevice = dcuDevices_.begin() ; (itDevice != dcuDevices_.end()) && !found; itDevice ++) {
    if (getFecRingCcuChannelKey((*itDevice)->getKey()) == getFecRingCcuChannelKey(index)) {
      dcuD = dynamic_cast<dcuDescription *>(*itDevice) ;
      found = true ;
    }
  }

  for (deviceVector::iterator itDevice = deviceValues_.begin() ; itDevice != deviceValues_.end() ; itDevice ++) {

    //       char msg[80] ; decodeKey(msg,getFecRingCcuChannelKey((*itDevice)->getKey())) ;
    //       char msg1[80] ; decodeKey(msg1,getFecRingCcuChannelKey(index)) ; 
    //       std::cout << msg << std::endl << msg1 << std::endl ; 

    if (getFecRingCcuChannelKey((*itDevice)->getKey()) == getFecRingCcuChannelKey(index)) {
      switch (getAddressKey((*itDevice)->getKey())) {
      case 0x20: 
	apvD20 = dynamic_cast<apvDescription *>(*itDevice) ; 
	devicesToBeChanged_[(*itDevice)->getKey()] = (*itDevice) ;
	break ;
      case 0x21: 
	apvD21 = dynamic_cast<apvDescription *>(*itDevice) ;
	devicesToBeChanged_[(*itDevice)->getKey()] = (*itDevice) ; 
	break ;
      case 0x22: 
	apvD22 = dynamic_cast<apvDescription *>(*itDevice) ;
	devicesToBeChanged_[(*itDevice)->getKey()] = (*itDevice) ; 
	break ;
      case 0x23: 
	apvD23 = dynamic_cast<apvDescription *>(*itDevice) ;
	devicesToBeChanged_[(*itDevice)->getKey()] = (*itDevice) ; 
	break ;
      case 0x24: 
	apvD24 = dynamic_cast<apvDescription *>(*itDevice) ;
	devicesToBeChanged_[(*itDevice)->getKey()] = (*itDevice) ; 
	break ;
      case 0x25: 
	apvD25 = dynamic_cast<apvDescription *>(*itDevice) ;
	devicesToBeChanged_[(*itDevice)->getKey()] = (*itDevice) ; 
	break ;
      case 0x44: 
	pllD   = dynamic_cast<pllDescription *>(*itDevice) ;
	devicesToBeChanged_[(*itDevice)->getKey()] = (*itDevice) ; 
	break ;
      case 0x60: 
	aohD   = dynamic_cast<laserdriverDescription *>(*itDevice) ; 
	devicesToBeChanged_[(*itDevice)->getKey()] = (*itDevice) ;
	break ;
      default: break ;
      }
    }
  }
      
  // Choose one the APV for display
  if (apvD20 != NULL) apvD = apvD20 ;
  else if (apvD21 != NULL) apvD = apvD21 ;
  else if (apvD22 != NULL) apvD = apvD22 ;
  else if (apvD23 != NULL) apvD = apvD23 ;
  else if (apvD24 != NULL) apvD = apvD24 ;
  else if (apvD25 != NULL) apvD = apvD25 ;

  // Form and action
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/ApplyModuleParameters";
  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  // Just to have the DCU hard ID
  *out << cgicc::p() << "DCU Hardware ID: " << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","dcuHardId").set("value",toString(dcuHardId)) << std::endl ;

  // Table of devices
  *out << cgicc::p() << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << cgicc::tr() << std::endl ;
  *out << "<th>" << "Parameter Name" << "</th>" ;
  //*out << "<th>" << "Operation" << "</th>" ;
  *out << "<th>" << "Original Value" << "</th>" ;
  *out << "<th>" << "Value to be Set" << "</th>" ;
  *out << cgicc::tr() << std::endl ;

  if (apvD != NULL) {
    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << apvDescription::APVPARAMETERNAMES[apvDescription::APVMODE] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","apvModeOrig").set("value",toString((int)apvD->getApvMode()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","apvModeSet").set("value",toString((int)apvD->getApvMode()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << apvDescription::APVPARAMETERNAMES[apvDescription::APVLATENCY] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","apvLatencyOrig").set("value",toString((int)apvD->getLatency()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","apvLatencySet").set("value",toString((int)apvD->getLatency()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << apvDescription::APVPARAMETERNAMES[apvDescription::MUXGAIN] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","apvMuxGainOrig").set("value",toString((int)apvD->getMuxGain()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","apvMuxGainSet").set("value",toString((int)apvD->getMuxGain()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << apvDescription::APVPARAMETERNAMES[apvDescription::ISHA] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","apvIshaOrig").set("value",toString((int)apvD->getIsha()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","apvIshaSet").set("value",toString((int)apvD->getIsha()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << apvDescription::APVPARAMETERNAMES[apvDescription::VFS] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","apvVfsOrig").set("value",toString((int)apvD->getVfs()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","apvVfsSet").set("value",toString((int)apvD->getVfs()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << apvDescription::APVPARAMETERNAMES[apvDescription::VPSP] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","apvVpspOrig").set("value",toString((int)apvD->getVpsp()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","apvVpspSet").set("value",toString((int)apvD->getVpsp()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 
  }
  else {
    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << "No APV found" << cgicc::td() << std::endl ;
    *out << cgicc::td() << "No APV found" << cgicc::td() << std::endl ;
    *out << cgicc::td() << "No APV found" << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 
  }

  if (pllD != NULL) {
    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYFINE] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","pllDelayFineOrig").set("value",toString((int)pllD->getDelayFine()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","pllDelayFineSet").set("value",toString((int)pllD->getDelayFine()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYCOARSE] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","pllDelayCoarseOrig").set("value",toString((int)pllD->getDelayCoarse()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","pllDelayCoarseSet").set("value",toString((int)pllD->getDelayCoarse()))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

  }
  else {
    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << "No PLL found" << cgicc::td() << std::endl ;
    *out << cgicc::td() << "No PLL found" << cgicc::td() << std::endl ;
    *out << cgicc::td() << "No PLL found" << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 
  }

  if (aohD != NULL) {

    tscType8 bias[3] ; aohD->getBias(bias) ;

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS0] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","aohBias0Orig").set("value",toString((int)bias[0]))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","aohBias0Set").set("value",toString((int)bias[0]))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS1] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","aohBias1Orig").set("value",toString((int)bias[1]))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","aohBias1Set").set("value",toString((int)bias[1]))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS2] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","aohBias2Orig").set("value",toString((int)bias[2]))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","aohBias2Set").set("value",toString((int)bias[2]))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN0] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","aohGain0Orig").set("value",toString((int)aohD->getGain(0)))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","aohGain0Set").set("value",toString((int)aohD->getGain(0)))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl;

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN1] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","aohGain1Orig").set("value",toString((int)aohD->getGain(1)))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","aohGain1Set").set("value",toString((int)aohD->getGain(1)))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl;

    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN2] << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","aohGain2Orig").set("value",toString((int)aohD->getGain(2)))
	 << cgicc::td() << std::endl ;
    *out << cgicc::td() 
	 << cgicc::input().set("type","text").set("size","10").set("name","aohGain2Set").set("value",toString((int)aohD->getGain(2)))
	 << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl;
  }    
  else {
    *out << cgicc::tr() << std::endl; 
    *out << cgicc::td() << "No AOH found" << cgicc::td() << std::endl ;
    *out << cgicc::td() << "No AOH found" << cgicc::td() << std::endl ;
    *out << cgicc::td() << "No AOH found" << cgicc::td() << std::endl ;
    *out << cgicc::tr() << std::endl; 
  }
  *out << cgicc::table() << std::endl ;

  // Display the devices enabled/disable
  *out << cgicc::p() << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << cgicc::tr() << std::endl ;
  *out << "<th>" << "Device" << "</th>" ;
  *out << "<th>" << "Disabled" << "</th>" ;
  *out << cgicc::tr() << std::endl ;
  *out << cgicc::tr() << cgicc::td() << "APV 0x20" <<  cgicc::td() << std::endl ;
  if (apvD20 != NULL) {
    if (apvD20->getEnabled()) 
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV20_DISABLE").set("readonly", "readonly") << cgicc::td() ;
    else
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV20_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
  }
  else *out << cgicc::td() << "ERROR" << cgicc::td() ;
  *out << cgicc::tr() << std::endl; 
  *out << cgicc::tr() << cgicc::td() << "APV 0x21" <<  cgicc::td() << std::endl ;
  if (apvD21 != NULL) {
    if (apvD21->getEnabled()) 
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV21_DISABLE").set("readonly", "readonly") << cgicc::td() ;
    else
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV21_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
  }
  else *out << cgicc::td() << "ERROR" << cgicc::td() ;
  *out << cgicc::tr() << std::endl; 
  if ( (apvNumber == 6) || (apvD22 != NULL) || (apvD23 != NULL) ) {
    *out << cgicc::tr() << cgicc::td() << "APV 0x22" <<  cgicc::td() << std::endl ;
    if (apvD22 != NULL) {
      if (apvD22->getEnabled()) 
	*out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV22_DISABLE").set("readonly", "readonly") << cgicc::td() ;
      else
	*out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV22_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
    }
    else *out << cgicc::td() << "ERROR" << cgicc::td() ;
    *out << cgicc::tr() << std::endl; 
    *out << cgicc::tr() << cgicc::td() << "APV 0x23" <<  cgicc::td() << std::endl ;
    if (apvD23 != NULL) {
      if (apvD23->getEnabled()) 
	*out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV23_DISABLE").set("readonly", "readonly") << cgicc::td() ;
      else
	*out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV23_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
    }
    else *out << cgicc::td() << "ERROR" << cgicc::td() ;
    *out << cgicc::tr() << std::endl; 
  }
  *out << cgicc::tr() << cgicc::td() << "APV 0x24" <<  cgicc::td() << std::endl ;
  if (apvD24 != NULL) {
    if (apvD24->getEnabled()) 
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV24_DISABLE").set("readonly", "readonly") << cgicc::td() ;
    else
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV24_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
  }
  else *out << cgicc::td() << "ERROR" << cgicc::td() ;
  *out << cgicc::tr() << std::endl; 
  *out << cgicc::tr() << cgicc::td() << "APV 0x25" <<  cgicc::td() << std::endl ;
  if (apvD25 != NULL) {
    if (apvD25->getEnabled()) 
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV25_DISABLE").set("readonly", "readonly") << cgicc::td() ;
    else
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","APV25_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
  }
  else *out << cgicc::td() << "ERROR" << cgicc::td() ;
  *out << cgicc::tr() << std::endl; 
  *out << cgicc::tr() << cgicc::td() << "PLL" <<  cgicc::td() << std::endl ;
  if (pllD != NULL) {
    if (pllD->getEnabled()) 
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","PLL_DISABLE").set("readonly", "readonly") << cgicc::td() ;
    else
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","PLL_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
  }
  else *out << cgicc::td() << "ERROR" << cgicc::td() ;
  *out << cgicc::tr() << std::endl; 
  *out << cgicc::tr() << cgicc::td() << "AOH" <<  cgicc::td() << std::endl ;
  if (aohD != NULL) {
    if (aohD->getEnabled()) 
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","AOH_DISABLE").set("readonly", "readonly") << cgicc::td() ;
    else
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","AOH_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
  }
  else *out << cgicc::td() << "ERROR" << cgicc::td() ;
  *out << cgicc::tr() << std::endl; 
  *out << cgicc::tr() << cgicc::td() << "DCU" <<  cgicc::td() << std::endl ;
  if (dcuD != NULL) {
    if (dcuD->getEnabled()) 
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","DCU_DISABLE").set("readonly", "readonly") << cgicc::td() ;
    else
      *out << cgicc::td() << cgicc::input().set("type", "checkbox").set("name","DCU_DISABLE").set("checked","true").set("readonly", "readonly") << cgicc::td() ;
  }
  else *out << cgicc::td() << "ERROR" << cgicc::td() ;
  *out << cgicc::tr() << std::endl; 
  *out << cgicc::table() << std::endl ;

  // Apply
  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply");

  // End of the form
  *out << cgicc::form() << std::endl;

  // Few remarks
  *out << cgicc::h2("Few remarks") << std::endl ;
  *out << "<lu>" << std::endl ;
  *out << "<li>" << "The APV modifications will be applied on all APVs from this module" << "</li>" << std::endl ;
  *out << "<li>" << "Please make all the changes and go back to the main web page to apply the modifications" << "</li>" << std::endl ;
  *out << "<li>" << "The disable / enable is not yet implemented" << std::endl ;
  *out << "</lu>" << std::endl ;
    
  // - End of HTML
  *out << "</HTML>" << std::endl ;
}

/** Apply the disable or enable or one or several modules
 */
void TkConfigurationDb::applyModuleParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  try {

    // Retreive the index of the module
    if (devicesToBeChanged_.size() > 0) {

      // Is a parameter has been changed
      bool parameterChanged = false ;

      Sgi::hash_map<keyType,deviceDescription *>::iterator itFirst = devicesToBeChanged_.begin() ;
      keyType index = getFecRingCcuChannelKey(itFirst->first) ;

      // Create a new Cgicc object containing all the CGI data
      cgicc::Cgicc cgi(in);

      // DCU Hard ID
      unsigned int dcuHardId = fromString<unsigned int>(cgi["dcuHardId"]->getValue()) ;

      // Retreive all devices
      apvDescription *apvD20       = dynamic_cast<apvDescription *>(devicesToBeChanged_[index | setAddressKey(0x20)]) ;
      apvDescription *apvD21       = dynamic_cast<apvDescription *>(devicesToBeChanged_[index | setAddressKey(0x21)]) ;
      apvDescription *apvD22       = dynamic_cast<apvDescription *>(devicesToBeChanged_[index | setAddressKey(0x22)]) ;
      apvDescription *apvD23       = dynamic_cast<apvDescription *>(devicesToBeChanged_[index | setAddressKey(0x23)]) ;
      apvDescription *apvD24       = dynamic_cast<apvDescription *>(devicesToBeChanged_[index | setAddressKey(0x24)]) ;
      apvDescription *apvD25       = dynamic_cast<apvDescription *>(devicesToBeChanged_[index | setAddressKey(0x25)]) ;
      pllDescription *pllD         = dynamic_cast<pllDescription *>(devicesToBeChanged_[index | setAddressKey(0x44)]) ;
      laserdriverDescription *aohD = dynamic_cast<laserdriverDescription *>(devicesToBeChanged_[index | setAddressKey(0x60)]) ;

      // Get all values for the corresponding module
      int apvModeSet = -1, apvLatencySet = -1, apvMuxGainSet = -1, apvIshaSet = -1, apvVfsSet = -1, apvVpspSet = -1 ;
      if ( (apvD20 != NULL) || (apvD21 != NULL) ||
	   (apvD22 != NULL) || (apvD23 != NULL) ||
	   (apvD24 != NULL) || (apvD25 != NULL) ) {
	
	apvModeSet = fromString<int>(cgi["apvModeSet"]->getValue()) ;
	apvLatencySet = fromString<int>(cgi["apvLatencySet"]->getValue()) ;
	apvMuxGainSet = fromString<int>(cgi["apvMuxGainSet"]->getValue()) ;
	apvIshaSet = fromString<int>(cgi["apvIshaSet"]->getValue()) ;
	apvVfsSet = fromString<int>(cgi["apvVfsSet"]->getValue()) ;
	apvVpspSet = fromString<int>(cgi["apvVpspSet"]->getValue()) ;
      }
      
      int pllDelayFineSet = -1, pllDelayCoarseSet = -1 ;
      if (pllD != NULL) {
	pllDelayFineSet = fromString<int>(cgi["pllDelayFineSet"]->getValue()) ;
	pllDelayCoarseSet = fromString<int>(cgi["pllDelayCoarseSet"]->getValue()) ;
      }
      
      int aohBias0Set = -1, aohBias1Set = -1, aohBias2Set = -1 ;
      int aohGain0Set = -1, aohGain1Set = -1, aohGain2Set = -1 ;
      if (aohD != NULL) {
	aohBias0Set = fromString<int>(cgi["aohBias0Set"]->getValue()) ;
	aohBias1Set = fromString<int>(cgi["aohBias1Set"]->getValue()) ;
	aohBias2Set = fromString<int>(cgi["aohBias2Set"]->getValue()) ;
	
	aohGain0Set = fromString<int>(cgi["aohGain0Set"]->getValue()) ;
	aohGain1Set = fromString<int>(cgi["aohGain1Set"]->getValue()) ;
	aohGain2Set = fromString<int>(cgi["aohGain2Set"]->getValue()) ;
      }

      // Apply the change
      if (apvD20 != NULL) {
	if (apvModeSet != -1) { apvD20->setApvMode((tscType8)apvModeSet) ; parameterChanged = true ; }
	if (apvLatencySet != -1) { apvD20->setLatency((tscType8)apvLatencySet) ; parameterChanged = true ; }
	if (apvMuxGainSet != -1) { apvD20->setMuxGain((tscType8)apvMuxGainSet) ; parameterChanged = true ; }
	if (apvIshaSet != -1) { apvD20->setIsha((tscType8)apvIshaSet) ; parameterChanged = true ; }
	if (apvVfsSet != -1) { apvD20->setVfs((tscType8)apvVfsSet) ; parameterChanged = true ; }
	if (apvVpspSet != -1) { apvD20->setVpsp((tscType8)apvVpspSet) ; parameterChanged = true ; }
      }
      if (apvD21 != NULL) {
	if (apvModeSet != -1) { apvD21->setApvMode((tscType8)apvModeSet) ; parameterChanged = true ; }
	if (apvLatencySet != -1) { apvD21->setLatency((tscType8)apvLatencySet) ; parameterChanged = true ; }
	if (apvMuxGainSet != -1) { apvD21->setMuxGain((tscType8)apvMuxGainSet) ; parameterChanged = true ; }
	if (apvIshaSet != -1) { apvD21->setIsha((tscType8)apvIshaSet) ; parameterChanged = true ; }
	if (apvVfsSet != -1) { apvD21->setVfs((tscType8)apvVfsSet) ; parameterChanged = true ; }
	if (apvVpspSet != -1) { apvD21->setVpsp((tscType8)apvVpspSet) ; parameterChanged = true ; }
      }
      if (apvD22 != NULL) {
	if (apvModeSet != -1) { apvD22->setApvMode((tscType8)apvModeSet) ; parameterChanged = true ; }
	if (apvLatencySet != -1) { apvD22->setLatency((tscType8)apvLatencySet) ; parameterChanged = true ; }
	if (apvMuxGainSet != -1) { apvD22->setMuxGain((tscType8)apvMuxGainSet) ; parameterChanged = true ; }
	if (apvIshaSet != -1) { apvD22->setIsha((tscType8)apvIshaSet) ; parameterChanged = true ; }
	if (apvVfsSet != -1) { apvD22->setVfs((tscType8)apvVfsSet) ; parameterChanged = true ; }
	if (apvVpspSet != -1) { apvD22->setVpsp((tscType8)apvVpspSet) ; parameterChanged = true ; }
      }
      if (apvD23 != NULL) {
	if (apvModeSet != -1) { apvD23->setApvMode((tscType8)apvModeSet) ; parameterChanged = true ; }
	if (apvLatencySet != -1) { apvD23->setLatency((tscType8)apvLatencySet) ; parameterChanged = true ; }
	if (apvMuxGainSet != -1) { apvD23->setMuxGain((tscType8)apvMuxGainSet) ; parameterChanged = true ; }
	if (apvIshaSet != -1) { apvD23->setIsha((tscType8)apvIshaSet) ; parameterChanged = true ; }
	if (apvVfsSet != -1) { apvD23->setVfs((tscType8)apvVfsSet) ; parameterChanged = true ; }
	if (apvVpspSet != -1) { apvD23->setVpsp((tscType8)apvVpspSet) ; parameterChanged = true ; }
      }
      if (apvD24 != NULL) {
	if (apvModeSet != -1) { apvD24->setApvMode((tscType8)apvModeSet) ; parameterChanged = true ; }
	if (apvLatencySet != -1) { apvD24->setLatency((tscType8)apvLatencySet) ; parameterChanged = true ; }
	if (apvMuxGainSet != -1) { apvD24->setMuxGain((tscType8)apvMuxGainSet) ; parameterChanged = true ; }
	if (apvIshaSet != -1) { apvD24->setIsha((tscType8)apvIshaSet) ; parameterChanged = true ; }
	if (apvVfsSet != -1) { apvD24->setVfs((tscType8)apvVfsSet) ; parameterChanged = true ; }
	if (apvVpspSet != -1) { apvD24->setVpsp((tscType8)apvVpspSet) ; parameterChanged = true ; }
      }
      if (apvD25 != NULL) {
	if (apvModeSet != -1) { apvD25->setApvMode((tscType8)apvModeSet) ; parameterChanged = true ; }
	if (apvLatencySet != -1) { apvD25->setLatency((tscType8)apvLatencySet) ; parameterChanged = true ; }
	if (apvMuxGainSet != -1) { apvD25->setMuxGain((tscType8)apvMuxGainSet) ; parameterChanged = true ; }
	if (apvIshaSet != -1) { apvD25->setIsha((tscType8)apvIshaSet) ; parameterChanged = true ; }
	if (apvVfsSet != -1) { apvD25->setVfs((tscType8)apvVfsSet) ; parameterChanged = true ; }
	if (apvVpspSet != -1) { apvD25->setVpsp((tscType8)apvVpspSet) ; parameterChanged = true ; }
      }
      if (pllD != NULL) {
	if (pllDelayFineSet != -1) { pllD->setDelayFine((tscType8)pllDelayFineSet) ; parameterChanged = true ; }
	if (pllDelayCoarseSet != -1) { pllD->setDelayCoarse((tscType8)pllDelayCoarseSet) ; parameterChanged = true ; }
      }
      if (aohD != NULL) {
	if (aohBias0Set != -1) { aohD->setBias(0,aohBias0Set) ; parameterChanged = true ; }
	if (aohBias1Set != -1) { aohD->setBias(1,aohBias1Set) ; parameterChanged = true ; }
	if (aohBias2Set != -1) { aohD->setBias(2,aohBias2Set) ; parameterChanged = true ; }
	if (aohGain0Set != -1) { aohD->setGain(0,aohGain0Set) ; parameterChanged = true ; }
	if (aohGain1Set != -1) { aohD->setGain(1,aohGain1Set) ; parameterChanged = true ; }
	if (aohGain2Set != -1) { aohD->setGain(2,aohGain2Set) ; parameterChanged = true ; }
      }
      
      if (parameterChanged)
	this->displayModuleParameters (in,out,dcuHardId,index) ;
      else {
	*out << "<HTML>" << std::endl ;
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
	*out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
	xgi::Utils::getPageHeader(*out, "Strip Tracker Configuration Database Interface");
	*out << cgicc::h2("No change has been applied").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
	*out << "</HTML>" << std::endl ; 
      }
    }
    else {
      *out << "<HTML>" << std::endl ;
      *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
      *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
      *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
      xgi::Utils::getPageHeader(*out, "Strip Tracker Configuration Database Interface");
      *out << cgicc::h2("No device found for this module").set("style","font-size: 15pt;  font-family: arial; font-weight: bold;") << std::endl;
      *out << "</HTML>" << std::endl ; 
    }
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Error CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

/** Display the connections inside the database
 */
void TkConfigurationDb::displayFECFEDConnections (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 4 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Connections Display");

  displayDatabaseRelatedLinks(in,out) ;

  try {

    if (partitionDbName_ != "") {

      ConnectionVector connections ;
      deviceFactory_->getConnectionDescriptions (partitionDbName_,connections,0,0,true) ;

      unsigned int connectionNumber = connections.size() ;
      if (connectionNumber == 0) {
	*out << cgicc::h2("No connections found in the database for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      else {
	*out << cgicc::h2("Find " + toString(connectionNumber) + " connections in the database for partition " + partitionDbName_) << std::endl ;
	*out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << "<th>" << "FED Id" << "</th>" << std::endl;
	*out << "<th>" << "FED Channel" << "</th>" << std::endl;
	*out << "<th>" << "FEC Slot" << "</th>" << std::endl;
	*out << "<th>" << "Ring Slot" << "</th>" << std::endl;
	*out << "<th>" << "CCU Address" << "</th>" << std::endl;
	*out << "<th>" << "I2C Channel" << "</th>" << std::endl;
	*out << "<th>" << "APV" << "</th>" << std::endl;
	*out << cgicc::tr() << std::endl;
	for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++) {
	  ConnectionDescription* fedChannelConnectionDescription = *it ;
	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::td() << std::dec << fedChannelConnectionDescription->getFedId() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::dec << fedChannelConnectionDescription->getFedChannel() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::dec << fedChannelConnectionDescription->getFecSlot() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::dec << fedChannelConnectionDescription->getRingSlot() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::hex << "0x" << fedChannelConnectionDescription->getCcuAddress() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::hex << "0x" << fedChannelConnectionDescription->getI2cChannel() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::hex << "0x" << fedChannelConnectionDescription->getApvAddress() << cgicc::td() << std::endl ;
	  *out << cgicc::tr() << std::endl;
	}
	*out << cgicc::table() << std::endl ;
      }   
    }
    else {

      // Retreive the partition names
      std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNames() ;
      if (partitionNames.size()) {
	
	// Form and action
	std::string url = "/";
	url += getApplicationDescriptor()->getURN();
	url += "/ApplyFECFEDConnections";
	*out << cgicc::form().set("method","post")
	  .set("action", url)
	  .set("enctype","multipart/form-data") << std::endl;
	
	// Build a list of partition name
	std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
	for (std::list<std::string>::iterator itPartition = partitionNames.begin() ; itPartition != partitionNames.end() ; itPartition ++) {
	  if (partitionDbName_ == (*itPartition)) 
	    partitionButton << "<option selected>" << (*itPartition) << "</option>";
	  else
	    partitionButton << "<option>" << (*itPartition) << "</option>";
	}
	partitionButton << "</select>" << std::endl ;
	
	*out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please select the partition then click Apply: " << "</big></big></span>"
	     << partitionButton.str() << std::endl ;
	
	// Apply
	*out << cgicc::p() << cgicc::input().set("type", "submit")
	  .set("name", "submit")
	  .set("value", "Apply");
	
	// End of the form
	*out << cgicc::form() << std::endl;

	// Few remarks
	*out << cgicc::h2("Few remarks") << std::endl ;
	*out << "<lu>" << std::endl ;
	if (partitionDbName_ == "") {
	  *out << "<li>" << "The list of the connections will be displayed after you click Apply" << "</li>" << std::endl ;
	  *out << "<li>" << "If you click again on the link " 
	       << "<a href=\"/%s/MoreParameters\">[More Parameters]</a>"
	       << ", you can choose a different partition" << "</li>" << std::endl ;
	  *out << "<lu>" << std::endl ;
	}
      }
      else *out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    }
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2("Unable to retreive the connection information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the  information", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2("Unable to retreive the connection information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the connection information", e, LOGFATAL) ;
  }

  *out << cgicc::html() << std::endl;
}

#ifdef OLDVERSION
/** Display the connections inside the database
 */
void TkConfigurationDb::displayFECFEDConnections (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 4 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Connections Display");

  displayDatabaseRelatedLinks(in,out) ;

  try {

    if (partitionDbName_ != "") {

      deviceFactory_->createInputDBAccess();
      deviceFactory_->setInputDBVersion(partitionDbName_);
      deviceFactory_->getTrackerParser()->buildModuleList();

      int connectionNumber = deviceFactory_->getNumberOfFedChannel() ;
      if (connectionNumber == 0) {
	*out << cgicc::h2("No connections found in the database for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      else {
	*out << cgicc::h2("Find " + toString(connectionNumber) + " connections in the database for partition " + partitionDbName_) << std::endl ;
	*out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << "<th>" << "FED Id" << "</th>" << std::endl;
	*out << "<th>" << "FED Channel" << "</th>" << std::endl;
	*out << "<th>" << "FEC Slot" << "</th>" << std::endl;
	*out << "<th>" << "Ring Slot" << "</th>" << std::endl;
	*out << "<th>" << "CCU Address" << "</th>" << std::endl;
	*out << "<th>" << "I2C Channel" << "</th>" << std::endl;
	*out << "<th>" << "APV" << "</th>" << std::endl;
	*out << cgicc::tr() << std::endl;
	for (int i = 0 ; i < connectionNumber ; i ++) {
	  FedChannelConnectionDescription* fedChannelConnectionDescription = deviceFactory_->getFedChannelConnection(i) ;
	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::td() << std::dec << fedChannelConnectionDescription->getFedId() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::dec << fedChannelConnectionDescription->getFedChannel() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::dec << fedChannelConnectionDescription->getSlot() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::dec << fedChannelConnectionDescription->getRing() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::hex << "0x" << fedChannelConnectionDescription->getCcu() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::hex << "0x" << fedChannelConnectionDescription->getI2c() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << std::hex << "0x" << fedChannelConnectionDescription->getApv() << cgicc::td() << std::endl ;
	  *out << cgicc::tr() << std::endl;
	}
	*out << cgicc::table() << std::endl ;
      }   
    }
    else {

      // Retreive the partition names
      std::list<std::string> *partitionNames = deviceFactory_->getAllPartitionNames() ;
      if ( (partitionNames != NULL) && (partitionNames->size()) ) {
	
	// Form and action
	std::string url = "/";
	url += getApplicationDescriptor()->getURN();
	url += "/ApplyFECFEDConnections";
	*out << cgicc::form().set("method","post")
	  .set("action", url)
	  .set("enctype","multipart/form-data") << std::endl;
	
	// Build a list of partition name
	std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
	for (std::list<std::string>::iterator itPartition = partitionNames->begin() ; itPartition != partitionNames->end() ; itPartition ++) {
	  if (partitionDbName_ == (*itPartition)) 
	    partitionButton << "<option selected>" << (*itPartition) << "</option>";
	  else
	    partitionButton << "<option>" << (*itPartition) << "</option>";
	}
	partitionButton << "</select>" << std::endl ;
	
	delete partitionNames ;
	
	*out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please select the partition then click Apply: " << "</big></big></span>"
	     << partitionButton.str() << std::endl ;
	
	// Apply
	*out << cgicc::p() << cgicc::input().set("type", "submit")
	  .set("name", "submit")
	  .set("value", "Apply");
	
	// End of the form
	*out << cgicc::form() << std::endl;

	// Few remarks
	*out << cgicc::h2("Few remarks") << std::endl ;
	*out << "<lu>" << std::endl ;
	if (partitionDbName_ == "") {
	  *out << "<li>" << "The list of the connections will be displayed after you click Apply" << "</li>" << std::endl ;
	  *out << "<li>" << "If you click again on the link " 
	       << "<a href=\"/%s/MoreParameters\">[More Parameters]</a>"
	       << ", you can choose a different partition" << "</li>" << std::endl ;
	  *out << "<lu>" << std::endl ;
	}
      }
      else *out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    }
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2("Unable to retreive the connection information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the  information", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2("Unable to retreive the connection information (Oracle exception) " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the connection information", e, LOGFATAL) ;
  }

  *out << cgicc::html() << std::endl;
}
#endif

/** Display the connections inside the database
 */
void TkConfigurationDb::applyFECFEDConnections (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Check if it is a partition name choose or it was already chosen
    if (partitionDbName_ == "") {
      partitionDbName_ = cgi["PartitionName"]->getValue() ;
      displayFECFEDConnections(in,out) ;
    }
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Error CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

/** Display the run information
 */
void TkConfigurationDb::displayRunInformation (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 5 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Run Information");

  displayDatabaseRelatedLinks(in,out) ;

  if (displayRunInformationMessage_.str().size()) {
    if (displayRunInformationMessage_.str().find("Error") != std::string::npos)
      *out << cgicc::h2(displayRunInformationMessage_.str()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl ;
    else
      *out << cgicc::h2(displayRunInformationMessage_.str()) << std::endl ;
    displayRunInformationMessage_.str("") ;
  }

  try {

    // Form and action
    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    url += "/ApplyRunInformation";
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;

    // ------------------------------------------------------------------------------------------------------
    // Getting all runs
    tkRunVector allRuns  = deviceFactory_->getAllRuns() ;

    // ------------------------------------------------------------------------------------------------------
    // Display the list of run for state change
    if (dbVersion_ >= 5.14) {
      std::stringstream runNumberList ; runNumberList << "<select name=\"runNumber\">" ;
      std::map<unsigned int, bool> allMapRuns ;
      for (tkRunVector::iterator it = allRuns.begin() ; it != allRuns.end() ; it ++) {
	if (!allMapRuns[(*it)->getRunNumber()]) {
	  allMapRuns[(*it)->getRunNumber()] = true ;
	  if (it == allRuns.begin()) {
	    runNumberList << "<option selected>" << (*it)->getRunNumber() << "</option>";
	  }
	  else {
	    runNumberList << "<option>" << (*it)->getRunNumber() << "</option>";
	  }
	}
      }
      runNumberList << "</select>" << std::endl ;
    
      // Set it in the database
      *out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	   << "Please check one these boxes to apply any action: " 
	   << "</big></big></span>" << std::endl ;
      *out << cgicc::p() << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
      *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << "<big>" << "Update the comments for a given run: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","UpdateComment").set("checked","true") << cgicc::td() << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << "<big>" << "Please select the run number for the two next operations: " << "</big>" << cgicc::td() << cgicc::td() << runNumberList.str() << cgicc::td() << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() << "<a href=\"#displayRemarksAboutUpdate\">" << "<big>" << "Update the current state with the run number for the partitions: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","UpdateCurrentState") << "</a>" << cgicc::td() << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
      *out << cgicc::td() <<  "<a href=\"#displayRemarksAboutChange\">" << "<big>" << "Change the current state for the one which are in the run number: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","ChangeCurrentState") << "</a>" << cgicc::td() << std::endl ;
      *out << cgicc::tr() << std::endl;
      *out << cgicc::table() << std::endl ;

      // Apply
      *out << cgicc::p() << cgicc::input().set("type", "submit")
	.set("name", "submit")
	.set("value", "Apply");
      
      *out << cgicc::p() << cgicc::p() << std::endl ;
    }
    // ------------------------------------------------------------------------------------------------------
    // Display the information about the run
    *out << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0") << std::endl ;
    *out << cgicc::tr() << std::endl ;
    *out << cgicc::th() << "Run Number" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "State Name" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "Partition Name" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "Mode" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "Start Time" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "FEC Version" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "FED Version" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "Connection Version" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "Det ID Version" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "DCU/PSU Map Version" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "O2O Transfer" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "Local Run" << cgicc::th() << std::endl ;
    *out << cgicc::th() << "Comments" << cgicc::th() << std::endl ;
    *out << cgicc::tr() << std::endl ;

    for (tkRunVector::iterator it = allRuns.begin() ; it != allRuns.end() ; it ++) {

      *out << cgicc::tr() << std::endl ;
      *out << cgicc::td() << (*it)->getRunNumber() << cgicc::td() 
	   << cgicc::td() << (*it)->getStateName() << cgicc::td() 
	   << cgicc::td() << (*it)->getPartitionName() << cgicc::td() 
	   << cgicc::td() << (*it)->getMode() << cgicc::td() 
	   << cgicc::td() << (*it)->getStartingTime() << cgicc::td() 
	   << cgicc::td() << (*it)->getFecVersionMajorId() << "." << (*it)->getFecVersionMinorId() << cgicc::td() 
	   << cgicc::td() << (*it)->getFedVersionMajorId() << "." << (*it)->getFedVersionMinorId() << cgicc::td() 
	   << cgicc::td() << (*it)->getConnectionVersionMajorId() << "." << (*it)->getConnectionVersionMinorId() << cgicc::td() 
	   << cgicc::td() << (*it)->getDcuInfoVersionMajorId() << "." << (*it)->getDcuInfoVersionMinorId() << cgicc::td()
	   << cgicc::td() << (*it)->getDcuPsuMapVersionMajorId() << "." << (*it)->getDcuPsuMapVersionMinorId() << cgicc::td() ;
      *out << cgicc::td() ;
      if ((*it)->getO2O())
	*out << cgicc::input().set("type", "checkbox")
	  .set("name","O2ORun").set("checked","true").set("readonly", "readonly") ;
      else
	*out << cgicc::input().set("type", "checkbox")
	  .set("name","O2ORun").set("readonly", "readonly") ;
      *out << cgicc::td() ;
      *out << cgicc::td() ;
      if ((*it)->getLocal()) 
	*out << cgicc::input().set("type", "checkbox")
	  .set("name","LocalRun").set("checked","true").set("readonly", "readonly") ;
      else
	*out << cgicc::input().set("type", "checkbox")
	  .set("name","LocalRun").set("readonly", "readonly") ;
      *out << cgicc::td() ;
      *out << cgicc::td() << cgicc::input().set("type","text").set("name",std::string("CommentRun") + toString((*it)->getRunNumber())).set("value",(*it)->getComments()) << cgicc::td() << std::endl ;
      *out << cgicc::tr() << std::endl ;
    }

    *out << cgicc::table() << std::endl ;

    // Apply
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Apply");

    // Few remarks
    *out << cgicc::h2("Few remarks about the update of state with the run number") << std::endl ;    
    *out << "Take the state given run number and update the current state with this information."
	 << "<br>" << "<span style=\"font-weight: bold;\">" << "Behaviour and example:" << "</span>"
	 << "<ul>"
	 << "<li>run 1000 with state history 100 has the following information:"
	 << "<ul>"
	 << "  <li> partition1, 7.0"
	 << "  <li> partition4, 4.0"
	 << "</ul>"
	 << "<li>State history 100"
	 << "<ul>"
	 << "  <li> partition1, 7.0  << used in the run 1000"
	 << "  <li> partition2, 2.0  << not used in the run 1000"
	 << "  <li> partition4, 4.0  << used in the run 1000"
	 << "</ul>"
	 << "<li>Current state is:"
	 << "<ul>"
	 << "  <li> partition1, 1.0"
	 << "  <li> partition2, 2.0"
	 << "  <li> partition3, 3.0"
	 << "</ul>"
	 << "</ul>"
	 << "<a name=displayRemarksAboutUpdate>" 
	 << "<span style=\"font-weight: bold;\">" << "Update the current state with the run number for the partitions: " << "</span>"
	 << "In the case where you want to update the current with the information in the state history coming from the run. If the partition is present in the current state then the version is updated, if the partition is not present in the current state then it is added. In the example, the current state result will be:" << "</a>"
	 << "<ul>"
	 << "<li> partition1, 7.0 << version updated"
	 << "<li> partition2, 2.0 << no change"
	 << "<li> partition3, 3.0 << no change"
	 << "<li> partition4, 4.0 << partition added to the current state"
	 << "</ul>"
	 << "<a name=displayRemarksAboutChange>" 
	 << "<span style=\"font-weight: bold;\">" << "Change the current state for the one which are in the run number: " << "</span>"
	 << "In the case you want to change the current state with the state in the run number. In the example, you will find the state history from the run 1000 ie. state history 100, current state result is:" << "</a>"
	 << "<ul>"
	 << "<li> partition1, 7.0"
	 << "<li> partition2, 2.0"
	 << "<li> partition4, 4.0"
	 << "</ul>"
	 << "Please note here that the partition2 that was not in the run is anyway since we copy the run. This part can be modified if you want only the partition in the run." << std::endl ;

    // End of the form
    *out << cgicc::form() << std::endl;
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2("Unable to retreive the run information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the run information", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2("Unable to retreive the run information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the run information", e, LOGFATAL) ;
  }

  *out << cgicc::html() << std::endl;
}

/** Display the connections inside the database
 */
void TkConfigurationDb::applyRunInformation (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  //*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //*out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  //xgi::Utils::getPageHeader(*out, "Run Information");
  //displayDatabaseRelatedLinks(in,out) ;
  
  // Create a new Cgicc object containing all the CGI data
  cgicc::Cgicc cgi(in);
  unsigned int runNumber = 0 ;

  try {
    // Update the comments
    if (cgi["actionToBeApply"]->getValue() == "UpdateComment") {
      tkRunVector allRuns  = deviceFactory_->getAllRuns() ;
      
      // Check if any comments has been changed
      for (tkRunVector::iterator it = allRuns.begin() ; it != allRuns.end() ; it ++) {
	std::string commentRun = std::string("CommentRun") + toString((*it)->getRunNumber()) ;
	if ((*it)->getComments() != cgi[commentRun]->getValue()) {
	  deviceFactory_->updateRunComment((*it)->getPartitionName(), (*it)->getRunNumber(), cgi[commentRun]->getValue()) ;
	}
      }

      displayRunInformationMessage_ << "Comment(s) updated" ;
    }
    // Update the current state
    if (cgi["actionToBeApply"]->getValue() == "UpdateCurrentState") {
      runNumber = fromString<unsigned int>(cgi["runNumber"]->getValue()) ;

      deviceFactory_->copyStateForRunNumber(runNumber,false);

      displayRunInformationMessage_ << "Current state updated with the one in run " << runNumber ;
    }
    // Change the current state
    if (cgi["actionToBeApply"]->getValue() == "ChangeCurrentState") {
      runNumber = fromString<unsigned int>(cgi["runNumber"]->getValue()) ;

      deviceFactory_->copyStateForRunNumber(runNumber,true);
      
      displayRunInformationMessage_ << "Current state changed for the one in run " << runNumber ;
    }
  }
  catch (FecExceptionHandler &e) {
    //*out << cgicc::h2("Unable to retreive the run information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    if (cgi["actionToBeApply"]->getValue() == "ChangeCurrentState")
      displayRunInformationMessage_ << "Error during the change of the current state for the run " << runNumber << ": " << e.what() ;
    else if (cgi["actionToBeApply"]->getValue() == "UpdateCurrentState")
      displayRunInformationMessage_ << "Error during the update of the current state with the run " << runNumber << ": " << e.what() ;
    else if (cgi["actionToBeApply"]->getValue() == "UpdateCurrentState")
      displayRunInformationMessage_ << "Error during the update of comments for the runs: " << e.what() ;

    errorReportLogger_->errorReport (displayRunInformationMessage_.str(), e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    //*out << cgicc::h2("Unable to retreive the run information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    if (cgi["actionToBeApply"]->getValue() == "ChangeCurrentState")
      displayRunInformationMessage_ << "Error in oracle during the change of the current state for the run " << runNumber << ": " << e.what() ;
    else if (cgi["actionToBeApply"]->getValue() == "UpdateCurrentState")
      displayRunInformationMessage_ << "Error in oracle during the update of the current state with the run " << runNumber << ": " << e.what() ;
    else if (cgi["actionToBeApply"]->getValue() == "UpdateCurrentState")
      displayRunInformationMessage_ << "Error in oracle during the update of comments for the runs: " << e.what() ;

    errorReportLogger_->errorReport (displayRunInformationMessage_.str(), e, LOGFATAL) ;
  }

  this->displayRunInformation(in,out) ;
  //*out << cgicc::html() << std::endl;
}

/** Display the connections inside the database
 */
void TkConfigurationDb::displayDCUPSUMap (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 4 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Connections Display");

  displayDatabaseRelatedLinks(in,out) ;

  try {

    if (partitionDbName_ != "") {

      deviceFactory_->getDcuPsuMapPartition(partitionDbName_) ;
      tkDcuPsuMapVector v = deviceFactory_->getAllTkDcuPsuMaps() ;
      deviceFactory_->getPsuNotConnectedPartition(partitionDbName_) ;

      if (v.empty()) {
	*out << cgicc::h2("No PSU/DCU map found in the database for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

	deviceFactory_->getPsuNamePartition(partitionDbName_) ;
	
	tkDcuPsuMapVector vPsuNameCG = deviceFactory_->getControlGroupDcuPsuMaps() ;
	tkDcuPsuMapVector vPsuNamePG = deviceFactory_->getPowerGroupDcuPsuMaps() ;

	if (vPsuNameCG.empty() && vPsuNamePG.empty()) 
	  *out << cgicc::h2("No PSU names in the database for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl ;
	else {

	  *out << cgicc::h2("Find " + toString(vPsuNameCG.size() + vPsuNamePG.size()) + " PSU for partition " + partitionDbName_) << std::endl ;
	  
	  *out << "<a name=displayDCUPSUMapPsuName>" << cgicc::h2("Display by PSU names") << "</a>" << std::endl ;
	  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << std::endl;
	  *out << "<th>" << "PVSS Name" << "</th>" << std::endl;
	  *out << "<th>" << "Datapoint name" << "</th>" << std::endl;
	  *out << cgicc::tr() << std::endl;
	  
	  for (tkDcuPsuMapVector::iterator it = vPsuNameCG.begin() ; it != vPsuNameCG.end() ; it ++) {
	    TkDcuPsuMap *vPVSS = (*it) ;
	    *out << cgicc::td() << vPVSS->getPVSSName() << cgicc::td() 
		 << cgicc::td() << vPVSS->getDatapointName() << cgicc::td() << std::endl ;
	    *out << cgicc::tr() << std::endl;
	  }
	  
	  for (tkDcuPsuMapVector::iterator it = vPsuNamePG.begin() ; it != vPsuNamePG.end() ; it ++) {
	    TkDcuPsuMap *vPVSS = (*it) ;
	    *out << cgicc::td() << vPVSS->getPVSSName() << cgicc::td() 
		 << cgicc::td() << vPVSS->getDatapointName() << cgicc::td() << std::endl ;
	    *out << cgicc::tr() << std::endl;
	  }
	  
	  *out << cgicc::table() << std::endl ;
	}
      }
      else {

	std::map<std::string, tkDcuPsuMapVector> psuDcuList ;
	unsigned int arrayLength = 0 ;
	for (tkDcuPsuMapVector::iterator it = v.begin() ; it != v.end() ; it ++) {
	  psuDcuList[(*it)->getPVSSName()].push_back(*it) ;
	  if (psuDcuList[(*it)->getPVSSName()].size() > arrayLength) arrayLength = psuDcuList[(*it)->getPVSSName()].size() ;
	}

	// Display the list
	*out << cgicc::h2("Find " + toString(v.size()) + " PSU/DCU information for partition " + partitionDbName_) << std::endl ;

	// Related links
	*out << "<ul><li><a href=\"#displayDCUPSUMapDcuHardId\">Display by DCU hardware ID</a></li>" << std::endl ;
//	*out << "<li><a href=\"#displayDCUPSUMapDETID\">Display by DET ID</a></li>" << std::endl ;
	*out << "<li><a href=\"#displayDCUPSUMapMissingPsuName\">Display PSU which have no DCU</a></li>" << std::endl ;
	*out << "</ul>" << std::endl ;

	// Display DCU/PSU map with DCUs
	*out << "<a name=displayDCUPSUMapDcuHardId>" << cgicc::h2("Display by DCU HARD ID") << "</a>"<< std::endl ;
	*out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	*out << cgicc::tr() << std::endl;
	*out << "<th>" << "PVSS Name" << "</th>" << std::endl;
	*out << "<th>" << "DCU list" << "</th>" << std::endl ;
	*out << cgicc::tr() << std::endl;
	for (std::map<std::string, tkDcuPsuMapVector>::iterator it = psuDcuList.begin() ; it != psuDcuList.end() ; it ++) {

	  tkDcuPsuMapVector vPVSS = it->second ;
	  unsigned int i = 0 ;
	  *out << cgicc::tr() << std::endl;
	  for (tkDcuPsuMapVector::iterator it = vPVSS.begin() ; it != vPVSS.end() ; it ++) {
	    if (i == 0) *out << cgicc::td() << std::dec << (*it)->getPVSSName() << cgicc::td() << std::endl ;
	    *out << cgicc::td() << std::dec << (*it)->getDcuHardId() << cgicc::td() << std::endl ;
	    i ++ ;
	  }
	  for ( ; i < arrayLength ; i ++) *out << cgicc::td() << cgicc::td() << std::endl ;	  
	  *out << cgicc::tr() << std::endl;
	}
	*out << cgicc::table() << std::endl ;

	*out << cgicc::p() << std::endl ;
	*out << cgicc::p() << std::endl ;

// 	// Display the list
// 	*out <<  "<a name=displayDCUPSUMapDETID>" << cgicc::h2("Display by DET ID") << "</a>" << std::endl ;
// 	*out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
// 	*out << cgicc::tr() << std::endl;
// 	*out << "<th>" << "PVSS Name" << "</th>" << std::endl;
// 	*out << "<th>" << "DET ID List" << "</th>" << std::endl ;
// 	*out << cgicc::tr() << std::endl;
// 	for (std::map<std::string, tkDcuPsuMapVector>::iterator it = psuDcuList.begin() ; it != psuDcuList.end() ; it ++) {

// 	  tkDcuPsuMapVector vPVSS = it->second ;
// 	  unsigned int i = 0 ;
// 	  *out << cgicc::tr() << std::endl;
// 	  for (tkDcuPsuMapVector::iterator it = vPVSS.begin() ; it != vPVSS.end() ; it ++) {
// 	    if (i == 0) *out << cgicc::td() << std::dec << (*it)->getPVSSName() << cgicc::td() << std::endl ;
// 	    try {
// 	      TkDcuInfo *tkDcuInfo = deviceFactory_->getTkDcuInfo((*it)->getDcuHardId()) ;
// 	      *out << cgicc::td() << std::dec << tkDcuInfo->getDetId() << cgicc::td() << std::endl ;
// 	    }
// 	    catch (FecExceptionHandler &e) {
// 	      *out << cgicc::td() << std::dec << "UNK" << cgicc::td() << std::endl ;
// 	    }
// 	    i ++ ;
// 	  }
// 	  for ( ; i < arrayLength ; i ++) *out << cgicc::td() << cgicc::td() << std::endl ;	  
// 	  *out << cgicc::tr() << std::endl;
// 	}
// 	*out << cgicc::table() << std::endl ;

// 	*out << cgicc::p() << std::endl ;
// 	*out << cgicc::p() << std::endl ;

	deviceFactory_->getPsuNotConnectedPartition(partitionDbName_);
	tkDcuPsuMapVector vCGNotConnected = deviceFactory_->getControlGroupPsuNotConnected();
	tkDcuPsuMapVector vPGNotConnected = deviceFactory_->getPowerGroupPsuNotConnected() ;
	
	if (vCGNotConnected.empty() && vPGNotConnected.empty()) {
	  *out << "<a name=displayDCUPSUMapMissingPsuName>" << cgicc::h2("All PSU have DCUs connected to it for partition " + partitionDbName_) << "</a>" << std::endl;
	}
	else {
	  // Display the list of PSU not connected
	  *out << cgicc::h2("Find " + toString(vCGNotConnected.size() + vPGNotConnected.size()) + " PSU not connected for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl ;
	  
	  *out << "<a name=displayDCUPSUMapMissingPsuName>" << cgicc::h2("Display by PSU names") << "</a>" << std::endl ;
	  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << std::endl;
	  *out << "<th>" << "PVSS Name" << "</th>" << std::endl;
	  *out << "<th>" << "Datapoint name" << "</th>" << std::endl;
	  *out << cgicc::tr() << std::endl;
	  
	  for (tkDcuPsuMapVector::iterator it = vCGNotConnected.begin() ; it != vCGNotConnected.end() ; it ++) {
	    TkDcuPsuMap *vPVSS = (*it) ;
	    *out << cgicc::td() << vPVSS->getPVSSName() << cgicc::td() 
		 << cgicc::td() << vPVSS->getDatapointName() << cgicc::td() << std::endl ;
	    *out << cgicc::tr() << std::endl;
	  }
	  
	  for (tkDcuPsuMapVector::iterator it = vPGNotConnected.begin() ; it != vPGNotConnected.end() ; it ++) {
	    TkDcuPsuMap *vPVSS = (*it) ;
	    *out << cgicc::td() << vPVSS->getPVSSName() << cgicc::td() 
		 << cgicc::td() << vPVSS->getDatapointName() << cgicc::td() << std::endl ;
	    *out << cgicc::tr() << std::endl;
	  }
	
	  *out << cgicc::table() << std::endl ;
	}
      }
    }
    else {

      // Retreive the partition names
      std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNames() ;
      if (partitionNames.size()) {
	
	// Form and action
	std::string url = "/";
	url += getApplicationDescriptor()->getURN();
	url += "/ApplyDcuPsuMap";
	*out << cgicc::form().set("method","post")
	  .set("action", url)
	  .set("enctype","multipart/form-data") << std::endl;
	
	// Build a list of partition name
	std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
	for (std::list<std::string>::iterator itPartition = partitionNames.begin() ; itPartition != partitionNames.end() ; itPartition ++) {
	  if (partitionDbName_ == (*itPartition)) 
	    partitionButton << "<option selected>" << (*itPartition) << "</option>";
	  else
	    partitionButton << "<option>" << (*itPartition) << "</option>";
	}
	partitionButton << "</select>" << std::endl ;
	
	*out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please select the partition then click Apply: " << "</big></big></span>"
	     << partitionButton.str() << std::endl ;
	
	// Apply
	*out << cgicc::p() << cgicc::input().set("type", "submit")
	  .set("name", "submit")
	  .set("value", "Apply");
	
	// End of the form
	*out << cgicc::form() << std::endl;

	// Few remarks
	*out << cgicc::h2("Few remarks") << std::endl ;
	*out << "<lu>" << std::endl ;
	if (partitionDbName_ == "") {
	  *out << "<li>" << "The list of PSU/DCU map will be displayed after you click Apply" << "</li>" << std::endl ;
	  *out << "<li>" << "If you click again on the link " 
	       << "<a href=\"/%s/MoreParameters\">[More Parameters]</a>"
	       << ", you can choose a different partition" << "</li>" << std::endl ;
	  *out << "<lu>" << std::endl ;
	}
      }
      else *out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    }
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2("Unable to retreive the connection information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the  information", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2("Unable to retreive the connection information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the connection information", e, LOGFATAL) ;
  }

  *out << cgicc::html() << std::endl;
}

/** Display the connections inside the database
 */
void TkConfigurationDb::applyDCUPSUMap (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Check if it is a partition name choose or it was already chosen
    if (partitionDbName_ == "") {
      partitionDbName_ = cgi["PartitionName"]->getValue() ;
      displayDCUPSUMap(in,out) ;
    }
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Error CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

/** Disable / enable structure based on the det id information
 */
void TkConfigurationDb::displayEnableDisableStructure (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 6 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Structure Selection");

  displayDatabaseRelatedLinks(in,out) ;

  if ((deviceFactory_ != NULL) && (deviceFactory_->getDbConnected())) {
    
    if (partitionDbName_ != "") {
      // Form and action
      std::string url = "/";
      url += getApplicationDescriptor()->getURN();
      url += "/applyEnableDisableStructure";
      *out << cgicc::form().set("method","post")
	.set("action", url)
	.set("enctype","multipart/form-data") << std::endl;
    
      // Mask
      *out << cgicc::fieldset() << std::endl;
      *out << cgicc::legend("Mask to be applied (in hexadecimal)") << std::endl;
      *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << "<th style=\"text-align: left; width: 150px;\">Mask </th>" << std::endl ;
      *out << cgicc::td() << cgicc::input().set("type","text").set("name","mask").set("value","0x") << cgicc::td() << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << "<th style=\"text-align: left; width: 150px;\">Enable Modules</th>" << std::endl ;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","disableenable1").set("checked","true").set("value","enable") << cgicc::td() << std::endl;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << "<th style=\"text-align: left; width: 150px;\">Disable Modules</th>" << std::endl ;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","disableenable1").set("value","disable") << cgicc::td() << std::endl;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::table() << std::endl ;
      *out << cgicc::input().set("type", "submit").set("name", "submit").set("value", "Mask Modules").set("readonly","readonly") << std::endl ;
      *out << cgicc::fieldset() << std::endl;
      
      // Display the detector
      *out << cgicc::fieldset() << std::endl;
      *out << cgicc::legend("Detector selection") << std::endl;
      *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << "<th style=\"text-align: left; width: 150px;\">Sub-detector </th>" << std::endl ;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","subdetector").set("value","TOB").set("checked","true") << cgicc::label("TOB") << cgicc::td() << std::endl;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","subdetector").set("value","TIB") << cgicc::label("TIB")  << cgicc::td() << std::endl;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","subdetector").set("value","TID") << cgicc::label("TID")  << cgicc::td() << std::endl;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","subdetector").set("value","TEC") << cgicc::label("TEC")  << cgicc::td() << std::endl;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << "<th style=\"text-align: left; width: 150px;\">Side </th>" << std::endl ;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","side").set("value","+").set("checked","true") << cgicc::label("+") << cgicc::td() << std::endl;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","side").set("value","-") << cgicc::label("-") << cgicc::td() << std::endl;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::table() << std::endl ;

      *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << "<th style=\"text-align: left; width: 150px;\">Layer</th>" << std::endl ;
      *out << cgicc::td() << cgicc::input().set("type","text").set("name","layer").set("value","") << cgicc::td() << std::endl ;
      *out << cgicc::tr() << std::endl ;      
      *out << cgicc::tr() << std::endl ;
      *out << "<th style=\"text-align: left; width: 150px;\">Enable Modules</th>" << std::endl ;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","disableenable2").set("checked","true").set("value","enable") << cgicc::td() << std::endl;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::tr() << std::endl ;
      *out << "<th style=\"text-align: left; width: 150px;\">Disable Modules</th>" << std::endl ;
      *out << cgicc::td() << cgicc::input().set("type", "radio").set("name","disableenable2").set("value","enable") << cgicc::td() << std::endl;
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::table() << std::endl ;      
      *out << cgicc::input().set("type", "submit").set("name", "submit").set("value", "Disable Modules") << std::endl ;
      *out << cgicc::fieldset() << std::endl;

      *out << cgicc::form() << std::endl ;
    }
    else {

      // Retreive the partition names
      std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNames() ;
      if (partitionNames.size()) {
	
	// Form and action
	std::string url = "/";
	url += getApplicationDescriptor()->getURN();
	url += "/applyEnableDisableStructure";
	*out << cgicc::form().set("method","post")
	  .set("action", url)
	  .set("enctype","multipart/form-data") << std::endl;
	
	// Build a list of partition name
	std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
	for (std::list<std::string>::iterator itPartition = partitionNames.begin() ; itPartition != partitionNames.end() ; itPartition ++) {
	  if (partitionDbName_ == (*itPartition)) 
	    partitionButton << "<option selected>" << (*itPartition) << "</option>";
	  else
	    partitionButton << "<option>" << (*itPartition) << "</option>";
	}
	partitionButton << "</select>" << std::endl ;
	
	*out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please select the partition then click Apply: " << "</big></big></span>"
	     << partitionButton.str() << std::endl ;
	
	// Apply
	*out << cgicc::p() << cgicc::input().set("type", "submit")
	  .set("name", "submit")
	  .set("value", "Apply");
	
	// End of the form
	*out << cgicc::form() << std::endl;

	// Few remarks
	*out << cgicc::h2("Few remarks") << std::endl ;
	*out << "<lu>" << std::endl ;
	if (partitionDbName_ == "") {
	  *out << "<li>" << "The disable/enable module page will be displayed after you click Apply" << "</li>" << std::endl ;
	  *out << "<li>" << "If you click again on the link " 
	       << "<a href=\"/%s/MoreParameters\">[More Parameters]</a>"
	       << ", you can choose a different partition" << "</li>" << std::endl ;
	  *out << "<lu>" << std::endl ;
	}
      }
      else *out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    }
  }
  else 
    *out << cgicc::h2("Please specify a database to use these links").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;      
  
  *out << cgicc::html() << std::endl ; 
}

/** Display the connections inside the database
 */
void TkConfigurationDb::applyEnableDisableStructure (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Check if it is a partition name choose or it was already chosen
    if ( (partitionDbName_ == "") || (deviceValues_.size() == 0) || (dcuDevices_.size() == 0) ) {
      partitionDbName_ = cgi["PartitionName"]->getValue() ;
      displayEnableDisableStructure(in,out) ;
    }
    else {
      *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
      *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
      *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
      xgi::Utils::getPageHeader(*out, "Module Management");
      
      displayDatabaseRelatedLinks(in,out) ;

      if (cgi["submit"]->getValue() == "Mask Modules") {

	// Retreive the parameters
	bool enableModule = (cgi["disableenable1"]->getValue() == "enable") ;
	if ( (cgi["mask"]->getValue() != "") && (cgi["mask"]->getValue() != "0x") ) {
	  unsigned int mask = fromHexString<unsigned int>(cgi["mask"]->getValue()) ;

	  // Retreive the DCU Info for all modules
	  bool dcuInformationRetreived = false ;
	  try {
	    deviceFactory_->addDetIdPartition(partitionDbName_) ;
	    dcuInformationRetreived = true ;
	  }
	  catch (FecExceptionHandler &e) {
	    *out << cgicc::h2("Unable to retreive the DCU information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the DCU information", e, LOGFATAL) ;
	  }
	  catch (oracle::occi::SQLException &e) {
	    *out << cgicc::h2("Unable to retreive the DCU information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    errorReportLogger_->errorReport ("Unable to retreive the DCU information", e, LOGFATAL) ;
	  }

	  if (dcuInformationRetreived) {
	    // Retreive the DCU info
	    Sgi::hash_map<unsigned long, TkDcuInfo *> vDcuInfoVector = deviceFactory_->getInfos() ;

	    if (vDcuInfoVector.size()) {

	      std::vector<unsigned int> vDcuHardId ;
	      // Apply the mask on the DCU id
	      for (Sgi::hash_map<unsigned long, TkDcuInfo *>::iterator it = vDcuInfoVector.begin() ; it != vDcuInfoVector.end() ; it ++) {
		// DCU ID pushed in the list of modules
		if (it->second->getDetId() & mask) {
		  vDcuHardId.push_back(it->second->getDcuHardId()) ;
		}
	      }

	      if (vDcuHardId.size()) {
		
		// ------------ Check to state to see a version should be uploaded
		bool createVersion = true ;
		unsigned short versionMajor = 0, versionMinor = 0 ;
		unsigned int major = 0, minor = 0 ;

		// A VERSION SHOULD BE ALWAYS uploaded even if it was never used
		if (createVersion) errorReportLogger_->errorReport ("A new version both for FEC and FED will be done", LOGDEBUG) ;
		else errorReportLogger_->errorReport ("Using the version FEC " + toString(major) + "." + toString(minor) + 
						      " and FED " + toString(versionMajor) + "." + toString(versionMinor), LOGDEBUG) ;

		// FED: Upload a major version and disable the devices => a failure can occurs of the FED version is already used
		unsigned long fedDownloadTime = 0, fedUploadTime = 0, fecUploadTime = 0, startMillis, endMillis ;
		if (createVersion) {
		  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		  std::vector<Fed9U::Fed9UDescription*> *fedVector = deviceFactory_->getFed9UDescriptions(partitionDbName_) ;
		  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		  fedDownloadTime = (endMillis - startMillis) ;
		  errorReportLogger_->errorReport ( "Download of " + toString(fedVector->size()) + " FEDs in " + toString(fedDownloadTime) + " ms", LOGDEBUG) ;
		  if (fedVector->size()) {
		    deviceFactory_->setUsingStrips(true);
		    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		    deviceFactory_->setFed9UDescriptions(*fedVector,partitionDbName_,&versionMajor,&versionMinor,0); // minor version
		    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		    fedUploadTime = (endMillis - startMillis) ;
		    errorReportLogger_->errorReport ("Upload of " + toString(fedVector->size()) + " FEDs in " + toString(fedUploadTime) + " ms", LOGDEBUG) ;
		  }
		
		  // FEC: upload a minor version
		  if (deviceValues_.size() == 0) {
		    startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();		  
		    deviceFactory_->getFecDeviceDescriptions (partitionDbName_, deviceValues_) ;
		    endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		    errorReportLogger_->errorReport ( "Download of " + toString( deviceValues_.size()) + " FEC devices in " + 
						      toString(endMillis - startMillis) + " ms", LOGDEBUG) ;
		  }

		  startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		  deviceFactory_->setFecDeviceDescriptions (deviceValues_, partitionDbName_, &major, &minor, false) ;
		  endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		  fecUploadTime = (endMillis - startMillis) ;
		  errorReportLogger_->errorReport ("Upload of " + toString(deviceValues_.size()) + " FEC devices in " + toString(fecUploadTime) + " ms", LOGDEBUG) ;
		}

		// ------------------------------------------------------------------------------------
		// Enable and disable the corresponding modules
		if (enableModule) deviceFactory_->enableDevice(partitionDbName_,vDcuHardId) ;
		else deviceFactory_->disableDevice(partitionDbName_,vDcuHardId) ;

		// -------------------------------------------------------------------------------------
		// Display
		if (createVersion) {
		  std::stringstream msgInfo1 ; msgInfo1 << "Version created for partition " << partitionDbName_ ;
		  *out << cgicc::h2(msgInfo1.str()) ; 
		  *out << "<ul>" << std::endl ;
		  std::stringstream msgInfo2 ; msgInfo2 << "<li>" << "FEC version " << std::dec << major << "." << minor << " created in " << fecUploadTime << " ms" << std::endl ;
		  *out << cgicc::h3(msgInfo2.str()) ; 
		  std::stringstream msgInfo3 ; msgInfo3 << "<li>" << "FED Version " << versionMajor << "." << versionMinor << " created in " << fedUploadTime << " ms" << std::endl ;
		  *out << cgicc::h3(msgInfo3.str()) ; 
		  *out << "</ul>" << std::endl ;
		}
		else {
		  std::stringstream msgInfo1 ; msgInfo1 << "Version used for partition " << partitionDbName_ ;
		  *out << cgicc::h2(msgInfo1.str()) ; 
		  *out << "<ul>" << std::endl ;
		  std::stringstream msgInfo2 ; msgInfo2 << "<li>" << "FEC version " << std::dec << major << "." << minor << std::endl ;
		  *out << cgicc::h3(msgInfo2.str()) ; 
		  std::stringstream msgInfo3 ; msgInfo3 << "<li>" << "FED Version " << versionMajor << "." << versionMinor << std::endl ;
		  *out << cgicc::h3(msgInfo3.str()) ; 
		  *out << "</ul>" << std::endl ;
		}

		std::stringstream msgInfo ; msgInfo << vDcuHardId.size() << " DCUs list of modules to be " ;
		if (enableModule )msgInfo << "enabled" ;
		else msgInfo << "disabled" ;
		*out << cgicc::h2(msgInfo.str()) ;
		*out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
		*out << cgicc::tr() << std::endl ;
		*out << "<th>" << "DCU Hard ID" << "</th>"; 
		*out << "<th>" << "Status" << "</th>"; 
		*out << "<th>" << "AND" << "</th>"; 
		*out << cgicc::tr() << std::endl ;
		for (std::vector<unsigned int>::iterator it = vDcuHardId.begin() ; it != vDcuHardId.end() ; it ++) {
		  *out << cgicc::tr() << cgicc::td() << std::endl ;
		  *out << (unsigned int)(*it) << std::endl ;
		  *out << cgicc::td() << cgicc::td() << std::endl ;
		  if (enableModule) *out << "ENABLED" << std::endl ;
		  else *out << "DISABLED" << std::endl ;
		  *out << cgicc::td() << std::endl ;
		  *out << cgicc::td() << std::endl ;
		  *out << ((*it) & mask) << std::endl ;
		  *out << cgicc::td() << std::endl ;
		  *out << cgicc::tr() << std::endl ;
		}
		*out << cgicc::table() << std::endl ;
	      }
	      else
		*out << cgicc::h2("No modules to be disabled").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl ;
	    }
	    else
	      *out << cgicc::h2("No DCU HARD ID/DET ID map found for that partition").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	  }
	  else
	    *out << cgicc::h2("No DCU HARD ID/DET ID map found for that partition").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	}
	else 
	  *out << cgicc::h2("Please specify a mask").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      else if (cgi["submit"]->getValue() == "Disable Modules") {
	*out << cgicc::h2("Detector not yet implemented").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;    
      }
      else
	*out << cgicc::h2("Detector not yet implemented").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;    
    }
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Error CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

/** Display a ring in an HTML table
 */
void TkConfigurationDb::displayRingHTMLTable ( xgi::Output *out, tkringVector allRings, bool readOnly ) throw (xgi::exception::Exception) {

  *out << cgicc::table().set("border","2").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << cgicc::tr() << std::endl ;
  *out << "<th>" << "FEC/Ring" << "</th>" ;
  *out << "<th>" << "FEC hardware ID" << "</th>" ;
  *out << "<th>" << "CCU address (hexa/dec)" << "</th>" ;
  *out << "<th>" << "CCU order" << "</th>" ;
  *out << "<th>" << "Ring Output / FEC input / CCU enabled" << "</th>" ;
  *out << "<th>" << "Ring Input / FEC output" << "</th>" ;
  *out << "<th>" << "Configuration display" << "</th>" ;
  *out << "<th>" << "Comment about configuration" << "</th>" ;
  *out << cgicc::tr() << std::endl ;
  keyType indexP = 0 ;
  for (tkringVector::iterator ringIt = allRings.begin() ; ringIt != allRings.end() ; ringIt ++) {

    keyType index = buildFecRingKey((*ringIt)->getFecSlot(),(*ringIt)->getRingSlot()) ;
    (*ringIt)->computeRedundancy() ;

    //http://cmsonline.cern.ch/portal/pls/portal/CMS_TRK_CABLING_PORTAL.TK_CCU_VIEW.show?p_arg_names=rack&p_arg_values=S1F00&p_arg_names=crate&p_arg_values=2&p_arg_names=slot&p_arg_values=3&p_arg_names=ring&p_arg_values=7

    // display the FEC ring
    if (indexP != index) { 
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::td() ;
      if (cmsOMDSlbAccess_) {
	*out << "<a href=http://cmsonline.cern.ch/portal/pls/portal/CMS_TRK_CABLING_PORTAL.TK_CCU_VIEW.show?" 
	     << "p_arg_names=fechardid&p_arg_values=" << (*ringIt)->getFecHardwareId()
	     << "&p_arg_names=ring&p_arg_values=" << (*ringIt)->getRingSlot()
	     << " target=\"_blank\">" ;
      }
      *out << "FEC " << std::dec << (*ringIt)->getFecSlot() << "." << (*ringIt)->getRingSlot() ;
      if (cmsOMDSlbAccess_) *out << "</a>" ;
      *out << cgicc::td() 
	   << cgicc::td() << (*ringIt)->getFecHardwareId() << cgicc::td() 
	   << cgicc::td() << cgicc::td()
	   << cgicc::td() << cgicc::td() ;
      
      // Ring input output
      if (readOnly) {
	if ((*ringIt)->getOutputAUsed()) *out << cgicc::td() << "A" << cgicc::td() ;
	else *out << cgicc::td() << "B" << cgicc::td() ;
	if ((*ringIt)->getInputAUsed()) *out << cgicc::td() << "A" << cgicc::td() ;
	else *out << cgicc::td() << "B" << cgicc::td() ;
      }
      else {
	std::stringstream ringOutput ; 
	ringOutput << "<select name=\"RingOutput" << (*ringIt)->getFecHardwareId() << (*ringIt)->getRingSlot() << "\">" ;
	if ((*ringIt)->getOutputAUsed()) {
	  ringOutput << "<option selected>" << "A" << "</option>";
	  ringOutput << "<option>" << "B" << "</option>";
	}
	else {
	  ringOutput << "<option>" << "A" << "</option>";
	  ringOutput << "<option selected>" << "B" << "</option>";
	}
	ringOutput << "</select>" << std::endl ;
	*out << cgicc::td() << ringOutput.str() << cgicc::td() ;

	std::stringstream ringInput ; 
	ringInput << "<select name=\"RingInput" << (*ringIt)->getFecHardwareId() << (*ringIt)->getRingSlot() << "\">" ;
	if ((*ringIt)->getInputAUsed()) {
	  ringInput << "<option selected>" << "A" << "</option>";
	  ringInput << "<option>" << "B" << "</option>";
	}
	else {
	  ringInput << "<option>" << "A" << "</option>";
	  ringInput << "<option selected>" << "B" << "</option>";
	}
	ringInput << "</select>" << std::endl ;
	*out << cgicc::td() << ringInput.str() << cgicc::td() ;
      }
      
      // More information / comments
      std::stringstream msgInfo ; (*ringIt)->display(msgInfo,false,true) ;
      *out << cgicc::td() << msgInfo.str() << cgicc::td() ;
      if (!(*ringIt)->isReconfigurable()) *out << cgicc::td() << (*ringIt)->getReconfigurationProblem() << cgicc::td() ;
      else {
	*out <<cgicc::td() << "Configuration is correct" << cgicc::td() ;
      }
      *out << cgicc::tr() << std::endl ;
    }
    else {
      *out << cgicc::td() <<  cgicc::td() 
	   << cgicc::td() <<  cgicc::td() << std::endl ;
    }
    
    // for each CCU on the rings
    std::vector<CCUDescription *> *ccuVector = (*ringIt)->getCcuVector( ) ;
    if ( (ccuVector == NULL) || (ccuVector->size() == 0) ) {
      *out << cgicc::tr() << std::endl ;
      *out << cgicc::td() << cgicc::td()
	   << cgicc::td() << cgicc::td()
	   << cgicc::td() << "NA" << cgicc::td()
	   << cgicc::td() << "NA" << cgicc::td()
	   << cgicc::td() << "NA" << cgicc::td()
	   << cgicc::td() << "NA" << cgicc::td()
	   << cgicc::td() << "NA" << cgicc::td()
	   << cgicc::td() << "No CCUs on ring" << cgicc::td() << std::endl ;
      *out << cgicc::tr() << std::endl ;
    }
    else {
      for (std::vector<CCUDescription *>::iterator ccuIt = ccuVector->begin()  ; ccuIt != ccuVector->end() ; ccuIt ++) {
	*out << cgicc::tr() <<  std::endl ;
	*out << cgicc::td() << cgicc::td()
	     << cgicc::td() << cgicc::td()
	     << cgicc::td() << std::dec << getCcuKey((*ccuIt)->getKey()) << " / 0x" << std::hex <<  getCcuKey((*ccuIt)->getKey()) << std::dec << cgicc::td()
	     << cgicc::td() << (*ccuIt)->getOrder() << cgicc::td() ;
	
	// CCU input output
	if (readOnly) {
	  //if ((*ccuIt)->getInputA()) *out << cgicc::td() << "A" << cgicc::td() ;
	  // else *out << cgicc::td() << "B" << cgicc::td() ;
	  //if ((*ccuIt)->getOutputA()) *out << cgicc::td() << "A" << cgicc::td() ;
	  // else *out << cgicc::td() << "B" << cgicc::td() ;
	  if ((*ccuIt)->getEnabled()) *out << cgicc::td() << "Enabled" << cgicc::td() ;
	  else *out << cgicc::td() << "Disabled" << cgicc::td() ;
	  *out << cgicc::td() << cgicc::td() << std::endl ;
	}
	else {
	  std::stringstream ccuInput ;

// 	  if ((*ccuIt)->getInputA()) {
// 	    ccuInput << "<option selected>" << "A" << "</option>";
// 	    ccuInput << "<option>" << "B" << "</option>";
// 	  }
// 	  else {
// 	    ccuInput << "<option>" << "A" << "</option>";
// 	    ccuInput << "<option selected>" << "B" << "</option>";
// 	  }
// 	  ccuInput << "</select>" << std::endl ;
// 	  std::stringstream ccuOutput ;
// 	  ccuOutput << "<select name=\"CCUOutput" << (*ringIt)->getFecHardwareId() << (*ringIt)->getRingSlot() << getCcuKey((*ccuIt)->getKey()) << "\">" ;
// 	  if ((*ccuIt)->getOutputA()) {
// 	    ccuOutput << "<option selected>" << "A" << "</option>";
// 	    ccuOutput << "<option>" << "B" << "</option>";
// 	  }
// 	  else {
// 	    ccuOutput << "<option>" << "A" << "</option>";
// 	    ccuOutput << "<option selected>" << "B" << "</option>";
// 	  }
// 	  ccuOutput << "</select>" << std::endl ;
// 	  *out << cgicc::td() << ccuOutput.str() << cgicc::td() ;

	  ccuInput << "<select name=\"CCUInput" << (*ringIt)->getFecHardwareId() << (*ringIt)->getRingSlot() << getCcuKey((*ccuIt)->getKey()) << "\">" ;
	  if ((*ccuIt)->getEnabled()) {
	    ccuInput << "<option selected>" << "Enabled" << "</option>";
	    ccuInput << "<option>" << "Disabled" << "</option>";
	  }
	  else {
	    ccuInput << "<option>" << "Enabled" << "</option>";
	    ccuInput << "<option selected>" << "Disabled" << "</option>";
	  }
	  ccuInput << "</select>" << std::endl ;
	  *out << cgicc::td() << ccuInput.str() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << cgicc::td() << std::endl ;
	}
	
	// More information / comments
	*out << cgicc::td() << "" << cgicc::td() ;
	if ((*ringIt)->hasDummyCcu() && ( ((*ccuIt)->getOrder() == 127) || ((*ccuIt)->getOrder() == 128) ) ) {
	  if ((*ringIt)->isDummyInverted())
	    *out << cgicc::td() << "TIB Dummy CCU" << cgicc::td() << std::endl ;
	  else
	    *out << cgicc::td() << "Dummy CCU" << cgicc::td() << std::endl ;
	}
	else *out << cgicc::td() << "" << cgicc::td() << std::endl ;
	*out << cgicc::tr() << std::endl ;
      }
    }
  }
  
  *out << cgicc::table() << std::endl ;
}

/** Display the run information
 */
void TkConfigurationDb::displayCCUConfiguration (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 7 ;

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface: CCU reconfiguration") << std::endl;
  xgi::Utils::getPageHeader(*out, "CCU configuration");

  displayDatabaseRelatedLinks(in,out) ;

  // Form and action
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/ApplyCCUConfiguration";
  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  try {

    // ------------------------------------------------------------------------------------------------------
    // No partition set
    if (partitionDbName_ == "") {

      // Retreive the partition names
      std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNames() ;
      if (partitionNames.size()) {
	
	// Build a list of partition name
	std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
	for (std::list<std::string>::iterator itPartition = partitionNames.begin() ; itPartition != partitionNames.end() ; itPartition ++) {
	  if (partitionDbName_ == (*itPartition)) 
	    partitionButton << "<option selected>" << (*itPartition) << "</option>";
	  else
	    partitionButton << "<option>" << (*itPartition) << "</option>";
	}
	partitionButton << "</select>" << std::endl ;
	
	*out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	     << "Please select the partition then click Apply: " << "</big></big></span>"
	     << partitionButton.str() << std::endl ;

	// Apply
	*out << cgicc::p() << cgicc::input().set("type", "submit")
	  .set("name", "submit")
	  .set("value", "Apply");
	
	// End of the form
	*out << cgicc::form() << std::endl;
      }
      else *out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    }
    // ------------------------------------------------------------------------------------------------------
    // Partition:
    else {
      *out << "<ul><li><a href=\"#displayByPartition\">Display CCU from the partition " << partitionDbName_ << "</a></li>" << std::endl ;
      if (cmsOMDSlbAccess_) {
	*out << "<li><a href=\"#displayTKCCDB\">Display CCU from TKCC database</a></li>" << std::endl ;
	*out << "<li><a href=http://cmsonline.cern.ch/portal/pls/portal/CMS_TRK_CABLING_PORTAL.TK_CCU_SEARCH.show target=\"_blank\">CCU configuration used in TKCC database</a></li>" << std::endl ;
      }
      else *out << "<li><a href=\"#displayTKCCDB\">Display CCU from static table in DB</a></li>" << std::endl ;
      *out << "</ul>" << std::endl ;
      *out << cgicc::fieldset() << std::endl;
      *out << cgicc::legend("CCU reconfiguration for partition " + partitionDbName_) << std::endl;
      *out << "<a name=displayByPartition></a>" << std::endl ;
      tkringVector allRings ;
      try {
	// Retreive the rings
	allRings = deviceFactory_->getDbRingDescription(partitionDbName_);
	
	if (allRings.size()) {
	  // Display it
	  displayRingHTMLTable ( out, allRings, false ) ;
	}
	else *out << cgicc::h2("No CCU found in database for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      catch (FecExceptionHandler &e) {
	*out << cgicc::h2("No ring configuration found for  partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      catch (oracle::occi::SQLException &e) {
	*out << cgicc::h2("No ring configuration found for  partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      *out << cgicc::fieldset() << std::endl;
      *out << cgicc::br() << std::endl ;
      *out << cgicc::br() << std::endl ;

      // Apply
      if (cmsOMDSlbAccess_) {
	*out << cgicc::h2("If you click apply, you will upload the information to the database for information not for configuration, please used the web page from <a href=http://cmsonline.cern.ch/portal/pls/portal/CMS_TRK_CABLING_PORTAL.TK_CCU_SEARCH.show target=\"_blank\">TKCC database</a> to be able to change or add configuration to the ring").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      //*out << cgicc::br() << std::endl ;
      //*out << cgicc::br() << std::endl ;

      *out << cgicc::p() << cgicc::input().set("type", "submit")
	.set("name", "submit")
	.set("value", "Apply");      

      *out << cgicc::br() << std::endl ;
      *out << cgicc::br() << std::endl ;

      // ------------------------------------------------------------------------------------------------------
      // From Construction database: FEC hardware ID
      // Find all FEC hardware ID / rings in the partitions
      if (allRings.size()) {
	std::map<std::pair<std::string, unsigned int>, bool > vFecRing ;
	for (tkringVector::iterator itRing = allRings.begin() ; itRing != allRings.end() ; itRing ++) {
	  std::pair<std::string, unsigned int> myPair = make_pair((*itRing)->getFecHardwareId(), (*itRing)->getRingSlot()) ;
	  if (vFecRing.find(myPair) == vFecRing.end()) {
	    vFecRing[myPair] = true ;
	  }
	}
	
	*out << cgicc::fieldset() << std::endl;
	*out << cgicc::legend("CCU reconfiguration from TKCC DB") << std::endl;
	*out << "<a name=displayTKCCDB></a>" << std::endl ;
	// for each FEC hardware ID, ring find the corresponding configuration
	tkringVector allRingsTkcc ;
	for (std::map<std::pair<std::string, unsigned int>, bool >::iterator it = vFecRing.begin() ; it != vFecRing.end() ; it ++) {
	  std::pair<std::string, unsigned int> myPair = it->first ;
	  try {
	    TkRingDescription* myRing = NULL ;
	    myRing = deviceFactory_->getDbRingDescription(myPair.first,myPair.second);

	    if (myRing != NULL) {
	      allRingsTkcc.push_back(myRing->clone()) ;

	      //myRing->computeRedundancy() ;
	      //if (!myRing->isReconfigurable()) std::cout << myRing->getReconfigurationProblem() << std::endl ;
	      //else { myRing->display(std::cout,false,true) ; std::cout << std::endl ; }
	    }
	    else {
	      *out << cgicc::h2("No ring configuration found for " + myPair.first + " on ring " + toString(myPair.second)).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	    }
	  }
	  catch (FecExceptionHandler &e) {
	    *out << cgicc::h2("No ring configuration found for " + myPair.first + " on ring " + toString(myPair.second)).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	  }
	  catch (oracle::occi::SQLException &e) {
	    *out << cgicc::h2("No ring configuration found for " + myPair.first + " on ring " + toString(myPair.second)).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	  }
	}

	if (allRingsTkcc.size()) {
	  // Display it
	  displayRingHTMLTable ( out, allRingsTkcc, true ) ;

	  FecFactory::deleteVectorI(allRingsTkcc) ;
	}
	*out << cgicc::fieldset() << std::endl;
      }
    }
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2("Unable to retreive the CCU information " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the CCU information", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2("Unable to retreive the CCU information (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Unable to retreive the CCU information", e, LOGFATAL) ;
  }

  // End of the form
  *out << cgicc::form() << std::endl;
  
  // Few remarks
  *out << cgicc::h2("Few remarks") << std::endl ;
  *out << "<lu>" << std::endl ;
  *out << "<lu>" << std::endl ;
  
  *out << cgicc::html() << std::endl;
}

/** Display the connections inside the database
 */
void TkConfigurationDb::applyCCUConfiguration (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Check if it is a partition name choose or it was already chosen
    if (partitionDbName_ == "") {
      partitionDbName_ = cgi["PartitionName"]->getValue() ;
      downloadAllDevicesFromDb() ;
      this->displayCCUConfiguration(in,out) ;
    }
    else {
      try {
	// Retreive the rings
	tkringVector allRings = deviceFactory_->getDbRingDescription(partitionDbName_);
	tkringVector newRings ;
	
	if (allRings.size()) {
	  for (tkringVector::iterator ringIt = allRings.begin() ; ringIt != allRings.end() ; ringIt ++) {

	    // Clone the ring
	    TkRingDescription *tkRingDescription = (*ringIt)->clone() ;
	    bool changed = false ;

	    // Input
	    std::stringstream nameInput ;
	    nameInput << "RingInput" << (*ringIt)->getFecHardwareId() << (*ringIt)->getRingSlot() ;
	    std::string ringInput = cgi[nameInput.str()]->getValue() ;

	    // std::cout << nameInput.str() << ": " << ringInput << " " << (*ringIt)->getInputAUsed() << std::endl ;

	    if ( (ringInput == "A") && !(*ringIt)->getInputAUsed() ) {
	      tkRingDescription->setInputAUsed() ;
	      changed = true ;
	    }
	    if ( (ringInput == "B") && (*ringIt)->getInputAUsed() ) {
	      tkRingDescription->setInputBUsed() ;
	      changed = true ;
	    }

	    // Output
	    std::stringstream nameOutput ;
	    nameOutput << "RingOutput" << (*ringIt)->getFecHardwareId() << (*ringIt)->getRingSlot() ;
	    std::string ringOutput = cgi[nameOutput.str()]->getValue() ;

	    // std::cout << nameOutput.str() << ": " << ringOutput << " " << (*ringIt)->getOutputAUsed() << std::endl ;

	    if ( (ringOutput == "A") && !(*ringIt)->getOutputAUsed() ) {
	      tkRingDescription->setOutputAUsed() ;
	      changed = true ;
	    }
	    if ( (ringOutput == "B") && (*ringIt)->getOutputAUsed() ) {
	      tkRingDescription->setOutputBUsed() ;
	      changed = true ;
	    }

	    // for each CCU on the rings
	    std::vector<CCUDescription *> *ccuVector = (*ringIt)->getCcuVector( ) ;
	    std::vector<CCUDescription *> newCCUs ;
	    if ( (ccuVector != NULL) && ccuVector->size() ) {
	      for (std::vector<CCUDescription *>::iterator ccuIt = ccuVector->begin()  ; ccuIt != ccuVector->end() ; ccuIt ++) {

		CCUDescription *ccu = (*ccuIt)->clone() ;

		std::stringstream ccuInputName ;
		ccuInputName << "CCUInput" << (*ringIt)->getFecHardwareId() << (*ringIt)->getRingSlot() << getCcuKey((*ccuIt)->getKey()) ;
		std::string ccuInput = cgi[ccuInputName.str()]->getValue() ;
		if ( (ccuInput == "Enabled") && !(*ccuIt)->getEnabled() ) {
		  changed = true ;
		  ccu->setEnabled(true) ;
		}
		else if ( (ccuInput == "Disabled") && (*ccuIt)->getEnabled() ) {
		  changed = true ;
		  ccu->setEnabled(false) ;
		}

//		// std::cout << ccuInputName.str() << ": " << ccuInput << " " << (*ccuIt)->getInputA() << std::endl ;
// 		if ( (ccuInput == "A") && !(*ccuIt)->getInputA()) {
// 		  changed = true ;
// 		  ccu->setInputA() ;
// 		}
// 		if ( (ccuInput == "B") && (*ccuIt)->getInputA()) {
// 		  changed = true ;
// 		  ccu->setInputB() ;
// 		}
// 		std::stringstream ccuOutputName ;
// 		ccuOutputName << "CCUOutput" << (*ringIt)->getFecHardwareId() << (*ringIt)->getRingSlot() << getCcuKey((*ccuIt)->getKey()) ;
// 		std::string ccuOutput = cgi[ccuOutputName.str()]->getValue() ;
// 		// std::cout << ccuOutputName.str() << ": " << ccuOutput << " " << (*ccuIt)->getOutputA() << std::endl ;
// 		if ( (ccuOutput == "A") && !(*ccuIt)->getOutputA()) {
// 		  changed = true ;
// 		  ccu->setOutputA() ;
// 		}
// 		if ( (ccuOutput == "B") && (*ccuIt)->getOutputA()) {
// 		  changed = true ;
// 		  ccu->setOutputB() ;
// 		}

		newCCUs.push_back(ccu) ;
	      }
	    }
	    // else no change

	    if (changed) {
	      tkRingDescription->setCcuVector(newCCUs) ;
	      newRings.push_back(tkRingDescription) ;
	    }
	    else {
	      delete tkRingDescription ;
	      FecFactory::deleteVectorI(newCCUs) ;
	    }
	  }

	  // Display the result
	  if (newRings.size() != 0) {

// 	    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
// 	    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
// 	    *out << cgicc::title("Strip Tracker Configuration Database Interface: CCU reconfiguration") << std::endl;
// 	    xgi::Utils::getPageHeader(*out, "CCU configuration uploaded to database (only for information)");
// 	    displayDatabaseRelatedLinks(in,out) ;
// 	    displayRingHTMLTable ( out, newRings, true) ;
	    
	    for (tkringVector::iterator ringIt = newRings.begin() ; ringIt != newRings.end() ; ringIt ++) {
	      try {
		deviceFactory_->setDbRingDescription(partitionDbName_,*(*ringIt)) ;
	      }
	      catch (FecExceptionHandler &e) {
		*out << cgicc::h2("Problem during the upload of ring into database for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	      }
	      catch (oracle::occi::SQLException &e) {
		*out << cgicc::h2("Problem during the upload of ring into database for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	      }
	    }
	    
//	    *out << cgicc::html() << std::endl;
	    FecFactory::deleteVectorI(newRings) ;
	  }
	}

	this->displayCCUConfiguration(in,out) ;

      }
      catch (FecExceptionHandler &e) {
	*out << cgicc::h2("No ring configuration found for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      catch (oracle::occi::SQLException &e) {
	*out << cgicc::h2("No ring configuration found for partition " + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      
      //this->displayCCUConfiguration(in,out) ;
    }
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2(e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Problem in accessing the modules in database", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2(e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Problem in accessing the modules in database", e, LOGFATAL) ;
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}


/** Display a popug in order to have an idea of the time needed for the operation
 */
void TkConfigurationDb::displayPopupWindowForEnableDisable(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration DB Parameters") << std::endl;
  xgi::Utils::getPageHeader(*out, "Strip Tracker Configuration DB Parameters") ;

  *out << cgicc::h2("This operation will take some times, please be patient").set("style","font-size: 15pt;  font-family: arial; color: rgb(0, 0, 0); font-weight: bold;") << std::endl;

  //xgi::Utils::getPageFooter(*out);
  *out << cgicc::html() << std::endl;
}
// ------------------------------------------------------------------------------------------------------------ //
// Related links
// ------------------------------------------------------------------------------------------------------------ //

/** Parameter page
 */
void TkConfigurationDb::displayMoreParameters(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("displayMoreParameters", LOGDEBUG) ;

  linkPosition_ = 1 ;

  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration DB Parameters") << std::endl;
  xgi::Utils::getPageHeader(*out, "Strip Tracker Configuration DB Parameters") ;

  // Check the db version and if an errors appears delete the deviceFactory to be recreated
  if ( (deviceFactory_ != NULL) && (deviceFactory_->getDbUsed()) ) {
      
    try {
      dbVersion_ = deviceFactory_->getDbVersion() ;
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Unable to retreive the database version of the database " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the information", e, LOGFATAL) ;
      delete deviceFactory_ ;
      dbVersion_ = 0 ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Unable to retreive the database version from the database (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the database version from the database (Oracle exception)", e, LOGFATAL) ;
      delete deviceFactory_ ;
      dbVersion_ = 0 ;
    }
  }

  // Status bar
  displayRelatedLinks (in,out) ;

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();

  url += "/tkConfigurationDbParameterSettings";	

  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

//   *out << cgicc::p() << cgicc::input().set("type", "submit")
//     .set("name", "submit")
//     .set("value", "Apply");
//   *out << cgicc::p() << std::endl ;

  // --------------------------------------------------------------------
  // Size of all the next fields
  std::string sizeField = "30" ;

  // Retreive all the partition names from DB
  std::stringstream partitionButton ; partitionButton << "<select name=\"partitionDbName\">" ;
  if (deviceFactory_ != NULL) {
    try {
      // Retreive the partition names
      std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNamesFromCurrentState() ; //deviceFactory_->getAllPartitionNames() ;
      if (partitionNames.size()) {
	
	// Build a list of partition name
	for (std::list<std::string>::iterator itPartition = partitionNames.begin() ; itPartition != partitionNames.end() ; itPartition ++) {
	  if (partitionDbName_ == (*itPartition)) 
	    partitionButton << "<option selected>" << (*itPartition) << "</option>";
	  else
	    partitionButton << "<option>" << (*itPartition) << "</option>";
	}
      }
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Unable to retreive the partition name from the database " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the information", e, LOGFATAL) ;
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Unable to retreive the partition name from the database (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the partition name from the database (Oracle exception)", e, LOGFATAL) ;
    }
  }
  partitionButton << "</select>" << std::endl ;

  // --------------------------------------------------------------------
  // General parameters
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("General") << std::endl;
  //*out << cgicc::p() << std::endl ;

  // Display the DB version
  if ( (deviceFactory_ != NULL) && (deviceFactory_->getDbUsed()) ) 
    *out << "<span style=\"color: rgb(51, 204, 0);\">" << "Database Connected, version " 
	 << "<a href=\"https://twiki.cern.ch/twiki/bin/view/CMS/TkDatabaseVersions\" target=\"_blank\">" 
	 << dbVersion_ << "</a>" << "</span>" << std::endl ;
  else *out << "<span style=\"color: rgb(255, 0, 0);\">" << "Database Not Yet Connected" << "</span>" ;

  *out << cgicc::p() << std::endl ;

  // refresh cache if needed
  if ( (deviceFactory_ != NULL) && (deviceFactory_->getDbUsed()) ) {
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","refreshCacheSystem") << std::endl;
    std::stringstream labelToBeDisplayed ; 
    labelToBeDisplayed << "Refresh cache for FEC/FED and connections" << "<span style=\"color: rgb(255, 0, 0);\">(This operation may take a long time (10 to 30 min)</span>" ;
    *out << cgicc::label(labelToBeDisplayed.str())  << std::endl;
  }

  *out << cgicc::p() << std::endl ;

  // Display debug messages
  if (displayDebugMessage_)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","displayDebugMessage").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
      .set("name","displayDebugMessage") << std::endl;
  *out << cgicc::label("Display debug messages (filter by the diagnostic system)")  << std::endl;

  *out << cgicc::fieldset() << std::endl;

  *out << cgicc::p() << std::endl ;

  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::legend("Database Parameters") << std::endl;
  // Database Parameters
  // First check dyanmically if the values are set
  std::string dblogin = "nil", dbpass  = "nil", dbpath = "nil" ;
  DbFecAccess::getDbConfiguration (dblogin, dbpass, dbpath) ;

  // Database login ...
  if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
      (dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) databaseAccess_ = false ;
    
  *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  *out << "<tr>" << std::endl ;
  //*out << "<td>"; *out << "</td>" << std::endl;
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
  //*out << "<td>"; *out << "</td>" << std::endl;
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
  //*out << "<td>"; *out << "</td>" << std::endl;
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
  //*out << "<td>"; *out << "</td>" << std::endl;
  *out << "<td>"; 
  *out << cgicc::label("Partition Name ") << std::endl;
  *out << "</td>" << std::endl;
  if (deviceFactory_ != NULL) {
    *out << "<td>"; 
    *out << partitionButton.str() << std::endl ;
    *out << "</td>" << std::endl;
  }
  else {
    *out << "<td>"; 
    *out << "<span style=\"color: rgb(255, 0, 0);\">" << "Database Not Yet Connected" << "</span>" ;
    *out << "</td>"; 
  }
  *out << "</tr>" << std::endl;
  *out << cgicc::table() << std::endl;
  *out << cgicc::fieldset() << std::endl;    
  
  // End of Configuration
  // --------------------------------------------------------------------

  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply");

  // Few remarks
  *out << cgicc::h2("The partition can be changed through this menu since you connect the database") << std::endl ;
  *out << cgicc::h2("If your partition does not appears after you connect the database, it means that it is disable") << std::endl ;
  *out << "<lu>" << std::endl ;
  *out << "<li>" << "Click on " << "<a href=\"/" << getApplicationDescriptor()->getURN() << "/displayRunInformation\">Run Information</a>" << " to check the version you had in the latest run made with that partition" << "</li>" << std::endl ;
  *out << "<li>" << "Click on " << "<a href=\"/" << getApplicationDescriptor()->getURN() << "/StateManagement\">Partition/Version</a>" << " to re-enable the partition with the version given by the run table and add a new state name." << "</li>" << std::endl ;
  *out << "<li>" << "NEVER revert to a previous state with the state name, you will affect the other partitions" << "</li> "<< std::endl ;
  *out << "</lu>" << std::endl ;
  // End of the form
  *out << cgicc::form() << std::endl;

  //xgi::Utils::getPageFooter(*out);
  *out << cgicc::html() << std::endl;
}

/** Initialise action or apply on the parameters
 */
void TkConfigurationDb::tkConfigurationDbParameterSettings ( xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport ("tkConfigurationDbParameterSettings", LOGDEBUG) ;
  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Display the debug messages
    displayDebugMessage_ = cgi.queryCheckbox("displayDebugMessage") ;

    // Database access    
    std::string dbLogin = cgi["dbLogin"]->getValue();
    std::string dbPasswd = cgi["dbPasswd"]->getValue();
    std::string dbPath = cgi["dbPath"]->getValue();

    if (deviceFactory_ != NULL) {
      //if (partitionDbName_ != cgi["partitionDbName"]->getValue()) {
      if (true) {
	partitionDbName_ = cgi["partitionDbName"]->getValue() ;
	// ----------------------------
	// Delete FEC
	downloadAllDevicesFromDb() ;
	apvDbChanged_ = pllDbChanged_ = aohDbChanged_ = false ;
	currentStateName_ = "" ;

	// -----------------------------
	// Delete FED
	// fedDescriptions_
	// fedDescriptionsEdited_
      }

      if ((deviceFactory_->getDbUsed()) && cgi.queryCheckbox("refreshCacheSystem")) deviceFactory_->refreshCacheXMLClob(1,true) ;
    }

    errorReportLogger_->errorReport ("Display for partition " + partitionDbName_, LOGDEBUG) ;

    if (partitionDbName_ != "") {
      std::ostringstream strProcess ; strProcess << "TkConfigurationDb " << getApplicationDescriptor()->getInstance() << ", partition " << partitionDbName_ ;
      errorReportLogger_->setStrProcess(strProcess.str()) ;
    }

    dbLogin_ = dbLogin ;
    dbPasswd_ = dbPasswd ;
    dbPath_ = dbPath ;
    
    if ((dbLogin_ == "nil") || (dbPasswd_ == "nil") || (dbPath_ == "nil") ||
	(dbLogin_.toString().size() == 0) || (dbPasswd_.toString().size() == 0) || (dbPath_.toString().size() == 0)) {
      
      databaseAccess_ = false ;
      
      // Error reporting
      errorReportLogger_->errorReport ("Cannot create a database access, disable the database access", LOGERROR) ;
    }
    else {
      if ( (deviceFactory_ == NULL) ||
	   (dbLogin != dbLogin_.toString()) || 
	   (dbPasswd != dbPasswd_.toString()) || 
	   (dbPath != dbPath_.toString()) ||
	   (!deviceFactory_->getDbConnected()) ) {

	createDatabaseAccess() ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2("Access problem for the state in the database: " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Error during the access to the database", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2("Access problem for the state in the database (oracle exception): " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Error during the access to the database", e, LOGFATAL) ;
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }

  this->Default(in,out);
  //this->displayInitialisePage(in,out) ;
}

// ------------------------------------------------------------------------------------------------------------ //
// Failure
// ------------------------------------------------------------------------------------------------------------ //

/** Failure page
 */
void TkConfigurationDb::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) {

  //errorReportLogger_->errorReport ("failedTransition", LOGDEBUG) ;

  toolbox::fsm::FailedEvent & fe = dynamic_cast< toolbox::fsm::FailedEvent&>(*e);
  std::stringstream msgError ; msgError << "Failure occurred when performing transition from: "  
					 << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what() ;
  errorReportLogger_->errorReport (msgError.str(), LOGERROR) ;
}

// ---------------------------------------------------------------------------------------------------------
//
// Database display for FED
//
// ---------------------------------------------------------------------------------------------------------

/** Display all the possible states
 */
void TkConfigurationDb::displayFEDManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  linkDbPosition_ = 4 ;

  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
  xgi::Utils::getPageHeader(*out, "Strip Tracker Configuration Database Interface");

  displayDatabaseRelatedLinks(in,out) ;

  if ((deviceFactory_ != NULL) && (deviceFactory_->getDbConnected())) {
    
    // Form and action
    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    url += "/ApplyFEDManagement";
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;

    try {
      Fed9U::Fed9UMessage<Fed9U::Fed9UDebugLevel>::smLevel = Fed9U::FED9U_DEBUG_LEVEL_INTERMEDIATE;

      if (partitionDbName_ != "") {
	
	// <JRF> - ADDED code here 30/6/2006
	// Here retreive the list of the FED
	
	std::vector<Fed9U::Fed9UDescription*>* tempDescvectorPtr;	

	tempDescvectorPtr = deviceFactory_->getFed9UDescriptions(partitionDbName_);

	// prepare the memeber variable vector of fed descriptions
	// delete any old descriptions and clear the vector
	
	for (Fed9U::Fed9UHashMapType::iterator p=fedDescriptions_.begin();p!=fedDescriptions_.end();p++)
	  delete p->second ;
	fedDescriptions_.clear();
	fedDescriptionsEdited_.clear();

	// loop over the temp vector of descriptions and copy them into the member variable hash map
	for ( std::vector<Fed9U::Fed9UDescription*>::iterator it = tempDescvectorPtr->begin() ; it != tempDescvectorPtr->end() ; it ++) {
	  Fed9U::Fed9UDescription * tmp = new Fed9U::Fed9UDescription();
	  *tmp = **it; 
	  
	  fedDescriptions_[tmp->getFedId()] = tmp;
	  fedDescriptionsEdited_[tmp->getFedId()] = false;
	}

	// -------------------- here the changes
	// </JRF>

	// Dummy example
	//std::list<unsigned int> listFedId ;
	//listFedId.push_back(1) ;
	//listFedId.push_back(2) ;
	//listFedId.push_back(3) ;

	if (fedDescriptions_.size() > 0) {
	  std::stringstream hrefdest;
	  hrefdest << "/" << getApplicationDescriptor()->getURN() << "/displayFEDModeParameters";
	  hrefdest.flush();

	  *out << cgicc::p() << cgicc::a("[Edit Fed Mode Parameters]").set("href",hrefdest.str()).set("target","_blank") << cgicc::p() <<std::endl ;
	  // Display the FEDs in a list
	  *out << cgicc::table().set("border","1").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << std::endl ;
	  *out << "<th>" << "FED ID" << "</th>" ;
	  *out << "<th>" << "FED Hardware ID" << "</th>" ;
	  *out << cgicc::tr() << std::endl ;
	  for (Fed9U::Fed9UHashMapType::iterator it=fedDescriptions_.begin();it!=fedDescriptions_.end();it++) {
	   	    	    
	    unsigned int fedid = (it->second)->getFedId() ;
	    unsigned int fedhardwareid = (it->second)->getFedHardwareId();

	    *out << cgicc::tr() << std::endl ;
	    *out << cgicc::td() << "<a href=\"/" << getApplicationDescriptor()->getURN()
		 << "/displayFEDParameters?param1=" << fedid 
		 << "\" target=\"_blank\"> FED " << fedid << "</a>" << cgicc::td() << std::endl ;
	    *out << cgicc::td() << "<a href=\"/" << getApplicationDescriptor()->getURN()
		 << "/displayFEDParameters?param1=" << fedid 
		 << "\" target=\"_blank\"> FED " << fedhardwareid << "</a>" << cgicc::td() << std::endl ;

	    *out << cgicc::tr() << std::endl ;
	  }
	  *out << cgicc::table() << std::endl ;
	  
	  // Set it in the database if needed or save to file
	  *out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	       << "Please check one of these boxes once you have performed all modifications for a Partition: " 
	       << "</big></big></span>" << std::endl ;
	  *out << cgicc::p()  << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Dump to files: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","DumpToFiles") << cgicc::td() << std::endl ;
	  *out << cgicc::p()  << std::endl ;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Create a major version: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CreateMajorVersion") << cgicc::td() << std::endl ;
	  *out << cgicc::tr() << std::endl ;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Create a minor version: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","CreateMinorVersion") << cgicc::td() << std::endl ;
	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Propagate the fiber delay in FEDs (> 8 min): " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","UpdateFiberDelay") << cgicc::td() << std::endl ;
	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::tr() << cgicc::td() << cgicc::td() << std::endl ;
	  *out << cgicc::td() << "<big>" << "Reset the modifications already performed: " << "</big>" << cgicc::td() << cgicc::td() << cgicc::input().set("type", "radio").set("name","actionToBeApply").set("value","ResetModifications").set("checked","true") << cgicc::td() << std::endl ;
	  *out << cgicc::tr() << std::endl;
	  *out << cgicc::table() << std::endl ;
	}
	else {
	  *out << cgicc::h2(std::string("No FED found for the partition ") + partitionDbName_).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
	}
      }
      else {

	// Retreive the partition names
	std::list<std::string> partitionNames = deviceFactory_->getAllPartitionNames() ;
	if (partitionNames.size()) {
	  
	  // Build a list of partition name
	  std::stringstream partitionButton ; partitionButton << "<select name=\"PartitionName\">" ;
	  for (std::list<std::string>::iterator itPartition = partitionNames.begin() ; itPartition != partitionNames.end() ; itPartition ++) {
	    if (partitionDbName_ == (*itPartition)) 
	      partitionButton << "<option selected>" << (*itPartition) << "</option>";
	    else
	      partitionButton << "<option>" << (*itPartition) << "</option>";
	  }
	  partitionButton << "</select>" << std::endl ;
	  
	  *out << cgicc::p() << "<big><big><span style=\"font-weight: bold;\">" 
	       << "Please select the partition then click Apply: " << "</big></big></span>"
	       << partitionButton.str() << std::endl ;
	}
	else *out << cgicc::h2("No Partition found in the database").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
    }
    catch (oracle::occi::SQLException &e) {
      *out << cgicc::h2("Unable to retreive the DCU from the database (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Unable to retreive the DCU from the database", e, LOGFATAL) ;
    }
    catch (FecExceptionHandler &e) {
      *out << cgicc::h2("Problem in accessing the modules in database " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      errorReportLogger_->errorReport ("Problem in accessing the modules in database", e, LOGFATAL) ;
    }
    catch(const std::exception& e) {
      errorReportLogger_->errorReport (e.what(), LOGERROR) ;
      *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      XCEPT_RAISE(xgi::exception::Exception,  e.what());
    }

    // Apply
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Apply");

    // End of the form
    *out << cgicc::form() << std::endl;
    
    // Few remarks
    *out << cgicc::h2("Few remarks") << std::endl ;
    *out << "<lu>" << std::endl ;
    if (partitionDbName_ != "") {
      *out << "<li>" << "By clicking on the FED link, you can display and change the FED parameters" << "</li>" << std::endl ;
      *out << "<li>" << "Once you have performed all the changed needed on the parameters, you can Apply, do not click on it before all the operations has been done on all the modules you may want to do" << "</li>" << std::endl ;
    }
    else {
      *out << "<li>" << "The list of the FEDs will be displayed after you click Apply" << "</li>" << std::endl ;
    }
    *out << "<li>" << "If you click again on the link " 
	 << "<a href=\"/%s/MoreParameters\">[More Parameters]</a>"
	 << ", you can choose a different partition" << "</li>" << std::endl ;
    *out << "<lu>" << std::endl ;
  }
  else {
    *out << cgicc::h2("Please specify a database to use these links").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;      
  }
  std::cout << "about to write /HTML" << std::endl;
  *out << "</HTML>" << std::endl ;
  std::cout << "written it!" << std::endl;

}


/** Apply the state change done in the method display
 */
void TkConfigurationDb::applyFEDManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Check if it is a partition name choose or it was already chosen
    if (partitionDbName_ == "") {
      partitionDbName_ = cgi["PartitionName"]->getValue() ;
      displayFEDManagement(in,out) ;
    }
    else {
      *out << "<HTML>" << std::endl ;
      *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
      *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
      *out << cgicc::title("Strip Tracker Configuration Database Interface") << std::endl;
      xgi::Utils::getPageHeader(*out, "Strip Tracker Configuration Database Interface");
      
      displayDatabaseRelatedLinks(in,out) ;

      // Depending of the action: "actionToBeApply", "NoAction", "CreateMajorVersion", "CreateMinorVersion", "ResetModifications"
      bool dumpToFiles = false ;
      if (cgi["actionToBeApply"]->getValue() == "DumpToFiles") dumpToFiles = true ;
      bool createMajorVersion = false ;
      if (cgi["actionToBeApply"]->getValue() == "CreateMajorVersion") createMajorVersion = true ;
      bool createMinorVersion = false ;
      if (cgi["actionToBeApply"]->getValue() == "CreateMinorVersion") createMinorVersion = true ;
      bool resetModifications = false ;
      if (cgi["actionToBeApply"]->getValue() == "ResetModifications") resetModifications = true ;
      bool updateFiberDelay = false ;
      if (cgi["actionToBeApply"]->getValue() == "UpdateFiberDelay") { updateFiberDelay = true ; createMajorVersion = true ; }

      // -------------------- here the changes
      // ---------------------------------------------------------------------
      // Manage the creation of the version inside the database
      // Upload of versions
      // ---------------------------------------------------------------------
      // -------------------- here the changes

      // Create major or minor version
      if (!resetModifications) {
	
	std::cout << "about to upload!" << std::endl;
	Fed9U::u16 versionMajor,versionMinor;
	std::vector<Fed9U::Fed9UDescription*> tempVector;	
	//Loop over all descriptions in cache
	std::stringstream fileName;
	for (Fed9U::Fed9UHashMapType::iterator it=fedDescriptions_.begin();it!=fedDescriptions_.end();it++) {
	  // if we have to save to file, we save all descriptions to file regardless if they have been changed or not
	  if (dumpToFiles) {
	    //save descriptions to file here one at a time.
	    fileName.str("");
	    fileName.clear();	    
	    fileName << "FedDescriptionCrate" << static_cast<Fed9U::u16>((it->second)->getCrateNumber())<< "Slot"<< static_cast<Fed9U::u16>((it->second)->getSlotNumber()) <<".xml";
	    fileName.flush();
	    cout << "filename = " << fileName.str() << endl;
	    deviceFactory_->setUsingFile(true);
	    deviceFactory_->setOutputFileName(fileName.str());
	    deviceFactory_->setFed9UDescription(*(it->second), &versionMajor, &versionMinor, 1);
	  } 
	  else {
	    std::cout << "making vector for upload!" << std::endl;
	    // if we are not saving to file then we are uploading to db
	    // if we are uploading a minor version then we only upload for edited descritions
	    // if we are uploading a major version, then we need to upload all descriptions regardless
	    // of whether they have been updated or not.
	    //if the description was edited or it is a major version upload, add it to the list
	    if ( fedDescriptionsEdited_[(it->second)->getFedId()] || (createMajorVersion == true) ) {
	      tempVector.push_back(it->second);
	    }
	  }
	}

	//note that if the vector is empty then we are saving to file so don't need to uploa
	if ( tempVector.size() == 1 ) {
	  if (deviceFactory_->getDbUsed() ) {
	    deviceFactory_->setFed9UDescription(*tempVector[0],partitionDbName_, &versionMajor, &versionMinor, createMajorVersion?1:0);
	    cout << std::dec << "Uploaded to version = " << versionMajor << "." << versionMinor << "Major Version?" << (createMajorVersion?"true":"false")<<  endl;
	  }
	  else {
	    deviceFactory_->setFed9UDescription(*tempVector[0], &versionMajor, &versionMinor, createMajorVersion?1:0);
	    
	  }
	}
	else if ( tempVector.size() > 1 ) { //note that if the vector is empty then we are saving to file so don't need to upload
	  
	  if (deviceFactory_->getDbUsed() ) {
	    deviceFactory_->setFed9UDescriptions(tempVector, partitionDbName_,&versionMajor, &versionMinor, createMajorVersion?1:0 );
	    cout << std::dec << "Uploaded multiple feds to version = " << versionMajor << "." << versionMinor << "Major Version?" << (createMajorVersion?"true":"false")<<  endl;
	  }
	  else {
	    deviceFactory_->setFed9UDescriptions(tempVector,std::string("null"),&versionMajor, &versionMinor, createMajorVersion?1:0 );
	    
	  }
	}

	// update the fiber delays if asked
	if (updateFiberDelay) {
	  unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  deviceFactory_->updateChannelDelays(partitionDbName_) ;
	  unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
	  unsigned long updateChannelTime = (endMillis-startMillis) ;
	  *out << cgicc::h2("Update of the fiber delay in FED delay was done correctly in " + toString(updateChannelTime) + " ms for the partition " + partitionDbName_) << std::endl ;
	}
      }
      
      deviceFactory_->setUsingFile(false);
	   
      // ----------------------------------------------------------------------
      // Delete the cache here
      // ----------------------------------------------------------------------
      for (Fed9U::Fed9UHashMapType::iterator p=fedDescriptions_.begin();p!=fedDescriptions_.end();p++)
	delete p->second ;
      fedDescriptions_.clear();
      fedDescriptionsEdited_.clear();
      
      *out << cgicc::h2("The FED cache is now clear to continue editing you must download again").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl ;
      *out << "</HTML>" << std::endl ;       
    }
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h2("Problem in accessing the modules in database " + e.what()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Problem in accessing the modules in database", e, LOGFATAL) ;
  }
  catch (oracle::occi::SQLException &e) {
    *out << cgicc::h2("Problem in accessing the modules in database (Oracle exception) " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport ("Problem in accessing the modules in database", e, LOGFATAL) ;
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

/** Display the parameters for the FED
 * param1 is the FED hardware ID
 */
void TkConfigurationDb::displayFEDParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  try {

    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);
    unsigned int fedId = fromString<unsigned int>(cgi["param1"]->getValue()) ;

    displayFEDParameters(in,out,fedId) ;
  }
  catch (const std::exception& e) {
    *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }
}

/** Display the parameters for the FED in case of re-display
 */
void TkConfigurationDb::displayFEDModeParameters (xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  try {
    
    
    // ----------------------------------------------------------------------
    // Start the WEB page here
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Set Fed Running Modes") << std::endl;

    xgi::Utils::getPageHeader(*out, "Set Fed Running Modes");

   
    // Form and action
    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    url += "/ApplyFEDModeParameters";
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;
    // Just to have the FED ID
    *out << cgicc::h2().set("style","font-size: 10pt;  font-family: arial; color: rgb(0, 0, 140); font-weight: bold;") << "Use the following form to select the mode of running for all FEDs in this partition:" << cgicc::h2() << std::endl ;
        
    Fed9U::Fed9UClockSource clock = fedDescriptions_.begin()->second->getClock();
    Fed9U::Fed9UTrigSource trigger = fedDescriptions_.begin()->second->getTriggerSource();
    Fed9U::Fed9UDaqMode mode = fedDescriptions_.begin()->second->getDaqMode();
    Fed9U::Fed9UDaqSuperMode superMode = fedDescriptions_.begin()->second->getDaqSuperMode();
    Fed9U::Fed9UReadRoute readRoute = fedDescriptions_.begin()->second->getBeFpgaReadRoute();
    *out << cgicc::input().set("type", "submit").set("value","Apply") << "<br><br>" << std::endl;

    *out << "Reenable all channels on all FEDs <br>" << cgicc::input().set("type","checkbox").set("name","reenableAllChansAllFeds") << "<br><br>" << std::endl;
	
 
    *out << "Clock Source<br><select name=\"clock\">" 
	 << "<option value=\"INTERNAL\"" << (clock==Fed9U::FED9U_CLOCK_INTERNAL?"SELECTED":"") << ">INTERNAL</option>"
	 << "<option value=\"BACKPLANE\"" << (clock==Fed9U::FED9U_CLOCK_BACKPLANE?"SELECTED":"") << ">BACKPLANE</option>"
	 << "<option value=\"TTC\"" << (clock==Fed9U::FED9U_CLOCK_TTC?"SELECTED":"") << ">TTC</option>"
	 << "</select><br><br>"
	 << std::endl;

    *out << "Trigger Source<br><select name=\"trigger\">" 
	 << "<option value=\"SOFTWARE\"" << (trigger==Fed9U::FED9U_TRIG_SOFTWARE?"SELECTED":"") << ">SOFTWARE</option>"
	 << "<option value=\"BACKPLANE\"" << (trigger==Fed9U::FED9U_TRIG_BACKPLANE?"SELECTED":"") << ">BACKPLANE</option>"
	 << "<option value=\"TTC\"" << (trigger==Fed9U::FED9U_TRIG_TTC?"SELECTED":"") << ">TTC</option>"
	 << "</select><br><br>"
	 << std::endl;

    *out << "Mode<br><select name=\"mode\">" 
	 << "<option value=\"SCOPE\"" << (mode==Fed9U::FED9U_MODE_SCOPE?"SELECTED":"") << ">SCOPE</option>"
	 << "<option value=\"VIRG\"" << (mode==Fed9U::FED9U_MODE_VIRGIN_RAW?"SELECTED":"") << ">VIRGIN RAW</option>"
	 << "<option value=\"PROC\"" << (mode==Fed9U::FED9U_MODE_PROCESSED_RAW?"SELECTED":"") << ">PROCESSED RAW</option>"
	 << "<option value=\"ZERO\"" << (mode==Fed9U::FED9U_MODE_ZERO_SUPPRESSED?"SELECTED":"") << ">ZERO SUPPRESSED</option>"
	 << "</select><br><br>"
	 << std::endl;
    
    *out << "Super Mode<br><select name=\"superMode\">" 
	 << "<option value=\"NORMAL\"" << (superMode==Fed9U::FED9U_SUPER_MODE_NORMAL?"SELECTED":"") << ">NORMAL</option>"
	 << "<option value=\"FAKE\"" << (superMode==Fed9U::FED9U_SUPER_MODE_FAKE?"SELECTED":"") << ">FAKE</option>"
	 << "<option value=\"FAKEZLITE\"" << (superMode==Fed9U::FED9U_SUPER_MODE_FAKE_ZERO_LITE?"SELECTED":"") << ">FAKE Z-LITE</option>"
	 << "<option value=\"NORMALZLITE\"" << (superMode==Fed9U::FED9U_SUPER_MODE_ZERO_LITE?"SELECTED":"") << ">NORMAL Z-LITE</option>"
	 << "</select><br><br>"
	 << std::endl;

   *out << "Read Route<br><select name=\"readRoute\">" 
	 << "<option value=\"VME\"" << (readRoute==Fed9U::FED9U_ROUTE_VME?"SELECTED":"") << ">VME</option>"
	 << "<option value=\"SLINK\"" << (readRoute==Fed9U::FED9U_ROUTE_SLINK64?"SELECTED":"") << ">SLINK</option>"
	 << "</select><br><br>"
	 << std::endl;

   Fed9U::u32 feFirmware = fedDescriptions_.begin()->second->getFeFirmwareVersion();
  Fed9U::u32 beFirmware = fedDescriptions_.begin()->second->getBeFirmwareVersion();
  Fed9U::u32 delayFirmware = fedDescriptions_.begin()->second->getDelayFirmwareVersion();
  Fed9U::u32 vmeFirmware = fedDescriptions_.begin()->second->getVmeFirmwareVersion();
  *out << "Front End Firmware<br> <input type=\"text\" name=\"feFirmware\" value=\"" << std::hex << feFirmware << "\">" << "<br><br>"<< std::endl;
  *out << "Back End Firmware<br> <input type=\"text\" name=\"beFirmware\" value=\"" << std::hex << beFirmware << "\">" << "<br><br>"<< std::endl;
  *out << "Delay Firmware<br> <input type=\"text\" name=\"delayFirmware\" value=\"" << std::hex << delayFirmware << "\">" << "<br><br>"<< std::endl;
  *out << "VME Firmware<br> <input type=\"text\" name=\"vmeFirmware\" value=\"" << std::hex << vmeFirmware << "\">" << "<br><br>"<< std::endl;

   // Apply
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Apply");
    
    // End of the form
    *out << cgicc::form() << std::endl;
    
    // Few remarks
    *out << cgicc::h2("A few remarks") << std::endl ;
    *out << "<lu>" << std::endl ;
    *out << "<li>" << "Please make all the changes and go back to the main web page to apply the modifications. If you have any problems with this software please send hatemail to Jonathan Fulcher as often as possible!!! if you see him in the street you can call him nasty names too!" << "</li>" << std::endl ;
    *out << "</lu>" << std::endl ;
    
    // - End of HTML
    *out << cgicc::html() << std::endl ;
  }
  catch (const std::exception& e) {
    *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

void TkConfigurationDb::applyFEDModeParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {
  try {

    cgicc::Cgicc cgi(in);
    std::vector<cgicc::FormEntry> uglycgicc;
    bool reenableAll = (cgi.getElement("reenableAllChansAllFeds", uglycgicc));
    Fed9U::Fed9UAddress fedAddress;
    for (Fed9U::Fed9UHashMapType::iterator it=fedDescriptions_.begin();it!=fedDescriptions_.end();it++) {
      
      fedDescriptionsEdited_[it->second->getFedId()] = true;

      // first we check if we are reenabling all FED channels
      if( reenableAll ) {
	for ( int i = 0 ; i < Fed9U::FEUNITS_PER_FED ; i++) {
	  fedAddress.setFedFeUnit(i);
	  it->second->setFedFeUnitDisable(fedAddress, false );
	  for ( int j = 0 ; j < Fed9U::CHANNELS_PER_FEUNIT ; j++) {
	  fedAddress.setFeUnitChannel(j);
	    for (int k = 0 ; k < Fed9U::APVS_PER_CHANNEL ; k++) {
	      fedAddress.setChannelApv(k);
	      it->second->setApvDisable(fedAddress, false );	    
	    }
	  }
	}
      }

      
    
      if (cgi["clock"]->getValue()=="INTERNAL") {
	it->second->setClock(Fed9U::FED9U_CLOCK_INTERNAL);
      }
      else if (cgi["clock"]->getValue()=="BACKPLANE") {
	it->second->setClock(Fed9U::FED9U_CLOCK_BACKPLANE);
      }
      else if (cgi["clock"]->getValue()=="TTC") {
	it->second->setClock(Fed9U::FED9U_CLOCK_TTC);
      }

      if (cgi["trigger"]->getValue()=="SOFTWARE") {
	it->second->setTriggerSource(Fed9U::FED9U_TRIG_SOFTWARE);
      }
      else if (cgi["trigger"]->getValue()=="BACKPLANE") {
	it->second->setTriggerSource(Fed9U::FED9U_TRIG_BACKPLANE);
      }
      else if (cgi["trigger"]->getValue()=="TTC") {
	it->second->setTriggerSource(Fed9U::FED9U_TRIG_TTC);
      }

      if (cgi["mode"]->getValue()=="ZERO") {
	it->second->setDaqMode(Fed9U::FED9U_MODE_ZERO_SUPPRESSED);
      }
      else if (cgi["mode"]->getValue()=="PROC") {
	it->second->setDaqMode(Fed9U::FED9U_MODE_PROCESSED_RAW);
      }
      else if (cgi["mode"]->getValue()=="VIRG") {
	it->second->setDaqMode(Fed9U::FED9U_MODE_VIRGIN_RAW);
      }
      else if (cgi["mode"]->getValue()=="SCOPE") {
	it->second->setDaqMode(Fed9U::FED9U_MODE_SCOPE);
      }


      if (cgi["superMode"]->getValue()=="FAKE") {
	it->second->setDaqSuperMode(Fed9U::FED9U_SUPER_MODE_FAKE);
      }
      else if (cgi["superMode"]->getValue()=="NORMAL") {
	it->second->setDaqSuperMode(Fed9U::FED9U_SUPER_MODE_NORMAL);
      }
      else if (cgi["superMode"]->getValue()=="NORMALZLITE") {
	it->second->setDaqSuperMode(Fed9U::FED9U_SUPER_MODE_ZERO_LITE);
      }
      else if (cgi["superMode"]->getValue()=="FAKEZLITE") {
	it->second->setDaqSuperMode(Fed9U::FED9U_SUPER_MODE_FAKE_ZERO_LITE);
      }

      if (cgi["readRoute"]->getValue()=="VME") {
	it->second->setBeFpgaReadRoute(Fed9U::FED9U_ROUTE_VME);
      }
      else if (cgi["readRoute"]->getValue()=="SLINK") {
	it->second->setBeFpgaReadRoute(Fed9U::FED9U_ROUTE_SLINK64);
      }
      
      // <NAC date="30/05/2007"> firmware versions
      char* pend;
      it->second->setFeFirmwareVersion(strtoul(cgi["feFirmware"]->getValue().c_str(),&pend,16));
      it->second->setBeFirmwareVersion(strtoul(cgi["beFirmware"]->getValue().c_str(),&pend,16));
      it->second->setDelayFirmwareVersion(strtoul(cgi["delayFirmware"]->getValue().c_str(),&pend,16));
      it->second->setVmeFirmwareVersion(strtoul(cgi["vmeFirmware"]->getValue().c_str(),&pend,16));
      // </NAC>
  

    }	  
    
    displayFEDModeParameters (in,out) ;
  
  }
  catch (const std::exception& e) {
    *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

/** Display the parameters for the FED in case of re-display
 */
void TkConfigurationDb::displayFEDParameters (xgi::Input * in, xgi::Output * out, unsigned int fedId) throw (xgi::exception::Exception) {

  
  std::stringstream msgInfo ;
  msgInfo << "FED " << fedId << std::endl ;

  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title(msgInfo.str()) << std::endl;
  xgi::Utils::getPageHeader(*out, msgInfo.str());
  
  // Form and action
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/ApplyFEDParameters";
  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;
    // Just to have the FED ID
  *out << cgicc::h2().set("style","font-size: 10pt;  font-family: arial; color: rgb(0, 0, 140); font-weight: bold;") << "use the checkboxes to enable FEUnits and/or APVs, to disable a channel disable both APVs in that channel. To reeable all, check the reenable all box and then submit." << cgicc::h2() << std::endl ;
  *out <<  cgicc::p() << "FED ID: &nbsp;&nbsp;&nbsp;" << cgicc::input().set("type","text").set("size","10").set("readonly", "readonly").set("name","fedId").set("value",toString(fedId)) << cgicc::p() <<  std::endl ;
 
  // -------------------- here the changes
  *out << "Reenable All " << cgicc::input().set("type","checkbox").set("name","reenableAll") 
       << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply") << cgicc::br() <<std::endl ;
  Fed9U::u32 feFirmware = fedDescriptions_[fedId]->getFeFirmwareVersion();
  Fed9U::u32 beFirmware = fedDescriptions_[fedId]->getBeFirmwareVersion();
  Fed9U::u32 delayFirmware = fedDescriptions_[fedId]->getDelayFirmwareVersion();
  Fed9U::u32 vmeFirmware = fedDescriptions_[fedId]->getVmeFirmwareVersion();
  Fed9U::u32 slotNumber = fedDescriptions_[fedId]->getSlotNumber();

    Fed9U::Fed9UClockSource clock = fedDescriptions_[fedId]->getClock();
    Fed9U::Fed9UTrigSource trigger = fedDescriptions_[fedId]->getTriggerSource();
    Fed9U::Fed9UDaqMode mode = fedDescriptions_[fedId]->getDaqMode();
    Fed9U::Fed9UDaqSuperMode superMode = fedDescriptions_[fedId]->getDaqSuperMode();
    Fed9U::Fed9UReadRoute readRoute = fedDescriptions_[fedId]->getBeFpgaReadRoute();
 
    *out << "Clock Source<br><select name=\"clock\">" 
	 << "<option value=\"INTERNAL\"" << (clock==Fed9U::FED9U_CLOCK_INTERNAL?"SELECTED":"") << ">INTERNAL</option>"
	 << "<option value=\"BACKPLANE\"" << (clock==Fed9U::FED9U_CLOCK_BACKPLANE?"SELECTED":"") << ">BACKPLANE</option>"
	 << "<option value=\"TTC\"" << (clock==Fed9U::FED9U_CLOCK_TTC?"SELECTED":"") << ">TTC</option>"
	 << "</select><br><br>"
	 << std::endl;

    *out << "Trigger Source<br><select name=\"trigger\">" 
	 << "<option value=\"SOFTWARE\"" << (trigger==Fed9U::FED9U_TRIG_SOFTWARE?"SELECTED":"") << ">SOFTWARE</option>"
	 << "<option value=\"BACKPLANE\"" << (trigger==Fed9U::FED9U_TRIG_BACKPLANE?"SELECTED":"") << ">BACKPLANE</option>"
	 << "<option value=\"TTC\"" << (trigger==Fed9U::FED9U_TRIG_TTC?"SELECTED":"") << ">TTC</option>"
	 << "</select><br><br>"
	 << std::endl;

    *out << "Mode<br><select name=\"mode\">" 
	 << "<option value=\"SCOPE\"" << (mode==Fed9U::FED9U_MODE_SCOPE?"SELECTED":"") << ">SCOPE</option>"
	 << "<option value=\"VIRG\"" << (mode==Fed9U::FED9U_MODE_VIRGIN_RAW?"SELECTED":"") << ">VIRGIN RAW</option>"
	 << "<option value=\"PROC\"" << (mode==Fed9U::FED9U_MODE_PROCESSED_RAW?"SELECTED":"") << ">PROCESSED RAW</option>"
	 << "<option value=\"ZERO\"" << (mode==Fed9U::FED9U_MODE_ZERO_SUPPRESSED?"SELECTED":"") << ">ZERO SUPPRESSED</option>"
	 << "</select><br><br>"
	 << std::endl;
    
    *out << "Super Mode<br><select name=\"superMode\">" 
	 << "<option value=\"NORMAL\"" << (superMode==Fed9U::FED9U_SUPER_MODE_NORMAL?"SELECTED":"") << ">NORMAL</option>"
	 << "<option value=\"FAKE\"" << (superMode==Fed9U::FED9U_SUPER_MODE_FAKE?"SELECTED":"") << ">FAKE</option>"
	 << "<option value=\"FAKEZLITE\"" << (superMode==Fed9U::FED9U_SUPER_MODE_FAKE_ZERO_LITE?"SELECTED":"") << ">FAKE Z-LITE</option>"
	 << "<option value=\"NORMALZLITE\"" << (superMode==Fed9U::FED9U_SUPER_MODE_ZERO_LITE?"SELECTED":"") << ">NORMAL Z-LITE</option>"
	 << "</select><br><br>"
	 << std::endl;

   *out << "Read Route<br><select name=\"readRoute\">" 
	 << "<option value=\"VME\"" << (readRoute==Fed9U::FED9U_ROUTE_VME?"SELECTED":"") << ">VME</option>"
	 << "<option value=\"SLINK\"" << (readRoute==Fed9U::FED9U_ROUTE_SLINK64?"SELECTED":"") << ">SLINK</option>"
	 << "</select><br><br>"
	 << std::endl;


  *out << "Front End Firmware<br> <input type=\"text\" name=\"feFirmware\" value=\"" << std::hex << feFirmware << "\">" << "<br><br>"<< std::endl;
  *out << "Back End Firmware<br> <input type=\"text\" name=\"beFirmware\" value=\"" << std::hex << beFirmware << "\">" << "<br><br>"<< std::endl;
  *out << "Delay Firmware<br> <input type=\"text\" name=\"delayFirmware\" value=\"" << std::hex << delayFirmware << "\">" << "<br><br>"<< std::endl;
  *out << "VME Firmware<br> <input type=\"text\" name=\"vmeFirmware\" value=\"" << std::hex << vmeFirmware << "\">" << "<br><br>"<< std::endl;
  *out << "Crate Slot<br> <input type=\"text\" name=\"slot\" value=\"" << std::hex << slotNumber << "\">" << "<br><br>"<< std::endl;
   
  *out << cgicc::table().set("border","1").set("cellpadding","2").set("cellspacing","0") << cgicc::tr();
  Fed9U::Fed9UAddress fedAddress;
  for ( int i = 0 ; i < Fed9U::FEUNITS_PER_FED ; i++) {
    fedAddress.setFedFeUnit(i);
    *out << cgicc::th().set("style","font-size: 8pt;  font-family: arial; color: rgb(0, 0, 140);").set("colspan","3") << "FeUnit" << static_cast<unsigned int>(fedAddress.getExternalFedFeUnit())
      //<< "FeUnit " << static_cast<unsigned int>(fedAddress.getExternalFedFeUnit() )
	 << cgicc::input().set("type", "checkbox")
      .set("name",toString(i)) // note all checkboxes are numerically named
      .set((!fedDescriptions_[fedId]->getFedFeUnitDisable(fedAddress) ?"checked":"blah"),"true") << cgicc::th() << std::endl;
  }
  *out << cgicc::tr() << std::endl ;
  for ( int i = 0 ; i < Fed9U::CHANNELS_PER_FEUNIT ; i++) {
    fedAddress.setFeUnitChannel(i); 
    *out << cgicc::tr() << std::endl ;
    for ( int j = 0 ; j < Fed9U::FEUNITS_PER_FED ; j++) {
      fedAddress.setFedFeUnit(j);
      *out << cgicc::td().set("style","font-size: 8pt;  font-family: arial; color: rgb(0, 0, 140);") << "CH " 
	   << static_cast<unsigned int>(fedAddress.getExternalFeUnitChannel())
	   << cgicc::br() << cgicc::input().set("type", "text")
	.set("size","4")
	.set("maxlength","4")
	.set("name",toString(i*Fed9U::FEUNITS_PER_FED + j + 8) ) // note the number here starts at 8 to allow all checkboxes to have numeric names only
	.set("value", toString(fedDescriptions_[fedId]->getFrameThreshold(fedAddress) ) ) 
	   <<  cgicc::td() << cgicc::td().set("style","font-size: 8pt;  font-family: arial; color: rgb(0, 0, 140);")  << std::endl;

      *out << "TD " << cgicc::br() << cgicc::input().set("type", "text")
	.set("size","4")
	.set("maxlength","4")
	.set("name",toString(i*Fed9U::FEUNITS_PER_FED + j + 500) ) // note the number here starts at 500 to allow all checkboxes to have numeric names only
	.set("value", toString(fedDescriptions_[fedId]->getTrimDacOffset(fedAddress) ) ) 
	   <<  cgicc::td() << cgicc::td().set("style","font-size: 8pt;  font-family: arial; color: rgb(0, 0, 140);")  << std::endl;


      for (int k = 0 ; k < Fed9U::APVS_PER_CHANNEL ; k++) {
	fedAddress.setChannelApv(k);
	*out << "APV" << static_cast<unsigned int>(fedAddress.getExternalChannelApv() ) 
	     << cgicc::input().set("type", "checkbox")
	  .set("name",toString(i*Fed9U::FEUNITS_PER_FED*2 + j*Fed9U::APVS_PER_CHANNEL + k + 200) ) // note the number here starts at 100 to allow all checkboxes to have numeric names only
	  .set((!fedDescriptions_[fedId]->getApvDisable(fedAddress) ? "checked" : "blah" ),"true");
	if (k == 0) 
	  *out << cgicc::br() << std::endl;
      }
      *out << cgicc::td() << std::endl;
    }
    *out << cgicc::tr() << std::endl ;
  }
  
  *out << cgicc::table() << std::endl ;
	  
  
  // Apply
  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply");

  // End of the form
  *out << cgicc::form() << std::endl;

  // Few remarks
  *out << cgicc::h2("A few remarks") << std::endl ;
  *out << "<lu>" << std::endl ;
  *out << "<li>" << "Please make all the changes and go back to the main web page to apply the modifications. If you have any problems with this software please send hatemail to Jonthan Fulcher (162924) as often as possible!!! if you see him in the street you can call him nasty names too!!!" << "</li>" << std::endl ;
  *out << "</lu>" << std::endl ;
    
  // - End of HTML
  *out << cgicc::html() << std::endl ;
}

/** Apply the parameters for the FED
 */
void TkConfigurationDb::applyFEDParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);
    
    // FED ID
    unsigned int fedId = fromString<unsigned int>(cgi["fedId"]->getValue()) ;
    
    
    // <JRF> July 2006 Added code to update the FED parameters, only the feunit and apv enable disable and frame threshold
    std::cout << "about to write description edited" << std::endl;
    
    fedDescriptionsEdited_[fedId] = true;
    bool tempEnable;
    std::cout << "about to write settings" << std::endl;
    Fed9U::Fed9UAddress fedAddress;
    std::vector<cgicc::FormEntry> uglycgicc;
    
    bool reenableAll = (cgi.getElement("reenableAll", uglycgicc));
    for ( int i = 0 ; i < Fed9U::FEUNITS_PER_FED ; i++) {
      std::cout << "about to write address" << std::endl;
      fedAddress.setFedFeUnit(i);
      std::cout << "about to write disable bool" << std::endl;
      tempEnable = (cgi.getElement(toString(i), uglycgicc));
      
      std::cout << "about to write address" << std::endl;
      fedDescriptions_[fedId]->setFedFeUnitDisable(fedAddress, (!reenableAll && !tempEnable ) );
    }
    std::cout << "done Fe Unit enables settings" << std::endl;
 
    // now set the firmware versions
    char * pend;
    fedDescriptions_[fedId]->setFeFirmwareVersion(strtoul(cgi["feFirmware"]->getValue().c_str(),&pend,16));
    fedDescriptions_[fedId]->setBeFirmwareVersion(strtoul(cgi["beFirmware"]->getValue().c_str(),&pend,16));
    fedDescriptions_[fedId]->setDelayFirmwareVersion(strtoul(cgi["delayFirmware"]->getValue().c_str(),&pend,16));
    fedDescriptions_[fedId]->setVmeFirmwareVersion(strtoul(cgi["vmeFirmware"]->getValue().c_str(),&pend,16));
    fedDescriptions_[fedId]->setSlotNumber(strtoul(cgi["slot"]->getValue().c_str(),&pend,16));

    if (cgi["clock"]->getValue()=="INTERNAL") {
      fedDescriptions_[fedId]->setClock(Fed9U::FED9U_CLOCK_INTERNAL);
    }
    else if (cgi["clock"]->getValue()=="BACKPLANE") {
      fedDescriptions_[fedId]->setClock(Fed9U::FED9U_CLOCK_BACKPLANE);
    }
    else if (cgi["clock"]->getValue()=="TTC") {
      fedDescriptions_[fedId]->setClock(Fed9U::FED9U_CLOCK_TTC);
    }
    
    if (cgi["trigger"]->getValue()=="SOFTWARE") {
      fedDescriptions_[fedId]->setTriggerSource(Fed9U::FED9U_TRIG_SOFTWARE);
    }
    else if (cgi["trigger"]->getValue()=="BACKPLANE") {
      fedDescriptions_[fedId]->setTriggerSource(Fed9U::FED9U_TRIG_BACKPLANE);
    }
    else if (cgi["trigger"]->getValue()=="TTC") {
      fedDescriptions_[fedId]->setTriggerSource(Fed9U::FED9U_TRIG_TTC);
    }
    
    if (cgi["mode"]->getValue()=="ZERO") {
      fedDescriptions_[fedId]->setDaqMode(Fed9U::FED9U_MODE_ZERO_SUPPRESSED);
    }
    else if (cgi["mode"]->getValue()=="PROC") {
      fedDescriptions_[fedId]->setDaqMode(Fed9U::FED9U_MODE_PROCESSED_RAW);
    }
    else if (cgi["mode"]->getValue()=="VIRG") {
      fedDescriptions_[fedId]->setDaqMode(Fed9U::FED9U_MODE_VIRGIN_RAW);
    }
    else if (cgi["mode"]->getValue()=="SCOPE") {
      fedDescriptions_[fedId]->setDaqMode(Fed9U::FED9U_MODE_SCOPE);
    }
    
    
    if (cgi["superMode"]->getValue()=="FAKE") {
      fedDescriptions_[fedId]->setDaqSuperMode(Fed9U::FED9U_SUPER_MODE_FAKE);
    }
    else if (cgi["superMode"]->getValue()=="NORMAL") {
      fedDescriptions_[fedId]->setDaqSuperMode(Fed9U::FED9U_SUPER_MODE_NORMAL);
    }
    else if (cgi["superMode"]->getValue()=="NORMALZLITE") {
      fedDescriptions_[fedId]->setDaqSuperMode(Fed9U::FED9U_SUPER_MODE_ZERO_LITE);
    }
    else if (cgi["superMode"]->getValue()=="FAKEZLITE") {
      fedDescriptions_[fedId]->setDaqSuperMode(Fed9U::FED9U_SUPER_MODE_FAKE_ZERO_LITE);
    }
    
    if (cgi["readRoute"]->getValue()=="VME") {
      fedDescriptions_[fedId]->setBeFpgaReadRoute(Fed9U::FED9U_ROUTE_VME);
    }
    else if (cgi["readRoute"]->getValue()=="SLINK") {
      fedDescriptions_[fedId]->setBeFpgaReadRoute(Fed9U::FED9U_ROUTE_SLINK64);
    }
    
    for ( int i = 0 ; i < Fed9U::CHANNELS_PER_FEUNIT ; i++) { 
      fedAddress.setFeUnitChannel(i);
      for ( int j = 0 ; j < Fed9U::FEUNITS_PER_FED ; j++) {
	fedAddress.setFedFeUnit(j);
	fedDescriptions_[fedId]->setFrameThreshold(fedAddress,fromString<unsigned int>(cgi[toString(i*8 + j + 8)]->getValue() ) );
	fedDescriptions_[fedId]->setTrimDacOffset(fedAddress,fromString<unsigned int>(cgi[toString(i*8 + j + 500)]->getValue() ) );
      	for (int k = 0 ; k < Fed9U::APVS_PER_CHANNEL ; k++) {
	  fedAddress.setChannelApv(k);
	  tempEnable = cgi.getElement(toString(i*Fed9U::FEUNITS_PER_FED*2 + j*Fed9U::APVS_PER_CHANNEL + k + 200), uglycgicc);
	  fedDescriptions_[fedId]->setApvDisable(fedAddress, (!reenableAll && !tempEnable ) );

	}
      }
    }
    std::cout << "done  settings" << std::endl;
 
 
    // </JRF>

    displayFEDParameters (in,out,fedId) ;
     
  }
  catch(const std::exception& e) {
    *out << cgicc::h2("Problem in CGI: " + std::string(e.what())).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }
}

// -----------------------------------------------------------------------------------------------
// The following methods are defined also in the file DisplayCCUModules.h in FecSupervisor/include
// -----------------------------------------------------------------------------------------------

#define CLASSNAME TkConfigurationDb
#define CLASSNAMESTR std::string("Strip Tracker Configuration DB")

/** Display the diagnostic page
 */
void CLASSNAME::displayConfigureDiagSystem ( xgi::Input * in, xgi::Output * out ) {

  //std::cout << "displayFecHardwareCheck" << std::endl ;
  std::string urn = getApplicationDescriptor()->getURN();
  linkPosition_ = 4 ;

  // ----------------------------------------------------------------------
  // Here start the WEB pages
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title(CLASSNAMESTR + ": Diagnotic System Configuration") << std::endl;
  xgi::Utils::getPageHeader(*out, CLASSNAMESTR + ": Diagnotic System Configuration");

  // Status bar
  displayRelatedLinks (in,out) ;

  // ----------------------------------------------------------------------
  // Integrated web pages
  //getDiagSystemContent(in,out) ;
#ifdef TKDIAG
  diagService_->getDiagSystemHtmlPage(in,out,urn) ;
#else
  *out << cgicc::h2("Diagnostic not used in the compilation system, please contact the support to handle it").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
#endif

  // ----------------------------------------------------------------------
  //xgi::Utils::getPageFooter(*out);
  *out << "</HTML>" << std::endl ;
}

/** Display all the parameters
 */
void CLASSNAME::displayRelatedLinks(xgi::Input * in, xgi::Output * out, bool withHTMLTag) throw (xgi::exception::Exception) {

  if (withHTMLTag) {
    // ----------------------------------------------------------------------
    // Here start the WEB pages
    *out << "<HTML>" << std::endl ;
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title(CLASSNAMESTR + "Related Links") << std::endl;
    xgi::Utils::getPageHeader(*out, CLASSNAMESTR + "Related Links");
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
}

