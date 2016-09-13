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
#ifndef TKCONFIGURATIONDB_H
#define TKCONFIGURATIONDB_H

#include <string>

  // XDAQ includes
#include "xdata/UnsignedLong.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"

#include "xdaq/WebApplication.h"

  //#include "FsmWithStateName.h"
#include "FecExceptionHandler.h"

// Error reporting
#include "ErrorReportLogger.h"
#ifdef TKDIAG
#  include <diagbag/DiagBagWizard.h>
// DIAG_AUTO_CONFIG
#  ifdef AUTO_CONFIGURE_PROCESSES
#    include "toolbox/task/Timer.h"
#    include "toolbox/task/TimerFactory.h"
#    include "toolbox/TimeInterval.h"
#  endif
#endif

/** Database access
 */
#include "DeviceFactory.h"

#ifndef FEC_RED
#define FEC_RED "<span style=\"color: rgb(255, 0, 0);\">"
#endif
#ifndef FEC_GREEN
#define FEC_GREEN "<span style=\"color: rgb(51, 204, 0);\">"
#endif

/**
 * \class TkConfigurationDb
 * \version 1.0
 * \author Frederic Drouhin
 * \date January 2007
 * This class take care about the display of the parameters for the database.
 */
#ifdef AUTO_CONFIGURE_PROCESSES
class TkConfigurationDb: public xdaq::WebApplication, public toolbox::task::TimerListener {
#else
class TkConfigurationDb: public xdaq::WebApplication  {
#endif

 private:

  /** Version of the database
   */
  double dbVersion_ ;

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

  /** Is the db provided on cms_omds_lb so TKCC database is accessible
   */
  bool cmsOMDSlbAccess_ ;

  /** Device factory to retreive or set values in database or files
   */
  DeviceFactory *deviceFactory_ ;

  // Application final state machine: toolbox::fsm::FiniteStateMachine fsm_;
  // FsmWithStateName fsm_;

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

  /** Navigation bar for database management
   */
  std::vector<std::string> databaseRelatedLinks_ ;

  /** Link management for the database
   */
  unsigned int linkDbPosition_ ;

  /** Error or information or warning must be put on this stringstream
   */
  std::stringstream textInformation_ ;

  // ----------------------------------------- Database management

  /** Current state managed
   */
  tkStateVector dbStates_ ;

  /** Devices read from database that can be changed to be uploaded to the DB
   */
  deviceVector deviceValues_ ;

  /** FED description vector
   */
  //std::vector<Fed9U::Fed9UDescription*> fedDescriptions_;
  Fed9U::Fed9UHashMapType fedDescriptions_;
  
  /** Fed description edited hash map
   */
  Sgi::hash_map<Fed9U::u16, bool> fedDescriptionsEdited_;
 
  /** Devices that can be modified (it is the same devices than in the previous vector)
   * this parameter is used to avoid duplication of search in the vector
   */
  Sgi::hash_map<keyType,deviceDescription *> devicesToBeChanged_ ;

  /** DCU retreived from the database
   */
  deviceVector dcuDevices_ ;

  /** Partition name used for the database changes
   */
  std::string partitionDbName_ ;
  
  /** Is one of the devices has been changed to be upload to the database
   */
  bool apvDbChanged_ ;

  /** Is one of the devices has been changed to be upload to the database
   */
  bool pllDbChanged_ ;

  /** Is one of the devices has been changed to be upload to the database
   */
  bool aohDbChanged_ ;

  /** Is a partition already chosen
   */
  bool partitionDbNameChosen_ ;

  /** Is a state name already existing
   */
  std::string currentStateName_ ;

  /** File to dump the database
   */
  std::string xmlFECFileName_ ;

  /** View all partition when the state are display
   */
  bool viewAllPartitionNames_ ;

  /** Software tag version set by CVS
   */
  xdata::String softwareTagVersion_ ;

  /** Current state history ID
   */
  unsigned int currentStateHistoryId_ ;

  /** Display an information when an action is applied
   */
  std::stringstream displayRunInformationMessage_ ;

 public:

  // --------------------------------------------------------- XDAQ call

  XDAQ_INSTANTIATOR();

  // --------------------------------------------------------- XDAQ Definition

  /** \brief Constructor for the initialisation of the xdata/export parameters
   * \warning nothing is created in this method (nor DeviceFactory, Database access), see tkConfigurationDbInitialise
   */
  TkConfigurationDb( xdaq::ApplicationStub* stub ) ;

  /** \brief Destructor that remove all the accesses done to the hardware 
   * and destroy the connection with the database
   */
  ~TkConfigurationDb ( ) ;

  // ------------------------------------------------------- Finite state machine

#ifdef TKDIAG
  /** Timer called
   */
  void timeExpired (toolbox::task::TimerEvent& e);
#endif

  // ------------------------------------------------------- Database
  /** \brief database creation
   */
  void createDatabaseAccess ( ) ;

  // ------------------------------------------------------- actionPerformed

  /** \brief When an item is fired
   */
  void actionPerformed(xdata::Event& event) ;

  // FSM Soap message: not used here
  // xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception) ;

  // ------------------------------------------------------- Web methods

  /** \brief Downlaod devices from DB
   */
  void downloadAllDevicesFromDb ( ) ;

  /** \brief check if a module is disabled or not
   */
  bool isModuleDisabled ( deviceDescription *deviceD ) ;

  /** \brief check if a module is disabled or not
   */
  void setModuleEnabled ( deviceDescription *deviceD, bool disable ) ;

  /** \brief Default web page with frames
   */
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) ;

  /** \brief Web page for the parameters for the upload
   */
  void displayMoreParameters(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Apply the parameters described just before
   */
  void tkConfigurationDbParameterSettings (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Display the related links
   */
  void displayRelatedLinks(xgi::Input * in, xgi::Output * out, bool withHTMLTag = false) throw (xgi::exception::Exception) ;

  /** \brief failure
   */
  void failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) ;

  // --------------------------------- Database management

  /** \brief Database management interface
   */
  void displayConfigureDatabase (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Display the database related links
   */
  void displayDatabaseRelatedLinks(xgi::Input * in, xgi::Output * out, bool withHTMLTag = false) throw (xgi::exception::Exception) ;

  /** \brief Display the state management
   */
  void displayStateManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Apply the change on a state
   */
  void applyStateManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Display the Parameters to be changed
   */
  void displayPartitionFecParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Apply the change on a device register
   */
  void applyPartitionFecParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief this method apply a file with APV parameters and create a new version
   */
  void geometryChangeMethod ( xgi::Input * in, xgi::Output * out, std::string fileName, unsigned int *major, unsigned int *minor ) throw (xgi::exception::Exception) ;

  /** Display the modules with or without the DCU information or with or without connection information
   */
  void displayModuleManagement (xgi::Input * in, xgi::Output * out, bool dcuInformationRetreived, bool connectionInformationRetreived) throw (xgi::exception::Exception) ;

  /** \brief Display the Parameters to be changed
   */
  void displayModuleManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Apply the change on a module (disable enable)
   */
  void applyModuleManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Display the FEC module parameters
   */
  void displayModuleParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Display the FEC module parameters
   */
  void displayModuleParameters (xgi::Input * in, xgi::Output * out, unsigned int dcuHardId, keyType index) throw (xgi::exception::Exception) ;

  /** \brief Apply the change on a module for the FEC parameter
   */
  void applyModuleParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the FED list
   */
  void displayFEDManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Apply the FED list
   */
  void applyFEDManagement (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the parameters for the FED
   */
  void displayFEDParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the mode parameters for all  FEDs
   */
  void displayFEDModeParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the parameters for the FED
   */
  void displayFEDParameters (xgi::Input * in, xgi::Output * out, unsigned int fedId ) throw (xgi::exception::Exception) ;

  /** \brief apply the parameters for the FED
   */
  void applyFEDParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief apply the mode parameters for all FEDs
   */
  void applyFEDModeParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the connections inside the database
   */
  void displayFECFEDConnections (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Apply the parameters for the FED/FEC connections
   */
  void applyFECFEDConnections (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the PSU/DCU map
   */
  void displayDCUPSUMap (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief Apply the PSU/DCU map
   */
  void applyDCUPSUMap (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the connections inside the database
   */
  void displayRunInformation (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief change a comment on the run
   */
  void applyRunInformation (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the connections inside the database
   */
  void displayEnableDisableStructure (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display the connections inside the database
   */
  void applyEnableDisableStructure (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display a popup window
   */
  void displayPopupWindowForEnableDisable(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief display a vector of rings in a array
   */
  void displayRingHTMLTable ( xgi::Output *out, tkringVector allRings, bool readOnly ) throw (xgi::exception::Exception) ;

  /** \brief display the connections inside the database
   */
  void displayCCUConfiguration (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  /** \brief change a comment on the run
   */
  void applyCCUConfiguration (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) ;

  // --------------------------------- Diagnostic system call back

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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  /** \brief callback XOAP for remote control pannel of LogReader application (global)
   */
  DIAG_REQUEST_ENTRYPOINT();

#endif

} ;

#endif

