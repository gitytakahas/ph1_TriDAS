/*
Authors:
Juha Petajajarvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#include <stdlib.h> // Standard

#include <qcombobox.h> // GUI
#include <qlineedit.h>
#include <qcheckbox.h> 
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qfiledialog.h>

#include "vfat_controller.h" // SW header

/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   Test Panel                                                                        */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */ 

void vfat_controller::testPulseOneChannelSel_pressed(){

  if (testPulseOneChannelSel->isChecked()){
    testPulseVCal1->setDisabled(true) ;
    testPulseVCal1T->setDisabled(true) ;
    testPulseMinus->setDisabled(true) ;
    testPulseVCal2T->setDisabled(true) ;
    testPulseAllChannelsSel->setDisabled(true) ;
    testPulseTrimDACSel->setDisabled(true) ;
  }
  else {
    testPulseVCal1->setDisabled(false) ;
    testPulseVCal1T->setDisabled(false) ;
    testPulseMinus->setDisabled(false) ;
    testPulseVCal2T->setDisabled(false) ;
    testPulseAllChannelsSel->setDisabled(false) ;
    testPulseTrimDACSel->setDisabled(false) ;
  }
}


void vfat_controller::testThresholdMeasurementSel_pressed(){
  
  if (testThresholdMeasurementSel->isChecked()){
    testThresholdVThreshold1T->setDisabled(true) ;
    testThresholdVThreshold1->setDisabled(true) ;
    testThresholdVThreshold12T->setDisabled(true) ;
    testThresholdMinus->setDisabled(true) ;
    testThresholdVThreshold2T->setDisabled(true) ;
    testThresholdVThreshold2->setDisabled(true) ;
    testThresholdInvertedSel->setDisabled(true) ;
  }
  else {
    testThresholdVThreshold1T->setDisabled(false) ;
    testThresholdVThreshold1->setDisabled(false) ;
    testThresholdVThreshold12T->setDisabled(false) ;
    testThresholdMinus->setDisabled(false) ;
    testThresholdVThreshold2T->setDisabled(false) ;
    testThresholdVThreshold2->setDisabled(false) ;
    testThresholdInvertedSel->setDisabled(false) ;
  }  
}

void vfat_controller::testLatencyScanSel_pressed() {

  if (testLatencyScanSel->isChecked()){
    testSLatencyScanLatency1->setDisabled(true) ;
    testLatencyScanLatency1T->setDisabled(true) ;
    testLatencyScanMinus->setDisabled(true) ;
    testLatencyScanLatency2T->setDisabled(true) ;
    testLatencyScanAllPhasesSel->setDisabled(true) ;
  }
  else {
    testSLatencyScanLatency1->setDisabled(false) ;
    testLatencyScanLatency1T->setDisabled(false) ;
    testLatencyScanMinus->setDisabled(false) ;
    testLatencyScanLatency2T->setDisabled(false) ;
    testLatencyScanAllPhasesSel->setDisabled(false) ;
  } 
}

void vfat_controller::testPulseVCal1T_lostFocus(){

  if (atoi(testPulseVCal1T->text())>VFAT_REG_VALUE_MAX || atoi(testPulseVCal1T->text())<=0){  
    if (atoi(testPulseVCal1T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      testPulseVCal1T->setText("0") ;
      testPulseVCal1T->setFocus() ;
    }
    else { testPulseVCal1T->setText("0") ; }
  }
}

void vfat_controller::testPulseVCal2T_lostFocus(){

  if (atoi(testPulseVCal2T->text())>VFAT_REG_VALUE_MAX || atoi(testPulseVCal2T->text())<=0){  
    if (atoi(testPulseVCal2T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      testPulseVCal2T->setText("0") ;
      testPulseVCal2T->setFocus() ;
    }
    else { testPulseVCal2T->setText("0") ; }
  }
}

void vfat_controller::testThresholdVThreshold1T_lostFocus(){

  if (atoi(testThresholdVThreshold1T->text())>VFAT_REG_VALUE_MAX || atoi(testThresholdVThreshold1T->text())<=0){  
    if (atoi(testThresholdVThreshold1T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      testThresholdVThreshold1T->setText("0") ;
      testThresholdVThreshold1T->setFocus() ;
    }
    else { testThresholdVThreshold1T->setText("0") ; }
  }
}

void vfat_controller::testThresholdVThreshold2T_lostFocus(){

  if (atoi(testThresholdVThreshold2T->text())>VFAT_REG_VALUE_MAX || atoi(testThresholdVThreshold2T->text())<=0){  
    if (atoi(testThresholdVThreshold2T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      testThresholdVThreshold2T->setText("0") ;
      testThresholdVThreshold2T->setFocus() ;
    }
    else { testThresholdVThreshold2T->setText("0") ; }
  }
}

void vfat_controller::testThresholdVThreshold12T_lostFocus(){

  if (atoi(testThresholdVThreshold12T->text())>VFAT_REG_VALUE_MAX || atoi(testThresholdVThreshold12T->text())<=0){  
    if (atoi(testThresholdVThreshold12T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      testThresholdVThreshold12T->setText("0") ;
      testThresholdVThreshold12T->setFocus() ;
    }
    else { testThresholdVThreshold12T->setText("0") ; }
  }
}

void vfat_controller::testLatencyScanLatency1T_lostFocus(){

  if (atoi(testLatencyScanLatency1T->text())>VFAT_REG_VALUE_MAX || atoi(testLatencyScanLatency1T->text())<=0){  
    if (atoi(testLatencyScanLatency1T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      testLatencyScanLatency1T->setText("0") ;
      testLatencyScanLatency1T->setFocus() ;
    }
    else { testLatencyScanLatency1T->setText("0") ; }
  }
}

void vfat_controller::testLatencyScanLatency2T_lostFocus(){

  if (atoi(testLatencyScanLatency2T->text())>VFAT_REG_VALUE_MAX || atoi(testLatencyScanLatency2T->text())<=0){  
    if (atoi(testLatencyScanLatency2T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      testLatencyScanLatency2T->setText("0") ;
      testLatencyScanLatency2T->setFocus() ;
    }
    else { testLatencyScanLatency2T->setText("0") ; }
  }
}

bool vfat_controller::testCheckValues(){

  bool vfatSelected = false ;

  // Any vfat(s) selected?
  for (int i=0;i<vfatAddresses_.size();i++){

    if (vfatList_test->isSelected(i)){ vfatSelected = true ; }
  }
  if(!vfatSelected){
    ErrorMessage("No Vfat(s) selected! Aborting...") ; 
    return(false) ;
  }

  if(testPulseOneChannelSel->isChecked()){
    if(atoi(testPulseVCal1T->text())>atoi(testPulseVCal2T->text())){ 
      ErrorMessage("Upper limit must be greater than lower limit") ; 
      return(false) ;
    }
  }

  if(testThresholdMeasurementSel->isChecked()){

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << std::dec << "TH1 " << testThresholdVThreshold1T->text() << ", " << atoi(testThresholdVThreshold1T->text()) << std::endl ;
    std::cout << std::dec << "TH12 " << testThresholdVThreshold12T->text() << ", " << atoi(testThresholdVThreshold12T->text()) << std::endl ;
    std::cout << std::dec << "TH2 " << testThresholdVThreshold2T->text() << ", " << atoi(testThresholdVThreshold2T->text()) << SET_DEFAULT_COLORS << std::endl ;
#endif

    if(testThresholdInvertedSel->isChecked()){
      if((atoi(testThresholdVThreshold1T->text()))<(atoi(testThresholdVThreshold12T->text()))){
	ErrorMessage("Upper limit must be greater than lower limit") ;
	return(false) ;
      }
    }
    else{
      if((atoi(testThresholdVThreshold1T->text()))>(atoi(testThresholdVThreshold12T->text()))){
	ErrorMessage("For inverted scan, lower limit must be greater than upper limit") ;
	return(false) ;
      }
    }
  }

  if(testLatencyScanSel->isChecked()){
    if(atoi(testLatencyScanLatency1T->text())>atoi(testLatencyScanLatency2T->text())){
      ErrorMessage("Upper limit must be greater than lower limit") ;
      return(false) ;
    }
  }

  return(true) ;
}

void vfat_controller::fillVfatTestList(){

  vfatList_test->clear() ;

  if (vfatAddresses_.size()>0){

    char key[100] ;
    std::stringstream testListItem ;
    vfatAccess *vfat = NULL ;

    for (int i=0;i<vfatAddresses_.size();i++){
 
      try {

	decodeKey(key,vfatAddresses_.at(i)) ;

	vfat = (vfatAccess *)fecAccessManager_->getAccess(VFAT, vfatAddresses_.at(i)) ;

	testListItem << "Chip id 0x" << IntToHex((int)vfat->getVfatChipid1()) << IntToHex((int)vfat->getVfatChipid0()) ;
	testListItem << " - " << key ;

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << testListItem.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

	vfatList_test->insertItem(testListItem.str()) ;
	testListItem.str("") ;   
      }
      catch (FecExceptionHandler &e) { ErrorMessage ("Unable to read chip id.", e) ; }
    }

    testListItem.flush() ;
  }
  else { ErrorMessage("NO VFATs FOUND.") ; }

  //delete vfat ;
}

keyType vfat_controller::getPiaKey ( bool initChannel ){

  keyType index = 0 ;
  uint fecSlot, ringSlot, ccuAddress, piaChannel ;

  if (fecSlots2->count() && sscanf (fecSlots2->currentText(), "%d,%d", &fecSlot, &ringSlot)) {
    
    if (ccuAddresses2->count() && sscanf (ccuAddresses2->currentText(), "%x", &ccuAddress)) {
      
      if (piaChannels->count() && sscanf (piaChannels->currentText(), "%x", &piaChannel)) { //%d
        
        index = buildCompleteKey(fecSlot,ringSlot,ccuAddress, piaChannel,0) ;
        FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(fecSlot,ringSlot)) ;

	if(initChannel){

	  try{

	    // Enable channel if not already
	    if(!fecRingDevice->isChannelEnabled(index)){ fecRingDevice->setChannelEnable (index, true) ; }
	  }
	  catch (FecExceptionHandler &e){ ErrorMessage ("Couldn't enable the PIA channel.") ; }
	}
      }
      else { ErrorMessage ("PIA channel number entered is not correct or empty") ; }
    }
    else { ErrorMessage ("CCU address entered is not correct or empty") ; }
  }
  else { ErrorMessage ("FEC slot entered is not correct or empty") ; }

  return (index) ;
}

uint vfat_controller::getPPBDirections(){

  uint data = 0 ;

  if(outputBit0->isChecked()){ data |= 0x01 ; }
  if(outputBit1->isChecked()){ data |= 0x02 ; }
  if(outputBit2->isChecked()){ data |= 0x04 ; }
  if(outputBit3->isChecked()){ data |= 0x08 ; }
  if(outputBit4->isChecked()){ data |= 0x10 ; }
  if(outputBit5->isChecked()){ data |= 0x20 ; }
  if(outputBit6->isChecked()){ data |= 0x40 ; }
  if(outputBit7->isChecked()){ data |= 0x80 ; }

  return data ;
}

void vfat_controller::setPPBDirections( uint ivalue ){

  if(ivalue & 0x01){ outputBit0->setChecked(true) ; }
  else { inputBit0->setChecked(true) ; }

  if(ivalue & 0x02){ outputBit1->setChecked(true) ; }
  else { inputBit1->setChecked(true) ; }

  if(ivalue & 0x04){ outputBit2->setChecked(true) ; }
  else { inputBit2->setChecked(true) ; }

  if(ivalue & 0x08){ outputBit3->setChecked(true) ; }
  else { inputBit3->setChecked(true) ; }

  if(ivalue & 0x10){ outputBit4->setChecked(true) ; }
  else { inputBit4->setChecked(true) ; }

  if(ivalue & 0x20){ outputBit5->setChecked(true) ; }
  else { inputBit5->setChecked(true) ; }

  if(ivalue & 0x40){ outputBit6->setChecked(true) ; }
  else { inputBit6->setChecked(true) ; }

  if(ivalue & 0x80){ outputBit7->setChecked(true) ; }
  else { inputBit7->setChecked(true) ; }

}

void vfat_controller::readPPBDirections_pressed(){

  try {

    keyType index = getPiaKey(true) ;
    
    if (index != 0) { 

      uint value = fecAccess_->getPiaChannelDDR(index) ;
      setPPBDirections(value) ;
    }
  }
  catch (FecExceptionHandler &e) { ErrorMessage ("Unable to read the CCU parallel port directions.", e) ; }

}

void vfat_controller::writePPBDirections_pressed(){

  try {

    keyType index = getPiaKey(true) ;
    
    if (index != 0) { fecAccess_->setPiaChannelDDR(index, getPPBDirections()) ; }
  }
  catch (FecExceptionHandler &e) { ErrorMessage ("Unable to change the CCU parallel port directions.", e) ; }
}

void vfat_controller::readPPBData_pressed(){

  bool initOK = false ;
  keyType index = 0 ;


    
  index = getPiaKey(true) ;
    
  if (index != 0) {

    try {

      // Create an access
      fecAccess_->addPiaAccess (index,MODE_SHARE) ;
      initOK = true ;
    }
    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_CYAN << "Pia Access already added" << SET_DEFAULT_COLORS << std::endl ; }

    try {

      uint value = fecAccess_->getPiaChannelDataReg(index) ;
      setPPBData(value) ;
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Unable to read a data from a PIA channel", e) ; }

  }


  // Remove the access after using it

  if (initOK){

    try {

      fecAccess_->removei2cAccess (index) ;
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Couldn't release the PIA channel after reading", e) ; }
  }
}

void vfat_controller::writePPBData_pressed(){

  bool initOK = false ;
  keyType index = 0 ;
    
  index = getPiaKey(true) ;
    
  if (index != 0) {

    try {

      // Create an access
      fecAccess_->addPiaAccess (index,MODE_SHARE) ;
      initOK = true ;
    }
    catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_CYAN << "Pia Access already added" << SET_DEFAULT_COLORS << std::endl ; }

    try {

      fecAccess_->setPiaChannelDataReg(index, getPPBData()) ;
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Unable to read a data from a PIA channel", e) ; }
  }



  // Remove the access after using it

  if (initOK){

    try {

      fecAccess_->removei2cAccess (index) ;
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Couldn't release the PIA channel after reading", e) ; }
  }
}

uint vfat_controller::getPPBData(){

  uint data = 0 ;
  if (sscanf (PPBDataT->text(), "%x", &data)) { return data ; }
  else{ std::cout << SET_FORECOLOR_RED << "Couldn't get PPB data." << SET_DEFAULT_COLORS << std::endl ; return 0 ; }
}

void vfat_controller::setPPBData( uint ivalue ){

  std::stringstream data ;
  data << "0x" << std::setw(2) << std::setfill('0') << std::hex << ivalue ;
  PPBDataT->setText(data.str()) ;
  data.flush() ;
}

void vfat_controller::fecSlots2_activated( int sel ){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "---> refresh CCUs" << SET_DEFAULT_COLORS << std::endl ;
#endif

  ccuAddresses2->clear() ;

  keyType fecSlot = 0, ringSlot = 0 ;

  if (vfatAddresses_.size()>0 && sscanf (fecSlots2->currentText(), "%d,%d", &fecSlot, &ringSlot) ){

    std::stringstream ss ;
   
    for (int i=0;i<vfatAddresses_.size();i++){

#ifdef DEBUG_VFAT_CONTROLLER
      //ss << "0x" << std::hex << (int)getFecKey(vfatAddresses_.at(i)) << std::dec ;
      ss << "FEC RING " << (int)getFecKey(vfatAddresses_.at(i)) << " " << (int)getRingKey(vfatAddresses_.at(i)) << SET_DEFAULT_COLORS << std::endl ;
      std::cout << SET_FORECOLOR_CYAN << ss.str() ;
#endif

      //std::cout << SET_FORECOLOR_CYAN << "FEC " << ss.str() << " / " << fecSlots->currentText().latin1() << SET_DEFAULT_COLORS << std::endl ;

      if(fecSlot==getFecKey(vfatAddresses_.at(i)) && ringSlot==getRingKey(vfatAddresses_.at(i))){

	ss.str("") ;
	ss << "0x" << std::hex << (int)getCcuKey(vfatAddresses_.at(i)) << std::dec ;

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "CCU " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

	bool addItem = true ;

	for(int j=0;j<ccuAddresses2->count();j++){

	  if(ccuAddresses2->text(j)==(QString)ss.str()) { addItem=false ; break ; } 
	}

	if(addItem){ ccuAddresses2->insertItem(ss.str()) ; }

      }
      ss.str("") ;
    }
    ss.flush() ;

    if(ccuAddresses2->count()>0){

      ccuAddresses2->setCurrentItem(0) ; 

    }
  }
}


#ifdef TTCCI_TRIGGER

void vfat_controller::findTTCciConfig_pressed(){

  // Try to read file
  QString fn = QFileDialog::getOpenFileName( QString::null, tr( "Text Files (*.txt);;All Files (*)" ), this ) ;
  ttcciConfigFile->setText (fn) ;
}

void vfat_controller::testTrigger_pressed(){

  if(ttcci_!=NULL){

    std::cout << SET_FORECOLOR_BLUE << "Starting trigger test" << SET_DEFAULT_COLORS << std::endl ;

    std::string configfile = ttcciConfigFile->text().latin1() ;
    std::cout << SET_FORECOLOR_BLUE << "TTCci config: " << configfile << SET_DEFAULT_COLORS << std::endl ;

    std::string seq = triggerSeqName->text().latin1() ;
    std::cout << SET_FORECOLOR_BLUE << "TTCci trigger sequence: " << seq << SET_DEFAULT_COLORS << std::endl ;

    int msleeptime = atoi(triggerSleepT->text()) ;
    std::cout << SET_FORECOLOR_BLUE << "Delay " << msleeptime << " ms" << SET_DEFAULT_COLORS << std::endl ;


    std::cout << SET_FORECOLOR_BLUE << "Configure.." << SET_DEFAULT_COLORS << std::endl ;
    ttcci_configure( configfile ) ;

    std::cout << SET_FORECOLOR_BLUE << "Execute" << SET_DEFAULT_COLORS << std::endl ;
    ttcci_executeSeq( seq , msleeptime ) ;

    std::cout << SET_FORECOLOR_GREEN << "Trigger test DONE" << SET_DEFAULT_COLORS << std::endl ;
  }
  else{

    ErrorMessage ("TTCci board not initialized properly.") ;
  }
}

#endif

#ifdef VMEREADOUT

void vfat_controller::boardReset_pressed(){

  if(totfed_!=NULL){

    std::cout << SET_FORECOLOR_BLUE << "Reseting the TOTFED..." << SET_DEFAULT_COLORS << std::endl ;
    totfed_reset() ;
  }
  else{

    std::cout << SET_FORECOLOR_RED << "TOTFED board not initialized properly." << SET_DEFAULT_COLORS << std::endl ;
  }
}

#endif
