/*
   FileName : 		DiagBagMacros.h

   Content : 		a bunch of macros

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


/*******************************************************
********************************************************
********************************************************
MACROS cut & past to be inserted into target code
********************************************************
********************************************************
*******************************************************/

#define DIAG_XGI_BIND_CALLS() \
    xgi::bind(this,&getApplicationDescriptor()->getClassName()::configureDiagSystem, "configureDiagSystem"); \
    xgi::bind(this,&getApplicationDescriptor()->getClassName()::applyConfigureDiagSystem, "applyConfigureDiagSystem");


#define DIAG_SET_SOCKETSOAP_APPENDER(ip, port, lid, command, server) \
    SharedAppenderPtr append_1(new SOAPDiagAppender(LOG4CPLUS_TEXT(ip), port, lid, LOG4CPLUS_TEXT(command),LOG4CPLUS_TEXT(server))); \
    append_1->setName(LOG4CPLUS_TEXT("SocketSOAPDiagAppender")); \
    this->getApplicationLogger().setAdditivity(0); \
    this->getApplicationLogger().removeAllAppenders(); \
    this->getApplicationLogger().addAppender(append_1);



#define DIAG_SET_CONFIG_CALLBACK() \
    std::string urlDiag_ = "/"; \
    urlDiag_ += getApplicationDescriptor()->getURN(); \
    urlDiag_ += "/configureDiagSystem"; \
    std::string buttonName = "Configure Diagnostic System for process : " +	getApplicationDescriptor()->getClassName(); \
    *out << cgicc::form().set("method","post").set("action", urlDiag_).set("enctype","multipart/form-data") << std::endl; \
    *out << cgicc::input().set("type", "submit").set("name", "Apply").set("value", buttonName); \
    *out << cgicc::p() << std::endl; \
    *out << cgicc::form() << std::endl;


	
#define DIAG_CONFIGURE_CALLBACK() \
    configureDiagSystem(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) \
    { \
        diagService_->configureCallback(in, out, getApplicationDescriptor()->getClassName(), \
        getApplicationDescriptor()->getContextDescriptor()->getURL(), \
        getApplicationDescriptor()->getURN() ); \
    }


#define DIAG_APPLY_CALLBACK() \
    applyConfigureDiagSystem(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) \
    { \
        bool lcl_use_diag=true; \
        diagService_->getFormValues(in, out); \
        if (diagService_->checkUseDiag() == true) \
        { \
            xdaq::ApplicationDescriptor * d; \
            try \
            { \
                d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor( \
                diagService_->getDiagName(), atoi(diagService_->getDiagInstance().c_str())); \
                lcl_use_diag = diagService_->setUseDiagOnName(lcl_use_diag); \
                if (lcl_use_diag == true) \
                { \
                    lcl_use_diag = diagService_->setUseDiagOnLid(d->getLocalId()); \
                    if (lcl_use_diag == true) \
                    { \
                        lcl_use_diag = diagService_->setUseDiagOnUrl(d->getContextDescriptor()->getURL()); \
                    } \
                } \
            } \
            catch (xdaq::exception::Exception& e) \
            { \
                lcl_use_diag=false; \
                diagService_->setUseDiagOnName(lcl_use_diag); \
            } \
        } \
        else lcl_use_diag=false; \
        if (lcl_use_diag == true) \
        { \
            if ((diagService_->getDiagIP() != diagService_->getOldDiagIP()) || \
                (diagService_->getDiagPort() != diagService_->getOldDiagPort()) || \
                (diagService_->getDiagName() != diagService_->getOldDiagName()) || \
                (diagService_->getDiagInstance() != diagService_->getOldDiagInstance()) || \
                (diagService_->getDiagLID() != diagService_->getOldDiagLID()) ) \
            { \
                diagService_->setSocketSoapExists(false); \
                diagService_->setOldDiagIP(diagService_->getDiagIP()); \
                diagService_->setOldDiagPort(diagService_->getDiagPort()); \
                diagService_->setOldDiagName(diagService_->getDiagName()); \
                diagService_->setOldDiagInstance(diagService_->getDiagInstance()); \
                diagService_->setOldDiagLID(diagService_->getDiagLID()); \
            } \
            if (diagService_->getSocketSoapExists() == false) \
            { \
                /*std::cout << std::endl << "creating new socket soap appender for supervisor process" << std::endl << std::endl; */ \
                SharedAppenderPtr append_1(new SOAPDiagAppender(LOG4CPLUS_TEXT(diagService_->getDiagIP()), \
                atoi(diagService_->getDiagPort().c_str()), \
                atoi(diagService_->getDiagLid().c_str()), \
                LOG4CPLUS_TEXT("receivedLog"),LOG4CPLUS_TEXT(diagService_->getDiagName()))); \
                append_1->setName(LOG4CPLUS_TEXT("SocketSOAPDiagAppender")); \
                this->getApplicationLogger().setAdditivity(0); \
                this->getApplicationLogger().removeAllAppenders(); \
                this->getApplicationLogger().addAppender(append_1); \
                /*route to soap */ \
                diagService_->setSocketSoapExists(true); \
            } \
        } \
        else \
        { \
            if (diagService_->getSocketSoapExists() == true) \
            { \
                this->getApplicationLogger().removeAllAppenders(); \
                this->getApplicationLogger().setAdditivity(1); \
                diagService_->setSocketSoapExists(false); \
                /*route to console */ \
            } \
        } \
        if (diagService_->checkUseLvl1() == true) \
        { \
            xdaq::ApplicationDescriptor * l1d; \
            bool lcl_use_lvl1=true; \
            try \
            { \
                l1d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor( \
                diagService_->getLvl1Name(), atoi(diagService_->getLvl1Instance().c_str())); \
                lcl_use_lvl1 = diagService_->setUseLvl1OnName(lcl_use_lvl1); \
                if (lcl_use_lvl1 == true) \
                { \
                    lcl_use_lvl1 = diagService_->setUseLvl1OnLid(l1d->getLocalId()); \
                    if (lcl_use_lvl1 == true) \
                    { \
                        lcl_use_lvl1 = diagService_->setUseLvl1OnUrl(l1d->getContextDescriptor()->getURL()); \
                    } \
                } \
            } \
            catch (xdaq::exception::Exception& e) \
            { \
                lcl_use_lvl1=false; \
                diagService_->setUseLvl1OnName(lcl_use_lvl1); \
            } \
        } \
        this->getApplicationLogger().setLogLevel(diagService_->getProcessLogLevel()); \
        this->configureDiagSystem(in,out) ; \
    }


