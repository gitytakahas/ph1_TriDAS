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

Copyright 2003 - 2004, Frederic DROUHIN, Laurent Gross - Universite de Haute-Alsace, Mulhouse-France, Institut de Recherche Subatomique de Strasbourg
*/
#ifndef FECFUNCTIONS_H
#define FECFUNCTIONS_H

//System includes

// FecSoftware includes
#include "FecVmeRingDevice.h"

class FecFunctions {

// private:
  
 public:


	/******************************************************
			CONSTRUCTOR - DESTRUCTOR
	******************************************************/

	//Constructor
	FecFunctions ( void ) throw ( FecExceptionHandler ) ;
	
	//Destructor
	virtual ~FecFunctions( void ) throw ( FecExceptionHandler );

static int FecFunctions::ffGetDeviceFrameStatus( tscType32 *frame, tscType16
*status ) throw ( FecExceptionHandler );

static int FecFunctions::ffGetFrameStatus( tscType32 *frame, tscType16 *status ) throw ( FecExceptionHandler );

static int FecFunctions::ffComputeFrame32Length( tscType32 frame_word, int *frame32_length ) throw ( FecExceptionHandler );

static int FecFunctions::ffReadFrame( tscType32 *frameToRead, FecVmeRingDevice * whereToRead ) throw ( FecExceptionHandler );

static int ffWriteFrame( tscType32 *frameToWrite, FecVmeRingDevice * whereToWrite ) throw ( FecExceptionHandler );

} ;

#endif
