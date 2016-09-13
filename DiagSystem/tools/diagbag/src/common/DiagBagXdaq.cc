/*
   FileName : 		DiagBag.cc

   Content : 		DiagBag module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		DiagSystem 2.1

   Date of last modification : 20/06/2006

   Support : 		mail to : laurent.gross@ires.in2p3.fr
   
   Online help : 	https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem
*/
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

Copyright 2005, Laurent GROSS - IReS/IN2P3
*/
#include <sstream>
#include <diagbag/DiagBagXdaq.h>


DiagBagXdaq::DiagBagXdaq(std::string l1name,
                        log4cplus::Logger& logger,
                        std::string classname,
                        unsigned long instance,
                        unsigned long lid)
                        : DiagBag (false, "TRACE")
{
    diagName_= "ErrorDispatcher";
    useDiag_=false;
    diagInstance_="0";
    diagIP_="127.0.0.1";
    diagPort_="1972";
    diagLID_="31";

    lvl1Name_=l1name;
    useLvl1_=false;
    lvl1Instance_="0";
    lvl1IP_="127.0.0.1";
    lvl1Port_="1972";
    lvl1LID_="32";

    diagNameStatus_ = "unchecked";
    useDiagStatus_ =  "unchecked";
    diagInstanceStatus_ =  "unchecked";
    diagIPStatus_ =  "unchecked";
    diagPortStatus_ =  "unchecked";
    diagLIDStatus_ =  "unchecked";

    lvl1NameStatus_ =  "unchecked";
    useLvl1Status_ =  "unchecked";
    lvl1InstanceStatus_ =  "unchecked";
    lvl1IPStatus_ =  "unchecked";
    lvl1PortStatus_ =  "unchecked";
    lvl1LIDStatus_ =  "unchecked";

    edUsable_=false;
    emUsable_=false;

    forcedDbLogs_ = false;
    forcedCsLogs_ = false;
    forcedCcLogs_ = false;

    dbLogLevel_="TRACE";
    csLogLevel_="TRACE";
    ccLogLevel_="TRACE";
    gLogLevel_="TRACE";

    oldDiagIP_="";
    oldDiagPort_="";
    oldDiagName_="";
    oldDiagInstance_="";
    oldDiagLID_="";
    socketSoapExists_ = false;

    callingAppLogger_ = &logger;
    callingAppClassName_ = classname;
    callingAppInstance_ = instance;
    callingAppLID_ = lid;

    consoleDumpEnabled_ = false;
    consoleDumpLevel_ = "TRACE";
}



DiagBagXdaq::DiagBagXdaq(std::string l1name)
{
    diagName_= "ErrorDispatcher";
    useDiag_=false;
    diagInstance_="0";
    diagIP_="127.0.0.1";
    diagPort_="1972";
    diagLID_="30";

    lvl1Name_=l1name;
    useLvl1_=false;
    lvl1Instance_="0";
    lvl1IP_="127.0.0.1";
    lvl1Port_="1972";
    lvl1LID_="32";

    diagNameStatus_ = "unchecked";
    useDiagStatus_ =  "unchecked";
    diagInstanceStatus_ =  "unchecked";
    diagIPStatus_ =  "unchecked";
    diagPortStatus_ =  "unchecked";
    diagLIDStatus_ =  "unchecked";

    lvl1NameStatus_ =  "unchecked";
    useLvl1Status_ =  "unchecked";
    lvl1InstanceStatus_ =  "unchecked";
    lvl1IPStatus_ =  "unchecked";
    lvl1PortStatus_ =  "unchecked";
    lvl1LIDStatus_ =  "unchecked";

    edUsable_=false;
    emUsable_=false;

    forcedDbLogs_ = false;
    forcedCsLogs_ = false;
    forcedCcLogs_ = false;

    dbLogLevel_="TRACE";
    csLogLevel_="TRACE";
    ccLogLevel_="TRACE";
    gLogLevel_="TRACE";

    oldDiagIP_="";
    oldDiagPort_="";
    oldDiagName_="";
    oldDiagInstance_="";
    oldDiagLID_="";
    socketSoapExists_ = false;

    consoleDumpEnabled_ = false;
    consoleDumpLevel_ = "TRACE";
}



std::string DiagBagXdaq::forgeErrorString(std::string msg,
                                        std::string text,
                                        int errorcode,
                                        std::string faultstate,
                                        std::string systemid,
                                        std::string subsystemid,
                                        std::string classname,
                                        unsigned long instance,
                                        unsigned long lid,
                                        std::string lvl1name,
                                        unsigned long lvl1instance,
                                        bool emusable,
                                        std::string dbloglevel,
                                        std::string csloglevel,
                                        std::string ccloglevel,
                                        bool forceddblogs,
                                        bool forcedcslogs,
                                        bool forcedcclogs,
                                        std::string gloglevel)
{
    std::string message;
    message += msg;
    message += "\"";
    message += " text=\"" + text + "\"";
    std::stringstream s1;
    s1 << errorcode;
    message += " errorcode=\"" + s1.str() + "\"";
    message += " faultstate=\"" + faultstate + "\"";
    message += " systemid=\"" + systemid + "\"";
    message += " subsystemid=\"" + subsystemid+ "\"";
    message += " classname=\""+ classname +"\"";
    std::stringstream s2;
    s2 << instance;
    message += " instance=\""+ s2.str() +"\"";
    std::stringstream s3;
    s3 << lid;
    message += " lid=\""+ s3.str() +"\"";
    message += " lvl1name=\""+ lvl1name +"\"";
    std::stringstream s4;
    s4 << lvl1instance;
    message += " lvl1instance=\""+ s4.str() +"\"";
    if (emusable == true) { message += " routetoem=\"true\""; } else message += " routetoem=\"false\"";
    message += " dbloglevel=\"" + dbloglevel + "\"";
    message += " csloglevel=\"" + csloglevel +"\"";
    message += " ccloglevel=\"" + ccloglevel + "\"";
    message += " gloglevel=\"" + gloglevel + "\"";
    if (forceddblogs == true) { message += " forceddblogs=\"true\""; } else message += " forceddblogs=\"false\"";
    if (forcedcslogs == true) { message += " forcedcslogs=\"true\""; } else message += " forcedcslogs=\"false\"";
    if (forcedcclogs == true) { message += " forcedcclogs=\"true"; } else message += " forcedcclogs=\"false";
	/* cout << "MESSAGE : " << message << std::endl; */
return message;
}




std::string DiagBagXdaq::forgeErrorString(std::string msg,
                                        std::string text,
                                        int errorcode,
                                        std::string faultstate,
                                        std::string systemid,
                                        std::string subsystemid,
                                        std::string classname,
                                        unsigned long instance,
                                        unsigned long lid)
{
    std::string message;
    message += msg;
    message += "\"";
    message += " text=\"" + text + "\"";
    std::stringstream s1;
    s1 << errorcode;
    message += " errorcode=\"" + s1.str() + "\"";
    message += " faultstate=\"" + faultstate + "\"";
    message += " systemid=\"" + systemid + "\"";
    message += " subsystemid=\"" + subsystemid + "\"";
    message += " classname=\""+ classname +"\"";
    std::stringstream s2;
    s2 << instance;
    message += " instance=\""+ s2.str() +"\"";
    std::stringstream s3;
    s3 << lid;
    message += " lid=\""+ s3.str() +"\"";
    message += " lvl1name=\""+ lvl1Name_+"\"";
    std::stringstream s4;
    s4 << lvl1Instance_;
    message += " lvl1instance=\""+ s4.str() +"\"";
    if (emUsable_ == true) { message += " routetoem=\"true\""; } else message += " routetoem=\"false\"";
    message += " dbloglevel=\"" + dbLogLevel_ + "\"";
    message += " csloglevel=\"" + csLogLevel_ +"\"";
    message += " ccloglevel=\"" + ccLogLevel_ + "\"";  \
    message += " gloglevel=\"" + gLogLevel_ + "\"";
    if (forcedDbLogs_ == true) { message += " forceddblogs=\"true\""; } else message += " forceddblogs=\"false\""; \
    if (forcedCsLogs_ == true) { message += " forcedcslogs=\"true\""; } else message += " forcedcslogs=\"false\""; \
    if (forcedCcLogs_ == true) { message += " forcedcclogs=\"true"; } else message += " forcedcclogs=\"false"; \
    /* cout << "MESSAGE : " << message << std::endl; */
return message;
}


std::string DiagBagXdaq::forgeErrorString(std::string msg,
                                        std::string classname,
                                        unsigned long instance,
                                        unsigned long lid)
{
    std::string message;
    message += msg;
    message += "\"";
    message += " text=\"\"";
    message += " errorcode=\"\"";
    message += " faultstate=\"\"";
    message += " systemid=\"\"";
    message += " subsystemid=\"\"";
    message += " classname=\""+ classname + "\"";
    std::stringstream s2;
    s2 << instance;
    message += " instance=\""+ s2.str() + "\"";
    std::stringstream s3;
    s3 << lid;
    message += " lid=\""+ s3.str() +"\"";
    message += " lvl1name=\""+ lvl1Name_+ "\"";
    std::stringstream s4;
    s4 << lvl1Instance_;
    message += " lvl1instance=\""+ s4.str() +"\"";
    if (emUsable_ == true) { message += " routetoem=\"true\""; } else message += " routetoem=\"false\"";
    message += " dbloglevel=\"" + dbLogLevel_ + "\"";
    message += " csloglevel=\"" + csLogLevel_ +"\"";
    message += " ccloglevel=\"" + ccLogLevel_ + "\"";  \
    message += " gloglevel=\"" + gLogLevel_ + "\"";
    if (forcedDbLogs_ == true) { message += " forceddblogs=\"true\""; } else message += " forceddblogs=\"false\""; \
    if (forcedCsLogs_ == true) { message += " forcedcslogs=\"true\""; } else message += " forcedcslogs=\"false\""; \
    if (forcedCcLogs_ == true) { message += " forcedcclogs=\"true"; } else message += " forcedcclogs=\"false"; \
return message;
}




int DiagBagXdaq::configureCallback( xgi::Input * in,
                                    xgi::Output * out,
                                    std::string classname,
                                    std::string URL,
                                    std::string URN)
{
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title(classname) << std::endl;

    xgi::Utils::getPageHeader(out, 
                            classname, 
                            URL,
                            URN,
                            "/daq/xgi/images/Application.jpg" );

    std::string resumeUrl = "/";
    resumeUrl += URN;
    resumeUrl += "/Default";

    /* Apply parameters */
    *out << cgicc::form().set("method","post")
    .set("action", resumeUrl)
    .set("enctype","multipart/form-data") << std::endl;

    *out << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Resume / Back to Default page");
    *out << cgicc::p() << std::endl;

    /* End of the form */
    *out << cgicc::form() << std::endl;

    std::string applyUrl = "/";
    applyUrl += URN;
    applyUrl += "/applyConfigureDiagSystem";

    /* Apply parameters */
    *out << cgicc::form().set("method","post")
    .set("action", applyUrl)
    .set("enctype","multipart/form-data") << std::endl;

    /* Apply Changes */
    *out << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply Settings");
    *out << cgicc::p() << std::endl;

    /* draw a separation line */
    *out << "<hr style=\"width: 100%; height: 2px;\">";

    /* Set General Logging level for this module */
    *out << "<span style=\"font-weight: bold;\">";
    *out << cgicc::label("Process Appender Settings") << "</span><br>" << std::endl;

    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Set the general log level for this process to :") << std::endl;
    *out << "&nbsp;<select name=\"gloglevel\">";
    if (gLogLevel_ != "TRACE") *out << "<option>TRACE</option>"; else *out << "<option selected>TRACE</option>";
    if (gLogLevel_ != "DEBUG") *out << "<option>DEBUG</option>"; else *out << "<option selected>DEBUG</option>";
    if (gLogLevel_ != "INFO") *out << "<option>INFO</option>"; else *out << "<option selected>INFO</option>";
    if (gLogLevel_ != "WARN") *out << "<option>WARN</option>"; else *out << "<option selected>WARN</option>";
    if (gLogLevel_ != "ERROR") *out << "<option>ERROR</option>"; else *out << "<option selected>ERROR</option>";
    if (gLogLevel_ != "FATAL") *out << "<option>FATAL</option>"; else *out << "<option selected>FATAL</option>";
    if (gLogLevel_ != "OFF") *out << "<option>OFF</option>"; else *out << "<option selected>OFF</option>";
    *out << "</select>"<< std::endl;
    *out << cgicc::p() << std::endl;

    /* draw a separation line */
    *out << "<hr style=\"width: 100%; height: 2px;\">";

    /* Create a CheckBox : Use Error Dispatcher Y/N */
    if (useDiag_ == true)
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","useDiag").set("checked","true") << std::endl;
    }
    else
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","useDiag").set("unchecked","false") << std::endl;
    }
    *out << "<span style=\"font-weight: bold;\">";
    *out << cgicc::label("Use Error Dispatcher for this process") << "</span>" << std::endl;
    if (useDiag_ == true)
    {
        if (edUsable_==true)
            *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
        if ((edUsable_==false) && (diagNameStatus_ != "unchecked"))
            *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error in process configuration</span>" << std::endl;
    }
    *out << cgicc::p() << std::endl;

    /* display status error if needed */
    if ( (diagNameStatus_ != "checked") && (diagNameStatus_ != "unchecked") )
        *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  diagNameStatus_ << "</span>" << std::endl;
    *out << cgicc::p() << std::endl;
    if ( (diagLIDStatus_ != "checked") && (diagLIDStatus_ != "unchecked") )
        *out << "<span style=\"color: rgb(204, 0, 0);\">" <<  diagLIDStatus_ << "</span>" << std::endl;
    if ( (diagIPStatus_ != "checked") && (diagIPStatus_ != "unchecked") )
        *out << "<span style=\"color: rgb(204, 0, 0);\">" <<  diagIPStatus_ << "</span>" << std::endl;

    /* Create an input : ED server name ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Dispatcher Server Name") << std::endl;
    if (diagNameStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (diagNameStatus_ != "checked") && (diagNameStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","diagName")
    .set("size","40")
    .set("value",diagName_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* Create an input : ED Instance ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Dispatcher Server Instance") << std::endl;
    if (diagNameStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (diagNameStatus_ != "checked") && (diagNameStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" << "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","diagInstance")
    .set("size","10")
    .set("value",diagInstance_) << std::endl;
    *out << cgicc::p() << std::endl;
		
    /* Create an input : ED LID ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Dispatcher Server LID") << std::endl;
    if (diagLIDStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (diagLIDStatus_ != "checked") && (diagLIDStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","diagLID")
    .set("size","10")
    .set("value",diagLID_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* Create an input : ED IP ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Dispatcher Server IP") << std::endl;
    if (diagIPStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (diagIPStatus_ != "checked") && (diagIPStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","diagIP")
    .set("size","20")
    .set("value",diagIP_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* Create an input : ED port ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Dispatcher Server port") << std::endl;
    if (diagIPStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (diagIPStatus_ != "checked") && (diagIPStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","diagPort")
    .set("size","10")
    .set("value",diagPort_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* draw a separation line */
    *out << "<hr style=\"width: 100%; height: 2px;\">";

    /* Create a CheckBox : Use Diag Level One Error Manager Y/N */
    if (useLvl1_ == true)
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","useLvl1").set("checked","true") << std::endl;
    }
    else
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","useLvl1").set("unchecked","false") << std::endl;
    }
    *out << "<span style=\"font-weight: bold;\">";
    *out << cgicc::label("Use a level 1 Error Manager for this process") << "</span>" << std::endl;
    if (useLvl1_ == true)
    {
        if ( (edUsable_==false) || (useDiag_ == false) )
        {
            *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error : You must set up an ErrorDispatcher first</span>" << std::endl;
        }
        else
        {		
            if (emUsable_==true)
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
            if ((emUsable_==false) && (lvl1NameStatus_ != "unchecked"))
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error in process configuration</span>" << std::endl;
        }
    }
    *out << cgicc::p() << std::endl;

    /* display status error if needed */
    if ( (lvl1NameStatus_ != "checked") && (lvl1NameStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  lvl1NameStatus_ << "</span><br>" << std::endl;
    if ( (lvl1LIDStatus_ != "checked") && (lvl1LIDStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  lvl1LIDStatus_ << "</span><br>" << std::endl;
    if ( (lvl1IPStatus_ != "checked") && (lvl1IPStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  lvl1IPStatus_ << "</span><br>" << std::endl;

    /* Create an input :Lvl1 server name ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Manager Server Name") << std::endl;
    if (lvl1NameStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (lvl1NameStatus_ != "checked") && (lvl1NameStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","lvl1Name")
    .set("size","40")
    .set("value",lvl1Name_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* Create an input : EM Instance ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Manager Server Instance") << std::endl;
    if (lvl1NameStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (lvl1NameStatus_ != "checked") && (lvl1NameStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","lvl1Instance")
    .set("size","10")
    .set("value",lvl1Instance_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* Create an input : EM LID ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Manager Server LID") << std::endl;
    if (lvl1LIDStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (lvl1LIDStatus_ != "checked") && (lvl1LIDStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","lvl1LID")
    .set("size","10")
    .set("value",lvl1LID_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* Create an input : EM IP ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Manager Server IP") << std::endl;
    if (lvl1IPStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (lvl1IPStatus_ != "checked") && (lvl1IPStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","lvl1IP")
    .set("size","20")
    .set("value",lvl1IP_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* Create an input : EM port ? */
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Error Manager Server port") << std::endl;
    if (lvl1IPStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (lvl1IPStatus_ != "checked") && (lvl1IPStatus_ != "unchecked") )
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::input().set("type","text")
    .set("name","lvl1Port")
    .set("size","10")
    .set("value",lvl1Port_) << std::endl;
    *out << cgicc::p() << std::endl;

    /* draw a separation line */
    *out << "<hr style=\"width: 100%; height: 2px;\">";

    /* Work on listbox now */
    *out << "<span style=\"font-weight: bold;\">";
    *out << cgicc::label("Errors Loggin local strategy") << "</span><br>" << std::endl;

    /* Create a CheckBox : force logs for DB ? */
    if (forcedDbLogs_ == true)
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","forceddblogs").set("checked","true") << std::endl;
    }
    else
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","forceddblogs").set("unchecked","false") << std::endl;
    }
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Force DB logging in ErrorDispatcher for this level and higher, whatever the ErrorDispatcher loglevel is.") << "</span>" << std::endl;
    *out << cgicc::p() << std::endl;
    *out << "&nbsp;<select name=\"dblogs\">";
    if (dbLogLevel_ != "TRACE") *out << "<option>TRACE</option>"; else *out << "<option selected>TRACE</option>";
    if (dbLogLevel_ != "DEBUG") *out << "<option>DEBUG</option>"; else *out << "<option selected>DEBUG</option>";
    if (dbLogLevel_ != "INFO") *out << "<option>INFO</option>"; else *out << "<option selected>INFO</option>";
    if (dbLogLevel_ != "WARN") *out << "<option>WARN</option>"; else *out << "<option selected>WARN</option>";
    if (dbLogLevel_ != "ERROR") *out << "<option>ERROR</option>"; else *out << "<option selected>ERROR</option>";
    if (dbLogLevel_ != "FATAL") *out << "<option>FATAL</option>"; else *out << "<option selected>FATAL</option>";
    if (dbLogLevel_ != "OFF") *out << "<option>OFF</option>"; else *out << "<option selected>OFF</option>";
    *out << "</select>"<< std::endl;
    *out << cgicc::p() << std::endl;

    *out << "<br>";
    /* Create a CheckBox : force logs for ChainSaw ? */
    if (forcedCsLogs_ == true)
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","forcedcslogs").set("checked","true") << std::endl;
    }
    else
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","forcedcslogs").set("unchecked","false") << std::endl;
    }
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Force Chainsaw logging for this level and higher, whatever the ErrorDispatcher loglevel is.") << "</span>" << std::endl;
    *out << cgicc::p() << std::endl;
    *out << "&nbsp;<select name=\"cslogs\">";
    if (csLogLevel_ != "TRACE") *out << "<option>TRACE</option>"; else *out << "<option selected>TRACE</option>";
    if (csLogLevel_ != "DEBUG") *out << "<option>DEBUG</option>"; else *out << "<option selected>DEBUG</option>";
    if (csLogLevel_ != "INFO") *out << "<option>INFO</option>"; else *out << "<option selected>INFO</option>";
    if (csLogLevel_ != "WARN") *out << "<option>WARN</option>"; else *out << "<option selected>WARN</option>";
    if (csLogLevel_ != "ERROR") *out << "<option>ERROR</option>"; else *out << "<option selected>ERROR</option>";
    if (csLogLevel_ != "FATAL") *out << "<option>FATAL</option>"; else *out << "<option selected>FATAL</option>";
    if (csLogLevel_ != "OFF") *out << "<option>OFF</option>"; else *out << "<option selected>OFF</option>";
    *out << "</select>"<< std::endl;
    *out << cgicc::p() << std::endl;
    *out << "<br>";

    /* Create a CheckBox : force logs for Console ? */
    if (forcedCcLogs_ == true)
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","forcedcclogs").set("checked","true") << std::endl;
    }
    else
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","forcedcclogs").set("unchecked","false") << std::endl;
    }
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Force Console logging for this level and higher, whatever the ErrorDispatcher loglevel is.") << "</span>" << std::endl;
    *out << cgicc::p() << std::endl;
    *out << "&nbsp;<select name=\"cclogs\">";
    if (ccLogLevel_ != "TRACE") *out << "<option>TRACE</option>"; else *out << "<option selected>TRACE</option>";
    if (ccLogLevel_ != "DEBUG") *out << "<option>DEBUG</option>"; else *out << "<option selected>DEBUG</option>";
    if (ccLogLevel_ != "INFO") *out << "<option>INFO</option>"; else *out << "<option selected>INFO</option>";
    if (ccLogLevel_ != "WARN") *out << "<option>WARN</option>"; else *out << "<option selected>WARN</option>";
    if (ccLogLevel_ != "ERROR") *out << "<option>ERROR</option>"; else *out << "<option selected>ERROR</option>";
    if (ccLogLevel_ != "FATAL") *out << "<option>FATAL</option>"; else *out << "<option selected>FATAL</option>";
    if (ccLogLevel_ != "OFF") *out << "<option>OFF</option>"; else *out << "<option selected>OFF</option>";
    if (ccLogLevel_ != "ERROR_DISPATCHER_LEVEL") *out << "<option>ERROR_DISPATCHER_LEVEL</option>"; else *out << "<option selected>ERROR_DISPATCHER_LEVEL</option>";
    *out << "</select>"<< std::endl;
    *out << cgicc::p() << std::endl;

    /* draw a separation line */
    *out << "<hr style=\"width: 100%; height: 2px;\">";

    /*  End of the form */
    *out << cgicc::form() << std::endl;	
return 0;
}



int DiagBagXdaq::getFormValues(xgi::Input * in,
                                xgi::Output * out)
{
//#define DEBUG_GET_FORM_VALUES
    /* Create a new Cgicc object containing all the CGI data */
    cgicc::Cgicc cgi(in);
    forcedDbLogs_ = cgi.queryCheckbox("forceddblogs");
    forcedCsLogs_ = cgi.queryCheckbox("forcedcslogs");
    forcedCcLogs_ = cgi.queryCheckbox("forcedcclogs");

    gLogLevel_ = cgi["gloglevel"]->getValue();
    dbLogLevel_ = cgi["dblogs"]->getValue();
    csLogLevel_ = cgi["cslogs"]->getValue();
    ccLogLevel_ = cgi["cclogs"]->getValue();

    /* Query the use error dispatcher checkbox */
    useDiag_ = cgi.queryCheckbox("useDiag");

    /* Query ED name */
    diagName_ = cgi["diagName"]->getValue() ;
    /* Query ED instance */
    diagInstance_ = cgi["diagInstance"]->getValue() ;
    /* Query ED IP */
    diagIP_ = cgi["diagIP"]->getValue() ;
    /* Query ED port */
    diagPort_ = cgi["diagPort"]->getValue() ;
    /* Query ED lid */
    diagLID_ = cgi["diagLID"]->getValue() ;

    /* Query the use error manager checkbox */
    useLvl1_ = cgi.queryCheckbox("useLvl1");

    /* Query EM name */
    lvl1Name_ = cgi["lvl1Name"]->getValue() ;
    /* Query EM instance */
    lvl1Instance_ = cgi["lvl1Instance"]->getValue() ;
    /* Query EM IP */
    lvl1IP_ = cgi["lvl1IP"]->getValue() ;
    /* Query EM port */
    lvl1Port_ = cgi["lvl1Port"]->getValue() ;
    /* Query EM lid */
    lvl1LID_ = cgi["lvl1LID"]->getValue() ;

    #ifdef DEBUG_GET_FORM_VALUES
        std::cout << "useDiag_ : " << useDiag_ << std::endl;
        std::cout << "diagName_ : " << diagName_ << std::endl;
        std::cout << "diagInstance_ : " << diagInstance_ << std::endl;
        std::cout << "diagIP_ : " << diagIP_ << std::endl;
        std::cout << "diagPort_ : " << diagPort_ << std::endl;
        std::cout << "diagLID_ : " << diagLID_ << std::endl;
        std::cout << "useLvl1_ : " << useLvl1_ << std::endl;
        std::cout << "lvl1Name_ : " << lvl1Name_ << std::endl;
        std::cout << "lvl1Instance_ : " << lvl1Instance_ << std::endl;
        std::cout << "lvl1IP_ : " << lvl1IP_ << std::endl;
        std::cout << "lvl1Port_ : " << lvl1Port_ << std::endl;
        std::cout << "lvl1LID_ : " << lvl1LID_ << std::endl;
    #endif
return 0;
}


bool DiagBagXdaq::checkUseDiag()
{
//#define DEBUG_CHECK_USE_DIAG
    if (useDiag_ == false)
    {
        #ifdef DEBUG_CHECK_USE_DIAG
            std::cout << "in DiagBagXdaq::checkUseDiag() : Usage of Diag NOT requested." << std::endl;
        #endif
        edUsable_=false;
        diagNameStatus_="unchecked";
        diagLIDStatus_="unchecked";
        diagIPStatus_="unchecked";
        return false;
    }
    else 
    {
        #ifdef DEBUG_CHECK_USE_DIAG
            std::cout << "in DiagBagXdaq::checkUseDiag() : Usage of Diag requested." << std::endl;
        #endif
        return true;
    }
}




bool DiagBagXdaq::setUseDiagOnName(bool usediag)
{
//#define DEBUG_SET_USE_DIAG_ON_NAME
    if ( usediag == true)
    {
        #ifdef DEBUG_SET_USE_DIAG_ON_NAME
            std::cout << "in DiagBagXdaq::setUseDiagOnName() : name/instance detected" << std::endl;
        #endif
        edUsable_ = true;
        diagNameStatus_ = "checked";
        return true;
    }
    else
    {
        #ifdef DEBUG_SET_USE_DIAG_ON_NAME
            std::cout << "in DiagBagXdaq::setUseDiagOnName() : name/instance NOT detected" << std::endl;
        #endif
        edUsable_ = false;
        std::stringstream errMsg;
        errMsg << "ErrorInfo : process " << diagName_ << " with instance " << diagInstance_ << " does not exists" << std::endl;
        diagNameStatus_ = errMsg.str();
        diagLIDStatus_="unchecked";
        diagIPStatus_="unchecked";
        return false;
    }
}



bool DiagBagXdaq::setUseDiagOnLid(unsigned long proc_lid)
{
    if (proc_lid != (unsigned long)atol(diagLID_.c_str()))
    {
        std::stringstream errMsg;
        errMsg << "&nbsp;&nbsp;Error : process " << diagName_ << " with instance " << diagInstance_ << " has LID " << proc_lid << " and not " << diagLID_ << std::endl;
        diagLIDStatus_ = errMsg.str();
        diagIPStatus_="unchecked";
        edUsable_ = false;
        return false;
    }
    else
    {
        diagLIDStatus_ = "checked";
        return true;
    }
}



bool DiagBagXdaq::setUseDiagOnUrl(std::string l_url)
{
    std::string givenURL="http://" + diagIP_ + ":" + diagPort_;
    if (givenURL != l_url)
    {
        std::stringstream errMsg;				
        errMsg << "&nbsp;&nbsp;Error : process " << diagName_ << " with instance " << diagInstance_ << " has url " << l_url << " and not " << givenURL << std::endl;
        diagIPStatus_ = errMsg.str();
        edUsable_=false;
        return false;
    }
    else
    {
        diagIPStatus_="checked";
        return true;
    }
}





bool DiagBagXdaq::checkUseLvl1()
{
//#define DEBUG_CHECK_USE_LVL1
    /* cout << "in DiagBagXdaq::checkUseLvl1() : Usage of Lvl1 requested." << std::endl; */
    if ( (useLvl1_ == false) || (useDiag_ == false) || (edUsable_==false))
    {
        #ifdef DEBUG_CHECK_USE_LVL1
            std::cout << "in DiagBagXdaq::checkUseLvl1() : Usage of Lvl1 requested." << std::endl;
        #endif
        emUsable_=false;
        lvl1NameStatus_="unchecked";
        lvl1LIDStatus_="unchecked";
        lvl1IPStatus_="unchecked";
        return false;
    }
    else
    {
        #ifdef DEBUG_CHECK_USE_LVL1
            std::cout << "in DiagBagXdaq::checkUseLvl1() : Usage of Lvl1 NOT requested." << std::endl;
        #endif
        return true;
    }
}


bool DiagBagXdaq::setUseLvl1OnName(bool uselvl1)
{
//#define DEBUG_SET_USE_LVL1_ON_NAME
    if ( uselvl1 == true)
    {
        #ifdef DEBUG_SET_USE_LVL1_ON_NAME
            std::cout << "in DiagBagXdaq::setUseLvl1OnName() : name/instance detected" << std::endl;
        #endif
        emUsable_ = true;
        lvl1NameStatus_ = "checked";
        return true;
    }
    else
    {
        #ifdef DEBUG_SET_USE_LVL1_ON_NAME
            std::cout << "in DiagBagXdaq::setUseLvl1OnName() : name/instance NOT detected" << std::endl;
        #endif
        emUsable_ = false;
        std::stringstream errMsg;
        errMsg << "ErrorInfo : process " << lvl1Name_ << " with instance " << lvl1Instance_ << " does not exists" << std::endl;
        lvl1NameStatus_ = errMsg.str();
        lvl1LIDStatus_="unchecked";
        lvl1IPStatus_="unchecked";
        return false;
    }
}



bool DiagBagXdaq::setUseLvl1OnLid(unsigned long proc_lid)
{
    if (proc_lid != (unsigned long)atol(lvl1LID_.c_str()))
    {
        std::stringstream errMsg;
        errMsg << "&nbsp;&nbsp;Error : process " << lvl1Name_ << " with instance " << lvl1Instance_ << " has LID " << proc_lid << " and not " << lvl1LID_ << std::endl;
        lvl1LIDStatus_ = errMsg.str();
        lvl1IPStatus_="unchecked";
        emUsable_ = false;
        return false;
    }
    else
    {
        lvl1LIDStatus_ = "checked";
        return true;
    }
}



bool DiagBagXdaq::setUseLvl1OnUrl(std::string l_url)
{
    std::string givenURL="http://" + lvl1IP_ + ":" + lvl1Port_;
    if (givenURL != l_url)
    {
        std::stringstream errMsg;				
        errMsg << "&nbsp;&nbsp;Error : process " << lvl1Name_ << " with instance " << lvl1Instance_ << " has url " << l_url << " and not " << givenURL << std::endl;
        lvl1IPStatus_ = errMsg.str();
        emUsable_=false;
        return false;
    }
    else
    {
        lvl1IPStatus_="checked";
        return true;
    }
}



std::string DiagBagXdaq::getDiagName() {return diagName_;}

std::string DiagBagXdaq::getDiagInstance() {return diagInstance_;}

std::string DiagBagXdaq::getLvl1Name() {return lvl1Name_;}

std::string DiagBagXdaq::getLvl1Instance() {return lvl1Instance_;}

std::string DiagBagXdaq::getDiagIP() {return diagIP_;}

std::string DiagBagXdaq::getDiagPort() {return diagPort_;}

std::string DiagBagXdaq::getDiagLid() {return diagLID_;}

std::string DiagBagXdaq::getGLogLevel() {return gLogLevel_;}


long DiagBagXdaq::getProcessLogLevel()
{
    if (gLogLevel_ == "TRACE") return log4cplus::TRACE_LOG_LEVEL;
    if (gLogLevel_ == "DEBUG") return log4cplus::DEBUG_LOG_LEVEL;
    if (gLogLevel_ == "INFO") return log4cplus::INFO_LOG_LEVEL;
    if (gLogLevel_ == "WARN") return log4cplus::WARN_LOG_LEVEL;
    if (gLogLevel_ == "ERROR") return log4cplus::ERROR_LOG_LEVEL;
    if (gLogLevel_ == "FATAL") return log4cplus::FATAL_LOG_LEVEL;
    if (gLogLevel_ == "OFF") return log4cplus::OFF_LOG_LEVEL;
    if (gLogLevel_ == "ERROR_DISPATCHER_LEVEL") return log4cplus::TRACE_LOG_LEVEL;
return log4cplus::NOT_SET_LOG_LEVEL;
}



std::string DiagBagXdaq::getOldDiagIP() {return oldDiagIP_;}
void DiagBagXdaq::setOldDiagIP(std::string s) {oldDiagIP_=s;}

std::string DiagBagXdaq::getOldDiagPort() {return oldDiagPort_;}
void DiagBagXdaq::setOldDiagPort(std::string s) {oldDiagPort_=s;}

std::string DiagBagXdaq::getOldDiagName() {return oldDiagName_;}
void DiagBagXdaq::setOldDiagName(std::string s) {oldDiagName_=s;}

std::string DiagBagXdaq::getOldDiagInstance() {return oldDiagInstance_;}
void DiagBagXdaq::setOldDiagInstance(std::string s) {oldDiagInstance_=s;}

std::string DiagBagXdaq::getOldDiagLID() {return oldDiagLID_;}
void DiagBagXdaq::setOldDiagLID(std::string s) {oldDiagLID_=s;}

bool DiagBagXdaq::getSocketSoapExists() {return socketSoapExists_;}
void DiagBagXdaq::setSocketSoapExists(bool b) {socketSoapExists_=b;}

std::string DiagBagXdaq::getDiagLID() {return diagLID_;}
void DiagBagXdaq::setDiagLID(std::string s) {diagLID_=s;}


void DiagBagXdaq::zeroLvlLog(std::string msg, std::string lvl, log4cplus::Logger& logger)
{
    if (lvl == "TRACE") LOG4CPLUS_TRACE(logger, msg);
    if (lvl == "DEBUG") LOG4CPLUS_DEBUG(logger, msg);
    if (lvl == "INFO") LOG4CPLUS_INFO(logger, msg);
    if (lvl == "WARN") LOG4CPLUS_WARN(logger, msg);
    if (lvl == "ERROR") LOG4CPLUS_ERROR(logger, msg);
    if (lvl == "FATAL") LOG4CPLUS_FATAL(logger, msg);
}


void DiagBagXdaq::minLvlLog(std::string msg, std::string lvl, log4cplus::Logger& logger, std::string classname, unsigned long instance, unsigned long lid)
{
    if (lvl == "TRACE") LOG4CPLUS_TRACE(logger, this->forgeErrorString(msg, classname, instance, lid) );
    if (lvl == "DEBUG") LOG4CPLUS_DEBUG(logger, this->forgeErrorString(msg, classname, instance, lid) );
    if (lvl == "INFO") LOG4CPLUS_INFO(logger, this->forgeErrorString(msg, classname, instance, lid) );
    if (lvl == "WARN") LOG4CPLUS_WARN(logger, this->forgeErrorString(msg, classname, instance, lid) );
    if (lvl == "ERROR") LOG4CPLUS_ERROR(logger, this->forgeErrorString(msg, classname, instance, lid) );
    if (lvl == "FATAL") LOG4CPLUS_FATAL(logger, this->forgeErrorString(msg, classname, instance, lid) );
}


void DiagBagXdaq::regularLvlLog(std::string msg, std::string lvl, log4cplus::Logger& logger, std::string classname, unsigned long instance, unsigned long lid, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid)
{
    if (lvl == "TRACE") LOG4CPLUS_TRACE(logger, this->forgeErrorString(msg, text, errorcode, faultstate, systemid, subsystemid, classname, instance, lid) );
    if (lvl == "DEBUG") LOG4CPLUS_DEBUG(logger, this->forgeErrorString(msg, text, errorcode, faultstate, systemid, subsystemid, classname, instance, lid) );
    if (lvl == "INFO") LOG4CPLUS_INFO(logger, this->forgeErrorString(msg, text, errorcode, faultstate, systemid, subsystemid, classname, instance, lid) );
    if (lvl == "WARN") LOG4CPLUS_WARN(logger, this->forgeErrorString(msg, text, errorcode, faultstate, systemid, subsystemid, classname, instance, lid) );
    if (lvl == "ERROR") LOG4CPLUS_ERROR(logger, this->forgeErrorString(msg, text, errorcode, faultstate, systemid, subsystemid, classname, instance, lid) );
    if (lvl == "FATAL") LOG4CPLUS_FATAL(logger, this->forgeErrorString(msg, text, errorcode, faultstate, systemid, subsystemid, classname, instance, lid) );
}


void DiagBagXdaq::reportError(std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid)
{
//define DEBUG_REPORT_ERROR_LONG
    #ifdef DEBUG_REPORT_ERROR_LONG
        std::cout << "entering : DiagBagXdaq::reportError - long level" << std::endl;
        if (useDiag_ == true) std::cout << "USEDIAG is TRUE" << std::endl;
        if (useDiag_ == false) std::cout << "USEDIAG is FALSE" << std::endl;
    #endif
    if (isConsoleDumpEnabled()==true) DiagBag::consoleLvlLog(msg, lvl);
    log4cplus::Logger & logger = ((log4cplus::Logger &)(*callingAppLogger_));
    std::string classname = callingAppClassName_;
    unsigned long instance = callingAppInstance_;
    unsigned long lid = callingAppLID_;
    if (useDiag_ == false)
    {
        zeroLvlLog(msg, lvl, logger);
        #ifdef DEBUG_REPORT_ERROR_LONG
            std::cout << "exiting : DiagBagXdaq::reportError  - long level after zerolevel call" << std::endl;
        #endif
    }
    else
    {
        regularLvlLog(msg, lvl, logger, classname, instance, lid, text, errorcode, faultstate, systemid, subsystemid);
        #ifdef DEBUG_REPORT_ERROR_LONG
            std::cout << "exiting : DiagBagXdaq::reportError  - long level after minLvlLog call" << std::endl;
        #endif
    }
}



void DiagBagXdaq::reportError(std::string msg, std::string lvl)
{
//define DEBUG_REPORT_ERROR_SHORT
    #ifdef DEBUG_REPORT_ERROR_SHORT
        std::cout << "entering : DiagBagXdaq::reportError - short level" << std::endl;
        if (useDiag_ == true) std::cout << "USEDIAG is TRUE" << std::endl;
        if (useDiag_ == false) std::cout << "USEDIAG is FALSE" << std::endl;
    #endif
    if (isConsoleDumpEnabled()==true) DiagBag::consoleLvlLog(msg, lvl);
    log4cplus::Logger & logger = ((log4cplus::Logger &)(*callingAppLogger_));
    std::string classname = callingAppClassName_;
    unsigned long instance = callingAppInstance_;
    unsigned long lid = callingAppLID_;
    if (useDiag_ == false)
    {
        zeroLvlLog(msg, lvl, logger);
        #ifdef DEBUG_REPORT_ERROR_SHORT
            std::cout << "exiting : DiagBagXdaq::reportError  - short level after zerolevel call" << std::endl;
        #endif
    }
    else
    {
        minLvlLog(msg, lvl, logger, classname, instance, lid);
        #ifdef DEBUG_REPORT_ERROR_SHORT
            std::cout << "exiting : DiagBagXdaq::reportError  - short level after minLvlLog call" << std::endl;
        #endif
    }
}


void DiagBagXdaq::setUseDiag(bool diagStatus) {useDiag_ = diagStatus;}

void DiagBagXdaq::setUseEd(bool diagStatus) {edUsable_ = diagStatus;}


bool DiagBagXdaq::getUseDiag(void) {return(useDiag_);}

bool DiagBagXdaq::getUseEd(void) {return(edUsable_);}



