/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#ifndef ESMEMBUFOUTPUTSOURCE_H
#define ESMEMBUFOUTPUTSOURCE_H

// declaration of the type XMLByte *
#include <xercesc/util/PlatformUtils.hpp>

// declaraction of the types deviceVector and piaResetVector, and description for each device
#include "pllDescription.h"
#include "dcuDescription.h"
#include "CCUDescription.h"
#include "deltaDescription.h"
#include "kchipDescription.h"
#include "paceDescription.h"
#include "gohDescription.h"
#include "esMbResetDescription.h"
#include "deviceType.h"
#include "esDeviceType.h"
#include "TkRingDescription.h"

#include "MemBufOutputSource.h"

// declaration of the type stringstream
#include <sstream>

#define COMMON_XML_SCHEME "http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/schema.xsd"
#define DCUCONVERSION_XML_SCHEME "http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/conversionSchema.xsd"

/** \brief This class is implemented to generate a character buffer from a device vector.
 */
class esMemBufOutputSource: public MemBufOutputSource {

 private:
  /**String stream containing the XML buffer
   */
  std::stringstream memBufferMbReset_ ;

 public:


  /** \brief Default constructor
   */
  esMemBufOutputSource();

  /** \brief Create a MemBufOutputSource object from a deviceVector
   */
  esMemBufOutputSource(deviceVector , bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOutputSource object from a deviceVector
   */
  esMemBufOutputSource(deviceVector , esMbResetVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOutputSource object from a esMbResetVector
   */
  esMemBufOutputSource(esMbResetVector , bool forDb=false) throw (FecExceptionHandler);

  /** \brief Destructor
   */
  ~esMemBufOutputSource();

  /**Write on <I>memBuffer_</I> attribute esMbReset information
   */
  void generateEsMbResetTag(esMbResetVector piaResetParameters, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Return the generated esMbReset XML buffer
   */
  std::stringstream *getEsMbResetOutputBuffer();
};

#endif
