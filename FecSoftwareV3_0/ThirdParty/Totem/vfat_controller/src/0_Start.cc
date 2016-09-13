/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#include <sstream>  // Stringstream
#include <stdlib.h> // Standard

#include <qmessagebox.h> // GUI
#include <qlabel.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qscrollview.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include "PixMapLed.h"     // Led
#include <sys/time.h>      // Time
#include "vfat_controller.h" // SW header

/** Constructor: build a FEC access
 */
vfat_controller::vfat_controller ( ):

  greenLed_((const char **)greenLed_data),
  redLed_  ((const char **)redLed_data),
  blackLed_  ((const char **)blackLed_data)

#ifdef VMEREADOUT
  ,
  myASCIIReader_((std::string)getenv("TOTFED_ROOT") + "/" + TOTFED_ADDR_TABLE),
  myAddressTable_((std::string)getenv("TOTFED_ROOT") + "/" + TOTFED_ADDR_TABLE, myASCIIReader_),
  vmeBusAdapter_(HAL::CAENLinuxBusAdapter::V2718)
#endif
{

  testInitOK_ = true ;
  i2cError_ = false ;
  failedTest_ = "" ;
  chanRegHis_ = 1 ;
  dataRun_ = false ;
  threadRunning_  = false ;

  const char* env = getenv("ENV_CMS_TK_FEC_ROOT") ;

  if(env!=0){ vfat_controller_home_ = (std::string)env  + "/ThirdParty/Totem/vfat_controller/" ; }
  else{ std::cout << SET_FORECOLOR_RED << "$ENV_CMS_TK_FEC_ROOT NOT DEFINED" << SET_DEFAULT_COLORS << std::endl ; }

  detectorType_ = "" ;
  deviceFile_ = "" ;

  uint ttcciSlot = MY_TTCCI_SLOT ;
  uint fedSlot = TOTFED_SLOT ;


  char **argv = qApp->argv() ;

#ifdef DEBUG_VFAT_CONTROLLER

  // Print the parameters
  std::cout << SET_FORECOLOR_CYAN << "----------------------------------------------" << std::endl ;
  for (int i = 0 ; i < qApp->argc() ; i++) { std::cout << i << ")\t" << argv[i] << std::endl ; }
  std::cout << "----------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;

#endif

  // Check the options
  for (int i = 0 ; i < qApp->argc() ; i++) {

    if (strcasecmp(argv[i],"-detectortype") == 0){

      i++ ;

      if (strcasecmp(argv[i],"RP") == 0){ detectorType_ = "RP" ; }
      else if (strcasecmp(argv[i],"GEM") == 0){ detectorType_ = "GEM" ; }
      else { std::cout << SET_FORECOLOR_RED << "Value \"" << argv[i] << "\" given for \"-detectortype\" option is invalid" << SET_DEFAULT_COLORS << std::endl ; }

      if(detectorType_ != ""){ std::cout << SET_FORECOLOR_BLUE << "Detector type set from command line to " << detectorType_ << SET_DEFAULT_COLORS << std::endl ; }
    }

    else if (strcasecmp(argv[i],"-devicefile") == 0){

      i++ ;

      QString fn = argv[i] ;

      if (!fn.isEmpty () && fn.endsWith( ".xml" )) { deviceFile_ = fn.latin1() ; }
      else{ std::cout << SET_FORECOLOR_RED << "Device file format needs to be  XML file (.xml)."<< SET_DEFAULT_COLORS << std::endl ; }

      if(deviceFile_ != ""){ std::cout << SET_FORECOLOR_BLUE << "Device file from command line: " << deviceFile_ << SET_DEFAULT_COLORS << std::endl ; }
    }

    else if (strcasecmp(argv[i],"-fed") == 0){

      i++ ;

      if (sscanf (argv[i], "%u", &fedSlot) == 0) {

	std::cout << SET_FORECOLOR_RED << "Value \"" << argv[i] << "\" given for \"-fed\" option is invalid" << SET_DEFAULT_COLORS << std::endl ;
	fedSlot = TOTFED_SLOT ;
      }
    }

    else if (strcasecmp(argv[i],"-ttcci") == 0){

      i++ ;

      if (sscanf (argv[i], "%u", &ttcciSlot) == 0) {

	std::cout << SET_FORECOLOR_RED << "Value \"" << argv[i] << "\" given for \"-ttcci\" option is invalid" << SET_DEFAULT_COLORS << std::endl ;
	ttcciSlot = TOTFED_SLOT ;
      }
    }
  }


#ifdef USB_GPIB_PS
#ifndef USB_GPIB_PS_NO_POWER_ON_OFF
   
  try{

    GPIBDevice gpibDevice( GPIB_BOARD_INDEX, GPIB_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;
    std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gpibDevice.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;

    std::cout << SET_FORECOLOR_BLUE  << "SETTING VOLTAGE TO " << GPIB_VOLTAGE << " V AND CURRENT LIMIT TO " << GPIB_CURRENT << " A ON THE PS" << SET_DEFAULT_COLORS << std::endl ;
    gpibDevice.setup(GPIB_VOLTAGE, GPIB_CURRENT) ;

    std::cout << SET_FORECOLOR_BLUE << "POWERING ON THE PS..." << SET_DEFAULT_COLORS << std::endl ;
    gpibDevice.powerOn() ;

    sleep(5) ; // Wait PS to power on
  }
  catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }
  
#endif
#endif

#ifdef TTCCI_TRIGGER

  try{

    std::cout << SET_FORECOLOR_BLUE << "Initializing TTCci board on slot " << ttcciSlot << " via " << MY_TTCCI_ADAPTER << SET_DEFAULT_COLORS << std::endl ;

    ttc::MultiBusAdapter::MultiBusAdapter busAdapter(MY_TTCCI_ADAPTER);

#ifdef DEBUG_VFAT_CONTROLLER
    ttcci_ = new ttc::TTCci::TTCci(busAdapter, ttcciSlot, true, DEFAULT_TTCCI_BTC, DEFAULT_TTCCI_DT2) ;
#else
    ttcci_ = new ttc::TTCci::TTCci(busAdapter, ttcciSlot, true, DEFAULT_TTCCI_BTC, DEFAULT_TTCCI_DT2, &std::cerr) ;
    //ttcci_ = new ttc::TTCci::TTCci(busAdapter, ttcciSlot, true, DEFAULT_TTCCI_BTC, DEFAULT_TTCCI_DT2, &std::ofstream("/dev/null")) ;
#endif

  }
  catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while initializing TTCci board." << SET_DEFAULT_COLORS << std::endl ; ttcci_ = NULL ; }

#else

  TTCciGroupBox->hide() ;

#endif


#ifdef VMEREADOUT

  totalBytesRead_ = 0 ;
  vmeofstream_.clear() ;
  for(int i=1;i<=TOTFED_READOUT_BUFFER_SIZE;i++){ vmeDataBuffer_[i] = '0' ; }

  const char* env_totfed = getenv("TOTFED_ROOT") ;
  std::string addressTable = "" ;

  if(env_totfed!=0){ addressTable = (std::string)env_totfed + "/" + TOTFED_ADDR_TABLE ; }
  else{ std::cout << SET_FORECOLOR_RED << "$TOTFED_ROOT NOT DEFINED" << SET_DEFAULT_COLORS << std::endl ; }

  try{

    std::cout << SET_FORECOLOR_BLUE << "Initializing TOTFED board on slot " <<fedSlot << " (ADDR=0x" << std::hex << (fedSlot << 27) << ")" << SET_DEFAULT_COLORS << std::dec << std::endl ;
    std::cout << SET_FORECOLOR_CYAN << "Using address table in " << addressTable << SET_DEFAULT_COLORS << std::endl ;

    totfed_ = new TOTFED(myAddressTable_, vmeBusAdapter_, (fedSlot << 27)) ;

    std::cout << SET_FORECOLOR_BLUE << "TOTFED firmware ver.(MAIN 1):\t0x" << std::hex << totfed_->getMainFirmwareVersion() << std::dec << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_BLUE << "OptoRx firmware ver.:\t0x" << std::hex << totfed_->getOptoRxFirmwareVersion() << std::dec << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_BLUE << "TOTFED initialization OK" << SET_DEFAULT_COLORS << std::endl ;

  }
  catch( HAL::NoSuchFileException &e ){ std::cerr << SET_FORECOLOR_RED << "NoSuchFileException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ; totfed_ = NULL ; }
  catch( HAL::BusAdapterException &e ){ std::cerr << SET_FORECOLOR_RED << "BusAdapterException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ; totfed_ = NULL ; }
  catch( HAL::UnsupportedException &e ){ std::cerr << SET_FORECOLOR_RED << "UnsupportedException:\t" << e.what() << SET_DEFAULT_COLORS << std::endl ; totfed_ = NULL ; }
  catch(...){ std::cout << SET_FORECOLOR_RED << "Unknown exception..." << SET_DEFAULT_COLORS << std::endl ; totfed_ = NULL ; }


  if(totfed_==NULL){ std::cout << SET_FORECOLOR_RED << "Problem while initializing TOTFED board." << SET_DEFAULT_COLORS << std::endl ; }

#ifdef OPTORX_DATAEMU

  std::cout << SET_FORECOLOR_BLUE << "Using OptoRx data emulator." << SET_DEFAULT_COLORS << std::endl ;
  testDataTakingSel->setText("Data Taking Using OptoRx Data Emulator") ;

#endif

#else

  VMEGroupBox->hide() ;
  testDataTakingSel->setDisabled(true) ;

#endif

  // Create a FEC access
  createFecAccess() ;

  fecType_ = fecAccess_->getFecBusType() ;

  if(fecType_==FECPCI){ std::cout << SET_FORECOLOR_CYAN << "Using PCI FEC" << SET_DEFAULT_COLORS << std::endl ; }
  else if(fecType_==FECVME){ std::cout << SET_FORECOLOR_CYAN << "Using VME FEC" << SET_DEFAULT_COLORS << std::endl ; }

#ifdef BUSUSBFEC
  else if(fecType_==FECUSB){

    std::stringstream ss ;
    crepeType32 ttp_version = 0 ;
    crepe_get_version(0,&ttp_version) ;
    ss << "0x" << std::hex << ttp_version ;
    ttpFirmwareVersion_ = ss.str() ;

    std::cout << SET_FORECOLOR_CYAN << "Using USB FEC" << SET_DEFAULT_COLORS << std::endl ;
    testPosMappingSel->setDisabled(true) ;
    testDataTakingSel->setDisabled(false) ;
  }
#endif

  for(int i=1;i<=VFAT_CH_NUM_MAX;i++){ chanReg[i] = 0 ; }

  vfatAccess_ = NULL ;
  vfatd_ = NULL ;
  dcuAccess_ = NULL ;

  fecIndex_ = 9 ;

  // Access manager
  fecAccessManager_ = new TotemFecAccessManager ( fecAccess_ ) ;

#ifdef DEBUG_VFAT_CONTROLLER
  fecAccessManager_->setDisplayDebugMessage(true) ;
#endif  

  miniFrames_ = false ;
  simpleTrigger_ = false ;
  chipTesting_ = false ;
  shortDACScan_ = false ;
  sbitDebug_ = false ;
  abortChipTesting_ = false ;
  detectorPresent_ = false ;
  abort_ = false ;

  advancedSettings->hide() ;

  // Display control panel's LEDs
  writeCompLed->setPixmap(blackLed_) ;
  readCompLed->setPixmap(blackLed_) ;
  multiWriteCompLed->setPixmap(blackLed_) ;
  multiReadCompLed->setPixmap(blackLed_) ;
  readCurrentCompLed->setPixmap(blackLed_) ;
  pvssInfoCompLed->setPixmap(blackLed_) ;

  // Display test panel's LEDs
  ledDataTaking->setPixmap(blackLed_) ;
  ledPosMapping->setPixmap(blackLed_) ;
  ledDACTest->setPixmap(blackLed_) ;
  ledPulseOneChannelTest->setPixmap(blackLed_) ;
  ledChannelTest->setPixmap(blackLed_) ;
  ledThresholdMeasurementTest->setPixmap(blackLed_) ;
  ledLatencyScanTest->setPixmap(blackLed_) ;

  // Display chip test panel's LED
  ledChipTest->setPixmap(blackLed_) ;

  // Display coincidence panel's LEDs
  readCChipCompLed->setPixmap(blackLed_) ;
  writeCChipCompLed->setPixmap(blackLed_) ;

#ifdef BUSUSBFEC
  if(fecType_==FECUSB){ crepeMasterReset(0) ; }
#endif

  // Find devices
  scanAll_pressed() ;

  // Show the detector dependent GUI parts
  if(detectorType_=="RP"){

    rpGroupBox->show() ;
    detectorTypeGroup->hide() ;
    hybridTypeGroup->hide() ;
    TriggerTypeSel->setChecked(false) ;

#ifdef USB_GPIB_PS
    biasDetectorSel->show() ;
#endif
  }
  else{

    rpGroupBox->hide() ;
    detectorTypeGroup->show() ;
    hybridTypeGroup->show() ;
    biasDetectorSel->hide() ;
  }

#ifdef BUSUSBFEC
  if(fecType_==FECUSB){

    setCrepeSettings() ;
    std::cout << SET_FORECOLOR_CYAN << "HYBRID TYPE: " << detectorType_ << SET_DEFAULT_COLORS << std::endl ;
  }
#endif

  // Make the panel srollable and show it
  //QScrollView *scrollView = new  QScrollView(0,0, Qt::WStyle_Customize | Qt::WStyle_NormalBorder & ~Qt::WStyle_Maximize) ;
  QScrollView *scrollView = new  QScrollView() ;
  scrollView->addChild((QWidget *)appFrame) ;
  scrollView->setMaximumSize(WINDOW_W,WINDOW_H) ;
  scrollView->resize(WINDOW_W,WINDOW_H) ;
  scrollView->show() ;
}

/** Destructor: disable all errors counting
 */
vfat_controller::~vfat_controller ( ) {

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << "vfat_controller destructor" << SET_DEFAULT_COLORS << std::endl ;
#endif

  cleanUp() ;
}

/**
 */
void vfat_controller::helpAbout() {
  QMessageBox::about( this, "vfat_controller -- About",
                      "<center><h1><font color=blue>vfat_controller</font></h1></center>"
                      "<p><i>vfat_controller - <b>Beta version</b> by <font color=red>Juha Petäjäjärvi and Andras Ster</font> (2007)</i></p>"
                      "<p><i>Based on FEC GUI Debugger made by <font color=red>Frederic Drouhin</font> (2003)</i></p>"
                      );
}

/**
 */
void vfat_controller::quitApplication() {

  close() ;
}

/** \brief Display the errors coming from FecAccessManager
 * Display the error coming from FecAccessManager
 */
void vfat_controller::displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList ) {

  if (errorList.size() == 0) return ;

  Sgi::hash_map<keyType, bool> errorDevice ; // in order to avoid multiple display of errors
  std::list<FecExceptionHandler *> deleteException ; // to delete the exception

  // simple message
  std::cout << SET_FORECOLOR_RED << message << ": found " << errorList.size() << " errors" << SET_DEFAULT_COLORS << std::endl ;

  for (std::list<FecExceptionHandler *>::iterator p = errorList.begin() ; p != errorList.end() ; p ++) {

    FecExceptionHandler *e = *p ;

    if (e->getPositionGiven()) {

      if (!errorDevice[e->getHardPosition()]) {

	errorDevice[e->getHardPosition()] = true ;

	std::cerr << SET_FORECOLOR_RED << "******************************** ERROR *************************" << std::endl ;
	std::cerr << e->what() << std::endl ;
	std::cerr << "****************************************************************" << SET_DEFAULT_COLORS << std::endl ;

	// Add to be deleted
	deleteException.push_back (e) ;	
      }
    }
    else {
      
      std::cerr << SET_FORECOLOR_RED << "******************************** ERROR *************************" << std::endl ;
      std::cerr << e->what() << std::endl ;
      std::cerr << "****************************************************************" << SET_DEFAULT_COLORS << std::endl ;
      
      // Add to be deleted
      deleteException.push_back (e) ;	
    }
  }

  // Delete the exception (not redundant)
  for (std::list<FecExceptionHandler *>::iterator p = deleteException.begin() ; p != deleteException.end() ; p ++) {  delete *p ; }
    
  // No more error
  errorList.clear() ;
}


/** \brief Display the error message
 * Display the error message
 */
void vfat_controller::ErrorMessage (std::string msg ) {

  QMessageBox::warning( this, "VfatDialog -- Error", msg ) ;
}


/** \brief Display the error message
 * Display the error message
 */
void vfat_controller::ErrorMessage (const char *msg ) {

  char msg1[1000] ;
  sprintf (msg1, "<h4><font color=red>%s</font></h4>", msg) ; 
  QMessageBox::warning( this, "VfatDialog -- Error", msg1 ) ;
}

/** \brief Display the error message
 * Display the error message
 */
void vfat_controller::ErrorMessage (const char *msg, const char *msg1 ) {

  char msg2[1000] ;
  sprintf (msg2, "<center><h4><font color=red>%s</font></h4></center><center><h4>%s</h4></center>", msg, msg1) ; 
  QMessageBox::warning( this, "VfatDialog -- Error", msg2 ) ;
}

/** \brief Display the error message
 * Display the error message
 */
void vfat_controller::ErrorMessage (char *title, FecExceptionHandler e ) {

  char msg[1000] ;
  sprintf (msg, "<center><h4><font color=red>%s</font></h4></center><center><h4>%s</h4></center><p>Error Code %ld<br>", 
           title, e.what().c_str(), e.getErrorCode()) ;
  
  char msg1[80] ;
  if (e.getPositionGiven()) {

    decodeKey ( msg1, e.getHardPosition() ) ;
    strcat (msg, "Position: ") ;
    strcat (msg, msg1) ;
    strcat (msg, "</p>") ;
  }

  QMessageBox::critical( this, "VfatDialog -- Exception", msg ) ;
}

/** To catch the close event method in order to do the same as quit button
 */
void vfat_controller::closeEvent (QCloseEvent *e) {

  QMessageBox mb( "Quit Application",
		  "Do you want to quit the application ?",
		  QMessageBox::NoIcon,
		  QMessageBox::Yes,
		  QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
		  QMessageBox::NoButton );

  if ( mb.exec() == QMessageBox::Yes ) {

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "Quiting..." << SET_DEFAULT_COLORS << std::endl ;
#endif

    cleanUp() ;
    qApp->quit() ;
  }
}

void vfat_controller::cleanUp() {

  if(fecAccessManager_!=NULL){

    // delete device accesses
    fecAccessManager_->removeDevices() ;

    try { delete fecAccessManager_ ; }
    catch (FecExceptionHandler &e) { ErrorMessage ("Unable to delete the access manager", e) ; }

    fecAccessManager_ = NULL ;
  }

  if(fecAccess_!=NULL){

    try { delete fecAccess_ ; }
    catch (FecExceptionHandler &e) { ErrorMessage ("Unable to delete the hardware accesses", e) ; }

    fecAccess_ = NULL ;
  }

  if(vfatd_!=NULL){ delete vfatd_ ; vfatd_ = NULL ; }

  // Accesses already deleted, just put NULL
  vfatAccess_ = NULL ;
  dcuAccess_ = NULL ;
  cchipAccess_ = NULL ;

#ifdef TTCCI_TRIGGER
  if (ttcci_ != NULL){ delete ttcci_ ; ttcci_ = NULL ; }
#endif

#ifdef VMEREADOUT
  if (totfed_ != NULL){ delete totfed_ ; totfed_ = NULL ; }
#endif

}

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   General methods                                                   */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

std::string vfat_controller::IntToString(int t) {
   std::ostringstream oss;
   oss << t;
   return oss.str();
}

std::string vfat_controller::IntToHex(int t) {
   std::ostringstream oss;
   oss << std::hex << t;
   return oss.str();
}

int vfat_controller::HexToInt(std::string hexStg) {
  int n = 0;         // position in string
  int m = 0;         // position in digit[] to shift
  int count;         // loop index
  int intValue = 0;  // integer value of hex string
  int digit[9];      // hold values to convert
  while (n < 9) {
     if (hexStg[n]=='\0')
        break;
     if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
        digit[n] = hexStg[n] & 0x0f;            //convert to int
     else if (hexStg[n] >='a' && hexStg[n] <= 'f') //if a to f
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
     else if (hexStg[n] >='A' && hexStg[n] <= 'F') //if A to F
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
     else break;
    n++;
  }
  count = n;
  m = n - 1;
  n = 0;
  while(n < count) {
     // digit[n] is value of hex digit at position n
     // (m << 2) is the number of positions to shift
     // OR the bits into return value
     intValue = intValue | (digit[n] << (m << 2));
     m--;   // adjust the position to set
     n++;   // next digit to process
  }
  return (intValue);
}

std::string vfat_controller::getTimeInfo() {
  time_t rawtime ;
  struct tm * timeinfo ;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return (asctime(timeinfo));
}

