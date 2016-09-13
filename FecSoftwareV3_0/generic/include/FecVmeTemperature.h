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

  Copyright Feb 2005, Evgueni Vlassov ITEP, Moscow
*/
#ifndef FECVMETEMPERATURE_H
#define FECVMETEMPERATURE_H

//System includes

// FecSoftware includes
#include "tscTypes.h"
#include "FecExceptionHandler.h"

//HAL includes
#include "hal/VME64xDevice.hh"

#define  item1 "DALLAS_TEMP1"
#define  item2 "DALLAS_TEMP2"

class FecVmeTemperature {

 private:

  /** vme board on which we will work, initialized in/by constructor  */
  HAL::VME64xDevice *vmeBoard_; 
  /** FEC slot */
  unsigned int fecSlot_ ;
  /** */
  int TEMPS[2];
  /** */
  int reset(char *Item);
  /** */
  void convertT();
  /** */
  void readTemps();
  /** */
  int inp(char * Item, int Offset = 0);
  /** */
  void outp(char * Item, int Offset , tscType32 Value);
  /** */
  void outp2(int Offset , tscType32 Value);
  /** */
  void waitforInterrupt();
 
 public:

  /******************************************************
			CONSTRUCTOR - DESTRUCTOR
  ******************************************************/

  //Constructor
  FecVmeTemperature (HAL::VME64xDevice *vmedevice, unsigned int fecSlot = 0 ) throw ( FecExceptionHandler ) ;
	
  //Destructor
  virtual ~FecVmeTemperature () throw ( FecExceptionHandler );

  /** return the FEC slot
   */
  inline unsigned int getFecSlot ( ) { return fecSlot_ ; } 
	
  /******************************************************
		Temperature  ACCESS - NATIVE 32 BITS FORMAT
  ******************************************************/
  int getTemp(int tempSlot) throw ( FecExceptionHandler );
       
} ;

#endif
