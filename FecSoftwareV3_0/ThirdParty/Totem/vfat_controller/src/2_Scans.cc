/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#include <fstream>  // Filestream
#include <sstream>  // Stringstream
#include <stdlib.h> // Standard

#include <qcombobox.h> // GUI
#include <qlineedit.h>
#include <qcheckbox.h> 
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistbox.h>

#include "vfat_controller.h" // SW header

/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   Auxiliary Test Methods                                                            */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */ 

/**\brief This method returns a test description with all registers initialized using given input value.
 * This method returns a test description with all registers initialized using given input value.
 * \return vfatDescription
 * \warning No position information set
 */
void vfat_controller::getTestVfatDescription ( vfatDescription &vfatd, tscType8 value) {

  vfatd.setControl0(value) ;
  vfatd.setControl1(value) ;
  vfatd.setControl2(value) ;
  vfatd.setControl3(value) ;

  vfatd.setChipid0(value) ;
  vfatd.setChipid1(value) ;

  vfatd.setUpset(value) ;
  vfatd.setHitCount0(value) ;
  vfatd.setHitCount1(value) ;
  vfatd.setHitCount2(value) ;

  vfatd.setIPreampIn(value) ;
  vfatd.setIPreampFeed(value) ;
  vfatd.setIPreampOut(value) ;
  vfatd.setIShaper(value) ;
  vfatd.setIShaperFeed(value) ;
  vfatd.setIComp(value) ;

  vfatd.setLat(value) ;

  for (int i=1;i<=VFAT_CH_NUM_MAX;i++) { vfatd.setChanReg(i, value) ; }

  vfatd.setVCal(value) ;
  vfatd.setVThreshold1(value) ;
  vfatd.setVThreshold2(value) ;
  vfatd.setCalPhase(value) ;
}

void vfat_controller::cloneTestVfatDescription ( vfatDescription &vfatd, vfatDescription *vfatD ) {

  vfatd.setFecSlot(vfatD->getFecSlot()) ;
  vfatd.setRingSlot(vfatD->getRingSlot()) ;
  vfatd.setCcuAddress(vfatD->getCcuAddress()) ;
  vfatd.setChannel(vfatD->getChannel()) ;
  vfatd.setAddress(vfatD->getAddress()) ;

  vfatd.setControl0(vfatD->getControl0()) ;
  vfatd.setControl1(vfatD->getControl1()) ;
  vfatd.setControl2(vfatD->getControl2()) ;
  vfatd.setControl3(vfatD->getControl3()) ;

  vfatd.setChipid0(vfatD->getChipid0()) ;
  vfatd.setChipid1(vfatD->getChipid1()) ;

  vfatd.setUpset(vfatD->getUpset()) ;
  vfatd.setHitCount0(vfatD->getHitCount0()) ;
  vfatd.setHitCount1(vfatD->getHitCount1()) ;
  vfatd.setHitCount2(vfatD->getHitCount2()) ;

  vfatd.setIPreampIn(vfatD->getIPreampIn()) ;
  vfatd.setIPreampFeed(vfatD->getIPreampFeed()) ;
  vfatd.setIPreampOut(vfatD->getIPreampOut()) ;
  vfatd.setIShaper(vfatD->getIShaper()) ;
  vfatd.setIShaperFeed(vfatD->getIShaperFeed()) ;
  vfatd.setIComp(vfatD->getIComp()) ;

  vfatd.setLat(vfatD->getLat()) ;

  for(int i=1;i<=VFAT_CH_NUM_MAX;i++){ vfatd.setChanReg(i,vfatD->getChanReg(i)) ; }

  vfatd.setVCal(vfatD->getVCal()) ;
  vfatd.setVThreshold1(vfatD->getVThreshold1()) ;
  vfatd.setVThreshold2(vfatD->getVThreshold2()) ;
  vfatd.setCalPhase(vfatD->getCalPhase()) ;
}

void vfat_controller::testSaveVfatValues(){

#ifdef DEBUG_VFAT_CONTROLLER
  //std::cout << SET_FORECOLOR_CYAN << "BEFORE COPYING" << SET_DEFAULT_COLORS << std::endl ;
  //displayDebugInfo() ; 
#endif  

  vfatAccess_ = NULL ;
  vfatd_ = NULL ;

  int loop = 0 ;
  bool exceptionThrown = false ;

  for( int i=0;i<testVfats_.size();i++) {

    loop = 0 ;
    do{

      exceptionThrown = false ;
      try{

	loop++ ;
	vfatAccess_ = testVfats_.at(i) ;
	vfatd_ = vfatAccess_->getVfatValues() ;

	//vfatd_->display() ;
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      failedTest_ = "Reading startup values" ;
      i2cError_ = true ;
      return ;
    }

    testVfatdValues_.push_back(vfatd_) ;

    vfatDescription vfatd ;
    cloneTestVfatDescription(vfatd,vfatd_) ;
    vfatd_start_.push_back(vfatd) ;
  }

#ifdef DEBUG_VFAT_CONTROLLER
  //std::cout << SET_FORECOLOR_CYAN << "AFTER COPYING" << SET_DEFAULT_COLORS << std::endl ;
  //displayDebugInfo() ; 
#endif  
}

void vfat_controller::testWriteBackVfatValues() {

#ifdef DEBUG_VFAT_CONTROLLER
  //std::cout << SET_FORECOLOR_CYAN << "BEFORE RETURN" << SET_DEFAULT_COLORS << std::endl ;
  //displayDebugInfo() ; 
#endif

  vfatAccess_ = NULL ;
  vfatd_ = NULL ;

  int loop = 0 ;
  bool exceptionThrown = false ;

  for( int i=0;i<testVfats_.size();i++) {

    //vfatDescription vfatd ;
    //vfatd_ = testVfatdValues_.at(i) ;
    //cloneTestVfatDescription(vfatd,vfatd_) ;

    vfatDescription vfatd ;
    vfatd = vfatd_start_.at(i) ;
    vfatAccess_ = testVfats_.at(i) ;

    loop = 0 ;
    do{

      exceptionThrown = false ;
      try{

	loop++ ;
	vfatAccess_->setVfatValues(vfatd) ;
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      failedTest_ = "Writing back the startup values" ;
      i2cError_ = true ;
      return ;
    }

#ifdef DEBUG_VFAT_CONTROLLER
//vfatd_->displayDifferences(vfatd) ;
#endif
  
  }

#ifdef DEBUG_VFAT_CONTROLLER
  //std::cout << SET_FORECOLOR_CYAN << "AFTER RETURN" << SET_DEFAULT_COLORS << std::endl ;
  //displayDebugInfo() ; 
#endif

}

void vfat_controller::testCleanUp(){

  vfatAccess_ = NULL ;
  vfatd_ = NULL ;
  
  // delete the pointers from vector
  while (testVfatdValues_.size() > 0) {

    delete testVfatdValues_.back();
    testVfatdValues_.pop_back();
  }
  
  // Clear vectors
  testVfats_.clear() ;
  testVfatIds_.clear() ;
  testVfatdValues_.clear() ;
  vfatd_start_.clear() ;
  chipTestDirs_.clear() ;
  chipTestI2cErrorDirs_.clear() ;
  chipTestBarcodes_.clear() ;
}

void vfat_controller::displayDebugInfo(){

  std::cout << SET_FORECOLOR_CYAN << "*********************************************************" << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << "testVfatdValues_ VECTOR SIZE: " << testVfatdValues_.size() << SET_DEFAULT_COLORS << std::endl ;  
 
  for (int a=0;a<testVfatdValues_.size();a++) {

    vfatDescription *vfatDevice = testVfatdValues_.at(a) ;

    std::cout << SET_FORECOLOR_CYAN << " Position FEC ("
	      << std::hex << (int)vfatDevice->getFecSlot() << ","
	      << std::hex << (int)vfatDevice->getRingSlot() << ") CCU 0x" 
	      << std::hex << (int)vfatDevice->getCcuAddress() << " CHANNEL 0x" 
	      << std::hex << (int)vfatDevice->getChannel() << " ADDRESS 0x" 
	      << std::hex << (int)vfatDevice->getAddress()
	      << std::dec << SET_DEFAULT_COLORS << std::endl ;

    vfatDevice->display() ;
  }

  std::cout << SET_FORECOLOR_CYAN << "vfatd_start_ VECTOR SIZE: " << vfatd_start_.size() << SET_DEFAULT_COLORS << std::endl ;  
 
  for (int a=0;a<vfatd_start_.size();a++) {

    vfatDescription vfatDevice = vfatd_start_.at(a) ;

    std::cout << SET_FORECOLOR_CYAN << " Position FEC ("
	      << std::hex << (int)vfatDevice.getFecSlot() << ","
	      << std::hex << (int)vfatDevice.getRingSlot() << ") CCU 0x" 
	      << std::hex << (int)vfatDevice.getCcuAddress() << " CHANNEL 0x" 
	      << std::hex << (int)vfatDevice.getChannel() << " ADDRESS 0x" 
	      << std::hex << (int)vfatDevice.getAddress()
	      << std::dec << SET_DEFAULT_COLORS << std::endl ;

    vfatDevice.display() ;
  }
  std::cout << SET_FORECOLOR_CYAN << "*********************************************************" << SET_DEFAULT_COLORS << std::endl ;

}

/**\brief This initializes VFAT test.
 * This initializes VFAT test.
 * \return the vfatAccess
 */
void vfat_controller::initVfatTest(){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "VFAT(S) to be tested:" << SET_DEFAULT_COLORS << std::endl ;
#endif

  testInitOK_ = true ;
  int loop = 0 ;
  bool exceptionThrown = false ;

  for (int i=0;i<vfatAddresses_.size();i++){

    if (vfatList_test->isSelected(i) || (chipTesting_ && !advancedChipTesting->isChecked())){

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  loop++ ;
	  testVfats_.push_back((vfatAccess *)fecAccessManager_->getAccess(VFAT, vfatAddresses_.at(i))) ;

	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

      if(exceptionThrown){

	failedTest_ = "Test initialization" ;
	i2cError_ = true ;
	testInitOK_ = false ;
	return ;
      }

#ifdef DEBUG_VFAT_CONTROLLER
      char key[100] ;
      decodeKey(key,vfatAddresses_.at(i)) ;
      std::cout << SET_FORECOLOR_CYAN << key << SET_DEFAULT_COLORS << std::endl ;
      std::cout << SET_FORECOLOR_CYAN << std::dec << "testVfats_ vector size:" << testVfats_.size() << SET_DEFAULT_COLORS << std::endl ;
#endif
    }
  }
}

/** 
 * This method reads VFAT's chip ID.
 */
std::string vfat_controller::readVfatChipID () {

  std::ostringstream results ;
  std::stringstream vfatid ;

  int loop = 0 ;
  bool exceptionThrown = false ;

  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "VFAT ID(S): " << SET_DEFAULT_COLORS ; }
  results << "VFAT ID(S) : " ;

  vfatAccess_ = NULL ;

  for( int i=0;i<testVfats_.size();i++) {

    vfatAccess_ = testVfats_.at(i) ;
    loop = 0 ;

    do{

      exceptionThrown = false ;
      vfatid.str("") ;

      try{

	loop++ ;
	vfatid << "0x" << IntToHex((int)vfatAccess_->getVfatChipid1()) << IntToHex((int)vfatAccess_->getVfatChipid0()) ;

      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; i2cError_ = true ;}
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      //failedTest_ = "Reading chip IDs" ;
      //i2cError_ = true ;
      //return (results.str());
      std::cout << SET_FORECOLOR_RED << "COULDN'T READ CHIP IDENTIFICATION !!!" << SET_DEFAULT_COLORS << std::endl ;
      vfatid.str(CHIPID_NONSENCE) ;
    }

    testVfatIds_.push_back( vfatid.str()) ;

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << testVfatIds_.at(i) << SET_DEFAULT_COLORS ; }

    results << testVfatIds_.at(i) ;

    if(i==testVfats_.size()-1) {

      if(showConsole_) { std::cout << std::endl ; }
      results << std::endl ;
    }
    else {

      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << ", " << SET_DEFAULT_COLORS ; }
      results << ", " ;
    }
    vfatid.str("") ;
  }

  vfatid.flush() ;

  return (results.str()) ;
}

std::string vfat_controller::testI2CError( int testNumber, int vfatNumber ) {

  std::ostringstream failedVfat ;

  if(showConsole_) { std::cout << SET_FORECOLOR_RED << testVfatIds_.at(vfatNumber) << "\tFAIL" << SET_DEFAULT_COLORS << std::endl ; }
  failedVfat << testVfatIds_.at(vfatNumber) << "\tFAIL" << std::endl ;  
  i2cError_ = true ;

  switch (testNumber){

  case 1:
    failedTest_ = "I2C Test" ;
    if(chipTesting_){ ledChipTest->setPixmap(redLed_) ; }
    break;
  case 2:
    //failedTest_ = "Power On" ;
    // ledPowerOnTest->setPixmap(redLed_) ;
    break;
  case 3:
    failedTest_ = "Treasure Hunt" ;
    if(chipTesting_){ ledChipTest->setPixmap(redLed_) ; }
    else{ ledPosMapping->setPixmap(redLed_) ; }
    break;
  case 4:
    failedTest_ = "DAC Characterization" ;
    if(chipTesting_){ ledChipTest->setPixmap(redLed_) ; }
    else{ ledDACTest->setPixmap(redLed_) ; }
    break;
  case 5:
    failedTest_ = "Pulse Channel Test" ;
    if(chipTesting_){ ledChipTest->setPixmap(redLed_) ; }
    else{ ledPulseOneChannelTest->setPixmap(redLed_) ; }
    break;
  case 6:
    failedTest_ = "Channel Test" ;
    if(chipTesting_){ ledChipTest->setPixmap(redLed_) ; }
    else{ ledChannelTest->setPixmap(redLed_) ; }
    break;
  case 7:
    failedTest_ = "Threshold Measurement" ;
    ledThresholdMeasurementTest->setPixmap(redLed_) ;
    break;
  case 8:
    failedTest_ = "Latency Scan" ;
    ledLatencyScanTest->setPixmap(redLed_) ;
    break;
  case 9:
    failedTest_ = "Data Packet Check" ;
    ledChipTest->setPixmap(redLed_) ;
    break;
  case 10:
    failedTest_ = "Mask Check" ;
    ledChipTest->setPixmap(redLed_) ;
    break;
  default:
    failedTest_ = "UNKNOWN Test" ;
    std::cout << SET_FORECOLOR_RED << "TEST NUMBER MISCMATCH"  << SET_DEFAULT_COLORS << std::endl ;
    break;
  }

  return (failedVfat.str()) ;
}

void vfat_controller::getDefaultDescription ( vfatDescription &vfatd, std::string file ) {

  if (file.size()>0) {

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "Load description values from " << file << SET_DEFAULT_COLORS << std::endl ;
#endif  

    TotemFecFactory fecFactory ;

    // Parse a file for VFATs
    //fecFactory.addFileName (file) ;
	
    // Load it from file
    deviceVector vDevice ;
    fecFactory.getFecDeviceDescriptions(vDevice, file) ;

    if (vDevice.size() > 0) { //vfatd = (vfatDescription)vDevice.at(0) ; }

      //deviceDescription *deviced = vDevice.at(0) ;
      vfatDescription *vfatDevice = (vfatDescription *)vDevice.at(0) ;

      cloneTestVfatDescription(vfatd,vfatDevice) ;

    }
    else {

      std::cout << SET_FORECOLOR_RED << "XML filename empty -> No values to write." << SET_DEFAULT_COLORS << std::endl ;
      ledChipTest->setPixmap(redLed_) ;
    }
  }
}


std::string vfat_controller::getDifferences( vfatDescription *vfatd, vfatDescription &vfatd_ref ){

  std::ostringstream diff ;

  if (vfatd->getControl0() != vfatd_ref.getControl0 ( )){ diff << "control0 " ; }
  if (vfatd->getControl1() != vfatd_ref.getControl1 ( )){ diff << "control1 " ; }
  if (vfatd->getIPreampIn() != vfatd_ref.getIPreampIn ( )){ diff << "ipreampin " ; }
  if (vfatd->getIPreampFeed() != vfatd_ref.getIPreampFeed ( )){ diff << "ipreampfeed " ; }
  if (vfatd->getIPreampOut() != vfatd_ref.getIPreampOut ( )){ diff << "ipreampout " ; }
  if (vfatd->getIShaper() != vfatd_ref.getIShaper()){ diff << "ishaper " ; }
  if (vfatd->getIShaperFeed() != vfatd_ref.getIShaperFeed()){ diff << "ishaperfeed " ; }
  if (vfatd->getIComp() != vfatd_ref.getIComp()){ diff << "icomp " ; }
  if (vfatd->getLat() != vfatd_ref.getLat()){ diff << "lat " ; }
  if (vfatd->getVCal() != vfatd_ref.getVCal()){ diff << "vcal " ; }
  if (vfatd->getVThreshold1() != vfatd_ref.getVThreshold1()){ diff << "vthreshold1 " ; }
  if (vfatd->getVThreshold2() != vfatd_ref.getVThreshold2()){ diff << "vyhreshold2 " ; }
  if (vfatd->getCalPhase() != vfatd_ref.getCalPhase()){ diff << "calphase " ; }
  if (vfatd->getControl2() != vfatd_ref.getControl2()){ diff << "control2 " ; } 
  if (vfatd->getControl3() != vfatd_ref.getControl3()){ diff << "control3 " ; } 

  for (int i=1;i<=VFAT_CH_NUM_MAX;i++){

    if(vfatd->getChanReg(i)!=vfatd_ref.getChanReg(i)){ diff << "chanreg" << i << " " ; }      
  }

  return(diff.str()) ;
}

/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   Test Methods                                                                      */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */ 

/** 
 * This method powers the VFAT on.
 */
/*
std::string vfat_controller::powerOnVfat () {

  std::ostringstream results ;
  tscType8 value = 0x00 ;

  int loop = 0 ;
  bool exceptionThrown = false ;

  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "POWER ON:" << SET_DEFAULT_COLORS << std::endl ; }
  results <<  "POWER ON:" << std::endl ;

  vfatAccess_ = NULL ;

  for( int i=0;i<testVfats_.size();i++) {

    vfatAccess_ = testVfats_.at(i) ;
    loop = 0 ;

    do{

      exceptionThrown = false ;
      try{

	loop++ ;
	value = vfatAccess_->getVfatControl0() ;
	vfatAccess_->setVfatControl0(value | VFAT_POWERON_MASK) ; // Set the VFAT to run mode
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      results << testI2CError(2, i) ;
      return (results.str());
    }
    else{

      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << testVfatIds_.at(i) << "\tOK " << SET_DEFAULT_COLORS << std::endl ; }
      results << testVfatIds_.at(i) << "\tOK " << std::endl ;
      ledPowerOnTest->setPixmap(greenLed_) ;
    }
  }
  
  return (results.str()) ;
}
*/


/** 
 * This method does the mapping according to the VFAT position.
 */
std::string vfat_controller::positionMappingVfat(){

  std::ostringstream results ;
  std::ostringstream oss ;
  std::ofstream outputfilestream ;
  std::string fn = vfat_controller_home_ + VFAT_PATH_POS_MAPPING ;

  int confNumber = 1 ;
  //bool completeTag = true ;
  tscType8 value = 0x00 ;

  int loop = 0 ;
  bool exceptionThrown = false ;

  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "VFAT Treasure Hunt:" << SET_DEFAULT_COLORS << std::endl ; }
  results <<  "VFAT Treasure Hunt:" << std::endl ;

  if(fecType_!=FECVME){

    std::cout << SET_FORECOLOR_RED << "This Treasure Hunt procedure is ONLY for VME FEC system with \"Master\" VFAT !!!" << SET_DEFAULT_COLORS << std::endl ;
    return results.str() ;
  }

#ifdef TTCCI_TRIGGER
  uint sequenceNumber = 0 ;
  std::string ttcci_seq = triggerSeqName->text().latin1() ;
  int ttcci_msleeptime = atoi(triggerSleepT->text()) ;
#endif

#ifdef VMEREADOUT

  std::string fn_data = vfat_controller_home_ + VME_DATA_POS_MAPPING ;

  totalBytesRead_ = 0 ;

  uint fiberEnable = 0x3fff ;
  sscanf (fiberEnableT->text(), "%x", &fiberEnable) ;
  int nRepetitions = atoi(triggerRepetitionsT->text()) ;

  std::cout << SET_FORECOLOR_CYAN << "nRepetitions=" << nRepetitions << ", fiberEnable=0x" << std::hex << fiberEnable << std::dec << SET_DEFAULT_COLORS << std::endl ;

  totfed_configure(fiberEnable) ;
  totfed_init() ; // empty fifos

  if (!vmeofstream_.is_open()){ vmeofstream_.open(fn_data.c_str()) ; }

#endif

  vfatAccess_ = NULL ;
  vfatd_ = NULL ;

  deviceVector vDevices ;
  int indexOfMaster = -1 ;

  // Search for the Master, put all but the Master VFAT to sleep mode
  for( int i=0;i<testVfats_.size();i++) {

    //vfatDescription vfatd ;
    vfatAccess_ = testVfats_.at(i) ;
    vfatd_ = testVfatdValues_.at(i) ;

    //cloneTestVfatDescription(vfatd,vfatd_) ;

    uint temp = vfatd_->getControl0() ;
    loop = 0 ;

    // Is this one the Master?
    if(indexOfMaster<0 && (vfatd_->getControl0() & 0x01 == 0x01)){

      indexOfMaster = i ;
      if(showConsole_){ std::cout << SET_FORECOLOR_BLUE << "Using VFAT #" << indexOfMaster << " " << testVfatIds_.at(i) << " as a \"Master\", putting the rest to sleep."  << SET_DEFAULT_COLORS << std::endl ; }
    }

    // If other VFAT in RUN mode
    else if(indexOfMaster>=0 && (vfatd_->getControl0() & 0x01 == 0x01)){

      do{

	exceptionThrown = false ;
	try{

	  loop++ ;
	  temp = vfatd_->getControl0() & VFAT_POWERON_INVERTED_MASK ;
	  vfatAccess_->setVfatControl0(temp) ; // Set the VFAT to sleep mode
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

      if(exceptionThrown){

	results << testI2CError(3, i) ;
	return (results.str());
      }
    }

#ifdef DEBUG_VFAT_CONTROLLER

    std::cout << SET_FORECOLOR_CYAN << "#" << i << " Position FEC ("
	      << std::hex << (int)vfatd_->getFecSlot() << ","
	      << std::hex << (int)vfatd_->getRingSlot() << ") CCU 0x" 
	      << std::hex << (int)vfatd_->getCcuAddress() << " CHANNEL 0x" 
	      << std::hex << (int)vfatd_->getChannel() << " ADDRESS 0x" 
	      << std::hex << (int)vfatd_->getAddress()
	      << std::dec << SET_DEFAULT_COLORS << std::endl ;
#endif

    //vfatd.setControl0(temp) ;
    //vDevices.push_back(&vfatd) ;
    vfatd_->setControl0(temp) ;
    vDevices.push_back(vfatd_) ;
  }

  oss << "<?xml version=\"1.0\"?>" << std::endl ;
  oss << "<ROWSET>" << std::endl ;

  oss << getXMLTagDefault(vDevices) ;


  // 1st the Master, which is in the run mode already. This is done first so that it's recognized by Monitor

  /*
    vfatAccess_ = testVfats_.at(indexOfMaster) ;
    vfatd_ = testVfatdValues_.at(indexOfMaster) ;

    loop = 0 ;
    exceptionThrown = false ;

    do{

    exceptionThrown = false ;

    try{

    loop++ ;
    //value = vfatAccess_->getVfatControl0() ;
    value = vfatd_->getControl0() | VFAT_POWERON_MASK ;
    vfatAccess_->setVfatControl0(value) ; // Set the VFAT to run mode

    #ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "CR0: " << std::hex << (int)value << std::dec << SET_DEFAULT_COLORS << std::endl ;
    #endif

    }
    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

    results << testI2CError(3, i) ;
    return (results.str());
    }
  */

  oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

  oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(indexOfMaster).substr(4,2)) // last 2 digits
      << "\" chipid1=\"" << HexToInt(testVfatIds_.at(indexOfMaster).substr(2,2)) << "\" />" << std::endl ; // first 2 digits
	      
  oss << "</configuration>" << std::endl ;
  confNumber+=1 ;

#ifdef TTCCI_TRIGGER
#ifdef VMEREADOUT
	    
  ttcci_executeSeq( MY_TTCCI_RSYNC , ttcci_msleeptime ) ; // RSync

  if(generateAndReadEvents( sequenceNumber, nRepetitions, ttcci_seq , ttcci_msleeptime ) != 0){

    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
    return (results.str()) ;
  }

  sequenceNumber++ ;

#else
  ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ;
#endif

#else
  // External trigger
  system (VFAT_TRIGGER_COMMAND) ;
#endif


  // The rest of the Hunt
  for( int i=0;i<testVfats_.size();i++) {

    // All but the Master (was done before)
    if(i!=indexOfMaster) {

      vfatAccess_ = testVfats_.at(i) ;
      vfatd_ = testVfatdValues_.at(i) ;

      //std::cout << testVfatIds_.at(i) ;
      // results << testVfatIds_.at(i) ;

      loop = 0 ;
      exceptionThrown = false ;

      do{

	exceptionThrown = false ;

	try{

	  loop++ ;
	  //value = vfatAccess_->getVfatControl0() ;
	  value = vfatd_->getControl0() | VFAT_POWERON_MASK ;
	  vfatAccess_->setVfatControl0(value) ; // Set the VFAT to run mode

#ifdef DEBUG_VFAT_CONTROLLER
	  std::cout << SET_FORECOLOR_CYAN << "CR0: " << std::hex << (int)value << std::dec << SET_DEFAULT_COLORS << std::endl ;
#endif

	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

      if(exceptionThrown){

	results << testI2CError(3, i) ;
	return (results.str());
      }

      oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

      oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(i).substr(4,2)) // last 2 digits
	  << "\" chipid1=\"" << HexToInt(testVfatIds_.at(i).substr(2,2)) << "\" />" << std::endl ; // first 2 digits
	      
      oss << "</configuration>" << std::endl ;
      confNumber+=1 ;

#ifdef TTCCI_TRIGGER
#ifdef VMEREADOUT
	    
      ttcci_executeSeq( MY_TTCCI_RSYNC , ttcci_msleeptime ) ; // RSync

      if(generateAndReadEvents( sequenceNumber, nRepetitions, ttcci_seq , ttcci_msleeptime ) != 0){

	if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
	std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	return (results.str()) ;
      }

      sequenceNumber++ ;

#else
      ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ;
#endif

#else
      // External trigger
      system (VFAT_TRIGGER_COMMAND) ;
#endif

      // Set VFAT to sleep mode
      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  loop++ ;
	  vfatAccess_->setVfatControl0(vfatd_->getControl0() & VFAT_POWERON_INVERTED_MASK) ; // Set to sleep mode	
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
    
      if(exceptionThrown){

	results << testI2CError(3, i) ;
	return (results.str());
      }

      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << testVfatIds_.at(i) << "\tOK " << SET_DEFAULT_COLORS << std::endl ; }
      results << testVfatIds_.at(i) << "\tOK " << std::endl ;
    }
  }

  oss << "</ROWSET>" << std::endl ;

#ifdef VMEREADOUT
    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
#endif

  if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
  if(outputfilestream.is_open()){

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "WRITING CONFIGURATION TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
    outputfilestream << oss.str() ;
    outputfilestream.close() ;
  }

  ledPosMapping->setPixmap(greenLed_) ;

  return (results.str()) ;
}

/** 
 * This method tests the DACs on VFAT.
 */
std::string vfat_controller::testVfatDAC () {

  std::ostringstream results ;
  tscType8 dcuVoltageChannel ;
  tscType8 dcuCurrentChannel ;

  int loop = 0 ;
  bool exceptionThrown = false ;
  int step = 1 ;

  if (detectorType_ == "RP"){

    dcuVoltageChannel = DCU_RP_V_CH ;
    dcuCurrentChannel = DCU_RP_I_CH ;
  }
  else{

    dcuVoltageChannel = DCU_V_CH ;
    dcuCurrentChannel = DCU_I_CH ;
  }

  // DAC characterisation common to all detectors
  for( int j=0;j<testVfats_.size();j++) {

    std::cout << SET_FORECOLOR_BLUE << "DAC scan, VFAT #" << j+1 << SET_DEFAULT_COLORS << std::endl ;

    tscType8 DACSel = (tscType8)0x01 ;
    tscType8 calMode = (tscType8)0x00 ;

    std::stringstream dcuChannel ;
    std::ostringstream measurements ;
    std::ofstream outputfilestream ;
    std::string fn ;

    if(chipTesting_) {

      if(detectorType_ == "RP"){

	for(int i=0;i<dcuAddresses_.size();i++) {

	  if(getChannelKey(dcuAddresses_.at(i)) == DCU_RPHYBRID_CH){

	    dcuAccess_ = (dcuAccess *)fecAccessManager_->getAccess(DCU, dcuAddresses_.at(i)) ;
	    break ;
	  }
	}

	//dcuAccess_ = (dcuAccess *)fecAccessManager_->getAccess(DCU, dcuAddresses_.at(1)) ;
      }
      else{

	for(int i=0;i<dcuAddresses_.size();i++) {

	  if(getChannelKey(dcuAddresses_.at(i)) == DCU_TTP_CH){

	    dcuAccess_ = (dcuAccess *)fecAccessManager_->getAccess(DCU, dcuAddresses_.at(i)) ;
	    break ;
	  }
	}

	//dcuAccess_ = (dcuAccess *)fecAccessManager_->getAccess(DCU, dcuAddresses_.at(0)) ;
      }
    }
    else{

      vfatd_ = testVfatdValues_.at(j) ; 

      // ROMAN POT DCU SELECTION
      if (detectorType_ == "RP"){

	uint channel = vfatd_->getChannel() ;

	for(int i=0;i<dcuAddresses_.size();i++) {

	  uint hybrid = getChannelKey(dcuAddresses_.at(i)) ;

#ifdef DEBUG_VFAT_CONTROLLER
	  std::cout << SET_FORECOLOR_CYAN << "ADDRESS: " << channel << " / " << hybrid << SET_DEFAULT_COLORS << std::endl ;
#endif

	  if(channel==hybrid){

	    dcuAccess_ = (dcuAccess *)fecAccessManager_->getAccess(DCU, dcuAddresses_.at(i)) ;

#ifdef DEBUG_VFAT_CONTROLLER
	    std::cout << SET_FORECOLOR_CYAN << "DCU on FEC (" 
		      << std::hex << (int)dcuAccess_->getFecSlot() << ","
		      << std::hex << (int)dcuAccess_->getRingSlot() << ") CCU 0x" 
		      << std::hex << (int)dcuAccess_->getCcuAddress() << " CHANNEL 0x" 
		      << std::hex << (int)dcuAccess_->getChannelId() << " ADDRESS 0x" 
		      << std::hex << (int)dcuAccess_->getI2CAddress()
		      << std::dec << SET_DEFAULT_COLORS << std::endl ;
#endif
	    break ; // DCU found no need check the next
	  }
	}
      }
    }

    // Abort if no DCU
    if(dcuAccess_==NULL){

      std::cout << SET_FORECOLOR_RED << "NO DCU FOUND" << SET_DEFAULT_COLORS << std::endl ;
      return results.str() ;
    }


    vfatAccess_ = testVfats_.at(j) ;

    loop = 0 ;
    do{

      exceptionThrown = false ;
      try{

	loop++ ;

	uint value = vfatAccess_->getVfatControl0() ;
	value = value & VFAT_CALMODE_INVERTED_MASK ; // Set CalMode to 0
	value = value | VFAT_POWERON_MASK ; // Power On

	//calMode = VFAT_CALMODE_NORMAL_MASK ^ value ;
	vfatAccess_->setVfatControl0(value) ; // Select Normal CalMode, which is 0 -> no need to change	
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
    if(exceptionThrown){

      results << testI2CError(4, j) ;
      return (results.str());
    }

    // Set the filename
    if(chipTesting_) { fn = chipTestDirs_.at(j) + TTP_DAC_CHECK ; }
    else{ fn = vfat_controller_home_ + VFAT_PATH_DAC_PREFIX + testVfatIds_.at(j) + VFAT_PATH_DAC_POSIX ; }

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "Filename to use: " << fn << SET_DEFAULT_COLORS << std::endl ;
#endif

    measurements << "VFAT ID : " << testVfatIds_.at(j) << std::endl << "DAC responses on " << getTimeInfo() ;
    measurements << "-------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;

    std::cout << SET_FORECOLOR_BLUE << "PROGRESS:" << SET_DEFAULT_COLORS << std::endl ;

    for ( int dacnum=1;dacnum<=VFAT_DAC_NUM_MAX;dacnum++){

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  loop++ ;
	  vfatAccess_->setVfatControl1(DACSel) ; // DAC selection
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }

#ifdef DEBUG_VFAT_CONTROLLER
      std::cout << SET_FORECOLOR_CYAN << "DACSel=" << std::dec << (int)DACSel << SET_DEFAULT_COLORS << std::endl ;
#endif

      if (dacnum==1) { measurements << "IPreampIn ( Input / DAC responce ): " << std::endl ; std::cout << SET_FORECOLOR_BLUE << "* IPreampIn" << SET_DEFAULT_COLORS << std::endl ; }
      else if (dacnum==2) { measurements << "IPreampFeed ( Input / DAC responce ): " << std::endl ; std::cout << SET_FORECOLOR_BLUE << "* IPreampFeed" << SET_DEFAULT_COLORS << std::endl ; }
      else if (dacnum==3) { measurements << "IPreampOut ( Input / DAC responce ): " << std::endl ; std::cout << SET_FORECOLOR_BLUE << "* IPreampOut" << SET_DEFAULT_COLORS << std::endl ; }
      else if (dacnum==4) { measurements << "IShaper ( Input / DAC responce ): " << std::endl ; std::cout << SET_FORECOLOR_BLUE << "* IShaper" << SET_DEFAULT_COLORS << std::endl ; }
      else if (dacnum==5) { measurements << "IShaperFeed ( Input / DAC responce ): " << std::endl ; std::cout << SET_FORECOLOR_BLUE << "* IShaperFeed" << SET_DEFAULT_COLORS << std::endl ; }
      else if (dacnum==6) { measurements << "IComp ( Input / DAC responce ): " << std::endl ; std::cout << SET_FORECOLOR_BLUE << "* IComp" << SET_DEFAULT_COLORS << std::endl ; }
      else if (dacnum==7) { measurements << "VThreshold1 ( Input / DAC responce ): " << std::endl ; std::cout << SET_FORECOLOR_BLUE << "* VThreshold1" << SET_DEFAULT_COLORS << std::endl ; }
      else if (dacnum==8) { measurements << "VThreshold2 ( Input / DAC responce ): " << std::endl ; std::cout << SET_FORECOLOR_BLUE << "* VThreshold2" << SET_DEFAULT_COLORS << std::endl ; }

      for (int i=0;i<=VFAT_REG_VALUE_MAX;i+=step){

	//if(!shortDACScan_ || ((0<=i<=4) || (6<=i<=8) || (14<=i<=16) || (30<=i<=32) || (62<=i<=64) || (126<=i<=128) || (254<=i<=255))){

	if(!shortDACScan_  || (shortDACScan_ && ((0<=i && i<=4) || (6<=i && i<=8) || (14<=i && i<=16) || (30<=i && i<=32) || (62<=i && i<=64) || (126<=i && i<=128) || (i==192) || (254<=i && i<=255)))){

	  //if(!shortDACScan_  || (shortDACScan_ && ((0<=i<=4) || (6<=i<=8) || (14<=i<=16) || (30<=i<=32) || (62<=i<=64) || (126<=i<=128) || (254<=i<=255)))){

	  loop = 0 ;
	  do{

	    exceptionThrown = false ;
	    try{

	      loop++ ;

	      switch (dacnum){

	      case 1:
		vfatAccess_->setVfatIPreampIn((tscType8)i) ; // Set input
		dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuCurrentChannel, false) ;
		if(i==VFAT_REG_VALUE_MAX){ vfatAccess_->setVfatIPreampIn((tscType8)0x00) ; }
		break;
	      case 2:
		vfatAccess_->setVfatIPreampFeed((tscType8)i) ; // Set input
		dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuCurrentChannel, false) ;
		if(i==VFAT_REG_VALUE_MAX){ vfatAccess_->setVfatIPreampFeed((tscType8)0x00) ; }
		break;
	      case 3:
		vfatAccess_->setVfatIPreampOut((tscType8)i) ; // Set input
		dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuCurrentChannel, false) ;
		if(i==VFAT_REG_VALUE_MAX){ vfatAccess_->setVfatIPreampOut((tscType8)0x00) ; }
		break;
	      case 4:
		vfatAccess_->setVfatIShaper((tscType8)i) ; // Set input
		dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuCurrentChannel, false) ;
		if(i==VFAT_REG_VALUE_MAX){ vfatAccess_->setVfatIShaper((tscType8)0x00) ; }
		break;
	      case 5:
		vfatAccess_->setVfatIShaperFeed((tscType8)i) ; // Set input
		dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuCurrentChannel, false) ;
		if(i==VFAT_REG_VALUE_MAX){ vfatAccess_->setVfatIShaperFeed((tscType8)0x00) ; }
		break;
	      case 6:
		vfatAccess_->setVfatIComp((tscType8)i) ; // Set input
		dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuCurrentChannel, false) ;
		if(i==VFAT_REG_VALUE_MAX){ vfatAccess_->setVfatIComp((tscType8)0x00) ; }
		break;
	      case 7:
		vfatAccess_->setVfatVThreshold1((tscType8)i) ; // Set input
		dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuVoltageChannel, false) ;
		if(i==VFAT_REG_VALUE_MAX){ vfatAccess_->setVfatVThreshold1((tscType8)0x00) ; }
		break;
	      case 8:
		vfatAccess_->setVfatVThreshold2((tscType8)i) ; // Set input
		dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuVoltageChannel, false) ;
		if(i==VFAT_REG_VALUE_MAX){ vfatAccess_->setVfatVThreshold2((tscType8)0x00) ; }
		break;
	      default:
		std::cout << SET_FORECOLOR_RED << "VFAT DAC MISCMATCH"  << SET_DEFAULT_COLORS << std::endl ;
		break;
	      }
#ifdef DEBUG_VFAT_CONTROLLER
	      //std::cout << SET_FORECOLOR_CYAN << dcuChannel.str() << SET_DEFAULT_COLORS << std::endl ;
#endif
	    }
	    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; dcuChannel.str("") ; }
	  }
	  while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	  if(exceptionThrown){

	    results << testI2CError(4, j) ;
	    return (results.str());
	  }

	  measurements << i << "\t" << dcuChannel.str() << std::endl ;
	  dcuChannel.str("") ;

	  //std::cout << SET_FORECOLOR_CYAN << i << SET_DEFAULT_COLORS << std::endl ;

	  /*
	    if(shortDACScan_){ 

	    //step = i+1 ; // original

	    if(i+(i+1)-1!=i){ step = i+(i+1)-1 ; } // 1 before
	    else if(i==step+2){ step = (i+1)-1 ; } // 1 after
	    else{ step = 1 ; }
	    }
	  */
	}
      }
      
      DACSel += (tscType8)1 ;
      measurements << std::endl ;
    }      
      
    // READ BASELINE VALUE (Vhi)

    DACSel += (tscType8)1 ; // Jump to CalOut

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "DACSel=" << std::dec << (int)DACSel << SET_DEFAULT_COLORS << std::endl ;
#endif

    measurements << "Baseline ( Input / DAC responce ): " << std::endl ;
    std::cout << SET_FORECOLOR_BLUE << "* Baseline" << SET_DEFAULT_COLORS << std::endl ;

    loop = 0 ;
    do{

      exceptionThrown = false ;	
      try{

        vfatAccess_->setVfatControl1(DACSel) ; // DAC selection

	uint value = vfatAccess_->getVfatControl0() ;
	value = value & VFAT_CALMODE_INVERTED_MASK ; // Set CalMode to 0
  
	calMode = VFAT_CALMODE_VHI_MASK ^ value ;
	vfatAccess_->setVfatControl0(calMode) ; // Set calibration mode to Vlow (01XX XXXX)
	//vfatAccess_->setVfatControl0((tscType8)0x41) ;
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; dcuChannel.str("") ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
    if(exceptionThrown){

      results << testI2CError(4, j) ;
      return (results.str());
    }

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "DACSel=" << std::dec << (int)DACSel << ", calMode: " << (int)calMode << SET_DEFAULT_COLORS << std::endl ;
#endif

    step = 1 ;

    for ( int i=0;i<=VFAT_REG_VALUE_MAX;i+=step){

      if(!shortDACScan_  || (shortDACScan_ && ((0<=i && i<=4) || (6<=i && i<=8) || (14<=i && i<=16) || (30<=i && i<=32) || (62<=i && i<=64) || (126<=i && i<=128) || (i==192) || (254<=i && i<=255)))){

	loop = 0 ;
	do{

	  exceptionThrown = false ;	
	  try{

	    vfatAccess_->setVfatVCal((tscType8)i) ;
   
	    dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuVoltageChannel, false) ;
	    measurements << i << "\t" << dcuChannel.str() << std::endl ;
	    dcuChannel.str("") ;
	  }
	  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; dcuChannel.str("") ; }
	}
	while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	if(exceptionThrown){

	  results << testI2CError(4, j) ;
	  return (results.str());
	}

	//if(shortDACScan_){ step = i+1 ; }
      }
    }
	 
    measurements << std::endl << "VCal ( Input / DAC responce ): " << std::endl ;
    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "* VCal" << SET_DEFAULT_COLORS << std::endl ; }

    // READ VCAL VALUE (Vlow)

    loop = 0 ;
    do{

      exceptionThrown = false ;
      try{

	uint value = vfatAccess_->getVfatControl0() ;
	value = value & VFAT_CALMODE_INVERTED_MASK ; // Set CalMode to 0

	calMode = VFAT_CALMODE_VLOW_MASK ^ value ;
	vfatAccess_->setVfatControl0(calMode) ; // Set calibration mode to Vhi (10XX XXXX)
	//vfatAccess_->setVfatControl0((tscType8)0x81) ;
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; dcuChannel.str("") ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
    if(exceptionThrown){

      results << testI2CError(4, j) ;
      return (results.str());
    }

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "DACSel=" << std::dec << (int)DACSel << ", calMode: " << (int)calMode << SET_DEFAULT_COLORS << std::endl ;
#endif

    step = 1 ;

    for ( int i=0;i<=VFAT_REG_VALUE_MAX;i+=step){

      if(!shortDACScan_  || (shortDACScan_ && ((0<=i && i<=4) || (6<=i && i<=8) || (14<=i && i<=16) || (30<=i && i<=32) || (62<=i && i<=64) || (126<=i && i<=128) || (i==192) || (254<=i && i<=255)))){

	loop = 0 ;
	do{

	  exceptionThrown = false ;
	  try{
	 
	    vfatAccess_->setVfatVCal((tscType8)i) ; // Set input

	    dcuChannel << std::dec << (int)dcuAccess_->getDcuChannel(dcuVoltageChannel, false) ;
	    measurements << i << "\t" << dcuChannel.str() << std::endl ;
	    dcuChannel.str("") ;
	  }
	  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; dcuChannel.str("") ; }
	}
	while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	if(exceptionThrown){

	  results << testI2CError(4, j) ;
	  return (results.str());
	}

	//if(shortDACScan_){ step = i+1 ; }
      }
    }


    // Restore settings (DAC selection, Cal Mode, DACs)
    if(chipTesting_) {

      vfatDescription vfatDefaults ;

      //std::string fn = vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS ;
      getDefaultDescription(vfatDefaults, vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS) ;

      //getDefaultDescription(vfatDefaults) ;

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatControl0(vfatDefaults.getControl0()) ; // Cal Mode
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatControl1(vfatDefaults.getControl1()) ; // DAC selection OFF
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatIPreampIn(vfatDefaults.getIPreampIn()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatIPreampFeed(vfatDefaults.getIPreampFeed()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatIPreampOut(vfatDefaults.getIPreampOut()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }


      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatIShaper(vfatDefaults.getIShaper()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }


      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatIShaperFeed(vfatDefaults.getIShaperFeed()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatIComp(vfatDefaults.getIComp()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatVCal(vfatDefaults.getVCal()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }


      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatVThreshold1(vfatDefaults.getVThreshold1()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }


      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  vfatAccess_->setVfatVThreshold2(vfatDefaults.getVThreshold2()) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }
    }
    else {

#ifdef DEBUG_VFAT_CONTROLLER
      std::cout << SET_FORECOLOR_CYAN << "Put CalMode Normal and VCal 0" << SET_DEFAULT_COLORS << std::endl ;
#endif

      loop = 0 ;
      do{

	exceptionThrown = false ;
	try{

	  uint value2 = vfatAccess_->getVfatControl0() ;
	  vfatAccess_->setVfatControl0(value2 & VFAT_CALMODE_INVERTED_MASK) ; // Set calibration mode to Normal (00XX XXXX)
	  vfatAccess_->setVfatVCal((tscType8)0x00) ;

	  DACSel = (tscType8)0 ;
	  vfatAccess_->setVfatControl1(DACSel) ; // DAC selection OFF
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; dcuChannel.str("") ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(4, j) ;
	return (results.str());
      }
    }

    measurements << std::endl ;

    if(j==0){ results << "DAC TESTS:" << std::endl ; } 

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << testVfatIds_.at(j) << "\tOK " << SET_DEFAULT_COLORS << std::endl ; }
    results << testVfatIds_.at(j) << "\tOK " << std::endl ;

    if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
    if(outputfilestream.is_open()){
      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "WRITING DAC MEASUREMENTS TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
      outputfilestream << measurements.str() ;
      outputfilestream.close() ;
    }
  }

  if(!chipTesting_){ ledDACTest->setPixmap(greenLed_) ; }

  return (results.str()) ;
}

/** 
 * The Pulse Scan
 */
std::string vfat_controller::pulseOneChannelVfat () {
 
  std::ostringstream results ;
  std::ostringstream oss ;
  std::ofstream outputfilestream ;
  std::string fn = vfat_controller_home_ + VFAT_PATH_PULSE_CH ;
  std::string fn_data = vfat_controller_home_ + VFAT_PATH_PULSE_CH_DATA ;

  bool testOK = true ;
  unsigned int  numberOfCH = 0 ;
  int confNumber = 1 ;
  short int PulseThisChannel[VFAT_CH_NUM_MAX + 1] = {0};
  bool completeTag = true ;

  uint vcal1, vcal2, chipid0, chipid1 ;
  uint vcalstep = 1 ;

  int triggerCount = 0 ;

  int loop = 0 ;
  bool exceptionThrown = false ;

  std::vector<Totem::VFATFrame> data ;
  std::vector<std::string> noDataFiles ;

  // Read final changes from channel register fields
  if (range1->isChecked()){ getChannelRegisters(1) ; }
  else if (range2->isChecked()){ getChannelRegisters(2) ; }
  else if (range3->isChecked()){ getChannelRegisters(3) ; }
  else if (range4->isChecked()){ getChannelRegisters(4) ; }

  if ( sscanf (testPulseVCal1T->text(), "%d", &vcal1) && 
       sscanf (testPulseVCal2T->text(), "%d", &vcal2) ){

#ifdef BUSUSBFEC
    if(fecType_==FECUSB){

      /*
	data.reserve(1*testVfats_.size()*80*(vcal2-vcal1+1)/CHIPTEST_VCALSTEP) ;
	#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "SIZE RESERVED FOR DATA VECTOR: " << data.capacity() << SET_DEFAULT_COLORS << std::endl ;
	#endif
      */

      std::cout << SET_FORECOLOR_BLUE << "WRITING DATA FILE TO: " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
      writeDataFile(data, fn_data.c_str(), false) ;

      writeCrepeSettings(0) ;
      writeCrepeTrigger(0) ;
    }
#endif

#ifdef TTCCI_TRIGGER
    uint sequenceNumber = 0 ;
    std::string ttcci_seq = triggerSeqName->text().latin1() ;
    int ttcci_msleeptime = atoi(triggerSleepT->text()) ;
#endif

#ifdef VMEREADOUT

    fn_data = vfat_controller_home_ + VME_DATA_PULSESCAN ;

    totalBytesRead_ = 0 ;

    uint fiberEnable = 0x3fff ;
    sscanf (fiberEnableT->text(), "%x", &fiberEnable) ;
    int nRepetitions = atoi(triggerRepetitionsT->text()) ;

    std::cout << SET_FORECOLOR_CYAN << "nRepetitions=" << nRepetitions << ", fiberEnable=0x" << std::hex << fiberEnable << std::dec << SET_DEFAULT_COLORS << std::endl ;

    totfed_configure(fiberEnable) ;
    totfed_init() ; // empty fifos

    if (!vmeofstream_.is_open()){ vmeofstream_.open(fn_data.c_str()) ; }

#endif

    vfatAccess_ = NULL ;
    vfatd_ = NULL ;
    vfatDescription device ;

    if(testPulseAllChannelsSel->isChecked()){

      numberOfCH = VFAT_CH_NUM_MAX;

      for (int i=1;i<=VFAT_CH_NUM_MAX;i++) { PulseThisChannel[i] = VFAT_PULSE_FLAG ; }
    }

    // Logical OR for all the vfats' channels
    else {

      for( int i=1;i<=VFAT_CH_NUM_MAX;i++){

	for( int j=0;j<testVfatdValues_.size();j++) {

	  vfatd_ = testVfatdValues_.at(j) ;

	  uint value = vfatd_->getChanReg(i) & VFAT_CALPULSE_MASK ;

	  if(value != 0){

#ifdef DEBUG_VFAT_CONTROLLER
	    std::cout << SET_FORECOLOR_CYAN << std::dec << "Channel " << i << " selected." << SET_DEFAULT_COLORS << std::endl ; 
#endif
	    numberOfCH ++;
	    PulseThisChannel[i] = VFAT_PULSE_FLAG ;
	    break ;
	  }
	}
      }
    }

    // If no channel selected
    if(numberOfCH==0){ 

      if(showConsole_) { std::cout << SET_FORECOLOR_RED << "NO CHANNEL SELECTED, ABORTING." << SET_DEFAULT_COLORS << std::endl ; }
      results << "PULSING TEST WAS ABORTED, NO CHANNEL SELECTED." << std:: endl ;
      return(results.str()) ;
    }

    oss << "<?xml version=\"1.0\"?>" << std::endl ;
    oss << "<ROWSET>" << std::endl ;

    if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
    if(outputfilestream.is_open()){

      outputfilestream << oss.str() ;
      outputfilestream.close() ;
      oss.str("") ;
    } 

    //if(fecType_!=FECUSB){ oss << getXMLTagInit() ; }

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "PULSE TEST:" << SET_DEFAULT_COLORS << std::endl ; }
    results <<  "PULSE TEST:" << std::endl ;

#ifdef DEBUG_VFAT_CONTROLLER      
    std::cout << SET_FORECOLOR_CYAN << std::dec << "Number of channels to be pulsed: " << (int)numberOfCH << SET_DEFAULT_COLORS << std::endl ;
#endif

    deviceVector vDevices ;



    // Disable Cal OR ( CAL AND TrimDAC) bits on every chip
    for( int j=0;j<testVfats_.size();j++) {

      //vfatDescription vfatd ;
      vfatAccess_ = testVfats_.at(j) ;
      vfatd_ = testVfatdValues_.at(j) ;

      uint value = 0x00 ;

      for( int i=1;i<=VFAT_CH_NUM_MAX;i++) { 

	if(testPulseTrimDACSel->isChecked()){

	  value = vfatd_->getChanReg(i) & VFAT_INVERTED_MASKCH_MASK ;
	  vfatd_->setChanReg(i, value) ;
	}
	else{

	  //value = vfatd_->getChanReg(j) & VFAT_MASKCH_MASK ;
	  vfatd_->setChanReg(i, value) ;
	}

	loop = 0 ;
	do{

	  exceptionThrown = false ;
	  try{

	    loop++ ;
	    vfatAccess_->setVfatChanReg(i, value) ;
	  }
	  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	}
	while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	if(exceptionThrown){

	  results << testI2CError(5, j) ;
	  return (results.str());
	}
      }
      vDevices.push_back(vfatd_) ;
    }


    // Default tag for Monitor
    oss << getXMLTagDefault(vDevices) ;

    // Scan through channels
    for(unsigned int a=1;a<=VFAT_CH_NUM_MAX;a++){

      if ( PulseThisChannel[a] == VFAT_PULSE_FLAG ) {

	// Activate channel
	for( int j=0;j<testVfats_.size();j++) {

	  vfatAccess_ = testVfats_.at(j) ;
	  uint chanregvalue = VFAT_CALPULSE_MASK ;

	  loop = 0 ;
	  do{

	    exceptionThrown = false ;
	    try{

	      loop++ ;

	      vfatAccess_->setVfatChanReg(a, (tscType8)chanregvalue) ;	           
	    }
	    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	  }
	  while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	  if(exceptionThrown){

	    results << testI2CError(5, j) ;
	    return (results.str());
	  }
	}

	// Scan through VCAL range	  
	for(unsigned int i=vcal1;i<=vcal2;i+=vcalstep){

	  oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

	  for( int j=0;j<testVfats_.size();j++) {

	    vfatAccess_ = testVfats_.at(j) ;

	    uint chanregvalue = VFAT_CALPULSE_MASK ;
	    uint maskvalue = 0 , trimdacvalue = 0 ;



	    vfatd_ = testVfatdValues_.at(j) ;

	    if(testPulseTrimDACSel->isChecked()){

	      chanregvalue = vfatd_->getChanReg(a) | VFAT_CALPULSE_MASK ;
	      trimdacvalue = chanregvalue & VFAT_TRIMDAC_MASK ;
	      maskvalue = chanregvalue & VFAT_MASKCH_MASK ;
	    }
	    else{

	      chanregvalue = vfatd_->getChanReg(a) | VFAT_CALPULSE_MASK ;
	      chanregvalue &= VFAT_CALPULSE_MASKCH_MASK ;
	      maskvalue = chanregvalue & VFAT_MASKCH_MASK ;
	    }
	    
      
	    oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
		<< "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) // first 2 digits
		<< "\" chanreg" << (unsigned long)a << "=\"" << (unsigned long)chanregvalue
		<< "\" vcal=\"" << (unsigned long)i << "\" />" << std::endl ;

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatVCal((tscType8)i) ;	           
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }
	      
	    if(showConsole_) {

	      if(i==vcal1 && j==0){

		std::cout << SET_FORECOLOR_BLUE << std::dec << "Using VCal " << i << " / CH " << a << " / chanregvalue " << (int)chanregvalue ;

		if(testPulseTrimDACSel->isChecked()){ std::cout << " / TrimDAC " << trimdacvalue ; }

		if (maskvalue != 0) { std::cout << " *** MASKED ***" ; }
		std::cout << SET_DEFAULT_COLORS << std::endl ;
	      }
	    }
	  }

	  oss << "</configuration>" << std::endl ;
	  confNumber+=1 ;

	  if(fecType_==FECVME){

#ifdef TTCCI_TRIGGER
#ifdef VMEREADOUT
	    
	    ttcci_executeSeq( MY_TTCCI_RSYNC , ttcci_msleeptime ) ; // RSync

	    if(generateAndReadEvents( sequenceNumber, nRepetitions, ttcci_seq , ttcci_msleeptime ) != 0){

	      if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
	      std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	      return (results.str()) ;
	    }

	    sequenceNumber++ ;

#else
	    ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ;
#endif

#else
	    // External trigger
	    system (VFAT_TRIGGER_COMMAND) ;
#endif

	  }

#ifdef BUSUSBFEC
	  else if(fecType_==FECUSB){

	    resetCrepeFifos(0) ;
	    startCrepeTrigger(0) ;
	    readCrepeDataBuffers(0, data, noDataFiles, true) ; //false
	  }
#endif

	  triggerCount += 1 ;
	  //device.setChanReg(a, 0) ;
	}

	// Disable channel
	for( int j=0;j<testVfats_.size();j++) {

	  vfatAccess_ = testVfats_.at(j) ;
	  loop = 0 ;

	  do{

	    exceptionThrown = false ;
	    try{

	      loop++ ;
	      vfatAccess_->setVfatChanReg(a, (tscType8)0x00) ;  // Set no cal pulse
	    }
	    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	  }
	  while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	  if(exceptionThrown){

	    results << testI2CError(5, j) ;
	    return (results.str());
	  }
	}

	PulseThisChannel[a] = 0 ;
      }

#ifdef BUSUSBFEC
      // Save data file
      if(fecType_==FECUSB && a%1==0){
#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "WRITING FILE - DATA SIZE: " << data.size() << " FRAMES" << SET_DEFAULT_COLORS << std::endl ;
#endif
	writeDataFile(data, fn_data.c_str(), true) ;
	data.clear() ;

	if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  outputfilestream << oss.str() ;
	  outputfilestream.close() ;
	  oss.str("") ;
	} 
      }
#endif

      //if(a==10){ break ; }
    }
    
#ifdef DEBUG_VFAT_CONTROLLER      
    std::cout << SET_FORECOLOR_CYAN << std::dec << "Triggercommand given " << triggerCount << " times." << SET_DEFAULT_COLORS << std::endl ;
#endif      

    oss << "</ROWSET>" << std::endl ;

#ifdef VMEREADOUT
    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str(), std::ios::app) ; }
 
    if(outputfilestream.is_open()){

      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "WRITING CONFIGURATION TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
      outputfilestream << oss.str() ;
      outputfilestream.close() ;
    }    
  }
  else { std::cout << SET_FORECOLOR_RED << "One or several VFAT parameters are incorrect." << SET_DEFAULT_COLORS << std::endl ; }


  for( int j=0;j<testVfats_.size();j++) { results << testVfatIds_.at(j) << "\tOK " << std::endl ; }

  ledPulseOneChannelTest->setPixmap(greenLed_) ;
  

  return (results.str()) ;
}

/** 
 * This method checks dead channels.
 */
std::string vfat_controller::checkDeadChannelsVfat () {

  std::ostringstream results ;
  std::ostringstream oss ;
  std::ofstream outputfilestream ;
  std::string fn = vfat_controller_home_ + VFAT_PATH_DEAD_CH ;
  std::string fn_data = vfat_controller_home_ + TTP_DATA_DEADCHECK ;

  int confNumber = 1 ;
  bool completeTag = true ;

  int loop = 0 ;
  bool exceptionThrown = false ;

  vfatAccess_ = NULL ;
  vfatd_ = NULL ;
  deviceVector vDevices ;

  std::vector<Totem::VFATFrame> data ;
  std::vector<std::string> noDataFiles ;

#ifdef BUSUSBFEC
  if(fecType_==FECUSB){

    std::cout << SET_FORECOLOR_BLUE << "WRITING DATA FILE TO: " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
    writeDataFile(data, fn_data.c_str(), false) ;

    writeCrepeSettings(0) ;
    writeCrepeTrigger(0) ;
  }
#endif

#ifdef TTCCI_TRIGGER
  uint sequenceNumber = 0 ;
  std::string ttcci_seq = triggerSeqName->text().latin1() ;
  int ttcci_msleeptime = atoi(triggerSleepT->text()) ;
#endif

#ifdef VMEREADOUT

  fn_data = vfat_controller_home_ + VME_DATA_DEADCHECK ;

  totalBytesRead_ = 0 ;

  uint fiberEnable = 0x3fff ;
  sscanf (fiberEnableT->text(), "%x", &fiberEnable) ;
  int nRepetitions = atoi(triggerRepetitionsT->text()) ;

  std::cout << SET_FORECOLOR_CYAN << "nRepetitions=" << nRepetitions << ", fiberEnable=0x" << std::hex << fiberEnable << std::dec << SET_DEFAULT_COLORS << std::endl ;

  totfed_configure(fiberEnable) ;
  totfed_init() ; // empty fifos

  if (!vmeofstream_.is_open()){ vmeofstream_.open(fn_data.c_str()) ; }

#endif

  oss << "<?xml version=\"1.0\"?>" << std::endl ;
  oss << "<ROWSET>" << std::endl ;

  //if(fecType_!=FECUSB){ oss << getXMLTagInit() ; }

  for( int j=0;j<testVfats_.size();j++) {

    vfatd_ = testVfatdValues_.at(j) ;
    uint value = 0x00 ;

    for( int i=1;i<=VFAT_CH_NUM_MAX;i++) { vfatd_->setChanReg(i, value) ; }

    vDevices.push_back(vfatd_) ;
  }

  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "CHANNEL TEST:" << SET_DEFAULT_COLORS << std::endl ; }
  results <<  "CHANNEL TEST:" << std::endl ;

  oss << getXMLTagDefault(vDevices) ;

  for(int i=1;i<=VFAT_CH_NUM_MAX;i++){

    oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

    for( int j=0;j<testVfats_.size();j++) {

      oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
	  << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) // first 2 digits
	  << "\" chanreg" << (unsigned long)i << "=\"" << (unsigned long)VFAT_CALPULSE_MASK
	  << "\" vcal=\"" << (unsigned long)VFAT_VCAL_TEST_VALUE << "\" />" << std::endl ;

      vfatAccess_ = testVfats_.at(j) ;
      loop = 0 ;

      do{

	exceptionThrown = false ;
	try{

	  loop++ ;

	  if(i==1) { vfatAccess_->setVfatVCal((tscType8)VFAT_VCAL_TEST_VALUE) ; }// Set large Calipration Pulse
	  vfatAccess_->setVfatChanReg((tscType8)i, VFAT_CALPULSE_MASK) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(6, j) ;
	return (results.str());
      }	

      if(showConsole_ && j==0) { std::cout << SET_FORECOLOR_BLUE << std::dec << "Pulsing CH " << std::dec << i << ", using VCal " << VFAT_VCAL_TEST_VALUE << SET_DEFAULT_COLORS << std::endl ; }
    }

    if(fecType_==FECVME){

#ifdef TTCCI_TRIGGER
#ifdef VMEREADOUT
	    
      ttcci_executeSeq( MY_TTCCI_RSYNC , ttcci_msleeptime ) ; // RSync

      if(generateAndReadEvents( sequenceNumber, nRepetitions, ttcci_seq , ttcci_msleeptime ) != 0){

	if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
	std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	return (results.str()) ;
      }

      sequenceNumber++ ;

#else
      ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ;
#endif

#else
      // External trigger
      system (VFAT_TRIGGER_COMMAND) ;
#endif

    }

#ifdef BUSUSBFEC
    else if(fecType_==FECUSB){

      resetCrepeFifos(0) ;
      startCrepeTrigger(0) ;
      readCrepeDataBuffers(0, data, noDataFiles, false) ;
    }
#endif

    oss << "</configuration>" << std::endl ;
    confNumber+=1 ;
  
    for( int j=0;j<testVfats_.size();j++) {

      vfatAccess_ = testVfats_.at(j) ;
      loop = 0 ;

      do{

	exceptionThrown = false ;
	try{

	  loop++ ;
	  vfatAccess_->setVfatChanReg(i, (tscType8)0x00) ;  // Set no cal pulse, CHANGE TO MASK
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(6, j) ;
	return (results.str());
      }
    }

#ifdef BUSUSBFEC
    // Save data file
    if(fecType_==FECUSB && i%1==0){
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "WRITING FILE - DATA SIZE: " << data.size() << " FRAMES" << SET_DEFAULT_COLORS << std::endl ;
#endif
      writeDataFile(data, fn_data.c_str(), true) ;
      data.clear() ;
    }
#endif

  }

  oss << "</ROWSET>" << std::endl ;

#ifdef VMEREADOUT
    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
#endif

  if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
  if(outputfilestream.is_open()){

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "WRITING CONFIGURATION TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
    outputfilestream << oss.str() ;
    outputfilestream.close() ;
  }

  for( int j=0;j<testVfats_.size();j++) { results << testVfatIds_.at(j) << "\tOK " << std::endl ; }

  ledChannelTest->setPixmap(greenLed_) ;

  return (results.str()) ;
}

/** 
 * This method measures thresholds.
 */
std::string vfat_controller::thresholdMeasurementVfat () {

  std::ostringstream results ;
  std::ostringstream oss ;
  std::ofstream outputfilestream ;
  std::string fn = vfat_controller_home_ + VFAT_PATH_THRESHOLD ;
  std::string fn_data = vfat_controller_home_ + TTP_DATA_THRESHOLD ;

  int confNumber = 1 ;
  int vthreshold = 0 ;
  bool completeTag = true ;

  int loop = 0 ;
  bool exceptionThrown = false ;

  uint vthreshold1, vthreshold2, vthreshold12 ;

  std::vector<Totem::VFATFrame> data ;
  std::vector<std::string> noDataFiles ;

  if ( sscanf (testThresholdVThreshold1T->text(), "%d", &vthreshold1) && 
       sscanf (testThresholdVThreshold2T->text(), "%d", &vthreshold2) &&
       sscanf (testThresholdVThreshold12T->text(), "%d", &vthreshold12) ) {

    // Read final changes from channel register fields
    if (range1->isChecked()){ getChannelRegisters(1) ; }
    else if (range2->isChecked()){ getChannelRegisters(2) ; }
    else if (range3->isChecked()){ getChannelRegisters(3) ; }
    else if (range4->isChecked()){ getChannelRegisters(4) ; }

#ifdef BUSUSBFEC
    if(fecType_==FECUSB){

      std::cout << SET_FORECOLOR_BLUE << "WRITING DATA FILE TO: " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
      writeDataFile(data, fn_data.c_str(), false) ;

      writeCrepeSettings(0) ;
      writeCrepeTrigger(0) ;
    }
#endif

#ifdef TTCCI_TRIGGER
    uint sequenceNumber = 0 ;
    std::string ttcci_seq = triggerSeqName->text().latin1() ;
    int ttcci_msleeptime = atoi(triggerSleepT->text()) ;
#endif

#ifdef VMEREADOUT

    fn_data = vfat_controller_home_ + VME_DATA_THRESHOLD ;

    totalBytesRead_ = 0 ;

    uint fiberEnable = 0x3fff ;
    sscanf (fiberEnableT->text(), "%x", &fiberEnable) ;
    int nRepetitions = atoi(triggerRepetitionsT->text()) ;

    std::cout << SET_FORECOLOR_CYAN << "nRepetitions=" << nRepetitions << ", fiberEnable=0x" << std::hex << fiberEnable << std::dec << SET_DEFAULT_COLORS << std::endl ;

    totfed_configure(fiberEnable) ;
    totfed_init() ; // empty fifos

    if (!vmeofstream_.is_open()){ vmeofstream_.open(fn_data.c_str()) ; }

#endif
    
    oss << "<?xml version=\"1.0\"?>" << std::endl ;
    oss << "<ROWSET>" << std::endl ;

    // if(fecType_!=FECUSB){ oss << getXMLTagInit() ; }

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "THRESHOLD MEASUREMENT:" << SET_DEFAULT_COLORS << std::endl ; }
    results <<  "THRESHOLD MEASUREMENT:" << std::endl ;

    deviceVector vDevices ;

    for( int j=0;j<testVfats_.size();j++) {

      vfatd_ = testVfatdValues_.at(j) ;
      vDevices.push_back(vfatd_) ;
    }

    oss << getXMLTagDefault(vDevices) ;

    vthreshold = vthreshold2 - vthreshold1 ;

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << std::dec << "Threshold range: " << vthreshold12-vthreshold1 << " (" <<  vthreshold1 << " -  " << vthreshold12 << ")" << SET_DEFAULT_COLORS << std::endl ; }

    if(testThresholdInvertedSel->isChecked()){

      for (uint i=vthreshold1;i>=vthreshold12;i--){

	oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

	for(int j=0;j<testVfats_.size();j++) {

	  oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
	      << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) // first 2 digits
	      << "\" vthreshold1=\"" << (unsigned long)i
	      << "\" vthreshold2=\"" << (unsigned long)vthreshold2 << "\" />" << std::endl ;

	  vfatAccess_ = testVfats_.at(j) ;
	  loop = 0 ;

	  do{

	    exceptionThrown = false ;
	    try{

	      loop++ ;

	      //if(j==0){ vfatAccess_->setVfatVThreshold2((tscType8)vthreshold2) ; }
              if(i==vthreshold1){ vfatAccess_->setVfatVThreshold2((tscType8)vthreshold2) ; }

	      vfatAccess_->setVfatVThreshold1((tscType8)i) ;
	    }
	    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	  }
	  while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	  if(exceptionThrown){

	    results << testI2CError(7, j) ;
	    return (results.str());
	  }

	  if(showConsole_ && j==0) { std::cout << SET_FORECOLOR_BLUE << std::dec << "Threshold 1: " << i << SET_DEFAULT_COLORS << std::endl ; }

	}

	if(fecType_==FECVME){

#ifdef TTCCI_TRIGGER
#ifdef VMEREADOUT
	    
	  ttcci_executeSeq( MY_TTCCI_RSYNC , ttcci_msleeptime ) ; // RSync

	  if(generateAndReadEvents( sequenceNumber, nRepetitions, ttcci_seq , ttcci_msleeptime ) != 0){

	    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
	    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	    return (results.str()) ;
	  }

	  sequenceNumber++ ;

#else
	  ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ;
#endif

#else
	  // External trigger
	  system (VFAT_TRIGGER_COMMAND) ;
#endif
	}

#ifdef BUSUSBFEC
	else if(fecType_==FECUSB){

	  resetCrepeFifos(0) ;
	  startCrepeTrigger(0) ;
	  readCrepeDataBuffers(0, data, noDataFiles, false) ;
	}
#endif

	oss << "</configuration>" << std::endl ;
	confNumber+=1 ;

#ifdef BUSUSBFEC
	// Save data file
	if(fecType_==FECUSB && i%1==0){
#ifdef DEBUGMODE
	  std::cout << SET_FORECOLOR_CYAN << "WRITING FILE - DATA SIZE: " << data.size() << " FRAMES" << SET_DEFAULT_COLORS << std::endl ;
#endif
	  writeDataFile(data, fn_data.c_str(), true) ;
	  data.clear() ;
	}
#endif

      }
    }
    else{

      for (uint i=vthreshold1;i<=vthreshold12;i++){

	oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

	for( int j=0;j<testVfats_.size();j++) {

	  oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
	      << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) // first 2 digits
	      << "\" vthreshold1=\"" << (unsigned long)i
	      << "\" vthreshold2=\"" << (unsigned long)vthreshold2 << "\" />" << std::endl ;

	  vfatAccess_ = testVfats_.at(j) ;

	  loop = 0 ;
	  do{

	    exceptionThrown = false ;
	    try{

	      loop++ ;

	      //if(j==0){ vfatAccess_->setVfatVThreshold2((tscType8)vthreshold2) ; }
              if(i==vthreshold1){ vfatAccess_->setVfatVThreshold2((tscType8)vthreshold2) ; }

	      vfatAccess_->setVfatVThreshold1((tscType8)i) ;
	    }
	    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	  }
	  while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	  if(exceptionThrown){

	    results << testI2CError(7, j) ;
	    return (results.str());
	  }

	  if(showConsole_ && j==0) { std::cout << SET_FORECOLOR_BLUE << std::dec << "Threshold 1: " << i  << SET_DEFAULT_COLORS << std::endl ; }

	}

	if(fecType_==FECVME){

#ifdef TTCCI_TRIGGER
#ifdef VMEREADOUT
	    
	  ttcci_executeSeq( MY_TTCCI_RSYNC , ttcci_msleeptime ) ; // RSync

	  if(generateAndReadEvents( sequenceNumber, nRepetitions, ttcci_seq , ttcci_msleeptime ) != 0){

	    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
	    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	    return (results.str()) ;
	  }

	  sequenceNumber++ ;

#else
	  ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ;
#endif

#else
	  // External trigger
	  system (VFAT_TRIGGER_COMMAND) ;
#endif

	}

#ifdef BUSUSBFEC
	else if(fecType_==FECUSB){

	  resetCrepeFifos(0) ;
	  startCrepeTrigger(0) ;
	  readCrepeDataBuffers(0, data, noDataFiles, false) ;
	}
#endif

	oss << "</configuration>" << std::endl ;
	confNumber+=1 ;

#ifdef BUSUSBFEC
	// Save data file
	if(fecType_==FECUSB && i%1==0){
#ifdef DEBUGMODE
	  std::cout << SET_FORECOLOR_CYAN << "WRITING FILE - DATA SIZE: " << data.size() << " FRAMES" << SET_DEFAULT_COLORS << std::endl ;
#endif
	  writeDataFile(data, fn_data.c_str(), true) ;
	  data.clear() ;
	}
#endif
      }
    }

    oss << "</ROWSET>" << std::endl ;

#ifdef VMEREADOUT
    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
    if(outputfilestream.is_open()){

      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "WRITING CONFIGURATION TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
      outputfilestream << oss.str() ;
      outputfilestream.close() ;
    }
  }
  else { std::cout << SET_FORECOLOR_RED << "One or several VFAT parameters are incorrect." ; }

  for( int j=0;j<testVfats_.size();j++) { results << testVfatIds_.at(j) << "\tOK " << std::endl ; }

  ledThresholdMeasurementTest->setPixmap(greenLed_) ;

  return (results.str()) ;
}
/**
 * This method does the latency scan.
 */
std::string vfat_controller::latencyScanVfat () {

  std::ostringstream results ;
  std::ostringstream oss ;
  std::ofstream outputfilestream ;
  std::string fn = vfat_controller_home_ + VFAT_PATH_LATENCY ;
  std::string fn_data = vfat_controller_home_ + TTP_DATA_LATENCY ;

  int confNumber = 1 ;
  bool chFound = false ;

  int loop = 0 ;
  bool exceptionThrown = false ;
 
  uint lat1, lat2 ;

  std::vector<Totem::VFATFrame> data ;
  std::vector<std::string> noDataFiles ;

  if ( sscanf (testLatencyScanLatency1T->text(), "%d", &lat1) &&
       sscanf (testLatencyScanLatency2T->text(), "%d", &lat2) ) {

    // Read final changes from channel register fields
    if (range1->isChecked()){ getChannelRegisters(1) ; }
    else if (range2->isChecked()){ getChannelRegisters(2) ; }
    else if (range3->isChecked()){ getChannelRegisters(3) ; }
    else if (range4->isChecked()){ getChannelRegisters(4) ; }

#ifdef BUSUSBFEC
    if(fecType_==FECUSB){

      std::cout << SET_FORECOLOR_BLUE << "WRITING DATA FILE TO: " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
      writeDataFile(data, fn_data.c_str(), false) ;

      writeCrepeSettings(0) ;
      writeCrepeTrigger(0) ;
    }
#endif

#ifdef TTCCI_TRIGGER
    uint sequenceNumber = 0 ;
    std::string ttcci_seq = triggerSeqName->text().latin1() ;
    int ttcci_msleeptime = atoi(triggerSleepT->text()) ;
#endif

#ifdef VMEREADOUT

    fn_data = vfat_controller_home_ + VME_DATA_LATENCY ;

    totalBytesRead_ = 0 ;

    uint fiberEnable = 0x3fff ;
    sscanf (fiberEnableT->text(), "%x", &fiberEnable) ;
    int nRepetitions = atoi(triggerRepetitionsT->text()) ;

    std::cout << SET_FORECOLOR_CYAN << "nRepetitions=" << nRepetitions << ", fiberEnable=0x" << std::hex << fiberEnable << std::dec << SET_DEFAULT_COLORS << std::endl ;

    totfed_configure(fiberEnable) ;
    totfed_init() ; // empty fifos

    if (!vmeofstream_.is_open()){ vmeofstream_.open(fn_data.c_str()) ; }

#endif

    oss << "<?xml version=\"1.0\"?>" << std::endl ;
    oss << "<ROWSET>" << std::endl ;

    //if(fecType_!=FECUSB){ oss << getXMLTagInit() ; }

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "LATENCY SCAN:" << SET_DEFAULT_COLORS << std::endl ; }
    results <<  "LATENCY SCAN:" << std::endl ;

    deviceVector vDevices ;

    for( int j=0;j<testVfatdValues_.size();j++) {

      vfatd_ = testVfatdValues_.at(j) ;
   
      for( int i=1;i<=VFAT_CH_NUM_MAX;i++){

	uint value = vfatd_->getChanReg(i) & VFAT_CALPULSE_MASK ;

	if(value != 0){

#ifdef DEBUG_VFAT_CONTROLLER
	  std::cout << SET_FORECOLOR_CYAN << std::dec << "Channel " << i << " selected." << SET_DEFAULT_COLORS << std::endl ; 
#endif
	  chFound = true ;
	  break ;
	}

	// No channels activated -> active default ch
	if (i==VFAT_CH_NUM_MAX && !chFound) {

#ifdef DEBUG_VFAT_CONTROLLER
	  std::cout << SET_FORECOLOR_CYAN << std::dec << "VFAT " << testVfatIds_.at(j) << " - No channels activated -> Use default (" << VFAT_DEFAULT_CH << ")." << SET_DEFAULT_COLORS << std::endl ; 
#endif

	  vfatd_->setChanReg(VFAT_DEFAULT_CH, (uint)VFAT_CALPULSE_MASK) ;

	  vfatAccess_ = testVfats_.at(j) ;
	  loop = 0 ;

	  do{

	    exceptionThrown = false ;
	    try{

	      loop++ ;
	      vfatAccess_->setVfatChanReg(VFAT_DEFAULT_CH, (uint)VFAT_CALPULSE_MASK) ;
	    }
	    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	  }
	  while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	  if(exceptionThrown){

	    results << testI2CError(8, j) ;
	    return (results.str());
	  }
	}
      }

      vDevices.push_back(vfatd_) ;
    }

    oss << getXMLTagDefault(vDevices) ;

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << std::dec << "Starting with latency " << lat1 << SET_DEFAULT_COLORS << std::endl ; }
    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << std::dec << "Latency scan range: " << lat2-lat1 << " (" <<  lat1 << " -  " << lat2 << ")" << SET_DEFAULT_COLORS << std::endl ; }

    for (int i=lat1;i<=lat2;i++){

      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << std::dec << "Latency: " << i  << SET_DEFAULT_COLORS << std::endl ; }

      if(testLatencyScanAllPhasesSel->isChecked()) {

	for(int a=0;a<=VFAT_CALPHASE_NUM_MAX;a++){

	  oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

	  for( int j=0;j<testVfats_.size();j++) {

	    tscType8 calphase = 0x00 ;
	      
	    if (a==0){ calphase = VFAT_CALPHASE_0 ; }
	    if (a==1){ calphase = VFAT_CALPHASE_45 ; }
	    if (a==2){ calphase = VFAT_CALPHASE_90 ; }
	    if (a==3){ calphase = VFAT_CALPHASE_135 ; }
	    if (a==4){ calphase = VFAT_CALPHASE_180 ; }
	    if (a==5){ calphase = VFAT_CALPHASE_225 ; }
	    if (a==6){ calphase = VFAT_CALPHASE_270 ; }
	    if (a==7){ calphase = VFAT_CALPHASE_315 ; }
	      	      
#ifdef DEBUG_VFAT_CONTROLLER
	    std::cout << SET_FORECOLOR_CYAN << "CalPhase: " << std::dec <<(int)calphase << SET_DEFAULT_COLORS << std::endl ;
#endif

	    oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
		<< "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) // first 2 digits
		<< "\" lat=\"" << (unsigned long)i
		<< "\" calphase=\"" << (unsigned long)calphase << "\" />" << std::endl ;

	    vfatAccess_ = testVfats_.at(j) ;
	    loop = 0 ;

	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;

		if(a==0){ vfatAccess_->setVfatLat((tscType8)i) ; }

		vfatAccess_->setVfatCalPhase((tscType8)calphase) ; 
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(8, j) ;
	      return (results.str());
	    }	      
	  }

	  if(fecType_==FECVME){


#ifdef TTCCI_TRIGGER
#ifdef VMEREADOUT
	    
	    ttcci_executeSeq( MY_TTCCI_RSYNC , ttcci_msleeptime ) ; // RSync

	    if(generateAndReadEvents( sequenceNumber, nRepetitions, ttcci_seq , ttcci_msleeptime ) != 0){

	      if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
	      std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	      return (results.str()) ;
	    }

	    sequenceNumber++ ;

#else
	    ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ;
#endif

#else
	    // External trigger
	    system (VFAT_TRIGGER_COMMAND) ;
#endif
	  }

#ifdef BUSUSBFEC
	  else if(fecType_==FECUSB){

	    resetCrepeFifos(0) ;
	    startCrepeTrigger(0) ;
	    readCrepeDataBuffers(0, data, noDataFiles, false) ;
	  }
#endif

	  oss << "</configuration>" << std::endl ;
	  confNumber+=1 ;
	}
      }
      else{

	oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

	for( int j=0;j<testVfats_.size();j++) {

	  oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
	      << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) // first 2 digits
	      << "\" lat=\"" << i << "\" />" << std::endl ;

	  vfatAccess_ = testVfats_.at(j) ;
	  loop = 0 ;

	  do{

	    exceptionThrown = false ;
	    try{

	      loop++ ;
	      vfatAccess_->setVfatLat((tscType8)i) ;
	    }
	    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	  }
	  while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	  if(exceptionThrown){

	    results << testI2CError(8, j) ;
	    return (results.str());
	  }		    
	}

	if(fecType_==FECVME){

#ifdef TTCCI_TRIGGER
#ifdef VMEREADOUT 

	  ttcci_executeSeq( MY_TTCCI_RSYNC, ttcci_msleeptime ) ; // RSync

	  if(generateAndReadEvents( sequenceNumber, nRepetitions, ttcci_seq , ttcci_msleeptime ) != 0){

	    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
	    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	    return (results.str()) ;
	  }

	  sequenceNumber++ ; 

#else
	  ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ;
#endif

#else
	  // External trigger
	  system (VFAT_TRIGGER_COMMAND) ;
#endif
	}

#ifdef BUSUSBFEC
	else if(fecType_==FECUSB){

	  resetCrepeFifos(0) ;
	  startCrepeTrigger(0) ;
	  readCrepeDataBuffers(0, data, noDataFiles, false) ;
	}
#endif

	oss << "</configuration>" << std::endl ;
	confNumber+=1 ;
      }

#ifdef BUSUSBFEC
      // Save data file
      if(fecType_==FECUSB && i%1==0){
#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "WRITING FILE - DATA SIZE: " << data.size() << " FRAMES" << SET_DEFAULT_COLORS << std::endl ;
#endif
	writeDataFile(data, fn_data.c_str(), true) ;
	data.clear() ;
      }
#endif

    }

    oss << "</ROWSET>" << std::endl ;

#ifdef VMEREADOUT
    if (vmeofstream_.is_open()){ vmeofstream_.close() ; }
    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << totalBytesRead_ << " BYTES TO " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
    if(outputfilestream.is_open()){

      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "WRITING CONFIGURATION TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
      outputfilestream << oss.str() ;
      outputfilestream.close() ;
    }
  }
  else { std::cout << SET_FORECOLOR_RED << "One or several VFAT parameters are incorrect" << SET_DEFAULT_COLORS << std::endl ; }

  for( int j=0;j<testVfats_.size();j++) { results << testVfatIds_.at(j) << "\tOK " << std::endl ; }

  ledLatencyScanTest->setPixmap(greenLed_) ;

  return (results.str()) ;
}

/** 
 * This method writes defaults to chip.
 */
void vfat_controller::setDefaultsVfat (){

  vfatDescription vfatd ;
  vfatd.setDescriptionDefaultValues() ;

  for( int i=0;i<testVfats_.size();i++) {

    vfatAccess_ = testVfats_.at(i) ;

    int loop = 0 ;
    bool exceptionThrown = false ;

    do{

      exceptionThrown = false ;

      try{

	loop++ ;
	vfatAccess_->setVfatValues(vfatd) ;
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      failedTest_ = "Write Defaults" ;
      i2cError_ = true ;
      testInitOK_ = false ;
      return ;
    }
  }
}

std::string vfat_controller::getFullConfigInfo(){

  int loop = 0 ;
  bool exceptionThrown = false ;

  vfatAccess *vfat ;
  vfatDescription *vfatd ;
  deviceVector vDevices ;

  std::ostringstream oss ;

  for (int i=0;i<vfatAddresses_.size();i++){

    loop = 0 ;
    do{

      exceptionThrown = false ;
      try{

	loop++ ;
	vfat = (vfatAccess *)fecAccessManager_->getAccess(VFAT, vfatAddresses_.at(i)) ;
	vfatd = vfat->getVfatValues() ;

      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      failedTest_ = "Get init configurations" ;
      i2cError_ = true ;
      testInitOK_ = false ;
      return (oss.str()) ;
    }

    vDevices.push_back(vfatd) ;  
  }

  TotemMemBufOutputSource memBuf(vDevices) ;
  oss << memBuf.getVfatOutputBuffer()->str() ;

  return (oss.str()) ;
}

/** 
 * This method generates default tag, which includes the initial values
 */
std::string vfat_controller::getXMLTagInit() {

  std::ostringstream oss ;

  // Config for all the chips
  oss << "<configuration default=\"0\">" << std::endl ;
  oss << getFullConfigInfo() ;
  oss << "</configuration>" << std::endl ;
 
  return oss.str() ;
}

/** 
 * This method generates default tag, which includes the start values
 */
std::string vfat_controller::getXMLTagDefault(deviceVector vDevice) {

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "VECTOR SIZE: " << vDevice.size() << SET_DEFAULT_COLORS << std::endl ;

  for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {

    deviceDescription *deviced = *device ;
    vfatDescription *vfatDevice = NULL ;

    vfatDevice = (vfatDescription *)deviced ;

    std::cout << SET_FORECOLOR_CYAN << "Description values loaded from XML file" << SET_DEFAULT_COLORS << std::endl ;

    std::cout << SET_FORECOLOR_CYAN << " Position FEC ("
	      << std::hex << (int)vfatDevice->getFecSlot() << ","
	      << std::hex << (int)vfatDevice->getRingSlot() << ") CCU 0x" 
	      << std::hex << (int)vfatDevice->getCcuAddress() << " CHANNEL 0x" 
	      << std::hex << (int)vfatDevice->getChannel() << " ADDRESS 0x" 
	      << std::hex << (int)vfatDevice->getAddress()
	      << std::dec << SET_DEFAULT_COLORS << std::endl ;
  }
#endif

  // Default config for Monitor
  std::ostringstream oss ;
  oss << "<configuration default=\"1\">" << std::endl ;
  TotemMemBufOutputSource memBuf(vDevice) ;
  oss << memBuf.getVfatOutputBuffer()->str() ;
  oss << "</configuration>" << std::endl ;
 
  return oss.str() ;
}

/** 
 * This method saves the configuration info for all the vfats
 */
void vfat_controller::saveConfigFile(){

  std::ostringstream oss ;
  std::ofstream outputfilestream ;
  std::string fn = vfat_controller_home_ + VFAT_PATH_CONFIG ;

  oss << "<?xml version=\"1.0\"?>" << std::endl ;
  oss << "<ROWSET " << COMMON_XML_SCHEME << ">" << std::endl ;
  oss << getFullConfigInfo() ;
  oss << "</ROWSET>" << std::endl ;

  if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
  if(outputfilestream.is_open()){

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "WRITING CONFIGURATION TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
    outputfilestream << oss.str() ;
    outputfilestream.close() ;
  }
}

/** 
 * This method saves the Monitor configuration file for all vfats
 */
void vfat_controller::saveMonitorConfigFile( std::vector <std::string> chipids ){

  std::ofstream outputfilestream ;
  std::ostringstream oss ;
  std::string fn = vfat_controller_home_ + VFAT_PATH_MONITOR_CONFIG ;

  oss << "<top>" << std::endl ;

  for(int i=0;i<chipids.size();i++){

    oss << "\t<test_vfat id=\"" << chipids.at(i) << "\" verbose=\"" << MONITOR_VERBOSE << "\" />" << std::endl ;
  }

  oss << "</top>" << std::endl ;

  if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
  if(outputfilestream.is_open()){

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "WRITING MONITOR CONFIGURATION TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
    outputfilestream << oss.str() ;
    outputfilestream.close() ;
  }
}


#ifdef TTCCI_TRIGGER

/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   TTCci Methods                                                                     */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */ 

void vfat_controller::ttcci_configure( std::string fn ){

  std::ifstream ifstream(fn.c_str());

  if (!ifstream){
    std::cout << SET_FORECOLOR_RED << " Unable to open file '" << fn <<"'!" << SET_DEFAULT_COLORS << std::endl ;
    return ;
  }

  if(ttcci_!=NULL){

    try{

      ttcci_->Configure(ifstream) ;
    }
    catch (...) {

      std::cout << SET_FORECOLOR_RED << "Exception while configuring the TTCci board." << SET_DEFAULT_COLORS << std::endl ;
    }
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TTCci board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }
}

void vfat_controller::ttcci_executeSeq( std::string seq , int msleeptime ){

  if(ttcci_!=NULL){

    try{

      ttcci_->ExecuteSequence(seq.c_str()) ;
      msleep(msleeptime) ;
    }
    catch (...) {

      std::cout << SET_FORECOLOR_RED << "Exception while executing the TTCci trigger sequence." << SET_DEFAULT_COLORS << std::endl ;
    }
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TTCci board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }
}

void vfat_controller::ttcci_setIntTrigger(){

  if(ttcci_!=NULL){

    try{

      std::vector<ttc::ExternalInterface> vTrigger ;
      vTrigger.push_back(ttc::CYCLIC) ;

      if (!ttcci_->IsSourceSelected("Trigger", ttc::CYCLIC)){ ttcci_->SelectTrigger(vTrigger) ; }

      if(ttcci_->IsSourceSelected("Trigger",ttc::CYCLIC)){

	std::cout << SET_FORECOLOR_BLUE << "The TTCci trigger mode is set to internal (CYCLIC)." << SET_DEFAULT_COLORS << std::endl ;  
      }
    }
    catch (...) {

      std::cout << SET_FORECOLOR_RED << "Exception while setting the TTCci trigger mode." << SET_DEFAULT_COLORS << std::endl ;
    }
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TTCci board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }

}

void vfat_controller::ttcci_setExtTrigger(){

  if(ttcci_!=NULL){

    try{

      std::vector<ttc::ExternalInterface> vTrigger ;
      vTrigger.push_back(ttc::FRONTP1) ;

      if (!ttcci_->IsSourceSelected("Trigger", ttc::FRONTP1)){ ttcci_->SelectTrigger(vTrigger) ; }

      if(ttcci_->IsSourceSelected("Trigger",ttc::FRONTP1)){

	std::cout << SET_FORECOLOR_BLUE << "The TTCci trigger mode is set to external (FRONTP1)." << SET_DEFAULT_COLORS << std::endl ;  
      }
    }
    catch (...) {

      std::cout << SET_FORECOLOR_RED << "Exception while setting the TTCci trigger mode." << SET_DEFAULT_COLORS << std::endl ;
    }
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TTCci board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }

}

#endif

#ifdef VMEREADOUT

/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   TOTFED Methods                                                                    */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */

void vfat_controller::totfed_reset(){

  if(totfed_!=NULL){

    try{
    
      totfed_->reset() ;
    }
    catch( HAL::BusAdapterException &e ){ std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << SET_DEFAULT_COLORS << e.what() << std::endl ; }
    catch(...){ std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ; }
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TOTFED board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }

}

void vfat_controller::totfed_init(){

  std::cout << SET_FORECOLOR_BLUE << "Emptying the TOTFED data FIFOs." << SET_DEFAULT_COLORS << std::endl ;

  if(totfed_!=NULL){

    try{

      for(int i=0;i<2;i++){

	totfed_->hardwareReadBlock( myAddressTable_.getGeneralHardwareAddress(ITEM_FED_MAIN1), TOTFED_READOUT_BUFFER_SIZE, vmeDataBuffer_, HAL::HAL_DO_INCREMENT ) ;
      }
    }
    catch( HAL::BusAdapterException &e ){
      std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
      return ;
    }
    catch(...){
      std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
      return ;
    }

    for(int i=1;i<=TOTFED_READOUT_BUFFER_SIZE;i++){ vmeDataBuffer_[i] = '0' ; }
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TOTFED board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }

}

void vfat_controller::totfed_configure( uint fiberPattern ){

  if(totfed_!=NULL){

    try{

      totfed_->setOptoRxFiberEnable(fiberPattern) ;
      //totfed_->configure(fiberPattern) ;
    }
    catch( HAL::BusAdapterException &e ){ std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << SET_DEFAULT_COLORS << e.what() << std::endl ; }
    catch(...){ std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ; }
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TOTFED board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }
}

uint vfat_controller::generateAndReadEvents( uint sequenceNumber, int nRepetitions, std::string seq, int msleeptime ){

  if(totfed_!=NULL){

    uint vmeheadervalue = TOTFED_HEADER | (sequenceNumber << 16) ;
    char *vmeheader = (char *)&vmeheadervalue ;

    uint32_t word_count1 = 0, word_count2 = 0, word_count3 = 0, bytes_to_read = 0 ;

    HAL::VMEHardwareAddress totfedDataAddr((myAddressTable_.getGeneralHardwareAddress(ITEM_FED_MAIN1)).getAddress(),
					   (myAddressTable_.getGeneralHardwareAddress(ITEM_FED_MAIN1)).getAddressModifier(),
					   (myAddressTable_.getGeneralHardwareAddress(ITEM_FED_MAIN1)).getDataWidth()) ;

    HAL::VMEHardwareAddress totfedWordCountAddr((myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_DATA_WORD_COUNT)).getAddress(),
						(myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_DATA_WORD_COUNT)).getAddressModifier(),
						(myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_DATA_WORD_COUNT)).getDataWidth()) ;

    // Generate and read the triggers requested
    for(int i=0;i<nRepetitions;i++){

      // Write software header ( B<sequence number, 12 bits><trigger number 16> )
      vmeheadervalue=(vmeheadervalue & 0xFFFF0000) | i ;
      if(vmeofstream_.is_open()){ vmeofstream_.write(vmeheader, sizeof(vmeheader)) ; }
      totalBytesRead_+=sizeof(vmeheader) ;
    
#ifdef TTCCI_TRIGGER
      ttcci_executeSeq( seq , msleeptime ) ; // Execute the trigger sequence
#endif

      try{

	totfed_->pollItem( ITEM_MAIN1_DATA_WORD_COUNT, TOTFED_POLL_REF, TOTFED_POLL_TIMEOUT_MS, &word_count1, HAL::HAL_POLL_UNTIL_DIFFERENT ) ;
      }
      catch( HAL::BusAdapterException &e ){
	std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	return TOTFED_READOUT_ERROR ;
      }
      catch( HAL::TimeoutException &e ){
	std::cout << SET_FORECOLOR_RED << "HAL::TimeoutException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	return TOTFED_READOUT_ERROR ;
      }
      catch(...){
	std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	return TOTFED_READOUT_ERROR ;
      }


      // Double check the counter
      try{

	totfed_->hardwareRead( totfedWordCountAddr, &word_count2) ;
      }
      catch( HAL::BusAdapterException &e ){
	std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	return TOTFED_READOUT_ERROR ;
      }
      catch(...){
	std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	return TOTFED_READOUT_ERROR ;
      }


      if(word_count1!=word_count2){

	// Check for the 3rd and last time...
	try{

	  totfed_->hardwareRead( totfedWordCountAddr, &word_count3) ;
	}
	catch( HAL::BusAdapterException &e ){
	  std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	  return TOTFED_READOUT_ERROR ;
	}
	catch(...){
	  std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	  return TOTFED_READOUT_ERROR ;
	}
      }


      if(word_count1>0 && (word_count1==word_count2 || word_count2==word_count3)){

	bytes_to_read = word_count2*4 ; // Convert to bytes

	// TO ENSURE TO EMPTY THE FIFOS - BUG IN THE FED'S WORD COUNT !!!
	if(word_count2>TOTFED_EVENT_SIZE_WORDS){ bytes_to_read += (word_count2-TOTFED_EVENT_SIZE_WORDS)*5*4 ; }

	if(word_count2!=TOTFED_EVENT_SIZE_WORDS){

	  std::cout << SET_FORECOLOR_CYAN << "WARNING: WORD COUNT READ " << word_count2 << " ( NOT " << TOTFED_EVENT_SIZE_WORDS << ")" << SET_DEFAULT_COLORS << std::endl ;

	  if(bytes_to_read>TOTFED_READOUT_BUFFER_SIZE){

	    std::cout << SET_FORECOLOR_RED << "The amount of data cannot fit to software readout buffer (" << bytes_to_read << "/" << TOTFED_READOUT_BUFFER_SIZE << "). Aborting..." << SET_DEFAULT_COLORS << std::endl ;
	    return TOTFED_READOUT_ERROR ;

	  }
	}

	try{

	  totfed_->hardwareReadBlock( totfedDataAddr, bytes_to_read, vmeDataBuffer_, HAL::HAL_DO_INCREMENT ) ; // size in bytes
	}
	catch( HAL::BusAdapterException &e ){
	  std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	  return TOTFED_READOUT_ERROR ;
	}
	catch(...){
	  std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	  return TOTFED_READOUT_ERROR ;
	}

	totalBytesRead_ += bytes_to_read ;

	if (vmeofstream_.is_open()){ vmeofstream_.write(vmeDataBuffer_, bytes_to_read) ; } // size in bytes

      }
      else{

	std::cout << SET_FORECOLOR_RED << "Word count not OK, 1st value=" << word_count1 << ", 2nd value=" << word_count2 << ", 3rd value=" << word_count3 << SET_DEFAULT_COLORS << std::endl ;
	return TOTFED_READOUT_ERROR ;
      }
    }
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TOTFED board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
    return TOTFED_READOUT_ERROR ;
  }

  return 0 ;
}

/** 
 * This method is for reading the data using external trigger.
 */
void vfat_controller::dataTakingExtTrigger(){

  if(totfed_!=NULL){

    uint32_t poll_timeout = TOTFED_POLL_TIMEOUT_EXT_MS ;
    uint32_t word_count1 = 0, word_count2 = 0, word_count3 = 0, bytes_to_read = 0, total_bytes_read = 0 ;
    int ntrig = 0 ;

    bool abort = false ;

    //uint vmeheadervalue = TOTFED_HEADER | (sequenceNumber << 16) ;
    uint vmeheadervalue = TOTFED_HEADER ;
    char *vmeheader = (char *)&vmeheadervalue ;

    HAL::VMEHardwareAddress totfedDataAddr((myAddressTable_.getGeneralHardwareAddress(ITEM_FED_MAIN1)).getAddress(),
					   (myAddressTable_.getGeneralHardwareAddress(ITEM_FED_MAIN1)).getAddressModifier(),
					   (myAddressTable_.getGeneralHardwareAddress(ITEM_FED_MAIN1)).getDataWidth()) ;


    HAL::VMEHardwareAddress totfedWordCountAddr((myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_DATA_WORD_COUNT)).getAddress(),
						(myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_DATA_WORD_COUNT)).getAddressModifier(),
						(myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_DATA_WORD_COUNT)).getDataWidth()) ;


    std::ofstream vmeofstream ;
    std::string fn_data = vfat_controller_home_ + VME_DATA ;

    if (!vmeofstream.is_open()){ vmeofstream.open(fn_data.c_str()) ; }
    std::cout << SET_FORECOLOR_BLUE << "\nStart the readout. Data will be saved to " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;

#ifdef TTCCI_TRIGGER

    // Disable Trigger

    ttcci_setIntTrigger() ; // Internal trigger mode

#endif

    // Configure TOTFED

    uint fiberEnable = 0x3fff ;
    sscanf (fiberEnableT->text(), "%x", &fiberEnable) ;

#ifdef OPTORX_DATAEMU
#ifdef VME_TRIGGER

    poll_timeout = TOTFED_POLL_TIMEOUT_MS ;
    uint32_t triggerPattern = 0xffff ;

    HAL::VMEHardwareAddress vmeTriggerAddr((myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_OPTORX_COUNTER_RESET)).getAddress(),
					   (myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_OPTORX_COUNTER_RESET)).getAddressModifier(),
					   (myAddressTable_.getGeneralHardwareAddress(ITEM_MAIN1_OPTORX_COUNTER_RESET)).getDataWidth()) ;

    if((fiberEnable & 0x8000) != 0x8000){

      fiberEnable |= 0x8000 ;
      std::cout << SET_FORECOLOR_BLUE << "The \"Fiber enable\" pattern doesn't have the data emulation bit on, adding it now... value will be set to 0x" << std::hex << fiberEnable << std::dec << SET_DEFAULT_COLORS << std::endl ;
    }

#endif
#ifdef TTCCI_TRIGGER

    if((fiberEnable & 0x9000) != 0x9000){

      fiberEnable |= 0x9000 ;
      std::cout << SET_FORECOLOR_BLUE << "The \"Fiber enable\" pattern doesn't have the data emulation and/or TTCci trigger bits on, adding it now... value will be set to 0x" << std::hex << fiberEnable << std::dec << SET_DEFAULT_COLORS << std::endl ;
    }

#endif
#endif

    totfed_configure(fiberEnable) ; // Enable active fibres
    totfed_init() ;                 // Read the data fifos empty

#ifdef OPTORX_DATAEMU

    // Reset OptoRx fifos and logic
    try{

      totfed_->resetOptoRx() ;
    }
    catch( HAL::BusAdapterException &e ){
      std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
      abort=true ;
    }
    catch(...){
      std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
      abort=true ;
    }

#endif


#ifdef TTCCI_TRIGGER

    ttcci_executeSeq(MY_TTCCI_RSYNC) ; // RSync

#ifdef OPTORX_DATAEMU

    std::string ttcci_seq = triggerSeqName->text().latin1() ;
    int ttcci_msleeptime = atoi(triggerSleepT->text()) ;

#else

    ttcci_setExtTrigger() ; // External trigger mode (enable trigger)

#endif

#endif


    while (!abort) {  

      word_count1 = 0 ; word_count2 = 0 ; word_count3 = 0 ; bytes_to_read = 0 ;


#ifdef OPTORX_DATAEMU
#ifdef TTCCI_TRIGGER

      ttcci_executeSeq( ttcci_seq , ttcci_msleeptime ) ; // Execute the trigger sequence

#elif VME_TRIGGER

      // Send VME trigger
      try{

	totfed_->hardwareWrite( vmeTriggerAddr, triggerPattern) ;
      }
      catch( HAL::BusAdapterException &e ){
	std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	abort=true ;
	break ;
      }
      catch(...){
	std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	abort=true ;
	break ;
      }

#endif
#endif

      try{

	totfed_->pollItem( ITEM_MAIN1_DATA_WORD_COUNT, TOTFED_POLL_REF, poll_timeout, &word_count1, HAL::HAL_POLL_UNTIL_DIFFERENT ) ;
      }
      catch( HAL::BusAdapterException &e ){
	std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	abort=true ;
	break ;
      }
      catch( HAL::TimeoutException &e ){
	std::cout << SET_FORECOLOR_RED << "HAL::TimeoutException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	abort=true ;
      }
      catch(...){
	std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	abort=true ;
	break ;
      }


      // Double check the counter
      try{

	totfed_->hardwareRead( totfedWordCountAddr, &word_count2) ;
      }
      catch( HAL::BusAdapterException &e ){
	std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	abort=true ;
	break ;
      }
      catch(...){
	std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	abort=true ;
	break ;
      }


      if(word_count1!=word_count2){

	// Check for the 3rd and last time...
	try{

	  totfed_->hardwareRead( totfedWordCountAddr, &word_count3) ;
	}
	catch( HAL::BusAdapterException &e ){
	  std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	  abort=true ;
	  break ;
	}
	catch(...){
	  std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	  abort=true ;
	  break ;
	}
      }


      if(word_count1>0 && (word_count1==word_count2 || word_count2==word_count3)){

	bytes_to_read = word_count2*4 ; // Convert to bytes

	// TO ENSURE TO EMPTY THE FIFOS - BUG IN THE FED'S WORD COUNT !!!
	if(word_count2>TOTFED_EVENT_SIZE_WORDS){ bytes_to_read += (word_count2-TOTFED_EVENT_SIZE_WORDS)*5*4 ; }

	if(word_count2!=TOTFED_EVENT_SIZE_WORDS){

	  std::cout << SET_FORECOLOR_CYAN << "WARNING: WORD COUNT READ " << word_count2 << " ( NOT " << TOTFED_EVENT_SIZE_WORDS << ")" << SET_DEFAULT_COLORS << std::endl ;

	  if(bytes_to_read>TOTFED_READOUT_BUFFER_SIZE){

	    std::cout << SET_FORECOLOR_RED << "The amount of data cannot fit to software readout buffer (" << bytes_to_read << "/" << TOTFED_READOUT_BUFFER_SIZE << "). Aborting..." << SET_DEFAULT_COLORS << std::endl ;
	    abort=true ;
	    break ;

	  }
	}

	try{

	  totfed_->hardwareReadBlock( totfedDataAddr, bytes_to_read, vmeDataBuffer_, HAL::HAL_DO_INCREMENT ) ; // size in bytes
	}
	catch( HAL::BusAdapterException &e ){
	  std::cout << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ;
	  abort=true ;
	  break ;
	}
	catch(...){
	  std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ;
	  abort=true ;
	  break ;
	}


	if (vmeofstream.is_open()){

	  // Write software header ( B<sequence number, 12 bits><trigger number 16> ), in this case a single counter...
	  vmeheadervalue=(vmeheadervalue & 0xF0000000) | ntrig ;
	  vmeofstream.write(vmeheader, sizeof(vmeheader)) ;

	  vmeofstream.write(vmeDataBuffer_, bytes_to_read) ;// size in bytes
	}

	total_bytes_read += bytes_to_read ;
	ntrig++ ;

	std::cout << SET_FORECOLOR_BLUE << ntrig << " events read."<< SET_DEFAULT_COLORS << std::endl ;
      }
      else{

	std::cout << SET_FORECOLOR_RED << "Word count not OK, 1st value=" << word_count1 << ", 2nd value=" << word_count2 << ", 3rd value=" << word_count3 << SET_DEFAULT_COLORS << std::endl ;
	abort=true ;
      }

      if(abort_){ abort = true ; } // Stop run pressed from GUI
    }

    if (vmeofstream.is_open()){ vmeofstream.close() ; }
    std::cout << SET_FORECOLOR_BLUE << "\nWROTE " << total_bytes_read << " BYTES TO " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;

#ifdef TTCCI_TRIGGER
#ifndef OPTORX_DATAEMU

    ttcci_setIntTrigger() ; // Internal trigger mode

#endif
#endif

  }
  else{

    std::cout << SET_FORECOLOR_RED << "TOTFED board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }
}
#endif

/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   Thread Methods                                                                    */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */ 

/** Start the thread
 */
void vfat_controller::testStart_pressed(){

  chipTesting_ = false ;
  bool start = false ;
  short int vfatsToTest = 0 ;

  // Set the global abort flag
  if(threadRunning_){

    abort_ = true ;
  }
  else{

    abort_ = false ;

    // Check if VFAT(S) selected
    for (int i=0;i<vfatAddresses_.size();i++){

      if (vfatList_test->isSelected(i)){ start = true ; vfatsToTest++ ; }
    }

#ifdef DEBUG_VFAT_CONTROLLER  
    std::cout << SET_FORECOLOR_CYAN << std::dec << "VFATS SELECTED: " << vfatsToTest << " / " << vfatAddresses_.size() << SET_DEFAULT_COLORS << std::endl ;
#endif

    // Data taking with external trigger
    if (testDataTakingSel->isChecked()){ dataRun_ = true ; }
    else { dataRun_ = false ; }

#ifdef TTCCI_TRIGGER
    // Configure the TTCci board
    std::string configfile = ttcciConfigFile->text().latin1() ;
    ttcci_configure( configfile ) ;
#endif

  }
  
  if(threadRunning_ || dataRun_ || (start && testCheckValues())){

    //Start thread if not already running, otherwise terminate
    if(testStart->text()=="Start" && !threadRunning_){

      threadRunning_ = true ;
      std::cout << SET_FORECOLOR_BLUE << "SCANNING STARTED" << SET_DEFAULT_COLORS << std::endl ;
      testStart->setText("Scan Running...") ;
      this->start() ;
      this->wait(THREAD_WAIT_TIME) ;
    }
    else{

      if(dataRun_){

#ifdef VMEREADOUT 

	std::cout << SET_FORECOLOR_BLUE << "DATA TAKING STOPPING AFTER NEXT TRIGGER" << SET_DEFAULT_COLORS << std::endl ;

#else

	std::cout << SET_FORECOLOR_BLUE << "STOPPING THE DATA TAKING" << SET_DEFAULT_COLORS << std::endl ;

#endif

      }
      else{

	std::cout << SET_FORECOLOR_RED << "SCANNING ABORTED" << SET_DEFAULT_COLORS << std::endl ;
	if(rememberSettingsSel->isChecked() && !dataRun_){ testWriteBackVfatValues() ; }
	msleep(1000) ;
	this->terminate() ;
	this->wait(THREAD_WAIT_TIME) ;
      }

      testStart->setText("Start") ;
      testCleanUp() ;
      threadRunning_ = false ;
    }
  }
  else { ErrorMessage ("NO VFATS SELECTED -> ABORTING..." ) ; }
}

/** Thread's run method
 *
 */
void vfat_controller::run() {

  struct timeval time1, time2;
  struct timezone zone1, zone2;

  bool showBuffer = true ;
  showConsole_ = true ;

  bool advChipTesting = false ;
  shortDACScan_ = false ;
  bool useLoop = false ;
  double loopTime = 0 ; 

  std::stringstream finalResults ;
  std::ofstream outputfilestream ;
  //std::ifstream inputfilestream ;
  std::string fn = vfat_controller_home_ + VFAT_PATH_SUMMARY ;

  i2cError_ = false ;
  failedTest_ = "" ;
  abortChipTesting_ = false ;
  abort_ = false ;

  if(!chipTesting_){

    // Show results in console ?
    if (resultsConsoleSel->isChecked()){ showConsole_ = true ; }
    else { showConsole_ = false ; }

    // Print output file to console ?
    if (printTextFileSel->isChecked()){ showBuffer = true ; }
    else { showBuffer = false ; }

    // Reset test panel's LEDs
    //ledPowerOnTest->setPixmap(blackLed_) ;
    ledDataTaking->setPixmap(blackLed_) ;
    ledPosMapping->setPixmap(blackLed_) ;
    ledDACTest->setPixmap(blackLed_) ;
    ledPulseOneChannelTest->setPixmap(blackLed_) ;
    ledChannelTest->setPixmap(blackLed_) ;
    ledThresholdMeasurementTest->setPixmap(blackLed_) ;
    ledLatencyScanTest->setPixmap(blackLed_) ;
  }
  else{

    if(!advancedChipTesting->isChecked()){

#ifdef BUSUSBFEC
      if(fecType_==FECUSB){ crepeMasterReset(0) ; }
#endif

      scanAll_pressed() ;
    }

    fn = vfat_controller_home_ + CHIPTEST_DATA + CHIPTEST_SUMMARY ;

    // Display chip test panel's LED
    ledChipTest->setPixmap(blackLed_) ;
  }

  try{


    if(dataRun_){

#ifdef VMEREADOUT 

      dataTakingExtTrigger() ;

#elif BUSUSBFEC

      if(fecType_==FECUSB){ crepeDataTakingExtTrigger(0) ; }

#endif

    }
    else{

      try{

	initVfatTest() ; // Create the VFAT accesses
      }
      catch (FecExceptionHandler &e) {

	std::cout << SET_FORECOLOR_RED << "------------ Exception ----------" << std::endl ;
	std::cout << e.what()  << std::endl ;
	std::cout << "---------------------------------" << SET_DEFAULT_COLORS << std::endl ;
      }

      if(testInitOK_){

	gettimeofday(&time1, &zone1) ; // Timestamp of test start

	// Normal scan procedure
	if(!chipTesting_){

#ifndef VMEREADOUT 
	  // Reset on OptoRx
	  if(fecType_==FECVME){ 

	    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "Performing reset on OptoRx" << SET_DEFAULT_COLORS << std::endl ; }
	    system(VFAT_RESET_OPTORX_COMMAND) ;
	  }
#endif

	  //if(!i2cError_){ saveConfigFile() ; }

	  if(!i2cError_){ testSaveVfatValues() ; }

	  if(showConsole_) { 

	    std::cout << std::endl ;
	    std::cout << SET_FORECOLOR_BLUE << "----------------- TEST RESULTS ------------------------" << std::endl ;
	    std::cout << "TEST STARTED: " << getTimeInfo() << SET_DEFAULT_COLORS ;
	  }

	  finalResults << "TEST STARTED: " << getTimeInfo() ;
	  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "-------------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ; }

	  if(!i2cError_){ finalResults << readVfatChipID() ; }
	  if(!i2cError_ && !testVfatIds_.empty()){ saveMonitorConfigFile(testVfatIds_) ; }
	  //if(testPowerOnSel->isChecked() && !i2cError_){ finalResults << powerOnVfat() ; }
	  if(testPosMappingSel->isChecked() && !i2cError_){ finalResults << positionMappingVfat() ; }
	  if(testDACSel->isChecked() && !i2cError_){ finalResults << testVfatDAC() ; }    
	  if(testPulseOneChannelSel->isChecked() && !i2cError_){ finalResults << pulseOneChannelVfat() ; }
	  if(testDeadChannelsSel->isChecked() && !i2cError_){ finalResults << checkDeadChannelsVfat() ; }
	  if(testThresholdMeasurementSel->isChecked() && !i2cError_){ finalResults << thresholdMeasurementVfat() ; }
	  if(testLatencyScanSel->isChecked() && !i2cError_){ finalResults << latencyScanVfat() ; }
	  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "-------------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ; }
	  if(setDefaultsSel->isChecked() && !i2cError_){ setDefaultsVfat() ; }   
	}

#ifdef BUSUSBFEC
	else { // Chip testing procedure

	  shortDACScan_ = true ;
	  sbitDebug_ = false ;

	  if(advancedChipTesting->isChecked()){

	    advChipTesting = true ;
	    if(fullDACScan->isChecked()){ shortDACScan_ = false ; }

	    if(sbitDebugSel->isChecked()){ sbitDebug_ = true ; }

	    if(testLoopSel->isChecked()){

	      useLoop = true ;
	      loopTime = atoi(testLoopT->text()) ;
	      std::cout << SET_FORECOLOR_BLUE << "Looping the selected tests for " << loopTime << " mins" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	  else { advChipTesting = false ; }

	  if(standardChipTesting->isChecked() || advChipTesting){

	    bool triggerHybrid = false ;
	    if(TriggerTypeSel->isChecked()){

	      triggerHybrid = true ;
	      std::cout << SET_FORECOLOR_BLUE << "\nTest for trigger hybrids selected, performing only I2C, Data Packet, Mask and Both Binary Checks\n" << std::endl ;
	    }

	    finalResults << "TEST STARTED: " << getTimeInfo() ;

	    if(!i2cError_){ finalResults << readVfatChipID() ; }

	    if(!i2cError_ && testVfatIds_.size()>0){

	      time_t start, end ;
	      time(&start) ;

	      loopTime = loopTime * 60 ; // Convert to seconds

	      do{
	  
		createDirectories() ;

		if(!abort_){

		  saveMonitorConfigFiles() ; // Create the Monitor config file
	  
		  std::cout << SET_FORECOLOR_BLUE << "Test running now for " << testVfats_.size() << " VFAT(S)" << SET_DEFAULT_COLORS << std::endl ;

		  if(!abortChipTesting_ && (!advChipTesting || (advChipTesting && I2CSel->isChecked()))){ finalResults << testVfatI2C() ; }

#ifdef USB_GPIB_PS
		  if(!abortChipTesting_ && (!advChipTesting || (advChipTesting && biasDetectorSel->isChecked())) && detectorPresent_ && !triggerHybrid){ powerOnHV() ; }
#endif

		  if(!abortChipTesting_ && (!advChipTesting || (advChipTesting && dataPacketCheckSel->isChecked()))){ finalResults << testDataPacketCheck () ; }
		  if(!abortChipTesting_ && (!advChipTesting || (advChipTesting && maskSel->isChecked()))){ finalResults << maskCheck () ; }
		  if(!abortChipTesting_ && (!advChipTesting || (advChipTesting && DACScanSel->isChecked())) && !triggerHybrid){ finalResults << testVfatDAC() ; }
		  if(!abortChipTesting_ && (!advChipTesting || (advChipTesting && pulseScanSel->isChecked()))){ finalResults << pulseScan() ; }

#ifdef USB_GPIB_PS
		  if(!abortChipTesting_ && (!advChipTesting || (advChipTesting && biasDetectorSel->isChecked())) && detectorPresent_ && !triggerHybrid){ powerOffHV() ; }
#endif

		  // Analysis without ELOG entry
		  if(!abortChipTesting_ && (advChipTesting && analysisSel->isChecked())){ runMonitorAnalysis() ; }

		  // Analyze and post ELOG entry for every standard run (even if aborted)
		  if(!advChipTesting){ runTestManager() ; }
		}

		time(&end) ;
		if(testLoopSel->isChecked()){ std::cout << SET_FORECOLOR_BLUE << "Test has now run " << difftime(end,start) << " / " << loopTime << " secs" << std::endl ; }
		if(loopTime<=difftime(end,start)){ useLoop = false ; }
	      
	      }
	      while(useLoop) ;

	      updateLog() ;

	      if(!abort_){ 

		std::cout << SET_FORECOLOR_BLUE << "-------------------------------------------------------" << std::endl ;
		std::cout << "Results are stored to:" << std::endl ;
		for(int i=0;i<chipTestDirs_.size();i++){ std::cout << chipTestDirs_.at(i) << std::endl ; }
		std::cout << "-------------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;

		ledChipTest->setPixmap(greenLed_) ;
	      }

	    
#ifdef USB_GPIB_PS
#ifndef USB_GPIB_PS_NO_POWER_ON_OFF

	      // Power off
	      if(standardChipTesting->isChecked()){
	     
		try{

		  std::cout << SET_FORECOLOR_BLUE << "POWERING OFF THE PS..." << SET_DEFAULT_COLORS << std::endl ;

		  GPIBDevice gpibDevice( GPIB_BOARD_INDEX, GPIB_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;
		  std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gpibDevice.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;
		  gpibDevice.powerOff() ;

		}
		catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }	     
	      }
#endif
#endif
	    
	    }
	    else{ std::cout << SET_FORECOLOR_RED << "Chip IDs for chips missing, aborting..." << SET_DEFAULT_COLORS << std::endl ; }
	  }
	  else if(fullI2Check->isChecked()){

	    finalResults << "TEST STARTED: " << getTimeInfo() ;
	    I2CScan() ;

	    if(fullI2CSel->isChecked()){ 

	      finalResults << readVfatChipID() ;

	      if(!i2cError_ && testVfatIds_.size()>=0){ finalResults << testVfatI2C() ; }
	      else{ std::cout << SET_FORECOLOR_RED << "Chip IDs for chips missing, aborting..." << SET_DEFAULT_COLORS << std::endl ; }
	    }
	  }

	  // Red led in case of error
	  if(i2cError_ || abort_ || abortChipTesting_ ){ ledChipTest->setPixmap(redLed_) ; }

	}
#endif

	if(rememberSettingsSel->isChecked() && !chipTesting_){ testWriteBackVfatValues() ; }

	gettimeofday(&time2, &zone2); // Timestamp of test end
	double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
	double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
	double timesec = timesec2 - timesec1 ;
	double timemin = 0 ;

	if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "TEST ENDED: " << getTimeInfo() << SET_DEFAULT_COLORS ; }
	finalResults << "TEST ENDED: " << getTimeInfo() ;

	if(timesec>=60){

	  timemin = floor(timesec/60) ;
	  timesec = timesec - timemin*60 ;

	  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << std::dec << "TOTAL TEST TIME: " << timemin << " min " << ceil(timesec) << " sec" << SET_DEFAULT_COLORS << std::endl ; }
	  finalResults << std::dec << "TOTAL TEST TIME: " << timemin << " min " << ceil(timesec) << " sec" << std::endl ;
	}
	else{

	  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << std::dec << "TOTAL TEST TIME: " << timesec << " sec" << SET_DEFAULT_COLORS << std::endl ; }
	  finalResults << std::dec << "TOTAL TEST TIME: " << timesec << " sec" << std::endl ;
	}
   
	if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
	if(outputfilestream.is_open()){
	  if(showConsole_ && !chipTesting_) { std::cout << SET_FORECOLOR_BLUE << "WRITING RESULTS TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ; }
	  outputfilestream << finalResults.str() ;
	  outputfilestream.close() ;
	}
    
	if(showBuffer && !chipTesting_){

	  std::cout << std::endl ;
	  std::cout << SET_FORECOLOR_BLUE << "------------------- BUFFER ---------------------------" << std::endl ;
	  std::cout << finalResults.str() << SET_DEFAULT_COLORS << std::endl ;
	}
      }
      else{

	std::cout << SET_FORECOLOR_RED << "---------------------------------------------------" << std::endl ;
	std::cout << "INITIALIZATION NOT OK -> NO FURTHER TESTS PERFORMED" << std::endl ;
	std::cout << "---------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;
      }

      if(abortChipTesting_){ std::cout << SET_FORECOLOR_RED << "\"" << failedTest_ << "\" ABORTED DUE TO SERIOUS FAILURE." << SET_DEFAULT_COLORS << std::endl ; }
      else if(i2cError_){ std::cout << SET_FORECOLOR_RED << "\"" << failedTest_ << "\" ABORTED DUE TO SEVERAL I2C ERRORS." << SET_DEFAULT_COLORS << std::endl ; }

      finalResults.flush() ;
    }
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "******************************** ERROR *************************" << std::endl ;
    std::cerr << e.what() << std::endl ;
    std::cerr << "****************************************************************" << SET_DEFAULT_COLORS << std::endl ;
  }
  catch (...) {

    std::cout << SET_FORECOLOR_RED << "UNKNOWN EXCEPTION OCCURED..." << SET_DEFAULT_COLORS << std::endl ;
  }

  testCleanUp() ;

  if(!chipTesting_){ testStart->setText("Start") ; }
  else{ startChipTest->setText("Start") ; }
}


