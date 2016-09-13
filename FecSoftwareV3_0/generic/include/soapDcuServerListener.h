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

Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace,
Mulhouse-France
*/
#ifndef SOAPDCUSERVERLISTENER_H
#define SOAPDCUSERVERLISTENER_H

#include "xoap.h"

#include "soapFecCommand.h"

/**
 * \class soapDcuServerListener
 * Define the listener method for soap messages to the specific values. This
 * messages are used between a DCU server from data acquisition message and the FEC supervisor
 * process.
 * \brief Define a listener for soap messages
 */
class soapDcuServerListener: public virtual soapListener {

 public:

  /** \brief constructor that binds all the message value related the
   * following methods.
   */
  soapDcuServerListener ( ) {

    /* ------------------------------------------------------------- */
    /* For the soap message from the dcu server                      */
    /* it's the thread that is able to answer to any request from it */
    soapBindMethod(this, &soapDcuServerListener::soapGetDcuInformation, GETDCUINFORMATION);

  }

  // This message is used to retreive the values for all DCUs from this FecSupervisor
  virtual SOAPMessage soapGetDcuInformation ( SOAPMessage &message ) = 0 ;
};

#endif
