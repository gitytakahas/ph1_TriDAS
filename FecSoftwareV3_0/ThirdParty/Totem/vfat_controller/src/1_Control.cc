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
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcheckbox.h> 
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfiledialog.h>

#include "vfat_controller.h" // SW header

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   Ring methods                                                      */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

void vfat_controller::createFecAccess(){

  int cpt = 0 ;

  try{

    // force acknowledge used (-noFack or -fack (default is -fack))
    // i2cSpeed - i2cSpeed used (-i2cSpeed)
    // initFec - initialise or not the FEC (true by default)
    // fecSlot - initialise only the corresponding FEC if it is different from 0xFFFFFFFF (only for VME FEC) (-fec <>)
    // invert the clock polarity (-invrecvclk)

    fecAccess_ = FecAccess::createFecAccess(qApp->argc(), qApp->argv(), &cpt) ;
  }
  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "COULDN'T CREATE THE FEC ACCESS" << SET_DEFAULT_COLORS << std::endl ; }
}


void vfat_controller::scanForFecs() {

  fecSlots->clear() ;
  fecSlots2->clear() ;
  fecSlots3->clear() ;

  try {
    // Scan the PCI slot for device driver loaded
    // list<keyType> *fecSlotList = fecAccess_->scanForFECs (0, 4) ;
    std::list<keyType> *fecSlotList = fecAccess_->getFecList() ;

    if (fecSlotList != NULL) {

      for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
      
	keyType index = *p ;
	fecIndex_ = index ; // For restoring the access later
     
	char msg[80] ;
	sprintf (msg, "%d,%d", getFecKey(index),getRingKey(index)) ;
	fecSlots->insertItem( msg );
	fecSlots2->insertItem( msg );
	fecSlots3->insertItem( msg );

	if (fecSlots->count()) {

	  fecAccessManager_->removeDevices () ;


	  // THE SSID SHOULD BE SET AUTOM. ONCE THE TOTEM MODE IS WORKING, LEAVE THIS FOR USER FOR NOW...
	  /*
	  // Set SSID if VME FEC
	  if(fecType_==FECVME){

	    CCSTrigger *ccsTrigger = fecAccess_->setCCSTrigger ( index, FEC_SSID ) ;

	    if (ccsTrigger->getSSID() != FEC_SSID) {

	      std::cout << SET_FORECOLOR_RED << "SSID read from hardware is different than what was set there: " << FEC_SSID << " / " << ccsTrigger->getSSID() << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	  */

	  // Initialize the FEC if chosen
	  if(fecInitSel->isChecked()){

	    fecAccess_->setFecRingCR0 (buildFecRingKey(getFecKey(index),getRingKey(index)), (FEC_CR0_POLARITY | FEC_CR0_RESETFSMFEC)) ; // Polarity and resetFSM
	    fecAccess_->fecRingReset ( buildFecRingKey(getFecKey(index),getRingKey(index)) ) ; // FEC reset
	  }
	}
	else { std::cout << SET_FORECOLOR_RED << "FEC INIT PROBLEM" << SET_DEFAULT_COLORS << std::endl ; }
	
      }
    }
    else { ErrorMessage ("Cannot access the FEC", "Please check if it is available") ; }
  }
  catch (FecExceptionHandler &e) {

    if(fecType_==FECVME){

      //system(VFAT_CAEN_DRIVER_RELOAD) ;
      ErrorMessage ("Cannot access the FEC...", e) ; 
    }
  }
}

void vfat_controller::scanForCcus() {

  ccuAddresses->clear() ;
  ccuAddresses2->clear() ;
  ccuAddresses3->clear() ;
  piaChannels->clear() ;

  try {

    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      // Scan the ring for CCUs
      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice ( buildFecRingKey(slot,ringSlot) ) ;
      fecRingDevice->fecScanRingBroadcast ( ) ;

      // Scan the ring for each FECs and display the CCUs
      std::list<keyType> *ccuList = fecAccess_->getCcuList ( buildFecRingKey(slot,ringSlot) ) ;

      if (ccuList != NULL ) {

        for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
  
          keyType index = *p ;
          if (getFecKey(index) == slot) {

	    char msg[80] ;
	    sprintf (msg, "0x%02X", getCcuKey(index)) ;
	    ccuAddresses->insertItem( msg );
	    ccuAddresses2->insertItem( msg );
	    ccuAddresses3->insertItem( msg );
          }
        }

	// Add PIA channels
	for (uint i = 0x30;i<0x34;i++) {

	  char msg[80] ;
	  sprintf (msg, "0x%02X", i) ;
	  piaChannels->insertItem ( msg ) ;
	}
      }
      else { ErrorMessage ("No CCU found on FECs") ; }  

      // Delete = already store in the table of FEC device class
      delete ccuList ;
    }
    else { ErrorMessage ("The FEC slot entered is not correct or empty", "Please scan FECs and CCUs before accessing the devices" ) ; }
  }
  catch (FecExceptionHandler &e) { ErrorMessage ("Scan ring for CCU failed", e) ; }
}


/*********************************************************************************/

void vfat_controller::scanForVfats() {

  keyType index = 0 , fecSlot = 0, ringSlot = 0, ccuAddress = 0, channel = 0 ;
  vfatAddresses_.clear() ;
  dcuAddresses_.clear() ;
  cchipAddresses_.clear() ;
  vfatChipidT->setText("") ;
  cchipChipidT->setText("") ;

  try {

    if ( fecSlots->count() && ccuAddresses->count() ) {

      // For each FEC
      for (int i = 0 ; i < fecSlots->count() ; i ++) {

        if (sscanf (fecSlots->text(i), "%d,%d", &fecSlot, &ringSlot)) {

	  //vfatAccess *vfat = NULL ;
	  //dcuAccess  *dcu  = NULL ;

          FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(fecSlot,ringSlot)) ;


          // Build a table with all values
          int sizeValues = 8 ;
	  keyType deviceValues[8][2] = { 

            {0x00, NORMALMODE},
            {0x10, NORMALMODE},
	    {0x20, NORMALMODE},
            {0x30, NORMALMODE},
            {0x40, NORMALMODE},
            {0x50, NORMALMODE},
            {0x60, NORMALMODE},
	    {0x70, NORMALMODE}

            } ;

	  if(fecType_==FECUSB){

	    std::cout << SET_FORECOLOR_BLUE << "As you can see \"fecRingDevice->scanRingForI2CDevice()\" is not extremely fast with USB..." << SET_DEFAULT_COLORS << std::endl ;
	  }

          std::list<keyType> *deviceList = fecRingDevice->scanRingForI2CDevice ( (keyType *)deviceValues, sizeValues ) ;


          if (deviceList != NULL) {

#ifdef DEBUG_VFAT_CONTROLLER
	    std::cout << SET_FORECOLOR_CYAN << "Number of devices found: " << deviceList->size() << SET_DEFAULT_COLORS << std::endl ; 
#endif

            for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) {

              keyType index = *p ;       
              char msg[100] ;
              decodeKey (msg, index) ;

	      getChipType(index) ; // Try to identify the chip 

            }

            delete deviceList ;
	  }
	  else {
            
	    char msg[80] ;
	    sprintf (msg, "No VFAT device found for the FEC %d", fecSlot) ;
	    ErrorMessage (msg) ;
	  }
	}
        else {
	  
          char msg[100] ; 
          sprintf (msg, "Inconsistency FEC slot: %s", fecSlots->text(i).latin1()) ;
          ErrorMessage (msg) ;
        }
      }
    }
    else { ErrorMessage ("The FEC / CCU slot entered is not correct or empty", "Please scan FECs and CCUs before accessing the devices" ) ; }

  }
  catch (FecExceptionHandler &e) { ErrorMessage ("Operation failed: unable to scan the ring for i2c devices", e) ; }

  std::cout << std::dec ;


  if(!vfatAddresses_.empty()){

    // Update CCU, channel and device address fields in the GUI
    refreshCcuAddresses() ;
  
    // Put the found devices to test panel list
    fillVfatTestList() ; 
  }

  if(!cchipAddresses_.empty()){

    // Update CCU, channel and device address fields in the GUI
    refreshCcuAddresses3() ;
  }


}

void vfat_controller::getChipType ( keyType index ){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "------------------------------------------------------------------------" << std::endl ;
  std::cout << "Try to ID chip found at: FEC (" 
	    << std::hex << (int)getFecKey(index) << ","
	    << std::hex << (int)getRingKey(index) << ") CCU 0x" 
	    << std::hex << (int)getCcuKey(index) << " CHANNEL 0x" 
	    << std::hex << (int)getChannelKey(index) << " ADDRESS 0x" 
	    << std::hex << (int)getAddressKey(index) << SET_DEFAULT_COLORS << std::endl ;
#endif

  std::string deviceType = "UNIDENTIFIED CHIP" ;
  bool exceptionThrown = false ;

  enumDeviceType modeType = VFAT ; // Scan devices which are used in NORMAL MODE as VFAT for example...

  // ADD THE TEMPERARY I2C ACCESSES NEEDED FOR THE IDENTIFICATION 
  try {

    fecAccess_->addi2cAccess (index, modeType, MODE_SHARE) ; // Base
    fecAccess_->addi2cAccess (index+1, modeType, MODE_SHARE) ; // FPGA

    fecAccess_->addi2cAccess (index+2, modeType, MODE_SHARE) ; // GOL
    fecAccess_->addi2cAccess (index+4, modeType, MODE_SHARE) ; // PLL
    //fecAccess_->addi2cAccess (index+5, modeType, MODE_SHARE) ;
    //fecAccess_->addi2cAccess (index+6, modeType, MODE_SHARE) ;
    fecAccess_->addi2cAccess (index+8, modeType, MODE_SHARE) ; // DCU

    fecAccess_->addi2cAccess (index+14, modeType, MODE_SHARE) ; // VFAT, CCHIP
    fecAccess_->addi2cAccess (index+15, modeType, MODE_SHARE) ; // VFAT, CCHIP

  }
  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "COULDN'T ADD I2C ACCESS" << SET_DEFAULT_COLORS << std::endl ; }
  

  // TRY IF DEVICE HAS EXTENDED REGISTER PART --> VFAT, CCHIP OR FPGA
  try {

    keyType i2cReg = index + 14 ; // EXTREG POINTER
    uint value = fecAccess_->read(i2cReg) ;
  }
  catch (FecExceptionHandler &e) {

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "EXCEPTION THROWN -> CHIP DOESN'T HAVE EXTENDED REGISTERS -> IS NOT VFAT OR CCHIP" << std ::endl ;
#endif
    exceptionThrown = true ;
  }

  // IF NO EXTENDED REGISTER PART --> DCU
  if(exceptionThrown){


    exceptionThrown = false ;

    // GOL?
    try {

      keyType i2cReg = index + 2 ;
      uint value = fecAccess_->read(i2cReg) ;
    }
    catch (FecExceptionHandler &e) { exceptionThrown = true ; /*deviceType = "GOL" ;*/ }

    if(!exceptionThrown){

      // PLL?
      try {

	keyType i2cReg = index + 4 ;
	uint value = fecAccess_->read(i2cReg) ;
      }
      catch (FecExceptionHandler &e) { exceptionThrown = true ; /*deviceType = "PLL" ;*/ }
    }
    /*
      if(!exceptionThrown){

      // DOH?
      try {

      keyType i2cReg = index + 6 ;
      uint value = fecAccess_->read(i2cReg) ;
      }
      catch (FecExceptionHandler &e) { exceptionThrown = true ; deviceType = "DOH" ; }
      }
    */

    if(!exceptionThrown){

      // DCU?
      try {

	keyType i2cReg = index + 8 ;
	uint value = fecAccess_->read(i2cReg) ;
      }
      catch (FecExceptionHandler &e) { exceptionThrown = true ; deviceType = "DCU" ; }


      if(exceptionThrown){


	//deviceType = "DCU" ;

	if(detectorType_ == "" && getAddressKey(index)==VFAT_DCU_RP_HYBRID){
	  detectorType_ = "RP" ;
	}
            
	if(detectorType_ == "" && (getAddressKey(index)==VFAT_DCU_TBB_1 || getAddressKey(index)==VFAT_DCU_TBB_2)){
	  detectorType_ = "GEM" ;
	}
      }
    }
  }
  else {

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "NO EXCEPTION THROWN -> CHIP HAS EXTENDED REGISTERS -> CCHIP, VFAT OR FPGA" << std ::endl ;
#endif

    // TRY TO FIGER OUT IF THE CHIP IS VFAT OR CCHIP OR FPGA
    try {

      uint testValue = 0x55 ;
      keyType i2cReg = index + 14 ; // EXTREG POINTER

      uint tmpValue1 = fecAccess_->read(i2cReg) ;

      fecAccess_->write(i2cReg, 0x10) ; // 10th CH REG

      uint tmpValue2 = fecAccess_->read(i2cReg+1) ;

      // Write test value to external register part
      fecAccess_->write(i2cReg+1, testValue) ;

      // Try to read the test value from external register part
      uint tmpValue3 = fecAccess_->read(i2cReg+1) ;

      if (tmpValue3==testValue){

	deviceType = "VFAT" ;
      }
      else {

	keyType i2cReg = index ; // FPGA ID

	uint value1 = 0x00 ;
	uint value2 = 0x00 ;

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "Try to read the FPGA ID" << SET_DEFAULT_COLORS << std::endl ;
#endif
	try {

	  value1 = fecAccess_->read(i2cReg) ;
	  value2 = fecAccess_->read(i2cReg+1) ;


#ifdef DEBUG_VFAT_CONTROLLER
	  std::cout << SET_FORECOLOR_CYAN << "Values read (possible FPGA ID) "<< value1 << ", " << value2 <<  SET_DEFAULT_COLORS << std::endl ;
#endif
	}
	catch (FecExceptionHandler &e) {}

	// CHECK IF FPGA ID IS READ (CAFE)
	if (value1==0xca && value2==0xfe) {

	  deviceType = "FPGA" ;
	}
	else{

	  deviceType = "CCHIP" ;

#ifdef DEBUG_VFAT_CONTROLLER
	  std::cout << SET_FORECOLOR_CYAN << "\"FPGA ID\" NOT CAFE  -> Type CCHIP" << SET_DEFAULT_COLORS << std::endl ;
#endif
	}
      }

      // Return the original values
      fecAccess_->write(i2cReg, 0x10) ; // 10th CH REG
      fecAccess_->write(i2cReg+1, tmpValue2) ;

      fecAccess_->write(i2cReg,tmpValue1) ; // EXTREG POINTER
    }

    // EXCEPTION -> DEVICE IS CCHIP
    catch (FecExceptionHandler &e) { 

      //deviceType = "CCHIP" ;
      deviceType = "UNKNOWN" ;
    }
  }

  // REMOVE THE TEMPERARY I2C ACCESSES
  try {

    fecAccess_->removei2cAccess(index) ;
    fecAccess_->removei2cAccess(index+1) ;

    fecAccess_->removei2cAccess(index+2) ;
    fecAccess_->removei2cAccess(index+4) ;
    //fecAccess_->removei2cAccess(index+5) ;
    //fecAccess_->removei2cAccess(index+6) ;
    fecAccess_->removei2cAccess(index+8) ;

    fecAccess_->removei2cAccess(index+14) ;
    fecAccess_->removei2cAccess(index+15) ;

  }
  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "COULDN'T REMOVE I2C ACCESS" << SET_DEFAULT_COLORS << std::endl ; }


  // ADD ACCESS INTO ACCESS MANAGER - DCU
  if (deviceType == "DCU"){

    try {

      dcuAccess_ = new dcuAccess(fecAccess_, index) ;
      dcuAddresses_.push_back(index) ;
      
      if (dcuAccess_ != (deviceAccess*)fecAccessManager_->getAccess(DCU, index)){

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "DCU ACCESS ADDED TO MANAGER" << SET_DEFAULT_COLORS << std::endl ;
#endif

        fecAccessManager_->setAccess((deviceAccess*)dcuAccess_) ;
      }
    }    
    catch (FecExceptionHandler &e) { ErrorMessage("ACCESS MANAGER::ADD DEVICE", e) ; }
      
    // Try to use the new access
    try {

      dcuAccess_ = (dcuAccess*)fecAccessManager_->getAccess(DCU, index) ;
    }
    catch (FecExceptionHandler &e) {

      std::cerr << SET_FORECOLOR_RED << "******************************** ERROR *************************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "****************************************************************" << SET_DEFAULT_COLORS << std::endl ;
    }
    
  }

  // ADD ACCESS INTO ACCESS MANAGER - VFAT
  else if (deviceType == "VFAT"){

    try {

      vfatAccess_ = new vfatAccess(fecAccess_, index) ;
      vfatAddresses_.push_back(index) ;

      if (vfatAccess_ != (vfatAccess *)fecAccessManager_->getAccess(VFAT, index)){

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "VFAT ACCESS ADDED TO MANAGER" << SET_DEFAULT_COLORS << std::endl ;
#endif

	fecAccessManager_->setAccess((deviceAccess*)vfatAccess_) ;
      }
    }
    catch (FecExceptionHandler &e) { ErrorMessage("ACCESS MANAGER::ADD DEVICE", e) ; }

    // Try to use the new access
    try {

      vfatAccess_ = (vfatAccess *)fecAccessManager_->getAccess(VFAT, index) ;
    }
    catch (FecExceptionHandler &e) {

      std::cerr << SET_FORECOLOR_RED << "******************************** ERROR *************************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "****************************************************************" << SET_DEFAULT_COLORS << std::endl ;
    }
  }

  // ADD ACCESS INTO ACCESS MANAGER - CCHIP
  else if (deviceType == "CCHIP"){

    try {

      cchipAccess_ = new totemCChipAccess(fecAccess_, index) ;
      cchipAddresses_.push_back(index) ;

      if (cchipAccess_ != (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, index)){

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "VFAT ACCESS ADDED TO MANAGER" << SET_DEFAULT_COLORS << std::endl ;
#endif

	fecAccessManager_->setAccess((deviceAccess*)cchipAccess_) ;
      }
    }
    catch (FecExceptionHandler &e) { ErrorMessage("ACCESS MANAGER::ADD DEVICE", e) ; }

    // Try to use the new access
    try {

      cchipAccess_ = (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, index) ;
    }
    catch (FecExceptionHandler &e) {

      std::cerr << SET_FORECOLOR_RED << "******************************** ERROR *************************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "****************************************************************" << SET_DEFAULT_COLORS << std::endl ;
    }
  }

  std::cout << SET_FORECOLOR_BLUE << deviceType << " Found at: FEC (" 
	    << std::hex << (int)getFecKey(index) << ","
	    << std::hex << (int)getRingKey(index) << ") CCU 0x" 
	    << std::hex << (int)getCcuKey(index) << " CHANNEL 0x" 
	    << std::hex << (int)getChannelKey(index) << " ADDRESS 0x" 
	    << std::hex << (int)getAddressKey(index) << std::dec << SET_DEFAULT_COLORS << std::endl ;

  if (deviceType == "UNIDENTIFIED CHIP"){ std::cout << SET_FORECOLOR_RED << "Device identification FAILED !" << SET_DEFAULT_COLORS << std::endl ; }
} 

/*********************************************************************************/

void vfat_controller::scanAll_pressed( ) {

  scanForFecs() ;

  if(fecSlots->count()<1){ return ; }

  scanForCcus() ;

  if(ccuAddresses->count()<1){ return ; }

  if(deviceFile_!=""){

    try{

      deviceVector vDevice ;
      TotemFecFactory fecFactory ;

      fecFactory.getFecDeviceDescriptions(vDevice, deviceFile_) ;
      createDevices(vDevice) ;
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Error during the upload in file", e) ; }

    // To do adding only once
    deviceFile_="" ;
  }
  else{

    scanForVfats() ;
  }
}

void vfat_controller::addAndConfigure_pressed(){


  QString fn = QFileDialog::getOpenFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;
      
  if (! fn.isEmpty ()) {

    if (!fn.endsWith( ".xml" )){ fn.append(".xml") ; }

    deviceVector vDevice ;
    TotemFecFactory fecFactory ;

    // Parse the file for devices
    fecFactory.getFecDeviceDescriptions(vDevice, fn.latin1()) ;

    PiaResetFactory piaResetFactory ;
    piaResetVector vPiaReset ;

    // Parse the file for PIA resets
    piaResetFactory.setInputFileName(fn.latin1()) ;
    piaResetFactory.getPiaResetDescriptions(vPiaReset) ;


    createDevices(vDevice) ;

    if(!vPiaReset.empty()){ resetPIA(vPiaReset) ; }

    if(!vDevice.empty()){ configureDevices(vDevice) ; }
    else{ ErrorMessage ("No devices found from the file") ; }
  }
}

void vfat_controller::writeI2cRegisters_pressed( ){

  try {

    if ( fecSlots->count() && ccuAddresses->count() ) {

      keyType fecSlot = 0, ringSlot = 0 ;

      std::ostringstream oss ;
      std::ofstream outputfilestream ;

      QString fn = QFileDialog::getSaveFileName( VFAT_PATH_I2CREGS, tr( "All Files (*)" ), this ) ;
      if (fn.isEmpty ()) { fn = vfat_controller_home_ +  VFAT_PATH_I2CREGS ; }


      // Scan the i2c registers for each FEC
      for (int i = 0 ; i < fecSlots->count() ; i ++) {

        if (sscanf (fecSlots->text(i), "%d,%d", &fecSlot, &ringSlot)) {

	  FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(fecSlot,ringSlot)) ;

	  std::list<keyType> *deviceList = fecRingDevice->scanRingForI2CDevice ( ) ;

	  if (deviceList != NULL) {

	    for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) {
      
	      keyType index = *p ;   
	      char msg[100] ;

	      decodeKey (msg, index) ;
	      oss << msg << std::endl ;
	      std::cout << SET_FORECOLOR_BLUE << msg << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	  else { ErrorMessage ("No devices found on the CCU and channels") ; }

	  delete deviceList ;
	}
      }

      if (!outputfilestream.is_open()){ outputfilestream.open(fn.latin1()) ; }
 
      if(outputfilestream.is_open()){

	std::cout << SET_FORECOLOR_BLUE << "WRITING THE I2C REGISTER INFO TO: " << fn.latin1() << SET_DEFAULT_COLORS << std::endl ;
	outputfilestream << oss.str() ;
	outputfilestream.close() ;
      }

    }
    else {  ErrorMessage ("No FECs and CCUs found." ) ; }
  }
  catch (FecExceptionHandler &e) { ErrorMessage ("Operation failed: unable to scan the ring for i2c devices", e) ; }

}

/* ------------------------------------------------------------------------------------------------*/
/*                                                                                                 */ 
/*                 VFAT                                                                            */
/*                                                                                                 */ 
/* ------------------------------------------------------------------------------------------------*/ 

/** \brief Return the value of the VFAT channel register corresponding to a VFAT register
 * Return the value of the VFAT channel register corresponding to a VFAT register
 * \param index - index
 * \return the value
 */
tscType8 vfat_controller::getChReg ( int index ) {

  if(index>0 && index<=VFAT_CH_NUM_MAX){ return (chanReg[index]) ; }
  else { std::cout << SET_FORECOLOR_RED << "Index (" << index << ") given was out of array boundaries." << SET_DEFAULT_COLORS << std::endl ; return((tscType8)0x00) ; }

}

 /** \brief set the value for register
  * Set the value for a VFAT description
  * \param index - index
  * \param value - value to be set
  */
void vfat_controller::setChReg ( int index, tscType8 value ) {

  if(index>0 && index<=VFAT_CH_NUM_MAX){ chanReg[index] = value ; }
  else { std::cout << SET_FORECOLOR_RED << "Index (" << index << ") given was out of array boundaries." << SET_DEFAULT_COLORS << std::endl ; }
}

 /** \brief Return the pointer of the VFAT channel registers corresponding to a VFAT channel registers' values
  * Return the pointer of the VFAT channel registers corresponding to a VFAT channel registers' values
  * \return the pointer
  */
tscType8 *vfat_controller::getAllChReg () { return (chanReg); }

/** \brief set the values for all channel registers
 * Set the values for all channel registers
 * \param *value - pointer to values to be set
 */
void vfat_controller::setAllChReg ( tscType8 *value )
{
  for (int i=1;i<=VFAT_CH_NUM_MAX;i++){ chanReg[i] = value[i] ; }
}


//*********************************************************************************/
//  Set the same value for all channel registers

void vfat_controller::setAllChRegSame ( tscType8 value )
{
  for (int i=1;i<=VFAT_CH_NUM_MAX;i++){ chanReg[i] = value ; }
}


/*********************************************************************************/
/** \brief Read the hybrid temperature
 * Read the hybrid temperature
 */
double vfat_controller::getDcuTemperature() {

  if(dcuAccess_==NULL){

    std::cout << SET_FORECOLOR_RED << "No DCU found." << SET_DEFAULT_COLORS << std::endl ;
    return(0.0) ;
  }

  dcuDescription dcud ;
  int itempr = 0 ;

  try{

    itempr = (int)dcuAccess_->getDcuChannel((tscType8)DCU_T_CH, false) ;
  }
  catch (FecExceptionHandler &e) {

    std::cout << SET_FORECOLOR_RED << "DCU Hardware Access Error." << SET_DEFAULT_COLORS << std::endl ;
    return(0.0) ;
  }

  return(dcud.calcTempInt(itempr));
}

/** \brief Read CTS/BE info
 * Read CTS/BE info
 */
void vfat_controller::readCurrentInfo_pressed(){

  // DAC characterisation, CTS and BE info - RP ONLY
  if (detectorType_ == "RP"){

    std::ofstream outputfilestream ;
    std::ostringstream hybridInfo ;
    std::string fn = vfat_controller_home_ + VFAT_PATH_DAC_RP_HYBRID ;

    dcuAccess *dcu = NULL ;
    vfatAccess *vfat = NULL ;

    try {

      //All the DCUs
      for(int i=0;i<dcuAddresses_.size();i++) {

	//std::stringstream dcuChannel ;

	dcu = (dcuAccess *)fecAccessManager_->getAccess(DCU, dcuAddresses_.at(i)) ;

	uint hybrid = dcu->getChannelId() ;

	hybridInfo << "RP current info on " << getTimeInfo() 
		   << "DCU on FEC (" 
		   << std::hex << (int)dcu->getFecSlot() << ","
		   << std::hex << (int)dcu->getRingSlot() << ") CCU 0x" 
		   << std::hex << (int)dcu->getCcuAddress() << " CHANNEL 0x" 
		   << std::hex << (int)dcu->getChannelId() << " ADDRESS 0x" 
		   << std::hex << (int)dcu->getI2CAddress()
		   << std::dec << " - VFAT IDs:" ;

	//Get VFAT chip ID for the chips connected to this hybrid 
	for(int j=0;j<vfatAddresses_.size();j++) {

	  uint channel = getChannelKey(vfatAddresses_.at(j)) ;

	  // VFAT on correct hybrid
	  if(channel==hybrid){

#ifdef DEBUG_VFAT_CONTROLLER
	    std::cout << SET_FORECOLOR_CYAN << "ADDRESS: " << channel << " / " << hybrid << ", save ID ..." << SET_DEFAULT_COLORS << std::endl ;
#endif
	    vfat = (vfatAccess *)fecAccessManager_->getAccess(VFAT, vfatAddresses_.at(j)) ;
	    hybridInfo << " 0x" << IntToHex((int)vfat->getVfatChipid1()) << IntToHex((int)vfat->getVfatChipid0()) ;
	  }
	}

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "DCU CHANNEL: " << std::hex << (int)dcu->getChannelId() << std::dec << SET_DEFAULT_COLORS << std::endl ;
#endif

	hybridInfo << std::endl ;

	int beBits = (int)dcu->getDcuChannel((tscType8)DCU_RP_BE_CH, false) ;
	hybridInfo << "BE: " << beBits*BE_CONVERTION_FACTOR << " nA (" << std::dec << beBits << ")" << std::endl ;


	int ctsBits = (int)dcu->getDcuChannel((tscType8)DCU_RP_CTS_CH, false) ;
	hybridInfo << "CTS: " << ctsBits*CTS_CONVERTION_FACTOR << " µA (" << std::dec << ctsBits << ")" << std::endl ;

	hybridInfo << std::endl ;
      }

      if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str(), std::fstream::app) ; }
 
      if(outputfilestream.is_open()){
	std::cout << SET_FORECOLOR_BLUE << "WRITING RP HYBRID INFO TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	outputfilestream << hybridInfo.str() ;
	outputfilestream.close() ;
      }
      readCurrentCompLed->setPixmap(greenLed_) ;
    }
    catch (FecExceptionHandler &e) {

      ErrorMessage ("I2C problem while reading CTS/BE info from RP hybrid." ) ;
      readCurrentCompLed->setPixmap(redLed_) ;
    }   
  }
}

/** \brief Update current and temperature values to PVSS
 * Update current and temperature values to PVSS
 */
void vfat_controller::pvssInfo_pressed(){

  updatePvssInfo() ;
}

void vfat_controller::updatePvssInfo(){

  if (detectorType_ == "RP"){

    std::ofstream outputfilestream ;
    std::ostringstream hybridInfo ;
    std::string fn = vfat_controller_home_ + "pvssDebug.txt" ;

    std::stringstream ss ;
    bool ccuOK = false ;

    dcuAccess *dcu = NULL ;
    vfatAccess *vfat = NULL ;

    try {

      //All the DCUs
      for(int i=0;i<dcuAddresses_.size();i++) {

	std::stringstream dpName ;

	dcu = (dcuAccess *)fecAccessManager_->getAccess(DCU, dcuAddresses_.at(i)) ;

	hybridInfo << "RP current info on " << getTimeInfo() 
		   << "DCU on FEC (" 
		   << std::hex << (int)dcu->getFecSlot() << ","
		   << std::hex << (int)dcu->getRingSlot() << ") CCU 0x" 
		   << std::hex << (int)dcu->getCcuAddress() << " CHANNEL 0x" 
		   << std::hex << (int)dcu->getChannelId() << " ADDRESS 0x" 
		   << std::hex << (int)dcu->getI2CAddress()
		   << std::dec << std::endl ;


	// Construct the start of PVSS datapoint name based on the CCU address
	if(dcu->getCcuAddress()==CCU_RP_1 && dcu->getI2CAddress()==VFAT_DCU_RP_HYBRID){

	  ccuOK=true ;
	  dpName << PVSS_NODE << PVSS_DP_PREFIX << PVSS_DP_START_RP1 << std::setw(2) << std::setfill('0') << (dcu->getChannelId() & 0x000f) ;
	}
	else if(dcu->getCcuAddress()==CCU_RP_2 && dcu->getI2CAddress()==VFAT_DCU_RP_HYBRID){

	  ccuOK=true ;
	  dpName << PVSS_NODE << PVSS_DP_PREFIX << PVSS_DP_START_RP2 << std::setw(2) << std::setfill('0') << (dcu->getChannelId() & 0x000f) ;
	}
	else if(dcu->getCcuAddress()==CCU_RP_3 && dcu->getI2CAddress()==VFAT_DCU_RP_HYBRID){

	  ccuOK=true ;
	  dpName << PVSS_NODE << PVSS_DP_PREFIX << PVSS_DP_START_RP3 << std::setw(2) << std::setfill('0') << (dcu->getChannelId() & 0x000f) ;
	}
	else if(dcu->getCcuAddress()==CCU_RP_4 && dcu->getI2CAddress()==VFAT_DCU_RP_HYBRID){

	  ccuOK=true ;
	  dpName << PVSS_NODE << PVSS_DP_PREFIX << PVSS_DP_START_RP4 << std::setw(2) << std::setfill('0') << (dcu->getChannelId() & 0x000f) ;
	}
	else if(dcu->getCcuAddress()==CCU_RP_5 && dcu->getI2CAddress()==VFAT_DCU_RP_HYBRID){

	  ccuOK=true ;
	  dpName << PVSS_NODE << PVSS_DP_PREFIX << PVSS_DP_START_RP5 << std::setw(2) << std::setfill('0') << (dcu->getChannelId() & 0x000f) ;
	}
	else if(dcu->getCcuAddress()==CCU_RP_6 && dcu->getI2CAddress()==VFAT_DCU_RP_HYBRID){

	  ccuOK=true ;
	  dpName << PVSS_NODE << PVSS_DP_PREFIX << PVSS_DP_START_RP6 << std::setw(2) << std::setfill('0') << (dcu->getChannelId() & 0x000f) ;
	}
	else if(dcu->getCcuAddress()==CCU_RP_7 && dcu->getI2CAddress()==VFAT_DCU_RP_HYBRID){

	  ccuOK=true ;
	  dpName << PVSS_NODE << PVSS_DP_PREFIX << PVSS_DP_START_RP7 << std::setw(2) << std::setfill('0') << (dcu->getChannelId() & 0x000f) ;
	}
	else{

	  ccuOK=false ;
	  std::cout << SET_FORECOLOR_RED
		    << "CCU address 0x" << std::hex << (int)dcu->getCcuAddress()
		    << " and I2C address 0x" << (int)dcu->getI2CAddress()
		    << " don't match with known DCUs." << SET_DEFAULT_COLORS << std::endl ;
	}

	hybridInfo << std::endl ;

	if(ccuOK){

	  int beBits = (int)dcu->getDcuChannel((tscType8)DCU_RP_BE_CH, false) ;
	  hybridInfo << "BE: " << beBits*BE_CONVERTION_FACTOR << " nA (" << std::dec << beBits << ")" << std::endl ;

	  ss << SET_PVSS_DATAPOINT << dpName.str() << PVSS_DP_BE_VALUE << beBits << PSX_SERVER ;
	  std::cout << ss.str().c_str() << std::endl ;
	  system((ss.str()).c_str()) ;
	  ss.str("") ;

	  int ctsBits = (int)dcu->getDcuChannel((tscType8)DCU_RP_CTS_CH, false) ;
	  hybridInfo << "CTS: " << ctsBits*CTS_CONVERTION_FACTOR << " µA (" << std::dec << ctsBits << ")" << std::endl ;

	  ss << SET_PVSS_DATAPOINT << dpName.str() << PVSS_DP_CTS_VALUE << ctsBits << PSX_SERVER ;
	  std::cout << ss.str().c_str() << std::endl ;
	  system((ss.str()).c_str()) ;
	  ss.str("") ;

	  int temprBits = (int)dcuAccess_->getDcuChannel((tscType8)DCU_T_CH, false) ;
	  hybridInfo << "Temperature: " << dcuDescription::calcTempInt(temprBits) << " °C (" << std::dec << temprBits << ")" << std::endl ;

	  ss << SET_PVSS_DATAPOINT << dpName.str() << PVSS_DP_TEMP_VALUE << temprBits << PSX_SERVER ;
	  std::cout << ss.str().c_str() << std::endl ;
	  system((ss.str()).c_str()) ;
	  ss.str("") ;

	  hybridInfo << std::endl ;
	}
      }

      if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str(), std::fstream::app) ; }
 
      if(outputfilestream.is_open()){
	std::cout << SET_FORECOLOR_CYAN << "WRITING DEBUG INFO TO: " << fn.c_str() << SET_DEFAULT_COLORS << std::endl ;
	outputfilestream << hybridInfo.str() ;
	outputfilestream.close() ;
      }
      pvssInfoCompLed->setPixmap(greenLed_) ;
    }
    catch (FecExceptionHandler &e) {

      ErrorMessage ("I2C problem while reading CTS/BE info from RP hybrid." ) ;
      pvssInfoCompLed->setPixmap(redLed_) ;
    }   
  }
}


/** \brief Get complete address for panel's VFAT
 * Get complete address for panel's VFAT
 */
keyType vfat_controller::getPosition(){

  keyType index = 0 , fecSlot = 0, ringSlot = 0, ccuAddress = 0, channel = 0 ;

  if ( fecSlots->count() && ccuAddresses->count() && i2cChannels->count() && deviceAddresses->count() ) {

    if ( sscanf (fecSlots->currentText(), "%d,%d", &fecSlot, &ringSlot) &&
	 sscanf (ccuAddresses->currentText(), "%x", &ccuAddress) &&
	 sscanf (i2cChannels->currentText(), "%x", &channel) && 
	 sscanf (deviceAddresses->currentText(), "%x", &index) ) {

      return (buildCompleteKey(fecSlot, ringSlot, ccuAddress, channel, index)) ;
    }
    else {  ErrorMessage ("The slot entered is not correct or empty", "Please scan FECs and CCUs reading ID" ) ; }

  }
  else {  ErrorMessage ("The slot entered is not correct or empty", "Please scan FECs and CCUs reading ID" ) ; }

  return ((keyType)0x00) ;
}

/** \brief Read the chip identification
 * Read the chip identification
 */
void vfat_controller::readVfatId(){

  if ( fecSlots->count() && ccuAddresses->count() && i2cChannels->count() && deviceAddresses->count() ) {

    try {

      vfatAccess *vfat = NULL ;

      vfat = (vfatAccess *)fecAccessManager_->getAccess(VFAT, getPosition()) ;

      std::stringstream chipid ;
	  	  
      chipid << "0x" << IntToHex((int)vfat->getVfatChipid1()) << IntToHex((int)vfat->getVfatChipid0()) ;
      vfatChipidT->setText (chipid.str()) ;
      chipid.flush() ;   
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Unable to read chip id.", e) ; }
  }
}

/*********************************************************************************/

/** \brief Read the counters' values and fill the fields
 * Read the counters' values and fill the fields
 */
void vfat_controller::readCounters_pressed(){

  writeCompLed->setPixmap(blackLed_) ;
  readCompLed->setPixmap(blackLed_) ;

  if (deviceAddresses->count() <= 0) {

    ErrorMessage ("No VFATs to be read.", "Please try to find them again.") ;
    readCompLed->setPixmap(redLed_) ;
    return ;
  }

  vfatAccess *vfat = NULL ;

  std::stringstream upset, counter, counter0, counter1, counter2 , hybridT ;

  try{

    if (vfat != (vfatAccess *)fecAccessManager_->getAccess(VFAT, getPosition())){ vfat = (vfatAccess *)fecAccessManager_->getAccess(VFAT, getPosition()) ; }

    upset << std::dec << (int)vfat->getVfatUpset() ;
    vfatUpsetT->setText (upset.str()) ;

    counter2 << std::hex << (int)vfat->getVfatHitCount2() ;
    if(counter2.str().size()<=1){ counter << "0" << counter2.str() ; }
    else{ counter << counter2.str() ; }

    counter1 << std::hex << (int)vfat->getVfatHitCount1() ;
    if(counter1.str().size()<=1){ counter << "0" << counter1.str() ; }
    else{ counter << counter1.str() ; }

    counter0 << std::hex << (int) vfat->getVfatHitCount0();
    if(counter0.str().size()<=1){ counter << "0" << counter0.str() ; }
    else{ counter << counter0.str() ; }

    vfatHitcountT->setText(IntToString(HexToInt(counter.str()))) ;

    hybridT << (int)getDcuTemperature() ; // Round to full degrees
    hybridT.put('°') ;
    hybridT.put('C') ;
    hybridTempT->setText(hybridT.str()) ;
    hybridT.flush() ;

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "*********************************" << std::endl ;
    std::cout << "COUNTERS (2-0):" << counter2.str() << " " << counter1.str() << " " << counter0.str() << std::endl ;
    std::cout << "TOTAL -> " << counter.str() << std::endl ;
    std::cout << "*********************************" << SET_DEFAULT_COLORS << std::endl ;
#endif

    upset.flush() ;
    counter0.flush() ;
    counter1.flush() ;
    counter2.flush() ;
    counter.flush() ;

    readCompLed->setPixmap(greenLed_) ;
  }

  catch (FecExceptionHandler &e) {

    ErrorMessage ("Read operation failed.", e) ;
    readCompLed->setPixmap(redLed_) ;
  }
}

/***********************************************
 * Read the values selected and fill the fields
 ***********************************************/
void vfat_controller::readChip_pressed(){

  if (deviceAddresses->count() <= 0) {

    ErrorMessage ("No VFATs to be read.", "Please try to find them again.") ;
    readCompLed->setPixmap(redLed_) ;
    return ;
  }

  vfatAccess *vfat = NULL ;

  try{

    if (vfat != (vfatAccess *)fecAccessManager_->getAccess(VFAT, getPosition())){ vfat = (vfatAccess *)fecAccessManager_->getAccess(VFAT, getPosition()) ; }

    if (vfat == NULL) { throw FecExceptionHandler() ; }
    else {

      if (cr0Sel->isChecked()){
	tscType8 value = vfat->getVfatControl0() ;
	setCr0(value) ; 
      }
      if (cr1Sel->isChecked()){
	tscType8 value = vfat->getVfatControl1() ;
	setCr1(value) ; 
      }
      if (cr2Sel->isChecked()){
	tscType8 value = vfat->getVfatControl2() ;
	setCr2(value) ; 
      }
      if (cr3Sel->isChecked()){
	tscType8 value = vfat->getVfatControl3() ;
	setCr3(value) ; 
      }
      if (IPreampInSel->isChecked()){
	std::stringstream ipreampin ;
	ipreampin << std::dec << (int)vfat->getVfatIPreampIn() ;
	vfatIPreampInT->setText (ipreampin.str()) ;
	ipreampin.flush() ;
      }
      if (IPreampFeedSel->isChecked()){
	std::stringstream ipreampfeed ;
	ipreampfeed << std::dec << (int)vfat->getVfatIPreampFeed() ;
	vfatIPreampFeedT->setText (ipreampfeed.str()) ;
	ipreampfeed.flush() ;
      }
      if (IPreampOutSel->isChecked()){
	std::stringstream ipreampout ;
	ipreampout << std::dec << (int)vfat->getVfatIPreampOut() ;
	vfatIPreampOutT->setText (ipreampout.str()) ;
	ipreampout.flush() ;
      }
      if (IShaperSel->isChecked()){ 
	std::stringstream ishaper ;
	ishaper << std::dec << (int)vfat->getVfatIShaper() ;
	vfatIShaperT->setText (ishaper.str()) ;
	ishaper.flush() ;
      }
      if (IShaperFeedSel->isChecked()){
	std::stringstream ishaperfeed ;
	ishaperfeed << std::dec << (int)vfat->getVfatIShaperFeed() ;
	vfatIShaperFeedT->setText (ishaperfeed.str()) ;
	ishaperfeed.flush() ;
      }
      if (ICompSel->isChecked()){
	std::stringstream icomp ;
	icomp << std::dec << (int)vfat->getVfatIComp() ;
	vfatICompT->setText (icomp.str()) ;
	icomp.flush() ;
      }
      if (latencySel->isChecked()){
	std::stringstream lat ;
	lat << std::dec << (int)vfat->getVfatLat() ;
	vfatLatencyT->setText (lat.str()) ;
	lat.flush() ;
      }

      if (chanRegSel->isChecked()){
	setAllChReg(vfat->getVfatAllChanReg()) ;

	// Show channel register fields selected
	if (range1->isChecked()){ setChannelRegisters(1) ; }
	else if (range2->isChecked()){ setChannelRegisters(2) ; }
	else if (range3->isChecked()){ setChannelRegisters(3) ; }
	else if (range4->isChecked()){ setChannelRegisters(4) ; }

      }
      if (VCalSel->isChecked()){
	std::stringstream vcal ;
	vcal << std::dec << (int)vfat->getVfatVCal() ;
	vfatVCalT->setText (vcal.str()) ;
	vcal.flush() ;
      }
      if (VThreshold1Sel->isChecked()){
	std::stringstream vthreshold1 ;
	vthreshold1 << std::dec << (int)vfat->getVfatVThreshold1() ;
	vfatVThreshold1T->setText (vthreshold1.str()) ;
	vthreshold1.flush() ;
      }
      if (VThreshold2Sel->isChecked()){
	std::stringstream vthreshold2 ;
	vthreshold2 << std::dec << (int)vfat->getVfatVThreshold2() ;
	vfatVThreshold2T->setText (vthreshold2.str()) ;
	vthreshold2.flush() ;
      }
      if (calPhaseSel->isChecked()){
	tscType8 value = vfat->getVfatCalPhase() ;
	setCalPhase(value) ; 
      }
    }
  }
  catch (FecExceptionHandler &e) {

    ErrorMessage ("VFAT Access could not be made.", e) ;
    readCompLed->setPixmap(redLed_) ;
  }
  readCompLed->setPixmap(greenLed_) ;
}

/** \brief Write all the registers' values, fill the fields and do the comparison if needed
 * Write all the registers' values, fill the fields and do the comparison if needed
 */
void vfat_controller::writeChip_pressed(){

  writeCompLed->setPixmap(blackLed_) ;
  readCompLed->setPixmap(blackLed_) ;

  // Read final changes from channel register fields
  if (range1->isChecked()){ getChannelRegisters(1) ; }
  else if (range2->isChecked()){ getChannelRegisters(2) ; }
  else if (range3->isChecked()){ getChannelRegisters(3) ; }
  else if (range4->isChecked()){ getChannelRegisters(4) ; }

  if (deviceAddresses->count() <= 0) {

    ErrorMessage ("No VFATs to be written.", "Please try to find them again.") ;
    writeCompLed->setPixmap(redLed_) ;
    return ;
  }

  int vfatsToWrite = 1 ;
  if(writeAllVfatSel->isChecked()){ vfatsToWrite = vfatAddresses_.size() ; }

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "Write values to " << vfatsToWrite << " VFATs" << SET_DEFAULT_COLORS << std::endl ;
#endif

  int errors = 0 ;
  vfatAccess *vfat = NULL ;

  uint ipreampin, ipreampfeed, ipreampout, ishaper, ishaperfeed, icomp, lat, vcal, vthreshold1, vthreshold2 ;
        
  try{

    for(int i=0;i<vfatsToWrite;i++){

      if(writeAllVfatSel->isChecked()){

	if (vfat != (vfatAccess *)fecAccessManager_->getAccess(VFAT, vfatAddresses_.at(i))){
	  vfat = (vfatAccess *)fecAccessManager_->getAccess(VFAT, vfatAddresses_.at(i)) ;
	}
      }
      else{

	if (vfat != (vfatAccess *)fecAccessManager_->getAccess(VFAT, getPosition())){
	  vfat = (vfatAccess *)fecAccessManager_->getAccess(VFAT, getPosition()) ;
	}
      }

      if (cr1Sel->isChecked()){
	vfat->setVfatControl1(getCr1()) ;
	if (writeCompareSel->isChecked()) {
	  if( getCr1() != vfat->getVfatControl1()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CR1" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cr2Sel->isChecked()){
	vfat->setVfatControl2(getCr2()) ;
	if (writeCompareSel->isChecked()) {
	  if( getCr2() != vfat->getVfatControl2()){
	    errors+=1 ; std::cout << SET_FORECOLOR_RED << "ERROR in CR2" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cr3Sel->isChecked()){
	vfat->setVfatControl3(getCr3()) ;
	if (writeCompareSel->isChecked()) {
	  if( getCr3() != vfat->getVfatControl3()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CR3" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (IPreampInSel->isChecked()){ 
	if (sscanf (vfatIPreampInT->text(), "%d", &ipreampin)) {
	  vfat->setVfatIPreampIn((tscType8)ipreampin) ; 
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)ipreampin != vfat->getVfatIPreampIn()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in IPreampIn" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (IPreampFeedSel->isChecked()){ 
	if (sscanf (vfatIPreampFeedT->text(), "%d", &ipreampfeed)) { 
	  vfat->setVfatIPreampFeed((tscType8)ipreampfeed) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)ipreampfeed != vfat->getVfatIPreampFeed()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in IPreampFeed" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (IPreampOutSel->isChecked()){ 
	if (sscanf (vfatIPreampOutT->text(), "%d", &ipreampout)) {
	  vfat->setVfatIPreampOut((tscType8)ipreampout) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)ipreampout != vfat->getVfatIPreampOut()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in IPreampOut" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (IShaperSel->isChecked()){ 
	if (sscanf (vfatIShaperT->text(), "%d", &ishaper)) {
	  vfat->setVfatIShaper((tscType8)ishaper) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)ishaper != vfat->getVfatIShaper()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in IShaper" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (IShaperFeedSel->isChecked()){ 
	if (sscanf (vfatIShaperFeedT->text(), "%d", &ishaperfeed)) { 
	  vfat->setVfatIShaperFeed((tscType8)ishaperfeed) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)ishaperfeed != vfat->getVfatIShaperFeed()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in IShaperFeed" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (ICompSel->isChecked()){ 
	if (sscanf (vfatICompT->text(), "%d", &icomp)) {
	  vfat->setVfatIComp((tscType8)icomp) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)icomp != vfat->getVfatIComp()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in IComp" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (latencySel->isChecked()){ 
	if (sscanf (vfatLatencyT->text(), "%d", &lat)) { 
	  vfat->setVfatLat((tscType8)lat) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)lat != vfat->getVfatLat()){ errors+=1 ; std::cout << SET_FORECOLOR_RED << "ERROR in Latency" << SET_DEFAULT_COLORS << std::endl ; }
	  }
	}
      }

      if (chanRegSel->isChecked()){ 
	vfat->setVfatAllChanReg(getAllChReg()) ;
	if (writeCompareSel->isChecked()) {
  
	  for (int i=1;i<=VFAT_CH_NUM_MAX;i++){

	    if(getChReg(i)!=vfat->getVfatChanReg(i)) {
	      std::cout << SET_FORECOLOR_RED << "ERROR in Channel Regs" << std::endl ;
	      std::cout << std::dec << "Channel<" << i << "> " << (int)getChReg(i) << "/" << (int)vfat->getVfatChanReg(i) << SET_DEFAULT_COLORS << std::endl ;
	      errors+=1 ;
	    }
	  }	      
	}
      }
      if (VCalSel->isChecked()){ 
	if (sscanf (vfatVCalT->text(), "%d", &vcal)) { 
	  vfat->setVfatVCal((tscType8)vcal) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)vcal != vfat->getVfatVCal()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in VCal" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (VThreshold1Sel->isChecked()){ 
	if (sscanf (vfatVThreshold1T->text(), "%d", &vthreshold1)) { 
	  vfat->setVfatVThreshold1((tscType8)vthreshold1) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)vthreshold1 != vfat->getVfatVThreshold1()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in VThreshold 1" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (VThreshold2Sel->isChecked()){ 
	if (sscanf (vfatVThreshold2T->text(), "%d", &vthreshold2)) {
	  vfat->setVfatVThreshold2((tscType8)vthreshold2) ;
	  if (writeCompareSel->isChecked()) {
	    if( (tscType8)vthreshold2 != vfat->getVfatVThreshold2()){
	      errors+=1 ;
	      std::cout << SET_FORECOLOR_RED << "ERROR in VThreshold 2" << SET_DEFAULT_COLORS << std::endl ;
	    }
	  }
	}
      }
      if (calPhaseSel->isChecked()){
	vfat->setVfatCalPhase(getCalPhase()) ;
	if (writeCompareSel->isChecked()) {
	  if( getCalPhase() != vfat->getVfatCalPhase()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CalPhase" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cr0Sel->isChecked()){
	vfat->setVfatControl0(getCr0()) ; 
	if (writeCompareSel->isChecked()) {
	  if( getCr0() != vfat->getVfatControl0()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CR0" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
    }
  }
  catch (FecExceptionHandler &e) {

    ErrorMessage ("VFAT Access could not be made.", e) ;
    writeCompLed->setPixmap(redLed_) ;
  }

  if(errors!=0){
    writeCompLed->setPixmap(redLed_) ;
    std::cout << SET_FORECOLOR_RED << std::dec << (int)errors << " mitcmatch(s) occured during the write." << SET_DEFAULT_COLORS << std::endl ;
  }
  else { 

    if (writeAllVfatSel->isChecked()) { writeAllVfatSel->setChecked(false) ; }
    if (writeCompareSel->isChecked()) { writeCompLed->setPixmap(greenLed_) ; }
  }
}

/** \brief Read vfatDescription values from file to the fields
 * Read vfatDescription values from file to the fields
 */
void vfat_controller::loadXML_pressed(){

  std::cout << SET_FORECOLOR_BLUE << "Load description values from XML file" << SET_DEFAULT_COLORS << std::endl ;

  QString fn = QFileDialog::getOpenFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;
  
  if (! fn.isEmpty ()) {

    singleFileName->setText (fn) ;

    if (!range1->isChecked()){ calAnalog->setDisabled(true) ; }
    
    try {

      TotemFecFactory fecFactory ;

      // Parse a file for PIA reset and for devices
      //fecFactory.addFileName (fn.latin1()) ;
	
      // Load it from file
      deviceVector vDevice ;
      fecFactory.getFecDeviceDescriptions(vDevice, fn.latin1()) ;

#ifdef DEBUG_VFAT_CONTROLLER
      std::cout << SET_FORECOLOR_CYAN << vDevice.size() << " descriptions found from XML file" << SET_DEFAULT_COLORS << std::endl ;
#endif

      for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {

        deviceDescription *deviced = *device ;
        vfatDescription *vfatDevice    = NULL ;

        // Find the correct item
        char msg[80] ;
        switch (deviced->getDeviceType()) {
        case VFAT:

          vfatDevice = (vfatDescription *)deviced ;
          std::stringstream ipreampin, ipreampfeed, ipreampout, ishaper, ishaperfeed, icomp, lat, vcal, vthreshold1, vthreshold2 ;

#ifdef DEBUG_VFAT_CONTROLLER
	  std::cout << SET_FORECOLOR_CYAN << "Description values loaded from XML file" << SET_DEFAULT_COLORS << std::endl ;
	  vfatDevice->display() ;
#endif
	      
          // Set the values on text

	  setCr0((uint)vfatDevice->getControl0()) ;
	  setCr1((uint)vfatDevice->getControl1()) ;
	  setCr2((uint)vfatDevice->getControl2()) ;
	  setCr3((uint)vfatDevice->getControl3()) ;

	  ipreampin << std::dec << (int)vfatDevice->getIPreampIn() ;
	  vfatIPreampInT->setText (ipreampin.str()) ;
	  ipreampin.flush() ;	      
	  ipreampfeed << std::dec << (int)vfatDevice->getIPreampFeed() ;
	  vfatIPreampFeedT->setText (ipreampfeed.str()) ;
	  ipreampfeed.flush() ;
	  ipreampout << std::dec << (int)vfatDevice->getIPreampOut() ;
	  vfatIPreampOutT->setText (ipreampout.str()) ;
	  ipreampout.flush() ;
	  ishaper << std::dec << (int)vfatDevice->getIShaper() ;
	  vfatIShaperT->setText (ishaper.str()) ;
	  ishaper.flush() ;
	  ishaperfeed << std::dec << (int)vfatDevice->getIShaperFeed() ;
	  vfatIShaperFeedT->setText (ishaperfeed.str()) ;
	  ishaperfeed.flush() ;
	  icomp << std::dec << (int)vfatDevice->getIComp() ;
	  vfatICompT->setText (icomp.str()) ;
	  icomp.flush() ;
	  lat << std::dec << (int)vfatDevice->getLat() ;
	  vfatLatencyT->setText (lat.str()) ;
	  lat.flush() ;
	  vcal << std::dec << (int)vfatDevice->getVCal() ;
	  vfatVCalT->setText (vcal.str()) ;
	  vcal.flush() ;
	  vthreshold1 << std::dec << (int)vfatDevice->getVThreshold1() ;
	  vfatVThreshold1T->setText (vthreshold1.str()) ;
	  vthreshold1.flush() ;
	  vthreshold2 << std::dec << (int)vfatDevice->getVThreshold2() ;
	  vfatVThreshold2T->setText (vthreshold2.str()) ;
	  vthreshold2.flush() ;

	  setCalPhase((uint)vfatDevice->getCalPhase()) ;
	    
	  // Put all channel register values to the array
	  setAllChReg(vfatDevice->getAllChanReg()) ;

	  // Show channel register fields selected
	  if (range1->isChecked()){ setChannelRegisters(1) ; }
	  else if (range2->isChecked()){ setChannelRegisters(2) ; }
	  else if (range3->isChecked()){ setChannelRegisters(3) ; }
	  else if (range4->isChecked()){ setChannelRegisters(4) ; }
	  	    
	  break;
        }	    
      }

      if(vDevice.size()>1){ ErrorMessage ("More than one configuration found from the file. Unable to show them all -> ONLY the last vfat config shown on the panel.") ; }

    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Error during the download from file", e) ; } 
  }
  else { ErrorMessage ("Please set the filename before loading it") ; }
}

/** \brief Write the fields' values to file
 * Write the fields' values to file
 */
void vfat_controller::saveXML_pressed(){

  std::cout << SET_FORECOLOR_BLUE << "Save description values to XML file" << SET_DEFAULT_COLORS << std::endl ;
  deviceVector vDevice ;
  vfatDescription *device = NULL ;
 
  uint ipreampin, ipreampfeed, ipreampout, ishaper, ishaperfeed, icomp, lat, vcal, vthreshold1, vthreshold2, calphase ;

  if ( sscanf (vfatIPreampInT->text(), "%d", &ipreampin) && 
       sscanf (vfatIPreampFeedT->text(), "%d", &ipreampfeed) && 
       sscanf (vfatIPreampOutT->text(), "%d", &ipreampout) &&
       sscanf (vfatIShaperT->text(), "%d", &ishaper) && 
       sscanf (vfatIShaperFeedT->text(), "%d", &ishaperfeed) &&
       sscanf (vfatICompT->text(), "%d", &icomp) &&
       sscanf (vfatLatencyT->text(), "%d", &lat) &&
       sscanf (vfatVCalT->text(), "%d", &vcal) && 
       sscanf (vfatVThreshold1T->text(), "%d", &vthreshold1) && 
       sscanf (vfatVThreshold2T->text(), "%d", &vthreshold2)) {

    // Read final changes from channel register fields
    if (range1->isChecked()){ getChannelRegisters(1) ; }
    else if (range2->isChecked()){ getChannelRegisters(2) ; }
    else if (range3->isChecked()){ getChannelRegisters(3) ; }
    else if (range4->isChecked()){ getChannelRegisters(4) ; }

    device = new vfatDescription ( (tscType8)getCr0(), (tscType8)getCr1(), 
				   (tscType8)ipreampin, (tscType8)ipreampfeed, 
				   (tscType8)ipreampout, (tscType8)ishaper,
				   (tscType8)ishaperfeed, (tscType8)icomp, 
				   (tscType8)0x00 ,(tscType8)0x00, // Chipids
				   (tscType8)0x00, (tscType8)0x00, // Upset, Hitcount0
				   (tscType8)0x00, (tscType8)0x00, // Hitcount1, Hitcount2
				   (tscType8)lat, getAllChReg(), 
				   (tscType8)vcal, (tscType8)vthreshold1,
 				   (tscType8)vthreshold2, (tscType8)getCalPhase(),
                                   (tscType8)getCr2(), (tscType8)getCr3() ) ;

#ifdef DEBUG_VFAT_CONTROLLER

    std::cout << SET_FORECOLOR_CYAN << "Description values saved to XML file" << SET_DEFAULT_COLORS << std::endl ;
    device->display() ;

#endif

    vDevice.push_back(device) ;
  }
  else { ErrorMessage ("One or several VFAT parameters are not correct") ; }

  if (vDevice.empty()) {  ErrorMessage ("The devices saved in XML file comes from the upload set. So download and upload values") ; }

  // Try to write file
  QString fn = QFileDialog::getSaveFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;
      
  if (! fn.isEmpty ()) {

    if (!fn.endsWith( ".xml" )){ fn.append(".xml") ; }

    try {

      TotemFecFactory fecFactory ;
      //fecFactory.setOutputFileName (fn.latin1()) ;
      fecFactory.setFecDeviceDescriptions (vDevice, fn.latin1()) ;
      singleFileName->setText (fn) ;
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Error during the upload in file", e) ; }
  }
  else { ErrorMessage ("Error: unable to perform an upload, the ouput buffer is not initialised") ; }  

}

void vfat_controller::readMultiXML_pressed(){

  QString fn = multiFileName->text() ;

  if (fn.isEmpty ()) {

    fn = QFileDialog::getSaveFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;

    if(!fn.endsWith( ".xml" )){ fn.append(".xml") ; }

    multiFileName->setText (fn) ;
  }
  
  readMultiXMLValues() ;
}

void vfat_controller::writeMultiXML_pressed(){

  QString fn = multiFileName->text() ;

  if (fn.isEmpty ()) {

    fn = QFileDialog::getOpenFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ; 

    if (!fn.endsWith( ".xml" )){ fn.append(".xml") ; }

    multiFileName->setText (fn) ;
  }

  writeMultiXMLValues() ;
}

void vfat_controller::findMultiXML(){

  QString fn = QFileDialog::getOpenFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;
  
  if (! fn.isEmpty ()) { multiFileName->setText (fn) ; }
}

void vfat_controller::writeMultiXMLValues(){

  QString fn = multiFileName->text() ;

  if (!fn.isEmpty ()) {

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "Load description values from " << multiFileName->text() << SET_DEFAULT_COLORS << std::endl ;
#endif  

    try {

      TotemFecFactory fecFactory ;
      deviceVector vDevice ;

      PiaResetFactory piaResetFactory ;
      piaResetVector vPiaReset ;

      // Parse the file for devices
      fecFactory.getFecDeviceDescriptions(vDevice, fn.latin1()) ;

      piaResetFactory.setInputFileName(fn.latin1()) ;
      piaResetFactory.getPiaResetDescriptions(vPiaReset) ;

      if(!vPiaReset.empty()){ resetPIA(vPiaReset) ; }

      if(!vDevice.empty()){ configureDevices(vDevice) ; }

    }
    catch (FecExceptionHandler &e) {

      std::cout << SET_FORECOLOR_RED << "Error during the download from file" << SET_DEFAULT_COLORS << std::endl ; 
      multiWriteCompLed->setPixmap(redLed_) ;
    }

    multiWriteCompLed->setPixmap(greenLed_) ;
  }
  else {

    std::cout << SET_FORECOLOR_RED << "XML filename empty -> No values to write." << SET_DEFAULT_COLORS << std::endl ;
    multiWriteCompLed->setPixmap(redLed_) ;
  }
}

void vfat_controller::readMultiXMLValues(){

  QString fn = multiFileName->text() ;

  if (!fn.isEmpty ()) {

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_BLUE << "Save description values to " << multiFileName->text() << SET_DEFAULT_COLORS << std::endl ;
#endif  

    try {

      deviceVector *vDevice = NULL ;
      std::list<FecExceptionHandler *> errorList  ;

      vDevice = fecAccessManager_->uploadValues ( errorList ) ;
      displayErrorMessage ("Error during the download", errorList) ;

      TotemXMLFecDevice xmlFecDevice ;
      xmlFecDevice.setDeviceVector(*vDevice);
      xmlFecDevice.setDevices (fn.latin1()) ;

#ifdef DEBUG_VFAT_CONTROLLER
      if(vDevice!=NULL{ std::cout << SET_FORECOLOR_CYAN << std::dec << vDevice->size() << " descriptions written into XML file." << SET_DEFAULT_COLORS << std::endl ; }
#endif

      delete vDevice ;


    }
    catch (FecExceptionHandler &e) {

      std::cout << SET_FORECOLOR_RED << "Error while saving description values into XML file." << SET_DEFAULT_COLORS << std::endl ; 
      multiReadCompLed->setPixmap(redLed_) ;
    }

    multiReadCompLed->setPixmap(greenLed_) ;
  }
  else {

    std::cout << SET_FORECOLOR_RED << "XML filename empty -> No values to write." << SET_DEFAULT_COLORS << std::endl ;
    multiReadCompLed->setPixmap(redLed_) ;
  }
}

void vfat_controller::createDevices( deviceVector vDevice ){

  vfatAddresses_.clear() ;
  dcuAddresses_.clear() ;
  cchipAddresses_.clear() ;

  // Remove existing devices before adding new ones
  try {

    fecAccessManager_->removeDevices () ;
  }
  catch (FecExceptionHandler &e) { ErrorMessage("ACCESS MANAGER::REMOVE DEVICES", e) ; }


  std::cout << SET_FORECOLOR_BLUE << "There were " << vDevice.size() << " device(s) in the file. Adding them now..."<< SET_DEFAULT_COLORS << std::endl ;

  for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {

    deviceDescription *deviced = *device ;

    std::cout << SET_FORECOLOR_BLUE << "FEC ("
	      << std::hex << (int)deviced->getFecSlot() << ","
	      << std::hex << (int)deviced->getRingSlot() << ") CCU 0x" 
	      << std::hex << (int)deviced->getCcuAddress() << " CHANNEL 0x" 
	      << std::hex << (int)deviced->getChannel() << " ADDRESS 0x" 
	      << std::hex << (int)deviced->getAddress() << std::dec ;

    try {


      switch (deviced->getDeviceType()){

      case VFAT:

	std::cout << SET_FORECOLOR_BLUE << " - VFAT" << SET_DEFAULT_COLORS << std::endl ;
	vfatAccess_ = new vfatAccess(fecAccess_, deviced->getKey()) ;
	vfatAddresses_.push_back(deviced->getKey()) ;

	if (vfatAccess_ != (vfatAccess *)fecAccessManager_->getAccess(VFAT, deviced->getKey())){ fecAccessManager_->setAccess((deviceAccess*)vfatAccess_) ; }

	break ;

      case DCU:

	std::cout << SET_FORECOLOR_BLUE << " - DCU" << SET_DEFAULT_COLORS << std::endl ;
	dcuAccess_ = new dcuAccess(fecAccess_, deviced->getKey()) ;
	dcuAddresses_.push_back(deviced->getKey()) ;

	if (dcuAccess_ != (dcuAccess *)fecAccessManager_->getAccess(DCU, deviced->getKey())){ fecAccessManager_->setAccess((deviceAccess*)dcuAccess_) ; }

	break;

	
      case CCHIP:

	std::cout << SET_FORECOLOR_BLUE << " - CCHIP" << SET_DEFAULT_COLORS << std::endl ;
	cchipAccess_ = new totemCChipAccess(fecAccess_, deviced->getKey()) ;
	cchipAddresses_.push_back(deviced->getKey()) ;

	if (cchipAccess_ != (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, deviced->getKey())){ fecAccessManager_->setAccess((deviceAccess*)cchipAccess_) ; }
	
       	break;

	
      case PLL:
   
   	std::cout << SET_FORECOLOR_BLUE << " - PLL" << SET_DEFAULT_COLORS << std::endl ;
	pllAccess *pll = new pllAccess(fecAccess_, deviced->getKey()) ;

	if (pll != (pllAccess *)fecAccessManager_->getAccess(PLL, deviced->getKey())){ fecAccessManager_->setAccess((deviceAccess*)pll) ; }

       	break ;	
	
      case DOH:	
      case LASERDRIVER: // a.k.a DOH

	std::cout << SET_FORECOLOR_BLUE << " - LASERDRIVER (DOH)" << SET_DEFAULT_COLORS << std::endl ;
	laserdriverAccess *laserdriver = new laserdriverAccess(fecAccess_, deviced->getKey()) ;

	if (laserdriver != (laserdriverAccess *)fecAccessManager_->getAccess(LASERDRIVER, deviced->getKey())){ fecAccessManager_->setAccess((deviceAccess*)laserdriver) ; }

        break;
	

      case GOH:

	std::cout << SET_FORECOLOR_BLUE << " - GOH" << SET_DEFAULT_COLORS << std::endl ;
	gohAccess *goh = new gohAccess(fecAccess_, deviced->getKey()) ;

	if (goh != (gohAccess *)fecAccessManager_->getAccess(GOH, deviced->getKey())){ fecAccessManager_->setAccess((deviceAccess*)goh) ; }

	break ;


      case PHILIPS:

	std::cout << SET_FORECOLOR_BLUE << " - PHILIPS (INDIVIDUAL REG.)" << SET_DEFAULT_COLORS << std::endl ;
	philipsAccess *philips = new philipsAccess(fecAccess_, deviced->getKey()) ;

	if (philips != (philipsAccess *)fecAccessManager_->getAccess(PHILIPS, deviced->getKey())){ fecAccessManager_->setAccess((deviceAccess*)philips) ; }

	break ;

      default:
	std::cout << SET_FORECOLOR_RED << " - UNKNOWN" << SET_DEFAULT_COLORS << std::endl ;
	std::ostringstream errorMsg ;
	errorMsg << "Unknown deviceType code : " << (int)deviced->getDeviceType() << std::endl;
	RAISEFECEXCEPTIONHANDLER ( CODECONSISTENCYERROR, errorMsg.str(), FATALERRORCODE) ;

      }
    }
    catch (FecExceptionHandler &e) { ErrorMessage("ACCESS MANAGER::ADD DEVICE", e) ; }
    catch (...){ std::cout << SET_FORECOLOR_RED << "UNKNOWN ERROR" << SET_DEFAULT_COLORS << std::endl ; }

  }

  if(!vfatAddresses_.empty()){

    // Update CCU, channel and device address fields in the GUI
    refreshCcuAddresses() ;

    // Put the found devices to test panel list
    fillVfatTestList() ;
  }

  if(!cchipAddresses_.empty()){

    // Update CCU, channel and device address fields in the GUI
    refreshCcuAddresses3() ;
  }

}

void vfat_controller::configureDevices( deviceVector vDevice ){

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << std::dec << vDevice.size() << " descriptions found from XML file." << SET_DEFAULT_COLORS << std::endl ;
#endif

  try {

    if (vDevice.size() > 0) {

      std::list<FecExceptionHandler *> errorList  ;
      uint error = fecAccessManager_->downloadValues ( &vDevice, errorList ) ;
      displayErrorMessage ("Error during the download", errorList) ;
    }
  }
  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "Error during the download from file" << SET_DEFAULT_COLORS << std::endl ; }
  catch (...) { std::cout << SET_FORECOLOR_RED << "Unknown error occured." << SET_DEFAULT_COLORS << std::endl ; }

}

 void vfat_controller::resetPIA( piaResetVector vPiaReset ){

#ifdef DEBUG_VFAT_CONTROLLER
   std::cout << SET_FORECOLOR_CYAN << std::dec << vPiaReset.size() << " reset descriptions found from XML file." << SET_DEFAULT_COLORS << std::endl ;
#endif

   try {

     if (!vPiaReset.empty()) {

       std::list<FecExceptionHandler *> errorList  ;
       uint error = fecAccessManager_->resetPia ( &vPiaReset, errorList ) ;
       displayErrorMessage ("Error during the download", errorList) ;
     }
   }
   catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "Error during the download from file" << SET_DEFAULT_COLORS << std::endl ; }
   catch (...) { std::cout << SET_FORECOLOR_RED << "Unknown error occured." << SET_DEFAULT_COLORS << std::endl ; }
 }


