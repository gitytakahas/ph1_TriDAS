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
#ifndef PIARESETACCESS_H
#define PIARESETACCESS_H

#include "deviceType.h"

#include "FecAccess.h"
#include "PiaChannelAccess.h"
#include "piaResetDescription.h"

/**
 * \class PiaResetAccess
 * This class defined a PIA access for the reset specific to the tracker
 * \author Frederic Drouhin
 * \date Febrary 2003
 * \brief This class define all the hardware access for the PIA reset
 */
class PiaResetAccess: public PiaChannelAccess {

 private:
  /** PIA reset description values
   */
  piaResetDescription *piaResetValues_ ;

  /** Initial value to be set
   */
  tscType8 initialValue_ ;

 public:

  /** \brief Call the PiaChannelAccess constructor, in order to build
   * an access to a PIA channel
   */
  PiaResetAccess ( FecAccess *fec, 
		   tscType16 fecSlot,
		   tscType16 ringSlot,
		   tscType16 ccuAddress,
		   tscType16 channelId,
		   tscType8 initialValue) ;

  /** \brief Call the PiaChannelAccess constructor, in order to build
   * an access to a PIA channel
   */
  PiaResetAccess ( FecAccess *fec, 
		   keyType key,
		   tscType8 initialValue) ;

  /** \brief Remove the access
   */
  ~PiaResetAccess () ;

  /** \brief set the attribut piaResetValues_
   */
  void setDescription ( piaResetDescription piaValues ) ;

  /** \brief Set all values for the PIA reset
   */
  void setValues ( piaResetDescription piaValues ) ;

  /** \brief Set the value in the specified register
   */
  piaResetDescription *getValues ( ) ;

  /** \brief set the delay active reset
   */
  void setPiaReset ( tscType8 mask, 
                     unsigned long delayActiveReset,
                     unsigned long intervalDelayReset ) 
    throw (FecExceptionHandler) ;

  /** \brief get the value from the resistor register
   */
  tscType8 getMask ( ) ;

  /** \brief send the frames in multiple access mode
   */
  static unsigned int resetPiaModulesMultipleFrame ( FecAccess &fecAccess, piaResetVector vPiaReset, 
						     unsigned int delayActiveReset, unsigned int intervalDelayReset,
						     tscType8 initialValue,
						     std::list<FecExceptionHandler *> &errorList ) 
    throw (FecExceptionHandler) ;

};

#endif
