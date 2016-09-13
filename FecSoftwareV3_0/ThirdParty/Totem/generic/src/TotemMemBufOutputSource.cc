
#include "TotemMemBufOutputSource.h"

/** Default Constructor
 */
TotemMemBufOutputSource::TotemMemBufOutputSource(): MemBufOutputSource() {}

TotemMemBufOutputSource::TotemMemBufOutputSource(ccuVector ccuVector, bool forDb) throw (FecExceptionHandler):
MemBufOutputSource(ccuVector, forDb){}

TotemMemBufOutputSource::TotemMemBufOutputSource (TkRingDescription TkRingDescription, bool forDb, bool withCcus ) throw (FecExceptionHandler):
MemBufOutputSource (TkRingDescription, forDb, withCcus){}

TotemMemBufOutputSource::TotemMemBufOutputSource (tkringVector rings, bool forDb, bool withCcus) throw (FecExceptionHandler):
MemBufOutputSource (rings, forDb, withCcus){}

TotemMemBufOutputSource::TotemMemBufOutputSource(ConnectionVector ConnectionVector, bool forDb) throw (FecExceptionHandler):
MemBufOutputSource(ConnectionVector, forDb){}

TotemMemBufOutputSource::TotemMemBufOutputSource(piaResetVector piaResetVector, bool forDb) throw (FecExceptionHandler):
MemBufOutputSource(piaResetVector, forDb){}

TotemMemBufOutputSource::TotemMemBufOutputSource (dcuConversionVector dcuConversionVector, bool forDb) throw (FecExceptionHandler):
MemBufOutputSource (dcuConversionVector, forDb){}

TotemMemBufOutputSource::TotemMemBufOutputSource (TkIdVsHostnameVector TkIdVsHostnameVector, bool forDb) throw (FecExceptionHandler):
MemBufOutputSource (TkIdVsHostnameVector, forDb){}

TotemMemBufOutputSource::TotemMemBufOutputSource (std::vector<CommissioningAnalysisDescription*> &inVector, CommissioningAnalysisDescription::commissioningType analysisType, bool forDb) throw (FecExceptionHandler):MemBufOutputSource (inVector, analysisType, forDb){}

TotemMemBufOutputSource::TotemMemBufOutputSource (tkDcuInfoVector tkDcuInfoVector, bool forDb) throw (FecExceptionHandler):
MemBufOutputSource (tkDcuInfoVector, forDb){}

TotemMemBufOutputSource::TotemMemBufOutputSource (tkDcuPsuMapVector tkDcuPsuMapVector, bool forDb) throw (FecExceptionHandler):
MemBufOutputSource (tkDcuPsuMapVector, forDb){}


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
 * @see TotemMemBufOutputSource::generateDeviceTag(deviceVector)
 * @see MemBufOutputSource::generateEndTag()
 */
TotemMemBufOutputSource::TotemMemBufOutputSource (deviceVector deviceParameters, bool forDb) throw (FecExceptionHandler){

  generateHeader();
  generateStartTag(COMMON_XML_SCHEME);
  generateDeviceTag(deviceParameters, forDb);
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
 * Write device information<BR>
 * Write an end tag<BR>
 * @param deviceParameters - device description
 * @param piaResetParameters - piaReset description
 * @exception FecExceptionHandler
 * @see MemBufOutputSource::generateHeader()
 * @see MemBufOutputSource::generateStartTag()
 * @see MemBufOutputSource::generatePiaResetTag(piaResetvector)
 * @see TotemMemBufOutputSource::generateTotemDeviceTag(deviceVector)
 * @see MemBufOutputSource::generateEndTag()
 */
TotemMemBufOutputSource::TotemMemBufOutputSource (deviceVector deviceParameters, piaResetVector piaResetParameters, bool forDb) throw (FecExceptionHandler){
  generateHeader();
  generateStartTag(COMMON_XML_SCHEME);
  generatePiaResetTag(piaResetParameters);
  generateDeviceTag(deviceParameters);
  generateEndTag();

#ifdef DISPLAYBUFFER
  std::cout << memBuffer_.str() << std::endl ;
#endif
}

/**Delete a TotemMemBufOuputSource object
 */
TotemMemBufOutputSource::~TotemMemBufOutputSource (){}

/**Generates device tag
 * @param deviceParameters - device descriptions
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if the deviceType code is unknown
 * @see generateXML<deviceType>(<deviceType>Description *, std::ostringstream &);
 */
void TotemMemBufOutputSource::generateDeviceTag(deviceVector deviceParameters, bool forDb) throw (FecExceptionHandler) {
  std::ostringstream errorMsg;

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

    memBufferVfat_ << "<ROWSET>" ;
    memBufferCChip_ << "<ROWSET>" ;
    memBufferTbb_ << "<ROWSET>" ;
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

    case VFAT:
      generateXMLVfat((vfatDescription *)deviced, memBufferVfat_, forDb);
      break;
    case CCHIP:
      generateXMLCChip((totemCChipDescription *)deviced, memBufferCChip_, forDb);
      break;
    case TBB:
      generateXMLTbb((totemBBDescription *)deviced, memBufferTbb_, forDb);
      break;

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

    memBufferVfat_ << "</ROWSET>" ; 
    memBufferVfat_.flush();
    memBufferCChip_ << "</ROWSET>" ; 
    memBufferCChip_.flush();
    memBufferTbb_ << "</ROWSET>" ; 
    memBufferTbb_.flush();

    memBufferPhilips_ << "</ROWSET>" ;
    memBufferPhilips_.flush() ;

    memBufferPll_.flush();
    memBufferApv25_.flush();
    memBufferApvMux_.flush();
    memBufferDcu_.flush();
    memBufferLaserdriver_.flush();
  }
  else {

    memBuffer_ << memBufferApv25_.str() << memBufferApvMux_.str() << memBufferDcu_.str() << memBufferLaserdriver_.str() << memBufferPll_.str() ;
    memBuffer_<< memBufferVfat_.str() << memBufferCChip_.str() << memBufferTbb_.str() << memBufferPhilips_.str() ;
  }

#ifdef PRESHOWER
  memBuffer_ << memBufferDelta.str() << memBufferPace.str() << memBufferKchip.str() 
	     << memBufferGoh.str() ;
#endif // PRESHOWER
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::ostringstream *TotemMemBufOutputSource::getVfatOutputBuffer(){
  return(&memBufferVfat_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::ostringstream *TotemMemBufOutputSource::getCChipOutputBuffer(){
  return(&memBufferCChip_);
};

/**Get the <I>memBuffer_</I> attribute
 * @return the generated character buffer corresponding to the memBuffer_ private attribute
 */
std::ostringstream *TotemMemBufOutputSource::getTbbOutputBuffer(){
  return(&memBufferTbb_);
};


/**Generates a Vfat element
 * @param vfat - vfat description
 * @param memBuffer - XML buffer to fill
 */
void TotemMemBufOutputSource::generateXMLVfat(vfatDescription *vfat, std::ostringstream &memBuffer, bool forDb) {
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
	      << "<I2CADRESS>" << (unsigned int)vfat->getAddress() << "</I2CADRESS>"
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
void TotemMemBufOutputSource::generateXMLCChip(totemCChipDescription *cchip, std::ostringstream &memBuffer, bool forDb) {
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
	      << "<I2CADRESS>" << (unsigned int)cchip->getAddress() << "</I2CADRESS>"
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
void TotemMemBufOutputSource::generateXMLTbb(totemBBDescription *tbb, std::ostringstream &memBuffer, bool forDb) {
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
	      << "<I2CADRESS>" << (unsigned int)tbb->getAddress() << "</I2CADRESS>"
	      << "<TBBCONTROL>" << (unsigned int)tbb->getControl() << "</TBBCONTROL>"
	      << "<TBBLATENCY1>" << (unsigned int)tbb->getLatency1() << "</TBBLATENCY1>"
	      << "<TBBLATENCY2>" << (unsigned int)tbb->getLatency2() << "</TBBLATENCY2>"
	      << "<TBBREPEATS1>" << (unsigned int)tbb->getRepeats1() << "</TBBREPEATS1>"
	      << "<TBBREPEATS2>" << (unsigned int)tbb->getRepeats2() << "</TBBREPEATS2>"
	      << "</RAWTBB>" << std::endl ;;
  }
};
