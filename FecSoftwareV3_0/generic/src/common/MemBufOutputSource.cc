/*
  This file is part of FEC Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE, IReS/IN2P3, Frederic Drouhin, CERN/UHA, Guillaume Baulieu IPNL/IN2P3
*/

#include <unistd.h>
#include <sys/types.h>
#include "MemBufOutputSource.h"

//#define DISPLAYBUFFER

/** Default Constructor
 */
MemBufOutputSource::MemBufOutputSource () {
}

/**Constructor for device parameter<BR>
 * Creates an XML buffer from a deviceVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write device information<BR>
 * Write an end tag<BR>
 * @param deviceParameters - device description
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generateDeviceTag(deviceVector)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (deviceVector deviceParameters, bool forDb) throw (FecExceptionHandler) {
  generateHeader();
  generateStartTag(COMMON_XML_SCHEME);
  generateDeviceTag(deviceParameters, forDb);
  generateEndTag();

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
}

/**Constructor for device parameter<BR> 
 * Creates an XML buffer from a deviceVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write device information<BR>
 * Write an end tag<BR> 
 * @param connections - connection vector                                                                                    
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generateDeviceTag(deviceVector *) 
 * @see MemBufOutputSource::generateEndTag()                                                                                          */
MemBufOutputSource::MemBufOutputSource (ConnectionVector connections, bool forDb) throw (FecExceptionHandler) {
  generateHeader();
  if (forDb) memBuffer_ << "<ROWSET>" ;
  else generateStartTag(COMMON_XML_SCHEME);
  generateConnectionTag(connections, forDb);
  generateEndTag();

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
}


/**Constructor for device parameter<BR>
 * Creates an XML buffer from a piaResetVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write piaReset information<BR>
 * Write an end tag<BR>
 * @param piaResetParameters - piaReset description
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generatePiaResetTag(piaResetVector *)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (piaResetVector piaResetParameters, bool forDb) throw (FecExceptionHandler) {

  generateHeader();
  generateStartTag(COMMON_XML_SCHEME);
  generatePiaResetTag(piaResetParameters, forDb);
  generateEndTag();

#ifdef DISPLAYBUFFER
  std::cout << memBufferPiaReset_.str() << std::endl ;
#endif
}

/**Constructor for device parameter<BR>
 * Creates an XML buffer from a piaResetVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write piaReset information<BR>
 * Write an end tag<BR>
 * @param piaResetParameters - piaReset description
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generatePiaResetTag(piaResetVector)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (dcuConversionVector dcuConversionParameters, bool forDb) throw (FecExceptionHandler) {
  generateHeader();
  generateStartTag(DCUCONVERSION_XML_SCHEME);
  generateTkDcuConversionTag(dcuConversionParameters, forDb);
  generateEndTag();

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
}

/**Constructor for IdVsHostname parameter<BR>
 * Creates an XML buffer from a TkIdVsHostname vector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write piaReset information<BR>
 * Write an end tag<BR>
 * @param piaResetParameters - piaReset description
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generatePiaResetTag(piaResetVector)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource ( TkIdVsHostnameVector tkIdVsHostnameVector , bool forDb) throw (FecExceptionHandler) {

  generateHeader();
  if (forDb) memBuffer_ << "<ROWSET>" ;
  else generateStartTag(COMMON_XML_SCHEME);

  generateTkIdVsHostnameTag(tkIdVsHostnameVector, forDb);

  generateEndTag();

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
}

/**Constructor for device parameter<BR>
 * Creates an XML buffer from a tkDcuInfoVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write dcuInfo information<BR>
 * Write an end tag<BR>
 * @param tkDcuInfoVector - dcuInfo description
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generatePiaResetTag(tkDcuInfoVector)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (tkDcuInfoVector dcuInfoParameters, bool forDb) throw (FecExceptionHandler) {
  if(!forDb){
    generateHeader();
    generateStartTag(DCUCONVERSION_XML_SCHEME);
  }
  generateTkDcuInfoTag(dcuInfoParameters, forDb);
  if(!forDb){
    generateEndTag();
  }

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
}

/**Constructor for device parameter<BR>
 * Creates an XML buffer from a tkDcuPsuMapVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write dcuPsuMap information<BR>
 * Write an end tag<BR>
 * @param tkDcuPsuMapVector - dcuPsuMap description
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generatePiaResetTag(tkDcuPsuMapVector *)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (tkDcuPsuMapVector dcuPsuMapParameters, bool forDb) throw (FecExceptionHandler) {
  if(!forDb){
    generateHeader();
    generateStartTag(DCUCONVERSION_XML_SCHEME);
  }
  generateTkDcuPsuMapTag(dcuPsuMapParameters, forDb);
  if(!forDb){
    generateEndTag();
  }

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
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
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generatePiaResetTag(piaResetvector)
 * @see MemBufOutputSource::generateDeviceTag(deviceVector)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (deviceVector deviceParameters, piaResetVector piaResetParameters) throw (FecExceptionHandler) {
  generateHeader();
  generateStartTag(COMMON_XML_SCHEME);
  generatePiaResetTag(piaResetParameters);
  generateDeviceTag(deviceParameters);
  generateEndTag();

#ifdef DISPLAYBUFFER
  std::cout << memBufferApv25_.str() << std::endl ;
  std::cout << memBufferApvMux_.str() << std::endl ;
  std::cout << memBufferPll_.str() << std::endl ;
  std::cout << memBufferDcu_.str() << std::endl ;
  std::cout << memBufferLaserdriver_.str() << std::endl ;
#endif
}

/**Constructor for ccu parameter<BR>
 * Creates an XML buffer from a ccuVector<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write ccu information<BR>
 * Write an end tag<BR>
 * @param ccuParameters - ccu description
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generateCcuTag(ccuVector,bool)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (ccuVector ccuParameters, bool forDb) throw (FecExceptionHandler) {
  if(!forDb){
    generateHeader();
    generateStartTag(COMMON_XML_SCHEME);
  }
  generateCcuTag(ccuParameters, forDb);
  if(!forDb){
    generateEndTag();
  }

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
}

/**Constructor for ring parameter<BR>
 * Creates an XML buffer from a TkRingDescription<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write ring information (with or without the contained CCUs) <BR>
 * Write an end tag<BR>
 * @param ringParameters - TkRingDescription
 * @param withCcus - NOT TO BE USED for the database
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generateTkRingTag(TkRingDescription,bool)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (TkRingDescription ringParameters, bool forDb, bool withCcus) throw (FecExceptionHandler) {

  if (forDb && withCcus) {
    std::cerr << "ERROR in running mode> " << __PRETTY_FUNCTION__ << "with forDb==true and withCcus==true." << std::endl ;
    std::cerr << "ERROR in running mode> the upload of the rings in the database should be done in two steps, one for the rings and one for the CCUs, upload only the rings" << std::endl ;
    withCcus = false;
  }

  if(!forDb){
    generateHeader();
    generateStartTag(COMMON_XML_SCHEME);
  }
  generateTkRingTag(ringParameters, forDb);

  if (withCcus) {
    generateCcuTag(*ringParameters.getCcuVector(), forDb);
  }

  if(!forDb){
    generateEndTag();
  }

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
}


/**Constructor for a vector of ring parameter<BR>
 * Creates an XML buffer from a TkRingDescription<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write ring information (with or without the contained CCUs) <BR>
 * Write an end tag<BR>
 * @param ringParameters - TkRingDescription
 * @param withCcus - NOT TO BE USED for the database
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generateTkRingTag(TkRingDescription,bool)
 * @see MemBufOutputSource::generateEndTag()
 */
MemBufOutputSource::MemBufOutputSource (tkringVector rings, bool forDb, bool withCcus) throw (FecExceptionHandler) {

  if (forDb && withCcus) {
    std::cerr << "ERROR in running mode> " << __PRETTY_FUNCTION__ << "with forDb==true and withCcus==true." << std::endl ;
    std::cerr << "ERROR in running mode> the upload of the rings in the database should be done in two steps, one for the rings and one for the CCUs, upload only the rings" << std::endl ;
    withCcus = false;
  }

  if(!forDb){
    generateHeader();
    generateStartTag(COMMON_XML_SCHEME);
  }

  for (tkringVector::iterator it = rings.begin() ; it != rings.end() ; it ++) {
    generateTkRingTag(*(*it), forDb);
    if (withCcus) generateCcuTag(*((*it)->getCcuVector()), forDb);
  }

  if(!forDb){
    generateEndTag();
  }

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
  std::cout << rings.size() << " have been bufferized" << std::endl ;
#endif
}




/**
 * Constructor for commissioning analysis parameter<BR>
 * Creates an XML buffer from a vector of analysis descriptions<BR>
 * Write an header<BR>
 * Write a start tag<BR>
 * Write XML content from vector<BR>
 * Write an end tag<BR>
 * Version 1.0: first version with cmsswId, feChan ...<BR>
 * Version 1.1: version used for tkcc (jan 2008)<BR>
 * @param inVector - Vector of descriptions
 * @param analysisType - Commissioning analysis type
 * @param forDb - Indicates whether XML output has to be generated for database target or ASCII file
 * @exception FecExceptionHandler
 */
MemBufOutputSource::MemBufOutputSource( std::vector<CommissioningAnalysisDescription*> &inVector, CommissioningAnalysisDescription::commissioningType analysisType, bool forDb ) throw (FecExceptionHandler) {
	generateHeader();
	//std::string xmlVersion("1.1");
	if (forDb)
		memBuffer_ << "<ROWSET>" ;
	else 
		generateStartTag(COMMON_XML_SCHEME);

	generateAnalysisTag(inVector, analysisType, forDb);

	generateEndTag();

	#ifdef DISPLAYBUFFER
		std::cout << memBuffer_.str() << std::endl ;
	#endif
}




/**Delete a MemBufOuputSource object
 */
MemBufOutputSource::~MemBufOutputSource (){
}

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getOutputBuffer(){
  return(&memBuffer_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getPllOutputBuffer(){
  return(&memBufferPll_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getLaserdriverOutputBuffer(){
  return(&memBufferLaserdriver_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getApvFecOutputBuffer(){
  return(&memBufferApv25_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getApvMuxOutputBuffer(){
  return(&memBufferApvMux_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getPhilipsOutputBuffer(){
  return(&memBufferPhilips_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getDcuOutputBuffer(){
  return(&memBufferDcu_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getPiaResetOutputBuffer(){
  return(&memBufferPiaReset_);
};

#ifdef TOTEM
/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getVfatOutputBuffer(){
  return(&memBufferVfat_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getCChipOutputBuffer(){
  return(&memBufferCChip_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getTbbOutputBuffer(){
  return(&memBufferTbb_);
};
#endif // TOTEM

#ifdef PRESHOWER
/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getKchipOutputBuffer(){
  return(&memBufferKchip_);
};

/**Get teh <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getDeltaOutputBuffer(){
  return(&memBufferDelta_);
};

/**Get teh <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getPaceOutputBuffer(){
  return(&memBufferPace_);
};

/**Get teh <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::stringstream *MemBufOutputSource::getGohOutputBuffer(){
  return (&memBufferGoh_);
};

#endif

/**Generates a header '<?xml version=\"1.0\"?>'
 */
void MemBufOutputSource::generateHeader(){
  memBuffer_ << "<?xml version=\"1.0\"?>" << std::endl ;;
};

/**Generates a start tag '<ROWSET>'
 */
void MemBufOutputSource::generateStartTag(std::string file){
  //memBuffer_ << "<ROWSET xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation='http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/schema.xsd'>" << std::endl ;;
  memBuffer_ << "<ROWSET xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation='"<<file<<"'>" << std::endl ;
};

/**Generates a end tag '</ROWSET>'
 */
void MemBufOutputSource::generateEndTag(){
  memBuffer_ << "</ROWSET>";
  // cause a oracle.xml.parser.v2.XMLParseException: Expected 'EOF'. in the java code for upload in database
  //memBuffer_ << std::ends;
  memBuffer_.flush();
};

/**Generates device tag
 * @param deviceParameters - device descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the deviceType code is unknown
 * @see generateXMLPll(pllDescription *, std::stringstream &);
 * @see generateXMLLaserdriver(laserdriverDescription *, std::stringstream &);
 * @see generateXMLApv25(apvDescription *, std::stringstream &);
 * @see generateXMLApvMux(muxDescription *, std::stringstream &);
 * @see generateXMLPhilips(philipsDescription *, std::stringstream &);
 * @see generateXMLDcu(dcuDescription *, std::stringstream &);
 * @see generateXMLDelta(deltaDescription *, std::stringstream &);
 * @see generateXMLPace(paceDescription *, std::stringstream &);
 * @see generateXMLKchip(kchipDescription *, std::stringstream &);
 * @see generateXMLVfat(vfatDescription *, std::stringstream &);
 */
void MemBufOutputSource::generateDeviceTag(deviceVector deviceParameters, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;

#ifdef PRESHOWER
  std::stringstream memBufferDelta, memBufferKchip, memBufferPace, memBufferGoh ;
#endif // PRESHOWER

  if (forDb) {
    memBufferPll_ << "<ROWSET>" ;
    memBufferApv25_ << "<ROWSET>" ; 
    memBufferApvMux_ << "<ROWSET>" ; 
    memBufferDcu_ << "<ROWSET>" ; 
    memBufferLaserdriver_ << "<ROWSET>" ; 
    memBufferPhilips_ << "<ROWSET>" ;
#ifdef TOTEM
    memBufferVfat_ << "<ROWSET>" ;
    memBufferCChip_ << "<ROWSET>" ;
    memBufferTbb_ << "<ROWSET>" ;
#endif // TOTEM 
  }

  for (deviceVector::iterator it = deviceParameters.begin() ; it != deviceParameters.end() ; it ++) {
    deviceDescription *deviced = *it ;

    switch (deviced->getDeviceType()) {
    case APV25:
      generateXMLApv25((apvDescription *)deviced, memBufferApv25_, forDb);
      break;
    case APVMUX:
      generateXMLApvMux((muxDescription *)deviced, memBufferApvMux_, forDb);
      break;
    case DCU:
      generateXMLDcu((dcuDescription *)deviced, memBufferDcu_, forDb);
      break;
    case DOH:
    case LASERDRIVER:
      generateXMLLaserdriver((laserdriverDescription *)deviced, memBufferLaserdriver_, forDb);
      break;
    case PHILIPS:
      generateXMLPhilips((philipsDescription *)deviced, memBufferPhilips_, forDb);
      break;
    case PLL:
      generateXMLPll((pllDescription *)deviced, memBufferPll_, forDb);
      break ;
      
#ifdef PRESHOWER
    case DELTA:
      generateXMLDelta((deltaDescription *)deviced, memBufferDelta);
      break ;
    case PACE:
      generateXMLPace((paceDescription *)deviced, memBufferPace);
      break ;
    case KCHIP:
      generateXMLKchip((kchipDescription *)deviced, memBufferKchip);
      break ;
    case GOH:
      generateXMLGoh((gohDescription *)deviced, memBufferGoh);
      break ;
#endif // PRESHOWER

#ifdef TOTEM
    case VFAT:
      generateXMLVfat((vfatDescription *)deviced, memBufferVfat_, forDb);
      break;
    case CCHIP:
      generateXMLCChip((totemCChipDescription *)deviced, memBufferCChip_, forDb);
      break;
    case TBB:
      generateXMLTbb((totemBBDescription *)deviced, memBufferTbb_, forDb);
      break;
#endif // TOTEM

    default:
      errorMsg << "Unknown deviceType code : " << (int)deviced->getDeviceType() << std::ends;
      RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR, errorMsg.str(), FATALERRORCODE) ;
    }
  }

  if (forDb) {
    memBufferPll_ << "</ROWSET>" ;
    memBufferApv25_ << "</ROWSET>" ; 
    memBufferApvMux_ << "</ROWSET>" ; 
    memBufferDcu_ << "</ROWSET>" ; 
    memBufferLaserdriver_ << "</ROWSET>" ;
#ifdef TOTEM
    memBufferVfat_ << "</ROWSET>" ; 
    memBufferVfat_.flush();
    memBufferCChip_ << "</ROWSET>" ; 
    memBufferCChip_.flush();
    memBufferTbb_ << "</ROWSET>" ; 
    memBufferTbb_.flush();
#endif // TOTEM
    memBufferPll_.flush();
    memBufferApv25_.flush();
    memBufferApvMux_.flush();
    memBufferDcu_.flush();
    memBufferLaserdriver_.flush();
  } else {
    memBuffer_ << memBufferApv25_.str() << memBufferApvMux_.str() << memBufferDcu_.str() << memBufferLaserdriver_.str() << memBufferPll_.str() ;
#ifdef TOTEM
    memBuffer_<< memBufferVfat_.str();
    memBuffer_<< memBufferCChip_.str();
    memBuffer_<< memBufferTbb_.str();
#endif // TOTEM
  }

#ifdef PRESHOWER
  memBuffer_ << memBufferDelta.str() << memBufferPace.str() << memBufferKchip.str() 
	     << memBufferGoh.str() ;
#endif // PRESHOWER
}

                                                                                              
/**Generates connections tag
 * @param ringParameters - TkRingDescriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the ccuType code is unknown
 */
void MemBufOutputSource::generateConnectionTag(ConnectionVector connections, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;

  for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++) {
    if (forDb) {
      memBuffer_ << std::dec << "<RAWCONNECTION>"
		 << "<FEDID>" << (*it)->getFedId() << "</FEDID>"
		 << "<FEDCHANNEL>" << (*it)->getFedChannel() << "</FEDCHANNEL>"
		 << "<APVADDRESS>" << (*it)->getApvAddress() << "</APVADDRESS>"
		 << "<DCUHARDID>" << (*it)->getDcuHardId() << "</DCUHARDID>"
		 << "<ENABLED>" << ((*it)->getEnabled() ? 'T': 'F') << "</ENABLED>"
		 << "</RAWCONNECTION>" << std::endl ;
    }
    else {
      memBuffer_ << std::dec << "\t<FedChannelConnection" 
		 << " fedId = \"" << (*it)->getFedId() << "\""
		 << " fedChannel = \"" << (*it)->getFedChannel() << "\""
		 << " fecHardId = \"" << (*it)->getFecHardwareId() << "\""
		 << " crateSlot = \"" << (*it)->getFecCrateId() << "\""
		 << " fecSlot = \"" << (*it)->getFecSlot() << "\""
		 << " fecRing = \"" << (*it)->getRingSlot() << "\""
		 << " ccuAddress = \"" << (*it)->getCcuAddress() << "\""
		 << " apvAddress = \"" << (*it)->getApvAddress() << "\""
		 << " dcuHardId = \"" << (*it)->getDcuHardId() << "\""
		 << " enabled = \"" << ((*it)->getEnabled() ? "T" : "F") << "\""
		 << " />" << std::endl ;
    }
  }
}

/**Generates ccu tag
 * @param ccuParameters - ccu descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the ccuType code is unknown
 * @see generateXMLCcu(CCUDescription *, std::stringstream &);
 */
void MemBufOutputSource::generateCcuTag(ccuVector ccuParameters, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;
  std::stringstream memBufferCcu ;

  if (forDb) {
    memBufferCcu << "<ROWSET>" ;
  }
  
  for (ccuVector::iterator it = ccuParameters.begin() ; it != ccuParameters.end() ; it ++) {
    CCUDescription *ccutd = *it;
    generateXMLCcu((CCUDescription *)ccutd, memBufferCcu, forDb);
  }

  if (forDb) {
    memBufferCcu << "</ROWSET>" ;
    memBuffer_ << memBufferCcu.str();
    
  } else {
    memBuffer_ << memBufferCcu.str();
  }
}

/**Generates ring tag
 * @param ringParameters - TkRingDescriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the ccuType code is unknown
 * @see generateXMLTkRing(TkRingDescription *, std::stringstream &);
 */
void MemBufOutputSource::generateTkRingTag(TkRingDescription ringParameters, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;
  std::stringstream memBufferTkRing ;

  if (forDb) {
    memBufferTkRing << "<ROWSET>" ;
  }
  
  generateXMLTkRing(ringParameters, memBufferTkRing, forDb);

  if (forDb) {
    memBufferTkRing << "</ROWSET>" ;
    memBuffer_ << memBufferTkRing.str();
    
  } else {
    memBuffer_ << memBufferTkRing.str();
  }
}


/**Generates piaReset tag
 * @param piaResetParameters - piaReset descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the deviceType code is unknown
 * @see generateXMLPiaReset(piaResetDescription *, std::stringstream &);
 */
void MemBufOutputSource::generatePiaResetTag(piaResetVector piaResetParameters, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;

  if (forDb) {
    memBufferPiaReset_ << "<ROWSET>" ;
  }
  for (piaResetVector::iterator it = piaResetParameters.begin() ; it != piaResetParameters.end() ; it ++) {
    piaResetDescription *piaResetd = *it;
    switch (piaResetd->getDeviceType()) {
    case PIARESET:
      generateXMLPiaReset((piaResetDescription *)piaResetd, memBufferPiaReset_, forDb);
      break;
    default:
      errorMsg << "MemBufOuputSource::MemBufOuputSource unknown deviceType code : " << (int)piaResetd->getDeviceType() << std::ends;
      RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR, errorMsg.str(), FATALERRORCODE) ;
    }
  }

  if (forDb) {
    memBufferPiaReset_ << "</ROWSET>" ;
  } else {
    memBuffer_ << memBufferPiaReset_.str();
  }
}

/**Generates DCU conversion tag
 * @param piaResetParameters - piaReset descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the deviceType code is unknown
 * @see generateXMLPiaReset(piaResetDescription *, std::stringstream &);
 */
void MemBufOutputSource::generateTkDcuConversionTag(dcuConversionVector dcuConversionParameters, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;

  if (forDb) {
    memBuffer_ << "<ROWSET>" ;
  }

  for (dcuConversionVector::iterator it = dcuConversionParameters.begin() ; it != dcuConversionParameters.end() ; it ++) {
    TkDcuConversionFactors *dcuConversiond = *it;
    generateXMLTkDcuConversion(dcuConversiond, memBuffer_, forDb);
  }

  if (forDb) {
    memBuffer_ << "</ROWSET>" ;
  }
}

/**Generates IdVsHostname tag
 * @param piaResetParameters - piaReset descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the deviceType code is unknown
 * @see generateXMLPiaReset(piaResetDescription *, std::stringstream &);
 */
void MemBufOutputSource::generateTkIdVsHostnameTag( TkIdVsHostnameVector tkIdVsHostnameVector, bool forDb ) throw (FecExceptionHandler) {
  std::stringstream errorMsg;

  for (TkIdVsHostnameVector::iterator it = tkIdVsHostnameVector.begin() ; it != tkIdVsHostnameVector.end() ; it ++) {
    TkIdVsHostnameDescription *idvshostname = *it;
    generateXMLTkIdVsHostname(idvshostname, memBuffer_, forDb);
  }
}


/**
 * Generates commissioning analysis tag
 * @param inVector - Vector of analysis descriptions
 * @param analysisType - Analysis type
 * @param forDb - Indicates whether XML output has to be built for database or ASCII file target 
 */
void MemBufOutputSource::generateAnalysisTag( std::vector<CommissioningAnalysisDescription*> &inVector, CommissioningAnalysisDescription::commissioningType analysisType, bool forDb ) throw (FecExceptionHandler) {
	for (std::vector<CommissioningAnalysisDescription*>::iterator it=inVector.begin(); it!=inVector.end(); it++) {
		CommissioningAnalysisDescription *description = *it;
		generateXMLAnalysisTag(description, memBuffer_, analysisType, forDb);
	}
}

/**
 * Generates a commissioning analysis XML element
 * @param dcuConversion - dcuConversionFactors description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLAnalysisTag( CommissioningAnalysisDescription *description, std::stringstream &memBuffer, CommissioningAnalysisDescription::commissioningType analysisType, bool forDb ) {
// BEL S. - Auto generated with parseheader.tcl
	if ( ! forDb ) {
		if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_APVLATENCY ) { 
			ApvLatencyAnalysisDescription *myDesc = static_cast<ApvLatencyAnalysisDescription*> ( description ); 
			memBuffer << "\t<APVLATENCYANALYSIS" 
				<< " crate=\"" << myDesc->getCrate() << "\"" << " slot=\"" << myDesc->getSlot() << "\"" << " ring=\"" << myDesc->getRing() << "\"" << " ccuAdr=\"" << myDesc->getCcuAdr() << "\"" << " ccuChan=\"" << myDesc->getCcuChan() << "\"" << " i2cAddr=\"" << myDesc->getI2cAddr() << "\"" << " partition=\"" << myDesc->getPartition() << "\"" 
				<< " runNumber=\"" << myDesc->getRunNumber() << "\"" << " valid=\"" << ( myDesc->isValid() ? "1" : "0" ) << "\"" << " comments=\"" << myDesc->getSerializedComments() << "\"" << " detId=\"" << myDesc->getDetId() << "\"" << " dcuHardId=\"" << myDesc->getDcuHardId() << "\"" << " fedId=\""  << myDesc->getFedId() << "\"" << " feUnit=\"" << myDesc->getFeUnit() << "\"" << " fechan=\"" << myDesc->getFeChan() << "\"" << " fedApv=\"" << myDesc->getFedApv() << "\"" 
				<< " latency=\"" << myDesc->getLatency() << "\"" 
				<< " />"; 
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_CALIBRATION ) { 
			CalibrationAnalysisDescription *myDesc = static_cast<CalibrationAnalysisDescription*> ( description ); 
			memBuffer << "\t<CALIBRATIONANALYSIS" 
				<< " crate=\"" << myDesc->getCrate() << "\"" << " slot=\"" << myDesc->getSlot() << "\"" << " ring=\"" << myDesc->getRing() << "\"" << " ccuAdr=\"" << myDesc->getCcuAdr() << "\"" << " ccuChan=\"" << myDesc->getCcuChan() << "\"" << " i2cAddr=\"" << myDesc->getI2cAddr() << "\"" << " partition=\"" << myDesc->getPartition() << "\"" 
				<< " runNumber=\"" << myDesc->getRunNumber() << "\"" << " valid=\"" << ( myDesc->isValid() ? "1" : "0" ) << "\"" << " comments=\"" << myDesc->getSerializedComments() << "\"" << " detId=\"" << myDesc->getDetId() << "\"" << " dcuHardId=\"" << myDesc->getDcuHardId() << "\"" << " fedId=\""  << myDesc->getFedId() << "\"" << " feUnit=\"" << myDesc->getFeUnit() << "\"" << " fechan=\"" << myDesc->getFeChan() << "\"" << " fedApv=\"" << myDesc->getFedApv() << "\"" 
				<< " amplitude=\"" << myDesc->getAmplitude() << "\"" << " tail=\"" << myDesc->getTail() << "\"" << " riseTime=\"" << myDesc->getRiseTime() << "\"" << " timeConstant=\"" << myDesc->getTimeConstant() << "\"" << " smearing=\"" << myDesc->getSmearing() << "\"" << " chi2=\"" << myDesc->getChi2() << "\"" 
				<< " deconvMode=\"" << myDesc->getDeconvMode() << "\"" 
				<< " calChan=\"" << myDesc->getCalChan() << "\"" 
				<< " isha=\"" << myDesc->getIsha() << "\"" << " vfs=\"" << myDesc->getVfs() << "\"" 
				<< " />";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_FASTFEDCABLING ) { 
			FastFedCablingAnalysisDescription *myDesc = static_cast<FastFedCablingAnalysisDescription*> ( description ); 
			memBuffer << "\t<FASTFEDCABLINGANALYSIS" 
				<< " crate=\"" << myDesc->getCrate() << "\"" << " slot=\"" << myDesc->getSlot() << "\"" << " ring=\"" << myDesc->getRing() << "\"" << " ccuAdr=\"" << myDesc->getCcuAdr() << "\"" << " ccuChan=\"" << myDesc->getCcuChan() << "\"" << " i2cAddr=\"" << myDesc->getI2cAddr() << "\"" << " partition=\"" << myDesc->getPartition() << "\"" 
				<< " runNumber=\"" << myDesc->getRunNumber() << "\"" << " valid=\"" << ( myDesc->isValid() ? "1" : "0" ) << "\"" << " comments=\"" << myDesc->getSerializedComments() << "\"" << " detId=\"" << myDesc->getDetId() << "\"" << " dcuHardId=\"" << myDesc->getDcuHardId() << "\"" << " fedId=\""  << myDesc->getFedId() << "\"" << " feUnit=\"" << myDesc->getFeUnit() << "\"" << " fechan=\"" << myDesc->getFeChan() << "\"" << " fedApv=\"" << myDesc->getFedApv() << "\"" 
				<< " highLevel=\"" << myDesc->getHighLevel() << "\"" << " highRms=\"" << myDesc->getHighRms() << "\"" << " lowLevel=\"" << myDesc->getLowLevel() << "\"" << " lowRms=\"" << myDesc->getLowRms() << "\"" << " maxLl=\"" << myDesc->getMaxll() << "\"" << " minLl=\"" << myDesc->getMinll() 
				<< "\"" << " dcuId=\"" << myDesc->getDcuId() << "\"" << " lldCh=\"" << myDesc->getLldCh() << "\"" << " isDirty=\"" << ( myDesc->isDirty() ? "1" : "0" ) << "\"" << " dirtyThreshold=\"" << myDesc->getDirtyThreshold() << "\"" << " threshold=\"" << myDesc->getThreshold() << "\"" 
				<< " />";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_FINEDELAY ) { 
			FineDelayAnalysisDescription *myDesc = static_cast<FineDelayAnalysisDescription*> ( description ); 
			memBuffer << "\t<FINEDELAYANALYSIS" 
				<< " crate=\"" << myDesc->getCrate() << "\"" << " slot=\"" << myDesc->getSlot() << "\"" << " ring=\"" << myDesc->getRing() << "\"" << " ccuAdr=\"" << myDesc->getCcuAdr() << "\"" << " ccuChan=\"" << myDesc->getCcuChan() << "\"" << " i2cAddr=\"" << myDesc->getI2cAddr() << "\"" << " partition=\"" << myDesc->getPartition() << "\"" 
				<< " runNumber=\"" << myDesc->getRunNumber() << "\"" << " valid=\"" << ( myDesc->isValid() ? "1" : "0" ) << "\"" << " comments=\"" << myDesc->getSerializedComments() << "\"" << " detId=\"" << myDesc->getDetId() << "\"" << " dcuHardId=\"" << myDesc->getDcuHardId() << "\"" << " fedId=\""  << myDesc->getFedId() << "\"" << " feUnit=\"" << myDesc->getFeUnit() << "\"" << " fechan=\"" << myDesc->getFeChan() << "\"" << " fedApv=\"" << myDesc->getFedApv() << "\"" << " maximum=\"" << myDesc->getMaximum() << "\"" << " error=\"" << myDesc->getError() << "\"" << " />";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_OPTOSCAN ) { 
			OptoScanAnalysisDescription *myDesc = static_cast<OptoScanAnalysisDescription*> ( description ); 
			memBuffer << "\t<OPTOSCANANALYSIS" 
				<< " crate=\"" << myDesc->getCrate() << "\"" << " slot=\"" << myDesc->getSlot() << "\"" << " ring=\"" << myDesc->getRing() << "\"" << " ccuAdr=\"" << myDesc->getCcuAdr() << "\"" << " ccuChan=\"" << myDesc->getCcuChan() << "\"" << " i2cAddr=\"" << myDesc->getI2cAddr() << "\"" << " partition=\"" << myDesc->getPartition() << "\"" 
				<< " runNumber=\"" << myDesc->getRunNumber() << "\"" << " valid=\"" << ( myDesc->isValid() ? "1" : "0" ) << "\"" << " comments=\"" << myDesc->getSerializedComments() << "\"" << " detId=\"" << myDesc->getDetId() << "\"" << " dcuHardId=\"" << myDesc->getDcuHardId() << "\"" << " fedId=\""  << myDesc->getFedId() << "\"" << " feUnit=\"" << myDesc->getFeUnit() << "\"" << " fechan=\"" << myDesc->getFeChan() << "\"" << " fedApv=\"" << myDesc->getFedApv() << "\"" 
				<< " gain=\"" << myDesc->getGain() << "\"" 
				<< " bias0=\"" << myDesc->getBias0() << "\"" << " bias1=\"" << myDesc->getBias1() << "\"" << " bias2=\"" << myDesc->getBias2() << "\"" << " bias3=\"" << myDesc->getBias3() << "\"" 
				<< " measGain0=\"" << myDesc->getMeasGain0() << "\"" << " measGain1=\"" << myDesc->getMeasGain1() << "\"" << " measGain2=\"" << myDesc->getMeasGain2() << "\"" << " measGain3=\"" << myDesc->getMeasGain3() << "\"" 
				<< " zeroLight0=\"" << myDesc->getZeroLight0() << "\"" << " zeroLight1=\"" << myDesc->getZeroLight1() << "\"" << " zeroLight2=\"" << myDesc->getZeroLight2() << "\"" << " zeroLight3=\"" << myDesc->getZeroLight3() << "\"" 
				<< " linkNoise0=\"" << myDesc->getLinkNoise0() << "\"" << " linkNoise1=\"" << myDesc->getLinkNoise1() << "\"" << " linkNoise2=\"" << myDesc->getLinkNoise2() << "\"" << " linkNoise3=\"" << myDesc->getLinkNoise3() << "\"" 
				<< " liftOff0=\"" << myDesc->getLiftOff0() << "\"" << " liftOff1=\"" << myDesc->getLiftOff1() << "\"" << " liftOff2=\"" << myDesc->getLiftOff2() << "\"" << " liftOff3=\"" << myDesc->getLiftOff3() << "\"" 
				<< " threshold0=\"" << myDesc->getThreshold0() << "\"" << " threshold1=\"" << myDesc->getThreshold1() << "\"" << " threshold2=\"" << myDesc->getThreshold2() << "\"" << " threshold3=\"" << myDesc->getThreshold3() << "\"" 
				<< " tickHeight0=\"" << myDesc->getTickHeight0() << "\"" << " tickHeight1=\"" << myDesc->getTickHeight1() << "\"" << " tickHeight2=\"" << myDesc->getTickHeight2() << "\"" << " tickHeight3=\"" << myDesc->getTickHeight3() << "\"" 
				<< " baseLineSlop0=\"" << myDesc->getBaseLineSlop0() << "\"" << " baseLineSlop1=\"" << myDesc->getBaseLineSlop1() << "\"" << " baseLineSlop2=\"" << myDesc->getBaseLineSlop2() << "\"" << " baseLineSlop3=\"" << myDesc->getBaseLineSlop3() << "\"" 
				<< " />";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_PEDESTALS ) { 
			PedestalsAnalysisDescription *myDesc = static_cast<PedestalsAnalysisDescription*> ( description ); 
			memBuffer << "\t<PEDESTALSANALYSIS" 
				<< " crate=\"" << myDesc->getCrate() << "\"" << " slot=\"" << myDesc->getSlot() << "\"" << " ring=\"" << myDesc->getRing() << "\"" << " ccuAdr=\"" << myDesc->getCcuAdr() << "\"" << " ccuChan=\"" << myDesc->getCcuChan() << "\"" << " i2cAddr=\"" << myDesc->getI2cAddr() << "\"" << " partition=\"" << myDesc->getPartition() << "\"" 
				<< " runNumber=\"" << myDesc->getRunNumber() << "\"" << " valid=\"" << ( myDesc->isValid() ? "1" : "0" ) << "\"" << " comments=\"" << myDesc->getSerializedComments() << "\"" << " detId=\"" << myDesc->getDetId() << "\"" << " dcuHardId=\"" << myDesc->getDcuHardId() << "\"" << " fedId=\""  << myDesc->getFedId() << "\"" 
				<< " feUnit=\"" << myDesc->getFeUnit() << "\"" << " fechan=\"" << myDesc->getFeChan() << "\"" << " fedApv=\"" << myDesc->getFedApv() << "\"" 
				<< " dead=\"" << myDesc->getDead() << "\"" << " noisy=\"" << myDesc->getNoisy() << "\"" << " pedsMean=\"" << myDesc->getPedsMean() << "\"" << " pedsSpread=\"" << myDesc->getPedsSpread() << "\"" << " noiseMean=\"" << myDesc->getNoiseMean() << "\"" << " noiseSpread=\"" << myDesc->getNoiseSpread() << "\"" << " rawMean=\"" << myDesc->getRawMean() << "\"" 
				<< " rawSpread=\"" << myDesc->getRawSpread() << "\"" << " pedsMax=\"" << myDesc->getPedsMax() << "\"" << " pedsMin=\"" << myDesc->getPedsMin() << "\"" << " noiseMax=\"" << myDesc->getNoiseMax() << "\"" << " noiseMin=\"" << myDesc->getNoiseMin() << "\"" << " rawMax=\"" << myDesc->getRawMax() << "\"" << " rawMin=\"" << myDesc->getRawMin() << "\"" 
				<< " />";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_TIMING ) { 
			TimingAnalysisDescription *myDesc = static_cast<TimingAnalysisDescription*> ( description ); 
			memBuffer << "\t<TIMINGANALYSIS" << " crate=\"" << myDesc->getCrate() << "\"" << " slot=\"" << myDesc->getSlot() << "\"" << " ring=\"" << myDesc->getRing() << "\"" << " ccuAdr=\"" << myDesc->getCcuAdr() << "\"" << " ccuChan=\"" << myDesc->getCcuChan() << "\"" << " i2cAddr=\"" << myDesc->getI2cAddr() << "\"" << " partition=\"" << myDesc->getPartition() << "\"" 
				<< " runNumber=\"" << myDesc->getRunNumber() << "\"" << " valid=\"" << ( myDesc->isValid() ? "1" : "0" ) << "\"" << " comments=\"" << myDesc->getSerializedComments() << "\"" << " detId=\"" << myDesc->getDetId() << "\"" << " dcuHardId=\"" << myDesc->getDcuHardId() << "\"" << " fedId=\""  << myDesc->getFedId() << "\"" 
				<< " feUnit=\"" << myDesc->getFeUnit() << "\"" << " fechan=\"" << myDesc->getFeChan() << "\"" << " fedApv=\"" << myDesc->getFedApv() << "\"" 
				<< " timetmre=\"" << myDesc->getTimetmre() << "\"" << " refTime=\"" << myDesc->getRefTime() << "\"" << " delay=\"" << myDesc->getDelay() << "\"" << " height=\"" << myDesc->getHeight() << "\"" << " base=\"" << myDesc->getBase() << "\"" << " peak=\"" << myDesc->getPeak() << "\"" << " frameFindingThreshold=\"" 
				<< myDesc->getFrameFindingThreshold() << "\"" << " optimumSamplingPoint=\"" << myDesc->getOptimumSamplingPoint() << "\"" << " tickMarkHeightThreshold=\"" << myDesc->getTickMarkHeightThreshold() << "\"" << " kind=\"" << myDesc->getKind() << "\"" << " />";
		} else if ( 
			analysisType == CommissioningAnalysisDescription::T_ANALYSIS_VPSPSCAN ) { 
			VpspScanAnalysisDescription *myDesc = static_cast<VpspScanAnalysisDescription*> ( description ); 
			memBuffer << "\t<VPSPSCANANALYSIS" 
				<< " crate=\"" << myDesc->getCrate() << "\"" << " slot=\"" << myDesc->getSlot() << "\"" << " ring=\"" << myDesc->getRing() << "\"" << " ccuAdr=\"" << myDesc->getCcuAdr() << "\"" << " ccuChan=\"" << myDesc->getCcuChan() << "\"" << " i2cAddr=\"" << myDesc->getI2cAddr() << "\"" << " partition=\"" << myDesc->getPartition() << "\"" 
				<< " runNumber=\"" << myDesc->getRunNumber() << "\"" << " valid=\"" << ( myDesc->isValid() ? "1" : "0" ) << "\"" << " comments=\"" << myDesc->getSerializedComments() << "\"" << " detId=\"" << myDesc->getDetId() << "\"" << " dcuHardId=\"" << myDesc->getDcuHardId() << "\"" << " fedId=\""  << myDesc->getFedId() << "\"" 
				<< " feUnit=\"" << myDesc->getFeUnit() << "\"" << " fechan=\"" << myDesc->getFeChan() << "\"" << " fedApv=\"" << myDesc->getFedApv() << "\"" 
				<< " vpsp=\"" << myDesc->getVpsp() << "\"" << " adcLevel=\"" << myDesc->getAdcLevel() << "\"" << " fraction=\"" << myDesc->getFraction() << "\"" << " topEdge=\"" << myDesc->getTopEdge() << "\"" << " bottomEdge=\"" << myDesc->getBottomEdge() << "\"" << " topLevel=\"" << myDesc->getTopLevel() << "\"" 
				<< " bottomLevel=\"" << myDesc->getBottomLevel() << "\"" << " />"; 
		}
	} else {
		if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_APVLATENCY ) { 
			ApvLatencyAnalysisDescription *myDesc = static_cast<ApvLatencyAnalysisDescription*> ( description ); 
			memBuffer << "<RAWANALYSIS>" 
				<< "<CRATE>" << myDesc->getCrate() << "</CRATE>" << "<SLOT>" << myDesc->getSlot() << "</SLOT>" << "<RING>" << myDesc->getRing() << "</RING>" << "<CCUADR>" << myDesc->getCcuAdr() << "</CCUADR>" << "<CCUCHAN>" << myDesc->getCcuChan() << "</CCUCHAN>" << "<I2CADR>" << myDesc->getI2cAddr() << "</I2CADR>" << "<PARTITIONNAME>" << myDesc->getPartition() << "</PARTITIONNAME>" << "<RUNNUMBER>" << myDesc->getRunNumber() << "</RUNNUMBER>" << "<VALID>" << ( myDesc->isValid() ? "1" : "0" ) << "</VALID>" << "<COMMENTS>" << myDesc->getSerializedComments() << "</COMMENTS>" << "<FEDID>"  << myDesc->getFedId() << "</FEDID>" << "<FEUNIT>" << myDesc->getFeUnit() << "</FEUNIT>" << "<FECHAN>" << myDesc->getFeChan() << "</FECHAN>" << "<FEDAPV>" << myDesc->getFedApv() << "</FEDAPV>" << "<LATENCY>" << myDesc->getLatency() << "</LATENCY>" 
				<< "</RAWANALYSIS>";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_CALIBRATION ) {
			CalibrationAnalysisDescription *myDesc = static_cast<CalibrationAnalysisDescription*> ( description ); 
			memBuffer << "<RAWANALYSIS>" 
				<< "<CRATE>" << myDesc->getCrate() << "</CRATE>" << "<SLOT>" << myDesc->getSlot() << "</SLOT>" << "<RING>" << myDesc->getRing() << "</RING>" << "<CCUADR>" << myDesc->getCcuAdr() << "</CCUADR>" << "<CCUCHAN>" << myDesc->getCcuChan() << "</CCUCHAN>" << "<I2CADR>" << myDesc->getI2cAddr() << "</I2CADR>" << "<PARTITIONNAME>" << myDesc->getPartition() << "</PARTITIONNAME>" << "<RUNNUMBER>" << myDesc->getRunNumber() << "</RUNNUMBER>" << "<VALID>" << ( myDesc->isValid() ? "1" : "0" ) << "</VALID>" << "<COMMENTS>" << myDesc->getSerializedComments() << "</COMMENTS>" << "<FEDID>"  << myDesc->getFedId() << "</FEDID>" << "<FEUNIT>" << myDesc->getFeUnit() << "</FEUNIT>" << "<FECHAN>" << myDesc->getFeChan() << "</FECHAN>" << "<FEDAPV>" << myDesc->getFedApv() << "</FEDAPV>" << "<AMPLITUDE>" << myDesc->getAmplitude() << "</AMPLITUDE>" << "<TAIL>" << myDesc->getTail() << "</TAIL>" << "<RISETIME>" << myDesc->getRiseTime() << "</RISETIME>" << "<TIMECONSTANT>" << myDesc->getTimeConstant() 
				<< "</TIMECONSTANT>" << "<SMEARING>" << myDesc->getSmearing() << "</SMEARING>" << "<CHI2>" << myDesc->getChi2() << "</CHI2>" << "<DECONVMODE>" << myDesc->getDeconvMode() << "</DECONVMODE>" 
				<< "<ISHA>" << myDesc->getIsha() << "</ISHA>" 
				<< "<VFS>" << myDesc->getVfs() << "</VFS>" 
				<< "<CALCHAN>" << myDesc->getCalChan() << "</CALCHAN>" 
				<< "</RAWANALYSIS>";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_FASTFEDCABLING ) {
			FastFedCablingAnalysisDescription *myDesc = static_cast<FastFedCablingAnalysisDescription*> ( description );
			memBuffer << "<RAWANALYSIS>" 
				<< "<CRATE>" << myDesc->getCrate() << "</CRATE>" << "<SLOT>" << myDesc->getSlot() << "</SLOT>" << "<RING>" << myDesc->getRing() << "</RING>" << "<CCUADR>" << myDesc->getCcuAdr() << "</CCUADR>" << "<CCUCHAN>" << myDesc->getCcuChan() << "</CCUCHAN>" << "<I2CADR>" << myDesc->getI2cAddr() << "</I2CADR>" << "<PARTITIONNAME>" << myDesc->getPartition() << "</PARTITIONNAME>" << "<RUNNUMBER>" << myDesc->getRunNumber() << "</RUNNUMBER>" << "<VALID>" << ( myDesc->isValid() ? "1" : "0" ) << "</VALID>" << "<COMMENTS>" << myDesc->getSerializedComments() << "</COMMENTS>" << "<FEDID>"  << myDesc->getFedId() << "</FEDID>" << "<FEUNIT>" << myDesc->getFeUnit() << "</FEUNIT>" << "<FECHAN>" << myDesc->getFeChan() << "</FECHAN>" << "<FEDAPV>" << myDesc->getFedApv() << "</FEDAPV>" << "<HIGHLEVEL>" << myDesc->getHighLevel() << "</HIGHLEVEL>" << "<HIGHRMS>" << myDesc->getHighRms() << "</HIGHRMS>" 
				<< "<LOWLEVEL>" << myDesc->getLowLevel() << "</LOWLEVEL>" << "<LOWRMS>" << myDesc->getLowRms() << "</LOWRMS>" << "<MAXLL>" << myDesc->getMaxll() << "</MAXLL>" << "<MINLL>" << myDesc->getMinll() << "</MINLL>" << "<DCUID>" << myDesc->getDcuId() << "</DCUID>" << "<LLDCH>" << myDesc->getLldCh() << "</LLDCH>" 
				<< "<ISDIRTY>" << ( myDesc->isDirty() ? "1" : "0" ) << "</ISDIRTY>" << "<DIRTYTHRESHOLD>" << myDesc->getDirtyThreshold() << "</DIRTYTHRESHOLD>" << "<THRESHOLD>" << myDesc->getThreshold() << "</THRESHOLD>" 
				<< "</RAWANALYSIS>";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_FINEDELAY ) { 
			FineDelayAnalysisDescription *myDesc = static_cast<FineDelayAnalysisDescription*> ( description ); 
			memBuffer << "<RAWANALYSIS>" 
				<< "<CRATE>" << myDesc->getCrate() << "</CRATE>" << "<SLOT>" << myDesc->getSlot() << "</SLOT>" << "<RING>" << myDesc->getRing() << "</RING>" << "<CCUADR>" << myDesc->getCcuAdr() << "</CCUADR>" << "<CCUCHAN>" << myDesc->getCcuChan() << "</CCUCHAN>" << "<I2CADR>" << myDesc->getI2cAddr() << "</I2CADR>" << "<PARTITIONNAME>" << myDesc->getPartition() << "</PARTITIONNAME>" << "<RUNNUMBER>" << myDesc->getRunNumber() << "</RUNNUMBER>" << "<VALID>" << ( myDesc->isValid() ? "1" : "0" ) << "</VALID>" << "<COMMENTS>" << myDesc->getSerializedComments() << "</COMMENTS>" << "<FEDID>"  << myDesc->getFedId() << "</FEDID>" << "<FEUNIT>" << myDesc->getFeUnit() << "</FEUNIT>" << "<FECHAN>" << myDesc->getFeChan() << "</FECHAN>" << "<FEDAPV>" << myDesc->getFedApv() << "</FEDAPV>" << "<MAXIMUM>" << myDesc->getMaximum() << "</MAXIMUM>" << "<ERROR>" << myDesc->getError() << "</ERROR>" 
				<< "</RAWANALYSIS>";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_OPTOSCAN ) { 
			OptoScanAnalysisDescription *myDesc = static_cast<OptoScanAnalysisDescription*> ( description ); 
			memBuffer << "<RAWANALYSIS>" 
				<< "<CRATE>" << myDesc->getCrate() << "</CRATE>" << "<SLOT>" << myDesc->getSlot() << "</SLOT>" << "<RING>" << myDesc->getRing() << "</RING>" << "<CCUADR>" << myDesc->getCcuAdr() << "</CCUADR>" << "<CCUCHAN>" << myDesc->getCcuChan() << "</CCUCHAN>" << "<I2CADR>" << myDesc->getI2cAddr() << "</I2CADR>" << "<PARTITIONNAME>" << myDesc->getPartition() << "</PARTITIONNAME>" 
				<< "<RUNNUMBER>" << myDesc->getRunNumber() << "</RUNNUMBER>" << "<VALID>" << ( myDesc->isValid() ? "1" : "0" ) << "</VALID>" << "<COMMENTS>" << myDesc->getSerializedComments() << "</COMMENTS>" << "<FEDID>"  << myDesc->getFedId() << "</FEDID>" << "<FEUNIT>" << myDesc->getFeUnit() << "</FEUNIT>" << "<FECHAN>" << myDesc->getFeChan() << "</FECHAN>" << "<FEDAPV>" << myDesc->getFedApv() << "</FEDAPV>" 
				<< "<GAIN>" << myDesc->getGain() << "</GAIN>" 
				<< "<BIAS0>" << myDesc->getBias0() << "</BIAS0>" << "<BIAS1>" << myDesc->getBias1() << "</BIAS1>" << "<BIAS2>" << myDesc->getBias2() << "</BIAS2>" << "<BIAS3>" << myDesc->getBias3() << "</BIAS3>" 
				<< "<MEASGAIN0>" << myDesc->getMeasGain0() << "</MEASGAIN0>" << "<MEASGAIN1>" << myDesc->getMeasGain1() << "</MEASGAIN1>" << "<MEASGAIN2>" << myDesc->getMeasGain2() << "</MEASGAIN2>" << "<MEASGAIN3>" << myDesc->getMeasGain3() << "</MEASGAIN3>" 
				<< "<ZEROLIGHT0>" << myDesc->getZeroLight0() << "</ZEROLIGHT0>" << "<ZEROLIGHT1>" << myDesc->getZeroLight1() << "</ZEROLIGHT1>" << "<ZEROLIGHT2>" << myDesc->getZeroLight2() << "</ZEROLIGHT2>" << "<ZEROLIGHT3>" << myDesc->getZeroLight3() << "</ZEROLIGHT3>" 
				<< "<LINKNOISE0>" << myDesc->getLinkNoise0() << "</LINKNOISE0>" << "<LINKNOISE1>" << myDesc->getLinkNoise1() << "</LINKNOISE1>" << "<LINKNOISE2>" << myDesc->getLinkNoise2() << "</LINKNOISE2>" << "<LINKNOISE3>" << myDesc->getLinkNoise3() << "</LINKNOISE3>" 
				<< "<LIFTOFF0>" << myDesc->getLiftOff0() << "</LIFTOFF0>" << "<LIFTOFF1>" << myDesc->getLiftOff1() << "</LIFTOFF1>" << "<LIFTOFF2>" << myDesc->getLiftOff2() << "</LIFTOFF2>" << "<LIFTOFF3>" << myDesc->getLiftOff3() << "</LIFTOFF3>" 
				<< "<THRESHOLD0>" << myDesc->getThreshold0() << "</THRESHOLD0>" << "<THRESHOLD1>" << myDesc->getThreshold1() << "</THRESHOLD1>" << "<THRESHOLD2>" << myDesc->getThreshold2() << "</THRESHOLD2>" << "<THRESHOLD3>" << myDesc->getThreshold3() << "</THRESHOLD3>" 
				<< "<TICKHEIGHT0>" << myDesc->getTickHeight0() << "</TICKHEIGHT0>" << "<TICKHEIGHT1>" << myDesc->getTickHeight1() << "</TICKHEIGHT1>" << "<TICKHEIGHT2>" << myDesc->getTickHeight2() << "</TICKHEIGHT2>" << "<TICKHEIGHT3>" << myDesc->getTickHeight3() << "</TICKHEIGHT3>" 
				<< "<BASELINESLOP0>" << myDesc->getBaseLineSlop0() << "</BASELINESLOP0>" << "<BASELINESLOP1>" << myDesc->getBaseLineSlop1() << "</BASELINESLOP1>" << "<BASELINESLOP2>" << myDesc->getBaseLineSlop2() << "</BASELINESLOP2>" << "<BASELINESLOP3>" << myDesc->getBaseLineSlop3() << "</BASELINESLOP3>" 
				<< "</RAWANALYSIS>";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_PEDESTALS ) { 
			PedestalsAnalysisDescription *myDesc = static_cast<PedestalsAnalysisDescription*> ( description ); 
			memBuffer << "<RAWANALYSIS>" 
				<< "<CRATE>" << myDesc->getCrate() << "</CRATE>" << "<SLOT>" << myDesc->getSlot() << "</SLOT>" << "<RING>" << myDesc->getRing() << "</RING>" << "<CCUADR>" << myDesc->getCcuAdr() << "</CCUADR>" << "<CCUCHAN>" << myDesc->getCcuChan() << "</CCUCHAN>" << "<I2CADR>" << myDesc->getI2cAddr() << "</I2CADR>" << "<PARTITIONNAME>" << myDesc->getPartition() << "</PARTITIONNAME>" << "<RUNNUMBER>" << myDesc->getRunNumber() << "</RUNNUMBER>" << "<VALID>" << ( myDesc->isValid() ? "1" : "0" ) << "</VALID>" << "<COMMENTS>" << myDesc->getSerializedComments() << "</COMMENTS>" << "<FEDID>"  << myDesc->getFedId() << "</FEDID>" << "<FEUNIT>" << myDesc->getFeUnit() << "</FEUNIT>" << "<FECHAN>" << myDesc->getFeChan() << "</FECHAN>" << "<FEDAPV>" << myDesc->getFedApv() << "</FEDAPV>" << "<DEAD>" << myDesc->getDead() << "</DEAD>" << "<NOISY>" << myDesc->getNoisy() << "</NOISY>" << "<PEDSMEAN>" << myDesc->getPedsMean() << "</PEDSMEAN>" << "<PEDSSPREAD>" << myDesc->getPedsSpread() << "</PEDSSPREAD>" 
				<< "<NOISEMEAN>" << myDesc->getNoiseMean() << "</NOISEMEAN>" << "<NOISESPREAD>" << myDesc->getNoiseSpread() << "</NOISESPREAD>" << "<RAWMEAN>" << myDesc->getRawMean() << "</RAWMEAN>" << "<RAWSPREAD>" << myDesc->getRawSpread() << "</RAWSPREAD>" << "<PEDSMAX>" << myDesc->getPedsMax() << "</PEDSMAX>" << "<PEDSMIN>" << myDesc->getPedsMin() << "</PEDSMIN>" 
				<< "<NOISEMAX>" << myDesc->getNoiseMax() << "</NOISEMAX>" << "<NOISEMIN>" << myDesc->getNoiseMin() << "</NOISEMIN>" << "<RAWMAX>" << myDesc->getRawMax() << "</RAWMAX>" << "<RAWMIN>" << myDesc->getRawMin() << "</RAWMIN>" 
				<< "</RAWANALYSIS>";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_TIMING ) { 
			TimingAnalysisDescription *myDesc = static_cast<TimingAnalysisDescription*> ( description ); 
			memBuffer << "<RAWANALYSIS>" 
				<< "<CRATE>" << myDesc->getCrate() << "</CRATE>" << "<SLOT>" << myDesc->getSlot() << "</SLOT>" << "<RING>" << myDesc->getRing() << "</RING>" << "<CCUADR>" << myDesc->getCcuAdr() << "</CCUADR>" << "<CCUCHAN>" << myDesc->getCcuChan() << "</CCUCHAN>" << "<I2CADR>" << myDesc->getI2cAddr() << "</I2CADR>" << "<PARTITIONNAME>" << myDesc->getPartition() << "</PARTITIONNAME>" << "<RUNNUMBER>" << myDesc->getRunNumber() << "</RUNNUMBER>" << "<VALID>" << ( myDesc->isValid() ? "1" : "0" ) << "</VALID>" << "<COMMENTS>" << myDesc->getSerializedComments() << "</COMMENTS>" << "<FEDID>"  << myDesc->getFedId() << "</FEDID>" << "<FEUNIT>" << myDesc->getFeUnit() << "</FEUNIT>" << "<FECHAN>" << myDesc->getFeChan() << "</FECHAN>" << "<FEDAPV>" << myDesc->getFedApv() << "</FEDAPV>" << "<TIMETMRE>" << myDesc->getTimetmre() << "</TIMETMRE>" << "<REFTIME>" << myDesc->getRefTime() << "</REFTIME>" << "<DELAY>" << myDesc->getDelay() << "</DELAY>" << "<HEIGHT>" << myDesc->getHeight() << "</HEIGHT>" 
				<< "<BASE>" << myDesc->getBase() << "</BASE>" << "<PEAK>" << myDesc->getPeak() << "</PEAK>" << "<FRAMEFINDINGTHRESHOLD>" << myDesc->getFrameFindingThreshold() << "</FRAMEFINDINGTHRESHOLD>" << "<OPTIMUMSAMPLINGPOINT>" << myDesc->getOptimumSamplingPoint() << "</OPTIMUMSAMPLINGPOINT>" << "<TICKMARKHEIGHTTHRESHOLD>" << myDesc->getTickMarkHeightThreshold() << "</TICKMARKHEIGHTTHRESHOLD>" 
				<< "<KIND>" << myDesc->getKind() << "</KIND>" 
				<< "</RAWANALYSIS>";
		} else if ( analysisType == CommissioningAnalysisDescription::T_ANALYSIS_VPSPSCAN ) { 
			VpspScanAnalysisDescription *myDesc = static_cast<VpspScanAnalysisDescription*> ( description ); 
			memBuffer << "<RAWANALYSIS>" 
				<< "<CRATE>" << myDesc->getCrate() << "</CRATE>" << "<SLOT>" << myDesc->getSlot() << "</SLOT>" << "<RING>" << myDesc->getRing() << "</RING>" << "<CCUADR>" << myDesc->getCcuAdr() << "</CCUADR>" << "<CCUCHAN>" << myDesc->getCcuChan() << "</CCUCHAN>" << "<I2CADR>" << myDesc->getI2cAddr() << "</I2CADR>" << "<PARTITIONNAME>" << myDesc->getPartition() << "</PARTITIONNAME>" << "<RUNNUMBER>" << myDesc->getRunNumber() << "</RUNNUMBER>" << "<VALID>" << ( myDesc->isValid() ? "1" : "0" ) << "</VALID>" << "<COMMENTS>" << myDesc->getSerializedComments() << "</COMMENTS>" << "<FEDID>"  << myDesc->getFedId() << "</FEDID>" << "<FEUNIT>" << myDesc->getFeUnit() << "</FEUNIT>" << "<FECHAN>" << myDesc->getFeChan() << "</FECHAN>" << "<FEDAPV>" << myDesc->getFedApv() << "</FEDAPV>" << "<VPSP>" << myDesc->getVpsp() << "</VPSP>" << "<ADCLEVEL>" << myDesc->getAdcLevel() << "</ADCLEVEL>" << "<FRACTION>" << myDesc->getFraction() << "</FRACTION>" << "<TOPEDGE>" << myDesc->getTopEdge() << "</TOPEDGE>" << "<BOTTOMEDGE>" 
				<< myDesc->getBottomEdge() << "</BOTTOMEDGE>" << "<TOPLEVEL>" << myDesc->getTopLevel() << "</TOPLEVEL>" << "<BOTTOMLEVEL>" << myDesc->getBottomLevel() << "</BOTTOMLEVEL>" 
				<< "</RAWANALYSIS>";
		}
	}
}



/**Generates a dcuConversion element
 * @param dcuConversion - dcuConversionFactors description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLTkDcuConversion(TkDcuConversionFactors *dcuConversion, std::stringstream &memBuffer, bool forDb){
  if (!forDb) {
    memBuffer << "\t<DCUCONVERSION dcuHardId=\"" << dcuConversion->getDcuHardId()
	      << "\" subDetector=\"" << dcuConversion->getSubDetector()
	      << "\" dcuType=\"" << dcuConversion->getDcuType()
	      << "\" structureId=\"" << dcuConversion->getStructureId()
	      << "\" containerId=\"" << dcuConversion->getContainerId();
    //if(dcuConversion->getDetId()!=0)
    memBuffer<< "\" detId=\"" << dcuConversion->getDetId();
    memBuffer << "\" adcGain0=\"" << dcuConversion->getAdcGain0()
	      << "\" adcOffset0=\"" << dcuConversion->getAdcOffset0();
    //if(dcuConversion->getDcuType()=="FEH"){
    memBuffer << "\" adcInl0=\"" << dcuConversion->getAdcInl0()
	      << "\" adcInl0OW=\"" << (dcuConversion->getAdcInl0OW()?STRTRUE:STRFALSE);
    //}
    memBuffer << "\" adcCal0=\"" << (dcuConversion->getAdcCal0()?STRTRUE:STRFALSE)
	      << "\" i20=\"" << dcuConversion->getI20()
	      << "\" i10=\"" << dcuConversion->getI10()
	      << "\" iCal=\"" << (dcuConversion->getICal()?STRTRUE:STRFALSE);
    //if(dcuConversion->getDcuType()=="FEH"){
    memBuffer << "\" kDiv=\"" << dcuConversion->getKDiv()
	      << "\" kCal=\"" << (dcuConversion->getKDivCal()?STRTRUE:STRFALSE);
    //}
    memBuffer << "\" tsGain=\"" << dcuConversion->getTsGain()
	      << "\" tsOffset=\"" << dcuConversion->getTsOffset()
	      << "\" tsCal=\"" << (dcuConversion->getTsCal()?STRTRUE:STRFALSE);
    //if(dcuConversion->getDcuType()!="FEH" && dcuConversion->getSubDetector()=="TOB"){
    memBuffer << "\" r68=\"" << dcuConversion->getR68()
	      << "\" r68Cal=\"" << (dcuConversion->getR68Cal()?STRTRUE:STRFALSE);
    //}
    //if(dcuConversion->getDcuType()!="FEH"){
    memBuffer<< "\" adcGain2=\"" << dcuConversion->getAdcGain2()
	     << "\" adcOffset2=\"" << dcuConversion->getAdcOffset2()
	     << "\" adcCal2=\"" << (dcuConversion->getAdcCal2()?STRTRUE:STRFALSE)
	     << "\" adcGain3=\"" << dcuConversion->getAdcGain3()
	     << "\" adcOffset3=\"" << dcuConversion->getAdcOffset3()
	     << "\" adcCal3=\"" << (dcuConversion->getAdcCal3()?STRTRUE:STRFALSE);
    //}
    memBuffer<< "\" />" << std::endl ;
  }
  else{
    memBuffer << "\t<DCUCONVERSION><DCUHARDID>" << dcuConversion->getDcuHardId() << "</DCUHARDID>"
	      << "<SUBDETECTOR>" << dcuConversion->getSubDetector() << "</SUBDETECTOR>"
	      << "<DCUTYPE>" << dcuConversion->getDcuType()<< "</DCUTYPE>"
	      << "<STRUCTUREID>" << dcuConversion->getStructureId()<< "</STRUCTUREID>"
	      << "<CONTAINERID>" << dcuConversion->getContainerId()<< "</CONTAINERID>"
	      << "<DETID>" << dcuConversion->getDetId()<< "</DETID>";
    memBuffer << "<ADCGAIN0>" << dcuConversion->getAdcGain0()<< "</ADCGAIN0>"
	      << "<ADCOFFSET0>" << dcuConversion->getAdcOffset0()<< "</ADCOFFSET0>";
    if(dcuConversion->getDcuType()=="FEH"){
      memBuffer << "<ADCINL0>" << dcuConversion->getAdcInl0() << "</ADCINL0>"
		<< "<ADCINL0OW>" << (dcuConversion->getAdcInl0OW()?STRTRUE:STRFALSE)<< "</ADCINL0OW>";
    }
    memBuffer << "<ADCCAL0>" << (dcuConversion->getAdcCal0()?STRTRUE:STRFALSE)<< "</ADCCAL0>"
	      << "<I20>" << dcuConversion->getI20()<< "</I20>"
	      << "<I10>" << dcuConversion->getI10()<< "</I10>"
	      << "<ICAL>" << (dcuConversion->getICal()?STRTRUE:STRFALSE)<< "</ICAL>";
    if(dcuConversion->getDcuType()=="FEH"){
      memBuffer << "<KDIV>" << dcuConversion->getKDiv()<< "</KDIV>"
		<< "<KCAL>" << (dcuConversion->getKDivCal()?STRTRUE:STRFALSE)<< "</KCAL>";
    }
    memBuffer << "<TSGAIN>" << dcuConversion->getTsGain()<< "</TSGAIN>"
	      << "<TSOFFSET>" << dcuConversion->getTsOffset()<< "</TSOFFSET>"
	      << "<TSCAL>" << (dcuConversion->getTsCal()?STRTRUE:STRFALSE)<< "</TSCAL>";
    if(dcuConversion->getDcuType()!="FEH" && dcuConversion->getSubDetector()=="/TOB"){
      memBuffer << "<R68>" << dcuConversion->getR68()<< "</R68>"
		<< "<R68CAL>" << (dcuConversion->getR68Cal()?STRTRUE:STRFALSE)<< "</R68CAL>";
    }
    if(dcuConversion->getDcuType()!="FEH"){
      memBuffer<< "<ADCGAIN2>" << dcuConversion->getAdcGain2()<< "</ADCGAIN2>"
	       << "<ADCOFFSET2>" << dcuConversion->getAdcOffset2()<< "</ADCOFFSET2>"
	       << "<ADCCAL2>" << (dcuConversion->getAdcCal2()?STRTRUE:STRFALSE)<< "</ADCCAL2>"
	       << "<ADCGAIN3>" << dcuConversion->getAdcGain3()<< "</ADCGAIN3>"
	       << "<ADCOFFSET3>" << dcuConversion->getAdcOffset3()<< "</ADCOFFSET3>"
	       << "<ADCCAL3>" << (dcuConversion->getAdcCal3()?STRTRUE:STRFALSE)<< "</ADCCAL3>";
    }
    memBuffer<< "</DCUCONVERSION>" << std::endl ;
  }
};


/**Generates a dcuConversion element
 * @param dcuConversion - dcuConversionFactors description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLTkIdVsHostname(TkIdVsHostnameDescription *hostnameId, std::stringstream &memBuffer, bool forDb){

  if (!forDb) {
    memBuffer << "\t<IDVSHOSTNAME " 
	      << "subDetector=\"" << hostnameId->getSubDetector() << "\" "
	      << "crateId=\"" << hostnameId->getCrateId() << "\" "
	      << "hostname=\"" << hostnameId->getHostname() << "\" "
	      << "slot=\"" << hostnameId->getSlot() << "\" "
	      << "id=\"" << hostnameId->getId() << "\" "
	      << "crateNumber=\"" << hostnameId->getCrateNumber() << "\" " 
	      << "/>"
	      << std::endl ;
  }
  else {
    memBuffer << "\t<RAWIDVSHOSTNAME>" 
	      << "<SUBDETECTOR>" << hostnameId->getSubDetector() << "</SUBDETECTOR>"
	      << "<CRATEID>" << hostnameId->getCrateId() << "</CRATEID>"
	      << "<HOSTNAME>" << hostnameId->getHostname() << "</HOSTNAME>"
	      << "<SLOT>" << hostnameId->getSlot() << "</SLOT>"
	      << "<ID>" << hostnameId->getId() << "</ID>" 
	      << "<CRATENUMBER>" << hostnameId->getCrateNumber() << "</CRATENUMBER>"
	      << "</RAWIDVSHOSTNAME>" << std::endl ;
  }
};


/**Generates dcuInfo tag
 * @param dcuInfoParameters - dcuInfo descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the deviceType code is unknown
 * @see generateXMLTkDcuInfo(TkDcuInfo*, std::stringstream &);
 */
void MemBufOutputSource::generateTkDcuInfoTag(tkDcuInfoVector dcuInfoParameters, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;

  if (forDb) {
    memBuffer_ << "<ROWSET>" ;
  }
  
  for (tkDcuInfoVector::iterator it = dcuInfoParameters.begin() ; it != dcuInfoParameters.end() ; it ++) {
    TkDcuInfo *dcuInfod = *it;
    generateXMLTkDcuInfo((TkDcuInfo *)dcuInfod, memBuffer_, forDb);
  }

  if (forDb) {
    memBuffer_ << "</ROWSET>" ;
  }
}


/**Generates a dcuInfo element
 * @param dcuInfo - dcuInfo description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLTkDcuInfo(TkDcuInfo *dcuInfo, std::stringstream &memBuffer, bool forDb){
  if (!forDb) {
    memBuffer << "\t<DCUINFO dcuHardId=\"" << dcuInfo->getDcuHardId()
	      << "\" detId=\"" << dcuInfo->getDetId()
	      << "\" fibreLength=\"" << dcuInfo->getFibreLength()
	      << "\" apvNumber=\"" << dcuInfo->getApvNumber();
    if(dcuInfo->getTimeOfFlight()>0.0001)
      memBuffer<< "\" timeOfFlight=\"" << dcuInfo->getTimeOfFlight();
    memBuffer<< "\" />" << std::endl ;
  }
  else{
    memBuffer << "<DCUINFO>"
	      << "<DCUHARDID>" << dcuInfo->getDcuHardId() << "</DCUHARDID>"
	      << "<DETID>" << dcuInfo->getDetId() << "</DETID>";
    memBuffer << "<FIBRELENGTH>" << dcuInfo->getFibreLength() << "</FIBRELENGTH>" 
	      << "<APVNUMBER>" << dcuInfo->getApvNumber() << "</APVNUMBER>" ;
    if(dcuInfo->getTimeOfFlight()>0.0001)
      memBuffer<< "<TIMEOFFLIGHT>" << dcuInfo->getTimeOfFlight() << "</TIMEOFFLIGHT>" ;
    memBuffer<< "</DCUINFO>" << std::endl ;
  }
};

/**Generates dcuPsuMap tag
 * @param dcuPsuMapParameters - dcuPsuMap descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the deviceType code is unknown
 * @see generateXMLTkDcuInfo(TkDcuInfo*, std::stringstream &);
 */
void MemBufOutputSource::generateTkDcuPsuMapTag(tkDcuPsuMapVector dcuPsuMapParameters, bool forDb) throw (FecExceptionHandler) {
  std::stringstream errorMsg;

  if (forDb) {
    memBuffer_ << "<ROWSET>" ;
  }

  for (tkDcuPsuMapVector::iterator it = dcuPsuMapParameters.begin() ; it != dcuPsuMapParameters.end() ; it ++) {
    TkDcuPsuMap *dcuPsuMapd = *it;
    generateXMLTkDcuPsuMap((TkDcuPsuMap *)dcuPsuMapd, memBuffer_, forDb);
  }

  if (forDb) {
    memBuffer_ << "</ROWSET>" ;
  }
}


/**Generates a dcuPsuMap element
 * @param dcuPsuMap - dcuPsuMap description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLTkDcuPsuMap(TkDcuPsuMap *dcuPsuMap, std::stringstream &memBuffer, bool forDb){
  if (!forDb) {
    memBuffer << "\t<DCUPSUMAP dcuHardId=\"" << dcuPsuMap->getDcuHardId()
	      << "\" psuName=\"" << dcuPsuMap->getPsuName()
	      << "\" psuType=\"" << dcuPsuMap->getPsuType();
    memBuffer<< "\" />" << std::endl ;
  }
  else{
    memBuffer << "<RAWDCUPSUMAP>"
	      << "<DCUHARDID>" << dcuPsuMap->getDcuHardId() << "</DCUHARDID>"
	      << "<PSUNAME>" << dcuPsuMap->getPsuName() << "</PSUNAME>"
	      << "<PSUTYPE>" << dcuPsuMap->getPsuType() << "</PSUTYPE>";
    memBuffer<< "</RAWDCUPSUMAP>" << std::endl ;
  }
};

/**Generates a PiaReset element
 * @param piaReset - piaReset description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLPiaReset(piaResetDescription *piaReset, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<PIARESET crateSlot=\"" << (unsigned int)piaReset->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)piaReset->getFecSlot()
	      << "\" fecHardwareId=\"" << piaReset->getFecHardwareId()
	      << "\" enabled=\"" << (piaReset->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)piaReset->getRingSlot()
	      << "\" ccuAddress=\"" << (unsigned int)piaReset->getCcuAddress()
	      << "\" piaChannel=\"" << (unsigned int)piaReset->getChannel()
	      << "\" mask=\"" << (unsigned int)piaReset->getMask()
	      << "\" delayActiveReset=\"" << (unsigned int)piaReset->getDelayActiveReset()
	      << "\" intervalDelayReset=\"" << (unsigned int)piaReset->getIntervalDelayReset()
	      << "\" />" << std::endl ;
  } else {
    memBuffer << "<RAWPIARESET>"
	      << "<CRATESLOT>" << (unsigned int)piaReset->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)piaReset->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDID>" << piaReset->getFecHardwareId() << "</FECHARDID>"  
	      << "<ENABLED>" << (piaReset->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>"  
	      << "<RINGSLOT>" << (unsigned int)piaReset->getRingSlot() << "</RINGSLOT>" 
	      << "<CCUADDRESS>" << (unsigned int)piaReset->getCcuAddress() << "</CCUADDRESS>" 
	      << "<PIACHANNEL>" << (unsigned int)piaReset->getChannel() << "</PIACHANNEL>" 
	      << "<MASK>" << (unsigned int)piaReset->getMask() << "</MASK>" 
	      << "<DELAYACTIVERESET>" << (unsigned int)piaReset->getDelayActiveReset() << "</DELAYACTIVERESET>" 
	      << "<INTERVALDELAYRESET>" << (unsigned int)piaReset->getIntervalDelayReset() << "</INTERVALDELAYRESET>" 
	      << "</RAWPIARESET>" << std::endl ;
  }
};

/**Generates a Pll element
 * @param pll - pll description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLPll(pllDescription *pll, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<PLL crateSlot=\"" << (unsigned int)pll->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)pll->getFecSlot()
	      << "\" fecHardwareId=\"" << pll->getFecHardwareId()
	      << "\" enabled=\"" << (pll->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)pll->getRingSlot()
	      << "\" ccuAddress=\"" << (unsigned int)pll->getCcuAddress()
	      << "\" i2cChannel=\"" << (unsigned int)pll->getChannel()
	      << "\" i2cAddress=\"" << (unsigned int)pll->getAddress()
	      << "\" delayCoarse=\"" << (unsigned int)pll->getDelayCoarse()
	      << "\" delayFine=\"" << (unsigned int)pll->getDelayFine()
	      << "\" pllDac=\"" << (unsigned int)pll->getPllDac()
	      << "\" />" << std::endl ;;
  } else {
    memBuffer << "<RAWPLL>"
	      << "<CRATESLOT>" << (unsigned int)pll->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)pll->getFecSlot() << "</FECSLOT>" 
	      << "<FECHARDID>" << pll->getFecHardwareId() << "</FECHARDID>" 
	      << "<ENABLED>" << (pll->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>"  
	      << "<RINGSLOT>" << (unsigned int)pll->getRingSlot() << "</RINGSLOT>" 
	      << "<CCUADDRESS>" << (unsigned int)pll->getCcuAddress() << "</CCUADDRESS>" 
	      << "<I2CCHANNEL>" << (unsigned int)pll->getChannel() << "</I2CCHANNEL>" 
	      << "<I2CADDRESS>" << (unsigned int)pll->getAddress() << "</I2CADDRESS>" 
	      << "<DELAYCOARSE>" << (unsigned int)pll->getDelayCoarse() << "</DELAYCOARSE>" 
	      << "<DELAYFINE>" << (unsigned int)pll->getDelayFine() << "</DELAYFINE>"
	      << "<PLLDAC>" << (unsigned int)pll->getPllDac() << "</PLLDAC>"
	      << "</RAWPLL>" << std::endl ;;
  }
};

/**Generates a Ccu element
 * @param ccu - ccu description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLCcu(CCUDescription *ccu, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<CCU crateSlot=\"" << (unsigned int)ccu->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)getFecKey(ccu->getKey())
	      << "\" fecHardwareId=\"" << ccu->getFecHardwareId()
	      << "\" ringSlot=\"" << (unsigned int)getRingKey(ccu->getKey())
	      << "\" ccuAddress=\"" << (unsigned int)getCcuKey(ccu->getKey())
	      << "\" enabled=\"" <<  (((unsigned int)ccu->getEnabled()) ? 'T' : 'F' )
	      << "\" arrangement=\"" << (unsigned int)ccu->getOrder()
	      << "\" />" << std::endl ;;
    
  } else {
    memBuffer << "<RAWCCU>"
	      << "<CRATESLOT>" << (unsigned int)ccu->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)getFecKey(ccu->getKey()) << "</FECSLOT>" 
	      << "<FECHARDID>" << ccu->getFecHardwareId() << "</FECHARDID>" 
	      << "<RINGSLOT>" << (unsigned int)getRingKey(ccu->getKey()) << "</RINGSLOT>" 
	      << "<CCUADDRESS>" << (unsigned int)getCcuKey(ccu->getKey()) << "</CCUADDRESS>" 
	      << "<ENABLED>" << (((unsigned int)ccu->getEnabled()) ? 'T' : 'F') << "</ENABLED>" 
	      << "<ARRANGEMENT>" << (unsigned int)ccu->getOrder() << "</ARRANGEMENT>" 
	      << "</RAWCCU>" << std::endl ;;
              
  }
};

/**Generates a tkRing element
 * @param tkRing - TkRingDescription
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLTkRing(TkRingDescription tkRing, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<RING crateSlot=\"" << (unsigned int)tkRing.getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)getFecKey(tkRing.getKey())
	      << "\" fecHardwareId=\"" << tkRing.getFecHardwareId()
	      << "\" ringSlot=\"" << (unsigned int)getRingKey(tkRing.getKey())
	      << "\" enabled=\"" <<  (((unsigned int)tkRing.getEnabled()) ? 'T' : 'F' )
	      << "\" inputAUsed=\"" <<  (((unsigned int)tkRing.getInputAUsed()) ? 'T' : 'F' )
	      << "\" outputAUsed=\"" <<  (((unsigned int)tkRing.getOutputAUsed()) ? 'T' : 'F' )
	      << "\" />" << std::endl ;
    
  } else {
    memBuffer << "<RAWRING>"
	      << "<CRATESLOT>" << (unsigned int)tkRing.getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)getFecKey(tkRing.getKey()) << "</FECSLOT>" 
	      << "<FECHARDID>" << tkRing.getFecHardwareId() << "</FECHARDID>" 
	      << "<RINGSLOT>" << (unsigned int)getRingKey(tkRing.getKey()) << "</RINGSLOT>" 
	      << "<ENABLED>" << (((unsigned int)tkRing.getEnabled()) ? 'T' : 'F') << "</ENABLED>" 
	      << "<INPUTAUSED>" << (((unsigned int)tkRing.getInputAUsed()) ? 'T' : 'F') << "</INPUTAUSED>" 
	      << "<OUTPUTAUSED>" << (((unsigned int)tkRing.getOutputAUsed()) ? 'T' : 'F') << "</OUTPUTAUSED>" 
	      << "</RAWRING>" << std::endl ;
              
  }
};

/**Generates a Laserdriver element
 * @param laserdriver - laserdriver description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLLaserdriver(laserdriverDescription *laserdriver, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<LASERDRIVER crateSlot=\"" << (unsigned int)laserdriver->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)laserdriver->getFecSlot()
	      << "\" fecHardwareId=\"" << laserdriver->getFecHardwareId()
	      << "\" enabled=\"" << (laserdriver->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)laserdriver->getRingSlot()
	      << "\" ccuAddress=\"" << (unsigned int)laserdriver->getCcuAddress()
	      << "\" i2cChannel=\"" << (unsigned int)laserdriver->getChannel()
	      << "\" i2cAddress=\"" << (unsigned int)laserdriver->getAddress()
	      << "\" bias0=\"" << (unsigned int)laserdriver->getBias0()
	      << "\" bias1=\"" << (unsigned int)laserdriver->getBias1()
	      << "\" bias2=\"" << (unsigned int)laserdriver->getBias2()
	      << "\" gain0=\"" << (unsigned int)laserdriver->getGain0()
	      << "\" gain1=\"" << (unsigned int)laserdriver->getGain1()
	      << "\" gain2=\"" << (unsigned int)laserdriver->getGain2()
	      << "\" />" << std::endl ;
  } else {
    memBuffer << "<RAWLASERDRIVER>"
	      << "<CRATESLOT>" << (unsigned int)laserdriver->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)laserdriver->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDID>" << laserdriver->getFecHardwareId() << "</FECHARDID>"
	      << "<ENABLED>" << (laserdriver->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>"  
	      << "<RINGSLOT>" << (unsigned int)laserdriver->getRingSlot() << "</RINGSLOT>" 
	      << "<CCUADDRESS>" << (unsigned int)laserdriver->getCcuAddress() << "</CCUADDRESS>" 
	      << "<I2CCHANNEL>" << (unsigned int)laserdriver->getChannel() << "</I2CCHANNEL>"
	      << "<I2CADDRESS>" << (unsigned int)laserdriver->getAddress() << "</I2CADDRESS>" 
	      << "<BIAS0>" << (unsigned int)laserdriver->getBias0() << "</BIAS0>"
	      << "<BIAS1>" << (unsigned int)laserdriver->getBias1() << "</BIAS1>"
	      << "<BIAS2>" << (unsigned int)laserdriver->getBias2() << "</BIAS2>"
	      << "<GAIN0>" << (unsigned int)laserdriver->getGain0() << "</GAIN0>"
	      << "<GAIN1>" << (unsigned int)laserdriver->getGain1() << "</GAIN1>"
	      << "<GAIN2>" << (unsigned int)laserdriver->getGain2() << "</GAIN2>"
	      << "</RAWLASERDRIVER>" << std::endl ;;
  };
};
/**Generates a Apv25 element
 * @param apv25 - apv25 description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLApv25(apvDescription *apv25, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<APV25 crateSlot=\"" << (unsigned int)apv25->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)apv25->getFecSlot()
	      << "\" fecHardwareId=\"" << apv25->getFecHardwareId()
	      << "\" enabled=\"" << (apv25->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)apv25->getRingSlot()
	      << "\" ccuAddress=\"" << (unsigned int)apv25->getCcuAddress()
	      << "\" i2cChannel=\"" << (unsigned int)apv25->getChannel()
	      << "\" i2cAddress=\"" << (unsigned int)apv25->getAddress()
	      << "\" apvError=\"" << (unsigned int)apv25->getApvError()
	      << "\" apvMode=\""  << (unsigned int)apv25->getApvMode()
	      << "\" cdrv=\""  << (unsigned int)apv25->getCdrv()
	      << "\" csel=\""  << (unsigned int)apv25->getCsel()
	      << "\" ical=\""  << (unsigned int)apv25->getIcal()
	      << "\" imuxin=\""  << (unsigned int)apv25->getImuxin()
	      << "\" ipcasc=\""  << (unsigned int)apv25->getIpcasc()
	      << "\" ipre=\""  << (unsigned int)apv25->getIpre()
	      << "\" ipsf=\""  << (unsigned int)apv25->getIpsf()
	      << "\" ipsp=\""  << (unsigned int)apv25->getIpsp()
	      << "\" isha=\""  << (unsigned int)apv25->getIsha()
	      << "\" ispare=\""  << (unsigned int)apv25->getIspare()
	      << "\" issf=\""  << (unsigned int)apv25->getIssf()
	      << "\" latency=\""  << (unsigned int)apv25->getLatency()
	      << "\" muxGain=\""  << (unsigned int)apv25->getMuxGain()
	      << "\" vfp=\""  << (unsigned int)apv25->getVfp()
	      << "\" vfs=\""  << (unsigned int)apv25->getVfs()
	      << "\" vpsp=\""  << (unsigned int)apv25->getVpsp()
	      << "\" />" << std::endl ;
  } else {
    memBuffer << "<RAWAPVFEC>"
	      << "<CRATESLOT>" << (unsigned int)apv25->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)apv25->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDID>" << apv25->getFecHardwareId() << "</FECHARDID>"
	      << "<ENABLED>" << (apv25->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>" 
	      << "<RINGSLOT>" << (unsigned int)apv25->getRingSlot() << "</RINGSLOT>"
	      << "<CCUADDRESS>" << (unsigned int)apv25->getCcuAddress() << "</CCUADDRESS>" 
	      << "<I2CCHANNEL>" << (unsigned int)apv25->getChannel() << "</I2CCHANNEL>" 
	      << "<I2CADDRESS>" << (unsigned int)apv25->getAddress() << "</I2CADDRESS>"
	      << "<APVERROR>" << (unsigned int)apv25->getApvError() << "</APVERROR>"
	      << "<APVMODE>" << (unsigned int)apv25->getApvMode() << "</APVMODE>"
	      << "<CDRV>"  << (unsigned int)apv25->getCdrv() << "</CDRV>"
	      << "<CSEL>"  << (unsigned int)apv25->getCsel() << "</CSEL>"
	      << "<ICAL>"  << (unsigned int)apv25->getIcal() << "</ICAL>"
	      << "<IMUXIN>"  << (unsigned int)apv25->getImuxin() << "</IMUXIN>"
	      << "<IPCASC>"  << (unsigned int)apv25->getIpcasc() << "</IPCASC>"
	      << "<IPRE>"  << (unsigned int)apv25->getIpre() << "</IPRE>"
	      << "<IPSF>"  << (unsigned int)apv25->getIpsf() << "</IPSF>"
	      << "<IPSP>"  << (unsigned int)apv25->getIpsp() << "</IPSP>"
	      << "<ISHA>"  << (unsigned int)apv25->getIsha() << "</ISHA>"
	      << "<ISPARE>"  << (unsigned int)apv25->getIspare() << "</ISPARE>"
	      << "<ISSF>"  << (unsigned int)apv25->getIssf() << "</ISSF>"
	      << "<LATENCY>"  << (unsigned int)apv25->getLatency() << "</LATENCY>"
	      << "<MUXGAIN>"  << (unsigned int)apv25->getMuxGain() << "</MUXGAIN>" 
	      << "<VFP>"  << (unsigned int)apv25->getVfp() << "</VFP>" 
	      << "<VFS>"  << (unsigned int)apv25->getVfs() << "</VFS>" 
	      << "<VPSP>"  << (unsigned int)apv25->getVpsp() << "</VPSP>"
	      << "</RAWAPVFEC>" << std::endl ;
  }
};

/**Generates a ApvMux element
 * @param apvMux - apvMux description
 * @param memBuffer - XML buffer to fill
 */
/**generates an ApvMux element
 */
void MemBufOutputSource::generateXMLApvMux(muxDescription *apvMux, std::stringstream &memBuffer, bool forDb) {  
  if (!forDb) {
    memBuffer << "\t<APVMUX crateSlot=\"" << (unsigned int)apvMux->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)apvMux->getFecSlot()
	      << "\" fecHardwareId=\"" << apvMux->getFecHardwareId()
	      << "\" enabled=\"" << (apvMux->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)apvMux->getRingSlot() 
	      << "\" ccuAddress=\"" << (unsigned int)apvMux->getCcuAddress() 
	      << "\" i2cChannel=\"" << (unsigned int)apvMux->getChannel() 
	      << "\" i2cAddress=\"" << (unsigned int)apvMux->getAddress() 
	      << "\" resistor=\"" << (unsigned int)apvMux->getResistor()
	      << "\" />" << std::endl ;;
  } else {
    memBuffer << "<RAWAPVMUX>"
	      << "<CRATESLOT>" << (unsigned int)apvMux->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)apvMux->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDID>" << apvMux->getFecHardwareId() << "</FECHARDID>" 
	      << "<ENABLED>" << (apvMux->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>"  
	      << "<RINGSLOT>" << (unsigned int)apvMux->getRingSlot() << "</RINGSLOT>"
	      << "<CCUADDRESS>" << (unsigned int)apvMux->getCcuAddress() << "</CCUADDRESS>"
	      << "<I2CCHANNEL>" << (unsigned int)apvMux->getChannel() << "</I2CCHANNEL>"
	      << "<I2CADDRESS>" << (unsigned int)apvMux->getAddress() << "</I2CADDRESS>"
	      << "<RESISTOR>" << (unsigned int)apvMux->getResistor() << "</RESISTOR>"
	      << "</RAWAPVMUX>" << std::endl ;
  }
};

/**Generates a Philips element
 * @param philips - philips description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLPhilips(philipsDescription *philips, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<PHILIPS crateSlot=\"" << (unsigned int)philips->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)philips->getFecSlot()
	      << "\" fecHardwareId=\"" << philips->getFecHardwareId()
	      << "\" enabled=\"" << (philips->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)philips->getRingSlot() 
	      << "\" ccuAddress=\"" << (unsigned int)philips->getCcuAddress() 
	      << "\" i2cChannel=\"" << (unsigned int)philips->getChannel() 
	      << "\" i2cAddress=\"" << (unsigned int)philips->getAddress() 
	      << "\" register=\"" << (unsigned int)philips->getRegister()
	      << "\" />" << std::endl ;;
  } else {
    memBuffer << "<RAWPHILIPS>"
	      << "<CRATESLOT>" << (unsigned int)philips->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)philips->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDWAREID>" << philips->getFecHardwareId() << "</FECHARDWAREID>"
	      << "<ENABLED>" << (philips->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>" 
	      << "<RINGSLOT>" << (unsigned int)philips->getRingSlot() << "</RINGSLOT>"  
	      << "<CCUADDRESS>" << (unsigned int)philips->getCcuAddress() << "</CCUADDRESS>"
	      << "<I2CCHANNEL>" << (unsigned int)philips->getChannel() << "</I2CCHANNEL>"
	      << "<I2CADDRESS>" << (unsigned int)philips->getAddress() << "</I2CADDRESS>"
	      << "<REGISTER>" << (unsigned int)philips->getRegister() << "</REGISTER>"
	      << "</RAWPHILIPS>" << std::endl ;
  }
};

/**Generates a Dcu element
 * @param dcu - dcu description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLDcu(dcuDescription *dcu, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<DCU crateSlot=\"" << (unsigned int)dcu->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)dcu->getFecSlot()
	      << "\" fecHardwareId=\"" << dcu->getFecHardwareId()
	      << "\" enabled=\"" << (dcu->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" dcuReadoutEnabled=\"" << (dcu->getDcuReadoutEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)dcu->getRingSlot()
	      << "\" ccuAddress=\"" << (unsigned int)dcu->getCcuAddress()
	      << "\" i2cChannel=\"" << (unsigned int)dcu->getChannel()
	      << "\" i2cAddress=\"" << (unsigned int)dcu->getAddress();

    if (dcu->getDcuType().length() > 0) {
      memBuffer << "\" dcuType=\"" << dcu->getDcuType();      
    }

    memBuffer    << "\" channel0=\"" << (unsigned int)dcu->getDcuChannel0()
		 << "\" channel1=\"" << (unsigned int)dcu->getDcuChannel1()
		 << "\" channel2=\"" << (unsigned int)dcu->getDcuChannel2()
		 << "\" channel3=\"" << (unsigned int)dcu->getDcuChannel3()
		 << "\" channel4=\"" << (unsigned int)dcu->getDcuChannel4()
		 << "\" channel5=\"" << (unsigned int)dcu->getDcuChannel5()
		 << "\" channel6=\"" << (unsigned int)dcu->getDcuChannel6()
		 << "\" channel7=\"" << (unsigned int)dcu->getDcuChannel7()
		 << "\" dcuHardId=\"" << (unsigned int)dcu->getDcuHardId()
		 << "\" dcuTimeStamp=\"" << (unsigned int)dcu->getTimeStamp()
		 << "\" />" << std::endl ;
  } else {
    memBuffer << "<RAWDCU>"
	      << "<CRATESLOT>" << (unsigned int)dcu->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)dcu->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDID>" << dcu->getFecHardwareId() << "</FECHARDID>"
	      << "<ENABLED>" << (dcu->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>" 
	      << "<DCUREADOUTENABLED>" << (dcu->getDcuReadoutEnabled() ? STRTRUE : STRFALSE) << "</DCUREADOUTENABLED>" 
	      << "<RINGSLOT>" << (unsigned int)dcu->getRingSlot() << "</RINGSLOT>"
	      << "<CCUADDRESS>" << (unsigned int)dcu->getCcuAddress() << "</CCUADDRESS>"
	      << "<I2CCHANNEL>" << (unsigned int)dcu->getChannel() << "</I2CCHANNEL>" 
	      << "<I2CADDRESS>" << (unsigned int)dcu->getAddress() << "</I2CADDRESS>"
	      << "<DCUTYPE>" << dcu->getDcuType() << "</DCUTYPE>"
	      << "<CHANNEL0>" << (unsigned int)dcu->getDcuChannel0() << "</CHANNEL0>"
	      << "<CHANNEL1>" << (unsigned int)dcu->getDcuChannel1() << "</CHANNEL1>"
	      << "<CHANNEL2>" << (unsigned int)dcu->getDcuChannel2() << "</CHANNEL2>"
	      << "<CHANNEL3>" << (unsigned int)dcu->getDcuChannel3() << "</CHANNEL3>"
	      << "<CHANNEL4>" << (unsigned int)dcu->getDcuChannel4() << "</CHANNEL4>"
	      << "<CHANNEL5>" << (unsigned int)dcu->getDcuChannel5() << "</CHANNEL5>"
	      << "<CHANNEL6>" << (unsigned int)dcu->getDcuChannel6() << "</CHANNEL6>"
	      << "<CHANNEL7>" << (unsigned int)dcu->getDcuChannel7() << "</CHANNEL7>"
	      << "<DCUHARDID>" << (unsigned int)dcu->getDcuHardId() << "</DCUHARDID>"
	      << "<DCUTIMESTAMP>" << (unsigned int)dcu->getTimeStamp() << "</DCUTIMESTAMP>"
	      << "</RAWDCU>" << std::endl ;
  }
};

#ifdef PRESHOWER
/**Generates a delta element
 * @param delta - delta description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLDelta(deltaDescription* delta, std::stringstream &memBuffer) {
  memBuffer << "\t<DELTA crateSlot=\"" << (unsigned int)delta->getCrateId()
	    << "\" fecSlot=\"" << (unsigned int)delta->getFecSlot()
	    << "\" fecHardwareId=\"" << delta->getFecHardwareId()
	    << "\" enabled=\"" << (delta->getEnabled() ? STRTRUE : STRFALSE)
	    << "\" ringSlot=\"" << (unsigned int)delta->getRingSlot()
	    << "\" ccuAddress=\"" << (unsigned int)delta->getCcuAddress()
	    << "\" i2cChannel=\"" << (unsigned int)delta->getChannel()
	    << "\" i2cAddress=\"" << (unsigned int)delta->getAddress()
	    << "\" CR0=\"" << (unsigned int)delta->getCR0()
	    << "\" CR1=\"" << (unsigned int)delta->getCR1()
	    << "\" ChipID=\""  << (unsigned int)delta->getChipID()
    //           << "\" chipid1=\""  << (unsigned int)delta->getChipID1()
	    << "\" CalChan=\""  << (unsigned int)delta->getCalChan()
    //           << "\" calchan1=\""  << (unsigned int)delta->getCalChan1()
    //           << "\" calchan2=\""  << (unsigned int)delta->getCalChan2()
    //           << "\" calchan3=\""  << (unsigned int)delta->getCalChan3()
	    << "\" Vcal=\""  << (unsigned int)delta->getVcal()
	    << "\" VoPreamp=\""  << (unsigned int)delta->getVoPreamp()
	    << "\" VoShaper=\""  << (unsigned int)delta->getVoShaper()
	    << "\" Vspare=\""  << (unsigned int)delta->getVspare()
	    << "\" Ipreamp=\""  << (unsigned int)delta->getIpreamp()
	    << "\" Ishaper=\""  << (unsigned int)delta->getIshaper()
	    << "\" ISF=\""  << (unsigned int)delta->getISF()
	    << "\" Ispare=\""  << (unsigned int)delta->getIspare()
	    << "\" />" << std::endl ;
};

/**Generates a Paceam element
 * @param pace - pacem description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLPace(paceDescription* pace, std::stringstream &memBuffer){
  memBuffer << "\t<PACEAM crateSlot=\"" << (unsigned int)pace->getCrateId()
	    << "\" fecSlot=\"" << (unsigned int)pace->getFecSlot()
	    << "\" fecHardwareId=\"" << pace->getFecHardwareId()
	    << "\" enabled=\"" << (pace->getEnabled() ? STRTRUE : STRFALSE)
	    << "\" ringSlot=\"" << (unsigned int)pace->getRingSlot()
	    << "\" ccuAddress=\"" << (unsigned int)pace->getCcuAddress()
	    << "\" i2cChannel=\"" << (unsigned int)pace->getChannel()
	    << "\" i2cAddress=\"" << (unsigned int)pace->getAddress()
	    << "\" CR=\"" << (unsigned int)pace->getCR()
	    << "\" Latency=\"" << (unsigned int)pace->getLatency()
	    << "\" ChipID=\""  << (unsigned int)pace->getChipID()
    //           << "\" chipid1=\""  << (unsigned int)pace->getChipID1()
	    << "\" Vmemref=\""  << (unsigned int)pace->getVmemref()
	    << "\" Vshifter=\""  << (unsigned int)pace->getVshift()
	    << "\" Voutbuf=\""  << (unsigned int)pace->getVoutbuf()
	    << "\" Ireadamp=\""  << (unsigned int)pace->getIreadamp()
	    << "\" Ishifter=\""  << (unsigned int)pace->getIshift()
	    << "\" Imuxbuf=\""  << (unsigned int)pace->getImuxbuf()
	    << "\" Ioutbuf=\""  << (unsigned int)pace->getIoutbuf()
	    << "\" Ispare=\""  << (unsigned int)pace->getIspare()
	    << "\" Upset=\""  << (unsigned int)pace->getUpsetReg()
	    << "\" />" << std::endl ;
};

/**Generates a Kchip element
 * @param kchip - kchip description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLKchip(kchipDescription* kchip, std::stringstream &memBuffer){
  memBuffer << "\t<KCHIP crateSlot=\"" << (unsigned int)kchip->getCrateId()
	    << "\" fecSlot=\"" << (unsigned int)kchip->getFecSlot()
	    << "\" fecHardwareId=\"" << kchip->getFecHardwareId()
	    << "\" enabled=\"" << (kchip->getEnabled() ? STRTRUE : STRFALSE)
	    << "\" ringSlot=\"" << (unsigned int)kchip->getRingSlot()
	    << "\" ccuAddress=\"" << (unsigned int)kchip->getCcuAddress()
	    << "\" i2cChannel=\"" << (unsigned int)kchip->getChannel()
	    << "\" i2cAddress=\"" << (unsigned int)kchip->getAddress()
	    << "\" Config=\"" << (unsigned int)kchip->getCR()
	    << "\" EConfig=\"" << (unsigned int)kchip->getECR()
	    << "\" KID=\""  << (unsigned int)kchip->getKid()
    //           << "\" kid_high=\""  << (unsigned int)kchip->getKid_H()
	    << "\" Mask_T1cmd=\""  << (unsigned int)kchip->getMaskT1Cmd()
	    << "\" Last_T1cmd=\""  << (unsigned int)kchip->getLastT1Cmd()
	    << "\" Latency=\""  << (unsigned int)kchip->getLatency()
	    << "\" EvCnt=\"" << (unsigned int)kchip->getEvCnt()
	    << "\" BnchCnt=\""  << (unsigned int)kchip->getBnchCnt()
    //           << "\" bnchcnt_high=\""  << (unsigned int)kchip->getBnchCnt_H()
	    << "\" GintBUSY=\""  << (unsigned int)kchip->getGintBusy()
	    << "\" GintIDLE=\""  << (unsigned int)kchip->getGintIdle()
    ///          << "\" fifomap=\""  << (unsigned int)kchip->getFifoMap()
    //           << "\" fifodata_low=\""  << (unsigned int)kchip->getFifoData_L()
    //           << "\" fifodata_high=\""  << (unsigned int)kchip->getFifoData_H()
	    << "\" Status0=\""  << (unsigned int)kchip->getSR0()
	    << "\" Status1=\""  << (unsigned int)kchip->getSR1()
	    << "\" SeuCnt=\""  << (unsigned int)kchip->getSeuCounter()
	    << "\" CalPulseDelay=\""  << (unsigned int)kchip->getCalPulseDelay()
	    << "\" CalPulseWidth=\""  << (unsigned int)kchip->getCalPulseWidth()
	    << "\" AdcLatency=\""  << (unsigned int)kchip->getAdcLatency()
	    << "\" PfifoOvfl=\""  << (unsigned int)kchip->getPFifoOverflow()
	    << "\" ChipID=\""  << (unsigned int)kchip->getChipID()
    //           << "\" chipid1=\""  << (unsigned int)kchip->getChipID1()
	    << "\" />" << std::endl ;
};


/**Generates a Goh element
 * @param goh - goh description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLGoh(gohDescription* goh, std::stringstream &memBuffer){
  memBuffer << "\t<GOH crateSlot=\"" << (unsigned int)goh->getCrateId()
	    << "\" fecSlot=\"" << (unsigned int)goh->getFecSlot()
	    << "\" fecHardwareId=\"" << goh->getFecHardwareId()
	    << "\" enabled=\"" << (goh->getEnabled() ? STRTRUE : STRFALSE)
	    << "\" ringSlot=\"" << (unsigned int)goh->getRingSlot()
	    << "\" ccuAddress=\"" << (unsigned int)goh->getCcuAddress()
	    << "\" i2cChannel=\"" << (unsigned int)goh->getChannel()
	    << "\" i2cAddress=\"" << (unsigned int)goh->getAddress()
	    << "\" Control0=\"" << (unsigned int)goh->getControl0()
	    << "\" Control1=\"" << (unsigned int)goh->getControl1()
	    << "\" Control2=\"" << (unsigned int)goh->getControl2()
	    << "\" Control3=\"" << (unsigned int)goh->getControl3()    
	    << "\" Status0=\"" << (unsigned int)goh->getStatus0()
	    << "\" Status1=\"" << (unsigned int)goh->getStatus1()

	    << "\" />" << std::endl ;
}

/**Generates a EsMbReset element
 * @param esmbreset - EsMbReset description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLEsMbReset(esMbResetDescription *esmbreset, std::stringstream &memBuffer) { 
  memBuffer << "\t<ESMBRESET crateSlot=\"" << (unsigned int)esmbreset->getCrateId()
	    << "\" fecSlot=\"" << (unsigned int)esmbreset->getFecSlot()
	    << "\" fecHardwareId=\"" << esmbreset->getFecHardwareId()
	    << "\" enabled=\"" << (esmbreset->getEnabled() ? STRTRUE : STRFALSE)
	    << "\" ringSlot=\"" << (unsigned int)esmbreset->getRingSlot()
	    << "\" ccuAddress=\"" << (unsigned int)esmbreset->getCcuAddress()
	    << "\" esMbChannel=\"" << (unsigned int)esmbreset->getChannel()
	    << "\" mask=\"" << (unsigned int)esmbreset->getMask()
	    << "\" delayActiveReset=\"" << (unsigned int)esmbreset->getDelayActiveReset()
	    << "\" intervalDelayReset=\"" << (unsigned int)esmbreset->getIntervalDelayReset()
	    << "\" />" << std::endl ;


}
 
#endif // PRESHOWER

#ifdef TOTEM
/**Generates a Vfat element
 * @param vfat - vfat description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLVfat(vfatDescription *vfat, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<VFAT crateSlot=\"" << (unsigned int)vfat->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)vfat->getFecSlot()
	      << "\" fecHardwareId=\"" << vfat->getFecHardwareId()
	      << "\" enabled=\"" << (vfat->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)vfat->getRingSlot() 
	      << "\" ccuAddress=\"" << (unsigned int)vfat->getCcuAddress() 
	      << "\" i2cChannel=\"" << (unsigned int)vfat->getChannel() 
	      << "\" i2cAddress=\"" << (unsigned int)vfat->getAddress() 
	      << "\" control0=\"" << (unsigned int)vfat->getControl0()
	      << "\" control1=\"" << (unsigned int)vfat->getControl1()
	      << "\" ipreampin=\"" << (unsigned int)vfat->getIPreampIn()
	      << "\" ipreampfeed=\"" << (unsigned int)vfat->getIPreampFeed()
	      << "\" ipreampout=\"" << (unsigned int)vfat->getIPreampOut()
	      << "\" ishaper=\"" << (unsigned int)vfat->getIShaper()
	      << "\" ishaperfeed=\"" << (unsigned int)vfat->getIShaperFeed()
	      << "\" icomp=\"" << (unsigned int)vfat->getIComp()
	      << "\" chipid0=\"" << (unsigned int)vfat->getChipid0()
	      << "\" chipid1=\"" << (unsigned int)vfat->getChipid1()
	      << "\" upset=\"" << (unsigned int)vfat->getUpset()
	      << "\" hitcount0=\"" << (unsigned int)vfat->getHitCount0()
	      << "\" hitcount1=\"" << (unsigned int)vfat->getHitCount1()
	      << "\" hitcount2=\"" << (unsigned int)vfat->getHitCount2()
	      << "\" lat=\"" << (unsigned int)vfat->getLat()
	      << "\"" ;

    for(int i=1;i<129;i++)
      {
	memBuffer << " chanreg"<< i << "=\"" << (unsigned int)vfat->getChanReg(i) << "\"" ;
      }      

    memBuffer << " vcal=\"" << (unsigned int)vfat->getVCal()
	      << "\" vthreshold1=\"" << (unsigned int)vfat->getVThreshold1()
	      << "\" vthreshold2=\"" << (unsigned int)vfat->getVThreshold2()
	      << "\" calphase=\"" << (unsigned int)vfat->getCalPhase()
	      << "\" control2=\"" << (unsigned int)vfat->getControl2()
	      << "\" control3=\"" << (unsigned int)vfat->getControl3()
	      << "\" />" << std::endl ;;
  } else {
    memBuffer << "<RAWVFAT>"
	      << "<CRATESLOT>" << (unsigned int)vfat->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)vfat->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDWAREID>" << vfat->getFecHardwareId() << "</FECHARDWAREID>"
	      << "<ENABLED>" << (vfat->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>" 
	      << "<RINGSLOT>" << (unsigned int)vfat->getRingSlot() << "</RINGSLOT>"  
	      << "<CCUADDRESS>" << (unsigned int)vfat->getCcuAddress() << "</CCUADDRESS>"
	      << "<I2CCHANNEL>" << (unsigned int)vfat->getChannel() << "</I2CCHANNEL>"
	      << "<I2CADDRESS>" << (unsigned int)vfat->getAddress() << "</I2CADDRESS>"
	      << "<CONTROL0>" << (unsigned int)vfat->getControl0() << "</CONTROL0>"
	      << "<CONTROL1>" << (unsigned int)vfat->getControl1() << "</CONTROL1>"
	      << "<IPREAMPIN>" << (unsigned int)vfat->getIPreampIn() << "</IPREAMPIN>"
	      << "<IPREAMPFEED>" << (unsigned int)vfat->getIPreampFeed() << "</IPREAMPFEED>"
	      << "<IPREAMPOUT>" << (unsigned int)vfat->getIPreampOut() << "</IPREAMPOUT>"
	      << "<ISHAPER>" << (unsigned int)vfat->getIShaper() << "</ISHAPER>"
	      << "<ISHAPERFEED>" << (unsigned int)vfat->getIShaperFeed() << "</ISHAPERFEED>"
	      << "<ICOMP>" << (unsigned int)vfat->getIComp() << "</ICOMP>"
	      << "<CHIPID0>" << (unsigned int)vfat->getChipid0() << "</CHIPID0>"
	      << "<CHIPID1>" << (unsigned int)vfat->getChipid1() << "</CHIPID1>"
	      << "<UPSET>" << (unsigned int)vfat->getUpset() << "</UPSET>"
	      << "<HITCOUNT0>" << (unsigned int)vfat->getHitCount0() << "</HITCOUNT0>"
	      << "<HITCOUNT1>" << (unsigned int)vfat->getHitCount1() << "</HITCOUNT1>"
	      << "<HITCOUNT2>" << (unsigned int)vfat->getHitCount2() << "</HITCOUNT2>"
	      << "<LAT>" << (unsigned int)vfat->getLat() << "</LAT>" ;

    for(int i=1;i<129;i++)
      {
	memBuffer << "<CHANREG"<< i << ">" << (unsigned int)vfat->getChanReg(i) << "</CHANREG"<< i << ">";
      }  

    memBuffer << "<VTHRESHOLD1>" << (unsigned int)vfat->getVThreshold1() << "</VTHRESHOLD1>"
	      << "<VTHRESHOLD2>" << (unsigned int)vfat->getVThreshold2() << "</VTHRESHOLD2>"
	      << "<CALPHASE>" << (unsigned int)vfat->getCalPhase() << "</CALPHASE>"
	      << "<CONTROL2>" << (unsigned int)vfat->getControl2() << "</CONTROL2>"
	      << "<CONTROL3>" << (unsigned int)vfat->getControl3() << "</CONTROL3>"
	      << "</RAWVFAT>" << std::endl ;;
  }
};

/**Generates a CChip element
 * @param cchip - cchip description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLCChip(totemCChipDescription *cchip, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<CCHIP crateSlot=\"" << (unsigned int)cchip->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)cchip->getFecSlot()
	      << "\" fecHardwareId=\"" << cchip->getFecHardwareId()
	      << "\" enabled=\"" << (cchip->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)cchip->getRingSlot() 
	      << "\" ccuAddress=\"" << (unsigned int)cchip->getCcuAddress() 
	      << "\" i2cChannel=\"" << (unsigned int)cchip->getChannel() 
	      << "\" i2cAddress=\"" << (unsigned int)cchip->getAddress() 
	      << "\" cchipcontrol0=\"" << (unsigned int)cchip->getControl0()
	      << "\" cchipcontrol1=\"" << (unsigned int)cchip->getControl1()
	      << "\" cchipcontrol2=\"" << (unsigned int)cchip->getControl2()
	      << "\" cchipcontrol3=\"" << (unsigned int)cchip->getControl3()
	      << "\" cchipcontrol4=\"" << (unsigned int)cchip->getControl4()
	      << "\" cchipchipid0=\"" << (unsigned int)cchip->getChipid0()
	      << "\" cchipchipid1=\"" << (unsigned int)cchip->getChipid1()
	      << "\" cchipcounter0=\"" << (unsigned int)cchip->getCounter0()
	      << "\" cchipcounter1=\"" << (unsigned int)cchip->getCounter1()
	      << "\" cchipcounter2=\"" << (unsigned int)cchip->getCounter2()
	      << "\" cchipmask0=\"" << (unsigned int)cchip->getMask0()
	      << "\" cchipmask1=\"" << (unsigned int)cchip->getMask1()
	      << "\" cchipmask2=\"" << (unsigned int)cchip->getMask2()
	      << "\" cchipmask3=\"" << (unsigned int)cchip->getMask3()
	      << "\" cchipmask4=\"" << (unsigned int)cchip->getMask4()
	      << "\" cchipmask5=\"" << (unsigned int)cchip->getMask5()
	      << "\" cchipmask6=\"" << (unsigned int)cchip->getMask6()
	      << "\" cchipmask7=\"" << (unsigned int)cchip->getMask7()
	      << "\" cchipmask8=\"" << (unsigned int)cchip->getMask8()
	      << "\" cchipmask9=\"" << (unsigned int)cchip->getMask9()
	      << "\" cchipres=\"" << (unsigned int)cchip->getRes()
	      << "\" />" << std::endl ;
  } else {
    memBuffer << "<RAWCCHIP>"
	      << "<CRATESLOT>" << (unsigned int)cchip->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)cchip->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDWAREID>" << cchip->getFecHardwareId() << "</FECHARDWAREID>"
	      << "<ENABLED>" << (cchip->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>" 
	      << "<RINGSLOT>" << (unsigned int)cchip->getRingSlot() << "</RINGSLOT>"  
	      << "<CCUADDRESS>" << (unsigned int)cchip->getCcuAddress() << "</CCUADDRESS>"
	      << "<I2CCHANNEL>" << (unsigned int)cchip->getChannel() << "</I2CCHANNEL>"
	      << "<I2CADDRESS>" << (unsigned int)cchip->getAddress() << "</I2CADDRESS>"
	      << "<CCHIPCONTROL0>" << (unsigned int)cchip->getControl0() << "</CCHIPCONTROL0>"
	      << "<CCHIPCONTROL1>" << (unsigned int)cchip->getControl1() << "</CCHIPCONTROL1>"
	      << "<CCHIPCONTROL2>" << (unsigned int)cchip->getControl2() << "</CCHIPCONTROL0>"
	      << "<CCHIPCONTROL3>" << (unsigned int)cchip->getControl3() << "</CCHIPCONTROL1>"
	      << "<CCHIPCONTROL4>" << (unsigned int)cchip->getControl4() << "</CCHIPCONTROL1>"
	      << "<CCHIPCHIPID0>" << (unsigned int)cchip->getChipid0() << "</CCHIPCHIPID0>"
	      << "<CCHIPCHIPID1>" << (unsigned int)cchip->getChipid1() << "</CCHIPCHIPID1>"
	      << "<CCHIPCOUNTER0>" << (unsigned int)cchip->getCounter0() << "</CCHIPCOUNTER0>"
	      << "<CCHIPCOUNTER1>" << (unsigned int)cchip->getCounter1() << "</CCHIPCOUNTER1>"
	      << "<CCHIPCOUNTER2>" << (unsigned int)cchip->getCounter2() << "</CCHIPCOUNTER2>"
	      << "<CCHIPMASK0>" << (unsigned int)cchip->getMask0() << "</CCHIPMASK0>"
	      << "<CCHIPMASK1>" << (unsigned int)cchip->getMask1() << "</CCHIPMASK1>"
	      << "<CCHIPMASK2>" << (unsigned int)cchip->getMask2() << "</CCHIPMASK2>"
	      << "<CCHIPMASK3>" << (unsigned int)cchip->getMask3() << "</CCHIPMASK3>"
	      << "<CCHIPMASK4>" << (unsigned int)cchip->getMask4() << "</CCHIPMASK4>"
	      << "<CCHIPMASK5>" << (unsigned int)cchip->getMask5() << "</CCHIPMASK5>"
	      << "<CCHIPMASK6>" << (unsigned int)cchip->getMask6() << "</CCHIPMASK6>"
	      << "<CCHIPMASK7>" << (unsigned int)cchip->getMask7() << "</CCHIPMASK7>"
	      << "<CCHIPMASK8>" << (unsigned int)cchip->getMask8() << "</CCHIPMASK8>"
	      << "<CCHIPMASK9>" << (unsigned int)cchip->getMask9() << "</CCHIPMASK9>"
	      << "<CCHIPRES>" << (unsigned int)cchip->getMask9() << "</CCHIPRES>"
	      << "</RAWCCHIP>" << std::endl ;;
  }
};

/**Generates a TBB element
 * @param tbb - TBB description
 * @param memBuffer - XML buffer to fill
 */
void MemBufOutputSource::generateXMLTbb(totemBBDescription *tbb, std::stringstream &memBuffer, bool forDb) {
  if (!forDb) {
    memBuffer << "\t<TBB crateSlot=\"" << (unsigned int)tbb->getCrateId()
	      << "\" fecSlot=\"" << (unsigned int)tbb->getFecSlot()
	      << "\" fecHardwareId=\"" << tbb->getFecHardwareId()
	      << "\" enabled=\"" << (tbb->getEnabled() ? STRTRUE : STRFALSE)
	      << "\" ringSlot=\"" << (unsigned int)tbb->getRingSlot() 
	      << "\" ccuAddress=\"" << (unsigned int)tbb->getCcuAddress() 
	      << "\" i2cChannel=\"" << (unsigned int)tbb->getChannel() 
	      << "\" i2cAddress=\"" << (unsigned int)tbb->getAddress() 
	      << "\" tbbcontrol=\"" << (unsigned int)tbb->getControl()
	      << "\" tbblatency1=\"" << (unsigned int)tbb->getLatency1()
	      << "\" tbblatency2=\"" << (unsigned int)tbb->getLatency2()
	      << "\" tbbrepeats1=\"" << (unsigned int)tbb->getRepeats1()
	      << "\" tbbrepeats2=\"" << (unsigned int)tbb->getRepeats2()
	      << "\" />" << std::endl ;
  } else {
    memBuffer << "<RAWTBB>"
	      << "<CRATESLOT>" << (unsigned int)tbb->getCrateId() << "</CRATESLOT>"
	      << "<FECSLOT>" << (unsigned int)tbb->getFecSlot() << "</FECSLOT>"
	      << "<FECHARDWAREID>" << tbb->getFecHardwareId() << "</FECHARDWAREID>"
	      << "<ENABLED>" << (tbb->getEnabled() ? STRTRUE : STRFALSE) << "</ENABLED>" 
	      << "<RINGSLOT>" << (unsigned int)tbb->getRingSlot() << "</RINGSLOT>"  
	      << "<CCUADDRESS>" << (unsigned int)tbb->getCcuAddress() << "</CCUADDRESS>"
	      << "<I2CCHANNEL>" << (unsigned int)tbb->getChannel() << "</I2CCHANNEL>"
	      << "<I2CADDRESS>" << (unsigned int)tbb->getAddress() << "</I2CADDRESS>"
	      << "<TBBCONTROL>" << (unsigned int)tbb->getControl() << "</TBBCONTROL>"
	      << "<TBBLATENCY1>" << (unsigned int)tbb->getLatency1() << "</TBBLATENCY1>"
	      << "<TBBLATENCY2>" << (unsigned int)tbb->getLatency2() << "</TBBLATENCY2>"
	      << "<TBBREPEATS1>" << (unsigned int)tbb->getRepeats1() << "</TBBREPEATS1>"
	      << "<TBBREPEATS2>" << (unsigned int)tbb->getRepeats2() << "</TBBREPEATS2>"
	      << "</RAWTBB>" << std::endl ;;
  }
};
#endif // TOTEM
