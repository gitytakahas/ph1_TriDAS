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
#ifndef PLLDESCRIPTION_H
#define PLLDESCRIPTION_H

#include "tscTypes.h"
#include "deviceDescription.h"
#include "pllDefinition.h"

/**
 * \class pllDescription
 * This class give a description of all the PLL registers:
 * <ul>
 * <li>clockPhase (delay fine)
 * <li>triggerDelay (delay coarse)
 * </ul>
 * \warning Note that this description never access the hardware, it is used to
 * keep the values to be set or set.
 * \warning The hardware path (FEC::Ring::CCU::Channel::Address) can be set also
 * \see pllDefinition.h that gives the definition of the version of the laserdriver
 *      and the offset of the different registers
 * \include "pllDefinition.h"
 * \author Frederic Drouhin
 * \date October 2005
 * \brief This class give a description of all the pll registers
 */
class pllDescription: public deviceDescription {

 private:
  tscType8 clockPhase_   ;
  tscType8 triggerDelay_ ;
  tscType8 pllDac_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum PllEnumType {DELAYFINE,DELAYCOARSE,PLLDAC} ;

  /** Parameter's names
   */
  static const char *PLLPARAMETERNAMES[PLLDAC+1] ; // = {"delayFine","delayCoarse", "pllDac"} ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a PLL with all the values specified:
   * \param accessKey - device key
   */
  pllDescription (keyType accessKey = 0) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a PLL with all the values specified:
   * \param accessKey - key access of a device
   * \param clockPhase - fine delay
   * \param triggerDelay - coarse delay
   */
  pllDescription ( keyType accessKey,
                   tscType8 clockPhase, 
                   tscType8 triggerDelay,
		   tscType8 pllDac = 255) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a PLL with all the values specified:
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - i2c channel
   * \param i2cAddress - i2c address
   * \param clockPhase - fine delay
   * \param triggerDelay - coarse delay
   */
  pllDescription ( tscType16 fecSlot,
                   tscType16 ringSlot,
                   tscType16 ccuAddress,
                   tscType16 i2cChannel,
                   tscType16 i2cAddress = 0x44,
                   tscType8 clockPhase = 0, 
                   tscType8 triggerDelay = 0,
		   tscType8 pllDac = 255 ) ;

  /** \brief Constructor in order to set all the parameters
   * Build a description of a PLL with all the values specified:
   * \param clockPhase - fine delay
   * \param triggerDelay - coarse delay
   */
  pllDescription ( tscType8 clockPhase, 
                   tscType8 triggerDelay,
		   tscType8 pllDac = 255 ) ;

  /* build a description of a PLL with a list of parameters name
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   */
  pllDescription ( parameterDescriptionNameType parameterNames ) ;

  /** \brief Method in order to set all the parameters
   * Build a description of a PLL with all the values specified:
   * \param clockPhase - fine delay
   * \param triggerDelay - coarse delay
   */
  void setDescriptionValues ( tscType8 clockPhase, 
                              tscType8 triggerDelay,
			      tscType8 pllDac = 255 ) ;

  /** \brief return the value of the param
   * Return the value of the initialisation
   * \return the value
   * \warning return always 1 (so the initialisation is done)
   */
  tscType8 getInit ( ) ;

  /** \brief return the value of the param
   * \deprecated use now getClockPhase()
   * Return the value of PLL register
   * \return the value
   */
  tscType8 getDelayFine ( ) ;

  /** \brief return the value of the param
   * \deprecated use now getClockPhase()
   * Return the value of PLL register
   * \return the value
   */
  tscType8 getClockPhase ( ) ;

  /** \brief return the value of the param
   * \deprecated use now getTriggerDelay
   * Return the value of PLL register
   * \return the value
   */
  tscType8 getDelayCoarse ( ) ;

  /** \brief return the value of the param
   * \deprecated use now getTriggerDelay
   * Return the value of PLL register
   * \return the value
   */
  tscType8 getTriggerDelay ( ) ;

  /** \brief return the value of the param
   */
  tscType8 getPllDac ( ) ;

  /** \brief set the value for register
   * \brief deprecated use now setClockPhase
   * Set the value for a PLL description
   * \param clockPhase - value to be set
   */
  void setDelayFine ( tscType8 clockPhase ) ;

  /** \brief set the value for register
   * Set the value for a PLL description
   * \param Delay_fine - value to be set
   */
  void setClockPhase ( tscType8 clockPhase ) ;

  /** \brief set the value for register
   * \deprecated use now setTriggerDelay
   * Set the value for a PLL description
   * \param triggerDelay - value to be set
   */
  void setDelayCoarse ( tscType8 triggerDelay ) ;

  /** \brief set the value for register
   * Set the value for a PLL description
   * \param Delay_coarse - value to be set
   */
  void setTriggerDelay ( tscType8 triggerDelay ) ;

  /** \brief set the value for register
   */
  void setPllDac ( tscType8 pllDac ) ;

  /** \brief In order to compare two PLL descriptions
   * \param pll - pll description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator!= ( pllDescription &pll ) ;

  /** \brief In order to compare two PLL descriptions
   * \param pll - pll description to be compared
   * \return bool - true if all values are equals, false if not
   */
  bool operator== ( pllDescription &pll ) ;

  /** Clone a device description
   * \return the device description cloned
   */
  pllDescription *clone ( ) ;

  /** \brief Display the APV values where the comparison is different
   * \param pll - uploaded values
   */
  void displayDifferences ( pllDescription &pll ) ;

  /** \brief Display the PLL values
   */
  void display ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;
} ;

#endif
