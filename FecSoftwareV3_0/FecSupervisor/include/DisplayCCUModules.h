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
//#ifndef DISPLAYCCUMODULES_H
//#define DISPLAYCCUMODULES_H

/*****************************************
I'm not really happy to make this kind of macro and methods but in order to simplify
the code and the maintenance of it, the next macros and methods are implemented
in order to avoid duplication code in the FecSupervisor and CrateController.
Is it a good way to make it ? I will say no, certainly a inheritance can solve the problem
but regarding the time to make and to make this files, it can explains why I have done it.
To use now this file you shoud declare the following defines:
  #define CLASSNAME    CrateController
  #define CLASSNAMESTR std::string("CrateController")
  #define __CrateControllerClass__
  #include "DisplayCCUModules.h"
You should afterwards declare the following methods in your .h file (CrateController.h in my example):
  void displayHardwareCheckNavigation (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void displayFecRingRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void setFecControlRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void displayRedundancy(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void displayCCSTrigger(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void setCCSControlRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void displayCcuModules(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void displayCCU ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void setCcuControlRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void displayModule ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void displayDCUCCU ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void displayDOH ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void displayModuleParameters ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void applyModuleParameters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
Here CrateController is my XDAQ class name that is also FecSupervisor.
Hope you never see this code :)
Frederic Drouhin 01/06/2006
*****************************************/

/** NUMBER OF CHAR TO BE DISPLAYED IN THE TEXTAREA
 */
#define MAXWEBDISPLAY 30000


#define BINDCCUMODULESMETHODS \
  std::string urlI = getApplicationDescriptor()->getURN(); \
  hardwareCheckNavigation_.push_back(toolbox::toString("<a href=\"/%s/FECRingRegisters\">FEC Ring Registers</a>", urlI.c_str())); \
  hardwareCheckNavigation_.push_back(toolbox::toString("<a href=\"/%s/Redundancy\">Redundancy</a>", urlI.c_str())); \
  hardwareCheckNavigation_.push_back(toolbox::toString("<a href=\"/%s/CCUModules\">CCU / Modules</a>", urlI.c_str())) ; \
  hardwareCheckNavigation_.push_back(toolbox::toString("<a href=\"/%s/CCSTrigger\">CCS Trigger</a>", urlI.c_str())); \
  xgi::bind(this, &CLASSNAME::displayFecRingRegisters, "FECRingRegisters"); \
  xgi::bind(this, &CLASSNAME::setFecControlRegisters, "setFecControlRegisters"); \
  xgi::bind(this, &CLASSNAME::displayRedundancy, "Redundancy"); \
  xgi::bind(this, &CLASSNAME::displayCcuModules, "CCUModules"); \
  xgi::bind(this, &CLASSNAME::displayCCSTrigger, "CCSTrigger"); \
xgi::bind(this, &CLASSNAME::setCCSControlRegisters, "setCCSControlRegisters"); \
  xgi::bind(this, &CLASSNAME::displayCCU, "displayCCU"); \
  xgi::bind(this, &CLASSNAME::setCcuControlRegisters, "setCcuControlRegisters"); \
  xgi::bind(this, &CLASSNAME::displayModule, "displayModule"); \
  xgi::bind(this, &CLASSNAME::displayDCUCCU, "displayDCUCCU"); \
  xgi::bind(this, &CLASSNAME::displayDOH, "displayDOH"); \
  xgi::bind(this, &CLASSNAME::displayHardwareCheckNavigation, "HardwareCheckNavigation");

// HTML Macros:
#define FEC_RED "<span style=\"color: rgb(255, 0, 0);\">"
#define FEC_GREEN "<span style=\"color: rgb(51, 204, 0);\">"
#define FEC_BLUE "<span style=\"color: rgb(51, 51, 255);\">"
#define FEC_YELLOW "<span style=\"color: rgb(255, 255, 0);\">"
#define FEC_ORANGE "<span style=\"color: rgb(255, 153, 0);\">"
#define FEC_GREY "<span style=\"color: rgb(153, 153, 153);\">"
#define FEC_BROWN "<span style=\"color: rgb(153, 51, 0);\">"
#define FEC_NOCOL "</span>"
#define FEC_MONO "<span style=\"font-family: monospace;\">"
#define FEC_NOMONO "</span>"
#define FEC_BOLD "<span style=\"font-weight: bold;\">"
#define FEC_NOBOLD "</span>"
#define FEC_ITALIC "<span style=\"font-style: italic;\">"
#define FEC_NOITALIC "</span>"
#define FEC_SMALLER "<font size=\"-1\">"
#define FEC_NOSMALLER "</font>"
#define FEC_BIGGER "<font size=\"+1\">"
#define FEC_NOBIGGER "</font>"
#define FEC_UNDERL "<span style=\"text-decoration: underline;\">"
#define FEC_NOUNDERL "</span>"
#define FEC_BGDCOL_YELLOW "background-color: rgb(255, 255, 153);"
#define FEC_BGDCOL_GREEN "background-color: rgb(153, 255, 153);"   // light green
#define FEC_BGDCOL_GREY "background-color: rgb(153, 153, 153);"
#define FEC_BGDCOL_PINK "background-color: rgb(255, 153, 255);"
#define FEC_BGDCOL_ORANGE "background-color: rgb(255, 204, 102);"
//#define FEC_BGDCOL_GREEN "background-color: rgb(255, 255, 255);"  //white

/** Display a field with a list of modules or DCU on CCU with the errors
 * \param out - output for the XML buffer
 * \param legendFieldName - legend of the field
 * \param nameAnchor - anchor name to cross the different fields
 * \param foundModule - Found 10 " module(s) with error ... " (the part between ")
 * \param moduleNameOn - "Module on "
 * \param modulesToDisplay - List of the keyType to be displayed
 * \param deviceError - hash_map of the errors
 * \param deviceMissing - hash_map of the missing devices
 */
void CLASSNAME::displayListModulesInList ( xgi::Output * out, 
					   std::string legendFieldName, std::string nameAnchor,
					   std::string foundModule, 
					   std::list<keyType> modulesToDisplay,
					   Sgi::hash_map<keyType, bool> deviceError,
					   Sgi::hash_map<keyType, bool> deviceMissing ) {
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayListModulesInList accessed", LOGUSERINFO) ;
#endif
  bool ulBool = true, liBool = true ;
  *out << "<a name=\"" << nameAnchor << "\"></a>" ;
  *out << cgicc::fieldset() << std::endl;
  *out << cgicc::p() << cgicc::legend(legendFieldName) << std::endl ;
  *out << "Found " << std::dec << modulesToDisplay.size() << foundModule << std::endl ;
  for (std::list<keyType> ::iterator it = modulesToDisplay.begin() ; it != modulesToDisplay.end() ; it ++) {
    if (ulBool) {
      ulBool = false ;
      *out << "<ul>" << std::endl ;
    }      

    keyType index = getFecRingCcuChannelKey((*it)) ;
    unsigned int address = getAddressKey((*it)) ;
    
    char msg[80] ; decodeKey(msg,index) ;
    if (address == 0x20)
      *out << "<li>" << "Found module with 4 APVs on " << msg << std::endl ;
    else if (address == 0x22)
      *out << "<li>" << "Found module with 6 APVs on " << msg << std::endl ;
    else 
      *out << "<li>" << "Found module with incoherent number of APVs on " << msg << std::endl ;

    if (deviceError[index|setAddressKey(0x20)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "APV 0x20 access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x20)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing APV 0x20</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x21)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "APV 0x21 access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x21)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing APV 0x21</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x22)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "APV 0x22 access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x22)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing APV 0x22</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x23)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "APV 0x23 access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x23)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing APV 0x23</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x24)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "APV 0x24 access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x24)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing APV 0x24</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x25)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "APV 0x25 access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x25)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing APV 0x25</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x43)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "APV MUX access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x43)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing APV MUX</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x0)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "DCU access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x0)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing DCU</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x60)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "AOH access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x60)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing AOH</li>" << std::endl ;
    }
    if (deviceError[index|setAddressKey(0x44)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "PLL access error</li>" << std::endl ;
    }
    if (deviceMissing[index|setAddressKey(0x44)]) {
      if (liBool) {
	liBool = false ;
	*out << "<ul>" << std::endl ;
      }
      *out << "<li>" << "Missing PLL</li>" << std::endl ;
    }
    if (!liBool) {
      *out << "</ul>" << std::endl ;
      liBool = true ;
    }
  }
  if (!ulBool) *out << "</ul>" << std::endl ;
  *out << cgicc::fieldset() << std::endl;
}

/** Display all the FEC Ring with status and resets
 */
void CLASSNAME::displayHardwareCheckNavigation (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("Disabling the DcuThread for hardware check, please go to MoreParameters or state machine to re-enable it", LOGWARNING) ;
  stopWorkLoop() ;
#endif

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);
    unsigned int fecSlot = fromHexString<unsigned int>(cgi["param1"]->getValue()) ;
    unsigned int ringSlot = fromHexString<unsigned int>(cgi["param2"]->getValue()) ;
    indexFecRingChosen_ = buildFecRingKey(fecSlot,ringSlot) ;
  }
  catch (const std::exception& e) {

    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
  }

  //*out << cgicc::HTTPHTMLHeader() ;
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  std::string titre = CLASSNAMESTR + ": FEC " + toString(getFecKey(indexFecRingChosen_)) + " Ring " + toString (getRingKey(indexFecRingChosen_)) ; // + " CCU Modules" ;
  *out << cgicc::title(titre) << std::endl;
  xgi::Utils::getPageHeader(*out, titre) ;

  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
     *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p() << std::endl ;

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, XDAQFEC_HARDWAREACCESSERROR, partitionName_.toString()) ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

    *out << "</HTML>" << std::endl ;     

    return ;
  }

  *out << "</HTML>" << std::endl ; 
}

/** Display all the FEC/Ring registers
 * SR0, SR1, CR0, CR1
 */
void CLASSNAME::displayFecRingRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayFecRingRegisters", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayFecRingRegisters accessed", LOGUSERINFO) ;
#endif

  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  std::string titre = CLASSNAMESTR + ": Registers on FEC " + toString(getFecKey(indexFecRingChosen_)) + " Ring " + toString (getRingKey(indexFecRingChosen_)) ;
  *out << cgicc::title(titre) << std::endl;
  xgi::Utils::getPageHeader(*out, titre) ;
  
  // Navigation Bar
  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
    *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p() << std::endl ;

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, XDAQFEC_HARDWAREACCESSERROR, partitionName_.toString()) ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

    *out << "</HTML>" << std::endl ; 
    
    return ;
  }

  std::string strFecRing = toString(getFecKey(indexFecRingChosen_)) + "." + toString(getRingKey(indexFecRingChosen_)) ;
  *out << cgicc::h3("FEC Specifications " + strFecRing).set("style", "font-family: arial") << std::endl;

  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/setFecControlRegisters";	

  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  *out << cgicc::h3("FEC Ring registers for FEC " + strFecRing).set("style", "font-family: arial") << std::endl;
  try {
    tscType32 fecSR0 = fecAccess_->getFecRingSR0(indexFecRingChosen_) ;
    tscType32 fecSR1 = fecAccess_->getFecRingSR1(indexFecRingChosen_) ;
    tscType32 fecCR0 = fecAccess_->getFecRingCR0(indexFecRingChosen_) ;
    tscType32 fecCR1 = 0x7 ; //fecAccess_->getFecRingCR1(indexFecRingChosen_) ; Not readable

    // Display all registers
    // Status registers  
    *out << "<table border=4 cellpadding=10 cellspacing=10>" << std::endl;
    *out << "<tr align=center VALIGN=TOP>" << std::endl;
    *out << "<td>";

    // Status register 0
    *out << "<table border=2 cellpadding=10 cellspacing=0>" << std::endl;
    *out << "<tr align=center>" << std::endl;
    *out << "<td>"; 
    *out << "Status Register 0" << std::endl;
    *out << "</td>";
    *out << "<td>"; 
    *out << cgicc::input().set("type","text").set("align","center").set("size","16").set("readonly","readonly").set("align","center").set("value","0x" + toHexString(fecSR0)) << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_TRARUN)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Tra. Running") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_RECRUN)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Rec. Running") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_TRAFULL)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Tra. Full") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_TRAEMPTY)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Tra. Empty") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_RECFULL)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Rec. Full") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_RECEMPTY)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Rec. Empty") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_RETFULL)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Ret. Full") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_RETEMPTY)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Ret. Empty") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_LINKINITIALIZED)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Link Init") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_TTCRXREADY)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("TTCRx") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_PENDINGIRQ)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Pending IRQ") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR0 & FEC_SR0_DATATOFEC)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Data to FEC") << std::endl;
    *out << "</td>"; 
    *out << "</tr>" << std::endl;
    *out << "</table>" << std::endl;

    *out << "</td>"; 
    *out << "<td>"; 

    // Status register 1
    *out << "<table border=2 cellpadding=10 cellspacing=0>" << std::endl;
    *out << "<tr align=center>" << std::endl;
    *out << "<td>"; 
    *out << "Status Register 1" << std::endl;
    *out << "</td>";
    *out << "<td>"; 
    *out << cgicc::input().set("type","text").set("align","center").set("size","16").set("readonly","readonly").set("align","center").set("value","0x" + toHexString(fecSR1)) << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR1 & FEC_SR1_ILLDATA)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Ill. Data") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR1 & FEC_SR1_ILLSEQ)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Ill. Seq.") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR1 & FEC_SR1_CRCERROR)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("CRC Error") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR1 & FEC_SR1_DATACOPIED)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Data Copied") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR1 & FEC_SR1_ADDRSEEN)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Addr. Seen") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR1 & FEC_SR1_ERROR)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Error") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecSR1 & FEC_SR1_TIMEOUT)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Ret. Full") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecSR1 & FEC_SR1_CLOCKERROR)
      *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
    *out << cgicc::label("Clock Error") << std::endl;
    *out << "</td>"; 
    *out << "</tr>" << std::endl;
    *out << "</table>" << std::endl;

    *out << "</tr>" << std::endl;
    *out << "<tr align=center VALIGN=TOP>" << std::endl;

    // Control registers
    *out << "<td>";

    // Control register 0
    *out << "<table border=2 cellpadding=10 cellspacing=0>" << std::endl;
    *out << "<tr align=center>" << std::endl;
    *out << "<td>"; 
    *out << "Control Register 0" << std::endl;
    *out << "</td>";
    *out << "<td>"; 
    *out << cgicc::input().set("type","text").set("readonly","readonly").set("align","center").set("name","CR0Value").set("size","8").set("align","center").set("value","0x" + toHexString(fecCR0)) << std::endl;
    //*out << cgicc::br() << cgicc::input().set("type", "checkbox").set("name","setCR0").set("selected") << std::endl;
    //*out << cgicc::label("Set CR0") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_ENABLEFEC)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_ENABLEFEC").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_ENABLEFEC").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Enable FEC") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_SEND)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_SEND").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_SEND").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Send") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_XTALCLOCK)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_XTALCLOCK").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_XTALCLOCK").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Internal Clock") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_SELSEROUT)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_SELSEROUT").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_SELSEROUT").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Sel. Ser. Output") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_SELSERIN)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_SELSERIN").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_SELSERIN").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Sel. Ser. Input") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_RESETTCRX)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_RESETTCRX").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_RESETTCRX").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Reset TTCRx") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_POLARITY)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_POLARITY").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_POLARITY").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Invert Polarity") << std::endl;
    *out << "</td>"; 
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_DISABLERECEIVE)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_DISABLERECEIVE").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_DISABLERECEIVE").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Disable Rec.") << std::endl;
    *out << "</td>";
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_LOOPBACK)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_LOOPBACK").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_LOOPBACK").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Loop Back") << std::endl;
    *out << "</td>";
    *out << "<td>"; 
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_RESETFSMFEC)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_RESETFSMFEC").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_RESETFSMFEC").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Reset FSM") << std::endl;
    *out << "</td>";
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_RESETRINGB)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_RESETRINGB").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_RESETRINGB").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Reset B") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR0 & FEC_CR0_RESETOUT)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_RESETOUT").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR0_RESETOUT").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Reset Out") << std::endl;
    *out << "</td>"; 
    *out << "<td>" ;
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "setCR0");
    *out << "</td>" ;
    *out << "</tr>" << std::endl;
    *out << "</table>" << std::endl;

    *out << "</td>"; 
    *out << "<td>"; 

    // Control register 1
    *out << "<table border=2 cellpadding=10 cellspacing=0>" << std::endl;
    *out << "<tr align=center>" << std::endl;
    *out << "<td>"; 
    *out << "Control Register 1" << std::endl;
    *out << "</td>";
    *out << "<td>"; 
    *out << cgicc::input().set("type","text").set("readonly","readonly").set("align","center").set("name","CR1Value").set("size","8").set("align","center").set("value","0x" + toHexString(fecCR1)) << std::endl;
    //*out << cgicc::br() << cgicc::input().set("type", "checkbox").set("name","setCR1").set("selected") << std::endl;
    //*out << cgicc::label("Set CR1") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR1 & FEC_CR1_CLEARIRQ)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR1_CLEARIRQ").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR1_CLEARIRQ").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Clear IRQ") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR1 & FEC_CR1_CLEARERRORS)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR1_CLEARERRORS").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR1_CLEARERRORS").set("type", "checkbox") << std::endl;
    *out << cgicc::label("Clear Errors Indicators") << std::endl;
    *out << "</td>"; 
    *out << "<tr align=left>" ;
    *out << "<td>"; 
    if (fecCR1 & FEC_CR1_RELEASEFEC)
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR1_RELEASEFEC").set("type", "checkbox").set("checked","true") << std::endl;
    else
      *out << cgicc::p() << cgicc::input().set("name","FEC_CR1_RELEASEFEC").set("type", "checkbox") << std::endl;
    *out << cgicc::label("FEC Release") << std::endl;
    *out << "</td>"; 
    *out << "<td>" ;
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "setCR1");
    *out << "</td>" ;
    *out << "</tr>" << std::endl;
    *out << "</table>" << std::endl;

    *out << "</td>"; 
    *out << "<td>"; 

    if ( (fecAccess_->getFecBusType() == FECVME) && (fecAccess_->getFecFirmwareVersion(indexFecRingChosen_) >= 17) ) {
      // Reload the different firmware
      *out << "<table border=2 cellpadding=10 cellspacing=0>" << std::endl;
      *out << "<tr align=center>" << std::endl;
      *out << "<td>"; 
      *out << "Firmware Reload" << std::endl;
      *out << "</td>";
      *out << "<td>"; 
      //*out << cgicc::input().set("type","text").set("readonly","readonly").set("align","center").set("name","CR1Value").set("size","8").set("align","center").set("value","0x" + toHexString(fecCR1)) << std::endl;
      *out << "</td>"; 
      *out << "<tr align=left>" ;
      *out << "<td>"; 
      *out << cgicc::p() << cgicc::input().set("name","MFECFGPGA").set("type", "checkbox") << std::endl;
      *out << cgicc::label("mFEC Firmware") << std::endl;
      *out << "</td>"; 
      *out << "<tr align=left>" ;
      *out << "<td>"; 
      *out << cgicc::p() << cgicc::input().set("name","TriggerFPGA").set("type", "checkbox") << std::endl;
      *out << cgicc::label("Trigger FPGA Firmware") << std::endl;
      *out << "</td>"; 
      *out << "<tr align=left>" ;
      *out << "<td>"; 
      *out << cgicc::p() << cgicc::input().set("name","VMEFPGA").set("type", "checkbox") << std::endl;
      *out << cgicc::label("VME FPAG Firmware") << std::endl;
      *out << "</td>"; 
      *out << "<td>" ;
      *out << cgicc::p() << cgicc::input().set("type", "submit")
	.set("name", "submit")
	.set("value", "setReloadFirmware");
      *out << "</td>" ;
      *out << "</tr>" << std::endl;
      *out << "</table>" << std::endl;
    }

    *out << "</tr>" << std::endl;
    *out << "</table>" << std::endl;
      
    // Apply
    *out << cgicc::br() << "<CENTER>" ;
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Display Again");
    *out << "</CENTER>" ;
    
    // End of the form
    *out << cgicc::form() << std::endl;
  }
  catch (FecExceptionHandler &e) {

    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot read one or several FEC/Ring registers").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;

    errorReportLogger_->errorReport ("Cannot read one or several FEC/Ring registers", e, LOGERROR) ;
  }
  *out << "</HTML>" << std::endl ; 
}


/** Set the control registers depending of the web page 
 */
void CLASSNAME::setFecControlRegisters (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception) {

#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page setFecControlRegisters accessed", LOGUSERINFO) ;
#endif

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    if (cgi["submit"]->getValue() == "setCR1") {
      //std::string strCR1 = cgi["CR1Value"]->getValue() ;
      tscType32 fecCR1 = 0x0 ; //fromHexString<unsigned int>(strCR1) ;
      if (cgi.queryCheckbox("FEC_CR1_CLEARIRQ")) fecCR1 |= FEC_CR1_CLEARIRQ ;
      if (cgi.queryCheckbox("FEC_CR1_CLEARERRORS")) fecCR1 |= FEC_CR1_CLEARERRORS ;
      if (cgi.queryCheckbox("FEC_CR1_RELEASEFEC")) fecCR1 |= FEC_CR1_RELEASEFEC ;
      
      std::ostringstream msgDebug ; msgDebug <<"Set the FEC control register 1 to " << std::hex << (int)(fecCR1 & 0x3);
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      
      fecAccess_->setFecRingCR1 (indexFecRingChosen_, (fecCR1 & 0x3)) ;
    }
    else if (cgi["submit"]->getValue() == "setCR0") {
      //std::string strCR0 = cgi["CR0Value"]->getValue() ;
      tscType32 fecCR0 = 0 ; //fromHexString<unsigned int>(strCR0) ;
      
      if (cgi.queryCheckbox("FEC_CR0_ENABLEFEC")) fecCR0 |= FEC_CR0_ENABLEFEC ;
      if (cgi.queryCheckbox("FEC_CR0_SEND")) fecCR0 |= FEC_CR0_SEND ;
      if (cgi.queryCheckbox("FEC_CR0_XTALCLOCK")) fecCR0 |= FEC_CR0_XTALCLOCK ;
      if (cgi.queryCheckbox("FEC_CR0_SELSEROUT")) fecCR0 |= FEC_CR0_SELSEROUT ;
      if (cgi.queryCheckbox("FEC_CR0_SELSERIN")) fecCR0 |= FEC_CR0_SELSERIN ;
      if (cgi.queryCheckbox("FEC_CR0_RESETTCRX")) fecCR0 |= FEC_CR0_RESETTCRX ;
      if (cgi.queryCheckbox("FEC_CR0_POLARITY")) fecCR0 |= FEC_CR0_POLARITY ;
      if (cgi.queryCheckbox("FEC_CR0_DISABLERECEIVE")) fecCR0 |= FEC_CR0_DISABLERECEIVE ;
      if (cgi.queryCheckbox("FEC_CR0_LOOPBACK")) fecCR0 |= FEC_CR0_LOOPBACK ;
      if (cgi.queryCheckbox("FEC_CR0_RESETFSMFEC")) fecCR0 |= FEC_CR0_RESETFSMFEC ;
      if (cgi.queryCheckbox("FEC_CR0_RESETRINGB")) fecCR0 |= FEC_CR0_RESETRINGB ;
      if (cgi.queryCheckbox("FEC_CR0_RESETOUT")) fecCR0 |= FEC_CR0_RESETOUT ;
      
      std::ostringstream msgDebug ; msgDebug <<"Set the FEC control register 0 to " << std::hex << (int)fecCR0 ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;

      // Maintain the modification in the core of the CLASSNAME
      if ( (fecCR0 & FEC_CR0_RESETOUT) || (fecCR0 & FEC_CR0_RESETRINGB)) {      
	
	if (fecCR0 & FEC_CR0_RESETOUT) {
	  fecAccess_->fecRingReset(indexFecRingChosen_) ;
	  fecCR0 &= ~FEC_CR0_RESETOUT ;
	}
	else {
	  fecAccess_->fecRingResetB(indexFecRingChosen_) ;
	  fecCR0 &= ~FEC_CR0_RESETRINGB ;
	}
	
	fecAccessManager_->removeDevices () ;
      }

      // Set the FEC CR0 register
      fecAccess_->setFecRingCR0 (indexFecRingChosen_, fecCR0) ;
    }
    else if (cgi["submit"]->getValue() == "setReloadFirmware") {

      tscType8 value = 0 ; //fromHexString<unsigned int>(strCR0) ;
      
      if (cgi.queryCheckbox("MFECFGPGA")) value |= 1 ;
      if (cgi.queryCheckbox("TriggerFPGA")) value |= 2 ;
      if (cgi.queryCheckbox("VMEFPGA")) value |= 4 ;

      if (value != 0) fecAccess_->fecHardReset(indexFecRingChosen_,value) ;
    }

    // Redisplay the page if all is fine
    this->displayFecRingRegisters (in,out) ;
  }
  catch (FecExceptionHandler &e) {

    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot set one the FEC ring control register").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;

    errorReportLogger_->errorReport ("Cannot set one the FEC ring control register", e, LOGERROR) ;
  }

  *out << "<HTML>" << std::endl ;
}

/** Display all the registers of the CCU and FECs related to the redundancy
 */
void CLASSNAME::displayRedundancy(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayRedundancy", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayRedundancy accessed", LOGUSERINFO) ;
#endif


  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  std::string titre = CLASSNAMESTR + ": Redundancy on FEC " + toString(getFecKey(indexFecRingChosen_)) + " Ring " + toString (getRingKey(indexFecRingChosen_)) ;
  *out << cgicc::title(titre) << std::endl;
  xgi::Utils::getPageHeader(*out, titre) ;  

  // Navigation Bar
  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
    *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p();
 
  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, XDAQFEC_HARDWAREACCESSERROR, partitionName_.toString()) ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

    *out << "</HTML>" << std::endl ;     

    return ;
  } 

  *out << cgicc::h3("Redundancy").set("style", "font-family: arial") << std::endl;
  *out << "</HTML>" << std::endl ; 
}

/** Display all the registers for the CCSTrigger
 */
void CLASSNAME::displayCCSTrigger(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayRedundancy", LOGDEBUG) ;

#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayCCSTrigger accessed", LOGUSERINFO) ;
#endif


  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  std::string titre = CLASSNAMESTR + ": CCS Trigger on FEC " + toString(getFecKey(indexFecRingChosen_)) ;
  *out << cgicc::title(titre) << std::endl;
  xgi::Utils::getPageHeader(*out, titre) ;  

  // Navigation Bar
  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
    *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p();

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, XDAQFEC_HARDWAREACCESSERROR, partitionName_.toString()) ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

    *out << "</HTML>" << std::endl ;     

    return ;
  }
  
  std::string url = "/";
  url += getApplicationDescriptor()->getURN();
  url += "/setCCSControlRegisters";	

  *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

  *out << cgicc::h3("FEC Trigger Controller and Status Registers for FEC " + toString(getFecKey(indexFecRingChosen_))).set("style", "font-family: arial") << std::endl;
  try {
    std::string triggerFirmwareVersion = toHexString(fecAccess_->getTriggerVersion(indexFecRingChosen_)) ;
    *out << cgicc::h3("FEC Trigger firmware version for FEC " + toString(getFecKey(indexFecRingChosen_)) + " is: " + triggerFirmwareVersion).set("style", "font-family: arial") << std::endl;
  }
  catch (FecExceptionHandler &e) {
    *out << cgicc::h3("FEC Trigger Controller and Status Registers for FEC " + toString(getFecKey(indexFecRingChosen_))).set("style", "font-family: arial") << std::endl;
    errorReportLogger_->errorReport ("Cannot read the trigger firmware version", e, LOGERROR) ;
  }

  unsigned int triggerConfig0Value = 0 ;
  unsigned int triggerStatus0Value = 0 ;

  try {
    triggerConfig0Value = fecAccess_->getCCSTriggerConfig0(indexFecRingChosen_) ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport ("Cannot read one of the trigger status/control registers", e, LOGERROR) ;
  }
  try {
    triggerStatus0Value = fecAccess_->getCCSTriggerStatus0(indexFecRingChosen_) ;
  }
  catch (FecExceptionHandler &e) {
    errorReportLogger_->errorReport ("Cannot read one of the trigger status/control registers", e, LOGERROR) ;
  }

  // Display all registers
  // Status registers  
  *out << "<table border=4 cellpadding=10 cellspacing=10>" << std::endl;
  *out << "<tr align=center VALIGN=TOP>" << std::endl;
  *out << "<td>";

  // Status register 0
  *out << "<table border=2 cellpadding=10 cellspacing=0>" << std::endl;
  *out << "<tr align=center>" << std::endl;
  *out << "<td>"; 
  *out << "Status Register 0" << std::endl;
  *out << "</td>";
  *out << "<td>"; 
  *out << cgicc::input().set("type","text").set("align","center").set("size","16").set("readonly","readonly").set("align","center").set("value","0x" + toHexString(triggerStatus0Value)) << std::endl;
  *out << "</td>"; 
  *out << "<tr align=left>" ;
  *out << "<td>"; 
  if (triggerStatus0Value & CCS_SR0_QPLL_ERROR)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
  *out << cgicc::label("QPLL Error") << std::endl;
  *out << "</td>"; 
  *out << "<tr align=left>" ;
  *out << "<td>";
  if (triggerStatus0Value & CCS_SR0_QPLL_LOCKED)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
  *out << cgicc::label("QPLL Locked") << std::endl;
  *out << "</td>"; 
  *out << "<tr align=left>" ;
  *out << "<td>";
  if (triggerStatus0Value & CCS_SR0_TTCRX_READY)
    *out << cgicc::p() << cgicc::input().set("type", "checkbox").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("type", "checkbox") << std::endl;
  *out << cgicc::label("TTCRx Ready") << std::endl;
  *out << "</td>"; 
  *out << "</tr>" ;
  *out << "</table>" << std::endl;

  *out << "</tr>" << std::endl;
  *out << "<tr align=center VALIGN=TOP>" << std::endl;
  
  // Control registers
  *out << "<td>";

  // Control register 0
  *out << "<table border=2 cellpadding=10 cellspacing=0>" << std::endl;
  *out << "<tr align=center>" << std::endl;
  *out << "<td>"; 
  *out << "Control Register 0" << std::endl;
  *out << "</td>";
  *out << "<td>"; 
  *out << cgicc::input().set("type","text").set("readonly","readonly").set("align","center").set("name","CR0Value").set("size","8").set("align","center").set("value","0x" + toHexString(triggerConfig0Value)) << std::endl;
  //*out << cgicc::br() << cgicc::input().set("type", "checkbox").set("name","setCR0").set("selected") << std::endl;
  //*out << cgicc::label("Set CR0") << std::endl;
  *out << "</td>"; 
  *out << "<tr align=left>" ;
  *out << "<td>"; 
  unsigned int qpllf0 = triggerConfig0Value & CCS_QPLL_F0SELECT ;
  *out << "QPLL F0 Select" << std::endl ;
  //*out << "</td>" << "<td>"; 
  *out << cgicc::input().set("type","text").set("align","center").set("name","QPLLF0SELECT").set("size","8").set("align","center").set("value","0x" + toHexString(qpllf0)) << std::endl;
  *out << "</td>" ;
  *out << "</tr>" ;
  *out << "<tr align=left>" ;
  *out << "<td>";
  if (triggerConfig0Value & CCS_QPLL_AUTORESTART)
    *out << cgicc::p() << cgicc::input().set("name","CCS_QPLL_AUTORESTART").set("type", "checkbox").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("name","CCS_QPLL_AUTORESTART").set("type", "checkbox") << std::endl;
  *out << cgicc::label("QPLL Auto-restart") << std::endl;
  *out << "</td>"; 
  *out << "<td>";
  if (triggerConfig0Value & CCS_QPLL_RESET)
    *out << cgicc::p() << cgicc::input().set("name","CCS_QPLL_RESET").set("type", "checkbox").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("name","CCS_QPLL_RESET").set("type", "checkbox") << std::endl;
  *out << cgicc::label("QPLL Reset") << std::endl;
  *out << "</td>" << "</tr>" ;
  *out << "<tr align=left>" ;
  *out << "<td>";
  if (triggerConfig0Value & CCS_QPLL_EXTCTRL)
    *out << cgicc::p() << cgicc::input().set("name","CCS_QPLL_EXTCTRL").set("type", "checkbox").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("name","CCS_QPLL_EXTCTRL").set("type", "checkbox") << std::endl;
  *out << cgicc::label("QPLL External Control") << std::endl;
  *out << "</td>"; 
  *out << "<td>";
  if (triggerConfig0Value & CCS_TTCRX_RESET)
    *out << cgicc::p() << cgicc::input().set("name","CCS_TTCRX_RESET").set("type", "checkbox").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("name","CCS_TTCRX_RESET").set("type", "checkbox") << std::endl;
  *out << cgicc::label("TTCRx Reset") << std::endl;
  *out << "</td>" << "</tr>" ;
  *out << "<td>";
  if (triggerConfig0Value & CCS_FORCELOCALMODE)
    *out << cgicc::p() << cgicc::input().set("name","CCS_FORCELOCALMODE").set("type", "checkbox").set("checked","true") << std::endl;
  else
    *out << cgicc::p() << cgicc::input().set("name","CCS_FORCELOCALMODE").set("type", "checkbox") << std::endl;
  *out << cgicc::label("Force Local Mode") << std::endl;
  *out << "</td>" ;
  *out << "<td>" ;
  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "setCR0");
  *out << "</td>" ;
  *out << "</tr>" << std::endl;
  *out << "</table>" << std::endl;

  *out << "</tr>" << std::endl;
  *out << "</table>" << std::endl;
  
  // Apply
  *out << cgicc::br() << "<CENTER>" ;
  *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Display Again");
  *out << "</CENTER>" ;
    
  // End of the form
  *out << cgicc::form() << std::endl;

  *out << "</HTML>" << std::endl ; 
}

/** Set the control registers depending of the web page 
 */
void CLASSNAME::setCCSControlRegisters (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception) {

#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page setCCSControlRegisters accessed", LOGUSERINFO) ;
#endif


  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    if (cgi["submit"]->getValue() == "setCR0") {
      unsigned int qpllF0 = fromHexString<unsigned int>(cgi["QPLLF0SELECT"]->getValue()) ;
      if (qpllF0 > 0xF) {
	errorReportLogger_->errorReport ("Value of QPLL F0 SELECT in FEC/CCS trigger control register cannot be greater than 0xF", LOGERROR) ;
	//*out << cgicc::h2("Value of QPLL F0 SELECT in FEC/CCS trigger control register cannot be greater than 0xF").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      }
      else {
	unsigned int ccsCtrl0 = qpllF0 ;
	if (cgi.queryCheckbox("CCS_QPLL_AUTORESTART")) {
	  ccsCtrl0 |= CCS_QPLL_AUTORESTART ;
	}
	if (cgi.queryCheckbox("CCS_QPLL_RESET")) {
	  ccsCtrl0 |= CCS_QPLL_RESET ;
	}
	if (cgi.queryCheckbox("CCS_QPLL_EXTCTRL")) {
	  ccsCtrl0 |= CCS_QPLL_EXTCTRL ;
	}
	if (cgi.queryCheckbox("CCS_TTCRX_RESET")) {
	  ccsCtrl0 |= CCS_TTCRX_RESET ;
	}
	if (cgi.queryCheckbox("CCS_FORCELOCALMODE")) {
	  ccsCtrl0 |= CCS_FORCELOCALMODE ;
	}
	
	std::ostringstream msgInfo1 ; msgInfo1 << "Set the CCS Trigger control register 0 to " << std::hex << ccsCtrl0 ;
	errorReportLogger_->errorReport (msgInfo1.str(), LOGINFO) ;

	fecAccess_->setCCSTriggerConfig0 (indexFecRingChosen_, ccsCtrl0) ;
	
	if (cgi.queryCheckbox("CCS_QPLL_RESET") || cgi.queryCheckbox("CCS_TTCRX_RESET")) {
	  ccsCtrl0 &= ~CCS_QPLL_RESET ;
	  ccsCtrl0 &= ~CCS_TTCRX_RESET ;
	}

	std::ostringstream msgInfo2 ; msgInfo2 << "Set the CCS Trigger control register 0 to " << std::hex << ccsCtrl0 ;
	errorReportLogger_->errorReport (msgInfo2.str(), LOGINFO) ;

	fecAccess_->setCCSTriggerConfig0 (indexFecRingChosen_, ccsCtrl0) ;
      }
    }

    // Redisplay the page if all is fine
    this->displayCCSTrigger (in,out) ;
  }
  catch (FecExceptionHandler &e) {

    *out << "<HTML>" << std::endl ;
    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot set one the FEC/CCS trigger control register").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;
    *out << "</HTML>" << std::endl ;

    errorReportLogger_->errorReport ("Cannot set one the FEC ring control register", e, LOGERROR) ;
  }
}

/** Display all the modules and CCU 
 */
void CLASSNAME::displayCcuModules( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayCcuModules", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayCcuModules accessed", LOGUSERINFO) ;
#endif

  
  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  std::string titre = CLASSNAMESTR + ": CCU modules on FEC " + toString(getFecKey(indexFecRingChosen_)) + " Ring " + toString (getRingKey(indexFecRingChosen_)) ;
  *out << cgicc::title(titre) << std::endl;
  xgi::Utils::getPageHeader(*out, titre) ;

  // Navigation Bar
  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
    *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p();

  try {

    // ---------------------------------------------------------------------------
    // If the process is the FecSupervisor then no scan has been done before the call
    // of this method so do it here
#ifdef __FecSupervisorClass__
    // Call the method
    std::list<std::string> listErrors ; // to be displayed in the web main page
    //                                                                                           No Upload
    FecDetectionUpload::uploadDetectFECs ( *fecAccess_, *fecAccessManager_, *errorReportLogger_, NULL,
					   // No files for good/bad modules and instance
					   "", "", "", 
					   // No template file, no output file, no structure or partition name
					   "", "", "", "",
					   // Remove bad modules and DOH calibration
					   false, 
					   // No upload in files, display if it is set
					   displayDebugMessage_, multiFrames_, false, false, false,
					   // Output of the modules
					   moduleCorrect_, moduleIncorrect_, moduleCannotBeUsed_, dcuDohOnCcu_, deviceError_, deviceMissing_, listErrors,
					   // Subdetector if it is set
					   "UNKNOWN",
					   // the ring chosen
					   indexFecRingChosen_, indexFecRingChosen_) ;
    // Display the list of the errors
    if (listErrors.size() > 0) {
      std::ostringstream msgWebError ;
      for (std::list<std::string>::iterator it = listErrors.begin() ; it != listErrors.end() ; it ++) {
	msgWebError << (*it) << std::endl ;
      }
      if (displayFrameErrorMessages_) {
	if (textInformation_.str().size() > MAXWEBDISPLAY) textInformation_.str("") ;
	std::stringstream temp ; temp << "Error: " << msgWebError.str()
				      << std::endl << textInformation_.str() ;
	textInformation_.str(temp.str()) ;    
      }
    }
#endif

    // ---------------------------------------------------------------------------
    // Check for problem in the devices
    std::list<keyType> allList, allListFecRing ;
    allList.merge(moduleCorrect_) ; allList.merge(moduleIncorrect_) ; allList.merge(moduleCannotBeUsed_) ; allList.merge(dcuDohOnCcu_) ;
    for (std::list<keyType> ::iterator it = allList.begin() ; it != allList.end() ; it ++) {
      if (getFecRingKey((*it)) == indexFecRingChosen_) allListFecRing.push_back((*it)) ;
      //char msg[80] ; decodeKey(msg,(*it)) ;
      //errorReportLogger_->errorReport(msg, LOGDEBUG) ;
    }
    allListFecRing.sort() ;

    // ---------------------------------------------------------------------------
    // Display the table with the differents modules
    unsigned int ccuAddress = 0 ;
    std::string url = getApplicationDescriptor()->getURN();
    *out << "<table border=1 cellpadding=10 cellspacing=0>" << std::endl;
    for (std::list<keyType> ::iterator it = allListFecRing.begin() ; it != allListFecRing.end() ; it ++) {

      keyType index = getFecRingCcuChannelKey((*it)) ;
      unsigned int address = getAddressKey((*it)) ;

      if (ccuAddress != getCcuKey(index)) { // start a new line
	if (ccuAddress != 0) *out << "</tr>" << std::endl;
	*out << "<TR ALIGN=CENTER>" << "<td>" << std::endl ;
	*out << "<a href=\"/" << url << "/displayCCU?param1=CCU&param2=" << toHexString(getCcuKey(index)) << "\">CCU 0x" << toHexString(getCcuKey(index)) << "</a>" << "</td>" << std::endl ;
      }

      if ( (address == 0x20 || address == 0x22) ) { // it is a module

	if (deviceError_[index|setAddressKey(0x20)] || deviceMissing_[index|setAddressKey(0x20)] ||
	    deviceError_[index|setAddressKey(0x21)] || deviceMissing_[index|setAddressKey(0x21)] ||
	    deviceError_[index|setAddressKey(0x22)] || deviceMissing_[index|setAddressKey(0x22)] ||
	    deviceError_[index|setAddressKey(0x23)] || deviceMissing_[index|setAddressKey(0x23)] ||
	    deviceError_[index|setAddressKey(0x24)] || deviceMissing_[index|setAddressKey(0x24)] ||
	    deviceError_[index|setAddressKey(0x25)] || deviceMissing_[index|setAddressKey(0x25)] ||
	    deviceError_[index|setAddressKey(0x0)]  || deviceMissing_[index|setAddressKey(0x0)] ) {  // Module with error or missing APV or DCU
	  
	  *out << "<td>" << std::endl ;
	  *out << "<a href=\"/" << url << "/displayModule?param1=Module&param2=Error&param3=" << toString((*it)) << "\">Module 0x" << toHexString(getChannelKey(index)) << "</a>" << std::endl ;
	  *out << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/orangeLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">";
	  *out << "</td>" << std::endl ;
	}
	else if (deviceError_[index|setAddressKey(0x43)] || deviceMissing_[index|setAddressKey(0x43)] ||
		 deviceError_[index|setAddressKey(0x44)] || deviceMissing_[index|setAddressKey(0x44)] ||
		 deviceError_[index|setAddressKey(0x60)] || deviceMissing_[index|setAddressKey(0x60)]) { // fatal errors
	  
	  *out << "<td>" << std::endl ;
	  *out << "<a href=\"/" << url << "/displayModule?param1=Module&param2=FatalError&param3=" << toString((*it)) << "\">Module 0x" << toHexString(getChannelKey(index)) << "</a>" << std::endl ;
	  *out << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/redLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">";
	  *out << "</td>" << std::endl ;
	}
	else { // No errors 
	  
	  *out << "<td>" << std::endl ;
	  *out << "<a href=\"/" << url << "/displayModule?param1=Module&param2=NoError&param3=" << toString((*it)) << "\">Module 0x" << toHexString(getChannelKey(index)) << "</a>" << std::endl ;
	  *out << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/greenLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">";
	  *out << "</td>" << std::endl ;
	}
      }
      else { // DOH or DCU
	if (address == 0x0) { // DCU on CCU
	  *out << "<td>" << std::endl ;
	  *out << "<a href=\"/" << url << "/displayDCUCCU?param1=DCUCCU&param2=" << toHexString(getCcuKey(index)) << "&param3=" << toHexString(getChannelKey(index)) << "\">DCU/CCU 0x" << toHexString(getChannelKey(index)) << "</a>" << std::endl ;
	  *out << "</td>" << std::endl ;
	}
	else if (address == 0x70) {
	  *out << "<td>" << std::endl ;
	  *out << "<a href=\"/" << url << "/displayDOH?param1=DOH&param2=" << toHexString(getCcuKey(index)) << "&param3=" << toHexString(getChannelKey(index)) << "\">DOH 0x" << toHexString(getChannelKey(index)) << "</a>" << std::endl ;
	  *out << "</td>" << std::endl ;
	}
	else {
	  *out << "<td>" << std::endl ;
	  *out << "Unknown 0x" << toHexString(getChannelKey(index)) << std::endl ;
	  *out << "</td>" << std::endl ;
	}
      }
      ccuAddress = getCcuKey(index) ;
    }
    *out << "</tr></table>" << std::endl;
  }
  catch (FecExceptionHandler &e) {

    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot retreive the list of CCUs and modules").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;
  }
  *out << "</HTML>" << std::endl ; 
}

/** Display a CCU
 */
void CLASSNAME::displayCCU ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayCCU", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayCCU accessed", LOGUSERINFO) ;
#endif

  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  std::string titre = CLASSNAMESTR + ": Display CCU from FEC " + toString(getFecKey(indexFecRingChosen_)) + " Ring " + toString (getRingKey(indexFecRingChosen_)) ;
  *out << cgicc::title(titre) << std::endl;
  xgi::Utils::getPageHeader(*out, titre) ;

  *out << cgicc::title(CLASSNAMESTR + " Display CCU") << std::endl;

  // Navigation Bar
  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
    *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p();

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, XDAQFEC_HARDWAREACCESSERROR, partitionName_.toString()) ;
    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    
    return ;
  }

  // retreive the parameter
  try {

    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    std::string strCCU = cgi["param2"]->getValue() ;
    //errorReportLogger_->errorReport("strCCU = " + strCCU, LOGDEBUG) ;

    std::string str = "CCU 0x" + strCCU + " on FEC " + toString(getFecKey(indexFecRingChosen_)) + " Ring " + toString(getRingKey(indexFecRingChosen_)) ;
    *out << cgicc::h3(str).set("style", "font-family: arial") << std::endl;

    unsigned short ccuVal = fromHexString<unsigned short>(strCCU) ;
    keyType indexCCU = buildCompleteKey(getFecKey(indexFecRingChosen_), getRingKey(indexFecRingChosen_), ccuVal, 0,0) ;

    std::string url = "/";
    url += getApplicationDescriptor()->getURN();
    url += "/setCcuControlRegisters" ;
    
    *out << cgicc::form().set("method","post")
      .set("action", url)
      .set("enctype","multipart/form-data") << std::endl;

    *out << "<table border=1 cellpadding=10 cellspacing=10 valign=\"top\">" << std::endl;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << std::endl ;

    // display each CCU control registers
    *out << "<table border=1 cellpadding=10 cellspacing=0>" << std::endl;
    *out << "<tr>" << std::endl ;
    *out << "<th>" << "Register" << "</th>" ;
    *out << "<th>" << "Value" << "</th>" ;
    *out << "<th>" << "Set it" << "</th>" ;
    *out << "</tr>" << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << "CRA" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","CRA").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuCRA(indexCCU))) << "</td>" << std::endl;
    *out << "<td>" << cgicc::input().set("type", "checkbox").set("name","checkCRA") << "</td>" << std::endl;
    *out << cgicc::tr() << std::endl ;
    *out << "<td>" << "CRB" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","CRB").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuCRB(indexCCU))) << "</td>" << std::endl;
    *out << "<td>" << cgicc::input().set("type", "checkbox").set("name","checkCRB") << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << "CRC" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","CRC").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuCRC(indexCCU))) << "</td>" << std::endl;
    *out << "<td>" << cgicc::input().set("type", "checkbox").set("name","checkCRC") << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << "CRD" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","CRD").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuCRD(indexCCU))) << "</td>" << std::endl;
    *out << "<td>" << cgicc::input().set("type", "checkbox").set("name","checkCRD") << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << "CRE" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","CRE").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuCRE(indexCCU))) << "</td>" << std::endl;
    *out << "<td>" << cgicc::input().set("type", "checkbox").set("name","checkCRE") << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "</table>" << std::endl ;

    *out << "</td>" << std::endl ;
    *out << "<td>" << std::endl ;

    // display each CCU control registers
    *out << "<table border=1 cellpadding=10 cellspacing=0>" << std::endl;
    *out << "<tr>" << std::endl ;
    *out << "<th>" << "Register" << "</th>" ;
    *out << "<th>" << "Value" << "</th>" ;
    *out << cgicc::tr() << std::endl ;
    *out << "<td>" << "SRA" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","SRA").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuSRA(indexCCU))) << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << "SRB" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","SRB").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuSRB(indexCCU))) << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << "SRC" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","SRC").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuSRC(indexCCU))) << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << "SRD" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","SRD").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuSRD(indexCCU))) << "</td>" << std::endl;
    *out << cgicc::tr() << std::endl ;
    *out << "<td>" << "SRE" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","SRE").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuSRE(indexCCU))) << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "<tr>" << std::endl ;
    *out << "<td>" << "SRF" << "</td>" << std::endl ;
    *out << "<td>" << cgicc::input().set("type","text").set("align","center").set("name","SRF").set("size","8").set("align","center").set("value","0x" + toHexString((unsigned int)fecAccess_->getCcuSRF(indexCCU))) << "</td>" << std::endl;
    *out << "</tr>" << std::endl ;
    *out << "</table>" << std::endl ;

    *out << "</td>" << std::endl ;
    *out << "</tr>" << std::endl ;
    *out << "</table>" << std::endl ;

    // Suspend the DCU thread during the download in order to avoid
    // Multiple access -> the synchronisation is done in the DcuThread with mutex
    // errorReportLogger_->errorReport( CLASSNAME << ": Release the semaphore" << std::endl ;
    // mutexDcuThread_->give() ;

    // Transmission of the FEC/Ring/CCU values
    *out << cgicc::input().set("type","hidden").set("value",cgi["param2"]->getValue()).set("name","param2") << std::endl ;

    // Apply
    *out << cgicc::br() << "<CENTER>" ;
    *out << cgicc::p() << cgicc::input().set("type", "submit")
      .set("name", "submit")
      .set("value", "Apply");
    *out << "</CENTER>" ;

    // End of the form
    *out << cgicc::form() << std::endl;
  }
  catch (FecExceptionHandler &e) {

    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot retreive the list of CCUs and modules").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;
  }
  catch(const std::exception& e) {

    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }
  *out << "</HTML>" << std::endl ; 
}

/** Set the CCU control registers depending of the web page displayCCU
 */
void CLASSNAME::setCcuControlRegisters (xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport( "setCcuControlRegisters", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page setCcuControlRegisters accessed", LOGUSERINFO) ;
#endif

  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    // Retreive the CCU address
    unsigned short ccuVal = fromHexString<unsigned short>(cgi["param2"]->getValue()) ;
    keyType indexCCU = buildCompleteKey(getFecKey(indexFecRingChosen_), getRingKey(indexFecRingChosen_), ccuVal, 0,0) ;

    //setCRA
    if (cgi.queryCheckbox("checkCRA")) {
      std::string strCRA = cgi["CRA"]->getValue() ;
      unsigned short CRA = fromHexString<unsigned int>(strCRA) ;
      std::ostringstream msgDebug ; msgDebug << "Set the CCU 0x" << std::hex << ccuVal << " control register A to " << CRA ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      fecAccess_->setCcuCRA (indexCCU, CRA) ;
    }

    //setCRB
    if (cgi.queryCheckbox("checkCRB")) {
      std::string strCRB = cgi["CRB"]->getValue() ;
      unsigned short CRB = fromHexString<unsigned int>(strCRB) ;
      std::ostringstream msgDebug ; msgDebug << "Set the CCU 0x" << std::hex << ccuVal << " control register B to " << CRB ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      fecAccess_->setCcuCRB (indexCCU, CRB) ;
    }

    //setCRC
    if (cgi.queryCheckbox("checkCRC")) {
      std::string strCRC = cgi["CRC"]->getValue() ;
      unsigned short CRC = fromHexString<unsigned int>(strCRC) ;
      std::ostringstream msgDebug ; msgDebug << "Set the CCU 0x" << std::hex << ccuVal << " control register C to " << CRC ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      fecAccess_->setCcuCRC (indexCCU, CRC) ;
    }

    //setCRD
    if (cgi.queryCheckbox("checkCRD")) {
      std::string strCRD = cgi["CRD"]->getValue() ;
      unsigned short CRD = fromHexString<unsigned int>(strCRD) ;
      std::ostringstream msgDebug ; msgDebug << "Set the CCU 0x" << std::hex << ccuVal << " control register D to " << CRD ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      fecAccess_->setCcuCRD (indexCCU, CRD) ;
    }

    //setCRE
    if (cgi.queryCheckbox("checkCRE")) {
      std::string strCRE = cgi["CRE"]->getValue() ;
      unsigned int CRE = fromHexString<unsigned int>(strCRE) ;
      std::ostringstream msgDebug ; msgDebug << "Set the CCU 0x" << std::hex << ccuVal << " control register E to " << CRE ;
      errorReportLogger_->errorReport (msgDebug.str(), LOGDEBUG) ;
      fecAccess_->setCcuCRE (indexCCU, CRE) ;
    }

    // Hidden parameter for the CCU address
    //*out << cgicc::input().set("type","hidden").set("value",cgi["param2"]->getValue()).set("name","ccuAddress") << std::endl ;
    // Redisplay the page if all is fine
    //this->displayCCU (in,out) ; => this page cannot redisplay, it contains a form, exception is created on the creation of the cgicc in displayCCU
  }
  catch (FecExceptionHandler &e) {

    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot set one the FEC ring control register").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;

    errorReportLogger_->errorReport ("Cannot set one the FEC ring control register", e, LOGERROR) ;
  }
  catch(const std::exception& e) {
    
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  // Suspend the DCU thread during the download in order to avoid
  // Multiple access -> the synchronisation is done in the DcuThread with mutex
  // errorReportLogger_->errorReport( CLASSNAME << ": Release the semaphore" << std::endl ;
  // mutexDcuThread_->give() ;
}

/** Display a module
 */
void CLASSNAME::displayModule ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {

  //*out << cgicc::HTTPHTMLHeader();
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayModule accessed", LOGUSERINFO) ;
#endif

  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title(CLASSNAMESTR + ": Display Module") << std::endl;
  xgi::Utils::getPageHeader(*out, CLASSNAMESTR + ": Module Display");

  // Navigation Bar
  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
    *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p();

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, XDAQFEC_HARDWAREACCESSERROR, partitionName_.toString()) ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;

    *out << "</HTML>" << std::endl ;     

    return ;
  }

  // retreive the parameter
  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    keyType indexF = fromString<unsigned int>(cgi["param3"]->getValue()) ;
    keyType index = getFecRingCcuChannelKey(indexF) ;
    
    std::string str = "Module on FEC " + toString(getFecKey(index)) + " Ring " + toString(getRingKey(index)) + " CCU 0x" + toHexString(getCcuKey(index)) + " Channel 0x" + toHexString(getChannelKey(index)) ;
    errorReportLogger_->errorReport (str, LOGDEBUG) ;
    *out << cgicc::h3(str).set("style", "font-family: arial") << std::endl;

    // moduleCorrect_, moduleIncorrect_, moduleCannotBeUsed_, dcuDohOnCcu_, deviceError_, deviceMissing_
    if ( (getAddressKey(indexF) != 0x20) && (getAddressKey(indexF) != 0x22) ) {
      *out << cgicc::br() ;
      std::ostringstream msgError ; msgError << "Address " << std::hex << getAddressKey(index) << " incorrect for a module" ;
      *out << cgicc::h2(msgError.str()).set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    }
    else {

      std::string moduleStr = cgi["param2"]->getValue() ;
      if (moduleStr == "NoError") *out << cgicc::h2("Module Correct") << std::endl ;
      else if (moduleStr == "Error") *out << cgicc::h2("Module Incorrect but will be used in the DAQ") << std::endl ;
      else if (moduleStr == "FatalError") *out << cgicc::h2("Module Incorrect and CANNOT be used in the DAQ") << std::endl ;

      *out << "<ul>" << std::endl ;
      if (deviceMissing_[index|setAddressKey(0x20)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">APV 0x20: Missing device</span></li>" << std::endl;
      else if (deviceError_[index|setAddressKey(0x20)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">APV 0x20: Error on access</span></li>" << std::endl;
      else *out << "<li><span style=\"color: rgb(0, 204, 0);\">APV 0x20: Ok</span></li>" << std::endl;
      if (deviceMissing_[index|setAddressKey(0x21)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">APV 0x21: Missing device</span></li>" << std::endl;
      else if (deviceError_[index|setAddressKey(0x21)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">APV 0x21: Error on access</span></li>" << std::endl;
      else *out << "<li><span style=\"color: rgb(0, 204, 0);\">APV 0x21: Ok</span></li>" << std::endl;
      if (getAddressKey(index) != 0x22) {
	if (deviceMissing_[index|setAddressKey(0x22)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">APV 0x22: Missing device</span></li>" << std::endl;
	else if (deviceError_[index|setAddressKey(0x22)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">APV 0x22: Error on access</span></li>" << std::endl;
	else *out << "<li><span style=\"color: rgb(0, 204, 0);\">APV 0x22: Ok</span></li>" << std::endl;
	if (deviceMissing_[index|setAddressKey(0x23)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">APV 0x23: Missing device</span></li>" << std::endl;
	else if (deviceError_[index|setAddressKey(0x23)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">APV 0x24: Error on access</span></li>" << std::endl;
	else *out << "<li><span style=\"color: rgb(0, 204, 0);\">APV 0x23: Ok</span></li>" << std::endl;
      }
      if (deviceMissing_[index|setAddressKey(0x24)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">APV 0x24: Missing device</span></li>" << std::endl;
      else if (deviceError_[index|setAddressKey(0x24)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">APV 0x24: Error on access</span></li>" << std::endl;
      else *out << "<li><span style=\"color: rgb(0, 204, 0);\">APV 0x24: Ok</span></li>" << std::endl;
      if (deviceMissing_[index|setAddressKey(0x25)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">APV 0x25: Missing device</span></li>" << std::endl;
      else if (deviceError_[index|setAddressKey(0x25)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">APV 0x25: Error on access</span></li>" << std::endl;
      else *out << "<li><span style=\"color: rgb(0, 204, 0);\">APV 0x25: Ok</span></li>" << std::endl;
      if (deviceMissing_[index|setAddressKey(0x43)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">MUX 0x43: Missing device</span></li>" << std::endl;
      else if (deviceError_[index|setAddressKey(0x43)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">APV 0x43: Error on access</span></li>" << std::endl;
      else *out << "<li><span style=\"color: rgb(0, 204, 0);\">MUX 0x43: Ok</span></li>" << std::endl;
      if (deviceMissing_[index|setAddressKey(0x44)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">PLL 0x44: Missing device</span></li>" << std::endl;
      else if (deviceError_[index|setAddressKey(0x44)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">PLL 0x44: Error on access</span></li>" << std::endl;
      else *out << "<li><span style=\"color: rgb(0, 204, 0);\">PLL 0x44: Ok</span></li>" << std::endl;
      if (deviceMissing_[index|setAddressKey(0x60)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">AOH 0x60: Missing device</span></li>" << std::endl;
      else if (deviceError_[index|setAddressKey(0x60)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">AOH 0x60: Error on access</span></li>" << std::endl;
      else *out << "<li><span style=\"color: rgb(0, 204, 0);\">AOH 0x60: Ok</span></li>" << std::endl;
      if (deviceMissing_[index|setAddressKey(0x0)]) *out << "<li><span style=\"color: rgb(204, 0, 0);\">DCU 0x0: Missing device</span></li>" << std::endl;
      else if (deviceError_[index|setAddressKey(0x0)]) *out << "<li><span style=\"color: rgb(04, 0, 0);\">DCU 0x0: Error on access</span></li>" << std::endl;
      else *out << "<li><span style=\"color: rgb(0, 204, 0);\">DCU 0x0: Ok</span></li>" << std::endl;
      *out << "</ul>" << std::endl ;
    }
  }
  catch(const std::exception& e) {
    
    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }

  *out << "</HTML>" << std::endl ; 
}

/** Display a DCU on a CCU
 */
void CLASSNAME::displayDCUCCU ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayDCUCCU", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayDCUCCU accessed", LOGUSERINFO) ;
#endif

  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title(CLASSNAMESTR + "Display DCU on CCU") << std::endl;

  // Navigation Bar
  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
    *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p();

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, XDAQFEC_HARDWAREACCESSERROR, partitionName_.toString()) ;
    *out << cgicc::td() << std::endl;
    //*out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::td() << std::endl;

    *out << "</HTML>" << std::endl ;     

    return ;
  }

  // retreive the parameter
  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    std::string str = "DCU on FEC " + toString(getFecKey(indexFecRingChosen_)) + " Ring " + toString(getRingKey(indexFecRingChosen_)) + "CCU 0x" + cgi["param2"]->getValue() + " Channel 0x" + cgi["param3"]->getValue() ;
    *out << cgicc::h3(str).set("style", "font-family: arial") << std::endl;

    //unsigned short ccuVal = fromHexString<unsigned short>(cgi["param2"]->getValue()) ;
    //unsigned short i2cchannel = fromHexString<unsigned short>(cgi["param3"]->getValue()) ;
    //keyType indexM = buildCompleteKey(getFecKey(indexFecRingChosen_), getRingKey(indexFecRingChosen_), ccuVal, i2cchannel, 0) ;
  }
  catch(const std::exception& e) {

    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }
  *out << "</HTML>" << std::endl ; 
}

/** Display a DOH
 */
void CLASSNAME::displayDOH ( xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayDOH", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayDOH accessed", LOGUSERINFO) ;
#endif
  
  //*out << cgicc::HTTPHTMLHeader();
  *out << "<HTML>" << std::endl ;
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::title(CLASSNAMESTR + ": Display DOH") << std::endl;

  // Navigation Bar
  for (unsigned int i = 0; i < hardwareCheckNavigation_.size(); i++) {
    *out << "[" << hardwareCheckNavigation_[i] << "] ";
  }
  *out << cgicc::p();

  // Check the the accesses was created
  if (fecAccessManager_ == NULL) {

    errorReportLogger_->errorReport ("Hardware access was not created, no command can be applied", LOGFATAL, 1, XDAQFEC_HARDWAREACCESSERROR, partitionName_.toString()) ;
    *out << cgicc::td() << std::endl;
    //*out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Hardware access was not created, no command can be applied").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::td() << std::endl;

    *out << "</HTML>" << std::endl ;     

    return ;
  }

  // retreive the parameter
  try {
    // Create a new Cgicc object containing all the CGI data
    cgicc::Cgicc cgi(in);

    std::string str = "DOH on FEC " + toString(getFecKey(indexFecRingChosen_)) + " Ring " + toString(getRingKey(indexFecRingChosen_)) + "CCU 0x" + cgi["param2"]->getValue() + " Channel 0x" + cgi["param3"]->getValue() ;
    *out << cgicc::h3(str).set("style", "font-family: arial") << std::endl;

    //unsigned short ccuVal = fromHexString<unsigned short>(cgi["param2"]->getValue()) ;
    //unsigned short i2cchannel = fromHexString<unsigned short>(cgi["param3"]->getValue()) ;
    //keyType indexM = buildCompleteKey(getFecKey(indexFecRingChosen_), getRingKey(indexFecRingChosen_), ccuVal, i2cchannel, 0) ;
  }
  catch(const std::exception& e) {

    XCEPT_RAISE(xgi::exception::Exception,  e.what());
    errorReportLogger_->errorReport (e.what(), LOGERROR) ;
  }
  *out << "</HTML>" << std::endl ; 
}

/** Display information about a ring
 */
void CLASSNAME::displayFecRingField (xgi::Input * in, xgi::Output * out, unsigned int fecSlot) throw (xgi::exception::Exception) {

  //errorReportLogger_->errorReport("displayHardwareCheck", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayFecRingField accessed", LOGUSERINFO) ;
#endif

  std::string url = getApplicationDescriptor()->getURN();

  // Read the FEC hardware ID, firmware version, VME firmware version, temperatures
  static double temp1Avergage = 0, temp2Avergage = 0, countTemp = 0 ;
  int tempSensor1 = 0, tempSensor2 = 0 ;
  std::string ringFirmwareVersion = "Error during the check of the VME version" ;
  std::string vmeFirmwareVersion  = "Error during the check of the VME version" ;
  std::string triggerFirmwareVersion  = "Error during the check of the VME version" ;
  std::string fecHardwareId = "Error during the check of the FEC hardware ID" ;
  std::string triggerStatus0Str = "Error during getting the FEC trigger status register 0" ;
  std::string triggerConfig0Str = "Error during getting the FEC trigger configuration register 0" ;
  std::string ssidName = "Error during getting SSID" ;
  unsigned int triggerConfig0Value = 0 ;
  unsigned int triggerStatus0Value = 0 ;
  bool triggerStatus0Flag = false ;
  bool triggerConfig0Flag = false ;
  bool error = false ;
  try {
    fecHardwareId = fecAccess_->getFecHardwareId (buildFecRingKey(fecSlot, FecVmeRingDevice::getMinVmeFecRingValue())) ;
    try {
      triggerConfig0Value = fecAccess_->getCCSTriggerConfig0(buildFecRingKey(fecSlot, FecVmeRingDevice::getMinVmeFecRingValue())) ;
      triggerConfig0Str = "0x" + toHexString(triggerConfig0Value) ;
      triggerConfig0Flag = true ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("Cannot read one of the trigger status/control registers", e, LOGERROR) ;
    }
    try { 
      triggerStatus0Value = fecAccess_->getCCSTriggerStatus0(buildFecRingKey(fecSlot, FecVmeRingDevice::getMinVmeFecRingValue())) ;
      triggerStatus0Str = "0x" + toHexString(triggerStatus0Value) ;
      triggerStatus0Flag = true ;
      ssidName = fecAccess_->getSSID(buildFecRingKey(fecSlot, FecVmeRingDevice::getMinVmeFecRingValue())) ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("Cannot read one of the trigger status/control registers", e, LOGERROR) ;
    }
    fecAccess_->getFecTemperature (buildFecRingKey(fecSlot, FecVmeRingDevice::getMinVmeFecRingValue()), tempSensor1, tempSensor2) ;
    temp1Avergage = (((temp1Avergage * countTemp) + tempSensor1) / (countTemp+1)) ;
    temp2Avergage = (((temp2Avergage * countTemp) + tempSensor2) / (countTemp+1)) ;
    countTemp += 1 ; 
    ringFirmwareVersion = toHexString(fecAccess_->getFecFirmwareVersion(buildFecRingKey(fecSlot,FecVmeRingDevice::getMinVmeFecRingValue()))) ;
    try {
      vmeFirmwareVersion  = toHexString(fecAccess_->getVmeVersion(buildFecRingKey(fecSlot,FecVmeRingDevice::getMinVmeFecRingValue()))) ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("Cannot read one the VME firmware version", e, LOGERROR) ;
    }
    try {
      triggerFirmwareVersion  = toHexString(fecAccess_->getTriggerVersion(buildFecRingKey(fecSlot,FecVmeRingDevice::getMinVmeFecRingValue()))) ;
    }
    catch (FecExceptionHandler &e) {
      errorReportLogger_->errorReport ("Cannot read one the trigger firmware version", e, LOGERROR) ;
    }
  }
  catch (FecExceptionHandler &e) {
      
    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot read one or several FEC registers").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;
      
    errorReportLogger_->errorReport ("Cannot read one or several FEC registers", e, LOGERROR) ;
    error = true ;
  }

  if (!error) {

    // Display the information about the fec slot
    *out << cgicc::fieldset() << std::endl;
    *out << "<a name=\"FEC" << std::dec << fecSlot << "\"></a>" ;
    *out << cgicc::p() << cgicc::legend("FEC Information") << std::endl;
      
    *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("FEC hardware ID") << std::endl;
    *out << "</td>"; *out << "<td>";
    *out << fecHardwareId ;
    *out << "</td>" << "</tr>";
    *out << "<tr>" << std::endl;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("FEC firmware version") << std::endl;
    *out << "</td>"; *out << "<td>";
    *out << ringFirmwareVersion ;
    *out << "</td>" << "</tr>";
    *out << "<tr>" << std::endl;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("VME firmware version") << std::endl;
    *out << "</td>"; *out << "<td>";
    *out << vmeFirmwareVersion ;
    *out << "</td>";
    *out << "<tr>" << std::endl;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Trigger firmware version") << std::endl;
    *out << "</td>"; *out << "<td>";
    *out << triggerFirmwareVersion ;
    *out << "</td>";
    *out << "<tr>" << std::endl;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Trigger FEC Status") << std::endl;
    *out << "</td>";
    if (triggerStatus0Flag) {
      *out << "<td>" << triggerStatus0Str << "</td>";
      *out << "<td>" ;
      if (triggerStatus0Value & CCS_SR0_QPLL_ERROR) 
	*out << FEC_RED << "QPLL Error" << "</span>";
      else
	*out << FEC_GREEN << "QPLL Ok" << "</span>";
      *out << " / " ;
      if (triggerStatus0Value & CCS_SR0_QPLL_LOCKED) 
	*out << FEC_GREEN << "QPLL locked" << "</span>";
      else
	*out << FEC_RED << "QPLL not lock" << "</span>";
      *out << " / " ;
      if (triggerStatus0Value & CCS_SR0_TTCRX_READY) 
	*out << FEC_GREEN << "TTCrx ready" << "</span>";
      else
	*out << FEC_RED << "TTCrx not ready" << "</span>";
      *out << "</td>" ;
    }
    *out << "<tr>" << std::endl;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Trigger FEC Configuration") << std::endl;
    *out << "</td>"; 
    *out << "<td>" << triggerConfig0Str << "</td>";
    *out << "<tr>" << std::endl;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("FEC SSID") << std::endl;
    *out << "</td>"; *out << "<td>";
    *out << ssidName ;
    *out << "</td>";
    *out << "<tr>" << std::endl;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Bottom Temperature Sensor") << std::endl;
    *out << "</td>"; *out << "<td>";
    *out << std::dec << tempSensor1 << " degree Celsius" ;
    *out << "</td>"; *out << "<td>"; 
    *out << std::dec << "(Mean: " << std::dec << temp1Avergage << " degree Celsius)" ;
    *out << "</td>" << "</tr>";
    *out << "<tr>" << std::endl;
    //*out << "<td>"; *out << "</td>" << std::endl;
    *out << "<td>"; 
    *out << cgicc::label("Top Temperature Sensor") << std::endl;
    *out << "</td>"; *out << "<td>";
    *out << std::dec << tempSensor2 << " degree Celsius" ;
    *out << "</td>";
    *out << "<td>"; 
    *out << std::dec << "(Mean: " << std::dec << temp2Avergage << " degree Celsius)" ;
    *out << "</td>";
    *out << "</tr>";
  }
  else {

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::p() << cgicc::legend("FEC Information") << std::endl;
      
    *out << cgicc::table().set("border","0").set("cellpadding","10").set("cellspacing","0") << std::endl ;
  }

  // ----------------------------------------------------------------------
  // retreive the list of the FECs
  std::list<keyType> *fecList = NULL ;
  try {
    fecList = fecAccess_->getFecList() ;
    if ((fecList == NULL) || (fecList->empty())) {
	
      *out << "<tr>" << std::endl;
      //*out << "<td>"; *out << "</td>" << std::endl;
      *out << "<td>"; 
      *out << cgicc::h2("No FEC/ring detected").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
      *out << "</td>";
      *out << "</tr>";
    }
    else {
	
      // Find a ring to select it if it is not
      bool findFec = false ;
      for (std::list<keyType>::iterator p=fecList->begin();p!=fecList->end();p++) 
	if (*p == indexFecRingChosen_) findFec = true ;
      if (!findFec)
	indexFecRingChosen_ = *(fecList->begin()) ;

      *out << cgicc::tr() << cgicc::td() << "<BOLD>FEC / Ring" << cgicc::td() << std::endl ;
      *out << cgicc::td() << "Status</BOLD>" << cgicc::td() << cgicc::tr() << std::endl ;

      // Retreive the FEC list
      for (std::list<keyType>::iterator p=fecList->begin();p!=fecList->end();p++) {
	
	keyType index = *p ;

	if (fecSlot == getFecKey(index)) {
	
	  std::ostringstream msgInfo ; msgInfo << "FEC " << std::dec << (int)getFecKey(index) << "." << (int)getRingKey(index) ;
	  
	  //std::string strFecRing = toString(getFecKey(index)) + "." + toString(getRingKey(index)) ;
	  //errorReportLogger_->errorReport (msgInfo.str(), LOGINFO) ;
	  
	  // Clear the errors and the pending IRQ
	  if (!isFecSR0Correct(fecAccess_->getFecRingSR0(index))) {
	    fecAccess_->setFecRingCR1(index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;
	  }
	  
	  *out << cgicc::tr() << cgicc::td() << std::endl ;
	  *out << toolbox::toString("<a href=\"/%s/HardwareCheckNavigation?param1=%x&param2=%x\" target=\"_blank\">%s</a>",url.c_str(),getFecKey(index),getRingKey(index),msgInfo.str().c_str()) ;
	  *out << cgicc::td() << std::endl ;
	  if (isFecSR0Correct(fecAccess_->getFecRingSR0(index)))
	    *out << cgicc::td() << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/greenLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">" << cgicc::td() ;
	  else
	    if (fecAccess_->getFecRingSR0(index) & FEC_SR0_LINKINITIALIZED)
	      *out << cgicc::td() << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/orangeLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">" << cgicc::td() ;
	    else
	      *out << cgicc::td() << "<img border=\"0\" src=\"http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/img/redLed.png\" title=\"FEC ring\" alt=\"\" style=\"width: 10px; height: 10px;\">" << cgicc::td() ;
	  
	  *out << cgicc::td() ;
	  *out << toolbox::toString("<a href=\"/%s/HardwareCheckNavigation?param1=%x&param2=%x\" target=\"_blank\">Click here for more information</a>",url.c_str(),getFecKey(index),getRingKey(index)) ;
	  *out << cgicc::td() << cgicc::tr() << std::endl;
	}
      }
    }
  }
  catch (FecExceptionHandler &e) {

    *out << cgicc::br() ;
    *out << cgicc::p() << "A failure has occured. Last known error was:" << std::endl;
    *out << cgicc::h2("Cannot retreive the list of the FEC and their status").set("style","font-size: 15pt;  font-family: arial; color: rgb(255, 0, 0); font-weight: bold;") << std::endl;
    *out << cgicc::p() << e.what() << std::endl ;

    errorReportLogger_->errorReport ("Cannot retreive the list of the FEC and their status", e, LOGERROR) ;
  }
   
  delete fecList ;
    
  *out << cgicc::table() << std::endl;
  *out << cgicc::fieldset() << std::endl;
}

/** Display the diagnostic page
 */
void CLASSNAME::displayConfigureDiagSystem ( xgi::Input * in, xgi::Output * out ) {

  //errorReportLogger_->errorReport("displayFecHardwareCheck", LOGDEBUG) ;
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayConfigureDiagSystem accessed", LOGUSERINFO) ;
#endif

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

// ------------------------------------------------------------------------------------------------------------ //
// Related links
// ------------------------------------------------------------------------------------------------------------ //

/** Display all the parameters
 */
void CLASSNAME::displayRelatedLinks(xgi::Input * in, xgi::Output * out, bool withHTMLTag) throw (xgi::exception::Exception) {
#ifdef __FecSupervisorClass__
  errorReportLogger_->errorReport ("page displayRelatedLinks accessed", LOGUSERINFO) ;
#endif

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
  for (unsigned int i = 0; i < relatedLinksNavigation_.size(); i++) {
    *out << "[" << relatedLinksNavigation_[i] << "] " ;
  }
  *out << "</p>" << std::endl ;

  if (withHTMLTag) {
    //xgi::Utils::getPageFooter(*out);
    *out << "</HTML>" << std::endl ;
  }
}

/** Return the time with a format of string
 */
static std::string getCurrentTime(){
  static time_t tnow_;
  tnow_= time(0);
  std::string mytime(ctime(&tnow_));
  if (mytime[mytime.size()-1]=='\n'){
    //cout <<"Cut away the endl... "<<endl;
    mytime[mytime.size()-1]=' '; // better remove it!
  }
  return mytime;
}

