/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h> 
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qthread.h>
#include <qpixmap.h>

#include "vfat_controller.h" // SW header

/** \brief Get complete address for panel's CCHIP
 * Get complete address for panel's CCHIP
 */
keyType vfat_controller::getCChipPosition(){

  keyType index = 0 , fecSlot = 0, ringSlot = 0, ccuAddress = 0, channel = 0 ;

  if ( fecSlots3->count() && ccuAddresses3->count() && i2cChannels3->count() && deviceAddresses3->count() ) {

    if ( sscanf (fecSlots3->currentText(), "%d,%d", &fecSlot, &ringSlot) &&
	 sscanf (ccuAddresses3->currentText(), "%x", &ccuAddress) &&
	 sscanf (i2cChannels3->currentText(), "%x", &channel) && 
	 sscanf (deviceAddresses3->currentText(), "%x", &index) ) {

      return (buildCompleteKey(fecSlot, ringSlot, ccuAddress, channel, index)) ;
    }
    else {  ErrorMessage ("The slot entered is not correct or empty", "Please scan FECs and CCUs before reading ID" ) ; }

  }
  else {  ErrorMessage ("The slot entered is not correct or empty", "Please scan FECs and CCUs before reading ID" ) ; }

  return ((keyType)0x00) ;
}

/** \brief Read the chip identification
 * Read the chip identification
 */
void vfat_controller::readCChipId(){

  if ( fecSlots3->count() && ccuAddresses3->count() && i2cChannels3->count() && deviceAddresses3->count() ) {

    try {

      totemCChipAccess *cchip = NULL ;

      cchip = (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, getCChipPosition()) ;

      std::stringstream chipid ;
	  	  
      chipid << "0x" << IntToHex((int)cchip->getChipid1()) << IntToHex((int)cchip->getChipid0()) ;
      cchipChipidT->setText (chipid.str()) ;
      chipid.flush() ;
      cchip = NULL ;    
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Unable to read chip id.", e) ; }
  }
}

/** \brief Read the values from the chip
 * Read the values from the chip
 */
void vfat_controller::readCChip_pressed(){

  if (deviceAddresses3->count() <= 0) {

    ErrorMessage ("No CCHIPs to be read.", "Please try to find them again.") ;
    writeCChipCompLed->setPixmap(redLed_) ;
    return ;
  }

  writeCChipCompareSel->setChecked(false) ;

  totemCChipAccess *cchip = NULL ;

  try{

    if (cchip != (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, getCChipPosition())){ cchip = (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, getCChipPosition()) ; }

    if (controlSel->isChecked()){
      setCChipCr0(cchip->getControl0()) ;
      setCChipCr1(cchip->getControl1()) ;
      setCChipCr2(cchip->getControl2()) ;
      setCChipCr3(cchip->getControl3()) ;
      setCChipCr4(cchip->getControl4()) ;
    }
    if (cchip_mask0Sel->isChecked()){
      tscType8 value = cchip->getMask0() ;
      setCChipMask(0, value) ; 
    }
    if (cchip_mask1Sel->isChecked()){
      tscType8 value = cchip->getMask1() ;
      setCChipMask(1, value) ; 
    }
    if (cchip_mask2Sel->isChecked()){
      tscType8 value = cchip->getMask2() ;
      setCChipMask(2, value) ; 
    }
    if (cchip_mask3Sel->isChecked()){
      tscType8 value = cchip->getMask3() ;
      setCChipMask(3, value) ; 
    }
    if (cchip_mask4Sel->isChecked()){
      tscType8 value = cchip->getMask4() ;
      setCChipMask(4, value) ; 
    }
    if (cchip_mask5Sel->isChecked()){
      tscType8 value = cchip->getMask5() ;
      setCChipMask(5, value) ; 
    }
    if (cchip_mask6Sel->isChecked()){
      tscType8 value = cchip->getMask6() ;
      setCChipMask(6, value) ; 
    }
    if (cchip_mask7Sel->isChecked()){
      tscType8 value = cchip->getMask7() ;
      setCChipMask(7, value) ; 
    }
    if (cchip_mask8Sel->isChecked()){
      tscType8 value = cchip->getMask8() ;
      setCChipMask(8, value) ; 
    }
    if (cchip_mask9Sel->isChecked()){
      tscType8 value = cchip->getMask9() ;
      setCChipMask(9, value) ; 
    }
    if (impedanceSel->isChecked()){
      tscType8 value = cchip->getRes() ;
      setCChipRes(value) ; 
    }
    if (counterSel->isChecked()){

      std::stringstream counter, counter0, counter1, counter2 ;

      counter0 << std::hex << (int)cchip->getCounter0() ;
      if(counter0.str().size()<=1){ counter << "0" << counter0.str() ; }
      else{ counter << counter0.str() ; }


      counter1 << std::hex << (int)cchip->getCounter1() ;
      if(counter1.str().size()<=1){ counter << "0" << counter1.str() ; }
      else{ counter << counter1.str() ; }

      counter2 << std::hex << (int)cchip->getCounter2() ;
      if(counter2.str().size()<=1){ counter << "0" << counter2.str() ; }
      else{ counter << counter2.str() ; }

      cchipCounterT->setText(IntToString(HexToInt(counter.str()))) ;
      counter.flush() ;
    }
  }
  catch (FecExceptionHandler &e) {

    ErrorMessage ("CCHIP access using given values could not be made.", e) ;
    readCChipCompLed->setPixmap(redLed_) ;
  }

  readCChipCompLed->setPixmap(greenLed_) ;
}

/** \brief Read the counter values from the chip
 * Read the counter values from the chip
 */
void vfat_controller::readCChipCounters_pressed() {

  if (deviceAddresses3->count() <= 0) {

    ErrorMessage ("No CCHIPs to be read.", "Please try to find them again.") ;
    writeCChipCompLed->setPixmap(redLed_) ;
    return ;
  }

  try{

    totemCChipAccess *cchip = NULL ;

    if (cchip != (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, getCChipPosition())){ cchip = (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, getCChipPosition()) ; }

    std::stringstream counter, counter0, counter1, counter2 ;

    counter0 << std::hex << (int)cchip->getCounter0() ;
    if(counter0.str().size()<=1){ counter << "0" << counter0.str() ; }
    else{ counter << counter0.str() ; }
    counter0.flush() ;

    counter1 << std::hex << (int)cchip->getCounter1() ;
    if(counter1.str().size()<=1){ counter << "0" << counter1.str() ; }
    else{ counter << counter1.str() ; }
    counter1.flush() ;

    counter2 << std::hex << (int)cchip->getCounter2() ;
    if(counter2.str().size()<=1){ counter << "0" << counter2.str() ; }
    else{ counter << counter2.str() ; }
    counter2.flush() ;

    cchipCounterT->setText(IntToString(HexToInt(counter.str()))) ;
    counter.flush() ;

  }
  catch (FecExceptionHandler &e) {

    ErrorMessage ("CCHIP access using given values could not be made.", e) ;
    readCChipCompLed->setPixmap(redLed_) ;
  }

  readCChipCompLed->setPixmap(greenLed_) ;   
}



/** \brief Write the values selected to the chip
 * Write the values selected to the chip
 */
void vfat_controller::writeCChip_pressed(){

  if (deviceAddresses3->count() <= 0) {

    ErrorMessage ("No CCHIPs to be written.", "Please try to find them again.") ;
    writeCChipCompLed->setPixmap(redLed_) ;
    return ;
  }

  int errors = 0 ;

  int cchipsToWrite = 1 ;
  if(writeAllCChipSel->isChecked()){ cchipsToWrite = cchipAddresses_.size() ; }

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "Write values to " << cchipsToWrite << " CCHIPs" << SET_DEFAULT_COLORS << std::endl ;
#endif

  totemCChipAccess *cchip = NULL ;

  try{

    for(int i=0;i<cchipsToWrite;i++){

      if(writeAllCChipSel->isChecked()){

	if (cchip != (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, cchipAddresses_.at(i))){
	  cchip = (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, cchipAddresses_.at(i)) ;
	}
      }
      else{

	if (cchip != (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, getCChipPosition())){
	  cchip = (totemCChipAccess *)fecAccessManager_->getAccess(CCHIP, getCChipPosition()) ;
	}
      }
 
      if (controlSel->isChecked()){

	cchip->setControl0(getCChipCr0()) ;
	cchip->setControl1(getCChipCr1()) ;
	cchip->setControl2(getCChipCr2()) ; 
	cchip->setControl3(getCChipCr3()) ; 
	cchip->setControl4(getCChipCr4()) ; 

	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipCr0() != cchip->getControl0()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CR0" << SET_DEFAULT_COLORS << std::endl ;
	  }
	  if( getCChipCr1() != cchip->getControl1()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CR1" << SET_DEFAULT_COLORS << std::endl ;
	  }
	  if( getCChipCr2() != cchip->getControl2()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CR2" << SET_DEFAULT_COLORS << std::endl ;
	  }
	  if( getCChipCr3() != cchip->getControl3()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CR3" << SET_DEFAULT_COLORS << std::endl ;
	  }
	  if( getCChipCr4() != cchip->getControl4()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in CR4" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask0Sel->isChecked()){
	cchip->setMask0(getCChipMask(0)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(0) != cchip->getMask0()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK0" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask1Sel->isChecked()){
	cchip->setMask1(getCChipMask(1)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(1) != cchip->getMask1()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK1" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask2Sel->isChecked()){
	cchip->setMask2(getCChipMask(2)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(2) != cchip->getMask2()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK2" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask3Sel->isChecked()){
	cchip->setMask3(getCChipMask(3)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(3) != cchip->getMask3()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK3" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask4Sel->isChecked()){
	cchip->setMask4(getCChipMask(4)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(4) != cchip->getMask4()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK4" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask5Sel->isChecked()){
	cchip->setMask5(getCChipMask(5)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(5) != cchip->getMask5()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK5" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask6Sel->isChecked()){
	cchip->setMask6(getCChipMask(6)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(6) != cchip->getMask6()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK6" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask7Sel->isChecked()){
	cchip->setMask7(getCChipMask(7)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(7) != cchip->getMask7()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK7" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask8Sel->isChecked()){
	cchip->setMask8(getCChipMask(8)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(8) != cchip->getMask8()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK8" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (cchip_mask9Sel->isChecked()){
	cchip->setMask9(getCChipMask(9)) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipMask(9) != cchip->getMask9()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in MASK9" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
      if (impedanceSel->isChecked()){
	cchip->setRes(getCChipRes()) ; 
	if (writeCChipCompareSel->isChecked()) {
	  if( getCChipRes() != cchip->getRes()){
	    errors+=1 ;
	    std::cout << SET_FORECOLOR_RED << "ERROR in IMPEDANCE" << SET_DEFAULT_COLORS << std::endl ;
	  }
	}
      }
    }
  }
  catch (FecExceptionHandler &e) {

    ErrorMessage ("CCHIP Access could not be made.", e) ;
    writeCChipCompLed->setPixmap(redLed_) ;
  }

  if(errors!=0){
    writeCChipCompLed->setPixmap(redLed_) ;
    std::cout << SET_FORECOLOR_RED << (int)errors << " error(s) occured during the write." << SET_DEFAULT_COLORS << std::endl ;
  }
  else { 
 
    if (writeAllCChipSel->isChecked()) { writeAllCChipSel->setChecked(false) ; }
    if (writeCChipCompareSel->isChecked()) { writeCChipCompLed->setPixmap(greenLed_) ; }
  }
}

void vfat_controller::scanAll2_pressed(){

  scanAll_pressed() ;
}
