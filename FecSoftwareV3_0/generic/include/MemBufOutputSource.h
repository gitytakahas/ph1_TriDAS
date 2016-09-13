/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#ifndef MEMBUFOUTPUTSOURCE_H
#define MEMBUFOUTPUTSOURCE_H

// declaration of the type XMLByte *
#include <xercesc/util/PlatformUtils.hpp>

// declaraction of the types deviceVector and piaResetVector, and description for each device
#include "apvDescription.h"
#include "pllDescription.h"
#include "laserdriverDescription.h"
#include "muxDescription.h"
#include "philipsDescription.h"
#include "dcuDescription.h"
#include "CCUDescription.h"
#include "piaResetDescription.h"
#include "TkDcuConversionFactors.h"
#include "TkDcuInfo.h"
#include "TkDcuPsuMap.h"
#include "CommissioningAnalysisDescription.h"
#include "ApvLatencyAnalysisDescription.h"
#include "CalibrationAnalysisDescription.h"
#include "FastFedCablingAnalysisDescription.h"
#include "FineDelayAnalysisDescription.h"
#include "OptoScanAnalysisDescription.h"
#include "PedestalsAnalysisDescription.h"
#include "TimingAnalysisDescription.h"
#include "VpspScanAnalysisDescription.h"

#ifdef PRESHOWER
#include "deltaDescription.h"
#include "kchipDescription.h"
#include "paceDescription.h"
#include "gohDescription.h"
#include "esMbResetDescription.h"
#endif // PRESHOWER
#ifdef TOTEM
#include "vfatDescription.h"
#include "totemCChipDescription.h"
#include "totemBBDescription.h"
#endif // TOTEM

#include "deviceType.h"
#include "TkRingDescription.h"

// declaration of the type stringstream
#include <sstream>

#define COMMON_XML_SCHEME "http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/schema.xsd"
#define DCUCONVERSION_XML_SCHEME "http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/conversionSchema.xsd"

/** \brief This class is implemented to generate a character buffer from a device vector.
 */
class MemBufOutputSource {
 protected:
  /**String stream containing the XML buffer
   */
  std::stringstream memBuffer_;
  std::stringstream memBufferApv25_, memBufferApvMux_, memBufferDcu_, memBufferLaserdriver_, memBufferPhilips_, memBufferPll_, memBufferPiaReset_ ;

#ifdef TOTEM
  std::stringstream memBufferVfat_, memBufferCChip_, memBufferTbb_ ;
#endif // TOTEM

#ifdef PRESHOWER
  std::stringstream memBufferKchip_, memBufferDelta_, memBufferPace_, memBufferGoh_ ;
#endif //PRESHOWER

 public:

  /**Generates a header "<?xml version = \"1.0\"?>"
   */
  void generateHeader();

  /**Generates a start tag "<ROWSET>"
   */
  void generateStartTag(std::string);

  /**Generates a end tag "</ROWSET>"
   */
  void generateEndTag();

  /**Write on <I>memBuffer_</I> attribute device information
   */
  virtual void generateDeviceTag(deviceVector deviceParameters, bool forDb=false) throw (FecExceptionHandler);

  /**Write on <I>memConnection_</I> attribut connection information
   */
  void generateConnectionTag(ConnectionVector connections, bool forDb=false) throw (FecExceptionHandler);

  /**Write on <I>memBufferCcu_</I> attribute ccu information
   */
  void generateCcuTag(ccuVector, bool forDb=false) throw (FecExceptionHandler);

  /**Write on <I>memBufferTkRing_</I> attribute ring information
   */
  void generateTkRingTag(TkRingDescription ringParameters, bool forDb ) throw (FecExceptionHandler);

  /**Write on <I>memBuffer_</I> attribute piaReset information
   */
  void generatePiaResetTag(piaResetVector piaResetParameters, bool forDb=false) throw (FecExceptionHandler);

  /**Write on <I>memBuffer_</I> attribute dcuConversion information
   */
  void generateTkDcuConversionTag(dcuConversionVector dcuConversionParameters, bool forDb=false) throw (FecExceptionHandler);

  /**Write on <I>memBuffer_</I> attribute dcuConversion information
   */
  void generateTkIdVsHostnameTag( TkIdVsHostnameVector tkIdVsHostnameVector, bool forDb=false) throw (FecExceptionHandler);

  /**Write on <I>memBuffer_</I> attribute analysis information
   */
  void generateAnalysisTag( std::vector<CommissioningAnalysisDescription*> &inVector, CommissioningAnalysisDescription::commissioningType analysisType, bool forDb ) throw (FecExceptionHandler);

  /**Generates a DcuConversion element
   */
  void generateXMLTkDcuConversion(TkDcuConversionFactors *dcuConversion, std::stringstream &memBuffer, bool forDb=false);

  /**Generates a IdVsHostname element
   */
  void generateXMLTkIdVsHostname(TkIdVsHostnameDescription *hostnameId, std::stringstream &memBuffer, bool forDb=false);

  /**Generates a XML commissionsing analysis element
   */
  void generateXMLAnalysisTag(CommissioningAnalysisDescription *description, std::stringstream &memBuffer, CommissioningAnalysisDescription::commissioningType analysisType, bool forDb=false);

  /**Write on <I>memBuffer_</I> attribute dcuInfo information
   */
  void generateTkDcuInfoTag(tkDcuInfoVector dcuInfoParameters, bool forDb=false) throw (FecExceptionHandler);

  /**Generates a DcuInfo element
   */
  void generateXMLTkDcuInfo(TkDcuInfo *dcuInfo, std::stringstream &memBuffer, bool forDb=false);

  /**Write on <I>memBuffer_</I> attribute dcuPsuMap information
   */
  void generateTkDcuPsuMapTag(tkDcuPsuMapVector dcuPsuMapParameters, bool forDb=false) throw (FecExceptionHandler);

  /**Generates a DcuPsuMap element
   */
  void generateXMLTkDcuPsuMap(TkDcuPsuMap *dcuInfo, std::stringstream &memBuffer, bool forDb=false);

  /**Generates a PiaReset element
   */
  void generateXMLPiaReset(piaResetDescription *, std::stringstream &, bool forDb=false);

  /**Generates a Pll element
   */
  void generateXMLPll(pllDescription *, std::stringstream &, bool forDb=false);

  /**Generates a Ccu element
   */
  void generateXMLCcu(CCUDescription *ccu, std::stringstream &memBuffer, bool forDb=false);

  /**Generates a Ring element
   */
  void generateXMLTkRing(TkRingDescription tkRing, std::stringstream &memBuffer, bool forDb);

  
  /**************************************** Berta 05-10-06 *******************************************************/

   /**Generates a Ccu element
   */
  void generateXMLCcu(CCUDescription *ccu, bool forDb=false);

  /**************************************** Berta 05-10-06 *******************************************************/

  /**Generates a Laserdriver element
   */
  void generateXMLLaserdriver(laserdriverDescription *, std::stringstream &, bool forDb=false);

  /**Generates an Apv25 element
   */
  void generateXMLApv25(apvDescription *, std::stringstream &, bool forDb=false);

  /**Generates an ApvMux element
   */
  void generateXMLApvMux(muxDescription *, std::stringstream &, bool forDb=false);

  /**Generates a Philips element
   */
  void generateXMLPhilips(philipsDescription *, std::stringstream &, bool forDb=false);

  /**Generates a DCU element
   */
  void generateXMLDcu(dcuDescription *, std::stringstream &, bool forDb=false);

#ifdef PRESHOWER
  /**Generates a DELTA element
   */
  void generateXMLDelta(deltaDescription *, std::stringstream &);

  /**Generates a PACEAM element
   */
  void generateXMLPace(paceDescription *, std::stringstream &);

  /**Generates a KCHIP element
   */
  void generateXMLKchip(kchipDescription *, std::stringstream &);

  /**Generates a GOH element
   */
  void generateXMLGoh(gohDescription *, std::stringstream &);

  /**Generates a ESMBRESET element
   */
  void generateXMLEsMbReset(esMbResetDescription *, std::stringstream &);

#endif // PRESHOWER

#ifdef TOTEM
  /**Generates a Vfat element
   */
  void generateXMLVfat(vfatDescription *, std::stringstream &, bool forDb=false);

  /**Generates a CChip element
   */
  void generateXMLCChip(totemCChipDescription *, std::stringstream &, bool forDb=false);

  /**Generates a Tbb element
   */
  void generateXMLTbb(totemBBDescription *, std::stringstream &, bool forDb=false);
#endif // TOTEM

  /** \brief Default constructor
   */
  MemBufOutputSource();

  /** \brief Create a MemBufOuputSource object from a ccuVector
   */
  MemBufOutputSource(ccuVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a TkRingDescription
   *  \param withCcus bool parameter default value is false
   */
  MemBufOutputSource (TkRingDescription, bool forDb, bool withCcus ) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a vector of TkRingDescription
   *  \param withCcus bool parameter default value is false
   */
  MemBufOutputSource (tkringVector rings, bool forDb, bool withCcus) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a deviceVector
   */
  MemBufOutputSource(deviceVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a deviceVector
   */
  MemBufOutputSource(ConnectionVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a piaResetVector
   */
  MemBufOutputSource(piaResetVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a dcuConversionVector
   */
  MemBufOutputSource (dcuConversionVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a dcuConversionVector
   */
  MemBufOutputSource (TkIdVsHostnameVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a vector of commissioning analysis
   */
  MemBufOutputSource (std::vector<CommissioningAnalysisDescription*> &inVector, CommissioningAnalysisDescription::commissioningType analysisType, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a tkDcuInfoVector
   */
  MemBufOutputSource (tkDcuInfoVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a tkDcuPsuMapVector
   */
  MemBufOutputSource (tkDcuPsuMapVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a deviceVector and a piaResetVector
   */
  MemBufOutputSource(deviceVector, piaResetVector) throw (FecExceptionHandler);

  /** \brief Destructor
   */
  virtual ~MemBufOutputSource();

  /** \brief Return the generated XML buffer
   */
  std::stringstream *getOutputBuffer();

  /** \brief Return the generated Pll XML buffer
   */
  std::stringstream *getPllOutputBuffer();

  /** \brief Return the generated Laserdriver XML buffer
   */
  std::stringstream *getLaserdriverOutputBuffer();

  /** \brief Return the generated ApvFec XML buffer
   */
  std::stringstream *getApvFecOutputBuffer();

  /** \brief Return the generated ApvMux XML buffer
   */
  std::stringstream *getApvMuxOutputBuffer();

  /** \brief Return the generated Philips XML buffer
   */
  std::stringstream *getPhilipsOutputBuffer();

  /** \brief Return the generated Dcu XML buffer
   */
  std::stringstream *getDcuOutputBuffer();

  /** \brief Return the generated Dcu XML buffer
   */
  std::stringstream *getPiaResetOutputBuffer();

#ifdef TOTEM
  /** \brief Return the generated Vfat XML buffer
   */
  std::stringstream *getVfatOutputBuffer();

  /** \brief Return the generated CChip XML buffer
   */
  std::stringstream *getCChipOutputBuffer();

  /** \brief Return the generated Tbb XML buffer
   */
  std::stringstream *getTbbOutputBuffer();
#endif // TOTEM

#ifdef PRESHOWER
  /** \brief Return the generated Kchip XML buffer
   */
  std::stringstream *getKchipOutputBuffer();
	
  /** \brief Return the generated Delta XML buffer
   */
  std::stringstream *getDeltaOutputBuffer();
	
  /** \brief Return the generated Pace XML buffer
   */
  std::stringstream *getPaceOutputBuffer();

  /** \brief Return the generated Goh XML buffer
   */
  std::stringstream *getGohOutputBuffer();
#endif //PRESHOWER
  
  /** ??????
   */
  std::string getProcessIdentifier();
};

#endif
