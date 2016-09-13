/*
   FileName : 		DiagBag.h

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

#ifndef _DiagBag_h_
#define _DiagBag_h_

#include <iostream>

#include <diagbag/helpers/clog.h>
     
class DiagBag
{
	private:
		/* DATA */
        /* is output o console enabled? */
        bool consoleDumpEnabled_;
	
        /* which level (and higher) of log must be displayed on console? */
	    std::string consoleDumpLevel_;


    public:
        /* Ctors */
        DiagBag(bool consoledumpenabled, std::string consoledumplevel);
        DiagBag(void);

        /* Dtor */
        ~DiagBag();

        /* Methods */
        bool isConsoleDumpEnabled();
        void setConsoleDumpEnabled(bool b);
        std::string getConsoleDumpLevel();
        void setConsoleDumpLevel(std::string s);

        /* display logs on console */
        void consoleLvlLog(std::string msg, std::string lvl);

        /* errors logging methods */
        void reportError(std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid);
        void reportError(std::string msg, std::string lvl);


};

#endif // _DiagBag_h_

