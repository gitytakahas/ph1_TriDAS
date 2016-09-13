/*
  This file is part of FEC Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE, IReS/IN2P3, Frederic Drouhin, CERN/UHA, Guillaume Baulieu IPNL/IN2P3
*/

#include <unistd.h>
#include <sys/types.h>
#include "esMemBufOutputSource.h"

/** Default Constructor
 */
esMemBufOutputSource::esMemBufOutputSource (): MemBufOutputSource() {
}

/**Constructor for device parameter<BR>
 * Creates an XML buffer from a deviceVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write device information<BR>
 * Write an end tag<BR>
 * @param deviceParameters - device description
 * @exception FecExceptionHandler
 * @see esMemBufOutputSource::generateHeader()
 * @see esMemBufOutputSource::generateStartTag()
 * @see esMemBufOutputSource::generateDeviceTag(deviceVector *)
 * @see esMemBufOutputSource::generateEndTag()
 */
esMemBufOutputSource::esMemBufOutputSource (deviceVector deviceParameters, bool forDb) throw (FecExceptionHandler): MemBufOutputSource(deviceParameters, forDb) {
}

/**Constructor for esMbReset parameter<BR>
 * Creates an XML buffer from a esMbResetVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write mbReset information<BR>
 * Write an end tag<BR>
 * @param esMbResetParameters - esMbReset description
 * @exception FecExceptionHandler
 * @see esMemBufOutputSource::generateHeader()
 * @see esMemBufOutputSource::generateStartTag()
 * @see esMemBufOutputSource::generatePiaResetTag(esMbResetVector *)
 * @see esMemBufOutputSource::generateEndTag()
 */
esMemBufOutputSource::esMemBufOutputSource (esMbResetVector piaResetParameters, bool forDb) throw (FecExceptionHandler) {
  generateHeader();
  generateStartTag(COMMON_XML_SCHEME);
  generateEsMbResetTag(piaResetParameters, forDb);
  generateEndTag();
}
                         
/**Constructor for device parameter<BR>
 * Creates an XML buffer from a piaResetVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write piaReset information<BR>
 * Write device information<BR>
 * Write an end tag<BR>
 * @param deviceParameters - device description
 * @param piaResetParameters - piaReset description
 * @exception FecExceptionHandler
 * @see esMemBufOutputSource::generateHeader()
 * @see esMemBufOutputSource::generateStartTag()
 * @see esMemBufOutputSource::generateEsMbResetTag(esMbResetVector )
 * @see esMemBufOutputSource::generateDeviceTag(deviceVector )
 * @see esMemBufOutputSource::generateEndTag()
 */
esMemBufOutputSource::esMemBufOutputSource (deviceVector deviceParameters, esMbResetVector piaResetParameters, bool forDb) throw (FecExceptionHandler) {
  generateHeader();
  generateStartTag(COMMON_XML_SCHEME);
  generateEsMbResetTag(piaResetParameters);
  generateDeviceTag(deviceParameters);
  generateEndTag();
}

/**Delete a MemBufOuputSource object
 */
esMemBufOutputSource::~esMemBufOutputSource (){
}

/**Generates MbReset tag
 * @param esMbResetParameters - esMbReset descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the deviceType code is unknown
 * @see generateXMLMbReset(esMbResetDescription *, std::stringstream &);
 */
void esMemBufOutputSource::generateEsMbResetTag(esMbResetVector piaResetParameters, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;

  if (forDb) {
    memBufferMbReset_ << "<ROWSET>" ;
  }
  for (esMbResetVector::iterator it = piaResetParameters.begin() ; it != piaResetParameters.end() ; it ++) {
    esMbResetDescription *piaResetd = *it;
    switch (piaResetd->getDeviceType()) {
     case ESMBRESET:
       generateXMLEsMbReset((esMbResetDescription *)piaResetd, memBufferMbReset_);
       break;
    default:
      errorMsg << "MemBufOuputSource::MemBufOuputSource unknown deviceType code : " << (int)piaResetd->getDeviceType() << std::ends;
      RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR, errorMsg.str(), FATALERRORCODE) ;
    }
  }

  if (forDb) {
    memBufferMbReset_ << "</ROWSET>" ;
  } else {
    memBuffer_ << memBufferMbReset_.str();
  }
}

/**Get teh <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *esMemBufOutputSource::getEsMbResetOutputBuffer(){
	return (&memBufferMbReset_);
};
