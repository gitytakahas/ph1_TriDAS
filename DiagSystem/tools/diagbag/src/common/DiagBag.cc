/*
   FileName : 		DiagBag.cc

   Content : 		DiagBag module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		DiagSystem 4.0

   Date of last modification : 19/12/2006

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

#include <diagbag/DiagBag.h>

DiagBag::DiagBag(bool consoledumpenabled, std::string consoledumplevel)
{
    consoleDumpEnabled_ = consoledumpenabled;
    consoleDumpLevel_ = consoledumplevel;
}


DiagBag::DiagBag(void)
{
    consoleDumpEnabled_ = CONSOLE_DUMP_ENABLED; //Defined in clog.h
    consoleDumpLevel_ = DIAGTRACE;				//Defined in clog.h
}



DiagBag::~DiagBag()
{
}



bool DiagBag::isConsoleDumpEnabled()
{
    return consoleDumpEnabled_;
}



void DiagBag::setConsoleDumpEnabled(bool b)
{
    consoleDumpEnabled_ = b;
}



std::string DiagBag::getConsoleDumpLevel()
{
    return consoleDumpLevel_;
}



void DiagBag::setConsoleDumpLevel(std::string s)
{
    consoleDumpLevel_ = s;
}




void DiagBag::consoleLvlLog(std::string msg, std::string lvl)
{
    int console_level = INFOLEVEL;
    int msg_level = INFOLEVEL;

    if (lvl == DIAGTRACE) msg_level=TRACELEVEL;
    else if (lvl == DIAGDEBUG) msg_level=DEBUGLEVEL;
    else if (lvl == DIAGINFO) msg_level=INFOLEVEL;
    else if (lvl == DIAGWARN) msg_level=WARNLEVEL;
    else if (lvl == DIAGUSERINFO) msg_level=USERINFOLEVEL;
    else if (lvl == DIAGERROR) msg_level=ERRORLEVEL;
    else if (lvl == DIAGFATAL) msg_level=FATALLEVEL;

    if (consoleDumpLevel_ == DIAGTRACE) console_level=TRACELEVEL;
    else if (consoleDumpLevel_ == DIAGDEBUG) console_level=DEBUGLEVEL;
    else if (consoleDumpLevel_ == DIAGINFO) console_level=INFOLEVEL;
    else if (consoleDumpLevel_ == DIAGWARN) console_level=WARNLEVEL;
    else if (consoleDumpLevel_ == DIAGUSERINFO) console_level=USERINFOLEVEL;
    else if (consoleDumpLevel_ == DIAGERROR) console_level=ERRORLEVEL;
    else if (consoleDumpLevel_ == DIAGFATAL) console_level=FATALLEVEL;
    else if (consoleDumpLevel_ == DIAGOFF) console_level=OFFLEVEL;

    if (msg_level >= console_level) std::cout << msg << std::endl;
}



void DiagBag::reportError(std::string msg,
                        std::string lvl,
                        std::string text,
                        int errorcode,
                        std::string faultstate,
                        std::string systemid,
                        std::string subsystemid )
{
    if ((isConsoleDumpEnabled()==true) && (lvl != DIAGOFF)) consoleLvlLog(msg, lvl);
}



void DiagBag::reportError(std::string msg, std::string lvl)
{
    if ((isConsoleDumpEnabled()==true) && (lvl != DIAGOFF)) consoleLvlLog(msg, lvl);
}


