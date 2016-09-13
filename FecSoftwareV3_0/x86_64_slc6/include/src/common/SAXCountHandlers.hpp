/*
 * Copyright 1999-2000,2004 The Apache Software Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Log: SAXCountHandlers.hpp,v $
 * Revision 1.7  2008/05/23 09:30:18  fdrouhin
 * change crateSlot naming by crateId, setFecHardwareId method has change in order to have also the crate ID included
 *
 * Revision 1.6  2007/08/22 08:35:22  fdrouhin
 * Porting to XDAQ 3.11: All Makefile and Strip Tracker XDAQ applications have been changed accordingly
 *
 * Revision 1.5  2007/08/16 16:38:44  fdrouhin
 * New database integration for SiTk applications
 *
 * Revision 1.3  2007/02/02 15:15:26  fdrouhin
 * Some problem in DeviceFactoryTemplate and Makefile from generic
 *
 * Revision 1.2  2007/01/26 17:45:10  fdrouhin
 * Upgrade of the LATENCY definition in apvDefinition.h and all the sub-classes
 *
 * Revision 1.1  2006/12/12 16:07:13  fdrouhin
 * Before migration under XDAQ 3.7
 *
 * Revision 1.9  2004/09/08 13:55:34  peiyongz
 * Apache License Version 2.0
 *
 * Revision 1.8  2004/02/15 19:43:15  amassari
 * Removed cause for warnings in VC 7.1
 *
 * Revision 1.7  2002/11/05 21:46:20  tng
 * Explicit code using namespace in application.
 *
 * Revision 1.6  2002/11/04 15:23:03  tng
 * C++ Namespace Support.
 *
 * Revision 1.5  2002/02/01 22:41:07  peiyongz
 * sane_include
 *
 * Revision 1.4  2001/08/02 17:10:29  tng
 * Allow DOMCount/SAXCount/IDOMCount/SAX2Count to take a file that has a list of xml file as input.
 *
 * Revision 1.3  2000/03/02 19:53:47  roddey
 * This checkin includes many changes done while waiting for the
 * 1.1.0 code to be finished. I can't list them all here, but a list is
 * available elsewhere.
 *
 * Revision 1.2  2000/02/06 07:47:23  rahulj
 * Year 2K copyright swat.
 *
 * Revision 1.1.1.1  1999/11/09 01:09:32  twl
 * Initial checkin
 *
 * Revision 1.7  1999/11/08 20:43:41  rahul
 * Swat for adding in Product name and CVS comment log variable.
 *
 */


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/sax/HandlerBase.hpp>
#include "deviceDescription.h"

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class AttributeList;
XERCES_CPP_NAMESPACE_END

//#define NEWPARSER

class SAXCountHandlers : public HandlerBase
{
public:
  // -----------------------------------------------------------------------
  //  Constructors and Destructor
  // -----------------------------------------------------------------------
  SAXCountHandlers();
  ~SAXCountHandlers();


  // -----------------------------------------------------------------------
  //  Getter methods
  // -----------------------------------------------------------------------
  unsigned int getElementCount() const
  {
    return fElementCount;
  }

  unsigned int getAttrCount() const
  {
    return fAttrCount;
  }

  unsigned int getCharacterCount() const
  {
    return fCharacterCount;
  }

  bool getSawErrors() const
  {
    return fSawErrors;
  }

  unsigned int getSpaceCount() const
  {
    return fSpaceCount;
  }


  // -----------------------------------------------------------------------
  //  Handlers for the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  void startElement(const XMLCh* const name, AttributeList& attributes);
  void characters(const XMLCh* const chars, const unsigned int length);
  void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
  void resetDocument();


  // -----------------------------------------------------------------------
  // Parsers
  // -----------------------------------------------------------------------
  void parseAttributesPiaReset    (AttributeList&  attributes) ;
  void parseAttributesApv25       (AttributeList&  attributes) ;
  void parseAttributesApvMux      (AttributeList&  attributes) ;
  void parseAttributesDcu         (AttributeList&  attributes) ;
  void parseAttributesLaserdriver (AttributeList&  attributes) ;
  void parseAttributesPll         (AttributeList&  attributes) ;
  unsigned int parseAttributes(parameterDescriptionNameType *parameterNames, AttributeList& Attributes) ;

  // -----------------------------------------------------------------------
  //  Handlers for the SAX ErrorHandler interface
  // -----------------------------------------------------------------------
  void warning(const SAXParseException& exc);
  void error(const SAXParseException& exc);
  void fatalError(const SAXParseException& exc);
  void resetErrors();

  // -----------------------------------------------------------------------
  //  Counters
  // -----------------------------------------------------------------------
  int getCountPIA () {
    return countPIA ;
  }
  int getCountAPV25 () {
    return countAPV25 ;
  }
  int getCountAPVMUX() {
    return countAPVMUX ;
  }
  int getCountDCUFEH() {
    return countDCUFEH ;
  }
  int getCountDCUCCU() {
    return countDCUCCU ;
  }
  int getCountAOH() {
    return countAOH ;
  }
  int getCountDOH() {
    return countDOH ;
  }
  int getCountPLL() {
    return countPLL ;
  }

private:
  // -----------------------------------------------------------------------
  //  Private data members
  //
  //  fAttrCount
  //  fCharacterCount
  //  fElementCount
  //  fSpaceCount
  //      These are just counters that are run upwards based on the input
  //      from the document handlers.
  //
  //  fSawErrors
  //      This is set by the error handlers, and is queryable later to
  //      see if any errors occured.
  // -----------------------------------------------------------------------
  unsigned int    fAttrCount;
  unsigned int    fCharacterCount;
  unsigned int    fElementCount;
  unsigned int    fSpaceCount;
  bool            fSawErrors;

  // For parsing of the attributes
  XMLCh* rowsetXML_ ;

  XMLCh* piaResetXML_ ;
  XMLCh* apv25XML_ ;
  XMLCh* muxXML_ ;
  XMLCh* dcuXML_ ;
  XMLCh* laserdriverXML_ ;
  XMLCh* pllXML_ ;

  XMLCh* fecHardIdXML_ ;
  XMLCh* crateSlotXML_ ;
  XMLCh* fecSlotXML_ ;
  XMLCh* ringSlotXML_ ;
  XMLCh* ccuAddressXML_ ;
  XMLCh* i2cChannelXML_ ;
  XMLCh* i2cAddressXML_ ;
  XMLCh* enabledXML_ ;
  XMLCh* piaCrateSlotXML_ ;
  XMLCh* piaFecSlotXML_ ;
  XMLCh* piaRingSlotXML_ ;
  XMLCh* piaCcuAddressXML_ ;
  XMLCh* piaChannelXML_ ;
  XMLCh* delayActiveResetXML_ ;
  XMLCh* intervalDelayResetXML_ ;
  XMLCh* maskXML_ ;
  XMLCh* apvModeXML_ ;
  XMLCh* latencyXML_ ;
  XMLCh* muxGainXML_ ;
  XMLCh* ipreXML_ ;
  XMLCh* ipcascXML_ ;
  XMLCh* ipsfXML_ ;
  XMLCh* ishaXML_ ;
  XMLCh* issfXML_ ;
  XMLCh* ipspXML_ ;
  XMLCh* imuxinXML_ ;
  XMLCh* icalXML_ ;
  XMLCh* ispareXML_ ;
  XMLCh* vfpXML_ ;
  XMLCh* vfsXML_ ;
  XMLCh* vpspXML_ ;
  XMLCh* cdrvXML_ ;
  XMLCh* cselXML_ ;
  XMLCh* apvErrorXML_ ;
  XMLCh* resistorXML_ ;
  XMLCh* timestampXML_ ;
  XMLCh* dcuhardidXML_ ;
  XMLCh* channel0XML_ ;
  XMLCh* channel1XML_ ;
  XMLCh* channel2XML_ ;
  XMLCh* channel3XML_ ;
  XMLCh* channel4XML_ ;
  XMLCh* channel5XML_ ;
  XMLCh* channel6XML_ ;
  XMLCh* channel7XML_ ;
  XMLCh* eDcuTypeXML_ ;
  XMLCh* dcuReadoutEnabledXML_ ;
  XMLCh* bias0XML_ ;
  XMLCh* bias1XML_ ;
  XMLCh* bias2XML_ ;
  XMLCh* gain0XML_ ;
  XMLCh* gain1XML_ ;
  XMLCh* gain2XML_ ;
  XMLCh* delayFineXML_ ;
  XMLCh* delayCoarseXML_ ;

  int countPLL, countAPV25, countDCUFEH, countDCUCCU, countAPVMUX, countPIA, countAOH, countDOH ;
};

#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>

#include "FecExceptionHandler.h"

#include "keyType.h"

#include "piaResetDescription.h"
#include "apvDescription.h"
#include "muxDescription.h"
#include "dcuDescription.h"
#include "laserdriverDescription.h"
#include "pllDescription.h"

// ---------------------------------------------------------------------------
//  SAXCountHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
SAXCountHandlers::SAXCountHandlers() :

    fAttrCount(0)
    , fCharacterCount(0)
    , fElementCount(0)
    , fSpaceCount(0)
    , fSawErrors(false)
    , countPLL(0), countAPV25(0), countDCUFEH(0), countDCUCCU(0), countAPVMUX(0), countPIA(0), countAOH(0), countDOH(0)
{
  rowsetXML_ = XMLString::transcode("ROWSET") ;

  piaResetXML_ = XMLString::transcode("PIARESET") ;
  apv25XML_ = XMLString::transcode("APV25") ;
  muxXML_ = XMLString::transcode("APVMUX") ;
  dcuXML_ = XMLString::transcode("DCU") ;
  laserdriverXML_ = XMLString::transcode("LASERDRIVER") ;
  pllXML_ = XMLString::transcode("PLL") ;

  fecHardIdXML_ = XMLString::transcode(deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID]) ;
  crateSlotXML_ = XMLString::transcode(deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID]) ;
  fecSlotXML_ = XMLString::transcode(deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT]) ;
  ringSlotXML_ = XMLString::transcode(deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT]) ;
  ccuAddressXML_ = XMLString::transcode(deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS]) ;
  i2cChannelXML_ = XMLString::transcode(deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL]) ;
  i2cAddressXML_ = XMLString::transcode(deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS]) ;
  enabledXML_ = XMLString::transcode(deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED]) ;

  piaCrateSlotXML_ = XMLString::transcode(piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACRATEID]) ;
  piaFecSlotXML_ = XMLString::transcode(piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIAFECSLOT]) ;
  piaRingSlotXML_ = XMLString::transcode(piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIARINGSLOT]) ;
  piaCcuAddressXML_ = XMLString::transcode(piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACCUADDRESS]) ;
  piaChannelXML_ = XMLString::transcode(piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACHANNEL]) ;
  delayActiveResetXML_ = XMLString::transcode(piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::DELAYACTIVERESET]) ;
  intervalDelayResetXML_ = XMLString::transcode(piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::INTERVALDELAYRESET]) ;
  maskXML_ = XMLString::transcode(piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::MASK]) ;

  apvModeXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::APVMODE]) ;
  latencyXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::APVLATENCY]) ;
  muxGainXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::MUXGAIN]) ;
  ipreXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::IPRE]) ;
  ipcascXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::IPCASC]) ;
  ipsfXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::IPSF]) ;
  ishaXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::ISHA]) ;
  issfXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::ISSF]) ;
  ipspXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::IPSP]) ;
  imuxinXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::IMUXIN]) ;
  icalXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::ICAL]) ;
  ispareXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::ISPARE]) ;
  vfpXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::VFP]) ;
  vfsXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::VFS]) ;
  vpspXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::VPSP]) ;
  cdrvXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::CDRV]) ;
  cselXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::CSEL]) ;
  apvErrorXML_ = XMLString::transcode(apvDescription::APVPARAMETERNAMES[apvDescription::APVERROR]) ;

  resistorXML_ = XMLString::transcode(muxDescription::MUXPARAMETERNAMES[muxDescription::RESISTOR]) ;

  timestampXML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUTIMESTAMP]) ;
  dcuhardidXML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUHARDID]) ;
  channel0XML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL0]) ;
  channel1XML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL1]) ;
  channel2XML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL2]) ;
  channel3XML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL3]) ;
  channel4XML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL4]) ;
  channel5XML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL5]) ;
  channel6XML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL6]) ;
  channel7XML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL7]) ;
  eDcuTypeXML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::EDCUTYPE]) ;
  dcuReadoutEnabledXML_ = XMLString::transcode(dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUREADOUTENABLED]) ;

  bias0XML_ = XMLString::transcode(laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS0]) ;
  bias1XML_ = XMLString::transcode(laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS1]) ;
  bias2XML_ = XMLString::transcode(laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS2]) ;
  gain0XML_ = XMLString::transcode(laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN0]) ;
  gain1XML_ = XMLString::transcode(laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN1]) ;
  gain2XML_ = XMLString::transcode(laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN2]) ;

  delayFineXML_ = XMLString::transcode(pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYFINE]) ;
  delayCoarseXML_ = XMLString::transcode(pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYCOARSE]) ;
}

SAXCountHandlers::~SAXCountHandlers()
{
  XMLString::release(&rowsetXML_) ;

  XMLString::release(&piaResetXML_) ;
  XMLString::release(&apv25XML_);
  XMLString::release(&muxXML_);
  XMLString::release(&dcuXML_);
  XMLString::release(&laserdriverXML_);
  XMLString::release(&pllXML_);

  XMLString::release(&fecHardIdXML_) ;
  XMLString::release(&crateSlotXML_) ;
  XMLString::release(&fecSlotXML_) ;
  XMLString::release(&ringSlotXML_) ;
  XMLString::release(&ccuAddressXML_) ;
  XMLString::release(&i2cChannelXML_) ;
  XMLString::release(&i2cAddressXML_) ;
  XMLString::release(&enabledXML_) ;
  XMLString::release(&piaCrateSlotXML_) ;
  XMLString::release(&piaFecSlotXML_) ;
  XMLString::release(&piaRingSlotXML_) ;
  XMLString::release(&piaCcuAddressXML_) ;
  XMLString::release(&piaChannelXML_) ;
  XMLString::release(&delayActiveResetXML_) ;
  XMLString::release(&intervalDelayResetXML_) ;
  XMLString::release(&maskXML_) ;
  XMLString::release(&apvModeXML_) ;
  XMLString::release(&latencyXML_) ;
  XMLString::release(&muxGainXML_) ;
  XMLString::release(&ipreXML_) ;
  XMLString::release(&ipcascXML_) ;
  XMLString::release(&ipsfXML_) ;
  XMLString::release(&ishaXML_) ;
  XMLString::release(&issfXML_) ;
  XMLString::release(&ipspXML_) ;
  XMLString::release(&imuxinXML_) ;
  XMLString::release(&icalXML_) ;
  XMLString::release(&ispareXML_) ;
  XMLString::release(&vfpXML_) ;
  XMLString::release(&vfsXML_) ;
  XMLString::release(&vpspXML_) ;
  XMLString::release(&cdrvXML_) ;
  XMLString::release(&cselXML_) ;
  XMLString::release(&apvErrorXML_) ;
  XMLString::release(&resistorXML_) ;
  XMLString::release(&timestampXML_) ;
  XMLString::release(&dcuhardidXML_) ;
  XMLString::release(&channel0XML_) ;
  XMLString::release(&channel1XML_) ;
  XMLString::release(&channel2XML_) ;
  XMLString::release(&channel3XML_) ;
  XMLString::release(&channel4XML_) ;
  XMLString::release(&channel5XML_) ;
  XMLString::release(&channel6XML_) ;
  XMLString::release(&channel7XML_) ;
  XMLString::release(&eDcuTypeXML_) ;
  XMLString::release(&dcuReadoutEnabledXML_) ;
  XMLString::release(&bias0XML_) ;
  XMLString::release(&bias1XML_) ;
  XMLString::release(&bias2XML_) ;
  XMLString::release(&gain0XML_) ;
  XMLString::release(&gain1XML_) ;
  XMLString::release(&gain2XML_) ;
  XMLString::release(&delayFineXML_) ;
  XMLString::release(&delayCoarseXML_) ;
}


// ---------------------------------------------------------------------------
//  SAXCountHandlers: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void SAXCountHandlers::startElement(const   XMLCh* const name
                                    ,       AttributeList&  attributes)
{
#ifndef NEWPARSER
  unsigned int i ;
#endif

  if (XMLString::equals(name,piaResetXML_)) {
#ifdef NEWPARSER
    parseAttributesPiaReset(attributes) ;
#else
    parameterDescriptionNameType *parameterNames = piaResetDescription::getParameterNames() ;
    i = parseAttributes(parameterNames, attributes);
    piaResetDescription::deleteParameterNames(parameterNames);

    countPIA ++ ;
#endif
  }
  else if (XMLString::equals(name,apv25XML_)) {
#ifdef NEWPARSER
    parseAttributesApv25(attributes) ;
#else
    parameterDescriptionNameType *parameterNames = apvDescription::getParameterNames() ;
    i = parseAttributes(parameterNames, attributes);
    apvDescription::deleteParameterNames(parameterNames);

    countAPV25 ++ ;
#endif
  }
  else if (XMLString::equals(name,muxXML_)) {
#ifdef NEWPARSER
    parseAttributesApvMux(attributes) ;
#else
    parameterDescriptionNameType *parameterNames = muxDescription::getParameterNames() ;
    i = parseAttributes(parameterNames, attributes);
    muxDescription::deleteParameterNames(parameterNames);

    countAPVMUX++ ;
#endif
  }
  else if (XMLString::equals(name,dcuXML_)) {
#ifdef NEWPARSER
    parseAttributesDcu(attributes) ;
#else
    parameterDescriptionNameType *parameterNames = dcuDescription::getParameterNames() ;
    i = parseAttributes(parameterNames, attributes);
    dcuDescription::deleteParameterNames(parameterNames);

    countDCUFEH ++ ;
#endif
  }
  else if (XMLString::equals(name,laserdriverXML_)) {
#ifdef NEWPARSER
    parseAttributesLaserdriver(attributes) ;
#else
    parameterDescriptionNameType *parameterNames = laserdriverDescription::getParameterNames() ;
    i = parseAttributes(parameterNames, attributes);
    laserdriverDescription::deleteParameterNames(parameterNames);

    countAOH ++ ;
#endif
  }
  else if (XMLString::equals(name,pllXML_)) {
#ifdef NEWPARSER
    parseAttributesPll(attributes) ;
#else
    parameterDescriptionNameType *parameterNames = pllDescription::getParameterNames() ;
    i = parseAttributes(parameterNames, attributes);
    pllDescription::deleteParameterNames(parameterNames);

    countPLL ++ ;
#endif
  }
  else if (XMLString::equals(name,rowsetXML_)) {
    // nothing
  }
  else {
    char *nameChar = XMLString::transcode(name) ;
    std::cerr << "Invalid tag " << nameChar << std::endl ;
    XMLString::release(&nameChar) ;
  }

  fElementCount++;
  fAttrCount += attributes.getLength();
}

void SAXCountHandlers::parseAttributesPiaReset (AttributeList&  attributes) {

  countPIA ++ ;

  unsigned int val = 0 ;
  char fecHardwareId[100] ;
  bool enabled = true ;
  int fec = 0,ring = 0,ccu = 0,channel = 0 ;
  unsigned long delayActiveReset = 0, intervalDelayReset = 0, mask = 0 ;

  for (unsigned int i=0; i<attributes.getLength(); i++){
    if (XMLString::equals(attributes.getName(i),fecHardIdXML_)) {
      char *fecHardId = XMLString::transcode(attributes.getValue(i)) ;
      strncpy (fecHardwareId,fecHardId,100) ;
      XMLString::release(&fecHardId) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),piaCrateSlotXML_)) {
      fec = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),piaFecSlotXML_)) {
      fec = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),piaRingSlotXML_)) {
      ring = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),piaCcuAddressXML_)) {
      ccu = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),piaChannelXML_)) {
      channel = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),enabledXML_)) {
      char *enabledChar = XMLString::transcode(attributes.getValue(i)) ;
      if (enabledChar[0] == 'F') enabled = false ;
      XMLString::release(&enabledChar) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),delayActiveResetXML_)) {
      char *value = XMLString::transcode(attributes.getValue(i)) ;
      if (sscanf(value,"%ld",&delayActiveReset)) val ++ ;
      XMLString::release(&value) ;
    }
    else if (XMLString::equals(attributes.getName(i),intervalDelayResetXML_)) {
      char *value = XMLString::transcode(attributes.getValue(i)) ;
      if (sscanf(value,"%ld",&intervalDelayReset)) val ++ ;
      XMLString::release(&value) ;
    }
    else if (XMLString::equals(attributes.getName(i),maskXML_)) {
      mask = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
  }

  if (val != 9) std::cerr << "PIA reset: invalid number of parameters: " << val << "/9" << std::endl ;

  piaResetDescription *piaDevice = new piaResetDescription(buildCompleteKey(fec,ring,ccu,channel,0),
							   delayActiveReset, intervalDelayReset, mask) ;
  piaDevice->setEnabled(enabled) ;
  //piaVector->push_back(piaDevice) ;
}

void SAXCountHandlers::parseAttributesApv25 (AttributeList&  attributes) {

  countAPV25 ++ ;

  unsigned int val = 0 ;
  char fecHardwareId[100] ;
  bool enabled = true ;
  int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
  int apvMode = 0, latency = 0, muxGain = 0, ipre = 0, ipcasc = 0, ipsf = 0 ;   // APV25
  int isha = 0, issf = 0, ipsp = 0, imuxin = 0, ical = 0, ispare = 0, vfp = 0 ; // APV25
  int vfs = 0, vpsp = 0, cdrv = 0, csel = 0, apvError = 0 ;                     // APV25

  for (unsigned int i=0; i<attributes.getLength(); i++){
    if (XMLString::equals(attributes.getName(i),fecHardIdXML_)) {
      char *fecHardId = XMLString::transcode(attributes.getValue(i)) ;
      strncpy (fecHardwareId,fecHardId,100) ;
      XMLString::release(&fecHardId) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),crateSlotXML_)) {
      crate = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),fecSlotXML_)) {
      fec = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ringSlotXML_)) {
      ring = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ccuAddressXML_)) {
      ccu = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cChannelXML_)) {
      channel = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cAddressXML_)) {
      address = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),enabledXML_)) {
      char *enabledChar = XMLString::transcode(attributes.getValue(i)) ;
      if (enabledChar[0] == 'F') enabled = false ;
      XMLString::release(&enabledChar) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),apvModeXML_)) {
      apvMode = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),latencyXML_)) {
      latency = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),muxGainXML_)) {
      muxGain = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ipreXML_)) {
      ipre = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ipcascXML_)) {
      ipcasc = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ipsfXML_)) {
      ipsf = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ishaXML_)) {
      isha = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),issfXML_)) {
      issf = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ipspXML_)) {
      ipsp = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),imuxinXML_)) {
      imuxin = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),icalXML_)) {
      ical = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ispareXML_)) {
      ispare = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),vfpXML_)) {
      vfp = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),vfsXML_)) {
      vfs = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),vpspXML_)) {
      vpsp = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),cdrvXML_)) {
      cdrv = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),cselXML_)) {
      csel = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),apvErrorXML_)) {
      apvError = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
  }

  if (val != 25) std::cerr << "APV25: invalid number of parameters: " << val << "/25" << std::endl ;

  apvDescription *device = new apvDescription(buildCompleteKey(fec,ring,ccu,channel,address),
					      apvMode, latency, muxGain, ipre, ipcasc, ipsf,
					      isha, issf, ipsp, imuxin, ical, ispare, vfp,
					      vfs, vpsp, cdrv, csel,apvError) ;
  device->setEnabled(enabled) ;
  //vDevices->push_back(device) ;
}

void SAXCountHandlers::parseAttributesApvMux (AttributeList&  attributes) {

  countAPVMUX ++ ;

  unsigned int val = 0 ;
  char fecHardwareId[100] ;
  bool enabled = true ;
  int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
  int resistor = 0 ;

  for (unsigned int i=0; i<attributes.getLength(); i++){
    if (XMLString::equals(attributes.getName(i),fecHardIdXML_)) {
      char *fecHardId = XMLString::transcode(attributes.getValue(i)) ;
      strncpy (fecHardwareId,fecHardId,100) ;
      XMLString::release(&fecHardId) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),crateSlotXML_)) {
      crate = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),fecSlotXML_)) {
      fec = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ringSlotXML_)) {
      ring = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ccuAddressXML_)) {
      ccu = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cChannelXML_)) {
      channel = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cAddressXML_)) {
      address = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),enabledXML_)) {
      char *enabledChar = XMLString::transcode(attributes.getValue(i)) ;
      if (enabledChar[0] == 'F') enabled = false ;
      XMLString::release(&enabledChar) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),resistorXML_)) {
      resistor = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
  }

  if (val != 8) std::cerr << "APVMUX: invalid number of parameters: " << val << "/8" << std::endl ;

  muxDescription *device = new muxDescription(buildCompleteKey(fec,ring,ccu,channel,address),resistor) ;
  device->setEnabled(enabled) ;
  //vDevices->push_back(device) ;
}

void SAXCountHandlers::parseAttributesDcu (AttributeList&  attributes) {

  unsigned int val = 0 ;
  char fecHardwareId[100] ;
  bool enabled = true ;
  int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
  unsigned int timestamp = 0, dcuHardId = 0 ;                                   // DCU
  int channel0 = 0, channel1 = 0, channel2 = 0, channel3 = 0, channel4 = 0 ;    // DCU
  int channel5 = 0, channel6 = 0, channel7 = 0 ;                                // DCU
  char dcuType[100] ;                                                           // DCU
  bool dcuEnabled = true ;                                                      // DCU

  for (unsigned int i=0; i<attributes.getLength(); i++){
    if (XMLString::equals(attributes.getName(i),fecHardIdXML_)) {
      char *fecHardId = XMLString::transcode(attributes.getValue(i)) ;
      strncpy (fecHardwareId,fecHardId,100) ;
      XMLString::release(&fecHardId) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),crateSlotXML_)) {
      crate = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),fecSlotXML_)) {
      fec = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ringSlotXML_)) {
      ring = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ccuAddressXML_)) {
      ccu = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cChannelXML_)) {
      channel = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cAddressXML_)) {
      address = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),enabledXML_)) {
      char *enabledChar = XMLString::transcode(attributes.getValue(i)) ;
      if (enabledChar[0] == 'F') enabled = false ;
      XMLString::release(&enabledChar) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),timestampXML_)) {
      char *timestampChar = XMLString::transcode(attributes.getValue(i)) ;
      if (sscanf(timestampChar,"%u",&timestamp)) val ++ ;
      XMLString::release(&timestampChar) ;
    }
    else if (XMLString::equals(attributes.getName(i),dcuhardidXML_)) {
      char *dcuHardIdChar = XMLString::transcode(attributes.getValue(i)) ;
      if (sscanf(dcuHardIdChar,"%u",&dcuHardId)) val ++ ;
      XMLString::release(&dcuHardIdChar) ;
    }
    else if (XMLString::equals(attributes.getName(i),channel0XML_)) {
      channel0 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),channel1XML_)) {
      channel1 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),channel2XML_)) {
      channel2 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),channel3XML_)) {
      channel3 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),channel4XML_)) {
      channel4 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),channel5XML_)) {
      channel5 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),channel6XML_)) {
      channel6 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),channel7XML_)) {
      channel7 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),eDcuTypeXML_)) {
      char *dcuTypeChar = XMLString::transcode(attributes.getValue(i)) ;
      strncpy (dcuType,dcuTypeChar,100) ;
      XMLString::release(&dcuTypeChar) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),dcuReadoutEnabledXML_)) {
      char *enabledChar = XMLString::transcode(attributes.getValue(i)) ;
      if (enabledChar[0] == 'F') dcuEnabled = false ;
      XMLString::release(&enabledChar) ;
      val ++ ;
    }
  }

  if (val != 19) std::cerr << "DCU: invalid number of parameters: " << val << "/19" << std::endl ;

  dcuDescription *device = new dcuDescription(buildCompleteKey(fec,ring,ccu,channel,address),timestamp, dcuHardId,
					      channel0, channel1, channel2, channel3, channel4, channel5, 
					      channel6, channel7, dcuType) ;
  device->setEnabled(enabled) ;
  device->setDcuReadoutEnabled(dcuEnabled) ;
  //vDevices->push_back(device) ;

  if (device->getDcuType() == DCUCCU) countDCUCCU ++ ;
  else if (device->getDcuType() == DCUFEH) countDCUFEH ++ ;
}

void SAXCountHandlers::parseAttributesLaserdriver (AttributeList&  attributes) {

  unsigned int val = 0 ;
  char fecHardwareId[100] ;
  bool enabled = true ;
  int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
  int bias0 = 0, bias1 = 0, bias2 = 0, gain0 = 0, gain1 = 0, gain2 = 0 ;        // Laserdriver (AOH or DOH)

  for (unsigned int i=0; i<attributes.getLength(); i++){
    if (XMLString::equals(attributes.getName(i),fecHardIdXML_)) {
      char *fecHardId = XMLString::transcode(attributes.getValue(i)) ;
      strncpy (fecHardwareId,fecHardId,100) ;
      XMLString::release(&fecHardId) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),crateSlotXML_)) {
      crate = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),fecSlotXML_)) {
      fec = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ringSlotXML_)) {
      ring = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ccuAddressXML_)) {
      ccu = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cChannelXML_)) {
      channel = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cAddressXML_)) {
      address = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),enabledXML_)) {
      char *enabledChar = XMLString::transcode(attributes.getValue(i)) ;
      if (enabledChar[0] == 'F') enabled = false ;
      XMLString::release(&enabledChar) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),bias0XML_)) {
      bias0 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),bias1XML_)) {
      bias1 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),bias2XML_)) {
      bias2 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),gain0XML_)) {
      gain0 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),gain1XML_)) {
      gain1 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),gain2XML_)) {
      gain2 = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
  }

  if (val != 13) std::cerr << "Laserdriver: invalid number of parameters: " << val << "/13" << std::endl ;

  laserdriverDescription *device = new laserdriverDescription(buildCompleteKey(fec,ring,ccu,channel,address),
							      bias0, bias1, bias2, gain0, gain1, gain2) ;
  device->setEnabled(enabled) ;
  //vDevices->push_back(device) ;

  if (address == 0x60) countAOH++ ;
  else if (address == 0x70) countDOH++ ;
}

void SAXCountHandlers::parseAttributesPll (AttributeList&  attributes) {

  countPLL ++ ;

  unsigned int val = 0 ;
  char fecHardwareId[100] ;
  bool enabled = true ;
  int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
  int delayFine = 0,delayCoarse = 0 ;

  for (unsigned int i=0; i<attributes.getLength(); i++){
    if (XMLString::equals(attributes.getName(i),fecHardIdXML_)) {
      char *fecHardId = XMLString::transcode(attributes.getValue(i)) ;
      strncpy (fecHardwareId,fecHardId,100) ;
      XMLString::release(&fecHardId) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),crateSlotXML_)) {
      crate = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),fecSlotXML_)) {
      fec = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ringSlotXML_)) {
      ring = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),ccuAddressXML_)) {
      ccu = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cChannelXML_)) {
      channel = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),i2cAddressXML_)) {
      address = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),enabledXML_)) {
      char *enabledChar = XMLString::transcode(attributes.getValue(i)) ;
      if (enabledChar[0] == 'F') enabled = false ;
      XMLString::release(&enabledChar) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),delayFineXML_)) {
      delayFine = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
    else if (XMLString::equals(attributes.getName(i),delayCoarseXML_)) {
      delayCoarse = XMLString::parseInt(attributes.getValue(i)) ;
      val ++ ;
    }
  }

  if (val != 9) std::cerr << "PLL: invalid number of parameters: " << val << "/9" << std::endl ;

  pllDescription *device = new pllDescription(buildCompleteKey(fec,ring,ccu,channel,address),delayFine,delayCoarse) ;
  device->setEnabled(enabled) ;
  //vDevices->push_back(device) ;
}

/**Parses the attribute<BR>
 * Get the name and the value for each attribute<BR>
 * Search in the parameterDescriptionNameType the corresponding element<BR>
 * Set the read value to this element.<BR>
 * The result is stored as a parameterDescriptionNameType<BR>
 * If an atttribute name is not found in the <I>parameterNames</I> parameter, this attribute is skipped<BR>
 * @param parameterNames - ParameterDescription hash_map 
 * @param attributes - XML attributes
 */
unsigned int SAXCountHandlers::parseAttributes(parameterDescriptionNameType *parameterNames, AttributeList &attributes)
{
  unsigned int i;
  for (i=0; i<attributes.getLength(); i++){
    StrX name(attributes.getName(i));
    StrX value(attributes.getValue(i));
    
    if ((*parameterNames).find(name.localForm()) != (*parameterNames).end()) {
       (*parameterNames)[name.localForm()]->setValue(value.getString());
    }
  }
  return i;
}

void SAXCountHandlers::characters(  const   XMLCh* const    /* chars */
								    , const unsigned int    length)
{
    fCharacterCount += length;
}

void SAXCountHandlers::ignorableWhitespace( const   XMLCh* const /* chars */
										    , const unsigned int length)
{
    fSpaceCount += length;
}

void SAXCountHandlers::resetDocument()
{
    fAttrCount = 0;
    fCharacterCount = 0;
    fElementCount = 0;
    fSpaceCount = 0;
}

// ---------------------------------------------------------------------------
//  SAXCountHandlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void SAXCountHandlers::error(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "\nError at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void SAXCountHandlers::fatalError(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "\nFatal Error at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void SAXCountHandlers::warning(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nWarning at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void SAXCountHandlers::resetErrors()
{
    fSawErrors = false;
}


